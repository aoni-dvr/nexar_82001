
/*
 * Copyright (c) 2018 Ambarella, Inc.
 * 
 * This file and its contents ("Software") are protected by intellectual property rights including,
 * without limitation, U.S. and/or foreign copyrights.  This Software is also the confidential and
 * proprietary information of Ambarella, Inc. and its licensors.  You may not use, reproduce, disclose,
 * distribute, modify, or otherwise prepare derivative works of this Software or any portion thereof
 * except pursuant to a signed license agreement or nondisclosure agreement with Ambarella, Inc. or
 * its authorized affiliates.  In the absence of such an agreement, you agree to promptly notify and
 * return this Software to Ambarella, Inc.
 * 
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef CVAPI_FEX_SP_INTERFACE_H_FILE
#define CVAPI_FEX_SP_INTERFACE_H_FILE

/**
 * @file    cvapi_fex_sp_interface.h
 * @brief   the header file contains config and output interfaces of the stereo processing cvtask.
 */

#include <cvapi_common.h>

#define TASKNAME_FEX_SP        "FEX_SP"

/* status code */
#define SPU_STATUS_OK                       (0)
#define SPU_STATUS_UNKNOWN_ERROR            (-1)
#define SPU_STATUS_DISABLED                 (-2)
#define SPU_STATUS_BAD_CONFIG               (-3)
#define SPU_STATUS_INPUT_TOO_LARGE          (-4)


#define SPU_HISTOGRAM_VALID     (1)
#define SPU_HISTOGRAM_INVALID     (0)

/**
 *  spu_config_header_t
 *  @brief
 *  configuration header used in each configuration messages.
 *  Note that the header has to be the first element of the configuration
 *  message.
 */
typedef struct {
    uint32_t    m_message_type;
} spu_fex_header_t;


/************************* SPU ******************************/

/**
 * Stereo processing CVTask output will be placed in a buffer
 * that can be referenced with the io_name
 * IONAME_SP_OUTPUT
 */
#define IONAME_SP_OUTPUT   "SP_DISPARITY_MAP"

#define SP_CONFIG                     0x00000001
#define SP_MAX_CONFIG                 0x0000ffff

#define SP_FIELD_ENABLE                 (1 << 1)
#define SP_FIELD_ROI_ENABLE             (1 << 7)
#define SP_FIELD_ROI                    (1 << 13)
#define SP_FIELD_ALL                    (SP_FIELD_ENABLE| SP_FIELD_ROI_ENABLE| SP_FIELD_ROI)

/**
 *  spu_scale_config_t
 *  @brief  The stereo processing configuration structure contains several
 *          user-configurable parameters. These params should be provided in
 *          the initialization phase. They can also be changed afterwards
 *          during the execution.
 */
typedef struct {

    uint32_t m_enable              : 1;  
    uint32_t m_reserved_0          : 5;  
    uint32_t m_roi_enable          : 1;       /**< whether to apply an ROI to the input */ 
    uint32_t m_reserved_1          : 25;  
    uint32_t m_reserved_2; 
    uint32_t m_reserved_3;
    uint32_t m_reserved_4;      
    uint32_t m_reserved_5;
    roi_t  m_roi;                             /**< input image ROI */
    uint32_t m_field_mask;                    /**< mask to mark which fields have been filled */
} spu_scale_config_t;

/**
 *  spu_config_t
 *  @brief
 *  configuration header used in each configuration messages.
 *  Note that the header has to be the first element of the configuration
 *  message.
 */
typedef struct {

	spu_fex_header_t  m_header;
	spu_scale_config_t   m_scale_config[MAX_HALF_OCTAVES];
} spu_config_t;


#define SP_HISTOGRAM_SIZE       0x500
#define SP_BUCKET_SHIFT_SIZE    64

/**
 *  spu_scale_out_t
 *  @brief  The stereo processing output structure contains the output
 *          information of the stereo processing output.
 */
typedef struct {

    int32_t     m_status;                   /**< Status code indicating if the result is valid. (NEG: error code) */
    uint32_t    m_buf_size;                 /**< The size of the mp_disparity_map in bytes */
    uint32_t    m_disparity_height;         /**< The height of the disparity map */
    uint32_t    m_disparity_width;          /**< The width of the disparity map */
    uint32_t    m_disparity_pitch;          /**< The pitch(actual value, not divided by 32) */
    uint32_t    m_disparity_bpp   :8;      /**< The disparity bit per pixel */
    uint32_t    m_disparity_qm    :8;       /**< The disparity integer bit per pixel */
    uint32_t    m_disparity_qf    :8;       /**< The disparity frational bit per pixel */
    uint32_t    m_reserved_0      :8;
    uint32_t    m_roi_start_row;            /**< the start row of ROI */
    uint32_t    m_roi_start_col;            /**< the start col of ROI */
    uint32_t    m_histogram_valid;          /**< Status code indicating if the histogram and bucket_shift are available*/ 
    uint32_t    m_roi_absolute_start_col;   /**< the absolute start col of ROI */
    uint32_t    m_roi_absolute_start_row;   /**< the absolute start row of ROI */
    uint32_t    m_invalid_disparities;      /**< The number of invalid disparities */         
    relative_ptr_t  mp_disparity_map;       /**< Disparity map buffer offset in bytes, relative to beginning of current spu_out_t instance address */
    relative_ptr_t  mp_stereo_histogram;    /**< [SP_HISTOGRAM_SIZE] Stereo histogram buffer offset in bytes, relative to beginning of current spu_out_t instance address */
    relative_ptr_t  mp_bucket_shift;        /**< [SP_BUCKET_SHIFT_SIZE]Bucket shift buffer offset in bytes, relative to beginning of current spu_out_t instance address*/
} spu_scale_out_t;

/**
 *  spu_debug_header_t
 *  @brief
 *  Store the config information from SPU. Used by the bin parser.
 */
typedef struct {

        uint32_t    m_base_address;
        uint32_t    m_max_num_scales;
        uint32_t    m_max_height;
        uint32_t    m_max_width;
        uint32_t    vp_mp_pic_info; /**< for debugging purpose. you should not rely on this */
} spu_debug_header_t;

/**
 *  spu_out_t
 *  @brief  The stereo processing output structure contains the output
 *          information of the stereo processing output.
 */
typedef struct {

    spu_debug_header_t   m_buf_header;                  /**< for bin parser use only */
    spu_scale_out_t      m_scales[MAX_HALF_OCTAVES];    /**< output for each scale */
    uint32_t             errorcode;                     /**< errorcode, must be set to 0 if no errors */
} spu_out_t;


/************************* FEX ******************************/

/**
 * FEX CVTask output will be placed in a buffer
 * that can be referenced with the io_name
 * IONAME_FEX_OUTPUT
 */
#define IONAME_FEX_OUTPUT   "FEX_FEATURES_LIST"

#define FEX_CONFIG          0xFEFE001U

#define FEX_MAX_BUCKETS (64)
#define FEX_MAX_KEYPOINTS  (32)
#define FEX_KEYPOINTS_SIZE (4)  /* 16-bit x 16-bit y (+ 16-bit score + padding) */
#define FEX_SCORE_SIZE (2)	// 16 bit score
#define FEX_DESCRIPTOR_SIZE  (32)

#define FEX_FIELD_ALL                       (0xFFFFFFFFU)
#define FEX_FIELD_ENABLE                    (1)
#define FEX_FIELD_KEYPOINTS_TH              (1 << 2)
#define FEX_FIELD_HOR_BLOCKS                (1 << 3)
#define FEX_FIELD_VER_BLOCKS                (1 << 4)
#define FEX_FIELD_FEATURES_PER_BLOCKS       (1 << 5)
#define FEX_FIELD_SMOOTHING_FILTER_SIZE     (1 << 6)
#define FEX_FIELD_NMS_RADIUS                (1 << 7)
#define FEX_FIELD_ROI_ENABLE                (1 << 8)
#define FEX_FIELD_ROI                       (1 << 9)
#define FEX_FIELD_SECONDARY_ROI_COL_OFF     (1 << 10)
#define FEX_FIELD_SECONDARY_ROI_ROW_OFF     (1 << 11)

#define FEX_ENABLE_LEFT  1
#define FEX_ENABLE_RIGHT  2
#define FEX_ENABLE_ALL (FEX_ENABLE_LEFT|FEX_ENABLE_RIGHT)

typedef struct {

    uint16_t m_enable;                       /**< scale enable. Valid options: FEX_ENABLE_ALL,FEX_ENABLE_LEFT,FEX_ENABLE_RIGHT*/

    uint16_t m_keypoints_th;                /**< keypoint extraction threshold  12.4 format*/
    uint8_t  m_hor_blocks;                   /**< number of horizontal buckets to use; range: [1-8] */
    uint8_t  m_ver_blocks;                   /**< number of vertical buckets to use; range: [1-8] */
    uint8_t  m_features_per_block;           /**< number of features per bucket; range: [1-32] */
    uint8_t  m_smoothing_filter_size;        /**< smoothing filter size */
    uint8_t  m_nms_radius;                   /**< non-maxima suppression radius */
    uint8_t  m_roi_enable;                   /**< whether to apply a ROI to the input images*/ 
	uint16_t padding;
    roi_t    m_roi;                          /**< primary (reference) image ROI */
    int16_t  m_secondary_roi_col_offset;     /**< secondary (target) image ROI col offset */
    int16_t  m_secondary_roi_row_offset;     /**< secondary (target) image ROI row offset */
    uint32_t m_field_mask;                  /**< mask to mark which fields have been filled */
} fex_scale_config_t;

/* Public Configuration State */
typedef struct {

    spu_fex_header_t  m_header;
    fex_scale_config_t m_scale_config[MAX_HALF_OCTAVES];
} fex_config_t;


//#undef REPACK_HARRIS_SCORES
#define REPACK_HARRIS_SCORES
typedef struct {

    uint32_t m_enable;

    uint32_t m_half_octave_id;

    uint8_t m_hor_blocks;                   /**< number of horizontal buckets to use; range: [1-8] */
    uint8_t m_ver_blocks;                   /**< number of vertical buckets to use; range: [1-8] */
    uint8_t m_features_per_block;           /**< number of features per bucket; range: [1-32] */
    uint8_t m_reserved_0;		    /**< 0: no scores, 1: repacked XY, scores format */

    relative_ptr_t m_keypoints_count_ptr;     /* uint8_t* pointer to list of valid points for every block_num [max_block_num]*/
    relative_ptr_t m_keypoints_ptr;           /* coord_t* pointer to starting block [max_block_num][max_features_per_block] */
    relative_ptr_t m_descriptors_ptr;         /* uint8_t* pointer to list of descriptors for every block_num [max_block_num][max_features_per_block][brief_desc_size] */
#ifdef REPACK_HARRIS_SCORES
    relative_ptr_t m_keypoint_scores_ptr;     /* uint16_t* pointer to starting block [max_block_num][max_features_per_block] */
#endif
} fex_feature_list_t;

typedef struct {

    fex_feature_list_t m_primary_list[MAX_HALF_OCTAVES];
    fex_feature_list_t m_secondary_list[MAX_HALF_OCTAVES];
    uint32_t    errorcode;                    /*  errorcode, must be set to 0 if no errors */
} fex_out_t;

#endif  /* CVAPI_STEREO_FEX_INTERFACE_H_FILE */
