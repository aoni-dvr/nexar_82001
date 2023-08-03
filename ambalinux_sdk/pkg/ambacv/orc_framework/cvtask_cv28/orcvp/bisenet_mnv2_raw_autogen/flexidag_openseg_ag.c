/*
 * Copyright (c) 2017-2020 Ambarella International LP
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

#include "flexidag_openseg_ag.h"
#include "flexidag_openseg_ag_private.h"

static const uint32_t dag_img_sizes[] = {
    3728U, 
    288U, 
    62856U, 
    115888U, 
    115852U, 
    4444U, 
    9384U, 
    8256U, 
    9056U, 
    15988U, 
    15280U, 
    16920U, 
    18168U, 
    17876U, 
    49876U, 
    3776U, 
    164U, 
    68020U, 
    45580U, 
    40360U, 
    6288U, 
    42888U, 
    292U, 
    45340U, 
    292U, 
    48136U, 
    292U, 
    50136U, 
    292U, 
    52488U, 
    292U, 
    165292U, 
    6856U, 
    156048U, 
    200U, 
    220U, 
    216U, 
    200U, 
    200U, 
    348U, 
    31092U, 
    352U, 
    31184U, 
    264U, 
    260U, 
    31184U, 
    264U, 
    260U, 
    31184U, 
    264U, 
    260U, 
    31184U, 
    264U, 
    260U, 
    31184U, 
    264U, 
    260U, 
    31184U, 
    264U, 
    260U, 
    31184U, 
    264U, 
    260U, 
    31184U, 
    264U, 
    260U, 
    31184U, 
    712U, 
    1316U, 
    472U, 
};

static const uint32_t vmem_dag_bases[] = {
    496616U, 
    98400U, 
    450144U, 
    380768U, 
    371808U, 
    505312U, 
    490720U, 
    501472U, 
    505120U, 
    475232U, 
    489440U, 
    472672U, 
    484064U, 
    492128U, 
    458848U, 
    450656U, 
    36960U, 
    446332U, 
    465120U, 
    472544U, 
    470112U, 
    428384U, 
    92256U, 
    459616U, 
    96352U, 
    463200U, 
    100448U, 
    464608U, 
    104544U, 
    461152U, 
    108640U, 
    340064U, 
    471136U, 
    210220U, 
    203368U, 
    369248U, 
    499820U, 
    403552U, 
    405600U, 
    499808U, 
    481128U, 
    511584U, 
    482024U, 
    500512U, 
    477856U, 
    482024U, 
    500512U, 
    477856U, 
    482024U, 
    500512U, 
    477856U, 
    482024U, 
    500512U, 
    477856U, 
    482024U, 
    500512U, 
    477856U, 
    482024U, 
    500512U, 
    477856U, 
    482024U, 
    500512U, 
    477856U, 
    482024U, 
    500512U, 
    477856U, 
    482024U, 
    75032U, 
    81224U, 
    255828U, 
};

static const uint32_t vmem_dagbin_bases[] = {
    499624U, 
    98400U, 
    512592U, 
    496132U, 
    487340U, 
    509332U, 
    499176U, 
    508924U, 
    513372U, 
    490004U, 
    503916U, 
    488788U, 
    501428U, 
    509272U, 
    508192U, 
    453344U, 
    36960U, 
    513036U, 
    509436U, 
    511896U, 
    475980U, 
    470476U, 
    92256U, 
    504100U, 
    96352U, 
    510480U, 
    100448U, 
    513888U, 
    104544U, 
    512844U, 
    108640U, 
    505112U, 
    476552U, 
    365388U, 
    203368U, 
    369248U, 
    499836U, 
    403552U, 
    405600U, 
    499808U, 
    511880U, 
    511584U, 
    512776U, 
    500512U, 
    477856U, 
    512776U, 
    500512U, 
    477856U, 
    512776U, 
    500512U, 
    477856U, 
    512776U, 
    500512U, 
    477856U, 
    512776U, 
    500512U, 
    477856U, 
    512776U, 
    500512U, 
    477856U, 
    512776U, 
    500512U, 
    477856U, 
    512776U, 
    500512U, 
    477856U, 
    512776U, 
    75032U, 
    81384U, 
    255908U, 
};

/******************************* CORE FUNCTIONS *******************************/
/* Populate structs with constants
 * Calls the wrapper function for cvtable_find()
 */
errcode_enum_t flexidag_openseg_ag_init (
    flexidag_openseg_ag_ips_t *ctxt
) {
    errcode_enum_t status = ERRCODE_NONE;

    if (!ctxt) {
        status = ERRCODE_BAD_PARAMETER;
        cvtask_printf(LVL_CRITICAL, " >>> ** FATAL! Context pointer is NULL",
            0, 0, 0, 0, 0);
        return status;
    }

    ctxt->split_id = 0U;

    // Find dram_dag_base for each split
    status = flexidag_openseg_ag_read_cvtable(ctxt);
    if (is_err(status)) {
        cvtask_printf(LVL_DEBUG, "flexidag_openseg_ag_read_cvtable() has failed.", 0, 0
            , 0, 0, 0);
        return status;
    }

    return status;
} // end of flexidag_openseg_ag_init()

/* Execute VP DAGs
 * run() is NOT re-entrant with respect to run() calls for other DAGs
 */
errcode_enum_t flexidag_openseg_ag_run (
    flexidag_openseg_ag_ips_t             *ctxt,
    flexidag_openseg_ag_required_fields_t *r_args,
    flexidag_openseg_ag_optional_fields_t *o_args
) {
    int split_id = 0;
    errcode_enum_t status = ERRCODE_NONE;
    struct flexidag_openseg_ag_split_context *split_ctxt = NULL;
#ifdef CVTASK_PRINT
    uint32_t time_start, time_end;
#endif

    if (!ctxt || !r_args || !o_args) {
        status = ERRCODE_BAD_PARAMETER;
        cvtask_printf(LVL_DEBUG, "  > Error: NULL pointer(s).", 0, 0, 0, 0, 0);
        return status;
    }
    if(!r_args->instance_private_uncached_storage) {
        status = ERRCODE_BAD_PARAMETER;
        cvtask_printf(LVL_DEBUG, "  > Error: NULL pointer(s).", 0, 0, 0, 0, 0);
        return status;
    }

    split_id = ctxt->split_id;
#ifdef CVTASK_PRINT
    time_start = get_cur_time();
#endif

    // Initialize VP
    vp_init();

    for (; split_id < FLEXIDAG_OPENSEG_AG_SPLIT_COUNT; split_id++) {
        split_ctxt = &ctxt->split_ctxt[split_id];
        flexidag_openseg_ag_load_split(split_ctxt, split_id);
        wait(VIS_W_COPROC); // wait on dummy run

        flexidag_openseg_ag_poke_split(r_args, o_args, split_ctxt, split_id);
#ifdef CVTASK_PRINT
        time_end = get_cur_time();
        cvtask_printf(LVL_NORMAL,
            "  >> done loading split-#%03u.     cycles=(%u)",
            split_id, time_end - time_start, 0, 0, 0);
        time_start = time_end;
#endif
        flexidag_openseg_ag_run_split(split_id);
        flexidag_openseg_ag_peek_split(r_args, split_ctxt, split_id);

        wait(VIS_W_COPROC); // wait on coproc_run()
        status |= flexidag_openseg_ag_vp_status();

#ifdef CVTASK_PRINT
        time_end = get_cur_time();
        cvtask_printf(LVL_NORMAL,
            "  >> done executing split-#%03u.   cycles=(%u)",
            split_id, time_end - time_start, 0, 0, 0);
        time_start = time_end;
#endif
        
        if (cvtask_check_yield()) {
            split_id += 1U;
            ctxt->split_id = split_id;
            return RETCODE_CVTASK_YIELDED;
        }

    }

    ctxt->split_id = 0U;

    return status;
} // end of flexidag_openseg_ag_run()

/* Populate struct with default values */
errcode_enum_t flexidag_openseg_ag_reset(
    flexidag_openseg_ag_optional_fields_t *o_args
) {
    errcode_enum_t status = ERRCODE_NONE;

    if (!o_args) {
        status = ERRCODE_BAD_PARAMETER;
        cvtask_printf(LVL_DEBUG, "  > Error: NULL pointer(s).", 0, 0, 0, 0, 0);
        return status;
    }

#ifdef ADD_DRAM_PITCH_MODIFIABLE
    o_args->Add_dram_pitch_enable = 1U;
    o_args->Add_dram_pitch_value = 64U;
#endif
#ifdef CONV_1__CONV2D___BIASADD___BATCHNORM_1__FUSEDBATCHNORM___MULI___10___BATCHNORM_1__FUSEDBATCHNORM___RELU_1_DRAM_PITCH_MODIFIABLE
    o_args->Conv_1__Conv2D___BiasAdd___BatchNorm_1__FusedBatchNorm___muli___10___BatchNorm_1__FusedBatchNorm___Relu_1_dram_pitch_enable = 1U;
    o_args->Conv_1__Conv2D___BiasAdd___BatchNorm_1__FusedBatchNorm___muli___10___BatchNorm_1__FusedBatchNorm___Relu_1_dram_pitch_value = 64U;
#endif
#ifdef CONV_2__CONV2D___BIASADD___BATCHNORM_2__FUSEDBATCHNORM___MULI___13___BATCHNORM_2__FUSEDBATCHNORM___RELU_2_DRAM_PITCH_MODIFIABLE
    o_args->Conv_2__Conv2D___BiasAdd___BatchNorm_2__FusedBatchNorm___muli___13___BatchNorm_2__FusedBatchNorm___Relu_2_dram_pitch_enable = 1U;
    o_args->Conv_2__Conv2D___BiasAdd___BatchNorm_2__FusedBatchNorm___muli___13___BatchNorm_2__FusedBatchNorm___Relu_2_dram_pitch_value = 64U;
#endif
#ifdef CONV_5__CONV2D___BIASADD___BATCHNORM_5__FUSEDBATCHNORM___MULI___136___BATCHNORM_5__FUSEDBATCHNORM___RELU_3___MULI___492___MEAN_3_SRC0_NHWC___NHWC___TRANSPOSE___138_580000004__DRAM_PITCH_MODIFIABLE
    o_args->Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___136___BatchNorm_5__FusedBatchNorm___Relu_3___muli___492___Mean_3_src0_nhwc___nhwc___transpose___138_580000004__dram_pitch_enable = 1U;
    o_args->Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___136___BatchNorm_5__FusedBatchNorm___Relu_3___muli___492___Mean_3_src0_nhwc___nhwc___transpose___138_580000004__dram_pitch_value = 64U;
#endif
#ifdef CONV_5__CONV2D___BIASADD___BATCHNORM_5__FUSEDBATCHNORM___MULI___136___BATCHNORM_5__FUSEDBATCHNORM___RELU_3___MULI___492___MEAN_3_SRC0_NHWC___NHWC___TRANSPOSE___138_580000009__DRAM_PITCH_MODIFIABLE
    o_args->Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___136___BatchNorm_5__FusedBatchNorm___Relu_3___muli___492___Mean_3_src0_nhwc___nhwc___transpose___138_580000009__dram_pitch_enable = 1U;
    o_args->Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___136___BatchNorm_5__FusedBatchNorm___Relu_3___muli___492___Mean_3_src0_nhwc___nhwc___transpose___138_580000009__dram_pitch_value = 64U;
#endif
#ifdef CONV_5__CONV2D___BIASADD___BATCHNORM_5__FUSEDBATCHNORM___MULI___136___BATCHNORM_5__FUSEDBATCHNORM___RELU_3___MULI___492___MEAN_3_SRC0_NHWC___NHWC___TRANSPOSE___138_580000014__DRAM_PITCH_MODIFIABLE
    o_args->Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___136___BatchNorm_5__FusedBatchNorm___Relu_3___muli___492___Mean_3_src0_nhwc___nhwc___transpose___138_580000014__dram_pitch_enable = 1U;
    o_args->Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___136___BatchNorm_5__FusedBatchNorm___Relu_3___muli___492___Mean_3_src0_nhwc___nhwc___transpose___138_580000014__dram_pitch_value = 64U;
#endif
#ifdef CONV_5__CONV2D___BIASADD___BATCHNORM_5__FUSEDBATCHNORM___MULI___136___BATCHNORM_5__FUSEDBATCHNORM___RELU_3___MULI___492___MEAN_3_SRC0_NHWC___NHWC___TRANSPOSE___138_580000019__DRAM_PITCH_MODIFIABLE
    o_args->Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___136___BatchNorm_5__FusedBatchNorm___Relu_3___muli___492___Mean_3_src0_nhwc___nhwc___transpose___138_580000019__dram_pitch_enable = 1U;
    o_args->Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___136___BatchNorm_5__FusedBatchNorm___Relu_3___muli___492___Mean_3_src0_nhwc___nhwc___transpose___138_580000019__dram_pitch_value = 64U;
#endif
#ifdef CONV_5__CONV2D___BIASADD___BATCHNORM_5__FUSEDBATCHNORM___MULI___136___BATCHNORM_5__FUSEDBATCHNORM___RELU_3___MULI___492___MEAN_3_SRC0_NHWC___NHWC___TRANSPOSE___138_580000024__DRAM_PITCH_MODIFIABLE
    o_args->Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___136___BatchNorm_5__FusedBatchNorm___Relu_3___muli___492___Mean_3_src0_nhwc___nhwc___transpose___138_580000024__dram_pitch_enable = 1U;
    o_args->Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___136___BatchNorm_5__FusedBatchNorm___Relu_3___muli___492___Mean_3_src0_nhwc___nhwc___transpose___138_580000024__dram_pitch_value = 64U;
#endif
#ifdef CONV_5__CONV2D___BIASADD___BATCHNORM_5__FUSEDBATCHNORM___MULI___136___BATCHNORM_5__FUSEDBATCHNORM___RELU_3___MULI___492___MEAN_3_SRC0_NHWC___NHWC___TRANSPOSE___138_580000029__DRAM_PITCH_MODIFIABLE
    o_args->Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___136___BatchNorm_5__FusedBatchNorm___Relu_3___muli___492___Mean_3_src0_nhwc___nhwc___transpose___138_580000029__dram_pitch_enable = 1U;
    o_args->Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___136___BatchNorm_5__FusedBatchNorm___Relu_3___muli___492___Mean_3_src0_nhwc___nhwc___transpose___138_580000029__dram_pitch_value = 64U;
#endif
#ifdef CONV_5__CONV2D___BIASADD___BATCHNORM_5__FUSEDBATCHNORM___MULI___136___BATCHNORM_5__FUSEDBATCHNORM___RELU_3___MULI___492___MEAN_3_SRC0_NHWC___NHWC___TRANSPOSE___138_580000034__DRAM_PITCH_MODIFIABLE
    o_args->Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___136___BatchNorm_5__FusedBatchNorm___Relu_3___muli___492___Mean_3_src0_nhwc___nhwc___transpose___138_580000034__dram_pitch_enable = 1U;
    o_args->Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___136___BatchNorm_5__FusedBatchNorm___Relu_3___muli___492___Mean_3_src0_nhwc___nhwc___transpose___138_580000034__dram_pitch_value = 64U;
#endif
#ifdef CONV_5__CONV2D___BIASADD___BATCHNORM_5__FUSEDBATCHNORM___MULI___136___BATCHNORM_5__FUSEDBATCHNORM___RELU_3___MULI___492___MEAN_3_SRC0_NHWC___NHWC___TRANSPOSE___138_580000039__DRAM_PITCH_MODIFIABLE
    o_args->Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___136___BatchNorm_5__FusedBatchNorm___Relu_3___muli___492___Mean_3_src0_nhwc___nhwc___transpose___138_580000039__dram_pitch_enable = 1U;
    o_args->Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___136___BatchNorm_5__FusedBatchNorm___Relu_3___muli___492___Mean_3_src0_nhwc___nhwc___transpose___138_580000039__dram_pitch_value = 64U;
#endif
#ifdef CONV_5__CONV2D___BIASADD___BATCHNORM_5__FUSEDBATCHNORM___MULI___136___BATCHNORM_5__FUSEDBATCHNORM___RELU_3___MULI___492___MEAN_3_SRC0_NHWC___NHWC___TRANSPOSE___138_580000044__DRAM_PITCH_MODIFIABLE
    o_args->Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___136___BatchNorm_5__FusedBatchNorm___Relu_3___muli___492___Mean_3_src0_nhwc___nhwc___transpose___138_580000044__dram_pitch_enable = 1U;
    o_args->Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___136___BatchNorm_5__FusedBatchNorm___Relu_3___muli___492___Mean_3_src0_nhwc___nhwc___transpose___138_580000044__dram_pitch_value = 64U;
#endif
#ifdef CONV_5__CONV2D___BIASADD___BATCHNORM_5__FUSEDBATCHNORM___MULI___136___BATCHNORM_5__FUSEDBATCHNORM___RELU_3___MULI___492___MEAN_3_SRC0_NHWC___NHWC___TRANSPOSE___138_580000049__DRAM_PITCH_MODIFIABLE
    o_args->Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___136___BatchNorm_5__FusedBatchNorm___Relu_3___muli___492___Mean_3_src0_nhwc___nhwc___transpose___138_580000049__dram_pitch_enable = 1U;
    o_args->Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___136___BatchNorm_5__FusedBatchNorm___Relu_3___muli___492___Mean_3_src0_nhwc___nhwc___transpose___138_580000049__dram_pitch_value = 64U;
#endif
#ifdef CONV_5__CONV2D___BIASADD___BATCHNORM_5__FUSEDBATCHNORM___MULI___136___BATCHNORM_5__FUSEDBATCHNORM___RELU_3___MULI___492___MEAN_3_SRC0_NHWC___NHWC___TRANSPOSE___138_DRAM_PITCH_MODIFIABLE
    o_args->Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___136___BatchNorm_5__FusedBatchNorm___Relu_3___muli___492___Mean_3_src0_nhwc___nhwc___transpose___138_dram_pitch_enable = 1U;
    o_args->Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___136___BatchNorm_5__FusedBatchNorm___Relu_3___muli___492___Mean_3_src0_nhwc___nhwc___transpose___138_dram_pitch_value = 192U;
#endif
#ifdef CONV__CONV2D___BIASADD___BATCHNORM__FUSEDBATCHNORM___MULI___7___BATCHNORM__FUSEDBATCHNORM___PELEENET__STEM_BLOCK__STEM_BLOCK_CONV0__CONV2D___BATCHNORM__FUSEDBATCHNORM___MULI___440___1_DRAM_PITCH_MODIFIABLE
    o_args->Conv__Conv2D___BiasAdd___BatchNorm__FusedBatchNorm___muli___7___BatchNorm__FusedBatchNorm___PeleeNet__stem_block__stem_block_conv0__Conv2D___BatchNorm__FusedBatchNorm___muli___440___1_dram_pitch_enable = 1U;
    o_args->Conv__Conv2D___BiasAdd___BatchNorm__FusedBatchNorm___muli___7___BatchNorm__FusedBatchNorm___PeleeNet__stem_block__stem_block_conv0__Conv2D___BatchNorm__FusedBatchNorm___muli___440___1_dram_pitch_value = 64U;
#endif
#ifdef MUL_1______MULI___491___EAN_2_SRC0_NHWC___NHWC___TRANSPOSE___130_DRAM_PITCH_MODIFIABLE
    o_args->Mul_1______muli___491___ean_2_src0_nhwc___nhwc___transpose___130_dram_pitch_enable = 1U;
    o_args->Mul_1______muli___491___ean_2_src0_nhwc___nhwc___transpose___130_dram_pitch_value = 64U;
#endif
#ifdef MUL_2_DRAM_PITCH_MODIFIABLE
    o_args->Mul_2_dram_pitch_enable = 1U;
    o_args->Mul_2_dram_pitch_value = 64U;
#endif
#ifdef PELEENET__STAGE_1_DENSE_BLOCK_0__CONCAT______MULI___447_DRAM_PITCH_MODIFIABLE
    o_args->PeleeNet__stage_1_dense_block_0__concat______muli___447_dram_pitch_enable = 1U;
    o_args->PeleeNet__stage_1_dense_block_0__concat______muli___447_dram_pitch_value = 64U;
#endif
#ifdef PELEENET__STAGE_1_DENSE_BLOCK_1__CONCAT______MULI___450_DRAM_PITCH_MODIFIABLE
    o_args->PeleeNet__stage_1_dense_block_1__concat______muli___450_dram_pitch_enable = 1U;
    o_args->PeleeNet__stage_1_dense_block_1__concat______muli___450_dram_pitch_value = 64U;
#endif
#ifdef PELEENET__STAGE_1_DENSE_BLOCK_2__CONCAT_DRAM_PITCH_MODIFIABLE
    o_args->PeleeNet__stage_1_dense_block_2__concat_dram_pitch_enable = 1U;
    o_args->PeleeNet__stage_1_dense_block_2__concat_dram_pitch_value = 64U;
#endif
#ifdef PELEENET__STAGE_1_TRANSITION_LAYER__TRANSITION_LAYER_AVGPOOL__AVGPOOL______MULI___453_DRAM_PITCH_MODIFIABLE
    o_args->PeleeNet__stage_1_transition_layer__transition_layer_avgpool__AvgPool______muli___453_dram_pitch_enable = 1U;
    o_args->PeleeNet__stage_1_transition_layer__transition_layer_avgpool__AvgPool______muli___453_dram_pitch_value = 64U;
#endif
#ifdef PELEENET__STAGE_2_DENSE_BLOCK_0__CONCAT______MULI___456_DRAM_PITCH_MODIFIABLE
    o_args->PeleeNet__stage_2_dense_block_0__concat______muli___456_dram_pitch_enable = 1U;
    o_args->PeleeNet__stage_2_dense_block_0__concat______muli___456_dram_pitch_value = 64U;
#endif
#ifdef PELEENET__STAGE_2_DENSE_BLOCK_1__CONCAT______MULI___459_DRAM_PITCH_MODIFIABLE
    o_args->PeleeNet__stage_2_dense_block_1__concat______muli___459_dram_pitch_enable = 1U;
    o_args->PeleeNet__stage_2_dense_block_1__concat______muli___459_dram_pitch_value = 64U;
#endif
#ifdef PELEENET__STAGE_2_DENSE_BLOCK_2__CONCAT___MULI___462_DRAM_PITCH_MODIFIABLE
    o_args->PeleeNet__stage_2_dense_block_2__concat___muli___462_dram_pitch_enable = 1U;
    o_args->PeleeNet__stage_2_dense_block_2__concat___muli___462_dram_pitch_value = 64U;
#endif
#ifdef PELEENET__STAGE_2_DENSE_BLOCK_2__CONCAT_DRAM_PITCH_MODIFIABLE
    o_args->PeleeNet__stage_2_dense_block_2__concat_dram_pitch_enable = 1U;
    o_args->PeleeNet__stage_2_dense_block_2__concat_dram_pitch_value = 64U;
#endif
#ifdef PELEENET__STAGE_2_DENSE_BLOCK_3__CONV_LEFT_1__CONV2D___BATCHNORM__FUSEDBATCHNORM___RELU_DRAM_PITCH_MODIFIABLE
    o_args->PeleeNet__stage_2_dense_block_3__conv_left_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_dram_pitch_enable = 1U;
    o_args->PeleeNet__stage_2_dense_block_3__conv_left_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_dram_pitch_value = 64U;
#endif
#ifdef PELEENET__STAGE_2_DENSE_BLOCK_3__CONV_RIGHT_1__CONV2D___BATCHNORM__FUSEDBATCHNORM___RELU_DRAM_PITCH_MODIFIABLE
    o_args->PeleeNet__stage_2_dense_block_3__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_dram_pitch_enable = 1U;
    o_args->PeleeNet__stage_2_dense_block_3__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_dram_pitch_value = 64U;
#endif
#ifdef PELEENET__STAGE_2_TRANSITION_LAYER__TRANSITION_LAYER_AVGPOOL__AVGPOOL______MULI___465___MEAN_SRC0_NHWC___NHWC___TRANSPOSE___66_1243__DRAM_PITCH_MODIFIABLE
    o_args->PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool______muli___465___Mean_src0_nhwc___nhwc___transpose___66_1243__dram_pitch_enable = 1U;
    o_args->PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool______muli___465___Mean_src0_nhwc___nhwc___transpose___66_1243__dram_pitch_value = 128U;
#endif
#ifdef PELEENET__STAGE_2_TRANSITION_LAYER__TRANSITION_LAYER_AVGPOOL__AVGPOOL______MULI___465___MEAN_SRC0_NHWC___NHWC___TRANSPOSE___66_DRAM_PITCH_MODIFIABLE
    o_args->PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool______muli___465___Mean_src0_nhwc___nhwc___transpose___66_dram_pitch_enable = 1U;
    o_args->PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool______muli___465___Mean_src0_nhwc___nhwc___transpose___66_dram_pitch_value = 64U;
#endif
#ifdef PELEENET__STAGE_2_TRANSITION_LAYER__TRANSITION_LAYER_AVGPOOL__AVGPOOL___MULI___466_DRAM_PITCH_MODIFIABLE
    o_args->PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool___muli___466_dram_pitch_enable = 1U;
    o_args->PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool___muli___466_dram_pitch_value = 64U;
#endif
#ifdef PELEENET__STAGE_2_TRANSITION_LAYER__TRANSITION_LAYER_CONV0__CONV2D___BATCHNORM__FUSEDBATCHNORM___RELU_DRAM_PITCH_MODIFIABLE
    o_args->PeleeNet__stage_2_transition_layer__transition_layer_conv0__Conv2D___BatchNorm__FusedBatchNorm___Relu_dram_pitch_enable = 1U;
    o_args->PeleeNet__stage_2_transition_layer__transition_layer_conv0__Conv2D___BatchNorm__FusedBatchNorm___Relu_dram_pitch_value = 64U;
#endif
#ifdef PELEENET__STAGE_3_DENSE_BLOCK_0__CONCAT______MULI___469_DRAM_PITCH_MODIFIABLE
    o_args->PeleeNet__stage_3_dense_block_0__concat______muli___469_dram_pitch_enable = 1U;
    o_args->PeleeNet__stage_3_dense_block_0__concat______muli___469_dram_pitch_value = 64U;
#endif
#ifdef PELEENET__STAGE_3_DENSE_BLOCK_0__CONV_LEFT_0__CONV2D___BATCHNORM__FUSEDBATCHNORM___RIGHT_0__CONV2D___BATCHNORM__FUSEDBATCHNORM___MULI___467___8_DRAM_PITCH_MODIFIABLE
    o_args->PeleeNet__stage_3_dense_block_0__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm___muli___467___8_dram_pitch_enable = 1U;
    o_args->PeleeNet__stage_3_dense_block_0__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm___muli___467___8_dram_pitch_value = 64U;
#endif
#ifdef PELEENET__STAGE_3_DENSE_BLOCK_0__CONV_RIGHT_0__RELU_DRAM_PITCH_MODIFIABLE
    o_args->PeleeNet__stage_3_dense_block_0__conv_right_0__Relu_dram_pitch_enable = 1U;
    o_args->PeleeNet__stage_3_dense_block_0__conv_right_0__Relu_dram_pitch_value = 64U;
#endif
#ifdef PELEENET__STAGE_3_DENSE_BLOCK_1__CONCAT______MULI___472_DRAM_PITCH_MODIFIABLE
    o_args->PeleeNet__stage_3_dense_block_1__concat______muli___472_dram_pitch_enable = 1U;
    o_args->PeleeNet__stage_3_dense_block_1__concat______muli___472_dram_pitch_value = 64U;
#endif
#ifdef PELEENET__STAGE_3_DENSE_BLOCK_1__CONV_LEFT_0__CONV2D___BATCHNORM__FUSEDBATCHNORM___RIGHT_0__CONV2D___CROP___400_DRAM_PITCH_MODIFIABLE
    o_args->PeleeNet__stage_3_dense_block_1__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___crop___400_dram_pitch_enable = 1U;
    o_args->PeleeNet__stage_3_dense_block_1__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___crop___400_dram_pitch_value = 64U;
#endif
#ifdef PELEENET__STAGE_3_DENSE_BLOCK_1__CONV_RIGHT_2__CONV2D___BATCHNORM__FUSEDBATCHNORM___RELU_DRAM_PITCH_MODIFIABLE
    o_args->PeleeNet__stage_3_dense_block_1__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu_dram_pitch_enable = 1U;
    o_args->PeleeNet__stage_3_dense_block_1__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu_dram_pitch_value = 64U;
#endif
#ifdef PELEENET__STAGE_3_DENSE_BLOCK_2__CONCAT______MULI___475_DRAM_PITCH_MODIFIABLE
    o_args->PeleeNet__stage_3_dense_block_2__concat______muli___475_dram_pitch_enable = 1U;
    o_args->PeleeNet__stage_3_dense_block_2__concat______muli___475_dram_pitch_value = 64U;
#endif
#ifdef PELEENET__STAGE_3_DENSE_BLOCK_2__CONV_LEFT_0__CONV2D___BATCHNORM__FUSEDBATCHNORM___RIGHT_0__CONV2D___CROP___402_DRAM_PITCH_MODIFIABLE
    o_args->PeleeNet__stage_3_dense_block_2__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___crop___402_dram_pitch_enable = 1U;
    o_args->PeleeNet__stage_3_dense_block_2__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___crop___402_dram_pitch_value = 64U;
#endif
#ifdef PELEENET__STAGE_3_DENSE_BLOCK_2__CONV_RIGHT_2__CONV2D___BATCHNORM__FUSEDBATCHNORM___RELU_DRAM_PITCH_MODIFIABLE
    o_args->PeleeNet__stage_3_dense_block_2__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu_dram_pitch_enable = 1U;
    o_args->PeleeNet__stage_3_dense_block_2__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu_dram_pitch_value = 64U;
#endif
#ifdef PELEENET__STAGE_3_DENSE_BLOCK_3__CONCAT______MULI___478_DRAM_PITCH_MODIFIABLE
    o_args->PeleeNet__stage_3_dense_block_3__concat______muli___478_dram_pitch_enable = 1U;
    o_args->PeleeNet__stage_3_dense_block_3__concat______muli___478_dram_pitch_value = 64U;
#endif
#ifdef PELEENET__STAGE_3_DENSE_BLOCK_3__CONV_LEFT_1__CONV2D___BATCHNORM__FUSEDBATCHNORM___RELU_DRAM_PITCH_MODIFIABLE
    o_args->PeleeNet__stage_3_dense_block_3__conv_left_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_dram_pitch_enable = 1U;
    o_args->PeleeNet__stage_3_dense_block_3__conv_left_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_dram_pitch_value = 64U;
#endif
#ifdef PELEENET__STAGE_3_DENSE_BLOCK_3__CONV_RIGHT_2__CONV2D___BATCHNORM__FUSEDBATCHNORM___RELU_DRAM_PITCH_MODIFIABLE
    o_args->PeleeNet__stage_3_dense_block_3__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu_dram_pitch_enable = 1U;
    o_args->PeleeNet__stage_3_dense_block_3__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu_dram_pitch_value = 64U;
#endif
#ifdef PELEENET__STAGE_3_DENSE_BLOCK_4__CONCAT______MULI___481_DRAM_PITCH_MODIFIABLE
    o_args->PeleeNet__stage_3_dense_block_4__concat______muli___481_dram_pitch_enable = 1U;
    o_args->PeleeNet__stage_3_dense_block_4__concat______muli___481_dram_pitch_value = 64U;
#endif
#ifdef PELEENET__STAGE_3_DENSE_BLOCK_4__CONV_LEFT_1__CONV2D___BATCHNORM__FUSEDBATCHNORM___RELU_DRAM_PITCH_MODIFIABLE
    o_args->PeleeNet__stage_3_dense_block_4__conv_left_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_dram_pitch_enable = 1U;
    o_args->PeleeNet__stage_3_dense_block_4__conv_left_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_dram_pitch_value = 64U;
#endif
#ifdef PELEENET__STAGE_3_DENSE_BLOCK_4__CONV_RIGHT_2__CONV2D___BATCHNORM__FUSEDBATCHNORM___RELU_DRAM_PITCH_MODIFIABLE
    o_args->PeleeNet__stage_3_dense_block_4__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu_dram_pitch_enable = 1U;
    o_args->PeleeNet__stage_3_dense_block_4__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu_dram_pitch_value = 64U;
#endif
#ifdef PELEENET__STAGE_3_DENSE_BLOCK_5__CONCAT______MULI___484_DRAM_PITCH_MODIFIABLE
    o_args->PeleeNet__stage_3_dense_block_5__concat______muli___484_dram_pitch_enable = 1U;
    o_args->PeleeNet__stage_3_dense_block_5__concat______muli___484_dram_pitch_value = 64U;
#endif
#ifdef PELEENET__STAGE_3_DENSE_BLOCK_5__CONV_LEFT_1__CONV2D___BATCHNORM__FUSEDBATCHNORM___RELU_DRAM_PITCH_MODIFIABLE
    o_args->PeleeNet__stage_3_dense_block_5__conv_left_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_dram_pitch_enable = 1U;
    o_args->PeleeNet__stage_3_dense_block_5__conv_left_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_dram_pitch_value = 64U;
#endif
#ifdef PELEENET__STAGE_3_DENSE_BLOCK_5__CONV_RIGHT_2__CONV2D___BATCHNORM__FUSEDBATCHNORM___RELU_DRAM_PITCH_MODIFIABLE
    o_args->PeleeNet__stage_3_dense_block_5__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu_dram_pitch_enable = 1U;
    o_args->PeleeNet__stage_3_dense_block_5__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu_dram_pitch_value = 64U;
#endif
#ifdef PELEENET__STAGE_3_DENSE_BLOCK_6__CONCAT______MULI___487_DRAM_PITCH_MODIFIABLE
    o_args->PeleeNet__stage_3_dense_block_6__concat______muli___487_dram_pitch_enable = 1U;
    o_args->PeleeNet__stage_3_dense_block_6__concat______muli___487_dram_pitch_value = 64U;
#endif
#ifdef PELEENET__STAGE_3_DENSE_BLOCK_6__CONV_LEFT_1__CONV2D___BATCHNORM__FUSEDBATCHNORM___RELU_DRAM_PITCH_MODIFIABLE
    o_args->PeleeNet__stage_3_dense_block_6__conv_left_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_dram_pitch_enable = 1U;
    o_args->PeleeNet__stage_3_dense_block_6__conv_left_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_dram_pitch_value = 64U;
#endif
#ifdef PELEENET__STAGE_3_DENSE_BLOCK_6__CONV_RIGHT_2__CONV2D___BATCHNORM__FUSEDBATCHNORM___RELU_DRAM_PITCH_MODIFIABLE
    o_args->PeleeNet__stage_3_dense_block_6__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu_dram_pitch_enable = 1U;
    o_args->PeleeNet__stage_3_dense_block_6__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu_dram_pitch_value = 64U;
#endif
#ifdef PELEENET__STAGE_3_DENSE_BLOCK_7__CONCAT_DRAM_PITCH_MODIFIABLE
    o_args->PeleeNet__stage_3_dense_block_7__concat_dram_pitch_enable = 1U;
    o_args->PeleeNet__stage_3_dense_block_7__concat_dram_pitch_value = 64U;
#endif
#ifdef PELEENET__STAGE_3_DENSE_BLOCK_7__CONV_LEFT_1__CONV2D___BATCHNORM__FUSEDBATCHNORM___RELU_DRAM_PITCH_MODIFIABLE
    o_args->PeleeNet__stage_3_dense_block_7__conv_left_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_dram_pitch_enable = 1U;
    o_args->PeleeNet__stage_3_dense_block_7__conv_left_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_dram_pitch_value = 64U;
#endif
#ifdef PELEENET__STAGE_3_DENSE_BLOCK_7__CONV_RIGHT_2__CONV2D___BATCHNORM__FUSEDBATCHNORM___RELU_DRAM_PITCH_MODIFIABLE
    o_args->PeleeNet__stage_3_dense_block_7__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu_dram_pitch_enable = 1U;
    o_args->PeleeNet__stage_3_dense_block_7__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu_dram_pitch_value = 64U;
#endif
#ifdef PELEENET__STAGE_3_TRANSITION_LAYER__TRANSITION_LAYER_AVGPOOL__AVGPOOL______MULI___490___MEAN_1_SRC0_NHWC___NHWC___TRANSPOSE___123_DRAM_PITCH_MODIFIABLE
    o_args->PeleeNet__stage_3_transition_layer__transition_layer_avgpool__AvgPool______muli___490___Mean_1_src0_nhwc___nhwc___transpose___123_dram_pitch_enable = 1U;
    o_args->PeleeNet__stage_3_transition_layer__transition_layer_avgpool__AvgPool______muli___490___Mean_1_src0_nhwc___nhwc___transpose___123_dram_pitch_value = 64U;
#endif
#ifdef PELEENET__STAGE_3_TRANSITION_LAYER__TRANSITION_LAYER_CONV0__CONV2D___BATCHNORM__FUSEDBATCHNORM___RELU_DRAM_PITCH_MODIFIABLE
    o_args->PeleeNet__stage_3_transition_layer__transition_layer_conv0__Conv2D___BatchNorm__FusedBatchNorm___Relu_dram_pitch_enable = 1U;
    o_args->PeleeNet__stage_3_transition_layer__transition_layer_conv0__Conv2D___BatchNorm__FusedBatchNorm___Relu_dram_pitch_value = 64U;
#endif
#ifdef PELEENET__STEM_BLOCK__CONCAT_DRAM_PITCH_MODIFIABLE
    o_args->PeleeNet__stem_block__concat_dram_pitch_enable = 1U;
    o_args->PeleeNet__stem_block__concat_dram_pitch_value = 64U;
#endif
#ifdef PELEENET__STEM_BLOCK__STEM_BLOCK_CONV0__RELU______MULI___443_DRAM_PITCH_MODIFIABLE
    o_args->PeleeNet__stem_block__stem_block_conv0__Relu______muli___443_dram_pitch_enable = 1U;
    o_args->PeleeNet__stem_block__stem_block_conv0__Relu______muli___443_dram_pitch_value = 64U;
#endif
#ifdef PELEENET__STEM_BLOCK__STEM_BLOCK_CONV0__RELU___MULI___442_DRAM_PITCH_MODIFIABLE
    o_args->PeleeNet__stem_block__stem_block_conv0__Relu___muli___442_dram_pitch_enable = 1U;
    o_args->PeleeNet__stem_block__stem_block_conv0__Relu___muli___442_dram_pitch_value = 64U;
#endif
#ifdef PELEENET__STEM_BLOCK__STEM_BLOCK_CONV0__RELU___PASSTHROUGH___414_DRAM_PITCH_MODIFIABLE
    o_args->PeleeNet__stem_block__stem_block_conv0__Relu___passthrough___414_dram_pitch_enable = 1U;
    o_args->PeleeNet__stem_block__stem_block_conv0__Relu___passthrough___414_dram_pitch_value = 64U;
#endif
#ifdef PELEENET__STEM_BLOCK__STEM_BLOCK_CONV1_L0__CONV2D___BATCHNORM__FUSEDBATCHNORM___RELU_DRAM_PITCH_MODIFIABLE
    o_args->PeleeNet__stem_block__stem_block_conv1_l0__Conv2D___BatchNorm__FusedBatchNorm___Relu_dram_pitch_enable = 1U;
    o_args->PeleeNet__stem_block__stem_block_conv1_l0__Conv2D___BatchNorm__FusedBatchNorm___Relu_dram_pitch_value = 64U;
#endif
#ifdef PELEENET__STEM_BLOCK__STEM_BLOCK_MAXPOOL1_R0__MAXPOOL_DRAM_PITCH_MODIFIABLE
    o_args->PeleeNet__stem_block__stem_block_maxpool1_r0__MaxPool_dram_pitch_enable = 1U;
    o_args->PeleeNet__stem_block__stem_block_maxpool1_r0__MaxPool_dram_pitch_value = 64U;
#endif
#ifdef PLACEHOLDER_DRAM_PITCH_MODIFIABLE
    o_args->Placeholder_dram_pitch_enable = 1U;
    o_args->Placeholder_dram_pitch_value = 1280U;
#endif
#ifdef PLACEHOLDER_UV_DRAM_PITCH_MODIFIABLE
    o_args->Placeholder_uv_dram_pitch_enable = 1U;
    o_args->Placeholder_uv_dram_pitch_value = 1280U;
#endif
#ifdef RELU_DRAM_PITCH_MODIFIABLE
    o_args->Relu_dram_pitch_enable = 1U;
    o_args->Relu_dram_pitch_value = 64U;
#endif
#ifdef RESIZEBILINEAR_1_DRAM_PITCH_MODIFIABLE
    o_args->ResizeBilinear_1_dram_pitch_enable = 1U;
    o_args->ResizeBilinear_1_dram_pitch_value = 64U;
#endif
#ifdef RESIZEBILINEAR_DRAM_PITCH_MODIFIABLE
    o_args->ResizeBilinear_dram_pitch_enable = 1U;
    o_args->ResizeBilinear_dram_pitch_value = 64U;
#endif
#ifdef SIGMOID_1_DRAM_PITCH_MODIFIABLE
    o_args->Sigmoid_1_dram_pitch_enable = 1U;
    o_args->Sigmoid_1_dram_pitch_value = 64U;
#endif
#ifdef __PVCN_1152__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_1152__dram_pitch_enable = 1U;
    o_args->__pvcn_1152__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_80000002__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_80000002__dram_pitch_enable = 1U;
    o_args->__pvcn_80000002__dram_pitch_value = 64U;
#endif
#ifdef CONCAT_1_580000000__DRAM_PITCH_MODIFIABLE
    o_args->concat_1_580000000__dram_pitch_enable = 1U;
    o_args->concat_1_580000000__dram_pitch_value = 64U;
#endif
#ifdef CONCAT_1_580000105__DRAM_PITCH_MODIFIABLE
    o_args->concat_1_580000105__dram_pitch_enable = 1U;
    o_args->concat_1_580000105__dram_pitch_value = 64U;
#endif
#ifdef CONCAT_1_580000109__DRAM_PITCH_MODIFIABLE
    o_args->concat_1_580000109__dram_pitch_enable = 1U;
    o_args->concat_1_580000109__dram_pitch_value = 64U;
#endif
#ifdef CONCAT_1_580000114__DRAM_PITCH_MODIFIABLE
    o_args->concat_1_580000114__dram_pitch_enable = 1U;
    o_args->concat_1_580000114__dram_pitch_value = 64U;
#endif
#ifdef CONCAT_1_580000115__DRAM_PITCH_MODIFIABLE
    o_args->concat_1_580000115__dram_pitch_enable = 1U;
    o_args->concat_1_580000115__dram_pitch_value = 64U;
#endif
#ifdef CONCAT_1_580000127__DRAM_PITCH_MODIFIABLE
    o_args->concat_1_580000127__dram_pitch_enable = 1U;
    o_args->concat_1_580000127__dram_pitch_value = 64U;
#endif
#ifdef CONCAT_1_580000128__DRAM_PITCH_MODIFIABLE
    o_args->concat_1_580000128__dram_pitch_enable = 1U;
    o_args->concat_1_580000128__dram_pitch_value = 64U;
#endif
#ifdef CONCAT_1_580000132__DRAM_PITCH_MODIFIABLE
    o_args->concat_1_580000132__dram_pitch_enable = 1U;
    o_args->concat_1_580000132__dram_pitch_value = 64U;
#endif
#ifdef CONCAT_1_580000133__DRAM_PITCH_MODIFIABLE
    o_args->concat_1_580000133__dram_pitch_enable = 1U;
    o_args->concat_1_580000133__dram_pitch_value = 64U;
#endif
#ifdef CONCAT_1_580000145__DRAM_PITCH_MODIFIABLE
    o_args->concat_1_580000145__dram_pitch_enable = 1U;
    o_args->concat_1_580000145__dram_pitch_value = 64U;
#endif
#ifdef CONCAT_1_580000146__DRAM_PITCH_MODIFIABLE
    o_args->concat_1_580000146__dram_pitch_enable = 1U;
    o_args->concat_1_580000146__dram_pitch_value = 64U;
#endif
#ifdef CONCAT_1_580000150__DRAM_PITCH_MODIFIABLE
    o_args->concat_1_580000150__dram_pitch_enable = 1U;
    o_args->concat_1_580000150__dram_pitch_value = 64U;
#endif
#ifdef CONCAT_1_580000151__DRAM_PITCH_MODIFIABLE
    o_args->concat_1_580000151__dram_pitch_enable = 1U;
    o_args->concat_1_580000151__dram_pitch_value = 64U;
#endif
#ifdef CONCAT_1_580000163__DRAM_PITCH_MODIFIABLE
    o_args->concat_1_580000163__dram_pitch_enable = 1U;
    o_args->concat_1_580000163__dram_pitch_value = 64U;
#endif
#ifdef CONCAT_1_580000164__DRAM_PITCH_MODIFIABLE
    o_args->concat_1_580000164__dram_pitch_enable = 1U;
    o_args->concat_1_580000164__dram_pitch_value = 64U;
#endif
#ifdef CONCAT_1_580000168__DRAM_PITCH_MODIFIABLE
    o_args->concat_1_580000168__dram_pitch_enable = 1U;
    o_args->concat_1_580000168__dram_pitch_value = 64U;
#endif
#ifdef CONCAT_1_580000169__DRAM_PITCH_MODIFIABLE
    o_args->concat_1_580000169__dram_pitch_enable = 1U;
    o_args->concat_1_580000169__dram_pitch_value = 64U;
#endif
#ifdef CONCAT_1_580000181__DRAM_PITCH_MODIFIABLE
    o_args->concat_1_580000181__dram_pitch_enable = 1U;
    o_args->concat_1_580000181__dram_pitch_value = 64U;
#endif
#ifdef CONCAT_1_580000182__DRAM_PITCH_MODIFIABLE
    o_args->concat_1_580000182__dram_pitch_enable = 1U;
    o_args->concat_1_580000182__dram_pitch_value = 64U;
#endif
#ifdef CONCAT_1_580000186__DRAM_PITCH_MODIFIABLE
    o_args->concat_1_580000186__dram_pitch_enable = 1U;
    o_args->concat_1_580000186__dram_pitch_value = 64U;
#endif
#ifdef CONCAT_1_580000187__DRAM_PITCH_MODIFIABLE
    o_args->concat_1_580000187__dram_pitch_enable = 1U;
    o_args->concat_1_580000187__dram_pitch_value = 64U;
#endif
#ifdef CONCAT_1_580000199__DRAM_PITCH_MODIFIABLE
    o_args->concat_1_580000199__dram_pitch_enable = 1U;
    o_args->concat_1_580000199__dram_pitch_value = 64U;
#endif
#ifdef CONCAT_1_580000200__DRAM_PITCH_MODIFIABLE
    o_args->concat_1_580000200__dram_pitch_enable = 1U;
    o_args->concat_1_580000200__dram_pitch_value = 64U;
#endif
#ifdef CONCAT_1_580000204__DRAM_PITCH_MODIFIABLE
    o_args->concat_1_580000204__dram_pitch_enable = 1U;
    o_args->concat_1_580000204__dram_pitch_value = 64U;
#endif
#ifdef CONCAT_1_580000205__DRAM_PITCH_MODIFIABLE
    o_args->concat_1_580000205__dram_pitch_enable = 1U;
    o_args->concat_1_580000205__dram_pitch_value = 64U;
#endif
#ifdef CONCAT_1_580000217__DRAM_PITCH_MODIFIABLE
    o_args->concat_1_580000217__dram_pitch_enable = 1U;
    o_args->concat_1_580000217__dram_pitch_value = 64U;
#endif
#ifdef CONCAT_1_580000218__DRAM_PITCH_MODIFIABLE
    o_args->concat_1_580000218__dram_pitch_enable = 1U;
    o_args->concat_1_580000218__dram_pitch_value = 64U;
#endif
#ifdef CONCAT_1_580000222__DRAM_PITCH_MODIFIABLE
    o_args->concat_1_580000222__dram_pitch_enable = 1U;
    o_args->concat_1_580000222__dram_pitch_value = 64U;
#endif
#ifdef CONCAT_1_580000223__DRAM_PITCH_MODIFIABLE
    o_args->concat_1_580000223__dram_pitch_enable = 1U;
    o_args->concat_1_580000223__dram_pitch_value = 64U;
#endif
#ifdef CONCAT_1_580000235__DRAM_PITCH_MODIFIABLE
    o_args->concat_1_580000235__dram_pitch_enable = 1U;
    o_args->concat_1_580000235__dram_pitch_value = 64U;
#endif
#ifdef CONCAT_1_580000236__DRAM_PITCH_MODIFIABLE
    o_args->concat_1_580000236__dram_pitch_enable = 1U;
    o_args->concat_1_580000236__dram_pitch_value = 64U;
#endif
#ifdef CONCAT_1_580000240__DRAM_PITCH_MODIFIABLE
    o_args->concat_1_580000240__dram_pitch_enable = 1U;
    o_args->concat_1_580000240__dram_pitch_value = 64U;
#endif
#ifdef CONCAT_1_580000241__DRAM_PITCH_MODIFIABLE
    o_args->concat_1_580000241__dram_pitch_enable = 1U;
    o_args->concat_1_580000241__dram_pitch_value = 64U;
#endif
#ifdef CONCAT_1_580000253__DRAM_PITCH_MODIFIABLE
    o_args->concat_1_580000253__dram_pitch_enable = 1U;
    o_args->concat_1_580000253__dram_pitch_value = 64U;
#endif
#ifdef CONCAT_1_580000254__DRAM_PITCH_MODIFIABLE
    o_args->concat_1_580000254__dram_pitch_enable = 1U;
    o_args->concat_1_580000254__dram_pitch_value = 64U;
#endif
#ifdef CONCAT_1_580000258__DRAM_PITCH_MODIFIABLE
    o_args->concat_1_580000258__dram_pitch_enable = 1U;
    o_args->concat_1_580000258__dram_pitch_value = 64U;
#endif
#ifdef CONCAT_1_580000259__DRAM_PITCH_MODIFIABLE
    o_args->concat_1_580000259__dram_pitch_enable = 1U;
    o_args->concat_1_580000259__dram_pitch_value = 64U;
#endif
#ifdef CONCAT_1_DRAM_PITCH_MODIFIABLE
    o_args->concat_1_dram_pitch_enable = 1U;
    o_args->concat_1_dram_pitch_value = 64U;
#endif
#ifdef CONCAT_DRAM_PITCH_MODIFIABLE
    o_args->concat_dram_pitch_enable = 1U;
    o_args->concat_dram_pitch_value = 64U;
#endif
#ifdef OUT_CLASS_DRAM_PITCH_MODIFIABLE
    o_args->out_class_dram_pitch_enable = 1U;
    o_args->out_class_dram_pitch_value = 320U;
#endif

    return status;
} // end of flexidag_openseg_ag_reset()


/********************* USED BY FLEXIDAG_OPENSEG_AG_INIT() **********************/
/* Wrapper function for cvtable_find() */
inline errcode_enum_t flexidag_openseg_ag_read_cvtable (
    flexidag_openseg_ag_ips_t *ctxt
) {
    unsigned int split_id = 0; // size_t
    int32_t tbar_sz;
    errcode_enum_t status = ERRCODE_NONE;
    struct flexidag_openseg_ag_split_context *split_ctxt = NULL;
    char dag_key[] = "FLEXIDAG_OPENSEG_AG_---_DAG";

    for (; split_id < FLEXIDAG_OPENSEG_AG_SPLIT_COUNT; split_id++) {
        tbar_sz = 0;
        split_ctxt = &ctxt->split_ctxt[split_id];

#ifdef CVTASK_PRINT
        cvtask_printf(LVL_DEBUG, "  >> #Split-%u# Extracting DAG base address "
            "from CV Table Archive", split_id, 0, 0, 0, 0);
#endif

        dag_key[20] = '0' + ((split_id / 100U) % 10U);
        dag_key[21] = '0' + ((split_id / 10U ) % 10U);
        dag_key[22] = '0' + ((split_id / 1U  ) % 10U);

        // extracting DAG base from CV Table Archive
        status = cvtable_find(
            dag_key,
            (const void **)&split_ctxt->dram_dag_base,
            &tbar_sz
        );
        if (is_err(status))
        {
            cvtask_printf(LVL_DEBUG, "  >> cvtable_find() key FLEXIDAG_OPENSEG_AG_"
                "%c%c%c_DAG look up failed for split(%u)", 
                '0' + ((split_id / 100U) % 10U),
                '0' + ((split_id / 10U ) % 10U),
                '0' + ((split_id / 1U  ) % 10U),
                split_id, 0);
            return ERRCODE_GENERIC;
        }
        else if(tbar_sz != dag_img_sizes[split_id])
        {
            cvtask_printf(LVL_DEBUG, "  >> size of dag from table archive(%u) "
                "!= size (%u) in header file for split(%u)",
                tbar_sz, dag_img_sizes[split_id], split_id, 0, 0);
            return ERRCODE_GENERIC;
        }
#ifdef CVTASK_PRINT
        else
            cvtask_printf(LVL_DEBUG, "  >> * extracted DAG binary "
                "@address(0x%08X)", split_ctxt->dram_dag_base, 0, 0, 0, 0);
#endif

    }

    return status;
} // end of flexidag_openseg_ag_read_cvtable()


/********************** USED BY FLEXIDAG_OPENSEG_AG_RUN() **********************/
/* Load machine code into VP */
inline void flexidag_openseg_ag_load_split(
    struct flexidag_openseg_ag_split_context *split_ctxt,
    uint32_t split_id
) {

    //vp_load_dag(
    //    split_ctxt->dram_dag_base,
    //    vmem_dag_bases[split_id],
    //    dag_img_sizes[split_id],
    //    0
    //);
    uint32_t dag_dbase = split_ctxt->dram_dag_base;
    uint32_t dag_vbase = vmem_dag_bases[split_id];
    uint32_t dag_size  = dag_img_sizes[split_id];

    // vp_load_dag()
    coproc_poke_ldq_word(dag_vbase,            0xFD4U);
    coproc_poke_ldq_word(dag_size + 256U - 1U, 0xFE0U);
    coproc_poke_ldq_word(dag_dbase,            0xFE8U); 
    coproc_dmald(0xFD0U);

    vp_dummy_run();

    return;
} // end of flexidag_openseg_ag_load_split()

/* Modify input and output buffer addresses of a particular split DAG */
inline void flexidag_openseg_ag_poke_split(
    flexidag_openseg_ag_required_fields_t *r_args,
    flexidag_openseg_ag_optional_fields_t *o_args,
    struct flexidag_openseg_ag_split_context *split_ctxt,
    uint32_t split_id) {
    uint32_t dag_base = (uint32_t) r_args->instance_private_uncached_storage;
    uint32_t dagbin_base = vmem_dagbin_bases[split_id];

    // Avoid unused variable warning
    (void) dag_base;

    switch (split_id) {
        case 0:
            /*** poke() calls ***/
            /* Primary inputs */
            // Poke HMB input buffer: Placeholder
            coproc_poke_word_offset(r_args->Placeholder_addr + 0, dagbin_base, 268U);

            // Poke HMB input buffer: Placeholder_uv
            coproc_poke_word_offset(r_args->Placeholder_uv_addr + 0, dagbin_base, 308U);

            /* Intermediate buffers */
            // Poke HMB intermediate output buffer: Conv__Conv2D___BiasAdd___BatchNorm__FusedBatchNorm___muli___7___BatchNorm__FusedBatchNorm___PeleeNet__stem_block__stem_block_conv0__Conv2D___BatchNorm__FusedBatchNorm___muli___440___1
            coproc_poke_word(dag_base + 0,
                dagbin_base + 632U);

            /* Pokable fields */
#ifdef PLACEHOLDER_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: Placeholder_dram_pitch
            if (o_args->Placeholder_dram_pitch_enable)
                coproc_poke_half_offset(o_args->Placeholder_dram_pitch_value, dagbin_base, 
                    272U);

#endif

#ifdef PLACEHOLDER_UV_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: Placeholder_uv_dram_pitch
            if (o_args->Placeholder_uv_dram_pitch_enable)
                coproc_poke_half_offset(o_args->Placeholder_uv_dram_pitch_value, dagbin_base, 
                    312U);

#endif

#ifdef CONV__CONV2D___BIASADD___BATCHNORM__FUSEDBATCHNORM___MULI___7___BATCHNORM__FUSEDBATCHNORM___PELEENET__STEM_BLOCK__STEM_BLOCK_CONV0__CONV2D___BATCHNORM__FUSEDBATCHNORM___MULI___440___1_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: Conv__Conv2D___BiasAdd___BatchNorm__FusedBatchNorm___muli___7___BatchNorm__FusedBatchNorm___PeleeNet__stem_block__stem_block_conv0__Conv2D___BatchNorm__FusedBatchNorm___muli___440___1_dram_pitch
            if (o_args->Conv__Conv2D___BiasAdd___BatchNorm__FusedBatchNorm___muli___7___BatchNorm__FusedBatchNorm___PeleeNet__stem_block__stem_block_conv0__Conv2D___BatchNorm__FusedBatchNorm___muli___440___1_dram_pitch_enable)
                coproc_poke_half(o_args->Conv__Conv2D___BiasAdd___BatchNorm__FusedBatchNorm___muli___7___BatchNorm__FusedBatchNorm___PeleeNet__stem_block__stem_block_conv0__Conv2D___BatchNorm__FusedBatchNorm___muli___440___1_dram_pitch_value, dagbin_base + 636U);

#endif

            /*** dmald() calls ***/
            break;
        case 1:
            /*** poke() calls ***/
            /* Intermediate buffers */
            // Poke HMB intermediate input buffer: Conv__Conv2D___BiasAdd___BatchNorm__FusedBatchNorm___muli___7___BatchNorm__FusedBatchNorm___PeleeNet__stem_block__stem_block_conv0__Conv2D___BatchNorm__FusedBatchNorm___muli___440___1
            coproc_poke_word_offset(dag_base + 0,
                dagbin_base, 28U);

            // Poke HMB intermediate output buffer: PeleeNet__stem_block__stem_block_conv0__Relu______muli___443
            coproc_poke_word_offset(dag_base + 15728640,
                dagbin_base, 116U);

            // Poke HMB intermediate output buffer: Relu
            coproc_poke_word_offset(dag_base + 20971520,
                dagbin_base, 200U);

            /* Pokable fields */
#ifdef CONV__CONV2D___BIASADD___BATCHNORM__FUSEDBATCHNORM___MULI___7___BATCHNORM__FUSEDBATCHNORM___PELEENET__STEM_BLOCK__STEM_BLOCK_CONV0__CONV2D___BATCHNORM__FUSEDBATCHNORM___MULI___440___1_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: Conv__Conv2D___BiasAdd___BatchNorm__FusedBatchNorm___muli___7___BatchNorm__FusedBatchNorm___PeleeNet__stem_block__stem_block_conv0__Conv2D___BatchNorm__FusedBatchNorm___muli___440___1_dram_pitch
            if (o_args->Conv__Conv2D___BiasAdd___BatchNorm__FusedBatchNorm___muli___7___BatchNorm__FusedBatchNorm___PeleeNet__stem_block__stem_block_conv0__Conv2D___BatchNorm__FusedBatchNorm___muli___440___1_dram_pitch_enable)
                coproc_poke_half_offset(o_args->Conv__Conv2D___BiasAdd___BatchNorm__FusedBatchNorm___muli___7___BatchNorm__FusedBatchNorm___PeleeNet__stem_block__stem_block_conv0__Conv2D___BatchNorm__FusedBatchNorm___muli___440___1_dram_pitch_value, dagbin_base, 
                    32U);

#endif

#ifdef PELEENET__STEM_BLOCK__STEM_BLOCK_CONV0__RELU______MULI___443_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: PeleeNet__stem_block__stem_block_conv0__Relu______muli___443_dram_pitch
            if (o_args->PeleeNet__stem_block__stem_block_conv0__Relu______muli___443_dram_pitch_enable)
                coproc_poke_half_offset(o_args->PeleeNet__stem_block__stem_block_conv0__Relu______muli___443_dram_pitch_value, dagbin_base, 
                    120U);

#endif

#ifdef RELU_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: Relu_dram_pitch
            if (o_args->Relu_dram_pitch_enable)
                coproc_poke_half_offset(o_args->Relu_dram_pitch_value, dagbin_base, 
                    204U);

#endif

            /*** dmald() calls ***/
            break;
        case 2:
            /*** poke() calls ***/
            /* Intermediate buffers */
            // Poke HMB intermediate input buffer: PeleeNet__stem_block__stem_block_conv0__Relu______muli___443
            coproc_poke_word_offset(dag_base + 15728640,
                dagbin_base, 68U);

            // Poke HMB intermediate input buffer: Relu
            coproc_poke_word_offset(dag_base + 20971520,
                dagbin_base, 28U);

            // Poke HMB intermediate output buffer: PeleeNet__stem_block__stem_block_conv0__Relu___passthrough___414
            coproc_poke_word_offset(dag_base + 0,
                dagbin_base, 212U);

            // Poke HMB intermediate output buffer: PeleeNet__stem_block__stem_block_conv0__Relu___muli___442
            coproc_poke_word_offset(dag_base + 5242880,
                dagbin_base, 260U);

            // Poke HMB intermediate output buffer: Conv_1__Conv2D___BiasAdd___BatchNorm_1__FusedBatchNorm___muli___10___BatchNorm_1__FusedBatchNorm___Relu_1
            coproc_poke_word_offset(dag_base + 10485760,
                dagbin_base, 320U);

            /* Pokable fields */
#ifdef RELU_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: Relu_dram_pitch
            if (o_args->Relu_dram_pitch_enable)
                coproc_poke_half_offset(o_args->Relu_dram_pitch_value, dagbin_base, 
                    32U);

#endif

#ifdef PELEENET__STEM_BLOCK__STEM_BLOCK_CONV0__RELU______MULI___443_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: PeleeNet__stem_block__stem_block_conv0__Relu______muli___443_dram_pitch
            if (o_args->PeleeNet__stem_block__stem_block_conv0__Relu______muli___443_dram_pitch_enable)
                coproc_poke_half_offset(o_args->PeleeNet__stem_block__stem_block_conv0__Relu______muli___443_dram_pitch_value, dagbin_base, 
                    72U);

#endif

#ifdef PELEENET__STEM_BLOCK__STEM_BLOCK_CONV0__RELU___PASSTHROUGH___414_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: PeleeNet__stem_block__stem_block_conv0__Relu___passthrough___414_dram_pitch
            if (o_args->PeleeNet__stem_block__stem_block_conv0__Relu___passthrough___414_dram_pitch_enable)
                coproc_poke_half_offset(o_args->PeleeNet__stem_block__stem_block_conv0__Relu___passthrough___414_dram_pitch_value, dagbin_base, 
                    216U);

#endif

#ifdef PELEENET__STEM_BLOCK__STEM_BLOCK_CONV0__RELU___MULI___442_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: PeleeNet__stem_block__stem_block_conv0__Relu___muli___442_dram_pitch
            if (o_args->PeleeNet__stem_block__stem_block_conv0__Relu___muli___442_dram_pitch_enable)
                coproc_poke_half_offset(o_args->PeleeNet__stem_block__stem_block_conv0__Relu___muli___442_dram_pitch_value, dagbin_base, 
                    264U);

#endif

#ifdef CONV_1__CONV2D___BIASADD___BATCHNORM_1__FUSEDBATCHNORM___MULI___10___BATCHNORM_1__FUSEDBATCHNORM___RELU_1_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: Conv_1__Conv2D___BiasAdd___BatchNorm_1__FusedBatchNorm___muli___10___BatchNorm_1__FusedBatchNorm___Relu_1_dram_pitch
            if (o_args->Conv_1__Conv2D___BiasAdd___BatchNorm_1__FusedBatchNorm___muli___10___BatchNorm_1__FusedBatchNorm___Relu_1_dram_pitch_enable)
                coproc_poke_half_offset(o_args->Conv_1__Conv2D___BiasAdd___BatchNorm_1__FusedBatchNorm___muli___10___BatchNorm_1__FusedBatchNorm___Relu_1_dram_pitch_value, dagbin_base, 
                    324U);

#endif

            /*** dmald() calls ***/
            break;
        case 3:
            /*** poke() calls ***/
            /* Intermediate buffers */
            // Poke HMB intermediate input buffer: PeleeNet__stem_block__stem_block_conv0__Relu___passthrough___414
            coproc_poke_word_offset(dag_base + 0,
                dagbin_base, 108U);

            // Poke HMB intermediate input buffer: PeleeNet__stem_block__stem_block_conv0__Relu___muli___442
            coproc_poke_word_offset(dag_base + 5242880,
                dagbin_base, 28U);

            // Poke HMB intermediate input buffer: Conv_1__Conv2D___BiasAdd___BatchNorm_1__FusedBatchNorm___muli___10___BatchNorm_1__FusedBatchNorm___Relu_1
            coproc_poke_word_offset(dag_base + 10485760,
                dagbin_base, 68U);

            // Poke HMB intermediate output buffer: PeleeNet__stem_block__stem_block_conv1_l0__Conv2D___BatchNorm__FusedBatchNorm___Relu
            coproc_poke_word_offset(dag_base + 15728640,
                dagbin_base, 316U);

            // Poke HMB intermediate output buffer: PeleeNet__stem_block__stem_block_maxpool1_r0__MaxPool
            coproc_poke_word_offset(dag_base + 18350080,
                dagbin_base, 376U);

            // Poke HMB intermediate output buffer: __pvcn_80000002_
            coproc_poke_word_offset(dag_base + 19660800,
                dagbin_base, 436U);

            /* Pokable fields */
#ifdef PELEENET__STEM_BLOCK__STEM_BLOCK_CONV0__RELU___MULI___442_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: PeleeNet__stem_block__stem_block_conv0__Relu___muli___442_dram_pitch
            if (o_args->PeleeNet__stem_block__stem_block_conv0__Relu___muli___442_dram_pitch_enable)
                coproc_poke_half_offset(o_args->PeleeNet__stem_block__stem_block_conv0__Relu___muli___442_dram_pitch_value, dagbin_base, 
                    32U);

#endif

#ifdef CONV_1__CONV2D___BIASADD___BATCHNORM_1__FUSEDBATCHNORM___MULI___10___BATCHNORM_1__FUSEDBATCHNORM___RELU_1_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: Conv_1__Conv2D___BiasAdd___BatchNorm_1__FusedBatchNorm___muli___10___BatchNorm_1__FusedBatchNorm___Relu_1_dram_pitch
            if (o_args->Conv_1__Conv2D___BiasAdd___BatchNorm_1__FusedBatchNorm___muli___10___BatchNorm_1__FusedBatchNorm___Relu_1_dram_pitch_enable)
                coproc_poke_half_offset(o_args->Conv_1__Conv2D___BiasAdd___BatchNorm_1__FusedBatchNorm___muli___10___BatchNorm_1__FusedBatchNorm___Relu_1_dram_pitch_value, dagbin_base, 
                    72U);

#endif

#ifdef PELEENET__STEM_BLOCK__STEM_BLOCK_CONV0__RELU___PASSTHROUGH___414_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: PeleeNet__stem_block__stem_block_conv0__Relu___passthrough___414_dram_pitch
            if (o_args->PeleeNet__stem_block__stem_block_conv0__Relu___passthrough___414_dram_pitch_enable)
                coproc_poke_half_offset(o_args->PeleeNet__stem_block__stem_block_conv0__Relu___passthrough___414_dram_pitch_value, dagbin_base, 
                    112U);

#endif

#ifdef PELEENET__STEM_BLOCK__STEM_BLOCK_CONV1_L0__CONV2D___BATCHNORM__FUSEDBATCHNORM___RELU_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: PeleeNet__stem_block__stem_block_conv1_l0__Conv2D___BatchNorm__FusedBatchNorm___Relu_dram_pitch
            if (o_args->PeleeNet__stem_block__stem_block_conv1_l0__Conv2D___BatchNorm__FusedBatchNorm___Relu_dram_pitch_enable)
                coproc_poke_half_offset(o_args->PeleeNet__stem_block__stem_block_conv1_l0__Conv2D___BatchNorm__FusedBatchNorm___Relu_dram_pitch_value, dagbin_base, 
                    320U);

#endif

#ifdef PELEENET__STEM_BLOCK__STEM_BLOCK_MAXPOOL1_R0__MAXPOOL_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: PeleeNet__stem_block__stem_block_maxpool1_r0__MaxPool_dram_pitch
            if (o_args->PeleeNet__stem_block__stem_block_maxpool1_r0__MaxPool_dram_pitch_enable)
                coproc_poke_half_offset(o_args->PeleeNet__stem_block__stem_block_maxpool1_r0__MaxPool_dram_pitch_value, dagbin_base, 
                    380U);

#endif

#ifdef __PVCN_80000002__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_80000002__dram_pitch
            if (o_args->__pvcn_80000002__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_80000002__dram_pitch_value, dagbin_base, 
                    440U);

#endif

            /*** dmald() calls ***/
            break;
        case 4:
            /*** poke() calls ***/
            /* Intermediate buffers */
            // Poke HMB intermediate input buffer: Conv_1__Conv2D___BiasAdd___BatchNorm_1__FusedBatchNorm___muli___10___BatchNorm_1__FusedBatchNorm___Relu_1
            coproc_poke_word_offset(dag_base + 10485760,
                dagbin_base, 68U);

            // Poke HMB intermediate input buffer: __pvcn_80000002_
            coproc_poke_word_offset(dag_base + 19660800,
                dagbin_base, 28U);

            // Poke HMB intermediate output buffer: __pvcn_1152_
            coproc_poke_word_offset(dag_base + 0,
                dagbin_base, 180U);

            /* Pokable fields */
#ifdef __PVCN_80000002__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_80000002__dram_pitch
            if (o_args->__pvcn_80000002__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_80000002__dram_pitch_value, dagbin_base, 
                    32U);

#endif

#ifdef CONV_1__CONV2D___BIASADD___BATCHNORM_1__FUSEDBATCHNORM___MULI___10___BATCHNORM_1__FUSEDBATCHNORM___RELU_1_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: Conv_1__Conv2D___BiasAdd___BatchNorm_1__FusedBatchNorm___muli___10___BatchNorm_1__FusedBatchNorm___Relu_1_dram_pitch
            if (o_args->Conv_1__Conv2D___BiasAdd___BatchNorm_1__FusedBatchNorm___muli___10___BatchNorm_1__FusedBatchNorm___Relu_1_dram_pitch_enable)
                coproc_poke_half_offset(o_args->Conv_1__Conv2D___BiasAdd___BatchNorm_1__FusedBatchNorm___muli___10___BatchNorm_1__FusedBatchNorm___Relu_1_dram_pitch_value, dagbin_base, 
                    72U);

#endif

#ifdef __PVCN_1152__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_1152__dram_pitch
            if (o_args->__pvcn_1152__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_1152__dram_pitch_value, dagbin_base, 
                    184U);

#endif

            /*** dmald() calls ***/
            break;
        case 5:
            /*** poke() calls ***/
            /* Intermediate buffers */
            // Poke HMB intermediate input buffer: __pvcn_1152_
            coproc_poke_word_offset(dag_base + 0,
                dagbin_base, 104U);

            // Poke HMB intermediate input buffer: PeleeNet__stem_block__stem_block_conv1_l0__Conv2D___BatchNorm__FusedBatchNorm___Relu
            coproc_poke_word_offset(dag_base + 15728640,
                dagbin_base, 28U);

            // Poke HMB intermediate input buffer: PeleeNet__stem_block__stem_block_maxpool1_r0__MaxPool
            coproc_poke_word_offset(dag_base + 18350080,
                dagbin_base, 64U);

            // Poke HMB intermediate output buffer: PeleeNet__stem_block__concat
            coproc_poke_word_offset(dag_base + 5242880,
                dagbin_base, 288U);

            // Poke HMB intermediate output buffer: Conv_2__Conv2D___BiasAdd___BatchNorm_2__FusedBatchNorm___muli___13___BatchNorm_2__FusedBatchNorm___Relu_2
            coproc_poke_word_offset(dag_base + 7864320,
                dagbin_base, 336U);

            /* Pokable fields */
#ifdef PELEENET__STEM_BLOCK__STEM_BLOCK_CONV1_L0__CONV2D___BATCHNORM__FUSEDBATCHNORM___RELU_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: PeleeNet__stem_block__stem_block_conv1_l0__Conv2D___BatchNorm__FusedBatchNorm___Relu_dram_pitch
            if (o_args->PeleeNet__stem_block__stem_block_conv1_l0__Conv2D___BatchNorm__FusedBatchNorm___Relu_dram_pitch_enable)
                coproc_poke_half_offset(o_args->PeleeNet__stem_block__stem_block_conv1_l0__Conv2D___BatchNorm__FusedBatchNorm___Relu_dram_pitch_value, dagbin_base, 
                    32U);

#endif

#ifdef PELEENET__STEM_BLOCK__STEM_BLOCK_MAXPOOL1_R0__MAXPOOL_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: PeleeNet__stem_block__stem_block_maxpool1_r0__MaxPool_dram_pitch
            if (o_args->PeleeNet__stem_block__stem_block_maxpool1_r0__MaxPool_dram_pitch_enable)
                coproc_poke_half_offset(o_args->PeleeNet__stem_block__stem_block_maxpool1_r0__MaxPool_dram_pitch_value, dagbin_base, 
                    68U);

#endif

#ifdef __PVCN_1152__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_1152__dram_pitch
            if (o_args->__pvcn_1152__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_1152__dram_pitch_value, dagbin_base, 
                    108U);

#endif

#ifdef PELEENET__STEM_BLOCK__CONCAT_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: PeleeNet__stem_block__concat_dram_pitch
            if (o_args->PeleeNet__stem_block__concat_dram_pitch_enable)
                coproc_poke_half_offset(o_args->PeleeNet__stem_block__concat_dram_pitch_value, dagbin_base, 
                    292U);

#endif

#ifdef CONV_2__CONV2D___BIASADD___BATCHNORM_2__FUSEDBATCHNORM___MULI___13___BATCHNORM_2__FUSEDBATCHNORM___RELU_2_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: Conv_2__Conv2D___BiasAdd___BatchNorm_2__FusedBatchNorm___muli___13___BatchNorm_2__FusedBatchNorm___Relu_2_dram_pitch
            if (o_args->Conv_2__Conv2D___BiasAdd___BatchNorm_2__FusedBatchNorm___muli___13___BatchNorm_2__FusedBatchNorm___Relu_2_dram_pitch_enable)
                coproc_poke_half_offset(o_args->Conv_2__Conv2D___BiasAdd___BatchNorm_2__FusedBatchNorm___muli___13___BatchNorm_2__FusedBatchNorm___Relu_2_dram_pitch_value, dagbin_base, 
                    340U);

#endif

            /*** dmald() calls ***/
            break;
        case 6:
            /*** poke() calls ***/
            /* Intermediate buffers */
            // Poke HMB intermediate input buffer: PeleeNet__stem_block__concat
            coproc_poke_word_offset(dag_base + 5242880,
                dagbin_base, 28U);

            // Poke HMB intermediate output buffer: PeleeNet__stage_1_dense_block_0__concat______muli___447
            coproc_poke_word(dag_base + 0,
                dagbin_base + 512U);

            /* Pokable fields */
#ifdef PELEENET__STEM_BLOCK__CONCAT_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: PeleeNet__stem_block__concat_dram_pitch
            if (o_args->PeleeNet__stem_block__concat_dram_pitch_enable)
                coproc_poke_half_offset(o_args->PeleeNet__stem_block__concat_dram_pitch_value, dagbin_base, 
                    32U);

#endif

#ifdef PELEENET__STAGE_1_DENSE_BLOCK_0__CONCAT______MULI___447_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: PeleeNet__stage_1_dense_block_0__concat______muli___447_dram_pitch
            if (o_args->PeleeNet__stage_1_dense_block_0__concat______muli___447_dram_pitch_enable)
                coproc_poke_half(o_args->PeleeNet__stage_1_dense_block_0__concat______muli___447_dram_pitch_value, dagbin_base + 516U);

#endif

            /*** dmald() calls ***/
            break;
        case 7:
            /*** poke() calls ***/
            /* Intermediate buffers */
            // Poke HMB intermediate input buffer: PeleeNet__stage_1_dense_block_0__concat______muli___447
            coproc_poke_word_offset(dag_base + 0,
                dagbin_base, 348U);

            // Poke HMB intermediate output buffer: PeleeNet__stage_1_dense_block_1__concat______muli___450
            coproc_poke_word_offset(dag_base + 2621440,
                dagbin_base, 388U);

            /* Pokable fields */
#ifdef PELEENET__STAGE_1_DENSE_BLOCK_0__CONCAT______MULI___447_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: PeleeNet__stage_1_dense_block_0__concat______muli___447_dram_pitch
            if (o_args->PeleeNet__stage_1_dense_block_0__concat______muli___447_dram_pitch_enable)
                coproc_poke_half_offset(o_args->PeleeNet__stage_1_dense_block_0__concat______muli___447_dram_pitch_value, dagbin_base, 
                    352U);

#endif

#ifdef PELEENET__STAGE_1_DENSE_BLOCK_1__CONCAT______MULI___450_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: PeleeNet__stage_1_dense_block_1__concat______muli___450_dram_pitch
            if (o_args->PeleeNet__stage_1_dense_block_1__concat______muli___450_dram_pitch_enable)
                coproc_poke_half_offset(o_args->PeleeNet__stage_1_dense_block_1__concat______muli___450_dram_pitch_value, dagbin_base, 
                    392U);

#endif

            /*** dmald() calls ***/
            break;
        case 8:
            /*** poke() calls ***/
            /* Intermediate buffers */
            // Poke HMB intermediate input buffer: PeleeNet__stage_1_dense_block_1__concat______muli___450
            coproc_poke_word_offset(dag_base + 2621440,
                dagbin_base, 348U);

            // Poke HMB intermediate output buffer: PeleeNet__stage_1_dense_block_2__concat
            coproc_poke_word_offset(dag_base + 13107200,
                dagbin_base, 388U);

            /* Pokable fields */
#ifdef PELEENET__STAGE_1_DENSE_BLOCK_1__CONCAT______MULI___450_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: PeleeNet__stage_1_dense_block_1__concat______muli___450_dram_pitch
            if (o_args->PeleeNet__stage_1_dense_block_1__concat______muli___450_dram_pitch_enable)
                coproc_poke_half_offset(o_args->PeleeNet__stage_1_dense_block_1__concat______muli___450_dram_pitch_value, dagbin_base, 
                    352U);

#endif

#ifdef PELEENET__STAGE_1_DENSE_BLOCK_2__CONCAT_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: PeleeNet__stage_1_dense_block_2__concat_dram_pitch
            if (o_args->PeleeNet__stage_1_dense_block_2__concat_dram_pitch_enable)
                coproc_poke_half_offset(o_args->PeleeNet__stage_1_dense_block_2__concat_dram_pitch_value, dagbin_base, 
                    392U);

#endif

            /*** dmald() calls ***/
            break;
        case 9:
            /*** poke() calls ***/
            /* Intermediate buffers */
            // Poke HMB intermediate input buffer: PeleeNet__stage_1_dense_block_2__concat
            coproc_poke_word(dag_base + 13107200,
                dagbin_base + 744U);

            // Poke HMB intermediate output buffer: PeleeNet__stage_1_transition_layer__transition_layer_avgpool__AvgPool______muli___453
            coproc_poke_word(dag_base + 0,
                dagbin_base + 840U);

            /* Pokable fields */
#ifdef PELEENET__STAGE_1_DENSE_BLOCK_2__CONCAT_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: PeleeNet__stage_1_dense_block_2__concat_dram_pitch
            if (o_args->PeleeNet__stage_1_dense_block_2__concat_dram_pitch_enable)
                coproc_poke_half(o_args->PeleeNet__stage_1_dense_block_2__concat_dram_pitch_value, dagbin_base + 748U);

#endif

#ifdef PELEENET__STAGE_1_TRANSITION_LAYER__TRANSITION_LAYER_AVGPOOL__AVGPOOL______MULI___453_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: PeleeNet__stage_1_transition_layer__transition_layer_avgpool__AvgPool______muli___453_dram_pitch
            if (o_args->PeleeNet__stage_1_transition_layer__transition_layer_avgpool__AvgPool______muli___453_dram_pitch_enable)
                coproc_poke_half(o_args->PeleeNet__stage_1_transition_layer__transition_layer_avgpool__AvgPool______muli___453_dram_pitch_value, dagbin_base + 844U);

#endif

            /*** dmald() calls ***/
            break;
        case 10:
            /*** poke() calls ***/
            /* Intermediate buffers */
            // Poke HMB intermediate input buffer: PeleeNet__stage_1_transition_layer__transition_layer_avgpool__AvgPool______muli___453
            coproc_poke_word_offset(dag_base + 0,
                dagbin_base, 348U);

            // Poke HMB intermediate output buffer: PeleeNet__stage_2_dense_block_0__concat______muli___456
            coproc_poke_word_offset(dag_base + 1310720,
                dagbin_base, 388U);

            /* Pokable fields */
#ifdef PELEENET__STAGE_1_TRANSITION_LAYER__TRANSITION_LAYER_AVGPOOL__AVGPOOL______MULI___453_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: PeleeNet__stage_1_transition_layer__transition_layer_avgpool__AvgPool______muli___453_dram_pitch
            if (o_args->PeleeNet__stage_1_transition_layer__transition_layer_avgpool__AvgPool______muli___453_dram_pitch_enable)
                coproc_poke_half_offset(o_args->PeleeNet__stage_1_transition_layer__transition_layer_avgpool__AvgPool______muli___453_dram_pitch_value, dagbin_base, 
                    352U);

#endif

#ifdef PELEENET__STAGE_2_DENSE_BLOCK_0__CONCAT______MULI___456_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: PeleeNet__stage_2_dense_block_0__concat______muli___456_dram_pitch
            if (o_args->PeleeNet__stage_2_dense_block_0__concat______muli___456_dram_pitch_enable)
                coproc_poke_half_offset(o_args->PeleeNet__stage_2_dense_block_0__concat______muli___456_dram_pitch_value, dagbin_base, 
                    392U);

#endif

            /*** dmald() calls ***/
            break;
        case 11:
            /*** poke() calls ***/
            /* Intermediate buffers */
            // Poke HMB intermediate input buffer: PeleeNet__stage_2_dense_block_0__concat______muli___456
            coproc_poke_word_offset(dag_base + 1310720,
                dagbin_base, 348U);

            // Poke HMB intermediate output buffer: PeleeNet__stage_2_dense_block_1__concat______muli___459
            coproc_poke_word_offset(dag_base + 2949120,
                dagbin_base, 388U);

            /* Pokable fields */
#ifdef PELEENET__STAGE_2_DENSE_BLOCK_0__CONCAT______MULI___456_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: PeleeNet__stage_2_dense_block_0__concat______muli___456_dram_pitch
            if (o_args->PeleeNet__stage_2_dense_block_0__concat______muli___456_dram_pitch_enable)
                coproc_poke_half_offset(o_args->PeleeNet__stage_2_dense_block_0__concat______muli___456_dram_pitch_value, dagbin_base, 
                    352U);

#endif

#ifdef PELEENET__STAGE_2_DENSE_BLOCK_1__CONCAT______MULI___459_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: PeleeNet__stage_2_dense_block_1__concat______muli___459_dram_pitch
            if (o_args->PeleeNet__stage_2_dense_block_1__concat______muli___459_dram_pitch_enable)
                coproc_poke_half_offset(o_args->PeleeNet__stage_2_dense_block_1__concat______muli___459_dram_pitch_value, dagbin_base, 
                    392U);

#endif

            /*** dmald() calls ***/
            break;
        case 12:
            /*** poke() calls ***/
            /* Intermediate buffers */
            // Poke HMB intermediate input buffer: PeleeNet__stage_2_dense_block_1__concat______muli___459
            coproc_poke_word_offset(dag_base + 2949120,
                dagbin_base, 348U);

            // Poke HMB intermediate output buffer: PeleeNet__stage_2_dense_block_2__concat
            coproc_poke_word_offset(dag_base + 0,
                dagbin_base, 388U);

            /* Pokable fields */
#ifdef PELEENET__STAGE_2_DENSE_BLOCK_1__CONCAT______MULI___459_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: PeleeNet__stage_2_dense_block_1__concat______muli___459_dram_pitch
            if (o_args->PeleeNet__stage_2_dense_block_1__concat______muli___459_dram_pitch_enable)
                coproc_poke_half_offset(o_args->PeleeNet__stage_2_dense_block_1__concat______muli___459_dram_pitch_value, dagbin_base, 
                    352U);

#endif

#ifdef PELEENET__STAGE_2_DENSE_BLOCK_2__CONCAT_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: PeleeNet__stage_2_dense_block_2__concat_dram_pitch
            if (o_args->PeleeNet__stage_2_dense_block_2__concat_dram_pitch_enable)
                coproc_poke_half_offset(o_args->PeleeNet__stage_2_dense_block_2__concat_dram_pitch_value, dagbin_base, 
                    392U);

#endif

            /*** dmald() calls ***/
            break;
        case 13:
            /*** poke() calls ***/
            /* Intermediate buffers */
            // Poke HMB intermediate input buffer: PeleeNet__stage_2_dense_block_2__concat
            coproc_poke_word_offset(dag_base + 0,
                dagbin_base, 276U);

            // Poke HMB intermediate output buffer: PeleeNet__stage_2_dense_block_2__concat___muli___462
            coproc_poke_word_offset(dag_base + 2293760,
                dagbin_base, 328U);

            // Poke HMB intermediate output buffer: PeleeNet__stage_2_dense_block_3__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu
            coproc_poke_word(dag_base + 4587520,
                dagbin_base + 512U);

            // Poke HMB intermediate output buffer: PeleeNet__stage_2_dense_block_3__conv_left_1__Conv2D___BatchNorm__FusedBatchNorm___Relu
            coproc_poke_word(dag_base + 4751360,
                dagbin_base + 644U);

            /* Pokable fields */
#ifdef PELEENET__STAGE_2_DENSE_BLOCK_2__CONCAT_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: PeleeNet__stage_2_dense_block_2__concat_dram_pitch
            if (o_args->PeleeNet__stage_2_dense_block_2__concat_dram_pitch_enable)
                coproc_poke_half_offset(o_args->PeleeNet__stage_2_dense_block_2__concat_dram_pitch_value, dagbin_base, 
                    280U);

#endif

#ifdef PELEENET__STAGE_2_DENSE_BLOCK_2__CONCAT___MULI___462_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: PeleeNet__stage_2_dense_block_2__concat___muli___462_dram_pitch
            if (o_args->PeleeNet__stage_2_dense_block_2__concat___muli___462_dram_pitch_enable)
                coproc_poke_half_offset(o_args->PeleeNet__stage_2_dense_block_2__concat___muli___462_dram_pitch_value, dagbin_base, 
                    332U);

#endif

#ifdef PELEENET__STAGE_2_DENSE_BLOCK_3__CONV_RIGHT_1__CONV2D___BATCHNORM__FUSEDBATCHNORM___RELU_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: PeleeNet__stage_2_dense_block_3__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_dram_pitch
            if (o_args->PeleeNet__stage_2_dense_block_3__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_dram_pitch_enable)
                coproc_poke_half(o_args->PeleeNet__stage_2_dense_block_3__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_dram_pitch_value, dagbin_base + 516U);

#endif

#ifdef PELEENET__STAGE_2_DENSE_BLOCK_3__CONV_LEFT_1__CONV2D___BATCHNORM__FUSEDBATCHNORM___RELU_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: PeleeNet__stage_2_dense_block_3__conv_left_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_dram_pitch
            if (o_args->PeleeNet__stage_2_dense_block_3__conv_left_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_dram_pitch_enable)
                coproc_poke_half(o_args->PeleeNet__stage_2_dense_block_3__conv_left_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_dram_pitch_value, dagbin_base + 648U);

#endif

            /*** dmald() calls ***/
            break;
        case 14:
            /*** poke() calls ***/
            /* Intermediate buffers */
            // Poke HMB intermediate input buffer: PeleeNet__stage_2_dense_block_2__concat___muli___462
            coproc_poke_word_offset(dag_base + 2293760,
                dagbin_base, 64U);

            // Poke HMB intermediate input buffer: PeleeNet__stage_2_dense_block_3__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu
            coproc_poke_word_offset(dag_base + 4587520,
                dagbin_base, 28U);

            // Poke HMB intermediate input buffer: PeleeNet__stage_2_dense_block_3__conv_left_1__Conv2D___BatchNorm__FusedBatchNorm___Relu
            coproc_poke_word_offset(dag_base + 4751360,
                dagbin_base, 104U);

            // Poke HMB intermediate output buffer: PeleeNet__stage_2_transition_layer__transition_layer_conv0__Conv2D___BatchNorm__FusedBatchNorm___Relu
            coproc_poke_word_offset(dag_base + 4915200,
                dagbin_base, 444U);

            /* Pokable fields */
#ifdef PELEENET__STAGE_2_DENSE_BLOCK_3__CONV_RIGHT_1__CONV2D___BATCHNORM__FUSEDBATCHNORM___RELU_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: PeleeNet__stage_2_dense_block_3__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_dram_pitch
            if (o_args->PeleeNet__stage_2_dense_block_3__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_dram_pitch_enable)
                coproc_poke_half_offset(o_args->PeleeNet__stage_2_dense_block_3__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_dram_pitch_value, dagbin_base, 
                    32U);

#endif

#ifdef PELEENET__STAGE_2_DENSE_BLOCK_2__CONCAT___MULI___462_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: PeleeNet__stage_2_dense_block_2__concat___muli___462_dram_pitch
            if (o_args->PeleeNet__stage_2_dense_block_2__concat___muli___462_dram_pitch_enable)
                coproc_poke_half_offset(o_args->PeleeNet__stage_2_dense_block_2__concat___muli___462_dram_pitch_value, dagbin_base, 
                    68U);

#endif

#ifdef PELEENET__STAGE_2_DENSE_BLOCK_3__CONV_LEFT_1__CONV2D___BATCHNORM__FUSEDBATCHNORM___RELU_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: PeleeNet__stage_2_dense_block_3__conv_left_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_dram_pitch
            if (o_args->PeleeNet__stage_2_dense_block_3__conv_left_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_dram_pitch_enable)
                coproc_poke_half_offset(o_args->PeleeNet__stage_2_dense_block_3__conv_left_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_dram_pitch_value, dagbin_base, 
                    108U);

#endif

#ifdef PELEENET__STAGE_2_TRANSITION_LAYER__TRANSITION_LAYER_CONV0__CONV2D___BATCHNORM__FUSEDBATCHNORM___RELU_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: PeleeNet__stage_2_transition_layer__transition_layer_conv0__Conv2D___BatchNorm__FusedBatchNorm___Relu_dram_pitch
            if (o_args->PeleeNet__stage_2_transition_layer__transition_layer_conv0__Conv2D___BatchNorm__FusedBatchNorm___Relu_dram_pitch_enable)
                coproc_poke_half_offset(o_args->PeleeNet__stage_2_transition_layer__transition_layer_conv0__Conv2D___BatchNorm__FusedBatchNorm___Relu_dram_pitch_value, dagbin_base, 
                    448U);

#endif

            /*** dmald() calls ***/
            break;
        case 15:
            /*** poke() calls ***/
            /* Intermediate buffers */
            // Poke HMB intermediate input buffer: PeleeNet__stage_2_transition_layer__transition_layer_conv0__Conv2D___BatchNorm__FusedBatchNorm___Relu
            coproc_poke_word_offset(dag_base + 4915200,
                dagbin_base, 28U);

            // Poke HMB intermediate output buffer: PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool______muli___465___Mean_src0_nhwc___nhwc___transpose___66
            coproc_poke_word(dag_base + 0,
                dagbin_base + 712U);

            /* Pokable fields */
#ifdef PELEENET__STAGE_2_TRANSITION_LAYER__TRANSITION_LAYER_CONV0__CONV2D___BATCHNORM__FUSEDBATCHNORM___RELU_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: PeleeNet__stage_2_transition_layer__transition_layer_conv0__Conv2D___BatchNorm__FusedBatchNorm___Relu_dram_pitch
            if (o_args->PeleeNet__stage_2_transition_layer__transition_layer_conv0__Conv2D___BatchNorm__FusedBatchNorm___Relu_dram_pitch_enable)
                coproc_poke_half_offset(o_args->PeleeNet__stage_2_transition_layer__transition_layer_conv0__Conv2D___BatchNorm__FusedBatchNorm___Relu_dram_pitch_value, dagbin_base, 
                    32U);

#endif

#ifdef PELEENET__STAGE_2_TRANSITION_LAYER__TRANSITION_LAYER_AVGPOOL__AVGPOOL______MULI___465___MEAN_SRC0_NHWC___NHWC___TRANSPOSE___66_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool______muli___465___Mean_src0_nhwc___nhwc___transpose___66_dram_pitch
            if (o_args->PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool______muli___465___Mean_src0_nhwc___nhwc___transpose___66_dram_pitch_enable)
                coproc_poke_half(o_args->PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool______muli___465___Mean_src0_nhwc___nhwc___transpose___66_dram_pitch_value, dagbin_base + 716U);

#endif

            /*** dmald() calls ***/
            break;
        case 16:
            /*** poke() calls ***/
            /* Intermediate buffers */
            // Poke HMB intermediate input buffer: PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool______muli___465___Mean_src0_nhwc___nhwc___transpose___66
            coproc_poke_word_offset(dag_base + 0,
                dagbin_base, 28U);

            // Poke HMB intermediate output buffer: PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool______muli___465___Mean_src0_nhwc___nhwc___transpose___66_1243_
            coproc_poke_word_offset(dag_base + 655360,
                dagbin_base, 76U);

            /* Pokable fields */
#ifdef PELEENET__STAGE_2_TRANSITION_LAYER__TRANSITION_LAYER_AVGPOOL__AVGPOOL______MULI___465___MEAN_SRC0_NHWC___NHWC___TRANSPOSE___66_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool______muli___465___Mean_src0_nhwc___nhwc___transpose___66_dram_pitch
            if (o_args->PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool______muli___465___Mean_src0_nhwc___nhwc___transpose___66_dram_pitch_enable)
                coproc_poke_half_offset(o_args->PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool______muli___465___Mean_src0_nhwc___nhwc___transpose___66_dram_pitch_value, dagbin_base, 
                    32U);

#endif

#ifdef PELEENET__STAGE_2_TRANSITION_LAYER__TRANSITION_LAYER_AVGPOOL__AVGPOOL______MULI___465___MEAN_SRC0_NHWC___NHWC___TRANSPOSE___66_1243__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool______muli___465___Mean_src0_nhwc___nhwc___transpose___66_1243__dram_pitch
            if (o_args->PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool______muli___465___Mean_src0_nhwc___nhwc___transpose___66_1243__dram_pitch_enable)
                coproc_poke_half_offset(o_args->PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool______muli___465___Mean_src0_nhwc___nhwc___transpose___66_1243__dram_pitch_value, dagbin_base, 
                    80U);

#endif

            /*** dmald() calls ***/
            break;
        case 17:
            /*** poke() calls ***/
            /* Intermediate buffers */
            // Poke HMB intermediate input buffer: PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool______muli___465___Mean_src0_nhwc___nhwc___transpose___66
            coproc_poke_word_offset(dag_base + 0,
                dagbin_base, 412U);

            // Poke HMB intermediate input buffer: PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool______muli___465___Mean_src0_nhwc___nhwc___transpose___66_1243_
            coproc_poke_word_offset(dag_base + 655360,
                dagbin_base, 28U);

            // Poke HMB intermediate output buffer: ResizeBilinear
            coproc_poke_word(dag_base + 1703936,
                dagbin_base + 1044U);

            // Poke HMB intermediate output buffer: PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool___muli___466
            coproc_poke_word(dag_base + 6946816,
                dagbin_base + 1092U);

            // Poke HMB intermediate output buffer: PeleeNet__stage_3_dense_block_0__conv_right_0__Relu
            coproc_poke_word(dag_base + 7602176,
                dagbin_base + 1228U);

            // Poke HMB intermediate output buffer: PeleeNet__stage_3_dense_block_0__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm___muli___467___8
            coproc_poke_word(dag_base + 13107200,
                dagbin_base + 1144U);

            /* Pokable fields */
#ifdef PELEENET__STAGE_2_TRANSITION_LAYER__TRANSITION_LAYER_AVGPOOL__AVGPOOL______MULI___465___MEAN_SRC0_NHWC___NHWC___TRANSPOSE___66_1243__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool______muli___465___Mean_src0_nhwc___nhwc___transpose___66_1243__dram_pitch
            if (o_args->PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool______muli___465___Mean_src0_nhwc___nhwc___transpose___66_1243__dram_pitch_enable)
                coproc_poke_half_offset(o_args->PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool______muli___465___Mean_src0_nhwc___nhwc___transpose___66_1243__dram_pitch_value, dagbin_base, 
                    32U);

#endif

#ifdef PELEENET__STAGE_2_TRANSITION_LAYER__TRANSITION_LAYER_AVGPOOL__AVGPOOL______MULI___465___MEAN_SRC0_NHWC___NHWC___TRANSPOSE___66_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool______muli___465___Mean_src0_nhwc___nhwc___transpose___66_dram_pitch
            if (o_args->PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool______muli___465___Mean_src0_nhwc___nhwc___transpose___66_dram_pitch_enable)
                coproc_poke_half_offset(o_args->PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool______muli___465___Mean_src0_nhwc___nhwc___transpose___66_dram_pitch_value, dagbin_base, 
                    416U);

#endif

#ifdef RESIZEBILINEAR_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: ResizeBilinear_dram_pitch
            if (o_args->ResizeBilinear_dram_pitch_enable)
                coproc_poke_half(o_args->ResizeBilinear_dram_pitch_value, dagbin_base + 1048U);

#endif

#ifdef PELEENET__STAGE_2_TRANSITION_LAYER__TRANSITION_LAYER_AVGPOOL__AVGPOOL___MULI___466_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool___muli___466_dram_pitch
            if (o_args->PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool___muli___466_dram_pitch_enable)
                coproc_poke_half(o_args->PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool___muli___466_dram_pitch_value, dagbin_base + 1096U);

#endif

#ifdef PELEENET__STAGE_3_DENSE_BLOCK_0__CONV_LEFT_0__CONV2D___BATCHNORM__FUSEDBATCHNORM___RIGHT_0__CONV2D___BATCHNORM__FUSEDBATCHNORM___MULI___467___8_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: PeleeNet__stage_3_dense_block_0__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm___muli___467___8_dram_pitch
            if (o_args->PeleeNet__stage_3_dense_block_0__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm___muli___467___8_dram_pitch_enable)
                coproc_poke_half(o_args->PeleeNet__stage_3_dense_block_0__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm___muli___467___8_dram_pitch_value, dagbin_base + 1148U);

#endif

#ifdef PELEENET__STAGE_3_DENSE_BLOCK_0__CONV_RIGHT_0__RELU_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: PeleeNet__stage_3_dense_block_0__conv_right_0__Relu_dram_pitch
            if (o_args->PeleeNet__stage_3_dense_block_0__conv_right_0__Relu_dram_pitch_enable)
                coproc_poke_half(o_args->PeleeNet__stage_3_dense_block_0__conv_right_0__Relu_dram_pitch_value, dagbin_base + 1232U);

#endif

            /*** dmald() calls ***/
            break;
        case 18:
            /*** poke() calls ***/
            /* Intermediate buffers */
            // Poke HMB intermediate input buffer: PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool___muli___466
            coproc_poke_word_offset(dag_base + 6946816,
                dagbin_base, 28U);

            // Poke HMB intermediate input buffer: PeleeNet__stage_3_dense_block_0__conv_right_0__Relu
            coproc_poke_word_offset(dag_base + 7602176,
                dagbin_base, 108U);

            // Poke HMB intermediate input buffer: PeleeNet__stage_3_dense_block_0__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm___muli___467___8
            coproc_poke_word_offset(dag_base + 13107200,
                dagbin_base, 68U);

            // Poke HMB intermediate output buffer: PeleeNet__stage_3_dense_block_0__concat______muli___469
            coproc_poke_word(dag_base + 0,
                dagbin_base + 892U);

            // Poke HMB intermediate output buffer: PeleeNet__stage_3_dense_block_1__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu
            coproc_poke_word(dag_base + 737280,
                dagbin_base + 1124U);

            // Poke HMB intermediate output buffer: PeleeNet__stage_3_dense_block_1__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___crop___400
            coproc_poke_word(dag_base + 778240,
                dagbin_base + 1176U);

            /* Pokable fields */
#ifdef PELEENET__STAGE_2_TRANSITION_LAYER__TRANSITION_LAYER_AVGPOOL__AVGPOOL___MULI___466_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool___muli___466_dram_pitch
            if (o_args->PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool___muli___466_dram_pitch_enable)
                coproc_poke_half_offset(o_args->PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool___muli___466_dram_pitch_value, dagbin_base, 
                    32U);

#endif

#ifdef PELEENET__STAGE_3_DENSE_BLOCK_0__CONV_LEFT_0__CONV2D___BATCHNORM__FUSEDBATCHNORM___RIGHT_0__CONV2D___BATCHNORM__FUSEDBATCHNORM___MULI___467___8_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: PeleeNet__stage_3_dense_block_0__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm___muli___467___8_dram_pitch
            if (o_args->PeleeNet__stage_3_dense_block_0__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm___muli___467___8_dram_pitch_enable)
                coproc_poke_half_offset(o_args->PeleeNet__stage_3_dense_block_0__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm___muli___467___8_dram_pitch_value, dagbin_base, 
                    72U);

#endif

#ifdef PELEENET__STAGE_3_DENSE_BLOCK_0__CONV_RIGHT_0__RELU_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: PeleeNet__stage_3_dense_block_0__conv_right_0__Relu_dram_pitch
            if (o_args->PeleeNet__stage_3_dense_block_0__conv_right_0__Relu_dram_pitch_enable)
                coproc_poke_half_offset(o_args->PeleeNet__stage_3_dense_block_0__conv_right_0__Relu_dram_pitch_value, dagbin_base, 
                    112U);

#endif

#ifdef PELEENET__STAGE_3_DENSE_BLOCK_0__CONCAT______MULI___469_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: PeleeNet__stage_3_dense_block_0__concat______muli___469_dram_pitch
            if (o_args->PeleeNet__stage_3_dense_block_0__concat______muli___469_dram_pitch_enable)
                coproc_poke_half(o_args->PeleeNet__stage_3_dense_block_0__concat______muli___469_dram_pitch_value, dagbin_base + 896U);

#endif

#ifdef PELEENET__STAGE_3_DENSE_BLOCK_1__CONV_RIGHT_2__CONV2D___BATCHNORM__FUSEDBATCHNORM___RELU_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: PeleeNet__stage_3_dense_block_1__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu_dram_pitch
            if (o_args->PeleeNet__stage_3_dense_block_1__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu_dram_pitch_enable)
                coproc_poke_half(o_args->PeleeNet__stage_3_dense_block_1__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu_dram_pitch_value, dagbin_base + 1128U);

#endif

#ifdef PELEENET__STAGE_3_DENSE_BLOCK_1__CONV_LEFT_0__CONV2D___BATCHNORM__FUSEDBATCHNORM___RIGHT_0__CONV2D___CROP___400_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: PeleeNet__stage_3_dense_block_1__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___crop___400_dram_pitch
            if (o_args->PeleeNet__stage_3_dense_block_1__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___crop___400_dram_pitch_enable)
                coproc_poke_half(o_args->PeleeNet__stage_3_dense_block_1__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___crop___400_dram_pitch_value, dagbin_base + 1180U);

#endif

            /*** dmald() calls ***/
            break;
        case 19:
            /*** poke() calls ***/
            /* Intermediate buffers */
            // Poke HMB intermediate input buffer: PeleeNet__stage_3_dense_block_0__concat______muli___469
            coproc_poke_word_offset(dag_base + 0,
                dagbin_base, 28U);

            // Poke HMB intermediate input buffer: PeleeNet__stage_3_dense_block_1__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu
            coproc_poke_word_offset(dag_base + 737280,
                dagbin_base, 68U);

            // Poke HMB intermediate input buffer: PeleeNet__stage_3_dense_block_1__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___crop___400
            coproc_poke_word_offset(dag_base + 778240,
                dagbin_base, 108U);

            // Poke HMB intermediate output buffer: PeleeNet__stage_3_dense_block_2__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu
            coproc_poke_word(dag_base + 942080,
                dagbin_base + 868U);

            // Poke HMB intermediate output buffer: PeleeNet__stage_3_dense_block_2__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___crop___402
            coproc_poke_word(dag_base + 983040,
                dagbin_base + 920U);

            // Poke HMB intermediate output buffer: PeleeNet__stage_3_dense_block_1__concat______muli___472
            coproc_poke_word_offset(dag_base + 6946816,
                dagbin_base, 476U);

            /* Pokable fields */
#ifdef PELEENET__STAGE_3_DENSE_BLOCK_0__CONCAT______MULI___469_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: PeleeNet__stage_3_dense_block_0__concat______muli___469_dram_pitch
            if (o_args->PeleeNet__stage_3_dense_block_0__concat______muli___469_dram_pitch_enable)
                coproc_poke_half_offset(o_args->PeleeNet__stage_3_dense_block_0__concat______muli___469_dram_pitch_value, dagbin_base, 
                    32U);

#endif

#ifdef PELEENET__STAGE_3_DENSE_BLOCK_1__CONV_RIGHT_2__CONV2D___BATCHNORM__FUSEDBATCHNORM___RELU_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: PeleeNet__stage_3_dense_block_1__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu_dram_pitch
            if (o_args->PeleeNet__stage_3_dense_block_1__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu_dram_pitch_enable)
                coproc_poke_half_offset(o_args->PeleeNet__stage_3_dense_block_1__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu_dram_pitch_value, dagbin_base, 
                    72U);

#endif

#ifdef PELEENET__STAGE_3_DENSE_BLOCK_1__CONV_LEFT_0__CONV2D___BATCHNORM__FUSEDBATCHNORM___RIGHT_0__CONV2D___CROP___400_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: PeleeNet__stage_3_dense_block_1__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___crop___400_dram_pitch
            if (o_args->PeleeNet__stage_3_dense_block_1__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___crop___400_dram_pitch_enable)
                coproc_poke_half_offset(o_args->PeleeNet__stage_3_dense_block_1__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___crop___400_dram_pitch_value, dagbin_base, 
                    112U);

#endif

#ifdef PELEENET__STAGE_3_DENSE_BLOCK_1__CONCAT______MULI___472_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: PeleeNet__stage_3_dense_block_1__concat______muli___472_dram_pitch
            if (o_args->PeleeNet__stage_3_dense_block_1__concat______muli___472_dram_pitch_enable)
                coproc_poke_half_offset(o_args->PeleeNet__stage_3_dense_block_1__concat______muli___472_dram_pitch_value, dagbin_base, 
                    480U);

#endif

#ifdef PELEENET__STAGE_3_DENSE_BLOCK_2__CONV_RIGHT_2__CONV2D___BATCHNORM__FUSEDBATCHNORM___RELU_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: PeleeNet__stage_3_dense_block_2__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu_dram_pitch
            if (o_args->PeleeNet__stage_3_dense_block_2__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu_dram_pitch_enable)
                coproc_poke_half(o_args->PeleeNet__stage_3_dense_block_2__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu_dram_pitch_value, dagbin_base + 872U);

#endif

#ifdef PELEENET__STAGE_3_DENSE_BLOCK_2__CONV_LEFT_0__CONV2D___BATCHNORM__FUSEDBATCHNORM___RIGHT_0__CONV2D___CROP___402_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: PeleeNet__stage_3_dense_block_2__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___crop___402_dram_pitch
            if (o_args->PeleeNet__stage_3_dense_block_2__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___crop___402_dram_pitch_enable)
                coproc_poke_half(o_args->PeleeNet__stage_3_dense_block_2__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___crop___402_dram_pitch_value, dagbin_base + 924U);

#endif

            /*** dmald() calls ***/
            break;
        case 20:
            /*** poke() calls ***/
            /* Intermediate buffers */
            // Poke HMB intermediate input buffer: PeleeNet__stage_3_dense_block_2__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu
            coproc_poke_word_offset(dag_base + 942080,
                dagbin_base, 68U);

            // Poke HMB intermediate input buffer: PeleeNet__stage_3_dense_block_2__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___crop___402
            coproc_poke_word_offset(dag_base + 983040,
                dagbin_base, 108U);

            // Poke HMB intermediate input buffer: PeleeNet__stage_3_dense_block_1__concat______muli___472
            coproc_poke_word_offset(dag_base + 6946816,
                dagbin_base, 28U);

            // Poke HMB intermediate output buffer: PeleeNet__stage_3_dense_block_2__concat______muli___475
            coproc_poke_word_offset(dag_base + 0,
                dagbin_base, 224U);

            /* Pokable fields */
#ifdef PELEENET__STAGE_3_DENSE_BLOCK_1__CONCAT______MULI___472_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: PeleeNet__stage_3_dense_block_1__concat______muli___472_dram_pitch
            if (o_args->PeleeNet__stage_3_dense_block_1__concat______muli___472_dram_pitch_enable)
                coproc_poke_half_offset(o_args->PeleeNet__stage_3_dense_block_1__concat______muli___472_dram_pitch_value, dagbin_base, 
                    32U);

#endif

#ifdef PELEENET__STAGE_3_DENSE_BLOCK_2__CONV_RIGHT_2__CONV2D___BATCHNORM__FUSEDBATCHNORM___RELU_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: PeleeNet__stage_3_dense_block_2__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu_dram_pitch
            if (o_args->PeleeNet__stage_3_dense_block_2__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu_dram_pitch_enable)
                coproc_poke_half_offset(o_args->PeleeNet__stage_3_dense_block_2__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu_dram_pitch_value, dagbin_base, 
                    72U);

#endif

#ifdef PELEENET__STAGE_3_DENSE_BLOCK_2__CONV_LEFT_0__CONV2D___BATCHNORM__FUSEDBATCHNORM___RIGHT_0__CONV2D___CROP___402_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: PeleeNet__stage_3_dense_block_2__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___crop___402_dram_pitch
            if (o_args->PeleeNet__stage_3_dense_block_2__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___crop___402_dram_pitch_enable)
                coproc_poke_half_offset(o_args->PeleeNet__stage_3_dense_block_2__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___crop___402_dram_pitch_value, dagbin_base, 
                    112U);

#endif

#ifdef PELEENET__STAGE_3_DENSE_BLOCK_2__CONCAT______MULI___475_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: PeleeNet__stage_3_dense_block_2__concat______muli___475_dram_pitch
            if (o_args->PeleeNet__stage_3_dense_block_2__concat______muli___475_dram_pitch_enable)
                coproc_poke_half_offset(o_args->PeleeNet__stage_3_dense_block_2__concat______muli___475_dram_pitch_value, dagbin_base, 
                    228U);

#endif

            /*** dmald() calls ***/
            break;
        case 21:
            /*** poke() calls ***/
            /* Intermediate buffers */
            // Poke HMB intermediate input buffer: PeleeNet__stage_3_dense_block_2__concat______muli___475
            coproc_poke_word_offset(dag_base + 0,
                dagbin_base, 344U);

            // Poke HMB intermediate output buffer: PeleeNet__stage_3_dense_block_3__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu
            coproc_poke_word(dag_base + 901120,
                dagbin_base + 576U);

            // Poke HMB intermediate output buffer: PeleeNet__stage_3_dense_block_3__conv_left_1__Conv2D___BatchNorm__FusedBatchNorm___Relu
            coproc_poke_word(dag_base + 942080,
                dagbin_base + 708U);

            /* Pokable fields */
#ifdef PELEENET__STAGE_3_DENSE_BLOCK_2__CONCAT______MULI___475_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: PeleeNet__stage_3_dense_block_2__concat______muli___475_dram_pitch
            if (o_args->PeleeNet__stage_3_dense_block_2__concat______muli___475_dram_pitch_enable)
                coproc_poke_half_offset(o_args->PeleeNet__stage_3_dense_block_2__concat______muli___475_dram_pitch_value, dagbin_base, 
                    348U);

#endif

#ifdef PELEENET__STAGE_3_DENSE_BLOCK_3__CONV_RIGHT_2__CONV2D___BATCHNORM__FUSEDBATCHNORM___RELU_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: PeleeNet__stage_3_dense_block_3__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu_dram_pitch
            if (o_args->PeleeNet__stage_3_dense_block_3__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu_dram_pitch_enable)
                coproc_poke_half(o_args->PeleeNet__stage_3_dense_block_3__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu_dram_pitch_value, dagbin_base + 580U);

#endif

#ifdef PELEENET__STAGE_3_DENSE_BLOCK_3__CONV_LEFT_1__CONV2D___BATCHNORM__FUSEDBATCHNORM___RELU_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: PeleeNet__stage_3_dense_block_3__conv_left_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_dram_pitch
            if (o_args->PeleeNet__stage_3_dense_block_3__conv_left_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_dram_pitch_enable)
                coproc_poke_half(o_args->PeleeNet__stage_3_dense_block_3__conv_left_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_dram_pitch_value, dagbin_base + 712U);

#endif

            /*** dmald() calls ***/
            break;
        case 22:
            /*** poke() calls ***/
            /* Intermediate buffers */
            // Poke HMB intermediate input buffer: PeleeNet__stage_3_dense_block_2__concat______muli___475
            coproc_poke_word_offset(dag_base + 0,
                dagbin_base, 104U);

            // Poke HMB intermediate input buffer: PeleeNet__stage_3_dense_block_3__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu
            coproc_poke_word_offset(dag_base + 901120,
                dagbin_base, 28U);

            // Poke HMB intermediate input buffer: PeleeNet__stage_3_dense_block_3__conv_left_1__Conv2D___BatchNorm__FusedBatchNorm___Relu
            coproc_poke_word_offset(dag_base + 942080,
                dagbin_base, 68U);

            // Poke HMB intermediate output buffer: PeleeNet__stage_3_dense_block_3__concat______muli___478
            coproc_poke_word_offset(dag_base + 13107200,
                dagbin_base, 144U);

            /* Pokable fields */
#ifdef PELEENET__STAGE_3_DENSE_BLOCK_3__CONV_RIGHT_2__CONV2D___BATCHNORM__FUSEDBATCHNORM___RELU_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: PeleeNet__stage_3_dense_block_3__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu_dram_pitch
            if (o_args->PeleeNet__stage_3_dense_block_3__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu_dram_pitch_enable)
                coproc_poke_half_offset(o_args->PeleeNet__stage_3_dense_block_3__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu_dram_pitch_value, dagbin_base, 
                    32U);

#endif

#ifdef PELEENET__STAGE_3_DENSE_BLOCK_3__CONV_LEFT_1__CONV2D___BATCHNORM__FUSEDBATCHNORM___RELU_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: PeleeNet__stage_3_dense_block_3__conv_left_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_dram_pitch
            if (o_args->PeleeNet__stage_3_dense_block_3__conv_left_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_dram_pitch_enable)
                coproc_poke_half_offset(o_args->PeleeNet__stage_3_dense_block_3__conv_left_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_dram_pitch_value, dagbin_base, 
                    72U);

#endif

#ifdef PELEENET__STAGE_3_DENSE_BLOCK_2__CONCAT______MULI___475_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: PeleeNet__stage_3_dense_block_2__concat______muli___475_dram_pitch
            if (o_args->PeleeNet__stage_3_dense_block_2__concat______muli___475_dram_pitch_enable)
                coproc_poke_half_offset(o_args->PeleeNet__stage_3_dense_block_2__concat______muli___475_dram_pitch_value, dagbin_base, 
                    108U);

#endif

#ifdef PELEENET__STAGE_3_DENSE_BLOCK_3__CONCAT______MULI___478_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: PeleeNet__stage_3_dense_block_3__concat______muli___478_dram_pitch
            if (o_args->PeleeNet__stage_3_dense_block_3__concat______muli___478_dram_pitch_enable)
                coproc_poke_half_offset(o_args->PeleeNet__stage_3_dense_block_3__concat______muli___478_dram_pitch_value, dagbin_base, 
                    148U);

#endif

            /*** dmald() calls ***/
            break;
        case 23:
            /*** poke() calls ***/
            /* Intermediate buffers */
            // Poke HMB intermediate input buffer: PeleeNet__stage_3_dense_block_3__concat______muli___478
            coproc_poke_word_offset(dag_base + 13107200,
                dagbin_base, 356U);

            // Poke HMB intermediate output buffer: PeleeNet__stage_3_dense_block_4__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu
            coproc_poke_word(dag_base + 0,
                dagbin_base + 636U);

            // Poke HMB intermediate output buffer: PeleeNet__stage_3_dense_block_4__conv_left_1__Conv2D___BatchNorm__FusedBatchNorm___Relu
            coproc_poke_word(dag_base + 40960,
                dagbin_base + 768U);

            /* Pokable fields */
#ifdef PELEENET__STAGE_3_DENSE_BLOCK_3__CONCAT______MULI___478_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: PeleeNet__stage_3_dense_block_3__concat______muli___478_dram_pitch
            if (o_args->PeleeNet__stage_3_dense_block_3__concat______muli___478_dram_pitch_enable)
                coproc_poke_half_offset(o_args->PeleeNet__stage_3_dense_block_3__concat______muli___478_dram_pitch_value, dagbin_base, 
                    360U);

#endif

#ifdef PELEENET__STAGE_3_DENSE_BLOCK_4__CONV_RIGHT_2__CONV2D___BATCHNORM__FUSEDBATCHNORM___RELU_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: PeleeNet__stage_3_dense_block_4__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu_dram_pitch
            if (o_args->PeleeNet__stage_3_dense_block_4__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu_dram_pitch_enable)
                coproc_poke_half(o_args->PeleeNet__stage_3_dense_block_4__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu_dram_pitch_value, dagbin_base + 640U);

#endif

#ifdef PELEENET__STAGE_3_DENSE_BLOCK_4__CONV_LEFT_1__CONV2D___BATCHNORM__FUSEDBATCHNORM___RELU_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: PeleeNet__stage_3_dense_block_4__conv_left_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_dram_pitch
            if (o_args->PeleeNet__stage_3_dense_block_4__conv_left_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_dram_pitch_enable)
                coproc_poke_half(o_args->PeleeNet__stage_3_dense_block_4__conv_left_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_dram_pitch_value, dagbin_base + 772U);

#endif

            /*** dmald() calls ***/
            break;
        case 24:
            /*** poke() calls ***/
            /* Intermediate buffers */
            // Poke HMB intermediate input buffer: PeleeNet__stage_3_dense_block_4__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu
            coproc_poke_word_offset(dag_base + 0,
                dagbin_base, 28U);

            // Poke HMB intermediate input buffer: PeleeNet__stage_3_dense_block_4__conv_left_1__Conv2D___BatchNorm__FusedBatchNorm___Relu
            coproc_poke_word_offset(dag_base + 40960,
                dagbin_base, 68U);

            // Poke HMB intermediate input buffer: PeleeNet__stage_3_dense_block_3__concat______muli___478
            coproc_poke_word_offset(dag_base + 13107200,
                dagbin_base, 104U);

            // Poke HMB intermediate output buffer: PeleeNet__stage_3_dense_block_4__concat______muli___481
            coproc_poke_word_offset(dag_base + 81920,
                dagbin_base, 144U);

            /* Pokable fields */
#ifdef PELEENET__STAGE_3_DENSE_BLOCK_4__CONV_RIGHT_2__CONV2D___BATCHNORM__FUSEDBATCHNORM___RELU_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: PeleeNet__stage_3_dense_block_4__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu_dram_pitch
            if (o_args->PeleeNet__stage_3_dense_block_4__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu_dram_pitch_enable)
                coproc_poke_half_offset(o_args->PeleeNet__stage_3_dense_block_4__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu_dram_pitch_value, dagbin_base, 
                    32U);

#endif

#ifdef PELEENET__STAGE_3_DENSE_BLOCK_4__CONV_LEFT_1__CONV2D___BATCHNORM__FUSEDBATCHNORM___RELU_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: PeleeNet__stage_3_dense_block_4__conv_left_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_dram_pitch
            if (o_args->PeleeNet__stage_3_dense_block_4__conv_left_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_dram_pitch_enable)
                coproc_poke_half_offset(o_args->PeleeNet__stage_3_dense_block_4__conv_left_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_dram_pitch_value, dagbin_base, 
                    72U);

#endif

#ifdef PELEENET__STAGE_3_DENSE_BLOCK_3__CONCAT______MULI___478_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: PeleeNet__stage_3_dense_block_3__concat______muli___478_dram_pitch
            if (o_args->PeleeNet__stage_3_dense_block_3__concat______muli___478_dram_pitch_enable)
                coproc_poke_half_offset(o_args->PeleeNet__stage_3_dense_block_3__concat______muli___478_dram_pitch_value, dagbin_base, 
                    108U);

#endif

#ifdef PELEENET__STAGE_3_DENSE_BLOCK_4__CONCAT______MULI___481_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: PeleeNet__stage_3_dense_block_4__concat______muli___481_dram_pitch
            if (o_args->PeleeNet__stage_3_dense_block_4__concat______muli___481_dram_pitch_enable)
                coproc_poke_half_offset(o_args->PeleeNet__stage_3_dense_block_4__concat______muli___481_dram_pitch_value, dagbin_base, 
                    148U);

#endif

            /*** dmald() calls ***/
            break;
        case 25:
            /*** poke() calls ***/
            /* Intermediate buffers */
            // Poke HMB intermediate input buffer: PeleeNet__stage_3_dense_block_4__concat______muli___481
            coproc_poke_word_offset(dag_base + 81920,
                dagbin_base, 356U);

            // Poke HMB intermediate output buffer: PeleeNet__stage_3_dense_block_5__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu
            coproc_poke_word(dag_base + 0,
                dagbin_base + 636U);

            // Poke HMB intermediate output buffer: PeleeNet__stage_3_dense_block_5__conv_left_1__Conv2D___BatchNorm__FusedBatchNorm___Relu
            coproc_poke_word(dag_base + 40960,
                dagbin_base + 768U);

            /* Pokable fields */
#ifdef PELEENET__STAGE_3_DENSE_BLOCK_4__CONCAT______MULI___481_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: PeleeNet__stage_3_dense_block_4__concat______muli___481_dram_pitch
            if (o_args->PeleeNet__stage_3_dense_block_4__concat______muli___481_dram_pitch_enable)
                coproc_poke_half_offset(o_args->PeleeNet__stage_3_dense_block_4__concat______muli___481_dram_pitch_value, dagbin_base, 
                    360U);

#endif

#ifdef PELEENET__STAGE_3_DENSE_BLOCK_5__CONV_RIGHT_2__CONV2D___BATCHNORM__FUSEDBATCHNORM___RELU_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: PeleeNet__stage_3_dense_block_5__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu_dram_pitch
            if (o_args->PeleeNet__stage_3_dense_block_5__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu_dram_pitch_enable)
                coproc_poke_half(o_args->PeleeNet__stage_3_dense_block_5__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu_dram_pitch_value, dagbin_base + 640U);

#endif

#ifdef PELEENET__STAGE_3_DENSE_BLOCK_5__CONV_LEFT_1__CONV2D___BATCHNORM__FUSEDBATCHNORM___RELU_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: PeleeNet__stage_3_dense_block_5__conv_left_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_dram_pitch
            if (o_args->PeleeNet__stage_3_dense_block_5__conv_left_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_dram_pitch_enable)
                coproc_poke_half(o_args->PeleeNet__stage_3_dense_block_5__conv_left_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_dram_pitch_value, dagbin_base + 772U);

#endif

            /*** dmald() calls ***/
            break;
        case 26:
            /*** poke() calls ***/
            /* Intermediate buffers */
            // Poke HMB intermediate input buffer: PeleeNet__stage_3_dense_block_5__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu
            coproc_poke_word_offset(dag_base + 0,
                dagbin_base, 28U);

            // Poke HMB intermediate input buffer: PeleeNet__stage_3_dense_block_5__conv_left_1__Conv2D___BatchNorm__FusedBatchNorm___Relu
            coproc_poke_word_offset(dag_base + 40960,
                dagbin_base, 68U);

            // Poke HMB intermediate input buffer: PeleeNet__stage_3_dense_block_4__concat______muli___481
            coproc_poke_word_offset(dag_base + 81920,
                dagbin_base, 104U);

            // Poke HMB intermediate output buffer: PeleeNet__stage_3_dense_block_5__concat______muli___484
            coproc_poke_word_offset(dag_base + 13107200,
                dagbin_base, 144U);

            /* Pokable fields */
#ifdef PELEENET__STAGE_3_DENSE_BLOCK_5__CONV_RIGHT_2__CONV2D___BATCHNORM__FUSEDBATCHNORM___RELU_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: PeleeNet__stage_3_dense_block_5__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu_dram_pitch
            if (o_args->PeleeNet__stage_3_dense_block_5__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu_dram_pitch_enable)
                coproc_poke_half_offset(o_args->PeleeNet__stage_3_dense_block_5__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu_dram_pitch_value, dagbin_base, 
                    32U);

#endif

#ifdef PELEENET__STAGE_3_DENSE_BLOCK_5__CONV_LEFT_1__CONV2D___BATCHNORM__FUSEDBATCHNORM___RELU_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: PeleeNet__stage_3_dense_block_5__conv_left_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_dram_pitch
            if (o_args->PeleeNet__stage_3_dense_block_5__conv_left_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_dram_pitch_enable)
                coproc_poke_half_offset(o_args->PeleeNet__stage_3_dense_block_5__conv_left_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_dram_pitch_value, dagbin_base, 
                    72U);

#endif

#ifdef PELEENET__STAGE_3_DENSE_BLOCK_4__CONCAT______MULI___481_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: PeleeNet__stage_3_dense_block_4__concat______muli___481_dram_pitch
            if (o_args->PeleeNet__stage_3_dense_block_4__concat______muli___481_dram_pitch_enable)
                coproc_poke_half_offset(o_args->PeleeNet__stage_3_dense_block_4__concat______muli___481_dram_pitch_value, dagbin_base, 
                    108U);

#endif

#ifdef PELEENET__STAGE_3_DENSE_BLOCK_5__CONCAT______MULI___484_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: PeleeNet__stage_3_dense_block_5__concat______muli___484_dram_pitch
            if (o_args->PeleeNet__stage_3_dense_block_5__concat______muli___484_dram_pitch_enable)
                coproc_poke_half_offset(o_args->PeleeNet__stage_3_dense_block_5__concat______muli___484_dram_pitch_value, dagbin_base, 
                    148U);

#endif

            /*** dmald() calls ***/
            break;
        case 27:
            /*** poke() calls ***/
            /* Intermediate buffers */
            // Poke HMB intermediate input buffer: PeleeNet__stage_3_dense_block_5__concat______muli___484
            coproc_poke_word_offset(dag_base + 13107200,
                dagbin_base, 356U);

            // Poke HMB intermediate output buffer: PeleeNet__stage_3_dense_block_6__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu
            coproc_poke_word(dag_base + 0,
                dagbin_base + 636U);

            // Poke HMB intermediate output buffer: PeleeNet__stage_3_dense_block_6__conv_left_1__Conv2D___BatchNorm__FusedBatchNorm___Relu
            coproc_poke_word(dag_base + 40960,
                dagbin_base + 768U);

            /* Pokable fields */
#ifdef PELEENET__STAGE_3_DENSE_BLOCK_5__CONCAT______MULI___484_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: PeleeNet__stage_3_dense_block_5__concat______muli___484_dram_pitch
            if (o_args->PeleeNet__stage_3_dense_block_5__concat______muli___484_dram_pitch_enable)
                coproc_poke_half_offset(o_args->PeleeNet__stage_3_dense_block_5__concat______muli___484_dram_pitch_value, dagbin_base, 
                    360U);

#endif

#ifdef PELEENET__STAGE_3_DENSE_BLOCK_6__CONV_RIGHT_2__CONV2D___BATCHNORM__FUSEDBATCHNORM___RELU_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: PeleeNet__stage_3_dense_block_6__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu_dram_pitch
            if (o_args->PeleeNet__stage_3_dense_block_6__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu_dram_pitch_enable)
                coproc_poke_half(o_args->PeleeNet__stage_3_dense_block_6__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu_dram_pitch_value, dagbin_base + 640U);

#endif

#ifdef PELEENET__STAGE_3_DENSE_BLOCK_6__CONV_LEFT_1__CONV2D___BATCHNORM__FUSEDBATCHNORM___RELU_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: PeleeNet__stage_3_dense_block_6__conv_left_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_dram_pitch
            if (o_args->PeleeNet__stage_3_dense_block_6__conv_left_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_dram_pitch_enable)
                coproc_poke_half(o_args->PeleeNet__stage_3_dense_block_6__conv_left_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_dram_pitch_value, dagbin_base + 772U);

#endif

            /*** dmald() calls ***/
            break;
        case 28:
            /*** poke() calls ***/
            /* Intermediate buffers */
            // Poke HMB intermediate input buffer: PeleeNet__stage_3_dense_block_6__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu
            coproc_poke_word_offset(dag_base + 0,
                dagbin_base, 28U);

            // Poke HMB intermediate input buffer: PeleeNet__stage_3_dense_block_6__conv_left_1__Conv2D___BatchNorm__FusedBatchNorm___Relu
            coproc_poke_word_offset(dag_base + 40960,
                dagbin_base, 68U);

            // Poke HMB intermediate input buffer: PeleeNet__stage_3_dense_block_5__concat______muli___484
            coproc_poke_word_offset(dag_base + 13107200,
                dagbin_base, 104U);

            // Poke HMB intermediate output buffer: PeleeNet__stage_3_dense_block_6__concat______muli___487
            coproc_poke_word_offset(dag_base + 81920,
                dagbin_base, 144U);

            /* Pokable fields */
#ifdef PELEENET__STAGE_3_DENSE_BLOCK_6__CONV_RIGHT_2__CONV2D___BATCHNORM__FUSEDBATCHNORM___RELU_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: PeleeNet__stage_3_dense_block_6__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu_dram_pitch
            if (o_args->PeleeNet__stage_3_dense_block_6__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu_dram_pitch_enable)
                coproc_poke_half_offset(o_args->PeleeNet__stage_3_dense_block_6__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu_dram_pitch_value, dagbin_base, 
                    32U);

#endif

#ifdef PELEENET__STAGE_3_DENSE_BLOCK_6__CONV_LEFT_1__CONV2D___BATCHNORM__FUSEDBATCHNORM___RELU_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: PeleeNet__stage_3_dense_block_6__conv_left_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_dram_pitch
            if (o_args->PeleeNet__stage_3_dense_block_6__conv_left_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_dram_pitch_enable)
                coproc_poke_half_offset(o_args->PeleeNet__stage_3_dense_block_6__conv_left_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_dram_pitch_value, dagbin_base, 
                    72U);

#endif

#ifdef PELEENET__STAGE_3_DENSE_BLOCK_5__CONCAT______MULI___484_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: PeleeNet__stage_3_dense_block_5__concat______muli___484_dram_pitch
            if (o_args->PeleeNet__stage_3_dense_block_5__concat______muli___484_dram_pitch_enable)
                coproc_poke_half_offset(o_args->PeleeNet__stage_3_dense_block_5__concat______muli___484_dram_pitch_value, dagbin_base, 
                    108U);

#endif

#ifdef PELEENET__STAGE_3_DENSE_BLOCK_6__CONCAT______MULI___487_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: PeleeNet__stage_3_dense_block_6__concat______muli___487_dram_pitch
            if (o_args->PeleeNet__stage_3_dense_block_6__concat______muli___487_dram_pitch_enable)
                coproc_poke_half_offset(o_args->PeleeNet__stage_3_dense_block_6__concat______muli___487_dram_pitch_value, dagbin_base, 
                    148U);

#endif

            /*** dmald() calls ***/
            break;
        case 29:
            /*** poke() calls ***/
            /* Intermediate buffers */
            // Poke HMB intermediate input buffer: PeleeNet__stage_3_dense_block_6__concat______muli___487
            coproc_poke_word_offset(dag_base + 81920,
                dagbin_base, 344U);

            // Poke HMB intermediate output buffer: PeleeNet__stage_3_dense_block_7__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu
            coproc_poke_word(dag_base + 0,
                dagbin_base + 576U);

            // Poke HMB intermediate output buffer: PeleeNet__stage_3_dense_block_7__conv_left_1__Conv2D___BatchNorm__FusedBatchNorm___Relu
            coproc_poke_word(dag_base + 40960,
                dagbin_base + 708U);

            /* Pokable fields */
#ifdef PELEENET__STAGE_3_DENSE_BLOCK_6__CONCAT______MULI___487_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: PeleeNet__stage_3_dense_block_6__concat______muli___487_dram_pitch
            if (o_args->PeleeNet__stage_3_dense_block_6__concat______muli___487_dram_pitch_enable)
                coproc_poke_half_offset(o_args->PeleeNet__stage_3_dense_block_6__concat______muli___487_dram_pitch_value, dagbin_base, 
                    348U);

#endif

#ifdef PELEENET__STAGE_3_DENSE_BLOCK_7__CONV_RIGHT_2__CONV2D___BATCHNORM__FUSEDBATCHNORM___RELU_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: PeleeNet__stage_3_dense_block_7__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu_dram_pitch
            if (o_args->PeleeNet__stage_3_dense_block_7__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu_dram_pitch_enable)
                coproc_poke_half(o_args->PeleeNet__stage_3_dense_block_7__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu_dram_pitch_value, dagbin_base + 580U);

#endif

#ifdef PELEENET__STAGE_3_DENSE_BLOCK_7__CONV_LEFT_1__CONV2D___BATCHNORM__FUSEDBATCHNORM___RELU_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: PeleeNet__stage_3_dense_block_7__conv_left_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_dram_pitch
            if (o_args->PeleeNet__stage_3_dense_block_7__conv_left_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_dram_pitch_enable)
                coproc_poke_half(o_args->PeleeNet__stage_3_dense_block_7__conv_left_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_dram_pitch_value, dagbin_base + 712U);

#endif

            /*** dmald() calls ***/
            break;
        case 30:
            /*** poke() calls ***/
            /* Intermediate buffers */
            // Poke HMB intermediate input buffer: PeleeNet__stage_3_dense_block_7__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu
            coproc_poke_word_offset(dag_base + 0,
                dagbin_base, 28U);

            // Poke HMB intermediate input buffer: PeleeNet__stage_3_dense_block_7__conv_left_1__Conv2D___BatchNorm__FusedBatchNorm___Relu
            coproc_poke_word_offset(dag_base + 40960,
                dagbin_base, 68U);

            // Poke HMB intermediate input buffer: PeleeNet__stage_3_dense_block_6__concat______muli___487
            coproc_poke_word_offset(dag_base + 81920,
                dagbin_base, 104U);

            // Poke HMB intermediate output buffer: PeleeNet__stage_3_dense_block_7__concat
            coproc_poke_word_offset(dag_base + 13107200,
                dagbin_base, 144U);

            /* Pokable fields */
#ifdef PELEENET__STAGE_3_DENSE_BLOCK_7__CONV_RIGHT_2__CONV2D___BATCHNORM__FUSEDBATCHNORM___RELU_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: PeleeNet__stage_3_dense_block_7__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu_dram_pitch
            if (o_args->PeleeNet__stage_3_dense_block_7__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu_dram_pitch_enable)
                coproc_poke_half_offset(o_args->PeleeNet__stage_3_dense_block_7__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu_dram_pitch_value, dagbin_base, 
                    32U);

#endif

#ifdef PELEENET__STAGE_3_DENSE_BLOCK_7__CONV_LEFT_1__CONV2D___BATCHNORM__FUSEDBATCHNORM___RELU_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: PeleeNet__stage_3_dense_block_7__conv_left_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_dram_pitch
            if (o_args->PeleeNet__stage_3_dense_block_7__conv_left_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_dram_pitch_enable)
                coproc_poke_half_offset(o_args->PeleeNet__stage_3_dense_block_7__conv_left_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_dram_pitch_value, dagbin_base, 
                    72U);

#endif

#ifdef PELEENET__STAGE_3_DENSE_BLOCK_6__CONCAT______MULI___487_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: PeleeNet__stage_3_dense_block_6__concat______muli___487_dram_pitch
            if (o_args->PeleeNet__stage_3_dense_block_6__concat______muli___487_dram_pitch_enable)
                coproc_poke_half_offset(o_args->PeleeNet__stage_3_dense_block_6__concat______muli___487_dram_pitch_value, dagbin_base, 
                    108U);

#endif

#ifdef PELEENET__STAGE_3_DENSE_BLOCK_7__CONCAT_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: PeleeNet__stage_3_dense_block_7__concat_dram_pitch
            if (o_args->PeleeNet__stage_3_dense_block_7__concat_dram_pitch_enable)
                coproc_poke_half_offset(o_args->PeleeNet__stage_3_dense_block_7__concat_dram_pitch_value, dagbin_base, 
                    148U);

#endif

            /*** dmald() calls ***/
            break;
        case 31:
            /*** poke() calls ***/
            /* Intermediate buffers */
            // Poke HMB intermediate input buffer: PeleeNet__stage_3_dense_block_7__concat
            coproc_poke_word_offset(dag_base + 13107200,
                dagbin_base, 104U);

            // Poke HMB intermediate output buffer: PeleeNet__stage_3_transition_layer__transition_layer_conv0__Conv2D___BatchNorm__FusedBatchNorm___Relu
            coproc_poke_word_offset(dag_base + 0,
                dagbin_base, 156U);

            /* Pokable fields */
#ifdef PELEENET__STAGE_3_DENSE_BLOCK_7__CONCAT_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: PeleeNet__stage_3_dense_block_7__concat_dram_pitch
            if (o_args->PeleeNet__stage_3_dense_block_7__concat_dram_pitch_enable)
                coproc_poke_half_offset(o_args->PeleeNet__stage_3_dense_block_7__concat_dram_pitch_value, dagbin_base, 
                    108U);

#endif

#ifdef PELEENET__STAGE_3_TRANSITION_LAYER__TRANSITION_LAYER_CONV0__CONV2D___BATCHNORM__FUSEDBATCHNORM___RELU_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: PeleeNet__stage_3_transition_layer__transition_layer_conv0__Conv2D___BatchNorm__FusedBatchNorm___Relu_dram_pitch
            if (o_args->PeleeNet__stage_3_transition_layer__transition_layer_conv0__Conv2D___BatchNorm__FusedBatchNorm___Relu_dram_pitch_enable)
                coproc_poke_half_offset(o_args->PeleeNet__stage_3_transition_layer__transition_layer_conv0__Conv2D___BatchNorm__FusedBatchNorm___Relu_dram_pitch_value, dagbin_base, 
                    160U);

#endif

            /*** dmald() calls ***/
            break;
        case 32:
            /*** poke() calls ***/
            /* Intermediate buffers */
            // Poke HMB intermediate input buffer: PeleeNet__stage_3_transition_layer__transition_layer_conv0__Conv2D___BatchNorm__FusedBatchNorm___Relu
            coproc_poke_word_offset(dag_base + 0,
                dagbin_base, 28U);

            // Poke HMB intermediate output buffer: PeleeNet__stage_3_transition_layer__transition_layer_avgpool__AvgPool______muli___490___Mean_1_src0_nhwc___nhwc___transpose___123
            coproc_poke_word(dag_base + 6946816,
                dagbin_base + 956U);

            /* Pokable fields */
#ifdef PELEENET__STAGE_3_TRANSITION_LAYER__TRANSITION_LAYER_CONV0__CONV2D___BATCHNORM__FUSEDBATCHNORM___RELU_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: PeleeNet__stage_3_transition_layer__transition_layer_conv0__Conv2D___BatchNorm__FusedBatchNorm___Relu_dram_pitch
            if (o_args->PeleeNet__stage_3_transition_layer__transition_layer_conv0__Conv2D___BatchNorm__FusedBatchNorm___Relu_dram_pitch_enable)
                coproc_poke_half_offset(o_args->PeleeNet__stage_3_transition_layer__transition_layer_conv0__Conv2D___BatchNorm__FusedBatchNorm___Relu_dram_pitch_value, dagbin_base, 
                    32U);

#endif

#ifdef PELEENET__STAGE_3_TRANSITION_LAYER__TRANSITION_LAYER_AVGPOOL__AVGPOOL______MULI___490___MEAN_1_SRC0_NHWC___NHWC___TRANSPOSE___123_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: PeleeNet__stage_3_transition_layer__transition_layer_avgpool__AvgPool______muli___490___Mean_1_src0_nhwc___nhwc___transpose___123_dram_pitch
            if (o_args->PeleeNet__stage_3_transition_layer__transition_layer_avgpool__AvgPool______muli___490___Mean_1_src0_nhwc___nhwc___transpose___123_dram_pitch_enable)
                coproc_poke_half(o_args->PeleeNet__stage_3_transition_layer__transition_layer_avgpool__AvgPool______muli___490___Mean_1_src0_nhwc___nhwc___transpose___123_dram_pitch_value, dagbin_base + 960U);

#endif

            /*** dmald() calls ***/
            break;
        case 33:
            /*** poke() calls ***/
            /* Intermediate buffers */
            // Poke HMB intermediate input buffer: PeleeNet__stage_3_transition_layer__transition_layer_avgpool__AvgPool______muli___490___Mean_1_src0_nhwc___nhwc___transpose___123
            coproc_poke_word_offset(dag_base + 6946816,
                dagbin_base, 244U);

            // Poke HMB intermediate output buffer: Sigmoid_1
            coproc_poke_word(dag_base + 0,
                dagbin_base + 792U);

            /* Pokable fields */
#ifdef PELEENET__STAGE_3_TRANSITION_LAYER__TRANSITION_LAYER_AVGPOOL__AVGPOOL______MULI___490___MEAN_1_SRC0_NHWC___NHWC___TRANSPOSE___123_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: PeleeNet__stage_3_transition_layer__transition_layer_avgpool__AvgPool______muli___490___Mean_1_src0_nhwc___nhwc___transpose___123_dram_pitch
            if (o_args->PeleeNet__stage_3_transition_layer__transition_layer_avgpool__AvgPool______muli___490___Mean_1_src0_nhwc___nhwc___transpose___123_dram_pitch_enable)
                coproc_poke_half_offset(o_args->PeleeNet__stage_3_transition_layer__transition_layer_avgpool__AvgPool______muli___490___Mean_1_src0_nhwc___nhwc___transpose___123_dram_pitch_value, dagbin_base, 
                    248U);

#endif

#ifdef SIGMOID_1_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: Sigmoid_1_dram_pitch
            if (o_args->Sigmoid_1_dram_pitch_enable)
                coproc_poke_half(o_args->Sigmoid_1_dram_pitch_value, dagbin_base + 796U);

#endif

            /*** dmald() calls ***/
            break;
        case 34:
            /*** poke() calls ***/
            /* Intermediate buffers */
            // Poke HMB intermediate input buffer: Sigmoid_1
            coproc_poke_word_offset(dag_base + 0,
                dagbin_base, 28U);

            // Poke HMB intermediate input buffer: PeleeNet__stage_3_transition_layer__transition_layer_avgpool__AvgPool______muli___490___Mean_1_src0_nhwc___nhwc___transpose___123
            coproc_poke_word_offset(dag_base + 6946816,
                dagbin_base, 64U);

            // Poke HMB intermediate output buffer: Mul_1______muli___491___ean_2_src0_nhwc___nhwc___transpose___130
            coproc_poke_word_offset(dag_base + 4096,
                dagbin_base, 112U);

            /* Pokable fields */
#ifdef SIGMOID_1_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: Sigmoid_1_dram_pitch
            if (o_args->Sigmoid_1_dram_pitch_enable)
                coproc_poke_half_offset(o_args->Sigmoid_1_dram_pitch_value, dagbin_base, 
                    32U);

#endif

#ifdef PELEENET__STAGE_3_TRANSITION_LAYER__TRANSITION_LAYER_AVGPOOL__AVGPOOL______MULI___490___MEAN_1_SRC0_NHWC___NHWC___TRANSPOSE___123_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: PeleeNet__stage_3_transition_layer__transition_layer_avgpool__AvgPool______muli___490___Mean_1_src0_nhwc___nhwc___transpose___123_dram_pitch
            if (o_args->PeleeNet__stage_3_transition_layer__transition_layer_avgpool__AvgPool______muli___490___Mean_1_src0_nhwc___nhwc___transpose___123_dram_pitch_enable)
                coproc_poke_half_offset(o_args->PeleeNet__stage_3_transition_layer__transition_layer_avgpool__AvgPool______muli___490___Mean_1_src0_nhwc___nhwc___transpose___123_dram_pitch_value, dagbin_base, 
                    68U);

#endif

#ifdef MUL_1______MULI___491___EAN_2_SRC0_NHWC___NHWC___TRANSPOSE___130_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: Mul_1______muli___491___ean_2_src0_nhwc___nhwc___transpose___130_dram_pitch
            if (o_args->Mul_1______muli___491___ean_2_src0_nhwc___nhwc___transpose___130_dram_pitch_enable)
                coproc_poke_half_offset(o_args->Mul_1______muli___491___ean_2_src0_nhwc___nhwc___transpose___130_dram_pitch_value, dagbin_base, 
                    116U);

#endif

            /*** dmald() calls ***/
            break;
        case 35:
            /*** poke() calls ***/
            /* Intermediate buffers */
            // Poke HMB intermediate input buffer: Mul_1______muli___491___ean_2_src0_nhwc___nhwc___transpose___130
            coproc_poke_word_offset(dag_base + 4096,
                dagbin_base, 28U);

            // Poke HMB intermediate output buffer: Mul_2
            coproc_poke_word_offset(dag_base + 528384,
                dagbin_base, 132U);

            /* Pokable fields */
#ifdef MUL_1______MULI___491___EAN_2_SRC0_NHWC___NHWC___TRANSPOSE___130_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: Mul_1______muli___491___ean_2_src0_nhwc___nhwc___transpose___130_dram_pitch
            if (o_args->Mul_1______muli___491___ean_2_src0_nhwc___nhwc___transpose___130_dram_pitch_enable)
                coproc_poke_half_offset(o_args->Mul_1______muli___491___ean_2_src0_nhwc___nhwc___transpose___130_dram_pitch_value, dagbin_base, 
                    32U);

#endif

#ifdef MUL_2_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: Mul_2_dram_pitch
            if (o_args->Mul_2_dram_pitch_enable)
                coproc_poke_half_offset(o_args->Mul_2_dram_pitch_value, dagbin_base, 
                    136U);

#endif

            /*** dmald() calls ***/
            break;
        case 36:
            /*** poke() calls ***/
            /* Intermediate buffers */
            // Poke HMB intermediate input buffer: Mul_2
            coproc_poke_word_offset(dag_base + 528384,
                dagbin_base, 28U);

            // Poke HMB intermediate output buffer: ResizeBilinear_1
            coproc_poke_word_offset(dag_base + 13107200,
                dagbin_base, 112U);

            /* Pokable fields */
#ifdef MUL_2_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: Mul_2_dram_pitch
            if (o_args->Mul_2_dram_pitch_enable)
                coproc_poke_half_offset(o_args->Mul_2_dram_pitch_value, dagbin_base, 
                    32U);

#endif

#ifdef RESIZEBILINEAR_1_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: ResizeBilinear_1_dram_pitch
            if (o_args->ResizeBilinear_1_dram_pitch_enable)
                coproc_poke_half_offset(o_args->ResizeBilinear_1_dram_pitch_value, dagbin_base, 
                    116U);

#endif

            /*** dmald() calls ***/
            break;
        case 37:
            /*** poke() calls ***/
            /* Intermediate buffers */
            // Poke HMB intermediate input buffer: ResizeBilinear
            coproc_poke_word_offset(dag_base + 1703936,
                dagbin_base, 28U);

            // Poke HMB intermediate input buffer: ResizeBilinear_1
            coproc_poke_word_offset(dag_base + 13107200,
                dagbin_base, 64U);

            // Poke HMB intermediate output buffer: concat
            coproc_poke_word_offset(dag_base + 23592960,
                dagbin_base, 112U);

            /* Pokable fields */
#ifdef RESIZEBILINEAR_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: ResizeBilinear_dram_pitch
            if (o_args->ResizeBilinear_dram_pitch_enable)
                coproc_poke_half_offset(o_args->ResizeBilinear_dram_pitch_value, dagbin_base, 
                    32U);

#endif

#ifdef RESIZEBILINEAR_1_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: ResizeBilinear_1_dram_pitch
            if (o_args->ResizeBilinear_1_dram_pitch_enable)
                coproc_poke_half_offset(o_args->ResizeBilinear_1_dram_pitch_value, dagbin_base, 
                    68U);

#endif

#ifdef CONCAT_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: concat_dram_pitch
            if (o_args->concat_dram_pitch_enable)
                coproc_poke_half_offset(o_args->concat_dram_pitch_value, dagbin_base, 
                    116U);

#endif

            /*** dmald() calls ***/
            break;
        case 38:
            /*** poke() calls ***/
            /* Intermediate buffers */
            // Poke HMB intermediate input buffer: Conv_2__Conv2D___BiasAdd___BatchNorm_2__FusedBatchNorm___muli___13___BatchNorm_2__FusedBatchNorm___Relu_2
            coproc_poke_word_offset(dag_base + 7864320,
                dagbin_base, 28U);

            // Poke HMB intermediate input buffer: concat
            coproc_poke_word_offset(dag_base + 23592960,
                dagbin_base, 64U);

            // Poke HMB intermediate output buffer: concat_1
            coproc_poke_word_offset(dag_base + 39321600,
                dagbin_base, 112U);

            /* Pokable fields */
#ifdef CONV_2__CONV2D___BIASADD___BATCHNORM_2__FUSEDBATCHNORM___MULI___13___BATCHNORM_2__FUSEDBATCHNORM___RELU_2_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: Conv_2__Conv2D___BiasAdd___BatchNorm_2__FusedBatchNorm___muli___13___BatchNorm_2__FusedBatchNorm___Relu_2_dram_pitch
            if (o_args->Conv_2__Conv2D___BiasAdd___BatchNorm_2__FusedBatchNorm___muli___13___BatchNorm_2__FusedBatchNorm___Relu_2_dram_pitch_enable)
                coproc_poke_half_offset(o_args->Conv_2__Conv2D___BiasAdd___BatchNorm_2__FusedBatchNorm___muli___13___BatchNorm_2__FusedBatchNorm___Relu_2_dram_pitch_value, dagbin_base, 
                    32U);

#endif

#ifdef CONCAT_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: concat_dram_pitch
            if (o_args->concat_dram_pitch_enable)
                coproc_poke_half_offset(o_args->concat_dram_pitch_value, dagbin_base, 
                    68U);

#endif

#ifdef CONCAT_1_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: concat_1_dram_pitch
            if (o_args->concat_1_dram_pitch_enable)
                coproc_poke_half_offset(o_args->concat_1_dram_pitch_value, dagbin_base, 
                    116U);

#endif

            /*** dmald() calls ***/
            break;
        case 39:
            /*** poke() calls ***/
            /* Intermediate buffers */
            // Poke HMB intermediate input buffer: concat_1
            coproc_poke_word_offset(dag_base + 39321600,
                dagbin_base, 28U);

            // Poke HMB intermediate output buffer: concat_1_580000000_
            coproc_poke_word_offset(dag_base + 0,
                dagbin_base, 108U);

            /* Pokable fields */
#ifdef CONCAT_1_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: concat_1_dram_pitch
            if (o_args->concat_1_dram_pitch_enable)
                coproc_poke_half_offset(o_args->concat_1_dram_pitch_value, dagbin_base, 
                    32U);

#endif

#ifdef CONCAT_1_580000000__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: concat_1_580000000__dram_pitch
            if (o_args->concat_1_580000000__dram_pitch_enable)
                coproc_poke_half_offset(o_args->concat_1_580000000__dram_pitch_value, dagbin_base, 
                    112U);

#endif

            /*** dmald() calls ***/
            break;
        case 40:
            /*** poke() calls ***/
            /* Intermediate buffers */
            // Poke HMB intermediate input buffer: concat_1_580000000_
            coproc_poke_word_offset(dag_base + 0,
                dagbin_base, 28U);

            // Poke HMB intermediate input buffer: concat_1
            coproc_poke_word_offset(dag_base + 39321600,
                dagbin_base, 64U);

            // Poke HMB intermediate output buffer: Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___136___BatchNorm_5__FusedBatchNorm___Relu_3___muli___492___Mean_3_src0_nhwc___nhwc___transpose___138_580000004_
            coproc_poke_word_offset(dag_base + 3145728,
                dagbin_base, 200U);

            // Poke HMB intermediate output buffer: concat_1_580000105_
            coproc_poke_word_offset(dag_base + 3150848,
                dagbin_base, 252U);

            /* Pokable fields */
#ifdef CONCAT_1_580000000__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: concat_1_580000000__dram_pitch
            if (o_args->concat_1_580000000__dram_pitch_enable)
                coproc_poke_half_offset(o_args->concat_1_580000000__dram_pitch_value, dagbin_base, 
                    32U);

#endif

#ifdef CONCAT_1_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: concat_1_dram_pitch
            if (o_args->concat_1_dram_pitch_enable)
                coproc_poke_half_offset(o_args->concat_1_dram_pitch_value, dagbin_base, 
                    68U);

#endif

#ifdef CONV_5__CONV2D___BIASADD___BATCHNORM_5__FUSEDBATCHNORM___MULI___136___BATCHNORM_5__FUSEDBATCHNORM___RELU_3___MULI___492___MEAN_3_SRC0_NHWC___NHWC___TRANSPOSE___138_580000004__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___136___BatchNorm_5__FusedBatchNorm___Relu_3___muli___492___Mean_3_src0_nhwc___nhwc___transpose___138_580000004__dram_pitch
            if (o_args->Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___136___BatchNorm_5__FusedBatchNorm___Relu_3___muli___492___Mean_3_src0_nhwc___nhwc___transpose___138_580000004__dram_pitch_enable)
                coproc_poke_half_offset(o_args->Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___136___BatchNorm_5__FusedBatchNorm___Relu_3___muli___492___Mean_3_src0_nhwc___nhwc___transpose___138_580000004__dram_pitch_value, dagbin_base, 
                    204U);

#endif

#ifdef CONCAT_1_580000105__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: concat_1_580000105__dram_pitch
            if (o_args->concat_1_580000105__dram_pitch_enable)
                coproc_poke_half_offset(o_args->concat_1_580000105__dram_pitch_value, dagbin_base, 
                    256U);

#endif

            /*** dmald() calls ***/
            break;
        case 41:
            /*** poke() calls ***/
            /* Intermediate buffers */
            // Poke HMB intermediate input buffer: concat_1_580000105_
            coproc_poke_word_offset(dag_base + 3150848,
                dagbin_base, 28U);

            // Poke HMB intermediate input buffer: concat_1
            coproc_poke_word_offset(dag_base + 39321600,
                dagbin_base, 64U);

            // Poke HMB intermediate output buffer: concat_1_580000109_
            coproc_poke_word_offset(dag_base + 0,
                dagbin_base, 120U);

            // Poke HMB intermediate output buffer: concat_1_580000114_
            coproc_poke_word_offset(dag_base + 1050624,
                dagbin_base, 212U);

            // Poke HMB intermediate output buffer: concat_1_580000115_
            coproc_poke_word_offset(dag_base + 10155008,
                dagbin_base, 264U);

            /* Pokable fields */
#ifdef CONCAT_1_580000105__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: concat_1_580000105__dram_pitch
            if (o_args->concat_1_580000105__dram_pitch_enable)
                coproc_poke_half_offset(o_args->concat_1_580000105__dram_pitch_value, dagbin_base, 
                    32U);

#endif

#ifdef CONCAT_1_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: concat_1_dram_pitch
            if (o_args->concat_1_dram_pitch_enable)
                coproc_poke_half_offset(o_args->concat_1_dram_pitch_value, dagbin_base, 
                    68U);

#endif

#ifdef CONCAT_1_580000109__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: concat_1_580000109__dram_pitch
            if (o_args->concat_1_580000109__dram_pitch_enable)
                coproc_poke_half_offset(o_args->concat_1_580000109__dram_pitch_value, dagbin_base, 
                    124U);

#endif

#ifdef CONCAT_1_580000114__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: concat_1_580000114__dram_pitch
            if (o_args->concat_1_580000114__dram_pitch_enable)
                coproc_poke_half_offset(o_args->concat_1_580000114__dram_pitch_value, dagbin_base, 
                    216U);

#endif

#ifdef CONCAT_1_580000115__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: concat_1_580000115__dram_pitch
            if (o_args->concat_1_580000115__dram_pitch_enable)
                coproc_poke_half_offset(o_args->concat_1_580000115__dram_pitch_value, dagbin_base, 
                    268U);

#endif

            /*** dmald() calls ***/
            break;
        case 42:
            /*** poke() calls ***/
            /* Intermediate buffers */
            // Poke HMB intermediate input buffer: concat_1_580000109_
            coproc_poke_word_offset(dag_base + 0,
                dagbin_base, 28U);

            // Poke HMB intermediate input buffer: concat_1_580000114_
            coproc_poke_word_offset(dag_base + 1050624,
                dagbin_base, 68U);

            // Poke HMB intermediate input buffer: concat_1_580000115_
            coproc_poke_word_offset(dag_base + 10155008,
                dagbin_base, 108U);

            // Poke HMB intermediate output buffer: Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___136___BatchNorm_5__FusedBatchNorm___Relu_3___muli___492___Mean_3_src0_nhwc___nhwc___transpose___138_580000009_
            coproc_poke_word_offset(dag_base + 2101248,
                dagbin_base, 344U);

            /* Pokable fields */
#ifdef CONCAT_1_580000109__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: concat_1_580000109__dram_pitch
            if (o_args->concat_1_580000109__dram_pitch_enable)
                coproc_poke_half_offset(o_args->concat_1_580000109__dram_pitch_value, dagbin_base, 
                    32U);

#endif

#ifdef CONCAT_1_580000114__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: concat_1_580000114__dram_pitch
            if (o_args->concat_1_580000114__dram_pitch_enable)
                coproc_poke_half_offset(o_args->concat_1_580000114__dram_pitch_value, dagbin_base, 
                    72U);

#endif

#ifdef CONCAT_1_580000115__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: concat_1_580000115__dram_pitch
            if (o_args->concat_1_580000115__dram_pitch_enable)
                coproc_poke_half_offset(o_args->concat_1_580000115__dram_pitch_value, dagbin_base, 
                    112U);

#endif

#ifdef CONV_5__CONV2D___BIASADD___BATCHNORM_5__FUSEDBATCHNORM___MULI___136___BATCHNORM_5__FUSEDBATCHNORM___RELU_3___MULI___492___MEAN_3_SRC0_NHWC___NHWC___TRANSPOSE___138_580000009__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___136___BatchNorm_5__FusedBatchNorm___Relu_3___muli___492___Mean_3_src0_nhwc___nhwc___transpose___138_580000009__dram_pitch
            if (o_args->Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___136___BatchNorm_5__FusedBatchNorm___Relu_3___muli___492___Mean_3_src0_nhwc___nhwc___transpose___138_580000009__dram_pitch_enable)
                coproc_poke_half_offset(o_args->Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___136___BatchNorm_5__FusedBatchNorm___Relu_3___muli___492___Mean_3_src0_nhwc___nhwc___transpose___138_580000009__dram_pitch_value, dagbin_base, 
                    348U);

#endif

            /*** dmald() calls ***/
            break;
        case 43:
            /*** poke() calls ***/
            /* Intermediate buffers */
            // Poke HMB intermediate input buffer: concat_1
            coproc_poke_word_offset(dag_base + 39321600,
                dagbin_base, 28U);

            // Poke HMB intermediate output buffer: concat_1_580000127_
            coproc_poke_word_offset(dag_base + 0,
                dagbin_base, 124U);

            // Poke HMB intermediate output buffer: concat_1_580000128_
            coproc_poke_word_offset(dag_base + 3150848,
                dagbin_base, 176U);

            /* Pokable fields */
#ifdef CONCAT_1_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: concat_1_dram_pitch
            if (o_args->concat_1_dram_pitch_enable)
                coproc_poke_half_offset(o_args->concat_1_dram_pitch_value, dagbin_base, 
                    32U);

#endif

#ifdef CONCAT_1_580000127__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: concat_1_580000127__dram_pitch
            if (o_args->concat_1_580000127__dram_pitch_enable)
                coproc_poke_half_offset(o_args->concat_1_580000127__dram_pitch_value, dagbin_base, 
                    128U);

#endif

#ifdef CONCAT_1_580000128__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: concat_1_580000128__dram_pitch
            if (o_args->concat_1_580000128__dram_pitch_enable)
                coproc_poke_half_offset(o_args->concat_1_580000128__dram_pitch_value, dagbin_base, 
                    180U);

#endif

            /*** dmald() calls ***/
            break;
        case 44:
            /*** poke() calls ***/
            /* Intermediate buffers */
            // Poke HMB intermediate input buffer: concat_1_580000128_
            coproc_poke_word_offset(dag_base + 3150848,
                dagbin_base, 28U);

            // Poke HMB intermediate input buffer: concat_1
            coproc_poke_word_offset(dag_base + 39321600,
                dagbin_base, 64U);

            // Poke HMB intermediate output buffer: concat_1_580000132_
            coproc_poke_word_offset(dag_base + 1050624,
                dagbin_base, 120U);

            // Poke HMB intermediate output buffer: concat_1_580000133_
            coproc_poke_word_offset(dag_base + 10155008,
                dagbin_base, 172U);

            /* Pokable fields */
#ifdef CONCAT_1_580000128__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: concat_1_580000128__dram_pitch
            if (o_args->concat_1_580000128__dram_pitch_enable)
                coproc_poke_half_offset(o_args->concat_1_580000128__dram_pitch_value, dagbin_base, 
                    32U);

#endif

#ifdef CONCAT_1_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: concat_1_dram_pitch
            if (o_args->concat_1_dram_pitch_enable)
                coproc_poke_half_offset(o_args->concat_1_dram_pitch_value, dagbin_base, 
                    68U);

#endif

#ifdef CONCAT_1_580000132__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: concat_1_580000132__dram_pitch
            if (o_args->concat_1_580000132__dram_pitch_enable)
                coproc_poke_half_offset(o_args->concat_1_580000132__dram_pitch_value, dagbin_base, 
                    124U);

#endif

#ifdef CONCAT_1_580000133__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: concat_1_580000133__dram_pitch
            if (o_args->concat_1_580000133__dram_pitch_enable)
                coproc_poke_half_offset(o_args->concat_1_580000133__dram_pitch_value, dagbin_base, 
                    176U);

#endif

            /*** dmald() calls ***/
            break;
        case 45:
            /*** poke() calls ***/
            /* Intermediate buffers */
            // Poke HMB intermediate input buffer: concat_1_580000127_
            coproc_poke_word_offset(dag_base + 0,
                dagbin_base, 104U);

            // Poke HMB intermediate input buffer: concat_1_580000132_
            coproc_poke_word_offset(dag_base + 1050624,
                dagbin_base, 28U);

            // Poke HMB intermediate input buffer: concat_1_580000133_
            coproc_poke_word_offset(dag_base + 10155008,
                dagbin_base, 68U);

            // Poke HMB intermediate output buffer: Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___136___BatchNorm_5__FusedBatchNorm___Relu_3___muli___492___Mean_3_src0_nhwc___nhwc___transpose___138_580000014_
            coproc_poke_word_offset(dag_base + 2106368,
                dagbin_base, 344U);

            /* Pokable fields */
#ifdef CONCAT_1_580000132__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: concat_1_580000132__dram_pitch
            if (o_args->concat_1_580000132__dram_pitch_enable)
                coproc_poke_half_offset(o_args->concat_1_580000132__dram_pitch_value, dagbin_base, 
                    32U);

#endif

#ifdef CONCAT_1_580000133__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: concat_1_580000133__dram_pitch
            if (o_args->concat_1_580000133__dram_pitch_enable)
                coproc_poke_half_offset(o_args->concat_1_580000133__dram_pitch_value, dagbin_base, 
                    72U);

#endif

#ifdef CONCAT_1_580000127__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: concat_1_580000127__dram_pitch
            if (o_args->concat_1_580000127__dram_pitch_enable)
                coproc_poke_half_offset(o_args->concat_1_580000127__dram_pitch_value, dagbin_base, 
                    108U);

#endif

#ifdef CONV_5__CONV2D___BIASADD___BATCHNORM_5__FUSEDBATCHNORM___MULI___136___BATCHNORM_5__FUSEDBATCHNORM___RELU_3___MULI___492___MEAN_3_SRC0_NHWC___NHWC___TRANSPOSE___138_580000014__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___136___BatchNorm_5__FusedBatchNorm___Relu_3___muli___492___Mean_3_src0_nhwc___nhwc___transpose___138_580000014__dram_pitch
            if (o_args->Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___136___BatchNorm_5__FusedBatchNorm___Relu_3___muli___492___Mean_3_src0_nhwc___nhwc___transpose___138_580000014__dram_pitch_enable)
                coproc_poke_half_offset(o_args->Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___136___BatchNorm_5__FusedBatchNorm___Relu_3___muli___492___Mean_3_src0_nhwc___nhwc___transpose___138_580000014__dram_pitch_value, dagbin_base, 
                    348U);

#endif

            /*** dmald() calls ***/
            break;
        case 46:
            /*** poke() calls ***/
            /* Intermediate buffers */
            // Poke HMB intermediate input buffer: concat_1
            coproc_poke_word_offset(dag_base + 39321600,
                dagbin_base, 28U);

            // Poke HMB intermediate output buffer: concat_1_580000145_
            coproc_poke_word_offset(dag_base + 0,
                dagbin_base, 124U);

            // Poke HMB intermediate output buffer: concat_1_580000146_
            coproc_poke_word_offset(dag_base + 3150848,
                dagbin_base, 176U);

            /* Pokable fields */
#ifdef CONCAT_1_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: concat_1_dram_pitch
            if (o_args->concat_1_dram_pitch_enable)
                coproc_poke_half_offset(o_args->concat_1_dram_pitch_value, dagbin_base, 
                    32U);

#endif

#ifdef CONCAT_1_580000145__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: concat_1_580000145__dram_pitch
            if (o_args->concat_1_580000145__dram_pitch_enable)
                coproc_poke_half_offset(o_args->concat_1_580000145__dram_pitch_value, dagbin_base, 
                    128U);

#endif

#ifdef CONCAT_1_580000146__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: concat_1_580000146__dram_pitch
            if (o_args->concat_1_580000146__dram_pitch_enable)
                coproc_poke_half_offset(o_args->concat_1_580000146__dram_pitch_value, dagbin_base, 
                    180U);

#endif

            /*** dmald() calls ***/
            break;
        case 47:
            /*** poke() calls ***/
            /* Intermediate buffers */
            // Poke HMB intermediate input buffer: concat_1_580000146_
            coproc_poke_word_offset(dag_base + 3150848,
                dagbin_base, 28U);

            // Poke HMB intermediate input buffer: concat_1
            coproc_poke_word_offset(dag_base + 39321600,
                dagbin_base, 64U);

            // Poke HMB intermediate output buffer: concat_1_580000150_
            coproc_poke_word_offset(dag_base + 1050624,
                dagbin_base, 120U);

            // Poke HMB intermediate output buffer: concat_1_580000151_
            coproc_poke_word_offset(dag_base + 10155008,
                dagbin_base, 172U);

            /* Pokable fields */
#ifdef CONCAT_1_580000146__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: concat_1_580000146__dram_pitch
            if (o_args->concat_1_580000146__dram_pitch_enable)
                coproc_poke_half_offset(o_args->concat_1_580000146__dram_pitch_value, dagbin_base, 
                    32U);

#endif

#ifdef CONCAT_1_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: concat_1_dram_pitch
            if (o_args->concat_1_dram_pitch_enable)
                coproc_poke_half_offset(o_args->concat_1_dram_pitch_value, dagbin_base, 
                    68U);

#endif

#ifdef CONCAT_1_580000150__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: concat_1_580000150__dram_pitch
            if (o_args->concat_1_580000150__dram_pitch_enable)
                coproc_poke_half_offset(o_args->concat_1_580000150__dram_pitch_value, dagbin_base, 
                    124U);

#endif

#ifdef CONCAT_1_580000151__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: concat_1_580000151__dram_pitch
            if (o_args->concat_1_580000151__dram_pitch_enable)
                coproc_poke_half_offset(o_args->concat_1_580000151__dram_pitch_value, dagbin_base, 
                    176U);

#endif

            /*** dmald() calls ***/
            break;
        case 48:
            /*** poke() calls ***/
            /* Intermediate buffers */
            // Poke HMB intermediate input buffer: concat_1_580000145_
            coproc_poke_word_offset(dag_base + 0,
                dagbin_base, 104U);

            // Poke HMB intermediate input buffer: concat_1_580000150_
            coproc_poke_word_offset(dag_base + 1050624,
                dagbin_base, 28U);

            // Poke HMB intermediate input buffer: concat_1_580000151_
            coproc_poke_word_offset(dag_base + 10155008,
                dagbin_base, 68U);

            // Poke HMB intermediate output buffer: Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___136___BatchNorm_5__FusedBatchNorm___Relu_3___muli___492___Mean_3_src0_nhwc___nhwc___transpose___138_580000019_
            coproc_poke_word_offset(dag_base + 2111488,
                dagbin_base, 344U);

            /* Pokable fields */
#ifdef CONCAT_1_580000150__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: concat_1_580000150__dram_pitch
            if (o_args->concat_1_580000150__dram_pitch_enable)
                coproc_poke_half_offset(o_args->concat_1_580000150__dram_pitch_value, dagbin_base, 
                    32U);

#endif

#ifdef CONCAT_1_580000151__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: concat_1_580000151__dram_pitch
            if (o_args->concat_1_580000151__dram_pitch_enable)
                coproc_poke_half_offset(o_args->concat_1_580000151__dram_pitch_value, dagbin_base, 
                    72U);

#endif

#ifdef CONCAT_1_580000145__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: concat_1_580000145__dram_pitch
            if (o_args->concat_1_580000145__dram_pitch_enable)
                coproc_poke_half_offset(o_args->concat_1_580000145__dram_pitch_value, dagbin_base, 
                    108U);

#endif

#ifdef CONV_5__CONV2D___BIASADD___BATCHNORM_5__FUSEDBATCHNORM___MULI___136___BATCHNORM_5__FUSEDBATCHNORM___RELU_3___MULI___492___MEAN_3_SRC0_NHWC___NHWC___TRANSPOSE___138_580000019__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___136___BatchNorm_5__FusedBatchNorm___Relu_3___muli___492___Mean_3_src0_nhwc___nhwc___transpose___138_580000019__dram_pitch
            if (o_args->Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___136___BatchNorm_5__FusedBatchNorm___Relu_3___muli___492___Mean_3_src0_nhwc___nhwc___transpose___138_580000019__dram_pitch_enable)
                coproc_poke_half_offset(o_args->Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___136___BatchNorm_5__FusedBatchNorm___Relu_3___muli___492___Mean_3_src0_nhwc___nhwc___transpose___138_580000019__dram_pitch_value, dagbin_base, 
                    348U);

#endif

            /*** dmald() calls ***/
            break;
        case 49:
            /*** poke() calls ***/
            /* Intermediate buffers */
            // Poke HMB intermediate input buffer: concat_1
            coproc_poke_word_offset(dag_base + 39321600,
                dagbin_base, 28U);

            // Poke HMB intermediate output buffer: concat_1_580000163_
            coproc_poke_word_offset(dag_base + 0,
                dagbin_base, 124U);

            // Poke HMB intermediate output buffer: concat_1_580000164_
            coproc_poke_word_offset(dag_base + 3150848,
                dagbin_base, 176U);

            /* Pokable fields */
#ifdef CONCAT_1_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: concat_1_dram_pitch
            if (o_args->concat_1_dram_pitch_enable)
                coproc_poke_half_offset(o_args->concat_1_dram_pitch_value, dagbin_base, 
                    32U);

#endif

#ifdef CONCAT_1_580000163__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: concat_1_580000163__dram_pitch
            if (o_args->concat_1_580000163__dram_pitch_enable)
                coproc_poke_half_offset(o_args->concat_1_580000163__dram_pitch_value, dagbin_base, 
                    128U);

#endif

#ifdef CONCAT_1_580000164__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: concat_1_580000164__dram_pitch
            if (o_args->concat_1_580000164__dram_pitch_enable)
                coproc_poke_half_offset(o_args->concat_1_580000164__dram_pitch_value, dagbin_base, 
                    180U);

#endif

            /*** dmald() calls ***/
            break;
        case 50:
            /*** poke() calls ***/
            /* Intermediate buffers */
            // Poke HMB intermediate input buffer: concat_1_580000164_
            coproc_poke_word_offset(dag_base + 3150848,
                dagbin_base, 28U);

            // Poke HMB intermediate input buffer: concat_1
            coproc_poke_word_offset(dag_base + 39321600,
                dagbin_base, 64U);

            // Poke HMB intermediate output buffer: concat_1_580000168_
            coproc_poke_word_offset(dag_base + 1050624,
                dagbin_base, 120U);

            // Poke HMB intermediate output buffer: concat_1_580000169_
            coproc_poke_word_offset(dag_base + 10155008,
                dagbin_base, 172U);

            /* Pokable fields */
#ifdef CONCAT_1_580000164__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: concat_1_580000164__dram_pitch
            if (o_args->concat_1_580000164__dram_pitch_enable)
                coproc_poke_half_offset(o_args->concat_1_580000164__dram_pitch_value, dagbin_base, 
                    32U);

#endif

#ifdef CONCAT_1_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: concat_1_dram_pitch
            if (o_args->concat_1_dram_pitch_enable)
                coproc_poke_half_offset(o_args->concat_1_dram_pitch_value, dagbin_base, 
                    68U);

#endif

#ifdef CONCAT_1_580000168__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: concat_1_580000168__dram_pitch
            if (o_args->concat_1_580000168__dram_pitch_enable)
                coproc_poke_half_offset(o_args->concat_1_580000168__dram_pitch_value, dagbin_base, 
                    124U);

#endif

#ifdef CONCAT_1_580000169__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: concat_1_580000169__dram_pitch
            if (o_args->concat_1_580000169__dram_pitch_enable)
                coproc_poke_half_offset(o_args->concat_1_580000169__dram_pitch_value, dagbin_base, 
                    176U);

#endif

            /*** dmald() calls ***/
            break;
        case 51:
            /*** poke() calls ***/
            /* Intermediate buffers */
            // Poke HMB intermediate input buffer: concat_1_580000163_
            coproc_poke_word_offset(dag_base + 0,
                dagbin_base, 104U);

            // Poke HMB intermediate input buffer: concat_1_580000168_
            coproc_poke_word_offset(dag_base + 1050624,
                dagbin_base, 28U);

            // Poke HMB intermediate input buffer: concat_1_580000169_
            coproc_poke_word_offset(dag_base + 10155008,
                dagbin_base, 68U);

            // Poke HMB intermediate output buffer: Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___136___BatchNorm_5__FusedBatchNorm___Relu_3___muli___492___Mean_3_src0_nhwc___nhwc___transpose___138_580000024_
            coproc_poke_word_offset(dag_base + 2116608,
                dagbin_base, 344U);

            /* Pokable fields */
#ifdef CONCAT_1_580000168__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: concat_1_580000168__dram_pitch
            if (o_args->concat_1_580000168__dram_pitch_enable)
                coproc_poke_half_offset(o_args->concat_1_580000168__dram_pitch_value, dagbin_base, 
                    32U);

#endif

#ifdef CONCAT_1_580000169__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: concat_1_580000169__dram_pitch
            if (o_args->concat_1_580000169__dram_pitch_enable)
                coproc_poke_half_offset(o_args->concat_1_580000169__dram_pitch_value, dagbin_base, 
                    72U);

#endif

#ifdef CONCAT_1_580000163__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: concat_1_580000163__dram_pitch
            if (o_args->concat_1_580000163__dram_pitch_enable)
                coproc_poke_half_offset(o_args->concat_1_580000163__dram_pitch_value, dagbin_base, 
                    108U);

#endif

#ifdef CONV_5__CONV2D___BIASADD___BATCHNORM_5__FUSEDBATCHNORM___MULI___136___BATCHNORM_5__FUSEDBATCHNORM___RELU_3___MULI___492___MEAN_3_SRC0_NHWC___NHWC___TRANSPOSE___138_580000024__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___136___BatchNorm_5__FusedBatchNorm___Relu_3___muli___492___Mean_3_src0_nhwc___nhwc___transpose___138_580000024__dram_pitch
            if (o_args->Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___136___BatchNorm_5__FusedBatchNorm___Relu_3___muli___492___Mean_3_src0_nhwc___nhwc___transpose___138_580000024__dram_pitch_enable)
                coproc_poke_half_offset(o_args->Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___136___BatchNorm_5__FusedBatchNorm___Relu_3___muli___492___Mean_3_src0_nhwc___nhwc___transpose___138_580000024__dram_pitch_value, dagbin_base, 
                    348U);

#endif

            /*** dmald() calls ***/
            break;
        case 52:
            /*** poke() calls ***/
            /* Intermediate buffers */
            // Poke HMB intermediate input buffer: concat_1
            coproc_poke_word_offset(dag_base + 39321600,
                dagbin_base, 28U);

            // Poke HMB intermediate output buffer: concat_1_580000181_
            coproc_poke_word_offset(dag_base + 0,
                dagbin_base, 124U);

            // Poke HMB intermediate output buffer: concat_1_580000182_
            coproc_poke_word_offset(dag_base + 3150848,
                dagbin_base, 176U);

            /* Pokable fields */
#ifdef CONCAT_1_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: concat_1_dram_pitch
            if (o_args->concat_1_dram_pitch_enable)
                coproc_poke_half_offset(o_args->concat_1_dram_pitch_value, dagbin_base, 
                    32U);

#endif

#ifdef CONCAT_1_580000181__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: concat_1_580000181__dram_pitch
            if (o_args->concat_1_580000181__dram_pitch_enable)
                coproc_poke_half_offset(o_args->concat_1_580000181__dram_pitch_value, dagbin_base, 
                    128U);

#endif

#ifdef CONCAT_1_580000182__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: concat_1_580000182__dram_pitch
            if (o_args->concat_1_580000182__dram_pitch_enable)
                coproc_poke_half_offset(o_args->concat_1_580000182__dram_pitch_value, dagbin_base, 
                    180U);

#endif

            /*** dmald() calls ***/
            break;
        case 53:
            /*** poke() calls ***/
            /* Intermediate buffers */
            // Poke HMB intermediate input buffer: concat_1_580000182_
            coproc_poke_word_offset(dag_base + 3150848,
                dagbin_base, 28U);

            // Poke HMB intermediate input buffer: concat_1
            coproc_poke_word_offset(dag_base + 39321600,
                dagbin_base, 64U);

            // Poke HMB intermediate output buffer: concat_1_580000186_
            coproc_poke_word_offset(dag_base + 1050624,
                dagbin_base, 120U);

            // Poke HMB intermediate output buffer: concat_1_580000187_
            coproc_poke_word_offset(dag_base + 10155008,
                dagbin_base, 172U);

            /* Pokable fields */
#ifdef CONCAT_1_580000182__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: concat_1_580000182__dram_pitch
            if (o_args->concat_1_580000182__dram_pitch_enable)
                coproc_poke_half_offset(o_args->concat_1_580000182__dram_pitch_value, dagbin_base, 
                    32U);

#endif

#ifdef CONCAT_1_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: concat_1_dram_pitch
            if (o_args->concat_1_dram_pitch_enable)
                coproc_poke_half_offset(o_args->concat_1_dram_pitch_value, dagbin_base, 
                    68U);

#endif

#ifdef CONCAT_1_580000186__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: concat_1_580000186__dram_pitch
            if (o_args->concat_1_580000186__dram_pitch_enable)
                coproc_poke_half_offset(o_args->concat_1_580000186__dram_pitch_value, dagbin_base, 
                    124U);

#endif

#ifdef CONCAT_1_580000187__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: concat_1_580000187__dram_pitch
            if (o_args->concat_1_580000187__dram_pitch_enable)
                coproc_poke_half_offset(o_args->concat_1_580000187__dram_pitch_value, dagbin_base, 
                    176U);

#endif

            /*** dmald() calls ***/
            break;
        case 54:
            /*** poke() calls ***/
            /* Intermediate buffers */
            // Poke HMB intermediate input buffer: concat_1_580000181_
            coproc_poke_word_offset(dag_base + 0,
                dagbin_base, 104U);

            // Poke HMB intermediate input buffer: concat_1_580000186_
            coproc_poke_word_offset(dag_base + 1050624,
                dagbin_base, 28U);

            // Poke HMB intermediate input buffer: concat_1_580000187_
            coproc_poke_word_offset(dag_base + 10155008,
                dagbin_base, 68U);

            // Poke HMB intermediate output buffer: Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___136___BatchNorm_5__FusedBatchNorm___Relu_3___muli___492___Mean_3_src0_nhwc___nhwc___transpose___138_580000029_
            coproc_poke_word_offset(dag_base + 2121728,
                dagbin_base, 344U);

            /* Pokable fields */
#ifdef CONCAT_1_580000186__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: concat_1_580000186__dram_pitch
            if (o_args->concat_1_580000186__dram_pitch_enable)
                coproc_poke_half_offset(o_args->concat_1_580000186__dram_pitch_value, dagbin_base, 
                    32U);

#endif

#ifdef CONCAT_1_580000187__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: concat_1_580000187__dram_pitch
            if (o_args->concat_1_580000187__dram_pitch_enable)
                coproc_poke_half_offset(o_args->concat_1_580000187__dram_pitch_value, dagbin_base, 
                    72U);

#endif

#ifdef CONCAT_1_580000181__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: concat_1_580000181__dram_pitch
            if (o_args->concat_1_580000181__dram_pitch_enable)
                coproc_poke_half_offset(o_args->concat_1_580000181__dram_pitch_value, dagbin_base, 
                    108U);

#endif

#ifdef CONV_5__CONV2D___BIASADD___BATCHNORM_5__FUSEDBATCHNORM___MULI___136___BATCHNORM_5__FUSEDBATCHNORM___RELU_3___MULI___492___MEAN_3_SRC0_NHWC___NHWC___TRANSPOSE___138_580000029__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___136___BatchNorm_5__FusedBatchNorm___Relu_3___muli___492___Mean_3_src0_nhwc___nhwc___transpose___138_580000029__dram_pitch
            if (o_args->Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___136___BatchNorm_5__FusedBatchNorm___Relu_3___muli___492___Mean_3_src0_nhwc___nhwc___transpose___138_580000029__dram_pitch_enable)
                coproc_poke_half_offset(o_args->Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___136___BatchNorm_5__FusedBatchNorm___Relu_3___muli___492___Mean_3_src0_nhwc___nhwc___transpose___138_580000029__dram_pitch_value, dagbin_base, 
                    348U);

#endif

            /*** dmald() calls ***/
            break;
        case 55:
            /*** poke() calls ***/
            /* Intermediate buffers */
            // Poke HMB intermediate input buffer: concat_1
            coproc_poke_word_offset(dag_base + 39321600,
                dagbin_base, 28U);

            // Poke HMB intermediate output buffer: concat_1_580000199_
            coproc_poke_word_offset(dag_base + 0,
                dagbin_base, 124U);

            // Poke HMB intermediate output buffer: concat_1_580000200_
            coproc_poke_word_offset(dag_base + 3150848,
                dagbin_base, 176U);

            /* Pokable fields */
#ifdef CONCAT_1_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: concat_1_dram_pitch
            if (o_args->concat_1_dram_pitch_enable)
                coproc_poke_half_offset(o_args->concat_1_dram_pitch_value, dagbin_base, 
                    32U);

#endif

#ifdef CONCAT_1_580000199__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: concat_1_580000199__dram_pitch
            if (o_args->concat_1_580000199__dram_pitch_enable)
                coproc_poke_half_offset(o_args->concat_1_580000199__dram_pitch_value, dagbin_base, 
                    128U);

#endif

#ifdef CONCAT_1_580000200__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: concat_1_580000200__dram_pitch
            if (o_args->concat_1_580000200__dram_pitch_enable)
                coproc_poke_half_offset(o_args->concat_1_580000200__dram_pitch_value, dagbin_base, 
                    180U);

#endif

            /*** dmald() calls ***/
            break;
        case 56:
            /*** poke() calls ***/
            /* Intermediate buffers */
            // Poke HMB intermediate input buffer: concat_1_580000200_
            coproc_poke_word_offset(dag_base + 3150848,
                dagbin_base, 28U);

            // Poke HMB intermediate input buffer: concat_1
            coproc_poke_word_offset(dag_base + 39321600,
                dagbin_base, 64U);

            // Poke HMB intermediate output buffer: concat_1_580000204_
            coproc_poke_word_offset(dag_base + 1050624,
                dagbin_base, 120U);

            // Poke HMB intermediate output buffer: concat_1_580000205_
            coproc_poke_word_offset(dag_base + 10155008,
                dagbin_base, 172U);

            /* Pokable fields */
#ifdef CONCAT_1_580000200__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: concat_1_580000200__dram_pitch
            if (o_args->concat_1_580000200__dram_pitch_enable)
                coproc_poke_half_offset(o_args->concat_1_580000200__dram_pitch_value, dagbin_base, 
                    32U);

#endif

#ifdef CONCAT_1_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: concat_1_dram_pitch
            if (o_args->concat_1_dram_pitch_enable)
                coproc_poke_half_offset(o_args->concat_1_dram_pitch_value, dagbin_base, 
                    68U);

#endif

#ifdef CONCAT_1_580000204__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: concat_1_580000204__dram_pitch
            if (o_args->concat_1_580000204__dram_pitch_enable)
                coproc_poke_half_offset(o_args->concat_1_580000204__dram_pitch_value, dagbin_base, 
                    124U);

#endif

#ifdef CONCAT_1_580000205__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: concat_1_580000205__dram_pitch
            if (o_args->concat_1_580000205__dram_pitch_enable)
                coproc_poke_half_offset(o_args->concat_1_580000205__dram_pitch_value, dagbin_base, 
                    176U);

#endif

            /*** dmald() calls ***/
            break;
        case 57:
            /*** poke() calls ***/
            /* Intermediate buffers */
            // Poke HMB intermediate input buffer: concat_1_580000199_
            coproc_poke_word_offset(dag_base + 0,
                dagbin_base, 104U);

            // Poke HMB intermediate input buffer: concat_1_580000204_
            coproc_poke_word_offset(dag_base + 1050624,
                dagbin_base, 28U);

            // Poke HMB intermediate input buffer: concat_1_580000205_
            coproc_poke_word_offset(dag_base + 10155008,
                dagbin_base, 68U);

            // Poke HMB intermediate output buffer: Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___136___BatchNorm_5__FusedBatchNorm___Relu_3___muli___492___Mean_3_src0_nhwc___nhwc___transpose___138_580000034_
            coproc_poke_word_offset(dag_base + 2126848,
                dagbin_base, 344U);

            /* Pokable fields */
#ifdef CONCAT_1_580000204__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: concat_1_580000204__dram_pitch
            if (o_args->concat_1_580000204__dram_pitch_enable)
                coproc_poke_half_offset(o_args->concat_1_580000204__dram_pitch_value, dagbin_base, 
                    32U);

#endif

#ifdef CONCAT_1_580000205__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: concat_1_580000205__dram_pitch
            if (o_args->concat_1_580000205__dram_pitch_enable)
                coproc_poke_half_offset(o_args->concat_1_580000205__dram_pitch_value, dagbin_base, 
                    72U);

#endif

#ifdef CONCAT_1_580000199__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: concat_1_580000199__dram_pitch
            if (o_args->concat_1_580000199__dram_pitch_enable)
                coproc_poke_half_offset(o_args->concat_1_580000199__dram_pitch_value, dagbin_base, 
                    108U);

#endif

#ifdef CONV_5__CONV2D___BIASADD___BATCHNORM_5__FUSEDBATCHNORM___MULI___136___BATCHNORM_5__FUSEDBATCHNORM___RELU_3___MULI___492___MEAN_3_SRC0_NHWC___NHWC___TRANSPOSE___138_580000034__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___136___BatchNorm_5__FusedBatchNorm___Relu_3___muli___492___Mean_3_src0_nhwc___nhwc___transpose___138_580000034__dram_pitch
            if (o_args->Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___136___BatchNorm_5__FusedBatchNorm___Relu_3___muli___492___Mean_3_src0_nhwc___nhwc___transpose___138_580000034__dram_pitch_enable)
                coproc_poke_half_offset(o_args->Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___136___BatchNorm_5__FusedBatchNorm___Relu_3___muli___492___Mean_3_src0_nhwc___nhwc___transpose___138_580000034__dram_pitch_value, dagbin_base, 
                    348U);

#endif

            /*** dmald() calls ***/
            break;
        case 58:
            /*** poke() calls ***/
            /* Intermediate buffers */
            // Poke HMB intermediate input buffer: concat_1
            coproc_poke_word_offset(dag_base + 39321600,
                dagbin_base, 28U);

            // Poke HMB intermediate output buffer: concat_1_580000217_
            coproc_poke_word_offset(dag_base + 0,
                dagbin_base, 124U);

            // Poke HMB intermediate output buffer: concat_1_580000218_
            coproc_poke_word_offset(dag_base + 3150848,
                dagbin_base, 176U);

            /* Pokable fields */
#ifdef CONCAT_1_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: concat_1_dram_pitch
            if (o_args->concat_1_dram_pitch_enable)
                coproc_poke_half_offset(o_args->concat_1_dram_pitch_value, dagbin_base, 
                    32U);

#endif

#ifdef CONCAT_1_580000217__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: concat_1_580000217__dram_pitch
            if (o_args->concat_1_580000217__dram_pitch_enable)
                coproc_poke_half_offset(o_args->concat_1_580000217__dram_pitch_value, dagbin_base, 
                    128U);

#endif

#ifdef CONCAT_1_580000218__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: concat_1_580000218__dram_pitch
            if (o_args->concat_1_580000218__dram_pitch_enable)
                coproc_poke_half_offset(o_args->concat_1_580000218__dram_pitch_value, dagbin_base, 
                    180U);

#endif

            /*** dmald() calls ***/
            break;
        case 59:
            /*** poke() calls ***/
            /* Intermediate buffers */
            // Poke HMB intermediate input buffer: concat_1_580000218_
            coproc_poke_word_offset(dag_base + 3150848,
                dagbin_base, 28U);

            // Poke HMB intermediate input buffer: concat_1
            coproc_poke_word_offset(dag_base + 39321600,
                dagbin_base, 64U);

            // Poke HMB intermediate output buffer: concat_1_580000222_
            coproc_poke_word_offset(dag_base + 1050624,
                dagbin_base, 120U);

            // Poke HMB intermediate output buffer: concat_1_580000223_
            coproc_poke_word_offset(dag_base + 10155008,
                dagbin_base, 172U);

            /* Pokable fields */
#ifdef CONCAT_1_580000218__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: concat_1_580000218__dram_pitch
            if (o_args->concat_1_580000218__dram_pitch_enable)
                coproc_poke_half_offset(o_args->concat_1_580000218__dram_pitch_value, dagbin_base, 
                    32U);

#endif

#ifdef CONCAT_1_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: concat_1_dram_pitch
            if (o_args->concat_1_dram_pitch_enable)
                coproc_poke_half_offset(o_args->concat_1_dram_pitch_value, dagbin_base, 
                    68U);

#endif

#ifdef CONCAT_1_580000222__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: concat_1_580000222__dram_pitch
            if (o_args->concat_1_580000222__dram_pitch_enable)
                coproc_poke_half_offset(o_args->concat_1_580000222__dram_pitch_value, dagbin_base, 
                    124U);

#endif

#ifdef CONCAT_1_580000223__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: concat_1_580000223__dram_pitch
            if (o_args->concat_1_580000223__dram_pitch_enable)
                coproc_poke_half_offset(o_args->concat_1_580000223__dram_pitch_value, dagbin_base, 
                    176U);

#endif

            /*** dmald() calls ***/
            break;
        case 60:
            /*** poke() calls ***/
            /* Intermediate buffers */
            // Poke HMB intermediate input buffer: concat_1_580000217_
            coproc_poke_word_offset(dag_base + 0,
                dagbin_base, 104U);

            // Poke HMB intermediate input buffer: concat_1_580000222_
            coproc_poke_word_offset(dag_base + 1050624,
                dagbin_base, 28U);

            // Poke HMB intermediate input buffer: concat_1_580000223_
            coproc_poke_word_offset(dag_base + 10155008,
                dagbin_base, 68U);

            // Poke HMB intermediate output buffer: Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___136___BatchNorm_5__FusedBatchNorm___Relu_3___muli___492___Mean_3_src0_nhwc___nhwc___transpose___138_580000039_
            coproc_poke_word_offset(dag_base + 2131968,
                dagbin_base, 344U);

            /* Pokable fields */
#ifdef CONCAT_1_580000222__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: concat_1_580000222__dram_pitch
            if (o_args->concat_1_580000222__dram_pitch_enable)
                coproc_poke_half_offset(o_args->concat_1_580000222__dram_pitch_value, dagbin_base, 
                    32U);

#endif

#ifdef CONCAT_1_580000223__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: concat_1_580000223__dram_pitch
            if (o_args->concat_1_580000223__dram_pitch_enable)
                coproc_poke_half_offset(o_args->concat_1_580000223__dram_pitch_value, dagbin_base, 
                    72U);

#endif

#ifdef CONCAT_1_580000217__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: concat_1_580000217__dram_pitch
            if (o_args->concat_1_580000217__dram_pitch_enable)
                coproc_poke_half_offset(o_args->concat_1_580000217__dram_pitch_value, dagbin_base, 
                    108U);

#endif

#ifdef CONV_5__CONV2D___BIASADD___BATCHNORM_5__FUSEDBATCHNORM___MULI___136___BATCHNORM_5__FUSEDBATCHNORM___RELU_3___MULI___492___MEAN_3_SRC0_NHWC___NHWC___TRANSPOSE___138_580000039__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___136___BatchNorm_5__FusedBatchNorm___Relu_3___muli___492___Mean_3_src0_nhwc___nhwc___transpose___138_580000039__dram_pitch
            if (o_args->Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___136___BatchNorm_5__FusedBatchNorm___Relu_3___muli___492___Mean_3_src0_nhwc___nhwc___transpose___138_580000039__dram_pitch_enable)
                coproc_poke_half_offset(o_args->Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___136___BatchNorm_5__FusedBatchNorm___Relu_3___muli___492___Mean_3_src0_nhwc___nhwc___transpose___138_580000039__dram_pitch_value, dagbin_base, 
                    348U);

#endif

            /*** dmald() calls ***/
            break;
        case 61:
            /*** poke() calls ***/
            /* Intermediate buffers */
            // Poke HMB intermediate input buffer: concat_1
            coproc_poke_word_offset(dag_base + 39321600,
                dagbin_base, 28U);

            // Poke HMB intermediate output buffer: concat_1_580000235_
            coproc_poke_word_offset(dag_base + 0,
                dagbin_base, 124U);

            // Poke HMB intermediate output buffer: concat_1_580000236_
            coproc_poke_word_offset(dag_base + 3150848,
                dagbin_base, 176U);

            /* Pokable fields */
#ifdef CONCAT_1_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: concat_1_dram_pitch
            if (o_args->concat_1_dram_pitch_enable)
                coproc_poke_half_offset(o_args->concat_1_dram_pitch_value, dagbin_base, 
                    32U);

#endif

#ifdef CONCAT_1_580000235__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: concat_1_580000235__dram_pitch
            if (o_args->concat_1_580000235__dram_pitch_enable)
                coproc_poke_half_offset(o_args->concat_1_580000235__dram_pitch_value, dagbin_base, 
                    128U);

#endif

#ifdef CONCAT_1_580000236__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: concat_1_580000236__dram_pitch
            if (o_args->concat_1_580000236__dram_pitch_enable)
                coproc_poke_half_offset(o_args->concat_1_580000236__dram_pitch_value, dagbin_base, 
                    180U);

#endif

            /*** dmald() calls ***/
            break;
        case 62:
            /*** poke() calls ***/
            /* Intermediate buffers */
            // Poke HMB intermediate input buffer: concat_1_580000236_
            coproc_poke_word_offset(dag_base + 3150848,
                dagbin_base, 28U);

            // Poke HMB intermediate input buffer: concat_1
            coproc_poke_word_offset(dag_base + 39321600,
                dagbin_base, 64U);

            // Poke HMB intermediate output buffer: concat_1_580000240_
            coproc_poke_word_offset(dag_base + 1050624,
                dagbin_base, 120U);

            // Poke HMB intermediate output buffer: concat_1_580000241_
            coproc_poke_word_offset(dag_base + 10155008,
                dagbin_base, 172U);

            /* Pokable fields */
#ifdef CONCAT_1_580000236__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: concat_1_580000236__dram_pitch
            if (o_args->concat_1_580000236__dram_pitch_enable)
                coproc_poke_half_offset(o_args->concat_1_580000236__dram_pitch_value, dagbin_base, 
                    32U);

#endif

#ifdef CONCAT_1_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: concat_1_dram_pitch
            if (o_args->concat_1_dram_pitch_enable)
                coproc_poke_half_offset(o_args->concat_1_dram_pitch_value, dagbin_base, 
                    68U);

#endif

#ifdef CONCAT_1_580000240__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: concat_1_580000240__dram_pitch
            if (o_args->concat_1_580000240__dram_pitch_enable)
                coproc_poke_half_offset(o_args->concat_1_580000240__dram_pitch_value, dagbin_base, 
                    124U);

#endif

#ifdef CONCAT_1_580000241__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: concat_1_580000241__dram_pitch
            if (o_args->concat_1_580000241__dram_pitch_enable)
                coproc_poke_half_offset(o_args->concat_1_580000241__dram_pitch_value, dagbin_base, 
                    176U);

#endif

            /*** dmald() calls ***/
            break;
        case 63:
            /*** poke() calls ***/
            /* Intermediate buffers */
            // Poke HMB intermediate input buffer: concat_1_580000235_
            coproc_poke_word_offset(dag_base + 0,
                dagbin_base, 104U);

            // Poke HMB intermediate input buffer: concat_1_580000240_
            coproc_poke_word_offset(dag_base + 1050624,
                dagbin_base, 28U);

            // Poke HMB intermediate input buffer: concat_1_580000241_
            coproc_poke_word_offset(dag_base + 10155008,
                dagbin_base, 68U);

            // Poke HMB intermediate output buffer: Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___136___BatchNorm_5__FusedBatchNorm___Relu_3___muli___492___Mean_3_src0_nhwc___nhwc___transpose___138_580000044_
            coproc_poke_word_offset(dag_base + 2137088,
                dagbin_base, 344U);

            /* Pokable fields */
#ifdef CONCAT_1_580000240__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: concat_1_580000240__dram_pitch
            if (o_args->concat_1_580000240__dram_pitch_enable)
                coproc_poke_half_offset(o_args->concat_1_580000240__dram_pitch_value, dagbin_base, 
                    32U);

#endif

#ifdef CONCAT_1_580000241__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: concat_1_580000241__dram_pitch
            if (o_args->concat_1_580000241__dram_pitch_enable)
                coproc_poke_half_offset(o_args->concat_1_580000241__dram_pitch_value, dagbin_base, 
                    72U);

#endif

#ifdef CONCAT_1_580000235__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: concat_1_580000235__dram_pitch
            if (o_args->concat_1_580000235__dram_pitch_enable)
                coproc_poke_half_offset(o_args->concat_1_580000235__dram_pitch_value, dagbin_base, 
                    108U);

#endif

#ifdef CONV_5__CONV2D___BIASADD___BATCHNORM_5__FUSEDBATCHNORM___MULI___136___BATCHNORM_5__FUSEDBATCHNORM___RELU_3___MULI___492___MEAN_3_SRC0_NHWC___NHWC___TRANSPOSE___138_580000044__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___136___BatchNorm_5__FusedBatchNorm___Relu_3___muli___492___Mean_3_src0_nhwc___nhwc___transpose___138_580000044__dram_pitch
            if (o_args->Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___136___BatchNorm_5__FusedBatchNorm___Relu_3___muli___492___Mean_3_src0_nhwc___nhwc___transpose___138_580000044__dram_pitch_enable)
                coproc_poke_half_offset(o_args->Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___136___BatchNorm_5__FusedBatchNorm___Relu_3___muli___492___Mean_3_src0_nhwc___nhwc___transpose___138_580000044__dram_pitch_value, dagbin_base, 
                    348U);

#endif

            /*** dmald() calls ***/
            break;
        case 64:
            /*** poke() calls ***/
            /* Intermediate buffers */
            // Poke HMB intermediate input buffer: concat_1
            coproc_poke_word_offset(dag_base + 39321600,
                dagbin_base, 28U);

            // Poke HMB intermediate output buffer: concat_1_580000253_
            coproc_poke_word_offset(dag_base + 0,
                dagbin_base, 124U);

            // Poke HMB intermediate output buffer: concat_1_580000254_
            coproc_poke_word_offset(dag_base + 3150848,
                dagbin_base, 176U);

            /* Pokable fields */
#ifdef CONCAT_1_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: concat_1_dram_pitch
            if (o_args->concat_1_dram_pitch_enable)
                coproc_poke_half_offset(o_args->concat_1_dram_pitch_value, dagbin_base, 
                    32U);

#endif

#ifdef CONCAT_1_580000253__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: concat_1_580000253__dram_pitch
            if (o_args->concat_1_580000253__dram_pitch_enable)
                coproc_poke_half_offset(o_args->concat_1_580000253__dram_pitch_value, dagbin_base, 
                    128U);

#endif

#ifdef CONCAT_1_580000254__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: concat_1_580000254__dram_pitch
            if (o_args->concat_1_580000254__dram_pitch_enable)
                coproc_poke_half_offset(o_args->concat_1_580000254__dram_pitch_value, dagbin_base, 
                    180U);

#endif

            /*** dmald() calls ***/
            break;
        case 65:
            /*** poke() calls ***/
            /* Intermediate buffers */
            // Poke HMB intermediate input buffer: concat_1_580000254_
            coproc_poke_word_offset(dag_base + 3150848,
                dagbin_base, 28U);

            // Poke HMB intermediate input buffer: concat_1
            coproc_poke_word_offset(dag_base + 39321600,
                dagbin_base, 64U);

            // Poke HMB intermediate output buffer: concat_1_580000258_
            coproc_poke_word_offset(dag_base + 1050624,
                dagbin_base, 120U);

            // Poke HMB intermediate output buffer: concat_1_580000259_
            coproc_poke_word_offset(dag_base + 10155008,
                dagbin_base, 172U);

            /* Pokable fields */
#ifdef CONCAT_1_580000254__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: concat_1_580000254__dram_pitch
            if (o_args->concat_1_580000254__dram_pitch_enable)
                coproc_poke_half_offset(o_args->concat_1_580000254__dram_pitch_value, dagbin_base, 
                    32U);

#endif

#ifdef CONCAT_1_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: concat_1_dram_pitch
            if (o_args->concat_1_dram_pitch_enable)
                coproc_poke_half_offset(o_args->concat_1_dram_pitch_value, dagbin_base, 
                    68U);

#endif

#ifdef CONCAT_1_580000258__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: concat_1_580000258__dram_pitch
            if (o_args->concat_1_580000258__dram_pitch_enable)
                coproc_poke_half_offset(o_args->concat_1_580000258__dram_pitch_value, dagbin_base, 
                    124U);

#endif

#ifdef CONCAT_1_580000259__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: concat_1_580000259__dram_pitch
            if (o_args->concat_1_580000259__dram_pitch_enable)
                coproc_poke_half_offset(o_args->concat_1_580000259__dram_pitch_value, dagbin_base, 
                    176U);

#endif

            /*** dmald() calls ***/
            break;
        case 66:
            /*** poke() calls ***/
            /* Intermediate buffers */
            // Poke HMB intermediate input buffer: concat_1_580000253_
            coproc_poke_word_offset(dag_base + 0,
                dagbin_base, 104U);

            // Poke HMB intermediate input buffer: concat_1_580000258_
            coproc_poke_word_offset(dag_base + 1050624,
                dagbin_base, 28U);

            // Poke HMB intermediate input buffer: concat_1_580000259_
            coproc_poke_word_offset(dag_base + 10155008,
                dagbin_base, 68U);

            // Poke HMB intermediate output buffer: Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___136___BatchNorm_5__FusedBatchNorm___Relu_3___muli___492___Mean_3_src0_nhwc___nhwc___transpose___138_580000049_
            coproc_poke_word_offset(dag_base + 2142208,
                dagbin_base, 344U);

            /* Pokable fields */
#ifdef CONCAT_1_580000258__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: concat_1_580000258__dram_pitch
            if (o_args->concat_1_580000258__dram_pitch_enable)
                coproc_poke_half_offset(o_args->concat_1_580000258__dram_pitch_value, dagbin_base, 
                    32U);

#endif

#ifdef CONCAT_1_580000259__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: concat_1_580000259__dram_pitch
            if (o_args->concat_1_580000259__dram_pitch_enable)
                coproc_poke_half_offset(o_args->concat_1_580000259__dram_pitch_value, dagbin_base, 
                    72U);

#endif

#ifdef CONCAT_1_580000253__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: concat_1_580000253__dram_pitch
            if (o_args->concat_1_580000253__dram_pitch_enable)
                coproc_poke_half_offset(o_args->concat_1_580000253__dram_pitch_value, dagbin_base, 
                    108U);

#endif

#ifdef CONV_5__CONV2D___BIASADD___BATCHNORM_5__FUSEDBATCHNORM___MULI___136___BATCHNORM_5__FUSEDBATCHNORM___RELU_3___MULI___492___MEAN_3_SRC0_NHWC___NHWC___TRANSPOSE___138_580000049__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___136___BatchNorm_5__FusedBatchNorm___Relu_3___muli___492___Mean_3_src0_nhwc___nhwc___transpose___138_580000049__dram_pitch
            if (o_args->Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___136___BatchNorm_5__FusedBatchNorm___Relu_3___muli___492___Mean_3_src0_nhwc___nhwc___transpose___138_580000049__dram_pitch_enable)
                coproc_poke_half_offset(o_args->Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___136___BatchNorm_5__FusedBatchNorm___Relu_3___muli___492___Mean_3_src0_nhwc___nhwc___transpose___138_580000049__dram_pitch_value, dagbin_base, 
                    348U);

#endif

            /*** dmald() calls ***/
            break;
        case 67:
            /*** poke() calls ***/
            /* Intermediate buffers */
            // Poke HMB intermediate input buffer: Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___136___BatchNorm_5__FusedBatchNorm___Relu_3___muli___492___Mean_3_src0_nhwc___nhwc___transpose___138_580000009_
            coproc_poke_word_offset(dag_base + 2101248,
                dagbin_base, 348U);

            // Poke HMB intermediate input buffer: Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___136___BatchNorm_5__FusedBatchNorm___Relu_3___muli___492___Mean_3_src0_nhwc___nhwc___transpose___138_580000014_
            coproc_poke_word_offset(dag_base + 2106368,
                dagbin_base, 308U);

            // Poke HMB intermediate input buffer: Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___136___BatchNorm_5__FusedBatchNorm___Relu_3___muli___492___Mean_3_src0_nhwc___nhwc___transpose___138_580000019_
            coproc_poke_word_offset(dag_base + 2111488,
                dagbin_base, 268U);

            // Poke HMB intermediate input buffer: Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___136___BatchNorm_5__FusedBatchNorm___Relu_3___muli___492___Mean_3_src0_nhwc___nhwc___transpose___138_580000024_
            coproc_poke_word_offset(dag_base + 2116608,
                dagbin_base, 228U);

            // Poke HMB intermediate input buffer: Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___136___BatchNorm_5__FusedBatchNorm___Relu_3___muli___492___Mean_3_src0_nhwc___nhwc___transpose___138_580000029_
            coproc_poke_word_offset(dag_base + 2121728,
                dagbin_base, 188U);

            // Poke HMB intermediate input buffer: Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___136___BatchNorm_5__FusedBatchNorm___Relu_3___muli___492___Mean_3_src0_nhwc___nhwc___transpose___138_580000034_
            coproc_poke_word_offset(dag_base + 2126848,
                dagbin_base, 148U);

            // Poke HMB intermediate input buffer: Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___136___BatchNorm_5__FusedBatchNorm___Relu_3___muli___492___Mean_3_src0_nhwc___nhwc___transpose___138_580000039_
            coproc_poke_word_offset(dag_base + 2131968,
                dagbin_base, 108U);

            // Poke HMB intermediate input buffer: Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___136___BatchNorm_5__FusedBatchNorm___Relu_3___muli___492___Mean_3_src0_nhwc___nhwc___transpose___138_580000044_
            coproc_poke_word_offset(dag_base + 2137088,
                dagbin_base, 68U);

            // Poke HMB intermediate input buffer: Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___136___BatchNorm_5__FusedBatchNorm___Relu_3___muli___492___Mean_3_src0_nhwc___nhwc___transpose___138_580000049_
            coproc_poke_word_offset(dag_base + 2142208,
                dagbin_base, 28U);

            // Poke HMB intermediate input buffer: Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___136___BatchNorm_5__FusedBatchNorm___Relu_3___muli___492___Mean_3_src0_nhwc___nhwc___transpose___138_580000004_
            coproc_poke_word_offset(dag_base + 3145728,
                dagbin_base, 384U);

            // Poke HMB intermediate output buffer: Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___136___BatchNorm_5__FusedBatchNorm___Relu_3___muli___492___Mean_3_src0_nhwc___nhwc___transpose___138
            coproc_poke_word_offset(dag_base + 0,
                dagbin_base, 424U);

            /* Pokable fields */
#ifdef CONV_5__CONV2D___BIASADD___BATCHNORM_5__FUSEDBATCHNORM___MULI___136___BATCHNORM_5__FUSEDBATCHNORM___RELU_3___MULI___492___MEAN_3_SRC0_NHWC___NHWC___TRANSPOSE___138_580000049__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___136___BatchNorm_5__FusedBatchNorm___Relu_3___muli___492___Mean_3_src0_nhwc___nhwc___transpose___138_580000049__dram_pitch
            if (o_args->Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___136___BatchNorm_5__FusedBatchNorm___Relu_3___muli___492___Mean_3_src0_nhwc___nhwc___transpose___138_580000049__dram_pitch_enable)
                coproc_poke_half_offset(o_args->Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___136___BatchNorm_5__FusedBatchNorm___Relu_3___muli___492___Mean_3_src0_nhwc___nhwc___transpose___138_580000049__dram_pitch_value, dagbin_base, 
                    32U);

#endif

#ifdef CONV_5__CONV2D___BIASADD___BATCHNORM_5__FUSEDBATCHNORM___MULI___136___BATCHNORM_5__FUSEDBATCHNORM___RELU_3___MULI___492___MEAN_3_SRC0_NHWC___NHWC___TRANSPOSE___138_580000044__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___136___BatchNorm_5__FusedBatchNorm___Relu_3___muli___492___Mean_3_src0_nhwc___nhwc___transpose___138_580000044__dram_pitch
            if (o_args->Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___136___BatchNorm_5__FusedBatchNorm___Relu_3___muli___492___Mean_3_src0_nhwc___nhwc___transpose___138_580000044__dram_pitch_enable)
                coproc_poke_half_offset(o_args->Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___136___BatchNorm_5__FusedBatchNorm___Relu_3___muli___492___Mean_3_src0_nhwc___nhwc___transpose___138_580000044__dram_pitch_value, dagbin_base, 
                    72U);

#endif

#ifdef CONV_5__CONV2D___BIASADD___BATCHNORM_5__FUSEDBATCHNORM___MULI___136___BATCHNORM_5__FUSEDBATCHNORM___RELU_3___MULI___492___MEAN_3_SRC0_NHWC___NHWC___TRANSPOSE___138_580000039__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___136___BatchNorm_5__FusedBatchNorm___Relu_3___muli___492___Mean_3_src0_nhwc___nhwc___transpose___138_580000039__dram_pitch
            if (o_args->Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___136___BatchNorm_5__FusedBatchNorm___Relu_3___muli___492___Mean_3_src0_nhwc___nhwc___transpose___138_580000039__dram_pitch_enable)
                coproc_poke_half_offset(o_args->Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___136___BatchNorm_5__FusedBatchNorm___Relu_3___muli___492___Mean_3_src0_nhwc___nhwc___transpose___138_580000039__dram_pitch_value, dagbin_base, 
                    112U);

#endif

#ifdef CONV_5__CONV2D___BIASADD___BATCHNORM_5__FUSEDBATCHNORM___MULI___136___BATCHNORM_5__FUSEDBATCHNORM___RELU_3___MULI___492___MEAN_3_SRC0_NHWC___NHWC___TRANSPOSE___138_580000034__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___136___BatchNorm_5__FusedBatchNorm___Relu_3___muli___492___Mean_3_src0_nhwc___nhwc___transpose___138_580000034__dram_pitch
            if (o_args->Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___136___BatchNorm_5__FusedBatchNorm___Relu_3___muli___492___Mean_3_src0_nhwc___nhwc___transpose___138_580000034__dram_pitch_enable)
                coproc_poke_half_offset(o_args->Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___136___BatchNorm_5__FusedBatchNorm___Relu_3___muli___492___Mean_3_src0_nhwc___nhwc___transpose___138_580000034__dram_pitch_value, dagbin_base, 
                    152U);

#endif

#ifdef CONV_5__CONV2D___BIASADD___BATCHNORM_5__FUSEDBATCHNORM___MULI___136___BATCHNORM_5__FUSEDBATCHNORM___RELU_3___MULI___492___MEAN_3_SRC0_NHWC___NHWC___TRANSPOSE___138_580000029__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___136___BatchNorm_5__FusedBatchNorm___Relu_3___muli___492___Mean_3_src0_nhwc___nhwc___transpose___138_580000029__dram_pitch
            if (o_args->Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___136___BatchNorm_5__FusedBatchNorm___Relu_3___muli___492___Mean_3_src0_nhwc___nhwc___transpose___138_580000029__dram_pitch_enable)
                coproc_poke_half_offset(o_args->Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___136___BatchNorm_5__FusedBatchNorm___Relu_3___muli___492___Mean_3_src0_nhwc___nhwc___transpose___138_580000029__dram_pitch_value, dagbin_base, 
                    192U);

#endif

#ifdef CONV_5__CONV2D___BIASADD___BATCHNORM_5__FUSEDBATCHNORM___MULI___136___BATCHNORM_5__FUSEDBATCHNORM___RELU_3___MULI___492___MEAN_3_SRC0_NHWC___NHWC___TRANSPOSE___138_580000024__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___136___BatchNorm_5__FusedBatchNorm___Relu_3___muli___492___Mean_3_src0_nhwc___nhwc___transpose___138_580000024__dram_pitch
            if (o_args->Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___136___BatchNorm_5__FusedBatchNorm___Relu_3___muli___492___Mean_3_src0_nhwc___nhwc___transpose___138_580000024__dram_pitch_enable)
                coproc_poke_half_offset(o_args->Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___136___BatchNorm_5__FusedBatchNorm___Relu_3___muli___492___Mean_3_src0_nhwc___nhwc___transpose___138_580000024__dram_pitch_value, dagbin_base, 
                    232U);

#endif

#ifdef CONV_5__CONV2D___BIASADD___BATCHNORM_5__FUSEDBATCHNORM___MULI___136___BATCHNORM_5__FUSEDBATCHNORM___RELU_3___MULI___492___MEAN_3_SRC0_NHWC___NHWC___TRANSPOSE___138_580000019__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___136___BatchNorm_5__FusedBatchNorm___Relu_3___muli___492___Mean_3_src0_nhwc___nhwc___transpose___138_580000019__dram_pitch
            if (o_args->Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___136___BatchNorm_5__FusedBatchNorm___Relu_3___muli___492___Mean_3_src0_nhwc___nhwc___transpose___138_580000019__dram_pitch_enable)
                coproc_poke_half_offset(o_args->Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___136___BatchNorm_5__FusedBatchNorm___Relu_3___muli___492___Mean_3_src0_nhwc___nhwc___transpose___138_580000019__dram_pitch_value, dagbin_base, 
                    272U);

#endif

#ifdef CONV_5__CONV2D___BIASADD___BATCHNORM_5__FUSEDBATCHNORM___MULI___136___BATCHNORM_5__FUSEDBATCHNORM___RELU_3___MULI___492___MEAN_3_SRC0_NHWC___NHWC___TRANSPOSE___138_580000014__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___136___BatchNorm_5__FusedBatchNorm___Relu_3___muli___492___Mean_3_src0_nhwc___nhwc___transpose___138_580000014__dram_pitch
            if (o_args->Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___136___BatchNorm_5__FusedBatchNorm___Relu_3___muli___492___Mean_3_src0_nhwc___nhwc___transpose___138_580000014__dram_pitch_enable)
                coproc_poke_half_offset(o_args->Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___136___BatchNorm_5__FusedBatchNorm___Relu_3___muli___492___Mean_3_src0_nhwc___nhwc___transpose___138_580000014__dram_pitch_value, dagbin_base, 
                    312U);

#endif

#ifdef CONV_5__CONV2D___BIASADD___BATCHNORM_5__FUSEDBATCHNORM___MULI___136___BATCHNORM_5__FUSEDBATCHNORM___RELU_3___MULI___492___MEAN_3_SRC0_NHWC___NHWC___TRANSPOSE___138_580000009__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___136___BatchNorm_5__FusedBatchNorm___Relu_3___muli___492___Mean_3_src0_nhwc___nhwc___transpose___138_580000009__dram_pitch
            if (o_args->Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___136___BatchNorm_5__FusedBatchNorm___Relu_3___muli___492___Mean_3_src0_nhwc___nhwc___transpose___138_580000009__dram_pitch_enable)
                coproc_poke_half_offset(o_args->Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___136___BatchNorm_5__FusedBatchNorm___Relu_3___muli___492___Mean_3_src0_nhwc___nhwc___transpose___138_580000009__dram_pitch_value, dagbin_base, 
                    352U);

#endif

#ifdef CONV_5__CONV2D___BIASADD___BATCHNORM_5__FUSEDBATCHNORM___MULI___136___BATCHNORM_5__FUSEDBATCHNORM___RELU_3___MULI___492___MEAN_3_SRC0_NHWC___NHWC___TRANSPOSE___138_580000004__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___136___BatchNorm_5__FusedBatchNorm___Relu_3___muli___492___Mean_3_src0_nhwc___nhwc___transpose___138_580000004__dram_pitch
            if (o_args->Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___136___BatchNorm_5__FusedBatchNorm___Relu_3___muli___492___Mean_3_src0_nhwc___nhwc___transpose___138_580000004__dram_pitch_enable)
                coproc_poke_half_offset(o_args->Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___136___BatchNorm_5__FusedBatchNorm___Relu_3___muli___492___Mean_3_src0_nhwc___nhwc___transpose___138_580000004__dram_pitch_value, dagbin_base, 
                    388U);

#endif

#ifdef CONV_5__CONV2D___BIASADD___BATCHNORM_5__FUSEDBATCHNORM___MULI___136___BATCHNORM_5__FUSEDBATCHNORM___RELU_3___MULI___492___MEAN_3_SRC0_NHWC___NHWC___TRANSPOSE___138_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___136___BatchNorm_5__FusedBatchNorm___Relu_3___muli___492___Mean_3_src0_nhwc___nhwc___transpose___138_dram_pitch
            if (o_args->Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___136___BatchNorm_5__FusedBatchNorm___Relu_3___muli___492___Mean_3_src0_nhwc___nhwc___transpose___138_dram_pitch_enable)
                coproc_poke_half_offset(o_args->Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___136___BatchNorm_5__FusedBatchNorm___Relu_3___muli___492___Mean_3_src0_nhwc___nhwc___transpose___138_dram_pitch_value, dagbin_base, 
                    428U);

#endif

            /*** dmald() calls ***/
            break;
        case 68:
            /*** poke() calls ***/
            /* Intermediate buffers */
            // Poke HMB intermediate input buffer: Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___136___BatchNorm_5__FusedBatchNorm___Relu_3___muli___492___Mean_3_src0_nhwc___nhwc___transpose___138
            coproc_poke_word_offset(dag_base + 0,
                dagbin_base, 28U);

            // Poke HMB intermediate output buffer: Add
            coproc_poke_word(dag_base + 61440,
                dagbin_base + 1068U);

            /* Pokable fields */
#ifdef CONV_5__CONV2D___BIASADD___BATCHNORM_5__FUSEDBATCHNORM___MULI___136___BATCHNORM_5__FUSEDBATCHNORM___RELU_3___MULI___492___MEAN_3_SRC0_NHWC___NHWC___TRANSPOSE___138_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___136___BatchNorm_5__FusedBatchNorm___Relu_3___muli___492___Mean_3_src0_nhwc___nhwc___transpose___138_dram_pitch
            if (o_args->Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___136___BatchNorm_5__FusedBatchNorm___Relu_3___muli___492___Mean_3_src0_nhwc___nhwc___transpose___138_dram_pitch_enable)
                coproc_poke_half_offset(o_args->Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___136___BatchNorm_5__FusedBatchNorm___Relu_3___muli___492___Mean_3_src0_nhwc___nhwc___transpose___138_dram_pitch_value, dagbin_base, 
                    32U);

#endif

#ifdef ADD_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: Add_dram_pitch
            if (o_args->Add_dram_pitch_enable)
                coproc_poke_half(o_args->Add_dram_pitch_value, dagbin_base + 1072U);

#endif

            /*** dmald() calls ***/
            break;
        case 69:
            /*** poke() calls ***/
            /* Intermediate buffers */
            // Poke HMB intermediate input buffer: Add
            coproc_poke_word_offset(dag_base + 61440,
                dagbin_base, 28U);

            /* Primary outputs */
            // Poke HMB output buffer: out_class
            coproc_poke_word_offset(r_args->out_class_addr + 0,
                dagbin_base, 304U);

            /* Pokable fields */
#ifdef ADD_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: Add_dram_pitch
            if (o_args->Add_dram_pitch_enable)
                coproc_poke_half_offset(o_args->Add_dram_pitch_value, dagbin_base, 
                    32U);

#endif

#ifdef OUT_CLASS_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: out_class_dram_pitch
            if (o_args->out_class_dram_pitch_enable)
                coproc_poke_half_offset(o_args->out_class_dram_pitch_value, dagbin_base, 
                    308U);

#endif

            /*** dmald() calls ***/
            break;
        default:
            break;
    }

    return;
} // end of flexidag_openseg_ag_poke_split()

/* Prompts VP to execute code */
inline void flexidag_openseg_ag_run_split(
    uint32_t split_id
) {
    coproc_run(vmem_dagbin_bases[split_id]);
    return;
} // end of flexidag_openseg_ag_run_split()

/* Handle SMB buffers of a particular split DAG */
inline void flexidag_openseg_ag_peek_split(
    flexidag_openseg_ag_required_fields_t *r_args,
    struct flexidag_openseg_ag_split_context *split_ctxt,
    uint32_t split_id
) {
    uint32_t dagbin_base = vmem_dagbin_bases[split_id];
    (void) dagbin_base; // avoid unused variable warning

    switch (split_id) {
        case 0:
            /*** dmast() calls ***/
            break;
        case 1:
            /*** dmast() calls ***/
            break;
        case 2:
            /*** dmast() calls ***/
            break;
        case 3:
            /*** dmast() calls ***/
            break;
        case 4:
            /*** dmast() calls ***/
            break;
        case 5:
            /*** dmast() calls ***/
            break;
        case 6:
            /*** dmast() calls ***/
            break;
        case 7:
            /*** dmast() calls ***/
            break;
        case 8:
            /*** dmast() calls ***/
            break;
        case 9:
            /*** dmast() calls ***/
            break;
        case 10:
            /*** dmast() calls ***/
            break;
        case 11:
            /*** dmast() calls ***/
            break;
        case 12:
            /*** dmast() calls ***/
            break;
        case 13:
            /*** dmast() calls ***/
            break;
        case 14:
            /*** dmast() calls ***/
            break;
        case 15:
            /*** dmast() calls ***/
            break;
        case 16:
            /*** dmast() calls ***/
            break;
        case 17:
            /*** dmast() calls ***/
            break;
        case 18:
            /*** dmast() calls ***/
            break;
        case 19:
            /*** dmast() calls ***/
            break;
        case 20:
            /*** dmast() calls ***/
            break;
        case 21:
            /*** dmast() calls ***/
            break;
        case 22:
            /*** dmast() calls ***/
            break;
        case 23:
            /*** dmast() calls ***/
            break;
        case 24:
            /*** dmast() calls ***/
            break;
        case 25:
            /*** dmast() calls ***/
            break;
        case 26:
            /*** dmast() calls ***/
            break;
        case 27:
            /*** dmast() calls ***/
            break;
        case 28:
            /*** dmast() calls ***/
            break;
        case 29:
            /*** dmast() calls ***/
            break;
        case 30:
            /*** dmast() calls ***/
            break;
        case 31:
            /*** dmast() calls ***/
            break;
        case 32:
            /*** dmast() calls ***/
            break;
        case 33:
            /*** dmast() calls ***/
            break;
        case 34:
            /*** dmast() calls ***/
            break;
        case 35:
            /*** dmast() calls ***/
            break;
        case 36:
            /*** dmast() calls ***/
            break;
        case 37:
            /*** dmast() calls ***/
            break;
        case 38:
            /*** dmast() calls ***/
            break;
        case 39:
            /*** dmast() calls ***/
            break;
        case 40:
            /*** dmast() calls ***/
            break;
        case 41:
            /*** dmast() calls ***/
            break;
        case 42:
            /*** dmast() calls ***/
            break;
        case 43:
            /*** dmast() calls ***/
            break;
        case 44:
            /*** dmast() calls ***/
            break;
        case 45:
            /*** dmast() calls ***/
            break;
        case 46:
            /*** dmast() calls ***/
            break;
        case 47:
            /*** dmast() calls ***/
            break;
        case 48:
            /*** dmast() calls ***/
            break;
        case 49:
            /*** dmast() calls ***/
            break;
        case 50:
            /*** dmast() calls ***/
            break;
        case 51:
            /*** dmast() calls ***/
            break;
        case 52:
            /*** dmast() calls ***/
            break;
        case 53:
            /*** dmast() calls ***/
            break;
        case 54:
            /*** dmast() calls ***/
            break;
        case 55:
            /*** dmast() calls ***/
            break;
        case 56:
            /*** dmast() calls ***/
            break;
        case 57:
            /*** dmast() calls ***/
            break;
        case 58:
            /*** dmast() calls ***/
            break;
        case 59:
            /*** dmast() calls ***/
            break;
        case 60:
            /*** dmast() calls ***/
            break;
        case 61:
            /*** dmast() calls ***/
            break;
        case 62:
            /*** dmast() calls ***/
            break;
        case 63:
            /*** dmast() calls ***/
            break;
        case 64:
            /*** dmast() calls ***/
            break;
        case 65:
            /*** dmast() calls ***/
            break;
        case 66:
            /*** dmast() calls ***/
            break;
        case 67:
            /*** dmast() calls ***/
            break;
        case 68:
            /*** dmast() calls ***/
            break;
        case 69:
            /*** dmast() calls ***/
            break;
        default:
            break;
    }

    coproc_peek_word(0U, 0x4U); // 0x4 is "status_register" in VMEM
    return;
} // end of flexidag_openseg_ag_peek_split()

/****************** USED BY FLEXIDAG_OPENSEG_AG_PEEK_SPLIT() *******************/
/* Check the health of VP and handle SMB buffers of a particular split DAG */
inline errcode_enum_t flexidag_openseg_ag_vp_status()
{
    // Read status register
    uint32_t status_register = get_peek_value_imm(0U);
#ifdef CVTASK_PRINT
    cvtask_printf(LVL_DEBUG, "   > VP status register is %X.",
        status_register, 0, 0, 0, 0);
#endif

    // Single out relevant bits
    uint32_t parse_err_flag       = (status_register & 0x80000000U) >> 31;
    uint32_t runtime_error        = (status_register & 0x40000000U) >> 30;
    uint32_t nan_flag             = (status_register & 0x20000000U) >> 29;
    uint32_t overflow_flag        = (status_register & 0x10000000U) >> 28;
#ifdef CVTASK_PRINT
    uint32_t first_op_runtime_id  = (status_register &   0xFF0000U) >> 16;
    uint32_t first_op_nan_id      = (status_register &     0xFF00U) >>  8;
    uint32_t first_op_overflow_id =  status_register &       0xFFU       ;
#endif
    uint32_t parse_error_code     = (status_register &     0xFF00U) >>  8;
    uint32_t parse_id             =  status_register &       0xFFU       ;

    errcode_enum_t status = ERRCODE_NONE;
    if (parse_err_flag) {
        cvtask_printf(LVL_CRITICAL, "  > Error: error #%d encountered at "
            "operator %d.", parse_error_code, parse_id, 0, 0, 0);
        return ERRCODE_GENERIC;
    }
    if (nan_flag) {
#ifdef CVTASK_PRINT
        cvtask_printf(LVL_CRITICAL, "  > Warning: NaN error encountered at "
            "operator %d.", first_op_nan_id, 0, 0, 0, 0);
#endif
        status |= ERRCODE_NONE;
    }
    if (overflow_flag) {
#ifdef CVTASK_PRINT
        cvtask_printf(LVL_CRITICAL, "  > Warning: overflow error encountered "
            "at operator %d.", first_op_overflow_id, 0, 0, 0, 0);
#endif
        status |= ERRCODE_NONE;
    }
    if (runtime_error) {
#ifdef CVTASK_PRINT
        cvtask_printf(LVL_CRITICAL, "  > Warning: runtime error encountered "
            "at operator %d.", first_op_runtime_id, 0, 0, 0, 0);
#endif
        status |= ERRCODE_NONE;
    }

    return status;
} // end of flexidag_openseg_ag_vp_status()

