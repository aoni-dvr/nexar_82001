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
#include "AmbaRTSL_DMA.h"
#include "AmbaRTSL_DRAMC.h"
#include "AmbaCSL_DMA.h"

#ifdef CONFIG_QNX
#include <sys/siginfo.h>
#include "AmbaCache.h"
#else
#include "AmbaRTSL_GIC.h"
#include "AmbaRTSL_Cache.h"
#endif

#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
#include "AmbaCortexA76.h"
#else
#include "AmbaCortexA53.h"
#endif

#if defined(CONFIG_DEVICE_TREE_SUPPORT)
#include <AmbaIOUtility.h>
#endif

AMBA_DMA_REG_s *pAmbaDMA_Reg[2];

typedef struct {
    UINT32  Status;         /* status of current DMA transaction */
    UINT32  DmaChanFunc;
} AMBA_DMA_CTRL_s;

static void DMA0_ISR(UINT32 IntID, UINT32 UserArg);
static void DMA1_ISR(UINT32 IntID, UINT32 UserArg);

static AMBA_DMA_ISR_CALLBACK_f AmbaDmaIsrCallBack = NULL;
static AMBA_DMA_CTRL_s AmbaDmaCtrl[AMBA_NUM_DMA_CHANNEL];
static UINT32 AmbaDmaIntCount[AMBA_NUM_DMA_CHANNEL];

static const UINT32 AmbaDmaChanFunc[AMBA_NUM_DMA_CHANNEL_TYPE] = {
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
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
    [AMBA_DMA_CHANNEL_SPI5_TX      ] = (25U),
    [AMBA_DMA_CHANNEL_SPI5_RX      ] = (26U),
#elif defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
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
    [AMBA_DMA_CHANNEL_I2S1_TX      ] = (23U),
    [AMBA_DMA_CHANNEL_I2S1_RX      ] = (24U),
    [AMBA_DMA_CHANNEL_SPI4_TX      ] = (25U),
    [AMBA_DMA_CHANNEL_SPI4_RX      ] = (26U),
    [AMBA_DMA_CHANNEL_SPI5_TX      ] = (27U),
    [AMBA_DMA_CHANNEL_SPI5_RX      ] = (28U),
#elif defined(CONFIG_SOC_CV28)
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
    [AMBA_DMA_CHANNEL_I2S1_TX      ] = (23U),
    [AMBA_DMA_CHANNEL_I2S1_RX      ] = (24U),
#elif defined(CONFIG_SOC_CV25) || defined(CONFIG_SOC_H32)
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
#else
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

#ifdef CONFIG_ARM32
/**
 *  DMA_GetPhysAddr - Translate a virtual address to a physical address
 *  @param[in] pVirtAddr A virtual address
 *  @return Translated physical address
 */
static UINT32 DMA_GetPhysAddr(UINT32 DmaChanNo, const void *pVirtAddr)
{
    UINT32 VirtAddr32, PhysAddr32;

    (void)DmaChanNo;
    AmbaMisra_TypeCast32(&VirtAddr32, &pVirtAddr);
    (void)AmbaMMU_Virt32ToPhys32(VirtAddr32, &PhysAddr32);

    return PhysAddr32;
}

/**
 *  DMA_CacheClean - Do a cache clean operation
 *  @param[in] pVirtAddr A virtual address
 *  @param[in] Size Number of bytes
 */
static void DMA_CacheClean(const void *pVirtAddr, UINT32 Size)
{
    UINT32 VirtAddr32;

    AmbaMisra_TypeCast32(&VirtAddr32, &pVirtAddr);
#ifdef CONFIG_QNX
    (void)AmbaCache_DataClean(VirtAddr32, Size);
#else
    (void)AmbaRTSL_CacheCleanData(VirtAddr32, Size);
#endif
}
#endif

#ifdef CONFIG_ARM64
static UINT32 DMA_GetAttClientAddr(UINT32 DmaChanNo, const ULONG VirtAddr)
{
    ULONG Addr = 0UL;
#if !defined(CONFIG_SOC_CV2FS) && !defined(CONFIG_SOC_CV22FS)
    if (DmaChanNo < AMBA_DMA_CHANNEL_8) {
        (void)AmbaRTSL_DramQueryAttP2V(VirtAddr, AMBA_DRAM_CLIENT_DMA0, &Addr);
    } else {
        (void)AmbaRTSL_DramQueryAttP2V(VirtAddr, AMBA_DRAM_CLIENT_DMA1, &Addr);
    }
#else
    (void)DmaChanNo;
    Addr = VirtAddr;
#endif
    return (UINT32)Addr;
}

/**
 *  DMA_GetPhysAddr - Translate a virtual address to a physical address
 *  @param[in] pVirtAddr A virtual address
 *  @return Translated physical address
 */
static UINT32 DMA_GetPhysAddr(UINT32 DmaChanNo, const void *pVirtAddr)
{
    ULONG VirtAddr64, PhysAddr64;
    UINT32 PhysAddr32;

    AmbaMisra_TypeCast(&VirtAddr64, &pVirtAddr);
    (void)AmbaMMU_VirtToPhys(VirtAddr64, &PhysAddr64);
    /* Get address value from ATT table. */
    if (VirtAddr64 < 0x200000000UL) {
        PhysAddr64 = DMA_GetAttClientAddr(DmaChanNo, PhysAddr64);
    }
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

    AmbaMisra_TypeCast(&VirtAddr64, &pVirtAddr);
#ifdef CONFIG_QNX
    (void)AmbaCache_DataClean((UINT32)VirtAddr64, Size);
#else
    (void)AmbaRTSL_CacheCleanData((UINT32)VirtAddr64, Size);
#endif
}
#endif

/**
 *  DMA_TransferDesc - Do dma transfers with descriptor mode
 *  @param[in] DmaChanNo DMA channel number
 *  @param[in] pDmaChanReg pointer to dma channel control registers
 *  @param[in] pDmaChanDescReg pointer to dma channel descriptor registers
 *  @param[in] pDmaDesc pointer to a linked list of dma descriptors
 */
static void DMA_TransferDesc(UINT32 DmaChanNo, volatile AMBA_DMA_CHAN_REG_s *pDmaChanReg, volatile UINT32 *pDmaChanDescReg, AMBA_DMA_DESC_s *pDmaDesc)
{
    UINT32 StartDescPhysAddr = DMA_GetPhysAddr(DmaChanNo, pDmaDesc);
    AMBA_DMA_DESC_s *pWorkDmaDesc = pDmaDesc;
    const void *pNextDesc;

    *pDmaChanDescReg = DMA_GetPhysAddr(DmaChanNo, pWorkDmaDesc);

    while (pWorkDmaDesc != NULL) {
        pNextDesc = pWorkDmaDesc->pNextDesc;

        /* Get address value from cortex MMU table. */
        pWorkDmaDesc->PrivData[0] = DMA_GetPhysAddr(DmaChanNo, pWorkDmaDesc->pSrcAddr);
        pWorkDmaDesc->PrivData[1] = DMA_GetPhysAddr(DmaChanNo, pWorkDmaDesc->pDstAddr);
        pWorkDmaDesc->PrivData[2] = DMA_GetPhysAddr(DmaChanNo, pWorkDmaDesc->pNextDesc);
        pWorkDmaDesc->PrivData[3] = DMA_GetPhysAddr(DmaChanNo, pWorkDmaDesc->pStatus);

        DMA_CacheClean(pWorkDmaDesc, (UINT32)sizeof(AMBA_DMA_DESC_s));

        /* Loop detection */
        if (pWorkDmaDesc->PrivData[2] == StartDescPhysAddr) {
            break;
        }

#ifdef CONFIG_ARM32
        AmbaMisra_TypeCast32(&pWorkDmaDesc, &pNextDesc);
#endif

#ifdef CONFIG_ARM64
#ifdef __LP64__
        /* LP64 */
        AmbaMisra_TypeCast(&pWorkDmaDesc, &pNextDesc);
#else
        /* ILP32 */
        AmbaMisra_TypeCast32(&pWorkDmaDesc, &pNextDesc);
#endif
#endif
    }

    /* DmaCtrl.DescMode = 1U, DmaCtrl.Enable = 1U */
    pDmaChanReg->Ctrl = 0xC0000000U;
}

/**
 *  DMA_TransferNonDesc - Do dma transfers with non-descriptor mode
 *  @param[in] DmaChanNo DMA channel number
 *  @param[in] pDmaChanReg pointer to dma channel control registers
 *  @param[in] pDmaDesc pointer to a linked list of dma descriptors
 */
static void DMA_TransferNonDesc(UINT32 DmaChanNo, volatile AMBA_DMA_CHAN_REG_s *pDmaChanReg, const AMBA_DMA_DESC_s *pDmaDesc)
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

#ifdef CONFIG_QNX
    AmbaMisra_TypeCast32(&pDmaChanReg->SrcAddr, &pDmaDesc->pSrcAddr);
    AmbaMisra_TypeCast32(&pDmaChanReg->DstAddr, &pDmaDesc->pDstAddr);
#else
    /* Get address value from cortex MMU table. */
    pDmaChanReg->SrcAddr = DMA_GetPhysAddr(DmaChanNo, pDmaDesc->pSrcAddr);
    pDmaChanReg->DstAddr = DMA_GetPhysAddr(DmaChanNo, pDmaDesc->pDstAddr);
#endif
    pDmaChanReg->Ctrl       = DmaCtrlRegVal;
}

/**
 *  AmbaRTSL_DmaInit - DMA module initialization
 */
void AmbaRTSL_DmaInit(void)
{
    AMBA_DMA_CTRL_s *pDmaCtrl;
#ifndef CONFIG_QNX
    AMBA_INT_CONFIG_s IntConfig;
#endif
    volatile AMBA_DMA_CHAN_REG_s *pDmaChanReg[2];
    UINT32 i, IntID;
    ULONG base_addr;
    INT32 offset = 0;
    (void) offset;

#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
    base_addr = AMBA_CA53_DMA0_BASE_ADDR;
    AmbaMisra_TypeCast(&pAmbaDMA_Reg[0], &base_addr);
    base_addr = AMBA_CA53_DMA1_BASE_ADDR;
    AmbaMisra_TypeCast(&pAmbaDMA_Reg[1], &base_addr);
#elif defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    base_addr = AMBA_CORTEX_A76_DMA0_BASE_ADDR;
    AmbaMisra_TypeCast(&pAmbaDMA_Reg[0], &base_addr);
    base_addr = AMBA_CORTEX_A76_DMA1_BASE_ADDR;
    AmbaMisra_TypeCast(&pAmbaDMA_Reg[1], &base_addr);
#else
    base_addr = AMBA_CORTEX_A53_DMA0_BASE_ADDR;
    AmbaMisra_TypeCast(&pAmbaDMA_Reg[0], &base_addr);
    base_addr = AMBA_CORTEX_A53_DMA1_BASE_ADDR;
    AmbaMisra_TypeCast(&pAmbaDMA_Reg[1], &base_addr);
#endif

#if defined(CONFIG_DEVICE_TREE_SUPPORT)
    for (i = 0U; i < AMBA_NUM_DMA_MODULE; i++) {
        if (i != 0U) {
            offset = IO_UtilityFDTNodeOffsetByCID(offset, "ambarella,dma");
        }
        base_addr = IO_UtilityFDTPropertyU32Quick(offset, "ambarella,dma", "reg", 0U);
        if ( base_addr != 0U ) {
#if defined (CONFIG_SOC_CV5)|| defined (CONFIG_SOC_CV52)
        base_addr = base_addr | AMBA_CORTEX_A76_AHB_PHYS_BASE_ADDR;
#endif
            AmbaMisra_TypeCast(&pAmbaDMA_Reg[i], &base_addr);
        }
    }
#endif

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

    /* Hook interrupt handler */
#ifndef CONFIG_QNX
    IntConfig.TriggerType  = INT_TRIG_HIGH_LEVEL;
    IntConfig.IrqType      = INT_TYPE_FIQ;      /* Since Linux does not suuport GIC group1 interrupt handling, set it as FIQ for AmbaLink. */
    IntConfig.CpuTargets   = 0x01U;             /* Target cores */

#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
    IntID = AMBA_INT_SPI_ID117_DMA_ENGINE0;
#elif defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    IntID = AMBA_INT_SPI_ID117_DMA0;
#else
    IntID = AMBA_INT_SPI_ID115_DMA0;
#endif
    (void)AmbaRTSL_GicIntConfig(IntID, &IntConfig, DMA0_ISR, 0U);
    (void)AmbaRTSL_GicIntEnable(IntID);

#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
    IntID = AMBA_INT_SPI_ID118_DMA_ENGINE1;
#elif defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    IntID = AMBA_INT_SPI_ID118_DMA1;
#else
    IntID = AMBA_INT_SPI_ID116_DMA1;
#endif
    (void)AmbaRTSL_GicIntConfig(IntID, &IntConfig, DMA1_ISR, 1U);
    (void)AmbaRTSL_GicIntEnable(IntID);

#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
#else
#if defined(CONFIG_ENABLE_AMBALINK)
    /* let linux handle dma1 */
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    AmbaRTSL_GicSetCpuTarget(AMBA_INT_SPI_ID118_DMA1, 0x1U << CONFIG_BOOT_CORE_LINUX);
#else
    AmbaRTSL_GicSetCpuTarget(AMBA_INT_SPI_ID116_DMA1, 0x1U << CONFIG_BOOT_CORE_LINUX);
#endif
#endif
#endif
#else
    IntID = 115;
    (void)InterruptMask(IntID, -1);

    IntID = 116;
    (void)InterruptMask(IntID, -1);

#if 0 //(TODO)
#if defined(CONFIG_ENABLE_AMBALINK)
    /* let linux handle dma1 */
    AmbaRTSL_GicSetCpuTarget(AMBA_INT_SPI_ID116_DMA1, 0x1U << CONFIG_BOOT_CORE_LINUX);
#endif
#endif
#endif
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
    AMBA_DMA_REG_s *pDmaReg;
    AMBA_DMA_CHAN_REG_s *pDmaChanReg;
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

#ifdef CONFIG_QNX
        if (DmaChanNo < AMBA_DMA_CHANNEL_8) {
            pthread_create(NULL, NULL, DMA0_ISR, NULL);
        } else {
            pthread_create(NULL, NULL, DMA1_ISR, NULL);
        }
#endif
        if (pDmaDesc->pNextDesc == NULL) {
            /* Use non-descritpor mode */
            DMA_TransferNonDesc(DmaChanNo, pDmaChanReg, pDmaDesc);
        } else {
            /* Use descritpor mode */
            pDmaChanDescReg = &pDmaReg->DmaChanDescAddr[DmaChanOffset];
            DMA_TransferDesc(DmaChanNo, pDmaChanReg, pDmaChanDescReg, pDmaDesc);
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
            /* stop DMA by sending early end request */
            pDmaReg->EarlyEndReq = (UINT32)((UINT32)1U << DmaChanNo);
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
            AmbaMisra_TypeCast(&pDmaReg, &pAmbaDMA_Reg[0]);
            DmaChanOffset = DmaChanNo;
        } else {
            AmbaMisra_TypeCast(&pDmaReg, &pAmbaDMA_Reg[1]);
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

/**
 *  AmbaRTSL_DmaGetIrqStatus - Get interrupt status of a DMA channel
 *  @param[in] DmaChanNo DMA channel number
 *  @return error code
 */
UINT32 AmbaRTSL_DmaGetIrqStatus(UINT32 DmaChanNo)
{
    UINT32 RegVal, BitMask;

    if (DmaChanNo < AMBA_DMA_CHANNEL_8) {
        RegVal = AmbaCSL_DmaGetIrqStatus(pAmbaDMA_Reg[0]);
        BitMask = ((UINT32)0x1U << DmaChanNo);
    } else {
        RegVal = AmbaCSL_DmaGetIrqStatus(pAmbaDMA_Reg[1]);
        BitMask = ((UINT32)0x1U << (DmaChanNo - AMBA_DMA_CHANNEL_8));
    }

    return (RegVal & BitMask);
}

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
#ifdef CONFIG_QNX
    struct sigevent event;
    int iid;
#endif
    AmbaMisra_TouchUnused(&IntID);
    AmbaMisra_TouchUnused(&UserArg);

#ifdef CONFIG_QNX
    SIGEV_INTR_INIT(&event);
    iid = InterruptAttachEvent(115, &event, 0U);
    InterruptUnmask(115, iid);

    InterruptWait(0, NULL);
#endif
    IrqStatus = AmbaCSL_DmaGetIrqStatus(pAmbaDMA_Reg[0]);
    for (i = AMBA_DMA_CHANNEL_0; i < AMBA_DMA_CHANNEL_8; i++) {
        if ((IrqStatus & 0x01U) != 0U) {
            pDmaCtrl = &AmbaDmaCtrl[i];
            pDmaChanReg = &pAmbaDMA_Reg[0]->DmaChanReg[RegIndex];
            AmbaDmaIntCount[i]++;

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
#ifdef CONFIG_QNX
    pthread_exit(NULL);
#endif
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
#ifdef CONFIG_QNX
    struct sigevent event;
    int iid;
#endif

    AmbaMisra_TouchUnused(&IntID);
    AmbaMisra_TouchUnused(&UserArg);

#ifdef CONFIG_QNX
    SIGEV_INTR_INIT(&event);
    iid = InterruptAttachEvent(116, &event, 0U);
    InterruptUnmask(116, iid);

    InterruptWait(0, NULL);
#endif

    IrqStatus = AmbaCSL_DmaGetIrqStatus(pAmbaDMA_Reg[1]);
    for (i = AMBA_DMA_CHANNEL_8; i < AMBA_NUM_DMA_CHANNEL; i++) {
        if ((IrqStatus & 0x01U) != 0U) {
            pDmaCtrl = &AmbaDmaCtrl[i];
            pDmaChanReg = &pAmbaDMA_Reg[1]->DmaChanReg[RegIndex];
            AmbaDmaIntCount[i]++;

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

#ifdef CONFIG_QNX
    pthread_exit(NULL);
#endif
}
#pragma GCC pop_options
