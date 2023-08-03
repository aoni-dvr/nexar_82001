/*******************************************************************************
 * optee_tls_ta_handle.c
 *
 * History:
 *  2021/01/05 - [Bo-Xi Chen] create file
 *
 * Copyright 2020 Ambarella International LP
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************************/
#include "tee_internal_api.h"
#include "tee_api_defines.h"
#include "tee_api_types.h"
#include "trace.h"
#include "tee_api_defines_extensions.h"
#include "string.h"
#include "stdio.h"

#include "mbedtls/net_sockets.h"
#include "mbedtls/ssl.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/error.h"
#include "mbedtls/certs.h"
#include "mbedtls/debug.h"

#include "optee_tls_test_ta_type.h"

#include "optee_tls_test_ta_handle.h"
#include "optee_tls_test_ta_socket_handle.h"

static void optee_tls_test_debug( void *ctx, int level,
                      const char *file, int line,
                      const char *str )
{
    ((void) ctx);
    ((void) level);
    printf("%s:%04d: %s\n", file, line, str );
}


static int f_rng(void *rng __unused, unsigned char *output, size_t output_len)
{
    TEE_GenerateRandom(output, output_len);
    return 0;
}

int optee_tls_ta_open(void *session_context, unsigned int paramTypes, TEE_Param params[4])
{
    int ret = 0;
    uint32_t flags;
    char *server_ip;
    int server_ip_len;
    int server_port;
    unsigned char *cert_file;
    unsigned int cert_len;
    const char *entropy = "optee_tls_client";
    char buf[128];
    UNUSED(paramTypes);

    server_ip = params[0].memref.buffer;
    server_ip_len = params[0].memref.size;
    server_port = params[1].value.a;
    cert_file = params[2].memref.buffer;
    cert_len = params[2].memref.size;
    mbedtls_debug_set_threshold(params[3].value.a);

    mbedtls_entropy_context* entropy_ctx;
    mbedtls_ctr_drbg_context* ctr_drbg;
    mbedtls_ssl_config* conf;
    mbedtls_x509_crt* crt;

    entropy_ctx = TEE_Malloc(sizeof(mbedtls_entropy_context), 0x0);
    conf = TEE_Malloc(sizeof(mbedtls_ssl_config), 0x0);
    crt = TEE_Malloc(sizeof(mbedtls_x509_crt), 0x0);
    ctr_drbg = TEE_Malloc(sizeof(mbedtls_ctr_drbg_context), 0x0);

    optee_tls_client_context* ctx = (optee_tls_client_context *)session_context;

    //initialize
    mbedtls_ssl_init(ctx->ssl);
    mbedtls_ssl_config_init(conf);
    mbedtls_x509_crt_init(crt);
    mbedtls_ctr_drbg_init(ctr_drbg);
    mbedtls_entropy_init(entropy_ctx);

    //seed RNG
    mbedtls_ssl_conf_dbg(conf, optee_tls_test_debug, NULL);

    ret = mbedtls_ctr_drbg_seed(ctr_drbg, f_rng, entropy_ctx,
                                   (const unsigned char *)entropy, strlen(entropy));
    if (ret) {
       DLOG("mbedtls_ctr_drbg_seed failed, ret:0x%x\n", -ret);
       goto tag_optee_tls_open_exit_2;
    }

    ret = mbedtls_x509_crt_parse(crt, cert_file, cert_len);
    if (ret) {
        DLOG("mbedtls_x509_crt_parse failed, ret:0x%x\n", -ret);
        goto tag_optee_tls_open_exit_2;
    }

    //start connection
    ret = optee_tls_ta_open_tcp(session_context, server_ip, server_ip_len, server_port);
    if (ret) {
        DLOG("mbedtls_net_connect failed, ret:0x%x\n", ret);
        goto tag_optee_tls_open_exit_2;
    }
    DLOG("tcp connection done.\n");

    //setup ssl config
    ret = mbedtls_ssl_config_defaults(conf,
                    MBEDTLS_SSL_IS_CLIENT,
                    MBEDTLS_SSL_TRANSPORT_STREAM,
                    MBEDTLS_SSL_PRESET_DEFAULT);
    if (ret) {
        DLOG("mbedtls_ssl_config_defaults failed, ret:0x%x\n", -ret);
        goto tag_optee_tls_open_exit_1;
    }

    mbedtls_ssl_conf_authmode(conf, MBEDTLS_SSL_VERIFY_OPTIONAL);
    mbedtls_ssl_conf_ca_chain(conf, crt, NULL);
    mbedtls_ssl_conf_rng(conf, f_rng, ctr_drbg);

    ret = mbedtls_ssl_setup(ctx->ssl, conf);
    if (ret) {
        DLOG("mbedtls_ssl_setup failed, ret:0x%0x\n", -ret);
        goto tag_optee_tls_open_exit_1;
    }

    mbedtls_ssl_set_bio(ctx->ssl, &ctx->socket_handle, optee_tls_ta_tcp_send,
        optee_tls_ta_tcp_recv, optee_tls_ta_tcp_recv_timeout);

    //handshake
    while( ( ret = mbedtls_ssl_handshake(ctx->ssl) ) != 0 ) {
        if(ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE) {
            DLOG("mbedtls_ssl_handshake failed, ret:0x%x\n", -ret);
            goto tag_optee_tls_open_exit_1;
        }
    }

    //verify server certificate
    flags = mbedtls_ssl_get_verify_result(ctx->ssl);
    if (flags) {
        ret = mbedtls_x509_crt_verify_info(buf, sizeof(buf), "!", flags);
        DLOG("verify failed, %s\n", buf);
        goto tag_optee_tls_open_exit_1;
    }

    DLOG("optee_tls_ta_open success.\n");

    return 0;

tag_optee_tls_open_exit_1:
    socket_handle_close(session_context);

tag_optee_tls_open_exit_2:
    mbedtls_ssl_free(ctx->ssl);

    return ret;
}


int optee_tls_ta_close(void *session_context, unsigned int paramTypes, TEE_Param params[4])
{
    optee_tls_client_context *ctx = (optee_tls_client_context *)session_context;
    UNUSED(paramTypes);
    UNUSED(params);

    mbedtls_ssl_close_notify(ctx->ssl);
    socket_handle_close(session_context);
    mbedtls_ssl_free(ctx->ssl);

    return 0;
}

int optee_tls_ta_write(void *session_context, unsigned int paramTypes, TEE_Param params[4])
{
    optee_tls_client_context *ctx = (optee_tls_client_context *)session_context;

    unsigned char *buf = NULL;
    int buf_len = 0, write_len = 0, ret = 0;
    UNUSED(paramTypes);

    buf = params[0].memref.buffer;
    buf_len = params[0].memref.size;

    while ((ret = mbedtls_ssl_write(ctx->ssl, buf, buf_len)) <= 0) {
        if (ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE) {
            DLOG("mbedtls_ssl_write failed, ret:0x%x\n", -ret);
            return (ret);
        }
    }

    write_len = ret;
    //DLOG(" %d bytes write to server\n%s\n", write_len, (char *) buf);

    params[1].value.a = write_len;

    return 0;
}

int optee_tls_ta_read(void *session_context, unsigned int paramTypes, TEE_Param params[4])
{
    optee_tls_client_context *ctx = (optee_tls_client_context *)session_context;

    unsigned char *out_buf = NULL;
    int read_len = 0, num = 0, ret = 0;
    UNUSED(paramTypes);

    out_buf = params[0].memref.buffer;
    num = params[1].value.a;

    do {
        ret = mbedtls_ssl_read(ctx->ssl, out_buf, num);

        if(ret == MBEDTLS_ERR_SSL_WANT_READ || ret == MBEDTLS_ERR_SSL_WANT_WRITE)
            continue;

        if(ret == MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY)
            break;

        if (ret < 0) {
            DLOG("mbedtls_ssl_read failed, ret:0x%x\n", -ret);
            break;
        }

        if (ret == 0) {
            DLOG("\n\nEOF\n\n");
            break;
        }

        read_len = ret;
        //DLOG(" %d bytes read\n%s\n", read_len, out_buf);
    } while (1);

    params[2].value.a = read_len;

    return 0;
}
