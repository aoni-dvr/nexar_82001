/**
 *  @file AmbaSPINAND.c
 *
 *  @copyright Copyright (c) 2019 Ambarella, Inc.
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
 *  @details NAND Control APIs
 *
 */

#include "AmbaTypes.h"
#include "AmbaKAL.h"

#include "AmbaWrap.h"
#include "AmbaDMA.h"
#include "AmbaNAND_Ctrl.h"
#include "AmbaNAND_OP.h"
#include "AmbaMisraFix.h"
#include "AmbaSPINAND.h"

/*#include "AmbaRTSL_Cache.h"
#include "AmbaRTSL_FIO.h"
#include "AmbaRTSL_NAND_Ctrl.h"
#include "AmbaRTSL_SPINAND.h"
#include "AmbaCSL_SPINAND.h"
#include "AmbaCSL_FIO.h"
#include "AmbaCSL_NAND.h"*/

#include "AmbaINT.h"

/*
 * Definitions for EventFlag
 */
#define AMBA_FIO_SPINAND_CMD_DONE_FLAG        0x1U

typedef struct {
    AMBA_KAL_EVENT_FLAG_t   EventFlag;  /* Event Flags */
    AMBA_KAL_MUTEX_t        Mutex;      /* Mutex */
} AMBA_SPINAND_CTRL_s;

static AMBA_SPINAND_CTRL_s _AmbaSpiNAND_Ctrl;    /* NAND Management Structure */

static void AmbaSpiNAND_HookApi(void);

/**
 *  AmbaSpiNAND_Lock - NAND Lock
 *  @return error code
 */
static UINT32 AmbaSpiNAND_Lock(void)
{
#ifdef CONFIG_ENABLE_AMBALINK
    extern void AmbaIPC_NandLock(void);
    AmbaIPC_NandLock();
#endif
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
#ifdef CONFIG_ENABLE_AMBALINK
    extern void AmbaIPC_NandUnlock(void);
    AmbaIPC_NandUnlock();
#endif
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
    /* Create Event Flag */
    (void)AmbaKAL_EventFlagCreate(&_AmbaSpiNAND_Ctrl.EventFlag, NULL);


    /* Create Mutex */
    (void)AmbaKAL_MutexCreate(&_AmbaSpiNAND_Ctrl.Mutex, NULL);

    //if (AmbaRTSL_FioIsRandomReadMode() != 0U) {
    //    AmbaRTSL_FioReset();
    //    AmbaCSL_FioDmaFifoModeEnable(); /* Enable DMA Mode for FIO-DMA FIFO */
    //}

    //AmbaRTSL_FioInit();             /* Initialize the FIO controller */

    //_AmbaRTSL_FioNandCmdIsrCallBack = NAND_FioNandCmdIsr; /* Call back function when NAND Command done */

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
    UINT32 ActualFlags = 0;
    (void) FeatureAddr;
    (void) pStatus;

    if (AmbaSpiNAND_Lock() != OK) {
        RetVal = SPINAND_ERR_OS_API_FAIL;
    } else {

        //AmbaCSL_SpiNandSendGetFeature(FeatureAddr);
        RetVal = AmbaKAL_EventFlagGet(&_AmbaSpiNAND_Ctrl.EventFlag, AMBA_FIO_SPINAND_CMD_DONE_FLAG,
                                      0x1U, 0x1U, &ActualFlags, TimeOut);
        if (RetVal == OK) {
            //AmbaCSL_SpiNandGetCustomCmdData(1U, pStatus);
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
    UINT32 ActualFlags = 0;
    (void) FeatureAddr;
    (void) Value;

    if (AmbaSpiNAND_Lock() != OK) {
        RetVal = SPINAND_ERR_OS_API_FAIL;
    } else {
        //AmbaRTSL_SpiNandWriteEnableCmd();
        RetVal = AmbaKAL_EventFlagGet(&_AmbaSpiNAND_Ctrl.EventFlag, AMBA_FIO_SPINAND_CMD_DONE_FLAG,
                                      1U, 1U, &ActualFlags, TimeOut);

        //if (RetVal == OK) {
        //AmbaCSL_SpiNandSendSetFeature(FeatureAddr, Value);
        RetVal = AmbaKAL_EventFlagGet(&_AmbaSpiNAND_Ctrl.EventFlag, AMBA_FIO_SPINAND_CMD_DONE_FLAG,
                                      1U, 1U, &ActualFlags, TimeOut);
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
    UINT32 RetVal = 1;
    (void) TimeOut;
    return RetVal;
}

/**
 *  AmbaSpiNAND_ConfigOnDevECC -
 *  @param[in] TimeOut The timeout value in system tick (ms)
 *  @return error code
 */
static UINT32 AmbaSpiNAND_ConfigOnDevECC(UINT32 TimeOut)
{
    const AMBA_SPINAND_DEV_INFO_s *pNandDevInfo = NULL;//AmbaRTSL_SpiNandDevInfo;
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
    UINT32 ActualFlags = 0;

    /*
     * Take the Mutex
     */
    if (AmbaSpiNAND_Lock() != OK) {
        RetVal = SPINAND_ERR_OS_API_FAIL;
    } else {
        AmbaRTSL_NandSendReadIdCmd(NumReadCycle);

        /* wait for Command Done: Event Flag ! */
        RetVal = AmbaKAL_EventFlagGet(&_AmbaSpiNAND_Ctrl.EventFlag, AMBA_FIO_SPINAND_CMD_DONE_FLAG,
                                      0x1U, 0x1U, &ActualFlags, TimeOut);
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

/**
 *  AmbaSpiNAND_Config - NAND software configurations
 *  @param[in] pNandConfig pointer to NAND software configurations
 *  @return error code
 */
UINT32 AmbaSpiNAND_Config(AMBA_SPINAND_CONFIG_s *pNandConfig)
{
    UINT32 RetVal = SPINAND_ERR_NONE;
    (void) pNandConfig;

    (void)AmbaSpiNAND_Init();

    AmbaSpiNAND_HookApi();

    RetVal = 0;//AmbaRTSL_SpiNandConfig(pNandConfig);

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
    UINT32 RetVal = 1;
    (void) PageAddr;
    (void) NumPage;
    (void) pMainBuf;
    (void) pSpareBuf;
    (void) TimeOut;
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
    UINT32 ActualFlags = 0;
    (void) PageAddr;
    (void) NumPage;
    (void) pMainBuf;
    (void) pSpareBuf;

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
            //if (0U == AmbaRTSL_SpiNandProgramStart(PageAddr, NumPage, pMainBuf, pSpareBuf)) {
            RetVal = AmbaKAL_EventFlagGet(&_AmbaSpiNAND_Ctrl.EventFlag, AMBA_FIO_SPINAND_CMD_DONE_FLAG,
                                          1U, 1U, &ActualFlags, TimeOut);
            //}
            if (RetVal == OK) {
                RetVal = 0;//AmbaRTSL_SpiNandCheckDeviceStatus(NumPage, AMBA_SPINAND_ERROR_PROGRAM_FAIL);
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
    static UINT8 PseudoBufMain[64U * 4U * 1024U]
    GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE;

    static UINT8 PseudoBufSpare[64U * 256U]
    GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE;

    UINT32 RetVal = OK;
    UINT8 *pMainBuf, *pSpareBuf;

    pMainBuf  = PseudoBufMain;
    pSpareBuf = PseudoBufSpare;

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
    UINT32 ActualFlags = 0;
    UINT32 RetVal;
    (void) BlkAddr;
    (void) TimeOut;

    /*
     * Take the Mutex
     */
    if (AmbaSpiNAND_Lock() != OK) {
        RetVal = SPINAND_ERR_OS_API_FAIL;
    }

    (void)AmbaKAL_EventFlagClear(&_AmbaSpiNAND_Ctrl.EventFlag, AMBA_FIO_SPINAND_CMD_DONE_FLAG);
    //AmbaRTSL_SpiNandEraseBlockStart(BlkAddr);

    RetVal = AmbaKAL_EventFlagGet(&_AmbaSpiNAND_Ctrl.EventFlag, AMBA_FIO_SPINAND_CMD_DONE_FLAG,
                                  0x1U, 0x1U, &ActualFlags, TimeOut);
    if (RetVal == OK) {
        //RetVal = AmbaRTSL_SpiNandCheckDeviceStatus(pNandDevInfo->BlockPageSize, AMBA_SPINAND_ERROR_ERASE_FAIL);
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

/**
 *  AmbaSpiNAND_HookApi -
 */
static void AmbaSpiNAND_HookApi(void)
{
    AmbaNandOp_Copyback   = AmbaSpiNAND_CopyBack;
    AmbaNandOp_EraseBlock = AmbaSpiNAND_EraseBlock;
    AmbaNandOp_Read       = AmbaSpiNAND_Read;
    AmbaNandOp_Program    = AmbaSpiNAND_Program;
}

