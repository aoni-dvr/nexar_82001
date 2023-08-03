/**
 *  @file AmbaNFTL.c
 *
 *  @copyright Copyright (c) 2015 Ambarella, Inc.
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
 *  @details NAND flash translation layer APIs.
 *
 */
#include "AmbaDef.h"
#include "AmbaMisraFix.h"
#include "AmbaKAL.h"
#include "AmbaNAND.h"
#include "AmbaNAND_Ctrl.h"
#include "AmbaNFTL.h"
#include "AmbaNAND_BBM_Def.h"
//#include "AmbaRTSL_NAND_Ctrl.h"
//#include "AmbaRTSL_FIO.h"
#include "AmbaNAND_OP.h"

#define NG (-1)

#define WRITE_ALL_PUB

#define AMBA_NUM_NFTL_PARTITION (AMBA_NUM_USER_PARTITION - AMBA_USER_PARTITION_VIDEO_REC_INDEX)

#define TRL_TABLE_SIZE   (0x2000U)
static UINT16 g_LbaTrlTable[AMBA_NUM_NFTL_PARTITION * TRL_TABLE_SIZE] GNU_ALIGNED_CACHESAFE GNU_SECTION_NOZEROINIT;

/*
 * Descriptor for the partition objects
 */
typedef struct {
    AMBA_NFTL_OBJ_s   Nftl;
} PARTITION_OBJ;

/* Instance of the NFTL object for different partitions */
static PARTITION_OBJ g_PartObj[AMBA_NUM_NFTL_PARTITION] GNU_ALIGNED_CACHESAFE GNU_SECTION_NOZEROINIT;

static AMBA_KAL_MUTEX_t  Mutex[AMBA_NUM_NFTL_PARTITION] = {0};

static AMBA_NFTL_PART_INFO_s g_Nftl_Info[AMBA_NUM_NFTL_PARTITION] GNU_ALIGNED_CACHESAFE;

static AMBA_KAL_MUTEX_t NftlSecReadMutex = {0}; /* Mutex */

typedef struct {
    UINT32  InitNum;        /* Init number */
    UINT32  Valid;          /* Indicate if this imark is valid */
    UINT32  Signature;      /* imark signature */
    UINT32  Number;         /* imark signature */
} AMBA_NFTL_TABLE_CACHE_s;

static AMBA_NFTL_TABLE_CACHE_s NftlTblCache[AMBA_NUM_NFTL_PARTITION]
GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE;

/*
 *  @RoutineName:: UserPartID_To_NftlID
 *
 *  @Description::
 *
 *  @Input      ::
 *          UINT32 UserPartID    :
 *
 *  @Output     :: NftlID
 *
 *  @Return     ::
 */
static UINT32 UserPartID_To_NftlID(UINT32 UserPartID)
{
    return (UserPartID - AMBA_USER_PARTITION_VIDEO_REC_INDEX);
}

/*
 *  @RoutineName:: AmbaNandBBT_IsBadBlock
 *
 *  @Description:: Check the input block is bad or not
 *
 *  @Input      ::
 *          UINT32 Block    : Block to be checked
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : Good(0)/Bad(>0)
 */
static UINT32 AmbaNandBBT_IsBadBlock(UINT32 Block)
{
    return AmbaNAND_GetBlkMark(Block);
}

/*
 *  @RoutineName:: NftlGetTblCacheAddr
 *
 *  @Description:: Get the nand physical address for logic table cache
 *
 *  @Input      ::
 *          AMBA_NFTL_OBJ_s *pNftl: Pointer to NFTL object
 *          UINT32 *pBlock      : Pointer to block address
 *          UINT32 TblNumber    :
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
static UINT32 NftlGetTblCacheAddr(const AMBA_NFTL_OBJ_s *pNftl, UINT32 *pBlock, UINT32 TblNumber)
{
    UINT32 RetVal = 0;
    INT32 GoodBlk;
    UINT32 BlkAddr;
    const AMBA_NFTL_PART_INFO_s *pNftlInfo = &g_Nftl_Info[pNftl->ID];

    GoodBlk = AmbaNAND_FindGoodBlkForward(pNftlInfo->StartTrlTbl + TblNumber);
    if (GoodBlk < 0) {
        RetVal = NAND_ERR_BAD_BLOCK_OVER;
    } else {
        AmbaMisra_TypeCast32(&BlkAddr, &GoodBlk);
        if (BlkAddr >= (pNftlInfo->StartTrlTbl + pNftlInfo->TrlBlocks)) {
            RetVal = NAND_ERR_BAD_BLOCK_OVER;
        } else {
            *pBlock = BlkAddr;
        }
    }
    return RetVal;

}

/*
 *  @RoutineName:: NftlReadTbLCache
 *
 *  @Description:: Read the logic table cache
 *
 *  @Input      ::
 *          AMBA_NFTL_OBJ_s *pNftl: Pointer to NFTL object
 *          UINT32 IMark        : IMark address which is the same as tchunk address
 *          UINT8 *pBuf         : Data buffer
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
static UINT32 NftlReadTbLCache(const AMBA_NFTL_OBJ_s *pNftl, UINT8 *pBuf, UINT32 TblNumber, UINT32 ReadTag)
{
    UINT32 Block, PageAddr, Pages;
    UINT32 RetStatus = 0;
    const AMBA_NAND_DEV_INFO_s *pDev = AmbaNAND_GetCommonInfo();

    (void)NftlGetTblCacheAddr(pNftl, &Block, TblNumber);

    PageAddr = (Block * pDev->BlockPageSize);
    Pages    = GetRoundUpValU32((pNftl->TotalBlks * sizeof(UINT16)), pDev->MainByteSize);

    if (ReadTag == 0U) {
        RetStatus = AmbaNandOp_Read(PageAddr, Pages, pBuf, NULL, 5000U);
    } else {
        /* Tag Store in the last page of cahce table */
        RetStatus = AmbaNandOp_Read(PageAddr + Pages, 1U, pBuf, NULL, 5000U);
    }

    return RetStatus;
}

/*
 *  @RoutineName:: NftlWriteTblCache
 *
 *  @Description:: Write the Table cache
 *
 *  @Input      ::
 *          AMBA_NFTL_OBJ_s *pNftl: Pointer to NFTL object
 *          UINT32 IMark        : IMark address which is the same as tchunk address
 *          UINT8 *pBuf         : Data buffer
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
static UINT32 NftlWriteTblCache(const AMBA_NFTL_OBJ_s *pNftl, UINT32 TblNumber)
{
    const AMBA_NAND_DEV_INFO_s *pDev = AmbaNAND_GetCommonInfo();
    UINT32 Block, Pages;
    UINT32 RetStatus;

    RetStatus = NftlGetTblCacheAddr(pNftl, &Block, TblNumber);
    if (RetStatus == OK) {

        Pages    = GetRoundUpValU32((pNftl->TotalBlks * sizeof(UINT16)), pDev->MainByteSize);
        RetStatus = AmbaNandOp_Program(Block * pDev->BlockPageSize, Pages, (UINT8 *)pNftl->pTrlTable, NULL, 5000U);

        if (RetStatus == OK) {
            RetStatus = AmbaNandOp_Program((Block * pDev->BlockPageSize) + Pages, 1U, (UINT8 *)&NftlTblCache[pNftl->ID], NULL, 5000U);
        }
    }
    return RetStatus;
}

/*
 *  @RoutineName:: NftlIsValidTblCache
 *
 *  @Description:: To see if it is the valid tchunk.
 *
 *  @Input      ::
 *          AMBA_NFTL_INIT_MARK_s *pIMark : IMark buffer
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : Valid(1)/Invalid(0)
 */
static UINT32 NftlIsValidTblCache(const AMBA_NFTL_TABLE_CACHE_s *pTblCache)
{
    UINT32 RetStatus = 0;
    if ((pTblCache->Signature == NFTL_IMARK_MAGIC) && (pTblCache->Valid == NFTL_IMARK_VALID)) {
        RetStatus = 1U;
    }
    return RetStatus;
}

/*
 *  @RoutineName:: NftlInvalidTblCache
 *
 *  @Description:: Invalidate a logic table cache
 *
 *  @Input      ::
 *          AMBA_NFTL_OBJ_s *pNftl: Pointer to NFTL object
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
static UINT32 NftlInvalidTblCache(const AMBA_NFTL_OBJ_s *pNftl)
{
    UINT32 Block;
    UINT32 RetStatus;
    const AMBA_NFTL_TABLE_CACHE_s *pNftlTblCache = &NftlTblCache[pNftl->ID];

    RetStatus = NftlGetTblCacheAddr(pNftl, &Block, pNftlTblCache->Number);
    if (RetStatus == OK) {
        RetStatus = AmbaNandOp_EraseBlock(Block, 5000U);
        //DBGMSG("%s erase block : %d", __func__, Block);
    }

    return RetStatus;
}

/*
 *  @RoutineName:: NftlValidTblCache
 *
 *  @Description:: Validate a logic table cache.
 *
 *  @Input      ::
 *          AMBA_NFTL_OBJ_s *pNftl: Pointer to NFTL object
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
static UINT32 NftlValidTblCache(const AMBA_NFTL_OBJ_s *pNftl)
{
    UINT32 RetStatus;
    UINT32 Block;

    AMBA_NFTL_TABLE_CACHE_s *pNftlTblCache = &NftlTblCache[pNftl->ID];

    if (NftlIsValidTblCache(pNftlTblCache) == 1U) {
        pNftlTblCache->Number ++;
    } else {
        /* Init 1st Table cache */
        pNftlTblCache->InitNum     = NFTL_IMARK_SRL;
        pNftlTblCache->Valid       = NFTL_IMARK_VALID;
        pNftlTblCache->Signature   = NFTL_IMARK_MAGIC;
    }

    RetStatus = NftlGetTblCacheAddr(pNftl, &Block, pNftlTblCache->Number);
    if (RetStatus != OK) {
        pNftlTblCache->Number = 0;
        RetStatus = NftlGetTblCacheAddr(pNftl, &Block, pNftlTblCache->Number);
    }

    //DBGMSG("%s write block : %d", __func__, Block);

    if (RetStatus == OK) {
        RetStatus = NftlWriteTblCache(pNftl, pNftlTblCache->Number);
    }

    return RetStatus;
}

/*
 *  @RoutineName:: NftlCreateSecsMask64
 *
 *  @Description:: Create the sector bit pattern.
 *
 *  @Input      ::
 *          UINT32 Sec  : Start bit address in 64 bit mask
 *          UINT32 Secs : Length of the bit mask
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          UINT32  : Mask
 */
static UINT32 NftlCreateSecsMask64(UINT32 Sec, UINT32 Secs)
{
    UINT32 n, Mask;

    //Assert(Sec + Secs <= 64U);

    n = GetRoundUpValU32(Secs, NAND_SPP_8);

    Mask = ((UINT32)0x1U << (n * 4U)) - 1U;
    //Mask = 0x1U;
    //Mask = (Mask << (n * 4U)) - 1U;
    Mask = Mask << (GetRoundDownValU32(Sec, NAND_SPP_8) * 4U);

    return Mask;
}

/*
 *  @RoutineName:: NftlCreateSecsMask32
 *
 *  @Description:: Create the sector bit pattern.
 *
 *  @Input      ::
 *          UINT32 Sec  : Start bit address in 32 bit mask
 *          UINT32 Secs : Length of the bit mask
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          UINT32  : Mask
 */
static UINT32 NftlCreateSecsMask32(UINT32 Sec, UINT32 Secs)
{
    UINT32 Mask;

    if (!((Sec + Secs) <= 32U)) {
        Mask = 0xffffffffU; /* Mask = -1 */
    } else {
        Mask = 0x1U;
        Mask = (Mask << Secs) - 1U;
        Mask = Mask << Sec;
    }

    return Mask;
}

static void NftlCreateSecsMask4k(UINT32 *pPat, UINT32 Sec, UINT32 Secs)
{
    UINT32 SPart, Part, EPart, Len;
    UINT32 PatIdx = 0U;
    UINT32 i, n;
    UINT32 SecTmp = Sec;

    SPart = 0;
    Part  = 0;
    EPart = 0;
    Len = MAX_BIT_PAT_32;

    if (Sec >= 64U) {
        n = Sec / 64U;
        for (i = 0; i < n; i++) {
            pPat[PatIdx] = 0U;
            PatIdx++;
        }

        SecTmp = SecTmp - (n * 64U);
        Len -= n;
    }

    if ((SecTmp + Secs) <= 64U) {
        SPart = Secs;
    } else {
        SPart = 64U - SecTmp;
    }

    Secs -= SPart;
    for (i = 0; i < Secs; i += 64U) {
        if ((Secs - i) >= 64U) {
            Part++;
        } else {
            EPart = Secs - i;
        }
    }

    pPat[PatIdx] = NftlCreateSecsMask64(SecTmp, SPart);
    PatIdx ++;

    for (i = 0; i < Part; i++) {
        pPat[PatIdx] = 0xffffffffU;
        PatIdx ++;
    }

    Len -= (Part + 1U);
    if (Len > 0U) {
        pPat[PatIdx] = NftlCreateSecsMask64(0U, EPart);
        PatIdx ++;
        Len--;
        for (i = 0; i < Len; i++) {
            pPat[PatIdx] = 0U;
            PatIdx ++;
        }
    }
}

/*
 *  @RoutineName:: NftlCreateSecsMask
 *
 *  @Description:: Create the sector bit pattern.
 *                 For 512 bytes page nand flash, the sector bit mask is 32 bit per block.
 *                 For 2k page nand flash, the sector bit mask is 256 bit per block.
 *
 *  @Input      ::
 *          AMBA_NFTL_OBJ_s *pNftl: Pointer to NFTL object
 *          UINT32 *pPat        : Pointer to bit mask
 *          UINT32 Sec          : Start bit address in 32 bit mask
 *          UINT32 Secs         : Length of the bit mask
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          UINT32  : Mask
 */
static void NftlCreateSecsMask(const AMBA_NFTL_OBJ_s *pNftl, UINT32 *pPat, UINT32 Sec, UINT32 Secs)
{
    UINT32 SPart, Part, EPart, Len;
    UINT32 i, n;
    UINT32 SecTmp = Sec;

    if (!((Sec + Secs) <= g_Nftl_Info[pNftl->ID].SectorsPerBlock)) {
        /*  */
    } else if (pNftl->PageType == NAND_TYPE_2K) {
        UINT32 PatIdx = 0U;

        SPart = 0U;
        Part  = 0U;
        EPart = 0U;
        Len = MAX_BIT_PAT_32;

        if (SecTmp >= 32U) {
            n = SecTmp >> 5U;
            for (i = 0; i < n; i++) {
                pPat[PatIdx] = 0U;
                PatIdx++;
            }

            SecTmp = SecTmp - (n << 5U);
            Len -= n;
        }

        if ((SecTmp + Secs) <= 32U) {
            SPart = Secs;
        } else {
            SPart = 32U - SecTmp;
        }

        Secs -= SPart;
        for (i = 0; i < Secs; i += 32U) {
            if ((Secs - i) >= 32U) {
                Part++;
            } else {
                EPart = Secs - i;
            }
        }

        pPat[PatIdx] = NftlCreateSecsMask32(SecTmp, SPart);
        PatIdx ++;

        for (i = 0; i < Part; i++) {
            pPat[PatIdx] = 0xffffffffU;
            PatIdx ++;
        }

        Len -= (Part + 1U);
        if (Len > 0U) {
            pPat[PatIdx] = NftlCreateSecsMask32(0U, EPart);
            PatIdx ++;
            Len--;
            for (i = 0; i < Len; i++) {
                pPat[PatIdx] = 0U;
                PatIdx ++;
            }
        }
    } else if (pNftl->PageType == NAND_TYPE_4K) {
        NftlCreateSecsMask4k(pPat, Sec, Secs);
    } else {
        /* For Misra C checking. */
    }
}

/*
 *  @RoutineName:: NftlUpdateBBTable
 *
 *  @Description:: Update bad block table.
 *
 *  @Input      ::
 *          AMBA_NFTL_OBJ_s *pNftl: Pointer to NFTL object
 *          UINT32 Pba          : Physical block address
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
 */
static void NftlUpdateBBTable(const AMBA_NFTL_OBJ_s *pNftl, UINT32 Pba)
{
    if (AmbaNAND_SetBlkMark(pNftl->StartBlkPart + Pba, AMBA_NAND_BLK_RUNTIME_BAD, 5000) != OK) {
        //AmbaPrint("mark bad block failed. <block %d>...", Pba);
    }

    if (OK != AmbaNAND_WriteBBT(5000)) {
        //AmbaPrint("AmbaNAND_UpdateBBT FAIL");
    }
}

/*
 *  @RoutineName:: NftlUpdateBBInfo
 *
 *  @Description:: Update bad block information.
 *
 *  @Input      ::
 *          AMBA_NFTL_OBJ_s *pNftl: Pointer to NFTL object
 *          UINT32 Pba          : Physical block address
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
static void NftlUpdateBBInfo(AMBA_NFTL_OBJ_s *pNftl, UINT32 Pba)
{
    pNftl->pBBInfo[pNftl->NumBB] = Pba;
    pNftl->NumBB++;

    if (pNftl->NumBB >= NFTL_BB_INFO_SIZE) {
        //(void)AmbaPrint("%s: dranger! too many bad blocks!", g_NftlPart[pNftl->ID]);
    }
}

/*
 *  @RoutineName:: NftlUpdateLogicTable
 *
 *  @Description:: Update the logical to physical address translation table
 *
 *  @Input      ::
 *          AMBA_NFTL_OBJ_s *pNftl: Pointer to NFTL object
 *          UINT32 Lba          : Absolutely logical block address
 *          UINT32 Pba          : Absolutely physical block address
 *
 *  @Output     :: none
 *
 *  @Return     :: none
 */
static void NftlUpdateLogicTable(const AMBA_NFTL_OBJ_s *pNftl, UINT32 Lba, UINT32 Pba)
{
    UINT16 *pTrlTable = pNftl->pTrlTable;

    if (Pba == NFTL_UNUSED_BLK) {
        pTrlTable[Lba] = NFTL_UNUSED_BLK;
    } else {
        if (!((Pba / pNftl->PBlksPerZone) == (Lba / pNftl->LBlksPerZone))) {
            /* For Miscra C checking */
        } else {
            UINT16 TmpPba = 0;
            Pba %= pNftl->PBlksPerZone;
            (void)AmbaWrap_memcpy(&TmpPba, &Pba, sizeof(UINT16));
            pTrlTable[Lba] = TmpPba;
        }
    }

    //DBGMSG("%s: %s (zone %d, lba %d, pba %d)", g_NftlPart[pNftl->ID], __FUNCTION__,
    //           Lba / pNftl->LBlksPerZone, Lba % pNftl->LBlksPerZone, Pba);
}

/*
 *  @RoutineName:: NftlUpdatePhyTable
 *
 *  @Description:: Update the physical to logical address translation table
 *
 *  @Input      ::
 *          AMBA_NFTL_OBJ_s *pNftl: Pointer to NFTL object
 *          UINT32 Pba          : Absolutely physical block address
 *          UINT32 Lba          : Absolutely logical block address
 *
 *  @Output     :: none
 *
 *  @Return     :: none
 */
static void NftlUpdatePhyTable(const AMBA_NFTL_OBJ_s *pNftl, UINT32 Pba, UINT32 Lba)
{
    UINT16 *pTrlTable  = pNftl->pPhyTable;
    UINT16 TmpLba = 0;

    if (Lba == NFTL_UNUSED_BLK) {
        (void)AmbaWrap_memcpy(&TmpLba, &Lba, sizeof(UINT16));
        pTrlTable[Pba] = TmpLba;
    } else {
        if (!((Pba / pNftl->PBlksPerZone) == (Lba / pNftl->LBlksPerZone))) {
            /* For Misra C checking */
        } else {

            Lba %= pNftl->LBlksPerZone;
            (void)AmbaWrap_memcpy(&TmpLba, &Lba, sizeof(UINT16));
            pTrlTable[Pba] = TmpLba;
        }
    }
    //DBGMSG("%s: %s (zone %d, pba %d, lba %d)", g_NftlPart[pNftl->ID], __FUNCTION__,
    //           Pba / pNftl->PBlksPerZone, Pba % pNftl->PBlksPerZone, Lba);
}

/*
 *  @RoutineName:: NftlGetUsedSecPat
 *
 *  @Description:: Get the used sector bit pattern from spare area to know which
 *                  sectors are used in the block.
 *
 *  @Input      ::
 *          AMBA_NFTL_OBJ_s *pNftl: Pointer to NFTL object
 *          UINT32 Pba          : Physical block address
 *          UINT32 *pPat        : Pointer to the used sector bit pattern
 *          UINT8 *pSpareBuf    : Pointer to spare area data
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
static UINT32 NftlGetUsedSecPat(AMBA_NFTL_OBJ_s *pNftl, UINT32 Pba, UINT32 *pPat, UINT8 *pSpareBuf)
{
    const AMBA_NAND_DEV_INFO_s *pDev = AmbaNAND_GetCommonInfo();
    const NAND_SPARE_LAYOUT_LARGE_s *pLargeSpr;
    UINT32 Tmp;
    UINT32 i, Page, Pages, PagePos, PageAddr;
    UINT32 RetStatus = 0;
    UINT32 SpareOffset;

    if (pDev->SpareByteSize == NAND_SPARE_128) {
        SpareOffset = 8U;
    } else if (pDev->SpareByteSize == NAND_SPARE_256) {
        SpareOffset = 16U;
    } else {
        SpareOffset = 4U;
    }

    /* Used sector pattern is stored in each page's  spare area */
    /* All pages should be read.*/
    Page = 0U;

    if (AmbaNandBBT_IsBadBlock(pNftl->StartBlkPart + Pba) != AMBA_NAND_BLK_GOOD) {
        /* Bad block, set used and return */
        (void)AmbaWrap_memset(pPat, 0xff, MAX_BIT_PAT_32 * sizeof(UINT32));
        RetStatus = OK;
    } else {
        Pages = BAD_BLOCK_PAGES;
        PageAddr = ((pNftl->StartBlkPart + Pba) * pDev->BlockPageSize);
        (void) AmbaNandOp_Read(PageAddr, Pages, NULL, pSpareBuf, 5000U);

        (void)AmbaWrap_memset(pPat, 0x0, MAX_BIT_PAT_32 * sizeof(UINT32));

        /* Handle page 0 ~ BAD_BLOCK_PAGES. */
        AmbaMisra_TypeCast32(&pLargeSpr, &pSpareBuf);
        //pLargeSpr = (NAND_SPARE_LAYOUT_LARGE_s*) pSpareBuf;

        for (i = 0; i < Pages; i++) {
            /* Truncate the 4 MSBs (1 page = 4 bits). */
            Tmp = pLargeSpr[i * SpareOffset].Pub;
            Tmp = ~(Tmp & 0xfU) & 0xfU;
            Tmp = (Tmp << (4U * i));
            *pPat |= Tmp;
        }

        Page = Pages;
        PageAddr  = ((pNftl->StartBlkPart + Pba) * pDev->BlockPageSize) + Page;
        RetStatus = AmbaNandOp_Read(PageAddr, pDev->BlockPageSize - Pages, NULL,
                                    &pNftl->pSpareBuf[Page * pDev->SpareByteSize], 5000U);
        if (RetStatus != OK) {
            /* Bypass Report ECC error */
            if (RetStatus == NAND_ERR_ECC) {
                RetStatus = 0;
            }
        }
        if (RetStatus != OK) {
            NftlUpdateBBInfo(pNftl, Pba);
            NftlUpdateBBTable(pNftl, Pba);
            pNftl->BBM.FailRdCnt++;
        } else {
            UINT32 PatIdx = 0U, Index = 0U;
            const UINT8 *pTmpPtr = &pNftl->pSpareBuf[Page * pDev->SpareByteSize];
            AmbaMisra_TypeCast32(&pLargeSpr, &pTmpPtr);

            for (Page = Pages; Page < pDev->BlockPageSize; Page += Pages) {

                /* Adjust */
                if ((Pages + Page) >= pDev->BlockPageSize) {
                    Pages = pDev->BlockPageSize - Page;
                }

                /* u32 = 8 pages */
                PagePos = Page % 8U;

                for (i = PagePos; i < (PagePos + Pages); i++) {
                    /* Truncate the 4 MSBs (1 page = 4 bits). */
                    //Tmp = ~(pLargeSpr[Index].Pub) & 0xfU;
                    Tmp = pLargeSpr[Index].Pub;
                    Tmp = ~(Tmp) & 0xfU;
                    pPat[PatIdx] |= (Tmp << (4U * i));
                    Index += SpareOffset;
                }

                if (((Page + Pages) % 8U) == 0U) {
                    PatIdx++;
                }
            }
        }
    }

    return RetStatus;
}

static void NftlWriteUsedSecPatSetPub(UINT8 *pPageUsp, UINT32 Page, UINT32 SpareOffset, NAND_SPARE_LAYOUT_LARGE_s *pLargeSpr, const UINT32 *pOldPat)
{
    if (pOldPat == NULL) {
        if ((*pPageUsp) > 0U) {
            pLargeSpr[Page * SpareOffset].Pub = 0x0;
        } else {
            pLargeSpr[Page * SpareOffset].Pub = 0xffU;
        }
    } else {
        UINT8 OldPageUsp;
        const UINT8 *pOldPatChar = NULL;

        AmbaMisra_TypeCast32(&pOldPatChar, &pOldPat);

        if ((Page % 2U) != 0U) {
            OldPageUsp = pOldPatChar[Page >> 1U] >> 4U;
        } else {
            OldPageUsp = pOldPatChar[Page >> 1U] & 0xfU;
        }
        (*pPageUsp) |= OldPageUsp;

        if ((*pPageUsp) > 0U) {
            pLargeSpr[Page * SpareOffset].Pub = 0x0;
        } else {
            pLargeSpr[Page * SpareOffset].Pub = 0xffU;
        }
    }
}

static UINT8 GetPageUsp2kPage(UINT32 Page, const UINT32 *pPat)
{
    const UINT8 *pPatChar;
    UINT8 PageUsp;

    AmbaMisra_TypeCast32(&pPatChar, &pPat);

    if ((Page % 2U) != 0U) {
        PageUsp = pPatChar[Page >> 1U] >> 4U;
    } else {
        PageUsp = pPatChar[Page >> 1U] & 0xfU;
    }

    return PageUsp;
}

static void CheckInSinglePage(UINT32 Sec, UINT32 Secs, UINT32 *pPat)
{
    if ((Secs == 0U) && (Sec > 3U)) {
        pPat[0] = 0xfU;
    }
}

static UINT32 NftlWriteUsedSecPat2kPage(AMBA_NFTL_OBJ_s *pNftl, UINT32 Pba, UINT32 Lba, UINT32 Sec,
                                        UINT32 Secs, const UINT32 *pOldPat, UINT32 SpareOffset)
{
    const AMBA_NAND_DEV_INFO_s *pDev = AmbaNAND_GetCommonInfo();
    NAND_SPARE_LAYOUT_LARGE_s *pLargeSpr = NULL;
    UINT32 PatBuf[MAX_BIT_PAT_32], NandPat[MAX_BIT_PAT_32];
    UINT8  Prog[64];
    UINT32 *pPat, Tmp;
    UINT32 RetStatus;
    UINT32 i;
    UINT32 Page, StartPage;

    pPat = PatBuf;

    /* 1 byte = 2 pages, u32 = 8 pages, 1 block = 32 bytes  */
    /* If sec = 37, secs = 1. Set sec < 36 all used!        */
    NftlCreateSecsMask(pNftl, pPat, Sec, Secs);

    CheckInSinglePage(Sec, Secs, pPat);

    Tmp = (Sec + Secs) >> 5U;
    /* Set all pages used before the current page. */
    for (i = 0; i < Tmp; i++) {
        pPat[i] = 0xffffffffU;
    }

    /* Set all pages used before current page in the same u32. */
    if (i != MAX_BIT_PAT_32) {
        UINT64 Pat64;
        Tmp = 0;
        while (pPat[i] != 0U) {
            Tmp++;
            pPat[i] >>= 4U;
        }
        Pat64 = ((UINT64) 0x1U << (4U * Tmp)) - 1U;
        AmbaMisra_TypeCast32(&pPat[i], &Pat64);
    }

    /* Get the original usp in nand flash. */
    RetStatus = NftlGetUsedSecPat(pNftl, Pba, NandPat, pNftl->pSpareBuf);
    (void)AmbaWrap_memset(Prog, 0x0, pDev->BlockPageSize);

    AmbaMisra_TypeCast32(&pLargeSpr, &pNftl->pSpareBuf);

    /* Check the first page's lba. */
    if ((pLargeSpr->Lsn[0] != 0xffU) || (pLargeSpr->Lsn[1] != 0xffU) || (Sec < 4U)) {
        Prog[0] = 1U;
    }

    /* Mark the page which has been programmed. */
    for (i = 1U; i < pDev->BlockPageSize; i++) {
        UINT32 Mask, Offset;
        Offset = i % 8U;
        /* mask of page located */
        Mask = (UINT32)0xfU << (Offset << 2U);
        if ((NandPat[i >> 3U] & Mask) != 0U) {
            Prog[i] = 1U;
        }
    }

    (void)AmbaWrap_memset(pNftl->pSpareBuf, 0xff, pDev->SpareByteSize * pDev->BlockPageSize);

    for (Page = 0; Page < pDev->BlockPageSize; Page++) {
        if (Prog[Page] == 0U) {
            break;
        }
    }
    StartPage = Page;

    for (Page = 0; Page < pDev->BlockPageSize; Page++) {
        UINT8 PageUsp;
        /* The page has been programmed! */
        if (Prog[Page] == 1U) {
            continue;
        }

        PageUsp = GetPageUsp2kPage(Page, pPat);

        AmbaMisra_TypeCast32(&pLargeSpr, &pNftl->pSpareBuf); /* pLargeSpr = (NAND_SPARE_LAYOUT_LARGE_s*) pNftl->pSpareBuf; */

        NftlWriteUsedSecPatSetPub(&PageUsp, Page, SpareOffset, pLargeSpr, pOldPat);

        if (Page == 0U) {
            pLargeSpr[Page * SpareOffset].Lsn[0] = (UINT8) Lba;
            pLargeSpr[Page * SpareOffset].Lsn[1] = (UINT8) ((Lba & 0xff00U) >> 8U);
        }

        /* The pages after this page are not changed! */
        if (pLargeSpr[Page * SpareOffset].Pub == 0xffU) {
            break;
        }
    }

    if ((Page - StartPage) != 0U) {
        UINT32 PageAddr  = ((pNftl->StartBlkPart + Pba) * pDev->BlockPageSize) + StartPage;
        RetStatus = AmbaNandOp_Program(PageAddr, Page - StartPage, NULL,
                                       &pNftl->pSpareBuf[StartPage * pDev->SpareByteSize], 5000U);
        if (RetStatus != NAND_ERR_NONE) {
            RetStatus = NAND_ERR_IO_FAIL;
            NftlUpdateBBInfo(pNftl, Pba);
            NftlUpdateBBTable(pNftl, Pba);
            pNftl->BBM.FailWrCnt++;
        }

    }
    return RetStatus;
}

static UINT32 NftlWriteUsedSecPat4kPage(AMBA_NFTL_OBJ_s *pNftl, UINT32 Pba, UINT32 Lba, UINT32 Sec,
                                        UINT32 Secs, const UINT32 *pOldPat, UINT32 SpareOffset)
{
    const AMBA_NAND_DEV_INFO_s *pDev = AmbaNAND_GetCommonInfo();
    NAND_SPARE_LAYOUT_LARGE_s *pLargeSpr = NULL;
    UINT32 PatBuf[MAX_BIT_PAT_32], NandPat[MAX_BIT_PAT_32];
    UINT32 Offset, Mask, PageAddr;
    UINT8  PageUsp;
    UINT32 *pPat, Tmp;
    UINT32 RetStatus;
    UINT32 i;
    UINT32 Page, StartPage;
    UINT8  Prog[64];

    //AmbaMisra_TouchUnused(pOldPat);

    pPat = PatBuf;

    /* 1 byte = 8 pages, u32 = 8 pages, 1 block = 64 bytes  */
    /* If sec = 37, secs = 1. Set sec < 40 all used!        */

    NftlCreateSecsMask(pNftl, pPat, Sec, Secs);

    if ((Secs == 0U) && (Sec > 7U)) {
        pPat[0] = 0xfU;
    }

    Tmp = (Sec + Secs) >> 6U;
    /* Set all pages used before the current page. */
    for (i = 0; i < Tmp; i++) {
        pPat[i] = 0xffffffffU;
    }

    /* Get the original usp in nand flash. */
    RetStatus = NftlGetUsedSecPat(pNftl, Pba, NandPat, pNftl->pSpareBuf);
    (void)AmbaWrap_memset(Prog, 0x0, pDev->BlockPageSize);

    AmbaMisra_TypeCast32(&pLargeSpr, &pNftl->pSpareBuf); /* pLargeSpr = (NAND_SPARE_LAYOUT_LARGE_s *) pNftl->pSpareBuf; */

    /* Check the first page's lba. */
    if ((pLargeSpr->Lsn[0] != 0xffU) || (pLargeSpr->Lsn[1] != 0xffU) || (Sec < 4U)) {
        Prog[0] = 1U;
    }

    /* Mark the page which has been programmed. */
    for (i = 1U; i < pDev->BlockPageSize; i++) {
        Offset = i % 8U;
        /* mask of page located */
        Mask = (UINT32)0xfU << (Offset << 2U);
        if ((NandPat[i >> 3U] & Mask) != 0U) {
            Prog[i] = 1U;
        }
    }

    (void)AmbaWrap_memset(pNftl->pSpareBuf, 0xff, pDev->SpareByteSize * pDev->BlockPageSize);

    for (Page = 0; Page < pDev->BlockPageSize; Page++) {
        if (Prog[Page] == 0U) {
            break;
        }
    }
    StartPage = Page;

    for (Page = 0; Page < pDev->BlockPageSize; Page++) {
        const UINT8 *pPatChar;

        /* The page has been programmed! */
        if (Prog[Page] == 1U) {
            continue;
        }

        pPatChar = (UINT8 *) PatBuf;

        if ((Page % 2U) != 0U) {
            PageUsp = pPatChar[Page >> 1U] >> 4U;
        } else {
            PageUsp = pPatChar[Page >> 1U] & 0xfU;
        }

        NftlWriteUsedSecPatSetPub(&PageUsp, Page, SpareOffset, pLargeSpr, pOldPat);

        if (Page == 0U) {
            pLargeSpr->Lsn[0] = (UINT8) Lba;
            pLargeSpr->Lsn[1] = (UINT8) ((Lba & 0xff00U) >> 8U);
        }

        /* The pages after this page are not changed! */
        if (pLargeSpr[Page * SpareOffset].Pub == 0xffU) {
            break;
        }
    }

    if ((Page - StartPage) != 0U) {
        PageAddr  = ((pNftl->StartBlkPart + Pba) * pDev->BlockPageSize) + StartPage;
        RetStatus = AmbaNandOp_Program(PageAddr, Page - StartPage, NULL,
                                       &pNftl->pSpareBuf[StartPage * pDev->SpareByteSize], 5000);
        if (RetStatus != NAND_ERR_NONE) {
            RetStatus = NAND_ERR_IO_FAIL;
            NftlUpdateBBInfo(pNftl, Pba);
            NftlUpdateBBTable(pNftl, Pba);
            pNftl->BBM.FailWrCnt++;
        }
    }
    return RetStatus;
}

/*
 *  @RoutineName:: NftlWriteUsedSecPat
 *
 *  @Description:: Write used sectors bit pattern in spare area.
 *
 *  @Input      ::
 *          AMBA_NFTL_OBJ_s *pNftl: Pointer to NFTL object
 *          UINT32 Pba          : Physical block address
 *          UINT32 Lba          : Logical block address
 *          UINT32 Sec          : Sector address in the block
 *          UINT32 Secs         : Number of sectors to be writen
 *          UINT32 *pOldPat     : Pointer to used sectors bit pattern corresponding to previous block
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
static UINT32 NftlWriteUsedSecPat(AMBA_NFTL_OBJ_s *pNftl, UINT32 Pba, UINT32 Lba, UINT32 Sec,
                                  UINT32 Secs, UINT32 *pOldPat)
{
    const AMBA_NAND_DEV_INFO_s *pDev = AmbaNAND_GetCommonInfo();
    UINT32  RetStatus = 0;
    UINT32 SpareOffset;

    AmbaMisra_TouchUnused(pOldPat);

    Lba %= pNftl->LBlksPerZone;

    if (pDev->SpareByteSize == NAND_SPARE_128) {
        SpareOffset = 8U;
    } else if (pDev->SpareByteSize == NAND_SPARE_256) {
        SpareOffset = 16U;
    } else {
        SpareOffset = 4U;
    }

    if (pNftl->PageType == NAND_TYPE_2K) {
        RetStatus = NftlWriteUsedSecPat2kPage(pNftl, Pba, Lba, Sec, Secs, pOldPat, SpareOffset);
    } else if (pNftl->PageType == NAND_TYPE_4K) {
        RetStatus = NftlWriteUsedSecPat4kPage(pNftl, Pba, Lba, Sec, Secs, pOldPat, SpareOffset);
    } else {
        //K_ASSERT(0);
    }

    return RetStatus;
}

static INT32 NftlCheckUsedSecs2kPage(const UINT32 *pPat, UINT32 Sec, UINT32 Secs)
{
    UINT32 Mask, n;
    UINT32 i, PatIdx = 0U;
    INT32 RetStatus = 0;
    UINT32 TmpSec = Sec, TmpSecs = Secs;

    /* check the first 32 bit patern */
    if (TmpSec >= 32U) {
        n = TmpSec >> 5U;
        PatIdx += n;
        TmpSec = TmpSec - (n << 5U);
    }

    if ((TmpSec + TmpSecs) <= 32U) {
        n = TmpSecs;
    } else {
        n = 32U - TmpSec;
    }
#if 1
    if (((Sec + n) % 4U) != 0U) {
        n += (4U - ((Sec + n) % 4U));
    }
#endif
    Mask = NftlCreateSecsMask32(TmpSec, n);

    if ((pPat[PatIdx] & Mask) > 0U) {
        RetStatus = 1;
    } else {
        PatIdx ++;
        TmpSecs -= n;

        /* check the remain bit patern */
        for (i = 0; i < TmpSecs; i += 32U) {
            if ((TmpSecs - i) >= 32U) {
                Mask = 0xffffffffU;
            } else {
                n = TmpSecs - i;
                Mask = NftlCreateSecsMask32(0, n);
            }

            if ((pPat[PatIdx] & Mask) > 0U) {
                RetStatus = 1;
                break;
            }
            PatIdx ++;
        }
    }
    return RetStatus;
}

/*
 *  @RoutineName:: NftlCheckUsedSecs
 *
 *  @Description:: Check if any used sectors in the block according to the used sector bit pattern.
 *
 *  @Input      ::
 *          AMBA_NFTL_OBJ_s *pNftl: Pointer to NFTL object
 *          UINT32 *pPat        : Pointer to the used sector bit pattern
 *          UINT32 Sec          : Starting sector number in the block
 *          UINT32 Secs         : Number of sectors to be check
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : unused (0) / used (1)
 */
static INT32 NftlCheckUsedSecs(const AMBA_NFTL_OBJ_s *pNftl, UINT32 *pPat, UINT32 Sec, UINT32 Secs)
{
    UINT32 Mask, n;
    UINT32 i, PatIdx = 0U;
    INT32 RetStatus = 0;
    UINT32 TmpSec = Sec, TmpSecs = Secs;

    AmbaMisra_TouchUnused(pPat);

    if (!((TmpSec + TmpSecs) <= g_Nftl_Info[pNftl->ID].SectorsPerBlock)) {
        RetStatus = NG;
    } else if (pNftl->PageType == NAND_TYPE_2K) {
        /* check the first 32 bit patern */
        RetStatus = NftlCheckUsedSecs2kPage(pPat, Sec, Secs);

    } else if (pNftl->PageType == NAND_TYPE_4K) {
        /* check the first 32 bit patern */
        if (TmpSec >= 64U) {
            n = TmpSec >> 6U;
            PatIdx += n;
            TmpSec = TmpSec - (n << 6U);
        }

        if ((TmpSec + TmpSecs) <= 64U) {
            n = TmpSecs;
        } else {
            n = 64U - Sec;
        }
#if 1
        if (((TmpSec + n) % 8U) != 0U) {
            n += (8U - ((TmpSec + n) % 8U));
        }
#endif
        Mask = NftlCreateSecsMask64(TmpSec, n);

        if ((pPat[PatIdx] & Mask) > 0U) {
            RetStatus = 1;
        } else {
            PatIdx ++;
            TmpSecs -= n;

            /* check the remain bit patern */
            for (i = 0; i < TmpSecs; i += 64U) {
                if ((TmpSecs - i) >= 64U) {
                    Mask = 0xffffffffU;
                } else {
                    n = TmpSecs - i;
                    Mask = NftlCreateSecsMask64(0U, n);
                }

                if ((pPat[PatIdx] & Mask) > 0U) {
                    RetStatus = 1;
                    break;
                }
                PatIdx ++;
            }
        }
    } else {
        RetStatus = NG;
    }

    return RetStatus;
}

/*
 *  @RoutineName:: NftlGetLba
 *
 *  @Description:: Get the logic block address mapped to the physical block address.
 *
 *  @Input      ::
 *          AMBA_NFTL_OBJ_s *pNftl: Pointer to NFTL object
 *          UINT32 Pba          : Physical block address
 *          UINT32 Page         : Page address in block
 *          UINT16 *pLba        : Pointer to the relative logical block address
 *          UINT8 *pSpareBuf    : Pointer to spare area data
 *          UINT32 Mode         : Indicate get lba from device or from spare_buf
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
static UINT32 NftlGetLba(AMBA_NFTL_OBJ_s *pNftl, UINT32 Pba, UINT32 Page, UINT16 *pLba,
                         UINT8 *pSpareBuf, UINT32 Mode)
{
    const NAND_SPARE_LAYOUT_LARGE_s *pLargeSpr;
    UINT32 RetStatus = 0;
    UINT32 PageAddr;
    const AMBA_NAND_DEV_INFO_s *pDev = AmbaNAND_GetCommonInfo();

    if (Mode == (UINT32)NFTL_FROM_DEV) {
        PageAddr  = ((pNftl->StartBlkPart + Pba) * pDev->BlockPageSize) + Page;
        RetStatus = AmbaNandOp_Read(PageAddr, BAD_BLOCK_PAGES, NULL, pSpareBuf, 5000U);
        if (RetStatus != OK) {
            /* Bypass Report ECC error */
            if (RetStatus == NAND_ERR_ECC) {
                RetStatus = OK;
            }
        }
        if (RetStatus != OK) {
            NftlUpdateBBInfo(pNftl, Pba);
            NftlUpdateBBTable(pNftl, Pba);
            pNftl->BBM.FailRdCnt++;
        }
    }

    if (RetStatus == OK) {
        UINT32 TmpLBA;
        UINT16 TmpLba16 = 0;
        AmbaMisra_TypeCast32(&pLargeSpr, &pSpareBuf);
        TmpLBA = pLargeSpr->Lsn[1];
        TmpLBA = (TmpLBA << 8U);
        TmpLBA = (TmpLBA | pLargeSpr->Lsn[0]);
        (void)AmbaWrap_memcpy(&TmpLba16, &TmpLBA, sizeof(UINT16));
        *pLba = TmpLba16;
    }
    return RetStatus;
}

/*
 *  @RoutineName:: NftlGetPba
 *
 *  @Description:: Get the absolutely physical block address corresponding to the logic block
 *              address from the lba table.
 *
 *  @Input      ::
 *          AMBA_NFTL_OBJ_s *pNftl: Pointer to NFTL object
 *          UINT32 Pba          : Logical block address
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          UINT32  : Absolutely physical block address
 */
static UINT32 NftlGetPba(const AMBA_NFTL_OBJ_s *pNftl, UINT32 Lba)
{
    UINT32 Zone, Pba = 0U;
    const UINT16 *pTrlTable = NULL;

    if (Lba == NFTL_UNUSED_BLK) {
        Pba = NFTL_UNUSED_BLK;
    } else {
        pTrlTable = pNftl->pTrlTable;

        if (!(pTrlTable != NULL)) {
            Pba = NFTL_UNUSED_BLK;
        }
    }

    if ((pTrlTable != NULL) &&
        (Pba == 0U)) {
        Zone = Lba / pNftl->LBlksPerZone;
        Pba = pTrlTable[Lba];

        if (Pba == NFTL_UNUSED_BLK) {
            Pba = NFTL_UNUSED_BLK;
        } else if (Pba >= pNftl->PBlksPerZone) {
            Pba = NFTL_UNUSED_BLK;
        } else {
            Pba = (Zone * pNftl->PBlksPerZone) + Pba;
        }
    }
    return Pba;
}

static INT32 NftlCreateLbaTblCheckArg(const AMBA_NFTL_OBJ_s *pNftl)
{
    UINT32 MaxZone;
    UINT32 TableSize;
    INT32 RetStatus = 0;

    /* Guarantee we have mutiple "pblks_per_zone" blocks */
    if (!((pNftl->TotalBlks % pNftl->PBlksPerZone) == 0U)) {
        RetStatus = NG;
    } else {
        MaxZone = pNftl->TotalBlks / pNftl->PBlksPerZone;
        TableSize = (MaxZone * pNftl->LBlksPerZone) << 1U;

        if (TableSize > TRL_TABLE_SIZE) {
            //(void)AmbaPrint("%s: Failed for logical NAND translation table", g_NftlPart[pNftl->ID]);
            RetStatus = NG;
        }
    }
    return RetStatus;
}

/*
 *  @RoutineName:: NftlCreateLbaTbl
 *
 *  @Description:: Create logical to physical block address translation table
 *
 *  @Input      ::
 *          AMBA_NFTL_OBJ_s *pNftl    : Pointer to NFTL object
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
static INT32 NftlCreateLbaTbl(AMBA_NFTL_OBJ_s *pNftl)
{
    static UINT8  LbaBuf[(NAND_SPARE_256 * BAD_BLOCK_PAGES)] GNU_ALIGNED_CACHESAFE GNU_SECTION_NOZEROINIT;
    UINT32 Zone, Block, AbsPBlk, MaxZone;
    UINT16 Lba = NFTL_UNUSED_BLK, Pba;
    UINT32 TableSize;
    INT32 RetStatus = 0;
#ifdef ENABLE_VERIFY_NFTL
    UINT32 PageAddr;
#endif
    UINT16 *pTrlTable = &g_LbaTrlTable[pNftl->ID * TRL_TABLE_SIZE];

    RetStatus = NftlCreateLbaTblCheckArg(pNftl);

    if (RetStatus == 0) {

        MaxZone = pNftl->TotalBlks / pNftl->PBlksPerZone;
        TableSize = (MaxZone * pNftl->LBlksPerZone) << 1U;

        pNftl->pTrlTable = &g_LbaTrlTable[pNftl->ID * TRL_TABLE_SIZE];

        pTrlTable = pNftl->pTrlTable;
        (void)AmbaWrap_memset(pTrlTable, 0xffff, TableSize);

        for (Zone = 0; Zone < MaxZone; Zone++) {
            for (Block = 0; Block < pNftl->PBlksPerZone; Block++) {
                UINT32 Skip = 0U;
                UINT16 Tmp;
                AbsPBlk = (Zone * pNftl->PBlksPerZone) + Block;

                if (OK != NftlGetLba(pNftl, AbsPBlk, 0, &Lba, LbaBuf, NFTL_FROM_DEV)) {
                    RetStatus = -1;
                    Skip = 1U;
                }

                if (AmbaNandBBT_IsBadBlock(pNftl->StartBlkPart + AbsPBlk) != AMBA_NAND_BLK_GOOD) {
                    NftlUpdateBBInfo(pNftl, AbsPBlk);
                    NftlUpdateBBTable(pNftl, AbsPBlk);
                    //DBGMSG("%s: %s bad block %d", pNftl->ID, __FUNCTION__, AbsPBlk + pNftl->StartBlkPart);
                    Skip = 1U;
                }

                if (Lba == NFTL_UNUSED_BLK) {
                    /* it is an unused block, check next block */
                    Skip = 1U;
                }

                /* is LBA abnormal */
                if ((Skip == 0U) &&
                    (Lba >= pNftl->LBlksPerZone)) {
                    //PRINT("%s: %s LBA abnormal, erase block %d", g_NftlPart[pNftl->ID], __FUNCTION__, AbsPBlk + pNftl->StartBlkPart);
                    if (OK != AmbaNandOp_EraseBlock(pNftl->StartBlkPart + AbsPBlk, 5000U)) {
                        RetStatus = NAND_OP_ERASE_ER;
                        NftlUpdateBBInfo(pNftl, AbsPBlk);
                        NftlUpdateBBTable(pNftl, AbsPBlk);
                    }
                    Skip = 1U;
                }

                /* check to see the current LBA is entried in table */
                Pba = pTrlTable[(Zone * pNftl->LBlksPerZone) + Lba];
                if ((Skip == 0U) &&
                    (Pba != NFTL_UNUSED_BLK)) {
#ifdef ENABLE_VERIFY_NFTL
                    /* read spare area of current block */
                    PageAddr = (pNftl->StartBlkPart + AbsPBlk) * pDev->BlockPageSize;
                    RetStatus = AmbaNandOp_Read(PageAddr, pDev->BlockPageSize, NULL, pNftl->pSpareDebug, 5000U);
                    if (RetStatus < 0) {
                        NftlUpdateBBInfo(pNftl, AbsPBlk);
                        NftlUpdateBBTable(pNftl, AbsPBlk);
                    }
                    Skip = 1U;
                    /* read entried block spare */
                    PageAddr = (pNftl->StartBlkPart + Zone * pNftl->PBlksPerZone + Pba) * pDev->BlockPageSize;
                    RetStatus = AmbaNandOp_Read(PageAddr, pDev->BlockPageSize, NULL, pNftl->pSpareDebug, 5000U);
                    if (RetStatus < 0) {
                        NftlUpdateBBInfo(pNftl, AbsPBlk);
                        NftlUpdateBBTable(pNftl, AbsPBlk);
                    }
                    Skip = 1U;
#endif
                    /* it's already entried in the table erase current block */
                    //PRINT("%s: %s erase current block %d (already)", g_NftlPart[pNftl->ID], __FUNCTION__, AbsPBlk + pNftl->StartBlkPart);
                    if (OK != AmbaNandOp_EraseBlock(pNftl->StartBlkPart + AbsPBlk, 5000U)) {
                        RetStatus = NAND_OP_ERASE_ER;
                        NftlUpdateBBInfo(pNftl, AbsPBlk);
                        NftlUpdateBBTable(pNftl, AbsPBlk);
                    }
                    Skip = 1U;
                }

                if (RetStatus == -1) {
                    break;
                }

                if (Skip == 1U) {
                    continue;
                }

                (void)AmbaWrap_memcpy(&Tmp, &Block, sizeof(Tmp));
                pTrlTable[(Zone * pNftl->LBlksPerZone) + Lba] = Tmp;
            }
            if (RetStatus == -1) {
                break;
            }
        }
    } /* RetStatus != NG */

    return RetStatus;
}

/*
 *  @RoutineName:: NftlCreatePbaTbl
 *
 *  @Description:: Create physical to logical block address translation table
 *
 *  @Input      ::
 *          AMBA_NFTL_OBJ_s *pNftl    : Pointer to NFTL object
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
static INT32 NftlCreatePbaTbl(AMBA_NFTL_OBJ_s *pNftl)
{
    static UINT16 g_PbaTrlTable[(AMBA_NUM_NFTL_PARTITION * TRL_TABLE_SIZE)] GNU_SECTION_NOZEROINIT;
    UINT16 Zone, Block, Pba;
    UINT32 TableSize, MaxZone;
    UINT16 *pPhyTable = NULL;
    const UINT16 *pTrlTable = NULL;
    INT32 RetVal = 0;

    /* Guarantee we have mutiple "pblks_per_zone" blocks */
    if (!((pNftl->TotalBlks % pNftl->PBlksPerZone) == 0U)) {
        RetVal = NG;
    } else {

        MaxZone = pNftl->TotalBlks / pNftl->PBlksPerZone;
        TableSize = (MaxZone * pNftl->PBlksPerZone) << 1U;

        if (TableSize > TRL_TABLE_SIZE) {
            //(void)AmbaPrint("%s: Failed for logical NAND translation table", g_NftlPart[pNftl->ID]);
            RetVal = NG;
        } else {
            pNftl->pPhyTable = &g_PbaTrlTable[pNftl->ID * TRL_TABLE_SIZE];

            pPhyTable = pNftl->pPhyTable;
            pTrlTable = pNftl->pTrlTable;

            (void)AmbaWrap_memset(pPhyTable, 0xff, TableSize);
            for (Zone = 0; Zone < MaxZone; Zone++) {
                for (Block = 0; Block < pNftl->LBlksPerZone; Block++) {
                    Pba = pTrlTable[(Zone * pNftl->LBlksPerZone) + Block];
                    if (Pba == NFTL_UNUSED_BLK) {
                        continue;
                    }

                    if (Pba >= pNftl->PBlksPerZone) {
                        RetVal = NG;
                    }

                    if (RetVal != 0) {
                        break;
                    }
                    pPhyTable[(Zone * pNftl->PBlksPerZone) + Pba] = Block;
                }
            }
        }
    }
    return RetVal;
}

/*
 *  @RoutineName:: NftlInitTblCache
 *
 *  @Description:: Initialize translation table cache
 *
 *  @Input      ::
 *          AMBA_NFTL_OBJ_s *pNftl    : Pointer to NFTL object
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
static INT32 NftlInitTblCache(const AMBA_NFTL_OBJ_s *pNftl)
{
    /* Work Buffer for 1 Block. Due to the FDMA 8-Byte Boundary issue */
    static UINT8 TblCacheBuf[(64 * 2 * 1024)]
    GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE;

    const AMBA_NAND_DEV_INFO_s  *pDev = AmbaNAND_GetCommonInfo();
    const AMBA_NFTL_PART_INFO_s *pNftlInfo = &g_Nftl_Info[pNftl->ID];
    UINT32 i, Pages;
    INT32 RetStatus = NG;

    (void)AmbaWrap_memset(&NftlTblCache[pNftl->ID], 0x0, sizeof(AMBA_NFTL_TABLE_CACHE_s));
    (void)AmbaWrap_memset(TblCacheBuf, 0x0, sizeof(TblCacheBuf));

    /* Try to find table cache */
    Pages = GetRoundUpValU32((pNftl->TotalBlks * sizeof(UINT16)), pDev->MainByteSize);

    for (i = 0U; i < pNftlInfo->TrlBlocks; i++) {
        const UINT8 *pTmp = TblCacheBuf;
        const AMBA_NFTL_TABLE_CACHE_s *pTblCache;
        AmbaMisra_TypeCast32(&pTblCache, &pTmp);
        (void) NftlReadTbLCache(pNftl, TblCacheBuf, i, 1U);
        if (NftlIsValidTblCache(pTblCache) == 1U) {
            (void)AmbaWrap_memcpy(&NftlTblCache[pNftl->ID], pTblCache, sizeof(AMBA_NFTL_TABLE_CACHE_s));

            (void)NftlReadTbLCache(pNftl, TblCacheBuf, i, 0);
            (void)AmbaWrap_memcpy(&g_LbaTrlTable[pNftl->ID * TRL_TABLE_SIZE], TblCacheBuf, Pages * pDev->MainByteSize);

            RetStatus = 0;
            break;
        }
    }

    return RetStatus;
}

/*
 *  @RoutineName:: NftlInitTrlTable
 *
 *  @Description:: Initialize the memory of the logical table and physical table.
 *
 *  @Input      ::
 *          AMBA_NFTL_OBJ_s *pNftl: Pointer to NFTL object
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
static INT32 NftlInitTrlTable(AMBA_NFTL_OBJ_s *pNftl)
{
    UINT32 i;
    INT32 RetStatus = 0;

    if (0x0U == pNftl->Init) {
        RetStatus = 0;
    } else {

        for (i = 0; i < pNftl->TotalZones; i++) {
            pNftl->SBlk[i] = i * pNftl->PBlksPerZone;
        }

        pNftl->NumBB = 0;
        (void)AmbaWrap_memset(pNftl->pBBInfo, 0xff, NFTL_BB_INFO_SIZE);

        RetStatus = NftlCreateLbaTbl(pNftl);
        if (RetStatus >= 0) {
            RetStatus = NftlCreatePbaTbl(pNftl);
        }
    }
    return RetStatus;
}

/*
 *  @RoutineName:: NftlGetReqArg
 *
 *  @Description:: Caculate the parameters required for NftlWrite().
 *
 *  @Input      ::
 *          AMBA_NFTL_OBJ_s *pNftl: Pointer to NFTL object
 *          AMBA_NFTL_ARGS_s *pArg: Pointer to the parameter structure
 *          UINT32 Sec          : Start logical sector address to be writen
 *          UINT32 Secs         : Number of sectors to be writen
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
static void NftlGetReqArg(const AMBA_NFTL_OBJ_s *pNftl, AMBA_NFTL_ARGS_s *pArg, UINT32 Sec, UINT32 Secs)
{
    const AMBA_NAND_DEV_INFO_s *pDev = AmbaNAND_GetCommonInfo();
    UINT32 Pages, Page;
    UINT32 SectorPerPage;
#ifdef ENABLE_VERIFY_NFTL
    INT32 Secs2, Secs3;
    Secs2 = Secs;
#endif

    if (!(pArg != NULL) || !(Secs > 0U)) {
        /* return */
    } else {

        if ((pNftl->PageType == NAND_TYPE_2K) || (pNftl->PageType == NAND_TYPE_4K)) {
            if (pNftl->PageType == NAND_TYPE_2K) {
                SectorPerPage = NAND_SPP_4;
            } else {
                SectorPerPage = NAND_SPP_8;
            }
            pArg->SecNum = Sec & (SectorPerPage - 1U);
            pArg->Page   = (Sec / SectorPerPage) & (NAND_PPB_64 - 1U);
            pArg->Lba    = Sec / (SectorPerPage * NAND_PPB_64);

            /* Caculate sector boundry conditon in first block */
            if (pArg->SecNum > 0U) {
                pArg->SecsStart = g_Nftl_Info[pNftl->ID].SectorsPerPage - pArg->SecNum;
                if (pArg->SecsStart > Secs) {
                    pArg->SecsStart = Secs;
                }

                Page = pArg->Page + 1U;
            } else {
                pArg->SecsStart = 0;
                Page = pArg->Page;
            }

            Secs -= pArg->SecsStart;
            Pages = Secs / SectorPerPage;

            /* Caculate page boundry conditon in first block */
            if (Page < NAND_PPB_64) {
                if ((Page + Pages) >= pDev->BlockPageSize) {
                    pArg->PagesStartBlk = pDev->BlockPageSize - Page;
                    pArg->SecsEndBlk = 0;
                } else {
                    pArg->PagesStartBlk = Pages;
                    pArg->SecsEndBlk = Secs & (SectorPerPage - 1U);
                }
            } else {
                pArg->PagesStartBlk = 0;
                pArg->SecsEndBlk = 0;
            }

            Secs -= (pArg->PagesStartBlk * SectorPerPage) + pArg->SecsEndBlk;

            /* remain pages in next blocks */
            Pages = Secs / SectorPerPage;

            /* Caculate block boundry conditon for remain sectors */
            pArg->Blocks        = Pages >> NAND_PPB_64_SHT;
            pArg->PagesEnd      = Pages & (NAND_PPB_64 - 1U);
            pArg->SecsEndPage   = Secs & (SectorPerPage - 1U);
        } else {
            /*  */
        }

#ifdef ENABLE_VERIFY_NFTL
        Secs3 = pArg->SecsStart + pArg->PagesStartBlk * g_Nftl_Info[pNftl->ID].SectorsPerPage +
                pArg->SecsEndBlk +
                pArg->Blocks * pDev->BlockPageSize * g_Nftl_Info[pNftl->ID].SectorsPerPage +
                pArg->PagesEnd * g_Nftl_Info[pNftl->ID].SectorsPerPage +
                pArg->SecsEndPage;

        if (Secs3 != Secs2) {
            //PRINT("%s: nftl request argument caculation incorrectly", g_NftlPart[pNftl->ID]);
        }
#endif
    }
}

/*
 *  @RoutineName:: NftlInitBbm
 *
 *  @Description:: Initialize bad blocks management
 *
 *  @Input      ::
 *          AMBA_NFTL_OBJ_s *pNftl: Pointer to NFTL object
 *
 *  @Output     :: none
 *
 *  @Return     :: none
 */
static void NftlInitBbm(AMBA_NFTL_OBJ_s *pNftl)
{
    AMBA_NFTL_BBM_s *pBbm;

    pBbm = &pNftl->BBM;
    pBbm->Count = 0;
}

/*
 *  @RoutineName:: NftlSaveParam
 *
 *  @Description:: Save the parameters
 *
 *  @Input      ::
 *          AMBA_NFTL_OBJ_s *pNftl: Pointer to NFTL object
 *          AMBA_NFTL_ARGS_s *pArg: Pointer to the parameter structure
 *          UINT8 *pBuf         : Pointer to buffer where write data is stored
 *          UINT32 *pPat        : Used sectors bit pattern corresponding to pba
 *          UINT32 Pba          : Absolutely physical block address
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
static void NftlSaveParam(AMBA_NFTL_OBJ_s *pNftl, const AMBA_NFTL_ARGS_s *pArg, UINT8 *pBuf, UINT32 *pPat,
                          UINT32 Pba, UINT32 Condition)
{
    UINT32 Size;
    AMBA_NFTL_BBM_s *pBbm;
    UINT32 Idx;

    pBbm = &pNftl->BBM;
    Idx = pBbm->Count;

    Size = sizeof(AMBA_NFTL_ARGS_s);
    (void)AmbaWrap_memcpy(&pBbm->Arg[Idx], pArg, Size);

    pBbm->pBuf      = pBuf;
    pBbm->pPat      = pPat;
    pBbm->Pba       = Pba;
    pBbm->Condition = Condition;
}

/*
 *  @RoutineName:: NftlReclaimBadBlks
 *
 *  @Description::
 *
 *  @Input      ::
 *          AMBA_NFTL_OBJ_s *pNftl    : Pointer to NFTL object
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
//static INT32 NftlReclaimBadBlks(AMBA_NFTL_OBJ_s *pNftl, UINT32 BBType)
//{
//    return 0;
//}

static UINT32 NftlSearchErasedBlockFromStart(AMBA_NFTL_OBJ_s *pNftl, const UINT16 *pPhyTable, UINT32 Start, UINT32 Min, UINT32 Max, UINT32 Zone, INT32 *pRvalBlk)
{
    UINT32 Pba;
    UINT32 Find = 0U;
    INT32 RetStatus = 0;

    for (Pba = Start; Pba < Max; Pba++) {
        if (pPhyTable[Pba] == NFTL_UNUSED_BLK) {
            if (AmbaNandBBT_IsBadBlock(pNftl->StartBlkPart + Pba) != AMBA_NAND_BLK_GOOD) {
                continue;
            } else {
                pNftl->SBlk[Zone] = Pba;
                if (!((Pba / pNftl->PBlksPerZone) == Zone)) {
                    RetStatus = NG;
                } else {
                    AmbaMisra_TypeCast32(&RetStatus, &Pba);
                    Find = 1U;
                }
            }
        }
    }

    if (Find == 0U) {
        for (Pba = Min; Pba < Start; Pba++) {
            if (pPhyTable[Pba] == NFTL_UNUSED_BLK) {
                if (AmbaNandBBT_IsBadBlock(pNftl->StartBlkPart + Pba) != AMBA_NAND_BLK_GOOD) {
                    continue;
                } else {
                    pNftl->SBlk[Zone] = Pba;
                    if (!((Pba / pNftl->PBlksPerZone) == Zone)) {
                        RetStatus = NG;
                    } else {
                        AmbaMisra_TypeCast32(&RetStatus, &Pba);
                        Find = 1U;
                    }
                }
            }
        }
    }

    *pRvalBlk = RetStatus;

    return Find;
}

/*
 *  @RoutineName:: NftlWrPSecsNewBlock
 *
 *  @Description:: Search erased block in the zone which the lba at.
 *
 *  @Input      ::
 *          AMBA_NFTL_OBJ_s *pNftl: Pointer to NFTL object
 *          UINT32 Lba          : Logical block address used to find out which zone needed to be search the erased block
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
static INT32 NftlSearchErasedBlock(AMBA_NFTL_OBJ_s *pNftl, UINT32 Lba)
{
#define MAX_SEARCH_RETRY 2

    UINT32 Zone, Min, Max, Start, Find = 0U;
    const UINT16 *pPhyTable;
    INT32 RetStatus = 0, Retry = 0;

    while (Retry < MAX_SEARCH_RETRY) {
        Zone = Lba / pNftl->LBlksPerZone;
        if (!(Zone < pNftl->TotalZones)) {
            RetStatus = NG;
        } else {

            pPhyTable = pNftl->pPhyTable;

            Min = Zone * pNftl->PBlksPerZone;
            Max = (Zone + 1U) * pNftl->PBlksPerZone;
            Start = pNftl->SBlk[Zone];

            Find = NftlSearchErasedBlockFromStart(pNftl, pPhyTable, Start, Min, Max, Zone, &RetStatus);

            if (Find == 1U) {
                break;
            } else {

                //PRINT("%s: %s fail!", g_NftlPart[pNftl->ID], __FUNCTION__);

                if (Retry == 0) {
                    //PRINT("%s: Try to do nftl_reclaim_bad_block...", g_NftlPart[pNftl->ID]);

                    //RetStatus = NftlReclaimBadBlks(pNftl, AMBA_NAND_BLK_RUNTIME_BAD);
                    //if (RetStatus == 0)
                    {
                        RetStatus = NftlInitTrlTable(pNftl);
                        if (RetStatus < 0) {
                            //PRINT("%s: init tables failed!", g_NftlPart[pNftl->ID]);
                            pNftl->WrProtect = 1;
                            RetStatus = NG;
                        }
                        if (RetStatus != 0) {
                            //PRINT("%s: Retry %s", g_NftlPart[pNftl->ID], __FUNCTION__);
                        }
                    }
                }
            }
        }
        if (RetStatus != 0) {
            Retry++;
        }
    }

    if (RetStatus != 0) {
        if (Retry >= MAX_SEARCH_RETRY) {
            pNftl->WrProtect = 1;
        }
    }

    return RetStatus;
}


static INT32 AmbaNFTL_FindErasedBlock(AMBA_NFTL_OBJ_s *pNftl, UINT32 Zone, UINT32 Max, UINT32 Min, UINT32 Start, UINT32 Offset, UINT32 *pFind)
{
    INT32  RetVal = 0;
    UINT32 Pba, SearchDone = 0;
    const UINT16 *pPhyTable = pNftl->pPhyTable;

    *pFind = 0;

    for (Pba = Start; Pba < Max; Pba += Offset) {
        if (pPhyTable[Pba] == NFTL_UNUSED_BLK) {
            if (AmbaNandBBT_IsBadBlock(pNftl->StartBlkPart + Pba) != AMBA_NAND_BLK_GOOD) {
                continue;
            } else {
                pNftl->SBlk[Zone] = Pba;
                if (!((Pba / pNftl->PBlksPerZone) == Zone)) {
                    RetVal = NG;
                } else {
                    AmbaMisra_TypeCast32(&RetVal, &Pba);
                    *pFind = 1U;
                }
                SearchDone = 1U;
                break;
            }
        }
    }

    if (SearchDone == 1U) {
        for (Pba = Min; Pba < Start; Pba += Offset) {
            if (pPhyTable[Pba] == NFTL_UNUSED_BLK) {
                if (AmbaNandBBT_IsBadBlock(pNftl->StartBlkPart + Pba) != AMBA_NAND_BLK_GOOD) {
                    continue;
                } else {
                    pNftl->SBlk[Zone] = Pba;
                    if (!((Pba / pNftl->PBlksPerZone) == Zone)) {
                        RetVal = NG;
                    } else {
                        AmbaMisra_TypeCast32(&RetVal, &Pba);
                        *pFind = 1U;
                    }
                    break;
                }
            }
        }
    }

    return RetVal;
}

static void GetMinStartFromPlane(UINT32 PlaneIn, UINT32 PlaneMin, UINT32 PlaneStart, UINT32 Offset, UINT32 *pMin, UINT32 *pStart)
{
    if (PlaneIn > PlaneMin) {
        *pMin += (PlaneIn - PlaneMin);
    } else if (PlaneIn < PlaneMin) {
        *pMin += Offset - (PlaneMin - PlaneIn);
    } else {
        /* Misra C */
    }

    if (PlaneIn > PlaneStart) {
        *pStart += (PlaneIn - PlaneStart);
    } else if (PlaneIn < PlaneStart) {
        *pStart += Offset - (PlaneStart - PlaneIn);
    } else {
        /* Misra C */
    }
}

static INT32 CheckSearchBankRange(UINT32 BankMin, UINT32 BankMax, UINT32 BankIn, UINT32 TotalBlocks, UINT32 *pMax, UINT32 *pMin)
{
    INT32 RetStatus = 0;

    if (BankMin != BankMax) {
        /* Garateen the search range is in the same bank to find an erased block for copyback command */
        if ((BankIn == BankMin) && (BankIn < BankMax)) {
            *pMax = BankMax * TotalBlocks;
        } else if ((BankIn > BankMin) && (BankIn == BankMax)) {
            *pMin = BankMax * TotalBlocks;
        } else {
            RetStatus = NG;
        }
    }
    return RetStatus;
}

/*
 *  @RoutineName:: NftlSearchErasedBlockCb
 *
 *  @Description:: Search erased block in the same plane and in the same zone which the lba at.
 *                 This is used in the case which copyback command used.
 *
 *  @Input      ::
 *          AMBA_NFTL_OBJ_s *pNftl: Pointer to NFTL object
 *          UINT32 Lba          : Logical block address used to find out which zone needed to be search the erased block
 *           UINT32 PbaIn       : Absolutely physical block address corresponding to the lba
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
static INT32 NftlSearchErasedBlockCb(AMBA_NFTL_OBJ_s *pNftl, UINT32 Lba, UINT32 PbaIn)
{
#define SEARCH_BLOCK_MAX_RETRY 2
    INT32 RetStatus = 0, Retry = 0;
    UINT32 Zone, PlaneIn, PlaneMin, PlaneStart, Offset = 0U, Find = 0U;
    UINT32 Min = 0U, Max = 0U, MinMin = 0U, MaxMax = 0U, Start = 0U;
    UINT32 BankMin, BankMax, BankIn;
    UINT32 TotalBlocks = 0;//AmbaRTSL_NandCtrl.TotalNumBlk;
    const AMBA_NAND_DEV_INFO_s *pDev = AmbaNAND_GetCommonInfo();

    while (Retry < SEARCH_BLOCK_MAX_RETRY) {
        Zone = Lba / pNftl->LBlksPerZone;
        if (Zone >= pNftl->TotalZones) {
            RetStatus = NG;
        } else {
            Offset = 0;
            Min    = Zone * pNftl->PBlksPerZone;
            Max    = (Zone + 1U) * pNftl->PBlksPerZone;
            MinMin = Min;
            MaxMax = Max;
            Start  = pNftl->SBlk[Zone];
        }

        if ((RetStatus == 0) &&
            (pDev->PlaneAddrMask != 0U)) {
            /* no copyback command support, just search erased block in the same Zone */
            BankIn  = PbaIn / TotalBlocks;
            BankMin = Min / TotalBlocks;
            BankMax = Max / TotalBlocks;

            RetStatus = CheckSearchBankRange(BankMin, BankMax, BankIn, TotalBlocks, &Max, &Min);
            if (RetStatus == 0) {
                /* It used PseudoCB to replace Copyback cmd of nand when enable BCH
                 * algorithm of Nand Controller */
                /* pDev->PlaneAddrMask == 1 */
                /* plane address is according the lowest block address */
                Offset      = pDev->TotalPlanes;
                PlaneIn     = PbaIn & (pDev->TotalPlanes - 1U);
                PlaneMin    = Min & (pDev->TotalPlanes - 1U);
                PlaneStart  = Start & (pDev->TotalPlanes - 1U);

                GetMinStartFromPlane(PlaneIn, PlaneMin, PlaneStart, Offset, &Min, &Start);
            }

            /*
             *  Find the erased block in the same plane
             *  search area = blocks in the same plane
             *  o-----------o--------o---------o------------o
             *  MinMin     Min      Start     Max          MaxMax
             */
            if (!((MinMin <= Min) && (MaxMax >= Max))) {
                RetStatus = NG;
            } else {
                RetStatus = AmbaNFTL_FindErasedBlock(pNftl, Zone, Max, Min, Start, Offset, &Find);
            }
        }

        if ((Find == 0U) &&
            (RetStatus == 0)) {
            /*
             *  Can not find erased block in the same plane, so find the erased block in the same Zone.
             *  search area = blocks per Zone
             *  o-------------------o---------------------o
             *  MinMin           Start                   MaxMax
             */
            RetStatus = AmbaNFTL_FindErasedBlock(pNftl, Zone, MaxMax, MinMin, Start, 1U, &Find);

            //PRINT("%s: %s fail!", g_NftlPart[pNftl->ID], __FUNCTION__);

            if ((Find == 0U) && (Retry == 0)) {
                //PRINT("%s: Try to do nftl_reclaim_bad_block...", g_NftlPart[pNftl->ID]);
                //RetStatus = NftlReclaimBadBlks(pNftl, AMBA_NAND_BLK_RUNTIME_BAD);
                //if (RetStatus == 0)
                {
                    RetStatus = NftlInitTrlTable(pNftl);
                    if (RetStatus < 0) {
                        //PRINT("%s: init tables failed!", g_NftlPart[pNftl->ID]);
                        pNftl->WrProtect = 1;
                        RetStatus = NG;
                    } else {
                        /* For Misra C checking */
                        //PRINT("%s: Retry nftl_search_erased_block", g_NftlPart[pNftl->ID]);
                    }
                }
            }
            if ((RetStatus == NG) && (pNftl->WrProtect == 1U)) {
                Retry = SEARCH_BLOCK_MAX_RETRY; /* Init tables failed! Break */
            } else {
                Retry++;
            }
        }
        if (Find == 1U) {
            break;
        }
    } /* while() */

    if (RetStatus == NG) {
        pNftl->WrProtect = 1U;
    }
    return RetStatus;
}
#if 0
/*
 *  @RoutineName:: NftlCopybackByPat
 *
 *  @Description:: NFTL copyback by input sectors used pattern
 *
 *  @Input      ::
 *          AMBA_NFTL_OBJ_s *pNftl: Pointer to the NFTL object
 *          UINT32 BlockFrom    : Copy from
 *          UINT32 BlockTo      : Copy to
 *          UINT32 pPat         : Sectors used pattern
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
static INT32 NftlCopybackByPat(AMBA_NFTL_OBJ_s *pNftl, UINT32 BlockFrom, UINT32 BlockTo, const UINT32 *pPat)
{
    UINT32 i, j, k, n, s, Value;
    INT32 RetStatus = 0;
    UINT32 DestPageAddr, SrcPageAddr, SectorPerPage, SecPerPageShift;
    const AMBA_NAND_DEV_INFO_s *pDev = AmbaNAND_GetCommonInfo();

    SectorPerPage   = (pNftl->PageType == NAND_TYPE_2K) ? NAND_SPP_4    : NAND_SPP_8;
    SecPerPageShift = (pNftl->PageType == NAND_TYPE_2K) ? NAND_SPP_4_SHT : NAND_SPP_8_SHT;

    /* total number of 32-bit pattern */
    n = g_Nftl_Info[pNftl->ID].SectorsPerBlock >> 5U;

    for (i = 0U; i < n; i++) {
        for (j = 0U; j < 32U; j = ((((k + 1U) << SecPerPageShift) - 1U) & (0x1fU)) + 1U) {
            s = (i << 5U) + j;          /* sector */
            k = s >> SecPerPageShift;    /* page */

            Value = (pPat[i] >> j) & 0x1U;
            if (Value != 0U) {
                DestPageAddr = ((pNftl->StartBlkPart + BlockTo) * pDev->BlockPageSize) + k;
                SrcPageAddr = ((pNftl->StartBlkPart + BlockFrom) * pDev->BlockPageSize) + k;
                if (OK != AmbaNandOp_Copyback(DestPageAddr, SrcPageAddr, 5000U)) {
                    NftlUpdateBBInfo(pNftl, BlockTo);
                    NftlUpdateBBTable(pNftl, BlockTo);
                    pNftl->BBM.FailWrCnt++;
                    RetStatus = NAND_OP_PROG_ER;
                    break;
                }
                /* update index to next page */
                /* j = (((k + 1U) << SecPerPageShift) - 1U) & (0x1fU) + 1U; */
            }
        }
        if (RetStatus < 0) {
            break;
        }
    }

    return RetStatus;
}
#endif
/*
 *  @RoutineName:: NftlBadBlockHandler
 *
 *  @Description:: NFTL bad block handler
 *
 *  @Input      ::
 *          AMBA_NFTL_OBJ_s *pNftl: Pointer to NFTL object
 *          AMBA_NFTL_ARGS_s *pArg: Pointer to the parameter structure
 *          INT32 ErrNo            : Error number
 *
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
static INT32 NftlBadBlockHandler(AMBA_NFTL_OBJ_s *pNftl, AMBA_NFTL_ARGS_s *pArg, INT32 ErrNo)
{
    INT32  RetStatus = 0;
    AMBA_NFTL_BBM_s *pBbm = &pNftl->BBM;;

    pBbm->Count++;
    pBbm->TotalCnt++;

    if (pBbm->Count > NFTL_BBM_HANDLER_MAX) {
        RetStatus = NFTL_FATAL_ER;
    } else {
        UINT32 Idx = pBbm->Count - 1U;

        switch (ErrNo) {
        case NAND_OP_DRIVER_ER:
            //PRINT("%s: %s (NAND_OP_DRIVER_ER)", g_NftlPart[pNftl->ID], __FUNCTION__);
            RetStatus = NAND_OP_DRIVER_ER;
            break;
        case NAND_OP_ERASE_ER:
            /* bad block has been marked in nand_op */
            //PRINT("%s: %s (NAND_OP_ERASE_ER)", g_NftlPart[pNftl->ID], __FUNCTION__);
            RetStatus = 0;
            break;
        case NAND_OP_PROG_ER:
            //PRINT("%s: %s (NAND_OP_PROG_ER)", g_NftlPart[pNftl->ID], __FUNCTION__);
#if 0
            RetStatus = NftlProgErHandler(pNftl, pBbm);
            if (RetStatus == 0) {
                (void)AmbaWrap_memcpy(pArg, &pBbm->Arg[Idx], sizeof(AMBA_NFTL_ARGS_s));
            }
#else
            (void)pArg;
#endif
            break;
        case NAND_OP_READ_ER:
            //PRINT("%s: %s (NAND_OP_READ_ER)", g_NftlPart[pNftl->ID], __FUNCTION__);
            /* update the tables to avoid to read the same block again */
            NftlUpdateLogicTable(pNftl, pBbm->Arg[Idx].Lba, NFTL_UNUSED_BLK);
            NftlUpdatePhyTable(pNftl, pBbm->Pba, NFTL_UNUSED_BLK);
            RetStatus = NAND_OP_READ_ER;
            break;
        case NAND_OP_WP_ER:
            //PRINT("%s: %s (NAND_OP_WP_ER)", g_NftlPart[pNftl->ID], __FUNCTION__);
            RetStatus = NAND_OP_WP_ER;
            break;
        case NAND_OP_NOT_READY_ER:
            //PRINT("%s: %s (NAND_OP_NOT_READY_ER)", g_NftlPart[pNftl->ID], __FUNCTION__);
            RetStatus = NAND_OP_NOT_READY_ER;
            break;
        case NAND_OP_MARK_BBLK_ER:
            //PRINT("%s: %s (NAND_OP_MARK_BBLK_ER)", g_NftlPart[pNftl->ID], __FUNCTION__);
            RetStatus = NAND_OP_MARK_BBLK_ER;
            break;
        default:
            //PRINT("%s: %s (fatal error)", g_NftlPart[pNftl->ID], __FUNCTION__);
            RetStatus = NFTL_FATAL_ER;
            break;
        }
    }
    return RetStatus;
}

/*
 *  @RoutineName:: NftlWriteSecs
 *
 *  @Description:: Write data to the specified sectors including lba in spare area.
 *                  Only for 2k page NAND flash.
 *
 *  @Input      ::
 *          AMBA_NFTL_OBJ_s *pNftl: Pointer to NFTL object
 *          UINT8 *pBuf         : Pointer to buffer where write data is stored
 *          UINT32 Lba          : Absolutely logical block address
 *          UINT32 Pba          : Absolutely physical block address
 *          UINT32 Page         : Page address in the block
 *          UINT32 Sec          : Sector address in page
 *          UINT32 Secs         : Number of sectors to be writen
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
static INT32 NftlWriteSecs(AMBA_NFTL_OBJ_s *pNftl, const UINT8 *pBuf, UINT32 Lba, UINT32 Pba,
                           UINT32 Page, UINT32 Sec, UINT32 Secs)
{
    NAND_SPARE_LAYOUT_LARGE_s *pLargeSpr = NULL;
    UINT32 i, PageAddr;
    INT32 RetStatus = 0;
    const AMBA_NAND_DEV_INFO_s *pDev = AmbaNAND_GetCommonInfo();

    if (!((Sec + Secs) <= g_Nftl_Info[pNftl->ID].SectorsPerPage) || !(Page < pDev->BlockPageSize)) {
        RetStatus = NG;
    } else {

        //DBGMSG("%s: %s(..., %d, %d)", g_NftlPart[pNftl->ID], __FUNCTION__, Sec, Secs);

        Lba %= pNftl->LBlksPerZone;

        (void)AmbaWrap_memset(pNftl->pMainBuf, 0xff, pDev->MainByteSize);
        (void)AmbaWrap_memset(pNftl->pSpareBuf, 0xff, pDev->SpareByteSize);

        (void)AmbaWrap_memcpy(&pNftl->pMainBuf[(Sec << NAND_SEC_SHT)], pBuf, Secs << NAND_SEC_SHT);

        AmbaMisra_TypeCast32(&pLargeSpr, &pNftl->pSpareBuf);

        /* Filled page used bit to spare area.*/
        pLargeSpr->Pub = 0x0;

        for (i = 0; i < g_Nftl_Info[pNftl->ID].SectorsPerPage; i++) {
            //            pLargeSpr->Lsn[0] = (UINT8) Lba;
            //            pLargeSpr->Lsn[1] = (UINT8) ((Lba & 0xff00U) >> 8U);
            pLargeSpr[i].Lsn[0] = (UINT8) Lba;
            pLargeSpr[i].Lsn[1] = (UINT8) ((Lba & 0xff00U) >> 8U);
#ifdef WRITE_ALL_PUB
            pLargeSpr[i].Pub = 0x0;
#endif
        }

        /* Dual-space mode can write main and spare area, simultaneously. */
        PageAddr = ((pNftl->StartBlkPart + Pba) * pDev->BlockPageSize) + Page;
        if (OK != AmbaNandOp_Program(PageAddr, 1, pNftl->pMainBuf, pNftl->pSpareBuf, 5000U)) {
            NftlUpdateBBInfo(pNftl, Pba);
            NftlUpdateBBTable(pNftl, Pba);
            pNftl->BBM.FailWrCnt++;
            RetStatus = NAND_OP_PROG_ER;
        }
    }
#ifdef ENABLE_VERIFY_NFTL

    if (RetStatus >= 0) {

        PageAddr = ((pNftl->StartBlkPart + Pba) * pDev->BlockPageSize) + Page;
        if (OK != AmbaNandOp_Read(PageAddr, 1, pNftl->pMainDebug, pNftl->pSpareDebug, 5000U)) {
            NftlUpdateBBInfo(pNftl, Pba);
            NftlUpdateBBTable(pNftl, Pba);
            pNftl->BBM.FailRdCnt++;
            RetStatus = NAND_OP_READ_ER;
        } else {
            INT32 Cnt, SubSpareSize;
            /* main area verify */
            Cnt = memcmp(pNftl->pMainBuf, pNftl->pMainDebug, pDev->MainByteSize);
            if (Cnt > 0) {
                //PRINT("%s: %s main area verify fail (Pba %d, Page %d, Sec %d, Secs %d)", g_NftlPart[pNftl->ID], __FUNCTION__, Pba, Page, Sec, Secs);
            } else {
                if (EccBits == NAND_ECC_BIT_8) {
                    SubSpareSize = 32;
                } else {
                    SubSpareSize = 16;
                }

                /* The bytes after the 6th bytes are ECC bytes. */
                for (Cnt = 0, i = 0; i < pDev->SpareByteSize; i += SubSpareSize) {
                    RetStatus = memcmp(pNftl->pSpareBuf + i, pNftl->pSpareDebug + i, 0x6);
                    Cnt += RetStatus;
                }

                if (Cnt > 0) {
                    //PRINT("%s: %s spare area verify fail (Pba %d, Page %d, Sec %d, Secs %d)", g_NftlPart[pNftl->ID], __FUNCTION__, Pba, Page, Sec, Secs);
                }
            }
        }
    }
#endif

    return RetStatus;
}

/*
 *  @RoutineName:: NftlWriteSecsCb
 *
 *  @Description:: Write data to the specified sectors including lba in spare area and
 *                  copyback the other sectors in the same page.
 *                  Only for 2k page NAND flash.
 *
 *  @Input      ::
 *          AMBA_NFTL_OBJ_s *pNftl: Pointer to NFTL object
 *          UINT8 *pBuf         : Pointer to buffer where write data is stored
 *          UINT32 Lba          : Absolutely logical block address
 *          UINT32 Pba          : Absolutely physical block address
 *          UINT32 CPba         : Absolutely physical block address to be copied back
 *          UINT32 Page         : Page address in the block
 *          UINT32 Sec          : Sector address in page
 *          UINT32 Secs         : Number of sectors to be writen
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
static INT32 NftlWriteSecsCb(AMBA_NFTL_OBJ_s *pNftl, const UINT8 *pBuf, UINT32 Lba, UINT32 Pba,
                             UINT32 CPba, UINT32 Page, UINT32 Sec, UINT32 Secs)
{

    NAND_SPARE_LAYOUT_LARGE_s *pLargeSpr = NULL;
    INT32 i, RetStatus = 0;
    const AMBA_NAND_DEV_INFO_s *pDev = AmbaNAND_GetCommonInfo();
    UINT32 PageAddr;

    if (!((Sec + Secs) <= g_Nftl_Info[pNftl->ID].SectorsPerPage) || !(Page < pDev->BlockPageSize)) {
        RetStatus = NG;
    }

    if (RetStatus == 0) {

        //DBGMSG("%s: %s(..., %d, %d)", g_NftlPart[pNftl->ID], __FUNCTION__, Sec, Secs);

        Lba %= pNftl->LBlksPerZone;

        /* Read back main area */
        PageAddr = ((pNftl->StartBlkPart + CPba) * pDev->BlockPageSize) + Page;
        if (OK != AmbaNandOp_Read(PageAddr, 1, pNftl->pMainBuf, NULL, 5000U)) {
            NftlUpdateBBInfo(pNftl, CPba);
            NftlUpdateBBTable(pNftl, CPba);
            pNftl->BBM.FailRdCnt++;
            RetStatus = NAND_OP_READ_ER;
        }

        if (RetStatus == 0) {
            (void)AmbaWrap_memcpy(&pNftl->pMainBuf[Sec << NAND_SEC_SHT], pBuf, Secs << NAND_SEC_SHT);
            (void)AmbaWrap_memset(pNftl->pSpareBuf, 0xff, pDev->SpareByteSize);

            AmbaMisra_TypeCast32(&pLargeSpr, &pNftl->pSpareBuf);

            /* Filled page used bit to spare area.*/
            pLargeSpr->Pub = 0x0;

            for (i = 0; i < (INT32)g_Nftl_Info[pNftl->ID].SectorsPerPage; i++) {
                pLargeSpr[i].Lsn[0] = (UINT8) Lba;
                pLargeSpr[i].Lsn[1] = (UINT8) ((Lba & 0xff00U) >> 8U);
#ifdef WRITE_ALL_PUB
                pLargeSpr[i].Pub    = 0x0;
#endif
            }

            /* Dual-space mode can write main and spare area, simultaneously. */
            PageAddr = ((pNftl->StartBlkPart + Pba) * pDev->BlockPageSize) + Page;
            if (OK != AmbaNandOp_Program(PageAddr, 1, pNftl->pMainBuf, pNftl->pSpareBuf, 5000U)) {
                NftlUpdateBBInfo(pNftl, Pba);
                NftlUpdateBBTable(pNftl, Pba);
                pNftl->BBM.FailWrCnt++;
                RetStatus = NAND_OP_PROG_ER;
            }
        }
    }
#ifdef ENABLE_VERIFY_NFTL
    if (RetStatus == 0) {
        PageAddr = ((pNftl->StartBlkPart + Pba) * pDev->BlockPageSize) + Page;
        RetStatus = AmbaNandOp_Read(PageAddr, 1, pNftl->pMainDebug, pNftl->pSpareDebug, 5000U);

        if (RetStatus < 0) {
            NftlUpdateBBInfo(pNftl, Pba);
            NftlUpdateBBTable(pNftl, Pba);
            pNftl->BBM.FailRdCnt++;
        } else {
            INT32 Cnt, SubSpareSize;
            Cnt = memcmp(pNftl->pMainBuf, pNftl->pMainDebug, pDev->MainByteSize);
            if (Cnt > 0) {
                //PRINT("%s: %s main area verify fail (Pba %d, Page %d, pages %d)", g_NftlPart[pNftl->ID], __FUNCTION__, Pba, Page, 1);
            }

            if (!(Cnt > 0)) {
                if (EccBits == NAND_ECC_BIT_8) {
                    SubSpareSize = 32;
                } else {
                    SubSpareSize = 16;
                }

                /* The bytes after the 6th bytes are ECC bytes. */
                for (Cnt = 0, i = 0; i < pDev->SpareByteSize; i += SubSpareSize) {
                    RetStatus = memcmp(pNftl->pSpareBuf + i, pNftl->pSpareDebug + i, 0x6);
                    Cnt += RetStatus;
                }
                if (Cnt > 0) {
                    //PRINT("%s: %s spare area verify fail (Pba %d, Page %d, pages %d)", g_NftlPart[pNftl->ID], __FUNCTION__, Pba, Page, 1);
                }
            }
        }
    }
#endif

    return RetStatus;
}

/*
 *  @RoutineName:: NftlWritePage
 *
 *  @Description:: Write data to the specified pages including lba in spare area.
 *
 *  @Input      ::
 *          AMBA_NFTL_OBJ_s *pNftl: Pointer to NFTL object
 *          UINT8 *pBuf         : Pointer to buffer where write data is stored
 *          UINT32 Lba          : Absolutely logical block address
 *          UINT32 Pba          : Absolutely physical block address
 *          UINT32 Page         : Page address in the block
 *          UINT32 Pages        : Pages to be written in block
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
static INT32 NftlWritePage(AMBA_NFTL_OBJ_s *pNftl, UINT8 *pBuf, UINT32 Lba, UINT32 Pba,
                           UINT32 Page, UINT32 Pages)
{
    NAND_SPARE_LAYOUT_LARGE_s *pLargeSpr = NULL;
    INT32 RetStatus = 0;
    UINT32 i, j, PageAddr = 0U;
    const AMBA_NAND_DEV_INFO_s *pDev = AmbaNAND_GetCommonInfo();

    if (!(Pages <= pDev->BlockPageSize)) {
        RetStatus = NG;
    }

    if (RetStatus == 0) {
        //DBGMSG("%s: %s(..., %d, %d)", g_NftlPart[pNftl->ID], __FUNCTION__, Page, Pages);

        Lba %= pNftl->LBlksPerZone;

        (void)AmbaWrap_memset(pNftl->pSpareBuf, 0xff, Pages * pDev->SpareByteSize);

        if (pNftl->PageType == NAND_TYPE_2K) {
            for (i = 0; i < Pages; i++) {
                const UINT8 *pSpareBuf = &pNftl->pSpareBuf[i * pDev->SpareByteSize];
                AmbaMisra_TypeCast32(&pLargeSpr, &pSpareBuf);

                /* Filled page used bit to spare area.*/
                pLargeSpr->Pub = 0x0;

                for (j = 0; j < g_Nftl_Info[pNftl->ID].SectorsPerPage; j++) {
                    pLargeSpr[j].Lsn[0] = (UINT8) Lba;
                    pLargeSpr[j].Lsn[1] = (UINT8) ((Lba & 0xff00U) >> 8);
#ifdef WRITE_ALL_PUB
                    pLargeSpr[j].Pub = 0x0;
#endif
                }
            }
        } else if (pNftl->PageType == NAND_TYPE_4K) {

            for (i = 0; i < Pages; i++) {
                const UINT8 *pSpareBuf = &pNftl->pSpareBuf[i * pDev->SpareByteSize];
                AmbaMisra_TypeCast32(&pLargeSpr, &pSpareBuf);

                /* Filled page used bit to spare area.*/
                pLargeSpr->Pub = 0x0;

                for (j = 0; j < g_Nftl_Info[pNftl->ID].SectorsPerPage; j++) {
                    pLargeSpr[j].Lsn[0] = (UINT8) Lba;
                    pLargeSpr[j].Lsn[1] = (UINT8) ((Lba & 0xff00U) >> 8);
                }
            }
        } else {
            RetStatus = NG;
        }
    }
    if (RetStatus == 0) {
        /* Dual-space mode can write main and spare area, simultaneously. */
        PageAddr = ((pNftl->StartBlkPart + Pba) * pDev->BlockPageSize) + Page;
        if (OK != AmbaNandOp_Program(PageAddr, Pages, pBuf, pNftl->pSpareBuf, 5000U)) {
            NftlUpdateBBInfo(pNftl, Pba);
            NftlUpdateBBTable(pNftl, Pba);
            pNftl->BBM.FailWrCnt++;
            RetStatus = NAND_OP_PROG_ER;
        }
    }
#ifdef ENABLE_VERIFY_NFTL

    if (RetStatus == 0) {
        PageAddr = ((pNftl->StartBlkPart + Pba) * pDev->BlockPageSize) + Page;
        RetStatus = AmbaNandOp_Read(PageAddr, Pages, pNftl->pMainDebug,
                                    pNftl->pSpareDebug, 5000U);
    }

    if (RetStatus < 0) {
        NftlUpdateBBInfo(pNftl, Pba);
        NftlUpdateBBTable(pNftl, Pba);
        pNftl->BBM.FailRdCnt++;
    } else {
        INT32 Cnt, SubSpareSize;
        /* verify main area */
        Cnt = memcmp(pBuf, pNftl->pMainDebug, Pages * pDev->MainByteSize);
        if (Cnt > 0) {
            //PRINT("%s: %s main area verify fail (pba %d, page %d, pages %d)", g_NftlPart[pNftl->ID], __FUNCTION__, Pba, Page, Pages);
        } else {

            if (EccBits == NAND_ECC_BIT_8) {
                SubSpareSize = 32;
            } else {
                SubSpareSize = 16;
            }

            /* verify spare area */
            for (i = 0; i < Pages; i++) {
                RetStatus = 0;
                /* The bytes after the 6th bytes are ECC bytes. */
                for (j = 0; j < pDev->SpareByteSize; j += SubSpareSize) {
                    Cnt = memcmp(pNftl->pSpareBuf + (pDev->SpareByteSize * i) + j,
                                 pNftl->pSpareDebug + (pDev->SpareByteSize * i) + j,
                                 0x6);

                    RetStatus += Cnt;
                }

                if (RetStatus > 0) {
                    //PRINT("%s: %s spare area verify fail (pba %d, page %d, pages %d)", g_NftlPart[pNftl->ID], __FUNCTION__, Pba, Page, Pages);
                }
            }
        }
    }
#endif

    return RetStatus;
}

/*
 *  @RoutineName:: NftlWritePSecsStartBlock
 *
 *  @Description:: Write the sectors in the first block and the start sector address is at the sectors boundry.
 *
 *  @Input      ::
 *          AMBA_NFTL_OBJ_s *pNftl: Pointer to NFTL object
 *          AMBA_NFTL_ARGS_s *pArg: Pointer to the parameter structure
 *          UINT8 *pBuf         : Pointer to buffer where write data is stored
 *          UINT32 Pba          : Absolutely physical block address
 *          UINT32 CPba         : Absolutely physical block address to be copied back.
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
static INT32 NftlWritePSecsStartBlock(AMBA_NFTL_OBJ_s *pNftl, AMBA_NFTL_ARGS_s *pArg, UINT8* pBuf,
                                      UINT32 Pba, UINT32 CPba)
{
    INT32 RetStatus = 0;
    const AMBA_NAND_DEV_INFO_s *pDev = AmbaNAND_GetCommonInfo();
    UINT32 BufIdx = 0U;

    //DBGMSG("%s: %s(..., %d, %d)", g_NftlPart[pNftl->ID], __FUNCTION__, Pba, CPba);

    if (pArg->SecsStart > 0U) {
        /* Process the sectors to be writen into start of first block */
        if (CPba == NFTL_UNUSED_BLK) {
            RetStatus = NftlWriteSecs(pNftl, &pBuf[BufIdx], pArg->Lba, Pba, pArg->Page,
                                      pArg->SecNum, pArg->SecsStart);
        } else {
            RetStatus = NftlWriteSecsCb(pNftl, &pBuf[BufIdx], pArg->Lba, Pba, CPba,
                                        pArg->Page, pArg->SecNum, pArg->SecsStart);
        }
        if (RetStatus >= 0) {
            BufIdx += pArg->SecsStart << NAND_SEC_SHT;
            pArg->Page++;
        }
    }

    if ((RetStatus >= 0) &&
        (pArg->PagesStartBlk > 0U)) {
        /* Process the pages to be writen into first block */
        RetStatus = NftlWritePage(pNftl, &pBuf[BufIdx], pArg->Lba, Pba, pArg->Page,
                                  pArg->PagesStartBlk);
        if (RetStatus >= 0) {
            BufIdx += pArg->PagesStartBlk * pDev->MainByteSize;
            pArg->Page += pArg->PagesStartBlk;
        }
    }

    if ((RetStatus >= 0) &&
        (pArg->SecsEndBlk > 0U)) {
        /* Process the sectors to be writen into end of first block. */
        if (CPba == NFTL_UNUSED_BLK) {
            RetStatus = NftlWriteSecs(pNftl, &pBuf[BufIdx], pArg->Lba, Pba, pArg->Page,
                                      0, pArg->SecsEndBlk);
        } else {
            RetStatus = NftlWriteSecsCb(pNftl, &pBuf[BufIdx], pArg->Lba, Pba, CPba,
                                        pArg->Page, 0, pArg->SecsEndBlk);
        }
        if (RetStatus >= 0) {
            pArg->Page++;
        }
    }

    return RetStatus;
}

/*
 *  @RoutineName:: NftlWritePSecsStartBlock
 *
 *  @Description::  Write the sectors in the last block and the start sector address is at the block boundry.
 *
 *  @Input      ::
 *          AMBA_NFTL_OBJ_s *pNftl: Pointer to NFTL object
 *          AMBA_NFTL_ARGS_s *pArg: Pointer to the parameter structure
 *          UINT8 *pBuf         : Pointer to buffer where write data is stored
 *          UINT32 Pba          : Absolutely physical block address
 *          UINT32 CPba         : Absolutely physical block address to be copied back.
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
static INT32 NftlWritePSecsEndBlock(AMBA_NFTL_OBJ_s *pNftl, AMBA_NFTL_ARGS_s *pArg, UINT8 *pBuf,
                                    UINT32 Pba, UINT32 CPba)
{
    INT32 RetStatus = 0;
    UINT32 Index = 0;
    const AMBA_NAND_DEV_INFO_s *pDev = AmbaNAND_GetCommonInfo();

    //DBGMSG("%s: %s(..., %d, %d)", g_NftlPart[pNftl->ID], __FUNCTION__, Pba, CPba);

    pArg->Page = 0;

    if (pArg->PagesEnd != 0U) {
        /* Process the pages to be writen in last block */
        RetStatus = NftlWritePage(pNftl, pBuf, pArg->Lba, Pba, pArg->Page, pArg->PagesEnd);
        if (RetStatus >= 0) {
            Index = pArg->PagesEnd * pDev->MainByteSize;
            pArg->Page += pArg->PagesEnd;
        }
    }

    if (RetStatus >= 0) {
        if (pArg->SecsEndPage != 0U) {
            /* Process the sectors to be writen in end of first block. */
            /* This is the last sectors to be writen and do not to update arguments. */
            if (CPba == NFTL_UNUSED_BLK) {
                RetStatus = NftlWriteSecs(pNftl, &pBuf[Index], pArg->Lba, Pba, pArg->Page, 0, pArg->SecsEndPage);
            } else {
                RetStatus = NftlWriteSecsCb(pNftl, &pBuf[Index], pArg->Lba, Pba, CPba, pArg->Page, 0, pArg->SecsEndPage);
            }
        }
    }
    return RetStatus;
}

static INT32 NftlWriteCopyDownUsedPages(AMBA_NFTL_OBJ_s *pNftl, AMBA_NFTL_ARGS_s *pArg, UINT32 Sec, UINT32 Secs, UINT32 Pba, UINT32 Eba, const UINT32 *pPat)
{
    UINT32 i, j, k, s, n, r;
    UINT32 Up, Down;
    INT32 RetStatus = 0, Stop = 0;
    const AMBA_NAND_DEV_INFO_s *pDev = AmbaNAND_GetCommonInfo();
    UINT32 DestPageAddr, SrcPageAddr;

    /* copyback the used pages from original Pba */
    /* total number of 32-bit pattern */
    n = g_Nftl_Info[pNftl->ID].SectorsPerBlock >> 5U;;
    Down = Sec - pArg->SecNum;

    r = (Sec + Secs) & (NAND_SPP_4 - 1U);
    if (r == 0U) {
        Up = Sec + Secs;
    } else {
        Up = Sec + Secs + (NAND_SPP_4 - (UINT32)r);
    }

    if (!((Up & (NAND_SPP_4 - 1U)) == 0U)) {
        RetStatus = -1;
    } else {

        for (i = 0; i < n; i++) {
            for (j = 0; j < 32U;) {
                s = (i << 5U) + j;  /* sector */
                k = s >> NAND_SPP_4_SHT;    /* page */

                if (s < Down) {
                    if (((pPat[i] >> j) & 0x1U) != 0U) {
                        DestPageAddr = ((pNftl->StartBlkPart + (UINT32)Eba) * pDev->BlockPageSize) + k;
                        SrcPageAddr  = ((pNftl->StartBlkPart + (UINT32)Pba) * pDev->BlockPageSize) + k;
                        if (OK != AmbaNandOp_Copyback(DestPageAddr, SrcPageAddr, 5000U)) {
                            NftlUpdateBBInfo(pNftl, Eba);
                            NftlUpdateBBTable(pNftl, Eba);
                            pNftl->BBM.FailWrCnt++;
                            RetStatus = NftlBadBlockHandler(pNftl, pArg, NAND_OP_PROG_ER);
                            Stop = 1;
                            break;
                        } else {
                            RetStatus = 0;
                        }

                        /* update index to next page */
                        j = (((k + 1U) << NAND_SPP_4_SHT) - 1U) & (0x1fU);
                    }
                }
                j++;
            }
            if (Stop != 0) {
                break;
            }
        }
    }

    return RetStatus;
}

static INT32 NftlWriteCopyUpUsedPages(AMBA_NFTL_OBJ_s *pNftl, AMBA_NFTL_ARGS_s *pArg, UINT32 Sec, UINT32 Secs, UINT32 Pba, UINT32 Eba, const UINT32 *pPat)
{
    INT32 Stop = 0;
    INT32 RetStatus = 0;
    UINT32 i, j, k, n, r, s, Up;
    const AMBA_NAND_DEV_INFO_s *pDev = AmbaNAND_GetCommonInfo();
    UINT32 DestPageAddr, SrcPageAddr;

    /* copyback the used pages from original Pba */
    /* total number of 32-bit pattern */
    n = g_Nftl_Info[pNftl->ID].SectorsPerBlock >> 5;

    r = (Sec + Secs) & (NAND_SPP_4 - 1U);
    if (r == 0U) {
        Up = Sec + Secs;
    } else {
        Up = Sec + Secs + (NAND_SPP_4 - r);
    }

    if (!((Up & (NAND_SPP_4 - 1U)) == 0U)) {
        RetStatus = -1;
    } else {

        for (i = 0; i < n; i++) {
            for (j = 0; j < 32U;) {
                s = (i << 5U) + j;  /* sector */
                k = s >> NAND_SPP_4_SHT;    /* page */

                if (s >= Up) {
                    if (((pPat[i] >> j) & 0x1U) != 0U) {
                        DestPageAddr = ((pNftl->StartBlkPart + (UINT32)Eba) * pDev->BlockPageSize) + k;
                        SrcPageAddr  = ((pNftl->StartBlkPart + (UINT32)Pba) * pDev->BlockPageSize) + k;
                        if (OK != AmbaNandOp_Copyback(DestPageAddr, SrcPageAddr, 5000U)) {
                            NftlUpdateBBInfo(pNftl, Eba);
                            NftlUpdateBBTable(pNftl, Pba);
                            pNftl->BBM.FailWrCnt++;
                            RetStatus = NftlBadBlockHandler(pNftl, pArg, NAND_OP_PROG_ER);
                            Stop = 1;
                            break;
                        } else {
                            RetStatus = 0;
                        }

                        /* update index to next page */
                        j = (((k + 1U) << NAND_SPP_4_SHT) - 1U) & (0x1fU);
                    }
                }
                j++;
            }
            if (Stop != 0) {
                break;
            }
        }
    }

    return RetStatus;
}

static INT32 NftlWriteCopyDownUsedPages4k(AMBA_NFTL_OBJ_s *pNftl, AMBA_NFTL_ARGS_s *pArg, UINT32 Sec, UINT32 Secs, UINT32 Pba, UINT32 Eba, const UINT32 *pPat)
{
    const AMBA_NAND_DEV_INFO_s *pDev = AmbaNAND_GetCommonInfo();
    INT32 Stop = 0;
    INT32 RetStatus = 0;
    UINT32 i, j, k, s, n, r;
    UINT32 Up, Down;
    UINT32 DestPageAddr, SrcPageAddr;

    UINT32 SecPerPage = NAND_SPP_8;
    UINT32 SecPerPageShift = NAND_SPP_8_SHT;

    /* total number of 32-bit pattern */
    n = (g_Nftl_Info[pNftl->ID].SectorsPerBlock / 32U) / 2U;
    Down = Sec - pArg->SecNum;

    r = (Sec + Secs) & (SecPerPage - 1U);
    if (r == 0U) {
        Up = Sec + Secs;
    } else {
        Up = Sec + Secs + (SecPerPage - r);
    }

    //K_ASSERT((Up & (SecPerPage - 1)) == 0);
    AmbaMisra_TouchUnused(&Up);

    for (i = 0; i < n; i++) {
        for (j = 0; j < 32U; j += 4U) {
            s = (i << 6U) + (j * 2U);     /* sector */
            k = s >> SecPerPageShift;     /* page */

            if (s < Down) {

                if (((pPat[i] >> j) & 0xFU) != 0U) {
                    DestPageAddr = ((pNftl->StartBlkPart + (UINT32)Eba) * pDev->BlockPageSize) + k;
                    SrcPageAddr = ((pNftl->StartBlkPart + (UINT32)Pba) * pDev->BlockPageSize) + k;
                    if (OK != AmbaNandOp_Copyback(DestPageAddr, SrcPageAddr, 5000U)) {
                        NftlUpdateBBInfo(pNftl, (UINT32)Eba);
                        NftlUpdateBBTable(pNftl, (UINT32)Eba);
                        pNftl->BBM.FailWrCnt++;
                        RetStatus = NftlBadBlockHandler(pNftl, pArg, NAND_OP_PROG_ER);
                        Stop = 1;
                        break;
                    } else {
                        RetStatus = 0;
                    }
                    /* update index to next page */
                    // j = (((k + 1) << SecPerPageShift) - 1) & (0x1f);
                }
            }
        }
        if (Stop != 0) {
            break;
        }
    }
    return RetStatus;
}

static INT32 NftlWriteCopyUpUsedPages4k(AMBA_NFTL_OBJ_s *pNftl, AMBA_NFTL_ARGS_s *pArg, UINT32 Sec, UINT32 Secs, UINT32 Pba, UINT32 Eba, const UINT32 *pPat)
{
    const AMBA_NAND_DEV_INFO_s *pDev = AmbaNAND_GetCommonInfo();
    INT32 Stop = 0;
    INT32 RetStatus = 0;
    UINT32 i, j, k, s, n, r;
    UINT32 Up;
    UINT32 DestPageAddr, SrcPageAddr;

    UINT32 SecPerPage = NAND_SPP_8;
    UINT32 SecPerPageShift = NAND_SPP_8_SHT;

    /* total number of 32-bit pattern */
    n = (g_Nftl_Info[pNftl->ID].SectorsPerBlock / 32U) / 2U;
    r = (Sec + Secs) & (SecPerPage - 1U);

    if (r == 0U) {
        Up = Sec + Secs;
    } else {
        Up = Sec + Secs + (SecPerPage - r);
    }

    //K_ASSERT((Up & (SecPerPage - 1)) == 0);

    for (i = 0; i < n; i++) {
        for (j = 0; j < 32U; j += 4U) {
            s = (i << 6U) + (j * 2U);        /* sector */
            k = s >> SecPerPageShift;    /* page */

            if (s >= Up) {

                if (((pPat[i] >> j) & 0xFU)  != 0U) {
                    DestPageAddr = ((pNftl->StartBlkPart + (UINT32)Eba) * pDev->BlockPageSize) + k;
                    SrcPageAddr = ((pNftl->StartBlkPart + (UINT32)Pba) * pDev->BlockPageSize) + k;
                    if (OK != AmbaNandOp_Copyback(DestPageAddr, SrcPageAddr, 5000U)) {
                        NftlUpdateBBInfo(pNftl, (UINT32)Eba);
                        NftlUpdateBBTable(pNftl, (UINT32)Eba);
                        pNftl->BBM.FailWrCnt++;
                        RetStatus = NftlBadBlockHandler(pNftl, pArg, NAND_OP_PROG_ER);
                        Stop = 1;
                        break;
                    } else {
                        RetStatus = 0;
                    }
                    /* update index to next page */
                    // j = (((k + 1) << SecPerPageShift) - 1) & (0x1f);
                }
            }
        }
        if (Stop != 0) {
            break;
        }
    }
    return RetStatus;
}

static INT32 NftlEraseUnusedBlockUpdateTbl(AMBA_NFTL_OBJ_s *pNftl, AMBA_NFTL_ARGS_s *pArg, UINT32 Pba, INT32 Eba)
{
    INT32 RetStatus = 0;
    if (Pba != NFTL_UNUSED_BLK) {
        /* Erase old block and update tables. If erase block fail, */
        /* the block will be marked as bad block later and tables are still need to be updated. */
        if (OK != AmbaNandOp_EraseBlock(pNftl->StartBlkPart + Pba, 5000)) {
            RetStatus = NAND_OP_ERASE_ER;
        }

        if (RetStatus < 0) {
            NftlUpdateBBInfo(pNftl, Pba);
            NftlUpdateBBTable(pNftl, Pba);
            pNftl->BBM.FailErCnt++;
        }

        NftlUpdatePhyTable(pNftl, Pba, NFTL_UNUSED_BLK);
    }

    NftlUpdateLogicTable(pNftl, pArg->Lba, (UINT32)Eba);
    NftlUpdatePhyTable(pNftl, (UINT32)Eba, pArg->Lba);

    pArg->Lba++;

    if (RetStatus < 0) {
        RetStatus = NftlBadBlockHandler(pNftl, pArg, RetStatus);
    }
    return RetStatus;
}

static INT32 NftlWritePSecsCopyUsedPage(AMBA_NFTL_OBJ_s *pNftl, AMBA_NFTL_ARGS_s *pArg, UINT8 *pBuf,
                                        const UINT32 *pPat, UINT32 Pba, INT32 Eba, UINT32 Sec, UINT32 Secs, UINT32 SecPerPage)
{
    INT32 RetStatus = 0;

    /* AT FIRST, write used sectors bit pattern to spare area. */
    if (Pba != NFTL_UNUSED_BLK) {

    } else {
        /* Skip page 0 (sector 0~3) to make this func auto fill the lba to page 0. */
        UINT32 TmpSec = Sec & (~(SecPerPage - 1U));
        if (((INT32)TmpSec - (INT32)SecPerPage) >= 0) {
            if (OK != NftlWriteUsedSecPat(pNftl, (UINT32)Eba, pArg->Lba, SecPerPage, (Sec & ~(SecPerPage - 1U)) - SecPerPage, NULL)) {
                RetStatus = NftlBadBlockHandler(pNftl, pArg, RetStatus);
            }
        }
    }

    /* copyback the used pages from original Pba (page smaller than pArg->page) */
    if ((RetStatus >= 0) &&
        ((pNftl->PageType == NAND_TYPE_2K) && (Pba != NFTL_UNUSED_BLK))) {
        RetStatus = NftlWriteCopyDownUsedPages(pNftl, pArg, Sec, Secs, Pba, (UINT32)Eba, pPat);
    } else if ((RetStatus >= 0) &&
               (pNftl->PageType == NAND_TYPE_4K) && (Pba != NFTL_UNUSED_BLK)) {
        RetStatus = NftlWriteCopyDownUsedPages4k(pNftl, pArg, Sec, Secs, Pba, (UINT32)Eba, pPat);
    } else {
        /*  */
    }

    if (RetStatus == 0) {
        RetStatus = NftlWritePSecsStartBlock(pNftl, pArg, pBuf, (UINT32)Eba, Pba);
        if (RetStatus < 0) {
            RetStatus = NftlBadBlockHandler(pNftl, pArg, RetStatus);
        }

        /* copyback the used pages from original Pba (page larger than pArg->page) */
        if ((RetStatus >= 0) &&
            ((pNftl->PageType == NAND_TYPE_2K) && (Pba != NFTL_UNUSED_BLK))) {
            RetStatus = NftlWriteCopyUpUsedPages(pNftl, pArg, Sec, Secs, Pba, (UINT32)Eba, pPat);

        } else if ((RetStatus >= 0) &&
                   (pNftl->PageType == NAND_TYPE_4K) && (Pba != NFTL_UNUSED_BLK)) {
            RetStatus = NftlWriteCopyUpUsedPages4k(pNftl, pArg, Sec, Secs, Pba, (UINT32)Eba, pPat);
        } else {
            /* For MisraC checking. */
        }

        if (RetStatus == 0) {
            RetStatus = NftlEraseUnusedBlockUpdateTbl(pNftl, pArg, Pba, Eba);
        }
    }
    return RetStatus;
}

/*
 *  @RoutineName:: NftlWritePSecsNewBlock
 *
 *  @Description:: Write the sectors in the first block and the start sector address is
 *              at the sectors boundry.
 *              The writen block is newly allocated to LBA and need to copyback pages from
 *              previous allocated block.
 *
 *  @Input      ::
 *          AMBA_NFTL_OBJ_s *pNftl: Pointer to NFTL object
 *          AMBA_NFTL_ARGS_s *pArg: Pointer to the parameter structure
 *          UINT8 *pBuf         : Pointer to buffer where write data is stored
 *          UINT32 *pPat        : Used sectors bit pattern corresponding to pba
 *          UINT32 Pba          : Absolutely physical block address
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
static INT32 NftlWritePSecsNewBlock(AMBA_NFTL_OBJ_s *pNftl, AMBA_NFTL_ARGS_s *pArg, UINT8 *pBuf,
                                    UINT32 *pPat, UINT32 Pba)
{
    UINT32 Sec, Secs;
    INT32 Eba = 0, RetStatus = 0;
    UINT32 SecPerPage;

    //DBGMSG("%s: %s(..., %d)", g_NftlPart[pNftl->ID], __FUNCTION__, Pba);

    if (pNftl->PageType == NAND_TYPE_2K) {
        SecPerPage = NAND_SPP_4;
    } else {
        SecPerPage = NAND_SPP_8;
    }

    NftlSaveParam(pNftl, pArg, pBuf, pPat, Pba, NFTL_PSECTS_NEW_BLOCK);

    Sec = (pArg->Page * g_Nftl_Info[pNftl->ID].SectorsPerPage) + pArg->SecNum;
    Secs = (pArg->PagesStartBlk * g_Nftl_Info[pNftl->ID].SectorsPerPage) + pArg->SecsStart + pArg->SecsEndBlk;

    if (!(((Sec + Secs) <= g_Nftl_Info[pNftl->ID].SectorsPerBlock))) {
        RetStatus = NG;
    }

    if (RetStatus == 0) {

        if (Pba == NFTL_UNUSED_BLK) {
            Eba = NftlSearchErasedBlock(pNftl, pArg->Lba);
        } else {
            Eba = NftlSearchErasedBlockCb(pNftl, pArg->Lba, Pba);
        }

        if (Eba < 0) {
            RetStatus = NftlBadBlockHandler(pNftl, pArg, RetStatus);
        }
    }

    if ((RetStatus == 0) &&
        (Eba >= 0)) {
        RetStatus = NftlWritePSecsCopyUsedPage(pNftl, pArg, pBuf, pPat, Pba, Eba, Sec, Secs, SecPerPage);
    }
    return RetStatus;
}

/*
 *  @RoutineName:: NftlWrPSecsNewBlock
 *
 *  @Description:: Write the sectors in the first block and the start sector address is at
 *              the sectors boundry.
 *              The writen block has already allocated to LBA and the wirten sectors is unused,
 *              so it do not need to copyback pages from previous allocated block.
 *
 *  @Input      ::
 *          AMBA_NFTL_OBJ_s *pNftl: Pointer to NFTL object
 *          AMBA_NFTL_ARGS_s *pArg: Pointer to the parameter structure
 *          UINT8 *pBuf         : Pointer to buffer where write data is stored
 *          UINT32 *pPat        : Used sectors bit pattern corresponding to pba
 *          UINT32 Pba          : Absolutely physical block address
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
static INT32 NftlWritePSecs(AMBA_NFTL_OBJ_s *pNftl, AMBA_NFTL_ARGS_s *pArg, UINT8* pBuf,
                            UINT32 *pPat, UINT32 Pba)
{
    UINT32 Sec, Secs;
    INT32 RetStatus = 0;
    UINT32 SecPerPage;

    if (pNftl->PageType == NAND_TYPE_2K) {
        SecPerPage = NAND_SPP_4;
    } else {
        SecPerPage = NAND_SPP_8;
    }

    //DBGMSG("%s: %s(..., %d)", g_NftlPart[pNftl->ID], __FUNCTION__, Pba);

    NftlSaveParam(pNftl, pArg, pBuf, pPat, Pba, NFTL_PSECTS);

    Sec = (pArg->Page * g_Nftl_Info[pNftl->ID].SectorsPerPage) + pArg->SecNum;
    Secs = (pArg->PagesStartBlk * g_Nftl_Info[pNftl->ID].SectorsPerPage) +
           pArg->SecsStart + pArg->SecsEndBlk;

    if (!((Sec + Secs) <= g_Nftl_Info[pNftl->ID].SectorsPerBlock)) {
        RetStatus = NG;
    } else {
        /* AT FIRST, write used sectors bit pattern to spare area. */
        /* Skip page 0 (sector 0~3) to make this func auto fill the lba to page 0. */
        UINT32 TmpSec = Sec & (~(SecPerPage - 1U));
        if (((INT32)TmpSec - (INT32)SecPerPage) >= 0) {
            if (OK != NftlWriteUsedSecPat(pNftl, Pba, pArg->Lba, SecPerPage, (Sec & ~(SecPerPage - 1U)) - SecPerPage, NULL)) {
                RetStatus = NAND_OP_PROG_ER;
            }
        }

        if (RetStatus >= 0) {
            RetStatus = NftlWritePSecsStartBlock(pNftl, pArg, pBuf, Pba, NFTL_UNUSED_BLK);
            if (RetStatus < 0) {
                RetStatus = NAND_OP_PROG_ER;
            }
        }

        if (RetStatus < 0) {
            RetStatus = NftlBadBlockHandler(pNftl, pArg, RetStatus);
        } else {
            pArg->Lba++;
        }
    }
    return RetStatus;
}

/*
 *  @RoutineName:: NftlWriteBlock
 *
 *  @Description:: Write the blocks from the block boundry address.
 *
 *  @Input      ::
 *          AMBA_NFTL_OBJ_s *pNftl: Pointer to NFTL object
 *          AMBA_NFTL_ARGS_s *pArg: Pointer to the parameter structure
 *          UINT8 *pBuf         : Pointer to buffer where write data is stored
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
static INT32 NftlWriteBlock(AMBA_NFTL_OBJ_s *pNftl, AMBA_NFTL_ARGS_s *pArg, UINT8 *pBuf)
{
    UINT32 Pba, Index = 0;
    INT32 Eba = 0, RetStatus = 0;
    const AMBA_NAND_DEV_INFO_s *pDev = AmbaNAND_GetCommonInfo();

    //DBGMSG("%s: %s(...)", g_NftlPart[pNftl->ID], __FUNCTION__);

    for (; pArg->Blocks > 0U; pArg->Blocks--) {
        Pba = NftlGetPba(pNftl, pArg->Lba);
        Eba = NftlSearchErasedBlock(pNftl, pArg->Lba);
        if (Eba < 0) {
            RetStatus = NG;
        } else {
            RetStatus = NftlWritePage(pNftl, &pBuf[Index], pArg->Lba, (UINT32)Eba, 0, pDev->BlockPageSize);
            if (RetStatus < 0) {
                RetStatus = NAND_OP_PROG_ER;
            }
        }

        if (RetStatus < 0) {
            break;
        }

        if (Pba != NFTL_UNUSED_BLK) {
            if (OK != AmbaNandOp_EraseBlock(pNftl->StartBlkPart + Pba, 5000)) {
                RetStatus = NAND_OP_ERASE_ER;
                NftlUpdateBBInfo(pNftl, Pba);
                NftlUpdateBBTable(pNftl, Pba);
                pNftl->BBM.FailErCnt++;
            }

            NftlUpdatePhyTable(pNftl, Pba, NFTL_UNUSED_BLK);
        }

        NftlUpdateLogicTable(pNftl, pArg->Lba, (UINT32)Eba);
        NftlUpdatePhyTable(pNftl, (UINT32)Eba, pArg->Lba);
        pArg->Lba++;
        Index += (pDev->MainByteSize * pDev->BlockPageSize);
    }

    if ((RetStatus < 0) && (Eba >= 0)) {
        NftlSaveParam(pNftl, pArg, &pBuf[Index], NULL, NFTL_UNUSED_BLK, NFTL_BLOCKS);
        RetStatus = NftlBadBlockHandler(pNftl, pArg, RetStatus);
    }

    return RetStatus;
}

/*
 *  @RoutineName:: NftlWriteBlockPSecsNewBlock
 *
 *  @Description::Write the pages from the block boundry address.
 *              The pages are smaller than pages per block.
 *              The writen block is newly allocated to LBA and need to copyback
 *              pages from previous allocated block.
 *
 *  @Input      ::
 *          AMBA_NFTL_OBJ_s *pNftl: Pointer to NFTL object
 *          AMBA_NFTL_ARGS_s *pArg: Pointer to the parameter structure
 *          UINT8 *pBuf         : Pointer to buffer where write data is stored
 *          UINT32 *pPat        : Used sectors bit pattern corresponding to pba
 *          UINT32 Pba          : Absolutely physical block address
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
static INT32 NftlWriteBlockPSecsNewBlock(AMBA_NFTL_OBJ_s *pNftl, AMBA_NFTL_ARGS_s *pArg, UINT8 *pBuf,
        UINT32 *pPat, UINT32 Pba)
{
    UINT32 Secs;
    INT32 Eba, RetStatus = 0;

    //DBGMSG("%s: %s(..., %d)", g_NftlPart[pNftl->ID], __FUNCTION__, Pba);

    NftlSaveParam(pNftl, pArg, pBuf, pPat, Pba, NFTL_BLOCKS_PSECTS_NEW_BLOCK);

    Secs = (pArg->PagesEnd * g_Nftl_Info[pNftl->ID].SectorsPerPage) + pArg->SecsEndPage;

    if (!(Secs < g_Nftl_Info[pNftl->ID].SectorsPerBlock)) {
        RetStatus = NG;
    } else {

        if (Pba == NFTL_UNUSED_BLK) {
            Eba = NftlSearchErasedBlock(pNftl, pArg->Lba);
        } else {
            Eba = NftlSearchErasedBlockCb(pNftl, pArg->Lba, Pba);
        }

        if (Eba < 0) {
            RetStatus = NG;
            RetStatus = NftlBadBlockHandler(pNftl, pArg, RetStatus);
        } else {

            RetStatus = NftlWritePSecsEndBlock(pNftl, pArg, pBuf, (UINT32)Eba, Pba);
            if (RetStatus < 0) {
                RetStatus = NftlBadBlockHandler(pNftl, pArg, RetStatus);
            } else {
                if ((pNftl->PageType == NAND_TYPE_2K) && (Pba != NFTL_UNUSED_BLK)) {
                    RetStatus = NftlWriteCopyUpUsedPages(pNftl, pArg, 0, Secs, Pba, (UINT32)Eba, pPat);

                } else if ((pNftl->PageType == NAND_TYPE_4K) && (Pba != NFTL_UNUSED_BLK)) {
                    RetStatus = NftlWriteCopyUpUsedPages4k(pNftl, pArg, 0, Secs, Pba, (UINT32)Eba, pPat);
                } else {
                    /*  */
                }

                if (RetStatus == 0) {
                    if (Pba != NFTL_UNUSED_BLK) {
                        /* Erase old block and update tables. If erase block fail, */
                        /* the block will be marked as bad block later and tables */
                        /* are still need to be updated. */
                        if (OK != AmbaNandOp_EraseBlock(pNftl->StartBlkPart + Pba, 5000)) {
                            RetStatus = NAND_OP_ERASE_ER;
                            NftlUpdateBBInfo(pNftl, Pba);
                            NftlUpdateBBTable(pNftl, Pba);
                            pNftl->BBM.FailErCnt++;
                        }
                        NftlUpdatePhyTable(pNftl, Pba, NFTL_UNUSED_BLK);
                    }

                    NftlUpdateLogicTable(pNftl, pArg->Lba, (UINT32)Eba);
                    NftlUpdatePhyTable(pNftl, (UINT32)Eba, pArg->Lba);

                    if (RetStatus < 0) {
                        RetStatus = NftlBadBlockHandler(pNftl, pArg, RetStatus);
                    }
                }
            }
        }
    }
    return RetStatus;
}

/*
 *  @RoutineName:: NftlWriteBlockPSecsNewBlock
 *
 *  @Description::Write the pages from the block boundry address.
 *              The pages are smaller than pages per block.
 *              The writen block has already allocated to LBA and the wirten sectors is unused,
 *              so it do not need to copyback pages from previous allocated block.
 *
 *  @Input      ::
 *          AMBA_NFTL_OBJ_s *pNftl: Pointer to NFTL object
 *          AMBA_NFTL_ARGS_s *pArg: Pointer to the parameter structure
 *          UINT8 *pBuf         : Pointer to buffer where write data is stored
 *          UINT32 *pPat        : Used sectors bit pattern corresponding to pba
 *          UINT32 Pba          : Absolutely physical block address
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
static INT32 NftlWriteBlockPSecs(AMBA_NFTL_OBJ_s *pNftl, AMBA_NFTL_ARGS_s *pArg, UINT8 *pBuf,
                                 UINT32 *pPat, UINT32 Pba)
{
    UINT32 Secs;
    INT32 RetStatus = 0;

    //DBGMSG("%s: %s(..., %d)", g_NftlPart[pNftl->ID], __FUNCTION__, Pba);

    NftlSaveParam(pNftl, pArg, pBuf, pPat, Pba, NFTL_BLOCKS_PSECTS);

    Secs = (pArg->PagesEnd * g_Nftl_Info[pNftl->ID].SectorsPerPage) + pArg->SecsEndPage;

    if (!(Secs < g_Nftl_Info[pNftl->ID].SectorsPerBlock)) {
        RetStatus = NG;
    } else {
        RetStatus = NftlWritePSecsEndBlock(pNftl, pArg, pBuf, Pba, NFTL_UNUSED_BLK);

        if (RetStatus < 0) {
            RetStatus = NftlBadBlockHandler(pNftl, pArg, RetStatus);
        }
    }
    return RetStatus;
}
#if 0
/*
 *  @RoutineName:: NftlProgErHandler
 *
 *  @Description:: NFTL program error handler
 *
 *  @Input      ::
 *          AMBA_NFTL_OBJ_s *pNftl: Pointer to the NFTL object
 *          AMBA_NFTL_BBM_s *pBbm : Pointer to the NFTL bad block management
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
static INT32 NftlProgErHandler(AMBA_NFTL_OBJ_s *pNftl, AMBA_NFTL_BBM_s *pBbm)
{
    INT32 RetStatus = 0, Eba;
    UINT32 Idx = pBbm->Count - 1U;
    UINT32 TmpEba;

    switch (pBbm->Condition) {
    case NFTL_PSECTS:
        Eba = NftlSearchErasedBlockCb(pNftl, pBbm->Arg[Idx].Lba, pBbm->Pba);
        if (Eba < 0) {
            RetStatus = NFTL_FATAL_ER;
        } else {
            AmbaMisra_TypeCast32(&TmpEba, &Eba);
            RetStatus = NftlWritePSecs(pNftl, &pBbm->Arg[Idx], pBbm->pBuf, pBbm->pPat, TmpEba);
            if (RetStatus < 0) {
                RetStatus = NFTL_FATAL_ER;
            } else {
                /* copyback pages from bad block pBbm->pba to Eba */
                RetStatus = NftlCopybackByPat(pNftl, pBbm->Pba, TmpEba, pBbm->pPat);
                if (RetStatus < 0) {
                    RetStatus = NFTL_FATAL_ER;
                }
            }
        }
        break;
    case NFTL_PSECTS_NEW_BLOCK:
        RetStatus = NftlWritePSecsNewBlock(pNftl, &pBbm->Arg[Idx], pBbm->pBuf,
                                           pBbm->pPat, pBbm->Pba);
        break;
    case NFTL_BLOCKS:
        RetStatus = NftlWriteBlock(pNftl, &pBbm->Arg[Idx], pBbm->pBuf);
        break;
    case NFTL_BLOCKS_PSECTS:
        Eba = NftlSearchErasedBlockCb(pNftl, pBbm->Arg[Idx].Lba, pBbm->Pba);
        if (Eba < 0) {
            RetStatus = NFTL_FATAL_ER;
        } else {
            AmbaMisra_TypeCast32(&TmpEba, &Eba);
            RetStatus = NftlWriteBlockPSecs(pNftl, &pBbm->Arg[Idx], pBbm->pBuf, pBbm->pPat, TmpEba);
            if (RetStatus < 0) {
                RetStatus = NFTL_FATAL_ER;
            } else {
                /* copyback pages from bad block pBbm->pba to Eba */
                RetStatus = NftlCopybackByPat(pNftl, pBbm->Pba, TmpEba, pBbm->pPat);
                if (RetStatus < 0) {
                    RetStatus = NFTL_FATAL_ER;
                }
            }
        }
        break;
    case NFTL_BLOCKS_PSECTS_NEW_BLOCK:
        RetStatus = NftlWriteBlockPSecsNewBlock(pNftl, &pBbm->Arg[Idx], pBbm->pBuf,
                                                pBbm->pPat, pBbm->Pba);
        break;
    default:
        RetStatus = NG;
        break;
    }

    return RetStatus;
}
#endif
/*
 *  @RoutineName:: NandConfigNftlPart
 *
 *  @Description:: Config partition info for NFTL.
 *
 *  @Input      ::
 *          AMBA_NAND_DEV_s *pDev : Pointer to nand device
 *          INT32 ID              : NFTL partition ID
 *          UINT32 StartBlk     : Start block address of current ftl partition
 *          UINT32 EndBlk       : End block address of current flt partition
 *          UINT32 TotalZones   : Total zones in current flt partition
 *          UINT32 RsvBlks      : Reserved blocks per zones
 *          UINT32 TrlBlks      : Number of translation tables blocks to be reserved
 *
 *  @Output     :: none
 *
 *  @Return     :: none
 */
static void NandConfigNftlPart(const AMBA_NAND_DEV_INFO_s *pDev, UINT32 ID, UINT32 StartBlk, UINT32 EndBlk,
                               UINT32 TotalZones, UINT32 RsvBlks, UINT32 TrlBlks)
{
    UINT32 Tmp, TrunkSize, TblSize;
    UINT32 TotalBlocks = 0;//AmbaRTSL_NandCtrl.TotalNumBlk;
    UINT32 BlockSize = 0;//AmbaRTSL_NandCtrl.BlkByteSize;
    UINT32 StartPart, StartFtl, NumBlks;
    AMBA_NFTL_PART_INFO_s *pNftlInfo = &g_Nftl_Info[ID];

    if (EndBlk > TotalBlocks) {
        //PRINT("Error: exceeds NAND capacity!!!");
    }

    /* Caculate the blocks for reserved */
    pNftlInfo->StartBlock = StartBlk;

    if (TrlBlks != 0U) {
        /* Caculate the pages of tchunk */
        TblSize = ((pDev->ZoneBlockSize * pDev->TotalZones) << 1);

        Tmp = GetRoundUpValU32(TblSize, pDev->MainByteSize);
        pNftlInfo->PagesPerTchunk = Tmp;

        Tmp = GetRoundUpValU32(NAND_BBINFO_SIZE, pDev->MainByteSize);
        Tmp += NAND_SS_BLK_PAGES + NAND_IMARK_PAGES;
        pNftlInfo->PagesPerTchunk += Tmp;

        /* Caculate the blocks for tchunk area */
        TrunkSize = pNftlInfo->PagesPerTchunk * pDev->MainByteSize * TrlBlks;

        NumBlks = 4U + (TrunkSize / BlockSize);
        if ((TrunkSize % BlockSize) != 0x0U) {
            NumBlks++;
        }

        pNftlInfo->StartTrlTbl = StartBlk;
        pNftlInfo->TrlBlocks   = NumBlks;

        StartFtl = pNftlInfo->StartTrlTbl + pNftlInfo->TrlBlocks;
    } else {
        pNftlInfo->PagesPerTchunk = 0;
        pNftlInfo->StartTrlTbl    = 0;
        pNftlInfo->TrlBlocks      = 0;
        StartFtl = pNftlInfo->StartBlock;
    }

    /* Caculate the starting block of partition */
    for (StartPart = 0;
         StartPart < StartFtl;
         StartPart += TotalZones) {};

    pNftlInfo->StartBlock      = StartPart;
    pNftlInfo->RBlksPerZone    = RsvBlks;
    pNftlInfo->FtlBlocks       = EndBlk - StartPart;
    pNftlInfo->TotalZones      = TotalZones;
    pNftlInfo->PBlksPerZone    = pNftlInfo->FtlBlocks / pNftlInfo->TotalZones;

    pNftlInfo->FtlBlocks      -= (pNftlInfo->FtlBlocks % pNftlInfo->PBlksPerZone);
    pNftlInfo->TotalBlocks     = pNftlInfo->FtlBlocks;

    pNftlInfo->LBlksPerZone    = pNftlInfo->PBlksPerZone - RsvBlks;
    pNftlInfo->SectorsPerPage  = pDev->MainByteSize >> NAND_SEC_SHT;
    pNftlInfo->SectorsPerBlock = pNftlInfo->SectorsPerPage * pDev->BlockPageSize;

    /* Guarantee we have mutiple "pblks_per_zone" blocks */
    if ((pNftlInfo->TotalBlocks % pNftlInfo->PBlksPerZone) != 0U) {
        pNftlInfo->TotalBlocks -= (pNftlInfo->TotalBlocks % pNftlInfo->PBlksPerZone);
        pNftlInfo->FtlBlocks = pNftlInfo->TotalBlocks;
    }

    if (pNftlInfo->FtlBlocks <= RsvBlks) {
        (void)AmbaWrap_memset(pNftlInfo, 0x0, sizeof(AMBA_NFTL_PART_INFO_s));
    }
}

/*
 *  @RoutineName:: AmbaRTSL_NandGetRsvBlks
 *
 *  @Description:: Get the reserved blocks for NFTL partitions.
 *
 *  @Input      ::
 *          INT32 NumBlk      : Number of blocks.
 *          INT32 ZoneT       : Zone threshold value.
 *          INT32 RsvPerZone  : Reserved blocks for a zone.
 *          INT32 MinRsv      : Minimum reserved blocks for zones.
 *          INT32 *pZones     : Number of zones.
 *          INT32 *pRsvPerZone: Reserved blocks for a zone.
 *
 *  @Output     ::
 *          INT32 *pZones     : Number of zones.
 *          INT32 *pRsvPerZone: Reserved blocks for a zone.
 *
 *  @Return     :: none
 */
static void AmbaNftl_GetRsvBlks(UINT32 NumBlk, UINT32 ZoneT, UINT32 RsvPerZone, UINT32 MinRsv,
                                UINT32 *pZones, UINT32 *pRsvPerZone)
{
    UINT32 i, r, BlksPerZone;

    //for (i = 0U; (i * (UINT32)ZoneT) < NumBlk; i++){;};
    i = NumBlk / ZoneT;
    i = (0x0U != (NumBlk % ZoneT)) ? (i + 1U) : i;

    BlksPerZone = NumBlk / i;

    r = RsvPerZone * BlksPerZone / ZoneT;

    if ((RsvPerZone * BlksPerZone % ZoneT) != 0U) {
        r++;
    }

    if (r < MinRsv) {
        r = MinRsv;
    }

    *pZones = i;
    *pRsvPerZone = r;
}

/*
 *  @RoutineName:: AmbaNAND_InitNftlPart
 *
 *  @Description:: Initialize the blocks infos for NFTL partitions.
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
UINT32 AmbaNAND_InitNftlPart(UINT32 ID, UINT32 StartBlk, UINT32 NumBlk)
{
    UINT32 RetStatus = NAND_ERR_NONE;
    UINT32 Zones = 0U, RsvBlks = 0U, Trlblks = 0U;
    UINT32 TotalNumBlk = 0;//AmbaRTSL_NandCtrl.TotalNumBlk;

    UINT32 NFTL_ID = UserPartID_To_NftlID(ID);
    PARTITION_OBJ *pPartObj = &g_PartObj[NFTL_ID];

    (void)AmbaWrap_memset(pPartObj, 0x0, sizeof(PARTITION_OBJ));
    if ((StartBlk + NumBlk) > TotalNumBlk) {
        RetStatus = NAND_ERR_ARG;
    } else {

        switch (ID) {
        case AMBA_USER_PARTITION_VIDEO_REC_INDEX:
            /* Raw file ftl partition. */
            Trlblks = IDX_TRL_TABLES;
            AmbaNftl_GetRsvBlks(NumBlk, IDX_ZONE_THRESHOLD, IDX_RSV_BLOCKS_PER_ZONET,
                                IDX_MIN_RSV_BLOCKS_PER_ZONE, &Zones, &RsvBlks);
            break;
        case AMBA_USER_PARTITION_CALIBRATION_DATA:
            /* Calibration ftl partition. */
            Trlblks = CAL_TRL_TABLES;
            AmbaNftl_GetRsvBlks(NumBlk, CAL_ZONE_THRESHOLD, CAL_RSV_BLOCKS_PER_ZONET,
                                CAL_MIN_RSV_BLOCKS_PER_ZONE, &Zones, &RsvBlks);
            break;
        case AMBA_USER_PARTITION_USER_SETTING:
            /* Preferences ftl partition. */
            Trlblks = PRF_TRL_TABLES;
            AmbaNftl_GetRsvBlks(NumBlk, PRF_ZONE_THRESHOLD, PRF_RSV_BLOCKS_PER_ZONET,
                                PRF_MIN_RSV_BLOCKS_PER_ZONE, &Zones, &RsvBlks);
            break;
        case AMBA_USER_PARTITION_FAT_DRIVE_A:
            /* Storage ftl partition. */
            Trlblks = STG1_TRL_TABLES;
            AmbaNftl_GetRsvBlks(NumBlk, STG1_ZONE_THRESHOLD, STG1_RSV_BLOCKS_PER_ZONET,
                                STG1_MIN_RSV_BLOCKS_PER_ZONE, &Zones, &RsvBlks);
            break;
        case AMBA_USER_PARTITION_FAT_DRIVE_B:
            /* Storage2 media ftl partition. */
            Trlblks = STG2_TRL_TABLES;
            AmbaNftl_GetRsvBlks(NumBlk, STG2_ZONE_THRESHOLD, STG2_RSV_BLOCKS_PER_ZONET,
                                STG2_MIN_RSV_BLOCKS_PER_ZONE, &Zones, &RsvBlks);
            break;
        case AMBA_USER_PARTITION_RESERVED0:
        case AMBA_USER_PARTITION_RESERVED1:
        case AMBA_USER_PARTITION_RESERVED2:
        case AMBA_USER_PARTITION_RESERVED3:
        case AMBA_USER_PARTITION_RESERVED4:
            /* Preferences ftl partition. */
            Trlblks = NAND_TRL_TABLES;
            AmbaNftl_GetRsvBlks(NumBlk, PRF_ZONE_THRESHOLD, PRF_RSV_BLOCKS_PER_ZONET,
                                PRF_MIN_RSV_BLOCKS_PER_ZONE, &Zones, &RsvBlks);
            break;
        default:
            //PRINT("The ID:%d of NFTL is not support", ID);
            RetStatus = NAND_ERR_ARG;
            break;
        }

        if (RetStatus == OK) {
            const AMBA_NAND_DEV_INFO_s *pDev = AmbaNAND_GetCommonInfo();
            NandConfigNftlPart(pDev, ID - AMBA_USER_PARTITION_VIDEO_REC_INDEX, StartBlk, StartBlk + NumBlk,
                               Zones, RsvBlks, Trlblks);
        }
    }
    return RetStatus;
}

static INT32 NftlInitTbl(AMBA_NFTL_OBJ_s *pNftl)
{
    INT32 RetStatus = 0;

    if ((pNftl->Mode & NFTL_MODE_HAVE_TABLE_CACHE) != 0U) {
        if (0 == NftlInitTblCache(pNftl)) {
            pNftl->pTrlTable = &g_LbaTrlTable[pNftl->ID * TRL_TABLE_SIZE];
        }
    }

    if (0U == NftlIsValidTblCache(&NftlTblCache[pNftl->ID])) {
        RetStatus = NftlCreateLbaTbl(pNftl);
    }

    if (RetStatus == 0) {
        RetStatus = NftlCreatePbaTbl(pNftl);
    }

    return RetStatus;
}

/*
 *  @RoutineName:: NftlInit
 *
 *  @Description:: Initialize the NFTL logical, physical tables and device information
 *
 *  @Input      ::
 *          AMBA_NFTL_OBJ_s *pNftl    : Pointer to NFTL object
 *          INT32 Mode                : Initialize mode
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
static INT32 NftlInit(AMBA_NFTL_OBJ_s *pNftl, UINT32 Mode)
{
#ifdef ENABLE_VERIFY_NFTL
    static UINT8 PageBufDebug[AMBA_NUM_NFTL_PARTITION  * NFTL_DEBUG_MBUF_SIZE] GNU_SECTION_NOZEROINIT;
    static UINT8 SpareBufDebug[AMBA_NUM_NFTL_PARTITION * NFTL_DEBUG_SBUF_SIZE] GNU_SECTION_NOZEROINIT;
#endif

    static UINT8  PageBuf[AMBA_NUM_NFTL_PARTITION   * NFTL_PAGE_BUF_SIZE]  GNU_ALIGNED_CACHESAFE GNU_SECTION_NOZEROINIT;
    static UINT8  SpareBuf[AMBA_NUM_NFTL_PARTITION  * NFTL_SPARE_BUF_SIZE] GNU_ALIGNED_CACHESAFE GNU_SECTION_NOZEROINIT;
    static UINT32 BBInfo[AMBA_NUM_NFTL_PARTITION    * NFTL_BB_INFO_SIZE]   GNU_ALIGNED_CACHESAFE GNU_SECTION_NOZEROINIT;
    static UINT8  SectorBuf[AMBA_NUM_NFTL_PARTITION * NFTL_PAGE_BUF_SIZE]  GNU_ALIGNED_CACHESAFE GNU_SECTION_NOZEROINIT;

    const AMBA_NAND_DEV_INFO_s  *pDev = AmbaNAND_GetCommonInfo();
    const AMBA_NFTL_PART_INFO_s *pNftlInfo = &g_Nftl_Info[pNftl->ID];
    INT32 RetStatus = 0;

    if ((pNftlInfo->FtlBlocks <= pNftlInfo->RBlksPerZone) || (!(pDev->TotalZones < NFTL_MAX_ZONE))) {
        //DBGMSG("%s: Nand host is not initialized, skip initializing NFTL", g_NftlPart[pNftl->ID]);
        RetStatus = NG;
    } else {

        /* Before zero pNftl object, save the pNftl id first and then restore. */
        UINT32 i = pNftl->ID;
        (void)AmbaWrap_memset(pNftl, 0x0, sizeof(*pNftl));
        pNftl->ID = i;

        pNftl->Mode = Mode;

#ifdef ENABLE_VERIFY_NFTL
        pNftl->pMainDebug  = &PageBufDebug[pNftl->ID  * NFTL_DEBUG_MBUF_SIZE];
        pNftl->pSpareDebug = &SpareBufDebug[pNftl->ID * NFTL_DEBUG_SBUF_SIZE];
#endif
        pNftl->pMainBuf  = &PageBuf[pNftl->ID   * NFTL_PAGE_BUF_SIZE];
        pNftl->pSpareBuf = &SpareBuf[pNftl->ID  * NFTL_SPARE_BUF_SIZE];
        pNftl->pSecBuf   = &SectorBuf[pNftl->ID * NFTL_PAGE_BUF_SIZE];

        pNftl->pBBInfo = (UINT32 *) &BBInfo[pNftl->ID * NFTL_BB_INFO_SIZE];
        (void)AmbaWrap_memset(pNftl->pBBInfo, 0xff, NFTL_BB_INFO_SIZE);

        pNftl->TotalBlks = pNftlInfo->FtlBlocks;
        if (pNftl->TotalBlks > 0xffffU) {
            pNftl->TotalBlks = 0xfffeU;
            //PRINT("%s: exceed the maximun supported blocks, set total blocks to 0x%x", g_NftlPart[pNftl->ID], pNftl->TotalBlks);
        }

        if ((pDev->MainByteSize == NAND_MAIN_2K) && (pDev->BlockPageSize == NAND_PPB_64)) {
            pNftl->PageType = NAND_TYPE_2K;
        } else if ((pDev->MainByteSize == NAND_MAIN_4K) && (pDev->BlockPageSize == NAND_PPB_64)) {
            pNftl->PageType = NAND_TYPE_4K;
        } else {
            pNftl->PageType = NAND_TYPE_NONE;
        }

        pNftl->TotalSecs    = pNftlInfo->LBlksPerZone * pNftlInfo->TotalZones * pNftlInfo->SectorsPerBlock;
        pNftl->TotalZones   = pNftlInfo->TotalZones;
        pNftl->LBlksPerZone = pNftlInfo->LBlksPerZone;
        pNftl->PBlksPerZone = pNftlInfo->PBlksPerZone;
        pNftl->StartBlkPart = pNftlInfo->StartBlock;
        pNftl->MaxZones     = pDev->TotalZones;

        for (i = 0U; i < pNftl->TotalZones; i++) {
            pNftl->SBlk[i] = i * pNftl->PBlksPerZone;
        }

        pNftl->NumBB = 0U;
        for (i = pNftl->StartBlkPart; i < (pNftl->StartBlkPart + pNftl->TotalBlks); i++) {
            if (AmbaNandBBT_IsBadBlock(i) != AMBA_NAND_BLK_GOOD) {
                pNftl->NumBB++;
            }
        }

        /* Check if bad blocks is too much */
        if (pNftl->NumBB >= NFTL_BB_INFO_SIZE) {
            //PRINT("%s: dranger! too many bad blocks!", g_NftlPart[pNftl->ID]);
            RetStatus = NG;
        } else {
            /* Create logic/phical translation table */
            RetStatus = NftlInitTbl(pNftl);
            if (RetStatus == 0) {
                //DBGMSG("%s: %s successful", __FUNCTION__, g_NftlPart[pNftl->ID]);
                pNftl->Init = 1U;
            }
        }
    }
    return RetStatus;
}

/*
 *  @RoutineName:: NftlDeinit
 *
 *  @Description:: Deinitialize NFTL logical and physical tables.
 *
 *  @Input      ::
 *          AMBA_NFTL_OBJ_s *pNftl    : Pointer to NFTL object
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
static UINT32 NftlDeinit(AMBA_NFTL_OBJ_s *pNftl)
{
    if (pNftl->Init != 0U) {
        (void)AmbaWrap_memset(pNftl, 0x0, sizeof(*pNftl));
    }

    return NAND_ERR_NONE;
}

/*
 *  @RoutineName:: NftlErasePart
 *
 *  @Description:: Erase NFTL partition
 *
 *  @Input      ::
 *          AMBA_NFTL_OBJ_s *pNftl    : Pointer to NFTL object
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
static UINT32 NftlErasePart(const AMBA_NFTL_OBJ_s *pNftl)
{
    UINT32 RetStatus = 0;
    UINT32 StartBlk = g_Nftl_Info[pNftl->ID].StartBlock;
    UINT32 EndBlk   = StartBlk + g_Nftl_Info[pNftl->ID].TotalBlocks;

    for (UINT32 Blk = StartBlk; Blk < EndBlk; Blk++) {
        if (AmbaNandBBT_IsBadBlock(Blk) == AMBA_NAND_BLK_FACTORY_BAD) {
            /* Always do not touch initial bad block! */
            continue;
        }

        RetStatus = AmbaNandOp_EraseBlock(Blk, 5000U);
    }

    return RetStatus;
}

/*
 *  @RoutineName:: AmbaNFTL_ReadSector
 *
 *  @Description:: Read sectors from NAND flash memory
 *
 *  @Input      ::
 *          UINT32 Block    : Block to be erased
 *          UINT32 Page     : Read from this page
 *          UINT32 Sec      : Read from this sector
 *          UINT32 Secs     : Sectors to be read
 *          UINT8 *pBuf     : Pointer to data
 *          UINT32 Area     : Read area
 *          UINT32 TimeOut  : Time out value
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(>0)/NG(-1)
 */
static INT32 AmbaNFTL_ReadSector(const AMBA_NFTL_OBJ_s *pNftl, UINT32 Block, UINT32 Page, UINT32 Sec, UINT32 Secs,
                                 UINT8 *pBuf, UINT32 TimeOut)
{
    UINT32 Size, MisAlign;
    INT32 RetStatus = 0;
    UINT8 *pTmpBuf = pNftl->pSecBuf;
    const AMBA_NAND_DEV_INFO_s *pDev = AmbaNAND_GetCommonInfo();
    UINT32 PageAddr, S_Secs = 0U, E_Secs = 0U, P_Secs;
    UINT32 BufIdx = 0U, TmpSecs = Secs;
    UINT32 SectorPerPage;

    //DBGMSG("%s: (block %d)(page %d)(sector %d) (sectors %d)", __FUNCTION__, Block, Page, Sec, Secs);
    (void)AmbaKAL_MutexTake(&NftlSecReadMutex, KAL_WAIT_FOREVER);

    if (pDev->MainByteSize == NAND_MAIN_2K) {
        SectorPerPage = NAND_SPP_4;
    } else {
        SectorPerPage = NAND_SPP_8;
    }

    MisAlign = Sec & (SectorPerPage - 1U);
    if (MisAlign != 0U) {
        S_Secs = SectorPerPage - MisAlign;
    } else {
        S_Secs = 0;
    }

    if (S_Secs > TmpSecs) {
        S_Secs = TmpSecs;
        TmpSecs = 0;
    } else {
        TmpSecs -= S_Secs;
    }

    if (TmpSecs != 0U) {
        E_Secs = TmpSecs & (SectorPerPage - 1U);
        TmpSecs -= E_Secs;
    } else {
        E_Secs = 0;
    }

    P_Secs = TmpSecs;

    /* Multiple block transfer */
    if (S_Secs > 0U) {
        PageAddr = (Block * pDev->BlockPageSize) + Page;
        if (OK != AmbaNandOp_Read(PageAddr, 1, pTmpBuf, NULL, TimeOut)) {
            RetStatus = NAND_OP_READ_ER;
            (void) AmbaKAL_MutexGive(&NftlSecReadMutex);
        } else {
            Size = S_Secs << NAND_MAIN_512_SHT;
            (void)AmbaWrap_memcpy(pBuf, &pTmpBuf[MisAlign << NAND_MAIN_512_SHT], Size);
            Page++;
            BufIdx += Size;
        }
    }

    if ((RetStatus >= 0) &&
        (P_Secs > 0U)) {
        PageAddr = (Block * pDev->BlockPageSize) + Page;
        if (OK != AmbaNandOp_Read(PageAddr, (P_Secs / SectorPerPage), &pBuf[BufIdx], NULL, TimeOut)) {
            RetStatus = NAND_OP_READ_ER;
            (void) AmbaKAL_MutexGive(&NftlSecReadMutex);
        } else {
            Page += P_Secs / SectorPerPage;
            BufIdx += P_Secs << NAND_MAIN_512_SHT;
        }
    }

    if ((RetStatus >= 0) &&
        (E_Secs != 0U)) {
        PageAddr = (Block * pDev->BlockPageSize) + Page;
        if (OK != AmbaNandOp_Read(PageAddr, 1, pTmpBuf, NULL, TimeOut)) {
            RetStatus = NAND_OP_READ_ER;
        } else {
            Size = E_Secs << NAND_MAIN_512_SHT;
            (void)AmbaWrap_memcpy(&pBuf[BufIdx], pTmpBuf, Size);
        }
    }

    (void) AmbaKAL_MutexGive(&NftlSecReadMutex);

    return RetStatus;
}

static void NftlReadErrorHandler(AMBA_NFTL_OBJ_s *pNftl, UINT32 Pba, UINT32 Lba)
{
    NftlUpdateBBInfo(pNftl, Pba);
    NftlUpdateBBTable(pNftl, Pba);
    pNftl->BBM.FailRdCnt++;
    //PRINT("(NAND_OP_READ_ER) (lba = %d, pba = %d)", g_NftlPart[pNftl->ID], Lba, Pba);
    /* update the tables to avoid to read the same block again */
    NftlUpdateLogicTable(pNftl, Lba, NFTL_UNUSED_BLK);
    NftlUpdatePhyTable(pNftl, Pba, NFTL_UNUSED_BLK);
}

static INT32 NftlRead2kPage(AMBA_NFTL_OBJ_s *pNftl, UINT8 *pBuf, UINT32 Sec, UINT32 Secs)
{
    const AMBA_NAND_DEV_INFO_s *pDev = AmbaNAND_GetCommonInfo();
    UINT32 BufIdx = 0;
    UINT32 SecNum = Sec & (NAND_SPP_4 - 1U);
    UINT32 Page = Sec >> NAND_SPP_4_SHT;
    UINT32 Lba = Page >> NAND_PPB_64_SHT;
    UINT32 Pba = NftlGetPba(pNftl, Lba);

    UINT32 NumSecs, PageAddr;

    INT32 RetStatus = 0;

    Page = Page & (NAND_PPB_64 - 1U);

    if (Pba == NFTL_UNUSED_BLK) {
        /* if the lba has no corresponding pba, fill read buf to 0xff and return 0 */
        (void)AmbaWrap_memset(pBuf, 0xff, Secs << NAND_SEC_SHT);
        RetStatus = 0;
    } else {

        /* start partial block read */
        if (SecNum != 0U) {
            NumSecs = ((pDev->BlockPageSize - Page) << NAND_SPP_4_SHT) - SecNum;
            if (NumSecs > Secs) {
                NumSecs = Secs;
            }
            RetStatus = AmbaNFTL_ReadSector(pNftl, pNftl->StartBlkPart + Pba, Page, SecNum,
                                            NumSecs, pBuf, 5000U);
            if (RetStatus < 0) {
                NftlReadErrorHandler(pNftl, Pba, Lba);
                RetStatus = NG;
            } else {
                BufIdx += NumSecs << NAND_SEC_SHT;
                Secs -= NumSecs;
                Sec += NumSecs;
            }

        } else if (Page != 0U ) {
            NumSecs = (pDev->BlockPageSize - Page) << NAND_SPP_4_SHT;
            if (NumSecs > Secs) {
                NumSecs = Secs;
            }

            Pba = NftlGetPba(pNftl, Lba);
            RetStatus = AmbaNFTL_ReadSector(pNftl, pNftl->StartBlkPart + Pba, Page, 0,
                                            NumSecs, &pBuf[BufIdx], 5000U);
            if (RetStatus < 0) {
                NftlReadErrorHandler(pNftl, Pba, Lba);
                RetStatus = NG;
            } else {
                BufIdx += NumSecs << NAND_SEC_SHT;
                Secs -= NumSecs;
                Sec += NumSecs;
            }
        } else {
            /* Misra C */
        }

        if (RetStatus == 0) {
            /* full block read */
            for (; Secs >= g_Nftl_Info[pNftl->ID].SectorsPerBlock; Secs -= g_Nftl_Info[pNftl->ID].SectorsPerBlock) {
                Lba = Sec >> (NAND_SPP_4_SHT + NAND_PPB_64_SHT);
                Pba = NftlGetPba(pNftl, Lba);
                PageAddr = (pNftl->StartBlkPart + Pba) * pDev->BlockPageSize;

                if (OK != AmbaNandOp_Read(PageAddr, g_Nftl_Info[pNftl->ID].SectorsPerBlock >> NAND_SPP_4_SHT, &pBuf[BufIdx], NULL, 5000U)) {
                    NftlReadErrorHandler(pNftl, Pba, Lba);
                    RetStatus = NG;
                    break;
                } else {
                    BufIdx += g_Nftl_Info[pNftl->ID].SectorsPerBlock << NAND_SEC_SHT;
                    Sec  += g_Nftl_Info[pNftl->ID].SectorsPerBlock;
                }
            }
        }

        /* end partial block read */
        if ((RetStatus == 0) &&
            (Secs > 0U)) {
            Lba = Sec >> (NAND_SPP_4_SHT + NAND_PPB_64_SHT);
            Pba = NftlGetPba(pNftl, Lba);
            RetStatus = AmbaNFTL_ReadSector(pNftl, pNftl->StartBlkPart + Pba, 0, 0,
                                            Secs, &pBuf[BufIdx], 5000U);
            if (RetStatus < 0) {
                NftlReadErrorHandler(pNftl, Pba, Lba);
            }
        }
    }
    return RetStatus;
}

static INT32 NftlRead4kPage(AMBA_NFTL_OBJ_s *pNftl, UINT8 *pBuf, UINT32 Sec, UINT32 Secs)
{
    const AMBA_NAND_DEV_INFO_s *pDev = AmbaNAND_GetCommonInfo();
    UINT32 BufIdx = 0;
    UINT32 SecNum = Sec & (NAND_SPP_8 - 1U);
    UINT32 Page = Sec >> NAND_SPP_8_SHT;
    UINT32 Lba = Page >> NAND_PPB_64_SHT;
    UINT32 Pba = NftlGetPba(pNftl, Lba);

    UINT32 NumSecs, PageAddr;

    INT32 RetStatus = 0;

    Page = Page & (NAND_PPB_64 - 1U);

    if (Pba == NFTL_UNUSED_BLK) {
        /* if the lba has no corresponding pba, fill read buf to 0xff and return 0 */
        (void)AmbaWrap_memset(pBuf, 0xff, Secs << NAND_SEC_SHT);
        RetStatus = 0;
    } else {

        /* start partial block read */
        if (SecNum != 0U) {
            NumSecs = ((pDev->BlockPageSize - Page) << NAND_SPP_8_SHT) - SecNum;
            if (NumSecs > Secs) {
                NumSecs = Secs;
            }

            RetStatus = AmbaNFTL_ReadSector(pNftl, pNftl->StartBlkPart + Pba, Page, SecNum,
                                            NumSecs, &pBuf[BufIdx], 5000);
            if (RetStatus < 0) {
                NftlReadErrorHandler(pNftl, Pba, Lba);
                RetStatus = NG;
            } else {
                BufIdx += NumSecs << NAND_SEC_SHT;
                Secs -= NumSecs;
                Sec  += NumSecs;
            }
        } else if (Page != 0U) {
            NumSecs = (pDev->BlockPageSize - Page) << NAND_SPP_8_SHT;
            if (NumSecs > Secs) {
                NumSecs = Secs;
            }

            Pba = NftlGetPba(pNftl, Lba);
            RetStatus = AmbaNFTL_ReadSector(pNftl, pNftl->StartBlkPart + Pba, Page, 0,
                                            NumSecs, &pBuf[BufIdx], 5000U);
            if (RetStatus < 0) {
                NftlReadErrorHandler(pNftl, Pba, Lba);
                RetStatus = NG;
            } else {
                BufIdx += NumSecs << NAND_SEC_SHT;
                Secs -= NumSecs;
                Sec  += NumSecs;
            }
        } else {
            /*  */
        }

        /* full block read */
        if (RetStatus == 0) {
            for (; Secs >= g_Nftl_Info[pNftl->ID].SectorsPerBlock; Secs -= g_Nftl_Info[pNftl->ID].SectorsPerBlock) {
                Lba = Sec >> (NAND_SPP_8_SHT + NAND_PPB_64_SHT);
                Pba = NftlGetPba(pNftl, Lba);
                PageAddr = (pNftl->StartBlkPart + Pba) * pDev->BlockPageSize;
                if (OK != AmbaNandOp_Read(PageAddr, g_Nftl_Info[pNftl->ID].SectorsPerBlock >> NAND_SPP_8_SHT, &pBuf[BufIdx], NULL, 5000)) {
                    NftlReadErrorHandler(pNftl, Pba, Lba);
                    RetStatus = NG;
                    break;
                } else {
                    BufIdx += g_Nftl_Info[pNftl->ID].SectorsPerBlock << NAND_SEC_SHT;
                    Sec  += g_Nftl_Info[pNftl->ID].SectorsPerBlock;
                }
            }
        }
        /* end partial block read */
        if ((RetStatus == 0) &&
            (Secs > 0U)) {
            Lba = Sec >> (NAND_SPP_8_SHT + NAND_PPB_64_SHT);
            Pba = NftlGetPba(pNftl, Lba);
            RetStatus = AmbaNFTL_ReadSector(pNftl, pNftl->StartBlkPart + Pba, 0, 0,
                                            Secs, &pBuf[BufIdx], 5000U);
            if (RetStatus < 0) {
                NftlReadErrorHandler(pNftl, Pba, Lba);
            }
        }
    }
    return RetStatus;
}

/*
 *  @RoutineName:: NftlRead
 *
 *  @Description:: Read data to the specified sector.
 *
 *  @Input      ::
 *          AMBA_NFTL_OBJ_s *pNftl: Pointer to NFTL object
 *          UINT8 *pBuf         : Pointer to buffer where read data is stored
 *          UINT32 Sec          : Logical sector number
 *          UINT32 Secs         : Number of sectors to read
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
static INT32 NftlRead(AMBA_NFTL_OBJ_s *pNftl, UINT8 *pBuf, UINT32 Sec, UINT32 Secs)
{
    UINT32 Lba = 0, Pba = 0;
    INT32 RetStatus = 0;

    //DBGMSG("%s: %s(..., %d, %d)", g_NftlPart[pNftl->ID], __FUNCTION__, Sec, Secs);

    /* if no correct nand flash exist, just return */
    if (0x0U == pNftl->Init) {
        //PRINT("%s: %s (NAND_OP_READ_ER) (lba = %d, pba = %d)", g_NftlPart[pNftl->ID], __FUNCTION__, Lba, Pba);
        /* update the tables to avoid to read the same block again */
        NftlUpdateLogicTable(pNftl, Lba, NFTL_UNUSED_BLK);
        NftlUpdatePhyTable(pNftl, Pba, NFTL_UNUSED_BLK);
        RetStatus = NG;
    }

    /* If invalid arguments, just return */
    if ((RetStatus == 0) &&
        (((Sec + Secs) > pNftl->TotalSecs) || (Secs == 0U))) {
        //PRINT("%s: %s (NAND_OP_READ_ER) (lba = %d, pba = %d)", g_NftlPart[pNftl->ID], __FUNCTION__, Lba, Pba);
        /* update the tables to avoid to read the same block again */
        NftlUpdateLogicTable(pNftl, Lba, NFTL_UNUSED_BLK);
        NftlUpdatePhyTable(pNftl, Pba, NFTL_UNUSED_BLK);
        RetStatus = NG;
    }

    if ((RetStatus == 0) && (pNftl->PageType == NAND_TYPE_2K)) {
        RetStatus = NftlRead2kPage(pNftl, pBuf, Sec, Secs);
    } else if ((RetStatus == 0) && (pNftl->PageType == NAND_TYPE_4K)) {
        RetStatus = NftlRead4kPage(pNftl, pBuf, Sec, Secs);
    } else {
        RetStatus = NG;
    }

    if (RetStatus != 0) {
        //PRINT("%s: %s (NAND_OP_READ_ER) (lba = %d, pba = %d)", g_NftlPart[pNftl->ID], __FUNCTION__, Lba, Pba);
        /* update the tables to avoid to read the same block again */
        NftlUpdateLogicTable(pNftl, Lba, NFTL_UNUSED_BLK);
        NftlUpdatePhyTable(pNftl, Pba, NFTL_UNUSED_BLK);
    }

    return RetStatus;
}

static INT32 NftlWriteLastBlock(AMBA_NFTL_OBJ_s *pNftl, AMBA_NFTL_ARGS_s *pArg, UINT8 *pBuf, UINT32 *pPat, UINT32 BufIdx)
{
    /* Address at block boundary and transfer size is bigger than one page or sectors */
    /* and smaller than one block. */
    INT32 Used, RetStatus;

    UINT32 Pba;
    NftlInitBbm(pNftl);
    Pba = NftlGetPba(pNftl, pArg->Lba);

    if (Pba != NFTL_UNUSED_BLK) {
        if (OK != NftlGetUsedSecPat(pNftl, Pba, pPat, pNftl->pSpareBuf)) {
            RetStatus = NG;
        } else {
            Used = NftlCheckUsedSecs(pNftl, pPat, 0, pArg->SecsEndPage +
                                     (pArg->PagesEnd * g_Nftl_Info[pNftl->ID].SectorsPerPage));

            if (Used != 0) {
                RetStatus = NftlWriteBlockPSecsNewBlock( pNftl, pArg, (UINT8 *) &pBuf[BufIdx], pPat, Pba);
            } else {
                RetStatus = NftlWriteBlockPSecs(pNftl, pArg, (UINT8 *) &pBuf[BufIdx], pPat, Pba);
            }
        }
    } else {
        (void)AmbaWrap_memset(pPat, 0x0, MAX_BIT_PAT_32);
        RetStatus = NftlWriteBlockPSecsNewBlock(pNftl, pArg, (UINT8 *) &pBuf[BufIdx], pPat, Pba);
    }
    return RetStatus;
}

static INT32 CheckCacheTblInvalid(const AMBA_NFTL_OBJ_s *pNftl, UINT32 Sec, UINT32 Secs)
{
    INT32 RetStatus = 0;

    if ((0x0U == pNftl->Init) ||
        (((Sec + Secs) > pNftl->TotalSecs) || (Secs == 0U)) ||
        (pNftl->WrProtect != 0U)) {
        /* If no correct nand flash exist, just return */
        /* If invalid arguments, just return */
        RetStatus = NG;
    } else {
        //DBGMSG("%s: %s(..., %d, %d)", g_NftlPart[pNftl->ID], __FUNCTION__, Sec, Secs);
        if ((pNftl->Mode & NFTL_MODE_HAVE_TABLE_CACHE) != 0U) {
            if (OK != NftlInvalidTblCache(pNftl)) {
                RetStatus = NG;
            }
        }
    }
    return RetStatus;
}

static UINT32 CheckCondirion(INT32 Used)
{
    UINT32 Condition;
    if (Used != 0) {
        /* write to an used block but the writen area are used */
        Condition = NFTL_PSECTS_NEW_BLOCK;
    } else {
        /* write to an used block but the writen area are not used */
        Condition = NFTL_PSECTS;
    }
    return Condition;
}

static INT32 NftlCheckWriteCondition(const AMBA_NFTL_ARGS_s *pArg, UINT32 *pCondition)
{
    INT32 RetStatus = 0;

    if (pArg->Blocks > 0U) {
        /* Start address at block boundary and transfer size is bigger than one block. */
        *pCondition = NFTL_BLOCKS;

    } else if ((pArg->Blocks == 0U) && ((pArg->PagesEnd > 0U) || (pArg->SecsEndPage > 0U))) {
        /* Start address at block boundary and transfer size is bigger than one page or sectors */
        /* and smaller than one block. */
        *pCondition = NFTL_BLOCKS_PSECTS;
    } else {
        RetStatus = NG;
    }
    return RetStatus;
}

static void NftlWriteCheckResult(INT32 *pRetStatus, UINT32 *pBufIdx, UINT32 Len)
{
    if ((*pRetStatus) < 0) {
        (*pRetStatus) = NG;
    } else {
        (*pBufIdx) += Len;
    }
}

/*
 *  @RoutineName:: NftlWrite
 *
 *  @Description:: Write data to the specified sector.
 *
 *  @Input      ::
 *          AMBA_NFTL_OBJ_s *pNftl: Pointer to NFTL object
 *          UINT8 *pBuf         : Pointer to buffer where write data is stored
 *          UINT32 Sec          : Logical sector number to write
 *          UINT32 Secs         : Number of sector to write
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
static INT32 NftlWrite(AMBA_NFTL_OBJ_s *pNftl, UINT8 *pBuf, UINT32 Sec, UINT32 Secs)
{
    UINT32 Pba = 0U;
    UINT32 Pat[MAX_BIT_PAT_32], Len, Condition = 0, SectorsPerPage, SectorsPerBlock;
    AMBA_NFTL_ARGS_s Arg = {0};
    INT32 Used, RetStatus;
    const AMBA_NAND_DEV_INFO_s *pDev = AmbaNAND_GetCommonInfo();
    UINT32 BufIdx = 0U;

    SectorsPerPage = pDev->MainByteSize >> NAND_SEC_SHT;
    SectorsPerBlock = pDev->BlockPageSize * SectorsPerPage;

    AmbaMisra_TouchUnused(&Arg);

    RetStatus = CheckCacheTblInvalid(pNftl, Sec, Secs);

    if (RetStatus == 0) {
        NftlGetReqArg(pNftl, &Arg, Sec, Secs);

        if ((Arg.SecsStart > 0U) || (Arg.PagesStartBlk > 0U) || (Arg.SecsEndBlk > 0U)) {
            /* Start address at sector or page boundry */
            Pba = NftlGetPba(pNftl, Arg.Lba);
            if (Pba != NFTL_UNUSED_BLK) {
                /* used block, check to see if the write area were used or not */
                if (OK != NftlGetUsedSecPat(pNftl, Pba, Pat, pNftl->pSpareBuf)) {
                    RetStatus = NG;
                } else {
                    Used = NftlCheckUsedSecs(pNftl, Pat, Arg.SecNum + (Arg.Page * SectorsPerPage),
                                             Arg.SecsStart + (Arg.PagesStartBlk * SectorsPerPage) +
                                             Arg.SecsEndBlk);
                    Condition = CheckCondirion(Used);
                }
            } else {
                /* write to an unused block */
                (void)AmbaWrap_memset(Pat, 0x0, sizeof(Pat));
                Condition = NFTL_PSECTS_NEW_BLOCK;
            }
        } else {
            RetStatus = NftlCheckWriteCondition(&Arg, &Condition);
        }
    }

    if (RetStatus == 0) {
        if (Condition == (UINT32)NFTL_PSECTS_NEW_BLOCK) {
            NftlInitBbm(pNftl);
            Len = (Arg.SecsStart + (Arg.PagesStartBlk * SectorsPerPage) + Arg.SecsEndBlk) << NAND_SEC_SHT;
            RetStatus = NftlWritePSecsNewBlock(pNftl, &Arg, (UINT8 *) pBuf, Pat, Pba);
            NftlWriteCheckResult(&RetStatus, &BufIdx, Len);

        } else if (Condition == (UINT32)NFTL_PSECTS) {
            NftlInitBbm(pNftl);
            Len = (Arg.SecsStart + (Arg.PagesStartBlk * SectorsPerPage) + Arg.SecsEndBlk) << NAND_SEC_SHT;
            RetStatus = NftlWritePSecs(pNftl, &Arg, (UINT8 *) &pBuf[BufIdx], Pat, Pba);
            NftlWriteCheckResult(&RetStatus, &BufIdx, Len);
        } else {
            /* Misra C */
        }
    }

    if ((RetStatus == 0) &&
        (Arg.Blocks > 0U)) {
        /* Address at block boundary and transfer size is bigger than one block. */
        NftlInitBbm(pNftl);
        Len = (Arg.Blocks * SectorsPerBlock) << NAND_SEC_SHT;
        RetStatus = NftlWriteBlock(pNftl, &Arg, (UINT8 *) &pBuf[BufIdx]);
        NftlWriteCheckResult(&RetStatus, &BufIdx, Len);
    }

    if ((RetStatus == 0) &&
        ((Arg.PagesEnd > 0U) || (Arg.SecsEndPage > 0U))) {
        /* Address at block boundary and transfer size is bigger than one page or sectors */
        /* and smaller than one block. */
        RetStatus = NftlWriteLastBlock(pNftl, &Arg, pBuf, Pat, BufIdx);
    }

    if ((RetStatus == 0) &&
        ((pNftl->Mode & NFTL_MODE_HAVE_TABLE_CACHE) != 0U)) {
        if (OK != NftlValidTblCache(pNftl)) {
            RetStatus = NAND_OP_TBL_CACHE_ER;
        }
    }

    return RetStatus;
}
#if 0
/*
 *  @RoutineName:: NftlCheckUnusedSecs
 *
 *  @Description:: Check the unused sectoes in a block
 *
 *  @Input      ::
 *          AMBA_NFTL_OBJ_s *pNftl        : Pointer to NFTL object
 *          UINT32 Block                : Block to be checked
 *          UINT32 *pPat                : Pointer to used sectors pattern
 *          AMBA_NFTL_USPAT_RPT_s *pRpt   : Pointer to report
 *          INT32 *pRpts                  : Pointer to number of errors
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
static INT32 NftlCheckUnusedSecs(AMBA_NFTL_OBJ_s *pNftl, UINT32 Block, const UINT32 *pPat,
                                 AMBA_NFTL_USPAT_RPT_s *pRpt, INT32 *pRpts)
{
    INT32 RetStatus = 0, i, j, k, l, n = 0;
    INT32 Page, Sec;

    if (pNftl->PageType == NAND_TYPE_2K) {

        for (i = 0; i < (INT32) g_Nftl_Info[pNftl->ID].SectorsPerBlock; i++) {
            k = (UINT32)i >> 5U;
            l = i % 32;

            if ((pPat[k] > (UINT32)l)) {
                continue;
            }

            /* Check to see if unused sectors contains 0xff. */
            Page = (UINT32)i >> NAND_SPP_4_SHT;
            Sec = (UINT32)i & (NAND_SPP_4 - 1U);

            RetStatus = AmbaNFTL_ReadSector(pNftl, pNftl->StartBlkPart + Block, Page,
                                            Sec, 1, pNftl->pMainBuf, 5000U);
            if (RetStatus < 0) {
                break;
            }

            for (j = 0; (UINT32)j < (1U << NAND_SEC_SHT); j++) {
                if (pNftl->pMainBuf[j] != 0xffU) {
                    if (pRpt && *pRpts > 0) {
                        pRpt->Block = Block;
                        pRpt->Page = Page;
                        pRpt->Sector = Sec;
                        pRpt++;
                        (*pRpts)--;
                    }
                    n++;
                    break;
                }
            }
        }
    } else {
        RetStatus = NG;
    }

    if (RetStatus == 0) {
        RetStatus = n;
    }

    return RetStatus;
}

/*
 *  @RoutineName:: NftlCheckTrlTable
 *
 *  @Description::
 *
 *  @Input      ::
 *          AMBA_NFTL_OBJ_s *pNftl        : Pointer to NFTL object
 *          AMBA_NFTL_USPAT_RPT_s *pRpt   : Pointer to report
 *          INT32 *pRpts                  : Pointer to number of errors
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
static INT32 NftlCheckTrlTable(AMBA_NFTL_OBJ_s *pNftl, AMBA_NFTL_USPAT_RPT_s *pRpt, INT32 Rpts)
{
    UINT32 Zone, Block, Pba, MaxZone;
    UINT8 Buf[NAND_SPARE_256 * BAD_BLOCK_PAGES];
    UINT32 Pat[MAX_BIT_PAT_32];
    INT32 TotalAbSecs, n, RetStatus = 0;

    if (0x0U == pNftl->Init) {
        RetStatus = NftlInit(pNftl, NFTL_MODE_NO_SAVE_TRL_TBL);
        if (RetStatus < 0) {
            RetStatus = NG;
        }
    }

    if (RetStatus == 0) {

        MaxZone = pNftl->TotalBlks / pNftl->PBlksPerZone;
        n = TotalAbSecs = 0;

        for (Zone = 0; (Zone < MaxZone) && (RetStatus == 0); Zone++) {
            for (Block = 0; (Block < pNftl->PBlksPerZone) && (RetStatus == 0); Block++) {
                Pba = Zone * pNftl->PBlksPerZone + Block;

                RetStatus = NftlGetUsedSecPat(pNftl, Pba, Pat, Buf);
                if (RetStatus < 0) {
                    RetStatus = NG;
                } else {
                    if (AmbaNandBBT_IsBadBlock(Pba) != (INT32)AMBA_NAND_BLK_GOOD) {
                        continue;
                    }

                    if (pRpt && Rpts > 0) {
                        pRpt += n;
                    }

                    n = NftlCheckUnusedSecs(pNftl, Pba, Pat, pRpt, &Rpts);
                    if (n >= 0) {
                        TotalAbSecs += n;
                    } else {
                        RetStatus = NG;
                    }
                }
            }
        }

        if (RetStatus == 0) {
            RetStatus = TotalAbSecs;
        }
    }
    return RetStatus;
}
#endif
/*
 *  @RoutineName:: NFTLEraseLogicalBlocks
 *
 *  @Description::
 *
 *  @Input      ::
 *          AMBA_NFTL_OBJ_s *pNftl        : Pointer to NFTL object
 *          UINT32 StartBlock             :
 *          INT32 Blocks                    :
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
static UINT32 NFTLEraseLogicalBlocks(const AMBA_NFTL_OBJ_s *pNftl, UINT32 StartBlock, INT32 Blocks)
{
    UINT32 Rval = 0;
    UINT32 Pba = 0, EndBlks;

    /* If no correct nand flash exist, just return */
    if (0x0U != pNftl->Init) {
        EndBlks = StartBlock + (UINT32)Blocks;

        for (; StartBlock < EndBlks; StartBlock++) {
            Pba = NftlGetPba(pNftl, StartBlock);

            if (Pba != NFTL_UNUSED_BLK) {
                //PRINT("Erase block %d (%d)!", pNftl->StartBlkPart + Pba, StartBlock);

                Rval = AmbaNandOp_EraseBlock(pNftl->StartBlkPart + Pba, 5000U);

                NftlUpdateLogicTable(pNftl, StartBlock, NFTL_UNUSED_BLK);
                NftlUpdatePhyTable(pNftl, Pba, NFTL_UNUSED_BLK);
            }
        }
    } else {
        Rval = NAND_ERR_ARG;
    }

    return Rval;
}

/*
 *  @RoutineName:: AmbaNFTL_EraseLogicalBlocks
 *
 *  @Description::
 *
 *  @Input      ::
 *          AMBA_NFTL_OBJ_s *pNftl        : Pointer to NFTL object
 *          UINT32 StartBlock             :
 *          INT32 Blocks                    :
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
UINT32 AmbaNFTL_EraseLogicalBlocks(UINT32 ID, UINT32 StartBlock, INT32 Blocks)
{
    UINT32 RetStatus = 0;
    PARTITION_OBJ *pPartObj;
    UINT32 NFTL_ID = UserPartID_To_NftlID(ID);

    if (!((UINT32)NFTL_ID < (UINT32)AMBA_NUM_NFTL_PARTITION)) {
        RetStatus = NAND_ERR_ARG;
    } else {
        pPartObj = &g_PartObj[NFTL_ID];
        pPartObj->Nftl.ID = NFTL_ID;

        /* Take the Mutex */
        if (AmbaKAL_MutexTake(&Mutex[NFTL_ID], KAL_WAIT_FOREVER) != OK) {
            RetStatus = NAND_ERR_OS_API_FAIL;  /* should never happen */
        } else {
            RetStatus = NFTLEraseLogicalBlocks(&pPartObj->Nftl, StartBlock, Blocks);
            (void) AmbaKAL_MutexGive(&Mutex[NFTL_ID]);  /* Release the Mutex */
        }
    }
    return RetStatus;
}

/*
 *  @RoutineName:: AmbaNFTL_InitLock
 *
 *  @Description:: Create a mutex for NFTL partition object
 *
 *  @Input      ::
 *          INT32 ID  : Partition object ID
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
UINT32 AmbaNFTL_InitLock(UINT32 ID)
{
    UINT32 Rval = 0;
    UINT32 NFTL_ID = UserPartID_To_NftlID(ID);

    if (!((UINT32)NFTL_ID < (UINT32)AMBA_NUM_NFTL_PARTITION)) {
        Rval = NAND_ERR_ARG;
    } else {

        /* Create a mutex */
        if (AmbaKAL_MutexCreate(&Mutex[NFTL_ID], NULL) != OK) {
            Rval = NAND_ERR_OS_API_FAIL;
        } else {
            /* Create a Nftl SectorRead Mutex */
            //if (NftlSecReadMutex.tx_mutex_id == 0U) {
            if (AmbaKAL_MutexCreate(&NftlSecReadMutex, NULL) != OK) {
                Rval = NAND_ERR_OS_API_FAIL;
            }
            //}
        }
    }

    return Rval;
}

/*
 *  @RoutineName:: AmbaNFTL_DeinitLock
 *
 *  @Description:: De-initialize the NFTL partition object
 *
 *  @Input      ::
 *          INT32 ID  : Partition object ID
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
UINT32 AmbaNFTL_DeinitLock(UINT32 ID)
{
    UINT32 Rval = 0;
    UINT32 NFTL_ID = UserPartID_To_NftlID(ID);

    if (!((UINT32)NFTL_ID < (UINT32)AMBA_NUM_NFTL_PARTITION)) {
        Rval = NAND_ERR_ARG;
    } else {

        /* Create a mutex */
        if (AmbaKAL_MutexDelete(&Mutex[NFTL_ID]) != OK) {
            Rval = NAND_ERR_OS_API_FAIL;
        }
    }

    return Rval;
}

/*
 *  @RoutineName:: AmbaNFTL_IsWrProtect
 *
 *  @Description:: Check if the NFTL partition is write protected
 *
 *  @Input      ::
 *          INT32 ID  : Partition object ID
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
UINT32 AmbaNFTL_IsWrProtect(UINT32 ID)
{
    PARTITION_OBJ *pPartObj;
    UINT32 IsWP;
    UINT32 NFTL_ID = UserPartID_To_NftlID(ID);

    if (!((UINT32)NFTL_ID < (UINT32)AMBA_NUM_NFTL_PARTITION)) {
        IsWP = 0;
    } else {
        pPartObj = &g_PartObj[NFTL_ID];
        pPartObj->Nftl.ID = NFTL_ID;

        /* Take the Mutex */
        (void)AmbaKAL_MutexTake(&Mutex[NFTL_ID], KAL_WAIT_FOREVER);
        IsWP = pPartObj->Nftl.WrProtect;
        /* Release the Mutex */
        (void) AmbaKAL_MutexGive(&Mutex[NFTL_ID]);
    }
    return IsWP;
}

/*
 *  @RoutineName:: AmbaNFTL_IsInit
 *
 *  @Description:: Check if the NFTL partition is initialized
 *
 *  @Input      ::
 *          INT32 ID  : Partition object ID
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
UINT32 AmbaNFTL_IsInit(UINT32 ID)
{
    PARTITION_OBJ *pPartObj;
    UINT32 RetStatus = 0;
    UINT32 NFTL_ID = UserPartID_To_NftlID(ID);

    if (!((UINT32)NFTL_ID < (UINT32)AMBA_NUM_NFTL_PARTITION)) {
        RetStatus = NAND_ERR_ARG;
    } else {
        pPartObj = &g_PartObj[NFTL_ID];
        pPartObj->Nftl.ID = NFTL_ID;

        /* Take the Mutex */
        if (AmbaKAL_MutexTake(&Mutex[NFTL_ID], KAL_WAIT_FOREVER) != OK) {
            RetStatus = NAND_ERR_OS_API_FAIL;  /* should never happen */
        } else {
            RetStatus = pPartObj->Nftl.Init;
            /* Release the Mutex */
            (void) AmbaKAL_MutexGive(&Mutex[NFTL_ID]);
        }
    }
    return RetStatus;
}

/*
 *  @RoutineName:: AmbaNFTL_Init
 *
 *  @Description:: Initialize the NFTL partition
 *
 *  @Input      ::
 *          INT32 ID  : Partition object ID
 *          INT32 Mode: Initialize mode
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
UINT32 AmbaNFTL_Init(UINT32 ID, UINT32 Mode)
{
    UINT32 RetStatus = 0;
    UINT32 NFTL_ID = UserPartID_To_NftlID(ID);
    PARTITION_OBJ *pPartObj = &g_PartObj[NFTL_ID];

    if (!(NFTL_ID < AMBA_NUM_NFTL_PARTITION)) {
        RetStatus = NAND_ERR_ARG;
    } else {
        pPartObj->Nftl.ID = NFTL_ID;

        /* If init already, just return */
        if (pPartObj->Nftl.Init == 1U) {
            RetStatus = 0;
        } else if (AmbaKAL_MutexTake(&Mutex[NFTL_ID], KAL_WAIT_FOREVER) != OK) {
            RetStatus = NAND_ERR_OS_API_FAIL;  /* should never happen */
        } else {
            if (0 != NftlInit(&pPartObj->Nftl, Mode)) {
                RetStatus = NAND_ERR_IO_FAIL;
            }
            /* Release the Mutex */
            (void) AmbaKAL_MutexGive(&Mutex[NFTL_ID]);
        }
    }
    return RetStatus;
}

/*
 *  @RoutineName:: AmbaNFTL_Deinit
 *
 *  @Description:: De-initialize the NFTL partition
 *
 *  @Input      ::
 *          INT32 ID  : Partition object ID
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
UINT32 AmbaNFTL_Deinit(UINT32 ID)
{
    PARTITION_OBJ *pPartObj;
    UINT32 RetStatus = 0;
    UINT32 NFTL_ID = UserPartID_To_NftlID(ID);

    if (!((UINT32)NFTL_ID < (UINT32)AMBA_NUM_NFTL_PARTITION)) {
        RetStatus = NAND_ERR_ARG;
    } else {
        pPartObj = &g_PartObj[NFTL_ID];
        pPartObj->Nftl.ID = NFTL_ID;

        /* Take the Mutex */
        if (AmbaKAL_MutexTake(&Mutex[NFTL_ID], KAL_WAIT_FOREVER) != OK) {
            RetStatus = NAND_ERR_OS_API_FAIL;  /* should never happen */
        } else {
            RetStatus = NftlDeinit(&pPartObj->Nftl);
            (void) AmbaKAL_MutexGive(&Mutex[NFTL_ID]); /* Release the Mutex */
        }
    }
    return RetStatus;
}

/*
 *  @RoutineName:: AmbaNFTL_ErasePart
 *
 *  @Description:: Erase the NFTL partition
 *
 *  @Input      ::
 *          INT32 ID  : Partition object ID
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
UINT32 AmbaNFTL_ErasePart(UINT32 ID)
{
    UINT32 RetStatus = 0;
    PARTITION_OBJ *pPartObj;
    UINT32 NFTL_ID = UserPartID_To_NftlID(ID);

    if (!((UINT32)NFTL_ID < (UINT32)AMBA_NUM_NFTL_PARTITION)) {
        RetStatus = NAND_ERR_ARG;
    } else {
        pPartObj = &g_PartObj[NFTL_ID];
        pPartObj->Nftl.ID = NFTL_ID;

        /* Take the Mutex */
        if (AmbaKAL_MutexTake(&Mutex[NFTL_ID], KAL_WAIT_FOREVER) != OK) {
            RetStatus = NAND_ERR_OS_API_FAIL;  /* should never happen */
        } else {
            RetStatus = NftlErasePart(&pPartObj->Nftl);
            if ((RetStatus == OK) && (pPartObj->Nftl.Init == 1U)) {

                if (NftlInitTrlTable(&pPartObj->Nftl) < 0) {
                    RetStatus = NAND_ERR_IO_FAIL;
                } else {
                    pPartObj->Nftl.WrProtect = 0; /* Disable write_protected */
                }
            }
            /* Release the Mutex */
            (void) AmbaKAL_MutexGive(&Mutex[NFTL_ID]);
        }
    }
    return RetStatus;
}

/*
 *  @RoutineName:: AmbaNFTL_Read
 *
 *  @Description:: Read sectors from the NFTL partition
 *
 *  @Input      ::
 *          INT32 ID      : Partition object ID
 *          UINT8 *pBuf : Pointer to store read data
 *          long Sec    : Read start from this sector
 *          INT32 Secs    : Sectors to read
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
UINT32 AmbaNFTL_Read(UINT32 ID, UINT8 *pBuf, UINT32 Sec, UINT32 Secs)
{
    PARTITION_OBJ *pPartObj;
    UINT32 RetStatus = 0;
    UINT32 NFTL_ID = UserPartID_To_NftlID(ID);

    if (!((UINT32)NFTL_ID < (UINT32)AMBA_NUM_NFTL_PARTITION)) {
        RetStatus = NAND_ERR_ARG;
    } else {
        pPartObj = &g_PartObj[NFTL_ID];
        pPartObj->Nftl.ID = NFTL_ID;

        /* Take the Mutex */
        if (AmbaKAL_MutexTake(&Mutex[NFTL_ID], KAL_WAIT_FOREVER) != OK) {
            RetStatus = NAND_ERR_OS_API_FAIL;  /* should never happen */
        } else {
            if (0 != NftlRead(&pPartObj->Nftl, pBuf, Sec, Secs)) {
                RetStatus = NAND_ERR_IO_FAIL;
            }
            /* Release the Mutex */
            (void) AmbaKAL_MutexGive(&Mutex[NFTL_ID]);
        }
    }
    return RetStatus;
}

/*
 *  @RoutineName:: AmbaNFTL_Write
 *
 *  @Description:: Write sectors to the NFTL partition
 *
 *  @Input      ::
 *          INT32 ID      : Partition object ID
 *          UINT8 *pBuf : Pointer to the data to be written
 *          long Sec    : Write start from this sector
 *          INT32 Secs    : Sectors to Write
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
UINT32 AmbaNFTL_Write(UINT32 ID, UINT8 *pBuf, UINT32 Sec, UINT32 Secs)
{
    PARTITION_OBJ *pPartObj;
    UINT32 RetStatus = 0;
    UINT32 NFTL_ID = UserPartID_To_NftlID(ID);

    if (!((UINT32)NFTL_ID < (UINT32)AMBA_NUM_NFTL_PARTITION)) {
        RetStatus = NAND_ERR_ARG;
    } else {
        pPartObj = &g_PartObj[NFTL_ID];
        pPartObj->Nftl.ID = NFTL_ID;

        /* Take the Mutex */
        if (AmbaKAL_MutexTake(&Mutex[NFTL_ID], KAL_WAIT_FOREVER) != OK) {
            RetStatus = NAND_ERR_OS_API_FAIL;  /* should never happen */
        } else {
            if (0 != NftlWrite(&pPartObj->Nftl, pBuf, Sec, Secs)) {
                RetStatus = NAND_ERR_IO_FAIL;
            }
            /* Release the Mutex */
            (void) AmbaKAL_MutexGive(&Mutex[NFTL_ID]);
        }
    }
    return RetStatus;
}

/*
 *  @RoutineName:: AmbaNFTL_GetTotalSecs
 *
 *  @Description:: Get total sectors of the NFTL partition
 *
 *  @Input      ::
 *          INT32 ID  : Partition object ID
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
UINT32 AmbaNFTL_GetTotalSecs(UINT32 ID, UINT32 *pTotalSecs)
{
    UINT32 RetStatus = 0;
    PARTITION_OBJ *pPartObj;
    UINT32 NFTL_ID = UserPartID_To_NftlID(ID);

    if (!((UINT32)NFTL_ID < (UINT32)AMBA_NUM_NFTL_PARTITION)) {
        RetStatus = NAND_ERR_ARG;
    } else {
        pPartObj = &g_PartObj[NFTL_ID];
        pPartObj->Nftl.ID = NFTL_ID;

        /* Take the Mutex */
        if (AmbaKAL_MutexTake(&Mutex[NFTL_ID], KAL_WAIT_FOREVER) != OK) {
            RetStatus = NAND_ERR_OS_API_FAIL;  /* should never happen */
        } else {
            *pTotalSecs = pPartObj->Nftl.TotalSecs;
            /* Release the Mutex */
            (void) AmbaKAL_MutexGive(&Mutex[NFTL_ID]);
        }
    }
    return RetStatus;
}
#if 0
/*
 *  @RoutineName:: AmbaNFTL_GetStatus
 *
 *  @Description:: Get the status of the NFTL partition
 *
 *  @Input      ::
 *          INT32 ID              : Partition object ID
 *          NFTL_STATUS *pStatus: Pointer to NFTL status
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
INT32 AmbaNFTL_GetStatus(INT32 ID, AMBA_NFTL_STATUS_s *pStatus)
{
    PARTITION_OBJ *pPartObj;
    INT32 RetStatus = 0;

    if (!((UINT32)ID < (UINT32)AMBA_NUM_NFTL_PARTITION)) {
        RetStatus = NG;
    } else {
        pPartObj = &g_PartObj[ID];
        pPartObj->Nftl.ID = ID;

        /* Take the Mutex */
        if (AmbaKAL_MutexTake(&Mutex[NFTL_ID], KAL_WAIT_FOREVER) != OK) {
            RetStatus = NG;  /* should never happen */
        } else {
            pStatus->FtlBlocks      = pPartObj->Nftl.TotalBlks;
            pStatus->TotalSecs      = pPartObj->Nftl.TotalSecs;
            pStatus->LBlksPerZone   = pPartObj->Nftl.LBlksPerZone;
            pStatus->PBlksPerZone   = pPartObj->Nftl.PBlksPerZone;
            pStatus->TotalZones     = pPartObj->Nftl.TotalZones;
            pStatus->StartFtlBlk    = pPartObj->Nftl.StartBlkPart;
            pStatus->NumBB          = pPartObj->Nftl.NumBB;
            pStatus->WrProtect      = pPartObj->Nftl.WrProtect;

            /* Release the Mutex */
            (void) AmbaKAL_MutexGive(&Mutex[NFTL_ID]);
        }
    }
    return RetStatus;
}
#endif
/*
 *  @RoutineName:: AmbaNFTL_ReclaimBadBlks
 *
 *  @Description:: Get the status of the NFTL partition
 *
 *  @Input      ::
 *          INT32 ID      : Partition object ID
 *          INT32 BBType  : Bad block type to be reclaimed
 *
 *  @Output     :: none
 *
 *  @Return     ::
  *         INT32 : OK(0)/NG(-1)
 */
#if 0
INT32 AmbaNFTL_ReclaimBadBlks(INT32 ID, INT32 BBType)
{
    PARTITION_OBJ *pPartObj;
    INT32 RetStatus = 0;

    if (!((UINT32)ID < (UINT32)AMBA_NUM_NFTL_PARTITION)) {
        return NG;
    }
    pPartObj = &g_PartObj[ID];
    pPartObj->Nftl.ID = ID;

    /* Take the Mutex */
    if (AmbaKAL_MutexTake(&Mutex[NFTL_ID], KAL_WAIT_FOREVER) != OK) {
        RetStatus = NG;  /* should never happen */
    } else {
        //RetStatus = NftlReclaimBadBlks(&pPartObj->Nftl, BBType);
        /* Release the Mutex */
        (void) AmbaKAL_MutexGive(&Mutex[NFTL_ID]);
    }
    return RetStatus;
}

/*
 *  @RoutineName:: AmbaNFTL_CheckTrlTable
 *
 *  @Description:: Check the correctness of NFTL translation table.
 *
 *  @Input      ::
 *          INT32 ID              : Partition object ID
 *          NFTL_USPAT_RPT *pRpt: Pointer to the report of NFTL unused sectors
 *          INT32 Rpts            : Reports
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
INT32 AmbaNFTL_CheckTrlTable(INT32 ID, AMBA_NFTL_USPAT_RPT_s *pRpt, INT32 Rpts)
{
    PARTITION_OBJ *pPartObj;
    INT32 RetStatus = 0;

    if (!((UINT32)ID < (UINT32)AMBA_NUM_NFTL_PARTITION)) {
        RetStatus = NG;
    } else {
        pPartObj = &g_PartObj[ID];
        pPartObj->Nftl.ID = ID;

        /* Take the Mutex */
        if (AmbaKAL_MutexTake(&Mutex[NFTL_ID], KAL_WAIT_FOREVER) != OK) {
            RetStatus = NG;  /* should never happen */
        } else {
            RetStatus = NftlCheckTrlTable(&pPartObj->Nftl, pRpt, Rpts);
            /* Release the Mutex */
            (void) AmbaKAL_MutexGive(&Mutex[NFTL_ID]);
        }
    }
    return RetStatus;
}

/*
 *  @RoutineName:: AmbaNFTL_GetBBMStatus
 *
 *  @Description:: Check the correctness of NFTL translation table.
 *
 *  @Input      ::
 *          INT32 ID                      : Partition object ID
 *          NFTL_BBM_STATUS *pBBMStatus : Pointer to NFTL bad block management status
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
INT32 AmbaNFTL_GetBBMStatus(INT32 ID, AMBA_NFTL_BBM_STATUS_s *pBBMStatus)
{
    INT32 RetStatus = 0;
    PARTITION_OBJ *pPartObj;

    if (!((UINT32)ID < (UINT32)AMBA_NUM_NFTL_PARTITION)) {
        RetStatus = NG;
    } else {
        pPartObj = &g_PartObj[ID];
        pPartObj->Nftl.ID = ID;

        /* Take the Mutex */
        if (AmbaKAL_MutexTake(&Mutex[NFTL_ID], KAL_WAIT_FOREVER) != OK) {
            RetStatus = NG;  /* should never happen */
        } else {

            pBBMStatus->NumBB       = pPartObj->Nftl.NumBB;
            pBBMStatus->pBBInfo     = pPartObj->Nftl.pBBInfo;
            pBBMStatus->TotalCnt    = pPartObj->Nftl.BBM.TotalCnt;
            pBBMStatus->FailWrCnt   = pPartObj->Nftl.BBM.FailWrCnt;
            pBBMStatus->FailRdCnt   = pPartObj->Nftl.BBM.FailRdCnt;
            pBBMStatus->FailErCnt   = pPartObj->Nftl.BBM.FailErCnt;

            /* Release the Mutex */
            (void) AmbaKAL_MutexGive(&Mutex[NFTL_ID]);
        }
    }
    return RetStatus;
}
#endif
