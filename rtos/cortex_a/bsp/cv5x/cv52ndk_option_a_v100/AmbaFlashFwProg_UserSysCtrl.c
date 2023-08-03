/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaFlashFwProg_UserSysCtrl.c
 *
 *  @Copyright      :: Copyright (C) 2019 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: User defined System Control APIs (board dependent) for Flash Firmware Programmer
\*-------------------------------------------------------------------------------------------------------------------*/

#include "AmbaTypes.h"
#include "AmbaDef.h"
#include "AmbaMisraFix.h"

#include "AmbaNVM_Partition.h"
#include "AmbaUtility.h"
#include "AmbaGPIO.h"
#if defined(CONFIG_ENABLE_EMMC_BOOT)
#include "AmbaRTSL_SD.h"
#endif

#include "AmbaFlashFwProg.h"

#include "AmbaRTSL_NAND.h"
#include "AmbaRTSL_NAND_Ctrl.h"
#include "AmbaRTSL_NAND_BBM.h"
#include "AmbaRTSL_Cache.h"
#include "AmbaRTSL_GIC.h"
#include "AmbaRTSL_MMU.h"
#if defined(CONFIG_ENABLE_SPINOR_BOOT)
#include "AmbaRTSL_SpiNOR.h"
#include "AmbaRTSL_SpiNOR_Ctrl.h"
#include "AmbaRTSL_DMA.h"
#endif

#if defined(CONFIG_ENABLE_SPINAND_BOOT)
#include "AmbaRTSL_SPINAND.h"
#endif

#include "AmbaRTSL_PLL.h"
#include "AmbaRTSL_UART.h"

#include "AmbaRTSL_GPIO.h"
#if defined(CONFIG_EMMC_ACCPART_USER)
extern UINT32 FlashProgEmmcAccessPart;
UINT32 FlashProgEmmcAccessPart = EMMC_ACCP_USER;
#elif defined(CONFIG_EMMC_ACCPART_BP1)
extern UINT32 FlashProgEmmcAccessPart;
UINT32 FlashProgEmmcAccessPart = EMMC_ACCP_BP_1;
#else
extern UINT32 FlashProgEmmcAccessPart;
UINT32 FlashProgEmmcAccessPart = EMMC_ACCP_BP_2;
#endif

#if defined(CONFIG_EMMC_BOOTPART_USER)
extern UINT32 FlashProgEmmcBootPart;
UINT32 FlashProgEmmcBootPart = EMMC_BOOTP_USER;
#elif defined(CONFIG_EMMC_BOOTPART_BP1)
extern UINT32 FlashProgEmmcBootPart;
UINT32 FlashProgEmmcBootPart = EMMC_BOOTP_BP_1;
#else
extern UINT32 FlashProgEmmcBootPart;
UINT32 FlashProgEmmcBootPart = EMMC_BOOTP_BP_2;
#endif

#if defined(CONFIG_EMMC_BOOT_1BIT)
extern UINT32 FlashProgEmmcBootBus;
UINT32 FlashProgEmmcBootBus = ((UINT32)EMMC_BOOT_1BIT | (UINT32)EMMC_BOOT_HIGHSPEED);
#elif defined(CONFIG_EMMC_BOOT_4BIT)
extern UINT32 FlashProgEmmcBootBus;
UINT32 FlashProgEmmcBootBus = ((UINT32)EMMC_BOOT_4BIT | (UINT32)EMMC_BOOT_HIGHSPEED);
#else
extern UINT32 FlashProgEmmcBootBus;
UINT32 FlashProgEmmcBootBus = ((UINT32)EMMC_BOOT_8BIT | (UINT32)EMMC_BOOT_HIGHSPEED);
#endif

extern AMBA_PARTITION_CONFIG_s AmbaNAND_UserPartConfig[];
extern AMBA_PARTITION_CONFIG_s AmbaNAND_SysPartConfig[];

extern AMBA_PARTITION_CONFIG_s AmbaNAND_UserPartConfig[AMBA_NUM_USER_PARTITION];
extern AMBA_PARTITION_CONFIG_s AmbaNAND_SysPartConfig[AMBA_NUM_SYS_PARTITION];

extern void AmbaRTSL_MmuSetupPrimaryFwprog(void);
extern AMBA_GPIO_DEFAULT_s GpioPinGrpConfig;

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: FlashFwProg_ShowImgHeader
 *
 *  @Description:: Show image header
 *
 *  @Input      ::
 *      pHeader: pointer to the image header
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void FlashFwProg_ShowImgHeader(const AMBA_IMG_HEADER *pHeader)
{
    AmbaMisra_TouchUnused(&pHeader);
    AmbaPrint_PrintUInt5Fwprog("\r    Crc32  :    0x%08x\r\n",      pHeader->Crc32, 0, 0, 0, 0);
    AmbaPrint_PrintUInt5Fwprog("\r    Version:    %d.%d\r\n",       pHeader->VerNum >> 16U, (pHeader->VerNum & 0xffffU), 0, 0, 0);
    AmbaPrint_PrintUInt5Fwprog("\r    VerDate:    %d/%d/%d\r\n",    pHeader->VerDate >> 16U, ((pHeader->VerDate >> 8U) & 0xffU), (pHeader->VerDate & 0xffU), 0, 0);
    AmbaPrint_PrintUInt5Fwprog("\r    ImgLen :    %d\r\n",          pHeader->ImgLen, 0, 0, 0, 0);
    AmbaPrint_PrintUInt5Fwprog("\r    MemAddr:    0x%08x\r\n",      pHeader->MemAddr, 0, 0, 0, 0);
    AmbaPrint_PrintUInt5Fwprog("\r    Magic  :    0x%08x\r\n",      pHeader->Magic, 0, 0, 0, 0);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: FlashFwProg_ShowProgramResult
 *
 *  @Description:: Show image program result
 *
 *  @Input      ::
 *      pName:      image name string
 *      ErrCode:    image program result
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void FlashFwProg_ShowProgramResult(const char *pName, INT32 ErrCode)
{
    AmbaMisra_TouchUnused(&pName);

    if (ErrCode >= 0) {
        AmbaPrint_PrintStr5Fwprog("\r%s program ok\r\n\r\n", pName, NULL, NULL, NULL, NULL);
    } else {
        AmbaPrint_PrintStr5Fwprog("\r%s program failed\r\n\r\n", pName, NULL, NULL, NULL, NULL);
    }
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: FlashFwProg_ShowSummaryReport
 *
 *  @Description:: Show summary of the program result of each image
 *
 *  @Input      ::
 *      pName:  image name string
 *      Flag:   image program result
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void FlashFwProg_ShowSummaryReport(const char *pName, UINT32 Flag)
{
    AmbaMisra_TouchUnused(&pName);

    if ((Flag & FWPROG_RESULT_FLAG_CODE_MASK) == 0U) {
        AmbaPrint_PrintStr5Fwprog("\r%s: SUCCESS\r\n\n", pName, NULL, NULL, NULL, NULL);
    } else {
        AmbaPrint_PrintStr5Fwprog("\r%s: FAILURE\r\n\n", pName, NULL, NULL, NULL, NULL);
    }
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: FlashFwProg_ValidateImg
 *
 *  @Description:: Validate the content of the image supplied by the caller.
 *
 *  @Input      ::
 *      pImg:   Pointer to image.
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(<0)
 *-----------------------------------------------------------------------------------------------*/
static INT32 FlashFwProg_ValidateImg(const UINT8 *pImg)
{
    UINT32 RawCrc32;
    INT32 Rval = FLPROG_OK;
    const AMBA_IMG_HEADER *pHeader = NULL;

    AmbaMisra_TypeCast64(&pHeader, &pImg);

    if ((pHeader->Magic != PART_HEADER_MAGIC_NUM)) {
        AmbaPrint_PrintStr5Fwprog("\r\nwrong magic!\r\n", NULL, NULL, NULL, NULL, NULL);
        Rval = FLPROG_ERR_MAGIC;
    }

    if ((Rval == FLPROG_OK) && (pHeader->VerNum == 0x0U)) {
        AmbaPrint_PrintStr5Fwprog("\r\ninvalid version!\r\n", NULL, NULL, NULL, NULL, NULL);
        Rval = FLPROG_ERR_VER_NUM;
    }

    if ((Rval == FLPROG_OK) && (pHeader->VerDate == 0x0U)) {
        AmbaPrint_PrintStr5Fwprog("\r\ninvalid date!\r\n", NULL, NULL, NULL, NULL, NULL);
        Rval = FLPROG_ERR_VER_DATE;
    }

    AmbaPrint_PrintStr5Fwprog("\r\nVerifying image crc ...", NULL, NULL, NULL, NULL, NULL);
    RawCrc32 = AmbaUtility_Crc32(&pImg[sizeof(AMBA_IMG_HEADER)], pHeader->ImgLen);

    if ((Rval == FLPROG_OK) && (RawCrc32 != pHeader->Crc32)) {
        AmbaPrint_PrintUInt5Fwprog("FAILED (0x%08x != 0x%08x)\r\n", RawCrc32, pHeader->Crc32, 0, 0, 0);
        Rval = FLPROG_ERR_CRC;
    } else {
        AmbaPrint_PrintStr5Fwprog("PASS\r\n", NULL, NULL, NULL, NULL, NULL);
        Rval = FLPROG_OK;
    }
    return Rval;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: FlashFwProg_GetImgInfo
 *
 *  @Description:: Get image info
 *
 *  @Input      :: none
 *
 *  @Output     ::
 *      pSysImgAddr:    DRAM address of system partition images
 *      pSysImgSize:    Byte size of system partition images
 *      pUserImgAddr:   DRAM address of user partition images
 *      pUserImgSize:   Byte size of user partition images
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void FlashFwProg_GetImgInfo(UINT32 *pSysImgAddr, UINT32 *pSysImgSize, UINT32 *pUserImgAddr, UINT32 *pUserImgSize)
{
    extern UINT32 begin_bst_image, end_bst_image;
    extern UINT32 begin_bld_image, end_bld_image;
    extern UINT32 begin_pba_image, end_pba_image;
    extern UINT32 begin_atf_image, end_atf_image;
    extern UINT32 begin_ipl_image, end_ipl_image;
    extern UINT32 begin_sys_image, end_sys_image;
    extern UINT32 begin_rom_image, end_rom_image;
    extern UINT32 begin_dsp_image, end_dsp_image;
    extern UINT32 begin_lnx_image, end_lnx_image;
    extern UINT32 begin_rfs_image, end_rfs_image;
    extern UINT32 begin_xen_image, end_xen_image;
    extern UINT32 begin_xtb_image, end_xtb_image;
    extern UINT32 begin_x0k_image, end_x0k_image;
    extern UINT32 begin_x0d_image, end_x0d_image;

    UINT32 BstImageBegin = (UINT32) begin_bst_image;
    UINT32 BldImageBegin = (UINT32) begin_bld_image;
    UINT32 PbaImageBegin = (UINT32) begin_pba_image;
    UINT32 IplImageBegin = (UINT32) begin_ipl_image;
    UINT32 AtfImageBegin = (UINT32) begin_atf_image;
    UINT32 SysImageBegin = (UINT32) begin_sys_image;
    UINT32 DspImageBegin = (UINT32) begin_dsp_image;
    UINT32 RomImageBegin = (UINT32) begin_rom_image;
    UINT32 LnxImageBegin = (UINT32) begin_lnx_image;
    UINT32 RfsImageBegin = (UINT32) begin_rfs_image;
    UINT32 XenImageBegin = (UINT32) begin_xen_image;
    UINT32 XtbImageBegin = (UINT32) begin_xtb_image;
    UINT32 X0kImageBegin = (UINT32) begin_x0k_image;
    UINT32 X0dImageBegin = (UINT32) begin_x0d_image;

    UINT32 BstImageEnd   = (UINT32) end_bst_image;
    UINT32 BldImageEnd   = (UINT32) end_bld_image;
    UINT32 PbaImageEnd   = (UINT32) end_pba_image;
    UINT32 IplImageEnd   = (UINT32) end_ipl_image;
    UINT32 AtfImageEnd   = (UINT32) end_atf_image;
    UINT32 SysImageEnd   = (UINT32) end_sys_image;
    UINT32 DspImageEnd   = (UINT32) end_dsp_image;
    UINT32 RomImageEnd   = (UINT32) end_rom_image;
    UINT32 LnxImageEnd   = (UINT32) end_lnx_image;
    UINT32 RfsImageEnd   = (UINT32) end_rfs_image;
    UINT32 XenImageEnd   = (UINT32) end_xen_image;
    UINT32 XtbImageEnd   = (UINT32) end_xtb_image;
    UINT32 X0kImageEnd   = (UINT32) end_x0k_image;
    UINT32 X0dImageEnd   = (UINT32) end_x0d_image;

    /* Calculate the firmware payload offsets of images */
    pSysImgAddr[AMBA_SYS_PARTITION_BOOTSTRAP]       = BstImageBegin;
    pSysImgAddr[AMBA_SYS_PARTITION_BOOTLOADER]      = BldImageBegin;
    pSysImgAddr[AMBA_SYS_PARTITION_FW_UPDATER]      = PbaImageBegin;
    pSysImgAddr[AMBA_SYS_PARTITION_ARM_TRUST_FW]    = AtfImageBegin;
    pSysImgAddr[AMBA_SYS_PARTITION_QNX_IPL]         = IplImageBegin;

    pUserImgAddr[AMBA_USER_PARTITION_PTB]           = 0;
    pUserImgAddr[AMBA_USER_PARTITION_SYS_SOFTWARE]  = SysImageBegin;
    pUserImgAddr[AMBA_USER_PARTITION_DSP_uCODE]     = DspImageBegin;
    pUserImgAddr[AMBA_USER_PARTITION_SYS_DATA]      = RomImageBegin;

    pUserImgAddr[AMBA_USER_PARTITION_XEN]           = XenImageBegin;
    pUserImgAddr[AMBA_USER_PARTITION_XTB]           = XtbImageBegin;
    pUserImgAddr[AMBA_USER_PARTITION_X0K]           = X0kImageBegin;
    pUserImgAddr[AMBA_USER_PARTITION_X0D]           = X0dImageBegin;
    pUserImgAddr[AMBA_USER_PARTITION_LINUX_KERNEL]  = LnxImageBegin;
    pUserImgAddr[AMBA_USER_PARTITION_LINUX_ROOT_FS] = RfsImageBegin;

    pSysImgSize[AMBA_SYS_PARTITION_BOOTSTRAP]       = BstImageEnd - BstImageBegin;
    pSysImgSize[AMBA_SYS_PARTITION_BOOTLOADER]      = BldImageEnd - BldImageBegin;
    pSysImgSize[AMBA_SYS_PARTITION_FW_UPDATER]      = PbaImageEnd - PbaImageBegin;
    pSysImgSize[AMBA_SYS_PARTITION_QNX_IPL]         = IplImageEnd - IplImageBegin;

    pSysImgSize[AMBA_SYS_PARTITION_ARM_TRUST_FW]    = AtfImageEnd - AtfImageBegin;

    pUserImgSize[AMBA_USER_PARTITION_PTB]           = 0;
    pUserImgSize[AMBA_USER_PARTITION_SYS_SOFTWARE]  = SysImageEnd - SysImageBegin;
    pUserImgSize[AMBA_USER_PARTITION_DSP_uCODE]     = DspImageEnd - DspImageBegin;
    pUserImgSize[AMBA_USER_PARTITION_SYS_DATA]      = RomImageEnd - RomImageBegin;

    pUserImgSize[AMBA_USER_PARTITION_XEN]           = XenImageEnd - XenImageBegin;
    pUserImgSize[AMBA_USER_PARTITION_XTB]           = XtbImageEnd - XtbImageBegin;
    pUserImgSize[AMBA_USER_PARTITION_X0K]           = X0kImageEnd - X0kImageBegin;
    pUserImgSize[AMBA_USER_PARTITION_X0D]           = X0dImageEnd - X0dImageBegin;
    pUserImgSize[AMBA_USER_PARTITION_LINUX_KERNEL]  = LnxImageEnd - LnxImageBegin;
    pUserImgSize[AMBA_USER_PARTITION_LINUX_ROOT_FS] = RfsImageEnd - RfsImageBegin;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaFlashFwProg_DeviceInit
 *
 *  @Description:: NAND Initializations
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
INT32 AmbaFlashFwProg_DeviceInit(void)
{
    INT32 Rval = FLPROG_OK;

#if defined(CONFIG_ENABLE_EMMC_BOOT)
    extern void AmbaUserSD_PowerCtrl(UINT32, UINT32);
    AMBA_SD_CONFIG_s SdConfig;

#if 0
    // GPIO ALT function should be configured by BSP.
    // Doing this is just for debug purpose.
    (void)AmbaRTSL_GpioSetFuncAlt(GPIO_PIN_87_SD0_CLK);
    (void)AmbaRTSL_GpioSetFuncAlt(GPIO_PIN_88_SD0_CMD);
    (void)AmbaRTSL_GpioSetFuncAlt(GPIO_PIN_79_SD0_DATA0);
    (void)AmbaRTSL_GpioSetFuncAlt(GPIO_PIN_80_SD0_DATA1);
    (void)AmbaRTSL_GpioSetFuncAlt(GPIO_PIN_81_SD0_DATA2);
    (void)AmbaRTSL_GpioSetFuncAlt(GPIO_PIN_82_SD0_DATA3);
    (void)AmbaRTSL_GpioSetFuncAlt(GPIO_PIN_83_SD0_DATA4);
    (void)AmbaRTSL_GpioSetFuncAlt(GPIO_PIN_84_SD0_DATA5);
    (void)AmbaRTSL_GpioSetFuncAlt(GPIO_PIN_85_SD0_DATA6);
    (void)AmbaRTSL_GpioSetFuncAlt(GPIO_PIN_86_SD0_DATA7);

    (void)AmbaRTSL_GpioSetFuncGPO(GPIO_PIN_129_SD0_HS_SEL, AMBA_GPIO_LEVEL_LOW);
    (void)AmbaRTSL_GpioSetFuncGPO(GPIO_PIN_91_SD0_RESET, AMBA_GPIO_LEVEL_HIGH);
#endif

    (void) AmbaRTSL_SdInit();

    (void)AmbaWrap_memset(&SdConfig, 0, sizeof(AMBA_SD_CONFIG_s));
    SdConfig.PowerCtrl           = NULL;
    SdConfig.PhyCtrl             = NULL;
    SdConfig.SdSetting.ClockDrive    = AMBA_SD_DRIVE_STRENGTH_12MA;
    SdConfig.SdSetting.DataDrive     = AMBA_SD_DRIVE_STRENGTH_12MA;
    SdConfig.SdSetting.CMDDrive      = AMBA_SD_DRIVE_STRENGTH_12MA;
    SdConfig.SdSetting.WPDrive       = AMBA_SD_DRIVE_STRENGTH_12MA;
    SdConfig.SdSetting.CDDrive       = AMBA_SD_DRIVE_STRENGTH_12MA;
    SdConfig.SdSetting.InitFrequency = 300000U;
    SdConfig.SdSetting.MaxFrequency  = CONFIG_EMMC_MAX_CLOCK;

    SdConfig.SmpCoreSet          = 0x1U;
    SdConfig.Priority            = 32U;

    SdConfig.pSysPartConfig      = AmbaNAND_SysPartConfig;
    SdConfig.pUserPartConfig     = AmbaNAND_UserPartConfig;

    AmbaRTSL_EmmcSetWritePtbFlag(1U);

    (void)AmbaRTSL_SdConfig(AMBA_SD_CHANNEL0, &SdConfig);

    Rval = AmbaRTSL_SdCardInit(AMBA_SD_CHANNEL0);

    if (AmbaRTSL_EmmcInitPtbBbt() != OK) {
        AmbaPrint_PrintStr5Fwprog("\r\n# eMMC init PtbBbt fail#\r\n", NULL, NULL, NULL, NULL, NULL);
        while(1);
    }

#elif defined(CONFIG_ENABLE_SPINOR_BOOT)
    extern void AmbaRTSL_NorSpiSetWritePtbFlag(UINT32 Flag);
    extern AMBA_NORSPI_DEV_INFO_s AmbaNORSPI_DevInfo;
    extern AMBA_PARTITION_CONFIG_s AmbaNORSPI_SysPartConfig[];
    extern AMBA_PARTITION_CONFIG_s AmbaNORSPI_UserPartConfig[];
    extern AMBA_SERIAL_SPI_CONFIG_s AmbaNOR_SPISetting;

    UINT32 DmaTxChan = 0, DmaRxChan = 0;

    AMBA_NOR_SPI_CONFIG_s NorSpiConfig = {
        .pNorSpiDevInfo  = &AmbaNORSPI_DevInfo,
        .pSysPartConfig  = AmbaNORSPI_SysPartConfig,
        .pUserPartConfig = AmbaNORSPI_UserPartConfig,
        .SpiSetting      = &AmbaNOR_SPISetting,
    };

    static AMBA_SYS_PARTITION_TABLE_s  AmbaRTSL_NorSysPartTable  GNU_ALIGNED_CACHESAFE;
    static AMBA_USER_PARTITION_TABLE_s AmbaRTSL_NorUserPartTable GNU_ALIGNED_CACHESAFE;

    _pAmbaRTSL_NorSpiSysPartTable  = &AmbaRTSL_NorSysPartTable;
    _pAmbaRTSL_NorSpiUserPartTable = &AmbaRTSL_NorUserPartTable;

#if 1
    // GPIO ALT function should be configured by BSP.
    // Doing this is just for debug purpose.
    AmbaRTSL_GpioSetFuncAlt(GPIO_PIN_51_NOR_SPI_CLK);
    AmbaRTSL_GpioSetFuncAlt(GPIO_PIN_52_NOR_SPI_EN0);
    AmbaRTSL_GpioSetFuncAlt(GPIO_PIN_53_NOR_SPI_DATA0);
    AmbaRTSL_GpioSetFuncAlt(GPIO_PIN_54_NOR_SPI_DATA1);
    AmbaRTSL_GpioSetFuncAlt(GPIO_PIN_55_NOR_SPI_DATA2);
    AmbaRTSL_GpioSetFuncAlt(GPIO_PIN_56_NOR_SPI_DATA3);
    AmbaRTSL_GpioSetFuncAlt(GPIO_PIN_57_NOR_SPI_DATA4);
    AmbaRTSL_GpioSetFuncAlt(GPIO_PIN_58_NOR_SPI_DATA5);
    AmbaRTSL_GpioSetFuncAlt(GPIO_PIN_59_NOR_SPI_DATA6);
    AmbaRTSL_GpioSetFuncAlt(GPIO_PIN_60_NOR_SPI_DATA7);
    AmbaRTSL_GpioSetFuncAlt(GPIO_PIN_61_NOR_SPI_DQS);
#endif

    /* Set DMA channel to SpiNOR RX and TX */
    AmbaRTSL_DmaInit();
    AmbaRTSL_DmaChanAllocate(AMBA_DMA_CHANNEL_NOR_SPI_TX, &DmaTxChan);
    AmbaRTSL_DmaChanAllocate(AMBA_DMA_CHANNEL_NOR_SPI_RX, &DmaRxChan);

    AmbaRTSL_AllocateNorSpiDMAChan(DmaRxChan, DmaTxChan);

    AmbaRTSL_NorSpiSetWritePtbFlag(1U);

    if (AmbaRTSL_SpiNORConfig(&NorSpiConfig) != OK) {
        AmbaPrint_PrintStr5Fwprog("\r\nWARNING: Incorrect SpiNOR device info!!\r\n", NULL, NULL, NULL, NULL, NULL);
        return -1;
    }

    if (AmbaRTSL_NorSpiInitPtbBbt() != OK) {
        AmbaPrint_PrintStr5Fwprog("\r\nwarning: partition data maybe incorrect!!\r\n", NULL, NULL, NULL, NULL, NULL);
    }

#if 0
    (void)AmbaWrap_memset(_pAmbaRTSL_NorSpiSysPartTable,  0xff, sizeof(AMBA_SYS_PARTITION_TABLE_s));
    (void)AmbaWrap_memset(_pAmbaRTSL_NorSpiUserPartTable, 0xff, sizeof(AMBA_USER_PARTITION_TABLE_s));
    AmbaRTSL_NorEraseBlock(0);
    AmbaRTSL_NorWaitInt();
    AmbaRTSL_NorSpiInitPtbBbt();
#endif

#elif defined(CONFIG_ENABLE_SPINAND_BOOT)
    extern AMBA_SPINAND_DEV_INFO_s AmbaSpiNAND_DevInfo;
    AMBA_SPINAND_CONFIG_s SpiNandConfig = {
        .pNandDevInfo    = &AmbaSpiNAND_DevInfo,        /* pointer to NAND flash device information */
        .pSysPartConfig  = AmbaNAND_SysPartConfig,      /* number of User Partitions */
        .pUserPartConfig = AmbaNAND_UserPartConfig,     /* pointer to partition configurations */
    };

#if 1
    // GPIO ALT function should be configured by BSP.
    // Doing this is just for debug purpose.
    AmbaRTSL_GpioSetFuncAlt(GPIO_PIN_51_NAND_SPI_CLK);
    AmbaRTSL_GpioSetFuncAlt(GPIO_PIN_53_NAND_SPI_DATA0);
    AmbaRTSL_GpioSetFuncAlt(GPIO_PIN_54_NAND_SPI_DATA1);
    AmbaRTSL_GpioSetFuncAlt(GPIO_PIN_55_NAND_SPI_DATA2);
    AmbaRTSL_GpioSetFuncAlt(GPIO_PIN_56_NAND_SPI_DATA3);
    AmbaRTSL_GpioSetFuncAlt(GPIO_PIN_52_NAND_SPI_CS);
#endif
    AmbaPrint_PrintStr5Fwprog("\r\nProgram to SPINAND\r\n", NULL, NULL, NULL, NULL, NULL);

    if (AmbaRTSL_SpiNandInit() != OK) {
        return -1;
    }

    AmbaRTSL_NandSetWritePtbFlag(1);

    if (AmbaRTSL_SpiNandConfig(&SpiNandConfig) != OK) {
        AmbaPrint_PrintStr5Fwprog("\r\nWARNING: Incorrect NAND device info!!\r\n", NULL, NULL, NULL, NULL, NULL);
        return -1;
    }

    AmbaRTSL_SpiNand_ConfigOnDevECC();

    /* Init BBT and System/User Partition Tables */
    if (AmbaRTSL_NandInitPtbBbt() != OK) {
        AmbaPrint_PrintStr5Fwprog("\r\nWARNING: Incorrect NAND partition info!!\r\n", NULL, NULL, NULL, NULL, NULL);
    }

#if 0
    (void)AmbaWrap_memset(pAmbaNandTblSysPart,  0xff, sizeof(AMBA_SYS_PARTITION_TABLE_s));
    (void)AmbaWrap_memset(pAmbaNandTblUserPart, 0xff, sizeof(AMBA_USER_PARTITION_TABLE_s));
    AmbaRTSL_SpiNandEraseBlock(0);
    AmbaRTSL_NandInitPtbBbt();
#endif


#else
    AmbaPrint_PrintStr5Fwprog("\r\nWARNING: Do not support P-NAND device !!\r\n", NULL, NULL, NULL, NULL, NULL);
#endif
    return Rval;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: FlashFwProg_TaskEntry
 *
 *  @Description:: main entry of Flash Firmware Programming
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void FlashFwProg_TaskEntry(void)
{
    extern UINT32 *__memfwprog_result;
    AMBA_FWPROG_RESULT_s *pResult = (AMBA_FWPROG_RESULT_s *)&__memfwprog_result;
    INT32  RetVal = 0, FwProgResult = 0;
    UINT32 i;
    UINT32 Code;
    const AMBA_IMG_HEADER *pImgHeader = NULL;
    UINT8 *pImg;

    UINT32 SysPartLen[AMBA_NUM_SYS_PARTITION]      = {0};
    UINT32 UserPartLen[AMBA_NUM_USER_PARTITION]    = {0};
    UINT32 SysImageStart[AMBA_NUM_SYS_PARTITION]   = {0};
    UINT32 UserImageStart[AMBA_NUM_USER_PARTITION] = {0};

    static const char *pFwProgBanner =
        "\r\n"                                                                   \
        "----------------------------------------------------------------\r\n"   \
        " NAND/eMMC/SPI-NOR Flash Programming Utility\r\n"                       \
        " Copyright (C) 2019 Ambarella Corporation. All rights reserved.\r\n"    \
        "----------------------------------------------------------------\r\n";

    AmbaPrint_PrintStr5Fwprog("%s", pFwProgBanner, NULL, NULL, NULL, NULL);

    /* Init NAND/EMMC/SPI-Nor device */
    if (AmbaFlashFwProg_DeviceInit() != FLPROG_OK) {
        AmbaPrint_PrintStr5Fwprog("\r\nERROR: Cannot access flash device!!\r\n", NULL, NULL, NULL, NULL, NULL);
        __asm__ __volatile__ ("B .");
    }

    (void)AmbaWrap_memset(pResult, 0x0, sizeof(AMBA_FWPROG_RESULT_s));
    pResult->Magic = FWPROG_RESULT_MAGIC;

    FlashFwProg_GetImgInfo(SysImageStart, SysPartLen, UserImageStart, UserPartLen);

    /* Program System partition */
    for (i = AMBA_SYS_PARTITION_BOOTSTRAP; i < AMBA_NUM_SYS_PARTITION; i++) {
        if (SysPartLen[i] == 0U) {
            continue;
        }

        AmbaPrint_PrintStr5Fwprog("\r\n%s found in firmware!\r\n", AmbaFlashFwProg_GetSysPartitionName(i), NULL, NULL, NULL, NULL);

        pImgHeader = NULL;
        AmbaMisra_TypeCast32(&pImgHeader, &SysImageStart[i]);
        FlashFwProg_ShowImgHeader(pImgHeader);

        pImg = NULL;
        AmbaMisra_TypeCast32(&pImg, &SysImageStart[i]);
        RetVal = FlashFwProg_ValidateImg(pImg);
        if (RetVal >= 0) {
            RetVal = AmbaFwProg_ProgramSysPartition(i, pImg);
        }

        (void)AmbaWrap_abs(RetVal, &Code);

        pResult->Flag[i] = (Code << 24U) | (SysPartLen[i] & FWPROG_RESULT_FLAG_LEN_MASK);

        if (RetVal == FLPROG_ERR_PROG_IMG) {
            pResult->BadBlockInfo |= ((UINT32)0x1U << i);
        }

        FlashFwProg_ShowProgramResult(AmbaFlashFwProg_GetSysPartitionName(i), RetVal);
    }

    for (i = AMBA_USER_PARTITION_SYS_SOFTWARE; i < AMBA_NUM_USER_PARTITION; i++) {
        if (UserPartLen[i] == 0U) {
            continue;
        }

        AmbaPrint_PrintStr5Fwprog("\r\n%s found in firmware!\r\n", AmbaFlashFwProg_GetUserPartitionName(i), NULL, NULL, NULL, NULL);

        AmbaMisra_TypeCast32(&pImgHeader, &UserImageStart[i]);
        FlashFwProg_ShowImgHeader(pImgHeader);

        pImg = NULL;
        AmbaMisra_TypeCast32(&pImg, &UserImageStart[i]);
        RetVal = FlashFwProg_ValidateImg(pImg);
        if (RetVal >= 0) {
            RetVal = AmbaFwProg_ProgramUserPartition(i, pImg);
        }

        (void)AmbaWrap_abs(RetVal, &Code);

        pResult->Flag[i + AMBA_NUM_SYS_PARTITION] = (Code << 24U) | (SysPartLen[i] & FWPROG_RESULT_FLAG_LEN_MASK);

        if (RetVal == FLPROG_ERR_PROG_IMG) {
            pResult->BadBlockInfo |= ((UINT32)0x1U << i);
        }

        FlashFwProg_ShowProgramResult(AmbaFlashFwProg_GetUserPartitionName(i), RetVal);
    }

    AmbaPrint_PrintStr5Fwprog("\r\n------ Report ------\r\n\n", NULL, NULL, NULL, NULL, NULL);
    for (i = 0; i < AMBA_NUM_SYS_PARTITION; i++) {
        if (SysPartLen[i] != 0U) {
            FlashFwProg_ShowSummaryReport(AmbaFlashFwProg_GetSysPartitionName(i), pResult->Flag[i]);
        }
        if((pResult->Flag[i] & FWPROG_RESULT_FLAG_CODE_MASK) != 0U) {
            FwProgResult = -1;
        }
    }
    for (i = 0; i < AMBA_NUM_USER_PARTITION; i++) {
        if (UserPartLen[i] != 0U) {
            FlashFwProg_ShowSummaryReport(AmbaFlashFwProg_GetUserPartitionName(i), pResult->Flag[i + AMBA_NUM_SYS_PARTITION]);
        }
        if((pResult->Flag[i] & FWPROG_RESULT_FLAG_CODE_MASK) != 0U) {
            FwProgResult = -1;
        }
    }

    if ((pResult->Flag[i] & FWPROG_RESULT_FLAG_CODE_MASK) != 0U) {
        FwProgResult = -1;
    }

    if (FwProgResult == 0) {
        AmbaPrint_PrintStr5Fwprog("\r\n- Program Terminated: SUCCESS -\r\n", NULL, NULL, NULL, NULL, NULL);
    } else {
        AmbaPrint_PrintStr5Fwprog("\r\n- Program Terminated: FAIL -\r\n", NULL, NULL, NULL, NULL, NULL);
    }

    //AmbaDelayCycles(0x400000);

    AmbaPrint_PrintStr5Fwprog("\r\r\r\r\r\r\r\r\r\r\r\r\r\r\r\r", NULL, NULL, NULL, NULL, NULL);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaUserSysInitPostOS
 *
 *  @Description:: User defined System Initializations (after OS running)
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
INT32 main(void)
{
    /* The buffer is used to queue characters from UART RX */
    static UINT8 AmbaUartRxRingBuf[1024] GNU_SECTION_NOZEROINIT;
    static AMBA_UART_CONFIG_s AmbaUartConfig = {
        .NumDataBits   = AMBA_UART_DATA_8_BIT,                    /* number of data bits */
        .ParityBitMode = AMBA_UART_PARITY_NONE,                    /* parity */
        .NumStopBits   = AMBA_UART_STOP_1_BIT,                    /* number of stop bits */
    };

    AmbaRTSL_MmuSetupPrimaryFwprog();   /* Init MMU with external cache disagbled */

    AmbaRTSL_PllInit(24000000U);         /* PLL RTSL Initializations */

    AmbaRTSL_GpioLoadDefaultVals(&GpioPinGrpConfig); /* GPIO Initializations before OS running */

    (void)AmbaRTSL_UartInit();
    (void)AmbaRTSL_UartConfig(AMBA_UART_APB_CHANNEL0, 24000000U, 115200U, &AmbaUartConfig);
    (void)AmbaRTSL_UartHookRxBuf(AMBA_UART_APB_CHANNEL0, sizeof(AmbaUartRxRingBuf), AmbaUartRxRingBuf);
#if 0
    AmbaPrint_PrintStr5Fwprog("\x1b[4l", NULL, NULL, NULL, NULL, NULL); /* Set terminal to replacement mode */
#endif
    AmbaPrint_PrintStr5Fwprog("\r\n", NULL, NULL, NULL, NULL, NULL); /* First, output a blank line to UART */

    FlashFwProg_TaskEntry(); /* start FlashFwProg task */

    AmbaFlashFwProg_ReturnToUsb();

    return 0;
}

