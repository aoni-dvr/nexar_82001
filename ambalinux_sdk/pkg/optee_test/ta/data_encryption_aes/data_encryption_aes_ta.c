
#include "tee_internal_api.h"
#include "tee_api_defines.h"
#include "trace.h"
#include "tee_api_defines_extensions.h"

#include "data_encryption_aes_ta_type.h"
#include "data_encryption_aes_ta.h"

int data_encryption_aes_ta_sha256_oper(char* input, unsigned int inlen, char* output)
{
    TEE_Result result = 0;
    TEE_OperationHandle operationhandle;
    int Retval = 0;
    unsigned int outlen = 32;

    result = TEE_AllocateOperation(&operationhandle, TEE_ALG_SHA256, TEE_MODE_DIGEST, 0);
    if(result != TEE_SUCCESS)
    {
        DLOG("Allocate SHA operation handle fail\n");
        Retval = -1;
        goto cleanup_1;
    }

    TEE_DigestUpdate(operationhandle, input, inlen);

    result = TEE_DigestDoFinal(operationhandle, NULL, 0, output, &outlen);
    //DLOG("The output length is :%d\n", outlen);
    //DLOG("The return value is :0x%x\n", result);
    if(result != TEE_SUCCESS)
    {
        DLOG("Do the final sha operation fail\n");
        Retval = -1;
        goto cleanup_2;
    }

    cleanup_2:
        TEE_FreeOperation(operationhandle);
    cleanup_1:
        return Retval;
}

int do_data_encryption_aes_ta(aes_operation_t *op)
{
    TEE_OperationHandle operation = NULL;
    TEE_ObjectHandle key_obj = NULL;
    TEE_Attribute attr;
    char* in_buf = op->in_buf;
    char* out_buf = op->out_buf;
    unsigned int data_len = op->data_len;
    TEE_Result result = 0;


    /* Allocate the operation handle */
    result = TEE_AllocateOperation(&operation, op->algorithm_id, op->mode, op->key_len);
    if (TEE_SUCCESS != result) {
        DLOG("TEE_AllocateOperation fail, result 0x%x\n", result);
        goto tag_exit_do_data_encryption_aes_ta_1;
    }

    /* Allocate the object handle */
    result = TEE_AllocateTransientObject(TEE_TYPE_AES, op->key_len, &key_obj);
    if (TEE_SUCCESS != result) {
        DLOG("TEE_AllocateTransientObject fail, result 0x%x\n", result);
        goto tag_exit_do_data_encryption_aes_ta_1;
    }

    /* Set the key object parameter */
    TEE_InitRefAttribute(&attr, TEE_ATTR_SECRET_VALUE, op->key, op->key_len / 8);
    result = TEE_PopulateTransientObject(key_obj, &attr, 1);
    if (TEE_SUCCESS != result) {
        DLOG("TEE_PopulateTransientObject fail, result 0x%x\n", result);
        goto tag_exit_do_data_encryption_aes_ta_1;
    }

    /* Assemble aes operation handle */
    result = TEE_SetOperationKey(operation, key_obj);
    if (TEE_SUCCESS != result) {
        DLOG("TEE_SetOperationKey fail, result 0x%x\n", result);
        goto tag_exit_do_data_encryption_aes_ta_2;
    }

    /* Initialze cipher operation */
    TEE_CipherInit(operation, op->iv, op->iv_len);

    /* Do the final AES operation */
    result = TEE_CipherDoFinal(operation, in_buf, data_len, out_buf, &data_len);
    if (TEE_SUCCESS != result) {
        DLOG("TEE_SetOperationKey fail, result 0x%x\n", result);
        goto tag_exit_do_data_encryption_aes_ta_2;
    }

tag_exit_do_data_encryption_aes_ta_2:
    TEE_FreeOperation(operation);
tag_exit_do_data_encryption_aes_ta_1:
    return result;
}

