/**
 *  @file AmbaRTSL_TempSensor.c
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
 *  @details Temperature Sensor RTSL APIs
 *
 */

#include "AmbaTypes.h"

#include "AmbaRTSL_OTP.h"
#include "AmbaRTSL_TempSensor.h"
#include "AmbaTempSensor_Def.h"

#include "AmbaCSL_TempSensor.h"

typedef struct {
    DOUBLE Delta;
    DOUBLE Code25C;
} AMBA_TempSen_CTRL_s;

static AMBA_TempSen_CTRL_s AmbaTempSenCtrl[AMBA_TEMPSEN_NUM];

void AmbaRTSL_TempSensorInit(void)
{
    UINT8 ChId = 0;
    UINT32 HighCode, LowCode, MidCode;

    for (ChId = 0; ChId < AMBA_TEMPSEN_NUM; ChId++) {
        (void)AmbaRTSL_OtpTempSensorRead(ChId, &LowCode, &MidCode, &HighCode);
        AmbaTempSenCtrl[ChId].Delta = ((DOUBLE)HighCode - (DOUBLE)MidCode);
        AmbaTempSenCtrl[ChId].Delta = AmbaTempSenCtrl[ChId].Delta / 80.0;
        AmbaTempSenCtrl[ChId].Code25C = (DOUBLE)MidCode;
    }
}

/**
 *  TempSensor0Fetch - Get the temperature data information
 */
static void TempSensor0Fetch(UINT32 EnableDOC, UINT32 Channel, DOUBLE *AvgTemp)
{
    UINT32 i;
    UINT32 RegVal_0, RegVal_1;
    DOUBLE sum, divisor;

    AmbaCSL_TempSensor0SetCtrl(Channel, EnableDOC);

    AmbaCSL_Ts0Enable();
    while ((AmbaCSL_GetTsEnState() & 0x1U) != 0x0U) { /* En_bit[0]: S0 */
        ;
    }

    sum = 0.0;
    for (i = 0U; i < 8U; i++) {
        RegVal_0 = (AmbaCSL_GetTs0Output(i) & (UINT32)0x1FF);
        RegVal_1 = ((AmbaCSL_GetTs0Output(i) >> 16U) & (UINT32)0x1FF);
        sum = sum + (DOUBLE)RegVal_0 + (DOUBLE)RegVal_1;
    }

    divisor = 16.0;
    if ((AmbaCSL_GetTs0Output(0) & 0x1FFU) == 0U) {
        divisor -= 1.0; //In DOC mode, the 1st data of the 1st round would be 0.
    }

    if (AvgTemp != NULL) {
        sum = sum / divisor;
        *AvgTemp = ((sum - AmbaTempSenCtrl[Channel].Code25C) / AmbaTempSenCtrl[Channel].Delta) + 25.0;
    }
}

/**
 *  TempSensor1Fetch - Get the temperature data information
 */
static void TempSensor1Fetch(UINT32 EnableDOC, UINT32 Channel, DOUBLE *AvgTemp)
{
    UINT32 i;
    UINT32 RegVal_0, RegVal_1;
    DOUBLE sum, divisor;

    AmbaCSL_TempSensor1SetCtrl(Channel, EnableDOC);

    AmbaCSL_Ts1Enable();
    while ((AmbaCSL_GetTsEnState() & 0x2U) != 0x0U) { /* En_bit[1]: S1 */
        ;
    }

    sum = 0.0;
    for (i = 0U; i < 8U; i++) {
        RegVal_0 = (AmbaCSL_GetTs1Output(i) & (UINT32)0x1FF);
        RegVal_1 = ((AmbaCSL_GetTs1Output(i) >> 16U) & (UINT32)0x1FF);
        sum = sum + (DOUBLE)RegVal_0 + (DOUBLE)RegVal_1;
    }

    divisor = 16.0;
    if ((AmbaCSL_GetTs1Output(0) & 0x1FFU) == 0U) {
        divisor -= 1.0; //In DOC mode, the 1st data of the 1st round would be 0.
    }

    if (AvgTemp != NULL) {
        sum = sum / divisor;
        *AvgTemp = ((sum - AmbaTempSenCtrl[Channel].Code25C) / AmbaTempSenCtrl[Channel].Delta) + 25.0;
    }
}

UINT32 AmbaRTSL_GetTemp(UINT32 EnableDOC, UINT32 Channel, DOUBLE *AvgTemp)
{
    UINT32 RetVal = TEMPSENSOR_ERR_NONE;

    if (Channel < AMBA_TEMPSEN1_DOWN_RIGHT) {
        TempSensor0Fetch(EnableDOC, Channel, AvgTemp);
    } else if (Channel < AMBA_TEMPSEN_NUM) {
        TempSensor1Fetch(EnableDOC, Channel - AMBA_TEMPSEN1_DOWN_RIGHT, AvgTemp);
    } else {
        RetVal = TEMPSENSOR_ERR_ARG;
    }

    return RetVal;
}

