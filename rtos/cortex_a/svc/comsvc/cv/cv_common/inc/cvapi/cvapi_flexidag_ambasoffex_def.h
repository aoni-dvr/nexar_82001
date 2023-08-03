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
#ifndef CVAPI_FLEXIDAG_AMBASOFFEX_DEF_H
#define CVAPI_FLEXIDAG_AMBASOFFEX_DEF_H
#include "AmbaTypes.h"

#if defined(CONFIG_SOC_CV2)
#include "cvapi_flexidag_ambaspufex_def_cv2.h"
#elif defined(CONFIG_SOC_CV2FS)
#include "cvapi_flexidag_ambaspufex_def_cv2fs.h"
#else
#include "cvapi_flexidag_ambaspufex_def_sw.h"
#endif

#define MAX_VP_SOF_FEX_WIDTH    (1280U)
#define MAX_VP_SOF_FEX_HEIGHT   (720U)
#define MAX_VP_SOF_FEX_INSTANCE (3U)

static inline UINT32 SOF_FEX_ALIGN32(UINT32 X)
{
    return ((X + 31U) & 0xFFFFFFE0U);
}
//static inline UINT32 SOF_FEX_ALIGN64(UINT32 X) {return ((X + 63U) & 0xFFFFFFC0U);}
static inline UINT32 SOF_FEX_ALIGN128(UINT32 X)
{
    return ((X + 127U) & 0xFFFFFF80U);
}

#define ALLOC_HARRIS_BIT_MAP_OUT_PH         (MAX_VP_SOF_FEX_WIDTH/8U)
#define ALLOC_HARRIS_BIT_MAP_OUT_PH_32      ((ALLOC_HARRIS_BIT_MAP_OUT_PH+31U) & 0xFFFFFFE0U)
#define ALLOC_HARRIS_BIT_MAP_OUT_SZ         (ALLOC_HARRIS_BIT_MAP_OUT_PH_32*MAX_VP_SOF_FEX_HEIGHT)

#define ALLOC_HARRIS_RESP_OUT_PH            (sizeof(UINT16)*MAX_VP_SOF_FEX_WIDTH)
#define ALLOC_HARRIS_RESP_OUT_PH_32         ((ALLOC_HARRIS_RESP_OUT_PH+31U)& 0xFFFFFFE0U)
#define ALLOC_HARRIS_RESP_OUT_SZ            (ALLOC_HARRIS_RESP_OUT_PH_32*MAX_VP_SOF_FEX_HEIGHT)

#define ALLOC_LK_DX_DY_PH                   (sizeof(UINT16)*MAX_VP_SOF_FEX_WIDTH)
#define ALLOC_LK_DX_DY_PH_32                ((ALLOC_LK_DX_DY_PH+31U)& 0xFFFFFFE0U)
#define ALLOC_LK_DX_DY_SZ                   (ALLOC_LK_DX_DY_PH_32*MAX_VP_SOF_FEX_HEIGHT)

#define LK_INVALID_ARRAY_SIZE               (64U)
#define ALLOC_LK_INVALID_COUNT_PH           (sizeof(UINT16)*LK_INVALID_ARRAY_SIZE)
#define ALLOC_LK_INVALID_COUNT_SZ           ((ALLOC_LK_INVALID_COUNT_PH+31U)& 0xFFFFFFE0U)

#define BRIEF_PAIRS_NUM                     (CV_FEX_DESCRIPTOR_SIZE*8U)
#define BRIEF_RADIUS                        (14U)

#define VP_SOF_FEX_LEFT                     (0U)
#define VP_SOF_FEX_RIGHT                    (1U)

#define SOF_FEX_INTERNAL_BUFF_PAD_SIZE      (127U)

typedef struct {
    UINT16  X;      // 14.2 format
    UINT16  Y;      // 14.2 format
    UINT32  Score;
} SOF_FEX_KEYPOINT_SCORE_s;

typedef struct {
    // VP out
    UINT8 BitOutMap[ALLOC_HARRIS_BIT_MAP_OUT_SZ+SOF_FEX_INTERNAL_BUFF_PAD_SIZE];
    UINT8 RespMap[ALLOC_HARRIS_RESP_OUT_SZ+SOF_FEX_INTERNAL_BUFF_PAD_SIZE];
    UINT8 *pAlignedBitOutMapPtr;
    UINT8 *pAlignedRespMapPtr;

    // Work buffer
    UINT8 RecvRawBuff[sizeof(memio_source_recv_raw_t)+SOF_FEX_INTERNAL_BUFF_PAD_SIZE];
    UINT8 *pAlignedRecvRawBuffPtr;

    INT32 BriefDesc_APosLUT[BRIEF_PAIRS_NUM];
    INT32 BriefDesc_BPosLUT[BRIEF_PAIRS_NUM];

    // SOF Interface out
    UINT8 OutKeypointsCount[2U][CV_FEX_MAX_BUCKETS];
    SOF_FEX_KEYPOINT_SCORE_s OutKeyPoints[2U][CV_FEX_MAX_BUCKETS][CV_FEX_MAX_KEYPOINTS];
    UINT8 OutDescriptors[2U][CV_FEX_MAX_BUCKETS][CV_FEX_MAX_KEYPOINTS][CV_FEX_DESCRIPTOR_SIZE];
} SOF_FEX_HARRIS_BUFF_s;

typedef struct {
    // VP out
    UINT8 InvalidCount[ALLOC_LK_INVALID_COUNT_SZ+SOF_FEX_INTERNAL_BUFF_PAD_SIZE];
    UINT8 *pAlignedInvalidCountPtr;

    // Work buffer
    UINT8 LastRecvRawBuff[sizeof(memio_source_recv_raw_t)+SOF_FEX_INTERNAL_BUFF_PAD_SIZE];
    UINT8 CurRecvRawBuff[sizeof(memio_source_recv_raw_t)+SOF_FEX_INTERNAL_BUFF_PAD_SIZE];
    UINT8 *pAlignedLastRecvRawPtr;
    UINT8 *pAlignedCurRecvRawPtr;

    // SOF Interface out
    UINT8 DxDy[ALLOC_LK_DX_DY_SZ+SOF_FEX_INTERNAL_BUFF_PAD_SIZE];
    UINT8 *pAlignedDxDyPtr;
} SOF_FEX_LK_OPC_FW_BUFF_s;

typedef struct {
    REF_FD_HANDLE_s *pHarrisHandler[MAX_VP_SOF_FEX_INSTANCE];
    REF_FD_HANDLE_s *pLKOpcFlowHandler[MAX_VP_SOF_FEX_INSTANCE];
    AMBA_CV_SPUFEX_CFG_s Config;
    AMBA_CV_FEX_DATA_s OutFexData;
    AMBA_CV_SPU_DATA_s OutSofData;
    SOF_FEX_HARRIS_BUFF_s HarrisBuff[MAX_VP_SOF_FEX_INSTANCE];
    SOF_FEX_LK_OPC_FW_BUFF_s LKOpcFwBuff[MAX_VP_SOF_FEX_INSTANCE];
    UINT32 RegisterHdlrNum;
    UINT16 ProcWidth[MAX_VP_SOF_FEX_INSTANCE];
    UINT16 ProcHeight[MAX_VP_SOF_FEX_INSTANCE];
} SOF_FEX_FD_HANDLE_s;

UINT32 SofFex_InitBriefDescPosLUT(UINT32 ImagePitch,
                                  UINT32 DimIndex, SOF_FEX_FD_HANDLE_s *pHandler);

UINT32 SofFex_GetProcVPInstIdx(const SOF_FEX_FD_HANDLE_s *pHandler,
                               const memio_source_recv_picinfo_t *pInPicInfo,
                               UINT32 InPicInfoOctaveIndex,
                               UINT32 RoiEnable, const roi_t *pRoi,
                               UINT32 *pOutProcVPInstIdx);

UINT32 SofFex_SetWorkBuffAligned(SOF_FEX_FD_HANDLE_s *pHandler);

UINT32 SofFex_HarrisCornerDetProc(SOF_FEX_FD_HANDLE_s *pHandler, const AMBA_CV_IMG_BUF_s *pImgInfo);
UINT32 SofFex_LKOpticalFlowProc(SOF_FEX_FD_HANDLE_s *pHandler, const AMBA_CV_IMG_BUF_s *pImgInfo);
#endif

