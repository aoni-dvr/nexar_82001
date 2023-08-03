
#include <cvtask_interface.h>
#include <vis_coproc.h>
#include <cvtask_errno.h>
#include <orc_memory.h>
#include <ucode_debug.h>
#include <vp_common.h>

#include "_bisenet_pelee_picinfo__split_4.h"
// DAG_NAME "bisenet_pelee_picinfo__prim_split_4"

#define CMEM_DAG_BASE (0x100)

typedef struct bisenet_pelee_picinfo__prim_split_4_cntxt_s {
    uint8_t *dag_base;
    uint8_t *ddi_base;
} bisenet_pelee_picinfo__prim_split_4_cntxt_t;

errcode_enum_t bisenet_pelee_picinfo__prim_split_4_dag_query(int32_t* cntxt_sz, int32_t* interm_out_sz, int32_t* task_out_sz)
{
    *cntxt_sz = sizeof(bisenet_pelee_picinfo__prim_split_4_cntxt_t);
    *interm_out_sz = 0;

    task_out_sz[0] = bisenet_pelee_picinfo__prim_split_4_PeleeNet__stage_3_dense_block_7__concat_dram_size_num_bytes;

    return ERRCODE_NONE;
}

errcode_enum_t bisenet_pelee_picinfo__prim_split_4_dag_init(void* internal_cntxt, int32_t avail_sz, int32_t *used_sz)
{
    errcode_enum_t retcode = ERRCODE_NONE;

    int32_t dag_size = bisenet_pelee_picinfo__prim_split_4_image_size;
    int32_t table_size;
    int32_t used_size = sizeof(bisenet_pelee_picinfo__prim_split_4_cntxt_t);

    if(avail_sz < 0) {
        CVTASK_PRINTF(LVL_CRITICAL, "Error in bisenet_pelee_picinfo__prim_split_4_dag_init(): avail_sz less than 0\n", avail_sz, 0, 0, 0, 0);
        return ERRCODE_GENERIC;
    }

    bisenet_pelee_picinfo__prim_split_4_cntxt_t *cntxt = (bisenet_pelee_picinfo__prim_split_4_cntxt_t *) internal_cntxt;

    retcode = cvtable_find("BISENET_PELEE_PICINFO__PRIM_SPLIT_4_DAG", (const void **)&cntxt->dag_base, &table_size);
    if (is_err(retcode)) {
        CVTASK_PRINTF(LVL_CRITICAL, "Error in bisenet_pelee_picinfo__prim_split_4_dag_init(): Cannot find BISENET_PELEE_PICINFO__PRIM_SPLIT_4_DAG in table\n", 0, 0, 0, 0, 0);
        return retcode;
    } else {
        if(table_size != dag_size) {
            CVTASK_PRINTF(LVL_CRITICAL, "Error in bisenet_pelee_picinfo__prim_split_4_dag_init(): table_size (%d) != dag_size (%d)\n", table_size, dag_size, 0, 0, 0);
            return ERRCODE_GENERIC;
        }
    }

    retcode = cvtable_find("BISENET_PELEE_PICINFO__PRIM_SPLIT_4_DDI", (const void **)&cntxt->ddi_base, &table_size);
    if (is_err(retcode)) {
        CVTASK_PRINTF(LVL_CRITICAL, "bisenet_pelee_picinfo__prim_split_4_dag_init() DDI entry not found. All weights in DVI\n", 0, 0, 0, 0, 0);
        retcode = ERRCODE_NONE;
    } else if(table_size == 0) {
        CVTASK_PRINTF(LVL_CRITICAL, "Error in bisenet_pelee_picinfo__prim_split_4_dag_init(): BISENET_PELEE_PICINFO__PRIM_SPLIT_4_DDI in table is 0 size\n", 0, 0, 0, 0, 0);
        return ERRCODE_GENERIC;
    } else {
        CVTASK_PRINTF(LVL_DEBUG, "bisenet_pelee_picinfo__prim_split_4_dag_init(): ddi_base(0x%X) dag_base(0x%X)\n", cntxt->ddi_base, cntxt->dag_base, 0, 0, 0);
    }

    if(used_size > avail_sz) {
        CVTASK_PRINTF(LVL_CRITICAL, "Error in bisenet_pelee_picinfo__prim_split_4_dag_init(): used_size (%d) > available_size(%d)\n", used_size, avail_sz, 0, 0, 0);
        return ERRCODE_GENERIC;
    }

    *used_sz = used_size;

    return retcode;
}

errcode_enum_t bisenet_pelee_picinfo__prim_split_4_dag_rundag(void* internal_cntxt, void* dram_scratchpad, int32_t dram_scratchpad_sz, uint32_t* in_buf_ptr, uint32_t* out_buf_ptr, uint32_t roi_pitch_m1)
{

    bisenet_pelee_picinfo__prim_split_4_cntxt_t *cntxt = (bisenet_pelee_picinfo__prim_split_4_cntxt_t *) internal_cntxt;
    uint32_t dag_base = (uint32_t ) cntxt->dag_base;

    uint32_t start_time, clk_time;

    start_time = get_cur_time();

    vp_init();
    vp_load_dag(dag_base, bisenet_pelee_picinfo__prim_split_4_image_start, bisenet_pelee_picinfo__prim_split_4_image_size, 1);

    dma_cmem_2_coproc(0x100, CMEM_DAG_BASE, 1024);


    clk_time = get_cur_time() - start_time;
    CVTASK_PRINTF(LVL_DEBUG, ">>>>> bisenet_pelee_picinfo__prim_split_4_dag_rundag load dag: %d", clk_time, 0, 0, 0, 0);

    coproc_poke_word((uint32_t )in_buf_ptr[0], bisenet_pelee_picinfo__prim_split_4_dagbin_start + bisenet_pelee_picinfo__prim_split_4_PeleeNet__stage_3_dense_block_4__concat_dbase_byte_offset);
    //coproc_poke_half((uint32_t ) (bisenet_pelee_picinfo__prim_split_4_PeleeNet__stage_3_dense_block_4__concat_dpitch_num_bytes - 1), bisenet_pelee_picinfo__prim_split_4_dagbin_start + bisenet_pelee_picinfo__prim_split_4_PeleeNet__stage_3_dense_block_4__concat_dpitchm1_byte_offset);

    coproc_poke_word((uint32_t )out_buf_ptr[0], bisenet_pelee_picinfo__prim_split_4_dagbin_start + bisenet_pelee_picinfo__prim_split_4_PeleeNet__stage_3_dense_block_7__concat_dbase_byte_offset);
    //coproc_poke_half((uint32_t ) (bisenet_pelee_picinfo__prim_split_4_PeleeNet__stage_3_dense_block_7__concat_dpitch_num_bytes - 1), bisenet_pelee_picinfo__prim_split_4_dagbin_start + bisenet_pelee_picinfo__prim_split_4_PeleeNet__stage_3_dense_block_7__concat_dpitchm1_byte_offset);

    start_time = get_cur_time();

    coproc_run(bisenet_pelee_picinfo__prim_split_4_dagbin_start);
    wait(VIS_W_COPROC);

    clk_time = get_cur_time() - start_time;
    CVTASK_PRINTF(LVL_DEBUG, ">>>>> bisenet_pelee_picinfo__prim_split_4_dag_rundag: %d", clk_time, 0, 0, 0, 0);

    return ERRCODE_NONE;
}


errcode_enum_t bisenet_pelee_picinfo__prim_split_4_dag_out_buf_list(uint32_t* cvtask_out_list, uint8_t* interm_out_base, int32_t avail_sz, int32_t *used_sz, uint32_t* out_buf_list)
{
    errcode_enum_t retcode = ERRCODE_NONE;
    int32_t i;
    int32_t used_size = 0;

    if(avail_sz < 0) {
        CVTASK_PRINTF(LVL_CRITICAL, "Error in bisenet_pelee_picinfo__prim_split_4_dag_out_buf_list(): avail_sz less than 0\n", avail_sz, 0, 0, 0, 0);
        return ERRCODE_GENERIC;
    }

    // cvtask_out_list indices match that in run function of cvtask

    out_buf_list[0] = (uint32_t ) (interm_out_base + used_size);
    used_size += bisenet_pelee_picinfo__prim_split_4_PeleeNet__stage_3_dense_block_7__concat_dram_size_num_bytes;

    if(used_size > avail_sz) {
        CVTASK_PRINTF(LVL_CRITICAL, "Error in bisenet_pelee_picinfo__prim_split_4_dag_out_buf_list(): used_size (%d) > available_size (%d)\n", used_size, avail_sz, 0, 0, 0);
        return ERRCODE_GENERIC;
    }

    *used_sz = used_size;

    for(i = 0; i < 1; i++) {
        CVTASK_PRINTF(LVL_DEBUG, "out_buf_list_fn_name: out_buf_list[%d]: 0x%x\n", i, out_buf_list[i], 0, 0, 0);
    }

    return retcode;
}
