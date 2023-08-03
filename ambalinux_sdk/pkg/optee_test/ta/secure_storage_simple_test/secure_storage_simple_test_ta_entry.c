/*
 * Copyright (C) 2018 Ambarella Inc.
 * All rights reserved.
 *
 * Author: Bo-xi Chen <bxchen@ambarella.com>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "tee_internal_api.h"
#include "tee_api_defines.h"
#include "trace.h"
#include "tee_api_defines_extensions.h"
#include "string.h"

#include "secure_storage_simple_test_ta_type.h"

#include "secure_storage_simple_test_ta_handle.h"

TEE_Result TA_CreateEntryPoint(void)
{
    //DLOG("Sec storage task test TA_CreateEntryPoint \n");

    return TEE_SUCCESS;
}

TEE_Result TA_OpenSessionEntryPoint(uint32_t paramTypes, TEE_Param __maybe_unused params[4],
                void __maybe_unused **sessionContext)
{
    TEE_Result ret=TEE_SUCCESS;
    //DLOG("Sec storage task test TA_OpenSessionEntryPoint\n");

    UNUSED(paramTypes);
    UNUSED(params);
    UNUSED(sessionContext);

    return ret;
}

void TA_CloseSessionEntryPoint(void __maybe_unused *session_context)
{
    //DLOG("Sec storage task test TA_CloseSessionEntryPoint\n");
    UNUSED(session_context);
}


void TA_DestroyEntryPoint(void)
{
    //DLOG("Sec storage task test TA_DestroyEntryPoint\n");
}


TEE_Result TA_InvokeCommandEntryPoint(void __maybe_unused *session_context,
                uint32_t cmd_id,
                uint32_t paramTypes, TEE_Param params[4])
{
    TEE_Result ret = TEE_SUCCESS;
    UNUSED(paramTypes);

    //DLOG("CMD_ID = %d\n", cmd_id);

    switch (cmd_id) {
        case CMD_CREATE_OPER:
            ret = secure_storage_simple_test_ta_create(paramTypes, params);
            break;
        case CMD_READ_OPER:
            ret = secure_storage_simple_test_ta_load(paramTypes, params);
            break;
        case CMD_WRITE_OPER:
            ret = secure_storage_simple_test_ta_store(paramTypes, params);
            break;
        case CMD_DELETE_OPER:
            ret = secure_storage_simple_test_ta_delete(paramTypes, params);
            break;
        case CMD_READ_TEST_OPER:
            ret = secure_storage_simple_test_ta_load_2buffer(paramTypes, params);
            break;
        default:
            ret = TEE_EXEC_FAIL;
            break;
    }

    return  ret;
}
