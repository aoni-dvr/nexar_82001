/*
 * Copyright (c) 2017-2017 Ambarella, Inc.
 *
 * This file and its contents ("Software") are protected by intellectual
 * property rights including, without limitation, U.S. and/or foreign copyrights.
 * This Software is also the confidential and proprietary information of
 * Ambarella, Inc. and its licensors.  You may not use, reproduce, disclose,
 * distribute, modify, or otherwise prepare derivative works of this Software
 * or any portion thereof except pursuant to a signed license agreement or
 * nondisclosure agreement with Ambarella, Inc. or its authorized affiliates.
 * In the absence of such an agreement, you agree to promptly notify and
 * return this Software to Ambarella, Inc.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 * MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef CVAPI_SP_ANALYZER_INTERFACE_H_FILE
#define CVAPI_SP_ANALYZER_INTERFACE_H_FILE

#include "cvapi_calibration_data.h"

#define TASKNAME_SP_ANALYZER    "SP_ANALYZER"

#define SPUAN_IDSP_FORMAT_Y_PGM      0  // output y comopnent in pgm format
#define SPUAN_IDSP_FORMAT_Y_UV_BIN   1  // output y/uv components as separate binary
#define SPUAN_IDSP_FORMAT_YUV_BIN    2  // output yuv in a single binary

#define SPUAN_IDSP_DUMP_LEFT          1
#define SPUAN_IDSP_DUMP_RIGHT         2
#define SPUAN_IDSP_DUMP_BOTH          3


#define SPUAN_FORMAT_PGM          0  // output map in 16-bit pgm format
#define SPUAN_FORMAT_APF          1  // output map in apf format, experimental
#define SPUAN_FORMAT_BIN          2  // output map in plain binary

/*******************************************************************************
 *                               INPUT BUFFER
 * One input buffer: SP_DISPARITY_MAP
 *
 *******************************************************************************
 *                              OUTPUT BUFFER
 * One output buffer: SP_ANALYZER_FINISH
 *
 *******************************************************************************
 *                              CVTABLE LABELS
 * SP_ANALYZER_MODE: optional
 *      override config[0].
 *
 *******************************************************************************
 *                       SYSFLOW CONFIGURATION SYNTAX
 * 0: bit-mask to enable the dump of a specific hier-scale
 *      bit[0]:  hierarchical layer 0
 *                 ...
 *      bit[12]: hierarchical layer 12
 *      bit[13]: lane-detection layer
 * 1: frame-num from which to start dump (inclusive)
 * 2: frame-num from which to stop dump  (exclusive)
 * 3: idsp output file format, supported values defined above
 * 4: idsp channels to dump, bitwise or between as above
 * 5: spu  output file format, supported values defined above
 * 6: dump delay value, default 2000                          
 *
 ******************************************************************************/

#define SP_ANALYZER_CONFIG          0x13CAFFE

#define SPUAN_FIELD_NONE                      (0)
#define SPUAN_FIELD_ALL                       (0xFFFFFFFFU)
#define SPUAN_FIELD_IDSP_MODE                 (1)
#define SPUAN_FIELD_FRAME_START               (1 << 2)
#define SPUAN_FIELD_FRAME_END                 (1 << 3)
#define SPUAN_FIELD_FRAME_END                 (1 << 3)
#define SPUAN_FIELD_MAX_NUM_FRAMES            (1 << 4)
#define SPUAN_FIELD_STEREO_DUMP               (1 << 5)
#define SPUAN_FIELD_IDSP_FORMAT               (1 << 6)
#define SPUAN_FIELD_SPU_FORMAT                (1 << 7)
#define SPUAN_FIELD_PATH                      (1 << 8)
#define SPUAN_FIELD_CALIBRATION               (1 << 9)


/*
 *  idsp_sp_analyzer_header_t
 *  @brief
 *  configuration header used in each configuration messages.
 *  Note that the header has to be the first element of the configuration
 *  message.
 *  */
typedef struct {
 uint32_t    m_message_type;
} spanal_header_t;



/*  Public Configuration State */
typedef struct {
 spanal_header_t  m_header;
 uint32_t m_field_mask;                  /* *< mask to mark which fields have been filled */
 uint32_t m_idsp_mode;
 uint32_t m_frame_start;
 uint32_t m_frame_end;
 uint32_t m_max_num_frames;
 uint32_t m_stereo_dump;
 uint32_t m_idsp_format;
 uint32_t m_spu_format;
 char     m_path[80];
 camera_octaves_t m_camera;
} spanal_config_t;



#endif /* !CVAPI_SP_ANALYZER_INTERFACE_H_FILE */

