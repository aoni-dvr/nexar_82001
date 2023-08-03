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

#include "flexidag_openseg_ag_cvtask.h"
#include "flexidag_openseg_ag.h"

/*********************** STRUCTURES AND INITIALIZATION ************************/

/* Memory need */
struct DRAM_temporary_scratchpad {
    flexidag_openseg_ag_dram_t flexidag_openseg_ag;
};

struct CMEM_temporary_scratchpad {
    uint32_t seed_area[MAX_NUM_VP_OPS];
    flexidag_openseg_ag_cmem_t flexidag_openseg_ag;
};

struct instance_private_storage {
    flexidag_openseg_ag_ips_t flexidag_openseg_ag;
    flexidag_openseg_ag_required_fields_t flexidag_openseg_ag_r_args;
    flexidag_openseg_ag_optional_fields_t flexidag_openseg_ag_o_args;
};

struct instance_private_uncached_storage {
    flexidag_openseg_ag_ipus_t flexidag_openseg_ag;
};

/* ORC CVTask Memory Requirements */
static cvtask_memory_interface_t flexidag_openseg_ag_memory_desc = {
    .num_inputs = 2,
    .input[0].io_name = "FLEXIDAG_OPENSEG_AG_INP_0",
    .input[1].io_name = "FLEXIDAG_OPENSEG_AG_INP_1",

    .num_outputs = 1,
    .output[0].io_name = "FLEXIDAG_OPENSEG_AG_OUT_0",
    .output[0].buffer_size =
        sizeof(flexidag_openseg_ag_FLEXIDAG_OPENSEG_AG_OUT_0_t),

    .DRAM_temporary_scratchpad_needed = sizeof(DRAM_temporary_scratchpad_t),
    .CMEM_temporary_scratchpad_needed = sizeof(CMEM_temporary_scratchpad_t),
    .Instance_private_storage_needed = sizeof(instance_private_storage_t),
    .Instance_private_uncached_storage_needed =
        sizeof(instance_private_uncached_storage_t),
};

/******************************* CORE FUNCTIONS *******************************/
/* cvtask_query() callback implementation */
errcode_enum_t flexidag_openseg_ag_cvtask_query (
    uint32_t instanceId,
    const uint32_t *pSysConf,
    cvtask_memory_interface_t *pMemDesc
) {
    errcode_enum_t status = ERRCODE_NONE;

#ifdef CVTASK_PRINT
    VERBOSE(" >>> start querying. instance-id(0x%X) ---{" "flexidag_openseg_ag"
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
    *pMemDesc = flexidag_openseg_ag_memory_desc;

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
    NORMAL(" >>> done querying. status(0x%X) ---{" "flexidag_openseg_ag"
        "}---\n", status, 0);
#endif

    return status;
}

/* cvtask_init() callback implementation */
errcode_enum_t flexidag_openseg_ag_cvtask_init (
    const cvtask_parameter_interface_t *pCVTaskEnv,
    const uint32_t *pSysConf
) {
    errcode_enum_t status = ERRCODE_NONE;
    instance_private_storage_t *ctxt;

#ifdef CVTASK_PRINT
    VERBOSE(" >>> start initializing. ---{" "flexidag_openseg_ag" "}---\n", 0, 0);
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
    status = flexidag_openseg_ag_init(&ctxt->flexidag_openseg_ag);

    if (is_err(status)) {
        DEBUG("   > Error: flexidag_openseg_ag_init() failed.\n", 0, 0);
        CRITICAL("  >> cvtask init failed!\n", 0, 0);
        return status;
    }

#ifdef CVTASK_PRINT
    NORMAL(" >>> done initializing. ---{" "flexidag_openseg_ag" "}--- status(0x%08X)\n",
        status, 0);
#endif

    return status;
}

/* cvtask_process_run() callback implementation */
errcode_enum_t flexidag_openseg_ag_cvtask_run (
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
    flexidag_openseg_ag_required_fields_t *flexidag_openseg_ag_r_args;
    flexidag_openseg_ag_optional_fields_t *flexidag_openseg_ag_o_args;
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
    flexidag_openseg_ag_print_debug_info(pCVTaskEnv);
#endif

    /* Set up context for ORC CVTask */
    // Recover memory containers
    dram = pCVTaskEnv->vpDRAM_temporary_scratchpad;
    cmem = pCVTaskEnv->vpCMEM_temporary_scratchpad;
    ctxt = pCVTaskEnv->vpInstance_private_storage;
    ipus = pCVTaskEnv->vpInstance_private_uncached_storage;
    flexidag_openseg_ag_r_args = &ctxt->flexidag_openseg_ag_r_args;
    flexidag_openseg_ag_o_args = &ctxt->flexidag_openseg_ag_o_args;

    if (cmem == NULL || ctxt == NULL) {
        status = ERRCODE_BAD_PARAMETER;
        DEBUG("   > Error: NULL pointer(s).\n", 0, 0);
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
    flexidag_openseg_ag_r_args->DRAM_temporary_scratchpad = &dram->flexidag_openseg_ag;
    flexidag_openseg_ag_r_args->CMEM_temporary_scratchpad = &cmem->flexidag_openseg_ag;
    flexidag_openseg_ag_r_args->instance_private_uncached_storage = &ipus->flexidag_openseg_ag;

    // Input buffers
    //     FLEXIDAG_OPENSEG_AG_INP_0
    memio_source_recv_raw_t * recv_raw_FLEXIDAG_OPENSEG_AG_INP_0;
    recv_raw_FLEXIDAG_OPENSEG_AG_INP_0 = (memio_source_recv_raw_t *) pCVTaskEnv->vpInputBuffer[0];
    base_ptr = (uint32_t) recv_raw_FLEXIDAG_OPENSEG_AG_INP_0->addr;
    //         Placeholder
    flexidag_openseg_ag_r_args->Placeholder_addr = (uint32_t) &((flexidag_openseg_ag_FLEXIDAG_OPENSEG_AG_INP_0_t *) base_ptr)->Placeholder[0][0];
;
    //     FLEXIDAG_OPENSEG_AG_INP_1
    memio_source_recv_raw_t * recv_raw_FLEXIDAG_OPENSEG_AG_INP_1;
    recv_raw_FLEXIDAG_OPENSEG_AG_INP_1 = (memio_source_recv_raw_t *) pCVTaskEnv->vpInputBuffer[1];
    base_ptr = (uint32_t) recv_raw_FLEXIDAG_OPENSEG_AG_INP_1->addr;
    //         Placeholder_uv
    flexidag_openseg_ag_r_args->Placeholder_uv_addr = (uint32_t) &((flexidag_openseg_ag_FLEXIDAG_OPENSEG_AG_INP_1_t *) base_ptr)->Placeholder_uv[0][0];
;

    // Output buffers
    //     FLEXIDAG_OPENSEG_AG_OUT_0
    base_ptr = (uint32_t) pCVTaskEnv->vpOutputBuffer[0];
    //         out_class
    buffer_ptr = (uint32_t) &((flexidag_openseg_ag_FLEXIDAG_OPENSEG_AG_OUT_0_t *) base_ptr)->out_class[0][0];
    flexidag_openseg_ag_r_args->out_class_addr = (uint32_t) buffer_ptr;

    /* Fill in optional fields */
    flexidag_openseg_ag_o_args->Placeholder_dram_pitch_enable = (uint32_t) (recv_raw_FLEXIDAG_OPENSEG_AG_INP_0->pitch != 0);
    flexidag_openseg_ag_o_args->Placeholder_dram_pitch_value = (uint32_t) (recv_raw_FLEXIDAG_OPENSEG_AG_INP_0->pitch - 1);
    flexidag_openseg_ag_o_args->Placeholder_uv_dram_pitch_enable = (uint32_t) (recv_raw_FLEXIDAG_OPENSEG_AG_INP_1->pitch != 0);
    flexidag_openseg_ag_o_args->Placeholder_uv_dram_pitch_value = (uint32_t) (recv_raw_FLEXIDAG_OPENSEG_AG_INP_1->pitch - 1);

#ifdef CVTASK_PRINT
    cvt_run_start = get_cur_time();
    VERBOSE(" >>> start running ---{" "flexidag_openseg_ag" "}---. @start-cycle=(%u)\n",
        cvt_run_start, 0);
#endif

    // Run VP-DAG
    status = flexidag_openseg_ag_run(
        &ctxt->flexidag_openseg_ag,
        flexidag_openseg_ag_r_args,
        flexidag_openseg_ag_o_args
    );

#ifdef CVTASK_PRINT
    cvt_run_finish = get_cur_time();
    cvtask_printf(LVL_MINIMAL, " >>> done running ---{" "flexidag_openseg_ag"
        "}--- @end-cycle=(%u). total-time=[%u] status=(0x%08X)\n",
        cvt_run_finish, (cvt_run_finish - cvt_run_start), status, 0, 0);
#endif


    return status;
}

/******************************* HELPER FUNCTION ******************************/
static void flexidag_openseg_ag_print_debug_info(
    const cvtask_parameter_interface_t *pCVTaskEnv
) {
    DEBUG(" >>> start dumping runtime context of ---{" "flexidag_openseg_ag" "}---"
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

    DEBUG(" >>> done dumping runtime context of ---{" "flexidag_openseg_ag" "}---\n",
        0, 0);
}

/******************************** BOILERPLATE *********************************/

/* cvtask_get_info() callback implementation */
errcode_enum_t flexidag_openseg_ag_cvtask_get_info (
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
errcode_enum_t flexidag_openseg_ag_cvtask_finish (
    const cvtask_parameter_interface_t *pCVTaskEnv
) {
    return ERRCODE_NONE;
}

/* cvtask_process_messages() callback implementation */
errcode_enum_t flexidag_openseg_ag_cvtask_itc (
    const cvtask_parameter_interface_t *pCVTaskEnv
) {
    errcode_enum_t retcode = ERRCODE_NONE;
    uint32_t *pframe;

	pframe = (uint32_t *)pCVTaskEnv->vpFeedbackBuffer[0];
	if(pframe != NULL) {
		if((*pframe + 1U) != (uint32_t)(pCVTaskEnv->cvtask_frameset_id)) {
			retcode = CVTASK_ERR_GENERAL;
            CVTASK_PRINTF(LVL_DEBUG, ">>>>> bisenet_mnv2_raw_prim_cvtask_run():  error : current frame @ #0x%x, feedback frame @ #0x%x", pCVTaskEnv->cvtask_frameset_id, *pframe, 0, 0, 0);
		} else {
			CVTASK_PRINTF(LVL_DEBUG, ">>>>> bisenet_mnv2_raw_prim_cvtask_run(): current frame @ #0x%x, feedback frame @ #0x%x", pCVTaskEnv->cvtask_frameset_id, *pframe, 0, 0, 0);
		}
	}
    return retcode;
}

/* cvtask_register() callback implementation */
errcode_enum_t flexidag_openseg_ag_cvtask_register (
    cvtask_entry_t *pCVTaskEntry
) {
#ifdef CVTASK_PRINT
    DEBUG(">>> start registering # ---{" "flexidag_openseg_ag" "}---\n", 0, 0);
#endif

    visorc_strcpy(pCVTaskEntry->cvtask_name, "flexidag_openseg_ag", NAME_MAX_LENGTH);
    pCVTaskEntry->cvtask_type            = CVTASK_TYPE_VPANY;
    pCVTaskEntry->cvtask_api_version     = CVTASK_API_VERSION;
    pCVTaskEntry->cvtask_query           = &flexidag_openseg_ag_cvtask_query;
    pCVTaskEntry->cvtask_init            = &flexidag_openseg_ag_cvtask_init;
    pCVTaskEntry->cvtask_get_info        = &flexidag_openseg_ag_cvtask_get_info;
    pCVTaskEntry->cvtask_run             = &flexidag_openseg_ag_cvtask_run;
    pCVTaskEntry->cvtask_finish          = &flexidag_openseg_ag_cvtask_finish;
    pCVTaskEntry->cvtask_process_messages= &flexidag_openseg_ag_cvtask_itc;

#ifdef CVTASK_PRINT
    DEBUG(">>> done registering # ---{" "flexidag_openseg_ag" "}---\n", 0, 0);
#endif

    return ERRCODE_NONE;
}

