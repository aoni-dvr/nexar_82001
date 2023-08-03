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
#include "AmbaCortexA76.h"

#include "AmbaRTSL_Cache.h"
#include "AmbaRTSL_CPU.h"
#include "AmbaRTSL_MMU.h"
#include "AmbaRTSL_UART.h"

#include "AmbaRTSL_NAND.h"
#include "AmbaRTSL_PLL.h"
#if defined(CONFIG_ENABLE_NAND_BOOT) || defined(CONFIG_ENABLE_SPINAND_BOOT)
#include "AmbaRTSL_NAND.h"
#define AmbaRTSL_NvmSetActivePtbNo AmbaRTSL_NandSetActivePtbNo
#define AmbaRTSL_NvmReadUserPTB    AmbaRTSL_NandReadUserPTB
#define AmbaRTSL_NvmWriteUserPTB   AmbaRTSL_NandWriteUserPTB
#elif defined(CONFIG_ENABLE_SPINOR_BOOT)
#include "AmbaRTSL_SpiNOR.h"
#define AmbaRTSL_NvmSetActivePtbNo AmbaRTSL_NorSpiSetActivePtbNo
#define AmbaRTSL_NvmReadUserPTB    AmbaRTSL_NorSpiReadUserPTB
#define AmbaRTSL_NvmWriteUserPTB   AmbaRTSL_NorSpiWriteUserPTB
#elif defined(CONFIG_ENABLE_EMMC_BOOT)
#include "AmbaRTSL_PLL.h"
#include "AmbaRTSL_SD.h"
#define AmbaRTSL_NvmSetActivePtbNo AmbaRTSL_EmmcSetActivePtbNo
#define AmbaRTSL_NvmReadUserPTB    AmbaRTSL_EmmcReadUserPTB
#define AmbaRTSL_NvmWriteUserPTB   AmbaRTSL_EmmcWriteUserPTB
#else
#pragma message ("[AmbaBLD_Shell.c] ERROR !! UNKNOWN BOOT DEVICE !!")
#endif


#include "AmbaCSL_RCT.h"
#include "AmbaCSL_DDRC.h"
#include "AmbaCSL_FIO.h"
#include "AmbaRTSL_DRAMC.h"
#if defined(CONFIG_TEST_DRAM_SAVE_RESTORE)
#include "AmbaSYS.h"
#include "AmbaSYS_Ctrl.h"
#include "AmbaSpiNOR_Ctrl.h"
#include "AmbaRTSL_GIC.h"
#include "AmbaRTSL_PWC.h"
#endif
#if defined(CONFIG_ATF_HAVE_BL2)
#include "AmbaCSL_Scratchpad.h"
#endif

static char SysPartName[AMBA_NUM_SYS_PARTITION][10U];
static UINT8 SysPartNameLen[AMBA_NUM_SYS_PARTITION];
static char UserPartName[AMBA_NUM_USER_PARTITION][10U];
static UINT8 UserPartNameLen[AMBA_NUM_USER_PARTITION];

#if defined(CONFIG_TEST_DRAM_SAVE_RESTORE)
#define DO_DBG_PRINT                0U
#define MAX_RESET_COUNT             100U
#define PEEP_DRAM_TRAINING_PARAM    0U
#endif
#define PRINT_BUF_SIZE  (1024U)
static char BldCmdLine[128U] GNU_SECTION_NOZEROINIT;

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
    UserPartNameLen[AMBA_USER_PARTITION_RESERVED0] = 4U;
    UserPartNameLen[AMBA_USER_PARTITION_RESERVED1] = 4U;
    UserPartNameLen[AMBA_USER_PARTITION_RESERVED2] = 4U;
    UserPartNameLen[AMBA_USER_PARTITION_RESERVED3] = 4U;
    UserPartNameLen[AMBA_USER_PARTITION_RESERVED4] = 4U;

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
    AmbaMisra_TypeCast(&pPrintBuf, &pString);
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

/*
static void BldShell_PrintStrInt(const char *pFmt, UINT32 value)
{
    char IntStr[16];

    BldShell_PrintStr(pFmt);
    (void)IO_UtilityUInt32ToStr(IntStr, 16U, value, 16U);
    BldShell_PrintStr(IntStr);
    BldShell_PrintStr("\r\n");
}
*/

UINT32 BLD_Read32(ULONG address)
{
    ULONG addr;
    UINT32 *pData;

    addr = address;
    AmbaMisra_TypeCast(&pData, &addr);
    return *pData;
}

void BLD_Write32(ULONG address, UINT32 value)
{
    ULONG addr;
    UINT32 *pData;

    addr = address;
    AmbaMisra_TypeCast(&pData, &addr);
    *pData = value;
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

    AmbaMisra_TypeCast(&pWorkUINT8, &pSysPtb);
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
                AmbaMisra_TypeCast(&pString, &pWorkUINT8);
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

        AmbaMisra_TypeCast(&pWorkUINT8, &pUserPtb);
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
                    AmbaMisra_TypeCast(&pString, &pWorkUINT8);
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

    (void)AmbaRTSL_NvmReadUserPTB(NULL, PtbNo);
    BldShell_PrintStr("\r\n\r\nPTB No: ");
    (void)IO_UtilityUInt32ToStr(IntStr, 16U, pUserPtb->PTBNumber, 16U);
    BldShell_PrintStr(IntStr);
    BldShell_PrintStr("\r\n");
    AmbaMisra_TypeCast(&pWorkUINT8, &pUserPtb);
    Crc32 = IO_UtilityCrc32(pWorkUINT8, AMBA_USER_PTB_CRC32_SIZE);
    if (Crc32 == pUserPtb->CRC32) {
        pPartEntry = pUserPtb->Entry;
        for (i = 0; i < AMBA_NUM_USER_PARTITION; i++) {
            if ((pPartEntry->BlkCount > 0U) && (pPartEntry->BlkCount != 0xFFFFFFFFU)) {
                //AmbaRTSL_UartPrintf("%-16s: attribute: 0x%08x, mem_addr: 0x%08x, ", pPartEntry->PartitionName, pPartEntry->Attribute.Data, pPartEntry->RamLoadAddr);
                //AmbaRTSL_UartPrintf("blk_addr: from %4u to %4u\r\n", pPartEntry->StartBlkAddr, pPartEntry->StartBlkAddr + pPartEntry->BlkCount - 1);
                pWorkUINT8 = pPartEntry->PartitionName;
                AmbaMisra_TypeCast(&pString, &pWorkUINT8);
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

//extern UINT32 DRAM_lpddr4DqCaVrefOffsetParam;
#if defined(CONFIG_DRAM_TYPE_LPDDR5)
extern UINT32 LPDDR5_ModeReg11ParamFsp0_Host0;
extern UINT32 LPDDR5_ModeReg13ParamFsp0_Host0;
extern UINT32 LPDDR5_ModeReg16ParamFsp0_Host0;
extern UINT32 LPDDR5_ModeReg18ParamFsp0_Host0;
extern UINT32 LPDDR5_ModeReg41ParamFsp0_Host0;
#else
extern UINT32 LPDDR4_ModeReg11ParamFsp0_Host0;
extern UINT32 TRAIN_ReadDelayOffset_Host0;
extern UINT32 TRAIN_ReadDelayOffset_Host1;
extern UINT32 TRAIN_WriteDelayOffset_Host0;
extern UINT32 TRAIN_WriteDelayOffset_Host1;
extern UINT8 TRAIN_VrefOffset_Host0;
extern UINT8 TRAIN_VrefOffset_Host1;
extern UINT8 TRAIN_MR14Offset_Host0;
extern UINT8 TRAIN_MR14Offset_Host1;
#endif
extern UINT32 DRAMC_ModeParam;
extern UINT32 DRAM_Config1Param;
extern UINT32 DRAM_Config2Param;

static UINT32 BLD_Check_DRAM_DIE1_exist(void)
{
    // Check bit 26
    return ((DRAM_Config1Param & 0x04000000U) ? 1U : 0U);
}

#if defined(CONFIG_DRAM_TYPE_LPDDR5)
static INT32 wck2dqi_trained[2][2][2] = {0};    // 2 host, 2 die, 2 ch
static void BLD_TimerWait(UINT32 Time)
{
    UINT32 TimeLimit;

    AmbaCSL_RctTimer0Reset();   /* reset RCT Timer */
    AmbaCSL_RctTimer0Enable();

    TimeLimit = Time * (24000000U / 1000U);
    while (AmbaCSL_RctTimer0GetCounter() < TimeLimit);

    AmbaCSL_RctTimer0Reset();   /* reset RCT Timer */
}

static UINT32 BLD_ddrc_mrr(UINT32 ddrc, UINT32 addr, UINT32 did, UINT32 cid)
{
    UINT32 data, temp;

    did = (did == 0x3) ? 0x3 : 1 << did;
    cid = (cid == 0x3) ? 0x3 : 1 << cid;

    temp = (1U << 31) | (cid << 27) | (did << 25) | (addr << 16);

    AmbaCSL_DdrcSetModeReg(ddrc, temp);

    while (1) {
        temp = AmbaCSL_DdrcGetModeReg(ddrc);
        if ((temp & (1U << 31)) == 0)
            break;
    }

    if (cid == 1)
        data = temp & 0xff;
    else if (cid == 2)
        data = (temp & 0xff00) >> 8;
    else
        data = temp & 0xffff;

    return data;
}

static void BLD_ddrc_mrw(UINT32 ddrc, UINT32 addr, UINT32 data, UINT32 did, UINT32 cid)
{
    UINT32 temp;

    did = (did == 0x3) ? 0x3 : 1 << did;
    cid = (cid == 0x3) ? 0x3 : 1 << cid;

    temp = (1U << 31) | (cid << 27) | (did << 25) | (1 << 24) | (addr << 16) | data;

    AmbaCSL_DdrcSetModeReg(ddrc, temp);

    while (1) {
        temp = AmbaCSL_DdrcGetModeReg(ddrc);
        if ((temp & (1U << 31)) == 0)
            break;
    }
}

static void BLD_ddrc_uinst(UINT32 ddrc, UINT32 uinst1, UINT32 uinst2, UINT32 uinst4)
{
    AmbaCSL_DdrcSetUInstruction(ddrc, 1-1, uinst1);
    AmbaCSL_DdrcSetUInstruction(ddrc, 2-1, uinst2);
    AmbaCSL_DdrcSetUInstruction(ddrc, 4-1, uinst4);

    AmbaCSL_DdrcSetUInstruction(ddrc, 5-1, 0x1);

    while (AmbaCSL_DdrcGetUInstruction(ddrc, 5-1) & 0x1);
}

static void ddrc_calc_wck2dqx(void)
{
    UINT32 data_mr35, data_mr36, count;
    INT32 h, d, ranknum;
    UINT64 freq;
    INT32 start_host_id = 0U;
    INT32 end_host_id = 0U;

    if (0x0 != (DRAMC_ModeParam & 0x2U)) {
        start_host_id = 0U;
    } else {
        start_host_id = 1U;
    }

    if (0x0 != (DRAMC_ModeParam & 0x4U)) {
        end_host_id = 1U;
    } else {
        end_host_id = 0U;
    }

    if (BLD_Check_DRAM_DIE1_exist()) {
        ranknum = 2;
    } else {
        ranknum = 1;
    }
    /* fs(femtosecond): fs_wck2dqx = 8192 * 1000000000 / f / count_wck2dqx / 2 */
    freq = AmbaRTSL_PllGetDramClk() / 4; /* CK rather than WCK */
    freq = freq / 1000000;

    for (h = start_host_id; h <= end_host_id; h++) {
        BLD_ddrc_mrw(h, 37, 0xc0, 0x3, 0x3);
        BLD_ddrc_uinst(h, 0xd, 0x170, 0x303);
        BLD_TimerWait(5);
        for (d = 0; d < ranknum; d++) {
            data_mr35 = BLD_ddrc_mrr(h, 35, d, 0x3);
            data_mr36 = BLD_ddrc_mrr(h, 36, d, 0x3);

            /* Channel A */
            count = ((data_mr35 >> 0) & 0xff) | (((data_mr36 >> 0) & 0xff) << 8);
            wck2dqi_trained[h][d][0] = 1000000000ULL * 8192 / freq / count / 2;

            /* Channel B */
            count = ((data_mr35 >> 8) & 0xff) | (((data_mr36 >> 8) & 0xff) << 8);
            wck2dqi_trained[h][d][1] = 1000000000ULL * 8192 / freq / count / 2;
        }
    }
}

static void ddrc_save_wck2dqx(UINT32 *pDramParam)
{
    INT32 h, d, c;

    ddrc_calc_wck2dqx();
    for (h = 0; h < 2; h++) {
        for (d = 0; d < 2; d++) {
            for (c = 0; c < 2; c++) {
                if (pDramParam != NULL) {
                    pDramParam[DRAM_TRAIN_PARAM_OFST_WCK2DQC0D0 + (h*DRAM_TRAIN_PARAM_OFST) + (d*2) + c] = wck2dqi_trained[h][d][c];   // Keep in saved training parameter for next boot
                }
                BLD_Write32(DDRCT_RESULT_WCK2DQX_ADDR(h, d, c), wck2dqi_trained[h][d][c]);    // Keep for booting to App
            }
        }
    }
    /* Set magic code to indicate the WCK2DQX result is valid */
    BLD_Write32(DDRCT_RESULT_WCK2DQX_VALID, DDRCT_RESULT_VALID_MAGIC_CODE);
}
#endif

static UINT8 BstBinBuf[BST_MAX_SIZE] GNU_SECTION_NOZEROINIT;
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
#if defined(CONFIG_ATF_HAVE_BL2)
/*  For CV5x, we use system scratchpad memory as a buffer
 *  to save the DRAM trained parameters temporarily in BL2,
 *  and save to NVM later in BLD.
 */
#if defined(CONFIG_ENABLE_ONE_DDRC_HOST)
#define DDRC_MAX_HOST 1
#else
#define DDRC_MAX_HOST 2
#endif
#define BL2BINBUF_SIZE (DRAM_TRAIN_PARAM_OFST*DDRC_MAX_HOST*4) // Beward to check the backup parameters size
UINT32 BL2BINBUF[BL2BINBUF_SIZE/4] = {0};
#endif
#endif

static void BLD_CmdStoreTrainingResult(UINT32 OpMode)
{
#ifdef CONFIG_ENABLE_SPINOR_BOOT
    const UINT8 *pData = &BstBinBuf[BST_OFST_DRAM_HIGH_FREQ_PARAM + 0x80UL];
#else
    const UINT8 *pData = &BstBinBuf[BST_OFST_DRAM_HIGH_FREQ_PARAM];
#endif
    const AMBA_SYS_PARTITION_TABLE_s *pSysPtb = AmbaBLD_GetSysPartTable();

    UINT32 *pDramParam;
    UINT32 Crc32;
#if (defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)) && defined(CONFIG_ATF_HAVE_BL2)
#ifdef CONFIG_ARM64
    UINT64 temp;
#else
    UINT32 temp;
#endif
    UINT32* BinBuf;
    UINT32 BinBufSize;
#else
    UINT32 Delay = 0;
    UINT32 Hid = 0U;
    UINT32 start_host_id = 0U;
    UINT32 end_host_id = 0U;
    UINT32 *pDramParamHid;

    if (0x0 != (DRAMC_ModeParam & 0x2U)) {
        start_host_id = 0U;
    } else {
        start_host_id = 1U;
    }

    if (0x0 != (DRAMC_ModeParam & 0x4U)) {
        end_host_id = 1U;
    } else {
        end_host_id = 0U;
    }
#endif

    AmbaMisra_TypeCast(&pDramParam, &pData);

    /* Start store training result to flash */
    AmbaMisra_TypeCast(&pData, &pSysPtb);
    Crc32 = IO_UtilityCrc32(pData, sizeof(AMBA_SYS_PARTITION_TABLE_s) - 4U);

    if (Crc32 != pSysPtb->CRC32) {
        BldShell_PrintStr("System partition check fail.\r\n");
    } else {
        BLD_LoadNvmPart(0U, AMBA_SYS_PARTITION_BOOTSTRAP, BstBinBuf);
#if (defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)) && defined(CONFIG_ATF_HAVE_BL2)
        // In CV5x BL2 case, we directly copy the training parameters
#ifdef CONFIG_ARM64
        temp = pAmbaScratchpadNS_Reg->AhbScratchpad[1];
        temp = (temp << 32) | pAmbaScratchpadNS_Reg->AhbScratchpad[0];
#else
        temp = pAmbaScratchpadNS_Reg->AhbScratchpad[0];
#endif
        BinBuf = (UINT32*)temp;
        BinBufSize = BL2BINBUF_SIZE;
        AmbaWrap_memcpy((UINT8*)pDramParam, (UINT8*)BinBuf, BinBufSize);
        pDramParam[DRAM_TRAIN_PARAM_OFST_STATE] = OpMode;   // For OpMode not being unused...
#else
        /* Read BST data to temp buffer */
        pDramParam[DRAM_TRAIN_PARAM_OFST_FLAG] = MAGIC_CODE_DRAM_TRAIN_COMPLETION;
        pDramParam[DRAM_TRAIN_PARAM_OFST_STATE] = OpMode;

        for (Hid = start_host_id; Hid <= end_host_id; Hid++) {
            pDramParamHid = &pDramParam[DRAM_TRAIN_PARAM_OFST*Hid];
#if defined(CONFIG_DRAM_TYPE_LPDDR5)
            pDramParamHid[DRAM_TRAIN_PARAM_OFST_CA_DELAY_COARSE]  = AmbaCSL_DdrcGetCaDlyCoars(Hid);
            pDramParamHid[DRAM_TRAIN_PARAM_OFST_CKE_DELAY_COARSE] = AmbaCSL_DdrcGetCkeDlyCoars(Hid);
            pDramParamHid[DRAM_TRAIN_PARAM_OFST_CK_DELAY]         = AmbaCSL_DdrcGetCkDly(Hid);
#endif  // End of CONFIG_DRAM_TYPE_LPDDR5
            pDramParamHid[DRAM_TRAIN_PARAM_OFST_RDDLY_DIE0]       = AmbaCSL_DdrcGetD0Dll0(Hid);
            // DIE1 only
            if(BLD_Check_DRAM_DIE1_exist() == 1) {
                pDramParamHid[DRAM_TRAIN_PARAM_OFST_RDDLY_DIE1]       = AmbaCSL_DdrcGetD1Dll0(Hid);
            }
            pDramParamHid[DRAM_TRAIN_PARAM_OFST_RDVREF_0]         = AmbaCSL_DdrcGetRdVref0(Hid);
            pDramParamHid[DRAM_TRAIN_PARAM_OFST_RDVREF_1]         = AmbaCSL_DdrcGetRdVref1(Hid);
#if defined(CONFIG_DRAM_TYPE_LPDDR5)
            pDramParamHid[DRAM_TRAIN_PARAM_OFST_WRVREF_0]         = AmbaCSL_DdrcGetWriteVref0(Hid);
            pDramParamHid[DRAM_TRAIN_PARAM_OFST_WRVREF_1]         = AmbaCSL_DdrcGetWriteVref1(Hid);
#endif  // End of CONFIG_DRAM_TYPE_LPDDR5
            pDramParamHid[DRAM_TRAIN_PARAM_OFST_B0_DLY0_D0]       = AmbaCSL_DdrcGetByte0D0Dly(Hid, 0U);
            pDramParamHid[DRAM_TRAIN_PARAM_OFST_B0_DLY1_D0]       = AmbaCSL_DdrcGetByte0D0Dly(Hid, 1U);
            pDramParamHid[DRAM_TRAIN_PARAM_OFST_B0_DLY2_D0]       = AmbaCSL_DdrcGetByte0D0Dly(Hid, 2U);
            pDramParamHid[DRAM_TRAIN_PARAM_OFST_B0_DLY3_D0]       = AmbaCSL_DdrcGetByte0D0Dly(Hid, 3U);
            pDramParamHid[DRAM_TRAIN_PARAM_OFST_B0_DLY4_D0]       = AmbaCSL_DdrcGetByte0D0Dly(Hid, 4U);
            pDramParamHid[DRAM_TRAIN_PARAM_OFST_B0_DLY5_D0]       = AmbaCSL_DdrcGetByte0D0Dly(Hid, 5U);
            pDramParamHid[DRAM_TRAIN_PARAM_OFST_B1_DLY0_D0]       = AmbaCSL_DdrcGetByte1D0Dly(Hid, 0U);
            pDramParamHid[DRAM_TRAIN_PARAM_OFST_B1_DLY1_D0]       = AmbaCSL_DdrcGetByte1D0Dly(Hid, 1U);
            pDramParamHid[DRAM_TRAIN_PARAM_OFST_B1_DLY2_D0]       = AmbaCSL_DdrcGetByte1D0Dly(Hid, 2U);
            pDramParamHid[DRAM_TRAIN_PARAM_OFST_B1_DLY3_D0]       = AmbaCSL_DdrcGetByte1D0Dly(Hid, 3U);
            pDramParamHid[DRAM_TRAIN_PARAM_OFST_B1_DLY4_D0]       = AmbaCSL_DdrcGetByte1D0Dly(Hid, 4U);
            pDramParamHid[DRAM_TRAIN_PARAM_OFST_B1_DLY5_D0]       = AmbaCSL_DdrcGetByte1D0Dly(Hid, 5U);
            pDramParamHid[DRAM_TRAIN_PARAM_OFST_B2_DLY0_D0]       = AmbaCSL_DdrcGetByte2D0Dly(Hid, 0U);
            pDramParamHid[DRAM_TRAIN_PARAM_OFST_B2_DLY1_D0]       = AmbaCSL_DdrcGetByte2D0Dly(Hid, 1U);
            pDramParamHid[DRAM_TRAIN_PARAM_OFST_B2_DLY2_D0]       = AmbaCSL_DdrcGetByte2D0Dly(Hid, 2U);
            pDramParamHid[DRAM_TRAIN_PARAM_OFST_B2_DLY3_D0]       = AmbaCSL_DdrcGetByte2D0Dly(Hid, 3U);
            pDramParamHid[DRAM_TRAIN_PARAM_OFST_B2_DLY4_D0]       = AmbaCSL_DdrcGetByte2D0Dly(Hid, 4U);
            pDramParamHid[DRAM_TRAIN_PARAM_OFST_B2_DLY5_D0]       = AmbaCSL_DdrcGetByte2D0Dly(Hid, 5U);
            pDramParamHid[DRAM_TRAIN_PARAM_OFST_B3_DLY0_D0]       = AmbaCSL_DdrcGetByte3D0Dly(Hid, 0U);
            pDramParamHid[DRAM_TRAIN_PARAM_OFST_B3_DLY1_D0]       = AmbaCSL_DdrcGetByte3D0Dly(Hid, 1U);
            pDramParamHid[DRAM_TRAIN_PARAM_OFST_B3_DLY2_D0]       = AmbaCSL_DdrcGetByte3D0Dly(Hid, 2U);
            pDramParamHid[DRAM_TRAIN_PARAM_OFST_B3_DLY3_D0]       = AmbaCSL_DdrcGetByte3D0Dly(Hid, 3U);
            pDramParamHid[DRAM_TRAIN_PARAM_OFST_B3_DLY4_D0]       = AmbaCSL_DdrcGetByte3D0Dly(Hid, 4U);
            pDramParamHid[DRAM_TRAIN_PARAM_OFST_B3_DLY5_D0]       = AmbaCSL_DdrcGetByte3D0Dly(Hid, 5U);
            // DIE1 only
            if(BLD_Check_DRAM_DIE1_exist() == 1) {
                pDramParamHid[DRAM_TRAIN_PARAM_OFST_B0_DLY0_D1]       = AmbaCSL_DdrcGetByte0D1Dly(Hid, 0U);
                pDramParamHid[DRAM_TRAIN_PARAM_OFST_B0_DLY1_D1]       = AmbaCSL_DdrcGetByte0D1Dly(Hid, 1U);
                pDramParamHid[DRAM_TRAIN_PARAM_OFST_B0_DLY2_D1]       = AmbaCSL_DdrcGetByte0D1Dly(Hid, 2U);
                pDramParamHid[DRAM_TRAIN_PARAM_OFST_B0_DLY3_D1]       = AmbaCSL_DdrcGetByte0D1Dly(Hid, 3U);
                pDramParamHid[DRAM_TRAIN_PARAM_OFST_B0_DLY4_D1]       = AmbaCSL_DdrcGetByte0D1Dly(Hid, 4U);
                pDramParamHid[DRAM_TRAIN_PARAM_OFST_B0_DLY5_D1]       = AmbaCSL_DdrcGetByte0D1Dly(Hid, 5U);
                pDramParamHid[DRAM_TRAIN_PARAM_OFST_B1_DLY0_D1]       = AmbaCSL_DdrcGetByte1D1Dly(Hid, 0U);
                pDramParamHid[DRAM_TRAIN_PARAM_OFST_B1_DLY1_D1]       = AmbaCSL_DdrcGetByte1D1Dly(Hid, 1U);
                pDramParamHid[DRAM_TRAIN_PARAM_OFST_B1_DLY2_D1]       = AmbaCSL_DdrcGetByte1D1Dly(Hid, 2U);
                pDramParamHid[DRAM_TRAIN_PARAM_OFST_B1_DLY3_D1]       = AmbaCSL_DdrcGetByte1D1Dly(Hid, 3U);
                pDramParamHid[DRAM_TRAIN_PARAM_OFST_B1_DLY4_D1]       = AmbaCSL_DdrcGetByte1D1Dly(Hid, 4U);
                pDramParamHid[DRAM_TRAIN_PARAM_OFST_B1_DLY5_D1]       = AmbaCSL_DdrcGetByte1D1Dly(Hid, 5U);
                pDramParamHid[DRAM_TRAIN_PARAM_OFST_B2_DLY0_D1]       = AmbaCSL_DdrcGetByte2D1Dly(Hid, 0U);
                pDramParamHid[DRAM_TRAIN_PARAM_OFST_B2_DLY1_D1]       = AmbaCSL_DdrcGetByte2D1Dly(Hid, 1U);
                pDramParamHid[DRAM_TRAIN_PARAM_OFST_B2_DLY2_D1]       = AmbaCSL_DdrcGetByte2D1Dly(Hid, 2U);
                pDramParamHid[DRAM_TRAIN_PARAM_OFST_B2_DLY3_D1]       = AmbaCSL_DdrcGetByte2D1Dly(Hid, 3U);
                pDramParamHid[DRAM_TRAIN_PARAM_OFST_B2_DLY4_D1]       = AmbaCSL_DdrcGetByte2D1Dly(Hid, 4U);
                pDramParamHid[DRAM_TRAIN_PARAM_OFST_B2_DLY5_D1]       = AmbaCSL_DdrcGetByte2D1Dly(Hid, 5U);
                pDramParamHid[DRAM_TRAIN_PARAM_OFST_B3_DLY0_D1]       = AmbaCSL_DdrcGetByte3D1Dly(Hid, 0U);
                pDramParamHid[DRAM_TRAIN_PARAM_OFST_B3_DLY1_D1]       = AmbaCSL_DdrcGetByte3D1Dly(Hid, 1U);
                pDramParamHid[DRAM_TRAIN_PARAM_OFST_B3_DLY2_D1]       = AmbaCSL_DdrcGetByte3D1Dly(Hid, 2U);
                pDramParamHid[DRAM_TRAIN_PARAM_OFST_B3_DLY3_D1]       = AmbaCSL_DdrcGetByte3D1Dly(Hid, 3U);
                pDramParamHid[DRAM_TRAIN_PARAM_OFST_B3_DLY4_D1]       = AmbaCSL_DdrcGetByte3D1Dly(Hid, 4U);
                pDramParamHid[DRAM_TRAIN_PARAM_OFST_B3_DLY5_D1]       = AmbaCSL_DdrcGetByte3D1Dly(Hid, 5U);
            }

#if defined(CONFIG_DRAM_TYPE_LPDDR5)
            /* MR 12 */
            // Get settings from C0/C1 D0
            AmbaCSL_DdrcSetModeReg(Hid, 0x9a0c0000);
            while (0x0U != (AmbaCSL_DdrcGetModeReg(Hid) & 0x80000000U)) {
                ;
            }
            Delay = AmbaCSL_DdrcGetModeReg(Hid);
            Delay = (Delay & 0xffU);
            pDramParamHid[DRAM_TRAIN_PARAM_OFST_MR12C0D0] = ((0x8b0c0000U) | (Delay & 0xffU));
            Delay = AmbaCSL_DdrcGetModeReg(Hid);
            Delay = ((Delay & 0xff00U) >> 0x8U);
            pDramParamHid[DRAM_TRAIN_PARAM_OFST_MR12C1D0] = ((0x930c0000U) | (Delay & 0xffU));
            // Get settings from C0/C1 D1
            if(BLD_Check_DRAM_DIE1_exist() == 1) {
                AmbaCSL_DdrcSetModeReg(Hid, 0x9c0c0000);
                while (0x0U != (AmbaCSL_DdrcGetModeReg(Hid) & 0x80000000U)) {
                    ;
                }
                Delay = AmbaCSL_DdrcGetModeReg(Hid);
                Delay = (Delay & 0xffU);
                pDramParamHid[DRAM_TRAIN_PARAM_OFST_MR12C0D1] = ((0x8d0c0000U) | (Delay & 0xffU));
                Delay = AmbaCSL_DdrcGetModeReg(Hid);
                Delay = ((Delay & 0xff00U) >> 0x8U);
                pDramParamHid[DRAM_TRAIN_PARAM_OFST_MR12C1D1] = ((0x950c0000U) | (Delay & 0xffU));
            }
#endif  // End of CONFIG_DRAM_TYPE_LPDDR5

            /* MR 14 */
            // Get settings from C0/C1 D0
            AmbaCSL_DdrcSetModeReg(Hid, 0x9a0e0000);
            while (0x0U != (AmbaCSL_DdrcGetModeReg(Hid) & 0x80000000U)) {
                ;
            }
            Delay = AmbaCSL_DdrcGetModeReg(Hid);
            Delay = (Delay & 0xffU);
            pDramParamHid[DRAM_TRAIN_PARAM_OFST_MR14C0D0] = ((0x8b0e0000U) | (Delay & 0xffU));
            Delay = AmbaCSL_DdrcGetModeReg(Hid);
            Delay = ((Delay & 0xff00U) >> 0x8U);
            pDramParamHid[DRAM_TRAIN_PARAM_OFST_MR14C1D0] = ((0x930e0000U) | (Delay & 0xffU));
            // Get settings from C0/C1 D1
            if(BLD_Check_DRAM_DIE1_exist() == 1) {
                AmbaCSL_DdrcSetModeReg(Hid, 0x9c0e0000);
                while (0x0U != (AmbaCSL_DdrcGetModeReg(Hid) & 0x80000000U)) {
                    ;
                }
                Delay = AmbaCSL_DdrcGetModeReg(Hid);
                Delay = (Delay & 0xffU);
                pDramParamHid[DRAM_TRAIN_PARAM_OFST_MR14C0D1] = ((0x8d0e0000U) | (Delay & 0xffU));
                Delay = AmbaCSL_DdrcGetModeReg(Hid);
                Delay = ((Delay & 0xff00U) >> 0x8U);
                pDramParamHid[DRAM_TRAIN_PARAM_OFST_MR14C1D1] = ((0x950e0000U) | (Delay & 0xffU));
            }

#if defined(CONFIG_DRAM_TYPE_LPDDR5)
            /* MR 15 */
            // Get settings from C0/C1 D0
            AmbaCSL_DdrcSetModeReg(Hid, 0x9a0f0000);
            while (0x0U != (AmbaCSL_DdrcGetModeReg(Hid) & 0x80000000U)) {
                ;
            }
            Delay = AmbaCSL_DdrcGetModeReg(Hid);
            Delay = (Delay & 0xffU);
            pDramParamHid[DRAM_TRAIN_PARAM_OFST_MR15C0D0] = ((0x8b0f0000U) | (Delay & 0xffU));
            Delay = AmbaCSL_DdrcGetModeReg(Hid);
            Delay = ((Delay & 0xff00U) >> 0x8U);
            pDramParamHid[DRAM_TRAIN_PARAM_OFST_MR15C1D0] = ((0x930f0000U) | (Delay & 0xffU));
            // Get settings from C0/C1 D1
            if(BLD_Check_DRAM_DIE1_exist() == 1) {
                AmbaCSL_DdrcSetModeReg(Hid, 0x9c0f0000);
                while (0x0U != (AmbaCSL_DdrcGetModeReg(Hid) & 0x80000000U)) {
                    ;
                }
                Delay = AmbaCSL_DdrcGetModeReg(Hid);
                Delay = (Delay & 0xffU);
                pDramParamHid[DRAM_TRAIN_PARAM_OFST_MR15C0D1] = ((0x8d0f0000U) | (Delay & 0xffU));
                Delay = AmbaCSL_DdrcGetModeReg(Hid);
                Delay = ((Delay & 0xff00U) >> 0x8U);
                pDramParamHid[DRAM_TRAIN_PARAM_OFST_MR15C1D1] = ((0x950f0000U) | (Delay & 0xffU));
            }

            /* MR 30 */
            // Get settings from C0 D0
            /* MR30 cannot be read from DDRC (LP5 spec limitation), so this is obsolete code
            AmbaCSL_DdrcSetModeReg(Hid, 0x8a1e0000);
            while (0x0U != (AmbaCSL_DdrcGetModeReg(Hid) & 0x80000000U)) {
                ;
            }
            Delay = AmbaCSL_DdrcGetModeReg(Hid);
            Delay = (Delay & 0xffU);
            pDramParamHid[DRAM_TRAIN_PARAM_OFST_MR30] = ((0x9f1e0000U) | (Delay & 0xffU));
            */

            // Get settings from system scratchpad memory
            if (BLD_Read32(DDRCT_RESULT_MR30_VALID) == DDRCT_RESULT_VALID_MAGIC_CODE) {
                Delay = BLD_Read32(DDRCT_RESULT_MR30_ADDR(Hid, 0, 0));
                pDramParamHid[DRAM_TRAIN_PARAM_OFST_MR30C0D0] = ((0x8b1e0000U) | (Delay & 0xffU));
                Delay = BLD_Read32(DDRCT_RESULT_MR30_ADDR(Hid, 0, 1));
                pDramParamHid[DRAM_TRAIN_PARAM_OFST_MR30C1D0] = ((0x931e0000U) | (Delay & 0xffU));
                if(BLD_Check_DRAM_DIE1_exist() == 1) {
                    Delay = BLD_Read32(DDRCT_RESULT_MR30_ADDR(Hid, 1, 0));
                    pDramParamHid[DRAM_TRAIN_PARAM_OFST_MR30C0D1] = ((0x8d1e0000U) | (Delay & 0xffU));
                    Delay = BLD_Read32(DDRCT_RESULT_MR30_ADDR(Hid, 1, 1));
                    pDramParamHid[DRAM_TRAIN_PARAM_OFST_MR30C1D1] = ((0x951e0000U) | (Delay & 0xffU));
                }
            } else {
                pDramParamHid[DRAM_TRAIN_PARAM_OFST_MR30C1D0] = pDramParamHid[DRAM_TRAIN_PARAM_OFST_MR30C0D0] = 0;
                pDramParamHid[DRAM_TRAIN_PARAM_OFST_MR30C1D1] = pDramParamHid[DRAM_TRAIN_PARAM_OFST_MR30C0D1] = 0;
            }
#endif  // End of CONFIG_DRAM_TYPE_LPDDR5
        }

#if defined(CONFIG_DRAM_TYPE_LPDDR5)
        ddrc_save_wck2dqx(pDramParam);
#endif

#endif  // End of defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)) && defined(CONFIG_ATF_HAVE_BL2)

        (void)AmbaRTSL_CacheCleanDataPtr(BstBinBuf, BST_MAX_SIZE);

        /* Write BST data back */
        BLD_WriteNvmPart(0, (UINT32)AMBA_SYS_PARTITION_BOOTSTRAP, BST_MAX_SIZE, BstBinBuf);
        BldShell_PrintStr("\r\nStore back parameter done.\r\n");
        BLD_LoadNvmPart(0U, AMBA_SYS_PARTITION_BOOTSTRAP, BstBinBuf);
    }
}

static void BLD_CmdDramCmdTrain(void)
{
#if !defined(CONFIG_ATF_HAVE_BL2)
    extern ULONG dram_training_ld_addr;
    const UINT32 *pTempData;
    UINT32 *pTemp;
    const volatile ULONG *ptr;
#endif
    ULONG Addr = DDRCT_EXEC_SPACE;
    ULONG StackAddr = DDRCT_STACK_SPACE + DDRCT_STACK_SIZE;
    UINT32 i;
    volatile UINT32 *pLog;

#if !defined(CONFIG_ATF_HAVE_BL2)
    /* move tcm data to SRAM */
    AmbaMisra_TypeCast(&pTemp, &Addr);
    ptr = &dram_training_ld_addr;
    AmbaMisra_TypeCast(&pTempData, &ptr);
    for (i = 0U; i < (DRAM_TRAINING_CODE_SIZE>>2U); i++) {
        pTemp[i] = pTempData[i];
    }
#endif

    Addr = DDRCT_LOG_SPACE;
    AmbaMisra_TypeCast(&pLog, &Addr);
    //AmbaCSL_FioFifoRandomReadModeEnable();

    // Training parameters
    {
        UINT8 tr_dual_die_en = 0U, tr_start_hid = 0U, tr_end_hid = 0U, tr_link_ecc_en = 0U, dram_type = 0U;

        tr_dual_die_en = (DRAM_Config1Param >> 26) & 0x1U;
        if (0x0 != (DRAMC_ModeParam & 0x2U)) {
            tr_start_hid = 0U;
        } else {
            tr_start_hid = 1U;
        }
        if (0x0 != (DRAMC_ModeParam & 0x4U)) {
            tr_end_hid = 1U;
        } else {
            tr_end_hid = 0U;
        }
        if ((DRAM_Config2Param & 0x300U) == 0x300U) {   // bit[9:8]
            tr_link_ecc_en = 1;
        }
#if defined(CONFIG_DRAM_TYPE_LPDDR5)
        // LPDDR5
        i = (tr_dual_die_en << 0x13) | (tr_start_hid << 0x18) | (tr_end_hid << 0x1c) | (tr_link_ecc_en << 0x15);
        pLog[0] = i;            // 0x00, option_l

        dram_type = 1U;         // LPDDR5
        i = (dram_type << 0x1e);
        pLog[1] = i;            // 0x04, option_m
        pLog[2] = 0U;           // 0x08, pll_setting
        //mr11_d0 = mr_values & 0xff;
        //mr11_d1 = (mr_values & (0xff << 8)) >> 8;
        //mr18 = (mr_values & (0xff << 16)) >> 16;
        //mr41 = (mr_values & (0xff << 24)) >> 24;
        i = ((LPDDR5_ModeReg11ParamFsp0_Host0 & 0xffU)) |
            ((LPDDR5_ModeReg11ParamFsp0_Host0 & 0xffU) << 8) |
            ((LPDDR5_ModeReg18ParamFsp0_Host0 & 0xffU) << 16) |
            ((LPDDR5_ModeReg41ParamFsp0_Host0 & 0xffU) << 24);
        pLog[3] = i;            // 0x0c, mr_values
        i = ((LPDDR5_ModeReg13ParamFsp0_Host0 & 0xffU)) |
            ((LPDDR5_ModeReg16ParamFsp0_Host0 & 0xffU) << 8);
        pLog[4] = i;            // 0x10, mr_values
        pLog[5] = 1U;           // 0x14, train_debug_level,
        pLog[6] = 0x04020404U;  // 0x18, rd_vref_margin
        pLog[7] = 0x080c0808U;  // 0x1c, wr_vref_margin, [0]:vl_corse_margin,[1]:vh_coarse_margin,[2]:vl_fine_margin,[3]:vh_fine_margin
        pLog[8] = 0x04040404U;  // 0x20, dtte_vref_margin
        pLog[9] = 0xfU;         // 0x24, tr_step_select_oh
        pLog[10] = 0x0000U;     // 0x28, vref_offsets, B0: read, B1: write (signed)
        pLog[11] = 4U;          // 0x2c, vref_pass_range
        pLog[12] = 0x00000000U; // 0x30, read_delay_offset_ddrc0, B0: byte0, B1: byte1, ... (signed)
        pLog[13] = 0x00000000U; // 0x34, read_delay_offset_ddrc1, B0: byte0, B1: byte1, ... (signed)
        pLog[14] = 0x00000000U; // 0x38, write_delay_offset_ddrc0, B0: byte0, B1: byte1, ... (signed)
        pLog[15] = 0x00000000U; // 0x3c, write_delay_offset_ddrc1, B0: byte0, B1: byte1, ... (signed)
#else
        // LPDDR4
        i = (tr_dual_die_en << 0x13) | (tr_start_hid << 0x18) | (tr_end_hid << 0x1c) | (tr_link_ecc_en << 0x15);
        pLog[0] = i;            // 0x00, option_l

        dram_type = 0U;         // LPDDR4
        i = (dram_type << 0x1e);
        pLog[1] = i;            // 0x04, option_m
        pLog[2] = 0U;           // 0x08, pll_setting
        i = (LPDDR4_ModeReg11ParamFsp0_Host0 & 0xffU) | ((LPDDR4_ModeReg11ParamFsp0_Host0 & 0xffU) << 8);
        pLog[3] = i;            // 0x0c, mr_values
        pLog[4] = i;            // 0x10, mr_values
        pLog[5] = 1;            // 0x14, train_debug_level (2|(0x25<<0x8)|(0x17<<0x10))
        pLog[6] = 0x08080808U;  // 0x18, rd_vref_margin
        pLog[7] = 0x08080808U;  // 0x1c, wr_vref_margin, [0]:vl_corse_margin,[1]:vh_coarse_margin,[2]:vl_fine_margin,[3]:vh_fine_margin
        pLog[8] = 0x04040404U;  // 0x20, dtte_vref_margin
        pLog[9] = 0x7U;         // 0x24, tr_step_select_oh
        i = (TRAIN_VrefOffset_Host0) | (TRAIN_MR14Offset_Host0 << 8) |
            (TRAIN_VrefOffset_Host1 << 16) | (TRAIN_MR14Offset_Host1 << 24);
        pLog[10] = i;           // 0x28, vref_offsets, B0: read, B1: write (signed),  B2: read(DDRC1), B3: write(DDRC1) (signed)
        pLog[11] = 4U;          // 0x2c, vref_pass_range
        pLog[12] = TRAIN_ReadDelayOffset_Host0;  // 0x30, read_delay_offset_ddrc0, B0: byte0, B1: byte1, ... (signed)
        pLog[13] = TRAIN_ReadDelayOffset_Host1;  // 0x34, read_delay_offset_ddrc1, B0: byte0, B1: byte1, ... (signed)
        pLog[14] = TRAIN_WriteDelayOffset_Host0; // 0x38, write_delay_offset_ddrc0, B0: byte0, B1: byte1, ... (signed)
        pLog[15] = TRAIN_WriteDelayOffset_Host1; // 0x3c, write_delay_offset_ddrc1, B0: byte0, B1: byte1, ... (signed)
#endif
    }

    AmbaMMU_Disable_Training();

    PreTraining(StackAddr);

    AmbaMMU_Enable();
}

static void BLD_ShellDramTraining(UINT32 StoreRegVal, UINT32 OpMode)
{
    if (0x0U != StoreRegVal) {
        if (0x0U == (AmbaCSL_DdrcGetTrainScratchPad(0U) & DRAM_TRAIN_PROC_RESULT_MASK)) {
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

    if (OpMode == DRAM_TRAIN_OPMODE_AUTO_CMD) {
        BldShell_PrintStr("Start Dram Training\r\n");
        BLD_CmdDramCmdTrain();
        BldShell_PrintStr("Dram Training Done\r\n");
        BLD_CmdStoreTrainingResult((UINT32)DRAM_TRAIN_OPMODE_WRITE_DONE);
    } else {
        /* do nothing */
    }
}

static void BLD_CmdDramTrain(void)
{
    if ((0x0 == IO_UtilityStringCompare("read", &BldCmdLine[9], (SIZE_t)0x4)) ||
        (0x0 == IO_UtilityStringCompare("write", &BldCmdLine[9], (SIZE_t)0x5))) {
#if defined(CONFIG_DRAM_TYPE_LPDDR5)
        BldShell_PrintStr("LPDDR5 uses BST training, no manual training.\r\n");
#else
        UINT32 SysCtrlReg;

        SysCtrlReg = AmbaRTSL_CpuReadSysCtrl();
        SysCtrlReg &= ~((UINT32)1U << (UINT32)0U);  // Disable MMU
        AmbaRTSL_CpuWriteSysCtrl(SysCtrlReg);

        BldShell_PrintStr("CV5 doing read/write training together.\r\n");
        BLD_ShellDramTraining(0, DRAM_TRAIN_OPMODE_AUTO_CMD);
#endif
    } else if (0x0 == IO_UtilityStringCompare("erase", &BldCmdLine[9], (SIZE_t)0x5)) {
        UINT32 i, j;
#ifdef CONFIG_ENABLE_SPINOR_BOOT
        UINT8 *pData = &BstBinBuf[BST_OFST_DRAM_HIGH_FREQ_PARAM + 0x80UL];
#else
        UINT8 *pData = &BstBinBuf[BST_OFST_DRAM_HIGH_FREQ_PARAM];
#endif
        BLD_LoadNvmPart(0U, AMBA_SYS_PARTITION_BOOTSTRAP, BstBinBuf);
        (void)AmbaRTSL_CacheCleanDataPtr(BstBinBuf, BST_MAX_SIZE);

        for (j = 0; j < 2; j++) {      // Loop for parameter of two DDRC
            for (i = 0; i <= DRAM_TRAIN_PARAM_OFST_STATE * 4; i++) {    // Just clear the saved one, we don't want to corrupt the key of secure boot
#ifdef CONFIG_ENABLE_EMMC_BOOT
                pData[i] = 0U;
#else
                pData[i] = 0xFFU;
#endif
            }
            pData += DRAM_TRAIN_PARAM_OFST * 4;
        }

        /* Write BST data back */
        BLD_WriteNvmPart(0, (UINT32)AMBA_SYS_PARTITION_BOOTSTRAP, BST_MAX_SIZE, BstBinBuf);
    } else {
        BldShell_PrintStr("Error command\r\n");
        BldShell_PrintStr("Please keyin \"training read\"  for training\r\n");
        BldShell_PrintStr("Please keyin \"training write\" for training\r\n");
    }
}

static void BLD_ResetPtb(void)
{
    (void) AmbaRTSL_NvmSetActivePtbNo(1U, 0U);
    (void) AmbaRTSL_NvmSetActivePtbNo(0U, 1U);

    /* reload user PTB */
    if (AmbaRTSL_NvmReadUserPTB(NULL, 0U) != 0U) {
        BldShell_PrintStr("# Read User PTB 0 Fail\r\n");
    } else {
        BldShell_PrintStr("# Reset User PTB select to 0\r\n");
        #if defined(CONFIG_SVC_ENABLE_WDT)
        {
            AMBA_USER_PARTITION_TABLE_s *pUserPtb = AmbaBLD_GetUserPartTable();
            pUserPtb->Reserved[0U] = 0U;
            pUserPtb->Reserved[1U] = 0U;
            pUserPtb->Reserved[2U] = 0U;
            (void) AmbaRTSL_NvmWriteUserPTB(pUserPtb, 0U);
            BldShell_PrintStr("# reset WDT flag\r\n");
        }
        #endif
    }
}

#if defined(CONFIG_TEST_DRAM_SAVE_RESTORE)
UINT32 BLD_CompareByVal(char* name, volatile UINT32* ptr, volatile UINT32 val, UINT32 print)
{
    char IntStr[32];
    UINT32 result = 0;

    if(ptr == NULL)
    {
        if(print)
        {
            BldShell_PrintStr("[BLD][DDRC_DBG] ");
            BldShell_PrintStr(name);
            BldShell_PrintStr(" : SKIPPED");
            BldShell_PrintStr("\r\n");
        }
    }
    else
    {
        result = (*ptr != val ? 1 : 0);
        if(print || result)
        {
            BldShell_PrintStr("[BLD][DDRC_DBG] ");
            BldShell_PrintStr(name);
            BldShell_PrintStr(" @0x");
            (void)IO_UtilityUInt64ToStr(IntStr, 32U, (UINT64)ptr, 16U);
            BldShell_PrintStr(IntStr);
            BldShell_PrintStr(" =0x");
            (void)IO_UtilityUInt64ToStr(IntStr, 32U, (UINT64)*ptr, 16U);
            BldShell_PrintStr(IntStr);
            BldShell_PrintStr(" <===>");
            BldShell_PrintStr(" 0x");
            (void)IO_UtilityUInt32ToStr(IntStr, 32U, val, 16U);
            BldShell_PrintStr(IntStr);
            BldShell_PrintStr(" : ");
            BldShell_PrintStr(result ? "FAILED" : "PASSED");
            BldShell_PrintStr("\r\n");
        }
    }

    return result;
}

UINT32 BLD_CompareByPtr(char* name, volatile UINT32* ptr1, volatile UINT32* ptr2, UINT32 print)
{
    char IntStr[32];
    UINT32 result = 0;

    if((ptr1 == NULL) || (ptr2 == NULL))
    {
        if(print)
        {
            BldShell_PrintStr("[BLD][DDRC_DBG] ");
            BldShell_PrintStr(name);
            BldShell_PrintStr(" : SKIPPED");
            BldShell_PrintStr("\r\n");
        }
    }
    else
    {
        result = (*ptr1 != *ptr2 ? 1 : 0);
        if(print || result)
        {
            BldShell_PrintStr("[BLD][DDRC_DBG] ");
            BldShell_PrintStr(name);
            BldShell_PrintStr(" @0x");
            (void)IO_UtilityUInt64ToStr(IntStr, 32U, (UINT64)ptr1, 16U);
            BldShell_PrintStr(IntStr);
            BldShell_PrintStr(" =0x");
            (void)IO_UtilityUInt64ToStr(IntStr, 32U, (UINT64)*ptr1, 16U);
            BldShell_PrintStr(IntStr);
            BldShell_PrintStr(" <===>");
            BldShell_PrintStr(" @0x");
            (void)IO_UtilityUInt64ToStr(IntStr, 32U, (UINT64)ptr2, 16U);
            BldShell_PrintStr(IntStr);
            BldShell_PrintStr(" =0x");
            (void)IO_UtilityUInt64ToStr(IntStr, 32U, (UINT64)*ptr2, 16U);
            BldShell_PrintStr(IntStr);
            BldShell_PrintStr(" : ");
            BldShell_PrintStr(result ? "FAILED" : "PASSED");
            BldShell_PrintStr("\r\n");
        }
    }

    return result;
}

/*
 *  Porting AmbaSYS_Reboot from rtos/cortex_a/soc/io/src/cv5x/io-mw/threadx/AmbaSYS.c
 *  due to BLD long jump function call
 */
UINT32 BLD_Reset()
{
    UINT32 RetVal = 0;

#ifdef CONFIG_ENABLE_SPINOR_BOOT
    //TODO
#endif
#ifdef CONFIG_ENABLE_EMMC_BOOT
    (void) AmbaRTSL_PllSetSd0Clk(24000000U);
    AmbaRTSL_SdClockEnable(0U, 1U);

    /* Disable the interrupt for avoiding the other emmc command operation */
    AmbaRTSL_GicIntGlobalDisable();

    if (AmbaRTSL_SdSendCMD0(0U, 0xF0F0F0F0U) != 0) {
        RetVal = SYS_ERR_UNEXPECTED;
    }
#endif

    /* Trigger soc soft-reset */
    AmbaRTSL_PwcReboot();

    return RetVal;
}

void BLD_DBG_printHEX(UINT32 val, char* extra)
{
    char IntStr[16];
    BldShell_PrintStr("[BLD][DDRC_DBG][");
    BldShell_PrintStr(extra);
    BldShell_PrintStr("] X = ");
    (void)IO_UtilityUInt32ToStr(IntStr, 16U, val, 16U);
    BldShell_PrintStr(IntStr);
    BldShell_PrintStr("\r\n");
}

#if defined(CONFIG_TEST_DRAM_SAVE_RESTORE)
void BLD_CompareTrainingParameters()
{
    static UINT8 BstBinBuf[BST_MAX_SIZE] GNU_SECTION_NOZEROINIT;
#ifdef CONFIG_ENABLE_SPINOR_BOOT
    const UINT8 *pData = &BstBinBuf[BST_OFST_DRAM_HIGH_FREQ_PARAM + 0x80UL];
#else
    const UINT8 *pData = &BstBinBuf[BST_OFST_DRAM_HIGH_FREQ_PARAM];
#endif
    const AMBA_SYS_PARTITION_TABLE_s *pSysPtb = AmbaBLD_GetSysPartTable();

    UINT32 Crc32;
    UINT32 Delay = 0;
    UINT32 temp = 0;
    UINT32 *pDramParam;
    UINT32 Hid = 0U;
    UINT32 start_host_id = 0U;
    UINT32 end_host_id = 0U;
    UINT32 i = 0U;
    UINT32 print = DO_DBG_PRINT;
    UINT32 rst_count;
    UINT32 fail_count = 0U, total_fail_count = 0U;
    char IntStr[16];

    if (0x0 != (DRAMC_ModeParam & 0x2U)) {
        start_host_id = 0U;
    } else {
        start_host_id = 1U;
    }

    if (0x0 != (DRAMC_ModeParam & 0x4U)) {
        end_host_id = 1U;
    } else {
        end_host_id = 0U;
    }

    AmbaMisra_TypeCast(&pDramParam, &pData);

    /* Clean buffer */
    for (i = 0U; i < BST_MAX_SIZE; i++) {
        BstBinBuf[i] = 0;
    }

    /* Read training result from flash to buffer */
    AmbaMisra_TypeCast(&pData, &pSysPtb);
    Crc32 = IO_UtilityCrc32(pData, sizeof(AMBA_SYS_PARTITION_TABLE_s) - 4U);
    if (Crc32 != pSysPtb->CRC32) {
        BldShell_PrintStr("System partition check fail.\r\n");
    } else {
        BLD_LoadNvmPart(0U, AMBA_SYS_PARTITION_BOOTSTRAP, BstBinBuf);

        /* Compare dram trained paramters in nvm and DDRC regs */
        BldShell_PrintStr("[BLD][DDRC_DBG] Checking DRAM trained parameters in NVM <===> DDRC regs...\r\n");
        BLD_CompareByVal("DRAM_TRAIN_PARAM_OFST_FLAG            ", &pDramParam[DRAM_TRAIN_PARAM_OFST_FLAG]              , MAGIC_CODE_DRAM_TRAIN_COMPLETION, print);
        BLD_CompareByPtr("DRAM_TRAIN_PARAM_OFST_STATE           ", &pDramParam[DRAM_TRAIN_PARAM_OFST_STATE]             , NULL, print);

        for (Hid = start_host_id; Hid <= end_host_id; Hid++)
        {
            pDramParam = &pDramParam[DRAM_TRAIN_PARAM_OFST*Hid];
            // 57-2+13 items (LP5) for each host
#if defined(CONFIG_DRAM_TYPE_LPDDR5)
            fail_count += BLD_CompareByPtr("DRAM_TRAIN_PARAM_OFST_CK_DELAY        ", &pDramParam[DRAM_TRAIN_PARAM_OFST_CK_DELAY]          , AmbaCSL_DdrcGetCkDlyAddr(Hid), print);
            fail_count += BLD_CompareByPtr("DRAM_TRAIN_PARAM_OFST_CA_DELAY_COARSE ", &pDramParam[DRAM_TRAIN_PARAM_OFST_CA_DELAY_COARSE]   , AmbaCSL_DdrcGetCaDlyCoarsAddr(Hid), print);
            fail_count += BLD_CompareByPtr("DRAM_TRAIN_PARAM_OFST_CKE_DELAY_COARSE", &pDramParam[DRAM_TRAIN_PARAM_OFST_CKE_DELAY_COARSE]  , AmbaCSL_DdrcGetCkeDlyCoarsAddr(Hid), print);
#endif  // End of CONFIG_DRAM_TYPE_LPDDR5
            fail_count += BLD_CompareByPtr("DRAM_TRAIN_PARAM_OFST_RDDLY_DIE0      ", &pDramParam[DRAM_TRAIN_PARAM_OFST_RDDLY_DIE0]        , AmbaCSL_DdrcGetD0Dll0Addr(Hid), print);
            // DIE1 only
            if(BLD_Check_DRAM_DIE1_exist() == 1) {
                fail_count += BLD_CompareByPtr("DRAM_TRAIN_PARAM_OFST_RDDLY_DIE1      ", &pDramParam[DRAM_TRAIN_PARAM_OFST_RDDLY_DIE1]        , AmbaCSL_DdrcGetD1Dll0Addr(Hid), print);
            }
            fail_count += BLD_CompareByPtr("DRAM_TRAIN_PARAM_OFST_RDVREF_0        ", &pDramParam[DRAM_TRAIN_PARAM_OFST_RDVREF_0]          , AmbaCSL_DdrcGetRdVref0Addr(Hid), print);
            fail_count += BLD_CompareByPtr("DRAM_TRAIN_PARAM_OFST_RDVREF_1        ", &pDramParam[DRAM_TRAIN_PARAM_OFST_RDVREF_1]          , AmbaCSL_DdrcGetRdVref1Addr(Hid), print);
#if defined(CONFIG_DRAM_TYPE_LPDDR5)
            fail_count += BLD_CompareByPtr("DRAM_TRAIN_PARAM_OFST_WRVREF_0        ", &pDramParam[DRAM_TRAIN_PARAM_OFST_WRVREF_0]          , NULL, print);
            fail_count += BLD_CompareByPtr("DRAM_TRAIN_PARAM_OFST_WRVREF_1        ", &pDramParam[DRAM_TRAIN_PARAM_OFST_WRVREF_1]          , NULL, print);
#endif  // End of CONFIG_DRAM_TYPE_LPDDR5
            fail_count += BLD_CompareByPtr("DRAM_TRAIN_PARAM_OFST_B0_DLY0_D0      ", &pDramParam[DRAM_TRAIN_PARAM_OFST_B0_DLY0_D0]        , AmbaCSL_DdrcGetByte0D0DlyAddr(Hid, 0U), print);
            fail_count += BLD_CompareByPtr("DRAM_TRAIN_PARAM_OFST_B0_DLY1_D0      ", &pDramParam[DRAM_TRAIN_PARAM_OFST_B0_DLY1_D0]        , AmbaCSL_DdrcGetByte0D0DlyAddr(Hid, 1U), print);
            fail_count += BLD_CompareByPtr("DRAM_TRAIN_PARAM_OFST_B0_DLY2_D0      ", &pDramParam[DRAM_TRAIN_PARAM_OFST_B0_DLY2_D0]        , AmbaCSL_DdrcGetByte0D0DlyAddr(Hid, 2U), print);
            fail_count += BLD_CompareByPtr("DRAM_TRAIN_PARAM_OFST_B0_DLY3_D0      ", &pDramParam[DRAM_TRAIN_PARAM_OFST_B0_DLY3_D0]        , AmbaCSL_DdrcGetByte0D0DlyAddr(Hid, 3U), print);
            fail_count += BLD_CompareByPtr("DRAM_TRAIN_PARAM_OFST_B0_DLY4_D0      ", &pDramParam[DRAM_TRAIN_PARAM_OFST_B0_DLY4_D0]        , AmbaCSL_DdrcGetByte0D0DlyAddr(Hid, 4U), print);
            fail_count += BLD_CompareByPtr("DRAM_TRAIN_PARAM_OFST_B0_DLY5_D0      ", &pDramParam[DRAM_TRAIN_PARAM_OFST_B0_DLY5_D0]        , AmbaCSL_DdrcGetByte0D0DlyAddr(Hid, 5U), print);
            fail_count += BLD_CompareByPtr("DRAM_TRAIN_PARAM_OFST_B1_DLY0_D0      ", &pDramParam[DRAM_TRAIN_PARAM_OFST_B1_DLY0_D0]        , AmbaCSL_DdrcGetByte1D0DlyAddr(Hid, 0U), print);
            fail_count += BLD_CompareByPtr("DRAM_TRAIN_PARAM_OFST_B1_DLY1_D0      ", &pDramParam[DRAM_TRAIN_PARAM_OFST_B1_DLY1_D0]        , AmbaCSL_DdrcGetByte1D0DlyAddr(Hid, 1U), print);
            fail_count += BLD_CompareByPtr("DRAM_TRAIN_PARAM_OFST_B1_DLY2_D0      ", &pDramParam[DRAM_TRAIN_PARAM_OFST_B1_DLY2_D0]        , AmbaCSL_DdrcGetByte1D0DlyAddr(Hid, 2U), print);
            fail_count += BLD_CompareByPtr("DRAM_TRAIN_PARAM_OFST_B1_DLY3_D0      ", &pDramParam[DRAM_TRAIN_PARAM_OFST_B1_DLY3_D0]        , AmbaCSL_DdrcGetByte1D0DlyAddr(Hid, 3U), print);
            fail_count += BLD_CompareByPtr("DRAM_TRAIN_PARAM_OFST_B1_DLY4_D0      ", &pDramParam[DRAM_TRAIN_PARAM_OFST_B1_DLY4_D0]        , AmbaCSL_DdrcGetByte1D0DlyAddr(Hid, 4U), print);
            fail_count += BLD_CompareByPtr("DRAM_TRAIN_PARAM_OFST_B1_DLY5_D0      ", &pDramParam[DRAM_TRAIN_PARAM_OFST_B1_DLY5_D0]        , AmbaCSL_DdrcGetByte1D0DlyAddr(Hid, 5U), print);
            fail_count += BLD_CompareByPtr("DRAM_TRAIN_PARAM_OFST_B2_DLY0_D0      ", &pDramParam[DRAM_TRAIN_PARAM_OFST_B2_DLY0_D0]        , AmbaCSL_DdrcGetByte2D0DlyAddr(Hid, 0U), print);
            fail_count += BLD_CompareByPtr("DRAM_TRAIN_PARAM_OFST_B2_DLY1_D0      ", &pDramParam[DRAM_TRAIN_PARAM_OFST_B2_DLY1_D0]        , AmbaCSL_DdrcGetByte2D0DlyAddr(Hid, 1U), print);
            fail_count += BLD_CompareByPtr("DRAM_TRAIN_PARAM_OFST_B2_DLY2_D0      ", &pDramParam[DRAM_TRAIN_PARAM_OFST_B2_DLY2_D0]        , AmbaCSL_DdrcGetByte2D0DlyAddr(Hid, 2U), print);
            fail_count += BLD_CompareByPtr("DRAM_TRAIN_PARAM_OFST_B2_DLY3_D0      ", &pDramParam[DRAM_TRAIN_PARAM_OFST_B2_DLY3_D0]        , AmbaCSL_DdrcGetByte2D0DlyAddr(Hid, 3U), print);
            fail_count += BLD_CompareByPtr("DRAM_TRAIN_PARAM_OFST_B2_DLY4_D0      ", &pDramParam[DRAM_TRAIN_PARAM_OFST_B2_DLY4_D0]        , AmbaCSL_DdrcGetByte2D0DlyAddr(Hid, 4U), print);
            fail_count += BLD_CompareByPtr("DRAM_TRAIN_PARAM_OFST_B2_DLY5_D0      ", &pDramParam[DRAM_TRAIN_PARAM_OFST_B2_DLY5_D0]        , AmbaCSL_DdrcGetByte2D0DlyAddr(Hid, 5U), print);
            fail_count += BLD_CompareByPtr("DRAM_TRAIN_PARAM_OFST_B3_DLY0_D0      ", &pDramParam[DRAM_TRAIN_PARAM_OFST_B3_DLY0_D0]        , AmbaCSL_DdrcGetByte3D0DlyAddr(Hid, 0U), print);
            fail_count += BLD_CompareByPtr("DRAM_TRAIN_PARAM_OFST_B3_DLY1_D0      ", &pDramParam[DRAM_TRAIN_PARAM_OFST_B3_DLY1_D0]        , AmbaCSL_DdrcGetByte3D0DlyAddr(Hid, 1U), print);
            fail_count += BLD_CompareByPtr("DRAM_TRAIN_PARAM_OFST_B3_DLY2_D0      ", &pDramParam[DRAM_TRAIN_PARAM_OFST_B3_DLY2_D0]        , AmbaCSL_DdrcGetByte3D0DlyAddr(Hid, 2U), print);
            fail_count += BLD_CompareByPtr("DRAM_TRAIN_PARAM_OFST_B3_DLY3_D0      ", &pDramParam[DRAM_TRAIN_PARAM_OFST_B3_DLY3_D0]        , AmbaCSL_DdrcGetByte3D0DlyAddr(Hid, 3U), print);
            fail_count += BLD_CompareByPtr("DRAM_TRAIN_PARAM_OFST_B3_DLY4_D0      ", &pDramParam[DRAM_TRAIN_PARAM_OFST_B3_DLY4_D0]        , AmbaCSL_DdrcGetByte3D0DlyAddr(Hid, 4U), print);
            fail_count += BLD_CompareByPtr("DRAM_TRAIN_PARAM_OFST_B3_DLY5_D0      ", &pDramParam[DRAM_TRAIN_PARAM_OFST_B3_DLY5_D0]        , AmbaCSL_DdrcGetByte3D0DlyAddr(Hid, 5U), print);
            // DIE1 only
            if(BLD_Check_DRAM_DIE1_exist() == 1) {
                fail_count += BLD_CompareByPtr("DRAM_TRAIN_PARAM_OFST_B0_DLY0_D1      ", &pDramParam[DRAM_TRAIN_PARAM_OFST_B0_DLY0_D1]        , AmbaCSL_DdrcGetByte0D1DlyAddr(Hid, 0U), print);
                fail_count += BLD_CompareByPtr("DRAM_TRAIN_PARAM_OFST_B0_DLY1_D1      ", &pDramParam[DRAM_TRAIN_PARAM_OFST_B0_DLY1_D1]        , AmbaCSL_DdrcGetByte0D1DlyAddr(Hid, 1U), print);
                fail_count += BLD_CompareByPtr("DRAM_TRAIN_PARAM_OFST_B0_DLY2_D1      ", &pDramParam[DRAM_TRAIN_PARAM_OFST_B0_DLY2_D1]        , AmbaCSL_DdrcGetByte0D1DlyAddr(Hid, 2U), print);
                fail_count += BLD_CompareByPtr("DRAM_TRAIN_PARAM_OFST_B0_DLY3_D1      ", &pDramParam[DRAM_TRAIN_PARAM_OFST_B0_DLY3_D1]        , AmbaCSL_DdrcGetByte0D1DlyAddr(Hid, 3U), print);
                fail_count += BLD_CompareByPtr("DRAM_TRAIN_PARAM_OFST_B0_DLY4_D1      ", &pDramParam[DRAM_TRAIN_PARAM_OFST_B0_DLY4_D1]        , AmbaCSL_DdrcGetByte0D1DlyAddr(Hid, 4U), print);
                fail_count += BLD_CompareByPtr("DRAM_TRAIN_PARAM_OFST_B0_DLY5_D1      ", &pDramParam[DRAM_TRAIN_PARAM_OFST_B0_DLY5_D1]        , AmbaCSL_DdrcGetByte0D1DlyAddr(Hid, 5U), print);
                fail_count += BLD_CompareByPtr("DRAM_TRAIN_PARAM_OFST_B1_DLY0_D1      ", &pDramParam[DRAM_TRAIN_PARAM_OFST_B1_DLY0_D1]        , AmbaCSL_DdrcGetByte1D1DlyAddr(Hid, 0U), print);
                fail_count += BLD_CompareByPtr("DRAM_TRAIN_PARAM_OFST_B1_DLY1_D1      ", &pDramParam[DRAM_TRAIN_PARAM_OFST_B1_DLY1_D1]        , AmbaCSL_DdrcGetByte1D1DlyAddr(Hid, 1U), print);
                fail_count += BLD_CompareByPtr("DRAM_TRAIN_PARAM_OFST_B1_DLY2_D1      ", &pDramParam[DRAM_TRAIN_PARAM_OFST_B1_DLY2_D1]        , AmbaCSL_DdrcGetByte1D1DlyAddr(Hid, 2U), print);
                fail_count += BLD_CompareByPtr("DRAM_TRAIN_PARAM_OFST_B1_DLY3_D1      ", &pDramParam[DRAM_TRAIN_PARAM_OFST_B1_DLY3_D1]        , AmbaCSL_DdrcGetByte1D1DlyAddr(Hid, 3U), print);
                fail_count += BLD_CompareByPtr("DRAM_TRAIN_PARAM_OFST_B1_DLY4_D1      ", &pDramParam[DRAM_TRAIN_PARAM_OFST_B1_DLY4_D1]        , AmbaCSL_DdrcGetByte1D1DlyAddr(Hid, 4U), print);
                fail_count += BLD_CompareByPtr("DRAM_TRAIN_PARAM_OFST_B1_DLY5_D1      ", &pDramParam[DRAM_TRAIN_PARAM_OFST_B1_DLY5_D1]        , AmbaCSL_DdrcGetByte1D1DlyAddr(Hid, 5U), print);
                fail_count += BLD_CompareByPtr("DRAM_TRAIN_PARAM_OFST_B2_DLY0_D1      ", &pDramParam[DRAM_TRAIN_PARAM_OFST_B2_DLY0_D1]        , AmbaCSL_DdrcGetByte2D1DlyAddr(Hid, 0U), print);
                fail_count += BLD_CompareByPtr("DRAM_TRAIN_PARAM_OFST_B2_DLY1_D1      ", &pDramParam[DRAM_TRAIN_PARAM_OFST_B2_DLY1_D1]        , AmbaCSL_DdrcGetByte2D1DlyAddr(Hid, 1U), print);
                fail_count += BLD_CompareByPtr("DRAM_TRAIN_PARAM_OFST_B2_DLY2_D1      ", &pDramParam[DRAM_TRAIN_PARAM_OFST_B2_DLY2_D1]        , AmbaCSL_DdrcGetByte2D1DlyAddr(Hid, 2U), print);
                fail_count += BLD_CompareByPtr("DRAM_TRAIN_PARAM_OFST_B2_DLY3_D1      ", &pDramParam[DRAM_TRAIN_PARAM_OFST_B2_DLY3_D1]        , AmbaCSL_DdrcGetByte2D1DlyAddr(Hid, 3U), print);
                fail_count += BLD_CompareByPtr("DRAM_TRAIN_PARAM_OFST_B2_DLY4_D1      ", &pDramParam[DRAM_TRAIN_PARAM_OFST_B2_DLY4_D1]        , AmbaCSL_DdrcGetByte2D1DlyAddr(Hid, 4U), print);
                fail_count += BLD_CompareByPtr("DRAM_TRAIN_PARAM_OFST_B2_DLY5_D1      ", &pDramParam[DRAM_TRAIN_PARAM_OFST_B2_DLY5_D1]        , AmbaCSL_DdrcGetByte2D1DlyAddr(Hid, 5U), print);
                fail_count += BLD_CompareByPtr("DRAM_TRAIN_PARAM_OFST_B3_DLY0_D1      ", &pDramParam[DRAM_TRAIN_PARAM_OFST_B3_DLY0_D1]        , AmbaCSL_DdrcGetByte3D1DlyAddr(Hid, 0U), print);
                fail_count += BLD_CompareByPtr("DRAM_TRAIN_PARAM_OFST_B3_DLY1_D1      ", &pDramParam[DRAM_TRAIN_PARAM_OFST_B3_DLY1_D1]        , AmbaCSL_DdrcGetByte3D1DlyAddr(Hid, 1U), print);
                fail_count += BLD_CompareByPtr("DRAM_TRAIN_PARAM_OFST_B3_DLY2_D1      ", &pDramParam[DRAM_TRAIN_PARAM_OFST_B3_DLY2_D1]        , AmbaCSL_DdrcGetByte3D1DlyAddr(Hid, 2U), print);
                fail_count += BLD_CompareByPtr("DRAM_TRAIN_PARAM_OFST_B3_DLY3_D1      ", &pDramParam[DRAM_TRAIN_PARAM_OFST_B3_DLY3_D1]        , AmbaCSL_DdrcGetByte3D1DlyAddr(Hid, 3U), print);
                fail_count += BLD_CompareByPtr("DRAM_TRAIN_PARAM_OFST_B3_DLY4_D1      ", &pDramParam[DRAM_TRAIN_PARAM_OFST_B3_DLY4_D1]        , AmbaCSL_DdrcGetByte3D1DlyAddr(Hid, 4U), print);
                fail_count += BLD_CompareByPtr("DRAM_TRAIN_PARAM_OFST_B3_DLY5_D1      ", &pDramParam[DRAM_TRAIN_PARAM_OFST_B3_DLY5_D1]        , AmbaCSL_DdrcGetByte3D1DlyAddr(Hid, 5U), print);
            }

#if defined(CONFIG_DRAM_TYPE_LPDDR5)
            /* MR 30 */
            // Get settings from C0 D0
            /* MR30 cannot be read from DDRC (LP5 spec limitation), so this is obsolete code
            AmbaCSL_DdrcSetModeReg(Hid, 0x8a1e0000);
            while (0x0U != (AmbaCSL_DdrcGetModeReg(Hid) & 0x80000000U)) {
                ;
            }
            Delay = AmbaCSL_DdrcGetModeReg(Hid);
            Delay = (Delay & 0xffU);
            temp = ((0x9f1e0000U) | (Delay & 0xffU));
            fail_count += BLD_CompareByVal("DRAM_TRAIN_PARAM_OFST_MR30            ", &pDramParam[DRAM_TRAIN_PARAM_OFST_MR30]              , temp, print);
            */

            /* MR 12 */
            // Get settings from C0/C1 D0
            AmbaCSL_DdrcSetModeReg(Hid, 0x9a0c0000);
            while (0x0U != (AmbaCSL_DdrcGetModeReg(Hid) & 0x80000000U)) {
                ;
            }
            Delay = AmbaCSL_DdrcGetModeReg(Hid);
            Delay = (Delay & 0xffU);
            temp = ((0x8b0c0000U) | (Delay & 0xffU));
            fail_count += BLD_CompareByVal("DRAM_TRAIN_PARAM_OFST_MR12C0D0        ", &pDramParam[DRAM_TRAIN_PARAM_OFST_MR12C0D0]          , temp, print);
            Delay = AmbaCSL_DdrcGetModeReg(Hid);
            Delay = ((Delay & 0xff00U) >> 0x8U);
            temp = ((0x930c0000U) | (Delay & 0xffU));
            fail_count += BLD_CompareByVal("DRAM_TRAIN_PARAM_OFST_MR12C1D0        ", &pDramParam[DRAM_TRAIN_PARAM_OFST_MR12C1D0]          , temp, print);
            // Get settings from C0/C1 D1
            if(BLD_Check_DRAM_DIE1_exist() == 1) {
                AmbaCSL_DdrcSetModeReg(Hid, 0x9c0c0000);
                while (0x0U != (AmbaCSL_DdrcGetModeReg(Hid) & 0x80000000U)) {
                    ;
                }
                Delay = AmbaCSL_DdrcGetModeReg(Hid);
                Delay = (Delay & 0xffU);
                temp = ((0x8d0c0000U) | (Delay & 0xffU));
                fail_count += BLD_CompareByVal("DRAM_TRAIN_PARAM_OFST_MR12C0D1        ", &pDramParam[DRAM_TRAIN_PARAM_OFST_MR12C0D1]          , temp, print);
                Delay = AmbaCSL_DdrcGetModeReg(Hid);
                Delay = ((Delay & 0xff00U) >> 0x8U);
                temp = ((0x950c0000U) | (Delay & 0xffU));
                fail_count += BLD_CompareByVal("DRAM_TRAIN_PARAM_OFST_MR12C1D1        ", &pDramParam[DRAM_TRAIN_PARAM_OFST_MR12C1D1]          , temp, print);
            }
#endif  // End of CONFIG_DRAM_TYPE_LPDDR5

            /* MR 14 */
            // Get settings from C0/C1 D0
            AmbaCSL_DdrcSetModeReg(Hid, 0x9a0e0000);
            while (0x0U != (AmbaCSL_DdrcGetModeReg(Hid) & 0x80000000U)) {
                ;
            }
            Delay = AmbaCSL_DdrcGetModeReg(Hid);
            Delay = (Delay & 0xffU);
            temp = ((0x8b0e0000U) | (Delay & 0xffU));
            fail_count += BLD_CompareByVal("DRAM_TRAIN_PARAM_OFST_MR14C0D0        ", &pDramParam[DRAM_TRAIN_PARAM_OFST_MR14C0D0]          , temp, print);
            Delay = AmbaCSL_DdrcGetModeReg(Hid);
            Delay = ((Delay & 0xff00U) >> 0x8U);
            temp = ((0x930e0000U) | (Delay & 0xffU));
            fail_count += BLD_CompareByVal("DRAM_TRAIN_PARAM_OFST_MR14C1D0        ", &pDramParam[DRAM_TRAIN_PARAM_OFST_MR14C1D0]          , temp, print);
            // Get settings from C0/C1 D1
            if(BLD_Check_DRAM_DIE1_exist() == 1) {
                AmbaCSL_DdrcSetModeReg(Hid, 0x9c0e0000);
                while (0x0U != (AmbaCSL_DdrcGetModeReg(Hid) & 0x80000000U)) {
                    ;
                }
                Delay = AmbaCSL_DdrcGetModeReg(Hid);
                Delay = (Delay & 0xffU);
                temp = ((0x8d0e0000U) | (Delay & 0xffU));
                fail_count += BLD_CompareByVal("DRAM_TRAIN_PARAM_OFST_MR14C0D1        ", &pDramParam[DRAM_TRAIN_PARAM_OFST_MR14C0D1]          , temp, print);
                Delay = AmbaCSL_DdrcGetModeReg(Hid);
                Delay = ((Delay & 0xff00U) >> 0x8U);
                temp = ((0x950e0000U) | (Delay & 0xffU));
                fail_count += BLD_CompareByVal("DRAM_TRAIN_PARAM_OFST_MR14C1D1        ", &pDramParam[DRAM_TRAIN_PARAM_OFST_MR14C1D1]          , temp, print);
            }

#if defined(CONFIG_DRAM_TYPE_LPDDR5)
            /* MR 15 */
            // Get settings from C0/C1 D0
            AmbaCSL_DdrcSetModeReg(Hid, 0x9a0f0000);
            while (0x0U != (AmbaCSL_DdrcGetModeReg(Hid) & 0x80000000U)) {
                ;
            }
            Delay = AmbaCSL_DdrcGetModeReg(Hid);
            Delay = (Delay & 0xffU);
            temp = ((0x8b0f0000U) | (Delay & 0xffU));
            fail_count += BLD_CompareByVal("DRAM_TRAIN_PARAM_OFST_MR15C0D0        ", &pDramParam[DRAM_TRAIN_PARAM_OFST_MR15C0D0]          , temp, print);

            Delay = AmbaCSL_DdrcGetModeReg(Hid);
            Delay = ((Delay & 0xff00U) >> 0x8U);
            temp = ((0x930f0000U) | (Delay & 0xffU));
            fail_count += BLD_CompareByVal("DRAM_TRAIN_PARAM_OFST_MR15C1D0        ", &pDramParam[DRAM_TRAIN_PARAM_OFST_MR15C1D0]          , temp, print);
            // Get settings from C0/C1 D1
            if(BLD_Check_DRAM_DIE1_exist() == 1) {
                AmbaCSL_DdrcSetModeReg(Hid, 0x9c0f0000);
                while (0x0U != (AmbaCSL_DdrcGetModeReg(Hid) & 0x80000000U)) {
                    ;
                }
                Delay = AmbaCSL_DdrcGetModeReg(Hid);
                Delay = (Delay & 0xffU);
                temp = ((0x8d0f0000U) | (Delay & 0xffU));
                fail_count += BLD_CompareByVal("DRAM_TRAIN_PARAM_OFST_MR15C0D1        ", &pDramParam[DRAM_TRAIN_PARAM_OFST_MR15C0D1]          , temp, print);
                Delay = AmbaCSL_DdrcGetModeReg(Hid);
                Delay = ((Delay & 0xff00U) >> 0x8U);
                temp = ((0x950f0000U) | (Delay & 0xffU));
                fail_count += BLD_CompareByVal("DRAM_TRAIN_PARAM_OFST_MR15C1D1        ", &pDramParam[DRAM_TRAIN_PARAM_OFST_MR15C1D1]          , temp, print);
            }
#endif  // End of CONFIG_DRAM_TYPE_LPDDR5
        }

        /* Deal with reset/fail count */
        rst_count = pDramParam[DRAM_TRAIN_PARAM_OFST_RSTCOUNT];
        rst_count += 1;
        pDramParam[DRAM_TRAIN_PARAM_OFST_RSTCOUNT] += 1;
        total_fail_count = pDramParam[DRAM_TRAIN_PARAM_OFST_TOTALFAILCOUNT];
        total_fail_count += fail_count;
        pDramParam[DRAM_TRAIN_PARAM_OFST_TOTALFAILCOUNT] = total_fail_count;

        BldShell_PrintStr("[BLD][DDRC_DBG] Compare done in ");
        (void)IO_UtilityUInt32ToStr(IntStr, 16U, rst_count, 10U);
        BldShell_PrintStr(IntStr);
        BldShell_PrintStr(" reset, fail count=");
        (void)IO_UtilityUInt32ToStr(IntStr, 16U, fail_count, 10U);
        BldShell_PrintStr(IntStr);
        BldShell_PrintStr("\r\n");

        (void)AmbaRTSL_CacheCleanDataPtr(BstBinBuf, BST_MAX_SIZE);

        /* Write BST data back */
        BLD_WriteNvmPart(0, (UINT32)AMBA_SYS_PARTITION_BOOTSTRAP, BST_MAX_SIZE, BstBinBuf);

        /* Do reset for stress */
        if(rst_count < MAX_RESET_COUNT)
        {
            BLD_Reset();
        }

        /* Final */
        BldShell_PrintStr("\r\n");
        BldShell_PrintStr("[BLD][DDRC_DBG] Compare all done, total fail=");
        (void)IO_UtilityUInt32ToStr(IntStr, 16U, total_fail_count, 10U);
        BldShell_PrintStr(IntStr);
        BldShell_PrintStr(" stress test ===> ");
        BldShell_PrintStr(total_fail_count ? "FAILED" : "PASSED");
        BldShell_PrintStr("\r\n");
    }
}
#endif

#if defined(CONFIG_TEST_DRAM_SAVE_RESTORE) && (PEEP_DRAM_TRAINING_PARAM == 1)
void BLD_PeepTrainingParameters(UINT32 dbg_count)
{
    static UINT8 BstBinBuf[BST_MAX_SIZE] GNU_SECTION_NOZEROINIT;
#ifdef CONFIG_ENABLE_SPINOR_BOOT
    const UINT8 *pData = &BstBinBuf[BST_OFST_DRAM_HIGH_FREQ_PARAM + 0x80UL];
#else
    const UINT8 *pData = &BstBinBuf[BST_OFST_DRAM_HIGH_FREQ_PARAM];
#endif
    const AMBA_SYS_PARTITION_TABLE_s *pSysPtb = AmbaBLD_GetSysPartTable();

    UINT32 Crc32;
    UINT32 Delay = 0;
    UINT32 temp = 0;
    UINT32 *pDramParam;
    UINT32 Hid = 0U;
    UINT32 start_host_id = 0U;
    UINT32 end_host_id = 0U;
    UINT32 i = 0U;
    UINT32 print = DO_DBG_PRINT;
    UINT32 fail_count = 0U;
    char IntStr[16];
    char IntStr2[32];

    if (0x0 != (DRAMC_ModeParam & 0x2U)) {
        start_host_id = 0U;
    } else {
        start_host_id = 1U;
    }

    if (0x0 != (DRAMC_ModeParam & 0x4U)) {
        end_host_id = 1U;
    } else {
        end_host_id = 0U;
    }

    AmbaMisra_TypeCast(&pDramParam, &pData);

    /* Clean buffer */
    for (i = 0U; i < BST_MAX_SIZE; i++) {
        BstBinBuf[i] = 0;
    }

    BldShell_PrintStr("@BLD_PeepTrainingParameters\r\n");
    BldShell_PrintStr("BstBinBuf addr: 0x");
    (void)IO_UtilityUInt64ToStr(IntStr2, 32U, (UINT64)&BstBinBuf[0], 16U);
    BldShell_PrintStr(IntStr2);
    BldShell_PrintStr("\r\n");
    BldShell_PrintStr("BstBinBuf end addr: 0x");
    (void)IO_UtilityUInt64ToStr(IntStr2, 32U, (UINT64)&BstBinBuf[BST_MAX_SIZE], 16U);
    BldShell_PrintStr(IntStr2);
    BldShell_PrintStr("\r\n");
    BldShell_PrintStr("pData: 0x");
    (void)IO_UtilityUInt64ToStr(IntStr2, 32U, (UINT64)(UINT64*)pData, 16U);
    BldShell_PrintStr(IntStr2);
    BldShell_PrintStr("\r\n");

    BldShell_PrintStr("DBG Peep mode... dbg_count = ");
    (void)IO_UtilityUInt32ToStr(IntStr, 16U, dbg_count, 10U);
    BldShell_PrintStr(IntStr);
    BldShell_PrintStr("\r\n");

    /* Read training result from flash to buffer */
    AmbaMisra_TypeCast(&pData, &pSysPtb);
    Crc32 = IO_UtilityCrc32(pData, sizeof(AMBA_SYS_PARTITION_TABLE_s) - 4U);
    if (Crc32 != pSysPtb->CRC32) {
        BldShell_PrintStr("System partition check fail.\r\n");
    } else {
        BLD_LoadNvmPart(0U, AMBA_SYS_PARTITION_BOOTSTRAP, BstBinBuf);

        /* Compare dram trained paramters in nvm and DDRC regs */
        BldShell_PrintStr("[BLD][DDRC_DBG] Checking DRAM trained parameters in NVM <===> DDRC regs...\r\n");
        BLD_CompareByVal("DRAM_TRAIN_PARAM_OFST_FLAG            ", &pDramParam[DRAM_TRAIN_PARAM_OFST_FLAG]              , MAGIC_CODE_DRAM_TRAIN_COMPLETION, print);
        BLD_CompareByVal("DRAM_TRAIN_PARAM_OFST_STATE           ", &pDramParam[DRAM_TRAIN_PARAM_OFST_STATE]             , 0x0, print);

        for (Hid = start_host_id; Hid <= end_host_id; Hid++)
        {
            pDramParam = &pDramParam[DRAM_TRAIN_PARAM_OFST*Hid];
#if defined(CONFIG_DRAM_TYPE_LPDDR5)
            fail_count += BLD_CompareByPtr("DRAM_TRAIN_PARAM_OFST_CK_DELAY        ", &pDramParam[DRAM_TRAIN_PARAM_OFST_CK_DELAY]          , AmbaCSL_DdrcGetCkDlyAddr(Hid), print);
            fail_count += BLD_CompareByPtr("DRAM_TRAIN_PARAM_OFST_CA_DELAY_COARSE ", &pDramParam[DRAM_TRAIN_PARAM_OFST_CA_DELAY_COARSE]   , AmbaCSL_DdrcGetCaDlyCoarsAddr(Hid), print);
            fail_count += BLD_CompareByPtr("DRAM_TRAIN_PARAM_OFST_CKE_DELAY_COARSE", &pDramParam[DRAM_TRAIN_PARAM_OFST_CKE_DELAY_COARSE]  , AmbaCSL_DdrcGetCkeDlyCoarsAddr(Hid), print);
#endif  // End of CONFIG_DRAM_TYPE_LPDDR5
            fail_count += BLD_CompareByPtr("DRAM_TRAIN_PARAM_OFST_RDDLY_DIE0      ", &pDramParam[DRAM_TRAIN_PARAM_OFST_RDDLY_DIE0]        , AmbaCSL_DdrcGetD0Dll0Addr(Hid), print);
            // DIE1 only
            if(BLD_Check_DRAM_DIE1_exist() == 1) {
                fail_count += BLD_CompareByPtr("DRAM_TRAIN_PARAM_OFST_RDDLY_DIE1      ", &pDramParam[DRAM_TRAIN_PARAM_OFST_RDDLY_DIE1]        , AmbaCSL_DdrcGetD1Dll0Addr(Hid), print);
            }
            fail_count += BLD_CompareByPtr("DRAM_TRAIN_PARAM_OFST_RDVREF_0        ", &pDramParam[DRAM_TRAIN_PARAM_OFST_RDVREF_0]          , AmbaCSL_DdrcGetRdVref0Addr(Hid), print);
            fail_count += BLD_CompareByPtr("DRAM_TRAIN_PARAM_OFST_RDVREF_1        ", &pDramParam[DRAM_TRAIN_PARAM_OFST_RDVREF_1]          , AmbaCSL_DdrcGetRdVref1Addr(Hid), print);
#if defined(CONFIG_DRAM_TYPE_LPDDR5)
            fail_count += BLD_CompareByPtr("DRAM_TRAIN_PARAM_OFST_WRVREF_0        ", &pDramParam[DRAM_TRAIN_PARAM_OFST_WRVREF_0]          , NULL, print);
            fail_count += BLD_CompareByPtr("DRAM_TRAIN_PARAM_OFST_WRVREF_1        ", &pDramParam[DRAM_TRAIN_PARAM_OFST_WRVREF_1]          , NULL, print);
#endif  // End of CONFIG_DRAM_TYPE_LPDDR5
            fail_count += BLD_CompareByPtr("DRAM_TRAIN_PARAM_OFST_B0_DLY0_D0      ", &pDramParam[DRAM_TRAIN_PARAM_OFST_B0_DLY0_D0]        , AmbaCSL_DdrcGetByte0D0DlyAddr(Hid, 0U), print);
            fail_count += BLD_CompareByPtr("DRAM_TRAIN_PARAM_OFST_B0_DLY1_D0      ", &pDramParam[DRAM_TRAIN_PARAM_OFST_B0_DLY1_D0]        , AmbaCSL_DdrcGetByte0D0DlyAddr(Hid, 1U), print);
            fail_count += BLD_CompareByPtr("DRAM_TRAIN_PARAM_OFST_B0_DLY2_D0      ", &pDramParam[DRAM_TRAIN_PARAM_OFST_B0_DLY2_D0]        , AmbaCSL_DdrcGetByte0D0DlyAddr(Hid, 2U), print);
            fail_count += BLD_CompareByPtr("DRAM_TRAIN_PARAM_OFST_B0_DLY3_D0      ", &pDramParam[DRAM_TRAIN_PARAM_OFST_B0_DLY3_D0]        , AmbaCSL_DdrcGetByte0D0DlyAddr(Hid, 3U), print);
            fail_count += BLD_CompareByPtr("DRAM_TRAIN_PARAM_OFST_B0_DLY4_D0      ", &pDramParam[DRAM_TRAIN_PARAM_OFST_B0_DLY4_D0]        , AmbaCSL_DdrcGetByte0D0DlyAddr(Hid, 4U), print);
            fail_count += BLD_CompareByPtr("DRAM_TRAIN_PARAM_OFST_B0_DLY5_D0      ", &pDramParam[DRAM_TRAIN_PARAM_OFST_B0_DLY5_D0]        , AmbaCSL_DdrcGetByte0D0DlyAddr(Hid, 5U), print);
            fail_count += BLD_CompareByPtr("DRAM_TRAIN_PARAM_OFST_B1_DLY0_D0      ", &pDramParam[DRAM_TRAIN_PARAM_OFST_B1_DLY0_D0]        , AmbaCSL_DdrcGetByte1D0DlyAddr(Hid, 0U), print);
            fail_count += BLD_CompareByPtr("DRAM_TRAIN_PARAM_OFST_B1_DLY1_D0      ", &pDramParam[DRAM_TRAIN_PARAM_OFST_B1_DLY1_D0]        , AmbaCSL_DdrcGetByte1D0DlyAddr(Hid, 1U), print);
            fail_count += BLD_CompareByPtr("DRAM_TRAIN_PARAM_OFST_B1_DLY2_D0      ", &pDramParam[DRAM_TRAIN_PARAM_OFST_B1_DLY2_D0]        , AmbaCSL_DdrcGetByte1D0DlyAddr(Hid, 2U), print);
            fail_count += BLD_CompareByPtr("DRAM_TRAIN_PARAM_OFST_B1_DLY3_D0      ", &pDramParam[DRAM_TRAIN_PARAM_OFST_B1_DLY3_D0]        , AmbaCSL_DdrcGetByte1D0DlyAddr(Hid, 3U), print);
            fail_count += BLD_CompareByPtr("DRAM_TRAIN_PARAM_OFST_B1_DLY4_D0      ", &pDramParam[DRAM_TRAIN_PARAM_OFST_B1_DLY4_D0]        , AmbaCSL_DdrcGetByte1D0DlyAddr(Hid, 4U), print);
            fail_count += BLD_CompareByPtr("DRAM_TRAIN_PARAM_OFST_B1_DLY5_D0      ", &pDramParam[DRAM_TRAIN_PARAM_OFST_B1_DLY5_D0]        , AmbaCSL_DdrcGetByte1D0DlyAddr(Hid, 5U), print);
            fail_count += BLD_CompareByPtr("DRAM_TRAIN_PARAM_OFST_B2_DLY0_D0      ", &pDramParam[DRAM_TRAIN_PARAM_OFST_B2_DLY0_D0]        , AmbaCSL_DdrcGetByte2D0DlyAddr(Hid, 0U), print);
            fail_count += BLD_CompareByPtr("DRAM_TRAIN_PARAM_OFST_B2_DLY1_D0      ", &pDramParam[DRAM_TRAIN_PARAM_OFST_B2_DLY1_D0]        , AmbaCSL_DdrcGetByte2D0DlyAddr(Hid, 1U), print);
            fail_count += BLD_CompareByPtr("DRAM_TRAIN_PARAM_OFST_B2_DLY2_D0      ", &pDramParam[DRAM_TRAIN_PARAM_OFST_B2_DLY2_D0]        , AmbaCSL_DdrcGetByte2D0DlyAddr(Hid, 2U), print);
            fail_count += BLD_CompareByPtr("DRAM_TRAIN_PARAM_OFST_B2_DLY3_D0      ", &pDramParam[DRAM_TRAIN_PARAM_OFST_B2_DLY3_D0]        , AmbaCSL_DdrcGetByte2D0DlyAddr(Hid, 3U), print);
            fail_count += BLD_CompareByPtr("DRAM_TRAIN_PARAM_OFST_B2_DLY4_D0      ", &pDramParam[DRAM_TRAIN_PARAM_OFST_B2_DLY4_D0]        , AmbaCSL_DdrcGetByte2D0DlyAddr(Hid, 4U), print);
            fail_count += BLD_CompareByPtr("DRAM_TRAIN_PARAM_OFST_B2_DLY5_D0      ", &pDramParam[DRAM_TRAIN_PARAM_OFST_B2_DLY5_D0]        , AmbaCSL_DdrcGetByte2D0DlyAddr(Hid, 5U), print);
            fail_count += BLD_CompareByPtr("DRAM_TRAIN_PARAM_OFST_B3_DLY0_D0      ", &pDramParam[DRAM_TRAIN_PARAM_OFST_B3_DLY0_D0]        , AmbaCSL_DdrcGetByte3D0DlyAddr(Hid, 0U), print);
            fail_count += BLD_CompareByPtr("DRAM_TRAIN_PARAM_OFST_B3_DLY1_D0      ", &pDramParam[DRAM_TRAIN_PARAM_OFST_B3_DLY1_D0]        , AmbaCSL_DdrcGetByte3D0DlyAddr(Hid, 1U), print);
            fail_count += BLD_CompareByPtr("DRAM_TRAIN_PARAM_OFST_B3_DLY2_D0      ", &pDramParam[DRAM_TRAIN_PARAM_OFST_B3_DLY2_D0]        , AmbaCSL_DdrcGetByte3D0DlyAddr(Hid, 2U), print);
            fail_count += BLD_CompareByPtr("DRAM_TRAIN_PARAM_OFST_B3_DLY3_D0      ", &pDramParam[DRAM_TRAIN_PARAM_OFST_B3_DLY3_D0]        , AmbaCSL_DdrcGetByte3D0DlyAddr(Hid, 3U), print);
            fail_count += BLD_CompareByPtr("DRAM_TRAIN_PARAM_OFST_B3_DLY4_D0      ", &pDramParam[DRAM_TRAIN_PARAM_OFST_B3_DLY4_D0]        , AmbaCSL_DdrcGetByte3D0DlyAddr(Hid, 4U), print);
            fail_count += BLD_CompareByPtr("DRAM_TRAIN_PARAM_OFST_B3_DLY5_D0      ", &pDramParam[DRAM_TRAIN_PARAM_OFST_B3_DLY5_D0]        , AmbaCSL_DdrcGetByte3D0DlyAddr(Hid, 5U), print);
            // DIE1 only
            if(BLD_Check_DRAM_DIE1_exist() == 1) {
                fail_count += BLD_CompareByPtr("DRAM_TRAIN_PARAM_OFST_B0_DLY0_D1      ", &pDramParam[DRAM_TRAIN_PARAM_OFST_B0_DLY0_D1]        , AmbaCSL_DdrcGetByte0D1DlyAddr(Hid, 0U), print);
                fail_count += BLD_CompareByPtr("DRAM_TRAIN_PARAM_OFST_B0_DLY1_D1      ", &pDramParam[DRAM_TRAIN_PARAM_OFST_B0_DLY1_D1]        , AmbaCSL_DdrcGetByte0D1DlyAddr(Hid, 1U), print);
                fail_count += BLD_CompareByPtr("DRAM_TRAIN_PARAM_OFST_B0_DLY2_D1      ", &pDramParam[DRAM_TRAIN_PARAM_OFST_B0_DLY2_D1]        , AmbaCSL_DdrcGetByte0D1DlyAddr(Hid, 2U), print);
                fail_count += BLD_CompareByPtr("DRAM_TRAIN_PARAM_OFST_B0_DLY3_D1      ", &pDramParam[DRAM_TRAIN_PARAM_OFST_B0_DLY3_D1]        , AmbaCSL_DdrcGetByte0D1DlyAddr(Hid, 3U), print);
                fail_count += BLD_CompareByPtr("DRAM_TRAIN_PARAM_OFST_B0_DLY4_D1      ", &pDramParam[DRAM_TRAIN_PARAM_OFST_B0_DLY4_D1]        , AmbaCSL_DdrcGetByte0D1DlyAddr(Hid, 4U), print);
                fail_count += BLD_CompareByPtr("DRAM_TRAIN_PARAM_OFST_B0_DLY5_D1      ", &pDramParam[DRAM_TRAIN_PARAM_OFST_B0_DLY5_D1]        , AmbaCSL_DdrcGetByte0D1DlyAddr(Hid, 5U), print);
                fail_count += BLD_CompareByPtr("DRAM_TRAIN_PARAM_OFST_B1_DLY0_D1      ", &pDramParam[DRAM_TRAIN_PARAM_OFST_B1_DLY0_D1]        , AmbaCSL_DdrcGetByte1D1DlyAddr(Hid, 0U), print);
                fail_count += BLD_CompareByPtr("DRAM_TRAIN_PARAM_OFST_B1_DLY1_D1      ", &pDramParam[DRAM_TRAIN_PARAM_OFST_B1_DLY1_D1]        , AmbaCSL_DdrcGetByte1D1DlyAddr(Hid, 1U), print);
                fail_count += BLD_CompareByPtr("DRAM_TRAIN_PARAM_OFST_B1_DLY2_D1      ", &pDramParam[DRAM_TRAIN_PARAM_OFST_B1_DLY2_D1]        , AmbaCSL_DdrcGetByte1D1DlyAddr(Hid, 2U), print);
                fail_count += BLD_CompareByPtr("DRAM_TRAIN_PARAM_OFST_B1_DLY3_D1      ", &pDramParam[DRAM_TRAIN_PARAM_OFST_B1_DLY3_D1]        , AmbaCSL_DdrcGetByte1D1DlyAddr(Hid, 3U), print);
                fail_count += BLD_CompareByPtr("DRAM_TRAIN_PARAM_OFST_B1_DLY4_D1      ", &pDramParam[DRAM_TRAIN_PARAM_OFST_B1_DLY4_D1]        , AmbaCSL_DdrcGetByte1D1DlyAddr(Hid, 4U), print);
                fail_count += BLD_CompareByPtr("DRAM_TRAIN_PARAM_OFST_B1_DLY5_D1      ", &pDramParam[DRAM_TRAIN_PARAM_OFST_B1_DLY5_D1]        , AmbaCSL_DdrcGetByte1D1DlyAddr(Hid, 5U), print);
                fail_count += BLD_CompareByPtr("DRAM_TRAIN_PARAM_OFST_B2_DLY0_D1      ", &pDramParam[DRAM_TRAIN_PARAM_OFST_B2_DLY0_D1]        , AmbaCSL_DdrcGetByte2D1DlyAddr(Hid, 0U), print);
                fail_count += BLD_CompareByPtr("DRAM_TRAIN_PARAM_OFST_B2_DLY1_D1      ", &pDramParam[DRAM_TRAIN_PARAM_OFST_B2_DLY1_D1]        , AmbaCSL_DdrcGetByte2D1DlyAddr(Hid, 1U), print);
                fail_count += BLD_CompareByPtr("DRAM_TRAIN_PARAM_OFST_B2_DLY2_D1      ", &pDramParam[DRAM_TRAIN_PARAM_OFST_B2_DLY2_D1]        , AmbaCSL_DdrcGetByte2D1DlyAddr(Hid, 2U), print);
                fail_count += BLD_CompareByPtr("DRAM_TRAIN_PARAM_OFST_B2_DLY3_D1      ", &pDramParam[DRAM_TRAIN_PARAM_OFST_B2_DLY3_D1]        , AmbaCSL_DdrcGetByte2D1DlyAddr(Hid, 3U), print);
                fail_count += BLD_CompareByPtr("DRAM_TRAIN_PARAM_OFST_B2_DLY4_D1      ", &pDramParam[DRAM_TRAIN_PARAM_OFST_B2_DLY4_D1]        , AmbaCSL_DdrcGetByte2D1DlyAddr(Hid, 4U), print);
                fail_count += BLD_CompareByPtr("DRAM_TRAIN_PARAM_OFST_B2_DLY5_D1      ", &pDramParam[DRAM_TRAIN_PARAM_OFST_B2_DLY5_D1]        , AmbaCSL_DdrcGetByte2D1DlyAddr(Hid, 5U), print);
                fail_count += BLD_CompareByPtr("DRAM_TRAIN_PARAM_OFST_B3_DLY0_D1      ", &pDramParam[DRAM_TRAIN_PARAM_OFST_B3_DLY0_D1]        , AmbaCSL_DdrcGetByte3D1DlyAddr(Hid, 0U), print);
                fail_count += BLD_CompareByPtr("DRAM_TRAIN_PARAM_OFST_B3_DLY1_D1      ", &pDramParam[DRAM_TRAIN_PARAM_OFST_B3_DLY1_D1]        , AmbaCSL_DdrcGetByte3D1DlyAddr(Hid, 1U), print);
                fail_count += BLD_CompareByPtr("DRAM_TRAIN_PARAM_OFST_B3_DLY2_D1      ", &pDramParam[DRAM_TRAIN_PARAM_OFST_B3_DLY2_D1]        , AmbaCSL_DdrcGetByte3D1DlyAddr(Hid, 2U), print);
                fail_count += BLD_CompareByPtr("DRAM_TRAIN_PARAM_OFST_B3_DLY3_D1      ", &pDramParam[DRAM_TRAIN_PARAM_OFST_B3_DLY3_D1]        , AmbaCSL_DdrcGetByte3D1DlyAddr(Hid, 3U), print);
                fail_count += BLD_CompareByPtr("DRAM_TRAIN_PARAM_OFST_B3_DLY4_D1      ", &pDramParam[DRAM_TRAIN_PARAM_OFST_B3_DLY4_D1]        , AmbaCSL_DdrcGetByte3D1DlyAddr(Hid, 4U), print);
                fail_count += BLD_CompareByPtr("DRAM_TRAIN_PARAM_OFST_B3_DLY5_D1      ", &pDramParam[DRAM_TRAIN_PARAM_OFST_B3_DLY5_D1]        , AmbaCSL_DdrcGetByte3D1DlyAddr(Hid, 5U), print);
            }

#if defined(CONFIG_DRAM_TYPE_LPDDR5)
            /* MR 30 */
            // Get settings from C0 D0
            /* MR30 cannot be read from DDRC (LP5 spec limitation), so this is obsolete code
            AmbaCSL_DdrcSetModeReg(Hid, 0x8a1e0000);
            while (0x0U != (AmbaCSL_DdrcGetModeReg(Hid) & 0x80000000U)) {
                ;
            }
            Delay = AmbaCSL_DdrcGetModeReg(Hid);
            Delay = (Delay & 0xffU);
            temp = ((0x9f1e0000U) | (Delay & 0xffU));
            fail_count += BLD_CompareByVal("DRAM_TRAIN_PARAM_OFST_MR30            ", &pDramParam[DRAM_TRAIN_PARAM_OFST_MR30]              , temp, print);
            */

            // Get settings from system scratchpad memory
            BldShell_PrintStr("[BLD][DDRC_DBG] DRAM_TRAIN_PARAM_OFST_MR30C0D0=");
            (void)IO_UtilityUInt32ToStr(IntStr, 16U, pDramParam[DRAM_TRAIN_PARAM_OFST_MR30C0D0], 16U);
            BldShell_PrintStr(IntStr);
            BldShell_PrintStr("\r\n");
            BldShell_PrintStr("[BLD][DDRC_DBG] DRAM_TRAIN_PARAM_OFST_MR30C0D1=");
            (void)IO_UtilityUInt32ToStr(IntStr, 16U, pDramParam[DRAM_TRAIN_PARAM_OFST_MR30C0D1], 16U);
            BldShell_PrintStr(IntStr);
            BldShell_PrintStr("\r\n");
            if(BLD_Check_DRAM_DIE1_exist() == 1) {
                BldShell_PrintStr("[BLD][DDRC_DBG] DRAM_TRAIN_PARAM_OFST_MR30C1D0=");
                (void)IO_UtilityUInt32ToStr(IntStr, 16U, pDramParam[DRAM_TRAIN_PARAM_OFST_MR30C1D0], 16U);
                BldShell_PrintStr(IntStr);
                BldShell_PrintStr("\r\n");
                BldShell_PrintStr("[BLD][DDRC_DBG] DRAM_TRAIN_PARAM_OFST_MR30C1D1=");
                (void)IO_UtilityUInt32ToStr(IntStr, 16U, pDramParam[DRAM_TRAIN_PARAM_OFST_MR30C1D1], 16U);
                BldShell_PrintStr(IntStr);
                BldShell_PrintStr("\r\n");
            }

            /* MR 12 */
            // Get settings from C0/C1 D0
            AmbaCSL_DdrcSetModeReg(Hid, 0x9a0c0000);
            while (0x0U != (AmbaCSL_DdrcGetModeReg(Hid) & 0x80000000U)) {
                ;
            }
            Delay = AmbaCSL_DdrcGetModeReg(Hid);
            Delay = (Delay & 0xffU);
            temp = ((0x8b0c0000U) | (Delay & 0xffU));
            fail_count += BLD_CompareByVal("DRAM_TRAIN_PARAM_OFST_MR12C0D0        ", &pDramParam[DRAM_TRAIN_PARAM_OFST_MR12C0D0]          , temp, print);
            Delay = AmbaCSL_DdrcGetModeReg(Hid);
            Delay = ((Delay & 0xff00U) >> 0x8U);
            temp = ((0x930c0000U) | (Delay & 0xffU));
            fail_count += BLD_CompareByVal("DRAM_TRAIN_PARAM_OFST_MR12C1D0        ", &pDramParam[DRAM_TRAIN_PARAM_OFST_MR12C1D0]          , temp, print);
            // Get settings from C0/C1 D1
            if(BLD_Check_DRAM_DIE1_exist() == 1) {
                AmbaCSL_DdrcSetModeReg(Hid, 0x9c0c0000);
                while (0x0U != (AmbaCSL_DdrcGetModeReg(Hid) & 0x80000000U)) {
                    ;
                }
                Delay = AmbaCSL_DdrcGetModeReg(Hid);
                Delay = (Delay & 0xffU);
                temp = ((0x8d0c0000U) | (Delay & 0xffU));
                fail_count += BLD_CompareByVal("DRAM_TRAIN_PARAM_OFST_MR12C0D1        ", &pDramParam[DRAM_TRAIN_PARAM_OFST_MR12C0D1]          , temp, print);
                Delay = AmbaCSL_DdrcGetModeReg(Hid);
                Delay = ((Delay & 0xff00U) >> 0x8U);
                temp = ((0x950c0000U) | (Delay & 0xffU));
                fail_count += BLD_CompareByVal("DRAM_TRAIN_PARAM_OFST_MR12C1D1        ", &pDramParam[DRAM_TRAIN_PARAM_OFST_MR12C1D1]          , temp, print);
            }
#endif  // End of CONFIG_DRAM_TYPE_LPDDR5

            /* MR 14 */
            // Get settings from C0/C1 D0
            AmbaCSL_DdrcSetModeReg(Hid, 0x9a0e0000);
            while (0x0U != (AmbaCSL_DdrcGetModeReg(Hid) & 0x80000000U)) {
                ;
            }
            Delay = AmbaCSL_DdrcGetModeReg(Hid);
            BLD_DBG_printHEX(Delay, (Hid == 0) ? "HOST0" : "HOST1");
            Delay = (Delay & 0xffU);
            temp = ((0x8b0e0000U) | (Delay & 0xffU));
            fail_count += BLD_CompareByVal("DRAM_TRAIN_PARAM_OFST_MR14C0D0        ", &pDramParam[DRAM_TRAIN_PARAM_OFST_MR14C0D0]          , temp, print);
            Delay = AmbaCSL_DdrcGetModeReg(Hid);
            BLD_DBG_printHEX(Delay, (Hid == 0) ? "HOST0" : "HOST1");
            Delay = ((Delay & 0xff00U) >> 0x8U);
            temp = ((0x930e0000U) | (Delay & 0xffU));
            fail_count += BLD_CompareByVal("DRAM_TRAIN_PARAM_OFST_MR14C1D0        ", &pDramParam[DRAM_TRAIN_PARAM_OFST_MR14C1D0]          , temp, print);
            // Get settings from C0/C1 D1
            if(BLD_Check_DRAM_DIE1_exist() == 1) {
                AmbaCSL_DdrcSetModeReg(Hid, 0x9c0e0000);
                while (0x0U != (AmbaCSL_DdrcGetModeReg(Hid) & 0x80000000U)) {
                    ;
                }
                Delay = AmbaCSL_DdrcGetModeReg(Hid);
                BLD_DBG_printHEX(Delay, (Hid == 0) ? "HOST0" : "HOST1");
                Delay = (Delay & 0xffU);
                temp = ((0x8d0e0000U) | (Delay & 0xffU));
                fail_count += BLD_CompareByVal("DRAM_TRAIN_PARAM_OFST_MR14C0D1        ", &pDramParam[DRAM_TRAIN_PARAM_OFST_MR14C0D1]          , temp, print);
                Delay = AmbaCSL_DdrcGetModeReg(Hid);
                BLD_DBG_printHEX(Delay, (Hid == 0) ? "HOST0" : "HOST1");
                Delay = ((Delay & 0xff00U) >> 0x8U);
                temp = ((0x950e0000U) | (Delay & 0xffU));
                fail_count += BLD_CompareByVal("DRAM_TRAIN_PARAM_OFST_MR14C1D1        ", &pDramParam[DRAM_TRAIN_PARAM_OFST_MR14C1D1]          , temp, print);
            }

#if defined(CONFIG_DRAM_TYPE_LPDDR5)
            /* MR 15 */
            // Get settings from C0/C1 D0
            AmbaCSL_DdrcSetModeReg(Hid, 0x9a0f0000);
            while (0x0U != (AmbaCSL_DdrcGetModeReg(Hid) & 0x80000000U)) {
                ;
            }
            Delay = AmbaCSL_DdrcGetModeReg(Hid);
            Delay = (Delay & 0xffU);
            temp = ((0x8b0f0000U) | (Delay & 0xffU));
            fail_count += BLD_CompareByVal("DRAM_TRAIN_PARAM_OFST_MR15C0D0        ", &pDramParam[DRAM_TRAIN_PARAM_OFST_MR15C0D0]          , temp, print);

            Delay = AmbaCSL_DdrcGetModeReg(Hid);
            Delay = ((Delay & 0xff00U) >> 0x8U);
            temp = ((0x930f0000U) | (Delay & 0xffU));
            fail_count += BLD_CompareByVal("DRAM_TRAIN_PARAM_OFST_MR15C1D0        ", &pDramParam[DRAM_TRAIN_PARAM_OFST_MR15C1D0]          , temp, print);
            // Get settings from C0/C1 D1
            if(BLD_Check_DRAM_DIE1_exist() == 1) {
                AmbaCSL_DdrcSetModeReg(Hid, 0x9c0f0000);
                while (0x0U != (AmbaCSL_DdrcGetModeReg(Hid) & 0x80000000U)) {
                    ;
                }
                Delay = AmbaCSL_DdrcGetModeReg(Hid);
                Delay = (Delay & 0xffU);
                temp = ((0x8d0f0000U) | (Delay & 0xffU));
                fail_count += BLD_CompareByVal("DRAM_TRAIN_PARAM_OFST_MR15C0D1        ", &pDramParam[DRAM_TRAIN_PARAM_OFST_MR15C0D1]          , temp, print);
                Delay = AmbaCSL_DdrcGetModeReg(Hid);
                Delay = ((Delay & 0xff00U) >> 0x8U);
                temp = ((0x950f0000U) | (Delay & 0xffU));
                fail_count += BLD_CompareByVal("DRAM_TRAIN_PARAM_OFST_MR15C1D1        ", &pDramParam[DRAM_TRAIN_PARAM_OFST_MR15C1D1]          , temp, print);
            }
#endif  // End of CONFIG_DRAM_TYPE_LPDDR5

        }

        BldShell_PrintStr("[BLD][DDRC_DBG] DDRC scratchpad=");
        (void)IO_UtilityUInt32ToStr(IntStr, 16U, AmbaCSL_DdrcGetTrainScratchPad(0U), 10U);
        BldShell_PrintStr(IntStr);
        BldShell_PrintStr("\r\n");
        BldShell_PrintStr("[BLD][DDRC_DBG] Compare done, fail count=");
        (void)IO_UtilityUInt32ToStr(IntStr, 16U, fail_count, 10U);
        BldShell_PrintStr(IntStr);
        BldShell_PrintStr("\r\n");

    }
}
#endif  // End of PEEP_DRAM_TRAINING_PARAM
#endif  // End of CONFIG_TEST_DRAM_SAVE_RESTORE

void BLD_CheckTrainingResult(void)
{
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
#if defined(CONFIG_ATF_HAVE_BL2)
    UINT8 *pData;
    UINT32 *pDramParam;
    char IntStr[32];
#ifdef CONFIG_ARM64
    UINT64 temp;
#else
    UINT32 temp;
#endif
#endif
#endif
    volatile UINT32 DramTrainingState;
#if defined(CONFIG_TEST_DRAM_SAVE_RESTORE) && (PEEP_DRAM_TRAINING_PARAM == 1)
    // Force to check anyway...
    char IntStr[32];
    BLD_PeepTrainingParameters(1);
#endif

    DramTrainingState = AmbaCSL_DdrcGetTrainScratchPad(0U);
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
#if defined(CONFIG_ATF_HAVE_BL2)
    // For CV5x cannot read DDRC regs in NS-EL1
    // DramTrainingState in this case should be 0xDEAD2BAD
#ifdef CONFIG_ARM64
    temp = pAmbaScratchpadNS_Reg->AhbScratchpad[1];
    temp = (temp << 32) | pAmbaScratchpadNS_Reg->AhbScratchpad[0];
#else
    temp = pAmbaScratchpadNS_Reg->AhbScratchpad[0];
#endif
    pData = (UINT8*)temp;
    AmbaMisra_TypeCast(&pDramParam, &pData);
    DramTrainingState = pDramParam[DRAM_TRAIN_PARAM_OFST_STATE];
#endif
#endif
    (void)AmbaRTSL_CacheFlushDataPtr((UINT32*)&DramTrainingState, sizeof(UINT32));

#if defined(CONFIG_ATF_HAVE_BL2)
    BldShell_PrintStr("[DBG] DramTrainingState: ");
    (void)IO_UtilityUInt64ToStr(IntStr, 32U, DramTrainingState, 16U);
    BldShell_PrintStr(IntStr);
    BldShell_PrintStr("\r\n");
#endif

    if ( MAGIC_CODE_DRAM_TRAIN_COMPLETION != DramTrainingState ) {
        if (MAGIC_CODE_DRAM_TRAIN_FAILURE == DramTrainingState) {
            BldShell_PrintStr("\r\nPlease try dram training manually. MAGIC_CODE_DRAM_TRAIN_FAILURE!\r\n");
        } else {
            DramTrainingState = (DramTrainingState >> DRAM_TRAIN_PROC_OPMODE_OFST);
            if ( DramTrainingState == 0U ) {
//#ifdef CONFIG_AUTO_DRAM_TRAINING
#if !defined(CONFIG_BST_DRAM_TRAINING)
                BLD_ShellDramTraining(0U, DRAM_TRAIN_OPMODE_AUTO_CMD);
#endif
//#endif
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
            } else if ( DramTrainingState == DRAM_TRAIN_OPMODE_BOTH_RW_DONE ) {
                BLD_CmdStoreTrainingResult(DRAM_TRAIN_OPMODE_BOTH_RW_DONE);
            } else {
                BldShell_PrintStr("\r\nPlease try dram training manually. Undefined case!\r\n");
            }
        }
    } else {
        BldShell_PrintStr("\r\nDram has been trained\r\n");
#if defined(CONFIG_BST_WDELAY_UPDATE_RETRAIN)
        // Write delay training everytime, so we need to update the wck2dqx in scratchpad
        ddrc_save_wck2dqx(NULL);
#endif
#if defined(CONFIG_ENABLE_DRAM_TRAINING_EVERY_BOOT)
        BLD_ShellDramTraining(0U, DRAM_TRAIN_OPMODE_AUTO_CMD);
#endif

#if defined(CONFIG_TEST_DRAM_SAVE_RESTORE)
        BLD_CompareTrainingParameters();
#endif
    }

#if defined(CONFIG_TEST_DRAM_SAVE_RESTORE) && (PEEP_DRAM_TRAINING_PARAM == 1)
    // Force to check anyway...
    BLD_PeepTrainingParameters(2);
    BLD_PeepTrainingParameters(3);

    BldShell_PrintStr("DBG: ");
    (void)IO_UtilityUInt64ToStr(IntStr, 32U, AMBA_CORTEX_A76_FLASH_IO_BASE_ADDR + BST_OFST_DRAM_HIGH_FREQ_PARAM + DRAM_TRAIN_PARAM_OFST_FLAG, 16U);
    BldShell_PrintStr(IntStr);
    BldShell_PrintStr("\r\n");
#endif
}

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
    RetVal = BLD_DetectEscapeKey(250U);
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
                } else if (0x0 == IO_UtilityStringCompare("resetptb", BldCmdLine, (SIZE_t)0x8)) {
                    BLD_ResetPtb();
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
                        BLD_CmdDramTrain();
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

/*============== APIs used for BL2 ==============*/

#if defined(CONFIG_ATF_HAVE_BL2)
void *BL2_getAmbaTmpBuffer(void)
{
    return BstBinBuf;
}

void BL2_DramTraining(void)
{
    BLD_CmdDramCmdTrain();
}

void BL2_UpdateDdrc(void)
{
    /*  Expected that DRAM parameters already restored in BST.
     *  Save OPMODE to system scratchpad memory only to let BLD check
     */
    const UINT8 *pData = (UINT8*)BL2BINBUF;
    UINT32 *pDramParam;
    AmbaMisra_TypeCast(&pDramParam, &pData);
    pDramParam[DRAM_TRAIN_PARAM_OFST_STATE] = (UINT32)MAGIC_CODE_DRAM_TRAIN_COMPLETION;
#ifdef CONFIG_ARM64
    pAmbaScratchpadNS_Reg->AhbScratchpad[0] = ((UINT64)pData & 0x00000000ffffffff);
    pAmbaScratchpadNS_Reg->AhbScratchpad[1] = ((UINT64)pData & 0xffffffff00000000) >> 32;
#else
    pAmbaScratchpadNS_Reg->AhbScratchpad[0] = (UINT32)pData;
#endif
}

void BL2_DramTrainFail(void)
{
    while(1);
}

void BL2_CmdStoreTrainingResult(UINT32 OpMode)
{
    const UINT8 *pData = (UINT8*)BL2BINBUF;

    UINT32 Delay = 0;
    UINT32 *pDramParam;
    UINT32 Hid = 0U;
    UINT32 start_host_id = 0U;
    UINT32 end_host_id = 0U;

    if (0x0 != (DRAMC_ModeParam & 0x2U)) {
        start_host_id = 0U;
    } else {
        start_host_id = 1U;
    }

    if (0x0 != (DRAMC_ModeParam & 0x4U)) {
        end_host_id = 1U;
    } else {
        end_host_id = 0U;
    }

    AmbaMisra_TypeCast(&pDramParam, &pData);
    // We use system scratchpad-NS registers
    // to save the real address of the temp buffer (BL2BINBUF) in DRAM and pass down to BLD
#ifdef CONFIG_ARM64
    pAmbaScratchpadNS_Reg->AhbScratchpad[0] = ((UINT64)pData & 0x00000000ffffffff);
    pAmbaScratchpadNS_Reg->AhbScratchpad[1] = ((UINT64)pData & 0xffffffff00000000) >> 32;
#else
    pAmbaScratchpadNS_Reg->AhbScratchpad[0] = (UINT32)pData;
#endif

    /* Start store training result to DRAM buffer */
    /* Read BST data to temp buffer */
    pDramParam[DRAM_TRAIN_PARAM_OFST_FLAG] = MAGIC_CODE_DRAM_TRAIN_COMPLETION;
    pDramParam[DRAM_TRAIN_PARAM_OFST_STATE] = OpMode;

    for (Hid = start_host_id; Hid <= end_host_id; Hid++) {
        pDramParam = &pDramParam[DRAM_TRAIN_PARAM_OFST*Hid];
#if defined(CONFIG_DRAM_TYPE_LPDDR5)
        pDramParam[DRAM_TRAIN_PARAM_OFST_CA_DELAY_COARSE]  = AmbaCSL_DdrcGetCaDlyCoars(Hid);
        pDramParam[DRAM_TRAIN_PARAM_OFST_CKE_DELAY_COARSE] = AmbaCSL_DdrcGetCkeDlyCoars(Hid);
        pDramParam[DRAM_TRAIN_PARAM_OFST_CK_DELAY]         = AmbaCSL_DdrcGetCkDly(Hid);
#endif  // End of CONFIG_DRAM_TYPE_LPDDR5
        pDramParam[DRAM_TRAIN_PARAM_OFST_RDDLY_DIE0]       = AmbaCSL_DdrcGetD0Dll0(Hid);
        // DIE1 only
        if(BLD_Check_DRAM_DIE1_exist() == 1) {
            pDramParam[DRAM_TRAIN_PARAM_OFST_RDDLY_DIE1]       = AmbaCSL_DdrcGetD1Dll0(Hid);
        }
        pDramParam[DRAM_TRAIN_PARAM_OFST_RDVREF_0]         = AmbaCSL_DdrcGetRdVref0(Hid);
        pDramParam[DRAM_TRAIN_PARAM_OFST_RDVREF_1]         = AmbaCSL_DdrcGetRdVref1(Hid);
#if defined(CONFIG_DRAM_TYPE_LPDDR5)
        pDramParam[DRAM_TRAIN_PARAM_OFST_WRVREF_0]         = AmbaCSL_DdrcGetWriteVref0(Hid);
        pDramParam[DRAM_TRAIN_PARAM_OFST_WRVREF_1]         = AmbaCSL_DdrcGetWriteVref1(Hid);
#endif  // End of CONFIG_DRAM_TYPE_LPDDR5
        pDramParam[DRAM_TRAIN_PARAM_OFST_B0_DLY0_D0]       = AmbaCSL_DdrcGetByte0D0Dly(Hid, 0U);
        pDramParam[DRAM_TRAIN_PARAM_OFST_B0_DLY1_D0]       = AmbaCSL_DdrcGetByte0D0Dly(Hid, 1U);
        pDramParam[DRAM_TRAIN_PARAM_OFST_B0_DLY2_D0]       = AmbaCSL_DdrcGetByte0D0Dly(Hid, 2U);
        pDramParam[DRAM_TRAIN_PARAM_OFST_B0_DLY3_D0]       = AmbaCSL_DdrcGetByte0D0Dly(Hid, 3U);
        pDramParam[DRAM_TRAIN_PARAM_OFST_B0_DLY4_D0]       = AmbaCSL_DdrcGetByte0D0Dly(Hid, 4U);
        pDramParam[DRAM_TRAIN_PARAM_OFST_B0_DLY5_D0]       = AmbaCSL_DdrcGetByte0D0Dly(Hid, 5U);
        pDramParam[DRAM_TRAIN_PARAM_OFST_B1_DLY0_D0]       = AmbaCSL_DdrcGetByte1D0Dly(Hid, 0U);
        pDramParam[DRAM_TRAIN_PARAM_OFST_B1_DLY1_D0]       = AmbaCSL_DdrcGetByte1D0Dly(Hid, 1U);
        pDramParam[DRAM_TRAIN_PARAM_OFST_B1_DLY2_D0]       = AmbaCSL_DdrcGetByte1D0Dly(Hid, 2U);
        pDramParam[DRAM_TRAIN_PARAM_OFST_B1_DLY3_D0]       = AmbaCSL_DdrcGetByte1D0Dly(Hid, 3U);
        pDramParam[DRAM_TRAIN_PARAM_OFST_B1_DLY4_D0]       = AmbaCSL_DdrcGetByte1D0Dly(Hid, 4U);
        pDramParam[DRAM_TRAIN_PARAM_OFST_B1_DLY5_D0]       = AmbaCSL_DdrcGetByte1D0Dly(Hid, 5U);
        pDramParam[DRAM_TRAIN_PARAM_OFST_B2_DLY0_D0]       = AmbaCSL_DdrcGetByte2D0Dly(Hid, 0U);
        pDramParam[DRAM_TRAIN_PARAM_OFST_B2_DLY1_D0]       = AmbaCSL_DdrcGetByte2D0Dly(Hid, 1U);
        pDramParam[DRAM_TRAIN_PARAM_OFST_B2_DLY2_D0]       = AmbaCSL_DdrcGetByte2D0Dly(Hid, 2U);
        pDramParam[DRAM_TRAIN_PARAM_OFST_B2_DLY3_D0]       = AmbaCSL_DdrcGetByte2D0Dly(Hid, 3U);
        pDramParam[DRAM_TRAIN_PARAM_OFST_B2_DLY4_D0]       = AmbaCSL_DdrcGetByte2D0Dly(Hid, 4U);
        pDramParam[DRAM_TRAIN_PARAM_OFST_B2_DLY5_D0]       = AmbaCSL_DdrcGetByte2D0Dly(Hid, 5U);
        pDramParam[DRAM_TRAIN_PARAM_OFST_B3_DLY0_D0]       = AmbaCSL_DdrcGetByte3D0Dly(Hid, 0U);
        pDramParam[DRAM_TRAIN_PARAM_OFST_B3_DLY1_D0]       = AmbaCSL_DdrcGetByte3D0Dly(Hid, 1U);
        pDramParam[DRAM_TRAIN_PARAM_OFST_B3_DLY2_D0]       = AmbaCSL_DdrcGetByte3D0Dly(Hid, 2U);
        pDramParam[DRAM_TRAIN_PARAM_OFST_B3_DLY3_D0]       = AmbaCSL_DdrcGetByte3D0Dly(Hid, 3U);
        pDramParam[DRAM_TRAIN_PARAM_OFST_B3_DLY4_D0]       = AmbaCSL_DdrcGetByte3D0Dly(Hid, 4U);
        pDramParam[DRAM_TRAIN_PARAM_OFST_B3_DLY5_D0]       = AmbaCSL_DdrcGetByte3D0Dly(Hid, 5U);
        // DIE1 only
        if(BLD_Check_DRAM_DIE1_exist() == 1) {
            pDramParam[DRAM_TRAIN_PARAM_OFST_B0_DLY0_D1]       = AmbaCSL_DdrcGetByte0D1Dly(Hid, 0U);
            pDramParam[DRAM_TRAIN_PARAM_OFST_B0_DLY1_D1]       = AmbaCSL_DdrcGetByte0D1Dly(Hid, 1U);
            pDramParam[DRAM_TRAIN_PARAM_OFST_B0_DLY2_D1]       = AmbaCSL_DdrcGetByte0D1Dly(Hid, 2U);
            pDramParam[DRAM_TRAIN_PARAM_OFST_B0_DLY3_D1]       = AmbaCSL_DdrcGetByte0D1Dly(Hid, 3U);
            pDramParam[DRAM_TRAIN_PARAM_OFST_B0_DLY4_D1]       = AmbaCSL_DdrcGetByte0D1Dly(Hid, 4U);
            pDramParam[DRAM_TRAIN_PARAM_OFST_B0_DLY5_D1]       = AmbaCSL_DdrcGetByte0D1Dly(Hid, 5U);
            pDramParam[DRAM_TRAIN_PARAM_OFST_B1_DLY0_D1]       = AmbaCSL_DdrcGetByte1D1Dly(Hid, 0U);
            pDramParam[DRAM_TRAIN_PARAM_OFST_B1_DLY1_D1]       = AmbaCSL_DdrcGetByte1D1Dly(Hid, 1U);
            pDramParam[DRAM_TRAIN_PARAM_OFST_B1_DLY2_D1]       = AmbaCSL_DdrcGetByte1D1Dly(Hid, 2U);
            pDramParam[DRAM_TRAIN_PARAM_OFST_B1_DLY3_D1]       = AmbaCSL_DdrcGetByte1D1Dly(Hid, 3U);
            pDramParam[DRAM_TRAIN_PARAM_OFST_B1_DLY4_D1]       = AmbaCSL_DdrcGetByte1D1Dly(Hid, 4U);
            pDramParam[DRAM_TRAIN_PARAM_OFST_B1_DLY5_D1]       = AmbaCSL_DdrcGetByte1D1Dly(Hid, 5U);
            pDramParam[DRAM_TRAIN_PARAM_OFST_B2_DLY0_D1]       = AmbaCSL_DdrcGetByte2D1Dly(Hid, 0U);
            pDramParam[DRAM_TRAIN_PARAM_OFST_B2_DLY1_D1]       = AmbaCSL_DdrcGetByte2D1Dly(Hid, 1U);
            pDramParam[DRAM_TRAIN_PARAM_OFST_B2_DLY2_D1]       = AmbaCSL_DdrcGetByte2D1Dly(Hid, 2U);
            pDramParam[DRAM_TRAIN_PARAM_OFST_B2_DLY3_D1]       = AmbaCSL_DdrcGetByte2D1Dly(Hid, 3U);
            pDramParam[DRAM_TRAIN_PARAM_OFST_B2_DLY4_D1]       = AmbaCSL_DdrcGetByte2D1Dly(Hid, 4U);
            pDramParam[DRAM_TRAIN_PARAM_OFST_B2_DLY5_D1]       = AmbaCSL_DdrcGetByte2D1Dly(Hid, 5U);
            pDramParam[DRAM_TRAIN_PARAM_OFST_B3_DLY0_D1]       = AmbaCSL_DdrcGetByte3D1Dly(Hid, 0U);
            pDramParam[DRAM_TRAIN_PARAM_OFST_B3_DLY1_D1]       = AmbaCSL_DdrcGetByte3D1Dly(Hid, 1U);
            pDramParam[DRAM_TRAIN_PARAM_OFST_B3_DLY2_D1]       = AmbaCSL_DdrcGetByte3D1Dly(Hid, 2U);
            pDramParam[DRAM_TRAIN_PARAM_OFST_B3_DLY3_D1]       = AmbaCSL_DdrcGetByte3D1Dly(Hid, 3U);
            pDramParam[DRAM_TRAIN_PARAM_OFST_B3_DLY4_D1]       = AmbaCSL_DdrcGetByte3D1Dly(Hid, 4U);
            pDramParam[DRAM_TRAIN_PARAM_OFST_B3_DLY5_D1]       = AmbaCSL_DdrcGetByte3D1Dly(Hid, 5U);
        }

#if defined(CONFIG_DRAM_TYPE_LPDDR5)
        /* MR 12 */
        // Get settings from C0/C1 D0
        AmbaCSL_DdrcSetModeReg(Hid, 0x9a0c0000);
        while (0x0U != (AmbaCSL_DdrcGetModeReg(Hid) & 0x80000000U)) {
            ;
        }
        Delay = AmbaCSL_DdrcGetModeReg(Hid);
        Delay = (Delay & 0xffU);
        pDramParam[DRAM_TRAIN_PARAM_OFST_MR12C0D0] = ((0x8b0c0000U) | (Delay & 0xffU));
        Delay = AmbaCSL_DdrcGetModeReg(Hid);
        Delay = ((Delay & 0xff00U) >> 0x8U);
        pDramParam[DRAM_TRAIN_PARAM_OFST_MR12C1D0] = ((0x930c0000U) | (Delay & 0xffU));
        // Get settings from C0/C1 D1
        if(BLD_Check_DRAM_DIE1_exist() == 1) {
            AmbaCSL_DdrcSetModeReg(Hid, 0x9c0c0000);
            while (0x0U != (AmbaCSL_DdrcGetModeReg(Hid) & 0x80000000U)) {
                ;
            }
            Delay = AmbaCSL_DdrcGetModeReg(Hid);
            Delay = (Delay & 0xffU);
            pDramParam[DRAM_TRAIN_PARAM_OFST_MR12C0D1] = ((0x8d0c0000U) | (Delay & 0xffU));
            Delay = AmbaCSL_DdrcGetModeReg(Hid);
            Delay = ((Delay & 0xff00U) >> 0x8U);
            pDramParam[DRAM_TRAIN_PARAM_OFST_MR12C1D1] = ((0x950c0000U) | (Delay & 0xffU));
        }
#endif  // End of CONFIG_DRAM_TYPE_LPDDR5

        /* MR 14 */
        // Get settings from C0/C1 D0
        AmbaCSL_DdrcSetModeReg(Hid, 0x9a0e0000);
        while (0x0U != (AmbaCSL_DdrcGetModeReg(Hid) & 0x80000000U)) {
            ;
        }
        Delay = AmbaCSL_DdrcGetModeReg(Hid);
        Delay = (Delay & 0xffU);
        pDramParam[DRAM_TRAIN_PARAM_OFST_MR14C0D0] = ((0x8b0e0000U) | (Delay & 0xffU));
        Delay = AmbaCSL_DdrcGetModeReg(Hid);
        Delay = ((Delay & 0xff00U) >> 0x8U);
        pDramParam[DRAM_TRAIN_PARAM_OFST_MR14C1D0] = ((0x930e0000U) | (Delay & 0xffU));
        // Get settings from C0/C1 D1
        if(BLD_Check_DRAM_DIE1_exist() == 1) {
            AmbaCSL_DdrcSetModeReg(Hid, 0x9c0e0000);
            while (0x0U != (AmbaCSL_DdrcGetModeReg(Hid) & 0x80000000U)) {
                ;
            }
            Delay = AmbaCSL_DdrcGetModeReg(Hid);
            Delay = (Delay & 0xffU);
            pDramParam[DRAM_TRAIN_PARAM_OFST_MR14C0D1] = ((0x8d0e0000U) | (Delay & 0xffU));
            Delay = AmbaCSL_DdrcGetModeReg(Hid);
            Delay = ((Delay & 0xff00U) >> 0x8U);
            pDramParam[DRAM_TRAIN_PARAM_OFST_MR14C1D1] = ((0x950e0000U) | (Delay & 0xffU));
        }

#if defined(CONFIG_DRAM_TYPE_LPDDR5)
        /* MR 15 */
        // Get settings from C0/C1 D0
        AmbaCSL_DdrcSetModeReg(Hid, 0x9a0f0000);
        while (0x0U != (AmbaCSL_DdrcGetModeReg(Hid) & 0x80000000U)) {
            ;
        }
        Delay = AmbaCSL_DdrcGetModeReg(Hid);
        Delay = (Delay & 0xffU);
        pDramParam[DRAM_TRAIN_PARAM_OFST_MR15C0D0] = ((0x8b0f0000U) | (Delay & 0xffU));
        Delay = AmbaCSL_DdrcGetModeReg(Hid);
        Delay = ((Delay & 0xff00U) >> 0x8U);
        pDramParam[DRAM_TRAIN_PARAM_OFST_MR15C1D0] = ((0x930f0000U) | (Delay & 0xffU));
        // Get settings from C0/C1 D1
        if(BLD_Check_DRAM_DIE1_exist() == 1) {
            AmbaCSL_DdrcSetModeReg(Hid, 0x9c0f0000);
            while (0x0U != (AmbaCSL_DdrcGetModeReg(Hid) & 0x80000000U)) {
                ;
            }
            Delay = AmbaCSL_DdrcGetModeReg(Hid);
            Delay = (Delay & 0xffU);
            pDramParam[DRAM_TRAIN_PARAM_OFST_MR15C0D1] = ((0x8d0f0000U) | (Delay & 0xffU));
            Delay = AmbaCSL_DdrcGetModeReg(Hid);
            Delay = ((Delay & 0xff00U) >> 0x8U);
            pDramParam[DRAM_TRAIN_PARAM_OFST_MR15C1D1] = ((0x950f0000U) | (Delay & 0xffU));
        }

        /* MR 30 */
        // Get settings from C0 D0
        /*
        AmbaCSL_DdrcSetModeReg(Hid, 0x8a1e0000);
        while (0x0U != (AmbaCSL_DdrcGetModeReg(Hid) & 0x80000000U)) {
            ;
        }
        Delay = AmbaCSL_DdrcGetModeReg(Hid);
        Delay = (Delay & 0xffU);
        pDramParam[DRAM_TRAIN_PARAM_OFST_MR30] = ((0x9f1e0000U) | (Delay & 0xffU));
        */

        // Get settings from system scratchpad memory
        if (BLD_Read32(DDRCT_RESULT_MR30_VALID) == DDRCT_RESULT_VALID_MAGIC_CODE) {
            Delay = BLD_Read32(DDRCT_RESULT_MR30_ADDR(Hid, 0, 0));
            pDramParam[DRAM_TRAIN_PARAM_OFST_MR30C0D0] = ((0x8b1e0000U) | (Delay & 0xffU));
            Delay = BLD_Read32(DDRCT_RESULT_MR30_ADDR(Hid, 0, 1));
            pDramParam[DRAM_TRAIN_PARAM_OFST_MR30C1D0] = ((0x931e0000U) | (Delay & 0xffU));
            if(BLD_Check_DRAM_DIE1_exist() == 1) {
                Delay = BLD_Read32(DDRCT_RESULT_MR30_ADDR(Hid, 1, 0));
                pDramParam[DRAM_TRAIN_PARAM_OFST_MR30C0D1] = ((0x8d1e0000U) | (Delay & 0xffU));
                Delay = BLD_Read32(DDRCT_RESULT_MR30_ADDR(Hid, 1, 1));
                pDramParam[DRAM_TRAIN_PARAM_OFST_MR30C1D1] = ((0x951e0000U) | (Delay & 0xffU));
            }
        } else {
            pDramParam[DRAM_TRAIN_PARAM_OFST_MR30C1D0] = pDramParam[DRAM_TRAIN_PARAM_OFST_MR30C0D0] = 0;
            pDramParam[DRAM_TRAIN_PARAM_OFST_MR30C1D1] = pDramParam[DRAM_TRAIN_PARAM_OFST_MR30C0D1] = 0;
        }

#endif  // End of CONFIG_DRAM_TYPE_LPDDR5
    }

    ddrc_save_wck2dqx(pDramParam);

}
#endif
