/**
 *  @file AppPCIE_Ep.c
 *
 *  @copyright Copyright (c) 2021 Ambarella, Inc.
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
 *  @details application for PCIE endpoint mode.
 */

#ifndef AMBA_KAL_H
#include <AmbaKAL.h>
#endif
#include <AmbaUtility.h>
#include <AmbaShell.h>
#include <AmbaIOUTDiag.h>
#include "AmbaPrint.h"
#include "AmbaMisraFix.h"
#include "AppPCIE.h"

//#define EP_DMA_INTERRUPT

void AppPcie_EpInit(void)
{
    // get non-cache memory pool
    IOUT_MEMORY_INFO_s *noncache_mem_info = AmbaIOUT_NoncacheMemInfoGet(IOUT_MEMID_PCIE);
    PCIE_MEM_CONFIG_s mem_parm = {
        .StackNonCacheMemory     = noncache_mem_info->Ptr,
        .StackNonCacheMemorySize = noncache_mem_info->Size,
    };
    PCIE_INIT_PARAM_s sys_parm = {
        .MemConfig = &mem_parm,
        .Mode      = PCIE_EP_MODE,
        .Gen       = PCIE_CTRL_GEN,
        .Lane      = PCIE_LANE_WIDTH,
        .ClockSrc  = PCIE_CLK_SRC,
#ifdef EP_DMA_INTERRUPT
        .IsrEnable = PCIE_ISR_ENABLE,
#else
        .IsrEnable = PCIE_ISR_DISABLE,
#endif
    };
    static UINT8 ib_mem[UT_EP_IB_REGION0_SIZE] __attribute__ ((aligned(UT_EP_IB_REGION0_SIZE))) __attribute__((section(".bss.noinit")));
    UINT64 inbound_addr;
    PCIED_IB_PARAM_s ib_parm;
    PCIED_OB_PARAM_s ob_parm;
    UINT32 ret;

    // Init driver
    ret = AmbaPCIE_Init(&sys_parm);
    if (ret != 0U) {
        AmbaPrint_PrintUInt5("[ERROR][EP] AmbaPCIE_Init(): ret = 0x%x", ret, 0, 0, 0, 0);
    }

    // Setup endpoint inbound
    //inbound_addr = (UINT64)AmbaPCIE_MemAlloc(UT_EP_IB_REGION0_SIZE);
    inbound_addr = (UINT64)ib_mem;
    AmbaPrint_PrintUInt5("[EP] inbound_addr = 0x%x.%x ",
                         (UINT32)(inbound_addr >> 32U),
                         (UINT32)(inbound_addr & 0xFFFFFFFFU), 0U, 0U, 0U);
    ib_parm.MemAddr = inbound_addr;
    ib_parm.Size    = UT_EP_IB_REGION0_SIZE;
    ret = AmbaPCIED_Inbound(&ib_parm); // 16KB
    if (ret != 0U) {
        AmbaPrint_PrintUInt5("[ERROR][EP] AmbaPCIED_Inbound(): ret = 0x%x", ret, 0, 0, 0, 0);
    }

    // Start link training
    ret = AmbaPCIE_Start();
    if (ret != 0U) {
        AmbaPrint_PrintUInt5("[ERROR][EP] AmbaPCIE_Start(): ret = 0x%x", ret, 0, 0, 0, 0);
    }

    // Setup endpoint outbound. How to know PCI address?
    ob_parm.AxiAddr = UT_EP_OB_REGION0_AXI_OFFSET;
    ob_parm.PciAddr = UT_EP_OB_REGION0_PCI;
    ob_parm.Size    = UT_EP_OB_REGION0_SIZE;
    ret = AmbaPCIED_Outbound(&ob_parm); // 16KB
    if (ret != 0U) {
        AmbaPrint_PrintUInt5("[ERROR][EP] AmbaPCIED_Outbound(): ret = 0x%x", ret, 0, 0, 0, 0);
    }

    //for ep inbound dma, we need to keep AXI touch number in odd number. why?
    IO_UtilityRegRead32(UT_EP_OB_REGION0_AXI_ADDR);
}

void AppPcie_EpWrite(UINT64 addr, UINT32 value)
{
    IO_UtilityRegWrite32(addr, value); // 4-byte write
}

UINT32 AppPcie_EpRead(UINT64 addr)
{
    return IO_UtilityRegRead32(addr); // 4-byte read
}

#ifdef EP_DMA_INTERRUPT
static void AppPcie_EpDmaCb(UINT32 status)
{
    AmbaPrint_PrintUInt5(" DMA callback status = 0x%x", status, 0, 0, 0, 0);
}
#endif

void AppPcie_EpDmaWrite(void)
{
    PCIE_DMA_XFER_PARAM_s dma_params;
    UINT32 ret;
    static UINT64 local_addr = 0;
    if (local_addr == 0U) {
        (void)AmbaPCIE_MemAlloc(1024, &local_addr);
    }
    AmbaPrint_PrintUInt5("[EP] local_addr = 0x%x.%x ",
                         (UINT32)(local_addr >> 32U),
                         (UINT32)(local_addr & 0xFFFFFFFFU), 0U, 0U, 0U);

    // assign a magic number
    IO_UtilityRegWrite32(local_addr, 0xCCDDCCDD);

    // endpoint ob dma
    dma_params.AxiAddr = local_addr;
    dma_params.PciAddr = UT_EP_OB_REGION0_PCI;
    dma_params.Size = 0x100;
    dma_params.Rw = UT_PCIE_DMA_WRITE;
#ifdef EP_DMA_INTERRUPT
    dma_params.Func = AppPcie_EpDmaCb;
#else
    dma_params.Func = NULL;
#endif
    ret = AmbaPCIE_DmaXfer(&dma_params);
    if (ret != 0U) {
        AmbaPrint_PrintUInt5("[ERROR][EP] AmbaPCIE_DmaXfer(): ret = 0x%x", ret, 0, 0, 0, 0);
    }
#ifndef EP_DMA_INTERRUPT
    ret = AmbaPCIE_WaitDmaXferDone();
    if (ret != 0U) {
        AmbaPrint_PrintUInt5("[ERROR][EP] AmbaPCIE_WaitDmaXferDone(): ret = 0x%x", ret, 0, 0, 0, 0);
    }
    // read back data through mmap
    // NOTE: when switch outbound to inboud DMA, we need to keep AXI touch number in odd number,
    // or inbound dma transfer would fail. why?
    AmbaPrint_PrintUInt5("[RP] read back data = 0x%x 0x%x",
                         IO_UtilityRegRead32(UT_EP_OB_REGION0_AXI_ADDR),
                         IO_UtilityRegRead32(UT_EP_OB_REGION0_AXI_ADDR + 4), 0U, 0U, 0U);
#endif
}

void AppPcie_EpDmaRead(void)
{
    PCIE_DMA_XFER_PARAM_s dma_params;
    UINT32 ret;
    static UINT64 local_addr = 0;
    if (local_addr == 0U) {
        (void)AmbaPCIE_MemAlloc(1024, &local_addr);
    }
    AmbaPrint_PrintUInt5("[EP] local_addr = 0x%x.%x ",
                         (UINT32)(local_addr >> 32U),
                         (UINT32)(local_addr & 0xFFFFFFFFU), 0U, 0U, 0U);

    // NOTE: For EP, before doing inbound DMA, we need to keep AXI touch number in odd number,
    // or inbound dma transfer would fail. why?

    // clear a magic number
    IO_UtilityRegWrite32(local_addr, 0xFFFFFFFF);

    // endpoint ib dma
    dma_params.AxiAddr = local_addr;
    dma_params.PciAddr = UT_EP_OB_REGION0_PCI;
    dma_params.Size = 0x100;
    dma_params.Rw = UT_PCIE_DMA_READ;
#ifdef EP_DMA_INTERRUPT
    dma_params.Func = AppPcie_EpDmaCb;
#else
    dma_params.Func = NULL;
#endif
    ret = AmbaPCIE_DmaXfer(&dma_params);
    if (ret != 0U) {
        AmbaPrint_PrintUInt5("[ERROR][EP] AmbaPCIE_DmaXfer(): ret = 0x%x", ret, 0, 0, 0, 0);
    }
#ifndef EP_DMA_INTERRUPT
    ret = AmbaPCIE_WaitDmaXferDone();
    if (ret != 0U) {
        AmbaPrint_PrintUInt5("[ERROR][EP] AmbaPCIE_WaitDmaXferDone(): ret = 0x%x", ret, 0, 0, 0, 0);
    }
    // read back data through mmap
    AmbaPrint_PrintUInt5("[EP] read back data = 0x%x ", IO_UtilityRegRead32(local_addr), 0U, 0U, 0U, 0U);
#endif
}
