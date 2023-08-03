/**
 *  @file AmbaIRIF.c
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
 *  @details InfraRed Interface APIs
 *
 */

#include "AmbaTypes.h"
#include "AmbaDef.h"
#include "AmbaKAL.h"

#include "AmbaDrvEntry.h"
#include "AmbaIRIF.h"
//#include "AmbaRTSL_IRIF.h"
//#include "AmbaRTSL_PLL.h"

/**
 *  AmbaIRIF_DrvEntry - Initializes driver-wide data structures and resources
 *  @return error code
 *  @note this function is an internal using only API
 */
UINT32 AmbaIRIF_DrvEntry(void)
{
    UINT32 RetVal = IRIF_ERR_NONE;
    return RetVal;
}

/**
 *  AmbaIRIF_Start - Start IR interface
 *  @param[in] SampleFreq: Sampling frequency (Hz).
 *  @return error code
 */
UINT32 AmbaIRIF_Start(UINT32 SampleFreq)
{
    UINT32 RetVal = IRIF_ERR_NONE;
    (void) SampleFreq;
    return RetVal;
}

/**
 *  AmbaIRIF_Stop - Stop IR interface
 *  @return error code
 */
UINT32 AmbaIRIF_Stop(void)
{
    UINT32 RetVal = IRIF_ERR_NONE;
    return RetVal;
}

/**
 *  AmbaIRIF_Read - Get IR data
 *  @param[in] NumData Maximum number of data to be read
 *  @param[out] pDataBuf pointer to the data buffer
 *  @param[out] pActualNumData number of items successfully read before timeout
 *  @param[in] TimeOut Timeout: Timeout value in ms (millisecond)
 *  @return error code
 */
UINT32 AmbaIRIF_Read(UINT32 NumData, UINT16 *pDataBuf, UINT32 *pActualNumData, UINT32 TimeOut)
{
    UINT32 RetVal = IRIF_ERR_NONE;
    (void) NumData;
    (void) pDataBuf;
    (void) pActualNumData;
    (void) TimeOut;
    return RetVal;
}

/**
 *  AmbaIRIF_GetInfo - Get IR info
 *  @param[out] pActualSampleFreq actual sampling frequency
 *  @return error code
 */
UINT32 AmbaIRIF_GetInfo(UINT32 *pActualSampleFreq)
{
    UINT32 RetVal = IRIF_ERR_NONE;
    (void) pActualSampleFreq;
    return RetVal;
}

