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
#include "cvtask_txsrv.h"
#endif

#include "cvtask_errno.h"
#include "build_version.h"
#include "AmbaTransfer.h"
#include "cvapi_memio_interface.h"

#ifndef CONFIG_CV_CONFIG_TX_SDK7
#define AmbaMisra_TypeCast    AmbaMisra_TypeCast64
#else
#define AmbaMisra_TypeCast    AmbaMisra_TypeCast32
#endif
#define TASKNAME_DATA_SINK	"DATA_SINK_TASK"

extern uint64_t ambacv_v2p(void *va);
extern void cvtask_printf(int lvl, const char *fmt,
    uint32_t v0, uint32_t v1, uint32_t v2, uint32_t v3, uint32_t v4);
extern int console_printS5(const char *fmt,
    const char *argv0, const char *argv1, const char *argv2, const char *argv3, const char *argv4);
extern uint32_t data_sink_create(void);

struct priv_database {
    int32_t sink_hdlr;
    uint32_t sub_channel;
    memio_sink_send_out_t req;
    memio_sink_recv_act_t out;
};


static uint32_t query(uint32_t iid, const uint32_t *config,
    cvtask_memory_interface_t *dst)
{
    char label[32] = "DATA_SINK_##_IONAME_###";
    const void *ioname;
    const char *str;
    uint32_t str_size, retcode = CVTASK_ERR_OK, idx;

    (void) iid;
    cvtask_printf(LVL_VERBOSE, "query cvtask [DATA_SINK_TASK]", 0, 0, 0, 0, 0);

    memset(dst, 0, sizeof(*dst));

    dst->Instance_private_storage_needed = sizeof(struct priv_database);
    dst->num_outputs = 1U;
    strcpy(dst->output[0].io_name, "DUMMY");
    dst->num_inputs = (uint8_t)(config[0]);
    for (idx = 0; idx < config[0]; idx++) {
        label[10] = '0' + ((config[2] / 10U) % 10U);
        label[11] = '0' + ((config[2]      ) % 10U);

        label[20] = '0' + ((idx / 100U) % 10U);
        label[21] = '0' + ((idx /  10U) % 10U);
        label[22] = '0' + ((idx       ) % 10U);
        retcode = cvtable_find(label, &ioname, &str_size);
        AmbaMisra_TypeCast(&str, &ioname);
        if (retcode != (uint32_t) CVTASK_ERR_OK) {
            console_printS5("can't find label %s", label, NULL, NULL, NULL, NULL);
            retcode = (uint32_t )CVTASK_ERR_GENERAL;
        } else {
            memcpy(dst->input[idx].io_name, str, str_size);
            retcode = (uint32_t) CVTASK_ERR_OK;
        }
    }
    return retcode;
}

static uint32_t init(const cvtask_parameter_interface_t *env,
    const uint32_t *config)
{
    uint32_t retcode;
    uint32_t Rval;
    struct priv_database *db;
    AMBA_TRANSFER_CONNECT_s Conf;
    memio_setting_t Setting;
    const memio_setting_t *pSet;

    cvtask_printf(LVL_VERBOSE, "init cvtask [DATA_SINK_TASK]", 0, 0, 0, 0, 0);
    AmbaMisra_TypeCast(&db, &env->vpInstance_private_storage);
    memset(db, 0x0, sizeof(struct priv_database));
    db->sub_channel = config[1];
    db->req.type= config[3];

    AmbaUtility_StringCopy(Conf.Name, AMBATRANFER_NAME_MAXLENGTH, TRANSFER_CHANNEL_MEMIO);

    pSet = &(Setting);
    Setting.SubChannel = db->sub_channel;
    Setting.Owner = MEMIO_OWNER_CVTASK;
    AmbaMisra_TypeCast(&Conf.Settings, &pSet);
    Rval = AmbaTransfer_Connect(&db->sink_hdlr, &Conf);
    if (Rval != TRANSFER_OK) {
        cvtask_printf(LVL_CRITICAL, "[data_sink] AmbaTransfer_Connect send fail", 0U, 0U, 0U, 0U, 0U);
    }
    retcode = (uint32_t)CVTASK_ERR_OK;

    return retcode;
}

static uint32_t proc_msg(const cvtask_parameter_interface_t *env)
{
    (void) env;
    return CVTASK_ERR_OK;
}

static uint32_t run(const cvtask_parameter_interface_t *env)
{
    //const char *vpInputBuffer0;
    uint32_t retcode = (uint32_t)CVTASK_ERR_OK, i;
    uint32_t Rval;
    AMBA_TRANSFER_RECEIVE_s Recv;
    AMBA_TRANSFER_SEND_s    Send;
    struct priv_database *db;
    memio_setting_t Setting;
    const memio_setting_t *pSet;
    const memio_sink_send_out_t *pReq;
    const memio_sink_recv_act_t *pOut;
    //VIS_UTIL_BBX_LIST_MSG_s *pBBX;

    AmbaMisra_TypeCast(&db, &env->vpInstance_private_storage);

    for(i = 0; i < env->cvtask_num_inputs ; i++) {
        db->req.io[i].addr = ambacv_v2p(env->vpInputBuffer[i]);
        db->req.io[i].size = cvtask_get_input_size(i);
    }
    db->req.num_of_io = env->cvtask_num_inputs;
    db->req.cvtask_frameset_id = env->cvtask_frameset_id;

    pReq = &db->req;
    AmbaMisra_TypeCast(&Send.PayloadAddr, &pReq);
    Send.Size = sizeof(memio_sink_send_out_t);
    Setting.SubChannel = db->sub_channel;
    Setting.Owner = MEMIO_OWNER_CVTASK;
    pSet = &(Setting);
    AmbaMisra_TypeCast(&Send.Settings, &pSet);
    Send.Result = NULL;
    Rval = AmbaTransfer_Send(db->sink_hdlr, &Send);
    if (Rval != TRANSFER_OK) {
         cvtask_printf(LVL_CRITICAL,"[data_sink] AmbaTransfer_Send fail", 0U, 0U, 0U, 0U, 0U);
         retcode = (uint32_t)CVTASK_ERR_GENERAL;
    }
    //cvtask_printf(LVL_DEBUG, "[data_sink]: send msg_va=0x%lx msg_pa=0x%x size=%d",
    //    env->vpInputBuffer[0], db->req.io[0].addr, db->req.io[0].size, 0U, 0U);
    //AmbaMisra_TypeCast(&pBBX, &env->vpInputBuffer[0]);
    //cvtask_printf(LVL_DEBUG, "[data_sink]: NumBbx = %d", pBBX->NumBbx, 0U, 0U, 0U, 0U);

    pOut = &db->out;
    AmbaMisra_TypeCast(&Recv.BufferAddr, &pOut);
    Recv.Size = sizeof(memio_sink_recv_act_t);
    Setting.SubChannel = db->sub_channel;
    Setting.Owner = MEMIO_OWNER_CVTASK;
    pSet = &(Setting);
    AmbaMisra_TypeCast(&Recv.Settings, &pSet);
    Recv.Result = NULL;
    Rval = AmbaTransfer_Receive(db->sink_hdlr, &Recv);
    if (Rval != TRANSFER_OK) {
         cvtask_printf(LVL_CRITICAL,"[data_sink] AmbaTransfer_Recv fail", 0U, 0U, 0U, 0U, 0U);
         retcode = (uint32_t)CVTASK_ERR_GENERAL;
    }
    //cvtask_printf(LVL_NORMAL,"[data_sink]: receive msg = 0x%x", db->out.seq_no, 0U, 0U, 0U, 0U);

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
    struct priv_database *db;
    memio_setting_t Setting;
    const memio_setting_t *pSet;

    AmbaMisra_TypeCast(&db, &env->vpInstance_private_storage);
    pSet = &(Setting);
    Setting.SubChannel = db->sub_channel;
    Setting.Owner = MEMIO_OWNER_CVTASK;
    AmbaMisra_TypeCast(&Disconf.Settings, &pSet);
    (void)AmbaTransfer_Disconnect(db->sink_hdlr, &Disconf);
    return CVTASK_ERR_OK;
}

uint32_t data_sink_create(void)
{
    static const cvtask_entry_t entry = {
        .cvtask_name     = TASKNAME_DATA_SINK,
        .cvtask_type     = (uint32_t)CVTASK_TYPE_ARM_BLOCKABLE,
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
cvtask_declare_create (data_sink_create);
#else
static  initcall_t __initcall_data_sink_create
            GNU_SECTION_CVTASK_CREATE_FP = data_sink_create;
#endif
