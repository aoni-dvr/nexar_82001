/**
 *  @file AmbaDSP_TimeManager.c
 *
 * Copyright (c) 2022 Ambarella International LP
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
 *
 *  @details Implementation of SSP internal time-tick manager
 *
 */

#include "AmbaDSP_TimeManager.h"
#include "dsp_osal.h"

static osal_mutex_t DspTimeMtx GNU_SECTION_NOZEROINIT;

#define DSP_TIME_MTX_TIMEOUT_MS (3000U)
#define ROUNDING_THREASHOLD     (0x80000000U) //2^31, 174sec when AudioTick is 12.288MHz

typedef struct {
    UINT64 Base;
    UINT64 Current;
} DSP_TIME_s;

static DSP_TIME_s DspTime[NUM_DSP_TIME_ID];

static inline void HL_TimeMngrInit(void)
{
    static UINT8 TimeMngrInit = 0U;
    UINT32 Rval = DSP_ERR_NONE;

    if (TimeMngrInit == 0U) {
        static char DspTimeMtxName[] = "DspTimeMtx";

        Rval = dsp_osal_mutex_init(&DspTimeMtx, DspTimeMtxName);
        if (Rval != DSP_ERR_NONE) {
            dsp_osal_printU5("[ERR] DspTimeMtx init fail[0x%X]", Rval, 0U, 0U, 0U, 0U);
        }

        if (Rval == DSP_ERR_NONE) {
            (void)dsp_osal_memset(DspTime, 0, sizeof(DSP_TIME_s)*NUM_DSP_TIME_ID);
        }

        TimeMngrInit = 1U;
    }
}

UINT32 HL_TimeMngrSetTime(UINT32 TimeId, UINT64 TimeVal)
{
    UINT32 Rval = DSP_ERR_NONE;

    HL_TimeMngrInit();

    if (TimeId == DSP_TIME_ID_AUDIO_TICK) {
        UINT32 TimeLsb32 = (UINT32)TimeVal;

        Rval = dsp_osal_mutex_lock(&DspTimeMtx, DSP_TIME_MTX_TIMEOUT_MS);
        if (Rval == DSP_ERR_NONE) {
            if ((DspTime[TimeId].Base == 0ULL) &&
                (DspTime[TimeId].Current == 0ULL)) {
                // First one, direct use it
                DspTime[TimeId].Current = TimeLsb32;
            } else {
                if (TimeLsb32 < (UINT32)DspTime[TimeId].Current) {
                    if ((DspTime[TimeId].Current - TimeLsb32) > ROUNDING_THREASHOLD) {
                        DspTime[TimeId].Base += 0x100000000ULL;
//dsp_osal_printU5("TmMngr SetTime Base++ 0x%X", DspTime[TimeId].Base>>32U, 0, 0U, 0U, 0U);
                    }
                }
                DspTime[TimeId].Current = TimeLsb32;
            }
            Rval = dsp_osal_mutex_unlock(&DspTimeMtx);
        }
    } else {
        // DO NOTHING
    }

    return Rval;
}

UINT32 HL_TimeMngrGetTime(UINT32 TimeId, UINT64 InTimeVal, UINT64 *pOutTimeVal)
{
    UINT32 Rval = DSP_ERR_NONE;

    HL_TimeMngrInit();

    if (TimeId == DSP_TIME_ID_AUDIO_TICK) {
        UINT32 InTimeLsb32 = (UINT32)InTimeVal;

        *pOutTimeVal = 0ULL;

        Rval = dsp_osal_mutex_lock(&DspTimeMtx, DSP_TIME_MTX_TIMEOUT_MS);
        if (Rval == DSP_ERR_NONE) {
            *pOutTimeVal = InTimeLsb32;
            *pOutTimeVal |= DspTime[TimeId].Base;
            if (InTimeLsb32 < DspTime[TimeId].Current) {
                if ((DspTime[TimeId].Current - InTimeLsb32) > ROUNDING_THREASHOLD) {
                    *pOutTimeVal += 0x100000000ULL;
                }
            } else if (InTimeLsb32 > DspTime[TimeId].Current) {
                if ((InTimeLsb32 - DspTime[TimeId].Current) > ROUNDING_THREASHOLD) {
                    if (DspTime[TimeId].Base >= 0x100000000ULL) {
                        *pOutTimeVal -= 0x100000000ULL;
                    }
                }
            } else {
                // Direct use it
            }
            Rval = dsp_osal_mutex_unlock(&DspTimeMtx);
        }
    } else {
        // DO NOTHING
    }

    return Rval;
}
