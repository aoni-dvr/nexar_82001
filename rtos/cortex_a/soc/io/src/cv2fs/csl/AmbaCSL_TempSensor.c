/**
 *  @file AmbaCSL_TempSensor.c
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
 *  @details Chip Support Library (CSL) for Temperature Sensor
 *
 */

#include "AmbaTypes.h"
#include "AmbaMisraFix.h"

#include "AmbaCSL_TempSensor.h"

/**
 *  AmbaCSL_TempSensor0SetCtrl - Set PLL Control register values
 */
void AmbaCSL_TempSensor0SetCtrl(UINT32 Channel, UINT32 EnableDOC)
{
    UINT32 ProbSetting[5U] = { 0U };

    ProbSetting[1U] = 0x11111111U;
    ProbSetting[2U] = 0x22222222U;
    ProbSetting[3U] = 0x33333333U;
    ProbSetting[4U] = 0x44444444U;

    if(EnableDOC == 0U) {
        /* Sensor0 in non-DOC mode  */
        AmbaCSL_Ts0Ctrl0(0x5U);
        AmbaCSL_Ts0SampleNum(0xFU);
    } else {
        /* Sensor0 in DOC mode  */
        AmbaCSL_Ts0Ctrl0(0x630005U);
        AmbaCSL_Ts0SampleNum(0xFU);
    }

    AmbaCSL_Ts0SetProbe0(ProbSetting[Channel]);
    AmbaCSL_Ts0SetProbe1(ProbSetting[Channel]);
}

/**
 *  AmbaCSL_TempSensor1SetCtrl - Set PLL Control register values
 */
void AmbaCSL_TempSensor1SetCtrl(UINT32 Channel, UINT32 EnableDOC)
{
    UINT32 ProbSetting[5U] = { 0U };

    ProbSetting[1U] = 0x11111111U;
    ProbSetting[2U] = 0x22222222U;
    ProbSetting[3U] = 0x33333333U;
    ProbSetting[4U] = 0x44444444U;

    if(EnableDOC == 0U) {
        /* Sensor1 in non-DOC mode  */
        AmbaCSL_Ts1Ctrl0(0x5U);
        AmbaCSL_Ts1SampleNum(0xFU);
    } else {
        /* Sensor1 in DOC mode */
        AmbaCSL_Ts1Ctrl0(0x630005U);
        AmbaCSL_Ts1SampleNum(0xFU);
    }
    AmbaCSL_Ts1SetProbe0(ProbSetting[Channel]);
    AmbaCSL_Ts1SetProbe1(ProbSetting[Channel]);
}

