/*
 * Copyright (c) 2021 Ambarella International LP
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

#ifndef AMBA_REG_DMA_H
#define AMBA_REG_DMA_H

#ifndef AMBA_TYPES_H
#include "AmbaTypes.h"
#endif

/*
 * DMA: Channel Control Register
 */
typedef struct {
    UINT32  ByteCount:          22;     /* [21:0]: DMA Transfer Byte Count */
    UINT32  BusDataSize:        2;      /* [23:22]: Bus Data Transfer size */
    UINT32  BusBlockSize:       3;      /* [26:24]: Bus Transaction Block size */
    UINT32  NoBusAddrInc:       1;      /* [27]: No Bus Address Increment Flag */
    UINT32  ReadMem:            1;      /* [28]: 1 - Read from Memory; 0 - Read from I/O */
    UINT32  WriteMem:           1;      /* [29]: 1 - Write to Memory; 0 - Write to I/O  */
    UINT32  DescMode:           1;      /* [30]: 1 - Descriptor Mode */
    UINT32  Enable:             1;      /* [31]: 1 - DMA Channel Enable */
} AMBA_DMA_CTRL_REG_s;

/*
 * DMA: Status Register
 */
typedef struct {
    UINT32  Count:              22;     /* [21:0]: Transfer byte count */
    UINT32  DmaDone:            1;      /* [22]: Operation done */
    UINT32  Reserved:           1;      /* [23]: Reserved */
    UINT32  BusReadWriteError:  1;      /* [24]: Bus read/write error */
    UINT32  BusError:           1;      /* [25]: Bus error */
    UINT32  MemError:           1;      /* [26]: Memory error */
    UINT32  DescDmaDone:        1;      /* [27]: Descriptor DMA operation done */
    UINT32  DescChainDone:      1;      /* [28]: Descriptor chain done */
    UINT32  Reserved1:          1;      /* [29]: Reserved */
    UINT32  DescDmaError:       1;      /* [30]: Descriptor DMA operation error */
    UINT32  DescMemError:       1;      /* [31]: Descriptor memory error */
} AMBA_DMA_STATUS_REG_s;

/*
 * DMA: Channel Mask Register
 */
typedef struct {
    UINT32  Channel0:           1;      /* [0]: DMA Channel 0 Interrupt */
    UINT32  Channel1:           1;      /* [1]: DMA Channel 1 Interrupt */
    UINT32  Channel2:           1;      /* [2]: DMA Channel 2 Interrupt */
    UINT32  Channel3:           1;      /* [3]: DMA Channel 3 Interrupt */
    UINT32  Channel4:           1;      /* [4]: DMA Channel 4 Interrupt */
    UINT32  Channel5:           1;      /* [5]: DMA Channel 5 Interrupt */
    UINT32  Channel6:           1;      /* [6]: DMA Channel 6 Interrupt */
    UINT32  Channel7:           1;      /* [7]: DMA Channel 7 Interrupt */
    UINT32  Reserved:           24;     /* [31:8]: Reserved */
} AMBA_DMA_CHAN_MASK_REG_s;

/*
 * DMA: Channel Registers
 */
typedef struct {
    volatile UINT32                  Ctrl;       /* control */
    volatile UINT32                  SrcAddr;    /* source address */
    volatile UINT32                  DstAddr;    /* destination address */
    volatile UINT32                  Status;     /* status */
} AMBA_DMA_CHAN_REG_s;

#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
/*
 * DMA: Fault Inject Registers
 */
typedef struct {
    UINT32  FaultInject:        1;      /* [0]: Assert dma_sm_error to ECRU */
    UINT32  Reserved:           31;     /* [31:1]: Reserved */
} AMBA_DMA_FAULT_INJECT_REG_s;
#endif

/*
 * DMA: All Registers
 */
typedef struct {
    UINT32                          Reserved0[192];     /* 0x000-0x2FC: Reserved */
    AMBA_DMA_CHAN_REG_s             DmaChanReg[8];      /* 0x300-0x37C(RW): Channel Control Registers */
    volatile UINT32                 DmaChanDescAddr[8]; /* 0x380-0x39C(RW): Channel Descriptor Address */
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
    UINT32                          Reserved1[19];      /* 0x3A0-0x3E8: Reserved */
    AMBA_DMA_FAULT_INJECT_REG_s     FaultInject;        /* 0x3EC(RW): Inject fault for test */
#else
    UINT32                          Reserved1[20];      /* 0x3A0-0x3EC: Reserved */
#endif
    volatile UINT32                 IrqStatus;          /* 0x3F0(RO): Interrupt Status */
    volatile UINT32                 PauseSet;           /* 0x3F4(RW): Set pause bit of DMA channel */
    volatile UINT32                 PauseClear;         /* 0x3F8(WO): Clear pause bit of DMA channel */
    volatile UINT32                 EarlyEndReq;        /* 0x3FC(RW): Send early end request to DMA channel */
} AMBA_DMA_REG_s;

/*
 * Defined in AmbaMmioBase.asm
 */
extern AMBA_DMA_REG_s *pAmbaDMA_Reg[2];
#endif /* AMBA_REG_DMA_H */
