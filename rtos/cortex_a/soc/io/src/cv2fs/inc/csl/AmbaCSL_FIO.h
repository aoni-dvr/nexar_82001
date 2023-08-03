/**
 *  @file AmbaCSL_FIO.h
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
 *  @details Definitions of Chip Support Library for Flash IO
 *
 */

#ifndef AMBA_CSL_FIO_H
#define AMBA_CSL_FIO_H

#include "AmbaReg_RCT.h"
#ifndef AMBA_REG_FIO_H
#include "AmbaReg_FIO.h"
#endif

/*
 * Macro Definitions
 */
#if 1
static inline void AmbaCSL_RctSetFlashControllerReset(void)
{
    pAmbaRCT_Reg->FioReset = 0x9;
}
static inline void AmbaCSL_RctClearFlashControllerReset(void)
{
    pAmbaRCT_Reg->FioReset = 0x0;
}

static inline void AmbaCSL_FioSetCtrlReg(AMBA_FIO_CTRL_REG_s d)
{
    pAmbaFIO_Reg->Ctrl = (d);
}

static inline AMBA_FIO_CTRL_REG_s AmbaCSL_FioGetCtrlReg(void)
{
    return pAmbaFIO_Reg->Ctrl;
}

static inline void AmbaCSL_FioFifoRandomReadModeEnable(void)
{
    pAmbaFIO_Reg->Ctrl.RandomReadMode = 1;
}
static inline void AmbaCSL_FioFifoRandomReadModeDisable(void)
{
    pAmbaFIO_Reg->Ctrl.RandomReadMode = 0;
}

static inline UINT32 AmbaCSL_FioIsRandomReadMode(void)
{
    return pAmbaFIO_Reg->Ctrl.RandomReadMode;
}

static inline void AmbaCSL_FioStopOnErrorEnable(void)
{
    pAmbaFIO_Reg->Ctrl.StopOnError = 1;
}
static inline void AmbaCSL_FioStopOnErrorDisable(void)
{
    pAmbaFIO_Reg->Ctrl.StopOnError = 0;
}

static inline void AmbaCSL_FioSkipBlankPageEccEnable(void)
{
    pAmbaFIO_Reg->Ctrl.SkipBlankPageEcc = 1;
}
static inline void AmbaCSL_FioSkipBlankPageEccDisable(void)
{
    pAmbaFIO_Reg->Ctrl.SkipBlankPageEcc = 0;
}

static inline void AmbaCSL_FioBCH8Bit(void)
{
    pAmbaFIO_Reg->Ctrl.Bch8Bits = 1;
}
static inline void AmbaCSL_FioBCH6Bit(void)
{
    pAmbaFIO_Reg->Ctrl.Bch8Bits = 0;
}

static inline void AmbaCSL_FioBchEnable(void)
{
    pAmbaFIO_Reg->Ctrl.BchEnable = 1;
}
static inline void AmbaCSL_FioBchDisable(void)
{
    pAmbaFIO_Reg->Ctrl.BchEnable = 0;
}

/**/
static inline void   AmbaCSL_FioSelectSpiNand(void)
{
    pAmbaFIO_Reg->Ctrl2.SelectSpiNand = 1;
}
static inline void   AmbaCSL_FioSelectParallelNand(void)
{
    pAmbaFIO_Reg->Ctrl2.SelectSpiNand = 0;
}
static inline UINT32 AmbaCSL_FioGetNandSelect(void)
{
    return pAmbaFIO_Reg->Ctrl2.SelectSpiNand;
}

/**/
static inline UINT32 AmbaCSL_FioDoneStatus(void)
{
    return pAmbaFIO_Reg->Status.FioDone;
}

/**/
static inline AMBA_FIO_ECC_STATUS_REG_s AmbaCSL_FioGetEccStatus(void)
{
    return pAmbaFIO_Reg->EccStatus;
}
static inline AMBA_FIO_ECC_STATUS2_REG_s AmbaCSL_FioGetEccStatus2(void)
{
    return pAmbaFIO_Reg->EccStatus2;
}

static inline UINT32 AmbaCSL_FioGetEccErrBlkNo(void)
{
    return pAmbaFIO_Reg->EccStatus.EccCorrectedBlockNo;
}
static inline UINT32 AmbaCSL_FioGetEccErrNotCorrectStatus(void)
{
    return pAmbaFIO_Reg->EccStatus.ErrorNotCorrectable;
}
static inline UINT32 AmbaCSL_FioGetEccErrStatus(void)
{
    return pAmbaFIO_Reg->EccStatus.ErrorDetected;
}

/**/
static inline AMBA_FDMA_MAIN_STATUS_REG_s AmbaCSL_FioFdmaGetMainStatus(void)
{
    return pAmbaFIO_Reg->FdmaMainStatus;
}

static inline AMBA_FDMA_DSM_CTRL_REG_s AmbaCSL_FioGetFdmaDsmCtrlReg(void)
{
    return pAmbaFIO_Reg->Chan0DsmCtrl;
}

static inline void AmbaCSL_FioSetFdmaDsmCtrlReg(AMBA_FDMA_DSM_CTRL_REG_s d)
{
    pAmbaFIO_Reg->Chan0DsmCtrl = (d);
}
static inline void AmbaCSL_FdmaSetDescAddr(UINT32 d)
{
    pAmbaFIO_Reg->FdmaDescAddr = (d);
}
static inline void AmbaCSL_FdmaSetSpareMemAddr(UINT32 d)
{
    pAmbaFIO_Reg->SpareMemAddr = (d);
}
static inline void AmbaCSL_FdmaSetMainByteCount(UINT32 d)
{
    pAmbaFIO_Reg->FdmaCtrl.ByteCount = (d);
}
static inline void AmbaCSL_FdmaSetMainDestAddr(UINT32 d)
{
    pAmbaFIO_Reg->DestAddr = (d);
}

static inline void AmbaCSL_NandSetCmdReg (AMBA_NAND_CMD_REG_s Val)
{
    pAmbaFIO_Reg->Cmd = Val;
}
static inline void AmbaCSL_NandSetCmdCode(UINT8 d)
{
    pAmbaFIO_Reg->Cmd.CmdCode = (d);
}
static inline void AmbaCSL_NandSetAddress31_4(UINT32 d)
{
    pAmbaFIO_Reg->Cmd.Addr = (d);
}
static inline void AmbaCSL_NandSetAddress33_32(UINT8 d)
{
    pAmbaFIO_Reg->NandCtrl.Addr33_32 = (d);
}

static inline void AmbaCSL_NandSetCopyDestAddr(UINT32 d)
{
    pAmbaFIO_Reg->CopyDestAddr = (d);
}
static inline void AmbaCSL_NandSetCopyDestAddrHigh(UINT8 d)
{
    pAmbaFIO_Reg->CopyAddrHigh.CopyAddrHigh = (d);
}
static inline void AmbaCSL_NandSetCopyAddr5Byte(UINT8 d)
{
    pAmbaFIO_Reg->CopyAddrHigh.CopyAddrHigh = (d);
}

static inline void AmbaCSL_NandSetTiming0(AMBA_NAND_TIMING0_REG_s d)
{
    pAmbaFIO_Reg->Timing0 = (d);
}
static inline void AmbaCSL_NandSetTiming1(AMBA_NAND_TIMING1_REG_s d)
{
    pAmbaFIO_Reg->Timing1 = (d);
}
static inline void AmbaCSL_NandSetTiming2(AMBA_NAND_TIMING2_REG_s d)
{
    pAmbaFIO_Reg->Timing2 = (d);
}
static inline void AmbaCSL_NandSetTiming3(AMBA_NAND_TIMING3_REG_s d)
{
    pAmbaFIO_Reg->Timing3 = (d);
}
static inline void AmbaCSL_NandSetTiming4(AMBA_NAND_TIMING4_REG_s d)
{
    pAmbaFIO_Reg->Timing4 = (d);
}
static inline void AmbaCSL_NandSetTiming5(AMBA_NAND_TIMING5_REG_s d)
{
    pAmbaFIO_Reg->Timing5 = (d);
}
static inline void AmbaCSL_NandSetTiming6(AMBA_NAND_TIMING6_REG_s d)
{
    pAmbaFIO_Reg->Timing6 = (d);
}

static inline void AmbaCSL_NandSetCustomCmdReg(AMBA_NAND_CUSTOM_CMD_REG_s d)
{
    pAmbaFIO_Reg->CustomCmd = (d);
}

static inline void AmbaCSL_NandSetCmd1Word0(UINT8 d)
{
    pAmbaFIO_Reg->CusCmdWord.Cmd1Val0 = (d);
}
static inline void AmbaCSL_NandSetCmd1Word1(UINT8 d)
{
    pAmbaFIO_Reg->CusCmdWord.Cmd1Val1 = (d);
}

static inline void AmbaCSL_NandSetCmd2Word0(UINT8 d)
{
    pAmbaFIO_Reg->CusCmdWord.Cmd2Val0 = (d);
}
static inline void AmbaCSL_NandSetCmd2Word1(UINT8 d)
{
    pAmbaFIO_Reg->CusCmdWord.Cmd2Val1 = (d);
}

static inline void AmbaCSL_FdmaSetMainCtrlReg(AMBA_FDMA_CTRL_REG_s d)
{
    pAmbaFIO_Reg->FdmaCtrl = (d);
}
#else
/*
 * Macro Definitions
 */
#define AmbaCSL_RctSetFlashControllerReset()    pAmbaRCT_Reg->FioReset.Data = 0xd
#define AmbaCSL_RctClearFlashControllerReset()  pAmbaRCT_Reg->FioReset.Data = 0x0

#define AmbaCSL_FioSetCtrlReg(d)                pAmbaFIO_Reg->Ctrl = (d)

#define AmbaCSL_FioGetCtrlReg()                 pAmbaFIO_Reg->Ctrl

#define AmbaCSL_FioFifoRandomReadModeEnable()   pAmbaFIO_Reg->Ctrl.RandomReadMode = 1
#define AmbaCSL_FioFifoRandomReadModeDisable()  pAmbaFIO_Reg->Ctrl.RandomReadMode = 0
#define AmbaCSL_FioIsRandomReadMode()           pAmbaFIO_Reg->Ctrl.RandomReadMode

#define AmbaCSL_FioStopOnErrorEnable()          pAmbaFIO_Reg->Ctrl.StopOnError = 1
#define AmbaCSL_FioStopOnErrorDisable()         pAmbaFIO_Reg->Ctrl.StopOnError = 0

#define AmbaCSL_FioSkipBlankPageEccEnable()     pAmbaFIO_Reg->Ctrl.SkipBlankPageEcc = 1
#define AmbaCSL_FioSkipBlankPageEccDisable()    pAmbaFIO_Reg->Ctrl.SkipBlankPageEcc = 0

#define AmbaCSL_FioBCH8Bit()                    pAmbaFIO_Reg->Ctrl.Bch8Bits = 1
#define AmbaCSL_FioBCH6Bit()                    pAmbaFIO_Reg->Ctrl.Bch8Bits = 0

#define AmbaCSL_FioBchEnable()                  pAmbaFIO_Reg->Ctrl.BchEnable = 1
#define AmbaCSL_FioBchDisable()                 pAmbaFIO_Reg->Ctrl.BchEnable = 0

#define AmbaCSL_FioDmaReqStatus()               pAmbaFIO_Reg->Ctrl.DmaReq
#define AmbaCSL_FioDmaAckkStatus()              pAmbaFIO_Reg->Ctrl.DmaAck

/**/
#define AmbaCSL_FioSelectSpiNand()              pAmbaFIO_Reg->Ctrl2.Bits.SelectSpiNand = 1
#define AmbaCSL_FioSelectParallelNand()         pAmbaFIO_Reg->Ctrl2.Bits.SelectSpiNand = 0
#define AmbaCSL_FioGetNandSelect()              pAmbaFIO_Reg->Ctrl2.Bits.SelectSpiNand
/**/
#define AmbaCSL_FioDoneStatus()                 pAmbaFIO_Reg->Status.Bits.FioDone

/**/
#define AmbaCSL_FioDmaEnable()                  pAmbaFIO_Reg->FioDmaCtrl.Bits.DmaEnable = 1

#define AmbaCSL_FioDmaSetByteCount(d)           pAmbaFIO_Reg->FioDmaCtrl.Bits.ByteCount = (d)
#define AmbaCSL_FioDmaSetBusDataSize(d)         pAmbaFIO_Reg->FioDmaCtrl.Bits.BusDataSize = (d)
#define AmbaCSL_FioDmaSetBusBlockSize(d)        pAmbaFIO_Reg->FioDmaCtrl.Bits.BusBlockSize = (d)
#define AmbaCSL_FioDmaSetMemTargetNAND()        pAmbaFIO_Reg->FioDmaCtrl.Bits.MemTarget = AMBA_FIO_DMA_MEM_TARGET_NAND
#define AmbaCSL_FioDmaSetMemTargetSD()          pAmbaFIO_Reg->FioDmaCtrl.Bits.MemTarget = AMBA_FIO_DMA_MEM_TARGET_SD

#define AmbaCSL_FioDmaFIFO2NAND()               pAmbaFIO_Reg->FioDmaCtrl.Bits.ReadMem = 1
#define AmbaCSL_FioDmaNAND2FIFO()               pAmbaFIO_Reg->FioDmaCtrl.Bits.ReadMem = 0

#define AmbaCSL_FioDmaSetMemStartAddr(d)        pAmbaFIO_Reg->FioDmaAddr = (d)

/**/
#define AmbaCSL_FioDmaGetStatus()               pAmbaFIO_Reg->FioDmaStatus.Data
#define AmbaCSL_FioDmaClearStatus()             pAmbaFIO_Reg->FioDmaStatus.Data = 0

#define AmbaCSL_FioDmaReadError()               pAmbaFIO_Reg->FioDmaStatus.Bits.ReadError
#define AmbaCSL_FioDmaAddrError()               pAmbaFIO_Reg->FioDmaStatus.Bits.AddrError
#define AmbaCSL_FioDmaDone()                    pAmbaFIO_Reg->FioDmaStatus.Bits.DmaDone
#define AmbaCSL_FioDmaTransferredByteCount()    pAmbaFIO_Reg->FioDmaStatus.Bits.ByteCount

/**/
#define AmbaCSL_FioSetDsmCtrlReg(d)             pAmbaFIO_Reg->DsmCtrl.Data = (d)
#define AmbaCSL_FioGetDsmCtrlReg()              pAmbaFIO_Reg->DsmCtrl.Data

#define AmbaCSL_FioDsmEnable()                  pAmbaFIO_Reg->DsmCtrl.Bits.DsmEnable = 1
#define AmbaCSL_FioDsmDisable()                 pAmbaFIO_Reg->DsmCtrl.Bits.DsmEnable = 0
#define AmbaCSL_FioIsDualSpaceMode()            pAmbaFIO_Reg->DsmCtrl.Bits.DsmEnable

#define AmbaCSL_FioDsmSetMainStride(d)          pAmbaFIO_Reg->DsmCtrl.Bits.MainStrideSize = (d)
#define AmbaCSL_FioDsmSetSpareStride(d)         pAmbaFIO_Reg->DsmCtrl.Bits.SpareStrideSize = (d)
/**/

/**/
#define AmbaCSL_FioGetEccStatus()               pAmbaFIO_Reg->EccStatus.Data
#define AmbaCSL_FioGetEccStatus2()              pAmbaFIO_Reg->EccStatus2.Data

#define AmbaCSL_FioGetEccErrBlkNo()             pAmbaFIO_Reg->EccStatus.ErrorBlkNo
#define AmbaCSL_FioGetEccErrNotCorrectStatus()  pAmbaFIO_Reg->EccStatus.Bits.ErrorNotCorrectable
#define AmbaCSL_FioGetEccErrStatus()            pAmbaFIO_Reg->EccStatus.Bits.ErrorDetected

#define AmbaCSL_FioFdmaGetMainStatus()          pAmbaFIO_Reg->FdmaMainStatus.Data
#define AmbaCSL_FioFdmaClearMainStatus()        pAmbaFIO_Reg->FdmaMainStatus.Data = 0

#define AmbaCSL_FioGetFdmaDsmCtrlReg()          pAmbaFIO_Reg->Chan0DsmCtrl


#define AmbaCSL_FioSetFdmaDsmCtrlReg(d)         pAmbaFIO_Reg->Chan0DsmCtrl = (d)

#define AmbaCSL_FdmaSetDescAddr(d)              pAmbaFIO_Reg->FdmaDescAddr = (d)

#define AmbaCSL_FdmaSetSpareMemAddr(d)          pAmbaFIO_Reg->SpareMemAddr = (d)

#define AmbaCSL_FdmaSetMainByteCount(d)         pAmbaFIO_Reg->FdmaCtrl.ByteCount = (d)

#define AmbaCSL_FdmaSetMainDestAddr(d)          pAmbaFIO_Reg->DestAddr = (d)


#define AmbaCSL_NandSetCmdReg(d)                pAmbaFIO_Reg->Cmd = (d)
#define AmbaCSL_NandSetCmdCode(d)               pAmbaFIO_Reg->Cmd.CmdCode = (d)
#define AmbaCSL_NandSetAddress31_4(d)           pAmbaFIO_Reg->Cmd.Addr = (d)
#define AmbaCSL_NandSetAddress33_32(d)          pAmbaFIO_Reg->NandCtrl.Addr33_32 = (d)
#define AmbaCSL_NandSetCopyDestAddr(d)      pAmbaFIO_Reg->CopyDestAddr = (d)
#define AmbaCSL_NandSetCopyDestAddrHigh(d)  pAmbaFIO_Reg->CopyAddrHigh.Bits.CopyAddrHigh = (d)
#define AmbaCSL_NandSetCopyAddr5Byte(d)         pAmbaFIO_Reg->CopyAddrHigh.Bits.CopyAddrHigh = (d)

#define AmbaCSL_NandSetTiming0(d)               pAmbaFIO_Reg->Timing0 = (d)
#define AmbaCSL_NandSetTiming1(d)               pAmbaFIO_Reg->Timing1 = (d)
#define AmbaCSL_NandSetTiming2(d)               pAmbaFIO_Reg->Timing2 = (d)
#define AmbaCSL_NandSetTiming3(d)               pAmbaFIO_Reg->Timing3 = (d)
#define AmbaCSL_NandSetTiming4(d)               pAmbaFIO_Reg->Timing4 = (d)
#define AmbaCSL_NandSetTiming5(d)               pAmbaFIO_Reg->Timing5 = (d)
#define AmbaCSL_NandSetTiming6(d)               pAmbaFIO_Reg->Timing6 = (d)

#define AmbaCSL_NandSetCustomCmdReg(d)          pAmbaFIO_Reg->CustomCmd.Data = (d)

#define AmbaCSL_NandSetCmd1Word0(d)             pAmbaFIO_Reg->CusCmdWord.Bits.Cmd1Val0 = (d)
#define AmbaCSL_NandSetCmd1Word1(d)             pAmbaFIO_Reg->CusCmdWord.Bits.Cmd1Val1 = (d)

#define AmbaCSL_NandSetCmd2Word0(d)             pAmbaFIO_Reg->CusCmdWord.Bits.Cmd2Val0 = (d)
#define AmbaCSL_NandSetCmd2Word1(d)             pAmbaFIO_Reg->CusCmdWord.Bits.Cmd2Val1 = (d)
#endif
void AmbaCSL_FioFdmaClearMainStatus(void);

void AmbaCSL_FioInterruptEnable(void);

void AmbaCSL_FioClearIrqStatus(void);

void AmbaCSL_FioDmaFifoModeEnable(void);

void AmbaCSL_FioSetEccReportConfig(UINT32 BchBits);

#endif /* AMBA_CSL_FIO_H */
