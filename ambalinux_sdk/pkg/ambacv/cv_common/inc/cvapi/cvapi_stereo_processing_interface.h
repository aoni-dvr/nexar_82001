/*
* Copyright (c) 2017-2017 Ambarella, Inc.
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
#ifndef CVAPI_STEREOPROC_INTERFACE_H_FILE
#define CVAPI_STEREOPROC_INTERFACE_H_FILE

#include <cvapi_common.h>

/* message types */
#define M_SPU_CONFIG_USER           0x00000001
#define M_SPU_MAX_CONFIG            0x0000ffff

#if 0
#define SPU_MAX_INPUT_WIDTH        (640 + 10)
#define SPU_MAX_INPUT_HEIGHT       (480 + 10)
#else
#define SPU_MAX_INPUT_WIDTH        (1920 + 6)
#define SPU_MAX_INPUT_HEIGHT       (1088 + 6)
#endif

/* status code */
#define M_SP_STATUS_OK              (0)
#define M_SP_STATUS_UNKNOWN_ERROR   (-1)
#define M_SP_STATUS_DISABLED        (-2)
#define M_SP_STATUS_BAD_CONFIG      (-3)
#define M_SP_STATUS_INPUT_TOO_LARGE (-4)

/**
 * @file    cvtask_sample_stereo_processing_interface.h
 * @brief   the header file contains config and output interfaces of the sample
 *          stereo processing task.
 */

/**
 *  spu_debug_header
 *  @brief
 *  Store the config information from SPU. Used by the bin parser.
 */
typedef struct spu_debug_header {
    uint32_t    m_base_address;
    uint32_t    m_max_num_scales;
    uint32_t    m_max_height;
    uint32_t    m_max_width;
    uint32_t    vp_mp_pic_info; /**< for debugging purpose. you should not rely on this */
} spu_debug_header;

/**
 *  spu_config_header
 *  @brief
 *  configuration header used in each configuration messages.
 *  Note that the header has to be the first element of the configuration
 *  message.
 */
typedef struct spu_config_header {
    uint32_t    m_message_type;
} spu_config_header;

/**
 *  spu_roi_config
 *  @brief
 *  Define the ROI region. Note that the coordinates are relative to the
 *  input cropped IDSP images.
 */
typedef struct spu_roi_config {

    uint16_t    m_start_row;
    uint16_t    m_start_col;
    uint16_t    m_width_m1;
    uint16_t    m_height_m1;

} spu_roi;

/**
 *  spu_user_scale_config
 *  @brief  The stereo processing configuration structure contains several
 *          user-configurable parameters. These params should be provided in
 *          the initialization phase. They can also be changed afterwards
 *          during the execustion.
 */
typedef struct spu_user_scale_config {

    uint32_t  m_scale_enable    : 1;      /* bool */
    uint32_t  m_roi_en          : 1;
    uint32_t  m_downscale       : 1;      /* bool */
    uint32_t  m_average         : 1;      /* bool */
    uint32_t  m_smoothing       : 1;      /* bool */
    uint32_t  m_upscale         : 1;      /* bool */
    uint32_t  m_despeckle       : 1;      /* bool */

    uint32_t  m_despeckle_th    : 8;
    uint32_t  _reserved_        : 17;

    spu_roi   m_roi;  /* valid when roi_en = 1 */

} spu_user_scale_config;

/**
 *  spu_config_header
 *  @brief
 *  configuration header used in each configuration messages.
 *  Note that the header has to be the first element of the configuration
 *  message.
 */
typedef struct spu_public_config {
    spu_config_header       m_header;
    spu_user_scale_config   m_config[MAX_HALF_OCTAVES];
} spu_public_config;

/**
 *  stereo_processing_scale_output
 *  @brief  The stereo processing output structure contains the output
 *          information of the stereo processing output.
 */
typedef struct stereo_processing_scale_output {

    int32_t     m_status;                   /**< Status code indicating if the result is valid.
                                                 (POS/ZERO: number of disparity points, NEG: error code) */
    uint32_t    m_buf_size;                 /**< The size of the mp_disparity_map in bytes */
    uint32_t    m_disparity_height;         /**< The height of the disparity map */
    uint32_t    m_disparity_width;          /**< The width of the disparity map */
    uint32_t    m_disparity_pitch;          /**< The pitch(actual value, not divided by 32) */
    uint32_t    m_roi_start_row;            /**< the start row of ROI */
    uint32_t    m_roi_start_col;            /**< the start col of ROI */
    relative_ptr_t  mp_disparity_map;       /* uint8[SPU_MAX_OUTPUT_SIZE];  */ /**< Actual data buffer pointer in DRAM */

} stereo_processing_scale_output;

/**
 *  stereo_processing_output
 *  @brief  The stereo processing output structure contains the output
 *          information of the stereo processing output.
 */
typedef struct stereo_processing_output {

    spu_debug_header m_buf_header;      /**< for bin parser use only */
    stereo_processing_scale_output  m_scales[MAX_HALF_OCTAVES]; /**< output for each scale */

} stereo_processing_output;


#define TASKNAME_STEREO_PROCESSING        "STEREO_PROCESSING"

/**
 * Stereo processing CVTask output will be placed in a buffer
 * that can be referenced with the io_name
 * IONAME_STEREO_PROCESSING_OUTPUT
 */
#define IONAME_STEREO_PROCESSING_OUTPUT   "SP_DISPARITY_MAP"


#endif  /* CVAPI_STEREOPROC_INTERFACE_H_FILE */

