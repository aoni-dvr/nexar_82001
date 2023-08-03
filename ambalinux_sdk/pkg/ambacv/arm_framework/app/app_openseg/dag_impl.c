#include <stdio.h>
#include <string.h>

#include "cvapi_ambacv_flexidag.h"
#include "idsp_roi_msg.h"
#include "cvapi_amba_mask_interface.h"

#include "mem_util.h"
#include "dag_util.h"
#include "dag_impl.h"

static amba_mask_config_t mask_cfg = {
    .msg_type = AMBA_MASK_CONFIG_MSG_TYPE,
    .image_pyramid_index = 1U,
    .roi_start_col = 42U,
    .roi_start_row = 180U,
    .roi_width = 1280U,
    .roi_height = 512U,
    .buff_width = 320,
    .buff_height = 128,
    .mask_width = 320,
    .mask_height = 128,
    .mask_start_col = 0,
    .mask_start_row = 0,
    .num_categories = 4,
};

#define MAX_FLOW_AMOUNT     (2)
#define MAX_FLEXIDAG_AMOUNT (2)

extern uint32_t debug; //show debug message

static flexidag_memblk_t g_temp_buf[MAX_FLOW_AMOUNT] = {0};
static FLEXIDAG_INSTANCE_s fd[MAX_FLOW_AMOUNT][MAX_FLEXIDAG_AMOUNT] = {0};
static uint32_t fd_amount[MAX_FLOW_AMOUNT] = {0}; //amount means how many flexidags in this flow.
static flexidag_memblk_t seg_maskoutblk[MAX_FLOW_AMOUNT] = {0};

static void Process_input_picinfo(memio_source_recv_picinfo_t *pInfo, flexidag_memblk_t *input)
{
    memio_source_recv_picinfo_t *dest;
    int i;

    /* Always copy frame descripter to dag's input buffer. */
    /* This is because the pic_info might not be in the area which Dag can access. */
    dest = (memio_source_recv_picinfo_t *)input->pBuffer;
    memcpy(dest, pInfo, sizeof(memio_source_recv_picinfo_t));

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

    MemUtil_MemblkClean(input);
}

static uint32_t Algo_0_Cfg(AMBA_CV_FLEXIDAG_HANDLE_s *Handler)
{
// UUID is defined in flow table
#define OPENSEGNET_VP  (2U)
    AMBA_CV_FLEXIDAG_MSG_s Msg;
    uint16_t FlowID = 0U;
    uint32_t Ret = 0U;
    amba_roi_config_t roi_cfg = {0};

    roi_cfg.msg_type = AMBA_ROI_CONFIG_MSG;
    roi_cfg.image_pyramid_index = mask_cfg.image_pyramid_index;
    roi_cfg.source_vin = 0U;
    roi_cfg.roi_start_col = mask_cfg.roi_start_col;
    roi_cfg.roi_start_row = mask_cfg.roi_start_row;

    (void) AmbaCV_FlexidagGetFlowIdByUUID(Handler, OPENSEGNET_VP, &FlowID);

    Msg.flow_id = FlowID;
    Msg.vpMessage = (void *)&roi_cfg;
    Msg.length = sizeof(amba_roi_config_t);
    (void) AmbaCV_FlexidagSendMsg(Handler, &Msg);

    return Ret;
}

static uint32_t Algo_0_Init(char *fl_path)
{
    uint32_t ret, id = 0, i;
    char path[128] = {0};
    FLEXIDAG_INSTANCE_s *inst;

    if (g_temp_buf[id].pBuffer != NULL) {
        printf("[Algo_0_Init] Already Inited!!\n");
        return 1;
    }

    fd_amount[id] = 0;
    /* create instance for all flexidag which will be used in this Algo */
    /****
    /* for example, if your flow is NN1->ARM tack->NN2->ARM task,
    /* you have to create instance for NN1 and NN2 now
    ****/
    snprintf(path, sizeof(path), "%s/flexibin0.bin",fl_path);
    ret = DagUtil_CreateDag(path, &fd[id][0]);
    if (ret != 0) {
        printf("[Algo_0_Init] DagUtil_CreateDag create Fail.\n");
        return ret;
    }
    fd_amount[id]++;

    /* alloc global temp buffer. */
    /****
    /* If there are multi dag existed and they will not run at the same time,
    /* it should be the maximal size over all.
    ****/
    {
        uint32_t max_val = 0, temp;

        for (i=0; i<fd_amount[id]; i++) {
            inst = &fd[id][i];
            temp = inst->handle.mem_req.flexidag_temp_buffer_size;
            if (temp > max_val) {
                max_val = temp;
            }
        }

        if (max_val != 0) {
            ret = MemUtil_MemblkAlloc(CACHE_ALIGN(max_val), &g_temp_buf[id]);
            if (ret != 0) {
                printf("[Algo_0_Init] MemUtil_MemblkAlloc temp_buf Fail.\n");
                return ret;
            }
        } else {
            printf("[Algo_0_Init] no need for temp buffer!\n");
        }
    }

    /* after all instance are created, now you have to init them one by one. */
    for (i=0; i<fd_amount[id]; i++) {
        inst = &fd[id][i];
        ret = DagUtil_InitDag(inst, &g_temp_buf[id]);
        if (ret != 0) {
            printf("[Algo_0_Init] DagUtil_InitDag %u Fail.\n", i);
        } else {
            /* After dag inited, you may want to do specific configurations */
            Algo_0_Cfg(&inst->handle);

            /* configure input buffer */
            //This is model/flow specific.
            //Maybe it doesn't need to create buffer for each flexidag.
            inst->in.num_of_buf = 1;
            if(inst->in.buf[0].pBuffer == NULL) {
                ret = MemUtil_MemblkAlloc(CACHE_ALIGN(sizeof(memio_source_recv_picinfo_t)), &(inst->in.buf[0]));
                if (ret != 0) {
                    printf("[Algo_0_Init] MemUtil_MemblkAlloc in_buf[%u] fail. ret=%u\n", i, ret);
                    ret = 1U;
                }
            }
        }
    }

    /* prepare amba_mask_out_t */
    if (ret == 0) {
        ret = MemUtil_MemblkAlloc(CACHE_ALIGN(sizeof(amba_mask_out_t)), &seg_maskoutblk[id]);
        if (ret != 0) {
            printf("[Algo_0_Init] MemUtil_MemblkAlloc od_results fail. ret=%u\n", ret);
            ret = 1U;
        }
    }

    return ret;
}

static uint32_t Algo_0_Release(void)
{
    uint32_t ret, id=0, i, j;
    FLEXIDAG_INSTANCE_s *inst;

    for (i=0; i<fd_amount[id]; i++) {
        inst = &fd[id][i];
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

    if (g_temp_buf[id].pBuffer != NULL) {
        if (MemUtil_MemblkFree(&g_temp_buf[id]) != 0 ) {
            printf("[Algo_0_Release] MemUtil_MemblkFree g_temp_buf fail!\n");
            return 3;
        }
    }

    if (seg_maskoutblk[id].pBuffer != NULL) {
        if (MemUtil_MemblkFree(&seg_maskoutblk[id]) != 0 ) {
            printf("[Algo_0_Release] MemUtil_MemblkFree seg_maskoutblk fail!\n");
            return 4;
        }
    }

    return 0;
}

/**
 * Run the network flow for CH0.
 * This function will feed a frame into NN and get the result after NN done.
 **/
static uint32_t Algo_0_Process(memio_source_recv_picinfo_t *in, void **result)
{
    uint32_t ret = 0U, i, id = 0U;
    FLEXIDAG_INSTANCE_s *fd_inst;
    AMBA_CV_FLEXIDAG_HANDLE_s *fd_gen_handle;
    AMBA_CV_FLEXIDAG_RUN_INFO_s run_info;

    if (in->pic_info.pyramid.half_octave[mask_cfg.image_pyramid_index].ctrl.disable == 1) {
        printf("\n\n");
        printf("[Fatal Error] Pyramod layer %u is not enabled!\n",mask_cfg.image_pyramid_index);
        printf("Please check your RTOS format_id setting!\n");
        printf("\n\n");
        *result = NULL;
        return 999U;
    }

    fd_inst = &fd[id][0];
    fd_gen_handle = &(fd_inst->handle);

    /* setup input */
    Process_input_picinfo(in, &(fd_inst->in.buf[0]));
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

    if (debug) {
        printf("[Algo_0_Process] start time (%d), end time (%d), output_not_generated (%d) \n", run_info.start_time, run_info.end_time, run_info.output_not_generated);
    }

    /* prepare result */
    if(run_info.output_not_generated == 1U) {
        printf("[Algo_0_Process] There is no result.\n");
        *result = NULL;
        ret = 2U;
    } else {
        amba_mask_out_t *maskOut;

        maskOut = (amba_mask_out_t *)seg_maskoutblk[id].pBuffer;

        maskOut->buff_addr = fd_inst->out.buf[0].buffer_daddr;
        maskOut->buff_width = mask_cfg.buff_width;
        maskOut->buff_height = mask_cfg.buff_height;
        maskOut->mask_start_col = mask_cfg.mask_start_col;
        maskOut->mask_start_row = mask_cfg.mask_start_row;
        maskOut->mask_width = mask_cfg.mask_width;
        maskOut->mask_height = mask_cfg.mask_height;

        MemUtil_MemblkClean(&seg_maskoutblk[id]);
        *result = (void *)&seg_maskoutblk[id];
    }

    return ret;
}

uint32_t DagFlow_Init(uint32_t id, char *fl_path)
{
    uint32_t ret = 1;

    if (id == 0) {
        ret = Algo_0_Init(fl_path);
    } else {
        printf("[Dag_Init] invalid id(%u)!\n",id);
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

uint32_t DagFlow_Process(uint32_t id, memio_source_recv_picinfo_t *in, void **result)
{
    uint32_t ret = 1;

    if (id == 0) {
        ret = Algo_0_Process(in, result);
    } else {
        printf("[Dag_Process] unknown id(%u)!!\n", id);
    }

    return ret;
}

