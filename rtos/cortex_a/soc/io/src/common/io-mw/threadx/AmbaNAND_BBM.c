/**
 *  @file AmbaNAND_BBM.c
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
 *  @details NAND Bad Block Management (BBM) APIs
 *
 */

#include "AmbaTypes.h"

#include "AmbaNAND.h"
#include "AmbaNAND_Ctrl.h"
#include "AmbaNAND_OP.h"

#include "AmbaRTSL_NAND.h"
#include "AmbaRTSL_NAND_Ctrl.h"
#include "AmbaRTSL_FIO.h"
#include "AmbaRTSL_NAND_BBM.h"
#include "AmbaMisraFix.h"

/*
 *  @RoutineName:: AmbaNAND_CreateBBT
 *
 *  @Description:: Create the BBT(Bad Block Table)
 *
 *  @Input      ::
 *      TimeOut: Time out value
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
UINT32 AmbaNAND_CreateBBT(UINT32 TimeOut)
{
    const AMBA_NAND_DEV_INFO_s *pNandDevInfo = AmbaRTSL_NandCommonInfo;
    UINT32 TotalNumBlk = AmbaRTSL_NandCtrl.TotalNumBlk;
    UINT32 *pBBT = pAmbaNandTblBadBlk;
    UINT8  *pWorkBufSpare;
    const UINT8  *pBadBlkMark1, *pBadBlkMark2;
    UINT32 BlkAddr, PageAddr, BlkMark;
    UINT32 i, Rval = NAND_ERR_NONE, Index = 0;
    INT32 GoodBlk;

    /* fill BBT with all 0xff */
    if (0U != AmbaWrap_memset(pBBT, 0xff, AMBA_NAND_MAX_BBT_BYTE_SIZE)) { /* Misrac */ };

    /* pointers to the Bad Block Mark of the 1st and 2nd page */
    pWorkBufSpare = AmbaRTSL_FioCtrl.pWorkBufSpare;
    pBadBlkMark1 = &pWorkBufSpare[Index + AmbaRTSL_NandCtrl.BadBlkMarkOffset];
    pBadBlkMark2 = &pBadBlkMark1[Index + pNandDevInfo->SpareByteSize];

    /* scan entire NAND Spare (OOB) area */
    BlkAddr  = 0U;
    PageAddr = 0U;
    while (BlkAddr < TotalNumBlk) {
        /* Read the 1st and 2nd page of each Block */
        Rval = AmbaNandOp_Read(PageAddr, 2U, NULL, pWorkBufSpare, TimeOut);

        /* Factory bad block marker are at the 1st or 2nd page of Spare area */
        BlkMark = AMBA_NAND_BLK_GOOD;
        if ((*pBadBlkMark1 != 0xFFU) || (*pBadBlkMark2 != 0xFFU)) {
            /* Factory Marked Bad Block */
            BlkMark = AMBA_NAND_BLK_FACTORY_BAD;
        }

        (void)AmbaNAND_SetBlkMark(BlkAddr, BlkMark, 5000U);

        /* check next Block */
        PageAddr += pNandDevInfo->BlockPageSize;
        BlkAddr++;
    }

    /* Find the last two good blocks for Primary and Mirro BBT */
    GoodBlk = AmbaRTSL_NandFindGoodBlkBackwd(TotalNumBlk - 1U);
    if (GoodBlk > 0) {
        AmbaMisra_TypeCast32(&i, &GoodBlk);
        AmbaRTSL_NandBlkAddrPrimaryBBT = i;
        GoodBlk = AmbaRTSL_NandFindGoodBlkBackwd(i - 1U);
    }

    if (GoodBlk > 0) {
        AmbaMisra_TypeCast32(&AmbaRTSL_NandBlkAddrMirrorBBT, &GoodBlk);
        AmbaRTSL_NandSignaturePrimaryBBT = AMBA_BBT_PRIMARY_SIGNATURE;
        AmbaRTSL_NandSignatureMirrorBBT  = AMBA_BBT_MIRROR_SIGNATURE;
        /* Update BBT; and BBT related parameters in System Partition Table */
        AmbaRTSL_NandCtrl.BbtVersion = 0U;      /* reset the version of BBT */
        (void)AmbaNAND_UpdateBBT(AmbaRTSL_NandCtrl.BbtVersion, AmbaRTSL_NandBlkAddrPrimaryBBT, AmbaRTSL_NandBlkAddrMirrorBBT, TimeOut);
    } else {
        Rval = NAND_ERR_BAD_BLOCK_OVER;
    }

    return Rval;
}

/*
 *  @RoutineName:: AmbaNAND_GetBlkMark
 *
 *  @Description:: Get the Block Mark of a Block
 *
 *  @Input      ::
 *      BlkAddr: Block address
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : the Block Mark
 */
UINT32 AmbaNAND_GetBlkMark(UINT32 BlkAddr)
{
    return AmbaRTSL_NandGetBlkMark(BlkAddr);
}

/*
 *  @RoutineName:: AmbaNAND_SetBlkMark
 *
 *  @Description:: Mark a bad block in BBT
 *
 *  @Input      ::
 *      BlkAddr:    Block address
 *      BlkMark:    Block mark
 *      TimeOut:    Time out value
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
UINT32 AmbaNAND_SetBlkMark(UINT32 BlkAddr, UINT32 BlkMark, UINT32 TimeOut)
{
    UINT32 Rval = NAND_ERR_NONE;

    if (TimeOut == 0U) {
        Rval = NAND_ERR_ARG;
    } else {
        AmbaRTSL_NandSetBlkMark(BlkAddr, BlkMark);
    }

    return Rval;
}

/*
 *  @RoutineName:: AmbaNAND_FindGoodBlkBackward
 *
 *  @Description:: Find a good block in BBT Backward
 *
 *  @Input      ::
 *      BlkAddr : start Block address for searching
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : Good Block Number/NG(-1)
 */
INT32 AmbaNAND_FindGoodBlkBackward(UINT32 BlkAddr)
{
    return AmbaRTSL_NandFindGoodBlkBackwd(BlkAddr);
}

/*
 *  @RoutineName:: AmbaNAND_FindGoodBlkForward
 *
 *  @Description:: Find a good block in BBT Forward
 *
 *  @Input      ::
 *      BlkAddr : start Block address for searching
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : Good Block Number/NG(-1)
 */
INT32 AmbaNAND_FindGoodBlkForward(UINT32 BlkAddr)
{
    return AmbaRTSL_NandFindGoodBlkForward(BlkAddr);
}

static UINT32 AmbaNAND_ProgramBBT(UINT32 IsPrimary, UINT32 BlkAddr, UINT32 TimeOut)
{
    const AMBA_NAND_DEV_INFO_s *pNandDevInfo = AmbaRTSL_NandCommonInfo;
    UINT8 *pTmpBufMain = NULL;
    UINT32 BbtPageCount = AmbaRTSL_NandCtrl.BbtPageCount;
    UINT32 RetVal;
    UINT32 BlkAddrPrimary = AmbaRTSL_NandBlkAddrPrimaryBBT;
    UINT32 BlkAddrMirror  = AmbaRTSL_NandBlkAddrMirrorBBT;
    UINT32 BlkAddrBbt     = BlkAddr;
    INT32  Tmp = 0;
    UINT32 PartitionBlks = 0;

    for (UINT32 i = 0; i < AMBA_NUM_USER_PARTITION; i ++) {
        if (pAmbaNandTblUserPart->Entry[i].BlkCount != 0U) {
            PartitionBlks = pAmbaNandTblUserPart->Entry[i].StartBlkAddr + pAmbaNandTblUserPart->Entry[i].BlkCount;
        }
    }

    for (UINT32 Cnt = 0; Cnt <= NAND_BBT_SCAN_MAXBLOCKS; Cnt++) {
        /* 1. Erase Primary/Mirror BBT Block */
        RetVal = AmbaNandOp_EraseBlock(BlkAddrBbt, TimeOut);
        if (RetVal == NAND_ERR_NONE) {
            /* 2. Write New BBT to Primary/Mirror BBT Block */
            AmbaMisra_TypeCast(&pTmpBufMain, &pAmbaNandTblBadBlk);
            RetVal = AmbaNandOp_Program(BlkAddrBbt * pNandDevInfo->BlockPageSize, BbtPageCount,
                                        pTmpBufMain, AmbaRTSL_FioCtrl.pWorkBufSpare, TimeOut);
        }

        if (RetVal != NAND_ERR_NONE) {
            (void) AmbaNAND_SetBlkMark(BlkAddrBbt, AMBA_NAND_BLK_RUNTIME_BAD, 5000U);
            Tmp = AmbaNAND_FindGoodBlkBackward(BlkAddrBbt - 1U);
            AmbaMisra_TypeCast32(&BlkAddrBbt, &Tmp);
            if (((IsPrimary == 1U) && (BlkAddrBbt == BlkAddrMirror)) ||
                ((IsPrimary == 0U) && (BlkAddrBbt == BlkAddrPrimary))) {
                /* Find another block if it's already locate to Primary/Mirror BBT */
                Tmp = AmbaNAND_FindGoodBlkBackward(BlkAddrBbt - 1U);
                AmbaMisra_TypeCast32(&BlkAddrBbt, &Tmp);
            }

            if (BlkAddrBbt < PartitionBlks) {
                RetVal = NAND_ERR_NOT_ENOUGH_SPACE;
            }
        }

        if ((RetVal == NAND_ERR_NONE) || (RetVal == NAND_ERR_NOT_ENOUGH_SPACE)) {
            break; /* write BBT done */
        }
    }

    if (RetVal == NAND_ERR_NONE) {
        if (IsPrimary == 1U) {
            AmbaRTSL_NandBlkAddrPrimaryBBT = BlkAddrBbt;
        } else {
            AmbaRTSL_NandBlkAddrMirrorBBT = BlkAddrBbt;
        }
    }

    /* Update BBT address in USER PTB */
    if ((BlkAddrPrimary != AmbaRTSL_NandBlkAddrPrimaryBBT) ||
        (BlkAddrMirror  != AmbaRTSL_NandBlkAddrMirrorBBT)) {
        UINT32 CurrentPtb = pAmbaNandTblUserPart->PTBNumber;

        RetVal = AmbaNAND_WriteUserPTB(NULL, CurrentPtb, 5000U);

        /* Switch to the other user PTB */
        RetVal = AmbaNAND_ReadUserPTB(pAmbaNandTblUserPart, (CurrentPtb + 1U) % 2U, TimeOut);

        if (IsPrimary == 1U) {
            AmbaRTSL_NandBlkAddrPrimaryBBT = BlkAddrBbt;
        } else {
            AmbaRTSL_NandBlkAddrMirrorBBT = BlkAddrBbt;
        }

        if (RetVal == NAND_ERR_NONE) {
            RetVal = AmbaNAND_WriteUserPTB(NULL, pAmbaNandTblUserPart->PTBNumber, 5000U);
        }

        /* Switch to the original user PTB */
        if (RetVal == NAND_ERR_NONE) {
            RetVal = AmbaNAND_ReadUserPTB(pAmbaNandTblUserPart, CurrentPtb, TimeOut);
        }
    }

    return RetVal;
}

/*
 *  @RoutineName:: AmbaNAND_UpdateBBT
 *
 *  @Description:: Update NAND BBTs
 *
 *  @Input      ::
 *      Version:        the Version of BBT
 *      BlkAddrPrimary: Block address of Primary BBT, don't write if it is negative
 *      BlkAddrMirror:  Block address of Mirror BBT, don't write if it is negative
 *      TimeOut:        Time out value
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
UINT32 AmbaNAND_UpdateBBT(UINT32 Version, UINT32 BlkAddrPrimary, UINT32 BlkAddrMirror, UINT32 TimeOut)
{
    const AMBA_NAND_DEV_INFO_s *pNandDevInfo = AmbaRTSL_NandCommonInfo;
    UINT32 BbtPageCount = AmbaRTSL_NandCtrl.BbtPageCount;
    UINT8 *pWorkBufSpare;
    UINT32 i, RetVal;
    UINT32 Index = 0U;
    UINT32 MisraCast32;
    UINT8  MisraCast8;

    UINT32 BlkMirrorOri  = AmbaRTSL_NandBlkAddrMirrorBBT;

    /* prepare BBT header */
    pWorkBufSpare = AmbaRTSL_FioCtrl.pWorkBufSpare;
    if (0U != AmbaWrap_memset(pWorkBufSpare, 0xff, (SIZE_t) pNandDevInfo->SpareByteSize * BbtPageCount)) { /* Misrac */ };
    for (i = 0U; i < BbtPageCount; i += 1U) {
        /* Signature */
        MisraCast32 = (AmbaRTSL_NandSignaturePrimaryBBT & 0xffU);
        if (0U != AmbaWrap_memcpy(&MisraCast8, &MisraCast32, sizeof(MisraCast8))) { /* Misrac */ };
        pWorkBufSpare[AmbaRTSL_NandCtrl.BbtSignatureOffset + Index] = MisraCast8;

        /* Version */
        MisraCast32 = (Version & 0xffU);
        if (0U != AmbaWrap_memcpy(&MisraCast8, &MisraCast32, sizeof(MisraCast8))) { /* Misrac */ };
        pWorkBufSpare[AmbaRTSL_NandCtrl.BbtVersionOffset + Index] = MisraCast8;

        /* Next page */
        Index += pNandDevInfo->SpareByteSize;
    }

    RetVal = AmbaNAND_ProgramBBT(1U, BlkAddrPrimary, TimeOut);

    if (RetVal == NAND_ERR_NONE) {
        /* prepare BBT header */
        pWorkBufSpare = AmbaRTSL_FioCtrl.pWorkBufSpare;
        if (0U != AmbaWrap_memset(pWorkBufSpare, 0xff, (SIZE_t) pNandDevInfo->SpareByteSize * BbtPageCount)) { /* Misrac */ };

        Index = 0U;
        for (i = 0U; i < BbtPageCount; i++) {
            /* Signature */

            MisraCast32 = (AmbaRTSL_NandSignatureMirrorBBT & 0xffU);
            if (0U != AmbaWrap_memcpy(&MisraCast8, &MisraCast32, sizeof(MisraCast8))) { /* Misrac */ };
            pWorkBufSpare[AmbaRTSL_NandCtrl.BbtSignatureOffset + Index] = MisraCast8;

            /* Version */
            MisraCast32 = (Version & 0xffU);
            if (0U != AmbaWrap_memcpy(&MisraCast8, &MisraCast32, sizeof(MisraCast8))) { /* Misrac */ };
            pWorkBufSpare[AmbaRTSL_NandCtrl.BbtVersionOffset + Index] = MisraCast8;

            /* Next page */
            Index += pNandDevInfo->SpareByteSize;
        }

        RetVal = AmbaNAND_ProgramBBT(0U, BlkAddrMirror, TimeOut);
        if ((RetVal == NAND_ERR_NONE) && (BlkMirrorOri != AmbaRTSL_NandBlkAddrMirrorBBT)) {
            /* Mirror BBT address changes, update to Primary again. */
            RetVal = AmbaNAND_ProgramBBT(1U, AmbaRTSL_NandBlkAddrPrimaryBBT, TimeOut);
        }
    }

    return RetVal;
}

/*
 *  @RoutineName:: AmbaNAND_WriteBBT
 *
 *  @Description:: Write NAND BBT to NAND form Dram
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
UINT32 AmbaNAND_WriteBBT(UINT32 TimeOut)
{
    return AmbaNAND_UpdateBBT(AmbaRTSL_NandCtrl.BbtVersion,
                              AmbaRTSL_NandBlkAddrPrimaryBBT,
                              AmbaRTSL_NandBlkAddrMirrorBBT,
                              TimeOut);
}

/*
 *  @RoutineName:: AmbaNAND_LoadBBT
 *
 *  @Description:: Load bad block table from flash
 *
 *  @Input      ::
 *      BlkAddr : Block address
 *      TimeOut:  Time out value
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
UINT32 AmbaNAND_LoadBBT(UINT32 BlkAddr, UINT32 TimeOut)
{
    const AMBA_NAND_DEV_INFO_s *pNandDevInfo = AmbaRTSL_NandCommonInfo;
    UINT8 *pWorkBufMain  = AmbaRTSL_FioCtrl.pWorkBufMain;
    UINT8 *pWorkBufSpare = AmbaRTSL_FioCtrl.pWorkBufSpare;
    UINT32 RetVal = NAND_ERR_IO_FAIL;

    /* read BBT and BBT header into Work buffers */
    if (OK == AmbaNandOp_Read(BlkAddr * pNandDevInfo->BlockPageSize,
                              AmbaRTSL_NandCtrl.BbtPageCount, pWorkBufMain, pWorkBufSpare, TimeOut)) {

        if (0U != AmbaWrap_memcpy(pAmbaNandTblBadBlk, pWorkBufMain, AMBA_NAND_MAX_BBT_BYTE_SIZE)) { /* Misrac */ };
        AmbaRTSL_NandCtrl.BbtVersion = pWorkBufSpare[AmbaRTSL_NandCtrl.BbtVersionOffset]; /* BBT Version */
        RetVal = NAND_ERR_NONE;
    }

    return RetVal;
}
