

#ifndef MOUDLE_DATA_ENCRYPTION_AES_TA_H_
#define MOUDLE_DATA_ENCRYPTION_AES_TA_H_

typedef struct
{
    char* in_buf;
    char* out_buf;
    char* key;
    char* iv;
    unsigned int data_len;
    unsigned int key_len;
    unsigned int iv_len;
    unsigned int algorithm_id;
    TEE_OperationMode mode;
} aes_operation_t;

extern int do_data_encryption_aes_ta(aes_operation_t *op);

extern int data_encryption_aes_ta_sha256_oper(char* input, unsigned int inlen, char* output);

#endif

