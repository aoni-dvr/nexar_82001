/**
 *  @file AmbaSensor.c
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
 *  @details Definitions for Ambarella sensor driver APIs
 *
 */

#include "AmbaTypes.h"
#include "AmbaSensor.h"

AMBA_SENSOR_OBJ_s *pAmbaSensorObj[AMBA_NUM_VIN_CHANNEL];

void AmbaSensor_Hook(const AMBA_SENSOR_CHANNEL_s *pChan, AMBA_SENSOR_OBJ_s *pSensorObj)
{
    if (pChan != NULL) {
        if ((pChan->VinID == AMBA_VIN_CHANNEL0) || (pChan->VinID == AMBA_VIN_CHANNEL1) || (pChan->VinID == AMBA_VIN_CHANNEL2) || (pChan->VinID == AMBA_VIN_CHANNEL3)) {
            pAmbaSensorObj[AMBA_VIN_CHANNEL0] = pSensorObj;
            pAmbaSensorObj[AMBA_VIN_CHANNEL1] = pSensorObj;
            pAmbaSensorObj[AMBA_VIN_CHANNEL2] = pSensorObj;
            pAmbaSensorObj[AMBA_VIN_CHANNEL3] = pSensorObj;
        } else if ((pChan->VinID == AMBA_VIN_CHANNEL4) || (pChan->VinID == AMBA_VIN_CHANNEL5) || (pChan->VinID == AMBA_VIN_CHANNEL6) || (pChan->VinID == AMBA_VIN_CHANNEL7)) {
            pAmbaSensorObj[AMBA_VIN_CHANNEL4] = pSensorObj;
            pAmbaSensorObj[AMBA_VIN_CHANNEL5] = pSensorObj;
            pAmbaSensorObj[AMBA_VIN_CHANNEL6] = pSensorObj;
            pAmbaSensorObj[AMBA_VIN_CHANNEL7] = pSensorObj;
        } else if ((pChan->VinID == AMBA_VIN_CHANNEL8) || (pChan->VinID == AMBA_VIN_CHANNEL9) || (pChan->VinID == AMBA_VIN_CHANNEL10)) {
            pAmbaSensorObj[AMBA_VIN_CHANNEL8] = pSensorObj;
            pAmbaSensorObj[AMBA_VIN_CHANNEL9] = pSensorObj;
            pAmbaSensorObj[AMBA_VIN_CHANNEL10] = pSensorObj;
        } else if ((pChan->VinID == AMBA_VIN_CHANNEL11) || (pChan->VinID == AMBA_VIN_CHANNEL12) || (pChan->VinID == AMBA_VIN_CHANNEL13)) {
            pAmbaSensorObj[AMBA_VIN_CHANNEL11] = pSensorObj;
            pAmbaSensorObj[AMBA_VIN_CHANNEL12] = pSensorObj;
            pAmbaSensorObj[AMBA_VIN_CHANNEL13] = pSensorObj;
        } else {
            /* do nothing */
        }
    }
}
