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
#include "AmbaDef.h"

#include "AmbaIOUtility.h"
#include "AmbaMisraFix.h"
#ifndef CONFIG_QNX
#include "AmbaRTSL_Cache.h"
#endif
#include "AmbaNAND.h"
#include "AmbaRTSL_NAND.h"
#include "AmbaRTSL_NAND_Ctrl.h"
#include "AmbaRTSL_NAND_OP.h"
#include "AmbaRTSL_FIO.h"
#include "AmbaRTSL_NAND_BBM.h"

static UINT32 WritePtbFlag = 0;
static UINT32 Rtsl_UserPtbNextBlock = 0U;

AMBA_SYS_PARTITION_TABLE_s  AmbaRTSL_SysPartTable GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE;
AMBA_USER_PARTITION_TABLE_s AmbaRTSL_UserPartTable GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE;
UINT32 AmbaRTSL_NandBadBlkTable[AMBA_NAND_MAX_BBT_BYTE_SIZE] GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE;

static void AmbaRTSL_NAND_memcpy(void *pDst, const void *pSrc, SIZE_t num)
{
    if (AmbaWrap_memcpy(pDst, pSrc, num) != 0U) {/* MisraC */}
}

static UINT32 AmbaRTSL_NandFtlGetNumReservedBlk(UINT32 NumBlk)
{
    const UINT32 MinNumReservedBlk = 12U;
    UINT32 NumReservedBlk;

    NumReservedBlk = GetRoundUpValU32(NumBlk * 24U, 1000U); /* Reserve 24 blocks per 1000 blocks */

    if (NumReservedBlk < (UINT32)MinNumReservedBlk) {
        NumReservedBlk = MinNumReservedBlk;
    }

    return NumReservedBlk;
}

#if 0
AMBA_SYS_PARTITION_TABLE_s  _AmbaRTSL_NandSysPartitionTable GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE;
AMBA_USER_PARTITION_TABLE_s _AmbaRTSL_NandUserPartitionTable GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE;

/* pointer to System Partition Table */
AMBA_SYS_PARTITION_TABLE_s  *pAmbaNandTblSysPart = &_AmbaRTSL_NandSysPartitionTable;

/* pointer to User Partition Table */
AMBA_USER_PARTITION_TABLE_s *pAmbaNandTblUserPart = &_AmbaRTSL_NandUserPartitionTable;
/* pointer to System Partition Table */
AMBA_SYS_PARTITION_TABLE_s  *pAmbaNandTblSysPart = (AMBA_SYS_PARTITION_TABLE_s *) (AMBA_DRAM_RESERVED_VIRT_ADDR + AMBA_RAM_NVM_SYS_PARTITION_TABLE_OFFSET);

/* pointer to User Partition Table */
AMBA_USER_PARTITION_TABLE_s *pAmbaNandTblUserPart = (AMBA_USER_PARTITION_TABLE_s *) (AMBA_DRAM_RESERVED_VIRT_ADDR + AMBA_RAM_NVM_USER_PARTITION_TABLE_OFFSET);
#endif

/*
 *  @RoutineName:: AmbaRTSL_NandGetActivePtbNo
 *
 *  @Description:: Get the No. of the current active parttiton table
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *        The No. of the current active parttiton table
 */
UINT32 AmbaRTSL_NandGetActivePtbNo(UINT32 * pPTBActive)
{
    UINT32 i;
    UINT32 RetVal;

    *pPTBActive = 0U;

    for(i = 0; i < 2U; i++) {
        RetVal = AmbaRTSL_NandReadUserPTB(NULL, i);

        if(pAmbaNandTblUserPart->PTBActive != 0U) {
            *pPTBActive = i;
            break;
        }
    }

    return RetVal;
}

/*
 *  @RoutineName:: AmbaRTSL_NandSetActivePtbNo
 *
 *  @Description:: Set specific parttiton table to active or nonactive
 *
 *  @Input      ::
 *      PtbNo: The No. of the  partition table (start form 0)
 *      Active: 0 -Nonactive 1 - Active
 *
 *  @Output     :: none
 *
 *  @Return     :: none
 *
 */
UINT32 AmbaRTSL_NandSetActivePtbNo(UINT32 PtbNo, UINT8 Active)
{
    UINT32 RetVal;
    RetVal = AmbaRTSL_NandReadUserPTB(NULL, PtbNo);
    if (RetVal == NAND_ERR_NONE) {
        pAmbaNandTblUserPart->PTBActive = Active;
        RetVal = AmbaRTSL_NandWriteUserPTB(NULL, PtbNo);
    }
    return RetVal;
}

static UINT32 AmbaRTSL_NandCreatePtbBbt(void)
{
    UINT32 RetVal;

    if (AmbaWrap_memset(pAmbaNandTblSysPart,  0xff, sizeof(AMBA_SYS_PARTITION_TABLE_s)) != OK) {
        /* Do nothing */
    };
    if (AmbaWrap_memset(pAmbaNandTblUserPart, 0xff, sizeof(AMBA_USER_PARTITION_TABLE_s)) != OK) {
        /* Do nothing */
    };
    RetVal = AmbaRTSL_NandCreateBBT();
    if (RetVal == NAND_ERR_NONE) {
        RetVal = AmbaRTSL_NandCreatePTB();
    }
    if ((WritePtbFlag == 1U) && (RetVal == NAND_ERR_NONE)) {
        RetVal = AmbaRTSL_NandWriteSysPTB(NULL);
        if (RetVal == NAND_ERR_NONE) {
            RetVal = AmbaRTSL_NandWriteUserPTB(NULL, 0U);
        }
    }

    if (RetVal == NAND_ERR_NONE) {
        RetVal = AmbaRTSL_NandCreateExtUserPTB(Rtsl_UserPtbNextBlock);
    }

    if ((WritePtbFlag == 1U) && (RetVal == NAND_ERR_NONE)) {
        RetVal = AmbaRTSL_NandWriteUserPTB(NULL, 1U);
    }

    if (RetVal == NAND_ERR_NONE) {
        RetVal = AmbaRTSL_NandReadUserPTB(NULL, 0U);
    }

    return RetVal;
}

/*
 *  @RoutineName:: AmbaRTSL_NandInitPtbBbt
 *
 *  @Description:: Init BBT and System/User Partition Tables
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
UINT32 AmbaRTSL_NandInitPtbBbt(void)
{
    UINT32 RetVal;
    UINT32 Crc32 = 0;
    const UINT8 *pBuffer = NULL;
    UINT32 ActivePTB = 0U;

    pAmbaNandTblSysPart = &AmbaRTSL_SysPartTable;
    pAmbaNandTblUserPart = &AmbaRTSL_UserPartTable;
    pAmbaNandTblBadBlk = AmbaRTSL_NandBadBlkTable;

    /* Read NAND System Partition Table (Block 0, Page 2) to DRAM */
    RetVal = AmbaRTSL_NandReadSysPTB(NULL);
    if (RetVal == NAND_ERR_NONE) {
        /* Check System Partition Table */
        UINT32 CrcSize = (UINT32)((sizeof(AMBA_SYS_PARTITION_TABLE_s) - 4U) & 0xffffffffU);
        AmbaMisra_TypeCast(&pBuffer, &pAmbaNandTblSysPart);
        Crc32 = IO_UtilityCrc32(pBuffer, CrcSize);
    }

    if ((RetVal == NAND_ERR_NONE) &&
        (pAmbaNandTblSysPart->CRC32 == Crc32) &&
        (pAmbaNandTblSysPart->BlkByteSize != 0U) &&
        (pAmbaNandTblSysPart->Version == USERPTB_VERSION)) {
        /* if System Partion is OK */
        RetVal = AmbaRTSL_NandGetActivePtbNo(&ActivePTB);

        if (RetVal == NAND_ERR_NONE) {
            RetVal = AmbaRTSL_NandReadUserPTB(NULL, ActivePTB);
        }

        if (RetVal == NAND_ERR_NONE) {
            /* Check User Partition Table */
            //AmbaMisra_TypeCast(&pBuffer, &pAmbaNandTblUserPart);
            //Crc32 = IO_UtilityCrc32(pBuffer, (UINT32)(AMBA_USER_PTB_CRC32_SIZE & 0xffffffffU));
            //if ((pAmbaNandTblUserPart->CRC32 == Crc32) &&
                //(pAmbaNandTblUserPart->CRC32 != 0xffffffffU)) {
                RetVal = AmbaRTSL_NandLoadBBT(AmbaRTSL_NandBlkAddrPrimaryBBT);
                if (RetVal != NAND_ERR_NONE) {
                    RetVal = AmbaRTSL_NandLoadBBT(AmbaRTSL_NandBlkAddrMirrorBBT);
                }

                if (RetVal != NAND_ERR_NONE) {
                    RetVal = AmbaRTSL_NandCreateBBT();
                    if ((RetVal == NAND_ERR_NONE) && (WritePtbFlag == 1U)) {
                        RetVal = AmbaRTSL_NandWriteUserPTB(NULL, ActivePTB);
                    }
                }
                /* Switch to active PTB */
                //if (RetVal == NAND_ERR_NONE) {
                    //RetVal = AmbaRTSL_NandGetActivePtbNo(&ActivePTB);
                    //if (RetVal == NAND_ERR_NONE) {
                        //RetVal = AmbaRTSL_NandReadUserPTB(NULL, ActivePTB);
                    //}
                //}
            //} else {
                //RetVal = NAND_ERR_PTB;
            //}
        }
    } else {
        RetVal = AmbaRTSL_NandCreatePtbBbt();
    }

    return RetVal;
}

static UINT32 AmbaRTSL_NandCreateUserPTB(UINT32 *pBlkAddr)
{
    const AMBA_PARTITION_CONFIG_s *pUserPartConfig = AmbaRTSL_NandCtrl.pUserPartConfig;
    AMBA_PARTITION_ENTRY_s  *pPartEntry;
    UINT32 BlkByteSize = AmbaRTSL_NandCtrl.BlkByteSize;
    UINT32 TotalNumBlk = AmbaRTSL_NandCtrl.TotalNumBlk;                /* Total number of blocks */
    UINT32 i, j;
    UINT32 RetVal = OK;
    INT32 GoodBlk;

    /* Configure User Partition Table */
    /* Configure remaining user partitions, except the partition of VENDOR_DATA */
    pAmbaNandTblUserPart->PtbMagic = AMBA_USER_PTB_MAGIC;
    pPartEntry = &(pAmbaNandTblUserPart->Entry[0]);
    for (i = 1U; i <= (AMBA_NUM_USER_PARTITION - 1U); i += 1U) {
        /* Partition Name */
        IO_UtilityStringCopy((char *)pPartEntry[i].PartitionName, sizeof(pPartEntry[i].PartitionName), pUserPartConfig[i].PartitionName);

        pPartEntry[i].Attribute      = pUserPartConfig[i].Attribute;   /* Attribute of the Partition */
        pPartEntry[i].ByteCount      = pUserPartConfig[i].ByteCount;        /* number of Bytes for the Partition */
        pPartEntry[i].ActualByteSize = 0U;                                 /* actual size in Bytes */
        pPartEntry[i].BlkCount       = GetRoundUpValU32(pPartEntry[i].ByteCount, BlkByteSize);  /* number of Blocks for the Partition  */

        if (pPartEntry[i].ByteCount == 0U) {
            pPartEntry[i].StartBlkAddr = 0U;
            pPartEntry[i].BlkCount     = 0U;
            continue;
        }

        if (CheckBits(pPartEntry[i].Attribute, AMBA_PARTITION_ATTR_FTL) != 0x0U) {
            pPartEntry[i].BlkCount += AmbaRTSL_NandFtlGetNumReservedBlk(pPartEntry[i].BlkCount);
        }

        GoodBlk = AmbaRTSL_NandFindGoodBlkForward(*pBlkAddr);
        if (GoodBlk < 0) {
            RetVal = NAND_ERR_BAD_BLOCK_OVER;    /* no more good block ? */
        }
        /*
            if ((RetVal != NAND_ERR_NONE) || (*pBlkAddr < TotalNumBlk)) {
                RetVal = NAND_ERR_BAD_BLOCK_OVER;
                break;
            }
        */
        AmbaMisra_TypeCast32(&j, &GoodBlk);

        pPartEntry[i].StartBlkAddr = j;   /* start Block Address */
        *pBlkAddr = j + 1U;                /* *pBlkAddr = (pPartEntry->StartBlkAddr + 1) */

        /* allocate good blocks for the partition */
        j = (pPartEntry[i].BlkCount - 1U);
        while (j > 0U) {
            if (*pBlkAddr >= TotalNumBlk) {
                RetVal = NAND_ERR_BAD_BLOCK_OVER;    /* no more good block ? */
                break;
            }

            if (AmbaRTSL_NandGetBlkMark(*pBlkAddr) == AMBA_NAND_BLK_GOOD) {
                j -= 1U;    /* one good block is allocated to this partition */
            } else {
                pPartEntry[i].BlkCount ++;    /* there's bad block add 1 blocks to this partition */
            }
            *pBlkAddr += 1U;
        }
    }
    return RetVal;
}

/*
 *  @RoutineName:: AmbaRTSL_NandCreatePTB
 *
 *  @Description:: Create NAND System and User Partition Tables
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
UINT32 AmbaRTSL_NandCreatePTB(void)
{
    const AMBA_PARTITION_CONFIG_s *pSysPartConfig  = AmbaRTSL_NandCtrl.pSysPartConfig;
    const AMBA_PARTITION_CONFIG_s *pUserPartConfig = AmbaRTSL_NandCtrl.pUserPartConfig;
    AMBA_PARTITION_ENTRY_s  *pPartEntry;
    UINT32 i, j, BlkByteSize;
    INT32 GoodBlk;
    UINT32 BlkAddr = 0U;
    UINT32 TotalNumBlk = AmbaRTSL_NandCtrl.TotalNumBlk;                /* Total number of blocks */
    UINT32 RetVal = OK;

    if ((pSysPartConfig == NULL) || (pUserPartConfig == NULL)) {
        RetVal = NAND_ERR_ARG;
    } else {

        /* Block size in Byte */
        pAmbaNandTblSysPart->BlkByteSize = AmbaRTSL_NandCtrl.BlkByteSize;
        BlkByteSize = AmbaRTSL_NandCtrl.BlkByteSize;
        pAmbaNandTblSysPart->Version = USERPTB_VERSION;

        /* Block-0: Reserved for Bootstrap & System Partition Table */
        /* next Block: Reserved for User Partition Table and Vendor Data, AMBA_USER_PARTITION_PTB must be 0 !!! */
        GoodBlk = AmbaRTSL_NandFindGoodBlkForward(1U);
        if (GoodBlk < 0) {
            RetVal = NAND_ERR_BAD_BLOCK_OVER;    /* no more good block ? */
        } else {
            AmbaMisra_TypeCast32(&BlkAddr, &GoodBlk);
        }
        if (RetVal == OK) {
            pAmbaNandTblSysPart->BlkAddrUserPTB = BlkAddr;   /* Block address of User Partition Table */

            /* Configure user partition VENDOR_DATA */
            pPartEntry = (AMBA_PARTITION_ENTRY_s  *) & (pAmbaNandTblUserPart->Entry[AMBA_USER_PARTITION_PTB]);
            IO_UtilityStringCopy((char *)pPartEntry->PartitionName, 9U, "USER_PTB");     /* Partition Name */
            pPartEntry->Attribute = pUserPartConfig->Attribute;   /* Attribute of the Partition */
            pPartEntry->ByteCount = pUserPartConfig->ByteCount;             /* number of Bytes for the Partition */
            pPartEntry->ActualByteSize = 0U;                                 /* actual size in Bytes */
            pPartEntry->BlkCount = 1U;                                       /* number of Blocks for the Partition: fixed to be 1  */
            pPartEntry->StartBlkAddr = BlkAddr;                             /* start Block Address = pAmbaNandTblSysPart->BlkAddrUserPTB */
            BlkAddr += pPartEntry->BlkCount;

            pAmbaNandTblUserPart->PTBNumber = 0U;
            pAmbaNandTblUserPart->BlkAddrCurentPTB = BlkAddr;

            /* Remain for 2 nd user partition table */
            GoodBlk = AmbaRTSL_NandFindGoodBlkForward((BlkAddr + 1U));
            if (GoodBlk < 0) {
                RetVal = NAND_ERR_BAD_BLOCK_OVER;    /* no more good block ? */
            } else {
                AmbaMisra_TypeCast32(&BlkAddr, &GoodBlk);
            }

            /* Configure sys partition: BOOTSTRAP */
            pPartEntry = (AMBA_PARTITION_ENTRY_s  *) & (pAmbaNandTblSysPart->Entry[AMBA_SYS_PARTITION_BOOTSTRAP]);
            IO_UtilityStringCopy((char *)pPartEntry->PartitionName, sizeof(pPartEntry->PartitionName), pSysPartConfig->PartitionName);     /* Partition Name */
            pPartEntry->Attribute = pSysPartConfig->Attribute;    /* Attribute of the Partition */
            pPartEntry->ByteCount = pSysPartConfig->ByteCount;              /* number of Bytes for the Partition */
            pPartEntry->ActualByteSize = 0U;                                 /* actual size in Bytes */
            pPartEntry->BlkCount = GetRoundUpValU32(pPartEntry->ByteCount, BlkByteSize);  /* number of Blocks for the Partition  */
            pPartEntry->StartBlkAddr = 0U;                                   /* start Block Address = 0 */

            /* Configure System Partition Table */
            /* Configure remaining sys partitions, except the partition of BOOTSTRAP */
            pPartEntry = (AMBA_PARTITION_ENTRY_s  *) & (pAmbaNandTblSysPart->Entry[0]);
            for (i = 1U;  i < AMBA_NUM_SYS_PARTITION; i += 1U) {
                IO_UtilityStringCopy((char *)pPartEntry[i].PartitionName, sizeof(pPartEntry[i].PartitionName), pSysPartConfig[i].PartitionName);     /* Partition Name */
                pPartEntry[i].Attribute = pSysPartConfig[i].Attribute;    /* Attribute of the Partition */
                pPartEntry[i].ByteCount = pSysPartConfig[i].ByteCount;              /* number of Bytes for the Partition */
                pPartEntry[i].ActualByteSize = 0;                                 /* actual size in Bytes */
                pPartEntry[i].BlkCount = GetRoundUpValU32(pPartEntry[i].ByteCount, BlkByteSize);  /* number of Blocks for the Partition  */

                if (pPartEntry[i].ByteCount == 0U) {
                    pPartEntry[i].StartBlkAddr = 0U;
                    pPartEntry[i].BlkCount     = 0U;
                    continue;
                }

                GoodBlk = AmbaRTSL_NandFindGoodBlkForward(BlkAddr);
                if (GoodBlk < 0) {
                    RetVal = NAND_ERR_BAD_BLOCK_OVER;    /* no more good block ? */
                }
                /*
                if ((RetVal != NAND_ERR_NONE) || (BlkAddr < TotalNumBlk)) {
                    RetVal = NAND_ERR_BAD_BLOCK_OVER;
                    break;
                }
                */
                AmbaMisra_TypeCast32(&pPartEntry[i].StartBlkAddr, &GoodBlk); /* start Block Address */
                BlkAddr = pPartEntry[i].StartBlkAddr + 1U;                   /* BlkAddr = (pPartEntry->StartBlkAddr + 1) */

                /* allocate good blocks for the partition */
                j = (pPartEntry[i].BlkCount - 1U);
                while (j > 0U) {
                    if (BlkAddr >= TotalNumBlk) {
                        RetVal = NAND_ERR_BAD_BLOCK_OVER;    /* no more good block ? */
                        break;
                    }

                    if (AmbaRTSL_NandGetBlkMark(BlkAddr) == AMBA_NAND_BLK_GOOD) {
                        j--;    /* one good block is allocated to this partition */
                    } else {
                        pPartEntry[i].BlkCount ++;    /* there's bad block add 1 blocks to this partition */
                    }
                    BlkAddr += 1U;
                }
            }

            /* Configure User Partition Table */
            /* Configure remaining user partitions, except the partition of VENDOR_DATA */
            if (RetVal == OK) {
                RetVal = AmbaRTSL_NandCreateUserPTB(&BlkAddr);
            }
        }
    }

    pAmbaNandTblUserPart->BlkAddrNextPTB = BlkAddr;
    Rtsl_UserPtbNextBlock = BlkAddr;

    return RetVal;
}


/*
 *  @RoutineName:: AmbaRTSL_NandCreatePTB
 *
 *  @Description:: Create NAND System and User Partition Tables
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
UINT32 AmbaRTSL_NandCreateExtUserPTB(UINT32 StartBlk)
{
    const AMBA_PARTITION_CONFIG_s *pSysPartConfig  = AmbaRTSL_NandCtrl.pSysPartConfig;
    const AMBA_PARTITION_CONFIG_s *pUserPartConfig = AmbaRTSL_NandCtrl.pUserPartConfig;
    AMBA_PARTITION_ENTRY_s  *pPartEntry;
    UINT32 i, j, BlkByteSize;
    INT32 GoodBlk;
    UINT32 BlkAddr = 0U;
    UINT32 TotalNumBlk = AmbaRTSL_NandCtrl.TotalNumBlk;                /* Total number of blocks */
    UINT32 RetVal = OK;

    if ((pSysPartConfig == NULL) || (pUserPartConfig == NULL)) {
        RetVal = NAND_ERR_ARG;
    } else {

        /* Block size in Byte */
        pAmbaNandTblSysPart->BlkByteSize = AmbaRTSL_NandCtrl.BlkByteSize;
        BlkByteSize = AmbaRTSL_NandCtrl.BlkByteSize;

        pAmbaNandTblUserPart->PTBNumber = 1U;
        pAmbaNandTblUserPart->BlkAddrCurentPTB = StartBlk;
        pAmbaNandTblUserPart->BlkAddrNextPTB = 0U;
        BlkAddr = StartBlk;

        /* Configure User Partition Table */
        /* Configure remaining user partitions, except the partition of VENDOR_DATA */
        pAmbaNandTblUserPart->PtbMagic = AMBA_USER_PTB_MAGIC;
        pPartEntry = &(pAmbaNandTblUserPart->Entry[0]);
        for (i = 1U; i < (AMBA_NUM_USER_PARTITION - 1U); i += 1U) {

            if ((pUserPartConfig[i].Attribute & AMBA_PARTITION_ATTR_BACKUP) == 0U) {
                continue;
            }

            pPartEntry[i].ByteCount      = pUserPartConfig[i].ByteCount;        /* number of Bytes for the Partition */
            pPartEntry[i].BlkCount       = GetRoundUpValU32(pPartEntry[i].ByteCount, BlkByteSize);  /* number of Blocks for the Partition  */

            if (pPartEntry[i].ByteCount == 0U) {
                pPartEntry[i].StartBlkAddr = 0U;
                pPartEntry[i].BlkCount     = 0U;
                continue;
            }

            if (CheckBits(pPartEntry[i].Attribute, AMBA_PARTITION_ATTR_FTL) != 0x0U) {
                pPartEntry[i].BlkCount += AmbaRTSL_NandFtlGetNumReservedBlk(pPartEntry[i].BlkCount);
            }

            GoodBlk = AmbaRTSL_NandFindGoodBlkForward(BlkAddr);
            if (GoodBlk < 0) {
                RetVal = NAND_ERR_BAD_BLOCK_OVER;    /* no more good block ? */
            }
            /*
                if ((RetVal != NAND_ERR_NONE) || (BlkAddr < TotalNumBlk)) {
                    RetVal = NAND_ERR_BAD_BLOCK_OVER;
                    break;
                }
            */
            AmbaMisra_TypeCast32(&j, &GoodBlk);

            pPartEntry[i].StartBlkAddr = j;   /* start Block Address */
            BlkAddr = j + 1U;                /* BlkAddr = (pPartEntry->StartBlkAddr + 1) */

            /* allocate good blocks for the partition */
            j = (pPartEntry[i].BlkCount - 1U);
            while (j > 0U) {
                if (BlkAddr >= TotalNumBlk) {
                    RetVal = NAND_ERR_BAD_BLOCK_OVER;    /* no more good block ? */
                    break;
                }

                if (AmbaRTSL_NandGetBlkMark(BlkAddr) == AMBA_NAND_BLK_GOOD) {
                    j -= 1U;    /* one good block is allocated to this partition */
                } else {
                    pPartEntry[i].BlkCount ++;    /* there's bad block add 1 blocks to this partition */
                }
                BlkAddr += 1U;
            }
        }
    }


    return RetVal;
}


/*
 *  @RoutineName:: AmbaRTSL_NandReadSysPTB
 *
 *  @Description:: Read NAND System Partition Table (Block 0, Page 2)
 *
 *  @Input      ::
 *      pSysPartTable: pointer to the buffer of System partition table
 *
 *  @Output     ::
 *      pSysPartTable: pointer to the data of System partition table
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
UINT32 AmbaRTSL_NandReadSysPTB(AMBA_SYS_PARTITION_TABLE_s *pSysPartTable)
{
    UINT8 *pWorkBufMain = AmbaRTSL_FioCtrl.pWorkBufMain;
    UINT32 RetVal;

    /* Read System partition table from NAND which is located at block-0 page-2 */
    RetVal = AmbaRTSL_NandOpRead(AmbaRTSL_NandCtrl.BstPageCount, AmbaRTSL_NandCtrl.SysPtblPageCount, pWorkBufMain, NULL);
    if (RetVal == NAND_ERR_NONE) {
        /* copy to the user area while needed */
        if (pSysPartTable != NULL) {
            AmbaRTSL_NAND_memcpy(pSysPartTable, pWorkBufMain, sizeof(AMBA_SYS_PARTITION_TABLE_s));
        } else {
            /* copy to the DRAM memory */
            AmbaRTSL_NAND_memcpy(pAmbaNandTblSysPart, pWorkBufMain, sizeof(AMBA_SYS_PARTITION_TABLE_s));
        }
    }

    return RetVal;
}

/*
 *  @RoutineName:: AmbaRTSL_NandWriteSysPTB
 *
 *  @Description:: Write NAND System Partition Table (Block 0, Page 2)
 *
 *  @Input      ::
 *      pSysPartTable: pointer to the data of System partition table
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
UINT32 AmbaRTSL_NandWriteSysPTB(AMBA_SYS_PARTITION_TABLE_s *pSysPartTable)
{
    UINT32 BstPageCount     = AmbaRTSL_NandCtrl.BstPageCount;      /* Number of Pages for BST */
    UINT32 SysPtblPageCount = AmbaRTSL_NandCtrl.SysPtblPageCount;  /* Number of Pages for System Partition Table */
    UINT8  *pWorkBufMain    = AmbaRTSL_FioCtrl.pWorkBufMain;
    AMBA_SYS_PARTITION_TABLE_s *pWorkSysPartTable;
    UINT32 RetVal = NAND_ERR_NONE;

    if (pSysPartTable != NULL) {
        /* update NAND through User defined Partition Table */
        pWorkSysPartTable = pSysPartTable;
    } else {
        /* update NAND through DRAM Partition Table */
        pWorkSysPartTable = pAmbaNandTblSysPart;
    }

    /* 1. Read BST data (block 0, page 0) */
    if (OK != AmbaRTSL_NandOpRead(0, BstPageCount, pWorkBufMain, NULL)) {
        RetVal = NAND_ERR_IO_FAIL;
    } else {
        /* 2. Erase the block 0 */
        if (AmbaRTSL_NandOpEraseBlock(0) != OK) {
            RetVal = NAND_ERR_IO_FAIL;
        } else {
            /* 3. Write back BST data (block 0, page 0) */
            if (AmbaRTSL_NandOpProgram(0, BstPageCount, pWorkBufMain, NULL) != OK) {
                RetVal = NAND_ERR_IO_FAIL;
            } else {
                /* 4. Calculate CRC32 */
                const UINT8 *pBuf = NULL;
                UINT32 CrcSize = (UINT32)((sizeof(AMBA_SYS_PARTITION_TABLE_s) - 4U) & 0xffffffffU);
                AmbaMisra_TypeCast(&pBuf, &pWorkSysPartTable);
                pWorkSysPartTable->CRC32 = IO_UtilityCrc32(pBuf, CrcSize);

                /* 5. Write new System Partition Table (block 0, page 2) */
                RetVal = AmbaRTSL_NandOpProgram(BstPageCount, SysPtblPageCount, (UINT8 *) pWorkSysPartTable, NULL);
            }
        }
    }
    return RetVal;
}

/*
 *  @RoutineName:: AmbaRTSL_NandReadUserPTB
 *
 *  @Description:: Read NAND User Partition Table (Block 1, Page 0)
 *
 *  @Input      ::
 *      pSysPartTable: pointer to the buffer of User partition table
 *
 *  @Output     ::
 *      pSysPartTable: pointer to the data of User partition table
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
UINT32 AmbaRTSL_NandReadUserPTB(AMBA_USER_PARTITION_TABLE_s *pUserPartTable, UINT32 UserPtbNo)
{
    const AMBA_NAND_COMMON_INFO_s *pNandDevInfo = AmbaRTSL_NandCommonInfo;     /* Pointer to external NAND device information */
    UINT8 *pWorkBufMain = AmbaRTSL_FioCtrl.pWorkBufMain;
    UINT32 RetVal = NAND_ERR_IO_FAIL;
    UINT32 UserPtblBlkAddr;
    INT32 GoodBlk;

    if (UserPtbNo == 0U) {
        UserPtblBlkAddr = pAmbaNandTblSysPart->BlkAddrUserPTB;
    } else {
        UserPtblBlkAddr = pAmbaNandTblSysPart->BlkAddrUserPTB;
        GoodBlk = AmbaRTSL_NandFindGoodBlkForward((UserPtblBlkAddr + 1U));

        if (GoodBlk < 0) {
            RetVal = NAND_ERR_BAD_BLOCK_OVER;    /* no more good block ? */
        } else {
            AmbaMisra_TypeCast32(&UserPtblBlkAddr, &GoodBlk);
        }
    }

    /* Read User partition table from NAND */
    if (RetVal != NAND_ERR_BAD_BLOCK_OVER) {
        RetVal = AmbaRTSL_NandOpRead(UserPtblBlkAddr * pNandDevInfo->BlockPageSize,
                                     AmbaRTSL_NandCtrl.UserPtblPageCount, pWorkBufMain, NULL);
    }
    if (RetVal == NAND_ERR_NONE) {
        /* copy to the user area while needed */
        if (pUserPartTable != NULL) {
            AmbaRTSL_NAND_memcpy(pUserPartTable, pWorkBufMain, sizeof(AMBA_USER_PARTITION_TABLE_s));
        } else {
            /* copy to the DRAM memory */
            AmbaRTSL_NAND_memcpy(pAmbaNandTblUserPart, pWorkBufMain, sizeof(AMBA_USER_PARTITION_TABLE_s));
        }
    }

    return RetVal;
}

/*
 *  @RoutineName:: AmbaRTSL_NandWriteUserPTB
 *
 *  @Description:: Write NAND User Partition Table (Block 1, Page 0)
 *
 *  @Input      ::
 *      pSysPartTable: pointer to the data of User partition table
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
UINT32 AmbaRTSL_NandWriteUserPTB(AMBA_USER_PARTITION_TABLE_s *pUserPartTable, UINT32 UserPtbNo)
{
    const AMBA_NAND_DEV_INFO_s *pNandDevInfo = AmbaRTSL_NandCommonInfo;
    UINT32 UserPtblBlkAddr;
    UINT32 UserPtblPageAddr;
    UINT32 UserPtblPageCount;
    AMBA_USER_PARTITION_TABLE_s *pWorkUserPartTable = pUserPartTable;
    UINT32 RetVal = NAND_ERR_NONE;
    INT32 GoodBlk;

    UserPtblBlkAddr = pAmbaNandTblSysPart->BlkAddrUserPTB;  /* Block address of User Partition Table */

    if (UserPtbNo != 0U) {
        GoodBlk = AmbaRTSL_NandFindGoodBlkForward((UserPtblBlkAddr + 1U));

        if (GoodBlk < 0) {
            RetVal = NAND_ERR_BAD_BLOCK_OVER;    /* no more good block ? */
        } else {
            AmbaMisra_TypeCast32(&UserPtblBlkAddr, &GoodBlk);
        }
    }

    UserPtblPageAddr  = UserPtblBlkAddr * pNandDevInfo->BlockPageSize;
    UserPtblPageCount = AmbaRTSL_NandCtrl.UserPtblPageCount;            /* Number of Pages for User Partition Table */

    if (pUserPartTable != NULL) {
        /* update NAND through User defined Partition Table */
        pWorkUserPartTable = pUserPartTable;
    } else {
        /* update NAND through DRAM Partition Table */
        pWorkUserPartTable = pAmbaNandTblUserPart;
    }

#if 0
    /* 1. Read metadata */
    UINT8  *pWorkBufMain = AmbaRTSL_FioCtrl.pWorkBufMain;
    UINT32 VendorDataPageCount;
    UINT32 VendorDataPageAddr;
    VendorDataPageCount = AmbaRTSL_NandCtrl.VendorDataPageCount;        /* Number of Pages for Vendor Specific Data */
    VendorDataPageAddr  = UserPtblPageAddr + UserPtblPageCount;
    RetVal = AmbaRTSL_NandOpRead(VendorDataPageAddr, VendorDataPageCount, pWorkBufMain, NULL);
#endif

    /* 2. Erase the block-1 */
    if (OK != AmbaRTSL_NandOpEraseBlock(UserPtblBlkAddr)) {
        RetVal = NAND_ERR_IO_FAIL;
    }

#if 0
    if (RetVal == NAND_ERR_NONE) {
        /* 3. Write back Vendor Data (block 1, page 1) */
        RetVal = AmbaRTSL_NandOpProgram(VendorDataPageAddr, VendorDataPageCount, pWorkBufMain, NULL);
    }
#endif
    if (RetVal == NAND_ERR_NONE) {
        /* 4. Calculate CRC32 */
        const UINT8 * pTmp = NULL;
        AmbaMisra_TypeCast(&pTmp, &pWorkUserPartTable);
        pWorkUserPartTable->CRC32 = IO_UtilityCrc32(pTmp, (UINT32)(AMBA_USER_PTB_CRC32_SIZE & 0xffffffffU));

        /* 5. Write new User partition table (block 1, page 0) */
        RetVal = AmbaRTSL_NandOpProgram(UserPtblPageAddr, UserPtblPageCount, (UINT8 *) pWorkUserPartTable, NULL);
    }

    return RetVal;
}

/*
 *  @RoutineName:: AmbaRTSL_NandGetPartEntry
 *
 *  @Description:: get partition entry
 *
 *  @Input      ::
 *      PartFlag: 0 - System Partition; 1 - User Partition
 *      PartID:   NAND flash partition ID
 *
 *  @Output     ::
 *
 *  @Return     ::
 *          AMBA_PARTITION_ENTRY_s * : pointer to the partition entry/NG(NULL)
 */
AMBA_PARTITION_ENTRY_s *AmbaRTSL_NandGetPartEntry(UINT32 PartFlag, UINT32 PartID)
{
    AMBA_PARTITION_ENTRY_s *pPartEntry;

    if (PartFlag != 0U) {
        /* User Partition */
        if (PartID >= AMBA_NUM_USER_PARTITION) {
            pPartEntry = NULL;    /* Wrong Parameter */
        } else {
            pPartEntry = (AMBA_PARTITION_ENTRY_s *) & (pAmbaNandTblUserPart->Entry[PartID]);
        }
    } else {
        /* System Partition */
        if (PartID >= AMBA_NUM_SYS_PARTITION) {
            pPartEntry = NULL;    /* Wrong Parameter */
        } else {
            pPartEntry = (AMBA_PARTITION_ENTRY_s *) & (pAmbaNandTblSysPart->Entry[PartID]);
        }
    }

    return pPartEntry;
}

/*
 *  @RoutineName:: AmbaRTSL_NandReadPartitionInfo
 *
 *  @Description:: Read partition information
 *
 *  @Input      ::
 *      PartFlag: 0 - System Partition; 1 - User Partition
 *      PartID:   NAND flash partition ID
 *
 *  @Output     ::
 *      pPartInfo: pointer to partition information
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
UINT32 AmbaRTSL_NandReadPartitionInfo(UINT32 PartFlag, UINT32 PartID, AMBA_PARTITION_ENTRY_s *pPartInfo)
{
    UINT32 Rval = NAND_ERR_NONE;
    const AMBA_PARTITION_ENTRY_s *pPartEntry = AmbaRTSL_NandGetPartEntry(PartFlag, PartID);

    if ((pPartInfo == NULL) || (pPartEntry == NULL)) {
        Rval = NAND_ERR_ARG;
    } else {
        AmbaRTSL_NAND_memcpy(pPartInfo, pPartEntry, sizeof(AMBA_PARTITION_ENTRY_s));
    }

    return Rval;
}

/*
 *  @RoutineName:: AmbaRTSL_NandReadPartition
 *
 *  @Description:: Read partition data from NAND flash
 *
 *  @Input      ::
 *      PartFlag: 0 - System Partition; 1 - User Partition
 *      PartID:   NAND flash partition ID
 *
 *  @Output     ::
 *      pDataBuf: pointer to partition data
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
UINT32 AmbaRTSL_NandReadPartition(UINT32 PartFlag, UINT32 PartID, UINT8 *pDataBuf)
{
    const AMBA_NAND_COMMON_INFO_s *pNandDevInfo = AmbaRTSL_NandCommonInfo;
    UINT32 BlkAddr, BlkCount, PageAddr, BlkPageSize, BlkByteSize, LoadSize = 0;
    const AMBA_PARTITION_ENTRY_s *pPartEntry = AmbaRTSL_NandGetPartEntry(PartFlag, PartID);
    UINT32 RetVal = NAND_ERR_NONE, Index = 0U;

    if ((pDataBuf == NULL) || (pPartEntry == NULL) ||
        (pPartEntry->ActualByteSize == 0U) || (pPartEntry->ImageCRC32 == 0xffffffffU)) {
        RetVal = NAND_ERR_ARG;  /* Wrong Parameters */
    } else {
        BlkPageSize = pNandDevInfo->BlockPageSize;
        BlkByteSize = pNandDevInfo->MainByteSize * BlkPageSize;

        BlkCount = (pPartEntry->ActualByteSize + BlkByteSize - 1U) / BlkByteSize;
        if (BlkCount > pPartEntry->BlkCount) {
            BlkCount = pPartEntry->BlkCount;  /* should never happen */
        }

        BlkAddr = pPartEntry->StartBlkAddr;
        while (BlkCount > 0U) {
            if (AmbaRTSL_NandGetBlkMark(BlkAddr) == AMBA_NAND_BLK_GOOD) {
                PageAddr = BlkAddr * BlkPageSize;
                if (OK != AmbaRTSL_NandOpRead(PageAddr, BlkPageSize, &pDataBuf[Index], NULL)) {
                    RetVal = NAND_ERR_IO_FAIL;
                }
                LoadSize += BlkByteSize;
                Index += BlkByteSize;
                BlkCount -= 1U;

                if ((LoadSize >= pPartEntry->ActualByteSize) || (RetVal == NAND_ERR_IO_FAIL)) {
                    break;
                }
            }
            BlkAddr += 1U;
        }
    }
    return RetVal;
}

static void CalcPartReadSize(UINT32 Size, UINT32 PageByteSize, UINT32 BlkPageSize, UINT32 PageAddr, UINT32 *pPages, UINT32 *pDataSize)
{
    if (Size >= PageByteSize) {
        if ((PageAddr % BlkPageSize) == 0U) {
            *pPages = BlkPageSize;
        } else {
            /* Read to block boubdary */
            *pPages = BlkPageSize - (PageAddr % BlkPageSize);
        }
        if (*pPages > (Size / PageByteSize)) {
            *pPages = Size / PageByteSize;
        }

        *pDataSize = *pPages * PageByteSize;
    } else {
        *pDataSize = Size;
    }
}

static UINT32 CheckPageAddr(const AMBA_PARTITION_ENTRY_s *pPartEntry, UINT32 BlkPageSize, UINT32 *pPageAddr, UINT32 Count)
{
    UINT32 BlkMark = AmbaRTSL_NandGetBlkMark(Count + pPartEntry->StartBlkAddr);
    if (BlkMark != AMBA_NAND_BLK_GOOD) {
        *pPageAddr += BlkPageSize;
    }
    return (*pPageAddr / BlkPageSize);
}

static void GetGoodBlockOffset(const AMBA_PARTITION_ENTRY_s *pPartEntry, UINT32 BlkPageSize, UINT32 *pPageAddr)
{
    UINT32 i = 0;
    while (i < CheckPageAddr(pPartEntry, BlkPageSize, pPageAddr, i)) {
        i++;
    }
}

/*
 *  @RoutineName:: AmbaRTSL_NandReadPartitionPartial
 *
 *  @Description:: Read partition data from NAND flash
 *
 *  @Input      ::
 *      PartFlag: 0 - System Partition; 1 - User Partition
 *      PartID:   NAND flash partition ID
 *
 *  @Output     ::
 *      pDataBuf: pointer to partition data
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
 */
UINT32 AmbaRTSL_NandReadPartitionPart(UINT32 PartFlag, UINT32 PartID, UINT32 StartPos, UINT32 Size, UINT8 *pDataBuf)
{
    const AMBA_PARTITION_ENTRY_s  *pPartEntry   = AmbaRTSL_NandGetPartEntry(PartFlag, PartID);
    const AMBA_NAND_COMMON_INFO_s *pNandDevInfo = AmbaRTSL_NandCommonInfo;
    UINT32 BlkPageSize  = pNandDevInfo->BlockPageSize;
    UINT32 PageByteSize = pNandDevInfo->MainByteSize;
    UINT8 *pWorkBufMain = AmbaRTSL_FioCtrl.pWorkBufMain;

    UINT32 BlkAddr, PageAddr;
    UINT32 Pages = 1U, FirstPageSize, DataSize;
    UINT32 DataBufIdx, StartPosTmp = StartPos;
    UINT32 RetVal = OK;
    UINT32 SizeTmp = Size;

    if (SizeTmp == 0U) {
        RetVal = OK;
    } else if ((pDataBuf == NULL) || (pPartEntry == NULL)) {
        RetVal = NAND_ERR_ARG;  /* Wrong Parameter */
    } else {
        /* Convert StartPos from file position to ROM partition position */
        PageAddr = StartPosTmp / PageByteSize;
        StartPosTmp %= PageByteSize;
        DataBufIdx = 0;
        if (StartPosTmp == 0U) {
            FirstPageSize = 0;
        } else {
            StartPosTmp %= PageByteSize;
            FirstPageSize = PageByteSize - StartPosTmp;
        }

        GetGoodBlockOffset(pPartEntry, BlkPageSize, &PageAddr);

        PageAddr += (pPartEntry->StartBlkAddr * BlkPageSize);
        while ((RetVal == OK) && (SizeTmp > 0U)) {
            BlkAddr = PageAddr / BlkPageSize;
            if (BlkAddr >= (pPartEntry->StartBlkAddr + pPartEntry->BlkCount)) {
                RetVal = NAND_ERR_ARG;    /* out of partition range */
            } else {
                Pages = 1U;

                if (AmbaRTSL_NandGetBlkMark(BlkAddr) != AMBA_NAND_BLK_GOOD) {
                    Pages = BlkPageSize;
                    continue;
                }

                if (0x0U != FirstPageSize) {
                    const UINT8 *pTmpPtr = &pDataBuf[DataBufIdx];
                    ULONG Addr = 0;
                    AmbaMisra_TypeCast(&Addr, &pTmpPtr);
                    if (OK != AmbaRTSL_NandOpRead(PageAddr, 1U, pWorkBufMain, NULL)) {
                        RetVal = NAND_ERR_IO_FAIL;
                    }
                    if (FirstPageSize >= SizeTmp) {
                        AmbaRTSL_NAND_memcpy(&pDataBuf[DataBufIdx], &pWorkBufMain[StartPosTmp], SizeTmp);
#ifndef CONFIG_QNX
                        (void)AmbaRTSL_CacheCleanData(Addr, SizeTmp);
#endif
                        break;
                    }
                    AmbaRTSL_NAND_memcpy(&pDataBuf[DataBufIdx], &pWorkBufMain[StartPosTmp], FirstPageSize);

                    /* Misrac */
#ifndef CONFIG_QNX
                    (void)AmbaRTSL_CacheCleanData(Addr, FirstPageSize);
#endif
                    PageAddr     += 1U;
                    SizeTmp      -= FirstPageSize;
                    DataBufIdx   += FirstPageSize;
                    StartPosTmp  -= FirstPageSize;
                    FirstPageSize = 0;
                }

                CalcPartReadSize(Size, PageByteSize, BlkPageSize, PageAddr, &Pages, &DataSize);

                /* Read one page size from file */
                if (DataSize >= PageByteSize) {
                    if (OK != AmbaRTSL_NandOpRead(PageAddr, Pages, &pDataBuf[DataBufIdx], NULL)) {
                        RetVal = NAND_ERR_IO_FAIL;
                    }
                } else {
                    const UINT8 *pTmpPtr = &pDataBuf[DataBufIdx];
                    ULONG Addr = 0;
                    AmbaMisra_TypeCast(&Addr, &pTmpPtr);

                    if (OK != AmbaRTSL_NandOpRead(PageAddr, 1U, pWorkBufMain, NULL)) {
                        RetVal = NAND_ERR_IO_FAIL;
                    }
                    AmbaRTSL_NAND_memcpy(&pDataBuf[DataBufIdx], pWorkBufMain, DataSize);
#ifndef CONFIG_QNX
                    (void)AmbaRTSL_CacheCleanData(Addr, DataSize);
#endif
                    /* Misrac */
                }

                if (RetVal == OK) {
                    SizeTmp    -= DataSize;
                    DataBufIdx += DataSize;
                    StartPosTmp = 0;
                }
            }
            PageAddr += Pages;
        }
    }

    return RetVal;
}

static UINT32 AmbaRTSL_NandSyncPTB(AMBA_PARTITION_ENTRY_s *pPartEntry, UINT32 DataSize, const UINT8 *pDataBuf)
{
    UINT32 RetVal = NAND_ERR_NONE;

    /* Sync partition info if the partition is nonbackup partition */
    if (CheckBits(pPartEntry->Attribute, AMBA_PARTITION_ATTR_BACKUP) == 0x0U) {
        /* Switch to the other user PTB */
        if (pAmbaNandTblUserPart->PTBNumber == 0U) {
            RetVal = AmbaRTSL_NandReadUserPTB(pAmbaNandTblUserPart, 1U);
        } else {
            RetVal = AmbaRTSL_NandReadUserPTB(pAmbaNandTblUserPart, 0U);
        }

        pPartEntry->Attribute = ClearBits(pPartEntry->Attribute, AMBA_PARTITION_ATTR_INVALID);
        pPartEntry->Attribute = ClearBits(pPartEntry->Attribute, AMBA_PARTITION_ATTR_ERASED);
        pPartEntry->ActualByteSize = DataSize;
        pPartEntry->ProgramStatus  = 0U;
        pPartEntry->ImageCRC32     = IO_UtilityCrc32(pDataBuf, DataSize);

        if (RetVal == OK) {
            /* Update the user PTB */
            RetVal = AmbaRTSL_NandWriteUserPTB(pAmbaNandTblUserPart, pAmbaNandTblUserPart->PTBNumber);
        }

        if (RetVal == OK) {
            /* Switch to the original user PTB */
            if (pAmbaNandTblUserPart->PTBNumber == 0U) {
                RetVal = AmbaRTSL_NandReadUserPTB(pAmbaNandTblUserPart, 1U);
            } else {
                RetVal = AmbaRTSL_NandReadUserPTB(pAmbaNandTblUserPart, 0U);
            }
        }
    }
    return RetVal;
}

/*
 *  @RoutineName:: AmbaRTSL_NandWritePartition
 *
 *  @Description:: Write partition data from NAND flash
 *
 *  @Input      ::
 *      PartFlag: 0 - System Partition; 1 - User Partition
 *      PartID:   NAND flash partition ID
 *      DataSize: Data size in Byte to write to the partition
 *      pDataBuf: pointer to partition data
 *
 *  @Output     ::
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
UINT32 AmbaRTSL_NandWritePartition(UINT32 PartFlag, UINT32 PartID, UINT32 DataSize, UINT8 *pDataBuf)
{
#if 1
    const AMBA_NAND_COMMON_INFO_s *pNandDevInfo = AmbaRTSL_NandCommonInfo;
    UINT32 BlkAddr, BlkCount = 0U, PageAddr, BlkPageSize = 0U, BlkByteSize = 0U, WriteSize = 0U;
    AMBA_PARTITION_ENTRY_s *pPartEntry = AmbaRTSL_NandGetPartEntry(PartFlag, PartID);
    UINT32 RetVal = NAND_ERR_NONE, BbtUpdateFlag = 0;
    UINT32 BufferIdx = 0U;

    if ((pPartEntry == NULL) || (pDataBuf == NULL) || (DataSize == 0U)) {
        RetVal = NAND_ERR_ARG;
    } else {
        BlkPageSize = pNandDevInfo->BlockPageSize;
        BlkByteSize = pNandDevInfo->MainByteSize * BlkPageSize;

        BlkCount = (DataSize + BlkByteSize - 1U) / BlkByteSize;
        if (BlkCount > pPartEntry->BlkCount) {
            RetVal = NAND_ERR_ARG;  /* should never happen */
        }

        if (RetVal == NAND_ERR_NONE) {
            /* 1. Program blocks in target partition */
            BlkAddr = pPartEntry->StartBlkAddr;
            PageAddr = BlkAddr * BlkPageSize;
            while (BlkCount > 0U) {
                if (AmbaRTSL_NandGetBlkMark(BlkAddr) == AMBA_NAND_BLK_GOOD) {
                    /* it is a good block */
                    if (0U != AmbaRTSL_NandOpEraseBlock(BlkAddr)) {
                        RetVal = NAND_ERR_IO_FAIL;
                    }

                    if (RetVal == NAND_ERR_NONE) {
                        if (0U != AmbaRTSL_NandOpProgram(PageAddr, BlkPageSize, &pDataBuf[BufferIdx], NULL)) {
                            RetVal = NAND_ERR_IO_FAIL;
                        }
                    }
                    if (RetVal == NAND_ERR_IO_FAIL) {
                        /* program failed. mark as runtime bad block. */
                        AmbaRTSL_NandSetBlkMark(BlkAddr, AMBA_NAND_BLK_RUNTIME_BAD);
                        BbtUpdateFlag = 1U;
                        BlkCount -= 1U;
                        BlkAddr += 1U;
                        PageAddr += BlkPageSize;
                        continue;
                    } else {
                        BufferIdx += BlkByteSize;
                        WriteSize += BlkByteSize;
                    }
                    if (WriteSize >= DataSize) {
                        break;
                    }
                }
                BlkCount -= 1U;
                BlkAddr += 1U;
                PageAddr += BlkPageSize;
            }
        }

        /* 2. Update NAND BBTs if necessary */
        if (BbtUpdateFlag == 1U) {
            AmbaRTSL_NandCtrl.BbtVersion += 1U;
            RetVal = AmbaRTSL_NandUpdateBBT((UINT32)AmbaRTSL_NandCtrl.BbtVersion, AmbaRTSL_NandBlkAddrPrimaryBBT, AmbaRTSL_NandBlkAddrMirrorBBT);
        }

        /* 3. Update partition table */
        if (RetVal == NAND_ERR_NONE) {
            pPartEntry->Attribute = ClearBits(pPartEntry->Attribute, AMBA_PARTITION_ATTR_INVALID);
            pPartEntry->Attribute = ClearBits(pPartEntry->Attribute, AMBA_PARTITION_ATTR_ERASED);
            pPartEntry->ActualByteSize = DataSize;
            pPartEntry->ProgramStatus  = 0U;
            pPartEntry->ImageCRC32     = IO_UtilityCrc32(pDataBuf, DataSize);

            if (0x0U != PartFlag) {
                /* User Partition */
                RetVal = AmbaRTSL_NandWriteUserPTB(NULL, pAmbaNandTblUserPart->PTBNumber);
                if (RetVal == OK) {
                    /* Sync partition info if the partition is nonbackup partition */
                    RetVal = AmbaRTSL_NandSyncPTB(pPartEntry, DataSize, pDataBuf);
                }
            } else {
                /* System Partition */
                RetVal = AmbaRTSL_NandWriteSysPTB(NULL);
            }
        }
    }
    return RetVal;
#else
    AmbaMisra_TouchUnused(&PartFlag);
    AmbaMisra_TouchUnused(&PartID);
    AmbaMisra_TouchUnused(&DataSize);
    AmbaMisra_TouchUnused(pDataBuf);

    return 0;
#endif
}

/*
 *  @RoutineName:: AmbaRTSL_NandErasePartition
 *
 *  @Description:: Erase a partition
 *
 *  @Input      ::
 *      PartFlag: 0 - System Partition; 1 - User Partition
 *      PartID:   NAND flash partition ID
 *
 *  @Output     ::
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
UINT32 AmbaRTSL_NandErasePartition(UINT32 PartFlag, UINT32 PartID)
{
    UINT32 RetVal = NAND_ERR_NONE, BbtUpdateFlag = 0;
    AMBA_PARTITION_ENTRY_s *pPartEntry = AmbaRTSL_NandGetPartEntry(PartFlag, PartID);
    UINT32 BlkAddr, BlkCount, BlkMark;

    if (pPartEntry == NULL) {
        RetVal = NAND_ERR_ARG;  /* Wrong Parameter */
    } else {
        /* 1. Erase all blocks in target partition */
        BlkCount = pPartEntry->BlkCount;
        BlkAddr  = pPartEntry->StartBlkAddr;
        while (BlkCount > 0U) {
            BlkMark = AmbaRTSL_NandGetBlkMark(BlkAddr);
            if (BlkMark != AMBA_NAND_BLK_FACTORY_BAD) {
                if (OK != AmbaRTSL_NandOpEraseBlock(BlkAddr)) {
                    RetVal = NAND_ERR_IO_FAIL;
                }
                if ((RetVal == OK) && (BlkMark == AMBA_NAND_BLK_RUNTIME_BAD)) {
                    /* erase successfully. mark as good block. */
                    AmbaRTSL_NandSetBlkMark(BlkAddr, AMBA_NAND_BLK_GOOD);
                    BbtUpdateFlag = 1U;
                } else if ((RetVal != OK) && (BlkMark == AMBA_NAND_BLK_GOOD)) {
                    /* erase failed. mark as runtime bad block. */
                    AmbaRTSL_NandSetBlkMark(BlkAddr, AMBA_NAND_BLK_RUNTIME_BAD);
                    BbtUpdateFlag = 1U;
                } else {
                    /* for misraC checking, do nothing */
                }
            }
            BlkCount--;
            BlkAddr++;
        }

        /* 2. Update NAND BBTs if necessary */
        if (0x0U != BbtUpdateFlag) {
            AmbaRTSL_NandCtrl.BbtVersion++;
            RetVal = AmbaRTSL_NandUpdateBBT(AmbaRTSL_NandCtrl.BbtVersion, AmbaRTSL_NandBlkAddrPrimaryBBT, AmbaRTSL_NandBlkAddrMirrorBBT);
        }

        /* 3. Update partition table */
        if (RetVal == NAND_ERR_NONE) {
            pPartEntry->Attribute = SetBits(pPartEntry->Attribute, AMBA_PARTITION_ATTR_ERASED);  /* 1 - Erased */
            pPartEntry->ActualByteSize = 0;
            pPartEntry->ProgramStatus  = 0xffffffffU;
            pPartEntry->ImageCRC32     = 0xffffffffU;
            if (0x0U != PartFlag) {
                /* User Partition */
                RetVal = AmbaRTSL_NandWriteUserPTB(NULL, pAmbaNandTblUserPart->PTBNumber);
                if (RetVal == OK) {
                    /* Sync partition info if the partition is nonbackup partition */
                    RetVal = AmbaRTSL_NandSyncPTB(pPartEntry, 0U, NULL);
                }
            } else {
                /* System Partition */
                RetVal = AmbaRTSL_NandWriteSysPTB(NULL);
            }
        }
    }

    return RetVal;
}

/*
 *  @RoutineName:: AmbaRTSL_NandIsBldMagicCodeSet
 *
 *  @Description:: check if Bootloader Magic Code has been set for System Software load or not
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : Yes(1)/No(0)
 */
UINT32 AmbaRTSL_NandIsBldMagicCodeSet(void)
{
#if 0
    const UINT32 *pBldMagicCode = NULL;
    UINT32 Addr = AMBA_ROM_MAGIC_CODE_VIRT_ADDR;

    AmbaMisra_TypeCast32(&pBldMagicCode, &Addr);

    return (pAmbaNandTblUserPart->BldMagicCode == *pBldMagicCode) ? 1U : 0;
#else
    return 0;
#endif
}

#if 0
/*
 *  @RoutineName:: AmbaRTSL_NandSetBldMagicCode
 *
 *  @Description:: Write Bootloader Magic Code onto NAND device
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
UINT32 AmbaRTSL_NandSetBldMagicCode(void)
{
    UINT32 RetVal = NAND_ERR_NONE;
    const UINT32 *pBldMagicCode = NULL;
    UINT32 Addr = AMBA_ROM_MAGIC_CODE_VIRT_ADDR;

    AmbaMisra_TypeCast32(&pBldMagicCode, &Addr);

    if (pAmbaNandTblUserPart->BldMagicCode != *pBldMagicCode) {
        /* Write Bootloader Magic Code */
        pAmbaNandTblUserPart->BldMagicCode = *pBldMagicCode; /* Bootloader Magic Code */
        RetVal = AmbaRTSL_NandWriteUserPTB(NULL);
    }

    return RetVal;
}
#endif
/*
 *  @RoutineName:: AmbaRTSL_NandEraseBldMagicCode
 *
 *  @Description:: Erase Bootloader Magic Code from NAND device
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
UINT32 AmbaRTSL_NandEraseBldMagicCode(void)
{
    UINT32 RetVal = 0;
    UINT32 PTBActive;

    (void)AmbaRTSL_NandGetActivePtbNo(&PTBActive);

    if (pAmbaNandTblUserPart->BldMagicCode == AMBA_USER_PTB_MAGIC) {
        /* Erase Bootloader Magic Code */
        pAmbaNandTblUserPart->BldMagicCode = 0xffffffffU; /* Bootloader Magic Code */
        RetVal = AmbaRTSL_NandWriteUserPTB(NULL, PTBActive);
    }

    return RetVal;
}

/*
 *  @RoutineName:: AmbaRTSL_NandReadBldMagicCode
 *
 *  @Description:: Read Bootloader Magic Code
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          UINT32 : Bootloader Magic Code
 */
UINT32 AmbaRTSL_NandReadBldMagicCode(void)
{
    return pAmbaNandTblUserPart->BldMagicCode;
}

void AmbaRTSL_NandSetWritePtbFlag(UINT32 Flag)
{
    WritePtbFlag = Flag;
}
