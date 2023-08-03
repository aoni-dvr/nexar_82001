/**
 *  @file AmbaCSL_I2C.h
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
 *  @details Definitions & Constants for I2C Chip Support Library (CSL) APIs
 *
 */

#ifndef AMBA_CSL_I2C_H
#define AMBA_CSL_I2C_H

#ifndef AMBA_I2C_DEF_H
#include "AmbaI2C_Def.h"
#endif
#include "AmbaReg_I2C.h"

#define AMBA_I2C_MASTER_FIFO_COUNT      64U
#define AMBA_I2C_SLAVE_FIFO_COUNT       64U

/*
 * Inline Function Definitions
 */
static inline void AmbaCSL_I2cEnable (AMBA_I2C_REG_s *pI2cReg)
{
    pI2cReg->Enable = (UINT32)1U;
}
static inline void AmbaCSL_I2cDisable(AMBA_I2C_REG_s *pI2cReg)
{
    pI2cReg->Enable = 0U;
}
static inline void AmbaCSL_I2cStart(AMBA_I2C_REG_s *pI2cReg)
{
    pI2cReg->Ctrl = 0x04U;
}
static inline void AmbaCSL_I2cHsStart(AMBA_I2C_REG_s *pI2cReg)
{
    pI2cReg->Ctrl = 0x14U;
}
static inline void AmbaCSL_I2cStop(AMBA_I2C_REG_s *pI2cReg)
{
    pI2cReg->Ctrl = 0x08U;
}
static inline void AmbaCSL_I2cClearIrqStatus(AMBA_I2C_REG_s *pI2cReg)
{
    pI2cReg->Ctrl = 0x00U;
}
static inline void AmbaCSL_I2cACK(AMBA_I2C_REG_s *pI2cReg)
{
    pI2cReg->Ctrl = 0x00U;
}
static inline void AmbaCSL_I2cNACK(AMBA_I2C_REG_s *pI2cReg)
{
    pI2cReg->Ctrl = 0x01U;
}
static inline void AmbaCSL_I2cFifoClear(AMBA_I2C_REG_s *pI2cReg)
{
    pI2cReg->Ctrl = (pI2cReg->Ctrl) | ((UINT32)0x1U << 0x5U);
}

static inline void AmbaCSL_I2cWriteData(AMBA_I2C_REG_s *pI2cReg, UINT32 Val)
{
    pI2cReg->Data.Value = (UINT8)Val;
}

static inline void AmbaCSL_I2cSetPrescalerLowByte(AMBA_I2C_REG_s *pI2cReg, UINT32 Val)
{
    pI2cReg->PrescalerLowByte.Value = (UINT8)Val;
}
static inline void AmbaCSL_I2cSetPrescalerHighByte(AMBA_I2C_REG_s *pI2cReg, UINT32 Val)
{
    pI2cReg->PrescalerHighByte.Value = (UINT8)Val;
}

static inline void AmbaCSL_I2cFifoModeStart(AMBA_I2C_REG_s *pI2cReg)
{
    pI2cReg->FifoModeCtrl = (pI2cReg->FifoModeCtrl) | ((UINT32)1U << 2U);
}
static inline void AmbaCSL_I2cFifoModeStop(AMBA_I2C_REG_s *pI2cReg)
{
    pI2cReg->FifoModeCtrl = (pI2cReg->FifoModeCtrl) | ((UINT32)1U << 3U);
}
static inline void AmbaCSL_I2cFifoModeIrqEnable(AMBA_I2C_REG_s *pI2cReg)
{
    pI2cReg->FifoModeCtrl = (pI2cReg->FifoModeCtrl) | (UINT32)1U;
}
static inline void AmbaCSL_I2cFifoModeIrqDisable(AMBA_I2C_REG_s *pI2cReg)
{
    pI2cReg->FifoModeCtrl = (UINT32)((pI2cReg->FifoModeCtrl) & (~1U));
}
static inline void AmbaCSL_I2cFifoModeIrqAndStop(AMBA_I2C_REG_s *pI2cReg)
{
    pI2cReg->FifoModeCtrl = 0x0aU;
}
static inline void AmbaCSL_I2cFifoModeDisable(AMBA_I2C_REG_s *pI2cReg)
{
    pI2cReg->FifoModeCtrl = 0x00U;
}

static inline void AmbaCSL_I2cWriteFifoData(AMBA_I2C_REG_s *pI2cReg, UINT32 Val)
{
    pI2cReg->FifoModeData.Value = (UINT8)Val;
}

static inline void AmbaCSL_I2cSetTurboPrescalerByte(AMBA_I2C_REG_s *pI2cReg, UINT32 Val)
{
    pI2cReg->PrescalerTurboMode.Value = (UINT8)Val;
}

static inline void AmbaCSL_I2cEnableSdaCurrentSrc(AMBA_I2C_REG_s *pI2cReg)
{
    pI2cReg->DutyCycle.SdaCurrSrcEnable = 1U;
}
static inline void AmbaCSL_I2cDisableSdaCurrentSrc(AMBA_I2C_REG_s *pI2cReg)
{
    pI2cReg->DutyCycle.SdaCurrSrcEnable = 0U;
}
static inline void AmbaCSL_I2cEnableSclCurrentSrc(AMBA_I2C_REG_s *pI2cReg)
{
    pI2cReg->DutyCycle.SclCurrSrcEnable = 1U;
}
static inline void AmbaCSL_I2cDisableSclCurrentSrc(AMBA_I2C_REG_s *pI2cReg)
{
    pI2cReg->DutyCycle.SclCurrSrcEnable = 0U;
}
static inline void AmbaCSL_I2cSetDutyCycle(AMBA_I2C_REG_s *pI2cReg, UINT32 Val)
{
    pI2cReg->DutyCycle.DutyCycle = (UINT8)(Val & 0x03U);
}
static inline void AmbaCSL_I2cSetDutyCycleZero(AMBA_I2C_REG_s *pI2cReg)
{
    pI2cReg->DutyCycle.DutyCycle = 0U;
}

static inline UINT32 AmbaCSL_I2cGetIrqStatus(const AMBA_I2C_REG_s *pI2cReg)
{
    return (pI2cReg->Ctrl >> 0x1U) & 0x1U;
}
static inline UINT32 AmbaCSL_I2cGetTurboPrescalerByte(const AMBA_I2C_REG_s *pI2cReg)
{
    return pI2cReg->PrescalerTurboMode.Value;
}
static inline UINT32 AmbaCSL_I2cGetSdaCurrentSrc(const AMBA_I2C_REG_s *pI2cReg)
{
    return pI2cReg->DutyCycle.SdaCurrSrcEnable;
}
static inline UINT32 AmbaCSL_I2cGetSclCurrentSrc(const AMBA_I2C_REG_s *pI2cReg)
{
    return pI2cReg->DutyCycle.SclCurrSrcEnable;
}
static inline UINT32 AmbaCSL_I2cGetDutyCycle(const AMBA_I2C_REG_s *pI2cReg)
{
    return pI2cReg->DutyCycle.DutyCycle;
}
static inline UINT32 AmbaCSL_I2cGetStatus(const AMBA_I2C_REG_s *pI2cReg)
{
    return pI2cReg->Status.Status;
}

#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS) || defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV28)
static inline void AmbaCSL_I2cEnableDefinedSclMode(AMBA_I2C_REG_s *pI2cReg)
{
    pI2cReg->DutyCycle.DefinedMode = 1U;
}
static inline void AmbaCSL_I2cDisableDefinedSclMode(AMBA_I2C_REG_s *pI2cReg)
{
    pI2cReg->DutyCycle.DefinedMode = 0U;
}
static inline void AmbaCSL_I2cSetStretchScl(AMBA_I2C_REG_s *pI2cReg, UINT32 Val)
{
    pI2cReg->StretchCtrl = (Val) & 0x0FU;
}
static inline UINT32 AmbaCSL_I2cGetStretchScl(const AMBA_I2C_REG_s *pI2cReg)
{
    return (pI2cReg->StretchCtrl & 0x0FU);
}
static inline void AmbaCSL_I2cSetPhaseNum(AMBA_I2C_REG_s *pI2cReg, UINT32 Val)
{
    pI2cReg->DefinedSclNum.PhaseNum = (UINT8)(Val & 0x0FU);
}
static inline void AmbaCSL_I2cSetPhaseNumHs(AMBA_I2C_REG_s *pI2cReg, UINT32 Val)
{
    pI2cReg->DefinedSclNum.PhaseNumHs = (UINT8)(Val & 0x0FU);
}
static inline void AmbaCSL_I2cSetRisingPhase(AMBA_I2C_REG_s *pI2cReg, UINT32 Val)
{
    pI2cReg->DefinedScl.RisingPhase = (UINT8)(Val & 0x0FU);
}
static inline void AmbaCSL_I2cSetFallingPhase(AMBA_I2C_REG_s *pI2cReg, UINT32 Val)
{
    pI2cReg->DefinedScl.FallingPhase = (UINT8)(Val & 0x0FU);
}
static inline void AmbaCSL_I2cSetRisingPhaseHs(AMBA_I2C_REG_s *pI2cReg, UINT32 Val)
{
    pI2cReg->DefinedSclHs.RisingPhase = (UINT8)(Val & 0x0FU);
}
static inline void AmbaCSL_I2cSetFallingPhaseHs(AMBA_I2C_REG_s *pI2cReg, UINT32 Val)
{
    pI2cReg->DefinedSclHs.FallingPhase = (UINT8)(Val & 0x0FU);
}
#else
static inline void AmbaCSL_I2cSetStretchScl(AMBA_I2C_REG_s *pI2cReg, UINT32 Val)
{
    pI2cReg->Delay.Delay = (UINT8)Val;
}
static inline UINT32 AmbaCSL_I2cGetStretchScl(const AMBA_I2C_REG_s *pI2cReg)
{
    return pI2cReg->Delay.Delay;
}
#endif

static inline void AmbaCSL_I2cSlaveReset(void)
{
    pAmbaI2C_SlaveReg->Ctrl = 0xe0U;
}
static inline void AmbaCSL_I2cSlaveEnableIrq(void)
{
    pAmbaI2C_SlaveReg->Ctrl = 0x0eU;
}
static inline void AmbaCSL_I2cSlaveClearIrq(void)
{
    pAmbaI2C_SlaveReg->Ctrl |= 0x40U;
}
static inline void AmbaCSL_I2cSlaveSendResponse(UINT32 Val)
{
    UINT32 RegVal = pAmbaI2C_SlaveReg->Ctrl;
    pAmbaI2C_SlaveReg->Ctrl = (RegVal & ~0x1U) | Val;
}

static inline void AmbaCSL_I2cSlaveEnable(void)
{
    pAmbaI2C_SlaveReg->Enable = 1U;
}
static inline void AmbaCSL_I2cSlaveDisable(void)
{
    pAmbaI2C_SlaveReg->Enable = 0U;
}

static inline void AmbaCSL_I2cSlaveSetSclStretch(UINT32 Val)
{
    pAmbaI2C_SlaveReg->SclStretchCtrl = Val;
}

static inline void AmbaCSL_I2cSlaveSetHoldTime(UINT32 Val)
{
    pAmbaI2C_SlaveReg->DataHoldTime.HoldTime = (UINT8)Val;
}

static inline void AmbaCSL_I2cSlaveSetRxFifoThr(UINT32 Val)
{
    pAmbaI2C_SlaveReg->RxFifoThreshold.FifoLevel = (UINT8)Val;
}
static inline void AmbaCSL_I2cSlaveSetTxFifoThr(UINT32 Val)
{
    pAmbaI2C_SlaveReg->TxFifoThreshold.FifoLevel = (UINT8)Val;
}
static inline UINT32 AmbaCSL_I2cSlaveGetFifoCount(void)
{
    return pAmbaI2C_SlaveReg->FifoCount.FifoLevel;
}

static inline void AmbaCSL_I2cSlaveSetAddr(UINT32 Val)
{
    pAmbaI2C_SlaveReg->SlaveAddr.SlaveAddr = (UINT8)Val;
}
static inline UINT32 AmbaCSL_I2cSlaveGetAddr(void)
{
    return pAmbaI2C_SlaveReg->SlaveAddr.SlaveAddr;
}

static inline void AmbaCSL_I2cSlaveWriteData(UINT32 Val)
{
    pAmbaI2C_SlaveReg->Data = (UINT8)Val;
}
static inline UINT32 AmbaCSL_I2cSlaveReadData(void)
{
    return pAmbaI2C_SlaveReg->Data;
}

static inline void AmbaCSL_I2cSlaveClearStopStatus(void)
{
    pAmbaI2C_SlaveReg->Status.Stop = 0U;
}
static inline void AmbaCSL_I2cSlaveClearReStartStatus(void)
{
    pAmbaI2C_SlaveReg->Status.StartRepeat = 0U;
}
static inline UINT32 AmbaCSL_I2cSlaveIsGeneralCall(void)
{
    return pAmbaI2C_SlaveReg->Status.GeneralCall;
}
static inline UINT32 AmbaCSL_I2cSlaveGetOpMode(void)
{
    return pAmbaI2C_SlaveReg->Status.RxTxState;
}
static inline UINT32 AmbaCSL_I2cSlaveGetStopStatus(void)
{
    return pAmbaI2C_SlaveReg->Status.Stop;
}
static inline UINT32 AmbaCSL_I2cSlaveGetReStartStatus(void)
{
    return pAmbaI2C_SlaveReg->Status.StartRepeat;
}

/*
 * Defined in AmbaCSL_I2C.c
 */
UINT32 AmbaCSL_I2cGetPrescaler(const AMBA_I2C_REG_s *pI2cReg);
void AmbaCSL_I2cSetPrescaler(AMBA_I2C_REG_s *pI2cReg, UINT32 Value);

#endif /* AMBA_CSL_I2C_H */
