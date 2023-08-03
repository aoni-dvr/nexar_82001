/**
 *  @file SvcCvFlow_RefODTable.c
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
 *  @details Table of MEMIO based Ambarella Object Detection
 *
 */

#include "AmbaTypes.h"
#include "AmbaDSP_Liveview.h"

#include "SvcCvAlgo.h"
#include "cvapi_svccvalgo_openod.h"
#include "cvapi_svccvalgo_linux.h"

#include "SvcCvFlowProfile.h"
#include "SvcResCfg.h"
#include "SvcCvFlow.h"
#include "SvcCvFlowAlgoUtil.h"
#include "SvcCvFlow_Comm.h"
#include "SvcCvFlow_RefOD.h"
#include "SvcCvFlow_Rebel_RefOD.h"

const SVC_CV_FLOW_REFOD_MODE_INFO_s SvcCvFlow_RefOD_ModeInfo[REBEL_NUM_MODE_OD] = {
    [REBEL_FDAG_LINUX_OPENOD] = {
        .ProfID = SVC_CV_FLOW_PROF_FLEXIDAG_LINUX,
        .AlgoGrp = {
            .NumAlgo = 1,
            .AlgoInfo = {
                [0] = {
#ifdef CONFIG_AMBALINK_BOOT_OS
                    .pAlgoObj = &LinuxAlgoObj,
#else
                    .pAlgoObj = NULL,
#endif
                    .NumFlexiBin = 0, .FlexiBinBits = 0x0,
                    .NumLcsBin = 0,
                    .NumExtBin = 0,
                }
            }
        },
        .Cfg = {
            [0] = {
                .NetWidth = REFOD_OPEN_NET_WIDTH, .NetHeight = REFOD_OPEN_NET_HEIGHT,
                .pNmsCfg = {
                    NULL,
                }
            }
        },
        .OutputType = REBEL_OUTPUT_TYPE_DETBOX,
        .OutputBufIdx = 0,
    },
};

