/*
 * Copyright (c) 2020 Ambarella International LP
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
 */

#include "AmbaTypes.h"
#include "AmbaKAL.h"

#include "AmbaWrap.h"
#include "AmbaDMA.h"
#include "AmbaNAND_Ctrl.h"
#include "AmbaNAND_OP.h"
#include "AmbaMisraFix.h"
#include "AmbaSPINAND.h"

#include "AmbaCache.h"
#include "AmbaRTSL_FIO.h"
#include "AmbaRTSL_NAND_Ctrl.h"
#include "AmbaRTSL_SPINAND.h"
#include "AmbaCSL_SPINAND.h"
#include "AmbaCSL_FIO.h"
#include "AmbaCSL_NAND.h"

#include "AmbaINT.h"
#include "AmbaCortexA53.h"

#ifdef CONFIG_QNX
#include "hw/ambarella_fio.h"

UINT32 AmbaRTSL_PllGetCortex1Clk(void)
{
    return 792000000U;
}
UINT32 AmbaRTSL_PllGetNandClk(void)
{
    return 528000000U;
}
#endif

/*
 * Definitions for EventFlag
 */
#define AMBA_FIO_SPINAND_CMD_DONE_FLAG        0x1U

typedef struct {
    AMBA_KAL_EVENT_FLAG_t   EventFlag;  /* Event Flags */
    AMBA_KAL_MUTEX_t        Mutex;      /* Mutex */
} AMBA_SPINAND_CTRL_s;

static AMBA_SPINAND_CTRL_s _AmbaSpiNAND_Ctrl;    /* NAND Management Structure */
static AMBA_KAL_MUTEX_t PseudoCB_Mutex;

#ifndef CONFIG_QNX
static void NAND_FioNandCmdIsr(void);
#endif
static void AmbaSpiNAND_HookApi(const AMBA_SPINAND_CONFIG_s *pNandConfig);
static UINT32 AmbaSPINAND_Wait(UINT32 TimeOut);

AMBA_NAND_COMMON_INFO_s *AmbaNAND_GetCommonInfo(void)
{
    return AmbaRTSL_NandCommonInfo;
}

/**
 *  AmbaSpiNAND_Lock - NAND Lock
 *  @return error code
 */
static UINT32 AmbaSpiNAND_Lock(void)
{
    if(AmbaNandOp_PreAccessNotify != NULL) {
        AmbaNandOp_PreAccessNotify();
    }
    /*
     * Take the Mutex
     */
    return AmbaKAL_MutexTake(&_AmbaSpiNAND_Ctrl.Mutex, KAL_WAIT_FOREVER);
}

/**
 *  AmbaSpiNAND_Unlock - Unlock NAND
 *  @return error code
 */
static UINT32 AmbaSpiNAND_Unlock(void)
{
    if(AmbaNandOp_PostAccessNotify != NULL) {
        AmbaNandOp_PostAccessNotify();
    }
    /*
     * Release the Mutex
     */

    return AmbaKAL_MutexGive(&_AmbaSpiNAND_Ctrl.Mutex);
}

/**
 *  AmbaSpiNAND_Init - Initialize NAND data structure
 *  @return error code
 */
static UINT32 AmbaSpiNAND_Init(void)
{
    static char AmbaPseudoCBMutexName[16] = "PseudoCB_Mutex";

    /* Create Event Flag */
    (void)AmbaKAL_EventFlagCreate(&_AmbaSpiNAND_Ctrl.EventFlag, NULL);


    /* Create Mutex */
    (void)AmbaKAL_MutexCreate(&_AmbaSpiNAND_Ctrl.Mutex, NULL);
    (void)AmbaKAL_MutexCreate(&PseudoCB_Mutex, AmbaPseudoCBMutexName);

    if (AmbaCSL_FioIsRandomReadMode() != 0U) {
        AmbaRTSL_FioReset();
        AmbaCSL_FioDmaFifoModeEnable(); /* Enable DMA Mode for FIO-DMA FIFO */
    }

    AmbaRTSL_FioInit();             /* Initialize the FIO controller */
#ifndef CONFIG_QNX
    _AmbaRTSL_FioNandCmdIsrCallBack = NAND_FioNandCmdIsr; /* Call back function when NAND Command done */
#endif
    return 0;
}

/**
 *  AmbaSpiNAND_GetFeature - send command 0xf to the SPI Nand status register
 *  @param[in] FeatureAddr Indicate the feature address
 *  @param[out] pStatus ID byte sequence (at most five bytes)
 *  @param[in] TimeOut The timeout value in system tick (ms)
 *  @return error code
 */
static UINT32 AmbaSpiNAND_GetFeature(UINT8 FeatureAddr, UINT8 *pStatus, UINT32 TimeOut)
{
    UINT32 RetVal;

    if (AmbaSpiNAND_Lock() != OK) {
        RetVal = SPINAND_ERR_OS_API_FAIL;
    } else {
        AmbaCSL_SpiNandSendGetFeature(FeatureAddr);
        RetVal = AmbaSPINAND_Wait(TimeOut);
        if (RetVal == OK) {
            AmbaCSL_SpiNandGetCustomCmdData(1U, pStatus);
        }

        /*
         * Release the Mutex
         */
        (void)AmbaSpiNAND_Unlock();
    }
    return RetVal;
}

/**
 *  AmbaSpiNAND_SetFeature -
 *  @param[in] FeatureAddr Indicate the feature address
 *  @param[in] Value
 *  @param[in] TimeOut The timeout value in system tick (ms)
 *  @return error code
 */
static UINT32 AmbaSpiNAND_SetFeature(UINT8 FeatureAddr, UINT8 Value, UINT32 TimeOut)
{
    UINT32 RetVal;

    if (AmbaSpiNAND_Lock() != OK) {
        RetVal = SPINAND_ERR_OS_API_FAIL;
    } else {
        AmbaRTSL_SpiNandWriteEnableCmd();
        RetVal = AmbaSPINAND_Wait(TimeOut);

        if (RetVal == OK) {
            AmbaCSL_SpiNandSendSetFeature(FeatureAddr, Value);
            RetVal = AmbaSPINAND_Wait(TimeOut);
        }

        /*
         * Release the Mutex
         */
        (void)AmbaSpiNAND_Unlock();
    }
    return RetVal;
}

/**
 *  AmbaSpiNAND_Reset - Reset NAND data structure
 *  @param[in] TimeOut The timeout value in system tick (ms)
 *  @return error code
 */
static UINT32 AmbaSpiNAND_Reset(UINT32 TimeOut)
{
    UINT32 RetVal;
    AMBA_SPINAND_CMDSTATUS_REG_s Status;

    (void)AmbaWrap_memset(&Status, 0, sizeof(AMBA_SPINAND_CMDSTATUS_REG_s));

    if (AmbaSpiNAND_Lock() != OK) {
        RetVal = SPINAND_ERR_OS_API_FAIL;
    } else {
        (void)AmbaKAL_EventFlagClear(&_AmbaSpiNAND_Ctrl.EventFlag, AMBA_FIO_SPINAND_CMD_DONE_FLAG);

        AmbaRTSL_SpiNandSendResetCmd();

        RetVal = AmbaSPINAND_Wait(TimeOut);

        /*
         * Release the Mutex
         */
        (void)AmbaSpiNAND_Unlock();

        if (RetVal == OK) {
            do {
                const AMBA_SPINAND_CMDSTATUS_REG_s *pStatus = &Status;
                UINT8 *pTmp = NULL;
                AmbaMisra_TypeCast(&pTmp, &pStatus);

                RetVal = AmbaSpiNAND_GetFeature(0xc0U, pTmp, 5000U);
            } while((Status.OIP == 1U) && (RetVal == OK));
        }
    }
    return RetVal;
}

/**
 *  AmbaSpiNAND_ConfigOnDevECC -
 *  @param[in] TimeOut The timeout value in system tick (ms)
 *  @return error code
 */
static UINT32 AmbaSpiNAND_ConfigOnDevECC(UINT32 TimeOut)
{
    const AMBA_SPINAND_DEV_INFO_s *pNandDevInfo = AmbaRTSL_SpiNandDevInfo;
    UINT8 Status = 0U;
    UINT32 RetVal;

    RetVal = AmbaSpiNAND_Reset(TimeOut);

    if (RetVal == OK) {
        /* Unlock BP0~2 From Device */
        RetVal = AmbaSpiNAND_GetFeature(0xa0U, &Status, TimeOut);
        if ((Status != 0U) && (RetVal == OK)) {
            (void)AmbaSpiNAND_SetFeature(0xA0U, 0, TimeOut);
        }

        /* Disable the ECC Feature on Device */
        if (pNandDevInfo->InternalECC == AMBA_SPINAND_DISABLE_ONDEVICE_ECC) {
            RetVal = AmbaSpiNAND_GetFeature(0xb0U, &Status, TimeOut);
            if (((Status & 0x10U) != 0U) && (RetVal == OK)) {
                Status &= 0xefU;
                RetVal = AmbaSpiNAND_SetFeature(0xB0U, Status, TimeOut);
            }
        }
    }
    return RetVal;
}

#if 0
/**
 *  AmbaSpiNAND_ReadID - Read device ID
 *  @param[in] NumReadCycle Indicate number of cycles
 *  @param[out] pDeviceID pointer to the Device ID
 *  @param[in] TimeOut The timeout value in system tick (ms)
 *  @return error code
 */
UINT32 AmbaSpiNAND_ReadID(UINT32 NumReadCycle, UINT8 *pDeviceID, UINT32 TimeOut)
{
    UINT32 RetVal;

    /*
     * Take the Mutex
     */
    if (AmbaSpiNAND_Lock() != OK) {
        RetVal = SPINAND_ERR_OS_API_FAIL;
    } else {
        AmbaRTSL_NandSendReadIdCmd(NumReadCycle);

        /* wait for Command Done: Event Flag ! */
        RetVal = AmbaSPINAND_Wait(TimeOut);
        if (RetVal == OK) {
            AmbaRTSL_NandGetReadIdResponse(NumReadCycle, pDeviceID);
        }

        /*
         * Release the Mutex
         */
        (void)AmbaSpiNAND_Unlock();
    }
    return RetVal;
}
#endif

#ifdef CONFIG_QNX
static UINT8 *PseudoBufMain;
static UINT8 *PseudoBufSpare;

#if defined (CONFIG_SOC_CV2FS)|| defined (CONFIG_SOC_CV22FS)
#define AMBA_CORTEX_A53_FLASH_CPU_BASE_ADDR     AMBA_CA53_FLASH_CPU_BASE_ADDR
#endif

void AmbaSpiNAND_DeviceInit(void)
{
    ULONG Vbase = mmap_device_io(sizeof(AMBA_FIO_REG_s), AMBA_CORTEX_A53_FLASH_CPU_BASE_ADDR);
    extern AMBA_FIO_REG_s *pAmbaFIO_Reg;
    pAmbaFIO_Reg = (AMBA_FIO_REG_s *) Vbase;

    ULONG Addr;
    extern int get_fio_work_buf(ULONG *pAddr, UINT32 Size);

    get_fio_work_buf(&Addr, 64U * 4U * 1024U);
    AmbaMisra_TypeCast(&PseudoBufMain, &Addr);

    get_fio_work_buf(&Addr, 64U * 256U);
    AmbaMisra_TypeCast(&PseudoBufSpare, &Addr);

}
#endif

/**
 *  AmbaSpiNAND_Config - NAND software configurations
 *  @param[in] pNandConfig pointer to NAND software configurations
 *  @return error code
 */
UINT32 AmbaSpiNAND_Config(AMBA_SPINAND_CONFIG_s *pNandConfig)
{
    UINT32 RetVal = SPINAND_ERR_NONE;

    AmbaSpiNAND_DeviceInit();

    (void)AmbaSpiNAND_Init();

    AmbaSpiNAND_HookApi(pNandConfig);

    RetVal = AmbaRTSL_SpiNandConfig(pNandConfig);

    if (RetVal == SPINAND_ERR_NONE) {
        RetVal = AmbaSpiNAND_ConfigOnDevECC(1000U);
    }

    if (RetVal == SPINAND_ERR_NONE) {
        /* Init BBT and System/User Partition Tables */
        RetVal = AmbaNAND_InitPtbBbt(1000U);
    }

    if (RetVal == SPINAND_ERR_NONE) {
        RetVal = AmbaNAND_LoadNvmRomFileTable();   /* Load NAND ROM File Table */
    }

    return RetVal;
}

void spinand_init(void)
{
    extern AMBA_SPINAND_DEV_INFO_s AmbaSpiNAND_DevInfo;
    extern AMBA_PARTITION_CONFIG_s AmbaNAND_SysPartConfig[AMBA_NUM_SYS_PARTITION];
    extern AMBA_PARTITION_CONFIG_s AmbaNAND_UserPartConfig[AMBA_NUM_USER_PARTITION];

    AMBA_SPINAND_CONFIG_s SpiNandConfig = {
        .pNandDevInfo    =  &AmbaSpiNAND_DevInfo,
        .pSysPartConfig  =  &(AmbaNAND_SysPartConfig[0]),   /* pointer to System partition configurations */
        .pUserPartConfig =  &(AmbaNAND_UserPartConfig[0]),  /* pointer to partition configurations */
    };

    /* NAND software configurations */
    if (AmbaSpiNAND_Config(&SpiNandConfig) != OK) {
        printf("NAND_Config Fail ");
    }
}

#if 0
/**
 *  AmbaSpiNAND_GetDevInfo - get the pointer to current NAND device information
 *  @param[in] pNandConfig pointer to NAND software configurations
 *  @return AMBA_SpiNAND_DEV_INFO_s *  the pointer to current NAND device information
 */
AMBA_SPINAND_DEV_INFO_s *AmbaSpiNAND_GetDevInfo(void)
{
    return AmbaRTSL_SpiNandDevInfo;
}
#endif

/**
 *  AmbaSpiNAND_Read - Read data from NAND flash
 *  @param[in] PageAddr The first page address to read
 *  @param[in] NumPage Number of pages to read
 *  @param[out] pMainBuf  pointer to main area data
 *  @param[out] pSpareBuf pointer to spare area data
 *  @param[in] TimeOut The timeout value in system tick (ms)
 *  @return error code
 */
static UINT32 AmbaSpiNAND_Read(UINT32 PageAddr, UINT32 NumPage, UINT8 *pMainBuf, UINT8 *pSpareBuf, UINT32 TimeOut)
{
    UINT32 RetVal;
    const AMBA_RTSL_FDMA_CTRL_s *pFdmaCtrl = (AMBA_RTSL_FDMA_CTRL_s *) &AmbaRTSL_FioCtrl.FdmaCtrl;

    if ((NumPage == 0U) || ((pMainBuf == NULL) && (pSpareBuf == NULL))) {
        RetVal = SPINAND_ERR_ARG;  /* wrong parameter */
    } else {

        /*
         * Take the Mutex
         */
        if (AmbaSpiNAND_Lock() != OK) {
            RetVal = SPINAND_ERR_OS_API_FAIL;
        } else {

            (void)AmbaKAL_EventFlagClear(&_AmbaSpiNAND_Ctrl.EventFlag, AMBA_FIO_SPINAND_CMD_DONE_FLAG);
            RetVal =  AmbaRTSL_SpiNandReadStart(PageAddr, NumPage, pMainBuf, pSpareBuf);


            if (RetVal == SPINAND_ERR_NONE) {

                RetVal = AmbaSPINAND_Wait(TimeOut);

                if (RetVal == OK) {
                    (void)AmbaCache_DataInvalidate((ULONG)pFdmaCtrl->pMainBuf, pFdmaCtrl->MainByteCount);
                    (void)AmbaCache_DataInvalidate((ULONG)pFdmaCtrl->pSpareBuf, pFdmaCtrl->SpareByteCount);
                    RetVal = AmbaRTSL_SpiNandCheckDeviceStatus(NumPage, (UINT8)AMBA_SPINAND_ERROR_UNCORRECTABLE_ECC);
                }

                if (RetVal == OK) {
                    /* Copy back through DMA if output buf is not 8-Byte aligned. */
                    if ((pMainBuf != NULL) && (pMainBuf != pFdmaCtrl->pMainBuf)) {
                        (void)AmbaWrap_memcpy(pMainBuf, pFdmaCtrl->pMainBuf, pFdmaCtrl->MainByteCount);
                        //AmbaRTSL_CacheCleanDataCacheRange(pMainBuf, pFdmaCtrl->MainByteCount);
                    }

                    if ((pSpareBuf != NULL) && (pSpareBuf != pFdmaCtrl->pSpareBuf)) {
                        (void)AmbaWrap_memcpy(pSpareBuf, pFdmaCtrl->pSpareBuf, pFdmaCtrl->SpareByteCount);
                        //AmbaRTSL_CacheCleanDataCacheRange(pSpareBuf, pFdmaCtrl->SpareByteCount);
                    }
                } else {
                    (void)AmbaSpiNAND_Reset(5000U);
                }

                /*
                 * Release the Mutex
                 */
                (void)AmbaSpiNAND_Unlock();
            }
        }
    }
    return RetVal;
}

/**
 *  AmbaSpiNAND_Program - Perform NAND write data cmd flow setup
 *  @param[in] PageAddr The first page address to read
 *  @param[in] NumPage Number of pages to read
 *  @param[out] pMainBuf  pointer to main area data
 *  @param[out] pSpareBuf pointer to spare area data
 *  @param[in] TimeOut The timeout value in system tick (ms)
 *  @return error code
 */
static UINT32 AmbaSpiNAND_Program(UINT32 PageAddr, UINT32 NumPage, UINT8 *pMainBuf, UINT8 *pSpareBuf, UINT32 TimeOut)
{
    UINT32 RetVal = SPINAND_ERR_NONE;

    if ((NumPage == 0U) || ((pMainBuf == NULL) && (pSpareBuf == NULL))) {
        RetVal = SPINAND_ERR_ARG;  /* wrong parameter */
    } else {

        /*
         * Take the Mutex
         */
        if (AmbaSpiNAND_Lock() != OK) {
            RetVal = SPINAND_ERR_OS_API_FAIL;
        } else {

            (void)AmbaKAL_EventFlagClear(&_AmbaSpiNAND_Ctrl.EventFlag, AMBA_FIO_SPINAND_CMD_DONE_FLAG);
            if (0U == AmbaRTSL_SpiNandProgramStart(PageAddr, NumPage, pMainBuf, pSpareBuf)) {
                RetVal = AmbaSPINAND_Wait(TimeOut);
            }
            if (RetVal == OK) {
                RetVal = AmbaRTSL_SpiNandCheckDeviceStatus(NumPage, AMBA_SPINAND_ERROR_PROGRAM_FAIL);
            }

            /*
             * Release the Mutex
             */
            (void)AmbaSpiNAND_Unlock();
        }
    }
    return RetVal;
}

/**
 *  AmbaSpiNAND_CopyBack - Copy data from source page to destinaiton page
 *  @param[in] DestPageAddr Destination page address
 *  @param[in] SrcPageAddr Source page address
 *  @param[in] TimeOut The timeout value in system tick (ms)
 *  @return error code
 */
static UINT32 AmbaSpiNAND_CopyBack(UINT32 DestPageAddr, UINT32 SrcPageAddr, UINT32 TimeOut)
{
    /* Work Buffer for 1 Block.  */
#if 0
    static UINT8 PseudoBufMain[64U * 4U * 1024U]
    GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE;

    static UINT8 PseudoBufSpare[64U * 256U]
    GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE;
#endif

    UINT32 RetVal = OK;
    UINT8 *pMainBuf, *pSpareBuf;

    pMainBuf  = PseudoBufMain;
    pSpareBuf = PseudoBufSpare;

    if (AmbaKAL_MutexTake(&PseudoCB_Mutex, 0xFFFFFFFFU) != KAL_ERR_NONE) {
        RetVal = NAND_ERR_OS_API_FAIL;
    } else {
        RetVal = AmbaSpiNAND_Read(SrcPageAddr, 1U, pMainBuf, pSpareBuf, TimeOut);
        if (RetVal != OK) {
            RetVal = SPINAND_ERR_IO_FAIL;
        } else {

            /* Program main and spare area */
            RetVal = AmbaSpiNAND_Program(DestPageAddr, 1U, pMainBuf, pSpareBuf, TimeOut);
            if (RetVal != OK) {
                RetVal = SPINAND_ERR_IO_FAIL;
            }
        }
        (void)AmbaKAL_MutexGive(&PseudoCB_Mutex);
    }
    return RetVal;
}

/**
 *  AmbaSpiNAND_EraseBlock - To erase the block of data identified by the block address parameter
 *  @param[in] BlkAddr Block address
 *  @param[in] TimeOut The timeout value in system tick (ms)
 *  @return error code
 */
static UINT32 AmbaSpiNAND_EraseBlock(UINT32 BlkAddr, UINT32 TimeOut)
{
    UINT32 RetVal;
    const AMBA_SPINAND_DEV_INFO_s *pNandDevInfo = AmbaRTSL_SpiNandDevInfo;

    /*
     * Take the Mutex
     */
    if (AmbaSpiNAND_Lock() != OK) {
        RetVal = SPINAND_ERR_OS_API_FAIL;
    }

    (void)AmbaKAL_EventFlagClear(&_AmbaSpiNAND_Ctrl.EventFlag, AMBA_FIO_SPINAND_CMD_DONE_FLAG);
    AmbaRTSL_SpiNandEraseBlockStart(BlkAddr);

    RetVal = AmbaSPINAND_Wait(TimeOut);
    if (RetVal == OK) {
        RetVal = AmbaRTSL_SpiNandCheckDeviceStatus(pNandDevInfo->BlockPageSize, AMBA_SPINAND_ERROR_ERASE_FAIL);
    }

    /*
     * Release the Mutex
     */
    (void)AmbaSpiNAND_Unlock();

    return RetVal;
}

/**
 *  NAND_FioNandCmdIsr - NAND command done ISR
 */
#ifndef CONFIG_QNX
static void NAND_FioNandCmdIsr(void)
{
    (void)AmbaKAL_EventFlagSet(&_AmbaSpiNAND_Ctrl.EventFlag, AMBA_FIO_SPINAND_CMD_DONE_FLAG);
}
#endif

/**
 *  AmbaSPINAND_Wait - NAND wait done ISR
 */
static UINT32 AmbaSPINAND_Wait(UINT32 TimeOut)
{
    UINT32 RetVal;
#ifndef CONFIG_QNX
    UINT32 ActualFlags = 0;
    RetVal = AmbaKAL_EventFlagGet(&_AmbaSpiNAND_Ctrl.EventFlag, AMBA_FIO_SPINAND_CMD_DONE_FLAG,
                                  0x1U, 0x1U, &ActualFlags, TimeOut);
#else
    int fd;
    amba_fio_config_t Config;
    (void) TimeOut;
    Config.TimeOut = 5000U;
    Config.Status  = 0;

    fd = open("/dev/fio", O_RDWR);
    devctl(fd, DCMD_FIO_WAIT, &Config, sizeof(amba_fio_config_t), NULL);
    close(fd);

    RetVal = OK;
#endif
    return RetVal;
}

/**
 *  AmbaSpiNAND_HookApi -
 */
static void AmbaSpiNAND_HookApi(const AMBA_SPINAND_CONFIG_s *pNandConfig)
{
    AmbaNandOp_Copyback   = AmbaSpiNAND_CopyBack;
    AmbaNandOp_EraseBlock = AmbaSpiNAND_EraseBlock;
    AmbaNandOp_Read       = AmbaSpiNAND_Read;
    AmbaNandOp_Program    = AmbaSpiNAND_Program;

    AmbaNandOp_PreAccessNotify  = pNandConfig->AmbaNandPreAccessNotify;
    AmbaNandOp_PostAccessNotify = pNandConfig->AmbaNandPostAccessNotify;
}

