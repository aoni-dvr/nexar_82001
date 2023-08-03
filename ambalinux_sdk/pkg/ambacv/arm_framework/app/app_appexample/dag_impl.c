#include <stdio.h>
#include <string.h>

#include "cvapi_ambacv_flexidag.h"

#include "mem_util.h"
#include "dag_util.h"
#include "dag_impl.h"
#include "postprocess_impl.h"
#include "preference.h"

// for raw mode setting
#define INPUT_DEPTH 2

typedef struct {
    /* FlexiDAG */
    flexidag_memblk_t temp_buf;
    FLEXIDAG_INSTANCE_s fd[MAX_FLEXIDAG_AMOUNT];
    uint32_t fd_amount;

    /* user preference */
    uint8_t dag_input_type;
    WINDOW_INFO_s InputInfo;
    amba_roi_config_t RoiCfg;
} PROGRAM_PARAMS_s;

static PROGRAM_PARAMS_s g_params[MAX_FLOW_AMOUNT] = {0};
static inline uint32_t ALIGN128(uint32_t X) {return ((X + 127U) & 0xFFFFFF80U);}

static void Process_input_picinfo(uint32_t id, memio_source_recv_picinfo_t *pInfo, flexidag_memblk_t *input)
{
    memio_source_recv_picinfo_t *dest;
    int i;

    /* Always copy frame data to dag's in buffer. */
    /* This is because the pic_info might not be in the area which Dag can access. */
    dest = (memio_source_recv_picinfo_t *)input->pBuffer;
    memcpy(dest, pInfo, sizeof(memio_source_recv_picinfo_t));

    //printf("pyrimid[%u] p:%u, w:%u, h:%u\n",
    //    OCTAVE_IDX,
    //    dest->pic_info.pyramid.half_octave[OCTAVE_IDX].ctrl.roi_pitch,
    //    dest->pic_info.pyramid.half_octave[OCTAVE_IDX].roi_width_m1+1,
    //    dest->pic_info.pyramid.half_octave[OCTAVE_IDX].roi_height_m1+1);

    /* scheduler expect the rpXXXX is the offset to base address of pic_ifo. */
    /* However, the data retrieved from RTOS is physical address. */
    /* Therefore, they have to be adjusted. */
    for (i = 0U ; i < MAX_HALF_OCTAVES ; i++) {
        if (dest->pic_info.rpLumaLeft[i] != 0) { //valid address
            dest->pic_info.rpLumaLeft[i] -= input->buffer_daddr;
            dest->pic_info.rpLumaRight[i] -= input->buffer_daddr;
            dest->pic_info.rpChromaLeft[i] -= input->buffer_daddr;
            dest->pic_info.rpChromaRight[i] -= input->buffer_daddr;
        }
    }

    input->buffer_size = ALIGN128(sizeof(memio_source_recv_picinfo_t));
    MemUtil_MemblkClean(input);
}

static void Process_input_raw(uint32_t id, memio_source_recv_picinfo_t *pInfo, flexidag_memblk_t *input)
{
    memio_source_recv_raw_t *raw;
    uint32_t start_addr;
    uint32_t start_x, start_y;
    uint32_t input_height, input_width, dag_roi_height, dag_roi_width;
    uint32_t pyramid_index;

    /* Input frame from RTOS pyramid is in cv_pic_info_t. */
    /* For the raw buffer mode, the input data should be filled into memio_source_recv_raw_t */
    input_height   = g_params[id].InputInfo.Height;
    input_width    = g_params[id].InputInfo.Width;
    dag_roi_height = g_params[id].RoiCfg.roi_height;
    dag_roi_width  = g_params[id].RoiCfg.roi_width;
    pyramid_index  = g_params[id].RoiCfg.image_pyramid_index;

    if (g_params[id].RoiCfg.roi_start_col == 9999) {
        start_x = (input_width - dag_roi_width) / 2; // center
    } else {
        start_x = g_params[id].RoiCfg.roi_start_col;
    }
    if (g_params[id].RoiCfg.roi_start_row == 9999) {
        start_y = (input_height - dag_roi_height) / 2; // center
    } else {
        start_y = g_params[id].RoiCfg.roi_start_row;
    }

    /* Y */
    raw = (memio_source_recv_raw_t*)input[0].pBuffer;
    raw->size = dag_roi_width * dag_roi_height;
    raw->pitch = pInfo->pic_info.pyramid.image_pitch_m1 + 1;
    start_addr = pInfo->pic_info.rpLumaLeft[pyramid_index];
    start_addr += (start_x + (start_y*raw->pitch));
    raw->addr = start_addr;
    input[0].buffer_size = ALIGN128(sizeof(memio_source_recv_raw_t));
    MemUtil_MemblkClean(&input[0]);
    /* UV */
    raw = (memio_source_recv_raw_t*)input[1].pBuffer;
    raw->size = dag_roi_width * dag_roi_height / 2;
    raw->pitch = pInfo->pic_info.pyramid.image_pitch_m1 + 1;
    start_addr = pInfo->pic_info.rpChromaLeft[pyramid_index];
    start_addr += (start_x + (start_y*raw->pitch/2));
    raw->addr = start_addr;
    input[1].buffer_size = ALIGN128(sizeof(memio_source_recv_raw_t));
    MemUtil_MemblkClean(&input[1]);

}

static uint32_t Algo_0_Cfg(AMBA_CV_FLEXIDAG_HANDLE_s *Handler)
{
// UUID is defined in flow table
#define MNET_SSD  (2U)
    AMBA_CV_FLEXIDAG_MSG_s Msg;
    uint16_t FlowID = 0U;
    uint32_t Id = 0;
    uint32_t Ret = 0U;

    (void) AmbaCV_FlexidagGetFlowIdByUUID(Handler, MNET_SSD, &FlowID);

    Msg.flow_id = FlowID;
    Msg.vpMessage = (void *)&g_params[Id].RoiCfg;
    Msg.length = sizeof(amba_roi_config_t);
    (void) AmbaCV_FlexidagSendMsg(Handler, &Msg);

    return Ret;
}

static uint32_t Algo_0_Init(uint32_t dag_input_type, char *fl_path)
{
    uint32_t ret = 0, id = 0, i;
    char path[128] = {0};
    FLEXIDAG_INSTANCE_s *inst;

    if (g_params[id].temp_buf.pBuffer != NULL) {
        printf("[Algo_0_Init] Already Inited!!\n");
        return 1;
    }

    g_params[id].fd_amount = 0;
    /* create instance for all flexidag which will be used in this Algo */
    /****
    /* for example, if your flow is NN1->ARM tack->NN2->ARM task,
    /* you have to create instance for NN1 and NN2 now
    ****/
    snprintf(path, sizeof(path), "%s/flexibin0.bin",fl_path);
    ret = DagUtil_CreateDag(path, &g_params[id].fd[0]);
    if (ret != 0) {
        printf("[Algo_0_Init] DagUtil_CreateDag create Fail.\n");
        return ret;
    }
    g_params[id].fd_amount++;

    /* alloc global temp buffer. */
    /****
    /* If there are multi dag existed and they will not run at the same time,
    /* it should be the maximal size over all.
    ****/
    {
        uint32_t max_val = 0, temp;

        for (i=0; i<g_params[id].fd_amount; i++) {
            inst = &g_params[id].fd[i];
            temp = inst->handle.mem_req.flexidag_temp_buffer_size;
            if (temp > max_val) {
                max_val = temp;
            }
        }
        if (max_val != 0) {
            ret = MemUtil_MemblkAlloc(CACHE_ALIGN(max_val), &g_params[id].temp_buf);
            if (ret != 0) {
                printf("[Algo_0_Init] MemUtil_MemblkAlloc temp_buf Fail.\n");
                return ret;
            }
        } else {
            printf("[Algo_0_Init] no need for temp buffer!\n");

        }
    }

    /* after all instance are created, now you have to init them one by one. */
    for (i=0; i<g_params[id].fd_amount; i++) {
        // save the user preference in g_params
        Preference_GetDAGRoiCfg(id, &g_params[id].RoiCfg);
        inst = &g_params[id].fd[i];
        ret = DagUtil_InitDag(inst, &g_params[id].temp_buf);
        if (ret != 0) {
            printf("[Algo_0_Init] DagUtil_InitDag %u Fail.\n", i);
        } else {
            /* After dag inited, you may want to do specific configures */
            ret = Algo_0_Cfg(&inst->handle);
            if (ret != 0) {
                printf("[Algo_0_Init] Algo_0_Cfg %u Fail.\n", i);
            }
            /* configure input buffer */
            //This is flow specific.
            //Maybe it doesn't need to create buffer for each flexidag.
            g_params[id].dag_input_type = dag_input_type;
            if (dag_input_type == DAG_INPUT_TYPE_PIC) {
                inst->in.num_of_buf = 1;
                if(inst->in.buf[0].pBuffer == NULL) {
                    ret = MemUtil_MemblkAlloc(CACHE_ALIGN(sizeof(memio_source_recv_picinfo_t)), &(inst->in.buf[0]));
                    if (ret != 0) {
                        printf("[Algo_0_Init] MemUtil_MemblkAlloc in_buf[%u] fail. ret=%u\n", i, ret);
                        ret = 1U;
                    }
                }
            } else {// input_type == DAG_INPUT_TYPE_RAW
                uint32_t j;
                // save the user preference in g_params for filling in memio_source_recv_raw_t
                Preference_GetInputInfo(id, &g_params[id].InputInfo);
                inst->in.num_of_buf = INPUT_DEPTH;
                for (j = 0; j < inst->in.num_of_buf; j++) {
                    if (inst->in.buf[j].pBuffer == NULL) {
                        ret = MemUtil_MemblkAlloc(CACHE_ALIGN(sizeof(memio_source_recv_raw_t)), &(inst->in.buf[j]));
                        if (ret != 0) {
                            printf("[Algo_0_Init] MemUtil_MemblkAlloc in_buf[%u] fail. ret=%u\n", j, ret);
                            ret = 1U;
                            break;
                        }
                    }
                }
            }
        }
    }

    return ret;
}

static uint32_t Algo_0_Release(void)
{
    uint32_t ret, id=0, i, j;
    FLEXIDAG_INSTANCE_s *inst;

    for (i=0; i<g_params[id].fd_amount; i++) {
        inst = &g_params[id].fd[i];
        for (j=0; j<inst->in.num_of_buf; j++) {
            if (inst->in.buf[j].pBuffer != NULL) {
                ret = MemUtil_MemblkFree(&(inst->in.buf[j]));
                if (ret != 0) {
                    printf("[Algo_0_Release] MemUtil_MemblkFree input[%u] fail!\n", j);
                    return 1;
                }
            }
        }
        ret = DagUtil_ReleaseDag(inst);
        if (ret != 0) {
            printf("[Algo_0_Release] DagUtil_ReleaseDag %u fail!\n", i);
            return 2;
        }
    }

    if (g_params[id].temp_buf.pBuffer != NULL) {
        if (MemUtil_MemblkFree(&g_params[id].temp_buf) != 0 ) {
            printf("[Algo_0_Release] MemUtil_MemblkFree g_temp_buf fail!\n");
            return 3;
        }
    }

    return 0;
}

/**
 * Run the network flow for CH0.
 * This function will feed a frame into NN and get the result after NN done.
 **/
static uint32_t Algo_0_Process(memio_source_recv_picinfo_t *in, AMBA_CV_FLEXIDAG_IO_s **result)
{
    uint32_t ret = 0U, i, id = 0U;
    FLEXIDAG_INSTANCE_s *fd_inst;
    AMBA_CV_FLEXIDAG_HANDLE_s *fd_gen_handle;
    AMBA_CV_FLEXIDAG_RUN_INFO_s run_info;
    uint32_t pyramid_index = g_params[id].RoiCfg.image_pyramid_index;

    if (in->pic_info.pyramid.half_octave[pyramid_index].ctrl.disable == 1) {
        printf("\n\n");
        printf("[Fatal Error] Pyramid layer %u is not enabled!\n", pyramid_index);
        printf("Please check your RTOS format_id setting!\n");
        printf("\n\n");
        *result = NULL;
        return 999U;
    }

    fd_inst = &g_params[id].fd[0];
    fd_gen_handle = &(fd_inst->handle);
    if (g_params[id].dag_input_type == DAG_INPUT_TYPE_PIC) {
        /* setup input */
        Process_input_picinfo(id, in, &(fd_inst->in.buf[0]));
    } else { // input_type == DAG_INPUT_TYPE_RAW
        Process_input_raw(id, in, fd_inst->in.buf);
    }

    /* clean output */
    for(i = 0; i < fd_inst->out.num_of_buf; i++) {
        /* since the output came from ORC/VP, ARM should invalidate cache area before access it. */
        MemUtil_MemblkInvalid(&(fd_inst->out.buf[i]));
    }

    /* run flexidag */
    /****
     * You should implement your flow here.
     * for example, you may want to run multiple flexidag such as
     * AmbaCV_FlexidagRun(NN1)->ARM process1->AmbaCV_FlexidagRun(NN2)->ARM process2->Generate Output
     ****/
    ret = AmbaCV_FlexidagRun(fd_gen_handle, &(fd_inst->in), &(fd_inst->out), &run_info);
    if (ret != 0) {
        char log_path[] = "./flexidag_log_dump.txt";

        printf("[Algo_0_Process] AmbaCV_FlexidagRun fail \n");
        AmbaCV_FlexidagDumpLog(fd_gen_handle,log_path, FLEXILOG_CORE0 | FLEXILOG_VIS_CVTASK);
        ret = 1U;
    }
    printf("[Algo_0_Process] start time (%d), end time (%d), output_not_generated (%d) \n", run_info.start_time, run_info.end_time, run_info.output_not_generated);

    if(run_info.output_not_generated == 1U) {
        printf("[Algo_0_Process] There is no result.\n");
        *result = NULL;
        ret = 2U;
    } else {
        *result = &(fd_inst->out);
    }

    return ret;
}

uint32_t DagFlow_Init(uint32_t id, char *fl_path)
{
    uint32_t ret = 1;
    uint32_t DagInputType;
    Preference_GetDagInputType(id, &DagInputType);
    if (DagInputType < SRC_INPUT_TYPE_NUM) {
        if (id == 0) {
            ret = Algo_0_Init(DagInputType, fl_path);
        } else {
            printf("[Dag_Init] invalid id(%u)!\n",id);
        }
    }

    return ret;
}

uint32_t DagFlow_Release(uint32_t id)
{
    uint32_t ret = 1;

    if (id == 0) {
        ret = Algo_0_Release();
    } else {
        printf("[Dag_Release] invalid id(%u)!\n",id);
    }

    return ret;
}

uint32_t DagFlow_Process(uint32_t id, memio_source_recv_picinfo_t *in, AMBA_CV_FLEXIDAG_IO_s **result)
{
    uint32_t ret = 1;
    if (id == 0) {
        ret = Algo_0_Process(in, result);
    } else {
        printf("[Dag_Process] unknown id(%u)!!\n", id);
    }

    return ret;
}

