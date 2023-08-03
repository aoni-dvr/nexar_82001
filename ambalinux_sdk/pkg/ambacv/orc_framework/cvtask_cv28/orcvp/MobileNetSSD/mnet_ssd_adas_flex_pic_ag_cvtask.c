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

#include "mnet_ssd_adas_flex_pic_ag_cvtask.h"
#include "mnet_ssd_adas_flex_pic_ag.h"

/*********************** STRUCTURES AND INITIALIZATION ************************/

/* Memory need */
struct DRAM_temporary_scratchpad {
    mnet_ssd_adas_flex_pic_ag_dram_t mnet_ssd_adas_flex_pic_ag;
};

struct CMEM_temporary_scratchpad {
    uint32_t seed_area[MAX_NUM_VP_OPS];
    mnet_ssd_adas_flex_pic_ag_cmem_t mnet_ssd_adas_flex_pic_ag;
};

struct instance_private_storage {
    mnet_ssd_adas_flex_pic_ag_ips_t mnet_ssd_adas_flex_pic_ag;
    mnet_ssd_adas_flex_pic_ag_required_fields_t mnet_ssd_adas_flex_pic_ag_r_args;
    mnet_ssd_adas_flex_pic_ag_optional_fields_t mnet_ssd_adas_flex_pic_ag_o_args;
    img_ctx_t img_ctx;
};

struct instance_private_uncached_storage {
    mnet_ssd_adas_flex_pic_ag_ipus_t mnet_ssd_adas_flex_pic_ag;
};

/* ORC CVTask Memory Requirements */
static cvtask_memory_interface_t mnet_ssd_adas_flex_pic_ag_memory_desc = {
    .num_inputs = 1,
    .input[0].io_name = "IDSP_PICINFO",

    .num_outputs = 2,
    .output[0].io_name = "MNET_SSD_ADAS_FLEX_PIC_AG_OUT_0",
    .output[0].buffer_size =
        sizeof(mnet_ssd_adas_flex_pic_ag_MNET_SSD_ADAS_FLEX_PIC_AG_OUT_0_t),
    .output[1].io_name = "MNET_SSD_ADAS_FLEX_PIC_AG_OUT_1",
    .output[1].buffer_size =
        sizeof(mnet_ssd_adas_flex_pic_ag_MNET_SSD_ADAS_FLEX_PIC_AG_OUT_1_t),

    .DRAM_temporary_scratchpad_needed = sizeof(DRAM_temporary_scratchpad_t),
    .CMEM_temporary_scratchpad_needed = sizeof(CMEM_temporary_scratchpad_t),
    .Instance_private_storage_needed = sizeof(instance_private_storage_t),
    .Instance_private_uncached_storage_needed =
        sizeof(instance_private_uncached_storage_t)
};

/******************************* CORE FUNCTIONS *******************************/
/* cvtask_query() callback implementation */
errcode_enum_t mnet_ssd_adas_flex_pic_ag_cvtask_query (
    uint32_t instanceId,
    const uint32_t *pSysConf,
    cvtask_memory_interface_t *pMemDesc
) {
    errcode_enum_t status = ERRCODE_NONE;

#ifdef CVTASK_PRINT
    VERBOSE(" >>> start querying. instance-id(0x%X) ---{" "mnet_ssd_adas_flex_pic_ag"
        "}---\n", instanceId, 0);
#endif

    if (pSysConf == NULL || pMemDesc == NULL) {
        status = ERRCODE_BAD_PARAMETER;
        DEBUG("   > Error: NULL pointer(s).\n", 0, 0);
        CRITICAL("  >> cvtask query failed!\n", 0, 0);
        return status;
    }

    /* Set up memory requirements for the ORC CVTask */
    // Initialization
    *pMemDesc = mnet_ssd_adas_flex_pic_ag_memory_desc;

#ifdef CVTASK_PRINT
    /* Print relevant info */
    DEBUG("  >> requesting for cvtask memory profile: context size(0x%08x) "
        "dram scratch(0x%08x)\n",
        (uint32_t) pMemDesc->Instance_private_storage_needed,
        (uint32_t) pMemDesc->DRAM_temporary_scratchpad_needed);
    DEBUG("                                           uncached buf(0x%08x) "
        "cmem scratch(0x%08x)\n",
        (uint32_t) pMemDesc->Instance_private_uncached_storage_needed,
        (uint32_t) pMemDesc->CMEM_temporary_scratchpad_needed);
    NORMAL(" >>> done querying. status(0x%X) ---{" "mnet_ssd_adas_flex_pic_ag"
        "}---\n", status, 0);
#endif

    return status;
}

/* cvtask_init() callback implementation */
errcode_enum_t mnet_ssd_adas_flex_pic_ag_cvtask_init (
    const cvtask_parameter_interface_t *pCVTaskEnv,
    const uint32_t *pSysConf
) {
    errcode_enum_t status = ERRCODE_NONE;
    instance_private_storage_t *ctxt;

#ifdef CVTASK_PRINT
    VERBOSE(" >>> start initializing. ---{" "mnet_ssd_adas_flex_pic_ag" "}---\n", 0, 0);
#endif

    if (pSysConf == NULL || pCVTaskEnv == NULL) {
        status = ERRCODE_BAD_PARAMETER;
        DEBUG("   > Error: NULL pointer(s).\n", 0, 0);
        CRITICAL("  >> cvtask init failed!\n", 0, 0);
        return status;
    }

    /* Set up context for ORC CVTask */
    // Recover private storage
    ctxt = pCVTaskEnv->vpInstance_private_storage;

    if (ctxt == NULL) {
        status = ERRCODE_BAD_PARAMETER;
        DEBUG("   > Error: NULL pointer(s).\n", 0, 0);
        CRITICAL("  >> cvtask init failed!\n", 0, 0);
        return status;
    }

    // Initialize VP-DAGs
    status = mnet_ssd_adas_flex_pic_ag_init(&ctxt->mnet_ssd_adas_flex_pic_ag);

    if (is_err(status)) {
        DEBUG("   > Error: mnet_ssd_adas_flex_pic_ag_init() failed.\n", 0, 0);
        CRITICAL("  >> cvtask init failed!\n", 0, 0);
        return status;
    }

    ctxt->img_ctx.idsp_pyramid_scale = pSysConf[0];
    ctxt->img_ctx.roi_start_col      = pSysConf[1];
    ctxt->img_ctx.roi_start_row      = pSysConf[2];
    ctxt->img_ctx.source_vin         = pSysConf[3];

#ifdef CVTASK_PRINT
    NORMAL(" >>> done initializing. ---{" "mnet_ssd_adas_flex_pic_ag" "}--- status(0x%08X)\n",
        status, 0);
#endif

    return status;
}

/* cvtask_process_run() callback implementation */
errcode_enum_t mnet_ssd_adas_flex_pic_ag_cvtask_run (
    const cvtask_parameter_interface_t *pCVTaskEnv
) {
#ifdef CVTASK_PRINT
    uint32_t cvt_run_start, cvt_run_finish;
#endif
    errcode_enum_t status = ERRCODE_NONE;
    DRAM_temporary_scratchpad_t *dram;
    CMEM_temporary_scratchpad_t *cmem;
    instance_private_storage_t *ctxt;
    instance_private_uncached_storage_t *ipus;
    mnet_ssd_adas_flex_pic_ag_required_fields_t *mnet_ssd_adas_flex_pic_ag_r_args;
    mnet_ssd_adas_flex_pic_ag_optional_fields_t *mnet_ssd_adas_flex_pic_ag_o_args;
    uint32_t base_ptr, buffer_ptr;
 
    uint32_t op_idx;

    if (pCVTaskEnv == NULL) {
        status = ERRCODE_BAD_PARAMETER;
        DEBUG("   > Error: NULL pointer(s).\n", 0, 0);
        CRITICAL("  >> cvtask run failed!\n", 0, 0);
        return status;
    }

#ifdef CVTASK_PRINT
    /* Print relevant info */
    mnet_ssd_adas_flex_pic_ag_print_debug_info(pCVTaskEnv);
#endif

    /* Set up context for ORC CVTask */
    // Recover memory containers
    dram = pCVTaskEnv->vpDRAM_temporary_scratchpad;
    cmem = pCVTaskEnv->vpCMEM_temporary_scratchpad;
    ctxt = pCVTaskEnv->vpInstance_private_storage;
    ipus = pCVTaskEnv->vpInstance_private_uncached_storage;
    mnet_ssd_adas_flex_pic_ag_r_args = &ctxt->mnet_ssd_adas_flex_pic_ag_r_args;
    mnet_ssd_adas_flex_pic_ag_o_args = &ctxt->mnet_ssd_adas_flex_pic_ag_o_args;

    if (cmem == NULL || ctxt == NULL) {
        status = ERRCODE_BAD_PARAMETER;
        DEBUG("   > Error: NULL pointer(s).\n", 0, 0);
        CRITICAL("  >> cvtask run failed!\n", 0, 0);
        return status;
    }

    mnet_ssd_adas_flex_pic_ag_cvtask_itc(pCVTaskEnv);
    status = mnet_ssd_adas_flex_pic_ag_roi_handling(pCVTaskEnv->vpInputBuffer[0], ctxt);
    if (is_err(status)) {
        DEBUG("   > Error: roi_handling() failed.\n", 0, 0);
        CRITICAL("  >> cvtask run failed!\n", 0, 0);
        return status;
    }
    for (op_idx = 0; op_idx < MAX_NUM_VP_OPS; op_idx++)
        cmem->seed_area[op_idx] = 1U;
    dma_cmem_2_coproc(
        0x100U,
        (uint32_t) &cmem->seed_area,
        sizeof(cmem->seed_area)
    );

    /* Fill in required fields */
    // Memory instances
    mnet_ssd_adas_flex_pic_ag_r_args->DRAM_temporary_scratchpad = &dram->mnet_ssd_adas_flex_pic_ag;
    mnet_ssd_adas_flex_pic_ag_r_args->CMEM_temporary_scratchpad = &cmem->mnet_ssd_adas_flex_pic_ag;
    mnet_ssd_adas_flex_pic_ag_r_args->instance_private_uncached_storage = &ipus->mnet_ssd_adas_flex_pic_ag;

    // Input buffers
    //     MNET_SSD_ADAS_FLEX_PIC_AG_INP_0
    base_ptr = ctxt->img_ctx.daddr_img_y_base;
    //         data
    mnet_ssd_adas_flex_pic_ag_r_args->data_addr = (uint32_t) &((mnet_ssd_adas_flex_pic_ag_MNET_SSD_ADAS_FLEX_PIC_AG_INP_0_t *) base_ptr)->data[0][0];
;
    //     MNET_SSD_ADAS_FLEX_PIC_AG_INP_1
    base_ptr = ctxt->img_ctx.daddr_img_uv_base;
    //         data_uv
    mnet_ssd_adas_flex_pic_ag_r_args->data_uv_addr = (uint32_t) &((mnet_ssd_adas_flex_pic_ag_MNET_SSD_ADAS_FLEX_PIC_AG_INP_1_t *) base_ptr)->data_uv[0][0];
;

    // Output buffers
    //     MNET_SSD_ADAS_FLEX_PIC_AG_OUT_0
    base_ptr = (uint32_t) pCVTaskEnv->vpOutputBuffer[0];
    //         mbox_loc
    buffer_ptr = (uint32_t) &((mnet_ssd_adas_flex_pic_ag_MNET_SSD_ADAS_FLEX_PIC_AG_OUT_0_t *) base_ptr)->mbox_loc[0][0];
    mnet_ssd_adas_flex_pic_ag_r_args->mbox_loc_addr = (uint32_t) buffer_ptr;
    //     MNET_SSD_ADAS_FLEX_PIC_AG_OUT_1
    base_ptr = (uint32_t) pCVTaskEnv->vpOutputBuffer[1];
    //         mbox_conf_flatten
    buffer_ptr = (uint32_t) &((mnet_ssd_adas_flex_pic_ag_MNET_SSD_ADAS_FLEX_PIC_AG_OUT_1_t *) base_ptr)->mbox_conf_flatten[0][0];
    mnet_ssd_adas_flex_pic_ag_r_args->mbox_conf_flatten_addr = (uint32_t) buffer_ptr;

    /* Fill in optional fields */
    mnet_ssd_adas_flex_pic_ag_o_args->data_dram_pitch_enable = (uint32_t) (1);
    mnet_ssd_adas_flex_pic_ag_o_args->data_dram_pitch_value = (uint32_t) (ctxt->img_ctx.dpitch_m1_img_y);
    mnet_ssd_adas_flex_pic_ag_o_args->data_uv_dram_pitch_enable = (uint32_t) (1);
    mnet_ssd_adas_flex_pic_ag_o_args->data_uv_dram_pitch_value = (uint32_t) (ctxt->img_ctx.dpitch_m1_img_uv);

#ifdef CVTASK_PRINT
    cvt_run_start = get_cur_time();
    VERBOSE(" >>> start running ---{" "mnet_ssd_adas_flex_pic_ag" "}---. @start-cycle=(%u)\n",
        cvt_run_start, 0);
#endif

    // Run VP-DAG
    status = mnet_ssd_adas_flex_pic_ag_run(
        &ctxt->mnet_ssd_adas_flex_pic_ag,
        mnet_ssd_adas_flex_pic_ag_r_args,
        mnet_ssd_adas_flex_pic_ag_o_args
    );

#ifdef CVTASK_PRINT
    cvt_run_finish = get_cur_time();
    cvtask_printf(LVL_MINIMAL, " >>> done running ---{" "mnet_ssd_adas_flex_pic_ag"
        "}--- @end-cycle=(%u). total-time=[%u] status=(0x%08X)\n",
        cvt_run_finish, (cvt_run_finish - cvt_run_start), status, 0, 0);
#endif

    return status;
}

/* cvtask_process_messages() callback implementation */
errcode_enum_t mnet_ssd_adas_flex_pic_ag_cvtask_itc (
    const cvtask_parameter_interface_t *pCVTaskEnv
) {
    errcode_enum_t status = ERRCODE_NONE;

    if (pCVTaskEnv == NULL) {
        status = ERRCODE_BAD_PARAMETER;
        DEBUG("   > Error: NULL pointer(s).\n", 0, 0);
        CRITICAL("  >> cvtask itc failed!\n", 0, 0);
        return status;
    }

    instance_private_storage_t *ctxt = pCVTaskEnv->vpInstance_private_storage;

    if (ctxt == NULL) {
        status = ERRCODE_BAD_PARAMETER;
        DEBUG("   > Error: NULL pointer(s).\n", 0, 0);
        CRITICAL("  >> cvtask itc failed!\n", 0, 0);
        return status;
    }

    uint32_t i;
    uint32_t msg_count = pCVTaskEnv->cvtask_num_messages;
    amba_roi_config_t *p_cfg_msg;
    img_ctx_t *img_ctx = &ctxt->img_ctx;

    if (msg_count == 0) {
        DEBUG(" >>> No messages to process\n", 0, 0);
    }
    else {
        for (i = 0; i < msg_count; i++) {
            p_cfg_msg = (amba_roi_config_t*) pCVTaskEnv->vpMessagePayload[i];
            DEBUG(" >>> Processing  message: %u, type: %u\n",
                i, p_cfg_msg->msg_type);

            if (p_cfg_msg->msg_type == AMBA_ROI_CONFIG_MSG) {
                img_ctx->idsp_pyramid_scale = p_cfg_msg->image_pyramid_index;
                img_ctx->roi_start_col      = p_cfg_msg->roi_start_col;
                img_ctx->roi_start_row      = p_cfg_msg->roi_start_row;
                img_ctx->source_vin         = p_cfg_msg->source_vin;

                cvtask_printf(LVL_DEBUG, " >>> Processing  message: scale=%u, "
                    "x_offset=%u, y_offset=%u, vin=%u\n",
                    img_ctx->idsp_pyramid_scale,
                    img_ctx->roi_start_col,
                    img_ctx->roi_start_row,
                    img_ctx->source_vin,
                    0
                );
            }
        }
    }

    return status;
}

/******************************* HELPER FUNCTION ******************************/
static void mnet_ssd_adas_flex_pic_ag_print_debug_info(
    const cvtask_parameter_interface_t *pCVTaskEnv
) {
    DEBUG(" >>> start dumping runtime context of ---{" "mnet_ssd_adas_flex_pic_ag" "}---"
        " instance #[%02u]\n",
        pCVTaskEnv->cvtask_instance, 0);

    /* dump input buffers info*/
    DEBUG("  >> Input Buffers: Number=(#%u)\n",
        pCVTaskEnv->cvtask_num_inputs, 0);
    cvtask_printf(LVL_DEBUG, "   > Buffer Addresses : %08X, %08X, %08X, %08X\n",
        (uint32_t)pCVTaskEnv->vpInputBuffer[0],
        (uint32_t)pCVTaskEnv->vpInputBuffer[1],
        (uint32_t)pCVTaskEnv->vpInputBuffer[2],
        (uint32_t)pCVTaskEnv->vpInputBuffer[3],
        0
    );
    cvtask_printf(LVL_DEBUG, "                      : %08X, %08X, %08X, %08X\n",
        (uint32_t)pCVTaskEnv->vpInputBuffer[4],
        (uint32_t)pCVTaskEnv->vpInputBuffer[5],
        (uint32_t)pCVTaskEnv->vpInputBuffer[6],
        (uint32_t)pCVTaskEnv->vpInputBuffer[7],
        0
    );
    cvtask_printf(LVL_DEBUG, "                      : %08X, %08X, %08X, %08X\n",
        (uint32_t)pCVTaskEnv->vpInputBuffer[8],
        (uint32_t)pCVTaskEnv->vpInputBuffer[9],
        (uint32_t)pCVTaskEnv->vpInputBuffer[10],
        (uint32_t)pCVTaskEnv->vpInputBuffer[11],
        0
    );
    cvtask_printf(LVL_DEBUG, "                      : %08X, %08X, %08X, %08X\n",
        (uint32_t)pCVTaskEnv->vpInputBuffer[12],
        (uint32_t)pCVTaskEnv->vpInputBuffer[13],
        (uint32_t)pCVTaskEnv->vpInputBuffer[14],
        (uint32_t)pCVTaskEnv->vpInputBuffer[15],
        0
    );

    /* dump output buffers info*/
    DEBUG("  >> Output Buffers: Number=(#%u)\n",
        pCVTaskEnv->cvtask_num_outputs, 0);
    cvtask_printf(LVL_DEBUG, "   > Buffer Addresses : %08X, %08X, %08X, %08X\n",
        (uint32_t)pCVTaskEnv->vpOutputBuffer[0],
        (uint32_t)pCVTaskEnv->vpOutputBuffer[1],
        (uint32_t)pCVTaskEnv->vpOutputBuffer[2],
        (uint32_t)pCVTaskEnv->vpOutputBuffer[3],
        0
    );
    cvtask_printf(LVL_DEBUG, "                      : %08X, %08X, %08X, %08X\n",
        (uint32_t)pCVTaskEnv->vpOutputBuffer[4],
        (uint32_t)pCVTaskEnv->vpOutputBuffer[5],
        (uint32_t)pCVTaskEnv->vpOutputBuffer[6],
        (uint32_t)pCVTaskEnv->vpOutputBuffer[7],
        0
    );

    /* dump feedback buffers info*/
    DEBUG("  >> Feedback Buffers: Number=(#%u)\n",
        pCVTaskEnv->cvtask_num_feedback, 0);
    cvtask_printf(LVL_DEBUG, "   > Buffer Addresses : %08X, %08X, %08X, %08X\n",
        (uint32_t)pCVTaskEnv->vpFeedbackBuffer[0],
        (uint32_t)pCVTaskEnv->vpFeedbackBuffer[1],
        (uint32_t)pCVTaskEnv->vpFeedbackBuffer[2],
        (uint32_t)pCVTaskEnv->vpFeedbackBuffer[3],
        0
    );

    /* dump message payload buffers info*/
    DEBUG("  >> Message Payload Buffers: Number=(#%u)\n",
        pCVTaskEnv->cvtask_num_messages, 0);
    cvtask_printf(LVL_DEBUG, "   > Buffer Addresses : %08X, %08X, %08X, %08X\n",
        (uint32_t)pCVTaskEnv->vpMessagePayload[0],
        (uint32_t)pCVTaskEnv->vpMessagePayload[1],
        (uint32_t)pCVTaskEnv->vpMessagePayload[2],
        (uint32_t)pCVTaskEnv->vpMessagePayload[3],
        0
    );
    cvtask_printf(LVL_DEBUG, "                      : %08X, %08X, %08X, %08X\n",
        (uint32_t)pCVTaskEnv->vpMessagePayload[4],
        (uint32_t)pCVTaskEnv->vpMessagePayload[5],
        (uint32_t)pCVTaskEnv->vpMessagePayload[6],
        (uint32_t)pCVTaskEnv->vpMessagePayload[7],
        0
    );
    cvtask_printf(LVL_DEBUG, "                      : %08X, %08X, %08X, %08X\n",
        (uint32_t)pCVTaskEnv->vpMessagePayload[8],
        (uint32_t)pCVTaskEnv->vpMessagePayload[9],
        (uint32_t)pCVTaskEnv->vpMessagePayload[10],
        (uint32_t)pCVTaskEnv->vpMessagePayload[11],
        0
    );
    cvtask_printf(LVL_DEBUG, "                      : %08X, %08X, %08X, %08X\n",
        (uint32_t)pCVTaskEnv->vpMessagePayload[12],
        (uint32_t)pCVTaskEnv->vpMessagePayload[13],
        (uint32_t)pCVTaskEnv->vpMessagePayload[14],
        (uint32_t)pCVTaskEnv->vpMessagePayload[15],
        0
    );

    /* dump CMEM scratch pad memory info */
    if (pCVTaskEnv->vpCMEM_temporary_scratchpad) {
        DEBUG("  >> CMEM Scratchpad: Base-(%08X), Size-(%9u)\n",
            (uint32_t)pCVTaskEnv->vpCMEM_temporary_scratchpad,
            pCVTaskEnv->CMEM_temporary_scratchpad_size
        );
    }

    /* dump DRAM scratch pad memory info */
    if (pCVTaskEnv->vpDRAM_temporary_scratchpad) {
        DEBUG("  >> DRAM Scratchpad: Base-(%08X), Size-(%9u)\n",
            (uint32_t)pCVTaskEnv->vpDRAM_temporary_scratchpad,
            pCVTaskEnv->DRAM_temporary_scratchpad_size
        );
    }

    /* dump Private Storage info */
    if (pCVTaskEnv->vpInstance_private_storage) {
        DEBUG("  >> Private Storage: Base-(%08X), Size-(%9u)\n",
            (uint32_t)pCVTaskEnv->vpInstance_private_storage,
            pCVTaskEnv->Instance_private_storage_size
        );
    }

    /* dump Uncached-Private Storage info */
    if (pCVTaskEnv->vpInstance_private_uncached_storage) {
        DEBUG("  >> Uncached Private Storage: Base-(%08X), Size-(%9u)\n",
            (uint32_t)pCVTaskEnv->vpInstance_private_uncached_storage,
            pCVTaskEnv->Instance_private_uncached_storage_size
        );
    }

    /* dump Shared Storage info */
    if (pCVTaskEnv->vpCVTask_shared_storage) {
        DEBUG("  >> Shared Storage: Base-(%08X), Size-(%9u)\n",
            (uint32_t)pCVTaskEnv->vpCVTask_shared_storage,
            pCVTaskEnv->CVTask_shared_storage_size
        );
    }

    DEBUG(" >>> done dumping runtime context of ---{" "mnet_ssd_adas_flex_pic_ag" "}---\n",
        0, 0);
}

static errcode_enum_t mnet_ssd_adas_flex_pic_ag_roi_handling(
    cv_pic_info_t *idsp_pic_info,
    instance_private_storage_t *ctxt
) {
    uint32_t roi_w_m1, roi_h_m1, luma, chroma, pitch;
    uint32_t network_input_width = 300;
    uint32_t network_input_height = 300;
    uint32_t p_scale = ctxt->img_ctx.idsp_pyramid_scale;
    img_ctx_t *img_ctx = &ctxt->img_ctx;
    win_ctx_t win_ctx = {0};

    roi_w_m1 = idsp_pic_info->pyramid.half_octave[p_scale].roi_width_m1;
    roi_h_m1 = idsp_pic_info->pyramid.half_octave[p_scale].roi_height_m1;

    if (img_ctx->roi_start_col == 9999)
        img_ctx->roi_start_col = (roi_w_m1 + 1 - network_input_width) / 2;
    else if (img_ctx->roi_start_col < 0)
        img_ctx->roi_start_col = 0;
    else if (img_ctx->roi_start_col > (roi_w_m1 + 1 - network_input_width))
        img_ctx->roi_start_col = (roi_w_m1 + 1 - network_input_width);

    if (img_ctx->roi_start_row == 9999)
        img_ctx->roi_start_row = (roi_h_m1 + 1 - network_input_height) / 2;
    else if (img_ctx->roi_start_row < 0)
        img_ctx->roi_start_row = 0;
    else if (img_ctx->roi_start_row > (roi_h_m1 + 1 - network_input_height))
        img_ctx->roi_start_row = (roi_h_m1 + 1 - network_input_height);

    cvtask_printf(LVL_DEBUG, "   > roi_handling: pyramid_scale = %d, "
        "start_col = %d, start_row= %d\n",
        p_scale, img_ctx->roi_start_col, img_ctx->roi_start_row, 0, 0);

    if (img_ctx->source_vin == 1) {
        luma   = idsp_pic_info->rpLumaLeft[p_scale];
        chroma = idsp_pic_info->rpChromaLeft[p_scale];
    } else {
        luma   = idsp_pic_info->rpLumaRight[p_scale];
        chroma = idsp_pic_info->rpChromaRight[p_scale];
    }
    pitch = idsp_pic_info->pyramid.half_octave[p_scale].ctrl.roi_pitch;

    img_ctx->daddr_img_y_base  = ((uint32_t) idsp_pic_info) + luma +
        img_ctx->roi_start_row * pitch + img_ctx->roi_start_col;
    img_ctx->daddr_img_uv_base = ((uint32_t) idsp_pic_info) + chroma +
        (img_ctx->roi_start_row & 0xfffffffe) * pitch / 2 +
        (img_ctx->roi_start_col & 0xfffffffe);
    img_ctx->dpitch_m1_img_y   = pitch - 1;
    img_ctx->dpitch_m1_img_uv  = pitch - 1;

    cvtask_printf(LVL_DEBUG, "   > roi_handling: dpitch_m1 = %d, roi_w_m1 = %d,"
        " roi_h_m1 = %d\n", img_ctx->dpitch_m1_img_y, roi_w_m1, roi_h_m1, 0, 0);

    win_ctx.type = AMBANET_MSG_TYPE_OD_WINDOWS;

    win_ctx.raw_w = idsp_pic_info->pyramid.half_octave[0].roi_width_m1 + 1;
    win_ctx.raw_h = idsp_pic_info->pyramid.half_octave[0].roi_height_m1 + 1;
    win_ctx.roi_w = roi_w_m1 + 1;
    win_ctx.roi_h = roi_h_m1 + 1;

    win_ctx.roi_start_col    = img_ctx->roi_start_col;
    win_ctx.roi_start_row    = img_ctx->roi_start_row;
    win_ctx.net_in_w         = network_input_width;
    win_ctx.net_in_h         = network_input_height;

    return ERRCODE_NONE;
}

/******************************** BOILERPLATE *********************************/

/* cvtask_get_info() callback implementation */
errcode_enum_t mnet_ssd_adas_flex_pic_ag_cvtask_get_info (
    const cvtask_parameter_interface_t *pCVTaskEnv,
    uint32_t infoIndex,
    void *pInfoBuf
) {
    errcode_enum_t status = ERRCODE_NONE;

    char *tool_ver = "cv2.qa.2020-09-07.779";
    char *gentask_ver = "CVTask Autogen v3";

    if (pInfoBuf == NULL) {
        status = ERRCODE_CVTASK_BASE;
        CRITICAL(" >>> get_info : invalid pInfoBuf(0x%08x)\n",
            (uint32_t)pInfoBuf, 0);
    }
    else {
        if (infoIndex == 0) {
            uint32_t *recast32;
            recast32    = (uint32_t *)pInfoBuf;
            recast32[0] = (uint32_t)gentask_ver;
        }
        else if (infoIndex == 1) {
            uint32_t *recast32;
            recast32    = (uint32_t *)pInfoBuf;
            recast32[0] = (uint32_t)tool_ver;
        }
    }

    return status;
}

/* cvtask_finish() callback implementation */
errcode_enum_t mnet_ssd_adas_flex_pic_ag_cvtask_finish (
    const cvtask_parameter_interface_t *pCVTaskEnv
) {
    return ERRCODE_NONE;
}


/* cvtask_register() callback implementation */
errcode_enum_t mnet_ssd_adas_flex_pic_ag_cvtask_register (
    cvtask_entry_t *pCVTaskEntry
) {
#ifdef CVTASK_PRINT
    DEBUG(">>> start registering # ---{" "mnet_ssd_adas_flex_pic_ag" "}---\n", 0, 0);
#endif

    visorc_strcpy(pCVTaskEntry->cvtask_name, "mnet_ssd_adas_flex_pic_ag", NAME_MAX_LENGTH);
    pCVTaskEntry->cvtask_type            = CVTASK_TYPE_VPANY;
    pCVTaskEntry->cvtask_api_version     = CVTASK_API_VERSION;
    pCVTaskEntry->cvtask_query           = &mnet_ssd_adas_flex_pic_ag_cvtask_query;
    pCVTaskEntry->cvtask_init            = &mnet_ssd_adas_flex_pic_ag_cvtask_init;
    pCVTaskEntry->cvtask_get_info        = &mnet_ssd_adas_flex_pic_ag_cvtask_get_info;
    pCVTaskEntry->cvtask_run             = &mnet_ssd_adas_flex_pic_ag_cvtask_run;
    pCVTaskEntry->cvtask_finish          = &mnet_ssd_adas_flex_pic_ag_cvtask_finish;
    pCVTaskEntry->cvtask_process_messages= &mnet_ssd_adas_flex_pic_ag_cvtask_itc;

#ifdef CVTASK_PRINT
    DEBUG(">>> done registering # ---{" "mnet_ssd_adas_flex_pic_ag" "}---\n", 0, 0);
#endif

    return ERRCODE_NONE;
}

