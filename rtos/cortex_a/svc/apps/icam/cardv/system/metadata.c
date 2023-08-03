#include <app_base.h>
#include "AmbaIntrinsics.h"
#include "AmbaCache.h"
#include "AmbaATF.h"

static unsigned char metadata_buf[METADATA_LEN];
static metadata_s *metadata = NULL;

int metadata_read_cpu_id(unsigned char *buf, int buf_len)
{
    unsigned int ret = 0;
    UINT8 key[16] __attribute__((aligned(4096))) = {0};

    if (buf == NULL || buf_len <= 0 || buf_len > 16) {
        debug_line("%s invalid param. buf_len=%d", __func__, buf_len);
        return -1;
    }
    memset((void *)key, 0, sizeof(key));
    (void)AmbaCache_DataClean((UINT32)key, sizeof(key));
    ret = AmbaSmcCall(SMC32_AMBA_SIP_OTP_GET_UNIQUE_ID, (UINT32)key, buf_len, 0, 0);
    (void)AmbaCache_DataInvalidate((UINT32)key, sizeof(key));
    if (ret == 0) {
        memcpy(buf, key, buf_len);
    }

    return (ret == 0) ? 0 : -1;
}

int metadata_read_otp_key(unsigned int cmd, unsigned char *buf, int buf_len, int index)
{
    unsigned int ret = 0;
    UINT8 key[512] __attribute__((aligned(4096))) = {0};

    if (buf == NULL || buf_len <= 0 || buf_len > 512 || index >= 4) {
        debug_line("%s invalid param. buf_len=%d, index=%d", __func__, buf_len, index);
        return -1;
    }
    memset((void *)key, 0, sizeof(key));
    (void)AmbaCache_DataClean((UINT32)key, sizeof(key));
    ret = AmbaSmcCall(cmd, (UINT32)key, buf_len, index, 0);
    (void)AmbaCache_DataInvalidate((UINT32)key, sizeof(key));
    if (ret == 0) {
        memcpy(buf, key, buf_len);
    }

    return (ret == 0) ? 0 : -1;
}

int metadata_write_otp_key(unsigned int cmd, unsigned char *buf, int buf_len, int index)
{
    unsigned int ret = 0;
    UINT8 key[512] __attribute__((aligned(4096))) = {0};
    UINT8 tmp_key[512] = {0};

    if (buf == NULL || buf_len <= 0 || buf_len > 512 || index >= 4) {        
        debug_line("%s invalid param. buf_len=%d, index=%d", __func__, buf_len, index);
        return -1;
    }
    memset((void *)tmp_key, 0, sizeof(tmp_key));
    memset((void *)key, 0, sizeof(key));
    memcpy((void *)key, buf, buf_len);
    (void)AmbaCache_DataClean((UINT32)key, sizeof(key));
    ret = AmbaSmcCall(cmd, (UINT32)key, buf_len, index, 0);
    if (ret != 0) {
        debug_line("otp write failed, try read and compare");
    }
    if (metadata_read_otp_key(cmd - 1, tmp_key, buf_len, index) < 0) {        
        debug_line("otp write readback failed");
        return -1;
    }
    if (memcmp(key, tmp_key, buf_len) != 0) {
        debug_line("otp write compare failed");
        return -1;
    }

    return 0;
}

int metadata_enable_otp_secure_boot(void)
{
    return (AmbaSmcCall(RTOS_AMBA_SIP_OTP_PERMANENTLY_ENABLE_SECURE_BOOT, 0U, 0U, 0U, 0U) == 0) ? 0 : -1;
}

//#define ECC_KEY_DEBUG
#ifdef ECC_KEY_DEBUG
static const uint8_t kPrivKey[] = {
    /* d */
    0x4c, 0x21, 0x15, 0x42, 0xfc, 0xf9, 0xe4, 0x9b, 
    0x1b, 0xb4, 0x90, 0xaf, 0xfe, 0x58, 0xdf, 0xe5, 
    0x0e, 0x95, 0xd5, 0xce, 0xf4, 0xf7, 0xaa, 0xbd, 
    0x90, 0xb5, 0xc3, 0xab, 0x09, 0x70, 0x64, 0x31
};
#endif

int metadata_load(void)
{
    unsigned char key[32] = {0};
    int i = 0;

    memset(metadata_buf, 0, sizeof(metadata_buf));
    metadata = (metadata_s *)metadata_buf;
    // read sn
    memset(metadata->SerialNumber, 0, sizeof(metadata->SerialNumber));
    metadata_read_otp_key(RTOS_AMBA_SIP_OTP_GET_CUSTOMER_ID, metadata->SerialNumber, 16, 0);
    if (!((metadata->SerialNumber[0] >= '0' && metadata->SerialNumber[0] <= '9')
        || (metadata->SerialNumber[0] >= 'a' && metadata->SerialNumber[0] <= 'z')
        || (metadata->SerialNumber[0] >= 'A' && metadata->SerialNumber[0] <= 'Z'))) {
        memset(metadata->SerialNumber, 0, sizeof(metadata->SerialNumber));
    }
    // read cpu_id
    memset(key, 0, sizeof(key));
    metadata_read_cpu_id(key, 16);
    memset(metadata->CpuId, 0, sizeof(metadata->CpuId));
    for (i = 0; i < 16; i++) {
        snprintf((char *)(metadata->CpuId + i * 2), 3, "%.2x", key[i]);
    }
#if 1
    memset(key, 0, sizeof(key));
    memset(metadata->WifiMac, 0, sizeof(metadata->WifiMac));
    memset(metadata->BtMac, 0, sizeof(metadata->BtMac));
    metadata_read_otp_key(RTOS_AMBA_SIP_OTP_GET_AES_KEY, key, 32, 0);
    memcpy(metadata->WifiMac, key, 6);
    if (metadata->WifiMac[0] % 2 != 0) {
        metadata->WifiMac[0] += 1;
    }
    memcpy(metadata->BtMac, key + 6, 6);
#else
    for (i = 0; i < ECC_MAX_KEY_NUM; i++) {
        memset(metadata->EccKeys[i].Key, 0, sizeof(metadata->EccKeys[i].Key));
        metadata_read_otp_key(RTOS_AMBA_SIP_OTP_GET_ECC_KEY, metadata->EccKeys[i].Key, ECC_KEY_LEN, i);        
#ifdef ECC_KEY_DEBUG
        memcpy(metadata->EccKeys[i].Key, kPrivKey, sizeof(kPrivKey));
#endif
    }
    for (i = 0; i < AES_MAX_KEY_NUM; i++) {
        memset(metadata->AesKeys[i].Key, 0, sizeof(metadata->AesKeys[i].Key));        
        metadata_read_otp_key(RTOS_AMBA_SIP_OTP_GET_AES_KEY, metadata->AesKeys[i].Key, AES_KEY_LEN, i);
    }
#endif
    metadata_dump();

    return 0;
}

int metadata_dump(void)
{
    char line[64] = {0};
    int i = 0;

    if (metadata == NULL) {
        debug_line("metadata not loaded");
        return -1;
    }
    debug_line("metadata information:");
    debug_line("       cpu id: %s", (char *)metadata->CpuId);
    debug_line("   serial num: %s", (char *)metadata->SerialNumber);
    memset(line, 0, sizeof(line));
    for (i = 0; i < 6; i++) {
        snprintf(line + i * 2, 3, "%.2X", metadata->WifiMac[i]);
    }
    debug_line("    wifi1 mac: %s", line);
    memset(line, 0, sizeof(line));
    for (i = 0; i < 6; i++) {
        snprintf(line + i * 2, 3, "%.2X", metadata->BtMac[i]);
    }
    debug_line("       bt mac: %s", line);

    return 0;
}

int metadata_get(metadata_s **data)
{
    if (data == NULL) {
        debug_line("metadata not loaded");
        return -1;
    }
    *data = metadata;

    return 0;
}

