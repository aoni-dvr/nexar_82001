/**
 *  @file SvcCvFlowProfile.c
 *
 * Copyright (c) [2020] Ambarella International LP
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
 * This file includes sample code and is only for internal testing and evaluation.  If you
 * distribute this sample code (whether in source, object, or binary code form), it will be
 * without any warranty or indemnity protection from Ambarella International LP or its affiliates.
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
 *  @details Implementation of SVC CVFlow profile
 *
 */

#include "AmbaTypes.h"
#include "AmbaDSP_Liveview.h"
#include "AmbaMisraFix.h"


#include "SvcLog.h"
#include "SvcErrCode.h"

#include "SvcCvFlowProfile.h"

/**
* get content of SvcCvFlow profile
* @param [in] ProfID ID of profile
* @param [out] pProfInfo pointer to content of the profile
* @return 0-OK, 1-NG
*/
UINT32 SvcCvFlow_GetProfile(UINT32 ProfID, const SVC_CV_FLOW_PROF_s **pProfInfo)
{
    UINT32 Rval;
    static SVC_CV_FLOW_PROF_s g_SvcCvfProfile[SVC_CV_FLOW_PROF_ID_NUM] =
    {
        [SVC_CV_FLOW_PROF_FLEXIDAG] = {
            .NumInIF = 0xff,
            .InIF = {
                [0] = { .Interface = SVC_CV_FLOW_INPUT_IF_FLEXIDAG,    .NumFov = 0xff },
            },
            .NumOutIF = 0xff,
            .OutIF = {
                [0] = {.Interface = SVC_CV_FLOW_OUTPUT_IF_FLEXIDAG},
            }
        },
        [SVC_CV_FLOW_PROF_FLEXIDAG_LINUX] = {
            .NumInIF = 0xff,
            .InIF = {
                [0] = { .Interface = SVC_CV_FLOW_INPUT_IF_FLEXIDAG_LINUX,     .NumFov = 0xff },
            },
            .NumOutIF = 0xff,
            .OutIF = {
                [0] = {.Interface = SVC_CV_FLOW_OUTPUT_IF_FLEXIDAG_LINUX},
            }
        }
    };

    AmbaMisra_TouchUnused(&g_SvcCvfProfile);

    if (ProfID < SVC_CV_FLOW_PROF_ID_NUM) {
        *pProfInfo = &g_SvcCvfProfile[ProfID];
        Rval = SVC_OK;
    } else {
        Rval = SVC_NG;
    }

    return Rval;
}

