/*
*  @file AmbaDSP_ImageDefine.h
*
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

#ifndef AMBA_IK_IMAGE_DEFINE_H
#define AMBA_IK_IMAGE_DEFINE_H

#include "AmbaTypes.h"

/* AUTO COPY TO VENDER INC */
#ifdef IK_RELEASE
#ifndef STATIC
#define STATIC static
#endif
#ifndef INLINE
#define INLINE inline
#endif
#else
#ifndef STATIC
#define STATIC
#endif
#ifndef INLINE
#define INLINE
#endif
#endif

#ifndef MAX_CONTEXT_NUM
#define MAX_CONTEXT_NUM (32U)
#endif

#ifndef MAX_CR_RING_NUM
#define MAX_CR_RING_NUM (32U)
#endif

#ifndef IK_MAX_HDR_EXPOSURE_NUM
#define IK_MAX_HDR_EXPOSURE_NUM (3U)
#endif

#ifndef MAX_CONTAINER_IDSP_CMD_NUM
#define MAX_CONTAINER_IDSP_CMD_NUM (10U)
#endif

#ifndef MAX_CONTAINER_VIN_CMD_NUM
#define MAX_CONTAINER_VIN_CMD_NUM (10U)
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
#define AMBA_IK_STILL_MAX (0U)
#endif

/* VIDEO */
#ifndef AMBA_IK_VIDEO_LINEAR
#define AMBA_IK_VIDEO_LINEAR (0U)
#endif

#ifndef AMBA_IK_VIDEO_LINEAR_CE
#define AMBA_IK_VIDEO_LINEAR_CE (1U)
#endif

#ifndef AMBA_IK_VIDEO_HDR_EXPO_2
#define AMBA_IK_VIDEO_HDR_EXPO_2 (2U)
#endif

#ifndef AMBA_IK_VIDEO_HDR_EXPO_3
#define AMBA_IK_VIDEO_HDR_EXPO_3 (3U)
#endif

#ifndef AMBA_IK_VIDEO_Y2Y
#define AMBA_IK_VIDEO_Y2Y (4U)
#endif

#ifndef AMBA_IK_VIDEO_MAX
#define AMBA_IK_VIDEO_MAX (5U)
#endif

typedef struct {
    UINT8 Pipe;
    UINT32 StillPipe;
    UINT32 VideoPipe;
} AMBA_IK_ABILITY_s;

typedef struct {
    UINT32 IkId;
    UINT32 Reserved;
    void *pIsoCfg;
} AMBA_IK_EXECUTE_CONTAINER_s;

#endif
