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

#ifndef _FLEXIDAG_OPENSEG_AG_H_
#define _FLEXIDAG_OPENSEG_AG_H_

// Enable printing
#define CVTASK_PRINT

// Enable/disable poking
#undef CONV_1__CONV2D___BIASADD___BATCHNORM_1__FUSEDBATCHNORM___MULI___10___BATCHNORM_1__FUSEDBATCHNORM___RELU_1_DRAM_PITCH_MODIFIABLE
#undef CONV_5__CONV2D___BIASADD___BATCHNORM_5__FUSEDBATCHNORM___MULI___156___BATCHNORM_5__FUSEDBATCHNORM___RELU_3_1526__DRAM_PITCH_MODIFIABLE
#undef CONV__CONV2D___BIASADD___BATCHNORM__FUSEDBATCHNORM___MULI___7___BATCHNORM__FUSEDBATCHNORM___PELEENET__STEM_BLOCK__STEM_BLOCK_CONV0__CONV2D___BATCHNORM__FUSEDBATCHNORM_DRAM_PITCH_MODIFIABLE
#undef MUL_2_DRAM_PITCH_MODIFIABLE
#undef PELEENET__STAGE_1_TRANSITION_LAYER__TRANSITION_LAYER_AVGPOOL__AVGPOOL_DRAM_PITCH_MODIFIABLE
#undef PELEENET__STAGE_2_TRANSITION_LAYER__TRANSITION_LAYER_AVGPOOL__AVGPOOL_DRAM_PITCH_MODIFIABLE
#undef PELEENET__STAGE_3_DENSE_BLOCK_1__CONCAT_DRAM_PITCH_MODIFIABLE
#undef PELEENET__STAGE_3_DENSE_BLOCK_4__CONCAT_DRAM_PITCH_MODIFIABLE
#undef PELEENET__STAGE_3_DENSE_BLOCK_7__CONCAT_DRAM_PITCH_MODIFIABLE
#define PLACEHOLDER_DRAM_PITCH_MODIFIABLE
#define PLACEHOLDER_UV_DRAM_PITCH_MODIFIABLE
#undef RESIZEBILINEAR_DRAM_PITCH_MODIFIABLE
#undef CONCAT_1_1522__DRAM_PITCH_MODIFIABLE
#undef CONCAT_1_1527__DRAM_PITCH_MODIFIABLE
#undef CONCAT_DRAM_PITCH_MODIFIABLE
#undef OUT_CLASS_DRAM_PITCH_MODIFIABLE

// for errcode_enum_t
#include <cvtask_interface.h>

/* Containers for memory buffers */
typedef struct flexidag_openseg_ag_DRAM_temporary_scratchpad         flexidag_openseg_ag_dram_t;
typedef struct flexidag_openseg_ag_CMEM_temporary_scratchpad         flexidag_openseg_ag_cmem_t;
typedef struct flexidag_openseg_ag_instance_private_storage          flexidag_openseg_ag_ips_t;
typedef struct flexidag_openseg_ag_instance_private_uncached_storage flexidag_openseg_ag_ipus_t;

/* Arguments to flexidag_openseg_ag_run/init() */
typedef struct flexidag_openseg_ag_required_fields {
    flexidag_openseg_ag_dram_t *DRAM_temporary_scratchpad;
    flexidag_openseg_ag_cmem_t *CMEM_temporary_scratchpad;
    flexidag_openseg_ag_ipus_t *instance_private_uncached_storage;
    uint32_t Placeholder_addr;
    uint32_t Placeholder_uv_addr;
    uint32_t out_class_addr;
} flexidag_openseg_ag_required_fields_t;

/* Arguments to flexidag_openseg_ag_run/reset() */
typedef struct flexidag_openseg_ag_optional_fields {
#ifdef CONV_1__CONV2D___BIASADD___BATCHNORM_1__FUSEDBATCHNORM___MULI___10___BATCHNORM_1__FUSEDBATCHNORM___RELU_1_DRAM_PITCH_MODIFIABLE
    uint32_t Conv_1__Conv2D___BiasAdd___BatchNorm_1__FusedBatchNorm___muli___10___BatchNorm_1__FusedBatchNorm___Relu_1_dram_pitch_enable;
    uint32_t Conv_1__Conv2D___BiasAdd___BatchNorm_1__FusedBatchNorm___muli___10___BatchNorm_1__FusedBatchNorm___Relu_1_dram_pitch_value;
#endif
#ifdef CONV_5__CONV2D___BIASADD___BATCHNORM_5__FUSEDBATCHNORM___MULI___156___BATCHNORM_5__FUSEDBATCHNORM___RELU_3_1526__DRAM_PITCH_MODIFIABLE
    uint32_t Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___156___BatchNorm_5__FusedBatchNorm___Relu_3_1526__dram_pitch_enable;
    uint32_t Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___156___BatchNorm_5__FusedBatchNorm___Relu_3_1526__dram_pitch_value;
#endif
#ifdef CONV__CONV2D___BIASADD___BATCHNORM__FUSEDBATCHNORM___MULI___7___BATCHNORM__FUSEDBATCHNORM___PELEENET__STEM_BLOCK__STEM_BLOCK_CONV0__CONV2D___BATCHNORM__FUSEDBATCHNORM_DRAM_PITCH_MODIFIABLE
    uint32_t Conv__Conv2D___BiasAdd___BatchNorm__FusedBatchNorm___muli___7___BatchNorm__FusedBatchNorm___PeleeNet__stem_block__stem_block_conv0__Conv2D___BatchNorm__FusedBatchNorm_dram_pitch_enable;
    uint32_t Conv__Conv2D___BiasAdd___BatchNorm__FusedBatchNorm___muli___7___BatchNorm__FusedBatchNorm___PeleeNet__stem_block__stem_block_conv0__Conv2D___BatchNorm__FusedBatchNorm_dram_pitch_value;
#endif
#ifdef MUL_2_DRAM_PITCH_MODIFIABLE
    uint32_t Mul_2_dram_pitch_enable;
    uint32_t Mul_2_dram_pitch_value;
#endif
#ifdef PELEENET__STAGE_1_TRANSITION_LAYER__TRANSITION_LAYER_AVGPOOL__AVGPOOL_DRAM_PITCH_MODIFIABLE
    uint32_t PeleeNet__stage_1_transition_layer__transition_layer_avgpool__AvgPool_dram_pitch_enable;
    uint32_t PeleeNet__stage_1_transition_layer__transition_layer_avgpool__AvgPool_dram_pitch_value;
#endif
#ifdef PELEENET__STAGE_2_TRANSITION_LAYER__TRANSITION_LAYER_AVGPOOL__AVGPOOL_DRAM_PITCH_MODIFIABLE
    uint32_t PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_dram_pitch_enable;
    uint32_t PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_dram_pitch_value;
#endif
#ifdef PELEENET__STAGE_3_DENSE_BLOCK_1__CONCAT_DRAM_PITCH_MODIFIABLE
    uint32_t PeleeNet__stage_3_dense_block_1__concat_dram_pitch_enable;
    uint32_t PeleeNet__stage_3_dense_block_1__concat_dram_pitch_value;
#endif
#ifdef PELEENET__STAGE_3_DENSE_BLOCK_4__CONCAT_DRAM_PITCH_MODIFIABLE
    uint32_t PeleeNet__stage_3_dense_block_4__concat_dram_pitch_enable;
    uint32_t PeleeNet__stage_3_dense_block_4__concat_dram_pitch_value;
#endif
#ifdef PELEENET__STAGE_3_DENSE_BLOCK_7__CONCAT_DRAM_PITCH_MODIFIABLE
    uint32_t PeleeNet__stage_3_dense_block_7__concat_dram_pitch_enable;
    uint32_t PeleeNet__stage_3_dense_block_7__concat_dram_pitch_value;
#endif
#ifdef PLACEHOLDER_DRAM_PITCH_MODIFIABLE
    uint32_t Placeholder_dram_pitch_enable;
    uint32_t Placeholder_dram_pitch_value;
#endif
#ifdef PLACEHOLDER_UV_DRAM_PITCH_MODIFIABLE
    uint32_t Placeholder_uv_dram_pitch_enable;
    uint32_t Placeholder_uv_dram_pitch_value;
#endif
#ifdef RESIZEBILINEAR_DRAM_PITCH_MODIFIABLE
    uint32_t ResizeBilinear_dram_pitch_enable;
    uint32_t ResizeBilinear_dram_pitch_value;
#endif
#ifdef CONCAT_1_1522__DRAM_PITCH_MODIFIABLE
    uint32_t concat_1_1522__dram_pitch_enable;
    uint32_t concat_1_1522__dram_pitch_value;
#endif
#ifdef CONCAT_1_1527__DRAM_PITCH_MODIFIABLE
    uint32_t concat_1_1527__dram_pitch_enable;
    uint32_t concat_1_1527__dram_pitch_value;
#endif
#ifdef CONCAT_DRAM_PITCH_MODIFIABLE
    uint32_t concat_dram_pitch_enable;
    uint32_t concat_dram_pitch_value;
#endif
#ifdef OUT_CLASS_DRAM_PITCH_MODIFIABLE
    uint32_t out_class_dram_pitch_enable;
    uint32_t out_class_dram_pitch_value;
#endif
} flexidag_openseg_ag_optional_fields_t;

/* Core functions */
// Reads CVTable
errcode_enum_t flexidag_openseg_ag_init (
    flexidag_openseg_ag_ips_t             *ctxt
);
// run() is NOT re-entrant with respect to run() calls for other DAGs
errcode_enum_t flexidag_openseg_ag_run (
    flexidag_openseg_ag_ips_t             *ctxt,
    flexidag_openseg_ag_required_fields_t *r_args,
    flexidag_openseg_ag_optional_fields_t *o_args
);
// Reset all fields to their respective default values
errcode_enum_t flexidag_openseg_ag_reset (
    flexidag_openseg_ag_optional_fields_t *o_args
);

/* Concretize struct definitions */
#include "flexidag_openseg_ag_private.h"
#endif

