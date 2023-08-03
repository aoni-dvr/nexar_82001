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
 *  @details Bootloader for eMMC Boot
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

#include "AmbaBLD_cfg.h"

extern const AMBA_PARTITION_CONFIG_s AmbaNAND_SysPartConfig[AMBA_NUM_SYS_PARTITION];
extern const AMBA_PARTITION_CONFIG_s AmbaNAND_UserPartConfig[AMBA_NUM_USER_PARTITION];
extern AMBA_GPIO_DEFAULT_s GpioPinGrpConfig;

#ifdef CONFIG_ENABLE_SECURITY
extern UINT32 AmbaUserRsaSigVerify(UINT8 *pData, UINT32 DataSize);
#endif

extern void BldStop(void);

static UINT8 *FwStartEL1 = NULL;
static UINT8 *FwStartEL2 = NULL;
static UINT8 *FwStartEL3 = NULL;

/* Check FW CRC32, 0 as disable */
#define AMBA_BLD_CHECK_FW_CRC32 (NO)

#define PRINT_BUF_SIZE  (1024U)
#define AMBA_BLD_UART_CH    AMBA_UART_APB_CHANNEL0

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
        if (AmbaRTSL_UartWrite(AMBA_BLD_UART_CH, UartTxSize, &BldPrintBuf[Index], &ActualTxSize) != UART_ERR_NONE) {
            break;
        } else {
            UartTxSize -= ActualTxSize;
            Index += ActualTxSize;
        }
    }
}

#if 0
/**
 *  @RoutineName:: BLD_LoadKernelPartial
 *
 *  @Description:: Load kernel partially.
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
 */
static UINT32 BLD_LoadKernelPartial(void)
{
    UINT32 PartID = AMBA_USER_PARTITION_SYS_SOFTWARE;
    UINT32 RetStatus, PartFlag = 1U;
    UINT8 *pDst = NULL;
    UINT32 Offset, Length;
    AMBA_PLOAD_PARTITION_s *pPloadInfo = &pAmbaNandTblUserPart->PloadInfo;

    /*
     * The following macros are linked-time symbols extracted from
     * the prkapp_{debug,release}.elf, and passed from build system
     * with -D compiler option
     *
     *   REGION0_RO_BASE, REGION0_RO_LENGTH
     *   REGION0_RW_BASE, REGION0_RW_LENGTH
     */
    AmbaMisra_TypeCast32(&pDst, &pPloadInfo->RegionRoStart[0]);
    Offset = 0;
    Length = pPloadInfo->RegionRoSize[0];
    RetStatus = AmbaRTSL_NandReadPartitionPart(PartFlag, PartID, Offset, Length, pDst);

    if (RetStatus == OK) {
        AmbaMisra_TypeCast32(&pDst, &pPloadInfo->RegionRwStart[0]);
        Offset = pPloadInfo->RegionRwStart[0] - pPloadInfo->RegionRoStart[0];
        Length = pPloadInfo->RegionRwSize[0];
        RetStatus = AmbaRTSL_NandReadPartitionPart(PartFlag, PartID, Offset, Length, pDst);
    }

    if (RetStatus == OK) {
        AmbaMisra_TypeCast32(&pDst, &pPloadInfo->LinkerStubStart);
        Offset = pPloadInfo->LinkerStubStart - pPloadInfo->RegionRoStart[0];
        Length = pPloadInfo->LinkerStubSize;
        RetStatus = AmbaRTSL_NandReadPartitionPart(PartFlag, PartID, Offset, Length, pDst);
    }

    if (RetStatus == OK) {
        AmbaMisra_TypeCast32(&pDst, &pPloadInfo->DspBufStart);
        Offset = pPloadInfo->DspBufStart - pPloadInfo->RegionRoStart[0];
        Length = pPloadInfo->DspBufSize;
        RetStatus = AmbaRTSL_NandReadPartitionPart(PartFlag, PartID, Offset, Length, pDst);
    }

    return RetStatus;
}
#endif

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
    pAmbaRTSL_EmmcUserPartTable ->Entry[PartID].Attribute &= ~AMBA_PARTITION_ATTR_ERASED;

    RetVal = AmbaRTSL_EmmcErasePartition(IsUserPart, PartID);

    return RetVal;
}

UINT32 BLD_InitBootDevice(void)
{
    AMBA_SD_CONFIG_s SdConfig;
    extern void AmbaUserSD_PowerCtrl(UINT32 SdChanNo, UINT32 VddPower);
    const AMBA_PARTITION_CONFIG_s * pPartConfigTmp;
    UINT32 RetVal;

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

    pPartConfigTmp = AmbaNAND_SysPartConfig;
    AmbaMisra_TypeCast64(&SdConfig.pSysPartConfig, &pPartConfigTmp);
    pPartConfigTmp = AmbaNAND_UserPartConfig;
    AmbaMisra_TypeCast64(&SdConfig.pUserPartConfig, &pPartConfigTmp);

    RetVal  = AmbaRTSL_SdConfig(AMBA_SD_CHANNEL0, &SdConfig);
    RetVal |= AmbaRTSL_SdCardInit(AMBA_SD_CHANNEL0);
    if (RetVal != OK) {
        BLD_PrintStr("\r\n# eMMC init fail#\r\n");
    } else {
        if (AmbaRTSL_EmmcInitPtbBbt() != OK) {
            BLD_PrintStr("\r\n# eMMC init PtbBbt fail#\r\n");
            while(1);
        }
    }

    return RetVal;
}

static void BLD_LoadKernel(const AMBA_PARTITION_ENTRY_s *pPartEntry, UINT32 PartID)
{
    UINT8   *pLoadAddr;
    UINT32  Crc32;
    ULONG   LoadAddr;

    LoadAddr = pPartEntry->RamLoadAddr;
    AmbaMisra_TypeCast64(&FwStartEL1, &LoadAddr);
#if defined(CONFIG_FASTBOOT_LZ4)
    LoadAddr = (ULONG)CONFIG_FB_LZ4_COMPBASE;
    AmbaMisra_TypeCast64(&pLoadAddr, &LoadAddr);
#else
    pLoadAddr = FwStartEL1;
#endif

#if !defined(CONFIG_BUILD_QNX_IPL)
    if (PartID == AMBA_USER_PARTITION_LINUX_KERNEL) {
        AmbaTime_A53Bld(A53_BLD_LOAD_LINUX_START);
    } else {
        AmbaTime_A53Bld(A53_BLD_LOAD_SYS_START);
    }
#endif

    if (AmbaRTSL_EmmcReadPartition(1, PartID, pLoadAddr) != BLD_ERR_NONE) {
        BLD_PrintStr("# Kernel image is not found\r\n#\r\n");
    } else {
        if (AMBA_BLD_CHECK_FW_CRC32 == YES) {
            BLD_PrintStr("# Check Kernel image Crc32 \r\n#\r\n");
            Crc32 = IO_UtilityCrc32(pLoadAddr, pPartEntry->ActualByteSize);
            if (Crc32 != pPartEntry->ImageCRC32) {
                BldStop();
            }
        }

    #ifdef CONFIG_ENABLE_SECURITY
        BLD_PrintStr("# Check Kernel image signature \r\n#\r\n");
        if (0u != AmbaUserRsaSigVerify(pLoadAddr, pPartEntry->ActualByteSize)) {
            BLD_PrintStr("# Verify failed \r\n#\r\n");
            BldStop();
        }
    #endif

    #if defined(CONFIG_FASTBOOT_LZ4)
        BLD_PrintStr("# LZ4 decompress ... \r\n");
        {
            extern UINT32 AmbaLZ4_DeCompressFile(UINT8 *pSrc, UINT8 *pDst, UINT32 SrcSize, UINT32 DstSize);

            UINT32  DstSize = (UINT32)CONFIG_FB_LZ4_COMPBASE - (UINT32)CONFIG_FWPROG_SYS_LOADADDR;

            AmbaTime_A53Bld(A53_BLD_DECOMPRESS_START);
            if (AmbaLZ4_DeCompressFile(pLoadAddr,
                                    FwStartEL1,
                                    pPartEntry->ActualByteSize,
                                    DstSize) == 0U) {
                BLD_PrintStr("# Fail to decompress Kernel image \r\n");
            }
            AmbaTime_A53Bld(A53_BLD_DECOMPRESS_DONE);
        }
        BLD_PrintStr("# LZ4 decompress done \r\n");
    #endif
    }

#if !defined(CONFIG_BUILD_QNX_IPL)
    if (PartID == AMBA_USER_PARTITION_LINUX_KERNEL) {
        AmbaTime_A53Bld(A53_BLD_LOAD_LINUX_DONE);
    } else {
        AmbaTime_A53Bld(A53_BLD_LOAD_SYS_DONE);
    }
#endif
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
    UINT32 Crc32;
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

    /* Load EL3 firmware image */
    if (BLDCFG_ATF_HAVE_BL2 == NO) {
        if (BLDCFG_BLD_SEQ_ATF == YES) {
            BLD_PrintStr("# Load ATF\r\n");
#if !defined(CONFIG_BUILD_QNX_IPL)
            AmbaTime_A53Bld(A53_BLD_LOAD_ATF_START);
#endif
            pPartEntry = AmbaRTSL_EmmcGetPartEntry(0, AMBA_SYS_PARTITION_ARM_TRUST_FW);
            if ((pPartEntry != NULL) && (pPartEntry->ActualByteSize != 0U)) {
                AmbaMisra_TypeCast32(&FwStartEL3, &pPartEntry->RamLoadAddr);
                if (AmbaRTSL_EmmcReadPartition(0, AMBA_SYS_PARTITION_ARM_TRUST_FW, FwStartEL3) != BLD_ERR_NONE) {
                    BLD_PrintStr("# ATF image is not found\r\n#\r\n");
                }
                if (AMBA_BLD_CHECK_FW_CRC32 == YES) {
                    BLD_PrintStr("# Check ATF image Crc32 \r\n#\r\n");
                    Crc32 = IO_UtilityCrc32(FwStartEL3, pPartEntry->ActualByteSize);
                    if (Crc32 != pPartEntry->ImageCRC32) {
                        BldStop();
                    }
                }
            }
#if !defined(CONFIG_BUILD_QNX_IPL)
            AmbaTime_A53Bld(A53_BLD_LOAD_ATF_DONE);
#endif
        }
    }

    /* Load alternate EL1 firmware image */
    if (AmbaRTSL_EmmcIsBldMagicCodeSet() != 0U) {
        BLD_PrintStr("# Load Firmware Updater\r\n");
#if !defined(CONFIG_BUILD_QNX_IPL)
        AmbaTime_A53Bld(A53_BLD_LOAD_FWUPDATER_START);
#endif
        pPartEntry = AmbaRTSL_EmmcGetPartEntry(0, AMBA_SYS_PARTITION_FW_UPDATER);
        if ((pPartEntry != NULL) && (pPartEntry->ActualByteSize != 0U)) {
            AmbaMisra_TypeCast32(&FwStartEL1, &pPartEntry->RamLoadAddr);
            if (AmbaRTSL_EmmcReadPartition(0, AMBA_SYS_PARTITION_FW_UPDATER, FwStartEL1) != (INT32)OK) {
                BLD_PrintStr("# Firmware Updater image is not found\r\n#\r\n");
            }
            if (AMBA_BLD_CHECK_FW_CRC32 == YES) {
                BLD_PrintStr("# Check Firmware Updater image Crc32 \r\n#\r\n");
                Crc32 = IO_UtilityCrc32(FwStartEL1, pPartEntry->ActualByteSize);
                if (Crc32 != pPartEntry->ImageCRC32) {
                    BldStop();
                }
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
            if (AMBA_BLD_CHECK_FW_CRC32 == YES) {
                BLD_PrintStr("# Check XTB image Crc32 \r\n#\r\n");
                Crc32 = IO_UtilityCrc32(XtbImage, pPartEntry->ActualByteSize);
                if (Crc32 != pPartEntry->ImageCRC32) {
                    BldStop();
                }
            }
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

        if (BLDCFG_ATF_FIP_RTOS == YES) {
            /* should never be here */
            __asm__ volatile("b .");
        } else if ((BLDCFG_BLD_SEQ_SRTOS == YES) || (BLDCFG_BLD_SEQ_ATF_SRTOS == YES)) {
            pPartEntry = AmbaRTSL_EmmcGetPartEntry(1U, AMBA_USER_PARTITION_SYS_SOFTWARE);
            if ((pPartEntry != NULL) && (pPartEntry->ActualByteSize != 0U)) {
                BLD_LoadKernel(pPartEntry, AMBA_USER_PARTITION_SYS_SOFTWARE);
            }
        } else if ((BLDCFG_BLD_SEQ_LINUX == YES) || (BLDCFG_BLD_SEQ_ATF_LINUX == YES)) {
            pPartEntry = AmbaRTSL_EmmcGetPartEntry(1, AMBA_USER_PARTITION_LINUX_KERNEL);
            if ((pPartEntry != NULL) && (pPartEntry->ActualByteSize != 0U)) {
                BLD_LoadKernel(pPartEntry, AMBA_USER_PARTITION_LINUX_KERNEL);

            #if !defined(CONFIG_DEVICE_TREE_SUPPORT)
                {
                    #define AMBARELLA_PARAMS_SIZE 0x10000

                    /* Prepare dtb */
                    UINT32 Rval;
                    UINT32 Block;
                    UINT8 *dtbAddr;
                    AMBA_NVM_SECTOR_CONFIG_s SecConfig;

                    Rval = pPartEntry->RamLoadAddr & ~0xFFFFF;
                    AmbaMisra_TypeCast32(&dtbAddr, &Rval);

                    /* DTB is programmed to the last good block of Linux partition. */
                    Block = pPartEntry->StartBlkAddr + pPartEntry->BlkCount - 1;

                    SecConfig.NumSector = AMBARELLA_PARAMS_SIZE / 512U;
                    SecConfig.StartSector = Block;
                    SecConfig.pDataBuf = dtbAddr;
                    Rval = AmbaRTSL_SdReadSector(AMBA_SD_CHANNEL0, &SecConfig);
                    if (Rval != 0u) {
                        BLD_PrintStr("# Load DTB failed\r\n#\r\n");
                    } else {
                        extern int fdt_update_memory(void *fdt);
                        extern int fdt_update_chosen(void *fdt);
                        extern int fdt_update_emmc(void *fdt);
                        fdt_update_emmc(dtbAddr);
                        fdt_update_chosen(dtbAddr);
                        fdt_update_memory(dtbAddr);
                    }
                }
            #endif
            }
        } else if (BLDCFG_BLD_SEQ_ATF_XEN == YES) {
            /* Load XEN image */
            BLD_PrintStr("# Load XEN\r\n");
            pPartEntry = AmbaRTSL_EmmcGetPartEntry(1, AMBA_USER_PARTITION_XEN);
            if (pPartEntry == NULL) {
                BLD_PrintStr("# XEN part is NULL!\r\n");
            } else if (pPartEntry->ActualByteSize == 0U) {
                BLD_PrintStr("# XEN part is 0!\r\n");
            } else {
                AmbaMisra_TypeCast32(&FwStartEL2, &pPartEntry->RamLoadAddr);
                if (AmbaRTSL_EmmcReadPartition(1, AMBA_USER_PARTITION_XEN, FwStartEL2) != BLD_ERR_NONE) {
                    BLD_PrintStr("# XEN image is not found!\r\n");
                }
                if (AMBA_BLD_CHECK_FW_CRC32 == YES) {
                    BLD_PrintStr("# Check XEN image Crc32 \r\n#\r\n");
                    Crc32 = IO_UtilityCrc32(FwStartEL2, pPartEntry->ActualByteSize);
                    if (Crc32 != pPartEntry->ImageCRC32) {
                        BldStop();
                    }
                }
            }
#if 0
            /* For Xen */
            UINT8 *XImage = NULL;
            /* Load Dom0/DomU kernel image */
            BLD_PrintStr("# Load dom kernel image\r\n");
            if ((BLDCFG_XEN_DOM0_LX == YES) ||
                (BLDCFG_XEN_DOMU_LX == YES)) {
                pPartEntry = AmbaRTSL_EmmcGetPartEntry(1, AMBA_USER_PARTITION_LINUX_KERNEL);
                if (pPartEntry == NULL) {
                    BLD_PrintStr("# Kernel part is NULL!\r\n");
                } else if (pPartEntry->ActualByteSize == 0U) {
                    BLD_PrintStr("# Kernel part is 0!\r\n");
                } else {
                    AmbaMisra_TypeCast32(&XImage, &pPartEntry->RamLoadAddr);
                    if (AmbaRTSL_EmmcReadPartition(1, AMBA_USER_PARTITION_LINUX_KERNEL, XImage) != BLD_ERR_NONE) {
                        BLD_PrintStr("# Kernel image is not found!\r\n");
                    }
                    if (AMBA_BLD_CHECK_FW_CRC32 == YES) {
                        BLD_PrintStr("# Check Kernel image Crc32 \r\n#\r\n");
                        Crc32 = IO_UtilityCrc32(XImage, pPartEntry->ActualByteSize);
                        if (Crc32 != pPartEntry->ImageCRC32) {
                            BldStop();
                        }
                    }
                }
            }

            if ((BLDCFG_XEN_DOM0_TX == YES) ||
                (BLDCFG_XEN_DOMU_TX == YES)) {
                pPartEntry = AmbaRTSL_EmmcGetPartEntry(1, AMBA_USER_PARTITION_SYS_SOFTWARE);
                if ((pPartEntry != NULL) && (pPartEntry->ActualByteSize != 0U)) {
                    AmbaMisra_TypeCast32(&XImage, &pPartEntry->RamLoadAddr);
                    if (AmbaRTSL_EmmcReadPartition(1, AMBA_USER_PARTITION_SYS_SOFTWARE, XImage) != BLD_ERR_NONE) {
                        BLD_PrintStr("# Kernel image is not found\r\n#\r\n");
                    }
                    if (AMBA_BLD_CHECK_FW_CRC32 == YES) {
                        BLD_PrintStr("# Check Kernel image Crc32 \r\n#\r\n");
                        Crc32 = IO_UtilityCrc32(XImage, pPartEntry->ActualByteSize);
                        if (Crc32 != pPartEntry->ImageCRC32) {
                            BldStop();
                        }
                    }
                }
            }
#endif
#if 0 // enable if have no r52
            AmbaBLD_BootCA53(1U, AMBA_RAM_CA53_ROM_CODE_ADDR);  // Park core 1
            AmbaBLD_BootCA53(2U, AMBA_RAM_CA53_ROM_CODE_ADDR);  // Park core 2
            AmbaBLD_BootCA53(3U, AMBA_RAM_CA53_ROM_CODE_ADDR);  // Park core 3
#endif
        } else if (BLDCFG_BLD_SEQ_SHELL == YES) {
            /* Do nothing */
        } else {
            BLD_PrintStr("# Incomplete branch!\r\n#\r\n");
        }
    }

#ifdef CONFIG_BUILD_QNX_IPL
    pPartEntry = AmbaRTSL_EmmcGetPartEntry(0, AMBA_SYS_PARTITION_QNX_IPL);

    if ((pPartEntry != NULL) && (pPartEntry->ActualByteSize != 0U)) {
        AmbaMisra_TypeCast32(&FwStartEL1, &pPartEntry->RamLoadAddr);
        if (AmbaRTSL_EmmcReadPartition(0, AMBA_SYS_PARTITION_QNX_IPL, FwStartEL1) != BLD_ERR_NONE) {
            BLD_PrintStr("# ipl image is not found\r\n#\r\n");
        } else {
            BLD_PrintStr("# IPL load\r\n#\r\n");
        }
#ifdef AMBA_BLD_CHECK_FW_CRC32
        BLD_PrintStr("# Check ipl image Crc32 \r\n#\r\n");
        Crc32 = IO_UtilityCrc32(FwStartEL1, pPartEntry->ActualByteSize);
        if (Crc32 != pPartEntry->ImageCRC32) {
            BldStop();
        }
#endif
    }
#endif
#if !defined(CONFIG_BUILD_QNX_IPL)
    if (AmbaBldUserCallbacks.UserCbAftLoadFw != NULL) {
        AmbaBldUserCallbacks.UserCbAftLoadFw();
    }
#endif
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

#pragma GCC push_options
#pragma GCC optimize ("O0")
//#if !defined(CONFIG_BLD_SEQ_ATF) && (defined(CONFIG_BOOT_CORE_LINUX) && (CONFIG_BOOT_CORE_LINUX == 0))
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
//#endif

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

//#if !defined(CONFIG_ATF_FIP_RTOS) && defined(CONFIG_BLD_SEQ_SRTOS) && (CONFIG_BOOT_CORE_SRTOS > 0)
static AmbaSMC_param64_t param_SRTOSBoot;
//#elif defined(CONFIG_BLD_SEQ_LINUX) && (CONFIG_BOOT_CORE_LINUX > 0)
static AmbaSMC_param64_t param_LinuxBoot;
//#endif

static inline void BLD_setVBAR(void)
{
    void AmbaBLD_handler(void);

    register UINT32 CurrentEL;
    UINT64 VBAR = (UINT64)AmbaBLD_handler;

    CurrentEL = AmbaASM_ReadCurrentEL();
    CurrentEL = (CurrentEL >> 2U) & 0x3U;
    AmbaASM_WriteVectorBaseAddr(CurrentEL, VBAR);
}

static inline void BLD_ClrDisIRQ(void)
{
    UINT32 SysCtrlReg;

#if !defined(CONFIG_BUILD_QNX_IPL)
    AmbaTime_A53Bld(A53_BLD_ENTER_ATF);
    if (AmbaBldUserCallbacks.UserCbBefEnterATF != NULL) {
        AmbaBldUserCallbacks.UserCbBefEnterATF();
    }
#endif
    // Enable IRQ to let handler clear it.
    __asm__ __volatile__("msr daifclr, #0xF":::"memory");
    SysCtrlReg = AmbaRTSL_CpuReadSysCtrl();
    if (0x0U != (SysCtrlReg & 0x4U)) {          /* If data caching is enabled */
        AmbaRTSL_CacheCleanDataAll();
    }
    AmbaMMU_Disable();
    // Disbale IRQ
    __asm__ __volatile__("msr daifset, #0xF":::"memory");
}

/* cf. Documentation/arm64/booting.rst of Linux */
void BLD_prep_linux(void)
{
    void _start_el1(void);
    UINT32 i;
    UINT64 v;
    UINT32 *pv;

    /* Non-Secure AHB */
    *((UINT32 *)0x20F1000008) = 1;
    /* Secure AHB */
    *((UINT32 *)0x20F100000C) = 1;
    /* AXI Config */
    *((UINT32 *)0x20F1000010) = 1;
    /* GIC */
    *((UINT32 *)0x20F1000014) = 1;

    /* For CV5 */
    *((UINT32 *)0x20F1000018) = 1;
    *((UINT32 *)0x20F100001C) = 1;
    *((UINT32 *)0x20F1000020) = 1;
    *((UINT32 *)0x20F1000024) = 1;
    *((UINT32 *)0x20F1000028) = 1;

    *((UINT32 *)0x20F2000090) = 0;
    *((UINT32 *)0x20F2000094) = 0;
    *((UINT32 *)0x20F2000098) = 0;

    /* Timer */
    __asm__ volatile("LDR X0, =0x02FAF080;" \
                     "MSR CNTFRQ_EL0, X0;");

    /* GICD_CTR */
    *((UINT32 *)0x20F3001000) = 0x00000003;
    /* Config SPIs as Grp1 */
    for (i = 0; i < 0x20; i += 4) {
        v = 0x20F3001080 + i;
        AmbaMisra_TypeCast64(&pv, &v);
        *pv = (UINT32)0xFFFFFFFF;
    }
    /* Enable SGI 0 */
    *((UINT32 *)0x20F3001100) = 0x01;

    /* GICC_CTLR, Disable IRQ/FIQ bypass, Enable Ack Gro1, Enable Grp0, Grp1 */
    *((UINT32 *)0x20F3002000) = 0x01E7;
    /* Allow NS access to GICC_PMR */
    *((UINT32 *)0x20F3002004) = 0x80;

    /* GICD_SGIR */
    *((UINT32 *)0x20F3001F00) = 0x01008000;

    AMBA_DSB();

    /* Boot up other core */

}

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
    if ((BLDCFG_ATF_FIP_RTOS == YES) &&
        ((isValid_BLDCFG_BOOT_CORE_SRTOS() == YES) && (BLDCFG_BOOT_CORE_SRTOS > 0))  &&
        ((isValid_BLDCFG_BOOT_CORE_LINUX() == YES) && (BLDCFG_BOOT_CORE_LINUX > 0))) {
        /* core1 RTOS will boot core3 linux, suspend core0 BLD here */
        __asm__ volatile("b .");
    } else if ((BLDCFG_ATF_FIP_RTOS == YES) || (BLDCFG_BLD_SEQ_LINUX == YES)) {
        void (*FwEntry)(void) = NULL;
        /* Linux Kernel Image must be 2MB aligned */
#ifdef CONFIG_FWPROG_LNX_LOADADDR
        UINT64 dtbAddr = CONFIG_FWPROG_LNX_LOADADDR & ~0xFFFFF;

        FwStartEL1 = (UINT8 *) CONFIG_FWPROG_LNX_LOADADDR;
#else
        UINT64 dtbAddr = 0;
#endif // CONFIG_FWPROG_LNX_LOADADDR
        AmbaMisra_TypeCast64(&FwEntry, &FwStartEL1);

        // TODO: Fix unknown exception
        if (1) {
            BLD_setVBAR();
        }

//        BLD_ClrDisIRQ();

        /* TODO: Ensure to run under EL1H or AArch32 */
        if (((isValid_BLDCFG_BOOT_CORE_LINUX() == YES) && BLDCFG_BOOT_CORE_LINUX > 0)) {
            BLD_PrintStr("# Enter Linux\r\n#\r\n");
            {
                void AmbaSmcCall64(UINT64 p0, UINT64 p1, UINT64 p2, UINT64 p3, UINT64 p4);
                void AmbaBLD_PsciOnEntry_Linux(void);

                param_LinuxBoot.x0 = 0;

#if defined(CONFIG_FWPROG_LNX_LOADADDR) && defined(CONFIG_BOOT_CORE_LINUX)
                AmbaSmcCall64(PSCI_CPU_ON_AARCH64, CONFIG_BOOT_CORE_LINUX, (UINT64)AmbaBLD_PsciOnEntry_Linux, (UINT64)&param_LinuxBoot, 0);
#endif // CONFIG_FWPROG_LNX_LOADADDR
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
        } else {
            if (BLDCFG_ATF_FIP_RTOS == YES) {
                /* RTOS core1 already booted by ATF, core0 jump to linux */
                do {
                    AMBA_SEVL();
                    AMBA_WFE();
                    AMBA_WFE();
                } while (pAmbaScratchpadNS_Reg->AhbScratchpad[3] != 0xdeadbeef);
                pAmbaScratchpadNS_Reg->AhbScratchpad[3] = 0x0U;
            }


            if (1) {
                BLD_prep_linux();
            }

            // SH
            if (0) {
                void jump_to_kernel(UINT64 *kernel, UINT64 *dtb);

                jump_to_kernel((UINT64 *)FwEntry, (UINT64 *)dtbAddr);
            } else {
                __asm__ volatile("bl    _clean_flush_all_cache;" \
                                 "bl    _disable_icache;" \
                                 "bl    _disable_dcache;" \
                                 "bl    __disable_mmu;");
            }
            bootLinux(dtbAddr, 0u, 0u, 0u, FwEntry);
        } /* defined(BLDCFG_BOOT_CORE_LINUX) && (BLDCFG_BOOT_CORE_LINUX > 0) */
        /* TODO: shell of bootloader will use NAND driver and trigger ISR. */
        __asm__ volatile("b .");
    } else if (BLDCFG_ATF_HAVE_BL2 == NO) {
        /* bst -> bld */
        if (FwStartEL3 == NULL) {
            if (BLDCFG_THREADX64 == YES) {
                /* bst -> bld -> srtos (64b) */
                void (*FwEntry)(void) = NULL;

                AmbaMisra_TypeCast64(&FwEntry, &FwStartEL1);
                BLD_ClrDisIRQ();
                FwEntry();
            } else {
                BLD_PrintStr("# Unable to enter kernel\r\n#\r\n");
            }
        } else {
            /* bst -> bld -> ATF */
            UINT32 KernelStartAddr;
            void (*FwEntry)(void) = NULL;

            /* Write RTOS start address and machine type flag to AHB Scratchpad Data1. */
            /* RTOS machine type (AArch32 or AArch64) should be programmed to partition too.) */
            AmbaMisra_TypeCast64(&KernelStartAddr, &FwStartEL1);
            pAmbaScratchpadNS_Reg->AhbScratchpad[0] = 0x0U;
            pAmbaScratchpadNS_Reg->AhbScratchpad[1] = (UINT32)(KernelStartAddr + 0x1U);
            pAmbaScratchpadNS_Reg->AhbScratchpad[2] = 0x0U;     /* Kernel parameter */
            pAmbaScratchpadNS_Reg->AhbScratchpad[3] = (UINT32)(CONFIG_SECURE_MEMORY_SIZE);

            AmbaMisra_TypeCast64(&FwEntry, &FwStartEL3);
            BLD_PrintStr("# Enter ATF\r\n#\r\n");

            BLD_ClrDisIRQ();
            FwEntry();
        }
    } else if (BLDCFG_BLD_SEQ_SRTOS == YES) {
        /* bst -> bl2 -> bl31 -> bl33 (bld) -> srtos (32b) */
        if (FwStartEL1 == NULL) {
            BLD_PrintStr("# Unable to enter kernel\r\n#\r\n");
        } else {
            /* TODO: SMC to boot secure-AArch32 */
            UINT32 KernelStartAddr;

            AmbaMisra_TypeCast64(&KernelStartAddr, &FwStartEL1);
            BLD_PrintStr("# Enter RTOS\r\n#\r\n");

            {
                AmbaRTSL_SdResetAll(AMBA_SD_CHANNEL0);

                (void)AmbaRTSL_GicIntDisable(AMBA_INT_SPI_ID104_SD);
            }

            // TODO: Fix unknown exception
            if (1) {
                BLD_setVBAR();
            }

            BLD_ClrDisIRQ();

            if ((isValid_BLDCFG_BOOT_CORE_SRTOS() == YES) && (BLDCFG_BOOT_CORE_SRTOS > 0) ) {
                {
                    void AmbaSmcCall64(UINT64 p0, UINT64 p1, UINT64 p2, UINT64 p3, UINT64 p4);
                    void AmbaBLD_PsciOnEntry_SRTOS(void);

                    param_SRTOSBoot.x0 = 0;

                    AmbaSmcCall64(PSCI_CPU_ON_AARCH64, BLDCFG_BOOT_CORE_SRTOS, (UINT64)AmbaBLD_PsciOnEntry_SRTOS, (UINT64)&param_SRTOSBoot, 0);
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
                if ((isValid_BLDCFG_BOOT_CORE_LINUX() == YES) && (BLDCFG_BOOT_CORE_LINUX == 0)) {
                    void (*FwEntry)(void) = NULL;
#ifdef CONFIG_FWPROG_LNX_LOADADDR
                    UINT64 dtbAddr = CONFIG_FWPROG_LNX_LOADADDR & ~0xFFFFF;
#else
                    UINT64 dtbAddr = 0;
#endif
                    /* Waiting trigger from AmbaLink to boot Linux */
                    do {
                        AMBA_SEVL();
                        AMBA_WFE();
                        AMBA_WFE();
                    } while (pAmbaScratchpadNS_Reg->AhbScratchpad[3] != 0xdeadbeef);
                    pAmbaScratchpadNS_Reg->AhbScratchpad[3] = 0x0U;

#ifdef CONFIG_FWPROG_LNX_LOADADDR
                    FwStartEL1 = (UINT8 *) CONFIG_FWPROG_LNX_LOADADDR;
#endif
                    AmbaMisra_TypeCast64(&FwEntry, &FwStartEL1);
                    bootLinux(dtbAddr, 0u, 0u, 0u, FwEntry);
                } else {
                    __asm__ volatile("b .");
                }
            } else {
                {
                    void AmbaSmcCall64(UINT64 p0, UINT64 p1, UINT64 p2, UINT64 p3, UINT64 p4);

                    AmbaSmcCall64(AMBA_SIP_BOOT_RTOS, KernelStartAddr, 0, S_SWITCH_AARCH32, 0);
                }
            } /* defined(BLDCFG_BOOT_CORE_SRTOS) && (BLDCFG_BOOT_CORE_SRTOS > 0) */
        }
    } else if (BLDCFG_BLD_SEQ_SHELL == YES) {
        /* Do nothing */
    } else {/* BLDCFG_BLD_SEQ_xxx */
        BLD_PrintStr("# Incomplete branch!\r\n#\r\n");
    } /* BLDCFG_BLD_SEQ_xxx */

    /* Should not be called */
    BLD_PrintStr("# Boot failed!\r\n#\r\n");
}
#pragma GCC pop_options

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
    Rval |= AmbaRTSL_UartConfig(AMBA_UART_APB_CHANNEL0, UartClk, 115200U, &AmbaUartConfig);
    if (0U != Rval) {
        BLD_PrintStr("\r\n# UART and PLL config failed !\r\n#\r\n");
    }

    BLD_PrintStr("\r\n# 64-bit Boot-Up Success\r\n#\r\n");

#if !defined(CONFIG_BUILD_QNX_IPL)
    AmbaTime_A53Bld(A53_BLD_SHELL_START);
#endif

    // check & save chip version
    BLD_ChipVersionCheck();

    if (BLDCFG_BLD_SEQ_SHELL == YES) {
        BLD_ShellStart(1U);
    } else {
        BLD_ShellStart(0U);
    }

#if !defined(CONFIG_BUILD_QNX_IPL)
    AmbaTime_A53Bld(A53_BLD_POLL_USER_STOP);
    AmbaTime_BldGetBssInfo();
#endif

    if ((BLDCFG_ATF_FIP_RTOS == YES) &&
        ((isValid_BLDCFG_BOOT_CORE_SRTOS() == YES) && (BLDCFG_BOOT_CORE_SRTOS > 0))  &&
        ((isValid_BLDCFG_BOOT_CORE_LINUX() == YES) && (BLDCFG_BOOT_CORE_LINUX == 0))) {
        BLD_EnterKernel();
    } else {
#if !defined(CONFIG_BUILD_QNX_IPL)
        AmbaTime_A53Bld(A53_BLD_DEV_INIT_START);
#endif

        if (BLD_ERR_NONE == BLD_InitBootDevice()) {
#if !defined(CONFIG_BUILD_QNX_IPL)
            AmbaTime_A53Bld(A53_BLD_DEV_INIT_DONE);
#endif
#ifdef CONFIG_AUTO_DRAM_TRAINING
            BLD_CheckTrainingResult();
#if !defined(CONFIG_BUILD_QNX_IPL)
            AmbaTime_A53Bld(A53_BLD_CHK_DRAM_TRAIN_DONE);
#endif
#endif
            BLD_LoadFirmware();
            BLD_EnterKernel();
        } else {
            BLD_PrintStr("\r\n# BLD_InitBootDevice failed !\r\n#\r\n");
        }

        /* Force enter shell if load code failed */
        BLD_ShellStart(1U);
    }

    BLD_PrintStr("# Trapped here..\r\n");
    return 0;
}

