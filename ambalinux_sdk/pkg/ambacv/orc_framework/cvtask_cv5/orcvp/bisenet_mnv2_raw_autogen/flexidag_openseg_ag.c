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

#include "flexidag_openseg_ag.h"
#include "flexidag_openseg_ag_private.h"

static const uint32_t dag_img_sizes[] = {
    53868U, 
    124064U, 
    120740U, 
    129780U, 
    152060U, 
    331788U, 
    63496U, 
    232556U, 
    30960U, 
    32776U, 
};

static const uint32_t vmem_dag_bases[] = {
    2024916U, 
    1952928U, 
    1953584U, 
    1939296U, 
    1918944U, 
    1633184U, 
    2011756U, 
    1826912U, 
    1150768U, 
    1450700U, 
};

static const uint32_t vmem_dagbin_bases[] = {
    2073332U, 
    2072944U, 
    2071784U, 
    2066996U, 
    2068924U, 
    1962568U, 
    2074568U, 
    2058864U, 
    1181480U, 
    1481748U, 
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

    for (; split_id < FLEXIDAG_OPENSEG_AG_SPLIT_COUNT; split_id++) {
        split_ctxt = &ctxt->split_ctxt[split_id];
        flexidag_openseg_ag_load_split(split_ctxt, split_id);
        flexidag_openseg_ag_poke_split(r_args, o_args, split_ctxt, split_id);
        wait(VIS_W_COPROC); // wait on dummy run in vp_load_dag()

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
    wait(VIS_W_COPROC_DMA);

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

#ifdef CONV_1__CONV2D___BIASADD___BATCHNORM_1__FUSEDBATCHNORM___MULI___10___BATCHNORM_1__FUSEDBATCHNORM___RELU_1_DRAM_PITCH_MODIFIABLE
    o_args->Conv_1__Conv2D___BiasAdd___BatchNorm_1__FusedBatchNorm___muli___10___BatchNorm_1__FusedBatchNorm___Relu_1_dram_pitch_enable = 1U;
    o_args->Conv_1__Conv2D___BiasAdd___BatchNorm_1__FusedBatchNorm___muli___10___BatchNorm_1__FusedBatchNorm___Relu_1_dram_pitch_value = 32U;
#endif
#ifdef CONV_5__CONV2D___BIASADD___BATCHNORM_5__FUSEDBATCHNORM___MULI___156___BATCHNORM_5__FUSEDBATCHNORM___RELU_3_1526__DRAM_PITCH_MODIFIABLE
    o_args->Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___156___BatchNorm_5__FusedBatchNorm___Relu_3_1526__dram_pitch_enable = 1U;
    o_args->Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___156___BatchNorm_5__FusedBatchNorm___Relu_3_1526__dram_pitch_value = 32U;
#endif
#ifdef CONV__CONV2D___BIASADD___BATCHNORM__FUSEDBATCHNORM___MULI___7___BATCHNORM__FUSEDBATCHNORM___PELEENET__STEM_BLOCK__STEM_BLOCK_CONV0__CONV2D___BATCHNORM__FUSEDBATCHNORM_DRAM_PITCH_MODIFIABLE
    o_args->Conv__Conv2D___BiasAdd___BatchNorm__FusedBatchNorm___muli___7___BatchNorm__FusedBatchNorm___PeleeNet__stem_block__stem_block_conv0__Conv2D___BatchNorm__FusedBatchNorm_dram_pitch_enable = 1U;
    o_args->Conv__Conv2D___BiasAdd___BatchNorm__FusedBatchNorm___muli___7___BatchNorm__FusedBatchNorm___PeleeNet__stem_block__stem_block_conv0__Conv2D___BatchNorm__FusedBatchNorm_dram_pitch_value = 32U;
#endif
#ifdef MUL_2_DRAM_PITCH_MODIFIABLE
    o_args->Mul_2_dram_pitch_enable = 1U;
    o_args->Mul_2_dram_pitch_value = 32U;
#endif
#ifdef PELEENET__STAGE_1_TRANSITION_LAYER__TRANSITION_LAYER_AVGPOOL__AVGPOOL_DRAM_PITCH_MODIFIABLE
    o_args->PeleeNet__stage_1_transition_layer__transition_layer_avgpool__AvgPool_dram_pitch_enable = 1U;
    o_args->PeleeNet__stage_1_transition_layer__transition_layer_avgpool__AvgPool_dram_pitch_value = 32U;
#endif
#ifdef PELEENET__STAGE_2_TRANSITION_LAYER__TRANSITION_LAYER_AVGPOOL__AVGPOOL_DRAM_PITCH_MODIFIABLE
    o_args->PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_dram_pitch_enable = 1U;
    o_args->PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_dram_pitch_value = 96U;
#endif
#ifdef PELEENET__STAGE_3_DENSE_BLOCK_1__CONCAT_DRAM_PITCH_MODIFIABLE
    o_args->PeleeNet__stage_3_dense_block_1__concat_dram_pitch_enable = 1U;
    o_args->PeleeNet__stage_3_dense_block_1__concat_dram_pitch_value = 32U;
#endif
#ifdef PELEENET__STAGE_3_DENSE_BLOCK_4__CONCAT_DRAM_PITCH_MODIFIABLE
    o_args->PeleeNet__stage_3_dense_block_4__concat_dram_pitch_enable = 1U;
    o_args->PeleeNet__stage_3_dense_block_4__concat_dram_pitch_value = 32U;
#endif
#ifdef PELEENET__STAGE_3_DENSE_BLOCK_7__CONCAT_DRAM_PITCH_MODIFIABLE
    o_args->PeleeNet__stage_3_dense_block_7__concat_dram_pitch_enable = 1U;
    o_args->PeleeNet__stage_3_dense_block_7__concat_dram_pitch_value = 32U;
#endif
#ifdef PLACEHOLDER_DRAM_PITCH_MODIFIABLE
    o_args->Placeholder_dram_pitch_enable = 1U;
    o_args->Placeholder_dram_pitch_value = 1280U;
#endif
#ifdef PLACEHOLDER_UV_DRAM_PITCH_MODIFIABLE
    o_args->Placeholder_uv_dram_pitch_enable = 1U;
    o_args->Placeholder_uv_dram_pitch_value = 1280U;
#endif
#ifdef RESIZEBILINEAR_DRAM_PITCH_MODIFIABLE
    o_args->ResizeBilinear_dram_pitch_enable = 1U;
    o_args->ResizeBilinear_dram_pitch_value = 32U;
#endif
#ifdef CONCAT_1_1522__DRAM_PITCH_MODIFIABLE
    o_args->concat_1_1522__dram_pitch_enable = 1U;
    o_args->concat_1_1522__dram_pitch_value = 32U;
#endif
#ifdef CONCAT_1_1527__DRAM_PITCH_MODIFIABLE
    o_args->concat_1_1527__dram_pitch_enable = 1U;
    o_args->concat_1_1527__dram_pitch_value = 32U;
#endif
#ifdef CONCAT_DRAM_PITCH_MODIFIABLE
    o_args->concat_dram_pitch_enable = 1U;
    o_args->concat_dram_pitch_value = 32U;
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
    vp_init();

    vp_load_dag(
        split_ctxt->dram_dag_base,
        vmem_dag_bases[split_id],
        dag_img_sizes[split_id],
        0
    );

    //dma_cmem_2_coproc(0x100, FLEXIDAG_OPENSEG_AG_CMEM_BASE, 1024);

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
    uint32_t *var_scalar_word_addr = NULL;
    uint16_t *var_scalar_half_addr = NULL;

    // Avoid unused variable warning
    (void) dag_base;
    (void) var_scalar_word_addr;
    (void) var_scalar_half_addr;

    switch (split_id) {
        case 0:
            /*** poke() calls ***/
            /* Primary inputs */
            // Poke HMB input buffer: Placeholder
            coproc_poke_word(r_args->Placeholder_addr + 0, dagbin_base + 2852U);

            // Poke HMB input buffer: Placeholder_uv
            coproc_poke_word(r_args->Placeholder_uv_addr + 0, dagbin_base + 2892U);

            /* Intermediate buffers */
            // Poke HMB intermediate output buffer: Conv__Conv2D___BiasAdd___BatchNorm__FusedBatchNorm___muli___7___BatchNorm__FusedBatchNorm___PeleeNet__stem_block__stem_block_conv0__Conv2D___BatchNorm__FusedBatchNorm
            coproc_poke_word(dag_base + 0,
                dagbin_base + 3288U);

            // Poke HMB intermediate output buffer: PeleeNet__stage_1_transition_layer__transition_layer_avgpool__AvgPool
            coproc_poke_word(dag_base + 15728640,
                dagbin_base + 4684U);

            /* Pokable fields */
#ifdef PLACEHOLDER_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: Placeholder_dram_pitch
            if (o_args->Placeholder_dram_pitch_enable)
                coproc_poke_half(o_args->Placeholder_dram_pitch_value, dagbin_base + 2856U);

#endif

#ifdef PLACEHOLDER_UV_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: Placeholder_uv_dram_pitch
            if (o_args->Placeholder_uv_dram_pitch_enable)
                coproc_poke_half(o_args->Placeholder_uv_dram_pitch_value, dagbin_base + 2896U);

#endif

#ifdef CONV__CONV2D___BIASADD___BATCHNORM__FUSEDBATCHNORM___MULI___7___BATCHNORM__FUSEDBATCHNORM___PELEENET__STEM_BLOCK__STEM_BLOCK_CONV0__CONV2D___BATCHNORM__FUSEDBATCHNORM_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: Conv__Conv2D___BiasAdd___BatchNorm__FusedBatchNorm___muli___7___BatchNorm__FusedBatchNorm___PeleeNet__stem_block__stem_block_conv0__Conv2D___BatchNorm__FusedBatchNorm_dram_pitch
            if (o_args->Conv__Conv2D___BiasAdd___BatchNorm__FusedBatchNorm___muli___7___BatchNorm__FusedBatchNorm___PeleeNet__stem_block__stem_block_conv0__Conv2D___BatchNorm__FusedBatchNorm_dram_pitch_enable)
                coproc_poke_half(o_args->Conv__Conv2D___BiasAdd___BatchNorm__FusedBatchNorm___muli___7___BatchNorm__FusedBatchNorm___PeleeNet__stem_block__stem_block_conv0__Conv2D___BatchNorm__FusedBatchNorm_dram_pitch_value, dagbin_base + 3292U);

#endif

#ifdef PELEENET__STAGE_1_TRANSITION_LAYER__TRANSITION_LAYER_AVGPOOL__AVGPOOL_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: PeleeNet__stage_1_transition_layer__transition_layer_avgpool__AvgPool_dram_pitch
            if (o_args->PeleeNet__stage_1_transition_layer__transition_layer_avgpool__AvgPool_dram_pitch_enable)
                coproc_poke_half(o_args->PeleeNet__stage_1_transition_layer__transition_layer_avgpool__AvgPool_dram_pitch_value, dagbin_base + 4688U);

#endif

            /*** dmald() calls ***/
            break;
        case 1:
            /*** poke() calls ***/
            /* Intermediate buffers */
            // Poke HMB intermediate input buffer: PeleeNet__stage_1_transition_layer__transition_layer_avgpool__AvgPool
            coproc_poke_word_offset(dag_base + 15728640,
                dagbin_base, 28U);

            // Poke HMB intermediate output buffer: PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool
            coproc_poke_word(dag_base + 17039360,
                dagbin_base + 3672U);

            /* Pokable fields */
#ifdef PELEENET__STAGE_1_TRANSITION_LAYER__TRANSITION_LAYER_AVGPOOL__AVGPOOL_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: PeleeNet__stage_1_transition_layer__transition_layer_avgpool__AvgPool_dram_pitch
            if (o_args->PeleeNet__stage_1_transition_layer__transition_layer_avgpool__AvgPool_dram_pitch_enable)
                coproc_poke_half_offset(o_args->PeleeNet__stage_1_transition_layer__transition_layer_avgpool__AvgPool_dram_pitch_value, dagbin_base, 
                    32U);

#endif

#ifdef PELEENET__STAGE_2_TRANSITION_LAYER__TRANSITION_LAYER_AVGPOOL__AVGPOOL_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_dram_pitch
            if (o_args->PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_dram_pitch_enable)
                coproc_poke_half(o_args->PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_dram_pitch_value, dagbin_base + 3676U);

#endif

            /*** dmald() calls ***/
            break;
        case 2:
            /*** poke() calls ***/
            /* Intermediate buffers */
            // Poke HMB intermediate input buffer: PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool
            coproc_poke_word_offset(dag_base + 17039360,
                dagbin_base, 28U);

            // Poke HMB intermediate output buffer: PeleeNet__stage_3_dense_block_1__concat
            coproc_poke_word(dag_base + 15728640,
                dagbin_base + 2188U);

            // Poke HMB intermediate output buffer: ResizeBilinear
            coproc_poke_word(dag_base + 17825792,
                dagbin_base + 1732U);

            /* Pokable fields */
#ifdef PELEENET__STAGE_2_TRANSITION_LAYER__TRANSITION_LAYER_AVGPOOL__AVGPOOL_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_dram_pitch
            if (o_args->PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_dram_pitch_enable)
                coproc_poke_half_offset(o_args->PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_dram_pitch_value, dagbin_base, 
                    32U);

#endif

#ifdef RESIZEBILINEAR_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: ResizeBilinear_dram_pitch
            if (o_args->ResizeBilinear_dram_pitch_enable)
                coproc_poke_half(o_args->ResizeBilinear_dram_pitch_value, dagbin_base + 1736U);

#endif

#ifdef PELEENET__STAGE_3_DENSE_BLOCK_1__CONCAT_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: PeleeNet__stage_3_dense_block_1__concat_dram_pitch
            if (o_args->PeleeNet__stage_3_dense_block_1__concat_dram_pitch_enable)
                coproc_poke_half(o_args->PeleeNet__stage_3_dense_block_1__concat_dram_pitch_value, dagbin_base + 2192U);

#endif

            /*** dmald() calls ***/
            break;
        case 3:
            /*** poke() calls ***/
            /* Intermediate buffers */
            // Poke HMB intermediate input buffer: PeleeNet__stage_3_dense_block_1__concat
            coproc_poke_word_offset(dag_base + 15728640,
                dagbin_base, 28U);

            // Poke HMB intermediate output buffer: PeleeNet__stage_3_dense_block_4__concat
            coproc_poke_word(dag_base + 16547840,
                dagbin_base + 1728U);

            /* Pokable fields */
#ifdef PELEENET__STAGE_3_DENSE_BLOCK_1__CONCAT_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: PeleeNet__stage_3_dense_block_1__concat_dram_pitch
            if (o_args->PeleeNet__stage_3_dense_block_1__concat_dram_pitch_enable)
                coproc_poke_half_offset(o_args->PeleeNet__stage_3_dense_block_1__concat_dram_pitch_value, dagbin_base, 
                    32U);

#endif

#ifdef PELEENET__STAGE_3_DENSE_BLOCK_4__CONCAT_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: PeleeNet__stage_3_dense_block_4__concat_dram_pitch
            if (o_args->PeleeNet__stage_3_dense_block_4__concat_dram_pitch_enable)
                coproc_poke_half(o_args->PeleeNet__stage_3_dense_block_4__concat_dram_pitch_value, dagbin_base + 1732U);

#endif

            /*** dmald() calls ***/
            break;
        case 4:
            /*** poke() calls ***/
            /* Intermediate buffers */
            // Poke HMB intermediate input buffer: PeleeNet__stage_3_dense_block_4__concat
            coproc_poke_word_offset(dag_base + 16547840,
                dagbin_base, 28U);

            // Poke HMB intermediate output buffer: PeleeNet__stage_3_dense_block_7__concat
            coproc_poke_word(dag_base + 23068672,
                dagbin_base + 1728U);

            /* Pokable fields */
#ifdef PELEENET__STAGE_3_DENSE_BLOCK_4__CONCAT_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: PeleeNet__stage_3_dense_block_4__concat_dram_pitch
            if (o_args->PeleeNet__stage_3_dense_block_4__concat_dram_pitch_enable)
                coproc_poke_half_offset(o_args->PeleeNet__stage_3_dense_block_4__concat_dram_pitch_value, dagbin_base, 
                    32U);

#endif

#ifdef PELEENET__STAGE_3_DENSE_BLOCK_7__CONCAT_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: PeleeNet__stage_3_dense_block_7__concat_dram_pitch
            if (o_args->PeleeNet__stage_3_dense_block_7__concat_dram_pitch_enable)
                coproc_poke_half(o_args->PeleeNet__stage_3_dense_block_7__concat_dram_pitch_value, dagbin_base + 1732U);

#endif

            /*** dmald() calls ***/
            break;
        case 5:
            /*** poke() calls ***/
            /* Intermediate buffers */
            // Poke HMB intermediate input buffer: PeleeNet__stage_3_dense_block_7__concat
            coproc_poke_word_offset(dag_base + 23068672,
                dagbin_base, 28U);

            // Poke HMB intermediate output buffer: Mul_2
            coproc_poke_word(dag_base + 15728640,
                dagbin_base + 2316U);

            /* Pokable fields */
#ifdef PELEENET__STAGE_3_DENSE_BLOCK_7__CONCAT_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: PeleeNet__stage_3_dense_block_7__concat_dram_pitch
            if (o_args->PeleeNet__stage_3_dense_block_7__concat_dram_pitch_enable)
                coproc_poke_half_offset(o_args->PeleeNet__stage_3_dense_block_7__concat_dram_pitch_value, dagbin_base, 
                    32U);

#endif

#ifdef MUL_2_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: Mul_2_dram_pitch
            if (o_args->Mul_2_dram_pitch_enable)
                coproc_poke_half(o_args->Mul_2_dram_pitch_value, dagbin_base + 2320U);

#endif

            /*** dmald() calls ***/
            break;
        case 6:
            /*** poke() calls ***/
            /* Intermediate buffers */
            // Poke HMB intermediate input buffer: Conv__Conv2D___BiasAdd___BatchNorm__FusedBatchNorm___muli___7___BatchNorm__FusedBatchNorm___PeleeNet__stem_block__stem_block_conv0__Conv2D___BatchNorm__FusedBatchNorm
            coproc_poke_word_offset(dag_base + 0,
                dagbin_base, 104U);

            // Poke HMB intermediate input buffer: Mul_2
            coproc_poke_word_offset(dag_base + 15728640,
                dagbin_base, 28U);

            // Poke HMB intermediate input buffer: ResizeBilinear
            coproc_poke_word_offset(dag_base + 17825792,
                dagbin_base, 64U);

            // Poke HMB intermediate output buffer: concat
            coproc_poke_word_offset(dag_base + 23068672,
                dagbin_base, 388U);

            // Poke HMB intermediate output buffer: Conv_1__Conv2D___BiasAdd___BatchNorm_1__FusedBatchNorm___muli___10___BatchNorm_1__FusedBatchNorm___Relu_1
            coproc_poke_word(dag_base + 38797312,
                dagbin_base + 532U);

            /* Pokable fields */
#ifdef MUL_2_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: Mul_2_dram_pitch
            if (o_args->Mul_2_dram_pitch_enable)
                coproc_poke_half_offset(o_args->Mul_2_dram_pitch_value, dagbin_base, 
                    32U);

#endif

#ifdef RESIZEBILINEAR_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: ResizeBilinear_dram_pitch
            if (o_args->ResizeBilinear_dram_pitch_enable)
                coproc_poke_half_offset(o_args->ResizeBilinear_dram_pitch_value, dagbin_base, 
                    68U);

#endif

#ifdef CONV__CONV2D___BIASADD___BATCHNORM__FUSEDBATCHNORM___MULI___7___BATCHNORM__FUSEDBATCHNORM___PELEENET__STEM_BLOCK__STEM_BLOCK_CONV0__CONV2D___BATCHNORM__FUSEDBATCHNORM_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: Conv__Conv2D___BiasAdd___BatchNorm__FusedBatchNorm___muli___7___BatchNorm__FusedBatchNorm___PeleeNet__stem_block__stem_block_conv0__Conv2D___BatchNorm__FusedBatchNorm_dram_pitch
            if (o_args->Conv__Conv2D___BiasAdd___BatchNorm__FusedBatchNorm___muli___7___BatchNorm__FusedBatchNorm___PeleeNet__stem_block__stem_block_conv0__Conv2D___BatchNorm__FusedBatchNorm_dram_pitch_enable)
                coproc_poke_half_offset(o_args->Conv__Conv2D___BiasAdd___BatchNorm__FusedBatchNorm___muli___7___BatchNorm__FusedBatchNorm___PeleeNet__stem_block__stem_block_conv0__Conv2D___BatchNorm__FusedBatchNorm_dram_pitch_value, dagbin_base, 
                    108U);

#endif

#ifdef CONCAT_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: concat_dram_pitch
            if (o_args->concat_dram_pitch_enable)
                coproc_poke_half_offset(o_args->concat_dram_pitch_value, dagbin_base, 
                    392U);

#endif

#ifdef CONV_1__CONV2D___BIASADD___BATCHNORM_1__FUSEDBATCHNORM___MULI___10___BATCHNORM_1__FUSEDBATCHNORM___RELU_1_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: Conv_1__Conv2D___BiasAdd___BatchNorm_1__FusedBatchNorm___muli___10___BatchNorm_1__FusedBatchNorm___Relu_1_dram_pitch
            if (o_args->Conv_1__Conv2D___BiasAdd___BatchNorm_1__FusedBatchNorm___muli___10___BatchNorm_1__FusedBatchNorm___Relu_1_dram_pitch_enable)
                coproc_poke_half(o_args->Conv_1__Conv2D___BiasAdd___BatchNorm_1__FusedBatchNorm___muli___10___BatchNorm_1__FusedBatchNorm___Relu_1_dram_pitch_value, dagbin_base + 536U);

#endif

            /*** dmald() calls ***/
            break;
        case 7:
            /*** poke() calls ***/
            /* Intermediate buffers */
            // Poke HMB intermediate input buffer: concat
            coproc_poke_word_offset(dag_base + 23068672,
                dagbin_base, 28U);

            // Poke HMB intermediate input buffer: Conv_1__Conv2D___BiasAdd___BatchNorm_1__FusedBatchNorm___muli___10___BatchNorm_1__FusedBatchNorm___Relu_1
            coproc_poke_word_offset(dag_base + 38797312,
                dagbin_base, 68U);

            // Poke HMB intermediate output buffer: concat_1_1522_
            coproc_poke_word_offset(dag_base + 0,
                dagbin_base, 464U);

            // Poke HMB intermediate output buffer: concat_1_1527_
            coproc_poke_word(dag_base + 11534336,
                dagbin_base + 516U);

            /* Pokable fields */
#ifdef CONCAT_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: concat_dram_pitch
            if (o_args->concat_dram_pitch_enable)
                coproc_poke_half_offset(o_args->concat_dram_pitch_value, dagbin_base, 
                    32U);

#endif

#ifdef CONV_1__CONV2D___BIASADD___BATCHNORM_1__FUSEDBATCHNORM___MULI___10___BATCHNORM_1__FUSEDBATCHNORM___RELU_1_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: Conv_1__Conv2D___BiasAdd___BatchNorm_1__FusedBatchNorm___muli___10___BatchNorm_1__FusedBatchNorm___Relu_1_dram_pitch
            if (o_args->Conv_1__Conv2D___BiasAdd___BatchNorm_1__FusedBatchNorm___muli___10___BatchNorm_1__FusedBatchNorm___Relu_1_dram_pitch_enable)
                coproc_poke_half_offset(o_args->Conv_1__Conv2D___BiasAdd___BatchNorm_1__FusedBatchNorm___muli___10___BatchNorm_1__FusedBatchNorm___Relu_1_dram_pitch_value, dagbin_base, 
                    72U);

#endif

#ifdef CONCAT_1_1522__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: concat_1_1522__dram_pitch
            if (o_args->concat_1_1522__dram_pitch_enable)
                coproc_poke_half_offset(o_args->concat_1_1522__dram_pitch_value, dagbin_base, 
                    468U);

#endif

#ifdef CONCAT_1_1527__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: concat_1_1527__dram_pitch
            if (o_args->concat_1_1527__dram_pitch_enable)
                coproc_poke_half(o_args->concat_1_1527__dram_pitch_value, dagbin_base + 520U);

#endif

            /*** dmald() calls ***/
            break;
        case 8:
            /*** poke() calls ***/
            /* Intermediate buffers */
            // Poke HMB intermediate input buffer: concat_1_1522_
            coproc_poke_word_offset(dag_base + 0,
                dagbin_base, 28U);

            // Poke HMB intermediate output buffer: Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___156___BatchNorm_5__FusedBatchNorm___Relu_3_1526_
            coproc_poke_word_offset(dag_base + 23068672,
                dagbin_base, 160U);

            /* Pokable fields */
#ifdef CONCAT_1_1522__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: concat_1_1522__dram_pitch
            if (o_args->concat_1_1522__dram_pitch_enable)
                coproc_poke_half_offset(o_args->concat_1_1522__dram_pitch_value, dagbin_base, 
                    32U);

#endif

#ifdef CONV_5__CONV2D___BIASADD___BATCHNORM_5__FUSEDBATCHNORM___MULI___156___BATCHNORM_5__FUSEDBATCHNORM___RELU_3_1526__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___156___BatchNorm_5__FusedBatchNorm___Relu_3_1526__dram_pitch
            if (o_args->Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___156___BatchNorm_5__FusedBatchNorm___Relu_3_1526__dram_pitch_enable)
                coproc_poke_half_offset(o_args->Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___156___BatchNorm_5__FusedBatchNorm___Relu_3_1526__dram_pitch_value, dagbin_base, 
                    164U);

#endif

            /*** dmald() calls ***/
            break;
        case 9:
            /*** poke() calls ***/
            /* Intermediate buffers */
            // Poke HMB intermediate input buffer: concat_1_1527_
            coproc_poke_word_offset(dag_base + 11534336,
                dagbin_base, 68U);

            // Poke HMB intermediate input buffer: Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___156___BatchNorm_5__FusedBatchNorm___Relu_3_1526_
            coproc_poke_word_offset(dag_base + 23068672,
                dagbin_base, 28U);

            /* Primary outputs */
            // Poke HMB output buffer: out_class
            coproc_poke_word(r_args->out_class_addr + 0,
                dagbin_base + 1640U);

            /* Pokable fields */
#ifdef CONV_5__CONV2D___BIASADD___BATCHNORM_5__FUSEDBATCHNORM___MULI___156___BATCHNORM_5__FUSEDBATCHNORM___RELU_3_1526__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___156___BatchNorm_5__FusedBatchNorm___Relu_3_1526__dram_pitch
            if (o_args->Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___156___BatchNorm_5__FusedBatchNorm___Relu_3_1526__dram_pitch_enable)
                coproc_poke_half_offset(o_args->Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___156___BatchNorm_5__FusedBatchNorm___Relu_3_1526__dram_pitch_value, dagbin_base, 
                    32U);

#endif

#ifdef CONCAT_1_1527__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: concat_1_1527__dram_pitch
            if (o_args->concat_1_1527__dram_pitch_enable)
                coproc_poke_half_offset(o_args->concat_1_1527__dram_pitch_value, dagbin_base, 
                    72U);

#endif

#ifdef OUT_CLASS_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: out_class_dram_pitch
            if (o_args->out_class_dram_pitch_enable)
                coproc_poke_half(o_args->out_class_dram_pitch_value, dagbin_base + 1644U);

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
inline errcode_enum_t flexidag_openseg_ag_peek_split(
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
        default:
            break;
    }

    return ERRCODE_NONE;
} // end of flexidag_openseg_ag_peek_split()

/****************** USED BY FLEXIDAG_OPENSEG_AG_PEEK_SPLIT() *******************/

