/**
 *  @file AmbaRTSL_ECRU.h
 *
 *  @copyright Copyright (c) 2020 Ambarella, Inc.
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
 *  @details Definitions of Run Time Support Library for ECRU
 *
 */

#ifndef AMBA_RTSL_ECRU_H
#define AMBA_RTSL_ECRU_H

#ifndef AMBA_CSL_ECRU_H
#include "AmbaCSL_ECRU.h"
#endif

#define RTSL_ERR_ECRU_SUCCESS        (0U)
#define RTSL_ERR_ECRU_PARAM          (1U)
#define RTSL_ERR_ECRU_REACH_MAX      (3U)


UINT32 AmbaRTSL_ECRUInputMaskGet(UINT32 ErrorID, UINT32 *Value);
UINT32 AmbaRTSL_ECRUInputMaskEnable(UINT32 ErrorID);
UINT32 AmbaRTSL_ECRUInputMaskDisable(UINT32 ErrorID);
UINT32 AmbaRTSL_ECRUOutputMaskGet(UINT32 ErrorID, UINT32 *Value);
UINT32 AmbaRTSL_ECRUOutputMaskEnable(UINT32 ErrorID);
UINT32 AmbaRTSL_ECRUOutputMaskDisable(UINT32 ErrorID);
UINT32 AmbaRTSL_ECRUErrorGet(UINT32 ErrorID, UINT32 *Value);
UINT32 AmbaRTSL_ECRUErrorClear(UINT32 ErrorID);


#endif /* AMBA_RTSL_ECRU_H */

