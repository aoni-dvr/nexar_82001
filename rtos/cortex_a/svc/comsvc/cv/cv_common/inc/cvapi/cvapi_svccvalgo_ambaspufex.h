/**
 *  @file cvapi_svccvalgo_ambaspufex.h
 *
 *  @copyright Copyright (c) 2019 Ambarella, Inc.
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
 *  @details Implementation of amba od svc cv algo
 *
 */

#ifndef CVAPI_SVCCVALGO_AMBASPUFEX__H
#define CVAPI_SVCCVALGO_AMBASPUFEX__H

#include "SvcCvAlgo.h"

#define AMBA_SCA_SPUFEX_CTRL_CFG          (0U)
#define AMBA_SCA_SPUFEX_CTRL_DISPLAY_MODE (1U)

/* Display Mode Type */
#define AMBA_SCA_CV_SPU_NORMAL_MODE       (0U) //AMBA_CV_SPU_NORMAL_MODE
#define AMBA_SCA_CV_SPU_DISPLAY_MODE      (1U) //AMBA_CV_SPU_DISPLAY_MODE

extern SVC_CV_ALGO_OBJ_s AmbaSpuFexAlgoObj;


#endif //CVAPI_SVCCVALGO_AMBASPUFEX__H
