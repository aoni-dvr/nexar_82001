/*
* Copyright (c) 2020 Ambarella International LP
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
*/

#ifndef IMG_COMMON_H
#define IMG_COMMON_H

#include "ik_data_type.h"

#define SUPPORT_IKC_ADDR_CHECK 0

#define IK_BUFFER_HEADER_SIZE 512U

#define IK_SAFETY_STATE_OK 0u
#define IK_SAFETY_STATE_ERROR 1u


#ifndef MAX_CONTEXT_NUM
#define MAX_CONTEXT_NUM (32U)
#endif

#ifndef MAX_CR_RING_NUM
#define MAX_CR_RING_NUM (32U)
#endif

#ifndef IK_MAX_HDR_EXPOSURE_NUM
#define IK_MAX_HDR_EXPOSURE_NUM (3U)
#endif

/***** ability definition *****/
/* PIPE  */
#ifndef AMBA_IK_PIPE_VIDEO
#define AMBA_IK_PIPE_VIDEO (0U)
#endif

#ifndef AMBA_IK_PIPE_STILL
#define AMBA_IK_PIPE_STILL (1U)
#endif

/* STILL */
#ifndef AMBA_IK_STILL_MAX
#define AMBA_IK_STILL_MAX (0UL)
#endif

/* VIDEO */
#ifndef AMBA_IK_VIDEO_LINEAR
#define AMBA_IK_VIDEO_LINEAR (0UL)
#endif

#ifndef AMBA_IK_VIDEO_LINEAR_CE
#define AMBA_IK_VIDEO_LINEAR_CE (1UL)
#endif

#ifndef AMBA_IK_VIDEO_HDR_EXPO_2
#define AMBA_IK_VIDEO_HDR_EXPO_2 (2UL)
#endif

#ifndef AMBA_IK_VIDEO_HDR_EXPO_3
#define AMBA_IK_VIDEO_HDR_EXPO_3 (3UL)
#endif

#ifndef AMBA_IK_VIDEO_Y2Y
#define AMBA_IK_VIDEO_Y2Y (4UL)
#endif

#ifndef AMBA_IK_VIDEO_MAX
#define AMBA_IK_VIDEO_MAX (5UL)
#endif

#define IK_WAIT_FOREVER (0xFFFFFFFFUL)

#define AMBA_IK_ARCH_HARD_INIT 0UL
#define AMBA_IK_ARCH_SOFT_INIT 1UL


typedef struct {
    uint8 pipe;
    uint32 still_pipe;
    uint32 video_pipe;
} ik_ability_t;

typedef struct {
    uint32 ik_version_major;
    uint32 ik_version_minor;
    uint32 init_flag;
    void *working_buffer_addr;
    uint32 number_of_context;
    uintptr context_entity[MAX_CONTEXT_NUM];
    uint32 buffer_size;
    uint32 init_mode;
    void *p_bin_data_dram_addr;
    uint32 safety_enable;
    uint32 safety_state;
    uint32 safety_logic_enable;
    uint32 safety_crc_enable;
    uint8 reserved_ik_buffer_info_t[(IK_BUFFER_HEADER_SIZE)-(((sizeof(uintptr)*MAX_CONTEXT_NUM) + (sizeof(uint32)*10U) + (sizeof(void *)*2U))%(IK_BUFFER_HEADER_SIZE))];
} ik_buffer_info_t;

typedef struct {
    uint32 context_number;
    struct {
        uint32 cr_ring_number;
        ik_ability_t *p_ability;
    } cr_ring_setting[MAX_CONTEXT_NUM];
} ik_context_setting_t;

typedef struct {
    uint32 ik_id;
    uint32 ik_crc;
    void *p_flow_ctrl;
} ik_execute_container_t;

#if SUPPORT_IKC_ADDR_CHECK
typedef struct {
    void *p_flow_start_addr;
    void *p_cr_start_addr;
} ik_buf_info_t;

uint32 img_notify_iks_buffer_addr(uint32 context_id, const ik_buf_info_t *p_ik_buf_info);
#endif

#endif
