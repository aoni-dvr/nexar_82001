/**
 *  @file AmbaBLD_Shell.c
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
 *  @details functions used in Amboot
 *
 */

#include "AmbaTypes.h"
#include "AmbaMisraFix.h"
#include "AmbaIOUtility.h"

#include "AmbaBLD.h"

#include "AmbaRTSL_Cache.h"
#include "AmbaRTSL_CPU.h"
#include "AmbaRTSL_MMU.h"
#include "AmbaRTSL_UART.h"

#include "AmbaRTSL_NAND.h"
#ifdef CONFIG_ENABLE_EMMC_BOOT
#include "AmbaRTSL_PLL.h"
#include "AmbaRTSL_SD.h"
#endif
#ifdef CONFIG_ENABLE_SPINOR_BOOT
#include "AmbaRTSL_SpiNOR.h"
#endif

#include "AmbaCSL_RCT.h"

static char SysPartName[AMBA_NUM_SYS_PARTITION][10U];
static UINT8 SysPartNameLen[AMBA_NUM_SYS_PARTITION];
static char UserPartName[AMBA_NUM_USER_PARTITION][10U];
static UINT8 UserPartNameLen[AMBA_NUM_USER_PARTITION];

#define PRINT_BUF_SIZE  (1024U)
static char BldCmdLine[128] GNU_SECTION_NOZEROINIT;

static void BldShell_PartInfoInit(void)
{
    SysPartNameLen[AMBA_SYS_PARTITION_BOOTSTRAP]    = 3U;
    SysPartNameLen[AMBA_SYS_PARTITION_BOOTLOADER]   = 3U;
    SysPartNameLen[AMBA_SYS_PARTITION_FW_UPDATER]   = 2U;
    SysPartNameLen[AMBA_SYS_PARTITION_ARM_TRUST_FW] = 3U;
    SysPartNameLen[AMBA_SYS_PARTITION_BLD2] = 4U;

    UserPartNameLen[AMBA_USER_PARTITION_PTB]           = 3U;
    UserPartNameLen[AMBA_USER_PARTITION_SYS_SOFTWARE]  = 3U;
    UserPartNameLen[AMBA_USER_PARTITION_DSP_uCODE]     = 3U;
    UserPartNameLen[AMBA_USER_PARTITION_SYS_DATA]      = 3U;
    UserPartNameLen[AMBA_USER_PARTITION_LINUX_KERNEL]  = 5U;
    UserPartNameLen[AMBA_USER_PARTITION_LINUX_ROOT_FS] = 3U;
    UserPartNameLen[AMBA_USER_PARTITION_LINUX_HIBERNATION_IMG] = 5U;
    UserPartNameLen[AMBA_USER_PARTITION_VIDEO_REC_INDEX]   = 3U;
    UserPartNameLen[AMBA_USER_PARTITION_CALIBRATION_DATA]  = 5U;
    UserPartNameLen[AMBA_USER_PARTITION_USER_SETTING]  = 4U;
    UserPartNameLen[AMBA_USER_PARTITION_FAT_DRIVE_A]   = 4U;
    UserPartNameLen[AMBA_USER_PARTITION_FAT_DRIVE_B]   = 4U;
    UserPartNameLen[AMBA_USER_PARTITION_RESERVED0] = 4U;
    UserPartNameLen[AMBA_USER_PARTITION_RESERVED1] = 4U;
    UserPartNameLen[AMBA_USER_PARTITION_RESERVED2] = 4U;
    UserPartNameLen[AMBA_USER_PARTITION_RESERVED3] = 4U;
    UserPartNameLen[AMBA_USER_PARTITION_RESERVED4] = 4U;

    IO_UtilityStringCopy(SysPartName[AMBA_SYS_PARTITION_BOOTSTRAP], 10U, "bst");
    IO_UtilityStringCopy(SysPartName[AMBA_SYS_PARTITION_BOOTLOADER], 10U, "bld");
    IO_UtilityStringCopy(SysPartName[AMBA_SYS_PARTITION_FW_UPDATER], 10U, "fw");
    IO_UtilityStringCopy(SysPartName[AMBA_SYS_PARTITION_ARM_TRUST_FW], 10U, "atf");
    IO_UtilityStringCopy(SysPartName[AMBA_SYS_PARTITION_BLD2], 10U, "bld2");

    IO_UtilityStringCopy(UserPartName[AMBA_USER_PARTITION_PTB], 10U, "ptb");
    IO_UtilityStringCopy(UserPartName[AMBA_USER_PARTITION_SYS_SOFTWARE], 10U, "sys");
    IO_UtilityStringCopy(UserPartName[AMBA_USER_PARTITION_DSP_uCODE], 10U, "dsp");
    IO_UtilityStringCopy(UserPartName[AMBA_USER_PARTITION_SYS_DATA], 10U, "rom");
    IO_UtilityStringCopy(UserPartName[AMBA_USER_PARTITION_LINUX_KERNEL], 10U, "linux");
    IO_UtilityStringCopy(UserPartName[AMBA_USER_PARTITION_LINUX_ROOT_FS], 10U, "rfs");
    IO_UtilityStringCopy(UserPartName[AMBA_USER_PARTITION_LINUX_HIBERNATION_IMG], 10U, "hiber");
    IO_UtilityStringCopy(UserPartName[AMBA_USER_PARTITION_VIDEO_REC_INDEX], 10U, "vri");
    IO_UtilityStringCopy(UserPartName[AMBA_USER_PARTITION_CALIBRATION_DATA], 10U, "calib");
    IO_UtilityStringCopy(UserPartName[AMBA_USER_PARTITION_USER_SETTING], 10U, "pref");
    IO_UtilityStringCopy(UserPartName[AMBA_USER_PARTITION_FAT_DRIVE_A], 10U, "stg1");
    IO_UtilityStringCopy(UserPartName[AMBA_USER_PARTITION_FAT_DRIVE_B], 10U, "stg2");
    IO_UtilityStringCopy(UserPartName[AMBA_USER_PARTITION_RESERVED0], 10U, "rsv0");
    IO_UtilityStringCopy(UserPartName[AMBA_USER_PARTITION_RESERVED1], 10U, "rsv1");
    IO_UtilityStringCopy(UserPartName[AMBA_USER_PARTITION_RESERVED2], 10U, "rsv2");
    IO_UtilityStringCopy(UserPartName[AMBA_USER_PARTITION_RESERVED3], 10U, "rsv3");
    IO_UtilityStringCopy(UserPartName[AMBA_USER_PARTITION_RESERVED4], 10U, "rsv4");
}

static void BldShell_PrintStr(const char *pFmt)
{
    static char ShellPrintBuf[PRINT_BUF_SIZE] GNU_SECTION_NOZEROINIT;
    UINT32 ActualTxSize, UartTxSize, Index = 0U;
    const UINT8 *pPrintBuf;
    const char *pString, *pArg = NULL;

    pString = &ShellPrintBuf[0];
    AmbaMisra_TypeCast64(&pPrintBuf, &pString);
    UartTxSize = IO_UtilityStringPrintStr(&ShellPrintBuf[0], PRINT_BUF_SIZE, pFmt, 1U, &pArg);
    while (UartTxSize > 0U) {
        if (AmbaRTSL_UartWrite(AMBA_UART_APB_CHANNEL0, UartTxSize, &pPrintBuf[Index], &ActualTxSize) != UART_ERR_NONE) {
            break;
        } else {
            UartTxSize -= ActualTxSize;
            Index += ActualTxSize;
        }
    }
}

static UINT32 BLD_GetCmdLine(char *pCmdLine, UINT32 CmdLineSize, UINT32 *pCommondSize)
{
    UINT32 UartRxSize = 0U;
    UINT32 i = 0U;
    UINT32 RetVal = BLD_ERR_NONE;
    UINT8 Char;

    if (CmdLineSize == 0x0U) {
        RetVal = BLD_ERR_ARG;
    } else {
        while (i < (CmdLineSize - 1U)) {
            (void)AmbaRTSL_UartRead(AMBA_UART_APB_CHANNEL0, 0x1, &Char, &UartRxSize);
            if (UartRxSize != 0x1U) {
                continue;
            }

            if ((Char == (UINT8)'\r') || (Char == (UINT8)'\n')) {
                pCmdLine[i] = '\0';
                break;
            } else if (Char == 0x08U) {
                if (i > 0U) {
                    i--;
                    BldShell_PrintStr("\b \b");
                }
            } else if ((Char == (UINT8)' ') ||
                       ((Char >= (UINT8)'0') && (Char <= (UINT8)'9')) ||
                       ((Char >= (UINT8)'A') && (Char <= (UINT8)'Z')) ||
                       ((Char >= (UINT8)'a') && (Char <= (UINT8)'z'))) {
                pCmdLine[i] = (char)Char;
                i++;
                (void)AmbaRTSL_UartWrite(AMBA_UART_APB_CHANNEL0, 1, &Char, &UartRxSize);
            } else {
                /* do nothing */
            }
        }
    }

    *pCommondSize = i;

    return RetVal;
}

static UINT32 BLD_DetectEscapeKey(UINT32 Time)
{
    UINT8 Char;
    UINT32 TimeLimit;
    UINT32 UartRxSize = 0U;
    UINT32 RetValue = BLD_ERR_TIMEOUT;

    if (Time == 0U) {
        RetValue = BLD_ERR_ARG;
    }

    AmbaCSL_RctTimer0Reset();   /* reset RCT Timer */
    AmbaCSL_RctTimer0Enable();

    TimeLimit = Time * (24000000U / 1000U);
    while (AmbaCSL_RctTimer0GetCounter() < TimeLimit) {
        Char = 0x0;
        (void)AmbaRTSL_UartRead(AMBA_UART_APB_CHANNEL0, 0x1U, &Char, &UartRxSize);
        if ((UartRxSize == 0x1U) && ((Char == 0x0aU) || (Char == 0x0dU) || (Char == 0x1bU))) {
            AmbaCSL_RctTimer0Reset();   /* reset RCT Timer */
            RetValue = BLD_ERR_NONE;
            break;
        }
    }

    AmbaCSL_RctTimer0Reset();   /* reset RCT Timer */

    return RetValue;
}

static void BLD_ShowPartTableInfo(void)
{
    const AMBA_SYS_PARTITION_TABLE_s *pSysPtb = AmbaBLD_GetSysPartTable();
    const AMBA_USER_PARTITION_TABLE_s *pUserPtb = AmbaBLD_GetUserPartTable();
    const AMBA_PARTITION_ENTRY_s *pPartEntry;
    UINT32 i, Crc32;
    char IntStr[16];
    const char *pString = NULL;
    const UINT8 *pWorkUINT8 = NULL;

    AmbaMisra_TypeCast32(&pWorkUINT8, &pSysPtb);
    Crc32 = IO_UtilityCrc32(pWorkUINT8, sizeof(AMBA_SYS_PARTITION_TABLE_s) - 4U);
    if (Crc32 != pSysPtb->CRC32) {
        BldShell_PrintStr("Partition info error (CRC check fail)\r\n");
    } else {
        BldShell_PrintStr("sys_ptb: blk_addr: 0, ver: ");
        (void)IO_UtilityUInt32ToStr(IntStr, 16U, pSysPtb->Version, 10U);
        BldShell_PrintStr(IntStr);
        BldShell_PrintStr(", blk_size: ");
        (void)IO_UtilityUInt32ToStr(IntStr, 16U, pSysPtb->BlkByteSize, 10U);
        BldShell_PrintStr(IntStr);
        BldShell_PrintStr("\r\n");

        pPartEntry = pSysPtb->Entry;
        for (i = 0U; i < AMBA_NUM_SYS_PARTITION; i++) {
            if (pPartEntry->BlkCount > 0U) {
                //AmbaRTSL_UartPrintf("%-16s: attribute: 0x%08x, mem_addr: 0x%08x, ", pPartEntry->PartitionName, pPartEntry->Attribute.Data, pPartEntry->RamLoadAddr);
                //AmbaRTSL_UartPrintf("blk_addr: from %4u to %4u\r\n", pPartEntry->StartBlkAddr, pPartEntry->StartBlkAddr + pPartEntry->BlkCount - 1);
                pWorkUINT8 = pPartEntry->PartitionName;
                AmbaMisra_TypeCast64(&pString, &pWorkUINT8);
                BldShell_PrintStr(pString);
                BldShell_PrintStr(": attribute: 0x");
                (void)IO_UtilityUInt32ToStr(IntStr, 16U, pPartEntry->Attribute, 16U);
                BldShell_PrintStr(IntStr);
                BldShell_PrintStr(", mem_addr: 0x");
                (void)IO_UtilityUInt32ToStr(IntStr, 16U, pPartEntry->RamLoadAddr, 16U);
                BldShell_PrintStr(IntStr);
                BldShell_PrintStr(", blk_addr: from ");
                (void)IO_UtilityUInt32ToStr(IntStr, 16U, pPartEntry->StartBlkAddr, 10U);
                BldShell_PrintStr(IntStr);
                BldShell_PrintStr(" to ");
                (void)IO_UtilityUInt32ToStr(IntStr, 16U, pPartEntry->StartBlkAddr + pPartEntry->BlkCount - 1U, 10U);
                BldShell_PrintStr(IntStr);
                BldShell_PrintStr("\r\n");
            }
            pPartEntry++;
        }

        AmbaMisra_TypeCast32(&pWorkUINT8, &pUserPtb);
        Crc32 = IO_UtilityCrc32(pWorkUINT8, AMBA_USER_PTB_CRC32_SIZE);
        if (Crc32 == pUserPtb->CRC32) {
            //AmbaRTSL_UartPrintf("user_ptb: blk_addr: %u, ver: %u, magic: 0x%08x\r\n", pSysPtb->BlkAddrUserPTB, pUserPtb->Version, pUserPtb->BldMagicCode);
            BldShell_PrintStr("\r\n\r\nuser_ptb: blk_addr: ");
            (void)IO_UtilityUInt32ToStr(IntStr, 16U, pSysPtb->BlkAddrUserPTB, 10U);
            BldShell_PrintStr(IntStr);
            BldShell_PrintStr(", ver: ");
            (void)IO_UtilityUInt32ToStr(IntStr, 16U, pUserPtb->Version, 10U);
            BldShell_PrintStr(IntStr);
            BldShell_PrintStr(", magic: 0x");
            (void)IO_UtilityUInt32ToStr(IntStr, 16U, pUserPtb->BldMagicCode, 16U);
            BldShell_PrintStr(IntStr);
            BldShell_PrintStr("\r\n");

            pPartEntry = pUserPtb->Entry;
            for (i = 0; i < AMBA_NUM_USER_PARTITION; i++) {
                if ((pPartEntry->BlkCount > 0U) && (pPartEntry->BlkCount != 0xFFFFFFFFU)) {
                    //AmbaRTSL_UartPrintf("%-16s: attribute: 0x%08x, mem_addr: 0x%08x, ", pPartEntry->PartitionName, pPartEntry->Attribute.Data, pPartEntry->RamLoadAddr);
                    //AmbaRTSL_UartPrintf("blk_addr: from %4u to %4u\r\n", pPartEntry->StartBlkAddr, pPartEntry->StartBlkAddr + pPartEntry->BlkCount - 1);
                    pWorkUINT8 = pPartEntry->PartitionName;
                    AmbaMisra_TypeCast64(&pString, &pWorkUINT8);
                    BldShell_PrintStr(pString);
                    BldShell_PrintStr(": attribute: 0x");
                    (void)IO_UtilityUInt32ToStr(IntStr, 16U, pPartEntry->Attribute, 16U);
                    BldShell_PrintStr(IntStr);
                    BldShell_PrintStr(", mem_addr: 0x");
                    (void)IO_UtilityUInt32ToStr(IntStr, 16U, pPartEntry->RamLoadAddr, 16U);
                    BldShell_PrintStr(IntStr);
                    BldShell_PrintStr(", blk_addr: from ");
                    (void)IO_UtilityUInt32ToStr(IntStr, 16U, pPartEntry->StartBlkAddr, 10U);
                    BldShell_PrintStr(IntStr);
                    BldShell_PrintStr(" to ");
                    (void)IO_UtilityUInt32ToStr(IntStr, 16U, pPartEntry->StartBlkAddr + pPartEntry->BlkCount - 1U, 10U);
                    BldShell_PrintStr(IntStr);
                    BldShell_PrintStr("\r\n");
                }
                pPartEntry++;
            }
        } else {
            BldShell_PrintStr("User System Partition error\r\n");
        }
    }
}

static void BLD_ShowPartTable(UINT32 PtbNo)
{
    const AMBA_USER_PARTITION_TABLE_s *pUserPtb = AmbaBLD_GetUserPartTable();
    const AMBA_PARTITION_ENTRY_s *pPartEntry;
    UINT32 i, Crc32;
    char IntStr[16];
    const char *pString = NULL;
    const UINT8 *pWorkUINT8 = NULL;

#if defined(CONFIG_ENABLE_SPINOR_BOOT)
    (void)AmbaRTSL_NorSpiReadUserPTB(NULL, PtbNo);
#elif defined(CONFIG_ENABLE_EMMC_BOOT)
    (void)AmbaRTSL_EmmcReadUserPTB(NULL, PtbNo);
#else
    (void)AmbaRTSL_NandReadUserPTB(NULL, PtbNo);
#endif
    BldShell_PrintStr("\r\n\r\nPTB No: ");
    (void)IO_UtilityUInt32ToStr(IntStr, 16U, pUserPtb->PTBNumber, 16U);
    BldShell_PrintStr(IntStr);
    BldShell_PrintStr("\r\n");
    AmbaMisra_TypeCast32(&pWorkUINT8, &pUserPtb);
    Crc32 = IO_UtilityCrc32(pWorkUINT8, AMBA_USER_PTB_CRC32_SIZE);
    if (Crc32 == pUserPtb->CRC32) {
        pPartEntry = pUserPtb->Entry;
        for (i = 0; i < AMBA_NUM_USER_PARTITION; i++) {
            if ((pPartEntry->BlkCount > 0U) && (pPartEntry->BlkCount != 0xFFFFFFFFU)) {
                //AmbaRTSL_UartPrintf("%-16s: attribute: 0x%08x, mem_addr: 0x%08x, ", pPartEntry->PartitionName, pPartEntry->Attribute.Data, pPartEntry->RamLoadAddr);
                //AmbaRTSL_UartPrintf("blk_addr: from %4u to %4u\r\n", pPartEntry->StartBlkAddr, pPartEntry->StartBlkAddr + pPartEntry->BlkCount - 1);
                pWorkUINT8 = pPartEntry->PartitionName;
                AmbaMisra_TypeCast64(&pString, &pWorkUINT8);
                BldShell_PrintStr(pString);
                BldShell_PrintStr(": attribute: 0x");
                (void)IO_UtilityUInt32ToStr(IntStr, 16U, pPartEntry->Attribute, 16U);
                BldShell_PrintStr(IntStr);
                BldShell_PrintStr(", mem_addr: 0x");
                (void)IO_UtilityUInt32ToStr(IntStr, 16U, pPartEntry->RamLoadAddr, 16U);
                BldShell_PrintStr(IntStr);
                BldShell_PrintStr(", blk_addr: from ");
                (void)IO_UtilityUInt32ToStr(IntStr, 16U, pPartEntry->StartBlkAddr, 10U);
                BldShell_PrintStr(IntStr);
                BldShell_PrintStr(" to ");
                (void)IO_UtilityUInt32ToStr(IntStr, 16U, pPartEntry->StartBlkAddr + pPartEntry->BlkCount - 1U, 10U);
                BldShell_PrintStr(IntStr);
                BldShell_PrintStr("\r\n");
            }
            pPartEntry++;
        }
    }
}

static void BLD_CmdShow(void)
{
    if (0x0 == IO_UtilityStringCompare("show ptb0", BldCmdLine, 0x9U)) {
        BLD_ShowPartTable(0U);
    } else  if (0x0 == IO_UtilityStringCompare("show ptb1", BldCmdLine, 0x9U)) {
        BLD_ShowPartTable(1U);
    } else  if (0x0 == IO_UtilityStringCompare("show ptb", BldCmdLine, 0x8U)) {
        BLD_ShowPartTableInfo();
    } else {
        BldShell_PrintStr("Please key in \"show-ptb\"\r\n");
    }
}

static void BLD_CmdErase(void)
{
    UINT32 CommandDone = 0U;
    UINT32 i;

    if (0x0 == IO_UtilityStringCompare("erase all", BldCmdLine, 0x9U)) {
        BLD_EraseNvm();
    } else {
        for (i = 0; i < AMBA_NUM_SYS_PARTITION; i++) {
            if (0x0 == IO_UtilityStringCompare(SysPartName[i], &BldCmdLine[6U], SysPartNameLen[i])) {
                if (BLD_EraseNvmPart((UINT32)0U, i) == BLD_ERR_NONE) {
                    BldShell_PrintStr("Partition ");
                    BldShell_PrintStr(SysPartName[i]);
                    BldShell_PrintStr(" erase done\r\n");
                    CommandDone = (UINT32)1U;
                } else {
                    BldShell_PrintStr("Partition ");
                    BldShell_PrintStr(SysPartName[i]);
                    BldShell_PrintStr(" erase fail\r\n");
                    CommandDone = (UINT32)2U;
                }
            }
        }

        for (i = 0; i < AMBA_NUM_USER_PARTITION; i++) {
            if (0x0 == IO_UtilityStringCompare(UserPartName[i], &BldCmdLine[6U], UserPartNameLen[i])) {
                if (BLD_EraseNvmPart((UINT32)1U, i) == BLD_ERR_NONE) {
                    BldShell_PrintStr("Partition ");
                    BldShell_PrintStr(UserPartName[i]);
                    BldShell_PrintStr(" erase done\r\n");
                    CommandDone = (UINT32)1U;
                } else {
                    BldShell_PrintStr("Partition ");
                    BldShell_PrintStr(UserPartName[i]);
                    BldShell_PrintStr(" erase fail\r\n");
                    CommandDone = (UINT32)2U;
                }
            }
        }

        if (CommandDone != (UINT32)1U) {
            BldShell_PrintStr("Please key in \"erase [PartName]\"\r\n");
            BldShell_PrintStr("PartName:\r\n");

            for (i = 0U; i < AMBA_NUM_SYS_PARTITION; i++) {
                BldShell_PrintStr(SysPartName[i]);
                BldShell_PrintStr("\t");
            }
            BldShell_PrintStr(" \r\n");
            for (i = 0U; i < AMBA_NUM_USER_PARTITION; i++) {
                BldShell_PrintStr(UserPartName[i]);
                BldShell_PrintStr("\t");
            }
            BldShell_PrintStr(" \r\n");
        }
    }
}

void ddrc_train_write_start(void);
void ddrc_train_read_start(void);
void ddrc_training_seq_read_end(void);
void ddrc_training_seq_write_end(void);
void ddrc_training_seq_stage2_end(void);
void ddrc_training_LdBld(void);

extern UINT32 DRAM_lpddr4DqCaVrefOffsetParam;
#if 0
static void BLD_CmdStoreTrainingResult(UINT32 OpMode)
{
    static UINT8 BstBinBuf[512U*1024U] GNU_SECTION_NOZEROINIT;
#ifdef CONFIG_ENABLE_SPINOR_BOOT
    const UINT8 *pData = &BstBinBuf[BST_OFST_DRAM_HIGH_FREQ_PARAM + 0x80UL];
#else
    const UINT8 *pData = &BstBinBuf[BST_OFST_DRAM_HIGH_FREQ_PARAM];
#endif
    const AMBA_SYS_PARTITION_TABLE_s *pSysPtb = AmbaBLD_GetSysPartTable();

    UINT32 Crc32, Delay = 0, DqVrefOffset = 0, DqVref = 0;
    UINT32 i = 0, RegValueOri = 0, RegValueNew = 0;
    UINT32 *pDramParam;
    char IntStr[16];

    AmbaMisra_TypeCast64(&pDramParam, &pData);

    /* Start store training result to flash */
    AmbaMisra_TypeCast64(&pData, &pSysPtb);
    Crc32 = IO_UtilityCrc32(pData, sizeof(AMBA_SYS_PARTITION_TABLE_s) - 4U);
    if (Crc32 != pSysPtb->CRC32) {
        BldShell_PrintStr("System partition check fail.\r\n");
    } else {
        /* Read BST data to temp buffer */
        BLD_LoadNvmPart(0U, AMBA_SYS_PARTITION_BOOTSTRAP, BstBinBuf);

        if ((OpMode == DRAM_TRAIN_OPMODE_AUTO_READ_DONE) || (OpMode == DRAM_TRAIN_OPMODE_READ_DONE)) {
            DqVrefOffset = (AmbaCSL_DdrcGetDqVref() & 0x7fU);
            if(DRAM_lpddr4DqCaVrefOffsetParam > 0x7fU) {
                DqVrefOffset -= (DRAM_lpddr4DqCaVrefOffsetParam & 0x7fU);
            } else {
                DqVrefOffset += (DRAM_lpddr4DqCaVrefOffsetParam & 0x7fU);
            }
            DqVref = (DqVrefOffset << 21) | (DqVrefOffset << 14) | (DqVrefOffset << 7) | (DqVrefOffset);
            pDramParam[DRAM_TRAIN_PARAM_OFST_DQCA] = DqVref;
            /* Apply new Vref to ddrc before r/w bst */
            RegValueOri = AmbaCSL_DdrcGetDqVref() & 0x7fU;
            RegValueNew = DqVref & 0x7fU;
            if (RegValueNew > RegValueOri) {
                i = RegValueNew - RegValueOri;
                for (; i != 0UL; i--) {
                    RegValueOri = RegValueOri + 1U;
                    RegValueNew = (RegValueOri << 21U) | (RegValueOri << 14U) | (RegValueOri << 7U) | (RegValueOri);
                    AmbaCSL_DdrcSetDqVref(RegValueNew);
                }
            } else {
                i = RegValueOri - RegValueNew;
                for (; i != 0UL; i--) {
                    RegValueOri = RegValueOri - 1U;
                    RegValueNew = (RegValueOri << 21U) | (RegValueOri << 14U) | (RegValueOri << 7U) | (RegValueOri);
                    AmbaCSL_DdrcSetDqVref(RegValueNew);
                }
            }
        } else {
            pDramParam[DRAM_TRAIN_PARAM_OFST_DQCA] = AmbaCSL_DdrcGetDqVref();
        }

        pDramParam[DRAM_TRAIN_PARAM_OFST_FLAG] = ~(DRAM_TRAIN_FLAG_FAIL);
        pDramParam[DRAM_TRAIN_PARAM_OFST_DQ_WDLY] = AmbaCSL_DdrcGetDqWriteDly();
        pDramParam[DRAM_TRAIN_PARAM_OFST_DQCA] = AmbaCSL_DdrcGetDqVref();
        pDramParam[DRAM_TRAIN_PARAM_OFST_DLL0] = AmbaCSL_DdrcGetDll0Setting();
        pDramParam[DRAM_TRAIN_PARAM_OFST_DLL1] = AmbaCSL_DdrcGetDll1Setting();
        pDramParam[DRAM_TRAIN_PARAM_OFST_DLL2] = AmbaCSL_DdrcGetDll2Setting();
        pDramParam[DRAM_TRAIN_PARAM_OFST_DLL3] = AmbaCSL_DdrcGetDll3Setting();
        pDramParam[DRAM_TRAIN_PARAM_OFST_DQSDLY] = AmbaCSL_DdrcGetDqReadDly();

        AmbaCSL_DdrcSetModeReg(0x000e0000);
        while (0x0U != (AmbaCSL_DdrcGetModeReg() & 0x80000000U)) {
            ;
        }
        Delay = AmbaCSL_DdrcGetModeReg();
        pDramParam[DRAM_TRAIN_PARAM_OFST_MR14] = ((0x010e0000U) | (Delay & 0xffU));

        pDramParam[DRAM_TRAIN_PARAM_OFST_FLAG_V] = 0x0;
        pDramParam[DRAM_TRAIN_PARAM_OFST_STATE] = OpMode;

        (void)AmbaRTSL_CacheCleanDataPtr(BstBinBuf, 0x1000U);

        /* Write BST data back */
        BLD_WriteNvmPart(0, (UINT32)AMBA_SYS_PARTITION_BOOTSTRAP, 0x1000U, BstBinBuf);

        if ((OpMode == DRAM_TRAIN_OPMODE_AUTO_READ_DONE) || (OpMode == DRAM_TRAIN_OPMODE_READ_DONE)) {
            BldShell_PrintStr("\r\nDLL0               0x");
            (void)IO_UtilityUInt32ToStr(IntStr, 16U, AmbaCSL_DdrcGetDll0Setting(), 16U);
            BldShell_PrintStr(IntStr);
            BldShell_PrintStr("\r\nDLL1               0x");
            (void)IO_UtilityUInt32ToStr(IntStr, 16U, AmbaCSL_DdrcGetDll1Setting(), 16U);
            BldShell_PrintStr(IntStr);
            BldShell_PrintStr("\r\nDLL2               0x");
            (void)IO_UtilityUInt32ToStr(IntStr, 16U, AmbaCSL_DdrcGetDll2Setting(), 16U);
            BldShell_PrintStr(IntStr);
            BldShell_PrintStr("\r\nDLL3               0x");
            (void)IO_UtilityUInt32ToStr(IntStr, 16U, AmbaCSL_DdrcGetDll3Setting(), 16U);
            BldShell_PrintStr(IntStr);
            BldShell_PrintStr("\r\nReadVref           0x");
            (void)IO_UtilityUInt32ToStr(IntStr, 16U, AmbaCSL_DdrcGetDqVref(), 16U);
            BldShell_PrintStr(IntStr);
            BldShell_PrintStr("\r\nDqVrefOffset       0x");
            (void)IO_UtilityUInt32ToStr(IntStr, 16U, DqVrefOffset, 16U);
            BldShell_PrintStr(IntStr);
            BldShell_PrintStr("\r\nDqVref             0x");
            (void)IO_UtilityUInt32ToStr(IntStr, 16U, DqVref, 16U);
            BldShell_PrintStr(IntStr);
            BldShell_PrintStr("\r\nDQs Read delay     0x");
            (void)IO_UtilityUInt32ToStr(IntStr, 16U, (AmbaCSL_DdrcGetDqReadDly()), 16U);
            BldShell_PrintStr(IntStr);
            BldShell_PrintStr("\r\nStore back parameter done.\r\n");
            BldShell_PrintStr("\r\n");
            BldShell_PrintStr("\r\n");
            BldShell_PrintStr("\r\n");
            BldShell_PrintStr("\r\n");
            BldShell_PrintStr("\r\n");
            BldShell_PrintStr("\r\n");
        } else if ((OpMode == DRAM_TRAIN_OPMODE_AUTO_WRITE_DONE) || (OpMode == DRAM_TRAIN_OPMODE_WRITE_DONE)) {
            BldShell_PrintStr("\r\nDQ write delay     0x");
            (void)IO_UtilityUInt32ToStr(IntStr, 16U, (AmbaCSL_DdrcGetDqWriteDly()), 16U);
            BldShell_PrintStr(IntStr);
            BldShell_PrintStr("\r\nMR14               0x");
            (void)IO_UtilityUInt32ToStr(IntStr, 16U, Delay&0xffU, 16U);
            BldShell_PrintStr(IntStr);
            BldShell_PrintStr("\r\nStore back parameter done.\r\n");
            BldShell_PrintStr("\r\n");
            BldShell_PrintStr("\r\n");
            BldShell_PrintStr("\r\n");
            BldShell_PrintStr("\r\n");
            BldShell_PrintStr("\r\n");
            BldShell_PrintStr("\r\n");
        } else {
            /* for MisraC checking */
        }
    }
}

static void BLD_CmdDramReadTrain(void)
{
    UINT64 Addr = AMBA_SCRATCH_PAD_BASE_ADDR;
    UINT64 Start, End;
    const UINT8 *pDramCode;
    UINT8 *pFifoCode;
    UINT32 i;
    void (*FuncDoDramTrainSeq)(void);
#ifdef CONFIG_ENABLE_SPINOR_BOOT
    AmbaRTSL_SpiNORDeviceReset();
#endif

    FuncDoDramTrainSeq = ddrc_train_read_start;
    AmbaMisra_TypeCast64(&Start, &(FuncDoDramTrainSeq));
    FuncDoDramTrainSeq = ddrc_training_seq_read_end;
    AmbaMisra_TypeCast64(&End, &FuncDoDramTrainSeq);

    AmbaMisra_TypeCast64(&pFifoCode, &Addr);
    FuncDoDramTrainSeq = ddrc_train_read_start;
    AmbaMisra_TypeCast64(&pDramCode, &FuncDoDramTrainSeq);
    for (i = 0U; i < (End - Start); i++) {
        pFifoCode[i] = pDramCode[i];
    }

    FuncDoDramTrainSeq = ddrc_training_LdBld;
    AmbaMisra_TypeCast64(&Start, &FuncDoDramTrainSeq);
    FuncDoDramTrainSeq = ddrc_training_seq_stage2_end;
    AmbaMisra_TypeCast64(&End, &FuncDoDramTrainSeq);

    Addr = AHB_SCRATCH_PAD_LOG_SPACE_END;
    AmbaMisra_TypeCast64(&pFifoCode, &Addr);
    FuncDoDramTrainSeq = ddrc_training_LdBld;
    AmbaMisra_TypeCast64(&pDramCode, &FuncDoDramTrainSeq);
    for (i = 0U; i < (End - Start); i++) {
        pFifoCode[i] = pDramCode[i];
    }

    AmbaMMU_Disable();

    Addr = AMBA_SCRATCH_PAD_BASE_ADDR;
    AmbaMisra_TypeCast64(&FuncDoDramTrainSeq, &Addr);
    FuncDoDramTrainSeq();
}

static void BLD_CmdDramWriteTrain(void)
{
    UINT64 Addr = AMBA_SCRATCH_PAD_BASE_ADDR;
    UINT64 Start, End;
    const UINT8 *pDramCode;
    UINT8 *pFifoCode;
    UINT32 i;
    void (*FuncDoDramTrainSeq)(void);

#ifdef CONFIG_ENABLE_SPINOR_BOOT
    AmbaRTSL_SpiNORDeviceReset();
#endif

    FuncDoDramTrainSeq = ddrc_train_write_start;
    AmbaMisra_TypeCast64(&Start, &FuncDoDramTrainSeq);
    FuncDoDramTrainSeq = ddrc_training_seq_write_end;
    AmbaMisra_TypeCast64(&End, &FuncDoDramTrainSeq);

    AmbaMisra_TypeCast64(&pFifoCode, &Addr);
    FuncDoDramTrainSeq = ddrc_train_write_start;
    AmbaMisra_TypeCast64(&pDramCode, &FuncDoDramTrainSeq);
    for (i = 0U; i < (End - Start); i++) {
        pFifoCode[i] = pDramCode[i];
    }

    FuncDoDramTrainSeq = ddrc_training_LdBld;
    AmbaMisra_TypeCast64(&Start, &FuncDoDramTrainSeq);
    FuncDoDramTrainSeq = ddrc_training_seq_stage2_end;
    AmbaMisra_TypeCast64(&End, &FuncDoDramTrainSeq);

    Addr = AHB_SCRATCH_PAD_LOG_SPACE_END;
    AmbaMisra_TypeCast64(&pFifoCode, &Addr);
    FuncDoDramTrainSeq = ddrc_training_LdBld;
    AmbaMisra_TypeCast64(&pDramCode, &FuncDoDramTrainSeq);
    for (i = 0U; i < (End - Start); i++) {
        pFifoCode[i] = pDramCode[i];
    }

    AmbaMMU_Disable();

    Addr = AMBA_SCRATCH_PAD_BASE_ADDR;
    AmbaMisra_TypeCast64(&FuncDoDramTrainSeq, &Addr);
    FuncDoDramTrainSeq();
}

static void BLD_ShellDramTraining(UINT32 StoreRegVal, UINT32 OpMode)
{
    if (0x0U != StoreRegVal) {
        if (0x0U == (AmbaCSL_DdrcGetTrainScratchPad() & DRAM_TRAIN_PROC_RESULT_MASK)) {
            BLD_CmdStoreTrainingResult(OpMode);
#ifdef CONFIG_ENABLE_SPINOR_BOOT
            AmbaRTSL_SpiNORDeviceReset();
#endif

#ifdef CONFIG_ENABLE_EMMC_BOOT
            AmbaRTSL_SDPhyDisable(0U);
            (void)AmbaRTSL_PllSetSd0Clk(24000000U);
            AmbaRTSL_SdClockEnable(0U, 1U);
            (void)AmbaRTSL_SdSendCMD0(0U, 0xF0F0F0F0U);
#endif
            AmbaCSL_RctChipSoftReset();
        } else {
            BldShell_PrintStr("Last dram training failed.\r\n");
        }
    }

    if (OpMode == DRAM_TRAIN_OPMODE_READ) {
        BldShell_PrintStr("Start Dram Training Read.\r\n");
        AmbaCSL_DdrcSetTrainScratchPad((UINT32)DRAM_TRAIN_OPMODE_READ_DONE << DRAM_TRAIN_PROC_OPMODE_OFST);
        BLD_CmdDramReadTrain();
    } else if (OpMode == DRAM_TRAIN_OPMODE_WRITE) {
        BldShell_PrintStr("Start Dram Training Write.\r\n");
        AmbaCSL_DdrcSetTrainScratchPad((UINT32)DRAM_TRAIN_OPMODE_WRITE_DONE << DRAM_TRAIN_PROC_OPMODE_OFST);
        BLD_CmdDramWriteTrain();
    } else if (OpMode == DRAM_TRAIN_OPMODE_AUTO_READ) {
        BldShell_PrintStr("Start Dram Training Read.\r\n");
        AmbaCSL_DdrcSetTrainScratchPad((UINT32)DRAM_TRAIN_OPMODE_AUTO_READ_DONE << DRAM_TRAIN_PROC_OPMODE_OFST);
        BLD_CmdStoreTrainingResult(DRAM_TRAIN_OPMODE_AUTO_READ);
        BLD_CmdDramReadTrain();
    } else if (OpMode == DRAM_TRAIN_OPMODE_AUTO_WRITE) {
        BldShell_PrintStr("Start Dram Training Write.\r\n");
        AmbaCSL_DdrcSetTrainScratchPad((UINT32)DRAM_TRAIN_OPMODE_AUTO_WRITE_DONE << DRAM_TRAIN_PROC_OPMODE_OFST);
        BLD_CmdStoreTrainingResult((UINT32)DRAM_TRAIN_OPMODE_AUTO_WRITE);
        BLD_CmdDramWriteTrain();
    } else if (OpMode == DRAM_TRAIN_OPMODE_FAIL) {
        /* Training fail at first boot */
        BldShell_PrintStr("Training failed last time.\r\n");
    } else {
        /* do nothing */
    }
}

static void BLD_CmdDramTrain(void)
{
    if (0x0 == IO_UtilityStringCompare("read", &BldCmdLine[9], (SIZE_t)0x4)) {
        BLD_ShellDramTraining(0, DRAM_TRAIN_OPMODE_READ);
    } else if (0x0 == IO_UtilityStringCompare("write", &BldCmdLine[9], (SIZE_t)0x5)) {
        BLD_ShellDramTraining(0, DRAM_TRAIN_OPMODE_WRITE);
    } else {
        BldShell_PrintStr("Error command\r\n");
        BldShell_PrintStr("Please keyin \"training read\"  for read training\r\n");
        BldShell_PrintStr("Please keyin \"training write\" for write training\r\n");
    }
}
#endif
static void BLD_CmdHelp(void)
{
    BldShell_PrintStr("The following commands are supported:\r\n");
    BldShell_PrintStr("\"show ptb\"  display partition table info\r\n");
    BldShell_PrintStr("\"show ptb0\"  display user partition table 0 info\r\n");
    BldShell_PrintStr("\"show ptb1\"  display user partition table 1 info\r\n");
    BldShell_PrintStr("\"erase [PartName]\" erase partition data on boot device\r\n");
    BldShell_PrintStr("\"boot\"      enter kernel\r\n");
}
#if 0
void BLD_CheckTrainingResult(void)
{
    UINT32 DramTrainingState;

    DramTrainingState = AmbaCSL_DdrcGetTrainScratchPad();
    (void)AmbaRTSL_CacheFlushDataPtr(&DramTrainingState, sizeof(UINT32));
    if ( MAGIC_CODE_DRAM_TRAIN_COMPLETION != DramTrainingState ) {
        if (MAGIC_CODE_DRAM_TRAIN_FAILURE == DramTrainingState) {
            BldShell_PrintStr("\r\nPlease try dram training manually\r\n");
        } else {
            DramTrainingState = (DramTrainingState >> DRAM_TRAIN_PROC_OPMODE_OFST);
            if ( DramTrainingState == 0U ) {
#ifdef CONFIG_AUTO_DRAM_TRAINING
                BLD_ShellDramTraining(0U, DRAM_TRAIN_OPMODE_AUTO_READ);
#endif
            } else if ( DramTrainingState == DRAM_TRAIN_OPMODE_AUTO_READ_DONE ) {
                BLD_CmdStoreTrainingResult(DRAM_TRAIN_OPMODE_AUTO_READ_DONE);
#ifdef CONFIG_ENABLE_SPINOR_BOOT
                AmbaRTSL_SpiNORDeviceReset();
#endif
#ifdef CONFIG_ENABLE_EMMC_BOOT
                AmbaRTSL_SDPhyDisable(0U);
                (void)AmbaRTSL_PllSetSd0Clk(24000000U);
                AmbaRTSL_SdClockEnable(0U, 1U);
                (void)AmbaRTSL_SdSendCMD0(0U, 0xF0F0F0F0U);
#endif
                AmbaCSL_RctChipSoftReset();
            } else if ( DramTrainingState == DRAM_TRAIN_OPMODE_AUTO_WRITE) {
                BLD_ShellDramTraining(0U, DRAM_TRAIN_OPMODE_AUTO_WRITE);
            } else if ( DramTrainingState == DRAM_TRAIN_OPMODE_AUTO_WRITE_DONE ) {
                BLD_ShellDramTraining(1U, DRAM_TRAIN_OPMODE_WRITE_DONE);
            } else if ( DramTrainingState == DRAM_TRAIN_OPMODE_READ_DONE ) {
                BLD_ShellDramTraining(1U, DRAM_TRAIN_OPMODE_READ_DONE);
            } else if ( DramTrainingState == DRAM_TRAIN_OPMODE_WRITE_DONE ) {
                BLD_ShellDramTraining(1U, DRAM_TRAIN_OPMODE_WRITE_DONE);
            } else {
                BldShell_PrintStr("\r\nPlease try dram training manually\r\n");
            }
        }
    } else {
        BldShell_PrintStr("\r\nDram has been trained\r\n");
    }
}
#endif
/**
 *  @RoutineName:: BLD_ShellStart
 *
 *  @Description:: start shell for BLD
 *
 *  @Input      ::
 *      ForceStartFlag: 0 - start shell if and only if escape key detected
 *
 *  @Output     :: none
 *
 *  @Return     :: none
 */
void BLD_ShellStart(UINT32 ForceStartFlag)
{
    UINT32 SysCtrlReg, Len;
    UINT32 RetVal, PtbStatus = BLD_ERR_NONE;

    /* Enter amboot if there is any escape key detected */
    RetVal = BLD_DetectEscapeKey(15U);
    if ((BLD_ERR_NONE != RetVal) && (0x0U == ForceStartFlag)) {
        RetVal = BLD_ERR_TIMEOUT;
    } else {
        /**
         * Disable data caching
         */
        AmbaRTSL_CacheFlushDataAll();

        SysCtrlReg = AmbaRTSL_CpuReadSysCtrl();
        SysCtrlReg &= ~((UINT32)1U << (UINT32)2U);
        AmbaRTSL_CpuWriteSysCtrl(SysCtrlReg);

        BldShell_PrintStr("\x1b[4l");                 /* Set terminal to replacement mode */
        BldShell_PrintStr("\r\n");                    /* First, output a blank line to UART */
        BldShell_PrintStr("\r\n");

        BldShell_PartInfoInit();

        if (BLD_ERR_NONE != BLD_InitBootDevice()) {
            BldShell_PrintStr("Boot device init fail, please burn-in new image\r\n");
            PtbStatus = BLD_ERR_INFO;
        }

        /**
         * Endless command loop
         */
        for(;;) {
            BldShell_PrintStr("\r\namboot> ");

            RetVal = BLD_GetCmdLine(BldCmdLine, sizeof(BldCmdLine), &Len);
            if ((RetVal == BLD_ERR_NONE) && (0x0U != Len)) {
                BldShell_PrintStr("\r\n");

                if (0x0 == IO_UtilityStringCompare("boot", BldCmdLine, (SIZE_t)0x4U)) {
                    if (PtbStatus != BLD_ERR_INFO) {
                        BLD_LoadFirmware();
                        BLD_EnterKernel();
                    } else {
                        BldShell_PrintStr("Partition info error\r\n");
                    }
                } else if (0x0 == IO_UtilityStringCompare("help", BldCmdLine, (SIZE_t)0x4)) {
                    BLD_CmdHelp();
                } else if (0x0 == IO_UtilityStringCompare("show", BldCmdLine, (SIZE_t)0x4)) {
                    if (PtbStatus != BLD_ERR_INFO) {
                        BLD_CmdShow();
                    } else {
                        BldShell_PrintStr("Partition info error\r\n");
                    }
                } else if (0x0 == IO_UtilityStringCompare("erase", BldCmdLine, 0x5)) {
                    if (PtbStatus != BLD_ERR_INFO) {
                        BLD_CmdErase();
                    } else {
                        BldShell_PrintStr("Partition info error\r\n");
                    }
                } else if (0x0 == IO_UtilityStringCompare("training", BldCmdLine, 0x8)) {
                    if (PtbStatus != BLD_ERR_INFO) {
                        //BLD_CmdDramTrain();
                    } else {
                        BldShell_PrintStr("Partition info error\r\n");
                    }
                } else {
                    BldShell_PrintStr("Error: unknown command.\r\n");
                }
            }
        }
    }
    AmbaMisra_TouchUnused(&RetVal);
}

