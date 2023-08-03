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

#ifndef ARCH_SPEC_H
#define ARCH_SPEC_H
#include "ik_data_type.h"
#include "idspdrv_imgknl_if.h"

#define SUPPORT_FUSION 0

/* AUTO COPY TO VENDER INC */
#ifdef IK_RELEASE
#define STATIC static
#define INLINE inline
#else
#define STATIC
#define INLINE
#endif

#ifndef MAX_CONTEXT_NUM
#define MAX_CONTEXT_NUM (32U)
#endif

#ifndef MAX_CR_RING_NUM
#define MAX_CR_RING_NUM (64U)
#endif

#ifndef MAX_CONTAINER_IDSP_CMD_NUM
#define MAX_CONTAINER_IDSP_CMD_NUM (10U)
#endif

#ifndef MAX_CONTAINER_VIN_CMD_NUM
#define MAX_CONTAINER_VIN_CMD_NUM (10U)
#endif

#define MAX_VIN_NUM (4U)

#ifndef IK_MAX_HDR_EXPOSURE_NUM
#define IK_MAX_HDR_EXPOSURE_NUM (3U)
#endif

#define IK_WAIT_FOREVER (0xFFFFFFFFU)

/***** ability definition *****/
/* PIPE  */
#ifndef AMBA_IK_VIDEO_PIPE
#define AMBA_IK_VIDEO_PIPE (0U)
#endif

#ifndef AMBA_IK_STILL_PIPE
#define AMBA_IK_STILL_PIPE (1U)
#endif

/* VIDEO */
#ifndef AMBA_IK_VIDEO_PIPE_LINEAR
#define AMBA_IK_VIDEO_PIPE_LINEAR (0U)
#endif

#ifndef AMBA_IK_VIDEO_PIPE_LINEAR_CE
#define AMBA_IK_VIDEO_PIPE_LINEAR_CE (1U)
#endif

#ifndef AMBA_IK_VIDEO_PIPE_HDR_EXPO_2
#define AMBA_IK_VIDEO_PIPE_HDR_EXPO_2 (2U)
#endif

#ifndef AMBA_IK_VIDEO_PIPE_HDR_EXPO_3
#define AMBA_IK_VIDEO_PIPE_HDR_EXPO_3 (3U)
#endif

#ifndef AMBA_IK_VIDEO_PIPE_LINEAR_MD
#define AMBA_IK_VIDEO_PIPE_LINEAR_MD (4U)
#endif

#ifndef AMBA_IK_VIDEO_PIPE_LINEAR_CE_MD
#define AMBA_IK_VIDEO_PIPE_LINEAR_CE_MD (5U)
#endif

#ifndef AMBA_IK_VIDEO_PIPE_HDR_EXPO_2_MD
#define AMBA_IK_VIDEO_PIPE_HDR_EXPO_2_MD (6U)
#endif

#ifndef AMBA_IK_VIDEO_PIPE_HDR_EXPO_3_MD
#define AMBA_IK_VIDEO_PIPE_HDR_EXPO_3_MD (7U)
#endif

#ifndef AMBA_IK_VIDEO_PIPE_FUSION
#define AMBA_IK_VIDEO_PIPE_FUSION (8U)
#endif

#ifndef AMBA_IK_VIDEO_PIPE_Y2Y
#define AMBA_IK_VIDEO_PIPE_Y2Y (9U)
#endif

#ifndef AMBA_IK_VIDEO_PIPE_LINEAR_HVH
#define AMBA_IK_VIDEO_PIPE_LINEAR_HVH (10U)
#endif

#ifndef AMBA_IK_VIDEO_PIPE_LINEAR_CE_HVH
#define AMBA_IK_VIDEO_PIPE_LINEAR_CE_HVH (11U)
#endif

#ifndef AMBA_IK_VIDEO_PIPE_HDR_EXPO_2_HVH
#define AMBA_IK_VIDEO_PIPE_HDR_EXPO_2_HVH (12U)
#endif

#ifndef AMBA_IK_VIDEO_PIPE_HDR_EXPO_3_HVH
#define AMBA_IK_VIDEO_PIPE_HDR_EXPO_3_HVH (13U)
#endif

#ifndef AMBA_IK_VIDEO_PIPE_Y2Y_MD
#define AMBA_IK_VIDEO_PIPE_Y2Y_MD (18U)
#endif

#ifndef AMBA_IK_VIDEO_PIPE_Y2Y_HVH
#define AMBA_IK_VIDEO_PIPE_Y2Y_HVH (19U)
#endif

#ifndef AMBA_IK_VIDEO_PIPE_LINEAR_HHB
#define AMBA_IK_VIDEO_PIPE_LINEAR_HHB (20U)
#endif

#ifndef AMBA_IK_VIDEO_PIPE_LINEAR_CE_HHB
#define AMBA_IK_VIDEO_PIPE_LINEAR_CE_HHB (21U)
#endif

#ifndef AMBA_IK_VIDEO_PIPE_HDR_EXPO_2_HHB
#define AMBA_IK_VIDEO_PIPE_HDR_EXPO_2_HHB (22U)
#endif

#ifndef AMBA_IK_VIDEO_PIPE_HDR_EXPO_3_HHB
#define AMBA_IK_VIDEO_PIPE_HDR_EXPO_3_HHB (23U)
#endif

#ifndef AMBA_IK_VIDEO_PIPE_LINEAR_MD_HHB
#define AMBA_IK_VIDEO_PIPE_LINEAR_MD_HHB (24U)
#endif

#ifndef AMBA_IK_VIDEO_PIPE_LINEAR_CE_MD_HHB
#define AMBA_IK_VIDEO_PIPE_LINEAR_CE_MD_HHB (25U)
#endif

#ifndef AMBA_IK_VIDEO_PIPE_HDR_EXPO_2_MD_HHB
#define AMBA_IK_VIDEO_PIPE_HDR_EXPO_2_MD_HHB (26U)
#endif

#ifndef AMBA_IK_VIDEO_PIPE_HDR_EXPO_3_MD_HHB
#define AMBA_IK_VIDEO_PIPE_HDR_EXPO_3_MD_HHB (27U)
#endif

#ifndef AMBA_IK_VIDEO_PIPE_Y2Y_HHB
#define AMBA_IK_VIDEO_PIPE_Y2Y_HHB (28U)
#endif

#ifndef AMBA_IK_VIDEO_PIPE_Y2Y_MD_HHB
#define AMBA_IK_VIDEO_PIPE_Y2Y_MD_HHB (29U)
#endif

#ifndef AMBA_IK_VIDEO_PIPE_MAX
#define AMBA_IK_VIDEO_PIPE_MAX (30U)
#endif


#ifndef AMBA_IK_STILL_PIPE_BASE
#define AMBA_IK_STILL_PIPE_BASE (32U)
#endif
#ifndef AMBA_IK_STILL_PIPE_LISO
#define AMBA_IK_STILL_PIPE_LISO (AMBA_IK_STILL_PIPE_BASE + 1U)
#endif
#ifndef AMBA_IK_STILL_PIPE_HISO
#define AMBA_IK_STILL_PIPE_HISO (AMBA_IK_STILL_PIPE_BASE + 2U)
#endif

//#ifndef AMBA_IK_STILL_HISO_CE
//    #define AMBA_IK_STILL_HISO_CE (AMBA_IK_STILL_PIPE_BASE + 3U)    // TODO, remove it if HISO always have sec4
//#endif
#ifndef AMBA_IK_STILL_PIPE_MAX
#define AMBA_IK_STILL_PIPE_MAX (AMBA_IK_STILL_PIPE_BASE + 3U)
#endif

#define AMBA_IK_ARCH_HARD_INIT 0U
#define AMBA_IK_ARCH_SOFT_INIT 1U

typedef struct {
    uint8 pipe;
    uint32 still_pipe;
    uint32 video_pipe;
} ik_ability_t;

typedef struct {
    uint32 ik_id;
    void *p_flow_ctrl;
} ik_execute_container_t;

#endif
