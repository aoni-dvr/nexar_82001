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

#ifndef _FLEXIDAG_OPENSEG_AG_PRIVATE_H_
#define _FLEXIDAG_OPENSEG_AG_PRIVATE_H_

#include <vp_common.h>          // vp_init(), vp_load_dag()
#include <orc_memory.h>         // dma_cmem_2_coproc()
#include <vis_coproc.h>         // coproc_run(), coproc_poke_*()
#include <cvtask_interface.h>   // CVTASK_PRINTF(), cvtable_find()

#define FLEXIDAG_OPENSEG_AG_SPLIT_COUNT   (10U)
#define FLEXIDAG_OPENSEG_AG_CMEM_BASE     (256U)

struct flexidag_openseg_ag_split_context {
    rptr32_t dram_dag_base;
};

struct flexidag_openseg_ag_instance_private_storage {
    unsigned int split_id;
    struct flexidag_openseg_ag_split_context split_ctxt[FLEXIDAG_OPENSEG_AG_SPLIT_COUNT];
};

struct flexidag_openseg_ag_CMEM_temporary_scratchpad {
    char buf[0];
};

struct flexidag_openseg_ag_DRAM_temporary_scratchpad {
    char buf[0];
};

struct flexidag_openseg_ag_instance_private_uncached_storage {
    char buf[44040192];
};

// Used by flexidag_openseg_ag_init()
inline errcode_enum_t flexidag_openseg_ag_read_cvtable (
    flexidag_openseg_ag_ips_t *ctxt
);

// Used by flexidag_openseg_ag_run()
inline void flexidag_openseg_ag_load_split(
    struct flexidag_openseg_ag_split_context *split_ctxt,
    uint32_t split_id
);
inline void flexidag_openseg_ag_poke_split(
    flexidag_openseg_ag_required_fields_t *r_args,
    flexidag_openseg_ag_optional_fields_t *o_args,
    struct flexidag_openseg_ag_split_context *split_ctxt,
    uint32_t split_id);
inline void flexidag_openseg_ag_run_split(
    uint32_t split_id
);
inline errcode_enum_t flexidag_openseg_ag_peek_split(
    flexidag_openseg_ag_required_fields_t *r_args,
    struct flexidag_openseg_ag_split_context *split_ctxt,
    uint32_t split_id
);


#endif

