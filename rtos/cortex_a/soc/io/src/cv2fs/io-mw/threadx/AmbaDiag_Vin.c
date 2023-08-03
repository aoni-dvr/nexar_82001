/**
 *  @file AmbaDiag_Vin.c
 *
 *  @copyright Copyright (c) 2020 Ambarella International LP
 *
 *  This file and its contents ("Software") are protected by intellectual
 *  property rights including, without limitation, U.S. and/or foreign
 *  copyrights. This Software is also the confidential and proprietary
 *  information of Ambarella International LP and its licensors. You may not use, reproduce,
 *  disclose, distribute, modify, or otherwise prepare derivative works of this
 *  Software or any portion thereof except pursuant to a signed license agreement
 *  or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
 *  In the absence of such an agreement, you agree to promptly notify and return
 *  this Software to Ambarella International LP.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 *  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 *  MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 *  IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 *  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details Diagnostic functions for video inputs
 *
 */

#include "AmbaTypes.h"
#include "AmbaMisraFix.h"
#include "AmbaIOUtility.h"

#include "AmbaKAL.h"
#include "AmbaDef.h"

#if defined(CONFIG_QNX)
#include "AmbaSYS.h"
#endif
#include "AmbaVIN.h"
#include "AmbaCSL_VIN.h"
#include "AmbaCSL_DebugPort.h"
#include "AmbaRTSL_PLL.h"

#if defined(CONFIG_VIN_ASIL)
#include "AmbaSafety_VIN.h"
#else
#include "AmbaVIN_Ctrl.h"
#endif

static UINT32 VIN_MutexTake(void)
{
#if defined(CONFIG_VIN_ASIL)
    /* Use global mutex */
    AmbaSafety_VinShadowLock();

    return OK;
#else
    return AmbaKAL_MutexTake(&AmbaVinMutex, 100);
#endif
}

static UINT32 VIN_MutexGive(void)
{
#if defined(CONFIG_VIN_ASIL)
    /* Use global mutex */
    AmbaSafety_VinShadowUnLock();

    return OK;
#else
    return AmbaKAL_MutexGive(&AmbaVinMutex);
#endif
}

static void IoDiag_PrintFormattedInt(const char *pFmtString, UINT32 Value, AMBA_SYS_LOG_f LogFunc)
{
    char StrBuf[64];
    UINT32 ArgUINT32[2];

    ArgUINT32[0] = Value;
    (void)IO_UtilityStringPrintUInt32(StrBuf, (UINT32)sizeof(StrBuf), pFmtString, 1U, ArgUINT32);
    LogFunc(StrBuf);
}

static void IoDiag_VinCtrlRegParse(const AMBA_DBG_PORT_IDSP_VIN_MAIN_REG_s *pVinMainReg, AMBA_SYS_LOG_f LogFunc)
{
    LogFunc("Ctrl:\n");
    IoDiag_PrintFormattedInt(" ENABLE:        %d\n", pVinMainReg->Ctrl0.Enable, LogFunc);
    IoDiag_PrintFormattedInt(" OUTPUT_ENABLE: %d\n", pVinMainReg->Ctrl0.OutputEnable, LogFunc);
    IoDiag_PrintFormattedInt(" WORD_SIZE:     %d-bit\n", 8U + ((UINT32)pVinMainReg->Ctrl0.BitsPerPixel << 1U), LogFunc);

    IoDiag_PrintFormattedInt(" LANE_ENABLE:   0x%04x\n", (UINT32)pVinMainReg->Ctrl1.DataLaneEnable, LogFunc);
    IoDiag_PrintFormattedInt("  [0]: %d\n", ((UINT32)pVinMainReg->Ctrl1.DataLaneEnable) & 0x1U, LogFunc);
    IoDiag_PrintFormattedInt("  [1]: %d\n", ((UINT32)pVinMainReg->Ctrl1.DataLaneEnable >> 1U) & 0x1U, LogFunc);
    IoDiag_PrintFormattedInt("  [2]: %d\n", ((UINT32)pVinMainReg->Ctrl1.DataLaneEnable >> 2U) & 0x1U, LogFunc);
    IoDiag_PrintFormattedInt("  [3]: %d\n", ((UINT32)pVinMainReg->Ctrl1.DataLaneEnable >> 3U) & 0x1U, LogFunc);
    IoDiag_PrintFormattedInt("  [4]: %d\n", ((UINT32)pVinMainReg->Ctrl1.DataLaneEnable >> 4U) & 0x1U, LogFunc);
    IoDiag_PrintFormattedInt("  [5]: %d\n", ((UINT32)pVinMainReg->Ctrl1.DataLaneEnable >> 5U) & 0x1U, LogFunc);
    IoDiag_PrintFormattedInt("  [6]: %d\n", ((UINT32)pVinMainReg->Ctrl1.DataLaneEnable >> 6U) & 0x1U, LogFunc);
    IoDiag_PrintFormattedInt("  [7]: %d\n", ((UINT32)pVinMainReg->Ctrl1.DataLaneEnable >> 7U) & 0x1U, LogFunc);
    IoDiag_PrintFormattedInt("  [8]: %d\n", ((UINT32)pVinMainReg->Ctrl1.DataLaneEnable >> 8U) & 0x1U, LogFunc);
    IoDiag_PrintFormattedInt("  [9]: %d\n", ((UINT32)pVinMainReg->Ctrl1.DataLaneEnable >> 9U) & 0x1U, LogFunc);
    IoDiag_PrintFormattedInt("  [10]: %d\n", ((UINT32)pVinMainReg->Ctrl1.DataLaneEnable >> 10U) & 0x1U, LogFunc);
    IoDiag_PrintFormattedInt("  [11]: %d\n", ((UINT32)pVinMainReg->Ctrl1.DataLaneEnable >> 11U) & 0x1U, LogFunc);

    if (pVinMainReg->DvpCtrl.YuvEnable == 0U) {
        LogFunc(" RGB_YUV_MODE:   RGB\n");
    } else {
        LogFunc(" RGB_YUV_MODE:   YUV\n");
        IoDiag_PrintFormattedInt(" PIX_REORDER:          %d\n", pVinMainReg->DvpCtrl.YuvOrder, LogFunc);
    }
    IoDiag_PrintFormattedInt(" DISABLE_AFTER_VSYNC:  %d\n", pVinMainReg->DvpCtrl.DisableAfterSync, LogFunc);
    IoDiag_PrintFormattedInt(" DOUBLE_BUFFER_ENABLE: %d\n", pVinMainReg->DvpCtrl.DoubleBufferEnable, LogFunc);

    LogFunc("-----------------------------------------------------------------------\n");
}

static void IoDiag_VinStatusRegParse(const AMBA_DBG_PORT_IDSP_VIN_MAIN_REG_s *pVinMainReg, AMBA_SYS_LOG_f LogFunc)
{
    UINT32 SyncState;
    LogFunc("Status:\n");
    IoDiag_PrintFormattedInt(" GOT_ACT_SOF:             %d\n", pVinMainReg->Status.GotActSof, LogFunc);
    IoDiag_PrintFormattedInt(" GOT_WIN_SOF:             %d\n", pVinMainReg->Status.GotWinSof, LogFunc);
    IoDiag_PrintFormattedInt(" GOT_WIN_EOF:             %d\n", pVinMainReg->Status.GotWinEof, LogFunc);
    IoDiag_PrintFormattedInt(" GOT_VSYNC:               %d\n", pVinMainReg->Status.GotVsync, LogFunc);
    IoDiag_PrintFormattedInt(" AFIFO_OVERFLOW:          %d\n", pVinMainReg->Status.AfifoOverflow, LogFunc);
    IoDiag_PrintFormattedInt(" SFIFO_OVERFLOW:          %d\n", pVinMainReg->Status.SfifoOverFlow, LogFunc);
    IoDiag_PrintFormattedInt(" SHORT_LINE:              %d\n", pVinMainReg->Status.ShortLine, LogFunc);
    IoDiag_PrintFormattedInt(" SHORT_FRAME:             %d\n", pVinMainReg->Status.ShortFrame, LogFunc);
    IoDiag_PrintFormattedInt(" FIELD:                   %d\n", pVinMainReg->Status.Field, LogFunc);
    IoDiag_PrintFormattedInt(" RESERVED0:               %d\n", pVinMainReg->Status.Reserved, LogFunc);
    IoDiag_PrintFormattedInt(" SENT_MASTER_VSYNC:       %d\n", pVinMainReg->Status.SentMasterVsync, LogFunc);
    IoDiag_PrintFormattedInt(" UNCORRECTABLE_656_ERROR: %d\n", pVinMainReg->Status.Uncorrectable656Error, LogFunc);
    IoDiag_PrintFormattedInt(" SYNC_LOCKED:             %d\n", pVinMainReg->Status.SyncLocked, LogFunc);
    IoDiag_PrintFormattedInt(" LOST_LOCK_AFTER_SOF:     %d\n", pVinMainReg->Status.LostLockAfterSof, LogFunc);
    IoDiag_PrintFormattedInt(" PARTIAL_SYNC_DETECTED:   %d\n", pVinMainReg->Status.PartialSyncDetected, LogFunc);
    IoDiag_PrintFormattedInt(" UNKNOWN_SYNC_CODE:       %d\n", pVinMainReg->Status.UnknownSyncCode, LogFunc);
    LogFunc("\n");

    SyncState = pVinMainReg->SlvsStatus.SyncState;
    if (SyncState == 0U) {
        LogFunc(" SYNC_STATE:              IDLE\n");
    } else if (SyncState == 1U) {
        LogFunc(" SYNC_STATE:              ACTIVE\n");
    } else if (SyncState == 2U) {
        LogFunc(" SYNC_STATE:              HSYNC\n");
    } else if (SyncState == 3U) {
        LogFunc(" SYNC_STATE:              VSYNC\n");
    } else {
        /* avoid misra-c error*/
    }

    IoDiag_PrintFormattedInt(" SERIAL_DESKEW_ERROR:     %d\n", pVinMainReg->SlvsStatus.SerialDeskewError, LogFunc);
    IoDiag_PrintFormattedInt(" WATCHDOG_TIMEOUT:        %d\n", pVinMainReg->SlvsStatus.WatchdogTimeout, LogFunc);
    IoDiag_PrintFormattedInt(" SFIFO_COUNT:             %d\n", pVinMainReg->SlvsStatus.SfifoCount, LogFunc);
    LogFunc("\n");
    IoDiag_PrintFormattedInt(" DETECTED_ACTIVE_WIDTH:   %d\n", pVinMainReg->DetectedActiveWidth, LogFunc);
    IoDiag_PrintFormattedInt(" DETECTED_ACTIVE_HEIGHT:  %d\n", pVinMainReg->DetectedActiveHeight, LogFunc);
    IoDiag_PrintFormattedInt(" SYNC_CODE:               %d\n", pVinMainReg->SyncCode, LogFunc);
    IoDiag_PrintFormattedInt(" ACT_V_COUNT:             %d\n", pVinMainReg->ActiveFrameHeight, LogFunc);
    IoDiag_PrintFormattedInt(" ACT_H_COUNT:             %d\n", pVinMainReg->ActiveFrameWidth, LogFunc);
    IoDiag_PrintFormattedInt(" WIN_V_COUNT:             %d\n", pVinMainReg->CropRegionHeight, LogFunc);
    IoDiag_PrintFormattedInt(" WIN_H_COUNT:             %d\n", pVinMainReg->CropRegionWidth, LogFunc);

    LogFunc("-----------------------------------------------------------------------\n");

}

static void IoDiag_VinMipiStatusRegParse(const AMBA_DBG_PORT_IDSP_VIN_MAIN_REG_s *pVinMainReg, AMBA_SYS_LOG_f LogFunc)
{
    LogFunc("MIPI Status:\n");
    IoDiag_PrintFormattedInt(" MIPI_ECC_ERR_2BIT:       %d\n", pVinMainReg->MipiStatus0.EccError2Bit, LogFunc);
    IoDiag_PrintFormattedInt(" MIPI_ECC_ERR_1BIT:       %d\n", pVinMainReg->MipiStatus0.EccError1Bit, LogFunc);
    IoDiag_PrintFormattedInt(" MIPI_CRC_ERROR:          %d\n", pVinMainReg->MipiStatus0.CrcError, LogFunc);
    IoDiag_PrintFormattedInt(" MIPI_ERR_FS:             %d\n", pVinMainReg->MipiStatus0.FrameSyncError, LogFunc);
    IoDiag_PrintFormattedInt(" MIPI_PROT_STATE:         %d\n", pVinMainReg->MipiStatus0.ProtState, LogFunc);
    IoDiag_PrintFormattedInt(" MIPI_SOF_PACKET_RCVD:    %d\n", pVinMainReg->MipiStatus0.SofPacketReceived, LogFunc);
    IoDiag_PrintFormattedInt(" MIPI_EOF_PACKET_RCVD:    %d\n", pVinMainReg->MipiStatus0.EofPacketReceived, LogFunc);
    IoDiag_PrintFormattedInt(" MIPI_LONG_PACKET_COUNT:  %d\n", pVinMainReg->MipiLongPacketCount, LogFunc);
    IoDiag_PrintFormattedInt(" MIPI_SHORT_PACKET_COUNT: %d\n", pVinMainReg->MipiShortPacketCount, LogFunc);
    IoDiag_PrintFormattedInt(" MIPI_LONG_PACKET_SIZE:   %d\n", pVinMainReg->MipiLongPacketSize, LogFunc);
    IoDiag_PrintFormattedInt(" MIPI_NUM_CRC_ERR_PACKET: %d\n", pVinMainReg->MipiNumCrcErrPacket, LogFunc);
    IoDiag_PrintFormattedInt(" MIPI_FRAME_NUMBER:       %d\n", pVinMainReg->MipiFrameNumber, LogFunc);
    IoDiag_PrintFormattedInt(" MIPI_CRC_ERR_TH:         %d\n", pVinMainReg->MipiCrcErrThreshold, LogFunc);
    IoDiag_PrintFormattedInt(" MIPI_CORR_ECC_ERR_TH:    %d\n", pVinMainReg->MipiCorrEccErrThreshold, LogFunc);
    IoDiag_PrintFormattedInt(" MIPI_NUM_CORR_ERR_PACKET:%d\n", pVinMainReg->MipiNumCorrEccErrPacket, LogFunc);

    LogFunc("-----------------------------------------------------------------------\n");

}

static void IoDiag_VinDumpData(const void *pData, UINT32 Size, UINT32 PhysicalAddr, AMBA_SYS_LOG_f LogFunc)
{
    const UINT32 *pReg;
    ULONG BaseAddr, DumpAddr, i;

    AmbaMisra_TypeCast(&BaseAddr, &pData);

    for (i = 0U; i < Size; i++) {
        DumpAddr = BaseAddr + (i * 4U);
        AmbaMisra_TypeCast(&pReg, &DumpAddr);

        if ((i & 0x3U) == 0x0U) {
            IoDiag_PrintFormattedInt("0x%08X:  ", PhysicalAddr + (i * 4U), LogFunc);
            IoDiag_PrintFormattedInt("0x%08X ", *pReg, LogFunc);
        } else {
            IoDiag_PrintFormattedInt("0x%08X ", *pReg, LogFunc);
        }

        if ((i & 0x3U) == 0x3U) {
            LogFunc("\n");
        }
    }

    if ((i & 0x3U) != 0x0U) {
        LogFunc("\n");
    }
}

static void IoDiag_VinDumpMainRegs(const AMBA_DBG_PORT_IDSP_VIN_MAIN_REG_s *pVinMainReg, AMBA_SYS_LOG_f LogFunc)
{
    UINT32 PhysicalAddr = AMBA_DBG_PORT_IDSP_VIN_MAIN_CONFIG_BASE_ADDR;
    UINT32 Size = AMBA_CSL_VIN_MAIN_CONFIG_SIZE - 1U; /* minus 1 to exclude debug_command_shift */

    LogFunc("Main Registers :\n");
    IoDiag_VinDumpData(pVinMainReg, Size, PhysicalAddr, LogFunc);
}

static void IoDiag_VinDumpMSyncRegs(const AMBA_DBG_PORT_IDSP_VIN_MASTER_SYNC_REG_s *pVinMasterSyncReg, UINT32 VinChan, AMBA_SYS_LOG_f LogFunc)
{
    UINT32 PhysicalAddr = AMBA_DBG_PORT_IDSP_VIN_MASTER_SYNC_CONFIG_BASE_ADDR;
    UINT32 Size = AMBA_CSL_VIN_MASTER_SYNC_CONFIG_SIZE;

    LogFunc("Master Sync Registers :\n");
    if ((VinChan != AMBA_VIN_CHANNEL0) && (VinChan != AMBA_VIN_CHANNEL1)) {
        LogFunc(" => Only Vin channel 0 and 1 are supported\n");
    } else {
        IoDiag_VinDumpData(pVinMasterSyncReg, Size, PhysicalAddr, LogFunc);
    }
}

static void IoDiag_VinDumpGlobalRegs(const AMBA_DBG_PORT_IDSP_VIN_GLOBAL_REG_s *pVinGlobalReg, AMBA_SYS_LOG_f LogFunc)
{
    UINT32 PhysicalAddr = AMBA_DBG_PORT_IDSP_VIN_GLOBAL_CONFIG_BASE_ADDR;
    UINT32 Size = AMBA_CSL_VIN_GLOBAL_CONFIG_SIZE;

    LogFunc("Global Register :\n");
    IoDiag_VinDumpData(pVinGlobalReg, Size, PhysicalAddr, LogFunc);
}

static void IoDiag_VinLaneSelRegParse(const AMBA_DBG_PORT_IDSP_VIN_MAIN_REG_s *pVinMainReg, AMBA_SYS_LOG_f LogFunc)
{
    LogFunc("Lane Select: {");
    IoDiag_PrintFormattedInt(" %d,", pVinMainReg->DataPinMux.DataLane0_4_8PinSelect, LogFunc);
    IoDiag_PrintFormattedInt(" %d,", pVinMainReg->DataPinMux.DataLane1_5_9PinSelect, LogFunc);
    IoDiag_PrintFormattedInt(" %d,", pVinMainReg->DataPinMux.DataLane2_6_10PinSelect, LogFunc);
    IoDiag_PrintFormattedInt(" %d,", pVinMainReg->DataPinMux.DataLane3_7_11PinSelect, LogFunc);
    IoDiag_PrintFormattedInt(" %d,", pVinMainReg->DataPinMux1.DataLane0_4_8PinSelect, LogFunc);
    IoDiag_PrintFormattedInt(" %d,", pVinMainReg->DataPinMux1.DataLane1_5_9PinSelect, LogFunc);
    IoDiag_PrintFormattedInt(" %d,", pVinMainReg->DataPinMux1.DataLane2_6_10PinSelect, LogFunc);
    IoDiag_PrintFormattedInt(" %d,", pVinMainReg->DataPinMux1.DataLane3_7_11PinSelect, LogFunc);
    IoDiag_PrintFormattedInt(" %d,", pVinMainReg->DataPinMux2.DataLane0_4_8PinSelect, LogFunc);
    IoDiag_PrintFormattedInt(" %d,", pVinMainReg->DataPinMux2.DataLane1_5_9PinSelect, LogFunc);
    IoDiag_PrintFormattedInt(" %d,", pVinMainReg->DataPinMux2.DataLane2_6_10PinSelect, LogFunc);
    IoDiag_PrintFormattedInt(" %d", pVinMainReg->DataPinMux2.DataLane3_7_11PinSelect, LogFunc);

    LogFunc("}\n");

    LogFunc("-----------------------------------------------------------------------\n");
}

static void IoDiag_VinCropWinRegParse(const AMBA_DBG_PORT_IDSP_VIN_MAIN_REG_s *pVinMainReg, AMBA_SYS_LOG_f LogFunc)
{
    LogFunc("Win:\n");
    IoDiag_PrintFormattedInt(" ACTIVE_WIDTH:            %d\n", pVinMainReg->ActiveRegionWidth, LogFunc);
    IoDiag_PrintFormattedInt(" ACTIVE_HEIGHT:           %d\n", pVinMainReg->ActiveRegionHeight, LogFunc);
    IoDiag_PrintFormattedInt(" CROP_REGION:             { %d,", pVinMainReg->CropStartCol, LogFunc);
    IoDiag_PrintFormattedInt(" %d,", pVinMainReg->CropStartRow, LogFunc);
    IoDiag_PrintFormattedInt(" %d,", pVinMainReg->CropEndCol, LogFunc);
    IoDiag_PrintFormattedInt(" %d", pVinMainReg->CropEndRow, LogFunc);
    LogFunc(" }\n");
    IoDiag_PrintFormattedInt(" CROP_DISABLE:            %d\n", pVinMainReg->Ctrl2.DisableCropRegion, LogFunc);

    LogFunc("-----------------------------------------------------------------------\n");
}

static void IoDiag_VinMipiCtrlRegParse(const AMBA_DBG_PORT_IDSP_VIN_MAIN_REG_s *pVinMainReg, AMBA_SYS_LOG_f LogFunc)
{
    LogFunc("-----------------------------------------------------------------------\n");
    LogFunc("MipiCtrl:\n");

    IoDiag_PrintFormattedInt(" MIPI_VC_MASK:            0x%x\n", pVinMainReg->MipiCtrl0.VirtChanMask, LogFunc);
    IoDiag_PrintFormattedInt(" MIPI_VC_PAT:             0x%x\n", pVinMainReg->MipiCtrl0.VirtChanPattern, LogFunc);
    IoDiag_PrintFormattedInt(" MIPI_DT_MASK:            0x%x\n", pVinMainReg->MipiCtrl0.DataTypeMask, LogFunc);
    IoDiag_PrintFormattedInt(" MIPI_DT_PAT:             0x%x\n", pVinMainReg->MipiCtrl0.DataTypePattern, LogFunc);
    IoDiag_PrintFormattedInt(" MIPI_ED_DT_MASK:         0x%x\n", pVinMainReg->MipiCtrl1.EDDataTypeMask, LogFunc);
    IoDiag_PrintFormattedInt(" MIPI_ED_DT_PAT:          0x%x\n", pVinMainReg->MipiCtrl1.EDDataTypePattern, LogFunc);
    IoDiag_PrintFormattedInt(" MIPI_BYTE_SWAP:          %d\n", pVinMainReg->MipiCtrl1.ByteSwapEnable, LogFunc);
    IoDiag_PrintFormattedInt(" MIPI_ECC_ENABLE:         %d\n", pVinMainReg->MipiCtrl1.EccEnable, LogFunc);
    IoDiag_PrintFormattedInt(" MIPI_FORWARD_ECC_ENABLE: %d\n", pVinMainReg->MipiCtrl1.ForwardEccEnable, LogFunc);
    IoDiag_PrintFormattedInt(" MIPI_ED_ENABLE:          %d\n", pVinMainReg->MipiCtrl1.MipiEDEnable, LogFunc);

    LogFunc("-----------------------------------------------------------------------\n");
}

static void IoDiag_VinMipiPhyRegParse(UINT32 VinChan, AMBA_SYS_LOG_f LogFunc)
{
    LogFunc("-----------------------------------------------------------------------\n");
    LogFunc("MipiPhy:\n");

    if ((VinChan == AMBA_VIN_CHANNEL0) ||
        (VinChan == AMBA_VIN_CHANNEL2) ||
        (VinChan == AMBA_VIN_CHANNEL3) ||
        (VinChan == AMBA_VIN_CHANNEL4)) {
        IoDiag_PrintFormattedInt(" HsSettleTime:            %d\n", AmbaCSL_VinGetMipiHsSettle0(), LogFunc);
        IoDiag_PrintFormattedInt(" HsTermTime:              %d\n", AmbaCSL_VinGetMipiHsTerm0(), LogFunc);
        IoDiag_PrintFormattedInt(" ClkSettleTime:           %d\n", AmbaCSL_VinGetMipiClkSettle0(), LogFunc);
        IoDiag_PrintFormattedInt(" ClkTermTime:             %d\n", AmbaCSL_VinGetMipiClkTerm0(), LogFunc);
        IoDiag_PrintFormattedInt(" ClkMissTime:             %d\n", AmbaCSL_VinGetMipiClkMiss0(), LogFunc);
        IoDiag_PrintFormattedInt(" RxInitTime:              %d\n", AmbaCSL_VinGetMipiRxInit0(), LogFunc);
        IoDiag_PrintFormattedInt(" ForceClkHs:              %d\n", AmbaCSL_VinGetMipiForceClkHs0(), LogFunc);
    } else if (VinChan == AMBA_VIN_CHANNEL8) {
        IoDiag_PrintFormattedInt(" HsSettleTime:            %d\n", AmbaCSL_VinGetMipiHsSettle1(), LogFunc);
        IoDiag_PrintFormattedInt(" HsTermTime:              %d\n", AmbaCSL_VinGetMipiHsTerm1(), LogFunc);
        IoDiag_PrintFormattedInt(" ClkSettleTime:           %d\n", AmbaCSL_VinGetMipiClkSettle1(), LogFunc);
        IoDiag_PrintFormattedInt(" ClkTermTime:             %d\n", AmbaCSL_VinGetMipiClkTerm1(), LogFunc);
        IoDiag_PrintFormattedInt(" ClkMissTime:             %d\n", AmbaCSL_VinGetMipiClkMiss1(), LogFunc);
        IoDiag_PrintFormattedInt(" RxInitTime:              %d\n", AmbaCSL_VinGetMipiRxInit1(), LogFunc);
        IoDiag_PrintFormattedInt(" ForceClkHs:              %d\n", AmbaCSL_VinGetMipiForceClkHs1(), LogFunc);
    } else if (VinChan == AMBA_VIN_CHANNEL1) {
        IoDiag_PrintFormattedInt(" HsSettleTime:            %d\n", AmbaCSL_VinGetMipiHsSettle2(), LogFunc);
        IoDiag_PrintFormattedInt(" HsTermTime:              %d\n", AmbaCSL_VinGetMipiHsTerm2(), LogFunc);
        IoDiag_PrintFormattedInt(" ClkSettleTime:           %d\n", AmbaCSL_VinGetMipiClkSettle2(), LogFunc);
        IoDiag_PrintFormattedInt(" ClkTermTime:             %d\n", AmbaCSL_VinGetMipiClkTerm2(), LogFunc);
        IoDiag_PrintFormattedInt(" ClkMissTime:             %d\n", AmbaCSL_VinGetMipiClkMiss2(), LogFunc);
        IoDiag_PrintFormattedInt(" RxInitTime:              %d\n", AmbaCSL_VinGetMipiRxInit2(), LogFunc);
        IoDiag_PrintFormattedInt(" ForceClkHs:              %d\n", AmbaCSL_VinGetMipiForceClkHs2(), LogFunc);
    } else {
        /* avoid misrac error */
    }

    LogFunc("-----------------------------------------------------------------------\n");
}

static void IoDiag_VinMipiGrp0PhyStatusGet(UINT32 MaxNumCheck, UINT16 LaneEnable, UINT16 *pRxValidHS, UINT16 *pRxActiveHS, UINT16 *pClkActiveHs)
{
    UINT32 i;
    UINT16 RxValidHS = 0U, RxActiveHS = 0U, ClkActiveHs = 0U;

    for (i = 0U; i < MaxNumCheck; i ++) {
        if (ClkActiveHs != 1U) {
            ClkActiveHs |= (UINT16)AmbaCSL_VinGetMipiRxClkActHs0();
        }
        if (RxActiveHS != LaneEnable) {
            RxActiveHS |= (UINT16)((UINT16)pAmbaMIPI_Reg->Dphy0Obsv2.Bits.RxActiveHS & LaneEnable);
        }
        if (RxValidHS != LaneEnable) {
            RxValidHS |= (UINT16)((UINT16)pAmbaMIPI_Reg->Dphy0Obsv1.Bits.RxValidHS & LaneEnable);
        }
        if ((ClkActiveHs == 1U) && (RxActiveHS == LaneEnable) && (RxValidHS == LaneEnable)) {
            break;
        }
        (void)AmbaKAL_TaskSleep(1U);
    }

    *pRxValidHS = RxValidHS;
    *pRxActiveHS = RxActiveHS;
    *pClkActiveHs = ClkActiveHs;
}

static void IoDiag_VinMipiGrp1PhyStatusGet(UINT32 MaxNumCheck, UINT16 LaneEnable, UINT16 *pRxValidHS, UINT16 *pRxActiveHS, UINT16 *pClkActiveHs)
{
    UINT32 i;
    UINT16 RxValidHS = 0U, RxActiveHS = 0U, ClkActiveHs = 0U;

    for (i = 0; i < MaxNumCheck; i ++) {
        if (ClkActiveHs != 1U) {
            ClkActiveHs |= (UINT16)AmbaCSL_VinGetMipiRxClkActHs1();
        }
        if (RxActiveHS != LaneEnable) {
            RxActiveHS |= (UINT16)(((UINT16)pAmbaMIPI_Reg->Dphy0Obsv2.Bits.RxActiveHS >> 4U) & LaneEnable);
        }
        if (RxValidHS != LaneEnable) {
            RxValidHS |= (UINT16)(((UINT16)pAmbaMIPI_Reg->Dphy0Obsv1.Bits.RxValidHS >> 4U) & LaneEnable);
        }
        if ((ClkActiveHs == 1U) && (RxActiveHS == LaneEnable) && (RxValidHS == LaneEnable)) {
            break;
        }
        (void)AmbaKAL_TaskSleep(1U);
    }

    *pRxValidHS = RxValidHS;
    *pRxActiveHS = RxActiveHS;
    *pClkActiveHs = ClkActiveHs;
}

static void IoDiag_VinMipiGrp2PhyStatusGet(UINT32 MaxNumCheck, UINT16 LaneEnable, UINT16 *pRxValidHS, UINT16 *pRxActiveHS, UINT16 *pClkActiveHs)
{
    UINT32 i;
    UINT16 RxValidHS = 0U, RxActiveHS = 0U, ClkActiveHs = 0U;

    for (i = 0; i < MaxNumCheck; i ++) {
        if (ClkActiveHs != 1U) {
            ClkActiveHs |= (UINT16)AmbaCSL_VinGetMipiRxClkActHs2();
        }
        if (RxActiveHS != LaneEnable) {
            RxActiveHS |= (UINT16)((UINT16)pAmbaMIPI_Reg->Dphy0Obsv6.Bits.RxActiveHS_11_8 & LaneEnable);
        }
        if (RxValidHS != LaneEnable) {
            RxValidHS |= (UINT16)((UINT16)pAmbaMIPI_Reg->Dphy0Obsv5.Bits.RxValidHS_11_8 & LaneEnable);
        }
        if ((ClkActiveHs == 1U) && (RxActiveHS == LaneEnable) && (RxValidHS == LaneEnable)) {
            break;
        }
        (void)AmbaKAL_TaskSleep(1U);
    }

    *pRxValidHS = RxValidHS;
    *pRxActiveHS = RxActiveHS;
    *pClkActiveHs = ClkActiveHs;
}

static void IoDiag_VinMipiPhyStatusCheck(UINT32 DataLaneEnable, UINT32 VinChan, AMBA_SYS_LOG_f LogFunc)
{
    UINT32 MaxNumCheck = 100U;
    UINT16 LaneEnable = (UINT16)DataLaneEnable;
    UINT16 RxValidHS = 0U, RxActiveHS = 0U, ClkActiveHs = 0U;
    UINT32 i;

    if ((VinChan == AMBA_VIN_CHANNEL0) ||
        (VinChan == AMBA_VIN_CHANNEL2) ||
        (VinChan == AMBA_VIN_CHANNEL3) ||
        (VinChan == AMBA_VIN_CHANNEL4)) {
        IoDiag_VinMipiGrp0PhyStatusGet(MaxNumCheck, LaneEnable, &RxValidHS, &RxActiveHS, &ClkActiveHs);
    } else if (VinChan == AMBA_VIN_CHANNEL8) {
        IoDiag_VinMipiGrp1PhyStatusGet(MaxNumCheck, LaneEnable, &RxValidHS, &RxActiveHS, &ClkActiveHs);
    } else if (VinChan == AMBA_VIN_CHANNEL1) {
        IoDiag_VinMipiGrp2PhyStatusGet(MaxNumCheck, LaneEnable, &RxValidHS, &RxActiveHS, &ClkActiveHs);
    } else {
        /* avoid misra-c error */
    }

    if (ClkActiveHs == 1U) {
        LogFunc("ClkActiveHs check: OK\n");
    } else {
        LogFunc("ClkActiveHs check: NG\n");
    }

    LogFunc("RxValidHS check:\n");
    for (i = 0; i < 4U; i ++) {
        if (((RxValidHS >> i) & 0x1U) != 0U) {
            IoDiag_PrintFormattedInt(" [%d]: OK\n", i, LogFunc);
        } else {
            IoDiag_PrintFormattedInt(" [%d]: NG\n", i, LogFunc);
        }
    }
    LogFunc("RxActiveHS check:\n");
    for (i = 0; i < 4U; i ++) {
        if (((RxActiveHS >> i) & 0x1U) != 0U) {
            IoDiag_PrintFormattedInt(" [%d]: OK\n", i, LogFunc);
        } else {
            IoDiag_PrintFormattedInt(" [%d]: NG\n", i, LogFunc);
        }
    }
    LogFunc("-----------------------------------------------------------------------\n");
}

static void IoDiag_VinMSyncRegParse(const AMBA_DBG_PORT_IDSP_VIN_MASTER_SYNC_REG_s *pVinMasterSyncReg, UINT32 VinChan, AMBA_SYS_LOG_f LogFunc)
{
    LogFunc("-----------------------------------------------------------------------\n");
    LogFunc("Master Sync:\n");

    if ((VinChan != AMBA_VIN_CHANNEL0) && (VinChan != AMBA_VIN_CHANNEL1)) {
        LogFunc(" => Only Vin channel 0 and 1 are supported\n");
    } else {
        UINT32 Reg_H, Reg_L;
        UINT32 VinClk = 0U;

        if (VinChan == AMBA_VIN_CHANNEL0) {
#if defined(CONFIG_QNX)
            (void)AmbaSYS_GetIoClkFreq(AMBA_CLK_VIN0, &VinClk);
#else
            VinClk = AmbaRTSL_PllGetVin0Clk();
#endif
        } else if (VinChan == AMBA_VIN_CHANNEL1) {
#if defined(CONFIG_QNX)
            (void)AmbaSYS_GetIoClkFreq(AMBA_CLK_VIN1, &VinClk);
#else
            VinClk = AmbaRTSL_PllGetVin1Clk();
#endif
        } else {
            /* avoid misra-c error */
        }
        IoDiag_PrintFormattedInt(" Reference clock:            %d\n", VinClk, LogFunc);

        LogFunc(" Hsync:\n");

        Reg_H = (UINT32)pVinMasterSyncReg->HSyncPeriodH;
        Reg_L = (UINT32)pVinMasterSyncReg->HSyncPeriodL;
        IoDiag_PrintFormattedInt("  Period     (in ref clk):   %d\n", (Reg_H << 16U) + Reg_L, LogFunc);
        IoDiag_PrintFormattedInt("  Pulse width(in ref clk):   %d\n", pVinMasterSyncReg->HSyncWidth, LogFunc);
        IoDiag_PrintFormattedInt("  offset     (in ref clk):   %d\n", pVinMasterSyncReg->HSyncOffset, LogFunc);
        if (pVinMasterSyncReg->MasterSyncCtrl.HsyncPolarity == 0U) {
            LogFunc("  Polarity:                  Active Low\n");
        } else {
            LogFunc("  Polarity:                  Active High\n");
        }

        LogFunc(" Vsync:\n");

        Reg_H = (UINT32)pVinMasterSyncReg->VSyncPeriodH;
        Reg_L = (UINT32)pVinMasterSyncReg->VSyncPeriodL;
        IoDiag_PrintFormattedInt("  Period     (in hsync):     %d\n", (Reg_H << 16U) + Reg_L, LogFunc);
        IoDiag_PrintFormattedInt("  Pulse width(in ref clk):   %d\n", pVinMasterSyncReg->VSyncWidth, LogFunc);
        IoDiag_PrintFormattedInt("  offset     (in ref clk):   %d\n", pVinMasterSyncReg->VSyncOffset, LogFunc);
        if (pVinMasterSyncReg->MasterSyncCtrl.VsyncPolarity == 0U) {
            LogFunc("  Polarity:                  Active Low\n");
        } else {
            LogFunc("  Polarity:                  Active High\n");
        }
    }

    LogFunc("-----------------------------------------------------------------------\n");
}

static void IoDiag_VinGlobalRegParse(const AMBA_DBG_PORT_IDSP_VIN_GLOBAL_REG_s *pVinGlobalReg, AMBA_SYS_LOG_f LogFunc)
{
    LogFunc("-----------------------------------------------------------------------\n");

    if (pVinGlobalReg->GlobalConfig.SensorType == 0U) {
        LogFunc("SensorType:     SLVS\n");
    } else if (pVinGlobalReg->GlobalConfig.SensorType == 1U) {
        LogFunc("SensorType:     Parallel SLVS\n");
    } else if (pVinGlobalReg->GlobalConfig.SensorType == 2U) {
        LogFunc("SensorType:     LVCMOS\n");
    } else if (pVinGlobalReg->GlobalConfig.SensorType == 3U) {
        LogFunc("SensorType:     MIPI\n");
    } else {
        /* avoid misrac error */
    }
    LogFunc("-----------------------------------------------------------------------\n");
}

static void IoDiag_VinRegCopy(void *pDstBuf, const void *pVinReg)
{
    UINT32 i, Size;
    UINT32 *pDst;       /* Pointer to buffer where data will be stored */
    const UINT32 *pSrc; /* Pointer to VIN debug register */

    AmbaMisra_TypeCast(&pDst, &pDstBuf);
    AmbaMisra_TypeCast(&pSrc, &pVinReg);

    if (pVinReg == pAmbaIDSP_VinMainReg) {
        Size = AMBA_CSL_VIN_MAIN_CONFIG_SIZE;
    } else if (pVinReg == pAmbaIDSP_VinGlobalReg) {
        Size = AMBA_CSL_VIN_GLOBAL_CONFIG_SIZE;
    } else { /* (pVinReg == pAmbaIDSP_VinMasterSyncReg) */
        Size = AMBA_CSL_VIN_MASTER_SYNC_CONFIG_SIZE;
    }

    for (i = 0U; i < Size; i++) {
        pDst[i] = pSrc[i];
    }
}

/**
 *  AmbaDiag_VinShowInfo - Show Vin 0/1 info
 *  @param[in] VinID Vin channel
 *  @return error code
 */
UINT32 AmbaDiag_VinShowInfo(UINT32 VinID, AMBA_SYS_LOG_f LogFunc)
{
    UINT32 RetVal = VIN_ERR_NONE;
    UINT32 Section = AMBA_DBG_PORT_IDSP_VIN0;
    AMBA_DBG_PORT_IDSP_VIN_MAIN_REG_s VinMainReg = {0};
    AMBA_DBG_PORT_IDSP_VIN_GLOBAL_REG_s VinGlobalReg = {0};
    AMBA_DBG_PORT_IDSP_VIN_MASTER_SYNC_REG_s VinMasterSyncReg = {0};

    if ((VinID >= AMBA_NUM_VIN_CHANNEL) || (LogFunc == NULL)) {
        RetVal = VIN_ERR_ARG;
    } else {
        if (VinID == AMBA_VIN_CHANNEL0) {
            Section = AMBA_DBG_PORT_IDSP_VIN0;
        } else if (VinID == AMBA_VIN_CHANNEL1) {
            Section = AMBA_DBG_PORT_IDSP_VIN1;
        } else if (VinID == AMBA_VIN_CHANNEL2) {
            Section = AMBA_DBG_PORT_IDSP_VIN2;
        } else if (VinID == AMBA_VIN_CHANNEL3) {
            Section = AMBA_DBG_PORT_IDSP_VIN3;
        } else if (VinID == AMBA_VIN_CHANNEL4) {
            Section = AMBA_DBG_PORT_IDSP_VIN4;
        } else if (VinID == AMBA_VIN_CHANNEL8) {
            Section = AMBA_DBG_PORT_IDSP_VIN8;
        } else {
            //nothing
        }

        if (VIN_MutexTake() == OK) {
            AmbaCSL_IdspSetAddrExtensionData(Section << 12U);
            AmbaDelayCycles(0x3fffU); /* wait for section selection */

            /* Copy VIN Main configuration to local buffer */
            IoDiag_VinRegCopy(&VinMainReg, pAmbaIDSP_VinMainReg);

            /* Copy VIN Global configuration to local buffer */
            IoDiag_VinRegCopy(&VinGlobalReg, pAmbaIDSP_VinGlobalReg);

            if ((VinID == AMBA_VIN_CHANNEL0) || (VinID == AMBA_VIN_CHANNEL1)) {
                /* Copy VIN Master Sync configuration to local buffer */
                IoDiag_VinRegCopy(&VinMasterSyncReg, pAmbaIDSP_VinMasterSyncReg);
            }

            /*
             * Release the Mutex
             */
            (void)VIN_MutexGive();
        }

        IoDiag_PrintFormattedInt("Section: %d\n", Section, LogFunc);

        LogFunc("<VIN INFO PARSE BEGIN>\n");

        IoDiag_VinGlobalRegParse(&VinGlobalReg, LogFunc);

        IoDiag_VinCtrlRegParse(&VinMainReg, LogFunc);

        IoDiag_VinLaneSelRegParse(&VinMainReg, LogFunc);

        IoDiag_VinCropWinRegParse(&VinMainReg, LogFunc);

        IoDiag_VinMipiCtrlRegParse(&VinMainReg, LogFunc);

        IoDiag_VinMipiPhyRegParse(VinID, LogFunc);

        IoDiag_VinStatusRegParse(&VinMainReg, LogFunc);

        IoDiag_VinMipiStatusRegParse(&VinMainReg, LogFunc);

        IoDiag_VinMipiPhyStatusCheck(VinMainReg.Ctrl1.DataLaneEnable, VinID, LogFunc);

        IoDiag_VinMSyncRegParse(&VinMasterSyncReg, VinID, LogFunc);

        IoDiag_VinDumpMainRegs(&VinMainReg, LogFunc);

        IoDiag_VinDumpGlobalRegs(&VinGlobalReg, LogFunc);

        IoDiag_VinDumpMSyncRegs(&VinMasterSyncReg, VinID, LogFunc);
    }

    return RetVal;
}

/**
 *  AmbaDiag_VinResetStatus - Reset status register
 *  @param[in] VinID Vin channel
 *  @return error code
 */
UINT32 AmbaDiag_VinResetStatus(UINT32 VinID, AMBA_SYS_LOG_f LogFunc)
{
    UINT32 RetVal = VIN_ERR_NONE;
    UINT32 Section = AMBA_DBG_PORT_IDSP_VIN0;
    UINT32* pAddr;
    const volatile AMBA_DBG_PORT_IDSP_STATUS_REG_s* pStatusReg;
    const volatile AMBA_DBG_PORT_IDSP_SLVS_STATUS_REG_s* pSlvsStatusReg;
    const volatile AMBA_DBG_PORT_IDSP_MIPI_STATUS0_REG_s* pMipiStatus0Reg;

    if (VinID >= AMBA_NUM_VIN_CHANNEL) {
        RetVal = VIN_ERR_ARG;
    } else {
        if (VinID == AMBA_VIN_CHANNEL0) {
            Section = AMBA_DBG_PORT_IDSP_VIN0;
        } else if (VinID == AMBA_VIN_CHANNEL1) {
            Section = AMBA_DBG_PORT_IDSP_VIN1;
        } else if (VinID == AMBA_VIN_CHANNEL2) {
            Section = AMBA_DBG_PORT_IDSP_VIN2;
        } else if (VinID == AMBA_VIN_CHANNEL3) {
            Section = AMBA_DBG_PORT_IDSP_VIN3;
        } else if (VinID == AMBA_VIN_CHANNEL4) {
            Section = AMBA_DBG_PORT_IDSP_VIN4;
        } else if (VinID == AMBA_VIN_CHANNEL8) {
            Section = AMBA_DBG_PORT_IDSP_VIN8;
        } else {
            //nothing
        }

        if (LogFunc != NULL) {
            IoDiag_PrintFormattedInt("Section: %d\n", Section, LogFunc);
        }

        if (VIN_MutexTake() == OK) {
            AmbaCSL_IdspSetAddrExtensionData(Section << 12U);
            AmbaDelayCycles(0x3fffU); /* wait for section selection */

            pStatusReg = &pAmbaIDSP_VinMainReg->Status;
            AmbaMisra_TypeCast(&pAddr, &pStatusReg);
            *pAddr  = 0xffffffffU;
            pSlvsStatusReg = &pAmbaIDSP_VinMainReg->SlvsStatus;
            AmbaMisra_TypeCast(&pAddr, &pSlvsStatusReg);
            *pAddr  = 0xffffffffU;
            pMipiStatus0Reg = &pAmbaIDSP_VinMainReg->MipiStatus0;
            AmbaMisra_TypeCast(&pAddr, &pMipiStatus0Reg);
            *pAddr  = 0xffffffffU;
            pAmbaIDSP_VinMainReg->MipiLongPacketCount     = 0xffffffffU;
            pAmbaIDSP_VinMainReg->MipiShortPacketCount    = 0xffffffffU;
            pAmbaIDSP_VinMainReg->MipiNumCrcErrPacket     = 0xffffffffU;
            pAmbaIDSP_VinMainReg->MipiNumCorrEccErrPacket = 0xffffffffU;

            /*
             * Release the Mutex
             */
            (void)VIN_MutexGive();

        } else {
            RetVal = VIN_ERR_MUTEX;
        }

        if (LogFunc != NULL) {
            LogFunc("clear Status,\n SlvsStatus,\n MipiStatus0,\n MipiLongPacketCount,\n MipiShortPacketCount,\n MipiNumCrcErrPacket\n, MipiNumCorrEccErrPacket\n");
        }
    }

    return RetVal;
}

/**
 *  AmbaDiag_VinConfigThrDbgPort - Configure Vin Main through debug port
 *  @param[in] VinID Vin channel
 *  @param[in] CaptureWidth Vin capture window width
 *  @param[in] CaptureHeight Vin Capture window height
 *  @return error code
 */
UINT32 AmbaDiag_VinConfigThrDbgPort(UINT32 VinID, UINT32 CaptureWidth, UINT32 CaptureHeight, AMBA_SYS_LOG_f LogFunc)
{
    UINT32 RetVal;
    ULONG BufAddr = 0U;
    UINT32 BufSize = 0U;

    if (VinID >= AMBA_NUM_VIN_CHANNEL) {
        LogFunc("[Diag][Vin] ERROR: Invalid Vin id\n");
        RetVal = VIN_ERR_ARG;
    } else {
        RetVal = AmbaVIN_GetMainCfgBufInfo(VinID, &BufAddr, &BufSize);
    }

    if (RetVal != VIN_ERR_NONE) {
        IoDiag_PrintFormattedInt("Get VIN MainBuf Address failed! RetVal:%d\n", RetVal, LogFunc);
    } else {
        if ((CaptureWidth != 0U) && (CaptureHeight != 0U)) {
            AMBA_VIN_WINDOW_s CapWin;

            CapWin.OffsetX = 0U;
            CapWin.OffsetY = 0U;
            CapWin.Width = (UINT16)CaptureWidth;
            CapWin.Height = (UINT16)CaptureHeight;

            RetVal = AmbaVIN_CaptureConfig(VinID, &CapWin);

            IoDiag_PrintFormattedInt("Config Capture Window = %d", CaptureWidth, LogFunc);
            IoDiag_PrintFormattedInt("x %d\n", CaptureHeight, LogFunc);
        }

        if (RetVal == VIN_ERR_NONE) {
            if (VIN_MutexTake() == OK) {
                RetVal = AmbaCSL_VinConfigMainThrDbgPort(VinID, BufAddr);

                /*
                 * Release the Mutex
                 */
                (void)VIN_MutexGive();

                IoDiag_PrintFormattedInt("Config VIN %d Main Through DebugPort\n", VinID, LogFunc);
            } else {
                RetVal = VIN_ERR_MUTEX;
            }
        }
    }

    return RetVal;
}

/**
 *  AmbaDiag_VinGetStatus - Get Vin status
 *  @param[in] VinID Vin channel
 *  @param[out] pVinStatus Pointer to Vin Status
 *  @return error code
 */
UINT32 AmbaDiag_VinGetStatus(UINT32 VinID, IODIAG_VIN_STATUS_s *pVinStatus)
{
    UINT32 RetVal = VIN_ERR_NONE;
    UINT32 Section = AMBA_DBG_PORT_IDSP_VIN0;

    if ((VinID >= AMBA_NUM_VIN_CHANNEL) || (pVinStatus == NULL)) {
        RetVal = VIN_ERR_ARG;
    } else {
        if (VinID == AMBA_VIN_CHANNEL0) {
            Section = AMBA_DBG_PORT_IDSP_VIN0;
        } else if (VinID == AMBA_VIN_CHANNEL1) {
            Section = AMBA_DBG_PORT_IDSP_VIN1;
        } else if (VinID == AMBA_VIN_CHANNEL2) {
            Section = AMBA_DBG_PORT_IDSP_VIN2;
        } else if (VinID == AMBA_VIN_CHANNEL3) {
            Section = AMBA_DBG_PORT_IDSP_VIN3;
        } else if (VinID == AMBA_VIN_CHANNEL4) {
            Section = AMBA_DBG_PORT_IDSP_VIN4;
        } else if (VinID == AMBA_VIN_CHANNEL8) {
            Section = AMBA_DBG_PORT_IDSP_VIN8;
        } else {
            // nothing
        }

        if (VIN_MutexTake() == OK) {
            AmbaCSL_IdspSetAddrExtensionData(Section << 12U);
            AmbaDelayCycles(0x3fffU); /* wait for section selection */

            //General case
            pVinStatus->GotActSof = pAmbaIDSP_VinMainReg->Status.GotActSof;
            pVinStatus->GotWinSof = pAmbaIDSP_VinMainReg->Status.GotWinSof;
            pVinStatus->GotWinEof = pAmbaIDSP_VinMainReg->Status.GotWinEof;
            pVinStatus->GotVsync = pAmbaIDSP_VinMainReg->Status.GotVsync;
            pVinStatus->AfifoOverflow = pAmbaIDSP_VinMainReg->Status.AfifoOverflow;
            pVinStatus->SfifoOverflow = pAmbaIDSP_VinMainReg->Status.SfifoOverFlow;
            pVinStatus->ShortLine = pAmbaIDSP_VinMainReg->Status.ShortLine;
            pVinStatus->ShortFrame = pAmbaIDSP_VinMainReg->Status.ShortFrame;
            pVinStatus->WatchdogTimeout = pAmbaIDSP_VinMainReg->SlvsStatus.WatchdogTimeout;

            //MIPI
            pVinStatus->EccError2Bit = pAmbaIDSP_VinMainReg->MipiStatus0.EccError2Bit;
            pVinStatus->EccError1Bit = pAmbaIDSP_VinMainReg->MipiStatus0.EccError1Bit;
            pVinStatus->CrcError = pAmbaIDSP_VinMainReg->MipiStatus0.CrcError;
            pVinStatus->FrameSyncError = pAmbaIDSP_VinMainReg->MipiStatus0.FrameSyncError;
            pVinStatus->SofPacketReceived = pAmbaIDSP_VinMainReg->MipiStatus0.SofPacketReceived;
            pVinStatus->EofPacketReceived = pAmbaIDSP_VinMainReg->MipiStatus0.EofPacketReceived;
            pVinStatus->SerialDeskewError = pAmbaIDSP_VinMainReg->SlvsStatus.SerialDeskewError; //MIPI & SLVS

            //SLVS & LVCMOS
            pVinStatus->Uncorrectable656Error = pAmbaIDSP_VinMainReg->Status.Uncorrectable656Error;
            pVinStatus->PartialSyncDetected = pAmbaIDSP_VinMainReg->Status.PartialSyncDetected;
            pVinStatus->UnknownSyncCode = pAmbaIDSP_VinMainReg->Status.UnknownSyncCode;

            /*
             * Release the Mutex
             */
            (void)VIN_MutexGive();
        } else {
            RetVal = VIN_ERR_MUTEX;
        }
    }

    return RetVal;
}

