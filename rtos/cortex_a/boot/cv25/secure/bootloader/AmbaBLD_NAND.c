/**
 *  @file AmbaBLD_NAND.c
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

#include "AmbaBLD.h"
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

#include "AmbaRTSL_SPINAND.h"

#include "AmbaRTSL_NAND.h"
#include "AmbaRTSL_NAND_Ctrl.h"
#include "AmbaRTSL_NAND_BBM.h"
#include "AmbaRTSL_NAND_OP.h"

#include "AmbaCSL_Scratchpad.h"

#include <AmbaCSL_FIO.h>
#include <AmbaINT_Def.h>
#include <AmbaRTSL_GIC.h>

#include <AmbaATF.h>
#include "bsp.h"
#include "AmbaBLD_I2C.c"

extern const AMBA_NAND_DEV_INFO_s AmbaNAND_DevInfo;
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

static void BLD_PrintUInt32(const char *pFmt, UINT32 arg1, UINT32 arg2)
{
    static UINT8 BldPrintBuf[PRINT_BUF_SIZE] GNU_SECTION_NOZEROINIT;
    UINT32 ActualTxSize, UartTxSize, Index = 0U;
    char *pString;
    const UINT8 *pBuf = BldPrintBuf;
    UINT32 arg[2] = {arg1,  arg2};

    AmbaMisra_TypeCast64(&pString, &pBuf);
    UartTxSize = IO_UtilityStringPrintUInt32(pString, PRINT_BUF_SIZE, pFmt, (UINT32)2U, arg);
    while (UartTxSize > 0U) {
        if (AmbaRTSL_UartWrite(AMBA_UART_APB_CHANNEL0, UartTxSize, &BldPrintBuf[Index], &ActualTxSize) != UART_ERR_NONE) {
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
    return pAmbaNandTblSysPart;
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
    return pAmbaNandTblUserPart;
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

    pPartEntry = AmbaRTSL_NandGetPartEntry(PartFlag, PartID);
    if ((pPartEntry != NULL) && (pPartEntry->ActualByteSize != 0U)) {
        if (AmbaRTSL_NandReadPartition(PartFlag, PartID, pDataBuf) != BLD_ERR_NONE) {
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

    pPartEntry = AmbaRTSL_NandGetPartEntry(PartFlag, PartID);
    if ((pPartEntry != NULL) && (pPartEntry->ActualByteSize != 0U)) {
        if (AmbaRTSL_NandWritePartition(PartFlag, PartID, DataSize, pDataBuf) != BLD_ERR_NONE) {
            BLD_PrintStr("Write partition failed!\r\n#\r\n");
        }
    }
}

static UINT8 BstBuffer[24 * 2 * 1024] GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE;

UINT32 BLD_WriteDdrcData(UINT8 *pDataBuf)
{
    UINT32 BstPageCount     = AmbaRTSL_NandCtrl.BstPageCount;      /* Number of Pages for BST */
    UINT32 RetVal = BLD_ERR_NONE;

    /* 1. Read BST data (block 0, page 0) */
    if (OK != AmbaRTSL_NandOpRead(0, BstPageCount, BstBuffer, NULL)) {
        RetVal = BLD_ERR_ARG;
        BLD_PrintStr("Nand Read BST Page0 fail\r\n");
    } else {
        /* 2. Erase the block 0 */
        if (AmbaRTSL_NandOpEraseBlock(0) != OK) {
            RetVal = BLD_ERR_ARG;
            BLD_PrintStr("Nand Erase Block0 fail\r\n");
        } else {
            /* 3. Write back BST data (block 0, page 0) */
            if (AmbaRTSL_NandOpProgram(0, BstPageCount, BstBuffer, NULL) != OK) {
                RetVal = BLD_ERR_ARG;
                BLD_PrintStr("Nand Write back BST data fail\r\n");
            } else {
                /* 4. Write back sys and user partition */
                RetVal = AmbaRTSL_NandWriteSysPTB(NULL);
                AmbaRTSL_NandWriteUserPTB(NULL, pAmbaNandTblUserPart->PTBNumber);

                /* 5. Write training resulet to page63 (block 0, page 63) */
                if (OK != AmbaRTSL_NandOpProgram(63, 1, (UINT8 *) pDataBuf, NULL)) {
                    RetVal = BLD_ERR_ARG;
                    BLD_PrintStr("Nand Write Page63 fail\r\n");
                }
            }
        }
    }

    return RetVal;
}

UINT32 BLD_LoadDdrcData(UINT8 *pDataBuf)
{

    UINT32 RetVal = BLD_ERR_NONE;
    RetVal = OK;
    if (OK != AmbaRTSL_NandOpRead(63, 1, pDataBuf, NULL)) {
        RetVal = BLD_ERR_ARG;
        BLD_PrintStr("Nand Read Page63 fail\r\n");
    }
    return RetVal;
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
    UINT32 i, BlkCount = AmbaRTSL_NandCtrl.TotalNumBlk;
    UINT32 TableSize;
    UINT8 *pTmp = NULL;
    const AMBA_PARTITION_ENTRY_s * pEntry = AmbaRTSL_NandGetPartEntry(1, AMBA_USER_PARTITION_CALIBRATION_DATA);

    for (i = 0U; i < BlkCount; i++) {
        if ((i >= pEntry->StartBlkAddr) &&
            (i < (pEntry->StartBlkAddr + pEntry->BlkCount))) {
            BLD_PrintStr("\r\nSkip calibration partition.\r\n");
            i = pEntry->StartBlkAddr + pEntry->BlkCount;
        }

        if (AmbaRTSL_NandGetBlkMark(i) == AMBA_NAND_BLK_FACTORY_BAD) {
            continue;
        }

        (void)AmbaRTSL_NandOpEraseBlock(i);
    }

    TableSize = sizeof(AMBA_SYS_PARTITION_TABLE_s);
    AmbaMisra_TypeCast64(&pTmp, &pAmbaNandTblSysPart);
    for (i = 0U; i < TableSize; i++) {
        pTmp[i] = 0xffU;
    }

    TableSize = sizeof(AMBA_USER_PARTITION_TABLE_s);
    AmbaMisra_TypeCast64(&pTmp, &pAmbaNandTblUserPart);
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
    UINT32 RetVal;
    /* Force to erase */
    pAmbaNandTblUserPart->Entry[PartID].Attribute |= AMBA_PARTITION_ATTR_ERASED;

    RetVal = AmbaRTSL_NandErasePartition(IsUserPart, PartID);

    if ((IsUserPart == 1U) && (PartID == 0U)) {
        (void)AmbaRTSL_NandOpEraseBlock(pAmbaNandTblUserPart->NAND_BlkAddrPrimaryBBT);
        (void)AmbaRTSL_NandOpEraseBlock(pAmbaNandTblUserPart->NAND_BlkAddrMirrorBBT);
    }

    return RetVal;
}

UINT32 BLD_InitBootDevice(void)
{
    const AMBA_PARTITION_CONFIG_s *pPartConfig;
    UINT32 RetVal = BLD_ERR_NONE;

#if defined(CONFIG_ENABLE_SPINAND_BOOT)
    extern const AMBA_SPINAND_DEV_INFO_s AmbaSpiNAND_DevInfo;
    AMBA_SPINAND_CONFIG_s SpiNandConfig;

    const AMBA_SPINAND_DEV_INFO_s *pSpiNandDev;

    pSpiNandDev = &AmbaSpiNAND_DevInfo;
    AmbaMisra_TypeCast64(&SpiNandConfig.pNandDevInfo, &pSpiNandDev);

    pPartConfig = AmbaNAND_SysPartConfig;
    AmbaMisra_TypeCast64(&SpiNandConfig.pSysPartConfig, &pPartConfig);

    pPartConfig = AmbaNAND_UserPartConfig;
    AmbaMisra_TypeCast64(&SpiNandConfig.pUserPartConfig, &pPartConfig);

    AmbaRTSL_GpioSetFuncAlt(GPIO_PIN_45_NAND_SPI_CLK);
    AmbaRTSL_GpioSetFuncAlt(GPIO_PIN_59_NAND_SPI_DATA0);
    AmbaRTSL_GpioSetFuncAlt(GPIO_PIN_60_NAND_SPI_DATA1);
    AmbaRTSL_GpioSetFuncAlt(GPIO_PIN_61_NAND_SPI_DATA2);
    AmbaRTSL_GpioSetFuncAlt(GPIO_PIN_62_NAND_SPI_DATA3);
    AmbaRTSL_GpioSetFuncAlt(GPIO_PIN_55_NAND_SPI_EN0);

    (void)AmbaRTSL_SpiNandInit();
    (void)AmbaRTSL_SpiNandConfig(&SpiNandConfig);
    (void)AmbaRTSL_SpiNand_ConfigOnDevECC();
#else
    const AMBA_NAND_DEV_INFO_s *pNandDev;
    AMBA_NAND_CONFIG_s NandConfig;

    pNandDev = &AmbaNAND_DevInfo;
    AmbaMisra_TypeCast64(&NandConfig.pNandDevInfo, &pNandDev);

    pPartConfig = AmbaNAND_SysPartConfig;
    AmbaMisra_TypeCast64(&NandConfig.pSysPartConfig, &pPartConfig);

    pPartConfig = AmbaNAND_UserPartConfig;
    AmbaMisra_TypeCast64(&NandConfig.pUserPartConfig, &pPartConfig);

    (void)AmbaRTSL_NandInit();
    (void)AmbaRTSL_NandConfig(&NandConfig);
#endif

    if (AmbaRTSL_NandInitPtbBbt() != BLD_ERR_NONE) {
        RetVal = BLD_ERR_INFO;
    }

    return RetVal;
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
#if defined(CONFIG_APP_FLOW_CARDV_AONI)
    AMBA_PARTITION_ENTRY_s *pPartEntry = NULL;
#else
    const AMBA_PARTITION_ENTRY_s *pPartEntry = NULL;
#endif
    UINT32 ActivePTB;
#if defined(CONFIG_APP_FLOW_CARDV_AONI)
    int BootFailTimes = 0;
    UINT8 Value1 = 0, Value2 = 0;
    int BootReason = 0;
#endif

    if (AmbaRTSL_NandGetActivePtbNo(&ActivePTB) != 0U) {
        BLD_PrintStr("Get Active User PTB Fail\r\n");
    } else {
        if (0U == ActivePTB) {
            BLD_PrintStr("# Active User PTB 0\r\n");
        } else {
            BLD_PrintStr("# Active User PTB 1\r\n");
        }
    }

    if(AmbaRTSL_NandReadUserPTB(NULL, ActivePTB) != 0U) {
        BLD_PrintStr("Read User PTB Fail\r\n");
    }
#if !defined(CONFIG_BUILD_QNX_IPL)
    if (AmbaBldUserCallbacks.UserCbBefLoadFw != NULL) {
        AmbaBldUserCallbacks.UserCbBefLoadFw();
    }
    AmbaTime_A53Bld(A53_BLD_LOAD_START);
#endif

#if defined(CONFIG_APP_FLOW_CARDV_AONI)
#if !defined(CONFIG_BSP_H32_NEXAR_D081)
    bld_i2c_write(LED_I2C_SLAVE_ADDR, 0x00, 0x55);//reset led    
    bld_i2c_write(LED_I2C_SLAVE_ADDR, 0x03, 0x15);//disable charge led
#endif
    bld_i2c_read(MAIN_MCU_I2C_SLAVE_ADDR, 0x08, &Value1);
    BootFailTimes = (Value1 >> 2) & 0x03;
    BLD_PrintUInt32("boot fail times: %d\r\n", BootFailTimes, 0);
    if (BootFailTimes >= 3) {
        BootFailTimes = 0;
        pPartEntry = AmbaRTSL_NandGetPartEntry(1, AMBA_USER_PARTITION_SYS_SOFTWARE);
        if ((pPartEntry != NULL) && (pPartEntry->ActualByteSize != 0U) && (pPartEntry->Attribute & FW_PARTITION_ACTIVE_FLAG)) {
            pPartEntry = AmbaRTSL_NandGetPartEntry(1, AMBA_USER_PARTITION_SYS_SOFTWARE_B);
            if ((pPartEntry != NULL) && (pPartEntry->ActualByteSize != 0U)) {
                pPartEntry = AmbaRTSL_NandGetPartEntry(1, AMBA_USER_PARTITION_SYS_SOFTWARE);
                pPartEntry->Attribute &= ~FW_PARTITION_ACTIVE_FLAG;
                pPartEntry = AmbaRTSL_NandGetPartEntry(1, AMBA_USER_PARTITION_SYS_SOFTWARE_B);
                pPartEntry->Attribute |= FW_PARTITION_ACTIVE_FLAG;
                BLD_PrintStr("# Part A reached max boot fail times. switch to B\r\n");
            } else {
                BLD_PrintStr("# Part A reached max boot fail times. but no Part B found, still boot from A\r\n");
            }
        } else {
            pPartEntry = AmbaRTSL_NandGetPartEntry(1, AMBA_USER_PARTITION_SYS_SOFTWARE);
            if ((pPartEntry != NULL) && (pPartEntry->ActualByteSize != 0U)) {
                pPartEntry = AmbaRTSL_NandGetPartEntry(1, AMBA_USER_PARTITION_SYS_SOFTWARE);
                pPartEntry->Attribute |= FW_PARTITION_ACTIVE_FLAG;
                pPartEntry = AmbaRTSL_NandGetPartEntry(1, AMBA_USER_PARTITION_SYS_SOFTWARE_B);
                pPartEntry->Attribute &= ~FW_PARTITION_ACTIVE_FLAG;
                BLD_PrintStr("# Part B reached max boot fail times. switch to A\r\n");
            } else {
                BLD_PrintStr("# Part B reached max boot fail times. but no Part A found, still boot from B\r\n");
            }
        }
        AmbaRTSL_NandWriteUserPTB(NULL, ActivePTB);
    }
    Value1 &= ~0x0C;
    Value1 |= ((BootFailTimes + 1) << 2);
    bld_i2c_write(MAIN_MCU_I2C_SLAVE_ADDR, 0x09, Value1);
#endif
    /* Load EL3 firmware image */
#ifndef CONFIG_ATF_HAVE_BL2
    BLD_PrintStr("# Load ATF\r\n");
#if !defined(CONFIG_BUILD_QNX_IPL)
    AmbaTime_A53Bld(A53_BLD_LOAD_ATF_START);
#endif
    pPartEntry = AmbaRTSL_NandGetPartEntry(0, AMBA_SYS_PARTITION_ARM_TRUST_FW);
    if ((pPartEntry != NULL) && (pPartEntry->ActualByteSize != 0U)) {
        AmbaMisra_TypeCast32(&FwStartEL3, &pPartEntry->RamLoadAddr);
        if (AmbaRTSL_NandReadPartition(0, AMBA_SYS_PARTITION_ARM_TRUST_FW, FwStartEL3) != BLD_ERR_NONE) {
            BLD_PrintStr("# ATF image is not found\r\n#\r\n");
        }
    }
#if !defined(CONFIG_BUILD_QNX_IPL)
    AmbaTime_A53Bld(A53_BLD_LOAD_ATF_DONE);
#endif
#endif /* ! CONFIG_ATF_HAVE_BL2 */
#if defined(CONFIG_APP_FLOW_CARDV_AONI) && defined(CONFIG_PMIC_CHIPON)
    bld_i2c_read(MAIN_MCU_I2C_SLAVE_ADDR, 0x05, &Value1);
    bld_i2c_read(MAIN_MCU_I2C_SLAVE_ADDR, 0x06, &Value2);
    BLD_PrintUInt32("boot reason: 0x05=%d, 0x06=%d\r\n", Value1, Value2);
    if (Value1 != 0xff && Value2 != 0xff) {
        BootReason = Value1 * 256 + Value2;
    }
    //set watch dog to 20s
    bld_i2c_write(MAIN_MCU_I2C_SLAVE_ADDR, 0x07, 20);

    bld_i2c_read(MAIN_MCU_I2C_SLAVE_ADDR, 0x08, &Value1);
    BLD_PrintUInt32("sram: 0x08=%d\r\n", Value1, 0U);
#endif

    if (FwStartEL1 == NULL) {
#if defined(CONFIG_DEVICE_TREE_SUPPORT)
        UINT8 *XtbImage = NULL;
        /* Load Xen/RTOS DTB */
        BLD_PrintStr("# Load XTB \r\n");
        pPartEntry = AmbaRTSL_NandGetPartEntry(1, AMBA_USER_PARTITION_XTB);
        if (pPartEntry == NULL) {
            BLD_PrintStr("# XTB part is NULL!\r\n");
        } else if (pPartEntry->ActualByteSize == 0U) {
            BLD_PrintStr("# XTB part is 0!\r\n");
        } else {
            AmbaMisra_TypeCast32(&XtbImage, &pPartEntry->RamLoadAddr);
            if (AmbaRTSL_NandReadPartition(1, AMBA_USER_PARTITION_XTB, XtbImage) != BLD_ERR_NONE) {
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
                extern int fdt_update_nand(void *fdt);

                AmbaMisra_TypeCast32(&xtb_ptr, &pPartEntry->RamLoadAddr);
                fdt_update_nand(xtb_ptr);
                fdt_update_chosen(xtb_ptr);
            }
#endif
        }
#endif
        /* Load alternate EL1 firmware image */
        //if (AmbaRTSL_NandIsBldMagicCodeSet() != 0U
        if ((Value1 & 0x20) == 0x20
#if defined(CONFIG_APP_FLOW_CARDV_AONI)
            || (BootReason & 0x208)
#endif
            ) {
            BLD_PrintStr("# Load Firmware Updater\r\n");
#if defined(CONFIG_APP_FLOW_CARDV_AONI)
            if ((Value1 & 0x20) == 0x20) {
                Value1 &= ~0x20;
                bld_i2c_write(MAIN_MCU_I2C_SLAVE_ADDR, 0x09, Value1);
            }
            //AmbaRTSL_NandEraseBldMagicCode();
#endif
#if !defined(CONFIG_BUILD_QNX_IPL)
            AmbaTime_A53Bld(A53_BLD_LOAD_FWUPDATER_START);
#endif
            pPartEntry = AmbaRTSL_NandGetPartEntry(0, AMBA_SYS_PARTITION_FW_UPDATER);
            if ((pPartEntry != NULL) && (pPartEntry->ActualByteSize != 0U)) {
                AmbaMisra_TypeCast32(&FwStartEL1, &pPartEntry->RamLoadAddr);
                if (AmbaRTSL_NandReadPartition(0, AMBA_SYS_PARTITION_FW_UPDATER, FwStartEL1) != BLD_ERR_NONE) {
                    BLD_PrintStr("# Firmware Updater image is not found\r\n#\r\n");
                } else {
                    BLD_PrintStr("# Firmware Updater image load success\r\n#\r\n");
                }
            }
#if !defined(CONFIG_BUILD_QNX_IPL)
            AmbaTime_A53Bld(A53_BLD_LOAD_FWUPDATER_DONE);
#endif
        } else {
            /* Load EL1 firmware image */
            BLD_PrintStr("# Load Kernel\r\n");
#if !defined(CONFIG_BUILD_QNX_IPL)
            AmbaTime_A53Bld(A53_BLD_LOAD_SYS_START);
#endif
#if defined(CONFIG_ATF_FIP_RTOS)
            /* should never be here */
            __asm__ volatile("b .");
#elif defined(CONFIG_BLD_SEQ_SRTOS) || defined(CONFIG_BLD_SEQ_ATF_SRTOS)
            pPartEntry = AmbaRTSL_NandGetPartEntry(1, AMBA_USER_PARTITION_SYS_SOFTWARE);
            if ((pPartEntry != NULL) && (pPartEntry->ActualByteSize != 0U) && (pPartEntry->Attribute & FW_PARTITION_ACTIVE_FLAG)) {
                AmbaMisra_TypeCast32(&FwStartEL1, &pPartEntry->RamLoadAddr);
                BLD_PrintStr("# Load image A \r\n#\r\n");
                if (AmbaRTSL_NandReadPartition(1, AMBA_USER_PARTITION_SYS_SOFTWARE, FwStartEL1) != BLD_ERR_NONE) {
                    BLD_PrintStr("# Kernel image A is not found\r\n#\r\n");
                }
            } else {
                pPartEntry = AmbaRTSL_NandGetPartEntry(1, AMBA_USER_PARTITION_SYS_SOFTWARE_B);
                if ((pPartEntry != NULL) && (pPartEntry->ActualByteSize != 0U) && (pPartEntry->Attribute & FW_PARTITION_ACTIVE_FLAG)) {
                    AmbaMisra_TypeCast32(&FwStartEL1, &pPartEntry->RamLoadAddr);
                    BLD_PrintStr("# Load image B \r\n#\r\n");
                    if (AmbaRTSL_NandReadPartition(1, AMBA_USER_PARTITION_SYS_SOFTWARE_B, FwStartEL1) != BLD_ERR_NONE) {
                        BLD_PrintStr("# Kernel image B is not found\r\n#\r\n");
                    }
                } else {
                    BLD_PrintStr("# Kernel image A & B not found\r\n#\r\n");
                }
            }
#if !defined(CONFIG_BUILD_QNX_IPL)
            AmbaTime_A53Bld(A53_BLD_LOAD_SYS_DONE);
#endif
#elif defined(CONFIG_BLD_SEQ_LINUX) || defined(CONFIG_BLD_SEQ_ATF_LINUX)
#if !defined(CONFIG_BUILD_QNX_IPL)
            AmbaTime_A53Bld(A53_BLD_LOAD_LINUX_START);
#endif
            pPartEntry = AmbaRTSL_NandGetPartEntry(1, AMBA_USER_PARTITION_LINUX_KERNEL);
            if ((pPartEntry != NULL) && (pPartEntry->ActualByteSize != 0U)) {
                FwStartEL1 = (UINT8 *) CONFIG_FWPROG_LNX_LOADADDR;
                if (AmbaRTSL_NandReadPartition(1, AMBA_USER_PARTITION_LINUX_KERNEL, FwStartEL1) != BLD_ERR_NONE) {
                    BLD_PrintStr("# Kernel image is not found\r\n#\r\n");
                }

#if !defined(CONFIG_DEVICE_TREE_SUPPORT)
                /* Prepare dtb */
                {
                    extern AMBA_NAND_DEV_INFO_s *AmbaNAND_GetDevInfo(void);
                    extern UINT32 AmbaNAND_Read(UINT32 PageAddr, UINT32 NumPage, UINT8 *pMainBuf, UINT8 *pSpareBuf, UINT32 TimeOut);
                    extern INT32 AmbaRTSL_NandFindGoodBlkBackwd(UINT32 BlkAddr);
                    UINT32 Rval;
                    UINT32 Block;
                    UINT32 Pages;
                    UINT32 PageAddr;
                    UINT8 *dtbAddr = (UINT8 *)(CONFIG_FWPROG_LNX_LOADADDR & ~0xFFFFF);
                    const AMBA_NAND_COMMON_INFO_s *pNandDevInfo = AmbaRTSL_NandCommonInfo;     /* Pointer to external NAND device information */
#if !defined(CONFIG_BUILD_QNX_IPL)
                    AmbaTime_A53Bld(A53_BLD_LOAD_LINUX_DONE);
#endif

                    /* DTB is programmed to the last good block of Linux partition. */
                    Block = pPartEntry->StartBlkAddr + pPartEntry->BlkCount - 1;
                    Block = AmbaRTSL_NandFindGoodBlkBackwd(Block);

                    PageAddr = Block * pNandDevInfo->BlockPageSize;
#define AMBARELLA_PARAMS_SIZE              0x10000
                    Pages = AMBARELLA_PARAMS_SIZE / pNandDevInfo->MainByteSize;
                    Rval = AmbaRTSL_NandOpRead(PageAddr, Pages, dtbAddr, NULL);
                    if (Rval != 0u) {
                        BLD_PrintStr("# Load DTB failed\r\n#\r\n");
                    } else {
                        extern int fdt_update_memory(void *fdt);
                        extern int fdt_update_chosen(void *fdt);
                        extern int fdt_update_nand(void *fdt);
                        fdt_update_nand(dtbAddr);
                        fdt_update_chosen(dtbAddr);
                        fdt_update_memory(dtbAddr);
                    }
                }
#else
#if !defined(CONFIG_BUILD_QNX_IPL)
                AmbaTime_A53Bld(A53_BLD_LOAD_LINUX_DONE);
#endif
#endif

#ifdef AMBA_BLD_CHECK_FW_CRC32
                BLD_PrintStr("# Check Kernel image Crc32 \r\n#\r\n");
                Crc32 = AmbaUtility_Crc32(FwStartEL1, pPartEntry->ActualByteSize);
                if (Crc32 != pPartEntry->ImageCRC32) {
                    BldStop();
                }
#endif
            }
#elif defined(CONFIG_BLD_SEQ_SHELL)
            /* Do nothing */
#else /* CONFIG_BLD_SEQ_xxx */
#error Incomplete branch.
#endif
        }
    }
#if !defined(CONFIG_BUILD_QNX_IPL)
    if (AmbaBldUserCallbacks.UserCbAftLoadFw != NULL) {
        AmbaBldUserCallbacks.UserCbAftLoadFw();
    }
#endif
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
            void AmbaCSL_FioClearIrqStatus(void);

            AmbaCSL_FioFdmaClearMainStatus();
            AmbaCSL_FioClearIrqStatus();

            AmbaRTSL_GicIntDisable(AMBA_INT_SPI_ID100_FIO);
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

    /* Should not be called */
    BLD_PrintStr("# Boot failed!\r\n#\r\n");
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
    }

    /* Force enter shell if load code failed */
    BLD_ShellStart(1U);
#endif
    return 0;
}

