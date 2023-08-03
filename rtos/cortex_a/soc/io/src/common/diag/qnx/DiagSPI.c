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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "hw/ambarella_spi.h"
#include "diag.h"
#include "AmbaKAL.h"
#include "AmbaSPI.h"
#include "AmbaPrint.h"
#include "AmbaMisraFix.h"

#define DIAG_SPI_NUM_DATA (128U)

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
            .ClkMode         = AMBA_SPI_CPOL_LOW_CPHA_LOW,
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
            .ClkMode         = AMBA_SPI_CPOL_LOW_CPHA_LOW,
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
            .ClkMode         = AMBA_SPI_CPOL_LOW_CPHA_LOW,
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
            .ClkMode         = AMBA_SPI_CPOL_LOW_CPHA_LOW,
            .ClkStretch      = 0U,
            .DataFrameSize   = 8U,
            .FrameBitOrder   = AMBA_SPI_TRANSFER_MSB_FIRST,
        },
    },
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
    [AMBA_SPI_MASTER4] = {
        .SlaveMask     = 0x01U,
        .TransferType  = 0U,
        .UseDma        = 0U,
        .SpiConfig     = {
            .BaudRate        = 8000000U,
            .CsPolarity      = AMBA_SPI_CHIP_SELECT_ACTIVE_LOW,
            .ClkMode         = AMBA_SPI_CPOL_LOW_CPHA_LOW,
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
            .ClkMode         = AMBA_SPI_CPOL_LOW_CPHA_LOW,
            .ClkStretch      = 0U,
            .DataFrameSize   = 8U,
            .FrameBitOrder   = AMBA_SPI_TRANSFER_MSB_FIRST,
        },
    },
#endif
};

static AMBA_KAL_EVENT_FLAG_t IoDiagSpiEventFlag;
static DIAG_SPI_CTRL_s *pIoDiagSpiSlaveCtrl;
static AMBA_KAL_MUTEX_t DiagSpiPrintMutex;

typedef struct {
    UINT32 SpiChan;
    UINT32 ArgCount;
    char * const *pArgVector;
} DIAG_SPI_PARAM_s;


//#define AMBA_SPI_CONFIG_B6N
#ifdef AMBA_SPI_CONFIG_B6N
/* SPI configurations (for B6N) */
static AMBA_SPI_CONFIG_s SpiConfig = {
    .BaudRate      = 12000000U,                         /* Transfer BaudRate in Hz */
    .CsPolarity    = AMBA_SPI_CHIP_SELECT_ACTIVE_LOW,   /* Slave select polarity */
    .ClkMode       = AMBA_SPI_CPOL_HIGH_CPHA_HIGH,      /* SPI Protocol mode */
    .ClkStretch    = 0U,                                /* Stretch the duration (us) from the active edge of chip select to the first edge of clock */
    .DataFrameSize = 8U,                                /* Data Frame Size in Bit */
    .FrameBitOrder = AMBA_SPI_TRANSFER_MSB_FIRST,       /* Bit transfer order */
};
#else
/* SPI configurations (for on board slave transfer) */
static AMBA_SPI_CONFIG_s SpiConfig = {
    .BaudRate      = 1000000U,                         /* Transfer BaudRate in Hz */
    .CsPolarity    = AMBA_SPI_CHIP_SELECT_ACTIVE_LOW,   /* Slave select polarity */
    .ClkMode       = AMBA_SPI_CPOL_LOW_CPHA_HIGH,      /* SPI Protocol mode */
    .ClkStretch    = 0U,                                /* Stretch the duration (us) from the active edge of chip select to the first edge of clock */
    .DataFrameSize = 8U,                                /* Data Frame Size in Bit */
    .FrameBitOrder = AMBA_SPI_TRANSFER_MSB_FIRST,       /* Bit transfer order */
};
#endif


static void IoDiag_SpiCmdUsage(char * const *pArgVector)
{
    printf("Usage: ");
    printf(pArgVector[0]);
    printf(" <channel> simpletest [start|stop]                       - start/stop a simple test monitor task using same config of this master using loopback\n");
    printf(" <channel> simpletest config <Baud> <CsPolarity> <Mode> <BitOrder> <Framesize>\n");

}

int IoDiag_Spi(UINT32 SpiChan, UINT32 ArgCount, char * const *pArgVector)
{
    static UINT8 IoDiagSpiMaster8bitBuf[2][AMBA_SPI_MASTER_FIFO_COUNT];
    static UINT16 IoDiagSpiMaster16bitBuf[2][AMBA_SPI_MASTER_FIFO_COUNT];

    UINT8 *pTxUINT8 = NULL, *pRxUINT8 = NULL;
    UINT16 *pTxUINT16 = NULL, *pRxUINT16 = NULL;
    UINT32 NumData = 0U, DfsMask, Val;
    UINT32 i, ActualSize = 0U, RetVal = SPI_ERR_NONE;
    UINT32 TransferType;
    UINT32 SlaveMask = 1U;

    if (SpiChan >= AMBA_NUM_SPI_MASTER) {
        printf("[Diag][SPI] ERROR: Invalid SPI master id.\n");
        RetVal = SPI_ERR_ARG;
    } else if ((SpiConfig.DataFrameSize < 4U) || (SpiConfig.DataFrameSize > 16U)) {
        printf("[Diag][SPI] ERROR: Do not support data frame size: %d", SpiConfig.DataFrameSize);
        RetVal = SPI_ERR_ARG;
    } else {
        if (strncmp(pArgVector[0], "trx", 3U) == 0) {
            TransferType = 0U;
        } else if (strncmp(pArgVector[0], "tx", 2U) == 0) {
            TransferType = 1U;
        } else if (strncmp(pArgVector[0], "rx", 2U) == 0) {
            TransferType = 2U;
        } else {
            printf("[Diag][SPI] ERROR: Unknown command argument:%s.\n", pArgVector[0]);
            RetVal = SPI_ERR_ARG;
        }

        if (strncmp(pArgVector[1], "msb", 3U) == 0) {
            SpiConfig.FrameBitOrder = AMBA_SPI_TRANSFER_MSB_FIRST;
        } else if (strncmp(pArgVector[1], "lsb", 3U) == 0) {
            SpiConfig.FrameBitOrder = AMBA_SPI_TRANSFER_LSB_FIRST;
        } else {
            printf("[Diag][SPI] ERROR: Unknown command argument:%s.\n", pArgVector[1]);
            RetVal = SPI_ERR_ARG;
        }

        NumData = strtoul(pArgVector[2], NULL, 0);
        if (NumData > (ArgCount - 3U)) {
            NumData = ArgCount - 3U;
        }

        if (RetVal == SPI_ERR_NONE) {
            DfsMask = (((UINT32)1U << SpiConfig.DataFrameSize) - 1U);

            if (TransferType != 2U) {
                /* trx or tx only */
                if (SpiConfig.DataFrameSize <= 8U) {
                    pTxUINT8 = IoDiagSpiMaster8bitBuf[0];
                    for (i = 0U; i < NumData; i ++) {
                        Val = strtoul(pArgVector[i + 3U], NULL, 0);
                        pTxUINT8[i] = (UINT8)(Val & DfsMask);
                    }
                } else {
                    pTxUINT16 = IoDiagSpiMaster16bitBuf[0];
                    for (i = 0U; i < NumData; i ++) {
                        Val = strtoul(pArgVector[i + 3U], NULL, 0);
                        pTxUINT16[i] = (UINT16)(Val & DfsMask);
                    }
                }
            }

            if (TransferType != 1U) {
                /* trx or rx only */
                if (SpiConfig.DataFrameSize <= 8U) {
                    pRxUINT8 = IoDiagSpiMaster8bitBuf[1];
                } else {
                    pRxUINT16 = IoDiagSpiMaster16bitBuf[1];
                }
            }

            (void)AmbaKAL_TaskSleep(100U);  /* configure spi slave first */

            if (SpiConfig.DataFrameSize <= 8U) {
                RetVal = AmbaSPI_MasterTransferD8(SpiChan, SlaveMask, &SpiConfig, NumData,
                                                  pTxUINT8, pRxUINT8, &ActualSize, 500);
            } else {
                RetVal = AmbaSPI_MasterTransferD16(SpiChan, SlaveMask, &SpiConfig, NumData,
                                                   pTxUINT16, pRxUINT16, &ActualSize, 500);
            }


            if (AmbaKAL_MutexTake(&DiagSpiPrintMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
                return SPI_ERR_MUTEX;
            } else {

                if (RetVal == SPI_ERR_NONE) {
                    printf("[Diag][SPI] Result = OK.\n");

                    if (SpiConfig.DataFrameSize <= 8U) {
                        for (i = 0; i < NumData; i++) {
                            printf("[Diag][SPI]");
                            printf(" Data %u,", i);
                            if (pRxUINT8 == NULL) {
                                if (pTxUINT8 != NULL) {
                                    printf(" TX=0x%02X\n", (UINT32)pTxUINT8[i]);
                                } else {
                                    printf(" N/A\n");
                                }
                            } else {
                                if (pTxUINT8 != NULL) {
                                    printf(" TX=0x%02X,", (UINT32)pTxUINT8[i]);
                                }
                                printf(" RX=0x%02X\n", (UINT32)pRxUINT8[i]);
                            }
                        }
                        printf("[Diag][SPI] ActualSize = %d\n", ActualSize);

                    } else {
                        for (i = 0; i < NumData; i++) {
                            printf("[Diag][SPI]");
                            printf(" Data %u,", i);
                            if (pRxUINT16 == NULL) {
                                if (pTxUINT16 != NULL) {
                                    printf(" TX=0x%04X\n", (UINT32)pTxUINT16[i]);
                                } else {
                                    printf(" N/A\n");
                                }
                            } else {
                                if (pTxUINT8 != NULL) {
                                    printf(" TX=0x%04X,", (UINT32)pTxUINT16[i]);
                                }
                                printf(" RX=0x%04X\n", (UINT32)pRxUINT16[i]);
                            }
                        }
                        printf("[Diag][SPI] ActualSize = %d\n", ActualSize);
                    }

                } else {
                    printf("[Diag][SPI] Result = NG.\n");
                    RetVal = SPI_ERR_ARG;
                }

                if (AmbaKAL_MutexGive(&DiagSpiPrintMutex) != KAL_ERR_NONE) {
                    return SPI_ERR_UNEXPECTED;
                }
            }
        }
    }

    return RetVal;
}

#if 0
static void IoDiag_SpiIllustrate(const AMBA_SPI_CONFIG_s *pSpiConfig)
{
    if ((pSpiConfig->ClkMode & 0x1U) != 0U) {
        // CPHA = 1
        if (pSpiConfig->CsPolarity != 0U) {
            // Active High
            printf("[Diag][SPI]                |    __________//________________//____________    \n");
            printf("[Diag][SPI]  SS (CPHA = 1) |   |     :     :     :     :     :     :      |   \n");
            printf("[Diag][SPI]                |___|     :     :     :     :     :     :      |___\n");
            printf("[Diag][SPI]                |         :     :     :     :     :     :          \n");
        } else {
            // Active Low
            printf("[Diag][SPI]                |___      :     :     :     :     :     :       ___\n");
            printf("[Diag][SPI]  SS (CPHA = 1) |   |     :     :     :     :     :     :      |   \n");
            printf("[Diag][SPI]                |   |__________//________________//____________|   \n");
            printf("[Diag][SPI]                |         :     :     :     :     :     :          \n");
        }

        if (pSpiConfig->ClkMode < 2U) {
            // CPOL = 0
            printf("[Diag][SPI]                |       __:   __:   __:   __:   __:   __:          \n");
            printf("[Diag][SPI] SCK (CPOL = 0) |      |  |  |  |  |  |  |  |  |  |  |  |          \n");
            printf("[Diag][SPI]                |______|  |__|  |__|  |__|  |__|  |__|  |__________\n");
        } else {
            // CPOL = 1
            printf("[Diag][SPI]                |______   :__   :__   :__   :__   :__   :__________\n");
            printf("[Diag][SPI] SCK (COPL = 1) |      |  |  |  |  |  |  |  |  |  |  |  |          \n");
            printf("[Diag][SPI]                |      |__|  |__|  |__|  |__|  |__|  |__|          \n");
        }
        if (pSpiConfig->FrameBitOrder != 0U) {
            // LSB first
            printf("[Diag][SPI]                |       __:__ __:__ __:__ __:__ __:__ __:__        \n");
            printf("[Diag][SPI] MISO/MOSI      |      | LSB |     | MSB | LSB |     | MSB |       \n");
            printf("[Diag][SPI]                |______|__0__|_//__|_");
            printf("%2u", pSpiConfig->DataFrameSize - 1U);
            printf("__|__0__|_//__|_");
            printf("%2u", pSpiConfig->DataFrameSize - 1U);
            printf("__|_______\n");
        } else {
            // MSB first
            printf("[Diag][SPI]                |       __:__ __:__ __:__ __:__ __:__ __:__        \n");
            printf("[Diag][SPI] MISO/MOSI      |      | MSB |     | LSB | MSB |     | LSB |       \n");
            printf("[Diag][SPI]                |______|_");
            printf("%2u", pSpiConfig->DataFrameSize - 1U);
            printf("__|_//__|__0__|_");
            printf("%2u", pSpiConfig->DataFrameSize - 1U);
            printf("__|_//__|__0__|_______\n");
        }
    } else {
        // CPHA = 0
        if (pSpiConfig->CsPolarity != 0U) {
            // Active High
            printf("[Diag][SPI]                |    _______//_________    _______//___________    \n");
            printf("[Diag][SPI]  SS (CPHA = 0) |   |  :     :     :   |  |  :     :     :     |   \n");
            printf("[Diag][SPI]                |___|  :     :     :   |__|  :     :     :     |___\n");
            printf("[Diag][SPI]                |      :     :     :         :     :     :         \n");
        } else {
            // Active Low
            printf("[Diag][SPI]                |___   :     :     :    __   :     :     :      ___\n");
            printf("[Diag][SPI]  SS (CPHA = 0) |   |  :     :     :   |  |  :     :     :     |   \n");
            printf("[Diag][SPI]                |   |_______//_________|  |_______//___________|   \n");
            printf("[Diag][SPI]                |      :     :     :         :     :     :         \n");
        }
        if (pSpiConfig->ClkMode < 2U) {
            // CPOL = 0
            printf("[Diag][SPI]                |      :__   :__   :__       :__   :__   :__       \n");
            printf("[Diag][SPI] SCK (CPOL = 0) |      |  |  |  |  |  |      |  |  |  |  |  |      \n");
            printf("[Diag][SPI]                |______|  |__|  |__|  |______|  |__|  |__|  |______\n");
        } else {
            // CPOL = 1
            printf("[Diag][SPI]                |______:   __:   __:   ______:   __:   __:   ______\n");
            printf("[Diag][SPI] SCK (COPL = 1) |      |  |  |  |  |  |      |  |  |  |  |  |      \n");
            printf("[Diag][SPI]                |      |__|  |__|  |__|      |__|  |__|  |__|      \n");
        }

        if (pSpiConfig->FrameBitOrder != 0U) {
            // LSB first
            printf("[Diag][SPI]                |    __:__ __:__ __:__     __:__ __:__ __:__       \n");
            printf("[Diag][SPI] MISO/MOSI      |   | LSB |     | MSB |   | LSB |     | MSB |      \n");
            printf("[Diag][SPI]                |___|__0__|_//__|_");
            printf("%2u", pSpiConfig->DataFrameSize - 1U);
            printf("__|___|__0__|_//__|_");
            printf("%2u", pSpiConfig->DataFrameSize - 1U);
            printf("__|______\n");
        } else {
            // MSB first
            printf("[Diag][SPI]                |    __:__ __:__ __:__     __:__ __:__ __:__       \n");
            printf("[Diag][SPI] MISO/MOSI      |   | MSB |     | LSB |   | MSB |     | LSB |      \n");
            printf("[Diag][SPI]                |___|_");
            printf("%2u", pSpiConfig->DataFrameSize - 1U);
            printf("__|_//__|__0__|___|_");
            printf("%2u", pSpiConfig->DataFrameSize - 1U);
            printf("__|_//__|__0__|______\n");
        }
    }
}
#endif

static void IoDIag_SpiShowConfig(UINT32 SpiChan)
{
    const DIAG_SPI_CTRL_s *pSpiCtrl = &IoDiagSpiCtrl[SpiChan];
    const AMBA_SPI_CONFIG_s *pSpiConfig = &pSpiCtrl->SpiConfig;

    printf("[Diag][SPI] Master ID     :");
    printf(" %u\n", SpiChan);

    printf("[Diag][SPI] Slave Mask    :");
    printf(" 0x%02X\n", pSpiCtrl->SlaveMask);

    printf("[Diag][SPI] Clock Mode    :");
    printf(" 0x%02X\n", pSpiConfig->ClkMode);

    if (pSpiConfig->CsPolarity == AMBA_SPI_CHIP_SELECT_ACTIVE_LOW) {
        printf("[Diag][SPI] CS Polarity   : active in low\n");
    } else {
        printf("[Diag][SPI] CS Polarity   : active in high\n");
    }

    printf("[Diag][SPI] DataFrameSize :");
    printf(" %u bits\n", pSpiConfig->DataFrameSize);

    if (pSpiConfig->FrameBitOrder == AMBA_SPI_TRANSFER_MSB_FIRST) {
        printf("[Diag][SPI] Bit-Order     : MSB first\n");
    } else {
        printf("[Diag][SPI] Bit-Order     : LSB first\n");
    }

    if (pSpiCtrl->TransferType == 1U) {
        printf("[Diag][SPI] Transfer Type : Tx only transfer\n");
    } else if (pSpiCtrl->TransferType == 2U) {
        printf("[Diag][SPI] Transfer Type : Rx only transfer\n");
    } else {
        printf("[Diag][SPI] Transfer Type : Tx & Rx transfer\n");
    }

    if (pSpiCtrl->UseDma == 0U) {
        printf("[Diag][SPI] DMA Function  : disabled\n");
    } else {
        printf("[Diag][SPI] DMA Function  : enabled\n");
    }

    //IoDiag_SpiIllustrate(pSpiConfig);
}

static void *IoDiag_SpiSlaveTaskEntry(void * Arg)
{
    static UINT8 IoDiagSpiSlave8bitBuf[2][DIAG_SPI_NUM_DATA];
    static UINT16 IoDiagSpiSlave16bitBuf[2][DIAG_SPI_NUM_DATA];

    const AMBA_SPI_CONFIG_s *pSpiConfig;
    UINT8 *pTxUINT8 = IoDiagSpiSlave8bitBuf[0], *pRxUINT8 = IoDiagSpiSlave8bitBuf[1];
    UINT16 *pTxUINT16 = IoDiagSpiSlave16bitBuf[0], *pRxUINT16 = IoDiagSpiSlave16bitBuf[1];
    UINT32 i, ActualFlags = 0U, ActualSize = 0U, RetVal; /* ActualFlags used as NumData here. */
    UINT32 ChannelID;
    UINT32 Loop = 1U;

    if (Arg != NULL) {
        AmbaMisra_TouchUnused(Arg);
        AmbaMisra_TypeCast(&ChannelID, &Arg);
    } else {
        ChannelID = AMBA_SPI_SLAVE;
    }

    while (1U == Loop) {
        printf("[Diag][SPI_Slave] Wait for master %u transfer..\n", ChannelID);
        AmbaPrint_Flush();

        (void)AmbaKAL_EventFlagGet(&IoDiagSpiEventFlag, 0xffffffffU, AMBA_KAL_FLAGS_ANY, AMBA_KAL_FLAGS_CLEAR_AUTO, &ActualFlags, AMBA_KAL_WAIT_FOREVER);

        if (pIoDiagSpiSlaveCtrl != NULL) {
            pSpiConfig = &pIoDiagSpiSlaveCtrl->SpiConfig;

            if (pSpiConfig->DataFrameSize <= 8U) {
                for (i = 0; i < ActualFlags; i ++) {
                    pTxUINT8[i] = (UINT8)(ActualFlags - i);
                }
            } else {
                for (i = 0; i < ActualFlags; i ++) {
                    pTxUINT16[i] = (UINT16)(ActualFlags - i);
                }
            }

            RetVal = AmbaSPI_SlaveConfig(AMBA_SPI_SLAVE, pSpiConfig);
            if (RetVal == OK) {
                if (pSpiConfig->DataFrameSize <= 8U) {
                    RetVal = AmbaSPI_SlaveTransferD8(AMBA_SPI_SLAVE, ActualFlags, pTxUINT8, pRxUINT8, &ActualSize, 1000U);
                } else {
                    RetVal = AmbaSPI_SlaveTransferD16(AMBA_SPI_SLAVE, ActualFlags, pTxUINT16, pRxUINT16, &ActualSize, 1000U);
                }

                printf("[Diag][SPI_Slave] Slave transfer %d bytes (%x)\n", ActualSize, RetVal);
                AmbaPrint_Flush();

                if (ActualSize != 0U) {
                    if (pSpiConfig->DataFrameSize <= 8U) {
                        for (i = 0; i < ActualSize; i ++) {
                            printf("[Diag][SPI_Slave] Data %u, TX=0x%02X, RX=0x%02X\n", i, (UINT32)pTxUINT8[i], (UINT32)pRxUINT8[i]);
                        }
                    } else {
                        for (i = 0; i < ActualSize; i ++) {
                            printf("[Diag][SPI_Slave] Data %u, TX=0x%04X, RX=0x%04X\n", i, (UINT32)pTxUINT16[i], (UINT32)pRxUINT16[i]);
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

#if 0
static void MasterTransferTask(const DIAG_SPI_PARAM_s* Arg)
{
    IoDiag_Spi(Arg->SpiChan, Arg->ArgCount, Arg->pArgVector);
}

static void SlaveTransferTask(const DIAG_SPI_PARAM_s* Arg)
{
    static UINT8 IoDiagSpiSlave8bitBuf[2][AMBA_SPI_MASTER_FIFO_COUNT];
    static UINT16 IoDiagSpiSlave16bitBuf[2][AMBA_SPI_MASTER_FIFO_COUNT];

    UINT8 *pTxUINT8 = IoDiagSpiSlave8bitBuf[0], *pRxUINT8 = IoDiagSpiSlave8bitBuf[1];
    UINT16 *pTxUINT16 = IoDiagSpiSlave16bitBuf[0], *pRxUINT16 = IoDiagSpiSlave16bitBuf[1];
    UINT32 i, NumData, ActualSize = 0U, RetVal;


    printf("[Diag][SPI_SLAVE] Wait for master %u transfer..\n", Arg->SpiChan);

    NumData = strtoul(Arg->pArgVector[2], NULL, 0);
    if (NumData > (Arg->ArgCount - 3U)) {
        NumData = Arg->ArgCount - 3U;
    }

    if (SpiConfig.DataFrameSize <= 8U) {
        for (i = 0; i < NumData; i ++) {
            pTxUINT8[i] = (UINT8)(NumData - i);
        }
    } else {
        for (i = 0; i < NumData; i ++) {
            pTxUINT16[i] = (UINT16)(NumData - i);
        }
    }

    RetVal = AmbaSPI_SlaveConfig(AMBA_SPI_SLAVE, &SpiConfig);

    if (RetVal == OK) {
        if (SpiConfig.DataFrameSize <= 8U) {
            RetVal = AmbaSPI_SlaveTransferD8(AMBA_SPI_SLAVE, NumData, pTxUINT8, pRxUINT8, &ActualSize, 1000U);
        } else {
            RetVal = AmbaSPI_SlaveTransferD16(AMBA_SPI_SLAVE, NumData, pTxUINT16, pRxUINT16, &ActualSize, 1000U);
        }

        if (AmbaKAL_MutexTake(&DiagSpiPrintMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            return SPI_ERR_MUTEX;
        } else {
            printf("[Diag][SPI_SLAVE] Slave transfer %d bytes (%x)\n", ActualSize, RetVal);

            if (ActualSize != 0U) {
                if (SpiConfig.DataFrameSize <= 8U) {
                    for (i = 0; i < ActualSize; i ++) {
                        printf("[Diag][SPI_SLAVE] Data %u, TX=0x%02X, RX=0x%02X\n", i, (UINT32)pTxUINT8[i], (UINT32)pRxUINT8[i]);
                    }
                } else {
                    for (i = 0; i < ActualSize; i ++) {
                        printf("[Diag][SPI_SLAVE] Data %u, TX=0x%04X, RX=0x%04X\n", i, (UINT32)pTxUINT16[i], (UINT32)pRxUINT16[i]);
                    }
                }
            }

            if (AmbaKAL_MutexGive(&DiagSpiPrintMutex) != KAL_ERR_NONE) {
                return SPI_ERR_UNEXPECTED;
            }
        }
    }
}
#endif

static void IoDiag_SpiMasterSimpleTest(UINT32 SpiChan)
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

        NumData = 10U;
        /* Notify slave task (if exist) for loopback test */
        (void)AmbaKAL_EventFlagSet(&IoDiagSpiEventFlag, NumData);
        (void)AmbaKAL_TaskSleep(100U);  /* configure spi slave first */

        if (pSpiConfig->DataFrameSize <= 8U) {
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
            RetVal = AmbaSPI_MasterTransferD8(SpiChan, pSpiCtrl->SlaveMask, pSpiConfig, NumData,
                                              pTxUINT8, pRxUINT8, &ActualSize, AMBA_KAL_WAIT_FOREVER);
        } else {
            pTxUINT16[0] = 0x1111U;
            pTxUINT16[1] = 0x2222U;
            pTxUINT16[2] = 0x3333U;
            pTxUINT16[3] = 0x4444U;
            pTxUINT16[4] = 0x5555U;
            pTxUINT16[5] = 0x6666U;
            pTxUINT16[6] = 0x7777U;
            pTxUINT16[7] = 0x8888U;
            pTxUINT16[8] = 0x9999U;
            pTxUINT16[9] = 0xAAAAU;
            RetVal = AmbaSPI_MasterTransferD16(SpiChan, pSpiCtrl->SlaveMask, pSpiConfig, NumData,
                                               pTxUINT16, pRxUINT16, &ActualSize, AMBA_KAL_WAIT_FOREVER);
        }

        IoDIag_SpiShowConfig(SpiChan);

        if (RetVal == SPI_ERR_NONE) {
            printf("[Diag][SPI] Result = OK.\n");

            for (i = 0; i < NumData; i ++) {
                printf("[Diag][SPI]");
                printf(" Data %u,", i);

                if (pSpiConfig->DataFrameSize <= 8U) {
                    if (pRxUINT8 == NULL) {
                        if (pTxUINT8 != NULL) {
                            printf(" TX=0x%02X\n", (UINT32)pTxUINT8[i]);
                        } else {
                            printf(" N/A\n");
                        }
                    } else {
                        if (pTxUINT8 != NULL) {
                            printf(" TX=0x%02X,", (UINT32)pTxUINT8[i]);
                        }
                        printf(" RX=0x%02X\n", (UINT32)pRxUINT8[i]);
                    }
                } else {
                    if (pRxUINT16 == NULL) {
                        if (pTxUINT16 != NULL) {
                            printf(" TX=0x%04X\n", (UINT32)pTxUINT16[i]);
                        } else {
                            printf(" N/A\n");
                        }
                    } else {
                        if (pTxUINT8 != NULL) {
                            printf(" TX=0x%04X,", (UINT32)pTxUINT16[i]);
                        }
                        printf(" RX=0x%04X\n", (UINT32)pRxUINT16[i]);
                    }
                }
            }
        } else {
            printf("[Diag][SPI] Result = NG.\n");
        }
    }
}


int DoSPIDiag(int argc, char *argv[])
{
    UINT32 RetVal = SPI_ERR_NONE;
    UINT32 SpiChan;
    UINT32 Baud, Polarity, Mode, Framesize, BitOrder, FrameSize;
    char **ptr = NULL;

    if (argc < 5) {
        IoDiag_SpiCmdUsage(argv);
    } else {
        SpiChan = strtoul(argv[2], ptr, 0);
        if (SpiChan >= AMBA_NUM_SPI_MASTER) {
            printf("[Diag][SPI] ERROR: Invalid spi master id.\n");
        }

        if (argc >= 3) {
            if (strncmp(argv[3], "simpletest", 10U) == 0) {
                if (argc >= 4) {
                    if (strncmp(argv[4], "start", 5U) == 0) {
                        IoDiag_SpiLoopBack(SpiChan);
                        IoDiag_SpiMasterSimpleTest(SpiChan);
                    } else if (strncmp(argv[4], "stop", 4U) == 0) {
                        IoDiag_SpiLoopBack(AMBA_NUM_SPI_MASTER);
                    } else if (strncmp(argv[4], "config", 6U) == 0) {
                        //Run with specific config
                        Baud = strtoul(argv[5], ptr, 0);
                        IoDiagSpiCtrl[SpiChan].SpiConfig.BaudRate = Baud;
                        Polarity = strtoul(argv[6], ptr, 0);
                        IoDiagSpiCtrl[SpiChan].SpiConfig.CsPolarity = Polarity;
                        Mode = strtoul(argv[7], ptr, 0);
                        IoDiagSpiCtrl[SpiChan].SpiConfig.ClkMode = Mode;
                        BitOrder = strtoul(argv[8], ptr, 0);
                        IoDiagSpiCtrl[SpiChan].SpiConfig.FrameBitOrder = BitOrder;
                        FrameSize = strtoul(argv[9], ptr, 0);
                        IoDiagSpiCtrl[SpiChan].SpiConfig.DataFrameSize = FrameSize;

                        IoDiag_SpiLoopBack(SpiChan);
                        IoDiag_SpiMasterSimpleTest(SpiChan);
                    }
                } else {
                    IoDiag_SpiCmdUsage(argv);
                }
            } else {
                IoDiag_SpiCmdUsage(argv);
            }
        }
    }

    return RetVal;
}

