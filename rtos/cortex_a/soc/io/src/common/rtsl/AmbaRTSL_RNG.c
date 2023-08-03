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

#include "AmbaTypes.h"
#include "AmbaDef.h"

#include "AmbaRTSL_RNG.h"
#include "AmbaCSL_RNG.h"

/**
 *  AmbaRTSL_RngInit - Initialize the Rng module
 */
void AmbaRTSL_RngInit(void)
{
#if defined (CONFIG_SOC_CV2) || defined (CONFIG_SOC_CV22) || defined (CONFIG_SOC_CV25) || defined (CONFIG_SOC_H32)
    AmbaCSL_SetRngMode(0x4);
#elif defined (CONFIG_SOC_CV28)
    // toggle bit 1 and 2.
    AmbaCSL_SetRngMode(0x0e1e);
    AmbaDelayCycles(2400U);
    AmbaCSL_SetRngMode(0x0e18);
#else
    // toggle bit 1 and 2.
    AmbaCSL_SetRngMode(0x0c1e);
    AmbaDelayCycles(2400U);
    AmbaCSL_SetRngMode(0x0c18);
#endif
    AmbaCSL_RngEnable();
}

/**
 *  AmbaRTSL_RngGetValue - Get 32-bit random number
 */
UINT32 AmbaRTSL_RngGetValue(void)
{
    UINT32 data;
#if defined (CONFIG_SOC_CV2)
    AmbaRTSL_RngDataGet(&data, NULL, NULL, NULL);
#else
    AmbaRTSL_RngDataGet(&data, NULL, NULL, NULL, NULL);
#endif
    return data;
}

#if defined (CONFIG_SOC_CV2)
/**
 *  AmbaRTSL_RngGetDataSet - Get 4 32-bit random numbers
 */
void AmbaRTSL_RngDataGet(UINT32 *data0, UINT32 *data1, UINT32 *data2, UINT32 *data3)
#else
/**
 *  AmbaRTSL_RngGetDataSet - Get 5 32-bit random numbers
 */
void AmbaRTSL_RngDataGet(UINT32 *data0, UINT32 *data1, UINT32 *data2, UINT32 *data3, UINT32 *data4)
#endif
{
    AmbaCSL_RngCleanStatus();
    AmbaCSL_RngStart();

    while (AmbaCSL_RngIsBusy() != 1U) {
        AmbaDelayCycles(2400U);
    }

    // data0 won't be null
    *data0 = AmbaCSL_RngGetData0();

    if (data1 != NULL) {
        *data1 = AmbaCSL_RngGetData1();
    }
    if (data2 != NULL) {
        *data2 = AmbaCSL_RngGetData2();
    }
    if (data3 != NULL) {
        *data3 = AmbaCSL_RngGetData3();
    }
#if !defined (CONFIG_SOC_CV2)
    if (data4 != NULL) {
        *data4 = AmbaCSL_RngGetData4();
    }
#endif
}

/**
 *  AmbaRTSL_RngInit - De-Initialize the Rng module
 */
void AmbaRTSL_RngDeinit(void)
{
    AmbaCSL_RngDisable();
}
