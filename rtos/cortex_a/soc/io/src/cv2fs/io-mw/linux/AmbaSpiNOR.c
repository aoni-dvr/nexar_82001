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
//#include "AmbaRTSL_SpiNOR_Ctrl.h"
//#include "AmbaRTSL_SpiNOR.h"
#include "AmbaSpiNOR_Ctrl.h"
//#include "AmbaCSL_SpiNOR.h"
//#include "AmbaRTSL_Cache.h"
//#include "AmbaRTSL_PLL.h"
//#include "AmbaCSL_PLL.h"
//#include "AmbaRTSL_GIC.h"
#include "AmbaIOUtility.h"
#include "AmbaWrap.h"

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
    return NULL;//AmbaRTSL_NorSpiDevInfo;
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
    (void) Offset;
    (void) ByteCount;
    (void) TimeOut;
    return 1;
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
    (void) Offset;
    (void) ByteCount;
    (void) TimeOut;
    return 1;
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
    (void) Offset;
    (void) ByteCount;
    (void) pDataBuf;
    (void) TimeOut;
    return 1;
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
    (void) Offset;
    (void) ByteCount;
    (void) pDataBuf;
    (void) TimeOut;
    return 1;
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
void AmbaSPINOR_RestoreRebootClkSetting(void)
{
}

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
    UINT32 RetVal = 1;
    (void) pNorSpiConfig;

    return RetVal;
}

UINT8 AmbaSpiNOR_Isinit(void)
{
    return 0;//_AmbaNOR_SPI_Ctrl.Isinit;
}

void AmbaSpiNOR_HandleCoreFreqChange(void)
{
}

