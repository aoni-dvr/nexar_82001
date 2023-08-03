/**
 *  @file AmbaSpiNOR.c
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
 *  @details NOR Control APIs
 *
 */

#include "AmbaTypes.h"
#include "AmbaDef.h"
#include "AmbaKAL.h"
#include "AmbaMisraFix.h"

#include "AmbaSPINOR.h"
#include "AmbaDMA.h"
#include "AmbaRTSL_SpiNOR_Ctrl.h"
#include "AmbaRTSL_SpiNOR.h"
#include "AmbaSpiNOR_Ctrl.h"
#include "AmbaCSL_SpiNOR.h"
#include "AmbaRTSL_Cache.h"
#include "AmbaRTSL_PLL.h"
#include "AmbaCSL_PLL.h"
#include "AmbaRTSL_GIC.h"
#include "AmbaIOUtility.h"
#include "AmbaWrap.h"

/* Value is in us */
#define BUSY_WAITING_TIME    (25U)

typedef struct {
    AMBA_KAL_EVENT_FLAG_t   EventFlag;  /* Event Flags */
    AMBA_KAL_MUTEX_t        Mutex;      /* Mutex */
    UINT32 NorRxChannel;
    UINT32 NorTxChannel;
    UINT8 Isinit;
    UINT8 BusMode;
} AMBA_NOR_SPI_CTRL_s;

static AMBA_NOR_SPI_CTRL_s AmbaNOR_SPI_Ctrl = {0};    /* NOR Management Structure */
#ifdef CONFIG_SPINOR_DMA_ENABLE
static AMBA_DMA_DESC_s DmaReq = {
    .PrivData  = { [0] = 0, [1] = 0, [2] = 0, [3] = 0 },
    .pSrcAddr  = NULL, /* should be updated on the fly */
    .pDstAddr  = NULL, /* should be updated on the fly */
    .pNextDesc = NULL,
    .pStatus   = NULL,
    .DataSize  = 0,    /* should be updated on the fly */
    .Ctrl      = {
        .StopOnError   = 1U,
        .IrqOnError    = 0,
        .IrqOnDone     = 0,
        .Reserved0     = 0,
        .BusBlockSize  = DMA_BUS_BLOCK_8BYTE,
        .BusDataSize   = 0, /* should be updated on the fly */
        .NoBusAddrInc  = 1U,
        .ReadMem       = 0, /* should be updated on the fly */
        .WriteMem      = 0, /* should be updated on the fly */
        .EndOfChain    = 1U,
        .Reserved1     = 0,
    }
};
#endif

static UINT32 AmbaSpiNOR_FlashCheckToggleBit(UINT8 CheckBit);

#ifdef CONFIG_ENABLE_AMBALINK
extern void AmbaIPC_SpiNORLock(void);
extern void AmbaIPC_SpiNORUnlock(void);
#endif

static void AmbaSPINOR_Lock(void)
{
#ifdef CONFIG_ENABLE_AMBALINK
    AmbaIPC_SpiNORLock();
#endif
    (void)AmbaKAL_MutexTake(&AmbaNOR_SPI_Ctrl.Mutex, KAL_WAIT_FOREVER);
}

static void AmbaSPINOR_Unlock(void)
{
    (void)AmbaKAL_MutexGive(&AmbaNOR_SPI_Ctrl.Mutex);
#ifdef CONFIG_ENABLE_AMBALINK
    /*
     * AmbaRTSL_NorWaitInt() will disable IRQ of Spi-NOR,
     * rollback most important one.
     */
    AmbaCSL_NorEnableDataLenthreachIRQ();

    AmbaIPC_SpiNORUnlock();
#endif
}

static void AmbaSpiNOR_TimerWait(UINT32 Delay_us)
{
    UINT32 CortexFreq, DelayCycle;
#if defined (CONFIG_SOC_CV2FS)|| defined (CONFIG_SOC_CV22FS)
    CortexFreq = AmbaRTSL_PllGetCortex1Clk();
#else
    CortexFreq = AmbaRTSL_PllGetCortexClk();
#endif
    DelayCycle = CortexFreq / 1000000U;
    AmbaDelayCycles(DelayCycle * Delay_us);
}

/*
 *  @RoutineName:: AmbaSpiNOR_SoftReset
 *
 *  @Description::
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
static UINT32 AmbaSpiNOR_ResetEnable(void)
{
    UINT32 ActFlag = 0;

    AmbaRTSL_NorResetEnable();

    return AmbaKAL_EventFlagGet(&AmbaNOR_SPI_Ctrl.EventFlag, NOR_TRANS_DONE_FLAG,
                                0x1U, 0x1U, &ActFlag, NOR_CMD_TIMEOUT_VALUE);
}

/*
 *  @RoutineName:: AmbaSpiNOR_SoftReset
 *
 *  @Description::
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
UINT32 AmbaSpiNOR_SoftReset(void);
UINT32 AmbaSpiNOR_SoftReset(void)
{
    UINT32 RetStatus = OK;
    UINT32 ActFlag;

    if (AmbaRTSL_NorSpiCtrl.pNorDevInfo->ResetEnable != 0U) {
        RetStatus = AmbaSpiNOR_ResetEnable();
    }

    if (RetStatus == OK) {
        AmbaRTSL_NorReset();
        RetStatus = AmbaKAL_EventFlagGet(&AmbaNOR_SPI_Ctrl.EventFlag, NOR_TRANS_DONE_FLAG,
                                         0x1U, 0x1U, &ActFlag, NOR_CMD_TIMEOUT_VALUE);
        /* Wait Trph SPINOR device execute time after Soft Reset */
        AmbaSpiNOR_TimerWait(SPINOR_RESET_TIME);
    }
    return RetStatus;
}

/*
 *  @RoutineName:: AmbaNOR_TranDoneSet
 *
 *  @Description:: Set event flag for NOR CMD line.
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
 */
/* disable NEON registers usage in ISR */
#pragma GCC push_options
#pragma GCC target("general-regs-only")

static void AmbaSPINOR_TransDoneSet(void)
{
    AmbaRTSL_NorSpiStop();

    /* Set Transfer Done Event Flag */
    (void)AmbaKAL_EventFlagSet(&AmbaNOR_SPI_Ctrl.EventFlag, NOR_TRANS_DONE_FLAG);
}
#pragma GCC pop_options

/*
 *  @RoutineName:: AmbaSpiNOR_GetDevInfo
 *
 *  @Description:: get the pointer to current NAND device information
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *      AMBA_NAND_DEV_INFO_s * : the pointer to current NAND device information
 */
AMBA_NORSPI_DEV_INFO_s *AmbaSpiNOR_GetDevInfo(void)
{
    return AmbaRTSL_NorSpiDevInfo;
}
/*
 *  @RoutineName:: AmbaRTSL_NorWriteEnable
 *
 *  @Description::
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
static UINT32 AmbaSpiNOR_WriteEnable(void)
{
    UINT32 ActFlag;

    AmbaRTSL_NorWriteEnableStart();
    return  AmbaKAL_EventFlagGet(&AmbaNOR_SPI_Ctrl.EventFlag, NOR_TRANS_DONE_FLAG,
                                 1U, 1U, &ActFlag, NOR_CMD_TIMEOUT_VALUE);
}

/*
 *  @RoutineName:: AmbaRTSL_NorWriteDisable
 *
 *  @Description::
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
UINT32 AmbaSPINOR_WriteDisable(void)
{
    UINT32 RetStatus;
    UINT32 ActFlag = 0;

    AmbaRTSL_NorWriteDisable();
    RetStatus = AmbaKAL_EventFlagGet(&AmbaNOR_SPI_Ctrl.EventFlag, NOR_TRANS_DONE_FLAG,
                                     0x1U, 0x1U, &ActFlag, NOR_CMD_TIMEOUT_VALUE);

    return RetStatus;
}

/*
 *  @RoutineName:: AmbaNOR_ReadQuadMode
 *
 *  @Description:: Issue the read Quad Mode command to read the Nor Device
 *
 *  @Input      ::
 *
 *  @Output     :: none
 *
 *  @Return     :: INT32 : OK(0)/NG(-1)
 */
static UINT32 AmbaSpiNOR_ReadQuadMode(UINT8 *pStatus)
{
    UINT32 RetStatus;
    UINT32 ActFlag = 0;

    AmbaRTSL_NorReadQuadModeCmd();
    RetStatus = AmbaKAL_EventFlagGet(&AmbaNOR_SPI_Ctrl.EventFlag, NOR_TRANS_DONE_FLAG,
                                     0x1U, 0x1U, &ActFlag, NOR_CMD_TIMEOUT_VALUE);
    if (RetStatus == OK) {
        UINT8 Dummy;
        AmbaRTSL_NorReadFIFO(1U, pStatus);
        AmbaRTSL_NorReadFIFO(1U, &Dummy);
    }
    return RetStatus;
}

/*
 *  @RoutineName::  AmbaSpiNOR_EnterQUAD
 *
 *  @Description::
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
static UINT32 AmbaSpiNOR_EnterQUAD(void)
{
    UINT32 RetStatus = OK;
    UINT32 ActFlag;
    UINT8 DataBuf[2], Status;
    UINT8 CmdArgLen = AmbaRTSL_NorSpiCtrl.pNorDevInfo->Quad.QuadCmdArgLen;

    if (AmbaRTSL_NorSpiCtrl.pNorDevInfo->Quad.EnableQuadMode != 0U) {
        RetStatus = AmbaSpiNOR_WriteEnable();

        if (RetStatus == OK) {
            if (IO_UtilityStringCompare("Spansion", AmbaRTSL_NorSpiCtrl.pNorDevInfo->DevName, 8) == 0) {
                DataBuf[0] = 0x2U;
                DataBuf[1] = AmbaRTSL_NorSpiCtrl.pNorDevInfo->Quad.QuadFlag;
            } else {
                DataBuf[0] = AmbaRTSL_NorSpiCtrl.pNorDevInfo->Quad.QuadFlag;
                DataBuf[1] = 0x27U;
            }
            if (CmdArgLen != 0U) {
                AmbaRTSL_NorEnterQUADStart(CmdArgLen, &DataBuf[0]);
            } else {
                AmbaRTSL_NorEnterQUADStart(0, &DataBuf[0]);
            }
            RetStatus = AmbaKAL_EventFlagGet(&AmbaNOR_SPI_Ctrl.EventFlag, NOR_TRANS_DONE_FLAG,
                                             1U, 1U, &ActFlag, NOR_CMD_TIMEOUT_VALUE);

            if ((RetStatus == OK) &&
                (AmbaRTSL_NorSpiCtrl.pNorDevInfo->Quad.ReadQuadMode != 0U)) {
                do { /* Get Status form NOR device */
                    RetStatus = AmbaSpiNOR_ReadQuadMode(&Status);
                    if (RetStatus != OK) {
                        (void) AmbaSPINOR_ClearStatus();
                        break;
                    }
                    if ((Status & NOR_STATUS_WRTIE_IN_PROCESS) != 0U) {
                        continue;
                    }
                } while ((Status & AmbaRTSL_NorSpiCtrl.pNorDevInfo->Quad.QuadFlag) == 0U);
            }
        }
    }

    if (RetStatus == OK) {
        AmbaNOR_SPI_Ctrl.BusMode = AMBA_NORSPI_DATA_4_LANE;
    }

    return RetStatus;
}

/*
 *  @RoutineName::  AmbaSpiNOR_ExitQUAD
 *
 *  @Description::
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
static UINT32 AmbaSpiNOR_ExitQUAD(void)
{
    UINT32 RetStatus = OK;
    UINT32 ActFlag;
    UINT8 DataBuf[2], Status;
    UINT8 CmdArgLen = AmbaRTSL_NorSpiCtrl.pNorDevInfo->Quad.QuadCmdArgLen;

    if (AmbaRTSL_NorSpiCtrl.pNorDevInfo->Quad.ExitQuadMode != 0U) {
        RetStatus = AmbaSpiNOR_WriteEnable();
        if (RetStatus == OK) {
            if (0 == IO_UtilityStringCompare("Spansion", AmbaRTSL_NorSpiCtrl.pNorDevInfo->DevName, 8)) {
                DataBuf[0] = 0x2U;
                DataBuf[1] = AmbaRTSL_NorSpiCtrl.pNorDevInfo->Quad.ExitQuadFlag;
            } else {
                DataBuf[0] = AmbaRTSL_NorSpiCtrl.pNorDevInfo->Quad.ExitQuadFlag;
                DataBuf[1] = 0x27U;
            }
            if (CmdArgLen != 0U) {
                AmbaRTSL_NorExitQUADStart(CmdArgLen, DataBuf);
            } else {
                AmbaRTSL_NorExitQUADStart(0, DataBuf);
            }

            RetStatus = AmbaKAL_EventFlagGet(&AmbaNOR_SPI_Ctrl.EventFlag, NOR_TRANS_DONE_FLAG,
                                             0x1U, 0x1U, &ActFlag, NOR_CMD_TIMEOUT_VALUE);

            if ((RetStatus == OK) &&
                (AmbaRTSL_NorSpiCtrl.pNorDevInfo->Quad.ReadQuadMode != 0U)) {
                do { /* Get Status form NOR device */
                    RetStatus = AmbaSpiNOR_ReadQuadMode(&Status);
                    if (RetStatus != OK) {
                        (void) AmbaSPINOR_ClearStatus();
                        break;
                    }
                    if ((Status & NOR_STATUS_WRTIE_IN_PROCESS) != 0U) {
                        continue;
                    }
                } while ((Status & AmbaRTSL_NorSpiCtrl.pNorDevInfo->Quad.QuadFlag) != 0U);
            }
        }
    }

    if (RetStatus == OK) {
        AmbaNOR_SPI_Ctrl.BusMode = 0;
    }

    return RetStatus;
}

static UINT32 AmbaSpiNOR_ChkDeviceQuadMode(UINT8 *Mode)
{
    UINT32 RetVal = OK;
    UINT8  Status = 0;

    if (AmbaRTSL_NorSpiCtrl.pNorDevInfo->Quad.ReadQuadMode != 0U) {
        RetVal = AmbaSpiNOR_ReadQuadMode(&Status);
        if (RetVal != OK) {
            (void) AmbaSPINOR_ClearStatus();
        } else {
            if ((Status & AmbaRTSL_NorSpiCtrl.pNorDevInfo->Quad.QuadFlag) != 0U) {
                *Mode = AMBA_NORSPI_DATA_4_LANE;
            } else {
                *Mode = AMBA_NORSPI_DATA_2_LANE;
            }
        }
    } else {
        *Mode = AmbaNOR_SPI_Ctrl.BusMode;
    }

    return RetVal;
}

/*
 *  @RoutineName:: AmbaSPINOR_DeviceBusSwitch
 *
 *  @Description::
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
static UINT32 AmbaSPINOR_DeviceBusSwitch(UINT8 BusMode)
{
    UINT8 CurBusMode;
    UINT32 RetStatus = 0;

    if((AmbaRTSL_NorSpiCtrl.pNorDevInfo->Program.DataLane == AMBA_NORSPI_DATA_4_LANE) ||
       (AmbaRTSL_NorSpiCtrl.pNorDevInfo->Read.DataLane == AMBA_NORSPI_DATA_4_LANE)) {
        RetStatus = AmbaSpiNOR_ChkDeviceQuadMode(&CurBusMode);
        if (RetStatus == OK) {
            if (BusMode == AMBA_NORSPI_DATA_4_LANE) {
                if (CurBusMode != AMBA_NORSPI_DATA_4_LANE) {
                    RetStatus = AmbaSpiNOR_EnterQUAD();
                }
            } else {
                if (CurBusMode == (UINT8)AMBA_NORSPI_DATA_4_LANE) {
                    RetStatus = AmbaSpiNOR_ExitQUAD();
                }
            }
        }
    }
    return RetStatus;
}

/*
 *  @RoutineName:: AmbaRTSL_NorClearStatus
 *
 *  @Description::
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
UINT32 AmbaSPINOR_ClearStatus(void)
{
    UINT32 RetVal = OK;
    UINT32 ActFlag = 0;

    AmbaRTSL_NorClearStatus();
    if (OK != AmbaKAL_EventFlagGet(&AmbaNOR_SPI_Ctrl.EventFlag, NOR_TRANS_DONE_FLAG,
                                   0x1U, 0x1U, &ActFlag, NOR_CMD_TIMEOUT_VALUE)) {
        RetVal = SPINOR_ERR_IO_FAIL;
    }

    return RetVal;
}

/*
 *  @RoutineName:: AmbaRTSL_SpiNOR_SetExtAddr
 *
 *  @Description:: Issue a command to Set the extended address of SpiNOR device
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: INT32 : OK(0)/NG(-1)
 */
static UINT32 AmbaSpiNOR_SetExtAddr(void)
{
    UINT32 RetStatus;
    UINT32 ActFlag = 0;

    RetStatus = AmbaSpiNOR_WriteEnable();
    if (RetStatus == OK) {
        AmbaRTSL_NorSetExtAddr();
        RetStatus = AmbaKAL_EventFlagGet(&AmbaNOR_SPI_Ctrl.EventFlag, NOR_TRANS_DONE_FLAG,
                                         0x1U, 0x1U, &ActFlag, NOR_CMD_TIMEOUT_VALUE);
    }

    return RetStatus;
}

/*
 *  @RoutineName:: AmbaNOR_NandReadStatusCmd
 *
 *  @Description:: Issue the read status command to read the NAND status
 *
 *  @Input      ::
 *          AMBA_NAND_DEV_s *pDev     : Pointer to nand device structure
 *
 *  @Output     :: none
 *
 *  @Return     :: INT32 : OK(0)/NG(-1)
 */
static UINT32 AmbaSpiNOR_ReadStatus(UINT8 StatusNum, UINT8 *Status)
{
    UINT32 RetStatus, ActFlag = 0;
    UINT8 Dummy;

    AmbaRTSL_NorReadStatusCmd(StatusNum);
    RetStatus = AmbaKAL_EventFlagGet(&AmbaNOR_SPI_Ctrl.EventFlag, NOR_TRANS_DONE_FLAG,
                                     0x1U, 0x1U, &ActFlag, NOR_CMD_TIMEOUT_VALUE);
    if (RetStatus == OK) {
        AmbaRTSL_NorReadFIFO(1U, Status);
        AmbaRTSL_NorReadFIFO(1U, &Dummy);
    }
    return RetStatus;
}

/*
 *  @RoutineName:: AmbaSpiNOR_SectorErase
 *
 *  @Description:: To Erase the necessary Sector
 *
 *  @Input      ::
 *      Offset:    starting offset (relative to base addr)
 *      ByteCount: Number of bytes to be erased
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
UINT32 AmbaSpiNOR_SectorErase(UINT32 Offset, UINT32 ByteCount, UINT32 TimeOut)
{
    UINT32 EraseSize = AmbaRTSL_NorSpiCtrl.pNorDevInfo->EraseSectorSize;
    UINT32 BlockStartOffset, BlockEndOffset, NumBlocks;
    UINT32 ActFlag = 0, RetStatus = 0, EraseDone = 0;

    UINT32 CurOffset = Offset, EndOffset = Offset + ByteCount;

    if ((EraseSize == 0U) || (AmbaRTSL_NorSpiCtrl.TotalByteSize == 0U) || (ByteCount == 0U)) {
        RetStatus = SPINOR_ERR_ARG;
    } else {
        /* Take the Mutex */
        AmbaSPINOR_Lock();

        /* Erase the necessary Sectors */
        BlockStartOffset = 0;
        NumBlocks = AmbaRTSL_NorSpiCtrl.TotalByteSize / EraseSize;
        for (UINT32 j = 0; j < NumBlocks; j++) {

            RetStatus = AmbaSpiNOR_WriteEnable();

            if (RetStatus == OK) {
                BlockEndOffset = BlockStartOffset + EraseSize;
                if (BlockStartOffset >= EndOffset) {
                    EraseDone = 1U;   /* Automatic Erase is complete. */
                } else {
                    if (BlockEndOffset > CurOffset) {
                        AmbaRTSL_NorSectorErase(BlockStartOffset);
                        RetStatus = AmbaKAL_EventFlagGet(&AmbaNOR_SPI_Ctrl.EventFlag, NOR_TRANS_DONE_FLAG,
                                                         0x1U, 0x1U, &ActFlag, TimeOut);
                        if (RetStatus == OK) {
                            RetStatus = AmbaSpiNOR_FlashCheckToggleBit(AmbaRTSL_NorSpiCtrl.pNorDevInfo->EraseFailFlag);
                        }
                    }
                }

                if (((RetStatus != OK)) || (EraseDone == 1U)) {
                    break;
                }

                BlockStartOffset = BlockEndOffset;
            }
        }
        /* Release the Mutex */
        AmbaSPINOR_Unlock();
    }


    return RetStatus;
}

/*
 *  @RoutineName:: AmbaSpiNOR_Erase
 *
 *  @Description:: To Erase the necessary Blocks
 *
 *  @Input      ::
 *      Offset:    starting offset (relative to base addr)
 *      ByteCount: Number of bytes to be erased
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
UINT32 AmbaSpiNOR_Erase(UINT32 Offset, UINT32 ByteCount, UINT32 TimeOut)
{
    UINT32 EraseSize = AmbaRTSL_NorSpiCtrl.pNorDevInfo->EraseBlockSize;
    UINT32 RetStatus = 0, ActFlag = 0x0;

    UINT32 CurOffset, EndOffset;

    CurOffset = Offset;
    EndOffset = Offset + ByteCount;

    if ((EraseSize == 0U) || (AmbaRTSL_NorSpiCtrl.TotalByteSize == 0U) || (ByteCount == 0U)) {
        RetStatus = SPINOR_ERR_ARG;
    } else {

        /* Take the Mutex */
        AmbaSPINOR_Lock();

        if ((Offset == 0U) && (ByteCount >= AmbaRTSL_NorSpiCtrl.TotalByteSize)) {
            RetStatus = AmbaSpiNOR_WriteEnable();
            if (RetStatus == OK) {
                AmbaRTSL_NorEraseChip();
                RetStatus = AmbaKAL_EventFlagGet(&AmbaNOR_SPI_Ctrl.EventFlag, NOR_TRANS_DONE_FLAG,
                                                 0x1U, 0x1U, &ActFlag, TimeOut);
            }

            if (RetStatus == OK) {
                RetStatus = AmbaSpiNOR_FlashCheckToggleBit(AmbaRTSL_NorSpiCtrl.pNorDevInfo->EraseFailFlag);
            }
        } else {
            /* Erase the necessary Blocks */
            UINT32 BlockStartOffset = 0, BlockEndOffset, EraseDone = 0;
            UINT32 NumBlocks = AmbaRTSL_NorSpiCtrl.TotalByteSize / EraseSize;

            for (UINT32 j = 0; j < NumBlocks; j++) {
                BlockEndOffset = BlockStartOffset + EraseSize;
                if (BlockStartOffset >= EndOffset) {
                    EraseDone = 1U;   /* Automatic Erase is complete. */
                } else {
                    if (BlockEndOffset > CurOffset) {
                        RetStatus = AmbaSpiNOR_WriteEnable();
                        if (RetStatus == OK) {
                            AmbaRTSL_NorEraseBlock(BlockStartOffset);
                            RetStatus = AmbaKAL_EventFlagGet(&AmbaNOR_SPI_Ctrl.EventFlag, NOR_TRANS_DONE_FLAG,
                                                             0x1U, 0x1U, &ActFlag, TimeOut);
                        }

                        if (RetStatus == OK) {
                            RetStatus = AmbaSpiNOR_FlashCheckToggleBit(AmbaRTSL_NorSpiCtrl.pNorDevInfo->EraseFailFlag);
                        }
                    }
                    BlockStartOffset = BlockEndOffset;
                }

                if (((RetStatus != OK)) || (EraseDone == 1U)) {
                    break;
                }

            }
        }
        /* Release the Mutex */
        AmbaSPINOR_Unlock();
    }


    return RetStatus;
}

#ifdef CONFIG_SPINOR_DMA_ENABLE
static void AmbaNor_SpiReadDMA(UINT32 From, UINT32 ByteCount, UINT8 *pDataBuf)
{
    ULONG SrcAddr = 0;
    void *pSrc = NULL;

    (void)AmbaRTSL_CacheFlushDataPtr(pDataBuf, ByteCount);

    AmbaMisra_TypeCast(&SrcAddr, (const void *)&pAmbaNOR_Reg);
    SrcAddr += AMBA_NORSPI_RXBUF_OFFSET;

    /* start DMA data transfer */
    /* DmaReq.pSrcAddr = (void *) & (pAmbaNOR_Reg->RxData[0]); */
    AmbaMisra_TypeCast(&pSrc, &SrcAddr);
    DmaReq.pSrcAddr = pSrc;

    DmaReq.pDstAddr = pDataBuf;
    DmaReq.DataSize = ByteCount;

    DmaReq.Ctrl.StopOnError  = 1U;
    DmaReq.Ctrl.IrqOnError   = 0U;
    DmaReq.Ctrl.IrqOnDone    = 0U;
    DmaReq.Ctrl.BusBlockSize = DMA_BUS_BLOCK_32BYTE;
    DmaReq.Ctrl.BusDataSize  = DMA_BUS_DATA_4BYTE;
    DmaReq.Ctrl.NoBusAddrInc = 1U;
    DmaReq.Ctrl.ReadMem      = 0U;
    DmaReq.Ctrl.WriteMem     = 1U;
    DmaReq.Ctrl.EndOfChain   = 1U;

    (void)AmbaDMA_Transfer(AmbaNOR_SPI_Ctrl.NorRxChannel, &DmaReq);

    AmbaRTSL_NorSpiReadSetup(From, ByteCount, 1U);
}
#endif

/*
 *  @RoutineName:: AmbaNOR_SpiRead
 *
 *  @Description:: Read Nor flash memory
 *
 *  @Input      ::
 *          UINT32 From           : The Address of SpiNOR device
 *          UINT32 ByteCount      : Length of read Data
 *          UINT8 *pDataBuf       : The Point to the destination buf
 *          UINT32 TimeOut        : TimeOut value
 *
 *  @Output     :: none
 *
 *  @Return     :: INT32 : OK(0)/NG(-1)
 */
static UINT32 AmbaNOR_SpiRead(UINT32 From, UINT32 ByteCount, UINT8 *pDataBuf, UINT32 TimeOut)
{
    UINT32 ActFlag = 0x0;
    UINT32 RetStatus  = 0;
    ULONG MainAddr = 0x0;
    UINT8 *pWorkBuf;
    UINT32 ReadByteCount = ByteCount;
    ULONG Misalign = 0;

    AmbaMisra_TypeCast(&MainAddr, &pDataBuf);
    /* If buf align to 64 bit , use SPI-DMA transfer */
    Misalign = (MainAddr & (CACHE_LINE_SIZE - 1U));

#if (CONFIG_DDR_SIZE > 0x100000000UL)
    if ((MainAddr >> 32U) != 0U) {
        Misalign = 1U;
    }
#endif
    if (Misalign != 0U) {
        pWorkBuf = AmbaRTSL_NorSpiCtrl.pBuf;
    } else {
        pWorkBuf = pDataBuf;
    }

    if (ReadByteCount > AMBA_SPINOR_DMA_BUF_SIZE) {
        RetStatus = SPINOR_ERR_ARG;
    } else {
#ifdef CONFIG_SPINOR_DMA_ENABLE
        UINT32 AlignSize = 64U;
        UINT32 Tail = ReadByteCount % AlignSize;
        UINT32 Bulk = ReadByteCount / AlignSize;

        if (Bulk >= 1U) {
            AmbaNor_SpiReadDMA(From, ReadByteCount - Tail, pWorkBuf);
            RetStatus = AmbaKAL_EventFlagGet(&AmbaNOR_SPI_Ctrl.EventFlag, NOR_TRANS_DONE_FLAG,
                                             0x1U, 0x1U, &ActFlag, TimeOut);
            if (RetStatus == OK) {
                /* Wait fo DMA operation done */
                RetStatus = AmbaDMA_Wait(AmbaNOR_SPI_Ctrl.NorRxChannel, TimeOut);
                if (RetStatus == OK) {
                    AmbaCSL_NorDisableRxDMA();
                    AmbaSpiNOR_TimerWait(BUSY_WAITING_TIME);
                    (void)AmbaRTSL_CacheInvalDataPtr(pWorkBuf, ReadByteCount - Tail);
                }
            }
        }

        if ((RetStatus == OK) && (Misalign != 0U)) {
            if (OK != AmbaWrap_memcpy(pDataBuf, pWorkBuf, (ULONG)ReadByteCount - Tail)) { /* Misrac */ };
        }

        if ((RetStatus == OK) && (Tail != 0U)) {
            AmbaRTSL_NorSpiReadSetup(From + (Bulk * AlignSize), Tail, 0);
            RetStatus = AmbaKAL_EventFlagGet(&AmbaNOR_SPI_Ctrl.EventFlag, NOR_TRANS_DONE_FLAG,
                                             0x1U, 0x1U, &ActFlag, TimeOut);
            if (RetStatus == OK) {
                AmbaSpiNOR_TimerWait(BUSY_WAITING_TIME);

                if (AmbaCSL_NorGetRxFifoDataSize() != Tail) {
                    RetStatus = SPINOR_ERR_IO_FAIL;
                } else {
                    AmbaRTSL_NorReadFIFO(Tail, &pWorkBuf[Bulk * AlignSize]);
                }

                if ((RetStatus == OK) && (Misalign != 0U)) {
                    if (OK != AmbaWrap_memcpy(&pDataBuf[Bulk * AlignSize], &pWorkBuf[Bulk * AlignSize], Tail)) { /* Misrac */ };
                }
            }
        }
#else
        UINT32 Size = ReadByteCount;
        UINT32 i = 0U, FifoSize = AMBA_SPINOR_FIFO_SIZE;

        if (ReadByteCount >= FifoSize) {
            i = 0;
            do {
                AmbaRTSL_NorSpiReadSetup(From + (FifoSize * i), FifoSize, 0);
                RetStatus = AmbaKAL_EventFlagGet(&AmbaNOR_SPI_Ctrl.EventFlag, NOR_TRANS_DONE_FLAG,
                                                 0x1U, 0x1U, &ActFlag, TimeOut);
                AmbaSpiNOR_TimerWait(BUSY_WAITING_TIME);
                while (FifoSize != AmbaCSL_NorGetRxFifoDataSize()) {
                    AmbaDelayCycles(0xfU);
                }

                AmbaRTSL_NorReadFIFO(FifoSize,  &pWorkBuf[i * FifoSize]);

                if ((RetStatus == OK) && (Misalign != 0U)) {
                    if (OK != AmbaWrap_memcpy(&pDataBuf[i * FifoSize], &pWorkBuf[i * FifoSize], FifoSize)) { /* Misrac */ };
                }
                ReadByteCount -= FifoSize;
                i++;
            } while (ReadByteCount >= FifoSize);
        }

        if (ReadByteCount != 0U) {
            AmbaRTSL_NorSpiReadSetup(From + (FifoSize * i), ReadByteCount, 0);
            RetStatus = AmbaKAL_EventFlagGet(&AmbaNOR_SPI_Ctrl.EventFlag, NOR_TRANS_DONE_FLAG,
                                             0x1U, 0x1U, &ActFlag, TimeOut);

            AmbaSpiNOR_TimerWait(BUSY_WAITING_TIME);
            while (ReadByteCount != AmbaCSL_NorGetRxFifoDataSize()) {
                AmbaDelayCycles(0xfU);
            }

            AmbaRTSL_NorReadFIFO(ReadByteCount,  &pWorkBuf[i * FifoSize]);

            if ((RetStatus == OK) && (Misalign != 0U)) {
                if (OK != AmbaWrap_memcpy(&pDataBuf[i * FifoSize], &pWorkBuf[i * FifoSize], ReadByteCount)) { /* Misrac */ };
            }
        }
        (void)AmbaRTSL_CacheFlushDataPtr(pDataBuf, Size);
#endif
    }
    return RetStatus;
}

/*
 *  @RoutineName:: AmbaSpiNOR_ReadByte
 *
 *  @Description:: Read Nor flash memory
 *
 *  @Input      ::
 *          UINT32 Offset             : The address of SpiNOR device
 *          UINT32 ByteCount          : Length data
 *          UINT8 *pDataBuf           : Pointer to the destination address of Data
 *          UINT32 TimeOut            : TimeOut value
 *
 *  @Output     :: none
 *
 *  @Return     :: INT32 : OK(0)/NG(-1)
 */
UINT32 AmbaSpiNOR_Readbyte(UINT32 Offset, UINT32 ByteCount, UINT8 *pDataBuf, UINT32 TimeOut)
{
    UINT32 RetStatus;
    UINT32 pCount, pOffset;

    /* Take the Mutex */
    AmbaSPINOR_Lock();

    RetStatus = AmbaSPINOR_DeviceBusSwitch(AmbaRTSL_NorSpiCtrl.pNorDevInfo->Read.DataLane);
    if (RetStatus == OK) {
        pOffset = 0;
        while (pOffset < ByteCount) {
            pCount = ByteCount - pOffset;
            if (pCount >= AMBA_SPINOR_DMA_BUF_SIZE) {
                RetStatus = AmbaNOR_SpiRead(Offset + pOffset, AMBA_SPINOR_DMA_BUF_SIZE, &pDataBuf[pOffset], TimeOut);
                if (RetStatus == OK) {
                    pOffset += AMBA_SPINOR_DMA_BUF_SIZE;
                }
            } else {
                RetStatus = AmbaNOR_SpiRead(Offset + pOffset, pCount, &pDataBuf[pOffset], TimeOut);
                if (RetStatus == OK) {
                    pOffset += pCount;
                }
            }
            if (RetStatus != OK) {
                break;
            }
        }
    }

    /* Release the Mutex */
    AmbaSPINOR_Unlock();

    return RetStatus;
}

#ifdef CONFIG_SPINOR_DMA_ENABLE
static void AmbaNor_SpiWriteDMA(UINT32 Offset, UINT32 ByteCount, UINT8 *pDataBuf)
{
    ULONG DstAddr = 0;
    void *pDst = NULL;

    /* Clear the remain Data on TxFIFO  */
    if (AmbaCSL_NorGetTxFifoEmpty() == 0U) {
        AmbaRTSL_ResetFIFO();
    }

    (void)AmbaRTSL_CacheCleanDataPtr(pDataBuf, ByteCount);

    AmbaMisra_TypeCast(&DstAddr, (const void *)&pAmbaNOR_Reg);
    DstAddr += AMBA_NORSPI_TXBUF_OFFSET;

    /* start DMA data transfer */
    AmbaMisra_TypeCast(&pDst, &DstAddr); /* DmaReq.pDstAddr = (void *) &(pAmbaNOR_Reg->TxData[0]); */
    DmaReq.pDstAddr = pDst;

    DmaReq.pSrcAddr = pDataBuf;
    DmaReq.DataSize = ByteCount;

    DmaReq.Ctrl.StopOnError  = 1U;
    DmaReq.Ctrl.IrqOnError   = 0U;
    DmaReq.Ctrl.IrqOnDone    = 0U;
    DmaReq.Ctrl.BusBlockSize = DMA_BUS_BLOCK_32BYTE;
    DmaReq.Ctrl.BusDataSize  = DMA_BUS_DATA_4BYTE;
    DmaReq.Ctrl.NoBusAddrInc = 1U;
    DmaReq.Ctrl.ReadMem      = 1U;
    DmaReq.Ctrl.WriteMem     = 0U;
    DmaReq.Ctrl.EndOfChain   = 1U;

    (void) AmbaDMA_Transfer(AmbaNOR_SPI_Ctrl.NorTxChannel, &DmaReq);

    AmbaRTSL_NorSpiWriteSetup(Offset, ByteCount, pDataBuf, 1U);
}
#endif

/*
 *  @RoutineName:: AmbaNOR_SpiWrite
 *
 *  @Description:: Program Nor flash memory
 *
 *  @Input      ::
 *          UINT32 Offset             : Block to be programmed
 *          UINT32 ByteCount          : Size of Data in Byte
 *          UINT8 *pDataBuf           : Pointer to the destination address of Data
 *          UINT32 TimeOut            : TimeOut value
 *
 *  @Output     :: none
 *
 *  @Return     :: INT32 : OK(0)/NG(-1)*
 */
static UINT32 AmbaNOR_SpiWrite(UINT32 Offset, UINT32 ByteCount, const UINT8 *pDataBuf, UINT32 TimeOut)
{
#ifdef CONFIG_SPINOR_DMA_ENABLE
    UINT32 Tail = 0, Bulk = 0, Remain, AlignSize = 64U;
    UINT8 *pWorkBuf;
#else
    UINT32 i = 0, FifoSize = AMBA_SPINOR_FIFO_SIZE;
    const UINT8 *pWorkBuf;
#endif
    UINT32 ActFlag   = 0;
    UINT32 RetStatus;
    ULONG MainAddr;
    UINT32 WriteByteCount = ByteCount;
    ULONG Misalign = 0;

    if (ByteCount > AMBA_SPINOR_DMA_BUF_SIZE) {
        RetStatus = SPINOR_ERR_ARG;
    } else {

        if (OK != AmbaSpiNOR_WriteEnable()) {
            RetStatus = SPINOR_ERR_IO_FAIL;
        } else {

            AmbaMisra_TypeCast(&MainAddr, &pDataBuf);

            do { /* Get Status form NOR device */
                RetStatus = AmbaSpiNOR_ReadStatus(1U, (UINT8 *)&AmbaRTSL_NorSpiCtrl.Status[0]);
                if (RetStatus != OK) {
                    RetStatus = AmbaSPINOR_ClearStatus();
                    break;
                }
                if (AmbaRTSL_NorSpiCtrl.Status[0].Bits[0].WriteInProgress == 1U) {
                    continue;
                }
            } while (!(AmbaRTSL_NorSpiCtrl.Status[0].Bits[0].WriteEnableLatch == 1U));

            if (RetStatus == OK) {

                /* If buf align to 32 bit , use SPI-DMA transfer */
                Misalign = (MainAddr & (CACHE_LINE_SIZE - 1U));
#if (CONFIG_DDR_SIZE > 0x100000000UL)
                if ((MainAddr >> 32U) != 0U) {
                    Misalign = 1U;
                }
#endif
                if (Misalign != 0U)  {
                    if (OK != AmbaWrap_memcpy(AmbaRTSL_NorSpiCtrl.pBuf, pDataBuf, WriteByteCount)) { /* Misrac */ };
                    pWorkBuf = AmbaRTSL_NorSpiCtrl.pBuf;
                } else {
                    AmbaMisra_TypeCast(&pWorkBuf, &pDataBuf);
                }
#ifdef CONFIG_SPINOR_DMA_ENABLE
                Tail = WriteByteCount % AlignSize;
                Bulk = WriteByteCount / AlignSize;

                if (Bulk >= 1U) {
                    AmbaNor_SpiWriteDMA(Offset, WriteByteCount - Tail, pWorkBuf);

                    /* Wait fo DMA operation done */
                    RetStatus = AmbaDMA_Wait(AmbaNOR_SPI_Ctrl.NorTxChannel, TimeOut);
                    if (RetStatus == OK) {
                        RetStatus = AmbaKAL_EventFlagGet(&AmbaNOR_SPI_Ctrl.EventFlag,
                                                         NOR_TRANS_DONE_FLAG, 1U, 1U, &ActFlag, TimeOut);
                        if (RetStatus == OK) {
                            AmbaSpiNOR_TimerWait(BUSY_WAITING_TIME);
                            AmbaCSL_NorDisableTxDMA();
                            (void)AmbaRTSL_CacheInvalDataPtr(pWorkBuf, WriteByteCount - Tail);
                        }
                    }
                }
                if ((RetStatus == OK) && (Tail != 0U)) {
                    RetStatus = AmbaSpiNOR_WriteEnable();
                    if (RetStatus == OK) {
                        AmbaRTSL_NorSpiWriteSetup(Offset + (Bulk * AlignSize), Tail,  &pWorkBuf[Bulk * AlignSize], 0);
                        RetStatus = AmbaKAL_EventFlagGet(&AmbaNOR_SPI_Ctrl.EventFlag, NOR_TRANS_DONE_FLAG,
                                                         1U, 1U, &ActFlag, TimeOut);
                    }
                }
                if (RetStatus == OK) {
                    do {
                        Remain = AmbaCSL_NorGetTxFifoDataSize();
                        if (Remain != 0U) {
                            AmbaSpiNOR_TimerWait(1U);
                        }
                    } while (Remain != 0U);
                    RetStatus = AmbaSpiNOR_FlashCheckToggleBit(AmbaRTSL_NorSpiCtrl.pNorDevInfo->ProgramFailFlag);
                }
#else
                while (WriteByteCount > FifoSize) {
                    AmbaRTSL_NorSpiWriteSetup(Offset + (i * FifoSize), FifoSize,  &pWorkBuf[i * FifoSize], 0);

                    RetStatus = AmbaKAL_EventFlagGet(&AmbaNOR_SPI_Ctrl.EventFlag, NOR_TRANS_DONE_FLAG,
                                                     1U, 1U, &ActFlag, TimeOut);
                    if (RetStatus != OK) {
                        break;
                    }
                    while (0U != AmbaCSL_NorGetTxFifoDataSize()) {
                        AmbaSpiNOR_TimerWait(1U);
                    }
                    i++;
                    WriteByteCount -= FifoSize;
                }

                if ((WriteByteCount != 0U) && (RetStatus == OK)) {
                    AmbaRTSL_NorSpiWriteSetup(Offset + (i * FifoSize), WriteByteCount,  &pWorkBuf[i * FifoSize], 0);
                    RetStatus = AmbaKAL_EventFlagGet(&AmbaNOR_SPI_Ctrl.EventFlag, NOR_TRANS_DONE_FLAG,
                                                     1U, 1U, &ActFlag, TimeOut);
                    if (RetStatus == OK) {
                        while (0U != AmbaCSL_NorGetTxFifoDataSize()) {
                            AmbaSpiNOR_TimerWait(1U);
                        }
                    }
                }
                RetStatus = AmbaSpiNOR_FlashCheckToggleBit(AmbaRTSL_NorSpiCtrl.pNorDevInfo->ProgramFailFlag);
#endif
            }
        }
    }
    return RetStatus;
}

/*
 *  @RoutineName:: AmbaNorSpi_Program
 *
 *  @Description:: Program Nor flash memory
 *
 *  @Input      ::
 *          UINT32 Offset        : Block to be programmed
 *          UINT32 ByteCount     : Size of Data in Byte
 *          UINT8 *pDataBuf      : Pointer to the source data
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
UINT32 AmbaSpiNOR_Program(UINT32 Offset, UINT32 ByteCount, const UINT8 *pDataBuf, UINT32 TimeOut)
{
    UINT32 Pages, FirstPageSize, LastPageSize, PageOffset, i;
    UINT32 PageSize = AmbaRTSL_NorSpiCtrl.ProgramPageSize;
    UINT32 RetStatus = 0;
    UINT32 DataIdx = 0;

    UINT32 WriteOffset = Offset;

    if (PageSize == 0x0U) {
        RetStatus = SPINOR_ERR_ARG;
    } else {

        /* Take the Mutex */
        AmbaSPINOR_Lock();

        RetStatus = AmbaSPINOR_DeviceBusSwitch(AmbaRTSL_NorSpiCtrl.pNorDevInfo->Program.DataLane);

        if (RetStatus == OK) {

            PageOffset = Offset & (PageSize - 1U);
            if (PageOffset == 0U) {
                FirstPageSize = 0;
            } else {
                FirstPageSize = PageSize - PageOffset;
            }

            if (ByteCount >= FirstPageSize) {
                Pages = (ByteCount - FirstPageSize) / PageSize;
                LastPageSize = (ByteCount - FirstPageSize) % PageSize;
            } else {
                Pages = 0;
                LastPageSize = 0;
                FirstPageSize = ByteCount;
            }

            if (FirstPageSize != 0U) {
                RetStatus = AmbaNOR_SpiWrite(WriteOffset, FirstPageSize, &pDataBuf[DataIdx], TimeOut);
                if (RetStatus == SPINOR_ERR_NONE) {
                    WriteOffset += FirstPageSize;
                    DataIdx += FirstPageSize;
                }
            }

            if (RetStatus == SPINOR_ERR_NONE) {

                for(i = 0; i < Pages; i++) {
                    RetStatus = AmbaNOR_SpiWrite(WriteOffset, PageSize, &pDataBuf[DataIdx], TimeOut);
                    if (RetStatus != SPINOR_ERR_NONE) {
                        break;
                    }

                    WriteOffset += PageSize;
                    DataIdx += PageSize;
                }
            }

            if (RetStatus == SPINOR_ERR_NONE) {
                if (LastPageSize != 0U) {
                    RetStatus = AmbaNOR_SpiWrite(WriteOffset, LastPageSize, &pDataBuf[DataIdx], TimeOut);
                }
            }

        }
        /* Release the Mutex */
        AmbaSPINOR_Unlock();
    }
    return RetStatus;
}

/*
 *  @RoutineName:: AmbaSpINOR_GetDeviceID
 *
 *  @Description:: Get the Maker ID and Device ID
 *
 *  @Input      ::
 *      pManufactuerID: pointer to the Manufactuer ID
 *      pDeviceID:      pointer to the Device ID (3 words)
 *
 *  @Output     ::
 *      pManufactuerID: pointer to the Manufactuer ID
 *      pDeviceID:      pointer to the Device ID (3 words)
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
static UINT32 AmbaSPINOR_GetDeviceID(void)
{
    UINT32 RetStatus;
    UINT32 ActFlag = 0;

    AmbaRTSL_NorReadIDCmd();

    RetStatus = AmbaKAL_EventFlagGet(&AmbaNOR_SPI_Ctrl.EventFlag, NOR_TRANS_DONE_FLAG,
                                     0x1U, 0x1U, &ActFlag, NOR_CMD_TIMEOUT_VALUE);
    if(RetStatus == OK) {
        AmbaRTSL_NorReadFIFO(1U, &AmbaRTSL_NorSpiCtrl.ManufactuerID);
        AmbaRTSL_NorReadFIFO(1U, &AmbaRTSL_NorSpiCtrl.DeviceID);
    } else {
        RetStatus = SPINOR_ERR_IO_FAIL;
    }

    return RetStatus;
}

/*
 *  @RoutineName:: AmbaSpiNOR_ReadFlagStatus
 *
 *  @Description:: Issue the read status command to read the NAND status
 *
 *  @Input      :: UINT8 *Status
 *
 *  @Output     :: none
 *
 *  @Return     :: INT32 : OK(0)/NG(-1)
 */
static UINT32 AmbaSpiNOR_ReadFlagStatus(UINT8 *Status)
{
    UINT32 RetStatus  = 0;
    UINT32 ActFlag;
    UINT8 Dummy;

    AmbaRTSL_NorReadFlagStatusCmd();
    RetStatus = AmbaKAL_EventFlagGet(&AmbaNOR_SPI_Ctrl.EventFlag, NOR_TRANS_DONE_FLAG,
                                     0x1U, 0x1U, &ActFlag, NOR_CMD_TIMEOUT_VALUE);
    if (RetStatus == OK) {
        AmbaRTSL_NorReadFIFO(1U, Status);
        AmbaRTSL_NorReadFIFO(1U, &Dummy);
    }

    return RetStatus;
}

/*
 *  @RoutineName:: AmbaSpiNOR_FlashCheckToggleBit
 *
 *  @Description:: Check the Toggle Bit for Program/Erase Operation
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
static UINT32 AmbaSpiNOR_FlashCheckToggleBit(UINT8 CheckBit)
{
    UINT8 PollStatus;
    UINT32 RetStatus;

    do {
        /* Get Status form NOR device */
        RetStatus = AmbaSpiNOR_ReadStatus(1U, &PollStatus);
        if (RetStatus != OK) {
            (void)AmbaSPINOR_ClearStatus();
            break;
        }

        if (NOR_STATUS_WRTIE_IN_PROCESS == CheckBits(PollStatus, NOR_STATUS_WRTIE_IN_PROCESS)) {
            AmbaSpiNOR_TimerWait(1U);
            continue;
        }
    } while (NOR_STATUS_WRTIE_IN_PROCESS == CheckBits(PollStatus, NOR_STATUS_WRTIE_IN_PROCESS));

    RetStatus = AmbaSpiNOR_ReadFlagStatus(&PollStatus);
    if (RetStatus != OK) {
        RetStatus = SPINOR_ERR_IO_FAIL;
    } else if (0x0U != CheckBits(PollStatus, CheckBit)) {
        RetStatus = SPINOR_ERR_IO_FAIL;
    } else {
        RetStatus = SPINOR_ERR_NONE;
    }

    return RetStatus;
}

/*
 *  @RoutineName:: AmbaSPINOR_Init
 *
 *  @Description:: NOR Flash memory s/w driver init
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
static UINT32 AmbaSPINOR_Init(void)
{
    UINT32 RetVal;
    UINT32 DmaTxChan = 0, DmaRxChan = 0;

    /* Create a EventFlag */
    if (OK != AmbaKAL_EventFlagCreate(&AmbaNOR_SPI_Ctrl.EventFlag, NULL)) {
        RetVal = SPINOR_ERR_OS_API_FAIL;
    } else if (OK !=  AmbaKAL_MutexCreate(&AmbaNOR_SPI_Ctrl.Mutex, NULL)) {
        RetVal = SPINOR_ERR_OS_API_FAIL;
    } else {
        /* Register FIO command/DMA done call back functions. */
        AmbaRTSL_NORIsrDoneCallBack = AmbaSPINOR_TransDoneSet;

        AmbaRTSL_NorInit();

        /* Set DMA channel to SpiNOR RX and TX */
        RetVal = AmbaDMA_ChannelAllocate(AMBA_DMA_CHANNEL_NOR_SPI_TX, &DmaTxChan);

        if (RetVal == OK) {
            RetVal = AmbaDMA_ChannelAllocate(AMBA_DMA_CHANNEL_NOR_SPI_RX, &DmaRxChan);
        }

        if (RetVal == OK) {
            AmbaNOR_SPI_Ctrl.NorTxChannel = DmaTxChan;
            AmbaNOR_SPI_Ctrl.NorRxChannel = DmaRxChan;
            AmbaNOR_SPI_Ctrl.Isinit = 1U;
        }
    }

    return RetVal;
}

#if defined (CONFIG_SOC_CV5)|| defined (CONFIG_SOC_CV52)|| defined (CONFIG_SOC_CV2FS)|| defined (CONFIG_SOC_CV22FS)
#ifndef CONFIG_AMBALINK_RPMSG_G1
static UINT32 AmbaSPINOR_WriteVcr(UINT32 RegAddr, const UINT8 *pDataBuf)
{
    UINT32 RetStatus;
    UINT32 ActFlag;
    void AmbaRTSL_NorWriteVcrSetup(UINT32 RegAddr, const UINT8 * pDataBuf);

    (void)AmbaSpiNOR_WriteEnable();

    AmbaRTSL_NorWriteVcrSetup(RegAddr, pDataBuf);

    RetStatus = AmbaKAL_EventFlagGet(&AmbaNOR_SPI_Ctrl.EventFlag, NOR_TRANS_DONE_FLAG,
                                     1U, 1U, &ActFlag, 5000U);
    return RetStatus;
}

static void AmbaSPINOR_Setup8bitDevice(void)
{
    UINT8 Status;
#ifndef CONFIG_SPINOR_DEFAULT_MODE_OCT
    AmbaRTSL_NorSpiCtrl.pNorDevInfo->Read.DataLane = 0;
#endif
    (void) AmbaSpiNOR_SetExtAddr();

    //AmbaRTSL_NorReadReg(0x0, 0x8U, &Status);

    Status = 0xE7U;
    (void) AmbaSPINOR_WriteVcr(0x0, &Status);

    AmbaRTSL_NorSpiCtrl.pNorDevInfo->Read.DataLane = AMBA_NORSPI_DATA_8_LANE;

    //AmbaRTSL_NorReadReg(0x0, 0x8U, &Status);
}
#endif
#endif /* (CV2FS) || (CV5) */

/*
 *  @RoutineName:: AmbaSpiNOR_Config
 *
 *  @Description:: NOR Flash memory s/w driver init
 *
 *  @Input      :: pointer to SpiNOR software configurations
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
UINT32 AmbaSpiNOR_Config(AMBA_NOR_SPI_CONFIG_s *pNorSpiConfig)
{
    UINT32 RetVal;

#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
    (void) AmbaCSL_NorSmErrorDisable(); /* Disable SM error detection before program nor_spi controller */
#endif
    RetVal = AmbaSPINOR_Init();

    if (RetVal == OK) {
        RetVal = AmbaRTSL_NorSpiConfig(pNorSpiConfig);
    }

    if (RetVal == OK) {
        RetVal = AmbaSpiNOR_SoftReset();   /* Reset Nor Flash Device*/
    }
#if defined (CONFIG_SOC_CV5)|| defined (CONFIG_SOC_CV52)|| defined (CONFIG_SOC_CV2FS)|| defined (CONFIG_SOC_CV22FS)
#ifndef CONFIG_AMBALINK_RPMSG_G1
    if (AmbaRTSL_NorSpiCtrl.pNorDevInfo->Read.DataLane == AMBA_NORSPI_DATA_8_LANE) {
        AmbaSPINOR_Setup8bitDevice();
    }
#endif
#endif /* (CV2FS) || (CV5) */
    if (RetVal == OK) {
        RetVal = AmbaSPINOR_GetDeviceID(); /* Get the Maker ID and Device ID */
    }

    if ((RetVal == OK) &&
        (pNorSpiConfig->pNorSpiDevInfo->AddrByte == AMBA_NORSPI_4BYTE_ADDR)) {
        RetVal = AmbaSpiNOR_SetExtAddr();
    }

    if (RetVal == OK) {
        RetVal = AmbaSPINOR_InitUserPartLock();
    }

    if (RetVal == OK) {
        RetVal = AmbaSpiNOR_InitPtbBbt(1000U); /* Init System/User Partition Tables */
    }

    if (RetVal == OK) {
#if defined(CONFIG_MUTI_BOOT_DEVICE)
        if (((pNorSpiConfig->pUserPartConfig[AMBA_USER_PARTITION_DSP_uCODE].Attribute & AMBA_PARTITION_ATTR_STORE_DEVIC) >> 6) == AMBA_NVM_SPI_NOR) {
            RetVal = AmbaSpiNOR_LoadNvmRomFileTable();
        }
#else
        RetVal = AmbaSpiNOR_LoadNvmRomFileTable();
#endif
    }

#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
    (void) AmbaCSL_NorSmErrorEnable(); /* Enable SM error detection after program nor_spi controller */
#endif
    return RetVal;
}

UINT8 AmbaSpiNOR_Isinit(void)
{
    return AmbaNOR_SPI_Ctrl.Isinit;
}

void AmbaSpiNOR_HandleCoreFreqChange(void)
{
    if (AmbaNOR_SPI_Ctrl.Isinit == 1U) {
        AmbaRTSL_NorSetSPIClk(AmbaRTSL_NorSpiCtrl.pNorDevInfo->SpiFrequncy);
    }
}

