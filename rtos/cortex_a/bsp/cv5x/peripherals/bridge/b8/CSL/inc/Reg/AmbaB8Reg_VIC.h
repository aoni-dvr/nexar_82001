/**
 *  @file AmbaReg_VIC.h
 *
 *  @copyright Copyright (c) 2015 Ambarella, Inc.
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
 *  @details Definitions & Constants for B6 Vector Interrupt Controller(VIC) Registers
 *
 */

#ifndef AMBA_B8_REG_VIC_H
#define AMBA_B8_REG_VIC_H

/*-----------------------------------------------------------------------------------------------*\
 * B6 VIC Interrupt Source Mask
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    UINT32  SpiSlave:           1;  /* [0] SPI slave to AHB master wrapper */
    UINT32  Gpio:               1;  /* [1] GPIO */
    UINT32  I2c0:               1;  /* [2] I2C Master 0 */
    UINT32  I2c1:               1;  /* [3] I2C Master 1 */
    UINT32  PwmEnc:             1;  /* [4] PWM Encoder */
    UINT32  Spi:                1;  /* [5] SPI Master */
    UINT32  Ch0DepackerOrVin:   1;  /* [6] Channel 0 Depacker / VIN0 Last Pixel */
    UINT32  Ch1DepackerOrVin:   1;  /* [7] Channel 1 Depacker / VIN1 Last Pixel */
    UINT32  Ch2DepackerOrVin:   1;  /* [8] Channel 2 Depacker / VIN2 Last Pixel */
    UINT32  Ch3DepackerOrVin:   1;  /* [9] Channel 3 Depacker / VIN3 Last Pixel */
    UINT32  RCT:                1;  /* [10] RCT */
    UINT32  VinVsync0:          1;  /* [11] VIN0 VSYNC (RX) */
    UINT32  VinVsync1:          1;  /* [12] VIN1 VSYNC (RX) */
    UINT32  VinVsync2:          1;  /* [13] VIN2 VSYNC (RX) */
    UINT32  VinVsync3:          1;  /* [14] VIN3 VSYNC (RX) */
    UINT32  VinSof0:            1;  /* [15] VIN0 Start-Of-Frame */
    UINT32  VinSof1:            1;  /* [16] VIN1 Start-Of-Frame */
    UINT32  VinSof2:            1;  /* [17] VIN2 Start-Of-Frame */
    UINT32  VinSof3:            1;  /* [18] VIN3 Start-Of-Frame */
    UINT32  VinMasterVsync0:    1;  /* [19] VIN0 VSYNC (TX) */
    UINT32  VinMasterVsync1:    1;  /* [20] VIN1 VSYNC (TX) */
    UINT32  VinMasterVsync2:    1;  /* [21] VIN2 VSYNC (TX) */
    UINT32  VinMasterVsync3:    1;  /* [22] VIN3 VSYNC (TX) */
    UINT32  Reserved:           4;  /* [26:23] */
    UINT32  Voutf:              1;  /* [27] VOUT Formatter */
    UINT32  VinDelayedVsync0:   1;  /* [28] VIN0 Delayed VSYNC (RX) */
    UINT32  VinDelayedVsync1:   1;  /* [29] VIN1 Delayed VSYNC (RX) */
    UINT32  VinDelayedVsync2:   1;  /* [30] VIN2 Delayed VSYNC (RX) */
    UINT32  VinDelayedVsync3:   1;  /* [31] VIN3 Delayed VSYNC (RX) */
} B8_VIC_SOURCE_MASK_REG_s;

/*-----------------------------------------------------------------------------------------------*\
 * B6 VIC Interrupt Source ID
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    UINT32  IntID:              5;  /* [4:0] Interrupt ID */
    UINT32  Reserved:           27; /* [31:5] */
} B8_VIC_SOURCE_ID_REG_s;

/*-----------------------------------------------------------------------------------------------*\
 * B6 VIC All Registers
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    volatile B8_VIC_SOURCE_MASK_REG_s     IrqStatus;          /* 0x00(R):  IRQ Status Register */
    volatile B8_VIC_SOURCE_MASK_REG_s     FiqStatus;          /* 0x04(R):  FIQ Status Register */
    volatile B8_VIC_SOURCE_MASK_REG_s     RawIntStatus;       /* 0x08(R):  Raw Interrupt Status Register */
    volatile B8_VIC_SOURCE_MASK_REG_s     MaskIntTypeSelect;  /* 0x0C(RW): Interrupt IRQ(0)/FIQ(1) Selection Register */
    volatile B8_VIC_SOURCE_MASK_REG_s     MaskIntEnable;      /* 0x10(RW): Interrupt Enable Register */
    volatile B8_VIC_SOURCE_MASK_REG_s     MaskIntDisable;     /* 0x14(W):  Interrupt Disable Register */
    volatile B8_VIC_SOURCE_MASK_REG_s     MaskSoftIntEnable;  /* 0x18(RW): Softwware Interrupt Enable Register */
    volatile B8_VIC_SOURCE_MASK_REG_s     MaskSoftIntDisable; /* 0x1C(W):  Softwware Interrupt Disable Register */
    volatile B8_VIC_SOURCE_MASK_REG_s     Protection;         /* 0x20(RW): Regiser Access Protection Control Register */
    volatile B8_VIC_SOURCE_MASK_REG_s     MaskIntSense;       /* 0x24(RW): Edge-Triggered(0)/Level-Sensitive(1) Interrupt Selection Register */
    volatile B8_VIC_SOURCE_MASK_REG_s     MaskIntBothEdge;    /* 0x28(RW): Single(0)/Both(1) Edge-Triggered Interrupt Selection Register */
    volatile B8_VIC_SOURCE_MASK_REG_s     MaskIntEvent;       /* 0x2C(RW): Falling(0)/Rising(1) Edge-Triggered Interrupt, Low(0)/High(1) Level-Sensitive Interrupt Selection Register */
    volatile UINT32                       Reserved[2];        /* 0x30 - 0x34 */
    volatile B8_VIC_SOURCE_MASK_REG_s     MaskIntClear;       /* 0x38(W):  Interrupt Clear Register (for edge-triggered interrupt) */
    volatile B8_VIC_SOURCE_ID_REG_s       IdSetFIQ;           /* 0x3C(W):  Interrupt FIQ Selection Register for Single Line */
    volatile B8_VIC_SOURCE_ID_REG_s       IdSetIRQ;           /* 0x40(W):  Interrupt IRQ Selection Register for Single Line */
    volatile B8_VIC_SOURCE_ID_REG_s       IdIntEnable;        /* 0x44(W):  Interrupt Enable Register for Single Line */
    volatile B8_VIC_SOURCE_ID_REG_s       IdIntDisable;       /* 0x48(W):  Interrupt Disable Register for Single Line */
    volatile B8_VIC_SOURCE_ID_REG_s       IdSoftIntEnable;    /* 0x4C(W):  Software Interrupt Enable Register for Single Line */
    volatile B8_VIC_SOURCE_ID_REG_s       IdSoftIntDisable;   /* 0x50(W):  Software Interrupt Disable Register for Single Line */
    volatile B8_VIC_SOURCE_ID_REG_s       IdSetLevelSense;    /* 0x54(W):  Level-Sensitive Interrupt Selection Register for Single Line */
    volatile B8_VIC_SOURCE_ID_REG_s       IdSetEdgeTriggered; /* 0x58(W):  Edge-Triggered Interrupt Selection Register for Single Line */
    volatile B8_VIC_SOURCE_ID_REG_s       IdSetBothEdgeInt;   /* 0x5C(W):  Both Edge Triggered Interrupt Register for Single Line */
    volatile B8_VIC_SOURCE_ID_REG_s       IdSetSingleEdgeInt; /* 0x60(W):  Single Edge Triggered Interrupt Register for Single Line */
    volatile B8_VIC_SOURCE_ID_REG_s       IdSetPosEdgeHighLv; /* 0x64(W):  Rising-Edge Triggered/High-Level Sensitive Interrupt Register for Single Line */
    volatile B8_VIC_SOURCE_ID_REG_s       IdSetNegEdgeLowLv;  /* 0x68(W):  Falling-Edge Triggered/Low-Level Sensitive Interrupt Register for Single Line */
} B8_VIC_REG_s;

#endif /* AMBA_B8_REG_VIC_H */
