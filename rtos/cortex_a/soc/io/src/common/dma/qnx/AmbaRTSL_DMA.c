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

#include "AmbaMMU.h"
#include "AmbaCache.h"

#include "AmbaRTSL_DMA.h"
#include "AmbaCSL_DMA.h"

#include <pthread.h>
#include <threads.h>
#include <sys/siginfo.h>
#include <sys/neutrino.h>

typedef struct {
    UINT32  Status;         /* status of current DMA transaction */
    UINT32  DmaChanFunc;
} AMBA_DMA_CTRL_s;

static void DMA0_ISR(UINT32 IntID, UINT32 UserArg);
static void DMA1_ISR(UINT32 IntID, UINT32 UserArg);

static AMBA_DMA_ISR_CALLBACK_f AmbaDmaIsrCallBack = NULL;
static AMBA_DMA_CTRL_s AmbaDmaCtrl[AMBA_NUM_DMA_CHANNEL];
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
static UINT32 AmbaDmaIntCount[AMBA_NUM_DMA_CHANNEL];
#endif

static const UINT32 AmbaDmaChanFunc[AMBA_NUM_DMA_CHANNEL_TYPE] = {
    [AMBA_DMA_CHANNEL_MEM_TRX      ] = (0U),
    [AMBA_DMA_CHANNEL_SPI0_TX      ] = (1U),
    [AMBA_DMA_CHANNEL_SPI0_RX      ] = (2U),
    [AMBA_DMA_CHANNEL_SPI1_TX      ] = (3U),
    [AMBA_DMA_CHANNEL_SPI1_RX      ] = (4U),
    [AMBA_DMA_CHANNEL_NOR_SPI_TX   ] = (5U),
    [AMBA_DMA_CHANNEL_NOR_SPI_RX   ] = (6U),
    [AMBA_DMA_CHANNEL_SPI_SLAVE_TX ] = (7U),
    [AMBA_DMA_CHANNEL_SPI_SLAVE_RX ] = (8U),
    [AMBA_DMA_CHANNEL_UART0_TX     ] = (9U),
    [AMBA_DMA_CHANNEL_UART0_RX     ] = (10U),
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
    [AMBA_DMA_CHANNEL_UART1_TX     ] = (11U),
    [AMBA_DMA_CHANNEL_UART1_RX     ] = (12U),
    [AMBA_DMA_CHANNEL_UART2_TX     ] = (13U),
    [AMBA_DMA_CHANNEL_UART2_RX     ] = (14U),
    [AMBA_DMA_CHANNEL_UART3_TX     ] = (15U),
    [AMBA_DMA_CHANNEL_UART3_RX     ] = (16U),
    [AMBA_DMA_CHANNEL_SPI2_TX      ] = (17U),
    [AMBA_DMA_CHANNEL_SPI2_RX      ] = (18U),
    [AMBA_DMA_CHANNEL_SPI3_TX      ] = (19U),
    [AMBA_DMA_CHANNEL_SPI3_RX      ] = (20U),
    [AMBA_DMA_CHANNEL_I2S_TX       ] = (21U),
    [AMBA_DMA_CHANNEL_I2S_RX       ] = (22U),
    [AMBA_DMA_CHANNEL_SPI4_TX      ] = (23U),
    [AMBA_DMA_CHANNEL_SPI4_RX      ] = (24U),
    [AMBA_DMA_CHANNEL_SPI5_TX      ] = (23U),
    [AMBA_DMA_CHANNEL_SPI5_RX      ] = (24U),
#else
    [AMBA_DMA_CHANNEL_I2S_1CH_TX   ] = (11U),
    [AMBA_DMA_CHANNEL_I2S_1CH_RX   ] = (12U),
    [AMBA_DMA_CHANNEL_UART1_TX     ] = (13U),
    [AMBA_DMA_CHANNEL_UART1_RX     ] = (14U),
    [AMBA_DMA_CHANNEL_UART2_TX     ] = (15U),
    [AMBA_DMA_CHANNEL_UART2_RX     ] = (16U),
    [AMBA_DMA_CHANNEL_UART3_TX     ] = (17U),
    [AMBA_DMA_CHANNEL_UART3_RX     ] = (18U),
    [AMBA_DMA_CHANNEL_SPI2_TX      ] = (19U),
    [AMBA_DMA_CHANNEL_SPI2_RX      ] = (20U),
    [AMBA_DMA_CHANNEL_SPI3_TX      ] = (21U),
    [AMBA_DMA_CHANNEL_SPI3_RX      ] = (22U),
    [AMBA_DMA_CHANNEL_I2S_TX       ] = (23U),
    [AMBA_DMA_CHANNEL_I2S_RX       ] = (24U),
#endif
};

/**
 *  DMA_GetPhysAddr - Translate a virtual address to a physical address
 *  @param[in] pVirtAddr A virtual address
 *  @return Translated physical address
 */
static UINT32 DMA_GetPhysAddr(const void *pVirtAddr)
{
    ULONG VirtAddr64, PhysAddr64;
    UINT32 PhysAddr32;

    /* LP64 */
    AmbaMisra_TypeCast(&VirtAddr64, &pVirtAddr);
    (void)AmbaMMU_VirtToPhys(VirtAddr64, &PhysAddr64);
    PhysAddr32 = (UINT32) PhysAddr64;

    return PhysAddr32;
}

/**
 *  DMA_CacheClean - Do a cache clean operation
 *  @param[in] pVirtAddr A virtual address
 *  @param[in] Size Number of bytes
 */
static void DMA_CacheClean(const void *pVirtAddr, UINT32 Size)
{
    ULONG VirtAddr64;

    /* LP64 */
    AmbaMisra_TypeCast(&VirtAddr64, &pVirtAddr);
    (void)AmbaCache_DataClean((UINT32)VirtAddr64, Size);
}

/**
 *  DMA_TransferDesc - Do dma transfers with descriptor mode
 *  @param[in] pDmaChanReg pointer to dma channel control registers
 *  @param[in] pDmaChanDescReg pointer to dma channel descriptor registers
 *  @param[in] pDmaDesc pointer to a linked list of dma descriptors
 */
static void DMA_TransferDesc(volatile AMBA_DMA_CHAN_REG_s *pDmaChanReg, volatile UINT32 *pDmaChanDescReg, AMBA_DMA_DESC_s *pDmaDesc)
{
    AmbaMisra_TypeCast32(pDmaChanDescReg, &pDmaDesc);

    /* DmaCtrl.DescMode = 1U, DmaCtrl.Enable = 1U */
    pDmaChanReg->Ctrl = 0xC0000000U;
}

/**
 *  DMA_TransferNonDesc - Do dma transfers with non-descriptor mode
 *  @param[in] pDmaChanReg pointer to dma channel control registers
 *  @param[in] pDmaDesc pointer to a linked list of dma descriptors
 */
static void DMA_TransferNonDesc(volatile AMBA_DMA_CHAN_REG_s *pDmaChanReg, const AMBA_DMA_DESC_s *pDmaDesc)
{
    AMBA_DMA_CTRL_REG_s DmaCtrl;
    UINT32 DmaCtrlRegVal;

    DmaCtrl.ByteCount = pDmaDesc->DataSize;
    DmaCtrl.BusDataSize = pDmaDesc->Ctrl.BusDataSize;
    DmaCtrl.BusBlockSize = pDmaDesc->Ctrl.BusBlockSize;
    DmaCtrl.NoBusAddrInc = pDmaDesc->Ctrl.NoBusAddrInc;
    DmaCtrl.ReadMem = pDmaDesc->Ctrl.ReadMem;
    DmaCtrl.WriteMem = pDmaDesc->Ctrl.WriteMem;
    DmaCtrl.DescMode = 0;
    DmaCtrl.Enable = 1U;
    AmbaMisra_TypeCast32(&DmaCtrlRegVal, &DmaCtrl);

    AmbaMisra_TypeCast32(&pDmaChanReg->SrcAddr, &pDmaDesc->pSrcAddr);
    AmbaMisra_TypeCast32(&pDmaChanReg->DstAddr, &pDmaDesc->pDstAddr);

    pDmaChanReg->Ctrl       = DmaCtrlRegVal;
}

/**
 *  AmbaRTSL_DmaInit - DMA module initialization
 */
void AmbaRTSL_DmaInit(void)
{
    AMBA_DMA_CTRL_s *pDmaCtrl;
    volatile AMBA_DMA_CHAN_REG_s *pDmaChanReg[2];
    UINT32 i, IntID;
    pthread_t Task;

    /* Init DMA control structure */
    for (i = 0; i < AMBA_NUM_DMA_CHANNEL; i++) {
        pDmaCtrl = &AmbaDmaCtrl[i];
        pDmaCtrl->Status = 0;
        pDmaCtrl->DmaChanFunc = 0xFFFFFFFFU;
        AmbaCSL_DmaSetChanFunc(i, 0);
    }

    /* Clear all interrupt status */
    for (i = 0; i < AMBA_DMA_CHANNEL_8; i++) {
        pDmaChanReg[0] = &pAmbaDMA_Reg[0]->DmaChanReg[i];
        pDmaChanReg[1] = &pAmbaDMA_Reg[1]->DmaChanReg[i];
        AmbaCSL_DmaClearStatus(pDmaChanReg[0]); /* clear all status and IRQ status */
        AmbaCSL_DmaClearStatus(pDmaChanReg[1]); /* clear all status and IRQ status */
        pDmaChanReg[0]->Ctrl = 0x38000000U;
        pDmaChanReg[1]->Ctrl = 0x38000000U;
    }

    pthread_create(&Task, NULL, DMA0_ISR, NULL);
    pthread_setschedprio(Task, 128U);
    pthread_create(&Task, NULL, DMA1_ISR, NULL);
    pthread_setschedprio(Task, 128U);
}

/**
 *  AmbaRTSL_DmaHookIntHandler - Hook interrupt handler
 *  @param[in] pIsr Interrupt service routine
 */
void AmbaRTSL_DmaHookIntHandler(AMBA_DMA_ISR_CALLBACK_f pIsr)
{
    AmbaDmaIsrCallBack = pIsr;
}

/**
 *  AmbaRTSL_DmaTransfer - Do dma transfers
 *  @param[in] DmaChanNo DMA channel number
 *  @param[in] pDmaDesc pointer to a linked list of dma descriptors
 *  @return error code
 */
UINT32 AmbaRTSL_DmaTransfer(UINT32 DmaChanNo, AMBA_DMA_DESC_s *pDmaDesc)
{
    volatile AMBA_DMA_REG_s *pDmaReg;
    volatile AMBA_DMA_CHAN_REG_s *pDmaChanReg;
    volatile UINT32 *pDmaChanDescReg;
    UINT32 DmaChanOffset;
    UINT32 RetVal = DMA_ERR_NONE;

    if ((DmaChanNo >= AMBA_NUM_DMA_CHANNEL) || (pDmaDesc == NULL)) {
        RetVal = DMA_ERR_ARG;
    } else {
        /* pointer to DMA H/W Registers */
        if (DmaChanNo < AMBA_DMA_CHANNEL_8) {
            pDmaReg = pAmbaDMA_Reg[0];
            DmaChanOffset = DmaChanNo;
        } else {
            pDmaReg = pAmbaDMA_Reg[1];
            DmaChanOffset = DmaChanNo - AMBA_DMA_CHANNEL_8;
        }

        pDmaChanReg = &pDmaReg->DmaChanReg[DmaChanOffset];
        /* clear all status and IRQ status */
        AmbaCSL_DmaClearStatus(pDmaChanReg);

        if (pDmaDesc->pNextDesc == NULL) {
            /* Use non-descritpor mode */
            DMA_TransferNonDesc(pDmaChanReg, pDmaDesc);
        } else {
            /* Use descritpor mode */
            pDmaChanDescReg = &pDmaReg->DmaChanDescAddr[DmaChanOffset];
            /* Note, pNextDesc points to the address containing real descriptions */
            DMA_TransferDesc(pDmaChanReg, pDmaChanDescReg, pDmaDesc->pNextDesc);
        }
    }

    return RetVal;
}

/**
 *  AmbaRTSL_DmaStop - Stop dma transfers
 *  @param[in] DmaChanNo DMA channel number
 *  @return error code
 */
UINT32 AmbaRTSL_DmaStop(UINT32 DmaChanNo)
{
    AMBA_DMA_REG_s *pDmaReg;
    volatile AMBA_DMA_CHAN_REG_s *pDmaChanReg;
    UINT32 DmaChanOffset;
    UINT32 RetVal = DMA_ERR_NONE;

    if (DmaChanNo >= AMBA_NUM_DMA_CHANNEL) {
        RetVal = DMA_ERR_ARG;
    } else {
        /* pointer to DMA H/W Registers */
        if (DmaChanNo < AMBA_DMA_CHANNEL_8) {
            pDmaReg = pAmbaDMA_Reg[0];
            DmaChanOffset = DmaChanNo;
        } else {
            pDmaReg = pAmbaDMA_Reg[1];
            DmaChanOffset = DmaChanNo - AMBA_DMA_CHANNEL_8;
        }

        pDmaChanReg = &pDmaReg->DmaChanReg[DmaChanOffset];
        if ((pDmaChanReg->Ctrl & 0x80000000U) != 0U) {
            /* update the latest status to AmbaRTSL_DmaCtrl */
            AmbaDmaCtrl[DmaChanNo].Status = pDmaChanReg->Status;
            AmbaCSL_DmaClearStatus(pDmaChanReg);
            pDmaChanReg->Ctrl = 0x38000000U;
        }
    }

    return RetVal;
}

/**
 *  AmbaRTSL_DmaGetTrfByteCount - Get dma transferred byte count
 *  @param[in] DmaChanNo DMA channel number
 *  @param[out] pActualSize Number of transferred bytes
 *  @return error code
 */
UINT32 AmbaRTSL_DmaGetTrfByteCount(UINT32 DmaChanNo, UINT32 *pActualSize)
{
    const AMBA_DMA_REG_s *pDmaReg;
    const volatile AMBA_DMA_CHAN_REG_s *pDmaChanReg;
    UINT32 DmaChanOffset;
    UINT32 MainStatus;
    UINT32 RetVal = DMA_ERR_NONE;

    if ((DmaChanNo >= AMBA_NUM_DMA_CHANNEL) || (pActualSize == NULL)) {
        RetVal = DMA_ERR_ARG;
    } else {
        /* pointer to DMA H/W Registers */
        if (DmaChanNo < AMBA_DMA_CHANNEL_8) {
            pDmaReg = pAmbaDMA_Reg[0];
            DmaChanOffset = DmaChanNo;
        } else {
            pDmaReg = pAmbaDMA_Reg[1];
            DmaChanOffset = DmaChanNo - AMBA_DMA_CHANNEL_8;
        }

        pDmaChanReg = &pDmaReg->DmaChanReg[DmaChanOffset];

        if ((pDmaChanReg->Ctrl & 0x80000000U) != 0U) {
            MainStatus = pDmaChanReg->Status;
        } else {   /* Dma status has been updated to AmbaRTSL_DmaCtrl, and cleared to 0 by ISR */
            MainStatus = AmbaDmaCtrl[DmaChanNo].Status;
        }

        *pActualSize = (MainStatus & 0x3FFFFFU);
    }
    return RetVal;
}

/**
 *  AmbaRTSL_DmaChanAllocate - Allocate a DMA channel for the specified purpose
 *  @param[in] DmaChanType DMA channel function selection
 *  @param[out] pDmaChanNo Allocated DMA channel number
 *  @return error code
 */
UINT32 AmbaRTSL_DmaChanAllocate(UINT32 DmaChanType, UINT32 *pDmaChanNo)
{
    AMBA_DMA_CTRL_s *pDmaCtrl = NULL;
    UINT32 i, RetVal = DMA_ERR_NONE;

    if ((DmaChanType >= AMBA_NUM_DMA_CHANNEL_TYPE) || (pDmaChanNo == NULL)) {
        RetVal = DMA_ERR_ARG;
    } else {
        *pDmaChanNo = 0xFFFFFFFFU;

        for (i = 0; i < AMBA_NUM_DMA_CHANNEL; i++) {
            pDmaCtrl = &AmbaDmaCtrl[i];
            if ((pDmaCtrl->DmaChanFunc == 0xFFFFFFFFU) ||
                ((pDmaCtrl->DmaChanFunc == AmbaDmaChanFunc[DmaChanType]) && (DmaChanType != AMBA_DMA_CHANNEL_MEM_TRX))) {
                if (pDmaCtrl->DmaChanFunc == 0xFFFFFFFFU) {
                    pDmaCtrl->DmaChanFunc = AmbaDmaChanFunc[DmaChanType];
                    AmbaCSL_DmaSetChanFunc(i, pDmaCtrl->DmaChanFunc);
                }
                *pDmaChanNo = i;
                break;
            }
        }
    }

    return RetVal;
}

/**
 *  AmbaRTSL_DmaChanRelease - Release a DMA channel
 *  @param[in] DmaChanNo DMA channel number
 *  @return error code
 */
UINT32 AmbaRTSL_DmaChanRelease(UINT32 DmaChanNo)
{
    UINT32 RetVal = DMA_ERR_NONE;

    if (DmaChanNo >= AMBA_NUM_DMA_CHANNEL) {
        RetVal = DMA_ERR_ARG;
    } else {
        AmbaDmaCtrl[DmaChanNo].DmaChanFunc = 0xFFFFFFFFU;
        AmbaCSL_DmaSetChanFunc(DmaChanNo, 0);
    }

    return RetVal;
}

/**
 *  AmbaRTSL_DmaSetChanType - Assign function to a DMA channel
 *  @param[in] DmaChanNo DMA channel number
 *  @param[in] DmaChanType DMA channel function selection
 *  @return error code
 */
UINT32 AmbaRTSL_DmaSetChanType(UINT32 DmaChanNo, UINT32 DmaChanType)
{
    UINT32 RetVal = DMA_ERR_NONE;

    if (AmbaDmaCtrl[DmaChanNo].DmaChanFunc != AmbaDmaChanFunc[DmaChanType]) {
        /* The dma channel shall be available */
        if (AmbaDmaCtrl[DmaChanNo].DmaChanFunc != 0xFFFFFFFFU) {
            RetVal = DMA_ERR_ARG;
        } else {
            AmbaDmaCtrl[DmaChanNo].DmaChanFunc = AmbaDmaChanFunc[DmaChanType];

            if (DmaChanType == AMBA_DMA_CHANNEL_MEM_TRX) {
                AmbaCSL_DmaSetChanFunc(DmaChanNo, 0);
            } else {
                AmbaCSL_DmaSetChanFunc(DmaChanNo, AmbaDmaChanFunc[DmaChanType]);
            }
        }
    }

    return RetVal;
}

#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
/**
 *  AmbaRTSL_DmaGetIntCount - Get interrupt count of a DMA channel
 *  @param[in] DmaChanNo DMA channel number
 *  @return error code
 */
UINT32 AmbaRTSL_DmaGetIntCount(UINT32 DmaChanNo)
{
    return AmbaDmaIntCount[DmaChanNo];
}

/**
 *  AmbaRTSL_DmaResetIntCount - Reset interrupt count of a DMA channel
 *  @param[in] DmaChanNo DMA channel number
 *  @return error code
 */
void AmbaRTSL_DmaResetIntCount(UINT32 DmaChanNo)
{
     AmbaDmaIntCount[DmaChanNo] = 0U;
}
#endif
#pragma GCC push_options
#pragma GCC target("general-regs-only")
/**
 *  DMA0_ISR - Interrupt service routine of DMA controller 0
 *  @param[in] IntID Interrupt ID
 *  @param[in] UserArg Optional argument of the interrupt handler
 */
static void DMA0_ISR(UINT32 IntID, UINT32 UserArg)
{
    AMBA_DMA_CTRL_s *pDmaCtrl;
    volatile AMBA_DMA_CHAN_REG_s *pDmaChanReg;
    UINT32 IrqStatus;
    UINT32 i;
    UINT32 RegIndex = 0;
    struct sigevent event;
    int iid;

    AmbaMisra_TouchUnused(&IntID);
    AmbaMisra_TouchUnused(&UserArg);

    SIGEV_INTR_INIT(&event);
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
    iid = InterruptAttachEvent(117, &event, 0U);
    InterruptUnmask(117, iid);
#else
    iid = InterruptAttachEvent(115, &event, 0U);
    InterruptUnmask(115, iid);
#endif

    while(1) {
        RegIndex = 0;
        InterruptWait(0, NULL);

        IrqStatus = AmbaCSL_DmaGetIrqStatus(pAmbaDMA_Reg[0]);
        for (i = AMBA_DMA_CHANNEL_0; i < AMBA_DMA_CHANNEL_8; i++) {
            if ((IrqStatus & 0x01U) != 0U) {
                pDmaCtrl = &AmbaDmaCtrl[i];
                pDmaChanReg = &pAmbaDMA_Reg[0]->DmaChanReg[RegIndex];
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
                AmbaDmaIntCount[i]++;
#endif
                /* Interrupt occurred */
                pDmaCtrl->Status = pDmaChanReg->Status; /* The status of the current transaction */
                AmbaCSL_DmaClearStatus(pDmaChanReg);    /* clear all status and IRQ status */

                if (AmbaDmaIsrCallBack != NULL) {
                    AmbaDmaIsrCallBack(i);
                }
            }
            RegIndex++;
            IrqStatus >>= 1U;
        }
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
        InterruptUnmask(117, iid);
#else
        InterruptUnmask(115, iid);
#endif
    }

    return NULL;
}

/**
 *  DMA1_ISR - Interrupt service routine of DMA controller 1
 *  @param[in] IntID Interrupt ID
 *  @param[in] UserArg Optional argument of the interrupt handler
 */
static void DMA1_ISR(UINT32 IntID, UINT32 UserArg)
{
    AMBA_DMA_CTRL_s *pDmaCtrl = NULL;
    volatile AMBA_DMA_CHAN_REG_s *pDmaChanReg;
    UINT32 IrqStatus;
    UINT32 i;
    UINT32 RegIndex = 0;
    struct sigevent event;
    int iid;

    AmbaMisra_TouchUnused(&IntID);
    AmbaMisra_TouchUnused(&UserArg);

    SIGEV_INTR_INIT(&event);
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
    iid = InterruptAttachEvent(118, &event, 0U);
    InterruptUnmask(118, iid);
#else
    iid = InterruptAttachEvent(116, &event, 0U);
    InterruptUnmask(116, iid);
#endif
    while(1) {
        RegIndex = 0;
        InterruptWait(0, NULL);

        IrqStatus = AmbaCSL_DmaGetIrqStatus(pAmbaDMA_Reg[1]);
        for (i = AMBA_DMA_CHANNEL_8; i < AMBA_NUM_DMA_CHANNEL; i++) {
            if ((IrqStatus & 0x01U) != 0U) {
                pDmaCtrl = &AmbaDmaCtrl[i];
                pDmaChanReg = &pAmbaDMA_Reg[1]->DmaChanReg[RegIndex];
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
                AmbaDmaIntCount[i]++;
#endif
                /* Interrupt occurred */
                pDmaCtrl->Status = pDmaChanReg->Status; /* The status of the current transaction */
                AmbaCSL_DmaClearStatus(pDmaChanReg);    /* clear all status and IRQ status */

                if (AmbaDmaIsrCallBack != NULL) {
                    AmbaDmaIsrCallBack(i);
                }
            }
            RegIndex++;
            IrqStatus >>= 1U;
        }
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
        InterruptUnmask(118, iid);
#else
        InterruptUnmask(116, iid);
#endif
    }
}
#pragma GCC pop_options
