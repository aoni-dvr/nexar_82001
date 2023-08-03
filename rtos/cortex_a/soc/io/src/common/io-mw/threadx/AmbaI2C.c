/**
 *  @file AmbaI2C.c
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
 *  @details I2C APIs
 *
 */

#include "AmbaTypes.h"
#include "AmbaDef.h"
#include "AmbaKAL.h"

#include "AmbaDrvEntry.h"
#include "AmbaI2C.h"
#include "AmbaRTSL_I2C.h"
#include "AmbaRTSL_PLL.h"

static AMBA_KAL_EVENT_FLAG_t AmbaI2cEventFlag;
static AMBA_KAL_MUTEX_t AmbaI2cMasterMutex[AMBA_NUM_I2C_CHANNEL];
static AMBA_KAL_MUTEX_t AmbaI2cSlaveMutex[AMBA_NUM_I2C_SLAVE];
static UINT32 AmbaI2cMasterIntHandlerErr[AMBA_NUM_I2C_CHANNEL];

static void I2C_MasterIntHandler(UINT32 I2cChanNo);
static UINT32 I2C_MasterWaitComplete(UINT32 I2cChanNo, UINT32 TimeOut);

/**
 *  AmbaI2C_DrvEntry - Initializes driver-wide data structures and resources
 *  @return error code
 *  @note this function is an internal using only API
 */
UINT32 AmbaI2C_DrvEntry(void)
{
    static char AmbaI2cEventFlagName[18] = "AmbaI2cEventFlags";
    static char AmbaI2cMasterMutexName[20] = "AmbaI2cMasterMutex";
    static char AmbaI2cSlaveMutexName[20] = "AmbaI2cSlaveMutex";

    UINT32 i, RetVal = I2C_ERR_NONE;

    /* Create EventFlags */
    if (AmbaKAL_EventFlagCreate(&AmbaI2cEventFlag, AmbaI2cEventFlagName) != KAL_ERR_NONE) {
        RetVal = I2C_ERR_UNEXPECTED;
    }

    /* Create Mutex */
    if (RetVal == I2C_ERR_NONE) {
        for (i = 0U; i < AMBA_NUM_I2C_CHANNEL; i++) {
            if (AmbaKAL_MutexCreate(&AmbaI2cMasterMutex[i], AmbaI2cMasterMutexName) != KAL_ERR_NONE) {
                RetVal = I2C_ERR_UNEXPECTED;
                break;
            }
        }

        for (i = 0U; i < AMBA_NUM_I2C_SLAVE; i++) {
            if (AmbaKAL_MutexCreate(&AmbaI2cSlaveMutex[i], AmbaI2cSlaveMutexName) != KAL_ERR_NONE) {
                RetVal = I2C_ERR_UNEXPECTED;
                break;
            }
        }
    }

    /* Initialize hardware module */
    if (RetVal == I2C_ERR_NONE) {
        (void)AmbaRTSL_I2cInit();
        (void)AmbaRTSL_I2cIntHookHandler(I2C_MasterIntHandler);
    }

    return RetVal;
}

/**
 *  AmbaI2C_MasterWrite - I2C Master write data operation
 *  @param[in] MasterID Indicate an I2C master
 *  @param[in] BusSpeed Bus speed mode with preset timing parameters
 *  @param[in] pTxTransaction Pointer to TX transaction
 *  @param[out] pActualTxSize The actual number of transmitted data
 *  @param[in] TimeOut Time out value
 *  @return error code
 */
UINT32 AmbaI2C_MasterWrite(UINT32 MasterID, UINT32 BusSpeed, const AMBA_I2C_TRANSACTION_s *pTxTransaction, UINT32 *pActualTxSize, UINT32 TimeOut)
{
    UINT32 RetVal = I2C_ERR_NONE;

    if ((MasterID >= AMBA_NUM_I2C_CHANNEL) || (BusSpeed >= NUM_I2C_SPEED) ||
        (pTxTransaction == NULL) || (pTxTransaction->DataSize == 0U) || (pTxTransaction->pDataBuf == NULL)) {
        RetVal = I2C_ERR_ARG;
    } else {
        if (AmbaKAL_MutexTake(&AmbaI2cMasterMutex[MasterID], TimeOut) != KAL_ERR_NONE) {
            RetVal = I2C_ERR_MUTEX;
        } else {
            if (AmbaKAL_EventFlagClear(&AmbaI2cEventFlag, (UINT32)1U << MasterID) != KAL_ERR_NONE) {
                RetVal = I2C_ERR_UNEXPECTED;
            } else {
                AmbaI2cMasterIntHandlerErr[MasterID] = I2C_ERR_NONE; /* clear error status reported by interrupt handler */
                RetVal = AmbaRTSL_I2cWrite(MasterID, BusSpeed, pTxTransaction);
                if (RetVal == I2C_ERR_NONE) {
                    RetVal = I2C_MasterWaitComplete(MasterID, TimeOut);
                }

                if ((BusSpeed == I2C_SPEED_HIGH) && (RetVal == I2C_ERR_NACK)) {
                    /* In high-speed mode, no need to care about NACK */
                    RetVal = I2C_ERR_NONE;
                }

                if (pActualTxSize != NULL) {
                    *pActualTxSize = AmbaRTSL_I2cGetActualSize(MasterID);
                }

                (void)AmbaRTSL_I2cStop(MasterID);

                if (AmbaI2cMasterIntHandlerErr[MasterID] != I2C_ERR_NONE) {  /* check if any error reported in interrupt handler */
                    RetVal = AmbaI2cMasterIntHandlerErr[MasterID];
                }
            }

            if (AmbaKAL_MutexGive(&AmbaI2cMasterMutex[MasterID]) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = I2C_ERR_UNEXPECTED;
            }
        }
    }

    return RetVal;
}

/**
 *  AmbaI2C_MasterRead - I2C Master read data operation
 *  @param[in] MasterID Indicate an I2C master
 *  @param[in] BusSpeed Bus speed mode with preset timing parameters
 *  @param[in] pRxransaction Pointer to RX transaction
 *  @param[out] pActualRxSize The actual number of recieved data
 *  @param[in] TimeOut Time out value
 *  @return error code
 */
UINT32 AmbaI2C_MasterRead(UINT32 MasterID, UINT32 BusSpeed, const AMBA_I2C_TRANSACTION_s *pRxTransaction, UINT32 *pActualRxSize, UINT32 TimeOut)
{
    UINT32 RetVal = I2C_ERR_NONE;

    if ((MasterID >= AMBA_NUM_I2C_CHANNEL) || (BusSpeed >= NUM_I2C_SPEED) ||
        (pRxTransaction == NULL) || (pRxTransaction->DataSize == 0U) || (pRxTransaction->pDataBuf == NULL)) {
        RetVal = I2C_ERR_ARG;
    } else {
        if (AmbaKAL_MutexTake(&AmbaI2cMasterMutex[MasterID], TimeOut) != KAL_ERR_NONE) {
            RetVal = I2C_ERR_MUTEX;
        } else {
            if (AmbaKAL_EventFlagClear(&AmbaI2cEventFlag, (UINT32)1U << MasterID) != KAL_ERR_NONE) {
                RetVal = I2C_ERR_UNEXPECTED;
            } else {
                AmbaI2cMasterIntHandlerErr[MasterID] = I2C_ERR_NONE; /* clear error status reported by interrupt handler */
                RetVal = AmbaRTSL_I2cRead(MasterID, BusSpeed, pRxTransaction);
                if (RetVal == I2C_ERR_NONE) {
                    RetVal = I2C_MasterWaitComplete(MasterID, TimeOut);
                }

                if (pActualRxSize != NULL) {
                    *pActualRxSize = AmbaRTSL_I2cGetActualSize(MasterID);
                }

                (void)AmbaRTSL_I2cStop(MasterID);

                if (AmbaI2cMasterIntHandlerErr[MasterID] != I2C_ERR_NONE) {  /* check if any error reported in interrupt handler */
                    RetVal = AmbaI2cMasterIntHandlerErr[MasterID];
                }
            }

            if (AmbaKAL_MutexGive(&AmbaI2cMasterMutex[MasterID]) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = I2C_ERR_UNEXPECTED;
            }
        }
    }

    return RetVal;
}

/**
 *  AmbaI2C_MasterReadAfterWrite - I2C Master write and then read data operation
 *  @param[in] MasterID Indicate an I2C master
 *  @param[in] BusSpeed Bus speed mode with preset timing parameters
 *  @param[in] NumTxTransaction Number of TX transactions
 *  @param[in] pTxTransaction Pointer to TX transaction
 *  @param[in] pRxransaction Pointer to RX transaction
 *  @param[out] pActualSize The actual number of rx/tx data
 *  @param[in] TimeOut Time out value
 *  @return error code
 */
UINT32 AmbaI2C_MasterReadAfterWrite(UINT32 MasterID, UINT32 BusSpeed, UINT32 NumTxTransaction, const AMBA_I2C_TRANSACTION_s * pTxTransaction, const AMBA_I2C_TRANSACTION_s * pRxTransaction, UINT32 * pActualSize, UINT32 TimeOut)
{
    UINT32 RetVal = I2C_ERR_NONE;

    if ((MasterID >= AMBA_NUM_I2C_CHANNEL) || (BusSpeed >= NUM_I2C_SPEED) || (NumTxTransaction == 0U) ||
        (pTxTransaction == NULL) || (pTxTransaction->DataSize == 0U) || (pTxTransaction->pDataBuf == NULL) ||
        (pRxTransaction == NULL) || (pRxTransaction->DataSize == 0U) || (pRxTransaction->pDataBuf == NULL)) {
        RetVal = I2C_ERR_ARG;
    } else {
        if (AmbaKAL_MutexTake(&AmbaI2cMasterMutex[MasterID], TimeOut) != KAL_ERR_NONE) {
            RetVal = I2C_ERR_MUTEX;
        } else {
            if (AmbaKAL_EventFlagClear(&AmbaI2cEventFlag, (UINT32)1U << MasterID) != KAL_ERR_NONE) {
                RetVal = I2C_ERR_UNEXPECTED;
            } else {
                AmbaI2cMasterIntHandlerErr[MasterID] = I2C_ERR_NONE; /* clear error status reported by interrupt handler */
                RetVal = AmbaRTSL_I2cReadAfterWrite(MasterID, BusSpeed, NumTxTransaction, pTxTransaction, pRxTransaction);
                if (RetVal == I2C_ERR_NONE) {
                    RetVal = I2C_MasterWaitComplete(MasterID, TimeOut);
                }

                if (pActualSize != NULL) {
                    *pActualSize = AmbaRTSL_I2cGetActualSize(MasterID);
                }

                (void)AmbaRTSL_I2cStop(MasterID);

                if (AmbaI2cMasterIntHandlerErr[MasterID] != I2C_ERR_NONE) {  /* check if any error reported in interrupt handler */
                    RetVal = AmbaI2cMasterIntHandlerErr[MasterID];
                }
            }

            if (AmbaKAL_MutexGive(&AmbaI2cMasterMutex[MasterID]) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = I2C_ERR_UNEXPECTED;
            }
        }
    }

    return RetVal;
}

#if defined(CONFIG_APP_FLOW_CARDV_AONI)
static int delay = -1;
void AmbaI2C_CustomSetDelay(int ms)
{
    delay = ms;
}

UINT32 AmbaI2C_MasterReadAfterWrite2(UINT32 MasterID, UINT32 BusSpeed, UINT32 NumTxTransaction, const AMBA_I2C_TRANSACTION_s * pTxTransaction, const AMBA_I2C_TRANSACTION_s * pRxTransaction, UINT32 * pActualSize, UINT32 TimeOut)
{
    UINT32 RetVal = I2C_ERR_NONE;

    if ((MasterID >= AMBA_NUM_I2C_CHANNEL) || (BusSpeed >= NUM_I2C_SPEED) ||
        (pTxTransaction == NULL) || (pTxTransaction->DataSize == 0U) || (pTxTransaction->pDataBuf == NULL)) {
        RetVal = I2C_ERR_ARG;
    } else {
        if (AmbaKAL_MutexTake(&AmbaI2cMasterMutex[MasterID], TimeOut) != KAL_ERR_NONE) {
            RetVal = I2C_ERR_MUTEX;
        } else {
            if (AmbaKAL_EventFlagClear(&AmbaI2cEventFlag, (UINT32)1U << MasterID) != KAL_ERR_NONE) {
                RetVal = I2C_ERR_UNEXPECTED;
            } else {
                //tx
                AmbaI2cMasterIntHandlerErr[MasterID] = I2C_ERR_NONE; /* clear error status reported by interrupt handler */
                RetVal = AmbaRTSL_I2cWrite(MasterID, BusSpeed, pTxTransaction);
                if (RetVal == I2C_ERR_NONE) {
                    RetVal = I2C_MasterWaitComplete(MasterID, TimeOut);
                }

                if ((BusSpeed == I2C_SPEED_HIGH) && (RetVal == I2C_ERR_NACK)) {
                    /* In high-speed mode, no need to care about NACK */
                    RetVal = I2C_ERR_NONE;
                }
                (void)AmbaRTSL_I2cStop(MasterID);
                if (AmbaI2cMasterIntHandlerErr[MasterID] != I2C_ERR_NONE) {  /* check if any error reported in interrupt handler */
                    RetVal = AmbaI2cMasterIntHandlerErr[MasterID];
                }
                if (delay > 0) {
                    AmbaKAL_TaskSleep(delay);
                } else {
                    AmbaKAL_TaskSleep(10);
                }
                //rx
                if (AmbaKAL_EventFlagClear(&AmbaI2cEventFlag, (UINT32)1U << MasterID) != KAL_ERR_NONE) {
                    RetVal = I2C_ERR_UNEXPECTED;
                } else {
                    AmbaI2cMasterIntHandlerErr[MasterID] = I2C_ERR_NONE; /* clear error status reported by interrupt handler */
                    RetVal = AmbaRTSL_I2cRead(MasterID, BusSpeed, pRxTransaction);
                    if (RetVal == I2C_ERR_NONE) {
                        RetVal = I2C_MasterWaitComplete(MasterID, TimeOut);
                    }

                    if (pActualSize != NULL) {
                        *pActualSize = AmbaRTSL_I2cGetActualSize(MasterID);
                    }
                    (void)AmbaRTSL_I2cStop(MasterID);

                    if (AmbaI2cMasterIntHandlerErr[MasterID] != I2C_ERR_NONE) {  /* check if any error reported in interrupt handler */
                        RetVal = AmbaI2cMasterIntHandlerErr[MasterID];
                    }
                }
            }

            if (AmbaKAL_MutexGive(&AmbaI2cMasterMutex[MasterID]) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = I2C_ERR_UNEXPECTED;
            }
        }
    }

    return RetVal;
}
#endif


/**
 *  AmbaI2C_MasterReadVarLength - I2C Master read Variable Length of data operation
 *  @param[in] MasterID Indicate an I2C master
 *  @param[in] BusSpeed Bus speed mode with preset timing parameters
 *  @param[in] VarLenInfo Number of bytes used to indicate the variable length information
 *  @param[in] pRxransaction Pointer to RX transaction
 *  @param[out] pActualSize The actual number of rx data
 *  @param[in] TimeOut Time out value
 *  @return error code
 */
UINT32 AmbaI2C_MasterReadVarLength(UINT32 MasterID, UINT32 BusSpeed, UINT32 VarLenInfo, const AMBA_I2C_TRANSACTION_s *pRxTransaction, UINT32 *pActualRxSize, UINT32 TimeOut)
{
    UINT32 RetVal = I2C_ERR_NONE;

    if ((MasterID >= AMBA_NUM_I2C_CHANNEL) || (BusSpeed >= NUM_I2C_SPEED) || (VarLenInfo == 0U) ||
        (pRxTransaction == NULL) || (pRxTransaction->DataSize == 0U) || (pRxTransaction->pDataBuf == NULL)) {
        RetVal = I2C_ERR_ARG;
    } else {
        if (AmbaKAL_MutexTake(&AmbaI2cMasterMutex[MasterID], TimeOut) != KAL_ERR_NONE) {
            RetVal = I2C_ERR_MUTEX;
        } else {
            if (AmbaKAL_EventFlagClear(&AmbaI2cEventFlag, (UINT32)1U << MasterID) != KAL_ERR_NONE) {
                RetVal = I2C_ERR_UNEXPECTED;
            } else {
                AmbaI2cMasterIntHandlerErr[MasterID] = I2C_ERR_NONE; /* clear error status reported by interrupt handler */
                RetVal = AmbaRTSL_I2cReadVarLength(MasterID, BusSpeed, VarLenInfo, pRxTransaction);
                if (RetVal == I2C_ERR_NONE) {
                    RetVal = I2C_MasterWaitComplete(MasterID, TimeOut);
                }

                if (pActualRxSize != NULL) {
                    *pActualRxSize = AmbaRTSL_I2cGetActualSize(MasterID);
                }

                (void)AmbaRTSL_I2cStop(MasterID);

                if (AmbaI2cMasterIntHandlerErr[MasterID] != I2C_ERR_NONE) {  /* check if any error reported in interrupt handler */
                    RetVal = AmbaI2cMasterIntHandlerErr[MasterID];
                }
            }

            if (AmbaKAL_MutexGive(&AmbaI2cMasterMutex[MasterID]) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = I2C_ERR_UNEXPECTED;
            }
        }
    }

    return RetVal;
}

/**
 *  AmbaI2C_MasterGetInfo - I2C Master get actual transimission bitrate
 *  @param[in] MasterID Indicate an I2C master
 *  @param[in] BusSpeed Bus speed mode with preset timing parameters
 *  @param[out] pActualBitRate The actual transmission bitrate
 *  @return error code
 */
UINT32 AmbaI2C_MasterGetInfo(UINT32 MasterID, UINT32 BusSpeed, UINT32 * pActualBitRate)
{
    static const UINT32 I2cSpeedFactor[4U] = {
        [I2C_SPEED_STANDARD]   = 400000U,   /* for I2C Standard speed: 100Kbps */
        [I2C_SPEED_FAST]       = 1320000U,  /* for I2C Fast speed: 330Kbps */
        [I2C_SPEED_FAST_PLUS]  = 4000000U,  /* for I2C Fast-mode Plus speed: 1Mbps */
        [I2C_SPEED_HIGH]       = 13600000U  /* for I2C High-speed mode: 3.4Mbps */
    };
    UINT32 RetVal = I2C_ERR_NONE;
    UINT32 RefClkFreq, PrescalerVal;

    if ((MasterID >= AMBA_NUM_I2C_CHANNEL) || (BusSpeed >= NUM_I2C_SPEED) || (pActualBitRate == NULL)) {
        RetVal = I2C_ERR_ARG;
    } else {
        /*
         * I2C period = (APB bus clock) / (4 * (prescaler[15:0] + 1) + 2)
         * 4 * (prescaler[15:0] + 1) = ((APB bus clock) / I2C period) - 2
         * prescaler[15:0] = ((((APB bus clock) / I2C period) - 2) / 4) - 1
         * prescaler[15:0] = (((APB bus clock) / ((I2C Period) * 4))) - 0.5) - 1
         */
        RefClkFreq = AmbaRTSL_PllGetApbClk();
        PrescalerVal = (RefClkFreq / I2cSpeedFactor[BusSpeed]) - 1U;
        *pActualBitRate = RefClkFreq / (((PrescalerVal + 1U) * 4U) + 2U);
    }

    return RetVal;
}

/**
 *  AmbaI2C_SlaveStart - Enable an I2C slave
 *  @param[in] SlaveID Indicate an I2C slave
 *  @param[in] WriteIntFunc ISR when slave detects a slave address for master read
 *  @param[in] ReadIntFunc ISR when slave detects a slave address for master write
 *  @param[in] BreakIntFunc ISR when either STOP or repeated-START condition is detected
 *  @return error code
 */
UINT32 AmbaI2C_SlaveStart(UINT32 SlaveID, UINT32 SlaveAddr, AMBA_I2C_SLAVE_WRITE_ISR_f WriteIntFunc, AMBA_I2C_SLAVE_READ_ISR_f ReadIntFunc, AMBA_I2C_SLAVE_BREAK_ISR_f BreakIntFunc)
{
    UINT32 RetVal = I2C_ERR_NONE;

    if ((SlaveID >= AMBA_NUM_I2C_SLAVE) || (SlaveAddr > 0xffU) ||
        (WriteIntFunc == NULL) || (ReadIntFunc == NULL) || (BreakIntFunc == NULL)) {
        RetVal = I2C_ERR_ARG;
    } else {
        if (AmbaKAL_MutexTake(&AmbaI2cSlaveMutex[SlaveID], KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = I2C_ERR_MUTEX;
        } else {
            RetVal = AmbaRTSL_I2cSlaveStart((SlaveAddr >> 1UL), WriteIntFunc, ReadIntFunc, BreakIntFunc);

            if (AmbaKAL_MutexGive(&AmbaI2cSlaveMutex[SlaveID]) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = I2C_ERR_UNEXPECTED;
            }
        }
    }

    return RetVal;
}

/**
 *  AmbaI2C_SlaveStop - Disable an I2C slave
 *  @param[in] SlaveID Indicate an I2C slave
 *  @return error code
 */
UINT32 AmbaI2C_SlaveStop(UINT32 SlaveID)
{
    UINT32 RetVal = I2C_ERR_NONE;

    if (SlaveID >= AMBA_NUM_I2C_SLAVE) {
        RetVal = I2C_ERR_ARG;
    } else {
        if (AmbaKAL_MutexTake(&AmbaI2cSlaveMutex[SlaveID], KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = I2C_ERR_MUTEX;
        } else {
            RetVal = AmbaRTSL_I2cSlaveStop();

            if (AmbaKAL_MutexGive(&AmbaI2cSlaveMutex[SlaveID]) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = I2C_ERR_UNEXPECTED;
            }
        }
    }

    return RetVal;
}

/**
 *  I2C_MasterWaitComplete - I2C polling transfer done function
 *  @param[in] I2cChanNo I2C Channel Number
 *  @return error code
 */
static UINT32 I2C_MasterWaitComplete(UINT32 I2cChanNo, UINT32 TimeOut)
{
    UINT32 KalRetVal, RetVal = I2C_ERR_NONE;
    UINT32 ActualFlags = 0U;

    /* wait for Tx Done: Event Flag ! */
    if (AmbaKAL_EventFlagGet(&AmbaI2cEventFlag, (UINT32)1U << I2cChanNo, KAL_FLAGS_WAIT_ALL, KAL_FLAGS_CLEAR_AUTO, &ActualFlags, TimeOut) != KAL_ERR_NONE) {
        /* If timeout, then set an early end status for ISR to send a stop condition */
        (void)AmbaRTSL_I2cTerminate(I2cChanNo);

        KalRetVal = AmbaKAL_EventFlagGet(&AmbaI2cEventFlag, (UINT32)1U << I2cChanNo, KAL_FLAGS_WAIT_ALL, KAL_FLAGS_CLEAR_AUTO, &ActualFlags, TimeOut);
        if (KalRetVal != KAL_ERR_NONE) {
            if (KalRetVal == KAL_ERR_TIMEOUT) {
                RetVal = I2C_ERR_TIMEOUT;
            } else {
                RetVal = I2C_ERR_UNEXPECTED;
            }
        }
    }

    if (AmbaRTSL_I2cGetResponse(I2cChanNo) != 0U) {
        RetVal = I2C_ERR_NACK;
    }

    return RetVal;
}

/**
 *  I2C_MasterIntHandler - ISR for I2C masters
 *  @param[in] I2cChanNo I2C Channel Number
 */
static void I2C_MasterIntHandler(UINT32 I2cChanNo)
{
    /* set Tx/Rx Done Event Flag */
    if (AmbaKAL_EventFlagSet(&AmbaI2cEventFlag, (UINT32)1U << I2cChanNo) != KAL_ERR_NONE) {
        AmbaI2cMasterIntHandlerErr[I2cChanNo] = I2C_ERR_UNEXPECTED;
    }
}

