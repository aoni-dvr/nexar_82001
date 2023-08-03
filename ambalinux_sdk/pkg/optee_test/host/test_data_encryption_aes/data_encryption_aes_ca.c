
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include "tee_client_api.h"
#include <unistd.h>

#include "data_encryption_aes_ta_type.h"

#include "data_encryption_aes_ca.h"

void print_memory(char *p, int size)
{
    while (size > 7) {
        printf("%02x %02x %02x %02x %02x %02x %02x %02x\n",
               p[0], p[1], p[2], p[3],
               p[4], p[5], p[6], p[7]);
        p += 8;
        size -= 8;
    }

    while (size) {
        printf("%02x ", p[0]);
        p ++;
        size --;
    }

    printf("\n");
}

TEEC_Result do_aes(char *p_data, unsigned int len, E_AES_MODE mode, unsigned int aes_len,
                   char *password, unsigned int password_len, E_AES_OPERATION op, char *output)
{
    TEEC_Context context;
    TEEC_Session session;
    TEEC_Operation operation;
    TEEC_Result result;
    TEEC_UUID svc_id = DATA_ENCRYPTION_AES_UUID_ID;
    uint32_t origin;
    unsigned int input_info[3];

    input_info[0] = mode;
    input_info[1] = op;
    input_info[2] = aes_len;

    result = TEEC_InitializeContext(NULL, &context);
    if (result != TEEC_SUCCESS) {
        printf("TEEC_InitializeContext fail, result=0x%x\n", result);
        return result;
    }

    result = TEEC_OpenSession(&context, &session, &svc_id,
                              TEEC_LOGIN_PUBLIC, NULL, NULL, &origin);
    if (result != TEEC_SUCCESS) {
        printf("TEEC_OpenSession failed, result=0x%x, origin=0x%x\n", result, origin);
        goto tag_exit_do_aes_1;
    }

    memset(&operation, 0x0, sizeof(operation));
    operation.started = 1;
    operation.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INPUT, TEEC_MEMREF_TEMP_INPUT,
                                            TEEC_MEMREF_TEMP_OUTPUT, TEEC_MEMREF_TEMP_INPUT);
    operation.params[0].tmpref.size = sizeof(input_info);
    operation.params[0].tmpref.buffer = input_info;
    operation.params[1].tmpref.size = len;
    operation.params[1].tmpref.buffer = p_data;
    operation.params[2].tmpref.size = len;
    operation.params[2].tmpref.buffer = output;
    operation.params[3].tmpref.buffer = password;
    operation.params[3].tmpref.size = password_len;

    result = TEEC_InvokeCommand(&session, CMD_AES, &operation, &origin);
    if (result != TEEC_SUCCESS) {
        printf("TEEC_InvokeCommand fail, result=0x%x, origin=0x%x\n", result, origin);
        goto tag_exit_do_aes_2;
    }

tag_exit_do_aes_2:
    TEEC_CloseSession(&session);
tag_exit_do_aes_1:
    TEEC_FinalizeContext(&context);

    return result;
}



