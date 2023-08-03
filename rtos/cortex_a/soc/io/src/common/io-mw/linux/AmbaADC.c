/**
 *  @file AmbaADC.c
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
 *  @details ADC APIs
 *
 */

#include "AmbaTypes.h"
#include "AmbaDef.h"
#include "AmbaKAL.h"

#include "AmbaDrvEntry.h"
#include "AmbaADC.h"
//#include "AmbaRTSL_ADC.h"

/**
 *  AmbaADC_DrvEntry - Initializes driver-wide data structures and resources
 *  @return error code
 *  @note this function is an internal using only API
 */
UINT32 AmbaADC_DrvEntry(void)
{
    UINT32 RetVal = ADC_ERR_NONE;
    return RetVal;
}

/**
 *  AmbaADC_Config - Configure the ADC module to target sample rate
 *  @param[in] SampleRate the value of target sample
 *  @return error code
 */
UINT32 AmbaADC_Config(UINT32 SampleRate)
{
    UINT32 RetVal = 1U;
    (void) SampleRate;
    return RetVal;
}

/**
 *  AmbaADC_SeamlessRead - collection ADC data from the specified ADC channel
 *  @param[in] AdcCh ADC channel number
 *  @param[in] BufSize buffer size
 *  @param[in] pBuf pointer to collection buffer
 *  @param[in] pActualSize number of the actual converted ADC values
 *  @param[in] TimeOut Timeout value
 *  @return error code
 */
UINT32 AmbaADC_SeamlessRead(UINT32 AdcCh, UINT32 BufSize, UINT32 * const pBuf, UINT32 * pActualSize, UINT32 TimeOut)
{
    UINT32 RetVal = ADC_ERR_NONE;
    (void) AdcCh;
    (void) BufSize;
    (void) pBuf;
    (void) pActualSize;
    (void) TimeOut;
    return RetVal;
}

/**
 *  AmbaADC_SingleRead - get an analog value from the specified channel
 *  @param[in] AdcCh ADC channel number
 *  @param[out] pData pointer to store output data
 *  @return error code
 */
UINT32 AmbaADC_SingleRead(UINT32 AdcCh, UINT32 *pData)
{
    UINT32 RetVal = ADC_ERR_NONE;
    (void) AdcCh;
    (void) pData;
    return RetVal;
}

/**
 *  AmbaADC_GetInfo - The function returns the time period necessary for performing each AD conversion and which channels are under continuous AD conversion.
 *  @param[out] pSampleRate pointer to store current sample rate
 *  @param[out] pActiveChanList pointer to store bit flag of active channels (EX. 0x3 means channel0/1 are active)
 *  @return error code
 */
UINT32 AmbaADC_GetInfo(UINT32 *pSampleRate, UINT32 *pActiveChanList)
{
    UINT32 RetVal = OK;
    (void) pSampleRate;
    (void) pActiveChanList;
    return RetVal;
}

