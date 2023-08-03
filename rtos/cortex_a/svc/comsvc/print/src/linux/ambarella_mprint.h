/*
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
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 *  AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _MPRINT_LIB_H_INCLUDED
#define _MPRINT_LIB_H_INCLUDED


#include <stdint.h>
#include <AmbaKAL.h>
//#include "AmbaUART_Def.h"

#define SHM_PRINT_CTRL "/PrintRegion"
#define SHM_PRINT_BUF  "/PrintRingBuf"
#define MQ_PRINT       "/AmbaPrintQueue"

#define PRINT_REQUEST_QUEUE_NUM  (200U)
#define PRINT_REQUEST_TIMEOUT    (100U)
#define PRINT_PORT                 (0U)
#define PRINT_PORT_MAX             (2U)

#define LINE_BUFFER_LENGTH      (4096U)
#define PRINT_BUFFER_LENGTH     (1024*1024U)
#define LOG_BUFFER_LENGTH       (1024U)

#define ALLOW_LIST_ENTRY_NUMBER        (65536U/32U)

/* Our real data comes after the header */
typedef struct _my_data {
    int          rIdx;
    int          wIdx;
} amba_print_data_t;

/* Defines "structure" of shared memory */
struct print_region {
    int              ring_r_idx;
    int              ring_w_idx;
    UINT32           ring_buf_size;
    //ring buffer is in another memory region
    //UINT8            ring_buf[PRINT_BUFFER_LENGTH];
    int              log_r_idx;
    int              log_w_idx;
    UINT8            log_buf[LOG_BUFFER_LENGTH];
    UINT32           log_buf_size;
    pthread_mutex_t  mutex_print;
    pthread_mutex_t  mutex_write;
    UINT32           module_allow_list[ALLOW_LIST_ENTRY_NUMBER];
    UINT32           flag_print_stop;
    struct timespec  mq_timeout;
};

#endif