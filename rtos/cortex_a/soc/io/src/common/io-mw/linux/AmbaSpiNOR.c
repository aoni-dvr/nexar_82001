/**
 *  @file AmbaSpiNOR.c
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
 *  @details NOR Control APIs
 *
 */

#include "AmbaTypes.h"
#include "AmbaDef.h"
#include "AmbaKAL.h"
#include "AmbaMisraFix.h"

#include "AmbaSPINOR.h"
#include "AmbaDMA.h"
#include "AmbaSpiNOR_Ctrl.h"
//#include "AmbaRTSL_SpiNOR.h"
//#include "AmbaCSL_SpiNOR.h"
//#include "AmbaRTSL_Cache.h"
//#include "AmbaRTSL_PLL.h"
//#include "AmbaCSL_PLL.h"
//#include "AmbaRTSL_GIC.h"
#include "AmbaIOUtility.h"
#include "AmbaWrap.h"

#include <mtd/mtd-user.h>
#include "AmbaRTSL_SpiNOR_Ctrl.h"

/* Value is in us */
#define BUSY_WAITING_TIME    (25U)

typedef struct {
    AMBA_KAL_EVENT_FLAG_t   EventFlag;  /* Event Flags */
    AMBA_KAL_MUTEX_t        Mutex;      /* Mutex */
    UINT32 NorRxChannel;
    UINT32 NorTxChannel;
    UINT8 Isinit;
    UINT8 BusMode;
} AMBA_NOR_SPI_CTRL_s;

static AMBA_NOR_SPI_CTRL_s AmbaNOR_SPI_Ctrl = {0};    /* NOR Management Structure */
#if 0
static AMBA_DMA_DESC_s DmaReq = {
    .PrivData  = { [0] = 0, [1] = 0, [2] = 0, [3] = 0 },
    .pSrcAddr  = NULL, /* should be updated on the fly */
    .pDstAddr  = NULL, /* should be updated on the fly */
    .pNextDesc = NULL,
    .pStatus   = NULL,
    .DataSize  = 0,    /* should be updated on the fly */
    .Ctrl      = {
        .StopOnError   = 1U,
        .IrqOnError    = 0,
        .IrqOnDone     = 0,
        .Reserved0     = 0,
        .BusBlockSize  = DMA_BUS_BLOCK_8BYTE,
        .BusDataSize   = 0, /* should be updated on the fly */
        .NoBusAddrInc  = 1U,
        .ReadMem       = 0, /* should be updated on the fly */
        .WriteMem      = 0, /* should be updated on the fly */
        .EndOfChain    = 1U,
        .Reserved1     = 0,
    }
};
#endif

#ifdef CONFIG_ENABLE_AMBALINK
extern void AmbaIPC_SpiNORLock(void);
extern void AmbaIPC_SpiNORUnlock(void);
#endif

static void AmbaSPINOR_Lock(void)
{
#ifdef CONFIG_ENABLE_AMBALINK
    AmbaIPC_SpiNORLock();
#endif
    (void)AmbaKAL_MutexTake(&AmbaNOR_SPI_Ctrl.Mutex, KAL_WAIT_FOREVER);
}

static void AmbaSPINOR_Unlock(void)
{
    (void)AmbaKAL_MutexGive(&AmbaNOR_SPI_Ctrl.Mutex);
#ifdef CONFIG_ENABLE_AMBALINK
    /*
     * AmbaRTSL_NorWaitInt() will disable IRQ of Spi-NOR,
     * rollback most important one.
     */
    AmbaCSL_NorEnableDataLenthreachIRQ();

    AmbaIPC_SpiNORUnlock();
#endif
}

/*
 *  @RoutineName:: AmbaSpiNOR_SoftReset
 *
 *  @Description::
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */

/*
 *  @RoutineName:: AmbaSpiNOR_SoftReset
 *
 *  @Description::
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */

/*
 *  @RoutineName:: AmbaNOR_TranDoneSet
 *
 *  @Description:: Set event flag for NOR CMD line.
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
 */
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
AMBA_NORSPI_DEV_INFO_s *AmbaSpiNOR_GetDevInfo(void)
{
    return AmbaRTSL_NorSpiDevInfo;
}
/*
 *  @RoutineName:: AmbaRTSL_NorWriteEnable
 *
 *  @Description::
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */

/*
 *  @RoutineName:: AmbaRTSL_NorWriteDisable
 *
 *  @Description::
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
UINT32 AmbaSPINOR_WriteDisable(void)
{
    return 1;
}

/*
 *  @RoutineName:: AmbaNOR_ReadQuadMode
 *
 *  @Description:: Issue the read Quad Mode command to read the Nor Device
 *
 *  @Input      ::
 *
 *  @Output     :: none
 *
 *  @Return     :: INT32 : OK(0)/NG(-1)
 */

/*
 *  @RoutineName::  AmbaSpiNOR_EnterQUAD
 *
 *  @Description::
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */

/*
 *  @RoutineName::  AmbaSpiNOR_ExitQUAD
 *
 *  @Description::
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */


/*
 *  @RoutineName:: AmbaSPINOR_DeviceBusSwitch
 *
 *  @Description::
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */

/*
 *  @RoutineName:: AmbaRTSL_NorClearStatus
 *
 *  @Description::
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
UINT32 AmbaSPINOR_ClearStatus(void)
{
    return 1;
}


/*
 *  @RoutineName:: AmbaNOR_NandReadStatusCmd
 *
 *  @Description:: Issue the read status command to read the NAND status
 *
 *  @Input      ::
 *          AMBA_NAND_DEV_s *pDev     : Pointer to nand device structure
 *
 *  @Output     :: none
 *
 *  @Return     :: INT32 : OK(0)/NG(-1)
 */

/*
 *  @RoutineName:: AmbaSpiNOR_SectorErase
 *
 *  @Description:: To Erase the necessary Sector
 *
 *  @Input      ::
 *      Offset:    starting offset (relative to base addr)
 *      ByteCount: Number of bytes to be erased
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
UINT32 AmbaSpiNOR_SectorErase(UINT32 Offset, UINT32 ByteCount, UINT32 TimeOut)
{
    UINT32 EraseSize = AmbaRTSL_NorSpiCtrl.pNorDevInfo->EraseSectorSize;
    UINT32 BlockStartOffset, BlockEndOffset, NumBlocks;
    UINT32 EraseDone = 0;
    UINT32 CurOffset = Offset, EndOffset = Offset + ByteCount;
    UINT32 RetVal = SPINOR_ERR_NONE;
    char Mtd[16] = "/dev/mtd0";
    INT32 fd, ret;
    erase_info_t Erase = {0};

    (void)TimeOut;
    if ((EraseSize == 0U) || (AmbaRTSL_NorSpiCtrl.TotalByteSize == 0U) || (ByteCount == 0U)) {
        RetVal = SPINOR_ERR_ARG;
    } else {
        /*
         * Take the Mutex
         */
        (void) AmbaSPINOR_Lock();
        fd = open(Mtd, O_RDWR | O_SYNC);
        if (fd < 0) {
            fprintf(stderr, "open %s failed!\n", Mtd);
            RetVal = SPINOR_ERR_IO_FAIL;
        }
        else {
            Erase.length = EraseSize;   //set the erase sector size
            /* Erase the necessary Sectors */
            BlockStartOffset = 0;
            NumBlocks = AmbaRTSL_NorSpiCtrl.TotalByteSize / EraseSize;
            for (UINT32 j = 0; j < NumBlocks; j++) {
                BlockEndOffset = BlockStartOffset + EraseSize;
                if (BlockStartOffset >= EndOffset) {
                    EraseDone = 1U;   /* Automatic Erase is complete. */
                } else if (BlockEndOffset > CurOffset) {
                    Erase.start = BlockStartOffset;
                    ret = ioctl(fd, MEMERASE, &Erase);
                    if (ret < 0) {
                        fprintf(stderr, "%s  fail! ret=%d\r\n", __func__, ret);
                        RetVal = SPINOR_ERR_IO_FAIL;
                    }
                    //fprintf(stderr, "%s  Erase.start: %d\r\n", __func__, Erase.start);
                }
                if (((RetVal != OK)) || (EraseDone == 1U)) {
                    break;
                }
                BlockStartOffset = BlockEndOffset;
            }
            close(fd);
        }
        /*
         * Release the Mutex
         */
        (void) AmbaSPINOR_Unlock();
    }
    //fprintf(stderr, "%s  Offset = %d, ByteCount = %d, mtd=%s, RetVal = %d\r\n", __func__, Offset, ByteCount, Mtd, RetVal);

    return RetVal;
}

/*
 *  @RoutineName:: AmbaSpiNOR_Erase
 *
 *  @Description:: To Erase the necessary Blocks
 *
 *  @Input      ::
 *      Offset:    starting offset (relative to base addr)
 *      ByteCount: Number of bytes to be erased
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
UINT32 AmbaSpiNOR_Erase(UINT32 Offset, UINT32 ByteCount, UINT32 TimeOut)
{
    UINT32 EraseSize = AmbaRTSL_NorSpiCtrl.pNorDevInfo->EraseBlockSize;
    UINT32 CurOffset, EndOffset;
    UINT32 RetVal = SPINOR_ERR_NONE;
    char Mtd[16] = "/dev/mtd0";
    INT32 fd, ret;
    erase_info_t Erase = {0};

    (void)TimeOut;
    if ((EraseSize == 0U) || (AmbaRTSL_NorSpiCtrl.TotalByteSize == 0U) || (ByteCount == 0U)) {
        RetVal = SPINOR_ERR_ARG;
    } else {
        /*
         * Take the Mutex
         */
        (void) AmbaSPINOR_Lock();

        fd = open(Mtd, O_RDWR | O_SYNC);
        if (fd < 0) {
            fprintf(stderr, "open %s failed!\n", Mtd);
            RetVal = SPINOR_ERR_IO_FAIL;
        }
        else {
            if ((Offset == 0U) && (ByteCount >= AmbaRTSL_NorSpiCtrl.TotalByteSize)) {

            } else {
                /* Erase the necessary Blocks */
                UINT32 BlockStartOffset = 0, BlockEndOffset, EraseDone = 0;
                UINT32 NumBlocks = AmbaRTSL_NorSpiCtrl.TotalByteSize / EraseSize;

                CurOffset = Offset;
                EndOffset = Offset + ByteCount;
                Erase.length = EraseSize;   //set the erase block size

                for (UINT32 j = 0; j < NumBlocks; j++) {
                    BlockEndOffset = BlockStartOffset + EraseSize;
                    if (BlockStartOffset >= EndOffset) {
                        EraseDone = 1U;   /* Automatic Erase is complete. */
                    } else if (BlockEndOffset > CurOffset) {
                        Erase.start = BlockStartOffset;

                        ret = ioctl(fd, MEMERASE, &Erase);
                        if (ret < 0) {
                            fprintf(stderr, "%s  fail! ret=%d\r\n", __func__, ret);
                            RetVal = SPINOR_ERR_IO_FAIL;
                        }
                        //fprintf(stderr, "%s  Erase.start: %d\r\n", __func__, Erase.start);
                    }

                    if (((RetVal != OK)) || (EraseDone == 1U)) {
                        break;
                    }
                    BlockStartOffset = BlockEndOffset;
                }
            }
            close(fd);
        }
        /*
         * Release the Mutex
         */
        (void) AmbaSPINOR_Unlock();
    }
    //fprintf(stderr, "%s  Offset = %d, ByteCount = %d, mtd=%s, RetVal = %d\r\n", __func__, Offset, ByteCount, Mtd, RetVal);

    return RetVal;
}

#if 0
static void AmbaNor_SpiReadDMA(UINT32 From, UINT32 ByteCount, UINT8 *pDataBuf)
{
    UINT32 SrcAddr = AMBA_CORTEX_A53_NOR_SPI_BASE_ADDR + AMBA_NORSPI_RXBUF_OFFSET;
    void *pSrc = NULL;

    (void)AmbaRTSL_CacheFlushDataPtr(pDataBuf, ByteCount);

    /* start DMA data transfer */
    /* DmaReq.pSrcAddr = (void *) & (pAmbaNOR_Reg->RxData[0]); */
    AmbaMisra_TypeCast32(&pSrc, &SrcAddr);
    DmaReq.pSrcAddr = pSrc;

    DmaReq.pDstAddr = pDataBuf;
    DmaReq.DataSize = ByteCount;

    DmaReq.Ctrl.StopOnError  = 1U;
    DmaReq.Ctrl.IrqOnError   = 0U;
    DmaReq.Ctrl.IrqOnDone    = 0U;
    DmaReq.Ctrl.BusBlockSize = DMA_BUS_BLOCK_32BYTE;
    DmaReq.Ctrl.BusDataSize  = DMA_BUS_DATA_4BYTE;
    DmaReq.Ctrl.NoBusAddrInc = 1U;
    DmaReq.Ctrl.ReadMem      = 0U;
    DmaReq.Ctrl.WriteMem     = 1U;
    DmaReq.Ctrl.EndOfChain   = 1U;

    (void)AmbaDMA_Transfer(_AmbaNOR_SPI_Ctrl.NorRxChannel, &DmaReq);

    AmbaRTSL_NorSpiReadSetup(From, ByteCount, 1U);
}
#endif

/*
 *  @RoutineName:: AmbaNOR_SpiRead
 *
 *  @Description:: Read Nor flash memory
 *
 *  @Input      ::
 *          UINT32 From           : The Address of SpiNOR device
 *          UINT32 ByteCount      : Length of read Data
 *          UINT8 *pDataBuf       : The Point to the destination buf
 *          UINT32 TimeOut        : TimeOut value
 *
 *  @Output     :: none
 *
 *  @Return     :: INT32 : OK(0)/NG(-1)
 */

/*
 *  @RoutineName:: AmbaSpiNOR_ReadByte
 *
 *  @Description:: Read Nor flash memory
 *
 *  @Input      ::
 *          UINT32 Offset             : The address of SpiNOR device
 *          UINT32 ByteCount          : Length data
 *          UINT8 *pDataBuf           : Pointer to the destination address of Data
 *          UINT32 TimeOut            : TimeOut value
 *
 *  @Output     :: none
 *
 *  @Return     :: INT32 : OK(0)/NG(-1)
 */
UINT32 AmbaSpiNOR_Readbyte(UINT32 Offset, UINT32 ByteCount, UINT8 *pDataBuf, UINT32 TimeOut)
{
    UINT32 RetVal = SPINOR_ERR_NONE;
    INT32 fd = -1;
    char Mtd[16] = "/dev/mtd0";

    (void) TimeOut;

    (void) AmbaSPINOR_Lock();
    fd = open(Mtd, O_RDONLY);    /* open mtd device */
    if (fd < 0) {
        fprintf(stderr, "open %s failed!\n", Mtd);
        RetVal = SPINOR_ERR_IO_FAIL;
    }
    else {
        lseek(fd, Offset, SEEK_SET);

        if (RetVal == OK) {
            UINT32 size = read(fd, pDataBuf, ByteCount);
            if (size != ByteCount) {
                fprintf(stderr, "read err, need :%d, real :%d\n", ByteCount, size);
                RetVal = SPINOR_ERR_IO_FAIL;
            }
        }
        close(fd);
    }
    //fprintf(stderr, "%s  Offset = %d, ByteCount = %d , mtd=%s, RetVal = %d\r\n", __func__, Offset, ByteCount, Mtd, RetVal);
    (void) AmbaSPINOR_Unlock();

    return RetVal;
}

#if 0
static void AmbaNor_SpiWriteDMA(UINT32 Offset, UINT32 ByteCount, UINT8 *pDataBuf)
{
    UINT32 DstAddr = AMBA_CORTEX_A53_NOR_SPI_BASE_ADDR + AMBA_NORSPI_TXBUF_OFFSET;
    void *pDst = NULL;

    /* Clear the remain Data on TxFIFO  */
    if (AmbaCSL_NorGetTxFifoEmpty() == 0U) {
        AmbaRTSL_ResetFIFO();
    }

    (void)AmbaRTSL_CacheCleanDataPtr(pDataBuf, ByteCount);

    /* start DMA data transfer */
    AmbaMisra_TypeCast32(&pDst, &DstAddr); /* DmaReq.pDstAddr = (void *) &(pAmbaNOR_Reg->TxData[0]); */
    DmaReq.pDstAddr = pDst;

    DmaReq.pSrcAddr = pDataBuf;
    DmaReq.DataSize = ByteCount;

    DmaReq.Ctrl.StopOnError  = 1U;
    DmaReq.Ctrl.IrqOnError   = 0U;
    DmaReq.Ctrl.IrqOnDone    = 0U;
    DmaReq.Ctrl.BusBlockSize = DMA_BUS_BLOCK_32BYTE;
    DmaReq.Ctrl.BusDataSize  = DMA_BUS_DATA_4BYTE;
    DmaReq.Ctrl.NoBusAddrInc = 1U;
    DmaReq.Ctrl.ReadMem      = 1U;
    DmaReq.Ctrl.WriteMem     = 0U;
    DmaReq.Ctrl.EndOfChain   = 1U;

    (void) AmbaDMA_Transfer(_AmbaNOR_SPI_Ctrl.NorTxChannel, &DmaReq);

    AmbaRTSL_NorSpiWriteSetup(Offset, ByteCount, pDataBuf, 1U);
}
#endif

/*
 *  @RoutineName:: AmbaNOR_SpiWrite
 *
 *  @Description:: Program Nor flash memory
 *
 *  @Input      ::
 *          UINT32 Offset             : Block to be programmed
 *          UINT32 ByteCount          : Size of Data in Byte
 *          UINT8 *pDataBuf           : Pointer to the destination address of Data
 *          UINT32 TimeOut            : TimeOut value
 *
 *  @Output     :: none
 *
 *  @Return     :: INT32 : OK(0)/NG(-1)*
 */

/*
 *  @RoutineName:: AmbaNorSpi_Program
 *
 *  @Description:: Program Nor flash memory
 *
 *  @Input      ::
 *          UINT32 Offset        : Block to be programmed
 *          UINT32 ByteCount     : Size of Data in Byte
 *          UINT8 *pDataBuf      : Pointer to the source data
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 *-*/
UINT32 AmbaSpiNOR_Program(UINT32 Offset, UINT32 ByteCount, const UINT8 *pDataBuf, UINT32 TimeOut)
{
    UINT32 RetVal = SPINOR_ERR_NONE;
    INT32  fd = -1;
    char Mtd[16] = "/dev/mtd0";

    (void) TimeOut;

    (void) AmbaSPINOR_Lock();
    fd = open(Mtd, O_WRONLY | O_SYNC);   /* open mtd device */
    if (fd < 0) {
        fprintf(stderr, "open %s failed!\n", Mtd);
        RetVal = SPINOR_ERR_IO_FAIL;
    }
    else {
        lseek(fd, Offset, SEEK_SET);

        if (RetVal == OK) {
            UINT32 size = write(fd, pDataBuf, ByteCount);
            if (size != ByteCount) {
                RetVal = SPINOR_ERR_IO_FAIL;
            }
        }
        close(fd);
    }
    (void) AmbaSPINOR_Unlock();
    //fprintf(stderr, "%s  Offset = %d, mtd=%s, RetVal = %d\r\n", __func__, Offset, Mtd, RetVal);
    return RetVal;
}

/*
 *  @RoutineName:: AmbaSpINOR_GetDeviceID
 *
 *  @Description:: Get the Maker ID and Device ID
 *
 *  @Input      ::
 *      pManufactuerID: pointer to the Manufactuer ID
 *
 *  @Description:: Issue the read status command to read the NAND status
 *
 *  @Input      :: UINT8 *Status
 *
 *  @Output     :: none
 *
 *  @Return     :: INT32 : OK(0)/NG(-1)
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */

/*
 *  @RoutineName:: AmbaSPINOR_RestoreRebootClkSetting
 *
 *  @Description::
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
/*void AmbaSPINOR_RestoreRebootClkSetting(void)
{
}*/

/*
 *  @RoutineName:: AmbaSPINOR_Init
 *
 *  @Description:: NOR Flash memory s/w driver init
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
static UINT32 AmbaSPINOR_Init(void)
{
    UINT32 RetVal = OK;
    //UINT32 DmaTxChan = 0, DmaRxChan = 0;

    /* Create a EventFlag */
    if (OK != AmbaKAL_EventFlagCreate(&AmbaNOR_SPI_Ctrl.EventFlag, NULL)) {
        RetVal = SPINOR_ERR_OS_API_FAIL;
    } else if (OK !=  AmbaKAL_MutexCreate(&AmbaNOR_SPI_Ctrl.Mutex, NULL)) {
        RetVal = SPINOR_ERR_OS_API_FAIL;
    } else {
        /* Register FIO command/DMA done call back functions. */
        //AmbaRTSL_NORIsrDoneCallBack = AmbaSPINOR_TransDoneSet;

        //AmbaRTSL_NorInit();

        /* Set DMA channel to SpiNOR RX and TX */
        /*
        RetVal = AmbaDMA_ChannelAllocate(AMBA_DMA_CHANNEL_NOR_SPI_TX, &DmaTxChan);

        if (RetVal == OK) {
            RetVal = AmbaDMA_ChannelAllocate(AMBA_DMA_CHANNEL_NOR_SPI_RX, &DmaRxChan);
        }
        */
        if (RetVal == OK) {
            //AmbaNOR_SPI_Ctrl.NorTxChannel = DmaTxChan;
            //AmbaNOR_SPI_Ctrl.NorRxChannel = DmaRxChan;
            AmbaNOR_SPI_Ctrl.Isinit = 1U;
        }

    }

    return RetVal;
}

static UINT32 AmbaRTSL_NorSpiConfig(AMBA_NOR_SPI_CONFIG_s *pNorSpiConfig)
{
    UINT32 RetVal = OK;
    AMBA_NORSPI_DEV_INFO_s *pNorDevInfo;
    const AMBA_PARTITION_CONFIG_s *pUserPartConfig;
    UINT32 PageSize, k;

    AmbaMisra_TouchUnused(pNorSpiConfig);

    if ((pNorSpiConfig == NULL) ||
        (pNorSpiConfig->pNorSpiDevInfo == NULL) ||
        (pNorSpiConfig->pUserPartConfig == NULL)) {
        RetVal = SPINOR_ERR_ARG;  /* wrong parameters */
    } else {
        pNorDevInfo     = pNorSpiConfig->pNorSpiDevInfo;
        pUserPartConfig = pNorSpiConfig->pUserPartConfig;
        PageSize        = pNorDevInfo->PageSize;

        AmbaRTSL_NorSpiCtrl.pNorDevInfo     = pNorDevInfo;                    /* save the pointer to NorSpi Device Information */
        AmbaRTSL_NorSpiCtrl.pSysPartConfig  = pNorSpiConfig->pSysPartConfig;  /* pointer to System partition configurations */
        AmbaRTSL_NorSpiCtrl.pUserPartConfig = pNorSpiConfig->pUserPartConfig; /* save the pointer to User Partition Configurations */
        AmbaRTSL_NorSpiCtrl.pSpiSetting     = pNorSpiConfig->SpiSetting;

        /* Block size in Byte */
        AmbaRTSL_NorSpiCtrl.TotalByteSize = pNorDevInfo->TotalByteSize;

        /* Total number of blocks */
        AmbaRTSL_NorSpiCtrl.ProgramPageSize = pNorDevInfo->PageSize;

        /* Total number of blocks */
        AmbaRTSL_NorSpiCtrl.PartitionAllocateSize = pNorDevInfo->EraseBlockSize;

        /* Number of Pages for BST, BST total size = Boot header + Bootstrap */
        AmbaRTSL_NorSpiCtrl.BstPageCount = GetRoundUpValU32(AMBA_NORSPI_BOOTSTRAP_CODE_SPACE_SIZE + AMBA_NORSPI_BOOT_HEADER_SIZE, PageSize);

        /* Number of Pages for System Partition Table */
        AmbaRTSL_NorSpiCtrl.SysPtblPageCount = GetRoundUpValU32((UINT32)(sizeof(AMBA_SYS_PARTITION_TABLE_s) & 0xffffffffU), PageSize);

        /* Number of Pages for User Partition Table */
        AmbaRTSL_NorSpiCtrl.UserPtblPageCount = GetRoundUpValU32((UINT32)(sizeof(AMBA_USER_PARTITION_TABLE_s) & 0xffffffffU), PageSize);

        /* Number of Pages for Vendor Specific Data */
        k = pUserPartConfig[AMBA_USER_PARTITION_PTB].ByteCount;
        AmbaRTSL_NorSpiCtrl.VendorDataPageCount = GetRoundUpValU32(k, PageSize);

        /* SPI Interface Config */
        //AmbaRTSL_NorSetSPIClk(pNorDevInfo->SpiFrequncy);
        //AmbaRTSL_NorSetSPISetting(pNorSpiConfig->SpiSetting);
    }
    return RetVal;
}

/*
 *  @RoutineName:: AmbaSpiNOR_Config
 *
 *  @Description:: NOR Flash memory s/w driver init
 *
 *  @Input      :: pointer to SpiNOR software configurations
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
UINT32 AmbaSpiNOR_Config(AMBA_NOR_SPI_CONFIG_s *pNorSpiConfig)
{
    UINT32 RetVal;

#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
    (void) AmbaCSL_NorSmErrorDisable(); /* Disable SM error detection before program nor_spi controller */
#endif
    RetVal = AmbaSPINOR_Init();

    if (RetVal == OK) {
        RetVal = AmbaRTSL_NorSpiConfig(pNorSpiConfig);
    }

    if (RetVal == OK) {
        //RetVal = AmbaSpiNOR_SoftReset();   /* Reset Nor Flash Device*/
    }
#if defined (CONFIG_SOC_CV5)|| defined (CONFIG_SOC_CV52)|| defined (CONFIG_SOC_CV2FS)|| defined (CONFIG_SOC_CV22FS)
#ifndef CONFIG_AMBALINK_RPMSG_G1
    /*
    if (AmbaRTSL_NorSpiCtrl.pNorDevInfo->Read.DataLane == AMBA_NORSPI_DATA_8_LANE) {
        AmbaSPINOR_Setup8bitDevice();
    }
    */
#endif
#endif /* (CV2FS) || (CV5) */
    if (RetVal == OK) {
        //RetVal = AmbaSPINOR_GetDeviceID(); /* Get the Maker ID and Device ID */
    }

    if ((RetVal == OK) &&
        (pNorSpiConfig->pNorSpiDevInfo->AddrByte == AMBA_NORSPI_4BYTE_ADDR)) {
        //RetVal = AmbaSpiNOR_SetExtAddr();
    }

    if (RetVal == OK) {
        RetVal = AmbaSPINOR_InitUserPartLock();
    }

    if (RetVal == OK) {
        RetVal = AmbaSpiNOR_InitPtbBbt(1000U); /* Init System/User Partition Tables */
    }

    if (RetVal == OK) {
#if defined(CONFIG_MUTI_BOOT_DEVICE)
        if (((pNorSpiConfig->pUserPartConfig[AMBA_USER_PARTITION_DSP_uCODE].Attribute & AMBA_PARTITION_ATTR_STORE_DEVIC) >> 6) == AMBA_NVM_SPI_NOR) {
            RetVal = AmbaSpiNOR_LoadNvmRomFileTable();
        }
#else
        RetVal = AmbaSpiNOR_LoadNvmRomFileTable();
#endif
    }

#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
    (void) AmbaCSL_NorSmErrorEnable(); /* Enable SM error detection after program nor_spi controller */
#endif
    return RetVal;
}

UINT8 AmbaSpiNOR_Isinit(void)
{
    return AmbaNOR_SPI_Ctrl.Isinit;
}

void AmbaSpiNOR_HandleCoreFreqChange(void)
{
}

