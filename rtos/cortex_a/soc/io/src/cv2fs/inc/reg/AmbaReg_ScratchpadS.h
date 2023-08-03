/**
 *  @file AmbaReg_ScratchpadS.h
 *
 *  @copyright Copyright (c) 2020 Ambarella, Inc.
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
 *  @details Definitions & Constants for AHB Secure Scratchpad Control APIs
 *
 */

#ifndef AMBA_REG_SCRATCHPAD_S_H
#define AMBA_REG_SCRATCHPAD_S_H

#ifndef AMBA_TYPES_H
#include "AmbaTypes.h"
#endif

/*-----------------------------------------------------------------------------------------------*\
 * Secure Scratchpad: Random Number Generator Control Register
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    UINT32  SampleComplete:     1;      /* [0][RW] 1 = Assert interrupt at samping completed */
    UINT32  StartSample:        1;      /* [1][WO] Write 1 to start sampling. Read for busy status (0 = Done, 1 = Busy) */
    UINT32  Reserved0:          2;      /* [3:2] Reserved */
    UINT32  SampleRate:         2;      /* [5:4][RW] 0 = 32K cycles, 1 = 64K cycles, 2 = 128K cycles, 3 = 256K cycles */
    UINT32  Reserved1:          26;     /* [31:6] Reserved */
} AMBA_RNG_CTRL_REG_s;

/*-----------------------------------------------------------------------------------------------*\
 * Secure Scratchpad: VROM Control Register
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    UINT32  VromPDE:            1;      /* [0](RW) VROM enable signal */
    UINT32  VromPRN:            1;      /* [1](RO) VROM PRN */
    UINT32  Reserved:           30;     /* [31:2] Reserved */
} AMBA_VROM_CTRL_REG_s;

/*-----------------------------------------------------------------------------------------------*\
 * Secure Scratchpad: Lock Control Register
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    UINT32  SpSecLock:           1;     /* [0] Lock accesses to ahb_scratchpad_sec */
    UINT32  WriteLock:           1;     /* [1] Write lock for SpSecLock. Need to set this to 0 every time before writing to SpSecLock. */
    UINT32  TimeOutUpperBound:  10;     /* [11:2] Upper bound of the timeout mechanism. */
    UINT32  Reserved:           20;     /* [31:12] Reserved */
} AMBA_LOCK_CTRL_S_REG_s;

/*-----------------------------------------------------------------------------------------------*\
 * Secure Scratchpad: Reset Register
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    UINT32  Reset:              1;      /* [0] Reset the RTC status. 1 = clear the RTC status */
    UINT32  Reserved:           31;     /* [31:1] Reserved */
} AMBA_RTC_RESET_REG_s;

/*-----------------------------------------------------------------------------------------------*\
 * Secure Scratchpad: All Registers
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    volatile AMBA_RNG_CTRL_REG_s                RngCtrl;            /* 0x000(RW): Random Number Generation Control Register */
    volatile UINT32                             RngData[4];         /* 0x004-0x10(RW): Sampled Random Number Data Registers */
    volatile AMBA_VROM_CTRL_REG_s               VromCtrl;           /* 0x014(RW): VROM control register */
    volatile UINT32                             JtagCtrl;           /* 0x018(RW): JTAG Enable Register (LSB 1-bit) */
    volatile UINT32                             InsertError;        /* 0x01C(RW): Insert Error (LSB 1-bit) */
    volatile AMBA_LOCK_CTRL_S_REG_s             LockCtrl;           /* 0x020(RW):  */
    volatile UINT32                             Reserved0[3];       /* 0x024-0x02C: Reserved */
    volatile UINT32                             CurTimeWrite;       /* 0x030(RW): RTC Current Time Programming Register */
    volatile UINT32                             CurTimeRead;        /* 0x034(RO): RTC Current Time Value Register */
    volatile UINT32                             Reserved1[2];       /* 0x038-0x03C: Reserved */
    volatile AMBA_RTC_RESET_REG_s               RtcReset;           /* 0x040(RW): RTC Reset Register */
    volatile UINT32                             Reserved2[4];       /* 0x044-0x050: Reserved */
    volatile UINT32                             RtcStatusRead;      /* 0x054(RO): RTC Status Read Register */
    volatile UINT32                             RtcStatusWrite;     /* 0x058(RW): RTC Status Set Register */
    volatile UINT32                             Reserved3[13];      /* 0x05C-0x08C Reserved */
    volatile UINT32                             RtcStrobe;          /* 0x090(RW): RTC Data Strobe Register (LSB 1-bit) */
    volatile UINT32                             SecureBoot;         /* 0x094(RO): Secure Boot Status Register (LSB 1-bit) */
    volatile UINT32                             Reserved4[2];       /* 0x098-0x09C Reserved */
    volatile UINT32                             OtpCtrl1;            /* 0x0A0(RW): OTP Control 1 Register */
    volatile UINT32                             OtpObsv;            /* 0x0A4(RO): OTP Observation Register */
    volatile UINT32                             OtpReadData0;       /* 0x0A8(RO): OTP Read Data Output 0 Register */
    volatile UINT32                             Reserved5;          /* 0x0AC: Reserved */
    volatile UINT32                             RngData4;           /* 0x0B0(RW): Sampled Random Number Data Registers */
    volatile UINT32                             OtpCtrl2;           /* 0x0B4(RW): OTP Control 2 Register */
    volatile UINT32                             OtpReadData1;       /* 0x0B8(RW): OTP Read Data Output 1 Register */
} AMBA_SCRATCHPAD_S_REG_s;

/*
 * Defined in AmbaMmioBase.asm
 */
#ifdef CONFIG_QNX
extern AMBA_SCRATCHPAD_S_REG_s *pAmbaScratchpadS_Reg;
#else
extern AMBA_SCRATCHPAD_S_REG_s *const pAmbaScratchpadS_Reg;
#endif

#endif /* AMBA_REG_SCRATCHPAD_H */
