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
#include "AmbaDef.h"

#include "AmbaMisraFix.h"
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
#include "AmbaCortexA76.h"
#else
#include "AmbaCortexA53.h"
#endif
#include "AmbaWrap.h"
#include "AmbaIOUtility.h"

#include "AmbaRTSL_SpiNOR.h"
#include "AmbaRTSL_SpiNOR_Ctrl.h"
#include "AmbaCSL_SpiNOR.h"
#ifdef CONFIG_SPINOR_DMA_ENABLE
#include "AmbaRTSL_DMA.h"
#endif
#include "AmbaRTSL_PLL.h"
#include "AmbaCSL_PLL.h"
#include "AmbaSpiNOR_Ctrl.h"
#ifdef CONFIG_QNX
#define AmbaDelayCycles(...)
UINT32 AmbaRTSL_CacheFlushDataPtr(const void * pVirtAddr, UINT32 Size)
{
    (void)pVirtAddr;
    (void)Size;
    return 0;
};
#else
#include "AmbaRTSL_GIC.h"
#include "AmbaRTSL_Cache.h"
#endif

#if defined (CONFIG_SOC_CV5)|| defined (CONFIG_SOC_CV52)|| defined (CONFIG_SOC_CV2FS)|| defined (CONFIG_SOC_CV22FS)
#define AMBA_SPINOR_OCT_ENABLE
#define AMBA_SPINOR_MIN_DIVIDER (0x4U)
#else
#define AMBA_SPINOR_MIN_DIVIDER (0x2U)
#endif

#if defined(AMBA_SPINOR_OCT_ENABLE) && !defined(CONFIG_AMBALINK_RPMSG_G1) && !defined(CONFIG_LINUX)
/* Enable cmd-addr-data 8-8-8 mode */
#define AMBA_SPINOR_OCT_CMD_ADDR_ENABLE
#endif

static ULONG base_addr;
static UINT32 IntNo;

//#define ASSERT() __asm__ __volatile__ ("B .");

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

void (*AmbaRTSL_NORIsrDoneCallBack)(void) = NULL;
#ifndef CONFIG_QNX
/* disable NEON registers usage in ISR */
#pragma GCC push_options
#pragma GCC target("general-regs-only")

static void AmbaRTSL_NorTranDoneIsr(UINT32 IntID, UINT32 IsrArg)
{
    AmbaMisra_TouchUnused(&IntID);
    AmbaMisra_TouchUnused(&IsrArg);

    if (AmbaRTSL_NORIsrDoneCallBack != NULL) {
        AmbaRTSL_NORIsrDoneCallBack();
    }
}
#pragma GCC pop_options
#endif

static UINT32 AmbaRTSL_NorReadStatus(UINT8 StatusNum, UINT8 *Status);
static UINT32 AmbaRTSL_NorReadFlag(UINT8 *Status);
static UINT32 AmbaNOR_FlashCheckStatus(UINT8 CheckBit);

AMBA_RTSL_NOR_SPI_CTRL_s AmbaRTSL_NorSpiCtrl;


static UINT32 AmbaRTSL_NorWriteEnable(void);

static void WriteByte(ULONG Address, UINT8 Value)
{
    volatile UINT8 *ptr = NULL;
    if (AmbaWrap_memcpy(&ptr, &Address, sizeof(ptr)) != OK) { /* Do nothing */ };
    *ptr = Value;
}

static UINT8 ReadByte(ULONG Address)
{
    const volatile UINT8 *ptr = NULL;
    if (AmbaWrap_memcpy(&ptr, &Address, sizeof(ptr)) != OK) { /* Do nothing */ };
    return *ptr;
}

/*
 *  @RoutineName:: AmbaRTSL_NandWaitInt
 *
 *  @Description:: Waiting for NAND command done interrupt
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
 */
void AmbaRTSL_NorWaitInt(void)
{
    AmbaCSL_NorWaitInt();
    AmbaCSL_NorClearAllIrqStatus();
    AmbaCSL_NorDisableAllIRQ(); /* disable all interrupts */
}

/*
 *  @RoutineName:: AmbaRTSL_NorSpiStop
 *
 *  @Description:: Stop SPI transactions
 *
 *  @Input      ::  none
 *
 *  @Output     :: none
 *
 *  @Return     :: void
 */
void AmbaRTSL_NorSpiStop(void)
{
    AmbaCSL_NorClearAllIrqStatus();
    AmbaCSL_NorDisableAllIRQ(); /* disable all interrupts */
}

/*
 *  @RoutineName:: AmbaRTSL_NorSpiStop
 *
 *  @Description:: Stop SPI transactions
 *
 *  @Input      ::  none
 *
 *  @Output     :: none
 *
 *  @Return     :: void
 */
void AmbaRTSL_AllocateNorSpiDMAChan(UINT32 RxChannelNum, UINT32 TxChannelNum)
{
    AmbaRTSL_NorSpiCtrl.NorRxDMAChannel = RxChannelNum;
    AmbaRTSL_NorSpiCtrl.NorTxDMAChannel = TxChannelNum;
}

/*
 *  @RoutineName:: AmbaRTSL_NorSetSPIClk
 *
 *  @Description:: Configure the SPI interface
 *
 *  @Input      ::
 *      pSpiReg:    pointer to SPI hardware registers
 *      pSpiConfig: pointer to SPI configurations
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
void AmbaRTSL_NorSetSPIClk(UINT32 Frequency)
{
#ifndef CONFIG_QNX
    /* The minimun divider of spinor should be 0x2/0x4 (can't set 0) */
    UINT8 ClkDivider = AMBA_SPINOR_MIN_DIVIDER;

    /* Choose the Enernet to the source of NorSpi */
    AmbaCSL_PllSetSpiNorRefClkSelect(AMBA_SPINOR_CLK_FROM_ENET);
    /* only use controller divider  */
#if defined (CONFIG_SOC_CV2FS)|| defined (CONFIG_SOC_CV22FS)
    (void)AmbaRTSL_PllSetSpiNorClk(Frequency * ClkDivider);
#endif
    AmbaCSL_NorSpiSetClockDiv(ClkDivider);

    while((AmbaRTSL_PllGetSpiNorClk() / ClkDivider) > Frequency) {
        ClkDivider ++;
        AmbaCSL_NorSpiSetClockDiv(ClkDivider);
    }
#else
    (void)Frequency;
#endif
}

/*
 *  @RoutineName:: AmbaRTSL_NorSpiConfig
 *
 *  @Description:: NOR SPI software configurations
 *
 *  @Input      ::
 *         pNorSpiConfig: pointer to NOR software configurations
 *
 *  @Output     :: none
 *
 *  @Return     :: none
 */
UINT32 AmbaRTSL_NorSpiConfig(AMBA_NOR_SPI_CONFIG_s *pNorSpiConfig)
{
    UINT32 RetVal = OK;
    AMBA_NORSPI_DEV_INFO_s *pNorDevInfo;
    const AMBA_PARTITION_CONFIG_s *pUserPartConfig;
    UINT32 PageSize, k;

    AmbaMisra_TouchUnused(pNorSpiConfig);

    if ((pNorSpiConfig == NULL) ||
        (pNorSpiConfig->pNorSpiDevInfo == NULL) ||
        (pNorSpiConfig->pUserPartConfig == NULL)) {
        RetVal = SPINOR_ERR_ARG;  /* wrong parameters */
    } else {
        pNorDevInfo     = pNorSpiConfig->pNorSpiDevInfo;
        pUserPartConfig = pNorSpiConfig->pUserPartConfig;
        PageSize        = pNorDevInfo->PageSize;

        AmbaRTSL_NorSpiCtrl.pNorDevInfo     = pNorDevInfo;                    /* save the pointer to NorSpi Device Information */
        AmbaRTSL_NorSpiCtrl.pSysPartConfig  = pNorSpiConfig->pSysPartConfig;  /* pointer to System partition configurations */
        AmbaRTSL_NorSpiCtrl.pUserPartConfig = pNorSpiConfig->pUserPartConfig; /* save the pointer to User Partition Configurations */
        AmbaRTSL_NorSpiCtrl.pSpiSetting     = pNorSpiConfig->SpiSetting;

        /* Block size in Byte */
        AmbaRTSL_NorSpiCtrl.TotalByteSize = pNorDevInfo->TotalByteSize;

        /* Total number of blocks */
        AmbaRTSL_NorSpiCtrl.ProgramPageSize = pNorDevInfo->PageSize;

        /* Total number of blocks */
        AmbaRTSL_NorSpiCtrl.PartitionAllocateSize = pNorDevInfo->EraseBlockSize;

        /* Number of Pages for BST, BST total size = Boot header + Bootstrap */
        AmbaRTSL_NorSpiCtrl.BstPageCount = GetRoundUpValU32(AMBA_NORSPI_BOOTSTRAP_CODE_SPACE_SIZE + AMBA_NORSPI_BOOT_HEADER_SIZE, PageSize);

        /* Number of Pages for System Partition Table */
        AmbaRTSL_NorSpiCtrl.SysPtblPageCount = GetRoundUpValU32((UINT32)(sizeof(AMBA_SYS_PARTITION_TABLE_s) & 0xffffffffU), PageSize);

        /* Number of Pages for User Partition Table */
        AmbaRTSL_NorSpiCtrl.UserPtblPageCount = GetRoundUpValU32((UINT32)(sizeof(AMBA_USER_PARTITION_TABLE_s) & 0xffffffffU), PageSize);

        /* Number of Pages for Vendor Specific Data */
        k = pUserPartConfig[AMBA_USER_PARTITION_PTB].ByteCount;
        AmbaRTSL_NorSpiCtrl.VendorDataPageCount = GetRoundUpValU32(k, PageSize);

        /* SPI Interface Config */
        AmbaRTSL_NorSetSPIClk(pNorDevInfo->SpiFrequncy);
        AmbaRTSL_NorSetSPISetting(pNorSpiConfig->SpiSetting);
    }
    return RetVal;
}

/*
 *  @RoutineName:: AmbaRTSL_NorWriteFIFO
 *
 *  @Description:: Write FIFO data
 *
 *  @Input      ::
 *      TxDataSize:    sending data size in Frames
 *      pTxDataBuf:    pointer to the sending data buffer
 *
 *  @Output     :: none
 *
 *  @Return     :: none
 */
static void AmbaRTSL_NorWriteFIFO(UINT32 TxDataSize, const void *pTxDataBuf)
{
    UINT32 i;
    const UINT8 *pWorkUINT8 = NULL;

    AmbaMisra_TypeCast(&pWorkUINT8, &pTxDataBuf);
    for (i = 0; i < TxDataSize; i++) {
#ifdef CONFIG_QNX
        WriteByte((ULONG)&(pAmbaNOR_Reg->TxData), pWorkUINT8[i]);
#else
        WriteByte((base_addr + AMBA_NORSPI_TXBUF_OFFSET), pWorkUINT8[i]);
#endif
    }
}

/*
 *  @RoutineName:: AmbaRTSL_NorReadFIFO
 *
 *  @Description:: Read FIFO data
 *
 *  @Input      ::
 *      pSpiReg:    pointer to the Hardware registers
 *      RxDataSize: receiving data size in Frames
 *      pRxDataBuf: pointer to the receiving data buffer
 *
 *  @Output     :: none
 *
 *  @Return     :: none
 */
void AmbaRTSL_NorReadFIFO(UINT32 RxDataSize, UINT8 *pRxDataBuf)
{
    UINT32 i;

    for (i = 0; i < RxDataSize; i++) {
#ifdef CONFIG_QNX
        pRxDataBuf[i] = ReadByte((ULONG)&(pAmbaNOR_Reg->RxData));
#else
        pRxDataBuf[i] = ReadByte((base_addr + AMBA_NORSPI_RXBUF_OFFSET));
#endif
    }
}

/*
 *  @RoutineName:: AmbaRTSL_ResetFIFO
 *
 *  @Description:: Reset the RxFIFO and TxFIFO
 *
 *  @Input      ::
 *
 *  @Output     ::
 *
 *  @Return     :: none
 */
void AmbaRTSL_ResetFIFO(void)
{
    /* dummy read buffer for reset RxFIFO bug. */
    //static UINT8 DummyReadBuffer[512] __attribute__((aligned(CACHE_LINE_SIZE)));

    AmbaCSL_NorResetRxFifio();
    AmbaCSL_NorResetTxFifo();

    /* after reset fifo, the 0x28 will become 0x10,
    *so , read REG200 times to clear the 0x28,  this is a bug in hardware
    */
    //AmbaRTSL_NorReadFIFO(220U, &DummyReadBuffer[0]);
}

#ifdef AMBA_SPINOR_OCT_CMD_ADDR_ENABLE
static void AmbaRTSL_NorSetupOctDTR(UINT8 CmdType, AMBA_NOR_DTR_CTRL_REG_s *pDTR)
{
    /* Setup DTR Ctrl */
    switch (CmdType) {
    case NOR_SPI_SEND_CMD:
        pDTR->DataWriteen = 1U;
        pDTR->NumCmdLane  = NOR_DTR_CMD_LANE8;
        pDTR->NumAddrLane = NOR_DTR_ADDR_LANE8;
        pDTR->AddressDTR  = 1;
        break;
    case NOR_SPI_WRITE_REG:
        pDTR->DataWriteen = 1U;
        pDTR->NumDataLane = NOR_DTR_DATA_LANE8;
        pDTR->NumAddrLane = NOR_DTR_ADDR_LANE8;
        pDTR->NumCmdLane  = NOR_DTR_CMD_LANE8;
        pDTR->AddressDTR  = 1U;
        pDTR->DataDTR     = 1U;
        break;
    case NOR_SPI_READ_REG:
        pDTR->DataReadn   = 1U;
        pDTR->NumDataLane = NOR_DTR_DATA_LANE8;
        pDTR->NumAddrLane = NOR_DTR_ADDR_LANE8;
        pDTR->NumCmdLane  = NOR_DTR_CMD_LANE8;
        pDTR->AddressDTR  = 1;
        pDTR->DataDTR     = 1;
        break;
    case NOR_SPI_OCT_WRITE_DATA:
        pDTR->DataWriteen = 1U;
        pDTR->NumDataLane = NOR_DTR_DATA_LANE8;
        pDTR->NumAddrLane = NOR_DTR_ADDR_LANE8;
        pDTR->NumCmdLane  = NOR_DTR_CMD_LANE8;
        pDTR->AddressDTR  = 1;
        pDTR->DataDTR     = 1;
        break;
    case NOR_SPI_OCT_READ_DATA:
        pDTR->DataReadn   = 1U;
        pDTR->NumDataLane = NOR_DTR_DATA_LANE8;
        pDTR->NumAddrLane = NOR_DTR_ADDR_LANE8;
        pDTR->NumCmdLane  = NOR_DTR_CMD_LANE8;
        pDTR->AddressDTR  = 1;
        pDTR->DataDTR     = 1;
        break;
    default:
        /* ASSERT(); */
        break;
    }
}
#endif

static void AmbaRTSL_NorSetupDTR(UINT8 CmdType, AMBA_NOR_DTR_CTRL_REG_s *pDTR)
{
    /* Setup DTR Ctrl */
    switch (CmdType) {
    case NOR_SPI_SEND_CMD:
        pDTR->DataWriteen = 1U;
        pDTR->NumDataLane = 0;
        break;
    case NOR_SPI_WRITE_DATA:
        pDTR->DataWriteen = 1U;
        pDTR->RxLane      = 0;
        pDTR->NumCmdLane  = 0;
        pDTR->NumAddrLane = 0;
        pDTR->NumDataLane = AmbaRTSL_NorSpiCtrl.pNorDevInfo->Program.DataLane;
        pDTR->DataReadn   = 0;
        pDTR->CmdDTR      = 0;
        break;
    case NOR_SPI_WRITE_DATA_IO:
        pDTR->DataWriteen = 1U;
        pDTR->RxLane      = 0;
        pDTR->NumCmdLane  = 0;
        pDTR->NumAddrLane = AmbaRTSL_NorSpiCtrl.pNorDevInfo->Program.DataLane;
        pDTR->NumDataLane = AmbaRTSL_NorSpiCtrl.pNorDevInfo->Program.DataLane;
        pDTR->DataReadn   = 0;
        pDTR->CmdDTR      = 0;
        break;
    case NOR_SPI_WRITE_REG:
        pDTR->DataWriteen = 1U;
        pDTR->RxLane      = 1U;
        pDTR->NumCmdLane  = 0;
        pDTR->NumAddrLane = 0;
        pDTR->NumDataLane = 0;
        pDTR->DataReadn   = 0;
        pDTR->CmdDTR      = 0;
        break;
    case NOR_SPI_READ_REG:
        pDTR->DataReadn   = 1U;
        pDTR->RxLane      = 1U;
        pDTR->NumDataLane = AMBA_NORSPI_DATA_2_LANE;
        break;
    case NOR_SPI_READ_DATA:
        pDTR->DataReadn   = 1U;
        pDTR->NumAddrLane = 0;
        pDTR->NumDataLane = AmbaRTSL_NorSpiCtrl.pNorDevInfo->Read.DataLane;
        pDTR->AddressDTR  = 0;
        pDTR->DataDTR     = 0;
        break;
    case NOR_SPI_READ_DATA_DTR:
        pDTR->DataReadn   = 1U;
        pDTR->NumAddrLane = AmbaRTSL_NorSpiCtrl.pNorDevInfo->Read.DataLane;
        pDTR->NumDataLane = AmbaRTSL_NorSpiCtrl.pNorDevInfo->Read.DataLane;
        pDTR->AddressDTR  = 1U;
        pDTR->DataDTR     = 1U;
        break;
    case NOR_SPI_READ_DATA_IO:
        pDTR->DataReadn   = 1U;
        pDTR->NumAddrLane = AmbaRTSL_NorSpiCtrl.pNorDevInfo->Read.DataLane;
        pDTR->NumDataLane = AmbaRTSL_NorSpiCtrl.pNorDevInfo->Read.DataLane;
        pDTR->AddressDTR  = 0;
        pDTR->DataDTR     = 0;
        break;
    default:
        /* ASSERT(); */
        break;
    }
}

/*
 *  @RoutineName:: AmbaRTSL_NorSendCmd
 *
 *  @Description:: issue a command to Nor Chip
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
 */
static void AmbaRTSL_NorSendCmd(UINT8 CmdType)
{
    AMBA_NOR_DTR_CTRL_REG_s DTR = {0};
#ifdef AMBA_SPINOR_OCT_CMD_ADDR_ENABLE
    if (AmbaRTSL_NorSpiCtrl.pNorDevInfo->Read.DataLane == AMBA_NORSPI_DATA_8_LANE) {
        AmbaRTSL_NorSetupOctDTR(CmdType, &DTR);
        if (DTR.DataWriteen == 1U) {
            AmbaCSL_NorSpiDqsDisable();
        } else {
            AmbaCSL_NorSpiDqsEnable();
        }
    } else {
        AmbaRTSL_NorSetupDTR(CmdType, &DTR);
    }
#else
    /* Use 1-1-8 mode in AmbaLink. */
    AmbaRTSL_NorSetupDTR(CmdType, &DTR);
#endif
    if (0x0U != AmbaRTSL_NorSpiCtrl.pNorDevInfo->LSBFirst) {
        DTR.LSBFirst = 1U;
    }

    if (CmdType == NOR_SPI_READ_REG) {
        /* Clear the remain Data on RxFIFO  */
        if (AmbaCSL_NorGetRxFifoNotEmpty() == 1U) {
            AmbaRTSL_ResetFIFO();
        }
    }
    AmbaCSL_NorSpiSetDTRCtrl(DTR);

    /* Set interrupt Mask*/
    AmbaCSL_NorEnableDataLenthreachIRQ();

    /* Enable Tansmiting/Receive bit*/
    AmbaCSL_NorTransEnable();
}

/*
 *  @RoutineName:: AmbaRTSL_NorReset
 *
 *  @Description:: Reset the Nor chip
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
 */
void AmbaRTSL_NorResetEnable(void)
{
    AmbaCSL_NorReset(AmbaRTSL_NorSpiCtrl.pNorDevInfo->ResetEnable);
    AmbaRTSL_NorSendCmd(NOR_SPI_SEND_CMD);
}

/*
 *  @RoutineName:: NOR_FlashCheckStatus
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
static UINT32 AmbaNOR_FlashCheckStatus(UINT8 CheckBit)
{
    UINT8 PollStatus;
    UINT32 RetStatus = OK;

    while (RetStatus == OK) {
        /* Get Status form NOR device */
        RetStatus = AmbaRTSL_NorReadStatus(1U, &PollStatus);
        if (RetStatus != OK) {
            AmbaRTSL_NorClearStatus();
            AmbaRTSL_NorWaitInt();
        } else if (0x0U != (PollStatus & NOR_STATUS_WRTIE_IN_PROCESS)) {
            AmbaDelayCycles(0xfU);
            continue;
        } else {
            break;
        }
    }

    if (RetStatus == OK) {
        RetStatus = AmbaRTSL_NorReadFlag(&PollStatus);
        if ((RetStatus != OK) ||
            (0x0U != (PollStatus & CheckBit))) {
            RetStatus = SPINOR_ERR_IO_FAIL;
        }
    }

    return RetStatus;
}

#if 0
UINT32 AmbaRTSL_NorReadReg(UINT32 RegAddr, UINT8 DummyCycle, UINT8 *pValue)
{
    UINT32 RetStatus = OK;

    /* Clear the remain Data on RxFIFO  */
    if (AmbaCSL_NorGetRxFifoNotEmpty() == 1U) {
        AmbaRTSL_ResetFIFO();
    }

    AmbaCSL_NorReadSetup(0x85U, DummyCycle, RegAddr, 1U, 0x4U);

    /* Set the Threshold to Receive Fifo */
    AmbaCSL_NorSetRxFifoThreshold(AmbaRTSL_NorSpiCtrl.pSpiSetting->RxThresholdLevel);

    AmbaRTSL_NorSendCmd(NOR_SPI_READ_REG);
    AmbaRTSL_NorWaitInt();

    if (AmbaCSL_NorGetRxFifoDataSize() == 1U) {
        AmbaRTSL_NorReadFIFO(1U, pValue);
        RetStatus = OK;
    } else {
        RetStatus = SPINOR_ERR_IO_FAIL;
    }

    return RetStatus;
}
#endif

/*
 *  @RoutineName:: AmbaRTSL_NorReset
 *
 *  @Description:: Reset the Nor chip
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
 */
void AmbaRTSL_NorReset(void)
{
    AmbaCSL_NorReset(AmbaRTSL_NorSpiCtrl.pNorDevInfo->Reset);
    AmbaRTSL_NorSendCmd(NOR_SPI_SEND_CMD);
}

/*
 *  @RoutineName:: AmbaRTSL_NorReadIDCmd
 *
 *  @Description:: Issue Read ID command to Nor flash chip
 *
 *  @Input      ::
 *          AMBA_NAND_DEV_s *pDev : Pointer to nand device structure
 *
 *  @Output     :: none
 *
 *  @Return     :: none
 */
void AmbaRTSL_NorReadIDCmd(void)
{
#ifdef AMBA_SPINOR_OCT_CMD_ADDR_ENABLE
    if (AmbaRTSL_NorSpiCtrl.pNorDevInfo->Read.DataLane == AMBA_NORSPI_DATA_8_LANE) {
        AmbaCSL_NorReadIDOct(AmbaRTSL_NorSpiCtrl.pNorDevInfo->ReadID);
    } else {
        AmbaCSL_NorReadID(AmbaRTSL_NorSpiCtrl.pNorDevInfo->ReadID);
    }
#else
    AmbaCSL_NorReadID(AmbaRTSL_NorSpiCtrl.pNorDevInfo->ReadID);
#endif
    AmbaRTSL_NorSendCmd(NOR_SPI_READ_REG);
}

void AmbaRTSL_NorReadSFDP(void)
{
    AmbaCSL_NorReadSFDP(0x5A);
    AmbaRTSL_NorSendCmd(NOR_SPI_READ_REG);
}

/*
 *  @RoutineName:: AmbaRTSL_NorReadStatusCmd
 *
 *  @Description:: Issue the read status command to read the Nor status
 *
 *  @Input      ::
 *          UINT8 StatusNum     :
 *
 *  @Output     :: none
 *
 *  @Return     :: none
 */
void AmbaRTSL_NorReadStatusCmd(UINT8 StatusNum)
{
    UINT8 CmdIndex;
    if (StatusNum == 1U) {
        CmdIndex = AmbaRTSL_NorSpiCtrl.pNorDevInfo->ReadStatus0;
    } else {
        CmdIndex = AmbaRTSL_NorSpiCtrl.pNorDevInfo->ReadStatus1;
    }
#if defined(AMBA_SPINOR_OCT_CMD_ADDR_ENABLE)
    if (AmbaRTSL_NorSpiCtrl.pNorDevInfo->Read.DataLane == AMBA_NORSPI_DATA_8_LANE) {
        AmbaCSL_NorReadStatusCmdOct(CmdIndex);
    } else {
        AmbaCSL_NorReadStatusCmd(CmdIndex);
    }
#else
    /* Use 1-1-8 mode in AmbaLink. */
    AmbaCSL_NorReadStatusCmd(CmdIndex);
#endif

    AmbaRTSL_NorSendCmd(NOR_SPI_READ_REG);
}

void AmbaRTSL_NorReadFlagStatusCmd(void)
{
#if defined(AMBA_SPINOR_OCT_CMD_ADDR_ENABLE)
    if (AmbaRTSL_NorSpiCtrl.pNorDevInfo->Read.DataLane == AMBA_NORSPI_DATA_8_LANE) {
        AmbaCSL_NorReadStatusCmdOct(AmbaRTSL_NorSpiCtrl.pNorDevInfo->ReadFlagStatus);
    } else {
        AmbaCSL_NorReadStatusCmd(AmbaRTSL_NorSpiCtrl.pNorDevInfo->ReadFlagStatus);
    }
#else
    AmbaCSL_NorReadStatusCmd(AmbaRTSL_NorSpiCtrl.pNorDevInfo->ReadFlagStatus);
#endif

    AmbaRTSL_NorSendCmd(NOR_SPI_READ_REG);
}

/*
 *  @RoutineName:: AmbaRTSL_NorReadFlag
 *
 *  @Description:: Issue the read status command to read the NOR status
 *
 *  @Input      :: Status
 *
 *  @Output     :: none
 *
 *  @Return     :: INT32 : OK(0)/NG(-1)
 */
static UINT32 AmbaRTSL_NorReadFlag(UINT8 *Status)
{
    UINT32 RetVal = OK;

    AmbaRTSL_NorReadFlagStatusCmd();
    /* Wait issue readsatus done */
    AmbaRTSL_NorWaitInt();

    if (AmbaCSL_NorGetRxFifoDataSize() == 1U) {
        AmbaRTSL_NorReadFIFO(1U, Status);
        RetVal = OK;
    } else {
        RetVal = SPINOR_ERR_NOT_READY;
    }

    return RetVal;
}

static UINT32 AmbaRTSL_NorReadStatus(UINT8 StatusNum, UINT8 *Status)
{
    UINT32 RetVal;
    AmbaRTSL_NorReadStatusCmd(StatusNum);
    AmbaRTSL_NorWaitInt();

    if (AmbaCSL_NorGetRxFifoDataSize() == 1U) {
        AmbaRTSL_NorReadFIFO(1U, Status);
        RetVal = OK;
    } else {
        RetVal = SPINOR_ERR_NOT_READY;
    }

    return RetVal;
}

#ifdef AMBA_SPINOR_OCT_CMD_ADDR_ENABLE
static void AmbaRTSL_NorWriteVcr(UINT32 RegAddr, const UINT8 *pDataBuf)
{
    (void)AmbaRTSL_NorWriteEnable();

    AmbaCSL_NorProgramSetup(0x81U, 0, RegAddr, 1U, 4U);

    AmbaCSL_NorSetTxFifoThreshold(AmbaRTSL_NorSpiCtrl.pSpiSetting->TxThresholdLevel);

    /* Clear the remain Data on TxFIFO  */
    if (AmbaCSL_NorGetTxFifoEmpty() == 0U) {
        AmbaRTSL_ResetFIFO();
    }

    /* Write data into TxFIFO */
    AmbaRTSL_NorWriteFIFO(1U, pDataBuf);

    AmbaRTSL_NorSendCmd(NOR_SPI_WRITE_REG);

    AmbaRTSL_NorWaitInt();
}
#endif

#if defined(AMBA_SPINOR_OCT_ENABLE)
void AmbaRTSL_NorWriteVcrSetup(UINT32 RegAddr, const UINT8 *pDataBuf)
{
    AmbaCSL_NorProgramSetup(0x81U, 0, RegAddr, 1U, 4U);

    AmbaCSL_NorSetTxFifoThreshold(AmbaRTSL_NorSpiCtrl.pSpiSetting->TxThresholdLevel);

    /* Clear the remain Data on TxFIFO  */
    if (AmbaCSL_NorGetTxFifoEmpty() == 0U) {
        AmbaRTSL_ResetFIFO();
    }

    /* Write data into TxFIFO */
    AmbaRTSL_NorWriteFIFO(1U, pDataBuf);

    AmbaRTSL_NorSendCmd(NOR_SPI_WRITE_REG);
}
#endif

/*
 *  @RoutineName:: AmbaRTSL_NorReadStatusCmd
 *
 *  @Description:: Issue the read status command to read the Nor status
 *
 *  @Input      ::
 *          UINT8 StatusNum     :
 *
 *  @Output     :: none
 *
 *  @Return     :: none
 */
void AmbaRTSL_NorSetExtAddr(void)
{
    UINT8 BankData = ((UINT8)1U << 7U);

    AmbaCSL_NorSetExtAddr(AmbaRTSL_NorSpiCtrl.pNorDevInfo->Enable4ByteMode, BankData);
    AmbaRTSL_NorSendCmd(NOR_SPI_SEND_CMD);
}

/*
 *  @RoutineName:: AmbaRTSL_NorWriteEnableStart
 *
 *  @Description:: Issue write_enable command to Nor Device
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
 */
void AmbaRTSL_NorWriteEnableStart(void)
{
    AmbaCSL_NorWriteEnableCmd(AmbaRTSL_NorSpiCtrl.pNorDevInfo->WriteEnable);
    AmbaRTSL_NorSendCmd(NOR_SPI_SEND_CMD);
}

static UINT32 AmbaRTSL_NorWriteEnable(void)
{
    UINT32 RetVal = OK;
    UINT8 Status;

    AmbaRTSL_NorWriteEnableStart();
    AmbaRTSL_NorWaitInt();
    do {
        RetVal = AmbaRTSL_NorReadStatus(1U, &Status);
        if (RetVal != OK) {
            RetVal = SPINOR_ERR_NOT_READY;
        }
        /* Small busy-wait loop */
        AmbaDelayCycles(0xfU);
    } while ((RetVal == OK) && (0x0U == (Status & (UINT8)NOR_STATUS_WRTIE_ENABLE)));

    return RetVal;
}

/*
 *  @RoutineName:: AmbaRTSL_NorWriteDisable
 *
 *  @Description::  Issue write_enable command to Nor Device
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
 *
 */
void AmbaRTSL_NorWriteDisable(void)
{
    AmbaCSL_NorWriteDisableCmd(AmbaRTSL_NorSpiCtrl.pNorDevInfo->WriteDisable);
    AmbaRTSL_NorSendCmd(NOR_SPI_SEND_CMD);
}

/*
 *  @RoutineName:: AmbaRTSL_NorReadQuadModeCmd
 *
 *  @Description:: Issue the read QuadMode command to Nor status
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
 */
void AmbaRTSL_NorReadQuadModeCmd(void)
{
    AmbaCSL_NorReadStatusCmd(AmbaRTSL_NorSpiCtrl.pNorDevInfo->Quad.ReadQuadMode);
    AmbaRTSL_NorSendCmd(NOR_SPI_READ_REG);
}

static UINT32 AmbaRTSL_NorReadQuadMode(UINT8 *Status)
{
    UINT32 RetStatus;

    AmbaRTSL_NorReadQuadModeCmd();
    AmbaRTSL_NorWaitInt();

    if (AmbaCSL_NorGetRxFifoDataSize() == AmbaRTSL_NorSpiCtrl.pNorDevInfo->Quad.QuadCmdArgLen) {
        AmbaRTSL_NorReadFIFO(1U, Status);
        RetStatus = OK;
    } else {
        RetStatus = SPINOR_ERR_IO_FAIL;
    }

    return RetStatus;
}

static UINT32 AmbaRTSL_NorChkDeviceQuadMode(UINT8 *Mode)
{
    UINT8 Status;
    UINT32 RetStatus = OK;

    if (0x0U != AmbaRTSL_NorSpiCtrl.pNorDevInfo->Quad.ReadQuadMode) {
        RetStatus = AmbaRTSL_NorReadQuadMode(&Status);
        if (RetStatus != OK) {
            AmbaRTSL_NorClearStatus();
            AmbaRTSL_NorWaitInt();
        } else {
            if (0x0U != (Status & AmbaRTSL_NorSpiCtrl.pNorDevInfo->Quad.QuadFlag)) {
                *Mode = AMBA_NORSPI_DATA_4_LANE;
            } else {
                *Mode = AMBA_NORSPI_DATA_2_LANE;
            }
        }
    } else {
        *Mode = AmbaRTSL_NorSpiCtrl.BusMode;
    }

    return RetStatus;
}

/*
 *  @RoutineName:: AmbaRTSL_NorEnterQUAD
 *
 *  @Description:: Issue Enter Quad Mode command to Nor Device
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
 *
 */
void AmbaRTSL_NorEnterQUADStart(UINT8 ByteCount, const UINT8 *pDataBuf)
{
    /* Clear the remain Data on TxFIFO  */
    if (AmbaCSL_NorGetTxFifoEmpty() == 0U) {
        AmbaRTSL_ResetFIFO();
    }

    /* Write data into TxFIFO */
    AmbaRTSL_NorWriteFIFO(ByteCount, pDataBuf);
    AmbaCSL_NorSendQUADCmd(AmbaRTSL_NorSpiCtrl.pNorDevInfo->Quad.EnableQuadMode, ByteCount);
    AmbaRTSL_NorSendCmd(NOR_SPI_WRITE_REG);
}

static UINT32 AmbaRTSL_NorEnterQUAD(void)
{
    UINT32 RetStatus  = 0;
    UINT8 DataBuf[2], Status;
    UINT8 CmdArgLen = AmbaRTSL_NorSpiCtrl.pNorDevInfo->Quad.QuadCmdArgLen;

    if (0x0U != AmbaRTSL_NorSpiCtrl.pNorDevInfo->Quad.EnableQuadMode) {
        (void)AmbaRTSL_NorWriteEnable();
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

        AmbaRTSL_NorWaitInt();

        if (0x0U != AmbaRTSL_NorSpiCtrl.pNorDevInfo->Quad.ReadQuadMode) {
            do { /* Wait NOR device into Quad Mode*/
                RetStatus = AmbaRTSL_NorReadQuadMode(&Status);
                if (RetStatus != OK) {
                    AmbaRTSL_NorClearStatus();
                    AmbaRTSL_NorWaitInt();
                }
                if ((RetStatus == OK) && (0x0U != (Status & NOR_STATUS_WRTIE_IN_PROCESS))) {
                    continue;
                }
            } while ((RetStatus == OK) && ((Status & AmbaRTSL_NorSpiCtrl.pNorDevInfo->Quad.QuadFlag) == 0U));
        }
    }
    if (RetStatus == OK) {
        AmbaRTSL_NorSpiCtrl.BusMode = AMBA_NORSPI_DATA_4_LANE;
    }

    return RetStatus;
}

/*
 *  @RoutineName:: AmbaRTSL_NorExitQUAD
 *
 *  @Description:: Issue Exit Quad Mode command to Nor Device
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
 *
 */
void AmbaRTSL_NorExitQUADStart(UINT8 ByteCount, const UINT8 *pDataBuf)
{
    /* Clear the remain Data on TxFIFO  */
    if (AmbaCSL_NorGetTxFifoEmpty() == 0U) {
        AmbaRTSL_ResetFIFO();
    }

    /* Write data into TxFIFO */
    AmbaRTSL_NorWriteFIFO(ByteCount, pDataBuf);
    AmbaCSL_NorSendQUADCmd(AmbaRTSL_NorSpiCtrl.pNorDevInfo->Quad.ExitQuadMode, ByteCount);
    AmbaRTSL_NorSendCmd(NOR_SPI_WRITE_REG);
}

static UINT32 AmbaRTSL_NorExitQUAD(void)
{
    UINT32 RetStatus  = 0;
    UINT8 DataBuf[2], Status;
    UINT8 CmdArgLen = AmbaRTSL_NorSpiCtrl.pNorDevInfo->Quad.QuadCmdArgLen;

    if (0x0U != AmbaRTSL_NorSpiCtrl.pNorDevInfo->Quad.ExitQuadMode) {
        (void)AmbaRTSL_NorWriteEnable();
        AmbaRTSL_NorWaitInt();

        if (0 == IO_UtilityStringCompare("Spansion", AmbaRTSL_NorSpiCtrl.pNorDevInfo->DevName, 8U)) {
            DataBuf[0] = 0x2U;
            DataBuf[1] = AmbaRTSL_NorSpiCtrl.pNorDevInfo->Quad.ExitQuadFlag;
        } else {
            DataBuf[0] = AmbaRTSL_NorSpiCtrl.pNorDevInfo->Quad.ExitQuadFlag;
            DataBuf[1] = 0x27U;
        }

        if (CmdArgLen != 0U) {
            AmbaRTSL_NorExitQUADStart(CmdArgLen, &DataBuf[0]);
        } else {
            AmbaRTSL_NorExitQUADStart(0, &DataBuf[0]);
        }

        AmbaRTSL_NorWaitInt();

        if (0x0U != AmbaRTSL_NorSpiCtrl.pNorDevInfo->Quad.ReadQuadMode) {
            do { /* Wait NOR device into Quad Mode*/
                RetStatus = AmbaRTSL_NorReadQuadMode(&Status);
                if (RetStatus != OK) {
                    AmbaRTSL_NorClearStatus();
                    AmbaRTSL_NorWaitInt();
                }
                if ((RetStatus == OK) && (0x0U != (Status & NOR_STATUS_WRTIE_IN_PROCESS))) {
                    continue;
                }
            } while ((RetStatus == OK) && (0x0U != (Status & AmbaRTSL_NorSpiCtrl.pNorDevInfo->Quad.QuadFlag)));
        }
    }
    if (RetStatus == OK) {
        AmbaRTSL_NorSpiCtrl.BusMode = 0;
    }

    return RetStatus;
}

/*
 *  @RoutineName:: AmbaRTSL_NorDeviceBusSwitch
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
static UINT32 AmbaRTSL_NorDeviceBusSwitch(UINT8 BusMode)
{
    UINT8 CurBusMode;
    UINT32 RetStatus = OK;

    if((AmbaRTSL_NorSpiCtrl.pNorDevInfo->Program.DataLane == AMBA_NORSPI_DATA_4_LANE) ||
       (AmbaRTSL_NorSpiCtrl.pNorDevInfo->Read.DataLane    == AMBA_NORSPI_DATA_4_LANE)) {
        RetStatus = AmbaRTSL_NorChkDeviceQuadMode(&CurBusMode);
        if (RetStatus == OK) {
            if (BusMode == AMBA_NORSPI_DATA_4_LANE) {
                if (CurBusMode != AMBA_NORSPI_DATA_4_LANE) {
                    RetStatus = AmbaRTSL_NorEnterQUAD();
                }
            } else {
                if (CurBusMode == AMBA_NORSPI_DATA_4_LANE) {
                    RetStatus = AmbaRTSL_NorExitQUAD();
                }
            }
        }
    }
    return RetStatus;
}

/*
 *  @RoutineName:: AmbaRTSL_NorClearStatus
 *
 *  @Description:: Issue  command to clear the status of Nor Device
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
 *
 */
void AmbaRTSL_NorClearStatus(void)
{
    AmbaCSL_NorClearStatusCmd(AmbaRTSL_NorSpiCtrl.pNorDevInfo->ClearStatus);
    AmbaRTSL_NorSendCmd(NOR_SPI_SEND_CMD);
}

/*
 *  @RoutineName:: AmbaRTSL_NorErase
 *
 *  @Description:: To erase the entire Norflash memory array
 *
 *  @Input      ::
 *          AMBA_NOR_DEV_s *pDev     : Pointer to Nor device structure
 *          UINT32 AddrHi           : [33:32] address
 *          UINT32 Addr             : [31:0] address
 *
 *  @Output     :: none
 *
 *  @Return     :: none
 */
void AmbaRTSL_NorEraseChip(void)
{
    AmbaCSL_NorEraseChip(AmbaRTSL_NorSpiCtrl.pNorDevInfo->EraseChip);
    AmbaRTSL_NorSendCmd(NOR_SPI_SEND_CMD);
}

/*
 *  @RoutineName:: AmbaRTSL_NorErase
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
UINT32 AmbaRTSL_NorErase(UINT32 Offset, UINT32 ByteCount)
{
    UINT32 CurOffset = Offset, EndOffset = Offset + ByteCount;
    UINT32 BlockStartOffset, BlockEndOffset, NumBlocks;
    UINT32 EraseSize = AmbaRTSL_NorSpiCtrl.pNorDevInfo->EraseBlockSize;
    UINT32 j;
    UINT32 RetStatus = 0;

    if ((ByteCount == 0U) || (EraseSize == 0U) || (AmbaRTSL_NorSpiCtrl.TotalByteSize == 0U)) {
        RetStatus = SPINOR_ERR_ARG;
    } else {
        if ((Offset == 0U) &&
            (ByteCount >= AmbaRTSL_NorSpiCtrl.TotalByteSize) &&
            (AmbaRTSL_NorSpiCtrl.pNorDevInfo->EraseChip != 0U)) {
            /* Issue Write-Enable Cmd */
            (void)AmbaRTSL_NorWriteEnable();
            AmbaRTSL_NorEraseChip();
            AmbaRTSL_NorWaitInt();

            RetStatus = AmbaNOR_FlashCheckStatus(AmbaRTSL_NorSpiCtrl.pNorDevInfo->EraseFailFlag);
        } else {
            /* Erase the necessary Blocks */
            BlockStartOffset = 0;
            NumBlocks = AmbaRTSL_NorSpiCtrl.TotalByteSize / EraseSize;
            for (j = 0; j < NumBlocks; j++) {
                /* Issue Write-Enable Cmd */
                (void)AmbaRTSL_NorWriteEnable();
                BlockEndOffset = BlockStartOffset + EraseSize;
                if ((BlockStartOffset >= EndOffset) || (RetStatus != OK)) {
                    break;
                } else {
                    if (BlockEndOffset > CurOffset) {
                        AmbaRTSL_NorEraseBlock(BlockStartOffset);
                        /* Wait erase operation done */
                        AmbaRTSL_NorWaitInt();
                        RetStatus = AmbaNOR_FlashCheckStatus(AmbaRTSL_NorSpiCtrl.pNorDevInfo->EraseFailFlag);
                        if (RetStatus != SPINOR_ERR_NONE) {
                            RetStatus = SPINOR_ERR_IO_FAIL;
                        }
                    }

                    if (RetStatus == SPINOR_ERR_NONE) {
                        BlockStartOffset = BlockEndOffset;
                    }
                }
            }
        }
    }

    return RetStatus;
}

/*
 *  @RoutineName:: AmbaRTSL_NorSectorErase
 *
 *  @Description:: To erase the necessary block
 *
 *  @Input      ::
 *          UINT32 Offset :  starting offset (relative to base addr)
 *
 *  @Output     :: none
 *
 *  @Return     :: none
 */
void AmbaRTSL_NorSectorErase(UINT32 Offset)
{
    AmbaCSL_NorEraseBlock(AmbaRTSL_NorSpiCtrl.pNorDevInfo->EraseSector, Offset,
                          (UINT8)AmbaRTSL_NorSpiCtrl.pNorDevInfo->AddrByte);
    AmbaRTSL_NorSendCmd(NOR_SPI_SEND_CMD);
}

/*
 *  @RoutineName:: AmbaRTSL_NorErase
 *
 *  @Description:: To erase the necessary block
 *
 *  @Input      ::
 *          UINT32 Offset :  starting offset (relative to base addr)
 *
 *  @Output     :: none
 *
 *  @Return     :: none
 */
void AmbaRTSL_NorEraseBlock(UINT32 Offset)
{
    AmbaCSL_NorEraseBlock(AmbaRTSL_NorSpiCtrl.pNorDevInfo->EraseBlk, Offset,
                          (UINT8)AmbaRTSL_NorSpiCtrl.pNorDevInfo->AddrByte);
    AmbaRTSL_NorSendCmd(NOR_SPI_SEND_CMD);
}

/*
 *  @RoutineName:: AmbaRTSL_NorSpiReadDMA
 *
 *  @Description:: Read Nor flash memory Without DMA
 *
 *  @Input      ::
 *          AMBA_NAND_HOST_s *pHost   : Pointer to NAND flash host
 *          UINT32 AddrHi           : [33:32] address
 *          UINT32 Addr             : [31:0] address
 *          UINT8 *pMain            : Pointer to the destination address of main area
 *          UINT8 *pSpare           : Pointer to the destination address of spare area
 *          UINT32 MainLen          : Length data
 *          UINT32 SpareLen         : Length of spare data
 *          UINT8 Area              : Programmed area
 *
 *  @Output     :: none
 *
 *  @Return     :: none
 */
void AmbaRTSL_NorSpiReadDMA(UINT32 From, UINT32 ByteCount, UINT8 *pDataBuf)
{
#if defined(CONFIG_SPINOR_DMA_ENABLE) && !defined(CONFIG_QNX)
    ULONG SrcAddr = base_addr + AMBA_NORSPI_RXBUF_OFFSET;
    void *pSrc = NULL;

    AmbaMisra_TouchUnused(pDataBuf);

    (void)AmbaRTSL_CacheFlushDataPtr(pDataBuf, ByteCount);

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

    (void)AmbaRTSL_DmaTransfer(AmbaRTSL_NorSpiCtrl.NorRxDMAChannel, &DmaReq);

    AmbaRTSL_NorSpiReadSetup(From, ByteCount, 1U);
#else
    AmbaMisra_TouchUnused(pDataBuf);
    (void)From;
    (void)ByteCount;
    (void)pDataBuf;
#endif
}

/*
 *  @RoutineName:: AmbaRTSL_NorSpiReadSetup
 *
 *  @Description:: Read Nor flash memory Without DMA
 *
 *  @Input      ::
 *          UINT32 From          : the addrss of SpiNOR device
 *          UINT32 ByteCount     : Length of read data
 *          UINT8 EnableDMA      : Enable/Disable DMA
 *
 *  @Output     :: none
 *
 *  @Return     :: none
 */
void AmbaRTSL_NorSpiReadSetup(UINT32 From, UINT32 ByteCount, UINT8 EnableDMA)
{
    UINT32 Type = AmbaRTSL_NorSpiCtrl.pNorDevInfo->Read.CmdType;

    /* Clear the remain Data on RxFIFO  */
    if (AmbaCSL_NorGetRxFifoNotEmpty() == 1U) {
        AmbaRTSL_ResetFIFO();
    }

    AmbaCSL_NorReadSetup(AmbaRTSL_NorSpiCtrl.pNorDevInfo->Read.CmdIndex,
                         AmbaRTSL_NorSpiCtrl.pNorDevInfo->Read.DummyCycles, From, ByteCount,
                         (UINT8)AmbaRTSL_NorSpiCtrl.pNorDevInfo->AddrByte);

    /* Set the Threshold to Receive Fifo */
    AmbaCSL_NorSetRxFifoThreshold(AmbaRTSL_NorSpiCtrl.pSpiSetting->RxThresholdLevel);

    /* Disable/Enable Nor DMA */
    AmbaCSL_NorDisableTxDMA();
    if(0x0U != EnableDMA) {
        AmbaCSL_NorEnableRxDMA();
    } else {
        AmbaCSL_NorDisableRxDMA();
    }

    if ((Type == AMBA_NORSPI_DDR_DORIO_READ) || (Type == AMBA_NORSPI_DDR_QORIO_READ)) {
        AmbaRTSL_NorSendCmd(NOR_SPI_READ_DATA_DTR);
    } else if ((Type == AMBA_NORSPI_DORIO_READ) || (Type == AMBA_NORSPI_QORIO_READ)) {
        AmbaRTSL_NorSendCmd(NOR_SPI_READ_DATA_IO);
#if defined(AMBA_SPINOR_OCT_ENABLE)
    }  else if (Type == AMBA_NORSPI_OCT_READ) {
        AmbaRTSL_NorSendCmd(NOR_SPI_OCT_READ_DATA);
#endif
    } else {
        AmbaRTSL_NorSendCmd(NOR_SPI_READ_DATA);
    }
}

/*
 *  @RoutineName:: AmbaRTSL_NorSpiWrite
 *
 *  @Description:: Write Nor flash memory With DMA
 *
 *  @Input      ::
 *          UINT32 Offset          : offset of writed address
 *          UINT32 ByteCount       : Length of writeed data
 *          UINT8 *pDataBuf        : Programmed data
 *
 *  @Output     :: none
 *
 *  @Return     :: none
 */
void AmbaRTSL_NorSpiWriteDMA(UINT32 Offset, UINT32 ByteCount, const UINT8 *pDataBuf)
{
#if defined(CONFIG_SPINOR_DMA_ENABLE) && !defined(CONFIG_QNX)
    ULONG DstAddr = base_addr + AMBA_NORSPI_TXBUF_OFFSET;
    void *pDst = NULL;
    void *pSrc = NULL;

    /* Clear the remain Data on TxFIFO  */
    if (AmbaCSL_NorGetTxFifoEmpty() == 0U) {
        AmbaRTSL_ResetFIFO();
    }

    (void)AmbaRTSL_CacheCleanDataPtr(pDataBuf, ByteCount);

    /* start DMA data transfer */
    AmbaMisra_TypeCast(&pDst, &DstAddr); /* DmaReq.pDstAddr = (void *) &(pAmbaNOR_Reg->TxData[0]); */
    DmaReq.pDstAddr = pDst;

    AmbaMisra_TypeCast(&pSrc, &pDataBuf); /* DmaReq.pSrcAddr = (void *)pDataBuf; */
    DmaReq.pSrcAddr = pSrc;
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

    (void)AmbaRTSL_DmaTransfer(AmbaRTSL_NorSpiCtrl.NorTxDMAChannel, &DmaReq);

    AmbaRTSL_NorSpiWriteSetup(Offset, ByteCount, pDataBuf, 1U);
#else
    (void)Offset;
    (void)ByteCount;
    (void)pDataBuf;
#endif
}

/*
 *  @RoutineName:: AmbaRTSL_NorSpiWriteSetup
 *
 *  @Description:: Write Nor flash memory Without DMA
 *
 *  @Input      ::
 *          UINT32 offset           : the destination address of SpiNOR device
 *          UINT32 ByteCount        : Length data
 *          UINT8 *pDataBuf         : Pointer to the Programmed Data
 *          UINT8 EnableDMA         : Enable/Disable DMA
 *
 *  @Output     :: none
 *
 *  @Return     :: none
 */
void AmbaRTSL_NorSpiWriteSetup(UINT32 Offset, UINT32 ByteCount, const UINT8 *pDataBuf, UINT8 EnableDMA)
{
    UINT32 Type = AmbaRTSL_NorSpiCtrl.pNorDevInfo->Program.CmdType;

    AmbaCSL_NorProgramSetup(AmbaRTSL_NorSpiCtrl.pNorDevInfo->Program.CmdIndex,
                            AmbaRTSL_NorSpiCtrl.pNorDevInfo->Program.DummyCycles, Offset, ByteCount,
                            AmbaRTSL_NorSpiCtrl.pNorDevInfo->AddrByte);

    /* Set the Threshold to Transmit Fifo */
    AmbaCSL_NorSetTxFifoThreshold(AmbaRTSL_NorSpiCtrl.pSpiSetting->RxThresholdLevel);

    /* Disable/Enable Nor DMA */
    AmbaCSL_NorDisableRxDMA();
    if(0x0U != EnableDMA) {
        AmbaCSL_NorEnableTxDMA();
        /* Make Sure TxFIFO is Empty*/
        while(AmbaCSL_NorGetTxFifoEmpty() != 0x0U) {};
    } else {
        /* Clear the remain Data on TxFIFO  */
        if (AmbaCSL_NorGetTxFifoEmpty() == 0U) {
            AmbaRTSL_ResetFIFO();
        }
        AmbaCSL_NorDisableTxDMA();
        /* Write data into TxFIFO */
        AmbaRTSL_NorWriteFIFO(ByteCount, pDataBuf);
    }
    if ((Type == AMBA_NORSPI_DORIO_WRITE) || (Type == AMBA_NORSPI_QORIO_WRITE)) {
        AmbaRTSL_NorSendCmd(NOR_SPI_WRITE_DATA_IO);
#if defined(AMBA_SPINOR_OCT_ENABLE)
    } else if (Type == AMBA_NORSPI_OCT_WRITE) {
        AmbaRTSL_NorSendCmd(NOR_SPI_OCT_WRITE_DATA);
#endif
    } else {
        AmbaRTSL_NorSendCmd(NOR_SPI_WRITE_DATA);
    }
}

#if defined(CONFIG_SPINOR_DMA_ENABLE)
static void AmbaRTSL_NorWaitDMA(UINT32 DmaChanNo)
{
#if !defined(CONFIG_QNX)
    while (AmbaRTSL_DmaGetIrqStatus((UINT32)DmaChanNo) == 0U) {
        continue;
    }
    (void)AmbaRTSL_DmaStop(DmaChanNo);
    /* Wait fo DMA operation done */
    AmbaRTSL_NorWaitInt();
#else
    (void)DmaChanNo;
#endif
}
#endif

/*
 *  @RoutineName:: AmbaRTSL_NorRead
 *
 *  @Description:: Read Nor flash memory
 *
 *  @Input      ::
 *
 *  @Output     :: none
 *
 *  @Return     :: none
 */
static UINT32 AmbaRTSL_NorSpiRead(UINT32 From, UINT32 ByteCount, UINT8 *pDataBuf)
{
    UINT8 *pWorkBuf;
    ULONG MainAddr = 0;
    UINT32 RetVal = 0;
    UINT32 ByteRead = ByteCount;

    AmbaMisra_TypeCast(&MainAddr, &pDataBuf);

    /* If buf align to 32 bit , use SPI-DMA transfer */
    if ((MainAddr & (CACHE_LINE_SIZE - 1U)) != 0U) {
        pWorkBuf = AmbaRTSL_NorSpiCtrl.pBuf;
    } else {
        pWorkBuf = pDataBuf;
    }

    if (ByteRead <= AMBA_SPINOR_DMA_BUF_SIZE) {
#ifdef CONFIG_SPINOR_DMA_ENABLE
        UINT32 AlignSize = 64U;
        UINT32 Tail = ByteRead % AlignSize;
        UINT32 Bulk = ByteRead / AlignSize;
        UINT32 Index;

        if (Bulk >= 1U) {
            AmbaRTSL_NorSpiReadDMA(From, ByteRead - Tail, pWorkBuf);
            /* Wait fo DMA operation done interrupt */
            AmbaRTSL_NorWaitDMA(AmbaRTSL_NorSpiCtrl.NorRxDMAChannel);
            /* Small busy-wait loop */
            AmbaDelayCycles(0xffU);
            AmbaCSL_NorDisableRxDMA();
        }
        if ((MainAddr & (CACHE_LINE_SIZE - 1U)) != 0U) {
            if (AmbaWrap_memcpy(pDataBuf, pWorkBuf, (SIZE_t)ByteRead - Tail) != OK) { /* Do nothing */ };
        }
        if (0x0U != Tail) {
            AmbaRTSL_NorSpiReadSetup(From + (Bulk * AlignSize), Tail, 0);
            AmbaRTSL_NorWaitInt();

            while (Tail != AmbaCSL_NorGetRxFifoDataSize()) {
                AmbaDelayCycles(0xfU);
            }
            Index = Bulk * AlignSize;
            AmbaRTSL_NorReadFIFO(Tail,  &pWorkBuf[Index]);

            if ((MainAddr & (CACHE_LINE_SIZE - 1U)) != 0U) {
                Index = Bulk * AlignSize;
                if (AmbaWrap_memcpy(&pDataBuf[Index], &pWorkBuf[Index], Tail) != OK) { /* Do nothing */ };
            }
        }
#else
        UINT32 Size = ByteRead;
        UINT32 i, FifoSize = AMBA_SPINOR_FIFO_SIZE;

        i = 0;
        while(ByteRead >= FifoSize) {
            AmbaRTSL_NorSpiReadSetup(From + (FifoSize * i), FifoSize, 0);
            AmbaRTSL_NorWaitInt();
            while (FifoSize != AmbaCSL_NorGetRxFifoDataSize()) {
                AmbaDelayCycles(0xfU);
            }

            AmbaRTSL_NorReadFIFO(FifoSize,  &pWorkBuf[i * FifoSize]);

            if ((MainAddr & (CACHE_LINE_SIZE - 1U)) != 0U) {
                if (AmbaWrap_memcpy(&pDataBuf[i * FifoSize], &pWorkBuf[i * FifoSize], FifoSize) != OK) { /* Do nothing */ };
            }
            ByteRead -= FifoSize;
            i++;
        }
        if (ByteRead != 0U) {
            AmbaRTSL_NorSpiReadSetup(From + (FifoSize * i), ByteRead, 0);
            AmbaRTSL_NorWaitInt();
            while (ByteRead != AmbaCSL_NorGetRxFifoDataSize()) {
                AmbaDelayCycles(0xfU);
            }

            AmbaRTSL_NorReadFIFO(ByteRead,  &pWorkBuf[i * FifoSize]);

            if ((MainAddr & (CACHE_LINE_SIZE - 1U)) != 0U) {
                if (AmbaWrap_memcpy(&pDataBuf[i * FifoSize], &pWorkBuf[i * FifoSize], ByteRead) != OK) { /* Do nothing */ };
            }
        }
        (void)AmbaRTSL_CacheFlushDataPtr(pDataBuf, Size);
#endif
    } else {
        RetVal = SPINOR_ERR_ARG;
    }

    return RetVal;
}

/*
 *  @RoutineName:: AmbaRTSL_NorRead
 *
 *  @Description:: Read Nor flash memory
 *
 *  @Input      ::
 *
 *  @Output     :: none
 *
 *  @Return     :: none
 */
UINT32 AmbaRTSL_NOR_Readbyte(UINT32 Offset, UINT32 ByteCount, UINT8 *pDataBuf)
{
    UINT32 RetStatus;
    UINT32 pCount, pOffset = 0;

    RetStatus = AmbaRTSL_NorDeviceBusSwitch(AmbaRTSL_NorSpiCtrl.pNorDevInfo->Read.DataLane);
    if (RetStatus == OK) {
        while (pOffset < ByteCount) {
            pCount = ByteCount - pOffset;
            if (pCount >= AMBA_SPINOR_DMA_BUF_SIZE) {
                RetStatus = AmbaRTSL_NorSpiRead(Offset + pOffset, AMBA_SPINOR_DMA_BUF_SIZE, &pDataBuf[pOffset]);
                if (RetStatus == OK) {
                    pOffset += AMBA_SPINOR_DMA_BUF_SIZE;
                }
            } else {
                RetStatus = AmbaRTSL_NorSpiRead(Offset + pOffset, pCount, &pDataBuf[pOffset]);
                if (RetStatus == OK) {
                    pOffset += pCount;
                }
            }

            if (RetStatus != OK) {
                break;
            }
        }
    }

    return RetStatus;
}

/*
 *  @RoutineName:: AmbaRTSL_NorSpiWrite
 *
 *  @Description:: Program Nor flash memory
 *
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
 *
 */
static UINT32 AmbaRTSL_NorSpiWrite(UINT32 Offset, UINT32 ByteCount, const UINT8 *pDataBuf)
{
    UINT32 RetVal = SPINOR_ERR_NONE;
    const UINT8 *pWorkBuf = pDataBuf;
    ULONG MainAddr = 0x0;
    UINT32 ByteWrite = ByteCount;

    if (ByteWrite <= AMBA_SPINOR_DMA_BUF_SIZE) {
        if (AmbaWrap_memcpy(&MainAddr, &pDataBuf, sizeof(MainAddr)) != OK) { /* Do nothing */ };
        /* If buf align to 32 bit , use SPI-DMA transfer */
        if ((MainAddr & (CACHE_LINE_SIZE - 1U)) != 0U) {
            if (AmbaWrap_memcpy(AmbaRTSL_NorSpiCtrl.pBuf, pDataBuf, ByteWrite) != OK) { /* Do nothing */ };
            pWorkBuf = AmbaRTSL_NorSpiCtrl.pBuf;
        } else {
            pWorkBuf = pDataBuf;
        }
    } else {
        RetVal = SPINOR_ERR_ARG;
    }

    if (RetVal == SPINOR_ERR_NONE) {
#ifdef CONFIG_SPINOR_DMA_ENABLE
        UINT32 Tail, Bulk, Remain, AlignSize = 64U;
        Tail = ByteWrite % AlignSize;
        Bulk = ByteWrite / AlignSize;

        if (Bulk >= 1U) {
            (void)AmbaRTSL_NorWriteEnable();
            AmbaRTSL_NorSpiWriteDMA(Offset, ByteWrite - Tail, pWorkBuf);
            /* Wait fo DMA operation done interrupt */
            AmbaRTSL_NorWaitDMA(AmbaRTSL_NorSpiCtrl.NorTxDMAChannel);
            AmbaCSL_NorDisableTxDMA();
            RetVal = AmbaNOR_FlashCheckStatus(AmbaRTSL_NorSpiCtrl.pNorDevInfo->ProgramFailFlag);
        }

        if (0x0U != Tail) {
            (void)AmbaRTSL_NorWriteEnable();
            AmbaRTSL_NorSpiWriteSetup(Offset + (Bulk * AlignSize), Tail, &pWorkBuf[Bulk * AlignSize], 0);
            /* Wait issue program operation done */
            AmbaRTSL_NorWaitInt();
        }
        do {
            Remain = AmbaCSL_NorGetTxFifoDataSize();
            if (Remain != 0U) {
                AmbaDelayCycles(0xfU);
            }
        } while(Remain != 0U);
#else
        /* ---- NON-DMA FIFO mode ---- */
        UINT32 i = 0, FifoSize = AMBA_SPINOR_FIFO_SIZE;

        while (ByteWrite > FifoSize) {
            (void)AmbaRTSL_NorWriteEnable();
            AmbaRTSL_NorSpiWriteSetup(Offset + (i * FifoSize), FifoSize,  &pWorkBuf[i * FifoSize], 0);
            /* Wait issue program operation done */
            AmbaRTSL_NorWaitInt();

            RetVal = AmbaNOR_FlashCheckStatus(AmbaRTSL_NorSpiCtrl.pNorDevInfo->ProgramFailFlag);
            if (RetVal != OK) {
                break;
            }
            while (0U != AmbaCSL_NorGetTxFifoDataSize()) {
                AmbaDelayCycles(0xfU);
            }
            i++;
            ByteWrite -= FifoSize;
        }

        if (ByteWrite != 0U) {
            (void)AmbaRTSL_NorWriteEnable();
            AmbaRTSL_NorSpiWriteSetup(Offset + (i * FifoSize), ByteWrite,  &pWorkBuf[i * FifoSize], 0);
            /* Wait issue program operation done */
            AmbaRTSL_NorWaitInt();
            while (0U != AmbaCSL_NorGetTxFifoDataSize()) {
                AmbaDelayCycles(0xfU);
            }
        }
#endif
        RetVal = AmbaNOR_FlashCheckStatus(AmbaRTSL_NorSpiCtrl.pNorDevInfo->ProgramFailFlag);
    }

    return RetVal;
}

/*
 *  @RoutineName:: AmbaRTSL_NorProgram
 *
 *  @Description:: Program Nor flash memory
 *
 *  @Input      ::
 *          UINT32 Offset        : Block to be programmed
 *          UINT32 ByteCount     : Size of Data in Byte
 *          UINT8 *pDataBuf     : Pointer to the source data
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          UINT32 : OK(0)/NG(-1)
 *-*/
UINT32 AmbaRTSL_NorProgram(UINT32 Offset, UINT32 ByteCount, UINT8 *pDataBuf)
{
    UINT32 Pages, FirstPageSize, LastPageSize, PageOffset;
    UINT32 PageSize = AmbaRTSL_NorSpiCtrl.ProgramPageSize, DataIdx = 0U;
    UINT32 i, RetStatus = OK;
    UINT32 OffsetTmp = Offset;

    AmbaMisra_TouchUnused(&pDataBuf);
    AmbaMisra_TouchUnused(pDataBuf);

    if (PageSize == 0x0U) {
        RetStatus = SPINOR_ERR_ARG;
    } else {
        RetStatus = AmbaRTSL_NorDeviceBusSwitch(AmbaRTSL_NorSpiCtrl.pNorDevInfo->Program.DataLane);

        if (RetStatus == OK) {
            PageOffset = OffsetTmp & (PageSize - 1U);
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
            }

            if (0x0U != FirstPageSize) {
                RetStatus = AmbaRTSL_NorSpiWrite(OffsetTmp, FirstPageSize, &pDataBuf[DataIdx]);
                if (RetStatus == OK) {
                    OffsetTmp += FirstPageSize;
                    DataIdx += FirstPageSize;
                }
            }

            if (RetStatus == OK) {
                for(i = 0U; i < Pages; i++) {
                    RetStatus = AmbaRTSL_NorSpiWrite(OffsetTmp, PageSize, &pDataBuf[DataIdx]);
                    if (RetStatus != OK) {
                        break;
                    }
                    OffsetTmp += PageSize;
                    DataIdx += PageSize;
                }
            }

            if ((RetStatus == OK) && (0x0U != LastPageSize)) {
                RetStatus = AmbaRTSL_NorSpiWrite(OffsetTmp, LastPageSize, &pDataBuf[DataIdx]);
            }
        }
    }
    return RetStatus;
}
/*
 *  @RoutineName:: AmbaRTSL_NorSetSPISetting
 *
 *  @Description:: Configure the SPI interface
 *
 *  @Input      ::
 *      pSpiReg:    pointer to SPI hardware registers
 *      pSpiConfig: pointer to SPI configurations
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
void AmbaRTSL_NorSetSPISetting(const AMBA_SERIAL_SPI_CONFIG_s *SpiConfig)
{
    /* Config SPI Interface */
    AmbaCSL_NorSpiSetChipSelect(SpiConfig->ChipSelect);
    if (0x0U != SpiConfig->FlowControl) {
        AmbaCSL_NorEnableFlowControl();
    }
    AmbaCSL_NorSpiSetHoldPing(SpiConfig->HoldTime);
    //AmbaCSL_NorSpiSetSpiclkpolarity();
    AmbaCSL_NorSpiSetRxSampleDelay(SpiConfig->RxSampleDelay);

    /* Reset TxFIFO and RxFIFO */
    AmbaRTSL_ResetFIFO();
    AmbaCSL_NorSetTxFifoThreshold(SpiConfig->TxThresholdLevel);
    AmbaCSL_NorSetRxFifoThreshold(SpiConfig->RxThresholdLevel);
}

/*
 *  @RoutineName:: AmbaRTSL_NorInit
 *
 *  @Description:: Initialize the Nor flash
 *
 *  @Input      ::
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
void AmbaRTSL_NorInit(void)
{
#if defined(CONFIG_DEVICE_TREE_SUPPORT)
    ULONG BaseAddrDTB;
#endif

#ifndef CONFIG_QNX
    static UINT8 pNorBuf[AMBA_SPINOR_DMA_BUF_SIZE] __attribute__((aligned(CACHE_LINE_SIZE))) __attribute__((section(".bss.noinit")));
    AMBA_INT_CONFIG_s IntConfig = {
        .TriggerType = INT_TRIG_HIGH_LEVEL,
#ifndef CONFIG_ENABLE_AMBALINK
        .IrqType     = INT_TYPE_IRQ,
#else
        .IrqType     = INT_TYPE_FIQ,
#endif
        .CpuTargets  = 1U,
    };

#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
    IntNo = AMBA_INT_SPI_ID109_SPI_NOR;
    base_addr = AMBA_CA53_NOR_SPI_BASE_ADDR;
#elif defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
    IntNo = AMBA_INT_SPI_ID105_SPI_NOR;
    base_addr = AMBA_CORTEX_A76_NOR_SPI_BASE_ADDR;
#else
    IntNo = AMBA_INT_SPI_ID105_SPI_NOR;
    base_addr = AMBA_CORTEX_A53_NOR_SPI_BASE_ADDR;
#endif
    AmbaMisra_TouchUnused(&IntNo);
    AmbaMisra_TypeCast(&pAmbaNOR_Reg, &base_addr);

#if defined(CONFIG_DEVICE_TREE_SUPPORT)
    BaseAddrDTB = IO_UtilityFDTPropertyU32Quick(0, "ambarella,spinor", "reg", 0U);
    IntNo       = IO_UtilityFDTPropertyU32Quick(0, "ambarella,spinor", "interrupts", 1U) + 32U;

    if (BaseAddrDTB != 0U) {
#if defined (CONFIG_SOC_CV5)|| defined (CONFIG_SOC_CV52)
    BaseAddrDTB |= AMBA_CORTEX_A76_APB_VIRT_BASE_ADDR;
#endif
        base_addr = BaseAddrDTB;
        AmbaMisra_TypeCast(&pAmbaNOR_Reg, &base_addr);
    }
#endif

    /* Register the NOR ISR. */
    /* Configure the VIC for level trigger high */
    (void)AmbaRTSL_GicIntDisable(IntNo);
    (void)AmbaRTSL_GicIntConfig(IntNo, &IntConfig, AmbaRTSL_NorTranDoneIsr, 0U);
    (void)AmbaRTSL_GicIntEnable(IntNo);
    AmbaRTSL_NorSpiCtrl.pBuf = pNorBuf;
#else
    ULONG Addr;
    extern int get_fio_work_buf(ULONG *pAddr, UINT32 Size);

    get_fio_work_buf(&Addr, AMBA_SPINOR_DMA_BUF_SIZE);
    AmbaMisra_TypeCast(&AmbaRTSL_NorSpiCtrl.pBuf, &Addr);
#endif /* CONFIG_QNX */
#if defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
    (void)AmbaCSL_NorSetDqsDelay(0U, 0U);
#endif
}

#ifdef AMBA_SPINOR_OCT_CMD_ADDR_ENABLE
static void AmbaRTSL_SPINORSetup8bitDevice(void)
{
    UINT8 Status = 0;
#ifndef CONFIG_SPINOR_DEFAULT_MODE_OCT
    AmbaRTSL_NorSpiCtrl.pNorDevInfo->Read.DataLane = 0;
#endif
    (void)AmbaRTSL_NorWriteEnable();
    AmbaRTSL_NorSetExtAddr();
    AmbaRTSL_NorWaitInt();

    //AmbaRTSL_NorReadReg(0x0, 0x8U, &Status);

    Status = 0xE7U;
    AmbaRTSL_NorWriteVcr(0x0, &Status);

    AmbaRTSL_NorSpiCtrl.pNorDevInfo->Read.DataLane = AMBA_NORSPI_DATA_8_LANE;

    //AmbaRTSL_NorReadReg(0x0, 0x8U, &Status);
}
#endif

#ifndef CONFIG_QNX
/*
 *  @RoutineName:: AmbaRTSL_SpiNORInit
 *
 *  @Description:: Initialize the Nor flash
 *
 *  @Input      ::
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
UINT32 AmbaRTSL_SpiNORConfig(AMBA_NOR_SPI_CONFIG_s *pNorSpiConfig)
{
    UINT32 RetVal;
    UINT32 Delay = AmbaRTSL_PllGetNumCpuCycleUs();

    AmbaRTSL_NorInit();

    RetVal = AmbaRTSL_NorSpiConfig(pNorSpiConfig);
    AmbaCSL_NorClearAllIrqStatus();     // Clear IRQ status for BL2
    if (RetVal == OK) {
        /* Software reset the SpiNOR Device */
        if (AmbaRTSL_NorSpiCtrl.pNorDevInfo->ResetEnable != 0U) {
            AmbaRTSL_NorResetEnable();
            AmbaRTSL_NorWaitInt();
        }
        AmbaRTSL_NorReset();
        AmbaRTSL_NorWaitInt();

        /* Wait Trph SPINOR device execute time after SoftReset */
        AmbaDelayCycles(SPINOR_RESET_TIME * Delay);
#ifdef AMBA_SPINOR_OCT_CMD_ADDR_ENABLE
        if (AmbaRTSL_NorSpiCtrl.pNorDevInfo->Read.DataLane == AMBA_NORSPI_DATA_8_LANE) {
            AmbaRTSL_SPINORSetup8bitDevice();
        }
#endif
        /* Get the Maker ID and Device ID */
        AmbaRTSL_NorReadIDCmd();
        AmbaRTSL_NorWaitInt();
        AmbaRTSL_NorReadFIFO(1U, &AmbaRTSL_NorSpiCtrl.ManufactuerID);
        AmbaRTSL_NorReadFIFO(1U, &AmbaRTSL_NorSpiCtrl.DeviceID);

        if (pNorSpiConfig->pNorSpiDevInfo->AddrByte == AMBA_NORSPI_4BYTE_ADDR) {
            (void)AmbaRTSL_NorWriteEnable();
            AmbaRTSL_NorSetExtAddr();
            AmbaRTSL_NorWaitInt();
        }
    }

    return RetVal;
}

/*
 *  @RoutineName:: AmbaRTSL_SpiNORDeviceReset
 *
 *  @Description:: Reset the Nor flash
 *
 *  @Input      ::
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
void AmbaRTSL_SpiNORDeviceReset(void)
{
    AMBA_NOR_FLOWCTRL_REG_s FlowCtrl;
    UINT32 Value = 0xBBF80801U;

    if (AmbaWrap_memcpy(&FlowCtrl, &Value, (SIZE_t)(sizeof(AMBA_NOR_FLOWCTRL_REG_s) & 0xffffffffU)) != OK) { /* Do nothing */ };

    /* Software reset the SpiNOR Device */
    if (AmbaRTSL_NorSpiCtrl.pNorDevInfo->ResetEnable != 0U) {
        AmbaRTSL_NorResetEnable();
        AmbaRTSL_NorWaitInt();
    }
    AmbaRTSL_NorReset();
    AmbaRTSL_NorWaitInt();
    /* Wait Trph SPINOR device execute time after SoftReset */
    (void)AmbaRTSL_RctTimerWait(SPINOR_RESET_TIME);

    /* Choose the Ent Clk to source of SSI2 */
    AmbaCSL_PllSetSpiNorRefClkSelect(0);
    AmbaCSL_NorSpiSetFlowControl(FlowCtrl);
    AmbaCSL_PllSetSpiNorClkDivider(0x2U);
}

/*
 *  @RoutineName:: AmbaSPINOR_RestoreRebootClkSetting
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
void AmbaSPINOR_RestoreRebootClkSetting(void)
{
    AMBA_NOR_FLOWCTRL_REG_s FlowCtrl;
    UINT32 Value = 0xBBF80801U;

    if (AmbaWrap_memcpy(&FlowCtrl, &Value, (SIZE_t)(sizeof(AMBA_NOR_FLOWCTRL_REG_s) & 0xffffffffU)) != OK) { /* Do nothing */ };

    /* Reset Nor Flash Device for clear  the extended address bit */
    (void)AmbaRTSL_GicIntDisable(IntNo);
    if (AmbaRTSL_NorSpiCtrl.pNorDevInfo->ResetEnable != 0U) {
        AmbaRTSL_NorResetEnable();
        AmbaRTSL_NorWaitInt();
    }
    AmbaRTSL_NorReset();
    AmbaRTSL_NorWaitInt();

    /* Wait Trph SPINOR device execute time after SoftReset */
    (void)AmbaRTSL_RctTimerWait(SPINOR_RESET_TIME);

    /* Choose the Ent Clk to source of SSI2 */
    AmbaCSL_PllSetSpiNorRefClkSelect(0);
    AmbaCSL_NorSpiSetFlowControl(FlowCtrl);
    AmbaCSL_PllSetSpiNorClkDivider(0x2U);
}
#endif
