
#include <cvtask_interface.h>
#include <vis_coproc.h>
#include <cvtask_errno.h>
#include <orc_memory.h>
#include <ucode_debug.h>
#include <vp_common.h>

#include "cvapi_idsp_interface.h"
#include "cvtask_bisenet_pelee_picinfo_.h"

#define PROTECTION_SAMPLE_CODE
#ifdef PROTECTION_SAMPLE_CODE
#include "aes.h"
#endif //PROTECTION_SAMPLE_CODE

#define ALIGN_32_BYTE(x)    ((((x) + 31) >> 5) << 5)

#define NET_INPUT_W 1280
#define NET_INPUT_H 512

#define MAX_PACKED_OUTPUTS 16
#define MAX_OUTPUTS (CVTASK_MAX_OUTPUTS * MAX_PACKED_OUTPUTS)
static int32_t task_out_sz[MAX_OUTPUTS];

typedef struct cvtask_output_s {
    int32_t magic_number;
    int32_t num_outputs;
    int32_t offsets[MAX_PACKED_OUTPUTS];
} cvtask_output_t;

#ifdef PROTECTION_SAMPLE_CODE
#define LICENSE_SIZE    (32U)
#endif

typedef struct img_ctx_s {
    uint32_t daddr_img_y_base;
    uint32_t dpitch_m1_img_y;
    uint32_t daddr_img_uv_base;
    uint32_t dpitch_m1_img_uv;

    uint32_t source_vin;
    uint32_t idsp_pyramid_scale;
    uint32_t roi_start_col;
    uint32_t roi_start_row;

#ifdef PROTECTION_SAMPLE_CODE
    uint8_t license[LICENSE_SIZE];
    uint8_t AmbaUUID[32];
    uint32_t license_status; // 0: not verified 1: pass 0xff: failed
#endif //PROTECTION_SAMPLE_CODE
} img_ctx_t;

typedef struct win_ctx_s {
    uint32_t raw_w;
    uint32_t raw_h;
    uint32_t roi_w;
    uint32_t roi_h;

    uint32_t roi_start_col;
    uint32_t roi_start_row;
    uint32_t net_in_w;
    uint32_t net_in_h;
} win_ctx_t;

errcode_enum_t bisenet_pelee_picinfo__prim_cvtask_register(cvtask_entry_t *pCVTaskEntry)
{
    char task_name[NAME_MAX_LENGTH] = "bisenet_pelee";

    CVTASK_PRINTF(LVL_NORMAL, ">>>>> starting bisenet_pelee_picinfo__prim_cvtask_register\n", 0, 0, 0, 0, 0);

    visorc_strcpy(&pCVTaskEntry->cvtask_name[0], &task_name[0], NAME_MAX_LENGTH);

    pCVTaskEntry->cvtask_type = CVTASK_TYPE_VPANY;
    pCVTaskEntry->cvtask_api_version = CVTASK_API_VERSION;

    pCVTaskEntry->cvtask_query = &bisenet_pelee_picinfo__prim_cvtask_query;
    pCVTaskEntry->cvtask_init = &bisenet_pelee_picinfo__prim_cvtask_init;
    pCVTaskEntry->cvtask_get_info = &bisenet_pelee_picinfo__prim_cvtask_get_info;
    pCVTaskEntry->cvtask_process_messages = &bisenet_pelee_picinfo__prim_cvtask_process_messages;
    pCVTaskEntry->cvtask_run = &bisenet_pelee_picinfo__prim_cvtask_run;

    CVTASK_PRINTF(LVL_NORMAL, ">>>>> completed bisenet_pelee_picinfo__prim_cvtask_register\n", 0, 0, 0, 0, 0);

    return ERRCODE_NONE;
}

cvtask_memory_interface_t bisenet_pelee_picinfo__prim_cvtask_interface = {
    .num_inputs = 1,
    .input[0].io_name = "IDSP_PICINFO",
    .input[0].history_needed = 0,

    .num_outputs = 1,
    .output[0].io_name = "BISENET_PELEE_PICINFO_OUTPUT_0",
    .output[0].history_needed = 0,
};

errcode_enum_t bisenet_pelee_picinfo__prim_cvtask_query(uint32_t instance, const uint32_t *pConfigData, cvtask_memory_interface_t *pCVTaskMemory)
{
    errcode_enum_t retcode = ERRCODE_NONE;


    if ((pCVTaskMemory == NULL) || (pConfigData == NULL)) {
        retcode = ERRCODE_BAD_PARAMETER;
    }

    if (is_not_err(retcode)) {
        int32_t dag_cntxt_sz, interm_out_sz;

        retcode = bisenet_pelee_picinfo__prim_dag_query(&dag_cntxt_sz, &interm_out_sz, &task_out_sz[0]);
        if (is_not_err(retcode)) {
            CVTASK_PRINTF(LVL_NORMAL, ">>>>> bisenet_pelee_picinfo__prim_cvtask_query(): bisenet_pelee_picinfo__prim_dag_query output: dag_cntxt_sz=%d, interm_out_sz=%d\n", dag_cntxt_sz, interm_out_sz, 0, 0, 0);

            *pCVTaskMemory = bisenet_pelee_picinfo__prim_cvtask_interface;

            // packed outputs order here matches the order in dag (and dag merge) query functions

            pCVTaskMemory->output[0].buffer_size = 0 + task_out_sz[0];
            CVTASK_PRINTF(LVL_NORMAL, ">>>>> bisenet_pelee_picinfo__prim_cvtask_query(): bisenet_pelee_picinfo__prim_dag_query task 0 output size: %d\n", pCVTaskMemory->output[0].buffer_size, 0, 0, 0, 0);

            pCVTaskMemory->CVTask_shared_storage_needed = 0;
            pCVTaskMemory->DRAM_temporary_scratchpad_needed = interm_out_sz;
            pCVTaskMemory->Instance_private_storage_needed = dag_cntxt_sz + sizeof(img_ctx_t);
        } else {
            CVTASK_PRINTF(LVL_CRITICAL, ">>>>> bisenet_pelee_picinfo__prim_cvtask_query(): bisenet_pelee_picinfo__prim_dag_query failed\n", 0, 0, 0, 0, 0);
        }
    }

    return retcode;
}

errcode_enum_t bisenet_pelee_picinfo__prim_cvtask_init(const cvtask_parameter_interface_t *pCVTaskParams, const uint32_t *pConfigData)
{
    errcode_enum_t retcode = ERRCODE_NONE;

    if ((pCVTaskParams == NULL) || (pConfigData == NULL)) {
        retcode = ERRCODE_BAD_PARAMETER;
    }

    if (is_not_err(retcode)) {
        uint32_t used_size;
        retcode = bisenet_pelee_picinfo__prim_dag_init((void*)(pCVTaskParams->vpInstance_private_storage + sizeof(img_ctx_t)), pCVTaskParams->Instance_private_storage_size, &used_size);
        if (is_not_err(retcode)) {
            CVTASK_PRINTF(LVL_NORMAL, ">>>>> bisenet_pelee_picinfo__prim_cvtask_init(): completed bisenet_pelee_picinfo__prim_dag_init(), allocated_size=%d, used_size=%d\n", pCVTaskParams->Instance_private_storage_size, used_size, 0, 0, 0);

            img_ctx_t* pctx = (img_ctx_t *) pCVTaskParams->vpInstance_private_storage;

            pctx->idsp_pyramid_scale = pConfigData[0];
            pctx->roi_start_col      = pConfigData[1];
            pctx->roi_start_row      = pConfigData[2];
            pctx->source_vin         = pConfigData[3];
#ifdef PROTECTION_SAMPLE_CODE
            pctx->license_status     = 0;
#endif //PROTECTION_SAMPLE_CODE
        } else {
            CVTASK_PRINTF(LVL_CRITICAL, ">>>>> bisenet_pelee_picinfo__prim_cvtask_init(): bisenet_pelee_picinfo__prim_dag_init failed\n", 0, 0, 0, 0, 0);
        }
    }

    return retcode;
}

errcode_enum_t bisenet_pelee_picinfo__prim_cvtask_get_info(const cvtask_parameter_interface_t *pCVTaskParams, uint32_t info_index, void *vpInfoReturn)
{
    errcode_enum_t retcode = ERRCODE_NONE;

    return retcode;
}

errcode_enum_t bisenet_pelee_picinfo__prim_cvtask_process_messages(const cvtask_parameter_interface_t *pCVTaskParams)
{
    errcode_enum_t retcode = ERRCODE_NONE;
#ifdef PROTECTION_SAMPLE_CODE
    uint32_t i, j;
    uint32_t msg_count = pCVTaskParams->cvtask_num_messages;

    img_ctx_t* pd;
    pd = (img_ctx_t*) pCVTaskParams->vpInstance_private_storage;

    if (msg_count == 0) {
        CVTASK_PRINTF(LVL_MINIMAL,
                      "[bisenet_pelee_picinfo__prim_cvtask_process_messages] "
                      "No messages to process\n");
        return ERRCODE_NONE;
    }

    for (i=0; i<msg_count; ++i) {
        uint32_t msg_type = *((uint32_t*)(pCVTaskParams->vpMessagePayload[i]));
        CVTASK_PRINTF(LVL_VERBOSE,
                      "\tProcessing bisenet_pelee_picinfo__prim_cvtask_process_messages message: %d, type: %d\n",
                      i, msg_type);

        if (msg_type == 0xAB001000) {
            uint8_t *tmp = (uint8_t*)(pCVTaskParams->vpMessagePayload[i]);
            tmp +=4;
            // store license
            for(j=0 ; j<LICENSE_SIZE ; j++) {
                pd->license[j] = tmp[j];
            }
            CVTASK_PRINTF(LVL_DEBUG,
                          "License:\n");
            CVTASK_PRINTF(LVL_DEBUG,
                          "%02x %02x %02x %02x\n",
                          pd->license[0], pd->license[1], pd->license[2], pd->license[3]);
            CVTASK_PRINTF(LVL_DEBUG,
                          "%02x %02x %02x %02x\n",
                          pd->license[4], pd->license[5], pd->license[6], pd->license[7]);
            CVTASK_PRINTF(LVL_DEBUG,
                          "%02x %02x %02x %02x\n",
                          pd->license[8], pd->license[9], pd->license[10], pd->license[11]);
            CVTASK_PRINTF(LVL_DEBUG,
                          "%02x %02x %02x %02x\n",
                          pd->license[12], pd->license[13], pd->license[14], pd->license[15]);
        }
    }
#endif //PROTECTION_SAMPLE_CODE    
    return retcode;
}
static errcode_enum_t roi_handling(img_ctx_t *pctx, const cvtask_parameter_interface_t *pCVTaskParams)
{
    errcode_enum_t retcode = ERRCODE_NONE;

    cv_pic_info_t *idsp_pic_info;
    uint32_t roi_w_m1, roi_h_m1, luma, chroma, pitch;
    uint32_t index, num_found;
    win_ctx_t my_ctx = {0};

    idsp_pic_info = (cv_pic_info_t *)(pCVTaskParams->vpInputBuffer[0]);

    roi_w_m1 = idsp_pic_info->pyramid.half_octave[pctx->idsp_pyramid_scale].roi_width_m1;
    roi_h_m1 = idsp_pic_info->pyramid.half_octave[pctx->idsp_pyramid_scale].roi_height_m1;

    if (pctx->roi_start_col == 9999)
        pctx->roi_start_col = (roi_w_m1 + 1 - NET_INPUT_W) / 2;
    else if (pctx->roi_start_col < 0)
        pctx->roi_start_col = 0;
    else if (pctx->roi_start_col > (roi_w_m1 + 1 - NET_INPUT_W))
        pctx->roi_start_col = (roi_w_m1 + 1 - NET_INPUT_W);

    if (pctx->roi_start_row == 9999)
        pctx->roi_start_row = (roi_h_m1 + 1 - NET_INPUT_H) / 2;
    else if (pctx->roi_start_row < 0)
        pctx->roi_start_row = 0;
    else if (pctx->roi_start_row > (roi_h_m1 + 1 - NET_INPUT_H))
        pctx->roi_start_row = (roi_h_m1 + 1 - NET_INPUT_H);

    CVTASK_PRINTF(LVL_DEBUG, "roi_handling: pyramid_scale = %d, start_col = %d, start_row= %d\n", pctx->idsp_pyramid_scale, pctx->roi_start_col, pctx->roi_start_row, 0, 0);

    if (pctx->source_vin == 1) {
        luma   = idsp_pic_info->rpLumaLeft[pctx->idsp_pyramid_scale];
        chroma = idsp_pic_info->rpChromaLeft[pctx->idsp_pyramid_scale];
    } else {
        luma   = idsp_pic_info->rpLumaRight[pctx->idsp_pyramid_scale];
        chroma = idsp_pic_info->rpChromaRight[pctx->idsp_pyramid_scale];
    }
    pitch = (uint32_t) idsp_pic_info->pyramid.half_octave[pctx->idsp_pyramid_scale].ctrl.roi_pitch;

    pctx->daddr_img_y_base  = (uint32_t) (idsp_pic_info) + luma + pctx->roi_start_row * pitch + pctx->roi_start_col;
    pctx->daddr_img_uv_base = (uint32_t) (idsp_pic_info) + chroma + (pctx->roi_start_row & 0xfffffffe) * pitch / 2 + (pctx->roi_start_col & 0xfffffffe);
    pctx->dpitch_m1_img_y   = pitch - 1;
    pctx->dpitch_m1_img_uv  = pitch - 1;

    CVTASK_PRINTF(LVL_DEBUG, "roi_handling: dpitch_m1 = %d, roi_w_m1 = %d, roi_h_m1 = %d\n", pctx->dpitch_m1_img_y, roi_w_m1, roi_h_m1, 0, 0);

    my_ctx.raw_w = idsp_pic_info->pyramid.half_octave[0].roi_width_m1 + 1;
    my_ctx.raw_h = idsp_pic_info->pyramid.half_octave[0].roi_height_m1 + 1;
    my_ctx.roi_w = roi_w_m1 + 1;
    my_ctx.roi_h = roi_h_m1 + 1;

    my_ctx.roi_start_col    = pctx->roi_start_col;
    my_ctx.roi_start_row    = pctx->roi_start_row;
    my_ctx.net_in_w         = NET_INPUT_W;
    my_ctx.net_in_h         = NET_INPUT_H;

    retcode = cvtask_get_sysflow_index_list(NULL, NULL, NULL, "SEGPOSTPROC", &index, 1, &num_found);
    retcode = cvtask_default_message_send(&my_ctx, sizeof(my_ctx), index, CVTASK_MESSAGE_ASAP);

    return retcode;
}

#ifdef PROTECTION_SAMPLE_CODE
static uint32_t _check_license(uint8_t* UUID, uint8_t* license)
{
    uint32_t ret = 0;

    uint32_t i;
    // key to decode license
    uint8_t iv[]  = { 0xf1, 0xe1, 0xd2, 0xc3, 0xb4, 0xa5, 0x96, 0x87, 0x78, 0x69, 0x5a, 0x4b, 0x3c, 0x2d, 0x1e, 0x0f };
    uint8_t key[] = { 0x12, 0x34, 0x56, 0x78, 0x90, 0xab, 0xcd, 0xef, 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef };

    uint8_t tmp[16];
    AES_ctx ctx;

    // a very weak kay protect... try a public key system for better key protection
    for (i=0 ; i<16 ; i++) {
        key[i] = key[i]^iv[i];
        tmp[i] = license[i];
    }
    AES_init_ctx_iv(&ctx, key, iv);
    AES_CBC_decrypt_buffer(&ctx, tmp, 32);
    for (i=0 ; i<32 ; i++) {
        if (UUID[i] != tmp[i]) {
            ret = 0xFFFFFFFF;
            break;
        }
        tmp[i] = 0;
    }

    return ret;
}
#endif //PROTECTION_SAMPLE_CODE

errcode_enum_t bisenet_pelee_picinfo__prim_cvtask_run(const cvtask_parameter_interface_t *pCVTaskParams)
{
    errcode_enum_t retcode = ERRCODE_NONE;

    CVTASK_PRINTF(LVL_DEBUG, ">>>>> cvtask_run() : input num    = %d : %08x %08x %08x %08x\n", pCVTaskParams->cvtask_num_inputs, (uint32_t)pCVTaskParams->vpInputBuffer[0], (uint32_t)pCVTaskParams->vpInputBuffer[1], (uint32_t)pCVTaskParams->vpInputBuffer[2], (uint32_t)pCVTaskParams->vpInputBuffer[3]);
    CVTASK_PRINTF(LVL_DEBUG, "                                         %08x %08x %08x %08x\n", (uint32_t)pCVTaskParams->vpInputBuffer[4], (uint32_t)pCVTaskParams->vpInputBuffer[5], (uint32_t)pCVTaskParams->vpInputBuffer[6], (uint32_t)pCVTaskParams->vpInputBuffer[7], 0);
    CVTASK_PRINTF(LVL_DEBUG, ">>>>> cvtask_run() : output num   = %d : %08x %08x %08x %08x\n", pCVTaskParams->cvtask_num_outputs, (uint32_t)pCVTaskParams->vpOutputBuffer[0], (uint32_t)pCVTaskParams->vpOutputBuffer[1], (uint32_t)pCVTaskParams->vpOutputBuffer[2], (uint32_t)pCVTaskParams->vpOutputBuffer[3]);
    CVTASK_PRINTF(LVL_DEBUG, "                                         %08x %08x %08x %08x\n", (uint32_t)pCVTaskParams->vpOutputBuffer[4], (uint32_t)pCVTaskParams->vpOutputBuffer[5], (uint32_t)pCVTaskParams->vpOutputBuffer[6], (uint32_t)pCVTaskParams->vpOutputBuffer[7], 0);
    CVTASK_PRINTF(LVL_DEBUG, ">>>>> cvtask_run() : feedback num = %d : %08x %08x %08x %08x\n", pCVTaskParams->cvtask_num_feedback, (uint32_t)pCVTaskParams->vpFeedbackBuffer[0], (uint32_t)pCVTaskParams->vpFeedbackBuffer[1], (uint32_t)pCVTaskParams->vpFeedbackBuffer[2], (uint32_t)pCVTaskParams->vpFeedbackBuffer[3]);
    CVTASK_PRINTF(LVL_DEBUG, ">>>>> cvtask_run() : message num  = %d : %08x %08x %08x %08x\n", pCVTaskParams->cvtask_num_messages, (uint32_t)pCVTaskParams->vpMessagePayload[0], (uint32_t)pCVTaskParams->vpMessagePayload[1], (uint32_t)pCVTaskParams->vpMessagePayload[2], (uint32_t)pCVTaskParams->vpMessagePayload[3]);
    if (pCVTaskParams->vpCMEM_temporary_scratchpad         != NULL) CVTASK_PRINTF(LVL_DEBUG, "      cvtask_run() : CMEM Scratchpad = %08x (%9d)\n", (uint32_t)pCVTaskParams->vpCMEM_temporary_scratchpad, pCVTaskParams->CMEM_temporary_scratchpad_size, 0, 0, 0);
    if (pCVTaskParams->vpDRAM_temporary_scratchpad         != NULL) CVTASK_PRINTF(LVL_DEBUG, "      cvtask_run() : DRAM Scratchpad = %08x (%9d)\n", (uint32_t)pCVTaskParams->vpDRAM_temporary_scratchpad, pCVTaskParams->DRAM_temporary_scratchpad_size, 0, 0, 0);
    if (pCVTaskParams->vpInstance_private_storage          != NULL) CVTASK_PRINTF(LVL_DEBUG, "      cvtask_run() : Private Storage = %08x (%9d)\n", (uint32_t)pCVTaskParams->vpInstance_private_storage, pCVTaskParams->Instance_private_storage_size, 0, 0, 0);
    if (pCVTaskParams->vpInstance_private_uncached_storage != NULL) CVTASK_PRINTF(LVL_DEBUG, "      cvtask_run() : UC-Priv Storage = %08x (%9d)\n", (uint32_t)pCVTaskParams->vpInstance_private_uncached_storage, pCVTaskParams->Instance_private_uncached_storage_size, 0, 0, 0);
    if (pCVTaskParams->vpCVTask_shared_storage             != NULL) CVTASK_PRINTF(LVL_DEBUG, "      cvtask_run() : Shared  Storage = %08x (%9d) [%2d]\n", (uint32_t)pCVTaskParams->vpCVTask_shared_storage, pCVTaskParams->CVTask_shared_storage_size, pCVTaskParams->cvtask_instance, 0, 0);

    retcode = bisenet_pelee_picinfo__prim_cvtask_process_messages(pCVTaskParams);

#ifdef PROTECTION_SAMPLE_CODE
    {
        img_ctx_t *pd = (img_ctx_t *) pCVTaskParams->vpInstance_private_storage;
        CVTASK_PRINTF(LVL_DEBUG, ">>>>> License verify start\n", 0, 0, 0, 0, 0);
        if (pd->license_status == 0xFF) {
            // not verified
            CVTASK_PRINTF(LVL_DEBUG, "[ERROR] >>>>> !!! License verify failed\n", 0, 0, 0, 0, 0);
            return ERRCODE_NONE;
        }
        if (pd->license_status == 0) {
            retcode = visorc_sec_get_unique_id(pd->AmbaUUID);

            if (is_err(retcode)) {
                CVTASK_PRINTF(LVL_DEBUG, "[ERROR] >>>>> visorc_sec_get_unique_id failed: ret = 0x%x\n", retcode, 0, 0, 0, 0);
                return ERRCODE_NONE;
            } else if (is_warn(retcode)) {
                CVTASK_PRINTF(LVL_DEBUG, "[WARN] >>>>> visorc_sec_get_unique_id failed: ret = 0x%x\n", retcode, 0, 0, 0, 0);
                return ERRCODE_NONE;
            } else {
                // check
                retcode = _check_license(pd->AmbaUUID, pd->license);
                if (retcode == 0) {
                    CVTASK_PRINTF(LVL_DEBUG, ">>>>> License verify okay\n", 0, 0, 0, 0, 0);
                    pd->license_status = 1;
                } else {
                    CVTASK_PRINTF(LVL_DEBUG, ">>>>> !!!! License verify failed\n", 0, 0, 0, 0, 0);
                    pd->license_status = 0xFF;
                    return ERRCODE_NONE;
                }
            }
        }
        CVTASK_PRINTF(LVL_DEBUG, ">>>>> License verify end\n", 0, 0, 0, 0, 0);
    }
#endif //PROTECTION_SAMPLE_CODE

    if (is_not_err(retcode)) {

        uint32_t start_time, clk_time;
        uint32_t *p_cvtask_in[MAX_OUTPUTS];
        uint32_t *p_cvtask_out[MAX_OUTPUTS];
        uint32_t roi_pitch_m1 = 0;
        img_ctx_t *pctx = (img_ctx_t *) pCVTaskParams->vpInstance_private_storage;

        retcode = roi_handling(pctx, pCVTaskParams);
        p_cvtask_in[0] = (uint32_t *) pctx->daddr_img_y_base;
        p_cvtask_in[1] = (uint32_t *) pctx->daddr_img_uv_base;
        roi_pitch_m1   = pctx->dpitch_m1_img_y;

        p_cvtask_out[0] = (uint32_t *) pCVTaskParams->vpOutputBuffer[0];

        cvtask_prof_new_action(pCVTaskParams->cvtask_core_id, pCVTaskParams->cvtask_sysflow_index, pCVTaskParams->cvtask_frameset_id);
        cvtask_prof_unit_on(pCVTaskParams->cvtask_core_id, pCVTaskParams->cvtask_sysflow_index);
        start_time = get_cur_time();
        CVTASK_PRINTF(LVL_NORMAL, ">>>>> bisenet_pelee_picinfo__prim_cvtask_run(): started @ #%d", start_time, 0, 0, 0, 0);;

        retcode = bisenet_pelee_picinfo__prim_dag_rundag((void*)(pCVTaskParams->vpInstance_private_storage + sizeof(img_ctx_t)), (void*)pCVTaskParams->vpDRAM_temporary_scratchpad, pCVTaskParams->DRAM_temporary_scratchpad_size, (uint32_t *) p_cvtask_in, (uint32_t *) p_cvtask_out, roi_pitch_m1);
        if (is_not_err(retcode)) {
            CVTASK_PRINTF(LVL_NORMAL, ">>>>> bisenet_pelee_picinfo__prim_cvtask_run():bisenet_pelee_picinfo__prim_dag_rundag completed\n", 0, 0, 0, 0, 0);
        } else {
            CVTASK_PRINTF(LVL_CRITICAL, ">>>>> bisenet_pelee_picinfo__prim_cvtask_run():bisenet_pelee_picinfo__prim_dag_rundag failed\n", 0, 0, 0, 0, 0);
        }

        clk_time = get_cur_time() - start_time;
        CVTASK_PRINTF(LVL_DEBUG, ">>>>> bisenet_pelee_picinfo__prim_cvtask_run: %d", clk_time, 0, 0, 0, 0);
        cvtask_prof_unit_off(pCVTaskParams->cvtask_core_id, pCVTaskParams->cvtask_sysflow_index);
    } else {
        CVTASK_PRINTF(LVL_CRITICAL, ">>>>> bisenet_pelee_picinfo__prim_cvtask_run():bisenet_pelee_picinfo__prim_cvtask_process_messages failed\n", 0, 0, 0, 0, 0);
    }

    return retcode;
}
