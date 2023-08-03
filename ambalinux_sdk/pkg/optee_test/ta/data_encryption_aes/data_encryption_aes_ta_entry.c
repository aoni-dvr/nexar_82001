
#include "tee_internal_api.h"
#include "tee_api_defines.h"
#include "trace.h"
#include "tee_api_defines_extensions.h"

#include "data_encryption_aes_ta_type.h"
#include "data_encryption_aes_ta_handle.h"

TEE_Result TA_CreateEntryPoint(void)
{
    DLOG("data_encryption_aes: TA_CreateEntryPoint\n");
    return TEE_SUCCESS;
}

TEE_Result TA_OpenSessionEntryPoint(uint32_t paramTypes, TEE_Param __maybe_unused params[4],
    void __maybe_unused **sessionContext)
{
    TEE_Result ret=TEE_SUCCESS;
    DLOG("data_encryption_aes: TA_OpenSessionEntryPoint\n");

    UNUSED(paramTypes);
    UNUSED(params);
    UNUSED(sessionContext);

    return ret;
}

void TA_CloseSessionEntryPoint(void __maybe_unused *session_context)
{
    DLOG("data_encryption_aes: TA_CloseSessionEntryPoint\n");
    UNUSED(session_context);
}

void TA_DestroyEntryPoint(void)
{
    DLOG("data_encryption_aes: TA_DestroyEntryPoint\n");
}

TEE_Result TA_InvokeCommandEntryPoint(void __maybe_unused *session_context,
    uint32_t cmd_id,
    uint32_t paramTypes, TEE_Param params[4])
{
    int ret = 0;

    DLOG("CMD_ID = %d\n", cmd_id);

    switch (cmd_id) {
        case CMD_AES:
            DLOG("Entry the aes operation!!!\n");
            ret = data_encryption_aes_ta_handle_aes(paramTypes, params);
            break;
        default:
            DLOG("unexpected cmd_id %d\n", cmd_id);
            return TEE_ERROR_BAD_PARAMETERS;
            break;
    }

    if (ret) {
        DLOG("handle fail, ret %d\n", ret);
        return TEE_ERROR_BAD_STATE;
    }

    return  TEE_SUCCESS;
}

