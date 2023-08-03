/**
 *  @file SvcCvFlowInputUtil.h
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
 *  @details Header of SvcCvFlow input utility
 *
 */

#ifndef SVC_CV_FLOW_INPUT_UTIL_H
#define SVC_CV_FLOW_INPUT_UTIL_H

typedef struct {
    UINT16 DataSrc;
    UINT32 StrmId;
} SVC_CV_INPUT_IMG_CONTENT_s;

typedef struct {
    SVC_CV_INPUT_IMG_CONTENT_s Content;
    UINT8 *pBase;
    UINT32 ElementSize;
} SVC_CV_INPUT_IMG_DESC_s;

#define SVC_CV_INPUT_INFO_MAX_DATA_NUM      (3U)    /* Maximum number of data frame info */
typedef struct {
    UINT32 NumInfo;
    SVC_CV_INPUT_IMG_DESC_s Info[SVC_CV_INPUT_INFO_MAX_DATA_NUM];
} SVC_CV_INPUT_IMG_INFO_s;

#endif /* SVC_CV_FLOW_INPUT_UTIL_H */