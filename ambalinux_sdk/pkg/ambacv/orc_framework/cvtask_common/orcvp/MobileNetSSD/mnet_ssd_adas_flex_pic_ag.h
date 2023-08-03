/*
* Copyright (c) 2017-2019 Ambarella, Inc.
*
* This file and its contents ("Software") are protected by intellectual property
* rights including, without limitation, U.S. and/or foreign copyrights. This
* Software is also the confidential and proprietary information of Ambarella,
* Inc. and its licensors. You may not use, reproduce, disclose, distribute,
* modify, or otherwise prepare derivative works of this Software or any portion
* thereof except pursuant to a signed license agreement or nondisclosure
* agreement with Ambarella, Inc. or its authorized affiliates. In the absence of
* such an agreement, you agree to promptly notify and return this Software to
* Ambarella, Inc.
*
* THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
* INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
* MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
* EVENT SHALL AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
* BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
* DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
* LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
* OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
* DAMAGE.
*/

#ifndef _MNET_SSD_ADAS_FLEX_PIC_AG_H_
#define _MNET_SSD_ADAS_FLEX_PIC_AG_H_

// Enable printing
#define CVTASK_PRINT

// Enable/disable poking
#undef __PVCN_1769__DRAM_PITCH_MODIFIABLE
#undef CONV5_1__DW_____BN___MULI___60_____BN_____SCALE_MUL_____SCALE___RELU5_1__DW_DRAM_PITCH_MODIFIABLE
#undef CONV5_4__DW_____BN___MULI___84_____BN_____SCALE_MUL_____SCALE___MULI___423_DRAM_PITCH_MODIFIABLE
#undef CONV5_5__SEP_____BN___MULI___96_____BN_____SCALE_MUL_____SCALE___RELU5_5__SEP_DRAM_PITCH_MODIFIABLE
#undef CONV5_5_MBOX_CONF___CONV2I___99______LOC___CONV2I___171____DRAM_PITCH_MODIFIABLE
#undef CONV5_5_MBOX_LOC_FLAT_DRAM_PITCH_MODIFIABLE
#undef CONV6_MBOX_CONF___CONV2I___117______7_1_____BN___MULI___120_____BN_____SCALE_MUL_____SCALE___LOC___CONV2I___173____DRAM_PITCH_MODIFIABLE
#undef CONV6_MBOX_LOC_FLAT_DRAM_PITCH_MODIFIABLE
#undef CONV7_2_MBOX_CONF___CONV2I___127______8_1_____BN___MULI___130_____BN_____SCALE_MUL_____SCALE___LOC___CONV2I___175____DRAM_PITCH_MODIFIABLE
#undef CONV8_2_MBOX_CONF___CONV2I___137______9_1_____BN___MULI___140_____BN_____SCALE_MUL_____SCALE___LOC___CONV2I___177____DRAM_PITCH_MODIFIABLE
#undef CONV9_2_MBOX_CONF___CONV2I___147______10_1_____BN___MULI___150_____BN_____SCALE_MUL_____SCALE___LOC___CONV2I___179____DRAM_PITCH_MODIFIABLE
#define DATA_DRAM_PITCH_MODIFIABLE
#define DATA_UV_DRAM_PITCH_MODIFIABLE
#undef MBOX_CONF_FLATTEN_DRAM_PITCH_MODIFIABLE
#undef MBOX_LOC_DRAM_PITCH_MODIFIABLE
#undef RELU10_1_DRAM_PITCH_MODIFIABLE
#undef RELU7_1_DRAM_PITCH_MODIFIABLE

// for errcode_enum_t
#include <cvtask_interface.h>

/* Containers for memory buffers */
typedef struct mnet_ssd_adas_flex_pic_ag_DRAM_temporary_scratchpad         mnet_ssd_adas_flex_pic_ag_dram_t;
typedef struct mnet_ssd_adas_flex_pic_ag_CMEM_temporary_scratchpad         mnet_ssd_adas_flex_pic_ag_cmem_t;
typedef struct mnet_ssd_adas_flex_pic_ag_instance_private_storage          mnet_ssd_adas_flex_pic_ag_ips_t;
typedef struct mnet_ssd_adas_flex_pic_ag_instance_private_uncached_storage mnet_ssd_adas_flex_pic_ag_ipus_t;

/* Arguments to mnet_ssd_adas_flex_pic_ag_run/init() */
typedef struct mnet_ssd_adas_flex_pic_ag_required_fields {
    mnet_ssd_adas_flex_pic_ag_dram_t *DRAM_temporary_scratchpad;
    mnet_ssd_adas_flex_pic_ag_cmem_t *CMEM_temporary_scratchpad;
    mnet_ssd_adas_flex_pic_ag_ipus_t *instance_private_uncached_storage;
    uint32_t data_uv_addr;
    uint32_t data_addr;
    uint32_t mbox_conf_flatten_addr;
    uint32_t mbox_loc_addr;
} mnet_ssd_adas_flex_pic_ag_required_fields_t;

/* Arguments to mnet_ssd_adas_flex_pic_ag_run/reset() */
typedef struct mnet_ssd_adas_flex_pic_ag_optional_fields {
#ifdef __PVCN_1769__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_1769__dram_pitch_enable;
    uint32_t __pvcn_1769__dram_pitch_value;
#endif
#ifdef CONV5_1__DW_____BN___MULI___60_____BN_____SCALE_MUL_____SCALE___RELU5_1__DW_DRAM_PITCH_MODIFIABLE
    uint32_t conv5_1__dw_____bn___muli___60_____bn_____scale_mul_____scale___relu5_1__dw_dram_pitch_enable;
    uint32_t conv5_1__dw_____bn___muli___60_____bn_____scale_mul_____scale___relu5_1__dw_dram_pitch_value;
#endif
#ifdef CONV5_4__DW_____BN___MULI___84_____BN_____SCALE_MUL_____SCALE___MULI___423_DRAM_PITCH_MODIFIABLE
    uint32_t conv5_4__dw_____bn___muli___84_____bn_____scale_mul_____scale___muli___423_dram_pitch_enable;
    uint32_t conv5_4__dw_____bn___muli___84_____bn_____scale_mul_____scale___muli___423_dram_pitch_value;
#endif
#ifdef CONV5_5__SEP_____BN___MULI___96_____BN_____SCALE_MUL_____SCALE___RELU5_5__SEP_DRAM_PITCH_MODIFIABLE
    uint32_t conv5_5__sep_____bn___muli___96_____bn_____scale_mul_____scale___relu5_5__sep_dram_pitch_enable;
    uint32_t conv5_5__sep_____bn___muli___96_____bn_____scale_mul_____scale___relu5_5__sep_dram_pitch_value;
#endif
#ifdef CONV5_5_MBOX_CONF___CONV2I___99______LOC___CONV2I___171____DRAM_PITCH_MODIFIABLE
    uint32_t conv5_5_mbox_conf___conv2i___99______loc___conv2i___171____dram_pitch_enable;
    uint32_t conv5_5_mbox_conf___conv2i___99______loc___conv2i___171____dram_pitch_value;
#endif
#ifdef CONV5_5_MBOX_LOC_FLAT_DRAM_PITCH_MODIFIABLE
    uint32_t conv5_5_mbox_loc_flat_dram_pitch_enable;
    uint32_t conv5_5_mbox_loc_flat_dram_pitch_value;
#endif
#ifdef CONV6_MBOX_CONF___CONV2I___117______7_1_____BN___MULI___120_____BN_____SCALE_MUL_____SCALE___LOC___CONV2I___173____DRAM_PITCH_MODIFIABLE
    uint32_t conv6_mbox_conf___conv2i___117______7_1_____bn___muli___120_____bn_____scale_mul_____scale___loc___conv2i___173____dram_pitch_enable;
    uint32_t conv6_mbox_conf___conv2i___117______7_1_____bn___muli___120_____bn_____scale_mul_____scale___loc___conv2i___173____dram_pitch_value;
#endif
#ifdef CONV6_MBOX_LOC_FLAT_DRAM_PITCH_MODIFIABLE
    uint32_t conv6_mbox_loc_flat_dram_pitch_enable;
    uint32_t conv6_mbox_loc_flat_dram_pitch_value;
#endif
#ifdef CONV7_2_MBOX_CONF___CONV2I___127______8_1_____BN___MULI___130_____BN_____SCALE_MUL_____SCALE___LOC___CONV2I___175____DRAM_PITCH_MODIFIABLE
    uint32_t conv7_2_mbox_conf___conv2i___127______8_1_____bn___muli___130_____bn_____scale_mul_____scale___loc___conv2i___175____dram_pitch_enable;
    uint32_t conv7_2_mbox_conf___conv2i___127______8_1_____bn___muli___130_____bn_____scale_mul_____scale___loc___conv2i___175____dram_pitch_value;
#endif
#ifdef CONV8_2_MBOX_CONF___CONV2I___137______9_1_____BN___MULI___140_____BN_____SCALE_MUL_____SCALE___LOC___CONV2I___177____DRAM_PITCH_MODIFIABLE
    uint32_t conv8_2_mbox_conf___conv2i___137______9_1_____bn___muli___140_____bn_____scale_mul_____scale___loc___conv2i___177____dram_pitch_enable;
    uint32_t conv8_2_mbox_conf___conv2i___137______9_1_____bn___muli___140_____bn_____scale_mul_____scale___loc___conv2i___177____dram_pitch_value;
#endif
#ifdef CONV9_2_MBOX_CONF___CONV2I___147______10_1_____BN___MULI___150_____BN_____SCALE_MUL_____SCALE___LOC___CONV2I___179____DRAM_PITCH_MODIFIABLE
    uint32_t conv9_2_mbox_conf___conv2i___147______10_1_____bn___muli___150_____bn_____scale_mul_____scale___loc___conv2i___179____dram_pitch_enable;
    uint32_t conv9_2_mbox_conf___conv2i___147______10_1_____bn___muli___150_____bn_____scale_mul_____scale___loc___conv2i___179____dram_pitch_value;
#endif
#ifdef DATA_DRAM_PITCH_MODIFIABLE
    uint32_t data_dram_pitch_enable;
    uint32_t data_dram_pitch_value;
#endif
#ifdef DATA_UV_DRAM_PITCH_MODIFIABLE
    uint32_t data_uv_dram_pitch_enable;
    uint32_t data_uv_dram_pitch_value;
#endif
#ifdef MBOX_CONF_FLATTEN_DRAM_PITCH_MODIFIABLE
    uint32_t mbox_conf_flatten_dram_pitch_enable;
    uint32_t mbox_conf_flatten_dram_pitch_value;
#endif
#ifdef MBOX_LOC_DRAM_PITCH_MODIFIABLE
    uint32_t mbox_loc_dram_pitch_enable;
    uint32_t mbox_loc_dram_pitch_value;
#endif
#ifdef RELU10_1_DRAM_PITCH_MODIFIABLE
    uint32_t relu10_1_dram_pitch_enable;
    uint32_t relu10_1_dram_pitch_value;
#endif
#ifdef RELU7_1_DRAM_PITCH_MODIFIABLE
    uint32_t relu7_1_dram_pitch_enable;
    uint32_t relu7_1_dram_pitch_value;
#endif
} mnet_ssd_adas_flex_pic_ag_optional_fields_t;

/* Core functions */
// Reads CVTable
errcode_enum_t mnet_ssd_adas_flex_pic_ag_init (
    mnet_ssd_adas_flex_pic_ag_ips_t             *ctxt
);
// run() is NOT re-entrant with respect to run() calls for other DAGs
errcode_enum_t mnet_ssd_adas_flex_pic_ag_run (
    mnet_ssd_adas_flex_pic_ag_ips_t             *ctxt,
    mnet_ssd_adas_flex_pic_ag_required_fields_t *r_args,
    mnet_ssd_adas_flex_pic_ag_optional_fields_t *o_args
);
// Reset all fields to their respective default values
errcode_enum_t mnet_ssd_adas_flex_pic_ag_reset (
    mnet_ssd_adas_flex_pic_ag_optional_fields_t *o_args
);

/* Concretize struct definitions */
#include "mnet_ssd_adas_flex_pic_ag_private.h"
#endif

