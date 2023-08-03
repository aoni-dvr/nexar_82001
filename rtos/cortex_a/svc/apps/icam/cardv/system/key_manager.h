#ifndef __KEYS_MANAGER_H__
#define __KEYS_MANAGER_H__

typedef struct _key_sha256_item_s_ {
    unsigned char Index;
    unsigned char Key[32];
} key_sha256_item_s;

typedef struct _ecc_sha256_items_s_ {
    unsigned char num;
    key_sha256_item_s item[ECC_MAX_KEY_NUM];
} ecc_sha256_items_s;

typedef struct _aes_sha256_items_s_ {
    unsigned char num;
    key_sha256_item_s item[AES_MAX_KEY_NUM];
} aes_sha256_items_s;

int key_manager_check_key_enabled(unsigned char *buffer);
int key_manager_list_ecc_keys(ecc_sha256_items_s *keys);
int key_manager_write_ecc_key(unsigned char *key, unsigned int key_len, unsigned char index);
int key_manager_export_ecc_public_key(unsigned char *key, unsigned int *key_len, unsigned char index);
int key_manager_sign_message(unsigned char *hash, unsigned int hash_len, unsigned char *sign, unsigned int *sign_size, unsigned char index);
int key_manager_list_aes_keys(aes_sha256_items_s *keys);
int key_manager_write_aes_key(unsigned char *key, unsigned int key_len, unsigned char index);
int key_manager_read_aes_key(unsigned char *key, unsigned int *key_len, unsigned char index);

#endif//__KEYS_MANAGER_H__

