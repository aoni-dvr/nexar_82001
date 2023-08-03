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

#include "openodnet_ag.h"
#include "openodnet_ag_private.h"

static const uint32_t dag_img_sizes[] = {
    68704U,
    253272U,
    367524U,
    654212U,
    587280U,
    1455320U,
    979512U,
    576692U,
};

static const uint32_t vmem_dag_bases[] = {
    2011848U, 
    1826504U, 
    1711584U, 
    1426192U, 
    1487384U, 
    623688U, 
    1081452U, 
    1502696U, 
};

static const uint32_t vmem_dagbin_bases[] = {
    2075872U, 
    2075104U, 
    2075408U, 
    2075804U, 
    2073240U, 
    2076340U, 
    2059764U, 
    2076628U, 
};

/******************************* CORE FUNCTIONS *******************************/
/* Populate structs with constants
 * Calls the wrapper function for cvtable_find()
 */
errcode_enum_t openodnet_ag_init (
    openodnet_ag_ips_t *ctxt
) {
    errcode_enum_t status = ERRCODE_NONE;

    if (!ctxt) {
        status = ERRCODE_BAD_PARAMETER;
        cvtask_printf(LVL_CRITICAL, " >>> ** FATAL! Context pointer is NULL",
            0, 0, 0, 0, 0);
        return status;
    }

    // Find dram_dag_base for each split
    status = openodnet_ag_read_cvtable(ctxt);
    if (is_err(status)) {
        cvtask_printf(LVL_DEBUG, "openodnet_ag_read_cvtable() has failed.", 0, 0
            , 0, 0, 0);
        return status;
    }

    return status;
} // end of openodnet_ag_init()

/* Execute VP DAGs
 * run() is NOT re-entrant with respect to run() calls for other DAGs
 */
errcode_enum_t openodnet_ag_run (
    openodnet_ag_ips_t             *ctxt,
    openodnet_ag_required_fields_t *r_args,
    openodnet_ag_optional_fields_t *o_args
) {
    int split_id = 0;
    errcode_enum_t status = ERRCODE_NONE;
    struct openodnet_ag_split_context *split_ctxt = NULL;
#ifdef CVTASK_PRINT
    uint32_t time_start, time_end;
#endif

    if (!ctxt || !r_args || !o_args) {
        status = ERRCODE_BAD_PARAMETER;
        cvtask_printf(LVL_DEBUG, "  > Error: NULL pointer(s).", 0, 0, 0, 0, 0);
        return status;
    }
    if(!r_args->DRAM_temporary_scratchpad) {
        status = ERRCODE_BAD_PARAMETER;
        cvtask_printf(LVL_DEBUG, "  > Error: NULL pointer(s).", 0, 0, 0, 0, 0);
        return status;
    }

#ifdef CVTASK_PRINT
    time_start = get_cur_time();
#endif

    // Initialize VP
    vp_init();
    // Set preferred DRAM transfer size
    visorc_set_preferred_dram_transfer_size(CVCORE_VP0, 1);

    for (; split_id < OPENODNET_AG_SPLIT_COUNT; split_id++) {
        split_ctxt = &ctxt->split_ctxt[split_id];
        openodnet_ag_load_split(split_ctxt, split_id);
        wait(VIS_W_COPROC); // wait on dummy run

        openodnet_ag_poke_split(r_args, o_args, split_ctxt, split_id);
#ifdef CVTASK_PRINT
        time_end = get_cur_time();
        cvtask_printf(LVL_NORMAL,
            "  >> done loading split-#%03u.     cycles=(%u)",
            split_id, time_end - time_start, 0, 0, 0);
        time_start = time_end;
#endif
        openodnet_ag_run_split(split_id);
        openodnet_ag_peek_split(r_args, split_ctxt, split_id);

        wait(VIS_W_COPROC); // wait on coproc_run()
        status = vp_decode_status(get_peek_value_imm(0U));
        if (is_err(status)) {
            cvtask_printf(LVL_CRITICAL, "   > Error: vp_decode_status(0x%08x) failed "
                "at split %u.\n", get_peek_value_imm(0U), split_id, 0, 0, 0);
            return status;
        }

#ifdef CVTASK_PRINT
        time_end = get_cur_time();
        cvtask_printf(LVL_NORMAL,
            "  >> done executing split-#%03u.   cycles=(%u)",
            split_id, time_end - time_start, 0, 0, 0);
        time_start = time_end;
#endif
        
    }

    return status;
} // end of openodnet_ag_run()

/* Populate struct with default values */
errcode_enum_t openodnet_ag_reset(
    openodnet_ag_optional_fields_t *o_args
) {
    errcode_enum_t status = ERRCODE_NONE;

    if (!o_args) {
        status = ERRCODE_BAD_PARAMETER;
        cvtask_printf(LVL_DEBUG, "  > Error: NULL pointer(s).", 0, 0, 0, 0, 0);
        return status;
    }

#ifdef ELT5____RELU_DRAM_PITCH_MODIFIABLE
    o_args->Elt5____relu_dram_pitch_enable = 1U;
    o_args->Elt5____relu_dram_pitch_value = 31U;
#endif
#ifdef P4____BN_P4_SCALE_MUL___MULI___445____BN_P4_SCALE_MUL____SCALE____RELU___MULI___1229_DRAM_PITCH_MODIFIABLE
    o_args->P4____bn_P4_scale_mul___muli___445____bn_P4_scale_mul____scale____relu___muli___1229_dram_pitch_enable = 1U;
    o_args->P4____bn_P4_scale_mul___muli___445____bn_P4_scale_mul____scale____relu___muli___1229_dram_pitch_value = 31U;
#endif
#ifdef P5____BN_P5_SCALE_MUL___MULI___437____BN_P5_SCALE_MUL____SCALE____RELU___MULI___1228_DRAM_PITCH_MODIFIABLE
    o_args->P5____bn_P5_scale_mul___muli___437____bn_P5_scale_mul____scale____relu___muli___1228_dram_pitch_enable = 1U;
    o_args->P5____bn_P5_scale_mul___muli___437____bn_P5_scale_mul____scale____relu___muli___1228_dram_pitch_value = 31U;
#endif
#ifdef P6____BN_P6_SCALE_MUL___MULI___411____BN_P6_SCALE_MUL____SCALE____RELU___MULI___1227_DRAM_PITCH_MODIFIABLE
    o_args->P6____bn_P6_scale_mul___muli___411____bn_P6_scale_mul____scale____relu___muli___1227_dram_pitch_enable = 1U;
    o_args->P6____bn_P6_scale_mul___muli___411____bn_P6_scale_mul____scale____relu___muli___1227_dram_pitch_value = 31U;
#endif
#ifdef __PVCN_1443__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_1443__dram_pitch_enable = 1U;
    o_args->__pvcn_1443__dram_pitch_value = 31U;
#endif
#ifdef __PVCN_1569__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_1569__dram_pitch_enable = 1U;
    o_args->__pvcn_1569__dram_pitch_value = 31U;
#endif
#ifdef ARM_CONF_FLATTEN_DRAM_PITCH_MODIFIABLE
    o_args->arm_conf_flatten_dram_pitch_enable = 1U;
    o_args->arm_conf_flatten_dram_pitch_value = 326399U;
#endif
#ifdef ARM_LOC_DRAM_PITCH_MODIFIABLE
    o_args->arm_loc_dram_pitch_enable = 1U;
    o_args->arm_loc_dram_pitch_value = 652799U;
#endif
#ifdef DATA_DRAM_PITCH_MODIFIABLE
    o_args->data_dram_pitch_enable = 1U;
    o_args->data_dram_pitch_value = 1023U;
#endif
#ifdef DATA_UV_DRAM_PITCH_MODIFIABLE
    o_args->data_uv_dram_pitch_enable = 1U;
    o_args->data_uv_dram_pitch_value = 1023U;
#endif
#ifdef ODM_CONF_FLATTEN_DRAM_PITCH_MODIFIABLE
    o_args->odm_conf_flatten_dram_pitch_enable = 1U;
    o_args->odm_conf_flatten_dram_pitch_value = 652799U;
#endif
#ifdef ODM_LOC_DRAM_PITCH_MODIFIABLE
    o_args->odm_loc_dram_pitch_enable = 1U;
    o_args->odm_loc_dram_pitch_value = 652799U;
#endif
#ifdef STAGE2_1__CONCAT_DRAM_PITCH_MODIFIABLE
    o_args->stage2_1__concat_dram_pitch_enable = 1U;
    o_args->stage2_1__concat_dram_pitch_value = 31U;
#endif
#ifdef STAGE2_TB_____BN_STAGE2_TB__SCALE_MUL___MULI___138_____BN_STAGE2_TB__SCALE_MUL_____SCALE_____RELU_DRAM_PITCH_MODIFIABLE
    o_args->stage2_tb_____bn_stage2_tb__scale_mul___muli___138_____bn_stage2_tb__scale_mul_____scale_____relu_dram_pitch_enable = 1U;
    o_args->stage2_tb_____bn_stage2_tb__scale_mul___muli___138_____bn_stage2_tb__scale_mul_____scale_____relu_dram_pitch_value = 31U;
#endif
#ifdef STAGE2_TB_MBOX_CONF_FLAT_DRAM_PITCH_MODIFIABLE
    o_args->stage2_tb_mbox_conf_flat_dram_pitch_enable = 1U;
    o_args->stage2_tb_mbox_conf_flat_dram_pitch_value = 31U;
#endif
#ifdef STAGE2_TB_MBOX_LOC___CONV2I___397______CONF___CONV2I___140______MULI___1176___7_DRAM_PITCH_MODIFIABLE
    o_args->stage2_tb_mbox_loc___conv2i___397______conf___conv2i___140______muli___1176___7_dram_pitch_enable = 1U;
    o_args->stage2_tb_mbox_loc___conv2i___397______conf___conv2i___140______muli___1176___7_dram_pitch_value = 31U;
#endif
#ifdef STAGE2_TB_MBOX_LOC_FLAT___MULI___1244_DRAM_PITCH_MODIFIABLE
    o_args->stage2_tb_mbox_loc_flat___muli___1244_dram_pitch_enable = 1U;
    o_args->stage2_tb_mbox_loc_flat___muli___1244_dram_pitch_value = 31U;
#endif
#ifdef STAGE3_2__CONCAT_DRAM_PITCH_MODIFIABLE
    o_args->stage3_2__concat_dram_pitch_enable = 1U;
    o_args->stage3_2__concat_dram_pitch_value = 31U;
#endif
#ifdef STAGE3_8__CONCAT_DRAM_PITCH_MODIFIABLE
    o_args->stage3_8__concat_dram_pitch_enable = 1U;
    o_args->stage3_8__concat_dram_pitch_value = 31U;
#endif
#ifdef STAGE3_TB_____BN_STAGE3_TB__SCALE_MUL___MULI___272_____BN_STAGE3_TB__SCALE_MUL_____SCALE_____RELU_DRAM_PITCH_MODIFIABLE
    o_args->stage3_tb_____bn_stage3_tb__scale_mul___muli___272_____bn_stage3_tb__scale_mul_____scale_____relu_dram_pitch_enable = 1U;
    o_args->stage3_tb_____bn_stage3_tb__scale_mul___muli___272_____bn_stage3_tb__scale_mul_____scale_____relu_dram_pitch_value = 31U;
#endif
#ifdef STAGE3_TB_MBOX_LOC___CONV2I___399______CONF___CONV2I___274______MULI___1202_DRAM_PITCH_MODIFIABLE
    o_args->stage3_tb_mbox_loc___conv2i___399______conf___conv2i___274______muli___1202_dram_pitch_enable = 1U;
    o_args->stage3_tb_mbox_loc___conv2i___399______conf___conv2i___274______muli___1202_dram_pitch_value = 31U;
#endif
#ifdef STAGE3_TB_MBOX_LOC_FLAT___MULI___1245_DRAM_PITCH_MODIFIABLE
    o_args->stage3_tb_mbox_loc_flat___muli___1245_dram_pitch_enable = 1U;
    o_args->stage3_tb_mbox_loc_flat___muli___1245_dram_pitch_value = 31U;
#endif
#ifdef STAGE4_5__BRANCH2C_____BN_STAGE4_5__BRANCH2C__SCALE_MUL___MULI___354_____BN_STAGE4_5__BRANCH2C__SCALE_MUL_____SCALE_____RELU_DRAM_PITCH_MODIFIABLE
    o_args->stage4_5__branch2c_____bn_stage4_5__branch2c__scale_mul___muli___354_____bn_stage4_5__branch2c__scale_mul_____scale_____relu_dram_pitch_enable = 1U;
    o_args->stage4_5__branch2c_____bn_stage4_5__branch2c__scale_mul___muli___354_____bn_stage4_5__branch2c__scale_mul_____scale_____relu_dram_pitch_value = 31U;
#endif
#ifdef STAGE4_TB_____BN_STAGE4_TB__SCALE_MUL___MULI___374_____BN_STAGE4_TB__SCALE_MUL_____SCALE_____RELU_DRAM_PITCH_MODIFIABLE
    o_args->stage4_tb_____bn_stage4_tb__scale_mul___muli___374_____bn_stage4_tb__scale_mul_____scale_____relu_dram_pitch_enable = 1U;
    o_args->stage4_tb_____bn_stage4_tb__scale_mul___muli___374_____bn_stage4_tb__scale_mul_____scale_____relu_dram_pitch_value = 31U;
#endif
#ifdef STAGE4_TB__RELU__EXT1__FE1_1____BN_STAGE4_TB__RELU__EXT1__FE1_1_SCALE_MUL___MULI___379____BN_STAGE4_TB__RELU__EXT1__FE1_1_SCALE_MUL____SCALE____RELU_DRAM_PITCH_MODIFIABLE
    o_args->stage4_tb__relu__ext1__fe1_1____bn_stage4_tb__relu__ext1__fe1_1_scale_mul___muli___379____bn_stage4_tb__relu__ext1__fe1_1_scale_mul____scale____relu_dram_pitch_enable = 1U;
    o_args->stage4_tb__relu__ext1__fe1_1____bn_stage4_tb__relu__ext1__fe1_1_scale_mul___muli___379____bn_stage4_tb__relu__ext1__fe1_1_scale_mul____scale____relu_dram_pitch_value = 31U;
#endif
#ifdef STAGE4_TB_MBOX_LOC___CONV2I___401______CONF___CONV2I___376______MULI___1222_DRAM_PITCH_MODIFIABLE
    o_args->stage4_tb_mbox_loc___conv2i___401______conf___conv2i___376______muli___1222_dram_pitch_enable = 1U;
    o_args->stage4_tb_mbox_loc___conv2i___401______conf___conv2i___376______muli___1222_dram_pitch_value = 31U;
#endif
#ifdef STAGE4_TB_MBOX_LOC_FLAT___MULI___1246_DRAM_PITCH_MODIFIABLE
    o_args->stage4_tb_mbox_loc_flat___muli___1246_dram_pitch_enable = 1U;
    o_args->stage4_tb_mbox_loc_flat___muli___1246_dram_pitch_value = 31U;
#endif

    return status;
} // end of openodnet_ag_reset()


/********************* USED BY OPENODNET_AG_INIT() **********************/
/* Wrapper function for cvtable_find() */
inline errcode_enum_t openodnet_ag_read_cvtable (
    openodnet_ag_ips_t *ctxt
) {
    unsigned int split_id = 0; // size_t
    int32_t tbar_sz;
    errcode_enum_t status = ERRCODE_NONE;
    struct openodnet_ag_split_context *split_ctxt = NULL;
    char dag_key[] = "OPENODNET_AG_---_DAG";

    for (; split_id < OPENODNET_AG_SPLIT_COUNT; split_id++) {
        tbar_sz = 0;
        split_ctxt = &ctxt->split_ctxt[split_id];

#ifdef CVTASK_PRINT
        cvtask_printf(LVL_DEBUG, "  >> #Split-%u# Extracting DAG base address "
            "from CV Table Archive", split_id, 0, 0, 0, 0);
#endif

        dag_key[13] = '0' + ((split_id / 100U) % 10U);
        dag_key[14] = '0' + ((split_id / 10U ) % 10U);
        dag_key[15] = '0' + ((split_id / 1U  ) % 10U);

        // extracting DAG base from CV Table Archive
        status = cvtable_find(
            dag_key,
            (const void **)&split_ctxt->dram_dag_base,
            &tbar_sz
        );
        if (is_err(status))
        {
            cvtask_printf(LVL_DEBUG, "  >> cvtable_find() key OPENODNET_AG_"
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
} // end of openodnet_ag_read_cvtable()


/********************** USED BY OPENODNET_AG_RUN() **********************/
/* Load machine code into VP */
inline void openodnet_ag_load_split(
    struct openodnet_ag_split_context *split_ctxt,
    uint32_t split_id
) {

    uint32_t dag_dbase = split_ctxt->dram_dag_base;
    uint32_t dag_vbase = vmem_dag_bases[split_id];
    uint32_t dag_size  = dag_img_sizes[split_id];

    // vp_load_dag()
    coproc_poke_ldq_word(dag_vbase,     0xFD4U);
    coproc_poke_ldq_word(dag_size - 1U, 0xFE0U);
    coproc_poke_ldq_word(dag_dbase,     0xFE8U);
    coproc_dmald(0xFD0U);
    vp_dummy_run();


    return;
} // end of openodnet_ag_load_split()

/* Modify input and output buffer addresses of a particular split DAG */
inline void openodnet_ag_poke_split(
    openodnet_ag_required_fields_t *r_args,
    openodnet_ag_optional_fields_t *o_args,
    struct openodnet_ag_split_context *split_ctxt,
    uint32_t split_id) {
    uint32_t dag_base = (uint32_t) r_args->DRAM_temporary_scratchpad;
    uint32_t dagbin_base = vmem_dagbin_bases[split_id];

    // Avoid unused variable warning
    (void) dag_base;

    switch (split_id) {
        case 0:
            /*** poke() calls ***/
            /* Primary inputs */
            // Poke HMB input buffer: data
            coproc_poke_word(r_args->data_addr + 0U, dagbin_base + 2404U);

            // Poke HMB input buffer: data_uv
            coproc_poke_word(r_args->data_uv_addr + 0U, dagbin_base + 2452U);

            /* Intermediate buffers */
            // Poke HMB intermediate output buffer: stage2_1__concat
            coproc_poke_word(dag_base + 0U,
                dagbin_base + 2920U);

            /* Pokable fields */
#ifdef DATA_DRAM_PITCH_MODIFIABLE
            // Poke 32-bit variable: data_dram_pitch
            if (o_args->data_dram_pitch_enable)
                coproc_poke_word(o_args->data_dram_pitch_value, dagbin_base + 2408U);

#endif

#ifdef DATA_UV_DRAM_PITCH_MODIFIABLE
            // Poke 32-bit variable: data_uv_dram_pitch
            if (o_args->data_uv_dram_pitch_enable)
                coproc_poke_word(o_args->data_uv_dram_pitch_value, dagbin_base + 2456U);

#endif

#ifdef STAGE2_1__CONCAT_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: stage2_1__concat_dram_pitch
            if (o_args->stage2_1__concat_dram_pitch_enable)
                coproc_poke_half(o_args->stage2_1__concat_dram_pitch_value, dagbin_base + 2924U);

#endif

            /*** dmald() calls ***/
            break;
        case 1:
            /*** poke() calls ***/
            /* Intermediate buffers */
            // Poke HMB intermediate input buffer: stage2_1__concat
            coproc_poke_word_offset(dag_base + 0U,
                dagbin_base, 28U);

            // Poke HMB intermediate output buffer: stage2_tb_____bn_stage2_tb__scale_mul___muli___138_____bn_stage2_tb__scale_mul_____scale_____relu
            coproc_poke_word(dag_base + 1638400U,
                dagbin_base + 3488U);

            // Poke HMB intermediate output buffer: stage2_tb_mbox_loc___conv2i___397______conf___conv2i___140______muli___1176___7
            coproc_poke_word(dag_base + 4259840U,
                dagbin_base + 3552U);

            // Poke HMB intermediate output buffer: stage2_tb_mbox_conf_flat
            coproc_poke_word(dag_base + 4444160U,
                dagbin_base + 3616U);

            // Poke HMB intermediate output buffer: stage3_2__concat
            coproc_poke_word(dag_base + 4505600U,
                dagbin_base + 4584U);

            /* Pokable fields */
#ifdef STAGE2_1__CONCAT_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: stage2_1__concat_dram_pitch
            if (o_args->stage2_1__concat_dram_pitch_enable)
                coproc_poke_half_offset(o_args->stage2_1__concat_dram_pitch_value, dagbin_base, 
                    32U);

#endif

#ifdef STAGE2_TB_____BN_STAGE2_TB__SCALE_MUL___MULI___138_____BN_STAGE2_TB__SCALE_MUL_____SCALE_____RELU_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: stage2_tb_____bn_stage2_tb__scale_mul___muli___138_____bn_stage2_tb__scale_mul_____scale_____relu_dram_pitch
            if (o_args->stage2_tb_____bn_stage2_tb__scale_mul___muli___138_____bn_stage2_tb__scale_mul_____scale_____relu_dram_pitch_enable)
                coproc_poke_half(o_args->stage2_tb_____bn_stage2_tb__scale_mul___muli___138_____bn_stage2_tb__scale_mul_____scale_____relu_dram_pitch_value, dagbin_base + 3492U);

#endif

#ifdef STAGE2_TB_MBOX_LOC___CONV2I___397______CONF___CONV2I___140______MULI___1176___7_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: stage2_tb_mbox_loc___conv2i___397______conf___conv2i___140______muli___1176___7_dram_pitch
            if (o_args->stage2_tb_mbox_loc___conv2i___397______conf___conv2i___140______muli___1176___7_dram_pitch_enable)
                coproc_poke_half(o_args->stage2_tb_mbox_loc___conv2i___397______conf___conv2i___140______muli___1176___7_dram_pitch_value, dagbin_base + 3556U);

#endif

#ifdef STAGE2_TB_MBOX_CONF_FLAT_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: stage2_tb_mbox_conf_flat_dram_pitch
            if (o_args->stage2_tb_mbox_conf_flat_dram_pitch_enable)
                coproc_poke_half(o_args->stage2_tb_mbox_conf_flat_dram_pitch_value, dagbin_base + 3620U);

#endif

#ifdef STAGE3_2__CONCAT_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: stage3_2__concat_dram_pitch
            if (o_args->stage3_2__concat_dram_pitch_enable)
                coproc_poke_half(o_args->stage3_2__concat_dram_pitch_value, dagbin_base + 4588U);

#endif

            /*** dmald() calls ***/
            break;
        case 2:
            /*** poke() calls ***/
            /* Intermediate buffers */
            // Poke HMB intermediate input buffer: stage3_2__concat
            coproc_poke_word_offset(dag_base + 4505600U,
                dagbin_base, 28U);

            // Poke HMB intermediate output buffer: stage3_8__concat
            coproc_poke_word(dag_base + 0U,
                dagbin_base + 3436U);

            /* Pokable fields */
#ifdef STAGE3_2__CONCAT_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: stage3_2__concat_dram_pitch
            if (o_args->stage3_2__concat_dram_pitch_enable)
                coproc_poke_half_offset(o_args->stage3_2__concat_dram_pitch_value, dagbin_base, 
                    32U);

#endif

#ifdef STAGE3_8__CONCAT_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: stage3_8__concat_dram_pitch
            if (o_args->stage3_8__concat_dram_pitch_enable)
                coproc_poke_half(o_args->stage3_8__concat_dram_pitch_value, dagbin_base + 3440U);

#endif

            /*** dmald() calls ***/
            break;
        case 3:
            /*** poke() calls ***/
            /* Intermediate buffers */
            // Poke HMB intermediate input buffer: stage3_8__concat
            coproc_poke_word_offset(dag_base + 0U,
                dagbin_base, 28U);

            // Poke HMB intermediate input buffer: stage2_tb_mbox_loc___conv2i___397______conf___conv2i___140______muli___1176___7
            coproc_poke_word_offset(dag_base + 4259840U,
                dagbin_base, 64U);

            // Poke HMB intermediate output buffer: stage3_tb_mbox_loc___conv2i___399______conf___conv2i___274______muli___1202
            coproc_poke_word(dag_base + 1310720U,
                dagbin_base + 2652U);

            // Poke HMB intermediate output buffer: stage3_tb_mbox_loc_flat___muli___1245
            coproc_poke_word(dag_base + 1356800U,
                dagbin_base + 2744U);

            // Poke HMB intermediate output buffer: stage4_5__branch2c_____bn_stage4_5__branch2c__scale_mul___muli___354_____bn_stage4_5__branch2c__scale_mul_____scale_____relu
            coproc_poke_word(dag_base + 1479680U,
                dagbin_base + 4412U);

            // Poke HMB intermediate output buffer: stage3_tb_____bn_stage3_tb__scale_mul___muli___272_____bn_stage3_tb__scale_mul_____scale_____relu
            coproc_poke_word(dag_base + 4505600U,
                dagbin_base + 2588U);

            // Poke HMB intermediate output buffer: stage2_tb_mbox_loc_flat___muli___1244
            coproc_poke_word(dag_base + 5816320U,
                dagbin_base + 2840U);

            // Poke HMB intermediate output buffer: __pvcn_1443_
            coproc_poke_word(dag_base + 6307840U,
                dagbin_base + 4512U);

            /* Pokable fields */
#ifdef STAGE3_8__CONCAT_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: stage3_8__concat_dram_pitch
            if (o_args->stage3_8__concat_dram_pitch_enable)
                coproc_poke_half_offset(o_args->stage3_8__concat_dram_pitch_value, dagbin_base, 
                    32U);

#endif

#ifdef STAGE2_TB_MBOX_LOC___CONV2I___397______CONF___CONV2I___140______MULI___1176___7_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: stage2_tb_mbox_loc___conv2i___397______conf___conv2i___140______muli___1176___7_dram_pitch
            if (o_args->stage2_tb_mbox_loc___conv2i___397______conf___conv2i___140______muli___1176___7_dram_pitch_enable)
                coproc_poke_half_offset(o_args->stage2_tb_mbox_loc___conv2i___397______conf___conv2i___140______muli___1176___7_dram_pitch_value, dagbin_base, 
                    68U);

#endif

#ifdef STAGE3_TB_____BN_STAGE3_TB__SCALE_MUL___MULI___272_____BN_STAGE3_TB__SCALE_MUL_____SCALE_____RELU_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: stage3_tb_____bn_stage3_tb__scale_mul___muli___272_____bn_stage3_tb__scale_mul_____scale_____relu_dram_pitch
            if (o_args->stage3_tb_____bn_stage3_tb__scale_mul___muli___272_____bn_stage3_tb__scale_mul_____scale_____relu_dram_pitch_enable)
                coproc_poke_half(o_args->stage3_tb_____bn_stage3_tb__scale_mul___muli___272_____bn_stage3_tb__scale_mul_____scale_____relu_dram_pitch_value, dagbin_base + 2592U);

#endif

#ifdef STAGE3_TB_MBOX_LOC___CONV2I___399______CONF___CONV2I___274______MULI___1202_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: stage3_tb_mbox_loc___conv2i___399______conf___conv2i___274______muli___1202_dram_pitch
            if (o_args->stage3_tb_mbox_loc___conv2i___399______conf___conv2i___274______muli___1202_dram_pitch_enable)
                coproc_poke_half(o_args->stage3_tb_mbox_loc___conv2i___399______conf___conv2i___274______muli___1202_dram_pitch_value, dagbin_base + 2656U);

#endif

#ifdef STAGE3_TB_MBOX_LOC_FLAT___MULI___1245_DRAM_PITCH_MODIFIABLE
            // Poke 32-bit variable: stage3_tb_mbox_loc_flat___muli___1245_dram_pitch
            if (o_args->stage3_tb_mbox_loc_flat___muli___1245_dram_pitch_enable)
                coproc_poke_word(o_args->stage3_tb_mbox_loc_flat___muli___1245_dram_pitch_value, dagbin_base + 2748U);

#endif

#ifdef STAGE2_TB_MBOX_LOC_FLAT___MULI___1244_DRAM_PITCH_MODIFIABLE
            // Poke 32-bit variable: stage2_tb_mbox_loc_flat___muli___1244_dram_pitch
            if (o_args->stage2_tb_mbox_loc_flat___muli___1244_dram_pitch_enable)
                coproc_poke_word(o_args->stage2_tb_mbox_loc_flat___muli___1244_dram_pitch_value, dagbin_base + 2844U);

#endif

#ifdef STAGE4_5__BRANCH2C_____BN_STAGE4_5__BRANCH2C__SCALE_MUL___MULI___354_____BN_STAGE4_5__BRANCH2C__SCALE_MUL_____SCALE_____RELU_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: stage4_5__branch2c_____bn_stage4_5__branch2c__scale_mul___muli___354_____bn_stage4_5__branch2c__scale_mul_____scale_____relu_dram_pitch
            if (o_args->stage4_5__branch2c_____bn_stage4_5__branch2c__scale_mul___muli___354_____bn_stage4_5__branch2c__scale_mul_____scale_____relu_dram_pitch_enable)
                coproc_poke_half(o_args->stage4_5__branch2c_____bn_stage4_5__branch2c__scale_mul___muli___354_____bn_stage4_5__branch2c__scale_mul_____scale_____relu_dram_pitch_value, dagbin_base + 4416U);

#endif

#ifdef __PVCN_1443__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_1443__dram_pitch
            if (o_args->__pvcn_1443__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_1443__dram_pitch_value, dagbin_base + 4516U);

#endif

            /*** dmald() calls ***/
            break;
        case 4:
            /*** poke() calls ***/
            /* Intermediate buffers */
            // Poke HMB intermediate input buffer: stage4_5__branch2c_____bn_stage4_5__branch2c__scale_mul___muli___354_____bn_stage4_5__branch2c__scale_mul_____scale_____relu
            coproc_poke_word_offset(dag_base + 1479680U,
                dagbin_base, 28U);

            // Poke HMB intermediate input buffer: __pvcn_1443_
            coproc_poke_word_offset(dag_base + 6307840U,
                dagbin_base, 64U);

            // Poke HMB intermediate output buffer: stage4_tb_____bn_stage4_tb__scale_mul___muli___374_____bn_stage4_tb__scale_mul_____scale_____relu
            coproc_poke_word(dag_base + 0U,
                dagbin_base + 1136U);

            // Poke HMB intermediate output buffer: stage4_tb_mbox_loc___conv2i___401______conf___conv2i___376______muli___1222
            coproc_poke_word(dag_base + 450560U,
                dagbin_base + 1192U);

            // Poke HMB intermediate output buffer: stage4_tb_mbox_loc_flat___muli___1246
            coproc_poke_word(dag_base + 462080U,
                dagbin_base + 1284U);

            // Poke HMB intermediate output buffer: stage4_tb__relu__ext1__fe1_1____bn_stage4_tb__relu__ext1__fe1_1_scale_mul___muli___379____bn_stage4_tb__relu__ext1__fe1_1_scale_mul____scale____relu
            coproc_poke_word(dag_base + 492800U,
                dagbin_base + 1336U);

            /* Pokable fields */
#ifdef STAGE4_5__BRANCH2C_____BN_STAGE4_5__BRANCH2C__SCALE_MUL___MULI___354_____BN_STAGE4_5__BRANCH2C__SCALE_MUL_____SCALE_____RELU_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: stage4_5__branch2c_____bn_stage4_5__branch2c__scale_mul___muli___354_____bn_stage4_5__branch2c__scale_mul_____scale_____relu_dram_pitch
            if (o_args->stage4_5__branch2c_____bn_stage4_5__branch2c__scale_mul___muli___354_____bn_stage4_5__branch2c__scale_mul_____scale_____relu_dram_pitch_enable)
                coproc_poke_half_offset(o_args->stage4_5__branch2c_____bn_stage4_5__branch2c__scale_mul___muli___354_____bn_stage4_5__branch2c__scale_mul_____scale_____relu_dram_pitch_value, dagbin_base, 
                    32U);

#endif

#ifdef __PVCN_1443__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_1443__dram_pitch
            if (o_args->__pvcn_1443__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_1443__dram_pitch_value, dagbin_base, 
                    68U);

#endif

#ifdef STAGE4_TB_____BN_STAGE4_TB__SCALE_MUL___MULI___374_____BN_STAGE4_TB__SCALE_MUL_____SCALE_____RELU_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: stage4_tb_____bn_stage4_tb__scale_mul___muli___374_____bn_stage4_tb__scale_mul_____scale_____relu_dram_pitch
            if (o_args->stage4_tb_____bn_stage4_tb__scale_mul___muli___374_____bn_stage4_tb__scale_mul_____scale_____relu_dram_pitch_enable)
                coproc_poke_half(o_args->stage4_tb_____bn_stage4_tb__scale_mul___muli___374_____bn_stage4_tb__scale_mul_____scale_____relu_dram_pitch_value, dagbin_base + 1140U);

#endif

#ifdef STAGE4_TB_MBOX_LOC___CONV2I___401______CONF___CONV2I___376______MULI___1222_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: stage4_tb_mbox_loc___conv2i___401______conf___conv2i___376______muli___1222_dram_pitch
            if (o_args->stage4_tb_mbox_loc___conv2i___401______conf___conv2i___376______muli___1222_dram_pitch_enable)
                coproc_poke_half(o_args->stage4_tb_mbox_loc___conv2i___401______conf___conv2i___376______muli___1222_dram_pitch_value, dagbin_base + 1196U);

#endif

#ifdef STAGE4_TB_MBOX_LOC_FLAT___MULI___1246_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: stage4_tb_mbox_loc_flat___muli___1246_dram_pitch
            if (o_args->stage4_tb_mbox_loc_flat___muli___1246_dram_pitch_enable)
                coproc_poke_half(o_args->stage4_tb_mbox_loc_flat___muli___1246_dram_pitch_value, dagbin_base + 1288U);

#endif

#ifdef STAGE4_TB__RELU__EXT1__FE1_1____BN_STAGE4_TB__RELU__EXT1__FE1_1_SCALE_MUL___MULI___379____BN_STAGE4_TB__RELU__EXT1__FE1_1_SCALE_MUL____SCALE____RELU_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: stage4_tb__relu__ext1__fe1_1____bn_stage4_tb__relu__ext1__fe1_1_scale_mul___muli___379____bn_stage4_tb__relu__ext1__fe1_1_scale_mul____scale____relu_dram_pitch
            if (o_args->stage4_tb__relu__ext1__fe1_1____bn_stage4_tb__relu__ext1__fe1_1_scale_mul___muli___379____bn_stage4_tb__relu__ext1__fe1_1_scale_mul____scale____relu_dram_pitch_enable)
                coproc_poke_half(o_args->stage4_tb__relu__ext1__fe1_1____bn_stage4_tb__relu__ext1__fe1_1_scale_mul___muli___379____bn_stage4_tb__relu__ext1__fe1_1_scale_mul____scale____relu_dram_pitch_value, dagbin_base + 1340U);

#endif

            /*** dmald() calls ***/
            break;
        case 5:
            /*** poke() calls ***/
            /* Intermediate buffers */
            // Poke HMB intermediate input buffer: stage4_tb_____bn_stage4_tb__scale_mul___muli___374_____bn_stage4_tb__scale_mul_____scale_____relu
            coproc_poke_word_offset(dag_base + 0U,
                dagbin_base, 28U);

            // Poke HMB intermediate input buffer: stage4_tb_mbox_loc___conv2i___401______conf___conv2i___376______muli___1222
            coproc_poke_word_offset(dag_base + 450560U,
                dagbin_base, 68U);

            // Poke HMB intermediate input buffer: stage4_tb_mbox_loc_flat___muli___1246
            coproc_poke_word_offset(dag_base + 462080U,
                dagbin_base, 104U);

            // Poke HMB intermediate input buffer: stage4_tb__relu__ext1__fe1_1____bn_stage4_tb__relu__ext1__fe1_1_scale_mul___muli___379____bn_stage4_tb__relu__ext1__fe1_1_scale_mul____scale____relu
            coproc_poke_word_offset(dag_base + 492800U,
                dagbin_base, 144U);

            // Poke HMB intermediate input buffer: stage3_tb_mbox_loc___conv2i___399______conf___conv2i___274______muli___1202
            coproc_poke_word_offset(dag_base + 1310720U,
                dagbin_base, 180U);

            // Poke HMB intermediate input buffer: stage3_tb_mbox_loc_flat___muli___1245
            coproc_poke_word_offset(dag_base + 1356800U,
                dagbin_base, 216U);

            // Poke HMB intermediate input buffer: stage2_tb_mbox_conf_flat
            coproc_poke_word_offset(dag_base + 4444160U,
                dagbin_base, 296U);

            // Poke HMB intermediate input buffer: stage2_tb_mbox_loc_flat___muli___1244
            coproc_poke_word_offset(dag_base + 5816320U,
                dagbin_base, 256U);

            // Poke HMB intermediate output buffer: P6____bn_P6_scale_mul___muli___411____bn_P6_scale_mul____scale____relu___muli___1227
            coproc_poke_word(dag_base + 644480U,
                dagbin_base + 2300U);

            // Poke HMB intermediate output buffer: Elt5____relu
            coproc_poke_word(dag_base + 664960U,
                dagbin_base + 2580U);

            /* Primary outputs */
            // Poke HMB output buffer: arm_loc
            coproc_poke_word(r_args->arm_loc_addr + 0U,
                dagbin_base + 1388U);

            // Poke HMB output buffer: arm_conf_flatten
            coproc_poke_word(r_args->arm_conf_flatten_addr + 0U,
                dagbin_base + 2156U);

            /* Pokable fields */
#ifdef STAGE4_TB_____BN_STAGE4_TB__SCALE_MUL___MULI___374_____BN_STAGE4_TB__SCALE_MUL_____SCALE_____RELU_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: stage4_tb_____bn_stage4_tb__scale_mul___muli___374_____bn_stage4_tb__scale_mul_____scale_____relu_dram_pitch
            if (o_args->stage4_tb_____bn_stage4_tb__scale_mul___muli___374_____bn_stage4_tb__scale_mul_____scale_____relu_dram_pitch_enable)
                coproc_poke_half_offset(o_args->stage4_tb_____bn_stage4_tb__scale_mul___muli___374_____bn_stage4_tb__scale_mul_____scale_____relu_dram_pitch_value, dagbin_base, 
                    32U);

#endif

#ifdef STAGE4_TB_MBOX_LOC___CONV2I___401______CONF___CONV2I___376______MULI___1222_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: stage4_tb_mbox_loc___conv2i___401______conf___conv2i___376______muli___1222_dram_pitch
            if (o_args->stage4_tb_mbox_loc___conv2i___401______conf___conv2i___376______muli___1222_dram_pitch_enable)
                coproc_poke_half_offset(o_args->stage4_tb_mbox_loc___conv2i___401______conf___conv2i___376______muli___1222_dram_pitch_value, dagbin_base, 
                    72U);

#endif

#ifdef STAGE4_TB_MBOX_LOC_FLAT___MULI___1246_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: stage4_tb_mbox_loc_flat___muli___1246_dram_pitch
            if (o_args->stage4_tb_mbox_loc_flat___muli___1246_dram_pitch_enable)
                coproc_poke_half_offset(o_args->stage4_tb_mbox_loc_flat___muli___1246_dram_pitch_value, dagbin_base, 
                    108U);

#endif

#ifdef STAGE4_TB__RELU__EXT1__FE1_1____BN_STAGE4_TB__RELU__EXT1__FE1_1_SCALE_MUL___MULI___379____BN_STAGE4_TB__RELU__EXT1__FE1_1_SCALE_MUL____SCALE____RELU_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: stage4_tb__relu__ext1__fe1_1____bn_stage4_tb__relu__ext1__fe1_1_scale_mul___muli___379____bn_stage4_tb__relu__ext1__fe1_1_scale_mul____scale____relu_dram_pitch
            if (o_args->stage4_tb__relu__ext1__fe1_1____bn_stage4_tb__relu__ext1__fe1_1_scale_mul___muli___379____bn_stage4_tb__relu__ext1__fe1_1_scale_mul____scale____relu_dram_pitch_enable)
                coproc_poke_half_offset(o_args->stage4_tb__relu__ext1__fe1_1____bn_stage4_tb__relu__ext1__fe1_1_scale_mul___muli___379____bn_stage4_tb__relu__ext1__fe1_1_scale_mul____scale____relu_dram_pitch_value, dagbin_base, 
                    148U);

#endif

#ifdef STAGE3_TB_MBOX_LOC___CONV2I___399______CONF___CONV2I___274______MULI___1202_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: stage3_tb_mbox_loc___conv2i___399______conf___conv2i___274______muli___1202_dram_pitch
            if (o_args->stage3_tb_mbox_loc___conv2i___399______conf___conv2i___274______muli___1202_dram_pitch_enable)
                coproc_poke_half_offset(o_args->stage3_tb_mbox_loc___conv2i___399______conf___conv2i___274______muli___1202_dram_pitch_value, dagbin_base, 
                    184U);

#endif

#ifdef STAGE3_TB_MBOX_LOC_FLAT___MULI___1245_DRAM_PITCH_MODIFIABLE
            // Poke 32-bit variable: stage3_tb_mbox_loc_flat___muli___1245_dram_pitch
            if (o_args->stage3_tb_mbox_loc_flat___muli___1245_dram_pitch_enable)
                coproc_poke_word_offset(o_args->stage3_tb_mbox_loc_flat___muli___1245_dram_pitch_value, dagbin_base, 
                    220U);

#endif

#ifdef STAGE2_TB_MBOX_LOC_FLAT___MULI___1244_DRAM_PITCH_MODIFIABLE
            // Poke 32-bit variable: stage2_tb_mbox_loc_flat___muli___1244_dram_pitch
            if (o_args->stage2_tb_mbox_loc_flat___muli___1244_dram_pitch_enable)
                coproc_poke_word_offset(o_args->stage2_tb_mbox_loc_flat___muli___1244_dram_pitch_value, dagbin_base, 
                    260U);

#endif

#ifdef STAGE2_TB_MBOX_CONF_FLAT_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: stage2_tb_mbox_conf_flat_dram_pitch
            if (o_args->stage2_tb_mbox_conf_flat_dram_pitch_enable)
                coproc_poke_half_offset(o_args->stage2_tb_mbox_conf_flat_dram_pitch_value, dagbin_base, 
                    300U);

#endif

#ifdef ARM_LOC_DRAM_PITCH_MODIFIABLE
            // Poke 32-bit variable: arm_loc_dram_pitch
            if (o_args->arm_loc_dram_pitch_enable)
                coproc_poke_word(o_args->arm_loc_dram_pitch_value, dagbin_base + 1392U);

#endif

#ifdef ARM_CONF_FLATTEN_DRAM_PITCH_MODIFIABLE
            // Poke 32-bit variable: arm_conf_flatten_dram_pitch
            if (o_args->arm_conf_flatten_dram_pitch_enable)
                coproc_poke_word(o_args->arm_conf_flatten_dram_pitch_value, dagbin_base + 2160U);

#endif

#ifdef P6____BN_P6_SCALE_MUL___MULI___411____BN_P6_SCALE_MUL____SCALE____RELU___MULI___1227_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: P6____bn_P6_scale_mul___muli___411____bn_P6_scale_mul____scale____relu___muli___1227_dram_pitch
            if (o_args->P6____bn_P6_scale_mul___muli___411____bn_P6_scale_mul____scale____relu___muli___1227_dram_pitch_enable)
                coproc_poke_half(o_args->P6____bn_P6_scale_mul___muli___411____bn_P6_scale_mul____scale____relu___muli___1227_dram_pitch_value, dagbin_base + 2304U);

#endif

#ifdef ELT5____RELU_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: Elt5____relu_dram_pitch
            if (o_args->Elt5____relu_dram_pitch_enable)
                coproc_poke_half(o_args->Elt5____relu_dram_pitch_value, dagbin_base + 2584U);

#endif

            /*** dmald() calls ***/
            break;
        case 6:
            /*** poke() calls ***/
            /* Intermediate buffers */
            // Poke HMB intermediate input buffer: Elt5____relu
            coproc_poke_word_offset(dag_base + 664960U,
                dagbin_base, 28U);

            // Poke HMB intermediate input buffer: stage3_tb_____bn_stage3_tb__scale_mul___muli___272_____bn_stage3_tb__scale_mul_____scale_____relu
            coproc_poke_word_offset(dag_base + 4505600U,
                dagbin_base, 64U);

            // Poke HMB intermediate output buffer: P5____bn_P5_scale_mul___muli___437____bn_P5_scale_mul____scale____relu___muli___1228
            coproc_poke_word(dag_base + 0U,
                dagbin_base + 688U);

            // Poke HMB intermediate output buffer: P4____bn_P4_scale_mul___muli___445____bn_P4_scale_mul____scale____relu___muli___1229
            coproc_poke_word(dag_base + 81920U,
                dagbin_base + 1016U);

            // Poke HMB intermediate output buffer: __pvcn_1569_
            coproc_poke_word(dag_base + 5816320U,
                dagbin_base + 1112U);

            /* Pokable fields */
#ifdef ELT5____RELU_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: Elt5____relu_dram_pitch
            if (o_args->Elt5____relu_dram_pitch_enable)
                coproc_poke_half_offset(o_args->Elt5____relu_dram_pitch_value, dagbin_base, 
                    32U);

#endif

#ifdef STAGE3_TB_____BN_STAGE3_TB__SCALE_MUL___MULI___272_____BN_STAGE3_TB__SCALE_MUL_____SCALE_____RELU_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: stage3_tb_____bn_stage3_tb__scale_mul___muli___272_____bn_stage3_tb__scale_mul_____scale_____relu_dram_pitch
            if (o_args->stage3_tb_____bn_stage3_tb__scale_mul___muli___272_____bn_stage3_tb__scale_mul_____scale_____relu_dram_pitch_enable)
                coproc_poke_half_offset(o_args->stage3_tb_____bn_stage3_tb__scale_mul___muli___272_____bn_stage3_tb__scale_mul_____scale_____relu_dram_pitch_value, dagbin_base, 
                    68U);

#endif

#ifdef P5____BN_P5_SCALE_MUL___MULI___437____BN_P5_SCALE_MUL____SCALE____RELU___MULI___1228_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: P5____bn_P5_scale_mul___muli___437____bn_P5_scale_mul____scale____relu___muli___1228_dram_pitch
            if (o_args->P5____bn_P5_scale_mul___muli___437____bn_P5_scale_mul____scale____relu___muli___1228_dram_pitch_enable)
                coproc_poke_half(o_args->P5____bn_P5_scale_mul___muli___437____bn_P5_scale_mul____scale____relu___muli___1228_dram_pitch_value, dagbin_base + 692U);

#endif

#ifdef P4____BN_P4_SCALE_MUL___MULI___445____BN_P4_SCALE_MUL____SCALE____RELU___MULI___1229_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: P4____bn_P4_scale_mul___muli___445____bn_P4_scale_mul____scale____relu___muli___1229_dram_pitch
            if (o_args->P4____bn_P4_scale_mul___muli___445____bn_P4_scale_mul____scale____relu___muli___1229_dram_pitch_enable)
                coproc_poke_half(o_args->P4____bn_P4_scale_mul___muli___445____bn_P4_scale_mul____scale____relu___muli___1229_dram_pitch_value, dagbin_base + 1020U);

#endif

#ifdef __PVCN_1569__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_1569__dram_pitch
            if (o_args->__pvcn_1569__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_1569__dram_pitch_value, dagbin_base + 1116U);

#endif

            /*** dmald() calls ***/
            break;
        case 7:
            /*** poke() calls ***/
            /* Intermediate buffers */
            // Poke HMB intermediate input buffer: P5____bn_P5_scale_mul___muli___437____bn_P5_scale_mul____scale____relu___muli___1228
            coproc_poke_word_offset(dag_base + 0U,
                dagbin_base, 28U);

            // Poke HMB intermediate input buffer: P4____bn_P4_scale_mul___muli___445____bn_P4_scale_mul____scale____relu___muli___1229
            coproc_poke_word_offset(dag_base + 81920U,
                dagbin_base, 68U);

            // Poke HMB intermediate input buffer: P6____bn_P6_scale_mul___muli___411____bn_P6_scale_mul____scale____relu___muli___1227
            coproc_poke_word_offset(dag_base + 644480U,
                dagbin_base, 108U);

            // Poke HMB intermediate input buffer: stage2_tb_____bn_stage2_tb__scale_mul___muli___138_____bn_stage2_tb__scale_mul_____scale_____relu
            coproc_poke_word_offset(dag_base + 1638400U,
                dagbin_base, 148U);

            // Poke HMB intermediate input buffer: __pvcn_1569_
            coproc_poke_word_offset(dag_base + 5816320U,
                dagbin_base, 188U);

            /* Primary outputs */
            // Poke HMB output buffer: odm_loc
            coproc_poke_word(r_args->odm_loc_addr + 0U,
                dagbin_base + 1816U);

            // Poke HMB output buffer: odm_conf_flatten
            coproc_poke_word(r_args->odm_conf_flatten_addr + 0U,
                dagbin_base + 2668U);

            /* Pokable fields */
#ifdef P5____BN_P5_SCALE_MUL___MULI___437____BN_P5_SCALE_MUL____SCALE____RELU___MULI___1228_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: P5____bn_P5_scale_mul___muli___437____bn_P5_scale_mul____scale____relu___muli___1228_dram_pitch
            if (o_args->P5____bn_P5_scale_mul___muli___437____bn_P5_scale_mul____scale____relu___muli___1228_dram_pitch_enable)
                coproc_poke_half_offset(o_args->P5____bn_P5_scale_mul___muli___437____bn_P5_scale_mul____scale____relu___muli___1228_dram_pitch_value, dagbin_base, 
                    32U);

#endif

#ifdef P4____BN_P4_SCALE_MUL___MULI___445____BN_P4_SCALE_MUL____SCALE____RELU___MULI___1229_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: P4____bn_P4_scale_mul___muli___445____bn_P4_scale_mul____scale____relu___muli___1229_dram_pitch
            if (o_args->P4____bn_P4_scale_mul___muli___445____bn_P4_scale_mul____scale____relu___muli___1229_dram_pitch_enable)
                coproc_poke_half_offset(o_args->P4____bn_P4_scale_mul___muli___445____bn_P4_scale_mul____scale____relu___muli___1229_dram_pitch_value, dagbin_base, 
                    72U);

#endif

#ifdef P6____BN_P6_SCALE_MUL___MULI___411____BN_P6_SCALE_MUL____SCALE____RELU___MULI___1227_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: P6____bn_P6_scale_mul___muli___411____bn_P6_scale_mul____scale____relu___muli___1227_dram_pitch
            if (o_args->P6____bn_P6_scale_mul___muli___411____bn_P6_scale_mul____scale____relu___muli___1227_dram_pitch_enable)
                coproc_poke_half_offset(o_args->P6____bn_P6_scale_mul___muli___411____bn_P6_scale_mul____scale____relu___muli___1227_dram_pitch_value, dagbin_base, 
                    112U);

#endif

#ifdef STAGE2_TB_____BN_STAGE2_TB__SCALE_MUL___MULI___138_____BN_STAGE2_TB__SCALE_MUL_____SCALE_____RELU_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: stage2_tb_____bn_stage2_tb__scale_mul___muli___138_____bn_stage2_tb__scale_mul_____scale_____relu_dram_pitch
            if (o_args->stage2_tb_____bn_stage2_tb__scale_mul___muli___138_____bn_stage2_tb__scale_mul_____scale_____relu_dram_pitch_enable)
                coproc_poke_half_offset(o_args->stage2_tb_____bn_stage2_tb__scale_mul___muli___138_____bn_stage2_tb__scale_mul_____scale_____relu_dram_pitch_value, dagbin_base, 
                    152U);

#endif

#ifdef __PVCN_1569__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_1569__dram_pitch
            if (o_args->__pvcn_1569__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_1569__dram_pitch_value, dagbin_base, 
                    192U);

#endif

#ifdef ODM_LOC_DRAM_PITCH_MODIFIABLE
            // Poke 32-bit variable: odm_loc_dram_pitch
            if (o_args->odm_loc_dram_pitch_enable)
                coproc_poke_word(o_args->odm_loc_dram_pitch_value, dagbin_base + 1820U);

#endif

#ifdef ODM_CONF_FLATTEN_DRAM_PITCH_MODIFIABLE
            // Poke 32-bit variable: odm_conf_flatten_dram_pitch
            if (o_args->odm_conf_flatten_dram_pitch_enable)
                coproc_poke_word(o_args->odm_conf_flatten_dram_pitch_value, dagbin_base + 2672U);

#endif

            /*** dmald() calls ***/
            break;
        default:
            break;
    }

    return;
} // end of openodnet_ag_poke_split()

/* Prompts VP to execute code */
inline void openodnet_ag_run_split(
    uint32_t split_id
) {
    coproc_run(vmem_dagbin_bases[split_id]);
    return;
} // end of openodnet_ag_run_split()

/* Handle SMB buffers of a particular split DAG */
inline void openodnet_ag_peek_split(
    openodnet_ag_required_fields_t *r_args,
    struct openodnet_ag_split_context *split_ctxt,
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
        default:
            break;
    }

    coproc_peek_word(0U, COPROC_STATUS_ADDR);
    return;
} // end of openodnet_ag_peek_split()

