/**
 *  @file AmbaFlashFwProg_eMMC.c
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
 *  @details eMMC firmware program utilities APIs
 */

#include "AmbaTypes.h"
#include "AmbaDef.h"
#include "AmbaMisraFix.h"
#include "AmbaWrap.h"
#include "AmbaUtility.h"

#include "AmbaNVM_Partition.h"
#include "AmbaFlashFwProg.h"

#include "AmbaSD.h"
#include "AmbaSD_STD.h"

#define  AMBA_KAL_EVENT_FLAG_t UINT32
#define  AMBA_KAL_MUTEX_t UINT32

#include "AmbaSD_Ctrl.h"

#include "AmbaRTSL_SD.h"
#include "AmbaRTSL_UART.h"
#include "AmbaRTSL_Cache.h"
#include "AmbaRTSL_USB.h"
#include "AmbaCSL_RCT.h"
//#include "AmbaSysCtrl.h"
#include "AmbaRTSL_GIC.h"

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

#define FDT_MAGIC   0xd00dfeed  /* 4: version, 4: total size */

/* conversion between little-endian and big-endian */
#define uswap_32(x) \
    ((((x) & 0xff000000) >> 24) | \
     (((x) & 0x00ff0000) >> 8)  | \
     (((x) & 0x0000ff00) << 8)  | \
     (((x) & 0x000000ff) << 24))

#define fdt32_to_cpu(x)       uswap_32(x)

#define fdt_get_header(fdt, field) \
    (fdt32_to_cpu(((const struct fdt_header *)(fdt))->field))
#define fdt_magic(fdt)          (fdt_get_header(fdt, magic))
#define fdt_version(fdt)        (fdt_get_header(fdt, version))

static UINT8 FlashProgChkBuf[AMBA_SD_WORK_BUFFER_SIZE] GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE;
#define EMMC_PHY_SHMOO 0
#define EMMC_READWRITE_TEST_SIZE 4096

extern UINT32 FlashProgEmmcAccessPart;
extern UINT32 FlashProgEmmcBootPart;
extern UINT32 FlashProgEmmcBootBus;
#define LINE_BUFFER_LENGTH   (4096U)
#if !defined(CONFIG_MUTI_BOOT_DEVICE)
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
#endif
/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaFlashFwProg_EmmcProgram
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
INT32 AmbaFlashFwProg_EmmcProgram(UINT8 *pRaw, UINT32 RawSize, UINT32 StartBlk, UINT32 NumBlks)
{
    AMBA_NVM_SECTOR_CONFIG_s   SecConfig;
    INT32 RetVal = 0, FirmOK = 0, CmpRel = 0;
    UINT32 Block, Percentage, /* PreOffset, */ Offset = 0;
    UINT32 SectorsPerWrite = sizeof(FlashProgChkBuf) / 512;
    UINT8 *pMainBuf;
    UINT32 ProgBlks = 0U, RemainBlks;

    RemainBlks = NumBlks;

    for (Block = StartBlk; Block < (StartBlk + NumBlks); Block += SectorsPerWrite) {

        /* erase the unused block after program ok */
        if (FirmOK == 1) {
            continue;
        }

        //PreOffset = Offset;

        pMainBuf = (UINT8 *)(pRaw + Offset);

        /* Program a page */
        ProgBlks += SectorsPerWrite;

        if (RemainBlks >= SectorsPerWrite) {
            ProgBlks   = SectorsPerWrite;
        } else {
            ProgBlks   = RemainBlks;
        }
        SecConfig.NumSector   = ProgBlks;
        SecConfig.pDataBuf    = (UINT8 *) pMainBuf;
        SecConfig.StartSector = Block;

        RetVal = AmbaRTSL_SdWriteSector(AMBA_SD_CHANNEL0, &SecConfig);
        if (RetVal < 0) {
            AmbaPrint_PrintUInt5Fwprog("program failed. <sector %d, sectors %d>", SecConfig.StartSector, SecConfig.NumSector, 0, 0, 0);
            break;
        }

        RemainBlks = (RemainBlks - ProgBlks);

        /* Read it back for verification */
        SecConfig.NumSector   = ProgBlks;
        SecConfig.pDataBuf    = (UINT8 *) FlashProgChkBuf;
        SecConfig.StartSector = Block;

        RetVal = AmbaRTSL_SdReadSector(AMBA_SD_CHANNEL0, &SecConfig);
        if (RetVal < 0) {
            AmbaPrint_PrintUInt5Fwprog("read failed. <sector %d, sectors %d>", SecConfig.StartSector, SecConfig.NumSector, 0, 0, 0);
            break;
        }

        /* Compare memory content after read back */
        RetVal = AmbaWrap_memcmp(pMainBuf, FlashProgChkBuf, ProgBlks * 512U, &CmpRel);
        if (CmpRel != 0) {
            AmbaPrint_PrintUInt5Fwprog("check failed. <sector %d, sectors %d>", SecConfig.StartSector, SecConfig.NumSector, 0, 0, 0);
            RetVal = -1;
            break;
        }

        if (!FirmOK) {
            Offset += sizeof(FlashProgChkBuf);
            if (Offset >= RawSize) {
                FirmOK = 1;
            }
        }

        if (Offset >= RawSize) {
            Percentage = 100;
        } else {
            Percentage = Offset / (RawSize / 100);
        }

        AmbaPrint_PrintUInt5Fwprog("\r\x1b[A\x1b[32C\x1b[0K%3u%%\r\n", Percentage, 0, 0, 0, 0);
    }

    if (RetVal < 0 || FirmOK == 0) {
        RetVal = FLPROG_ERR_PROG_IMG;
    }

    return RetVal;
}
#if !defined(CONFIG_MUTI_BOOT_DEVICE)
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
    int RetVal = 0;
    AMBA_IMG_HEADER *pHeader;
    UINT8 *pRaw;
    UINT32 StartBlk = 0, NumBlks = 0;

    AMBA_PARTITION_ENTRY_s *pPartEntry = (AMBA_PARTITION_ENTRY_s  *) & (pAmbaRTSL_EmmcSysPartTable ->Entry[PartID]);

    if (PartID >= AMBA_NUM_SYS_PARTITION)
        return -1;

    pHeader = (AMBA_IMG_HEADER *) pImage;
    pRaw = pImage + sizeof(AMBA_IMG_HEADER);

    AmbaPrint_PrintStr5Fwprog("\rProgram image to EMMC ...", 0, 0, 0, 0, 0);

    StartBlk = pPartEntry->StartBlkAddr;
    NumBlks  = pPartEntry->BlkCount;

    if (pHeader->ImgLen > AmbaRTSL_SdCtrl[0].pSysPartConfig[PartID].ByteCount) {
        AmbaPrint_PrintStr5Fwprog("\r%s length is bigger than partition size", (char *)pPartEntry->PartitionName, 0, 0, 0, 0);
        return FLPROG_ERR_LENGTH;
    }

    if (PartID == AMBA_SYS_PARTITION_BOOTSTRAP) {
        AMBA_SD_EMMC_CMD6_ARGUMENT_s ArgCmd6 = {0};

        if (pHeader->ImgLen > AMBA_EMMC_BOOTSTRAP_CODE_SPACE_SIZE) {
            AmbaPrint_PrintStr5Fwprog("\rBST length is bigger than 4KB", 0, 0, 0, 0, 0);
            return FLPROG_ERR_LENGTH;    /* BST must fit in 4KB for FIO FIFO */
        }

        ArgCmd6.Access = ACCESS_WRITE_BYTE;
        ArgCmd6.Index  = BOOT_CONFIG;

        if (FlashProgEmmcBootPart == EMMC_BOOTP_USER)
            ArgCmd6.Value = (EMMC_ACCP_USER);
        else if (FlashProgEmmcBootPart == EMMC_BOOTP_BP_1)
            ArgCmd6.Value = (EMMC_ACCP_BP_1);
        else
            ArgCmd6.Value = (EMMC_ACCP_BP_2);

        ArgCmd6.Value |= FlashProgEmmcBootPart;

        AmbaRTSL_SdSendEmmcCMD6(AMBA_SD_CHANNEL0, &ArgCmd6);
        RetVal = AmbaRTSL_SdWaitCmdDone(AMBA_SD_CHANNEL0, AMBA_SD_IRQ_CMD_DONE | AMBA_SD_IRQ_TRANSFER_DONE);
        if (RetVal != OK) {
            AmbaPrint_PrintStr5Fwprog("AmbaSD_SendEmmcCMD6() fail!", 0, 0, 0, 0, 0);
            return FLPROG_ERR_IOPROCESS;
        }

        ArgCmd6.Access = ACCESS_WRITE_BYTE;
        ArgCmd6.Index  = BOOT_BUS_WIDTH;
        ArgCmd6.Value  = FlashProgEmmcBootBus;

        AmbaRTSL_SdSendEmmcCMD6(AMBA_SD_CHANNEL0, &ArgCmd6);
        RetVal = AmbaRTSL_SdWaitCmdDone(AMBA_SD_CHANNEL0, AMBA_SD_IRQ_CMD_DONE | AMBA_SD_IRQ_TRANSFER_DONE);
        if (RetVal != OK) {
            AmbaPrint_PrintStr5Fwprog("AmbaSD_SendEmmcCMD6() fail!", 0, 0, 0, 0, 0);
            return FLPROG_ERR_IOPROCESS;
        }

#ifdef CONFIG_ENABLE_EMMC_HW_RESET
        ArgCmd6.Access = ACCESS_WRITE_BYTE;
        ArgCmd6.Index  = EMMC_HW_RESET;
        ArgCmd6.Value  = 1;

        AmbaRTSL_SdSendEmmcCMD6(AMBA_SD_CHANNEL0, &ArgCmd6);
        RetVal = AmbaRTSL_SdWaitCmdDone(AMBA_SD_CHANNEL0, AMBA_SD_IRQ_CMD_DONE | AMBA_SD_IRQ_TRANSFER_DONE);
        if (RetVal != OK) {
            AmbaPrint_PrintStr5Fwprog("AmbaSD_SendEmmcCMD6() fail!", 0, 0, 0, 0, 0);
            return FLPROG_ERR_IOPROCESS;
        }
#endif
        RetVal = AmbaFlashFwProg_EmmcProgram(pRaw, pHeader->ImgLen, StartBlk, NumBlks);

        if (FlashProgEmmcBootPart != EMMC_BOOTP_USER) {
            ArgCmd6.Access = ACCESS_WRITE_BYTE;
            ArgCmd6.Index  = BOOT_CONFIG;
            ArgCmd6.Value  = (FlashProgEmmcBootPart | EMMC_ACCP_USER);

            AmbaRTSL_SdSendEmmcCMD6(AMBA_SD_CHANNEL0, &ArgCmd6);
            RetVal = AmbaRTSL_SdWaitCmdDone(AMBA_SD_CHANNEL0, AMBA_SD_IRQ_CMD_DONE | AMBA_SD_IRQ_TRANSFER_DONE);
            if (RetVal != OK) {
                AmbaPrint_PrintStr5Fwprog("AmbaSD_SendEmmcCMD6() fail!", 0, 0, 0, 0, 0);
                return FLPROG_ERR_IOPROCESS;
            }
        }
    } else {
        RetVal = AmbaFlashFwProg_EmmcProgram(pRaw, pHeader->ImgLen, StartBlk, NumBlks);
    }

    /* Update the PTB's entry */
    if (RetVal >= 0) {
        pPartEntry->ActualByteSize = pHeader->ImgLen;
        pPartEntry->Attribute = AmbaRTSL_SdCtrl[0].pSysPartConfig[PartID].Attribute;
        pPartEntry->RamLoadAddr    = pHeader->MemAddr;
        pPartEntry->ImageCRC32     = pHeader->Crc32;

        AmbaRTSL_EmmcWriteSysPTB(pAmbaRTSL_EmmcSysPartTable );
    }

#if defined(CONFIG_SOC_CV2) || defined(CONFIG_SOC_CV22) || defined(CONFIG_SOC_CV25) || defined(CONFIG_SOC_CV28) || \
    defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS) || defined(CONFIG_SOC_H32)
    /* Remove training data when BST or BLD updated */
    if ((PartID == AMBA_SYS_PARTITION_BOOTSTRAP) || (PartID == AMBA_SYS_PARTITION_BOOTLOADER)) {
        if (FlashProgEmmcBootPart != EMMC_BOOTP_USER) {
            AMBA_NVM_SECTOR_CONFIG_s SecConfig;
            UINT8 pDataBuf[512] = {0};

            pPartEntry = (AMBA_PARTITION_ENTRY_s  *) & (pAmbaRTSL_EmmcSysPartTable->Entry[AMBA_SYS_PARTITION_BOOTSTRAP]);
            if (pPartEntry->BlkCount != 0U) {
                SecConfig.NumSector   = 1;
                SecConfig.pDataBuf    = pDataBuf;
                SecConfig.StartSector = pPartEntry->BlkCount - 1;

                AmbaRTSL_SdWriteSector(AMBA_SD_CHANNEL0, &SecConfig);
            }
        }
    }
#endif

    return RetVal;
}

INT32 AmbaFwProg_ProgramUserPartition(UINT32 PartID, UINT8 *pImage)
{
    AMBA_IMG_HEADER *pHeader;
    AMBA_PARTITION_ENTRY_s *pPartEntry;
    UINT8 *pRaw;
    INT32 RetVal = FLPROG_OK, StartBlk;
    UINT64 BlkAddr;
    UINT32 BlkByteSize, j;
    AMBA_SD_CARD_STATUS_s CardStatus;
    const AMBA_SD_CARD_INFO_s *pCardInfo = &AmbaSD_Ctrl[0].CardInfo;

    if ((PartID >= AMBA_NUM_USER_PARTITION) ||
        (PartID <= AMBA_USER_PARTITION_PTB))
        return -1;

    //AmbaSD_GetCardStatus(AMBA_SD_CHANNEL0, &CardStatus);
    CardStatus.CardSize   = (UINT64) pCardInfo->MemCardInfo.TotalSectors * (UINT64)512;

    pHeader = (AMBA_IMG_HEADER *) pImage;
    pRaw = pImage + sizeof(AMBA_IMG_HEADER);

    if (PartID > AMBA_USER_PARTITION_SYS_SOFTWARE) {
#if 0
        UINT32  i;

        for (i = PartID; AMBA_USER_PARTITION_SYS_SOFTWARE < i; i--) {
            pPartEntry = &(pAmbaRTSL_EmmcUserPartTable ->Entry[i - 1]);
            if (0U < pPartEntry->BlkCount) {
                break;
            }
        }

        if (i == AMBA_USER_PARTITION_SYS_SOFTWARE) {
            RetVal = FLPROG_ERR_NOT_READY;
        }
#endif
    } else {
        pPartEntry = &(pAmbaRTSL_EmmcSysPartTable ->Entry[AMBA_NUM_SYS_PARTITION - 1]);
    }

    if (RetVal == FLPROG_OK) {
        if (PartID > AMBA_USER_PARTITION_SYS_SOFTWARE) {
            pPartEntry = &(pAmbaRTSL_EmmcUserPartTable ->Entry[PartID]);
            StartBlk = pPartEntry->StartBlkAddr;
        } else {
            StartBlk = pPartEntry->StartBlkAddr + pPartEntry->BlkCount;
        }
        pPartEntry = &(pAmbaRTSL_EmmcUserPartTable ->Entry[PartID]);
        AmbaPrint_PrintStr5Fwprog("\rProgram image to EMMC ......\r\n", 0, 0, 0, 0, 0);

        if (pHeader->ImgLen > AmbaRTSL_SdCtrl[0].pUserPartConfig[PartID].ByteCount) {
            AmbaPrint_PrintStr5Fwprog("%s length is bigger than partition size", (char *)pPartEntry->PartitionName, 0, 0, 0, 0);
            return FLPROG_ERR_LENGTH;
        }

        BlkByteSize = pAmbaRTSL_EmmcSysPartTable ->BlkByteSize;

        pPartEntry->StartBlkAddr   = StartBlk;   /* start Block Address */
        pPartEntry->ByteCount      = AmbaRTSL_SdCtrl[0].pUserPartConfig[PartID].ByteCount;      /* number of Bytes for the Partition */
        pPartEntry->ActualByteSize = pHeader->ImgLen;                                /* actual size in Bytes */
        pPartEntry->BlkCount       = GetRoundDownValU32(pPartEntry->ByteCount, BlkByteSize); /* number of Blocks for the Partition  */

        BlkAddr = pPartEntry->StartBlkAddr + 1;

        /* allocate good blocks for the partition */
        for (j = (pPartEntry->BlkCount - 1); j > 0; BlkAddr++) {
            if (BlkAddr >= (CardStatus.CardSize / 512)) {
                RetVal = FLPROG_ERR_NOMOREBLOCK;    /* no more good block ? */
                break;
            }
        }
        if (j > pPartEntry->BlkCount)
            pPartEntry->BlkCount = j;

        RetVal = AmbaFlashFwProg_EmmcProgram(pRaw, pHeader->ImgLen, StartBlk, pPartEntry->BlkCount);

        /* Update the PTB's entry */
        if (RetVal >= 0) {
            pPartEntry->ActualByteSize = pHeader->ImgLen;
            pPartEntry->Attribute = AmbaRTSL_SdCtrl[0].pUserPartConfig[PartID].Attribute;
            pPartEntry->RamLoadAddr    = pHeader->MemAddr;
            pPartEntry->ImageCRC32     = pHeader->Crc32;

            AmbaRTSL_EmmcWriteUserPTB(pAmbaRTSL_EmmcUserPartTable, pAmbaRTSL_EmmcUserPartTable ->PTBNumber);
        }

        if (PartID == AMBA_USER_PARTITION_SYS_SOFTWARE ||
            PartID == AMBA_USER_PARTITION_LINUX_KERNEL) {
            AmbaRTSL_EmmcErasePartition(1, AMBA_USER_PARTITION_LINUX_HIBERNATION_IMG);
        }
#ifndef CONFIG_LINUX
        /* Reserved the last block for DTB. */
        if (PartID == AMBA_USER_PARTITION_LINUX_KERNEL) {
            extern UINT32 begin_dtb_image;
            UINT32 Size;
            UINT32 i = AmbaRTSL_SdCtrl[0].pUserPartConfig[PartID].ByteCount / pPartEntry->BlkCount;
            Size = pHeader->ImgLen / i;
            Size += (pHeader->ImgLen % i) ? 1 : 0;
            Size += 128;

            if (Size > pPartEntry->BlkCount) {
                AmbaPrint_PrintStr5Fwprog("Please reserve 1 block for Linux DTB! \r\n\r\n", 0, 0, 0, 0, 0);
                return FLPROG_ERR_LENGTH;
            }

            if (PartID == AMBA_USER_PARTITION_LINUX_KERNEL) {
                AmbaMisra_TypeCast32(&pHeader, &begin_dtb_image);  //pHeader = (AMBA_IMG_HEADER *)begin_dtb_image;
                AmbaMisra_TypeCast32(&pRaw, &begin_dtb_image);     //pRaw = (UINT8 *) begin_dtb_image;
            }

            pRaw = pRaw + sizeof(AMBA_IMG_HEADER);

            if ((fdt_magic(pRaw) == FDT_MAGIC) && (fdt_version(pRaw) >= 17)) {
                AmbaPrint_PrintStr5Fwprog("\r\nDTB found in firmware!\r\n", 0, 0, 0, 0, 0);
                AmbaPrint_PrintStr5Fwprog("\rProgram DTB to the last block .\r\n", 0, 0, 0, 0, 0);

                if (PartID == AMBA_USER_PARTITION_LINUX_KERNEL) {
                    AmbaMisra_TypeCast32(&pHeader, &begin_dtb_image);  //pHeader = (AMBA_IMG_HEADER *)begin_dtb_image;
                    AmbaMisra_TypeCast32(&pRaw, &begin_dtb_image);     //pRaw = (UINT8 *) begin_dtb_image;
                }

                pRaw = pRaw + sizeof(AMBA_IMG_HEADER);

                RetVal = AmbaFlashFwProg_EmmcProgram(pRaw, pHeader->ImgLen,
                                                     (StartBlk + pPartEntry->BlkCount - 128),
                                                     (pHeader->ImgLen / i) + 1);
                /* NumBlks -= 1; */
            } else {
                AmbaPrint_PrintStr5Fwprog("\r\nDTB is not found in firmware!"
                                          "\r\nLinux can't be booted!\r\n\r\n", 0, 0, 0, 0, 0);
            }
            pHeader = (AMBA_IMG_HEADER *) pImage;
            pRaw = pImage + sizeof(AMBA_IMG_HEADER);
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
                static UINT8 MainBuf[64 * 1024] = {0};
                AMBA_NVM_SECTOR_CONFIG_s   SecConfig;

                (void)AmbaWrap_memset(pAmbaRTSL_EmmcSysPartTable,  0xff, sizeof(AMBA_SYS_PARTITION_TABLE_s));
                (void)AmbaWrap_memset(pAmbaRTSL_EmmcUserPartTable, 0xff, sizeof(AMBA_USER_PARTITION_TABLE_s));

                SecConfig.NumSector   = sizeof(MainBuf) / 512U;
                SecConfig.pDataBuf    = MainBuf;
                SecConfig.StartSector = 0;
                (void) AmbaRTSL_SdWriteSector(AMBA_SD_CHANNEL0, &SecConfig);

                (void) AmbaRTSL_EmmcInitPtbBbt();
            }
        }
    }
}

char * AmbaFlashFwProg_GetSysPartitionName(UINT32 PartID)
{
    return (char *)pAmbaRTSL_EmmcSysPartTable ->Entry[PartID].PartitionName;
}

char * AmbaFlashFwProg_GetUserPartitionName(UINT32 PartID)
{
    return (char *)pAmbaRTSL_EmmcUserPartTable ->Entry[PartID].PartitionName;
}
#endif

#if EMMC_PHY_SHMOO
void AmbaFlashFwProg_EmmcShmoo(UINT32 ClkBypass, UINT32 RxClkPol)
{
    UINT32 SbcCore, SelValue, SecCount = AmbaRTSL_SdGetWorkBufferSize() / 512, Block;
    AMBA_SD_DETAIL_DELAY_u DetailDelay = {0};
    int ShmooRegResult[4][64] = {0}, ReVal = 0;
    AMBA_NVM_SECTOR_CONFIG_s   SecConfig;
    UINT8 *pWorkBufMain = AmbaRTSL_SdWorkBuf;

    AmbaRTSL_SdDelayCtrlEnable(0);

    for(SbcCore = 0; SbcCore < 4; SbcCore++)
        for(SelValue = 0; SelValue < 64; SelValue++) {
            AmbaRTSL_SetPhyDelay(AMBA_SD_CHANNEL0, ClkBypass, RxClkPol, SbcCore, SelValue, 0, 0);

            AmbaRTSL_SdSetHostBusWidth(AMBA_SD_CHANNEL0, 1);

            if (AmbaRTSL_SdCardInit(AMBA_SD_CHANNEL0) != OK) {
                ShmooRegResult[SbcCore][SelValue] = -1;
                AmbaPrint_PrintUInt5Fwprog("eMMC init fail: SbcCore = %u, SelValue = %u\r\n", SbcCore, SelValue, 0, 0, 0);
                continue;
            } else {
                DetailDelay.Bits.RdLatency = 1;
                DetailDelay.Bits.RXClkPol = RxClkPol;
                DetailDelay.Bits.ClkOutBypass = ClkBypass;
                DetailDelay.Bits.SelValue = SelValue;
                DetailDelay.Bits.SbcCoreDelay = SbcCore;
                ShmooRegResult[SbcCore][SelValue] = 1;
            }

            for (Block = 0; Block < EMMC_READWRITE_TEST_SIZE; Block += SecCount) {

                SecConfig.NumSector   = SecCount;
                SecConfig.pDataBuf    = (UINT8 *) pWorkBufMain;
                SecConfig.StartSector = Block;

                ReVal = AmbaRTSL_SdWriteSector(AMBA_SD_CHANNEL0, &SecConfig);

                if(ReVal != OK) {
                    ShmooRegResult[SbcCore][SelValue] = -1;
                    AmbaPrint_PrintStr5Fwprog("eMMC shmoo write sector fail!\r\n", 0, 0, 0, 0, 0);
                    break;
                }
            }

            for (Block = 0; Block < EMMC_READWRITE_TEST_SIZE; Block += SecCount) {
                SecConfig.NumSector   = SecCount;
                SecConfig.pDataBuf    = (UINT8 *) pWorkBufMain;
                SecConfig.StartSector = Block;

                ReVal = AmbaRTSL_SdReadSector(AMBA_SD_CHANNEL0, &SecConfig);

                if(ReVal != OK) {
                    ShmooRegResult[SbcCore][SelValue] = -1;
                    AmbaPrint_PrintStr5Fwprog("eMMC shmoo write sector fail!\r\n", 0, 0, 0, 0, 0);
                    break;
                }
            }
            if(ShmooRegResult[SbcCore][SelValue] == 1)
                AmbaPrint_PrintUInt5Fwprog("eMMC init OK: SbcCore = %u, SelValue = %u DetailDelay = 0x%x\r\n", SbcCore, SelValue, DetailDelay.Data, 0, 0);
            else
                AmbaPrint_PrintUInt5Fwprog("eMMC shmoo test fail: SbcCore = %u, SelValue = %u\r\n", SbcCore, SelValue, 0, 0, 0);
        }


    AmbaPrint_PrintUInt5Fwprog("---------[Pass Value] ClkBypass :%d RxClkPol:%d ----------\r\n", ClkBypass, RxClkPol, 0, 0, 0);
    for(SbcCore = 0; SbcCore < 4; SbcCore++) {
        AmbaPrint_PrintUInt5Fwprog("[sc %d]\r\n", SbcCore, 0, 0, 0, 0);
        for(SelValue = 63; SelValue > 31; SelValue--) {
            if(ShmooRegResult[SbcCore][SelValue] == 1)
                AmbaPrint_PrintUInt5Fwprog("%2d(%3d)  ", SelValue, 32 - SelValue, 0, 0, 0);
            else
                AmbaPrint_PrintStr5Fwprog("   X     ", 0, 0, 0, 0, 0);

            if((SelValue % 16) == 0)
                AmbaPrint_PrintStr5Fwprog("\r\n", 0, 0, 0, 0, 0);
        }
        for(SelValue = 0; SelValue < 32; SelValue++) {
            if(ShmooRegResult[SbcCore][SelValue] == 1)
                AmbaPrint_PrintUInt5Fwprog("%2d(%3d)  ", SelValue, SelValue, 0, 0, 0);
            else
                AmbaPrint_PrintStr5Fwprog("   X     ", 0, 0, 0, 0, 0);
            if((SelValue % 16) == 15)
                AmbaPrint_PrintStr5Fwprog("\r\n", 0, 0, 0, 0, 0);
        }
        AmbaPrint_PrintStr5Fwprog("\r\n", 0, 0, 0, 0, 0);
    }
    AmbaRTSL_SdDelayCtrlEnable(1);
    AmbaRTSL_SdSetHostBusWidth(AMBA_SD_CHANNEL0, 1);
}
#endif
