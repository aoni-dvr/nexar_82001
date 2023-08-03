/**
 *  @file AmbaStu_MvacTask.h
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
 *  @details Multi View Auto Calibration algo task, wrap up mvac software function
 *
 */

#ifndef __APPAC_SCA_AMBA_MVAC_H__
#define __APPAC_SCA_AMBA_MVAC_H__
#include "AmbaSTU_AlgoTaskUtil.h"
#include "cvapi_flexidag_ambamvac_cv2.h"

#define APPAC_MVAC_MEM_SIZE (16U<<20U)

typedef struct {
    AMBA_CV_MVAC_CFG_s MvacConfig;
    AMBA_MVAC_FD_HANDLE_s MvacFdHandler;
    flexidag_memblk_t InMetaRight;
    flexidag_memblk_t InMetaLeft;
    UINT32 MvacAlgoState;
} AppAcMvac_t;

UINT32 AppAc_AmbaMvacInit(AppAcTaskHandle_t *pHandle, UINT32 MemPoolId, flexidag_memblk_t *pInitMetaLeftBuf, AMBA_CV_MVAC_VIEW_INFO_s *pMvacViewInfo);
UINT32 AppAc_AmbaMvacStart(AppAcTaskHandle_t *pHandle);
UINT32 AppAc_AmbaMvacStop(AppAcTaskHandle_t *pHandle);
UINT32 AppAc_AmbaMvacDeinit(AppAcTaskHandle_t *pHandle, UINT32 MemPoolId);


#endif
