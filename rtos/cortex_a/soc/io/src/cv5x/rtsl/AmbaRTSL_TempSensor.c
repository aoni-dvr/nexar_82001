/**
 *  @file AmbaRTSL_TempSensor.c
 *
 *  @copyright Copyright (c) 2022 Ambarella, Inc.
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
 *  @details Temperature Sensor RTSL APIs
 *
 */

#include "AmbaTypes.h"

#include "AmbaRTSL_TempSensor.h"
#include "AmbaTempSensor_Def.h"

#include "AmbaCSL_PLL.h"

UINT32 AmbaRTSL_GetTemp(UINT32 AdcData, DOUBLE *AvgTemp)
{
    UINT32 RetVal = TEMPSENSOR_ERR_NONE;
    UINT32 CalibData = 0U;
    UINT64 Tmp = 0U;

    CalibData = AmbaCSL_PllGetT2vData();

    if (CalibData != 0x0U) {
        /* Tt2v=85+Kc(CODEt2v - ADC value@85C) */
        if (CalibData > AdcData) {
            Tmp = ((UINT64)CalibData - (UINT64)AdcData);
            Tmp = 8500000U - (6757U * Tmp);
        } else {
            Tmp = ((UINT64)AdcData - (UINT64)CalibData);
            Tmp = 8500000U + (6757U * Tmp);
        }
    } else {
        Tmp = 6757U * ((UINT64)AdcData - (UINT64)1433U);
    }
    *AvgTemp = ((DOUBLE)Tmp / (DOUBLE)100000U);

    return RetVal;
}

