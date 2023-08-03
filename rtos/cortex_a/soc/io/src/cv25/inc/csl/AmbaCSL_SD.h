/**
 *  @file AmbaCSL_SD.h
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
 *  @details Definitions & Constants for CSL APIs of SD Controller
 *
 */

#ifndef AMBA_CSL_SD_H
#define AMBA_CSL_SD_H

#include "AmbaReg_RCT.h"
#include "AmbaReg_SD.h"

#ifndef AMBA_SD_DEF_H
#include "AmbaSD_Def.h"
#endif

void AmbaCSL_RctSetSd0CtrlPhy0(UINT32 Value);
void AmbaCSL_RctSetSd2CtrlPhy0(UINT32 Value);
void AmbaCSL_RctSetSd0CtrlDinClkPolarity(UINT8 Value);
void AmbaCSL_RctSetSd0CtrlClkOutBypass(UINT8 Value);
void AmbaCSL_RctSetSd0CtrlReset(UINT8 Value);
void AmbaCSL_RctSetSd0CtrlClk270Alone(UINT8 Value);
void AmbaCSL_RctSetSd0CtrlDelayChainSelect(UINT8 Value);
void AmbaCSL_RctSetSd0CtrlSdDutySelect(UINT8 Value);
void AmbaCSL_RctSetSd0CtrlRxClkPolarity(UINT8 Value);
void AmbaCSL_RctSetSd0CtrlDataCmdBypass(UINT8 Value);
void AmbaCSL_RctSetSd0CtrlDllBypass(UINT8 Value);
void AmbaCSL_RctSetSd0CtrlEnableDll(UINT8 Value);
void AmbaCSL_RctSetSd0CtrlCoarseDelayStep(UINT8 Value);
void AmbaCSL_RctSetSd0CtrlLockRange(UINT8 Value);
void AmbaCSL_RctSetSd0CtrlSelectInterClock(UINT8 Value);
void AmbaCSL_RctSetSd0CtrlPDbb(UINT8 Value);
void AmbaCSL_RctSetSd0CtrlBypassFilter(UINT8 Value);
void AmbaCSL_RctSetSd0CtrlSelectFSM(UINT8 Value);
void AmbaCSL_RctSetSd0CtrlForceLockVshift(UINT8 Value);
void AmbaCSL_RctSetSd0CtrlForceLockCycle(UINT8 Value);
void AmbaCSL_RctSetSd0CtrlPowerDownShift(UINT8 Value);
void AmbaCSL_RctSetSd0CtrlEnableAutoCoarse(UINT8 Value);
void AmbaCSL_RctSetSd0CtrlSelect(UINT32 Value);
void AmbaCSL_RctSetSd2CtrlSelect(UINT32 Value);
void AmbaCSL_RctSetSd0CtrlSelect0(UINT8 Value);
void AmbaCSL_RctSetSd0CtrlSelect1(UINT8 Value);
void AmbaCSL_RctSetSd0CtrlSelect2(UINT8 Value);
void AmbaCSL_RctSetSd0CtrlSelect3(UINT8 Value);
UINT8 AmbaCSL_RctGetSd0CtrlDinClkPolarity(void);
UINT8 AmbaCSL_RctGetSd0CtrlClkoutBypass(void);
UINT8 AmbaCSL_RctGetSd0CtrlReset(void);
UINT8 AmbaCSL_RctGetSd0CtrlClk270Alone(void);
UINT8 AmbaCSL_RctGetSd0CtrlDelayChainSelect(void);
UINT8 AmbaCSL_RctGetSd0CtrlSdDutySelect(void);
UINT8 AmbaCSL_RctGetSd0CtrlRxClkPolarity(void);
UINT8 AmbaCSL_RctGetSd0CtrlDataCmdBypass(void);
UINT8 AmbaCSL_RctGetSd0CtrlDllBypass(void);
UINT32 AmbaCSL_RctGetSd0CtrlSelect(void);
UINT8 AmbaCSL_RctGetSd0CtrlSelect0(void);
UINT8 AmbaCSL_RctGetSd0CtrlSelect1(void);
UINT8 AmbaCSL_RctGetSd0CtrlSelect2(void);
UINT8 AmbaCSL_RctGetSd0CtrlSelect3(void);
UINT32 AmbaCSL_RctGetSd0CtrlSharedBusCtrl(void);
UINT8 AmbaCSL_RctGetSd0CtrlEnableDll(void);
UINT8 AmbaCSL_RctGetSd0CtrlCoarseDelayStep(void);
UINT8 AmbaCSL_RctGetSd0CtrlLockRange(void);
UINT8 AmbaCSL_RctGetSd0CtrlSelectInterClock(void);
UINT8 AmbaCSL_RctGetSd0CtrlPDbb(void);
UINT8 AmbaCSL_RctGetSd0CtrlBypassFilter(void);
UINT8 AmbaCSL_RctGetSd0CtrlSelectFSM(void);
UINT8 AmbaCSL_RctGetSd0CtrlForceLockVshift(void);
UINT8 AmbaCSL_RctGetSd0CtrlForceLockCycle(void);
UINT8 AmbaCSL_RctGetSd0CtrlPowerDownShift(void);
UINT8 AmbaCSL_RctGetSd0CtrlEnableAutoCoarse(void);
void AmbaCSL_RctSetSd2CtrlDinClkPolarity(UINT8 Value);
void AmbaCSL_RctSetSd2CtrlClkOutBypass(UINT8 Value);
void AmbaCSL_RctSetSd2CtrlReset(UINT8 Value);
void AmbaCSL_RctSetSd2CtrlClk270Alone(UINT8 Value);
void AmbaCSL_RctSetSd2CtrlDelayChainSelect(UINT8 Value);
void AmbaCSL_RctSetSd2CtrlSdDutySelect(UINT8 Value);
void AmbaCSL_RctSetSd2CtrlRxClkPolarity(UINT8 Value);
void AmbaCSL_RctSetSd2CtrlDataCmdBypass(UINT8 Value);
void AmbaCSL_RctSetSd2CtrlDllBypass(UINT8 Value);
void AmbaCSL_RctSetSd2CtrlSelect0(UINT8 Value);
void AmbaCSL_RctSetSd2CtrlSelect1(UINT8 Value);
void AmbaCSL_RctSetSd2CtrlSelect2(UINT8 Value);
void AmbaCSL_RctSetSd2CtrlSelect3(UINT8 Value);
void AmbaCSL_RctSetSd2CtrlEnableDll(UINT8 Value);
void AmbaCSL_RctSetSd2CtrlCoarseDelayStep(UINT8 Value);
void AmbaCSL_RctSetSd2CtrlLockRange(UINT8 Value);
void AmbaCSL_RctSetSd2CtrlSelectInterClock(UINT8 Value);
void AmbaCSL_RctSetSd2CtrlPDbb(UINT8 Value);
void AmbaCSL_RctSetSd2CtrlBypassFilter(UINT8 Value);
void AmbaCSL_RctSetSd2CtrlSelectFSM(UINT8 Value);
void AmbaCSL_RctSetSd2CtrlForceLockVshift(UINT8 Value);
void AmbaCSL_RctSetSd2CtrlForceLockCycle(UINT8 Value);
void AmbaCSL_RctSetSd2CtrlPowerDownShift(UINT8 Value);
void AmbaCSL_RctSetSd2CtrlEnableAutoCoarse(UINT8 Value);
UINT8 AmbaCSL_RctGetSd2CtrlDinClkPolarity(void);
UINT8 AmbaCSL_RctGetSd2CtrlClkoutBypass(void);
UINT8 AmbaCSL_RctGetSd2CtrlReset(void);
UINT8 AmbaCSL_RctGetSd2CtrlClk270Alone(void);
UINT8 AmbaCSL_RctGetSd2CtrlDelayChainSelect(void);
UINT8 AmbaCSL_RctGetSd2CtrlSdDutySelect(void);
UINT8 AmbaCSL_RctGetSd2CtrlRxClkPolarity(void);
UINT8 AmbaCSL_RctGetSd2CtrlDataCmdBypass(void);
UINT8 AmbaCSL_RctGetSd2CtrlDllBypass(void);
UINT32 AmbaCSL_RctGetSd2CtrlSharedBusCtrl(void);
UINT8 AmbaCSL_RctGetSd2CtrlEnableDll(void);
UINT8 AmbaCSL_RctGetSd2CtrlCoarseDelayStep(void);
UINT8 AmbaCSL_RctGetSd2CtrlLockRange(void);
UINT8 AmbaCSL_RctGetSd2CtrlSelectInterClock(void);
UINT8 AmbaCSL_RctGetSd2CtrlPDbb(void);
UINT8 AmbaCSL_RctGetSd2CtrlBypassFilter(void);
UINT8 AmbaCSL_RctGetSd2CtrlSelectFSM(void);
UINT8 AmbaCSL_RctGetSd2CtrlForceLockVshift(void);
UINT8 AmbaCSL_RctGetSd2CtrlForceLockCycle(void);
UINT8 AmbaCSL_RctGetSd2CtrlPowerDownShift(void);
UINT8 AmbaCSL_RctGetSd2CtrlEnableAutoCoarse(void);
UINT32 AmbaCSL_RctGetSd2CtrlSelect(void);
UINT8 AmbaCSL_RctGetSd2CtrlSelect0(void);
UINT8 AmbaCSL_RctGetSd2CtrlSelect1(void);
UINT8 AmbaCSL_RctGetSd2CtrlSelect2(void);
UINT8 AmbaCSL_RctGetSd2CtrlSelect3(void);

/*
 * Defined in AmbaCSL_SD.c
 */
extern AMBA_SD_REG_s *pAmbaSD_Reg[AMBA_NUM_SD_CHANNEL];

#if 0
#define AmbaCSL_SdSetSdmaSysMemAddr(pSdReg, d)      pSdReg->SdmaSysMemAddr = (d)

#define AmbaCSL_SdSetAdmaDescriptorAddr(pSdReg, d)  pSdReg->AdmaAddr = (d)

#define AmbaCSL_SdSetSdmaBufSize(pSdReg, d)         pSdReg->BlkCtrl.Bits.SdmaBufSize = (d)
#define AmbaCSL_SdSetBlkCount(pSdReg, d)            pSdReg->BlkCtrl.Bits.BlkCount = (d)

#define AmbaCSL_SdSetCmdArgument(pSdReg, d)         pSdReg->CmdArgument = (d)

#define AmbaCSL_SdDmaEnable(pSdReg)                 pSdReg->TransferCtrl.Bits.DmaEnable = 1
#define AmbaCSL_SdDmaDisable(pSdReg)                pSdReg->TransferCtrl.Bits.DmaEnable = 0

#define AmbaCSL_SdBlkCountEnable(pSdReg)            pSdReg->TransferCtrl.Bits.BlkCountEnable = 1
#define AmbaCSL_SdBlkCountDisable(pSdReg)           pSdReg->TransferCtrl.Bits.BlkCountEnable = 0

#define AmbaCSL_SdAutoCmd12Enable(pSdReg)           pSdReg->TransferCtrl.Bits.AutoCmd12Enable = 1
#define AmbaCSL_SdAutoCmd12Disable(pSdReg)          pSdReg->TransferCtrl.Bits.AutoCmd12Enable = 0

#define AmbaCSL_SdSetDataTransferDir(pSdReg, d)     pSdReg->TransferCtrl.Bits.DataTransferDir = (d)
#define AmbaCSL_SdTransferDirRead(pSdReg)           pSdReg->TransferCtrl.Bits.DataTransferDir = 1
#define AmbaCSL_SdTransferDirWrite(pSdReg)          pSdReg->TransferCtrl.Bits.DataTransferDir = 0

#define AmbaCSL_SdMultiBlkEnable(pSdReg)            pSdReg->TransferCtrl.Bits.MultiBlkEnable = 1
#define AmbaCSL_SdMultiBlkDisable(pSdReg)           pSdReg->TransferCtrl.Bits.MultiBlkEnable = 0

#define AmbaCSL_SdCmdCompleteSigEnable(pSdReg)   pSdReg->TransferCtrl.Bits.CmdCompleteSignal = 1
#define AmbaCSL_SdCmdCompleteSigDisable(pSdReg)  pSdReg->TransferCtrl.Bits.CmdCompleteSignal = 0

#define AmbaCSL_SdSetResponseType(pSdReg, d)        pSdReg->TransferCtrl.Bits.ResponseType = (d)

#define AmbaCSL_SdCmdCrcCheckEnable(pSdReg)         pSdReg->TransferCtrl.Bits.CmdCrcCheckEnable = 1
#define AmbaCSL_SdCmdCrcCheckDisable(pSdReg)        pSdReg->TransferCtrl.Bits.CmdCrcCheckEnable = 0

#define AmbaCSL_SdCmdIndexCheckEnable(pSdReg)       pSdReg->TransferCtrl.Bits.CmdIndexCheckEnable = 1
#define AmbaCSL_SdCmdIndexCheckDisable(pSdReg)      pSdReg->TransferCtrl.Bits.CmdIndexCheckEnable = 0

#define AmbaCSL_SdDataPresent(pSdReg)               pSdReg->TransferCtrl.Bits.DataPresent = 1
#define AmbaCSL_SdNoDataPresent(pSdReg)             pSdReg->TransferCtrl.Bits.DataPresent = 0

#define AmbaCSL_SdSetCmdType(pSdReg, d)             pSdReg->TransferCtrl.Bits.CmdType = (d)
#define AmbaCSL_SdSetCmdIndex(pSdReg, d)            pSdReg->TransferCtrl.Bits.CmdIndex = (d)

#define AmbaCSL_SdGetResponse(pSdReg, d)            pSdReg->Response[d]
#define AmbaCSL_SdGetSdmaSysMemAddr(pSdReg)         pSdReg->SdmaSysMemAddr

#define AmbaCSL_SdGetCardDetectionPin(pSdReg)       pSdReg->PresentState.Bits.CardDetectPin
#define AmbaCSL_SdGetWriteProtectPin(pSdReg)        pSdReg->PresentState.Bits.WriteProtectSwitchPin
#define AmbaCSL_SdGetDataLineStatus(pSdReg)         pSdReg->PresentState.Bits.DataLineSignalLow4

#define AmbaCSL_SdGetIrqStatus(pSdReg)              pSdReg->IrqStatus.Data
#define AmbaCSL_SdClearIrqStatus(pSdReg, d)         pSdReg->IrqStatus.Data = (d)

#define AmbaCSL_SdSetClkDivider(pSdReg, d)          pSdReg->MiscCtrl1.SdClkDivider = (d)
#define AmbaCSL_SdSetDataTimeOutCounter(pSdReg, d)  pSdReg->MiscCtrl1.DataTimeoutCounter = (d)

#define AmbaCSL_SdAdmaEnable(pSdReg)                pSdReg->MiscCtrl0.Bits.DmaSelect = 1
#define AmbaCSL_SDGetHiSpdEnable(pSdReg)            pSdReg->MiscCtrl0.Bits.HighSpeedMode
#define AmbaCSL_SDSetHiSpdEnable(pSdReg, d)         pSdReg->MiscCtrl0.Bits.HighSpeedMode = (d)

#define AmbaCSL_SDSetEnableDDR(pSdReg, d)           pSdReg->AutoCmd12Status.Bits.DdrMode = (d)
#define AmbaCSL_SDGetEnableDDR(pSdReg)              pSdReg->AutoCmd12Status.Bits.DdrMode

#define AmbaCSL_SdGetDataLineLow4(pSdReg)           pSdReg->PresentState.Bits.DataLineSignalLow4

#define AmbaCSL_SdVoltSwitchDatLineStat(pSdReg)  pSdReg->VoltageSwitch.Bits.DataLineStatus
#define AmbaCSL_SdVoltSwitchCmdLineStat(pSdReg)   pSdReg->VoltageSwitch.Bits.CmdLineStatus

#define AmbaCSL_SdSetTransferCtrl(pSdReg, d)        pSdReg->TransferCtrl.Data = (d)
#else
static inline void AmbaCSL_SdSetSdmaSysMemAddr(AMBA_SD_REG_s *pSdReg, UINT32 d)
{
    pSdReg->SdmaSysMemAddr = (d);
}

static inline void AmbaCSL_SdSetAdmaDescriptorAddr(AMBA_SD_REG_s *pSdReg, UINT32 d)
{
    pSdReg->AdmaAddr = (d);
}

static inline void AmbaCSL_SdSetSdmaBufSize(AMBA_SD_REG_s *pSdReg, UINT8 d)
{
    pSdReg->BlkCtrl.SdmaBufSize = (d);
}
static inline void AmbaCSL_SdSetBlkCount(AMBA_SD_REG_s *pSdReg, UINT16 d)
{
    pSdReg->BlkCtrl.BlkCount = (d);
}

static inline void AmbaCSL_SdSetCmdArgument(AMBA_SD_REG_s *pSdReg, UINT32 d)
{
    pSdReg->CmdArgument = (d);
}

static inline void AmbaCSL_SdDmaEnable(AMBA_SD_REG_s *pSdReg)
{
    pSdReg->TransferCtrl.DmaEnable = 1U;
}
static inline void AmbaCSL_SdDmaDisable(AMBA_SD_REG_s *pSdReg)
{
    pSdReg->TransferCtrl.DmaEnable = 0 ;
}

static inline void AmbaCSL_SdBlkCountEnable(AMBA_SD_REG_s *pSdReg)
{
    pSdReg->TransferCtrl.BlkCountEnable = 1U;
}
static inline void AmbaCSL_SdBlkCountDisable(AMBA_SD_REG_s *pSdReg)
{
    pSdReg->TransferCtrl.BlkCountEnable = 0 ;
}

static inline void AmbaCSL_SdAutoCmd12Enable(AMBA_SD_REG_s *pSdReg)
{
    pSdReg->TransferCtrl.AutoCmd12Enable = 1U;
}
static inline void AmbaCSL_SdAutoCmd12Disable(AMBA_SD_REG_s *pSdReg)
{
    pSdReg->TransferCtrl.AutoCmd12Enable = 0 ;
}

static inline void AmbaCSL_SdSetDataTransferDir(AMBA_SD_REG_s *pSdReg, UINT8 d)
{
    pSdReg->TransferCtrl.DataTransferDir = (d);
}
static inline void AmbaCSL_SdTransferDirRead(AMBA_SD_REG_s *pSdReg)
{
    pSdReg->TransferCtrl.DataTransferDir = 1U;
}
static inline void AmbaCSL_SdTransferDirWrite(AMBA_SD_REG_s *pSdReg)
{
    pSdReg->TransferCtrl.DataTransferDir = 0 ;
}

static inline void AmbaCSL_SdMultiBlkEnable(AMBA_SD_REG_s *pSdReg)
{
    pSdReg->TransferCtrl.MultiBlkEnable = 1U;
}
static inline void AmbaCSL_SdMultiBlkDisable(AMBA_SD_REG_s *pSdReg)
{
    pSdReg->TransferCtrl.MultiBlkEnable = 0 ;
}

static inline void AmbaCSL_SdCmdCompleteSigEnable(AMBA_SD_REG_s *pSdReg)
{
    pSdReg->TransferCtrl.CmdCompleteSignal = 1U;
}
static inline void AmbaCSL_SdCmdCompleteSigDisable(AMBA_SD_REG_s *pSdReg)
{
    pSdReg->TransferCtrl.CmdCompleteSignal = 0 ;
}

static inline void AmbaCSL_SdSetResponseType(AMBA_SD_REG_s *pSdReg, UINT8 d)
{
    pSdReg->TransferCtrl.ResponseType = (d);
}

static inline void AmbaCSL_SdCmdCrcCheckEnable(AMBA_SD_REG_s *pSdReg)
{
    pSdReg->TransferCtrl.CmdCrcCheckEnable = 1U;
}
static inline void AmbaCSL_SdCmdCrcCheckDisable(AMBA_SD_REG_s *pSdReg)
{
    pSdReg->TransferCtrl.CmdCrcCheckEnable = 0 ;
}

static inline void AmbaCSL_SdCmdIndexCheckEnable(AMBA_SD_REG_s *pSdReg)
{
    pSdReg->TransferCtrl.CmdIndexCheckEnable = 1U;
}
static inline void AmbaCSL_SdCmdIndexCheckDisable(AMBA_SD_REG_s *pSdReg)
{
    pSdReg->TransferCtrl.CmdIndexCheckEnable = 0 ;
}

static inline void AmbaCSL_SdDataPresent(AMBA_SD_REG_s *pSdReg)
{
    pSdReg->TransferCtrl.DataPresent = 1U;
}
static inline void AmbaCSL_SdNoDataPresent(AMBA_SD_REG_s *pSdReg)
{
    pSdReg->TransferCtrl.DataPresent = 0 ;
}

static inline void AmbaCSL_SdSetCmdType(AMBA_SD_REG_s *pSdReg, UINT8 d)
{
    pSdReg->TransferCtrl.CmdType = (d);
}
static inline void AmbaCSL_SdSetCmdIndex(AMBA_SD_REG_s *pSdReg, UINT8 d)
{
    pSdReg->TransferCtrl.CmdIndex = (d);
}

static inline UINT32 AmbaCSL_SdGetResponse(const AMBA_SD_REG_s *pSdReg, UINT32 d)
{
    return pSdReg->Response[d];
}
static inline UINT32 AmbaCSL_SdGetSdmaSysMemAddr(const AMBA_SD_REG_s *pSdReg)
{
    return pSdReg->SdmaSysMemAddr;
}

static inline UINT32 AmbaCSL_SdGetCardDetectionPin(const AMBA_SD_REG_s *pSdReg)
{
    return pSdReg->PresentState.Bits.CardDetectPin;
}
static inline UINT32 AmbaCSL_SdGetWriteProtectPin(const AMBA_SD_REG_s *pSdReg)
{
    return pSdReg->PresentState.Bits.WriteProtectSwitchPin;
}
static inline UINT32 AmbaCSL_SdGetDataLineStatus(const AMBA_SD_REG_s *pSdReg)
{
    return pSdReg->PresentState.Bits.DataLineSignalLow4;
}

static inline void AmbaCSL_SdSetClkDivider(AMBA_SD_REG_s *pSdReg, UINT8 d)
{
    pSdReg->MiscCtrl1.SdClkDivider = (d);
}
static inline void AmbaCSL_SdSetDataTimeOutCounter(AMBA_SD_REG_s *pSdReg, UINT8 d)
{
    pSdReg->MiscCtrl1.DataTimeoutCounter = (d);
}

static inline void AmbaCSL_SdAdmaEnable(AMBA_SD_REG_s *pSdReg)
{
    pSdReg->MiscCtrl0.DmaSelect = 1U;
}
static inline UINT8 AmbaCSL_SDGetHiSpdEnable(const AMBA_SD_REG_s *pSdReg)
{
    return pSdReg->MiscCtrl0.HighSpeedMode;
}
static inline void AmbaCSL_SDSetHiSpdEnable(AMBA_SD_REG_s *pSdReg, UINT8 d)
{
    pSdReg->MiscCtrl0.HighSpeedMode = (d);
}

static inline void AmbaCSL_SDSetEnableDDR(AMBA_SD_REG_s *pSdReg, UINT8 d)
{
    pSdReg->AutoCmd12Status.DdrMode = (d);
}
static inline UINT8 AmbaCSL_SDGetEnableDDR(const AMBA_SD_REG_s *pSdReg)
{
    return pSdReg->AutoCmd12Status.DdrMode;
}

static inline UINT32 AmbaCSL_SdGetDataLineLow4(const AMBA_SD_REG_s *pSdReg)
{
    return pSdReg->PresentState.Bits.DataLineSignalLow4;
}

static inline UINT32 AmbaCSL_SdVoltSwitchDatLineStat(const AMBA_SD_REG_s *pSdReg)
{
    return pSdReg->VoltageSwitch.Bits.DataLineStatus;
}
static inline UINT32 AmbaCSL_SdVoltSwitchCmdLineStat(const AMBA_SD_REG_s *pSdReg)
{
    return pSdReg->VoltageSwitch.Bits.CmdLineStatus;
}
#endif

/*
 * Defined in AmbaCSL_SD.c
 */
void AmbaCSL_SdSetHostBusWidth(AMBA_SD_REG_s *pSdReg, UINT32 BusWidth);
void AmbaCSL_SdSetBlkSizeBlkCount(AMBA_SD_REG_s * pSdReg, UINT32 BlkSize, UINT16 BlkCnt);
void AmbaCSL_SdSetBlkSize(AMBA_SD_REG_s *pSdReg, UINT32 Value);
void AmbaCSL_SdClockEnable(AMBA_SD_REG_s *pSdReg, UINT32 Enable);
void AmbaCSL_SdResetCmdLine(AMBA_SD_REG_s *pSdReg);
void AmbaCSL_SdResetDataLine(AMBA_SD_REG_s *pSdReg);
void AmbaCSL_SdResetAll(UINT32 ChannelNo, AMBA_SD_REG_s *pSdReg);
void AmbaCSL_SdEnableIrq(AMBA_SD_REG_s *pSdReg);
void AmbaCSL_SdioEnableCardIrq(AMBA_SD_REG_s * pSdReg, UINT32 Enable);
void AmbaCSL_SdDisableIrqStatus(AMBA_SD_REG_s *pSdReg);
UINT32 AmbaCSL_SdWaitCmdLineReady(AMBA_SD_REG_s *pSdReg);
UINT32 AmbaCSL_SdWaitDataLineReady(AMBA_SD_REG_s *pSdReg);
void  AmbaCSL_SDSetRdLatencyCtrl(UINT32 ChannelNo, UINT32 DelayCycle);
UINT32 AmbaCSL_SDGetRdLatencyCtrl(UINT32 ChannelNo);
UINT32 AmbaCSL_SdGetIrqStatus(const AMBA_SD_REG_s *pSdReg);
void AmbaCSL_SdClearIrqStatus(AMBA_SD_REG_s *pSdReg, UINT32 d);
void AmbaCSL_SdSetTransferCtrl(AMBA_SD_REG_s *pSdReg, UINT32 d);
void AmbaCSL_SdSetBlkCtrl(AMBA_SD_REG_s *pSdReg, UINT32 BlkSize, UINT16 BlkCnt);

#endif /* AMBA_CSL_SD_H */
