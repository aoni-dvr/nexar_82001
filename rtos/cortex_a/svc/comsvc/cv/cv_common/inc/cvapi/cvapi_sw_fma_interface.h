/**
 *  @file AmbaFmaIF.h
 *
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
 */
#ifndef AMBA_FMA_IF_H
#define AMBA_FMA_IF_H

#include "cvapi_flexidag.h"
#if defined(CONFIG_SOC_CV2)
#include "cvapi_flexidag_ambafma_cv2.h"
#elif defined(CONFIG_SOC_CV2FS)
#include "cvapi_flexidag_ambaspufex_def_cv2fs.h"
#else
#include "cvapi_flexidag_ambaspufex_def_sw.h"
#endif

typedef UINT8 AMBA_FMA_HANDLE_s[6272];

typedef struct {
    UINT8 MatchAlgo;
    UINT8 DistTh;
}  AMBA_FMA_CFG_s;


typedef struct {
    UINT16 Idx[2048];
    UINT8  MinDist[2048];
}  AMBA_FMA_RESULT_s;

UINT32 AmbaSWFma_Cfg(const AMBA_FMA_HANDLE_s *pHandler, const AMBA_FMA_CFG_s *pFmaCfg);
UINT32 AmbaSWFma_Init(const AMBA_FMA_HANDLE_s *pHandler);
UINT32 AmbaSWFma_Process(const AMBA_FMA_HANDLE_s *pHandler, UINT8 *pDescriptors);
UINT32 AmbaSWFma_GetResult(const AMBA_FMA_HANDLE_s *pHandler, AMBA_FMA_RESULT_s *pFmaResult);
UINT32 AmbaSWFma_Open(const AMBA_FMA_HANDLE_s *pHandler);
UINT32 AmbaSWFma_Close(const AMBA_FMA_HANDLE_s *pHandler);
#endif
