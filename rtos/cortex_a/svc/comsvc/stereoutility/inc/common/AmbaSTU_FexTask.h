/**
 *  @file AmbaStu_FexTask.h
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
 *  @details FEX algo task, wrap up SCA FEX flexidag
 *
 */

#ifndef __APPAC_SCA_AMBA_FEX_H__
#define __APPAC_SCA_AMBA_FEX_H__
#include "AmbaSTU_AlgoTaskUtil.h"

#define APPAC_SCA_FEX_MEM_SIZE (9183232U)

typedef struct {
    SVC_CV_ALGO_HANDLE_s ScaHandle;
} AppAcScaFex_t;

UINT32 AppAc_ScaAmbaFexInit(AppAcTaskHandle_t *pScaAmbaFexHandle, UINT32 MemPoolId, char *FlexiBinFileName);
UINT32 AppAc_ScaAmbaFexStart(AppAcTaskHandle_t *pScaAmbaFexHandle);
UINT32 AppAc_ScaAmbaFexStop(AppAcTaskHandle_t *pScaAmbaFexHandle);
UINT32 AppAc_ScaAmbaFexDeinit(AppAcTaskHandle_t *pScaAmbaFexHandle);


#endif
