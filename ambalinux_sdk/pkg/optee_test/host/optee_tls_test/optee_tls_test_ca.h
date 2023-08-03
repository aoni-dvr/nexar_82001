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

#ifndef __OPTEE_TLS_TEST_CA_H__
#define __OPTEE_TLS_TEST_CA_H__

typedef struct {
    TEEC_Context *ctx;
    TEEC_Session *sess;
    int debug_level;
}optee_tls_context;

/*! @function optee_tls_read_cert
 *  @brief read a certificate in filesystem
 *  @param path certificate path
 *  @param size certificate size
 *  @return certificate, retrun NULL means read fail
 */
extern char* optee_tls_read_cert(const char *path, int *size);
/*! @function optee_tls_initialize
 *  @brief init an optee_tls_context
 *  @param debug_level debug_level for ta
 *  @return an optee_tls_context, return NULL means initialize fail
 */
extern optee_tls_context* optee_tls_initialize(int debug_level);
/*! @function optee_tls_open
 *  @brief open a tls connection in tee
 *  @param optee_tls_ctx optee_tls_context
 *  @param server_ip server address
 *  @param server_port server port
 *  @param cert_file cert file
 *  @param cert_len cert length
 *  @return TEEC_SUCCESS means success, otherwise means read fail
 */
extern TEEC_Result optee_tls_open(optee_tls_context* optee_tls_ctx, char *server_ip, int server_port, char *cert_file, int cert_len);
/*! @function optee_tls_close
 *  @brief close tls connection in tee
 *  @param optee_tls_ctx optee_tls_context
 *  @return TEEC_SUCCESS means success, otherwise means read fail
 */
extern TEEC_Result optee_tls_close(optee_tls_context* optee_tls_ctx);
/*! @function optee_tls_write
 *  @brief writes num bytes from the buffer buf into the specified optee tls connection
 *  @param optee_tls_ctx optee_tls_context
 *  @param buf buf
 *  @param num the number of the bytes
 *  @return the number of bytes actually written to the connection, return value < 0 means write operation fail
 */
extern int optee_tls_write(optee_tls_context* optee_tls_ctx, char *buf, int num);
/*! @function optee_tls_read
 *  @brief read num bytes into the buffer buf from the specified optee tls connection
 *  @param optee_tls_ctx optee_tls_context
 *  @param out_buf out buf
 *  @param buf_len out buf len
 *  @param num the number of the bytes
 *  @return the number of bytes actually read from the connection, return value < 0 means read operation fail
 */
extern int optee_tls_read(optee_tls_context* optee_tls_ctx, char *out_buf, int buf_len, int num);

#endif
