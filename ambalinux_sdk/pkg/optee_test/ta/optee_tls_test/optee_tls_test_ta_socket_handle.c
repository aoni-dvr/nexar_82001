/*******************************************************************************
 * optee_tls_test_ta_socket_handle.c
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

#include <string.h>
#include "optee_tls_test_ta_type.h"
#include "optee_tls_test_ta_socket_handle.h"

TEE_UUID uuid_pta_socket = PTA_SOCKET_UUID;

static TEE_Result socket_handle_init(void *session) {

    optee_tls_client_context* ctx;
    optee_tls_socket_handle* sock_handle;
    ctx = (optee_tls_client_context*) session;
    sock_handle = ctx->socket_handle;

    sock_handle->socket_handle = 0;
    TEE_Result res;
    uint32_t err_origin;

    res = TEE_OpenTASession(&uuid_pta_socket, TEE_TIMEOUT_INFINITE,
                    0, NULL, &sock_handle->session, &err_origin);

    if (res != TEE_SUCCESS)
    {
        DLOG("socket handle init TEE_OpenTASession fail, ret ");
        return res;
    }

    return TEE_SUCCESS;
}


static TEE_Result socket_handle_open(void *session, unsigned char * server, size_t server_len, uint32_t port)
{
    optee_tls_client_context* ctx;
    optee_tls_socket_handle* sock_handle;
    ctx = (optee_tls_client_context*) session;
    sock_handle = ctx->socket_handle;

    TEE_Result res;
    uint32_t err_origin;

    uint32_t ptypes = TEE_PARAM_TYPES(TEE_PARAM_TYPE_VALUE_INPUT,
                        TEE_PARAM_TYPE_MEMREF_INPUT,
                        TEE_PARAM_TYPE_VALUE_INPUT,
                        TEE_PARAM_TYPE_VALUE_OUTPUT);

    TEE_Param op[4];
    memset(&op, 0, sizeof(op));
    op[0].value.a = TEE_IP_VERSION_4;
    op[0].value.b = port;
    op[1].memref.buffer = server;
    op[1].memref.size = server_len;
    op[2].value.a = TEE_ISOCKET_PROTOCOLID_TCP;


    res = TEE_InvokeTACommand(sock_handle->session, TEE_TIMEOUT_INFINITE,
        PTA_SOCKET_OPEN, ptypes, op, &err_origin);

    sock_handle->socket_handle = op[3].value.a;

    return res;
}

TEE_Result socket_handle_close(void *session)
{
    optee_tls_client_context* ctx;
    optee_tls_socket_handle* sock_handle;
    ctx = (optee_tls_client_context*) session;
    sock_handle = ctx->socket_handle;

    TEE_Result res;
    uint32_t err_origin;


    uint32_t ptypes = TEE_PARAM_TYPES(TEE_PARAM_TYPE_VALUE_INPUT,
                        TEE_PARAM_TYPE_NONE,
                        TEE_PARAM_TYPE_NONE,
                        TEE_PARAM_TYPE_NONE);

    TEE_Param op[4];
    memset(&op, 0, sizeof(op));
    op[0].value.a = sock_handle->socket_handle;

    res = TEE_InvokeTACommand(sock_handle->session, TEE_TIMEOUT_INFINITE,
                PTA_SOCKET_CLOSE, ptypes, op, &err_origin);

    if(res != TEE_SUCCESS) {
        DLOG("socket handle close fail.\n");
        return res;
    }

    TEE_CloseTASession(sock_handle->session);

    return res;
}

TEE_Result optee_tls_ta_open_tcp(void* session, const char * server_addr, int server_addr_size, const int port)
{
    TEE_Result ret;

    ret = socket_handle_init(session);
    if(ret != TEE_SUCCESS) {
        DLOG("init socket handle failed.\n");
        return ret;
    }

    unsigned char buff[200];
    memset(buff, 0x0, 200);
    memcpy(buff, server_addr, server_addr_size);
    //DLOG("buff, addr_size, port: %s, %d, %d\n", buff, server_addr_size, port);
    ret = socket_handle_open(session, (unsigned char *) buff, server_addr_size, port);

    return ret;
}

int optee_tls_ta_tcp_send(void * session, const unsigned char * buf, size_t len)
{
    optee_tls_client_context* ctx;
    optee_tls_socket_handle* sock_handle;
    ctx = (optee_tls_client_context*) session;
    sock_handle = ctx->socket_handle;

    TEE_Result err;
    uint32_t err_origin;

    TEE_Param op[4];
    memset(&op, 0, sizeof(op));
    op[0].value.a = sock_handle->socket_handle;
    op[0].value.b = TEE_TIMEOUT_INFINITE;
    op[1].memref.buffer = (unsigned char *) buf;
    op[1].memref.size = len;

    uint32_t ptypes = TEE_PARAM_TYPES(TEE_PARAM_TYPE_VALUE_INPUT,
                        TEE_PARAM_TYPE_MEMREF_INPUT,
                        TEE_PARAM_TYPE_VALUE_OUTPUT,
                        TEE_PARAM_TYPE_NONE);

    err = TEE_InvokeTACommand(sock_handle->session, TEE_TIMEOUT_INFINITE,
        PTA_SOCKET_SEND,
        ptypes,
        op, &err_origin);

    if (err != TEE_SUCCESS)
    {
        DLOG("optee_tls_ta_tcp_send fail, ret: 0x%x\n", err);
        return err;
    }
    return op[2].value.a;
}

int optee_tls_ta_tcp_recv_timeout(void * session, unsigned char * buf, size_t len,  uint32_t timeout)
{
    optee_tls_client_context* ctx;
    optee_tls_socket_handle* sock_handle;
    ctx = (optee_tls_client_context*) session;
    sock_handle = ctx->socket_handle;


    TEE_Result err;
    uint32_t err_origin;

    if(!timeout)
        timeout = TEE_TIMEOUT_INFINITE;

    TEE_Param op[4];
    memset(&op, 0, sizeof(op));

    op[0].value.a = sock_handle->socket_handle;
    op[0].value.b = timeout;
    op[1].memref.buffer = buf;
    op[1].memref.size = len;

    uint32_t ptypes = TEE_PARAM_TYPES(TEE_PARAM_TYPE_VALUE_INPUT,
                        TEE_PARAM_TYPE_MEMREF_OUTPUT,
                        TEE_PARAM_TYPE_NONE,
                        TEE_PARAM_TYPE_NONE);

    err = TEE_InvokeTACommand(sock_handle->session, TEE_TIMEOUT_INFINITE, PTA_SOCKET_RECV,
        ptypes, op, &err_origin);

    if (err != TEE_SUCCESS)
    {
        DLOG("optee_tls_ta_tcp_recv_timeout fail, ret: 0x%x\n", err);
        return err;
    }

    return op[1].memref.size;
}


int optee_tls_ta_tcp_recv(void * session, unsigned char * buf, size_t len)
{
    return optee_tls_ta_tcp_recv_timeout( session, buf, len, TEE_TIMEOUT_INFINITE);

}

