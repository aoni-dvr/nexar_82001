/**
 *  @file AmbaRTSL_SPI.c
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
 *  @details SPI RTSL Device Driver
 *
 */

#include "AmbaTypes.h"
#include "AmbaMisraFix.h"
#include "AmbaDef.h"

#include "AmbaRTSL_PLL.h"
#include "AmbaRTSL_GIC.h"
#include "AmbaRTSL_SPI.h"
#include "AmbaCSL_SPI.h"

#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
#include "AmbaCortexA76.h"
#else
#include "AmbaCortexA53.h"
#endif

#if defined(CONFIG_DEVICE_TREE_SUPPORT)
#include <AmbaIOUtility.h>
#endif

AMBA_SPI_REG_s *pAmbaSPI_MasterReg[AMBA_NUM_SPI_MASTER];
AMBA_SPI_REG_s *pAmbaSPI_SlaveReg[AMBA_NUM_SPI_SLAVE];

typedef struct {
    UINT32      BaudRate;           /* PrefBaudRate */
    UINT32      ClkStretch;         /* Stretch the duration (us) from the active edge of chip select to the first edge of clock */
    UINT32      SlaveSelect;        /* Slave select */
    UINT32      MaxNumSlaveSelect;  /* maximum Number of Slave Selects */
    UINT32      MaxNumFifoEntry;    /* maximum Number of FIFO entries */

    UINT32      TxDataSize;         /* Tx Data byte size */
    UINT32      RxDataSize;         /* Rx Data byte size */
    UINT32      TxBufIdx;           /* Tx Buffer Index */
    UINT32      RxBufIdx;           /* Rx Buffer Index */
    const void  *pTxDataBuf;        /* pointer to Tx Data buffer */
    void        *pRxDataBuf;        /* pointer to Rx Data buffer */

    UINT32      IntID;              /* Interrupt ID */
    void        (*pISR)(UINT32 IntID, UINT32 IsrArg); /* pointer to the ISR */
} AMBA_SPI_CTRL_s;

static AMBA_SPI_CTRL_s AmbaSpiMasterCtrl[AMBA_NUM_SPI_MASTER] = {
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
    [AMBA_SPI_MASTER0] = {
        .BaudRate = 0U,
        .MaxNumSlaveSelect = 8U,            /* maximum Number of Slave Selects */
        .MaxNumFifoEntry = AMBA_SPI_MASTER_MAX_FIFO_ENTRY,

        .TxDataSize = 0U,
        .RxDataSize = 0U,
        .pTxDataBuf = NULL,
        .pRxDataBuf = NULL,

        .IntID = AMBA_INT_SPI_ID110_SSI_MASTER0,
        .pISR = SPI_MasterISR
    },

    [AMBA_SPI_MASTER1] = {
        .BaudRate = 0U,
        .MaxNumSlaveSelect = 4U,            /* maximum Number of Slave Selects */
        .MaxNumFifoEntry = AMBA_SPI_MASTER_MAX_FIFO_ENTRY,

        .TxDataSize = 0U,
        .RxDataSize = 0U,
        .pTxDataBuf = NULL,
        .pRxDataBuf = NULL,

        .IntID = AMBA_INT_SPI_ID111_SSI_MASTER1,
        .pISR = SPI_MasterISR
    },

    [AMBA_SPI_MASTER2] = {
        .BaudRate = 0U,
        .MaxNumSlaveSelect = 1U,            /* maximum Number of Slave Selects */
        .MaxNumFifoEntry = AMBA_SPI_MASTER_MAX_FIFO_ENTRY,

        .TxDataSize = 0U,
        .RxDataSize = 0U,
        .pTxDataBuf = NULL,
        .pRxDataBuf = NULL,

        .IntID = AMBA_INT_SPI_ID112_SSI_MASTER2,
        .pISR = SPI_MasterISR
    },

    [AMBA_SPI_MASTER3] = {
        .BaudRate = 0U,
        .MaxNumSlaveSelect = 1U,            /* maximum Number of Slave Selects */
        .MaxNumFifoEntry = AMBA_SPI_MASTER_MAX_FIFO_ENTRY,

        .TxDataSize = 0U,
        .RxDataSize = 0U,
        .pTxDataBuf = NULL,
        .pRxDataBuf = NULL,

        .IntID = AMBA_INT_SPI_ID113_SSI_MASTER3,
        .pISR = SPI_MasterISR
    },

    [AMBA_SPI_MASTER4] = {
        .BaudRate = 0U,
        .MaxNumSlaveSelect = 1U,            /* maximum Number of Slave Selects */
        .MaxNumFifoEntry = AMBA_SPI_MASTER_MAX_FIFO_ENTRY,

        .TxDataSize = 0U,
        .RxDataSize = 0U,
        .pTxDataBuf = NULL,
        .pRxDataBuf = NULL,

        .IntID = AMBA_INT_SPI_ID166_SSI_MASTER4,
        .pISR = SPI_MasterISR
    },

    [AMBA_SPI_MASTER5] = {
        .BaudRate = 0U,
        .MaxNumSlaveSelect = 1U,            /* maximum Number of Slave Selects */
        .MaxNumFifoEntry = AMBA_SPI_MASTER_MAX_FIFO_ENTRY,

        .TxDataSize = 0U,
        .RxDataSize = 0U,
        .pTxDataBuf = NULL,
        .pRxDataBuf = NULL,

        .IntID = AMBA_INT_SPI_ID167_SSI_MASTER5,
        .pISR = SPI_MasterISR
    },
#elif defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)

    [AMBA_SPI_MASTER0] = {
        .BaudRate = 0U,
        .MaxNumSlaveSelect = 6U,            /* maximum Number of Slave Selects */
        .MaxNumFifoEntry = AMBA_SPI_MASTER_MAX_FIFO_ENTRY,

        .TxDataSize = 0U,
        .RxDataSize = 0U,
        .pTxDataBuf = NULL,
        .pRxDataBuf = NULL,

        .IntID = AMBA_INT_SPI_ID106_SSI_MASTER0,
        .pISR = SPI_MasterISR
    },

    [AMBA_SPI_MASTER1] = {
        .BaudRate = 0U,
        .MaxNumSlaveSelect = 4U,            /* maximum Number of Slave Selects */
        .MaxNumFifoEntry = AMBA_SPI_MASTER_MAX_FIFO_ENTRY,

        .TxDataSize = 0U,
        .RxDataSize = 0U,
        .pTxDataBuf = NULL,
        .pRxDataBuf = NULL,

        .IntID = AMBA_INT_SPI_ID107_SSI_MASTER1,
        .pISR = SPI_MasterISR
    },

    [AMBA_SPI_MASTER2] = {
        .BaudRate = 0U,
        .MaxNumSlaveSelect = 1U,            /* maximum Number of Slave Selects */
        .MaxNumFifoEntry = AMBA_SPI_MASTER_MAX_FIFO_ENTRY,

        .TxDataSize = 0U,
        .RxDataSize = 0U,
        .pTxDataBuf = NULL,
        .pRxDataBuf = NULL,

        .IntID = AMBA_INT_SPI_ID108_SSI_MASTER2,
        .pISR = SPI_MasterISR
    },

    [AMBA_SPI_MASTER3] = {
        .BaudRate = 0U,
        .MaxNumSlaveSelect = 1U,            /* maximum Number of Slave Selects */
        .MaxNumFifoEntry = AMBA_SPI_MASTER_MAX_FIFO_ENTRY,

        .TxDataSize = 0U,
        .RxDataSize = 0U,
        .pTxDataBuf = NULL,
        .pRxDataBuf = NULL,

        .IntID = AMBA_INT_SPI_ID109_SSI_MASTER3,
        .pISR = SPI_MasterISR
    },
    [AMBA_SPI_MASTER4] = {
        .BaudRate = 0U,
        .MaxNumSlaveSelect = 1U,            /* maximum Number of Slave Selects */
        .MaxNumFifoEntry = AMBA_SPI_MASTER_MAX_FIFO_ENTRY,

        .TxDataSize = 0U,
        .RxDataSize = 0U,
        .pTxDataBuf = NULL,
        .pRxDataBuf = NULL,

        .IntID = AMBA_INT_SPI_ID110_SSI_MASTER4,
        .pISR = SPI_MasterISR
    },

    [AMBA_SPI_MASTER5] = {
        .BaudRate = 0U,
        .MaxNumSlaveSelect = 1U,            /* maximum Number of Slave Selects */
        .MaxNumFifoEntry = AMBA_SPI_MASTER_MAX_FIFO_ENTRY,

        .TxDataSize = 0U,
        .RxDataSize = 0U,
        .pTxDataBuf = NULL,
        .pRxDataBuf = NULL,

        .IntID = AMBA_INT_SPI_ID111_SSI_MASTER5,
        .pISR = SPI_MasterISR
    },

#else
    [AMBA_SPI_MASTER0] = {
        .BaudRate = 0U,
        .MaxNumSlaveSelect = 8U,            /* maximum Number of Slave Selects */
        .MaxNumFifoEntry = AMBA_SPI_MASTER_MAX_FIFO_ENTRY,

        .TxDataSize = 0U,
        .RxDataSize = 0U,
        .pTxDataBuf = NULL,
        .pRxDataBuf = NULL,

        .IntID = AMBA_INT_SPI_ID106_SSI_MASTER0,
        .pISR = SPI_MasterISR
    },

    [AMBA_SPI_MASTER1] = {
        .BaudRate = 0U,
        .MaxNumSlaveSelect = 4U,            /* maximum Number of Slave Selects */
        .MaxNumFifoEntry = AMBA_SPI_MASTER_MAX_FIFO_ENTRY,

        .TxDataSize = 0U,
        .RxDataSize = 0U,
        .pTxDataBuf = NULL,
        .pRxDataBuf = NULL,

        .IntID = AMBA_INT_SPI_ID107_SSI_MASTER1,
        .pISR = SPI_MasterISR
    },

    [AMBA_SPI_MASTER2] = {
        .BaudRate = 0U,
        .MaxNumSlaveSelect = 1U,            /* maximum Number of Slave Selects */
        .MaxNumFifoEntry = AMBA_SPI_MASTER_MAX_FIFO_ENTRY,

        .TxDataSize = 0U,
        .RxDataSize = 0U,
        .pTxDataBuf = NULL,
        .pRxDataBuf = NULL,

        .IntID = AMBA_INT_SPI_ID108_SSI_MASTER2,
        .pISR = SPI_MasterISR
    },

    [AMBA_SPI_MASTER3] = {
        .BaudRate = 0U,
        .MaxNumSlaveSelect = 1U,            /* maximum Number of Slave Selects */
        .MaxNumFifoEntry = AMBA_SPI_MASTER_MAX_FIFO_ENTRY,

        .TxDataSize = 0U,
        .RxDataSize = 0U,
        .pTxDataBuf = NULL,
        .pRxDataBuf = NULL,

        .IntID = AMBA_INT_SPI_ID109_SSI_MASTER3,
        .pISR = SPI_MasterISR
    },
#endif
};

#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
static AMBA_SPI_CTRL_s AmbaSpiSlaveCtrl[AMBA_NUM_SPI_SLAVE] = {
    [AMBA_SPI_SLAVE] = {
        .BaudRate = 0U,
        .MaxNumSlaveSelect = 0U,            /* maximum Number of Slave Selects */
        .MaxNumFifoEntry = AMBA_SPI_SLAVE_MAX_FIFO_ENTRY,

        .TxDataSize = 0U,
        .RxDataSize = 0U,
        .pTxDataBuf = NULL,
        .pRxDataBuf = NULL,

        .IntID = AMBA_INT_SPI_ID114_SSI_SLAVE,
        .pISR = SPI_SlaveISR
    },
};
#else
static AMBA_SPI_CTRL_s AmbaSpiSlaveCtrl[AMBA_NUM_SPI_SLAVE] = {
    [AMBA_SPI_SLAVE] = {
        .BaudRate = 0U,
        .MaxNumSlaveSelect = 0U,            /* maximum Number of Slave Selects */
        .MaxNumFifoEntry = AMBA_SPI_SLAVE_MAX_FIFO_ENTRY,

        .TxDataSize = 0U,
        .RxDataSize = 0U,
        .pTxDataBuf = NULL,
        .pRxDataBuf = NULL,

        .IntID = AMBA_INT_SPI_ID112_SSI_SLAVE,
        .pISR = SPI_SlaveISR
    },
};
#endif

static AMBA_SPI_ISR_CALLBACK_f AmbaSpiMasterIsrCb = NULL;
static AMBA_SPI_ISR_CALLBACK_f AmbaSpiSlaveIsrCb = NULL;

/**
 *  SPI_WriteFIFO - Write FIFO data
 *  @param[in] pSpiReg pointer to the Hardware registers
 *  @param[in] TxDataSize Number of data frames to be sent
 *  @param[in] pTxDataBuf pointer to a data frame buffer
 */
static void SPI_WriteFIFO(AMBA_SPI_REG_s *pSpiReg, UINT32 TxDataSize, const void *pTxDataBuf, UINT32 *pBufIdx)
{
    UINT32 i;
    UINT8 *pWorkUINT8;
    UINT16 *pWorkUINT16;

    if (AmbaCSL_SpiGetDataFrameSize(pSpiReg) <= 8U) {
        AmbaMisra_TypeCast(&pWorkUINT8, &pTxDataBuf);
        pWorkUINT8 = &pWorkUINT8[*pBufIdx];
        for (i = 0U; i < TxDataSize; i++) {
            AmbaCSL_SpiWriteData(pSpiReg, pWorkUINT8[i]);
        }
    } else {
        AmbaMisra_TypeCast(&pWorkUINT16, &pTxDataBuf);
        pWorkUINT16 = &pWorkUINT16[*pBufIdx];
        for (i = 0U; i < TxDataSize; i++) {
            AmbaCSL_SpiWriteData(pSpiReg, pWorkUINT16[i]);
        }
    }

    *pBufIdx += TxDataSize;
}

/**
 *  SPI_ReadFIFO - Read FIFO data
 *  @param[in] pSpiReg pointer to the Hardware registers
 *  @param[in] RxDataSize Number of data frames to be received
 *  @param[in] pRxDataBuf pointer to a data frame buffer
 */
static void SPI_ReadFIFO(const AMBA_SPI_REG_s *pSpiReg, UINT32 RxDataSize, const void *pRxDataBuf, UINT32 *pBufIdx)
{
    UINT32 i;
    UINT8 *pWorkUINT8;
    UINT16 *pWorkUINT16;

    if (AmbaCSL_SpiGetDataFrameSize(pSpiReg) <= 8U) {
        AmbaMisra_TypeCast(&pWorkUINT8, &pRxDataBuf);
        pWorkUINT8 = &pWorkUINT8[*pBufIdx];
        for (i = 0U; i < RxDataSize; i++) {
            pWorkUINT8[i] = (UINT8) (AmbaCSL_SpiReadData(pSpiReg) & 0x00ffU);
        }
    } else {
        AmbaMisra_TypeCast(&pWorkUINT16, &pRxDataBuf);
        pWorkUINT16 = &pWorkUINT16[*pBufIdx];
        for (i = 0U; i < RxDataSize; i++) {
            pWorkUINT16[i] = (UINT16) (AmbaCSL_SpiReadData(pSpiReg) & 0xffffU);
        }
    }

    *pBufIdx += RxDataSize;
}

/**
 *  SPI_MasterSetBufCtrl - Setup TX/RX buffer
 *  @param[in] MasterID Indicate a SPI master
 *  @param[in] BaudRate Data transfer speed
 *  @param[in] BufSize The size of TX and RX buffer if exist
 *  @param[in] pTxBuf TX buffer. If this pointer is NULL, the signal on SDO/MOSI pin will be ignored.
 *  @param[out] pRxBuf RX buffer. If this pointer is NULL, the signal on SDI/MISO pin will be ignored.
 */
static void SPI_MasterSetBufCtrl(UINT32 MasterID, UINT32 BufSize, const void *pTxBuf, void *pRxBuf)
{
    AMBA_SPI_CTRL_s *pSpiCtrl = &(AmbaSpiMasterCtrl[MasterID]);

    pSpiCtrl->pTxDataBuf = pTxBuf;
    pSpiCtrl->pRxDataBuf = pRxBuf;
    pSpiCtrl->TxBufIdx = 0U;
    pSpiCtrl->RxBufIdx = 0U;

    if (pRxBuf == NULL) {
        /* Write only */
        pSpiCtrl->TxDataSize = BufSize;
        pSpiCtrl->RxDataSize = 0U;
    } else if (pTxBuf == NULL) {
        /* Read only */
        pSpiCtrl->TxDataSize = 0U;
        pSpiCtrl->RxDataSize = BufSize;
    } else {
        /* Read and Write */
        pSpiCtrl->TxDataSize = BufSize;
        pSpiCtrl->RxDataSize = BufSize;
    }
}

/**
 *  SPI_SlaveSetBufCtrl - Setup TX/RX buffer
 *  @param[in] SlaveID Indicate a SPI slave
 *  @param[in] BaudRate Data transfer speed
 *  @param[in] BufSize The size of TX and RX buffer if exist
 *  @param[in] pTxBuf TX buffer. If this pointer is NULL, the signal on MISO pin will be ignored.
 *  @param[out] pRxBuf RX buffer. If this pointer is NULL, the signal on MOSI pin will be ignored.
 */
static void SPI_SlaveSetBufCtrl(UINT32 SlaveID, UINT32 BufSize, const void *pTxBuf, void *pRxBuf)
{
    AMBA_SPI_CTRL_s *pSpiCtrl = &(AmbaSpiSlaveCtrl[SlaveID]);

    AmbaMisra_TypeCast(&(pSpiCtrl->pTxDataBuf), &pTxBuf);
    pSpiCtrl->TxBufIdx = 0U;
    pSpiCtrl->pRxDataBuf = pRxBuf;
    pSpiCtrl->RxBufIdx = 0U;

    if (pRxBuf == NULL) {
        /* Write only */
        pSpiCtrl->TxDataSize = BufSize;
        pSpiCtrl->RxDataSize = 0U;
    } else if (pTxBuf == NULL) {
        /* Read only */
        pSpiCtrl->TxDataSize = 0U;
        pSpiCtrl->RxDataSize = BufSize;
    } else {
        /* Read and Write */
        pSpiCtrl->TxDataSize = BufSize;
        pSpiCtrl->RxDataSize = BufSize;
    }
}

/**
 *  SPI_MasterStart - SPI master transfer start
 *  @param[in] MasterID Indicate a SPI master
 *  @param[in] BufSize The size of TX and RX buffer if exist
 */
static void SPI_MasterStart(UINT32 MasterID, UINT32 BufSize)
{
    AMBA_SPI_REG_s *pSpiReg = pAmbaSPI_MasterReg[MasterID];
    AMBA_SPI_CTRL_s *pSpiCtrl = &(AmbaSpiMasterCtrl[MasterID]);

    if (pSpiCtrl->pRxDataBuf == NULL) {
        /* Write only */
        AmbaCSL_SpiSetTransferModeTx(pSpiReg);
    } else if (pSpiCtrl->pTxDataBuf == NULL) {
        /* Read only */
        AmbaCSL_SpiSetTransferModeRx(pSpiReg);
        AmbaCSL_SpiSetRxNumDataFrames(pSpiReg, BufSize);    /* number of data frame to be receieved continously */
    } else {
        /* Read and Write */
        AmbaCSL_SpiSetTransferModeTxRx(pSpiReg);
    }

    /* Enables TX/RX FIFO for data pushing */
    AmbaCSL_SpiDmaTxEnable(pSpiReg);

    if (BufSize > pSpiCtrl->MaxNumFifoEntry) {
        if (pSpiCtrl->pTxDataBuf == NULL) {     /* Rx only case */
            AmbaCSL_SpiWriteData(pSpiReg, 0U);    /* write a dummy data to force SPI controller output the bit clock */
            AmbaCSL_SpiSetRxFifoThreshold(pSpiReg, 0U);
            AmbaCSL_SpiEnableRxFifoFullIRQ(pSpiReg);
        } else {
            /* SPI Transfer will not be terminated until TxNumDataFrames are transfered. */
            AmbaCSL_SpiSpiHoldEnable(pSpiReg);
            AmbaCSL_SpiSetTxNumDataFrames(pSpiReg, pSpiCtrl->TxDataSize);

            /* Fill Tx FIFO */
            SPI_WriteFIFO(pSpiReg, pSpiCtrl->MaxNumFifoEntry, (pSpiCtrl->pTxDataBuf), &pSpiCtrl->TxBufIdx);
            pSpiCtrl->TxDataSize -= pSpiCtrl->MaxNumFifoEntry;

            if (pSpiCtrl->pRxDataBuf == NULL) { /* Tx only case */
                AmbaCSL_SpiSetTxFifoThreshold(pSpiReg, ((UINT8)pSpiCtrl->MaxNumFifoEntry) / 2U);
                AmbaCSL_SpiEnableTxFifoEmptyIRQ(pSpiReg);   /* enable TX FIFO empty IRQ */
            } else {                            /* Tx&Rx case */
                AmbaCSL_SpiSetRxFifoThreshold(pSpiReg, (((UINT8)pSpiCtrl->MaxNumFifoEntry) / 2U) - 1U);
                AmbaCSL_SpiEnableRxFifoFullIRQ(pSpiReg);    /* enable RX FIFO full IRQ */
            }
        }
    } else {
        if (pSpiCtrl->pTxDataBuf == NULL) {     /* Rx only case */
            AmbaCSL_SpiWriteData(pSpiReg, 0U);    /* write a dummy data to force SPI controller output the bit clock */
            AmbaCSL_SpiSetRxFifoThreshold(pSpiReg, (UINT8)BufSize - 1U);
            AmbaCSL_SpiEnableRxFifoFullIRQ(pSpiReg);
        } else {
            /* SPI Transfer will not be terminated until TxNumDataFrames are transfered. */
            AmbaCSL_SpiSpiHoldEnable(pSpiReg);
            AmbaCSL_SpiSetTxNumDataFrames(pSpiReg, pSpiCtrl->TxDataSize);

            /* Fill Tx FIFO */
            SPI_WriteFIFO(pSpiReg, pSpiCtrl->TxDataSize, pSpiCtrl->pTxDataBuf, &pSpiCtrl->TxBufIdx);
            pSpiCtrl->TxDataSize = 0U;

            if (pSpiCtrl->pRxDataBuf == NULL) { /* Tx only case */
                AmbaCSL_SpiEnableTransDoneIRQ(pSpiReg);
            } else {                            /* Tx&Rx case */
                AmbaCSL_SpiSetRxFifoThreshold(pSpiReg, (UINT8)BufSize - 1U);
                AmbaCSL_SpiEnableRxFifoFullIRQ(pSpiReg);
                AmbaCSL_SpiEnableTransDoneIRQ(pSpiReg);
            }
        }
    }

    AmbaCSL_SpiSlaveEnable(pSpiReg, pSpiCtrl->SlaveSelect); /* activate dedicated chip select (or slave select) pin */
    AmbaCSL_SpiEnable(pSpiReg);
}

/**
 *  SPI_MasterDmaStart - SPI master transfer start with DMA
 *  @param[in] MasterID Indicate a SPI master
 *  @param[in] BufSize The size of TX and RX buffer if exist
 */
static void SPI_MasterDmaStart(UINT32 MasterID, UINT32 BufSize)
{
    AMBA_SPI_REG_s *pSpiReg = pAmbaSPI_MasterReg[MasterID];
    const AMBA_SPI_CTRL_s *pSpiCtrl = &(AmbaSpiMasterCtrl[MasterID]);

    if (pSpiCtrl->pRxDataBuf == NULL) {
        /* Write only */
        AmbaCSL_SpiSetTransferModeTx(pSpiReg);
    } else if (pSpiCtrl->pTxDataBuf == NULL) {
        /* Read only */
        AmbaCSL_SpiSetTransferModeRx(pSpiReg);
        AmbaCSL_SpiSetRxNumDataFrames(pSpiReg, BufSize);    /* number of data frame to be receieved continously */
    } else {
        /* Read and Write */
        AmbaCSL_SpiSetTransferModeTxRx(pSpiReg);
    }

    if (AmbaCSL_SpiGetDataFrameSize(pSpiReg) <= 8U) {
        /* DMA Bus Transaction Block size is 8 byte, and Bus Data Size is 1 byte. Set threshold to 24/8 */
        AmbaCSL_SpiSetRxFifoThreshold(pSpiReg, 7U);
        AmbaCSL_SpiSetTxFifoThreshold(pSpiReg, 23U);
    } else {
        /* DMA Bus Transaction Block size is 8 byte, and Bus Data Size is 2 byte. Set threshold to 12/4 */
        AmbaCSL_SpiSetRxFifoThreshold(pSpiReg, 3U);
        AmbaCSL_SpiSetTxFifoThreshold(pSpiReg, 11U);
    }

    /* prepare for tx dma */
    AmbaCSL_SpiDmaTxEnable(pSpiReg);
    AmbaCSL_SpiSpiHoldEnable(pSpiReg);
    AmbaCSL_SpiSetTxNumDataFrames(pSpiReg, pSpiCtrl->TxDataSize);

    if (pSpiCtrl->pTxDataBuf == NULL) {    /* Rx only case */
        AmbaCSL_SpiWriteData(pSpiReg, 0U);    /* write a dummy data to force SPI controller output the bit clock */
    }

    if (pSpiCtrl->RxDataSize != 0U) {
        /* prepare for rx dma */
        AmbaCSL_SpiDmaRxEnable(pSpiReg);
        AmbaCSL_SpiResidueFlushModeEnable(pSpiReg);
    }

    if (pSpiCtrl->TxDataSize > 11U) {
        while (AmbaCSL_SpiGetTxFifoDataSize(pSpiReg) <= 11U) {
            AmbaDelayCycles(500U);
        }
    } else {
        while (AmbaCSL_SpiGetTxFifoDataSize(pSpiReg) == 0U) {
            AmbaDelayCycles(500U);
        }
    }

    AmbaCSL_SpiSlaveEnable(pSpiReg, pSpiCtrl->SlaveSelect); /* activate dedicated chip select (or slave select) pin */
    AmbaCSL_SpiEnable(pSpiReg);
}

/**
 *  SPI_SlaveStart - SPI slave transfer start
 *  @param[in] SlaveID Indicate a SPI slave
 *  @param[in] BufSize The size of TX and RX buffer if exist
 */
static void SPI_SlaveStart(UINT32 SlaveID, UINT32 BufSize)
{
    AMBA_SPI_REG_s *pSpiReg = pAmbaSPI_SlaveReg[SlaveID];
    AMBA_SPI_CTRL_s *pSpiCtrl = &(AmbaSpiSlaveCtrl[SlaveID]);
    UINT32 TxBufIdx;

    if (pSpiCtrl->pRxDataBuf == NULL) {
        /* Write only */
        AmbaCSL_SpiSetTransferModeTx(pSpiReg);
    } else if (pSpiCtrl->pTxDataBuf == NULL) {
        /* Read only */
        AmbaCSL_SpiSetTransferModeRx(pSpiReg);
        AmbaCSL_SpiSetRxNumDataFrames(pSpiReg, BufSize);    /* number of data frame to be receieved continously */
    } else {
        /* Read and Write */
        AmbaCSL_SpiSetTransferModeTxRx(pSpiReg);
    }

    /* Enables TX/RX FIFO for data pushing */
    AmbaCSL_SpiDmaTxEnable(pSpiReg);

    if (BufSize > pSpiCtrl->MaxNumFifoEntry) {
        if (pSpiCtrl->pTxDataBuf == NULL) {    /* Rx only case */
            AmbaCSL_SpiWriteData(pSpiReg, 0U);    /* write a dummy data to force SPI controller output the bit clock */
            AmbaCSL_SpiSetRxFifoThreshold(pSpiReg, 0U);
            AmbaCSL_SpiEnableRxFifoFullIRQ(pSpiReg);
        } else {
            /* SPI Transfer will not be terminated until TxNumDataFrames are transfered. */
            AmbaCSL_SpiSpiHoldEnable(pSpiReg);
            AmbaCSL_SpiSetTxNumDataFrames(pSpiReg, pSpiCtrl->TxDataSize);

            /* Fill Tx FIFO */
            SPI_WriteFIFO(pSpiReg, pSpiCtrl->MaxNumFifoEntry, (pSpiCtrl->pTxDataBuf), &pSpiCtrl->TxBufIdx);
            pSpiCtrl->TxDataSize -= pSpiCtrl->MaxNumFifoEntry;

            if (pSpiCtrl->pRxDataBuf == NULL) { /* Tx only case */
                AmbaCSL_SpiSetTxFifoThreshold(pSpiReg, ((UINT8)pSpiCtrl->MaxNumFifoEntry) / 2U);
                AmbaCSL_SpiEnableTxFifoEmptyIRQ(pSpiReg);   /* enable TX FIFO empty IRQ */
            } else {                            /* Tx&Rx case */
                AmbaCSL_SpiSetRxFifoThreshold(pSpiReg, (((UINT8)pSpiCtrl->MaxNumFifoEntry) / 2U) - 1U);
                AmbaCSL_SpiEnableRxFifoFullIRQ(pSpiReg);    /* enable RX FIFO full IRQ */
            }
        }
    } else if (BufSize == pSpiCtrl->MaxNumFifoEntry) {
        if (pSpiCtrl->pTxDataBuf == NULL) {    /* Rx only case */
            AmbaCSL_SpiWriteData(pSpiReg, 0U);    /* write a dummy data to force SPI controller output the bit clock */
            AmbaCSL_SpiSetRxFifoThreshold(pSpiReg, 0U);
            AmbaCSL_SpiEnableRxFifoFullIRQ(pSpiReg);
        } else {
            /* SPI Transfer will not be terminated until TxNumDataFrames are transfered. */
            AmbaCSL_SpiSpiHoldEnable(pSpiReg);
            AmbaCSL_SpiSetTxNumDataFrames(pSpiReg, pSpiCtrl->TxDataSize);

            /* Fill Tx FIFO */
            SPI_WriteFIFO(pSpiReg, (pSpiCtrl->TxDataSize - 1U), (pSpiCtrl->pTxDataBuf), &pSpiCtrl->TxBufIdx);
            pSpiCtrl->TxDataSize = 1U;

            if (pSpiCtrl->pRxDataBuf == NULL) { /* Tx only case */
                AmbaCSL_SpiSetTxFifoThreshold(pSpiReg, ((UINT8)pSpiCtrl->MaxNumFifoEntry) / 2U);
                AmbaCSL_SpiEnableTxFifoEmptyIRQ(pSpiReg);   /* enable TX FIFO empty IRQ */
            } else {                            /* Tx&Rx case */
                AmbaCSL_SpiSetRxFifoThreshold(pSpiReg, (((UINT8)pSpiCtrl->MaxNumFifoEntry) / 2U) - 1U);
                AmbaCSL_SpiEnableRxFifoFullIRQ(pSpiReg);    /* enable RX FIFO full IRQ */
            }
        }
    } else {
        if (pSpiCtrl->pTxDataBuf == NULL) {    /* Rx only case */
            AmbaCSL_SpiWriteData(pSpiReg, 0U);    /* write a dummy data to force SPI controller output the bit clock */
            AmbaCSL_SpiSetRxFifoThreshold(pSpiReg, (UINT8)BufSize - 1U);
            AmbaCSL_SpiEnableRxFifoFullIRQ(pSpiReg);
        } else {
            /* SPI Transfer will not be terminated until TxNumDataFrames are transfered. */
            AmbaCSL_SpiSpiHoldEnable(pSpiReg);
            AmbaCSL_SpiSetTxNumDataFrames(pSpiReg, pSpiCtrl->TxDataSize);

            /* Fill Tx FIFO */
            SPI_WriteFIFO(pSpiReg, pSpiCtrl->TxDataSize, pSpiCtrl->pTxDataBuf, &pSpiCtrl->TxBufIdx);
            pSpiCtrl->TxDataSize = 0U;

            /* Fill TX FIFO with redundant data to avoid emptying FIFO. */
            /* For [total Tx data size] < [Max FIFO size] case */
            TxBufIdx = 0U;
            SPI_WriteFIFO(pSpiReg, 1U, pSpiCtrl->pTxDataBuf, &TxBufIdx);

            if (pSpiCtrl->pRxDataBuf == NULL) { /* Tx only case */
                AmbaCSL_SpiSetTxFifoThreshold(pSpiReg, (UINT8)BufSize);
                AmbaCSL_SpiEnableTxFifoEmptyIRQ(pSpiReg);
            } else {                            /* Tx&Rx case */
                AmbaCSL_SpiSetRxFifoThreshold(pSpiReg, (UINT8)BufSize - 1U);
                AmbaCSL_SpiEnableRxFifoFullIRQ(pSpiReg);
            }
        }
    }
    AmbaCSL_SpiEnable(pSpiReg);
}

/**
 *  SPI_SlaveDmaStart - SPI slave transfer start with DMA
 *  @param[in] SlaveID Indicate a SPI slave
 *  @param[in] BufSize The size of TX and RX buffer if exist
 */
static void SPI_SlaveDmaStart(UINT32 SlaveID, UINT32 BufSize)
{
    AMBA_SPI_REG_s *pSpiReg = pAmbaSPI_SlaveReg[SlaveID];
    const AMBA_SPI_CTRL_s *pSpiCtrl = &(AmbaSpiSlaveCtrl[SlaveID]);

    if (pSpiCtrl->pRxDataBuf == NULL) {
        /* Write only */
        AmbaCSL_SpiSetTransferModeTx(pSpiReg);
    } else if (pSpiCtrl->pTxDataBuf == NULL) {
        /* Read only */
        AmbaCSL_SpiSetTransferModeRx(pSpiReg);
        AmbaCSL_SpiSetRxNumDataFrames(pSpiReg, BufSize);    /* number of data frame to be receieved continously */
    } else {
        /* Read and Write */
        AmbaCSL_SpiSetTransferModeTxRx(pSpiReg);
    }

    if (AmbaCSL_SpiGetDataFrameSize(pSpiReg) <= 8U) {
        /* DMA Bus Transaction Block size is 8 byte, and Bus Data Size is 1 byte. Set threshold to 24/8 */
        AmbaCSL_SpiSetRxFifoThreshold(pSpiReg, 7U);
        AmbaCSL_SpiSetTxFifoThreshold(pSpiReg, 23U);
    } else {
        /* DMA Bus Transaction Block size is 8 byte, and Bus Data Size is 2 byte. Set threshold to 12/4 */
        AmbaCSL_SpiSetRxFifoThreshold(pSpiReg, 3U);
        AmbaCSL_SpiSetTxFifoThreshold(pSpiReg, 11U);
    }

    /* prepare for tx dma */
    AmbaCSL_SpiDmaTxEnable(pSpiReg);
    AmbaCSL_SpiSpiHoldEnable(pSpiReg);
    AmbaCSL_SpiSetTxNumDataFrames(pSpiReg, pSpiCtrl->TxDataSize);

    if (pSpiCtrl->pTxDataBuf == NULL) {    /* Rx only case */
        AmbaCSL_SpiWriteData(pSpiReg, 0U);    /* write a dummy data to force SPI controller output the bit clock */
    }

    if (pSpiCtrl->RxDataSize != 0U) {
        /* prepare for rx dma */
        AmbaCSL_SpiDmaRxEnable(pSpiReg);
        AmbaCSL_SpiResidueFlushModeEnable(pSpiReg);
    }

    if (pSpiCtrl->TxDataSize > 11U) {
        while (AmbaCSL_SpiGetTxFifoDataSize(pSpiReg) <= 11U) {
            AmbaDelayCycles(500U);
        }
    } else {
        while (AmbaCSL_SpiGetTxFifoDataSize(pSpiReg) == 0U) {
            AmbaDelayCycles(500U);
        }
    }

    AmbaCSL_SpiSlaveEnable(pSpiReg, pSpiCtrl->SlaveSelect); /* activate dedicated chip select (or slave select) pin */
    AmbaCSL_SpiEnable(pSpiReg);
}

#ifdef CONFIG_QNX
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
/**
 *  SPI_GetSpiMasterClk - Get SPI clock From RCT registers
 *  @param[in] None
 */
static UINT32 SPI_GetSpiMasterClk(void)
{
    UINT32 Coreclk, IntProg, Sdiv, Sout;

    IntProg = pAmbaRCT_Reg->CorePllCtrl.IntegerPart + (UINT32)1U;
    Sdiv = pAmbaRCT_Reg->CorePllCtrl.Sdiv + (UINT32)1U;
    Sout = pAmbaRCT_Reg->CorePllCtrl.Sout + (UINT32)1U;

    /* PLL Output Frequency = (RefFreq/preScaler) * ((IntProg + 1) + Fraction/2^32) * (sdiv + 1)/(sout+1) */
    Coreclk = AMBA_SPI_REF_CLK * IntProg * Sdiv / Sout;

    return Coreclk / pAmbaRCT_Reg->SpiMasterClkDivider.ClkDivider;
}

/**
 *  SPI_SetSpiMasterClk - Set SPI clock
 *  @param[in] Frequency Indicate the required SPI clock
 */
static void SPI_SetSpiMasterClk(UINT32 Frequency)
{
    UINT32 SpiClkOrig, ClkDivd;

    SpiClkOrig = SPI_GetSpiMasterClk();
    ClkDivd = SpiClkOrig * (pAmbaRCT_Reg->SpiMasterClkDivider.ClkDivider) / Frequency;

    if (ClkDivd < 1U) {
        ClkDivd = 1U;
    }

    pAmbaRCT_Reg->SpiMasterClkDivider.ClkDivider = ClkDivd;
}
#else
/**
 *  SPI_GetSpiMasterClk - Get SPI clock From RCT registers
 *  @param[in] None
 */
static UINT32 SPI_GetSpiMasterClk(void)
{
    UINT32 Coreclk, IntProg, Sdiv, Sout;

    IntProg = pAmbaRCT_Reg->CorePllCtrl.IntProg + (UINT32)1U;
    Sdiv = pAmbaRCT_Reg->CorePllCtrl.Sdiv + (UINT32)1U;
    Sout = pAmbaRCT_Reg->CorePllCtrl.Sout + (UINT32)1U;

    /* PLL Output Frequency = (RefFreq/preScaler) * ((IntProg + 1) + Fraction/2^32) * (sdiv + 1)/(sout+1) */
    Coreclk = AMBA_SPI_REF_CLK * IntProg * Sdiv / Sout;

    return Coreclk / pAmbaRCT_Reg->SpiMasterClkDivider.Divider;
}

/**
 *  SPI_SetSpiMasterClk - Set SPI clock
 *  @param[in] Frequency Indicate the required SPI clock
 */
static void SPI_SetSpiMasterClk(UINT32 Frequency)
{
    UINT32 SpiClkOrig, ClkDivd;

    SpiClkOrig = SPI_GetSpiMasterClk();
    ClkDivd = SpiClkOrig * (pAmbaRCT_Reg->SpiMasterClkDivider.Divider) / Frequency;

    if (ClkDivd < 1U) {
        ClkDivd = 1U;
    }

    pAmbaRCT_Reg->SpiMasterClkDivider.Divider = ClkDivd;
}
#endif
#endif

/**
 *  SPI_MasterSetBaudRate - Set SPI baudrate
 *  @param[in] MasterID Indicate a SPI master
 *  @param[in] BaudRate Target baudrate value
 */
static void SPI_MasterSetBaudRate(UINT32 MasterID, UINT32 BaudRate)
{
    AMBA_SPI_REG_s *pSpiReg = pAmbaSPI_MasterReg[MasterID];
    AMBA_SPI_CTRL_s *pSpiCtrl = &AmbaSpiMasterCtrl[MasterID];
    UINT32 SpiInputClock, SpiOutputFreq, ClockDivider;
    FLOAT SclkDelay;
    FLOAT RxdMargin, SsiClk;

    pSpiCtrl->BaudRate = BaudRate;

#ifdef CONFIG_THREADX
    (void)AmbaRTSL_PllSetSpiMasterClk(120000000U);  /* gclk_ssi for spi masters */
    SpiInputClock = AmbaRTSL_PllGetSpiMasterClk();
#endif
#ifdef CONFIG_QNX
    (void)SPI_SetSpiMasterClk(120000000U);  /* gclk_ssi for spi masters */
    SpiInputClock = SPI_GetSpiMasterClk();
#endif

    if (BaudRate > SpiInputClock) {
        ClockDivider = 2U;
    } else {
        if (BaudRate != 0x0U) {
            ClockDivider = ((SpiInputClock / BaudRate) + 0x01U) & 0xFFFEU;
        } else {
            ClockDivider = 1U;
        }
    }
    AmbaCSL_SpiSetClkDivider(pSpiReg, ClockDivider);

    if ((SpiInputClock != 0UL) && (ClockDivider != 0UL)) {
        /* Derive the corresponding delay cycles according to the baud-rate */
        SclkDelay = ((FLOAT)pSpiCtrl->ClkStretch * ((FLOAT) SpiInputClock / ((FLOAT)ClockDivider * 1000000.0F)));
        AmbaCSL_SpiSetSclkDelay(pSpiReg, (UINT32)SclkDelay);

        /* Set rxd_margin  */
        /* (1)If the round chip delay > SsiClk/2, delay - SsiClk/2 <= rxd_margin * gclk_ssi */
        /* (2)If the round chip delay <= SsiClk/2, rxd_margin = 0 */
        SsiClk = 1.0F / (FLOAT)SpiInputClock;
        SpiOutputFreq = SpiInputClock / ClockDivider;

        if ((SpiOutputFreq > 10000000U) && (SpiOutputFreq < 15000000U)) {
            /* 10MHz < Baud rate < 16MHz */
            /* RX Margin Time = 10ns */
            RxdMargin = (0.000000010F) / SsiClk;
        } else if (SpiOutputFreq >= 15000000U) {
            /* Baud rate >= 16MHz */
            /* RX Margin Time = 20ns */
            RxdMargin = (0.000000020F) / SsiClk;
        } else {
            RxdMargin = 0.0F;
        }

        AmbaCSL_SpiSetExtraRxdMargin(pSpiReg, (UINT8)RxdMargin);
    }
}

/**
 *  SPI_SlaveSetBaudRate - Set SPI baudrate
 *  @param[in] SlaveID Indicate a SPI slave
 *  @param[in] BaudRate Target baudrate value
 */
static void SPI_SlaveSetBaudRate(UINT32 SlaveID, UINT32 BaudRate)
{
    AMBA_SPI_CTRL_s *pSpiCtrl = &AmbaSpiSlaveCtrl[SlaveID];

    pSpiCtrl->BaudRate = BaudRate;

#ifdef CONFIG_THREADX
    (void)AmbaRTSL_PllSetSpiSlaveClk(BaudRate);   /* gclk_ssi2 for spi slaves */
#endif
}

#ifdef CONFIG_THREADX
/**
 *  AmbaRTSL_SpiInit - SPI module initializations
 *  @return error code
 */
UINT32 AmbaRTSL_SpiInit(AMBA_SPI_ISR_CALLBACK_f CallBackFunc, AMBA_SPI_ISR_CALLBACK_f SlaveCallBackFunc)
{
    const AMBA_SPI_CTRL_s *pSpiCtrl;
    AMBA_INT_CONFIG_s IntConfig;
    UINT32 Irq, i;
    ULONG base_addr;
    INT32 offset = 0;
    (void) offset;
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
    base_addr = AMBA_CA53_SPI0_BASE_ADDR;
    AmbaMisra_TypeCast(&pAmbaSPI_MasterReg[0], &base_addr);
    base_addr = AMBA_CA53_SPI1_BASE_ADDR;
    AmbaMisra_TypeCast(&pAmbaSPI_MasterReg[1], &base_addr);
    base_addr = AMBA_CA53_SPI2_BASE_ADDR;
    AmbaMisra_TypeCast(&pAmbaSPI_MasterReg[2], &base_addr);
    base_addr = AMBA_CA53_SPI3_BASE_ADDR;
    AmbaMisra_TypeCast(&pAmbaSPI_MasterReg[3], &base_addr);
    base_addr = AMBA_CA53_SPI4_BASE_ADDR;
    AmbaMisra_TypeCast(&pAmbaSPI_MasterReg[4], &base_addr);
    base_addr = AMBA_CA53_SPI5_BASE_ADDR;
    AmbaMisra_TypeCast(&pAmbaSPI_MasterReg[5], &base_addr);

    base_addr = AMBA_CA53_SPI_SLAVE_BASE_ADDR;
    AmbaMisra_TypeCast(&pAmbaSPI_SlaveReg[0], &base_addr);
#elif defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    base_addr = AMBA_CORTEX_A76_SSI0_BASE_ADDR;
    AmbaMisra_TypeCast(&pAmbaSPI_MasterReg[0], &base_addr);
    base_addr = AMBA_CORTEX_A76_SSI1_BASE_ADDR;
    AmbaMisra_TypeCast(&pAmbaSPI_MasterReg[1], &base_addr);
    base_addr = AMBA_CORTEX_A76_SSI2_BASE_ADDR;
    AmbaMisra_TypeCast(&pAmbaSPI_MasterReg[2], &base_addr);
    base_addr = AMBA_CORTEX_A76_SSI3_BASE_ADDR;
    AmbaMisra_TypeCast(&pAmbaSPI_MasterReg[3], &base_addr);
    base_addr = AMBA_CORTEX_A76_SSI4_BASE_ADDR;
    AmbaMisra_TypeCast(&pAmbaSPI_MasterReg[4], &base_addr);
    base_addr = AMBA_CORTEX_A76_SSI5_BASE_ADDR;
    AmbaMisra_TypeCast(&pAmbaSPI_MasterReg[5], &base_addr);

    base_addr = AMBA_CORTEX_A76_SSI_SLAVE_BASE_ADDR;
    AmbaMisra_TypeCast(&pAmbaSPI_SlaveReg[0], &base_addr);
#else
    base_addr = AMBA_CORTEX_A53_SSI0_BASE_ADDR;
    AmbaMisra_TypeCast(&pAmbaSPI_MasterReg[0], &base_addr);
    base_addr = AMBA_CORTEX_A53_SSI1_BASE_ADDR;
    AmbaMisra_TypeCast(&pAmbaSPI_MasterReg[1], &base_addr);
    base_addr = AMBA_CORTEX_A53_SSI2_BASE_ADDR;
    AmbaMisra_TypeCast(&pAmbaSPI_MasterReg[2], &base_addr);
    base_addr = AMBA_CORTEX_A53_SSI3_BASE_ADDR;
    AmbaMisra_TypeCast(&pAmbaSPI_MasterReg[3], &base_addr);

    base_addr = AMBA_CORTEX_A53_SSI_SLAVE_BASE_ADDR;
    AmbaMisra_TypeCast(&pAmbaSPI_SlaveReg[0], &base_addr);
#endif

#if defined(CONFIG_DEVICE_TREE_SUPPORT)
    for (i = 0U; i < AMBA_NUM_SPI_MASTER; i++) {
        if (i != 0U) {
            offset = IO_UtilityFDTNodeOffsetByCID(offset, "ambarella,spi");
        }
        base_addr = IO_UtilityFDTPropertyU32Quick(offset, "ambarella,spi", "reg", 0U);
        if ( base_addr != 0U ) {
#if defined (CONFIG_SOC_CV5)|| defined (CONFIG_SOC_CV52)
            base_addr = base_addr | AMBA_CORTEX_A76_AHB_PHYS_BASE_ADDR;
#endif
            AmbaMisra_TypeCast(&pAmbaSPI_MasterReg[i], &base_addr);
        }
    }
#endif

    AmbaSpiMasterIsrCb = CallBackFunc;
    AmbaSpiSlaveIsrCb = SlaveCallBackFunc;

    IntConfig.TriggerType = INT_TRIG_HIGH_LEVEL;
    IntConfig.IrqType = INT_TYPE_FIQ;
    IntConfig.CpuTargets = 1U;

    for (i = 0U; i < AMBA_NUM_SPI_MASTER; i++) {
        pSpiCtrl = &AmbaSpiMasterCtrl[i];
        Irq = pSpiCtrl->IntID;
        (void)AmbaRTSL_GicIntConfig(Irq, &IntConfig, pSpiCtrl->pISR, i);
        (void)AmbaRTSL_GicIntEnable(Irq);
    }

    for (i = 0U; i < AMBA_NUM_SPI_SLAVE; i++) {
        pSpiCtrl = &AmbaSpiSlaveCtrl[i];
        Irq = pSpiCtrl->IntID;
        (void)AmbaRTSL_GicIntConfig(Irq, &IntConfig, pSpiCtrl->pISR, i);
        (void)AmbaRTSL_GicIntEnable(Irq);
    }

    return SPI_ERR_NONE;
}
#endif

/**
 *  AmbaRTSL_SpiMasterConfig - Configure the SPI interface
 *  @param[in] MasterID Indicate a SPI master
 *  @param[in] SlaveMask Indicate active slave devices of the master
 *  @param[in] pConfig Define the combinations of polarity and phases for transitions where data should be latched.
 *  @return error code
 */
UINT32 AmbaRTSL_SpiMasterConfig(UINT32 MasterID, UINT32 SlaveMask, const AMBA_SPI_CONFIG_s *pConfig)
{
    AMBA_SPI_CTRL_s *pSpiCtrl = &AmbaSpiMasterCtrl[MasterID];
    AMBA_SPI_REG_s *pSpiReg = pAmbaSPI_MasterReg[MasterID];
    UINT32 RetVal = SPI_ERR_NONE;

    if ((MasterID >= AMBA_NUM_SPI_MASTER) ||
        (pConfig == NULL) || (pConfig->BaudRate == 0U) || (pConfig->DataFrameSize == 0U)) {
        RetVal = SPI_ERR_ARG;
    } else {
        pSpiCtrl = &AmbaSpiMasterCtrl[MasterID];
        if ((SlaveMask & (((UINT32)1U << pSpiCtrl->MaxNumSlaveSelect) - 1U)) == 0U) {
            RetVal = SPI_ERR_ARG;
        } else {
            pSpiCtrl->SlaveSelect = SlaveMask;
        }

        if (RetVal == SPI_ERR_NONE) {
            AmbaCSL_SpiDisable(pSpiReg);
            AmbaCSL_SpiSlaveDisableAll(pSpiReg);
            AmbaCSL_SpiDisableAllIRQ(pSpiReg);

            pSpiCtrl->ClkStretch = pConfig->ClkStretch;
            SPI_MasterSetBaudRate(MasterID, pConfig->BaudRate);

            if (pConfig->ClkMode == SPI_CPOL_LO_CPHA_LO) {
                AmbaCSL_SpiSetClockPolarity(pSpiReg, 0U);   /* the leading (first) clock edge is rising edge */
                AmbaCSL_SpiSetClockPhase(pSpiReg, 0U);      /* sample on the leading (first) clock edge */
            } else if (pConfig->ClkMode == SPI_CPOL_LO_CPHA_HI) {
                AmbaCSL_SpiSetClockPolarity(pSpiReg, 0U);   /* the leading (first) clock edge is rising edge */
                AmbaCSL_SpiSetClockPhase(pSpiReg, 1U);      /* sample on the trailing (second) clock edge */
            } else if (pConfig->ClkMode == SPI_CPOL_HI_CPHA_LO) {
                AmbaCSL_SpiSetClockPolarity(pSpiReg, 1U);   /* the leading (first) clock edge is fallng edge */
                AmbaCSL_SpiSetClockPhase(pSpiReg, 0U);      /* sample on the leading (first) clock edge */
            } else if (pConfig->ClkMode == SPI_CPOL_HI_CPHA_HI) {
                AmbaCSL_SpiSetClockPolarity(pSpiReg, 1U);   /* the leading (first) clock edge is fallng edge */
                AmbaCSL_SpiSetClockPhase(pSpiReg, 1U);      /* sample on the trailing (second) clock edge */
            } else {
                RetVal = SPI_ERR_CFG;
            }

            if (pConfig->CsPolarity == SPI_CS_ACTIVE_HI) {
                AmbaCSL_SpiSetCsActiveHigh(pSpiReg, pSpiCtrl->SlaveSelect);
            } else {
                AmbaCSL_SpiSetCsActiveLow(pSpiReg, pSpiCtrl->SlaveSelect);
            }

            /* Set Tx/Rx LSB/MSB first */
            if (pConfig->FrameBitOrder == SPI_XFER_LSB_FIRST) {
                AmbaCSL_SpiSetTxLsbTransmitFirst(pSpiReg);
                AmbaCSL_SpiSetRxLsbTransmitFirst(pSpiReg);
            } else {
                AmbaCSL_SpiSetTxMsbTransmitFirst(pSpiReg);
                AmbaCSL_SpiSetRxMsbTransmitFirst(pSpiReg);
            }

            AmbaCSL_SpiSetDataFrameSize(pSpiReg, (UINT8)pConfig->DataFrameSize);
        }
    }

    return RetVal;
}

/**
 *  AmbaRTSL_SpiSlaveConfig - Configure the SPI interface
 *  @param[in] SlaveID Indicate a SPI slave
 *  @param[in] pConfig Define the combinations of polarity and phases for transitions where data should be latched.
 *  @return error code
 */
UINT32 AmbaRTSL_SpiSlaveConfig(UINT32 SlaveID, const AMBA_SPI_CONFIG_s *pConfig)
{
    AMBA_SPI_CTRL_s *pSpiCtrl = &AmbaSpiSlaveCtrl[SlaveID];
    AMBA_SPI_REG_s *pSpiReg = pAmbaSPI_SlaveReg[SlaveID];
    UINT32 RetVal = SPI_ERR_NONE;

    if ((SlaveID >= AMBA_NUM_SPI_SLAVE) ||
        (pConfig == NULL) || (pConfig->BaudRate == 0U) || (pConfig->DataFrameSize == 0U)) {
        RetVal = SPI_ERR_ARG;
    } else {
        pSpiCtrl->SlaveSelect = 0x1U;

        AmbaCSL_SpiDisable(pSpiReg);
        AmbaCSL_SpiSlaveDisableAll(pSpiReg);
        AmbaCSL_SpiDisableAllIRQ(pSpiReg);

        SPI_SlaveSetBaudRate(SlaveID, pConfig->BaudRate);

        if (pConfig->ClkMode == SPI_CPOL_LO_CPHA_LO) {
            AmbaCSL_SpiSetClockPolarity(pSpiReg, 0U);   /* the leading (first) clock edge is rising edge */
            AmbaCSL_SpiSetClockPhase(pSpiReg, 0U);      /* sample on the leading (first) clock edge */
        } else if (pConfig->ClkMode == SPI_CPOL_LO_CPHA_HI) {
            AmbaCSL_SpiSetClockPolarity(pSpiReg, 0U);   /* the leading (first) clock edge is rising edge */
            AmbaCSL_SpiSetClockPhase(pSpiReg, 1U);      /* sample on the trailing (second) clock edge */
        } else if (pConfig->ClkMode == SPI_CPOL_HI_CPHA_LO) {
            AmbaCSL_SpiSetClockPolarity(pSpiReg, 1U);   /* the leading (first) clock edge is fallng edge */
            AmbaCSL_SpiSetClockPhase(pSpiReg, 0U);      /* sample on the leading (first) clock edge */
        } else if (pConfig->ClkMode == SPI_CPOL_HI_CPHA_HI) {
            AmbaCSL_SpiSetClockPolarity(pSpiReg, 1U);   /* the leading (first) clock edge is fallng edge */
            AmbaCSL_SpiSetClockPhase(pSpiReg, 1U);      /* sample on the trailing (second) clock edge */
        } else {
            RetVal = SPI_ERR_CFG;
        }

        if (pConfig->CsPolarity == SPI_CS_ACTIVE_HI) {
            AmbaCSL_SpiSetCsActiveHigh(pSpiReg, pSpiCtrl->SlaveSelect);
        } else {
            AmbaCSL_SpiSetCsActiveLow(pSpiReg, pSpiCtrl->SlaveSelect);
        }

        /*Set Tx/Rx LSB/MSB first*/
        if (pConfig->FrameBitOrder == SPI_XFER_LSB_FIRST) {
            AmbaCSL_SpiSetTxLsbTransmitFirst(pSpiReg);
            AmbaCSL_SpiSetRxLsbTransmitFirst(pSpiReg);
        } else {
            AmbaCSL_SpiSetTxMsbTransmitFirst(pSpiReg);
            AmbaCSL_SpiSetRxMsbTransmitFirst(pSpiReg);
        }

        AmbaCSL_SpiSetDataFrameSize(pSpiReg, (UINT8)pConfig->DataFrameSize);
    }

    return RetVal;
}

/**
 *  AmbaRTSL_SpiMasterTransferD8 - SPI Master write and read data operation
 *  @param[in] MasterID Indicate a SPI master
 *  @param[in] BufSize The size of TX and RX buffer if exist
 *  @param[in] pTxBuf TX buffer. If this pointer is NULL, the signal on SDO/MOSI pin will be ignored.
 *  @param[out] pRxBuf RX buffer. If this pointer is NULL, the signal on SDI/MISO pin will be ignored.
 *  @return error code
 */
UINT32 AmbaRTSL_SpiMasterTransferD8(UINT32 MasterID, UINT32 BufSize, const UINT8 *pTxBuf, UINT8 *pRxBuf)
{
    const AMBA_SPI_REG_s *pSpiReg = pAmbaSPI_MasterReg[MasterID];
    UINT32 RetVal = SPI_ERR_NONE;

    if ((MasterID >= AMBA_NUM_SPI_MASTER) || (BufSize == 0U) || ((pTxBuf == NULL) && (pRxBuf == NULL))) {
        RetVal = SPI_ERR_ARG;
    } else {
        if (AmbaCSL_SpiGetDataFrameSize(pSpiReg) > 8U) {
            RetVal = SPI_ERR_CFG;
        } else {
            SPI_MasterSetBufCtrl(MasterID, BufSize, pTxBuf, pRxBuf);
            SPI_MasterStart(MasterID, BufSize);
        }
    }

    return RetVal;
}

/**
 *  AmbaRTSL_SpiMasterTransferD16 - SPI Master write and read data operation
 *  @param[in] MasterID Indicate a SPI master
 *  @param[in] BufSize The size of TX and RX buffer if exist
 *  @param[in] pTxBuf TX buffer. If this pointer is NULL, the signal on SDO/MOSI pin will be ignored.
 *  @param[out] pRxBuf RX buffer. If this pointer is NULL, the signal on SDI/MISO pin will be ignored.
 *  @return error code
 */
UINT32 AmbaRTSL_SpiMasterTransferD16(UINT32 MasterID, UINT32 BufSize, const UINT16 *pTxBuf, UINT16 *pRxBuf)
{
    const AMBA_SPI_REG_s *pSpiReg = pAmbaSPI_MasterReg[MasterID];
    UINT32 RetVal = SPI_ERR_NONE;

    if ((MasterID >= AMBA_NUM_SPI_MASTER) || (BufSize == 0U) || ((pTxBuf == NULL) && (pRxBuf == NULL))) {
        RetVal = SPI_ERR_ARG;
    } else {
        if (AmbaCSL_SpiGetDataFrameSize(pSpiReg) <= 8U) {
            RetVal = SPI_ERR_CFG;
        } else {
            SPI_MasterSetBufCtrl(MasterID, BufSize, pTxBuf, pRxBuf);
            SPI_MasterStart(MasterID, BufSize);
        }
    }

    return RetVal;
}

/**
 *  AmbaRTSL_SpiMasterDmaTranD8 - SPI Master write and read data operation with DMA
 *  @param[in] MasterID Indicate a SPI master
 *  @param[in] BufSize The size of TX and RX buffer if exist
 *  @param[in] pTxBuf TX buffer. If this pointer is NULL, the signal on SDO/MOSI pin will be ignored.
 *  @param[out] pRxBuf RX buffer. If this pointer is NULL, the signal on SDI/MISO pin will be ignored.
 *  @return error code
 */
UINT32 AmbaRTSL_SpiMasterDmaTranD8(UINT32 MasterID, UINT32 BufSize, const UINT8 *pTxBuf, UINT8 *pRxBuf)
{
    const AMBA_SPI_REG_s *pSpiReg = pAmbaSPI_MasterReg[MasterID];
    UINT32 RetVal = SPI_ERR_NONE;

    if ((MasterID >= AMBA_NUM_SPI_MASTER) || (BufSize == 0U) || ((pTxBuf == NULL) && (pRxBuf == NULL))) {
        RetVal = SPI_ERR_ARG;
    } else {
        if (AmbaCSL_SpiGetDataFrameSize(pSpiReg) > 8U) {
            RetVal = SPI_ERR_CFG;
        } else {
            SPI_MasterSetBufCtrl(MasterID, BufSize, pTxBuf, pRxBuf);
            SPI_MasterDmaStart(MasterID, BufSize);
        }
    }

    return RetVal;
}

/**
 *  AmbaRTSL_SpiMasterDmaTranD16 - SPI Master write and read data operation with DMA
 *  @param[in] MasterID Indicate a SPI master
 *  @param[in] BufSize The size of TX and RX buffer if exist
 *  @param[in] pTxBuf TX buffer. If this pointer is NULL, the signal on SDO/MOSI pin will be ignored.
 *  @param[out] pRxBuf RX buffer. If this pointer is NULL, the signal on SDI/MISO pin will be ignored.
 *  @return error code
 */
UINT32 AmbaRTSL_SpiMasterDmaTranD16(UINT32 MasterID, UINT32 BufSize, const UINT16 *pTxBuf, UINT16 *pRxBuf)
{
    const AMBA_SPI_REG_s *pSpiReg = pAmbaSPI_MasterReg[MasterID];
    UINT32 RetVal = SPI_ERR_NONE;

    if ((MasterID >= AMBA_NUM_SPI_MASTER) || (BufSize == 0U) || ((pTxBuf == NULL) && (pRxBuf == NULL))) {
        RetVal = SPI_ERR_ARG;
    } else {
        if (AmbaCSL_SpiGetDataFrameSize(pSpiReg) <= 8U) {
            RetVal = SPI_ERR_CFG;
        } else {
            SPI_MasterSetBufCtrl(MasterID, BufSize, pTxBuf, pRxBuf);
            SPI_MasterDmaStart(MasterID, BufSize);
        }
    }

    return RetVal;
}

/**
 *  AmbaRTSL_SpiSlaveTransferD8 - SPI Slave write and read data operation
 *  @param[in] SlaveID Indicate a SPI slave
 *  @param[in] BufSize The size of TX and RX buffer if exist
 *  @param[in] pTxBuf TX buffer. If this pointer is NULL, the signal on MISO pin will be ignored.
 *  @param[out] pRxBuf RX buffer. If this pointer is NULL, the signal on MOSI pin will be ignored.
 *  @return error code
 */
UINT32 AmbaRTSL_SpiSlaveTransferD8(UINT32 SlaveID, UINT32 BufSize, const UINT8 *pTxBuf, UINT8 *pRxBuf)
{
    const AMBA_SPI_REG_s *pSpiReg = pAmbaSPI_SlaveReg[SlaveID];
    UINT32 RetVal = SPI_ERR_NONE;

    if ((SlaveID >= AMBA_NUM_SPI_SLAVE) || (BufSize == 0U) || ((pTxBuf == NULL) && (pRxBuf == NULL))) {
        RetVal = SPI_ERR_ARG;
    } else {
        if (AmbaCSL_SpiGetDataFrameSize(pSpiReg) > 8U) {
            RetVal = SPI_ERR_CFG;
        } else {
            SPI_SlaveSetBufCtrl(SlaveID, BufSize, pTxBuf, pRxBuf);
            SPI_SlaveStart(SlaveID, BufSize);
        }
    }

    return RetVal;
}

/**
 *  AmbaRTSL_SpiSlaveTransferD16 - SPI Slave write and read data operation
 *  @param[in] SlaveID Indicate a SPI slave
 *  @param[in] BufSize The size of TX and RX buffer if exist
 *  @param[in] pTxBuf TX buffer. If this pointer is NULL, the signal on MISO pin will be ignored.
 *  @param[out] pRxBuf RX buffer. If this pointer is NULL, the signal on MOSI pin will be ignored.
 *  @return error code
 */
UINT32 AmbaRTSL_SpiSlaveTransferD16(UINT32 SlaveID, UINT32 BufSize, const UINT16 *pTxBuf, UINT16 *pRxBuf)
{
    const AMBA_SPI_REG_s *pSpiReg = pAmbaSPI_SlaveReg[SlaveID];
    UINT32 RetVal = SPI_ERR_NONE;

    if ((SlaveID >= AMBA_NUM_SPI_SLAVE) || (BufSize == 0U) || ((pTxBuf == NULL) && (pRxBuf == NULL))) {
        RetVal = SPI_ERR_ARG;
    } else {
        if (AmbaCSL_SpiGetDataFrameSize(pSpiReg) <= 8U) {
            RetVal = SPI_ERR_CFG;
        } else {
            SPI_SlaveSetBufCtrl(SlaveID, BufSize, pTxBuf, pRxBuf);
            SPI_SlaveStart(SlaveID, BufSize);
        }
    }

    return RetVal;
}

/**
 *  AmbaRTSL_SpiSlaveDmaTranD8 - SPI Slave write and read data operation with DMA
 *  @param[in] SlaveID Indicate a SPI slave
 *  @param[in] BufSize The size of TX and RX buffer if exist
 *  @param[in] pTxBuf TX buffer. If this pointer is NULL, the signal on MISO pin will be ignored.
 *  @param[out] pRxBuf RX buffer. If this pointer is NULL, the signal on MOSI pin will be ignored.
 *  @return error code
 */
UINT32 AmbaRTSL_SpiSlaveDmaTranD8(UINT32 SlaveID, UINT32 BufSize, const UINT8 *pTxBuf, UINT8 *pRxBuf)
{
    const AMBA_SPI_REG_s *pSpiReg = pAmbaSPI_SlaveReg[SlaveID];
    UINT32 RetVal = SPI_ERR_NONE;

    if ((SlaveID >= AMBA_NUM_SPI_SLAVE) || (BufSize == 0U) || ((pTxBuf == NULL) && (pRxBuf == NULL))) {
        RetVal = SPI_ERR_ARG;
    } else {
        if (AmbaCSL_SpiGetDataFrameSize(pSpiReg) > 8U) {
            RetVal = SPI_ERR_CFG;
        } else {
            SPI_SlaveSetBufCtrl(SlaveID, BufSize, pTxBuf, pRxBuf);
            SPI_SlaveDmaStart(SlaveID, BufSize);
        }
    }

    return RetVal;
}

/**
 *  AmbaRTSL_SpiSlaveDmaTranD16 - SPI Slave write and read data operation with DMA
 *  @param[in] SlaveID Indicate a SPI slave
 *  @param[in] BufSize The size of TX and RX buffer if exist
 *  @param[in] pTxBuf TX buffer. If this pointer is NULL, the signal on MISO pin will be ignored.
 *  @param[out] pRxBuf RX buffer. If this pointer is NULL, the signal on MOSI pin will be ignored.
 *  @return error code
 */
UINT32 AmbaRTSL_SpiSlaveDmaTranD16(UINT32 SlaveID, UINT32 BufSize, const UINT16 *pTxBuf, UINT16 *pRxBuf)
{
    const AMBA_SPI_REG_s *pSpiReg = pAmbaSPI_SlaveReg[SlaveID];
    UINT32 RetVal = SPI_ERR_NONE;

    if ((SlaveID >= AMBA_NUM_SPI_SLAVE) || (BufSize == 0U) || ((pTxBuf == NULL) && (pRxBuf == NULL))) {
        RetVal = SPI_ERR_ARG;
    } else {
        if (AmbaCSL_SpiGetDataFrameSize(pSpiReg) <= 8U) {
            RetVal = SPI_ERR_CFG;
        } else {
            SPI_SlaveSetBufCtrl(SlaveID, BufSize, pTxBuf, pRxBuf);
            SPI_SlaveDmaStart(SlaveID, BufSize);
        }
    }

    return RetVal;
}

/**
 *  AmbaRTSL_SpiMasterStop - Stop SPI transactions
 *  @param[in] MasterID Indicate a SPI master
 */
void AmbaRTSL_SpiMasterStop(UINT32 MasterID)
{
    AMBA_SPI_REG_s *pSpiReg = pAmbaSPI_MasterReg[MasterID];
    AMBA_SPI_CTRL_s *pSpiCtrl = &AmbaSpiMasterCtrl[MasterID];

    AmbaCSL_SpiDmaDisable(pSpiReg);
    AmbaCSL_SpiSlaveDisableAll(pSpiReg);
    AmbaCSL_SpiDisableAllIRQ(pSpiReg);
    AmbaCSL_SpiDisable(pSpiReg);

    pSpiCtrl->BaudRate = 0U;
}

/**
 *  AmbaRTSL_SpiSlaveStop - Stop SPI transactions
 *  @param[in] SlaveID Indicate a SPI slave
 */
void AmbaRTSL_SpiSlaveStop(UINT32 SlaveID)
{
    AMBA_SPI_REG_s *pSpiReg = pAmbaSPI_SlaveReg[SlaveID];
    AMBA_SPI_CTRL_s *pSpiCtrl = &AmbaSpiSlaveCtrl[SlaveID];

    AmbaCSL_SpiDmaDisable(pSpiReg);
    AmbaCSL_SpiSlaveDisableAll(pSpiReg);
    AmbaCSL_SpiDisableAllIRQ(pSpiReg);
    AmbaCSL_SpiDisable(pSpiReg);

    pSpiCtrl->BaudRate = 0U;
}

/**
 *  AmbaRTSL_SpiMasterGetInfo - The function returns all the information about the master.
 *  @param[in] MasterID Indicate a SPI master
 *  @param[out] pNumSlaves Number of slave support of the master
 *  @param[out] pStatus The current bus status
 *  @param[out] pTransCount Number of transfered data
 *  @return error code
 */
UINT32 AmbaRTSL_SpiMasterGetInfo(UINT32 MasterID, UINT32 *pNumSlaves, UINT32 *pStatus, UINT32 *pTransCount)
{
    const AMBA_SPI_CTRL_s *pSpiCtrl;
    const AMBA_SPI_REG_s *pSpiReg;
    UINT32 RegVal, RetVal = SPI_ERR_NONE;

    if ((MasterID >= AMBA_NUM_SPI_MASTER) || ((pNumSlaves == NULL) && (pStatus == NULL) && (pTransCount == NULL))) {
        RetVal = SPI_ERR_ARG;
    } else {
        if (pNumSlaves != NULL) {
            pSpiCtrl = &(AmbaSpiMasterCtrl[MasterID]);
            *pNumSlaves = pSpiCtrl->MaxNumSlaveSelect;
        }

        if (pStatus != NULL) {
            pSpiReg = pAmbaSPI_MasterReg[MasterID];
            RegVal = AmbaCSL_SpiGetStat(pSpiReg);

            if ((RegVal & 0x60U) != 0x0U) {
                *pStatus = SPI_STATUS_ERROR;
            } else if (AmbaCSL_SpiGetStatBusy(pSpiReg) == 0x0U) {
                *pStatus = SPI_STATUS_IDLE;
            } else {
                *pStatus = SPI_STATUS_BUSY;
            }
        }

        if (pTransCount != NULL) {
            pSpiCtrl = &(AmbaSpiMasterCtrl[MasterID]);
            if (pSpiCtrl->TxBufIdx >= pSpiCtrl->RxBufIdx) {
                *pTransCount = pSpiCtrl->TxBufIdx;
            } else {
                *pTransCount = pSpiCtrl->RxBufIdx;
            }
        }
    }

    return RetVal;
}

/**
 *  AmbaRTSL_SpiSlaveGetInfo - The function returns all the information about the slave.
 *  @param[in] SlaveID Indicate a SPI slave
 *  @param[out] pStatus The current bus status
 *  @param[out] pTransCount Number of transfered data
 *  @return error code
 */
UINT32 AmbaRTSL_SpiSlaveGetInfo(UINT32 SlaveID, UINT32 *pStatus, UINT32 *pTransCount)
{
    const AMBA_SPI_CTRL_s *pSpiCtrl;
    const AMBA_SPI_REG_s *pSpiReg;
    UINT32 RegVal, RetVal = SPI_ERR_NONE;

    if ((SlaveID >= AMBA_NUM_SPI_SLAVE) || ((pStatus == NULL) && (pTransCount == NULL))) {
        RetVal = SPI_ERR_ARG;
    } else {
        if (pStatus != NULL) {
            pSpiReg = pAmbaSPI_SlaveReg[SlaveID];
            RegVal = AmbaCSL_SpiGetStat(pSpiReg);

            if ((RegVal & 0x60U) != 0x0U) {
                *pStatus = SPI_STATUS_ERROR;
            } else if (AmbaCSL_SpiGetStatBusy(pSpiReg) == 0x0U) {
                *pStatus = SPI_STATUS_IDLE;
            } else {
                *pStatus = SPI_STATUS_BUSY;
            }
        }

        if (pTransCount != NULL) {
            pSpiCtrl = &(AmbaSpiSlaveCtrl[SlaveID]);
            if (pSpiCtrl->TxBufIdx >= pSpiCtrl->RxBufIdx) {
                *pTransCount = pSpiCtrl->TxBufIdx;
            } else {
                *pTransCount = pSpiCtrl->RxBufIdx;
            }
        }
    }

    return RetVal;
}

/**
 *  AmbaRTSL_SpiMasterGetPortAddr - Get data port register address for dma
 *  @param[in] MasterID Indicate a SPI master
 *  @param[out] pDataPortAddr Data port register address
 */
void AmbaRTSL_SpiMasterGetPortAddr(UINT32 MasterID, ULONG *pDataPortAddr)
{
    const AMBA_SPI_REG_s *pSpiReg = pAmbaSPI_MasterReg[MasterID];
    volatile const UINT32 *pDataBuf;

    /* Destination is to SPI buf registers */
    pDataBuf = &pSpiReg->DataBuf[0];
    AmbaMisra_TypeCast(pDataPortAddr, &pDataBuf);
}

/**
 *  AmbaRTSL_SpiSlaveGetPortAddr - Get data port register address for dma
 *  @param[in] SlaveID Indicate a SPI slave
 *  @param[out] pDataPortAddr Data port register address
 */
void AmbaRTSL_SpiSlaveGetPortAddr(UINT32 SlaveID, ULONG *pDataPortAddr)
{
    const AMBA_SPI_REG_s *pSpiReg = pAmbaSPI_SlaveReg[SlaveID];
    volatile const UINT32 *pDataBuf;

    /* Destination is to SPI buf registers */
    pDataBuf = &pSpiReg->DataBuf[0];
    AmbaMisra_TypeCast(pDataPortAddr, &pDataBuf);
}

/**
 *  AmbaRTSL_SpiSlaveReadFifo - Get slave read data from FIFO
 *  @param[in] SlaveID Indicate a SPI slave
 *  @param[out] ReadData Read data
 */
UINT32 AmbaRTSL_SpiSlaveReadFifo(UINT32 SlaveID)
{
    const AMBA_SPI_REG_s *pSpiReg = pAmbaSPI_SlaveReg[SlaveID];
    UINT32 ReadData;

    if (AmbaCSL_SpiGetDataFrameSize(pSpiReg) <= 8U) {
        ReadData = (AmbaCSL_SpiReadData(pSpiReg) & 0x00ffU);
    } else {
        ReadData = (AmbaCSL_SpiReadData(pSpiReg) & 0xffffU);
    }

    return ReadData;
}

/**
 *  AmbaRTSL_SpiSlaveGetReadSize - Get slave read FIFO size
 *  @param[in] SlaveID Indicate a SPI slave
 *  @param[out] ReadData Read size
 */
UINT32 AmbaRTSL_SpiSlaveGetReadSize(UINT32 SlaveID)
{
    const AMBA_SPI_REG_s *pSpiReg = pAmbaSPI_SlaveReg[SlaveID];
    UINT32 ReadSize;

    ReadSize = AmbaCSL_SpiGetRxFifoDataSize(pSpiReg);

    return ReadSize;
}

#pragma GCC push_options
#pragma GCC target("general-regs-only")
/**
 *  SPI_MasterISR - SPI ISR
 *  @param[in] Irq Interrupt ID
 *  @param[in] SpiChanNo Indicate a SPI master
 */
void SPI_MasterISR(UINT32 Irq, UINT32 SpiChanNo)
{
    AMBA_SPI_REG_s *pSpiReg = pAmbaSPI_MasterReg[SpiChanNo];
    AMBA_SPI_CTRL_s *pSpiCtrl = (AMBA_SPI_CTRL_s *) & (AmbaSpiMasterCtrl[SpiChanNo]);
    UINT32 i, RxFifoLv;

    AmbaMisra_TouchUnused(&Irq);

    AmbaCSL_SpiClearIrqStatus(pSpiReg);
    AmbaCSL_SpiDisableAllIRQ(pSpiReg);

    if (pSpiCtrl->TxDataSize > 0U) {
        /* Triggered by RxFIFO half or TxFIFO half IRQ. */
        /* For [total Tx data size] > [Max FIFO size] case */

        for (i = 0U; i < (pSpiCtrl->MaxNumFifoEntry / 2U); i++) {
            if (pSpiCtrl->RxDataSize > 0U) {
                SPI_ReadFIFO(pSpiReg, 1U, pSpiCtrl->pRxDataBuf, &pSpiCtrl->RxBufIdx);
                pSpiCtrl->RxDataSize--;
            }

            SPI_WriteFIFO(pSpiReg, 1U, pSpiCtrl->pTxDataBuf, &pSpiCtrl->TxBufIdx);
            pSpiCtrl->TxDataSize--;

            if (pSpiCtrl->TxDataSize == 0U) {
                break;
            }
        }

        if (pSpiCtrl->pRxDataBuf == NULL) {
            if (pSpiCtrl->TxDataSize > 0U) {
                AmbaCSL_SpiEnableTxFifoEmptyIRQ(pSpiReg);   /* Need to TX again */
            } else {
                AmbaCSL_SpiEnableTransDoneIRQ(pSpiReg);     /* No more TX/RX is needed */
            }
        } else {
            AmbaCSL_SpiEnableRxFifoFullIRQ(pSpiReg);        /* Need to RX */
        }
    } else {
        if (pSpiCtrl->pRxDataBuf == NULL) { /* for Tx only case */
            if (AmbaCSL_SpiGetStatBusy(pSpiReg) != 0U) {
                /* there are still data in TX FIFO */
                AmbaCSL_SpiEnableTxFifoEmptyIRQ(pSpiReg);
            } else {
                if (AmbaSpiMasterIsrCb != NULL) {
                    AmbaSpiMasterIsrCb(SpiChanNo);
                }
            }
        } else {                            /* for Tx/Rx and Rx only case */
            RxFifoLv = (UINT32)AmbaCSL_SpiGetRxFifoThreshold(pSpiReg) + 1U;

            if (pSpiCtrl->RxDataSize > RxFifoLv) {
                SPI_ReadFIFO(pSpiReg, RxFifoLv, pSpiCtrl->pRxDataBuf, &pSpiCtrl->RxBufIdx);
                pSpiCtrl->RxDataSize -= RxFifoLv;
                AmbaCSL_SpiEnableRxFifoFullIRQ(pSpiReg);
            } else {
                SPI_ReadFIFO(pSpiReg, pSpiCtrl->RxDataSize, pSpiCtrl->pRxDataBuf, &pSpiCtrl->RxBufIdx);
                pSpiCtrl->RxDataSize = 0U;

                if (AmbaSpiMasterIsrCb != NULL) {
                    AmbaSpiMasterIsrCb(SpiChanNo);
                }
            }
        }
    }
}

/**
 *  SPI_SlaveISR - SPI Slave ISR
 *  @param[in] Irq Interrupt ID
 *  @param[in] SpiChanNo Indicate a SPI slave
 */
void SPI_SlaveISR(UINT32 Irq, UINT32 SpiChanNo)
{
    AMBA_SPI_REG_s *pSpiReg = pAmbaSPI_SlaveReg[SpiChanNo];
    AMBA_SPI_CTRL_s *pSpiCtrl = (AMBA_SPI_CTRL_s *) & (AmbaSpiSlaveCtrl[SpiChanNo]);
    UINT32 i, RxFifoLv;
    UINT32 TxBufIdx;
    UINT32 BusyState, TxFifoLv;

    AmbaMisra_TouchUnused(&Irq);

    AmbaCSL_SpiClearIrqStatus(pSpiReg);
    AmbaCSL_SpiDisableAllIRQ(pSpiReg);

    if ((pSpiCtrl->TxDataSize == 1U) && (pSpiCtrl->RxDataSize == pSpiCtrl->MaxNumFifoEntry)) {
        /* Triggered by RxFIFO half or TxFIFO half IRQ. */
        /* For [total Tx data size] == [Max FIFO size] case */
        /* Fill TX FIFO with the last data */
        SPI_WriteFIFO(pSpiReg, 1U, pSpiCtrl->pTxDataBuf, &pSpiCtrl->TxBufIdx);
        pSpiCtrl->TxDataSize = 0U;

        /* Fill the TX FIFO with redundant data to avoid emptying the FIFO. */
        TxBufIdx = 0U;
        SPI_WriteFIFO(pSpiReg, 1U, pSpiCtrl->pTxDataBuf, &TxBufIdx);

        if (pSpiCtrl->pRxDataBuf == NULL) {
            AmbaCSL_SpiEnableTxFifoEmptyIRQ(pSpiReg);       /* No more TX/RX is needed */
        } else {
            AmbaCSL_SpiEnableRxFifoFullIRQ(pSpiReg);        /* Need to RX */
        }
    } else if (pSpiCtrl->TxDataSize > 0U) {
        /* Triggered by RxFIFO half or TxFIFO half IRQ. */
        /* For [total Tx data size] > [Max FIFO size] case */

        for (i = 0U; i < (pSpiCtrl->MaxNumFifoEntry / 2U); i++) {
            if (pSpiCtrl->RxDataSize > 0U) {
                SPI_ReadFIFO(pSpiReg, 1U, pSpiCtrl->pRxDataBuf, &pSpiCtrl->RxBufIdx);
                pSpiCtrl->RxDataSize--;
            }

            SPI_WriteFIFO(pSpiReg, 1U, pSpiCtrl->pTxDataBuf, &pSpiCtrl->TxBufIdx);
            pSpiCtrl->TxDataSize--;

            if (pSpiCtrl->TxDataSize == 0U) {
                /* Fill the TX FIFO with redundant data to avoid emptying the FIFO. */
                /* For [total Tx data size] > [Max FIFO size] case */
                TxBufIdx = 0U;
                SPI_WriteFIFO(pSpiReg, 1U, pSpiCtrl->pTxDataBuf, &TxBufIdx);
                break;
            }
        }

        if (pSpiCtrl->pRxDataBuf == NULL) {
            AmbaCSL_SpiEnableTxFifoEmptyIRQ(pSpiReg);       /* Need to TX again */
        } else {
            AmbaCSL_SpiEnableRxFifoFullIRQ(pSpiReg);        /* Need to RX */
        }
    } else {
        if (pSpiCtrl->pRxDataBuf == NULL) { /* for Tx only case */
            BusyState = AmbaCSL_SpiGetStatBusy(pSpiReg);
            TxFifoLv = AmbaCSL_SpiGetTxFifoDataSize(pSpiReg);
            if ((BusyState != 0U) || (TxFifoLv > 1U)) {
                /* there are still data in TX FIFO */
                AmbaCSL_SpiEnableTxFifoEmptyIRQ(pSpiReg);
            } else {
                if (AmbaSpiSlaveIsrCb != NULL) {
                    AmbaSpiSlaveIsrCb(SpiChanNo);
                }
            }
        } else {                            /* for Tx/Rx and Rx only case */
            RxFifoLv = (UINT32)AmbaCSL_SpiGetRxFifoThreshold(pSpiReg) + 1U;

            if (pSpiCtrl->RxDataSize > RxFifoLv) {
                SPI_ReadFIFO(pSpiReg, RxFifoLv, pSpiCtrl->pRxDataBuf, &pSpiCtrl->RxBufIdx);
                pSpiCtrl->RxDataSize -= RxFifoLv;
                AmbaCSL_SpiEnableRxFifoFullIRQ(pSpiReg);
            } else {
                SPI_ReadFIFO(pSpiReg, pSpiCtrl->RxDataSize, pSpiCtrl->pRxDataBuf, &pSpiCtrl->RxBufIdx);
                pSpiCtrl->RxDataSize = 0U;

                if (AmbaSpiSlaveIsrCb != NULL) {
                    AmbaSpiSlaveIsrCb(SpiChanNo);
                }
            }
        }
    }
}
#pragma GCC pop_options
