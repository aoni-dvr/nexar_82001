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
#ifndef CVAPI_FLEXIDAG_AMBASPUFEX_DEF_H
#define CVAPI_FLEXIDAG_AMBASPUFEX_DEF_H

/**
 * @defgroup API_VisionDriver filter api
 * @brief 
 */

/**
 * @addtogroup API_VisionDriver api for vision driver control
 * @{
 */
#include "cvapi_idsp_interface.h"
#include "cvapi_memio_interface.h"
#include "cvapi_flexidag.h"

#if defined(CONFIG_BUILD_CV_THREADX) || defined(CONFIG_QNX) || defined(CONFIG_LINUX)
#include "AmbaTypes.h"
#else
#include "rtos/AmbaTypes.h"
#endif

#define TASKNAME_STEREO_THROTTLER     "STEREO_THROTTLER"

#define AMBA_STEREO_FD_HDLR_SIZE (6000U)
typedef UINT8 AMBA_STEREO_FD_HANDLE_s[AMBA_STEREO_FD_HDLR_SIZE];

/*****************************Image Info*****************************/
typedef struct {
    memio_source_recv_picinfo_t    *pBuffer;
    ULONG                           BufferDaddr;
    UINT32                          BufferCacheable :1;
    UINT32                          BufferSize      :31;
    ULONG                           BufferCaddr;
    UINT32                          Reserved;
} AMBA_CV_IMG_BUF_s;

/*****************************SPU*****************************/
#define AMBA_CV_SPU_NORMAL_MODE     (0U)
#define AMBA_CV_SPU_DISPLAY_MODE    (1U)

//config
typedef struct {
    UINT8           Enable;
    UINT8           RoiEnable;                   /**< whether to apply an ROI to the input */
    roi_t           Roi;                         /**< input image ROI */
    UINT8           FusionDisparity;             /**< output for 3 layer fusion scale[0] */
} __attribute__((packed)) AMBA_CV_SPU_SCALE_CFG_s;

//output
typedef struct {
    INT32            Status;                    /**< Status code indicating if the result is valid. (NEG: error code) */
    UINT32           BufSize;                   /**< The size of the DisparityMap in bytes */
    UINT32           DisparityHeight;           /**< The height of the disparity map */
    UINT32           DisparityWidth;            /**< The width of the disparity map */
    UINT32           DisparityPitch;            /**< The pitch(actual value, not divided by 32) */
    UINT8            DisparityBpp;              /**< The disparity bit per pixel */
    UINT8            DisparityQm;               /**< The disparity integer bit per pixel */
    UINT8            DisparityQf;               /**< The disparity frational bit per pixel */
    UINT8            Reserved_0;

    UINT32           RoiStartRow;               /**< the start row of ROI */
    UINT32           RoiStartCol;               /**< the start col of ROI */
    UINT32           Reserved_1;
    UINT32           RoiAbsoluteStartCol;       /**< the absolute start col of ROI */
    UINT32           RoiAbsoluteStartRow;       /**< the absolute start row of ROI */
    UINT32           InvalidDisparities;        /**< The number of invalid disparities */
    relative_ptr_t   DisparityMapOffset;        /**< Disparity map buffer offset in bytes, relative to beginning of current spu_out_t instance address */
    UINT32           Reserved_2;
    UINT32           Reserved_3;
} AMBA_CV_SPU_SCALE_OUT_s;

typedef struct {
    UINT32                       Reserved_0[5];
    AMBA_CV_SPU_SCALE_OUT_s      Scales[MAX_HALF_OCTAVES];      /**< output for each scale */
    UINT32                       Reserved_1;
} AMBA_CV_SPU_DATA_s;

typedef struct {
    AMBA_CV_SPU_DATA_s         *pBuffer;
    ULONG                       BufferDaddr;
    UINT32                      BufferCacheable :1;
    UINT32                      BufferSize      :31;
    ULONG                       BufferCaddr;
    UINT32                      Reserved;
} AMBA_CV_SPU_BUF_s;

/*****************************FEX*****************************/
#define CV_FEX_MAX_BUCKETS      (64U)
#define CV_FEX_MAX_KEYPOINTS    (32U)
#define CV_FEX_KEYPOINTS_SIZE   (4U)  /* 16-bit x 16-bit y (+ 16-bit score + padding) */
#define CV_FEX_DESCRIPTOR_SIZE  (32U)

#define CV_FEX_DISABLE       0U
#define CV_FEX_ENABLE_LEFT   1U
#define CV_FEX_ENABLE_RIGHT  2U
#define CV_FEX_ENABLE_ALL    (CV_FEX_ENABLE_LEFT|CV_FEX_ENABLE_RIGHT)

//config
typedef struct {
    UINT8           Enable;                         /**< scale enable. Valid options: CV_FEX_ENABLE_ALL,CV_FEX_ENABLE_LEFT,CV_FEX_ENABLE_RIGHT*/
    UINT8           RoiEnable;                      /**< whether to apply a ROI to the input images*/
    roi_t           Roi;                            /**< primary (reference) image ROI */
    INT16          SecondaryRoiColOffset;           /**< secondary (target) image ROI col offset */
    INT16          SecondaryRoiRowOffset;           /**< secondary (target) image ROI row offset */
} __attribute__((packed)) AMBA_CV_FEX_SCALE_CFG_s;

typedef struct {
    UINT8          Enable;                          /**< scale enable. Valid options: CV_FEX_ENABLE_ALL,CV_FEX_ENABLE_LEFT,CV_FEX_ENABLE_RIGHT*/
    UINT8          RoiEnable;                       /**< whether to apply a ROI to the input images*/
    roi_t          Roi;                             /**< primary (reference) image ROI */
    INT16          SecondaryRoiColOffset;           /**< secondary (target) image ROI col offset */
    INT16          SecondaryRoiRowOffset;           /**< secondary (target) image ROI row offset */
    UINT16         KeypointsTh;                     /**< keypoint extraction threshold  12.4 format*/
    UINT8          FeaturePreBlock;                 /**< number of features per bucket; range: [1-32]. For fma usage, set [4,8,16,24,32] */
    UINT8          SmoothFilterSize;                /**< smoothing filter size */
    UINT8          NmsRadius;                       /**< non-maxima suppression radius */
} __attribute__((packed)) AMBA_CV_FEX_SCALE_CFG_V1_s;

//output
typedef struct {
    UINT16 X;
    UINT16 Y;
} AMBA_CV_FEX_KEYPOINT_s;

typedef struct {
    UINT32 Enable;
    UINT32 HalfOctaveId;
    UINT32 Reserved;
    relative_ptr_t      KeypointsCountOffset;  /* uint8_t* pointer to list of valid points for every block_num [CV_FEX_MAX_BUCKETS]*/
    relative_ptr_t      KeypointsOffset;       /* AMBA_CV_FEX_KEYPOINT_s* pointer to starting block [CV_FEX_MAX_BUCKETS][CV_FEX_MAX_KEYPOINTS] */
    relative_ptr_t      DescriptorsOffset;     /* uint8_t* pointer to list of descriptors for every block_num [CV_FEX_MAX_BUCKETS][CV_FEX_MAX_KEYPOINTS][CV_FEX_DESCRIPTOR_SIZE] */
    relative_ptr_t      KeypointsScoreOffset;  /* uint16_t* pointer to starting block [CV_FEX_MAX_BUCKETS][CV_FEX_MAX_KEYPOINTS], first 12 bit for mantissa, last 4 bit for exponent */
} AMBA_CV_FEX_FEATURE_LIST_s;

typedef struct {
    AMBA_CV_FEX_FEATURE_LIST_s PrimaryList[MAX_HALF_OCTAVES];       /**< output for each scale */
    AMBA_CV_FEX_FEATURE_LIST_s SecondaryList[MAX_HALF_OCTAVES];     /**< output for each scale */
    UINT32    Reserved;
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
    idsp_pyramid_t              PyramidInfo;
    AMBA_CV_SPU_SCALE_CFG_s     SpuScaleCfg[MAX_HALF_OCTAVES];
    AMBA_CV_FEX_SCALE_CFG_s     FexScaleCfg[MAX_HALF_OCTAVES];
} __attribute__((packed)) AMBA_CV_SPUFEX_CFG_s;

typedef struct {
    idsp_pyramid_t              PyramidInfo;
    AMBA_CV_SPU_SCALE_CFG_s     SpuScaleCfg[MAX_HALF_OCTAVES];
    AMBA_CV_FEX_SCALE_CFG_V1_s  FexScaleCfg[MAX_HALF_OCTAVES];
} __attribute__((packed)) AMBA_CV_SPUFEX_CFG_V1_s;

/*****************************Fex Only Cfg*****************************/
typedef struct {
    idsp_pyramid_t              PyramidInfo;
    AMBA_CV_FEX_SCALE_CFG_s     FexScaleCfg[MAX_HALF_OCTAVES];
} __attribute__((packed)) AMBA_CV_FEX_CFG_s;

typedef struct {
    idsp_pyramid_t              PyramidInfo;
    AMBA_CV_FEX_SCALE_CFG_V1_s     FexScaleCfg[MAX_HALF_OCTAVES];
} __attribute__((packed)) AMBA_CV_FEX_CFG_V1_s;

/**
 * @}
 */
#endif //CVAPI_FLEXIDAG_AMBASPUFEX_DEF_H

