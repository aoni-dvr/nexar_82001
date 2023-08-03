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

#ifndef IMG_EXECUTER_CALIB_H
#define IMG_EXECUTER_CALIB_H

#include "AmbaDSP_ImgControlState.h"
#include "AmbaDSP_ImgContextEntity.h"
#include "AmbaDSP_ImgConfigEntity.h"
#include "cmd_msg_cv2.h"

#define WARP_FRAC_BIT_NUM   4UL
#define CAWARP_FRAC_BIT_NUM 5UL
#define INTERPOLATE_HOR     1UL
#define INTERPOLATE_VER     2UL
#define SFT                 16UL

typedef struct {
    int32 tile_w;
    int32 tile_h;
    int32 hor_grid_num;
    int32 ver_grid_num;
    int32 map_pitch;
    int32 x_offset;
    int32 y_offset;
    int32 x_scale;
    int32 y_scale;
    int32 calib_2_warp_input_scale;
    uint32 sec3_initial_phase_q16; /*(16.16)*/
    uint8 vert_tbl_chk;
} img_core_warp_itpl_info;

typedef struct {
    int32 tile_w;
    int32 tile_h;
    int32 hor_grid_num;
    int32 ver_grid_num;
    int32 map_pitch;
    int32 x_offset;
    int32 y_offset;
    int32 x_scale;
    int32 y_scale;
    int32 calib_2_warp_input_scale;
} img_core_ca_warp_itpl_info;

typedef struct {
    ik_window_size_info_t *p_window_size_info;
    ik_vin_active_window_t *p_active_window;
} amba_ik_check_active_win_validataion_t;

typedef struct {
    ik_window_size_info_t *p_window_size_info;
    ik_vin_active_window_t *p_active_window;
    ik_dzoom_info_t *p_dzoom_info;
    amba_ik_calc_win_result_t *p_result_win;
} amba_ik_actual_win_calculate_t;

typedef struct {
    ik_window_size_info_t *p_window_size_info;
} amba_ik_tune_output_win_without_tiny_scaling_t;

typedef struct {
    int32 warp_enable;
    uint32 is_ce_supported;
    uint32 sensor_mode;
    ik_window_size_info_t *p_window_size_info;
    ik_vin_active_window_t *p_active_window;
    ik_dummy_margin_range_t *p_dmy_range;
    amba_ik_calc_win_result_t *p_result_win;
} amba_ik_dummy_win_calculate_t;

typedef struct {
    ik_cfa_window_size_info_t *p_cfa_window_size_info;
    amba_ik_calc_win_result_t *p_result_win;
} amba_ik_cfa_win_calculate_t;

typedef struct {
    // from p_in
    int32   warp_enable;
    uint32  flip_mode;
    ik_window_size_info_t *p_window_size_info;
    ik_vin_active_window_t *p_active_window;
    ik_dummy_margin_range_t *p_dmy_range;
    amba_ik_calc_win_result_t *p_result_win;

    ik_warp_info_t *p_calib_warp_info;

    // from p_work_buf
    ik_grid_point_t   *p_remap_buffer;
    int32             *p_remap_row;
    ik_grid_point_t   *p_remap_search_buffer; // p_remap_search_buffer
    uint32            *p_remap_motion_scan_buffer; // p_remap_motion_scan_buffer
    ik_grid_point_t   *p_warp_add_buffer;

    // from p_out
    set_warp_control_t *p_geo_warp_control;
    set_chromatic_aberration_warp_control_t *p_ca_warp_control;

    // predict warp resources
    uint32 query_warp_enb;
    uint32 out_wait_line;
    uint32 out_dma_size;
    uint32 out_vertical_range;
    uint32 out_warp_vector_v_flip_num;
    uint32 out_dmy_broken;
    ik_window_geomery_t out_auto_roi;
} amba_ik_warp_calc_t;

typedef struct {
    int32 warp_enable;
    uint32 is_ce_supported;
    uint32 sensor_mode;
    ik_window_size_info_t *p_window_size_info;
    ik_cfa_window_size_info_t *p_cfa_window_size_info;
    ik_vin_active_window_t *p_active_window;
    ik_dummy_margin_range_t *p_dmy_range;
    ik_dzoom_info_t *p_dzoom_info;
    amba_ik_calc_win_result_t *p_result_win;
} amba_ik_win_calc_t;

typedef struct {
    // from p_in
    int32   cawarp_enable;
    uint32  flip_mode;
    ik_window_size_info_t *p_window_size_info;
    amba_ik_calc_win_result_t *p_result_win;

    ik_cawarp_info_t *p_calib_ca_warp_info;


    // from p_work_buf
    ik_grid_point_t   *p_ca_red_remap_buffer;
    ik_grid_point_t   *p_ca_blue_remap_buffer;
    int32             *p_ca_remap_row;

    // from p_out
    set_chromatic_aberration_warp_control_t *p_ca_warp_control;
} amba_ik_cawarp_calc_t;

typedef struct {
    ik_grid_point_t         remap_buffer[WARP_VIDEO_REMAP_MAXHORGRID * WARP_VIDEO_REMAP_MAXVERGRID];
    ik_grid_point_t         ca_red_remap_buffer[CAWARP_VIDEO_REMAP_MAXHORGRID * CAWARP_VIDEO_REMAP_MAXVERGRID];
    ik_grid_point_t         ca_blue_remap_buffer[CAWARP_VIDEO_REMAP_MAXHORGRID * CAWARP_VIDEO_REMAP_MAXVERGRID];
    int32                   remap_row[WARP_VIDEO_REMAP_MAXVERGRID];
    int32                   ca_remap_row[CAWARP_VIDEO_REMAP_MAXVERGRID];
    set_warp_control_t      geo_warp_control;
    int16                   warp_hor[WARP_VIDEO_MAXHORGRID * WARP_VIDEO_MAXVERGRID]; // multi_slice
    int16                   warp_ver[WARP_VIDEO_MAXHORGRID * WARP_VIDEO_MAXVERGRID]; // multi_slice
    ik_grid_point_t         remap_search_buffer[WARP_VIDEO_REMAP_MAXHORGRID * WARP_VIDEO_REMAP_MAXVERGRID];
    uint32                  remap_motion_scan_buffer[WARP_VIDEO_REMAP_MAXHORGRID * WARP_VIDEO_REMAP_MAXVERGRID];
    ik_grid_point_t         warp_add_buffer[WARP_VIDEO_REMAP_MAXHORGRID * WARP_VIDEO_REMAP_MAXVERGRID];
} amba_ik_calc_geo_work_buf_t;

typedef struct {
    uint16 warp_hor[WARP_VIDEO_MAXHORGRID*WARP_VIDEO_MAXVERGRID]; // multi_slice
    uint16 warp_ver[WARP_VIDEO_MAXHORGRID*WARP_VIDEO_MAXVERGRID]; // multi_slice
    uint16 cawarp_hor_red[CAWARP_VIDEO_MAXHORGRID*CAWARP_VIDEO_MAXVERGRID];
    uint16 cawarp_hor_blue[CAWARP_VIDEO_MAXHORGRID*CAWARP_VIDEO_MAXVERGRID];
    uint16 cawarp_ver_red[CAWARP_VIDEO_MAXHORGRID*CAWARP_VIDEO_MAXVERGRID];
    uint16 cawarp_ver_blue[CAWARP_VIDEO_MAXHORGRID*CAWARP_VIDEO_MAXVERGRID];
} amba_ik_calc_geo_out_tbl_t;

uint32 exe_calc_warp_resources(const ik_query_warp_t *p_info, ik_query_warp_result_t *p_result);
uint32 exe_query_calc_geo_buf_size(ik_query_calc_geo_buf_size_t *p_buf_size_info);
uint32 exe_calc_geo_settings(const ik_calc_geo_settings_t *p_info, const ik_calc_geo_result_t *p_result);
#endif
