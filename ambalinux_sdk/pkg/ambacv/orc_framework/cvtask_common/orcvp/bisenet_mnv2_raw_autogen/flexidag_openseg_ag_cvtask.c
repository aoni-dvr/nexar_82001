/*
* Copyright (c) 2017-2019 Ambarella, Inc.
*
* This file and its contents ("Software") are protected by intellectual property rights including,
* without limitation, U.S. and/or foreign copyrights.  This Software is also the confidential and
* proprietary information of Ambarella, Inc. and its licensors.  You may not use, reproduce, disclose,
* distribute, modify, or otherwise prepare derivative works of this Software or any portion thereof
* except pursuant to a signed license agreement or nondisclosure agreement with Ambarella, Inc. or
* its authorized affiliates.  In the absence of such an agreement, you agree to promptly notify and
* return this Software to Ambarella, Inc.
*
* THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
* TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
* OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR
* BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
* LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "flexidag_openseg_ag_cvtask.h"
#include "build_version.h"

/* ORC CVTask Context */
static struct flexidag_openseg_ag_cvtask_ctxt *pgCVTaskContext = NULL;

/* ORC CVTask memory requirements */
static cvtask_memory_interface_t flexidag_openseg_ag_memory_desc = {
    .num_inputs = 2,
    // -- input to operator-node (['Placeholder']) will be read from port (FLEXIDAG_OPENSEG_AG_INP_0)
    .input[0].io_name = "FLEXIDAG_OPENSEG_AG_INP_0",
    // -- input to operator-node (['Placeholder_uv']) will be read from port (FLEXIDAG_OPENSEG_AG_INP_1)
    .input[1].io_name = "FLEXIDAG_OPENSEG_AG_INP_1",

    .num_outputs = 1,
    // -- outputs from operator-nodes (['out_class']) will be written to port (FLEXIDAG_OPENSEG_AG_OUT_0)
    .output[0].io_name = "FLEXIDAG_OPENSEG_AG_OUT_0",
};

/* cvtask_query() callback implementation */
errcode_enum_t flexidag_openseg_ag_cvtask_query (
    uint32_t instanceId,
    const uint32_t *pSysConf,
    cvtask_memory_interface_t *pMemDesc
) {
    errcode_enum_t status = ERRCODE_NONE;

    ILOG(" >>> start querying. instance-id(0x%X) ---{" ORCCVTASK_NAME "}---", instanceId, 0, 0, 0, 0);

    if (pSysConf == NULL || pMemDesc == NULL)
        status = ERRCODE_BAD_PARAMETER;

    /* setup memory requirements for the ORC CVTask */
    if (is_not_err(status))
    {
        *pMemDesc = flexidag_openseg_ag_memory_desc;
        pMemDesc->DRAM_temporary_scratchpad_needed = 0;
        pMemDesc->Instance_private_uncached_storage_needed = 68812800;
        pMemDesc->Instance_private_storage_needed = sizeof(struct flexidag_openseg_ag_cvtask_ctxt);

        ILOG("  >> requesting for cvtask memory profile: context size(0x%08X) dram scratch(0x%08x)",
                pMemDesc->Instance_private_storage_needed,
                pMemDesc->DRAM_temporary_scratchpad_needed,
                0, 0, 0);

        // %jinja2% generated code using the variable dictionary - (primary_ports['outs'])
        // allocate required output buffer sizes also considering user requests
        
        // -- requesting memory for port FLEXIDAG_OPENSEG_AG_OUT_0 with operator-nodes ['out_class']
        pMemDesc->output[0].buffer_size = 40960;
        
    }
    else
    {
        CLOG("  >> cvtask query failed!", 0, 0, 0, 0, 0);
    }

    ILOG(" >>> done querying. status(0x%0X) ---{" ORCCVTASK_NAME "}---", status, 0, 0, 0, 0);

    return status;
}

inline errcode_enum_t flexidag_openseg_ag_set_init_context(
) {
    struct dag_ctxt *pSplitCtxt = NULL;

    // flexidag_openseg_ag_set_context() - logic depends on pgCVTaskContext to be set
    if (pgCVTaskContext == NULL) {
        CLOG(" >>> ** FATAL! Global Context Pointer is NULL", 0, 0, 0, 0, 0);
        return ERRCODE_GENERIC;
    }

    // %jinja2% generated code with variable (dag_info) dictionary given by cvtask.py
    // populates context with image_size, vmem_base, vmem_dagbin_base, & dram scratchpad bases
    
    /* --->>> flexidag_openseg_prim_split_0 <<<--- ***/
    pSplitCtxt = &pgCVTaskContext->split_ctxt[0];
    pSplitCtxt->dag_img_size = 59896;
    pSplitCtxt->vmem_dag_base = 2017512;
    pSplitCtxt->vmem_dagbin_base = 2073224;

    /* populate input buffer artefacts */
    pSplitCtxt->dag_inps_count = 0;
    pSplitCtxt->inps_vmem_offs[0] = 2004;
    pSplitCtxt->inps_dpitch_offs[0] = 2008;
    pSplitCtxt->dag_inps_count += 1;
    pSplitCtxt->inps_vmem_offs[1] = 2044;
    pSplitCtxt->inps_dpitch_offs[1] = 2048;
    pSplitCtxt->dag_inps_count += 1;

    /* populate output buffer artefacts */
    pSplitCtxt->dag_outs_count = 0;
    pSplitCtxt->outs_vmem_offs[0] = 2404;
    pSplitCtxt->outs_dpitch_offs[0] = 2408;
    pSplitCtxt->dag_outs_count += 1;
    pSplitCtxt->outs_vmem_offs[1] = 3972;
    pSplitCtxt->outs_dpitch_offs[1] = 3976;
    pSplitCtxt->dag_outs_count += 1;
    pSplitCtxt->outs_vmem_offs[2] = 4024;
    pSplitCtxt->outs_dpitch_offs[2] = 4028;
    pSplitCtxt->dag_outs_count += 1;
    pSplitCtxt->outs_vmem_offs[3] = 4124;
    pSplitCtxt->outs_dpitch_offs[3] = 4128;
    pSplitCtxt->dag_outs_count += 1;
    
    /* --->>> flexidag_openseg_prim_split_1 <<<--- ***/
    pSplitCtxt = &pgCVTaskContext->split_ctxt[1];
    pSplitCtxt->dag_img_size = 146512;
    pSplitCtxt->vmem_dag_base = 1930624;
    pSplitCtxt->vmem_dagbin_base = 2072776;

    /* populate input buffer artefacts */
    pSplitCtxt->dag_inps_count = 0;
    pSplitCtxt->inps_vmem_offs[0] = 28;
    pSplitCtxt->inps_dpitch_offs[0] = 32;
    pSplitCtxt->dag_inps_count += 1;
    pSplitCtxt->inps_vmem_offs[1] = 68;
    pSplitCtxt->inps_dpitch_offs[1] = 72;
    pSplitCtxt->dag_inps_count += 1;
    pSplitCtxt->inps_vmem_offs[2] = 104;
    pSplitCtxt->inps_dpitch_offs[2] = 108;
    pSplitCtxt->dag_inps_count += 1;

    /* populate output buffer artefacts */
    pSplitCtxt->dag_outs_count = 0;
    pSplitCtxt->outs_vmem_offs[0] = 3596;
    pSplitCtxt->outs_dpitch_offs[0] = 3600;
    pSplitCtxt->dag_outs_count += 1;
    pSplitCtxt->outs_vmem_offs[1] = 4300;
    pSplitCtxt->outs_dpitch_offs[1] = 4304;
    pSplitCtxt->dag_outs_count += 1;
    
    /* --->>> flexidag_openseg_prim_split_2 <<<--- ***/
    pSplitCtxt = &pgCVTaskContext->split_ctxt[2];
    pSplitCtxt->dag_img_size = 157804;
    pSplitCtxt->vmem_dag_base = 1920128;
    pSplitCtxt->vmem_dagbin_base = 2075512;

    /* populate input buffer artefacts */
    pSplitCtxt->dag_inps_count = 0;
    pSplitCtxt->inps_vmem_offs[0] = 28;
    pSplitCtxt->inps_dpitch_offs[0] = 32;
    pSplitCtxt->dag_inps_count += 1;

    /* populate output buffer artefacts */
    pSplitCtxt->dag_outs_count = 0;
    pSplitCtxt->outs_vmem_offs[0] = 2164;
    pSplitCtxt->outs_dpitch_offs[0] = 2168;
    pSplitCtxt->dag_outs_count += 1;
    pSplitCtxt->outs_vmem_offs[1] = 2260;
    pSplitCtxt->outs_dpitch_offs[1] = 2264;
    pSplitCtxt->dag_outs_count += 1;
    pSplitCtxt->outs_vmem_offs[2] = 2360;
    pSplitCtxt->outs_dpitch_offs[2] = 2364;
    pSplitCtxt->dag_outs_count += 1;
    
    /* --->>> flexidag_openseg_prim_split_3 <<<--- ***/
    pSplitCtxt = &pgCVTaskContext->split_ctxt[3];
    pSplitCtxt->dag_img_size = 150380;
    pSplitCtxt->vmem_dag_base = 1925376;
    pSplitCtxt->vmem_dagbin_base = 2073520;

    /* populate input buffer artefacts */
    pSplitCtxt->dag_inps_count = 0;
    pSplitCtxt->inps_vmem_offs[0] = 28;
    pSplitCtxt->inps_dpitch_offs[0] = 32;
    pSplitCtxt->dag_inps_count += 1;
    pSplitCtxt->inps_vmem_offs[1] = 68;
    pSplitCtxt->inps_dpitch_offs[1] = 72;
    pSplitCtxt->dag_inps_count += 1;
    pSplitCtxt->inps_vmem_offs[2] = 104;
    pSplitCtxt->inps_dpitch_offs[2] = 108;
    pSplitCtxt->dag_inps_count += 1;

    /* populate output buffer artefacts */
    pSplitCtxt->dag_outs_count = 0;
    pSplitCtxt->outs_vmem_offs[0] = 2024;
    pSplitCtxt->outs_dpitch_offs[0] = 2028;
    pSplitCtxt->dag_outs_count += 1;
    pSplitCtxt->outs_vmem_offs[1] = 2076;
    pSplitCtxt->outs_dpitch_offs[1] = 2080;
    pSplitCtxt->dag_outs_count += 1;
    pSplitCtxt->outs_vmem_offs[2] = 2176;
    pSplitCtxt->outs_dpitch_offs[2] = 2180;
    pSplitCtxt->dag_outs_count += 1;
    
    /* --->>> flexidag_openseg_prim_split_4 <<<--- ***/
    pSplitCtxt = &pgCVTaskContext->split_ctxt[4];
    pSplitCtxt->dag_img_size = 339020;
    pSplitCtxt->vmem_dag_base = 1739276;
    pSplitCtxt->vmem_dagbin_base = 2075508;

    /* populate input buffer artefacts */
    pSplitCtxt->dag_inps_count = 0;
    pSplitCtxt->inps_vmem_offs[0] = 28;
    pSplitCtxt->inps_dpitch_offs[0] = 32;
    pSplitCtxt->dag_inps_count += 1;
    pSplitCtxt->inps_vmem_offs[1] = 68;
    pSplitCtxt->inps_dpitch_offs[1] = 72;
    pSplitCtxt->dag_inps_count += 1;
    pSplitCtxt->inps_vmem_offs[2] = 104;
    pSplitCtxt->inps_dpitch_offs[2] = 108;
    pSplitCtxt->dag_inps_count += 1;

    /* populate output buffer artefacts */
    pSplitCtxt->dag_outs_count = 0;
    pSplitCtxt->outs_vmem_offs[0] = 2728;
    pSplitCtxt->outs_dpitch_offs[0] = 2732;
    pSplitCtxt->dag_outs_count += 1;
    
    /* --->>> flexidag_openseg_prim_split_5 <<<--- ***/
    pSplitCtxt = &pgCVTaskContext->split_ctxt[5];
    pSplitCtxt->dag_img_size = 341112;
    pSplitCtxt->vmem_dag_base = 1729804;
    pSplitCtxt->vmem_dagbin_base = 2069304;

    /* populate input buffer artefacts */
    pSplitCtxt->dag_inps_count = 0;
    pSplitCtxt->inps_vmem_offs[0] = 28;
    pSplitCtxt->inps_dpitch_offs[0] = 32;
    pSplitCtxt->dag_inps_count += 1;
    pSplitCtxt->inps_vmem_offs[1] = 68;
    pSplitCtxt->inps_dpitch_offs[1] = 72;
    pSplitCtxt->dag_inps_count += 1;
    pSplitCtxt->inps_vmem_offs[2] = 108;
    pSplitCtxt->inps_dpitch_offs[2] = 112;
    pSplitCtxt->dag_inps_count += 1;

    /* populate output buffer artefacts */
    pSplitCtxt->dag_outs_count = 0;
    pSplitCtxt->outs_vmem_offs[0] = 1552;
    pSplitCtxt->outs_dpitch_offs[0] = 1556;
    pSplitCtxt->dag_outs_count += 1;
    
    /* --->>> flexidag_openseg_prim_split_6 <<<--- ***/
    pSplitCtxt = &pgCVTaskContext->split_ctxt[6];
    pSplitCtxt->dag_img_size = 31008;
    pSplitCtxt->vmem_dag_base = 1691344;
    pSplitCtxt->vmem_dagbin_base = 1722028;

    /* populate input buffer artefacts */
    pSplitCtxt->dag_inps_count = 0;
    pSplitCtxt->inps_vmem_offs[0] = 28;
    pSplitCtxt->inps_dpitch_offs[0] = 32;
    pSplitCtxt->dag_inps_count += 1;

    /* populate output buffer artefacts */
    pSplitCtxt->dag_outs_count = 0;
    pSplitCtxt->outs_vmem_offs[0] = 208;
    pSplitCtxt->outs_dpitch_offs[0] = 212;
    pSplitCtxt->dag_outs_count += 1;
    pSplitCtxt->outs_vmem_offs[1] = 264;
    pSplitCtxt->outs_dpitch_offs[1] = 268;
    pSplitCtxt->dag_outs_count += 1;
    
    /* --->>> flexidag_openseg_prim_split_7 <<<--- ***/
    pSplitCtxt = &pgCVTaskContext->split_ctxt[7];
    pSplitCtxt->dag_img_size = 32700;
    pSplitCtxt->vmem_dag_base = 1502304;
    pSplitCtxt->vmem_dagbin_base = 1533272;

    /* populate input buffer artefacts */
    pSplitCtxt->dag_inps_count = 0;
    pSplitCtxt->inps_vmem_offs[0] = 28;
    pSplitCtxt->inps_dpitch_offs[0] = 32;
    pSplitCtxt->dag_inps_count += 1;
    pSplitCtxt->inps_vmem_offs[1] = 64;
    pSplitCtxt->inps_dpitch_offs[1] = 68;
    pSplitCtxt->dag_inps_count += 1;

    /* populate output buffer artefacts */
    pSplitCtxt->dag_outs_count = 0;
    pSplitCtxt->outs_vmem_offs[0] = 1672;
    pSplitCtxt->outs_dpitch_offs[0] = 1676;
    pSplitCtxt->dag_outs_count += 1;
    

    return ERRCODE_NONE;
}

inline errcode_enum_t flexidag_openseg_ag_set_run_context(
    const cvtask_parameter_interface_t *pCVTaskEnv
) {
    struct dag_ctxt *pSplitCtxt = NULL;
    memio_source_recv_raw_t *recv_raw = NULL;

    // flexidag_openseg_ag_set_context() - logic depends on pgCVTaskContext to be set
    if (pgCVTaskContext == NULL) {
        CLOG(" >>> ** FATAL! Global Context Pointer is NULL", 0, 0, 0, 0, 0);
        return ERRCODE_GENERIC;
    }

    // %jinja2% generated code with variable (dag_info) dictionary given by cvtask.py
    // populates context with image_size, vmem_base, vmem_dagbin_base, & dram scratchpad bases
    
        
    /* --->>> flexidag_openseg_prim_split_0 <<<--- ***/
    pSplitCtxt = &pgCVTaskContext->split_ctxt[0];

    /* populate input buffer artefacts */
    // ----> [Placeholder] %input% ===> from "simple input buffer"
	recv_raw = (memio_source_recv_raw_t *) pCVTaskEnv->vpInputBuffer[0];
    pSplitCtxt->inps_dram_base[0] = 0;
    pSplitCtxt->inps_dram_base[0] += (uint32_t)recv_raw->addr;
    // Only poke the input pitch of the 1st dag
    pSplitCtxt->inps_dram_pitch[0] = (uint32_t)recv_raw->pitch;
    pSplitCtxt->inps_dpitch_bsize[0] = (uint32_t)16;
    // ----> [Placeholder_uv] %input% ===> from "simple input buffer"
	recv_raw = (memio_source_recv_raw_t *) pCVTaskEnv->vpInputBuffer[1];
    pSplitCtxt->inps_dram_base[1] = 0;
    pSplitCtxt->inps_dram_base[1] += (uint32_t)recv_raw->addr;
    // Only poke the input pitch of the 1st dag
    pSplitCtxt->inps_dram_pitch[1] = (uint32_t)recv_raw->pitch;
    pSplitCtxt->inps_dpitch_bsize[1] = (uint32_t)16;

    /* populate output buffer artefacts */
    // <---- [Conv__Conv2D___BiasAdd___BatchNorm__FusedBatchNorm___muli___7___BatchNorm__FusedBatchNorm___PeleeNet__stem_block__stem_block_conv0__Conv2D___BatchNorm__FusedBatchNorm ] %output% <=== to "instance private uncached storage"
    pSplitCtxt->outs_dram_base[0] = 0;
    pSplitCtxt->outs_dram_base[0] += (uint32_t)pCVTaskEnv->vpInstance_private_uncached_storage;
    pSplitCtxt->outs_dram_pitch[0] = (uint32_t)32;
    pSplitCtxt->outs_dpitch_bsize[0] = (uint32_t)16;
    // <---- [__vcn_1219_ ] %output% <=== to "instance private uncached storage"
    pSplitCtxt->outs_dram_base[1] = 15728640;
    pSplitCtxt->outs_dram_base[1] += (uint32_t)pCVTaskEnv->vpInstance_private_uncached_storage;
    pSplitCtxt->outs_dram_pitch[1] = (uint32_t)32;
    pSplitCtxt->outs_dpitch_bsize[1] = (uint32_t)16;
    // <---- [PeleeNet__stage_2_dense_block_0__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___BatchNorm__FusedBatchNorm ] %output% <=== to "instance private uncached storage"
    pSplitCtxt->outs_dram_base[2] = 17039360;
    pSplitCtxt->outs_dram_base[2] += (uint32_t)pCVTaskEnv->vpInstance_private_uncached_storage;
    pSplitCtxt->outs_dram_pitch[2] = (uint32_t)32;
    pSplitCtxt->outs_dpitch_bsize[2] = (uint32_t)16;
    // <---- [PeleeNet__stage_2_dense_block_0__conv_left_1__Conv2D___BatchNorm__FusedBatchNorm___Relu ] %output% <=== to "instance private uncached storage"
    pSplitCtxt->outs_dram_base[3] = 17694720;
    pSplitCtxt->outs_dram_base[3] += (uint32_t)pCVTaskEnv->vpInstance_private_uncached_storage;
    pSplitCtxt->outs_dram_pitch[3] = (uint32_t)32;
    pSplitCtxt->outs_dpitch_bsize[3] = (uint32_t)16;
    
        
    /* --->>> flexidag_openseg_prim_split_1 <<<--- ***/
    pSplitCtxt = &pgCVTaskContext->split_ctxt[1];

    /* populate input buffer artefacts */
    // ----> [__vcn_1219_] %input% ===> from "instance private uncached storage"
    pSplitCtxt->inps_dram_base[0] = 15728640;
    pSplitCtxt->inps_dram_base[0] += (uint32_t)pCVTaskEnv->vpInstance_private_uncached_storage;
    pSplitCtxt->inps_dram_pitch[0] = (uint32_t)32;
    pSplitCtxt->inps_dpitch_bsize[0] = (uint32_t)16;
    // ----> [PeleeNet__stage_2_dense_block_0__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___BatchNorm__FusedBatchNorm] %input% ===> from "instance private uncached storage"
    pSplitCtxt->inps_dram_base[1] = 17039360;
    pSplitCtxt->inps_dram_base[1] += (uint32_t)pCVTaskEnv->vpInstance_private_uncached_storage;
    pSplitCtxt->inps_dram_pitch[1] = (uint32_t)32;
    pSplitCtxt->inps_dpitch_bsize[1] = (uint32_t)16;
    // ----> [PeleeNet__stage_2_dense_block_0__conv_left_1__Conv2D___BatchNorm__FusedBatchNorm___Relu] %input% ===> from "instance private uncached storage"
    pSplitCtxt->inps_dram_base[2] = 17694720;
    pSplitCtxt->inps_dram_base[2] += (uint32_t)pCVTaskEnv->vpInstance_private_uncached_storage;
    pSplitCtxt->inps_dram_pitch[2] = (uint32_t)32;
    pSplitCtxt->inps_dpitch_bsize[2] = (uint32_t)16;

    /* populate output buffer artefacts */
    // <---- [PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool ] %output% <=== to "instance private uncached storage"
    pSplitCtxt->outs_dram_base[0] = 17858560;
    pSplitCtxt->outs_dram_base[0] += (uint32_t)pCVTaskEnv->vpInstance_private_uncached_storage;
    pSplitCtxt->outs_dram_pitch[0] = (uint32_t)32;
    pSplitCtxt->outs_dpitch_bsize[0] = (uint32_t)16;
    // <---- [PeleeNet__stage_3_dense_block_0__concat ] %output% <=== to "instance private uncached storage"
    pSplitCtxt->outs_dram_base[1] = 18513920;
    pSplitCtxt->outs_dram_base[1] += (uint32_t)pCVTaskEnv->vpInstance_private_uncached_storage;
    pSplitCtxt->outs_dram_pitch[1] = (uint32_t)32;
    pSplitCtxt->outs_dpitch_bsize[1] = (uint32_t)16;
    
        
    /* --->>> flexidag_openseg_prim_split_2 <<<--- ***/
    pSplitCtxt = &pgCVTaskContext->split_ctxt[2];

    /* populate input buffer artefacts */
    // ----> [PeleeNet__stage_3_dense_block_0__concat] %input% ===> from "instance private uncached storage"
    pSplitCtxt->inps_dram_base[0] = 18513920;
    pSplitCtxt->inps_dram_base[0] += (uint32_t)pCVTaskEnv->vpInstance_private_uncached_storage;
    pSplitCtxt->inps_dram_pitch[0] = (uint32_t)32;
    pSplitCtxt->inps_dpitch_bsize[0] = (uint32_t)16;

    /* populate output buffer artefacts */
    // <---- [__vcn_1207_ ] %output% <=== to "instance private uncached storage"
    pSplitCtxt->outs_dram_base[0] = 19251200;
    pSplitCtxt->outs_dram_base[0] += (uint32_t)pCVTaskEnv->vpInstance_private_uncached_storage;
    pSplitCtxt->outs_dram_pitch[0] = (uint32_t)32;
    pSplitCtxt->outs_dpitch_bsize[0] = (uint32_t)16;
    // <---- [PeleeNet__stage_3_dense_block_4__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___muli___494___5 ] %output% <=== to "instance private uncached storage"
    pSplitCtxt->outs_dram_base[1] = 20234240;
    pSplitCtxt->outs_dram_base[1] += (uint32_t)pCVTaskEnv->vpInstance_private_uncached_storage;
    pSplitCtxt->outs_dram_pitch[1] = (uint32_t)32;
    pSplitCtxt->outs_dpitch_bsize[1] = (uint32_t)16;
    // <---- [PeleeNet__stage_3_dense_block_4__conv_left_1__Conv2D___BatchNorm__FusedBatchNorm___Relu ] %output% <=== to "instance private uncached storage"
    pSplitCtxt->outs_dram_base[2] = 20561920;
    pSplitCtxt->outs_dram_base[2] += (uint32_t)pCVTaskEnv->vpInstance_private_uncached_storage;
    pSplitCtxt->outs_dram_pitch[2] = (uint32_t)32;
    pSplitCtxt->outs_dpitch_bsize[2] = (uint32_t)16;
    
        
    /* --->>> flexidag_openseg_prim_split_3 <<<--- ***/
    pSplitCtxt = &pgCVTaskContext->split_ctxt[3];

    /* populate input buffer artefacts */
    // ----> [__vcn_1207_] %input% ===> from "instance private uncached storage"
    pSplitCtxt->inps_dram_base[0] = 19251200;
    pSplitCtxt->inps_dram_base[0] += (uint32_t)pCVTaskEnv->vpInstance_private_uncached_storage;
    pSplitCtxt->inps_dram_pitch[0] = (uint32_t)32;
    pSplitCtxt->inps_dpitch_bsize[0] = (uint32_t)16;
    // ----> [PeleeNet__stage_3_dense_block_4__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___muli___494___5] %input% ===> from "instance private uncached storage"
    pSplitCtxt->inps_dram_base[1] = 20234240;
    pSplitCtxt->inps_dram_base[1] += (uint32_t)pCVTaskEnv->vpInstance_private_uncached_storage;
    pSplitCtxt->inps_dram_pitch[1] = (uint32_t)32;
    pSplitCtxt->inps_dpitch_bsize[1] = (uint32_t)16;
    // ----> [PeleeNet__stage_3_dense_block_4__conv_left_1__Conv2D___BatchNorm__FusedBatchNorm___Relu] %input% ===> from "instance private uncached storage"
    pSplitCtxt->inps_dram_base[2] = 20561920;
    pSplitCtxt->inps_dram_base[2] += (uint32_t)pCVTaskEnv->vpInstance_private_uncached_storage;
    pSplitCtxt->inps_dram_pitch[2] = (uint32_t)32;
    pSplitCtxt->inps_dpitch_bsize[2] = (uint32_t)16;

    /* populate output buffer artefacts */
    // <---- [__vcn_1201_ ] %output% <=== to "instance private uncached storage"
    pSplitCtxt->outs_dram_base[0] = 20602880;
    pSplitCtxt->outs_dram_base[0] += (uint32_t)pCVTaskEnv->vpInstance_private_uncached_storage;
    pSplitCtxt->outs_dram_pitch[0] = (uint32_t)32;
    pSplitCtxt->outs_dpitch_bsize[0] = (uint32_t)16;
    // <---- [PeleeNet__stage_3_dense_block_7__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___BatchNorm__FusedBatchNorm ] %output% <=== to "instance private uncached storage"
    pSplitCtxt->outs_dram_base[1] = 21831680;
    pSplitCtxt->outs_dram_base[1] += (uint32_t)pCVTaskEnv->vpInstance_private_uncached_storage;
    pSplitCtxt->outs_dram_pitch[1] = (uint32_t)32;
    pSplitCtxt->outs_dpitch_bsize[1] = (uint32_t)16;
    // <---- [PeleeNet__stage_3_dense_block_7__conv_left_1__Conv2D___BatchNorm__FusedBatchNorm___Relu ] %output% <=== to "instance private uncached storage"
    pSplitCtxt->outs_dram_base[2] = 22159360;
    pSplitCtxt->outs_dram_base[2] += (uint32_t)pCVTaskEnv->vpInstance_private_uncached_storage;
    pSplitCtxt->outs_dram_pitch[2] = (uint32_t)32;
    pSplitCtxt->outs_dpitch_bsize[2] = (uint32_t)16;
    
        
    /* --->>> flexidag_openseg_prim_split_4 <<<--- ***/
    pSplitCtxt = &pgCVTaskContext->split_ctxt[4];

    /* populate input buffer artefacts */
    // ----> [__vcn_1201_] %input% ===> from "instance private uncached storage"
    pSplitCtxt->inps_dram_base[0] = 20602880;
    pSplitCtxt->inps_dram_base[0] += (uint32_t)pCVTaskEnv->vpInstance_private_uncached_storage;
    pSplitCtxt->inps_dram_pitch[0] = (uint32_t)32;
    pSplitCtxt->inps_dpitch_bsize[0] = (uint32_t)16;
    // ----> [PeleeNet__stage_3_dense_block_7__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___BatchNorm__FusedBatchNorm] %input% ===> from "instance private uncached storage"
    pSplitCtxt->inps_dram_base[1] = 21831680;
    pSplitCtxt->inps_dram_base[1] += (uint32_t)pCVTaskEnv->vpInstance_private_uncached_storage;
    pSplitCtxt->inps_dram_pitch[1] = (uint32_t)32;
    pSplitCtxt->inps_dpitch_bsize[1] = (uint32_t)16;
    // ----> [PeleeNet__stage_3_dense_block_7__conv_left_1__Conv2D___BatchNorm__FusedBatchNorm___Relu] %input% ===> from "instance private uncached storage"
    pSplitCtxt->inps_dram_base[2] = 22159360;
    pSplitCtxt->inps_dram_base[2] += (uint32_t)pCVTaskEnv->vpInstance_private_uncached_storage;
    pSplitCtxt->inps_dram_pitch[2] = (uint32_t)32;
    pSplitCtxt->inps_dpitch_bsize[2] = (uint32_t)16;

    /* populate output buffer artefacts */
    // <---- [ResizeBilinear_1 ] %output% <=== to "instance private uncached storage"
    pSplitCtxt->outs_dram_base[0] = 22200320;
    pSplitCtxt->outs_dram_base[0] += (uint32_t)pCVTaskEnv->vpInstance_private_uncached_storage;
    pSplitCtxt->outs_dram_pitch[0] = (uint32_t)32;
    pSplitCtxt->outs_dpitch_bsize[0] = (uint32_t)16;
    
        
    /* --->>> flexidag_openseg_prim_split_5 <<<--- ***/
    pSplitCtxt = &pgCVTaskContext->split_ctxt[5];

    /* populate input buffer artefacts */
    // ----> [ResizeBilinear_1] %input% ===> from "instance private uncached storage"
    pSplitCtxt->inps_dram_base[0] = 22200320;
    pSplitCtxt->inps_dram_base[0] += (uint32_t)pCVTaskEnv->vpInstance_private_uncached_storage;
    pSplitCtxt->inps_dram_pitch[0] = (uint32_t)32;
    pSplitCtxt->inps_dpitch_bsize[0] = (uint32_t)16;
    // ----> [PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool] %input% ===> from "instance private uncached storage"
    pSplitCtxt->inps_dram_base[1] = 17858560;
    pSplitCtxt->inps_dram_base[1] += (uint32_t)pCVTaskEnv->vpInstance_private_uncached_storage;
    pSplitCtxt->inps_dram_pitch[1] = (uint32_t)32;
    pSplitCtxt->inps_dpitch_bsize[1] = (uint32_t)16;
    // ----> [Conv__Conv2D___BiasAdd___BatchNorm__FusedBatchNorm___muli___7___BatchNorm__FusedBatchNorm___PeleeNet__stem_block__stem_block_conv0__Conv2D___BatchNorm__FusedBatchNorm] %input% ===> from "instance private uncached storage"
    pSplitCtxt->inps_dram_base[2] = 0;
    pSplitCtxt->inps_dram_base[2] += (uint32_t)pCVTaskEnv->vpInstance_private_uncached_storage;
    pSplitCtxt->inps_dram_pitch[2] = (uint32_t)32;
    pSplitCtxt->inps_dpitch_bsize[2] = (uint32_t)16;

    /* populate output buffer artefacts */
    // <---- [concat_1 ] %output% <=== to "instance private uncached storage"
    pSplitCtxt->outs_dram_base[0] = 32686080;
    pSplitCtxt->outs_dram_base[0] += (uint32_t)pCVTaskEnv->vpInstance_private_uncached_storage;
    pSplitCtxt->outs_dram_pitch[0] = (uint32_t)32;
    pSplitCtxt->outs_dpitch_bsize[0] = (uint32_t)16;
    
        
    /* --->>> flexidag_openseg_prim_split_6 <<<--- ***/
    pSplitCtxt = &pgCVTaskContext->split_ctxt[6];

    /* populate input buffer artefacts */
    // ----> [concat_1] %input% ===> from "instance private uncached storage"
    pSplitCtxt->inps_dram_base[0] = 32686080;
    pSplitCtxt->inps_dram_base[0] += (uint32_t)pCVTaskEnv->vpInstance_private_uncached_storage;
    pSplitCtxt->inps_dram_pitch[0] = (uint32_t)32;
    pSplitCtxt->inps_dpitch_bsize[0] = (uint32_t)16;

    /* populate output buffer artefacts */
    // <---- [concat_1_1232_ ] %output% <=== to "instance private uncached storage"
    pSplitCtxt->outs_dram_base[0] = 53657600;
    pSplitCtxt->outs_dram_base[0] += (uint32_t)pCVTaskEnv->vpInstance_private_uncached_storage;
    pSplitCtxt->outs_dram_pitch[0] = (uint32_t)32;
    pSplitCtxt->outs_dpitch_bsize[0] = (uint32_t)16;
    // <---- [Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___156___BatchNorm_5__FusedBatchNorm___Relu_3_1231_ ] %output% <=== to "instance private uncached storage"
    pSplitCtxt->outs_dram_base[1] = 65191936;
    pSplitCtxt->outs_dram_base[1] += (uint32_t)pCVTaskEnv->vpInstance_private_uncached_storage;
    pSplitCtxt->outs_dram_pitch[1] = (uint32_t)32;
    pSplitCtxt->outs_dpitch_bsize[1] = (uint32_t)16;
    
        
    /* --->>> flexidag_openseg_prim_split_7 <<<--- ***/
    pSplitCtxt = &pgCVTaskContext->split_ctxt[7];

    /* populate input buffer artefacts */
    // ----> [concat_1_1232_] %input% ===> from "instance private uncached storage"
    pSplitCtxt->inps_dram_base[0] = 53657600;
    pSplitCtxt->inps_dram_base[0] += (uint32_t)pCVTaskEnv->vpInstance_private_uncached_storage;
    pSplitCtxt->inps_dram_pitch[0] = (uint32_t)32;
    pSplitCtxt->inps_dpitch_bsize[0] = (uint32_t)16;
    // ----> [Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___156___BatchNorm_5__FusedBatchNorm___Relu_3_1231_] %input% ===> from "instance private uncached storage"
    pSplitCtxt->inps_dram_base[1] = 65191936;
    pSplitCtxt->inps_dram_base[1] += (uint32_t)pCVTaskEnv->vpInstance_private_uncached_storage;
    pSplitCtxt->inps_dram_pitch[1] = (uint32_t)32;
    pSplitCtxt->inps_dpitch_bsize[1] = (uint32_t)16;

    /* populate output buffer artefacts */
    // <---- [out_class ] %output% <=== to "simple output buffer"
    pSplitCtxt->outs_dram_base[0] = 0;
    pSplitCtxt->outs_dram_base[0] += (uint32_t)pCVTaskEnv->vpOutputBuffer[0];
    pSplitCtxt->outs_dram_pitch[0] = (uint32_t)320;
    pSplitCtxt->outs_dpitch_bsize[0] = (uint32_t)16;
    

    return ERRCODE_NONE;
}

inline errcode_enum_t flexidag_openseg_ag_read_cvtable(
    uint32_t split_id,
    uint32_t img_size
) {
    struct dag_ctxt *pSplitCtxt = &pgCVTaskContext->split_ctxt[split_id];
    errcode_enum_t status = ERRCODE_NONE;
    int32_t tbar_sz = 0;

    // %jinja2% generated code with variable - (cvtable_prefix = FLEXIDAG_OPENSEG_AG_)
    char cvtable_dag_key[64] = "FLEXIDAG_OPENSEG_AG_---_DAG";

    cvtable_dag_key[20] = '0' + ((split_id / 100) % 10);
    cvtable_dag_key[21] = '0' + ((split_id / 10 ) % 10);
    cvtable_dag_key[22] = '0' + ((split_id / 1  ) % 10);

    ILOG("  >> #Split-%u# Extracting DAG base address from CV Table Archive", split_id, 0, 0, 0, 0);

    // extracting DAG base from CV Table Archive
    status = cvtable_find(cvtable_dag_key, (const void**)&pSplitCtxt->dram_dag_base, &tbar_sz);
    if (is_err(status))
    {
        CLOG("  >> cvtable_find() key FLEXIDAG_OPENSEG_AG_%u_DAG look up failed for split(%u)", split_id, split_id, 0, 0, 0);
        goto readcv_exit;
    }
    else if(tbar_sz != img_size)
    {
        CLOG("  >> size of dag from table archive(%u) != size (%u) in header file for split(%u)", tbar_sz, img_size, split_id, 0, 0);
        status = ERRCODE_GENERIC;
        goto readcv_exit;
    }
    else
    {
        DLOG("  >> * extracted DAG binary @address(0x%08X)", pSplitCtxt->dram_dag_base, 0, 0, 0, 0);
    }

readcv_exit:
    return status;
}

/* cvtask_init() callback implementation */
errcode_enum_t flexidag_openseg_ag_cvtask_init (
    const cvtask_parameter_interface_t *pCVTaskEnv,
    const uint32_t *pSysConf
) {
    int split_id;
    errcode_enum_t status = ERRCODE_NONE;

    ILOG(" >>> start initializing. ---{" ORCCVTASK_NAME "}---", 0, 0, 0, 0, 0);

    if (pCVTaskEnv == NULL || pSysConf == NULL)
        status = ERRCODE_BAD_PARAMETER;

    /* At this point scheduler allotted memory for this ORC CVTask context. extract
     * pointer to cvtask context from private storage and cache in pgCVTaskContext */
    pgCVTaskContext = (struct flexidag_openseg_ag_cvtask_ctxt *)pCVTaskEnv->vpInstance_private_storage;
    pgCVTaskContext->split_id = -1;

    /* populate CVTask */
    if (is_not_err(status)) {
        status = flexidag_openseg_ag_set_init_context();
        if (is_err(status)) {
            goto cvtask_init_exit;
        }

        /* extract DRAM base addresses of loaded individual DAG & DDI binaries */
        for (split_id = 0; split_id < SPLITS_COUNT; split_id++) {
            status = flexidag_openseg_ag_read_cvtable(split_id, pgCVTaskContext->split_ctxt[split_id].dag_img_size);
            if (is_err(status)) {
                goto cvtask_init_exit;
            }
        }
    }

cvtask_init_exit:
    ILOG(" >>> done initializing. ---{" ORCCVTASK_NAME "}--- status(0x%08X)", status, 0, 0, 0, 0);

    return status;
}

/* cvtask_get_info() callback implementation */
errcode_enum_t flexidag_openseg_ag_cvtask_get_info (
    const cvtask_parameter_interface_t *pCVTaskEnv,
    uint32_t infoIndex,
    void *pInfoBuf
) {
    errcode_enum_t status = ERRCODE_NONE;

    char *tool_ver = TOOL_VERSION;
    char *gentask_ver = "CVTask Autogen v2";

    if (pInfoBuf == NULL) {
        status = ERRCODE_CVTASK_BASE;
        CLOG(" >>> get_info : invalid pInfoBuf(0x%08x)\n", (uint32_t)pInfoBuf, 0, 0, 0, 0);
    }
    else {
        status = ERRCODE_NONE;
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

/* cvtask_process_messages() callback implementation */
errcode_enum_t flexidag_openseg_ag_cvtask_itc (
    const cvtask_parameter_interface_t *pCVTaskEnv
) {
	errcode_enum_t retcode = ERRCODE_NONE;
	uint32_t msg_count = pCVTaskEnv->cvtask_num_messages;
	uint32_t value;
	static uint32_t count = 0xFFFFFFFFU;

	if (msg_count != 0U) {
		memcpy(&value, pCVTaskEnv->vpMessagePayload[0], sizeof(uint32_t));
		if( (count != 0xFFFFFFFFU) && ((count + 1U) != value)) {
			retcode = CVTASK_ERR_GENERAL;
			CVTASK_PRINTF(LVL_DEBUG,">>>>> [ERROR] cvtask_process_messages() : messages %d != count+1 %d", value, count+1, 0U, 0U, 0U);
		} else {
			count = value;
			CVTASK_PRINTF(LVL_DEBUG,">>>>> cvtask_process_messages() : messages %d ", value, 0U, 0U, 0U, 0U);
		}
	} else {
		retcode = CVTASK_ERR_GENERAL;
	}

	return retcode;
}

static void flexidag_openseg_ag_print_debug_info(
    const cvtask_parameter_interface_t *pCVTaskEnv
) {
    int loop;

    DLOG(" >>> start dumping runtime context of ---{" ORCCVTASK_NAME "}---" " instance #[%02u]",
            pCVTaskEnv->cvtask_instance, 0, 0, 0, 0);

    /* dump input buffers info*/
    DLOG("  >> Input Buffers: Number=(#%u)\n", pCVTaskEnv->cvtask_num_inputs, 0, 0, 0, 0);
    for (loop = 0; loop < 2; loop++) {
        DLOG("   > Buffer Addresses : %08X, %08X, %08X, %08X",
            (uint32_t)pCVTaskEnv->vpInputBuffer[(loop * 4) + 0],
            (uint32_t)pCVTaskEnv->vpInputBuffer[(loop * 4) + 1],
            (uint32_t)pCVTaskEnv->vpInputBuffer[(loop * 4) + 2],
            (uint32_t)pCVTaskEnv->vpInputBuffer[(loop * 4) + 3],
            0
        );
    }

    /* dump output buffers info*/
    DLOG("  >> Output Buffers: Number=(#%u)\n", pCVTaskEnv->cvtask_num_outputs, 0, 0, 0, 0);
    for (loop = 0; loop < 2; loop++) {
        DLOG("   > Buffer Addresses : %08X, %08X, %08X, %08X",
            (uint32_t)pCVTaskEnv->vpOutputBuffer[(loop * 4) + 0],
            (uint32_t)pCVTaskEnv->vpOutputBuffer[(loop * 4) + 1],
            (uint32_t)pCVTaskEnv->vpOutputBuffer[(loop * 4) + 2],
            (uint32_t)pCVTaskEnv->vpOutputBuffer[(loop * 4) + 3],
            0
        );
    }

    /* dump feedback buffers info*/
    DLOG("  >> Feedback Buffers: Number=(#%u)\n", pCVTaskEnv->cvtask_num_feedback, 0, 0, 0, 0);
    for (loop = 0; loop < 1; loop++) {
        DLOG("   > Buffer Addresses : %08X, %08X, %08X, %08X",
            (uint32_t)pCVTaskEnv->vpFeedbackBuffer[(loop * 4) + 0],
            (uint32_t)pCVTaskEnv->vpFeedbackBuffer[(loop * 4) + 1],
            (uint32_t)pCVTaskEnv->vpFeedbackBuffer[(loop * 4) + 2],
            (uint32_t)pCVTaskEnv->vpFeedbackBuffer[(loop * 4) + 3],
            0
        );
    }

    /* dump message payload buffers info*/
    DLOG("  >> Message Payload Buffers: Number=(#%u)\n", pCVTaskEnv->cvtask_num_messages, 0, 0, 0, 0);
    for (loop = 0; loop < 1; loop++) {
        DLOG("   > Buffer Addresses : %08X, %08X, %08X, %08X",
            (uint32_t)pCVTaskEnv->vpMessagePayload[(loop * 4) + 0],
            (uint32_t)pCVTaskEnv->vpMessagePayload[(loop * 4) + 1],
            (uint32_t)pCVTaskEnv->vpMessagePayload[(loop * 4) + 2],
            (uint32_t)pCVTaskEnv->vpMessagePayload[(loop * 4) + 3],
            0
        );
    }

    /* dump CMEM scratch pad memory info */
    if (pCVTaskEnv->vpCMEM_temporary_scratchpad) {
        DLOG("  >> CMEM Scratchpad: Base-(%08X), Size-(%9u)\n",
            (uint32_t)pCVTaskEnv->vpCMEM_temporary_scratchpad,
            pCVTaskEnv->CMEM_temporary_scratchpad_size,
            0, 0, 0
        );
    }

    /* dump DRAM scratch pad memory info */
    if (pCVTaskEnv->vpDRAM_temporary_scratchpad) {
        DLOG("  >> DRAM Scratchpad: Base-(%08X), Size-(%9u)\n",
            (uint32_t)pCVTaskEnv->vpDRAM_temporary_scratchpad,
            pCVTaskEnv->DRAM_temporary_scratchpad_size,
            0, 0, 0
        );
    }

    /* dump Private Storage info */
    if (pCVTaskEnv->vpInstance_private_storage) {
        DLOG("  >> Private Storage: Base-(%08X), Size-(%9u)\n",
            (uint32_t)pCVTaskEnv->vpInstance_private_storage,
            pCVTaskEnv->Instance_private_storage_size,
            0, 0, 0
        );
    }

    /* dump Uncached-Private Storage info */
    if (pCVTaskEnv->vpInstance_private_uncached_storage) {
        DLOG("  >> Uncached Private Storage: Base-(%08X), Size-(%9u)\n",
            (uint32_t)pCVTaskEnv->vpInstance_private_uncached_storage,
            pCVTaskEnv->Instance_private_uncached_storage_size,
            0, 0, 0
        );
    }

    /* dump Shared Storage info */
    if (pCVTaskEnv->vpCVTask_shared_storage) {
        DLOG("  >> Shared Storage: Base-(%08X), Size-(%9u)\n",
            (uint32_t)pCVTaskEnv->vpCVTask_shared_storage,
            pCVTaskEnv->CVTask_shared_storage_size,
            0, 0, 0
        );
    }

    DLOG(" >>> done dumping runtime context of ---{" ORCCVTASK_NAME "}---", 0, 0, 0, 0, 0);
}

inline void flexidag_openseg_ag_load_split(
    uint32_t split_id,
    int wait
) {
    int ip, op;
    uint32_t load_start;
    struct dag_ctxt *pSplitCtxt = &pgCVTaskContext->split_ctxt[split_id];

    ILOG("  >> start loading split-#%03u.    @cycle=(%u)", split_id, (load_start = get_cur_time()), 0, 0, 0);

    // initialize vector processor core for every run
    vp_init();

    // load dag split in to vector memory from dram memory
    // *** BLOCKED call, since we need to load for pokes to succeed
    vp_load_dag(
        (uint32_t)pSplitCtxt->dram_dag_base,
        pSplitCtxt->vmem_dag_base,
        pSplitCtxt->dag_img_size,
        1
    );

    // setting up coprocessor register for vector processor health
    dma_cmem_2_coproc(0x100, CMEM_DAG_BASE, 1024);

    /* poke DRAM addresses of input and output buffers for Vector Processor DMA to load/store data */
    // poke constant hmb input addresses

    // poke input addresses
    for(ip = 0; ip < pSplitCtxt->dag_inps_count; ip++) {
        coproc_poke_word(
            (uint32_t)pSplitCtxt->inps_dram_base[ip],
            pSplitCtxt->vmem_dagbin_base + pSplitCtxt->inps_vmem_offs[ip]
        );

        // Only poke the input pitch of the 1st dag
        if ( (split_id == 0) && (pSplitCtxt->inps_dram_pitch[ip] != 0) ) {
            if (16 == pSplitCtxt->inps_dpitch_bsize[ip]) {
                coproc_poke_half(
                    (uint32_t)pSplitCtxt->inps_dram_pitch[ip]-1,
                    pSplitCtxt->vmem_dagbin_base + pSplitCtxt->inps_dpitch_offs[ip]
                );
            }
            else {
                coproc_poke_word(
                    (uint32_t)pSplitCtxt->inps_dram_pitch[ip]-1,
                    pSplitCtxt->vmem_dagbin_base + pSplitCtxt->inps_dpitch_offs[ip]
                );
            }
        }
    }

    // poke output addresses
    for(op = 0; op < pSplitCtxt->dag_outs_count; op++) {
        coproc_poke_word(
            (uint32_t)pSplitCtxt->outs_dram_base[op],
            pSplitCtxt->vmem_dagbin_base + pSplitCtxt->outs_vmem_offs[op]
        );
        /*
        if (16 == pSplitCtxt->outs_dpitch_bsize[op]) {
            coproc_poke_half(
                (uint32_t)pSplitCtxt->outs_dram_pitch[op],
                pSplitCtxt->vmem_dagbin_base + pSplitCtxt->outs_dpitch_offs[op]
            );
        }
        else {
            coproc_poke_word(
                (uint32_t)pSplitCtxt->outs_dram_pitch[op],
                pSplitCtxt->vmem_dagbin_base + pSplitCtxt->outs_dpitch_offs[op]
            );
        }
        */
    }

    // wait for Vision ORC to finish
    if (wait) {
        wait(VIS_W_COPROC);
        ILOG("  >>  done run-#%03u : load-#%03u.  cycles=(%u)", split_id-1, split_id, get_cur_time() - load_start, 0, 0);
    } else {
        ILOG("  >>  done loading split-#%03u.    cycles=(%u)", split_id, get_cur_time() - load_start, 0, 0, 0);
    }
}

inline void flexidag_openseg_ag_run_split(
    uint32_t split_id,
    int wait
) {
    uint32_t run_start = 0;

    if (wait) {
        ILOG("  >> start executing split-#%03u.  @cycle=(%u)", split_id, (run_start = get_cur_time()), 0, 0, 0);
    }

    // kick off co-processor to run vector processor
    coproc_run(pgCVTaskContext->split_ctxt[split_id].vmem_dagbin_base);

    // wait for Vision ORC to finish
    if (wait) {
        wait(VIS_W_COPROC);
        ILOG("  >>  done executing split-#%03u.  cycles=(%u)", split_id, get_cur_time() - run_start, 0, 0, 0);
    }
}

/* cvtask_process_run() callback implementation */
errcode_enum_t flexidag_openseg_ag_cvtask_run (
    const cvtask_parameter_interface_t *pCVTaskEnv
) {
    int split_id = 0;
    uint32_t cvt_run_start, cvt_run_finish;
    errcode_enum_t status = ERRCODE_NONE;

    

    if (pgCVTaskContext->split_id < 0) {
        /* populate split dag's context in CVTask context with VAS provided information */
        ILOG(" >>> start setting context ---{" ORCCVTASK_NAME "}--- @start-cycle=(%u)", (cvt_run_start = get_cur_time()), 0, 0, 0, 0);
        status = flexidag_openseg_ag_set_run_context(pCVTaskEnv);
        cvt_run_finish = get_cur_time();
        ILOG(" >>> done setting context  ---{" ORCCVTASK_NAME "}--- @end-cycle=(%u). total-time=[%u] status=(0x%08X)\n", \
                cvt_run_finish, (cvt_run_finish - cvt_run_start), status, 0, 0);
        if (is_err(status)) {
            return status;
        }

        /* dump runtime context of this ORC CVTask */
        flexidag_openseg_ag_print_debug_info(pCVTaskEnv);

        ILOG(" >>> start running ---{" ORCCVTASK_NAME "}---. @start-cycle=(%u)", (cvt_run_start = get_cur_time()), 0, 0, 0, 0);

        // start from very first split DAG
        split_id = 0;
    } else {
        // resume execution from the DAG following the last finished one
        split_id = pgCVTaskContext->split_id + 1;
        ILOG(" >>> resuming ---{" ORCCVTASK_NAME "}--- from DAG #%u. @start-cycle=(%u)", \
                pgCVTaskContext->split_id, (cvt_run_start = get_cur_time()), 0, 0, 0); 
    }

    //  %jinja2% generated with flag - %ping_pong_sched = False%
    /* execute every dag split sequentially */
    for (; split_id < SPLITS_COUNT; split_id++) {
        
        // load the dag - don't wait for load to finish
        flexidag_openseg_ag_load_split(split_id, 0);

        
        // run the loaded split - wait for run to finish
        flexidag_openseg_ag_run_split(split_id, 1);

        // yield control to scheduler whenever necessary
        if (cvtask_check_yield()) {
            // store context to uncached storage
            pgCVTaskContext->split_id = split_id;

            
            cvt_run_finish = get_cur_time();
            ILOG(" >>> pausing ---{" ORCCVTASK_NAME "}--- at DAG #%u @end-cycle=(%u). total-time=[%u] status=(0x%08X)\n", \
                    split_id, cvt_run_finish, (cvt_run_finish - cvt_run_start), status, 0);
            return RETCODE_CVTASK_YIELDED;
        }
    }

    // reset split_id
    pgCVTaskContext->split_id = -1;

    cvt_run_finish = get_cur_time();
    ILOG(" >>> done running ---{" ORCCVTASK_NAME "}--- @end-cycle=(%u). total-time=[%u] status=(0x%08X)\n", \
            cvt_run_finish, (cvt_run_finish - cvt_run_start), status, 0, 0);

    return status;
}

/* cvtask_finish() callback implementation */
errcode_enum_t flexidag_openseg_ag_cvtask_finish (
    const cvtask_parameter_interface_t *pCVTaskEnv
) {
    errcode_enum_t status = ERRCODE_NONE;

    /* TODO - needs implementation */
    return status;
}

/* cvtask_register() callback implementation */
errcode_enum_t flexidag_openseg_ag_cvtask_register (
    cvtask_entry_t *pCVTaskEntry
) {
    ILOG(">>> start registering # ---{" ORCCVTASK_NAME "}---", 0, 0, 0, 0, 0);

    visorc_strcpy(pCVTaskEntry->cvtask_name, ORCCVTASK_NAME, NAME_MAX_LENGTH);
    pCVTaskEntry->cvtask_type        = ORCCVTASK_TYPE;
    pCVTaskEntry->cvtask_api_version = CVTASK_API_VER;
    pCVTaskEntry->cvtask_query       = &flexidag_openseg_ag_cvtask_query;
    pCVTaskEntry->cvtask_init        = &flexidag_openseg_ag_cvtask_init;
    pCVTaskEntry->cvtask_get_info    = &flexidag_openseg_ag_cvtask_get_info;
    pCVTaskEntry->cvtask_run         = &flexidag_openseg_ag_cvtask_run;
    pCVTaskEntry->cvtask_finish      = &flexidag_openseg_ag_cvtask_finish;
    pCVTaskEntry->cvtask_process_messages = &flexidag_openseg_ag_cvtask_itc;

    ILOG(">>> done registering # ---{" ORCCVTASK_NAME "}---", 0, 0, 0, 0, 0);

    return ERRCODE_NONE;
}
