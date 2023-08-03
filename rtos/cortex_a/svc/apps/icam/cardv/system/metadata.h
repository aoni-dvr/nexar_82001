#ifndef __METADATA_H__
#define __METADATA_H__

#define METADATA_LEN           (4096)

#define ECC_MAX_KEY_NUM (4)
#define ECC_KEY_LEN (32)
typedef struct _ecc_key_s_ {
    unsigned char Key[ECC_KEY_LEN];
} ecc_key_s;

#define AES_MAX_KEY_NUM (4)
#define AES_KEY_LEN (32)
typedef struct _aes_key_s_ {
    unsigned char Key[AES_KEY_LEN];
} aes_key_s;

typedef struct _metadata_s_ {
    unsigned char SerialNumber[32];
    unsigned char CpuId[64];
    ecc_key_s EccKeys[ECC_MAX_KEY_NUM];
    aes_key_s AesKeys[AES_MAX_KEY_NUM];
    unsigned char WifiMac[6];
    unsigned char BtMac[6];
    unsigned char Reserved[METADATA_LEN - 32 - 64 - 6 - 6 - sizeof(aes_key_s) * AES_MAX_KEY_NUM - sizeof(ecc_key_s) * ECC_MAX_KEY_NUM];
} metadata_s;

#define RTOS_AMBA_SIP_OTP_SET_CUSTOMER_ID 0x82000506
#define RTOS_AMBA_SIP_OTP_GET_CUSTOMER_ID 0x82000505
#define RTOS_AMBA_SIP_OTP_SET_AES_KEY 0x82000524
#define RTOS_AMBA_SIP_OTP_GET_AES_KEY 0x82000523
#define RTOS_AMBA_SIP_OTP_SET_ECC_KEY 0x82000526
#define RTOS_AMBA_SIP_OTP_GET_ECC_KEY 0x82000525
#define RTOS_AMBA_SIP_OTP_SET_SECURE_BOOT_PUBLIC_KEY 0x82000504U
#define RTOS_AMBA_SIP_OTP_GET_SECURE_BOOT_PUBLIC_KEY 0x82000503U
#define RTOS_AMBA_SIP_OTP_PERMANENTLY_ENABLE_SECURE_BOOT 0x82000509U

int metadata_load(void);
int metadata_dump(void);
int metadata_get(metadata_s **data);
int metadata_read_otp_key(unsigned int cmd, unsigned char *buf, int buf_len, int index);
int metadata_write_otp_key(unsigned int cmd, unsigned char *buf, int buf_len, int index);
int metadata_enable_otp_secure_boot(void);
int metadata_read_cpu_id(unsigned char *buf, int buf_len);

#endif//__METADATA_H__

