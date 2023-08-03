
#include <cvtask_interface.h>
#include <vis_coproc.h>
#include <cvtask_errno.h>
#include <orc_memory.h>
#include <ucode_debug.h>
#include <vp_common.h>

#define MAX_INTERM_OUTPUTS 128

typedef struct bisenet_pelee_picinfo__prim_cntxt_s {
    void* bisenet_pelee_picinfo__prim_split_0_cntxt;
    uint32_t *bisenet_pelee_picinfo__prim_split_0_output[MAX_INTERM_OUTPUTS];

    void* bisenet_pelee_picinfo__prim_split_1_cntxt;
    uint32_t *bisenet_pelee_picinfo__prim_split_1_output[MAX_INTERM_OUTPUTS];

    void* bisenet_pelee_picinfo__prim_split_2_cntxt;
    uint32_t *bisenet_pelee_picinfo__prim_split_2_output[MAX_INTERM_OUTPUTS];

    void* bisenet_pelee_picinfo__prim_split_3_cntxt;
    uint32_t *bisenet_pelee_picinfo__prim_split_3_output[MAX_INTERM_OUTPUTS];

    void* bisenet_pelee_picinfo__prim_split_4_cntxt;
    uint32_t *bisenet_pelee_picinfo__prim_split_4_output[MAX_INTERM_OUTPUTS];

    void* bisenet_pelee_picinfo__prim_split_5_cntxt;
    uint32_t *bisenet_pelee_picinfo__prim_split_5_output[MAX_INTERM_OUTPUTS];

    void* bisenet_pelee_picinfo__prim_split_6_cntxt;
    uint32_t *bisenet_pelee_picinfo__prim_split_6_output[MAX_INTERM_OUTPUTS];

    void* bisenet_pelee_picinfo__prim_split_7_cntxt;
    uint32_t *bisenet_pelee_picinfo__prim_split_7_output[MAX_INTERM_OUTPUTS];

    void* bisenet_pelee_picinfo__prim_split_8_cntxt;
    uint32_t *bisenet_pelee_picinfo__prim_split_8_output[MAX_INTERM_OUTPUTS];

} bisenet_pelee_picinfo__prim_cntxt_t;

errcode_enum_t bisenet_pelee_picinfo__prim_dag_query(int32_t* cntxt_sz, int32_t* interm_out_sz, int32_t* task_out_sz)
{
    errcode_enum_t retcode;

    int32_t sub_cntxt_sz, sub_interm_out_sz;
    int32_t sub_task_out_sz[MAX_INTERM_OUTPUTS];

    *cntxt_sz = sizeof(bisenet_pelee_picinfo__prim_cntxt_t);
    *interm_out_sz = 0;

    retcode = bisenet_pelee_picinfo__prim_split_0_dag_query(&sub_cntxt_sz, &sub_interm_out_sz, &sub_task_out_sz[0]);
    if(retcode != ERRCODE_NONE) return retcode;
    *cntxt_sz += sub_cntxt_sz;
    *interm_out_sz += (0 + sub_task_out_sz[0] + sub_task_out_sz[1]);

    retcode = bisenet_pelee_picinfo__prim_split_1_dag_query(&sub_cntxt_sz, &sub_interm_out_sz, &sub_task_out_sz[0]);
    if(retcode != ERRCODE_NONE) return retcode;
    *cntxt_sz += sub_cntxt_sz;
    *interm_out_sz += (0 + sub_task_out_sz[0] + sub_task_out_sz[1]);

    retcode = bisenet_pelee_picinfo__prim_split_2_dag_query(&sub_cntxt_sz, &sub_interm_out_sz, &sub_task_out_sz[0]);
    if(retcode != ERRCODE_NONE) return retcode;
    *cntxt_sz += sub_cntxt_sz;
    *interm_out_sz += (0 + sub_task_out_sz[0] + sub_task_out_sz[1]);

    retcode = bisenet_pelee_picinfo__prim_split_3_dag_query(&sub_cntxt_sz, &sub_interm_out_sz, &sub_task_out_sz[0]);
    if(retcode != ERRCODE_NONE) return retcode;
    *cntxt_sz += sub_cntxt_sz;
    *interm_out_sz += (0 + sub_task_out_sz[0]);

    retcode = bisenet_pelee_picinfo__prim_split_4_dag_query(&sub_cntxt_sz, &sub_interm_out_sz, &sub_task_out_sz[0]);
    if(retcode != ERRCODE_NONE) return retcode;
    *cntxt_sz += sub_cntxt_sz;
    *interm_out_sz += (0 + sub_task_out_sz[0]);

    retcode = bisenet_pelee_picinfo__prim_split_5_dag_query(&sub_cntxt_sz, &sub_interm_out_sz, &sub_task_out_sz[0]);
    if(retcode != ERRCODE_NONE) return retcode;
    *cntxt_sz += sub_cntxt_sz;
    *interm_out_sz += (0 + sub_task_out_sz[0]);

    retcode = bisenet_pelee_picinfo__prim_split_6_dag_query(&sub_cntxt_sz, &sub_interm_out_sz, &sub_task_out_sz[0]);
    if(retcode != ERRCODE_NONE) return retcode;
    *cntxt_sz += sub_cntxt_sz;
    *interm_out_sz += (0 + sub_task_out_sz[0]);

    retcode = bisenet_pelee_picinfo__prim_split_7_dag_query(&sub_cntxt_sz, &sub_interm_out_sz, &sub_task_out_sz[0]);
    if(retcode != ERRCODE_NONE) return retcode;
    *cntxt_sz += sub_cntxt_sz;
    *interm_out_sz += (0 + sub_task_out_sz[0] + sub_task_out_sz[1]);

    retcode = bisenet_pelee_picinfo__prim_split_8_dag_query(&sub_cntxt_sz, &sub_interm_out_sz, &sub_task_out_sz[0]);
    if(retcode != ERRCODE_NONE) return retcode;
    *cntxt_sz += sub_cntxt_sz;
    *interm_out_sz += (0);
    task_out_sz[0] = sub_task_out_sz[0];

    return ERRCODE_NONE;
}

errcode_enum_t bisenet_pelee_picinfo__prim_dag_init(void* internal_cntxt, int32_t avail_sz, int32_t *used_sz)
{

    errcode_enum_t retcode;
    bisenet_pelee_picinfo__prim_cntxt_t *cntxt = (bisenet_pelee_picinfo__prim_cntxt_t*) internal_cntxt;
    uint8_t *base = (uint8_t *)cntxt;
    int32_t used_size = sizeof(bisenet_pelee_picinfo__prim_cntxt_t);
    avail_sz -= used_size;
    *used_sz = used_size;

    base += used_size;
    cntxt->bisenet_pelee_picinfo__prim_split_0_cntxt = base;
    retcode = bisenet_pelee_picinfo__prim_split_0_dag_init(base, avail_sz, &used_size);
    if(retcode != ERRCODE_NONE) return retcode;
    avail_sz -= used_size;
    *used_sz += used_size;

    base += used_size;
    cntxt->bisenet_pelee_picinfo__prim_split_1_cntxt = base;
    retcode = bisenet_pelee_picinfo__prim_split_1_dag_init(base, avail_sz, &used_size);
    if(retcode != ERRCODE_NONE) return retcode;
    avail_sz -= used_size;
    *used_sz += used_size;

    base += used_size;
    cntxt->bisenet_pelee_picinfo__prim_split_2_cntxt = base;
    retcode = bisenet_pelee_picinfo__prim_split_2_dag_init(base, avail_sz, &used_size);
    if(retcode != ERRCODE_NONE) return retcode;
    avail_sz -= used_size;
    *used_sz += used_size;

    base += used_size;
    cntxt->bisenet_pelee_picinfo__prim_split_3_cntxt = base;
    retcode = bisenet_pelee_picinfo__prim_split_3_dag_init(base, avail_sz, &used_size);
    if(retcode != ERRCODE_NONE) return retcode;
    avail_sz -= used_size;
    *used_sz += used_size;

    base += used_size;
    cntxt->bisenet_pelee_picinfo__prim_split_4_cntxt = base;
    retcode = bisenet_pelee_picinfo__prim_split_4_dag_init(base, avail_sz, &used_size);
    if(retcode != ERRCODE_NONE) return retcode;
    avail_sz -= used_size;
    *used_sz += used_size;

    base += used_size;
    cntxt->bisenet_pelee_picinfo__prim_split_5_cntxt = base;
    retcode = bisenet_pelee_picinfo__prim_split_5_dag_init(base, avail_sz, &used_size);
    if(retcode != ERRCODE_NONE) return retcode;
    avail_sz -= used_size;
    *used_sz += used_size;

    base += used_size;
    cntxt->bisenet_pelee_picinfo__prim_split_6_cntxt = base;
    retcode = bisenet_pelee_picinfo__prim_split_6_dag_init(base, avail_sz, &used_size);
    if(retcode != ERRCODE_NONE) return retcode;
    avail_sz -= used_size;
    *used_sz += used_size;

    base += used_size;
    cntxt->bisenet_pelee_picinfo__prim_split_7_cntxt = base;
    retcode = bisenet_pelee_picinfo__prim_split_7_dag_init(base, avail_sz, &used_size);
    if(retcode != ERRCODE_NONE) return retcode;
    avail_sz -= used_size;
    *used_sz += used_size;

    base += used_size;
    cntxt->bisenet_pelee_picinfo__prim_split_8_cntxt = base;
    retcode = bisenet_pelee_picinfo__prim_split_8_dag_init(base, avail_sz, &used_size);
    if(retcode != ERRCODE_NONE) return retcode;
    avail_sz -= used_size;
    *used_sz += used_size;

    return ERRCODE_NONE;
}

errcode_enum_t bisenet_pelee_picinfo__prim_dag_rundag(void* internal_cntxt, void* dram_scratchpad, int32_t dram_scratchpad_sz, uint32_t* in_buf_ptr, uint32_t* out_buf_ptr, uint32_t roi_pitch_m1)
{
    errcode_enum_t retcode;

    uint32_t i;
    uint32_t *interm_buf_ptrs[MAX_INTERM_OUTPUTS];
    bisenet_pelee_picinfo__prim_cntxt_t *cntxt = (bisenet_pelee_picinfo__prim_cntxt_t *) internal_cntxt;

    uint8_t *base = (uint8_t *)dram_scratchpad;
    int32_t avail_sz = dram_scratchpad_sz;
    int32_t used_sz = 0;

    for(i = 0; i < MAX_INTERM_OUTPUTS; i++) interm_buf_ptrs[i] = 0;
    interm_buf_ptrs[0] = (uint32_t *) in_buf_ptr[0];
    interm_buf_ptrs[1] = (uint32_t *) in_buf_ptr[1];

    base += used_sz;
    used_sz = 0;
    retcode = bisenet_pelee_picinfo__prim_split_0_dag_out_buf_list(out_buf_ptr, base, avail_sz, &used_sz, cntxt->bisenet_pelee_picinfo__prim_split_0_output);
    if(retcode != ERRCODE_NONE) return retcode;
    avail_sz -= used_sz;

    retcode = bisenet_pelee_picinfo__prim_split_0_dag_rundag(cntxt->bisenet_pelee_picinfo__prim_split_0_cntxt, NULL, 0, interm_buf_ptrs, cntxt->bisenet_pelee_picinfo__prim_split_0_output, roi_pitch_m1);
    if(retcode != ERRCODE_NONE) return retcode;

    for(i = 0; i < MAX_INTERM_OUTPUTS; i++) interm_buf_ptrs[i] = 0;
    interm_buf_ptrs[0] = cntxt->bisenet_pelee_picinfo__prim_split_0_output[1];

    base += used_sz;
    used_sz = 0;
    retcode = bisenet_pelee_picinfo__prim_split_1_dag_out_buf_list(out_buf_ptr, base, avail_sz, &used_sz, cntxt->bisenet_pelee_picinfo__prim_split_1_output);
    if(retcode != ERRCODE_NONE) return retcode;
    avail_sz -= used_sz;

    retcode = bisenet_pelee_picinfo__prim_split_1_dag_rundag(cntxt->bisenet_pelee_picinfo__prim_split_1_cntxt, NULL, 0, interm_buf_ptrs, cntxt->bisenet_pelee_picinfo__prim_split_1_output, roi_pitch_m1);
    if(retcode != ERRCODE_NONE) return retcode;

    for(i = 0; i < MAX_INTERM_OUTPUTS; i++) interm_buf_ptrs[i] = 0;
    interm_buf_ptrs[0] = cntxt->bisenet_pelee_picinfo__prim_split_1_output[0];
    interm_buf_ptrs[1] = cntxt->bisenet_pelee_picinfo__prim_split_1_output[1];
    interm_buf_ptrs[2] = cntxt->bisenet_pelee_picinfo__prim_split_0_output[0];

    base += used_sz;
    used_sz = 0;
    retcode = bisenet_pelee_picinfo__prim_split_2_dag_out_buf_list(out_buf_ptr, base, avail_sz, &used_sz, cntxt->bisenet_pelee_picinfo__prim_split_2_output);
    if(retcode != ERRCODE_NONE) return retcode;
    avail_sz -= used_sz;

    retcode = bisenet_pelee_picinfo__prim_split_2_dag_rundag(cntxt->bisenet_pelee_picinfo__prim_split_2_cntxt, NULL, 0, interm_buf_ptrs, cntxt->bisenet_pelee_picinfo__prim_split_2_output, roi_pitch_m1);
    if(retcode != ERRCODE_NONE) return retcode;

    for(i = 0; i < MAX_INTERM_OUTPUTS; i++) interm_buf_ptrs[i] = 0;
    interm_buf_ptrs[0] = cntxt->bisenet_pelee_picinfo__prim_split_2_output[0];

    base += used_sz;
    used_sz = 0;
    retcode = bisenet_pelee_picinfo__prim_split_3_dag_out_buf_list(out_buf_ptr, base, avail_sz, &used_sz, cntxt->bisenet_pelee_picinfo__prim_split_3_output);
    if(retcode != ERRCODE_NONE) return retcode;
    avail_sz -= used_sz;

    retcode = bisenet_pelee_picinfo__prim_split_3_dag_rundag(cntxt->bisenet_pelee_picinfo__prim_split_3_cntxt, NULL, 0, interm_buf_ptrs, cntxt->bisenet_pelee_picinfo__prim_split_3_output, roi_pitch_m1);
    if(retcode != ERRCODE_NONE) return retcode;

    for(i = 0; i < MAX_INTERM_OUTPUTS; i++) interm_buf_ptrs[i] = 0;
    interm_buf_ptrs[0] = cntxt->bisenet_pelee_picinfo__prim_split_3_output[0];

    base += used_sz;
    used_sz = 0;
    retcode = bisenet_pelee_picinfo__prim_split_4_dag_out_buf_list(out_buf_ptr, base, avail_sz, &used_sz, cntxt->bisenet_pelee_picinfo__prim_split_4_output);
    if(retcode != ERRCODE_NONE) return retcode;
    avail_sz -= used_sz;

    retcode = bisenet_pelee_picinfo__prim_split_4_dag_rundag(cntxt->bisenet_pelee_picinfo__prim_split_4_cntxt, NULL, 0, interm_buf_ptrs, cntxt->bisenet_pelee_picinfo__prim_split_4_output, roi_pitch_m1);
    if(retcode != ERRCODE_NONE) return retcode;

    for(i = 0; i < MAX_INTERM_OUTPUTS; i++) interm_buf_ptrs[i] = 0;
    interm_buf_ptrs[0] = cntxt->bisenet_pelee_picinfo__prim_split_4_output[0];

    base += used_sz;
    used_sz = 0;
    retcode = bisenet_pelee_picinfo__prim_split_5_dag_out_buf_list(out_buf_ptr, base, avail_sz, &used_sz, cntxt->bisenet_pelee_picinfo__prim_split_5_output);
    if(retcode != ERRCODE_NONE) return retcode;
    avail_sz -= used_sz;

    retcode = bisenet_pelee_picinfo__prim_split_5_dag_rundag(cntxt->bisenet_pelee_picinfo__prim_split_5_cntxt, NULL, 0, interm_buf_ptrs, cntxt->bisenet_pelee_picinfo__prim_split_5_output, roi_pitch_m1);
    if(retcode != ERRCODE_NONE) return retcode;

    for(i = 0; i < MAX_INTERM_OUTPUTS; i++) interm_buf_ptrs[i] = 0;
    interm_buf_ptrs[0] = cntxt->bisenet_pelee_picinfo__prim_split_1_output[0];

    base += used_sz;
    used_sz = 0;
    retcode = bisenet_pelee_picinfo__prim_split_6_dag_out_buf_list(out_buf_ptr, base, avail_sz, &used_sz, cntxt->bisenet_pelee_picinfo__prim_split_6_output);
    if(retcode != ERRCODE_NONE) return retcode;
    avail_sz -= used_sz;

    retcode = bisenet_pelee_picinfo__prim_split_6_dag_rundag(cntxt->bisenet_pelee_picinfo__prim_split_6_cntxt, NULL, 0, interm_buf_ptrs, cntxt->bisenet_pelee_picinfo__prim_split_6_output, roi_pitch_m1);
    if(retcode != ERRCODE_NONE) return retcode;

    for(i = 0; i < MAX_INTERM_OUTPUTS; i++) interm_buf_ptrs[i] = 0;
    interm_buf_ptrs[0] = cntxt->bisenet_pelee_picinfo__prim_split_6_output[0];
    interm_buf_ptrs[1] = cntxt->bisenet_pelee_picinfo__prim_split_5_output[0];
    interm_buf_ptrs[2] = cntxt->bisenet_pelee_picinfo__prim_split_2_output[1];

    base += used_sz;
    used_sz = 0;
    retcode = bisenet_pelee_picinfo__prim_split_7_dag_out_buf_list(out_buf_ptr, base, avail_sz, &used_sz, cntxt->bisenet_pelee_picinfo__prim_split_7_output);
    if(retcode != ERRCODE_NONE) return retcode;
    avail_sz -= used_sz;

    retcode = bisenet_pelee_picinfo__prim_split_7_dag_rundag(cntxt->bisenet_pelee_picinfo__prim_split_7_cntxt, NULL, 0, interm_buf_ptrs, cntxt->bisenet_pelee_picinfo__prim_split_7_output, roi_pitch_m1);
    if(retcode != ERRCODE_NONE) return retcode;

    for(i = 0; i < MAX_INTERM_OUTPUTS; i++) interm_buf_ptrs[i] = 0;
    interm_buf_ptrs[0] = cntxt->bisenet_pelee_picinfo__prim_split_7_output[0];
    interm_buf_ptrs[1] = cntxt->bisenet_pelee_picinfo__prim_split_7_output[1];

    base += used_sz;
    used_sz = 0;
    retcode = bisenet_pelee_picinfo__prim_split_8_dag_out_buf_list(out_buf_ptr, base, avail_sz, &used_sz, cntxt->bisenet_pelee_picinfo__prim_split_8_output);
    if(retcode != ERRCODE_NONE) return retcode;
    avail_sz -= used_sz;

    retcode = bisenet_pelee_picinfo__prim_split_8_dag_rundag(cntxt->bisenet_pelee_picinfo__prim_split_8_cntxt, NULL, 0, interm_buf_ptrs, cntxt->bisenet_pelee_picinfo__prim_split_8_output, roi_pitch_m1);
    if(retcode != ERRCODE_NONE) return retcode;

    return ERRCODE_NONE;
}
