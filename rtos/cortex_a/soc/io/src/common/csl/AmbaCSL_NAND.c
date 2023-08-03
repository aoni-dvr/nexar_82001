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

#include "AmbaCSL_FIO.h"
#include "AmbaCSL_NAND.h"
#include "AmbaCSL_DMA.h"

/*
 *  @RoutineName:: AmbaCSL_NandSendResetCmd
 *
 *  @Description:: Send Reset command to NAND device
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
 */
void AmbaCSL_NandSendResetCmd(void)
{
    AmbaCSL_FioClearIrqStatus();   /* clear NAND interrupt status */
    AmbaCSL_NandSetCmdCode(AMBA_NAND_CMD_RESET);
}

/*
 *  @RoutineName:: AmbaCSL_NandSendReadIdCmd
 *
 *  @Description:: Retrieve IDs from NAND flash device
 *
 *  @Input      ::
 *      NumIdCycle: Number of ID read cycles
 *
 *  @Output     :: none
 *
 *  @Return     :: none
 */
void AmbaCSL_NandSendReadIdCmd(UINT32 NumIdCycle)
{
    switch (NumIdCycle) {
    case 2:
        pAmbaFIO_Reg->ExtCtrl.Id5Bytes = 0U;
        pAmbaFIO_Reg->NandCtrl.ReadId4Cycle = 0U;
        break;

    case 4:
        pAmbaFIO_Reg->ExtCtrl.Id5Bytes = 0U;
        pAmbaFIO_Reg->NandCtrl.ReadId4Cycle = 1U;
        break;

    case 5:
    default:
        pAmbaFIO_Reg->ExtCtrl.Id5Bytes  = 1U;
        pAmbaFIO_Reg->NandCtrl.ReadId4Cycle = 0U;
        break;
    }

    AmbaCSL_NandSetAddress33_32(0);
    AmbaCSL_NandSetAddress31_4(0);  /* Bank address */
    AmbaCSL_NandSetCmdCode(AMBA_NAND_CMD_READ_ID);
}

/*
 *  @RoutineName:: AmbaCSL_NandGetReadIdResponse
 *
 *  @Description:: Retrieve IDs from NAND flash device
 *
 *  @Input      ::
 *      NumIdCycle: Number of ID read cycles
 *      pDeviceID:  ID byte sequence
 *
 *  @Output     :: none
 *
 *  @Return     :: none
 */
void AmbaCSL_NandGetReadIdResponse(UINT32 NumIdCycle, UINT8 *pDeviceID)
{
    pDeviceID[0] = (UINT8) pAmbaFIO_Reg->ID.MakerID;
    pDeviceID[1] = (UINT8) pAmbaFIO_Reg->ID.DeviceID;

    switch (NumIdCycle) {
    case 4U:
        pDeviceID[2] = (UINT8) pAmbaFIO_Reg->ID.IdByte3;
        pDeviceID[3] = (UINT8) pAmbaFIO_Reg->ID.IdByte4;
        break;

    case 5U:
        pDeviceID[2] = (UINT8) pAmbaFIO_Reg->ID.IdByte3;
        pDeviceID[3] = (UINT8) pAmbaFIO_Reg->ID.IdByte4;
        pDeviceID[4] = (UINT8) pAmbaFIO_Reg->ExtID.IdByte5;
        break;

    default:
        /* NAND device ID should be 4 or 5 clcles. */
        break;
    }
}

/*
 *  @RoutineName:: AmbaCSL_NandSendReadStatusCmd
 *
 *  @Description:: Read NAND flash status
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
 */
void AmbaCSL_NandSendReadStatusCmd(void)
{
    AmbaCSL_FioClearIrqStatus();   /* clear NAND interrupt status */

    AmbaCSL_NandSetCmdCode(AMBA_NAND_CMD_READ_STATUS);
}

/*
 *  @RoutineName:: AmbaCSL_NandGetReadStatusResponse
 *
 *  @Description:: Retrieve status from NAND flash device
 *
 *  @Input      ::
 *      pStatus: Status byte sequence
 *
 *  @Output     :: none
 *
 *  @Return     :: none
 */
void AmbaCSL_NandGetCmdResponse(UINT8 *pStatus)
{
    UINT32 Status = pAmbaFIO_Reg->NandStatus.Status;
    if (OK != AmbaWrap_memcpy(pStatus, &Status, sizeof(*pStatus))) {
        /* Do nothing */
    }
}

#if 0
/*
 *  @RoutineName:: AmbaCSL_NandSendCopyBackCmd
 *
 *  @Description:: Copyback NAND page
 *
 *  @Input      ::
 *      DestAddr: Destination page address
 *      SrcAddr: Source page address
 *
 *  @Output     :: none
 *
 *  @Return     :: none
 */
void AmbaCSL_NandSendCopyBackCmd(UINT64 SrcAddr, UINT64 DestAddr)
{
    UINT64 SrcShiftTmp;
    UINT32 SrcShift;
    AMBA_NAND_CUSTOM_CMD_REG_s CustomCmdRegVal = {0U};

    AmbaCSL_FioClearIrqStatus();   /* clear NAND Interrupt status */

    SrcShiftTmp = SrcAddr >> 32U;
    AmbaMisra_TypeCast32(&SrcShift, &SrcShiftTmp);
    AmbaCSL_NandSetAddress33_32(SrcShift);

    SrcShiftTmp = SrcAddr >> 4U;
    AmbaMisra_TypeCast32(&SrcShift, &SrcShiftTmp);
    AmbaCSL_NandSetAddress31_4(SrcShift);

    AmbaCSL_NandSetCopyDestAddrHigh(DestAddr >> 32U);
    AmbaCSL_NandSetCopyDestAddr((UINT32)DestAddr);

    AmbaCSL_NandSetCmd1Word0(0x00U);
    AmbaCSL_NandSetCmd1Word1(0x85U);

    AmbaCSL_NandSetCmd2Word0(0x35U);
    AmbaCSL_NandSetCmd2Word1(0x10U);

    CustomCmdRegVal.DataCycle           = 1U;
    CustomCmdRegVal.WaitCycle           = AMBA_NAND_CMD_WAIT_RB;
    CustomCmdRegVal.ReadWriteType       = AMBA_NAND_CMD_NO_DATA;
    CustomCmdRegVal.CmdPhase2Count      = AMBA_NAND_CMD_PHASE2_PRESENT;
    CustomCmdRegVal.AdressCycle         = 5U;
    CustomCmdRegVal.CmdPhase1Count      = AMBA_NAND_CMD_PHASE1_PRESENT_CMD1;
    CustomCmdRegVal.AddrSrc             = 3U;
    CustomCmdRegVal.Addr1NoIncrease     = 0U;
    CustomCmdRegVal.Addr2NoIncrease     = 0U;
    CustomCmdRegVal.DataSrcSelect       = AMBA_NAND_DATA_FROM_REGISTER;
    CustomCmdRegVal.ChipEnableTerminate = 1U;

    AmbaCSL_NandSetCustomCmdReg(CustomCmdRegVal);
}
#endif

/*
 *  @RoutineName:: AmbaCSL_NandSendBlockEraseCmd
 *
 *  @Description:: Erase NAND block
 *
 *  @Input      ::
 *      Addr: Block address
 *
 *  @Output     :: none
 *
 *  @Return     :: none
 */
void AmbaCSL_NandSendBlockEraseCmd(UINT32 Addr)
{
    AMBA_NAND_CUSTOM_CMD_REG_s CustomCmdRegVal = {0};

    AmbaCSL_FioClearIrqStatus();   /* clear NAND Interrupt status */

    AmbaCSL_NandSetAddress33_32(0);
    AmbaCSL_NandSetAddress31_4(Addr >> 4U);

    AmbaCSL_NandSetCmd1Word0(0x60U);
    AmbaCSL_NandSetCmd2Word0(0xD0U);

    CustomCmdRegVal.DataCycle           = 5U;
    CustomCmdRegVal.WaitCycle           = AMBA_NAND_CMD_WAIT_RB;
    CustomCmdRegVal.ReadWriteType       = AMBA_NAND_CMD_NO_DATA;
    CustomCmdRegVal.CmdPhase2Count      = AMBA_NAND_CMD_PHASE2_PRESENT;
    CustomCmdRegVal.AdressCycle         = 3U;
    CustomCmdRegVal.CmdPhase1Count      = AMBA_NAND_CMD_PHASE1_PRESENT_CMD1;
    CustomCmdRegVal.AddrSrc             = 1U;
    CustomCmdRegVal.Addr1NoIncrease     = 0U;
    CustomCmdRegVal.Addr2NoIncrease     = 0U;
    CustomCmdRegVal.DataSrcSelect       = AMBA_NAND_DATA_FROM_REGISTER;
    CustomCmdRegVal.ChipEnableTerminate = 1U;

    AmbaCSL_NandSetCustomCmdReg(CustomCmdRegVal);
}

/*
 *  @RoutineName:: AmbaCSL_NandSetTiming
 *
 *  @Description:: Set the NAND flash timing
 *
 *  @Input      ::
 *      pNandTiming: pointer to NAND timing registers' values
 *
 *  @Output     :: none
 *
 *  @Return     :: none
 */
void AmbaCSL_NandSetTiming(const AMBA_NAND_TIMING_CTRL_s *pNandTiming)
{
    AmbaCSL_NandSetTiming0(pNandTiming->Timing0);
    AmbaCSL_NandSetTiming1(pNandTiming->Timing1);
    AmbaCSL_NandSetTiming2(pNandTiming->Timing2);
    AmbaCSL_NandSetTiming3(pNandTiming->Timing3);
    AmbaCSL_NandSetTiming4(pNandTiming->Timing4);
    AmbaCSL_NandSetTiming5(pNandTiming->Timing5);
    AmbaCSL_NandSetTiming6(pNandTiming->Timing6);
}

/*
 *  @RoutineName:: AmbaCSL_NandDisableBCH
 *
 *  @Description:: Disable Bose-Chaudhuri-Hocquenghem (BCH) error correction
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
 */
void AmbaCSL_NandDisableBCH(UINT32 MainByteSize, UINT32 SpareByteSize)
{
    AMBA_FIO_CTRL_REG_s FioCtrlRegVal;
    AMBA_FDMA_DSM_CTRL_REG_s FdmaDsmCtrlRegVal = {0};

    FioCtrlRegVal = AmbaCSL_FioGetCtrlReg();

    /* Setup FIO Dual Space Mode Control Register */
    if (MainByteSize == 4096U) {
        FdmaDsmCtrlRegVal.MainStrideSize = AMBA_FDMA_DSM_STRIDE_SIZE_4KB;
        if (SpareByteSize == 128U) {
            FdmaDsmCtrlRegVal.SpareStrideSize =  AMBA_FDMA_DSM_STRIDE_SIZE_128B;
        } else {
            FdmaDsmCtrlRegVal.SpareStrideSize =  AMBA_FDMA_DSM_STRIDE_SIZE_256B;
        }

    } else {
        FdmaDsmCtrlRegVal.MainStrideSize = AMBA_FDMA_DSM_STRIDE_SIZE_2KB;
        if (SpareByteSize == 128U) {
            FdmaDsmCtrlRegVal.SpareStrideSize = AMBA_FDMA_DSM_STRIDE_SIZE_128B;
        } else {
            FdmaDsmCtrlRegVal.SpareStrideSize = AMBA_FDMA_DSM_STRIDE_SIZE_64B;
        }
    }
    /* Turn off FIO Dual Space Mode Control Register */
    FioCtrlRegVal.StopOnError = 0;
    FioCtrlRegVal.BchEnable   = 0;
    FioCtrlRegVal.Bch8Bits    = 0;

    AmbaCSL_FioSetCtrlReg(FioCtrlRegVal);
    AmbaCSL_FioSetFdmaDsmCtrlReg(FdmaDsmCtrlRegVal);
    AmbaCSL_FioSetEccReportConfig(0);
}

/*
 *  @RoutineName:: AmbaCSL_NandEnableBCH
 *
 *  @Description:: Enable Bose-Chaudhuri-Hocquenghem (BCH) error correction (6-bit or 8-bit)
 *
 *  @Input      ::
 *      BchBits: Number of ECC bits
 *
 *  @Output     :: none
 *
 *  @Return     :: none
 */
void AmbaCSL_NandEnableBCH(UINT32 BchBits)
{
    AMBA_FIO_CTRL_REG_s      FioCtrlRegVal;
    AMBA_FDMA_DSM_CTRL_REG_s FdmaDsmCtrlRegVal = {0};

    if ((BchBits != 6U) && (BchBits != 8U)) {
        /* wrong parameter */
    } else {

        AmbaCSL_FioSetEccReportConfig(BchBits);

        FioCtrlRegVal = AmbaCSL_FioGetCtrlReg();

        FioCtrlRegVal.StopOnError      = 1U;
        FioCtrlRegVal.SkipBlankPageEcc = 1U;
        FioCtrlRegVal.BchEnable        = 1U;

        if (BchBits == 6U) {
            FioCtrlRegVal.Bch8Bits = 0U;
            FdmaDsmCtrlRegVal.SpareStrideSize = AMBA_FDMA_DSM_STRIDE_SIZE_16B;
        } else if (BchBits == 8U) {
            FioCtrlRegVal.Bch8Bits = 1U;
            FdmaDsmCtrlRegVal.SpareStrideSize = AMBA_FDMA_DSM_STRIDE_SIZE_32B;
        } else {
            /* For Misra C checking */
        }

        /* Setup FIO Dual Space Mode Control Register */
        FdmaDsmCtrlRegVal.MainStrideSize = AMBA_FDMA_DSM_STRIDE_SIZE_512B;

        AmbaCSL_FioSetCtrlReg(FioCtrlRegVal);
        AmbaCSL_FioSetFdmaDsmCtrlReg(FdmaDsmCtrlRegVal);
    }
}

AMBA_NAND_CTRL_REG_s AmbaCSL_NandGetCtrlReg(void)
{
    return pAmbaFIO_Reg->NandCtrl;
}

