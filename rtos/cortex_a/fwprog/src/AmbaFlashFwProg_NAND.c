/**
 *  @file AmbaFlashFwProg_NAND.c
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
 *  @details NAND flash firmware program utilities APIs
 *
 */

#include "AmbaTypes.h"
#include "AmbaDef.h"

#include "AmbaMisraFix.h"
#include "AmbaWrap.h"
#include "AmbaUtility.h"
#include "AmbaSYS.h"

#include "AmbaNVM_Partition.h"
#include "AmbaFlashFwProg.h"

#include "AmbaRTSL_NAND.h"
#include "AmbaRTSL_NAND_Ctrl.h"
#include "AmbaRTSL_NAND_BBM.h"

#include "AmbaRTSL_UART.h"
#include "AmbaRTSL_Cache.h"
#include "AmbaRTSL_GIC.h"
#include "AmbaRTSL_USB.h"

#include "AmbaCSL_RCT.h"

#include "AmbaRTSL_NAND_OP.h"

struct fdt_header {
    UINT32  magic;               /* magic word FDT_MAGIC */
    UINT32  totalsize;           /* total size of DT block */
    UINT32  off_dt_struct;       /* offset to structure */
    UINT32  off_dt_strings;      /* offset to strings */
    UINT32  off_mem_rsvmap;      /* offset to memory reserve map */
    UINT32  version;             /* format version */
    UINT32  last_comp_version;   /* last compatible version */
    /* version 2 fields below */
    UINT32  boot_cpuid_phys;     /* Which physical CPU id we're booting on */
    /* version 3 fields below */
    UINT32  size_dt_strings;     /* size of the strings block */
    /* version 17 fields below */
    UINT32  size_dt_struct;      /* size of the structure block */
};

#define FDT_MAGIC   (0xd00dfeedU)  /* 4: version, 4: total size */

/* conversion between little-endian and big-endian */
static inline UINT32 uswap_32(UINT32 x)
{
    return ((((x) & 0xff000000U) >> 24U) |
            (((x) & 0x00ff0000U) >> 8U)  |
            (((x) & 0x0000ff00U) << 8U)  |
            (((x) & 0x000000ffU) << 24U));
}

#define LINE_BUFFER_LENGTH   (4096U)
static UINT8 _AmbaPrint_LineBuf[LINE_BUFFER_LENGTH] = {0}; /* Temporary buffer */

void AmbaPrint_PrintStr5Fwprog(const char *pFmt, const char *pArg1, const char *pArg2, const char *pArg3, const char *pArg4, const char *pArg5)
{
    char *pString;
    const UINT8 *pBuf = _AmbaPrint_LineBuf;
    const char *Arg[5];
    UINT32  ActualTxSize, UartTxSize, Index = 0;

    Arg[0] = pArg1;
    Arg[1] = pArg2;
    Arg[2] = pArg3;
    Arg[3] = pArg4;
    Arg[4] = pArg5;

    AmbaMisra_TypeCast64(&pString, &pBuf);
    UartTxSize = AmbaUtility_StringPrintStr(pString, LINE_BUFFER_LENGTH, pFmt, 5U, Arg);

    while (AmbaRTSL_UartWrite(AMBA_UART_APB_CHANNEL0, UartTxSize, &_AmbaPrint_LineBuf[Index], &ActualTxSize) == UART_ERR_NONE) {
        UartTxSize -= ActualTxSize;
        Index += ActualTxSize;
        if (UartTxSize == 0U) {
            break;
        }
    }
}

void AmbaPrint_PrintUInt5Fwprog(const char *pFmt, UINT32 Arg1, UINT32 Arg2, UINT32 Arg3, UINT32 Arg4, UINT32 Arg5)
{
    char *pString;
    const UINT8 *pBuf = _AmbaPrint_LineBuf;
    UINT32 Arg[5];
    UINT32 ActualTxSize, UartTxSize, Index = 0;

    Arg[0] = Arg1;
    Arg[1] = Arg2;
    Arg[2] = Arg3;
    Arg[3] = Arg4;
    Arg[4] = Arg5;

    AmbaMisra_TypeCast64(&pString, &pBuf);
    UartTxSize = AmbaUtility_StringPrintUInt32(pString, LINE_BUFFER_LENGTH, pFmt, 5U, Arg);

    while (AmbaRTSL_UartWrite(AMBA_UART_APB_CHANNEL0, UartTxSize, &_AmbaPrint_LineBuf[Index], &ActualTxSize) == UART_ERR_NONE) {
        UartTxSize -= ActualTxSize;
        Index += ActualTxSize;
        if (UartTxSize == 0U) {
            break;
        }
    }
}

static void AmbaFlashFwProg_SetPloadInfo(void)
{
    extern UINT32 Region0RoBase, Region0RoLength, Region0RwBase, Region0RwLength;
    extern UINT32 Region1RoBase, Region1RoLength, Region1RwBase, Region1RwLength;
    extern UINT32 Region2RoBase, Region2RoLength, Region2RwBase, Region2RwLength;
    extern UINT32 Region3RoBase, Region3RoLength, Region3RwBase, Region3RwLength;
    extern UINT32 Region4RoBase, Region4RoLength, Region4RwBase, Region4RwLength;
    extern UINT32 Region5RoBase, Region5RoLength, Region5RwBase, Region5RwLength;

    extern UINT32 LinkerStubBase, LinkerStubLength;
    extern UINT32 DspBufBase, DspBufLength;

    AMBA_PLOAD_PARTITION_s *pPloadInfo = &(pAmbaNandTblUserPart->PloadInfo);
    (void)AmbaWrap_memset(pPloadInfo, 0x0, sizeof(AMBA_PLOAD_PARTITION_s));

    pPloadInfo->RegionRoStart[0] = Region0RoBase;
    pPloadInfo->RegionRoSize [0] = Region0RoLength;
    pPloadInfo->RegionRwStart[0] = Region0RwBase;
    pPloadInfo->RegionRwSize [0] = Region0RwLength;

    pPloadInfo->RegionRoStart[1] = Region1RoBase;
    pPloadInfo->RegionRoSize [1] = Region1RoLength;
    pPloadInfo->RegionRwStart[1] = Region1RwBase;
    pPloadInfo->RegionRwSize [1] = Region1RwLength;

    pPloadInfo->RegionRoStart[2] = Region2RoBase;
    pPloadInfo->RegionRoSize [2] = Region2RoLength;
    pPloadInfo->RegionRwStart[2] = Region2RwBase;
    pPloadInfo->RegionRwSize [2] = Region2RwLength;

    pPloadInfo->RegionRoStart[3] = Region3RoBase;
    pPloadInfo->RegionRoSize [3] = Region3RoLength;
    pPloadInfo->RegionRwStart[3] = Region3RwBase;
    pPloadInfo->RegionRwSize [3] = Region3RwLength;

    pPloadInfo->RegionRoStart[4] = Region4RoBase;
    pPloadInfo->RegionRoSize [4] = Region4RoLength;
    pPloadInfo->RegionRwStart[4] = Region4RwBase;
    pPloadInfo->RegionRwSize [4] = Region4RwLength;

    pPloadInfo->RegionRoStart[5] = Region5RoBase;
    pPloadInfo->RegionRoSize [5] = Region5RoLength;
    pPloadInfo->RegionRwStart[5] = Region5RwBase;
    pPloadInfo->RegionRwSize [5] = Region5RwLength;

    pPloadInfo->LinkerStubStart  = LinkerStubBase;
    pPloadInfo->LinkerStubSize   = LinkerStubLength;
    pPloadInfo->DspBufStart      = DspBufBase;
    pPloadInfo->DspBufSize       = DspBufLength;
}

static INT32 AmbaFlashFwProg_NandProgramPage(UINT32 Block, UINT32 Page, UINT8 *pMainBuf)
{
    static UINT8 FlashProgChkBuf[4U * 1024U] GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE;

    const AMBA_NAND_DEV_INFO_s *pNandDevInfo = AmbaRTSL_NandCommonInfo;
    INT32 RetVal = 0, CmpResult;
    UINT32 PageAddr;

    PageAddr = (Block * pNandDevInfo->BlockPageSize) + Page;

    /* Program a page */
    if (OK != AmbaRTSL_NandOpProgram(PageAddr, 1U, pMainBuf, NULL)) {
        AmbaPrint_PrintUInt5Fwprog("program failed. <block %d, page %d> = \r\n", Block, Page, 0, 0, 0);
        RetVal = FLPROG_ERR_PROG_IMG;
    }

    /* Read it back for verification */
    if (RetVal == FLPROG_OK) {
        if (OK != AmbaRTSL_NandOpRead(PageAddr, 1U, FlashProgChkBuf, NULL)) {
            AmbaPrint_PrintUInt5Fwprog("read failed. <block %d, page %d>\r\n", Block, Page, 0, 0, 0);
            RetVal = FLPROG_ERR_FIFO_READ;
        }
    }

    /* Compare memory content after read back */
    if (RetVal == FLPROG_OK) {
        (void)AmbaWrap_memcmp(pMainBuf, FlashProgChkBuf, pNandDevInfo->MainByteSize, &CmpResult);
        if (CmpResult != 0) {
            AmbaPrint_PrintUInt5Fwprog("check failed. <block %d, page %d>\r\n", Block, Page, 0, 0, 0);
            RetVal = FLPROG_ERR_PROG_IMG;
        }
    }
    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaFlashFwProg_NandProgram
 *
 *  @Description:: Program the partition to NAND device.
 *
 *  @Input      ::
 *          UINT8 *pRaw     : Pointer to raw image.
 *          UINT32 RawSize  : Raw image size.
 *          UINT32 StartBlk : Start block to be programmed.
 *          UINT32 NumBlk   : Number of blocks to be programmed
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static INT32 AmbaFlashFwProg_NandProgram(UINT8 *pRaw, UINT32 RawSize, UINT32 StartBlk, UINT32 NumBlks)
{
    static UINT8 FlashProgZeroBuf[4U * 1024U] GNU_ALIGNED_CACHESAFE = {0};
    INT32 RetVal = 0;
    UINT32 Percentage = 0, PreOffset, Offset = 0, FirmOK = 0;
    const AMBA_NAND_DEV_INFO_s *pNandDevInfo = AmbaRTSL_NandCommonInfo;

    for (UINT32 Block = StartBlk; Block < (StartBlk + NumBlks); Block++) {
        UINT32 BlockType = AmbaRTSL_NandGetBlkMark(Block);
        if (BlockType == AMBA_NAND_BLK_FACTORY_BAD) {
            AmbaPrint_PrintUInt5Fwprog("initial bad block. block: %d\r\n Try next block...\r\n", Block, 0, 0, 0, 0);
            RetVal = 0;
        } else {
            if (BlockType == AMBA_NAND_BLK_RUNTIME_BAD) {
                /* If it's runtime bad, since we are still trying to reprogram below, so we need to mark it's good first */
                AmbaPrint_PrintUInt5Fwprog("runtime bad block block: %d...\r\n", Block, 0, 0, 0, 0);
                AmbaRTSL_NandSetBlkMark(Block, AMBA_NAND_BLK_GOOD);
                AmbaPrint_PrintStr5Fwprog("mark Good block.\r\n", NULL, NULL, NULL, NULL, NULL);
            }

            RetVal = AmbaRTSL_NandOpEraseBlock(Block);
            if (RetVal != OK) {
                AmbaPrint_PrintUInt5Fwprog("erase failed. Try next block... block: %d \r\n", Block, 0, 0, 0, 0);
                AmbaRTSL_NandSetBlkMark(Block, AMBA_NAND_BLK_RUNTIME_BAD);
                AmbaPrint_PrintStr5Fwprog("mark bad block failed.\r\n", NULL, NULL, NULL, NULL, NULL);
                RetVal = FLPROG_ERR_PROG_IMG;
            }
        }

        /* erase the unused block after program ok */
        if ((BlockType == AMBA_NAND_BLK_FACTORY_BAD) || (FirmOK == 1U) || (RetVal != FLPROG_OK)) {
            continue;
        }

        PreOffset = Offset;

        /* Program each page */
        for (UINT32 Page = 0; Page < pNandDevInfo->BlockPageSize; Page++) {
            UINT8 *pMainBuf;

            if (FirmOK != 0U) {
                pMainBuf = FlashProgZeroBuf; /* Force to program all unused pages to 0 */
            } else {
                pMainBuf = &pRaw[Offset];
            }

            RetVal = AmbaFlashFwProg_NandProgramPage(Block, Page, pMainBuf);

            if (FirmOK == 0U) {
                Offset += pNandDevInfo->MainByteSize;
                if (Offset >= RawSize) {
                    FirmOK = 1U;
                }
            }
            if (RetVal != FLPROG_OK) {
                break;
            }
        }

        if (RetVal < 0) {
            Offset = PreOffset;
            AmbaRTSL_NandSetBlkMark(Block, AMBA_NAND_BLK_RUNTIME_BAD);
            AmbaPrint_PrintUInt5Fwprog("mark bad block failed. <block %d>...\r\n", Block, 0, 0, 0, 0);
            AmbaPrint_PrintUInt5Fwprog("late developed bad block. <block %d>\r\n", Block, 0, 0, 0, 0);
            RetVal = 0;
        } else {
            if (Offset != 0U) {
                if (Offset >= RawSize) {
                    Percentage = 100U;
                } else {
                    Percentage = Offset / (RawSize / 100U);
                }
            }

            AmbaPrint_PrintUInt5Fwprog("\r%3u%%\r", Percentage, 0, 0, 0, 0);

            /* AmbaPrint_PrintUInt5Fwprog("\r\x1b[A\x1b[32C\x1b[0K%3u%%\r\n", Percentage, 0, 0, 0, 0); */
        }
    }

    AmbaPrint_PrintUInt5Fwprog("\r\n", 0, 0, 0, 0, 0);

    if (OK != AmbaRTSL_NandUpdateBBT(AmbaRTSL_NandCtrl.BbtVersion, AmbaRTSL_NandBlkAddrPrimaryBBT,
                                     AmbaRTSL_NandBlkAddrMirrorBBT)) {
        AmbaPrint_PrintStr5Fwprog("AmbaRTSL_NandUpdateBBT FAIL", NULL, NULL, NULL, NULL, NULL);
    }

    if ((RetVal < 0) || (FirmOK == 0U)) {
        RetVal = FLPROG_ERR_PROG_IMG;
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaFwProg_ProgramSysPartition
 *
 *  @Description:: Program to a particular partition. (Only support NAND flash.)
 *
 *  @Input      ::
 *          INT32 PartID      : Partition ID.
 *          UINT8 *pImage   : Pointer to the programmed image.
 *          UINT32 Len      : Image length.
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
INT32 AmbaFwProg_ProgramSysPartition(UINT32 PartID, UINT8 *pImage)
{
    INT32 RetVal = 0;
    const AMBA_IMG_HEADER *pHeader = NULL;
    UINT8 *pRaw;
    UINT32 StartBlk = 0, NumBlks = 0;

    AMBA_PARTITION_ENTRY_s *pPartEntry = (AMBA_PARTITION_ENTRY_s  *) & (pAmbaNandTblSysPart->Entry[PartID]);

    if (PartID >= AMBA_NUM_SYS_PARTITION) {
        RetVal = -1;
    } else {
        //AmbaPrint_PrintStr5Fwprog("\rProgram image to NAND flash ...\r\n", NULL, NULL, NULL, NULL, NULL);

        AmbaMisra_TypeCast64(&pHeader, &pImage);
        pRaw = &pImage[sizeof(AMBA_IMG_HEADER)];

        StartBlk = pPartEntry->StartBlkAddr;
        NumBlks  = pPartEntry->BlkCount;

        AmbaPrint_PrintUInt5Fwprog("\rProgram image to NAND flash(0x%X)\r\n", StartBlk, 0, 0, 0, 0);
        if (pHeader->ImgLen > AmbaRTSL_NandCtrl.pSysPartConfig[PartID].ByteCount) {
            AmbaPrint_PrintStr5Fwprog("\r%s length is bigger than partition size\r\n", (char *)pPartEntry->PartitionName, NULL, NULL, NULL, NULL);
            RetVal = FLPROG_ERR_LENGTH;
        } else {
            if (PartID == AMBA_SYS_PARTITION_BOOTSTRAP) {
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
                if (pHeader->ImgLen > AMBA_NAND_BOOTSTRAP_CODE_SPACE_SIZE) {
                    AmbaPrint_PrintStr5Fwprog("\rBST length is bigger than 8KB\r\n", NULL, NULL, NULL, NULL, NULL);
                    RetVal = FLPROG_ERR_LENGTH;    /* BST must fit in 4KB for FIO FIFO */
                }
#else
                if (pHeader->ImgLen > 4096U) {
                    AmbaPrint_PrintStr5Fwprog("\rBST length is bigger than 4KB\r\n", NULL, NULL, NULL, NULL, NULL);
                    RetVal = FLPROG_ERR_LENGTH;    /* BST must fit in 4KB for FIO FIFO */
                }
#endif
            }
            if (RetVal == FLPROG_OK) {
                RetVal = AmbaFlashFwProg_NandProgram(pRaw, pHeader->ImgLen, StartBlk, NumBlks);
            }

            /* Update the PTB's entry */
            if (RetVal == FLPROG_OK) {
                pPartEntry->ActualByteSize = pHeader->ImgLen;
                pPartEntry->Attribute      = AmbaRTSL_NandCtrl.pSysPartConfig[PartID].Attribute;
                pPartEntry->RamLoadAddr    = pHeader->MemAddr;
                pPartEntry->ImageCRC32     = pHeader->Crc32;

                (void)AmbaRTSL_NandWriteSysPTB(pAmbaNandTblSysPart);
            }
        }
    }
    return RetVal;
}

static INT32 AmbaFwProg_UpdatePartEntry(UINT32 PartID, const AMBA_IMG_HEADER *pHeader, UINT32 StartBlk)
{
    AMBA_PARTITION_ENTRY_s *pPartEntry = &(pAmbaNandTblUserPart->Entry[PartID]);
    INT32 RetVal = FLPROG_OK;
    UINT32 BlkAddr, BadBlockCount = 0;
    UINT32 BlkByteSize = pAmbaNandTblSysPart->BlkByteSize;

    AmbaPrint_PrintUInt5Fwprog("\rProgram image to NAND flash(0x%X)\r\n", StartBlk, 0, 0, 0, 0);

    if (pHeader->ImgLen > AmbaRTSL_NandCtrl.pUserPartConfig[PartID].ByteCount) {
        AmbaPrint_PrintStr5Fwprog("%s length is bigger than partition size\r\n", (char *)pPartEntry->PartitionName, NULL, NULL, NULL, NULL);
        RetVal = FLPROG_ERR_LENGTH;
    } else {
        pPartEntry->StartBlkAddr   = StartBlk;   /* start Block Address */
        pPartEntry->ByteCount      = AmbaRTSL_NandCtrl.pUserPartConfig[PartID].ByteCount;   /* number of Bytes for the Partition */
        pPartEntry->ActualByteSize = pHeader->ImgLen;                                        /* actual size in Bytes */
        pPartEntry->BlkCount       = GetRoundDownValU32(pPartEntry->ByteCount, BlkByteSize); /* number of Blocks for the Partition  */

        BlkAddr = pPartEntry->StartBlkAddr + 1U;

        BadBlockCount = 0;
        /* allocate good blocks for the partition */
        for (UINT32 j = (pPartEntry->BlkCount - 1U); j > 0U; BlkAddr++) {
            if (BlkAddr >= AmbaRTSL_NandCtrl.TotalNumBlk) {
                RetVal = -1;    /* no more good block ? */
                break;
            }

            if (AmbaRTSL_NandGetBlkMark(BlkAddr) == AMBA_NAND_BLK_GOOD) {
                j--;    /* one good block is allocated to this partition */
            } else {
                BadBlockCount++;
            }
        }
        if (BadBlockCount != 0U) {
            pPartEntry->BlkCount += BadBlockCount;
        }
    }
    return RetVal;
}

#ifndef CONFIG_LINUX
static INT32 AmbaFwProg_ProgramDTB(const AMBA_IMG_HEADER *pHeader, UINT32 StartBlk)
{
    INT32 RetVal = FLPROG_OK;
    UINT8 *pRaw;
    const AMBA_PARTITION_ENTRY_s *pPartEntry = &(pAmbaNandTblUserPart->Entry[AMBA_USER_PARTITION_LINUX_KERNEL]);
    const struct fdt_header * pFdt = NULL;
    extern UINT32 begin_dtb_image;

    /* Reserved the last block for DTB. */
    UINT32 i = AmbaRTSL_NandCtrl.pUserPartConfig[AMBA_USER_PARTITION_LINUX_KERNEL].ByteCount / pPartEntry->BlkCount;
    UINT32 DtbBlkNum = pHeader->ImgLen / i;
    if ((pHeader->ImgLen % i) != 0U) {
        DtbBlkNum += 1U;
    }
    DtbBlkNum += 1U;

    if (DtbBlkNum > pPartEntry->BlkCount) {
        AmbaPrint_PrintStr5Fwprog("Please reserve 1 block for Linux DTB!\r\n\r\n", NULL, NULL, NULL, NULL, NULL);
        RetVal = FLPROG_ERR_LENGTH;
    } else {
        pHeader = NULL;
        pRaw    = NULL;
        AmbaMisra_TypeCast32(&pHeader, &begin_dtb_image);  //pHeader = (AMBA_IMG_HEADER *)begin_dtb_image;
        AmbaMisra_TypeCast32(&pRaw, &begin_dtb_image);     //pRaw = (UINT8 *) begin_dtb_image;
        pRaw = &pRaw[sizeof(AMBA_IMG_HEADER)];

        AmbaMisra_TypeCast64(&pFdt, &pRaw);

        if ((uswap_32(pFdt->magic) == FDT_MAGIC) && (uswap_32(pFdt->version) >= 17U)) {
            AmbaPrint_PrintStr5Fwprog("\r\nDTB found in firmware!\r\n", NULL, NULL, NULL, NULL, NULL);
            AmbaPrint_PrintStr5Fwprog("\rProgram DTB to the last block .\r\n", NULL, NULL, NULL, NULL, NULL);

            AmbaMisra_TypeCast32(&pHeader, &begin_dtb_image);  //pHeader = (AMBA_IMG_HEADER *) begin_dtb_image;
            AmbaMisra_TypeCast32(&pRaw, &begin_dtb_image);     //pRaw = (UINT8 *) begin_dtb_image;
            pRaw = &pRaw[sizeof(AMBA_IMG_HEADER)];

            RetVal = AmbaFlashFwProg_NandProgram(pRaw, pHeader->ImgLen, (StartBlk + pPartEntry->BlkCount - 1U), 1U);
            /* NumBlks -= 1; */
        } else {
            AmbaPrint_PrintStr5Fwprog("\r\nDTB is not found in firmware!"
                                      "\r\nLinux can't be booted!\r\n\r\n", NULL, NULL, NULL, NULL, NULL);
            RetVal = FLPROG_ERR_EXTRAS_MAGIC;
        }
    }
    return RetVal;
}
#endif

#if defined(CONFIG_APP_FLOW_CARDV_AONI)
void AmbaFWProg_SetUserActivePartititon(void)
{
    AMBA_PARTITION_ENTRY_s *pPartEntry;

    pPartEntry = &(pAmbaNandTblUserPart->Entry[AMBA_USER_PARTITION_SYS_SOFTWARE]);
    pPartEntry->Attribute |= FW_PARTITION_ACTIVE_FLAG;
    pPartEntry = &(pAmbaNandTblUserPart->Entry[AMBA_USER_PARTITION_SYS_SOFTWARE_B]);
    pPartEntry->Attribute &= ~FW_PARTITION_ACTIVE_FLAG;
    pPartEntry->ActualByteSize = 0U;//disable part b
    pAmbaNandTblUserPart->ErrorCode = 0U;
    AmbaPrint_PrintStr5Fwprog("Set FW-A Active\r\n", 0, 0, 0, 0, 0);
    AmbaRTSL_NandWriteUserPTB(pAmbaNandTblUserPart, pAmbaNandTblUserPart->PTBNumber);
}
#endif

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaFwProg_ProgramUserPartition
 *
 *  @Description:: Program to a particular partition. (Only support NAND flash.)
 *
 *  @Input      ::
 *          INT32 PartID      : Partition ID.
 *          UINT8 *pImage   : Pointer to the programmed image.
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
INT32 AmbaFwProg_ProgramUserPartition(UINT32 PartID, UINT8 *pImage)
{
    AMBA_PARTITION_ENTRY_s *pPartEntry = &(pAmbaNandTblUserPart->Entry[PartID]);
    UINT8 *pRaw;
    INT32 RetVal = FLPROG_OK;
    UINT32 StartBlk = 0;
    const AMBA_IMG_HEADER *pHeader = NULL;

    AmbaMisra_TypeCast64(&pHeader, &pImage);

    pRaw = &pImage[sizeof(AMBA_IMG_HEADER)];

    if ((PartID >= AMBA_NUM_USER_PARTITION) || (PartID == AMBA_USER_PARTITION_PTB)) {
        RetVal = -1;
    } else {
        if (RetVal == FLPROG_OK) {
            pPartEntry = &(pAmbaNandTblUserPart ->Entry[PartID]);
            StartBlk = pPartEntry->StartBlkAddr;

            if (AmbaRTSL_NandFindGoodBlkForward(StartBlk) < 0) {
                RetVal = FLPROG_ERR_NOMOREBLOCK;    /* no more good block ? */
            } else {
                StartBlk = (UINT32)AmbaRTSL_NandFindGoodBlkForward(StartBlk);
            }
        }
    }

    if (RetVal == FLPROG_OK) {
        RetVal = AmbaFwProg_UpdatePartEntry(PartID, pHeader, StartBlk);
    }

    if (RetVal == FLPROG_OK) {
        RetVal = AmbaFlashFwProg_NandProgram(pRaw, pHeader->ImgLen, StartBlk, pPartEntry->BlkCount);
    }

    /* Update the PTB's entry */
    if (RetVal >= FLPROG_OK) {
        pPartEntry->ActualByteSize = pHeader->ImgLen;
        pPartEntry->Attribute      = AmbaRTSL_NandCtrl.pUserPartConfig[PartID].Attribute;
        pPartEntry->RamLoadAddr    = pHeader->MemAddr;
        pPartEntry->ImageCRC32     = pHeader->Crc32;

        if (PartID == AMBA_USER_PARTITION_SYS_SOFTWARE) {
            AmbaFlashFwProg_SetPloadInfo();
        }

        (void) AmbaRTSL_NandWriteUserPTB(pAmbaNandTblUserPart, pAmbaNandTblUserPart->PTBNumber);

        /* Sync partition info if the partition is nonbackup partition */
        if ((CheckBits(pPartEntry->Attribute, AMBA_PARTITION_ATTR_BACKUP) == 0x0U) && (RetVal == FLPROG_OK)) {
            /* Switch to the other user PTB */
            if (pAmbaNandTblUserPart->PTBNumber == 0U) {
                RetVal = AmbaRTSL_NandReadUserPTB(pAmbaNandTblUserPart, 1U);
            } else {
                RetVal = AmbaRTSL_NandReadUserPTB(pAmbaNandTblUserPart, 0U);
            }

            pPartEntry->ActualByteSize = pHeader->ImgLen;
            pPartEntry->Attribute      = AmbaRTSL_NandCtrl.pUserPartConfig[PartID].Attribute;
            pPartEntry->RamLoadAddr    = pHeader->MemAddr;
            pPartEntry->ImageCRC32     = pHeader->Crc32;

            if (RetVal == FLPROG_OK) {
                /* Update the user PTB */
                RetVal = AmbaRTSL_NandWriteUserPTB(pAmbaNandTblUserPart, pAmbaNandTblUserPart->PTBNumber);
            }

            if (RetVal == FLPROG_OK) {
                /* Switch to the original user PTB */
                if (pAmbaNandTblUserPart->PTBNumber == 0U) {
                    RetVal = AmbaRTSL_NandReadUserPTB(pAmbaNandTblUserPart, 1U);
                } else {
                    RetVal = AmbaRTSL_NandReadUserPTB(pAmbaNandTblUserPart, 0U);
                }
            }
        }

        if ((PartID == AMBA_USER_PARTITION_SYS_SOFTWARE) ||
            (PartID == AMBA_USER_PARTITION_LINUX_KERNEL)) {
            (void)AmbaRTSL_NandErasePartition(1U, AMBA_USER_PARTITION_LINUX_HIBERNATION_IMG);
        }

#ifndef CONFIG_LINUX
        if (PartID == AMBA_USER_PARTITION_LINUX_KERNEL) {
            RetVal = AmbaFwProg_ProgramDTB(pHeader, StartBlk);
        }
#endif
    }

    return RetVal;
}

void AmbaFlashFwProg_ReturnToUsb(void)
{
    extern UINT32 *__memfwprog_command;
    //volatile UINT32 *pCmd  = (volatile UINT32 *) &__memfwprog_command;
    UINT32 Cmd[8];

    (void)AmbaWrap_memcpy(Cmd, &__memfwprog_command, sizeof(Cmd));

    if (pAmbaRCT_Reg->SysConfig.UsbBoot != 0U) {
#if !defined (CONFIG_SOC_CV5) && !defined (CONFIG_SOC_CV52)
        if (AmbaRTSL_UsbGetDevConfig() != 0x28U) {
#endif
            //if (Cmd[7] == 0x7E57U) {
            UINT32 Status = 0;
            UINT32 Addr = (UINT32) 0x0;

            AmbaMisra_TouchUnused(&Addr);

            do {
                (void)AmbaRTSL_UartGetStatus(AMBA_UART_APB_CHANNEL0, &Status);
            } while ((Status & 0x3U) == 0);

            AmbaRTSL_CacheFlushDataAll();
            AmbaRTSL_GicIntGlobalDisable();
            __asm__ volatile ("br  %0": : "r" ((Addr)));
            //} else {
            //UINT32 Status = 0;
            //do {
            //    (void)AmbaRTSL_UartGetStatus(AMBA_UART_APB_CHANNEL0, &Status);
            //} while ((Status & 0x3U) == 0);
            //(void)AmbaSYS_Reboot();
            //}
#if !defined (CONFIG_SOC_CV5) && !defined (CONFIG_SOC_CV52)
        }
#endif
    }
}

void AmbaFlashFwProg_CheckErasePTB(void)
{
    /* Erase PTB can be trigged by Set "Firmware Program parameter" to special value */
    /* In this example case, we set CMD0 to 0x8524,                                  */
    /* it notice Firmware programmer to erase and re-create partition table          */
#define AMBA_USB_CMD_ERASE_PTB  (0x8524U)
    extern UINT32 *__memfwprog_command;
    UINT32 Cmd[8];

    (void)AmbaWrap_memcpy(Cmd, &__memfwprog_command, sizeof(Cmd));

    if (pAmbaRCT_Reg->SysConfig.UsbBoot != 0U) {
        if (AmbaRTSL_UsbGetDevConfig() != 0x28U) {
            if (Cmd[0] == AMBA_USB_CMD_ERASE_PTB) {
                (void)AmbaWrap_memset(pAmbaNandTblSysPart,  0xff, sizeof(AMBA_SYS_PARTITION_TABLE_s));
                (void)AmbaWrap_memset(pAmbaNandTblUserPart, 0xff, sizeof(AMBA_USER_PARTITION_TABLE_s));
                AmbaRTSL_NandOpEraseBlock(0);
                (void) AmbaRTSL_NandInitPtbBbt();
            }
        }
    }
}

char * AmbaFlashFwProg_GetSysPartitionName(UINT32 PartID)
{
    return (char *)pAmbaNandTblSysPart->Entry[PartID].PartitionName;
}

char * AmbaFlashFwProg_GetUserPartitionName(UINT32 PartID)
{
    return (char *)pAmbaNandTblUserPart->Entry[PartID].PartitionName;
}
