/*
 * Copyright (c) 2020 Ambarella International LP
 *
 * This file and its contents ("Software") are protected by intellectual
 * property rights including, without limitation, U.S. and/or foreign
 * copyrights. This Software is also the confidential and proprietary
 * information of Ambarella International LP and its licensors. You may not use, reproduce,
 * disclose, distribute, modify, or otherwise prepare derivative works of this
 * Software or any portion thereof except pursuant to a signed license agreement
 * or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
 * In the absence of such an agreement, you agree to promptly notify and return
 * this Software to Ambarella International LP.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 * MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "AmbaTypes.h"
#include "AmbaMisraFix.h"

#include "AmbaRTSL_I2C.h"
#include "AmbaCSL_I2C.h"

static UINT32 I2C_Config(AMBA_I2C_REG_s *pI2cReg, UINT32 I2cSpeed);
static UINT32 I2C_Start(AMBA_I2C_REG_s *pI2cReg, UINT32 I2cSpeed, UINT32 SlaveAddr);

/**
 *  AmbaRTSL_I2cInit - I2C initializations
 *  @return error code
 */
UINT32 AmbaRTSL_I2cInit(void)
{
    /* Force duty cycle to be 1:1 */
    AmbaCSL_I2cSetDutyCycleZero(pAmbaI2C_MasterReg);

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
    UINT32 RetVal = I2C_ERR_NONE;

    if ((I2cChanNo >= AMBA_NUM_I2C_CHANNEL) ||
        (pTxTransaction == NULL) || (pTxTransaction->DataSize == 0U) || (pTxTransaction->pDataBuf == NULL)) {
        RetVal = I2C_ERR_ARG;
    } else {
        pI2cReg = pAmbaI2C_MasterReg;

        RetVal = I2C_Config(pI2cReg, I2cSpeed);

        /* Write Slave Address: r/w bit = 0(w) */
        RetVal = I2C_Start(pI2cReg, I2cSpeed, pTxTransaction->SlaveAddr & 0xfeU);
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
    UINT32 RetVal = I2C_ERR_NONE;

    if ((I2cChanNo >= AMBA_NUM_I2C_CHANNEL) ||
        (pRxTransaction == NULL) || (pRxTransaction->DataSize == 0U) || (pRxTransaction->pDataBuf == NULL)) {
        RetVal = I2C_ERR_ARG;
    } else {
        pI2cReg = pAmbaI2C_MasterReg;

        if (I2cSpeed & 0x10) {
            RetVal = I2C_Config(pI2cReg, I2cSpeed);
        }
        /* Write Slave Address: r/w bit = 1(r) */
        RetVal = I2C_Start(pI2cReg, I2cSpeed, pRxTransaction->SlaveAddr | 0x01U);
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
    UINT32 RetVal = I2C_ERR_NONE;

    if ((I2cChanNo & 0xf) >= AMBA_NUM_I2C_CHANNEL) {
        RetVal = I2C_ERR_ARG;
    } else {
        pI2cReg = pAmbaI2C_MasterReg;

        if (0x0 != (I2cChanNo & 0xf0U)) {
            AmbaCSL_I2cStop(pI2cReg);
            AmbaCSL_I2cDisable(pI2cReg);
        }
        AmbaCSL_I2cClearIrqStatus(pI2cReg);
        AmbaCSL_I2cFifoClear(pI2cReg);
    }

    return RetVal;
}

/**
 *  I2C_Config - Set I2C Bus Speed
 *  @param[in] pI2cReg pointer to I2C H/W Registers
 *  @param[in] I2cSpeed I2C communication speed
 *  @return error code
 */
static UINT32 I2C_Config(AMBA_I2C_REG_s *pI2cReg, UINT32 I2cSpeed)
{
    UINT32 RetVal = I2C_ERR_NONE;
    UINT32 SpeedTmp = I2cSpeed & 0xfU;
    UINT32 Stop = (I2cSpeed & 0xf0U) >> 4U;

    if (SpeedTmp >= 4U) {
        RetVal = I2C_ERR_ARG;
    } else {
        if (0x0 != Stop) {
            AmbaCSL_I2cStop(pI2cReg);
            AmbaCSL_I2cDisable(pI2cReg);
        }

        /* Only the active master can enable its current-source pull-up circuit on SCL output */
        if (SpeedTmp == I2C_SPEED_HIGH) {
            AmbaCSL_I2cEnableSdaCurrentSrc(pI2cReg);
            AmbaCSL_I2cEnableSclCurrentSrc(pI2cReg);
        } else {
            AmbaCSL_I2cDisableSdaCurrentSrc(pI2cReg);
            AmbaCSL_I2cDisableSclCurrentSrc(pI2cReg);
        }

        if (SpeedTmp == I2C_SPEED_STANDARD) {
            AmbaCSL_I2cSetStretchScl(pI2cReg, (UINT32)1U);
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
        } else if (SpeedTmp == I2C_SPEED_FAST) {
            AmbaCSL_I2cSetStretchScl(pI2cReg, (UINT32)2U);
        } else if (SpeedTmp == I2C_SPEED_FAST_PLUS) {
            AmbaCSL_I2cSetStretchScl(pI2cReg, (UINT32)1U);
#endif
        } else {
            AmbaCSL_I2cSetStretchScl(pI2cReg, (UINT32)0U);
        }

#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
        if (SpeedTmp == I2C_SPEED_FAST) {
            AmbaCSL_I2cEnableDefinedSclMode(pI2cReg);
            AmbaCSL_I2cSetPhaseNum(pI2cReg, 14U);
            AmbaCSL_I2cSetRisingPhase(pI2cReg, 1U);
            AmbaCSL_I2cSetFallingPhase(pI2cReg, 6U);
        } else if (SpeedTmp == I2C_SPEED_FAST_PLUS) {
            AmbaCSL_I2cEnableDefinedSclMode(pI2cReg);
            AmbaCSL_I2cSetPhaseNum(pI2cReg, 14U);
            AmbaCSL_I2cSetRisingPhase(pI2cReg, 1U);
            AmbaCSL_I2cSetFallingPhase(pI2cReg, 7U);
        } else {
            AmbaCSL_I2cDisableDefinedSclMode(pI2cReg);
        }
#endif
        AmbaCSL_I2cEnable(pI2cReg);
    }

    return RetVal;
}

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

    if ((I2cSpeed & 0xfU) == I2C_SPEED_HIGH) {
        AmbaCSL_I2cHsStart(pI2cReg);
    } else {
        AmbaCSL_I2cStart(pI2cReg);
    }

    return I2C_ERR_NONE;
}


