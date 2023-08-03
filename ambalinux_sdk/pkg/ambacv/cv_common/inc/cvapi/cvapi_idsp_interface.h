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
#ifndef CVAPI_IDSP_INTERFACE_H_FILE
#define CVAPI_IDSP_INTERFACE_H_FILE

#ifndef CVAPI_COMMON_H_FILE
#include <cvapi_common.h>             /* for MAX_HALF_OCTAVES */
#endif
#include <cvapi_metadata_interface.h> /* for gyro_data - this may be deprecated in a future release */

typedef struct
{
    uint32_t     disable           :1;  /* Scale disable flag, set to 1 to disable this */
    uint32_t     mode              :1;  /* 0: mono, 1: stereo */
    uint32_t     octave_mode       :1;  /* 0: 1/sqrt2, 1: half (CHIP_CV2) */
    uint32_t     luma_only         :1;  /* 0: YUV available, 1: Y only available.  In this case rpChroma(Left/Right) will be zero */
    uint32_t     lu_bit_depth      :2;  /* 00: 8-bit, 01: (reserved), 10: 12-bit (CHIP_CV2A), 11: (reserved) */
    uint32_t     reserved          :2;  /* reserved for ch_bit_depth (if ever needed) */
    uint32_t     mode_unused_flags :8;  /* available_for_use */
    uint32_t     roi_pitch         :16; /* Pitch for the ROI */
} half_octave_ctrl_t;

#define IDSP_BIT_DEPTH_8BIT           0x00U
#define IDSP_BIT_DEPTH_12BIT          0x02U

typedef struct /* half_octave_info_s */
{
    half_octave_ctrl_t  ctrl;
    int16_t             roi_start_row;
    int16_t             roi_start_col;
    uint16_t            roi_height_m1;
    uint16_t            roi_width_m1;
} half_octave_info_t;

typedef struct /* idsp_pyramid_s */
{
    uint16_t    image_height_m1;
    uint16_t    image_width_m1;
    uint32_t    image_pitch_m1;
    half_octave_info_t half_octave[MAX_HALF_OCTAVES];
} idsp_pyramid_t;

typedef struct /* idsp_ld_info_s */
{
    uint16_t    ld_img_height_m1;
    uint16_t    ld_img_width_m1;
    int16_t     ld_roi_start_row;
    int16_t     ld_roi_start_col;
    int16_t     ld_roi_end_row;
    int16_t     ld_roi_end_col;
    uint16_t    ld_roi_height_m1;
    uint16_t    ld_roi_width_m1;
    uint16_t    ld_roi_pitch;
    uint16_t    ld_alignment;
} idsp_ld_info_t;

typedef struct /* cv_pic_info_s */
{
    uint32_t frame_num;
    uint32_t token_id;
    idsp_pyramid_t pyramid;

    /* DRAM locations of special lane-detection image */
    idsp_ld_info_t lane_detect_info;
    uint32_t  rpLaneDetectLuma;
    uint32_t  rpLaneDetectChroma;

    uint32_t  rpLumaLeft[MAX_HALF_OCTAVES];
    uint32_t  rpLumaRight[MAX_HALF_OCTAVES];
    uint32_t  rpChromaLeft[MAX_HALF_OCTAVES];
    uint32_t  rpChromaRight[MAX_HALF_OCTAVES];

    uint32_t  capture_time;
    uint32_t  channel_id          : 8;
    uint32_t  additional_flags    :24;

    uint32_t  num_tokens;
    uint32_t  token_array[16];

} cv_pic_info_t;

/**
 * All IDSP-style CVTasks will have at least one output of type IONAME_CV_PIC_INFO.  This
 * output will contain the cv_pic_info_t, and all of the relative pointers (rp prefixed)
 * will point to parts of the image pyramid within this buffer.  Note that the relative
 * pointers are in reference to the base address of the buffer.
 */
#define IONAME_CV_PIC_INFO        "IDSP_PICINFO"

/**
 * Some IDSP-style CVTaks will also have an output that contains only the cv_pic_info_t.
 * This buffer is a direct copy of the cv_pic_info_t from IONAME_CV_PIC_INFO, but is
 * significantly smaller since it does not bring along with it the actual image.  This can
 * be used to extract specific information about the roi and/or time of the image, without
 * needing the entire output buffer.  Please note that all of the relative pointers will
 * not be changed, and will point outside of the buffer.
 */
#define IONAME_CV_PIC_INFO_ONLY   "IDSP_CVPICINFO_ONLY"

/*
 * IDSP_RX is the task designed to connect either an IDSP_FEEDER, or a live camera to the
 * vision orc scheduler.
 *
 * IDSP_RX currently has three outputs:
 *
 * IONAME_CV_PIC_INFO
 * IONAME_PIC_METADATA
 * IONAME_CV_PIC_INFO_ONLY
 *
 * config[0] : Cvtask_instance_identifier (replaces ### below as a %03d)
 * config[1] : Maximum image width  (overrideable via IDSP_FRAME_WIDTH_###)
 * config[2] : Maximum image height (overrideable via IDSP_FRAME_HEIGHT_###)
 * config[3] : Maximum lane detection output width (overrideable via IDSP_LANEDET_WIDTH_###)
 * config[4] : Maximum lane detection output height (overrideable via IDSP_LANEDET_HEIGHT_###)
 * config[5] : Additional data for meta-buffer
 * config[6] : Additional buffers to allocate for output
 * config[7] : Additional flags:
 *               b0: Stereo
 *               b1: incoherent picture buffer error mode
 *               b2: Enable PIC_INFO output
 *               b3: Enable Asynchronous mode
 *               b4: half_octave_mode
 *            b5-b6: luma bit depth (see IDSP_BIT_DEPTH above)
 */
#define TASKNAME_IDSP_RX      "IDSP_RX"

#endif /* !CVAPI_IDSP_INTERFACE_H_FILE */

