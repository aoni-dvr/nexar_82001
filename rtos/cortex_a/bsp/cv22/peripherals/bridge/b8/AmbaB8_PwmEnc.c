/**
 *  @file AmbaB8_PwmEnc.c
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
 *  @details B8 PWM Encoder CSL Device Driver
 *
 */
#include "AmbaB8.h"
#include "AmbaB8_IoMap.h"
#include "AmbaB8_Communicate.h"

#include "AmbaB8_PwmEnc.h"
#include "AmbaB8CSL_PwmEnc.h"

#define POLLING_TIMEOUT 10000U
#define TRANSACTION_HEADR_SIZE      4U

/* Header consists of 16-bit command and 16-bit address + 124-byte data */
static UINT8 B8nTxBuf[128];/* TX buffer for communication with B8N (and B8F through PWM) */

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: B8_PrepareTransactionHeader
 *
 *  @Description:: Prepare command and address for SPI/I2C communication with B8
 *
 *  @Input      ::
 *      IsWrite:   Write(1) or Read(0)
 *      SubChipID: B8 sub-chip id
 *      RegAddr:   The first register address for write/read
 *      FixedAddr: Auto-increment register address after one data read/write
 *      DataWidth: Data unit is 8/16/32-bit
 *      DataSize:  Number of following data
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void B8_PrepareTransactionHeader(UINT32 IsWrite, UINT32 ChipID, UINT32 RegAddr, UINT32 AddrIncrement, UINT32 DataWidth, UINT32 DataSize)
{
    B8_COMMAND_s SpiCommand;
    UINT16 u16Data;

    if (RegAddr >= B8_APB_BASE_ADDR) {
        SpiCommand.IsAPB = 1;
    } else {
        SpiCommand.IsAPB = 0;
    }

    SpiCommand.DataWidth = (UINT8)DataWidth;
    SpiCommand.DataSize = (UINT8)(DataSize - 1U);
    SpiCommand.AddrInc = (UINT8)AddrIncrement;
    SpiCommand.ReadOrWrite = (UINT8)IsWrite;

    if ((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK) != 0U) {
        if (AmbaB8_GetSubChipCount(ChipID) > 1U) {
            SpiCommand.ChipID = 0xfU;   /* multicast_cid */
        } else {
            SpiCommand.ChipID = (UINT8)(ChipID & B8_SUB_CHIP_ID_MASK);
        }
    } else {  /* if ((ChipID & B8_MAIN_CHIP_ID_B8ND_MASK) != 0U){ */
        SpiCommand.ChipID = (UINT8)(ChipID & B8_SUB_CHIP_ID_MASK);
    }

    (void) AmbaB8_Wrap_memcpy(&u16Data, &SpiCommand, sizeof(UINT16));

    /* 16-bit MSB-first command */
    B8nTxBuf[0] = (UINT8)((u16Data >> 8) & 0xffU);
    B8nTxBuf[1] = (UINT8)(u16Data & 0xffU);

    /* 16-bit MSB-first address */
    B8nTxBuf[2] = (UINT8)((RegAddr >> 8) & 0xffU);
    B8nTxBuf[3] = (UINT8)(RegAddr & 0xffU);
}

static UINT32 B8_RegWrite(UINT32 ChipID, const volatile void *pReg, UINT8 AddrIncrement,
                          UINT32 DataWidth, UINT32 DataSize, const void *pTxDataBuf)
{
    UINT32 RetVal = B8_ERR_NONE;
    UINT32 ActualSize;
    UINT32 RegAddr;
    UINT32 B8nTxDataSize = TRANSACTION_HEADR_SIZE;
    UINT8 *pTxData = &B8nTxBuf[TRANSACTION_HEADR_SIZE];

    const UINT32 *pWordData;
    const UINT16 *pHalfWordData;
    const UINT8  *pByteData;

    if (pTxDataBuf == NULL) {
        RetVal = B8_ERR_ARG;

    } else {
        AmbaB8_Misra_TypeCast32(&RegAddr, &pReg);
        B8_PrepareTransactionHeader(B8_REG_WRITE, ChipID, RegAddr, AddrIncrement, DataWidth, DataSize);

        if (DataWidth <= B8_DATA_WIDTH_32BIT) {
            if (DataWidth == B8_DATA_WIDTH_32BIT) {
                AmbaB8_Misra_TypeCast32(&pWordData, &pTxDataBuf);

                B8nTxDataSize += (UINT32)(DataSize << 2);
                while (DataSize > 0U) {
                    /* 32 bits data */
                    *pTxData = (UINT8)(*pWordData >> 24);
                    pTxData ++;
                    *pTxData = (UINT8)((*pWordData >> 16) & 0xffU);
                    pTxData ++;
                    *pTxData = (UINT8)((*pWordData >> 8) & 0xffU);
                    pTxData ++;
                    *pTxData = (UINT8)((*pWordData & 0xffU));
                    pTxData ++;
                    pWordData ++;
                    DataSize --;
                }
            } else if (DataWidth == B8_DATA_WIDTH_16BIT) {
                AmbaB8_Misra_TypeCast32(&pHalfWordData, &pTxDataBuf);

                B8nTxDataSize += (UINT32)(DataSize << 1);
                while (DataSize > 0U) {
                    /* 16 bits data */
                    *pTxData = (UINT8)((*pHalfWordData >> 8));
                    pTxData ++;
                    *pTxData = (UINT8)((*pHalfWordData & 0xffU));
                    pTxData ++;

                    pHalfWordData ++;
                    DataSize --;
                }
            } else { /* if (DataWidth == B8_DATA_WIDTH_8BIT) { */
                AmbaB8_Misra_TypeCast32(&pByteData, &pTxDataBuf);

                B8nTxDataSize += (UINT32)DataSize;
                while (DataSize > 0U) {
                    /* 8 bits data */
                    *pTxData = (UINT8)(*pByteData);
                    pTxData ++;
                    pByteData ++;
                    DataSize --;
                }
            }
            if (AmbaB8_SPI_MasterTransfer(ChipID, B8nTxDataSize, B8nTxBuf, NULL, &ActualSize, 1000U) != B8_ERR_NONE) {
                RetVal = B8_ERR_COMMUNICATE;
            } else {
                RetVal = B8_ERR_NONE;
            }
        } else {
            RetVal = B8_ERR_ARG;
        }
    }

    return RetVal;
}

static UINT32 B8_RegRead(UINT32 ChipID, const volatile void *pReg, UINT8 AddrIncrement,
                         UINT32 DataWidth, UINT32 DataSize, void * pRxDataBuf)
{
    UINT32 RetVal = B8_ERR_NONE;
    UINT32 ActualSize;
    UINT32 RegAddr;
    UINT8 B8nRxBuf[128];                                           /* RX buffer for communication with B8N (and B8F through PWM) */
    const UINT8 *pRxData = &B8nRxBuf[TRANSACTION_HEADR_SIZE+1U];   /* Data 0 is a dummy byte */

    UINT8   *pByteData;
    UINT16  *pHalfWordData;
    UINT32  *pWordData;

    AmbaB8_Misra_TypeCast32(&RegAddr, &pReg);
    B8_PrepareTransactionHeader(B8_REG_READ, ChipID, RegAddr, AddrIncrement, DataWidth, DataSize);

    if ((ChipID & B8_SUB_CHIP_ID_MASK) != B8_SUB_CHIP_ID_B8N) {
        /* B8F will not immediately return values */
        if (DataWidth <= B8_DATA_WIDTH_32BIT) {
            RetVal = AmbaB8_SPI_MasterTransfer(ChipID, 4U, B8nTxBuf, NULL, &ActualSize, 1000U);
        } else {
            RetVal = B8_ERR_ARG;
        }
    } else { /* read B8N registers */
        if (pRxDataBuf == NULL) {
            RetVal = B8_ERR_ARG;
        } else {
            AmbaB8_Misra_TouchUnused(pRxDataBuf);

            if (DataWidth == B8_DATA_WIDTH_32BIT) {
                RetVal = AmbaB8_SPI_MasterTransfer(ChipID, 4U + 1U + (DataSize << 2), B8nTxBuf, B8nRxBuf, &ActualSize, 1000U);
                AmbaB8_Misra_TypeCast32(&pWordData, &pRxDataBuf);
                while (DataSize > 0U) {
                    /* 32 bits data */
                    *pWordData = *pRxData;
                    pRxData ++;
                    *pWordData = (*pWordData << 8) | *pRxData;
                    pRxData ++;
                    *pWordData = (*pWordData << 8) | *pRxData;
                    pRxData ++;
                    *pWordData = (*pWordData << 8) | *pRxData;
                    pRxData ++;
                    pWordData ++;
                    DataSize --;
                }

            } else if (DataWidth == B8_DATA_WIDTH_16BIT) {
                RetVal = AmbaB8_SPI_MasterTransfer(ChipID, 4U + 1U + (DataSize << 1), B8nTxBuf, B8nRxBuf, &ActualSize, 1000U);
                AmbaB8_Misra_TypeCast32(&pHalfWordData, &pRxDataBuf);
                while (DataSize > 0U) {
                    /* 16 bits data */
                    *pHalfWordData = *pRxData;
                    pRxData ++;
                    *pHalfWordData = (UINT16)(*pHalfWordData << 8) | *pRxData;
                    pRxData ++;
                    pHalfWordData ++;
                    DataSize --;
                }

            } else if (DataWidth == B8_DATA_WIDTH_8BIT) {
                RetVal = AmbaB8_SPI_MasterTransfer(ChipID, 4U + 1U + DataSize, B8nTxBuf, B8nRxBuf, &ActualSize, 1000U);
                AmbaB8_Misra_TypeCast32(&pByteData, &pRxDataBuf);
                while (DataSize > 0U) {
                    /* 8 bits data */
                    *pByteData = *pRxData;
                    pRxData ++;
                    pByteData ++;
                    DataSize --;
                }
            } else {
                RetVal = B8_ERR_ARG;
            }
        }
    }

    return RetVal;
}

static UINT32 B8N_RegWrite(UINT32 ChipID, const volatile void *pReg, UINT8 AddrIncrement, UINT32 DataWidth, UINT32 DataSize, const void *pTxDataBuf)
{
    UINT32 RetVal = B8_ERR_NONE;

    if ((ChipID & B8_MAIN_CHIP_ID_B8N_MASK) != 0U) {
        if ((ChipID & B8_SUB_CHIP_ID_MASK) == B8_SUB_CHIP_ID_B8N) { /* B8N */
            RetVal = B8_RegWrite(ChipID, pReg, AddrIncrement, DataWidth, DataSize, pTxDataBuf);

        } else { /* B8NF or B8ND */
            B8_PWM_ENC_CTRL_REG_s PwmCtrl = {0};
            UINT32 PwmIntStatus[2];
            UINT32 SysTickStamp0 = 0, SysTickStamp1 = 0;
            UINT8 PollingDone = 0U;

            /* Set Multicast ID, B8NF only */
            if (((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK) != 0U) && (AmbaB8_GetSubChipCount(ChipID) > 1U)) {
                RetVal |= B8_RegRead((ChipID & B8_MAIN_CHIP_ID_B8N_MASK), & pAmbaB8_PwmEncReg->Ctrl, 0, B8_DATA_WIDTH_32BIT, 1, &PwmCtrl);
                PwmCtrl.MultiCastID = (UINT16)(ChipID & B8_SUB_CHIP_ID_MASK);
                RetVal |= B8_RegWrite((ChipID & B8_MAIN_CHIP_ID_B8N_MASK), & pAmbaB8_PwmEncReg->Ctrl, 0, B8_DATA_WIDTH_32BIT, 1, &PwmCtrl);
            }

            /* Step 1: Request to write B8F register */
            RetVal |= B8_RegWrite(ChipID, pReg, AddrIncrement, DataWidth, DataSize, pTxDataBuf);

            /* Step 2: Polling B8N PwmEncIdle flag */
            if (RetVal == B8_ERR_NONE) {

                (void) AmbaB8_KAL_GetSysTickCount(&SysTickStamp0);

                do {
                    (void) B8_RegRead((ChipID & B8_MAIN_CHIP_ID_B8N_MASK), & pAmbaB8_PwmEncReg->Ctrl, 0, B8_DATA_WIDTH_32BIT, 1, &PwmCtrl);

                    if (PwmCtrl.EncIdle == 1U) {
                        PollingDone = 1U;

                    } else {
                        /* Check SysTicks Difference */
                        (void) AmbaB8_KAL_GetSysTickCount(&SysTickStamp1);
                        SysTickStamp1 -= SysTickStamp0;

                        if (SysTickStamp1 >= B8_PWMENC_POLLING_START) {
                            (void) B8_RegRead((ChipID & B8_MAIN_CHIP_ID_B8N_MASK), & pAmbaB8_PwmEncReg->ErrorIntStatus0, 1, B8_DATA_WIDTH_32BIT, 2, PwmIntStatus);

                            if (((PwmIntStatus[0] & 0x10030000U) != 0U) ||
                                (SysTickStamp1 >= B8_PWMENC_POLLING_TIMEOUT)) {
                                AmbaB8_PrintUInt5("B8(D)F Write TimeOut! (err_int_sts0 = 0x%08X, err_int_sts1 = 0x%08X)", PwmIntStatus[0], PwmIntStatus[1], 0U, 0U, 0U);
                                RetVal = B8_ERR_SERDES_LINK;
                                PollingDone = 1U;
                            } else {
                                (void) AmbaB8_KAL_TaskSleep(1);
                            }
                        }
                    }
                } while(PollingDone == 0U);
            } else {
                /* access fail before request to write B8F register*/
            }
        }
    } else {
        RetVal = B8_ERR_ARG;
    }

    return RetVal;

}

static UINT32 B8N_RegRead(UINT32 ChipID, const volatile void *pReg, UINT8 AddrIncrement, UINT32 DataWidth, UINT32 DataSize, void *pRxDataBuf)
{
    UINT32 RetVal = B8_ERR_NONE;

    if ((ChipID & B8_MAIN_CHIP_ID_B8N_MASK) != 0U) {
        if ((ChipID & B8_SUB_CHIP_ID_MASK) == B8_SUB_CHIP_ID_B8N) { /* B8N */
            RetVal = B8_RegRead(ChipID, pReg, AddrIncrement, DataWidth, DataSize, pRxDataBuf);

        } else { /* B8NF or B8ND */
            B8_PWM_ENC_CTRL_REG_s PwmCtrl = {0};
            UINT32 PwmIntStatus[2];
            UINT32 SysTickStamp0, SysTickStamp1;
            UINT8 PollingDone = 0U;

            /* Todo: Set Multicast ID */

            /* Step 1: Request to read B8F register */
            RetVal = B8_RegRead(ChipID, pReg, AddrIncrement, DataWidth, DataSize, NULL);

            /* Step 2: Polling B8N PwmEncIdle flag */
            if (RetVal == B8_ERR_NONE) {

                (void) AmbaB8_KAL_GetSysTickCount(&SysTickStamp0);

                do {
                    (void) B8_RegRead((ChipID & B8_MAIN_CHIP_ID_B8N_MASK), & pAmbaB8_PwmEncReg->Ctrl, 0, B8_DATA_WIDTH_32BIT, 1, &PwmCtrl);

                    if (PwmCtrl.EncIdle == 1U) {
                        PollingDone = 1U;

                    } else {
                        /* Check SysTicks Difference */
                        (void) AmbaB8_KAL_GetSysTickCount(&SysTickStamp1);
                        SysTickStamp1 -= SysTickStamp0;

                        if (SysTickStamp1 >= B8_PWMENC_POLLING_START) {
                            (void) B8_RegRead((ChipID & B8_MAIN_CHIP_ID_B8N_MASK), & pAmbaB8_PwmEncReg->ErrorIntStatus0, 1, B8_DATA_WIDTH_32BIT, 2, PwmIntStatus);

                            if (((PwmIntStatus[0] & 0x10030000U) != 0U) ||
                                (SysTickStamp1 >= B8_PWMENC_POLLING_TIMEOUT)) {
                                AmbaB8_PrintUInt5("B8(D)F Write TimeOut! (err_int_sts0 = 0x%08X, err_int_sts1 = 0x%08X)", PwmIntStatus[0], PwmIntStatus[1], 0U, 0U, 0U);
                                RetVal = B8_ERR_SERDES_LINK;
                                PollingDone = 1U;
                            } else {
                                (void) AmbaB8_KAL_TaskSleep(1);
                            }
                        }
                    }
                } while(PollingDone == 0U);

                if (RetVal == B8_ERR_NONE) {

                    if((ChipID & B8_MAIN_CHIP_ID_B8ND_MASK) != 0U) {
                        /* Read data from port ch12 */
                        (void) B8_RegRead((ChipID & B8_MAIN_CHIP_ID_B8N_MASK), & pAmbaB8_PwmEncReg->ReadDataPort12, 0, DataWidth, DataSize, pRxDataBuf);
                    } else {
                        /* Read from data from port ch0 */
                        (void) B8_RegRead((ChipID & B8_MAIN_CHIP_ID_B8N_MASK), & pAmbaB8_PwmEncReg->ReadDataPort0, 0, DataWidth, DataSize, pRxDataBuf);
                        /* Todo: Read from data from port ch1 */
                    }
                }
            }
        }

    } else {
        RetVal = B8_ERR_ARG;
    }

    return RetVal;
}

UINT32 AmbaB8_PwmEncClearErrStatus(UINT32 ChipID)
{
    UINT32 RetVal = B8_ERR_NONE;

    B8_PWM_ENC_ERROR_INT_SATUS0_REG_s ErrorIntStatus0 = {0};
    B8_PWM_ENC_ERROR_INT_SATUS1_REG_s ErrorIntStatus1 = {0};

    if ((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK) != 0U) {
        /* reset error status */
        ErrorIntStatus0.RportFifoOverflow0 = 1U;
        ErrorIntStatus0.LsAwportFifoOverflow = 1U;
        ErrorIntStatus0.ReplayFail0 = 1U;
        (void) B8N_RegWrite(ChipID, & pAmbaB8_PwmEncReg->ErrorIntStatus0, 0U, B8_DATA_WIDTH_32BIT, 1U, &ErrorIntStatus0);

        ErrorIntStatus1.SeqIDError0 = 1U;
        ErrorIntStatus1.NoisyConn0 = 1U;
        (void) B8N_RegWrite(ChipID, & pAmbaB8_PwmEncReg->ErrorIntStatus1, 0U, B8_DATA_WIDTH_32BIT, 1U, &ErrorIntStatus1);

    } else if ((ChipID & B8_MAIN_CHIP_ID_B8ND_MASK) != 0U) {
        /* reset error status */
        ErrorIntStatus0.RportFifoOverflow12 = 1U;
        ErrorIntStatus0.Hs0AwportFifoOverflow = 1U;
        ErrorIntStatus0.ReplayFail12 = 1U;
        (void) B8N_RegWrite(ChipID, & pAmbaB8_PwmEncReg->ErrorIntStatus0, 0U, B8_DATA_WIDTH_32BIT, 1U, &ErrorIntStatus0);

        ErrorIntStatus1.SeqIDError12 = 1U;
        ErrorIntStatus1.NoisyConn12 = 1U;
        (void) B8N_RegWrite(ChipID, & pAmbaB8_PwmEncReg->ErrorIntStatus1, 0U, B8_DATA_WIDTH_32BIT, 1U, &ErrorIntStatus1);

    } else {
        RetVal = B8_ERR_ARG;
    }

    return RetVal;
}

UINT32 AmbaB8_PwmEncMaskErrStatus(UINT32 ChipID, UINT32 Enable)
{
    UINT32 RetVal = B8_ERR_NONE;
    UINT32 ErrorIntMask0;

    (void) B8N_RegRead(ChipID, & pAmbaB8_PwmEncReg->ErrorIntMask0, 0U, B8_DATA_WIDTH_32BIT, 1U, &ErrorIntMask0);

    if ((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK) != 0U) {
        if (Enable != 0U) {
            ErrorIntMask0 |= (0x00010000U);
        } else {
            ErrorIntMask0 &= ~(0x00010000U);
        }
        (void) B8N_RegWrite(ChipID, & pAmbaB8_PwmEncReg->ErrorIntMask0, 0U, B8_DATA_WIDTH_32BIT, 1U, &ErrorIntMask0);

    } else if ((ChipID & B8_MAIN_CHIP_ID_B8ND_MASK) != 0U) {
        if (Enable != 0U) {
            ErrorIntMask0 |= (0x10000000U);
        } else {
            ErrorIntMask0 &= ~(0x10000000U);
        }
        (void) B8N_RegWrite(ChipID, & pAmbaB8_PwmEncReg->ErrorIntMask0, 0U, B8_DATA_WIDTH_32BIT, 1U, &ErrorIntMask0);

    } else {
        RetVal = B8_ERR_ARG;
    }

    return RetVal;
}


UINT32 AmbaB8_PwmEncTxReset(UINT32 ChipID)
{
    UINT32 RetVal = B8_ERR_NONE;
    UINT32 SysTickStamp0 = 0, SysTickStamp1 = 0;

    B8_PWM_ENC_RESET_CTRL0_REG_s ResetCtrl0 = {0};

    if ((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK) != 0U) {
        ResetCtrl0.TxReset0 = 1U;
        ResetCtrl0.RportFifoReset0 = 1U;
        (void) B8N_RegWrite(ChipID, & pAmbaB8_PwmEncReg->ResetCtrl0, 0U, B8_DATA_WIDTH_32BIT, 1U, &ResetCtrl0);

        /* polling reset tx */
        (void) AmbaB8_KAL_GetSysTickCount(&SysTickStamp0);
        do {
            (void) B8N_RegRead(ChipID, & pAmbaB8_PwmEncReg->ResetCtrl0, 0U, B8_DATA_WIDTH_32BIT, 1U, &ResetCtrl0);
            if ((ResetCtrl0.TxReset0 == 0U) && (ResetCtrl0.RportFifoReset0 == 0U)) {
                RetVal = B8_ERR_NONE;
                break;
            } else {
                RetVal = B8_ERR_SERDES_LINK;
                (void) AmbaB8_KAL_GetSysTickCount(&SysTickStamp1);
            }
        } while ((SysTickStamp1 - SysTickStamp0) < POLLING_TIMEOUT);

    } else if ((ChipID & B8_MAIN_CHIP_ID_B8ND_MASK) != 0U) {
        /* reset tx & awport_fifo */
        ResetCtrl0.TxReset12 = 1U;
        ResetCtrl0.RportFifoReset12 = 1U;
        (void) B8N_RegWrite(ChipID, & pAmbaB8_PwmEncReg->ResetCtrl0, 0U, B8_DATA_WIDTH_32BIT, 1U, &ResetCtrl0);

        /* polling reset tx */
        RetVal = B8_ERR_SERDES_LINK;
        (void) AmbaB8_KAL_GetSysTickCount(&SysTickStamp0);
        do {
            (void) B8N_RegRead(ChipID, & pAmbaB8_PwmEncReg->ResetCtrl0, 0U, B8_DATA_WIDTH_32BIT, 1U, &ResetCtrl0);
            if ((ResetCtrl0.TxReset12 == 0U) && (ResetCtrl0.RportFifoReset12 == 0U)) {
                RetVal = B8_ERR_NONE;
                break;
            } else {
                (void) AmbaB8_KAL_GetSysTickCount(&SysTickStamp1);
            }
        } while ((SysTickStamp1 - SysTickStamp0) < POLLING_TIMEOUT);
    } else {
        RetVal = B8_ERR_ARG;
    }

    if (RetVal == B8_ERR_SERDES_LINK) {
        AmbaB8_PrintStr5("PwmEncTxReset fail", NULL, NULL, NULL, NULL, NULL);
    }
    return RetVal;
}

UINT32 AmbaB8_PwmEncSendResetPkt(UINT32 ChipID)
{
    UINT32 RetVal = B8_ERR_NONE;
    UINT32 SysTickStamp0 = 0, SysTickStamp1 = 0;

    B8_PWM_ENC_RESET_CTRL1_REG_s ResetCtrl1 = {0};

    if ((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK) != 0U) {
        /* reset rx (send rst pkt) */
        ResetCtrl1.SendRstPkt0 = 1U;
        (void) B8N_RegWrite(ChipID, & pAmbaB8_PwmEncReg->ResetCtrl1, 0U, B8_DATA_WIDTH_32BIT, 1U, &ResetCtrl1);

        /* polling reset rx */
        (void) AmbaB8_KAL_GetSysTickCount(&SysTickStamp0);
        do {
            (void) B8N_RegRead(ChipID, & pAmbaB8_PwmEncReg->ResetCtrl1, 0U, B8_DATA_WIDTH_32BIT, 1U, &ResetCtrl1);
            if (ResetCtrl1.SendRstPkt0 == 0U) {
                RetVal = B8_ERR_NONE;
                break;
            } else {
                RetVal = B8_ERR_SERDES_LINK;
                (void) AmbaB8_KAL_GetSysTickCount(&SysTickStamp1);
            }
        } while ((SysTickStamp1 - SysTickStamp0) < POLLING_TIMEOUT);

    } else { /* if ((ChipID & B8_MAIN_CHIP_ID_B8ND_MASK) != 0U) { */
        /* reset rx (send rst pkt) */
        ResetCtrl1.SendRstPkt12 = 1U;
        (void) B8N_RegWrite(ChipID, & pAmbaB8_PwmEncReg->ResetCtrl1, 0U, B8_DATA_WIDTH_32BIT, 1U, &ResetCtrl1);

        /* polling reset rx */
        (void) AmbaB8_KAL_GetSysTickCount(&SysTickStamp0);
        do {
            (void) B8N_RegRead(ChipID, & pAmbaB8_PwmEncReg->ResetCtrl1, 0U, B8_DATA_WIDTH_32BIT, 1U, &ResetCtrl1);
            if (ResetCtrl1.SendRstPkt12 == 0U) {
                RetVal = B8_ERR_NONE;
                break;
            } else {
                RetVal = B8_ERR_SERDES_LINK;
                (void) AmbaB8_KAL_GetSysTickCount(&SysTickStamp1);
            }
        } while ((SysTickStamp1 - SysTickStamp0) < POLLING_TIMEOUT);
    }

    if (RetVal == B8_ERR_SERDES_LINK) {
        AmbaB8_PrintStr5("AmbaB8_PwmEncSendResetPkt fail", NULL, NULL, NULL, NULL, NULL);
    }

    return RetVal;
}

UINT32 AmbaB8_PwmEncSetCreditPwr(UINT32 ChipID, UINT32 CreditPwr)
{
    UINT32 RetVal = B8_ERR_NONE;

    if ((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK) != 0U) {
        B8_PWM_ENC_CREDIT_CFG0_REG_s CreditCfg0 = {0};

        RetVal |= B8N_RegRead(ChipID, & pAmbaB8_PwmEncReg->CreditCfg0, 0U, B8_DATA_WIDTH_32BIT, 1U, &CreditCfg0);
        CreditCfg0.Ch0CreditPwr = (UINT8)CreditPwr;
        RetVal |= B8N_RegWrite(ChipID, & pAmbaB8_PwmEncReg->CreditCfg0, 0U, B8_DATA_WIDTH_32BIT, 1U, &CreditCfg0);

    } else if ((ChipID & B8_MAIN_CHIP_ID_B8ND_MASK) != 0U) {
        B8_PWM_ENC_CREDIT_CFG1_REG_s CreditCfg1 = {0};

        RetVal |= B8N_RegRead(ChipID, & pAmbaB8_PwmEncReg->CreditCfg1, 0U, B8_DATA_WIDTH_32BIT, 1U, &CreditCfg1);
        CreditCfg1.Ch12CreditPwr = (UINT8)CreditPwr;
        RetVal |= B8N_RegWrite(ChipID, & pAmbaB8_PwmEncReg->CreditCfg1, 0U, B8_DATA_WIDTH_32BIT, 1U, &CreditCfg1);

    } else {
        RetVal = B8_ERR_ARG;
    }

    return RetVal;
}

UINT32 AmbaB8_PwmEncSetChunkSize(UINT32 ChipID, UINT32 ChunkSize)
{
    UINT32 RetVal = B8_ERR_NONE;

    if ((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK) != 0U) {
        B8_PWM_ENC_CREDIT_CFG1_REG_s CreditCfg1 = {0};

        RetVal |= B8N_RegRead(ChipID, & pAmbaB8_PwmEncReg->CreditCfg1, 0U, B8_DATA_WIDTH_32BIT, 1U, &CreditCfg1);
        CreditCfg1.LsChunkSize = (UINT8)ChunkSize;
        RetVal |= B8N_RegWrite(ChipID, & pAmbaB8_PwmEncReg->CreditCfg1, 0U, B8_DATA_WIDTH_32BIT, 1U, &CreditCfg1);

    } else if ((ChipID & B8_MAIN_CHIP_ID_B8ND_MASK) != 0U) {
        B8_PWM_ENC_CREDIT_CFG1_REG_s CreditCfg1 = {0};

        RetVal |= B8N_RegRead(ChipID, & pAmbaB8_PwmEncReg->CreditCfg1, 0U, B8_DATA_WIDTH_32BIT, 1U, &CreditCfg1);
        CreditCfg1.HsChunkSize = (UINT8)ChunkSize;
        RetVal |= B8N_RegWrite(ChipID, & pAmbaB8_PwmEncReg->CreditCfg1, 0U, B8_DATA_WIDTH_32BIT, 1U, &CreditCfg1);

    } else {
        RetVal = B8_ERR_ARG;
    }

    return RetVal;
}

UINT32 AmbaB8_PwmEncSetDisableAck(UINT32 ChipID, UINT32 DisableAck)
{
    UINT32 RetVal = B8_ERR_NONE;
    B8_PWM_ENC_CTRL1_REG_s Ctrl1 = {0};

    RetVal |= B8N_RegRead(ChipID, & pAmbaB8_PwmEncReg->Ctrl1, 0U, B8_DATA_WIDTH_32BIT, 1U, &Ctrl1);
    if ((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK) != 0U) {
        if (DisableAck != 0U) {
            Ctrl1.LSC0DisableAck = 1U;
        } else {
            Ctrl1.LSC0DisableAck = 0U;
        }
        RetVal |= B8N_RegWrite(ChipID, & pAmbaB8_PwmEncReg->Ctrl1, 0U, B8_DATA_WIDTH_32BIT, 1U, &Ctrl1);
    } else if ((ChipID & B8_MAIN_CHIP_ID_B8ND_MASK) != 0U) {
        if (DisableAck != 0U) {
            Ctrl1.HSC0DisableAck = 1U;
        } else {
            Ctrl1.HSC0DisableAck = 0U;
        }
        RetVal |= B8N_RegWrite(ChipID, & pAmbaB8_PwmEncReg->Ctrl1, 0U, B8_DATA_WIDTH_32BIT, 1U, &Ctrl1);
    } else {
        RetVal = B8_ERR_ARG;
    }

    return RetVal;
}

UINT32 AmbaB8_PwmEncEnable(UINT32 ChipID, UINT32 ReplayTimes)
{
    UINT32 RetVal = B8_ERR_NONE;
    B8_PWM_ENC_CTRL_REG_s Ctrl = {0};

    RetVal |= B8N_RegRead(ChipID, & pAmbaB8_PwmEncReg->Ctrl, 0U, B8_DATA_WIDTH_32BIT, 1U, &Ctrl);

    Ctrl.Enable = 1U;
    Ctrl.ReplayTimes = (UINT8)ReplayTimes;
    RetVal |= B8N_RegWrite(ChipID, & pAmbaB8_PwmEncReg->Ctrl, 0U, B8_DATA_WIDTH_32BIT, 1U, &Ctrl);

    return RetVal;
}

UINT32 AmbaB8_PwmEncRegWrite(UINT32 ChipID, const volatile void *pReg, UINT8 AddrIncrement, UINT32 DataWidth, UINT32 DataSize, const void *pTxDataBuf)
{
    return B8N_RegWrite(ChipID, pReg, AddrIncrement, DataWidth, DataSize, pTxDataBuf);
}

UINT32 AmbaB8_PwmEncRegRead(UINT32 ChipID, const volatile void *pReg, UINT8 AddrIncrement, UINT32 DataWidth, UINT32 DataSize, void *pRxDataBuf)
{
    return B8N_RegRead(ChipID, pReg, AddrIncrement, DataWidth, DataSize, pRxDataBuf);
}



