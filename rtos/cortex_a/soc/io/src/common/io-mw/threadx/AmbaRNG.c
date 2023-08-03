/**
 *  @file AmbaRNG.c
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
 *  @details random number generator (RNG) APIs
 *
 */

#include "AmbaTypes.h"
#include "AmbaKAL.h"
#include "AmbaRNG.h"
#include "AmbaRTSL_RNG.h"
#if defined (CONFIG_SOC_CV2)
#include <AmbaMisraFix.h>
#endif

#define FLAG_RNG_API_FLOW_UNINIT    0x0000U
#define FLAG_RNG_API_FLOW_START     0x0001U

static UINT32  flag_rng_api= FLAG_RNG_API_FLOW_UNINIT;

UINT32 AmbaRNG_Init(VOID)
{
    UINT32 Ret;

    if ((flag_rng_api & FLAG_RNG_API_FLOW_START) == 0U) {
        AmbaRTSL_RngInit();
        flag_rng_api |= FLAG_RNG_API_FLOW_START;
        Ret = RNG_ERR_NONE;
    } else {
        Ret = RNG_ERR_FLOW;
    }

    return Ret;
}

UINT32 AmbaRNG_DataGet(UINT32 *data0, UINT32 *data1, UINT32 *data2, UINT32 *data3, UINT32 *data4)
{
    UINT32 Ret;
#if defined (CONFIG_SOC_CV2)
    // pass misra check
    AmbaMisra_TouchUnused(data4);
#endif

    if (data0 == NULL) {
        Ret = RNG_ERR_ARG;
#if defined (CONFIG_SOC_CV2)
    } else if (data4 != NULL) { // cv2 only support at most 4 outputs, so data4 should be null from the caller.
        Ret = RNG_ERR_ARG;
#endif
    } else {
        if ((flag_rng_api & FLAG_RNG_API_FLOW_START) != 0U) {
#if defined (CONFIG_SOC_CV2)
            if ((data1 == NULL) && (data2 == NULL) && (data3 == NULL)) {
#else
            if ((data1 == NULL) && (data2 == NULL) && (data3 == NULL) && (data4 == NULL)) {
#endif
                // only request one number.
                *data0 = AmbaRTSL_RngGetValue();
            } else {
#if defined (CONFIG_SOC_CV2)
                AmbaRTSL_RngDataGet(data0, data1, data2, data3);
#else
                AmbaRTSL_RngDataGet(data0, data1, data2, data3, data4);
#endif
            }
            Ret = RNG_ERR_NONE;
        } else {
            Ret = RNG_ERR_FLOW;
        }
    }

    return Ret;
}

UINT32 AmbaRNG_Deinit(VOID)
{
    UINT32 Ret;

    if ((flag_rng_api & FLAG_RNG_API_FLOW_START) != 0U) {
        AmbaRTSL_RngDeinit();
        flag_rng_api &= ~FLAG_RNG_API_FLOW_START;
        Ret = RNG_ERR_NONE;
    } else {
        Ret = RNG_ERR_FLOW;
    }

    return Ret;
}
