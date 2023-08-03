#include <ucode_debug.h>
#include <orc_memory.h>   // for visdma
#include <vp_common.h>
#include "cvapi_idsp_interface.h"
#include "cvtask_imgproc_func.h"
#include "cvtask_adj.h"
#include "cvtask_imgproc.h"


errcode_enum_t read_memory(awb_sim_ccb_t *ccb, awb_sim_scb_t *scb, uint32_t da, uint32_t dataSize,uint32_t memIndex, uint32_t prtFlag)
{
    errcode_enum_t rc = ERRCODE_NONE;
    int i, j, ping_pong = 1;
    int k;
    uint32_t  start_time, end_time;
    int per_param_size = 4;  //uint32_t,int32_t ...........4 bytes
    int num_of_params_per_cache = 128;  //cache size is 512, 512/4 = 128
    int num_of_params_per_cache_def = 128;
    int dma_xfer_size = per_param_size * num_of_params_per_cache;  //4*128   = 512
    int num_of_itr;     //the times for reading cmem (the times for moving cmem cache to stack memory)
    int num_of_nowait;  //the times for reading cmem with no wait
    int num_of_remainder;
    uint32_t *word_ptr0[2], *wptr_0;

    start_time = get_cur_time();

    num_of_itr = dataSize / dma_xfer_size;  //siezof(idsp filter)/512
    num_of_remainder = (dataSize % (num_of_itr*dma_xfer_size));

    word_ptr0[0] = (uint32_t*)ccb->cache[0];  //pointer to cache
    word_ptr0[1] = (uint32_t*)ccb->cache[1];

    if(num_of_remainder > 0) {
        num_of_params_per_cache = num_of_remainder >> 2;
        visdma_dram_2_cmem(ccb->cache[0], da, (num_of_remainder >> 2));
        da += num_of_remainder;
        num_of_nowait = num_of_itr;
        num_of_itr += 1;
    } else {
        visdma_dram_2_cmem(ccb->cache[0], da, (dma_xfer_size >> 2));
        da += dma_xfer_size;
        num_of_nowait = num_of_itr - 1;
    }

    k = 0;
    for (i=0; i<num_of_itr; i++) { // loop through cache lines
        if(num_of_nowait > 0) {
            visdma_dram_2_cmem_nowait(ccb->cache[ping_pong], da, ((dma_xfer_size) >> 2));
            da += dma_xfer_size;
            num_of_nowait --;
        }
        ping_pong = !ping_pong; // point to the other ping-pong buffer, which should be ready now

        for (j=0; j<num_of_params_per_cache; j++) {
            wptr_0 = word_ptr0[ping_pong] + j;
            scb->temp[memIndex][k] = *wptr_0;
            if(prtFlag ==1) {
                CVTASK_PRINTF(LVL_CRITICAL, "read_memory[wptr_0][%d] : %d, \n",
                              k, *wptr_0, 0, 0, 0);
            }
            k += 1;
        }
        num_of_params_per_cache = num_of_params_per_cache_def;
        wait_vis_w_dram_dma(); // wait for last nowait DMA to finish
    }

    end_time =  get_cur_time();
    if(prtFlag ==1) {
        CVTASK_PRINTF(LVL_CRITICAL, "[read_memory Time] :%10u->%10u  total:%10u\n",
                      start_time, end_time, (end_time-start_time), 0, 0);
    }
    return rc;
}

errcode_enum_t filter_check(awb_sim_ccb_t *ccb, awb_sim_scb_t *scb, AMBA_AAA_Flexidag_INFO_t *inputdata, Adj_Filter_Info_t adjFilterInfo)
{
    errcode_enum_t rc = ERRCODE_NONE;
    int i, j, ping_pong = 1;
    int k;
    int error_code = 0;
    uint32_t  start_time, end_time;

    int per_param_size = 4;  //uint32_t,int32_t ...........4 bytes
    int num_of_params_per_cache = 128;
    int num_of_params_per_cache_def = 128;
    int dma_xfer_size = per_param_size * num_of_params_per_cache;//     4*128   = 512
    int num_of_itr;
    int num_of_nowait;
    int num_of_remainder;

    uint32_t da_0,da_1,da_2;
    uint32_t *word_ptr0[2], *wptr;

    start_time = get_cur_time();

    da_0 =  adjFilterInfo.PreFilterAdd;
    read_memory(ccb, scb, da_0, adjFilterInfo.Size,0,0);

    da_1 =  adjFilterInfo.FilterAdd;
    read_memory(ccb, scb, da_1, adjFilterInfo.Size,1,0);

    da_2 =  adjFilterInfo.NextFilterAdd;
    num_of_itr = adjFilterInfo.Size / dma_xfer_size;  //    siezof(idsp filter)/512
    num_of_remainder = (adjFilterInfo.Size % (num_of_itr*dma_xfer_size));

    word_ptr0[0] = (uint32_t*)ccb->cache[0];
    word_ptr0[1] = (uint32_t*)ccb->cache[1];

    if(num_of_remainder > 0) {
        num_of_params_per_cache = num_of_remainder >> 2;
        visdma_dram_2_cmem(ccb->cache[0], da_2, (num_of_remainder >> 2));
        da_2 += num_of_remainder;
        num_of_nowait = num_of_itr;
        num_of_itr += 1;
    } else {
        visdma_dram_2_cmem(ccb->cache[0], da_2, (dma_xfer_size >> 2));
        da_2 += dma_xfer_size;
        num_of_nowait = num_of_itr - 1;
    }

    k = 0;
    for (i=0; i<num_of_itr; i++) { // loop through cache lines

        if(num_of_nowait > 0) {
            visdma_dram_2_cmem_nowait(ccb->cache[ping_pong], da_2, ((dma_xfer_size) >> 2));
            da_2 += dma_xfer_size;
            num_of_nowait --;
        }
        ping_pong = !ping_pong; // point to the other ping-pong buffer, which should be ready now

        for (j=0; j<num_of_params_per_cache; j++) {
            wptr = word_ptr0[ping_pong] + j;
            if(((scb->temp[1][k]>=scb->temp[0][k]) && (*wptr>=scb->temp[1][k])) || ((scb->temp[1][k]<=scb->temp[0][k]) && (*wptr<=scb->temp[1][k]))) {
                error_code = 0;
            } else {
                error_code = 1;
            }

            // CVTASK_PRINTF(LVL_CRITICAL, "filter_check [%d] : %d, %d, %d \n",
            // k, scb->temp[0][k], scb->temp[1][k], *wptr, 0);
            k += 1;
        }
        num_of_params_per_cache = num_of_params_per_cache_def;
        wait_vis_w_dram_dma(); // wait for last nowait DMA to finish
    }

    end_time =  get_cur_time();
    CVTASK_PRINTF(LVL_CRITICAL, "[filter_check Time] :%10u->%10u  total:%10u\n",
                  start_time, end_time, (end_time-start_time), 0, 0);
    return rc;
}

errcode_enum_t filter_check_cmem(awb_sim_ccb_t *ccb, awb_sim_scb_t *scb, AMBA_AAA_Flexidag_INFO_t *inputdata, Adj_Filter_Info_t adjFilterInfo)
{
    errcode_enum_t rc = ERRCODE_NONE;
    int i, j, ping_pong = 1;
    int k;
    int error_code = 0;
    uint32_t  start_time, end_time;

    int per_param_size = 4;  //uint32_t,int32_t ...........4 bytes
    int num_of_params_per_cache = 128;
    int num_of_params_per_cache_def = 128;
    int dma_xfer_size = per_param_size * num_of_params_per_cache;//     4*128   = 512
    int num_of_itr;
    int num_of_nowait;
    int num_of_remainder;

    uint32_t da_0,da_1,da_2;
    uint32_t *word_ptr0[2], *wptr;
    uint32_t *wptr_pre;
    uint32_t *wptr_cal;

    start_time = get_cur_time();

    da_0 =  adjFilterInfo.PreFilterAdd;
    da_1 =  adjFilterInfo.FilterAdd;
    da_2 =  adjFilterInfo.NextFilterAdd;
    num_of_itr = adjFilterInfo.Size / dma_xfer_size;  //    siezof(idsp filter)/512
    num_of_remainder = (adjFilterInfo.Size % (num_of_itr*dma_xfer_size));

    word_ptr0[0] = (uint32_t*)ccb->cache[0];
    word_ptr0[1] = (uint32_t*)ccb->cache[1];

    wptr_pre = (uint32_t*)ccb->cache_pre;
    wptr_cal = (uint32_t*)ccb->cache_cal;

    if(num_of_remainder > 0) {
        num_of_params_per_cache = num_of_remainder >> 2;
        visdma_dram_2_cmem(ccb->cache_pre, da_0, (num_of_remainder >> 2));
        visdma_dram_2_cmem(ccb->cache_cal, da_1, (num_of_remainder >> 2));
        visdma_dram_2_cmem(ccb->cache[0], da_2, (num_of_remainder >> 2));
        da_0 += num_of_remainder;
        da_1 += num_of_remainder;
        da_2 += num_of_remainder;
        num_of_nowait = num_of_itr;
        num_of_itr += 1;
    } else {
        visdma_dram_2_cmem(ccb->cache_pre, da_0, (num_of_remainder >> 2));
        visdma_dram_2_cmem(ccb->cache_cal, da_1, (num_of_remainder >> 2));
        visdma_dram_2_cmem(ccb->cache[0], da_2, (dma_xfer_size >> 2));
        da_0 += dma_xfer_size;
        da_1 += dma_xfer_size;
        da_2 += dma_xfer_size;
        num_of_nowait = num_of_itr - 1;
    }

    k = 0;
    for (i=0; i<num_of_itr; i++) { // loop through cache lines

        if(num_of_nowait > 0) {
            visdma_dram_2_cmem_nowait(ccb->cache[ping_pong], da_2, ((dma_xfer_size) >> 2));
            da_2 += dma_xfer_size;
            // num_of_nowait --;
        }
        ping_pong = !ping_pong; // point to the other ping-pong buffer, which should be ready now

        for (j=0; j<num_of_params_per_cache; j++) {
            wptr = word_ptr0[ping_pong] + j;
            if(((*(wptr_cal+j)>=*(wptr_pre+j)) && (*wptr>=*(wptr_cal+j))) || ((*(wptr_cal+j)<=*(wptr_pre+j)) && (*wptr<=*(wptr_cal+j)))) {
                error_code = 0;
            } else {
                error_code = 1;
            }
            // CVTASK_PRINTF(LVL_CRITICAL, "filter_check_cmem [%d] : %d, %d, %d \n",
            // k, *(wptr_pre+j), *(wptr_cal+j), *wptr, 0);
            k += 1;
        }
        num_of_params_per_cache = num_of_params_per_cache_def;
        wait_vis_w_dram_dma(); // wait for last nowait DMA to finish

        if(num_of_nowait > 0) {
            visdma_dram_2_cmem(ccb->cache_pre, da_0, (dma_xfer_size >> 2));
            visdma_dram_2_cmem(ccb->cache_cal, da_1, (dma_xfer_size >> 2));
            da_0 += dma_xfer_size;
            da_1 += dma_xfer_size;
            num_of_nowait --;
        }
    }

    end_time =  get_cur_time();
    CVTASK_PRINTF(LVL_CRITICAL, "[filter_check_cmem Time] :%10u->%10u  total:%10u\n",
                  start_time, end_time, (end_time-start_time), 0, 0);
    return rc;
}

errcode_enum_t all_filter_check(awb_sim_ccb_t *ccb, awb_sim_scb_t *scb, AMBA_AAA_Flexidag_INFO_t *inputdata)
{
    errcode_enum_t rc = ERRCODE_NONE;
    uint32_t  start_time, end_time;

    Adj_Filter_Info_t adjFilterInfo;

    start_time = get_cur_time();
    CVTASK_PRINTF(LVL_CRITICAL, "[AMBA_IK_DYNAMIC_BAD_PXL_COR_s] ", 0, 0, 0, 0, 0);
    adjFilterInfo.PreFilterAdd = (uint32_t)&inputdata->pPipe0->BadpixCorr;
    adjFilterInfo.FilterAdd = (uint32_t)&inputdata->pPipe1->BadpixCorr;
    adjFilterInfo.NextFilterAdd = (uint32_t)&inputdata->pPipe2->BadpixCorr;
    adjFilterInfo.Size = sizeof(AMBA_IK_DYNAMIC_BAD_PXL_COR_s);
    filter_check(ccb, scb, inputdata, adjFilterInfo);
    filter_check_cmem(ccb, scb, inputdata, adjFilterInfo);

    CVTASK_PRINTF(LVL_CRITICAL, "[AMBA_IK_TONE_CURVE_s] ", 0, 0, 0, 0, 0);
    adjFilterInfo.PreFilterAdd = (uint32_t)&inputdata->pPipe0->GammaTable;
    adjFilterInfo.FilterAdd = (uint32_t)&inputdata->pPipe1->GammaTable;
    adjFilterInfo.NextFilterAdd = (uint32_t)&inputdata->pPipe2->GammaTable;
    adjFilterInfo.Size = sizeof(AMBA_IK_TONE_CURVE_s);
    filter_check(ccb, scb, inputdata, adjFilterInfo);
    filter_check_cmem(ccb, scb, inputdata, adjFilterInfo);

    CVTASK_PRINTF(LVL_CRITICAL, "[AMBA_IK_COLOR_CORRECTION_s] ", 0, 0, 0, 0, 0);
    adjFilterInfo.PreFilterAdd = (uint32_t)&inputdata->pPipe0->ColorCorr;
    adjFilterInfo.FilterAdd = (uint32_t)&inputdata->pPipe1->ColorCorr;
    adjFilterInfo.NextFilterAdd = (uint32_t)&inputdata->pPipe2->ColorCorr;
    adjFilterInfo.Size = sizeof(AMBA_IK_COLOR_CORRECTION_s);
    filter_check(ccb, scb, inputdata, adjFilterInfo);
    filter_check_cmem(ccb, scb, inputdata, adjFilterInfo);

    CVTASK_PRINTF(LVL_CRITICAL, "[AMBA_IK_CE_s] ", 0, 0, 0, 0, 0);
    adjFilterInfo.PreFilterAdd = (uint32_t)&inputdata->pPipe0->CEInfo;
    adjFilterInfo.FilterAdd = (uint32_t)&inputdata->pPipe1->CEInfo;
    adjFilterInfo.NextFilterAdd = (uint32_t)&inputdata->pPipe2->CEInfo;
    adjFilterInfo.Size = sizeof(AMBA_IK_CE_s);
    filter_check(ccb, scb, inputdata, adjFilterInfo);
    filter_check_cmem(ccb, scb, inputdata, adjFilterInfo);

    end_time =  get_cur_time();
    CVTASK_PRINTF(LVL_CRITICAL, "[all filter_check Time] :%10u->%10u  total:%10u\n",
                  start_time, end_time, (end_time-start_time), 0, 0);
    return rc;
}

errcode_enum_t dbp_check(awb_sim_ccb_t *ccb, awb_sim_scb_t *scb, AMBA_AAA_Flexidag_INFO_t *inputdata)
{
    errcode_enum_t rc = ERRCODE_NONE;
    int i, j, ping_pong = 1;
    int k;
    int error_code = 0;
    uint32_t  start_time, end_time;

    int per_param_size = 4;  //uint32_t,int32_t ...........4 bytes
    int num_of_params_per_cache = 128;
    int num_of_params_per_cache_def = 128;
    int dma_xfer_size = per_param_size * num_of_params_per_cache;//     4*128   = 512
    int num_of_itr;
    int num_of_nowait;
    int num_of_remainder;

    uint32_t da_0,da_1,da_2;
    uint32_t *word_ptr0[2], *wptr;

    start_time = get_cur_time();

    da_0 =  (uint32_t)&inputdata->pPipe0->BadpixCorr;
    read_memory(ccb, scb, da_0, sizeof(AMBA_IK_DYNAMIC_BAD_PXL_COR_s),0,0);

    da_1 =  (uint32_t)&inputdata->pPipe1->BadpixCorr;
    read_memory(ccb, scb, da_1, sizeof(AMBA_IK_DYNAMIC_BAD_PXL_COR_s),1,0);

    da_2 =  (uint32_t)&inputdata->pPipe2->BadpixCorr;
    num_of_itr = sizeof(AMBA_IK_DYNAMIC_BAD_PXL_COR_s) / dma_xfer_size;  //    siezof(idsp filter)/512
    num_of_remainder = (sizeof(AMBA_IK_DYNAMIC_BAD_PXL_COR_s) % (num_of_itr*dma_xfer_size));

    word_ptr0[0] = (uint32_t*)ccb->cache[0];
    word_ptr0[1] = (uint32_t*)ccb->cache[1];

    if(num_of_remainder > 0) {
        num_of_params_per_cache = num_of_remainder >> 2;
        visdma_dram_2_cmem(ccb->cache[0], da_2, (num_of_remainder >> 2));
        da_2 += num_of_remainder;
        num_of_nowait = num_of_itr;
        num_of_itr += 1;
    } else {
        visdma_dram_2_cmem(ccb->cache[0], da_2, (dma_xfer_size >> 2));
        da_2 += dma_xfer_size;
        num_of_nowait = num_of_itr - 1;
    }

    k = 0;
    for (i=0; i<num_of_itr; i++) { // loop through cache lines

        if(num_of_nowait > 0) {
            visdma_dram_2_cmem_nowait(ccb->cache[ping_pong], da_2, ((dma_xfer_size) >> 2));
            da_2 += dma_xfer_size;
            num_of_nowait --;
        }
        ping_pong = !ping_pong; // point to the other ping-pong buffer, which should be ready now

        for (j=0; j<num_of_params_per_cache; j++) {
            wptr = word_ptr0[ping_pong] + j;
            if(((scb->temp[1][k]>=scb->temp[0][k]) && (*wptr>=scb->temp[1][k])) || ((scb->temp[1][k]<=scb->temp[0][k]) && (*wptr<=scb->temp[1][k]))) {
                error_code = 0;
            } else {
                error_code = 1;
            }

            // CVTASK_PRINTF(LVL_CRITICAL, "dbp_check [%d] : %d, %d, %d \n",
            // k, scb->temp[0][k], scb->temp[1][k], *wptr, 0);
            k += 1;
        }
        num_of_params_per_cache = num_of_params_per_cache_def;
        wait_vis_w_dram_dma(); // wait for last nowait DMA to finish
    }

    end_time =  get_cur_time();
    CVTASK_PRINTF(LVL_CRITICAL, "[dbp_check Time] :%10u->%10u  total:%10u\n",
                  start_time, end_time, (end_time-start_time), 0, 0);
    return rc;
}

errcode_enum_t tonecurve_check(awb_sim_ccb_t *ccb, awb_sim_scb_t *scb, AMBA_AAA_Flexidag_INFO_t *inputdata)
{
    errcode_enum_t rc = ERRCODE_NONE;
    int i, j, ping_pong = 1;
    int k;
    int error_code = 0;
    uint32_t  start_time, end_time;

    int per_param_size = 4;  //uint32_t,int32_t ...........4 bytes
    int num_of_params_per_cache = 128;
    int num_of_params_per_cache_def = 128;
    int dma_xfer_size = per_param_size * num_of_params_per_cache;//     4*128   = 512
    int num_of_itr;
    int num_of_nowait;
    int num_of_remainder;

    uint32_t da_0,da_1,da_2;
    uint32_t *word_ptr0[2], *wptr;

    start_time = get_cur_time();

    da_0 =  (uint32_t)&inputdata->pPipe0->GammaTable;
    read_memory(ccb, scb, da_0, sizeof(AMBA_IK_TONE_CURVE_s),0,0);

    da_1 =  (uint32_t)&inputdata->pPipe1->GammaTable;
    read_memory(ccb, scb, da_1, sizeof(AMBA_IK_TONE_CURVE_s),1,0);

    da_2 =  (uint32_t)&inputdata->pPipe2->GammaTable;
    num_of_itr = sizeof(AMBA_IK_TONE_CURVE_s) / dma_xfer_size;  //    siezof(idsp filter)/512
    num_of_remainder = (sizeof(AMBA_IK_TONE_CURVE_s) % (num_of_itr*dma_xfer_size));

    word_ptr0[0] = (uint32_t*)ccb->cache[0];
    word_ptr0[1] = (uint32_t*)ccb->cache[1];

    if(num_of_remainder > 0) {
        num_of_params_per_cache = num_of_remainder >> 2;
        visdma_dram_2_cmem(ccb->cache[0], da_2, (num_of_remainder >> 2));
        da_2 += num_of_remainder;
        num_of_nowait = num_of_itr;
        num_of_itr += 1;
    } else {
        visdma_dram_2_cmem(ccb->cache[0], da_2, (dma_xfer_size >> 2));
        da_2 += dma_xfer_size;
        num_of_nowait = num_of_itr - 1;
    }

    k = 0;
    for (i=0; i<num_of_itr; i++) { // loop through cache lines

        if(num_of_nowait > 0) {
            visdma_dram_2_cmem_nowait(ccb->cache[ping_pong], da_2, ((dma_xfer_size) >> 2));
            da_2 += dma_xfer_size;
            num_of_nowait --;
        }
        ping_pong = !ping_pong; // point to the other ping-pong buffer, which should be ready now

        for (j=0; j<num_of_params_per_cache; j++) {
            wptr = word_ptr0[ping_pong] + j;
            if(((scb->temp[1][k]>=scb->temp[0][k]) && (*wptr>=scb->temp[1][k])) || ((scb->temp[1][k]<=scb->temp[0][k]) && (*wptr<=scb->temp[1][k]))) {
                error_code = 0;
            } else {
                error_code = 1;
            }

            // CVTASK_PRINTF(LVL_CRITICAL, "tonecurve_check [%d] : %d, %d, %d \n",
            // k, scb->temp[0][k], scb->temp[1][k], *wptr, 0);
            k += 1;
        }
        num_of_params_per_cache = num_of_params_per_cache_def;
        wait_vis_w_dram_dma(); // wait for last nowait DMA to finish
    }

    end_time =  get_cur_time();
    CVTASK_PRINTF(LVL_CRITICAL, "[tonecurve_check Time] :%10u->%10u  total:%10u\n",
                  start_time, end_time, (end_time-start_time), 0, 0);
    return rc;
}

errcode_enum_t ce_check(awb_sim_ccb_t *ccb, awb_sim_scb_t *scb, AMBA_AAA_Flexidag_INFO_t *inputdata)
{
    errcode_enum_t rc = ERRCODE_NONE;
    int i, j, ping_pong = 1;
    int k;
    int error_code = 0;
    uint32_t  start_time, end_time;

    int per_param_size = 4;  //uint32_t,int32_t ...........4 bytes
    int num_of_params_per_cache = 128;
    int num_of_params_per_cache_def = 128;
    int dma_xfer_size = per_param_size * num_of_params_per_cache;//     4*128   = 512
    int num_of_itr;
    int num_of_nowait;
    int num_of_remainder;

    uint32_t da_0,da_1,da_2;
    uint32_t *word_ptr0[2], *wptr;

    start_time = get_cur_time();

    da_0 =  (uint32_t)&inputdata->pPipe0->CEInfo;
    read_memory(ccb, scb, da_0, sizeof(AMBA_IK_CE_s),0,0);

    da_1 =  (uint32_t)&inputdata->pPipe1->CEInfo;
    read_memory(ccb, scb, da_1, sizeof(AMBA_IK_CE_s),1,0);

    da_2 =  (uint32_t)&inputdata->pPipe2->CEInfo;
    num_of_itr = sizeof(AMBA_IK_CE_s) / dma_xfer_size;  //    siezof(idsp filter)/512
    num_of_remainder = (sizeof(AMBA_IK_CE_s) % (num_of_itr*dma_xfer_size));

    word_ptr0[0] = (uint32_t*)ccb->cache[0];
    word_ptr0[1] = (uint32_t*)ccb->cache[1];

    if(num_of_remainder > 0) {
        num_of_params_per_cache = num_of_remainder >> 2;
        visdma_dram_2_cmem(ccb->cache[0], da_2, (num_of_remainder >> 2));
        da_2 += num_of_remainder;
        num_of_nowait = num_of_itr;
        num_of_itr += 1;
    } else {
        visdma_dram_2_cmem(ccb->cache[0], da_2, (dma_xfer_size >> 2));
        da_2 += dma_xfer_size;
        num_of_nowait = num_of_itr - 1;
    }

    k = 0;
    for (i=0; i<num_of_itr; i++) { // loop through cache lines

        if(num_of_nowait > 0) {
            visdma_dram_2_cmem_nowait(ccb->cache[ping_pong], da_2, ((dma_xfer_size) >> 2));
            da_2 += dma_xfer_size;
            num_of_nowait --;
        }
        ping_pong = !ping_pong; // point to the other ping-pong buffer, which should be ready now

        for (j=0; j<num_of_params_per_cache; j++) {
            wptr = word_ptr0[ping_pong] + j;
            if(((scb->temp[1][k]>=scb->temp[0][k]) && (*wptr>=scb->temp[1][k])) || ((scb->temp[1][k]<=scb->temp[0][k]) && (*wptr<=scb->temp[1][k]))) {
                error_code = 0;
            } else {
                error_code = 1;
            }

            // CVTASK_PRINTF(LVL_CRITICAL, "ce_check [%d] : %d, %d, %d \n",
            // k, scb->temp[0][k], scb->temp[1][k], *wptr, 0);
            k += 1;
        }
        num_of_params_per_cache = num_of_params_per_cache_def;
        wait_vis_w_dram_dma(); // wait for last nowait DMA to finish
    }

    end_time =  get_cur_time();
    CVTASK_PRINTF(LVL_CRITICAL, "[ce_check Time] :%10u->%10u  total:%10u\n",
                  start_time, end_time, (end_time-start_time), 0, 0);
    return rc;
}

errcode_enum_t cc_check(awb_sim_ccb_t *ccb, awb_sim_scb_t *scb, AMBA_AAA_Flexidag_INFO_t *inputdata)
{
    errcode_enum_t rc = ERRCODE_NONE;
    int i, j, ping_pong = 1;
    int k;
    int error_code = 0;
    uint32_t  start_time, end_time;

    int per_param_size = 4;  //uint32_t,int32_t ...........4 bytes
    int num_of_params_per_cache = 128;
    int num_of_params_per_cache_def = 128;
    int dma_xfer_size = per_param_size * num_of_params_per_cache;//     4*128   = 512
    int num_of_itr;
    int num_of_nowait;
    int num_of_remainder;

    uint32_t da_0,da_1,da_2;
    uint32_t *word_ptr0[2], *wptr;

    start_time = get_cur_time();

    da_0 =  (uint32_t)&inputdata->pPipe0->ColorCorr;
    read_memory(ccb, scb, da_0, sizeof(AMBA_IK_COLOR_CORRECTION_s),0,0);

    da_1 =  (uint32_t)&inputdata->pPipe1->ColorCorr;
    read_memory(ccb, scb, da_1, sizeof(AMBA_IK_COLOR_CORRECTION_s),1,0);


    da_2 =  (uint32_t)&inputdata->pPipe2->ColorCorr;
    num_of_itr = sizeof(AMBA_IK_COLOR_CORRECTION_s) / dma_xfer_size;  //    siezof(idsp filter)/512
    num_of_remainder = (sizeof(AMBA_IK_COLOR_CORRECTION_s) % (num_of_itr*dma_xfer_size));

    word_ptr0[0] = (uint32_t*)ccb->cache[0];
    word_ptr0[1] = (uint32_t*)ccb->cache[1];

    if(num_of_remainder > 0) {
        num_of_params_per_cache = num_of_remainder >> 2;
        visdma_dram_2_cmem(ccb->cache[0], da_2, (num_of_remainder >> 2));
        da_2 += num_of_remainder;
        num_of_nowait = num_of_itr;
        num_of_itr += 1;
    } else {
        visdma_dram_2_cmem(ccb->cache[0], da_2, (dma_xfer_size >> 2));
        da_2 += dma_xfer_size;
        num_of_nowait = num_of_itr - 1;
    }

    k = 0;
    for (i=0; i<num_of_itr; i++) { // loop through cache lines

        if(num_of_nowait > 0) {
            visdma_dram_2_cmem_nowait(ccb->cache[ping_pong], da_2, ((dma_xfer_size) >> 2));
            da_2 += dma_xfer_size;
            num_of_nowait --;
        }
        ping_pong = !ping_pong; // point to the other ping-pong buffer, which should be ready now

        for (j=0; j<num_of_params_per_cache; j++) {
            wptr = word_ptr0[ping_pong] + j;
            if(((scb->temp[1][k]>=scb->temp[0][k]) && (*wptr>=scb->temp[1][k])) || ((scb->temp[1][k]<=scb->temp[0][k]) && (*wptr<=scb->temp[1][k]))) {
                error_code = 0;
            } else {
                error_code = 1;
            }

            // CVTASK_PRINTF(LVL_CRITICAL, "cc_check [%d] : %d, %d, %d \n",
            // k, scb->temp[0][k], scb->temp[1][k], *wptr, 0);
            k += 1;
        }
        num_of_params_per_cache = num_of_params_per_cache_def;
        wait_vis_w_dram_dma(); // wait for last nowait DMA to finish
    }

    end_time =  get_cur_time();
    CVTASK_PRINTF(LVL_CRITICAL, "[cc_check Time] :%10u->%10u  total:%10u\n",
                  start_time, end_time, (end_time-start_time), 0, 0);
    return rc;
}
