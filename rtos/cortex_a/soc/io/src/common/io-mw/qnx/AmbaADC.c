/*
 * Copyright (c) 2020 Ambarella International LP
 *
 * This file and its contents ("Software") are protected by intellectual
 * property rights including, without limitation, U.S. and/or foreign
 * copyrights. This Software is also the confidential and proprietary
 * information of Ambarella International LP and its licensors. You may not use, reproduce,
 * disclose, distribute, modify, or otherwise prepare derivative works of this
 * Software or any portion thereof except pursuant to a signed license agreement
 * or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
 * In the absence of such an agreement, you agree to promptly notify and return
 * this Software to Ambarella International LP.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 * MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "hw/ambarella_adc.h"
#include <sys/types.h>
#include "Generic.h"

#include "AmbaTypes.h"
#include "AmbaDef.h"
#include "AmbaKAL.h"
#include "AmbaADC.h"

//static void ADC_IntHandler(UINT32 EventData);

static AMBA_KAL_EVENT_FLAG_t AmbaAdcEventFlag;
static AMBA_KAL_MUTEX_t AmbaAdcMutex;

/**
 *  AmbaADC_DrvEntry - Initializes driver-wide data structures and resources
 *  @return error code
 *  @note this function is an internal using only API
 */
UINT32 AmbaADC_DrvEntry(void)
{
    static char AmbaAdcEventFlagName[18] = "AmbaAdcEventFlags";
    static char AmbaAdcMutexName[16] = "AmbaAdcMutex";
    UINT32 RetVal = ADC_ERR_NONE;

    if (AmbaKAL_MutexCreate(&AmbaAdcMutex, AmbaAdcMutexName) != KAL_ERR_NONE) {
        RetVal = ADC_ERR_UNEXPECTED;
    }

    if (RetVal == ADC_ERR_NONE) {
        if (AmbaKAL_EventFlagCreate(&AmbaAdcEventFlag, AmbaAdcEventFlagName) != KAL_ERR_NONE) {
            RetVal = ADC_ERR_UNEXPECTED;
        }
    }

    return RetVal;
}

/**
 *  AmbaADC_Config - Configure the ADC module to target sample rate
 *  @param[in] SampleRate the value of target sample
 *  @return error code
 */
UINT32 AmbaADC_Config(UINT32 SampleRate)
{
    UINT32 RetVal = ADC_ERR_NONE;
    int fd, err = 0;
    adc_ch_t adcCtrl;

    if (SampleRate == 0x0U) {
        RetVal = ADC_ERR_ARG;
    } else {
        fd = open("/dev/adc", O_RDWR);
        if (fd == -1) {
            RetVal = ADC_ERR_UNEXPECTED;
        } else {
            adcCtrl.SampleRate = SampleRate;

            err = devctl(fd, DCMD_ADC_SET_SAMPLERATE, &adcCtrl, sizeof(adc_ch_t), NULL);
            if (err) {
                RetVal = ADC_ERR_UNEXPECTED;
            }

            close(fd);
        }
    }

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
    UINT32 ActualFlags, ActualSize = 0U;
    UINT32 KalRetVal, RetVal = ADC_ERR_NONE;
    int fd, err = 0;
    adc_ch_t adcCtrl;

    if ((AdcCh >= AMBA_NUM_ADC_CHANNEL) || (BufSize == 0x0U) || (pBuf == NULL)) {
        RetVal = ADC_ERR_ARG;
    } else {
        if (AmbaKAL_MutexTake(&AmbaAdcMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = ADC_ERR_MUTEX;
        } else {
            if (AmbaKAL_EventFlagClear(&AmbaAdcEventFlag, 0x1U) != KAL_ERR_NONE) {
                RetVal = ADC_ERR_UNEXPECTED;
            } else {
                fd = open("/dev/adc", O_RDWR);
                if (fd == -1) {
                    RetVal = ADC_ERR_UNEXPECTED;
                } else {
                    adcCtrl.AdcCh = AdcCh;
                    adcCtrl.BufSize = BufSize;
                    adcCtrl.pBuffer = pBuf;

                    err = devctl(fd, DCMD_ADC_SEAMLESS_READ, &adcCtrl, sizeof(adc_ch_t), NULL);
                    if (err) {
                        RetVal = ADC_ERR_UNEXPECTED;
                    }

                    close(fd);
                }

                if (RetVal == OK) {
                    KalRetVal = AmbaKAL_EventFlagGet(&AmbaAdcEventFlag, 0x1U, KAL_FLAGS_WAIT_ALL, KAL_FLAGS_CLEAR_AUTO, &ActualFlags, TimeOut);
                    if (KalRetVal != KAL_ERR_NONE) {
                        if (KalRetVal == KAL_ERR_TIMEOUT) {
                            RetVal = ADC_ERR_TIMEOUT;
                        } else {
                            RetVal = ADC_ERR_UNEXPECTED;
                        }
                    }

                    /* stop ADC sampling */
                    fd = open("/dev/adc", O_RDWR);
                    if (fd == -1) {
                        RetVal = ADC_ERR_UNEXPECTED;
                    } else {
                        adcCtrl.pActualSize = &ActualSize;

                        err = devctl(fd, DCMD_ADC_SEAMLESS_READ, &adcCtrl, sizeof(adc_ch_t), NULL);
                        if (err) {
                            RetVal = ADC_ERR_UNEXPECTED;
                        }

                        close(fd);
                    }
                }

                if (pActualSize != NULL) {
                    *pActualSize = ActualSize;
                }
            }

            if (AmbaKAL_MutexGive(&AmbaAdcMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = ADC_ERR_UNEXPECTED;
            }
        }
    }

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
    int fd, err = 0;
    adc_ch_t adcCtrl;
    UINT32 RetVal = ADC_ERR_NONE;

    if ((AdcCh >= AMBA_NUM_ADC_CHANNEL) || (pData == NULL)) {
        RetVal = ADC_ERR_ARG;
    } else {
        if (AmbaKAL_MutexTake(&AmbaAdcMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = ADC_ERR_MUTEX;
        } else {
            fd = open("/dev/adc", O_RDWR);
            if (fd == -1) {
                RetVal = ADC_ERR_UNEXPECTED;
            } else {
                adcCtrl.AdcCh = AdcCh;

                err = devctl(fd, DCMD_ADC_GET_SINGLEREAD, &adcCtrl, sizeof(adc_ch_t), NULL);
                if (err) {
                    RetVal = ADC_ERR_UNEXPECTED;
                }

                *pData = adcCtrl.ChData;

                close(fd);
            }

            if (AmbaKAL_MutexGive(&AmbaAdcMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = ADC_ERR_UNEXPECTED;
            }
        }
    }

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
    int fd, err = 0;
    adc_ch_t adcCtrl;
    UINT32 RetVal = ADC_ERR_NONE;

    if ((pSampleRate == NULL) || (pActiveChanList == NULL)) {
        RetVal = ADC_ERR_ARG;
    } else {
        fd = open("/dev/adc", O_RDWR);
        if (fd == -1) {
            RetVal = ADC_ERR_UNEXPECTED;
        } else {

            err = devctl(fd, DCMD_ADC_GET_INFO, &adcCtrl, sizeof(adc_ch_t), NULL);
            if (err) {
                RetVal = ADC_ERR_UNEXPECTED;
            } else {
                *pSampleRate = adcCtrl.SampleRate;
                *pActiveChanList = adcCtrl.ActiveChanList;
            }

            close(fd);
        }
    }

    return RetVal;
}

#if 0
/**
 *  ADC_IntHandler - ADC data ready interrupt handler.
 *  @param[in] EventData number of collected adc values
 */
static void ADC_IntHandler(UINT32 EventData)
{
    /* increase the counting semaphore */
    if (EventData != 0U) {
        if (AmbaKAL_EventFlagSet(&AmbaAdcEventFlag, 0x1U) != KAL_ERR_NONE) {
            /* Trap here because there is no way to notify application. */
            AmbaAssert();
        }
    }
}
#endif