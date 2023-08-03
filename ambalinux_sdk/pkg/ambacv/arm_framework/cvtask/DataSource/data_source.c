/*
 * Copyright (c) 2017-2017 Ambarella, Inc.
 *
 * This file and its contents ("Software") are protected by intellectual
 * property rights including, without limitation, U.S. and/or foreign copyrights.
 * This Software is also the confidential and proprietary information of
 * Ambarella, Inc. and its licensors.  You may not use, reproduce, disclose,
 * distribute, modify, or otherwise prepare derivative works of this Software
 * or any portion thereof except pursuant to a signed license agreement or
 * nondisclosure agreement with Ambarella, Inc. or its authorized affiliates.
 * In the absence of such an agreement, you agree to promptly notify and
 * return this Software to Ambarella, Inc.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 * MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef CONFIG_CV_CONFIG_TX_SDK7
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include "cvtask_api.h"
#include <rtos/AmbaRTOSWrapper.h>
#else
#include "cvtask_txapi.h"
#endif
#include "cvtask_ossrv.h"
#include "cvtask_errno.h"
#include "build_version.h"
#include "AmbaTransfer.h"
#include "cvapi_idsp_interface.h"
#include "cvapi_memio_interface.h"
#include "cvapi_svccvalgo_memio_interface.h"

#ifndef CONFIG_CV_CONFIG_TX_SDK7
#define AmbaMisra_TypeCast    AmbaMisra_TypeCast64
#else
#define AmbaMisra_TypeCast    AmbaMisra_TypeCast32
#endif
#define TASKNAME_DATA_SOURCE	"DATA_SOURCE_TASK"

extern uint64_t ambacv_v2p(void *va);
extern void cvtask_printf(int lvl, const char *fmt,
    uint32_t v0, uint32_t v1, uint32_t v2, uint32_t v3, uint32_t v4);
extern uint32_t data_source_create(void);

typedef struct{
    int32_t source_hdlr;
    uint32_t sub_channel;
    uint32_t output_mode;
    uint32_t num_outputs;
    memio_source_send_req_t req;
    memio_source_recv_raw_t out_mode0;
    memio_source_recv_picinfo_t out_mode1;
    memio_source_recv_multi_raw_t out_mode2;
}data_source_priv_t;


static uint32_t query(uint32_t iid, const uint32_t *config,
    cvtask_memory_interface_t *dst)
{
    char label[32] = "DATA_SOURCE_##_IONAME_###";
    const void *ioname;
    const char *str;
    uint32_t str_size, res, retcode = CVTASK_ERR_OK;
    uint32_t input, idx;

    (void) iid;
    cvtask_printf(LVL_VERBOSE, "query cvtask [DATA_SOURCE_TASK]", 0, 0, 0, 0, 0);

    memset(dst, 0, sizeof(*dst));
    dst->Instance_private_storage_needed  = sizeof(data_source_priv_t);
    dst->num_inputs = 0;

    if (config[0] == 0U) {
        // yuv buffer desc
        input = 0U;
        label[12] = '0' + ((config[2] / 10U) % 10U);
        label[13] = '0' + ((config[2]      ) % 10U);

        label[22] = '0' + ((input / 100U) % 10U);
        label[23] = '0' + ((input /  10U) % 10U);
        label[24] = '0' + ((input       ) % 10U);
        res = cvtable_find(label, &ioname, &str_size);
        AmbaMisra_TypeCast(&str, &ioname);
        if (res != (uint32_t)CVTASK_ERR_OK) {
            memcpy(dst->output[0].io_name, "INPUT", 6U);
        } else {
            memcpy(dst->output[0].io_name, str, str_size);
        }
        dst->num_outputs = 1U;
        dst->output[0].buffer_size = sizeof(memio_source_recv_raw_t);
    } else if (config[0] == 1U) {
        // cv_pic_info_t
        dst->num_outputs = 2U;
        memcpy(dst->output[0].io_name, IONAME_CV_PIC_INFO, 13U);
        dst->output[0].buffer_size = sizeof(cv_pic_info_t);
        memcpy(dst->output[1].io_name, IONAME_CV_PIC_INFO_ONLY, 20U);
        dst->output[1].buffer_size = sizeof(cv_pic_info_t);
    } else if (config[0] == 2U) {
        // multiple raw output
        dst->num_outputs = (uint8_t) config[3];
        for (idx = 0; idx < config[3]; idx++) {
            label[12] = '0' + ((config[2] / 10U) % 10U);
            label[13] = '0' + ((config[2]      ) % 10U);

            label[22] = '0' + ((idx / 100U) % 10U);
            label[23] = '0' + ((idx /  10U) % 10U);
            label[24] = '0' + ((idx       ) % 10U);
            res = cvtable_find(label, &ioname, &str_size);
            AmbaMisra_TypeCast(&str, &ioname);
            if (res != (uint32_t)CVTASK_ERR_OK) {
                memcpy(dst->output[idx].io_name, "INPUT", 6U);
            } else {
                memcpy(dst->output[idx].io_name, str, str_size);
            }

            dst->output[idx].buffer_size = sizeof(memio_source_recv_raw_t);
        }
    } else {
        //do nothing
    }
    return retcode;
}

static uint32_t init(const cvtask_parameter_interface_t *env,
    const uint32_t *config)
{
    uint32_t retcode;
    AMBA_TRANSFER_CONNECT_s Conf;
    data_source_priv_t *pd;
    memio_setting_t Setting;
    const memio_setting_t *pSet;

    cvtask_printf(LVL_VERBOSE, "init cvtask [DATA_SOURCE_TASK]", 0, 0, 0, 0, 0);
    AmbaMisra_TypeCast(&pd, &env->vpInstance_private_storage);

    pd->output_mode = config[0];
    pd->sub_channel = config[1];
    pd->num_outputs = config[3];
    retcode = (uint32_t)CVTASK_ERR_OK;

    AmbaUtility_StringCopy(Conf.Name, AMBATRANFER_NAME_MAXLENGTH, TRANSFER_CHANNEL_MEMIO);
    pSet = &(Setting);
    Setting.SubChannel = pd->sub_channel;
    Setting.Owner = MEMIO_OWNER_CVTASK;
    AmbaMisra_TypeCast(&Conf.Settings, &pSet);
    (void)AmbaTransfer_Connect(&pd->source_hdlr, &Conf);

    return retcode;
}

static uint32_t proc_msg(const cvtask_parameter_interface_t *env)
{
    (void) env;
    return CVTASK_ERR_OK;
}

static void proc_yuv_buf(const cvtask_parameter_interface_t *env)
{
    const data_source_priv_t *pd;
    memio_source_recv_raw_t *pDataOut;
    AmbaMisra_TypeCast(&pd, &env->vpInstance_private_storage);
    AmbaMisra_TypeCast(&pDataOut , &env->vpOutputBuffer[0]);

    if(pd->out_mode0.magic == MMIO_SOURCE_REVC_RAW) {
        memcpy(pDataOut, &pd->out_mode0, sizeof(memio_source_recv_raw_t));
        pDataOut->addr = ambacv_p2c(pd->out_mode0.addr);
    } else {
        cvtask_printf(LVL_NORMAL, "[data_source] mode0 magic number(0x%x) is not currect(0x%x)", pd->out_mode0.magic, MMIO_SOURCE_REVC_RAW, 0U, 0U, 0U);
    }
}

static void proc_cv_pic_info_t(const cvtask_parameter_interface_t *env)
{
    const data_source_priv_t *pd;
    cv_pic_info_t *pDataOut;
    const cv_pic_info_t *pDataIn;
    UINT32 U32DataOut;
    UINT32 i;

    AmbaMisra_TypeCast(&pd, &env->vpInstance_private_storage);

    pDataIn = &pd->out_mode1.pic_info;
    AmbaMisra_TypeCast(&pDataOut , &env->vpOutputBuffer[0]);
    memcpy(pDataOut, pDataIn, sizeof(cv_pic_info_t));

    U32DataOut = ambacv_v2p(pDataOut);
    for (i = 0U ; i < MAX_HALF_OCTAVES ; i++) {
        pDataOut->rpLumaLeft[i] = pDataIn->rpLumaLeft[i] - U32DataOut;
        pDataOut->rpLumaRight[i] = pDataIn->rpLumaRight[i] - U32DataOut;
        pDataOut->rpChromaLeft[i] = pDataIn->rpChromaLeft[i] - U32DataOut;
        pDataOut->rpChromaRight[i] = pDataIn->rpChromaRight[i] - U32DataOut;
    }
    AmbaMisra_TypeCast(&pDataOut , &env->vpOutputBuffer[1]);
    memcpy(pDataOut, pDataIn, sizeof(cv_pic_info_t));
}

static void proc_multi_raw_buf(const cvtask_parameter_interface_t *env)
{
    UINT32 i;
    const data_source_priv_t *pd;
    memio_source_recv_raw_t *pDataOut;
    AmbaMisra_TypeCast(&pd, &env->vpInstance_private_storage);

    for (i = 0; i < pd->num_outputs; i++) {
        AmbaMisra_TypeCast(&pDataOut , &env->vpOutputBuffer[i]);
        if(pd->out_mode2.io[i].magic == MMIO_SOURCE_REVC_RAW) {
            memcpy(pDataOut, &pd->out_mode2.io[i], sizeof(memio_source_recv_raw_t));
            pDataOut->addr = ambacv_p2c(pd->out_mode2.io[i].addr);
        } else {
            cvtask_printf(LVL_NORMAL, "[data_source] mode2 magic number(0x%x) is not currect(0x%x) on io[%d]", pd->out_mode2.io[i].magic, MMIO_SOURCE_REVC_RAW, i, 0U, 0U);
        }
    }
}

static uint32_t run(const cvtask_parameter_interface_t *env)
{
    uint32_t retcode = (uint32_t)CVTASK_ERR_OK;
    UINT32 Rval;
    AMBA_TRANSFER_RECEIVE_s Recv;
    AMBA_TRANSFER_SEND_s    Send;
    data_source_priv_t *pd;
    memio_setting_t Setting;
    const memio_setting_t *pSet;
    const memio_source_send_req_t *pReq;
    const memio_source_recv_raw_t *pOut0;
    const memio_source_recv_picinfo_t *pOut1;
    const memio_source_recv_multi_raw_t *pOut2;

    AmbaMisra_TypeCast(&pd, &env->vpInstance_private_storage);
    pd->req.seq_no++;
    pd->req.cvtask_frameset_id = env->cvtask_frameset_id;
    pReq = &(pd->req);
    AmbaMisra_TypeCast(&Send.PayloadAddr, &pReq);
    Send.Size = sizeof(memio_source_send_req_t);
    Setting.SubChannel = pd->sub_channel;
    Setting.Owner = MEMIO_OWNER_CVTASK;
    pSet = &Setting;
    AmbaMisra_TypeCast(&Send.Settings, &pSet);
    Send.Result = NULL;
    //cvtask_printf(LVL_NORMAL, "[data_source] AmbaTransfer_Send seq_no = %d", pd->req.seq_no, 0U, 0U, 0U, 0U);
    Rval = AmbaTransfer_Send(pd->source_hdlr, &Send);
    if (Rval != TRANSFER_OK) {
         cvtask_printf(LVL_NORMAL, "[data_source] AmbaTransfer_Send fail", 0U, 0U, 0U, 0U, 0U);
         retcode = (uint32_t)CVTASK_ERR_GENERAL;
    }

    if (pd->output_mode == 0U) {
        pOut0 = &(pd->out_mode0);
        AmbaMisra_TypeCast(&Recv.BufferAddr, &pOut0);
        Recv.Size = sizeof(memio_source_recv_raw_t);
    } else if (pd->output_mode == 1U) {
        pOut1 = &(pd->out_mode1);
        AmbaMisra_TypeCast(&Recv.BufferAddr, &pOut1);
        Recv.Size = sizeof(memio_source_recv_picinfo_t);
    } else if (pd->output_mode == 2U) {
        pOut2 = &(pd->out_mode2);
        AmbaMisra_TypeCast(&Recv.BufferAddr, &pOut2);
        Recv.Size = sizeof(memio_source_recv_multi_raw_t);
    } else {
         cvtask_printf(LVL_NORMAL, "[data_source] ERROR: wrong output_mode = %d", pd->output_mode, 0U, 0U, 0U, 0U);
         retcode = (uint32_t)CVTASK_ERR_GENERAL;
    }

    Setting.SubChannel = pd->sub_channel;
    Setting.Owner = MEMIO_OWNER_CVTASK;
    pSet = &Setting;
    AmbaMisra_TypeCast(&Recv.Settings, &pSet);
    Recv.Result = NULL;
    Rval = AmbaTransfer_Receive(pd->source_hdlr, &Recv);
    if (Rval != TRANSFER_OK) {
         cvtask_printf(LVL_NORMAL, "[data_source] AmbaTransfer_Recv fail", 0U, 0U, 0U, 0U, 0U);
         retcode = (uint32_t)CVTASK_ERR_GENERAL;
    }

    if (pd->output_mode == 0U) {
        //cvtask_printf(LVL_DEBUG, "[data_source]: receive msg = 0x%x", pd->out_mode0.addr, 0U, 0U, 0U, 0U);
        proc_yuv_buf(env);
    } else if (pd->output_mode == 1U) {
        //cvtask_printf(LVL_DEBUG, "[data_source] receive rpLumaLeft(0x%x) rpChromaLeft(0x%x) rpLumaRight(0x%x) rpChromaRight(0x%x)",pd->out_mode1.pic_info.rpLumaLeft[0], pd->out_mode1.pic_info.rpChromaLeft[0], pd->out_mode1.pic_info.rpLumaRight[0], pd->out_mode1.pic_info.rpChromaRight[0], 0U);
        proc_cv_pic_info_t(env);
    } else if (pd->output_mode == 2U) {
        proc_multi_raw_buf(env);
    } else {
        cvtask_printf(LVL_NORMAL, "[data_source] ERROR: wrong output_mode = %d", pd->output_mode, 0U, 0U, 0U, 0U);
        retcode = (uint32_t)CVTASK_ERR_GENERAL;
    }

    return retcode;
}

static uint32_t get_info(const cvtask_parameter_interface_t *env,
    uint32_t info_id, void *info_data)
{
    (void) env;
    (void) info_id;
    (void) info_data;

    return CVTASK_ERR_OK;
}

static uint32_t finish(const cvtask_parameter_interface_t *env)
{
    AMBA_TRANSFER_DISCONNECT_s Disconf;
    data_source_priv_t *pd;
    memio_setting_t Setting;
    const memio_setting_t *pSet;

    AmbaMisra_TypeCast(&pd, &env->vpInstance_private_storage);
    pSet = &(Setting);
    Setting.SubChannel = pd->sub_channel;
    Setting.Owner = MEMIO_OWNER_CVTASK;
    AmbaMisra_TypeCast(&Disconf.Settings, &pSet);
    (void)AmbaTransfer_Disconnect(pd->source_hdlr, &Disconf);

    return CVTASK_ERR_OK;
}

uint32_t data_source_create(void)
{
    static const cvtask_entry_t entry = {
        .cvtask_name     = TASKNAME_DATA_SOURCE,
        .cvtask_type     = (uint32_t)CVTASK_TYPE_ARM_BLOCKABLE_FEEDER,
        .cvtask_query    = query,
        .cvtask_init     = init,
        .cvtask_get_info = get_info,
        .cvtask_process_messages = proc_msg,
        .cvtask_run      = run,
        .cvtask_finish = finish,
    };
    // register ourself to the framework so it knows our existence
    cvtask_register(&entry, CVTASK_API_VERSION);
    return CVTASK_ERR_OK;
}

#ifndef CONFIG_CV_CONFIG_TX_SDK7
cvtask_declare_create (data_source_create);
#else
static  initcall_t __initcall_data_source_create
            GNU_SECTION_CVTASK_CREATE_FP = data_source_create;
#endif
