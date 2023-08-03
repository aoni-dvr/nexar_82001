/**
 *  @file AmbaReg_I2C.h
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
 *  @details Definitions & Constants for B6 I2C Registers
 *
 */

#ifndef AMBA_B8_REG_I2C_H
#define AMBA_B8_REG_I2C_H

/*-----------------------------------------------------------------------------------------------*\
 * B6 I2C : Enable Register
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    UINT32  Enable:                 1;      /* [0]: 1 - I2C Enabled; 0 - Disable */
    UINT32  Reserved0:              1;      /* [1]: Reserved */
    UINT32  SuperModeEnable:        1;      /* [2]: 1 - Turbo write super mode Enabled; 0 - Disable */
    UINT32  IrqSuperMaskEnable:     1;      /* [3]: 1 - Super mode interrupt mask Enabled; 0 - Disable */
    UINT32  Reserved1:              28;     /* [31:4]: Reserved */
} B8_I2C_ENABLE_REG_s;

/*-----------------------------------------------------------------------------------------------*\
 * B6 I2C : Control Register
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    UINT32  Acknowledge:    1;      /* [0]: Write: 0 - ACK, 1 - NACK */
    UINT32  IrqFlag:        1;      /* [1]: 0 = No interrupt, 1= Interrupt pending */
    UINT32  Start:          1;      /* [2]: 1 = The next process generates a START condition */
    UINT32  Stop:           1;      /* [3]: 1 = The next process generates a STOP condition */
    UINT32  HsMode:         1;      /* [4]: 1 = Enable High-Speed mode */
    UINT32  ClearFIFO:      1;      /* [5]: 1 = Clear the 63-entry FIFO used during Turbo Mode */
    UINT32  IrqScdc:        1;      /* [6]: Write 1 to clear */
    UINT32  Reserved0:      25;     /* [31:7]: */
} B8_I2C_CTRL_REG_s;

/*-----------------------------------------------------------------------------------------------*\
 * B6 I2C : Data Register
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    UINT32  Value:          8;      /* [7:0]: Data Value */
    UINT32  Reserved0:      24;     /* [31:8]: */
} B8_I2C_DATA_REG_s;

/*-----------------------------------------------------------------------------------------------*\
 * B6 I2C : status Register
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    UINT32  OpMode:         1;      /* [0]: 0 - Master Tx; 1 - Master Rx */
    UINT32  FifoFull:       1;      /* [1]: 1 - FIFO full */
    UINT32  FifoEmpty:      1;      /* [2]: 1 - FIFO empty */
    UINT32  Reserved0:      1;      /* [3]: */
    UINT32  State:          4;      /* [7:4]: State */
    UINT32  FifoCount:      8;      /* [15:8]: FIFO count of empty entries */
    UINT32  Reserved1:      16;     /* [31:16]: */
} B8_I2C_STATUS_REG_s;

/*-----------------------------------------------------------------------------------------------*\
 * B6 I2C : Prescale Register
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    UINT32  Value:          8;      /* [7:0]: Scaler Value */
    UINT32  Reserved0:      24;     /* [31:8]: */
} B8_I2C_PRESCALE_REG_s;

/*-----------------------------------------------------------------------------------------------*\
 * B6 I2C : FIFO-mode Control Register
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    UINT32  ReadFifoMode:   1;      /* [0]: */
    UINT32  CompleteIrq:    1;      /* [1]: 1 - Assert interrupt at transmission completed */
    UINT32  Start:          1;      /* [2]: 1 - The next process generates a START condition */
    UINT32  Stop:           1;      /* [3]: 1 - The next process generates a STOP condition */
    UINT32  HighSpeed:      1;      /* [4]: 1 - Enable High Speed mode */
    UINT32  ReadCount:      3;      /* [7:5]: */
    UINT32  Reserved0:      24;     /* [31:8]: */
} B8_I2C_FIFO_MODE_CTRL_REG_s;

/*-----------------------------------------------------------------------------------------------*\
 * B6 I2C : Duty Cycle Control Register
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    UINT32  DutyCycle:              2;  /* [1:0]: Duty cycle setting for Standard(non-High Speed) Mode. 0 - 1:1; 1 - 2:3; 2 - 1:2 */
    UINT32  CurrentSourceForSCL:    1;  /* [2]: 1 - Enable current source for SCL pin */
    UINT32  CurrentSourceForSDA:    1;  /* [3]: 1 - Enable current source for SDA pin */
    UINT32  Reserved0:              28; /* [31:4]: */
} B8_I2C_DUTY_CYCLE_CTRL_REG_s;

/*-----------------------------------------------------------------------------------------------*\
 * B6 I2C : Stretch Control Register
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    UINT32  SclStretch:     4;  /* [3:0]: Extend the SCL clock START/STOP setup/hold timing */
    UINT32  Reserved0:      28; /* [31:4]: */
} B8_I2C_STRETCH_CTRL_REG_s;

/*-----------------------------------------------------------------------------------------------*\
 * B6 I2C : All Registers
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    volatile B8_I2C_ENABLE_REG_s          Enable;             /* 0x00(RW): I2C Enable Register (LSB 1-bit): 1 = enabled */
    volatile B8_I2C_CTRL_REG_s            Ctrl;               /* 0x04(RW): I2C Control Register */
    volatile B8_I2C_DATA_REG_s            Data;               /* 0x08(RW): I2C Data Register (LSB 8-bit) */
    volatile B8_I2C_STATUS_REG_s          Status;             /* 0x0C(RO): I2C Status Register: 0 = Tx, 1 = Rx */
    volatile B8_I2C_PRESCALE_REG_s        PrescalerLowByte;   /* 0x10(RW): I2C Prescaler Register (Low byte) */
    volatile B8_I2C_PRESCALE_REG_s        PrescalerHighByte;  /* 0x14(RW): I2C Prescaler Register (High byte) */
    volatile B8_I2C_FIFO_MODE_CTRL_REG_s  FifoModeCtrl;       /* 0x18(RW): I2C FIFO-mode Control Register */
    volatile B8_I2C_DATA_REG_s            FifoModeWriteData;  /* 0x1C(RW): I2C FIFO-mode Data Register (LSB 8-bit) */
    volatile B8_I2C_PRESCALE_REG_s        PrescalerHighSpeed; /* 0x20(RW): I2C Prescaler Register for High Speed mode */
    volatile B8_I2C_DUTY_CYCLE_CTRL_REG_s DutyCycleCtrl;      /* 0x24(RW): I2C Duty Cycle Control Register */
    volatile B8_I2C_STRETCH_CTRL_REG_s    StretchCtrl;        /* 0x28(RW): Extend SCL clock for START/STOP setup and hold timing */
    volatile B8_I2C_DATA_REG_s            FifoModeReadData;   /* 0x2C(RO): I2C FIFO-mode Data Register (LSB 8-bit) */
    volatile UINT32                       DelayCounter;       /* 0x30(RW): B6 I2C Delay Counter Register */
    volatile B8_I2C_DATA_REG_s            StretchLowByte;     /* 0x34(RW): Stretch bypass region low byte */
    volatile UINT32                       Reserved0;          /* 0x38: Reserved */
    volatile B8_I2C_DATA_REG_s            StretchHighByte;    /* 0x3C(RW): Stretch bypass region high byte */
    volatile B8_I2C_DATA_REG_s            FifoModeNack;       /* 0x40(RW): FIFO mode nack log */
} B8_I2C_REG_s;

#endif /* AMBA_B8_REG_I2C_H */
