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
#ifndef CVAPI_FLEXIDAG_AMBAVO_DEF_H
#define CVAPI_FLEXIDAG_AMBAVO_DEF_H

#include "cvapi_flexidag.h"
#include "cvapi_flexidag_ambaspufex_def_cv2.h"

#define AMBA_CV_CALIB_DATA_SIZE (2784U)// sizeof (CalibrationData_t)
typedef struct {
    unsigned int UpdateMask;
    unsigned int UpdateCount;
    unsigned char Data[AMBA_CV_CALIB_DATA_SIZE];
} __attribute__((packed)) AMBA_CV_META_DATA_s;

typedef struct {
    AMBA_CV_META_DATA_s         *pBuffer;
    ULONG                       BufferDaddr;
    UINT32                      BufferCacheable :1;
    UINT32                      BufferSize      :31;
    ULONG                       BufferCaddr;
    UINT32                      Reserved;
} AMBA_CV_META_BUF_s;

typedef struct {
    uint16_t             RoiX;  //VO is expected to run at full FOV  1080p (assuming FEX is configured the same) and the ROI should be configured accordingly (0,0,1920,1080)
    uint16_t             RoiY;
    uint16_t             RoiWidth;
    uint16_t             RoiHeight;
    uint8_t              Scale; //Scale number corresponding to 1080p (scale 2 on SC5)
} AMBA_CV_VO_VIEW_s;

/**
 *  arm_vo_config_t
 *  @brief The structure contains the control params of the ARM VO module.
 */
typedef struct {
    AMBA_CV_VO_VIEW_s       View;
    uint8_t                 MaxMinimizationIters;     //Default: 8; Range: [5,10]
    uint16_t                ViewMaxMinimizationFeaturesStereo;  //Default: 400; Range: [100,500]
    AMBA_CV_META_BUF_s      *pInMetaRight;
} AMBA_CV_VO_CFG_s;


/******************************************************************************/
/* OUTPUT ARM VO                                                                  */
/******************************************************************************/
typedef struct {
    float         Residual;     /* residual value [px] */
    uint32_t      Inlier;       /* number of inlier */
} AMBA_CV_VO_VIEW_STATS_s;

/**
 *  arm_vo_output_t
 *  @brief  The output data structure of the ARM VO module
 */
typedef struct {
    float      Rotation[3];                        /* rotation model  [axis angle, body coordinates] */
    float      Translation[3];                     /* translation model [body coordinates] */
    float      Covariance[6*6];                    /* covariance matrix [axis angle, body coordinates] */
    uint32_t   Iterations;                         /* number of iterations of minimization process */
    float      Residual;                           /* residual value of minimization process [px] */
    AMBA_CV_VO_VIEW_STATS_s    ViewStat;           /* view statistics */
} AMBA_CV_VO_DATA_s;

typedef struct {
    AMBA_CV_VO_DATA_s           *pBuffer;
    ULONG                       BufferDaddr;
    UINT32                      BufferCacheable :1;
    UINT32                      BufferSize      :31;
    ULONG                       BufferCaddr;
    UINT32                      Reserved;
} AMBA_CV_VO_BUF_s;

#endif //CVAPI_FLEXIDAG_AMBAVO_DEF_H
