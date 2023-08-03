/**
 *  @file SvcCvFlow_Fex.h
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
 *  @details Header of FlexiDAG based Ambarella Feature Extraction
 *
 */

#ifndef SVC_CV_FLOW_FEX_H
#define SVC_CV_FLOW_FEX_H

#if defined (CONFIG_SOC_CV2FS)
#include "cvapi_flexidag_ambaspufex_cv2fs.h"
#else
#include "cvapi_flexidag_ambaspufex_cv2.h"
#endif

/* AmbaFex mode */
#define FEX_MODE_AMBA_FEX                       (0U)
#define FEX_NUM_MODE                            (1U)

typedef struct {
    UINT8   Reserved;
} SVC_CV_FLOW_FEX_CFG_s;

#define FEX_MAX_ALGO_NUM                        (1U)
typedef struct {
    UINT32  ProfID;
    SVC_CV_FLOW_ALGO_GROUP_s     AlgoGrp;
    UINT8   OutputType;
    UINT8   OutputBufIdx;
    SVC_CV_FLOW_FEX_CFG_s        Cfg[FEX_MAX_ALGO_NUM];
} SVC_CV_FLOW_FEX_MODE_INFO_s;

typedef struct {
    UINT32 Msg;
    UINT32 FrameNum;
    UINT32 CaptureTime;
    AMBA_CV_FEX_DATA_s          *pFexOut;
} SVC_CV_FLOW_FEX_OUTPUT_s;

/*-----------------------------------------------------------------------------------------------*\
 *  Defined in SvcCvFlow_AmbaFex.c
\*-----------------------------------------------------------------------------------------------*/
extern SVC_CV_FLOW_OBJ_s SvcCvFlow_FexObj;

extern const SVC_CV_FLOW_FEX_MODE_INFO_s SvcCvFlow_Fex_ModeInfo[FEX_NUM_MODE];

#endif /* SVC_CV_FLOW_FEX_H */
