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

#include "mnet_ssd_adas_flex_pic_ag.h"
#include "mnet_ssd_adas_flex_pic_ag_private.h"

static const uint32_t dag_img_sizes[] = {
    286364U, 
    786932U, 
    968756U, 
    1339644U, 
    1960656U, 
    57228U, 
};

static const uint32_t vmem_dag_bases[] = {
    1798520U, 
    1294656U, 
    1110624U, 
    710700U, 
    88560U, 
    216948U, 
};

static const uint32_t vmem_dagbin_bases[] = {
    2074716U, 
    2075136U, 
    2074128U, 
    2046636U, 
    2048188U, 
    271856U, 
};

/******************************* CORE FUNCTIONS *******************************/
/* Populate structs with constants
 * Calls the wrapper function for cvtable_find()
 */
errcode_enum_t mnet_ssd_adas_flex_pic_ag_init (
    mnet_ssd_adas_flex_pic_ag_ips_t *ctxt
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
    status = mnet_ssd_adas_flex_pic_ag_read_cvtable(ctxt);
    if (is_err(status)) {
        cvtask_printf(LVL_DEBUG, "mnet_ssd_adas_flex_pic_ag_read_cvtable() has failed.", 0, 0
            , 0, 0, 0);
        return status;
    }

    return status;
} // end of mnet_ssd_adas_flex_pic_ag_init()

/* Execute VP DAGs
 * run() is NOT re-entrant with respect to run() calls for other DAGs
 */
errcode_enum_t mnet_ssd_adas_flex_pic_ag_run (
    mnet_ssd_adas_flex_pic_ag_ips_t             *ctxt,
    mnet_ssd_adas_flex_pic_ag_required_fields_t *r_args,
    mnet_ssd_adas_flex_pic_ag_optional_fields_t *o_args
) {
    int split_id = 0;
    errcode_enum_t status = ERRCODE_NONE;
    struct mnet_ssd_adas_flex_pic_ag_split_context *split_ctxt = NULL;
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

    for (; split_id < MNET_SSD_ADAS_FLEX_PIC_AG_SPLIT_COUNT; split_id++) {
        split_ctxt = &ctxt->split_ctxt[split_id];
        mnet_ssd_adas_flex_pic_ag_load_split(split_ctxt, split_id);
        mnet_ssd_adas_flex_pic_ag_poke_split(r_args, o_args, split_ctxt, split_id);
        wait(VIS_W_COPROC); // wait on dummy run in vp_load_dag()

#ifdef CVTASK_PRINT
        time_end = get_cur_time();
        cvtask_printf(LVL_NORMAL,
            "  >> done loading split-#%03u.     cycles=(%u)",
            split_id, time_end - time_start, 0, 0, 0);
        time_start = time_end;
#endif
        mnet_ssd_adas_flex_pic_ag_run_split(split_id);
        mnet_ssd_adas_flex_pic_ag_peek_split(r_args, split_ctxt, split_id);

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
} // end of mnet_ssd_adas_flex_pic_ag_run()

/* Populate struct with default values */
errcode_enum_t mnet_ssd_adas_flex_pic_ag_reset(
    mnet_ssd_adas_flex_pic_ag_optional_fields_t *o_args
) {
    errcode_enum_t status = ERRCODE_NONE;

    if (!o_args) {
        status = ERRCODE_BAD_PARAMETER;
        cvtask_printf(LVL_DEBUG, "  > Error: NULL pointer(s).", 0, 0, 0, 0, 0);
        return status;
    }

#ifdef __PVCN_1769__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_1769__dram_pitch_enable = 1U;
    o_args->__pvcn_1769__dram_pitch_value = 32U;
#endif
#ifdef CONV5_1__DW_____BN___MULI___60_____BN_____SCALE_MUL_____SCALE___RELU5_1__DW_DRAM_PITCH_MODIFIABLE
    o_args->conv5_1__dw_____bn___muli___60_____bn_____scale_mul_____scale___relu5_1__dw_dram_pitch_enable = 1U;
    o_args->conv5_1__dw_____bn___muli___60_____bn_____scale_mul_____scale___relu5_1__dw_dram_pitch_value = 32U;
#endif
#ifdef CONV5_4__DW_____BN___MULI___84_____BN_____SCALE_MUL_____SCALE___MULI___423_DRAM_PITCH_MODIFIABLE
    o_args->conv5_4__dw_____bn___muli___84_____bn_____scale_mul_____scale___muli___423_dram_pitch_enable = 1U;
    o_args->conv5_4__dw_____bn___muli___84_____bn_____scale_mul_____scale___muli___423_dram_pitch_value = 32U;
#endif
#ifdef CONV5_5__SEP_____BN___MULI___96_____BN_____SCALE_MUL_____SCALE___RELU5_5__SEP_DRAM_PITCH_MODIFIABLE
    o_args->conv5_5__sep_____bn___muli___96_____bn_____scale_mul_____scale___relu5_5__sep_dram_pitch_enable = 1U;
    o_args->conv5_5__sep_____bn___muli___96_____bn_____scale_mul_____scale___relu5_5__sep_dram_pitch_value = 32U;
#endif
#ifdef CONV5_5_MBOX_CONF___CONV2I___99______LOC___CONV2I___171____DRAM_PITCH_MODIFIABLE
    o_args->conv5_5_mbox_conf___conv2i___99______loc___conv2i___171____dram_pitch_enable = 1U;
    o_args->conv5_5_mbox_conf___conv2i___99______loc___conv2i___171____dram_pitch_value = 32U;
#endif
#ifdef CONV5_5_MBOX_LOC_FLAT_DRAM_PITCH_MODIFIABLE
    o_args->conv5_5_mbox_loc_flat_dram_pitch_enable = 1U;
    o_args->conv5_5_mbox_loc_flat_dram_pitch_value = 32U;
#endif
#ifdef CONV6_MBOX_CONF___CONV2I___117______7_1_____BN___MULI___120_____BN_____SCALE_MUL_____SCALE___LOC___CONV2I___173____DRAM_PITCH_MODIFIABLE
    o_args->conv6_mbox_conf___conv2i___117______7_1_____bn___muli___120_____bn_____scale_mul_____scale___loc___conv2i___173____dram_pitch_enable = 1U;
    o_args->conv6_mbox_conf___conv2i___117______7_1_____bn___muli___120_____bn_____scale_mul_____scale___loc___conv2i___173____dram_pitch_value = 32U;
#endif
#ifdef CONV6_MBOX_LOC_FLAT_DRAM_PITCH_MODIFIABLE
    o_args->conv6_mbox_loc_flat_dram_pitch_enable = 1U;
    o_args->conv6_mbox_loc_flat_dram_pitch_value = 32U;
#endif
#ifdef CONV7_2_MBOX_CONF___CONV2I___127______8_1_____BN___MULI___130_____BN_____SCALE_MUL_____SCALE___LOC___CONV2I___175____DRAM_PITCH_MODIFIABLE
    o_args->conv7_2_mbox_conf___conv2i___127______8_1_____bn___muli___130_____bn_____scale_mul_____scale___loc___conv2i___175____dram_pitch_enable = 1U;
    o_args->conv7_2_mbox_conf___conv2i___127______8_1_____bn___muli___130_____bn_____scale_mul_____scale___loc___conv2i___175____dram_pitch_value = 32U;
#endif
#ifdef CONV8_2_MBOX_CONF___CONV2I___137______9_1_____BN___MULI___140_____BN_____SCALE_MUL_____SCALE___LOC___CONV2I___177____DRAM_PITCH_MODIFIABLE
    o_args->conv8_2_mbox_conf___conv2i___137______9_1_____bn___muli___140_____bn_____scale_mul_____scale___loc___conv2i___177____dram_pitch_enable = 1U;
    o_args->conv8_2_mbox_conf___conv2i___137______9_1_____bn___muli___140_____bn_____scale_mul_____scale___loc___conv2i___177____dram_pitch_value = 32U;
#endif
#ifdef CONV9_2_MBOX_CONF___CONV2I___147______10_1_____BN___MULI___150_____BN_____SCALE_MUL_____SCALE___LOC___CONV2I___179____DRAM_PITCH_MODIFIABLE
    o_args->conv9_2_mbox_conf___conv2i___147______10_1_____bn___muli___150_____bn_____scale_mul_____scale___loc___conv2i___179____dram_pitch_enable = 1U;
    o_args->conv9_2_mbox_conf___conv2i___147______10_1_____bn___muli___150_____bn_____scale_mul_____scale___loc___conv2i___179____dram_pitch_value = 32U;
#endif
#ifdef DATA_DRAM_PITCH_MODIFIABLE
    o_args->data_dram_pitch_enable = 1U;
    o_args->data_dram_pitch_value = 320U;
#endif
#ifdef DATA_UV_DRAM_PITCH_MODIFIABLE
    o_args->data_uv_dram_pitch_enable = 1U;
    o_args->data_uv_dram_pitch_value = 320U;
#endif
#ifdef MBOX_CONF_FLATTEN_DRAM_PITCH_MODIFIABLE
    o_args->mbox_conf_flatten_dram_pitch_enable = 1U;
    o_args->mbox_conf_flatten_dram_pitch_value = 53696U;
#endif
#ifdef MBOX_LOC_DRAM_PITCH_MODIFIABLE
    o_args->mbox_loc_dram_pitch_enable = 1U;
    o_args->mbox_loc_dram_pitch_value = 30688U;
#endif
#ifdef RELU10_1_DRAM_PITCH_MODIFIABLE
    o_args->relu10_1_dram_pitch_enable = 1U;
    o_args->relu10_1_dram_pitch_value = 32U;
#endif
#ifdef RELU7_1_DRAM_PITCH_MODIFIABLE
    o_args->relu7_1_dram_pitch_enable = 1U;
    o_args->relu7_1_dram_pitch_value = 32U;
#endif

    return status;
} // end of mnet_ssd_adas_flex_pic_ag_reset()


/********************* USED BY MNET_SSD_ADAS_FLEX_PIC_AG_INIT() **********************/
/* Wrapper function for cvtable_find() */
inline errcode_enum_t mnet_ssd_adas_flex_pic_ag_read_cvtable (
    mnet_ssd_adas_flex_pic_ag_ips_t *ctxt
) {
    unsigned int split_id = 0; // size_t
    int32_t tbar_sz;
    errcode_enum_t status = ERRCODE_NONE;
    struct mnet_ssd_adas_flex_pic_ag_split_context *split_ctxt = NULL;
    char dag_key[] = "MNET_SSD_ADAS_FLEX_PIC_AG_---_DAG";

    for (; split_id < MNET_SSD_ADAS_FLEX_PIC_AG_SPLIT_COUNT; split_id++) {
        tbar_sz = 0;
        split_ctxt = &ctxt->split_ctxt[split_id];

#ifdef CVTASK_PRINT
        cvtask_printf(LVL_DEBUG, "  >> #Split-%u# Extracting DAG base address "
            "from CV Table Archive", split_id, 0, 0, 0, 0);
#endif

        dag_key[26] = '0' + ((split_id / 100U) % 10U);
        dag_key[27] = '0' + ((split_id / 10U ) % 10U);
        dag_key[28] = '0' + ((split_id / 1U  ) % 10U);

        // extracting DAG base from CV Table Archive
        status = cvtable_find(
            dag_key,
            (const void **)&split_ctxt->dram_dag_base,
            &tbar_sz
        );
        if (is_err(status))
        {
            cvtask_printf(LVL_DEBUG, "  >> cvtable_find() key MNET_SSD_ADAS_FLEX_PIC_AG_"
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
} // end of mnet_ssd_adas_flex_pic_ag_read_cvtable()


/********************** USED BY MNET_SSD_ADAS_FLEX_PIC_AG_RUN() **********************/
/* Load machine code into VP */
inline void mnet_ssd_adas_flex_pic_ag_load_split(
    struct mnet_ssd_adas_flex_pic_ag_split_context *split_ctxt,
    uint32_t split_id
) {
    vp_init();

    vp_load_dag(
        split_ctxt->dram_dag_base,
        vmem_dag_bases[split_id],
        dag_img_sizes[split_id],
        0
    );

    //dma_cmem_2_coproc(0x100, MNET_SSD_ADAS_FLEX_PIC_AG_CMEM_BASE, 1024);

    return;
} // end of mnet_ssd_adas_flex_pic_ag_load_split()

/* Modify input and output buffer addresses of a particular split DAG */
inline void mnet_ssd_adas_flex_pic_ag_poke_split(
    mnet_ssd_adas_flex_pic_ag_required_fields_t *r_args,
    mnet_ssd_adas_flex_pic_ag_optional_fields_t *o_args,
    struct mnet_ssd_adas_flex_pic_ag_split_context *split_ctxt,
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
            // Poke HMB input buffer: data
            coproc_poke_word(r_args->data_addr + 0, dagbin_base + 4512U);

            // Poke HMB input buffer: data_uv
            coproc_poke_word(r_args->data_uv_addr + 0, dagbin_base + 4552U);

            /* Intermediate buffers */
            // Poke HMB intermediate output buffer: conv5_1__dw_____bn___muli___60_____bn_____scale_mul_____scale___relu5_1__dw
            coproc_poke_word(dag_base + 0,
                dagbin_base + 4908U);

            /* Pokable fields */
#ifdef DATA_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: data_dram_pitch
            if (o_args->data_dram_pitch_enable)
                coproc_poke_half(o_args->data_dram_pitch_value, dagbin_base + 4516U);

#endif

#ifdef DATA_UV_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: data_uv_dram_pitch
            if (o_args->data_uv_dram_pitch_enable)
                coproc_poke_half(o_args->data_uv_dram_pitch_value, dagbin_base + 4556U);

#endif

#ifdef CONV5_1__DW_____BN___MULI___60_____BN_____SCALE_MUL_____SCALE___RELU5_1__DW_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: conv5_1__dw_____bn___muli___60_____bn_____scale_mul_____scale___relu5_1__dw_dram_pitch
            if (o_args->conv5_1__dw_____bn___muli___60_____bn_____scale_mul_____scale___relu5_1__dw_dram_pitch_enable)
                coproc_poke_half(o_args->conv5_1__dw_____bn___muli___60_____bn_____scale_mul_____scale___relu5_1__dw_dram_pitch_value, dagbin_base + 4912U);

#endif

            /*** dmald() calls ***/
            break;
        case 1:
            /*** poke() calls ***/
            /* Intermediate buffers */
            // Poke HMB intermediate input buffer: conv5_1__dw_____bn___muli___60_____bn_____scale_mul_____scale___relu5_1__dw
            coproc_poke_word_offset(dag_base + 0,
                dagbin_base, 28U);

            // Poke HMB intermediate output buffer: conv5_4__dw_____bn___muli___84_____bn_____scale_mul_____scale___muli___423
            coproc_poke_word(dag_base + 311296,
                dagbin_base + 6364U);

            /* Pokable fields */
#ifdef CONV5_1__DW_____BN___MULI___60_____BN_____SCALE_MUL_____SCALE___RELU5_1__DW_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: conv5_1__dw_____bn___muli___60_____bn_____scale_mul_____scale___relu5_1__dw_dram_pitch
            if (o_args->conv5_1__dw_____bn___muli___60_____bn_____scale_mul_____scale___relu5_1__dw_dram_pitch_enable)
                coproc_poke_half_offset(o_args->conv5_1__dw_____bn___muli___60_____bn_____scale_mul_____scale___relu5_1__dw_dram_pitch_value, dagbin_base, 
                    32U);

#endif

#ifdef CONV5_4__DW_____BN___MULI___84_____BN_____SCALE_MUL_____SCALE___MULI___423_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: conv5_4__dw_____bn___muli___84_____bn_____scale_mul_____scale___muli___423_dram_pitch
            if (o_args->conv5_4__dw_____bn___muli___84_____bn_____scale_mul_____scale___muli___423_dram_pitch_enable)
                coproc_poke_half(o_args->conv5_4__dw_____bn___muli___84_____bn_____scale_mul_____scale___muli___423_dram_pitch_value, dagbin_base + 6368U);

#endif

            /*** dmald() calls ***/
            break;
        case 2:
            /*** poke() calls ***/
            /* Intermediate buffers */
            // Poke HMB intermediate input buffer: conv5_4__dw_____bn___muli___84_____bn_____scale_mul_____scale___muli___423
            coproc_poke_word_offset(dag_base + 311296,
                dagbin_base, 28U);

            // Poke HMB intermediate output buffer: conv5_5__sep_____bn___muli___96_____bn_____scale_mul_____scale___relu5_5__sep
            coproc_poke_word(dag_base + 0,
                dagbin_base + 3732U);

            // Poke HMB intermediate output buffer: __pvcn_1769_
            coproc_poke_word(dag_base + 544768,
                dagbin_base + 5164U);

            /* Pokable fields */
#ifdef CONV5_4__DW_____BN___MULI___84_____BN_____SCALE_MUL_____SCALE___MULI___423_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: conv5_4__dw_____bn___muli___84_____bn_____scale_mul_____scale___muli___423_dram_pitch
            if (o_args->conv5_4__dw_____bn___muli___84_____bn_____scale_mul_____scale___muli___423_dram_pitch_enable)
                coproc_poke_half_offset(o_args->conv5_4__dw_____bn___muli___84_____bn_____scale_mul_____scale___muli___423_dram_pitch_value, dagbin_base, 
                    32U);

#endif

#ifdef CONV5_5__SEP_____BN___MULI___96_____BN_____SCALE_MUL_____SCALE___RELU5_5__SEP_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: conv5_5__sep_____bn___muli___96_____bn_____scale_mul_____scale___relu5_5__sep_dram_pitch
            if (o_args->conv5_5__sep_____bn___muli___96_____bn_____scale_mul_____scale___relu5_5__sep_dram_pitch_enable)
                coproc_poke_half(o_args->conv5_5__sep_____bn___muli___96_____bn_____scale_mul_____scale___relu5_5__sep_dram_pitch_value, dagbin_base + 3736U);

#endif

#ifdef __PVCN_1769__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_1769__dram_pitch
            if (o_args->__pvcn_1769__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_1769__dram_pitch_value, dagbin_base + 5168U);

#endif

            /*** dmald() calls ***/
            break;
        case 3:
            /*** poke() calls ***/
            /* Intermediate buffers */
            // Poke HMB intermediate input buffer: conv5_5__sep_____bn___muli___96_____bn_____scale_mul_____scale___relu5_5__sep
            coproc_poke_word_offset(dag_base + 0,
                dagbin_base, 28U);

            // Poke HMB intermediate input buffer: __pvcn_1769_
            coproc_poke_word_offset(dag_base + 544768,
                dagbin_base, 68U);

            // Poke HMB intermediate output buffer: conv6_mbox_conf___conv2i___117______7_1_____bn___muli___120_____bn_____scale_mul_____scale___loc___conv2i___173___
            coproc_poke_word(dag_base + 311296,
                dagbin_base + 3384U);

            // Poke HMB intermediate output buffer: conv6_mbox_loc_flat
            coproc_poke_word(dag_base + 362816,
                dagbin_base + 3452U);

            // Poke HMB intermediate output buffer: conv5_5_mbox_conf___conv2i___99______loc___conv2i___171___
            coproc_poke_word(dag_base + 365248,
                dagbin_base + 3504U);

            // Poke HMB intermediate output buffer: conv5_5_mbox_loc_flat
            coproc_poke_word(dag_base + 385344,
                dagbin_base + 3568U);

            // Poke HMB intermediate output buffer: relu7_1
            coproc_poke_word(dag_base + 389696,
                dagbin_base + 3620U);

            /* Pokable fields */
#ifdef CONV5_5__SEP_____BN___MULI___96_____BN_____SCALE_MUL_____SCALE___RELU5_5__SEP_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: conv5_5__sep_____bn___muli___96_____bn_____scale_mul_____scale___relu5_5__sep_dram_pitch
            if (o_args->conv5_5__sep_____bn___muli___96_____bn_____scale_mul_____scale___relu5_5__sep_dram_pitch_enable)
                coproc_poke_half_offset(o_args->conv5_5__sep_____bn___muli___96_____bn_____scale_mul_____scale___relu5_5__sep_dram_pitch_value, dagbin_base, 
                    32U);

#endif

#ifdef __PVCN_1769__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_1769__dram_pitch
            if (o_args->__pvcn_1769__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_1769__dram_pitch_value, dagbin_base, 
                    72U);

#endif

#ifdef CONV6_MBOX_CONF___CONV2I___117______7_1_____BN___MULI___120_____BN_____SCALE_MUL_____SCALE___LOC___CONV2I___173____DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: conv6_mbox_conf___conv2i___117______7_1_____bn___muli___120_____bn_____scale_mul_____scale___loc___conv2i___173____dram_pitch
            if (o_args->conv6_mbox_conf___conv2i___117______7_1_____bn___muli___120_____bn_____scale_mul_____scale___loc___conv2i___173____dram_pitch_enable)
                coproc_poke_half(o_args->conv6_mbox_conf___conv2i___117______7_1_____bn___muli___120_____bn_____scale_mul_____scale___loc___conv2i___173____dram_pitch_value, dagbin_base + 3388U);

#endif

#ifdef CONV6_MBOX_LOC_FLAT_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: conv6_mbox_loc_flat_dram_pitch
            if (o_args->conv6_mbox_loc_flat_dram_pitch_enable)
                coproc_poke_half(o_args->conv6_mbox_loc_flat_dram_pitch_value, dagbin_base + 3456U);

#endif

#ifdef CONV5_5_MBOX_CONF___CONV2I___99______LOC___CONV2I___171____DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: conv5_5_mbox_conf___conv2i___99______loc___conv2i___171____dram_pitch
            if (o_args->conv5_5_mbox_conf___conv2i___99______loc___conv2i___171____dram_pitch_enable)
                coproc_poke_half(o_args->conv5_5_mbox_conf___conv2i___99______loc___conv2i___171____dram_pitch_value, dagbin_base + 3508U);

#endif

#ifdef CONV5_5_MBOX_LOC_FLAT_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: conv5_5_mbox_loc_flat_dram_pitch
            if (o_args->conv5_5_mbox_loc_flat_dram_pitch_enable)
                coproc_poke_half(o_args->conv5_5_mbox_loc_flat_dram_pitch_value, dagbin_base + 3572U);

#endif

#ifdef RELU7_1_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: relu7_1_dram_pitch
            if (o_args->relu7_1_dram_pitch_enable)
                coproc_poke_half(o_args->relu7_1_dram_pitch_value, dagbin_base + 3624U);

#endif

            /*** dmald() calls ***/
            break;
        case 4:
            /*** poke() calls ***/
            /* Intermediate buffers */
            // Poke HMB intermediate input buffer: relu7_1
            coproc_poke_word_offset(dag_base + 389696,
                dagbin_base, 28U);

            // Poke HMB intermediate output buffer: conv7_2_mbox_conf___conv2i___127______8_1_____bn___muli___130_____bn_____scale_mul_____scale___loc___conv2i___175___
            coproc_poke_word(dag_base + 0,
                dagbin_base + 608U);

            // Poke HMB intermediate output buffer: conv8_2_mbox_conf___conv2i___137______9_1_____bn___muli___140_____bn_____scale_mul_____scale___loc___conv2i___177___
            coproc_poke_word(dag_base + 7808,
                dagbin_base + 748U);

            // Poke HMB intermediate output buffer: conv9_2_mbox_conf___conv2i___147______10_1_____bn___muli___150_____bn_____scale_mul_____scale___loc___conv2i___179___
            coproc_poke_word(dag_base + 10176,
                dagbin_base + 888U);

            // Poke HMB intermediate output buffer: relu10_1
            coproc_poke_word(dag_base + 11264,
                dagbin_base + 940U);

            /* Pokable fields */
#ifdef RELU7_1_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: relu7_1_dram_pitch
            if (o_args->relu7_1_dram_pitch_enable)
                coproc_poke_half_offset(o_args->relu7_1_dram_pitch_value, dagbin_base, 
                    32U);

#endif

#ifdef CONV7_2_MBOX_CONF___CONV2I___127______8_1_____BN___MULI___130_____BN_____SCALE_MUL_____SCALE___LOC___CONV2I___175____DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: conv7_2_mbox_conf___conv2i___127______8_1_____bn___muli___130_____bn_____scale_mul_____scale___loc___conv2i___175____dram_pitch
            if (o_args->conv7_2_mbox_conf___conv2i___127______8_1_____bn___muli___130_____bn_____scale_mul_____scale___loc___conv2i___175____dram_pitch_enable)
                coproc_poke_half(o_args->conv7_2_mbox_conf___conv2i___127______8_1_____bn___muli___130_____bn_____scale_mul_____scale___loc___conv2i___175____dram_pitch_value, dagbin_base + 612U);

#endif

#ifdef CONV8_2_MBOX_CONF___CONV2I___137______9_1_____BN___MULI___140_____BN_____SCALE_MUL_____SCALE___LOC___CONV2I___177____DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: conv8_2_mbox_conf___conv2i___137______9_1_____bn___muli___140_____bn_____scale_mul_____scale___loc___conv2i___177____dram_pitch
            if (o_args->conv8_2_mbox_conf___conv2i___137______9_1_____bn___muli___140_____bn_____scale_mul_____scale___loc___conv2i___177____dram_pitch_enable)
                coproc_poke_half(o_args->conv8_2_mbox_conf___conv2i___137______9_1_____bn___muli___140_____bn_____scale_mul_____scale___loc___conv2i___177____dram_pitch_value, dagbin_base + 752U);

#endif

#ifdef CONV9_2_MBOX_CONF___CONV2I___147______10_1_____BN___MULI___150_____BN_____SCALE_MUL_____SCALE___LOC___CONV2I___179____DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: conv9_2_mbox_conf___conv2i___147______10_1_____bn___muli___150_____bn_____scale_mul_____scale___loc___conv2i___179____dram_pitch
            if (o_args->conv9_2_mbox_conf___conv2i___147______10_1_____bn___muli___150_____bn_____scale_mul_____scale___loc___conv2i___179____dram_pitch_enable)
                coproc_poke_half(o_args->conv9_2_mbox_conf___conv2i___147______10_1_____bn___muli___150_____bn_____scale_mul_____scale___loc___conv2i___179____dram_pitch_value, dagbin_base + 892U);

#endif

#ifdef RELU10_1_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: relu10_1_dram_pitch
            if (o_args->relu10_1_dram_pitch_enable)
                coproc_poke_half(o_args->relu10_1_dram_pitch_value, dagbin_base + 944U);

#endif

            /*** dmald() calls ***/
            break;
        case 5:
            /*** poke() calls ***/
            /* Intermediate buffers */
            // Poke HMB intermediate input buffer: conv7_2_mbox_conf___conv2i___127______8_1_____bn___muli___130_____bn_____scale_mul_____scale___loc___conv2i___175___
            coproc_poke_word_offset(dag_base + 0,
                dagbin_base, 28U);

            // Poke HMB intermediate input buffer: conv8_2_mbox_conf___conv2i___137______9_1_____bn___muli___140_____bn_____scale_mul_____scale___loc___conv2i___177___
            coproc_poke_word_offset(dag_base + 7808,
                dagbin_base, 68U);

            // Poke HMB intermediate input buffer: conv9_2_mbox_conf___conv2i___147______10_1_____bn___muli___150_____bn_____scale_mul_____scale___loc___conv2i___179___
            coproc_poke_word_offset(dag_base + 10176,
                dagbin_base, 108U);

            // Poke HMB intermediate input buffer: relu10_1
            coproc_poke_word_offset(dag_base + 11264,
                dagbin_base, 148U);

            // Poke HMB intermediate input buffer: conv6_mbox_conf___conv2i___117______7_1_____bn___muli___120_____bn_____scale_mul_____scale___loc___conv2i___173___
            coproc_poke_word_offset(dag_base + 311296,
                dagbin_base, 184U);

            // Poke HMB intermediate input buffer: conv6_mbox_loc_flat
            coproc_poke_word_offset(dag_base + 362816,
                dagbin_base, 220U);

            // Poke HMB intermediate input buffer: conv5_5_mbox_conf___conv2i___99______loc___conv2i___171___
            coproc_poke_word_offset(dag_base + 365248,
                dagbin_base, 260U);

            // Poke HMB intermediate input buffer: conv5_5_mbox_loc_flat
            coproc_poke_word_offset(dag_base + 385344,
                dagbin_base, 296U);

            /* Primary outputs */
            // Poke HMB output buffer: mbox_loc
            coproc_poke_word(r_args->mbox_loc_addr + 0,
                dagbin_base + 1340U);

            // Poke HMB output buffer: mbox_conf_flatten
            coproc_poke_word(r_args->mbox_conf_flatten_addr + 0,
                dagbin_base + 2232U);

            /* Pokable fields */
#ifdef CONV7_2_MBOX_CONF___CONV2I___127______8_1_____BN___MULI___130_____BN_____SCALE_MUL_____SCALE___LOC___CONV2I___175____DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: conv7_2_mbox_conf___conv2i___127______8_1_____bn___muli___130_____bn_____scale_mul_____scale___loc___conv2i___175____dram_pitch
            if (o_args->conv7_2_mbox_conf___conv2i___127______8_1_____bn___muli___130_____bn_____scale_mul_____scale___loc___conv2i___175____dram_pitch_enable)
                coproc_poke_half_offset(o_args->conv7_2_mbox_conf___conv2i___127______8_1_____bn___muli___130_____bn_____scale_mul_____scale___loc___conv2i___175____dram_pitch_value, dagbin_base, 
                    32U);

#endif

#ifdef CONV8_2_MBOX_CONF___CONV2I___137______9_1_____BN___MULI___140_____BN_____SCALE_MUL_____SCALE___LOC___CONV2I___177____DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: conv8_2_mbox_conf___conv2i___137______9_1_____bn___muli___140_____bn_____scale_mul_____scale___loc___conv2i___177____dram_pitch
            if (o_args->conv8_2_mbox_conf___conv2i___137______9_1_____bn___muli___140_____bn_____scale_mul_____scale___loc___conv2i___177____dram_pitch_enable)
                coproc_poke_half_offset(o_args->conv8_2_mbox_conf___conv2i___137______9_1_____bn___muli___140_____bn_____scale_mul_____scale___loc___conv2i___177____dram_pitch_value, dagbin_base, 
                    72U);

#endif

#ifdef CONV9_2_MBOX_CONF___CONV2I___147______10_1_____BN___MULI___150_____BN_____SCALE_MUL_____SCALE___LOC___CONV2I___179____DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: conv9_2_mbox_conf___conv2i___147______10_1_____bn___muli___150_____bn_____scale_mul_____scale___loc___conv2i___179____dram_pitch
            if (o_args->conv9_2_mbox_conf___conv2i___147______10_1_____bn___muli___150_____bn_____scale_mul_____scale___loc___conv2i___179____dram_pitch_enable)
                coproc_poke_half_offset(o_args->conv9_2_mbox_conf___conv2i___147______10_1_____bn___muli___150_____bn_____scale_mul_____scale___loc___conv2i___179____dram_pitch_value, dagbin_base, 
                    112U);

#endif

#ifdef RELU10_1_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: relu10_1_dram_pitch
            if (o_args->relu10_1_dram_pitch_enable)
                coproc_poke_half_offset(o_args->relu10_1_dram_pitch_value, dagbin_base, 
                    152U);

#endif

#ifdef CONV6_MBOX_CONF___CONV2I___117______7_1_____BN___MULI___120_____BN_____SCALE_MUL_____SCALE___LOC___CONV2I___173____DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: conv6_mbox_conf___conv2i___117______7_1_____bn___muli___120_____bn_____scale_mul_____scale___loc___conv2i___173____dram_pitch
            if (o_args->conv6_mbox_conf___conv2i___117______7_1_____bn___muli___120_____bn_____scale_mul_____scale___loc___conv2i___173____dram_pitch_enable)
                coproc_poke_half_offset(o_args->conv6_mbox_conf___conv2i___117______7_1_____bn___muli___120_____bn_____scale_mul_____scale___loc___conv2i___173____dram_pitch_value, dagbin_base, 
                    188U);

#endif

#ifdef CONV6_MBOX_LOC_FLAT_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: conv6_mbox_loc_flat_dram_pitch
            if (o_args->conv6_mbox_loc_flat_dram_pitch_enable)
                coproc_poke_half_offset(o_args->conv6_mbox_loc_flat_dram_pitch_value, dagbin_base, 
                    224U);

#endif

#ifdef CONV5_5_MBOX_CONF___CONV2I___99______LOC___CONV2I___171____DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: conv5_5_mbox_conf___conv2i___99______loc___conv2i___171____dram_pitch
            if (o_args->conv5_5_mbox_conf___conv2i___99______loc___conv2i___171____dram_pitch_enable)
                coproc_poke_half_offset(o_args->conv5_5_mbox_conf___conv2i___99______loc___conv2i___171____dram_pitch_value, dagbin_base, 
                    264U);

#endif

#ifdef CONV5_5_MBOX_LOC_FLAT_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: conv5_5_mbox_loc_flat_dram_pitch
            if (o_args->conv5_5_mbox_loc_flat_dram_pitch_enable)
                coproc_poke_half_offset(o_args->conv5_5_mbox_loc_flat_dram_pitch_value, dagbin_base, 
                    300U);

#endif

#ifdef MBOX_LOC_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: mbox_loc_dram_pitch
            if (o_args->mbox_loc_dram_pitch_enable)
                coproc_poke_half(o_args->mbox_loc_dram_pitch_value, dagbin_base + 1344U);

#endif

#ifdef MBOX_CONF_FLATTEN_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: mbox_conf_flatten_dram_pitch
            if (o_args->mbox_conf_flatten_dram_pitch_enable)
                coproc_poke_half(o_args->mbox_conf_flatten_dram_pitch_value, dagbin_base + 2236U);

#endif

            /*** dmald() calls ***/
            break;
        default:
            break;
    }

    return;
} // end of mnet_ssd_adas_flex_pic_ag_poke_split()

/* Prompts VP to execute code */
inline void mnet_ssd_adas_flex_pic_ag_run_split(
    uint32_t split_id
) {
    coproc_run(vmem_dagbin_bases[split_id]);
    return;
} // end of mnet_ssd_adas_flex_pic_ag_run_split()

/* Handle SMB buffers of a particular split DAG */
inline errcode_enum_t mnet_ssd_adas_flex_pic_ag_peek_split(
    mnet_ssd_adas_flex_pic_ag_required_fields_t *r_args,
    struct mnet_ssd_adas_flex_pic_ag_split_context *split_ctxt,
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
        default:
            break;
    }

    return ERRCODE_NONE;
} // end of mnet_ssd_adas_flex_pic_ag_peek_split()

/****************** USED BY MNET_SSD_ADAS_FLEX_PIC_AG_PEEK_SPLIT() *******************/

