/**
 *  @file AppPCIE_Rc.c
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
 *  @details application for PCIE root complex mode.
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
#include "AmbaRTSL_TMR.h"

//#define RC_DMA_INTERRUPT

void AppPcie_RcInit(void)
{
    IOUT_MEMORY_INFO_s *noncache_mem_info = AmbaIOUT_NoncacheMemInfoGet(IOUT_MEMID_PCIE);
    PCIE_MEM_CONFIG_s mem_parm = {
        .StackNonCacheMemory     = noncache_mem_info->Ptr,
        .StackNonCacheMemorySize = noncache_mem_info->Size,
    };
    PCIE_INIT_PARAM_s sys_parm = {
        .MemConfig = &mem_parm,
        .Mode      = PCIE_RC_MODE,
        .Gen       = PCIE_CTRL_GEN,
        .Lane      = PCIE_LANE_WIDTH,
        .ClockSrc  = PCIE_CLK_SRC,
#ifdef RC_DMA_INTERRUPT
        .IsrEnable = PCIE_ISR_ENABLE,
#else
        .IsrEnable = PCIE_ISR_DISABLE,
#endif
    };
    UINT64 inbound_addr;
    PCIER_IB_PARAM_s ib_parm;
    PCIER_OB_PARAM_s ob_parm;
    UINT32 ret;

    // Init driver
    ret = AmbaPCIE_Init(&sys_parm);
    if (ret != 0U) {
        AmbaPrint_PrintUInt5("[ERROR][RC] AmbaPCIE_Init(): ret = 0x%x", ret, 0, 0, 0, 0);
    }

    // Start link training
    ret = AmbaPCIE_Start();
    if (ret != 0U) {
        AmbaPrint_PrintUInt5("[ERROR][RC] AmbaPCIE_Start(): ret = 0x%x", ret, 0, 0, 0, 0);
    }

    // Enable host BAR0 for inbound
    AmbaPCIE_MemAlloc(UT_RP_IB_REGION0_SIZE, &inbound_addr);
    AmbaPrint_PrintUInt5("[RC] inbound_addr = 0x%x.%x ",
                         (UINT32)(inbound_addr >> 32),
                         (UINT32)(inbound_addr & 0xFFFFFFFF), 0U, 0U, 0U);

    ib_parm.MemAddr     = inbound_addr;
    ib_parm.PciAddr     = UT_RP_IB_REGION0_PCI;
    ib_parm.Size        = UT_RP_IB_REGION0_SIZE;
    ret = AmbaPCIER_Inbound(&ib_parm); // inbound
    if (ret != 0U) {
        AmbaPrint_PrintUInt5("[ERROR][RC] AmbaPCIER_Inbound(): ret = 0x%x", ret, 0, 0, 0, 0);
    }

    // Enable endpoint BAR0 for outbound
    ob_parm.AxiAddr     = UT_RP_OB_REGION1_AXI_OFFSET;
    ob_parm.PciAddr     = UT_RP_OB_REGION1_PCI;
    ob_parm.Size        = UT_RP_OB_REGION1_SIZE;
    ret = AmbaPCIER_Outbound(&ob_parm); // 16KB
    if (ret != 0U) {
        AmbaPrint_PrintUInt5("[ERROR][RC] AmbaPCIER_Outbound(): ret = 0x%x", ret, 0, 0, 0, 0);
    }
}

void AppPcie_RcWrite(UINT64 addr, UINT32 value)
{
    IO_UtilityRegWrite32(addr, value); // 4-byte write
}

UINT32 AppPcie_RcRead(UINT64 addr)
{
    return IO_UtilityRegRead32(addr); // 4-byte read
}

#ifdef RC_DMA_INTERRUPT
static void AppPcie_RcDmaCb(UINT32 status)
{
    AmbaPrint_PrintUInt5(" DMA callback status = 0x%x", status, 0, 0, 0, 0);
}
#endif

void AppPcie_RcDmaWrite(void)
{
    PCIE_DMA_XFER_PARAM_s dma_params;
    UINT32 ret;
    static UINT64 local_addr = 0;
    if (local_addr == 0U) {
        (void)AmbaPCIE_MemAlloc(1024, &local_addr);
    }
    AmbaPrint_PrintUInt5("[RC] local_addr = 0x%x.%x ",
                         (UINT32)(local_addr >> 32U),
                         (UINT32)(local_addr & 0xFFFFFFFFU), 0U, 0U, 0U);

    // assign a magic number
    IO_UtilityRegWrite32(local_addr, 0xCCDDCCDD);
    // ob dma
    dma_params.AxiAddr = local_addr;
    dma_params.PciAddr = UT_RP_OB_REGION1_PCI;
    dma_params.Size = 0x100;
    dma_params.Rw = UT_PCIE_DMA_WRITE;
#ifdef RC_DMA_INTERRUPT
    dma_params.Func = AppPcie_RcDmaCb;
#else
    dma_params.Func = NULL;
#endif
    ret = AmbaPCIE_DmaXfer(&dma_params);
    if (ret != 0U) {
        AmbaPrint_PrintUInt5("[ERROR][RC] AmbaPCIE_DmaXfer(): ret = 0x%x", ret, 0, 0, 0, 0);
    }
#ifndef RC_DMA_INTERRUPT
    // polling mode
    ret = AmbaPCIE_WaitDmaXferDone();
    if (ret != 0U) {
        AmbaPrint_PrintUInt5("[ERROR][RC] AmbaPCIE_WaitDmaXferDone(): ret = 0x%x", ret, 0, 0, 0, 0);
    }
    // read back data through mmap
    // NOTE: when switch outbound to inboud DMA, we need to keep AXI touch number in even number,
    // or inbound dma transfer would fail. why?
    AmbaPrint_PrintUInt5("[RC] read back data = 0x%x 0x%x",
                         IO_UtilityRegRead32(UT_RP_OB_REGION1_AXI_ADDR),
                         IO_UtilityRegRead32(UT_RP_OB_REGION1_AXI_ADDR + 4), 0U, 0U, 0U);
#endif
}

void AppPcie_RcDmaRead(void)
{
    PCIE_DMA_XFER_PARAM_s dma_params;
    UINT32 ret;
    static UINT64 local_addr = 0;
    if (local_addr == 0U) {
        (void)AmbaPCIE_MemAlloc(1024, &local_addr);
    }
    AmbaPrint_PrintUInt5("[RC] local_addr = 0x%x.%x ",
                         (UINT32)(local_addr >> 32U),
                         (UINT32)(local_addr & 0xFFFFFFFFU), 0U, 0U, 0U);

    // update the data content on device.
    IO_UtilityRegWrite32(UT_RP_OB_REGION1_AXI_ADDR, 0xCCDDCCDD);

    // NOTE: For RC, before doing inbound DMA, we need to keep AXI touch number in even number,
    // or inbound dma transfer would fail. why?

    // assign a magic number
    IO_UtilityRegWrite32(local_addr, 0xAABBCCDD);
    // ob dma
    dma_params.AxiAddr = local_addr;
    dma_params.PciAddr = UT_RP_OB_REGION1_PCI;
    dma_params.Size = 0x100;
    dma_params.Rw = UT_PCIE_DMA_READ;
#ifdef RC_DMA_INTERRUPT
    dma_params.Func = AppPcie_RcDmaCb;
#else
    dma_params.Func = NULL;
#endif
    ret = AmbaPCIE_DmaXfer(&dma_params);
    if (ret != 0U) {
        AmbaPrint_PrintUInt5("[ERROR][RC] AmbaPCIE_DmaXfer(): ret = 0x%x", ret, 0, 0, 0, 0);
    }
#ifndef RC_DMA_INTERRUPT
    // polling mode
    ret = AmbaPCIE_WaitDmaXferDone();
    if (ret != 0U) {
        AmbaPrint_PrintUInt5("[ERROR][RC] AmbaPCIE_WaitDmaXferDone(): ret = 0x%x", ret, 0, 0, 0, 0);
    }
    // read back data through mmap
    AmbaPrint_PrintUInt5("[RC] read back data = 0x%x ", IO_UtilityRegRead32(local_addr), 0U, 0U, 0U, 0U);
#endif
}
