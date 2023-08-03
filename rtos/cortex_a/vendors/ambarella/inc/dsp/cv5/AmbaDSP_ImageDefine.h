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
#define MAX_CR_RING_NUM (64U)
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

#ifndef AMBA_IK_VIDEO_LINEAR_MD
#define AMBA_IK_VIDEO_LINEAR_MD (4U)
#endif

#ifndef AMBA_IK_VIDEO_LINEAR_CE_MD
#define AMBA_IK_VIDEO_LINEAR_CE_MD (5U)
#endif

#ifndef AMBA_IK_VIDEO_HDR_EXPO_2_MD
#define AMBA_IK_VIDEO_HDR_EXPO_2_MD (6U)
#endif

#ifndef AMBA_IK_VIDEO_HDR_EXPO_3_MD
#define AMBA_IK_VIDEO_HDR_EXPO_3_MD (7U)
#endif

#ifndef AMBA_IK_VIDEO_FUSION
#define AMBA_IK_VIDEO_FUSION (8U)
#endif

#ifndef AMBA_IK_VIDEO_Y2Y
#define AMBA_IK_VIDEO_Y2Y (9U)
#endif

#ifndef AMBA_IK_VIDEO_LINEAR_HVH
#define AMBA_IK_VIDEO_LINEAR_HVH (10U)
#endif

#ifndef AMBA_IK_VIDEO_LINEAR_CE_HVH
#define AMBA_IK_VIDEO_LINEAR_CE_HVH (11U)
#endif

#ifndef AMBA_IK_VIDEO_HDR_EXPO_2_HVH
#define AMBA_IK_VIDEO_HDR_EXPO_2_HVH (12U)
#endif

#ifndef AMBA_IK_VIDEO_HDR_EXPO_3_HVH
#define AMBA_IK_VIDEO_HDR_EXPO_3_HVH (13U)
#endif

#ifndef AMBA_IK_VIDEO_Y2Y_MD
#define AMBA_IK_VIDEO_Y2Y_MD (18U)
#endif

#ifndef AMBA_IK_VIDEO_Y2Y_HVH
#define AMBA_IK_VIDEO_Y2Y_HVH (19U)
#endif

#ifndef AMBA_IK_VIDEO_LINEAR_HHB
#define AMBA_IK_VIDEO_LINEAR_HHB (20U)
#endif

#ifndef AMBA_IK_VIDEO_LINEAR_CE_HHB
#define AMBA_IK_VIDEO_LINEAR_CE_HHB (21U)
#endif

#ifndef AMBA_IK_VIDEO_HDR_EXPO_2_HHB
#define AMBA_IK_VIDEO_HDR_EXPO_2_HHB (22U)
#endif

#ifndef AMBA_IK_VIDEO_HDR_EXPO_3_HHB
#define AMBA_IK_VIDEO_HDR_EXPO_3_HHB (23U)
#endif

#ifndef AMBA_IK_VIDEO_LINEAR_MD_HHB
#define AMBA_IK_VIDEO_LINEAR_MD_HHB (24U)
#endif

#ifndef AMBA_IK_VIDEO_LINEAR_CE_MD_HHB
#define AMBA_IK_VIDEO_LINEAR_CE_MD_HHB (25U)
#endif

#ifndef AMBA_IK_VIDEO_HDR_EXPO_2_MD_HHB
#define AMBA_IK_VIDEO_HDR_EXPO_2_MD_HHB (26U)
#endif

#ifndef AMBA_IK_VIDEO_HDR_EXPO_3_MD_HHB
#define AMBA_IK_VIDEO_HDR_EXPO_3_MD_HHB (27U)
#endif

#ifndef AMBA_IK_VIDEO_Y2Y_HHB
#define AMBA_IK_VIDEO_Y2Y_HHB (28U)
#endif

#ifndef AMBA_IK_VIDEO_Y2Y_MD_HHB
#define AMBA_IK_VIDEO_Y2Y_MD_HHB (29U)
#endif

#ifndef AMBA_IK_VIDEO_MAX
#define AMBA_IK_VIDEO_MAX (30U)
#endif


#ifndef AMBA_IK_STILL_BASE
    #define AMBA_IK_STILL_BASE (32U)
#endif
#ifndef AMBA_IK_STILL_LISO
    #define AMBA_IK_STILL_LISO (AMBA_IK_STILL_BASE + 1U)
#endif
#ifndef AMBA_IK_STILL_PIPE_HISO
    #define AMBA_IK_STILL_HISO (AMBA_IK_STILL_BASE + 2U)
#endif

#ifndef AMBA_IK_STILL_MAX
    #define AMBA_IK_STILL_MAX (AMBA_IK_STILL_BASE + 3U)
#endif

typedef enum {
    AMBA_IK_STILL_LINEAR    = 0u,
    AMBA_IK_STILL_LINEAR_CE = 1u,
    AMBA_IK_STILL_HDR2x     = 2u,
    AMBA_IK_STILL_HDR3x     = 3u,
} AMBA_IK_STILL_PIPE_MODE;

#define AMBA_IK_ARCH_HARD_INIT 0U
#define AMBA_IK_ARCH_SOFT_INIT 1U

typedef struct {
    UINT8 Pipe;
    UINT32 StillPipe;
    UINT32 VideoPipe;
} AMBA_IK_ABILITY_s;

typedef struct {
    UINT32 IkId;
    void *pIsoCfg;
} AMBA_IK_EXECUTE_CONTAINER_s;

#endif
