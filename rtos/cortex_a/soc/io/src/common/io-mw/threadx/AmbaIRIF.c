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
#include "AmbaRTSL_IRIF.h"
#include "AmbaRTSL_PLL.h"

static void IRIF_IntHandler(void);

static AMBA_KAL_MUTEX_t AmbaIrMutex;
static AMBA_KAL_EVENT_FLAG_t AmbaIrEventFlag;
static UINT32 AmbaIrIntHandlerErr;

/**
 *  AmbaIRIF_DrvEntry - Initializes driver-wide data structures and resources
 *  @return error code
 *  @note this function is an internal using only API
 */
UINT32 AmbaIRIF_DrvEntry(void)
{
    static char AmbaIrEventFlagName[18] = "AmbaIrEventFlags";
    static char AmbaIrMutexName[16] = "AmbaIrMutex";
    UINT32 RetVal = IRIF_ERR_NONE;

    /* Create Mutex */
    if (AmbaKAL_MutexCreate(&AmbaIrMutex, AmbaIrMutexName) != KAL_ERR_NONE) {
        RetVal = IRIF_ERR_UNEXPECTED;
    }

    /* Create EventFlags */
    if (RetVal == IRIF_ERR_NONE) {
        if (AmbaKAL_EventFlagCreate(&AmbaIrEventFlag, AmbaIrEventFlagName) != KAL_ERR_NONE) {
            RetVal = IRIF_ERR_UNEXPECTED;
        }
    }

    /* Initialize hardware module */
    if (RetVal == IRIF_ERR_NONE) {
        (void)AmbaRTSL_IrInit();
        (void)AmbaRTSL_IrIntHookHandler(IRIF_IntHandler);
    }

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

    if (AmbaKAL_MutexTake(&AmbaIrMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
        RetVal = IRIF_ERR_MUTEX;
    } else {
        RetVal = AmbaRTSL_IrStart(SampleFreq);

        if (AmbaKAL_MutexGive(&AmbaIrMutex) != KAL_ERR_NONE) {
            /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
            RetVal = IRIF_ERR_UNEXPECTED;
        }
    }

    return RetVal;
}

/**
 *  AmbaIRIF_Stop - Stop IR interface
 *  @return error code
 */
UINT32 AmbaIRIF_Stop(void)
{
    UINT32 RetVal = IRIF_ERR_NONE;

    if (AmbaKAL_MutexTake(&AmbaIrMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
        RetVal = IRIF_ERR_MUTEX;
    } else {
        RetVal = AmbaRTSL_IrStop();

        if (AmbaKAL_MutexGive(&AmbaIrMutex) != KAL_ERR_NONE) {
            /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
            RetVal = IRIF_ERR_UNEXPECTED;
        }
    }

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
    UINT32 ActualFlags, KalRetVal, RetVal = IRIF_ERR_NONE;
    UINT32 i = 0U, NumReadData = 0U, NumRemainData = NumData;

    if (AmbaKAL_MutexTake(&AmbaIrMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
        RetVal = IRIF_ERR_MUTEX;
    } else {
        if (AmbaKAL_EventFlagClear(&AmbaIrEventFlag, (UINT32)1U) != KAL_ERR_NONE) {
            RetVal = IRIF_ERR_UNEXPECTED;
        } else {
            AmbaIrIntHandlerErr = IRIF_ERR_NONE;    /* clear error status reported by interrupt handler */
            RetVal = AmbaRTSL_IrRead(NumRemainData, pDataBuf, &NumReadData);
            if (RetVal == IRIF_ERR_NONE) {
                i += NumReadData;
                NumRemainData -= NumReadData;
                while (NumRemainData != 0U) {
                    KalRetVal = AmbaKAL_EventFlagGet(&AmbaIrEventFlag, (UINT32)1U, KAL_FLAGS_WAIT_ALL, KAL_FLAGS_CLEAR_AUTO, &ActualFlags, TimeOut);
                    if (KalRetVal != KAL_ERR_NONE) {
                        if (KalRetVal == KAL_ERR_TIMEOUT) {
                            RetVal = IRIF_ERR_TIMEOUT;
                        } else {
                            RetVal = IRIF_ERR_UNEXPECTED;
                        }
                    }

                    if (RetVal != IRIF_ERR_NONE) {
                        (void)AmbaRTSL_IrRead(NumRemainData, &pDataBuf[i], &NumReadData);
                        i += NumReadData;
                        break;
                    } else {
                        (void)AmbaRTSL_IrRead(NumRemainData, &pDataBuf[i], &NumReadData);
                        i += NumReadData;
                        NumRemainData -= NumReadData;
                    }
                }

                if (pActualNumData != NULL) {
                    *pActualNumData = i;
                }
            }

            if (AmbaIrIntHandlerErr != IRIF_ERR_NONE) { /* check if any error reported in interrupt handler */
                RetVal = AmbaIrIntHandlerErr;
            }

        }

        if (AmbaKAL_MutexGive(&AmbaIrMutex) != KAL_ERR_NONE) {
            /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
            RetVal = IRIF_ERR_UNEXPECTED;
        }
    }

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

    if (pActualSampleFreq == NULL) {
        RetVal = IRIF_ERR_ARG;
    } else {
        *pActualSampleFreq = AmbaRTSL_PllGetIrClk();
    }

    return RetVal;
}

/**
 *  IRIF_IntHandler - ISR for IR event
 */
static void IRIF_IntHandler(void)
{
    if (AmbaKAL_EventFlagSet(&AmbaIrEventFlag, (UINT32)1U) != KAL_ERR_NONE) {
        AmbaIrIntHandlerErr = IRIF_ERR_UNEXPECTED;
    }
}
