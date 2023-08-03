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
#ifndef CVAPI_FLEXIDAG_AMBASPUFEX_DEF_SW_H
#define CVAPI_FLEXIDAG_AMBASPUFEX_DEF_SW_H

#include "cvapi_idsp_interface.h"
#include "cvapi_memio_interface.h"
#ifndef CONFIG_BUILD_CV_THREADX
#if defined(CONFIG_QNX)
#include "rtos/AmbaRTOSWrapper.h"
#endif
#endif


/*****************************Image Info*****************************/
typedef struct {
    memio_source_recv_picinfo_t    *pBuffer;
    ULONG                           BufferDaddr;
    UINT32                          BufferCacheable :1;
    UINT32                          BufferSize      :31;
    ULONG                           BufferCaddr;
    UINT32                          Reserved;
} AMBA_CV_IMG_BUF_s;


/*****************************Spu Output*****************************/
typedef struct {
    INT32     Status;                   /**< Status code returned by HW. (POS/ZERO: number of disparity points, NEG: error code) */
    UINT32    BufSize;                 /**< The size of the mp_disparity_map in bytes */
    UINT32    DisparityHeight;         /**< The height of the disparity map */
    UINT32    DisparityWidth;          /**< width of disparity/Optical-Flow/Confidence map in pixels */
    UINT32    DisparityWidthInBytes; /**< The width of the disparity map in bytes */
    UINT32    DisparityPitch;          /**< The pitch(actual value, not divided by 32) */
    UINT32    RoiStartRow;            /**< the start row of ROI */
    UINT32    RoiStartCol;            /**< the start col of ROI */
    UINT32    Reserved_1;
    UINT32    RoiAbsoluteStartRow;   // relative to top of scale, i.e. adds crop rows
    UINT32    RoiAbsoluteStartCol;   // relative to left edge of scale, i.e. adds crop cols
    UINT32    InvalidDisparities;      // count of invalid disparity values, updated by DiFu
    UINT8    DisparityBpp;       /**< For DiFu output: The disparity bit per pixel */
    UINT8    DisparityQm;       /**< For DIFu output: The disparity integer bit per pixel */
    UINT8    DisparityQf;       /**< For DiFu output: The disparity fractional bit per pixel */
    UINT8    Reserved_0;
    relative_ptr_t  DisparityMapOffset;       /* uint8[SPU_MAX_OUTPUT_SIZE];  */ /**< Actual data buffer pointer in DRAM */
    UINT32  Reserved_2;
    UINT32  Reserved_3;
    UINT32  Reserved_4;
    UINT32  Reserved_5;
    UINT32  Reserved_6;
} AMBA_CV_SPU_SCALE_OUT_s;

typedef struct {
    UINT32                      Reserved_0[6];
    AMBA_CV_SPU_SCALE_OUT_s     Scales[MAX_HALF_OCTAVES]; /**< output for each scale */
    UINT32                      Reserved_1;
    UINT32                      Reserved_2;
} AMBA_CV_SPU_DATA_s;

typedef struct {
    AMBA_CV_SPU_DATA_s         *pBuffer;
    ULONG                       BufferDaddr;
    UINT32                      BufferCacheable :1;
    UINT32                      BufferSize      :31;
    ULONG                       BufferCaddr;
    UINT32                      Reserved;
} AMBA_CV_SPU_BUF_s;


/*****************************Fex Output*****************************/
#define CV_FEX_MAX_BUCKETS      (64U)
#define CV_FEX_MAX_KEYPOINTS    (32U)
#define CV_FEX_KEYPOINTS_SIZE   (8U)
#define CV_FEX_DESCRIPTOR_SIZE  (32U)

#define CV_FEX_DISABLE       0U
#define CV_FEX_ENABLE_LEFT   1U
#define CV_FEX_ENABLE_RIGHT  2U
#define CV_FEX_ENABLE_ALL    (CV_FEX_ENABLE_LEFT|CV_FEX_ENABLE_RIGHT)

typedef struct {
    UINT32    Reserved        :15;
    UINT32    ScoreExponent   :5;
    UINT32    ScoreMantissa   :12;
    UINT16    X;      // 14.2 format
    UINT16    Y;      // 14.2 format
} AMBA_CV_FEX_KEYPOINT_SCORE_s;

typedef struct {
    UINT32 Enable;
    UINT32 HalfOctaveId;
    UINT32 Reserved;
    relative_ptr_t      KeypointsCountOffset;  /* UINT8* pointer to list of valid points for every block_num [CV_FEX_MAX_BUCKETS]*/
    relative_ptr_t      KeypointsOffset;       /* AMBA_CV_FEX_KEYPOINT_SCORE_s* pointer to starting block [CV_FEX_MAX_BUCKETS][CV_FEX_MAX_KEYPOINTS] */
    relative_ptr_t      DescriptorsOffset;     /* UINT8* pointer to list of descriptors for every block_num [CV_FEX_MAX_BUCKETS][CV_FEX_MAX_KEYPOINTS][CV_FEX_DESCRIPTOR_SIZE] */
} AMBA_CV_FEX_FEATURE_LIST_s;

typedef struct {
    UINT32  NumHalfOctaves;
    AMBA_CV_FEX_FEATURE_LIST_s PrimaryList[MAX_HALF_OCTAVES];       /**< output for each scale */
    AMBA_CV_FEX_FEATURE_LIST_s SecondaryList[MAX_HALF_OCTAVES];     /**< output for each scale */
} AMBA_CV_FEX_DATA_s;

typedef struct {
    AMBA_CV_FEX_DATA_s         *pBuffer;
    ULONG                       BufferDaddr;
    UINT32                      BufferCacheable :1;
    UINT32                      BufferSize      :31;
    ULONG                       BufferCaddr;
    UINT32                      Reserved;
} AMBA_CV_FEX_BUF_s;


/*****************************SpuFex Cfg*****************************/
typedef struct {
    UINT8           Enable;
    UINT8           RoiEnable;                   /**< whether to apply an ROI to the input */
    roi_t           Roi;                         /**< input image ROI */
    UINT8           FusionDisparity;             /**< output for 3 layer fusion scale[0] */
} __attribute__((packed)) AMBA_CV_SPU_SCALE_CFG_s;

typedef struct {
    UINT8           Enable;                         /**< scale enable. Valid options: CV_FEX_ENABLE_ALL,CV_FEX_ENABLE_LEFT,CV_FEX_ENABLE_RIGHT*/
    UINT8           RoiEnable;                      /**< whether to apply a ROI to the input images*/
    roi_t           Roi;                            /**< primary (reference) image ROI */
    INT16           SecondaryRoiColOffset;           /**< secondary (target) image ROI col offset */
    INT16           SecondaryRoiRowOffset;           /**< secondary (target) image ROI row offset */
} __attribute__((packed)) AMBA_CV_FEX_SCALE_CFG_s;

typedef struct {
    AMBA_CV_SPU_SCALE_CFG_s     SpuScaleCfg[MAX_HALF_OCTAVES];
    AMBA_CV_FEX_SCALE_CFG_s     FexScaleCfg[MAX_HALF_OCTAVES];
} __attribute__((packed)) AMBA_CV_SPUFEX_CFG_s;


#endif //CVAPI_FLEXIDAG_AMBASPUFEX_DEF_SW_H

