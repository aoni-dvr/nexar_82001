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
#include "schdr_api.h"


#ifndef CONFIG_CV_CONFIG_TX_SDK7
#define AmbaMisra_TypeCast    AmbaMisra_TypeCast64
#else
#define AmbaMisra_TypeCast    AmbaMisra_TypeCast32
#endif
#define TASKNAME_DATA_TRIGGER	"DATA_TRIGGER_TASK"

extern void cvtask_printf(int lvl, const char *fmt,
    uint32_t v0, uint32_t v1, uint32_t v2, uint32_t v3, uint32_t v4);
extern int console_printS5(const char *fmt,
    const char *argv0, const char *argv1, const char *argv2, const char *argv3, const char *argv4);
extern uint32_t data_trigger_create(void);

struct priv_data_trigger {
    uint32_t feeder_uuid;
};


static uint32_t query(uint32_t iid, const uint32_t *config,
    cvtask_memory_interface_t *dst)
{
    char label[32] = "DATA_TRIGGER_##_IONAME_###";
    const void *ioname;
    const char *str;
    uint32_t str_size, res, retcode = CVTASK_ERR_OK;

    (void) iid;
    cvtask_printf(LVL_VERBOSE, "query cvtask [DATA_TRIGGER_TASK]", 0, 0, 0, 0, 0);

    memset(dst, 0, sizeof(*dst));

    dst->Instance_private_storage_needed = sizeof(struct priv_data_trigger);
    dst->num_outputs = 0U;

    label[13] = '0' + ((config[2] / 10U) % 10U);
    label[14] = '0' + ((config[2]      ) % 10U);

    label[23] = '0' + ((config[0] / 100U) % 10U);
    label[24] = '0' + ((config[0] /  10U) % 10U);
    label[25] = '0' + ((config[0]       ) % 10U);
    res = cvtable_find(label, &ioname, &str_size);
    AmbaMisra_TypeCast(&str, &ioname);
    dst->num_inputs = 1U;
    if (res != (uint32_t)CVTASK_ERR_OK) {
        memcpy(dst->input[0].io_name, "DUMMY", 6U);
        dst->input[0].dependency_only = 1;
    } else {
        memcpy(dst->input[0].io_name, str, str_size);
    }
    return retcode;
}

static uint32_t init(const cvtask_parameter_interface_t *env,
    const uint32_t *config)
{
    uint32_t retcode;
    struct priv_data_trigger *db;

    (void) config;
    cvtask_printf(LVL_VERBOSE, "init cvtask [DATA_TRIGGER_TASK]", 0, 0, 0, 0, 0);
    AmbaMisra_TypeCast(&db, &env->vpInstance_private_storage);
    memset(db, 0x0, sizeof(struct priv_data_trigger));
    db->feeder_uuid = config[1];

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
    uint32_t retcode;
    const struct priv_data_trigger *db;


    AmbaMisra_TypeCast(&db, &env->vpInstance_private_storage);
    (void) schdr_wake_feeders(0, db->feeder_uuid);
    //cvtask_printf(LVL_DEBUG, "[data_trigger] wake feeder (%d) ", db->feeder_uuid, 0, 0, 0, 0);
    retcode = (uint32_t)CVTASK_ERR_OK;
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
    (void) env;
    return CVTASK_ERR_OK;
}

uint32_t data_trigger_create(void)
{
    static const cvtask_entry_t entry = {
        .cvtask_name     = TASKNAME_DATA_TRIGGER,
        .cvtask_type     = (uint32_t)CVTASK_TYPE_ARM,
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
cvtask_declare_create (data_trigger_create);
#else
static  initcall_t __initcall_data_trigger_create
            GNU_SECTION_CVTASK_CREATE_FP = data_trigger_create;
#endif
