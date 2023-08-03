/*******************************************************************************
 * optee_tls_ta_socket_handle.h
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
#ifndef __OPTEE_TLS_TEST_TA_SOCKET_HANDLE_H__
#define __OPTEE_TLS_TEST_TA_SOCKET_HANDLE_H__

/*! @function optee_tls_ta_open_tcp
 *  @brief open a tcp connection
 *  @param session_context sessioncontext
 *  @param server_addr server address
 *  @param server_addr_size length of server address
 *  @param port port
 *  @return TEE_SUCCESS means success, otherwise means fail
 */
extern TEE_Result optee_tls_ta_open_tcp(void* session_context, const char * server_addr, int server_addr_size, const int port);
/*! @function socket_handle_close
 *  @brief close a tcp connection
 *  @param session_context session
 *  @return TEE_SUCCESS means success, otherwise means fail
 */
extern TEE_Result socket_handle_close(void *session);
/*! @function optee_tls_ta_tcp_send
 *  @brief send message into tcp connections
 *  @param buf buf
 *  @param len buf size
 *  @return the number of bytes actually written to the connection, return value < 0 means write operation fail
 */
extern int optee_tls_ta_tcp_send(void * session, const unsigned char * buf, size_t len);
/*! @function optee_tls_ta_tcp_recv_timeout
 *  @brief receive message from tcp connections
 *  @param buf buf
 *  @param len receive messge size
 *  @param timeout timeout ms
 *  @return the number of bytes actually read from the connection, return value < 0 means read operation fail
 */
extern int optee_tls_ta_tcp_recv_timeout(void * session, unsigned char * buf, size_t len,  uint32_t timeout);
/*! @function optee_tls_ta_tcp_recv
 *  @brief receive message from tcp connections
 *  @param buf buf
 *  @param len receive messge size
 *  @return the number of bytes actually read from the connection, return value < 0 means read operation fail
 */
extern int optee_tls_ta_tcp_recv(void * session, unsigned char * buf, size_t len);

#endif
