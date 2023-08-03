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

#ifndef AMBA_REG_ADC_H
#define AMBA_REG_ADC_H

#ifndef AMBA_TYPES_H
#include "AmbaTypes.h"
#endif

/*
 * Definitions & Constants for ADC Status Register
 */
/*
 * ADC: Status Register: Status[0]
 */
#define ADC_STATUS_GOING                0U  /* Value goes to 0 after the 'start' command and before the first conversion is complete */
#define ADC_STATUS_COMPLETE             1U  /* At least one conversion since the last 'start' command is complete */

typedef struct {
    UINT32  Status:             1;      /* [0] 1 = At least one conversion since the last 'start' command is completed */
    UINT32  SamplingDone:       1;      /* [1] 1 = ADC just finishes all sampling */
    UINT32  Reserved:           30;     /* [31:2] Reserved */
} AMBA_ADC_STATUS_REG_s;

/*
 * Definitions & Constants for ADC Control Register
 */
/*
 * ADC: Control Register: Mode[1]
 */
#define ADC_CTRL_SINGLE_SAMPLING        0U  /* Single sampling */
#define ADC_CTRL_CONTINUOUS_SAMPLING    1U  /* Continuous sampling */

typedef struct {
    UINT32  Clear:              1;      /* [0] 1 = Software reset */
    UINT32  Mode:               1;      /* [1] 0 = Single sampling, 1 = Continuous sampling */
    UINT32  Enable:             1;      /* [2] 1 = Enable the ADC */
    UINT32  Start:              1;      /* [3] 1 = Start ADC conversion */
    UINT32  Reserved:           28;     /* [31:4] Reserved */
} AMBA_ADC_CTRL_REG_s;

/*
 * ADC: Control Interrupt Status Register
 */
typedef struct {
    UINT32  ErrorEventInt:      1;      /* [0] 1 = Invalid slot period setting or FIFO underflow occurs */
    UINT32  VcmThInt:           1;      /* [1] 1 = Event counter interrupt is asserted */
    UINT32  Reserved:           30;     /* [31:2] Reserved */
} AMBA_ADC_CTRL_INT_STATUS_REG_s;

/*
 * ADC: Channel Interrupt Status Register
 */
typedef struct {
    UINT32  Ch0ThreholdInt:     1;      /* [0] 1 = ADC channel 0's interrupt */
    UINT32  Ch1ThreholdInt:     1;      /* [1] 1 = ADC channel 1's interrupt */
    UINT32  Ch2ThreholdInt:     1;      /* [2] 1 = ADC channel 2's interrupt */
    UINT32  Reserved:           29;     /* [31:3] Reserved */
} AMBA_ADC_DATA_INT_STATUS_REG_s;

/*
 * ADC: FIFO Status Interrupt Status Register
 */
typedef struct {
    UINT32  Fifo0ThresholdInt:  1;      /* [0] 1 = FIFO 0's threshold interrupt */
    UINT32  Fifo1ThresholdInt:  1;      /* [1] 1 = FIFO 1's threshold interrupt */
    UINT32  Fifo2ThresholdInt:  1;      /* [2] 1 = FIFO 2's threshold interrupt */
    UINT32  Fifo3ThresholdInt:  1;      /* [3] 1 = FIFO 3's threshold interrupt */
    UINT32  Fifo0UnderflowInt:  1;      /* [4] 1 = FIFO 0's underflow interrupt */
    UINT32  Fifo1UnderflowInt:  1;      /* [5] 1 = FIFO 1's underflow interrupt */
    UINT32  Fifo2UnderflowInt:  1;      /* [6] 1 = FIFO 2's underflow interrupt */
    UINT32  Fifo3UnderflowInt:  1;      /* [7] 1 = FIFO 3's underflow interrupt */
    UINT32  Fifo0OverflowInt:   1;      /* [8] 1 = FIFO 0's overflow interrupt */
    UINT32  Fifo1OverflowInt:   1;      /* [9] 1 = FIFO 1's overflow interrupt */
    UINT32  Fifo2OverflowInt:   1;      /* [10] 1 = FIFO 2's overflow interrupt */
    UINT32  Fifo3OverflowInt:   1;      /* [11] 1 = FIFO 3's overflow interrupt */
    UINT32  Reserved:           20;     /* [31:12] Reserved */
} AMBA_ADC_FIFO_INT_STATUS_REG_s;

/*
 * ADC: Error Status Register
 */
typedef struct {
    UINT32  OverPeriod:         1;      /* [0] 1 = Sampling period over its limit */
    UINT32  ErrFifo:            1;      /* [1] 1 = Invalid FIFO parameters or FIFO goes underflow */
    UINT32  Reserved:           30;     /* [31:2] Reserved */
} AMBA_ADC_ERROR_STATUS_REG_s;

/*
 * ADC: Slot Control Register
 */
typedef struct {
    UINT32  SlotChannel0:       1;      /* [0] 1 = Sampling channel 0 */
    UINT32  SlotChannel1:       1;      /* [1] 1 = Sampling channel 1 */
    UINT32  SlotChannel2:       1;      /* [2] 1 = Sampling channel 2 */
    UINT32  Reserved:           29;     /* [31:3] Reserved */
} AMBA_ADC_SLOT_CTRL_REG_s;

/*
 * ADC: Channel Interrupt Control Register
 */
typedef struct {
    UINT32  LowerBound:         12;     /* [11:0] 1 = Threshold value as lower bound of ADC data */
    UINT32  Reserved0:          4;      /* [15:12] */
    UINT32  UpperBound:         12;     /* [27:16] 1 = Threshold value as upper bound of ADC data */
    UINT32  Reserved1:          3;      /* [30:28] */
    UINT32  DataOutOfRangeInt:  1;      /* [31] 1 = Enable interrupt trigger on data level out-of-range */
} AMBA_ADC_DATA_INT_CTRL_REG_s;

/*
 * ADC: FIFO Control Register
 */
typedef struct {
    UINT32  FifoDepth:          11;     /* [10:0] FIFO depth for 12-bit ADC samples */
    UINT32  Reserved0:          1;      /* [11] */
    UINT32  ChannelId:          3;      /* [14:12] Channel number associated with this FIFO */
    UINT32  Reserved1:          1;      /* [15] */
    UINT32  FifoTheshold:       11;     /* [26:16] Trigger interrupt when FIFO depth is over the threshold */
    UINT32  Reserved2:          3;      /* [29:27] */
    UINT32  FifoUnderflowInt:   1;      /* [30] 1 = Enable FIFO underflow interrupt */
    UINT32  FifoOverflowInt:    1;      /* [31] 1 = Enable FIFO overflow interrupt */
} AMBA_ADC_FIFO_CTRL_REG_s;

/*
 * ADC: FIFO Status Register
 */
typedef struct {
    UINT32  FifoCount:          11;     /* [10:0] FIFO data counter */
    UINT32  Reserved0:          5;      /* [15:11] */
    UINT32  FifoActive:         1;      /* [16] 1 = FIFO is activated */
    UINT32  Reserved1:          15;     /* [31:17] */
} AMBA_ADC_FIFO_STATUS_REG_s;

/*
 * ADC: OIS PWM Event Counter Control Register
 */
typedef struct {
    UINT32  VcmEnable:          1;      /* [0] Enable VCM event counting */
    UINT32  VcmOption:          1;      /* [1] 0 = Counting rule uses option-1; 1 = Counting rule uses option-2 */
    UINT32  Reserved:           30;     /* [31:2] */
} AMBA_ADC_EC_CTRL_REG_s;

/*
 * ADC: OIS PWM Event Counter Register
 */
typedef struct {
    UINT32  VcmEcCounter:       16;     /* [15:0] Event counter value */
    UINT32  VcmEcSet:           1;      /* [16] 1 = Set initial value to event counter */
    UINT32  Reserved:           15;     /* [31:17] */
} AMBA_ADC_EC_CNT_REG_s;

/*
 * ADC: OIS PWM Event Counter Interrupt Threshold
 */
typedef struct {
    UINT32  VcmIntThreshold:    16;     /* [15:0] Interrupt threshold of event counter */
    UINT32  VcmIntEnable:       1;      /* [16] 1 = Interrupt enable control */
    UINT32  Reserved:           15;     /* [31:17] */
} AMBA_ADC_EC_CNT_TH_REG_s;

/*
 * Definitions & Constants for ADC OIS PWM Event Counter ADC Channel ID Register
 */
/*
 * ADC: OIS PWM Event Counter ADC Channel ID Register: VcmrefCidA[11:8] and VcmrefCidB[15:12]
 */
#define ADC_EC_REF_INT_REG              0xfU /* ADC channel of signal A/B reference to internal register */

typedef struct {
    UINT32  VcmAdcCidA:         3;      /* [2:0] ADC channel select value of signal A */
    UINT32  Reserved0:          1;      /* [3] */
    UINT32  VcmAdcCidB:         3;      /* [6:4] ADC channel select value of signal B */
    UINT32  Reserved1:          1;      /* [7] */
    UINT32  VcmRefCidA:         3;      /* [10:8] Reference ADC channel of signal A. 0x7 = Reference to internal register */
    UINT32  Reserved2:          1;      /* [11] */
    UINT32  VcmRefCidB:         3;      /* [14:12] Reference ADC channel of signal B. 0x7 = Reference to internal register */
    UINT32  Reserved3:          17;     /* [31:15] */
} AMBA_ADC_EC_ADC_REG_s;

/*
 * ADC: OIS PWM Event Counter Internal Reference Value Register
 */
typedef struct {
    UINT32  VcmRefA:            12;     /* [11:0] Internal ADC reference value of signal A in VCM */
    UINT32  Reserved0:          4;      /* [15:12] */
    UINT32  VcmRefB:            12;     /* [27:16] Internal ADC reference value of signal B in VCM */
    UINT32  Reserved1:          4;      /* [31:28] */
} AMBA_ADC_EC_REFVAL_REG_s;

/*
 * ADC: OIS PWM Event Counter ADC Reshaping Register
 */
typedef struct {
    UINT32  VcmPrimeLowTh:      13;     /* [12:0] Low threshold for waveform reshaper (Schmitt trigger) */
    UINT32  Reserved0:          3;      /* [15:13] */
    UINT32  VcmPrimeHighTh:     13;     /* [28:16] High threshold for waveform reshaper (Schmitt trigger) */
    UINT32  Reserved1:          3;      /* [31:29] */
} AMBA_ADC_EC_RESHAPE_REG_s;

/*
 * ADC: All Registers
 */
typedef struct {
    volatile AMBA_ADC_STATUS_REG_s          Status;             /* 0x000(RO): ADC Enable Register */
    volatile AMBA_ADC_CTRL_REG_s            Ctrl;               /* 0x004(RW): ADC Control Register */
    volatile UINT32                         Reserved0;          /* 0x008: Reserved*/
    volatile UINT32                         TimeSlotNum;        /* 0x00C(RW): Number of Valid ADC Time Slots Register (LSB 3-bit) */
    volatile UINT32                         TimeSlotPeriod;     /* 0x010(RW): Period of one ADC Time Slot Register (LSB 16-bit) */
    volatile UINT32                         Reserved1[12];      /* Reserved */
    volatile UINT32                         CtrlIntStatus;      /* 0x044(RW): ADC Control Interrupt Status Register */
    volatile UINT32                         DataIntStatus;      /* 0x048(RW): ADC Channel Interrupt Status Register */
    volatile UINT32                         FifoIntStatus;      /* 0x04C(RW): ADC FIFO Interrupt Status Register */
    volatile AMBA_ADC_ERROR_STATUS_REG_s    ErrorStatus;        /* 0x050(RO): ADC Error Status Register */
    volatile UINT32                         Reserved2[43];      /* Reserved */
    volatile UINT32                         TimeSlotCtrl[8];    /* 0x100-0x11C: ADC Sampling Control Registers of each Time Slot */
    volatile AMBA_ADC_DATA_INT_CTRL_REG_s   DataIntCtrl[3];     /* 0x120-0x128: ADC Channel 0~2 Interrupt Control Registers */
    volatile UINT32                         Reserved3[9];       /* 0x12c-0x14c: Reserved */
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    volatile UINT32                         Data[5];            /* 0x150-0x160: ADC Data Registers for Channel 0~2 */
    volatile UINT32                         Reserved4[7];       /* 0x164-0x17c: Reserved */
#else
    volatile UINT32                         Data[3];            /* 0x150-0x158: ADC Data Registers for Channel 0~2 */
    volatile UINT32                         Reserved4[9];       /* 0x15c-0x17c: Reserved */
#endif
    volatile AMBA_ADC_FIFO_CTRL_REG_s       FifoCtrl[4];        /* 0x180-0x18C: ADC FIFO 0~3 Control Registers */
    volatile UINT32                         FifoReset;          /* 0x190(RW): ADC FIFO Clear Register (LSB 1-bit): 1 = Clear and set FIFOs */
    volatile UINT32                         Reserved5[3];       /* Reserved */
    volatile AMBA_ADC_FIFO_STATUS_REG_s     FifoStatus[4];      /* 0x1A0-0x1AC: ADC FIFO 0~3 Status Register */
    volatile AMBA_ADC_EC_CTRL_REG_s         EventCtrl;          /* 0x1B0(RW): ADC OIS PWM Event Counter Control Register */
    volatile AMBA_ADC_EC_CNT_REG_s          EventCounter;       /* 0x1B4(RW): ADC OIS PWM Event Counter Register */
    volatile AMBA_ADC_EC_CNT_TH_REG_s       EventCounterTh;     /* 0x1B8(RW): ADC OIS PWM Event Counter Interrupt Threshold */
    volatile UINT32                         EventCounterParam;  /* 0x1BC: Event counter parameter register (Reserved) */
    volatile AMBA_ADC_EC_ADC_REG_s          EventADC;           /* 0x1C0(RW): ADC OIS PWM Event Counter ADC Channel ID Register */
    volatile AMBA_ADC_EC_REFVAL_REG_s       EventRefVal;        /* 0x1C4(RW): ADC OIS PWM Event Counter Internal Reference Value Register */
    volatile AMBA_ADC_EC_RESHAPE_REG_s      EventReshape;       /* 0x1C8(RW): ADC OIS PWM Event Counter ADC Reshaping Register */
    volatile UINT32                         Reserved6[13];      /* Reserved */
    volatile UINT32                         FifoData[4][32];    /* 0x200-0x3FF: ADC FIFO 0~3 Read Offset */
} AMBA_ADC_REG_s;

/*
 * Defined in AmbaMmioBase.asm
 */
extern AMBA_ADC_REG_s * pAmbaADC_Reg;

#endif /* AMBA_REG_ADC_H */
