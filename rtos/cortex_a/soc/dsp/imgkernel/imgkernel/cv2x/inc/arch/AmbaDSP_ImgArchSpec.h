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
#include "AmbaDSP_ImgIkcSpec.h"

/* AUTO COPY TO VENDER INC */
#ifdef IK_RELEASE
#define STATIC static
#define INLINE inline
#else
#define STATIC
#define INLINE
#endif
#ifndef MAX_CONTEXT_NUM
#define MAX_CONTEXT_NUM (16U)
#endif
#ifndef MAX_CONFIG_NUM
#define MAX_CONFIG_NUM (10U)
#endif
#define MAX_CONTAINER_IDSP_CMD_NUM (10U)
#define MAX_CONTAINER_VIN_CMD_NUM (10U)
#define MAX_VIN_NUM (4U)
#define IK_MAX_HDR_EXPOSURE_NUM (3U)

#define AMBA_IK_ALGO_MODE_LISO (0x0U)

#define IK_WAIT_FOREVER (0xFFFFFFFFUL)

/***** ability definition *****/
/* PIPE  */
#ifndef AMBA_IK_PIPE_VIDEO
#define AMBA_IK_PIPE_VIDEO AMBA_IKC_PIPE_VIDEO
#endif
#ifndef AMBA_IK_PIPE_STILL
#define AMBA_IK_PIPE_STILL AMBA_IKC_PIPE_STILL
#endif
/* VIDEO */
#ifndef AMBA_IK_VIDEO_LINEAR
#define AMBA_IK_VIDEO_LINEAR AMBA_IKC_VIDEO_LINEAR
#endif
#ifndef AMBA_IK_VIDEO_LINEAR_CE
#define AMBA_IK_VIDEO_LINEAR_CE AMBA_IKC_VIDEO_LINEAR_CE
#endif
#ifndef AMBA_IK_VIDEO_HDR_EXPO_2
#define AMBA_IK_VIDEO_HDR_EXPO_2 AMBA_IKC_VIDEO_HDR_EXPO_2
#endif
#ifndef AMBA_IK_VIDEO_HDR_EXPO_3
#define AMBA_IK_VIDEO_HDR_EXPO_3 AMBA_IKC_VIDEO_HDR_EXPO_3
#endif
#ifndef AMBA_IK_VIDEO_LINEAR_MD
#define AMBA_IK_VIDEO_LINEAR_MD AMBA_IKC_VIDEO_LINEAR_MD
#endif
#ifndef AMBA_IK_VIDEO_LINEAR_CE_MD
#define AMBA_IK_VIDEO_LINEAR_CE_MD AMBA_IKC_VIDEO_LINEAR_CE_MD
#endif
#ifndef AMBA_IK_VIDEO_HDR_EXPO_2_MD
#define AMBA_IK_VIDEO_HDR_EXPO_2_MD AMBA_IKC_VIDEO_HDR_EXPO_2_MD
#endif
#ifndef AMBA_IK_VIDEO_HDR_EXPO_3_MD
#define AMBA_IK_VIDEO_HDR_EXPO_3_MD AMBA_IKC_VIDEO_HDR_EXPO_3_MD
#endif
#ifndef AMBA_IK_VIDEO_Y2Y
#define AMBA_IK_VIDEO_Y2Y AMBA_IKC_VIDEO_Y2Y
#endif
#ifndef AMBA_IK_VIDEO_Y2Y_MD
#define AMBA_IK_VIDEO_Y2Y_MD AMBA_IKC_VIDEO_Y2Y_MD
#endif
#ifndef AMBA_IK_VIDEO_FUSION_CE_MD
#define AMBA_IK_VIDEO_FUSION_CE_MD AMBA_IKC_VIDEO_FUSION_CE_MD
#endif
#ifndef AMBA_IK_VIDEO_FUSION_CE
#define AMBA_IK_VIDEO_FUSION_CE AMBA_IKC_VIDEO_FUSION_CE
#endif
#ifndef AMBA_IK_VIDEO_Y2Y_MIPI
#define AMBA_IK_VIDEO_Y2Y_MIPI AMBA_IKC_VIDEO_Y2Y_MIPI
#endif
#ifndef AMBA_IK_VIDEO_PRE_NN_PROCESS
#define AMBA_IK_VIDEO_PRE_NN_PROCESS AMBA_IKC_VIDEO_PRE_NN_PROCESS
#endif
#ifndef AMBA_IK_VIDEO_MAX
#define AMBA_IK_VIDEO_MAX AMBA_IKC_VIDEO_MAX
#endif

#ifndef AMBA_IK_STILL_BASE
#define AMBA_IK_STILL_BASE AMBA_IKC_STILL_BASE
#endif
#ifndef AMBA_IK_STILL_LISO
#define AMBA_IK_STILL_LISO AMBA_IKC_STILL_LISO
#endif
#ifndef AMBA_IK_STILL_HISO
#define AMBA_IK_STILL_HISO AMBA_IKC_STILL_HISO
#endif
#ifndef AMBA_IK_STILL_MAX
#define AMBA_IK_STILL_MAX AMBA_IKC_STILL_MAX
#endif

#ifndef AMBA_IK_ARCH_DEFINE
#define AMBA_IK_ARCH_DEFINE AMBA_IKC_ARCH_DEFINE
#define AMBA_IK_ARCH_HARD_INIT AMBA_IKC_ARCH_HARD_INIT
#define AMBA_IK_ARCH_SOFT_INIT AMBA_IKC_ARCH_SOFT_INIT
#endif

typedef enum {
    amba_ik_func_mode_fv = 0x00U,
    amba_ik_func_mode_qv = 0x01U,
    amba_ik_func_mode_piv = 0X02U,
    amba_ik_func_mode_r2r = 0x04U,
    amba_ik_func_mode_vhdr = 0x08U,
    amba_ik_func_mode_y2y = 0x10U,
    amba_ik_func_mode_md = 0x20U,
    amba_ik_func_mode_ce = 0x40U,
    amba_ik_func_mode_y2y_bypass,
} amba_ik_func_mode_t;

#define ik_ability_t ikc_ability_t

typedef struct {
    uint32 ik_id;
    void *p_iso_cfg;
    ikc_iso_config_update update;
} ik_execute_container_t;

#endif
