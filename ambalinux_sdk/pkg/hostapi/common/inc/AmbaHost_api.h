#ifndef __AMBA_HOST_API__
#define __AMBA_HOST_API__

#include <stdint.h>

#define AMBA_HOST_ERR_OK                 0
#define AMBA_HOST_ERR_TASK_DONE          1
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

#ifdef __cplusplus
extern "C" {
#endif

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
 * Callback function to print output received from target
 * @arg:        passed from user
 * @string:     output strings
 *              connection is closed if @string=NULL
 * @size:       size fo the output
 *
 */
typedef void (*printer_func2)(void *arg, const char *string, int size);

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
 * Execute a cmd on target, version2
 * @cid:        connnection ID
 * @printer:    printer to handle target output, @cid is passed to it as pid
 *              can be set to NULL if we don't care about output
 * @arg:        argument passed through to printer function
 * @cmd:        command to be executed on target
 */
 int AmbaHost_exec2(int cid, printer_func2 printer, void *arg,const char *cmd);

/*
 * Get the address of a connection
 * @cid:        connnection ID
 * @addr:       caller-provided buffer
 * @max_size:   size of the buffer
 */
int AmbaHost_get_addr(int cid, char *addr, int max_size);

/*
 * Initialize an interactive shell on target
 * @cid:        connection ID
 * @flags:      not used now
 *
 * return a positive unique ID number that identify the shell
 */
int AmbaHost_shell_init(int cid,  uint32_t flags);

/*
 * Shut down the interactive shell on target
 * @sid:        shell ID
 */
int AmbaHost_shell_fini(int sid);

/*
 * open the shell connection
 * @sid:        shell ID
 * @printer:    printer to handle shell output, @sid is passed to it as pid
 *              can be set to NULL if we don't care about output
 *
 * If @printer is NULL, returns a file decriptor from which caller can read
 * the incoming log from target
 */
int AmbaHost_shell_open(int sid,  printer_func printer);

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

#ifdef __cplusplus
}
#endif

#endif  //__AMBA_HOST_API__
