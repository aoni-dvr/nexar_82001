/*
 * Copyright (c) 2020 Ambarella International LP
 *
 * This file and its contents ("Software") are protected by intellectual
 * property rights including, without limitation, U.S. and/or foreign
 * copyrights. This Software is also the confidential and proprietary
 * information of Ambarella International LP and its licensors. You may not use, reproduce,
 * disclose, distribute, modify, or otherwise prepare derivative works of this
 * Software or any portion thereof except pursuant to a signed license agreement
 * or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
 * In the absence of such an agreement, you agree to promptly notify and return
 * this Software to Ambarella International LP.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 * MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "AmbaTypes.h"

#include "AmbaRTSL_NAND.h"
#include "AmbaRTSL_NAND_Ctrl.h"
#include "AmbaRTSL_NAND_OP.h"
#include "AmbaRTSL_FIO.h"
#include "AmbaRTSL_NAND_BBM.h"
#include "AmbaMisraFix.h"

/*
 *  @RoutineName:: AmbaRTSL_NandCreateBBT
 *
 *  @Description:: Create the BBT(Bad Block Table)
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
UINT32 AmbaRTSL_NandCreateBBT(void)
{
    const AMBA_NAND_COMMON_INFO_s *pNandDevInfo = AmbaRTSL_NandCommonInfo;
    UINT32 TotalNumBlk = AmbaRTSL_NandCtrl.TotalNumBlk;
    UINT32 *pBBT = pAmbaNandTblBadBlk;
    UINT8  *pWorkBufSpare;
    const UINT8  *pBadBlkMark1, *pBadBlkMark2;
    UINT32 BlkAddr, PageAddr, BlkMark;
    UINT32 i, Rval = NAND_ERR_NONE, Index = 0;
    INT32 GoodBlk;

    /* fill BBT with all 0xff */
    if (AmbaWrap_memset(pBBT, 0xff, AMBA_NAND_MAX_BBT_BYTE_SIZE) != OK) { /* Do nothing */ };

    /* pointers to the Bad Block Mark of the 1st and 2nd page */
    pWorkBufSpare = AmbaRTSL_FioCtrl.pWorkBufSpare;
    pBadBlkMark1 = &pWorkBufSpare[Index + AmbaRTSL_NandCtrl.BadBlkMarkOffset];
    pBadBlkMark2 = &pBadBlkMark1[Index + pNandDevInfo->SpareByteSize];

    /* scan entire NAND Spare (OOB) area */
    BlkAddr  = 0U;
    PageAddr = 0U;
    while (BlkAddr < TotalNumBlk) {
        /* Read the 1st and 2nd page of each Block */
        if (OK != AmbaRTSL_NandOpRead(PageAddr, 2U, NULL, pWorkBufSpare)) {
            BlkMark = AMBA_NAND_BLK_RUNTIME_BAD;
        } else {
            /* Factory bad block marker are at the 1st or 2nd page of Spare area */
            BlkMark = AMBA_NAND_BLK_GOOD;
            if ((*pBadBlkMark1 != 0xFFU) || (*pBadBlkMark2 != 0xFFU)) {
                /* Factory Marked Bad Block */
                BlkMark = AMBA_NAND_BLK_FACTORY_BAD;
            }
        }

        AmbaRTSL_NandSetBlkMark(BlkAddr, BlkMark);

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
        AmbaRTSL_NandCtrl.BbtVersion = 1U;      /* reset the version of BBT */
        (void)AmbaRTSL_NandUpdateBBT(AmbaRTSL_NandCtrl.BbtVersion, AmbaRTSL_NandBlkAddrPrimaryBBT, AmbaRTSL_NandBlkAddrMirrorBBT);
        Rval = NAND_ERR_NONE;
    } else {
        Rval = NAND_ERR_BAD_BLOCK_OVER;
    }

    return Rval;
}

/*
 *  @RoutineName:: AmbaRTSL_NandGetBlkMark
 *
 *  @Description:: Get the Block Mark of a Block
 *
 *  @Input      ::
 *      BlkAddr: Block address
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : the Block Mark
 */
UINT32 AmbaRTSL_NandGetBlkMark(UINT32 BlkAddr)
{
    /* Use 2-bit Mark per block. 1 Word (=UINT32) = 32/2 = 16 blocks */
    UINT32 BlkMark = (pAmbaNandTblBadBlk[(BlkAddr + 20U) >> 4U] >> (((BlkAddr + 20U) & 0x0fU) << 1U)) & 3U;

    return BlkMark;
}

/*
 *  @RoutineName:: AmbaRTSL_NandSetBlkMark
 *
 *  @Description:: Mark a bad block in BBT
 *
 *  @Input      ::
 *      BlkAddr:    Block address
 *      BlkMark:    Block mark
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
void AmbaRTSL_NandSetBlkMark(UINT32 BlkAddr, UINT32 BlkMark)
{
    /* Use 2-bit Mark per block. 1 Word (=UINT32) = 32/2 = 16 blocks */
    UINT32 *pBlkMark = &(pAmbaNandTblBadBlk[(BlkAddr + 20U) >> 4U]);
    UINT32 BlkMarkOffset = ((BlkAddr + 20U) & 0x0fU) << 1U;

    UINT32 Mark = 3U;

    *pBlkMark &= (~(Mark << BlkMarkOffset));             /* clear the Mark */
    *pBlkMark |= ((BlkMark & 3U) << BlkMarkOffset);  /* set the Mark */
}

/*
 *  @RoutineName:: AmbaRTSL_NandFindGoodBlkBackwd
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
INT32 AmbaRTSL_NandFindGoodBlkBackwd(UINT32 BlkAddr)
{
    /* Use 2-bit Mark per block. 1 Word (=UINT32) = 32/2 = 16 blocks */
    const UINT32 *pBlkMark;
    UINT32 WorkUINT32;
    INT32 i, k, GoodBlkAddr = -1;
    UINT32 Tmp, Index = 0;

    Tmp = ((BlkAddr + 20U) & 0x0fU) << 1U;
    AmbaMisra_TypeCast32(&i, &Tmp);

    Tmp = BlkAddr + 20U;
    AmbaMisra_TypeCast32(&k, &Tmp);

    while ((k >= 0) && (GoodBlkAddr < 0)) {
        pBlkMark   = &(pAmbaNandTblBadBlk[((BlkAddr + 20U) >> 4U) - Index]);
        WorkUINT32 = *pBlkMark;   /* check a word */
        for (; (i >= 0); i -= 2) {
            UINT32 Mark;
            UINT32 End = 0;
            AmbaMisra_TypeCast32(&Tmp, &i);
            Mark = ((WorkUINT32 >> Tmp) & 3U);

            if (Mark == AMBA_NAND_BLK_GOOD) {
                GoodBlkAddr = k;    /* found a good block */
                End= 1U;
            } else {
                k--;
                if (k < 0) {
                    End = 1U;
                }
            }
            if (End == 1U) {
                break;
            }
        }

        /* check next Word */
        i = 30;
    }

    GoodBlkAddr -= 20;

    return GoodBlkAddr;
}

/*
 *  @RoutineName:: AmbaRTSL_NandFindGoodBlkForward
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
INT32 AmbaRTSL_NandFindGoodBlkForward(UINT32 BlkAddr)
{
    INT32 TotalNumBlk;

    /* Use 2-bit Mark per block. 1 Word (=UINT32) = 32/2 = 16 blocks */
    const UINT32 *pBlkMark = &(pAmbaNandTblBadBlk[(BlkAddr + 20U) >> 4U]);
    UINT32 WorkUINT32;
    INT32 i, k, GoodBlkAddr = -1;
    UINT32 Tmp, Index = 0;

    AmbaMisra_TypeCast32(&TotalNumBlk, &AmbaRTSL_NandCtrl.TotalNumBlk);

    Tmp = ((BlkAddr + 20U) & 0x0fU) << 1U;
    AmbaMisra_TypeCast32(&i, &Tmp);

    Tmp = (BlkAddr + 20U);
    AmbaMisra_TypeCast32(&k, &Tmp);

    while ((k < TotalNumBlk) && (GoodBlkAddr < 0)) {
        WorkUINT32 = pBlkMark[Index];   /* check a word */
        Index ++;
        while ((i < 32) && (k < TotalNumBlk)) {
            UINT32 Mark;
            AmbaMisra_TypeCast32(&Tmp, &i);
            Mark = ((WorkUINT32 >> Tmp) & 3U);

            if (Mark == AMBA_NAND_BLK_GOOD) {
                GoodBlkAddr = k;    /* found a good block */
                break;
            }
            k ++;
            i += 2;
        }

        /* check next Word */
        i = 0;
    }

    GoodBlkAddr -= 20;

    return GoodBlkAddr;
}

static UINT32 AmbaRTSL_NandWriteBlockBBT(UINT32 BlkAddr, UINT8 *pTmpBufMain)
{
    const AMBA_NAND_COMMON_INFO_s *pNandDevInfo = AmbaRTSL_NandCommonInfo;
    UINT32 BbtPageCount = AmbaRTSL_NandCtrl.BbtPageCount;
    UINT32 RetVal;

    /* 1. Erase BBT Block */
    RetVal = AmbaRTSL_NandOpEraseBlock(BlkAddr);
    if (OK == RetVal) {
        /* 2. Write New BBT to BBT Block */
        RetVal = AmbaRTSL_NandOpProgram(BlkAddr * pNandDevInfo->BlockPageSize,
                                        BbtPageCount, pTmpBufMain,
                                        AmbaRTSL_FioCtrl.pWorkBufSpare);
        if (OK == RetVal) {
            RetVal = NAND_ERR_NONE;
        } else {
            RetVal = NAND_ERR_IO_FAIL;
        }
    }
    return RetVal;
}

/*
 *  @RoutineName:: AmbaRTSL_NandUpdateBBT
 *
 *  @Description:: Update NAND BBTs
 *
 *  @Input      ::
 *      Version:        the Version of BBT
 *      BlkAddrPrimary: Block address of Primary BBT, don't write if it is negative
 *      BlkAddrMirror:  Block address of Mirror BBT, don't write if it is negative
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
UINT32 AmbaRTSL_NandUpdateBBT(UINT32 Version, UINT32 BlkAddrPrimary, UINT32 BlkAddrMirror)
{
    const AMBA_NAND_COMMON_INFO_s *pNandDevInfo = AmbaRTSL_NandCommonInfo;
    UINT32 BbtPageCount = AmbaRTSL_NandCtrl.BbtPageCount;
    UINT8 *pWorkBufSpare;
    UINT8 *pTmpBufMain = NULL;
    UINT32 i, Index = 0U, RetVal = NAND_ERR_IO_FAIL;
    UINT32 MisraCast32;
    UINT8  MisraCast8;

    /* prepare BBT header */
    pWorkBufSpare = AmbaRTSL_FioCtrl.pWorkBufSpare;
    if (AmbaWrap_memset(pWorkBufSpare, 0xff, (SIZE_t)((SIZE_t)pNandDevInfo->SpareByteSize * (SIZE_t)BbtPageCount)) != OK) { /* Do nothing */ }
    for (i = 0U; i < BbtPageCount; i += 1U) {
        /* Signature */
        MisraCast32 = (AmbaRTSL_NandSignaturePrimaryBBT & 0xffU);
        if (AmbaWrap_memcpy(&MisraCast8, &MisraCast32, sizeof(MisraCast8)) != OK) { /* Do nothing */ };
        pWorkBufSpare[AmbaRTSL_NandCtrl.BbtSignatureOffset + Index] = MisraCast8;

        /* Version */
        MisraCast32 = (Version & 0xffU);
        if (AmbaWrap_memcpy(&MisraCast8, &MisraCast32, sizeof(MisraCast8)) != OK) { /* Do nothing */ };
        pWorkBufSpare[AmbaRTSL_NandCtrl.BbtVersionOffset + Index] = MisraCast8;

        /* Next page */
        Index += pNandDevInfo->SpareByteSize;
    }

    AmbaMisra_TypeCast(&pTmpBufMain, &pAmbaNandTblBadBlk);

    MisraCast32 = AmbaRTSL_NandSignaturePrimaryBBT;
    if (OK != AmbaWrap_memcpy(pTmpBufMain, &MisraCast32, sizeof(MisraCast32))) { /* MisraC */ }
    pTmpBufMain[4U] = 0x1U;

    /* 1. Erase Primary BBT Block */
    /* 2. Write New BBT to Primary BBT Block */
    RetVal = AmbaRTSL_NandWriteBlockBBT(BlkAddrPrimary, pTmpBufMain);

    if (RetVal == NAND_ERR_NONE) {
        /* prepare BBT header */
        pWorkBufSpare = AmbaRTSL_FioCtrl.pWorkBufSpare;
        if (AmbaWrap_memset(pWorkBufSpare, 0xff, (SIZE_t)((SIZE_t)pNandDevInfo->SpareByteSize * (SIZE_t)BbtPageCount)) != OK) { /* Do nothing */ }

        Index = 0U;
        for (i = 0U; i < BbtPageCount; i++) {
            /* Signature */

            MisraCast32 = (AmbaRTSL_NandSignatureMirrorBBT & 0xffU);
            if (AmbaWrap_memcpy(&MisraCast8, &MisraCast32, sizeof(MisraCast8)) != OK) { /* Do nothing */ };
            pWorkBufSpare[AmbaRTSL_NandCtrl.BbtSignatureOffset + Index] = MisraCast8;

            /* Version */
            MisraCast32 = (Version & 0xffU);
            if (AmbaWrap_memcpy(&MisraCast8, &MisraCast32, sizeof(MisraCast8)) != OK) { /* Do nothing */ };
            pWorkBufSpare[AmbaRTSL_NandCtrl.BbtVersionOffset + Index] = MisraCast8;

            /* Next page */
            Index += pNandDevInfo->SpareByteSize;
        }

        AmbaMisra_TypeCast(&pTmpBufMain, &pAmbaNandTblBadBlk);

        MisraCast32 = AmbaRTSL_NandSignatureMirrorBBT;
        if (AmbaWrap_memcpy(pTmpBufMain, &MisraCast32, sizeof(MisraCast32)) != OK) { /* Do nothing */ }
        pTmpBufMain[4U] = 0x1U;

        /* 3. Erase Mirror BBT Block */
        /* 4. Write New BBT to Mirror BBT Block */
        RetVal = AmbaRTSL_NandWriteBlockBBT(BlkAddrMirror, pTmpBufMain);
    }

    return RetVal;
}

/*
 *  @RoutineName:: AmbaRTSL_NandLoadBBT
 *
 *  @Description:: Load bad block table from flash
 *
 *  @Input      ::
 *      BlkAddr : Block address
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
UINT32 AmbaRTSL_NandLoadBBT(UINT32 BlkAddr)
{
    const AMBA_NAND_COMMON_INFO_s *pNandDevInfo = AmbaRTSL_NandCommonInfo;
    UINT8 *pWorkBufMain  = AmbaRTSL_FioCtrl.pWorkBufMain;
    UINT8 *pWorkBufSpare = AmbaRTSL_FioCtrl.pWorkBufSpare;
    UINT32 RetVal = NAND_ERR_IO_FAIL;

    /* read BBT and BBT header into Work buffers */
    if (OK == AmbaRTSL_NandOpRead(BlkAddr * pNandDevInfo->BlockPageSize,
                                  AmbaRTSL_NandCtrl.BbtPageCount, pWorkBufMain, pWorkBufSpare)) {

        if (AmbaWrap_memcpy(pAmbaNandTblBadBlk, pWorkBufMain, AMBA_NAND_MAX_BBT_BYTE_SIZE) != OK) { /* Do nothing */ };
        AmbaRTSL_NandCtrl.BbtVersion = pWorkBufSpare[AmbaRTSL_NandCtrl.BbtVersionOffset]; /* BBT Version */
        RetVal = NAND_ERR_NONE;
    }

    return RetVal;
}
