/**
 *  @file AmbaReg_I2C.h
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
 *  @details Definitions & Constants for I2C Control Registers
 *
 */

#ifndef AMBA_REG_I2C_H
#define AMBA_REG_I2C_H

#ifndef AMBA_TYPES_H
#include "AmbaTypes.h"
#endif

/*
 * I2C Master: Enable Register
 */
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS) || defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV28)
typedef struct {
    UINT32  Enable:         1;      /* [0]: 1 - I2C Enabled; 0 - Disable */
    UINT32  ScdcEnable:     1;      /* [1]: 1 - Scdc Enabled; 0 - Disable */
    UINT32  SuperMode:      1;      /* [2]: 1 - turbo write super mode enabled; 0 - Disable */
    UINT32  SuperModeMask:  1;      /* [3]: 1 - Super mode interrupt mask enabled; 0 - Disable */
    UINT32  Reserved:       28;     /* [31:4]: Reserved */
} AMBA_I2C_ENABLE_REG_s;
#else
typedef struct {
    UINT32  Enable:         1;      /* [0]: 1 - I2C Enabled; 0 - Disable */
    UINT32  ScdcEnable:     1;      /* [1]: 1 - Scdc Enabled; 0 - Disable */
    UINT32  Reserved:       30;     /* [31:2]: Reserved */
} AMBA_I2C_ENABLE_REG_s;
#endif

/*
 * I2C Master: Control Register
 */
typedef struct {
    UINT32  Acknowledge:    1;      /* [0]: Write: 0 - ACK, 1 - NACK */
    UINT32  IrqFlag:        1;      /* [1]: 0 = No interrupt, 1= Interrupt pending */
    UINT32  Start:          1;      /* [2]: 1 = The next process generates a START condition */
    UINT32  Stop:           1;      /* [3]: 1 = The next process generates a STOP condition */
    UINT32  HsMode:         1;      /* [4]: 1 = Enables High-Speed Mode */
    UINT32  ClearFIFO:      1;      /* [5]: 1 = Clear 63-entry FIFO used during turbo mode*/
    UINT32  IrqScdc:        1;      /* [6]: 1 = Clear */
    UINT32  Reserved:       25;     /* [31:7]: Reserved */
} AMBA_I2C_CTRL_REG_s;

/*
 * I2C Master: Data Register
 */
typedef struct {
    UINT32  Value:          8;      /* [7:0]: Data Value */
    UINT32  Reserved:       24;     /* [31:8]: Reserved */
} AMBA_I2C_DATA_REG_s;

/*
 * I2C Master: status Register
 */
typedef struct {
    UINT32  OpMode:         1;      /* [0]: 0 - Master Tx; 1 - Master Rx */
    UINT32  FifoFull:       1;      /* [1]: 1 = Fifo Full */
    UINT32  FifoEmpty:      1;      /* [2]: 1 = Fifo empty */
    UINT32  Reserved0:      1;      /* [3]: Reserved */
    UINT32  Status:         4;      /* [7:4]: IDC Finite State Machine (FSM) status */
    UINT32  FifoCount:      6;      /* [13:8]: FIFO count of empty entries */
    UINT32  Reserved1:      18;     /* [31:14]: Reserved */
} AMBA_I2C_STATUS_REG_s;

/*
 * I2C Master: Prescale Register
 */
typedef struct {
    UINT32  Value:          8;      /* [7:0]: Scaler Value */
    UINT32  Reserved:       24;     /* [31:8]: Reserved */
} AMBA_I2C_PRESCALE_REG_s;

/*
 * I2C Master: FIFO-mode Control Register
 */
typedef struct {
    UINT32  Reserved0:      1;      /* [0]: Reserved */
    UINT32  CompleteIrq:    1;      /* [1]: 1 - Assert interrupt at transmission completed */
    UINT32  Start:          1;      /* [2]: 1 - The next process generates a START condition */
    UINT32  Stop:           1;      /* [3]: 1 - The next process generates a STOP condition */
    UINT32  HsMode:         1;      /* [4]: 1 - Enables High-Speed Mode */
    UINT32  Reserved1:      27;     /* [31:5]: Reserved */
} AMBA_I2C_FIFO_MODE_CTRL_REG_s;

/*
 * I2C Master: Standard mode (non-turbo) Duty cycle Register
 */
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS) || defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV28)
typedef struct {
    UINT32  DutyCycle:          2;      /* [1:0]: Duty Cycle setting for standard mode 0-1:1, 1-2:3, 2-1:2 */
    UINT32  SclCurrSrcEnable:   1;      /* [2]: 1 - SCL (clock line) pin current source enable */
    UINT32  SdaCurrSrcEnable:   1;      /* [3]: 1 - SDA (Data line) pin current source enable */
    UINT32  DefinedMode:        1;      /* [4]: 1 - Enable defined SCL mode */
    UINT32  Reserved:           27;     /* [31:5]: Reserved */
} AMBA_I2C_DUTY_CYCLE_REG_s;
#else
typedef struct {
    UINT32  DutyCycle:        2;      /* [1:0]: Duty Cycle setting for standard mode 0-1:1, 1-2:3, 2-1:2 */
    UINT32  SclCurrSrcEnable: 1;      /* [2]: 1 - SCL (clock line) pin current source enable */
    UINT32  SdaCurrSrcEnable: 1;      /* [3]: 1 - SDA (Data line) pin current source enable */
    UINT32  Reserved:         28;     /* [31:4]: Reserved */
} AMBA_I2C_DUTY_CYCLE_REG_s;
#endif

/*
 * I2C Master: Delay Register
 */
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS) || defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV28)
typedef struct {
    UINT32  Delay:              22;     /* [21:0]: Delay conunter. Controller will wait Delay * apb_clock_period to send out the command */
    UINT32  Reserved:           10;     /* [31:22]: Reserved */
} AMBA_I2C_DELAY_REG_s;
#else
typedef struct {
    UINT32  Delay:            8;      /* [7:0]: Delay cycles from START condition to the first data transfer and from the last acknowledge to STOP condition */
    UINT32  Reserved:         24;     /* [31:8]: Reserved */
} AMBA_I2C_DELAY_REG_s;
#endif

#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS) || defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV28)
/*
 * I2C Master: Stretch Bypass Region Register
 */
typedef struct {
    UINT32  BypassRegion:       8;      /* [7:0]: High/Low byte for bypass region */
    UINT32  Reserved:           24;     /* [31:8]: Reserved */
} AMBA_I2C_STRETCH_BYPASS_REGION_REG_s;

/*
 * I2C Master: Phase Number Register
 */
typedef struct {
    UINT32  PhaseNum:           4;      /* [3:0]: Number = PhaseNum +1. Minimum number should be 3 */
    UINT32  PhaseNumHs:         4;      /* [7:4]: Number = PhaseNumHs +1. Minimum number should be 5, Number should be a multiple of 3  */
    UINT32  Reserved:           24;     /* [31:8]: Reserved */
} AMBA_I2C_PHASE_NUMBER_REG_s;

/*
 * I2C Master: Phase Control Register
 */
typedef struct {
    UINT32  RisingPhase:        4;      /* [3:0]: Rising phase of SCL */
    UINT32  FallingPhase:       4;      /* [7:4]: Falling phase of SCL */
    UINT32  Reserved:           24;     /* [31:8]: Reserved */
} AMBA_I2C_PHASE_CONTROL_REG_s;
#else
/*
 * I2C Master: Throughput Refinement Register
 */
typedef struct {
    UINT32  Cycle:            8;      /* [7:0]:  Number of compensate clock cycles to make throughput match to the clock rate */
    UINT32  Reserved:         24;     /* [31:8]: Reserved */
} AMBA_I2C_THROUGHPUT_REFINE_REG_s;
#endif

#if defined(CONFIG_SOC_CV5)
/*
 * I2C Master: Glitch Filter Control Register
 */
typedef struct {
    UINT32  GFEnable:           1;      /* [0](RW): 1 = Enable glitch filter, 0 = Disable glitch filter */
    UINT32  Reserved:           3;      /* [3:1]: Reserved */
    UINT32  GFCounter:          5;      /* [8:4](RW): Set clock cycle as glitch filter based on APB clock, Counter = GFCounter + 1 */
    UINT32  Reserved1:          7;      /* [15:9]: Reserved */
    UINT32  GFCounterHs:        5;      /* [20:16](RW): Set clock cycle as glitch gilter under I2C high speed mode based on apb clock, Counter = GFCounterHs + 1 */
    UINT32  Reserved2:          11;     /* [31:21]: Reserved */
} AMBA_I2C_GLITCH_FILTER_REG_s;
#endif

/*
 * I2C Master: All Registers
 */
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS) || defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV28)
typedef struct {
    volatile UINT32                                 Enable;             /* 0x00(RW): I2C Enable Register (LSB 1-bit): 1 = enabled */
    volatile UINT32                                 Ctrl;               /* 0x04(RW): I2C Control Register */
    volatile AMBA_I2C_DATA_REG_s                    Data;               /* 0x08(RW): I2C Data Register (LSB 8-bit) */
    volatile AMBA_I2C_STATUS_REG_s                  Status;             /* 0x0C(RO): I2C Status Register: 0 = Tx, 1 = Rx */
    volatile AMBA_I2C_PRESCALE_REG_s                PrescalerLowByte;   /* 0x10(RW): I2C Prescaler Register (Low byte) */
    volatile AMBA_I2C_PRESCALE_REG_s                PrescalerHighByte;  /* 0x14(RW): I2C Prescaler Register (High byte) */
    volatile UINT32                                 FifoModeCtrl;       /* 0x18(RW): I2C FIFO Turbo Mode Control Register */
    volatile AMBA_I2C_DATA_REG_s                    FifoModeData;       /* 0x1C(RW): I2C FIFO Turbo Mode Data Register (LSB 8-bit) */
    volatile AMBA_I2C_PRESCALE_REG_s                PrescalerTurboMode; /* 0x20(RW): I2C Prescaler Register for Turbo mode */
    volatile AMBA_I2C_DUTY_CYCLE_REG_s              DutyCycle;          /* 0x24(RW): I2C Standard mode (non-Turbo) Duty cycle Register */
    volatile UINT32                                 StretchCtrl;        /* 0x28(RW): I2C SCL Clock Stretch Register*/
    volatile UINT32                                 RxFifo;             /* 0x2C(RW): Rx FIFO for I2C Turbo Read */
    volatile AMBA_I2C_DELAY_REG_s                   Delay;              /* 0x30(RW): I2C Delay Register */
    volatile AMBA_I2C_STRETCH_BYPASS_REGION_REG_s   StrechBypLow;       /* 0x34(RW): Stretch Bypass Region Low Byte */
    volatile AMBA_I2C_STRETCH_BYPASS_REGION_REG_s   StrechBypHigh;      /* 0x38(RW): Stretch Bypass Region High Byte */
    volatile UINT32                                 FmNack;             /* 0x3C(RW): FIFO Mode Nack Log */
    volatile AMBA_I2C_PHASE_NUMBER_REG_s            DefinedSclNum;      /* 0x40(RW): Phase Number for User Defined Mode */
    volatile AMBA_I2C_PHASE_CONTROL_REG_s           DefinedScl;         /* 0x44(RW): Non High-speed Mode Rising/Falling phase configuration */
    volatile AMBA_I2C_PHASE_CONTROL_REG_s           DefinedSclHs;       /* 0x48(RW): High-speed Mode Rising/Falling phase configuration */
#if defined(CONFIG_SOC_CV5)
    volatile UINT32                                 Reserved[2];        /* 0x4C-0x50: Reserved */
    volatile AMBA_I2C_GLITCH_FILTER_REG_s           GlitchFilter;       /* 0x54(RW): I2C input Glitech filter Register */
#else
    volatile UINT32                                 Reserved0;          /* 0x4C: Reserved */
    volatile UINT32                                 FaultInject;        /* 0x50(RW): Fault Injection Test */
#endif
} AMBA_I2C_REG_s;
#else
typedef struct {
    volatile UINT32                                 Enable;             /* 0x00(RW): I2C Enable Register (LSB 1-bit): 1 = enabled */
    volatile UINT32                                 Ctrl;               /* 0x04(RW): I2C Control Register */
    volatile AMBA_I2C_DATA_REG_s                    Data;               /* 0x08(RW): I2C Data Register (LSB 8-bit) */
    volatile AMBA_I2C_STATUS_REG_s                  Status;             /* 0x0C(RO): I2C Status Register: 0 = Tx, 1 = Rx */
    volatile AMBA_I2C_PRESCALE_REG_s                PrescalerLowByte;   /* 0x10(RW): I2C Prescaler Register (Low byte) */
    volatile AMBA_I2C_PRESCALE_REG_s                PrescalerHighByte;  /* 0x14(RW): I2C Prescaler Register (High byte) */
    volatile UINT32                                 FifoModeCtrl;       /* 0x18(RW): I2C FIFO Turbo Mode Control Register */
    volatile AMBA_I2C_DATA_REG_s                    FifoModeData;       /* 0x1C(RW): I2C FIFO Turbo Mode Data Register (LSB 8-bit) */
    volatile AMBA_I2C_PRESCALE_REG_s                PrescalerTurboMode; /* 0x20(RW): I2C Prescaler Register for Turbo mode */
    volatile AMBA_I2C_DUTY_CYCLE_REG_s              DutyCycle;          /* 0x24(RW): I2C Standard mode (non-Turbo) Duty cycle Register */
    volatile AMBA_I2C_DELAY_REG_s                   Delay;              /* 0x28(RW): I2C Delay Register */
    volatile AMBA_I2C_THROUGHPUT_REFINE_REG_s       Compensate[2];      /* 0x2C-0x30(RW): I2C Throughput Refinement Register */
} AMBA_I2C_REG_s;
#endif

/*
 * I2C Slave: Enable Register
 */
typedef struct {
    UINT32  Enable:             1;      /* [0](RW): 1 = Enabled; 0 = Disable */
    UINT32  Reserved:           31;     /* [31:1]: Reserved */
} AMBA_I2CS_ENABLE_REG_s;

/*
 * I2C Slave: Control Register
 */
typedef struct {
    UINT32  Acknowledge:        1;      /* [0](RW): 0 = ACK, 1 = NACK (received or needs to be transmitted) */
    UINT32  IrqEnable:          1;      /* [1](RW): 1 = Enable interrupt to GIC */
    UINT32  IrqSr:              1;      /* [2](RW): 1 = IRQ asserts (Slave-receiver mode only) when slave detects STOP or Repeated START condition */
    UINT32  IrqFifoThresh:      1;      /* [3](RW): 1 = IRQ asserts when RX fifo count is more than the threshold value or TX fifo count is less than or equal to threshold */
    UINT32  IrqTimeout:         1;      /* [4](RW): 1 = IRQ asserts when clock stretching timeout occurs */
    UINT32  FifoClear:          1;      /* [5](WO): 1 = Clear all FIFO data */
    UINT32  IrqClear:           1;      /* [6](WO): 1 = Clear interrupt */
    UINT32  Reset:              1;      /* [7](WO): 1 = Trigger software reset */
#if defined(CONFIG_SOC_CV5)
    UINT32  TransferDone:       1;      /* [8](WO): 1 = IRQ asserts (Slave-receiver mode only) when transfer complete (fifo empty or recieve ack) */
    UINT32  Reserved:           23;     /* [31:9]: Reserved */
#else
    UINT32  Reserved:           24;     /* [31:8]: Reserved */
#endif
} AMBA_I2CS_CTRL_REG_s;

/*
 * I2C Slave: Data Register
 */
typedef struct {
    UINT32  Value:              8;      /* [7:0](RW): Data Value */
    UINT32  FirstByte:          1;      /* [8](RO): (Slave-receiver mode only) 1 = the data is the first byte after the START or Repeated START */
    UINT32  Reserved:           23;     /* [31:9]: Reserved */
} AMBA_I2CS_DATA_REG_s;

/*
 * I2C Slave: Status Register
 */
typedef struct {
    UINT32  RxTxState:          1;      /* [0](RO): 1 = Slave-receiver mode, 0 = Slave-transmitter mode */
    UINT32  FifoEmpty:          1;      /* [1](RO): 1 = FIFO is empty */
    UINT32  FifoFull:           1;      /* [2](RO): 1 = FIFO is full */
    UINT32  GeneralCall:        1;      /* [3](RO): 1 = the current transfer is a general call address */
    UINT32  SlaveSelect:        1;      /* [4](RO): 1 = IDCS is selected by a master */
    UINT32  FifoOverThresh:     1;      /* [5](RO): (Slave-receiver mode only) 1 = if FIFO data count > idcs_rx_fifo_th */
    UINT32  StartRepeat:        1;      /* [6](RW): 1 = A repeated START condition is received */
    UINT32  Stop:               1;      /* [7](RW): 1 = STOP condition is received. The bit should be cleared by writing 0. */
    UINT32  StretchTimeout:     1;      /* [8](RW): 1 = SCL stretching timeout condition occurs */
    UINT32  Reserved:           23;     /* [31:9]: Reserved */
} AMBA_I2CS_STATUS_REG_s;

/*
 * I2C Slave: Fifo Count Register
 */
typedef struct {
    UINT32  FifoLevel:          7;      /* [6:0](RO): The number of valid entries in the FIFO. */
    UINT32  Reserved:           25;     /* [31:7]: Reserved */
} AMBA_I2CS_FIFO_COUNT_REG_s;

/*
 * I2C Slave: Data Hold Time Register
 */
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS) || defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV28)
typedef struct {
    UINT32  HoldTime:           8;      /* [7:0](RW): Data hold time setup for slave-transmitter. */
    UINT32  HoldTimeHs:         8;      /* [15:8](RW): Data hold time setup for slave-transmitter at high speed mode */
    UINT32  Reserved:           16;     /* [31:16]: Reserved */
} AMBA_I2CS_DATA_HOLD_TIME_REG_s;

#else
typedef struct {
    UINT32  HoldTime:           8;      /* [7:0](RW): Data hold time setup for slave-transmitter. */
    UINT32  Reserved:           24;     /* [31:8]: Reserved */
} AMBA_I2CS_DATA_HOLD_TIME_REG_s;
#endif

/*
 * I2C Slave: Slave Address Register
 */
typedef struct {
    UINT32  SlaveAddr:          7;      /* [6:0](RW): 7-bit slave address */
    UINT32  Reserved:           25;     /* [31:7]: Reserved */
} AMBA_I2CS_SLAVE_ADDR_REG_s;

/*
 * I2C Slave: Error Status Register
 */
typedef struct {
    UINT32  TxFifoEmpty:        1;      /* [0](RW): 1 = FIFO is empty in transmitter mode */
    UINT32  RxFifoFull:         1;      /* [1](RW): 1 = FIFO is full in receiver mode */
    UINT32  FifoInvalid:        1;      /* [2](RW): 1 = The received data in FIFO is not read, but IDCS is switched to transmitter mode. */
    UINT32  IrqNotClear:        1;      /* [3](RW): 1 = The interrupts from STOP or repeated START condition are not cleared. */
    UINT32  Reserved:           28;     /* [31:4]: Reserved */
} AMBA_I2CS_ERROR_STATUS_REG_s;

#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS) || defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV28)
/*
 * I2C Slave: Debug Register
 */
typedef struct {
    UINT32  State:              3;      /* [2:0]: Finite state machine status */
    UINT32  BitCount:           4;      /* [6:3]: Bit counter */
    UINT32  SclHoldLow:         1;      /* [7]: SCL should keep low after transfer is done if error occurs */
    UINT32  TxFifoEmptyErr:     1;      /* [8]: FIFO is empty in transmitter */
    UINT32  RxFifoFullErr:      1;      /* [9]: FIFO is full in receiver mode */
    UINT32  RxFifoBusyErr:      1;      /* [10]: The received data in FIFO is not read, but IDCS is switched to transmitter mode */
    UINT32  IntBusyErr:         1;      /* [11]: The interrupts from stop or repeated start condition are not cleared */
    UINT32  TimeoutNack:        1;      /* [12]: The slave will return NACK when the internal counter reaches idcs_scl_timer */
    UINT32  Reserved:           19;     /* [31:13]: Reserved */
} AMBA_I2CS_DEBUG_REG_s;
#endif

#if defined(CONFIG_SOC_CV5)
/*
 * I2C Slave: Glitch Filter Control Register
 */
typedef struct {
    UINT32  GFEnable:           1;      /* [0](RW): 1 = Enable glitch filter, 0 = Disable glitch filter */
    UINT32  Reserved:           3;      /* [3:1]: Reserved */
    UINT32  GFCounter:          5;      /* [8:4](RW): Set clock cycle as glitch filter based on APB clock, Counter = GFCounter + 1 */
    UINT32  Reserved1:          7;      /* [15:9]: Reserved */
    UINT32  GFCounterHs:        5;      /* [20:16](RW): Set clock cycle as glitch gilter under I2C high speed mode based on apb clock, Counter = GFCounterHs + 1 */
    UINT32  Reserved2:          11;     /* [31:21]: Reserved */
} AMBA_I2CS_GLITCH_FILTER_REG_s;
#endif

/*
 * I2C Slave: All Registers
 */
typedef struct {
    volatile UINT32                             Enable;             /* 0x00(RW): I2CS Enable Register (LSB 1-bit): 1 = enabled */
    volatile UINT32                             Ctrl;               /* 0x04(RW): I2CS Control Register */
    volatile UINT32                             Data;               /* 0x08(RW): I2CS Data Register (LSB 9-bit) */
    volatile AMBA_I2CS_STATUS_REG_s             Status;             /* 0x0C(RO): I2CS Status Register: 0 = Tx, 1 = Rx */
    volatile AMBA_I2CS_FIFO_COUNT_REG_s         FifoCount;          /* 0x10(RW): I2CS Current FIFO Count Register (LSB 7-bit) */
    volatile AMBA_I2CS_FIFO_COUNT_REG_s         RxFifoThreshold;    /* 0x14(RW): I2CS RX FIFO Threshold Register (LSB 7-bit) */
    volatile AMBA_I2CS_FIFO_COUNT_REG_s         TxFifoThreshold;    /* 0x18(RW): I2CS TX FIFO Threshold Register (LSB 7-bit) */
    volatile AMBA_I2CS_DATA_HOLD_TIME_REG_s     DataHoldTime;       /* 0x1C(RW): I2CS Data Hold Time Register (LSB 8-bit) */
    volatile AMBA_I2CS_SLAVE_ADDR_REG_s         SlaveAddr;          /* 0x20(RW): I2CS Slave Address Register */
    volatile UINT32                             SclStretchCtrl;     /* 0x24(RW): I2CS SCL Stretch Control Register */
    volatile AMBA_I2CS_ERROR_STATUS_REG_s       ErrorStatus;        /* 0x28(RW): I2CS Error Status Register */
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS) || defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV28)
    volatile AMBA_I2CS_DEBUG_REG_s              DebugInfo;          /* 0x2C(RO): Debug Information */
    volatile UINT32                             AckControl;         /* 0x30(RW): ACK Control */
    volatile UINT32                             IrqStatus;          /* 0x34(RO): IRQ Status*/
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
    volatile UINT32                             FaultInject;        /* 0x38(RW): Fault Injection to IDCS */
#endif
#if defined(CONFIG_SOC_CV5)
    volatile UINT32                             Reserved;           /* 0x38: Reserved */
    volatile AMBA_I2CS_GLITCH_FILTER_REG_s      GlitchFilter;       /* 0x3C(RW): I2CS Glitech filter Register */
#endif
#endif
} AMBA_I2CS_REG_s;

/*
 * Defined in AmbaMmioBase.asm
 */
#ifdef CONFIG_QNX
extern AMBA_I2C_REG_s * pAmbaI2C_MasterReg;
extern AMBA_I2CS_REG_s * pAmbaI2C_SlaveReg;
#else
extern AMBA_I2C_REG_s * pAmbaI2C_MasterReg[AMBA_NUM_I2C_CHANNEL];
extern AMBA_I2CS_REG_s * pAmbaI2C_SlaveReg;
#endif

#endif /* AMBA_REG_I2C_H */
