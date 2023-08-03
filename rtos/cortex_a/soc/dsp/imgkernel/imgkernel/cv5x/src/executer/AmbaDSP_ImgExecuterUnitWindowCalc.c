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

#include "AmbaDSP_ImgConfigEntity.h"
#include "AmbaDSP_ImgArchSystemAPI.h"
#include "AmbaDSP_ImgExecuterCalib.h"
#include "AmbaDSP_ImgDebugUtility.h"

#define OVERLAP_X_MIN   64U

static uint32 min_u32(uint32 x, uint32 y)
{
    uint32 r_val;
    if (x > y) {
        r_val = y;
    } else {
        r_val = x;
    }
    return r_val;
}

static uint32 max_u32(uint32 x, uint32 y)
{
    uint32 r_val;
    if (x > y) {
        r_val = x;
    } else {
        r_val = y;
    }
    return r_val;
}


static inline uint32 exe_check_active_win_validataion(const amba_ik_check_active_win_validataion_t *p_info)
{
    uint32 rval = 0U;

    if (p_info == NULL) {
        rval = IK_ERR_0005;
        amba_ik_system_print_str_5("[IK]_exe_iso_cfg_check_active_win_validataion(): p_info == NULL", DC_S, DC_S, DC_S, DC_S, DC_S);
    } else {
        if (p_info->p_active_window->enable == 1U) {
            rval = 1U;
            if ((p_info->p_active_window->active_geo.start_x + p_info->p_active_window->active_geo.width) > p_info->p_window_size_info->vin_sensor.width) {
                rval = 0U;
            }
            if ((p_info->p_active_window->active_geo.start_y + p_info->p_active_window->active_geo.height) > p_info->p_window_size_info->vin_sensor.height) {
                rval = 0U;
            }
        } else {
            rval = 0U;
        }
    }
    return rval;
}

static inline uint32 exe_actual_win_calculate(const amba_ik_actual_win_calculate_t *p_info)
{
    uint32 rval = IK_OK;
    uint32 RawW, RawH, ZX, ZY;
    int32 left_top_x, left_top_y, CompensateX, CompensateY;
    ik_win_coordinates_t Act;
    uint64 misra_uint64;
    uint32 misra_uint32;
    int32 misra_int32;
    amba_ik_check_active_win_validataion_t check_active_win_validataion_info;

    if (p_info == NULL) {
        rval = IK_ERR_0005;
        amba_ik_system_print_str_5("[IK]_exe_iso_cfg_actual_win_calculate(): p_info == NULL", DC_S, DC_S, DC_S, DC_S, DC_S);
    } else {
        check_active_win_validataion_info.p_window_size_info = p_info->p_window_size_info;
        check_active_win_validataion_info.p_active_window = p_info->p_active_window;

        /* A. Get RawW and RawH */
        RawW = p_info->p_window_size_info->vin_sensor.width;
        RawH = p_info->p_window_size_info->vin_sensor.height;
        if (exe_check_active_win_validataion(&check_active_win_validataion_info)==1U) {
            RawW = p_info->p_active_window->active_geo.width;
            RawH = p_info->p_active_window->active_geo.height;
        }

        /* B. Calc Act width and height by zoom factor */
        ZX = p_info->p_dzoom_info->zoom_x;
        ZY = p_info->p_dzoom_info->zoom_y;
        if (ZX>=1U) {
            misra_uint64 = (((uint64)RawW<<(uint64)SFT) - (((uint64)RawW << ((uint64)SFT << 1UL)) / (uint64)ZX)) >> 1UL;
            Act.left_top_x = (uint32)misra_uint64;
        } else {
            Act.left_top_x = 0U;
        }
        if (ZY>=1U) {
            misra_uint64 = (((uint64)RawH<<(uint64)SFT) - (((uint64)RawH << ((uint64)SFT << 1UL)) / ZY)) >> 1UL;
            Act.left_top_y = (uint32)misra_uint64;
        } else {
            Act.left_top_y = 0U;
        }
        Act.right_bot_x = (RawW<<SFT) - Act.left_top_x;
        Act.right_bot_y = (RawH<<SFT) - Act.left_top_y;

        /* C. Calc Act window shift by ShiftX and ShiftY */
        left_top_x = (int32)Act.left_top_x;
        left_top_x += p_info->p_dzoom_info->shift_x;
        left_top_y = (int32)Act.left_top_y;
        left_top_y += p_info->p_dzoom_info->shift_y;
        CompensateX = 0;
        CompensateY = 0;
        if (left_top_x < 0) {
            amba_ik_system_print_int32_5("[IK] left_top_x %d < 0\n", (int32)left_top_x/65536/*>>SFT*/, DC_I, DC_I, DC_I, DC_I);
            CompensateX = 0 - left_top_x;
            left_top_x = 0;
        }
        if (left_top_y < 0) {
            amba_ik_system_print_int32_5("[IK] left_top_y %d < 0\n", (int32)left_top_y/65536/*>>SFT*/, DC_I, DC_I, DC_I, DC_I);
            CompensateY = 0 - left_top_y;
            left_top_y = 0;
        }
        Act.left_top_x = (uint32)left_top_x;
        Act.left_top_y = (uint32)left_top_y;
        misra_int32 = (int32)Act.right_bot_x + p_info->p_dzoom_info->shift_x + CompensateX;
        Act.right_bot_x = (uint32)misra_int32;
        misra_int32 = (int32)Act.right_bot_y + p_info->p_dzoom_info->shift_y + CompensateY;
        Act.right_bot_y = (uint32)misra_int32;

        /* D. Range check. Act window cannot exceed raw window */
        if (Act.right_bot_x > (RawW << SFT)) {
            amba_ik_system_print_uint32_5("[IK] Act.right_bot_x %d > width %d<<16\n", Act.right_bot_x, RawW, DC_U, DC_U, DC_U);
            misra_uint32 = Act.right_bot_x - (RawW << SFT);
            CompensateX = (int32)misra_uint32;
            Act.right_bot_x = (RawW<<SFT);
        }
        if (Act.right_bot_y > (RawH << SFT)) {
            amba_ik_system_print_uint32_5("[IK] Act.right_bot_y %d > height %d<<16\n", Act.right_bot_y, RawH, DC_U, DC_U, DC_U);
            misra_uint32 = Act.right_bot_y - (RawH << SFT);
            CompensateY = (int32)misra_uint32;
            Act.right_bot_y = (RawH << SFT);
        }
        if (CompensateX>=1) {
            if ((int32)Act.left_top_x < CompensateX) {
                amba_ik_system_print_uint32_5("[IK] Act.left_top_x %d < CompensateX %d\n", Act.left_top_x, (uint32)CompensateX, DC_U, DC_U, DC_U);
                Act.left_top_x = 0U;
            } else {
                misra_int32 = (int32)Act.left_top_x - CompensateX;
                Act.left_top_x = (uint32)misra_int32;
            }
        }
        if (CompensateY>=1) {
            if ((int32)Act.left_top_y < CompensateY) {
                amba_ik_system_print_uint32_5("[IK] Act.left_top_y %d < CompensateY %d\n", Act.left_top_y, (uint32)CompensateY, DC_U, DC_U, DC_U);
                Act.left_top_y = 0;
            } else {
                misra_int32 = (int32)Act.left_top_y - CompensateY;
                Act.left_top_y = (uint32)misra_int32;
            }
        }

        /* E. Assign act_win_crop and return */
        p_info->p_result_win->act_win_crop = Act;
        //amba_ik_system_print("[IK]  act x:%d, y:%d, x:%d, y:%d", Act.left_top_x>>16, Act.left_top_y>>16, Act.right_bot_x>>16, Act.right_bot_y>>16);
    }
    return rval;
}

static inline uint32 exe_stitching_symmetric_calculate(uint32 raw_w, const ik_win_coordinates_t *p_act_win_crop, uint32 range_left_q16, uint32 range_right_q16)
{
    uint32 misra_range_left_q16;
    uint32 misra_range_right_q16;
    if(p_act_win_crop->left_top_x < range_left_q16) {
        misra_range_left_q16 = p_act_win_crop->left_top_x;
    } else {
        misra_range_left_q16 = range_left_q16;
    }
    if((p_act_win_crop->right_bot_x + range_right_q16) > (raw_w<<SFT)) {
        misra_range_right_q16 = (raw_w<<SFT) - p_act_win_crop->right_bot_x;
    } else {
        misra_range_right_q16 = range_right_q16;
    }
    return (misra_range_left_q16 > misra_range_right_q16) ? misra_range_right_q16 : misra_range_left_q16;//return minimum.
}

#define MAX_IDSP_COLUMN 2048U

static inline uint32 exe_idsp_max_cols_protect(uint32 stitching_num_x, uint32 raw_w, const ik_win_coordinates_t *p_act_win_crop, ik_window_geomery_t *p_dmy)
{
    //actual win coordinate here locate on Vin.
    uint32 rval = IK_OK;
    uint32 divided_tile_w;
    uint32 ideal_max_overlap;
    uint32 idea_dmy_bound;
    uint32 aligned_dma_boud;

    if(stitching_num_x>1u){
        if (((((raw_w+stitching_num_x-1U)/stitching_num_x)+127U)&0xffffff80U) > MAX_IDSP_COLUMN) {
            //Vin width / stitching number > 2048.
            amba_ik_system_print_uint32_5("[IK] Vin(Active) width : %d divided by horizontal stitching number %d, align 128 will > 2048 pixels wide \n", raw_w, stitching_num_x, DC_U, DC_U, DC_U);
            rval = IK_ERR_0101;
        }
        divided_tile_w = (p_act_win_crop->right_bot_x - p_act_win_crop->left_top_x + stitching_num_x - 1U)/stitching_num_x;
        divided_tile_w = (divided_tile_w+65535U)>>SFT;
        ideal_max_overlap = (MAX_IDSP_COLUMN-divided_tile_w)>>1U;
        if(ideal_max_overlap > (p_act_win_crop->left_top_x>>SFT)) {
            idea_dmy_bound = 0;
        } else {
            idea_dmy_bound = (p_act_win_crop->left_top_x>>SFT) - ideal_max_overlap;
        }
        aligned_dma_boud = (idea_dmy_bound+127U)&0xffffff80U;//left bound rightward align.
        if(p_dmy->start_x < aligned_dma_boud) { //furthest left bound.
            p_dmy->start_x = aligned_dma_boud;
        }
        idea_dmy_bound = ((p_act_win_crop->right_bot_x+65535U)>>SFT) + ideal_max_overlap;
        aligned_dma_boud = idea_dmy_bound&0xffffff80U;//right bound leftward align.
        if((p_dmy->start_x+p_dmy->width) > aligned_dma_boud) { //furthest right bound.
            p_dmy->width = aligned_dma_boud-p_dmy->start_x;
        }
    } else {
        // misraC
    }

    return rval;
}

static inline void exe_logical_dummy_win_calc(const ik_window_geomery_t *p_dmy, const ik_win_coordinates_t *p_act_win_crop, uint32 range_left_q16, uint32 range_right_q16, ik_window_geomery_t *p_logical_dmy)
{
    //dummy and actual win coordinate here locate on Vin(Active).
    //logical dummy window relatively coordinates on dummy window.
    //AAA tiles align on logical dummy window, to avoid non-symmetric stitching issue.
    if(((p_dmy->start_x<<SFT) + range_left_q16) > p_act_win_crop->left_top_x) {
        p_logical_dmy->start_x = 0;
    } else {
        p_logical_dmy->start_x = (p_act_win_crop->left_top_x-range_left_q16)>>SFT;
        p_logical_dmy->start_x = p_logical_dmy->start_x&0xfffffffcU;//leftward 4-align.
        p_logical_dmy->start_x -= p_dmy->start_x;
    }

    if((p_act_win_crop->right_bot_x + range_right_q16) > ((p_dmy->start_x + p_dmy->width)<<SFT)) {
        p_logical_dmy->width = p_dmy->start_x + p_dmy->width - p_logical_dmy->start_x - p_dmy->start_x;
    } else {
        p_logical_dmy->width = ((p_act_win_crop->right_bot_x + range_right_q16 + 65535U)>>SFT) - p_logical_dmy->start_x - p_dmy->start_x;
        p_logical_dmy->width = (p_logical_dmy->width+3U)&0xfffffffcU;//rightward 4-align.
    }

    p_logical_dmy->start_y = 0;
    p_logical_dmy->height = p_dmy->height;
}

static inline uint32 int32_right_shift(int32 input, uint32 shift)
{
    uint32 misra_u32;
    uint32 sign_mask = 0xffffffffU;

    if (input < 0) {
        sign_mask = (sign_mask >> shift) << shift;
        misra_u32 = (uint32)input >> shift;
        misra_u32 |= sign_mask;
    } else {
        misra_u32 = (uint32)input >> shift;
    }

    return misra_u32;
}

static inline void exe_dummy_hdr_sft_calc(const ik_window_geomery_t *p_dmy, const ik_window_dimension_t *p_raw_size, const ik_hdr_eis_info_t *p_hdr_eis_sft_exp1, const ik_hdr_eis_info_t *p_hdr_eis_sft_exp2, ik_win_hdr_eis_sft_t *p_hdr_eis_sft)
{
    //dummy win coordinate here locate on Vin(Active).

    int32 out_offset_x;
    int32 out_offset_y;
    int32 sft_x;
    int32 sft_y;
    uint32 sign_bit;
    uint32 misra_tmp;

    {
        //exp1
        //positive round down, negative round up.
        sign_bit = (((uint32)p_hdr_eis_sft_exp1->shift_x)>>31);
        misra_tmp = (int32_right_shift(p_hdr_eis_sft_exp1->shift_x, SFT)+sign_bit)&0xfffffffeU;//2-alignment.
        (void)amba_ik_system_memcpy(&sft_x, &misra_tmp, sizeof(int32));

        //positive round down, negative round up.
        sign_bit = (((uint32)p_hdr_eis_sft_exp1->shift_y)>>31);
        misra_tmp = (int32_right_shift(p_hdr_eis_sft_exp1->shift_y, SFT)+sign_bit)&0xfffffffeU;//2-alignment.
        (void)amba_ik_system_memcpy(&sft_y, &misra_tmp, sizeof(int32));

        if(((int32)p_dmy->start_x + sft_x) < 0) {
            out_offset_x = 0;//clamp.
        } else if(((int32)p_dmy->start_x + (int32)p_dmy->width + sft_x) > (int32)p_raw_size->width) {
            out_offset_x = (int32)p_raw_size->width - (int32)p_dmy->start_x - (int32)p_dmy->width;//clamp.
            misra_tmp = (uint32)out_offset_x & 0xfffffffeU;//2-alignment.
            (void)amba_ik_system_memcpy(&out_offset_x, &misra_tmp, sizeof(int32));
        } else {
            out_offset_x = sft_x;
        }

        if(((int32)p_dmy->start_y + sft_y) < 0) {
            out_offset_y = 0;//clamp.
        } else if(((int32)p_dmy->start_y + (int32)p_dmy->height + sft_y) > (int32)p_raw_size->height) {
            out_offset_y = (int32)p_raw_size->height - (int32)p_dmy->start_y - (int32)p_dmy->height;//clamp.
            misra_tmp = (uint32)out_offset_y & 0xfffffffeU;//2-alignment.
            (void)amba_ik_system_memcpy(&out_offset_y, &misra_tmp, sizeof(int32));
        } else {
            out_offset_y = sft_y;
        }

        p_hdr_eis_sft->horizontal_shift_exp1 = out_offset_x;
        p_hdr_eis_sft->vertical_shift_exp1 = out_offset_y;
    }

    {
        //exp2
        //positive round down, negative round up.
        sign_bit = (((uint32)p_hdr_eis_sft_exp1->shift_x)>>31);
        misra_tmp = (int32_right_shift(p_hdr_eis_sft_exp2->shift_x, SFT)+sign_bit)&0xfffffffeU;//2-alignment.
        (void)amba_ik_system_memcpy(&sft_x, &misra_tmp, sizeof(int32));

        //positive round down, negative round up.
        sign_bit = (((uint32)p_hdr_eis_sft_exp1->shift_y)>>31);
        misra_tmp = (int32_right_shift(p_hdr_eis_sft_exp2->shift_y, SFT)+sign_bit)&0xfffffffeU;//2-alignment.
        (void)amba_ik_system_memcpy(&sft_y, &misra_tmp, sizeof(int32));

        if(((int32)p_dmy->start_x + sft_x) < 0) {
            out_offset_x = 0;//clamp.
        } else if(((int32)p_dmy->start_x + (int32)p_dmy->width + sft_x) > (int32)p_raw_size->width) {
            out_offset_x = (int32)p_raw_size->width - (int32)p_dmy->start_x - (int32)p_dmy->width;//clamp.
            misra_tmp = (uint32)out_offset_x & 0xfffffffeU;//2-alignment.
            (void)amba_ik_system_memcpy(&out_offset_x, &misra_tmp, sizeof(int32));
        } else {
            out_offset_x = sft_x;
        }

        if(((int32)p_dmy->start_y + sft_y) < 0) {
            out_offset_y = 0;//clamp.
        } else if(((int32)p_dmy->start_y + (int32)p_dmy->height + sft_y) > (int32)p_raw_size->height) {
            out_offset_y = (int32)p_raw_size->height - (int32)p_dmy->start_y - (int32)p_dmy->height;//clamp.
            misra_tmp = (uint32)out_offset_y & 0xfffffffeU;//2-alignment.
            (void)amba_ik_system_memcpy(&out_offset_y, &misra_tmp, sizeof(int32));
        } else {
            out_offset_y = sft_y;
        }

        p_hdr_eis_sft->horizontal_shift_exp2 = out_offset_x;
        p_hdr_eis_sft->vertical_shift_exp2 = out_offset_y;
    }
}

static inline uint32 exe_dummy_win_calculate(const amba_ik_dummy_win_calculate_t *p_info)
{
    uint32 rval = IK_OK;
    int32 tmp;
    uint32 raw_w, raw_h, rad;
    uint32 range_left_q16, range_top_q16, range_right_q16, range_bottom_q16;
    ik_window_geomery_t dmy = {0, 0, 0, 0};
    uint32 start_x, start_y, end_x, end_y;
    uint32 start_x_q16, start_y_q16, end_x_q16, end_y_q16;
    uint32 align;
    uint32 misra_uint32;
    amba_ik_check_active_win_validataion_t check_active_win_validataion_info;
    uint32 overlap_x = 0U;

    if (p_info == NULL) {
        rval = IK_ERR_0005;
        amba_ik_system_print_str_5("[IK]_exe_iso_cfg_dummy_win_calculate(): p_info == NULL", DC_S, DC_S, DC_S, DC_S, DC_S);
    } else {
        overlap_x = max_u32(OVERLAP_X_MIN, p_info->p_overlap_x->overlap_x);
        check_active_win_validataion_info.p_window_size_info = p_info->p_window_size_info;
        check_active_win_validataion_info.p_active_window = p_info->p_active_window;

        /* A. Get RawW and RawH */
        raw_w = p_info->p_window_size_info->vin_sensor.width;
        raw_h = p_info->p_window_size_info->vin_sensor.height;
        if (exe_check_active_win_validataion(&check_active_win_validataion_info)==1U) {
            raw_w = p_info->p_active_window->active_geo.width;
            raw_h = p_info->p_active_window->active_geo.height;
        }

        /* B. Calculate Y direction dummy window. */
        if ((p_info->warp_enable==1) && (p_info->p_dmy_range->enable==1U)) {
            range_top_q16 = raw_h * p_info->p_dmy_range->top;
            range_bottom_q16 = raw_h * p_info->p_dmy_range->bottom;
        } else {
            range_top_q16 = 0U;
            range_bottom_q16 = 0U;
        }

        if (p_info->p_result_win->act_win_crop.left_top_y > range_top_q16) {
            start_y_q16 = p_info->p_result_win->act_win_crop.left_top_y - range_top_q16;
        } else {
            start_y_q16 = 0U;
        }
        if ((p_info->p_result_win->act_win_crop.right_bot_y + range_bottom_q16) > (raw_h<<SFT)) {
            end_y_q16 = raw_h << SFT;
        } else {
            end_y_q16 = p_info->p_result_win->act_win_crop.right_bot_y + range_bottom_q16;
        }

        if (p_info->sensor_mode == IK_SENSOR_MODE_RGBIR){ /*4 alignment for RGB IR */
            align = 0xFFFFFFFCU;
        } else {
            align = 0xFFFFFFFEU;
        }

        /* 2 alignment */
        start_y = (start_y_q16>>SFT) & align;
        end_y = ((end_y_q16 + 0x1FFFFU) & 0xFFFE0000U)>>SFT;
        dmy.start_y = start_y;
        dmy.height = end_y - start_y;
        dmy.height = (dmy.height+7U) & 0xFFFFFFF8U;//CV2A amalgam asserts dummy height must 8 aligned, due to CE.

        /* Range check between raw window and dmy window */
        tmp = (int32)raw_h - (int32)dmy.height; /* tmp means the valid max start position. */
        if (tmp < 0) {
            /* When tmp < 0, it means that calculating dmy.height is bigger than Raw.
                       So we use the max valid value, raw_h&0xFFFFFFFE, and edit the valid max start position. */
            dmy.height = raw_h & align;
            tmp = (int32)raw_h - (int32)dmy.height;
        }
        if ((int32)dmy.start_y > tmp) {
            /* If calculating dmy.start_y is bigger than the valid max start position,
                        we adjust to the max valid value.
                        It must be 2 alignment for avoiding mismatch with bayer pattern. */
            dmy.start_y = (uint32)tmp & align;
        }

        /* C. Calculate X direction dummy window. */
        if ((p_info->p_dmy_range->enable==1U)) {
            range_left_q16 = raw_w * p_info->p_dmy_range->left;
            range_right_q16 = raw_w * p_info->p_dmy_range->right;
        } else {
            range_left_q16 = 0U;
            range_right_q16 = 0U;
        }
        if((p_info->p_stitching_info->enable != 0)&&(p_info->p_stitching_info->tile_num_x > 1)) {
            //horizontal stitching condtion, make sure actual window center = logical dummy window center.
            //ucode restriction.2020.01.14.
            range_left_q16 = min_u32(range_left_q16, overlap_x<<SFT);
            range_right_q16 = min_u32(range_right_q16, overlap_x<<SFT);
            range_left_q16 = exe_stitching_symmetric_calculate(raw_w, &p_info->p_result_win->act_win_crop, range_left_q16, range_right_q16);
            range_right_q16 = range_left_q16;
        }

        if (p_info->p_result_win->act_win_crop.left_top_x > range_left_q16) {
            start_x_q16 = p_info->p_result_win->act_win_crop.left_top_x - range_left_q16;
        } else {
            start_x_q16 = 0U;
        }
        if ((p_info->p_result_win->act_win_crop.right_bot_x + range_right_q16) > (raw_w<<SFT)) {
            end_x_q16 = raw_w << SFT;
        } else {
            end_x_q16 = p_info->p_result_win->act_win_crop.right_bot_x + range_right_q16;
        }

        /* 2 alignment */
        start_x = (start_x_q16>>SFT) & 0xFFFFFFFEU;
        end_x = ((end_x_q16 + 0x1FFFFU) & 0xFFFE0000U)>>SFT;
        dmy.start_x = start_x;
        dmy.width = end_x - start_x;

        //32-align
        /* Alignment for compressed raw start x */
        rad = dmy.start_x&(AMBA_IK_BASIC_ALIGNMENT-1U);
        if (rad>=1U) {
            dmy.start_x -= rad;
            dmy.width += rad;
        }
        /* Alignment for compressed raw width */
        align = 0xFFFFFFFFU-(AMBA_IK_BASIC_ALIGNMENT-1U);
        dmy.width = (dmy.width + (AMBA_IK_BASIC_ALIGNMENT-1U))&align;

        /* Range check between raw window and dmy window */
        tmp = (int32)raw_w - (int32)dmy.width;
        if (tmp < 0) {
            dmy.width = raw_w&align;
            tmp = (int32)raw_w - (int32)dmy.width;
        }
        if ((int32)dmy.start_x > tmp) {
            dmy.start_x = (uint32)tmp;
        }
#if 0
        /* Check no stitching tile width > 2048 */
        rval |= exe_idsp_max_cols_protect(
                    (p_info->p_stitching_info->enable == 0) ? 1U : (uint32)p_info->p_stitching_info->tile_num_x,
                    raw_w,
                    &p_info->p_result_win->act_win_crop,
                    &dmy);
#endif
        /* D. Range Check. */
        misra_uint32 = ((p_info->p_result_win->act_win_crop.right_bot_x+0xFFFFU)>>SFT) - (dmy.start_x + dmy.width);
        tmp = (int32)misra_uint32;
        if (tmp > 0) {
            amba_ik_system_print_uint32_5("[IK] Detect dummy window cannot cover actual window. actX (%d ~ %d), dmyX (%d ~ %d)\n",
                                          ((p_info->p_result_win->act_win_crop.left_top_x + 0xFFFFU)>>SFT),  ((p_info->p_result_win->act_win_crop.right_bot_x + 0xFFFFU)>>SFT),
                                          dmy.start_x, dmy.start_x + dmy.width, DC_U);
            p_info->p_result_win->act_win_crop.right_bot_x -= ((uint32)tmp<<SFT);
        }
        misra_uint32 = ((p_info->p_result_win->act_win_crop.right_bot_y + 0xFFFFU)>>SFT)  - (dmy.start_y + dmy.height);
        tmp = (int32)misra_uint32;
        if (tmp > 0) {
            amba_ik_system_print_uint32_5("[IK] Detect dummy window cannot cover actual window. actY (%d ~ %d), dmyY (%d ~ %d)\n",
                                          ((p_info->p_result_win->act_win_crop.left_top_y + 0xFFFFU)>>SFT),  ((p_info->p_result_win->act_win_crop.right_bot_y + 0xFFFFU)>>SFT),
                                          dmy.start_y, dmy.start_y + dmy.height, DC_U);
            p_info->p_result_win->act_win_crop.right_bot_y -= ((uint32)tmp<<SFT);
        }

        /* E. Assign dmy_win_geo and return */
        p_info->p_result_win->dmy_win_geo = dmy;

        /* Calculate logical dummy window */
        exe_logical_dummy_win_calc(
            &dmy,
            &p_info->p_result_win->act_win_crop,
            range_left_q16,
            range_right_q16,
            &p_info->p_result_win->logical_dmy_win_geo);

        /* Calculate hdr eis shift exp1 and exp2 */
        exe_dummy_hdr_sft_calc(
            &dmy,
        &(ik_window_dimension_t) {
            .width = raw_w, .height = raw_h
        },
        p_info->p_hdr_eis_sft_exp1,
        p_info->p_hdr_eis_sft_exp2,
        &p_info->p_result_win->hdr_eis_sft);

        //amba_ik_system_print("[IK]  dmy x:%d, y:%d, w:%d, h:%d", dmy.start_x, dmy.start_y, dmy.width, dmy.height);
    }
    return rval;

}

static inline uint32 exe_cfa_win_calculate(const amba_ik_cfa_win_calculate_t *p_info)
{
    uint32 rval = IK_OK;

    if (p_info == NULL) {
        rval = IK_ERR_0005;
        amba_ik_system_print_str_5("[IK]_exe_iso_cfg_cfa_win_calculate(): p_info == NULL", DC_S, DC_S, DC_S, DC_S, DC_S);
    } else {
        if (p_info->p_cfa_window_size_info->enable == 1U) {
            p_info->p_result_win->cfa_win_dim.width = p_info->p_cfa_window_size_info->cfa.width;
            p_info->p_result_win->cfa_win_dim.height = p_info->p_cfa_window_size_info->cfa.height;
        } else {
            p_info->p_result_win->cfa_win_dim.width = p_info->p_result_win->dmy_win_geo.width;
            p_info->p_result_win->cfa_win_dim.height = p_info->p_result_win->dmy_win_geo.height;
        }
        //(void)amba_ik_system_memcpy(&p_info->p_result_win->cfa_win_dim, &p_filters->input_param.window_size_info.cfa_window, sizeof(ik_window_dimension_t));

        // *********************** checker clamp *********************** //
        // cfa > dummy
        if (p_info->p_result_win->cfa_win_dim.width > p_info->p_result_win->dmy_win_geo.width) {
            amba_ik_system_print_uint32_5("[IK][WARNING] cfa width:%d > dummy width:%d, ik set cfa width = dummy width", p_info->p_result_win->cfa_win_dim.width, p_info->p_result_win->dmy_win_geo.width, DC_U, DC_U, DC_U);
            p_info->p_result_win->cfa_win_dim.width = p_info->p_result_win->dmy_win_geo.width;
        }
        if (p_info->p_result_win->cfa_win_dim.height > p_info->p_result_win->dmy_win_geo.height) {
            amba_ik_system_print_uint32_5("[IK][WARNING] cfa height:%d > dummy height:%d, ik set cfa height = dummy height", p_info->p_result_win->cfa_win_dim.height, p_info->p_result_win->dmy_win_geo.height, DC_U, DC_U, DC_U);
            p_info->p_result_win->cfa_win_dim.height = p_info->p_result_win->dmy_win_geo.height;
        }

        // cfa 2 alignment
        if ((((p_info->p_result_win->cfa_win_dim.width>>1U)<<1U) - p_info->p_result_win->cfa_win_dim.width)>0U) {
            amba_ik_system_print_uint32_5("[IK][WARNING] cfa width:%d is needed to be 2 alignment, ik set it %d", p_info->p_result_win->cfa_win_dim.width, p_info->p_result_win->cfa_win_dim.width-1U, DC_U, DC_U, DC_U);
            p_info->p_result_win->cfa_win_dim.width--;
        }
        if ((((p_info->p_result_win->cfa_win_dim.height>>1U)<<1U) - p_info->p_result_win->cfa_win_dim.height)>0U) {
            amba_ik_system_print_uint32_5("[IK][WARNING] cfa height:%d is needed to be 2 alignment, ik set it %d", p_info->p_result_win->cfa_win_dim.height, p_info->p_result_win->cfa_win_dim.height-1U, DC_U, DC_U, DC_U);
            p_info->p_result_win->cfa_win_dim.height--;
        }
    }

    return rval;
}

uint32 exe_win_calc(const amba_ik_win_calc_t *p_info)
{
    uint32 rval = IK_OK;
    amba_ik_actual_win_calculate_t actual_win_calculate_info;
    amba_ik_dummy_win_calculate_t dummy_win_calculate_info;
    amba_ik_cfa_win_calculate_t cfa_win_calculate_info;
    amba_ik_check_active_win_validataion_t check_active_win_validataion_info;

    if (p_info == NULL) {
        rval = IK_ERR_0005;
        amba_ik_system_print_str_5("[IK]exe_iso_cfg_win_calc(): p_info == NULL", DC_S, DC_S, DC_S, DC_S, DC_S);
    } else {
        /* A. Calc Act Win: Vin + ZF -> Act */
        actual_win_calculate_info.p_window_size_info = p_info->p_window_size_info;
        actual_win_calculate_info.p_active_window = p_info->p_active_window;
        actual_win_calculate_info.p_dzoom_info = p_info->p_dzoom_info;
        actual_win_calculate_info.p_result_win = p_info->p_result_win;
        if (exe_actual_win_calculate(&actual_win_calculate_info) == IK_OK) {
        } else {
            amba_ik_system_print_str_5("[IK] Error, actual_win_calculate fail.\n", DC_S, DC_S, DC_S, DC_S, DC_S);
            rval |= IK_ERR_0104;
        }

        /* B. Calc Dummy Win: Act + Dmy Range -> Dmy */
        dummy_win_calculate_info.warp_enable = p_info->warp_enable;
        dummy_win_calculate_info.ability = p_info->ability;
        dummy_win_calculate_info.sensor_mode = p_info->sensor_mode;
        dummy_win_calculate_info.p_window_size_info = p_info->p_window_size_info;
        dummy_win_calculate_info.p_active_window = p_info->p_active_window;
        dummy_win_calculate_info.p_dmy_range = p_info->p_dmy_range;
        dummy_win_calculate_info.p_stitching_info = p_info->p_stitching_info;
        dummy_win_calculate_info.p_overlap_x = p_info->p_overlap_x;
        dummy_win_calculate_info.p_hdr_eis_sft_exp1 = p_info->p_hdr_eis_sft_exp1;
        dummy_win_calculate_info.p_hdr_eis_sft_exp2 = p_info->p_hdr_eis_sft_exp2;
        dummy_win_calculate_info.p_result_win = p_info->p_result_win;
        if (exe_dummy_win_calculate(&dummy_win_calculate_info) == IK_OK) {
        } else {
            amba_ik_system_print_str_5("[IK] Error, dummy_win_calculate fail.\n", DC_S, DC_S, DC_S, DC_S, DC_S);
            rval |= IK_ERR_0104;
        }

        /* C. Calc Cfa Output Win: Dmy + Main + IDSP Clock -> Cfa */
        cfa_win_calculate_info.p_cfa_window_size_info = p_info->p_cfa_window_size_info;
        cfa_win_calculate_info.p_result_win = p_info->p_result_win;
        if (exe_cfa_win_calculate(&cfa_win_calculate_info) == IK_OK) {
        } else {
            amba_ik_system_print_str_5("[IK] Error, cfa_win_calculate fail.\n", DC_S, DC_S, DC_S, DC_S, DC_S);
            rval |= IK_ERR_0104;
        }

        /* D. Add Active window offset back to actual window and dummy window */
        check_active_win_validataion_info.p_window_size_info = p_info->p_window_size_info;
        check_active_win_validataion_info.p_active_window = p_info->p_active_window;
        if (exe_check_active_win_validataion(&check_active_win_validataion_info)>=1U) {
            if (p_info->p_active_window->active_geo.start_x>=1U) {
                p_info->p_result_win->act_win_crop.left_top_x += (p_info->p_active_window->active_geo.start_x << SFT);
                p_info->p_result_win->act_win_crop.right_bot_x += (p_info->p_active_window->active_geo.start_x << SFT);
                p_info->p_result_win->dmy_win_geo.start_x += p_info->p_active_window->active_geo.start_x;
            }
            if (p_info->p_active_window->active_geo.start_y>=1U) {
                p_info->p_result_win->act_win_crop.left_top_y += (p_info->p_active_window->active_geo.start_y << SFT);
                p_info->p_result_win->act_win_crop.right_bot_y += (p_info->p_active_window->active_geo.start_y << SFT);
                p_info->p_result_win->dmy_win_geo.start_y += p_info->p_active_window->active_geo.start_y;
            }
        }
        // Coordinate actual window on dummy window.
        p_info->p_result_win->act_win_crop.left_top_x -= (p_info->p_result_win->dmy_win_geo.start_x<<SFT);
        p_info->p_result_win->act_win_crop.right_bot_x -= (p_info->p_result_win->dmy_win_geo.start_x<<SFT);
        p_info->p_result_win->act_win_crop.left_top_y -= (p_info->p_result_win->dmy_win_geo.start_y<<SFT);
        p_info->p_result_win->act_win_crop.right_bot_y -= (p_info->p_result_win->dmy_win_geo.start_y<<SFT);

        /* E. Update Result Count */
        p_info->p_result_win->result_cnt = 1U;//p_filters->input_param.window_size_info.in_win_update_cnt;

        /* F. (option) Debug log */
#if 0
        {
            amba_ik_system_print_str_5("\n[IK] %s", __FUNCTION__, DC_S, DC_S, DC_S, DC_S);
            amba_ik_system_print_uint32_5("[IK] Zf  (%d, %d)", p_info->p_dzoom_info->zoom_x, p_info->p_dzoom_info->zoom_y, DC_U, DC_U, DC_U);
            amba_ik_system_print_int32_5("[IK] Sft (%d, %d)", p_info->p_dzoom_info->shift_x, p_info->p_dzoom_info->shift_y, DC_I, DC_I, DC_I);
            amba_ik_system_print_uint32_5("[IK] Vin x %d y %d w %d h %d",
                                          p_info->p_window_size_info->vin_sensor.start_x, p_info->p_window_size_info->vin_sensor.start_y,
                                          p_info->p_window_size_info->vin_sensor.width, p_info->p_window_size_info->vin_sensor.height, DC_U);
            amba_ik_system_print_uint32_5("[IK] HorSmp %d/%d VerSmp %d/%d",
                                          p_info->p_window_size_info->vin_sensor.h_sub_sample.factor_num, p_info->p_window_size_info->vin_sensor.h_sub_sample.factor_den,
                                          p_info->p_window_size_info->vin_sensor.v_sub_sample.factor_num, p_info->p_window_size_info->vin_sensor.v_sub_sample.factor_den, DC_U);
            amba_ik_system_print_uint32_5("[IK] Active x %d y %d w %d h %d",
                                          p_info->p_active_window->active_geo.start_x, p_info->p_active_window->active_geo.start_y,
                                          p_info->p_active_window->active_geo.width, p_info->p_active_window->active_geo.height, DC_U);
            amba_ik_system_print_uint32_5("[IK] Dmy x %d y %d w %d h %d",
                                          p_info->p_result_win->dmy_win_geo.start_x, p_info->p_result_win->dmy_win_geo.start_y,
                                          p_info->p_result_win->dmy_win_geo.width, p_info->p_result_win->dmy_win_geo.height, DC_U);
            amba_ik_system_print_uint32_5("[IK] Logical Dmy x %d y %d w %d h %d",
                                          p_info->p_result_win->logical_dmy_win_geo.start_x, p_info->p_result_win->logical_dmy_win_geo.start_y,
                                          p_info->p_result_win->logical_dmy_win_geo.width, p_info->p_result_win->logical_dmy_win_geo.height, DC_U);
            amba_ik_system_print_uint32_5("[IK] Cfa w %d h %d", p_info->p_result_win->cfa_win_dim.width, p_info->p_result_win->cfa_win_dim.height, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK] Act x %d y %d w %d h %d",
                                          p_info->p_result_win->act_win_crop.left_top_x>>SFT, p_info->p_result_win->act_win_crop.left_top_y>>SFT,
                                          (p_info->p_result_win->act_win_crop.right_bot_x - p_info->p_result_win->act_win_crop.left_top_x)>>SFT,
                                          (p_info->p_result_win->act_win_crop.right_bot_y - p_info->p_result_win->act_win_crop.left_top_y)>>SFT, DC_U);
            amba_ik_system_print_uint32_5("[IK] Main w %d h %d", p_info->p_window_size_info->main_win.width, p_info->p_window_size_info->main_win.height, DC_U, DC_U, DC_U);
        }
#endif
    }
    return rval;
}

uint32 exe_win_calc_wrapper(amba_ik_filter_t *p_filters)
{
    uint32 rval;
    amba_ik_win_calc_t info;

    info.warp_enable = p_filters->input_param.warp_enable;
    info.ability = p_filters->input_param.ability;
    info.sensor_mode = p_filters->input_param.sensor_info.sensor_mode;
    info.p_window_size_info = &p_filters->input_param.window_size_info;
    info.p_cfa_window_size_info = &p_filters->input_param.cfa_window_size_info;
    info.p_active_window = &p_filters->input_param.active_window;
    info.p_dmy_range = &p_filters->input_param.dmy_range;
    info.p_dzoom_info = &p_filters->input_param.dzoom_info;
    info.p_hdr_eis_sft_exp1 = &p_filters->input_param.hdr_eis_sft_exp1;
    info.p_hdr_eis_sft_exp2 = &p_filters->input_param.hdr_eis_sft_exp2;
    info.p_result_win = &p_filters->input_param.ctx_buf.result_win;
    info.p_stitching_info = &p_filters->input_param.stitching_info;
    info.p_overlap_x = &p_filters->input_param.overlap_x;

    rval = exe_win_calc(&info);

    return rval;
}


