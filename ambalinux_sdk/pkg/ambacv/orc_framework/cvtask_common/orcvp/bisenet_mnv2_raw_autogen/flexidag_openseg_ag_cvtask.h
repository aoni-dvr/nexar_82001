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

#ifndef _FLEXIDAG_OPENSEG_AG_H_
#define _FLEXIDAG_OPENSEG_AG_H_

// for CVTask framework structures - cvtask_entry_t
// for errcode_enum_t, CVTASK_PRINTF()
#include <cvtask_interface.h>

// for vp_init(), vp_load_dag()
#include <vp_common.h>

// for coproc_run(), coproc_poke_*() & coproc_peek_*()
#include <vis_coproc.h>

// for dma_cmem_2_coproc()
#include <orc_memory.h>

// for memio interface
#include "cvapi_memio_interface.h"

// *** custom values set by jinja2 template engine
#define DNN_NAME_LOWER    "flexidag_openseg_ag"
#define DNN_NAME_GIVEN    "flexidag_openseg_ag"
#define SPLITS_COUNT      (8)
#define CMEM_DAG_BASE     (256)

// Macros derived from framework & user requests
#define ORCCVTASK_NAME    DNN_NAME_GIVEN
#define ORCCVTASK_TYPE    CVTASK_TYPE_VPANY
#define CVTASK_API_VER    CVTASK_API_VERSION

// Aliased LOG functions
#define CLOG(...)  CVTASK_PRINTF(LVL_CRITICAL, __VA_ARGS__)
#define ILOG(...)  CVTASK_PRINTF(LVL_NORMAL,   __VA_ARGS__)
#define DLOG(...)  CVTASK_PRINTF(LVL_DEBUG,    __VA_ARGS__)
#define VLOG(...)  CVTASK_PRINTF(LVL_VERBOSE,  __VA_ARGS__)



// %jinja2% generated - all the following array sizes are declared using
// smartly found variable (max_inter_hmb_outs = 4)
// Per DAG-split private info
struct dag_ctxt {

    uint32_t dag_img_size;
    uint32_t vmem_dag_base;
    uint32_t vmem_dagbin_base;

    uint8_t *dram_dag_base;
    uint32_t dag_inps_count;
    uint32_t inps_dram_base[4];
    uint32_t inps_dram_pitch[4];
    uint32_t inps_dpitch_bsize[4];
    uint32_t inps_vmem_offs[4];
    uint32_t inps_dpitch_offs[4];

    uint32_t dag_outs_count;
    uint32_t outs_dram_base[4];
    uint32_t outs_dram_pitch[4];
    uint32_t outs_dpitch_bsize[4];
    uint32_t outs_vmem_offs[4];
    uint32_t outs_dpitch_offs[4];
};

// Whole ORC CVTask context to store private info
struct flexidag_openseg_ag_cvtask_ctxt {
    // check sum the context
    uint32_t magic_number;

    // per-split context
    struct dag_ctxt split_ctxt[SPLITS_COUNT];

// context for yielding control to scheduler
    int32_t split_id;
};


/* Description:
 *  Callback to register ORC CVTask with Scheduler Framework
 *
 * Arguments:
 *  @pCVTaskEntry@ : Pointer to memory allotted by scheduler to be filled
 *                   by this callback function
 */
errcode_enum_t flexidag_openseg_ag_cvtask_register (
    cvtask_entry_t *pCVTaskEntry
);

/* Description:
 *  Callback that allows scheduler to Query ORC CVTask's memory requirements
 *
 * Arguments:
 *  @instanceId@ : Instance of CVTask being queried
 *  @pSysConf@   : Read-only pointer to memory loaded System Flow config
 *  @pMemDesc@   : Pointer to cvtask_memory_interface_t structure that
 *                 must be populated b this callback to ask memory needed
 */
errcode_enum_t flexidag_openseg_ag_cvtask_query (
    uint32_t instanceId,
    const uint32_t *pSysConf,
    cvtask_memory_interface_t *pMemDesc
);

/* Description:
 *  Callback that allows scheduler to Initialize ORC CVTask's internal data
 *
 * Arguments:
 *  @pCVTaskEnv@ : Read-only pointer that contains scheduler allocated memory
 *                 as per the requests made in the *_cvtask_query() callback
 *  @pSysConf@   : Read-only pointer to memory loaded System Flow config
 */
errcode_enum_t flexidag_openseg_ag_cvtask_init (
    const cvtask_parameter_interface_t *pCVTaskEnv,
    const uint32_t *pSysConf
);

/* Description:
 *  Callback that allows scheduler to Query versions of the CVTask lib
 *
 * Arguments:
 *  @pCVTaskENv@ : Read-only pointer that contains scheduler allocated memory
 *                 as per the requests made in the *_cvtask_query() callback
 *  @infoIndex@  : Type of info being requested
 *                     0 - Version of this CVTask Library
 *                     1 - Version of the Build tool
 *  @pInfoBuf@   : Buffer pointer to be filled with requested version info
 */
errcode_enum_t flexidag_openseg_ag_cvtask_get_info (
    const cvtask_parameter_interface_t *pCVTaskEnv,
    uint32_t infoIndex,
    void *pInfoBuf
);

/* Description:
 *  Callback that allows CVTask's in a SuperDAG runtime environment to
 *  communicate with each other by sending messages.
 *     ITC - Inter-Task Communication
 *
 * Arguments:
 *  @pCVTaskENv@ : Read-only pointer that contains scheduler allocated memory
 *                 as per the requests made in the *_cvtask_query() callback
 */
errcode_enum_t flexidag_openseg_ag_cvtask_itc (
    const cvtask_parameter_interface_t *pCVTaskEnv
);

/* Description:
 *  Callback that allows scheduler to run a DAG on Vector Processor
 *
 * Arguments:
 *  @pCVTaskENv@ : Read-only pointer that contains scheduler allocated memory
 *                 as per the requests made in the *_cvtask_query() callback
 */
errcode_enum_t flexidag_openseg_ag_cvtask_run (
    const cvtask_parameter_interface_t *pCVTaskEnv
);

/* Description:
 *  Callback that allows scheduler to clean up CVTask's resources before exiting
 *
 * Arguments:
 *  @pCVTaskENv@ : Read-only pointer that contains scheduler allocated memory
 *                 as per the requests made in the *_cvtask_query() callback
 */
errcode_enum_t flexidag_openseg_ag_cvtask_finish (
    const cvtask_parameter_interface_t *pCVTaskEnv
);

#endif
