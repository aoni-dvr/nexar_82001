/*******************************************************************************
 * optee_tls_test_ta_hadle.h
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
#ifndef __OPTEE_TLS_TEST_TA_HANDLE_H__
#define __OPTEE_TLS_TEST_TA_HANDLE_H__

/*! @function optee_tls_ta_open
 *  @brief open a tls connection in tee
 *  @param session_context sessioncontext
 *  @param paramTypes paramTypes
 *  @param params, params
 *  @return 0 means success, otherwise means fail
 */
extern int optee_tls_ta_open(void *session_context, unsigned int paramTypes, TEE_Param params[4]);
/*! @function optee_tls_ta_close
 *  @brief close a tls connection
 *  @param session_context sessioncontext
 *  @param paramTypes paramTypes
 *  @param params, params
 *  @return 0 means success, otherwise means fail
 */
extern int optee_tls_ta_close(void *session_context, unsigned int paramTypes, TEE_Param params[4]);
/*! @function optee_tls_ta_write
 *  @brief write into the specified optee tls connection
 *  @param session_context sessioncontext
 *  @param paramTypes paramTypes
 *  @param params, params
 *  @return 0 means success, otherwise means fail
 */
extern int optee_tls_ta_write(void *session_context, unsigned int paramTypes, TEE_Param params[4]);
/*! @function optee_tls_ta_read
 *  @brief read from the specified optee tls connection
 *  @param session_context sessioncontext
 *  @param paramTypes paramTypes
 *  @param params, params
 *  @return 0 means success, otherwise means fail
 */
extern int optee_tls_ta_read(void *session_context, unsigned int paramTypes, TEE_Param params[4]);

#endif
