/**
 *  @AmbaSEG_BufferDef.h
 *
 *  @copyright Copyright (c) 2015 Ambarella, Inc.
 *
 *  This file and its contents ("Software") are protected by intellectual property rights including, without limitation,
 *  U.S. and/or foreign copyrights.  This Software is also the confidential and proprietary information of Ambarella, Inc.
 *  and its licensors.  You may not use, reproduce, disclose, distribute, modify, or otherwise prepare derivative works
 *  of this Software or any portion thereof except pursuant to a signed license agreement or nondisclosure agreement with
 *  Ambarella, Inc. or its authorized affiliates.  In the absence of such an agreement, you agree to promptly notify and
 *  return this Software to Ambarella, Inc.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 *  AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details segmentation buffer definition
 *
 */


#ifndef AMBA_SEG_BUFFER_DEF
#define AMBA_SEG_BUFFER_DEF

#ifndef AMBA_TYPES_H
#include "AmbaTypes.h"
#endif

/** Maximum of idx list number */
#define AMBA_SEG_MAX_IDX_LIST (32U)

/** Type definition */
#define AMBA_SEG_TYPE_LANE                 (0U)
#define AMBA_SEG_TYPE_CURB                 (1U)

#define AMBA_SEG_TYPE_UNDEFINE    (0xFFFFFFFFU)

typedef struct {
    UINT32 SrcW;    /**< Source image width */
    UINT32 SrcH;    /**< Source image height */
    UINT32 RoiW;    /**< ROI width at source image */
    UINT32 RoiH;    /**< ROI height at source image */
    UINT32 RoiX;    /**< ROI X offset at source image */
    UINT32 RoiY;    /**< ROI Y offset at source image */
} AMBA_SEG_CROP_INFO_s;

typedef struct {
    UINT8* pBuf;                         /**< Buffer address */
    UINT32 Pitch;                        /**< Buffer pitch which is the width to change to next image row */
    UINT32 Width;                        /**< Image width */
    UINT32 Height;                       /**< Image height */
    UINT32 TimeStamp;                    /**< Time stamp of detection frame */
    UINT32 FrameNum;                     /**< frame number of detection frame */
    AMBA_SEG_CROP_INFO_s CropInfo;       /**< Crop information of segmentation buffer */
} AMBA_SEG_BUF_INFO_s;

typedef struct {
    UINT32 IdxNum;                                 /**< Total index number for the list */
    UINT32 ClassList[AMBA_SEG_MAX_IDX_LIST];       /**< Class index of segmentation buffer */
    UINT32 TypeList[AMBA_SEG_MAX_IDX_LIST];        /**< Type of the class, please refer to AMBA_SEG_TYPE_XXX */
} AMBA_SEG_CLASS_INFO;

#endif
