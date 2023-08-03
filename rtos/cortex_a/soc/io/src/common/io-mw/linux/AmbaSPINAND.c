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

#include <mtd/mtd-user.h>
#include "AmbaDef.h"
#include "AmbaRTSL_NAND_Ctrl.h"

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
#if 0
/**
 *  AmbaSpiNAND_GetFeature - send command 0xf to the SPI Nand status register
 *  @param[in] FeatureAddr Indicate the feature address
 *  @param[out] pStatus ID byte sequence (at most five bytes)
 *  @param[in] TimeOut The timeout value in system tick (ms)
 *  @return error code
 */
static UINT32 AmbaSpiNAND_GetFeature(UINT8 FeatureAddr, UINT8 *pStatus, UINT32 TimeOut)
{
    UINT32 RetVal = SPINAND_ERR_NONE;
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
    UINT32 RetVal = SPINAND_ERR_NONE;
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
    UINT32 RetVal = SPINAND_ERR_NONE;
    (void) TimeOut;
    return RetVal;
}
#endif
/**
 *  AmbaSpiNAND_ConfigOnDevECC -
 *  @param[in] TimeOut The timeout value in system tick (ms)
 *  @return error code
 */
static UINT32 AmbaSpiNAND_ConfigOnDevECC(UINT32 TimeOut)
{
    UINT32 RetVal = 0U;
    (void)TimeOut;
#if 0
    const AMBA_SPINAND_DEV_INFO_s *pNandDevInfo = NULL;//AmbaRTSL_SpiNandDevInfo;
    UINT8 Status = 0U;

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
#endif
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

static UINT32 AmbaRTSL_SpiNandConfig(AMBA_SPINAND_CONFIG_s *pNandConfig)
{
    AMBA_SPINAND_DEV_INFO_s *pNandDevInfo = NULL;
    AMBA_PARTITION_CONFIG_s *pUserPartConfig = NULL;
    UINT32 MainByteSize, SpareByteSize;
    UINT32 NumEccBit, k;
    UINT32 RetVal = SPINAND_ERR_NONE;
    UINT32 Tmp32;
    UINT8  Tmp8;

    AmbaMisra_TouchUnused(pNandConfig);
    AmbaMisra_TouchUnused(pNandDevInfo);

    if ((pNandConfig == NULL) || (pNandConfig->pNandDevInfo == NULL) || (pNandConfig->pUserPartConfig == NULL)) {
        RetVal = SPINAND_ERR_ARG;  /* wrong parameters */
    } else {


        pNandDevInfo    = pNandConfig->pNandDevInfo;
        pUserPartConfig = pNandConfig->pUserPartConfig;
        SpareByteSize   = pNandDevInfo->SpareByteSize;
        MainByteSize    = pNandDevInfo->MainByteSize;
        k               = MainByteSize / 512U;

        AmbaRTSL_NandCtrl.NandInfo.BlockPageSize  = pNandDevInfo->BlockPageSize;
        AmbaRTSL_NandCtrl.NandInfo.MainByteSize   = pNandDevInfo->MainByteSize;
        AmbaRTSL_NandCtrl.NandInfo.PlaneAddrMask  = 0;
        AmbaRTSL_NandCtrl.NandInfo.PlaneBlockSize = pNandDevInfo->PlaneBlockSize;
        AmbaRTSL_NandCtrl.NandInfo.SpareByteSize  = pNandDevInfo->SpareByteSize;
        AmbaRTSL_NandCtrl.NandInfo.TotalPlanes    = pNandDevInfo->TotalPlanes;
        AmbaRTSL_NandCtrl.NandInfo.TotalZones     = pNandDevInfo->TotalZones;
        AmbaRTSL_NandCtrl.NandInfo.ZoneBlockSize  = pNandDevInfo->ZoneBlockSize;

        AmbaRTSL_NandCtrl.DevInfo.pSpiNandDevInfo = pNandDevInfo;          /* save the pointer to NAND Device Information */
        AmbaRTSL_NandCtrl.pSysPartConfig  = pNandConfig->pSysPartConfig;   /* pointer to System partition configurations */
        AmbaRTSL_NandCtrl.pUserPartConfig = pUserPartConfig;               /* save the pointer to User Partition Configurations */

        /* BBM: large page (OOB size >= 64 Bytes; Page size >= 2K Bytes), check the 0th Byte */
        AmbaRTSL_NandCtrl.BadBlkMarkOffset = 0;        /* the offset of Bad Block Mark */

        if (pNandDevInfo->InternalECC == AMBA_SPINAND_DISABLE_ONDEVICE_ECC) {
            if (SpareByteSize >= (32U * k)) {
                NumEccBit = 8U;  /* Bose-Chaudhuri-Hocquenghem (BCH) error correction (8-bit) */
            } else {
                NumEccBit = 6U;  /* Bose-Chaudhuri-Hocquenghem (BCH) error correction (6-bit) */
            }
        } else {
            NumEccBit = 0;
        }

        if (SpareByteSize >= (32U * k)) {
            AmbaRTSL_NandCtrl.BbtSignatureOffset = 2U;  /* the offset of BBT Signature */
            AmbaRTSL_NandCtrl.BbtVersionOffset   = 8U;  /* the offset of BBT Version */
        } else {
            AmbaRTSL_NandCtrl.BbtSignatureOffset = 1U;  /* the offset of BBT Signature */
            AmbaRTSL_NandCtrl.BbtVersionOffset   = 18U; /* the offset of BBT Version */
        }

        /* 1 - Column Address in 2 Cycles for 2KB/page, 0 - for 512B/page */

        Tmp32 = pNandDevInfo->ChipSize;
        if (AmbaWrap_memcpy(&Tmp8, &Tmp32, sizeof(Tmp8)) != OK) { /* Do nothing */ };

        //NandCtrl.Addr33_32 = (pNandDevInfo->ChipSize > AMBA_NAND_SIZE_4G_BITS) ? 1U : 0U;

        /* Set the timeout value to SpiNand Controller */


        AmbaRTSL_NandCtrl.NumEccBit = NumEccBit;

        /* Block size in Byte */
        AmbaRTSL_NandCtrl.BlkByteSize = MainByteSize * pNandDevInfo->BlockPageSize;

        /* Total number of blocks */
        AmbaRTSL_NandCtrl.TotalNumBlk = pNandDevInfo->TotalPlanes * pNandDevInfo->PlaneBlockSize;

        /* Number of Pages for BST */
        AmbaRTSL_NandCtrl.BstPageCount = GetRoundUpValU32(AMBA_NAND_BOOTSTRAP_CODE_SPACE_SIZE, MainByteSize);

        /* Number of Pages for System Partition Table */
        AmbaRTSL_NandCtrl.SysPtblPageCount = GetRoundUpValU32((UINT32)(sizeof(AMBA_SYS_PARTITION_TABLE_s) & 0xffffffffU), MainByteSize);

        /* Number of Pages for User Partition Table */
        AmbaRTSL_NandCtrl.UserPtblPageCount = GetRoundUpValU32((UINT32)(sizeof(AMBA_USER_PARTITION_TABLE_s) & 0xffffffffU), MainByteSize);

        /* Number of Pages for Vendor Specific Data */
        k = pUserPartConfig[AMBA_USER_PARTITION_PTB].ByteCount;
        AmbaRTSL_NandCtrl.VendorDataPageCount = GetRoundUpValU32(k, MainByteSize);

        /* Number of Pages for BBT: Use 2-bit Mark per block, 1 Byte holds Marks of 4 Blocks */
        AmbaRTSL_NandCtrl.BbtPageCount = GetRoundUpValU32(GetRoundUpValU32(AmbaRTSL_NandCtrl.TotalNumBlk, 4U), MainByteSize);
    }
    return RetVal;
}

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
    UINT32 RetVal = SPINAND_ERR_NONE;
    INT32  ret, fd = -1;
    mtd_info_t meminfo;

    char Mtd[16] = "/dev/mtd0";

    (void) pSpareBuf;
    (void) TimeOut;

    RetVal = AmbaSpiNAND_Lock();
    if (RetVal != OK) {
        return SPINAND_ERR_OS_API_FAIL;
    }
    else {
        fd = open(Mtd, O_RDONLY);    /* open mtd device */
        if (fd < 0) {
            fprintf(stderr, "open %s failed!\n", Mtd);
            RetVal = SPINAND_ERR_IO_FAIL;
        }
        else {
            ret = ioctl(fd, MEMGETINFO, &meminfo);  /* get meminfo */
            if (ret < 0) {
                fprintf(stderr, "get MEMGETINFO failed!\n");
                RetVal = SPINAND_ERR_IO_FAIL;
            }
            else {
                lseek(fd, PageAddr * meminfo.writesize, SEEK_SET);
            }

            if (RetVal == OK) {
                UINT32 size = read(fd, pMainBuf, (NumPage * meminfo.writesize));
                if (size != (NumPage * meminfo.writesize)) {
                    fprintf(stderr, "read err, need :%d, real :%d\n", meminfo.writesize, size );
                    RetVal = SPINAND_ERR_IO_FAIL;
                }
            }
            close(fd);
        }
        //fprintf(stderr, "%s  PageAddr = %d, NumPage = %d , mtd=%s, RetVal = %d\r\n", __func__, PageAddr, NumPage, Mtd, RetVal);
        (void) AmbaSpiNAND_Unlock();
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
    INT32  ret, fd = -1;
    mtd_info_t meminfo;

    char Mtd[16] = "/dev/mtd0";

    (void) pSpareBuf;
    (void) TimeOut;

    RetVal = AmbaSpiNAND_Lock();
    if (RetVal != OK) {
        return SPINAND_ERR_OS_API_FAIL;
    }
    else {
        fd = open(Mtd, O_WRONLY | O_SYNC);   /* open mtd device */
        if (fd < 0) {
            fprintf(stderr, "open %s failed!\n", Mtd);
            RetVal = SPINAND_ERR_IO_FAIL;
        }
        else {
            ret = ioctl(fd, MEMGETINFO, &meminfo);          //get meminfo
            if (ret < 0) {
                fprintf(stderr, "get MEMGETINFO failed!\n");
                RetVal = SPINAND_ERR_IO_FAIL;
            }
            else {
                lseek(fd, PageAddr * meminfo.writesize, SEEK_SET);
            }

            if (RetVal == OK) {
                UINT32 size = write(fd, pMainBuf, (NumPage * meminfo.writesize));
                if (size != (NumPage * meminfo.writesize)) {
                    RetVal = SPINAND_ERR_IO_FAIL;
                }
            }
            close(fd);
        }
        (void) AmbaSpiNAND_Unlock();
        //fprintf(stderr, "%s  PageAddr = %d, mtd=%s, RetVal = %d\r\n", __func__, PageAddr, Mtd, RetVal);
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
    UINT32 RetVal = SPINAND_ERR_NONE;
    char Mtd[16] = "/dev/mtd0";

    (void)TimeOut;

    /*
     * Take the Mutex
     */
    if (AmbaSpiNAND_Lock() != OK) {
        RetVal = SPINAND_ERR_OS_API_FAIL;
    } else {
        INT32 fd, ret;
        mtd_info_t MtdInfo;
        erase_info_t Erase = {0};

        fd = open(Mtd, O_RDWR | O_SYNC);
        if (fd < 0) {
            fprintf(stderr, "open %s failed!\n", Mtd);
            RetVal = SPINAND_ERR_IO_FAIL;
        }
        else {
            ret = ioctl(fd, MEMGETINFO, &MtdInfo);   // get meminfo
            if (ret < 0) {
                fprintf(stderr, "%s  fail! ret=%d\r\n", __func__, ret);
                RetVal = SPINAND_ERR_IO_FAIL;
            }
            else {
                Erase.length = MtdInfo.erasesize;   //set the erase block size
                Erase.start = BlkAddr * MtdInfo.erasesize;

                ret = ioctl(fd, MEMERASE, &Erase);
                if (ret < 0) {
                    fprintf(stderr, "%s  fail! ret=%d\r\n", __func__, ret);
                    RetVal = SPINAND_ERR_IO_FAIL;
                }
            }

            close(fd);
        }
        /*
         * Release the Mutex
         */
        (void) AmbaSpiNAND_Unlock();
    }
    //fprintf(stderr, "%s  BlkAddr = %d, mtd=%s, RetVal = %d\r\n", __func__, BlkAddr, Mtd, RetVal);

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

