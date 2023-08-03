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
#include "AmbaDramTrain.h"

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
#include "AmbaCSL_DDRC.h"
#include "AmbaCSL_Scratchpad.h"

#define DEBUG_TRAINING      0

static char SysPartName[AMBA_NUM_SYS_PARTITION][10U];
static UINT8 SysPartNameLen[AMBA_NUM_SYS_PARTITION];
static char UserPartName[AMBA_NUM_USER_PARTITION][10U];
static UINT8 UserPartNameLen[AMBA_NUM_USER_PARTITION];

#define PRINT_BUF_SIZE  (1024U)
static char BldCmdLine[128] GNU_SECTION_NOZEROINIT;
static UINT8 AmbaTmpBuffer[4096U];//(NAND)Page size:4K

static void BldShell_PartInfoInit(void)
{
    SysPartNameLen[AMBA_SYS_PARTITION_BOOTSTRAP]    = 3U;
    SysPartNameLen[AMBA_SYS_PARTITION_BOOTLOADER]   = 3U;
    SysPartNameLen[AMBA_SYS_PARTITION_FW_UPDATER]   = 2U;
    SysPartNameLen[AMBA_SYS_PARTITION_ARM_TRUST_FW] = 3U;

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
#if !defined(CONFIG_APP_FLOW_CARDV_AONI)
    UserPartNameLen[AMBA_USER_PARTITION_RESERVED0] = 4U;
    UserPartNameLen[AMBA_USER_PARTITION_RESERVED1] = 4U;
    UserPartNameLen[AMBA_USER_PARTITION_RESERVED2] = 4U;
    UserPartNameLen[AMBA_USER_PARTITION_RESERVED3] = 4U;
    UserPartNameLen[AMBA_USER_PARTITION_RESERVED4] = 4U;
#endif

    IO_UtilityStringCopy(SysPartName[AMBA_SYS_PARTITION_BOOTSTRAP], 10U, "bst");
    IO_UtilityStringCopy(SysPartName[AMBA_SYS_PARTITION_BOOTLOADER], 10U, "bld");
    IO_UtilityStringCopy(SysPartName[AMBA_SYS_PARTITION_FW_UPDATER], 10U, "fw");
    IO_UtilityStringCopy(SysPartName[AMBA_SYS_PARTITION_ARM_TRUST_FW], 10U, "atf");

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
#if !defined(CONFIG_APP_FLOW_CARDV_AONI)
    IO_UtilityStringCopy(UserPartName[AMBA_USER_PARTITION_RESERVED0], 10U, "rsv0");
    IO_UtilityStringCopy(UserPartName[AMBA_USER_PARTITION_RESERVED1], 10U, "rsv1");
    IO_UtilityStringCopy(UserPartName[AMBA_USER_PARTITION_RESERVED2], 10U, "rsv2");
    IO_UtilityStringCopy(UserPartName[AMBA_USER_PARTITION_RESERVED3], 10U, "rsv3");
    IO_UtilityStringCopy(UserPartName[AMBA_USER_PARTITION_RESERVED4], 10U, "rsv4");
#endif
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

/*static*/ void BldShell_PrintStrInt(const char *pFmt, UINT32 value)
{
    char IntStr[16];

    BldShell_PrintStr(pFmt);
    (void)IO_UtilityUInt32ToStr(IntStr, 16U, value, 16U);
    BldShell_PrintStr(IntStr);
    BldShell_PrintStr("\r\n");
}

#if DEBUG_TRAINING
static void BldShell_PrintMemory(UINT32 AddrStart, UINT32 AddrEnd, UINT32 *Address)
{
    char IntStr[16];
    UINT32 i = 0U, max = 256U;

    for (i = 0; (i < max) && (AddrStart < AddrEnd); i += 4)  {
        BldShell_PrintStr("0x");
        (void)IO_UtilityUInt32ToStr(IntStr, 16U, AddrStart, 16U);
        BldShell_PrintStr(IntStr);
        BldShell_PrintStr(": 0x");
        (void)IO_UtilityUInt32ToStr(IntStr, 16U, Address[i], 16U);
        BldShell_PrintStr(IntStr);
        BldShell_PrintStr(" 0x");
        (void)IO_UtilityUInt32ToStr(IntStr, 16U, Address[i+1], 16U);
        BldShell_PrintStr(IntStr);
        BldShell_PrintStr(" 0x");
        (void)IO_UtilityUInt32ToStr(IntStr, 16U, Address[i+2], 16U);
        BldShell_PrintStr(IntStr);
        BldShell_PrintStr(" 0x");
        (void)IO_UtilityUInt32ToStr(IntStr, 16U, Address[i+3], 16U);
        BldShell_PrintStr(IntStr);
        BldShell_PrintStr("\r\n");
        AddrStart += 16U;
    }
}

static void BldShell_PrintScratchPad(void)
{
    UINT32 Start, End;
    UINT32 *pScratchPad;
    UINT64 StartU64;

    /* Show training parameter */
    Start = AHB_SCRATCH_PAD_PARAM_SPACE;
    StartU64 = Start;
    AmbaMisra_TypeCast64(&pScratchPad, &StartU64);
    End = Start + 16U;
    BldShell_PrintMemory(Start, End, pScratchPad);

    /* Show training log */
    Start = AHB_SCRATCH_PAD_LOG_SPACE;
    StartU64 = Start;
    AmbaMisra_TypeCast64(&pScratchPad, &StartU64);
    End = pScratchPad[0];
    BldShell_PrintMemory(Start, End, pScratchPad);
}
#else   /* DEBUG_TRAINING */
//static void BldShell_PrintScratchPad(void)  {}
#endif  /* DEBUG_TRAINING */

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
        if ((UartRxSize == 0x1U) && (/*(Char == 0x0aU) || (Char == 0x0dU) ||*/ (Char == 0x1bU))) {
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
#if defined(CONFIG_APP_FLOW_CARDV_AONI)
                BldShell_PrintStr(", crc32: 0x");
                (void)IO_UtilityUInt32ToStr(IntStr, 16U, pPartEntry->ImageCRC32, 16U);
                BldShell_PrintStr(IntStr);
#endif
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
#if defined(CONFIG_APP_FLOW_CARDV_AONI)
                BldShell_PrintStr(", crc32: 0x");
                (void)IO_UtilityUInt32ToStr(IntStr, 16U, pPartEntry->ImageCRC32, 16U);
                BldShell_PrintStr(IntStr);
#endif
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

#ifdef CONFIG_DRAM_TYPE_LPDDR4
void ddrc_train_ca_start(void);
void ddrc_train_write_start(void);
void ddrc_train_read_start(void);
void ddrc_training_seq_ca_end(void);
void ddrc_training_seq_read_end(void);
void ddrc_training_seq_write_end(void);
void ddrc_training_seq_stage2_end(void);
void ddrc_training_LdBld(void);

extern UINT32 DRAM_lpddr4DqCaVrefOffsetParam;
extern UINT32 DRAM_lpddr4Mr12OffsetParam;
extern UINT32 DRAM_lpddr4Mr14OffsetParam;
extern UINT32 DRAM_lpddr4DqWriteOffsetParam;
extern UINT32 DRAM_lpddr4DqReadOffsetParam;
extern UINT32 DRAM_lpddr4DllOffsetParam;

static void BLD_ShellReboot(void)
{
#define EXTRA_PRINT_AFTER_REBOOT    36
    INT32 i;

#ifdef CONFIG_ENABLE_SPINOR_BOOT
    AmbaRTSL_SpiNORDeviceReset();
#endif

#ifdef CONFIG_ENABLE_EMMC_BOOT
    AmbaRTSL_SDPhyDisable(0U);
    (void)AmbaRTSL_PllSetSd0Clk(24000000U);
    AmbaRTSL_SdClockEnable(0U, 1U);
    (void)AmbaRTSL_SdSendCMD0(0U, 0xF0F0F0F0U);
#endif
    BldShell_PrintStr("Rebooting...");
    for (i = 0; i < EXTRA_PRINT_AFTER_REBOOT; i++) {
        BldShell_PrintStr("\r\n");
    }
    AmbaCSL_RctChipSoftReset();
}

static void BLD_CmdShowTrainingResult(UINT32 OpMode, UINT32 BstTrainingResult)
{
    char IntStr[16];

    if (BstTrainingResult == 1U) {
        BldShell_PrintStr("============ BST result ===========");
    }
    if ((OpMode == DRAM_TRAIN_OPMODE_AUTO_READ_DONE) || (OpMode == DRAM_TRAIN_OPMODE_READ_DONE) || (OpMode == DRAM_TRAIN_OPMODE_BOTH_RW_DONE)) {
        BldShell_PrintStr("\r\nDLL0               0x");
        (void)IO_UtilityUInt32ToStr(IntStr, 16U, AmbaCSL_DdrcGetDll0Setting(0U), 16U);
        BldShell_PrintStr(IntStr);
        BldShell_PrintStr("\r\nDLL1               0x");
        (void)IO_UtilityUInt32ToStr(IntStr, 16U, AmbaCSL_DdrcGetDll1Setting(0U), 16U);
        BldShell_PrintStr(IntStr);
        BldShell_PrintStr("\r\nDLL2               0x");
        (void)IO_UtilityUInt32ToStr(IntStr, 16U, AmbaCSL_DdrcGetDll2Setting(0U), 16U);
        BldShell_PrintStr(IntStr);
        BldShell_PrintStr("\r\nDLL3               0x");
        (void)IO_UtilityUInt32ToStr(IntStr, 16U, AmbaCSL_DdrcGetDll3Setting(0U), 16U);
        BldShell_PrintStr(IntStr);
        BldShell_PrintStr("\r\nReadVref           0x");
        (void)IO_UtilityUInt32ToStr(IntStr, 16U, AmbaCSL_DdrcGetDqVref(0U), 16U);
        BldShell_PrintStr(IntStr);
        BldShell_PrintStr("\r\nDQ Read delay      0x");
        (void)IO_UtilityUInt32ToStr(IntStr, 16U, (AmbaCSL_DdrcGetDqReadDly(0U)), 16U);
        BldShell_PrintStr(IntStr);
        AmbaCSL_DdrcSetModeReg(0U, 0x000c0000);
        while (0x0U != (AmbaCSL_DdrcGetModeReg(0U) & 0x80000000U)) {
            ;
        }
        BldShell_PrintStr("\r\nMR12               0x");
        (void)IO_UtilityUInt32ToStr(IntStr, 16U, AmbaCSL_DdrcGetModeReg(0U) & 0xffU, 16U);
        BldShell_PrintStr(IntStr);

        if (BstTrainingResult == 0U) {
            BldShell_PrintStr("\r\nStore back parameter done.\r\n");
            BldShell_PrintScratchPad();
        }
    }
    if ((OpMode == DRAM_TRAIN_OPMODE_AUTO_WRITE_DONE) || (OpMode == DRAM_TRAIN_OPMODE_WRITE_DONE) || (OpMode == DRAM_TRAIN_OPMODE_BOTH_RW_DONE)) {
        AmbaCSL_DdrcSetModeReg(0U, 0x000e0000);
        while (0x0U != (AmbaCSL_DdrcGetModeReg(0U) & 0x80000000U)) {
            ;
        }
        BldShell_PrintStr("\r\nDQ write delay     0x");
        (void)IO_UtilityUInt32ToStr(IntStr, 16U, (AmbaCSL_DdrcGetDqWriteDly(0U)), 16U);
        BldShell_PrintStr(IntStr);
        BldShell_PrintStr("\r\nMR14               0x");
        (void)IO_UtilityUInt32ToStr(IntStr, 16U, AmbaCSL_DdrcGetModeReg(0U) & 0xffU, 16U);
        BldShell_PrintStr(IntStr);

        if (BstTrainingResult == 0U) {
            BldShell_PrintStr("\r\nStore back parameter done.\r\n");
            BldShell_PrintScratchPad();
        }
    } else {
        /* for MisraC checking */
    }
    if (BstTrainingResult == 1U) {
        BldShell_PrintStr("\r\n===================================");
        BldShell_PrintStr("\r\n");
    }
}

static void BLD_CmdStoreTrainingResult(UINT32 OpMode)
{
    UINT8 *pBuffer = &AmbaTmpBuffer[0];
    const AMBA_SYS_PARTITION_TABLE_s *pSysPtb = AmbaBLD_GetSysPartTable();

    UINT32 Crc32, Delay = 0, DqVrefOffset = 0, DqVref = 0;
    UINT32 i = 0, RegValueOri = 0, RegValueNew = 0;
    UINT32 *pDramParam;


    /* Start store training result to flash */
    AmbaMisra_TypeCast64(&pBuffer, &pSysPtb);
    Crc32 = IO_UtilityCrc32(pBuffer, sizeof(AMBA_SYS_PARTITION_TABLE_s) - 4U);
    if (Crc32 != pSysPtb->CRC32) {
        BldShell_PrintStr("System partition check fail.\r\n");
    } else {
        pBuffer = &AmbaTmpBuffer[0U];
        AmbaMisra_TypeCast64(&pDramParam, &pBuffer);

        /* Do the required reset of training parameter once */
        if (pDramParam[DRAM_TRAIN_PARAM_OFST_RESET] != MAGIC_CODE_DRAM_TRAIN_RESET) {
            pDramParam[DRAM_TRAIN_PARAM_OFST_STATE] = 0;        // Majorly for reset valid bits
            pDramParam[DRAM_TRAIN_PARAM_OFST_RESET] = MAGIC_CODE_DRAM_TRAIN_RESET;
        }

        if ((OpMode == DRAM_TRAIN_OPMODE_AUTO_READ_DONE) || (OpMode == DRAM_TRAIN_OPMODE_READ_DONE) || (OpMode == DRAM_TRAIN_OPMODE_BOTH_RW_DONE)) {
            DqVrefOffset = (AmbaCSL_DdrcGetDqVref(0U) & 0x7fU);
#if 0
            if(DRAM_lpddr4DqCaVrefOffsetParam > 0x7fU) {
                DqVrefOffset -= (DRAM_lpddr4DqCaVrefOffsetParam & 0x7fU);
            } else {
                DqVrefOffset += (DRAM_lpddr4DqCaVrefOffsetParam & 0x7fU);
            }
#endif
            DqVref = (DqVrefOffset << 21) | (DqVrefOffset << 14) | (DqVrefOffset << 7) | (DqVrefOffset);
            pDramParam[DRAM_TRAIN_PARAM_OFST_DQCA] = DqVref;

            RegValueOri = AmbaCSL_DdrcGetDqVref(0U) & 0x7fU;
            RegValueNew = DqVref & 0x7fU;
            if (RegValueNew > RegValueOri) {
                i = RegValueNew - RegValueOri;
                for (; i != 0UL; i--) {
                    RegValueOri = RegValueOri + 1U;
                    RegValueNew = (RegValueOri << 21U) | (RegValueOri << 14U) | (RegValueOri << 7U) | (RegValueOri);
                    AmbaCSL_DdrcSetDqVref(0U, RegValueNew);
                }
            } else {
                i = RegValueOri - RegValueNew;
                for (; i != 0UL; i--) {
                    RegValueOri = RegValueOri - 1U;
                    RegValueNew = (RegValueOri << 21U) | (RegValueOri << 14U) | (RegValueOri << 7U) | (RegValueOri);
                    AmbaCSL_DdrcSetDqVref(0U, RegValueNew);
                }
            }
            pDramParam[DRAM_TRAIN_PARAM_OFST_DLL0] = AmbaCSL_DdrcGetDll0Setting(0U);
            pDramParam[DRAM_TRAIN_PARAM_OFST_DLL1] = AmbaCSL_DdrcGetDll1Setting(0U);
            pDramParam[DRAM_TRAIN_PARAM_OFST_DLL2] = AmbaCSL_DdrcGetDll2Setting(0U);
            pDramParam[DRAM_TRAIN_PARAM_OFST_DLL3] = AmbaCSL_DdrcGetDll3Setting(0U);
            pDramParam[DRAM_TRAIN_PARAM_OFST_DQSDLY] = AmbaCSL_DdrcGetDqReadDly(0U);
            pDramParam[DRAM_TRAIN_PARAM_OFST_STATE] |= DRAM_TRAIN_VALID_READ;

            AmbaCSL_DdrcSetModeReg(0U, 0x000c0000);
            while (0x0U != (AmbaCSL_DdrcGetModeReg(0U) & 0x80000000U)) {
                ;
            }
            Delay = AmbaCSL_DdrcGetModeReg(0U);
            Delay = Delay & 0xffU;
            if (Delay & 0x40U) {
                Delay = (Delay & 0x3fU) - 21U;
                Delay = Delay + 51U;
            }
            if (0x0U != DRAM_lpddr4Mr12OffsetParam) {
                if (DRAM_lpddr4Mr12OffsetParam > 0x7fU) {
                    if (Delay > (DRAM_lpddr4Mr12OffsetParam & 0x7fU)) {
                        Delay = Delay - (DRAM_lpddr4Mr12OffsetParam & 0x7fU);
                    } else {
                        Delay = 0U;
                    }
                } else {
                    Delay = Delay + DRAM_lpddr4Mr12OffsetParam;
                    if (Delay > 80U) {
                        Delay = 80;
                    }
                }
            }
            if (Delay > 50U) {
                Delay = Delay - 51U + 21U;
                Delay = Delay | 0x40U;
            }
            pDramParam[DRAM_TRAIN_PARAM_OFST_MR12] = ((0x010c0000U) | Delay);
        }
        /* Store Write result */
        if ((OpMode == DRAM_TRAIN_OPMODE_AUTO_WRITE_DONE) || (OpMode == DRAM_TRAIN_OPMODE_WRITE_DONE) || (OpMode == DRAM_TRAIN_OPMODE_BOTH_RW_DONE)) {
            pDramParam[DRAM_TRAIN_PARAM_OFST_DQ_WDLY] = AmbaCSL_DdrcGetDqWriteDly(0U);
            AmbaCSL_DdrcSetModeReg(0U, 0x000e0000);
            while (0x0U != (AmbaCSL_DdrcGetModeReg(0U) & 0x80000000U)) {
                ;
            }
            Delay = AmbaCSL_DdrcGetModeReg(0U);
            pDramParam[DRAM_TRAIN_PARAM_OFST_MR14] = ((0x010e0000U) | (Delay & 0xffU));
            pDramParam[DRAM_TRAIN_PARAM_OFST_STATE] |= DRAM_TRAIN_VALID_WRITE;
        }

        pDramParam[DRAM_TRAIN_PARAM_OFST_STATE] &= ~(DRAM_TRAIN_OPMODE_MASK);   // Clear opcode field to be updated below
        if (DRAM_TRAIN_OPMODE_AUTO_READ_DONE == OpMode) {
            pDramParam[DRAM_TRAIN_PARAM_OFST_FLAG] = 0x0U;
            pDramParam[DRAM_TRAIN_PARAM_OFST_STATE] |= DRAM_TRAIN_OPMODE_AUTO_WRITE;
        } else if ((DRAM_TRAIN_OPMODE_AUTO_WRITE_DONE == OpMode) || (DRAM_TRAIN_OPMODE_READ_DONE == OpMode) ||
                   (DRAM_TRAIN_OPMODE_WRITE_DONE == OpMode) || (OpMode == DRAM_TRAIN_OPMODE_BOTH_RW_DONE)) {
            pDramParam[DRAM_TRAIN_PARAM_OFST_FLAG] = MAGIC_CODE_DRAM_TRAIN_COMPLETION;
            pDramParam[DRAM_TRAIN_PARAM_OFST_STATE] |= OpMode;
        } else {
            pDramParam[DRAM_TRAIN_PARAM_OFST_FLAG] = 0x0U;
            pDramParam[DRAM_TRAIN_PARAM_OFST_STATE] |= OpMode;
        }

        (void)AmbaRTSL_CacheCleanDataPtr(AmbaTmpBuffer, 4096U);

        /* Write trained data back */
        (void)BLD_WriteDdrcData(&AmbaTmpBuffer[0U]);

        BLD_CmdShowTrainingResult(OpMode, 0);
    }
}

/* also called from BL2 */
void BLD_CmdDramReadTrain(void)
{
    UINT64 Addr = AMBA_SCRATCH_PAD_BASE_ADDR;
    UINT64 Start, End;
    const UINT8 *pDramCode;
    UINT8 *pFifoCode, *pParam;
    UINT32 i;
    void (*FuncDoDramTrainSeq)(void);
#if defined(CONFIG_ENABLE_SPINOR_BOOT) && !defined(CONFIG_ATF_HAVE_BL2)
    AmbaRTSL_SpiNORDeviceReset();
#endif

    FuncDoDramTrainSeq = ddrc_train_read_start;
    AmbaMisra_TypeCast64(&Start, &FuncDoDramTrainSeq);
    FuncDoDramTrainSeq = ddrc_training_seq_read_end;
    AmbaMisra_TypeCast64(&End, &FuncDoDramTrainSeq);

    AmbaMisra_TypeCast64(&pFifoCode, &Addr);
    FuncDoDramTrainSeq = ddrc_train_read_start;
    AmbaMisra_TypeCast64(&pDramCode, &FuncDoDramTrainSeq);
    for (i = 0U; i < (End - Start); i++) {
        pFifoCode[i] = pDramCode[i];
    }

    /* Copy training parameters */
    Addr = AHB_SCRATCH_PAD_PARAM_SPACE;
    AmbaMisra_TypeCast64(&pParam, &Addr);
    pParam[0] = DRAM_lpddr4DqCaVrefOffsetParam;
    pParam[1] = (DRAM_lpddr4DqReadOffsetParam >> 0)  & 0xffU;
    pParam[2] = (DRAM_lpddr4DqReadOffsetParam >> 8)  & 0xffU;
    pParam[3] = (DRAM_lpddr4DqReadOffsetParam >> 16) & 0xffU;
    pParam[4] = (DRAM_lpddr4DqReadOffsetParam >> 24) & 0xffU;
    pParam[5] = (DRAM_lpddr4DllOffsetParam >> 0)  & 0xffU;
    pParam[6] = (DRAM_lpddr4DllOffsetParam >> 8)  & 0xffU;
    pParam[7] = (DRAM_lpddr4DllOffsetParam >> 16) & 0xffU;
    pParam[8] = (DRAM_lpddr4DllOffsetParam >> 24) & 0xffU;

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

/* also called from BL2 */
void BLD_CmdDramWriteTrain(void)
{
    UINT64 Addr = AMBA_SCRATCH_PAD_BASE_ADDR;
    UINT64 Start, End;
    const UINT8 *pDramCode;
    UINT8 *pFifoCode, *pParam;
    UINT32 i;
    void (*FuncDoDramTrainSeq)(void);

#if defined(CONFIG_ENABLE_SPINOR_BOOT) && !defined(CONFIG_ATF_HAVE_BL2)
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

    /* Copy training parameters */
    Addr = AHB_SCRATCH_PAD_PARAM_SPACE;
    AmbaMisra_TypeCast64(&pParam, &Addr);
    pParam[0] = DRAM_lpddr4Mr14OffsetParam & 0xffU;
    pParam[1] = (DRAM_lpddr4DqWriteOffsetParam >> 0)  & 0xffU;
    pParam[2] = (DRAM_lpddr4DqWriteOffsetParam >> 8)  & 0xffU;
    pParam[3] = (DRAM_lpddr4DqWriteOffsetParam >> 16) & 0xffU;
    pParam[4] = (DRAM_lpddr4DqWriteOffsetParam >> 24) & 0xffU;

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

/* also called from BL2 */
void BLD_CmdDramCaTrain(void)
{
    UINT64 Addr = AMBA_SCRATCH_PAD_BASE_ADDR;
    UINT64 Start, End;
    const UINT8 *pDramCode;
    UINT8 *pFifoCode;
    UINT32 i;
    void (*FuncDoDramTrainSeq)(void);
#if defined(CONFIG_ENABLE_SPINOR_BOOT) && !defined(CONFIG_ATF_HAVE_BL2)
    AmbaRTSL_SpiNORDeviceReset();
#endif

    FuncDoDramTrainSeq = ddrc_train_ca_start;
    AmbaMisra_TypeCast64(&Start, &FuncDoDramTrainSeq);
    FuncDoDramTrainSeq = ddrc_training_seq_ca_end;
    AmbaMisra_TypeCast64(&End, &FuncDoDramTrainSeq);

    AmbaMisra_TypeCast64(&pFifoCode, &Addr);
    FuncDoDramTrainSeq = ddrc_train_ca_start;
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
        if (0x0U == (AmbaCSL_DdrcGetTrainScratchPad(0U) & DRAM_TRAIN_PROC_RESULT_MASK)) {
            BLD_CmdStoreTrainingResult(OpMode);
            BLD_ShellReboot();
        } else {
            BldShell_PrintStr("Last dram training failed.\r\n");
        }
    }

    if (OpMode == DRAM_TRAIN_OPMODE_READ) {
        BldShell_PrintStr("Start Dram Training Read.\r\n");
        AmbaCSL_DdrcSetTrainScratchPad(0U, (UINT32)DRAM_TRAIN_OPMODE_READ_DONE << DRAM_TRAIN_PROC_OPMODE_OFST);
        BLD_CmdDramReadTrain();
    } else if (OpMode == DRAM_TRAIN_OPMODE_WRITE) {
        BldShell_PrintStr("Start Dram Training Write.\r\n");
        AmbaCSL_DdrcSetTrainScratchPad(0U, (UINT32)DRAM_TRAIN_OPMODE_WRITE_DONE << DRAM_TRAIN_PROC_OPMODE_OFST);
        BLD_CmdDramWriteTrain();
    } else if (OpMode == DRAM_TRAIN_OPMODE_AUTO_READ) {
        BldShell_PrintStr("Start Dram Training Read.\r\n");
        AmbaCSL_DdrcSetTrainScratchPad(0U, (UINT32)DRAM_TRAIN_OPMODE_AUTO_READ_DONE << DRAM_TRAIN_PROC_OPMODE_OFST);
        BLD_CmdStoreTrainingResult(DRAM_TRAIN_OPMODE_AUTO_READ);
        BLD_CmdDramReadTrain();
    } else if (OpMode == DRAM_TRAIN_OPMODE_AUTO_WRITE) {
        BldShell_PrintStr("Start Dram Training Write.\r\n");
        AmbaCSL_DdrcSetTrainScratchPad(0U, (UINT32)DRAM_TRAIN_OPMODE_AUTO_WRITE_DONE << DRAM_TRAIN_PROC_OPMODE_OFST);
        BLD_CmdStoreTrainingResult((UINT32)DRAM_TRAIN_OPMODE_AUTO_WRITE);
        BLD_CmdDramWriteTrain();
    } else if (OpMode == DRAM_TRAIN_OPMODE_AUTO_CA) {
        BldShell_PrintStr("Start Dram Training CA.\r\n");
        AmbaCSL_DdrcSetTrainScratchPad(0U, (UINT32)DRAM_TRAIN_OPMODE_AUTO_CA_DONE << DRAM_TRAIN_PROC_OPMODE_OFST);
        BLD_CmdDramCaTrain();
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
    } else if (0x0 == IO_UtilityStringCompare("erase", &BldCmdLine[9], (SIZE_t)0x5)) {
        UINT32 i;

        for (i = 0; i < sizeof(AmbaTmpBuffer); i++) {
#ifdef CONFIG_ENABLE_EMMC_BOOT
            AmbaTmpBuffer[i] = 0U;
#else
            AmbaTmpBuffer[i] = 0xFFU;
#endif
        }
        (void)BLD_WriteDdrcData(&AmbaTmpBuffer[0U]);
    } else {
        BldShell_PrintStr("Error command\r\n");
        BldShell_PrintStr("Please keyin \"training read\"  for read training\r\n");
        BldShell_PrintStr("Please keyin \"training write\" for write training\r\n");
    }
}

static void BLD_UpdateDelay(UINT32 IsRead, UINT32 DdrcId, UINT32 RegValue)
{
#define DEBUG_LOG   0
    UINT32 RegValueOri, RegValueNew = RegValue;
    UINT32 DqDlySteps[4], MaxDqDlyStep = 0, DqDlyValOri[4], DqDlyValNew[4], DqDlyValTarget[4];
    INT32 DqDlyDiff[4];
    UINT32 i;
#if DEBUG_LOG
    char IntStr[16];
#endif

    if (IsRead) {
        /* DQ_Read_Delay */
        RegValueOri = AmbaCSL_DdrcGetDqReadDly(DdrcId);
    } else {
        /* DQ_Write_Delay */
        RegValueOri = AmbaCSL_DdrcGetDqWriteDly(DdrcId);
    }

#if DEBUG_LOG
    BldShell_PrintStrInt("Before Target: ", RegValueOri);
#endif
    // Store each byte delay and find max steps
    for (i = 0; i < 4; i++) {
        if (IsRead) {
            DqDlyValOri[i] = (RegValueOri >> ((i*5) + 1)) & 0xf;
            DqDlyValNew[i] = (RegValueNew >> ((i*5) + 1)) & 0xf;
        } else {
            DqDlyValOri[i] = (RegValueOri >> (i*7)) & 0x7f;
            DqDlyValNew[i] = (RegValueNew >> (i*7)) & 0x7f;
        }

        if (DqDlyValNew[i] >= DqDlyValOri[i]) {
            DqDlySteps[i] = DqDlyValNew[i] - DqDlyValOri[i];
            DqDlyDiff[i] = 1;
        } else {
            DqDlySteps[i] = DqDlyValOri[i] - DqDlyValNew[i];
            DqDlyDiff[i] = -1;
        }

        if (DqDlySteps[i] > MaxDqDlyStep) {
            MaxDqDlyStep = DqDlySteps[i];
        }
        // store target value from original value
        DqDlyValTarget[i] = DqDlyValOri[i];
#if DEBUG_LOG
        BldShell_PrintStr("DqDly: ");
        (void)IO_UtilityUInt32ToStr(IntStr, 16U, DqDlyValOri[i], 16U);
        BldShell_PrintStr(IntStr);
        BldShell_PrintStr(" -> ");
        (void)IO_UtilityUInt32ToStr(IntStr, 16U, DqDlyValNew[i], 16U);
        BldShell_PrintStr(IntStr);
        BldShell_PrintStrInt(" Steps ", DqDlySteps[i]);
#endif
    }

#if DEBUG_LOG
    BldShell_PrintStrInt("MaxStep: ", MaxDqDlyStep);
#endif
    // Apply training value to each byte.
    while (MaxDqDlyStep > 0) {
        for (i = 0; i < 4; i++) {
            if (DqDlySteps[i] > 0) {
                DqDlyValTarget[i] += DqDlyDiff[i];
                DqDlySteps[i]--;
            }
        }

        if (IsRead) {
            RegValueNew = (0x8421) | (DqDlyValTarget[3] << 16U) | (DqDlyValTarget[2] << 11U) | (DqDlyValTarget[1] << 6U) | (DqDlyValTarget[0] << 1U);
            AmbaCSL_DdrcSetDqReadDly(DdrcId, RegValueNew);
        } else {
            RegValueNew = (0xDU << 28U) | (DqDlyValTarget[3] << 21U) | (DqDlyValTarget[2] << 14U) | (DqDlyValTarget[1] << 7U) | (DqDlyValTarget[0]);
            AmbaCSL_DdrcSetDqWriteDly(DdrcId, RegValueNew);
            RegValueNew = (~(0x10000000U) & RegValueNew);
            AmbaCSL_DdrcSetDqWriteDly(DdrcId, RegValueNew);
        }
        MaxDqDlyStep--;

#if DEBUG_LOG
        BldShell_PrintStr("TargetVal byte: 0x");
        (void)IO_UtilityUInt32ToStr(IntStr, 16U, DqDlyValTarget[0], 16U);
        BldShell_PrintStr(IntStr);
        BldShell_PrintStr(" 0x");
        (void)IO_UtilityUInt32ToStr(IntStr, 16U, DqDlyValTarget[1], 16U);
        BldShell_PrintStr(IntStr);
        BldShell_PrintStr(" 0x");
        (void)IO_UtilityUInt32ToStr(IntStr, 16U, DqDlyValTarget[2], 16U);
        BldShell_PrintStr(IntStr);
        BldShell_PrintStr(" 0x");
        (void)IO_UtilityUInt32ToStr(IntStr, 16U, DqDlyValTarget[3], 16U);
        BldShell_PrintStr(IntStr);
        BldShell_PrintStrInt(", Val: 0x", RegValueNew);
#endif
    }
}

static void BLD_UpdateDdrc(void)
{
    static UINT8 DdrPhaseSetting[] = {   /* for DLL phase shift  */
        0x3F, 0x3E, 0x3D, 0x3C, 0x3B, 0x3A, 0x39, 0x38, 0x37, 0x36, 0x35, 0x34, 0x33, 0x32, 0x31, 0x30,
        0x2F, 0x2E, 0x2D, 0x2C, 0x2B, 0x2A, 0x29, 0x28, 0x27, 0x26, 0x25, 0x24, 0x23, 0x22, 0x21, 0x20,
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F
    };
    UINT32 i;
    UINT32 *pBuffer = NULL;
    UINT32 RegValueOri, RegValueNew;
    UINT32 DllValue[4], DllPhaseIdx = 0U;
    UINT8 *pTmp;
    UINT32 CurState = 0U, Valid = 0U;

    CurState = (AmbaCSL_DdrcGetTrainScratchPad(0U) >> DRAM_TRAIN_PROC_OPMODE_OFST);
    if (CurState == DRAM_TRAIN_OPMODE_READ_DONE) {
        BLD_CmdStoreTrainingResult(DRAM_TRAIN_OPMODE_READ_DONE);
    } else if ((CurState == DRAM_TRAIN_OPMODE_WRITE_DONE)) {
        BLD_CmdStoreTrainingResult(DRAM_TRAIN_OPMODE_WRITE_DONE);
    }

    if (AmbaTmpBuffer[0] != 0xffU) {
        pTmp = &AmbaTmpBuffer[0];
        AmbaMisra_TypeCast64(&pBuffer,&pTmp);

        Valid = pBuffer[DRAM_TRAIN_PARAM_OFST_STATE] & DRAM_TRAIN_VALID_MASK;
        if ((Valid & DRAM_TRAIN_VALID_WRITE) != 0U) {
            /* DQ_Write_Delay */
            if (0xFFFFFFFFU != pBuffer[DRAM_TRAIN_PARAM_OFST_DQ_WDLY]) {
                RegValueNew = pBuffer[DRAM_TRAIN_PARAM_OFST_DQ_WDLY];
                BLD_UpdateDelay(0U, 0U, RegValueNew);
            }

            /* MR14 value */
            if (0xFFFFFFFFU != pBuffer[DRAM_TRAIN_PARAM_OFST_MR14]) {
                RegValueNew = pBuffer[DRAM_TRAIN_PARAM_OFST_MR14] & 0xffU;
                AmbaCSL_DdrcSetModeReg(0U, 0x000e0000);
                while (0x0U != (AmbaCSL_DdrcGetModeReg(0U) & 0x80000000U)) {
                    ;
                }
                RegValueOri = AmbaCSL_DdrcGetModeReg(0U) & 0xffU;

                if (RegValueNew > RegValueOri) {
                    while(RegValueOri != RegValueNew) {
                        AmbaCSL_DdrcSetModeReg(0U, (0x010e0000U) | (RegValueOri));
                        while (AmbaCSL_DdrcGetModeReg(0U) & 0x80000000U) { ; }
                        RegValueOri++;
                    }
                } else {
                    while(RegValueOri != RegValueNew) {
                        AmbaCSL_DdrcSetModeReg(0U, (0x010e0000U) | (RegValueOri));
                        while (AmbaCSL_DdrcGetModeReg(0U) & 0x80000000U) { ; }
                        RegValueOri--;
                    }
                }
                AmbaCSL_DdrcSetModeReg(0U, (0x010e0000U) | (RegValueNew));
                while (AmbaCSL_DdrcGetModeReg(0U) & 0x80000000U) { ; }
            }

            /* MR12 value */
            if (0xFFFFFFFFU != pBuffer[DRAM_TRAIN_PARAM_OFST_MR12]) {
                RegValueNew = pBuffer[DRAM_TRAIN_PARAM_OFST_MR12] & 0xffU;
                if (RegValueNew & 0x40U) {
                    RegValueNew = (RegValueNew & 0x3fU) - 21U;
                    RegValueNew = RegValueNew + 51U;
                }

                AmbaCSL_DdrcSetModeReg(0U, 0x000c0000);
                while (0x0U != (AmbaCSL_DdrcGetModeReg(0U) & 0x80000000U)) {
                    ;
                }
                RegValueOri = AmbaCSL_DdrcGetModeReg(0U) & 0xffU;
                if (RegValueOri & 0x40U) {
                    RegValueOri = (RegValueOri & 0x3fU) - 21U;
                    RegValueOri = RegValueOri + 51U;
                }
                if (RegValueNew > RegValueOri) {
                    while(RegValueOri != RegValueNew) {
                        if (RegValueOri > 50U) {
                            i = RegValueOri - 51U + 21U;
                            i = i | 0x40U;
                        } else {
                            i = RegValueOri;
                        }

                        AmbaCSL_DdrcSetModeReg(0U, (0x010c0000U) | (i));
                        while (AmbaCSL_DdrcGetModeReg(0U) & 0x80000000U) { ; }
                        RegValueOri++;
                    }
                } else {
                    while(RegValueOri != RegValueNew) {
                        if (RegValueOri > 50U) {
                            i = RegValueOri - 51U + 21U;
                            i = i | 0x40U;
                        } else {
                            i = RegValueOri;
                        }

                        AmbaCSL_DdrcSetModeReg(0U, (0x010c0000U) | (i));
                        while (AmbaCSL_DdrcGetModeReg(0U) & 0x80000000U) { ; }
                        RegValueOri--;
                    }
                }
                AmbaCSL_DdrcSetModeReg(0, pBuffer[DRAM_TRAIN_PARAM_OFST_MR12]);
                while (AmbaCSL_DdrcGetModeReg(0U) & 0x80000000U) { ; }
            }
        }

        if ((Valid & DRAM_TRAIN_VALID_READ) != 0U) {
            /* DQ_Read_Delay */
            if (0xFFFFFFFFU != pBuffer[DRAM_TRAIN_PARAM_OFST_DQSDLY]) {
                RegValueNew = (pBuffer[DRAM_TRAIN_PARAM_OFST_DQSDLY]);
                BLD_UpdateDelay(1U, 0U, RegValueNew);
            }

            /* read Vref */
            if (0xFFFFFFFFU != pBuffer[DRAM_TRAIN_PARAM_OFST_DQCA]) {
                RegValueOri = AmbaCSL_DdrcGetDqVref(0U) & 0x7fU;
                RegValueNew = pBuffer[DRAM_TRAIN_PARAM_OFST_DQCA] & 0x7fU;
                if (RegValueNew > RegValueOri) {
                    i = RegValueNew - RegValueOri;
                    for (; i != 0; i--) {
                        RegValueOri = RegValueOri + 1U;
                        RegValueNew = (RegValueOri << 21U) | (RegValueOri << 14U) | (RegValueOri << 7U) | (RegValueOri);
                        AmbaCSL_DdrcSetDqVref(0U, RegValueNew);
                    }
                } else {
                    i = RegValueOri - RegValueNew;
                    for (; i != 0; i--) {
                        RegValueOri = RegValueOri - 1U;
                        RegValueNew = (RegValueOri << 21U) | (RegValueOri << 14U) | (RegValueOri << 7U) | (RegValueOri);
                        AmbaCSL_DdrcSetDqVref(0U, RegValueNew);
                    }
                }
            }
            /* DLL */
            if (0xFFFFFFFFU != pBuffer[DRAM_TRAIN_PARAM_OFST_DLL0]) {
                DllValue[0U] = AmbaCSL_DdrcGetDll0Setting(0U);
                DllValue[1U] = AmbaCSL_DdrcGetDll1Setting(0U);
                DllValue[2U] = AmbaCSL_DdrcGetDll2Setting(0U);
                DllValue[3U] = AmbaCSL_DdrcGetDll3Setting(0U);

                for (i = 0U; i < 4U; i++) {
                    RegValueNew = pBuffer[DRAM_TRAIN_PARAM_OFST_DLL0 + i] & 0xffU;
                    if ((RegValueNew & 0x20U) != 0x0U) {
                        RegValueNew = DdrPhaseSetting[0U] - RegValueNew;
                    } else {
                        RegValueNew = 0x20U + RegValueNew;
                    }

                    if ((DllValue[i] & 0x20) != 0x0U) {
                        DllPhaseIdx = DdrPhaseSetting[0U] - (DllValue[i] & 0xffU);
                    } else {
                        DllPhaseIdx = 0x20U + (DllValue[i] & 0xffU);
                    }

                    DllValue[i] = DllValue[i] & 0xffffff00U;
                    if (RegValueNew > DllPhaseIdx) {
                        while (DllPhaseIdx != RegValueNew) {
                            AmbaCSL_DdrcSetDll0Setting(0U, DllValue[i] | (UINT32)DdrPhaseSetting[DllPhaseIdx]);
                            DllPhaseIdx++;
                        }
                    } else {
                        while (DllPhaseIdx != RegValueNew) {
                            AmbaCSL_DdrcSetDll0Setting(0U, DllValue[i] | (UINT32)DdrPhaseSetting[DllPhaseIdx]);
                            DllPhaseIdx--;
                        }
                    }
                    DllValue[i] = (DllValue[i] | (UINT32)DdrPhaseSetting[RegValueNew]);
                }
                AmbaCSL_DdrcSetDll0Setting(0U, DllValue[0U]);
                AmbaCSL_DdrcSetDll1Setting(0U, DllValue[1U]);
                AmbaCSL_DdrcSetDll2Setting(0U, DllValue[2U]);
                AmbaCSL_DdrcSetDll3Setting(0U, DllValue[3U]);
            }
        }
    }
}
void BLD_CheckTrainingResult(void)
{
    UINT32 DramTrainingState;
    UINT32 RetVal = OK;
    UINT32 *pBuffer;
    UINT8 *pTmp;

    RetVal = BLD_LoadDdrcData(&AmbaTmpBuffer[0]);
    if (RetVal == OK) {
        pTmp = &AmbaTmpBuffer[0];
        AmbaMisra_TypeCast64(&pBuffer,&pTmp);
        DramTrainingState = pBuffer[DRAM_TRAIN_PARAM_OFST_FLAG];
    }

    (void)AmbaRTSL_CacheFlushDataPtr(&DramTrainingState, sizeof(UINT32));
    if ( MAGIC_CODE_DRAM_TRAIN_COMPLETION != DramTrainingState ) {
        if (MAGIC_CODE_DRAM_TRAIN_FAILURE == DramTrainingState) {
            BldShell_PrintStr("\r\nPlease try dram training manually\r\n");
        } else {
            if (0x0U != AmbaCSL_DdrcGetTrainScratchPad(0U)) {
                DramTrainingState = AmbaCSL_DdrcGetTrainScratchPad(0U);
                BldShell_PrintStrInt("SPad=0x", DramTrainingState);
                DramTrainingState = (DramTrainingState >> DRAM_TRAIN_PROC_OPMODE_OFST);
            } else {
#if defined(CONFIG_BST_DRAM_TRAINING)
                BLD_CmdShowTrainingResult(DRAM_TRAIN_OPMODE_BOTH_RW_DONE, 1);
#endif
                DramTrainingState = pBuffer[DRAM_TRAIN_PARAM_OFST_STATE];
                BldShell_PrintStrInt("State=0x", DramTrainingState);
                DramTrainingState = (DramTrainingState & 0xffU);
            }

#ifdef CONFIG_ENABLE_EMMC_BOOT
            if ( DramTrainingState == 0x0U ) {
#else
            if ( DramTrainingState == 0xffU ) {
#endif
#ifdef CONFIG_AUTO_DRAM_TRAINING
                BLD_ShellDramTraining(0U, DRAM_TRAIN_OPMODE_AUTO_READ);
#endif
            } else if ( DramTrainingState == DRAM_TRAIN_OPMODE_AUTO_CA_DONE ) {
                BLD_ShellDramTraining(0U, DRAM_TRAIN_OPMODE_AUTO_READ);
            } else if ( DramTrainingState == DRAM_TRAIN_OPMODE_AUTO_READ_DONE ) {
                BLD_CmdStoreTrainingResult(DRAM_TRAIN_OPMODE_AUTO_READ_DONE);
                BLD_ShellReboot();
            } else if ( DramTrainingState == DRAM_TRAIN_OPMODE_AUTO_WRITE) {
                BLD_ShellDramTraining(0U, DRAM_TRAIN_OPMODE_AUTO_WRITE);
            } else if ( DramTrainingState == DRAM_TRAIN_OPMODE_AUTO_WRITE_DONE ) {
                BLD_ShellDramTraining(1U, DRAM_TRAIN_OPMODE_WRITE_DONE);
            } else if ( DramTrainingState == DRAM_TRAIN_OPMODE_READ_DONE ) {
                BLD_ShellDramTraining(1U, DRAM_TRAIN_OPMODE_READ_DONE);
            } else if ( DramTrainingState == DRAM_TRAIN_OPMODE_WRITE_DONE ) {
                BLD_ShellDramTraining(1U, DRAM_TRAIN_OPMODE_WRITE_DONE);
            } else if ( DramTrainingState == DRAM_TRAIN_OPMODE_BOTH_RW_DONE ) {
                BLD_ShellDramTraining(1U, DramTrainingState);
            } else {
                BldShell_PrintStr("\r\nPlease try dram training manually\r\n");
            }
        }
    } else {
#if defined(CONFIG_BST_DRAM_TRAINING)
        BLD_CmdShowTrainingResult(DRAM_TRAIN_OPMODE_BOTH_RW_DONE, 1);
#endif
        BLD_UpdateDdrc();
        BldShell_PrintStrInt("\r\nDram has been trained ", pBuffer[DRAM_TRAIN_PARAM_OFST_STATE]);
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
    RetVal = BLD_DetectEscapeKey(1000U);
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
                }
#ifdef CONFIG_DRAM_TYPE_LPDDR4
                else if (0x0 == IO_UtilityStringCompare("training", BldCmdLine, 0x8)) {
                    if (PtbStatus != BLD_ERR_INFO) {
                        BLD_CmdDramTrain();
                    } else {
                        BldShell_PrintStr("Partition info error\r\n");
                    }
                }
#endif
                else {
                    BldShell_PrintStr("Error: unknown command.\r\n");
                }
            }
        }
    }
    AmbaMisra_TouchUnused(&RetVal);
}

void BLD_SetOtpVppTiming(void)
{
    // @ 2021/Aug/13
    // H32 doesn't have internal OTP VPP.
    // For external OTP VPP, we need to set larger setup time
    // the values below work for BUB.
    // hold time  = 0x00F0 (default value)
    // setup time = 0x1000 (~170us, typical value of BUB external VPP IC is 75~100us)
    // the unit is 1/24Mhz
    pAmbaScratchpadS_Reg->OtpCtrl2 = 0x00F01000;
}

/*============== APIs used for BL2 ==============*/

void *BL2_getAmbaTmpBuffer(void)
{
    return AmbaTmpBuffer;
}

void BL2_UpdateDdrc(void)
{
#ifdef CONFIG_DRAM_TYPE_LPDDR4
    BLD_UpdateDdrc();
#endif
}

