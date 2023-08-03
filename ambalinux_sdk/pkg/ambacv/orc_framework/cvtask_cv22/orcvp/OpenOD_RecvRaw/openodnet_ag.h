/*
 * Copyright (c) 2017-2022 Ambarella International LP
 *
 * This file and its contents ("Software") are protected by intellectual
 * property rights including, without limitation, U.S. and/or foreign
 * copyrights. This Software is also the confidential and proprietary
 * information of Ambarella International LP and its licensors. You may not use,
 * reproduce, disclose, distribute, modify, or otherwise prepare derivative
 * works of this Software or any portion thereof except pursuant to a signed
 * license agreement or nondisclosure agreement with Ambarella International LP
 * or its authorized affiliates. In the absence of such an agreement, you agree
 * to promptly notify and return this Software to Ambarella International LP.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 * MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _OPENODNET_AG_H_
#define _OPENODNET_AG_H_

// Disable printing
#undef CVTASK_PRINT

// Enable/disable poking
#undef ELT5____RELU_DRAM_PITCH_MODIFIABLE
#undef P4____BN_P4_SCALE_MUL___MULI___445____BN_P4_SCALE_MUL____SCALE____RELU___MULI___1229_DRAM_PITCH_MODIFIABLE
#undef P5____BN_P5_SCALE_MUL___MULI___437____BN_P5_SCALE_MUL____SCALE____RELU___MULI___1228_DRAM_PITCH_MODIFIABLE
#undef P6____BN_P6_SCALE_MUL___MULI___411____BN_P6_SCALE_MUL____SCALE____RELU___MULI___1227_DRAM_PITCH_MODIFIABLE
#undef __PVCN_1443__DRAM_PITCH_MODIFIABLE
#undef __PVCN_1569__DRAM_PITCH_MODIFIABLE
#undef ARM_CONF_FLATTEN_DRAM_PITCH_MODIFIABLE
#undef ARM_LOC_DRAM_PITCH_MODIFIABLE
#define DATA_DRAM_PITCH_MODIFIABLE
#define DATA_UV_DRAM_PITCH_MODIFIABLE
#undef ODM_CONF_FLATTEN_DRAM_PITCH_MODIFIABLE
#undef ODM_LOC_DRAM_PITCH_MODIFIABLE
#undef STAGE2_1__CONCAT_DRAM_PITCH_MODIFIABLE
#undef STAGE2_TB_____BN_STAGE2_TB__SCALE_MUL___MULI___138_____BN_STAGE2_TB__SCALE_MUL_____SCALE_____RELU_DRAM_PITCH_MODIFIABLE
#undef STAGE2_TB_MBOX_CONF_FLAT_DRAM_PITCH_MODIFIABLE
#undef STAGE2_TB_MBOX_LOC___CONV2I___397______CONF___CONV2I___140______MULI___1176___7_DRAM_PITCH_MODIFIABLE
#undef STAGE2_TB_MBOX_LOC_FLAT___MULI___1244_DRAM_PITCH_MODIFIABLE
#undef STAGE3_2__CONCAT_DRAM_PITCH_MODIFIABLE
#undef STAGE3_8__CONCAT_DRAM_PITCH_MODIFIABLE
#undef STAGE3_TB_____BN_STAGE3_TB__SCALE_MUL___MULI___272_____BN_STAGE3_TB__SCALE_MUL_____SCALE_____RELU_DRAM_PITCH_MODIFIABLE
#undef STAGE3_TB_MBOX_LOC___CONV2I___399______CONF___CONV2I___274______MULI___1202_DRAM_PITCH_MODIFIABLE
#undef STAGE3_TB_MBOX_LOC_FLAT___MULI___1245_DRAM_PITCH_MODIFIABLE
#undef STAGE4_5__BRANCH2C_____BN_STAGE4_5__BRANCH2C__SCALE_MUL___MULI___354_____BN_STAGE4_5__BRANCH2C__SCALE_MUL_____SCALE_____RELU_DRAM_PITCH_MODIFIABLE
#undef STAGE4_TB_____BN_STAGE4_TB__SCALE_MUL___MULI___374_____BN_STAGE4_TB__SCALE_MUL_____SCALE_____RELU_DRAM_PITCH_MODIFIABLE
#undef STAGE4_TB__RELU__EXT1__FE1_1____BN_STAGE4_TB__RELU__EXT1__FE1_1_SCALE_MUL___MULI___379____BN_STAGE4_TB__RELU__EXT1__FE1_1_SCALE_MUL____SCALE____RELU_DRAM_PITCH_MODIFIABLE
#undef STAGE4_TB_MBOX_LOC___CONV2I___401______CONF___CONV2I___376______MULI___1222_DRAM_PITCH_MODIFIABLE
#undef STAGE4_TB_MBOX_LOC_FLAT___MULI___1246_DRAM_PITCH_MODIFIABLE

// for errcode_enum_t
#include <cvtask_interface.h>

/* Containers for memory buffers */
typedef struct openodnet_ag_DRAM_temporary_scratchpad         openodnet_ag_dram_t;
typedef struct openodnet_ag_CMEM_temporary_scratchpad         openodnet_ag_cmem_t;
typedef struct openodnet_ag_instance_private_storage          openodnet_ag_ips_t;
typedef struct openodnet_ag_instance_private_uncached_storage openodnet_ag_ipus_t;

/* Arguments to openodnet_ag_run/init() */
typedef struct openodnet_ag_required_fields {
    openodnet_ag_dram_t *DRAM_temporary_scratchpad;
    openodnet_ag_cmem_t *CMEM_temporary_scratchpad;
    openodnet_ag_ipus_t *instance_private_uncached_storage;
    uint32_t data_addr;
    uint32_t data_uv_addr;
    uint32_t arm_loc_addr;
    uint32_t arm_conf_flatten_addr;
    uint32_t odm_loc_addr;
    uint32_t odm_conf_flatten_addr;
} openodnet_ag_required_fields_t;

/* Arguments to openodnet_ag_run/reset() */
typedef struct openodnet_ag_optional_fields {
#ifdef ELT5____RELU_DRAM_PITCH_MODIFIABLE
    uint32_t Elt5____relu_dram_pitch_enable;
    uint32_t Elt5____relu_dram_pitch_value;
#endif
#ifdef P4____BN_P4_SCALE_MUL___MULI___445____BN_P4_SCALE_MUL____SCALE____RELU___MULI___1229_DRAM_PITCH_MODIFIABLE
    uint32_t P4____bn_P4_scale_mul___muli___445____bn_P4_scale_mul____scale____relu___muli___1229_dram_pitch_enable;
    uint32_t P4____bn_P4_scale_mul___muli___445____bn_P4_scale_mul____scale____relu___muli___1229_dram_pitch_value;
#endif
#ifdef P5____BN_P5_SCALE_MUL___MULI___437____BN_P5_SCALE_MUL____SCALE____RELU___MULI___1228_DRAM_PITCH_MODIFIABLE
    uint32_t P5____bn_P5_scale_mul___muli___437____bn_P5_scale_mul____scale____relu___muli___1228_dram_pitch_enable;
    uint32_t P5____bn_P5_scale_mul___muli___437____bn_P5_scale_mul____scale____relu___muli___1228_dram_pitch_value;
#endif
#ifdef P6____BN_P6_SCALE_MUL___MULI___411____BN_P6_SCALE_MUL____SCALE____RELU___MULI___1227_DRAM_PITCH_MODIFIABLE
    uint32_t P6____bn_P6_scale_mul___muli___411____bn_P6_scale_mul____scale____relu___muli___1227_dram_pitch_enable;
    uint32_t P6____bn_P6_scale_mul___muli___411____bn_P6_scale_mul____scale____relu___muli___1227_dram_pitch_value;
#endif
#ifdef __PVCN_1443__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_1443__dram_pitch_enable;
    uint32_t __pvcn_1443__dram_pitch_value;
#endif
#ifdef __PVCN_1569__DRAM_PITCH_MODIFIABLE
    uint32_t __pvcn_1569__dram_pitch_enable;
    uint32_t __pvcn_1569__dram_pitch_value;
#endif
#ifdef ARM_CONF_FLATTEN_DRAM_PITCH_MODIFIABLE
    uint32_t arm_conf_flatten_dram_pitch_enable;
    uint32_t arm_conf_flatten_dram_pitch_value;
#endif
#ifdef ARM_LOC_DRAM_PITCH_MODIFIABLE
    uint32_t arm_loc_dram_pitch_enable;
    uint32_t arm_loc_dram_pitch_value;
#endif
#ifdef DATA_DRAM_PITCH_MODIFIABLE
    uint32_t data_dram_pitch_enable;
    uint32_t data_dram_pitch_value;
#endif
#ifdef DATA_UV_DRAM_PITCH_MODIFIABLE
    uint32_t data_uv_dram_pitch_enable;
    uint32_t data_uv_dram_pitch_value;
#endif
#ifdef ODM_CONF_FLATTEN_DRAM_PITCH_MODIFIABLE
    uint32_t odm_conf_flatten_dram_pitch_enable;
    uint32_t odm_conf_flatten_dram_pitch_value;
#endif
#ifdef ODM_LOC_DRAM_PITCH_MODIFIABLE
    uint32_t odm_loc_dram_pitch_enable;
    uint32_t odm_loc_dram_pitch_value;
#endif
#ifdef STAGE2_1__CONCAT_DRAM_PITCH_MODIFIABLE
    uint32_t stage2_1__concat_dram_pitch_enable;
    uint32_t stage2_1__concat_dram_pitch_value;
#endif
#ifdef STAGE2_TB_____BN_STAGE2_TB__SCALE_MUL___MULI___138_____BN_STAGE2_TB__SCALE_MUL_____SCALE_____RELU_DRAM_PITCH_MODIFIABLE
    uint32_t stage2_tb_____bn_stage2_tb__scale_mul___muli___138_____bn_stage2_tb__scale_mul_____scale_____relu_dram_pitch_enable;
    uint32_t stage2_tb_____bn_stage2_tb__scale_mul___muli___138_____bn_stage2_tb__scale_mul_____scale_____relu_dram_pitch_value;
#endif
#ifdef STAGE2_TB_MBOX_CONF_FLAT_DRAM_PITCH_MODIFIABLE
    uint32_t stage2_tb_mbox_conf_flat_dram_pitch_enable;
    uint32_t stage2_tb_mbox_conf_flat_dram_pitch_value;
#endif
#ifdef STAGE2_TB_MBOX_LOC___CONV2I___397______CONF___CONV2I___140______MULI___1176___7_DRAM_PITCH_MODIFIABLE
    uint32_t stage2_tb_mbox_loc___conv2i___397______conf___conv2i___140______muli___1176___7_dram_pitch_enable;
    uint32_t stage2_tb_mbox_loc___conv2i___397______conf___conv2i___140______muli___1176___7_dram_pitch_value;
#endif
#ifdef STAGE2_TB_MBOX_LOC_FLAT___MULI___1244_DRAM_PITCH_MODIFIABLE
    uint32_t stage2_tb_mbox_loc_flat___muli___1244_dram_pitch_enable;
    uint32_t stage2_tb_mbox_loc_flat___muli___1244_dram_pitch_value;
#endif
#ifdef STAGE3_2__CONCAT_DRAM_PITCH_MODIFIABLE
    uint32_t stage3_2__concat_dram_pitch_enable;
    uint32_t stage3_2__concat_dram_pitch_value;
#endif
#ifdef STAGE3_8__CONCAT_DRAM_PITCH_MODIFIABLE
    uint32_t stage3_8__concat_dram_pitch_enable;
    uint32_t stage3_8__concat_dram_pitch_value;
#endif
#ifdef STAGE3_TB_____BN_STAGE3_TB__SCALE_MUL___MULI___272_____BN_STAGE3_TB__SCALE_MUL_____SCALE_____RELU_DRAM_PITCH_MODIFIABLE
    uint32_t stage3_tb_____bn_stage3_tb__scale_mul___muli___272_____bn_stage3_tb__scale_mul_____scale_____relu_dram_pitch_enable;
    uint32_t stage3_tb_____bn_stage3_tb__scale_mul___muli___272_____bn_stage3_tb__scale_mul_____scale_____relu_dram_pitch_value;
#endif
#ifdef STAGE3_TB_MBOX_LOC___CONV2I___399______CONF___CONV2I___274______MULI___1202_DRAM_PITCH_MODIFIABLE
    uint32_t stage3_tb_mbox_loc___conv2i___399______conf___conv2i___274______muli___1202_dram_pitch_enable;
    uint32_t stage3_tb_mbox_loc___conv2i___399______conf___conv2i___274______muli___1202_dram_pitch_value;
#endif
#ifdef STAGE3_TB_MBOX_LOC_FLAT___MULI___1245_DRAM_PITCH_MODIFIABLE
    uint32_t stage3_tb_mbox_loc_flat___muli___1245_dram_pitch_enable;
    uint32_t stage3_tb_mbox_loc_flat___muli___1245_dram_pitch_value;
#endif
#ifdef STAGE4_5__BRANCH2C_____BN_STAGE4_5__BRANCH2C__SCALE_MUL___MULI___354_____BN_STAGE4_5__BRANCH2C__SCALE_MUL_____SCALE_____RELU_DRAM_PITCH_MODIFIABLE
    uint32_t stage4_5__branch2c_____bn_stage4_5__branch2c__scale_mul___muli___354_____bn_stage4_5__branch2c__scale_mul_____scale_____relu_dram_pitch_enable;
    uint32_t stage4_5__branch2c_____bn_stage4_5__branch2c__scale_mul___muli___354_____bn_stage4_5__branch2c__scale_mul_____scale_____relu_dram_pitch_value;
#endif
#ifdef STAGE4_TB_____BN_STAGE4_TB__SCALE_MUL___MULI___374_____BN_STAGE4_TB__SCALE_MUL_____SCALE_____RELU_DRAM_PITCH_MODIFIABLE
    uint32_t stage4_tb_____bn_stage4_tb__scale_mul___muli___374_____bn_stage4_tb__scale_mul_____scale_____relu_dram_pitch_enable;
    uint32_t stage4_tb_____bn_stage4_tb__scale_mul___muli___374_____bn_stage4_tb__scale_mul_____scale_____relu_dram_pitch_value;
#endif
#ifdef STAGE4_TB__RELU__EXT1__FE1_1____BN_STAGE4_TB__RELU__EXT1__FE1_1_SCALE_MUL___MULI___379____BN_STAGE4_TB__RELU__EXT1__FE1_1_SCALE_MUL____SCALE____RELU_DRAM_PITCH_MODIFIABLE
    uint32_t stage4_tb__relu__ext1__fe1_1____bn_stage4_tb__relu__ext1__fe1_1_scale_mul___muli___379____bn_stage4_tb__relu__ext1__fe1_1_scale_mul____scale____relu_dram_pitch_enable;
    uint32_t stage4_tb__relu__ext1__fe1_1____bn_stage4_tb__relu__ext1__fe1_1_scale_mul___muli___379____bn_stage4_tb__relu__ext1__fe1_1_scale_mul____scale____relu_dram_pitch_value;
#endif
#ifdef STAGE4_TB_MBOX_LOC___CONV2I___401______CONF___CONV2I___376______MULI___1222_DRAM_PITCH_MODIFIABLE
    uint32_t stage4_tb_mbox_loc___conv2i___401______conf___conv2i___376______muli___1222_dram_pitch_enable;
    uint32_t stage4_tb_mbox_loc___conv2i___401______conf___conv2i___376______muli___1222_dram_pitch_value;
#endif
#ifdef STAGE4_TB_MBOX_LOC_FLAT___MULI___1246_DRAM_PITCH_MODIFIABLE
    uint32_t stage4_tb_mbox_loc_flat___muli___1246_dram_pitch_enable;
    uint32_t stage4_tb_mbox_loc_flat___muli___1246_dram_pitch_value;
#endif
} openodnet_ag_optional_fields_t;

/* Core functions */
// Reads CVTable
errcode_enum_t openodnet_ag_init (
    openodnet_ag_ips_t             *ctxt
);
// run() is NOT re-entrant with respect to run() calls for other DAGs
errcode_enum_t openodnet_ag_run (
    openodnet_ag_ips_t             *ctxt,
    openodnet_ag_required_fields_t *r_args,
    openodnet_ag_optional_fields_t *o_args
);
// Reset all fields to their respective default values
errcode_enum_t openodnet_ag_reset (
    openodnet_ag_optional_fields_t *o_args
);

/* Concretize struct definitions */
#include "openodnet_ag_private.h"
#endif

