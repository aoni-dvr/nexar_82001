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

#include "ik_data_type.h"
#include "AmbaDSP_ImgContextEntity.h"
#include "AmbaDSP_ImgConfigEntity.h"

#define SFT                 16UL

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
    int32 warp_enable;
    uint32 ability;
    ik_window_size_info_t *p_window_size_info;
    ik_vin_active_window_t *p_active_window;
    ik_dummy_margin_range_t *p_dmy_range;
    ik_stitch_info_t *p_stitching_info;
    amba_ik_calc_win_result_t *p_result_win;
} amba_ik_dummy_win_calculate_t;

typedef struct {
    amba_ik_calc_win_result_t *p_result_win;
} amba_ik_cfa_win_calculate_t;

typedef struct {
    int32 warp_enable;
    uint32 ability;
    ik_window_size_info_t *p_window_size_info;
    ik_vin_active_window_t *p_active_window;
    ik_dummy_margin_range_t *p_dmy_range;
    ik_dzoom_info_t *p_dzoom_info;
    ik_stitch_info_t *p_stitching_info;
    amba_ik_calc_win_result_t *p_result_win;
} amba_ik_win_calc_t;

typedef struct {
    amba_ik_calc_win_result_t result_win;
    uint8 reserved0[128u-(sizeof(amba_ik_calc_win_result_t)%128u)];
    idsp_ik_flow_info_t flow_info;
    uint8 reserved1[128u-(sizeof(idsp_ik_flow_info_t)%128u)];
    idsp_ik_stitch_data_t stitch;
    uint8 reserved2[128u-(sizeof(idsp_ik_stitch_data_t)%128u)];
    uint8 CR_buf_42[CR_SIZE_42];
} amba_ik_calc_geo_work_buf_t;

uint32 exe_win_calc(const amba_ik_win_calc_t *p_info);
uint32 exe_win_calc_wrapper(amba_ik_filter_t *p_filters);

uint32 exe_query_calc_geo_buf_size(ik_query_calc_geo_buf_size_t *p_buf_size_info);
uint32 exe_calc_geo_settings(const ik_ability_t *p_ability, const ik_calc_geo_settings_t *p_info, const ik_calc_geo_result_t *p_result);

uint32 exe_warp_remap(const ik_in_warp_remap_t *p_warp_remap_in, ik_out_warp_remap_t *p_warp_remap_out);
uint32 exe_cawarp_remap(const ik_in_cawarp_remap_t *p_cawarp_remap_in, ik_out_cawarp_remap_t *p_cawarp_remap_out);

#endif
