#include <app_base.h>
#include "sha256_helper.h"
#if defined(CONFIG_BUILD_FOSS_WOLFSSL)
#include <wolfssl/openssl/sha.h>
#include <wolfssl/openssl/pem.h>
#include <wolfssl/openssl/crypto.h>
#endif

int key_manager_check_key_enabled(unsigned char *buffer)
{
    int i = 0;

    for (i = 0; i < 32; i++) {
        if (buffer[i] != 0x00) {
            return 1;
        }
    }
    return 0;
}

// for ecc
int key_manager_list_ecc_keys(ecc_sha256_items_s *keys)
{
    int i = 0, j = 0;
    metadata_s *meta_data = NULL;
    unsigned char sha256_buf[32] = {0};

    if (keys == NULL) {
        debug_line("%s invalid param", __func__);
        return -1;
    }
    metadata_get(&meta_data);
    for (i = 0; i < ECC_MAX_KEY_NUM; i++) {
        if (key_manager_check_key_enabled(meta_data->EccKeys[i].Key)) {
            if (sha256_buffer(meta_data->EccKeys[i].Key, ECC_KEY_LEN, sha256_buf) < 0) {
                continue;
            }
            keys->item[j].Index = i;
            memcpy(keys->item[j].Key, sha256_buf, sizeof(sha256_buf));
            j += 1;
        }
    }
    keys->num = j;

    return 0;
}

int key_manager_write_ecc_key(unsigned char *key, unsigned int key_len, unsigned char index)
{
    metadata_s *meta_data = NULL;

    if (key == NULL || key_len != ECC_KEY_LEN || index >= ECC_MAX_KEY_NUM) {
        debug_line("%s invalid param", __func__);
        return -1;
    }
    metadata_get(&meta_data);
    if (key_manager_check_key_enabled(meta_data->EccKeys[index].Key)) {
        debug_line("%s index:%d already used", __func__, index);
        return -1;
    }
    if (metadata_write_otp_key(RTOS_AMBA_SIP_OTP_SET_ECC_KEY, key, key_len, (int)index) < 0) {
        debug_line("%s failed", __func__);
        return -1;
    }
    memset(meta_data->EccKeys[index].Key, 0, sizeof(meta_data->EccKeys[index].Key));
    memcpy(meta_data->EccKeys[index].Key, key, key_len);

    return 0;
}

#define ECC_CURVE_SZ 32 /* SECP256R1 curve size in bytes */
#define ECC_CURVE_ID ECC_SECP256R1
//https://github.com/wolfSSL/wolfssl-examples/blob/master/pk/ecc/ecc_pub.c
int key_manager_export_ecc_public_key(unsigned char *key, unsigned int *key_len, unsigned char index)
{
#if defined(CONFIG_BUILD_FOSS_WOLFSSL)
    metadata_s *meta_data = NULL;
    int ret = 0;
    ecc_key ecc;
    uint32_t pubQxSz = ECC_CURVE_SZ, pubQySz = ECC_CURVE_SZ;

    if (key == NULL || key_len == NULL || index >= ECC_MAX_KEY_NUM) {
        debug_line("%s invalid param", __func__);
        return -1;
    }
    metadata_get(&meta_data);
    if (key_manager_check_key_enabled(meta_data->EccKeys[index].Key) == 0) {
        debug_line("%s index:%d no data", __func__, index);
        return -1;
    }
    /* Setup the ECC key */
    ret = wc_ecc_init(&ecc);
    if (ret < 0) {
        debug_line("%s wc_ecc_init failed", __func__, index);
        return -1;
    }
    /* Import private key "k" */
    ret = wc_ecc_import_private_key_ex(meta_data->EccKeys[index].Key, ECC_KEY_LEN, /* private key "d" */
                                        NULL, 0,                    /* public (optional) */
                                        &ecc,
                                        ECC_CURVE_ID);
    /* Export public key */
    if (ret == 0) {
        ret = wc_ecc_make_pub(&ecc, NULL);
    }
    if (ret == 0) {
        ret = wc_ecc_export_public_raw(&ecc,
                                        key, &pubQxSz,               /* public Qx */
                                        key + ECC_CURVE_SZ, &pubQySz);   /* public Qy */
    }
    wc_ecc_free(&ecc);

    if (ret == 0) {
        *key_len = pubQxSz + pubQySz;
    }

    return (ret == 0) ? 0 : -1;
#else
    return -1;
#endif
}

#if defined(CONFIG_BUILD_FOSS_WOLFSSL)
//https://github.com/wolfSSL/wolfssl-examples/blob/master/pk/ecc/ecc_sign.c
static int crypto_ecc_sign(const uint8_t *key, uint32_t keySz, const uint8_t *hash, uint32_t hashSz, uint8_t *sig, uint32_t* sigSz, int curveSz, int curveId)
{
    int ret;
    mp_int r, s;
    ecc_key ecc;
    WC_RNG rng;

    /* validate arguments */
    if (key == NULL || hash == NULL || sig == NULL || sigSz == NULL ||
        curveSz == 0 || hashSz == 0 || keySz < (uint32_t)curveSz || *sigSz < ((uint32_t)curveSz * 2)) {
        return BAD_FUNC_ARG;
    }

    /* Initialize signature result */
    memset(sig, 0, curveSz*2);

    /* Setup the RNG */
    ret = wc_InitRng(&rng);
    if (ret < 0) {
        return ret;
    }

    /* Setup the ECC key */
    ret = wc_ecc_init(&ecc);
    if (ret < 0) {
        wc_FreeRng(&rng);
        return ret;
    }

    /* Setup the signature r/s variables */
    ret = mp_init(&r);
    if (ret != MP_OKAY) {
        wc_ecc_free(&ecc);
        wc_FreeRng(&rng);
        return ret;
    }
    ret = mp_init(&s);
    if (ret != MP_OKAY) {
        mp_clear(&r);
        wc_ecc_free(&ecc);
        wc_FreeRng(&rng);
        return ret;
    }

    /* Import private key "k" */
    ret = wc_ecc_import_private_key_ex(
        key, keySz, /* private key "d" */
        NULL, 0,    /* public (optional) */
        &ecc,
        curveId     /* ECC Curve Id */
    );
    if (ret == 0) {
        /* Verify ECC Signature */
        ret = wc_ecc_sign_hash_ex(
            hash, hashSz, /* computed hash digest */
            &rng, &ecc,   /* random and key context */
            &r, &s        /* r/s as mp_int */
        );

        /* export r/s */
        //mp_to_unsigned_bin(&r, sig);
        //mp_to_unsigned_bin(&s, sig + curveSz);
        mp_to_unsigned_bin(&r, sig + (32 - mp_unsigned_bin_size(&r)));
        mp_to_unsigned_bin(&s, sig + curveSz + (32 - mp_unsigned_bin_size(&s)));
    }
    mp_clear(&r);
    mp_clear(&s);
    wc_ecc_free(&ecc);
    wc_FreeRng(&rng);

    return ret;
}
#endif

int crypto_ecc_verify(const uint8_t *key, uint32_t keySz,
    const uint8_t *hash, uint32_t hashSz, const uint8_t *sig, uint32_t sigSz,
    int curveSz, int curveId)
{
#if defined(CONFIG_BUILD_FOSS_WOLFSSL)
    int ret, verify_res = 0;
    mp_int r, s;
    ecc_key ecc;

    /* validate arguments */
    if (key == NULL || hash == NULL || sig == NULL || curveSz == 0 || 
        hashSz == 0 || keySz < (uint32_t)(curveSz*2) || sigSz < (uint32_t)(curveSz*2))
    {
        return BAD_FUNC_ARG;
    }

    /* Setup the ECC key */
    ret = wc_ecc_init(&ecc);
    if (ret < 0) {
        return ret;
    }

    /* Setup the signature r/s variables */
    ret = mp_init(&r);
    if (ret != MP_OKAY) {
        wc_ecc_free(&ecc);
        return ret;
    }
    ret = mp_init(&s);
    if (ret != MP_OKAY) {
        mp_clear(&r);
        wc_ecc_free(&ecc);
        return ret;
    }

    /* Import public key x/y */
    ret = wc_ecc_import_unsigned(
        &ecc, 
        (byte*)key,             /* Public "x" Coordinate */
        (byte*)(key + curveSz), /* Public "y" Coordinate */
        NULL,                   /* Private "d" (optional) */
        curveId                 /* ECC Curve Id */
    );
    /* Make sure it was a public key imported */
    if (ret == 0 && ecc.type != ECC_PUBLICKEY) {
        ret = ECC_BAD_ARG_E;
    }

    /* Import signature r/s */
    if (ret == 0) {
        ret = mp_read_unsigned_bin(&r, sig,  curveSz);
    }
    if (ret == 0) {
        ret = mp_read_unsigned_bin(&s, sig + curveSz, curveSz);
    }

    /* Verify ECC Signature */
    if (ret == 0) {
        ret = wc_ecc_verify_hash_ex(
            &r, &s,       /* r/s as mp_int */
            hash, hashSz, /* computed hash digest */
            &verify_res,  /* verification result 1=success */
            &ecc
        );
    }
    
    /* check verify result */
    if (ret == 0 && verify_res == 0) {
        ret = SIG_VERIFY_E;
    }

    mp_clear(&r);
    mp_clear(&s);
    wc_ecc_free(&ecc);

    return ret;
#else
    return -1;
#endif
}

int key_manager_sign_message(unsigned char *hash, unsigned int hash_len, unsigned char *sign, unsigned int *sign_size, unsigned char index)
{
#if defined(CONFIG_BUILD_FOSS_WOLFSSL)
//#define ENABLE_KEY_DEBUG
//#define ENABLE_KEY_VERIFY
#if defined(ENABLE_KEY_DEBUG)
    int i = 0;
    char tmp[128] = {0};
    static unsigned int sign_times = 0;
#endif
#if defined(ENABLE_KEY_VERIFY)
    unsigned char pub_key[64] = {0};
    unsigned int pub_key_len = 0;
#endif
    metadata_s *meta_data = NULL;

    if (hash == NULL || hash_len == 0 || sign == NULL || sign_size == NULL || index >= ECC_MAX_KEY_NUM) {
        debug_line("%s invalid param", __func__);
        return -1;
    }
    metadata_get(&meta_data);
    if (key_manager_check_key_enabled(meta_data->EccKeys[index].Key) == 0) {
        debug_line("%s index:%d no data", __func__, index);
        return -1;
    }
#if defined(ENABLE_KEY_DEBUG)
    debug_line("");
    sign_times += 1;
    debug_line("sign times:%d key index: %d", sign_times, index);
    memset(tmp, 0, sizeof(tmp));
    for (i = 0; i < 32; i++) {
        snprintf(tmp + strlen(tmp), 3, "%.2x", meta_data->EccKeys[index].Key[i]);
    }
    debug_line("ecc key: %s", tmp);
    memset(tmp, 0, sizeof(tmp));
    for (i = 0; i < 32; i++) {
        snprintf(tmp + strlen(tmp), 3, "%.2x", hash[i]);
    }
    debug_line("hash: %s", tmp);
#endif

    if (crypto_ecc_sign(meta_data->EccKeys[index].Key, ECC_KEY_LEN,
                        hash, hash_len,
                        sign, sign_size,
                        ECC_CURVE_SZ, ECC_CURVE_ID) < 0) {
        debug_line("%s ecc sign failed", __func__);
        return -1;
    }
    *sign_size = ECC_CURVE_SZ * 2;
#if defined(ENABLE_KEY_DEBUG)
    memset(tmp, 0, sizeof(tmp));
    for (i = 0; i < 32; i++) {
        snprintf(tmp + strlen(tmp), 3, "%.2x", sign[i]);
    }
    debug_line("wolfssl_sigr: %s", tmp);
    memset(tmp, 0, sizeof(tmp));
    for (i = 32; i < 64; i++) {
        snprintf(tmp + strlen(tmp), 3, "%.2x", sign[i]);
    }
    debug_line("wolfssl_sigs: %s", tmp);
#endif

#if defined(ENABLE_KEY_VERIFY)
    if (key_manager_export_ecc_public_key(pub_key, &pub_key_len, index) < 0) {
        debug_line("%s export ecc pub_key failed", __func__);
        return -1;
    }
    if (crypto_ecc_verify(pub_key, pub_key_len, hash, hash_len, sign, *sign_size, ECC_CURVE_SZ, ECC_CURVE_ID) < 0) {
        debug_line("%s ecc verify failed------------------------------------------------------------------------------", __func__);
        return -1;
    }
#if defined(ENABLE_KEY_DEBUG)
    debug_line("%s verify success", __func__);
#endif
#endif

    return 0;
#else
    return -1;
#endif
}

// for aes
int key_manager_list_aes_keys(aes_sha256_items_s *keys)
{
    int i = 0, j = 0;
    metadata_s *meta_data = NULL;
    unsigned char sha256_buf[32] = {0};

    if (keys == NULL) {
        debug_line("%s invalid param", __func__);
        return -1;
    }
    metadata_get(&meta_data);
    for (i = 0; i < AES_MAX_KEY_NUM; i++) {
        if (key_manager_check_key_enabled(meta_data->AesKeys[i].Key)) {
            if (sha256_buffer(meta_data->AesKeys[i].Key, AES_KEY_LEN, sha256_buf) < 0) {
                debug_line("%s index=%d sha256 get failed", __func__, i);
                continue;
            }
            keys->item[j].Index = i;
            memcpy(keys->item[j].Key, sha256_buf, sizeof(sha256_buf));
            j += 1;
        }
    }
    keys->num = j;

    return 0;
}

int key_manager_write_aes_key(unsigned char *key, unsigned int key_len, unsigned char index)
{
    metadata_s *meta_data = NULL;

    if (key == NULL || key_len <= 0 || key_len > AES_KEY_LEN || index >= AES_MAX_KEY_NUM) {
        debug_line("%s invalid param", __func__);
        return -1;
    }
    metadata_get(&meta_data);
    if (key_manager_check_key_enabled(meta_data->AesKeys[index].Key)) {
        debug_line("%s index:%d already used", __func__, index);
        return -1;
    }
    if (metadata_write_otp_key(RTOS_AMBA_SIP_OTP_SET_AES_KEY, key, key_len, (int)index) < 0) {
        debug_line("%s failed", __func__);
        return -1;
    }
    memset(meta_data->AesKeys[index].Key, 0, sizeof(meta_data->AesKeys[index].Key));
    memcpy(meta_data->AesKeys[index].Key, key, key_len);

    return 0;
}

int key_manager_read_aes_key(unsigned char *key, unsigned int *key_len, unsigned char index)
{
    metadata_s *meta_data = NULL;

    if (key == NULL || index >= AES_MAX_KEY_NUM) {
        debug_line("%s invalid param", __func__);
        return -1;
    }
    metadata_get(&meta_data);
    if (key_manager_check_key_enabled(meta_data->AesKeys[index].Key) == 0) {
        debug_line("%s index:%d no data", __func__, index);
        return -1;
    }
    memcpy(key, meta_data->AesKeys[index].Key, AES_KEY_LEN);
    if (key_len != NULL) {
        *key_len = AES_KEY_LEN;
    }
    return 0;
}

