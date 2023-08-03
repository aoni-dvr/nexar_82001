/**
 *  @file AmbaFB_Utility.c
 *
 *  @copyright Copyright (c) 2018 Ambarella, Inc.
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
 *  @details Utilities for FB profiling.
 *
 */


#if defined(CONFIG_CC_ENABLE_INSTRUMENT_FUNC)

#include "AmbaTypes.h"

/*
 * This could not be used for a mixed 64/32 bits system.
 * Currently, we are focusing on SYS part, so we just care about the 32 bits flow
 */

extern void AmbaRTSL_CacheCleanDataCacheRange(void *pAddr, UINT32 Size);

#define FUNC_LOG_MAX    (512U << 10)
#define REGION_AMOUNT   (7U)

static UINT32 function_ivk[REGION_AMOUNT][FUNC_LOG_MAX] = { 0 };
static UINT32 func_ivk_cnt[REGION_AMOUNT] = { 0 };
static UINT32 func_ivk_idx = 0;

UINT32 *AmbaUser_GetFuncIvkBuf(UINT32 idx)
{
    return &(function_ivk[idx][0]);
}

UINT32 AmbaUser_GetFuncIvkCnt(UINT32 idx)
{
    return func_ivk_cnt[idx];
}

void AmbaUser_SetFuncIvkIdx(UINT32 idx)
{
    func_ivk_idx = idx;
}

inline
void __attribute__((__no_instrument_function__))
__cyg_profile_func_enter (void *func,  void *caller)
{
    UINT32 i, k;

    (void)caller;

    if (func_ivk_idx == (REGION_AMOUNT - 1U)) {
        return;
    }

    for (i = 0; i < FUNC_LOG_MAX; i++) {
        if (function_ivk[func_ivk_idx][i] == (UINT32)func) {
            break;
        } else if (function_ivk[func_ivk_idx][i] == 0U) {
            k = __sync_fetch_and_add(&func_ivk_cnt[func_ivk_idx], 1);
            function_ivk[func_ivk_idx][k] = (UINT32)func;
//            AmbaRTSL_CacheCleanDataCacheRange((void *) &function_ivk[func_ivk_idx][k], sizeof(UINT32));
            break;
        }
    }
}

inline
void __attribute__((__no_instrument_function__))
__cyg_profile_func_exit (void *func, void *caller)
{
    (void)func;
    (void)caller;
}

#endif

