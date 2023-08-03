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
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include "cvtask_api.h"
#include "cvtask_errno.h"
#include "cvtask_ossrv.h"
#include "build_version.h"
#define CVTASK_NAME "EXTERN_OPENOD_NMS"
#include "cvapi_extern_openod_nms.h"
#define CONCAT1(a, b) a ## b
#define CONCAT(a, b) CONCAT1(a, b)

#define IDENTIFIER extern_openod_nms
#define fname_init CONCAT(IDENTIFIER, _init)
#define fname_iter_init CONCAT(IDENTIFIER, _iter_init)
#define fname_embedded_forward CONCAT(IDENTIFIER, _forward)
#define fname_memory_desc CONCAT(IDENTIFIER, _memory_desc)

static const cvtask_entry_t entry;

#define NUM_IN (4U)
#define NUM_OUT (4U)
#define TOTAL_BATCH_SIZE (1U)

#define ATTR_NAME ""

static const amba_cnn_c_vcoord_t maximum_input_sizes[NUM_IN] = {{.h = 1U,.d = 1U,.p = 1U,.w = 81600U},{.h = 1U,.d = 1U,.p = 1U,.w = 163200U},{.h = 1U,.d = 1U,.p = 1U,.w = 163200U},{.h = 1U,.d = 1U,.p = 1U,.w = 163200U}};

static const amba_cnn_c_vcoord_t maximum_output_sizes[NUM_OUT] = {{.h = 1U,.d = 1U,.p = 1U,.w = 1U},{.h = 150U,.d = 1U,.p = 1U,.w = 4U},{.h = 1U,.d = 1U,.p = 1U,.w = 150U},{.h = 1U,.d = 1U,.p = 1U,.w = 150U}};

static const amba_cnn_c_data_format_t input_data_formats[NUM_IN]= {{.bitvector = 0U,.datasize = 2U,.expoff = 0U,.expbits = 7U,.sign = 1U,.undef = 0U},{.bitvector = 0U,.datasize = 2U,.expoff = 0U,.expbits = 7U,.sign = 1U,.undef = 0U},{.bitvector = 0U,.datasize = 2U,.expoff = 0U,.expbits = 7U,.sign = 1U,.undef = 0U},{.bitvector = 0U,.datasize = 2U,.expoff = 0U,.expbits = 7U,.sign = 1U,.undef = 0U}};

static const amba_cnn_c_data_format_t output_data_formats[NUM_OUT] ={{.bitvector = 0U,.datasize = 2U,.expoff = 0U,.expbits = 0U,.sign = 0U,.undef = 0U},{.bitvector = 0U,.datasize = 2U,.expoff = 0U,.expbits = 7U,.sign = 1U,.undef = 0U},{.bitvector = 0U,.datasize = 2U,.expoff = 0U,.expbits = 0U,.sign = 0U,.undef = 0U},{.bitvector = 0U,.datasize = 2U,.expoff = 0U,.expbits = 7U,.sign = 1U,.undef = 0U}};

typedef struct {
    int cid;
    const amba_cnn_c_vcoord_t *isz;
    amba_cnn_c_data_format_t idf[NUM_IN];
    amba_cnn_c_vcoord_t osz[NUM_OUT];
    amba_cnn_c_data_format_t odf[NUM_OUT];
} ctxt_t;

static cvtask_memory_interface_t fname_memory_desc = {
    .num_inputs = NUM_IN,
    .input[0].io_name = "arm_conf_flatten",
    .input[1].io_name = "arm_loc",
    .input[2].io_name = "odm_conf_flatten",
    .input[3].io_name = "odm_loc",
     
    .num_outputs = NUM_OUT,
    .output[0].io_name = "num_objects",
    .output[1].io_name = "decbox",
    .output[2].io_name = "clsId",
    .output[3].io_name = "score",
     
    .Instance_private_storage_needed = sizeof(ctxt_t),
    .num_custom_msgpool = 1,
    .custom_msgpool_message_num[0] = 2,
    .custom_msgpool_message_size[0] = 64,
};

static uint32_t query(
    uint32_t iid, const uint32_t *config,
        cvtask_memory_interface_t *pCVTaskMemory
) {

    unsigned int i;
 
    if (config == NULL || pCVTaskMemory == NULL)
        return ERRCODE_BAD_PARAMETER;
 
    *pCVTaskMemory = fname_memory_desc;
 
    for (i = 0; i < NUM_OUT; i++) {
        pCVTaskMemory->output[i].buffer_size = TOTAL_BATCH_SIZE;
        pCVTaskMemory->output[i].buffer_size *= maximum_output_sizes[i].d;
        pCVTaskMemory->output[i].buffer_size *= maximum_output_sizes[i].p;
        pCVTaskMemory->output[i].buffer_size *= maximum_output_sizes[i].h;
        pCVTaskMemory->output[i].buffer_size *= maximum_output_sizes[i].w;
        switch (output_data_formats[i].datasize) {
            case 1U:
                pCVTaskMemory->output[i].buffer_size *= 2;
                break;
            case 2U:
                pCVTaskMemory->output[i].buffer_size *= 4;
                break;
            default:
                pCVTaskMemory->output[i].buffer_size *= 1;
        }
    }
 
    return ERRCODE_NONE;
}

static uint32_t init(
    const cvtask_parameter_interface_t *pCVTaskEnv,
    const uint32_t *config
) {
    
    int i, ret;
    int data_mode = 0, num_src = NUM_IN;
    ctxt_t *ctxt;
 
    if (config == NULL || pCVTaskEnv == NULL)
        return ERRCODE_BAD_PARAMETER;
 
    ctxt = pCVTaskEnv->vpInstance_private_storage;
    if (ctxt == NULL)
        return ERRCODE_BAD_PARAMETER;

    ctxt->isz = maximum_input_sizes;

    ret = fname_init(
        &ctxt->cid, ctxt->osz,
        ctxt->idf,
        ctxt->odf, &data_mode,
        num_src, ctxt->isz,
        ATTR_NAME, NULL
    );
    if (ret != 0)
        return ERRCODE_GENERIC;
 
    for (i = 0; i < NUM_IN; i++) {
        assert(ctxt->idf[i].undef == input_data_formats[i].undef);
        assert(ctxt->idf[i].sign == input_data_formats[i].sign);
        assert(ctxt->idf[i].datasize == input_data_formats[i].datasize);
        assert(ctxt->idf[i].expoff == input_data_formats[i].expoff);
        assert(ctxt->idf[i].expbits == input_data_formats[i].expbits);
        assert(ctxt->idf[i].bitvector == input_data_formats[i].bitvector);
    }
    for (i = 0; i < NUM_OUT; i++) {
        assert(ctxt->osz[i].d == maximum_output_sizes[i].d);
        assert(ctxt->osz[i].p == maximum_output_sizes[i].p);
        assert(ctxt->osz[i].h == maximum_output_sizes[i].h);
        assert(ctxt->osz[i].w == maximum_output_sizes[i].w);
        assert(ctxt->odf[i].undef == output_data_formats[i].undef);
        assert(ctxt->odf[i].sign == output_data_formats[i].sign);
        assert(ctxt->odf[i].datasize == output_data_formats[i].datasize);
        assert(ctxt->odf[i].expoff == output_data_formats[i].expoff);
        assert(ctxt->odf[i].expbits == output_data_formats[i].expbits);
        assert(ctxt->odf[i].bitvector == output_data_formats[i].bitvector);
    }
 
    return ERRCODE_NONE;
}

static uint32_t run(const cvtask_parameter_interface_t *env) {

    int i, i_batch_size, o_batch_size;
    void *src[NUM_IN], *dst[NUM_OUT];
    ctxt_t *ctxt;
    int ret;

    #ifdef CVTASK_PRINT
        VERBOSE("run cvtask [ARMTASK]");
    #endif
 
    if (env == NULL)
        return ERRCODE_BAD_PARAMETER;
 
    ctxt = (ctxt_t*)env->vpInstance_private_storage;
    if (ctxt == NULL)
        return ERRCODE_BAD_PARAMETER;

    ctxt->isz = maximum_input_sizes;
 
    i_batch_size = o_batch_size = TOTAL_BATCH_SIZE;
       ret = fname_iter_init(
        ctxt->cid, ctxt->osz,
        ctxt->isz,
        o_batch_size,
        &i_batch_size
    );

    if (ret != 0)
        return ERRCODE_GENERIC;
 
    for (i = 0; i < NUM_IN; i++)
    {
        src[i] = (void *) env->vpInputBuffer[i];
    }

   for (i = 0; i < NUM_OUT; i++)
        dst[i] = (void *) env->vpOutputBuffer[i];
    ret = fname_embedded_forward(ctxt->cid, dst, src);

    if (ret != 0)
        return ERRCODE_GENERIC;
 
    return ERRCODE_NONE;

}

static uint32_t proc_msg(const cvtask_parameter_interface_t *env)
{
        return CVTASK_ERR_OK;
}


static uint32_t get_info(const cvtask_parameter_interface_t *env,
    uint32_t info_id, void *info_data)
{

    #ifdef CVTASK_PRINT
        VERBOSE("get_info cvtask [ARMTASK]");
    #endif

    uint32_t err;
    const char **str;

    switch (info_id) {
    case CVTASK_INFO_ID_GET_SELF_INFO:
        str = (const char **)info_data;
        *str = LIBRARY_VERSION;
        err = CVTASK_ERR_OK;
        break;
    default:
        err = CVTASK_ERR_UNIMPLEMENTED;
    }

    return err;
}

static const cvtask_entry_t entry = {
    .cvtask_name     = CVTASK_NAME,
    .cvtask_type     = CVTASK_TYPE_ARM,
    .cvtask_query    = query,
    .cvtask_init     = init,
    .cvtask_get_info = get_info,
    .cvtask_process_messages = proc_msg,
    .cvtask_run      = run,
};
uint32_t extern_openod_nms_create(void)
{

    cvtask_register(&entry, CVTASK_API_VERSION);
    return CVTASK_ERR_OK;
}

cvtask_declare_create(extern_openod_nms_create);

