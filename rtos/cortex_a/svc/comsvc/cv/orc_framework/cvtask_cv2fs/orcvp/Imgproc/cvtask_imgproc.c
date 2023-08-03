/**
 * File: ctask_vptask_awb.c
 *
 * VERSION  DATE      WHO   DETAIL
 * 0.1      06/13/21  ZZ    Initial example
 *
 * Main cvtask C file. Most of the logic is in cvtask_vptask_awb_run().
 */
//#include "cvtask_vptask_awb.h"
#include <ucode_debug.h>
#include <orc_memory.h>   // for visdma
#include <vp_common.h>

#include "cvapi_idsp_interface.h"
// #include "cvtask_fetch_chip_info.h"
//#include "cvapi_flexidag_fetch_chip_info.h"
#include "cvtask_imgproc_func.h"
#include "cvtask_adj.h"
#include "cvtask_imgproc.h"
//dbg_prt 1  segments' results
//dbg_prt 2  each color temperature R/B sum
//dbg_prt 3  processing time
//dbg_prt 4  processing time (detail information)
//dbg_prt 5  white table
//dbg_prt 99 all


#define WT_LENGTH                   121
#define WB_UNIT_GAIN                4096U
#define WB_Y_VALUE                  63U
#define MEMIO_ALIGN_SIZE            64U
#define MEMIO_ALIGN_SIZE_MASK       (~(MEMIO_ALIGN_SIZE - 1U))


/**
 * @ZZ: no need to change this function. Just make sure the task_name[]
 * matches with what's inside of the flexidag_sysflow.csv
 */
errcode_enum_t  imgproc_cvtask_register(cvtask_entry_t *pCVTaskEntry)
{
    char task_name[NAME_MAX_LENGTH] = "FTECH_CHIP_INFO";
  // char task_name[NAME_MAX_LENGTH] = "THREEA_ASIL";
  visorc_strcpy(&pCVTaskEntry->cvtask_name[0], &task_name[0], NAME_MAX_LENGTH);

  pCVTaskEntry->cvtask_type             = CVTASK_TYPE_ORCVP;
  pCVTaskEntry->cvtask_api_version      = CVTASK_API_VERSION;
  pCVTaskEntry->cvtask_query            = &cvtask_vptask_awb_query;
  pCVTaskEntry->cvtask_init             = &cvtask_vptask_awb_init;
  pCVTaskEntry->cvtask_get_info         = &cvtask_vptask_awb_get_info;
  pCVTaskEntry->cvtask_run              = &cvtask_vptask_awb_run;
  pCVTaskEntry->cvtask_process_messages = &cvtask_vptask_awb_process_messages;

#ifdef AMALGAM_DIAG
    orc_printf("cvtask_vptask_awb_register() : Registration called\n", 0, 0, 0, 0, 0);
#endif

    return ERRCODE_NONE;
} /* cvtask_vptask_awb_register() */

typedef struct cvtask_vptaska_params_s {
    uint32_t  instance_id;
} cvtask_vptaska_params_t;

// @ZZ: FIXME: need to hook up with real inputs; ask Peter?
static const cvtask_memory_interface_t cvtask_vptask_awb_interface =
{
  .CVTask_shared_storage_needed     = 0,
  .Instance_private_storage_needed  = sizeof(cvtask_vptaska_params_t),
    .num_inputs = 1,
    .input[0].io_name = "IDSP_PICINFO",
  //  .input[0].io_name = "IMGPROC_PICINFO",
  // .input[0].buffer_size = 128U,
  .input[0].buffer_size = sizeof(AMBA_AAA_Flexidag_INFO_t),
    .input[0].history_needed = 0,

    .num_outputs = 1,
    .output[0].io_name = "CHIP_INFO",
  // .output[0].io_name = "IMGPROC_INFO",
    .output[0].buffer_size = sizeof(AMBA_IMGPROC_FlEXIDAG_RESULT_t),
    .output[0].history_needed = 0,

}; /* cvtask_vptask_awb_interface */

/**
 * @ZZ: no need to change this function. Keep it dummy.
 */
errcode_enum_t  cvtask_vptask_awb_query(uint32_t instance, const uint32_t *pConfigData, cvtask_memory_interface_t *pCVTaskMemory)
{
    errcode_enum_t  retcode;

    retcode = ERRCODE_NONE;

    CVTASK_PRINTF(LVL_MINIMAL, "cvtask_vptask_awb_query() (DUMMY): Query called\n");

    if ((pConfigData == NULL) || (pCVTaskMemory == NULL)) {
        retcode = ERRCODE_BAD_PARAMETER;
    } /* if ((pConfigData == NULL) || (pCVTaskMemory == NULL)) */

    if (is_not_err(retcode)) {
        *pCVTaskMemory = cvtask_vptask_awb_interface; /* Copy data */

        /* Modified based on pConfigData */
    } /* if (is_not_err(retcode)) */

    return retcode;
} /* cvtask_vptask_awb_query() */

/**
 * @ZZ: no need to change this function. Keep it dummy.
 */
errcode_enum_t  cvtask_vptask_awb_init(const cvtask_parameter_interface_t *pCVTaskParams, const uint32_t *pConfigData)
{
    errcode_enum_t  retcode;

    retcode = ERRCODE_NONE;

    CVTASK_PRINTF(LVL_MINIMAL, "cvtask_vptask_awb_init() (DUMMY): Init called\n");

    if ((pCVTaskParams == NULL) || (pConfigData == NULL)) {
        retcode = ERRCODE_BAD_PARAMETER;
    } /* if ((pCVTaskParams == NULL) || (pConfigData == NULL)) */

    if (is_not_err(retcode)) {
        cvtask_vptaska_params_t *pLocalVar;

        pLocalVar = (cvtask_vptaska_params_t *)pCVTaskParams->vpInstance_private_storage;
        pLocalVar->instance_id  = pConfigData[0];
    } /* if (is_not_err(retcode)) */

    return retcode;
} /* cvtask_vptask_awb_init() */

/**
 * @ZZ: no need to change this function. Keep it dummy.
 */
errcode_enum_t  cvtask_vptask_awb_get_info(const cvtask_parameter_interface_t *pCVTaskParams, uint32_t info_index, void *vpInfoReturn)
{
    return ERRCODE_NONE;
} /* cvtask_vptask_awb_get_info() */

/**
 * @ZZ: no need to change this function. Keep it dummy.
 */
errcode_enum_t  cvtask_vptask_awb_process_messages(const cvtask_parameter_interface_t *pCVTaskParams)
{
    return ERRCODE_NONE;
} /* cvtask_vptask_awb_process_messages() */

static int32_t awb_1d_interpo(int32_t tp,const int32_t *input_p,const int32_t *value_p)
{
    int32_t     result = 0;
    int32_t     input[2];
    int32_t     value[2];

    if(input_p[0] <= input_p[1]) {
        input[0] = input_p[0];
        input[1] = input_p[1];
        value[0] = value_p[0];
        value[1] = value_p[1];
    } else {
        input[0] = input_p[1];
        input[1] = input_p[0];
        value[0] = value_p[1];
        value[1] = value_p[0];
    }

    if(tp <= input[0]) {
        result = value[0];
    } else if(tp >= input[1]) {
        result = value[1];
    } else {
        result = ((value[0] * (input[1] - tp)) + (value[1] * (tp - input[0]))) / (input[1] - input[0]);
    }

    return(result);
}

void awb_r_set2tareget_deltagain(awb_sim_ccb_t *ccb, awb_sim_scb_t *scb, uint32_t *pGain)
{

    int32_t tp;
    int32_t input[2];
    int32_t value[2];
    int32_t delta_tmp;

    tp = (int32_t)*pGain;
    input[0] = (int32_t)ccb->cal_set_gainR[0];
    input[1] = (int32_t)ccb->cal_set_gainR[1];
    value[0] = (int32_t)ccb->cal_deltaR[0];
    value[1] = (int32_t)ccb->cal_deltaR[1];
    delta_tmp = (int32_t)*pGain + awb_1d_interpo( tp, input, value);
    if(delta_tmp <= 0) {
        *pGain = 0;
    } else {
        *pGain = (uint32_t)delta_tmp;
    }

}

void awb_b_set2tareget_deltagain(awb_sim_ccb_t *ccb, awb_sim_scb_t *scb, uint32_t *pGain)
{

    int32_t tp;
    int32_t input[2];
    int32_t value[2];
    int32_t delta_tmp;

    tp = (int32_t)*pGain;
    input[0] = (int32_t)ccb->cal_set_gainB[0];
    input[1] = (int32_t)ccb->cal_set_gainB[1];
    value[0] = (int32_t)ccb->cal_deltaB[0];
    value[1] = (int32_t)ccb->cal_deltaB[1];
    delta_tmp = (int32_t)*pGain + awb_1d_interpo( tp, input, value);
    if(delta_tmp <= 0) {
        *pGain = 0;
    } else {
        *pGain = (uint32_t)delta_tmp;
    }

}

errcode_enum_t awb_wtab_init(awb_sim_ccb_t *ccb, awb_sim_scb_t *scb, AMBA_AAA_Flexidag_INFO_t *inputdata,uint8_t single_mode, uint8_t wt_index)
{
    uint32_t i;
    uint32_t j;
    uint32_t  starttime, endtime;
    errcode_enum_t rc = ERRCODE_NONE;
    starttime = get_cur_time();
    int ping_pong = 0;
//   int tab_index = 0;
    int x,y;
    int x_index,y_index;
    int y_length;
    int num_of_itr;// = WT_LENGTH*WT_LENGTH / CMEM_CACHE_LINE; // CMEM_CACHE_LINE:512
    uint32_t da;

    num_of_itr = (ccb->box[single_mode].xmax - ccb->box[single_mode].xmin +1)*( ccb->box[single_mode].ymax - ccb->box[single_mode].ymin +1);
    num_of_itr = (num_of_itr/CMEM_CACHE_LINE)+1;  //interge division may round down the value
    switch (single_mode) {
    case INDOOR:
        da = (uint32_t)inputdata->IndoorWRFULL.patch_no;
        break;
    case OUTDOOR:
        da = (uint32_t)inputdata->OutdoorWRFULL.patch_no;
        break;
    case HILIGHT:
        da = (uint32_t)inputdata->HighlightWRFULL.patch_no;
        break;
    default:
        da = (uint32_t)inputdata->IndoorWRFULL.patch_no;
        break;
    }

    visdma_dram_2_cmem(ccb->cache[0], da, ((CMEM_CACHE_LINE) >> 2));
    da += CMEM_CACHE_LINE;
    visdma_dram_2_cmem_nowait(ccb->cache[1], da, ((CMEM_CACHE_LINE) >> 2));

    x = 0;
    y = 0;
    y_length = ccb->box[single_mode].ymax - ccb->box[single_mode].ymin +1;
    for (i=0; i<num_of_itr; i++) { // loop through cache lines
        for (j=0; j<CMEM_CACHE_LINE; j++, y++) {
            if(y==y_length) { //change a column
                x +=1;
                y = 0;
            }

            x_index = x+ccb->box[single_mode].xmin;
            y_index = y+ccb->box[single_mode].ymin;
            if((x_index<121)&&(y_index<121)) {
                scb->White_Tbl[wt_index][x_index][y_index] = ccb->cache[ping_pong][j];
                //   if((ccb->dbg_prt==5U) ||(ccb->dbg_prt==99U) ){
                //     if( scb->White_Tbl[wt_index][x_index][y_index] != 0){
                //        CVTASK_PRINTF(LVL_CRITICAL, "xy scb->White_Tbl[%d][%d][%d] = %d,",wt_index, x_index, y_index,  scb->White_Tbl[wt_index][x_index][y_index], 0);
                //     }
                //   }
            }
        }
        wait_vis_w_dram_dma(); // wait for last nowait DMA to finish
        da += CMEM_CACHE_LINE; // kick off another DMA transfer
        visdma_dram_2_cmem_nowait(ccb->cache[ping_pong], da, ((CMEM_CACHE_LINE) >> 2));
        ping_pong = !ping_pong; // point to the other ping-pong buffer, which should be ready now
    }
    endtime = get_cur_time();
    if((ccb->dbg_prt==3U) ||(ccb->dbg_prt==99U) ) {
        CVTASK_PRINTF(LVL_CRITICAL, " [awb_wtab_init] = %10u ->%10u ,%10u",starttime, endtime,  (endtime-starttime), 0, 0);
    }
    return rc;

}

errcode_enum_t awb_sim_init(awb_sim_ccb_t *ccb, awb_sim_scb_t *scb, AMBA_AAA_Flexidag_INFO_t *inputdata)
{
    uint32_t i;
    // uint32_t  starttime, endtime;
    errcode_enum_t rc = ERRCODE_NONE;

    /// @ZZ: FIXME: use info from cvtask inputs to populate the
    /// followings parameters

    ccb->frame_index = inputdata->FrameIndex;
    ccb->num_of_tiles = inputdata->NumOfTiles;
    ccb->RGain_To_Cmp = inputdata->RGainToCmp;
    ccb->BGain_To_Cmp = inputdata->BGainToCmp;
    ccb->RGain_Default = inputdata->RGainDefault;
    ccb->BGain_Default = inputdata->BGainDefault;
    ccb->ROI_Weight_da = (uint32_t)inputdata->ROIWeight;
    ccb->CFA_AWB_da = (uint32_t)inputdata->SumRGB;
    ccb->CfaAWBRatio = inputdata->CfaAWBRatio;
    ccb->AwbRgbShift = inputdata->AwbRgbShift;
    ccb->AwbTileWidth = inputdata->AwbTileWidth;
    ccb->AwbTileHeight = inputdata->AwbTileHeight;
    ccb->ev_current = inputdata->EvIndex;
    ccb->ev_double = inputdata->DoubleInc;
    ccb->ev_outdoor = inputdata->OutdoorEvIndex;
    ccb->ev_hilight = inputdata->HighLightEvIndex;

    ccb->RGRatio[0] = inputdata->RGRatio[0];
    ccb->RGRatio[1] = inputdata->RGRatio[1];
    ccb->BGRatio[0] = inputdata->BGRatio[0];
    ccb->BGRatio[1] = inputdata->BGRatio[1];

    ccb->CurGainR = inputdata->CurGain.GainR;
    ccb->CurGainG = inputdata->CurGain.GainG;
    ccb->CurGainB = inputdata->CurGain.GainB;
    ccb->Speed = inputdata->Speed;

    ccb->cal_set_gainR[0] = inputdata->Cal_Set_GainR[0];
    ccb->cal_set_gainR[1] = inputdata->Cal_Set_GainR[1];
    ccb->cal_set_gainB[0] = inputdata->Cal_Set_GainB[0];
    ccb->cal_set_gainB[1] = inputdata->Cal_Set_GainB[1];
    ccb->cal_deltaR[0] = inputdata->Cal_DeltaR[0];
    ccb->cal_deltaR[1] = inputdata->Cal_DeltaR[1];
    ccb->cal_deltaB[0] = inputdata->Cal_DeltaB[0];
    ccb->cal_deltaB[1] = inputdata->Cal_DeltaB[1];

    ccb->dbg_prt = inputdata->DbgPrt;
    CVTASK_PRINTF(LVL_CRITICAL, "[ccb->dbg_prt] :%d,   inputdata->DbgPrt:%d\n",
                  ccb->dbg_prt,inputdata->DbgPrt, 0, 0, 0);
    for(i=0; i<64; i++) {
        ccb->luma_weight[i] = inputdata->LumaWeight[i];
    }
    for(i=0; i<20; i++) {
        ccb->table_weight[i]= inputdata->TableWeight[i];
    }


    ccb->box[INDOOR].xmin  = (uint8_t)(inputdata->IndoorWRFULL.min_gr/256);
    ccb->box[INDOOR].xmax  = (uint8_t)(inputdata->IndoorWRFULL.max_gr/256);
    ccb->box[INDOOR].ymin  = (uint8_t)(inputdata->IndoorWRFULL.min_gb/256);
    ccb->box[INDOOR].ymax  = (uint8_t)(inputdata->IndoorWRFULL.max_gb/256);
    ccb->box[OUTDOOR].xmin = (uint8_t)(inputdata->OutdoorWRFULL.min_gr/256);
    ccb->box[OUTDOOR].xmax = (uint8_t)(inputdata->OutdoorWRFULL.max_gr/256);
    ccb->box[OUTDOOR].ymin = (uint8_t)(inputdata->OutdoorWRFULL.min_gb/256);
    ccb->box[OUTDOOR].ymax = (uint8_t)(inputdata->OutdoorWRFULL.max_gb/256);
    ccb->box[HILIGHT].xmin = (uint8_t)(inputdata->HighlightWRFULL.min_gr/256);
    ccb->box[HILIGHT].xmax = (uint8_t)(inputdata->HighlightWRFULL.max_gr/256);
    ccb->box[HILIGHT].ymin = (uint8_t)(inputdata->HighlightWRFULL.min_gb/256);
    ccb->box[HILIGHT].ymax = (uint8_t)(inputdata->HighlightWRFULL.max_gb/256);


    // ccb->ev_current =  ccb->ev_hilight - (ccb->ev_double-20);
    if (ccb->ev_current > ccb->ev_hilight) {
        ccb->mode_w_blending = HILIGHT;
        awb_wtab_init(ccb, scb, inputdata,(uint8_t)HILIGHT, 0);
    } else if (ccb->ev_current > (ccb->ev_hilight-ccb->ev_double)) {
        ccb->mode_w_blending = BLENDING_OUTDOOR_HILIGHT;
        awb_wtab_init(ccb, scb, inputdata,(uint8_t)OUTDOOR, 0);
        awb_wtab_init(ccb, scb, inputdata,(uint8_t)HILIGHT, 1);
    } else if (ccb->ev_current > ccb->ev_outdoor) {
        ccb->mode_w_blending = OUTDOOR;
        awb_wtab_init(ccb, scb, inputdata,(uint8_t)OUTDOOR, 0);
    } else if (ccb->ev_current > (ccb->ev_outdoor-ccb->ev_double)) {
        ccb->mode_w_blending = BLENDING_INDOOR_OUTDOOR;
        awb_wtab_init(ccb, scb, inputdata,(uint8_t)INDOOR, 0);
        awb_wtab_init(ccb, scb, inputdata,(uint8_t)OUTDOOR, 1);
    } else {
        ccb->mode_w_blending = INDOOR;
        awb_wtab_init(ccb, scb, inputdata,(uint8_t)INDOOR, 0);
    }


    if((ccb->dbg_prt==1U) ||(ccb->dbg_prt==99U) ) {
        CVTASK_PRINTF(LVL_CRITICAL, "[awb_sim_init] :ccb->frame_index:%d, num_of_tiles:%d,  AwbRgbShift=%d, AwbTileWidth=%d, AwbTileHeight=%d,\n",
                      ccb->frame_index, ccb->num_of_tiles, ccb->AwbRgbShift, ccb->AwbTileWidth, ccb->AwbTileHeight);
        CVTASK_PRINTF(LVL_CRITICAL, "CMP Rgain:%d,Bgain:%d, Default Rgain:%d,Bgain:%d,,\n",
                      ccb->RGain_To_Cmp, ccb->BGain_To_Cmp, ccb->RGain_Default, ccb->BGain_Default, 0);

        CVTASK_PRINTF(LVL_CRITICAL, "ev_current=%d, ev_double=%d, ev_outdoor=%d, ev_hilight= %d\n",
                      ccb->ev_current, ccb->ev_double, ccb->ev_outdoor, ccb->ev_hilight, 0);
        CVTASK_PRINTF(LVL_CRITICAL, "LumaWeight[0-4] %d,%d,%d,%d,%d\n",
                      ccb->luma_weight[0], ccb->luma_weight[1], ccb->luma_weight[2], ccb->luma_weight[3], ccb->luma_weight[4]);
        CVTASK_PRINTF(LVL_CRITICAL, "TableWeight[0-4] %d,%d,%d,%d,%d\n",
                      ccb->table_weight[0], ccb->table_weight[1], ccb->table_weight[2], ccb->table_weight[3], ccb->table_weight[4]);

        CVTASK_PRINTF(LVL_CRITICAL, "[SumRGB]%d, %d, %d, %d, %d\n",
                      inputdata->SumRGB[95], inputdata->SumRGB[96], inputdata->SumRGB[97], inputdata->SumRGB[98], inputdata->SumRGB[99]);
        CVTASK_PRINTF(LVL_CRITICAL, "ROIWeight[%d]=%d\n",
                      4095, inputdata->ROIWeight[4095], 0, 0, 0);
        CVTASK_PRINTF(LVL_CRITICAL, "CfaAWBRatio(statistics gain ratio): %d\n",
                      ccb->CfaAWBRatio, 0, 0, 0, 0);

        CVTASK_PRINTF(LVL_CRITICAL, "RG[0],BG[0] ratio: %d, %d, RG[1],BG[1] ratio: %d, %d\n",
                      ccb->RGRatio[0], ccb->BGRatio[0], ccb->RGRatio[1], ccb->BGRatio[1], 0);

        CVTASK_PRINTF(LVL_CRITICAL, "CurGainR/G/B: %d, %d, %d\n",
                      ccb->CurGainR, ccb->CurGainG, ccb->CurGainB, 0, 0);

        CVTASK_PRINTF(LVL_CRITICAL, "Speed: %d\n",
                      ccb->Speed, 0, 0, 0, 0);

        CVTASK_PRINTF(LVL_CRITICAL, "calibraiton R target: %d,%d, delta: %d,%d\n",
                      ccb->cal_set_gainR[0], ccb->cal_set_gainR[1], ccb->cal_deltaR[0],ccb->cal_deltaR[1], 0);

        CVTASK_PRINTF(LVL_CRITICAL, "calibraiton B target: %d,%d, delta: %d,%d\n",
                      ccb->cal_set_gainB[0], ccb->cal_set_gainB[1], ccb->cal_deltaB[0],ccb->cal_deltaB[1], 0);

        CVTASK_PRINTF(LVL_CRITICAL, "[IndoorWRFULL]%d, %d, %d, %d,\n",
                      inputdata->IndoorWRFULL.min_gr, inputdata->IndoorWRFULL.max_gr, inputdata->IndoorWRFULL.min_gb, inputdata->IndoorWRFULL.max_gb, 0);
        CVTASK_PRINTF(LVL_CRITICAL, "[INDOOR]%d, %d, %d, %d,\n",
                      ccb->box[INDOOR].xmin, ccb->box[INDOOR].xmax,ccb->box[INDOOR].ymin, ccb->box[INDOOR].ymax, 0);

        CVTASK_PRINTF(LVL_CRITICAL, "[OutdoorWRFULL]%d, %d, %d, %d,\n",
                      inputdata->OutdoorWRFULL.min_gr, inputdata->OutdoorWRFULL.max_gr, inputdata->OutdoorWRFULL.min_gb, inputdata->OutdoorWRFULL.max_gb, 0);
        CVTASK_PRINTF(LVL_CRITICAL, "[OUTDOOR]%d, %d, %d, %d,\n",
                      ccb->box[OUTDOOR].xmin, ccb->box[OUTDOOR].xmax,ccb->box[OUTDOOR].ymin, ccb->box[OUTDOOR].ymax, 0);

        CVTASK_PRINTF(LVL_CRITICAL, "[HighlightWRFULL]%d, %d, %d, %d,\n",
                      inputdata->HighlightWRFULL.min_gr, inputdata->HighlightWRFULL.max_gr, inputdata->HighlightWRFULL.min_gb, inputdata->HighlightWRFULL.max_gb, 0);
        CVTASK_PRINTF(LVL_CRITICAL, "[HILIGHT]%d, %d, %d, %d,\n",
                      ccb->box[HILIGHT].xmin, ccb->box[HILIGHT].xmax,ccb->box[HILIGHT].ymin, ccb->box[HILIGHT].ymax, 0);

        CVTASK_PRINTF(LVL_CRITICAL, "[awb_sim_init  mode_w_blending   ENNNNNNNNNNNNND] :%d,\n",
                      ccb->mode_w_blending, 0, 0, 0,0);
    }

    return rc;
}

void zero_out_gain_sums(local_cd_t *lcd)
{
    int i;
    for (i=0; i<MAX_WHITE_REGION_X; i++) {
        lcd->RGainSum[i] = 0;
        lcd->BGainSum[i] = 0;
        lcd->WeightSum[i] = 0;
        lcd->WhiteNum[i] = 0;
    }
}

#define CMEM_CB ((awb_sim_ccb_t*)pCVTaskParams->vpCMEM_temporary_scratchpad)
#define AWB_SHIFT_BIT   8

errcode_enum_t awb_sim_part_ab(awb_sim_ccb_t *ccb, awb_sim_scb_t *scb)
{
    int i, j, tile_id = 0, ping_pong = 0;
    uint32_t R, G, B, Y;
    errcode_enum_t rc = ERRCODE_NONE;
    /**
     * @ZZ: Each tile has 12 bytes. We'll use 32 tiles per cache line
     * Works for 32x32 and 64x64 cases
     * FIXME: This code only works when ccb->num_of_tiles % 32 == 0
     */
//   int per_tile_size = 12;
    int per_tile_size = 6;  //R,G,B  sizeof(uint16_t)*3
    int num_of_tiles_per_cache = 32;
    int dma_xfer_size = per_tile_size * num_of_tiles_per_cache;//     12*32   = 384
    int num_of_itr = ccb->num_of_tiles / num_of_tiles_per_cache;//    4096/32 = 128
    uint32_t da = ccb->CFA_AWB_da;
    uint16_t *word_ptr[2], *wptr;
    uint32_t AwbTilePixelTmp = (1<<24)/((ccb->AwbTileWidth*ccb->AwbTileHeight)/4);
    int rshift = 24 - ccb->AwbRgbShift;
// static uint16_t ii = 0;
    /**
     *  @ZZ: CMEM ping-pong cache buffers store CFA AWB stats of the tile.
     *  The trick is to overlap a nowait DMA with CPU processing.
     *  Before processing the cache, call wait function to ensure data is ready.
     *  Then fire off another nowait DMA.
     */
    word_ptr[0] = (uint16_t*)ccb->cache[0];
    word_ptr[1] = (uint16_t*)ccb->cache[1];
    visdma_dram_2_cmem(ccb->cache[0], da, (dma_xfer_size >> 2));
    da += dma_xfer_size;
    visdma_dram_2_cmem_nowait(ccb->cache[1], da, (dma_xfer_size >> 2));

    // CVTASK_PRINTF(LVL_CRITICAL, "AwbTilePixelTmp : %d,rshift: %d,\n",
    // AwbTilePixelTmp, rshift, 0, 0, 0);

    for (i=0; i<num_of_itr; i++) { // loop through cache lines
        for (j=0; j<num_of_tiles_per_cache; j++, tile_id++) {
            wptr = word_ptr[ping_pong] + j*3;
            R = *wptr++;
            G = *wptr++;
            B = *wptr;

            /// AwbTilePixelTmp must have <= 16 bits
            R = (R * AwbTilePixelTmp*ccb->CfaAWBRatio) >> rshift;
            G = (G * AwbTilePixelTmp*ccb->CfaAWBRatio) >> rshift;
            B = (B * AwbTilePixelTmp*ccb->CfaAWBRatio) >> rshift;

            Y = G >> 8;    //shifting instead of divisoin
            if(Y>WB_Y_VALUE) {
                Y = WB_Y_VALUE;
            }
            scb->Awb_Y[tile_id] = (uint8_t)Y;

            // CVTASK_PRINTF(LVL_CRITICAL, "pAwb[%d] R/B/G    %d,    %d,    %d,  Y:  %d\n",
            // tile_id,R, B, G, Y);

            /// calculate per tile AwbTV_RGain[], AwbTV_BGain[]
            G <<= 12; // G = G * 4096
            scb->AwbTV_RGain[tile_id] = G / R;
            scb->AwbTV_BGain[tile_id] = G / B;

            // CVTASK_PRINTF(LVL_CRITICAL, "pAwb gain[%d] R/B/G    %d,    %d,    %d,  Y:  %d",
            // tile_id,scb->AwbTV_RGain[tile_id], scb->AwbTV_BGain[tile_id], 4096/*G*/, Y);
        }
        /**
         * @ZZ: end of cache line process
         */
        da += dma_xfer_size;
        wait_vis_w_dram_dma(); // wait for last nowait DMA to finish
        // kick off another DMA transfer
        visdma_dram_2_cmem_nowait(ccb->cache[ping_pong], da, ((dma_xfer_size) >> 2));
        ping_pong = !ping_pong; // point to the other ping-pong buffer, which should be ready now
    }

    return rc;
}

errcode_enum_t awb_sim_part_cd(awb_sim_ccb_t *ccb, awb_sim_scb_t *scb, int single_mode, uint8_t wt_index)
{
    int i, j, has_white_region = FALSE, ping_pong = 0;
    int tile_id = 0;
    int num_of_itr = ccb->num_of_tiles / CMEM_CACHE_LINE; // CMEM_CACHE_LINE:512
    uint32_t da = ccb->ROI_Weight_da;
    uint32_t x,y;
    int16_t  t1, t2;
    int X = 0;
    int Y = 0;
    local_cd_t lcd;
    uint32_t weight = 0;
    uint64_t All_Wg_Sum = 0;
    uint64_t RG_Sum = 0;
    uint64_t BG_Sum = 0;
    errcode_enum_t rc = ERRCODE_NONE;
    uint32_t start_time,end_time;
    uint32_t time0,time1;
    uint32_t total_white_num = 0; //only for debug

    /**
     * @ZZ: FIXME!!!
     *  This code only works when ccb->num_of_tiles % CMEM_CACHE_LINE == 0
     *  It works for 32x32 and 64x64 cases
     */
    // CVTASK_PRINTF(LVL_CRITICAL, "[awb_sim_part_cd] : start, single_mode:%d, wt_index:%d\n",
    // single_mode,wt_index, 0, 0, 0);
    zero_out_gain_sums(&lcd);

    /**
     *  @ZZ: CMEM ping-pong cache buffers store ROI weight of the tile.
     */
    start_time = get_cur_time();
    visdma_dram_2_cmem(ccb->cache[0], da, ((CMEM_CACHE_LINE) >> 2));
    da += CMEM_CACHE_LINE;
    visdma_dram_2_cmem_nowait(ccb->cache[1], da, ((CMEM_CACHE_LINE) >> 2));
    end_time = get_cur_time();
    if((ccb->dbg_prt==3U) ||(ccb->dbg_prt==99U) ) {
        CVTASK_PRINTF(LVL_CRITICAL, "[cd visdma_dram_2_cmem ExeTime] :%10u->%10u  total:%10u\n",
                      start_time, end_time, (end_time - start_time), 0, 0);
    }

    start_time = get_cur_time();
    for (i=0; i<num_of_itr; i++) { // loop through cache lines
        for (j=0; j<CMEM_CACHE_LINE; j++, tile_id++) {
            // CVTASK_PRINTF(LVL_CRITICAL, "[awb_sim_part_cd] : ccb->cache[%d][%d]:%d\n",
            // ping_pong,j, ccb->cache[ping_pong][j], 0, 0);

            // check to see if x falls between [xmin, xmax]
            // x = (scb->AwbTV_RGain[tile_id] >> AWB_SHIFT_BIT);
            x = scb->AwbTV_RGain[tile_id];
            // awb_r_set2tareget_deltagain(ccb, scb,&x);    //calibration data , from set to target
            x = (x >> AWB_SHIFT_BIT);
            t1 = x - ccb->box[single_mode].xmin;
            t2 = ccb->box[single_mode].xmax - x;
            // CVTASK_PRINTF(LVL_CRITICAL, "[cd]AwbTV_RGain[%d]:%d, x:%d : xmin:%d, xmax:%d\n",
            // tile_id,scb->AwbTV_RGain[tile_id], x, ccb->box[single_mode].xmin, ccb->box[single_mode].xmax);
            if ((t1 >= 0) && (t2 >= 0)) {
                // x = t1; // use offset to xmin as x
                // check to see if y falls between [ymin, ymax]
                // y = (scb->AwbTV_BGain[tile_id] >> AWB_SHIFT_BIT);
                y = scb->AwbTV_BGain[tile_id];
                // awb_b_set2tareget_deltagain(ccb, scb,&y);    //calibration data , from set to target
                y = (y >> AWB_SHIFT_BIT);
                t1 = y - ccb->box[single_mode].ymin;
                t2 = ccb->box[single_mode].ymax - y;

                if ((t1 >= 0) && (t2 >= 0)) {
                    // CVTASK_PRINTF(LVL_CRITICAL, "[awb_sim_part_cd] : ((t1 >= 0) && (t2 >= 0)):[%d],\n",
                    // tile_id, 0, 0, 0, 0);
                    // y = t1; // use offset to ymin as y
                    X = scb->White_Tbl[wt_index][x][y];// get X index from white table
                    Y = scb->Awb_Y[tile_id]; // @ZZ: FIXME: where do we get Y?

                    if (X != 0) { // got a tile in the white region
                        if(ccb->luma_weight[Y]>0) {
                            time0 = get_cur_time();
                            weight = ccb->cache[ping_pong][j] * ccb->luma_weight[Y];
                            lcd.RGainSum [X-1] += scb->AwbTV_RGain[tile_id] * weight;
                            lcd.BGainSum [X-1] += scb->AwbTV_BGain[tile_id] * weight;
                            lcd.WeightSum[X-1] += weight;
                            lcd.WhiteNum [X-1]++;
                            has_white_region = TRUE; // signal there's at least one white tile

                            time1 = get_cur_time();
                        }
                    }
                }
            }
// CVTASK_PRINTF(LVL_CRITICAL, "[awb_sim_part_cd] : has_white_region ,tile[%d],whitetable[%d],Y[%d] lumaweight:%d",
// tile_id, X,scb->Awb_Y[tile_id],ccb->luma_weight[Y], 0);

        }
        /**
         * @ZZ: end of cache line process
         */
        time0 = get_cur_time();
        wait_vis_w_dram_dma(); // wait for last nowait DMA to finish
        da += CMEM_CACHE_LINE; // kick off another DMA transfer
        visdma_dram_2_cmem_nowait(ccb->cache[ping_pong], da, ((CMEM_CACHE_LINE) >> 2));
        ping_pong = !ping_pong; // point to the other ping-pong buffer, which should be ready now
        time1 = get_cur_time();
        if((ccb->dbg_prt==4U) ||(ccb->dbg_prt==99U) ) {
            CVTASK_PRINTF(LVL_CRITICAL, "[cd ping_pong ExeTime] :%10u->%10u  total:%10u\n", time0, time1, (time1 - time0), 0, 0);
        }
    }

    end_time = get_cur_time();
    if((ccb->dbg_prt==3U) ||(ccb->dbg_prt==99U) ) {
        CVTASK_PRINTF(LVL_CRITICAL, "[cd main algo ExeTime] :%10u->%10u  total:%10u\n",
                      start_time, end_time, (end_time - start_time), 0, 0);
    }

    if((ccb->dbg_prt==2U) ||(ccb->dbg_prt==99U) ) {
        for(i=0; i<20; i++) {
            CVTASK_PRINTF(LVL_CRITICAL, "[awb_sim_part_cd][%d], RGainSum:    %llu,BGainSum:    %llu,WeightSum:    %d,WhiteNum:    %d,",
                          i, lcd.RGainSum [i], lcd.BGainSum [i], lcd.WeightSum[i],  lcd.WhiteNum [i]);
        }
    }
    start_time = get_cur_time();
    if (has_white_region) {
        for (X=0; X<MAX_WHITE_REGION_X; X++) {
            if (lcd.WeightSum[X] == 0) continue; // make sure divider is non-zero
            total_white_num += lcd.WhiteNum[X];
            lcd.RGainSum[X] /= lcd.WeightSum[X];
            lcd.BGainSum[X] /= lcd.WeightSum[X];
            lcd.WhiteNum[X] *= (lcd.WhiteNum[X] *ccb->table_weight[X]);
            All_Wg_Sum += lcd.WhiteNum[X];
            RG_Sum += lcd.RGainSum[X] * lcd.WhiteNum[X];
            BG_Sum += lcd.BGainSum[X] * lcd.WhiteNum[X];
        }
        scb->GainR[single_mode] = RG_Sum/All_Wg_Sum;
        scb->GainB[single_mode] = BG_Sum/All_Wg_Sum;
    } else {
        // no white region -- use default or last gains
        scb->GainR[single_mode] = ccb->RGain_Default;
        scb->GainB[single_mode] = ccb->BGain_Default;
    }

    if((ccb->dbg_prt==2U) ||(ccb->dbg_prt==99U) ) {
        CVTASK_PRINTF(LVL_CRITICAL, "[awb_sim_part_cd] scb->GainR: %d, scb->GainB: %d\n",
                      scb->GainR[single_mode], scb->GainB[single_mode], 0, 0, 0);
    }
    if((ccb->dbg_prt==3U) ||(ccb->dbg_prt==99U) ) {
        end_time = get_cur_time();
        CVTASK_PRINTF(LVL_CRITICAL, "[cd has_white_region(%d) ExeTime] :%10u->%10u  total:%10u\n", total_white_num,start_time, end_time, (end_time - start_time), 0);
        // CVTASK_PRINTF(LVL_CRITICAL, "\n[awb_sim_part_cd] single_mode[%d], scb->GainR:  %d,  scb->GainB: %d,",
        // single_mode,scb->GainR[single_mode], scb->GainB[single_mode], 0,  0);
    }

    return rc;
}

/**
 * @ZZ: FIXME: not implemented, result should be in
 * scb->GainR_Res, scb->GainB_Res
 */
errcode_enum_t awb_sim_blend(awb_sim_ccb_t *ccb, awb_sim_scb_t *scb, uint8_t state_pre, uint8_t state_next)
{
    errcode_enum_t  retcode = ERRCODE_NONE;
    uint32_t tmp = 0;
    if(state_pre == INDOOR) {
        tmp = ccb->ev_outdoor - ccb->ev_current;
    } else if(state_pre == OUTDOOR) {
        tmp = ccb->ev_hilight - ccb->ev_current;
    }

    scb->GainR_Res = (tmp*(uint32_t)scb->GainR[state_pre]+((uint32_t)ccb->ev_double-tmp)*(uint32_t)scb->GainR[state_pre])/ccb->ev_double-tmp;
    scb->GainB_Res = (tmp*(uint32_t)scb->GainB[state_pre]+((uint32_t)ccb->ev_double-tmp)*(uint32_t)scb->GainB[state_pre])/ccb->ev_double-tmp;

    return retcode;
}

errcode_enum_t adj_boundary_check(awb_sim_ccb_t *ccb, awb_sim_scb_t *scb, AMBA_AAA_Flexidag_INFO_t *inputdata)
{
  errcode_enum_t rc = ERRCODE_NONE;

  rc = all_filter_check(ccb, scb,inputdata);

  return rc;
}
errcode_enum_t ae_boundary_check(awb_sim_ccb_t *ccb, awb_sim_scb_t *scb, AMBA_AAA_Flexidag_INFO_t *inputdata)
{
    uint32_t          PrtUIntTmp[3];
    float             FTmp;
    static int16_t    Count = 0;
    double            CurAe = 0.0;
    double            PreAe = 0.0;
    double            AeStep = 0.0;
    double            LumaStatTmp = 0.0;
    double            TargetTmp = 0.0;
    int16_t           Result = 0;
    static int16_t    test = 0;
    errcode_enum_t rc = ERRCODE_NONE;

    test += 1;

    /// @ZZ: FIXME: use info from cvtask inputs to populate the
    /// followings parameters
    ccb->CurShutterTime = inputdata->CurShutterTime;
    ccb->CurAgcGain = inputdata->CurAgcGain;
    ccb->CurDgain = inputdata->CurDgain;
    ccb->CurLumaStat = inputdata->CurLumaStat;
    ccb->CurLimitStatus = inputdata->CurLimitStatus;
    ccb->CurTarget = inputdata->CurTarget;
    ccb->PreShutterTime = inputdata->PreShutterTime;
    ccb->PreAgcGain = inputdata->PreAgcGain;
    ccb->PreDgain = inputdata->PreDgain;


//   if((ccb->dbg_prt==1U) ||(ccb->dbg_prt==99U) ){
    PrtUIntTmp[0] = (uint32_t)ccb->CurAgcGain;
    FTmp = ccb->CurAgcGain - (float)PrtUIntTmp[0];
    FTmp = FTmp * 1000000.0f;
    PrtUIntTmp[1] = (uint32_t)FTmp;
    FTmp = ccb->CurShutterTime * 1000000.0f;
    PrtUIntTmp[2] = (uint32_t)FTmp;
    CVTASK_PRINTF(LVL_CRITICAL, "[ae_boundary_check] : CurAgcGain:%d.%6d,  CurShutterTime:0.%6d,CurDgain=%d,\n",
                  PrtUIntTmp[0], PrtUIntTmp[1], PrtUIntTmp[2], ccb->CurDgain, 0);
    PrtUIntTmp[0] = (uint32_t)ccb->PreAgcGain;
    FTmp = ccb->PreAgcGain - (float)PrtUIntTmp[0];
    FTmp = FTmp * 1000000.0f;
    PrtUIntTmp[1] = (uint32_t)FTmp;
    FTmp = ccb->PreShutterTime * 1000000.0f;
    PrtUIntTmp[2] = (uint32_t)FTmp;
    CVTASK_PRINTF(LVL_CRITICAL, "[ae_boundary_check] : CurAgcGain:%d.%6d,  CurShutterTime:0.%6d,CurDgain=%d,\n",
                  PrtUIntTmp[0], PrtUIntTmp[1], PrtUIntTmp[2], ccb->PreDgain, 0);
    CVTASK_PRINTF(LVL_CRITICAL, "[ae_boundary_check] : CurLumaStat:%d, CurTarget:%d,  CurLimitStatus:%d,\n",
                  ccb->CurLumaStat, ccb->CurTarget, ccb->CurLimitStatus, 0, 0);
//   }

    CurAe =  (double)ccb->CurShutterTime * (double)ccb->CurAgcGain* (double)ccb->CurDgain;
    PreAe =  (double)ccb->PreShutterTime * (double)ccb->PreAgcGain * (double)ccb->PreDgain;
    AeStep = CurAe / PreAe;

    LumaStatTmp = (double)ccb->CurLumaStat * AeStep;
    TargetTmp = (double)ccb->CurTarget;

    if(LumaStatTmp < TargetTmp / 4.0) {
        Count ++;
        Result = -10;
        CVTASK_PRINTF(LVL_CRITICAL, "---- Result = -10", 0, 0, 0, 0, 0);
        if(ccb->CurLimitStatus == -1) {
            Count = 0;
            Result = -1;
            CVTASK_PRINTF(LVL_CRITICAL, "---- Result = -1", 0, 0, 0, 0, 0);
        }
    } else if(LumaStatTmp > TargetTmp * 4.0) {
        Count ++;
        Result = 10;
        CVTASK_PRINTF(LVL_CRITICAL, "---- Result = 10", 0, 0, 0, 0, 0);
        if(ccb->CurLimitStatus == 1) {
            Count = 0;
            Result = 1;
            CVTASK_PRINTF(LVL_CRITICAL, "---- Result = 1", 0, 0, 0, 0, 0);
        }
    } else {
        Count = 0;
        Result = 0;
    }
    if(Count < 10) {
        Result = 0;
    }
    if(Result != 0) {
        CVTASK_PRINTF(LVL_CRITICAL, "--~~~~~-- Result = %d, test :%d", Result, test, 0, 0, 0);
    }
    ccb->AeResult = Result;
    CVTASK_PRINTF(LVL_CRITICAL, "$$$$$$$$$$$ Result = %d, test :%d", Result, test, 0, 0, 0);

    return rc;
}

errcode_enum_t awb_redundancy_func(awb_sim_ccb_t *ccb, awb_sim_scb_t *scb, AMBA_AAA_Flexidag_INFO_t *inputdata)
{
    uint32_t tmp = 0;
    uint32_t  start_time, end_time;
    uint32_t  time0, time1;
    errcode_enum_t rc = ERRCODE_NONE;
    start_time = get_cur_time(); // take the start audio clock tick

    // initializes AWB-similar control blocks
    time0 = get_cur_time();
    awb_sim_init(ccb, scb,inputdata);
    time1 = get_cur_time();
    CVTASK_PRINTF(LVL_CRITICAL, "[awb_sim_init ExeTime] :%10u->%10u  total:%10u, frame_index:%d\n",
                  time0, time1, (time1-time0), ccb->frame_index, 0);

    time0 = get_cur_time();
    awb_sim_part_ab(ccb, scb);
    time1 = get_cur_time();
    CVTASK_PRINTF(LVL_CRITICAL, "[awb_sim_part_ab ExeTime] :%10u->%10u  total:%10u\n",
                  time0, time1, (time1-time0), 0, 0);


    time0 = get_cur_time();
    switch (ccb->mode_w_blending) {
    case INDOOR:
        awb_sim_part_cd(ccb, scb, INDOOR,0);
        scb->GainR_Res = scb->GainR[INDOOR];
        scb->GainB_Res = scb->GainB[INDOOR];
        break;
    case BLENDING_INDOOR_OUTDOOR:
        awb_sim_part_cd(ccb, scb, INDOOR,0);
        awb_sim_part_cd(ccb, scb, OUTDOOR,1);
        awb_sim_blend(ccb, scb, INDOOR, OUTDOOR);
        break;
    case OUTDOOR:
        awb_sim_part_cd(ccb, scb, OUTDOOR,0);
        scb->GainR_Res = scb->GainR[OUTDOOR];
        scb->GainB_Res = scb->GainB[OUTDOOR];
        break;
    case BLENDING_OUTDOOR_HILIGHT:
        awb_sim_part_cd(ccb, scb, OUTDOOR,0);
        awb_sim_part_cd(ccb, scb, HILIGHT,1);
        awb_sim_blend(ccb, scb, OUTDOOR, HILIGHT);
        break;
    case HILIGHT:
        awb_sim_part_cd(ccb, scb, HILIGHT,0);
        scb->GainR_Res = scb->GainR[HILIGHT];
        scb->GainB_Res = scb->GainB[HILIGHT];
        break;
    default:
        scb->GainR_Res = ccb->RGain_Default;
        scb->GainB_Res = ccb->BGain_Default;
        break;
    }

    time1 = get_cur_time();
    CVTASK_PRINTF(LVL_CRITICAL, "[awb_sim_part_cd ExeTime] :%10u->%10u  total:%10u\n",
                  time0, time1, (time1-time0), 0, 0);

    scb->GainR_Res = (scb->GainR_Res * ccb->RGRatio[0]);
    scb->GainR_Res = scb->GainR_Res >>12;    //divided by WB_UNIT_GAIN;
    scb->GainB_Res = (scb->GainB_Res * ccb->BGRatio[0]);
    scb->GainB_Res = scb->GainB_Res >> 12;   //divided by WB_UNIT_GAIN;

    scb->GainR_Res = (scb->GainR_Res * ccb->RGRatio[1]);
    scb->GainR_Res = scb->GainR_Res >>12;    //divided by WB_UNIT_GAIN;
    scb->GainB_Res = (scb->GainB_Res * ccb->BGRatio[1]);
    scb->GainB_Res = scb->GainB_Res >> 12;   //divided by WB_UNIT_GAIN;

    if((ccb->dbg_prt==1U) || (ccb->dbg_prt==99U) || (ccb->dbg_prt==2U) ) {
        CVTASK_PRINTF(LVL_CRITICAL, "\n[awb_redundancy_func] adj ratio , scb->GainR:  %d,  scb->GainB: %d,",
                      scb->GainR_Res, scb->GainB_Res, 0, 0, 0);
    }
    scb->GainR_Res = (scb->GainR_Res * ccb->CurGainG) / WB_UNIT_GAIN;
    scb->GainB_Res = (scb->GainB_Res * ccb->CurGainG) / WB_UNIT_GAIN;
    if((ccb->dbg_prt==1U) ||(ccb->dbg_prt==99U) || (ccb->dbg_prt==2U) ) {
        CVTASK_PRINTF(LVL_CRITICAL, "\n[awb_redundancy_func] refine , scb->GainR:  %d,  scb->GainB: %d,",
                      scb->GainR_Res, scb->GainB_Res, 0, 0, 0);
    }
    tmp = ((scb->GainR_Res * (uint32_t)ccb->Speed) +
           (ccb->CurGainR * (64 - (uint32_t)ccb->Speed)));
    scb->GainR_Res = tmp >>6;    //divided by 64;
    tmp = ((scb->GainB_Res * (uint32_t)ccb->Speed) +
           (ccb->CurGainB * (64U - (uint32_t)ccb->Speed)));
    scb->GainB_Res = tmp >>6;    //divided by 64;
    if((ccb->dbg_prt==1U) ||(ccb->dbg_prt==99U) || (ccb->dbg_prt==2U) ) {
        CVTASK_PRINTF(LVL_CRITICAL, "\n[awb_redundancy_func] results , scb->GainR:  %d,  scb->GainB: %d,",
                      scb->GainR_Res, scb->GainB_Res, 0, 0, 0);
    }
    /*** final comparison result ***/
    // @ZZ: FIXME - hook the result up with cvtask_vptask_awb_interface::output[]
    // scb->Cmp_Result = (ccb->RGain_To_Cmp == scb->GainR_Res) && (ccb->BGain_To_Cmp == scb->GainB_Res);
    if((ccb->RGain_To_Cmp == scb->GainR_Res) && (ccb->BGain_To_Cmp == scb->GainB_Res)){
      scb->Cmp_Result = 0;
    }else{
      scb->Cmp_Result = 1;
    }
    end_time = get_cur_time(); // take the end audio clock tick
    CVTASK_PRINTF(LVL_CRITICAL, "[DEBUG] : Cmp_Result=%d Processing time %10u -> %10u (%10u)\n",
                  scb->Cmp_Result, start_time, end_time, end_time - start_time, 0);
    CVTASK_PRINTF(LVL_CRITICAL, "[DEBUG] : GainOriginal [R:%d, B:%d], Gain [R:%d, B:%d]\n",
                  ccb->RGain_To_Cmp, ccb->BGain_To_Cmp, scb->GainR_Res, scb->GainB_Res, 0);
    CVTASK_PRINTF(LVL_CRITICAL, "[DEBUG] : scb=%d ccb=%d\n",
                  sizeof(awb_sim_scb_t), sizeof(awb_sim_ccb_t), 0, 0, 0);
    return rc;
}

/**
 * @ZZ: the main run() function, all dynamic logic starts from here
 */
errcode_enum_t  cvtask_vptask_awb_run(const cvtask_parameter_interface_t *pCVTaskParams)
{
    cvtask_vptaska_params_t *pLocalVar;
    errcode_enum_t  retcode;

    AMBA_IMGPROC_FlEXIDAG_RESULT_t *output;
    AMBA_AAA_Flexidag_INFO_t *inputdata;

    pLocalVar = (cvtask_vptaska_params_t *)pCVTaskParams->vpInstance_private_storage;
    retcode = cvtask_vptask_awb_process_messages(pCVTaskParams);

    if (pCVTaskParams->CMEM_temporary_scratchpad_size >= 2560) { // should always be 4096 bytes
        awb_sim_ccb_t *ccb = CMEM_CB;   // control block in CMEM
        awb_sim_scb_t scb; // control block in stack
        uint32_t  start_time, end_time;

        output = pCVTaskParams->vpOutputBuffer[0];
        inputdata = (AMBA_AAA_Flexidag_INFO_t*)pCVTaskParams->vpInputBuffer[0];

        start_time = get_cur_time(); // take the start audio clock tick
        ae_boundary_check(ccb, &scb,inputdata);
        end_time = get_cur_time();
        CVTASK_PRINTF(LVL_CRITICAL, "[ae_boundary_check ExeTime] :%10u->%10u  total:%10u, frame_index:%d\n",
                      start_time, end_time, (end_time-start_time), ccb->frame_index, 0);

        output->AdjResult.Result = adj_boundary_check(ccb, &scb,inputdata);

        awb_redundancy_func(ccb, &scb,inputdata);

        // output->AwbResult.GainRO = ccb->RGain_To_Cmp;
        // output->AwbResult.GainGO = 4096;
        // output->AwbResult.GainBO = ccb->BGain_To_Cmp;
        // output->AwbResult.GainR = scb.GainR_Res;
        // output->AwbResult.GainG = 4096;
        // output->AwbResult.GainB = scb.GainB_Res;
        output->AwbResult.Result = scb.Cmp_Result;
        output->AeResult.Result = ccb->AeResult;

  } /* processing block*/

    return retcode;
} /* cvtask_vptask_awb_run() */
