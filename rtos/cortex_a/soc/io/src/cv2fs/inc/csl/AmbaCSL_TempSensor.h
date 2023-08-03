/**
 *  @file AmbaCSL_TempSensor.h
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
 *  @details Definitions & Constants for DDR Controller CSL APIs
 *
 */

#ifndef AMBA_CSL_TEMPSENSOR_H
#define AMBA_CSL_TEMPSENSOR_H

#include "AmbaReg_TempSensor.h"

/*
 * Inline Function Definitions
 */
static inline void AmbaCSL_Ts0Enable(void)
{
    (pAmbaTempSensor_Reg->TsEnable |= 0x1U);
}
static inline void AmbaCSL_Ts1Enable(void)
{
    pAmbaTempSensor_Reg->TsEnable |= 0x2U;
}
static inline UINT32 AmbaCSL_GetTsEnState(void)
{
    return pAmbaTempSensor_Reg->TsEnable;
}

static inline void AmbaCSL_Ts0Ctrl0(UINT32 Val)
{
    pAmbaTempSensor_Reg->Ts0Ctrl0 = Val;
}
static inline void AmbaCSL_Ts1Ctrl0(UINT32 Val)
{
    pAmbaTempSensor_Reg->Ts1Ctrl0 = Val;
}
static inline void AmbaCSL_Ts0SampleNum(UINT32 Val)
{
    pAmbaTempSensor_Reg->Ts0SampleNum = Val & 0x0FU;
}
static inline void AmbaCSL_Ts1SampleNum(UINT32 Val)
{
    pAmbaTempSensor_Reg->Ts1SampleNum = Val & 0x0FU;
}

static inline void AmbaCSL_Ts0SetProbe0(UINT32 Val)
{
    pAmbaTempSensor_Reg->Ts0Probe[0] = Val;
}
static inline void AmbaCSL_Ts0SetProbe1(UINT32 Val)
{
    pAmbaTempSensor_Reg->Ts0Probe[1] = Val;
}
static inline void AmbaCSL_Ts1SetProbe0(UINT32 Val)
{
    pAmbaTempSensor_Reg->Ts1Probe[0] = Val;
}
static inline void AmbaCSL_Ts1SetProbe1(UINT32 Val)
{
    pAmbaTempSensor_Reg->Ts1Probe[1] = Val;
}

static inline UINT32 AmbaCSL_GetTs0Output(UINT32 Index)
{
    return pAmbaTempSensor_Reg->Ts0Data[Index];
}
static inline UINT32 AmbaCSL_GetTs1Output(UINT32 Index)
{
    return pAmbaTempSensor_Reg->Ts1Data[Index];
}

void AmbaCSL_TempSensor0SetCtrl(UINT32 Channel, UINT32 EnableDOC);
void AmbaCSL_TempSensor1SetCtrl(UINT32 Channel, UINT32 EnableDOC);

#endif /* AMBA_CSL_TEMPSENSOR_H */
