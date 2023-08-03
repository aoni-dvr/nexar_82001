
#ifndef MOUDLE_DATA_ENCRYPTION_AES_CA_H_
#define MOUDLE_DATA_ENCRYPTION_AES_CA_H_

extern void print_memory(char *p, int size);

extern TEEC_Result do_aes(char *p_data, unsigned int len, E_AES_MODE mode, unsigned int aes_len,
                          char *password, unsigned int password_len, E_AES_OPERATION op, char *output);

#endif

