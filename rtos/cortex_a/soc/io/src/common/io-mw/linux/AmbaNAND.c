/**
 *  @file AmbaNAND.c
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
#include "AmbaDef.h"
#include "AmbaNAND.h"
#include "AmbaNAND_Ctrl.h"
#include "AmbaNAND_OP.h"

#include "AmbaRTSL_Cache.h"
#include "AmbaRTSL_FIO.h"
#include "AmbaRTSL_NAND.h"
#include "AmbaRTSL_NAND_Ctrl.h"

#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <mtd/mtd-user.h>

/*
 * Definitions for EventFlag
 */
#define AMBA_FIO_NAND_CMD_DONE_FLAG     0x1U
#define DISABLE_COPYBACK

#define AMBA_FIO_EVENT_MASK (AMBA_FIO_NAND_CMD_DONE_FLAG)

typedef struct  {
    AMBA_KAL_EVENT_FLAG_t   EventFlag;  /* Event Flags */
    AMBA_KAL_MUTEX_t        Mutex;      /* Mutex */
} AMBA_NAND_CTRL_s;

static AMBA_NAND_CTRL_s _AmbaNAND_Ctrl = {0};    /* NAND Management Structure */
static void AmbaNAND_HookApi(const AMBA_NAND_CONFIG_s *pNandConfig);


/*
 *  @RoutineName:: AmbaNAND_Lock
 *
 *  @Description:: NAND Lock
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
UINT32 AmbaNAND_Lock(void)
{
    if(AmbaNandOp_PreAccessNotify != NULL) {
        AmbaNandOp_PreAccessNotify();
    }

    return AmbaKAL_MutexTake(&_AmbaNAND_Ctrl.Mutex, 0xFFFFFFFFUL);
}

/*
 *  @RoutineName:: AmbaNAND_Unlock
 *
 *  @Description:: Unlock NAND
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
UINT32 AmbaNAND_Unlock(void)
{
    UINT32 Rval = AmbaKAL_MutexGive(&_AmbaNAND_Ctrl.Mutex);

    if(AmbaNandOp_PostAccessNotify != NULL) {
        AmbaNandOp_PostAccessNotify();
    }

    return Rval;
}

/*
 *  @RoutineName:: AmbaNAND_Init
 *
 *  @Description:: Initialize NAND data structure.
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          UINT32 : OK(0)/NG(-1)
 */
static UINT32 AmbaNAND_Init(void)
{
    UINT32 RetVal = NAND_ERR_NONE;

    /* Create Event Flag */
    if (AmbaKAL_EventFlagCreate(&_AmbaNAND_Ctrl.EventFlag, NULL) != OK) {
        RetVal = NAND_ERR_OS_API_FAIL;  /* should never happen ! */
    } else {
        /* Create Mutex */
        if (AmbaKAL_MutexCreate(&_AmbaNAND_Ctrl.Mutex, NULL) != OK) {
            RetVal = NAND_ERR_OS_API_FAIL;  /* should never happen ! */
        } else {
            //(void)AmbaRTSL_NandInit();
            //AmbaRTSL_FioInit();             /* Initialize IRQ of the FIO controller */

            //_AmbaRTSL_FioNandCmdIsrCallBack = NAND_FioNandCmdIsr; /* Call back function when NAND Command done */
        }
    }

    return RetVal;
}

/*
 *  @RoutineName:: AmbaNAND_ReadID
 *
 *  @Description:: Read device ID
 *
 *  @Input      ::
 *      NumReadCycle: number of cycles
 *      pDeviceID:    pointer to the buffer of Device ID
 *      TimeOut:      Time out value
 *
 *  @Output     ::
 *      pDeviceID: pointer to the Device ID
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
UINT32 AmbaNAND_ReadID(UINT8 NumReadCycle, UINT8 *pDeviceID, UINT32 TimeOut)
{
    UINT32 RetVal;
    UINT32 ActualFlags = 0U;
    (void)NumReadCycle;
    (void)pDeviceID;

    /*
     * Take the Mutex
     */
    if (AmbaNAND_Lock() != OK) {
        RetVal = NAND_ERR_OS_API_FAIL;
    } else {
        RetVal = AmbaNAND_Reset(TimeOut);
        if (RetVal == NAND_ERR_NONE) {
#if 0
            (void )AmbaKAL_EventFlagClear(&_AmbaNAND_Ctrl.EventFlag, AMBA_FIO_NAND_CMD_DONE_FLAG);
            //AmbaRTSL_NandSendReadIdCmd(NumReadCycle);

            /* wait for Command Done: Event Flag ! */
            RetVal = AmbaKAL_EventFlagGet(&_AmbaNAND_Ctrl.EventFlag, AMBA_FIO_NAND_CMD_DONE_FLAG,
                                          0x1U, 0x1U, &ActualFlags, TimeOut);
            if (RetVal == NAND_ERR_NONE) {
                //AmbaRTSL_NandGetReadIdResponse(NumReadCycle, pDeviceID);
            }
#endif
            (void)ActualFlags;
            (void)TimeOut;

            /*
             * Release the Mutex
             */
            (void) AmbaNAND_Unlock();
        }
    }
    return RetVal;
}

static UINT32 AmbaRTSL_NandConfigDeviceInfo(AMBA_NAND_CONFIG_s *pNandConfig)
{
    UINT32 RetVal = NAND_ERR_NONE;
    AMBA_NAND_DEV_INFO_s *pNandDevInfo;
    AMBA_PARTITION_CONFIG_s *pUserPartConfig;
    UINT32 SpareByteSize;
    UINT32 MainByteSize;
    UINT32 NumEccBit, k, Tmp32;
    UINT8  Tmp8 = 0;

    if ((pNandConfig == NULL) || (pNandConfig->pNandDevInfo == NULL) || (pNandConfig->pUserPartConfig == NULL)) {
        RetVal = NAND_ERR_ARG;  /* wrong parameters */
    } else {

        pNandDevInfo    = pNandConfig->pNandDevInfo;
        pUserPartConfig = pNandConfig->pUserPartConfig;
        SpareByteSize   = pNandDevInfo->SpareByteSize;
        MainByteSize    = pNandDevInfo->MainByteSize;

        AmbaRTSL_NandCtrl.NandInfo.BlockPageSize  = pNandDevInfo->BlockPageSize;
        AmbaRTSL_NandCtrl.NandInfo.MainByteSize   = pNandDevInfo->MainByteSize;
        AmbaRTSL_NandCtrl.NandInfo.PlaneAddrMask  = pNandDevInfo->PlaneAddrMask;
        AmbaRTSL_NandCtrl.NandInfo.PlaneBlockSize = pNandDevInfo->PlaneBlockSize;
        AmbaRTSL_NandCtrl.NandInfo.SpareByteSize  = pNandDevInfo->SpareByteSize;
        AmbaRTSL_NandCtrl.NandInfo.TotalPlanes    = pNandDevInfo->TotalPlanes;
        AmbaRTSL_NandCtrl.NandInfo.TotalZones     = pNandDevInfo->TotalZones;
        AmbaRTSL_NandCtrl.NandInfo.ZoneBlockSize  = pNandDevInfo->ZoneBlockSize;

        AmbaRTSL_NandCtrl.DevInfo.pNandDevInfo    = pNandDevInfo;         /* save the pointer to NAND Device Information */

        AmbaRTSL_NandCtrl.pSysPartConfig  = pNandConfig->pSysPartConfig;   /* pointer to System partition configurations */
        AmbaRTSL_NandCtrl.pUserPartConfig = pUserPartConfig;               /* save the pointer to User Partition Configurations */

        k = MainByteSize / 512U;

        /* BBM: large page (OOB size >= 64 Bytes; Page size >= 2K Bytes), check the 0th Byte */
        AmbaRTSL_NandCtrl.BadBlkMarkOffset = 0;        /* the offset of Bad Block Mark */

        if (SpareByteSize >= (32U * k)) {
            NumEccBit = 8U;  /* Bose-Chaudhuri-Hocquenghem (BCH) error correction (8-bit) */
            AmbaRTSL_NandCtrl.BbtSignatureOffset = 2U;  /* the offset of BBT Signature */
            AmbaRTSL_NandCtrl.BbtVersionOffset   = 8U;  /* the offset of BBT Version */

        } else {
            NumEccBit = 6U;  /* Bose-Chaudhuri-Hocquenghem (BCH) error correction (6-bit) */
            AmbaRTSL_NandCtrl.BbtSignatureOffset = 1U;  /* the offset of BBT Signature */
            AmbaRTSL_NandCtrl.BbtVersionOffset   = 18U; /* the offset of BBT Version */
        }

        Tmp32 = pNandDevInfo->ChipSize & AMBA_NAND_SIZE_8G_BITS;
        (void)AmbaWrap_memcpy(&Tmp8, &Tmp32, sizeof(Tmp8));


        AmbaRTSL_NandCtrl.NumEccBit = NumEccBit;

        /* Block size in Byte */
        AmbaRTSL_NandCtrl.BlkByteSize = MainByteSize * pNandDevInfo->BlockPageSize;

        /* Total number of blocks */
        AmbaRTSL_NandCtrl.TotalNumBlk = pNandDevInfo->TotalPlanes * pNandDevInfo->PlaneBlockSize;

        /* Number of Pages for BST */
        AmbaRTSL_NandCtrl.BstPageCount = GetRoundUpValU32(AMBA_NAND_BOOTSTRAP_CODE_SPACE_SIZE, MainByteSize);

        /* Number of Pages for System Partition Table */
        AmbaRTSL_NandCtrl.SysPtblPageCount = GetRoundUpValU32(sizeof(AMBA_SYS_PARTITION_TABLE_s), MainByteSize);

        /* Number of Pages for User Partition Table */
        AmbaRTSL_NandCtrl.UserPtblPageCount = GetRoundUpValU32(sizeof(AMBA_USER_PARTITION_TABLE_s), MainByteSize);

        /* Number of Pages for Vendor Specific Data */
        k = pUserPartConfig[AMBA_USER_PARTITION_PTB].ByteCount;
        AmbaRTSL_NandCtrl.VendorDataPageCount = GetRoundUpValU32(k, MainByteSize);

        /* Number of Pages for BBT: Use 2-bit Mark per block, 1 Byte holds Marks of 4 Blocks */
        AmbaRTSL_NandCtrl.BbtPageCount = GetRoundUpValU32(GetRoundUpValU32(AmbaRTSL_NandCtrl.TotalNumBlk, 4), MainByteSize);
    }
    return RetVal;
}

/*
 *  @RoutineName:: AmbaNAND_Config
 *
 *  @Description:: NAND software configurations
 *
 *  @Input      ::
 *      pNandConfig: pointer to NAND software configurations
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          UINT32 : OK(0)/NG(-1)
 */
UINT32 AmbaNAND_Config(AMBA_NAND_CONFIG_s *pNandConfig)
{
    UINT32 RetVal;

    RetVal = AmbaNAND_Init();
    if (RetVal == NAND_ERR_NONE) {
        AmbaNAND_HookApi(pNandConfig);
        (void)AmbaNAND_Lock();
        RetVal = AmbaNAND_Reset(1000);
        (void)AmbaNAND_Unlock();
    }

    if (RetVal == NAND_ERR_NONE) {
        (void) AmbaRTSL_NandConfigDeviceInfo(pNandConfig);
        /* Init BBT and System/User Partition Tables */
        RetVal = AmbaNAND_InitPtbBbt(1000);
    }

    if (RetVal == NAND_ERR_NONE) {
        RetVal = AmbaNAND_LoadNvmRomFileTable();   /* Load NAND ROM File Table */
    }

    return RetVal;
}

/*
 *  @RoutineName:: AmbaNAND_GetDevInfo
 *
 *  @Description:: get the pointer to current NAND device information
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *      AMBA_NAND_DEV_INFO_s * : the pointer to current NAND device information
 */
AMBA_NAND_DEV_INFO_s *AmbaNAND_GetDevInfo(void)
{
    return AmbaRTSL_NandDevInfo;
}

AMBA_NAND_COMMON_INFO_s *AmbaNAND_GetCommonInfo(void)
{
    return AmbaRTSL_NandCommonInfo;
}

/*
 *  @RoutineName:: AmbaNAND_Reset
 *
 *  @Description:: Reset NAND data structure.
 *
 *  @Input      ::
 *      TimeOut:   The timeout value
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
UINT32 AmbaNAND_Reset(UINT32 TimeOut)
{
    UINT32 RetVal = NAND_ERR_NONE;
#if 0
    UINT32 ActualFlags = 0U;

    (void) AmbaKAL_EventFlagClear(&_AmbaNAND_Ctrl.EventFlag, AMBA_FIO_NAND_CMD_DONE_FLAG);

    //AmbaRTSL_NandSendResetCmd();

    if (OK != AmbaKAL_EventFlagGet(&_AmbaNAND_Ctrl.EventFlag, AMBA_FIO_NAND_CMD_DONE_FLAG,
                                   0x1U, 0x1U, &ActualFlags, TimeOut)) {
        RetVal = NAND_ERR_IO_FAIL;
    }
#else
    (void)TimeOut;
#endif
    return RetVal;
}
#if 0
/*
 *  @RoutineName:: AmbaNAND_ReadStatus
 *
 *  @Description:: Get Nand current status.
 *
 *  @Input      ::
 *      pStatus: pointer to the Status
 *
 *  @Output     ::
 *      pStatus: pointer to the Status
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
INT32 AmbaNAND_ReadStatus(AMBA_NAND_STATUS_u *pStatus, UINT32 TimeOut)
{
    UINT32 ActualFlags = 0U;
    INT32 RetVal;

    /*
     * Take the Mutex
     */
    if (AmbaNAND_Lock() != OK) {
        RetVal = NG;
    } else {

        (void) AmbaKAL_EventFlagClear(&_AmbaNAND_Ctrl.EventFlag, AMBA_FIO_NAND_CMD_DONE_FLAG);

        AmbaRTSL_NandSendReadStatusCmd();

        RetVal = AmbaKAL_EventFlagGet(&_AmbaNAND_Ctrl.EventFlag, AMBA_FIO_NAND_CMD_DONE_FLAG, 0x1U, 0x1U, &ActualFlags, TimeOut);
        if (RetVal == OK) {
            AmbaRTSL_NandGetCmdResponse((UINT8*)pStatus);
        }

        /*
         * Release the Mutex
         */
        (void) AmbaNAND_Unlock();
    }
    return RetVal;
}
#endif
/*
 *  @RoutineName:: AmbaNAND_Read
 *
 *  @Description:: Read data from NAND flash
 *
 *  @Input      ::
 *      PageAddr:  The first page address to read
 *      NumPage:   Number of pages to read
 *      pMainBuf:  pointer to DRAM buffer for main area data
 *      pSpareBuf: pointer to DRAM buffer for spare area data
 *      TimeOut:   The timeout value
 *
 *  @Output     ::
 *      pMainBuf:  pointer to main area data
 *      pSpareBuf: pointer to spare area data
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
UINT32 AmbaNAND_Read(UINT32 PageAddr, UINT32 NumPage, UINT8 *pMainBuf, UINT8 *pSpareBuf, UINT32 TimeOut)
{
    UINT32 RetVal = NAND_ERR_NONE;
    INT32  ret, fd = -1;
    mtd_info_t meminfo;

    char Mtd[16] = "/dev/mtd0";

    (void) pSpareBuf;
    (void) TimeOut;

    RetVal = AmbaNAND_Lock();
    if (RetVal != OK) {
        return NAND_ERR_OS_API_FAIL;
    }
    else {
        fd = open(Mtd, O_RDONLY);    /* open mtd device */
        if (fd < 0) {
            fprintf(stderr, "open %s failed!\n", Mtd);
            RetVal = NAND_ERR_IO_FAIL;
        }
        else {
            ret = ioctl(fd, MEMGETINFO, &meminfo);  /* get meminfo */
            if (ret < 0) {
                fprintf(stderr, "get MEMGETINFO failed!\n");
                RetVal = NAND_ERR_IO_FAIL;
            }
            else {
                lseek(fd, PageAddr * meminfo.writesize, SEEK_SET);
            }

            if (RetVal == OK) {
                UINT32 size = read(fd, pMainBuf, (NumPage * meminfo.writesize));
                if (size != (NumPage * meminfo.writesize)) {
                    fprintf(stderr, "read err, need :%d, real :%d\n", meminfo.writesize, size );
                    RetVal = NAND_ERR_IO_FAIL;
                }
            }
            close(fd);
        }
        //fprintf(stderr, "%s  PageAddr = %d, NumPage = %d , mtd=%s, RetVal = %d", __func__, PageAddr, NumPage, Mtd, RetVal);
        (void) AmbaNAND_Unlock();
    }
    return RetVal;
}

/*
 *  @RoutineName:: AmbaNAND_Program
 *
 *  @Description:: Perform NAND write data cmd flow setup
 *
 *  @Input      ::
 *      PageAddr:  The first page address to write
 *      NumPage:   Number of pages to write
 *      pMainBuf:  pointer to DRAM buffer for main area data
 *      pSpareBuf: pointer to DRAM buffer for spare area data
 *      TimeOut:   The timeout value
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
UINT32 AmbaNAND_Program(UINT32 PageAddr, UINT32 NumPage, UINT8 *pMainBuf, UINT8 *pSpareBuf, UINT32 TimeOut)
{
    UINT32 RetVal = NAND_ERR_NONE;
    INT32  ret, fd = -1;
    mtd_info_t meminfo;

    char Mtd[16] = "/dev/mtd0";

    (void) pSpareBuf;
    (void) TimeOut;

    RetVal = AmbaNAND_Lock();
    if (RetVal != OK) {
        return NAND_ERR_OS_API_FAIL;
    }
    else {
        fd = open(Mtd, O_WRONLY | O_SYNC);    /* open mtd device */
        if (fd < 0) {
            fprintf(stderr, "open %s failed!\n", Mtd);
            RetVal = NAND_ERR_IO_FAIL;
        }
        else {
            ret = ioctl(fd, MEMGETINFO, &meminfo);  /* get meminfo */
            if (ret < 0) {
                fprintf(stderr, "get MEMGETINFO failed!\n");
                RetVal = NAND_ERR_IO_FAIL;
            }
            else {
                lseek(fd, PageAddr * meminfo.writesize, SEEK_SET);
            }

            if (RetVal == OK) {
                UINT32 size = write(fd, pMainBuf, (NumPage * meminfo.writesize));
                if (size != (NumPage * meminfo.writesize)) {
                    RetVal = NAND_ERR_IO_FAIL;
                }
            }
            close(fd);
        }
        //fprintf(stderr, "%s  PageAddr = %d, NumPage = %d , mtd=%s, RetVal = %d", __func__, PageAddr, NumPage, Mtd, RetVal);
        (void) AmbaNAND_Unlock();
    }
    return RetVal;
}

/*
 *  @RoutineName:: NandPseudoCB
 *
 *  @Description:: Copyback by read and program command.
 *
 *  @Input      ::
 *          UINT32 BlockFrom    : Copy from
 *          UINT32 Page         : Page to copied
 *          UINT32 BlockTo      : Copy to
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
static UINT32 AmbaNAND_PseudoCB(UINT32 DestPageAddr, UINT32 SrcPageAddr, UINT32 TimeOut)
{
    /* Work Buffer for 1 Block.  */
    static UINT8 PseudoCBBufMain[64 * 2 * 1024]
    GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE;

    static UINT8 PseudoCBBufSpare[64 * 128]
    GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE;

    UINT32 RetVal;

    RetVal = AmbaNAND_Read(SrcPageAddr, 1, PseudoCBBufMain, PseudoCBBufSpare, TimeOut);
    if (RetVal == NAND_ERR_NONE) {
        RetVal = AmbaNAND_Program(DestPageAddr, 1, PseudoCBBufMain, PseudoCBBufSpare, TimeOut);
    }

    return RetVal;
}

/*
 *  @RoutineName:: AmbaNAND_CopyBack
 *
 *  @Description:: Copy data from source page to destinaiton page
 *
 *  @Input      ::
 *      DestPageAddr: Destination page address
 *      SrcPageAddr: Source page address
 *      TimeOut:     Time out value
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
UINT32 AmbaNAND_CopyBack(UINT32 DestPageAddr, UINT32 SrcPageAddr, UINT32 TimeOut)
{
#if !defined(DISABLE_COPYBACK)
    AMBA_NAND_DEV_INFO_s *pNandDevInfo = AmbaRTSL_NandDevInfo;
    UINT32 MainByteSize = 0;
    UINT32 ActualFlags = 0U;
    INT32 RetVal = OK;

    if (pNandDevInfo == NULL) {
        RetVal = -1;
    } else {
        AmbaCSL_NandDisableWriteProtect();

        MainByteSize = pNandDevInfo->MainByteSize;
        (void) AmbaKAL_EventFlagClear(&_AmbaNAND_Ctrl.EventFlag, AMBA_FIO_NAND_CMD_DONE_FLAG);
        AmbaCSL_NandSendCopyBackCmd(SrcPageAddr * MainByteSize, DestPageAddr * MainByteSize);

        RetVal = AmbaKAL_EventFlagGet(&_AmbaNAND_Ctrl.EventFlag, AMBA_FIO_NAND_CMD_DONE_FLAG, 0x1U, 0x1U, &ActualFlags, TimeOut);
        if (RetVal != OK) {
            return RetVal;
        }

        AmbaCSL_NandGetCmdResponse((UINT8 *) & (AmbaRTSL_NandCtrl.Status[0]));

        AmbaCSL_NandEnableWriteProtect();

        return AmbaRTSL_NandCtrl.Status[0].Bits[0].LastCmdFailed ? -1 : OK;
    }
#else
    return AmbaNAND_PseudoCB(DestPageAddr, SrcPageAddr, TimeOut);
#endif
}

/*
 *  @RoutineName:: AmbaNAND_EraseBlock
 *
 *  @Description:: To erase the block of data identified by the block address parameter
 *
 *  @Input      ::
 *      BlkAddr: Block address
 *      TimeOut: Time out value
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
UINT32 AmbaNAND_EraseBlock(UINT32 BlkAddr, UINT32 TimeOut)
{
    UINT32 RetVal = NAND_ERR_NONE;
    char Mtd[16] = "/dev/mtd0";

    (void)TimeOut;

    /*
     * Take the Mutex
     */
    if (AmbaNAND_Lock() != OK) {
        RetVal = NAND_ERR_OS_API_FAIL;
    } else {
        INT32 fd, ret;
        mtd_info_t MtdInfo;
        erase_info_t Erase = {0};

        fd = open(Mtd, O_RDWR | O_SYNC);
        if (fd < 0) {
            fprintf(stderr, "open %s failed!\n", Mtd);
            RetVal = NAND_ERR_IO_FAIL;
        }
        else {
            ret = ioctl(fd, MEMGETINFO, &MtdInfo);   // get meminfo
            if (ret < 0) {
                fprintf(stderr, "%s  fail! ret=%d\r\n", __func__, ret);
                RetVal = NAND_ERR_IO_FAIL;
            }
            else {
                Erase.length = MtdInfo.erasesize;   //set the erase block size
                Erase.start = BlkAddr * MtdInfo.erasesize;

                ret = ioctl(fd, MEMERASE, &Erase);
                if (ret < 0) {
                    fprintf(stderr, "%s  fail! ret=%d\r\n", __func__, ret);
                    RetVal = NAND_ERR_IO_FAIL;
                }
            }

            close(fd);
        }
        /*
         * Release the Mutex
         */
        (void) AmbaNAND_Unlock();
    }
    //fprintf(stderr, "%s  BlkAddr = %d, mtd=%s, RetVal = %d\r\n", __func__, BlkAddr, Mtd, RetVal);

    return RetVal;
}

static void AmbaNAND_HookApi(const AMBA_NAND_CONFIG_s *pNandConfig)
{
    AmbaNandOp_Copyback   = AmbaNAND_CopyBack;
    AmbaNandOp_EraseBlock = AmbaNAND_EraseBlock;
    AmbaNandOp_Read       = AmbaNAND_Read;
    AmbaNandOp_Program    = AmbaNAND_Program;

    AmbaNandOp_PreAccessNotify  = pNandConfig->AmbaNandPreAccessNotify;
    AmbaNandOp_PostAccessNotify = pNandConfig->AmbaNandPostAccessNotify;
}

void AmbaSysInitNand(void)
{
    extern AMBA_NAND_DEV_INFO_s AmbaNAND_DevInfo;
    extern AMBA_PARTITION_CONFIG_s AmbaNAND_SysPartConfig[AMBA_NUM_SYS_PARTITION];
    extern AMBA_PARTITION_CONFIG_s AmbaNAND_UserPartConfig[AMBA_NUM_USER_PARTITION];
    extern UINT32 AmbaNAND_FtlInit(UINT32 UserPartID);

    AMBA_NAND_CONFIG_s NandConfig = {
        .pNandDevInfo    =  &AmbaNAND_DevInfo,
        .pSysPartConfig  =  &(AmbaNAND_SysPartConfig[0]),   /* pointer to System partition configurations */
        .pUserPartConfig =  &(AmbaNAND_UserPartConfig[0]),  /* pointer to partition configurations */
    };

    /* NAND software configurations */
    if (AmbaNAND_Config(&NandConfig) != OK) {
        //AmbaPrint_PrintUInt5("NAND_Config Fail 0x%X", 0U, 0U, 0U, 0U, 0U);
    }
}


