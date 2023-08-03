/*
 * Copyright (c) 2021 Ambarella International LP
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

#define VOID     void

#include "AmbaKAL.h"
#include "AmbaTypes.h"
#include "AmbaRNG.h"

#define FLAG_RNG_API_FLOW_UNINIT    0x0000U
#define FLAG_RNG_API_FLOW_START     0x0001U

static UINT32  flag_rng_api= FLAG_RNG_API_FLOW_UNINIT;

VOID AmbaRNG_GetValue(UINT32 *random)
{
    *random = 0xFFFFFFFF;
}

UINT32 AmbaRNG_Init(VOID)
{
    UINT32 Ret;
    if ((flag_rng_api & FLAG_RNG_API_FLOW_START) == 0U) {
        // TODO
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

    if (data0 == NULL) {
        Ret = RNG_ERR_ARG;
    } else {
        if ((flag_rng_api & FLAG_RNG_API_FLOW_START) != 0U) {
            // TODO
            *data0 = 0xFFFFFFFF;
            *data1 = 0xFFFFFFFF;
            *data2 = 0xFFFFFFFF;
            *data3 = 0xFFFFFFFF;
            *data4 = 0xFFFFFFFF;

            Ret = RNG_ERR_NONE;
        } else {
            Ret = RNG_ERR_FLOW;
        }
    }

    return Ret;
}

UINT32 AmbaRNG_Deinit(void)
{
    UINT32 Ret;

    if ((flag_rng_api & FLAG_RNG_API_FLOW_START) != 0U) {
        // TODO
        flag_rng_api &= ~FLAG_RNG_API_FLOW_START;
        Ret = RNG_ERR_NONE;
    } else {
        Ret = RNG_ERR_FLOW;
    }

    return Ret;
}
