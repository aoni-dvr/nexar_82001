/**
 *  @file AmbaBLD_eMMC.c
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
 *  @details Bootloader for NAND Boot
 *
 */

#include "AmbaTypes.h"
#include "AmbaMisraFix.h"
#include "AmbaIOUtility.h"
#include "AmbaWrap.h"

#include "AmbaBLD.h"
#if !defined(CONFIG_BUILD_QNX_IPL)
#include "AmbaBLD_UserCallBack.h"
#include "AmbaTimerInfo.h"
#endif
#include "AmbaNVM_Partition.h"
#include "AmbaSD.h"
#include "AmbaSD_Ctrl.h"

#include "AmbaRTSL_GPIO.h"
#include "AmbaRTSL_CPU.h"
#include "AmbaRTSL_Cache.h"
#include "AmbaRTSL_MMU.h"
#include "AmbaRTSL_PLL.h"
#include "AmbaRTSL_UART.h"
#include "AmbaRTSL_PWC.h"

#include "AmbaRTSL_SD.h"

#include "AmbaCSL_Scratchpad.h"

#include <AmbaINT_Def.h>
#include <AmbaRTSL_GIC.h>

#include <AmbaATF.h>

extern const AMBA_PARTITION_CONFIG_s AmbaNAND_SysPartConfig[AMBA_NUM_SYS_PARTITION];
extern const AMBA_PARTITION_CONFIG_s AmbaNAND_UserPartConfig[AMBA_NUM_USER_PARTITION];
extern AMBA_GPIO_DEFAULT_s GpioPinGrpConfig;

static UINT8 *FwStartEL1 = NULL;
#ifndef CONFIG_ATF_HAVE_BL2
static UINT8 *FwStartEL3 = NULL;
#endif

#define PRINT_BUF_SIZE  (1024U)

static void BLD_PrintStr(const char *pFmt)
{
    static UINT8 PrintBuf[PRINT_BUF_SIZE] GNU_SECTION_NOZEROINIT;
    UINT32 ActualTxSize, UartTxSize, Index = 0U;
    const char *pArg = NULL;

    UartTxSize = IO_UtilityStringPrintStr((char *)PrintBuf, (UINT32)PRINT_BUF_SIZE, pFmt, 1U, &pArg);
    while (UartTxSize > 0U) {
        if (AmbaRTSL_UartWrite(AMBA_UART_APB_CHANNEL0, UartTxSize, &PrintBuf[Index], &ActualTxSize) != UART_ERR_NONE) {
            break;
        } else {
            UartTxSize -= ActualTxSize;
            Index += ActualTxSize;
        }
    }
}

/**
 *  @RoutineName:: BLD_EraseNvm
 *
 *  @Description:: Erase entire NVM
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
 */
void BLD_EraseNvm(void)
{
    UINT32 BlkAddr, BlkCount,  EraseSecCount = AmbaRTSL_SdGetWorkBufferSize() / 512U;
    UINT8 *pWorkBufMain = AmbaRTSL_SdWorkBuf;
    const AMBA_SD_CARD_INFO_s *pCardInfo = &AmbaSD_Ctrl[0].CardInfo;
    AMBA_NVM_SECTOR_CONFIG_s SecConfig;
    AMBA_PARTITION_ENTRY_s *pEntryCalib = AmbaRTSL_EmmcGetPartEntry(1U, AMBA_USER_PARTITION_CALIBRATION_DATA);
    AMBA_PARTITION_ENTRY_s *pEntryStg0  = AmbaRTSL_EmmcGetPartEntry(1U, AMBA_USER_PARTITION_FAT_DRIVE_A);
    UINT32 TableSize, i;
    UINT8 *pTmp = NULL;

    (void) AmbaWrap_memset(AmbaRTSL_SdWorkBuf, 0x0, AmbaRTSL_SdGetWorkBufferSize());

    BlkCount = pCardInfo->MemCardInfo.TotalSectors;
    for (BlkAddr = 0; BlkAddr < BlkCount; BlkAddr += SecConfig.NumSector) {

        if ((BlkAddr >= pEntryCalib->StartBlkAddr) &&
            (BlkAddr < (pEntryCalib->StartBlkAddr + pEntryCalib->BlkCount))) {
            BLD_PrintStr("\r\nSkip calibration partition\r\n");
            BlkAddr = pEntryCalib->StartBlkAddr + pEntryCalib->BlkCount;
        }

        if ((BlkAddr >= pEntryStg0->StartBlkAddr) &&
            (BlkAddr < (pEntryStg0->StartBlkAddr + pEntryStg0->BlkCount))) {
            BLD_PrintStr("\r\nSkip Storage0 partition\r\n");
            BlkAddr = pEntryStg0->StartBlkAddr + pEntryStg0->BlkCount;
        }


        if (BlkAddr == BlkCount) {
            break;
        } else if (BlkAddr + EraseSecCount > BlkCount) {
            EraseSecCount = BlkCount - BlkAddr - EraseSecCount;
        } else {
            /* For MisraC */
        }

        SecConfig.NumSector   = EraseSecCount;
        SecConfig.pDataBuf    = (UINT8 *) pWorkBufMain;
        SecConfig.StartSector = BlkAddr;

        AmbaRTSL_SdWriteSector(AMBA_SD_CHANNEL0, &SecConfig);
    }

    TableSize = sizeof(AMBA_SYS_PARTITION_TABLE_s);
    AmbaMisra_TypeCast64(&pTmp, &pAmbaRTSL_EmmcSysPartTable );
    for (i = 0U; i < TableSize; i++) {
        pTmp[i] = 0xff;
    }

    TableSize = sizeof(AMBA_USER_PARTITION_TABLE_s);
    AmbaMisra_TypeCast64(&pTmp, &pAmbaRTSL_EmmcUserPartTable );
    for (i = 0U; i < TableSize; i++) {
        pTmp[i] = 0xff;
    }

    BLD_PrintStr("\r\nerase done\r\n");
}

UINT32 BLD_EraseNvmPart(UINT32 IsUserPart, UINT32 PartID)
{
    UINT32 RetVal = BLD_ERR_NONE;
    /* Force to erase */
    pAmbaRTSL_EmmcUserPartTable ->Entry[PartID].Attribute &= ~AMBA_PARTITION_ATTR_ERASED;

    RetVal = AmbaRTSL_EmmcErasePartition(IsUserPart, PartID);

    return RetVal;
}

/**
 *  @RoutineName:: AmbaBLD_GetSysPartTable
 *
 *  @Description:: Get system partition table
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          AMBA_SYS_PARTITION_TABLE_s * : pointer to the system partition table/NG(NULL)
 */
AMBA_SYS_PARTITION_TABLE_s *AmbaBLD_GetSysPartTable(void)
{
    return pAmbaRTSL_EmmcSysPartTable ;
}

/**
 *  @RoutineName:: AmbaBLD_GetUserPartTable
 *
 *  @Description:: Get system partition table
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          AMBA_USER_PARTITION_TABLE_s * : pointer to the user partition table/NG(NULL)
 */
AMBA_USER_PARTITION_TABLE_s *AmbaBLD_GetUserPartTable(void)
{
    return pAmbaRTSL_EmmcUserPartTable ;
}

/**
 *  @RoutineName:: BLD_LoadNvmPart
 *
 *  @Description:: Load one NVM partition
 *
 *  @Input      ::
 *      PartFlag: 0 - System Partition; 1 - User Partition
 *      PartID:   NAND flash partition ID
 *
 *  @Output     ::
 *      pDataBuf: pointer to partition data
 *
 *  @Return     :: none
 */
void BLD_LoadNvmPart(UINT32 PartFlag, UINT32 PartID, UINT8 *pDataBuf)
{
    const AMBA_PARTITION_ENTRY_s *pPartEntry;

    pPartEntry = AmbaRTSL_EmmcGetPartEntry(PartFlag, PartID);
    if ((pPartEntry != NULL) && (pPartEntry->ActualByteSize != 0U)) {
        if (AmbaRTSL_EmmcReadPartition(PartFlag, PartID, pDataBuf) != BLD_ERR_NONE) {
            BLD_PrintStr("Load partition failed!\r\n#\r\n");
        }
    }
}

/**
 *  @RoutineName:: BLD_WriteNvmPart
 *
 *  @Description:: Write one NVM partition
 *
 *  @Input      ::
 *      PartFlag: 0 - System Partition; 1 - User Partition
 *      PartID:   NAND flash partition ID
 *      DataSize: Data size in Byte to write to the partition
 *      pDataBuf: pointer to partition data
 *
 *  @Output     :: none
 *
 *  @Return     :: none
 */
void BLD_WriteNvmPart(UINT32 PartFlag, UINT32 PartID, UINT32 DataSize, UINT8 *pDataBuf)
{
    const AMBA_PARTITION_ENTRY_s *pPartEntry;

    pPartEntry = AmbaRTSL_EmmcGetPartEntry(PartFlag, PartID);
    if ((pPartEntry != NULL) && (pPartEntry->ActualByteSize != 0U)) {
        if (AmbaRTSL_EmmcWritePartition(PartFlag, PartID, DataSize, pDataBuf) != BLD_ERR_NONE) {
            BLD_PrintStr("Write partition failed!\r\n#\r\n");
        }
    }
}

UINT32 BLD_WriteDdrcData(UINT8 *pDataBuf)
{
    UINT32 RetVal = OK;
    AMBA_PARTITION_ENTRY_s * pPartEntry;
    AMBA_NVM_SECTOR_CONFIG_s SecConfig;

    pPartEntry = AmbaRTSL_EmmcGetPartEntry(0U, AMBA_SYS_PARTITION_BOOTSTRAP);
    if (pPartEntry->BlkCount != 0U) {
        SecConfig.NumSector   = 1;
        SecConfig.pDataBuf    = pDataBuf;
        SecConfig.StartSector = pPartEntry->BlkCount - 1;

        AmbaRTSL_SdWriteSector(AMBA_SD_CHANNEL0, &SecConfig);
    }

    return RetVal;
}

UINT32 BLD_LoadDdrcData(UINT8 *pDataBuf)
{
    UINT32 RetVal = OK;
    AMBA_PARTITION_ENTRY_s * pPartEntry;
    AMBA_NVM_SECTOR_CONFIG_s SecConfig;

    pPartEntry = AmbaRTSL_EmmcGetPartEntry(0U, AMBA_SYS_PARTITION_BOOTSTRAP);
    if (pPartEntry->BlkCount != 0U) {
        SecConfig.NumSector   = 1;
        SecConfig.pDataBuf    = pDataBuf;
        SecConfig.StartSector = pPartEntry->BlkCount - 1;

        RetVal = AmbaRTSL_SdReadSector(AMBA_SD_CHANNEL0, &SecConfig);
    }

    return RetVal;
}

UINT32 BLD_InitBootDevice(void)
{
    AMBA_SD_CONFIG_s SdConfig = {0};
    UINT32 Rval;
    extern void AmbaUserSD_PowerCtrl(UINT32, UINT32);

    (void) AmbaRTSL_SdInit();

    (void) AmbaWrap_memset(&SdConfig, 0, sizeof(AMBA_SD_CONFIG_s));
    SdConfig.PowerCtrl               = NULL;
    SdConfig.PhyCtrl                 = NULL;
    SdConfig.SdSetting.ClockDrive    = AMBA_SD_DRIVE_STRENGTH_12MA;
    SdConfig.SdSetting.DataDrive     = AMBA_SD_DRIVE_STRENGTH_12MA;
    SdConfig.SdSetting.CMDDrive      = AMBA_SD_DRIVE_STRENGTH_12MA;
    SdConfig.SdSetting.WPDrive       = AMBA_SD_DRIVE_STRENGTH_12MA;
    SdConfig.SdSetting.CDDrive       = AMBA_SD_DRIVE_STRENGTH_12MA;
    SdConfig.SdSetting.InitFrequency = 300000U;
#ifdef CONFIG_EMMC_MAX_CLOCK
    SdConfig.SdSetting.MaxFrequency  = CONFIG_EMMC_MAX_CLOCK;
#else
    SdConfig.SdSetting.MaxFrequency  = 10000000U;
#endif
    /* if you're using emmc mounted on adapter,
       please set this MaxFreq to lower (ex.12MHz) since signal quality issue. */

    SdConfig.SmpCoreSet              = 0x1U;
    SdConfig.Priority                = 32U;
    SdConfig.pSysPartConfig          = (AMBA_PARTITION_CONFIG_s *)AmbaNAND_SysPartConfig;
    SdConfig.pUserPartConfig         = (AMBA_PARTITION_CONFIG_s *)AmbaNAND_UserPartConfig;

    (void)AmbaRTSL_GpioSetFuncAlt(GPIO_PIN_81_SD0_RESET);

    Rval  = AmbaRTSL_SdConfig(AMBA_SD_CHANNEL0, &SdConfig);
    Rval |= AmbaRTSL_SdCardInit(AMBA_SD_CHANNEL0);

    if (Rval != OK) {
        BLD_PrintStr("BLD_InitBootDevice: AmbaRTSL_SdCardInit failed!\r\n#\r\n");
    } else {
        INT32 cRval;
        cRval = AmbaRTSL_EmmcInitPtbBbt();
        if (cRval != OK) {
            BLD_PrintStr("BLD_InitBootDevice: AmbaRTSL_EmmcInitPtbBbt failed!\r\n#\r\n");
        }
    }

    return Rval;
}

/**
 *  @RoutineName:: BLD_LoadFirmware
 *
 *  @Description:: Erase entire NVM
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
 */
void BLD_LoadFirmware(void)
{
    const AMBA_PARTITION_ENTRY_s *pPartEntry = NULL;
    UINT32 ActivePTB;

    if (AmbaRTSL_EmmcGetActivePtbNo(&ActivePTB) != 0U) {
        BLD_PrintStr("Get Active User PTB Fail\r\n");
    } else {
        if (0U == ActivePTB) {
            BLD_PrintStr("# Active User PTB 0\r\n");
        } else {
            BLD_PrintStr("# Active User PTB 1\r\n");
        }
    }

    if(AmbaRTSL_EmmcReadUserPTB(NULL, ActivePTB) != 0U) {
        BLD_PrintStr("Read User PTB Fail\r\n");
    }

#if !defined(CONFIG_BUILD_QNX_IPL)
    if (AmbaBldUserCallbacks.UserCbBefLoadFw != NULL) {
        AmbaBldUserCallbacks.UserCbBefLoadFw();
    }
    AmbaTime_A53Bld(A53_BLD_LOAD_START);
#endif
#if !defined(CONFIG_ATF_HAVE_BL2)
    /* Load EL3 firmware image */
    BLD_PrintStr("# Load ATF\r\n");
#if !defined(CONFIG_BUILD_QNX_IPL)
    AmbaTime_A53Bld(A53_BLD_LOAD_ATF_START);
#endif
    pPartEntry = AmbaRTSL_EmmcGetPartEntry(0U, AMBA_SYS_PARTITION_ARM_TRUST_FW);
    if ((pPartEntry != NULL) && (pPartEntry->ActualByteSize != 0U)) {
        AmbaMisra_TypeCast32(&FwStartEL3, &pPartEntry->RamLoadAddr);
        if (AmbaRTSL_EmmcReadPartition(0, AMBA_SYS_PARTITION_ARM_TRUST_FW, FwStartEL3) != BLD_ERR_NONE) {
            BLD_PrintStr("# ATF image is not found\r\n#\r\n");
        }
    }
#endif /* !defined(CONFIG_ATF_HAVE_BL2) */
#if !defined(CONFIG_BUILD_QNX_IPL)
    AmbaTime_A53Bld(A53_BLD_LOAD_ATF_DONE);
#endif

    /* Load alternate EL1 firmware image */
    if (AmbaRTSL_EmmcIsBldMagicCodeSet() != 0U) {
        BLD_PrintStr("# Load Firmware Updater\r\n");
#if !defined(CONFIG_BUILD_QNX_IPL)
        AmbaTime_A53Bld(A53_BLD_LOAD_FWUPDATER_START);
#endif
        pPartEntry = AmbaRTSL_EmmcGetPartEntry(0U, AMBA_SYS_PARTITION_FW_UPDATER);
        if ((pPartEntry != NULL) && (pPartEntry->ActualByteSize != 0U)) {
            AmbaMisra_TypeCast32(&FwStartEL1, &pPartEntry->RamLoadAddr);
            if (AmbaRTSL_EmmcReadPartition(0, AMBA_SYS_PARTITION_FW_UPDATER, FwStartEL1) != BLD_ERR_NONE) {
                BLD_PrintStr("# Firmware Updater image is not found\r\n#\r\n");
            }
#if !defined(CONFIG_BUILD_QNX_IPL)
            AmbaTime_A53Bld(A53_BLD_LOAD_FWUPDATER_DONE);
#endif
        }
    }

    if (FwStartEL1 == NULL) {
#if defined(CONFIG_DEVICE_TREE_SUPPORT)
        UINT8 *XtbImage = NULL;
        /* Load Xen/RTOS DTB */
        BLD_PrintStr("# Load XTB \r\n");
        pPartEntry = AmbaRTSL_EmmcGetPartEntry(1, AMBA_USER_PARTITION_XTB);
        if (pPartEntry == NULL) {
            BLD_PrintStr("# XTB part is NULL!\r\n");
        } else if (pPartEntry->ActualByteSize == 0U) {
            BLD_PrintStr("# XTB part is 0!\r\n");
        } else {
            AmbaMisra_TypeCast32(&XtbImage, &pPartEntry->RamLoadAddr);
            if (AmbaRTSL_EmmcReadPartition(1, AMBA_USER_PARTITION_XTB, XtbImage) != BLD_ERR_NONE) {
                BLD_PrintStr("# XTB image is not found!\r\n");
            }
#ifdef AMBA_BLD_CHECK_FW_CRC32
            BLD_PrintStr("# Check XTB image Crc32 \r\n#\r\n");
            Crc32 = IO_UtilityCrc32(XtbImage, pPartEntry->ActualByteSize);
            if (Crc32 != pPartEntry->ImageCRC32) {
                BldStop();
            }
#endif
#ifdef CONFIG_LINUX
            {
                void *xtb_ptr = NULL;
                extern int fdt_update_chosen(void *fdt);
                extern int fdt_update_emmc(void *fdt);

                AmbaMisra_TypeCast32(&xtb_ptr, &pPartEntry->RamLoadAddr);
                fdt_update_emmc(xtb_ptr);
                fdt_update_chosen(xtb_ptr);
            }
#endif
        }
#endif
        /* Load EL1 firmware image */
        BLD_PrintStr("# Load Kernel\r\n");
#if !defined(CONFIG_BUILD_QNX_IPL)
        AmbaTime_A53Bld(A53_BLD_LOAD_SYS_START);
#endif
#if defined(CONFIG_ATF_FIP_RTOS)
        /* should never be here */
        __asm__ volatile("b .");
#elif defined(CONFIG_BLD_SEQ_SRTOS) || defined(CONFIG_BLD_SEQ_ATF_SRTOS)
        pPartEntry = AmbaRTSL_EmmcGetPartEntry(1, AMBA_USER_PARTITION_SYS_SOFTWARE);
        if ((pPartEntry != NULL) && (pPartEntry->ActualByteSize != 0U)) {
            AmbaMisra_TypeCast32(&FwStartEL1, &pPartEntry->RamLoadAddr);
            if (AmbaRTSL_EmmcReadPartition(1, AMBA_USER_PARTITION_SYS_SOFTWARE, FwStartEL1) != BLD_ERR_NONE) {
                BLD_PrintStr("# Kernel image is not found\r\n#\r\n");
            }
        }
#if !defined(CONFIG_BUILD_QNX_IPL)
        AmbaTime_A53Bld(A53_BLD_LOAD_SYS_DONE);
#endif
#elif defined(CONFIG_BLD_SEQ_LINUX) || defined(CONFIG_BLD_SEQ_ATF_LINUX)
        pPartEntry = AmbaRTSL_EmmcGetPartEntry(1, AMBA_USER_PARTITION_LINUX_KERNEL);
        if ((pPartEntry != NULL) && (pPartEntry->ActualByteSize != 0U)) {
            FwStartEL1 = (UINT8 *) CONFIG_FWPROG_LNX_LOADADDR;
#if !defined(CONFIG_BUILD_QNX_IPL)
            AmbaTime_A53Bld(A53_BLD_LOAD_LINUX_START);
#endif
            if (AmbaRTSL_EmmcReadPartition(1, AMBA_USER_PARTITION_LINUX_KERNEL, FwStartEL1) != BLD_ERR_NONE) {
                BLD_PrintStr("# Kernel image is not found\r\n#\r\n");
            }

#if !defined(CONFIG_DEVICE_TREE_SUPPORT)
            /* Prepare dtb */
            {
                UINT32 Rval;
                UINT32 Block;
                UINT8 *dtbAddr = (UINT8 *)(CONFIG_FWPROG_LNX_LOADADDR & ~0xFFFFF);
                AMBA_NVM_SECTOR_CONFIG_s SecConfig;

#if !defined(CONFIG_BUILD_QNX_IPL)
                AmbaTime_A53Bld(A53_BLD_LOAD_LINUX_DONE);
#endif
                /* DTB is programmed to the last 128 sectors of Linux partition. */
                Block = pPartEntry->StartBlkAddr + pPartEntry->BlkCount - 128;

                SecConfig.NumSector   = 128;
                SecConfig.StartSector = Block;
                SecConfig.pDataBuf    = (void *)dtbAddr;

                Rval = AmbaRTSL_SdReadSector(AMBA_SD_CHANNEL0, &SecConfig);
                if (Rval != 0u) {
                    BLD_PrintStr("# Load DTB failed\r\n#\r\n");
                }
#ifdef CONFIG_ENABLE_AMBALINK
                else {
                    extern int fdt_update_memory(void *fdt);
                    extern int fdt_update_chosen(void *fdt);
                    extern int fdt_update_emmc(void *fdt);
                    fdt_update_emmc(dtbAddr);
                    fdt_update_chosen(dtbAddr);
                    fdt_update_memory(dtbAddr);
                }
#endif
            }
#else
#if !defined(CONFIG_BUILD_QNX_IPL)
            AmbaTime_A53Bld(A53_BLD_LOAD_LINUX_DONE);
#endif
#endif
        }
#elif defined(CONFIG_BLD_SEQ_SHELL)
        /* Do nothing */
#else /* CONFIG_BLD_SEQ_xxx */
#error Incomplete branch.
#endif /* CONFIG_BLD_SEQ */
    }
#if !defined(CONFIG_BUILD_QNX_IPL)
    if (AmbaBldUserCallbacks.UserCbAftLoadFw != NULL) {
        AmbaBldUserCallbacks.UserCbAftLoadFw();
    }
#endif
}

#if !defined(CONFIG_BLD_SEQ_ATF) && (defined(CONFIG_BOOT_CORE_LINUX) && (CONFIG_BOOT_CORE_LINUX == 0))
/* cf. arm64/booting.txt under linux/Documentation. */
static void bootLinux(UINT64 p0, UINT64 p1, UINT64 p2, UINT64 p3, void * addr)
{
    if (addr == NULL) {
        BLD_PrintStr("# Unable to enter kernel\r\n#\r\n");
    } else {
        extern void AmbaBootLinux(UINT64 p0, UINT64 p1, UINT64 p2, UINT64 p3, void *addr);

        AmbaBootLinux(p0, p1, p2, p3, addr);
    }
}
#endif

/* SMC parameters. */
typedef struct {
    UINT64 x0;  /**< Function ID */
    UINT64 x1;  /**< Parameter */
    UINT64 x2;  /**< Parameter */
    UINT64 x3;  /**< Parameter */
    UINT64 x4;  /**< Parameter */
    UINT64 x5;  /**< Parameter */
    UINT64 x6;  /**< Parameter or Optional Session ID */
    UINT64 x7;  /**< Optional Client/Secure-OS ID */
    /* More */
} AmbaSMC_param64_t;

#if !defined(CONFIG_ATF_FIP_RTOS) && defined(CONFIG_BLD_SEQ_SRTOS) && (CONFIG_BOOT_CORE_SRTOS > 0)
static AmbaSMC_param64_t param_SRTOSBoot;
#elif defined(CONFIG_BLD_SEQ_LINUX) && (CONFIG_BOOT_CORE_LINUX > 0)
static AmbaSMC_param64_t param_LinuxBoot;
#endif

/**
 *  @RoutineName:: BLD_EnterKernel
 *
 *  @Description:: Jump to kernel.
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
 */
void BLD_EnterKernel(void)
{
#if defined(CONFIG_ATF_FIP_RTOS) && (CONFIG_BOOT_CORE_SRTOS > 0) && (CONFIG_BOOT_CORE_LINUX > 0)
    /* core1 RTOS will boot core3 linux, suspend core0 BLD here */
    __asm__ volatile("b .");
#elif defined(CONFIG_ATF_FIP_RTOS) || defined(CONFIG_BLD_SEQ_LINUX)
    UINT32 SysCtrlReg;
    void (*FwEntry)(void) = NULL;
    /* Linux Kernel Image must be 2MB aligned */
    UINT64 dtbAddr = CONFIG_FWPROG_LNX_LOADADDR & ~0xFFFFF;

    FwStartEL1 = (UINT8 *) CONFIG_FWPROG_LNX_LOADADDR;
    AmbaMisra_TypeCast64(&FwEntry, &FwStartEL1);

    {
        AmbaRTSL_SdResetAll(AMBA_SD_CHANNEL0);

        (void)AmbaRTSL_GicIntDisable(AMBA_INT_SPI_ID104_SD);
    }

    // TODO: Fix unknown exception
    if (1) {
        void AmbaBLD_handler(void);

        register UINT32 CurrentEL;
        UINT64 VBAR = (UINT64)AmbaBLD_handler;

        CurrentEL = AmbaASM_ReadCurrentEL();
        CurrentEL = (CurrentEL >> 2U) & 0x3U;
        AmbaASM_WriteVectorBaseAddr(CurrentEL, VBAR);
    }

    // Enable IRQ to let handler clear it.
    __asm__ __volatile__("msr daifclr, #0xF":::"memory");
    SysCtrlReg = AmbaRTSL_CpuReadSysCtrl();
    if (0x0U != (SysCtrlReg & 0x4U)) {          /* If data caching is enabled */
        AmbaRTSL_CacheCleanDataAll();
    }
    AmbaMMU_Disable();
    // Disbale IRQ
    __asm__ __volatile__("msr daifset, #0xF":::"memory");

    /* TODO: Ensure to run under EL1H or AArch32 */
#if defined(CONFIG_BOOT_CORE_LINUX) && (CONFIG_BOOT_CORE_LINUX > 0)
    BLD_PrintStr("# Enter Linux\r\n#\r\n");
    {
        void AmbaSmcCall64(UINT64 p0, UINT64 p1, UINT64 p2, UINT64 p3, UINT64 p4);
        void AmbaBLD_PsciOnEntry_Linux(void);

        param_LinuxBoot.x0 = 0;

        AmbaSmcCall64(PSCI_CPU_ON_AARCH64, CONFIG_BOOT_CORE_LINUX, (UINT64)AmbaBLD_PsciOnEntry_Linux, (UINT64)&param_LinuxBoot, 0);
    }

    /* TODO: mode switch */
    param_LinuxBoot.x1 = dtbAddr;
    param_LinuxBoot.x2 = 0;
    param_LinuxBoot.x3 = 0;
    param_LinuxBoot.x4 = 0;
    param_LinuxBoot.x5 = 0;
    param_LinuxBoot.x6 = 0;
    param_LinuxBoot.x7 = 0;

    /* Loop will stop after value is assigned */
    param_LinuxBoot.x0 = (UINT64)FwEntry;
#else
#if defined(CONFIG_ATF_FIP_RTOS)
    /* RTOS core1 already booted by ATF, core0 jump to linux */
    do {
        AMBA_SEVL();
        AMBA_WFE();
        AMBA_WFE();
    } while (pAmbaScratchpadNS_Reg->AhbScratchpad[3] != 0xdeadbeef);
    pAmbaScratchpadNS_Reg->AhbScratchpad[3] = 0x0U;
#endif
    bootLinux(dtbAddr, 0u, 0u, 0u, FwEntry);
#endif /* defined(CONFIG_BOOT_CORE_LINUX) && (CONFIG_BOOT_CORE_LINUX > 0) */
    /* TODO: shell of bootloader will use NAND driver and trigger ISR. */
    __asm__ volatile("b .");
#elif !defined(CONFIG_ATF_HAVE_BL2)
    if (FwStartEL3 == NULL) {
        BLD_PrintStr("# Unable to enter kernel\r\n#\r\n");
    } else {
        UINT32 KernelStartAddr;

        /* Write RTOS start address and machine type flag to AHB Scratchpad Data1. */
        /* RTOS machine type (AArch32 or AArch64) should be programmed to partition too.) */
        AmbaMisra_TypeCast32(&KernelStartAddr, &FwStartEL1);
        pAmbaScratchpadNS_Reg->AhbScratchpad[0] = 0x0U;
        pAmbaScratchpadNS_Reg->AhbScratchpad[1] = (UINT32)(KernelStartAddr + 0x1U);
        pAmbaScratchpadNS_Reg->AhbScratchpad[2] = 0x0U;     /* Kernel parameter */
        pAmbaScratchpadNS_Reg->AhbScratchpad[3] = (UINT32)(CONFIG_SECURE_MEMORY_SIZE);

        BLD_PrintStr("# Enter ATF\r\n#\r\n");

#if !defined(CONFIG_BUILD_QNX_IPL)
        AmbaTime_A53Bld(A53_BLD_ENTER_ATF);
        if (AmbaBldUserCallbacks.UserCbBefEnterATF != NULL) {
            AmbaBldUserCallbacks.UserCbBefEnterATF();
        }
#endif
        AmbaRTSL_CacheCleanDataAll();
        AmbaMMU_Disable();
        __asm__ volatile ("br  %0": : "r" (FwStartEL3));
    }
#elif defined(CONFIG_BLD_SEQ_SRTOS)
    UINT32 SysCtrlReg;

    if (FwStartEL1 == NULL) {
        BLD_PrintStr("# Unable to enter kernel\r\n#\r\n");
    } else {
        /* TODO: SMC to boot secure-AArch32 */
        UINT32 KernelStartAddr;

        AmbaMisra_TypeCast32(&KernelStartAddr, &FwStartEL1);
        BLD_PrintStr("# Enter RTOS\r\n#\r\n");

        {
            AmbaRTSL_SdResetAll(AMBA_SD_CHANNEL0);

            (void)AmbaRTSL_GicIntDisable(AMBA_INT_SPI_ID104_SD);
        }

        // TODO: Fix unknown exception
        if (1) {
            void AmbaBLD_handler(void);

            register UINT32 CurrentEL;
            UINT64 VBAR = (UINT64)AmbaBLD_handler;

            CurrentEL = AmbaASM_ReadCurrentEL();
            CurrentEL = (CurrentEL >> 2U) & 0x3U;
            AmbaASM_WriteVectorBaseAddr(CurrentEL, VBAR);
        }

        // Enable IRQ to let handler clear it.
        __asm__ __volatile__("msr daifclr, #0xF":::"memory");
        SysCtrlReg = AmbaRTSL_CpuReadSysCtrl();
        if (0x0U != (SysCtrlReg & 0x4U)) {          /* If data caching is enabled */
            AmbaRTSL_CacheCleanDataAll();
        }
        AmbaMMU_Disable();
        // Disbale IRQ
        __asm__ __volatile__("msr daifset, #0xF":::"memory");

#if defined(CONFIG_BOOT_CORE_SRTOS) && (CONFIG_BOOT_CORE_SRTOS > 0)
        {
            void AmbaSmcCall64(UINT64 p0, UINT64 p1, UINT64 p2, UINT64 p3, UINT64 p4);
            void AmbaBLD_PsciOnEntry_SRTOS(void);

            param_SRTOSBoot.x0 = 0;

            AmbaSmcCall64(PSCI_CPU_ON_AARCH64, CONFIG_BOOT_CORE_SRTOS, (UINT64)AmbaBLD_PsciOnEntry_SRTOS, (UINT64)&param_SRTOSBoot, 0);
        }

        param_SRTOSBoot.x1 = (UINT64)KernelStartAddr;
        param_SRTOSBoot.x2 = 0;
        param_SRTOSBoot.x3 = S_SWITCH_AARCH32;
        param_SRTOSBoot.x4 = 0;
        param_SRTOSBoot.x5 = 0;
        param_SRTOSBoot.x6 = 0;
        param_SRTOSBoot.x7 = 0;

        /* Loop will stop after value is assigned */
        param_SRTOSBoot.x0 = AMBA_SIP_BOOT_RTOS;

        /* TODO: shell of bootloader will use NAND driver and trigger ISR. */
#if (defined(CONFIG_BOOT_CORE_LINUX) && (CONFIG_BOOT_CORE_LINUX == 0))
        {
            void (*FwEntry)(void) = NULL;
            UINT64 dtbAddr = CONFIG_FWPROG_LNX_LOADADDR & ~0xFFFFF;

            do {
                AMBA_SEVL();
                AMBA_WFE();
                AMBA_WFE();
            } while (pAmbaScratchpadNS_Reg->AhbScratchpad[3] != 0xdeadbeef);
            pAmbaScratchpadNS_Reg->AhbScratchpad[3] = 0x0U;

            FwStartEL1 = (UINT8 *) CONFIG_FWPROG_LNX_LOADADDR;

            AmbaMisra_TypeCast64(&FwEntry, &FwStartEL1);
            bootLinux(dtbAddr, 0u, 0u, 0u, FwEntry);
        }
#else
        __asm__ volatile("b .");
#endif  //(defined(CONFIG_BOOT_CORE_LINUX) && (CONFIG_BOOT_CORE_LINUX == 0))
#else
        {
            void AmbaSmcCall64(UINT64 p0, UINT64 p1, UINT64 p2, UINT64 p3, UINT64 p4);

            AmbaSmcCall64(AMBA_SIP_BOOT_RTOS, KernelStartAddr, 0, S_SWITCH_AARCH32, 0);
        }
#endif /* defined(CONFIG_BOOT_CORE_SRTOS) && (CONFIG_BOOT_CORE_SRTOS > 0) */
    }
#elif defined(CONFIG_BLD_SEQ_SHELL)
    /* Do nothing */
#else /* CONFIG_BLD_SEQ_xxx */
#error Incomplete branch.
#endif /* CONFIG_BLD_SEQ_xxx */
}

/**
 *  @RoutineName:: main
 *
 *  @Description:: main entry of bootloader.
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
 */
INT32 main(void)
{
    UINT32 Rval;
    UINT32 UartClk = 24000000U;
    static AMBA_UART_CONFIG_s AmbaUartConfig = {
        .NumDataBits   = AMBA_UART_DATA_8_BIT,
        .ParityBitMode = AMBA_UART_PARITY_NONE,
        .NumStopBits   = AMBA_UART_STOP_1_BIT,
    };

    AmbaMMU_SetupPrimary(0U);

#if !defined(CONFIG_BUILD_QNX_IPL)
    AmbaTimeInfo_Init();
    AmbaTime_A53Bld(A53_BLD_START);
#endif

#if defined(CONFIG_ATF_SPD_OPTEE)
    {
        /* NS BL33 cannot access pAmbaScratchpadS_Reg, set it to somewhere dummy */
        ULONG **ptr;
        ptr = (ULONG **)&pAmbaScratchpadS_Reg;
        *ptr = (ULONG *)CONFIG_ATF_BL2_BASE;
    }
#endif

    AmbaRTSL_PllInit(24000000U);
    AmbaRTSL_GpioLoadDefaultVals(&GpioPinGrpConfig);

    Rval  = AmbaRTSL_UartInit();
    Rval |= AmbaRTSL_PllGetIOClk(AMBA_CLK_UARTAPB, &UartClk);
    Rval |= AmbaRTSL_UartConfig(AMBA_UART_APB_CHANNEL0, UartClk, 115200U, &AmbaUartConfig);
    if (0U != Rval) {
        BLD_PrintStr("\r\n# UART and PLL config failed !\r\n#\r\n");
    }

    BLD_PrintStr("\r\n# 64-bit Boot-Up Success\r\n#\r\n");

#if !defined(CONFIG_BUILD_QNX_IPL)
    AmbaTime_A53Bld(A53_BLD_SHELL_START);
#endif

#ifdef CONFIG_BLD_SEQ_SHELL
    BLD_ShellStart(1U);
#else
    BLD_ShellStart(0U);
#endif

#if !defined(CONFIG_BUILD_QNX_IPL)
    AmbaTime_A53Bld(A53_BLD_POLL_USER_STOP);
    AmbaTime_BldGetBssInfo();
#endif

#if defined(CONFIG_ATF_FIP_RTOS) && (CONFIG_BOOT_CORE_SRTOS > 0) && (CONFIG_BOOT_CORE_LINUX == 0)
    BLD_EnterKernel();
#else
#if !defined(CONFIG_BUILD_QNX_IPL)
    AmbaTime_A53Bld(A53_BLD_DEV_INIT_START);
#endif
    if (BLD_ERR_NONE == BLD_InitBootDevice()) {
#if !defined(CONFIG_BUILD_QNX_IPL)
        AmbaTime_A53Bld(A53_BLD_DEV_INIT_DONE);
#endif
        BLD_CheckTrainingResult();
#if !defined(CONFIG_BUILD_QNX_IPL)
        AmbaTime_A53Bld(A53_BLD_CHK_DRAM_TRAIN_DONE);
#endif
        BLD_LoadFirmware();
        BLD_EnterKernel();
    } else {
        BLD_PrintStr("\r\n# BLD_InitBootDevice failed !\r\n#\r\n");
    }
#endif
    /* Force enter shell if load code failed */
    BLD_ShellStart(1U);

    BLD_PrintStr("# Trapped here..\r\n");

    return OK;
}

