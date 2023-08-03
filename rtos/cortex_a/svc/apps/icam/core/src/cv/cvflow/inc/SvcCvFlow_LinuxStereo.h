/**
 *  @file SvcCvFlow_LinuxACStixel.h
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
 *  @details Header of Stereo Auto Calibration driver
 *
 */

#ifndef SVC_CV_FLOW_LINUX_H
#define SVC_CV_FLOW_LINUX_H

#include "cvapi_svccvalgo_stixel.h"


/* StereoAC mode */
#define LINUX_STEREO_SCA_1          (0U)
#define LINUX_STEREO_NUM_MODE              (1U)

/* CtrlType */

/* Output Type */
#define SVC_LINUX_STEREO_MAX_ALGO_NUM                      (SVC_CV_FLOW_MAX_ALGO_PER_GRP)

typedef struct {
    UINT32  ProfID;
    SVC_CV_FLOW_ALGO_GROUP_s          AlgoGrp;
} SVC_CV_FLOW_LINUX_STEREO_MODE_INFO_s;

/*-----------------------------------------------------------------------------------------------*\
 *  Defined in SvcCvFlow_StereoAC.c
\*-----------------------------------------------------------------------------------------------*/
extern SVC_CV_FLOW_OBJ_s SvcCvFlow_LinuxStereoObj;

extern const SVC_CV_FLOW_LINUX_STEREO_MODE_INFO_s SvcCvFlow_LinuxStereo_ModeInfo[LINUX_STEREO_NUM_MODE];

#endif /* SVC_CV_FLOW_LINUX_AC_STIXEL_H */
