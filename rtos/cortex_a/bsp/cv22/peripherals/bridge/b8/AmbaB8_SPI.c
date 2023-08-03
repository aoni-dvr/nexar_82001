/**
 *  @file AmbaB8_SPI.c
 *
 *  @copyright Copyright (c) 2015 Ambarella, Inc.
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
 *  @details B8 SPI APIs
 *
 */
#include "AmbaB8.h"

#include "AmbaB8_SPI.h"
#include "AmbaB8_Communicate.h"
#include "AmbaB8_PLL.h"

#include "AmbaB8CSL_SPI.h"

/*-----------------------------------------------------------------------------------------------*\
 * SPI Management Structure
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    AmbaB8_KAL_SEM_t SemID;         /* SPI transfer semaphore */
    UINT32  TxDataSize;             /* Tx Data byte size */
    UINT32  *pTxDataBuf;            /* pointer to Tx Data buffer */
    UINT32  RxDataSize;             /* Rx Data byte size */
    UINT32  *pRxDataBuf;            /* pointer to Rx Data buffer */
} B8_SPI_CTRL_s;

static B8_SPI_CTRL_s AmbaB8_SpiCtrl;/* SPI Management Structure */

static UINT32  B8_SpiSetConfiguration(UINT32 ChipID, const B8_SPI_CONFIG_s *pSpiConfig);
static UINT32  B8_SpiTransfer(UINT32 ChipID, const B8_SPI_CONFIG_s *pSpiConfig, UINT32 DataSize, UINT32 *pTxDataBuf, UINT32 *pRxDataBuf);
static void B8_SpiWriteFifo(UINT32 ChipID, UINT32 TxDataSize, const UINT32 *pTxDataBuf);
static void B8_SpiReadFifo(UINT32 ChipID, UINT32 RxDataSize, UINT32 *pRxDataBuf);
static UINT32  B8_SpiPollingStatus(UINT32 ChipID);

static UINT32 B8_SpiRegDataBuf[B8_SPI_MASTER_MAX_FIFO_ENTRY];

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaB8_SpiInit
 *
 *  @Description:: SPI device driver initializations
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *      int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaB8_SpiInit(void)
{
    UINT32 RetVal = B8_ERR_NONE;
    static UINT8 SemInitDone = 0U;

    if (SemInitDone == 0U) {
        /* Create a semaphore */
        if (AmbaB8_KAL_SemCreate(&AmbaB8_SpiCtrl.SemID, NULL, 1) == OK) {
            SemInitDone = 1U;
        } else {
            AmbaB8_PrintStr5("Unable to create semaphore for B8 SPI transfer", NULL, NULL, NULL, NULL, NULL);
            RetVal = B8_ERR_MUTEX;
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: B8_SpiSetBaudRate
 *
 *  @Description:: SPI Master write and read data operation
 *
 *  @Input      ::
 *      ChipID:    B8 chip id
 *      BaudRate:  Target SPI transmission baud rate
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void B8_SpiSetBaudRate(UINT32 ChipID, UINT32 BaudRate)
{
    UINT32 SpiInputClock = AmbaB8_PllGetSpiClk(ChipID);
    UINT32 ClockDivider;

    if (BaudRate > SpiInputClock) {
        ClockDivider = 2U;
    } else {
        ClockDivider = ((SpiInputClock / BaudRate) + 0x01U) & 0xFFFEU;
    }

    (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_SpiReg->BaudRate), 0U, B8_DATA_WIDTH_32BIT, 1, &ClockDivider);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: B8_SpiSetConfiguration
 *
 *  @Description:: Configure the SPI interface
 *
 *  @Input      ::
 *      ChipID:     B8 chip id
 *      pSpiConfig: Pointer to SPI configurations
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 B8_SpiSetConfiguration(UINT32 ChipID, const B8_SPI_CONFIG_s *pSpiConfig)
{
    UINT32 RetVal = B8_ERR_NONE;

    B8_SPI_SLAVE_CTRL_REG_s TxData_SlaveCtrl;
    B8_SPI_CTRL0_REG_s TxData_Ctrl0;
    B8_SPI_CTRL1_REG_s TxData_Ctrl1;

    B8_SPI_SLAVE_CTRL_REG_s *pNewChipSelectPolRegVal = &TxData_SlaveCtrl;
    B8_SPI_CTRL0_REG_s *pNewCtrl0RegVal = &TxData_Ctrl0;
    B8_SPI_CTRL1_REG_s *pNewCtrl1RegVal = &TxData_Ctrl1;

    UINT32 TxData_SlaveTemp;

    if(pSpiConfig == NULL) {
        RetVal = B8_ERR_ARG;
    } else {

        /* Set SPI baudrate */
        B8_SpiSetBaudRate(ChipID, pSpiConfig->BaudRate);

        /* Set signal polarity of the ChipSelect pin */
        (void) AmbaB8_Wrap_memset(pNewChipSelectPolRegVal, 0, sizeof(B8_SPI_SLAVE_CTRL_REG_s));
        if (pSpiConfig->CsPolarity == B8_SPI_CHIP_SEL_ACTIVE_HIGH) {
            (void) AmbaB8_Wrap_memcpy(&TxData_SlaveTemp, pNewChipSelectPolRegVal, sizeof(B8_SPI_SLAVE_CTRL_REG_s));
            TxData_SlaveTemp |= 1U;
            (void) AmbaB8_Wrap_memcpy(pNewChipSelectPolRegVal, &TxData_SlaveTemp, sizeof(B8_SPI_SLAVE_CTRL_REG_s));
        } else {
            (void) AmbaB8_Wrap_memcpy(&TxData_SlaveTemp, pNewChipSelectPolRegVal, sizeof(B8_SPI_SLAVE_CTRL_REG_s));
            TxData_SlaveTemp &= ~1U;
            (void) AmbaB8_Wrap_memcpy(pNewChipSelectPolRegVal, &TxData_SlaveTemp, sizeof(B8_SPI_SLAVE_CTRL_REG_s));
        }
        (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_SpiReg->ChipSelectPol), 0U, B8_DATA_WIDTH_32BIT, 1, &TxData_SlaveCtrl);

        /* Set operation mode and communication protocol */
        (void) AmbaB8_Wrap_memset(pNewCtrl0RegVal, 0, sizeof(B8_SPI_CTRL0_REG_s));
        (void) AmbaB8_Wrap_memset(pNewCtrl1RegVal, 0, sizeof(B8_SPI_CTRL1_REG_s));

        if (AmbaB8_SpiCtrl.pRxDataBuf == NULL) {
            /* Write only */
            pNewCtrl0RegVal->TransferMode = 1U;
        } else if (AmbaB8_SpiCtrl.pTxDataBuf == NULL) {
            /* Read only */
            pNewCtrl0RegVal->TransferMode = 2U;
            pNewCtrl1RegVal->RxNumDataFrames = (UINT16)AmbaB8_SpiCtrl.RxDataSize - 1U;
        } else {
            /* Read and Write */
            pNewCtrl0RegVal->TransferMode = 0U;
        }

        if (pSpiConfig->ProtocolMode == B8_SPI_CPOL_LOW_CPHA_LOW) {
            pNewCtrl0RegVal->ClkIdleState = 0U; /* The leading (first) clock edge is rising edge */
            pNewCtrl0RegVal->ClkPhase = 0U;     /* Sample on the leading (first) clock edge */
        } else if (pSpiConfig->ProtocolMode == B8_SPI_CPOL_LOW_CPHA_HIGH) {
            pNewCtrl0RegVal->ClkIdleState = 0U; /* The leading (first) clock edge is rising edge */
            pNewCtrl0RegVal->ClkPhase = 1U;     /* Sample on the trailing (second) clock edge */
        } else if (pSpiConfig->ProtocolMode == B8_SPI_CPOL_HIGH_CPHA_LOW) {
            pNewCtrl0RegVal->ClkIdleState = 1U; /* The leading (first) clock edge is fallng edge */
            pNewCtrl0RegVal->ClkPhase = 0U;     /* Sample on the leading (first) clock edge */
        } else if (pSpiConfig->ProtocolMode == B8_SPI_CPOL_HIGH_CPHA_HIGH) {
            pNewCtrl0RegVal->ClkIdleState = 1U; /* The leading (first) clock edge is fallng edge */
            pNewCtrl0RegVal->ClkPhase = 1U;     /* Sample on the trailing (second) clock edge */
        } else {
            AmbaB8_PrintStr5("Wrong pSpiConfig->ProtocolMode!", NULL, NULL, NULL, NULL, NULL);
        }

        pNewCtrl0RegVal->DataFrameSize = pSpiConfig->DataFrameSize - 1U;

        /* Set extra_rxd_margin be 1 to avoid SPI read fail problem */
        pNewCtrl0RegVal->ExtraRxdMargin = 1U;

        (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_SpiReg->Ctrl0), 0U, B8_DATA_WIDTH_32BIT, 1, &TxData_Ctrl0);
        (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_SpiReg->Ctrl1), 0U, B8_DATA_WIDTH_32BIT, 1, &TxData_Ctrl1);
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaB8_SpiTransfer
 *
 *  @Description:: SPI Master write and read data operation
 *
 *  @Input      ::
 *      ChipID:     B8 chip id
 *      pSpiConfig: pointer to configuration of SPI transaction
 *      DataSize:   Data buffer size (in frame count) for either Tx or/and Rx data buffer
 *      pTxDataBuf: pointer to the Tx data buffer (NULL - don't send data)
 *      pRxDataBuf: pointer to the Rx data buffer (NULL - don't receive data)
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *      int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaB8_SpiTransfer(UINT32 ChipID, const B8_SPI_CONFIG_s *pSpiConfig, UINT32 DataSize, UINT32 *pTxDataBuf, UINT32 *pRxDataBuf)
{
    UINT32 RetVal;

    if ((pSpiConfig == NULL) || (DataSize == 0U) || (DataSize > B8_SPI_MASTER_MAX_FIFO_ENTRY) ||
        ((pTxDataBuf == NULL) && (pRxDataBuf == NULL))) {
        RetVal = B8_ERR_ARG;
    } else {
        (void) AmbaB8_KAL_SemTake(&AmbaB8_SpiCtrl.SemID, AMBAB8_KAL_WAIT_FOREVER);
        RetVal = B8_SpiTransfer(ChipID, pSpiConfig, DataSize, pTxDataBuf, pRxDataBuf);
        (void) AmbaB8_KAL_SemGive(&AmbaB8_SpiCtrl.SemID);
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: B8_SpiTransfer
 *
 *  @Description:: SPI Master write and read data operation
 *
 *  @Input      ::
 *      ChipID:     B8 chip id
 *      pSpiConfig: pointer to configuration of SPI transaction
 *      DataSize:   Data buffer size (in frame count) for either Tx or/and Rx data buffer
 *      pTxDataBuf: pointer to the Tx data buffer (NULL - don't send data)
 *      pRxDataBuf: pointer to the Rx data buffer (NULL - don't receive data)
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 B8_SpiTransfer(UINT32 ChipID, const B8_SPI_CONFIG_s *pSpiConfig, UINT32 DataSize, UINT32 *pTxDataBuf, UINT32 *pRxDataBuf)
{
    UINT32 TxData[1];
    UINT32 RetVal = B8_ERR_NONE;

    if ((pSpiConfig == NULL) ||
        (DataSize == 0U) || (DataSize > B8_SPI_MASTER_MAX_FIFO_ENTRY) ||
        ((pTxDataBuf == NULL) && (pRxDataBuf == NULL))) {
        RetVal = B8_ERR_ARG;
    } else if ((pSpiConfig->BaudRate == 0U) || (pSpiConfig->DataFrameSize > 16U)) {
        RetVal = B8_ERR_ARG;
    } else {

        if (pRxDataBuf == NULL) {
            /* Write only */
            AmbaB8_SpiCtrl.pTxDataBuf = pTxDataBuf;
            AmbaB8_SpiCtrl.pRxDataBuf = NULL;
            AmbaB8_SpiCtrl.TxDataSize = DataSize;
            AmbaB8_SpiCtrl.RxDataSize = 0;
        } else if (pTxDataBuf == NULL) {
            /* Read only */
            AmbaB8_SpiCtrl.pTxDataBuf = NULL;
            AmbaB8_SpiCtrl.pRxDataBuf = pRxDataBuf;
            AmbaB8_SpiCtrl.TxDataSize = 0;
            AmbaB8_SpiCtrl.RxDataSize = DataSize;
        } else {
            /* Read and Write */
            AmbaB8_SpiCtrl.pTxDataBuf = pTxDataBuf;
            AmbaB8_SpiCtrl.pRxDataBuf = pRxDataBuf;
            AmbaB8_SpiCtrl.TxDataSize = DataSize;
            AmbaB8_SpiCtrl.RxDataSize = DataSize;
        }

        RetVal |= B8_SpiSetConfiguration(ChipID, pSpiConfig);

        /* Enable FIFO before access FIFO */
        TxData[0] = 0x03U;
        (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_SpiReg->DmaCtrl), 0U, B8_DATA_WIDTH_32BIT, 1, TxData);

        if (pTxDataBuf != NULL) {
            /* Fill Tx FIFO */
            B8_SpiWriteFifo(ChipID, DataSize, pTxDataBuf);
        } else {
            /* Write a dummy data to force SPI controller output the bit clock */
            (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_SpiReg->DataBuf[0]), 0U, B8_DATA_WIDTH_32BIT, 1, TxData);
        }


        /* Enable dedicated Chip Select (or Slave Select) pin */
        TxData[0] = (UINT32)1U << pSpiConfig->SlaveID;
        (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_SpiReg->ChipSelect), 0U, B8_DATA_WIDTH_32BIT, 1, TxData);

        /* Enable SPI master */
        TxData[0] = 0x01U;
        (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_SpiReg->Enable), 0U, B8_DATA_WIDTH_32BIT, 1, TxData);

        RetVal |= B8_SpiPollingStatus(ChipID);

        if (pRxDataBuf != NULL) {
            /* Read data from FIFO */
            B8_SpiReadFifo(ChipID, DataSize, pRxDataBuf);
        }

        /* Disable SPI master */
        TxData[0] = 0x00U;
        (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_SpiReg->Enable), 0U, B8_DATA_WIDTH_32BIT, 1, TxData);

        /* Disable all Slave Devices */
        TxData[0] = 0x00U;
        (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_SpiReg->ChipSelect), 0U, B8_DATA_WIDTH_32BIT, 1, TxData);
    }

    return RetVal;
}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: B8_SpiWriteFifo
 *
 *  @Description:: Write FIFO data
 *
 *  @Input      ::
 *      ChipID:           B8 chip id
 *      TxDaChipIDtaSize: Sending data size in Frames
 *      pTxDataBuf:       Pointer to the sending data buffer
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void B8_SpiWriteFifo(UINT32 ChipID, UINT32 TxDataSize, const UINT32 *pTxDataBuf)
{
    UINT32 i;

    for (i = 0; i < TxDataSize; i++) {
        (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_SpiReg->DataBuf[0]), 0U, B8_DATA_WIDTH_32BIT, 1, &pTxDataBuf[i]);
    }
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: B8_SpiReadFifo
 *
 *  @Description:: Read Data from FIFO
 *
 *  @Input      ::
 *      ChipID:     B8 chip id
 *      RxDataSize: Receiving data size in Frames
 *      pRxDataBuf: Pointer to the receiving data buffer
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void B8_SpiReadFifo(UINT32 ChipID, UINT32 RxDataSize, UINT32 *pRxDataBuf)
{
    UINT32 i;
    UINT32 *pRegDataBuf = (UINT32 *) B8_SpiRegDataBuf;

    for (i = 0; i < RxDataSize; i++) {
        (void) AmbaB8_RegRead(ChipID, & (pAmbaB8_SpiReg->DataBuf[0]), 0U, B8_DATA_WIDTH_32BIT, 1, pRegDataBuf);
        pRxDataBuf[i] = pRegDataBuf[0];
    }
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: B8_SpiPollingStatus
 *
 *  @Description:: Polling for B5F SPI transfer status
 *
 *  @Input      ::
 *      ChipID:    B8 chip id
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static UINT32 B8_SpiPollingStatus(UINT32 ChipID)
{
    UINT32 *DataBuf = (UINT32 *) B8_SpiRegDataBuf;
    INT32 i;
    UINT32 RetVal = B8_ERR_NONE;

    /* Clear read buffer */
    DataBuf[0] = 0;

    for (i = 0; i < 200; i++) {
        RetVal = AmbaB8_RegRead(ChipID, & (pAmbaB8_SpiReg->Status), 0, B8_DATA_WIDTH_32BIT, 1, DataBuf);

        if ((DataBuf[0] & 0x05U) == 0x04U) {
            break;
        }
    }

    if (i == 200) {
        AmbaB8_PrintStr5("B8 SPI TimeOut!", NULL, NULL, NULL, NULL, NULL);
    }

    return RetVal;
}

