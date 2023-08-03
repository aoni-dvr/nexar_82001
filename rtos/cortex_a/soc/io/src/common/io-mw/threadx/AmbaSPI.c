/**
 *  @file AmbaSPI.c
 *
 *  @copyright Copyright (c) 2021 Ambarella, Inc.
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
 *  @details SPI APIs
 *
 */

#include "AmbaTypes.h"
#include "AmbaDef.h"
#include "AmbaMisraFix.h"
#include "AmbaKAL.h"

#include "AmbaDrvEntry.h"
#include "AmbaCache.h"
#include "AmbaDMA.h"
#include "AmbaSPI.h"
#include "AmbaRTSL_DMA.h"
#include "AmbaRTSL_SPI.h"

static AMBA_KAL_EVENT_FLAG_t AmbaSpiMstEventFlag;
static AMBA_KAL_EVENT_FLAG_t AmbaSpiSlvEventFlag;
static AMBA_KAL_MUTEX_t AmbaSpiMstMutex[AMBA_NUM_SPI_MASTER];
static AMBA_KAL_MUTEX_t AmbaSpiSlvMutex[AMBA_NUM_SPI_SLAVE];

static UINT32 AmbaSpiMasterIntHandlerErr[AMBA_NUM_SPI_MASTER];
static UINT32 AmbaSpiSlaveIntHandlerErr[AMBA_NUM_SPI_SLAVE];

static void SPI_MasterIntHandler(UINT32 MasterID);
static void SPI_SlaveIntHandler(UINT32 SlaveID);

/**
 *  SPI_DmaSetup - Configure DMA for SPI master
 *  @param[in] MasterID Indicate a SPI master
 *  @param[in] DmaChan Allocated DMA channel for either TX or RX
 *  @param[in] DataWidth Bit width of single spi data
 *  @param[in] DataSize The size of TX and RX buffer if exist
 *  @param[in] pTxDataBuf TX buffer. If this pointer is NULL, the signal on SDO/MOSI pin will be ignored.
 *  @param[in] pRxDataBuf RX buffer. If this pointer is NULL, the signal on SDI/MISO pin will be ignored.
 *  @return error code
 */
static UINT32 SPI_DmaSetup(UINT32 MasterID, UINT32 DmaChan, UINT32 DataWidth, UINT32 DataSize, const void *pTxDataBuf, const void *pRxDataBuf)
{
    AMBA_DMA_DESC_s SpiDmaDesc;
    ULONG DataPortAddr, DataBufAddr;
    void *pSrcAddr = NULL, *pDstAddr = NULL;

    if (DataWidth == 1U) {
        SpiDmaDesc.DataSize = DataSize;
        SpiDmaDesc.Ctrl.BusDataSize = DMA_BUS_DATA_1BYTE;
    } else {
        SpiDmaDesc.DataSize = DataSize * 2U;
        SpiDmaDesc.Ctrl.BusDataSize = DMA_BUS_DATA_2BYTE;
    }

    /* Fill DMA control data */
    SpiDmaDesc.Ctrl.StopOnError = 1U;
    SpiDmaDesc.Ctrl.IrqOnError = 0U;
    SpiDmaDesc.Ctrl.IrqOnDone = 0U;
    SpiDmaDesc.Ctrl.BusBlockSize = DMA_BUS_BLOCK_8BYTE;
    SpiDmaDesc.Ctrl.NoBusAddrInc = 1U;
    SpiDmaDesc.Ctrl.EndOfChain = 1U;

    AmbaRTSL_SpiMasterGetPortAddr(MasterID, &DataPortAddr);
    if (pTxDataBuf != NULL) {
        /* Flush cache */
        AmbaMisra_TypeCast(&DataBufAddr, &pTxDataBuf);
        (void)AmbaCache_DataFlush(DataBufAddr, SpiDmaDesc.DataSize);

        AmbaMisra_TypeCast(&pSrcAddr, &pTxDataBuf);
        AmbaMisra_TypeCast(&pDstAddr, &DataPortAddr);
        SpiDmaDesc.Ctrl.ReadMem = 1U;
        SpiDmaDesc.Ctrl.WriteMem = 0U;
    }
    if (pRxDataBuf != NULL) {
        /* Flush cache */
        AmbaMisra_TypeCast(&DataBufAddr, &pRxDataBuf);
        (void)AmbaCache_DataFlush(DataBufAddr, SpiDmaDesc.DataSize);

        AmbaMisra_TypeCast(&pSrcAddr, &DataPortAddr);
        AmbaMisra_TypeCast(&pDstAddr, &pRxDataBuf);
        SpiDmaDesc.Ctrl.ReadMem = 0U;
        SpiDmaDesc.Ctrl.WriteMem = 1U;
    }

    SpiDmaDesc.pSrcAddr = pSrcAddr;
    SpiDmaDesc.pDstAddr = pDstAddr;
    SpiDmaDesc.pNextDesc = NULL;
    SpiDmaDesc.pStatus = NULL;

    /* start DMA data transfer */
    return AmbaDMA_Transfer(DmaChan, &SpiDmaDesc);
}

/**
 *  SPI_MasterSetDmaChan - Acquire DMA resource for SPI master
 *  @param[in] MasterID Indicate a SPI master
 *  @param[out] pTxDmaChanNo Allocated DMA channel for MOSI
 *  @param[out] pRxDmaChanNo Allocated DMA channel for MISO
 *  @return error code
 */
static UINT32 SPI_MasterSetDmaChan(UINT32 MasterID, UINT32 *pTxDmaChanNo, UINT32 *pRxDmaChanNo)
{
    static const UINT32 SpiMstDmaType[AMBA_NUM_SPI_MASTER][2] = {
        [AMBA_SPI_MASTER0] = { [0] = AMBA_DMA_CHANNEL_SPI0_TX, [1] = AMBA_DMA_CHANNEL_SPI0_RX },
        [AMBA_SPI_MASTER1] = { [0] = AMBA_DMA_CHANNEL_SPI1_TX, [1] = AMBA_DMA_CHANNEL_SPI1_RX },
        [AMBA_SPI_MASTER2] = { [0] = AMBA_DMA_CHANNEL_SPI2_TX, [1] = AMBA_DMA_CHANNEL_SPI2_RX },
        [AMBA_SPI_MASTER3] = { [0] = AMBA_DMA_CHANNEL_SPI3_TX, [1] = AMBA_DMA_CHANNEL_SPI3_RX },
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS) || defined(CONFIG_SOC_CV5)
        [AMBA_SPI_MASTER4] = { [0] = AMBA_DMA_CHANNEL_SPI4_TX, [1] = AMBA_DMA_CHANNEL_SPI4_RX },
        [AMBA_SPI_MASTER5] = { [0] = AMBA_DMA_CHANNEL_SPI5_TX, [1] = AMBA_DMA_CHANNEL_SPI5_RX },
#endif
    };
    UINT32 RetVal = SPI_ERR_NONE;

    if (MasterID >= AMBA_NUM_SPI_MASTER) {
        RetVal = SPI_ERR_ARG;
    } else {
        if (pTxDmaChanNo != NULL) {
            if (AmbaDMA_ChannelAllocate(SpiMstDmaType[MasterID][0], pTxDmaChanNo) != DMA_ERR_NONE) {
                RetVal = SPI_ERR_UNEXPECTED;
            } else {
                if (*pTxDmaChanNo >= AMBA_NUM_DMA_CHANNEL) {
                    RetVal = SPI_ERR_UNAVAIL;
                }
            }
        }

        if ((pRxDmaChanNo != NULL) && (RetVal == SPI_ERR_NONE)) {
            if (AmbaDMA_ChannelAllocate(SpiMstDmaType[MasterID][1], pRxDmaChanNo) != DMA_ERR_NONE) {
                RetVal = SPI_ERR_UNEXPECTED;
            } else {
                if (*pRxDmaChanNo >= AMBA_NUM_DMA_CHANNEL) {
                    RetVal = SPI_ERR_UNAVAIL;
                }
            }

            /* if Rx dma channel is failed to allocate, release the allocated tx dma channel */
            if ((RetVal != SPI_ERR_NONE) && (pTxDmaChanNo != NULL)) {
                if (AmbaDMA_ChannelRelease(*pTxDmaChanNo) != DMA_ERR_NONE) {
                    RetVal = SPI_ERR_UNEXPECTED;
                }
            }
        }
    }

    return RetVal;  /* Could be either DMA or SPI error code */
}

/**
 *  SPI_MasterSetDmaBit08 - Set SPI master transfer with DMA
 *  @param[in] MasterID Indicate a SPI master
 *  @return error code
 */
static UINT32 SPI_MasterSetDmaBit08(UINT32 MasterID, UINT32 TxDmaChanNo, UINT32 RxDmaChanNo, UINT32 BufSize, const UINT8 *pTxBuf8, UINT8 *pRxBuf8)
{
    UINT32 RetVal = SPI_ERR_NONE;

    if (pTxBuf8 != NULL) {
        RetVal = SPI_DmaSetup(MasterID, TxDmaChanNo, 1U, BufSize, pTxBuf8, NULL);
    }
    if ((pRxBuf8 != NULL) && (RetVal == SPI_ERR_NONE)) {
        RetVal = SPI_DmaSetup(MasterID, RxDmaChanNo, 1U, BufSize, NULL, pRxBuf8);
    }
    if (RetVal == SPI_ERR_NONE) {
        RetVal = AmbaRTSL_SpiMasterDmaTranD8(MasterID, BufSize, pTxBuf8, pRxBuf8);
    }

    return RetVal;
}

/**
 *  SPI_MasterSetDmaBit16 - Set SPI master transfer with DMA
 *  @param[in] MasterID Indicate a SPI master
 *  @return error code
 */
static UINT32 SPI_MasterSetDmaBit16(UINT32 MasterID, UINT32 TxDmaChanNo, UINT32 RxDmaChanNo, UINT32 BufSize, const UINT16 *pTxBuf16, UINT16 *pRxBuf16)
{
    UINT32 RetVal = SPI_ERR_NONE;

    if (pTxBuf16 != NULL) {
        RetVal = SPI_DmaSetup(MasterID, TxDmaChanNo, 2U, BufSize, pTxBuf16, NULL);
    }
    if ((pRxBuf16 != NULL) && (RetVal == SPI_ERR_NONE)) {
        RetVal = SPI_DmaSetup(MasterID, RxDmaChanNo, 2U, BufSize, NULL, pRxBuf16);
    }
    if (RetVal == SPI_ERR_NONE) {
        RetVal = AmbaRTSL_SpiMasterDmaTranD16(MasterID, BufSize, pTxBuf16, pRxBuf16);
    }

    return RetVal;
}

/**
 *  SPI_MasterWaitDmaDone - Wait SPI master transfer done with DMA
 *  @param[in] MasterID Indicate a SPI master
 *  @return error code
 */
static UINT32 SPI_MasterWaitDmaDone(UINT32 DmaChanNo, UINT32 TimeOut)
{
    UINT32 DmaRetVal, RetVal = SPI_ERR_NONE;

    DmaRetVal = AmbaDMA_Wait(DmaChanNo, TimeOut);
    if (DmaRetVal != DMA_ERR_NONE) {
        if (DmaRetVal == DMA_ERR_TMO) {
            RetVal = SPI_ERR_TMO;
        }

        if (AmbaRTSL_DmaStop(DmaChanNo) != DMA_ERR_NONE) {
            RetVal = SPI_ERR_UNEXPECTED;
        }
    }

    return RetVal;
}

/**
 *  SPI_MasterStartDma - Start SPI master transfer with DMA
 *  @param[in] MasterID Indicate a SPI master
 *  @param[out] pTxDmaChanNo Allocated DMA channel for MOSI
 *  @param[out] pRxDmaChanNo Allocated DMA channel for MISO
 *  @return error code
 */
static UINT32 SPI_MasterStartDma(UINT32 MasterID, UINT32 TxDmaChanNo, UINT32 RxDmaChanNo, UINT32 BufSize, const UINT8 *pTxBuf8, UINT8 *pRxBuf8, const UINT16 *pTxBuf16, UINT16 *pRxBuf16, UINT32 TimeOut)
{
    UINT32 RetVal = SPI_ERR_NONE;

    /* SPI Tx/Rx with DMA */
    if ((pTxBuf8 != NULL) || (pRxBuf8 != NULL)) {
        RetVal = SPI_MasterSetDmaBit08(MasterID, TxDmaChanNo, RxDmaChanNo, BufSize, pTxBuf8, pRxBuf8);
    }
    if ((pTxBuf16 != NULL) || (pRxBuf16 != NULL)) {
        RetVal = SPI_MasterSetDmaBit16(MasterID, TxDmaChanNo, RxDmaChanNo, BufSize, pTxBuf16, pRxBuf16);
    }

    /* wait DMA done */
    if (TxDmaChanNo < AMBA_NUM_DMA_CHANNEL) {
        RetVal = SPI_MasterWaitDmaDone(TxDmaChanNo, TimeOut);
    }
    if (RxDmaChanNo < AMBA_NUM_DMA_CHANNEL) {
        RetVal = SPI_MasterWaitDmaDone(RxDmaChanNo, TimeOut);
    }

    return RetVal;
}

/**
 *  SPI_SlaveDmaSetup - Configure DMA for SPI slave
 *  @param[in] SlaveID Indicate a SPI slave
 *  @param[in] DmaChan Allocated DMA channel for either TX or RX
 *  @param[in] DataWidth Bit width of single spi data
 *  @param[in] DataSize The size of TX and RX buffer if exist
 *  @param[in] pTxDataBuf TX buffer. If this pointer is NULL, the signal on SDO/MOSI pin will be ignored.
 *  @param[in] pRxDataBuf RX buffer. If this pointer is NULL, the signal on SDI/MISO pin will be ignored.
 *  @return error code
 */
static UINT32 SPI_SlaveDmaSetup(UINT32 SlaveID, UINT32 DmaChan, UINT32 DataWidth, UINT32 DataSize, const void *pTxDataBuf, const void *pRxDataBuf)
{
    AMBA_DMA_DESC_s SpiDmaDesc;
    ULONG DataPortAddr, DataBufAddr;
    void *pSrcAddr = NULL, *pDstAddr = NULL;

    if (DataWidth == 1U) {
        SpiDmaDesc.DataSize = DataSize;
        SpiDmaDesc.Ctrl.BusDataSize = DMA_BUS_DATA_1BYTE;
    } else {
        SpiDmaDesc.DataSize = DataSize * 2U;
        SpiDmaDesc.Ctrl.BusDataSize = DMA_BUS_DATA_2BYTE;
    }

    /* Fill DMA control data */
    SpiDmaDesc.Ctrl.StopOnError = 1U;
    SpiDmaDesc.Ctrl.IrqOnError = 0U;
    SpiDmaDesc.Ctrl.IrqOnDone = 0U;
    SpiDmaDesc.Ctrl.BusBlockSize = DMA_BUS_BLOCK_8BYTE;
    SpiDmaDesc.Ctrl.NoBusAddrInc = 1U;
    SpiDmaDesc.Ctrl.EndOfChain = 1U;

    AmbaRTSL_SpiSlaveGetPortAddr(SlaveID, &DataPortAddr);
    if (pTxDataBuf != NULL) {
        /* Flush cache */
        AmbaMisra_TypeCast(&DataBufAddr, &pTxDataBuf);
        (void)AmbaCache_DataFlush(DataBufAddr, SpiDmaDesc.DataSize);

        AmbaMisra_TypeCast(&pSrcAddr, &pTxDataBuf);
        AmbaMisra_TypeCast(&pDstAddr, &DataPortAddr);
        SpiDmaDesc.Ctrl.ReadMem = 1U;
        SpiDmaDesc.Ctrl.WriteMem = 0U;
    }
    if (pRxDataBuf != NULL) {
        /* Flush cache */
        AmbaMisra_TypeCast(&DataBufAddr, &pRxDataBuf);
        (void)AmbaCache_DataFlush(DataBufAddr, SpiDmaDesc.DataSize);

        AmbaMisra_TypeCast(&pSrcAddr, &DataPortAddr);
        AmbaMisra_TypeCast(&pDstAddr, &pRxDataBuf);
        SpiDmaDesc.Ctrl.ReadMem = 0U;
        SpiDmaDesc.Ctrl.WriteMem = 1U;
    }

    SpiDmaDesc.pSrcAddr = pSrcAddr;
    SpiDmaDesc.pDstAddr = pDstAddr;
    SpiDmaDesc.pNextDesc = NULL;
    SpiDmaDesc.pStatus = NULL;

    /* start DMA data transfer */
    return AmbaDMA_Transfer(DmaChan, &SpiDmaDesc);
}

/**
 *  SPI_SlaveSetDmaChan - Acquire DMA resource for SPI master
 *  @param[in] SlaveID Indicate a SPI slave
 *  @param[out] pTxDmaChanNo Allocated DMA channel for MOSI
 *  @param[out] pRxDmaChanNo Allocated DMA channel for MISO
 *  @return error code
 */
static UINT32 SPI_SlaveSetDmaChan(UINT32 SlaveID, UINT32 *pTxDmaChanNo, UINT32 *pRxDmaChanNo)
{
    static const UINT32 SpiMstDmaType[AMBA_NUM_SPI_SLAVE][2] = {
        [AMBA_SPI_SLAVE] = { [0] = AMBA_DMA_CHANNEL_SPI_SLAVE_TX, [1] = AMBA_DMA_CHANNEL_SPI_SLAVE_RX },
    };
    UINT32 RetVal = SPI_ERR_NONE;

    if (SlaveID >= AMBA_NUM_SPI_SLAVE) {
        RetVal = SPI_ERR_ARG;
    } else {
        if (pTxDmaChanNo != NULL) {
            if (AmbaDMA_ChannelAllocate(SpiMstDmaType[SlaveID][0], pTxDmaChanNo) != DMA_ERR_NONE) {
                RetVal = SPI_ERR_UNEXPECTED;
            } else {
                if (*pTxDmaChanNo >= AMBA_NUM_DMA_CHANNEL) {
                    RetVal = SPI_ERR_UNAVAIL;
                }
            }
        }

        if ((pRxDmaChanNo != NULL) && (RetVal == SPI_ERR_NONE)) {
            if (AmbaDMA_ChannelAllocate(SpiMstDmaType[SlaveID][1], pRxDmaChanNo) != DMA_ERR_NONE) {
                RetVal = SPI_ERR_UNEXPECTED;
            } else {
                if (*pRxDmaChanNo >= AMBA_NUM_DMA_CHANNEL) {
                    RetVal = SPI_ERR_UNAVAIL;
                }
            }

            /* if Rx dma channel is failed to allocate, release the allocated tx dma channel */
            if ((RetVal != SPI_ERR_NONE) && (pTxDmaChanNo != NULL)) {
                if (AmbaDMA_ChannelRelease(*pTxDmaChanNo) != DMA_ERR_NONE) {
                    RetVal = SPI_ERR_UNEXPECTED;
                }
            }
        }
    }

    return RetVal;  /* Could be either DMA or SPI error code */
}

/**
 *  SPI_SlaveSetDmaBit08 - Set SPI slave transfer with DMA
 *  @param[in] MasterID Indicate a SPI master
 *  @return error code
 */
static UINT32 SPI_SlaveSetDmaBit08(UINT32 SlaveID, UINT32 TxDmaChanNo, UINT32 RxDmaChanNo, UINT32 BufSize, const UINT8 *pTxBuf8, UINT8 *pRxBuf8)
{
    UINT32 DmaDataSize, RetVal = SPI_ERR_NONE;

    if (pTxBuf8 != NULL) {
        RetVal = SPI_SlaveDmaSetup(SlaveID, TxDmaChanNo, 1U, BufSize, pTxBuf8, NULL);
    }
    if ((pRxBuf8 != NULL) && (RetVal == SPI_ERR_NONE)) {
        /* DMA alignment restriction is 8 byte, and each data takes 1 byte. Aligned to 8 */
        DmaDataSize = ((BufSize >> 3U) << 3U);

        RetVal = SPI_SlaveDmaSetup(SlaveID, RxDmaChanNo, 1U, DmaDataSize, NULL, pRxBuf8);
    }
    if (RetVal == SPI_ERR_NONE) {
        RetVal = AmbaRTSL_SpiSlaveDmaTranD8(SlaveID, BufSize, pTxBuf8, pRxBuf8);
    }

    return RetVal;
}

/**
 *  SPI_SlaveSetDmaBit16 - Set SPI slave transfer with DMA
 *  @param[in] MasterID Indicate a SPI master
 *  @return error code
 */
static UINT32 SPI_SlaveSetDmaBit16(UINT32 SlaveID, UINT32 TxDmaChanNo, UINT32 RxDmaChanNo, UINT32 BufSize, const UINT16 *pTxBuf16, UINT16 *pRxBuf16)
{
    UINT32 DmaDataSize, RetVal = SPI_ERR_NONE;

    if (pTxBuf16 != NULL) {
        RetVal = SPI_SlaveDmaSetup(SlaveID, TxDmaChanNo, 2U, BufSize, pTxBuf16, NULL);
    }
    if ((pRxBuf16 != NULL) && (RetVal == SPI_ERR_NONE)) {
        /* DMA alignment restriction is 8 byte, and each data takes 2 byte. Aligned to 4 */
        DmaDataSize = ((BufSize >> 2U) << 2U);

        RetVal = SPI_SlaveDmaSetup(SlaveID, RxDmaChanNo, 2U, DmaDataSize, NULL, pRxBuf16);
    }
    if (RetVal == SPI_ERR_NONE) {
        RetVal = AmbaRTSL_SpiSlaveDmaTranD16(SlaveID, BufSize, pTxBuf16, pRxBuf16);
    }

    return RetVal;
}

/**
 *  SPI_SlaveWaitDmaDone - Wait SPI slave transfer done with DMA
 *  @param[in] MasterID Indicate a SPI master
 *  @return error code
 */
static UINT32 SPI_SlaveWaitDmaDone(UINT32 DmaChanNo, UINT32 TimeOut)
{
    UINT32 DmaRetVal, RetVal = SPI_ERR_NONE;

    DmaRetVal = AmbaDMA_Wait(DmaChanNo, TimeOut);
    if (DmaRetVal != DMA_ERR_NONE) {
        if (DmaRetVal == DMA_ERR_TMO) {
            RetVal = SPI_ERR_TMO;
        }

        if (AmbaRTSL_DmaStop(DmaChanNo) != DMA_ERR_NONE) {
            RetVal = SPI_ERR_UNEXPECTED;
        }
    }

    return RetVal;
}

/**
 *  SPI_SlaveStartDma - Start SPI master transfer with DMA
 *  @param[in] SlaveID Indicate a SPI slave
 *  @param[out] pTxDmaChanNo Allocated DMA channel for MOSI
 *  @param[out] pRxDmaChanNo Allocated DMA channel for MISO
 *  @return error code
 */
static UINT32 SPI_SlaveStartDma(UINT32 SlaveID, UINT32 TxDmaChanNo, UINT32 RxDmaChanNo, UINT32 BufSize, const UINT8 *pTxBuf8, UINT8 *pRxBuf8, const UINT16 *pTxBuf16, UINT16 *pRxBuf16, UINT32 TimeOut)
{
    UINT32 RetVal = SPI_ERR_NONE;

    /* SPI Tx/Rx with DMA */
    if ((pTxBuf8 != NULL) || (pRxBuf8 != NULL)) {
        RetVal = SPI_SlaveSetDmaBit08(SlaveID, TxDmaChanNo, RxDmaChanNo, BufSize, pTxBuf8, pRxBuf8);
    }
    if ((pTxBuf16 != NULL) || (pRxBuf16 != NULL)) {
        RetVal = SPI_SlaveSetDmaBit16(SlaveID, TxDmaChanNo, RxDmaChanNo, BufSize, pTxBuf16, pRxBuf16);
    }

    /* wait DMA done */
    if (TxDmaChanNo < AMBA_NUM_DMA_CHANNEL) {
        RetVal = SPI_SlaveWaitDmaDone(TxDmaChanNo, TimeOut);
    }
    if (RxDmaChanNo < AMBA_NUM_DMA_CHANNEL) {
        RetVal = SPI_SlaveWaitDmaDone(RxDmaChanNo, TimeOut);
    }

    return RetVal;
}

/**
 *  SPI_SlaveDoTransferD8 - SPI Slave write and read data operation with Dma
 *  @param[in] SlaveID Indicate a SPI slave
 *  @param[in] BufSize The size of TX and RX buffer if exist
 *  @param[in] pTxBuf TX buffer. If this pointer is NULL, the signal on SDO/MOSI pin will be ignored.
 *  @param[out] pRxBuf RX buffer. If this pointer is NULL, the signal on SDI/MISO pin will be ignored.
 *  @param[out] pActualSize The actual number of the TX/RX frames
 *  @param[in] TimeOut The timeout value in system tick (ms)
 *  @return error code
 */
static UINT32 SPI_SlaveDoTransferD8(UINT32 SlaveID, UINT32 BufSize, const UINT8 *pTxBuf, UINT8 *pRxBuf, UINT32 *pActualSize, UINT32 TimeOut)
{
    UINT32 RetVal = SPI_ERR_NONE;
    UINT32 TxDmaChanNo = 0xffffffffU, RxDmaChanNo = 0xffffffffU;
    UINT32 SpiStatus = SPI_STATUS_IDLE;
    UINT32 i, RemainSize = 0U, DmaDataSize = 0U;
    UINT32 RemainTransfer = 0U;

    if (pTxBuf == NULL) {
        RetVal = SPI_SlaveSetDmaChan(SlaveID, NULL, &RxDmaChanNo);
    } else if (pRxBuf == NULL) {
        RetVal = SPI_SlaveSetDmaChan(SlaveID, &TxDmaChanNo, NULL);
    } else {
        RetVal = SPI_SlaveSetDmaChan(SlaveID, &TxDmaChanNo, &RxDmaChanNo);
    }

    if (RetVal == SPI_ERR_NONE) {
        RetVal = SPI_SlaveStartDma(SlaveID, TxDmaChanNo, RxDmaChanNo, BufSize, pTxBuf, pRxBuf, NULL, NULL, TimeOut);
    }

    /* Handling remaining RX data that not aligned to DMA alignment restriction (8 byte) */
    if (pRxBuf != NULL) {
        DmaDataSize = (BufSize >> 3U) << 3U;
        RemainSize = BufSize - DmaDataSize;

        for (i = 0U; i < 0xfffU; i++) {
            if (AmbaRTSL_SpiSlaveGetReadSize(SlaveID) != 0U) {
                pRxBuf[DmaDataSize + RemainTransfer] = (UINT8)AmbaRTSL_SpiSlaveReadFifo(SlaveID);
                RemainTransfer ++;
            }

            if (RemainTransfer == RemainSize) {
                break;
            }
        }
    }

    if (RetVal == SPI_ERR_NONE) {
        do {
            RetVal = AmbaRTSL_SpiSlaveGetInfo(SlaveID, &SpiStatus, NULL);
        } while ((RetVal == SPI_ERR_NONE) && (SpiStatus == SPI_STATUS_BUSY));

        AmbaRTSL_SpiSlaveStop(SlaveID);

        if ((RetVal == SPI_ERR_NONE) && (pActualSize != NULL)) {
            if (pRxBuf != NULL) {
                *pActualSize = RemainTransfer + DmaDataSize;
            } else {
                *pActualSize = BufSize;
            }
        }
    }

    return RetVal;
}

/**
 *  SPI_SlaveDoTransferD16 - SPI Slave write and read data operation with Dma
 *  @param[in] SlaveID Indicate a SPI slave
 *  @param[in] BufSize The size of TX and RX buffer if exist
 *  @param[in] pTxBuf TX buffer. If this pointer is NULL, the signal on SDO/MOSI pin will be ignored.
 *  @param[out] pRxBuf RX buffer. If this pointer is NULL, the signal on SDI/MISO pin will be ignored.
 *  @param[out] pActualSize The actual number of the TX/RX frames
 *  @param[in] TimeOut The timeout value in system tick (ms)
 *  @return error code
 */
static UINT32 SPI_SlaveDoTransferD16(UINT32 SlaveID, UINT32 BufSize, const UINT16 *pTxBuf, UINT16 *pRxBuf, UINT32 *pActualSize, UINT32 TimeOut)
{
    UINT32 RetVal = SPI_ERR_NONE;
    UINT32 TxDmaChanNo = 0xffffffffU, RxDmaChanNo = 0xffffffffU;
    UINT32 SpiStatus = SPI_STATUS_IDLE;
    UINT32 i, RemainSize = 0U, DmaDataSize = 0U;
    UINT32 RemainTransfer = 0U;

    if (pTxBuf == NULL) {
        RetVal = SPI_SlaveSetDmaChan(SlaveID, NULL, &RxDmaChanNo);
    } else if (pRxBuf == NULL) {
        RetVal = SPI_SlaveSetDmaChan(SlaveID, &TxDmaChanNo, NULL);
    } else {
        RetVal = SPI_SlaveSetDmaChan(SlaveID, &TxDmaChanNo, &RxDmaChanNo);
    }

    if (RetVal == SPI_ERR_NONE) {
        RetVal = SPI_SlaveStartDma(SlaveID, TxDmaChanNo, RxDmaChanNo, BufSize, NULL, NULL, pTxBuf, pRxBuf, TimeOut);
    }

    /* Handling remaining RX data that not aligned to DMA alignment restriction (8 byte) */
    if (pRxBuf != NULL) {
        DmaDataSize = (BufSize >> 2U) << 2U;
        RemainSize = BufSize - DmaDataSize;

        for (i = 0U; i < 0xfffU; i++) {
            if (AmbaRTSL_SpiSlaveGetReadSize(SlaveID) != 0U) {
                pRxBuf[DmaDataSize + RemainTransfer] = (UINT16)AmbaRTSL_SpiSlaveReadFifo(SlaveID);
                RemainTransfer ++;
            }

            if (RemainTransfer == RemainSize) {
                break;
            }
        }
    }

    if (RetVal == SPI_ERR_NONE) {
        do {
            RetVal = AmbaRTSL_SpiSlaveGetInfo(SlaveID, &SpiStatus, NULL);
        } while ((RetVal == SPI_ERR_NONE) && (SpiStatus == SPI_STATUS_BUSY));

        AmbaRTSL_SpiSlaveStop(SlaveID);

        if ((RetVal == SPI_ERR_NONE) && (pActualSize != NULL)) {
            if (pRxBuf != NULL) {
                *pActualSize = RemainTransfer + DmaDataSize;
            } else {
                *pActualSize = BufSize;
            }
        }
    }

    return RetVal;
}
/**
 *  AmbaSPI_DrvEntry - SPI device driver initializations
 *  @return error code
 *  @note this function is an internal using only API
 */
UINT32 AmbaSPI_DrvEntry(void)
{
    static char AmbaSpiMstEventFlagName[24] = "AmbaSpiMasterEventFlags";
    static char AmbaSpiSlvEventFlagName[24] = "AmbaSpiSlaveEventFlags";
    static char AmbaSpiMstMutexName[20] = "AmbaSpiMstMutex";
    static char AmbaSpiSlvMutexName[20] = "AmbaSpiSlvMutex";
    UINT32 i, RetVal = SPI_ERR_NONE;

    if (AmbaKAL_EventFlagCreate(&AmbaSpiMstEventFlag, AmbaSpiMstEventFlagName) != KAL_ERR_NONE) {
        RetVal = SPI_ERR_UNEXPECTED;
    }

    if (RetVal == SPI_ERR_NONE) {
        if (AmbaKAL_EventFlagCreate(&AmbaSpiSlvEventFlag, AmbaSpiSlvEventFlagName) != KAL_ERR_NONE) {
            RetVal = SPI_ERR_UNEXPECTED;
        }
    }

    if (RetVal == SPI_ERR_NONE) {
        for (i = 0U; i < AMBA_NUM_SPI_MASTER; i++) {
            if (AmbaKAL_MutexCreate(&AmbaSpiMstMutex[i], AmbaSpiMstMutexName) != KAL_ERR_NONE) {
                RetVal = SPI_ERR_UNEXPECTED;
                break;
            }
        }
    }

    if (RetVal == SPI_ERR_NONE) {
        for (i = 0U; i < AMBA_NUM_SPI_SLAVE; i++) {
            if (AmbaKAL_MutexCreate(&AmbaSpiSlvMutex[i], AmbaSpiSlvMutexName) != KAL_ERR_NONE) {
                RetVal = SPI_ERR_UNEXPECTED;
            }
        }
    }

    if (RetVal == SPI_ERR_NONE) {
        RetVal = AmbaRTSL_SpiInit(SPI_MasterIntHandler, SPI_SlaveIntHandler);
    }

    return RetVal;
}

/**
 *  AmbaSPI_MasterTransferD8 - SPI Master write and read data operation
 *  @param[in] MasterID Indicate a SPI master
 *  @param[in] SlaveMask Indicate active slave devices of the master
 *  @param[in] pConfig Define the combinations of polarity and phases for transitions where data should be latched.
 *  @param[in] BufSize The size of TX and RX buffer if exist
 *  @param[in] pTxBuf TX buffer. If this pointer is NULL, the signal on SDO/MOSI pin will be ignored.
 *  @param[out] pRxBuf RX buffer. If this pointer is NULL, the signal on SDI/MISO pin will be ignored.
 *  @param[out] pActualSize The actual number of the TX/RX frames
 *  @param[in] TimeOut: The timeout value in system tick (ms)
 *  @return error code
 */
UINT32 AmbaSPI_MasterTransferD8(UINT32 MasterID, UINT32 SlaveMask, const AMBA_SPI_CONFIG_s *pConfig,
                                UINT32 BufSize, const UINT8 *pTxBuf, UINT8 *pRxBuf, UINT32 *pActualSize, UINT32 TimeOut)
{
    UINT32 KalRetVal, RetVal = SPI_ERR_NONE;
    UINT32 ActualFlags;

    if ((MasterID >= AMBA_NUM_SPI_MASTER) || (pConfig == NULL) || (BufSize == 0U) || ((pTxBuf == NULL) && (pRxBuf == NULL))) {
        RetVal = SPI_ERR_ARG;
    } else {
        if (AmbaKAL_MutexTake(&AmbaSpiMstMutex[MasterID], KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = SPI_ERR_MUTEX;
        } else {
            if (AmbaKAL_EventFlagClear(&AmbaSpiMstEventFlag, (UINT32)1U << MasterID) != KAL_ERR_NONE) {
                RetVal = SPI_ERR_UNEXPECTED;
            } else {
                AmbaSpiMasterIntHandlerErr[MasterID] = SPI_ERR_NONE;    /* clear error status reported by interrupt handler */
                RetVal = AmbaRTSL_SpiMasterConfig(MasterID, SlaveMask, pConfig);
                if (RetVal == SPI_ERR_NONE) {
                    RetVal = AmbaRTSL_SpiMasterTransferD8(MasterID, BufSize, pTxBuf, pRxBuf);
                    if (RetVal == SPI_ERR_NONE) {
                        /* wait for SPI IRQ Event Flag ! */
                        KalRetVal = AmbaKAL_EventFlagGet(&AmbaSpiMstEventFlag, (UINT32)1U << MasterID, KAL_FLAGS_WAIT_ALL, KAL_FLAGS_CLEAR_AUTO, &ActualFlags, TimeOut);
                        if (KalRetVal != KAL_ERR_NONE) {
                            if (KalRetVal == KAL_ERR_TIMEOUT) {
                                RetVal = SPI_ERR_TMO;
                            } else {
                                RetVal = SPI_ERR_UNEXPECTED;
                            }
                        }

                        AmbaRTSL_SpiMasterStop(MasterID);

                        if ((RetVal == SPI_ERR_NONE) && (pActualSize != NULL)) {
                            RetVal = AmbaRTSL_SpiMasterGetInfo(MasterID, NULL, NULL, pActualSize);
                        }
                    }
                }
                if (AmbaSpiMasterIntHandlerErr[MasterID] != SPI_ERR_NONE) { /* check if any error reported in interrupt handler */
                    RetVal =  AmbaSpiMasterIntHandlerErr[MasterID];
                }
            }

            if (AmbaKAL_MutexGive(&AmbaSpiMstMutex[MasterID]) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = SPI_ERR_UNEXPECTED;
            }
        }
    }

    return RetVal;
}

/**
 *  AmbaSPI_MasterTransferD16 - SPI Master write and read data operation
 *  @param[in] MasterID Indicate a SPI master
 *  @param[in] SlaveMask Indicate active slave devices of the master
 *  @param[in] pConfig Define the combinations of polarity and phases for transitions where data should be latched.
 *  @param[in] BufSize The size of TX and RX buffer if exist
 *  @param[in] pTxBuf TX buffer. If this pointer is NULL, the signal on SDO/MOSI pin will be ignored.
 *  @param[out] pRxBuf RX buffer. If this pointer is NULL, the signal on SDI/MISO pin will be ignored.
 *  @param[out] pActualSize The actual number of the TX/RX frames
 *  @param[in] TimeOut: The timeout value in system tick (ms)
 *  @return error code
 */
UINT32 AmbaSPI_MasterTransferD16(UINT32 MasterID, UINT32 SlaveMask, const AMBA_SPI_CONFIG_s *pConfig,
                                 UINT32 BufSize, const UINT16 *pTxBuf, UINT16 *pRxBuf, UINT32 *pActualSize, UINT32 TimeOut)
{
    UINT32 KalRetVal, RetVal = SPI_ERR_NONE;
    UINT32 ActualFlags;

    if ((MasterID >= AMBA_NUM_SPI_MASTER) || (pConfig == NULL) || (BufSize == 0U) || ((pTxBuf == NULL) && (pRxBuf == NULL))) {
        RetVal = SPI_ERR_ARG;
    } else {
        if (AmbaKAL_MutexTake(&AmbaSpiMstMutex[MasterID], KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = SPI_ERR_MUTEX;
        } else {
            if (AmbaKAL_EventFlagClear(&AmbaSpiMstEventFlag, (UINT32)1U << MasterID) != KAL_ERR_NONE) {
                RetVal = SPI_ERR_UNEXPECTED;
            } else {
                AmbaSpiMasterIntHandlerErr[MasterID] = SPI_ERR_NONE;    /* clear error status reported by interrupt handler */
                RetVal = AmbaRTSL_SpiMasterConfig(MasterID, SlaveMask, pConfig);
                if (RetVal == SPI_ERR_NONE) {
                    RetVal = AmbaRTSL_SpiMasterTransferD16(MasterID, BufSize, pTxBuf, pRxBuf);
                    if (RetVal == SPI_ERR_NONE) {
                        /* wait for SPI IRQ Event Flag ! */
                        KalRetVal = AmbaKAL_EventFlagGet(&AmbaSpiMstEventFlag, (UINT32)1U << MasterID, KAL_FLAGS_WAIT_ALL, KAL_FLAGS_CLEAR_AUTO, &ActualFlags, TimeOut);
                        if (KalRetVal != KAL_ERR_NONE) {
                            if (KalRetVal == KAL_ERR_TIMEOUT) {
                                RetVal = SPI_ERR_TMO;
                            } else {
                                RetVal = SPI_ERR_UNEXPECTED;
                            }
                        }

                        AmbaRTSL_SpiMasterStop(MasterID);

                        if ((RetVal == SPI_ERR_NONE) && (pActualSize != NULL)) {
                            RetVal = AmbaRTSL_SpiMasterGetInfo(MasterID, NULL, NULL, pActualSize);
                        }
                    }
                }
                if (AmbaSpiMasterIntHandlerErr[MasterID] != SPI_ERR_NONE) { /* check if any error reported in interrupt handler */
                    RetVal =  AmbaSpiMasterIntHandlerErr[MasterID];
                }
            }

            if (AmbaKAL_MutexGive(&AmbaSpiMstMutex[MasterID]) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = SPI_ERR_UNEXPECTED;
            }
        }
    }

    return RetVal;
}

/**
 *  AmbaSPI_MasterDmaTransferD8 - SPI Master write and read data operation with Dma
 *  @param[in] MasterID Indicate a SPI master
 *  @param[in] SlaveMask Indicate active slave devices of the master
 *  @param[in] pConfig Define the combinations of polarity and phases for transitions where data should be latched.
 *  @param[in] BufSize The size of TX and RX buffer if exist
 *  @param[in] pTxBuf TX buffer. If this pointer is NULL, the signal on SDO/MOSI pin will be ignored.
 *  @param[out] pRxBuf RX buffer. If this pointer is NULL, the signal on SDI/MISO pin will be ignored.
 *  @param[out] pActualSize The actual number of the TX/RX frames
 *  @param[in] TimeOut The timeout value in system tick (ms)
 *  @return error code
 */
UINT32 AmbaSPI_MasterDmaTransferD8(UINT32 MasterID, UINT32 SlaveMask, const AMBA_SPI_CONFIG_s *pConfig,
                                   UINT32 BufSize, const UINT8 *pTxBuf, UINT8 *pRxBuf, UINT32 *pActualSize, UINT32 TimeOut)
{
    UINT32 RetVal = SPI_ERR_NONE;
    UINT32 TxDmaChanNo = 0xffffffffU, RxDmaChanNo = 0xffffffffU;
    UINT32 SpiStatus = SPI_STATUS_IDLE;

    if ((MasterID >= AMBA_NUM_SPI_MASTER) || (pConfig == NULL) || (BufSize == 0U) || ((pTxBuf == NULL) && (pRxBuf == NULL))) {
        RetVal = SPI_ERR_ARG;
    } else {
        if (AmbaKAL_MutexTake(&AmbaSpiMstMutex[MasterID], KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = SPI_ERR_MUTEX;
        } else if (AmbaKAL_EventFlagClear(&AmbaSpiMstEventFlag, ((UINT32)1U << MasterID)) != KAL_ERR_NONE) {
            RetVal = SPI_ERR_UNEXPECTED;
        } else {
            if (pTxBuf == NULL) {
                RetVal = SPI_MasterSetDmaChan(MasterID, NULL, &RxDmaChanNo);
            } else if (pRxBuf == NULL) {
                RetVal = SPI_MasterSetDmaChan(MasterID, &TxDmaChanNo, NULL);
            } else {
                RetVal = SPI_MasterSetDmaChan(MasterID, &TxDmaChanNo, &RxDmaChanNo);
            }

            if (RetVal == SPI_ERR_NONE) {
                RetVal = AmbaRTSL_SpiMasterConfig(MasterID, SlaveMask, pConfig);
                if (RetVal == SPI_ERR_NONE) {
                    RetVal = SPI_MasterStartDma(MasterID, TxDmaChanNo, RxDmaChanNo, BufSize, pTxBuf, pRxBuf, NULL, NULL, TimeOut);
                }
            }

            if (RetVal == SPI_ERR_NONE) {
                do {
                    RetVal = AmbaRTSL_SpiMasterGetInfo(MasterID, NULL, &SpiStatus, NULL);
                } while ((RetVal == SPI_ERR_NONE) && (SpiStatus == SPI_STATUS_BUSY));

                AmbaRTSL_SpiMasterStop(MasterID);

                if ((RetVal == SPI_ERR_NONE) && (pActualSize != NULL)) {
                    *pActualSize = BufSize;
                }
            }

            if (AmbaKAL_MutexGive(&AmbaSpiMstMutex[MasterID]) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = SPI_ERR_UNEXPECTED;
            }
        }
    }

    return RetVal;
}

/**
 *  AmbaSPI_MasterDmaTransferD16 - SPI Master write and read data operation with Dma
 *  @param[in] MasterID Indicate a SPI master
 *  @param[in] SlaveMask Indicate active slave devices of the master
 *  @param[in] pConfig Define the combinations of polarity and phases for transitions where data should be latched.
 *  @param[in] BufSize The size of TX and RX buffer if exist
 *  @param[in] pTxBuf TX buffer. If this pointer is NULL, the signal on SDO/MOSI pin will be ignored.
 *  @param[out] pRxBuf RX buffer. If this pointer is NULL, the signal on SDI/MISO pin will be ignored.
 *  @param[out] pActualSize The actual number of the TX/RX frames
 *  @param[in] TimeOut The timeout value in system tick (ms)
 *  @return error code
 */
UINT32 AmbaSPI_MasterDmaTransferD16(UINT32 MasterID, UINT32 SlaveMask, const AMBA_SPI_CONFIG_s *pConfig,
                                    UINT32 BufSize, const UINT16 *pTxBuf, UINT16 *pRxBuf, UINT32 *pActualSize, UINT32 TimeOut)
{
    UINT32 RetVal = SPI_ERR_NONE;
    UINT32 TxDmaChanNo = 0xffffffffU, RxDmaChanNo = 0xffffffffU;
    UINT32 SpiStatus = SPI_STATUS_IDLE;

    if ((MasterID >= AMBA_NUM_SPI_MASTER) || (pConfig == NULL) || (BufSize == 0U) || ((pTxBuf == NULL) && (pRxBuf == NULL))) {
        RetVal = SPI_ERR_ARG;
    } else {
        if (AmbaKAL_MutexTake(&AmbaSpiMstMutex[MasterID], KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = SPI_ERR_MUTEX;
        } else if (AmbaKAL_EventFlagClear(&AmbaSpiMstEventFlag, ((UINT32)1U << MasterID)) != KAL_ERR_NONE) {
            RetVal = SPI_ERR_UNEXPECTED;
        } else {
            if (pTxBuf == NULL) {
                RetVal = SPI_MasterSetDmaChan(MasterID, NULL, &RxDmaChanNo);
            } else if (pRxBuf == NULL) {
                RetVal = SPI_MasterSetDmaChan(MasterID, &TxDmaChanNo, NULL);
            } else {
                RetVal = SPI_MasterSetDmaChan(MasterID, &TxDmaChanNo, &RxDmaChanNo);
            }

            if (RetVal == SPI_ERR_NONE) {
                RetVal = AmbaRTSL_SpiMasterConfig(MasterID, SlaveMask, pConfig);
                if (RetVal == SPI_ERR_NONE) {
                    RetVal = SPI_MasterStartDma(MasterID, TxDmaChanNo, RxDmaChanNo, BufSize, NULL, NULL, pTxBuf, pRxBuf, TimeOut);
                }
            }

            if (RetVal == SPI_ERR_NONE) {
                do {
                    RetVal = AmbaRTSL_SpiMasterGetInfo(MasterID, NULL, &SpiStatus, NULL);
                } while ((RetVal == SPI_ERR_NONE) && (SpiStatus == SPI_STATUS_BUSY));

                AmbaRTSL_SpiMasterStop(MasterID);

                if ((RetVal == SPI_ERR_NONE) && (pActualSize != NULL)) {
                    *pActualSize = BufSize;
                }
            }

            if (AmbaKAL_MutexGive(&AmbaSpiMstMutex[MasterID]) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = SPI_ERR_UNEXPECTED;
            }
        }
    }

    return RetVal;
}

/**
 *  AmbaSPI_MasterGetInfo - The function returns all the information about the master.
 *  @param[in] MasterID Indicate a SPI master
 *  @param[out] pNumSlaves Number of slave support of the master
 *  @param[out] pStatus The current bus status
 *  @return error code
 */
UINT32 AmbaSPI_MasterGetInfo(UINT32 MasterID, UINT32 *pNumSlaves, UINT32 *pStatus)
{
    UINT32 RetVal = SPI_ERR_NONE;

    if ((MasterID >= AMBA_NUM_SPI_MASTER) || (pNumSlaves == NULL) || (pStatus == NULL)) {
        RetVal = SPI_ERR_ARG;
    } else {
        RetVal = AmbaRTSL_SpiMasterGetInfo(MasterID, pNumSlaves, pStatus, NULL);
    }

    return RetVal;
}

/**
 *  AmbaSPI_SlaveConfig - The function setup the configurations for slave.
 *  @param[in] SlaveID Indicate a SPI slave
 *  @param[in] pConfig Define the combinations of polarity and phases for transitions where data should be latched
 *  @return error code
 */
UINT32 AmbaSPI_SlaveConfig(UINT32 SlaveID, const AMBA_SPI_CONFIG_s * pConfig)
{
    UINT32 RetVal = SPI_ERR_NONE;

    if ((SlaveID >= AMBA_NUM_SPI_SLAVE) || (pConfig == NULL)) {
        RetVal = SPI_ERR_ARG;
    } else {
        if (AmbaKAL_MutexTake(&AmbaSpiSlvMutex[SlaveID], KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = SPI_ERR_MUTEX;
        } else {
            RetVal = AmbaRTSL_SpiSlaveConfig(SlaveID, pConfig);

            if (AmbaKAL_MutexGive(&AmbaSpiSlvMutex[SlaveID]) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = SPI_ERR_UNEXPECTED;
            }
        }
    }

    return RetVal;
}

/**
 *  AmbaSPI_SlaveTransferD8 - SPI Slave write and read data operation
 *  @param[in] SlaveID Indicate a SPI slave
 *  @param[in] BufSize The size of TX and RX buffer if exist
 *  @param[in] pTxBuf TX buffer. If this pointer is NULL, the signal on SDO/MOSI pin will be ignored.
 *  @param[out] pRxBuf RX buffer. If this pointer is NULL, the signal on SDI/MISO pin will be ignored.
 *  @param[out] pActualSize The actual number of the TX/RX frames
 *  @param[in] TimeOut The timeout value in system tick (ms)
 *  @return error code
 */
UINT32 AmbaSPI_SlaveTransferD8(UINT32 SlaveID, UINT32 BufSize, const UINT8 *pTxBuf, UINT8 *pRxBuf, UINT32 *pActualSize, UINT32 TimeOut)
{
    UINT32 KalRetVal, RetVal = SPI_ERR_NONE;
    UINT32 ActualFlags;

    if ((SlaveID >= AMBA_NUM_SPI_SLAVE) || (BufSize == 0U) || ((pTxBuf == NULL) && (pRxBuf == NULL))) {
        RetVal = SPI_ERR_ARG;
    } else {
        if (AmbaKAL_MutexTake(&AmbaSpiSlvMutex[SlaveID], KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = SPI_ERR_MUTEX;
        } else {
            if (AmbaKAL_EventFlagClear(&AmbaSpiSlvEventFlag, (UINT32)1U << SlaveID) != KAL_ERR_NONE) {
                RetVal = SPI_ERR_UNEXPECTED;
            } else {
                AmbaSpiSlaveIntHandlerErr[SlaveID] = SPI_ERR_NONE;    /* clear error status reported by interrupt handler */
                RetVal = AmbaRTSL_SpiSlaveTransferD8(SlaveID, BufSize, pTxBuf, pRxBuf);
                if (RetVal == SPI_ERR_NONE) {
                    /* wait for SPI IRQ Event Flag ! */
                    KalRetVal = AmbaKAL_EventFlagGet(&AmbaSpiSlvEventFlag, ((UINT32)1U << SlaveID), KAL_FLAGS_WAIT_ALL, KAL_FLAGS_CLEAR_AUTO, &ActualFlags, TimeOut);
                    if (KalRetVal != KAL_ERR_NONE) {
                        if (KalRetVal == KAL_ERR_TIMEOUT) {
                            RetVal = SPI_ERR_TMO;
                        } else {
                            RetVal = SPI_ERR_UNEXPECTED;
                        }
                    }

                    AmbaRTSL_SpiSlaveStop(SlaveID);

                    if ((RetVal == SPI_ERR_NONE) && (pActualSize != NULL)) {
                        RetVal = AmbaRTSL_SpiSlaveGetInfo(SlaveID, NULL, pActualSize);
                    }
                }
                if (AmbaSpiSlaveIntHandlerErr[SlaveID] != SPI_ERR_NONE) { /* check if any error reported in interrupt handler */
                    RetVal =  AmbaSpiSlaveIntHandlerErr[SlaveID];
                }
            }

            if (AmbaKAL_MutexGive(&AmbaSpiSlvMutex[SlaveID]) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = SPI_ERR_UNEXPECTED;
            }
        }
    }

    return RetVal;
}

/**
 *  AmbaSPI_SlaveTransferD16 - SPI Slave write and read data operation
 *  @param[in] SlaveID Indicate a SPI slave
 *  @param[in] BufSize The size of TX and RX buffer if exist
 *  @param[in] pTxBuf TX buffer. If this pointer is NULL, the signal on SDO/MOSI pin will be ignored.
 *  @param[out] pRxBuf RX buffer. If this pointer is NULL, the signal on SDI/MISO pin will be ignored.
 *  @param[out] pActualSize The actual number of the TX/RX frames
 *  @param[in] TimeOut The timeout value in system tick (ms)
 *  @return error code
 */
UINT32 AmbaSPI_SlaveTransferD16(UINT32 SlaveID, UINT32 BufSize, const UINT16 *pTxBuf, UINT16 *pRxBuf, UINT32 *pActualSize, UINT32 TimeOut)
{
    UINT32 KalRetVal, RetVal = SPI_ERR_NONE;
    UINT32 ActualFlags;

    if ((SlaveID >= AMBA_NUM_SPI_SLAVE) || (BufSize == 0U) || ((pTxBuf == NULL) && (pRxBuf == NULL))) {
        RetVal = SPI_ERR_ARG;
    } else {
        if (AmbaKAL_MutexTake(&AmbaSpiSlvMutex[SlaveID], KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = SPI_ERR_MUTEX;
        } else {
            if (AmbaKAL_EventFlagClear(&AmbaSpiSlvEventFlag, (UINT32)1U << SlaveID) != KAL_ERR_NONE) {
                RetVal = SPI_ERR_UNEXPECTED;
            } else {
                AmbaSpiSlaveIntHandlerErr[SlaveID] = SPI_ERR_NONE;    /* clear error status reported by interrupt handler */
                RetVal = AmbaRTSL_SpiSlaveTransferD16(SlaveID, BufSize, pTxBuf, pRxBuf);
                if (RetVal == SPI_ERR_NONE) {
                    /* wait for SPI IRQ Event Flag ! */
                    KalRetVal = AmbaKAL_EventFlagGet(&AmbaSpiSlvEventFlag, ((UINT32)1U << SlaveID), KAL_FLAGS_WAIT_ALL, KAL_FLAGS_CLEAR_AUTO, &ActualFlags, TimeOut);
                    if (KalRetVal != KAL_ERR_NONE) {
                        if (KalRetVal == KAL_ERR_TIMEOUT) {
                            RetVal = SPI_ERR_TMO;
                        } else {
                            RetVal = SPI_ERR_UNEXPECTED;
                        }
                    }

                    AmbaRTSL_SpiSlaveStop(SlaveID);

                    if ((RetVal == SPI_ERR_NONE) && (pActualSize != NULL)) {
                        RetVal = AmbaRTSL_SpiSlaveGetInfo(SlaveID, NULL, pActualSize);
                    }
                }
                if (AmbaSpiSlaveIntHandlerErr[SlaveID] != SPI_ERR_NONE) { /* check if any error reported in interrupt handler */
                    RetVal =  AmbaSpiSlaveIntHandlerErr[SlaveID];
                }
            }

            if (AmbaKAL_MutexGive(&AmbaSpiSlvMutex[SlaveID]) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = SPI_ERR_UNEXPECTED;
            }
        }
    }

    return RetVal;
}

/**
 *  AmbaSPI_SlaveDmaTransferD8 - SPI Slave write and read data operation with Dma
 *  @param[in] SlaveID Indicate a SPI slave
 *  @param[in] BufSize The size of TX and RX buffer if exist
 *  @param[in] pTxBuf TX buffer. If this pointer is NULL, the signal on SDO/MOSI pin will be ignored.
 *  @param[out] pRxBuf RX buffer. If this pointer is NULL, the signal on SDI/MISO pin will be ignored.
 *  @param[out] pActualSize The actual number of the TX/RX frames
 *  @param[in] TimeOut The timeout value in system tick (ms)
 *  @return error code
 */
UINT32 AmbaSPI_SlaveDmaTransferD8(UINT32 SlaveID, UINT32 BufSize, const UINT8 *pTxBuf, UINT8 *pRxBuf, UINT32 *pActualSize, UINT32 TimeOut)
{
    UINT32 RetVal = SPI_ERR_NONE;

    if ((SlaveID >= AMBA_NUM_SPI_SLAVE) || (BufSize < 8U) || ((pTxBuf == NULL) && (pRxBuf == NULL))) {
        RetVal = SPI_ERR_ARG;
    } else {
        if (AmbaKAL_MutexTake(&AmbaSpiSlvMutex[SlaveID], KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = SPI_ERR_MUTEX;
        } else if (AmbaKAL_EventFlagClear(&AmbaSpiSlvEventFlag, ((UINT32)1U << SlaveID)) != KAL_ERR_NONE) {
            RetVal = SPI_ERR_UNEXPECTED;
        } else {
            RetVal = SPI_SlaveDoTransferD8(SlaveID, BufSize, pTxBuf, pRxBuf, pActualSize, TimeOut);
            if (AmbaKAL_MutexGive(&AmbaSpiSlvMutex[SlaveID]) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = SPI_ERR_UNEXPECTED;
            }
        }
    }

    return RetVal;
}

/**
 *  AmbaSPI_SlaveDmaTransferD16 - SPI Slave write and read data operation with Dma
 *  @param[in] SlaveID Indicate a SPI slave
 *  @param[in] BufSize The size of TX and RX buffer if exist
 *  @param[in] pTxBuf TX buffer. If this pointer is NULL, the signal on SDO/MOSI pin will be ignored.
 *  @param[out] pRxBuf RX buffer. If this pointer is NULL, the signal on SDI/MISO pin will be ignored.
 *  @param[out] pActualSize The actual number of the TX/RX frames
 *  @param[in] TimeOut The timeout value in system tick (ms)
 *  @return error code
 */
UINT32 AmbaSPI_SlaveDmaTransferD16(UINT32 SlaveID, UINT32 BufSize, const UINT16 *pTxBuf, UINT16 *pRxBuf, UINT32 *pActualSize, UINT32 TimeOut)
{
    UINT32 RetVal = SPI_ERR_NONE;

    if ((SlaveID >= AMBA_NUM_SPI_SLAVE) || (BufSize < 8U) || ((pTxBuf == NULL) && (pRxBuf == NULL))) {
        RetVal = SPI_ERR_ARG;
    } else {
        if (AmbaKAL_MutexTake(&AmbaSpiSlvMutex[SlaveID], KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = SPI_ERR_MUTEX;
        } else if (AmbaKAL_EventFlagClear(&AmbaSpiSlvEventFlag, ((UINT32)1U << SlaveID)) != KAL_ERR_NONE) {
            RetVal = SPI_ERR_UNEXPECTED;
        } else {
            RetVal = SPI_SlaveDoTransferD16(SlaveID, BufSize, pTxBuf, pRxBuf, pActualSize, TimeOut);

            if (AmbaKAL_MutexGive(&AmbaSpiSlvMutex[SlaveID]) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = SPI_ERR_UNEXPECTED;
            }
        }
    }

    return RetVal;
}

/**
 *  AmbaSPI_MasterGetInfo - The function returns all the information about the slave.
 *  @param[in] SlaveID Indicate a SPI slave
 *  @param[out] pStatus The current bus status
 *  @return error code
 */
UINT32 AmbaSPI_SlaveGetInfo(UINT32 SlaveID, UINT32 *pStatus)
{
    UINT32 RetVal = SPI_ERR_NONE;

    if ((SlaveID >= AMBA_NUM_SPI_SLAVE) || (pStatus == NULL)) {
        RetVal = SPI_ERR_ARG;
    } else {
        RetVal = AmbaRTSL_SpiSlaveGetInfo(SlaveID, pStatus, NULL);
    }

    return RetVal;
}

/**
 *  SPI_MasterIntHandler - SPI ISR for Reading/Writing data
 *  @param[in] MasterID Indicate a SPI master
 */
static void SPI_MasterIntHandler(UINT32 MasterID)
{
    /* set SPI Tx/Rx Done Event Flag */
    if (AmbaKAL_EventFlagSet(&AmbaSpiMstEventFlag, (UINT32)((UINT32)1U << MasterID)) != KAL_ERR_NONE) {
        AmbaSpiMasterIntHandlerErr[MasterID] = SPI_ERR_UNEXPECTED;
    }
}

/**
 *  SPI_SlaveIntHandler - SPI ISR for Reading/Writing data
 *  @param[in] SlaveID Indicate a SPI slave
 */
static void SPI_SlaveIntHandler(UINT32 SlaveID)
{
    /* set SPI Tx/Rx Done Event Flag */
    if (AmbaKAL_EventFlagSet(&AmbaSpiSlvEventFlag, (UINT32)((UINT32)1U << SlaveID)) != KAL_ERR_NONE) {
        AmbaSpiSlaveIntHandlerErr[SlaveID] = SPI_ERR_UNEXPECTED;
    }
}

