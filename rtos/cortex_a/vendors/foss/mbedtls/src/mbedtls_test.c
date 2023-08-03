
// Here are wrappers for standard headers.
#include "AmbaTypes.h"

#include <AmbaMisraFix.h>
#include <comsvc/shell/inc/AmbaShell.h>
#include <comsvc/misc/AmbaUtility.h>
#include <AmbaPrint.h>
#include <string.h>

#include "mbedtls/md.h"
#include "mbedtls/pkcs5.h"
#include "memory_buffer_alloc.h"
#include "sha256.h"
#include "rsa.h"

#if 0
/** [WPA-PSK] Key = PBKDF2(HMAC-SHA1, passphrase, ssid, 4096, 256)
 *  input: prf=MBEDTLS_MD_SHA1, password, ssid, iter_c=4096, key_le=256, Read RFC2898 for detail
 *  output:64-digits PSK
 */
static int wifi_PBKDF2(int prf, char* pw, char* salt, int iter_c, int key_le, char *psk_str)
{
    int i, rval;
    unsigned char out[key_le/8];
    char buf[4];
    mbedtls_md_context_t md_ctx;
    const mbedtls_md_info_t* md_info;
    UINT32 u;

    if ((md_info = mbedtls_md_info_from_type(prf)) == NULL) {
        AmbaPrint_PrintStr5("mbedtls_md_info_from_type() fail", NULL, NULL, NULL, NULL, NULL);
        return -1;
    }

    mbedtls_md_init(&md_ctx);
    if ((rval = mbedtls_md_setup(&md_ctx, md_info, 1)) != OK) {
        AmbaPrint_PrintStr5("mbedtls_md_setup() fail", NULL, NULL, NULL, NULL, NULL);
        return -1;
    }
    if ((rval = mbedtls_pkcs5_pbkdf2_hmac(&md_ctx, (UINT8 *)pw, strlen(pw),
                (UINT8 *)salt, strlen(salt), iter_c, key_le/8, out)) != OK) {
        AmbaPrint_PrintStr5("mbedtls_pkcs5_pbkdf2_hmac() fail", NULL, NULL, NULL, NULL, NULL);
        return -1;
    }

    psk_str[0] = '\0';
    for (i = 0; i < (key_le/8); i++) {
        u = (UINT32)out[i];
        (void)AmbaUtility_StringPrintUInt32(&(buf[0]), 4, "%02x", 1, &u);
        AmbaUtility_StringAppend(psk_str, 68, buf);
    }
    mbedtls_md_free(&md_ctx);

    AmbaPrint_PrintStr5("%s", psk_str, NULL, NULL, NULL, NULL);

    return OK;
}

void mbedtls_shellmain(UINT32 argc, char * const * argv, AMBA_SHELL_PRINT_f PrintFunc)
{
    char passkey[68];
    unsigned char memory_buf[100000];
    INT32 v;

    (void)argc;
    (void)argv;
    (void)PrintFunc;

    mbedtls_memory_buffer_alloc_init( memory_buf, sizeof(memory_buf) );

    AmbaPrint_PrintStr5("[WiFi WPA2-PSK-CCMP]", NULL, NULL, NULL, NULL, NULL);
    AmbaPrint_PrintStr5("  Calculate PSK(Pre-Shared Key) for SSID:ccyao password:1234567890", NULL, NULL, NULL, NULL, NULL);
    wifi_PBKDF2(MBEDTLS_MD_SHA1, "1234567890", "ccyao", 4096, 256, passkey);
    (void)AmbaWrap_memcmp(passkey, "DF27B5A728E534E14BAAA73BB12465D0DCC488DC61B1E6DB4F0A1E405881C790", 64, &v);
    if (v == 0) {
        AmbaPrint_PrintStr5("  Test Result: PASS", NULL, NULL, NULL, NULL, NULL);
    } else {
        AmbaPrint_PrintStr5("  Test Result: NG", NULL, NULL, NULL, NULL, NULL);
    }
}

static AMBA_SHELL_COMMAND_s mbedtls_commands = {
    .pName = "mbedtls",
    .MainFunc = mbedtls_shellmain,
    .pNext = NULL
};

void AmbaShell_mbedtls(void)
{
    AmbaShell_CommandRegister(&mbedtls_commands);
}
#endif

UINT32 AmbaRsaSigVerify_mbedtls(UINT8 *pData, UINT32 DataSize, const char *p_rsa_n, const char *p_rsa_e)
{
    #define SHA256_LEN              (32)
    #define MBEDTLS_BUFFER_SIZE     (0x2000U)   /* 8KB */
    UINT32 RetVal = 1;
    unsigned char SHA256Sum[SHA256_LEN];    /* Also plaintext for rsa */

    unsigned char *pSig;

    static UINT8 mbedtls_internal_buffer[MBEDTLS_BUFFER_SIZE];

    /* mbedtls working buffer */
    mbedtls_memory_buffer_alloc_init( mbedtls_internal_buffer, sizeof(mbedtls_internal_buffer) );

    {   /* SHA256 processing */
        UINT32 Rval = OK;
        mbedtls_sha256_context ctx;

        mbedtls_sha256_init( &ctx );
        Rval = mbedtls_sha256_starts_ret( &ctx, 0 );
        if (OK == Rval) {
            Rval = mbedtls_sha256_update_ret( &ctx, pData, DataSize - 256 );
            if (OK == Rval) {
                Rval = mbedtls_sha256_finish_ret( &ctx, SHA256Sum );
                if (OK == Rval) {
                    RetVal = OK;
                }
            }
        }
    }

    if (OK == RetVal) { /* Read public key and verify */
        UINT32 Rval = OK;
        mbedtls_rsa_context rsa;
        mbedtls_mpi N, E;

        RetVal = 1;
        pSig = &pData[DataSize - 256];

        mbedtls_rsa_init( &rsa, MBEDTLS_RSA_PKCS_V15, 0 );

        mbedtls_mpi_init( &N );
        mbedtls_mpi_init( &E );

        if (OK != mbedtls_mpi_read_string( &N, 16, p_rsa_n )) {
            Rval = 1U;
        }

        if (OK != mbedtls_mpi_read_string( &E, 16, p_rsa_e )) {
            Rval = 1U;
        }

        if (OK == Rval) {
            if (OK != mbedtls_rsa_import( &rsa, &N, NULL, NULL, NULL, &E )) {
                Rval = 1U;
            }
        }

        rsa.len = ( mbedtls_mpi_bitlen( &rsa.N ) + 7 ) >> 3;
        if (OK == Rval) {
            if (OK != mbedtls_rsa_pkcs1_verify( &rsa, NULL, NULL, MBEDTLS_RSA_PUBLIC, MBEDTLS_MD_SHA256, 20, SHA256Sum, pSig )) {
                Rval = 1U;
            }
        }

        if (OK == Rval) {
            RetVal = OK;
        }

    }

    return RetVal;
}