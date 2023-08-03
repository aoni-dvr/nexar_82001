/**
 *  @file AmbaRTSL_I2C.c
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
 *  @details I2C RTSL APIs
 *
 */

#include "AmbaDef.h"
#include "AmbaTypes.h"
#include "AmbaMisraFix.h"

#include "AmbaRTSL_PLL.h"
#include "AmbaRTSL_GIC.h"
#include "AmbaRTSL_I2C.h"
#include "AmbaCSL_I2C.h"

#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
#include "AmbaCortexA76.h"
#else
#include "AmbaCortexA53.h"
#endif

#if defined(CONFIG_DEVICE_TREE_SUPPORT)
#include <AmbaIOUtility.h>
#endif

#define AMBA_I2C_RX_MODE_START          0x8000U  /* the flag to indicate RX mode start */
#define AMBA_I2C_RX_GET_VAR_LENGTH      0x4000U  /* the flag to indicate the first several bytes are the RX data length */
#define AMBA_I2C_FORCE_TERMINATE        0x2000U  /* the flag to indicate the transaction needs to be terminated immediately */
#define AMBA_I2C_STATE_ERROR            0x1000U  /* the flag to indicate the driver is hit unexpected error */
#define AMBA_I2C_SLAVE_NO_ACK           0x00ffU  /* the flag to indicate there is no ACK from slave or slave returns NACK */

AMBA_I2C_REG_s * pAmbaI2C_MasterReg[AMBA_NUM_I2C_CHANNEL];
AMBA_I2CS_REG_s * pAmbaI2C_SlaveReg;

typedef struct {
    const AMBA_I2C_TRANSACTION_s *pTxTransaction;   /* the original transaction list */
    const AMBA_I2C_TRANSACTION_s *pRxTransaction;   /* the original transaction list */
    UINT32 NumTxTransaction;
    UINT32 NumRxTransaction;
    AMBA_I2C_TRANSACTION_s TxTransaction;           /* the current transaction */
    AMBA_I2C_TRANSACTION_s RxTransaction;           /* the current transaction */

    UINT32  ActualSize;     /* the number of actually transfered data size */
    UINT32  VarLenInfo;
    UINT32  Status;         /* Tx/Rx status */
} AMBA_I2C_CTRL_s;

static UINT32 I2C_Config(AMBA_I2C_REG_s *pI2cReg, UINT32 I2cSpeed);
static UINT32 I2C_Start(AMBA_I2C_REG_s *pI2cReg, UINT32 I2cSpeed, UINT32 SlaveAddr);
static void I2C_CheckStatus(UINT32 I2cChanNo);
static void I2C_SlaveReset(UINT32 SlaveAddr);
static void I2C_MasterIntHandler(UINT32 Irq, UINT32 I2cChanNo);
static void I2C_SlaveIntHandler(UINT32 Irq, UINT32 I2cChanNo);
static void I2C_MasterHsWriteData(UINT32 I2cChanNo);

static const UINT32 AmbaI2cMasterIRQ[AMBA_NUM_I2C_CHANNEL] = {
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
    [AMBA_I2C_CHANNEL0] = AMBA_INT_SPI_ID046_I2C_MASTER0,
    [AMBA_I2C_CHANNEL1] = AMBA_INT_SPI_ID047_I2C_MASTER1,
    [AMBA_I2C_CHANNEL2] = AMBA_INT_SPI_ID048_I2C_MASTER2,
    [AMBA_I2C_CHANNEL3] = AMBA_INT_SPI_ID049_I2C_MASTER3,
    [AMBA_I2C_CHANNEL4] = AMBA_INT_SPI_ID050_I2C_MASTER4,
    [AMBA_I2C_CHANNEL5] = AMBA_INT_SPI_ID051_I2C_MASTER5,
#elif defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    [AMBA_I2C_CHANNEL0] = AMBA_INT_SPI_ID46_I2C_MASTER0,
    [AMBA_I2C_CHANNEL1] = AMBA_INT_SPI_ID47_I2C_MASTER1,
    [AMBA_I2C_CHANNEL2] = AMBA_INT_SPI_ID48_I2C_MASTER2,
    [AMBA_I2C_CHANNEL3] = AMBA_INT_SPI_ID49_I2C_MASTER3,
    [AMBA_I2C_CHANNEL4] = AMBA_INT_SPI_ID50_I2C_MASTER4,
    [AMBA_I2C_CHANNEL5] = AMBA_INT_SPI_ID51_I2C_MASTER5,
#else
    [AMBA_I2C_CHANNEL0] = AMBA_INT_SPI_ID47_I2C_MASTER0,
    [AMBA_I2C_CHANNEL1] = AMBA_INT_SPI_ID48_I2C_MASTER1,
    [AMBA_I2C_CHANNEL2] = AMBA_INT_SPI_ID49_I2C_MASTER2,
    [AMBA_I2C_CHANNEL3] = AMBA_INT_SPI_ID50_I2C_MASTER3,
#endif
};
static const UINT32 AmbaI2cSlaveIRQ[AMBA_NUM_I2C_SLAVE] = {
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
    [AMBA_I2C_SLAVE0] = AMBA_INT_SPI_ID052_I2C_SLAVE,
#elif defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    [AMBA_I2C_SLAVE0] = AMBA_INT_SPI_ID52_I2C_SLAVE,
#else
    [AMBA_I2C_SLAVE0] = AMBA_INT_SPI_ID51_I2C_SLAVE,
#endif
};

static AMBA_I2C_CTRL_s AmbaI2cCtrl[AMBA_NUM_I2C_CHANNEL];
static UINT32 ApbClock = 0U;

/* Call back functions when completed transactions */
static AMBA_I2C_ISR_f AmbaI2cMasterIsrCb;
static AMBA_I2C_SLAVE_WRITE_ISR_f AmbaI2cSlaveWriteIsrCb;
static AMBA_I2C_SLAVE_READ_ISR_f AmbaI2cSlaveReadIsrCb;
static AMBA_I2C_SLAVE_BREAK_ISR_f AmbaI2cSlaveBreakIsrCb;

/**
 *  AmbaRTSL_I2cInit - I2C initializations
 *  @return error code
 */
UINT32 AmbaRTSL_I2cInit(void)
{
    AMBA_INT_CONFIG_s IntConfig;
    UINT32 i;
    ULONG base_addr;
    INT32 offset = 0;
    (void)offset;

#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
    base_addr = AMBA_CA53_I2C0_BASE_ADDR;
    AmbaMisra_TypeCast(&pAmbaI2C_MasterReg[0], &base_addr);
    base_addr = AMBA_CA53_I2C1_BASE_ADDR;
    AmbaMisra_TypeCast(&pAmbaI2C_MasterReg[1], &base_addr);
    base_addr = AMBA_CA53_I2C2_BASE_ADDR;
    AmbaMisra_TypeCast(&pAmbaI2C_MasterReg[2], &base_addr);
    base_addr = AMBA_CA53_I2C3_BASE_ADDR;
    AmbaMisra_TypeCast(&pAmbaI2C_MasterReg[3], &base_addr);
    base_addr = AMBA_CA53_I2C4_BASE_ADDR;
    AmbaMisra_TypeCast(&pAmbaI2C_MasterReg[4], &base_addr);
    base_addr = AMBA_CA53_I2C5_BASE_ADDR;
    AmbaMisra_TypeCast(&pAmbaI2C_MasterReg[5], &base_addr);

    base_addr = AMBA_CA53_I2C_SLAVE_BASE_ADDR;
    AmbaMisra_TypeCast(&pAmbaI2C_SlaveReg, &base_addr);
#elif defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    base_addr = AMBA_CORTEX_A76_I2C0_BASE_ADDR;
    AmbaMisra_TypeCast(&pAmbaI2C_MasterReg[0], &base_addr);
    base_addr = AMBA_CORTEX_A76_I2C1_BASE_ADDR;
    AmbaMisra_TypeCast(&pAmbaI2C_MasterReg[1], &base_addr);
    base_addr = AMBA_CORTEX_A76_I2C2_BASE_ADDR;
    AmbaMisra_TypeCast(&pAmbaI2C_MasterReg[2], &base_addr);
    base_addr = AMBA_CORTEX_A76_I2C3_BASE_ADDR;
    AmbaMisra_TypeCast(&pAmbaI2C_MasterReg[3], &base_addr);
    base_addr = AMBA_CORTEX_A76_I2C4_BASE_ADDR;
    AmbaMisra_TypeCast(&pAmbaI2C_MasterReg[4], &base_addr);
    base_addr = AMBA_CORTEX_A76_I2C5_BASE_ADDR;
    AmbaMisra_TypeCast(&pAmbaI2C_MasterReg[5], &base_addr);

    base_addr = AMBA_CORTEX_A76_I2C_SLAVE_BASE_ADDR;
    AmbaMisra_TypeCast(&pAmbaI2C_SlaveReg, &base_addr);
#else
    base_addr = AMBA_CORTEX_A53_I2C0_BASE_ADDR;
    AmbaMisra_TypeCast(&pAmbaI2C_MasterReg[0], &base_addr);
    base_addr = AMBA_CORTEX_A53_I2C1_BASE_ADDR;
    AmbaMisra_TypeCast(&pAmbaI2C_MasterReg[1], &base_addr);
    base_addr = AMBA_CORTEX_A53_I2C2_BASE_ADDR;
    AmbaMisra_TypeCast(&pAmbaI2C_MasterReg[2], &base_addr);
    base_addr = AMBA_CORTEX_A53_I2C3_BASE_ADDR;
    AmbaMisra_TypeCast(&pAmbaI2C_MasterReg[3], &base_addr);

    base_addr = AMBA_CORTEX_A53_I2C_SLAVE_BASE_ADDR;
    AmbaMisra_TypeCast(&pAmbaI2C_SlaveReg, &base_addr);
#endif

#if defined(CONFIG_DEVICE_TREE_SUPPORT)
    for (i = 0U; i < AMBA_NUM_I2C_CHANNEL; i++) {
        if (i != 0U) {
            offset = IO_UtilityFDTNodeOffsetByCID(offset, "ambarella,i2c");
        }
        base_addr = IO_UtilityFDTPropertyU32Quick(offset, "ambarella,i2c", "reg", 0U);
        if ( base_addr != 0UL ) {
#if defined (CONFIG_SOC_CV5)|| defined (CONFIG_SOC_CV52)
            base_addr = base_addr | AMBA_CORTEX_A76_APB_VIRT_BASE_ADDR;
#endif
            AmbaMisra_TypeCast(&pAmbaI2C_MasterReg[i], &base_addr);
        }
    }
#endif

    IntConfig.TriggerType = INT_TRIG_HIGH_LEVEL;
    IntConfig.IrqType = INT_TYPE_FIQ;
    IntConfig.CpuTargets = 0x01U;

    for (i = 0U; i < AMBA_NUM_I2C_CHANNEL; i++) {
        (void)AmbaRTSL_GicIntConfig(AmbaI2cMasterIRQ[i], &IntConfig, I2C_MasterIntHandler, i);

        /* Force duty cycle to be 1:1 */
        AmbaCSL_I2cSetDutyCycleZero(pAmbaI2C_MasterReg[i]);
    }

    for (i = 0U; i < AMBA_NUM_I2C_SLAVE; i++) {
        (void)AmbaRTSL_GicIntConfig(AmbaI2cSlaveIRQ[i], &IntConfig, I2C_SlaveIntHandler, i);
    }

    return I2C_ERR_NONE;
}

/**
 *  AmbaRTSL_I2cIntHookHandler - Hook I2C Interrupt Service Routine (ISR)
 *  @param[in] I2cMasterIsrCb Interrupt Service Routine for I2C masters
 *  @return error code
 */
UINT32 AmbaRTSL_I2cIntHookHandler(AMBA_I2C_ISR_f I2cMasterIsrCb)
{
    AmbaI2cMasterIsrCb = I2cMasterIsrCb;

    return I2C_ERR_NONE;
}

/**
 *  AmbaRTSL_I2cWrite - I2C Master write data operation
 *  @param[in] I2cChanNo I2C channel number
 *  @param[in] I2cSpeed Bus speed mode with preset timing parameters
 *  @param[in] pTxTransaction Pointer to TX transaction
 *  @return error code
 */
UINT32 AmbaRTSL_I2cWrite(UINT32 I2cChanNo, UINT32 I2cSpeed, const AMBA_I2C_TRANSACTION_s *pTxTransaction)
{
    AMBA_I2C_REG_s *pI2cReg;
    AMBA_I2C_CTRL_s *pI2cCtrl;
    UINT32 RetVal = I2C_ERR_NONE;

    if ((I2cChanNo >= AMBA_NUM_I2C_CHANNEL) ||
        (pTxTransaction == NULL) || (pTxTransaction->DataSize == 0U) || (pTxTransaction->pDataBuf == NULL)) {
        RetVal = I2C_ERR_ARG;
    } else {
        I2C_CheckStatus(I2cChanNo);

        pI2cReg = pAmbaI2C_MasterReg[I2cChanNo];
        pI2cCtrl = (AMBA_I2C_CTRL_s *) & (AmbaI2cCtrl[I2cChanNo]);

        (void)AmbaRTSL_GicIntEnable(AmbaI2cMasterIRQ[I2cChanNo]);

        RetVal = I2C_Config(pI2cReg, I2cSpeed);
        if (RetVal == I2C_ERR_NONE) {
            pI2cCtrl->Status = 0U;
            pI2cCtrl->ActualSize = 0U;
            pI2cCtrl->VarLenInfo = 0U;
            pI2cCtrl->pTxTransaction = pTxTransaction;
            pI2cCtrl->pRxTransaction = NULL;
            pI2cCtrl->NumTxTransaction = 1U;
            pI2cCtrl->NumRxTransaction = 0U;
            pI2cCtrl->TxTransaction.SlaveAddr = pTxTransaction->SlaveAddr;
            pI2cCtrl->TxTransaction.DataSize = pTxTransaction->DataSize;
            pI2cCtrl->TxTransaction.pDataBuf = pTxTransaction->pDataBuf;

            if (I2cSpeed == I2C_SPEED_HIGH) {
                I2C_MasterHsWriteData(I2cChanNo);
            }

            /* Write Slave Address: r/w bit = 0(w) */
            RetVal = I2C_Start(pI2cReg, I2cSpeed, pTxTransaction->SlaveAddr & 0xfeU);
        }
    }

    return RetVal;
}

/**
 *  AmbaRTSL_I2cRead - I2C Master read data operation
 *  @param[in] I2cChanNo I2C channel number
 *  @param[in] I2cSpeed Bus speed mode with preset timing parameters
 *  @param[in] pRxTransaction Pointer to RX transaction
 *  @return error code
 */
UINT32 AmbaRTSL_I2cRead(UINT32 I2cChanNo, UINT32 I2cSpeed, const AMBA_I2C_TRANSACTION_s *pRxTransaction)
{
    AMBA_I2C_REG_s *pI2cReg;
    AMBA_I2C_CTRL_s *pI2cCtrl;
    UINT32 RetVal = I2C_ERR_NONE;

    if ((I2cChanNo >= AMBA_NUM_I2C_CHANNEL) ||
        (pRxTransaction == NULL) || (pRxTransaction->DataSize == 0U) || (pRxTransaction->pDataBuf == NULL)) {
        RetVal = I2C_ERR_ARG;
    } else {
        I2C_CheckStatus(I2cChanNo);

        pI2cReg = pAmbaI2C_MasterReg[I2cChanNo];
        pI2cCtrl = (AMBA_I2C_CTRL_s *) & (AmbaI2cCtrl[I2cChanNo]);

        (void)AmbaRTSL_GicIntEnable(AmbaI2cMasterIRQ[I2cChanNo]);

        RetVal = I2C_Config(pI2cReg, I2cSpeed);
        if (RetVal == I2C_ERR_NONE) {
            pI2cCtrl->Status = AMBA_I2C_RX_MODE_START;
            pI2cCtrl->ActualSize = 0U;
            pI2cCtrl->VarLenInfo = 0U;
            pI2cCtrl->pTxTransaction = NULL;
            pI2cCtrl->pRxTransaction = pRxTransaction;
            pI2cCtrl->NumTxTransaction = 0U;
            pI2cCtrl->NumRxTransaction = 1U;
            pI2cCtrl->RxTransaction.SlaveAddr = pRxTransaction->SlaveAddr;
            pI2cCtrl->RxTransaction.DataSize = pRxTransaction->DataSize;
            pI2cCtrl->RxTransaction.pDataBuf = pRxTransaction->pDataBuf;

            /* Write Slave Address: r/w bit = 1(r) */
            RetVal = I2C_Start(pI2cReg, I2cSpeed, pRxTransaction->SlaveAddr | 0x01U);
        }
    }

    return RetVal;
}

/**
 *  AmbaRTSL_I2cReadAfterWrite - I2C Master read data operation after multi-byte writing
 *  @param[in] I2cChanNo I2C channel number
 *  @param[in] I2cSpeed I2C communication speed: Standard(100Kbps)/Fast-Mode(400Kbps)/Fast-mode plus(1Mbps)/High-speed mode(3.4Mbps)
 *  @param[in] NumTxTransaction Number of TX transactions
 *  @param[in] pTxTransaction Pointer to TX transaction
 *  @param[in] pRxransaction Pointer to RX transaction
 *  @return error code
 */
UINT32 AmbaRTSL_I2cReadAfterWrite(UINT32 I2cChanNo, UINT32 I2cSpeed, UINT32 NumTxTransaction, const AMBA_I2C_TRANSACTION_s *pTxTransaction,
                                  const AMBA_I2C_TRANSACTION_s *pRxTransaction)
{
    AMBA_I2C_REG_s *pI2cReg;
    AMBA_I2C_CTRL_s *pI2cCtrl;
    UINT32 RetVal = I2C_ERR_NONE;

    if ((I2cChanNo >= AMBA_NUM_I2C_CHANNEL) || (NumTxTransaction == 0U) || (pTxTransaction == NULL) || (pRxTransaction == NULL)) {
        RetVal = I2C_ERR_ARG;
    } else {
        I2C_CheckStatus(I2cChanNo);

        pI2cReg = pAmbaI2C_MasterReg[I2cChanNo];
        pI2cCtrl = (AMBA_I2C_CTRL_s *) & (AmbaI2cCtrl[I2cChanNo]);

        (void)AmbaRTSL_GicIntEnable(AmbaI2cMasterIRQ[I2cChanNo]);

        RetVal = I2C_Config(pI2cReg, I2cSpeed);
        if (RetVal == I2C_ERR_NONE) {
            pI2cCtrl->Status = 0U;
            pI2cCtrl->ActualSize = 0U;
            pI2cCtrl->VarLenInfo = 0U;
            pI2cCtrl->pTxTransaction = pTxTransaction;
            pI2cCtrl->pRxTransaction = pRxTransaction;
            pI2cCtrl->NumTxTransaction = NumTxTransaction;
            pI2cCtrl->NumRxTransaction = 1U;
            pI2cCtrl->TxTransaction.SlaveAddr = pTxTransaction->SlaveAddr;
            pI2cCtrl->TxTransaction.DataSize = pTxTransaction->DataSize;
            pI2cCtrl->TxTransaction.pDataBuf = pTxTransaction->pDataBuf;
            pI2cCtrl->RxTransaction.SlaveAddr = pRxTransaction->SlaveAddr;
            pI2cCtrl->RxTransaction.DataSize = pRxTransaction->DataSize;
            pI2cCtrl->RxTransaction.pDataBuf = pRxTransaction->pDataBuf;

            /* Write the 1st Data: in general it is the Slave Address; r/w bit = 0(w) */
            RetVal = I2C_Start(pI2cReg, I2cSpeed, pTxTransaction->SlaveAddr & 0xfeU);
        }
    }

    return RetVal;
}

/**
 *  AmbaRTSL_I2cReadVarLength - I2C Master read data operation
 *  @param[in] I2cChanNo I2C channel number
 *  @param[in] I2cSpeed I2C communication speed: Standard(100Kbps)/Fast-Mode(400Kbps)/Fast-mode plus(1Mbps)/High-speed mode(3.4Mbps)
 *  @param[in] VarLenInfo Number of bytes used to indicate the variable length information
 *  @param[in] pRxransaction Pointer to RX transaction
 *  @return error code
 */
UINT32 AmbaRTSL_I2cReadVarLength(UINT32 I2cChanNo, UINT32 I2cSpeed, UINT32 VarLenInfo, const AMBA_I2C_TRANSACTION_s *pRxTransaction)
{
    AMBA_I2C_REG_s *pI2cReg;
    AMBA_I2C_CTRL_s *pI2cCtrl;
    UINT32 RetVal = I2C_ERR_NONE;

    if ((I2cChanNo >= AMBA_NUM_I2C_CHANNEL) || (VarLenInfo == 0U) || (VarLenInfo > 4U) ||
        (pRxTransaction == NULL) || (pRxTransaction->DataSize == 0U) || (pRxTransaction->pDataBuf == NULL) ||
        (pRxTransaction->DataSize < VarLenInfo)) {
        RetVal = I2C_ERR_ARG;
    } else {
        I2C_CheckStatus(I2cChanNo);

        pI2cReg = pAmbaI2C_MasterReg[I2cChanNo];
        pI2cCtrl = (AMBA_I2C_CTRL_s *) & (AmbaI2cCtrl[I2cChanNo]);

        (void)AmbaRTSL_GicIntEnable(AmbaI2cMasterIRQ[I2cChanNo]);

        RetVal = I2C_Config(pI2cReg, I2cSpeed);
        if (RetVal == I2C_ERR_NONE) {
            pI2cCtrl->Status = AMBA_I2C_RX_GET_VAR_LENGTH;
            pI2cCtrl->ActualSize = 0U;
            pI2cCtrl->VarLenInfo = VarLenInfo;
            pI2cCtrl->pTxTransaction = NULL;
            pI2cCtrl->pRxTransaction = pRxTransaction;
            pI2cCtrl->NumTxTransaction = 0U;
            pI2cCtrl->NumRxTransaction = 1U;
            pI2cCtrl->RxTransaction.SlaveAddr = pRxTransaction->SlaveAddr;
            pI2cCtrl->RxTransaction.DataSize = 0U;  /* the value will be updated by the first VarLenInfo bytes from slave */
            pI2cCtrl->RxTransaction.pDataBuf = pRxTransaction->pDataBuf;

            /* Write Slave Address: r/w bit = 1(r) */
            RetVal = I2C_Start(pI2cReg, I2cSpeed, pRxTransaction->SlaveAddr | 0x01U);
        }
    }

    return RetVal;
}

/**
 *  AmbaRTSL_I2cStop - Stop I2C channel
 *  @param[in] I2cChanNo I2C channel number
 *  @return error code
 */
UINT32 AmbaRTSL_I2cStop(UINT32 I2cChanNo)
{
    AMBA_I2C_REG_s *pI2cReg;
    AMBA_I2C_CTRL_s *pI2cCtrl;
    UINT32 RetVal = I2C_ERR_NONE;

    if (I2cChanNo >= AMBA_NUM_I2C_CHANNEL) {
        RetVal = I2C_ERR_ARG;
    } else {
        pI2cReg = pAmbaI2C_MasterReg[I2cChanNo];
        pI2cCtrl = (AMBA_I2C_CTRL_s *) & (AmbaI2cCtrl[I2cChanNo]);

        AmbaCSL_I2cStop(pI2cReg);
        AmbaCSL_I2cDisable(pI2cReg);

        pI2cCtrl->Status = 0U;
        pI2cCtrl->ActualSize = 0U;
        pI2cCtrl->VarLenInfo = 0U;
        pI2cCtrl->NumTxTransaction = 0x0U;
        pI2cCtrl->NumRxTransaction = 0x0U;
        pI2cCtrl->pTxTransaction = NULL;
        pI2cCtrl->pRxTransaction = NULL;

        (void)AmbaRTSL_GicIntDisable(AmbaI2cMasterIRQ[I2cChanNo]);
    }

    return RetVal;
}

/**
 *  AmbaRTSL_I2cTerminate - Force terminating the I2C transaction
 *  @param[in] I2cChanNo I2C channel number
 *  @return error code
 */
UINT32 AmbaRTSL_I2cTerminate(UINT32 I2cChanNo)
{
    if (I2cChanNo < AMBA_NUM_I2C_CHANNEL) {
        AmbaI2cCtrl[I2cChanNo].Status |= AMBA_I2C_FORCE_TERMINATE;
    }

    return I2C_ERR_NONE;
}

/**
 *  AmbaRTSL_I2cGetResponse - Get the response from I2C slave
 *  @param[in] I2cChanNo I2C channel number
 *  @return ACK(0)/NACK(1)
 */
UINT32 AmbaRTSL_I2cGetResponse(UINT32 I2cChanNo)
{
    UINT32 RetVal = 0U;

    if (I2cChanNo >= AMBA_NUM_I2C_CHANNEL) {
        RetVal = 1U;
    } else {
        if ((AmbaI2cCtrl[I2cChanNo].Status & AMBA_I2C_SLAVE_NO_ACK) == AMBA_I2C_SLAVE_NO_ACK) {
            RetVal = 1U;
        } else {
            RetVal = 0U;
        }
    }

    return RetVal;
}

/**
 *  AmbaRTSL_I2cGetActualSize - Get the actual data size among all the transaction.
 *  @param[in] I2cChanNo I2C channel number
 *  @return Total number of transfered data
 */
UINT32 AmbaRTSL_I2cGetActualSize(UINT32 I2cChanNo)
{
    UINT32 RetVal = 0U;

    if (I2cChanNo < AMBA_NUM_I2C_CHANNEL) {
        RetVal = AmbaI2cCtrl[I2cChanNo].ActualSize;
    }

    return RetVal;
}

/**
 *  AmbaRTSL_I2cSlaveStart - Start an I2C slave service
 *  @param[in] SlaveAddr The slave address to be listen
 *  @param[in] WriteIntFunc Callback function at slave write (master read)
 *  @param[in] ReadIntFunc Callback function at slave read (master write)
 *  @param[in] BreakIntFunc Callback function at repeated-START or STOP condition
 *  @return error code
 */
UINT32 AmbaRTSL_I2cSlaveStart(UINT32 SlaveAddr, AMBA_I2C_SLAVE_WRITE_ISR_f WriteIntFunc, AMBA_I2C_SLAVE_READ_ISR_f ReadIntFunc, AMBA_I2C_SLAVE_BREAK_ISR_f BreakIntFunc)
{
    UINT32 RetVal = I2C_ERR_NONE;

    AmbaI2cSlaveWriteIsrCb = WriteIntFunc;
    AmbaI2cSlaveReadIsrCb = ReadIntFunc;
    AmbaI2cSlaveBreakIsrCb = BreakIntFunc;

    (void)AmbaRTSL_GicIntEnable(AmbaI2cSlaveIRQ[0]);

    /* Update apb clock */
    ApbClock = AmbaRTSL_PllGetApbClk();

    I2C_SlaveReset(SlaveAddr);

    return RetVal;
}

/**
 *  AmbaRTSL_I2cSlaveStop - Stop an I2C slave service
 *  @return error code
 */
UINT32 AmbaRTSL_I2cSlaveStop(void)
{
    UINT32 RetVal = I2C_ERR_NONE;

    AmbaCSL_I2cSlaveDisable();
    (void)AmbaRTSL_GicIntDisable(AmbaI2cSlaveIRQ[0]);

    return RetVal;
}

/**
 *  I2C_Config - Set I2C Bus Speed
 *  @param[in] pI2cReg pointer to I2C H/W Registers
 *  @param[in] I2cSpeed I2C communication speed
 *  @return error code
 */
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS) || defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV28)
static UINT32 I2C_Config(AMBA_I2C_REG_s *pI2cReg, UINT32 I2cSpeed)
{
    static const UINT32 I2cSpeedFactor[4U] = {
        [I2C_SPEED_STANDARD]   = 400000U,   /* for I2C Standard speed: 100Kbps */
        [I2C_SPEED_FAST]       = 6281458U,  /* for I2C Fast speed: 330Kbps */
        [I2C_SPEED_FAST_PLUS]  = 15765618U, /* for I2C Fast-mode Plus speed: 1Mbps */
        [I2C_SPEED_HIGH]       = 13600000U  /* for I2C High-speed mode: 3.4Mbps */
    };
    UINT32 RetVal = I2C_ERR_NONE;
    UINT32 PrescalerVal, TurboPrescalerVal;
    UINT32 CheckCount = 0U;

    if (I2cSpeed >= NUM_I2C_SPEED) {
        RetVal = I2C_ERR_ARG;
    } else {
        RetVal = AmbaCSL_I2cGetStatus(pI2cReg);
        while ((RetVal != 0U) && (CheckCount < 2000U)) {
            AmbaDelayCycles(2000U);
            CheckCount++;
            RetVal = AmbaCSL_I2cGetStatus(pI2cReg);
        }

        /*
         * I2C period = (APB bus clock) / (4 * (prescaler[15:0] + 1) + 2)
         * 4 * (prescaler[15:0] + 1) = ((APB bus clock) / I2C period) - 2
         * prescaler[15:0] = ((((APB bus clock) / I2C period) - 2) / 4) - 1
         * prescaler[15:0] = (((APB bus clock) / ((I2C Period) * 4))) - 0.5) - 1
         */
        if (I2cSpeed == I2C_SPEED_HIGH) {
            PrescalerVal = (AmbaRTSL_PllGetApbClk() / I2cSpeedFactor[I2C_SPEED_STANDARD]) - 1U;
            TurboPrescalerVal = (AmbaRTSL_PllGetApbClk() / I2cSpeedFactor[I2C_SPEED_HIGH]) - 1U;

            AmbaCSL_I2cStop(pI2cReg);
            AmbaCSL_I2cDisable(pI2cReg);

            AmbaCSL_I2cSetPrescaler(pI2cReg, PrescalerVal);
            AmbaCSL_I2cSetTurboPrescalerByte(pI2cReg, TurboPrescalerVal);
        } else {
            PrescalerVal = (AmbaRTSL_PllGetApbClk() / I2cSpeedFactor[I2cSpeed]) - 1U;

            AmbaCSL_I2cStop(pI2cReg);
            AmbaCSL_I2cDisable(pI2cReg);

            AmbaCSL_I2cSetPrescaler(pI2cReg, PrescalerVal);
        }

        /* Only the active master can enable its current-source pull-up circuit on SCL output */
        if (I2cSpeed == I2C_SPEED_HIGH) {
            AmbaCSL_I2cEnableSdaCurrentSrc(pI2cReg);
            AmbaCSL_I2cEnableSclCurrentSrc(pI2cReg);
        } else {
            AmbaCSL_I2cDisableSdaCurrentSrc(pI2cReg);
            AmbaCSL_I2cDisableSclCurrentSrc(pI2cReg);
        }

        if (I2cSpeed == I2C_SPEED_STANDARD) {
            AmbaCSL_I2cSetStretchScl(pI2cReg, (UINT32)1U);
        } else if (I2cSpeed == I2C_SPEED_FAST) {
            AmbaCSL_I2cSetStretchScl(pI2cReg, (UINT32)2U);
        } else if (I2cSpeed == I2C_SPEED_FAST_PLUS) {
            AmbaCSL_I2cSetStretchScl(pI2cReg, (UINT32)1U);
        } else {
            AmbaCSL_I2cSetStretchScl(pI2cReg, (UINT32)0U);
        }

        if (I2cSpeed == I2C_SPEED_FAST) {
            AmbaCSL_I2cEnableDefinedSclMode(pI2cReg);
            AmbaCSL_I2cSetPhaseNum(pI2cReg, 14U);
            AmbaCSL_I2cSetRisingPhase(pI2cReg, 1U);
            AmbaCSL_I2cSetFallingPhase(pI2cReg, 6U);
        } else if (I2cSpeed == I2C_SPEED_FAST_PLUS) {
            AmbaCSL_I2cEnableDefinedSclMode(pI2cReg);
            AmbaCSL_I2cSetPhaseNum(pI2cReg, 14U);
            AmbaCSL_I2cSetRisingPhase(pI2cReg, 1U);
            AmbaCSL_I2cSetFallingPhase(pI2cReg, 7U);
        } else {
            AmbaCSL_I2cDisableDefinedSclMode(pI2cReg);
        }

        AmbaCSL_I2cEnable(pI2cReg);
    }

    return RetVal;
}
#else
static UINT32 I2C_Config(AMBA_I2C_REG_s *pI2cReg, UINT32 I2cSpeed)
{
    static const UINT32 I2cSpeedFactor[4U] = {
        [I2C_SPEED_STANDARD]   = 400000U,   /* for I2C Standard speed: 100Kbps */
        [I2C_SPEED_FAST]       = 1320000U,  /* for I2C Fast speed: 330Kbps */
        [I2C_SPEED_FAST_PLUS]  = 4000000U,  /* for I2C Fast-mode Plus speed: 1Mbps */
        [I2C_SPEED_HIGH]       = 13600000U  /* for I2C High-speed mode: 3.4Mbps */
    };
    UINT32 RetVal = I2C_ERR_NONE;
    UINT32 PrescalerVal, TurboPrescalerVal;

    if (I2cSpeed >= NUM_I2C_SPEED) {
        RetVal = I2C_ERR_ARG;
    } else {
        /*
         * I2C period = (APB bus clock) / (4 * (prescaler[15:0] + 1) + 2)
         * 4 * (prescaler[15:0] + 1) = ((APB bus clock) / I2C period) - 2
         * prescaler[15:0] = ((((APB bus clock) / I2C period) - 2) / 4) - 1
         * prescaler[15:0] = (((APB bus clock) / ((I2C Period) * 4))) - 0.5) - 1
         */
        if (I2cSpeed == I2C_SPEED_HIGH) {
            PrescalerVal = (AmbaRTSL_PllGetApbClk() / I2cSpeedFactor[I2C_SPEED_STANDARD]) - 1U;
            TurboPrescalerVal = (AmbaRTSL_PllGetApbClk() / I2cSpeedFactor[I2C_SPEED_HIGH]) - 1U;

            AmbaCSL_I2cStop(pI2cReg);
            AmbaCSL_I2cDisable(pI2cReg);

            AmbaCSL_I2cSetPrescaler(pI2cReg, PrescalerVal);
            AmbaCSL_I2cSetTurboPrescalerByte(pI2cReg, TurboPrescalerVal);
        } else {
            PrescalerVal = (AmbaRTSL_PllGetApbClk() / I2cSpeedFactor[I2cSpeed]) - 1U;

            AmbaCSL_I2cStop(pI2cReg);
            AmbaCSL_I2cDisable(pI2cReg);

            AmbaCSL_I2cSetPrescaler(pI2cReg, PrescalerVal);
        }

        /* Only the active master can enable its current-source pull-up circuit on SCL output */
        if (I2cSpeed == I2C_SPEED_HIGH) {
            AmbaCSL_I2cEnableSdaCurrentSrc(pI2cReg);
            AmbaCSL_I2cEnableSclCurrentSrc(pI2cReg);
        } else {
            AmbaCSL_I2cDisableSdaCurrentSrc(pI2cReg);
            AmbaCSL_I2cDisableSclCurrentSrc(pI2cReg);
        }

        if (I2cSpeed == I2C_SPEED_STANDARD) {
            AmbaCSL_I2cSetStretchScl(pI2cReg, (UINT32)1U);
        } else {
            AmbaCSL_I2cSetStretchScl(pI2cReg, (UINT32)0U);
        }

        AmbaCSL_I2cEnable(pI2cReg);
    }

    return RetVal;
}
#endif

/**
 *  I2C_Start - Start an I2C transaction
 *  @param[in] pI2cReg pointer to I2C H/W Registers
 *  @param[in] I2cSpeed I2C communication speed
 *  @param[in] SlaveAddr I2C slave address
 *  @return error code
 */
static UINT32 I2C_Start(AMBA_I2C_REG_s *pI2cReg, UINT32 I2cSpeed, UINT32 SlaveAddr)
{
    AmbaCSL_I2cWriteData(pI2cReg, SlaveAddr);

    if (I2cSpeed == I2C_SPEED_HIGH) {
        AmbaCSL_I2cHsStart(pI2cReg);
    } else {
        AmbaCSL_I2cStart(pI2cReg);
    }

    return I2C_ERR_NONE;
}

/**
 *  I2C_CheckStatus - Check I2C FSM status and recover it if necessary.
 *  @param[in] I2cChanNo I2C channel number
 *  @return error code
 */
static void I2C_CheckStatus(UINT32 I2cChanNo)
{
    AMBA_I2C_REG_s *pI2cReg;
    UINT32 Status;
    UINT32 CheckCount = 0U;

    pI2cReg = pAmbaI2C_MasterReg[I2cChanNo];
    Status = AmbaCSL_I2cGetStatus(pI2cReg);

    if (Status != 0U) {
        /* There should be error in I2C FSM, recover it by trigger I2C again */
        AmbaCSL_I2cEnable(pI2cReg);
        AmbaCSL_I2cStop(pI2cReg);
    }

    Status = AmbaCSL_I2cGetStatus(pI2cReg);
    while ((Status != 0U) && (CheckCount < 2000U)) {
        AmbaDelayCycles(2000U);
        CheckCount++;
        Status = AmbaCSL_I2cGetStatus(pI2cReg);
    }
}

/**
 *  I2C_SlaveReset - Re-start an I2C transaction
 *  @param[in] SlaveAddr I2C slave address
 *  @return error code
 */
static void I2C_SlaveReset(UINT32 SlaveAddr)
{
    UINT32 HoldTime;

    AmbaCSL_I2cSlaveReset();
    AmbaCSL_I2cSlaveSetAddr(SlaveAddr);

    /* (1)A device must internally provide a hold time of at least 300 ns for the SDA signal (with respect to the VIH(min) of the SCL signal)
     *    to bridge the undefined region of the falling edge of SCL.
     * (2)The maximum tHD;DAT could be 3.45 us and 0.9 us for Standard-mode and Fast-mode,
     *    but must be less than the maximum of tVD;DAT or tVD;ACK by a transition time.
     *    This maximum must only be met if the device does not stretch the LOW period (tLOW) of the SCL signal.
     *    If the clock stretches the SCL, the data must be valid by the set-up time before it releases the clock.
     */
    HoldTime = ApbClock;
    HoldTime *= 3U;
    HoldTime /= 10000000U;
    HoldTime += 1U;
    AmbaCSL_I2cSlaveSetHoldTime((UINT8)HoldTime);

    AmbaCSL_I2cSlaveSetSclStretch(0xffffffffU); /* Keep pull SCL pin low before related software process is executed */
    AmbaCSL_I2cSlaveSetRxFifoThr(0x0U);
    AmbaCSL_I2cSlaveSetTxFifoThr(0x1U);
    AmbaCSL_I2cSlaveEnableIrq();
    AmbaCSL_I2cSlaveEnable();
}

/**
 *  I2C_MasterCheckACK - Check the response from slave
 *  @param[in] I2cChanNo I2C channel number
 *  @return ACK(0)/NACK(1)
 */
static UINT32 I2C_MasterCheckACK(UINT32 I2cChanNo)
{
    AMBA_I2C_CTRL_s *pI2cCtrl = (AMBA_I2C_CTRL_s *) & (AmbaI2cCtrl[I2cChanNo]);
    AMBA_I2C_REG_s *pI2cReg = pAmbaI2C_MasterReg[I2cChanNo];
    UINT32 RetVal = 0U;

    /* The following code segment is necessary for the device cannot ACK on time.                */
    /* However it needs to skip the check for those devices never ACK. (e.g., some HDMI devices) */
#if defined(CONFIG_SOC_CV22) || defined(CONFIG_SOC_CV2) || defined(CONFIG_SOC_CV25) || defined(CONFIG_SOC_H32) || defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    if (I2cChanNo != AMBA_I2C_HDMI_CHANNEL) {
#endif
        if ((pI2cReg->Ctrl & 0x1U) != 0x0U) {
            /* error: haven't received ACK from slave */
            pI2cCtrl->Status |= AMBA_I2C_SLAVE_NO_ACK;
            AmbaCSL_I2cStop(pI2cReg);
            AmbaCSL_I2cDisable(pI2cReg);

            if (AmbaI2cMasterIsrCb != NULL) {
                AmbaI2cMasterIsrCb(I2cChanNo);
            }

            RetVal = 1U;
        }
#if defined(CONFIG_SOC_CV22) || defined(CONFIG_SOC_CV2) || defined(CONFIG_SOC_CV25) || defined(CONFIG_SOC_H32) || defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    }
#endif

    return RetVal;
}

/**
 *  AmbaRTSL_I2cGetFinalState - Get the final state of I2C driver
 *  @param[in] I2cChanNo I2C channel number
 *  @return OK(0)/error(1)
 */
UINT32 AmbaRTSL_I2cGetFinalState(UINT32 I2cChanNo)
{
    UINT32 RetVal = 0U;

    if (I2cChanNo >= AMBA_NUM_I2C_CHANNEL) {
        RetVal = 1U;
    } else {
        if ((AmbaI2cCtrl[I2cChanNo].Status & AMBA_I2C_STATE_ERROR) == AMBA_I2C_STATE_ERROR) {
            RetVal = 1U;
        } else {
            RetVal = 0U;
        }
    }

    return RetVal;
}

/**
 *  I2C_MasterWriteIntHandler - I2C ISR for Writing data
 *  @param[in] I2cChanNo I2C channel number
 */
static void I2C_MasterWriteIntHandler(UINT32 I2cChanNo)
{
    AMBA_I2C_CTRL_s *pI2cCtrl = (AMBA_I2C_CTRL_s *) & (AmbaI2cCtrl[I2cChanNo]);
    AMBA_I2C_REG_s *pI2cReg = pAmbaI2C_MasterReg[I2cChanNo];
    AMBA_I2C_TRANSACTION_s *pTxTransaction;

    if (I2C_MasterCheckACK(I2cChanNo) == 0x0U) {
        if ((pI2cCtrl->NumTxTransaction != 0U)) {
            pTxTransaction = &(pI2cCtrl->TxTransaction);
            if (pTxTransaction->DataSize != 0U) {
                AmbaCSL_I2cWriteData(pI2cReg, *(pTxTransaction->pDataBuf));
                pI2cCtrl->ActualSize++;
                pTxTransaction->DataSize--;
                pTxTransaction->pDataBuf++;
                AmbaCSL_I2cClearIrqStatus(pI2cReg);
            } else {
                pI2cCtrl->NumTxTransaction--;
                if ((pI2cCtrl->NumTxTransaction != 0U)) {
                    pI2cCtrl->pTxTransaction++;
                    pI2cCtrl->TxTransaction.SlaveAddr = pI2cCtrl->pTxTransaction->SlaveAddr;
                    pI2cCtrl->TxTransaction.DataSize = pI2cCtrl->pTxTransaction->DataSize;
                    pI2cCtrl->TxTransaction.pDataBuf = pI2cCtrl->pTxTransaction->pDataBuf;
                    AmbaCSL_I2cWriteData(pI2cReg, pTxTransaction->SlaveAddr);
                    AmbaCSL_I2cStart(pI2cReg);  /* generate a Restart (Repeated Start) condition */
                } else if (pI2cCtrl->NumRxTransaction != 0U) {
                    pI2cCtrl->Status = AMBA_I2C_RX_MODE_START;
                    pTxTransaction = &(pI2cCtrl->RxTransaction);
                    AmbaCSL_I2cWriteData(pI2cReg, pTxTransaction->SlaveAddr);
                    AmbaCSL_I2cStart(pI2cReg);      /* generate a Restart (Repeated Start) condition */
                } else {
                    AmbaCSL_I2cStop(pI2cReg);
                    AmbaCSL_I2cDisable(pI2cReg);

                    if (AmbaI2cMasterIsrCb != NULL) {
                        AmbaI2cMasterIsrCb(I2cChanNo);
                    }

                }
            }
        } else {
            AmbaCSL_I2cStop(pI2cReg);
            AmbaCSL_I2cDisable(pI2cReg);
            AmbaI2cCtrl[I2cChanNo].Status |= AMBA_I2C_STATE_ERROR;
            /* Shall not happen!! */
        }
    }
}

/**
 *  I2C_MasterHsWriteData - I2C Writing data on the high speed mode
 *  @param[in] I2cChanNo I2C channel number
 */
static void I2C_MasterHsWriteData(UINT32 I2cChanNo)
{
    AMBA_I2C_CTRL_s *pI2cCtrl = (AMBA_I2C_CTRL_s *) & (AmbaI2cCtrl[I2cChanNo]);
    AMBA_I2C_REG_s *pI2cReg = pAmbaI2C_MasterReg[I2cChanNo];
    AMBA_I2C_TRANSACTION_s *pTxTransaction;

    if ((pI2cCtrl->NumTxTransaction != 0U)) {
        pTxTransaction = &(pI2cCtrl->TxTransaction);
        if (pTxTransaction->DataSize != 0U) {
            AmbaCSL_I2cFifoModeStart(pI2cReg);
            AmbaCSL_I2cWriteFifoData(pI2cReg, pTxTransaction->SlaveAddr);

            while (pTxTransaction->DataSize > 0U) {
                AmbaCSL_I2cWriteFifoData(pI2cReg, *(pTxTransaction->pDataBuf));
                pI2cCtrl->ActualSize++;
                pTxTransaction->DataSize--;
                pTxTransaction->pDataBuf++;
            }
        } else {
            /* Do nothing */
        }
    } else {
        AmbaCSL_I2cStop(pI2cReg);
        AmbaCSL_I2cDisable(pI2cReg);
        AmbaI2cCtrl[I2cChanNo].Status |= AMBA_I2C_STATE_ERROR;
        /* Shall not happen!! */
    }
}

/**
 *  I2C_MasterReadIntHandler - I2C ISR for Reading data
 *  @param[in] I2cChanNo I2C channel number
 */
static void I2C_MasterReadIntHandler(UINT32 I2cChanNo)
{
    AMBA_I2C_CTRL_s *pI2cCtrl = (AMBA_I2C_CTRL_s *) & (AmbaI2cCtrl[I2cChanNo]);
    AMBA_I2C_REG_s *pI2cReg = pAmbaI2C_MasterReg[I2cChanNo];
    AMBA_I2C_TRANSACTION_s *pRxTransaction;

    if (pI2cCtrl->NumRxTransaction == 0U) {
        /* Do nothing. All the RX transaction done. */
    } else if ((pI2cCtrl->Status & AMBA_I2C_RX_MODE_START) != 0x0U) {
        /*
         * In this case, the slave address for read is just transfered done.
         * However, the data is not yet transfered from slave. (That's why no read data register.)
         * The master needs to prepare auto-response (NACK/ACK. HW will set the state after the next slave tranfer done.)
         * and then start the slave transfer.
         */
        if (I2C_MasterCheckACK(I2cChanNo) == 0U) {
            pRxTransaction = &(pI2cCtrl->RxTransaction);
            if (pRxTransaction->DataSize == 1U) {
                /* One byte remains */
                AmbaCSL_I2cNACK(pI2cReg);   /* prepare NACK, clear Interrupt and Start bits */
            } else {
                /* More than one byte remains */
                AmbaCSL_I2cACK(pI2cReg);    /* prepare ACK, clear Interrupt and Start bits */
            }

            pI2cCtrl->Status &= ~AMBA_I2C_RX_MODE_START;
        }
    } else {
        pRxTransaction = &(pI2cCtrl->RxTransaction);

        /* Read one byte */
        *pRxTransaction->pDataBuf = (UINT8)pI2cReg->Data.Value;
        pRxTransaction->pDataBuf++;
        pI2cCtrl->ActualSize++;

        if ((pI2cCtrl->Status & AMBA_I2C_RX_GET_VAR_LENGTH) != 0x0U) {
            /* Read Variable Length of data mode: received the first VarLenInfo bytes (= Data Length) */
            pRxTransaction->DataSize = (pRxTransaction->DataSize << 8U) | (UINT32)(*pRxTransaction->pDataBuf);
            pI2cCtrl->VarLenInfo--;

            if (pI2cCtrl->VarLenInfo == 0U) {
                pRxTransaction->DataSize++;         /* Total received data size = Data_Length + 1 */
                if (pRxTransaction->DataSize > (pI2cCtrl->RxTransaction.DataSize - pI2cCtrl->ActualSize)) {
                    pRxTransaction->DataSize = pI2cCtrl->RxTransaction.DataSize - pI2cCtrl->ActualSize;
                }
                pI2cCtrl->Status &= ~AMBA_I2C_RX_GET_VAR_LENGTH;
            }
        } else {
            if (pRxTransaction->DataSize != 0U) {
                pRxTransaction->DataSize--;
                if (pRxTransaction->DataSize == 0U) {
                    /* has received the last byte data */
                    pI2cCtrl->NumRxTransaction--;

                    AmbaCSL_I2cStop(pI2cReg);
                    AmbaCSL_I2cDisable(pI2cReg);

                    if (AmbaI2cMasterIsrCb != NULL) {
                        AmbaI2cMasterIsrCb(I2cChanNo);
                    }
                } else {
                    if (pRxTransaction->DataSize == 1U) {
                        /* One byte remains */
                        AmbaCSL_I2cNACK(pI2cReg);   /* prepare NACK, clear Interrupt and Start bits */
                    } else {
                        /* More than one byte remains */
                        AmbaCSL_I2cACK(pI2cReg);    /* prepare ACK, clear Interrupt and Start bits */
                    }
                }
            } else {
                AmbaCSL_I2cStop(pI2cReg);
                AmbaCSL_I2cDisable(pI2cReg);
                AmbaI2cCtrl[I2cChanNo].Status |= AMBA_I2C_STATE_ERROR;
                /* Shall not happen!! */
            }
        }
    }
}

#pragma GCC push_options
#pragma GCC target("general-regs-only")
/**
 *  I2C_MasterIntHandler - I2C Master ISR for Reading/Writing data
 *  @param[in] Irq Interrupt ID
 *  @param[in] I2cChanNo I2C channel number
 */
static void I2C_MasterIntHandler(UINT32 Irq, UINT32 I2cChanNo)
{
    AMBA_I2C_CTRL_s *pI2cCtrl = (AMBA_I2C_CTRL_s *) & (AmbaI2cCtrl[I2cChanNo]);
    AMBA_I2C_REG_s *pI2cReg = pAmbaI2C_MasterReg[I2cChanNo];

    AmbaMisra_TouchUnused(&Irq);

    /* Send a stop condition once get the early end status */
    if ((pI2cCtrl->Status & AMBA_I2C_FORCE_TERMINATE) != 0x0U) {
        pI2cCtrl->Status &= ~AMBA_I2C_FORCE_TERMINATE;    /* Clear the early end status */
        AmbaCSL_I2cStop(pI2cReg);
        AmbaCSL_I2cDisable(pI2cReg);

        if (AmbaI2cMasterIsrCb != NULL) {
            AmbaI2cMasterIsrCb(I2cChanNo);
        }

    } else {
        if ((pI2cReg->Status.OpMode) == 0U) {
            /* Master Write Mode */
            I2C_MasterWriteIntHandler(I2cChanNo);
        } else {
            /* Master Read Mode */
            I2C_MasterReadIntHandler(I2cChanNo);
        }
    }
}

/**
 *  I2C_SlaveIntHandler - I2C Slave ISR for Reading/Writing data
 *  @param[in] Irq Interrupt ID
 *  @param[in] I2cChanNo I2C channel number
 */
static void I2C_SlaveIntHandler(UINT32 Irq, UINT32 I2cChanNo)
{
    static UINT8 I2cDataBuf[AMBA_I2C_SLAVE_FIFO_COUNT];
    UINT8 *pTxData = NULL;
    UINT32 i, NumTxResidual, NumData = 0U;
    UINT32 SlaveAddr, NACK = 0U;

    AmbaMisra_TouchUnused(&Irq);
    AmbaMisra_TouchUnused(&I2cChanNo);

    if (AmbaCSL_I2cSlaveIsGeneralCall() != 0U) {
        SlaveAddr = 0U;
    } else {
        SlaveAddr = AmbaCSL_I2cSlaveGetAddr();
    }

    if (AmbaCSL_I2cSlaveGetOpMode() != 0U) {
        /* Slave Receiver mode */
        SlaveAddr = (SlaveAddr << 1U);

        NumData = AmbaCSL_I2cSlaveGetFifoCount();
        if ((NumData != 0U) && (NumData <= AMBA_I2C_SLAVE_FIFO_COUNT)) {
            for (i = 0U; i < NumData; i ++) {
                I2cDataBuf[i] = (UINT8)AmbaCSL_I2cSlaveReadData();
            }
            AmbaI2cSlaveReadIsrCb(SlaveAddr, NumData, I2cDataBuf, &NACK);
            AmbaCSL_I2cSlaveSendResponse(NACK);
        }

        NumTxResidual = 0U;
    } else {
        /* Slave Transmitter mode */
        NumTxResidual = AmbaCSL_I2cSlaveGetFifoCount();
        NumTxResidual ++;   /* We assume that one more data is in controller shift register */
    }

    /* Repeated-START condition */
    if (AmbaCSL_I2cSlaveGetReStartStatus() != 0U) {
        AmbaI2cSlaveBreakIsrCb(1U, NumTxResidual);
        AmbaCSL_I2cSlaveClearReStartStatus();
    }

    /* STOP condition */
    if (AmbaCSL_I2cSlaveGetStopStatus() != 0U) {
        AmbaI2cSlaveBreakIsrCb(0U, NumTxResidual);
        I2C_SlaveReset(AmbaCSL_I2cSlaveGetAddr());
    } else {
        if (AmbaCSL_I2cSlaveGetOpMode() == 0U) {
            /* Slave Transmitter mode */
            SlaveAddr = (SlaveAddr << 1U) | 1U;

            NumData = AmbaCSL_I2cSlaveGetFifoCount();
            if (NumData == 0U) {
                AmbaI2cSlaveWriteIsrCb(SlaveAddr, AMBA_I2C_SLAVE_FIFO_COUNT, &NumData, &pTxData);
                if (pTxData != NULL) {
                    if (NumData > AMBA_I2C_SLAVE_FIFO_COUNT) {
                        /* Shall not happen! */
                        NumData = AMBA_I2C_SLAVE_FIFO_COUNT;
                    }
                    for (i = 0U; i < NumData; i ++) {
                        AmbaCSL_I2cSlaveWriteData(pTxData[i]);
                    }
                } else {
                    AmbaCSL_I2cSlaveWriteData(0xFF);
                }
            }
        }
        AmbaCSL_I2cSlaveClearIrq();
    }
}
#pragma GCC pop_options
#if defined(CONFIG_APP_FLOW_CARDV_AONI)
int AmbaRTSL_I2cGetIrqStatus(UINT32 I2cChanNo)
{
    return AmbaCSL_I2cGetIrqStatus(pAmbaI2C_MasterReg[I2cChanNo]);
}

void AmbaRTSL_I2cClearIrqStatus(UINT32 I2cChanNo)
{
    AmbaCSL_I2cClearIrqStatus(pAmbaI2C_MasterReg[I2cChanNo]);
}

void AmbaRTSL_I2cMasterIntHandler(UINT32 Irq, UINT32 I2cChanNo)
{
    I2C_MasterIntHandler(Irq, I2cChanNo);
}
#endif
