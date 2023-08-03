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
                rc = ERRCODE_NONE;
            } else {
                CVTASK_PRINTF(LVL_CRITICAL, "filter_check_cmem [%d] : %d, %d, %d \n", 
                k, *(wptr_pre+j), *(wptr_cal+j), *wptr, 0);          
                rc = 1;
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
//   if(inputdata->pPipe1->BadpixCorrUpdate == 1U){
    CVTASK_PRINTF(LVL_CRITICAL, "[AMBA_IK_DYNAMIC_BAD_PXL_COR_s] ", 0, 0, 0, 0, 0);
    adjFilterInfo.PreFilterAdd = (uint32_t)&inputdata->pPipe0->BadpixCorr;
    adjFilterInfo.FilterAdd = (uint32_t)&inputdata->pPipe1->BadpixCorr;
    adjFilterInfo.NextFilterAdd = (uint32_t)&inputdata->pPipe2->BadpixCorr;
    adjFilterInfo.Size = sizeof(AMBA_IK_DYNAMIC_BAD_PXL_COR_s);
    // filter_check(ccb, scb, inputdata, adjFilterInfo);
    rc = filter_check_cmem(ccb, scb, inputdata, adjFilterInfo);
  
//   }

// //GammaUpdate
//   if(inputdata->pPipe1->GammaUpdate == 1U){
    CVTASK_PRINTF(LVL_CRITICAL, "[AMBA_IK_TONE_CURVE_s] ", 0, 0, 0, 0, 0);
    adjFilterInfo.PreFilterAdd = (uint32_t)&inputdata->pPipe0->GammaTable;
    adjFilterInfo.FilterAdd = (uint32_t)&inputdata->pPipe1->GammaTable;
    adjFilterInfo.NextFilterAdd = (uint32_t)&inputdata->pPipe2->GammaTable;
    adjFilterInfo.Size = sizeof(AMBA_IK_TONE_CURVE_s);
    // filter_check(ccb, scb, inputdata, adjFilterInfo);
    filter_check_cmem(ccb, scb, inputdata, adjFilterInfo);
//   }
// //ColorCorrUpdate
  // if(inputdata->pPipe1->ColorCorrUpdate == 1U){
    CVTASK_PRINTF(LVL_CRITICAL, "[AMBA_IK_COLOR_CORRECTION_s] ", 0, 0, 0, 0, 0);
    adjFilterInfo.PreFilterAdd = (uint32_t)&inputdata->pPipe0->ColorCorr;
    adjFilterInfo.FilterAdd = (uint32_t)&inputdata->pPipe1->ColorCorr;
    adjFilterInfo.NextFilterAdd = (uint32_t)&inputdata->pPipe2->ColorCorr;
    adjFilterInfo.Size = sizeof(AMBA_IK_COLOR_CORRECTION_s);
    // filter_check(ccb, scb, inputdata, adjFilterInfo);
    filter_check_cmem(ccb, scb, inputdata, adjFilterInfo);
//   }
//CEInfoUpdate
  // if(inputdata->pPipe1->CEInfoUpdate == 1U){
    CVTASK_PRINTF(LVL_CRITICAL, "[AMBA_IK_CE_s] ", 0, 0, 0, 0, 0);
    adjFilterInfo.PreFilterAdd = (uint32_t)&inputdata->pPipe0->CEInfo;
    adjFilterInfo.FilterAdd = (uint32_t)&inputdata->pPipe1->CEInfo;
    adjFilterInfo.NextFilterAdd = (uint32_t)&inputdata->pPipe2->CEInfo;
    adjFilterInfo.Size = sizeof(AMBA_IK_CE_s);
    filter_check(ccb, scb, inputdata, adjFilterInfo);
    filter_check_cmem(ccb, scb, inputdata, adjFilterInfo);
  // }




////////////////////////////////////////////////////////////////

  // if(inputdata->pPipe1->RgbYuvMatrixUpdate == 1U){
    CVTASK_PRINTF(LVL_CRITICAL, "[AMBA_IK_RGB_TO_YUV_MATRIX_s] ", 0, 0, 0, 0, 0);
    adjFilterInfo.PreFilterAdd = (uint32_t)&inputdata->pPipe0->RgbYuvMatrix;
    adjFilterInfo.FilterAdd = (uint32_t)&inputdata->pPipe1->RgbYuvMatrix;
    adjFilterInfo.NextFilterAdd = (uint32_t)&inputdata->pPipe2->RgbYuvMatrix;
    adjFilterInfo.Size = sizeof(AMBA_IK_RGB_TO_YUV_MATRIX_s);
    rc = filter_check_cmem(ccb, scb, inputdata, adjFilterInfo);
  // }

  // if(inputdata->pPipe1->BlackCorrUpdate == 1U){
    CVTASK_PRINTF(LVL_CRITICAL, "[AMBA_IK_STATIC_BLC_LVL_s] ", 0, 0, 0, 0, 0);
    adjFilterInfo.PreFilterAdd = (uint32_t)&inputdata->pPipe0->BlackCorr;
    adjFilterInfo.FilterAdd = (uint32_t)&inputdata->pPipe1->BlackCorr;
    adjFilterInfo.NextFilterAdd = (uint32_t)&inputdata->pPipe2->BlackCorr;
    adjFilterInfo.Size = sizeof(AMBA_IK_STATIC_BLC_LVL_s);
    rc = filter_check_cmem(ccb, scb, inputdata, adjFilterInfo);
  // }

  // if(inputdata->pPipe1->AntiAliasingUpdate == 1U){
    CVTASK_PRINTF(LVL_CRITICAL, "[AMBA_IK_ANTI_ALIASING_s] ", 0, 0, 0, 0, 0);
    adjFilterInfo.PreFilterAdd = (uint32_t)&inputdata->pPipe0->AntiAliasing;
    adjFilterInfo.FilterAdd = (uint32_t)&inputdata->pPipe1->AntiAliasing;
    adjFilterInfo.NextFilterAdd = (uint32_t)&inputdata->pPipe2->AntiAliasing;
    adjFilterInfo.Size = sizeof(AMBA_IK_ANTI_ALIASING_s);
    rc = filter_check_cmem(ccb, scb, inputdata, adjFilterInfo);
  // }

  // if(inputdata->pPipe1->CfaFilterUpdate == 1U){
    CVTASK_PRINTF(LVL_CRITICAL, "[AMBA_IK_CFA_NOISE_FILTER_s] ", 0, 0, 0, 0, 0);
    adjFilterInfo.PreFilterAdd = (uint32_t)&inputdata->pPipe0->CfaFilter;
    adjFilterInfo.FilterAdd = (uint32_t)&inputdata->pPipe1->CfaFilter;
    adjFilterInfo.NextFilterAdd = (uint32_t)&inputdata->pPipe2->CfaFilter;
    adjFilterInfo.Size = sizeof(AMBA_IK_CFA_NOISE_FILTER_s);
    rc = filter_check_cmem(ccb, scb, inputdata, adjFilterInfo);
  // }

  if(inputdata->pPipe1->RgbIrUpdate == 1U){
    CVTASK_PRINTF(LVL_CRITICAL, "[AMBA_IK_RGB_IR_s] ", 0, 0, 0, 0, 0);
    adjFilterInfo.PreFilterAdd = (uint32_t)&inputdata->pPipe0->RgbIr;
    adjFilterInfo.FilterAdd = (uint32_t)&inputdata->pPipe1->RgbIr;
    adjFilterInfo.NextFilterAdd = (uint32_t)&inputdata->pPipe2->RgbIr;
    adjFilterInfo.Size = sizeof(AMBA_IK_RGB_IR_s);
    rc = filter_check_cmem(ccb, scb, inputdata, adjFilterInfo);
  }

  // if(inputdata->pPipe1->DemosaicUpdate == 1U){
    CVTASK_PRINTF(LVL_CRITICAL, "[AMBA_IK_DEMOSAIC_s] ", 0, 0, 0, 0, 0);
    adjFilterInfo.PreFilterAdd = (uint32_t)&inputdata->pPipe0->Demosaic;
    adjFilterInfo.FilterAdd = (uint32_t)&inputdata->pPipe1->Demosaic;
    adjFilterInfo.NextFilterAdd = (uint32_t)&inputdata->pPipe2->Demosaic;
    adjFilterInfo.Size = sizeof(AMBA_IK_DEMOSAIC_s);
    rc = filter_check_cmem(ccb, scb, inputdata, adjFilterInfo);
  // }

  // if(inputdata->pPipe1->ChromaScaleUpdate == 1U){
    CVTASK_PRINTF(LVL_CRITICAL, "[AMBA_IK_CHROMA_SCALE_s] ", 0, 0, 0, 0, 0);
    adjFilterInfo.PreFilterAdd = (uint32_t)&inputdata->pPipe0->ChromaScale;
    adjFilterInfo.FilterAdd = (uint32_t)&inputdata->pPipe1->ChromaScale;
    adjFilterInfo.NextFilterAdd = (uint32_t)&inputdata->pPipe2->ChromaScale;
    adjFilterInfo.Size = sizeof(AMBA_IK_CHROMA_SCALE_s);
    rc = filter_check_cmem(ccb, scb, inputdata, adjFilterInfo);
  // }

  // if(inputdata->pPipe1->ChromaMedianUpdate == 1U){
    CVTASK_PRINTF(LVL_CRITICAL, "[AMBA_IK_CHROMA_MEDIAN_FILTER_s] ", 0, 0, 0, 0, 0);
    adjFilterInfo.PreFilterAdd = (uint32_t)&inputdata->pPipe0->ChromaMedian;
    adjFilterInfo.FilterAdd = (uint32_t)&inputdata->pPipe1->ChromaMedian;
    adjFilterInfo.NextFilterAdd = (uint32_t)&inputdata->pPipe2->ChromaMedian;
    adjFilterInfo.Size = sizeof(AMBA_IK_CHROMA_MEDIAN_FILTER_s);
    rc = filter_check_cmem(ccb, scb, inputdata, adjFilterInfo);
  // }

  // if(inputdata->pPipe1->LisoProcessSelectUpdate == 1U){
    CVTASK_PRINTF(LVL_CRITICAL, "[AMBA_IK_FIRST_LUMA_PROC_MODE_s] ", 0, 0, 0, 0, 0);
    adjFilterInfo.PreFilterAdd = (uint32_t)&inputdata->pPipe0->LisoProcessSelect;
    adjFilterInfo.FilterAdd = (uint32_t)&inputdata->pPipe1->LisoProcessSelect;
    adjFilterInfo.NextFilterAdd = (uint32_t)&inputdata->pPipe2->LisoProcessSelect;
    adjFilterInfo.Size = sizeof(AMBA_IK_FIRST_LUMA_PROC_MODE_s);
    rc = filter_check_cmem(ccb, scb, inputdata, adjFilterInfo);
  // }

  // if(inputdata->pPipe1->AsfUpdate == 1U){
    CVTASK_PRINTF(LVL_CRITICAL, "[AMBA_IK_ADV_SPAT_FLTR_s] ", 0, 0, 0, 0, 0);
    adjFilterInfo.PreFilterAdd = (uint32_t)&inputdata->pPipe0->Asf;
    adjFilterInfo.FilterAdd = (uint32_t)&inputdata->pPipe1->Asf;
    adjFilterInfo.NextFilterAdd = (uint32_t)&inputdata->pPipe2->Asf;
    adjFilterInfo.Size = sizeof(AMBA_IK_ADV_SPAT_FLTR_s);
    rc = filter_check_cmem(ccb, scb, inputdata, adjFilterInfo);
  // }

  // if(inputdata->pPipe1->SharpenBothUpdate == 1U){
    CVTASK_PRINTF(LVL_CRITICAL, "[AMBA_IK_FSTSHPNS_BOTH_s] ", 0, 0, 0, 0, 0);
    adjFilterInfo.PreFilterAdd = (uint32_t)&inputdata->pPipe0->SharpenBoth;
    adjFilterInfo.FilterAdd = (uint32_t)&inputdata->pPipe1->SharpenBoth;
    adjFilterInfo.NextFilterAdd = (uint32_t)&inputdata->pPipe2->SharpenBoth;
    adjFilterInfo.Size = sizeof(AMBA_IK_FSTSHPNS_BOTH_s);
    rc = filter_check_cmem(ccb, scb, inputdata, adjFilterInfo);
  // }

  // if(inputdata->pPipe1->SharpenNoiseUpdate == 1U){
    CVTASK_PRINTF(LVL_CRITICAL, "[AMBA_IK_FSTSHPNS_NOISE_s] ", 0, 0, 0, 0, 0);
    adjFilterInfo.PreFilterAdd = (uint32_t)&inputdata->pPipe0->SharpenNoise;
    adjFilterInfo.FilterAdd = (uint32_t)&inputdata->pPipe1->SharpenNoise;
    adjFilterInfo.NextFilterAdd = (uint32_t)&inputdata->pPipe2->SharpenNoise;
    adjFilterInfo.Size = sizeof(AMBA_IK_FSTSHPNS_NOISE_s);
    rc = filter_check_cmem(ccb, scb, inputdata, adjFilterInfo);
  // }

  // if(inputdata->pPipe1->SharpenFirUpdate == 1U){
    CVTASK_PRINTF(LVL_CRITICAL, "[AMBA_IK_FSTSHPNS_FIR_s] ", 0, 0, 0, 0, 0);
    adjFilterInfo.PreFilterAdd = (uint32_t)&inputdata->pPipe0->SharpenFir;
    adjFilterInfo.FilterAdd = (uint32_t)&inputdata->pPipe1->SharpenFir;
    adjFilterInfo.NextFilterAdd = (uint32_t)&inputdata->pPipe2->SharpenFir;
    adjFilterInfo.Size = sizeof(AMBA_IK_FSTSHPNS_FIR_s);
    rc = filter_check_cmem(ccb, scb, inputdata, adjFilterInfo);
  // }

  // if(inputdata->pPipe1->SharpenCoringUpdate == 1U){
    CVTASK_PRINTF(LVL_CRITICAL, "[AMBA_IK_CORING_s] ", 0, 0, 0, 0, 0);
    adjFilterInfo.PreFilterAdd = (uint32_t)&inputdata->pPipe0->SharpenCoring;
    adjFilterInfo.FilterAdd = (uint32_t)&inputdata->pPipe1->SharpenCoring;
    adjFilterInfo.NextFilterAdd = (uint32_t)&inputdata->pPipe2->SharpenCoring;
    adjFilterInfo.Size = sizeof(AMBA_IK_CORING_s);
    rc = filter_check_cmem(ccb, scb, inputdata, adjFilterInfo);
  // }
    
  // if(inputdata->pPipe1->SharpenCoringIndexScaleUpdate == 1U){
    CVTASK_PRINTF(LVL_CRITICAL, "[AMBA_IK_LEVEL_s] ", 0, 0, 0, 0, 0);
    adjFilterInfo.PreFilterAdd = (uint32_t)&inputdata->pPipe0->SharpenCoringIndexScale;
    adjFilterInfo.FilterAdd = (uint32_t)&inputdata->pPipe1->SharpenCoringIndexScale;
    adjFilterInfo.NextFilterAdd = (uint32_t)&inputdata->pPipe2->SharpenCoringIndexScale;
    adjFilterInfo.Size = sizeof(AMBA_IK_LEVEL_s);
    rc = filter_check_cmem(ccb, scb, inputdata, adjFilterInfo);
  // }

  // if(inputdata->pPipe1->SharpenMinCoringResultUpdate == 1U){
    CVTASK_PRINTF(LVL_CRITICAL, "[AMBA_IK_LEVEL_s] ", 0, 0, 0, 0, 0);
    adjFilterInfo.PreFilterAdd = (uint32_t)&inputdata->pPipe0->SharpenMinCoringResult;
    adjFilterInfo.FilterAdd = (uint32_t)&inputdata->pPipe1->SharpenMinCoringResult;
    adjFilterInfo.NextFilterAdd = (uint32_t)&inputdata->pPipe2->SharpenMinCoringResult;
    adjFilterInfo.Size = sizeof(AMBA_IK_LEVEL_s);
    rc = filter_check_cmem(ccb, scb, inputdata, adjFilterInfo);
  // }

  // if(inputdata->pPipe1->SharpenScaleCoringUpdate == 1U){
    CVTASK_PRINTF(LVL_CRITICAL, "[AMBA_IK_LEVEL_s] ", 0, 0, 0, 0, 0);
    adjFilterInfo.PreFilterAdd = (uint32_t)&inputdata->pPipe0->SharpenScaleCoring;
    adjFilterInfo.FilterAdd = (uint32_t)&inputdata->pPipe1->SharpenScaleCoring;
    adjFilterInfo.NextFilterAdd = (uint32_t)&inputdata->pPipe2->SharpenScaleCoring;
    adjFilterInfo.Size = sizeof(AMBA_IK_LEVEL_s);
    rc = filter_check_cmem(ccb, scb, inputdata, adjFilterInfo);
  // }                
                    
  // if(inputdata->pPipe1->SharpenMaxCoringResultUpdate == 1U){
    CVTASK_PRINTF(LVL_CRITICAL, "[AMBA_IK_LEVEL_s] ", 0, 0, 0, 0, 0);
    adjFilterInfo.PreFilterAdd = (uint32_t)&inputdata->pPipe0->SharpenMaxCoringResult;
    adjFilterInfo.FilterAdd = (uint32_t)&inputdata->pPipe1->SharpenMaxCoringResult;
    adjFilterInfo.NextFilterAdd = (uint32_t)&inputdata->pPipe2->SharpenMaxCoringResult;
    adjFilterInfo.Size = sizeof(AMBA_IK_LEVEL_s);
    rc = filter_check_cmem(ccb, scb, inputdata, adjFilterInfo);
  // }                        

  // if(inputdata->pPipe1->SharpenBBothUpdate == 1U){
    CVTASK_PRINTF(LVL_CRITICAL, "[AMBA_IK_FNLSHPNS_BOTH_s] ", 0, 0, 0, 0, 0);
    adjFilterInfo.PreFilterAdd = (uint32_t)&inputdata->pPipe0->SharpenBBoth;
    adjFilterInfo.FilterAdd = (uint32_t)&inputdata->pPipe1->SharpenBBoth;
    adjFilterInfo.NextFilterAdd = (uint32_t)&inputdata->pPipe2->SharpenBBoth;
    adjFilterInfo.Size = sizeof(AMBA_IK_FNLSHPNS_BOTH_s);
    rc = filter_check_cmem(ccb, scb, inputdata, adjFilterInfo);
  // }    

  // if(inputdata->pPipe1->SharpenBNoiseUpdate == 1U){
    CVTASK_PRINTF(LVL_CRITICAL, "[AMBA_IK_FNLSHPNS_NOISE_s] ", 0, 0, 0, 0, 0);
    adjFilterInfo.PreFilterAdd = (uint32_t)&inputdata->pPipe0->SharpenBNoise;
    adjFilterInfo.FilterAdd = (uint32_t)&inputdata->pPipe1->SharpenBNoise;
    adjFilterInfo.NextFilterAdd = (uint32_t)&inputdata->pPipe2->SharpenBNoise;
    adjFilterInfo.Size = sizeof(AMBA_IK_FNLSHPNS_NOISE_s);
    rc = filter_check_cmem(ccb, scb, inputdata, adjFilterInfo);
  // }    
       
  // if(inputdata->pPipe1->SharpenBFirUpdate == 1U){
    CVTASK_PRINTF(LVL_CRITICAL, "[AMBA_IK_FNLSHPNS_FIR_s] ", 0, 0, 0, 0, 0);
    adjFilterInfo.PreFilterAdd = (uint32_t)&inputdata->pPipe0->SharpenBFir;
    adjFilterInfo.FilterAdd = (uint32_t)&inputdata->pPipe1->SharpenBFir;
    adjFilterInfo.NextFilterAdd = (uint32_t)&inputdata->pPipe2->SharpenBFir;
    adjFilterInfo.Size = sizeof(AMBA_IK_FNLSHPNS_FIR_s);
    rc = filter_check_cmem(ccb, scb, inputdata, adjFilterInfo);
  // }             

  // if(inputdata->pPipe1->SharpenBCoringUpdate == 1U){
    CVTASK_PRINTF(LVL_CRITICAL, "[AMBA_IK_CORING_s] ", 0, 0, 0, 0, 0);
    adjFilterInfo.PreFilterAdd = (uint32_t)&inputdata->pPipe0->SharpenBCoring;
    adjFilterInfo.FilterAdd = (uint32_t)&inputdata->pPipe1->SharpenBCoring;
    adjFilterInfo.NextFilterAdd = (uint32_t)&inputdata->pPipe2->SharpenBCoring;
    adjFilterInfo.Size = sizeof(AMBA_IK_CORING_s);
    rc = filter_check_cmem(ccb, scb, inputdata, adjFilterInfo);
  // }       

  // if(inputdata->pPipe1->SharpenBCoringIndexScaleUpdate == 1U){
    CVTASK_PRINTF(LVL_CRITICAL, "[AMBA_IK_LEVEL_METHOD_s] ", 0, 0, 0, 0, 0);
    adjFilterInfo.PreFilterAdd = (uint32_t)&inputdata->pPipe0->SharpenBCoringIndexScale;
    adjFilterInfo.FilterAdd = (uint32_t)&inputdata->pPipe1->SharpenBCoringIndexScale;
    adjFilterInfo.NextFilterAdd = (uint32_t)&inputdata->pPipe2->SharpenBCoringIndexScale;
    adjFilterInfo.Size = sizeof(AMBA_IK_LEVEL_METHOD_s);
    rc = filter_check_cmem(ccb, scb, inputdata, adjFilterInfo);
  // }    

  // if(inputdata->pPipe1->SharpenBMinCoringResultUpdate == 1U){
    CVTASK_PRINTF(LVL_CRITICAL, "[AMBA_IK_LEVEL_METHOD_s] ", 0, 0, 0, 0, 0);
    adjFilterInfo.PreFilterAdd = (uint32_t)&inputdata->pPipe0->SharpenBMinCoringResult;
    adjFilterInfo.FilterAdd = (uint32_t)&inputdata->pPipe1->SharpenBMinCoringResult;
    adjFilterInfo.NextFilterAdd = (uint32_t)&inputdata->pPipe2->SharpenBMinCoringResult;
    adjFilterInfo.Size = sizeof(AMBA_IK_LEVEL_METHOD_s);
    rc = filter_check_cmem(ccb, scb, inputdata, adjFilterInfo);
  // }  

  // if(inputdata->pPipe1->SharpenBScaleCoringUpdate == 1U){
    CVTASK_PRINTF(LVL_CRITICAL, "[AMBA_IK_LEVEL_METHOD_s] ", 0, 0, 0, 0, 0);
    adjFilterInfo.PreFilterAdd = (uint32_t)&inputdata->pPipe0->SharpenBScaleCoring;
    adjFilterInfo.FilterAdd = (uint32_t)&inputdata->pPipe1->SharpenBScaleCoring;
    adjFilterInfo.NextFilterAdd = (uint32_t)&inputdata->pPipe2->SharpenBScaleCoring;
    adjFilterInfo.Size = sizeof(AMBA_IK_LEVEL_METHOD_s);
    rc = filter_check_cmem(ccb, scb, inputdata, adjFilterInfo);
  // }  

  // if(inputdata->pPipe1->SharpenBMaxCoringResultUpdate == 1U){
    CVTASK_PRINTF(LVL_CRITICAL, "[AMBA_IK_LEVEL_METHOD_s] ", 0, 0, 0, 0, 0);
    adjFilterInfo.PreFilterAdd = (uint32_t)&inputdata->pPipe0->SharpenBMaxCoringResult;
    adjFilterInfo.FilterAdd = (uint32_t)&inputdata->pPipe1->SharpenBMaxCoringResult;
    adjFilterInfo.NextFilterAdd = (uint32_t)&inputdata->pPipe2->SharpenBMaxCoringResult;
    adjFilterInfo.Size = sizeof(AMBA_IK_LEVEL_METHOD_s);
    rc = filter_check_cmem(ccb, scb, inputdata, adjFilterInfo);
  // }  

  // if(inputdata->pPipe1->ChromaFilterUpdate == 1U){
    CVTASK_PRINTF(LVL_CRITICAL, "[AMBA_IK_CHROMA_FILTER_s] ", 0, 0, 0, 0, 0);
    adjFilterInfo.PreFilterAdd = (uint32_t)&inputdata->pPipe0->ChromaFilter;
    adjFilterInfo.FilterAdd = (uint32_t)&inputdata->pPipe1->ChromaFilter;
    adjFilterInfo.NextFilterAdd = (uint32_t)&inputdata->pPipe2->ChromaFilter;
    adjFilterInfo.Size = sizeof(AMBA_IK_CHROMA_FILTER_s);
    rc = filter_check_cmem(ccb, scb, inputdata, adjFilterInfo);
  // }  

  // if(inputdata->pPipe1->WideChromaFilterUpdate == 1U){
    CVTASK_PRINTF(LVL_CRITICAL, "[AMBA_IK_WIDE_CHROMA_FILTER_s] ", 0, 0, 0, 0, 0);
    adjFilterInfo.PreFilterAdd = (uint32_t)&inputdata->pPipe0->WideChromaFilter;
    adjFilterInfo.FilterAdd = (uint32_t)&inputdata->pPipe1->WideChromaFilter;
    adjFilterInfo.NextFilterAdd = (uint32_t)&inputdata->pPipe2->WideChromaFilter;
    adjFilterInfo.Size = sizeof(AMBA_IK_WIDE_CHROMA_FILTER_s);
    rc = filter_check_cmem(ccb, scb, inputdata, adjFilterInfo);
  // }  

  // if(inputdata->pPipe1->WideChromaFilterCMBUpdate == 1U){
    CVTASK_PRINTF(LVL_CRITICAL, "[AMBA_IK_WIDE_CHROMA_FILTER_COMBINE_s] ", 0, 0, 0, 0, 0);
    adjFilterInfo.PreFilterAdd = (uint32_t)&inputdata->pPipe0->WideChromaFilterCMB;
    adjFilterInfo.FilterAdd = (uint32_t)&inputdata->pPipe1->WideChromaFilterCMB;
    adjFilterInfo.NextFilterAdd = (uint32_t)&inputdata->pPipe2->WideChromaFilterCMB;
    adjFilterInfo.Size = sizeof(AMBA_IK_WIDE_CHROMA_FILTER_COMBINE_s);
    rc = filter_check_cmem(ccb, scb, inputdata, adjFilterInfo);
  // }  

  // if(inputdata->pPipe1->GbGrMismatchUpdate == 1U){
    CVTASK_PRINTF(LVL_CRITICAL, "[AMBA_IK_GRGB_MISMATCH_s] ", 0, 0, 0, 0, 0);
    adjFilterInfo.PreFilterAdd = (uint32_t)&inputdata->pPipe0->GbGrMismatch;
    adjFilterInfo.FilterAdd = (uint32_t)&inputdata->pPipe1->GbGrMismatch;
    adjFilterInfo.NextFilterAdd = (uint32_t)&inputdata->pPipe2->GbGrMismatch;
    adjFilterInfo.Size = sizeof(AMBA_IK_GRGB_MISMATCH_s);
    rc = filter_check_cmem(ccb, scb, inputdata, adjFilterInfo);
  // }  

  // if(inputdata->pPipe1->ResamplerStrUpdate == 1U){
    CVTASK_PRINTF(LVL_CRITICAL, "[AMBA_IK_RESAMPLER_STR_s] ", 0, 0, 0, 0, 0);
    adjFilterInfo.PreFilterAdd = (uint32_t)&inputdata->pPipe0->ResamplerStr;
    adjFilterInfo.FilterAdd = (uint32_t)&inputdata->pPipe1->ResamplerStr;
    adjFilterInfo.NextFilterAdd = (uint32_t)&inputdata->pPipe2->ResamplerStr;
    adjFilterInfo.Size = sizeof(AMBA_IK_RESAMPLER_STR_s);
    rc = filter_check_cmem(ccb, scb, inputdata, adjFilterInfo);
  // }      

  // if(inputdata->pPipe1->MctfInfoUpdate == 1U){
    CVTASK_PRINTF(LVL_CRITICAL, "[AMBA_IK_VIDEO_MCTF_s] ", 0, 0, 0, 0, 0);
    adjFilterInfo.PreFilterAdd = (uint32_t)&inputdata->pPipe0->MctfInfo;
    adjFilterInfo.FilterAdd = (uint32_t)&inputdata->pPipe1->MctfInfo;
    adjFilterInfo.NextFilterAdd = (uint32_t)&inputdata->pPipe2->MctfInfo;
    adjFilterInfo.Size = sizeof(AMBA_IK_VIDEO_MCTF_s);
    rc = filter_check_cmem(ccb, scb, inputdata, adjFilterInfo);
  // } 
  // if(inputdata->pPipe1->MctfInfoUpdate == 1U){
    CVTASK_PRINTF(LVL_CRITICAL, "[AMBA_IK_VIDEO_MCTF_TA_s] ", 0, 0, 0, 0, 0);
    adjFilterInfo.PreFilterAdd = (uint32_t)&inputdata->pPipe0->MctfTemporalAdjust;
    adjFilterInfo.FilterAdd = (uint32_t)&inputdata->pPipe1->MctfTemporalAdjust;
    adjFilterInfo.NextFilterAdd = (uint32_t)&inputdata->pPipe2->MctfTemporalAdjust;
    adjFilterInfo.Size = sizeof(AMBA_IK_VIDEO_MCTF_TA_s);
    rc = filter_check_cmem(ccb, scb, inputdata, adjFilterInfo);
  // } 

  // if(inputdata->pPipe1->ColorDeptNRUpdate == 1U){
    CVTASK_PRINTF(LVL_CRITICAL, "[AMBA_IK_LUMA_NOISE_REDUCTION_s] ", 0, 0, 0, 0, 0);
    adjFilterInfo.PreFilterAdd = (uint32_t)&inputdata->pPipe0->ColorDeptNR;
    adjFilterInfo.FilterAdd = (uint32_t)&inputdata->pPipe1->ColorDeptNR;
    adjFilterInfo.NextFilterAdd = (uint32_t)&inputdata->pPipe2->ColorDeptNR;
    adjFilterInfo.Size = sizeof(AMBA_IK_LUMA_NOISE_REDUCTION_s);
    rc = filter_check_cmem(ccb, scb, inputdata, adjFilterInfo);
  // } 



////////////////////////////////////////////////////////////////



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
