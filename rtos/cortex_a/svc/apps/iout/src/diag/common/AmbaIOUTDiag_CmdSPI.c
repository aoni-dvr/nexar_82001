/**
 *  @file AmbaIOUTDiag_CmdSPI.c
 *
 *  @copyright Copyright (c) 2020 Ambarella, Inc.
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
 *  @details SPI diagnostic command
 *
 */

#include "AmbaTypes.h"
#include "AmbaMisraFix.h"

#include "AmbaKAL.h"
#include "AmbaPrint.h"
#include "AmbaShell.h"
#include "AmbaUtility.h"

#include "AmbaIOUTDiag.h"
#include "AmbaSPI.h"

#define DIAG_SPI_NUM_DATA   (196U)

typedef struct {
    UINT32 SlaveMask;
    UINT32 TransferType;
    UINT32 UseDma;
    AMBA_SPI_CONFIG_s SpiConfig;
} DIAG_SPI_CTRL_s;

static DIAG_SPI_CTRL_s IoDiagSpiCtrl[AMBA_NUM_SPI_MASTER] = {
    [AMBA_SPI_MASTER0] = {
        .SlaveMask     = 0x01U,
        .TransferType  = 0U,
        .UseDma        = 0U,
        .SpiConfig     = {
            .BaudRate        = 8000000U,
            .CsPolarity      = AMBA_SPI_CHIP_SELECT_ACTIVE_LOW,
            .ClkMode         = AMBA_SPI_CPOL_HIGH_CPHA_HIGH,
            .ClkStretch      = 0U,
            .DataFrameSize   = 8U,
            .FrameBitOrder   = AMBA_SPI_TRANSFER_MSB_FIRST,
        },
    },
    [AMBA_SPI_MASTER1] = {
        .SlaveMask     = 0x01U,
        .TransferType  = 0U,
        .UseDma        = 0U,
        .SpiConfig     = {
            .BaudRate        = 8000000U,
            .CsPolarity      = AMBA_SPI_CHIP_SELECT_ACTIVE_LOW,
            .ClkMode         = AMBA_SPI_CPOL_HIGH_CPHA_HIGH,
            .ClkStretch      = 0U,
            .DataFrameSize   = 8U,
            .FrameBitOrder   = AMBA_SPI_TRANSFER_MSB_FIRST,
        },
    },
    [AMBA_SPI_MASTER2] = {
        .SlaveMask     = 0x01U,
        .TransferType  = 0U,
        .UseDma        = 0U,
        .SpiConfig     = {
            .BaudRate        = 8000000U,
            .CsPolarity      = AMBA_SPI_CHIP_SELECT_ACTIVE_LOW,
            .ClkMode         = AMBA_SPI_CPOL_HIGH_CPHA_HIGH,
            .ClkStretch      = 0U,
            .DataFrameSize   = 8U,
            .FrameBitOrder   = AMBA_SPI_TRANSFER_MSB_FIRST,
        },
    },
    [AMBA_SPI_MASTER3] = {
        .SlaveMask     = 0x01U,
        .TransferType  = 0U,
        .UseDma        = 0U,
        .SpiConfig     = {
            .BaudRate        = 8000000U,
            .CsPolarity      = AMBA_SPI_CHIP_SELECT_ACTIVE_LOW,
            .ClkMode         = AMBA_SPI_CPOL_HIGH_CPHA_HIGH,
            .ClkStretch      = 0U,
            .DataFrameSize   = 8U,
            .FrameBitOrder   = AMBA_SPI_TRANSFER_MSB_FIRST,
        },
    },

#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS) || defined(CONFIG_SOC_CV5)
    [AMBA_SPI_MASTER4] = {
        .SlaveMask     = 0x01U,
        .TransferType  = 0U,
        .UseDma        = 0U,
        .SpiConfig     = {
            .BaudRate        = 8000000U,
            .CsPolarity      = AMBA_SPI_CHIP_SELECT_ACTIVE_LOW,
            .ClkMode         = AMBA_SPI_CPOL_HIGH_CPHA_HIGH,
            .ClkStretch      = 0U,
            .DataFrameSize   = 8U,
            .FrameBitOrder   = AMBA_SPI_TRANSFER_MSB_FIRST,
        },
    },

    [AMBA_SPI_MASTER5] = {
        .SlaveMask     = 0x01U,
        .TransferType  = 0U,
        .UseDma        = 0U,
        .SpiConfig     = {
            .BaudRate        = 8000000U,
            .CsPolarity      = AMBA_SPI_CHIP_SELECT_ACTIVE_LOW,
            .ClkMode         = AMBA_SPI_CPOL_HIGH_CPHA_HIGH,
            .ClkStretch      = 0U,
            .DataFrameSize   = 8U,
            .FrameBitOrder   = AMBA_SPI_TRANSFER_MSB_FIRST,
        },
    },
#endif
};

static AMBA_KAL_EVENT_FLAG_t IoDiagSpiEventFlag;
static DIAG_SPI_CTRL_s *pIoDiagSpiSlaveCtrl;

static void IoDiag_PrintFormattedInt(const char *pFmtString, UINT32 Value, AMBA_SHELL_PRINT_f PrintFunc)
{
    char StrBuf[64];
    UINT32 ArgUINT32[2];

    ArgUINT32[0] = Value;
    (void)AmbaUtility_StringPrintUInt32(StrBuf, sizeof(StrBuf), pFmtString, 1U, ArgUINT32);
    PrintFunc(StrBuf);
}

static void IoDiag_SpiIllustrate(const AMBA_SPI_CONFIG_s *pSpiConfig, AMBA_SHELL_PRINT_f PrintFunc)
{
    if ((pSpiConfig->ClkMode & 0x1U) != 0U) {
        // CPHA = 1
        if (pSpiConfig->CsPolarity != 0U) {
            // Active High
            PrintFunc("[Diag][SPI]                |    __________//________________//____________    \n");
            PrintFunc("[Diag][SPI]  SS (CPHA = 1) |   |     :     :     :     :     :     :      |   \n");
            PrintFunc("[Diag][SPI]                |___|     :     :     :     :     :     :      |___\n");
            PrintFunc("[Diag][SPI]                |         :     :     :     :     :     :          \n");
        } else {
            // Active Low
            PrintFunc("[Diag][SPI]                |___      :     :     :     :     :     :       ___\n");
            PrintFunc("[Diag][SPI]  SS (CPHA = 1) |   |     :     :     :     :     :     :      |   \n");
            PrintFunc("[Diag][SPI]                |   |__________//________________//____________|   \n");
            PrintFunc("[Diag][SPI]                |         :     :     :     :     :     :          \n");
        }

        if (pSpiConfig->ClkMode < 2U) {
            // CPOL = 0
            PrintFunc("[Diag][SPI]                |       __:   __:   __:   __:   __:   __:          \n");
            PrintFunc("[Diag][SPI] SCK (CPOL = 0) |      |  |  |  |  |  |  |  |  |  |  |  |          \n");
            PrintFunc("[Diag][SPI]                |______|  |__|  |__|  |__|  |__|  |__|  |__________\n");
        } else {
            // CPOL = 1
            PrintFunc("[Diag][SPI]                |______   :__   :__   :__   :__   :__   :__________\n");
            PrintFunc("[Diag][SPI] SCK (COPL = 1) |      |  |  |  |  |  |  |  |  |  |  |  |          \n");
            PrintFunc("[Diag][SPI]                |      |__|  |__|  |__|  |__|  |__|  |__|          \n");
        }

        if (pSpiConfig->FrameBitOrder != 0U) {
            // LSB first
            PrintFunc("[Diag][SPI]                |       __:__ __:__ __:__ __:__ __:__ __:__        \n");
            PrintFunc("[Diag][SPI] MISO/MOSI      |      | LSB |     | MSB | LSB |     | MSB |       \n");
            PrintFunc("[Diag][SPI]                |______|__0__|_//__|_");
            IoDiag_PrintFormattedInt("%2u", pSpiConfig->DataFrameSize - 1U, PrintFunc);
            PrintFunc("__|__0__|_//__|_");
            IoDiag_PrintFormattedInt("%2u", pSpiConfig->DataFrameSize - 1U, PrintFunc);
            PrintFunc("__|_______\n");
        } else {
            // MSB first
            PrintFunc("[Diag][SPI]                |       __:__ __:__ __:__ __:__ __:__ __:__        \n");
            PrintFunc("[Diag][SPI] MISO/MOSI      |      | MSB |     | LSB | MSB |     | LSB |       \n");
            PrintFunc("[Diag][SPI]                |______|_");
            IoDiag_PrintFormattedInt("%2u", pSpiConfig->DataFrameSize - 1U, PrintFunc);
            PrintFunc("__|_//__|__0__|_");
            IoDiag_PrintFormattedInt("%2u", pSpiConfig->DataFrameSize - 1U, PrintFunc);
            PrintFunc("__|_//__|__0__|_______\n");
        }
    } else {
        // CPHA = 0
        if (pSpiConfig->CsPolarity != 0U) {
            // Active High
            PrintFunc("[Diag][SPI]                |    _______//_________    _______//___________    \n");
            PrintFunc("[Diag][SPI]  SS (CPHA = 0) |   |  :     :     :   |  |  :     :     :     |   \n");
            PrintFunc("[Diag][SPI]                |___|  :     :     :   |__|  :     :     :     |___\n");
            PrintFunc("[Diag][SPI]                |      :     :     :         :     :     :         \n");
        } else {
            // Active Low
            PrintFunc("[Diag][SPI]                |___   :     :     :    __   :     :     :      ___\n");
            PrintFunc("[Diag][SPI]  SS (CPHA = 0) |   |  :     :     :   |  |  :     :     :     |   \n");
            PrintFunc("[Diag][SPI]                |   |_______//_________|  |_______//___________|   \n");
            PrintFunc("[Diag][SPI]                |      :     :     :         :     :     :         \n");
        }

        if (pSpiConfig->ClkMode < 2U) {
            // CPOL = 0
            PrintFunc("[Diag][SPI]                |      :__   :__   :__       :__   :__   :__       \n");
            PrintFunc("[Diag][SPI] SCK (CPOL = 0) |      |  |  |  |  |  |      |  |  |  |  |  |      \n");
            PrintFunc("[Diag][SPI]                |______|  |__|  |__|  |______|  |__|  |__|  |______\n");
        } else {
            // CPOL = 1
            PrintFunc("[Diag][SPI]                |______:   __:   __:   ______:   __:   __:   ______\n");
            PrintFunc("[Diag][SPI] SCK (COPL = 1) |      |  |  |  |  |  |      |  |  |  |  |  |      \n");
            PrintFunc("[Diag][SPI]                |      |__|  |__|  |__|      |__|  |__|  |__|      \n");
        }

        if (pSpiConfig->FrameBitOrder != 0U) {
            // LSB first
            PrintFunc("[Diag][SPI]                |    __:__ __:__ __:__     __:__ __:__ __:__       \n");
            PrintFunc("[Diag][SPI] MISO/MOSI      |   | LSB |     | MSB |   | LSB |     | MSB |      \n");
            PrintFunc("[Diag][SPI]                |___|__0__|_//__|_");
            IoDiag_PrintFormattedInt("%2u", pSpiConfig->DataFrameSize - 1U, PrintFunc);
            PrintFunc("__|___|__0__|_//__|_");
            IoDiag_PrintFormattedInt("%2u", pSpiConfig->DataFrameSize - 1U, PrintFunc);
            PrintFunc("__|______\n");
        } else {
            // MSB first
            PrintFunc("[Diag][SPI]                |    __:__ __:__ __:__     __:__ __:__ __:__       \n");
            PrintFunc("[Diag][SPI] MISO/MOSI      |   | MSB |     | LSB |   | MSB |     | LSB |      \n");
            PrintFunc("[Diag][SPI]                |___|_");
            IoDiag_PrintFormattedInt("%2u", pSpiConfig->DataFrameSize - 1U, PrintFunc);
            PrintFunc("__|_//__|__0__|___|_");
            IoDiag_PrintFormattedInt("%2u", pSpiConfig->DataFrameSize - 1U, PrintFunc);
            PrintFunc("__|_//__|__0__|______\n");
        }
    }
}

static void IoDIag_SpiShowConfig(UINT32 SpiChan, AMBA_SHELL_PRINT_f PrintFunc)
{
    const DIAG_SPI_CTRL_s *pSpiCtrl = &IoDiagSpiCtrl[SpiChan];
    const AMBA_SPI_CONFIG_s *pSpiConfig = &pSpiCtrl->SpiConfig;

    PrintFunc("[Diag][SPI] Master ID     :");
    IoDiag_PrintFormattedInt(" %u\n", SpiChan, PrintFunc);

    PrintFunc("[Diag][SPI] Slave Mask    :");
    IoDiag_PrintFormattedInt(" 0x%02X\n", pSpiCtrl->SlaveMask, PrintFunc);

    PrintFunc("[Diag][SPI] Clock Mode    :");
    IoDiag_PrintFormattedInt(" 0x%02X\n", pSpiConfig->ClkMode, PrintFunc);

    if (pSpiConfig->CsPolarity == AMBA_SPI_CHIP_SELECT_ACTIVE_LOW) {
        PrintFunc("[Diag][SPI] CS Polarity   : active in low\n");
    } else {
        PrintFunc("[Diag][SPI] CS Polarity   : active in high\n");
    }

    PrintFunc("[Diag][SPI] DataFrameSize :");
    IoDiag_PrintFormattedInt(" %u bits\n", pSpiConfig->DataFrameSize, PrintFunc);

    if (pSpiConfig->FrameBitOrder == AMBA_SPI_TRANSFER_MSB_FIRST) {
        PrintFunc("[Diag][SPI] Bit-Order     : MSB first\n");
    } else {
        PrintFunc("[Diag][SPI] Bit-Order     : LSB first\n");
    }

    if (pSpiCtrl->TransferType == 1U) {
        PrintFunc("[Diag][SPI] Transfer Type : Tx only transfer\n");
    } else if (pSpiCtrl->TransferType == 2U) {
        PrintFunc("[Diag][SPI] Transfer Type : Rx only transfer\n");
    } else {
        PrintFunc("[Diag][SPI] Transfer Type : Tx & Rx transfer\n");
    }

    if (pSpiCtrl->UseDma == 0U) {
        PrintFunc("[Diag][SPI] DMA Function  : disabled\n");
    } else {
        PrintFunc("[Diag][SPI] DMA Function  : enabled\n");
    }

    IoDiag_SpiIllustrate(pSpiConfig, PrintFunc);
}

static void IoDiag_SpiMasterSimpleTest(UINT32 SpiChan, UINT32 Size, AMBA_SHELL_PRINT_f PrintFunc)
{
    static UINT8 IoDiagSpiMaster8bitBuf[2][DIAG_SPI_NUM_DATA];
    static UINT16 IoDiagSpiMaster16bitBuf[2][DIAG_SPI_NUM_DATA];
    DIAG_SPI_CTRL_s *pSpiCtrl;
    AMBA_SPI_CONFIG_s *pSpiConfig;
    UINT8 *pTxUINT8 = IoDiagSpiMaster8bitBuf[0], *pRxUINT8 = IoDiagSpiMaster8bitBuf[1];
    UINT16 *pTxUINT16 = IoDiagSpiMaster16bitBuf[0], *pRxUINT16 = IoDiagSpiMaster16bitBuf[1];
    UINT32 NumData = 0U;
    UINT32 i, ActualSize = 0, RetVal = OK;

    if (SpiChan < AMBA_NUM_SPI_MASTER) {
        pSpiCtrl = &IoDiagSpiCtrl[SpiChan];
        pSpiConfig = &pSpiCtrl->SpiConfig;

        if (Size == 0U) {
            NumData = 10U;
            pTxUINT8[0] = 0x11U;
            pTxUINT8[1] = 0x22U;
            pTxUINT8[2] = 0x33U;
            pTxUINT8[3] = 0x44U;
            pTxUINT8[4] = 0x55U;
            pTxUINT8[5] = 0x66U;
            pTxUINT8[6] = 0x77U;
            pTxUINT8[7] = 0x88U;
            pTxUINT8[8] = 0x99U;
            pTxUINT8[9] = 0xAAU;

        } else {
            NumData = Size;
            for (i = 0; i < NumData; i++) {
                pTxUINT8[i] = (UINT8)i;
                pTxUINT16[i] = (UINT16)i | (i << 8);
                pRxUINT8[i] = 0U;
                pRxUINT16[i] = 0U;
            }
        }

        /* Notify slave task (if exist) for loopback test */
        (void)AmbaKAL_EventFlagSet(&IoDiagSpiEventFlag, NumData);
        (void)AmbaKAL_TaskSleep(100U);  /* configure spi slave first */

        if (pSpiCtrl->UseDma == 0U) {
            if (pSpiConfig->DataFrameSize <= 8U) {
                RetVal = AmbaSPI_MasterTransferD8(SpiChan, pSpiCtrl->SlaveMask, pSpiConfig, NumData,
                                                  pTxUINT8, pRxUINT8, &ActualSize, AMBA_KAL_WAIT_FOREVER);
            } else {
                RetVal = AmbaSPI_MasterTransferD16(SpiChan, pSpiCtrl->SlaveMask, pSpiConfig, NumData,
                                                   pTxUINT16, pRxUINT16, &ActualSize, AMBA_KAL_WAIT_FOREVER);
            }
        } else {
            if (pSpiConfig->DataFrameSize <= 8U) {
                RetVal = AmbaSPI_MasterDmaTransferD8(SpiChan, pSpiCtrl->SlaveMask, pSpiConfig, NumData,
                                                     pTxUINT8, pRxUINT8, &ActualSize, AMBA_KAL_WAIT_FOREVER);
            } else {
                RetVal = AmbaSPI_MasterDmaTransferD16(SpiChan, pSpiCtrl->SlaveMask, pSpiConfig, NumData,
                                                      pTxUINT16, pRxUINT16, &ActualSize, AMBA_KAL_WAIT_FOREVER);
            }
        }

        (void)AmbaKAL_TaskSleep(100U);  /* let spi slave task print first */
        IoDIag_SpiShowConfig(SpiChan, PrintFunc);

        if (RetVal == SPI_ERR_NONE) {
            PrintFunc("[Diag][SPI] Result = OK.\n");
            if (pSpiConfig->DataFrameSize <= 8U) {
                for (i = 0; i < NumData; i ++) {
                    PrintFunc("[Diag][SPI]");
                    IoDiag_PrintFormattedInt(" Data %u,", i, PrintFunc);
                    if (pRxUINT8 == NULL) {
                        if (pTxUINT8 != NULL) {
                            IoDiag_PrintFormattedInt(" TX=0x%02X\n", (UINT32)pTxUINT8[i], PrintFunc);
                        } else {
                            PrintFunc(" N/A\n");
                        }
                    } else {
                        if (pTxUINT8 != NULL) {
                            IoDiag_PrintFormattedInt(" TX=0x%02X,", (UINT32)pTxUINT8[i], PrintFunc);
                        }
                        IoDiag_PrintFormattedInt(" RX=0x%02X\n", (UINT32)pRxUINT8[i], PrintFunc);
                    }
                }
            } else {
                for (i = 0; i < NumData; i ++) {
                    PrintFunc("[Diag][SPI]");
                    IoDiag_PrintFormattedInt(" Data %u,", i, PrintFunc);
                    if (pRxUINT16 == NULL) {
                        if (pTxUINT16 != NULL) {
                            IoDiag_PrintFormattedInt(" TX=0x%04X\n", (UINT32)pTxUINT16[i], PrintFunc);
                        } else {
                            PrintFunc(" N/A\n");
                        }
                    } else {
                        if (pTxUINT16 != NULL) {
                            IoDiag_PrintFormattedInt(" TX=0x%04X,", (UINT32)pTxUINT16[i], PrintFunc);
                        }
                        IoDiag_PrintFormattedInt(" RX=0x%04X\n", (UINT32)pRxUINT16[i], PrintFunc);
                    }
                }

            }
        } else {
            IoDiag_PrintFormattedInt("[Diag][SPI] Result = NG.(0x%X)\n", RetVal, PrintFunc);
        }
    }
}

static void *IoDiag_SpiSlaveTaskEntry(void * Arg)
{
    static UINT8 IoDiagSpiSlave8bitBuf[2][DIAG_SPI_NUM_DATA];
    static UINT16 IoDiagSpiSlave16bitBuf[2][DIAG_SPI_NUM_DATA];

    const AMBA_SPI_CONFIG_s *pSpiConfig;
    UINT8 *pTxUINT8 = IoDiagSpiSlave8bitBuf[0], *pRxUINT8 = IoDiagSpiSlave8bitBuf[1];
    UINT16 *pTxUINT16 = IoDiagSpiSlave16bitBuf[0], *pRxUINT16 = IoDiagSpiSlave16bitBuf[1];
    UINT32 i, ActualFlags = 0U, ActualSize = 0U, RetVal; /* ActualFlags used as NumData here. */
    ULONG ChannelID;
    UINT32 Loop = 1U;

    if (Arg != NULL) {
        AmbaMisra_TouchUnused(Arg);
        AmbaMisra_TypeCast(&ChannelID, &Arg);
    } else {
        ChannelID = AMBA_SPI_SLAVE;
    }

    while (1U == Loop) {
        AmbaPrint_PrintUInt5("[Diag][SPI] Wait for master %u transfer..", ChannelID, 0, 0, 0, 0);
        AmbaPrint_Flush();
        (void)AmbaKAL_EventFlagGet(&IoDiagSpiEventFlag, 0xffffffffU, AMBA_KAL_FLAGS_ANY, AMBA_KAL_FLAGS_CLEAR_AUTO, &ActualFlags, AMBA_KAL_WAIT_FOREVER);

        if (pIoDiagSpiSlaveCtrl != NULL) {
            pSpiConfig = &pIoDiagSpiSlaveCtrl->SpiConfig;

            for (i = 0; i < ActualFlags; i ++) {
                pRxUINT8[i] = 0U;
                pRxUINT16[i] = 0U;
            }

            if (pSpiConfig->DataFrameSize <= 8U) {
                for (i = 0; i < ActualFlags; i ++) {
                    pTxUINT8[i] = (UINT8)(ActualFlags - i);
                }
            } else {
                for (i = 0; i < ActualFlags; i ++) {
                    pTxUINT16[i] = (UINT16)(ActualFlags - i) | ((ActualFlags - i) << 8);
                }
            }

            RetVal = AmbaSPI_SlaveConfig(AMBA_SPI_SLAVE, pSpiConfig);
            if (RetVal == OK) {
                if (pIoDiagSpiSlaveCtrl->UseDma == 0U) {
                    AmbaPrint_PrintUInt5("[Diag][SPISlave] Slave transfer in non-DMA mode", 0, 0, 0, 0, 0);
                    if (pSpiConfig->DataFrameSize <= 8U) {
                        RetVal = AmbaSPI_SlaveTransferD8(AMBA_SPI_SLAVE, ActualFlags, pTxUINT8, pRxUINT8, &ActualSize, 1000U);
                    } else {
                        RetVal = AmbaSPI_SlaveTransferD16(AMBA_SPI_SLAVE, ActualFlags, pTxUINT16, pRxUINT16, &ActualSize, 1000U);
                    }
                } else {
                    AmbaPrint_PrintUInt5("[Diag][SPISlave] Slave transfer in DMA mode", 0, 0, 0, 0, 0);

                    if (pSpiConfig->DataFrameSize <= 8U) {
                        RetVal = AmbaSPI_SlaveDmaTransferD8(AMBA_SPI_SLAVE, ActualFlags, pTxUINT8, pRxUINT8, &ActualSize, 1000U);
                    } else {
                        RetVal = AmbaSPI_SlaveDmaTransferD16(AMBA_SPI_SLAVE, ActualFlags, pTxUINT16, pRxUINT16, &ActualSize, 1000U);
                    }
                }

                AmbaPrint_PrintUInt5("[Diag][SPISlave] Slave transfer %d bytes (%x)", ActualSize, RetVal, 0, 0, 0);
                AmbaPrint_Flush();

                if (ActualSize != 0U) {
                    if (pSpiConfig->DataFrameSize <= 8U) {
                        for (i = 0; i < ActualSize; i ++) {
                            AmbaPrint_PrintUInt5("[Diag][SPISlave] Data %u, TX=0x%02X, RX=0x%02X", i, (UINT32)pTxUINT8[i], (UINT32)pRxUINT8[i], 0, 0);
                        }
                    } else {
                        for (i = 0; i < ActualSize; i ++) {
                            AmbaPrint_PrintUInt5("[Diag][SPISlave] Data %u, TX=0x%04X, RX=0x%04X", i, (UINT32)pTxUINT16[i], (UINT32)pRxUINT16[i], 0, 0);
                        }
                    }
                    AmbaPrint_Flush();
                }
            }
        }
    }

    return NULL;
}

static void IoDiag_SpiLoopBack(UINT32 SpiChan)
{
    static AMBA_KAL_TASK_t IoDiagSpiSlaveTask;
    static UINT8 IoDiagSpiSlaveTaskStack[0x1000];
    static char SpiEventFlagName[24] = "DiagSpiSlaveEventFlag";
    static char spiTaskName[20] = "DiagSpiSlaveTask";
    UINT32 RetVal;
    void * EntryArg;

    if (SpiChan < AMBA_NUM_SPI_MASTER) {
        (void)AmbaKAL_EventFlagCreate(&IoDiagSpiEventFlag, SpiEventFlagName);
        (void)AmbaKAL_EventFlagClear(&IoDiagSpiEventFlag, 0xffffffffU);

        AmbaMisra_TypeCast(&EntryArg, &SpiChan);
        AmbaMisra_TouchUnused(EntryArg);
        RetVal = AmbaKAL_TaskCreate(&IoDiagSpiSlaveTask, spiTaskName, 60U, IoDiag_SpiSlaveTaskEntry, EntryArg, IoDiagSpiSlaveTaskStack, sizeof(IoDiagSpiSlaveTaskStack), AMBA_KAL_DONT_START);
        if (RetVal == KAL_ERR_NONE) {
            pIoDiagSpiSlaveCtrl = &IoDiagSpiCtrl[SpiChan];
            (void)AmbaKAL_TaskSetSmpAffinity(&IoDiagSpiSlaveTask, 0x1U);
            (void)AmbaKAL_TaskResume(&IoDiagSpiSlaveTask);
            (void)AmbaKAL_TaskSleep(100U);  /* Make slave task be suspended on eventflags */
        }
    } else {
        (void)AmbaKAL_TaskTerminate(&IoDiagSpiSlaveTask);
        (void)AmbaKAL_TaskDelete(&IoDiagSpiSlaveTask);
        (void)AmbaKAL_EventFlagDelete(&IoDiagSpiEventFlag);
        pIoDiagSpiSlaveCtrl = NULL;
    }
}

static void IoDiag_SpiCmdUsage(char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    PrintFunc("Usage: ");
    PrintFunc(pArgVector[0]);
    PrintFunc(" <channel> simpletest [start|stop]           - start/stop a simple test monitor task using same config of this master using loopback\n");
    PrintFunc(" <channel> test [start|stop] <size>          - start/stop a test monitor task with assigned size\n");
    PrintFunc(" <channel> config usedma [0/1]               - enable dma mode\n");
    PrintFunc(" <channel> config framesize <size>           - config test frame size\n");
    PrintFunc(" <channel> config baud <clk>                 - config test baud\n");
}

/**
 *  SPI diagnostic command
 *  @param[in] ArgCount Argument count
 *  @param[in] pArgVector Argument vector
 *  @param[in] PrintFunc function to print messages on shell task
 */
void AmbaIOUTDiag_CmdSPI(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 SpiChan;
    UINT32 Size = 0U, UseDma = 0U, Framesize = 8U, Baud = 10000000U;

    if (ArgCount < 2U) {
        IoDiag_SpiCmdUsage(pArgVector, PrintFunc);
    } else {
        (void)AmbaUtility_StringToUInt32(pArgVector[1], &SpiChan);
        if (SpiChan >= AMBA_NUM_SPI_MASTER) {
            PrintFunc("[Diag][SPI] ERROR: Invalid spi master id.\n");
        }

        if (ArgCount >= 3U) {
            if (AmbaUtility_StringCompare(pArgVector[2], "simpletest", 10U) == 0) {
                if (ArgCount >= 4U) {
                    if (AmbaUtility_StringCompare(pArgVector[3], "start", 5U) == 0) {
                        IoDiag_SpiLoopBack(SpiChan);
                        IoDiag_SpiMasterSimpleTest(SpiChan, 0, PrintFunc);
                    }
                    if (AmbaUtility_StringCompare(pArgVector[3], "stop", 4U) == 0) {
                        IoDiag_SpiLoopBack(AMBA_NUM_SPI_MASTER);
                    }
                }
            } else if (AmbaUtility_StringCompare(pArgVector[2], "test", 4U) == 0) {
                if (ArgCount >= 4U) {
                    if (AmbaUtility_StringCompare(pArgVector[3], "start", 5U) == 0) {
                        if (ArgCount >= 5U) {
                            (void)AmbaUtility_StringToUInt32(pArgVector[4], &Size);
                        }
                        IoDiag_SpiLoopBack(SpiChan);
                        IoDiag_SpiMasterSimpleTest(SpiChan, Size, PrintFunc);
                    } else if (AmbaUtility_StringCompare(pArgVector[3], "stop", 4U) == 0) {
                        IoDiag_SpiLoopBack(AMBA_NUM_SPI_MASTER);
                    }
                } else {
                    IoDiag_SpiCmdUsage(pArgVector, PrintFunc);
                }
            } else if (AmbaUtility_StringCompare(pArgVector[2], "config", 6U) == 0) {
                if (ArgCount >= 4U) {
                    if (AmbaUtility_StringCompare(pArgVector[3], "usedma", 6U) == 0) {
                        if (ArgCount >= 5U) {
                            (void)AmbaUtility_StringToUInt32(pArgVector[4], &UseDma);
                            if (UseDma) {
                                UseDma = 1U;
                            } else {
                                UseDma = 0U;
                            }
                            IoDiagSpiCtrl[SpiChan].UseDma = UseDma;
                        }
                        IoDiag_PrintFormattedInt("[Diag][SPI]UseDma = %d\n", UseDma, PrintFunc);
                    } else if (AmbaUtility_StringCompare(pArgVector[3], "framesize", 9U) == 0) {
                        if (ArgCount >= 5U) {
                            (void)AmbaUtility_StringToUInt32(pArgVector[4], &Framesize);
                            if (Framesize <= 8U) {
                                Framesize = 8U;
                            } else {
                                Framesize = 16U;
                            }
                            IoDiagSpiCtrl[SpiChan].SpiConfig.DataFrameSize = Framesize;
                        }
                        IoDiag_PrintFormattedInt("[Diag][SPI]Framesize = %d\n", Framesize, PrintFunc);
                    } else if (AmbaUtility_StringCompare(pArgVector[3], "baud", 4U) == 0) {
                        if (ArgCount >= 5U) {
                            (void)AmbaUtility_StringToUInt32(pArgVector[4], &Baud);
                            IoDiagSpiCtrl[SpiChan].SpiConfig.BaudRate = Baud;
                        }
                        IoDiag_PrintFormattedInt("[Diag][SPI]Baud = %d\n", Baud, PrintFunc);
                    }
                } else {
                    IoDiag_SpiCmdUsage(pArgVector, PrintFunc);
                }
            } else {
                IoDiag_SpiCmdUsage(pArgVector, PrintFunc);
            }
        }
    }
}
