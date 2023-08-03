/**
 *  @file AmbaStu_SinkTask.h
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
 *  @details Sink task, can be connect to a algo task and receive output from it, callback to high level app.
 *
 */

#ifndef __APPAC_SINK_H__
#define __APPAC_SINK_H__

#include "AmbaSTU_AlgoTaskUtil.h"

#define MaxFileNum 4U
#define MaxFileNameLen 128U
#define APPAC_SINK_MEM_SIZE (8192U)

typedef struct AppAcSink_s {
    AcGetResultCb_t AcGetResultCb;
} AppAcSink_t;

UINT32 AppAc_SinkTaskInit(AppAcTaskHandle_t *pFileOutHandle, UINT32 MemPoolId, AcGetResultCb_t AcGetResultCb);
UINT32 AppAc_SinkTaskStart(AppAcTaskHandle_t *pFileOutHandle);
UINT32 AppAc_SinkTaskStop(AppAcTaskHandle_t *pFileOutHandle);
UINT32 AppAc_SinkTaskDeinit(AppAcTaskHandle_t *pFileOutHandle);


#endif
