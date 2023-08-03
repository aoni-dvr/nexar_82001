/**
*  @file AmbaHost_api.h
*
* Copyright (c) [2020] Ambarella International LP
*
* This file and its contents ("Software") are protected by intellectual
* property rights including, without limitation, U.S. and/or foreign
* copyrights. This Software is also the confidential and proprietary
* information of Ambarella International LP and its licensors. You may not use, reproduce,
* disclose, distribute, modify, or otherwise prepare derivative works of this
* Software or any portion thereof except pursuant to a signed license agreement
* or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
* In the absence of such an agreement, you agree to promptly notify and return
* this Software to Ambarella International LP.
*
* This file includes sample code and is only for internal testing and evaluation.  If you
* distribute this sample code (whether in source, object, or binary code form), it will be
* without any warranty or indemnity protection from Ambarella International LP or its affiliates.
*
* THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
* INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
* MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*
*  @detail xxxx
*
*/
#ifndef __AMBA_HOST_API__
#define __AMBA_HOST_API__

#include <stdint.h>

#define AMBA_HOST_ERR_OK                0
#define AMBA_HOST_ERR_GENERAL           -1
#define AMBA_HOST_ERR_NETWORK_SEND      -2
#define AMBA_HOST_ERR_NETWORK_RECV      -3
#define AMBA_HOST_ERR_INVALID_CID       -4
#define AMBA_HOST_ERR_NO_CONNECTION     -5
#define AMBA_HOST_ERR_NO_SHELL          -6
#define AMBA_HOST_ERR_UNKNOWN_TYPE      -7
#define AMBA_HOST_ERR_INVALID_ADDR      -8
#define AMBA_HOST_ERR_CONNECTION_MAXOUT -9
#define AMBA_FILENAME_TOO_LONG          -10

#define HOSTAPI_LIBRARY_PRINTER_ID      0


/*
 * Callback function to print output received from target
 * @pid:        printer ID
 * @string:     output strings
 *              connection is closed if @string=NULL
 * @size:       size fo the output
 *
 */
typedef void (*printer_func)(int pid, const char *string, int size);

/*
 * Initialize AmbaHost module
 * @printer:    printer to handle the output from hostapi library
 */
int AmbaHost_init(uint32_t flags, printer_func printer);

/*
 * Shut down AmbaHost module and free all of it resources
 */
int AmbaHost_fini(void);

/*
 * Open a connection to a target
 * @addr:       address of the target
 *
 * return a positive unique ID number that identify the connection
 */
int AmbaHost_connection_open(const char *addr, uint32_t flags);

/*
 * Close the connection
 * @cid:        connection ID
 */
int AmbaHost_connection_close(int cid);

/*
 * loopback test
 * @cid:        connection ID
 * @message:    the message to be looped back by the connection
 */
int AmbaHost_loopback(int cid, const char *message);

/*
 * Read from target memory
 * @cid:        connection ID
 * @hAddr:      host memory address to receive the values
 * @tpAddr:     target physical memory address
 * @size:       the size of memory to be read
 */
int AmbaHost_read_mem(int cid, void *hAddr, uint32_t tpAddr, uint32_t size);

/*
 * Write to target memory
 * @cid:        connection ID
 * @hAddr:      host memory address to provide the values
 * @tpAddr:     target physical memory address
 * @size:       the size of memory to be written
 */
int AmbaHost_write_mem(int cid, void *hAddr, uint32_t tpAddr, uint32_t size);

/*
 * Copy file From target to host
 * @cid:        connection ID
 * @dst:        destination file name on host
 * @src:        source file name on target
 */
int AmbaHost_pull_file(int cid, const char *dst, const char *src);

/*
 * Copy file From host to target
 * @cid:        connection ID
 * @dst:        destination file name on target
 * @src:        source file name on host
 */
int AmbaHost_push_file(int cid, const char *dst, const char *src);

/*
 * Execute a cmd on target
 * @cid:        connnection ID
 * @printer:    printer to handle target output, @cid is passed to it as pid
 *              can be set to NULL if we don't care about output
 * @cmd:        command to be executed on target
 */
int AmbaHost_exec(int cid, printer_func printer, const char *cmd);

/*
 * Get the address of a connection
 * @cid:        connnection ID
 * @addr:       pointer to put address pointer
 */
int AmbaHost_get_addr(int cid, const char **addr);

/*
 * Open a interactive shell on target
 * @cid:        connection ID
 * @printer:    printer to handle shell output, @sid is passed to it as pid
 *              can be set to NULL if we don't care about output
 *
 * return a positive unique ID number that identify the shell
 */
int AmbaHost_shell_open(int cid,  printer_func printer, uint32_t flags);

/*
 * Close the shell
 * @sid:        shell ID
 */
int AmbaHost_shell_close(int sid);

/*
 * Send a sequence of chars to target
 * @sid:        shell ID
 * @chars:      char sequence
 * @size:       size of the sequence
 */
int AmbaHost_shell_sendstring(int sid, const char *chars, int size);

#endif  //__AMBA_HOST_API__
