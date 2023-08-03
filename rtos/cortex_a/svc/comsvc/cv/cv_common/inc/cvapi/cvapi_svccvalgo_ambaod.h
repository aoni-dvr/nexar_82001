/**
 *  @file cvapi_svccvalgo_ambaod.h
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

#ifndef CVAPI_SVCCVALGO_AMBAOD__H
#define CVAPI_SVCCVALGO_AMBAOD__H

#include "SvcCvAlgo.h"

extern SVC_CV_ALGO_OBJ_s AmbaODAlgoObj;
extern SVC_CV_ALGO_OBJ_s AmbaOD37AlgoObj;


#define SCA_CT_AMBAOD_ROI_F0        (0U)
#define SCA_CT_AMBAOD_ROI_F1        (1U)

#define SCA_CT_AMBAOD_LCS_2SCALE    (2U)

#define SCA_CT_AMBAOD_SIZE0         (0U)    /* 1280x640 */
#define SCA_CT_AMBAOD_SIZE1         (1U)    /* 896x512 */

/* Extention config to setup AmbaOD task and ROIs. */
#define ExtTaskCreateCfgMagic     0xBABEABBAU
typedef struct {
    UINT32          MagicCode;
    CCF_TSK_CTRL_s  RunTskCtrl;
    CCF_TSK_CTRL_s  CBTskCtrl;
    UINT16          ROIs;
    UINT16          ODSize;
} ExtTaskCreateCfg;

#endif //CVAPI_SVCCVALGO_AMBAOD__H

