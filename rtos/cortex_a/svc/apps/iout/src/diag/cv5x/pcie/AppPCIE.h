/**
 *  @file AppPCIE.h
 *
 *  Copyright (c) 2021 Ambarella International LP
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
 *  @details PCIe main functions header file.
 */

#ifndef __APPPCIE_H__
#define __APPPCIE_H__

#include <AmbaPCIE.h>

#define UT_PCIE_CFG_BASE              PCIE_CFG_BASE
#define UT_PCIE_AXI_BASE              PCIE_AXI_BASE

#define UT_PCIE_DMA_READ              PCIE_DMA_READ
#define UT_PCIE_DMA_WRITE             PCIE_DMA_WRITE

/* Amba RC -> Amba EP */
#define UT_RP_OB_REGION1_AXI_OFFSET   0x0002000000
#define UT_RP_OB_REGION1_AXI_ADDR     UT_PCIE_AXI_BASE + UT_RP_OB_REGION1_AXI_OFFSET
#define UT_RP_OB_REGION1_PCI          0x72000000 // ob region1
#define UT_RP_OB_REGION1_SIZE         16*1024

#define UT_EP_IB_REGION0_SIZE         UT_RP_OB_REGION1_SIZE

/* Amba EP -> Amba RC */
#define UT_RP_IB_REGION0_PCI          0x73000000 // ib region0
#define UT_RP_IB_REGION0_SIZE         16*1024

#define UT_EP_OB_REGION0_AXI_OFFSET   0x0002000000
#define UT_EP_OB_REGION0_AXI_ADDR     UT_PCIE_AXI_BASE + UT_EP_OB_REGION0_AXI_OFFSET
#define UT_EP_OB_REGION0_PCI          UT_RP_IB_REGION0_PCI
#define UT_EP_OB_REGION0_SIZE         UT_RP_IB_REGION0_SIZE


void AppPcie_RcInit(void);
void AppPcie_RcDmaWrite(void);
void AppPcie_RcDmaRead(void);

void AppPcie_EpInit(void);
void AppPcie_EpDmaWrite(void);
void AppPcie_EpDmaRead(void);

void AppPCIE_ProgramRc(void);
void AppPCIE_ProgramEp(void);

void AppPCIE_BootActiveEp(void);
void AppPCIE_BootPassiveEp(void);

extern void IO_UtilityRegWrite32(UINT64 Address, UINT32 Value);
extern UINT32 IO_UtilityRegRead32(UINT64 Address);

#endif
