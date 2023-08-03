/*******************************************************************************
 * optee_tls_test_client.c
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
#include "stdlib.h"
#include "stdio.h"
#include "string.h"

#include "tee_client_api.h"

#include "optee_tls_test_ca.h"

#define DEBUG_LEVEL 0

int main(int argc, char *argv[])
{
    char buf[128], out_buf[256];
    int len = 0, ret = 0;

    char *host_name = NULL, *cert_file_path = NULL, *cert_file = NULL;
    int portnum = 0, cert_len = 0;
    optee_tls_context *ctx = NULL;

    if (argc != 4) {
        printf("usage: %s <hostname> <portnum> <ca cert file>\n", argv[0]);
        return (-1);
    }

    host_name = argv[1];
    portnum = atoi(argv[2]);
    cert_file_path = argv[3];

    //get cert
    cert_file = optee_tls_read_cert(cert_file_path, &cert_len);
    //printf("cert %s\n, len %d\n", cert_file, cert_len);
    if (cert_file == NULL) {
        printf("cert not existed, check the certificate file.\n");
        return (-1);
    }

    //init ctx
    ctx = optee_tls_initialize(DEBUG_LEVEL);

    //open tls connection
    optee_tls_open(ctx, host_name, portnum , cert_file, cert_len);
    free(cert_file);

    //send operation
    memset(buf, 0x0, 128);
    len = sprintf((char *) buf, "test optee tls client");
    ret = optee_tls_write(ctx, buf, len);
    if (ret < 0) {
        printf("optee_tls_write fail, ret: %d\n", ret);
    } else {
        printf("%d message %s have been written.\n", ret, buf);
    }

    //read operation
    ret = optee_tls_read(ctx, out_buf, sizeof(out_buf), 256);
    if (ret < 0) {
        printf("optee_tls_read fail, ret: %d\n", ret);
    } else {
        printf("recv from server:\n %s\n len %d\n", out_buf, ret);
    }

    optee_tls_close(ctx);

    return 0;
}

