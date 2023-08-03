/**
*  @file SvcBootDev.c
 *
 * Copyright (c) [2020] Ambarella International LP
 *
 * This file and its contents ("Software") are protected by intellectual
 * property rights including, without limitation, U.S. and/or foreign
 * copyrights. This Software is also the confidential and proprietary
 * information of Ambarella International LP and its licensors. You may not use, reproduce,
 * disclose, distribute, modify, or otherwise prepare derivative works of this
 * Software or any portion thereof except pursuant to a signed license agreement
 * or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
 * In the absence of such an agreement, you agree to promptly notify and return
 * this Software to Ambarella International LP.
 *
 * This file includes sample code and is only for internal testing and evaluation.  If you 
 * distribute this sample code (whether in source, object, or binary code form), it will be 
 * without any warranty or indemnity protection from Ambarella International LP or its affiliates.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 * MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
*
*  @details svc boot device
*
*/
#include ".svc_autogen"

#include "AmbaMisraFix.h"
#include "AmbaWrap.h"
#include "AmbaGPIO.h"
#if defined(CONFIG_ENABLE_NAND_BOOT)
#include "AmbaNAND.h"
#elif defined(CONFIG_ENABLE_SPINOR_BOOT)
#include "AmbaSpiNOR_Def.h"
#include "AmbaSPINOR.h"
#include "AmbaRTSL_SpiNOR.h"
#if defined(CONFIG_SPINOR_DMA_ENABLE)
#include "AmbaDMA.h"
#endif
#elif defined(CONFIG_ENABLE_SPINAND_BOOT)
#include "AmbaSPINAND.h"
#endif

#if defined(CONFIG_ENABLE_EMMC_BOOT) || defined(CONFIG_MUTI_BOOT_DEVICE)
#include "AmbaSD.h"
#include "AmbaSD_Ctrl.h"
#include "AmbaRTSL_SD.h"
#endif

#include "SvcLog.h"
#include "SvcErrCode.h"
#include "SvcNvm.h"
#include "SvcBootDev.h"
#include "SvcFwUpdate.h"

#if defined(CONFIG_AMBALINK_BOOT_OS)
extern void AmbaIPC_NandLock(void);
extern void AmbaIPC_NandUnlock(void);
#endif

#define SVC_LOG_BDEV    "BDEV"

/**
* initialization of boot device
* @param [in] pfnEMMCBoot emmc boot function
* @return 0-OK, 1-NG
*/
UINT32 SvcBootDev_Init(CB_EMMC_BOOT_t pfnEMMCBoot)
{
    UINT32  Rval = SVC_NG;
    SvcLog_OK(SVC_LOG_BDEV, "## SvcBootDev_Init START", 0U, 0U);

#if defined(CONFIG_ENABLE_NAND_BOOT)
    {
        extern AMBA_NAND_DEV_INFO_s    AmbaNAND_DevInfo;
        extern AMBA_PARTITION_CONFIG_s AmbaNAND_SysPartConfig[AMBA_NUM_SYS_PARTITION];
        extern AMBA_PARTITION_CONFIG_s AmbaNAND_UserPartConfig[AMBA_NUM_USER_PARTITION];

        AMBA_NAND_CONFIG_s NandConfig = {
            .pNandDevInfo             = &AmbaNAND_DevInfo,
            .pSysPartConfig           = &(AmbaNAND_SysPartConfig[0]),
            .pUserPartConfig          = &(AmbaNAND_UserPartConfig[0]),
            #if defined(CONFIG_AMBALINK_BOOT_OS)
            .AmbaNandPreAccessNotify  = AmbaIPC_NandLock,
            .AmbaNandPostAccessNotify = AmbaIPC_NandUnlock,
            #else
            .AmbaNandPreAccessNotify  = NULL,
            .AmbaNandPostAccessNotify = NULL
            #endif
        };

        Rval = AmbaNAND_Config(&NandConfig);
        if (Rval != SVC_OK) {
            SvcLog_NG(SVC_LOG_BDEV, "## AmbaNAND_Config fail, return 0x%x", Rval, 0U);
        } else {
            SvcLog_OK(SVC_LOG_BDEV, "## AmbaNAND_Config Done", 0U, 0U);
        }
    }

#elif defined(CONFIG_ENABLE_EMMC_BOOT)
    if (pfnEMMCBoot != NULL) {
        Rval = pfnEMMCBoot();
        if (Rval != SVC_OK) {
            SvcLog_NG(SVC_LOG_BDEV, "## pfnEMMCBoot fail, return 0x%x", Rval, 0U);
        } else {
            SvcLog_OK(SVC_LOG_BDEV, "## pfnEMMCBoot Done", 0U, 0U);
        }
    } else {
        Rval = AmbaSD_CardInit(AMBA_SD_CHANNEL0);
        if (Rval != SVC_OK) {
            SvcLog_NG(SVC_LOG_BDEV, "## EMMCBoot fail, return 0x%x", Rval, 0U);
        } else {
            SvcLog_OK(SVC_LOG_BDEV, "## EMMCBoot Done", 0U, 0U);

            Rval = AmbaEMMC_InitPtbBbt(1000);
            if (Rval != SVC_OK) {
                SvcLog_NG(SVC_LOG_BDEV, "## EMMC InitPtbBbt fail, return 0x%x", Rval, 0U);
            } else {
                SvcLog_OK(SVC_LOG_BDEV, "## EMMC InitPtbBbt Done", 0U, 0U);
                Rval = AmbaEMMC_LoadNvmRomFileTable();
                if (Rval != SVC_OK) {
                    SvcLog_NG(SVC_LOG_BDEV, "## EMMC LoadNvmRom fail, return 0x%x", Rval, 0U);
                } else {
                    SvcLog_OK(SVC_LOG_BDEV, "## EMMC LoadNvmRom Done", 0U, 0U);
                }
            }
        }
    }
    (void)AmbaGPIO_SetFuncAlt(GPIO_PIN_SD0_RST);
    
#elif defined(CONFIG_ENABLE_SPINOR_BOOT)
    {
        extern AMBA_NORSPI_DEV_INFO_s   AmbaNORSPI_DevInfo;
        extern AMBA_PARTITION_CONFIG_s  AmbaNORSPI_SysPartConfig[AMBA_NUM_SYS_PARTITION];
        extern AMBA_PARTITION_CONFIG_s  AmbaNORSPI_UserPartConfig[AMBA_NUM_USER_PARTITION];
        extern AMBA_SERIAL_SPI_CONFIG_s AmbaNOR_SPISetting;
        AMBA_NOR_SPI_CONFIG_s NorSpiConfig;
        UINT32                Err;

        Err = AmbaWrap_memset(&NorSpiConfig, 0, sizeof(AMBA_NOR_SPI_CONFIG_s));
        if (Err != SVC_OK) {
            SvcLog_NG(SVC_LOG_BDEV, "AmbaWrap_memset failed %u", Err, 0U);
        }
        NorSpiConfig.pNorSpiDevInfo  = &AmbaNORSPI_DevInfo;
        NorSpiConfig.pSysPartConfig  = AmbaNORSPI_SysPartConfig;
        NorSpiConfig.pUserPartConfig = AmbaNORSPI_UserPartConfig;
        NorSpiConfig.SpiSetting      = &AmbaNOR_SPISetting;

        #if defined(CONFIG_SOC_CV2FS)
        Err = AmbaGPIO_SetFuncAlt(SVCAQ_NOR_SPI_CLK);AmbaMisra_TouchUnused(&Err);
        Err = AmbaGPIO_SetFuncAlt(SVCAQ_NOR_SPI_DATA0);AmbaMisra_TouchUnused(&Err);
        Err = AmbaGPIO_SetFuncAlt(SVCAQ_NOR_SPI_DATA1);AmbaMisra_TouchUnused(&Err);
        Err = AmbaGPIO_SetFuncAlt(SVCAQ_NOR_SPI_DATA2);AmbaMisra_TouchUnused(&Err);
        Err = AmbaGPIO_SetFuncAlt(SVCAQ_NOR_SPI_DATA3);AmbaMisra_TouchUnused(&Err);
        Err = AmbaGPIO_SetFuncAlt(SVCAQ_NOR_SPI_DATA4);AmbaMisra_TouchUnused(&Err);
        Err = AmbaGPIO_SetFuncAlt(SVCAQ_NOR_SPI_DATA5);AmbaMisra_TouchUnused(&Err);
        Err = AmbaGPIO_SetFuncAlt(SVCAQ_NOR_SPI_DATA6);AmbaMisra_TouchUnused(&Err);
        Err = AmbaGPIO_SetFuncAlt(SVCAQ_NOR_SPI_DATA7);AmbaMisra_TouchUnused(&Err);
        Err = AmbaGPIO_SetFuncAlt(SVCAQ_NOR_SPI_CS);AmbaMisra_TouchUnused(&Err);
        Err = AmbaGPIO_SetFuncAlt(SVCAQ_NOR_SPI_DQS);AmbaMisra_TouchUnused(&Err);
        #endif

        #if defined(CONFIG_SPINOR_DMA_ENABLE)
        {
            UINT32 DmaTxChan = 0, DmaRxChan = 0;
            /* Set DMA channel to SpiNOR RX and TX */
            if (AmbaDMA_ChannelAllocate(AMBA_DMA_CHANNEL_NOR_SPI_TX, &DmaTxChan) != OK) {
                SvcLog_NG(SVC_LOG_BDEV, "## fail to allocate spi tx dma", 0U, 0U);
            }

            if (AmbaDMA_ChannelAllocate(AMBA_DMA_CHANNEL_NOR_SPI_RX, &DmaRxChan) != OK) {
                SvcLog_NG(SVC_LOG_BDEV, "## fail to allocate spi rx dma", 0U, 0U);
            }
        }
        #endif

        Rval = AmbaSpiNOR_Config(&NorSpiConfig);
        if (Rval != SVC_OK) {
            SvcLog_NG(SVC_LOG_BDEV, "## SpiNor_Config Fail 0x%X", Rval, 0U);
        } else {
            SvcLog_OK(SVC_LOG_BDEV, "## SpiNor_Config Done", 0U, 0U);
        }
    }

#if defined(CONFIG_MUTI_BOOT_DEVICE)

    if (pfnEMMCBoot != NULL) {
        Rval = pfnEMMCBoot();
        if (Rval != SVC_OK) {
            SvcLog_NG(SVC_LOG_BDEV, "## pfnEMMCBoot fail, return 0x%x", Rval, 0U);
        } else {
            SvcLog_OK(SVC_LOG_BDEV, "## pfnEMMCBoot Done", 0U, 0U);
            // Rval = AmbaEMMC_InitPtbBbt(1000);
            // if (Rval != SVC_OK) {
            //     SvcLog_NG(SVC_LOG_BDEV, "## EMMC InitPtbBbt fail, return 0x%x", Rval, 0U);
            // } else {
            //     SvcLog_OK(SVC_LOG_BDEV, "## EMMC InitPtbBbt Done", 0U, 0U);
            //     Rval = AmbaEMMC_LoadNvmRomFileTable();
            //     if (Rval != SVC_OK) {
            //         SvcLog_NG(SVC_LOG_BDEV, "## EMMC LoadNvmRom fail, return 0x%x", Rval, 0U);
            //     } else {
            //         SvcLog_OK(SVC_LOG_BDEV, "## EMMC LoadNvmRom Done", 0U, 0U);
            //     }
            // }


                Rval = AmbaEMMC_LoadNvmRomFileTable();
                if (Rval != SVC_OK) {
                    SvcLog_NG(SVC_LOG_BDEV, "## EMMC LoadNvmRom fail, return 0x%x", Rval, 0U);
                } else {
                    SvcLog_OK(SVC_LOG_BDEV, "## EMMC LoadNvmRom Done", 0U, 0U);
                }
            
        }
    // } else {
    //     Rval = AmbaSD_CardInit(AMBA_SD_CHANNEL0);
    //     if (Rval != SVC_OK) {
    //         SvcLog_NG(SVC_LOG_BDEV, "## EMMC init fail, return 0x%x", Rval, 0U);
    //     } else {
    //         SvcLog_OK(SVC_LOG_BDEV, "## EMMC init Done", 0U, 0U);

    //     }
        (void)AmbaGPIO_SetFuncAlt(GPIO_PIN_SD0_RST);
    }
#endif

#elif defined(CONFIG_ENABLE_SPINAND_BOOT)
    {
        #if defined(SVCAQ_NAND_SPI_CLK)
        extern const AMBA_SPINAND_DEV_INFO_s AmbaSpiNAND_DevInfo;
               const AMBA_SPINAND_DEV_INFO_s* pDevInfo = &AmbaSpiNAND_DevInfo;
        extern       AMBA_PARTITION_CONFIG_s AmbaNAND_SysPartConfig[AMBA_NUM_SYS_PARTITION];
        extern       AMBA_PARTITION_CONFIG_s AmbaNAND_UserPartConfig[AMBA_NUM_USER_PARTITION];

        UINT32                Err;
        AMBA_SPINAND_CONFIG_s SpiNandConfig = {
            .pNandDevInfo             = NULL,
            .pSysPartConfig           = AmbaNAND_SysPartConfig,
            .pUserPartConfig          = AmbaNAND_UserPartConfig,
            #if defined(CONFIG_AMBALINK_BOOT_OS)
            .AmbaNandPreAccessNotify  = AmbaIPC_NandLock,
            .AmbaNandPostAccessNotify = AmbaIPC_NandUnlock,
            #else
            .AmbaNandPreAccessNotify  = NULL,
            .AmbaNandPostAccessNotify = NULL
            #endif
        };
        AmbaMisra_TypeCast(&SpiNandConfig.pNandDevInfo, &pDevInfo);

        Err = AmbaGPIO_SetFuncAlt(SVCAQ_NAND_SPI_CLK);AmbaMisra_TouchUnused(&Err);
        Err = AmbaGPIO_SetFuncAlt(SVCAQ_NAND_SPI_DATA0);AmbaMisra_TouchUnused(&Err);
        Err = AmbaGPIO_SetFuncAlt(SVCAQ_NAND_SPI_DATA1);AmbaMisra_TouchUnused(&Err);
        Err = AmbaGPIO_SetFuncAlt(SVCAQ_NAND_SPI_DATA2);AmbaMisra_TouchUnused(&Err);
        Err = AmbaGPIO_SetFuncAlt(SVCAQ_NAND_SPI_DATA3);AmbaMisra_TouchUnused(&Err);
        Err = AmbaGPIO_SetFuncAlt(SVCAQ_NAND_SPI_CS);AmbaMisra_TouchUnused(&Err);

        Rval = AmbaSpiNAND_Config(&SpiNandConfig);
        if (Rval != SVC_OK) {
            SvcLog_NG(SVC_LOG_BDEV, "## AmbaSpiNAND_Config Fail 0x%X", Rval, 0U);
        } else {
            SvcLog_OK(SVC_LOG_BDEV, "## AmbaSpiNAND_Config init Done", 0U, 0U);
            if (NULL != SpiNandConfig.AmbaNandPreAccessNotify) {
                SvcLog_OK(SVC_LOG_BDEV, "## Hook AmbaNandPreAccessNotify", 0U, 0U);
            }
        }
        #else
        SvcLog_NG(SVC_LOG_BDEV, "## SPI-NAND pins aren't defined", 0U, 0U);
        #endif
    }
#else
    #pragma message ("[SvcBootDev.c] ERROR !! UNKNOWN BOOT DEVICE !!")
    SvcLog_NG(SVC_LOG_BDEV, "## No Boot Device!", 0, 0U);
#endif
    if (Rval == SVC_OK) {
        UINT32 UserPtbNo;
        Rval = SvcFwUpdate_GetActivePtb(&UserPtbNo);
        if (Rval == SVC_OK) {
            SvcLog_OK(SVC_LOG_BDEV, " Boot from PTB %u", UserPtbNo, 0U);
        }
    }

    AmbaMisra_TouchUnused(&pfnEMMCBoot);
    return Rval;
}
