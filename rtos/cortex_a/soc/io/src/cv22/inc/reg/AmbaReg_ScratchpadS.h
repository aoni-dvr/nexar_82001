/**
 *  @file AmbaReg_ScratchpadS.h
 *
 *  @copyright Copyright (c) 2019 Ambarella, Inc.
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
 *  @details Definitions & Constants for Secure Scratchpad Control APIs
 *
 */

#ifndef AMBA_REG_SCRATCHPAD_S_H
#define AMBA_REG_SCRATCHPAD_S_H

#ifndef AMBA_TYPES_H
#include "AmbaTypes.h"
#endif

/*
 * Secure Scratchpad: Random Number Generator Control Register
 */
typedef struct {
    UINT32  SampleComplete:     1;      /* [0] 1 = Assert interrupt at samping completed */
    UINT32  StartSample:        1;      /* [1] Write 1 to start sampling. Read for busy status (0 = Done, 1 = Busy) */
    UINT32  Reserved0:          2;      /* [3:2] Reserved */
    UINT32  SampleRate:         2;      /* [5:4] 0 = 32K cycles, 1 = 64K cycles, 2 = 128K cycles, 3 = 256K cycles */
    UINT32  Reserved1:          26;     /* [31:6] Reserved */
} AMBA_RNG_CTRL_REG_s;

/*
 * Secure Scratchpad: VROM Control Register
 */
typedef struct {
    UINT32  VromPDE:            1;      /* [0](RW) VROM enable signal */
    UINT32  VromPRN:            1;      /* [1](RO) VROM PRN */
    UINT32  Reserved:           30;     /* [31:2] Reserved */
} AMBA_VROM_CTRL_REG_s;

/*
 * Secure Scratchpad: PWC Sequance Signal Delay Control Register
 */
typedef struct {
    UINT32  Delay:              8;      /* [7:0] PWC Sequence. Used to set delay time(ms) */
    UINT32  Reserved:           24;     /* [31:8] Reserved */
} AMBA_PWC_PSEQ_DELAY_REG_s;

/*
 * Secure Scratchpad: Status Register
 */
typedef struct {
    UINT32  RtcClk1s:           1;      /* [0] RTC one-second clock */
    UINT32  Reserved:           1;      /* [1] Reserved */
    UINT32  AlarmWakeUp:        1;      /* [2] RTC alarm wake up state */
    UINT32  PwcWakeUp:          1;      /* [3] RTC wake state with the PWC_WKUP pin */
    UINT32  Reserved1:          28;     /* [31:4] Reserved */
} AMBA_RTC_STATUS_REG_s;

/*
 * Secure Scratchpad: Reset Register
 */
typedef struct {
    UINT32  Reset:              1;      /* [0] Reset the RTC status. 1 = clear the RTC status */
    UINT32  Reserved:           31;     /* [31:1] Reserved */
} AMBA_RTC_RESET_REG_s;

/*
 * Secure Scratchpad: Extra Wake Up Status Register
 */
typedef struct {
    UINT32  PwcWakeUp1:         1;      /* [0] PWC wake state with the PWC_WKUP0 pin */
    UINT32  PwcWakeUp2:         1;      /* [1] PWC wake state with the PWC_WKUP1 pin */
    UINT32  PwcWakeUp3:         1;      /* [2] Reserve */
    UINT32  Reserved:           29;     /* [31:3] Reserved */
} AMBA_PWC_WAKE_UP_REG_s;

/*
 * Secure Scratchpad: PWC Alert Register
 */
typedef struct {
    UINT32  Alert:              1;      /* [0] PWC is forced off by pressing PWC_WKUP longer than 16s. */
    UINT32  Reserved:           31;     /* [31:1] Reserved */
} AMBA_PWC_ALERT_REG_s;

/*
 * Secure Scratchpad: PWC Strobe Register
 */
typedef struct {
    UINT32  KeyStrobe:          1;      /* [0] Strobe to store scrambling keys into PWC */
    UINT32  Reserved:           31;     /* [31:1] Reserved */
} AMBA_PWC_STROBE_REG_s;

/*
 * Secure Scratchpad: PWC Status Read Register
 */
typedef struct {
    UINT32  Scratchpad0:        1;      /* [0] Content will be retained if RTC module is kept powered on. */
    UINT32  Scratchpad1:        1;      /* [1] Content will be retained if RTC module is kept powered on. */
    UINT32  ForcePseq3High:     1;      /* [2] 1 = Force PSEQ3 pin keep HIGH */
    UINT32  EnableAlarmWakeup:  1;      /* [3] 1 = Enable Wake-Up by alarm */
    UINT32  Scratchpad2:        1;      /* [4] Content will be retained if RTC module is kept powered on. */
    UINT32  Scratchpad3:        1;      /* [5] Content will be retained if RTC module is kept powered on. */
    UINT32  Scratchpad4:        1;      /* [6] Content will be retained if RTC module is kept powered on. */
    UINT32  Scratchpad5:        1;      /* [7] Content will be retained if RTC module is kept powered on. */
    UINT32  Reserved:           24;     /* [31:8] Reserved */
} AMBA_PWC_STATUS_REG_s;

/*
 * Secure Scratchpad: All Registers
 */
typedef struct {
    volatile AMBA_RNG_CTRL_REG_s                RngCtrl;            /* 0x000(RW): Random Number Generation Control Register */
    volatile UINT32                             RngData[4];         /* 0x004-0x10(RW): Sampled Random Number Data Registers */
    volatile AMBA_VROM_CTRL_REG_s               VromCtrl;           /* 0x014(RW): VROM control register */
    volatile UINT32                             JtagCtrl;           /* 0x018(RW): JTAG Enable Register (LSB 1-bit) */
    volatile UINT32                             Reserved0;          /* 0x01C: Reserved */
    volatile AMBA_PWC_PSEQ_DELAY_REG_s          Pseq1Delay;         /* 0x020(RW): PWC Time Interval between PWC_WKUP and PWC_PSEQ1 Voltage Changes */
    volatile AMBA_PWC_PSEQ_DELAY_REG_s          Pseq2Delay;         /* 0x024(RW): PWC Time Interval between PWC_PSEQ1 and PWC_PSEQ2 Voltage Changes */
    volatile AMBA_PWC_PSEQ_DELAY_REG_s          Pseq3Delay;         /* 0x028(RW): PWC Time Interval between PWC_PSEQ2 and PWC_PSEQ3 Voltage Changes */
    volatile UINT32                             AlarmWrite;         /* 0x02C(RW): RTC Alarm Time Programming Register */
    volatile UINT32                             CurTimeWrite;       /* 0x030(RW): RTC Current Time Programming Register */
    volatile UINT32                             CurTimeRead;        /* 0x034(RO): RTC Current Time Value Register */
    volatile UINT32                             AlarmRead;          /* 0x038(RO): RTC Alarm Time Value Register */
    volatile AMBA_RTC_STATUS_REG_s              RtcStatus;          /* 0x03C(RO): RTC Status Register */
    volatile AMBA_RTC_RESET_REG_s               RtcReset;           /* 0x040(RW): RTC Reset Register */
    volatile UINT32                             RtcClkSelect;       /* 0x044(RW): RTC Real Time Clock Input Select Register (LSB 1-bit) */
    volatile UINT32                             CurTimerLatch;      /* 0x048(RW): RTC Latch Current Timer Value From Write(0x30) to Read(0x34) */
    volatile AMBA_PWC_WAKE_UP_REG_s             ExtraWakeUpStatus;  /* 0x04C(RO): RTC Extra Wake Up Status Register */
    volatile AMBA_PWC_ALERT_REG_s               PwcDownAlert;       /* 0x050(RO): PWC Down Alert Status Register (LSB 1-bit) */
    volatile UINT32                             PwcStatusRead;      /* 0x054(RO): PWC Status Read Register */
    volatile UINT32                             PwcStatusWrite;     /* 0x058(RW): PWC Status Set Register */
    volatile AMBA_PWC_PSEQ_DELAY_REG_s          Pseq4Delay;         /* 0x05C(RW): PWC Time Interval between PWC_PSEQ3 and PWC_RSTOB Voltage Changes */
    volatile UINT32                             Reserved1[12];      /* 0x060-0x08C Reserved */
    volatile UINT32                             PwcStrobe;          /* 0x090(RW): PWC Data Strobe Register (LSB 1-bit) */
    volatile UINT32                             SecureBoot;         /* 0x094(RO): Secure Boot Status Register (LSB 1-bit) */
    volatile UINT32                             Reserved2[2];       /* 0x098-0x09C Reserved */
    volatile UINT32                             OtpCtrl;            /* 0x0A0(RW): OTP Control Register */
    volatile UINT32                             OtpObsv;            /* 0x0A4(RO): OTP Observation Register */
    volatile UINT32                             OtpReadData;        /* 0x0A8(RO): OTP Read Data Output Register */
    volatile UINT32                             Reserved3;          /* 0x0AC: Reserved */
    volatile UINT32                             RngData4;           /* 0x0B0(RW): Sampled Random Number Data Registers */
} AMBA_SCRATCHPAD_S_REG_s;

/*
 * Defined in AmbaMmioBase.asm
 */
extern AMBA_SCRATCHPAD_S_REG_s *const pAmbaScratchpadS_Reg;

#endif /* AMBA_REG_SCRATCHPAD_S_H */
