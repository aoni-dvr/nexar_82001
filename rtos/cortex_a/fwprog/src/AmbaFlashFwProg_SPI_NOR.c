/**
 *  @file AmbaFlashFwProg_SPI_NOR.c
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
 *  @details SPI-NOR Flash firmware program utilities APIs
 */

#include "AmbaTypes.h"
#include "AmbaDef.h"

#include "AmbaWrap.h"
#include "AmbaUtility.h"
#include "AmbaSYS.h"

#include "AmbaRTSL_Cache.h"
#include "AmbaRTSL_SpiNOR.h"
#include "AmbaRTSL_SpiNOR_Ctrl.h"
#include "AmbaFlashFwProg.h"
#include "AmbaRTSL_UART.h"
#include "AmbaRTSL_Cache.h"
#include "AmbaCSL_RCT.h"
#include "AmbaRTSL_GIC.h"
#include "AmbaRTSL_USB.h"

#include "AmbaMisraFix.h"

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

#define FDT_MAGIC (0xd00dfeedU)     /* 4: version, 4: total size */

/* conversion between little-endian and big-endian */
#define uswap_32(x) \
    ((((x) & 0xff000000) >> 24) | \
     (((x) & 0x00ff0000) >> 8)  | \
     (((x) & 0x0000ff00) << 8)  | \
     (((x) & 0x000000ff) << 24))

#define fdt32_to_cpu(x)       uswap_32(x)

#define fdt_get_header(fdt, field) \
    (fdt32_to_cpu(((const struct fdt_header *)(fdt))->field))
#define fdt_magic(fdt)   (fdt_get_header(fdt, magic))
#define fdt_version(fdt) (fdt_get_header(fdt, version))

static UINT8 FlashProgChkBuf[2 * 1024 * 1024] GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE;
static UINT8 FlashProgZeroBuf[2 * 1024] GNU_ALIGNED_CACHESAFE = {0};
static UINT8 BstBuf[AMBA_NORSPI_BOOTSTRAP_CODE_SPACE_SIZE + AMBA_NORSPI_BOOT_HEADER_SIZE] GNU_ALIGNED_CACHESAFE GNU_SECTION_NOZEROINIT;

#define LINE_BUFFER_LENGTH   (4096U)
static UINT8 _AmbaPrint_LineBuf[LINE_BUFFER_LENGTH] GNU_SECTION_NOZEROINIT; /* Temporary buffer */

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

void AmbaNorSpi_Setup_Boot_Header(AMBA_NOR_FW_HEADER_s * header)
{
#ifdef CONFIG_SPINOR_DEFAULT_MODE_OCT
    UINT32 AMBA_NOR_CMD_READ = 0x8bU;
#else
    UINT32 AMBA_NOR_CMD_READ = 0x3U;
#endif
    AmbaWrap_memset(header, 0x0, sizeof(AMBA_NOR_FW_HEADER_s));

    /* SPINOR_LENGTH_REG */
#if defined(CONFIG_SOC_CV2FS)
    header->Img.Bits.ImageLen  = 8192U - 1U;
#elif defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    header->Img.Bits.ImageLen  = ((AMBA_NORSPI_BOOTSTRAP_CODE_SPACE_SIZE > 8192) ? 8192 : AMBA_NORSPI_BOOTSTRAP_CODE_SPACE_SIZE) - 1U;
#else
    header->Img.Bits.ImageLen  = AMBA_NORSPI_BOOTSTRAP_CODE_SPACE_SIZE - 1U;
#endif
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    header->Flow.Bits.Clock_Div = 10;
#else
    header->Img.Bits.Clock_Div = 10;
#endif

#ifdef CONFIG_SPINOR_DEFAULT_MODE_OCT
    header->Img.Bits.DummyLen  = 16U;
    header->Img.Bits.AddrLen   = 4U;
    header->DqsEnable          = 1U;
#else
    header->Img.Bits.DummyLen  = 0;
    header->Img.Bits.AddrLen   = 3;
#endif
    header->Img.Bits.CmdLen    = 1;

    /* SPINOR_CTRL_REG */
    header->DTR.Bits.DataReadn   = 1;
    header->DTR.Bits.DataWriteen = 0;
#ifdef CONFIG_SPINOR_DEFAULT_MODE_OCT
    header->DTR.Bits.RxLane      = 0;
    header->DTR.Bits.NumDataLane = 3;
    header->DTR.Bits.NumAddrLane = 3;
    header->DTR.Bits.NumCmdLane  = 3;
    header->DTR.Bits.DataDTR     = 1;
    header->DTR.Bits.AddressDTR  = 1;
#else
    header->DTR.Bits.RxLane      = 1;
    header->DTR.Bits.NumDataLane = 1;
    header->DTR.Bits.NumAddrLane = 0;
    header->DTR.Bits.NumCmdLane  = 0;
    header->DTR.Bits.DataDTR     = 0;
    header->DTR.Bits.AddressDTR  = 0;
#endif

    header->DTR.Bits.DummyDTR    = 0;
    header->DTR.Bits.CmdDTR      = 0;

    /* SPINOR_CFG_REG */
    header->Flow.Bits.RxSampleDelay = 0;
    header->Flow.Bits.ChipSelect = (~(1 << 0)) & 0xff;
    header->Flow.Bits.HoldSwitchphase = 0;
    header->Flow.Bits.Hold = 0;
    header->Flow.Bits.FlowControl = 1;

    /* SPINOR_CMD_REG */
    header->Cmd.Data = AMBA_NOR_CMD_READ;
    /* SPINOR_ADDRHI_REG */
    header->AddrHi.Data = 0x0;

    /* SPINOR_ADDRLO_REG */
    header->AddrLow.Data = 0x0 + sizeof(AMBA_NOR_FW_HEADER_s);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaFlashFwProg_SpiNORProgram
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
INT32 AmbaFlashFwProg_SpiNORProgram(UINT8 *pRaw, UINT32 RawSize, UINT32 StartBlk, UINT32 NumBlks)
{
    INT32 RetVal = 0, FirmOK = 0, CmpResult = 0;
    UINT32 Block, Page, Percentage, PreOffset, Offset = 0;
    AMBA_NORSPI_DEV_INFO_s *pNorDevInfo = AmbaRTSL_NorSpiDevInfo;
    UINT32 PageSize = pNorDevInfo->PageSize;
    UINT32 BlkByteSize = pNorDevInfo->EraseBlockSize;
    UINT32 PagePerBlock;

    PagePerBlock = BlkByteSize / PageSize;
    for (Block = StartBlk; Block < (StartBlk + NumBlks); Block++) {

        RetVal = AmbaRTSL_NorErase(Block * BlkByteSize, BlkByteSize);
        if (RetVal < 0) {
            AmbaPrint_PrintUInt5Fwprog("erase failed. <block %d>\r\n", Block, 0, 0, 0, 0);
            break;
        }

        /* erase the unused block after program ok */
        if (FirmOK == 1)
            break;

        PreOffset = Offset;
        /* Program each page */
        for (Page = 0; Page < PagePerBlock; Page++) {
            UINT32 PageAddr = (Block * PagePerBlock) + Page;
            UINT8 *pMainBuf;

            if (FirmOK)
                pMainBuf = FlashProgZeroBuf; /* Force to program all unused pages to 0 */
            else
                pMainBuf = (UINT8 *)&pRaw[Offset];

            /* Program a page */
            RetVal = AmbaRTSL_NorProgram(PageAddr * PageSize, PageSize, pMainBuf);
            if (RetVal < 0) {
                AmbaPrint_PrintUInt5Fwprog("program failed. <block %d, page %d>\r\n", Block, Page, 0, 0, 0);
                break;
            }

            /* Read it back for verification */
            RetVal = AmbaRTSL_NOR_Readbyte(PageAddr * PageSize, PageSize, FlashProgChkBuf);
            if (RetVal < 0) {
                AmbaPrint_PrintUInt5Fwprog("read failed. <block %d, page %d>\r\n", Block, Page, 0, 0, 0);
                break;
            }

            /* Compare memory content after read back */
            (void)AmbaWrap_memcmp(pMainBuf, FlashProgChkBuf, PageSize, &CmpResult);
            if (CmpResult != 0) {
                AmbaPrint_PrintUInt5Fwprog("check failed. <block %d, page %d>\r\n", Block, Page, 0, 0, 0);
                RetVal = -1;
                break;
            }
            if (!FirmOK) {
                Offset += PageSize;
                if (Offset >= RawSize) {
                    FirmOK = 1;
                }
            }
        }
        if (RetVal < 0) {
            Offset = PreOffset;
            return RetVal;
        }
        if (Offset >= RawSize)
            Percentage = 100;
        else
            Percentage = Offset / (RawSize / 100);

        AmbaPrint_PrintUInt5Fwprog("\r\x1b[A\x1b[32C\x1b[0K%3u%%\r\n", Percentage, 0, 0, 0, 0);
    }

    if ((RetVal < 0) || (FirmOK == 0U)) {
        RetVal = FLPROG_ERR_PROG_IMG;
    }

    return RetVal;
}

AMBA_NOR_FW_HEADER_s BootHeader;

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
    AMBA_IMG_HEADER *pHeader = NULL;
    UINT8 *pRaw;
    UINT32 StartBlk = 0, NumBlks = 0;
    AMBA_NORSPI_DEV_INFO_s *pNorDevInfo = AmbaRTSL_NorSpiDevInfo;
    UINT32 BlkByteSize = pNorDevInfo->EraseBlockSize;

    AMBA_PARTITION_ENTRY_s *pPartEntry = (AMBA_PARTITION_ENTRY_s *) & (pAmbaRTSL_NorSpiSysPartTable->Entry[PartID]);

    if (PartID >= AMBA_NUM_SYS_PARTITION)
        return -1;

    pHeader = (AMBA_IMG_HEADER *) pImage;
    pRaw = pImage + sizeof(AMBA_IMG_HEADER);

    AmbaPrint_PrintStr5Fwprog("\rProgram image to SpiNOR flash ...\r\n", 0, 0, 0, 0, 0);

    StartBlk = pPartEntry->StartBlkAddr;
    NumBlks  = pPartEntry->BlkCount;

    if (pHeader->ImgLen > AmbaRTSL_NorSpiCtrl.pSysPartConfig[PartID].ByteCount) {
        AmbaPrint_PrintStr5Fwprog("\r%s length is bigger than partition size\r\n", (char *)pPartEntry->PartitionName, 0, 0, 0, 0);
        return FLPROG_ERR_LENGTH;
    }

    if (PartID == AMBA_SYS_PARTITION_BOOTSTRAP) {
        INT32 CmpResult = 0;
        if (pHeader->ImgLen > AMBA_NORSPI_BOOTSTRAP_CODE_SPACE_SIZE) {
            AmbaPrint_PrintStr5Fwprog("\rBST length is bigger than BST size\r\n", 0, 0, 0, 0, 0);
            return FLPROG_ERR_LENGTH;    /* BST must fit in 4KB for FIO FIFO */
        }

        if ((RetVal = AmbaRTSL_NorErase(0, BlkByteSize)) != OK) {
            return RetVal;
        }

        AmbaNorSpi_Setup_Boot_Header(&BootHeader);
        AmbaWrap_memcpy(BstBuf, &BootHeader, sizeof(AMBA_NOR_FW_HEADER_s));
        AmbaWrap_memcpy(BstBuf + sizeof(AMBA_NOR_FW_HEADER_s), pRaw, pHeader->ImgLen);
        RetVal = AmbaRTSL_NorProgram(0, pHeader->ImgLen + sizeof(AMBA_NOR_FW_HEADER_s), BstBuf);
        if (RetVal >= 0) {
            AmbaPrint_PrintStr5Fwprog("\r\x1b[A\x1b[32C\x1b[0K100%%\r\n", 0, 0, 0, 0, 0);
        } else {
            AmbaPrint_PrintStr5Fwprog("\r\x1b[A\x1b[32C\x1b[0K  0%%\r\n", 0, 0, 0, 0, 0);
        }

        AmbaRTSL_NOR_Readbyte(0, pHeader->ImgLen + sizeof(AMBA_NOR_FW_HEADER_s), FlashProgChkBuf);
        (void)AmbaWrap_memcmp(BstBuf, FlashProgChkBuf, pHeader->ImgLen + sizeof(AMBA_NOR_FW_HEADER_s), &CmpResult);
        /* Compare memory content after read back */
        if (CmpResult != 0) {
            AmbaPrint_PrintUInt5Fwprog("check failed. <block , page >\r\n", 0, 0, 0, 0, 0);
        }
    } else {
        RetVal = AmbaFlashFwProg_SpiNORProgram(pRaw, pHeader->ImgLen, StartBlk, NumBlks);
    }

    /* Update the PTB's entry */
    if (RetVal >= 0) {
        pPartEntry->ActualByteSize = pHeader->ImgLen;
        pPartEntry->Attribute      = AmbaRTSL_NorSpiCtrl.pSysPartConfig[PartID].Attribute;
        pPartEntry->RamLoadAddr    = pHeader->MemAddr;
        pPartEntry->ImageCRC32     = pHeader->Crc32;

        AmbaRTSL_NorSpiWriteSysPTB(pAmbaRTSL_NorSpiSysPartTable);
    }
    return RetVal;
}

#ifndef CONFIG_LINUX
#if defined(CONFIG_MUTI_BOOT_DEVICE)
static INT32 AmbaFwProg_ProgramDTB(const AMBA_IMG_HEADER *pHeader, UINT32 StartBlk)
{
    /* Reserved the last block for DTB. */
    INT32 RetVal = FLPROG_OK;
    UINT8 *pRaw;
    extern UINT32 begin_dtb_image;
    extern INT32 AmbaFlashFwProg_EmmcProgram(UINT8 *pRaw, UINT32 RawSize, UINT32 StartBlk, UINT32 NumBlks);
    const AMBA_PARTITION_ENTRY_s *pPartEntry = &(pAmbaRTSL_NorSpiUserPartTable->Entry[AMBA_USER_PARTITION_LINUX_KERNEL]);
    UINT32 Size;
    UINT32 i = AmbaRTSL_NorSpiCtrl.pUserPartConfig[AMBA_USER_PARTITION_LINUX_KERNEL].ByteCount / pPartEntry->BlkCount;
    Size = pHeader->ImgLen / i;
    Size += (pHeader->ImgLen % i) ? 1 : 0;
    Size += 128;

    if (Size > pPartEntry->BlkCount) {
        AmbaPrint_PrintStr5Fwprog("Please reserve 1 block for Linux DTB! \r\n\r\n", 0, 0, 0, 0, 0);
        RetVal = FLPROG_ERR_LENGTH;

    } else {
        AmbaMisra_TypeCast32(&pHeader, &begin_dtb_image);  //pHeader = (AMBA_IMG_HEADER *)begin_dtb_image;
        AmbaMisra_TypeCast32(&pRaw, &begin_dtb_image);     //pRaw = (UINT8 *) begin_dtb_image;
        pRaw = pRaw + sizeof(AMBA_IMG_HEADER);

        if ((fdt_magic(pRaw) == FDT_MAGIC) && (fdt_version(pRaw) >= 17U)) {
            AmbaPrint_PrintStr5Fwprog("\r\nDTB found in firmware!\r\n", 0, 0, 0, 0, 0);
            AmbaPrint_PrintStr5Fwprog("\remmc: Program DTB to the last block\r\n", 0, 0, 0, 0, 0);

            AmbaMisra_TypeCast32(&pHeader, &begin_dtb_image);  //pHeader = (AMBA_IMG_HEADER *)begin_dtb_image;
            AmbaMisra_TypeCast32(&pRaw, &begin_dtb_image);     //pRaw = (UINT8 *) begin_dtb_image;
            pRaw = pRaw + sizeof(AMBA_IMG_HEADER);

            RetVal = AmbaFlashFwProg_EmmcProgram(pRaw, pHeader->ImgLen,
                                                 (StartBlk + pPartEntry->BlkCount - 128),
                                                 (pHeader->ImgLen / i) + 1);
            /* NumBlks -= 1; */
        } else {
            AmbaPrint_PrintStr5Fwprog("\r\nDTB is not found in firmware!"
                                      "\r\nLinux can't be booted!\r\n\r\n", 0, 0, 0, 0, 0);
            RetVal = FLPROG_ERR_EXTRAS_MAGIC;
        }
    }
    return RetVal;
}

#else
static INT32 AmbaFwProg_ProgramDTB(const AMBA_IMG_HEADER *pHeader, UINT32 StartBlk)
{
    INT32 RetVal = FLPROG_OK;
    UINT8 *pRaw;
    const AMBA_PARTITION_ENTRY_s *pPartEntry = &(pAmbaRTSL_NorSpiUserPartTable->Entry[AMBA_USER_PARTITION_LINUX_KERNEL]);
    const struct fdt_header * pFdt = NULL;
    extern UINT32 begin_dtb_image;

    /* Reserved the last block for DTB. */
    UINT32 i = AmbaRTSL_NorSpiCtrl.pUserPartConfig[AMBA_USER_PARTITION_LINUX_KERNEL].ByteCount / pPartEntry->BlkCount;
    UINT32 DtbBlkNum = pHeader->ImgLen / i;
    if ((pHeader->ImgLen % i) != 0U) {
        DtbBlkNum += 1U;
    }
    DtbBlkNum += 1U;

    if (DtbBlkNum > pPartEntry->BlkCount) {
        AmbaPrint_PrintStr5Fwprog("Please reserve 1 block for Linux DTB!\r\n\r\n", NULL, NULL, NULL, NULL, NULL);
        RetVal = FLPROG_ERR_LENGTH;
    } else {
        AmbaMisra_TypeCast32(&pHeader, &begin_dtb_image);  //pHeader = (AMBA_IMG_HEADER *)begin_dtb_image;
        AmbaMisra_TypeCast32(&pRaw, &begin_dtb_image);     //pRaw = (UINT8 *) begin_dtb_image;
        pRaw = &pRaw[sizeof(AMBA_IMG_HEADER)];

        AmbaMisra_TypeCast64(&pFdt, &pRaw);

        if ((uswap_32(pFdt->magic) == FDT_MAGIC) && (uswap_32(pFdt->version) >= 17U)) {

            AmbaPrint_PrintStr5Fwprog("\r\nDTB found in firmware!\r\n", NULL, NULL, NULL, NULL, NULL);
            AmbaPrint_PrintStr5Fwprog("\rspinor: Program DTB to the last block\r\n", NULL, NULL, NULL, NULL, NULL);

            AmbaMisra_TypeCast32(&pHeader, &begin_dtb_image);  //pHeader = (AMBA_IMG_HEADER *) begin_dtb_image;
            AmbaMisra_TypeCast32(&pRaw, &begin_dtb_image);     //pRaw = (UINT8 *) begin_dtb_image;
            pRaw = &pRaw[sizeof(AMBA_IMG_HEADER)];

            RetVal = AmbaFlashFwProg_SpiNORProgram(pRaw, pHeader->ImgLen, (StartBlk + pPartEntry->BlkCount - 1U), 1U);
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
    AMBA_IMG_HEADER *pHeader;
    AMBA_PARTITION_ENTRY_s *pPartEntry;
    UINT8 *pRaw;
    INT32 RetVal = FLPROG_OK, StartBlk;
#if defined(CONFIG_MUTI_BOOT_DEVICE)
    extern int AmbaFlashFwProg_EmmcProgram(UINT8 * pRaw, UINT32 RawSize, UINT32 StartBlk, UINT32 NumBlks);
#endif
    UINT32 BlkByteSize;

    if ((PartID >= AMBA_NUM_USER_PARTITION) || (PartID <= AMBA_USER_PARTITION_PTB))
        return -1;

    pHeader = (AMBA_IMG_HEADER *) pImage;
    pRaw = pImage + sizeof(AMBA_IMG_HEADER);

#if defined(CONFIG_MUTI_BOOT_DEVICE)
    pPartEntry = &(pAmbaRTSL_NorSpiUserPartTable->Entry[PartID]);

    StartBlk = pPartEntry->StartBlkAddr;

    if (pHeader->ImgLen > AmbaRTSL_NorSpiCtrl.pUserPartConfig[PartID].ByteCount) {
        AmbaPrint_PrintStr5Fwprog("%s length is bigger than partition size\r\n", (char *)pPartEntry->PartitionName, 0, 0, 0, 0);
        return FLPROG_ERR_LENGTH;
    }

    BlkByteSize = pAmbaRTSL_NorSpiSysPartTable->BlkByteSize;                                /* actual size in Bytes */

    if (((pPartEntry->Attribute & AMBA_PARTITION_ATTR_STORE_DEVIC) >> 6) == AMBA_NVM_SPI_NOR) {
        AmbaPrint_PrintStr5Fwprog("\rProgram image to SpiNOR flash ...\r\n", 0, 0, 0, 0, 0);
        RetVal = AmbaFlashFwProg_SpiNORProgram(pRaw, pHeader->ImgLen, StartBlk, pPartEntry->BlkCount);
    } else if (((pPartEntry->Attribute & AMBA_PARTITION_ATTR_STORE_DEVIC) >> 6) == AMBA_NVM_eMMC) {
        AmbaPrint_PrintStr5Fwprog("\rProgram image to EMMC ...", 0, 0, 0, 0, 0);
        RetVal = AmbaFlashFwProg_EmmcProgram(pRaw, pHeader->ImgLen, StartBlk, pPartEntry->BlkCount);
    } else {
        RetVal = -1;
    }

    /* Update the PTB's entry */
    if (RetVal >= 0) {
        pPartEntry->ActualByteSize = pHeader->ImgLen;
        pPartEntry->RamLoadAddr    = pHeader->MemAddr;
        pPartEntry->ImageCRC32     = pHeader->Crc32;

        AmbaRTSL_NorSpiWriteUserPTB(pAmbaRTSL_NorSpiUserPartTable, 0U);
    }
#else
    if (PartID > AMBA_USER_PARTITION_SYS_SOFTWARE) {
        UINT32  i;

        for (i = PartID; AMBA_USER_PARTITION_SYS_SOFTWARE < i; i--) {
            pPartEntry = &(pAmbaRTSL_NorSpiUserPartTable->Entry[i - 1]);
            if (0U < pPartEntry->BlkCount) {
                break;
            }
        }

        if (i == AMBA_USER_PARTITION_SYS_SOFTWARE) {
            RetVal = FLPROG_ERR_NOT_READY;
        }
    } else {
        pPartEntry = &(pAmbaRTSL_NorSpiSysPartTable->Entry[AMBA_NUM_SYS_PARTITION - 1]);
    }

    if (RetVal == FLPROG_OK) {
        StartBlk = pPartEntry->StartBlkAddr + pPartEntry->BlkCount;

        pPartEntry = &(pAmbaRTSL_NorSpiUserPartTable->Entry[PartID]);
        AmbaPrint_PrintStr5Fwprog("\rProgram image to SpiNOR flash ...\r\n", 0, 0, 0, 0, 0);

        if (pHeader->ImgLen > AmbaRTSL_NorSpiCtrl.pUserPartConfig[PartID].ByteCount) {
            AmbaPrint_PrintStr5Fwprog("%s length is bigger than partition size\r\n", (char *)pPartEntry->PartitionName, 0, 0, 0, 0);
            return FLPROG_ERR_LENGTH;
        }

        BlkByteSize = pAmbaRTSL_NorSpiSysPartTable->BlkByteSize;

        pPartEntry->StartBlkAddr   = StartBlk;   /* start Block Address */
        pPartEntry->ByteCount      = AmbaRTSL_NorSpiCtrl.pUserPartConfig[PartID].ByteCount;      /* number of Bytes for the Partition */
        pPartEntry->ActualByteSize = pHeader->ImgLen;                                /* actual size in Bytes */
        pPartEntry->BlkCount       = GetRoundUpValU32(pPartEntry->ByteCount, BlkByteSize); /* number of Blocks for the Partition  */

        RetVal = AmbaFlashFwProg_SpiNORProgram(pRaw, pHeader->ImgLen, StartBlk, pPartEntry->BlkCount);

        /* Update the PTB's entry */
        if (RetVal >= 0) {
            pPartEntry->ActualByteSize = pHeader->ImgLen;
            pPartEntry->Attribute      = AmbaRTSL_NorSpiCtrl.pUserPartConfig[PartID].Attribute;
            pPartEntry->RamLoadAddr    = pHeader->MemAddr;
            pPartEntry->ImageCRC32     = pHeader->Crc32;

            AmbaRTSL_NorSpiWriteUserPTB(pAmbaRTSL_NorSpiUserPartTable, 0U);
        }
    }
#endif

    if (RetVal == FLPROG_OK) {
        /* Only erase the header of linux_Hibernation image,the size of
        * header is two blocks */
        if (PartID == AMBA_USER_PARTITION_SYS_SOFTWARE ||
            PartID == AMBA_USER_PARTITION_LINUX_KERNEL) {
            pPartEntry = &(pAmbaRTSL_NorSpiUserPartTable->Entry[AMBA_USER_PARTITION_LINUX_HIBERNATION_IMG]);
            BlkByteSize = pAmbaRTSL_NorSpiSysPartTable->BlkByteSize;

            if (pPartEntry->ActualByteSize > 0) {
                RetVal = AmbaRTSL_NorErase(pPartEntry->StartBlkAddr * BlkByteSize, 2 * BlkByteSize);
                if (RetVal < 0)
                    AmbaPrint_PrintStr5Fwprog("Erase the header of Linux Hibernation failed.\r\n", 0, 0, 0, 0, 0);
                else
                    AmbaPrint_PrintStr5Fwprog("Erase the header of Linux Hibernation \r\n", 0, 0, 0, 0, 0);
            }
        }

#ifndef CONFIG_LINUX
        if (RetVal == 0) {
            /* Reserved the last block for DTB. */
            if (PartID == AMBA_USER_PARTITION_LINUX_KERNEL) {
                RetVal = AmbaFwProg_ProgramDTB(pHeader, StartBlk);
            }
        }
#endif
    }

    return RetVal;
}

void AmbaFlashFwProg_ReturnToUsb(void)
{
    //extern UINT32 *__memfwprog_command;
    //volatile UINT32 *pCmd  = (volatile UINT32 *) &__memfwprog_command;
    //UINT32 Cmd[8];

    //(void)AmbaWrap_memcpy(Cmd, &__memfwprog_command, sizeof(Cmd));

    if (pAmbaRCT_Reg->SysConfig.UsbBoot != 0U) {
        //if (AmbaRTSL_UsbGetDevConfig() != 0x28U) {
            //if (Cmd[7] == 0x7E57U) {
            UINT32 Addr = (UINT32) 0x0;

            AmbaMisra_TouchUnused(&Addr);

            AmbaRTSL_CacheFlushDataAll();
            AmbaRTSL_GicIntGlobalDisable();
            __asm__ volatile ("br  %0": : "r" ((Addr)));
            //} else {
            //UINT32 Status = 0;
            //do {
            //(void)AmbaRTSL_UartGetStatus(AMBA_UART_APB_CHANNEL0, &Status);
            //} while ((Status & 0x3U) == 0);
            //(void)AmbaSYS_Reboot();
            //}
        //}
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
                (void)AmbaWrap_memset(pAmbaRTSL_NorSpiSysPartTable,  0xff, sizeof(AMBA_SYS_PARTITION_TABLE_s));
                (void)AmbaWrap_memset(pAmbaRTSL_NorSpiUserPartTable, 0xff, sizeof(AMBA_USER_PARTITION_TABLE_s));
                AmbaRTSL_NorEraseBlock(0);
                AmbaRTSL_NorWaitInt();
                AmbaRTSL_NorSpiInitPtbBbt();
            }
        }
    }
}

char * AmbaFlashFwProg_GetSysPartitionName(UINT32 PartID)
{
    return (char *)pAmbaRTSL_NorSpiSysPartTable->Entry[PartID].PartitionName;
}

char * AmbaFlashFwProg_GetUserPartitionName(UINT32 PartID)
{
    return (char *)pAmbaRTSL_NorSpiUserPartTable->Entry[PartID].PartitionName;
}

