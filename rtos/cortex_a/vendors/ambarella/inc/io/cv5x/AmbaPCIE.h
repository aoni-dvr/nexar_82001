/**
 *  @file AmbaPCIE.h
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
 *  @details PCIE API header file.
 */

#ifndef AMBAPCIE_H
#define AMBAPCIE_H

// mode selection
#define PCIE_RC_MODE 1U
#define PCIE_EP_MODE 0U

// pcie generation. 0 means default gen 3
#define PCIE_CTRL_GEN 0

// pcie land width. It could be 1, 2 and 4
#define PCIE_LANE_WIDTH 4U

// clock source. 0 means external 100 MHz. 1 means internal 24MHz
#define PCIE_CLK_SRC 0

// dma direction
#define PCIE_DMA_READ  0U
#define PCIE_DMA_WRITE 1U

// ISR option
#define PCIE_ISR_ENABLE  1U
#define PCIE_ISR_DISABLE 0U

#define PCIE_CFG_BASE                 0x2040000000UL
#define PCIE_AXI_BASE                 0x2000000000UL

#define PCIE_RP_OB_REGION0_AXI_OFFSET 0x01000000UL // offset from axi base (store device configure)
#define PCIE_RP_OB_REGION0_AXI_ADDR   (PCIE_AXI_BASE + PCIE_RP_OB_REGION0_AXI_OFFSET)
#define PCIE_RP_OB_REGION0_PCI        0x71000000


typedef void  (*DmaCb) (UINT32 status);

typedef struct {
    UINT8    *StackCacheMemory;       // Cache memory pool address
    UINT32   StackCacheMemorySize;    // Cache memory size
    UINT8    *StackNonCacheMemory;    // Non-cache memory pool address
    UINT32   StackNonCacheMemorySize; // Non-cache memory size
} PCIE_MEM_CONFIG_s;

typedef struct {
    PCIE_MEM_CONFIG_s *MemConfig;
    UINT8 Mode;
    UINT8 Gen;
    UINT8 Lane;
    UINT8 ClockSrc;
    UINT8 IsrEnable;
} PCIE_INIT_PARAM_s;

typedef struct {
    UINT64 AxiAddr;
    UINT64 PciAddr;
    UINT32 Size;    // transfer size
    UINT8  Rw;      // DMA read (0) or write (1)
    DmaCb  Func;    // callback when DMA done
} PCIE_DMA_XFER_PARAM_s;

typedef struct {
    UINT64 MemAddr;
    UINT64 PciAddr;
    UINT64 Size;
} PCIER_IB_PARAM_s;

typedef struct {
    UINT64 AxiAddr;
    UINT64 PciAddr;
    UINT64 Size;
} PCIER_OB_PARAM_s;

typedef struct {
    UINT64 MemAddr;
    UINT64 Size;
} PCIED_IB_PARAM_s;

typedef struct {
    UINT64 AxiAddr;
    UINT64 PciAddr;
    UINT64 Size;
} PCIED_OB_PARAM_s;

// General APIs
UINT32 AmbaPCIE_Init(const PCIE_INIT_PARAM_s *params);
UINT32 AmbaPCIE_Start(void);
UINT32 AmbaPCIE_MemAlloc(UINT32 size, UINT64 *addr);
UINT32 AmbaPCIE_DmaXfer(const PCIE_DMA_XFER_PARAM_s *params);
UINT32 AmbaPCIE_WaitDmaXferDone(void);

// Root APIs
UINT32 AmbaPCIER_Inbound(const PCIER_IB_PARAM_s *params);
UINT32 AmbaPCIER_Outbound(const PCIER_OB_PARAM_s *params);

// Endpoint APIs
UINT32 AmbaPCIED_Inbound(const PCIED_IB_PARAM_s *params);
UINT32 AmbaPCIED_Outbound(const PCIED_OB_PARAM_s *params);

/*------------------------------ *
 *        Deprecated APIs        *
 *-------------------------------*/
#define PCIE_RP_OB_REGION0_AXI 0x01000000 // offset from axi base (store device configure)
#define PCIE_RP_OB_REGION0_PCI 0x71000000
void AmbaPCIE_LinkInit(UINT32 mode, UINT32 gen, UINT32 use24mhz);
void AmbaPCIE_LinkStart(UINT32 mode, UINT32 gen, UINT32 use24mhz);
UINT32 AmbaPCIE_Ep_Init (UINT64 ep_base);
UINT32 AmbaPCIE_Ep_MemAccessFromRp(UINT64 AxiAddr, UINT8 NumPassBits);
UINT32 AmbaPCIE_Ep_MemAccessToRp(UINT64 AxiAddr, UINT64 PciAddr, UINT8 NumPassBits);
UINT32 AmbaPCIE_Rp_Init(UINT64 rp_base, UINT64 axi_base);
UINT32 AmbaPCIE_Rp_MemAccessToEp(UINT64 AxiAddr, UINT64 PciAddr, UINT8 NumPassBits);
UINT32 AmbaPCIE_Rp_MemAccessFromEp(UINT64 AxiAddr, UINT64 PciAddr, UINT8 NumPassBits);
UINT32 AmbaPCIE_Rp_GetEpBarInfo(void);

#endif

