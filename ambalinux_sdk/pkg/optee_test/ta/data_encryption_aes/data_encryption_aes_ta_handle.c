
#include "tee_internal_api.h"
#include "tee_api_defines.h"
#include "tee_api_types.h"
#include "trace.h"
#include "tee_api_defines_extensions.h"

#include "data_encryption_aes_ta_type.h"
#include "data_encryption_aes_ta.h"

#include "data_encryption_aes_ta_handle.h"

static void __set_aes_iv(aes_operation_t *op)
{
    char buf[256];
    data_encryption_aes_ta_sha256_oper(op->key, op->key_len, buf);
    op->iv = buf;
    op->iv_len = 16;
}

static void __set_info(aes_operation_t *op, aes_operation_info_t *info)
{
    switch (info->action) {
        case E_AES_OP_ENCRYPT:
            op->mode = TEE_MODE_ENCRYPT;
            break;
        case E_AES_OP_DECRYPT:
            op->mode = TEE_MODE_DECRYPT;
            break;
        default:
            DLOG("invalid action %d\n", info->action);
            break;
    }

    switch (info->mode) {
        case E_AES_MODE_CBC:
            op->algorithm_id = TEE_ALG_AES_CBC_NOPAD;
            break;
        case E_AES_MODE_ECB:
            op->algorithm_id = TEE_ALG_AES_ECB_NOPAD;
            break;
        case E_AES_MODE_CTR:
            op->algorithm_id = TEE_ALG_AES_CTR;
            break;
        case E_AES_MODE_CTS:
            op->algorithm_id = TEE_ALG_AES_CTS;
            break;
        default:
            DLOG("invalid mode %d\n", info->mode);
            break;
    }
}


int data_encryption_aes_ta_handle_aes(unsigned int paramTypes, TEE_Param params[4])
{
    aes_operation_t op;
    aes_operation_info_t info;
    char* password;
    //char data_len_buf[16] = {0};
    unsigned int password_len;
    unsigned int *buf;
    char test[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06};
    UNUSED(paramTypes);

    DLOG("Start to do AES operation\n");

    /**1) Get the aes mode, input data info & output info */
    buf = params[0].memref.buffer;
    info.mode = (E_AES_MODE)buf[0];
    info.action = (E_AES_OPERATION)buf[1];
    op.key_len = buf[2];
    op.in_buf = params[1].memref.buffer;
    op.data_len = params[1].memref.size;
    op.out_buf = params[2].memref.buffer;
    password = params[3].memref.buffer;
    password_len = params[3].memref.size;


    TEE_MemMove(op.out_buf, test, sizeof(test));
    op.key = TEE_Malloc(256, 0);

    data_encryption_aes_ta_sha256_oper(password, password_len, op.key);

    __set_aes_iv(&op);
    __set_info(&op, &info);

    DLOG("ID: 0x%x, mode: 0x%x\n", op.algorithm_id, op.mode);

    /**4) Do AES operation */

    do_data_encryption_aes_ta(&op);

    return 0;
}

