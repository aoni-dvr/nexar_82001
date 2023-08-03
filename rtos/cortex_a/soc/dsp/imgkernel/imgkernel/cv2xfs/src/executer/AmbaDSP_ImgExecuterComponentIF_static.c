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

#ifndef AMBA_IK_IMG_EXE_CI_C
#define AMBA_IK_IMG_EXE_CI_C

#include "AmbaDSP_ImgConfigComponentIF.h"
#include "AmbaDSP_ImgContextComponentIF.h"
#include "AmbaDSP_ImgExecuterComponentIF.h"
#include "AmbaDSP_ImgArchComponentIF.h"
//#include "AmbaDSP_ImgArchSystemAPI.h"
#include "AmbaDSP_ImgExecuterUtilities.h"
#include "AmbaDSP_ImgExecuterCalib.h"
#include "AmbaDSP_ImgContextDebugUtility.h"
#include "AmbaDSP_ImgUtility.h"
#ifndef EARLYTEST_ENV
#include "AmbaDSP_Monitor.h"
#endif

#define IK_DBG_PROFILE_CRC 0

//for misra casting.
static inline uint32 physical2u32(const void *p)
{
    uint32 out;
    (void)amba_ik_system_memcpy(&out, &p, sizeof(uint32));
    return out;
}

//for misra casting.
static inline void* ptr2ptr(void *p)
{
    return p;
}

static inline uint8 null_check_op(const void *p)
{
    return (p == NULL) ? 1U : 0U;
}

static inline uint8 equal_op_u32(uint32 compare1, uint32 compare2)
{
    return (compare1 == compare2) ? 1U : 0U;
}

static inline uint8 non_equal_op_u32(uint32 compare1, uint32 compare2)
{
    return (compare1 != compare2) ? 1U : 0U;
}

static inline uint32 uint32_min_ik(uint32 x, uint32 y)
{
    uint32 r_val;
    if (x > y) {
        r_val = y;
    } else {
        r_val = x;
    }
    return r_val;
}

static INLINE uint32 exe_clone_flow_control_and_tbl_list(uint32 context_id, idsp_ik_flow_ctrl_t *p_dst_flow, amba_ik_flow_tables_list_t *p_dst_flow_tbl_list, uint32 src_flow_idx)
{
    uint32 rval = IK_OK;
    uintptr addr = 0;
    const idsp_ik_flow_ctrl_t *p_src_flow = NULL;
    const amba_ik_flow_tables_list_t *p_src_flow_tbl_list = NULL;

    rval |= img_ctx_get_flow_control(context_id, src_flow_idx, &addr);
    (void)amba_ik_system_memcpy(&p_src_flow, &addr, sizeof(void *));

    rval |= img_ctx_get_flow_tbl_list(context_id, src_flow_idx, &addr);
    (void)amba_ik_system_memcpy(&p_src_flow_tbl_list, &addr, sizeof(void *));

    if (rval == IK_OK) {
        (void)amba_ik_system_memcpy(p_dst_flow, p_src_flow, sizeof(idsp_ik_flow_ctrl_t));
        (void)amba_ik_system_memcpy(p_dst_flow_tbl_list, p_src_flow_tbl_list, sizeof(amba_ik_flow_tables_list_t));
    } else {
        amba_ik_system_print_uint32_5("[IK] call exe_clone_flow_control(%d) fail", context_id, DC_U, DC_U, DC_U, DC_U);
        rval = IK_ERR_0000;
    }

    return rval;
}

static INLINE uint32 exe_get_next_ring_buffer_index(uint32 buffer_number, uint32 current_index)
{
    uint32 rval;
    rval = (current_index+1U) % buffer_number;
    return rval;
}

static INLINE uint32 exe_get_next_flow_control_index(uint32 buffer_number, const amba_ik_active_cr_state_t *active_cr_state, uint32 *p_flow_idx)
{
    uint32 rval = IK_OK;
    if (active_cr_state == NULL) {
        rval = IK_ERR_0005;
    } else if(buffer_number == 0U) {
        rval = IK_ERR_0000;
    } else {
        *p_flow_idx = exe_get_next_ring_buffer_index(buffer_number, active_cr_state->active_flow_idx);
    }
    return rval;
}

static inline uint8 exe_get_next_ring_buffer_index_condition(uint8 updated, uint8 current_idx, uint32 buffer_number)
{
    uint8 new_idx;
    if(updated == 0U) {
        new_idx = (uint8)exe_get_next_ring_buffer_index(buffer_number, current_idx);
    } else {
        new_idx = current_idx;
    }
    return new_idx;
}

static inline void exe_get_next_cr_index_part1(uint32 buffer_number,
        amba_ik_filter_update_flags_t *p_update_flag,
        amba_ik_cr_index_t *p_table_index)
{
    uint8 bool_result;

    if (p_update_flag->iso.sensor_information_updated != 0U) {
        p_table_index->cr_13_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_13_update, p_table_index->cr_13_index, buffer_number);
        p_update_flag->cr.cr_13_update = 1;

        p_table_index->cr_51_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_51_update, p_table_index->cr_51_index, buffer_number);
        p_update_flag->cr.cr_51_update = 1;

        p_table_index->cr_24_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_24_update, p_table_index->cr_24_index, buffer_number);
        p_update_flag->cr.cr_24_update = 1;

        p_table_index->cr_23_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_23_update, p_table_index->cr_23_index, buffer_number);
        p_update_flag->cr.cr_23_update = 1;

        p_table_index->cr_22_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_22_update, p_table_index->cr_22_index, buffer_number);
        p_update_flag->cr.cr_22_update = 1;

        p_table_index->cr_21_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_21_update, p_table_index->cr_21_index, buffer_number);
        p_update_flag->cr.cr_21_update = 1;

        p_table_index->cr_16_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_16_update, p_table_index->cr_16_index, buffer_number);
        p_update_flag->cr.cr_16_update = 1;

        p_table_index->cr_12_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_12_update, p_table_index->cr_12_index, buffer_number);
        p_update_flag->cr.cr_12_update = 1;

        p_table_index->cr_11_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_11_update, p_table_index->cr_11_index, buffer_number);
        p_update_flag->cr.cr_11_update = 1;

        p_table_index->cr_9_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_9_update, p_table_index->cr_9_index, buffer_number);
        p_update_flag->cr.cr_9_update = 1;

        p_table_index->cr_6_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_6_update, p_table_index->cr_6_index, buffer_number);
        p_update_flag->cr.cr_6_update = 1;

        p_table_index->cr_7_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_7_update, p_table_index->cr_7_index, buffer_number);
        p_update_flag->cr.cr_7_update = 1;

        p_table_index->cr_8_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_8_update, p_table_index->cr_8_index, buffer_number);
        p_update_flag->cr.cr_8_update = 1;

        p_table_index->cr_4_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_4_update, p_table_index->cr_4_index, buffer_number);
        p_update_flag->cr.cr_4_update = 1;

        p_table_index->cr_45_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_45_update, p_table_index->cr_45_index, buffer_number);
        p_update_flag->cr.cr_45_update = 1;

        p_table_index->cr_47_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_47_update, p_table_index->cr_47_index, buffer_number);
        p_update_flag->cr.cr_47_update = 1;

        p_table_index->cr_49_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_49_update, p_table_index->cr_49_index, buffer_number);
        p_update_flag->cr.cr_49_update = 1;

        p_table_index->cr_50_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_50_update, p_table_index->cr_50_index, buffer_number);
        p_update_flag->cr.cr_50_update = 1;
    }

    bool_result = non_equal_op_u32(p_update_flag->iso.aaa_stat_info_updated, 0U) + non_equal_op_u32(p_update_flag->iso.af_stat_ex_info_updated, 0U) +
                  non_equal_op_u32(p_update_flag->iso.pg_af_stat_ex_info_updated, 0U) + non_equal_op_u32(p_update_flag->iso.histogram_info_update, 0U) +
                  non_equal_op_u32(p_update_flag->iso.histogram_info_pg_update, 0U) + non_equal_op_u32(p_update_flag->iso.sensor_information_updated, 0u) +
                  non_equal_op_u32(p_update_flag->iso.rgb_ir_updated, 0u);
    if (bool_result != 0u) {
        p_table_index->aaa_data = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.aaa_data, p_table_index->aaa_data, buffer_number);
        p_update_flag->cr.aaa_data = 1;

        p_table_index->cr_9_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_9_update, p_table_index->cr_9_index, buffer_number);
        p_update_flag->cr.cr_9_update = 1;

        p_table_index->cr_12_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_12_update, p_table_index->cr_12_index, buffer_number);
        p_update_flag->cr.cr_12_update = 1;

        p_table_index->cr_21_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_21_update, p_table_index->cr_21_index, buffer_number);
        p_update_flag->cr.cr_21_update = 1;

        p_table_index->cr_28_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_28_update, p_table_index->cr_28_index, buffer_number);
        p_update_flag->cr.cr_28_update = 1;
    }

    bool_result = non_equal_op_u32(p_update_flag->iso.static_bpc_updated, 0U) + non_equal_op_u32(p_update_flag->iso.static_bpc_internal_updated, 0U);
    if (bool_result != 0u) {
        p_table_index->cr_12_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_12_update, p_table_index->cr_12_index, buffer_number);
        p_update_flag->cr.cr_12_update = 1;
    }

    bool_result = non_equal_op_u32(p_update_flag->iso.vignette_compensation_updated, 0U) + non_equal_op_u32(p_update_flag->iso.vignette_compensation_internal_updated, 0U);
    if (bool_result != 0u) {
        p_table_index->cr_9_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_9_update, p_table_index->cr_9_index, buffer_number);
        p_update_flag->cr.cr_9_update = 1;

        p_table_index->cr_10_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_10_update, p_table_index->cr_10_index, buffer_number);
        p_update_flag->cr.cr_10_update = 1;

        p_table_index->cr_11_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_11_update, p_table_index->cr_11_index, buffer_number);
        p_update_flag->cr.cr_11_update = 1;

        p_table_index->cr_47_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_47_update, p_table_index->cr_47_index, buffer_number);
        p_update_flag->cr.cr_47_update = 1;

        p_table_index->cr_48_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_48_update, p_table_index->cr_48_index, buffer_number);
        p_update_flag->cr.cr_48_update = 1;

        p_table_index->cr_49_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_49_update, p_table_index->cr_49_index, buffer_number);
        p_update_flag->cr.cr_49_update = 1;
    }

    bool_result = non_equal_op_u32(p_update_flag->iso.calib_warp_info_updated, 0U) + non_equal_op_u32(p_update_flag->iso.warp_internal_updated, 0U) +
                  non_equal_op_u32(p_update_flag->iso.chroma_filter_updated, 0U) + non_equal_op_u32(p_update_flag->iso.warp_buf_info_updated, 0U);
    if (bool_result != 0u) {
        //due to radius update will call ikc_warp_radius, but not calling ikc_warp, thus use the previous warp table.
        // use previous frame, don't need to update warp table in flow_t, thus no need to raise cr update flag
        if((non_equal_op_u32(p_update_flag->iso.calib_warp_info_updated, 0U) + non_equal_op_u32(p_update_flag->iso.warp_internal_updated, 0U)) != 0U) {
            p_table_index->warp = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.warp, p_table_index->warp, buffer_number);
            p_update_flag->cr.warp = 1;
        }

        p_table_index->cr_33_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_33_update, p_table_index->cr_33_index, buffer_number);
        p_update_flag->cr.cr_33_update = 1;

        p_table_index->cr_34_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_34_update, p_table_index->cr_34_index, buffer_number);
        p_update_flag->cr.cr_34_update = 1;

        p_table_index->cr_35_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_35_update, p_table_index->cr_35_index, buffer_number);
        p_update_flag->cr.cr_35_update = 1;

        p_table_index->cr_42_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_42_update, p_table_index->cr_42_index, buffer_number);
        p_update_flag->cr.cr_42_update = 1;

        p_table_index->cr_43_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_43_update, p_table_index->cr_43_index, buffer_number);
        p_update_flag->cr.cr_43_update = 1;

        p_table_index->cr_44_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_44_update, p_table_index->cr_44_index, buffer_number);
        p_update_flag->cr.cr_44_update = 1;
    }

    bool_result = non_equal_op_u32(p_update_flag->iso.calib_ca_warp_info_updated, 0U) + non_equal_op_u32(p_update_flag->iso.cawarp_enable_updated, 0U) +
                  non_equal_op_u32(p_update_flag->iso.cawarp_internal_updated, 0U);
    if (bool_result != 0u) {
        p_table_index->cawarp = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cawarp, p_table_index->cawarp, buffer_number);
        p_update_flag->cr.cawarp = 1;

        p_table_index->cr_16_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_16_update, p_table_index->cr_16_index, buffer_number);
        p_update_flag->cr.cr_16_update = 1;

        p_table_index->cr_17_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_17_update, p_table_index->cr_17_index, buffer_number);
        p_update_flag->cr.cr_17_update = 1;
    }

    if (p_update_flag->iso.before_ce_wb_gain_updated != 0U) {
        p_table_index->cr_13_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_13_update, p_table_index->cr_13_index, buffer_number);
        p_update_flag->cr.cr_13_update = 1;

        p_table_index->cr_51_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_51_update, p_table_index->cr_51_index, buffer_number);
        p_update_flag->cr.cr_51_update = 1;

        p_table_index->cr_12_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_12_update, p_table_index->cr_12_index, buffer_number);
        p_update_flag->cr.cr_12_update = 1;

        p_table_index->cr_50_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_50_update, p_table_index->cr_50_index, buffer_number);
        p_update_flag->cr.cr_50_update = 1;
    }

    if (p_update_flag->iso.after_ce_wb_gain_updated != 0U) {
        p_table_index->cr_12_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_12_update, p_table_index->cr_12_index, buffer_number);
        p_update_flag->cr.cr_12_update = 1;

        p_table_index->cr_50_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_50_update, p_table_index->cr_50_index, buffer_number);
        p_update_flag->cr.cr_50_update = 1;

        p_table_index->cr_22_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_22_update, p_table_index->cr_22_index, buffer_number);
        p_update_flag->cr.cr_22_update = 1;

        p_table_index->cr_30_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_30_update, p_table_index->cr_30_index, buffer_number);
        p_update_flag->cr.cr_30_update = 1;
    }
}

static inline void exe_get_next_cr_index_part2(uint32 buffer_number,
        amba_ik_filter_update_flags_t *p_update_flag,
        amba_ik_cr_index_t *p_table_index)
{
    uint8 bool_result;

    if (p_update_flag->iso.cfa_leakage_filter_updated != 0U) {
        p_table_index->cr_49_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_49_update, p_table_index->cr_49_index, buffer_number);
        p_update_flag->cr.cr_49_update = 1;

        p_table_index->cr_23_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_23_update, p_table_index->cr_23_index, buffer_number);
        p_update_flag->cr.cr_23_update = 1;

        p_table_index->cr_12_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_12_update, p_table_index->cr_12_index, buffer_number);
        p_update_flag->cr.cr_12_update = 1;

        p_table_index->cr_50_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_50_update, p_table_index->cr_50_index, buffer_number);
        p_update_flag->cr.cr_50_update = 1;
    }

    if (p_update_flag->iso.anti_aliasing_updated != 0U) {
        p_table_index->cr_23_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_23_update, p_table_index->cr_23_index, buffer_number);
        p_update_flag->cr.cr_23_update = 1;

        p_table_index->cr_12_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_12_update, p_table_index->cr_12_index, buffer_number);
        p_update_flag->cr.cr_12_update = 1;
    }

    bool_result = non_equal_op_u32(p_update_flag->iso.dynamic_bpc_updated, 0u) + non_equal_op_u32(p_update_flag->iso.sensor_information_updated, 0u) +
                  non_equal_op_u32(p_update_flag->iso.rgb_ir_updated, 0u);
    if (bool_result != 0U) {
        p_table_index->cr_11_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_11_update, p_table_index->cr_11_index, buffer_number);
        p_update_flag->cr.cr_11_update = 1;

        p_table_index->cr_12_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_12_update, p_table_index->cr_12_index, buffer_number);
        p_update_flag->cr.cr_12_update = 1;
    }

    if (p_update_flag->iso.grgb_mismatch_updated != 0U) {
        p_table_index->cr_23_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_23_update, p_table_index->cr_23_index, buffer_number);
        p_update_flag->cr.cr_23_update = 1;
    }

    if (p_update_flag->iso.cfa_noise_filter_updated != 0U) {
        p_table_index->cr_23_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_23_update, p_table_index->cr_23_index, buffer_number);
        p_update_flag->cr.cr_23_update = 1;

        p_table_index->cr_11_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_11_update, p_table_index->cr_11_index, buffer_number);
        p_update_flag->cr.cr_11_update = 1;

        p_table_index->cr_12_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_12_update, p_table_index->cr_12_index, buffer_number);
        p_update_flag->cr.cr_12_update = 1;

        p_table_index->cr_49_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_49_update, p_table_index->cr_49_index, buffer_number);
        p_update_flag->cr.cr_49_update = 1;

        p_table_index->cr_50_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_50_update, p_table_index->cr_50_index, buffer_number);
        p_update_flag->cr.cr_50_update = 1;
    }

    if (p_update_flag->iso.dgain_sat_lvl_updated != 0U) {
        p_table_index->cr_22_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_22_update, p_table_index->cr_22_index, buffer_number);
        p_update_flag->cr.cr_22_update = 1;
    }

    if (p_update_flag->iso.demosaic_updated != 0U) {
        p_table_index->cr_12_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_12_update, p_table_index->cr_12_index, buffer_number);
        p_update_flag->cr.cr_12_update = 1;

        p_table_index->cr_23_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_23_update, p_table_index->cr_23_index, buffer_number);
        p_update_flag->cr.cr_23_update = 1;

        p_table_index->cr_24_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_24_update, p_table_index->cr_24_index, buffer_number);
        p_update_flag->cr.cr_24_update = 1;
    }

    bool_result = non_equal_op_u32(p_update_flag->iso.color_correction_updated, 0u)+ non_equal_op_u32(p_update_flag->iso.sensor_information_updated, 0u) +
                  non_equal_op_u32(p_update_flag->iso.rgb_ir_updated, 0u);
    if (bool_result != 0u) {
        p_table_index->cr_26_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_26_update, p_table_index->cr_26_index, buffer_number);
        p_update_flag->cr.cr_26_update = 1;

        p_table_index->cr_27_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_27_update, p_table_index->cr_27_index, buffer_number);
        p_update_flag->cr.cr_27_update = 1;

        p_table_index->cr_30_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_30_update, p_table_index->cr_30_index, buffer_number);
        p_update_flag->cr.cr_30_update = 1;
    }

    if (p_update_flag->iso.pre_cc_gain_updated != 0U) {
        p_table_index->cr_26_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_26_update, p_table_index->cr_26_index, buffer_number);
        p_update_flag->cr.cr_26_update = 1;
    }

    if (p_update_flag->iso.tone_curve_updated != 0U) {
        p_table_index->cr_26_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_26_update, p_table_index->cr_26_index, buffer_number);
        p_update_flag->cr.cr_26_update = 1;

        p_table_index->cr_30_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_30_update, p_table_index->cr_30_index, buffer_number);
        p_update_flag->cr.cr_30_update = 1;
    }
}

static inline void exe_get_next_cr_index_part3(uint32 buffer_number,
        amba_ik_filter_update_flags_t *p_update_flag,
        amba_ik_cr_index_t *p_table_index)
{
    uint8 bool_result;

    if (p_update_flag->iso.rgb_to_12y_updated != 0U) {
        p_table_index->cr_25_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_25_update, p_table_index->cr_25_index, buffer_number);
        p_update_flag->cr.cr_25_update = 1;

        p_table_index->cr_30_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_30_update, p_table_index->cr_30_index, buffer_number);
        p_update_flag->cr.cr_30_update = 1;
    }

    bool_result = non_equal_op_u32(p_update_flag->iso.rgb_to_yuv_matrix_updated, 0u) +
                  non_equal_op_u32(p_update_flag->iso.sensor_information_updated, 0u) +
                  non_equal_op_u32(p_update_flag->iso.rgb_ir_updated, 0u);
    if (bool_result != 0u) {
        p_table_index->cr_29_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_29_update, p_table_index->cr_29_index, buffer_number);
        p_update_flag->cr.cr_29_update = 1;
    }

    if (p_update_flag->iso.rgb_ir_updated != 0U) {
        p_table_index->cr_4_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_4_update, p_table_index->cr_4_index, buffer_number);
        p_update_flag->cr.cr_4_update = 1;

        p_table_index->cr_6_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_6_update, p_table_index->cr_6_index, buffer_number);
        p_update_flag->cr.cr_6_update = 1;

        p_table_index->cr_7_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_7_update, p_table_index->cr_7_index, buffer_number);
        p_update_flag->cr.cr_7_update = 1;

        p_table_index->cr_8_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_8_update, p_table_index->cr_8_index, buffer_number);
        p_update_flag->cr.cr_8_update = 1;

        p_table_index->cr_9_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_9_update, p_table_index->cr_9_index, buffer_number);
        p_update_flag->cr.cr_9_update = 1;

        p_table_index->cr_11_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_11_update, p_table_index->cr_11_index, buffer_number);
        p_update_flag->cr.cr_11_update = 1;

        p_table_index->cr_12_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_12_update, p_table_index->cr_12_index, buffer_number);
        p_update_flag->cr.cr_12_update = 1;

        p_table_index->cr_21_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_21_update, p_table_index->cr_21_index, buffer_number);
        p_update_flag->cr.cr_21_update = 1;

        p_table_index->cr_45_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_45_update, p_table_index->cr_45_index, buffer_number);
        p_update_flag->cr.cr_45_update = 1;

        p_table_index->cr_47_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_47_update, p_table_index->cr_47_index, buffer_number);
        p_update_flag->cr.cr_47_update = 1;

        p_table_index->cr_49_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_49_update, p_table_index->cr_49_index, buffer_number);
        p_update_flag->cr.cr_49_update = 1;

        p_table_index->cr_50_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_50_update, p_table_index->cr_50_index, buffer_number);
        p_update_flag->cr.cr_50_update = 1;
    }

    if (p_update_flag->iso.chroma_scale_updated != 0U) {
        p_table_index->cr_29_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_29_update, p_table_index->cr_29_index, buffer_number);
        p_update_flag->cr.cr_29_update = 1;
    }

    if (p_update_flag->iso.chroma_median_filter_updated != 0U) {
        p_table_index->cr_30_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_30_update, p_table_index->cr_30_index, buffer_number);
        p_update_flag->cr.cr_30_update = 1;

        p_table_index->cr_31_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_31_update, p_table_index->cr_31_index, buffer_number);
        p_update_flag->cr.cr_31_update = 1;
    }

    bool_result = non_equal_op_u32(p_update_flag->iso.first_luma_processing_mode_updated, 0U) + non_equal_op_u32(p_update_flag->iso.advance_spatial_filter_updated, 0U) +
                  non_equal_op_u32(p_update_flag->iso.fstshpns_fir_updated, 0U) + non_equal_op_u32(p_update_flag->iso.fstshpns_both_updated, 0U) +
                  non_equal_op_u32(p_update_flag->iso.fstshpns_noise_updated, 0U) + non_equal_op_u32(p_update_flag->iso.fstshpns_coring_updated, 0U) +
                  non_equal_op_u32(p_update_flag->iso.fstshpns_coring_index_scale_updated, 0U) + non_equal_op_u32(p_update_flag->iso.fstshpns_min_coring_result_updated, 0U) +
                  non_equal_op_u32(p_update_flag->iso.fstshpns_max_coring_result_updated, 0U) + non_equal_op_u32(p_update_flag->iso.fstshpns_scale_coring_updated, 0U);
    if (bool_result != 0u) {
        p_table_index->cr_30_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_30_update, p_table_index->cr_30_index, buffer_number);
        p_update_flag->cr.cr_30_update = 1;
    }

    bool_result = non_equal_op_u32(p_update_flag->iso.video_mctf_updated, 0U) + non_equal_op_u32(p_update_flag->iso.internal_video_mctf_updated, 0U) +
                  non_equal_op_u32(p_update_flag->iso.video_mctf_ta_updated, 0U) + non_equal_op_u32(p_update_flag->iso.fnlshpns_both_updated, 0U) +
                  non_equal_op_u32(p_update_flag->iso.fnlshpns_noise_updated, 0U) + non_equal_op_u32(p_update_flag->iso.fnlshpns_fir_updated, 0U) +
                  non_equal_op_u32(p_update_flag->iso.fnlshpns_coring_updated, 0U) + non_equal_op_u32(p_update_flag->iso.fnlshpns_coring_index_scale_updated, 0U) +
                  non_equal_op_u32(p_update_flag->iso.fnlshpns_min_coring_result_updated, 0U) + non_equal_op_u32(p_update_flag->iso.fnlshpns_max_coring_result_updated, 0U) +
                  non_equal_op_u32(p_update_flag->iso.fnlshpns_scale_coring_updated, 0U) + non_equal_op_u32(p_update_flag->iso.fnlshpns_both_tdt_updated, 0U) +
                  non_equal_op_u32(p_update_flag->iso.video_mctf_and_final_sharpen_updated, 0U);
    if (bool_result != 0u) {
        p_table_index->cr_111_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_111_update, p_table_index->cr_111_index, buffer_number);
        p_update_flag->cr.cr_111_update = 1;

        p_table_index->cr_112_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_112_update, p_table_index->cr_112_index, buffer_number);
        p_update_flag->cr.cr_112_update = 1;

        p_table_index->cr_113_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_113_update, p_table_index->cr_113_index, buffer_number);
        p_update_flag->cr.cr_113_update = 1;

        p_table_index->cr_114_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_114_update, p_table_index->cr_114_index, buffer_number);
        p_update_flag->cr.cr_114_update = 1;

        p_table_index->cr_115_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_115_update, p_table_index->cr_115_index, buffer_number);
        p_update_flag->cr.cr_115_update = 1;

        p_table_index->cr_116_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_116_update, p_table_index->cr_116_index, buffer_number);
        p_update_flag->cr.cr_116_update = 1;

        p_table_index->cr_117_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_117_update, p_table_index->cr_117_index, buffer_number);
        p_update_flag->cr.cr_117_update = 1;

        p_table_index->cr_118_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_118_update, p_table_index->cr_118_index, buffer_number);
        p_update_flag->cr.cr_118_update = 1;

        p_table_index->cr_119_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_119_update, p_table_index->cr_119_index, buffer_number);
        p_update_flag->cr.cr_119_update = 1;

        p_table_index->cr_120_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_120_update, p_table_index->cr_120_index, buffer_number);
        p_update_flag->cr.cr_120_update = 1;

        p_table_index->cr_121_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_121_update, p_table_index->cr_121_index, buffer_number);
        p_update_flag->cr.cr_121_update = 1;
    }

    if (p_update_flag->iso.window_size_info_updated != 0U) {
        p_table_index->cr_111_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_111_update, p_table_index->cr_111_index, buffer_number);
        p_update_flag->cr.cr_111_update = 1;

        p_table_index->cr_112_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_112_update, p_table_index->cr_112_index, buffer_number);
        p_update_flag->cr.cr_112_update = 1;

        p_table_index->cr_113_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_113_update, p_table_index->cr_113_index, buffer_number);
        p_update_flag->cr.cr_113_update = 1;

        p_table_index->cr_116_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_116_update, p_table_index->cr_116_index, buffer_number);
        p_update_flag->cr.cr_116_update = 1;

        p_table_index->cr_117_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_117_update, p_table_index->cr_117_index, buffer_number);
        p_update_flag->cr.cr_117_update = 1;
    }

    bool_result = non_equal_op_u32(p_update_flag->iso.chroma_filter_updated, 0U) + non_equal_op_u32(p_update_flag->iso.wide_chroma_filter, 0U) +
                  non_equal_op_u32(p_update_flag->iso.wide_chroma_filter_combine, 0U);
    if (bool_result != 0U) {
        p_table_index->cr_36_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_36_update, p_table_index->cr_36_index, buffer_number);
        p_update_flag->cr.cr_36_update = 1;

        p_table_index->cr_37_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_37_update, p_table_index->cr_37_index, buffer_number);
        p_update_flag->cr.cr_37_update = 1;

        p_table_index->cr_35_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_35_update, p_table_index->cr_35_index, buffer_number);
        p_update_flag->cr.cr_35_update = 1;

        p_table_index->cr_42_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_42_update, p_table_index->cr_42_index, buffer_number);
        p_update_flag->cr.cr_42_update = 1;

        p_table_index->cr_33_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_33_update, p_table_index->cr_33_index, buffer_number);
        p_update_flag->cr.cr_33_update = 1;
    }

    if (p_update_flag->iso.hdr_raw_info_updated != 0U) {
        // no CR, update flow_info...
    }
}

static inline void exe_get_next_cr_index_part4(uint32 buffer_number,
        amba_ik_filter_update_flags_t *p_update_flag,
        amba_ik_cr_index_t *p_table_index)
{
    uint8 bool_result;

    bool_result = non_equal_op_u32(p_update_flag->iso.exp0_fe_static_blc_updated, 0U) + non_equal_op_u32(p_update_flag->iso.exp1_fe_static_blc_updated, 0U) +
                  non_equal_op_u32(p_update_flag->iso.exp2_fe_static_blc_updated, 0U);
    if (bool_result != 0U) {
        p_table_index->cr_4_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_4_update, p_table_index->cr_4_index, buffer_number);
        p_update_flag->cr.cr_4_update = 1;

        p_table_index->cr_45_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_45_update, p_table_index->cr_45_index, buffer_number);
        p_update_flag->cr.cr_45_update = 1;

        p_table_index->cr_6_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_6_update, p_table_index->cr_6_index, buffer_number);
        p_update_flag->cr.cr_6_update = 1;

        p_table_index->cr_7_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_7_update, p_table_index->cr_7_index, buffer_number);
        p_update_flag->cr.cr_7_update = 1;

        p_table_index->cr_8_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_8_update, p_table_index->cr_8_index, buffer_number);
        p_update_flag->cr.cr_8_update = 1;
    }

    bool_result = non_equal_op_u32(p_update_flag->iso.exp0_fe_wb_gain_updated, 0U) + non_equal_op_u32(p_update_flag->iso.exp1_fe_wb_gain_updated, 0U) +
                  non_equal_op_u32(p_update_flag->iso.exp2_fe_wb_gain_updated, 0U);
    if (bool_result != 0U) {
        p_table_index->cr_4_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_4_update, p_table_index->cr_4_index, buffer_number);
        p_update_flag->cr.cr_4_update = 1;

        p_table_index->cr_45_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_45_update, p_table_index->cr_45_index, buffer_number);
        p_update_flag->cr.cr_45_update = 1;
    }

    bool_result = non_equal_op_u32(p_update_flag->iso.ce_updated, 0U) + non_equal_op_u32(p_update_flag->iso.ce_input_table_updated, 0U) +
                  non_equal_op_u32(p_update_flag->iso.ce_out_table_updated, 0U);
    if (bool_result != 0U) {
        p_table_index->cr_13_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_13_update, p_table_index->cr_13_index, buffer_number);
        p_update_flag->cr.cr_13_update = 1;

        p_table_index->cr_14_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_14_update, p_table_index->cr_14_index, buffer_number);
        p_update_flag->cr.cr_14_update = 1;
    }

    if (p_update_flag->iso.ce_updated != 0U) {
        p_table_index->cr_49_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_49_update, p_table_index->cr_49_index, buffer_number);
        p_update_flag->cr.cr_49_update = 1;

        p_table_index->cr_50_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_50_update, p_table_index->cr_50_index, buffer_number);
        p_update_flag->cr.cr_50_update = 1;

        p_table_index->cr_51_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_51_update, p_table_index->cr_51_index, buffer_number);
        p_update_flag->cr.cr_51_update = 1;
    }

    if (p_update_flag->iso.ce_input_table_updated != 0U) {
        p_table_index->cr_51_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_51_update, p_table_index->cr_51_index, buffer_number);
        p_update_flag->cr.cr_51_update = 1;

        p_table_index->cr_52_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_52_update, p_table_index->cr_52_index, buffer_number);
        p_update_flag->cr.cr_52_update = 1;
    }

    if (p_update_flag->iso.hdr_blend_updated != 0U) {
        p_table_index->cr_4_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_4_update, p_table_index->cr_4_index, buffer_number);
        p_update_flag->cr.cr_4_update = 1;

        p_table_index->cr_45_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_45_update, p_table_index->cr_45_index, buffer_number);
        p_update_flag->cr.cr_45_update = 1;
    }

    if (p_update_flag->iso.fe_tone_curve_updated != 0U) {
        p_table_index->cr_4_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_4_update, p_table_index->cr_4_index, buffer_number);
        p_update_flag->cr.cr_4_update = 1;

        p_table_index->cr_5_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_5_update, p_table_index->cr_5_index, buffer_number);
        p_update_flag->cr.cr_5_update = 1;

        p_table_index->cr_45_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_45_update, p_table_index->cr_45_index, buffer_number);
        p_update_flag->cr.cr_45_update = 1;

        p_table_index->cr_46_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_46_update, p_table_index->cr_46_index, buffer_number);
        p_update_flag->cr.cr_46_update = 1;
    }

    if (p_update_flag->iso.resample_str_update != 0U) {
        p_table_index->cr_16_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_16_update, p_table_index->cr_16_index, buffer_number);
        p_update_flag->cr.cr_16_update = 1;

        p_table_index->cr_33_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_33_update, p_table_index->cr_33_index, buffer_number);
        p_update_flag->cr.cr_33_update = 1;

        p_table_index->cr_35_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_35_update, p_table_index->cr_35_index, buffer_number);
        p_update_flag->cr.cr_35_update = 1;

        p_table_index->cr_37_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_37_update, p_table_index->cr_37_index, buffer_number);
        p_update_flag->cr.cr_37_update = 1;

        p_table_index->cr_42_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_42_update, p_table_index->cr_42_index, buffer_number);
        p_update_flag->cr.cr_42_update = 1;
    }

    // handle for frame info
    if (p_update_flag->iso.hdr_raw_info_updated != 0U) {
        p_table_index->frame_info = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.frame_info, p_table_index->frame_info, buffer_number);
        p_update_flag->cr.frame_info = 1;
    }
}


static uint32 exe_get_next_cr_index(uint32 context_id,
                                    uint32 buffer_number,
                                    amba_ik_filter_update_flags_t *p_update_flag,
                                    amba_ik_cr_index_t *p_table_index)
{
    uint32 rval = IK_OK;
    uint32 window_calculate_updated = 0UL;
    amba_ik_context_entity_t *p_ctx;
    uint8 bool_result;

    if ((null_check_op(p_update_flag) + null_check_op(p_table_index)) != 0U) {
        rval = IK_ERR_0005;
    } else {
        bool_result = non_equal_op_u32(p_update_flag->iso.window_size_info_updated, 0U) + non_equal_op_u32(p_update_flag->iso.dmy_range_updated, 0U) +
                      non_equal_op_u32(p_update_flag->iso.dzoom_info_updated, 0U) + non_equal_op_u32(p_update_flag->iso.vin_active_win_updated, 0U) +
                      non_equal_op_u32(p_update_flag->iso.stitching_info_updated, 0U) + non_equal_op_u32(p_update_flag->iso.flip_mode_updated, 0U) +
                      non_equal_op_u32(p_update_flag->iso.warp_enable_updated, 0U);
        if (bool_result != 0U) {
            window_calculate_updated = 1UL;
        }

        if (window_calculate_updated != 0UL) {
            p_table_index->cawarp = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cawarp, p_table_index->cawarp, buffer_number);
            p_update_flag->cr.cawarp = 1;

            p_table_index->warp = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.warp, p_table_index->warp, buffer_number);
            p_update_flag->cr.warp = 1;

            p_table_index->aaa_data = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.aaa_data, p_table_index->aaa_data, buffer_number);
            p_update_flag->cr.aaa_data = 1;

            p_table_index->cr_4_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_4_update, p_table_index->cr_4_index, buffer_number);
            p_update_flag->cr.cr_4_update = 1;

            p_table_index->cr_9_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_9_update, p_table_index->cr_9_index, buffer_number);
            p_update_flag->cr.cr_9_update = 1;

            p_table_index->cr_10_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_10_update, p_table_index->cr_10_index, buffer_number);
            p_update_flag->cr.cr_10_update = 1;

            p_table_index->cr_11_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_11_update, p_table_index->cr_11_index, buffer_number);
            p_update_flag->cr.cr_11_update = 1;

            p_table_index->cr_12_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_12_update, p_table_index->cr_12_index, buffer_number);
            p_update_flag->cr.cr_12_update = 1;

            p_table_index->cr_13_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_13_update, p_table_index->cr_13_index, buffer_number);
            p_update_flag->cr.cr_13_update = 1;

            p_table_index->cr_16_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_16_update, p_table_index->cr_16_index, buffer_number);
            p_update_flag->cr.cr_16_update = 1;

            p_table_index->cr_17_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_17_update, p_table_index->cr_17_index, buffer_number);
            p_update_flag->cr.cr_17_update = 1;

            p_table_index->cr_21_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_21_update, p_table_index->cr_21_index, buffer_number);
            p_update_flag->cr.cr_21_update = 1;

            p_table_index->cr_28_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_28_update, p_table_index->cr_28_index, buffer_number);
            p_update_flag->cr.cr_28_update = 1;

            p_table_index->cr_30_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_30_update, p_table_index->cr_30_index, buffer_number);
            p_update_flag->cr.cr_30_update = 1;

            p_table_index->cr_33_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_33_update, p_table_index->cr_33_index, buffer_number);
            p_update_flag->cr.cr_33_update = 1;

            p_table_index->cr_34_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_34_update, p_table_index->cr_34_index, buffer_number);
            p_update_flag->cr.cr_34_update = 1;

            p_table_index->cr_35_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_35_update, p_table_index->cr_35_index, buffer_number);
            p_update_flag->cr.cr_35_update = 1;

            p_table_index->cr_36_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_36_update, p_table_index->cr_36_index, buffer_number);
            p_update_flag->cr.cr_36_update = 1;

            p_table_index->cr_37_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_37_update, p_table_index->cr_37_index, buffer_number);
            p_update_flag->cr.cr_37_update = 1;

            p_table_index->cr_42_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_42_update, p_table_index->cr_42_index, buffer_number);
            p_update_flag->cr.cr_42_update = 1;

            p_table_index->cr_43_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_43_update, p_table_index->cr_43_index, buffer_number);
            p_update_flag->cr.cr_43_update = 1;

            p_table_index->cr_44_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_44_update, p_table_index->cr_44_index, buffer_number);
            p_update_flag->cr.cr_44_update = 1;

            p_table_index->cr_45_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_45_update, p_table_index->cr_45_index, buffer_number);
            p_update_flag->cr.cr_45_update = 1;

            p_table_index->cr_47_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_47_update, p_table_index->cr_47_index, buffer_number);
            p_update_flag->cr.cr_47_update = 1;

            p_table_index->cr_48_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_48_update, p_table_index->cr_48_index, buffer_number);
            p_update_flag->cr.cr_48_update = 1;

            p_table_index->cr_49_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_49_update, p_table_index->cr_49_index, buffer_number);
            p_update_flag->cr.cr_49_update = 1;

            p_table_index->cr_50_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_50_update, p_table_index->cr_50_index, buffer_number);
            p_update_flag->cr.cr_50_update = 1;

            p_table_index->cr_51_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_51_update, p_table_index->cr_51_index, buffer_number);
            p_update_flag->cr.cr_51_update = 1;
        }
        //complexity fix below.
        exe_get_next_cr_index_part1(buffer_number, p_update_flag, p_table_index);
        exe_get_next_cr_index_part2(buffer_number, p_update_flag, p_table_index);
        exe_get_next_cr_index_part3(buffer_number, p_update_flag, p_table_index);
        exe_get_next_cr_index_part4(buffer_number, p_update_flag, p_table_index);

        rval |= img_ctx_get_context(context_id, &p_ctx);
        if(rval == IK_OK) {
            if(p_ctx->filters.input_param.crc_mismatch_error_injection != 0U) {
                p_table_index->cr_11_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_11_update, p_table_index->cr_11_index, buffer_number);
                p_update_flag->cr.cr_11_update = 1;

                p_table_index->cr_21_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_21_update, p_table_index->cr_21_index, buffer_number);
                p_update_flag->cr.cr_21_update = 1;

                p_table_index->cr_30_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_30_update, p_table_index->cr_30_index, buffer_number);
                p_update_flag->cr.cr_30_update = 1;

                p_table_index->cr_33_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_33_update, p_table_index->cr_33_index, buffer_number);
                p_update_flag->cr.cr_33_update = 1;

                p_table_index->cr_35_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_35_update, p_table_index->cr_35_index, buffer_number);
                p_update_flag->cr.cr_35_update = 1;

                p_table_index->cr_42_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_42_update, p_table_index->cr_42_index, buffer_number);
                p_update_flag->cr.cr_42_update = 1;

                p_table_index->cr_49_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_49_update, p_table_index->cr_49_index, buffer_number);
                p_update_flag->cr.cr_49_update = 1;

                p_table_index->cr_112_index = exe_get_next_ring_buffer_index_condition((uint8)p_update_flag->cr.cr_112_update, p_table_index->cr_112_index, buffer_number);
                p_update_flag->cr.cr_112_update = 1;
            }
        } else {
            amba_ik_system_print_str_5("[IK] can't get context", NULL, NULL, NULL, NULL, NULL);
        }
    }

    return rval;
}

static uint32 exe_check_next_cr_index(uint32 context_id, amba_ik_cr_id_list_t *p_update_flag, amba_ik_cr_index_t *p_table_index)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if(rval == IK_OK) {
        if((p_ctx->organization.attribute.ability.video_pipe == AMBA_IK_VIDEO_LINEAR) || (p_ctx->organization.attribute.ability.video_pipe == AMBA_IK_VIDEO_Y2Y)) {
            p_update_flag->cr_45_update = 0;
            p_update_flag->cr_46_update = 0;
            p_update_flag->cr_47_update = 0;
            p_update_flag->cr_48_update = 0;
            p_update_flag->cr_49_update = 0;
            p_update_flag->cr_50_update = 0;
            p_update_flag->cr_51_update = 0;
            p_update_flag->cr_52_update = 0;

            p_table_index->cr_45_index = 0;
            p_table_index->cr_46_index = 0;
            p_table_index->cr_47_index = 0;
            p_table_index->cr_48_index = 0;
            p_table_index->cr_49_index = 0;
            p_table_index->cr_50_index = 0;
            p_table_index->cr_51_index = 0;
            p_table_index->cr_52_index = 0;
        }
    } else {
        amba_ik_system_print_str_5("[IK] can't get context", NULL, NULL, NULL, NULL, NULL);
        rval = IK_ERR_0000;
    }

    return rval;
}

static inline uint32 exe_update_flow_tbl_list_cr4_cr20(uint32 context_id, const amba_ik_cr_index_t *p_table_index, const amba_ik_cr_id_list_t *p_cr_upd_flag, amba_ik_flow_tables_list_t *p_flow_tbl_list)
{
    uint32 rval = IK_OK;
    uintptr addr = 0;
    amba_ik_flow_tables_t *p_flow_tbl = NULL;

    if(p_cr_upd_flag->cr_4_update != 0U) {
        rval |= img_ctx_get_flow_tbl(context_id, p_table_index->cr_4_index, &addr);
        (void)amba_ik_system_memcpy(&p_flow_tbl, &addr, sizeof(void *));
        (void)amba_ik_system_memcpy(p_flow_tbl->CR_buf_4, p_flow_tbl_list->p_CR_buf_4, CR_SIZE_4);
        p_flow_tbl_list->p_CR_buf_4 = ptr2ptr(p_flow_tbl->CR_buf_4);
    }
    if(p_cr_upd_flag->cr_5_update != 0U) { // TBD, I think we don't need copy table, replace it by re-calculation
        rval |= img_ctx_get_flow_tbl(context_id, p_table_index->cr_5_index, &addr);
        (void)amba_ik_system_memcpy(&p_flow_tbl, &addr, sizeof(void *));
        (void)amba_ik_system_memcpy(p_flow_tbl->CR_buf_5, p_flow_tbl_list->p_CR_buf_5, CR_SIZE_5);
        p_flow_tbl_list->p_CR_buf_5 = ptr2ptr(p_flow_tbl->CR_buf_5);
    }
    if(p_cr_upd_flag->cr_6_update != 0U) {
        rval |= img_ctx_get_flow_tbl(context_id, p_table_index->cr_6_index, &addr);
        (void)amba_ik_system_memcpy(&p_flow_tbl, &addr, sizeof(void *));
        (void)amba_ik_system_memcpy(p_flow_tbl->CR_buf_6, p_flow_tbl_list->p_CR_buf_6, CR_SIZE_6);
        p_flow_tbl_list->p_CR_buf_6 = ptr2ptr(p_flow_tbl->CR_buf_6);
    }
    if(p_cr_upd_flag->cr_7_update != 0U) {
        rval |= img_ctx_get_flow_tbl(context_id, p_table_index->cr_7_index, &addr);
        (void)amba_ik_system_memcpy(&p_flow_tbl, &addr, sizeof(void *));
        (void)amba_ik_system_memcpy(p_flow_tbl->CR_buf_7, p_flow_tbl_list->p_CR_buf_7, CR_SIZE_7);
        p_flow_tbl_list->p_CR_buf_7 = ptr2ptr(p_flow_tbl->CR_buf_7);
    }
    if(p_cr_upd_flag->cr_8_update != 0U) {
        rval |= img_ctx_get_flow_tbl(context_id, p_table_index->cr_8_index, &addr);
        (void)amba_ik_system_memcpy(&p_flow_tbl, &addr, sizeof(void *));
        (void)amba_ik_system_memcpy(p_flow_tbl->CR_buf_8, p_flow_tbl_list->p_CR_buf_8, CR_SIZE_8);
        p_flow_tbl_list->p_CR_buf_8 = ptr2ptr(p_flow_tbl->CR_buf_8);
    }
    if(p_cr_upd_flag->cr_9_update != 0U) {
        rval |= img_ctx_get_flow_tbl(context_id, p_table_index->cr_9_index, &addr);
        (void)amba_ik_system_memcpy(&p_flow_tbl, &addr, sizeof(void *));
        (void)amba_ik_system_memcpy(p_flow_tbl->CR_buf_9, p_flow_tbl_list->p_CR_buf_9, CR_SIZE_9);
        p_flow_tbl_list->p_CR_buf_9 = ptr2ptr(p_flow_tbl->CR_buf_9);
    }
    if(p_cr_upd_flag->cr_10_update != 0U) { // TBD, I think we don't need copy table, replace it by re-calculation
        rval |= img_ctx_get_flow_tbl(context_id, p_table_index->cr_10_index, &addr);
        (void)amba_ik_system_memcpy(&p_flow_tbl, &addr, sizeof(void *));
        (void)amba_ik_system_memcpy(p_flow_tbl->CR_buf_10, p_flow_tbl_list->p_CR_buf_10, CR_SIZE_10);
        p_flow_tbl_list->p_CR_buf_10 = ptr2ptr(p_flow_tbl->CR_buf_10);
    }
    if(p_cr_upd_flag->cr_11_update != 0U) {
        rval |= img_ctx_get_flow_tbl(context_id, p_table_index->cr_11_index, &addr);
        (void)amba_ik_system_memcpy(&p_flow_tbl, &addr, sizeof(void *));
        (void)amba_ik_system_memcpy(p_flow_tbl->CR_buf_11, p_flow_tbl_list->p_CR_buf_11, CR_SIZE_11);
        p_flow_tbl_list->p_CR_buf_11 = ptr2ptr(p_flow_tbl->CR_buf_11);
    }
    if(p_cr_upd_flag->cr_12_update != 0U) {
        rval |= img_ctx_get_flow_tbl(context_id, p_table_index->cr_12_index, &addr);
        (void)amba_ik_system_memcpy(&p_flow_tbl, &addr, sizeof(void *));
        (void)amba_ik_system_memcpy(p_flow_tbl->CR_buf_12, p_flow_tbl_list->p_CR_buf_12, CR_SIZE_12);
        p_flow_tbl_list->p_CR_buf_12 = ptr2ptr(p_flow_tbl->CR_buf_12);
    }
    if(p_cr_upd_flag->cr_13_update != 0U) {
        rval |= img_ctx_get_flow_tbl(context_id, p_table_index->cr_13_index, &addr);
        (void)amba_ik_system_memcpy(&p_flow_tbl, &addr, sizeof(void *));
        (void)amba_ik_system_memcpy(p_flow_tbl->CR_buf_13, p_flow_tbl_list->p_CR_buf_13, CR_SIZE_13);
        p_flow_tbl_list->p_CR_buf_13 = ptr2ptr(p_flow_tbl->CR_buf_13);
    }
    if(p_cr_upd_flag->cr_14_update != 0U) { // TBD, I think we don't need copy table, replace it by re-calculation
        rval |= img_ctx_get_flow_tbl(context_id, p_table_index->cr_14_index, &addr);
        (void)amba_ik_system_memcpy(&p_flow_tbl, &addr, sizeof(void *));
        (void)amba_ik_system_memcpy(p_flow_tbl->CR_buf_14, p_flow_tbl_list->p_CR_buf_14, CR_SIZE_14);
        p_flow_tbl_list->p_CR_buf_14 = ptr2ptr(p_flow_tbl->CR_buf_14);
    }
    //never happens, for coverage testing.
    /*if(p_cr_upd_flag->cr_15_update != 0U) {
        rval |= img_ctx_get_flow_tbl(context_id, p_table_index->cr_15_index, &addr);
        (void)amba_ik_system_memcpy(&p_flow_tbl, &addr, sizeof(void *));
        (void)amba_ik_system_memcpy(p_flow_tbl->CR_buf_15, p_flow_tbl_list->p_CR_buf_15, CR_SIZE_15);
        p_flow_tbl_list->p_CR_buf_15 = ptr2ptr(p_flow_tbl->CR_buf_15);
    }*/
    if(p_cr_upd_flag->cr_16_update != 0U) {
        rval |= img_ctx_get_flow_tbl(context_id, p_table_index->cr_16_index, &addr);
        (void)amba_ik_system_memcpy(&p_flow_tbl, &addr, sizeof(void *));
        (void)amba_ik_system_memcpy(p_flow_tbl->CR_buf_16, p_flow_tbl_list->p_CR_buf_16, CR_SIZE_16);
        p_flow_tbl_list->p_CR_buf_16 = ptr2ptr(p_flow_tbl->CR_buf_16);
    }
    if(p_cr_upd_flag->cr_17_update != 0U) { // TBD, I think we don't need copy table, replace it by re-calculation
        rval |= img_ctx_get_flow_tbl(context_id, p_table_index->cr_17_index, &addr);
        (void)amba_ik_system_memcpy(&p_flow_tbl, &addr, sizeof(void *));
        (void)amba_ik_system_memcpy(p_flow_tbl->CR_buf_17, p_flow_tbl_list->p_CR_buf_17, CR_SIZE_17);
        p_flow_tbl_list->p_CR_buf_17 = ptr2ptr(p_flow_tbl->CR_buf_17);

        (void)amba_ik_system_memcpy(p_flow_tbl->CR_buf_18, p_flow_tbl_list->p_CR_buf_18, CR_SIZE_18);
        p_flow_tbl_list->p_CR_buf_18 = ptr2ptr(p_flow_tbl->CR_buf_18);

        (void)amba_ik_system_memcpy(p_flow_tbl->CR_buf_19, p_flow_tbl_list->p_CR_buf_19, CR_SIZE_19);
        p_flow_tbl_list->p_CR_buf_19 = ptr2ptr(p_flow_tbl->CR_buf_19);

        (void)amba_ik_system_memcpy(p_flow_tbl->CR_buf_20, p_flow_tbl_list->p_CR_buf_20, CR_SIZE_20);
        p_flow_tbl_list->p_CR_buf_20 = ptr2ptr(p_flow_tbl->CR_buf_20);
    }

    return rval;
}

static inline uint32 exe_update_flow_tbl_list_cr21_cr34(uint32 context_id, const amba_ik_cr_index_t *p_table_index, const amba_ik_cr_id_list_t *p_cr_upd_flag, amba_ik_flow_tables_list_t *p_flow_tbl_list)
{
    uint32 rval = IK_OK;
    uintptr addr = 0;
    amba_ik_flow_tables_t *p_flow_tbl = NULL;

    if(p_cr_upd_flag->cr_21_update != 0U) {
        rval |= img_ctx_get_flow_tbl(context_id, p_table_index->cr_21_index, &addr);
        (void)amba_ik_system_memcpy(&p_flow_tbl, &addr, sizeof(void *));
        (void)amba_ik_system_memcpy(p_flow_tbl->CR_buf_21, p_flow_tbl_list->p_CR_buf_21, CR_SIZE_21);
        p_flow_tbl_list->p_CR_buf_21 = ptr2ptr(p_flow_tbl->CR_buf_21);
    }
    if(p_cr_upd_flag->cr_22_update != 0U) {
        rval |= img_ctx_get_flow_tbl(context_id, p_table_index->cr_22_index, &addr);
        (void)amba_ik_system_memcpy(&p_flow_tbl, &addr, sizeof(void *));
        (void)amba_ik_system_memcpy(p_flow_tbl->CR_buf_22, p_flow_tbl_list->p_CR_buf_22, CR_SIZE_22);
        p_flow_tbl_list->p_CR_buf_22 = ptr2ptr(p_flow_tbl->CR_buf_22);
    }
    if(p_cr_upd_flag->cr_23_update != 0U) {
        rval |= img_ctx_get_flow_tbl(context_id, p_table_index->cr_23_index, &addr);
        (void)amba_ik_system_memcpy(&p_flow_tbl, &addr, sizeof(void *));
        (void)amba_ik_system_memcpy(p_flow_tbl->CR_buf_23, p_flow_tbl_list->p_CR_buf_23, CR_SIZE_23);
        p_flow_tbl_list->p_CR_buf_23 = ptr2ptr(p_flow_tbl->CR_buf_23);
    }
    if(p_cr_upd_flag->cr_24_update != 0U) {
        rval |= img_ctx_get_flow_tbl(context_id, p_table_index->cr_24_index, &addr);
        (void)amba_ik_system_memcpy(&p_flow_tbl, &addr, sizeof(void *));
        (void)amba_ik_system_memcpy(p_flow_tbl->CR_buf_24, p_flow_tbl_list->p_CR_buf_24, CR_SIZE_24);
        p_flow_tbl_list->p_CR_buf_24 = ptr2ptr(p_flow_tbl->CR_buf_24);
    }
    if(p_cr_upd_flag->cr_25_update != 0U) {
        rval |= img_ctx_get_flow_tbl(context_id, p_table_index->cr_25_index, &addr);
        (void)amba_ik_system_memcpy(&p_flow_tbl, &addr, sizeof(void *));
        (void)amba_ik_system_memcpy(p_flow_tbl->CR_buf_25, p_flow_tbl_list->p_CR_buf_25, CR_SIZE_25);
        p_flow_tbl_list->p_CR_buf_25 = ptr2ptr(p_flow_tbl->CR_buf_25);
    }
    if(p_cr_upd_flag->cr_26_update != 0U) {
        rval |= img_ctx_get_flow_tbl(context_id, p_table_index->cr_26_index, &addr);
        (void)amba_ik_system_memcpy(&p_flow_tbl, &addr, sizeof(void *));
        (void)amba_ik_system_memcpy(p_flow_tbl->CR_buf_26, p_flow_tbl_list->p_CR_buf_26, CR_SIZE_26);
        p_flow_tbl_list->p_CR_buf_26 = ptr2ptr(p_flow_tbl->CR_buf_26);
    }
    if(p_cr_upd_flag->cr_27_update != 0U) { // TBD, I think we don't need copy table, replace it by re-calculation
        rval |= img_ctx_get_flow_tbl(context_id, p_table_index->cr_27_index, &addr);
        (void)amba_ik_system_memcpy(&p_flow_tbl, &addr, sizeof(void *));
        (void)amba_ik_system_memcpy(p_flow_tbl->CR_buf_27, p_flow_tbl_list->p_CR_buf_27, CR_SIZE_27);
        p_flow_tbl_list->p_CR_buf_27 = ptr2ptr(p_flow_tbl->CR_buf_27);
    }
    if(p_cr_upd_flag->cr_28_update != 0U) {
        rval |= img_ctx_get_flow_tbl(context_id, p_table_index->cr_28_index, &addr);
        (void)amba_ik_system_memcpy(&p_flow_tbl, &addr, sizeof(void *));
        (void)amba_ik_system_memcpy(p_flow_tbl->CR_buf_28, p_flow_tbl_list->p_CR_buf_28, CR_SIZE_28);
        p_flow_tbl_list->p_CR_buf_28 = ptr2ptr(p_flow_tbl->CR_buf_28);
    }
    if(p_cr_upd_flag->cr_29_update != 0U) {
        rval |= img_ctx_get_flow_tbl(context_id, p_table_index->cr_29_index, &addr);
        (void)amba_ik_system_memcpy(&p_flow_tbl, &addr, sizeof(void *));
        (void)amba_ik_system_memcpy(p_flow_tbl->CR_buf_29, p_flow_tbl_list->p_CR_buf_29, CR_SIZE_29);
        p_flow_tbl_list->p_CR_buf_29 = ptr2ptr(p_flow_tbl->CR_buf_29);
    }
    if(p_cr_upd_flag->cr_30_update != 0U) {
        rval |= img_ctx_get_flow_tbl(context_id, p_table_index->cr_30_index, &addr);
        (void)amba_ik_system_memcpy(&p_flow_tbl, &addr, sizeof(void *));
        (void)amba_ik_system_memcpy(p_flow_tbl->CR_buf_30, p_flow_tbl_list->p_CR_buf_30, CR_SIZE_30);
        p_flow_tbl_list->p_CR_buf_30 = ptr2ptr(p_flow_tbl->CR_buf_30);
    }
    if(p_cr_upd_flag->cr_31_update != 0U) {
        rval |= img_ctx_get_flow_tbl(context_id, p_table_index->cr_31_index, &addr);
        (void)amba_ik_system_memcpy(&p_flow_tbl, &addr, sizeof(void *));
        (void)amba_ik_system_memcpy(p_flow_tbl->CR_buf_31, p_flow_tbl_list->p_CR_buf_31, CR_SIZE_31);
        p_flow_tbl_list->p_CR_buf_31 = ptr2ptr(p_flow_tbl->CR_buf_31);
    }
    //never happens, for coverage testing.
    /*if(p_cr_upd_flag->cr_32_update != 0U) {
        rval |= img_ctx_get_flow_tbl(context_id, p_table_index->cr_32_index, &addr);
        (void)amba_ik_system_memcpy(&p_flow_tbl, &addr, sizeof(void *));
        (void)amba_ik_system_memcpy(p_flow_tbl->CR_buf_32, p_flow_tbl_list->p_CR_buf_32, CR_SIZE_32);
        p_flow_tbl_list->p_CR_buf_32 = ptr2ptr(p_flow_tbl->CR_buf_32);
    }*/
    if(p_cr_upd_flag->cr_33_update != 0U) {
        rval |= img_ctx_get_flow_tbl(context_id, p_table_index->cr_33_index, &addr);
        (void)amba_ik_system_memcpy(&p_flow_tbl, &addr, sizeof(void *));
        (void)amba_ik_system_memcpy(p_flow_tbl->CR_buf_33, p_flow_tbl_list->p_CR_buf_33, CR_SIZE_33);
        p_flow_tbl_list->p_CR_buf_33 = ptr2ptr(p_flow_tbl->CR_buf_33);
    }
    if(p_cr_upd_flag->cr_34_update != 0U) { // TBD, I think we don't need copy table, replace it by re-calculation
        rval |= img_ctx_get_flow_tbl(context_id, p_table_index->cr_34_index, &addr);
        (void)amba_ik_system_memcpy(&p_flow_tbl, &addr, sizeof(void *));
        (void)amba_ik_system_memcpy(p_flow_tbl->CR_buf_34, p_flow_tbl_list->p_CR_buf_34, CR_SIZE_34);
        p_flow_tbl_list->p_CR_buf_34 = ptr2ptr(p_flow_tbl->CR_buf_34);
    }

    return rval;
}

static inline uint32 exe_update_flow_tbl_list_cr35_cr48(uint32 context_id, const amba_ik_cr_index_t *p_table_index, const amba_ik_cr_id_list_t *p_cr_upd_flag, amba_ik_flow_tables_list_t *p_flow_tbl_list)
{
    uint32 rval = IK_OK;
    uintptr addr = 0;
    amba_ik_flow_tables_t *p_flow_tbl = NULL;

    if(p_cr_upd_flag->cr_35_update != 0U) {
        rval |= img_ctx_get_flow_tbl(context_id, p_table_index->cr_35_index, &addr);
        (void)amba_ik_system_memcpy(&p_flow_tbl, &addr, sizeof(void *));
        (void)amba_ik_system_memcpy(p_flow_tbl->CR_buf_35, p_flow_tbl_list->p_CR_buf_35, CR_SIZE_35);
        p_flow_tbl_list->p_CR_buf_35 = ptr2ptr(p_flow_tbl->CR_buf_35);
    }
    if(p_cr_upd_flag->cr_36_update != 0U) {
        rval |= img_ctx_get_flow_tbl(context_id, p_table_index->cr_36_index, &addr);
        (void)amba_ik_system_memcpy(&p_flow_tbl, &addr, sizeof(void *));
        (void)amba_ik_system_memcpy(p_flow_tbl->CR_buf_36, p_flow_tbl_list->p_CR_buf_36, CR_SIZE_36);
        p_flow_tbl_list->p_CR_buf_36 = ptr2ptr(p_flow_tbl->CR_buf_36);
    }
    if(p_cr_upd_flag->cr_37_update != 0U) {
        rval |= img_ctx_get_flow_tbl(context_id, p_table_index->cr_37_index, &addr);
        (void)amba_ik_system_memcpy(&p_flow_tbl, &addr, sizeof(void *));
        (void)amba_ik_system_memcpy(p_flow_tbl->CR_buf_37, p_flow_tbl_list->p_CR_buf_37, CR_SIZE_37);
        p_flow_tbl_list->p_CR_buf_37 = ptr2ptr(p_flow_tbl->CR_buf_37);
    }
    //never happens, for coverage testing.
    /*if(p_cr_upd_flag->cr_38_update != 0U) {
        rval |= img_ctx_get_flow_tbl(context_id, p_table_index->cr_38_index, &addr);
        (void)amba_ik_system_memcpy(&p_flow_tbl, &addr, sizeof(void *));
        (void)amba_ik_system_memcpy(p_flow_tbl->CR_buf_38, p_flow_tbl_list->p_CR_buf_38, CR_SIZE_38);
        p_flow_tbl_list->p_CR_buf_38 = ptr2ptr(p_flow_tbl->CR_buf_38);
    }
    if(p_cr_upd_flag->cr_39_update != 0U) {
        rval |= img_ctx_get_flow_tbl(context_id, p_table_index->cr_39_index, &addr);
        (void)amba_ik_system_memcpy(&p_flow_tbl, &addr, sizeof(void *));
        (void)amba_ik_system_memcpy(p_flow_tbl->CR_buf_39, p_flow_tbl_list->p_CR_buf_39, CR_SIZE_39);
        p_flow_tbl_list->p_CR_buf_39 = ptr2ptr(p_flow_tbl->CR_buf_39);
    }
    if(p_cr_upd_flag->cr_40_update != 0U) {
        rval |= img_ctx_get_flow_tbl(context_id, p_table_index->cr_40_index, &addr);
        (void)amba_ik_system_memcpy(&p_flow_tbl, &addr, sizeof(void *));
        (void)amba_ik_system_memcpy(p_flow_tbl->CR_buf_40, p_flow_tbl_list->p_CR_buf_40, CR_SIZE_40);
        p_flow_tbl_list->p_CR_buf_40 = ptr2ptr(p_flow_tbl->CR_buf_40);
    }
    if(p_cr_upd_flag->cr_41_update != 0U) {
        rval |= img_ctx_get_flow_tbl(context_id, p_table_index->cr_41_index, &addr);
        (void)amba_ik_system_memcpy(&p_flow_tbl, &addr, sizeof(void *));
        (void)amba_ik_system_memcpy(p_flow_tbl->CR_buf_41, p_flow_tbl_list->p_CR_buf_41, CR_SIZE_41);
        p_flow_tbl_list->p_CR_buf_41 = ptr2ptr(p_flow_tbl->CR_buf_41);
    }*/
    if(p_cr_upd_flag->cr_42_update != 0U) {
        rval |= img_ctx_get_flow_tbl(context_id, p_table_index->cr_42_index, &addr);
        (void)amba_ik_system_memcpy(&p_flow_tbl, &addr, sizeof(void *));
        (void)amba_ik_system_memcpy(p_flow_tbl->CR_buf_42, p_flow_tbl_list->p_CR_buf_42, CR_SIZE_42);
        p_flow_tbl_list->p_CR_buf_42 = ptr2ptr(p_flow_tbl->CR_buf_42);
    }
    if(p_cr_upd_flag->cr_43_update != 0U) { // TBD, I think we don't need copy table, replace it by re-calculation
        rval |= img_ctx_get_flow_tbl(context_id, p_table_index->cr_43_index, &addr);
        (void)amba_ik_system_memcpy(&p_flow_tbl, &addr, sizeof(void *));
        (void)amba_ik_system_memcpy(p_flow_tbl->CR_buf_43, p_flow_tbl_list->p_CR_buf_43, CR_SIZE_43);
        p_flow_tbl_list->p_CR_buf_43 = ptr2ptr(p_flow_tbl->CR_buf_43);
    }
    if(p_cr_upd_flag->cr_44_update != 0U) {
        rval |= img_ctx_get_flow_tbl(context_id, p_table_index->cr_44_index, &addr);
        (void)amba_ik_system_memcpy(&p_flow_tbl, &addr, sizeof(void *));
        (void)amba_ik_system_memcpy(p_flow_tbl->CR_buf_44, p_flow_tbl_list->p_CR_buf_44, CR_SIZE_44);
        p_flow_tbl_list->p_CR_buf_44 = ptr2ptr(p_flow_tbl->CR_buf_44);
    }
    if(p_cr_upd_flag->cr_45_update != 0U) {
        rval |= img_ctx_get_flow_tbl(context_id, p_table_index->cr_45_index, &addr);
        (void)amba_ik_system_memcpy(&p_flow_tbl, &addr, sizeof(void *));
        (void)amba_ik_system_memcpy(p_flow_tbl->CR_buf_45, p_flow_tbl_list->p_CR_buf_45, CR_SIZE_45);
        p_flow_tbl_list->p_CR_buf_45 = ptr2ptr(p_flow_tbl->CR_buf_45);
    }
    if(p_cr_upd_flag->cr_46_update != 0U) { // TBD, I think we don't need copy table, replace it by re-calculation
        rval |= img_ctx_get_flow_tbl(context_id, p_table_index->cr_46_index, &addr);
        (void)amba_ik_system_memcpy(&p_flow_tbl, &addr, sizeof(void *));
        (void)amba_ik_system_memcpy(p_flow_tbl->CR_buf_46, p_flow_tbl_list->p_CR_buf_46, CR_SIZE_46);
        p_flow_tbl_list->p_CR_buf_46 = ptr2ptr(p_flow_tbl->CR_buf_46);
    }
    if(p_cr_upd_flag->cr_47_update != 0U) {
        rval |= img_ctx_get_flow_tbl(context_id, p_table_index->cr_47_index, &addr);
        (void)amba_ik_system_memcpy(&p_flow_tbl, &addr, sizeof(void *));
        (void)amba_ik_system_memcpy(p_flow_tbl->CR_buf_47, p_flow_tbl_list->p_CR_buf_47, CR_SIZE_47);
        p_flow_tbl_list->p_CR_buf_47 = ptr2ptr(p_flow_tbl->CR_buf_47);
    }
    if(p_cr_upd_flag->cr_48_update != 0U) { // TBD, I think we don't need copy table, replace it by re-calculation
        rval |= img_ctx_get_flow_tbl(context_id, p_table_index->cr_48_index, &addr);
        (void)amba_ik_system_memcpy(&p_flow_tbl, &addr, sizeof(void *));
        (void)amba_ik_system_memcpy(p_flow_tbl->CR_buf_48, p_flow_tbl_list->p_CR_buf_48, CR_SIZE_48);
        p_flow_tbl_list->p_CR_buf_48 = ptr2ptr(p_flow_tbl->CR_buf_48);
    }

    return rval;
}

static inline uint32 exe_update_flow_tbl_list_cr49_cr52(uint32 context_id, const amba_ik_cr_index_t *p_table_index, const amba_ik_cr_id_list_t *p_cr_upd_flag, amba_ik_flow_tables_list_t *p_flow_tbl_list)
{
    uint32 rval = IK_OK;
    uintptr addr = 0;
    amba_ik_flow_tables_t *p_flow_tbl = NULL;

    if(p_cr_upd_flag->cr_49_update != 0U) {
        rval |= img_ctx_get_flow_tbl(context_id, p_table_index->cr_49_index, &addr);
        (void)amba_ik_system_memcpy(&p_flow_tbl, &addr, sizeof(void *));
        (void)amba_ik_system_memcpy(p_flow_tbl->CR_buf_49, p_flow_tbl_list->p_CR_buf_49, CR_SIZE_49);
        p_flow_tbl_list->p_CR_buf_49 = ptr2ptr(p_flow_tbl->CR_buf_49);
    }
    if(p_cr_upd_flag->cr_50_update != 0U) {
        rval |= img_ctx_get_flow_tbl(context_id, p_table_index->cr_50_index, &addr);
        (void)amba_ik_system_memcpy(&p_flow_tbl, &addr, sizeof(void *));
        (void)amba_ik_system_memcpy(p_flow_tbl->CR_buf_50, p_flow_tbl_list->p_CR_buf_50, CR_SIZE_50);
        p_flow_tbl_list->p_CR_buf_50 = ptr2ptr(p_flow_tbl->CR_buf_50);
    }
    if(p_cr_upd_flag->cr_51_update != 0U) {
        rval |= img_ctx_get_flow_tbl(context_id, p_table_index->cr_51_index, &addr);
        (void)amba_ik_system_memcpy(&p_flow_tbl, &addr, sizeof(void *));
        (void)amba_ik_system_memcpy(p_flow_tbl->CR_buf_51, p_flow_tbl_list->p_CR_buf_51, CR_SIZE_51);
        p_flow_tbl_list->p_CR_buf_51 = ptr2ptr(p_flow_tbl->CR_buf_51);
    }
    if(p_cr_upd_flag->cr_52_update != 0U) { // TBD, I think we don't need copy table, replace it by re-calculation
        rval |= img_ctx_get_flow_tbl(context_id, p_table_index->cr_52_index, &addr);
        (void)amba_ik_system_memcpy(&p_flow_tbl, &addr, sizeof(void *));
        (void)amba_ik_system_memcpy(p_flow_tbl->CR_buf_52, p_flow_tbl_list->p_CR_buf_52, CR_SIZE_52);
        p_flow_tbl_list->p_CR_buf_52 = ptr2ptr(p_flow_tbl->CR_buf_52);
    }

    return rval;
}

static inline uint32 exe_update_flow_tbl_list_cr111_cr121(uint32 context_id, const amba_ik_cr_index_t *p_table_index, const amba_ik_cr_id_list_t *p_cr_upd_flag, amba_ik_flow_tables_list_t *p_flow_tbl_list)
{
    uint32 rval = IK_OK;
    uintptr addr = 0;
    amba_ik_flow_tables_t *p_flow_tbl = NULL;

    if(p_cr_upd_flag->cr_111_update != 0U) {
        rval |= img_ctx_get_flow_tbl(context_id, p_table_index->cr_111_index, &addr);
        (void)amba_ik_system_memcpy(&p_flow_tbl, &addr, sizeof(void *));
        (void)amba_ik_system_memcpy(p_flow_tbl->CR_buf_111, p_flow_tbl_list->p_CR_buf_111, CR_SIZE_111);
        p_flow_tbl_list->p_CR_buf_111 = ptr2ptr(p_flow_tbl->CR_buf_111);
    }
    if(p_cr_upd_flag->cr_112_update != 0U) {
        rval |= img_ctx_get_flow_tbl(context_id, p_table_index->cr_112_index, &addr);
        (void)amba_ik_system_memcpy(&p_flow_tbl, &addr, sizeof(void *));
        (void)amba_ik_system_memcpy(p_flow_tbl->CR_buf_112, p_flow_tbl_list->p_CR_buf_112, CR_SIZE_112);
        p_flow_tbl_list->p_CR_buf_112 = ptr2ptr(p_flow_tbl->CR_buf_112);
    }
    if(p_cr_upd_flag->cr_113_update != 0U) { // TBD, I think we don't need copy table, replace it by re-calculation
        rval |= img_ctx_get_flow_tbl(context_id, p_table_index->cr_113_index, &addr);
        (void)amba_ik_system_memcpy(&p_flow_tbl, &addr, sizeof(void *));
        (void)amba_ik_system_memcpy(p_flow_tbl->CR_buf_113, p_flow_tbl_list->p_CR_buf_113, CR_SIZE_113);
        p_flow_tbl_list->p_CR_buf_113 = ptr2ptr(p_flow_tbl->CR_buf_113);
    }
    if(p_cr_upd_flag->cr_114_update != 0U) { // TBD, I think we don't need copy table, replace it by re-calculation
        rval |= img_ctx_get_flow_tbl(context_id, p_table_index->cr_114_index, &addr);
        (void)amba_ik_system_memcpy(&p_flow_tbl, &addr, sizeof(void *));
        (void)amba_ik_system_memcpy(p_flow_tbl->CR_buf_114, p_flow_tbl_list->p_CR_buf_114, CR_SIZE_114);
        p_flow_tbl_list->p_CR_buf_114 = ptr2ptr(p_flow_tbl->CR_buf_114);
    }
    if(p_cr_upd_flag->cr_115_update != 0U) { // TBD, I think we don't need copy table, replace it by re-calculation
        rval |= img_ctx_get_flow_tbl(context_id, p_table_index->cr_115_index, &addr);
        (void)amba_ik_system_memcpy(&p_flow_tbl, &addr, sizeof(void *));
        (void)amba_ik_system_memcpy(p_flow_tbl->CR_buf_115, p_flow_tbl_list->p_CR_buf_115, CR_SIZE_115);
        p_flow_tbl_list->p_CR_buf_115 = ptr2ptr(p_flow_tbl->CR_buf_115);
    }
    if(p_cr_upd_flag->cr_116_update != 0U) {
        rval |= img_ctx_get_flow_tbl(context_id, p_table_index->cr_116_index, &addr);
        (void)amba_ik_system_memcpy(&p_flow_tbl, &addr, sizeof(void *));
        (void)amba_ik_system_memcpy(p_flow_tbl->CR_buf_116, p_flow_tbl_list->p_CR_buf_116, CR_SIZE_116);
        p_flow_tbl_list->p_CR_buf_116 = ptr2ptr(p_flow_tbl->CR_buf_116);
    }
    if(p_cr_upd_flag->cr_117_update != 0U) {
        rval |= img_ctx_get_flow_tbl(context_id, p_table_index->cr_117_index, &addr);
        (void)amba_ik_system_memcpy(&p_flow_tbl, &addr, sizeof(void *));
        (void)amba_ik_system_memcpy(p_flow_tbl->CR_buf_117, p_flow_tbl_list->p_CR_buf_117, CR_SIZE_117);
        p_flow_tbl_list->p_CR_buf_117 = ptr2ptr(p_flow_tbl->CR_buf_117);
    }
    if(p_cr_upd_flag->cr_118_update != 0U) { // TBD, I think we don't need copy table, replace it by re-calculation
        rval |= img_ctx_get_flow_tbl(context_id, p_table_index->cr_118_index, &addr);
        (void)amba_ik_system_memcpy(&p_flow_tbl, &addr, sizeof(void *));
        (void)amba_ik_system_memcpy(p_flow_tbl->CR_buf_118, p_flow_tbl_list->p_CR_buf_118, CR_SIZE_118);
        p_flow_tbl_list->p_CR_buf_118 = ptr2ptr(p_flow_tbl->CR_buf_118);
    }
    if(p_cr_upd_flag->cr_119_update != 0U) { // TBD, I think we don't need copy table, replace it by re-calculation
        rval |= img_ctx_get_flow_tbl(context_id, p_table_index->cr_119_index, &addr);
        (void)amba_ik_system_memcpy(&p_flow_tbl, &addr, sizeof(void *));
        (void)amba_ik_system_memcpy(p_flow_tbl->CR_buf_119, p_flow_tbl_list->p_CR_buf_119, CR_SIZE_119);
        p_flow_tbl_list->p_CR_buf_119 = ptr2ptr(p_flow_tbl->CR_buf_119);
    }
    if(p_cr_upd_flag->cr_120_update != 0U) {
        rval |= img_ctx_get_flow_tbl(context_id, p_table_index->cr_120_index, &addr);
        (void)amba_ik_system_memcpy(&p_flow_tbl, &addr, sizeof(void *));
        (void)amba_ik_system_memcpy(p_flow_tbl->CR_buf_120, p_flow_tbl_list->p_CR_buf_120, CR_SIZE_120);
        p_flow_tbl_list->p_CR_buf_120 = ptr2ptr(p_flow_tbl->CR_buf_120);
    }
    if(p_cr_upd_flag->cr_121_update != 0U) {
        rval |= img_ctx_get_flow_tbl(context_id, p_table_index->cr_121_index, &addr);
        (void)amba_ik_system_memcpy(&p_flow_tbl, &addr, sizeof(void *));
        (void)amba_ik_system_memcpy(p_flow_tbl->CR_buf_121, p_flow_tbl_list->p_CR_buf_121, CR_SIZE_121);
        p_flow_tbl_list->p_CR_buf_121 = ptr2ptr(p_flow_tbl->CR_buf_121);
    }

    return rval;
}
static uint32 exe_update_flow_tbl_list(uint32 context_id, uint32 flow_idx, const amba_ik_cr_index_t *p_table_index, const amba_ik_cr_id_list_t *p_cr_upd_flag)
{
    uint32 rval = IK_OK;
    uintptr addr = 0;
    amba_ik_flow_tables_list_t *p_flow_tbl_list = NULL;
    amba_ik_flow_tables_t *p_flow_tbl = NULL;

    rval |= img_ctx_get_flow_tbl_list(context_id, flow_idx, &addr);
    (void)amba_ik_system_memcpy(&p_flow_tbl_list, &addr, sizeof(void *));

    if(p_cr_upd_flag->cawarp != 0U) {
        rval |= img_ctx_get_flow_tbl(context_id, p_table_index->cawarp, &addr);
        (void)amba_ik_system_memcpy(&p_flow_tbl, &addr, sizeof(void *));
        p_flow_tbl_list->p_ca_warp_hor_red = ptr2ptr(p_flow_tbl->ca_warp_hor_red);
        p_flow_tbl_list->p_ca_warp_hor_blue = ptr2ptr(p_flow_tbl->ca_warp_hor_blue);
        p_flow_tbl_list->p_ca_warp_ver_red = ptr2ptr(p_flow_tbl->ca_warp_ver_red);
        p_flow_tbl_list->p_ca_warp_ver_blue = ptr2ptr(p_flow_tbl->ca_warp_ver_blue);
    }
    if(p_cr_upd_flag->warp != 0U) {
        rval |= img_ctx_get_flow_tbl(context_id, p_table_index->warp, &addr);
        (void)amba_ik_system_memcpy(&p_flow_tbl, &addr, sizeof(void *));
        p_flow_tbl_list->p_warp_hor = ptr2ptr(p_flow_tbl->warp_hor);
        p_flow_tbl_list->p_warp_ver = ptr2ptr(p_flow_tbl->warp_ver);
    }
    if(p_cr_upd_flag->aaa_data != 0U) {
        rval |= img_ctx_get_flow_tbl(context_id, p_table_index->aaa_data, &addr);
        (void)amba_ik_system_memcpy(&p_flow_tbl, &addr, sizeof(void *));
        p_flow_tbl_list->p_aaa = ptr2ptr(p_flow_tbl->aaa);
    }
    if(p_cr_upd_flag->frame_info != 0U) {
        rval |= img_ctx_get_flow_tbl(context_id, p_table_index->frame_info, &addr);
        (void)amba_ik_system_memcpy(&p_flow_tbl, &addr, sizeof(void *));
        p_flow_tbl_list->p_frame_info = ptr2ptr(&p_flow_tbl->frame_info);
    }

    rval |= exe_update_flow_tbl_list_cr4_cr20(context_id, p_table_index, p_cr_upd_flag, p_flow_tbl_list);
    rval |= exe_update_flow_tbl_list_cr21_cr34(context_id, p_table_index, p_cr_upd_flag, p_flow_tbl_list);
    rval |= exe_update_flow_tbl_list_cr35_cr48(context_id, p_table_index, p_cr_upd_flag, p_flow_tbl_list);
    rval |= exe_update_flow_tbl_list_cr49_cr52(context_id, p_table_index, p_cr_upd_flag, p_flow_tbl_list);
    rval |= exe_update_flow_tbl_list_cr111_cr121(context_id, p_table_index, p_cr_upd_flag, p_flow_tbl_list);

    return rval;
}

static INLINE uint32 exe_compute_linear_cr(idsp_ik_flow_ctrl_t *p_flow, amba_ik_flow_tables_list_t *p_flow_tbl_list, amba_ik_filter_t *p_filters)
{
    uint32 rval = IK_OK;

    /******************** Calibration ********************/
    rval |= exe_process_calib_cr(p_flow, p_flow_tbl_list, p_filters);

    /******************** CFA ********************/
    rval |= exe_process_hdr_ce_cr(p_flow, p_flow_tbl_list, p_filters, 0);

    rval |= exe_process_cfa_cr(p_flow, p_flow_tbl_list, p_filters);

    /******************** RGB ********************/
    rval |= exe_process_rgb_cr(p_flow_tbl_list, p_filters);

    /******************** YUV ********************/
    rval |= exe_process_yuv_cr(p_flow, p_flow_tbl_list, p_filters);

    /******************** MD ********************/
    rval |= exe_process_mctf_mcts_cr(p_flow, p_flow_tbl_list, p_filters);

    return rval;
}

static INLINE uint32 exe_compute_hdr_1x_cr(idsp_ik_flow_ctrl_t *p_flow, amba_ik_flow_tables_list_t *p_flow_tbl_list, amba_ik_filter_t *p_filters)
{
    uint32 rval = IK_OK;

    rval |= exe_process_calib_cr(p_flow, p_flow_tbl_list, p_filters);

    /******************** CFA ********************/
    rval |= exe_process_hdr_ce_cr(p_flow, p_flow_tbl_list, p_filters, 1);

    rval |= exe_process_hdr_ce_sub_cr(p_flow_tbl_list, p_filters, 1);

    rval |= exe_process_cfa_cr(p_flow, p_flow_tbl_list, p_filters);

    /******************** RGB ********************/
    rval |= exe_process_rgb_cr(p_flow_tbl_list, p_filters);

    /******************** YUV ********************/
    rval |= exe_process_yuv_cr(p_flow, p_flow_tbl_list, p_filters);

    /******************** MD ********************/
    rval |= exe_process_mctf_mcts_cr(p_flow, p_flow_tbl_list, p_filters);

    return rval;
}

static INLINE uint32 exe_compute_hdr_2x_cr(idsp_ik_flow_ctrl_t *p_flow, amba_ik_flow_tables_list_t *p_flow_tbl_list, amba_ik_filter_t *p_filters)
{
    uint32 rval = IK_OK;

    rval |= exe_process_calib_cr(p_flow, p_flow_tbl_list, p_filters);

    /******************** CFA ********************/
    rval |= exe_process_hdr_ce_cr(p_flow, p_flow_tbl_list, p_filters, 2);

    rval |= exe_process_hdr_ce_sub_cr(p_flow_tbl_list, p_filters, 2);

    rval |= exe_process_cfa_cr(p_flow, p_flow_tbl_list, p_filters);

    /******************** RGB ********************/
    rval |= exe_process_rgb_cr(p_flow_tbl_list, p_filters);

    /******************** YUV ********************/
    rval |= exe_process_yuv_cr(p_flow, p_flow_tbl_list, p_filters);

    /******************** MD ********************/
    rval |= exe_process_mctf_mcts_cr(p_flow, p_flow_tbl_list, p_filters);

    return rval;
}

static INLINE uint32 exe_compute_hdr_3x_cr(idsp_ik_flow_ctrl_t *p_flow, amba_ik_flow_tables_list_t *p_flow_tbl_list, amba_ik_filter_t *p_filters)
{
    uint32 rval = IK_OK;

    rval |= exe_process_calib_cr(p_flow, p_flow_tbl_list, p_filters);

    /******************** CFA ********************/
    rval |= exe_process_hdr_ce_cr(p_flow, p_flow_tbl_list, p_filters, 3);

    rval |= exe_process_hdr_ce_sub_cr(p_flow_tbl_list, p_filters, 3);

    rval |= exe_process_cfa_cr(p_flow, p_flow_tbl_list, p_filters);

    /******************** RGB ********************/
    rval |= exe_process_rgb_cr(p_flow_tbl_list, p_filters);

    /******************** YUV ********************/
    rval |= exe_process_yuv_cr(p_flow, p_flow_tbl_list, p_filters);

    /******************** MD ********************/
    rval |= exe_process_mctf_mcts_cr(p_flow, p_flow_tbl_list, p_filters);

    return rval;
}

static INLINE uint32 exe_compute_y2y_cr(idsp_ik_flow_ctrl_t *p_flow, amba_ik_flow_tables_list_t *p_flow_tbl_list, amba_ik_filter_t *p_filters)
{
    uint32 rval = IK_OK;

    /******************** Calibration ********************/
    rval |= exe_process_calib_cr(p_flow, p_flow_tbl_list, p_filters);

    /******************** CFA ********************/
    rval |= exe_process_cfa_cr(p_flow, p_flow_tbl_list, p_filters);

    /******************** RGB ********************/
    rval |= exe_process_rgb_cr(p_flow_tbl_list, p_filters);

    /******************** YUV ********************/
    rval |= exe_process_yuv_cr(p_flow, p_flow_tbl_list, p_filters);

    /******************** MD ********************/
    rval |= exe_process_mctf_mcts_cr(p_flow, p_flow_tbl_list, p_filters);

    return rval;
}

static INLINE uint32 exe_cmpt_invalid_cr_utility(idsp_ik_flow_ctrl_t *p_flow, amba_ik_flow_tables_list_t *p_flow_tbl_list, amba_ik_filter_t *p_filters)
{
    uint32 rval = IK_ERR_0000;

    if (p_flow != NULL) {
        (void)amba_ik_system_memcpy(p_flow,p_flow,sizeof(uint8));//misra fix.
    }
    if (p_flow_tbl_list != NULL) {
        (void)amba_ik_system_memcpy(p_flow_tbl_list,p_flow_tbl_list,sizeof(uint8));//misra fix.
    }
    if (p_filters != NULL) {
        (void)amba_ik_system_memcpy(p_filters,p_filters,sizeof(uint8));//misra fix.
    }

    return rval;
}

static INLINE uint32 exe_get_cr_utilities(const ik_ability_t *p_ability, amba_ik_exe_cr_utilities_t *p_utilities)
{
    uint32 rval = IK_OK;
    if (p_ability->pipe == AMBA_IK_PIPE_VIDEO) {
        switch (p_ability->video_pipe) {
        case AMBA_IK_VIDEO_LINEAR:
            p_utilities->compute_cr = exe_compute_linear_cr;
            break;
        case AMBA_IK_VIDEO_LINEAR_CE:
            p_utilities->compute_cr = exe_compute_hdr_1x_cr;
            break;
        case AMBA_IK_VIDEO_HDR_EXPO_2:
            p_utilities->compute_cr = exe_compute_hdr_2x_cr;
            break;
        case AMBA_IK_VIDEO_HDR_EXPO_3:
            p_utilities->compute_cr = exe_compute_hdr_3x_cr;
            break;
        case AMBA_IK_VIDEO_Y2Y:
            p_utilities->compute_cr = exe_compute_y2y_cr;
            break;
        case AMBA_IK_VIDEO_MAX:
        default:
            p_utilities->compute_cr = exe_cmpt_invalid_cr_utility;
            amba_ik_system_print_uint32_5("[IK][ERROR] video did not support this video_pipe %d", p_ability->video_pipe, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0000;
            break;
        }
    } else {
        p_utilities->compute_cr = exe_cmpt_invalid_cr_utility;
        amba_ik_system_print_uint32_5("[IK] did not support this pipe %d", p_ability->pipe, DC_U, DC_U, DC_U, DC_U);
        rval = IK_ERR_0000;
    }

    return rval;
}

static uint32 exe_set_flow_header(uint32 context_id, idsp_ik_flow_ctrl_t *p_flow)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if (rval == IK_OK) {
        p_flow->header.ik_cfg_id.running_number = p_ctx->organization.active_cr_state.cr_running_number;
        (void)amba_ik_system_memcpy(&p_flow->header.idsp_flow_addr, &p_flow, sizeof(uint32));
    }

    return rval;
}

static inline uint32 exe_clean_flow_ctrl_cache_sec2(ik_config_region_t *p_flow_sec2_crs_array, uint8 update, const void *p_CR_buf, uint32 cr_id)
{
    uint32 rval = IK_OK;
    uint32 cr_size;
    uint32 cr_array_idx;
    static const uint32 sec2_cr_size[38] = {
        CR_SIZE_4,
        CR_SIZE_5,
        CR_SIZE_6,
        CR_SIZE_7,
        CR_SIZE_8,
        CR_SIZE_9,
        CR_SIZE_10,
        CR_SIZE_11,
        CR_SIZE_12,
        CR_SIZE_13,
        CR_SIZE_14,
        CR_SIZE_15,
        CR_SIZE_16,
        CR_SIZE_17,
        CR_SIZE_18,
        CR_SIZE_19,
        CR_SIZE_20,
        CR_SIZE_21,
        CR_SIZE_22,
        CR_SIZE_23,
        CR_SIZE_24,
        CR_SIZE_25,
        CR_SIZE_26,
        CR_SIZE_27,
        CR_SIZE_28,
        CR_SIZE_29,
        CR_SIZE_30,
        CR_SIZE_31,
        CR_SIZE_32,
        CR_SIZE_33,
        CR_SIZE_34,
        CR_SIZE_35,
        CR_SIZE_36,
        CR_SIZE_37,
        CR_SIZE_38,
        CR_SIZE_39,
        CR_SIZE_40,
        CR_SIZE_41,
    };

    if ((cr_id>=4UL) && (cr_id<=41UL)) {
        cr_size = sec2_cr_size[cr_id-4U];
    } else { //should never go here.
        cr_size = 0;
        rval = IK_ERR_0000;
    }

    if(update != 0u) {
        if ((cr_id >= 36U)&&(cr_id <= 39U)) { //CR40 insert to CR36 before. not successive here!
            cr_array_idx = cr_id-3U;
        } else if(cr_id == 40U) {
            cr_array_idx = 36U-4U;
        } else {
            cr_array_idx = cr_id-4U;
        }
        amba_ik_system_clean_cache(p_CR_buf, cr_size);
        p_flow_sec2_crs_array[cr_array_idx].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_CR_buf)) >> 7);
    }
    return rval;
}

static inline uint32 exe_clean_flow_ctrl_cache_sec3(ik_config_region_t *p_flow_sec3_crs_array, uint8 update, const void *p_CR_buf, uint32 cr_id)
{
    uint32 rval = IK_OK;
    uint32 cr_size;
    static const uint32 sec3_cr_size[3] = {
        CR_SIZE_42,
        CR_SIZE_43,
        CR_SIZE_44,
    };

    if ((cr_id>=42UL) && (cr_id<=44UL)) {
        cr_size = sec3_cr_size[cr_id-42U];
    } else { //should never go here.
        cr_size = 0;
        rval = IK_ERR_0000;
    }

    if(update != 0u) {
        amba_ik_system_clean_cache(p_CR_buf, cr_size);
        p_flow_sec3_crs_array[cr_id-42U].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_CR_buf)) >> 7);
    }
    return rval;
}

static inline uint32 exe_clean_flow_ctrl_cache_sec4(ik_config_region_t *p_flow_sec4_crs_array, uint8 update, const void *p_CR_buf, uint32 cr_id)
{
    uint32 rval = IK_OK;
    uint32 cr_size;
    static const uint32 sec4_cr_size[8] = {
        CR_SIZE_45,
        CR_SIZE_46,
        CR_SIZE_47,
        CR_SIZE_48,
        CR_SIZE_49,
        CR_SIZE_50,
        CR_SIZE_51,
        CR_SIZE_52,
    };

    if ((cr_id>=45UL) && (cr_id<=52UL)) {
        cr_size = sec4_cr_size[cr_id-45U];
    } else { //should never go here.
        cr_size = 0;
        rval = IK_ERR_0000;
    }

    if(update != 0u) {
        amba_ik_system_clean_cache(p_CR_buf, cr_size);
        p_flow_sec4_crs_array[cr_id-45U].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_CR_buf)) >> 7);
    }
    return rval;
}

static inline uint32 exe_clean_flow_ctrl_cache_sec18(ik_config_region_t *p_flow_sec18_crs_array, uint8 update, const void *p_CR_buf, uint32 cr_id)
{
    uint32 rval = IK_OK;
    uint32 cr_size;
    static const uint32 sec18_cr_size[11] = {
        CR_SIZE_111,
        CR_SIZE_112,
        CR_SIZE_113,
        CR_SIZE_114,
        CR_SIZE_115,
        CR_SIZE_116,
        CR_SIZE_117,
        CR_SIZE_118,
        CR_SIZE_119,
        CR_SIZE_120,
        CR_SIZE_121,
    };

    if ((cr_id>=111UL) && (cr_id<=121UL)) {
        cr_size = sec18_cr_size[cr_id-111U];
    } else { //should never go here.
        cr_size = 0;
        rval = IK_ERR_0000;
    }

    if(update != 0u) {
        amba_ik_system_clean_cache(p_CR_buf, cr_size);
        p_flow_sec18_crs_array[cr_id-111U].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_CR_buf)) >> 7);
    }
    return rval;
}

static uint32 exe_clean_flow_ctrl_cache(idsp_ik_flow_ctrl_t *p_flow, const amba_ik_flow_tables_list_t *p_flow_tbl_list, const amba_ik_cr_id_list_t *p_cr_upd_flag) // TODO, consider multiple steps...
{
    uint32 rval = IK_OK;
    uint32 size = 0u;

    if ((null_check_op(p_flow)+ null_check_op(p_flow_tbl_list)+ null_check_op(p_cr_upd_flag)) != 0U) {
        amba_ik_system_print_str_5("[IK][ERROR] img_cfg_clean_flow_ctrl_cache() detect NULL pointer", NULL, NULL, NULL, NULL, NULL);
        rval |= IK_ERR_0005;
    } else {
        if(p_cr_upd_flag->cawarp != 0u) {
            size = sizeof(int16) * (CAWARP_VIDEO_MAXHORGRID * CAWARP_VIDEO_MAXVERGRID);
            amba_ik_system_clean_cache(p_flow_tbl_list->p_ca_warp_hor_red, size);
            p_flow->calib.cawarp_horizontal_table_addr_red = physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_ca_warp_hor_red));

            amba_ik_system_clean_cache(p_flow_tbl_list->p_ca_warp_hor_blue, size);
            p_flow->calib.cawarp_horizontal_table_addr_blue = physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_ca_warp_hor_blue));

            amba_ik_system_clean_cache(p_flow_tbl_list->p_ca_warp_ver_red, size);
            p_flow->calib.cawarp_vertical_table_addr_red = physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_ca_warp_ver_red));

            amba_ik_system_clean_cache(p_flow_tbl_list->p_ca_warp_ver_blue, size);
            p_flow->calib.cawarp_vertical_table_addr_blue = physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_ca_warp_ver_blue));
        }

        if(p_cr_upd_flag->warp != 0u) {
            size = sizeof(int16) * (WARP_VIDEO_MAXHORGRID * WARP_VIDEO_MAXVERGRID * 2U);
            amba_ik_system_clean_cache(p_flow_tbl_list->p_warp_hor, size);
            p_flow->calib.warp_horizontal_table_address = physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_warp_hor));

            amba_ik_system_clean_cache(p_flow_tbl_list->p_warp_ver, size);
            p_flow->calib.warp_vertical_table_address = physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_warp_ver));
        }

        if(p_cr_upd_flag->aaa_data != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_aaa, sizeof(idsp_ik_aaa_data_t)*(uint32)IK_MAX_TILE_NUM_X*(uint32)IK_MAX_TILE_NUM_Y);
            p_flow->stitch.aaa_info_daddr = physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_aaa));
        }

        if(p_cr_upd_flag->frame_info != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_frame_info, sizeof(ik_query_frame_info_t));
        }

        rval |= exe_clean_flow_ctrl_cache_sec2(&p_flow->step_crs[0].sec2_crs[0], (uint8)p_cr_upd_flag->cr_4_update, p_flow_tbl_list->p_CR_buf_4, 4UL);
        rval |= exe_clean_flow_ctrl_cache_sec2(&p_flow->step_crs[0].sec2_crs[0], (uint8)p_cr_upd_flag->cr_5_update, p_flow_tbl_list->p_CR_buf_5, 5UL);
        rval |= exe_clean_flow_ctrl_cache_sec2(&p_flow->step_crs[0].sec2_crs[0], (uint8)p_cr_upd_flag->cr_6_update, p_flow_tbl_list->p_CR_buf_6, 6UL);
        rval |= exe_clean_flow_ctrl_cache_sec2(&p_flow->step_crs[0].sec2_crs[0], (uint8)p_cr_upd_flag->cr_7_update, p_flow_tbl_list->p_CR_buf_7, 7UL);
        rval |= exe_clean_flow_ctrl_cache_sec2(&p_flow->step_crs[0].sec2_crs[0], (uint8)p_cr_upd_flag->cr_8_update, p_flow_tbl_list->p_CR_buf_8, 8UL);
        rval |= exe_clean_flow_ctrl_cache_sec2(&p_flow->step_crs[0].sec2_crs[0], (uint8)p_cr_upd_flag->cr_9_update, p_flow_tbl_list->p_CR_buf_9, 9UL);
        rval |= exe_clean_flow_ctrl_cache_sec2(&p_flow->step_crs[0].sec2_crs[0], (uint8)p_cr_upd_flag->cr_10_update, p_flow_tbl_list->p_CR_buf_10, 10UL);
        rval |= exe_clean_flow_ctrl_cache_sec2(&p_flow->step_crs[0].sec2_crs[0], (uint8)p_cr_upd_flag->cr_11_update, p_flow_tbl_list->p_CR_buf_11, 11UL);
        rval |= exe_clean_flow_ctrl_cache_sec2(&p_flow->step_crs[0].sec2_crs[0], (uint8)p_cr_upd_flag->cr_12_update, p_flow_tbl_list->p_CR_buf_12, 12UL);
        rval |= exe_clean_flow_ctrl_cache_sec2(&p_flow->step_crs[0].sec2_crs[0], (uint8)p_cr_upd_flag->cr_13_update, p_flow_tbl_list->p_CR_buf_13, 13UL);
        rval |= exe_clean_flow_ctrl_cache_sec2(&p_flow->step_crs[0].sec2_crs[0], (uint8)p_cr_upd_flag->cr_14_update, p_flow_tbl_list->p_CR_buf_14, 14UL);
        rval |= exe_clean_flow_ctrl_cache_sec2(&p_flow->step_crs[0].sec2_crs[0], (uint8)p_cr_upd_flag->cr_15_update, p_flow_tbl_list->p_CR_buf_15, 15UL);
        rval |= exe_clean_flow_ctrl_cache_sec2(&p_flow->step_crs[0].sec2_crs[0], (uint8)p_cr_upd_flag->cr_16_update, p_flow_tbl_list->p_CR_buf_16, 16UL);

        if(p_cr_upd_flag->cr_17_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_17, CR_SIZE_17);
            p_flow->step_crs[0].sec2_crs[13].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_17)) >> 7);

            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_18, CR_SIZE_18);
            p_flow->step_crs[0].sec2_crs[14].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_18)) >> 7);

            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_19, CR_SIZE_19);
            p_flow->step_crs[0].sec2_crs[15].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_19)) >> 7);

            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_20, CR_SIZE_20);
            p_flow->step_crs[0].sec2_crs[16].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_20)) >> 7);
        }

        rval |= exe_clean_flow_ctrl_cache_sec2(&p_flow->step_crs[0].sec2_crs[0], (uint8)p_cr_upd_flag->cr_21_update, p_flow_tbl_list->p_CR_buf_21, 21UL);
        rval |= exe_clean_flow_ctrl_cache_sec2(&p_flow->step_crs[0].sec2_crs[0], (uint8)p_cr_upd_flag->cr_22_update, p_flow_tbl_list->p_CR_buf_22, 22UL);
        rval |= exe_clean_flow_ctrl_cache_sec2(&p_flow->step_crs[0].sec2_crs[0], (uint8)p_cr_upd_flag->cr_23_update, p_flow_tbl_list->p_CR_buf_23, 23UL);
        rval |= exe_clean_flow_ctrl_cache_sec2(&p_flow->step_crs[0].sec2_crs[0], (uint8)p_cr_upd_flag->cr_24_update, p_flow_tbl_list->p_CR_buf_24, 24UL);
        rval |= exe_clean_flow_ctrl_cache_sec2(&p_flow->step_crs[0].sec2_crs[0], (uint8)p_cr_upd_flag->cr_25_update, p_flow_tbl_list->p_CR_buf_25, 25UL);
        rval |= exe_clean_flow_ctrl_cache_sec2(&p_flow->step_crs[0].sec2_crs[0], (uint8)p_cr_upd_flag->cr_26_update, p_flow_tbl_list->p_CR_buf_26, 26UL);
        rval |= exe_clean_flow_ctrl_cache_sec2(&p_flow->step_crs[0].sec2_crs[0], (uint8)p_cr_upd_flag->cr_27_update, p_flow_tbl_list->p_CR_buf_27, 27UL);
        rval |= exe_clean_flow_ctrl_cache_sec2(&p_flow->step_crs[0].sec2_crs[0], (uint8)p_cr_upd_flag->cr_28_update, p_flow_tbl_list->p_CR_buf_28, 28UL);
        rval |= exe_clean_flow_ctrl_cache_sec2(&p_flow->step_crs[0].sec2_crs[0], (uint8)p_cr_upd_flag->cr_29_update, p_flow_tbl_list->p_CR_buf_29, 29UL);
        rval |= exe_clean_flow_ctrl_cache_sec2(&p_flow->step_crs[0].sec2_crs[0], (uint8)p_cr_upd_flag->cr_30_update, p_flow_tbl_list->p_CR_buf_30, 30UL);
        rval |= exe_clean_flow_ctrl_cache_sec2(&p_flow->step_crs[0].sec2_crs[0], (uint8)p_cr_upd_flag->cr_31_update, p_flow_tbl_list->p_CR_buf_31, 31UL);
        rval |= exe_clean_flow_ctrl_cache_sec2(&p_flow->step_crs[0].sec2_crs[0], (uint8)p_cr_upd_flag->cr_32_update, p_flow_tbl_list->p_CR_buf_32, 32UL);
        rval |= exe_clean_flow_ctrl_cache_sec2(&p_flow->step_crs[0].sec2_crs[0], (uint8)p_cr_upd_flag->cr_33_update, p_flow_tbl_list->p_CR_buf_33, 33UL);
        rval |= exe_clean_flow_ctrl_cache_sec2(&p_flow->step_crs[0].sec2_crs[0], (uint8)p_cr_upd_flag->cr_34_update, p_flow_tbl_list->p_CR_buf_34, 34UL);
        rval |= exe_clean_flow_ctrl_cache_sec2(&p_flow->step_crs[0].sec2_crs[0], (uint8)p_cr_upd_flag->cr_35_update, p_flow_tbl_list->p_CR_buf_35, 35UL);
        rval |= exe_clean_flow_ctrl_cache_sec2(&p_flow->step_crs[0].sec2_crs[0], (uint8)p_cr_upd_flag->cr_36_update, p_flow_tbl_list->p_CR_buf_36, 36UL);
        rval |= exe_clean_flow_ctrl_cache_sec2(&p_flow->step_crs[0].sec2_crs[0], (uint8)p_cr_upd_flag->cr_37_update, p_flow_tbl_list->p_CR_buf_37, 37UL);
        rval |= exe_clean_flow_ctrl_cache_sec2(&p_flow->step_crs[0].sec2_crs[0], (uint8)p_cr_upd_flag->cr_38_update, p_flow_tbl_list->p_CR_buf_38, 38UL);
        rval |= exe_clean_flow_ctrl_cache_sec2(&p_flow->step_crs[0].sec2_crs[0], (uint8)p_cr_upd_flag->cr_39_update, p_flow_tbl_list->p_CR_buf_39, 39UL);
        rval |= exe_clean_flow_ctrl_cache_sec2(&p_flow->step_crs[0].sec2_crs[0], (uint8)p_cr_upd_flag->cr_40_update, p_flow_tbl_list->p_CR_buf_40, 40UL);
        rval |= exe_clean_flow_ctrl_cache_sec2(&p_flow->step_crs[0].sec2_crs[0], (uint8)p_cr_upd_flag->cr_41_update, p_flow_tbl_list->p_CR_buf_41, 41UL);

        rval |= exe_clean_flow_ctrl_cache_sec3(&p_flow->step_crs[0].sec3_crs[0], (uint8)p_cr_upd_flag->cr_42_update, p_flow_tbl_list->p_CR_buf_42, 42UL);
        rval |= exe_clean_flow_ctrl_cache_sec3(&p_flow->step_crs[0].sec3_crs[0], (uint8)p_cr_upd_flag->cr_43_update, p_flow_tbl_list->p_CR_buf_43, 43UL);
        rval |= exe_clean_flow_ctrl_cache_sec3(&p_flow->step_crs[0].sec3_crs[0], (uint8)p_cr_upd_flag->cr_44_update, p_flow_tbl_list->p_CR_buf_44, 44UL);

        rval |= exe_clean_flow_ctrl_cache_sec4(&p_flow->step_crs[0].sec4_crs[0], (uint8)p_cr_upd_flag->cr_45_update, p_flow_tbl_list->p_CR_buf_45, 45UL);
        rval |= exe_clean_flow_ctrl_cache_sec4(&p_flow->step_crs[0].sec4_crs[0], (uint8)p_cr_upd_flag->cr_46_update, p_flow_tbl_list->p_CR_buf_46, 46UL);
        rval |= exe_clean_flow_ctrl_cache_sec4(&p_flow->step_crs[0].sec4_crs[0], (uint8)p_cr_upd_flag->cr_47_update, p_flow_tbl_list->p_CR_buf_47, 47UL);
        rval |= exe_clean_flow_ctrl_cache_sec4(&p_flow->step_crs[0].sec4_crs[0], (uint8)p_cr_upd_flag->cr_48_update, p_flow_tbl_list->p_CR_buf_48, 48UL);
        rval |= exe_clean_flow_ctrl_cache_sec4(&p_flow->step_crs[0].sec4_crs[0], (uint8)p_cr_upd_flag->cr_49_update, p_flow_tbl_list->p_CR_buf_49, 49UL);
        rval |= exe_clean_flow_ctrl_cache_sec4(&p_flow->step_crs[0].sec4_crs[0], (uint8)p_cr_upd_flag->cr_50_update, p_flow_tbl_list->p_CR_buf_50, 50UL);
        rval |= exe_clean_flow_ctrl_cache_sec4(&p_flow->step_crs[0].sec4_crs[0], (uint8)p_cr_upd_flag->cr_51_update, p_flow_tbl_list->p_CR_buf_51, 51UL);
        rval |= exe_clean_flow_ctrl_cache_sec4(&p_flow->step_crs[0].sec4_crs[0], (uint8)p_cr_upd_flag->cr_52_update, p_flow_tbl_list->p_CR_buf_52, 52UL);

        rval |= exe_clean_flow_ctrl_cache_sec18(&p_flow->step_crs[0].sec18_crs[0], (uint8)p_cr_upd_flag->cr_111_update, p_flow_tbl_list->p_CR_buf_111, 111UL);
        rval |= exe_clean_flow_ctrl_cache_sec18(&p_flow->step_crs[0].sec18_crs[0], (uint8)p_cr_upd_flag->cr_112_update, p_flow_tbl_list->p_CR_buf_112, 112UL);
        rval |= exe_clean_flow_ctrl_cache_sec18(&p_flow->step_crs[0].sec18_crs[0], (uint8)p_cr_upd_flag->cr_113_update, p_flow_tbl_list->p_CR_buf_113, 113UL);
        rval |= exe_clean_flow_ctrl_cache_sec18(&p_flow->step_crs[0].sec18_crs[0], (uint8)p_cr_upd_flag->cr_114_update, p_flow_tbl_list->p_CR_buf_114, 114UL);
        rval |= exe_clean_flow_ctrl_cache_sec18(&p_flow->step_crs[0].sec18_crs[0], (uint8)p_cr_upd_flag->cr_115_update, p_flow_tbl_list->p_CR_buf_115, 115UL);
        rval |= exe_clean_flow_ctrl_cache_sec18(&p_flow->step_crs[0].sec18_crs[0], (uint8)p_cr_upd_flag->cr_116_update, p_flow_tbl_list->p_CR_buf_116, 116UL);
        rval |= exe_clean_flow_ctrl_cache_sec18(&p_flow->step_crs[0].sec18_crs[0], (uint8)p_cr_upd_flag->cr_117_update, p_flow_tbl_list->p_CR_buf_117, 117UL);
        rval |= exe_clean_flow_ctrl_cache_sec18(&p_flow->step_crs[0].sec18_crs[0], (uint8)p_cr_upd_flag->cr_118_update, p_flow_tbl_list->p_CR_buf_118, 118UL);
        rval |= exe_clean_flow_ctrl_cache_sec18(&p_flow->step_crs[0].sec18_crs[0], (uint8)p_cr_upd_flag->cr_119_update, p_flow_tbl_list->p_CR_buf_119, 119UL);
        rval |= exe_clean_flow_ctrl_cache_sec18(&p_flow->step_crs[0].sec18_crs[0], (uint8)p_cr_upd_flag->cr_120_update, p_flow_tbl_list->p_CR_buf_120, 120UL);
        rval |= exe_clean_flow_ctrl_cache_sec18(&p_flow->step_crs[0].sec18_crs[0], (uint8)p_cr_upd_flag->cr_121_update, p_flow_tbl_list->p_CR_buf_121, 121UL);

        p_flow->header.idsp_flow_addr = physical2u32(amba_ik_system_virt_to_phys(p_flow));
        amba_ik_system_clean_cache(p_flow, sizeof(idsp_ik_flow_ctrl_t) + (64u - (sizeof(idsp_ik_flow_ctrl_t) & 0x3FU)));
    }

    return rval;
}

static inline uint32 exe_flow_ctrl_ring_check_sec2(uint32 ring_depth, uint8 update, const ik_config_region_t *p_flow_sec2_crs_array, const ik_config_region_t *p_flow_sec2_crs_array_previous, uint32 cr_id)
{
    uint32 rval = IK_OK;
    uint32 cr_array_idx;
    uintptr ring_offset = sizeof(amba_ik_flow_tables_t)>>7;//ring buffer offset should always = sizeof(amba_ik_flow_tables_t).

    if ((cr_id >= 36U)&&(cr_id <= 39U)) { //CR40 insert to CR36 before. not successive here!
        cr_array_idx = cr_id-3U;
    } else if(cr_id == 40U) {
        cr_array_idx = 36U-4U;
    } else {
        cr_array_idx = cr_id-4U;
    }

    if ((uint32)p_flow_sec2_crs_array[cr_array_idx].config_region_addr < (uint32)p_flow_sec2_crs_array_previous[cr_array_idx].config_region_addr) {
        //negative offset, due to ring buffer wrap around.
        ring_offset -= ring_offset*ring_depth;
    }

    if (update == 0u) {
        if ((uint32)p_flow_sec2_crs_array[cr_array_idx].config_region_addr != (uint32)p_flow_sec2_crs_array_previous[cr_array_idx].config_region_addr) {
            amba_ik_system_print_uint32_5("[IK] detect CR ring buffer error at CR_%d, current(%p), previous(%p) expected_ring_offset(0x%x)",
                                          cr_id, (uint32)p_flow_sec2_crs_array[cr_array_idx].config_region_addr<<7, (uint32)p_flow_sec2_crs_array_previous[cr_array_idx].config_region_addr<<7, 0, DC_U);
            rval = IK_ERR_0205;//Ring buffer Eorror
        }
    } else {
        if ((uint32)p_flow_sec2_crs_array[cr_array_idx].config_region_addr != ((uint32)p_flow_sec2_crs_array_previous[cr_array_idx].config_region_addr + ring_offset)) {
            amba_ik_system_print_uint32_5("[IK] detect CR ring buffer error at CR_%d, current(%p), previous(%p) expected_ring_offset(0x%x)",
                                          cr_id, (uint32)p_flow_sec2_crs_array[cr_array_idx].config_region_addr<<7, (uint32)p_flow_sec2_crs_array_previous[cr_array_idx].config_region_addr<<7, ring_offset<<7, DC_U);
            rval = IK_ERR_0205;//Ring buffer Eorror
        }
    }
    return rval;
}

static inline uint32 exe_flow_ctrl_ring_check_sec3(uint32 ring_depth, uint8 update, const ik_config_region_t *p_flow_sec3_crs_array, const ik_config_region_t *p_flow_sec3_crs_array_previous, uint32 cr_id)
{
    uint32 rval = IK_OK;
    uint32 cr_array_idx = cr_id-42U;
    uintptr ring_offset = sizeof(amba_ik_flow_tables_t)>>7;//ring buffer offset should always = sizeof(amba_ik_flow_tables_t).

    if ((uint32)p_flow_sec3_crs_array[cr_array_idx].config_region_addr < (uint32)p_flow_sec3_crs_array_previous[cr_array_idx].config_region_addr) {
        //negative offset, due to ring buffer wrap around.
        ring_offset -= ring_offset*ring_depth;
    }

    if (update == 0u) {
        if ((uint32)p_flow_sec3_crs_array[cr_array_idx].config_region_addr != (uint32)p_flow_sec3_crs_array_previous[cr_array_idx].config_region_addr) {
            amba_ik_system_print_uint32_5("[IK] detect CR ring buffer error at CR_%d, current(%p), previous(%p) expected_ring_offset(0x%x)",
                                          cr_id, (uint32)p_flow_sec3_crs_array[cr_array_idx].config_region_addr<<7, (uint32)p_flow_sec3_crs_array_previous[cr_array_idx].config_region_addr<<7, 0, DC_U);
            rval = IK_ERR_0205;//Ring buffer Eorror
        }
    } else {
        if ((uint32)p_flow_sec3_crs_array[cr_array_idx].config_region_addr != ((uint32)p_flow_sec3_crs_array_previous[cr_array_idx].config_region_addr + ring_offset)) {
            amba_ik_system_print_uint32_5("[IK] detect CR ring buffer error at CR_%d, current(%p), previous(%p) expected_ring_offset(0x%x)",
                                          cr_id, (uint32)p_flow_sec3_crs_array[cr_array_idx].config_region_addr<<7, (uint32)p_flow_sec3_crs_array_previous[cr_array_idx].config_region_addr<<7, ring_offset<<7, DC_U);
            rval = IK_ERR_0205;//Ring buffer Eorror
        }
    }
    return rval;
}

static inline uint32 exe_flow_ctrl_ring_check_sec4(uint32 ring_depth, uint8 update, const ik_config_region_t *p_flow_sec4_crs_array, const ik_config_region_t *p_flow_sec4_crs_array_previous, uint32 cr_id)
{
    uint32 rval = IK_OK;
    uint32 cr_array_idx = cr_id-45U;
    uintptr ring_offset = sizeof(amba_ik_flow_tables_t)>>7;//ring buffer offset should always = sizeof(amba_ik_flow_tables_t).

    if ((uint32)p_flow_sec4_crs_array[cr_array_idx].config_region_addr < (uint32)p_flow_sec4_crs_array_previous[cr_array_idx].config_region_addr) {
        //negative offset, due to ring buffer wrap around.
        ring_offset -= ring_offset*ring_depth;
    }

    if (update == 0u) {
        if ((uint32)p_flow_sec4_crs_array[cr_array_idx].config_region_addr != (uint32)p_flow_sec4_crs_array_previous[cr_array_idx].config_region_addr) {
            amba_ik_system_print_uint32_5("[IK] detect CR ring buffer error at CR_%d, current_addr(%p), previous_addr(%p), expected_ring_offset(0x%x)",
                                          cr_id, (uint32)p_flow_sec4_crs_array[cr_array_idx].config_region_addr<<7, (uint32)p_flow_sec4_crs_array_previous[cr_array_idx].config_region_addr<<7, 0, DC_U);
            rval = IK_ERR_0205;//Ring buffer Eorror
        }
    } else {
        if ((uint32)p_flow_sec4_crs_array[cr_array_idx].config_region_addr != ((uint32)p_flow_sec4_crs_array_previous[cr_array_idx].config_region_addr + ring_offset)) {
            amba_ik_system_print_uint32_5("[IK] detect CR ring buffer error at CR_%d, current_addr(%p), previous_addr(%p), expected_ring_offset(0x%x)",
                                          cr_id, (uint32)p_flow_sec4_crs_array[cr_array_idx].config_region_addr<<7, (uint32)p_flow_sec4_crs_array_previous[cr_array_idx].config_region_addr<<7, ring_offset<<7, DC_U);
            rval = IK_ERR_0205;//Ring buffer Eorror
        }
    }
    return rval;
}

static inline uint32 exe_flow_ctrl_ring_check_sec18(uint32 ring_depth, uint8 update, const ik_config_region_t *p_flow_sec18_crs_array, const ik_config_region_t *p_flow_sec18_crs_array_previous, uint32 cr_id)
{
    uint32 rval = IK_OK;
    uint32 cr_array_idx = cr_id-111U;
    uintptr ring_offset = sizeof(amba_ik_flow_tables_t)>>7;//ring buffer offset should always = sizeof(amba_ik_flow_tables_t).

    if ((uint32)p_flow_sec18_crs_array[cr_array_idx].config_region_addr < (uint32)p_flow_sec18_crs_array_previous[cr_array_idx].config_region_addr) {
        //negative offset, due to ring buffer wrap around.
        ring_offset -= ring_offset*ring_depth;
    }

    if (update == 0u) {
        if ((uint32)p_flow_sec18_crs_array[cr_array_idx].config_region_addr != (uint32)p_flow_sec18_crs_array_previous[cr_array_idx].config_region_addr) {
            amba_ik_system_print_uint32_5("[IK] detect CR ring buffer error at CR_%d, current_addr(%p), previous_addr(%p), expected_ring_offset(0x%x)",
                                          cr_id, (uint32)p_flow_sec18_crs_array[cr_array_idx].config_region_addr<<7, (uint32)p_flow_sec18_crs_array_previous[cr_array_idx].config_region_addr<<7, 0, DC_U);
            rval = IK_ERR_0205;//Ring buffer Eorror
        }
    } else {
        if ((uint32)p_flow_sec18_crs_array[cr_array_idx].config_region_addr != ((uint32)p_flow_sec18_crs_array_previous[cr_array_idx].config_region_addr + ring_offset)) {
            amba_ik_system_print_uint32_5("[IK] detect CR ring buffer error at CR_%d, current_addr(%p), previous_addr(%p), expected_ring_offset(0x%x)",
                                          cr_id, (uint32)p_flow_sec18_crs_array[cr_array_idx].config_region_addr<<7, (uint32)p_flow_sec18_crs_array_previous[cr_array_idx].config_region_addr<<7, ring_offset<<7, DC_U);
            rval = IK_ERR_0205;//Ring buffer Eorror
        }
    }
    return rval;
}

static uint32 exe_flow_ctrl_ring_check_ca(uint32 ring_depth, const idsp_ik_flow_ctrl_t *p_flow, const idsp_ik_flow_ctrl_t *p_flow_previous, const amba_ik_cr_id_list_t *p_cr_upd_flag)
{
    uint32 rval = IK_OK;
    uintptr ring_offset;

    if(p_cr_upd_flag->cawarp != 0u) {
        ring_offset = sizeof(amba_ik_flow_tables_t);//ring buffer offset should always = sizeof(amba_ik_flow_tables_t).
        if (p_flow->calib.cawarp_horizontal_table_addr_red < p_flow_previous->calib.cawarp_horizontal_table_addr_red) {
            //negative offset, due to ring buffer wrap around.
            ring_offset -= ring_offset*ring_depth;
        }

        if (p_flow->calib.cawarp_horizontal_table_addr_red != (p_flow_previous->calib.cawarp_horizontal_table_addr_red + ring_offset)) {
            amba_ik_system_print_uint32_5("[IK] detect CR ring buffer error at cawarp_horizontal_table_addr_red, current_addr(%p), previous_addr(%p), expected_ring_offset(0x%x)"
                                          , p_flow->calib.cawarp_horizontal_table_addr_red, p_flow_previous->calib.cawarp_horizontal_table_addr_red, ring_offset, DC_U, DC_U);
            rval = IK_ERR_0205;//Ring buffer Eorror
        }
        if (p_flow->calib.cawarp_horizontal_table_addr_blue != (p_flow_previous->calib.cawarp_horizontal_table_addr_blue + ring_offset)) {
            amba_ik_system_print_uint32_5("[IK] detect CR ring buffer error at cawarp_horizontal_table_addr_blue, current_addr(%p), previous_addr(%p), expected_ring_offset(0x%x)"
                                          , p_flow->calib.cawarp_horizontal_table_addr_blue, p_flow_previous->calib.cawarp_horizontal_table_addr_blue, ring_offset, DC_U, DC_U);
            rval = IK_ERR_0205;//Ring buffer Eorror
        }
        if (p_flow->calib.cawarp_vertical_table_addr_red != (p_flow_previous->calib.cawarp_vertical_table_addr_red + ring_offset)) {
            amba_ik_system_print_uint32_5("[IK] detect CR ring buffer error at cawarp_vertical_table_addr_red, current_addr(%p), previous_addr(%p), expected_ring_offset(0x%x)"
                                          , p_flow->calib.cawarp_vertical_table_addr_red, p_flow_previous->calib.cawarp_vertical_table_addr_red, ring_offset, DC_U, DC_U);
            rval = IK_ERR_0205;//Ring buffer Eorror
        }
        if (p_flow->calib.cawarp_vertical_table_addr_blue != (p_flow_previous->calib.cawarp_vertical_table_addr_blue + ring_offset)) {
            amba_ik_system_print_uint32_5("[IK] detect CR ring buffer error at cawarp_vertical_table_addr_blue, current_addr(%p), previous_addr(%p), expected_ring_offset(0x%x)"
                                          , p_flow->calib.cawarp_vertical_table_addr_blue, p_flow_previous->calib.cawarp_vertical_table_addr_blue, ring_offset, DC_U, DC_U);
            rval = IK_ERR_0205;//Ring buffer Eorror
        }
    } else {
        if (p_flow->calib.cawarp_horizontal_table_addr_red != p_flow_previous->calib.cawarp_horizontal_table_addr_red) {
            amba_ik_system_print_uint32_5("[IK] detect CR ring buffer error at cawarp_horizontal_table_addr_red, current_addr(%p), previous_addr(%p), expected_ring_offset(0x%x)"
                                          , p_flow->calib.cawarp_horizontal_table_addr_red, p_flow_previous->calib.cawarp_horizontal_table_addr_red, 0U, DC_U, DC_U);
            rval = IK_ERR_0205;//Ring buffer Eorror
        }
        if (p_flow->calib.cawarp_horizontal_table_addr_blue != p_flow_previous->calib.cawarp_horizontal_table_addr_blue) {
            amba_ik_system_print_uint32_5("[IK] detect CR ring buffer error at cawarp_horizontal_table_addr_blue, current_addr(%p), previous_addr(%p), expected_ring_offset(0x%x)"
                                          , p_flow->calib.cawarp_horizontal_table_addr_blue, p_flow_previous->calib.cawarp_horizontal_table_addr_blue, 0U, DC_U, DC_U);
            rval = IK_ERR_0205;//Ring buffer Eorror
        }
        if (p_flow->calib.cawarp_vertical_table_addr_red != p_flow_previous->calib.cawarp_vertical_table_addr_red) {
            amba_ik_system_print_uint32_5("[IK] detect CR ring buffer error at cawarp_vertical_table_addr_red, current_addr(%p), previous_addr(%p), expected_ring_offset(0x%x)"
                                          , p_flow->calib.cawarp_vertical_table_addr_red, p_flow_previous->calib.cawarp_vertical_table_addr_red, 0U, DC_U, DC_U);
            rval = IK_ERR_0205;//Ring buffer Eorror
        }
        if (p_flow->calib.cawarp_vertical_table_addr_blue != p_flow_previous->calib.cawarp_vertical_table_addr_blue) {
            amba_ik_system_print_uint32_5("[IK] detect CR ring buffer error at cawarp_vertical_table_addr_blue, current_addr(%p), previous_addr(%p), expected_ring_offset(0x%x)"
                                          , p_flow->calib.cawarp_vertical_table_addr_blue, p_flow_previous->calib.cawarp_vertical_table_addr_blue, 0U, DC_U, DC_U);
            rval = IK_ERR_0205;//Ring buffer Eorror
        }
    }

    return rval;
}

static uint32 exe_flow_ctrl_ring_check(uint32 ring_depth, const idsp_ik_flow_ctrl_t *p_flow, const idsp_ik_flow_ctrl_t *p_flow_previous, const amba_ik_cr_id_list_t *p_cr_upd_flag)
{
    uint32 rval = IK_OK;
    uintptr ring_offset;

    if ((null_check_op(p_flow)+ null_check_op(p_flow_previous)+ null_check_op(p_cr_upd_flag)) != 0U) {
        amba_ik_system_print_str_5("[IK][ERROR] exe_flow_ctrl_ring_check() detect NULL pointer", NULL, NULL, NULL, NULL, NULL);
        rval |= IK_ERR_0005;
    } else {

        rval |= exe_flow_ctrl_ring_check_ca(ring_depth, p_flow, p_flow_previous, p_cr_upd_flag);//complexity refine.

        if(p_cr_upd_flag->warp != 0u) {
            ring_offset = sizeof(amba_ik_flow_tables_t);//ring buffer offset should always = sizeof(amba_ik_flow_tables_t).
            if (p_flow->calib.warp_horizontal_table_address < p_flow_previous->calib.warp_horizontal_table_address) {
                //negative offset, due to ring buffer wrap around.
                ring_offset -= ring_offset*ring_depth;
            }

            if (p_flow->calib.warp_horizontal_table_address != (p_flow_previous->calib.warp_horizontal_table_address + ring_offset)) {
                amba_ik_system_print_uint32_5("[IK] detect CR ring buffer error at warp_horizontal_table_address, current_addr(%p), previous_addr(%p), expected_ring_offset(0x%x)"
                                              , p_flow->calib.warp_horizontal_table_address, p_flow_previous->calib.warp_horizontal_table_address, ring_offset, DC_U, DC_U);
                rval = IK_ERR_0205;//Ring buffer Eorror
            }
            if (p_flow->calib.warp_vertical_table_address != (p_flow_previous->calib.warp_vertical_table_address + ring_offset)) {
                amba_ik_system_print_uint32_5("[IK] detect CR ring buffer error at warp_vertical_table_address, current_addr(%p), previous_addr(%p), expected_ring_offset(0x%x)"
                                              , p_flow->calib.warp_vertical_table_address, p_flow_previous->calib.warp_vertical_table_address, ring_offset, DC_U, DC_U);
                rval = IK_ERR_0205;//Ring buffer Eorror
            }
        } else {
            if (p_flow->calib.warp_horizontal_table_address != p_flow_previous->calib.warp_horizontal_table_address) {
                amba_ik_system_print_uint32_5("[IK] detect CR ring buffer error at warp_horizontal_table_address, current_addr(%p), previous_addr(%p), expected_ring_offset(0x%x)"
                                              , p_flow->calib.warp_horizontal_table_address, p_flow_previous->calib.warp_horizontal_table_address, 0U, DC_U, DC_U);
                rval = IK_ERR_0205;//Ring buffer Eorror
            }
            if (p_flow->calib.warp_vertical_table_address != p_flow_previous->calib.warp_vertical_table_address) {
                amba_ik_system_print_uint32_5("[IK] detect CR ring buffer error at warp_vertical_table_address, current_addr(%p), previous_addr(%p), expected_ring_offset(0x%x)"
                                              , p_flow->calib.warp_vertical_table_address, p_flow_previous->calib.warp_vertical_table_address, 0U, DC_U, DC_U);
                rval = IK_ERR_0205;//Ring buffer Eorror
            }
        }

        if(p_cr_upd_flag->aaa_data != 0u) {
            ring_offset = sizeof(amba_ik_flow_tables_t);//ring buffer offset should always = sizeof(amba_ik_flow_tables_t).
            if (p_flow->stitch.aaa_info_daddr < p_flow_previous->stitch.aaa_info_daddr) {
                //negative offset, due to ring buffer wrap around.
                ring_offset -= ring_offset*ring_depth;
            }

            if (p_flow->stitch.aaa_info_daddr != (p_flow_previous->stitch.aaa_info_daddr + ring_offset)) {
                amba_ik_system_print_uint32_5("[IK] detect CR ring buffer error at aaa_data, current_addr(%p), previous_addr(%p), expected_ring_offset(0x%x)"
                                              , p_flow->stitch.aaa_info_daddr, p_flow_previous->stitch.aaa_info_daddr, ring_offset, DC_U, DC_U);
                rval = IK_ERR_0205;//Ring buffer Eorror
            }
        } else {
            if (p_flow->stitch.aaa_info_daddr != p_flow_previous->stitch.aaa_info_daddr) {
                amba_ik_system_print_uint32_5("[IK] detect CR ring buffer error at aaa_data, current_addr(%p), previous_addr(%p), expected_ring_offset(0x%x)"
                                              , p_flow->stitch.aaa_info_daddr, p_flow_previous->stitch.aaa_info_daddr, 0, DC_U, DC_U);
                rval = IK_ERR_0205;//Ring buffer Eorror
            }
        }

        rval |= exe_flow_ctrl_ring_check_sec2(ring_depth, (uint8)p_cr_upd_flag->cr_4_update, &p_flow->step_crs[0].sec2_crs[0], &p_flow_previous->step_crs[0].sec2_crs[0], 4UL);
        rval |= exe_flow_ctrl_ring_check_sec2(ring_depth, (uint8)p_cr_upd_flag->cr_5_update, &p_flow->step_crs[0].sec2_crs[0], &p_flow_previous->step_crs[0].sec2_crs[0], 5UL);
        rval |= exe_flow_ctrl_ring_check_sec2(ring_depth, (uint8)p_cr_upd_flag->cr_6_update, &p_flow->step_crs[0].sec2_crs[0], &p_flow_previous->step_crs[0].sec2_crs[0], 6UL);
        rval |= exe_flow_ctrl_ring_check_sec2(ring_depth, (uint8)p_cr_upd_flag->cr_7_update, &p_flow->step_crs[0].sec2_crs[0], &p_flow_previous->step_crs[0].sec2_crs[0], 7UL);
        rval |= exe_flow_ctrl_ring_check_sec2(ring_depth, (uint8)p_cr_upd_flag->cr_8_update, &p_flow->step_crs[0].sec2_crs[0], &p_flow_previous->step_crs[0].sec2_crs[0], 8UL);
        rval |= exe_flow_ctrl_ring_check_sec2(ring_depth, (uint8)p_cr_upd_flag->cr_9_update, &p_flow->step_crs[0].sec2_crs[0], &p_flow_previous->step_crs[0].sec2_crs[0], 9UL);
        rval |= exe_flow_ctrl_ring_check_sec2(ring_depth, (uint8)p_cr_upd_flag->cr_10_update, &p_flow->step_crs[0].sec2_crs[0], &p_flow_previous->step_crs[0].sec2_crs[0], 10UL);
        rval |= exe_flow_ctrl_ring_check_sec2(ring_depth, (uint8)p_cr_upd_flag->cr_11_update, &p_flow->step_crs[0].sec2_crs[0], &p_flow_previous->step_crs[0].sec2_crs[0], 11UL);
        rval |= exe_flow_ctrl_ring_check_sec2(ring_depth, (uint8)p_cr_upd_flag->cr_12_update, &p_flow->step_crs[0].sec2_crs[0], &p_flow_previous->step_crs[0].sec2_crs[0], 12UL);
        rval |= exe_flow_ctrl_ring_check_sec2(ring_depth, (uint8)p_cr_upd_flag->cr_13_update, &p_flow->step_crs[0].sec2_crs[0], &p_flow_previous->step_crs[0].sec2_crs[0], 13UL);
        rval |= exe_flow_ctrl_ring_check_sec2(ring_depth, (uint8)p_cr_upd_flag->cr_14_update, &p_flow->step_crs[0].sec2_crs[0], &p_flow_previous->step_crs[0].sec2_crs[0], 14UL);
        rval |= exe_flow_ctrl_ring_check_sec2(ring_depth, (uint8)p_cr_upd_flag->cr_15_update, &p_flow->step_crs[0].sec2_crs[0], &p_flow_previous->step_crs[0].sec2_crs[0], 15UL);
        rval |= exe_flow_ctrl_ring_check_sec2(ring_depth, (uint8)p_cr_upd_flag->cr_16_update, &p_flow->step_crs[0].sec2_crs[0], &p_flow_previous->step_crs[0].sec2_crs[0], 16UL);

        rval |= exe_flow_ctrl_ring_check_sec2(ring_depth, (uint8)p_cr_upd_flag->cr_17_update, &p_flow->step_crs[0].sec2_crs[0], &p_flow_previous->step_crs[0].sec2_crs[0], 17UL);
        rval |= exe_flow_ctrl_ring_check_sec2(ring_depth, (uint8)p_cr_upd_flag->cr_17_update, &p_flow->step_crs[0].sec2_crs[0], &p_flow_previous->step_crs[0].sec2_crs[0], 18UL);
        rval |= exe_flow_ctrl_ring_check_sec2(ring_depth, (uint8)p_cr_upd_flag->cr_17_update, &p_flow->step_crs[0].sec2_crs[0], &p_flow_previous->step_crs[0].sec2_crs[0], 19UL);
        rval |= exe_flow_ctrl_ring_check_sec2(ring_depth, (uint8)p_cr_upd_flag->cr_17_update, &p_flow->step_crs[0].sec2_crs[0], &p_flow_previous->step_crs[0].sec2_crs[0], 20UL);

        rval |= exe_flow_ctrl_ring_check_sec2(ring_depth, (uint8)p_cr_upd_flag->cr_21_update, &p_flow->step_crs[0].sec2_crs[0], &p_flow_previous->step_crs[0].sec2_crs[0], 21UL);
        rval |= exe_flow_ctrl_ring_check_sec2(ring_depth, (uint8)p_cr_upd_flag->cr_22_update, &p_flow->step_crs[0].sec2_crs[0], &p_flow_previous->step_crs[0].sec2_crs[0], 22UL);
        rval |= exe_flow_ctrl_ring_check_sec2(ring_depth, (uint8)p_cr_upd_flag->cr_23_update, &p_flow->step_crs[0].sec2_crs[0], &p_flow_previous->step_crs[0].sec2_crs[0], 23UL);
        rval |= exe_flow_ctrl_ring_check_sec2(ring_depth, (uint8)p_cr_upd_flag->cr_24_update, &p_flow->step_crs[0].sec2_crs[0], &p_flow_previous->step_crs[0].sec2_crs[0], 24UL);
        rval |= exe_flow_ctrl_ring_check_sec2(ring_depth, (uint8)p_cr_upd_flag->cr_25_update, &p_flow->step_crs[0].sec2_crs[0], &p_flow_previous->step_crs[0].sec2_crs[0], 25UL);
        rval |= exe_flow_ctrl_ring_check_sec2(ring_depth, (uint8)p_cr_upd_flag->cr_26_update, &p_flow->step_crs[0].sec2_crs[0], &p_flow_previous->step_crs[0].sec2_crs[0], 26UL);
        rval |= exe_flow_ctrl_ring_check_sec2(ring_depth, (uint8)p_cr_upd_flag->cr_27_update, &p_flow->step_crs[0].sec2_crs[0], &p_flow_previous->step_crs[0].sec2_crs[0], 27UL);
        rval |= exe_flow_ctrl_ring_check_sec2(ring_depth, (uint8)p_cr_upd_flag->cr_28_update, &p_flow->step_crs[0].sec2_crs[0], &p_flow_previous->step_crs[0].sec2_crs[0], 28UL);
        rval |= exe_flow_ctrl_ring_check_sec2(ring_depth, (uint8)p_cr_upd_flag->cr_29_update, &p_flow->step_crs[0].sec2_crs[0], &p_flow_previous->step_crs[0].sec2_crs[0], 29UL);
        rval |= exe_flow_ctrl_ring_check_sec2(ring_depth, (uint8)p_cr_upd_flag->cr_30_update, &p_flow->step_crs[0].sec2_crs[0], &p_flow_previous->step_crs[0].sec2_crs[0], 30UL);
        rval |= exe_flow_ctrl_ring_check_sec2(ring_depth, (uint8)p_cr_upd_flag->cr_31_update, &p_flow->step_crs[0].sec2_crs[0], &p_flow_previous->step_crs[0].sec2_crs[0], 31UL);
        rval |= exe_flow_ctrl_ring_check_sec2(ring_depth, (uint8)p_cr_upd_flag->cr_32_update, &p_flow->step_crs[0].sec2_crs[0], &p_flow_previous->step_crs[0].sec2_crs[0], 32UL);
        rval |= exe_flow_ctrl_ring_check_sec2(ring_depth, (uint8)p_cr_upd_flag->cr_33_update, &p_flow->step_crs[0].sec2_crs[0], &p_flow_previous->step_crs[0].sec2_crs[0], 33UL);
        rval |= exe_flow_ctrl_ring_check_sec2(ring_depth, (uint8)p_cr_upd_flag->cr_34_update, &p_flow->step_crs[0].sec2_crs[0], &p_flow_previous->step_crs[0].sec2_crs[0], 34UL);
        rval |= exe_flow_ctrl_ring_check_sec2(ring_depth, (uint8)p_cr_upd_flag->cr_35_update, &p_flow->step_crs[0].sec2_crs[0], &p_flow_previous->step_crs[0].sec2_crs[0], 35UL);
        rval |= exe_flow_ctrl_ring_check_sec2(ring_depth, (uint8)p_cr_upd_flag->cr_36_update, &p_flow->step_crs[0].sec2_crs[0], &p_flow_previous->step_crs[0].sec2_crs[0], 36UL);
        rval |= exe_flow_ctrl_ring_check_sec2(ring_depth, (uint8)p_cr_upd_flag->cr_37_update, &p_flow->step_crs[0].sec2_crs[0], &p_flow_previous->step_crs[0].sec2_crs[0], 37UL);
        rval |= exe_flow_ctrl_ring_check_sec2(ring_depth, (uint8)p_cr_upd_flag->cr_38_update, &p_flow->step_crs[0].sec2_crs[0], &p_flow_previous->step_crs[0].sec2_crs[0], 38UL);
        rval |= exe_flow_ctrl_ring_check_sec2(ring_depth, (uint8)p_cr_upd_flag->cr_39_update, &p_flow->step_crs[0].sec2_crs[0], &p_flow_previous->step_crs[0].sec2_crs[0], 39UL);
        rval |= exe_flow_ctrl_ring_check_sec2(ring_depth, (uint8)p_cr_upd_flag->cr_40_update, &p_flow->step_crs[0].sec2_crs[0], &p_flow_previous->step_crs[0].sec2_crs[0], 40UL);
        rval |= exe_flow_ctrl_ring_check_sec2(ring_depth, (uint8)p_cr_upd_flag->cr_41_update, &p_flow->step_crs[0].sec2_crs[0], &p_flow_previous->step_crs[0].sec2_crs[0], 41UL);

        rval |= exe_flow_ctrl_ring_check_sec3(ring_depth, (uint8)p_cr_upd_flag->cr_42_update, &p_flow->step_crs[0].sec3_crs[0], &p_flow_previous->step_crs[0].sec3_crs[0], 42UL);
        rval |= exe_flow_ctrl_ring_check_sec3(ring_depth, (uint8)p_cr_upd_flag->cr_43_update, &p_flow->step_crs[0].sec3_crs[0], &p_flow_previous->step_crs[0].sec3_crs[0], 43UL);
        rval |= exe_flow_ctrl_ring_check_sec3(ring_depth, (uint8)p_cr_upd_flag->cr_44_update, &p_flow->step_crs[0].sec3_crs[0], &p_flow_previous->step_crs[0].sec3_crs[0], 44UL);

        rval |= exe_flow_ctrl_ring_check_sec4(ring_depth, (uint8)p_cr_upd_flag->cr_45_update, &p_flow->step_crs[0].sec4_crs[0], &p_flow_previous->step_crs[0].sec4_crs[0], 45UL);
        rval |= exe_flow_ctrl_ring_check_sec4(ring_depth, (uint8)p_cr_upd_flag->cr_46_update, &p_flow->step_crs[0].sec4_crs[0], &p_flow_previous->step_crs[0].sec4_crs[0], 46UL);
        rval |= exe_flow_ctrl_ring_check_sec4(ring_depth, (uint8)p_cr_upd_flag->cr_47_update, &p_flow->step_crs[0].sec4_crs[0], &p_flow_previous->step_crs[0].sec4_crs[0], 47UL);
        rval |= exe_flow_ctrl_ring_check_sec4(ring_depth, (uint8)p_cr_upd_flag->cr_48_update, &p_flow->step_crs[0].sec4_crs[0], &p_flow_previous->step_crs[0].sec4_crs[0], 48UL);
        rval |= exe_flow_ctrl_ring_check_sec4(ring_depth, (uint8)p_cr_upd_flag->cr_49_update, &p_flow->step_crs[0].sec4_crs[0], &p_flow_previous->step_crs[0].sec4_crs[0], 49UL);
        rval |= exe_flow_ctrl_ring_check_sec4(ring_depth, (uint8)p_cr_upd_flag->cr_50_update, &p_flow->step_crs[0].sec4_crs[0], &p_flow_previous->step_crs[0].sec4_crs[0], 50UL);
        rval |= exe_flow_ctrl_ring_check_sec4(ring_depth, (uint8)p_cr_upd_flag->cr_51_update, &p_flow->step_crs[0].sec4_crs[0], &p_flow_previous->step_crs[0].sec4_crs[0], 51UL);
        rval |= exe_flow_ctrl_ring_check_sec4(ring_depth, (uint8)p_cr_upd_flag->cr_52_update, &p_flow->step_crs[0].sec4_crs[0], &p_flow_previous->step_crs[0].sec4_crs[0], 52UL);

        rval |= exe_flow_ctrl_ring_check_sec18(ring_depth, (uint8)p_cr_upd_flag->cr_111_update, &p_flow->step_crs[0].sec18_crs[0], &p_flow_previous->step_crs[0].sec18_crs[0], 111UL);
        rval |= exe_flow_ctrl_ring_check_sec18(ring_depth, (uint8)p_cr_upd_flag->cr_112_update, &p_flow->step_crs[0].sec18_crs[0], &p_flow_previous->step_crs[0].sec18_crs[0], 112UL);
        rval |= exe_flow_ctrl_ring_check_sec18(ring_depth, (uint8)p_cr_upd_flag->cr_113_update, &p_flow->step_crs[0].sec18_crs[0], &p_flow_previous->step_crs[0].sec18_crs[0], 113UL);
        rval |= exe_flow_ctrl_ring_check_sec18(ring_depth, (uint8)p_cr_upd_flag->cr_114_update, &p_flow->step_crs[0].sec18_crs[0], &p_flow_previous->step_crs[0].sec18_crs[0], 114UL);
        rval |= exe_flow_ctrl_ring_check_sec18(ring_depth, (uint8)p_cr_upd_flag->cr_115_update, &p_flow->step_crs[0].sec18_crs[0], &p_flow_previous->step_crs[0].sec18_crs[0], 115UL);
        rval |= exe_flow_ctrl_ring_check_sec18(ring_depth, (uint8)p_cr_upd_flag->cr_116_update, &p_flow->step_crs[0].sec18_crs[0], &p_flow_previous->step_crs[0].sec18_crs[0], 116UL);
        rval |= exe_flow_ctrl_ring_check_sec18(ring_depth, (uint8)p_cr_upd_flag->cr_117_update, &p_flow->step_crs[0].sec18_crs[0], &p_flow_previous->step_crs[0].sec18_crs[0], 117UL);
        rval |= exe_flow_ctrl_ring_check_sec18(ring_depth, (uint8)p_cr_upd_flag->cr_118_update, &p_flow->step_crs[0].sec18_crs[0], &p_flow_previous->step_crs[0].sec18_crs[0], 118UL);
        rval |= exe_flow_ctrl_ring_check_sec18(ring_depth, (uint8)p_cr_upd_flag->cr_119_update, &p_flow->step_crs[0].sec18_crs[0], &p_flow_previous->step_crs[0].sec18_crs[0], 119UL);
        rval |= exe_flow_ctrl_ring_check_sec18(ring_depth, (uint8)p_cr_upd_flag->cr_120_update, &p_flow->step_crs[0].sec18_crs[0], &p_flow_previous->step_crs[0].sec18_crs[0], 120UL);
        rval |= exe_flow_ctrl_ring_check_sec18(ring_depth, (uint8)p_cr_upd_flag->cr_121_update, &p_flow->step_crs[0].sec18_crs[0], &p_flow_previous->step_crs[0].sec18_crs[0], 121UL);
    }

    return rval;
}

static void exe_inject_safety_error_ring_buffer(idsp_ik_flow_ctrl_t *p_flow)
{
    // corrupt flow_t CR addr, to trigger ring buffer addr checker.
    p_flow->step_crs[0].sec2_crs[0].config_region_addr = 0xDEAD0000U>>7;//failure addr.
    p_flow->step_crs[0].sec3_crs[0].config_region_addr = 0xDEAD0000U>>7;//failure addr.
    p_flow->step_crs[0].sec4_crs[0].config_region_addr = 0xDEAD0000U>>7;//failure addr.
    p_flow->step_crs[0].sec18_crs[0].config_region_addr = 0xDEAD0000U>>7;//failure addr.
    p_flow->calib.cawarp_horizontal_table_addr_red = 0xDEAD0000U;//failure addr.
    p_flow->calib.cawarp_vertical_table_addr_red = 0xDEAD0000U;//failure addr.
    p_flow->calib.cawarp_horizontal_table_addr_blue = 0xDEAD0000U;//failure addr.
    p_flow->calib.cawarp_vertical_table_addr_blue = 0xDEAD0000U;//failure addr.
    p_flow->calib.warp_horizontal_table_address = 0xDEAD0000U;//failure addr.
    p_flow->calib.warp_vertical_table_address = 0xDEAD0000U;//failure addr.
    p_flow->stitch.aaa_info_daddr = 0xDEAD0000U;//failure addr.

}

#if SUPPORT_CR_MEMORY_FENCE
static inline uint32 ik_cr_memory_fence_check(uint32 cr_id, uint32 length, const uint8 *ptr)
{
    uint32 rval = IK_OK;
    uint32 golden = 0x58585858UL;
    uint8 golden_8 = 0x58u;
    uint32 size = length;
    uint32 size_remain = length & 0x3u;
    const uint32 *ptr32 = NULL; // please do not change this to uint64 *, due to we compare 4 bytes at one time.
    const uint8 *ptr8 = NULL; // please do not change this to uint64 *, due to we compare 1 bytes at one time.

    (void)amba_ik_system_memcpy(&ptr32, &ptr, sizeof(void *));

    while(size >= 4u) {
        if(golden != *ptr32) {
            amba_ik_system_print_uint32_5("[IK] detect CR buffer corruption at CR_%d", cr_id, DC_U, DC_U, DC_U, DC_U);
            rval |= IK_ERR_0204;
        }
        ptr32++;
        size -= 4u;
    }

    (void)amba_ik_system_memcpy(&ptr8, &ptr32, sizeof(void *));

    while(size_remain > 0u) {
        if(golden_8 != *ptr8) {
            amba_ik_system_print_uint32_5("[IK] detect CR buffer corruption at CR_%d", cr_id, DC_U, DC_U, DC_U, DC_U);
            rval |= IK_ERR_0204;
        }
        ptr8++;
        size_remain--;
    }
    return rval;
}

static INLINE uint32 exe_get_previous_flow_control_index(const amba_ik_context_organization_t *p_ctx_org, const uint32 idx, uint32 previous_count, uint32 *p_flow_idx)
{
    uint32 rval = IK_OK;
    int32 flow_id;

    if (p_ctx_org == NULL) {
        rval = IK_ERR_0005;
    } else {
        if(previous_count > (p_ctx_org->attribute.cr_ring_number - 1u)) {
            amba_ik_system_print_uint32_5("[IK][ERROR] previous_count > (cr_ring_number-1) = %d", (p_ctx_org->attribute.cr_ring_number - 1u), DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0000; // TBD
        }
        if (rval == IK_OK) {
            flow_id = (int32)idx - (int32)previous_count;
            if (flow_id < 0) {
                flow_id = flow_id + (int32)p_ctx_org->attribute.cr_ring_number;
            }
            (void)amba_ik_system_memcpy(p_flow_idx, &flow_id, sizeof(int32));
        }
    }
    return rval;
}

static uint32 exe_ik_check_cr_mem_fence_4(uint32 context_id, uint32 flow_idx)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx;
    uintptr addr = 0;
    const amba_ik_flow_tables_list_t *p_flow_tbl_list = NULL;
    uint8 *p_tmp_u8 = NULL;
    const amba_ik_cr_fence_info_t *p_fence = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);

    if(rval == IK_OK) {
        p_fence = p_ctx->fence;
        rval |= img_ctx_get_flow_tbl_list(context_id, flow_idx, &addr);
        (void)amba_ik_system_memcpy(&p_flow_tbl_list, &addr, sizeof(void *));

        if (rval == IK_OK) {
            if (p_flow_tbl_list->p_CR_buf_50 != NULL) {
                (void)amba_ik_system_memcpy(&p_tmp_u8, &p_flow_tbl_list->p_CR_buf_50, sizeof(void *));
                p_tmp_u8 = &p_tmp_u8[p_fence[46].offset];
                rval |= ik_cr_memory_fence_check(50U, p_fence[46].length, p_tmp_u8);
            }
            if (p_flow_tbl_list->p_CR_buf_51 != NULL) {
                (void)amba_ik_system_memcpy(&p_tmp_u8, &p_flow_tbl_list->p_CR_buf_51, sizeof(void *));
                p_tmp_u8 = &p_tmp_u8[p_fence[47].offset];
                rval |= ik_cr_memory_fence_check(51U, p_fence[47].length, p_tmp_u8);
            }
            if (p_flow_tbl_list->p_CR_buf_52 != NULL) {
                (void)amba_ik_system_memcpy(&p_tmp_u8, &p_flow_tbl_list->p_CR_buf_52, sizeof(void *));
                p_tmp_u8 = &p_tmp_u8[p_fence[48].offset];
                rval |= ik_cr_memory_fence_check(52U, p_fence[48].length, p_tmp_u8);
            }
            if(p_flow_tbl_list->p_CR_buf_111 != NULL) {
                (void)amba_ik_system_memcpy(&p_tmp_u8, &p_flow_tbl_list->p_CR_buf_111, sizeof(void *));
                p_tmp_u8 = &p_tmp_u8[p_fence[49].offset];
                rval |= ik_cr_memory_fence_check(111U, p_fence[49].length, p_tmp_u8);
            }
            if(p_flow_tbl_list->p_CR_buf_114 != NULL) {
                (void)amba_ik_system_memcpy(&p_tmp_u8, &p_flow_tbl_list->p_CR_buf_114, sizeof(void *));
                p_tmp_u8 = &p_tmp_u8[p_fence[52].offset];
                rval |= ik_cr_memory_fence_check(114U, p_fence[52].length, p_tmp_u8);
            }
            if(p_flow_tbl_list->p_CR_buf_116 != NULL) {
                (void)amba_ik_system_memcpy(&p_tmp_u8, &p_flow_tbl_list->p_CR_buf_116, sizeof(void *));
                p_tmp_u8 = &p_tmp_u8[p_fence[54].offset];
                rval |= ik_cr_memory_fence_check(116U, p_fence[54].length, p_tmp_u8);
            }
            if(p_flow_tbl_list->p_CR_buf_117 != NULL) {
                (void)amba_ik_system_memcpy(&p_tmp_u8, &p_flow_tbl_list->p_CR_buf_117, sizeof(void *));
                p_tmp_u8 = &p_tmp_u8[p_fence[55].offset];
                rval |= ik_cr_memory_fence_check(117U, p_fence[55].length, p_tmp_u8);
            }
            if(p_flow_tbl_list->p_CR_buf_118 != NULL) {
                (void)amba_ik_system_memcpy(&p_tmp_u8, &p_flow_tbl_list->p_CR_buf_118, sizeof(void *));
                p_tmp_u8 = &p_tmp_u8[p_fence[56].offset];
                rval |= ik_cr_memory_fence_check(118U, p_fence[56].length, p_tmp_u8);
            }
            if(p_flow_tbl_list->p_CR_buf_120 != NULL) {
                (void)amba_ik_system_memcpy(&p_tmp_u8, &p_flow_tbl_list->p_CR_buf_120, sizeof(void *));
                p_tmp_u8 = &p_tmp_u8[p_fence[58].offset];
                rval |= ik_cr_memory_fence_check(120U, p_fence[58].length, p_tmp_u8);
            }
            if(p_flow_tbl_list->p_CR_buf_121 != NULL) {
                (void)amba_ik_system_memcpy(&p_tmp_u8, &p_flow_tbl_list->p_CR_buf_121, sizeof(void *));
                p_tmp_u8 = &p_tmp_u8[p_fence[59].offset];
                rval |= ik_cr_memory_fence_check(121U, p_fence[59].length, p_tmp_u8);
            }
        } else {
            amba_ik_system_print_uint32_5("[IK] call img_ctx_get_flow_tbl_list(%d) fail", context_id, DC_U, DC_U, DC_U, DC_U);
        }
    } else {
        amba_ik_system_print_str_5("[IK] can't get context", NULL, NULL, NULL, NULL, NULL);
    }

    return rval;
}

static uint32 exe_ik_check_cr_mem_fence_3(uint32 context_id, uint32 flow_idx)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx;
    uintptr addr = 0;
    const amba_ik_flow_tables_list_t *p_flow_tbl_list = NULL;
    uint8 *p_tmp_u8 = NULL;
    const amba_ik_cr_fence_info_t *p_fence = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);

    if(rval == IK_OK) {
        p_fence = p_ctx->fence;
        rval |= img_ctx_get_flow_tbl_list(context_id, flow_idx, &addr);
        (void)amba_ik_system_memcpy(&p_flow_tbl_list, &addr, sizeof(void *));

        if (rval == IK_OK) {
            if(p_flow_tbl_list->p_CR_buf_36 != NULL) {
                (void)amba_ik_system_memcpy(&p_tmp_u8, &p_flow_tbl_list->p_CR_buf_36, sizeof(void *));
                p_tmp_u8 = &p_tmp_u8[p_fence[33].offset];
                rval |= ik_cr_memory_fence_check(36U, p_fence[33].length, p_tmp_u8);
            }
            if(p_flow_tbl_list->p_CR_buf_37 != NULL) {
                (void)amba_ik_system_memcpy(&p_tmp_u8, &p_flow_tbl_list->p_CR_buf_37, sizeof(void *));
                p_tmp_u8 = &p_tmp_u8[p_fence[34].offset];
                rval |= ik_cr_memory_fence_check(37U, p_fence[34].length, p_tmp_u8);
            }
            if(p_flow_tbl_list->p_CR_buf_38 != NULL) {
                (void)amba_ik_system_memcpy(&p_tmp_u8, &p_flow_tbl_list->p_CR_buf_38, sizeof(void *));
                p_tmp_u8 = &p_tmp_u8[p_fence[35].offset];
                rval |= ik_cr_memory_fence_check(38U, p_fence[35].length, p_tmp_u8);
            }
            if(p_flow_tbl_list->p_CR_buf_39 != NULL) {
                (void)amba_ik_system_memcpy(&p_tmp_u8, &p_flow_tbl_list->p_CR_buf_39, sizeof(void *));
                p_tmp_u8 = &p_tmp_u8[p_fence[36].offset];
                rval |= ik_cr_memory_fence_check(39U, p_fence[36].length, p_tmp_u8);
            }
            if(p_flow_tbl_list->p_CR_buf_40 != NULL) {
                (void)amba_ik_system_memcpy(&p_tmp_u8, &p_flow_tbl_list->p_CR_buf_40, sizeof(void *));
                p_tmp_u8 = &p_tmp_u8[p_fence[32].offset];
                rval |= ik_cr_memory_fence_check(40U, p_fence[32].length, p_tmp_u8);
            }
            if(p_flow_tbl_list->p_CR_buf_41 != NULL) {
                (void)amba_ik_system_memcpy(&p_tmp_u8, &p_flow_tbl_list->p_CR_buf_41, sizeof(void *));
                p_tmp_u8 = &p_tmp_u8[p_fence[37].offset];
                rval |= ik_cr_memory_fence_check(41U, p_fence[37].length, p_tmp_u8);
            }
            if(p_flow_tbl_list->p_CR_buf_44 != NULL) {
                (void)amba_ik_system_memcpy(&p_tmp_u8, &p_flow_tbl_list->p_CR_buf_44, sizeof(void *));
                p_tmp_u8 = &p_tmp_u8[p_fence[40].offset];
                rval |= ik_cr_memory_fence_check(44U, p_fence[40].length, p_tmp_u8);
            }
            if (p_flow_tbl_list->p_CR_buf_45 != NULL) {
                (void)amba_ik_system_memcpy(&p_tmp_u8, &p_flow_tbl_list->p_CR_buf_45, sizeof(void *));
                p_tmp_u8 = &p_tmp_u8[p_fence[41].offset];
                rval |= ik_cr_memory_fence_check(45U, p_fence[41].length, p_tmp_u8);
            }
            if (p_flow_tbl_list->p_CR_buf_46 != NULL) {
                (void)amba_ik_system_memcpy(&p_tmp_u8, &p_flow_tbl_list->p_CR_buf_46, sizeof(void *));
                p_tmp_u8 = &p_tmp_u8[p_fence[42].offset];
                rval |= ik_cr_memory_fence_check(46U, p_fence[42].length, p_tmp_u8);
            }
            if (p_flow_tbl_list->p_CR_buf_47 != NULL) {
                (void)amba_ik_system_memcpy(&p_tmp_u8, &p_flow_tbl_list->p_CR_buf_47, sizeof(void *));
                p_tmp_u8 = &p_tmp_u8[p_fence[43].offset];
                rval |= ik_cr_memory_fence_check(47U, p_fence[43].length, p_tmp_u8);
            }
        } else {
            amba_ik_system_print_uint32_5("[IK] call img_ctx_get_flow_tbl_list(%d) fail", context_id, DC_U, DC_U, DC_U, DC_U);
        }
    } else {
        amba_ik_system_print_str_5("[IK] can't get context", NULL, NULL, NULL, NULL, NULL);
    }

    return rval;
}

static uint32 exe_ik_check_cr_mem_fence_2(uint32 context_id, uint32 flow_idx)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx;
    uintptr addr = 0;
    const amba_ik_flow_tables_list_t *p_flow_tbl_list = NULL;
    uint8 *p_tmp_u8 = NULL;
    const amba_ik_cr_fence_info_t *p_fence = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);

    if(rval == IK_OK) {
        p_fence = p_ctx->fence;
        rval |= img_ctx_get_flow_tbl_list(context_id, flow_idx, &addr);
        (void)amba_ik_system_memcpy(&p_flow_tbl_list, &addr, sizeof(void *));

        if (rval == IK_OK) {
            if(p_flow_tbl_list->p_CR_buf_16 != NULL) {
                (void)amba_ik_system_memcpy(&p_tmp_u8, &p_flow_tbl_list->p_CR_buf_16, sizeof(void *));
                p_tmp_u8 = &p_tmp_u8[p_fence[12].offset];
                rval |= ik_cr_memory_fence_check(16U, p_fence[12].length, p_tmp_u8);
            }
            if(p_flow_tbl_list->p_CR_buf_22 != NULL) {
                (void)amba_ik_system_memcpy(&p_tmp_u8, &p_flow_tbl_list->p_CR_buf_22, sizeof(void *));
                p_tmp_u8 = &p_tmp_u8[p_fence[18].offset];
                rval |= ik_cr_memory_fence_check(22U, p_fence[18].length, p_tmp_u8);
            }
            if(p_flow_tbl_list->p_CR_buf_23 != NULL) {
                (void)amba_ik_system_memcpy(&p_tmp_u8, &p_flow_tbl_list->p_CR_buf_23, sizeof(void *));
                p_tmp_u8 = &p_tmp_u8[p_fence[19].offset];
                rval |= ik_cr_memory_fence_check(23U, p_fence[19].length, p_tmp_u8);
            }
            if(p_flow_tbl_list->p_CR_buf_24 != NULL) {
                (void)amba_ik_system_memcpy(&p_tmp_u8, &p_flow_tbl_list->p_CR_buf_24, sizeof(void *));
                p_tmp_u8 = &p_tmp_u8[p_fence[20].offset];
                rval |= ik_cr_memory_fence_check(24U, p_fence[20].length, p_tmp_u8);
            }
            if(p_flow_tbl_list->p_CR_buf_25 != NULL) {
                (void)amba_ik_system_memcpy(&p_tmp_u8, &p_flow_tbl_list->p_CR_buf_25, sizeof(void *));
                p_tmp_u8 = &p_tmp_u8[p_fence[21].offset];
                rval |= ik_cr_memory_fence_check(25U, p_fence[21].length, p_tmp_u8);
            }
            if(p_flow_tbl_list->p_CR_buf_26 != NULL) {
                (void)amba_ik_system_memcpy(&p_tmp_u8, &p_flow_tbl_list->p_CR_buf_26, sizeof(void *));
                p_tmp_u8 = &p_tmp_u8[p_fence[22].offset];
                rval |= ik_cr_memory_fence_check(26U, p_fence[22].length, p_tmp_u8);
            }
            if(p_flow_tbl_list->p_CR_buf_28 != NULL) {
                (void)amba_ik_system_memcpy(&p_tmp_u8, &p_flow_tbl_list->p_CR_buf_28, sizeof(void *));
                p_tmp_u8 = &p_tmp_u8[p_fence[24].offset];
                rval |= ik_cr_memory_fence_check(28U, p_fence[24].length, p_tmp_u8);
            }
            if(p_flow_tbl_list->p_CR_buf_29 != NULL) {
                (void)amba_ik_system_memcpy(&p_tmp_u8, &p_flow_tbl_list->p_CR_buf_29, sizeof(void *));
                p_tmp_u8 = &p_tmp_u8[p_fence[25].offset];
                rval |= ik_cr_memory_fence_check(29U, p_fence[25].length, p_tmp_u8);
            }
            if(p_flow_tbl_list->p_CR_buf_31 != NULL) {
                (void)amba_ik_system_memcpy(&p_tmp_u8, &p_flow_tbl_list->p_CR_buf_31, sizeof(void *));
                p_tmp_u8 = &p_tmp_u8[p_fence[27].offset];
                rval |= ik_cr_memory_fence_check(31U, p_fence[27].length, p_tmp_u8);
            }
            if(p_flow_tbl_list->p_CR_buf_32 != NULL) {
                (void)amba_ik_system_memcpy(&p_tmp_u8, &p_flow_tbl_list->p_CR_buf_32, sizeof(void *));
                p_tmp_u8 = &p_tmp_u8[p_fence[28].offset];
                rval |= ik_cr_memory_fence_check(32U, p_fence[28].length, p_tmp_u8);
            }
        } else {
            amba_ik_system_print_uint32_5("[IK] call img_ctx_get_flow_tbl_list(%d) fail", context_id, DC_U, DC_U, DC_U, DC_U);
        }
    } else {
        amba_ik_system_print_str_5("[IK] can't get context", NULL, NULL, NULL, NULL, NULL);
    }

    return rval;
}

static uint32 exe_ik_check_cr_mem_fence_1(uint32 context_id, uint32 flow_idx)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx;
    uintptr addr = 0;
    const amba_ik_flow_tables_list_t *p_flow_tbl_list = NULL;
    uint8 *p_tmp_u8 = NULL;
    const amba_ik_cr_fence_info_t *p_fence = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);

    if(rval == IK_OK) {
        p_fence = p_ctx->fence;
        rval |= img_ctx_get_flow_tbl_list(context_id, flow_idx, &addr);
        (void)amba_ik_system_memcpy(&p_flow_tbl_list, &addr, sizeof(void *));

        if (rval == IK_OK) {
            if(p_flow_tbl_list->p_CR_buf_4 != NULL) {
                (void)amba_ik_system_memcpy(&p_tmp_u8, &p_flow_tbl_list->p_CR_buf_4, sizeof(void *));
                p_tmp_u8 = &p_tmp_u8[p_fence[0].offset];
                rval |= ik_cr_memory_fence_check(4U, p_fence[0].length, p_tmp_u8);
            }
            if(p_flow_tbl_list->p_CR_buf_5 != NULL) {
                (void)amba_ik_system_memcpy(&p_tmp_u8, &p_flow_tbl_list->p_CR_buf_5, sizeof(void *));
                p_tmp_u8 = &p_tmp_u8[p_fence[1].offset];
                rval |= ik_cr_memory_fence_check(5U, p_fence[1].length, p_tmp_u8);
            }
            if(p_flow_tbl_list->p_CR_buf_6 != NULL) {
                (void)amba_ik_system_memcpy(&p_tmp_u8, &p_flow_tbl_list->p_CR_buf_6, sizeof(void *));
                p_tmp_u8 = &p_tmp_u8[p_fence[2].offset];
                rval |= ik_cr_memory_fence_check(6U, p_fence[2].length, p_tmp_u8);
            }
            if(p_flow_tbl_list->p_CR_buf_7 != NULL) {
                (void)amba_ik_system_memcpy(&p_tmp_u8, &p_flow_tbl_list->p_CR_buf_7, sizeof(void *));
                p_tmp_u8 = &p_tmp_u8[p_fence[3].offset];
                rval |= ik_cr_memory_fence_check(7U, p_fence[3].length, p_tmp_u8);
            }
            if(p_flow_tbl_list->p_CR_buf_8 != NULL) {
                (void)amba_ik_system_memcpy(&p_tmp_u8, &p_flow_tbl_list->p_CR_buf_8, sizeof(void *));
                p_tmp_u8 = &p_tmp_u8[p_fence[4].offset];
                rval |= ik_cr_memory_fence_check(8U, p_fence[4].length, p_tmp_u8);
            }
            if(p_flow_tbl_list->p_CR_buf_9 != NULL) {
                (void)amba_ik_system_memcpy(&p_tmp_u8, &p_flow_tbl_list->p_CR_buf_9, sizeof(void *));
                p_tmp_u8 = &p_tmp_u8[p_fence[5].offset];
                rval |= ik_cr_memory_fence_check(9U, p_fence[5].length, p_tmp_u8);
            }
            if(p_flow_tbl_list->p_CR_buf_12 != NULL) {
                (void)amba_ik_system_memcpy(&p_tmp_u8, &p_flow_tbl_list->p_CR_buf_12, sizeof(void *));
                p_tmp_u8 = &p_tmp_u8[p_fence[8].offset];
                rval |= ik_cr_memory_fence_check(12U, p_fence[8].length, p_tmp_u8);
            }
            if(p_flow_tbl_list->p_CR_buf_13 != NULL) {
                (void)amba_ik_system_memcpy(&p_tmp_u8, &p_flow_tbl_list->p_CR_buf_13, sizeof(void *));
                p_tmp_u8 = &p_tmp_u8[p_fence[9].offset];
                rval |= ik_cr_memory_fence_check(13U, p_fence[9].length, p_tmp_u8);
            }
            if(p_flow_tbl_list->p_CR_buf_14 != NULL) {
                (void)amba_ik_system_memcpy(&p_tmp_u8, &p_flow_tbl_list->p_CR_buf_14, sizeof(void *));
                p_tmp_u8 = &p_tmp_u8[p_fence[10].offset];
                rval |= ik_cr_memory_fence_check(14U, p_fence[10].length, p_tmp_u8);
            }
            if(p_flow_tbl_list->p_CR_buf_15 != NULL) {
                (void)amba_ik_system_memcpy(&p_tmp_u8, &p_flow_tbl_list->p_CR_buf_15, sizeof(void *));
                p_tmp_u8 = &p_tmp_u8[p_fence[11].offset];
                rval |= ik_cr_memory_fence_check(15U, p_fence[11].length, p_tmp_u8);
            }
        } else {
            amba_ik_system_print_uint32_5("[IK] call img_ctx_get_flow_tbl_list(%d) fail", context_id, DC_U, DC_U, DC_U, DC_U);
        }
    } else {
        amba_ik_system_print_str_5("[IK] can't get context", NULL, NULL, NULL, NULL, NULL);
    }

    return rval;
}

static uint32 exe_ik_check_cr_mem_fence(uint32 context_id, uint32 flow_idx)
{
    uint32 rval = IK_OK;
    rval |= exe_ik_check_cr_mem_fence_1(context_id, flow_idx);
    rval |= exe_ik_check_cr_mem_fence_2(context_id, flow_idx);
    rval |= exe_ik_check_cr_mem_fence_3(context_id, flow_idx);
    rval |= exe_ik_check_cr_mem_fence_4(context_id, flow_idx);
    return rval;
}

static uint32 exe_ik_check_flow_tbl_memory_fence(uint32 context_id)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx;
    uintptr addr = 0;
    const amba_ik_flow_tables_t *p_flow_tbl = NULL;
    uint8 *p_tmp_u8 = NULL;
    const uint32 *tmp_p_u32;
    const amba_ik_cr_fence_info_t *p_fence = NULL;
    uint32 i;
    uint32 index;
    uint32 pre_idx;
    uint32 depth;

    rval |= img_ctx_get_context(context_id, &p_ctx);

    if(rval == IK_OK) {
        p_fence = p_ctx->fence;
        depth = p_ctx->organization.attribute.cr_ring_number;
        depth = uint32_min_ik(depth, 3u);

        // TBD, should we check update flags??
        if(p_ctx->filters.update_flags.cr.cawarp != 0u) {
            index = (uint32)p_ctx->organization.active_cr_state.cr_index.cawarp;
            for(i=0u; i<depth; i++) {
                rval |= exe_get_previous_flow_control_index(&p_ctx->organization, index, i, &pre_idx);
                rval |= img_ctx_get_flow_tbl(context_id, pre_idx, &addr);
                (void)amba_ik_system_memcpy(&p_flow_tbl, &addr, sizeof(void *));

                tmp_p_u32 = p_flow_tbl->fence_0;
                (void)amba_ik_system_memcpy(&p_tmp_u8, &tmp_p_u32, sizeof(void *));
                p_tmp_u8 = &p_tmp_u8[p_fence[60].offset];
                rval |= ik_cr_memory_fence_check(0u, p_fence[60].length, p_tmp_u8);

                tmp_p_u32 = p_flow_tbl->fence_1;
                (void)amba_ik_system_memcpy(&p_tmp_u8, &tmp_p_u32, sizeof(void *));
                p_tmp_u8 = &p_tmp_u8[p_fence[61].offset];
                rval |= ik_cr_memory_fence_check(0u, p_fence[61].length, p_tmp_u8);

                tmp_p_u32 = p_flow_tbl->fence_2;
                (void)amba_ik_system_memcpy(&p_tmp_u8, &tmp_p_u32, sizeof(void *));
                p_tmp_u8 = &p_tmp_u8[p_fence[62].offset];
                rval |= ik_cr_memory_fence_check(0u, p_fence[62].length, p_tmp_u8);

                tmp_p_u32 = p_flow_tbl->fence_3;
                (void)amba_ik_system_memcpy(&p_tmp_u8, &tmp_p_u32, sizeof(void *));
                p_tmp_u8 = &p_tmp_u8[p_fence[63].offset];
                rval |= ik_cr_memory_fence_check(0u, p_fence[63].length, p_tmp_u8);
            }
        }
        if(p_ctx->filters.update_flags.cr.warp != 0u) {
            index = (uint32)p_ctx->organization.active_cr_state.cr_index.warp;
            for(i=0u; i<depth; i++) {
                rval |= exe_get_previous_flow_control_index(&p_ctx->organization, index, i, &pre_idx);
                rval |= img_ctx_get_flow_tbl(context_id, pre_idx, &addr);
                (void)amba_ik_system_memcpy(&p_flow_tbl, &addr, sizeof(void *));

                tmp_p_u32 = p_flow_tbl->fence_4;
                (void)amba_ik_system_memcpy(&p_tmp_u8, &tmp_p_u32, sizeof(void *));
                p_tmp_u8 = &p_tmp_u8[p_fence[64].offset];
                rval |= ik_cr_memory_fence_check(0u, p_fence[64].length, p_tmp_u8);

                tmp_p_u32 = p_flow_tbl->fence_5;
                (void)amba_ik_system_memcpy(&p_tmp_u8, &tmp_p_u32, sizeof(void *));
                p_tmp_u8 = &p_tmp_u8[p_fence[65].offset];
                rval |= ik_cr_memory_fence_check(0u, p_fence[65].length, p_tmp_u8);
            }
        }
        if(p_ctx->filters.update_flags.cr.aaa_data != 0u) {
            index = (uint32)p_ctx->organization.active_cr_state.cr_index.aaa_data;
            for(i=0u; i<depth; i++) {
                rval |= exe_get_previous_flow_control_index(&p_ctx->organization, index, i, &pre_idx);
                rval |= img_ctx_get_flow_tbl(context_id, pre_idx, &addr);
                (void)amba_ik_system_memcpy(&p_flow_tbl, &addr, sizeof(void *));

                tmp_p_u32 = p_flow_tbl->fence_6;
                (void)amba_ik_system_memcpy(&p_tmp_u8, &tmp_p_u32, sizeof(void *));
                p_tmp_u8 = &p_tmp_u8[p_fence[66].offset];
                rval |= ik_cr_memory_fence_check(0u, p_fence[66].length, p_tmp_u8);
            }
        }
        if(p_ctx->filters.update_flags.cr.frame_info != 0u) {
            index = (uint32)p_ctx->organization.active_cr_state.cr_index.frame_info;
            for(i=0u; i<depth; i++) {
                rval |= exe_get_previous_flow_control_index(&p_ctx->organization, index, i, &pre_idx);
                rval |= img_ctx_get_flow_tbl(context_id, pre_idx, &addr);
                (void)amba_ik_system_memcpy(&p_flow_tbl, &addr, sizeof(void *));

                tmp_p_u32 = p_flow_tbl->fence_7;
                (void)amba_ik_system_memcpy(&p_tmp_u8, &tmp_p_u32, sizeof(void *));
                p_tmp_u8 = &p_tmp_u8[p_fence[67].offset];
                rval |= ik_cr_memory_fence_check(0u, p_fence[67].length, p_tmp_u8);
            }
        }
    } else {
        amba_ik_system_print_str_5("[IK] can't get context", NULL, NULL, NULL, NULL, NULL);
    }

    return rval;
}

static uint32 exe_ik_check_ctx_mem_fence_2(uint32 context_id)
{
    uint32 rval = IK_OK;
    uint32 golden = 0x58585858UL;
    amba_ik_context_entity_t *p_ctx;

    rval |= img_ctx_get_context(context_id, &p_ctx);

    if(rval == IK_OK) {
        if(golden != p_ctx->filters.input_param.ctx_fence_10) {
            amba_ik_system_print_str_5("[IK] detect context memory corruption at 10", NULL, NULL, NULL, NULL, NULL);
            rval |= IK_ERR_0203;
        }
        if(golden != p_ctx->filters.input_param.ctx_fence_11) {
            amba_ik_system_print_str_5("[IK] detect context memory corruption at 11", NULL, NULL, NULL, NULL, NULL);
            rval |= IK_ERR_0203;
        }
        if(golden != p_ctx->filters.input_param.ctx_fence_12) {
            amba_ik_system_print_str_5("[IK] detect context memory corruption at 12", NULL, NULL, NULL, NULL, NULL);
            rval |= IK_ERR_0203;
        }
        if(golden != p_ctx->filters.input_param.ctx_fence_13) {
            amba_ik_system_print_str_5("[IK] detect context memory corruption at 13", NULL, NULL, NULL, NULL, NULL);
            rval |= IK_ERR_0203;
        }
        if(golden != p_ctx->filters.input_param.ctx_fence_14) {
            amba_ik_system_print_str_5("[IK] detect context memory corruption at 14", NULL, NULL, NULL, NULL, NULL);
            rval |= IK_ERR_0203;
        }
        if(golden != p_ctx->filters.input_param.ctx_fence_15) {
            amba_ik_system_print_str_5("[IK] detect context memory corruption at 15", NULL, NULL, NULL, NULL, NULL);
            rval |= IK_ERR_0203;
        }
        if(golden != p_ctx->filters.input_param.ctx_fence_16) {
            amba_ik_system_print_str_5("[IK] detect context memory corruption at 16", NULL, NULL, NULL, NULL, NULL);
            rval |= IK_ERR_0203;
        }
        if(golden != p_ctx->filters.input_param.ctx_fence_17) {
            amba_ik_system_print_str_5("[IK] detect context memory corruption at 17", NULL, NULL, NULL, NULL, NULL);
            rval |= IK_ERR_0203;
        }
        if(golden != p_ctx->filters.input_param.ctx_fence_18) {
            amba_ik_system_print_str_5("[IK] detect context memory corruption at 18", NULL, NULL, NULL, NULL, NULL);
            rval |= IK_ERR_0203;
        }
        if(golden != p_ctx->filters.input_param.ctx_fence_19) {
            amba_ik_system_print_str_5("[IK] detect context memory corruption at 19", NULL, NULL, NULL, NULL, NULL);
            rval |= IK_ERR_0203;
        }
    } else {
        amba_ik_system_print_str_5("[IK] can't get context", NULL, NULL, NULL, NULL, NULL);
    }

    return rval;
}

static uint32 exe_ik_check_ctx_mem_fence_1(uint32 context_id)
{
    uint32 rval = IK_OK;
    uint32 golden = 0x58585858UL;
    amba_ik_context_entity_t *p_ctx;

    rval |= img_ctx_get_context(context_id, &p_ctx);

    if(rval == IK_OK) {
        if(golden != p_ctx->filters.input_param.ctx_fence_0) {
            amba_ik_system_print_str_5("[IK] detect context memory corruption at 0", NULL, NULL, NULL, NULL, NULL);
            rval |= IK_ERR_0203;
        }
        if(golden != p_ctx->filters.input_param.ctx_fence_1) {
            amba_ik_system_print_str_5("[IK] detect context memory corruption at 1", NULL, NULL, NULL, NULL, NULL);
            rval |= IK_ERR_0203;
        }
        if(golden != p_ctx->filters.input_param.ctx_fence_2) {
            amba_ik_system_print_str_5("[IK] detect context memory corruption at 2", NULL, NULL, NULL, NULL, NULL);
            rval |= IK_ERR_0203;
        }
        if(golden != p_ctx->filters.input_param.ctx_fence_3) {
            amba_ik_system_print_str_5("[IK] detect context memory corruption at 3", NULL, NULL, NULL, NULL, NULL);
            rval |= IK_ERR_0203;
        }
        if(golden != p_ctx->filters.input_param.ctx_fence_4) {
            amba_ik_system_print_str_5("[IK] detect context memory corruption at 4", NULL, NULL, NULL, NULL, NULL);
            rval |= IK_ERR_0203;
        }
        if(golden != p_ctx->filters.input_param.ctx_fence_5) {
            amba_ik_system_print_str_5("[IK] detect context memory corruption at 5", NULL, NULL, NULL, NULL, NULL);
            rval |= IK_ERR_0203;
        }
        if(golden != p_ctx->filters.input_param.ctx_fence_6) {
            amba_ik_system_print_str_5("[IK] detect context memory corruption at 6", NULL, NULL, NULL, NULL, NULL);
            rval |= IK_ERR_0203;
        }
        if(golden != p_ctx->filters.input_param.ctx_fence_7) {
            amba_ik_system_print_str_5("[IK] detect context memory corruption at 7", NULL, NULL, NULL, NULL, NULL);
            rval |= IK_ERR_0203;
        }
        if(golden != p_ctx->filters.input_param.ctx_fence_8) {
            amba_ik_system_print_str_5("[IK] detect context memory corruption at 8", NULL, NULL, NULL, NULL, NULL);
            rval |= IK_ERR_0203;
        }
        if(golden != p_ctx->filters.input_param.ctx_fence_9) {
            amba_ik_system_print_str_5("[IK] detect context memory corruption at 9", NULL, NULL, NULL, NULL, NULL);
            rval |= IK_ERR_0203;
        }
    } else {
        amba_ik_system_print_str_5("[IK] can't get context", NULL, NULL, NULL, NULL, NULL);
    }

    return rval;
}


static uint32 exe_ik_check_ctx_mem_fence(uint32 context_id)
{
    uint32 rval = IK_OK;
    rval |= exe_ik_check_ctx_mem_fence_1(context_id);
    rval |= exe_ik_check_ctx_mem_fence_2(context_id);
    return rval;
}

static uint32 img_exe_inject_safety_error_mem_fence(amba_ik_context_entity_t *p_ctx, const amba_ik_flow_tables_list_t *p_flow_tbl_list)
{
    uint32 rval = IK_OK;
    const amba_ik_cr_fence_info_t *p_fence = NULL;
    uint8 *p_tmp_u8 = NULL;
    uint32 aaa_size = (sizeof(idsp_ik_aaa_data_t) * IK_MAX_TILE_NUM_Y) * IK_MAX_TILE_NUM_X;

    // corrupt ctx fences in purpose
    p_ctx->filters.input_param.ctx_fence_0 |= 0x1u;
    p_ctx->filters.input_param.ctx_fence_1 |= 0x1u;
    p_ctx->filters.input_param.ctx_fence_2 |= 0x1u;
    p_ctx->filters.input_param.ctx_fence_3 |= 0x1u;
    p_ctx->filters.input_param.ctx_fence_4 |= 0x1u;
    p_ctx->filters.input_param.ctx_fence_5 |= 0x1u;
    p_ctx->filters.input_param.ctx_fence_6 |= 0x1u;
    p_ctx->filters.input_param.ctx_fence_7 |= 0x1u;
    p_ctx->filters.input_param.ctx_fence_8 |= 0x1u;
    p_ctx->filters.input_param.ctx_fence_9 |= 0x1u;
    p_ctx->filters.input_param.ctx_fence_10 |= 0x1u;
    p_ctx->filters.input_param.ctx_fence_11 |= 0x1u;
    p_ctx->filters.input_param.ctx_fence_12 |= 0x1u;
    p_ctx->filters.input_param.ctx_fence_13 |= 0x1u;
    p_ctx->filters.input_param.ctx_fence_14 |= 0x1u;
    p_ctx->filters.input_param.ctx_fence_15 |= 0x1u;
    p_ctx->filters.input_param.ctx_fence_16 |= 0x1u;
    p_ctx->filters.input_param.ctx_fence_17 |= 0x1u;
    p_ctx->filters.input_param.ctx_fence_18 |= 0x1u;
    p_ctx->filters.input_param.ctx_fence_19 |= 0x1u;

    // corrupt any cr fences in purpose
    p_fence = p_ctx->fence;
    if(p_flow_tbl_list->p_CR_buf_22 != NULL) {
        (void)amba_ik_system_memcpy(&p_tmp_u8, &p_flow_tbl_list->p_CR_buf_22, sizeof(void *));
        p_tmp_u8 = &p_tmp_u8[p_fence[18].offset];
        *p_tmp_u8 |= 0x1u;
    }

    // corrupt any flow_tbl fences in purpose
    // TBD, should we check update flags??
    if(p_ctx->filters.update_flags.cr.aaa_data != 0u) {
        (void)amba_ik_system_memcpy(&p_tmp_u8, &p_flow_tbl_list->p_aaa, sizeof(void *));
        p_tmp_u8 = &p_tmp_u8[aaa_size];
        *p_tmp_u8 |= 0x1u;
    }

    return rval;
}
#endif

static inline uint32 img_exe_inject_safety_crc_mismatch_error(const amba_ik_flow_tables_list_t *p_flow_tbl_list)
{
    if(p_flow_tbl_list->p_CR_buf_11 != NULL) {
        (void)amba_ik_system_memset(p_flow_tbl_list->p_CR_buf_11, 0x0, 4u);
    }
    if(p_flow_tbl_list->p_CR_buf_21 != NULL) {
        (void)amba_ik_system_memset(p_flow_tbl_list->p_CR_buf_21, 0x0, 4u);
    }
    if(p_flow_tbl_list->p_CR_buf_30 != NULL) {
        (void)amba_ik_system_memset(p_flow_tbl_list->p_CR_buf_30, 0x0, 4u);
    }
    if(p_flow_tbl_list->p_CR_buf_33 != NULL) {
        (void)amba_ik_system_memset(p_flow_tbl_list->p_CR_buf_33, 0x0, 4u);
    }
    if(p_flow_tbl_list->p_CR_buf_35 != NULL) {
        (void)amba_ik_system_memset(p_flow_tbl_list->p_CR_buf_35, 0x0, 4u);
    }
    if(p_flow_tbl_list->p_CR_buf_42 != NULL) {
        (void)amba_ik_system_memset(p_flow_tbl_list->p_CR_buf_42, 0x0, 4u);
    }
    if(p_flow_tbl_list->p_CR_buf_49 != NULL) {
        (void)amba_ik_system_memset(p_flow_tbl_list->p_CR_buf_49, 0x0, 4u);
    }
    if(p_flow_tbl_list->p_CR_buf_112 != NULL) {
        (void)amba_ik_system_memset(p_flow_tbl_list->p_CR_buf_112, 0x0, 4u);
    }
    return IK_OK;
}

#ifndef EARLYTEST_ENV
static uint32 exe_ik_select_crc_check_list(uint32 context_id)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx;

    rval |= img_ctx_get_context(context_id, &p_ctx);

    if(rval == IK_OK) {
#if 0
        p_ctx->filters.crc_check_list.flow_ctrl = 1u;

        p_ctx->filters.crc_check_list.cr_4 = 1u;
        p_ctx->filters.crc_check_list.cr_5 = 1u;
        p_ctx->filters.crc_check_list.cr_6 = 1u;
        p_ctx->filters.crc_check_list.cr_7 = 1u;
        p_ctx->filters.crc_check_list.cr_8 = 1u;
        p_ctx->filters.crc_check_list.cr_12 = 1u;
        p_ctx->filters.crc_check_list.cr_13 = 1u;
        p_ctx->filters.crc_check_list.cr_14 = 1u;
        p_ctx->filters.crc_check_list.cr_22 = 1u;
        p_ctx->filters.crc_check_list.cr_23 = 1u;
        p_ctx->filters.crc_check_list.cr_24 = 1u;
        p_ctx->filters.crc_check_list.cr_26 = 1u;
        p_ctx->filters.crc_check_list.cr_27 = 1u;
        p_ctx->filters.crc_check_list.cr_29 = 1u;
        p_ctx->filters.crc_check_list.cr_30 = 1u;

#if 1 // for window_geo
        p_ctx->filters.crc_check_list.cawarp = 1u;
        p_ctx->filters.crc_check_list.warp = 1u;
        p_ctx->filters.crc_check_list.aaa_data = 1u;
        p_ctx->filters.crc_check_list.cr_4 = 1u;
        p_ctx->filters.crc_check_list.cr_9 = 1u;
        p_ctx->filters.crc_check_list.cr_10 = 1u;
        p_ctx->filters.crc_check_list.cr_11 = 1u;
        p_ctx->filters.crc_check_list.cr_12 = 1u;
        p_ctx->filters.crc_check_list.cr_13 = 1u;
        p_ctx->filters.crc_check_list.cr_16 = 1u;
        p_ctx->filters.crc_check_list.cr_17 = 1u;
        p_ctx->filters.crc_check_list.cr_21 = 1u;
        p_ctx->filters.crc_check_list.cr_28 = 1u;
        p_ctx->filters.crc_check_list.cr_30 = 1u;
        p_ctx->filters.crc_check_list.cr_33 = 1u;
        p_ctx->filters.crc_check_list.cr_34 = 1u;
        p_ctx->filters.crc_check_list.cr_35 = 1u;
        p_ctx->filters.crc_check_list.cr_36 = 1u;
        p_ctx->filters.crc_check_list.cr_37 = 1u;
        p_ctx->filters.crc_check_list.cr_42 = 1u;
        p_ctx->filters.crc_check_list.cr_43 = 1u;
        p_ctx->filters.crc_check_list.cr_44 = 1u;
        p_ctx->filters.crc_check_list.cr_45 = 1u;
        p_ctx->filters.crc_check_list.cr_47 = 1u;
        p_ctx->filters.crc_check_list.cr_48 = 1u;
        p_ctx->filters.crc_check_list.cr_49 = 1u;
        p_ctx->filters.crc_check_list.cr_50 = 1u;
        p_ctx->filters.crc_check_list.cr_51 = 1u;
        p_ctx->filters.crc_check_list.cr_111 = 1u;
        p_ctx->filters.crc_check_list.cr_112 = 1u;
        p_ctx->filters.crc_check_list.cr_113 = 1u;
        p_ctx->filters.crc_check_list.cr_116 = 1u;
        p_ctx->filters.crc_check_list.cr_117 = 1u;
#endif

        if ((p_ctx->organization.attribute.ability.pipe == AMBA_IK_VIDEO_LINEAR_CE) || (p_ctx->organization.attribute.ability.pipe == AMBA_IK_VIDEO_HDR_EXPO_2) ||
            (p_ctx->organization.attribute.ability.pipe == AMBA_IK_VIDEO_HDR_EXPO_3)) {
            p_ctx->filters.crc_check_list.cr_45 = 1u;
            p_ctx->filters.crc_check_list.cr_46 = 1u;
            p_ctx->filters.crc_check_list.cr_49 = 1u;
            p_ctx->filters.crc_check_list.cr_50 = 1u;
            p_ctx->filters.crc_check_list.cr_51 = 1u;
            p_ctx->filters.crc_check_list.cr_52 = 1u;
        }

#else // jack hack for profiling
        (void)amba_ik_system_memset(&p_ctx->filters.crc_check_list, 0xff, sizeof(amba_ik_crc_check_list_t));
#endif
    } else {
        amba_ik_system_print_str_5("[IK] can't get context", NULL, NULL, NULL, NULL, NULL);
    }

    return rval;
}

static inline uint32 exe_ik_calc_crc32_hdr(uint32 context_id, uint32 flow_idx)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx;
    uintptr addr = 0;
    idsp_crc_data_t *p_crc = NULL;
    const amba_ik_flow_tables_list_t *p_flow_tbl_list = NULL;
    const uint8 *p_misra_u8 = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);

    if(rval == IK_OK) {
        rval |= img_ctx_get_crc_data(context_id, flow_idx, &addr);
        (void)amba_ik_system_memcpy(&p_crc, &addr, sizeof(void *));

        rval |= img_ctx_get_flow_tbl_list(context_id, flow_idx, &addr);
        (void)amba_ik_system_memcpy(&p_flow_tbl_list, &addr, sizeof(void *));

        if(p_ctx->filters.crc_check_list.cr_4 != 0u) {
            (void)amba_ik_system_memcpy(&p_misra_u8, &p_flow_tbl_list->p_CR_buf_4, sizeof(uint8 *));
            p_crc->sec2_cfg_buf[0] = amba_ik_system_crc32(p_misra_u8, CR_SIZE_4);
        }

        if(p_ctx->filters.crc_check_list.cr_5 != 0u) {
            (void)amba_ik_system_memcpy(&p_misra_u8, &p_flow_tbl_list->p_CR_buf_5, sizeof(uint8 *));
            p_crc->sec2_cfg_buf[1] = amba_ik_system_crc32(p_misra_u8, CR_SIZE_5);
        }

        if(p_ctx->filters.crc_check_list.cr_6 != 0u) {
            (void)amba_ik_system_memcpy(&p_misra_u8, &p_flow_tbl_list->p_CR_buf_6, sizeof(uint8 *));
            p_crc->sec2_cfg_buf[2] = amba_ik_system_crc32(p_misra_u8, CR_SIZE_6);
        }

        if(p_ctx->filters.crc_check_list.cr_7 != 0u) {
            (void)amba_ik_system_memcpy(&p_misra_u8, &p_flow_tbl_list->p_CR_buf_7, sizeof(uint8 *));
            p_crc->sec2_cfg_buf[3] = amba_ik_system_crc32(p_misra_u8, CR_SIZE_7);
        }

        if(p_ctx->filters.crc_check_list.cr_8 != 0u) {
            (void)amba_ik_system_memcpy(&p_misra_u8, &p_flow_tbl_list->p_CR_buf_8, sizeof(uint8 *));
            p_crc->sec2_cfg_buf[4] = amba_ik_system_crc32(p_misra_u8, CR_SIZE_8);
        }

        if(p_ctx->filters.crc_check_list.cr_13 != 0u) {
            (void)amba_ik_system_memcpy(&p_misra_u8, &p_flow_tbl_list->p_CR_buf_13, sizeof(uint8 *));
            p_crc->sec2_cfg_buf[9] = amba_ik_system_crc32(p_misra_u8, CR_SIZE_13);
        }

        if(p_ctx->filters.crc_check_list.cr_14 != 0u) {
            (void)amba_ik_system_memcpy(&p_misra_u8, &p_flow_tbl_list->p_CR_buf_14, sizeof(uint8 *));
            p_crc->sec2_cfg_buf[10] = amba_ik_system_crc32(p_misra_u8, CR_SIZE_14);
        }

        if(p_ctx->filters.crc_check_list.cr_15 != 0u) {
            (void)amba_ik_system_memcpy(&p_misra_u8, &p_flow_tbl_list->p_CR_buf_15, sizeof(uint8 *));
            p_crc->sec2_cfg_buf[11] = amba_ik_system_crc32(p_misra_u8, CR_SIZE_15);
        }
    }

    return rval;
}

static inline uint32 exe_ik_calc_crc32_cfa(uint32 context_id, uint32 flow_idx)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx;
    uintptr addr = 0;
    idsp_crc_data_t *p_crc = NULL;
    const amba_ik_flow_tables_list_t *p_flow_tbl_list = NULL;
    const uint8 *p_misra_u8 = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);

    if(rval == IK_OK) {
        rval |= img_ctx_get_crc_data(context_id, flow_idx, &addr);
        (void)amba_ik_system_memcpy(&p_crc, &addr, sizeof(void *));

        rval |= img_ctx_get_flow_tbl_list(context_id, flow_idx, &addr);
        (void)amba_ik_system_memcpy(&p_flow_tbl_list, &addr, sizeof(void *));

        if(p_ctx->filters.crc_check_list.cr_9 != 0u) {
            (void)amba_ik_system_memcpy(&p_misra_u8, &p_flow_tbl_list->p_CR_buf_9, sizeof(uint8 *));
            p_crc->sec2_cfg_buf[5] = amba_ik_system_crc32(p_misra_u8, CR_SIZE_9);
        }

        if(p_ctx->filters.crc_check_list.cr_10 != 0u) {
            (void)amba_ik_system_memcpy(&p_misra_u8, &p_flow_tbl_list->p_CR_buf_10, sizeof(uint8 *));
            p_crc->sec2_cfg_buf[6] = amba_ik_system_crc32(p_misra_u8, CR_SIZE_10);
        }

        if(p_ctx->filters.crc_check_list.cr_11 != 0u) {
            (void)amba_ik_system_memcpy(&p_misra_u8, &p_flow_tbl_list->p_CR_buf_11, sizeof(uint8 *));
            p_crc->sec2_cfg_buf[7] = amba_ik_system_crc32(p_misra_u8, CR_SIZE_11);
        }

        if(p_ctx->filters.crc_check_list.cr_12 != 0u) {
            (void)amba_ik_system_memcpy(&p_misra_u8, &p_flow_tbl_list->p_CR_buf_12, sizeof(uint8 *));
            p_crc->sec2_cfg_buf[8] = amba_ik_system_crc32(p_misra_u8, CR_SIZE_12);
        }

        if(p_ctx->filters.crc_check_list.cr_16 != 0u) {
            (void)amba_ik_system_memcpy(&p_misra_u8, &p_flow_tbl_list->p_CR_buf_16, sizeof(uint8 *));
            p_crc->sec2_cfg_buf[12] = amba_ik_system_crc32(p_misra_u8, CR_SIZE_16);
        }

        if(p_ctx->filters.crc_check_list.cr_17 != 0u) {
            (void)amba_ik_system_memcpy(&p_misra_u8, &p_flow_tbl_list->p_CR_buf_17, sizeof(uint8 *));
            p_crc->sec2_cfg_buf[13] = amba_ik_system_crc32(p_misra_u8, CR_SIZE_17);

            (void)amba_ik_system_memcpy(&p_misra_u8, &p_flow_tbl_list->p_CR_buf_18, sizeof(uint8 *));
            p_crc->sec2_cfg_buf[14] = amba_ik_system_crc32(p_misra_u8, CR_SIZE_18);

            (void)amba_ik_system_memcpy(&p_misra_u8, &p_flow_tbl_list->p_CR_buf_19, sizeof(uint8 *));
            p_crc->sec2_cfg_buf[15] = amba_ik_system_crc32(p_misra_u8, CR_SIZE_19);

            (void)amba_ik_system_memcpy(&p_misra_u8, &p_flow_tbl_list->p_CR_buf_20, sizeof(uint8 *));
            p_crc->sec2_cfg_buf[16] = amba_ik_system_crc32(p_misra_u8, CR_SIZE_20);
        }

        if(p_ctx->filters.crc_check_list.cr_21 != 0u) {
            (void)amba_ik_system_memcpy(&p_misra_u8, &p_flow_tbl_list->p_CR_buf_21, sizeof(uint8 *));
            p_crc->sec2_cfg_buf[17] = amba_ik_system_crc32(p_misra_u8, CR_SIZE_21);
        }

        if(p_ctx->filters.crc_check_list.cr_22 != 0u) {
            (void)amba_ik_system_memcpy(&p_misra_u8, &p_flow_tbl_list->p_CR_buf_22, sizeof(uint8 *));
            p_crc->sec2_cfg_buf[18] = amba_ik_system_crc32(p_misra_u8, CR_SIZE_22);
        }

        if(p_ctx->filters.crc_check_list.cr_23 != 0u) {
            (void)amba_ik_system_memcpy(&p_misra_u8, &p_flow_tbl_list->p_CR_buf_23, sizeof(uint8 *));
            p_crc->sec2_cfg_buf[19] = amba_ik_system_crc32(p_misra_u8, CR_SIZE_23);
        }
    }

    return rval;
}

static inline uint32 exe_ik_calc_crc32_rgb(uint32 context_id, uint32 flow_idx)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx;
    uintptr addr = 0;
    idsp_crc_data_t *p_crc = NULL;
    const amba_ik_flow_tables_list_t *p_flow_tbl_list = NULL;
    const uint8 *p_misra_u8 = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);

    if(rval == IK_OK) {
        rval |= img_ctx_get_crc_data(context_id, flow_idx, &addr);
        (void)amba_ik_system_memcpy(&p_crc, &addr, sizeof(void *));

        rval |= img_ctx_get_flow_tbl_list(context_id, flow_idx, &addr);
        (void)amba_ik_system_memcpy(&p_flow_tbl_list, &addr, sizeof(void *));

        if(p_ctx->filters.crc_check_list.cr_24 != 0u) {
            (void)amba_ik_system_memcpy(&p_misra_u8, &p_flow_tbl_list->p_CR_buf_24, sizeof(uint8 *));
            p_crc->sec2_cfg_buf[20] = amba_ik_system_crc32(p_misra_u8, CR_SIZE_24);
        }

        if(p_ctx->filters.crc_check_list.cr_25 != 0u) {
            (void)amba_ik_system_memcpy(&p_misra_u8, &p_flow_tbl_list->p_CR_buf_25, sizeof(uint8 *));
            p_crc->sec2_cfg_buf[21] = amba_ik_system_crc32(p_misra_u8, CR_SIZE_25);
        }

        if(p_ctx->filters.crc_check_list.cr_26 != 0u) {
            (void)amba_ik_system_memcpy(&p_misra_u8, &p_flow_tbl_list->p_CR_buf_26, sizeof(uint8 *));
            p_crc->sec2_cfg_buf[22] = amba_ik_system_crc32(p_misra_u8, CR_SIZE_26);
        }

        if(p_ctx->filters.crc_check_list.cr_27 != 0u) {
            (void)amba_ik_system_memcpy(&p_misra_u8, &p_flow_tbl_list->p_CR_buf_27, sizeof(uint8 *));
            p_crc->sec2_cfg_buf[23] = amba_ik_system_crc32(p_misra_u8, CR_SIZE_27);
        }

        if(p_ctx->filters.crc_check_list.cr_28 != 0u) {
            (void)amba_ik_system_memcpy(&p_misra_u8, &p_flow_tbl_list->p_CR_buf_28, sizeof(uint8 *));
            p_crc->sec2_cfg_buf[24] = amba_ik_system_crc32(p_misra_u8, CR_SIZE_28);
        }

        if(p_ctx->filters.crc_check_list.cr_29 != 0u) {
            (void)amba_ik_system_memcpy(&p_misra_u8, &p_flow_tbl_list->p_CR_buf_29, sizeof(uint8 *));
            p_crc->sec2_cfg_buf[25] = amba_ik_system_crc32(p_misra_u8, CR_SIZE_29);
        }
    }

    return rval;
}

static inline uint32 exe_ik_calc_crc32_yuv(uint32 context_id, uint32 flow_idx)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx;
    uintptr addr = 0;
    idsp_crc_data_t *p_crc = NULL;
    const amba_ik_flow_tables_list_t *p_flow_tbl_list = NULL;
    const uint8 *p_misra_u8 = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);

    if(rval == IK_OK) {
        rval |= img_ctx_get_crc_data(context_id, flow_idx, &addr);
        (void)amba_ik_system_memcpy(&p_crc, &addr, sizeof(void *));

        rval |= img_ctx_get_flow_tbl_list(context_id, flow_idx, &addr);
        (void)amba_ik_system_memcpy(&p_flow_tbl_list, &addr, sizeof(void *));

        if(p_ctx->filters.crc_check_list.cr_30 != 0u) {
            (void)amba_ik_system_memcpy(&p_misra_u8, &p_flow_tbl_list->p_CR_buf_30, sizeof(uint8 *));
            p_crc->sec2_cfg_buf[26] = amba_ik_system_crc32(p_misra_u8, CR_SIZE_30);
        }

        if(p_ctx->filters.crc_check_list.cr_31 != 0u) {
            (void)amba_ik_system_memcpy(&p_misra_u8, &p_flow_tbl_list->p_CR_buf_31, sizeof(uint8 *));
            p_crc->sec2_cfg_buf[27] = amba_ik_system_crc32(p_misra_u8, CR_SIZE_31);
        }

        if(p_ctx->filters.crc_check_list.cr_32 != 0u) {
            (void)amba_ik_system_memcpy(&p_misra_u8, &p_flow_tbl_list->p_CR_buf_32, sizeof(uint8 *));
            p_crc->sec2_cfg_buf[28] = amba_ik_system_crc32(p_misra_u8, CR_SIZE_32);
        }

        if(p_ctx->filters.crc_check_list.cr_33 != 0u) {
            (void)amba_ik_system_memcpy(&p_misra_u8, &p_flow_tbl_list->p_CR_buf_33, sizeof(uint8 *));
            p_crc->sec2_cfg_buf[29] = amba_ik_system_crc32(p_misra_u8, CR_SIZE_33);
        }

        if(p_ctx->filters.crc_check_list.cr_34 != 0u) {
            (void)amba_ik_system_memcpy(&p_misra_u8, &p_flow_tbl_list->p_CR_buf_34, sizeof(uint8 *));
            p_crc->sec2_cfg_buf[30] = amba_ik_system_crc32(p_misra_u8, CR_SIZE_34);
        }

        if(p_ctx->filters.crc_check_list.cr_35 != 0u) {
            (void)amba_ik_system_memcpy(&p_misra_u8, &p_flow_tbl_list->p_CR_buf_35, sizeof(uint8 *));
            p_crc->sec2_cfg_buf[31] = amba_ik_system_crc32(p_misra_u8, CR_SIZE_35);
        }

        if(p_ctx->filters.crc_check_list.cr_36 != 0u) {
            (void)amba_ik_system_memcpy(&p_misra_u8, &p_flow_tbl_list->p_CR_buf_36, sizeof(uint8 *));
            p_crc->sec2_cfg_buf[33] = amba_ik_system_crc32(p_misra_u8, CR_SIZE_36);
        }

        if(p_ctx->filters.crc_check_list.cr_37 != 0u) {
            (void)amba_ik_system_memcpy(&p_misra_u8, &p_flow_tbl_list->p_CR_buf_37, sizeof(uint8 *));
            p_crc->sec2_cfg_buf[34] = amba_ik_system_crc32(p_misra_u8, CR_SIZE_37);
        }

        if(p_ctx->filters.crc_check_list.cr_38 != 0u) {
            (void)amba_ik_system_memcpy(&p_misra_u8, &p_flow_tbl_list->p_CR_buf_38, sizeof(uint8 *));
            p_crc->sec2_cfg_buf[35] = amba_ik_system_crc32(p_misra_u8, CR_SIZE_38);
        }

        if(p_ctx->filters.crc_check_list.cr_39 != 0u) {
            (void)amba_ik_system_memcpy(&p_misra_u8, &p_flow_tbl_list->p_CR_buf_39, sizeof(uint8 *));
            p_crc->sec2_cfg_buf[36] = amba_ik_system_crc32(p_misra_u8, CR_SIZE_39);
        }

        if(p_ctx->filters.crc_check_list.cr_40 != 0u) {
            (void)amba_ik_system_memcpy(&p_misra_u8, &p_flow_tbl_list->p_CR_buf_40, sizeof(uint8 *));
            p_crc->sec2_cfg_buf[32] = amba_ik_system_crc32(p_misra_u8, CR_SIZE_40);
        }

        if(p_ctx->filters.crc_check_list.cr_41 != 0u) {
            (void)amba_ik_system_memcpy(&p_misra_u8, &p_flow_tbl_list->p_CR_buf_41, sizeof(uint8 *));
            p_crc->sec2_cfg_buf[37] = amba_ik_system_crc32(p_misra_u8, CR_SIZE_41);
        }
    }

    return rval;
}

static inline uint32 exe_ik_calc_crc32_sec3(uint32 context_id, uint32 flow_idx)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx;
    uintptr addr = 0;
    idsp_crc_data_t *p_crc = NULL;
    const amba_ik_flow_tables_list_t *p_flow_tbl_list = NULL;
    const uint8 *p_misra_u8 = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);

    if(rval == IK_OK) {
        rval |= img_ctx_get_crc_data(context_id, flow_idx, &addr);
        (void)amba_ik_system_memcpy(&p_crc, &addr, sizeof(void *));

        rval |= img_ctx_get_flow_tbl_list(context_id, flow_idx, &addr);
        (void)amba_ik_system_memcpy(&p_flow_tbl_list, &addr, sizeof(void *));

        if(p_ctx->filters.crc_check_list.cr_42 != 0u) {
            (void)amba_ik_system_memcpy(&p_misra_u8, &p_flow_tbl_list->p_CR_buf_42, sizeof(uint8 *));
            p_crc->sec3_cfg_buf[0] = amba_ik_system_crc32(p_misra_u8, CR_SIZE_42);
        }

        if(p_ctx->filters.crc_check_list.cr_43 != 0u) {
            (void)amba_ik_system_memcpy(&p_misra_u8, &p_flow_tbl_list->p_CR_buf_43, sizeof(uint8 *));
            p_crc->sec3_cfg_buf[1] = amba_ik_system_crc32(p_misra_u8, CR_SIZE_43);
        }

        if(p_ctx->filters.crc_check_list.cr_44 != 0u) {
            (void)amba_ik_system_memcpy(&p_misra_u8, &p_flow_tbl_list->p_CR_buf_44, sizeof(uint8 *));
            p_crc->sec3_cfg_buf[2] = amba_ik_system_crc32(p_misra_u8, CR_SIZE_44);
        }
    }

    return rval;
}

static inline uint32 exe_ik_calc_crc32_sec4(uint32 context_id, uint32 flow_idx)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx;
    uintptr addr = 0;
    idsp_crc_data_t *p_crc = NULL;
    const amba_ik_flow_tables_list_t *p_flow_tbl_list = NULL;
    const uint8 *p_misra_u8 = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);

    if(rval == IK_OK) {
        rval |= img_ctx_get_crc_data(context_id, flow_idx, &addr);
        (void)amba_ik_system_memcpy(&p_crc, &addr, sizeof(void *));

        rval |= img_ctx_get_flow_tbl_list(context_id, flow_idx, &addr);
        (void)amba_ik_system_memcpy(&p_flow_tbl_list, &addr, sizeof(void *));

        if ((p_ctx->organization.attribute.ability.video_pipe == AMBA_IK_VIDEO_LINEAR_CE)||
            (p_ctx->organization.attribute.ability.video_pipe == AMBA_IK_VIDEO_HDR_EXPO_2)||
            (p_ctx->organization.attribute.ability.video_pipe == AMBA_IK_VIDEO_HDR_EXPO_3)) {

            if(p_ctx->filters.crc_check_list.cr_45 != 0u) {
                (void)amba_ik_system_memcpy(&p_misra_u8, &p_flow_tbl_list->p_CR_buf_45, sizeof(uint8 *));
                p_crc->sec4_cfg_buf[0] = amba_ik_system_crc32(p_misra_u8, CR_SIZE_45);
            }

            if(p_ctx->filters.crc_check_list.cr_46 != 0u) {
                (void)amba_ik_system_memcpy(&p_misra_u8, &p_flow_tbl_list->p_CR_buf_46, sizeof(uint8 *));
                p_crc->sec4_cfg_buf[1] = amba_ik_system_crc32(p_misra_u8, CR_SIZE_46);
            }

            if(p_ctx->filters.crc_check_list.cr_47 != 0u) {
                (void)amba_ik_system_memcpy(&p_misra_u8, &p_flow_tbl_list->p_CR_buf_47, sizeof(uint8 *));
                p_crc->sec4_cfg_buf[2] = amba_ik_system_crc32(p_misra_u8, CR_SIZE_47);
            }

            if(p_ctx->filters.crc_check_list.cr_48 != 0u) {
                (void)amba_ik_system_memcpy(&p_misra_u8, &p_flow_tbl_list->p_CR_buf_48, sizeof(uint8 *));
                p_crc->sec4_cfg_buf[3] = amba_ik_system_crc32(p_misra_u8, CR_SIZE_48);
            }

            if(p_ctx->filters.crc_check_list.cr_49 != 0u) {
                (void)amba_ik_system_memcpy(&p_misra_u8, &p_flow_tbl_list->p_CR_buf_49, sizeof(uint8 *));
                p_crc->sec4_cfg_buf[4] = amba_ik_system_crc32(p_misra_u8, CR_SIZE_49);
            }

            if(p_ctx->filters.crc_check_list.cr_50 != 0u) {
                (void)amba_ik_system_memcpy(&p_misra_u8, &p_flow_tbl_list->p_CR_buf_50, sizeof(uint8 *));
                p_crc->sec4_cfg_buf[5] = amba_ik_system_crc32(p_misra_u8, CR_SIZE_50);
            }

            if(p_ctx->filters.crc_check_list.cr_51 != 0u) {
                (void)amba_ik_system_memcpy(&p_misra_u8, &p_flow_tbl_list->p_CR_buf_51, sizeof(uint8 *));
                p_crc->sec4_cfg_buf[6] = amba_ik_system_crc32(p_misra_u8, CR_SIZE_51);
            }

            if(p_ctx->filters.crc_check_list.cr_52 != 0u) {
                (void)amba_ik_system_memcpy(&p_misra_u8, &p_flow_tbl_list->p_CR_buf_52, sizeof(uint8 *));
                p_crc->sec4_cfg_buf[7] = amba_ik_system_crc32(p_misra_u8, CR_SIZE_52);
            }
        } else { //linear or Y2Y doesn't have sec4 CRs.
            p_crc->sec4_cfg_buf[0] = 0U;
            p_crc->sec4_cfg_buf[1] = 0U;
            p_crc->sec4_cfg_buf[2] = 0U;
            p_crc->sec4_cfg_buf[3] = 0U;
            p_crc->sec4_cfg_buf[4] = 0U;
            p_crc->sec4_cfg_buf[5] = 0U;
            p_crc->sec4_cfg_buf[6] = 0U;
            p_crc->sec4_cfg_buf[7] = 0U;
        }
    }

    return rval;
}

static inline uint32 exe_ik_calc_crc32_sec18(uint32 context_id, uint32 flow_idx)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx;
    uintptr addr = 0;
    idsp_crc_data_t *p_crc = NULL;
    const amba_ik_flow_tables_list_t *p_flow_tbl_list = NULL;
    const uint8 *p_misra_u8 = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);

    if(rval == IK_OK) {
        rval |= img_ctx_get_crc_data(context_id, flow_idx, &addr);
        (void)amba_ik_system_memcpy(&p_crc, &addr, sizeof(void *));

        rval |= img_ctx_get_flow_tbl_list(context_id, flow_idx, &addr);
        (void)amba_ik_system_memcpy(&p_flow_tbl_list, &addr, sizeof(void *));

        if(p_ctx->filters.crc_check_list.cr_111 != 0u) {
            (void)amba_ik_system_memcpy(&p_misra_u8, &p_flow_tbl_list->p_CR_buf_111, sizeof(uint8 *));
            p_crc->sec18_cfg_buf[0] = amba_ik_system_crc32(p_misra_u8, CR_SIZE_111);
        }

        if(p_ctx->filters.crc_check_list.cr_112 != 0u) {
            (void)amba_ik_system_memcpy(&p_misra_u8, &p_flow_tbl_list->p_CR_buf_112, sizeof(uint8 *));
            p_crc->sec18_cfg_buf[1] = amba_ik_system_crc32(p_misra_u8, CR_SIZE_112);
        }

        if(p_ctx->filters.crc_check_list.cr_113 != 0u) {
            (void)amba_ik_system_memcpy(&p_misra_u8, &p_flow_tbl_list->p_CR_buf_113, sizeof(uint8 *));
            p_crc->sec18_cfg_buf[2] = amba_ik_system_crc32(p_misra_u8, CR_SIZE_113);
        }

        if(p_ctx->filters.crc_check_list.cr_114 != 0u) {
            (void)amba_ik_system_memcpy(&p_misra_u8, &p_flow_tbl_list->p_CR_buf_114, sizeof(uint8 *));
            p_crc->sec18_cfg_buf[3] = amba_ik_system_crc32(p_misra_u8, CR_SIZE_114);
        }

        if(p_ctx->filters.crc_check_list.cr_115 != 0u) {
            (void)amba_ik_system_memcpy(&p_misra_u8, &p_flow_tbl_list->p_CR_buf_115, sizeof(uint8 *));
            p_crc->sec18_cfg_buf[4] = amba_ik_system_crc32(p_misra_u8, CR_SIZE_115);
        }

        if(p_ctx->filters.crc_check_list.cr_116 != 0u) {
            (void)amba_ik_system_memcpy(&p_misra_u8, &p_flow_tbl_list->p_CR_buf_116, sizeof(uint8 *));
            p_crc->sec18_cfg_buf[5] = amba_ik_system_crc32(p_misra_u8, CR_SIZE_116);
        }

        if(p_ctx->filters.crc_check_list.cr_117 != 0u) {
            (void)amba_ik_system_memcpy(&p_misra_u8, &p_flow_tbl_list->p_CR_buf_117, sizeof(uint8 *));
            p_crc->sec18_cfg_buf[6] = amba_ik_system_crc32(p_misra_u8, CR_SIZE_117);
        }

        if(p_ctx->filters.crc_check_list.cr_118 != 0u) {
            (void)amba_ik_system_memcpy(&p_misra_u8, &p_flow_tbl_list->p_CR_buf_118, sizeof(uint8 *));
            p_crc->sec18_cfg_buf[7] = amba_ik_system_crc32(p_misra_u8, CR_SIZE_118);
        }

        if(p_ctx->filters.crc_check_list.cr_119 != 0u) {
            (void)amba_ik_system_memcpy(&p_misra_u8, &p_flow_tbl_list->p_CR_buf_119, sizeof(uint8 *));
            p_crc->sec18_cfg_buf[8] = amba_ik_system_crc32(p_misra_u8, CR_SIZE_119);
        }

        if(p_ctx->filters.crc_check_list.cr_120 != 0u) {
            (void)amba_ik_system_memcpy(&p_misra_u8, &p_flow_tbl_list->p_CR_buf_120, sizeof(uint8 *));
            p_crc->sec18_cfg_buf[9] = amba_ik_system_crc32(p_misra_u8, CR_SIZE_120);
        }

        if(p_ctx->filters.crc_check_list.cr_121 != 0u) {
            (void)amba_ik_system_memcpy(&p_misra_u8, &p_flow_tbl_list->p_CR_buf_121, sizeof(uint8 *));
            p_crc->sec18_cfg_buf[10] = amba_ik_system_crc32(p_misra_u8, CR_SIZE_121);
        }
    }

    return rval;
}

static inline uint32 exe_ik_calc_crc32_misra_1(const idsp_ik_flow_ctrl_t *p_flow_ctrl_crc)
{
    uint32 rval = IK_OK;
    // coverity fix
    if(p_flow_ctrl_crc->header.idsp_flow_addr == 0u) { /* coverity misra */ }
    if(p_flow_ctrl_crc->flow_info.update_sbp == 0u) { /* coverity misra */ }
    if(p_flow_ctrl_crc->flow_info.update_vignette == 0u) { /* coverity misra */ }
    if(p_flow_ctrl_crc->flow_info.sbp_map_addr == 0u) { /* coverity misra */ }
    if(p_flow_ctrl_crc->window.update_chroma_radius == 0u) { /* coverity misra */ }
    if(p_flow_ctrl_crc->calib.update_CA_warp == 0u) { /* coverity misra */ }
    if(p_flow_ctrl_crc->calib.update_lens_warp == 0u) { /* coverity misra */ }
    return rval;
}

static inline uint32 exe_ik_calc_crc32_misra_2(const idsp_ik_flow_ctrl_t *p_flow_ctrl_crc)
{
    uint32 rval = IK_OK;
    // coverity fix
    if(p_flow_ctrl_crc->calib.cawarp_horizontal_table_addr_red == 0u) { /* coverity misra */ }
    if(p_flow_ctrl_crc->calib.cawarp_horizontal_table_addr_blue == 0u) { /* coverity misra */ }
    if(p_flow_ctrl_crc->calib.cawarp_vertical_table_addr_red == 0u) { /* coverity misra */ }
    if(p_flow_ctrl_crc->calib.cawarp_vertical_table_addr_blue == 0u) { /* coverity misra */ }
    if(p_flow_ctrl_crc->calib.warp_horizontal_table_address == 0u) { /* coverity misra */ }
    if(p_flow_ctrl_crc->calib.warp_vertical_table_address == 0u) { /* coverity misra */ }
    if(p_flow_ctrl_crc->stitch.update_aaa_info == 0u) { /* coverity misra */ }
    if(p_flow_ctrl_crc->stitch.aaa_info_daddr == 0u) { /* coverity misra */ }
    return rval;
}

static uint32 exe_ik_calc_crc32(uint32 context_id, uint32 flow_idx)
{
    uint32 rval = IK_OK;
    uint32 i;
    uint32 j;
    amba_ik_context_entity_t *p_ctx;
    uintptr addr = 0;
    idsp_crc_data_t *p_crc = NULL;
    const idsp_ik_flow_ctrl_t *p_flow = NULL;
    const amba_ik_flow_tables_list_t *p_flow_tbl_list = NULL;
    const idsp_ik_aaa_data_t *p_aaa = NULL;
    const uint8 *p_misra_u8 = NULL;
    const uint8 *p_misra_u8_coverity = NULL;
    const uint32 *p_u32 = NULL;
#if IK_DBG_PROFILE_CRC
    uint32 start_tick = 0;
    uint32 end_tick = 0;
    uint32 start_sys_time = 0;
    uint32 end_sys_time = 0;

    extern UINT32 AmbaKAL_GetSysTickCount(UINT32 * pSysTickCount);
    extern UINT32 AmbaRTSL_GetOrcTimer(void);
#endif

    rval |= img_ctx_get_context(context_id, &p_ctx);

    if(rval == IK_OK) {
        rval |= img_ctx_get_crc_data(context_id, flow_idx, &addr);
        (void)amba_ik_system_memcpy(&p_crc, &addr, sizeof(void *));

        rval |= img_ctx_get_flow_control(context_id, flow_idx, &addr);
        (void)amba_ik_system_memcpy(&p_flow, &addr, sizeof(void *));

        rval |= img_ctx_get_flow_tbl_list(context_id, flow_idx, &addr);
        (void)amba_ik_system_memcpy(&p_flow_tbl_list, &addr, sizeof(void *));

#if IK_DBG_PROFILE_CRC
        (void)AmbaKAL_GetSysTickCount(&start_sys_time);
        start_tick = AmbaRTSL_GetOrcTimer();
#endif

        // flow_ctrl
        if(p_ctx->filters.crc_check_list.flow_ctrl != 0u) {
#if 0
            if(p_ctx->filters.input_param.safety_info.update_freq == 0u) { // change to 0 from 1, we are no longer check address and update flags of flow_ctrl crc
                (void)amba_ik_system_memcpy(&p_misra_u8_coverity, &p_flow, sizeof(uint8 *));
                p_crc->flow_ctrl = amba_ik_system_crc32(p_misra_u8_coverity, sizeof(idsp_ik_flow_ctrl_t));
            } else
#endif
            {
#if 1
                idsp_ik_flow_ctrl_t flow_ctrl_crc = {0};
                const idsp_ik_flow_ctrl_t *p_flow_ctrl_crc = NULL;

                p_flow_ctrl_crc = &flow_ctrl_crc;
                (void)amba_ik_system_memcpy(&p_flow_ctrl_crc, &p_flow_ctrl_crc, sizeof(idsp_ik_flow_ctrl_t *));//coverity fix
                (void)amba_ik_system_memcpy(&flow_ctrl_crc, p_flow, sizeof(idsp_ik_flow_ctrl_t));
                flow_ctrl_crc.header.idsp_flow_addr = 0u;
                flow_ctrl_crc.flow_info.update_sbp = 0u;
                flow_ctrl_crc.flow_info.update_vignette = 0u;
                flow_ctrl_crc.flow_info.sbp_map_addr = 0u;
                flow_ctrl_crc.window.update_chroma_radius = 0u;
                flow_ctrl_crc.calib.update_CA_warp = 0u;
                flow_ctrl_crc.calib.update_lens_warp = 0u;
                flow_ctrl_crc.calib.cawarp_horizontal_table_addr_red = 0u;
                flow_ctrl_crc.calib.cawarp_horizontal_table_addr_blue = 0u;
                flow_ctrl_crc.calib.cawarp_vertical_table_addr_red = 0u;
                flow_ctrl_crc.calib.cawarp_vertical_table_addr_blue = 0u;
                flow_ctrl_crc.calib.warp_horizontal_table_address = 0u;
                flow_ctrl_crc.calib.warp_vertical_table_address = 0u;
                flow_ctrl_crc.stitch.update_aaa_info = 0u;
                flow_ctrl_crc.stitch.aaa_info_daddr = 0u;
                for(i=0; i<SEC_2_CRS_COUNT; i++) {
                    flow_ctrl_crc.step_crs[0].sec2_crs[i].config_region_addr = 0u;
                }
                for(i=0; i<SEC_3_CRS_COUNT; i++) {
                    flow_ctrl_crc.step_crs[0].sec3_crs[i].config_region_addr = 0u;
                }
                for(i=0; i<SEC_4_CRS_COUNT; i++) {
                    flow_ctrl_crc.step_crs[0].sec4_crs[i].config_region_addr = 0u;
                }
                for(i=0; i<SEC_18_CRS_COUNT; i++) {
                    flow_ctrl_crc.step_crs[0].sec18_crs[i].config_region_addr = 0u;
                }

                (void)exe_ik_calc_crc32_misra_1(&flow_ctrl_crc);
                (void)exe_ik_calc_crc32_misra_2(&flow_ctrl_crc);
                (void)amba_ik_system_memcpy(&p_misra_u8_coverity, &p_flow_ctrl_crc, sizeof(uint8 *));
                p_crc->flow_ctrl = amba_ik_system_crc32(p_misra_u8_coverity, sizeof(idsp_ik_flow_ctrl_t));
#else
                idsp_ik_flow_info_t flow_info;
                idsp_ik_window_info_t win_info;
                idsp_ik_calib_data_t calib;
                const idsp_ik_flow_header_t *p_header = &p_flow->header;
                const idsp_ik_flow_info_t *p_flow_info = &flow_info;
                const idsp_ik_window_info_t *p_window = &win_info;
                const idsp_ik_phase_info_t *p_phase = &p_flow->phase;
                const idsp_ik_calib_data_t *p_calib = &calib;
                const idsp_ik_stitch_data_t *p_stitch = &p_flow->stitch;
                const idsp_ik_step_info_t *p_step_info = &p_flow->step_info;
                uint32 tmp;
                uintptr misra_uintptr;

                (void)amba_ik_system_memcpy(&p_misra_u8, &p_header, sizeof(uint8 *));
                flow_ctrl_crc[0] = amba_ik_system_crc32(p_misra_u8, sizeof(idsp_ik_flow_header_t) - sizeof(uint32));

                // workaround for sbp and vig update flags
                (void)amba_ik_system_memcpy(&flow_info, &p_flow->flow_info, sizeof(idsp_ik_flow_info_t));
                (void)amba_ik_system_memcpy(&p_misra_u8, &p_flow_info, sizeof(uint8 *));
                flow_ctrl_crc[1] = amba_ik_system_crc32(p_misra_u8, sizeof(uint32));

                //avoid the update flags
                (void)amba_ik_system_memcpy(&misra_uintptr, &p_misra_u8, sizeof(uintptr));
                misra_uintptr += sizeof(uint32);
                (void)amba_ik_system_memcpy(&p_misra_u8, &misra_uintptr, sizeof(uint8 *));
                (void)amba_ik_system_memcpy(&tmp, p_misra_u8, sizeof(uint32));
                tmp &= 0xFFFFF3FFUL;
                p_u32 = &tmp;
                (void)amba_ik_system_memcpy(&p_misra_u8, &p_u32, sizeof(uint8 *));
                flow_ctrl_crc[2] = amba_ik_system_crc32(p_misra_u8, sizeof(uint32));

                (void)amba_ik_system_memcpy(&misra_uintptr, &p_flow_info, sizeof(uintptr));
                misra_uintptr += (sizeof(uint32) * 2u);
                (void)amba_ik_system_memcpy(&p_misra_u8, &misra_uintptr, sizeof(uint8 *));
                flow_ctrl_crc[3] = amba_ik_system_crc32(p_misra_u8, sizeof(idsp_ik_flow_info_t) - (sizeof(uint32)*3u));

                // workaround for radius update flag
                (void)amba_ik_system_memcpy(&win_info, &p_flow->window, sizeof(idsp_ik_window_info_t));
                (void)amba_ik_system_memcpy(&p_misra_u8, &p_window, sizeof(uint8 *));
                (void)amba_ik_system_memcpy(&tmp, p_misra_u8, sizeof(uint32));
                tmp &= 0xFFFFFFFBUL;
                p_u32 = &tmp;
                (void)amba_ik_system_memcpy(&p_misra_u8, &p_u32, sizeof(uint8 *));
                flow_ctrl_crc[4] = amba_ik_system_crc32(p_misra_u8, sizeof(uint32));

                (void)amba_ik_system_memcpy(&misra_uintptr, &p_window, sizeof(uintptr));
                misra_uintptr += sizeof(uint32);
                (void)amba_ik_system_memcpy(&p_misra_u8, &misra_uintptr, sizeof(uint8 *));
                flow_ctrl_crc[5] = amba_ik_system_crc32(p_misra_u8, sizeof(idsp_ik_window_info_t) - sizeof(uint32));

                (void)amba_ik_system_memcpy(&p_misra_u8, &p_phase, sizeof(uint8 *));
                flow_ctrl_crc[6] = amba_ik_system_crc32(p_misra_u8, sizeof(idsp_ik_phase_info_t));

                // workaround for CA and Warp update flags
                (void)amba_ik_system_memcpy(&calib, &p_flow->calib, sizeof(idsp_ik_calib_data_t));
                (void)amba_ik_system_memcpy(&p_misra_u8, &p_calib, sizeof(uint8 *));
                (void)amba_ik_system_memcpy(&tmp, p_misra_u8, sizeof(uint32));
                tmp &= 0xFFFFFFF3UL;
                p_u32 = &tmp;
                (void)amba_ik_system_memcpy(&p_misra_u8, &p_u32, sizeof(uint8 *));
                flow_ctrl_crc[7] = amba_ik_system_crc32(p_misra_u8, sizeof(uint32));

                (void)amba_ik_system_memcpy(&misra_uintptr, &p_calib, sizeof(uintptr));
                misra_uintptr += sizeof(uint32);
                (void)amba_ik_system_memcpy(&p_misra_u8, &misra_uintptr, sizeof(uint8 *));
                flow_ctrl_crc[8] = amba_ik_system_crc32(p_misra_u8, sizeof(idsp_ik_calib_data_t) - (sizeof(uint32)*7u));

                (void)amba_ik_system_memcpy(&p_misra_u8, &p_stitch, sizeof(uint8 *));
                flow_ctrl_crc[9] = amba_ik_system_crc32(p_misra_u8, sizeof(idsp_ik_stitch_data_t) - sizeof(uint32));

                (void)amba_ik_system_memcpy(&p_misra_u8, &p_step_info, sizeof(uint8 *));
                flow_ctrl_crc[10] = amba_ik_system_crc32(p_misra_u8, sizeof(idsp_ik_step_info_t));

                p_u32 = flow_ctrl_crc;
                (void)amba_ik_system_memcpy(&p_misra_u8, &p_u32, sizeof(uint8 *));
                p_crc->flow_ctrl = amba_ik_system_crc32(p_misra_u8, sizeof(uint32)*11u);
#endif
            }
        }

        // CR
        rval |= exe_ik_calc_crc32_hdr(context_id, flow_idx);
        rval |= exe_ik_calc_crc32_cfa(context_id, flow_idx);
        rval |= exe_ik_calc_crc32_rgb(context_id, flow_idx);
        rval |= exe_ik_calc_crc32_yuv(context_id, flow_idx);
        rval |= exe_ik_calc_crc32_sec3(context_id, flow_idx);
        rval |= exe_ik_calc_crc32_sec4(context_id, flow_idx);
        rval |= exe_ik_calc_crc32_sec18(context_id, flow_idx);

        // aaa
        if(p_ctx->filters.crc_check_list.aaa_data != 0u) {
            (void)amba_ik_system_memcpy(&p_aaa, &p_flow_tbl_list->p_aaa, sizeof(idsp_ik_aaa_data_t *));
            for(i=0; i<(uint32)p_ctx->filters.input_param.stitching_info.tile_num_y; i++) {
                for(j=0; j<(uint32)p_ctx->filters.input_param.stitching_info.tile_num_x; j++) {
                    (void)amba_ik_system_memcpy(&p_misra_u8, &p_aaa, sizeof(uint8 *));
                    p_crc->aaa[i][j] = amba_ik_system_crc32(p_misra_u8, sizeof(idsp_ik_aaa_data_t));
                    ++p_aaa;
                }
            }
        }

        // ca
        if((p_ctx->filters.input_param.stitching_info.enable == 1) && (p_ctx->filters.crc_check_list.cawarp != 0u)) {
            (void)amba_ik_system_memcpy(&p_misra_u8, &p_flow_tbl_list->p_ca_warp_hor_red, sizeof(uint8 *));
            p_crc->ca_warp_hor_red = amba_ik_system_crc32(p_misra_u8, sizeof(int16) * CAWARP_VIDEO_MAXHORGRID * CAWARP_VIDEO_MAXVERGRID);
            (void)amba_ik_system_memcpy(&p_misra_u8, &p_flow_tbl_list->p_ca_warp_hor_blue, sizeof(uint8 *));
            p_crc->ca_warp_hor_blue = amba_ik_system_crc32(p_misra_u8, sizeof(int16) * CAWARP_VIDEO_MAXHORGRID * CAWARP_VIDEO_MAXVERGRID);
            (void)amba_ik_system_memcpy(&p_misra_u8, &p_flow_tbl_list->p_ca_warp_ver_red, sizeof(uint8 *));
            p_crc->ca_warp_ver_red = amba_ik_system_crc32(p_misra_u8, sizeof(int16) * CAWARP_VIDEO_MAXHORGRID * CAWARP_VIDEO_MAXVERGRID);
            (void)amba_ik_system_memcpy(&p_misra_u8, &p_flow_tbl_list->p_ca_warp_ver_blue, sizeof(uint8 *));
            p_crc->ca_warp_ver_blue = amba_ik_system_crc32(p_misra_u8, sizeof(int16) * CAWARP_VIDEO_MAXHORGRID * CAWARP_VIDEO_MAXVERGRID);

            p_crc->sec2_cfg_buf[13] = 0u;
            p_crc->sec2_cfg_buf[14] = 0u;
            p_crc->sec2_cfg_buf[15] = 0u;
            p_crc->sec2_cfg_buf[16] = 0u;
        } else {
            p_crc->ca_warp_hor_red = 0u;
            p_crc->ca_warp_hor_blue = 0u;
            p_crc->ca_warp_ver_red = 0u;
            p_crc->ca_warp_ver_blue = 0u;
        }

        // warp
        if((p_ctx->filters.input_param.stitching_info.enable == 1) && (p_ctx->filters.crc_check_list.warp != 0u)) {
            (void)amba_ik_system_memcpy(&p_misra_u8, &p_flow_tbl_list->p_warp_hor, sizeof(uint8 *));
            p_crc->warp_hor = amba_ik_system_crc32(p_misra_u8, sizeof(int16) * WARP_VIDEO_MAXHORGRID * WARP_VIDEO_MAXVERGRID * 2U);
            (void)amba_ik_system_memcpy(&p_misra_u8, &p_flow_tbl_list->p_warp_ver, sizeof(uint8 *));
            p_crc->warp_ver = amba_ik_system_crc32(p_misra_u8, sizeof(int16) * WARP_VIDEO_MAXHORGRID * WARP_VIDEO_MAXVERGRID * 2U);

            p_crc->sec2_cfg_buf[30] = 0u;
            p_crc->sec3_cfg_buf[1] = 0u;
        } else {
            p_crc->warp_hor = 0u;
            p_crc->warp_ver = 0u;
        }

        p_u32 = &p_crc->flow_ctrl;
        (void)amba_ik_system_memcpy(&p_u32, &p_u32, sizeof(uint32 *));//coverity fix
        (void)amba_ik_system_memcpy(&p_misra_u8, &p_u32, sizeof(uint8 *));
        p_crc->frame_crc32 = amba_ik_system_crc32(p_misra_u8, sizeof(idsp_crc_data_t) - sizeof(uint32));

#if IK_DBG_PROFILE_CRC
        end_tick = AmbaRTSL_GetOrcTimer();
        (void)AmbaKAL_GetSysTickCount(&end_sys_time);
        amba_ik_system_print_uint32_5("[IK][Profile exe_ik_calc_crc32()] flow_idx %d, sys_time = %d ms, tick = %d", flow_idx, (end_sys_time - start_sys_time), (end_tick - start_tick), DC_U, DC_U);
#endif

    } else {
        amba_ik_system_print_str_5("[IK] can't get context", NULL, NULL, NULL, NULL, NULL);
    }

#if 0 // crc32 log
    amba_ik_system_print_uint32_5("[IK][CRC] (ctx_id %d) (index %d) flow_ctrl CRC 0x%8x, addr 0x%8x, upd_interval %d!", context_id, flow_idx, p_crc->flow_ctrl, (uintptr)p_flow, p_ctx->filters.input_param.safety_info.update_freq);
    for(i=0; i<SEC_2_CRS_COUNT; i++) {
        amba_ik_system_print_uint32_5("[IK][CRC] (index %d) sec2_cfg_buf[%d] 0x%8x !", flow_idx, i, p_crc->sec2_cfg_buf[i], DC_U, DC_U);
    }
    for(i=0; i<SEC_3_CRS_COUNT; i++) {
        amba_ik_system_print_uint32_5("[IK][CRC] (index %d) sec3_cfg_buf[%d] 0x%8x !", flow_idx, i, p_crc->sec3_cfg_buf[i], DC_U, DC_U);
    }
    for(i=0; i<SEC_4_CRS_COUNT; i++) {
        amba_ik_system_print_uint32_5("[IK][CRC] (index %d) sec4_cfg_buf[%d] 0x%8x !", flow_idx, i, p_crc->sec4_cfg_buf[i], DC_U, DC_U);
    }
    for(i=0; i<SEC_18_CRS_COUNT; i++) {
        amba_ik_system_print_uint32_5("[IK][CRC] (index %d) sec18_cfg_buf[%d] 0x%8x !", flow_idx, i, p_crc->sec18_cfg_buf[i], DC_U, DC_U);
    }
    for(i=0; i<IK_MAX_TILE_NUM_Y; i++) {
        for(j=0; j<IK_MAX_TILE_NUM_X; j++) {
            amba_ik_system_print_uint32_5("[IK][CRC] (index %d) aaa[%d][%d] 0x%8x !", flow_idx, i, j, p_crc->aaa[i][j], DC_U);
        }
    }
    amba_ik_system_print_uint32_5("[IK][CRC] (index %d) ca_warp_hor_red 0x%8x !", flow_idx, p_crc->ca_warp_hor_red, DC_U, DC_U, DC_U);
    amba_ik_system_print_uint32_5("[IK][CRC] (index %d) ca_warp_hor_blue 0x%8x !", flow_idx, p_crc->ca_warp_hor_blue, DC_U, DC_U, DC_U);
    amba_ik_system_print_uint32_5("[IK][CRC] (index %d) ca_warp_ver_red 0x%8x !", flow_idx, p_crc->ca_warp_ver_red, DC_U, DC_U, DC_U);
    amba_ik_system_print_uint32_5("[IK][CRC] (index %d) ca_warp_ver_blue 0x%8x !", flow_idx, p_crc->ca_warp_ver_blue, DC_U, DC_U, DC_U);
    amba_ik_system_print_uint32_5("[IK][CRC] (index %d) warp_hor 0x%8x !", flow_idx, p_crc->warp_hor, DC_U, DC_U, DC_U);
    amba_ik_system_print_uint32_5("[IK][CRC] (index %d) warp_ver 0x%8x !", flow_idx, p_crc->warp_ver, DC_U, DC_U, DC_U);
#endif

    return rval;
}
#endif

static uint32 exe_set_frame_info(uint32 context_id, uint32 flow_idx)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx;
    uintptr addr = 0;
    const amba_ik_flow_tables_list_t *p_flow_tbl_list = NULL;
    ik_query_frame_info_t *p_frame_info = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if(rval == IK_OK) {
        rval |= img_ctx_get_flow_tbl_list(context_id, flow_idx, &addr);
        (void)amba_ik_system_memcpy(&p_flow_tbl_list, &addr, sizeof(void *));
        (void)amba_ik_system_memcpy(&p_frame_info, &p_flow_tbl_list->p_frame_info, sizeof(ik_query_frame_info_t *));

        // hdr raw offset
        (void)amba_ik_system_memcpy(&p_frame_info->hdr_raw_info, &p_ctx->filters.input_param.hdr_raw_info, sizeof(ik_hdr_raw_info_t));

    } else {
        amba_ik_system_print_str_5("[IK] can't get context", NULL, NULL, NULL, NULL, NULL);
    }

    return rval;
}

static uint32 img_exe_execute_invalid_cr(uint32 context_id)
{
    uint32 rval = IK_ERR_0000;

    amba_ik_system_print_uint32_5("[IK][ERROR] img_exe_execute_invalid_cr, context_id %d", context_id, DC_U, DC_U, DC_U, DC_U);

    return rval;
}

static uint32 img_exe_execute_safety_cr(uint32 context_id)
{
    uint32 rval = IK_ERR_0200;

    amba_ik_system_print_uint32_5("[IK][ERROR] img_exe_execute_safety_cr(%d) Detect Safety ERROR... ", context_id, DC_U, DC_U, DC_U, DC_U);

    return rval;
}

static uint32 img_exe_execute_update_cr(uint32 context_id)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx;
    uintptr addr = 0;
    idsp_ik_flow_ctrl_t *p_flow = NULL;
    const idsp_ik_flow_ctrl_t *p_flow_previous = NULL;
    amba_ik_flow_tables_list_t *p_flow_tbl_list = NULL;
    amba_ik_exe_cr_utilities_t utilities = {NULL};
    uint32 flow_idx = 0, cr_ring_num = 0;
    uint32 pre_flow_idx = 0u;
    uint32 src_flow_idx;
    ik_buffer_info_t *p_ik_buffer_info = NULL;
    uint32 depth;
    uint32 i;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    rval |= img_arch_get_ik_working_buffer(&p_ik_buffer_info);
    if(rval == IK_OK) {
        if(p_ik_buffer_info->safety_state != IK_SAFETY_STATE_ERROR) {
            if(p_ctx->organization.initial_flag != 0U) {
                cr_ring_num = p_ctx->organization.attribute.cr_ring_number;
                rval |= exe_get_next_flow_control_index(cr_ring_num, &p_ctx->organization.active_cr_state, &flow_idx);
                rval |= img_ctx_get_flow_control(context_id, flow_idx, &addr);
                if(rval == IK_OK) {
                    (void)amba_ik_system_memcpy(&p_flow, &addr, sizeof(void *));
                }
                rval |= img_ctx_get_flow_tbl_list(context_id, flow_idx, &addr);
                if(rval == IK_OK) {
                    (void)amba_ik_system_memcpy(&p_flow_tbl_list, &addr, sizeof(void *));
                }

                if (rval == IK_OK) {
#if SUPPORT_CR_MEMORY_FENCE
                    rval |= exe_ik_check_ctx_mem_fence(context_id);
#endif
                    if(rval == IK_OK) {
                        src_flow_idx = p_ctx->organization.active_cr_state.active_flow_idx;
                        rval |= exe_clone_flow_control_and_tbl_list(context_id, p_flow, p_flow_tbl_list, src_flow_idx);
                        rval |= exe_get_next_cr_index(context_id, cr_ring_num, &p_ctx->filters.update_flags, &p_ctx->organization.active_cr_state.cr_index);
                        rval |= exe_check_next_cr_index(context_id, &p_ctx->filters.update_flags.cr, &p_ctx->organization.active_cr_state.cr_index);
                        rval |= exe_update_flow_tbl_list(context_id, flow_idx, &p_ctx->organization.active_cr_state.cr_index, &p_ctx->filters.update_flags.cr);

                        rval |= exe_get_cr_utilities(&p_ctx->organization.attribute.ability, &utilities);
                        rval |= utilities.compute_cr(p_flow, p_flow_tbl_list, &p_ctx->filters);

                        // prepare idsp_ik_flow_ctrl_t
                        rval |= exe_set_flow_header(context_id, p_flow);
                        rval |= exe_clean_flow_ctrl_cache(p_flow, p_flow_tbl_list, &p_ctx->filters.update_flags.cr);
                        p_ctx->organization.active_cr_state.active_flow_idx = flow_idx;
                        //ring buffer error injection.
                        if(p_ctx->filters.input_param.ring_buffer_error_injection != 0U) {
                            exe_inject_safety_error_ring_buffer(p_flow);
                        }
                        //ring buffer checker.
                        rval |= img_ctx_get_flow_control(context_id, src_flow_idx, &addr);
                        (void)amba_ik_system_memcpy(&p_flow_previous, &addr, sizeof(void *));
                        rval |= exe_flow_ctrl_ring_check(cr_ring_num, p_flow, p_flow_previous, &p_ctx->filters.update_flags.cr);

#if SUPPORT_CR_MEMORY_FENCE
                        depth = p_ctx->organization.attribute.cr_ring_number;
                        depth = uint32_min_ik(depth, 3u);
                        pre_flow_idx = flow_idx;
                        for(i=0u; i<depth; i++) {
                            rval |= exe_ik_check_cr_mem_fence(context_id, pre_flow_idx);
                            rval |= exe_get_previous_flow_control_index(&p_ctx->organization, flow_idx, i+1u, &pre_flow_idx);
                        }
                        rval |= exe_ik_check_flow_tbl_memory_fence(context_id);
#endif
                        if(rval == IK_OK) {
                            rval |= img_ctx_reset_update_flags(context_id);

                            if(p_ctx->filters.input_param.crc_mismatch_error_injection != 0U) {
                                rval |= img_exe_inject_safety_crc_mismatch_error(p_flow_tbl_list);
                            }

                            //crc
                            if(p_ik_buffer_info->safety_crc_enable == 1u) {
#ifndef EARLYTEST_ENV
                                if((p_ctx->filters.input_param.safety_info.update_freq == 1u) ||
                                   ((((uint32)p_ctx->organization.active_cr_state.cr_running_number - 1u) % p_ctx->filters.input_param.safety_info.update_freq) == 0u)) {
                                    rval |= exe_ik_calc_crc32(context_id, flow_idx);
                                }
#endif
                            }
                            rval |= exe_set_frame_info(context_id, flow_idx);
                        } else {
                            p_ik_buffer_info->safety_state = IK_SAFETY_STATE_ERROR;
                            p_ctx->organization.executer_method.execute_cr = img_exe_execute_safety_cr;
                            img_ctx_hook_safety_filter_method(&p_ctx->organization.filter_methods);
                        }
                    } else {
                        p_ik_buffer_info->safety_state = IK_SAFETY_STATE_ERROR;
                        p_ctx->organization.executer_method.execute_cr = img_exe_execute_safety_cr;
                        img_ctx_hook_safety_filter_method(&p_ctx->organization.filter_methods);
                    }
                } else {
                    amba_ik_system_print_uint32_5("[IK] call img_ctx_get_flow_control(%d) fail", context_id, DC_U, DC_U, DC_U, DC_U);
                }
            } else {
                amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial", context_id, DC_U, DC_U, DC_U, DC_U);
                rval = IK_ERR_0002;
            }
        } else {
            p_ik_buffer_info->safety_state = IK_SAFETY_STATE_ERROR;
            p_ctx->organization.executer_method.execute_cr = img_exe_execute_safety_cr;
            img_ctx_hook_safety_filter_method(&p_ctx->organization.filter_methods);
            rval |= IK_ERR_0200;
        }
    } else {
        amba_ik_system_print_str_5("[IK] can't get context", NULL, NULL, NULL, NULL, NULL);
    }

    return rval;
}

static inline uint32 filter_update_check_asf_fstshp(const amba_ik_filter_update_flags_t *p_update_flag,uint32 use_sharpen_not_asf)
{
    uint32 rval = IK_OK;

    if (p_update_flag->iso.first_luma_processing_mode_updated != 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] first_luma_processing_mode_updated = 0. Please check ik_set_fst_luma_process_mode() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    } else {
        if(use_sharpen_not_asf == 0U) {
            if (p_update_flag->iso.advance_spatial_filter_updated != 1U) {
                rval |= IK_ERR_0100;
                amba_ik_system_print_str_5("[IK][ERROR] advance_spatial_filter_updated = 0. Please check ik_set_adv_spatial_filter() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
            }
        } else {
            if (p_update_flag->iso.fstshpns_both_updated != 1U) {
                rval |= IK_ERR_0100;
                amba_ik_system_print_str_5("[IK][ERROR] fstshpns_both_updated = 0. Please check ik_set_fst_shp_both() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
            }
            if (p_update_flag->iso.fstshpns_noise_updated != 1U) {
                rval |= IK_ERR_0100;
                amba_ik_system_print_str_5("[IK][ERROR] fstshpns_noise_updated = 0. Please check ik_set_fst_shp_noise() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
            }
            if (p_update_flag->iso.fstshpns_fir_updated != 1U) {
                rval |= IK_ERR_0100;
                amba_ik_system_print_str_5("[IK][ERROR] fstshpns_fir_updated = 0. Please check ik_set_fst_shp_fir() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
            }
            if (p_update_flag->iso.fstshpns_coring_updated != 1U) {
                rval |= IK_ERR_0100;
                amba_ik_system_print_str_5("[IK][ERROR] fstshpns_coring_updated = 0. Please check ik_set_fst_shp_coring() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
            }
            if (p_update_flag->iso.fstshpns_coring_index_scale_updated != 1U) {
                rval |= IK_ERR_0100;
                amba_ik_system_print_str_5("[IK][ERROR] fstshpns_coring_index_scale_updated = 0. Please check ik_set_fst_shp_coring_idx_scale() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
            }
            if (p_update_flag->iso.fstshpns_min_coring_result_updated != 1U) {
                rval |= IK_ERR_0100;
                amba_ik_system_print_str_5("[IK][ERROR] fstshpns_min_coring_result_updated = 0. Please check ik_set_fst_shp_min_coring_rslt() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
            }
            if (p_update_flag->iso.fstshpns_max_coring_result_updated != 1U) {
                rval |= IK_ERR_0100;
                amba_ik_system_print_str_5("[IK][ERROR] fstshpns_max_coring_result_updated = 0. Please check ik_set_fst_shp_max_coring_rslt() API have been updated.\n", NULL, NULL, NULL, NULL, NULL);
            }
            if (p_update_flag->iso.fstshpns_scale_coring_updated != 1U) {
                rval |= IK_ERR_0100;
                amba_ik_system_print_str_5("[IK][ERROR] fstshpns_scale_coring_updated = 0. Please check ik_set_fst_shp_scale_coring() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
            }
        }
    }

    return rval;
}

static inline uint32 filter_update_check_general(const amba_ik_filter_update_flags_t *p_update_flag)
{
    uint32 rval = IK_OK;

    if (p_update_flag->iso.stitching_info_updated != 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] stitching_info_updated = 0. Please check ik_set_stitching_info() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    if (p_update_flag->iso.warp_buf_info_updated != 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] warp_buf_info_updated = 0. Please check ik_set_warp_buffer_info() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    if (p_update_flag->iso.chroma_scale_updated != 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] chroma_scale_updated = 0. Please check ik_set_chroma_scale() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    if (p_update_flag->iso.chroma_filter_updated != 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] chroma_filter_updated = 0. Please check ik_set_chroma_filter() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    if (p_update_flag->iso.wide_chroma_filter != 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] wide_chroma_filter_updated = 0. Please check ik_set_wide_chroma_filter() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    if (p_update_flag->iso.wide_chroma_filter_combine != 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] wide_chroma_filter_combine_updated = 0. Please check ik_set_wide_chroma_filter_combine() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    if (p_update_flag->iso.chroma_median_filter_updated != 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] chroma_median_filter_updated = 0. Please check ik_set_chroma_median_filter() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }

    return rval;
}

static inline uint32 filter_update_check_mctf_shpb(const amba_ik_filter_update_flags_t *p_update_flag)
{
    uint32 rval = IK_OK;

    if (p_update_flag->iso.video_mctf_updated != 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] video_mctf_updated = 0. Please check ik_set_video_mctf() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    /*if (p_update_flag->iso.video_mctf_and_final_sharpen_updated != 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] video_mctf_and_final_sharpen_updated = 0. Please check ik_set_video_mctf_and_fnl_shp() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }*/
    if (p_update_flag->iso.video_mctf_ta_updated != 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] video_mctf_ta_updated = 0. Please check ik_set_video_mctf_ta() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    if (p_update_flag->iso.fnlshpns_both_updated != 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] fnlshpns_both_updated = 0. Please check ik_set_fnl_shp_both() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    if (p_update_flag->iso.fnlshpns_noise_updated != 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] fnlshpns_noise_updated = 0. Please check ik_set_fnl_shp_noise() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    if (p_update_flag->iso.fnlshpns_fir_updated != 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] fnlshpns_fir_updated = 0. Please check ik_set_fnl_shp_fir() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    if (p_update_flag->iso.fnlshpns_coring_updated != 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] fnlshpns_coring_updated = 0. Please check ik_set_fnl_shp_coring() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    if (p_update_flag->iso.fnlshpns_coring_index_scale_updated != 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] fnlshpns_coring_index_scale_updated = 0. Please check ik_set_fnl_shp_coring_idx_scale() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    if (p_update_flag->iso.fnlshpns_min_coring_result_updated != 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] fnlshpns_min_coring_result_updated = 0. Please check ik_set_fnl_shp_min_coring_rslt() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    if (p_update_flag->iso.fnlshpns_max_coring_result_updated != 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] fnlshpns_max_coring_result_updated = 0. Please check ik_set_fnl_shp_max_coring_rslt() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    if (p_update_flag->iso.fnlshpns_scale_coring_updated != 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] fnlshpns_scale_coring_updated = 0. Please check ik_set_fnl_shp_scale_coring() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }

    return rval;
}

static inline uint32 filter_update_check_non_y2y_p1(const amba_ik_filter_update_flags_t *p_update_flag)
{
    uint32 rval = IK_OK;

    if (p_update_flag->iso.sensor_information_updated != 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] sensor_information_updated = 0. Please check ik_set_vin_sensor_info() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    if (p_update_flag->iso.exp0_fe_static_blc_updated != 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] exp0_fe_static_blc_updated = 0. Please check ik_set_exp0_frontend_static_blc() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    if (p_update_flag->iso.exp0_fe_wb_gain_updated != 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] exp0_fe_wb_gain_updated = 0. Please check ik_set_exp0_frontend_wb_gain() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    /*if (p_update_flag->iso.rgb_to_12y_updated != 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] rgb_to_12y_updated = 0. Please check ik_set_rgb_to_12y() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }*/
    if (p_update_flag->iso.dynamic_bpc_updated != 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] dynamic_bpc_updated = 0. Please check ik_set_dynamic_bad_pixel_corr() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    if (p_update_flag->iso.cfa_leakage_filter_updated != 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] cfa_leakage_filter_updated = 0. Please check ik_set_cfa_leakage_filter() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    if (p_update_flag->iso.anti_aliasing_updated != 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] anti_aliasing_updated = 0. Please check ik_set_anti_aliasing() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    if (p_update_flag->iso.grgb_mismatch_updated != 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] grgb_mismatch_updated = 0. Please check ik_set_grgb_mismatch() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    if (p_update_flag->iso.before_ce_wb_gain_updated != 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] before_ce_wb_gain_updated = 0. Please check ik_set_before_ce_wb_gain() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }

    return rval;
}

static inline uint32 filter_update_check_non_y2y_p2(const amba_ik_filter_update_flags_t *p_update_flag)
{
    uint32 rval = IK_OK;

    if (p_update_flag->iso.after_ce_wb_gain_updated != 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] after_ce_wb_gain_updated = 0. Please check ik_set_after_ce_wb_gain() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    if (p_update_flag->iso.cfa_noise_filter_updated != 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] cfa_noise_filter_updated = 0. Please check ik_set_cfa_noise_filter() API have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    if (p_update_flag->iso.demosaic_updated != 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] demosaic_updated = 0. Please check ik_set_demosaic() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    if (p_update_flag->iso.color_correction_updated != 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] color_correction_updated = 0. Please check ik_set_color_correction()I have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    if (p_update_flag->iso.tone_curve_updated != 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] tone_curve_updated = 0. Please check ik_set_tone_curve() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    if (p_update_flag->iso.rgb_to_yuv_matrix_updated != 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] rgb_to_yuv_matrix_updated = 0. Please check ik_set_rgb_to_yuv_matrix() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    if (p_update_flag->iso.lnl_updated != 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] lnl_updated = 0. Please check ik_set_luma_noise_reduction() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }

    return rval;
}

static inline uint32 filter_update_check_non_linear(const ik_ability_t *p_ability, const amba_ik_filter_update_flags_t *p_update_flag)
{
    uint32 rval = IK_OK;
    uint32 video_pipe = p_ability->video_pipe;

    if ((equal_op_u32(video_pipe, AMBA_IK_VIDEO_HDR_EXPO_2)+ \
         equal_op_u32(video_pipe, AMBA_IK_VIDEO_HDR_EXPO_3)) != 0U) {
        if (p_update_flag->iso.exp1_fe_static_blc_updated != 1U) {
            rval |= IK_ERR_0100;
            amba_ik_system_print_str_5("[IK][ERROR] exp1_fe_static_blc_updated = 0. Please check ik_set_exp1_frontend_static_blc() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
        }
        if (p_update_flag->iso.exp1_fe_wb_gain_updated != 1U) {
            rval |= IK_ERR_0100;
            amba_ik_system_print_str_5("[IK][ERROR] exp1_fe_wb_gain_updated = 0. Please check ik_set_exp1_frontend_wb_gain() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
        }
        if (video_pipe == AMBA_IK_VIDEO_HDR_EXPO_3) {
            if (p_update_flag->iso.exp2_fe_static_blc_updated != 1U) {
                rval |= IK_ERR_0100;
                amba_ik_system_print_str_5("[IK][ERROR] exp2_fe_static_blc_updated = 0. Please check ik_set_exp2_frontend_static_blc() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
            }
            if (p_update_flag->iso.exp2_fe_wb_gain_updated != 1U) {
                rval |= IK_ERR_0100;
                amba_ik_system_print_str_5("[IK][ERROR] exp2_fe_wb_gain_updated = 0. Please check ik_set_exp2_frontend_wb_gain() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
            }
        }
        if (p_update_flag->iso.hdr_blend_updated != 1U) {
            rval |= IK_ERR_0100;
            amba_ik_system_print_str_5("[IK][ERROR] hdr_blend_updated = 0. Please check ik_set_hdr_blend() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
        }
    }

    if ((equal_op_u32(video_pipe, AMBA_IK_VIDEO_LINEAR_CE)+ \
         equal_op_u32(video_pipe, AMBA_IK_VIDEO_HDR_EXPO_2)+ \
         equal_op_u32(video_pipe, AMBA_IK_VIDEO_HDR_EXPO_3)) != 0U) {
        if (p_update_flag->iso.ce_updated != 1U) {
            rval |= IK_ERR_0100;
            amba_ik_system_print_str_5("[IK][ERROR] ce_updated = 0. Please check ik_set_ce() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
        }
        if (p_update_flag->iso.ce_input_table_updated != 1U) {
            rval |= IK_ERR_0100;
            amba_ik_system_print_str_5("[IK][ERROR] ce_input_table_updated = 0. Please check ik_set_ce_input_table() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
        }
        if (p_update_flag->iso.ce_out_table_updated != 1U) {
            rval |= IK_ERR_0100;
            amba_ik_system_print_str_5("[IK][ERROR] before_ce_wb_gain_updated = 0. Please check ik_set_ce_out_table() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
        }
    }

    return rval;
}

static uint32 INLINE exe_execute_filter_update_check(const ik_ability_t *p_ability, const amba_ik_filter_update_flags_t *p_update_flag,uint32 use_sharpen_not_asf)
{
    uint32 rval = IK_OK;
    uint32 video_pipe = p_ability->video_pipe;

    rval |= filter_update_check_asf_fstshp(p_update_flag, use_sharpen_not_asf);
    rval |= filter_update_check_general(p_update_flag);
    rval |= filter_update_check_mctf_shpb(p_update_flag);
    if (video_pipe != AMBA_IK_VIDEO_Y2Y) {
        rval |= filter_update_check_non_y2y_p1(p_update_flag);
        rval |= filter_update_check_non_y2y_p2(p_update_flag);
    }
    if ((equal_op_u32(video_pipe, AMBA_IK_VIDEO_LINEAR_CE)+ \
         equal_op_u32(video_pipe, AMBA_IK_VIDEO_HDR_EXPO_2)+ \
         equal_op_u32(video_pipe, AMBA_IK_VIDEO_HDR_EXPO_3)) != 0U) {
        rval |= filter_update_check_non_linear(p_ability, p_update_flag);
    }

    return rval;
}

static uint32 img_exe_execute_first_cr(uint32 context_id)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx;
    uintptr addr = 0;
    idsp_ik_flow_ctrl_t *p_flow = NULL;
    amba_ik_flow_tables_list_t *p_flow_tbl_list = NULL;
    amba_ik_exe_cr_utilities_t utilities = {NULL};
    ik_buffer_info_t *p_ik_buffer_info = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    rval |= img_arch_get_ik_working_buffer(&p_ik_buffer_info);
    if(rval == IK_OK) {
        if(p_ik_buffer_info->safety_state != IK_SAFETY_STATE_ERROR) {
            if(p_ctx->organization.initial_flag != 0U) {
                rval |= img_ctx_get_flow_control(context_id, 0, &addr);
                if(rval == IK_OK) {
                    (void)amba_ik_system_memcpy(&p_flow, &addr, sizeof(void *));
                }
                rval |= img_ctx_get_flow_tbl_list(context_id, 0, &addr);
                if(rval == IK_OK) {
                    (void)amba_ik_system_memcpy(&p_flow_tbl_list, &addr, sizeof(void *));
                }

                if (rval == IK_OK) {
                    rval |= exe_execute_filter_update_check(&p_ctx->organization.attribute.ability, &p_ctx->filters.update_flags, p_ctx->filters.input_param.first_luma_process_mode.use_sharpen_not_asf);
                    if (rval == IK_OK) {
#if SUPPORT_CR_MEMORY_FENCE
                        rval |= exe_ik_check_ctx_mem_fence(context_id);
#endif
                        if(rval == IK_OK) {
                            p_ctx->organization.active_cr_state.active_flow_idx = 0;
                            rval |= exe_check_next_cr_index(context_id, &p_ctx->filters.update_flags.cr, &p_ctx->organization.active_cr_state.cr_index);
                            rval |= exe_get_cr_utilities(&p_ctx->organization.attribute.ability, &utilities);
                            rval |= utilities.compute_cr(p_flow, p_flow_tbl_list, &p_ctx->filters);

                            // prepare idsp_ik_flow_ctrl_t
                            rval |= exe_set_flow_header(context_id, p_flow);
                            rval |= exe_clean_flow_ctrl_cache(p_flow, p_flow_tbl_list, &p_ctx->filters.update_flags.cr);

#if SUPPORT_CR_MEMORY_FENCE
                            rval |= exe_ik_check_cr_mem_fence(context_id, 0);
                            rval |= exe_ik_check_flow_tbl_memory_fence(context_id);
#endif
                            if(rval == IK_OK) {
                                rval |= img_ctx_reset_update_flags(context_id);

#if SUPPORT_IKC_ADDR_CHECK
                                if(p_ik_buffer_info->safety_enable == 1u) {
#ifndef EARLYTEST_ENV
                                    ik_buf_info_t ik_buf_info;
                                    (void)amba_ik_system_memset(&ik_buf_info, 0x0, sizeof(ik_buf_info));
                                    (void)amba_ik_system_memcpy(&ik_buf_info.p_flow_start_addr, &p_flow, sizeof(void *));
                                    (void)amba_ik_system_memcpy(&ik_buf_info.p_cr_start_addr, &p_flow_tbl_list->p_CR_buf_4, sizeof(void *));
                                    rval |= img_notify_iks_buffer_addr(context_id, &ik_buf_info);
#endif
                                }
#endif

                                if(p_ctx->filters.input_param.crc_mismatch_error_injection != 0U) {
                                    rval |= img_exe_inject_safety_crc_mismatch_error(p_flow_tbl_list);
                                }

                                //crc
                                if(p_ik_buffer_info->safety_crc_enable == 1u) {
#ifndef EARLYTEST_ENV
                                    if((p_ctx->filters.input_param.safety_info.update_freq == 1u) ||
                                       ((((uint32)p_ctx->organization.active_cr_state.cr_running_number - 1u) % p_ctx->filters.input_param.safety_info.update_freq) == 0u)) {
                                        rval |= exe_ik_select_crc_check_list(context_id);
                                        rval |= exe_ik_calc_crc32(context_id, 0);
                                    }
#endif
                                }

                                rval |= exe_set_frame_info(context_id, 0);
                                //special use for cr_ring_number == 1, always 1st_update and user should set all mandatory filters all the time.
                                if(p_ctx->organization.attribute.cr_ring_number > 1U) {
                                    p_ctx->organization.executer_method.execute_cr = img_exe_execute_update_cr;
                                }
                            } else {
                                p_ik_buffer_info->safety_state = IK_SAFETY_STATE_ERROR;
                                p_ctx->organization.executer_method.execute_cr = img_exe_execute_safety_cr;
                                img_ctx_hook_safety_filter_method(&p_ctx->organization.filter_methods);
                            }
                        } else {
                            p_ik_buffer_info->safety_state = IK_SAFETY_STATE_ERROR;
                            p_ctx->organization.executer_method.execute_cr = img_exe_execute_safety_cr;
                            img_ctx_hook_safety_filter_method(&p_ctx->organization.filter_methods);
                        }
                    } else {
                        amba_ik_system_print_uint32_5("[IK][ERROR] img_exe_execute_filter_update_check(context_id %d) fail !", context_id, DC_U, DC_U, DC_U, DC_U);
                    }
                } else {
                    amba_ik_system_print_uint32_5("[IK] call img_ctx_get_flow_control(%d) fail", context_id, DC_U, DC_U, DC_U, DC_U);
                }
            } else {
                amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial", context_id, DC_U, DC_U, DC_U, DC_U);
                rval = IK_ERR_0002;
            }
        } else {
            p_ik_buffer_info->safety_state = IK_SAFETY_STATE_ERROR;
            p_ctx->organization.executer_method.execute_cr = img_exe_execute_safety_cr;
            img_ctx_hook_safety_filter_method(&p_ctx->organization.filter_methods);
            rval |= IK_ERR_0200;
        }
    } else {
        amba_ik_system_print_str_5("[IK] can't get context", NULL, NULL, NULL, NULL, NULL);
    }

    return rval;
}

static uint32 exe_query_frame_info_check_run_num(uint32 current_running_num, uint32 max_running_num, uint32 running_num, uint32 cr_ring_number)
{
    uint32 rval = IK_OK;
    uint32 gap;

    if((equal_op_u32(running_num, 0U)+ equal_op_u32(current_running_num, 0U)) != 0U) {
        amba_ik_system_print_uint32_5("[IK][ERROR] running_num : %d or current_running_num : %d is Zero, should'n be Zero, start from 1", running_num, current_running_num, DC_U, DC_U, DC_U);
        rval = IK_ERR_0000; // TBD
    } else if(running_num > max_running_num) {
        amba_ik_system_print_uint32_5("[IK][ERROR] running_num : %d > max_running_num = : %d", running_num, max_running_num, DC_U, DC_U, DC_U);
        rval = IK_ERR_0000; // TBD
    } else if(current_running_num > running_num) {
        gap = (current_running_num - running_num) + 1U;
        if(gap > cr_ring_number) {
            amba_ik_system_print_uint32_5("[IK][ERROR] running_num : %d, current_running_num : %d, max_running_num : %d, gap = %d, exceed ring buffer depth : %d", running_num, current_running_num, max_running_num, gap, cr_ring_number);
            rval = IK_ERR_0000; // TBD
        }
    } else if(current_running_num < running_num) {
        //ex, max_running_num = 65536, running_num = 65535, current_running_num = 1, [65535, 65536, 1] total number = 3.
        gap = (max_running_num - running_num) + current_running_num + 1U;
        if(gap > cr_ring_number) {
            amba_ik_system_print_uint32_5("[IK][ERROR] running_num : %d, current_running_num : %d, max_running_num : %d, gap = %d, exceed ring buffer depth : %d", running_num, current_running_num, max_running_num, gap, cr_ring_number);
            rval = IK_ERR_0000; // TBD
        }
    } else { //allow current_running_num == running_num to get post-exe N from timing N.
        ;
    }
    return rval;
}

#endif

