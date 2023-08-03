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

#ifndef AMBA_REG_GDMA_H
#define AMBA_REG_GDMA_H

#ifndef AMBA_TYPES_H
#include "AmbaTypes.h"
#endif

/*
 * GDMA: Pitch Register
 */
typedef struct {
    UINT32  RowPitch:       16;     /* [15:0] Row Pitch of Source Area */
    UINT32  Reserved:       16;     /* [31:16] */
} AMBA_GDMA_PITCH_REG_s;

/*
 * GDMA: Width Register
 */
typedef struct {
    UINT32  Width:          13;     /* [12:0] Number of Pixels Minus 1 per Row */
    UINT32  Reserved0:      2;      /* [14:13] */
    UINT32  CompleteInt:    1;      /* [15] Interrupt on 0 = each, 1 = all operation completion */
    UINT32  Reserved:       16;     /* [31:16] */
} AMBA_GDMA_WIDTH_REG_s;

/*
 * GDMA: Height Register
 */
typedef struct {
    UINT32  Height:         12;     /* [11:0] Number of Rows Minus 1 in Area */
    UINT32  Reserved:       20;     /* [31:12] */
} AMBA_GDMA_HEIGHT_REG_s;

/*
 * GDMA: Number of Pending Operations Register
 */
typedef struct {
    UINT32  QueueDepth:     4;      /* [3:0] 0 = Number of pending requests */
    UINT32  Reserved:       28;     /* [31:4] */
} AMBA_GDMA_QUEUE_STATUS_REG_s;

/*
 * GDMA: Pixel Format Register
 */
typedef struct {
    UINT32  DestFormat:     3;      /* [2:0] Destination Pixel Format */
    UINT32  Reserved0:      1;      /* [3] */
    UINT32  Src2Format:     3;      /* [6:4] Source 2 Pixel Format */
    UINT32  Reserved1:      1;      /* [7] */
    UINT32  Src1Format:     3;      /* [10:8] Source 1 Pixel Format */
    UINT32  DramThrottle:   1;      /* [11] */
    UINT32  Reserved2:      20;     /* [31:12] */
} AMBA_GDMA_PIXEL_FORMAT_REG_s;

/*
 * GDMA: All Registers
 */
typedef struct {
    volatile UINT32     Src1Base;       /* 0x000: Base Address of Source 1 */
    volatile UINT32     Src1Pitch;      /* 0x004: Row Pitch of Source 1 (LSB 16-bit) */
    volatile UINT32     Src2Base;       /* 0x008: Base Address of Source 2 */
    volatile UINT32     Src2Pitch;      /* 0x00C: Row Pitch of Source 2 (LSB 16-bit) */
    volatile UINT32     DestBase;       /* 0x010: Base Address of Destination */
    volatile UINT32     DestPitch;      /* 0x014: Row Pitch of Destination (LSB 16-bit) */
    volatile UINT32     Width;          /* 0x018: Source/Destination Width (in pixels) */
    volatile UINT32     Height;         /* 0x01C: Source/Destination Height (in rows) */
    volatile UINT32     Transparent;    /* 0x020: Transparent Value for Composite operation */
    volatile UINT32     OpCode;         /* 0x024: Operation Code (LSB 3-bit) */
    volatile UINT32     QueueDepth;     /* 0x028: Number of Pending Requests */
    volatile UINT32     PixelFormat;    /* 0x02C: Pixel Format */
    volatile UINT32     Alpha;          /* 0x030: Alpha Value for Alpha Blend operation (LSB 8-bit) */
    volatile UINT32     Reserved[243];  /* 0x034-0x3FC */
    volatile UINT32     Clut[256];      /* 0x400: 8-bit/4-bit Color Lookup Table */
} AMBA_GDMA_REG_s;

/*
 * Defined in AmbaMmioBase.asm
 */
extern AMBA_GDMA_REG_s *pAmbaGDMA_Reg;

#endif /* AMBA_REG_GDMA_H */
