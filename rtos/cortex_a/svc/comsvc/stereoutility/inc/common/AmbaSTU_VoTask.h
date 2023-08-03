/**
 *  @file AmbaStu_VoTask.h
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
 *  @details Visual Odometry algo task, wrap up vo software function
 *
 */

#ifndef __APPAC_SCA_AMBA_VO_H__
#define __APPAC_SCA_AMBA_VO_H__
#include "AmbaSTU_AlgoTaskUtil.h"
#include "cvapi_flexidag_ambavo_cv2.h"

#define APPAC_VO_MEM_SIZE (4U<<20U)

typedef struct {
    AMBA_CV_VO_CFG_s VoConfig;
    AMBA_VO_FD_HANDLE_s VoFdHandler;
    AMBA_CV_FMA_BUF_s PreFmaBuf;
    flexidag_memblk_t StateBuf;
    flexidag_memblk_t TempBuf;
} AppAcVo_t;

UINT32 AppAc_AmbaVoInit(AppAcTaskHandle_t *pHandle, UINT32 MemPoolId, flexidag_memblk_t *pInitMetaRightBuf, AMBA_CV_VO_CFG_s *pVoCfg);
UINT32 AppAc_AmbaVoStart(AppAcTaskHandle_t *pHandle);
UINT32 AppAc_AmbaVoStop(AppAcTaskHandle_t *pHandle);
UINT32 AppAc_AmbaVoDeinit(AppAcTaskHandle_t *pHandle);


#endif
