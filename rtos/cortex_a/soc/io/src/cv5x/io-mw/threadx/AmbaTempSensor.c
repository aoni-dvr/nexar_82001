/**
 *  @file AmbaTempSensor.c
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
 *  @details Temperature Sensor Control APIs
 *
 */

#include "AmbaTypes.h"
#include "AmbaDef.h"
#include "AmbaKAL.h"
#include "AmbaADC.h"

#include "AmbaDrvEntry.h"
#include "AmbaTempSensor.h"

#include "AmbaRTSL_TempSensor.h"

static AMBA_KAL_MUTEX_t AmbaTempSensorMutex;

/**
 *  Initializes driver-wide data structures and resources
 *  @return error code
 *  @note this function is an internal using only API
 */
UINT32 AmbaTempSensor_DrvEntry(void)
{
    static char AmbaTempSensorMutexName[20] = "AmbaTempSensorMutex";
    UINT32 RetVal = TEMPSENSOR_ERR_NONE;

    if (AmbaKAL_MutexCreate(&AmbaTempSensorMutex, AmbaTempSensorMutexName) != KAL_ERR_NONE) {
        RetVal = TEMPSENSOR_ERR_UNEXPECTED;
    }

    return RetVal;
}

/**
 *  AmbaTempSensor_GetTemp - Find an avaliable timer instance
 *  @param[in] EnableDOC Mesure method
 *  @param[in] Channel The specified channel id
 *  @param[out] pTemp Temperature value
 *  @return error code
 */
UINT32 AmbaTempSensor_GetTemp(UINT32 EnableDOC, UINT32 Channel, DOUBLE *pTemp)
{
    UINT32 RetVal = TEMPSENSOR_ERR_NONE;
    UINT32 Data;

    (void)EnableDOC;

    if (Channel < AMBA_TEMPSEN_NUM) {
        if (AmbaKAL_MutexTake(&AmbaTempSensorMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = TEMPSENSOR_ERR_MUTEX;
        } else {
            RetVal = AmbaADC_SingleRead(AMBA_ADC_CHANNEL_T2V, &Data);
            if (RetVal == OK) {
                RetVal = AmbaRTSL_GetTemp(Data, pTemp);
            }

            if (AmbaKAL_MutexGive(&AmbaTempSensorMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = TEMPSENSOR_ERR_UNEXPECTED;
            }
        }
    } else {
        RetVal = TEMPSENSOR_ERR_ARG;
    }

    return RetVal;
}

