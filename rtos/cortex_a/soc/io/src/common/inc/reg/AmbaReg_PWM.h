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

#ifndef AMBA_REG_PWM_H
#define AMBA_REG_PWM_H

#ifndef AMBA_TYPES_H
#include "AmbaTypes.h"
#endif

/*
 * PWM: PWM-0/1/2/3 Control Register
 */
typedef struct {
    UINT32  OffTicks:           16;     /* [15:0] Clock cycles minus 1 for output logic low */
    UINT32  OnTicks:            16;     /* [31:16] Clock cycles minus 1 for output logic high */
} AMBA_PWM_CTRL_16B_REG_s;

/*
 * PWM: PWM-0/2 Enable Register
 */
typedef struct {
    UINT32  Enable:             1;      /* [0] 1 = Enable PWM-1/3 signal output */
    UINT32  Divider:            30;     /* [30:1] Clock divider minus 1 of PWM-1/3 */
    UINT32  Reserved:           1;      /* [31] Reserved */
} AMBA_PWM_ENABLE_0_REG_s;

/*
 * PWM: PWM-1/3 Enable Register
 */
typedef struct {
    UINT32  Enable:             1;      /* [0] 1 = Enable PWM-2/4 signal output */
    UINT32  Divider:            30;     /* [30:1] Clock divider minus 1 of PWM-2/4 */
    UINT32  Complementary:      1;      /* [31] 1 = PWM-2/4 is the signal in opposite direction of PWM-1/3 */
} AMBA_PWM_ENABLE_1_REG_s;

/*
 * PWM: Stepper Control Register
 */
typedef struct {
    UINT32  ClkDivider:         12;     /* [11:0] phase clock = gclk_motor / (2*(ClkDivider + 1)) */
    UINT32  LastPinState:       1;      /* [12] 0 = logical low, 1 = logical high */
    UINT32  UseLastPinState:    1;      /* [13] last pin state is, 0 = same as last phase, 1 = LastPinState */
    UINT32  Reserved0:          2;      /* [15:14] Reserved */
    UINT32  PatternSize:        6;      /* [21:16] pattern size = (PatternSize + 1) from MSB */
    UINT32  Reserved1:          9;      /* [30:22] Reserved */
    UINT32  Reset:              1;      /* [31] 1 = Reset internal bit pointer of pattern */
} AMBA_PWM_STEP_CTRL_REG_s;

/*
 * PWM: Stepper Count Register
 */
typedef struct {
    UINT32  NumPhase:           16;     /* [15:0] Number of phases to be done */
    UINT32  RepeatFirst:        7;      /* [22:16] Output the first phase (4*RepeatFirst + 1) times */
    UINT32  Rewind:             1;      /* [23] 1 = Rewind */
    UINT32  RepeatLast:         7;      /* [30:24] Output the last phase (4*RepeatLast + 1) times */
    UINT32  Reserved:           1;      /* [31] Reserved */
} AMBA_PWM_STEP_COUNT_REG_s;

/*
 * PWM: Stepper Status Register
 */
typedef struct {
    UINT32  RepeatCount:        16;     /* [15:0] Repeat counter */
    UINT32  Reserved0:          8;      /* [23:16] */
    UINT32  RepeatLastFlag:     1;      /* [24] Repeat last flag status */
    UINT32  RepeatFlag:         1;      /* [25] Repeat flag status */
    UINT32  RepeatFirstFlag:    1;      /* [26] Repeat first flag status */
    UINT32  Active:             1;      /* [27] Active flag status */
    UINT32  Reserved1:          4;      /* [31:28] Reserved */
} AMBA_PWM_STEP_STATUS_REG_s;

/*
 * PWM: Stepper Burst Control Register: BurstCtrl[1:0]
 */
#define PWM_STEPPER_BURST_NONE          0x0     /* Disable burst mode */
#define PWM_STEPPER_BURST_FORWARD       0x1     /* Enable burst mode in forward direction */
#define PWM_STEPPER_BURST_BACKWARD      0x2     /* Enable burst mode in backward direction */
#define PWM_STEPPER_BURST_ROUND_TRIP    0x3     /* Enable burst mode in forward direction and then in backward direction */

/*
 * PWM: Stepper Burst Control Register
 */
typedef struct {
    UINT32  BurstLength:        10;     /* [9:0] Number of phases to be done */
    UINT32  Reserved0:          6;      /* [15:10] Reserved */
    UINT32  BurstCtrl:          2;      /* [17:16] 0 = No burst, 1 = Burst forward, 2 = Burst backward, 3 = Burst forward and then backward */
    UINT32  Reserved1:          14;     /* [31:18] Reserved */
} AMBA_PWM_STEP_BURST_CTRL_REG_s;

/*
 * PWM: Stepper Burst Clock Control Register
 */
typedef struct {
    UINT32  ClkDivider0:        12;     /* [11:0] Clock divider for the 1st/3rd burst phase */
    UINT32  Reserved0:          4;      /* [15:12] Reserved */
    UINT32  ClkDivider1:        12;     /* [27:16] Clock divider for the 2nd/4th burst phase */
    UINT32  Reserved1:          4;      /* [31:28] Reserved */
} AMBA_PWM_STEP_BURST_CLK_CTRL_REG_s;

/*
 * PWM: Micro Stepper Control Register
 */
typedef struct {
    UINT32  ClockDiv:           4;      /* [3:0] Clock divider, clk = GCLK_CORE / (clk_div + 1) */
    UINT32  InvertPolarity:     1;      /* [4] Invert PWM polarity */
    UINT32  PinsToLogic:        1;      /* [5] 0 - Set pins to logic low after last phase if set_last = 1, 1 - Set pins to logic high after last phase if set_last = 1 */
    UINT32  PinsToLastPol:      1;      /* [6] 0 - Hold last phase state, 1 - Set pins to last_pol after last phase */
    UINT32  ContinueRepeat:     1;      /* [7] Continuous repeat */
    UINT32  ForceStop:          1;      /* [8] Force stop (automatically clears) */
    UINT32  Reset:              1;      /* [9] Internal bit pointer reset (automatically clears) */
    UINT32  EnableMs01:         1;      /* [10] Micro Stepper x0 / x1 output enable */
    UINT32  EnableMs23:         1;      /* [11] Micro Stepper x2 / x3 output enable */
    UINT32  Reserved:           20;     /* [31:12] Reserved */
} AMBA_PWM_MICRO_STEP_CTRL_REG_s;

/*
 * PWM: Micro Stepper PWM Control Register
 */
typedef struct {
    UINT32 PwmNumber:           16;     /* [15:0] Each micro step contains (PwmNumber + 1) PWM waves */
    UINT32 PwmPeriod:           8;      /* [23:16] Ticks per PWM wave = (PwmPeriod + 1) */
    UINT32 Reserved:            8;      /* [31:24] Reserved */
} AMBA_PWM_MICRO_STEP_PWM_CTRL_REG_s;

/*
 * PWM: Micro Stepper Count Register 0
 */
typedef struct {
    UINT32 RepeatFirst:         8;      /* [7:0] Output the first phase (RepeatFirst*256) times */
    UINT32 RepeatLast:          8;      /* [15:8] Repeat the last phase (RepeatLast*256) times */
    UINT32 Reserved:            16;     /* [31:16] Reserved */
} AMBA_PWM_MICRO_STEP_COUNT_0_REG_s;

/*
 * PWM: Micro Stepper Count Register 1
 */
typedef struct {
    UINT32 MsCount:             24;     /* [23:0] Do (MsCount+1) micro steps */
    UINT32 EnableRewind:        1;      /* [24] Rewind enable */
    UINT32 Reserved:            7;      /* [31:25] Reserved */
} AMBA_PWM_MICRO_STEP_COUNT_1_REG_s;

/*
 * PWM: Micro Stepper Status Register
 */
typedef struct {
    UINT32 RepeatCnt:           24;     /* [23:0] Repeat counter */
    UINT32 RepeatLastFlag:      1;      /* [24] The flag of repeat last status */
    UINT32 RepeatFlag:          1;      /* [25] The flag of repeat status */
    UINT32 RepeatFirstFlag:     1;      /* [26] The flag of repeat first status */
    UINT32 ActiveFlag:          1;      /* [27] The flag of active status */
    UINT32 Reserved:            4;      /* [31:28] Reserved */
} AMBA_PWM_MICRO_STEP_STATUS_REG_s;

/*
 * PWM: Micro Stepper PWM Wave Register
 */
typedef struct {
    UINT32 PwmHigh0:            8;      /* [7:0]   Ticks of high period for each PWM wave in micro step 0/4/8/../252 */
    UINT32 PwmHigh1:            8;      /* [15:8]  Ticks of high period for each PWM wave in micro step 1/5/9/../253 */
    UINT32 PwmHigh2:            8;      /* [23:16] Ticks of high period for each PWM wave in micro step 2/6/10/../254 */
    UINT32 PwmHigh3:            8;      /* [31:24] Ticks of high period for each PWM wave in micro step 3/7/11/../255 */
} AMBA_PWM_MICRO_STEP_PWM_WAVE_REG_s;

/*
 * PWM: Stepper/Micro Stepper Interrupt Status Register
 */
typedef struct {
    UINT32  StepperAEndInt:         1;  /* [0] 1 = Stepper A is at end, cleared by software */
    UINT32  StepperBEndInt:         1;  /* [1] 1 = Stepper B is at end, cleared by software */
    UINT32  StepperCEndInt:         1;  /* [2] 1 = Stepper C is at end, cleared by software */
    UINT32  Reserved0:              2;  /* [4:3] */
    UINT32  MicroStepperAEndInt:    1;  /* [5] 1 = Micro Stepper A is at end, cleared by software */
    UINT32  MicroStepperBEndInt:    1;  /* [6] 1 = Micro Stepper B is at end, cleared by software */
    UINT32  MicroStepperCEndInt:    1;  /* [7] 1 = Micro Stepper C is at end, cleared by software */
    UINT32  Reserved1:              1;  /* [8] */
    UINT32  MicroStepperAStepInt:   1;  /* [9]  1 = Micro Stepper A is at an angle step, cleared by software */
    UINT32  MicroStepperBStepInt:   1;  /* [10] 1 = Micro Stepper B is at an angle step, cleared by software */
    UINT32  MicroStepperCStepInt:   1;  /* [11] 1 = Micro Stepper C is at an angle step, cleared by software */
    UINT32  Reserved2:              20; /* [31:12] */
} AMBA_PWM_STEP_INT_STATUS_REG_s;

/*
 * PWM: Stepper/Micro Stepper Interrupt Control Register
 */
typedef struct {
    UINT32  StepperAEndIntEn:       1;  /* [0] 1 = Trigger interrupt when Stepper A is at end */
    UINT32  StepperBEndIntEn:       1;  /* [1] 1 = Trigger interrupt when Stepper B is at end */
    UINT32  StepperCEndIntEn:       1;  /* [2] 1 = Trigger interrupt when Stepper C is at end */
    UINT32  Reserved0:              2;  /* [4:3] */
    UINT32  MicroStepperAEndIntEn:  1;  /* [5] 1 = Trigger interrupt when Micro Stepper A is at end */
    UINT32  MicroStepperBEndIntEn:  1;  /* [6] 1 = Trigger interrupt when Micro Stepper B is at end */
    UINT32  MicroStepperCEndIntEn:  1;  /* [7] 1 = Trigger interrupt when Micro Stepper C is at end */
    UINT32  Reserved1:              1;  /* [8] */
    UINT32  MicroStepperAStepIntEn: 1;  /* [9] 1 = Trigger interrupt when Micro Stepper A is at angle step */
    UINT32  MicroStepperBStepIntEn: 1;  /* [10] 1 = Trigger interrupt when Micro Stepper B is at angle step */
    UINT32  MicroStepperCStepIntEn: 1;  /* [11] 1 = Trigger interrupt when Micro Stepper C is at angle step */
    UINT32  Reserved2:              1;  /* [12] */
    UINT32  MicroStepperAAngleStep: 2;  /* [14:13] Angle step of Micro Stepper A is every 0 = 45 degree, 1 = 90 degree, 2 = 180 degree, 3 = 360 degree */
    UINT32  MicroStepperBAngleStep: 2;  /* [16:15] Angle step of Micro Stepper B is every 0 = 45 degree, 1 = 90 degree, 2 = 180 degree, 3 = 360 degree */
    UINT32  MicroStepperCAngleStep: 2;  /* [18:17] Angle step of Micro Stepper C is every 0 = 45 degree, 1 = 90 degree, 2 = 180 degree, 3 = 360 degree */
    UINT32  Reserved:               13; /* [31:19] */
} AMBA_PWM_STEP_INT_CTRL_REG_s;

typedef struct {
    volatile UINT32                             Data[2];            /* Pattern(63:0) = {Data[0](31:0), Data[1](31:0)} */
} AMBA_STEPPER_PATTERN_REG_s;

/*
 * PWM: All Registers
 */
typedef struct {
    volatile UINT32                                 Ctrl;                   /* 0x00(RW): Stepper Control Register */
    volatile AMBA_STEPPER_PATTERN_REG_s             Pattern[4];             /* 0x04-0x20(RW): Stepper Pattern Registers */
    volatile UINT32                                 Count;                  /* 0x24(RW): Stepper Conunt Register */
    volatile AMBA_PWM_STEP_STATUS_REG_s             Status;                 /* 0x28(RO): Stepper Status Register */
    volatile AMBA_PWM_STEP_BURST_CTRL_REG_s         BurstCtrl;              /* 0x2C(RW): Stepper Burst Control Register */
    volatile AMBA_PWM_STEP_BURST_CLK_CTRL_REG_s     BurstClkCtrl[2];        /* 0x30-0x34(RW): Stepper Burst Clock Control Register */
    volatile UINT32                                 ActualCount;            /* 0x38(RO): Stepper Accomplished Phase Amount Register (LSB 16-bit) */
} AMBA_PWM_STEPPER_MOTOR_REG_s;

typedef struct {
    volatile AMBA_PWM_MICRO_STEP_CTRL_REG_s         Ctrl;                   /* 0x00(RW): Micro Stepper Control Register */
    volatile AMBA_PWM_MICRO_STEP_PWM_CTRL_REG_s     PwmCtrl;                /* 0x04(RW): Micro Stepper PWM Control Register */
    volatile AMBA_PWM_MICRO_STEP_COUNT_0_REG_s      Count0;                 /* 0x08(RW): Micro Stepper Count Register 0 */
    volatile AMBA_PWM_MICRO_STEP_COUNT_1_REG_s      Count1;                 /* 0x0C(RW): Micro Stepper Count Register 1 */
    volatile AMBA_PWM_MICRO_STEP_STATUS_REG_s       Status;                 /* 0x10(RW): Micro StepperStatus Register */
    volatile UINT32                                 ActualCount;            /* 0x14(RO): Micro Stepper Accomplished Phase Amount Register (LSB 24-bit) */
} AMBA_PWM_MICRO_STEPPER_REG_s;

typedef struct {
    volatile UINT32                                 Reserved0;              /* 0x000: Reserved */
    volatile AMBA_PWM_ENABLE_0_REG_s                Pwm0Enable;             /* 0x004(RW): PMW0 Enable Register */
    volatile UINT32                                 Reserved1;              /* 0x008: Reserved */
    volatile AMBA_PWM_ENABLE_1_REG_s                Pwm1Enable;             /* 0x00C(RW): PMW1 Enable Register */
    volatile UINT32                                 Reserved2;              /* 0x010: Reserved */
    volatile AMBA_PWM_ENABLE_0_REG_s                Pwm2Enable;             /* 0x014(RW): PMW2 Enable Register */
    volatile UINT32                                 Reserved3;              /* 0x018: Reserved */
    volatile AMBA_PWM_ENABLE_1_REG_s                Pwm3Enable;             /* 0x01C(RW): PMW3 Enable Register */
    volatile UINT32                                 Pwm0CtrlBank;           /* 0x020(RW): Bank register for PWM0 Control */
    volatile UINT32                                 Pwm1CtrlBank;           /* 0x024(RW): Bank register for PWM1 Control */
    volatile UINT32                                 Pwm2CtrlBank;           /* 0x028(RW): Bank register for PWM2 Control */
    volatile UINT32                                 Pwm3CtrlBank;           /* 0x02C(RW): Bank register for PWM3 Control */
    volatile UINT32                                 Reserved4[1012];        /* 0x030-0xFFC: Reserved */
} AMBA_PWM_REG_s;

typedef struct {
    volatile AMBA_PWM_STEPPER_MOTOR_REG_s           StepperA;               /* 0x000-0x038: Stepper Motor Interface A Register */
    volatile UINT32                                 Reserved0[17];          /* 0x03C-0x07C: Reserved */
    volatile AMBA_PWM_STEPPER_MOTOR_REG_s           StepperB;               /* 0x080-0x0B8: Stepper Motor Interface B Register */
    volatile UINT32                                 Reserved1[17];          /* 0x0BC-0x0FC: Reserved */
    volatile AMBA_PWM_STEPPER_MOTOR_REG_s           StepperC;               /* 0x100-0x138: Stepper Motor Interface C Register */
    volatile UINT32                                 Reserved2[114];         /* 0x13C-0x300: Reserved */
    volatile AMBA_PWM_ENABLE_0_REG_s                PwmB0Enable;            /* 0x304: PWM B0 Enable Register */
    volatile UINT32                                 Reserved3;              /* 0x308: Reserved */
    volatile AMBA_PWM_ENABLE_1_REG_s                PwmB1Enable;            /* 0x30C: PMW B1 Enable Register */
    volatile UINT32                                 Reserved4;              /* 0x310: Reserved */
    volatile AMBA_PWM_ENABLE_0_REG_s                PwmC0Enable;            /* 0x314: PMW C0 Enable Register */
    volatile UINT32                                 Reserved5;              /* 0x318: Reserved */
    volatile AMBA_PWM_ENABLE_1_REG_s                PwmC1Enable;            /* 0x31C: PMW C1 Enable Register */
    volatile UINT32                                 PwmB0CtrlBank;          /* 0x320(RW): Bank register for PWM B0 Control */
    volatile UINT32                                 PwmB1CtrlBank;          /* 0x324(RW): Bank register for PWM B1 Control */
    volatile UINT32                                 PwmC0CtrlBank;          /* 0x328(RW): Bank register for PWM C0 Control */
    volatile UINT32                                 PwmC1CtrlBank;          /* 0x32C(RW): Bank register for PWM C1 Control */
    volatile UINT32                                 Reserved6[20];          /* 0x330-0x37C: Reserved */
    volatile AMBA_PWM_STEP_INT_STATUS_REG_s         StepperIntStatus;       /* 0x380(RW): Stepper Interrupt Status Register */
    volatile AMBA_PWM_STEP_INT_CTRL_REG_s           StepperIntCtrl;         /* 0x384(RW): Stepper Interrupt Control Control */
    volatile UINT32                                 Reserved7[30];          /* 0x388-0x3FC: Reserved */
    volatile AMBA_PWM_MICRO_STEPPER_REG_s           MicroStepCtrlA;         /* 0x400-0x414: Registers of Micro Stepper A */
    volatile UINT32                                 Reserved8[58];          /* 0x418-0x4FC: Reserved */
    volatile AMBA_PWM_MICRO_STEPPER_REG_s           MicroStepCtrlB;         /* 0x500-0x514: Registers of Micro Stepper B */
    volatile UINT32                                 Reserved9[58];          /* 0x518-0x5FC: Reserved */
    volatile AMBA_PWM_MICRO_STEPPER_REG_s           MicroStepCtrlC;         /* 0x600-0x614: Registers of Micro Stepper C */
    volatile UINT32                                 Reserved10[122];        /* 0x618-0x7FC: Reserved */
    volatile AMBA_PWM_MICRO_STEP_PWM_WAVE_REG_s     MicroStepPatternA0[64]; /* 0x800-0x8FC(RW): Configuration of each micro step of Micro Stepper A0/A1 */
    volatile AMBA_PWM_MICRO_STEP_PWM_WAVE_REG_s     MicroStepPatternA2[64]; /* 0x900-0x9FC(RW): Configuration of each micro step of Micro Stepper A2/A3 */
    volatile AMBA_PWM_MICRO_STEP_PWM_WAVE_REG_s     MicroStepPatternB0[64]; /* 0xA00-0xAFC(RW): Configuration of each micro step of Micro Stepper B0/B1 */
    volatile AMBA_PWM_MICRO_STEP_PWM_WAVE_REG_s     MicroStepPatternB2[64]; /* 0xB00-0xBFC(RW): Configuration of each micro step of Micro Stepper B2/B3 */
    volatile AMBA_PWM_MICRO_STEP_PWM_WAVE_REG_s     MicroStepPatternC0[64]; /* 0xC00-0xCFC(RW): Configuration of each micro step of Micro Stepper C0/C1 */
    volatile AMBA_PWM_MICRO_STEP_PWM_WAVE_REG_s     MicroStepPatternC2[64]; /* 0xD00-0xDFC(RW): Configuration of each micro step of Micro Stepper C2/C3 */
} AMBA_PWM_STEPPER_REG_s;

/*
 * Defined in AmbaMmioBase.asm
 */
extern AMBA_PWM_REG_s *pAmbaPWM_Reg[3];
#if defined(CONFIG_QNX)
extern AMBA_PWM_STEPPER_REG_s *pAmbaPwmStep_Reg;
#else
extern AMBA_PWM_STEPPER_REG_s *const pAmbaPwmStep_Reg;
#endif

#endif /* AMBA_REG_PWM_H */
