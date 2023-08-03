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

#ifndef _FLEXIDAG_OPENSEG_AG_CVTASK_H_
#define _FLEXIDAG_OPENSEG_AG_CVTASK_H_

// for CVTask framework structures - cvtask_entry_t
// for errcode_enum_t, cvtask_printf()
#include <cvtask_interface.h>

// for memio_source_recv_raw_t
#include "cvapi_memio_interface.h"

#define MAX_NUM_VP_OPS 256U

// Aliased printf functions
#define CRITICAL(fmt,a,b) cvtask_printf(LVL_CRITICAL, (fmt), (a), (b), 0, 0, 0)
#define MINIMAL(fmt,a,b)  cvtask_printf(LVL_MINIMAL,  (fmt), (a), (b), 0, 0, 0)
#define NORMAL(fmt,a,b)   cvtask_printf(LVL_NORMAL,   (fmt), (a), (b), 0, 0, 0)
#define VERBOSE(fmt,a,b)  cvtask_printf(LVL_VERBOSE,  (fmt), (a), (b), 0, 0, 0)
#define DEBUG(fmt,a,b)    cvtask_printf(LVL_DEBUG,    (fmt), (a), (b), 0, 0, 0)
#define CVTASK_PRINT


// Structs for I/O ports
typedef struct {
    // Size: 655360; Padding: 0
    uint8_t Placeholder[1][655360];
} flexidag_openseg_ag_FLEXIDAG_OPENSEG_AG_INP_0_t;

typedef struct {
    // Size: 327680; Padding: 0
    uint8_t Placeholder_uv[1][327680];
} flexidag_openseg_ag_FLEXIDAG_OPENSEG_AG_INP_1_t;

typedef struct {
    // Size: 40960; Padding: 0
    uint8_t out_class[1][40960];
} flexidag_openseg_ag_FLEXIDAG_OPENSEG_AG_OUT_0_t;

typedef struct DRAM_temporary_scratchpad    DRAM_temporary_scratchpad_t;
typedef struct CMEM_temporary_scratchpad    CMEM_temporary_scratchpad_t;
typedef struct instance_private_storage     instance_private_storage_t;
typedef struct instance_private_uncached_storage 
                                            instance_private_uncached_storage_t;

#define AUTOGEN_TASKNAME            "flexidag_openseg_ag"
#define AUTOGEN_TASKNAME_LOWER      "flexidag_openseg_ag"
#define AUTOGEN_NETWORKNAME         "flexidag_openseg_ag"
#define AUTOGEN_NETWORKNAME_UPPER   "FLEXIDAG_OPENSEG_AG"
#define AUTOGEN_VERSION             "cv2.qa.2020-09-07.779"

static void flexidag_openseg_ag_print_debug_info(
    const cvtask_parameter_interface_t *pCVTaskEnv
);

/******************************** BOILERPLATE *********************************/

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

