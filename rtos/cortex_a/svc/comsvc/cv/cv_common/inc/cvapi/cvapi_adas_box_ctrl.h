/**
 *  @file cvapi_adas_box_ctrl.h
 *
 *  @copyright Copyright (c) 2015 Ambarella, Inc.
 *
 *  This file and its contents (!¡±Software!¡L) are protected by intellectual property rights including, without limitation,
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
 *  @details ADAS box control input output API
 *
 */


#ifndef CVAPI_ADAS_BOX_CTRL_H
#define CVAPI_ADAS_BOX_CTRL_H

#ifdef __linux__
#include <stdint.h>
#endif

#include <cvapi_ll_lane_detection_persp_interface.h>
#include <cvapi_hl_lane_detection_persp_interface.h>
#include <cvapi_fcw_interface.h>
//#include <cvapi_ttc_estimation_interface.h>
/// message code
#define AMAS_BOX_MSG(cat, idx)  (cat<<24 | idx)
#define ADAS_BOX_MSG_CAT_CTRL           (0x1)
#define ADAS_BOX_MSG_CAT_VO             (0x2)
#define ADAS_BOX_MSG_CAT_EXT_SENSOR     (0x3)

#define ADAS_BOX_MSG_BBX                AMAS_BOX_MSG(ADAS_BOX_MSG_CAT_VO, 1)
#define ADAS_BOX_MSG_FRM_RECV           AMAS_BOX_MSG(ADAS_BOX_MSG_CAT_VO, 2)
#define ADAS_BOX_MSG_FSD                AMAS_BOX_MSG(ADAS_BOX_MSG_CAT_VO, 3)
#define ADAS_BOX_MSG_LL_LD              AMAS_BOX_MSG(ADAS_BOX_MSG_CAT_VO, 4)
#define ADAS_BOX_MSG_EDGE               AMAS_BOX_MSG(ADAS_BOX_MSG_CAT_VO, 5)
#define ADAS_BOX_MSG_LINE               AMAS_BOX_MSG(ADAS_BOX_MSG_CAT_VO, 6)
#define ADAS_BOX_MSG_HL_LINE            AMAS_BOX_MSG(ADAS_BOX_MSG_CAT_VO, 7)
#define ADAS_BOX_MSG_LL_LINE            AMAS_BOX_MSG(ADAS_BOX_MSG_CAT_VO, 8)

#define ADAS_BOX_CTRL_MAX_BBX_NUM       (150)
typedef struct adas_box_ctrl_bbx_s {
    uint16_t cat;
    uint16_t fid;
    uint32_t score;
    uint16_t x;
    uint16_t y;
    uint16_t w;
    uint16_t h;
} adas_box_ctrl_bbx_t;

//ADAS_BOX_MSG_BBX
typedef struct adas_box_ctrl_bbx_list_msg_s {
    uint32_t msg_code;
    uint32_t source;
    uint32_t capture_time;
    uint32_t frame_num;
    uint32_t num_bbx;
    adas_box_ctrl_bbx_t bbx[ADAS_BOX_CTRL_MAX_BBX_NUM];
} adas_box_ctrl_bbx_list_msg_t;

//ADAS_BOX_MSG_FRM_RECV
typedef struct adas_box_ctrl_frame_recv_msg_s {
    uint32_t msg_code;
    uint32_t cnt;
} adas_box_ctrl_frame_recv_msg_t;

//ADAS_BOX_MSG_FSD
typedef struct adas_box_ctrl_fsd_msg_s {
    uint32_t msg_code;
    void* addr;
} adas_box_ctrl_fsd_msg_t;

//ADAS_BOX_MSG_LL_LD
typedef struct adas_box_ctrl_ll_ld_msg_s {
    uint32_t msg_code;
    void* addr;
} adas_box_ctrl_ll_ld_msg_t;

//ADAS_BOX_MSG_EDGE
typedef struct adas_box_ctrl_edge_msg_s {
    uint32_t msg_code;
    void* addr;
} adas_box_ctrl_edge_msg_t;

//ADAS_BOX_MSG_LINE
#define LD_MAX_LINE  (12)
#define LD_LEFT_IDX  (10)
#define LD_RIGHT_IDX (11)
typedef struct adas_box_ctrl_line_msg_s {
    uint32_t msg_code;
    uint32_t capture_time;
    uint32_t frame_num;
    float line_par_a[LD_MAX_LINE];
    float line_par_b[LD_MAX_LINE];
    int32_t score[LD_MAX_LINE];
} adas_box_ctrl_line_msg_t;


// //HL LINE
#define HL_LINE_MAX_POINTS (50)
#define LD_MAX_HL_LINE  (4)

typedef struct adas_box_ctrl_hl_line_debug_msg_s {
    uint32_t msg_code;
    uint32_t capture_time;
    uint32_t frame_num;

} adas_box_ctrl_hl_line_debug_msg_t;

typedef struct adas_box_ctrl_ll_line_msg_s {
    uint32_t msg_code;
    uint32_t capture_time;
    uint32_t frame_num;
    //ll_lane_detection_persp_output_t ll_cc;
    ll_seg_lane_detection_persp_output_t ll_cc;
    uint32_t num_roi;
    uint32_t roi_w;
    uint32_t roi_h;
    uint32_t cc_max_w;
    uint32_t cc_w_bytes;

} adas_box_ctrl_ll_line_msg_t;

typedef struct adas_box_ctrl_hl_line_msg_s {
    uint32_t msg_code;
    uint32_t capture_time;
    uint32_t frame_num;
    hl_lane_detection_persp_output_t  hl_out;
    adas_box_ctrl_ll_line_msg_t       ll_msg;//remove
    fcw_output_t          fcw_out;
} adas_box_ctrl_hl_line_msg_t;

#endif //CVAPI_ADAS_BOX_CTRL_H

