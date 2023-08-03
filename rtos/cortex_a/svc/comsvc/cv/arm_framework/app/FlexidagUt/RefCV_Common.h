/**
 *  @file RefCV_Common.h
 *
 *  Copyright (c) 2020 Ambarella International LP
 *
 *  This file and its contents ("Software") are protected by intellectual
 *  property rights including, without limitation, U.S. and/or foreign
 *  copyrights. This Software is also the confidential and proprietary
 *  information of Ambarella International LP and its licensors. You may not use, reproduce,
 *  disclose, distribute, modify, or otherwise prepare derivative works of this
 *  Software or any portion thereof except pursuant to a signed license agreement
 *  or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
 *  In the absence of such an agreement, you agree to promptly notify and return
 *  this Software to Ambarella International LP.
 *
 *  This file includes sample code and is only for internal testing and evaluation.  If you
 *  distribute this sample code (whether in source, object, or binary code form), it will be
 *  without any warranty or indemnity protection from Ambarella International LP or its affiliates.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 *  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 *  MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 *  IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 *  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details Definitions & Constants for CV Common APIs
 *
 */

#ifndef REF_CV_COMMON_H
#define REF_CV_COMMON_H

#include "cvapi_svccvalgo_memio_interface.h"

#define REF_CV_RUN_MODE_RTOS        0U
#define REF_CV_RUN_MODE_AMBALINK    1U

typedef struct {
    uint32_t                        id;
    AMBA_CV_FLEXIDAG_HANDLE_s       fd_gen_handle;
    char                            name[64];
    AMBA_CV_FLEXIDAG_INIT_s         init;
    flexidag_memblk_t               bin_buf;
    AMBA_CV_FLEXIDAG_IO_s           in_buf;
    AMBA_CV_FLEXIDAG_IO_s           out_buf;
} REF_CV_HANDLE_s;

typedef struct {
    UINT32                          id;
    UINT32                          num_runs;
    UINT32                          cur_runs;
    UINT32                          num_err;
    UINT32                          sleep_usec;
    UINT32                          RunFlag;
    UINT32                          blocking_run;
    char                            file_nameY[128];
    char                            file_nameUV[128];
    char                            flexidag_path[128];
    REF_CV_HANDLE_s                 handle;
    AMBA_KAL_TASK_t                 task;
    AMBA_KAL_TASK_t                 result_task;
    flexidag_memblk_t               file_regionY;
    flexidag_memblk_t               file_regionUV;
    AMBA_KAL_MSG_QUEUE_t            msg_queue;
    AMBA_KAL_MSG_QUEUE_t            result_msg_queue;
    UINT32                          queue_buffer[8U];
    UINT32                          result_queue_buffer[8U];
} REF_CV_PARAMS_s;

#define REF_CV_BUF_NUM              4U

typedef struct {
    uint32_t                        id;
    AMBA_CV_FLEXIDAG_HANDLE_s       fd_gen_handle;
    char                            name[64];
    AMBA_CV_FLEXIDAG_INIT_s         init;
    flexidag_memblk_t               bin_buf;
    AMBA_CV_FLEXIDAG_IO_s           in_buf[REF_CV_BUF_NUM];
    AMBA_CV_FLEXIDAG_IO_s           out_buf[REF_CV_BUF_NUM];
} REF_CV_RING_HANDLE_s;

typedef struct {
    UINT32 frame_id;
    UINT32 token_id;
    flexidag_memblk_t frame_buf;
} REF_CV_FRAME_s;

typedef struct {
    UINT32                          id;
    UINT32                          num_runs;
    UINT32                          cur_runs;
    UINT32                          num_err;
    UINT32                          sleep_usec;
    UINT32                          RunFlag;
    UINT32                          blocking_run;
    char                            flexidag_path[128];
    REF_CV_RING_HANDLE_s            handle;
    AMBA_KAL_TASK_t                 task;
    AMBA_KAL_TASK_t                 result_task;
    AMBA_KAL_MSG_QUEUE_t            msg_queue;
    AMBA_KAL_MSG_QUEUE_t            result_msg_queue;
    UINT32                          queue_buffer[REF_CV_BUF_NUM];
    REF_CV_FRAME_s                  result_queue_buffer[REF_CV_BUF_NUM];
} REF_CV_RING_PARAMS_s;

UINT32 RefCVSprintfStr(char *str, const char *str_src);
UINT32 RefCVSprintfUint1(char *str,UINT32 strbuf_size,const char *format,UINT32 arg0);
UINT32 RefCVSprintfUint2(char *str,UINT32 strbuf_size,const char *format,UINT32 arg0,UINT32 arg1);
UINT32 RefCVSprintfUint3(char *str,UINT32 strbuf_size,const char *format,UINT32 arg0,UINT32 arg1,UINT32 arg2);
UINT32 RefCVSprintfUint4(char *str,UINT32 strbuf_size,const char *format,UINT32 arg0,UINT32 arg1,UINT32 arg2,UINT32 arg3);
UINT32 RefCVSprintfUint5(char *str,UINT32 strbuf_size,const char *format,UINT32 arg0,UINT32 arg1,UINT32 arg2,UINT32 arg3,UINT32 arg4);
UINT32 RefCVRunPicinfo(REF_CV_HANDLE_s *handle, const cv_pic_info_t *in, AMBA_CV_FLEXIDAG_RUN_INFO_s *run_info);
UINT32 RefCVRunNonBlockingPicinfo(REF_CV_HANDLE_s *handle, flexidag_cb cb, void *cb_param, const cv_pic_info_t *in, UINT32 *token_id);
UINT32 RefCVRunRaw(REF_CV_HANDLE_s *handle, const memio_source_recv_multi_raw_t *in, AMBA_CV_FLEXIDAG_RUN_INFO_s *run_info);
UINT32 RefCVRunNonBlockingRaw(REF_CV_HANDLE_s *handle, flexidag_cb cb, void *cb_param, const memio_source_recv_multi_raw_t *in, UINT32 *token_id);
#endif /* REF_CV_COMMON_H */

