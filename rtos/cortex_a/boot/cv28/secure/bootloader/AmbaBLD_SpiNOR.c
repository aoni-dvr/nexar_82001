/**
 *  @file AmbaBLD_SpiNOR.c
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
 *  @details Bootloader for SPI-NOR Boot
 *
 */

#include "AmbaTypes.h"
#include "AmbaMisraFix.h"
#include "AmbaIOUtility.h"
#include "AmbaWrap.h"

#include "AmbaBLD.h"
#include "AmbaNVM_Partition.h"
#if !defined(CONFIG_BUILD_QNX_IPL)
#include "AmbaBLD_UserCallBack.h"
#include "AmbaTimerInfo.h"
#endif

#include "AmbaRTSL_GPIO.h"
#include "AmbaRTSL_CPU.h"
#include "AmbaRTSL_Cache.h"
#include "AmbaRTSL_MMU.h"
#include "AmbaRTSL_PLL.h"
#include "AmbaRTSL_UART.h"
#include "AmbaRTSL_PWC.h"

#include "AmbaSpiNOR_Def.h"
#include "AmbaRTSL_SpiNOR.h"
#include "AmbaRTSL_SpiNOR_Ctrl.h"

#include "AmbaCSL_DMA.h"
#include "AmbaRTSL_DMA.h"

#include "AmbaCSL_Scratchpad.h"
#if defined(CONFIG_MUTI_BOOT_DEVICE)
#include "AmbaRTSL_SD.h"
#include "AmbaWrap.h"
#endif

#include <AmbaINT_Def.h>
#include <AmbaRTSL_GIC.h>

#include <AmbaATF.h>

extern const AMBA_NORSPI_DEV_INFO_s AmbaNORSPI_DevInfo;
extern const AMBA_PARTITION_CONFIG_s AmbaNORSPI_SysPartConfig[AMBA_NUM_SYS_PARTITION];
extern const AMBA_PARTITION_CONFIG_s AmbaNORSPI_UserPartConfig[AMBA_NUM_USER_PARTITION];
extern const AMBA_SERIAL_SPI_CONFIG_s AmbaNOR_SPISetting;
extern AMBA_GPIO_DEFAULT_s GpioPinGrpConfig;

static UINT8 *FwStartEL1 = NULL;
#ifndef CONFIG_ATF_HAVE_BL2
static UINT8 *FwStartEL3 = NULL;
#endif

#define PRINT_BUF_SIZE  (1024U)

static void BLD_PrintStr(const char *pFmt)
{
    static UINT8 BldPrintBuf[PRINT_BUF_SIZE] GNU_SECTION_NOZEROINIT;
    UINT32 ActualTxSize, UartTxSize, Index = 0U;
    char *pString;
    const UINT8 *pBuf = BldPrintBuf;
    const char *pArg = NULL;

    AmbaMisra_TypeCast64(&pString, &pBuf);
    UartTxSize = IO_UtilityStringPrintStr(pString, PRINT_BUF_SIZE, pFmt, (UINT32)1U, &pArg);
    while (UartTxSize > 0U) {
        if (AmbaRTSL_UartWrite(AMBA_UART_APB_CHANNEL0, UartTxSize, &BldPrintBuf[Index], &ActualTxSize) != UART_ERR_NONE) {
            break;
        } else {
            UartTxSize -= ActualTxSize;
            Index += ActualTxSize;
        }
    }
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
    return pAmbaRTSL_NorSpiSysPartTable;
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
    return pAmbaRTSL_NorSpiUserPartTable;
}

/**
 *  @RoutineName:: BLD_LoadNvmPart
 *
 *  @Description:: Load one NVM partition
 *
 *  @Input      ::
 *      PartFlag: 0 - System Partition; 1 - User Partition
 *      PartID:   flash partition ID
 *
 *  @Output     ::
 *      pDataBuf: pointer to partition data
 *
 *  @Return     :: none
 */
void BLD_LoadNvmPart(UINT32 PartFlag, UINT32 PartID, UINT8 *pDataBuf)
{
    const AMBA_PARTITION_ENTRY_s *pPartEntry;

    pPartEntry = AmbaRTSL_NorSpiGetPartEntry(PartFlag, PartID);
    if ((pPartEntry != NULL) && (pPartEntry->ActualByteSize != 0U)) {
        if (AmbaRTSL_NorSpiReadPartition(PartFlag, PartID, pDataBuf) != BLD_ERR_NONE) {
            BLD_PrintStr("Load partition failed!\r\n#\r\n");
        }
    }
}

static UINT8 BstBuffer[128 * 1024] GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE;

UINT32 BLD_WriteDdrcData(UINT8 *pDataBuf)
{
    UINT32 RetVal = OK;
    const AMBA_NORSPI_DEV_INFO_s *pNorDevInfo = AmbaRTSL_NorSpiDevInfo; /* Pointer to external NAND device information */
    UINT32 PageSize = pNorDevInfo->PageSize;
    UINT32 BlkByteSize = pNorDevInfo->EraseBlockSize;
    UINT32 Offset = (BlkByteSize - (2*PageSize));

    /* 1. Read BST data (block 0, page 0) */
    RetVal = AmbaRTSL_NOR_Readbyte(0, BlkByteSize, BstBuffer);

    for(UINT32 i=0 ; i<(2*PageSize); i++) {
        BstBuffer[i+Offset] = pDataBuf[i];
    }

    if (RetVal == OK) {
        /* 2. Erase the block 0 */
        RetVal = AmbaRTSL_NorErase(0, BlkByteSize);
    }

    /* 3. Write back block 0 data */
    if (RetVal == OK) {
        RetVal = AmbaRTSL_NorProgram(0, BlkByteSize, BstBuffer);
    }

    return RetVal;
}

UINT32 BLD_LoadDdrcData(UINT8 *pDataBuf)
{
    UINT32 RetVal = OK;
    AMBA_NORSPI_DEV_INFO_s *pNorDevInfo; /* Pointer to external NAND device information */
    UINT32 PageSize;

    pNorDevInfo = AmbaRTSL_NorSpiDevInfo;
    PageSize = pNorDevInfo->PageSize;
    RetVal = AmbaRTSL_NOR_Readbyte((pNorDevInfo->EraseBlockSize - (2*PageSize)), 2*PageSize, pDataBuf);

    return RetVal;
}

/**
 *  @RoutineName:: BLD_WriteNvmPart
 *
 *  @Description:: Write one NVM partition
 *
 *  @Input      ::
 *      PartFlag: 0 - System Partition; 1 - User Partition
 *      PartID:   flash partition ID
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

    if (0x0 == PartFlag) {
        pPartEntry = AmbaRTSL_NorSpiGetPartEntry(PartFlag, PartID);
        if ((pPartEntry != NULL) && (pPartEntry->ActualByteSize != 0U)) {
            if (AmbaRTSL_NorSpiWritePartition(PartFlag, PartID, DataSize, pDataBuf) != BLD_ERR_NONE) {
                BLD_PrintStr("Write partition failed!\r\n#\r\n");
            }
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
    UINT32 TotalByte = AmbaRTSL_NorSpiCtrl.TotalByteSize;
    UINT32 TableSize, i;
    UINT8 *pTmp = NULL;

    AmbaRTSL_NorErase(0, TotalByte);

    TableSize = sizeof(AMBA_SYS_PARTITION_TABLE_s);
    AmbaMisra_TypeCast64(&pTmp, &pAmbaRTSL_NorSpiSysPartTable);
    for (i = 0U; i < TableSize; i++) {
        pTmp[i] = 0xff;
    }

    TableSize = sizeof(AMBA_USER_PARTITION_TABLE_s);
    AmbaMisra_TypeCast64(&pTmp, &pAmbaRTSL_NorSpiUserPartTable);
    for (i = 0U; i < TableSize; i++) {
        pTmp[i] = 0xff;
    }

    BLD_PrintStr("\r\nerase done\r\n");
}

/**
 *  @RoutineName:: BLD_EraseNvmPart
 *
 *  @Description:: Erase specified partition
 *
 *  @Input      ::
 *              UINT32: target partition is in user partition or not
 *              UINT32: partition ID
 *
 *  @Output     ::
 *
 *  @Return     :: BLD error code
 */
UINT32 BLD_EraseNvmPart(UINT32 IsUserPart, UINT32 PartID)
{
    UINT32 RetVal = BLD_ERR_NONE;
    /* Force to erase */
    pAmbaRTSL_NorSpiUserPartTable->Entry[PartID].Attribute |= AMBA_PARTITION_ATTR_ERASED;

    RetVal = AmbaRTSL_NorSpiErasePartition(IsUserPart, PartID);

    return RetVal;
}

UINT32 BLD_InitBootDevice(void)
{
    AMBA_NOR_SPI_CONFIG_s NorSpiConfig;
    UINT32 RetVal = BLD_ERR_NONE;
    const AMBA_PARTITION_CONFIG_s * pPartConfigTmp;
    const AMBA_SERIAL_SPI_CONFIG_s * pSpiConfigTmp;
    const AMBA_NORSPI_DEV_INFO_s * pSpiDevTmp;
#if defined(CONFIG_MUTI_BOOT_DEVICE)
    extern void AmbaUserSD_PowerCtrl(UINT32, UINT32);
    AMBA_SD_CONFIG_s SdConfig;
#endif

    (void)AmbaWrap_memset(&NorSpiConfig, 0, sizeof(AMBA_NOR_SPI_CONFIG_s));
    pSpiDevTmp = &AmbaNORSPI_DevInfo;
    AmbaMisra_TypeCast64(&NorSpiConfig.pNorSpiDevInfo, &pSpiDevTmp);
    pPartConfigTmp = AmbaNORSPI_SysPartConfig;
    AmbaMisra_TypeCast64(&NorSpiConfig.pSysPartConfig, &pPartConfigTmp);
    pPartConfigTmp = AmbaNORSPI_UserPartConfig;
    AmbaMisra_TypeCast64(&NorSpiConfig.pUserPartConfig, &pPartConfigTmp);
    pSpiConfigTmp = &AmbaNOR_SPISetting;
    AmbaMisra_TypeCast64(&NorSpiConfig.SpiSetting, &pSpiConfigTmp);

    /* Set DMA channel to SpiNOR RX and TX */
    AmbaRTSL_DmaInit();
    AmbaRTSL_DmaSetChanType(AMBA_DMA_CHANNEL_0, AMBA_DMA_CHANNEL_NOR_SPI_RX);
    AmbaRTSL_DmaSetChanType(AMBA_DMA_CHANNEL_1, AMBA_DMA_CHANNEL_NOR_SPI_TX);
    AmbaRTSL_AllocateNorSpiDMAChan(AMBA_DMA_CHANNEL_0, AMBA_DMA_CHANNEL_1);

    (void) AmbaRTSL_SpiNORConfig(&NorSpiConfig);

#if defined(CONFIG_MUTI_BOOT_DEVICE)
    (void) AmbaRTSL_SdInit();

    (void)AmbaWrap_memset(&SdConfig, 0, sizeof(AMBA_SD_CONFIG_s));
    SdConfig.PowerCtrl           = AmbaUserSD_PowerCtrl;
    SdConfig.SdSetting.ClockDrive    = AMBA_SD_DRIVE_STRENGTH_12MA;
    SdConfig.SdSetting.DataDrive     = AMBA_SD_DRIVE_STRENGTH_12MA;
    SdConfig.SdSetting.CMDDrive      = AMBA_SD_DRIVE_STRENGTH_12MA;
    SdConfig.SdSetting.WPDrive       = AMBA_SD_DRIVE_STRENGTH_12MA;
    SdConfig.SdSetting.CDDrive       = AMBA_SD_DRIVE_STRENGTH_12MA;
    SdConfig.SdSetting.InitFrequency = 300000U;
    SdConfig.SdSetting.MaxFrequency  = CONFIG_EMMC_MAX_CLOCK; /* 100MHz */
    /* if you're using emmc mounted on adapter,
       please set this MaxFreq to lower (ex.12MHz) since signal quality issue. */

    (void)AmbaRTSL_SdConfig(AMBA_SD_CHANNEL0, &SdConfig);

    RetVal = AmbaRTSL_SdCardInit(AMBA_SD_CHANNEL0);
    if (RetVal != 0U) {
        BLD_PrintStr("\r\n# eMMC init Card fail#\r\n");
        RetVal = BLD_ERR_INFO;
    } else {
        BLD_PrintStr("\r\n# eMMC init Card OK #\r\n");
    }
#endif

    if (AmbaRTSL_NorSpiInitPtbBbt() != BLD_ERR_NONE) {
        RetVal = BLD_ERR_INFO;
    }
    return RetVal;
}

#if defined(CONFIG_MUTI_BOOT_DEVICE)
static inline UINT32 GetRoundUpValU32 (UINT32 Val, UINT32 NumDigits)
{
    return (0x0U != NumDigits) ? (((Val) + (NumDigits) - 0x1U) / (NumDigits)) : (Val);
}

INT32 BLD_ReadEmmcPartition(INT32 PartFlag, UINT32 PartID, UINT8 *pDataBuf)
{
    UINT32 BlkCount, BlkByteSize = 512U;
    const AMBA_PARTITION_ENTRY_s *pPartEntry = pPartEntry = AmbaRTSL_NorSpiGetPartEntry(PartFlag, PartID);
    AMBA_NVM_SECTOR_CONFIG_s   SecConfig;
    INT32 RetVal = -1;

    if ((pDataBuf != NULL) && (pPartEntry != NULL)) {

        BlkCount = GetRoundUpValU32(pPartEntry->ActualByteSize, BlkByteSize);
        {
            UINT32 ByteCount = 0U;
            UINT32 ReadCount = 2048U;
            UINT32 SecAddr;
            INT32 i;

            if (BlkCount > pPartEntry->BlkCount) {
                BlkCount = pPartEntry->BlkCount;  /* should never happen */
            }

            SecAddr = pPartEntry->StartBlkAddr;
            for (i = BlkCount; i > 0; i -= ReadCount) {
                if (BlkCount < ReadCount) {
                    SecConfig.NumSector   = BlkCount;
                } else {
                    SecConfig.NumSector  = ReadCount;
                }
                SecConfig.pDataBuf    = &pDataBuf[ByteCount];
                SecConfig.StartSector = SecAddr;

                RetVal = AmbaRTSL_SdReadSector(AMBA_SD_CHANNEL0, &SecConfig);

                if (RetVal < 0) {
                    break;
                } else {
                    ByteCount += SecConfig.NumSector * BlkByteSize;
                    SecAddr += ReadCount;
                }
            }
        }
    }
    return RetVal;
}
#endif

void BLD_LoadFirmware(void)
{
    const AMBA_PARTITION_ENTRY_s *pPartEntry = NULL;
    UINT32 ActivePTB;

    if (AmbaRTSL_NorSpiGetActivePtbNo(&ActivePTB) != 0U) {
        BLD_PrintStr("Get Active User PTB Fail\r\n");
    } else {
        if (0U == ActivePTB) {
            BLD_PrintStr("# Active User PTB 0\r\n");
        } else {
            BLD_PrintStr("# Active User PTB 1\r\n");
        }
    }

    if(AmbaRTSL_NorSpiReadUserPTB(NULL, ActivePTB) != 0U) {
        BLD_PrintStr("Read User PTB Fail\r\n");
    }
#if !defined(CONFIG_BUILD_QNX_IPL)
    if (AmbaBldUserCallbacks.UserCbBefLoadFw != NULL) {
        AmbaBldUserCallbacks.UserCbBefLoadFw();
    }
    AmbaTime_A53Bld(A53_BLD_LOAD_START);
#endif
#ifndef CONFIG_ATF_HAVE_BL2
    /* Load EL3 firmware image */
    BLD_PrintStr("# Load ATF\r\n");
#if !defined(CONFIG_BUILD_QNX_IPL)
    AmbaTime_A53Bld(A53_BLD_LOAD_ATF_START);
#endif
    pPartEntry = AmbaRTSL_NorSpiGetPartEntry(0, AMBA_SYS_PARTITION_ARM_TRUST_FW);
    if ((pPartEntry != NULL) && (pPartEntry->ActualByteSize != 0U)) {
        AmbaMisra_TypeCast32(&FwStartEL3, &pPartEntry->RamLoadAddr);
        if (AmbaRTSL_NorSpiReadPartition(0, AMBA_SYS_PARTITION_ARM_TRUST_FW, FwStartEL3) != BLD_ERR_NONE) {
            BLD_PrintStr("# ATF image is not found\r\n#\r\n");
        }
    }
#if !defined(CONFIG_BUILD_QNX_IPL)
    AmbaTime_A53Bld(A53_BLD_LOAD_ATF_DONE);
#endif
#endif /* CONFIG_ATF_HAVE_BL2 */

    /* Load alternate EL1 firmware image */
    if (AmbaRTSL_NorSpiIsBldMagicCodeSet() != 0U) {
        BLD_PrintStr("# Load Firmware Updater\r\n");
#if !defined(CONFIG_BUILD_QNX_IPL)
        AmbaTime_A53Bld(A53_BLD_LOAD_FWUPDATER_START);
#endif
        pPartEntry = AmbaRTSL_NorSpiGetPartEntry(0, AMBA_SYS_PARTITION_FW_UPDATER);
        if ((pPartEntry != NULL) && (pPartEntry->ActualByteSize != 0U)) {
            AmbaMisra_TypeCast32(&FwStartEL1, &pPartEntry->RamLoadAddr);
            if (AmbaRTSL_NorSpiReadPartition(0, AMBA_SYS_PARTITION_FW_UPDATER, FwStartEL1) != BLD_ERR_NONE) {
                BLD_PrintStr("# Firmware Updater image is not found\r\n#\r\n");
            }
        }
    }
#if !defined(CONFIG_BUILD_QNX_IPL)
    AmbaTime_A53Bld(A53_BLD_LOAD_FWUPDATER_DONE);
#endif

    if (FwStartEL1 == NULL) {
#if defined(CONFIG_DEVICE_TREE_SUPPORT)
        UINT8 *XtbImage = NULL;
        /* Load Xen/RTOS DTB */
        BLD_PrintStr("# Load XTB \r\n");
        pPartEntry = AmbaRTSL_NorSpiGetPartEntry(1, AMBA_USER_PARTITION_XTB);
        if (pPartEntry == NULL) {
            BLD_PrintStr("# XTB part is NULL!\r\n");
        } else if (pPartEntry->ActualByteSize == 0U) {
            BLD_PrintStr("# XTB part is 0!\r\n");
        } else {
            AmbaMisra_TypeCast32(&XtbImage, &pPartEntry->RamLoadAddr);

#if defined(CONFIG_MUTI_BOOT_DEVICE)
            if(((pPartEntry->Attribute & AMBA_PARTITION_ATTR_STORE_DEVIC) >> 6) == AMBA_NVM_SPI_NOR) {
                if (AmbaRTSL_NorSpiReadPartition(1, AMBA_USER_PARTITION_XTB, XtbImage) != OK) {
                    BLD_PrintStr("# XTB image on spinor is not found\r\n#\r\n");
                    __asm__ volatile("b .");
                }
            } else if(((pPartEntry->Attribute & AMBA_PARTITION_ATTR_STORE_DEVIC) >> 6) == AMBA_NVM_eMMC) {
                if (BLD_ReadEmmcPartition(1, AMBA_USER_PARTITION_XTB, XtbImage) != OK) {
                    BLD_PrintStr("# XTB image on emmc is not found\r\n#\r\n");
                    __asm__ volatile("b .");
                }
            } else {
                BLD_PrintStr("# XTB image load failed: Storage does not support\r\n#\r\n");
                __asm__ volatile("b .");
            }
#else
            if (AmbaRTSL_NorSpiReadPartition(1, AMBA_USER_PARTITION_XTB, XtbImage) != BLD_ERR_NONE) {
                BLD_PrintStr("# XTB image is not found\r\n#\r\n");
                __asm__ volatile("b .");
            }
#endif

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
                extern int fdt_update_spinor(void *fdt);

                AmbaMisra_TypeCast32(&xtb_ptr, &pPartEntry->RamLoadAddr);
                fdt_update_spinor(xtb_ptr);
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
        pPartEntry = AmbaRTSL_NorSpiGetPartEntry(1, AMBA_USER_PARTITION_SYS_SOFTWARE);
        if ((pPartEntry != NULL) && (pPartEntry->ActualByteSize != 0U)) {
            AmbaMisra_TypeCast32(&FwStartEL1, &pPartEntry->RamLoadAddr);
#if defined(CONFIG_MUTI_BOOT_DEVICE)
            if(((pPartEntry->Attribute & AMBA_PARTITION_ATTR_STORE_DEVIC) >> 6) == AMBA_NVM_SPI_NOR) {
                if (AmbaRTSL_NorSpiReadPartition(1, AMBA_USER_PARTITION_SYS_SOFTWARE, FwStartEL1) != OK) {
                    BLD_PrintStr("# Kernel image on spinor is not found\r\n#\r\n");
                }
            } else if(((pPartEntry->Attribute & AMBA_PARTITION_ATTR_STORE_DEVIC) >> 6) == AMBA_NVM_eMMC) {
                if (BLD_ReadEmmcPartition(1, AMBA_USER_PARTITION_SYS_SOFTWARE, FwStartEL1) != OK) {
                    BLD_PrintStr("# Kernel image on emmc is not found\r\n#\r\n");
                }
            } else {
                BLD_PrintStr("# Kernel image load failed: Storage does not support\r\n#\r\n");
            }
#else
            if (AmbaRTSL_NorSpiReadPartition(1, AMBA_USER_PARTITION_SYS_SOFTWARE, FwStartEL1) != OK) {
                BLD_PrintStr("# Kernel image is not found\r\n#\r\n");
            }
#endif
        }
#if !defined(CONFIG_BUILD_QNX_IPL)
        AmbaTime_A53Bld(A53_BLD_LOAD_SYS_DONE);
#endif
#elif defined(CONFIG_BLD_SEQ_LINUX) || defined(CONFIG_BLD_SEQ_ATF_LINUX)
#if !defined(CONFIG_BUILD_QNX_IPL)
        AmbaTime_A53Bld(A53_BLD_LOAD_LINUX_START);
#endif
        pPartEntry = AmbaRTSL_NorSpiGetPartEntry(1, AMBA_USER_PARTITION_LINUX_KERNEL);
        if ((pPartEntry != NULL) && (pPartEntry->ActualByteSize != 0U)) {
            FwStartEL1 = (UINT8 *) CONFIG_FWPROG_LNX_LOADADDR;
#if defined(CONFIG_MUTI_BOOT_DEVICE)
            if(((pPartEntry->Attribute & AMBA_PARTITION_ATTR_STORE_DEVIC) >> 6) == AMBA_NVM_SPI_NOR) {
                if (AmbaRTSL_NorSpiReadPartition(1, AMBA_USER_PARTITION_LINUX_KERNEL, FwStartEL1) != OK) {
                    BLD_PrintStr("# Kernel image is not found\r\n#\r\n");
                }
            } else if(((pPartEntry->Attribute & AMBA_PARTITION_ATTR_STORE_DEVIC) >> 6) == AMBA_NVM_eMMC) {
                if (BLD_ReadEmmcPartition(1, AMBA_USER_PARTITION_LINUX_KERNEL, FwStartEL1) != OK) {
                    BLD_PrintStr("# Kernel image is not found\r\n#\r\n");
                }
            } else {
                BLD_PrintStr("# Storage does not support\r\n#\r\n");
            }
#else
            if (AmbaRTSL_NorSpiReadPartition(1, AMBA_USER_PARTITION_LINUX_KERNEL, FwStartEL1) != OK) {
                BLD_PrintStr("# Kernel image is not found\r\n#\r\n");
            }
#if !defined(CONFIG_BUILD_QNX_IPL)
            AmbaTime_A53Bld(A53_BLD_LOAD_LINUX_DONE);
#endif
#endif /* defined(CONFIG_MUTI_BOOT_DEVICE) */

#if !defined(CONFIG_DEVICE_TREE_SUPPORT)
            /* Prepare dtb */
            {
                UINT32 Block;
                UINT32 Rval;
                UINT8 *dtbAddr = (UINT8 *)(CONFIG_FWPROG_LNX_LOADADDR & ~0xFFFFF);

                Block = pPartEntry->StartBlkAddr + pPartEntry->BlkCount - 1;
                Block *= AmbaNORSPI_DevInfo.EraseBlockSize;

#if defined(CONFIG_MUTI_BOOT_DEVICE)
#else
                Rval = AmbaRTSL_NOR_Readbyte(Block, AmbaNORSPI_DevInfo.EraseBlockSize,
                                             dtbAddr);

#endif /* defined(CONFIG_MUTI_BOOT_DEVICE) */

                if (Rval != OK) {
                    BLD_PrintStr("# Load DTB failed\r\n#\r\n");
                }
#ifdef CONFIG_ENABLE_AMBALINK
                else {
                    extern int fdt_update_memory(void *fdt);
                    extern int fdt_update_chosen(void *fdt);
                    extern int fdt_update_spinor(void *fdt);

                    fdt_update_spinor(dtbAddr);
                    fdt_update_chosen(dtbAddr);
                    fdt_update_memory(dtbAddr);
                }
#endif
            }
#endif
        }

#elif defined(CONFIG_BLD_SEQ_ATF_XEN)
        // TODO: proting from NAND
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
        AmbaRTSL_NorSpiStop();

        (void)AmbaRTSL_GicIntDisable(AMBA_INT_SPI_ID105_SPI_NOR);
        AmbaRTSL_DmaChanRelease(AmbaRTSL_NorSpiCtrl.NorRxDMAChannel);
        AmbaRTSL_DmaChanRelease(AmbaRTSL_NorSpiCtrl.NorTxDMAChannel);
        (void)AmbaRTSL_GicIntDisable(AMBA_INT_SPI_ID115_DMA0);
        (void)AmbaRTSL_GicIntDisable(AMBA_INT_SPI_ID116_DMA1);
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
    UINT32 SysCtrlReg;

    if (FwStartEL3 == NULL) {
        BLD_PrintStr("# Unable to enter kernel\r\n#\r\n");
    } else {
        UINT64 KernelStartAddr;
        void (*FwEntry)(void) = NULL;
        UINT32 *pTmp = (UINT32 *)&FwEntry;

        /* Write RTOS start address and machine type flag to AHB Scratchpad Data1. */
        /* RTOS machine type (AArch32 or AArch64) should be programmed to partition too.) */
        AmbaMisra_TypeCast64(&KernelStartAddr, &FwStartEL1);
        pAmbaScratchpadNS_Reg->AhbScratchpad[0] = 0x0U;
        pAmbaScratchpadNS_Reg->AhbScratchpad[1] = (UINT32)(KernelStartAddr + 0x1U);
        pAmbaScratchpadNS_Reg->AhbScratchpad[2] = 0x0U;     /* Kernel parameter */
        pAmbaScratchpadNS_Reg->AhbScratchpad[3] = (UINT32)(CONFIG_SECURE_MEMORY_SIZE);

        AmbaMisra_TypeCast64(&FwEntry, &FwStartEL3);
        AmbaMisra_TypeCast64(&KernelStartAddr, &pTmp);
        AmbaRTSL_CacheFlushData(KernelStartAddr, sizeof(UINT32 *));
        BLD_PrintStr("# Enter ATF\r\n#\r\n");
#if !defined(CONFIG_BUILD_QNX_IPL)
        AmbaTime_A53Bld(A53_BLD_ENTER_ATF);
        if (AmbaBldUserCallbacks.UserCbBefEnterATF != NULL) {
            AmbaBldUserCallbacks.UserCbBefEnterATF();
        }
#endif

        SysCtrlReg = AmbaRTSL_CpuReadSysCtrl();
        if (0x0U != (SysCtrlReg & 0x4U)) {          /* If data caching is enabled */
            AmbaRTSL_CacheCleanDataAll();
        }
        AmbaMMU_Disable();
        FwEntry();
    }
#elif defined(CONFIG_BLD_SEQ_SRTOS)
    UINT32 SysCtrlReg;

    if (FwStartEL1 == NULL) {
        BLD_PrintStr("# Unable to enter kernel\r\n#\r\n");
    } else {
        /* TODO: SMC to boot secure-AArch32 */
        UINT32 KernelStartAddr;

        AmbaMisra_TypeCast64(&KernelStartAddr, &FwStartEL1);
        BLD_PrintStr("# Enter RTOS\r\n#\r\n");

        {
            AmbaRTSL_NorSpiStop();

            (void)AmbaRTSL_GicIntDisable(AMBA_INT_SPI_ID105_SPI_NOR);
            AmbaRTSL_DmaChanRelease(AmbaRTSL_NorSpiCtrl.NorRxDMAChannel);
            AmbaRTSL_DmaChanRelease(AmbaRTSL_NorSpiCtrl.NorTxDMAChannel);
            (void)AmbaRTSL_GicIntDisable(AMBA_INT_SPI_ID115_DMA0);
            (void)AmbaRTSL_GicIntDisable(AMBA_INT_SPI_ID116_DMA1);
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
#endif
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

    (void)AmbaRTSL_UartInit();
    (void)AmbaRTSL_UartConfig(AMBA_UART_APB_CHANNEL0, 24000000U, 115200U, &AmbaUartConfig);

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

    BLD_SetOtpVppTiming();

#if defined(CONFIG_ATF_FIP_RTOS) && (CONFIG_BOOT_CORE_SRTOS > 0) && (CONFIG_BOOT_CORE_LINUX == 0)
    BLD_EnterKernel();
#else
#if !defined(CONFIG_BUILD_QNX_IPL)
    AmbaTime_A53Bld(A53_BLD_DEV_INIT_START);
#endif
    if (BLD_ERR_NONE == BLD_InitBootDevice()) {
        void AmbaRTSL_SpiNORDeviceReset(void);
#if !defined(CONFIG_BUILD_QNX_IPL)
        AmbaTime_A53Bld(A53_BLD_DEV_INIT_DONE);
#endif
#ifdef CONFIG_DRAM_TYPE_LPDDR4
        BLD_CheckTrainingResult();
#if !defined(CONFIG_BUILD_QNX_IPL)
        AmbaTime_A53Bld(A53_BLD_CHK_DRAM_TRAIN_DONE);
#endif
#endif
        BLD_LoadFirmware();
        AmbaRTSL_SpiNORDeviceReset();
        BLD_EnterKernel();
    }

    /* Force enter shell if load code failed */
    BLD_ShellStart(1U);
#endif
    return OK;
}

