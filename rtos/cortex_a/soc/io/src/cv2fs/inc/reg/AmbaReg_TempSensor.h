/**
 *  @file AmbaReg_TempSensor.h
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

#ifndef AMBA_REG_TEMPSENSOR_H
#define AMBA_REG_TEMPSENSOR_H

#ifndef AMBA_TYPES_H
#include "AmbaTypes.h"
#endif

/*
 * Temperature Sensor: Enable Register
 */
typedef struct {
    UINT32  Ts0Enable:      1;      /* [0]: 1 - Ts0 Enabled; 0 - Disable */
    UINT32  Ts1Enable:      1;      /* [1]: 1 - Ts1 Enabled; 0 - Disable */
    UINT32  Reserved:       30;     /* [31:2]: Reserved */
} AMBA_TEMPSENSOR_ENABLE_REG_s;

/*-----------------------------------------------------------------------------------------------*\
 * Temperature Sensor : All Registers
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    volatile UINT32                             TsEnable;           /* 0x080(RW): Temp Sensor enable. TS0 = b[0]; TS1 = b[1] */
    volatile UINT32                             Ts0Ctrl0;           /* 0x084(RW): Sampled Random Number Data Registers */
    volatile UINT32                             Ts0Ctrl1;           /* 0x088(RW): JTAG Enable Register (LSB 1-bit) */
    volatile UINT32                             Ts0SampleNum;       /* 0x08C(RW): Insert Error (LSB 1-bit) */
    volatile UINT32                             Ts0Probe[2];        /* 0x090-0x094(RW):  */
    volatile UINT32                             Ts0Data[8];         /* 0x098-0x0B4:  */
    volatile UINT32                             Ts1Ctrl0;           /* 0x0B8(RW): Sampled Random Number Data Registers */
    volatile UINT32                             Ts1Ctrl1;           /* 0x0BC(RW): JTAG Enable Register (LSB 1-bit) */
    volatile UINT32                             Ts1SampleNum;       /* 0x0C0(RW): Insert Error (LSB 1-bit) */
    volatile UINT32                             Ts1Probe[2];        /* 0x0C4-0x0C8(RW):  */
    volatile UINT32                             Ts1Data[8];         /* 0x0CC-0x0E8:  */
} AMBA_TEMPSENSOR_REG_s;

/*
 * Defined in AmbaMmioBase.asm
 */
extern AMBA_TEMPSENSOR_REG_s *const pAmbaTempSensor_Reg;

#endif /* AMBA_REG_TEMPSENSOR_H */
