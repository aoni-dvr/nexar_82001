#ifndef __AMBA_TEST_MSG__
#define __AMBA_TEST_MSG__

#include <stdint.h>
#define MAX_FILENAME_SIZE      256
#define MAX_LOGSIZE            (64*1024)

#define HEARTBEAT_KEEPALIVE    0
#define HEARTBEAT_SHUTDOWN     1

#define AT_ERR_OK              0
#define AT_ERR_WRONG_STATUS   -256
#define AT_ERR_REQ_INVALID    -267
#define AT_ERR_DEV_NOTFOUND   -258
#define AT_ERR_CLIENT_DOWN    -259
#define AT_ERR_FILE_NOTFOUND  -260
#define AT_ERR_CONN_REJECTED  -261

/*******************************************************************************
 *                           DEVICE <--> SERVER
 ******************************************************************************/
#define AMBATEST_DEVICE_PORT    6868

/* REGISTER: a device register with test server */
#define AMBA_TEST_MSG_REGISTER          0x00000001
#define AMBA_TEST_MSG_DEREGISTER        0x00000002

typedef struct {
        char            device_hw[32];
        char            device_os[32];
} atd_spec_t;


/*******************************************************************************
 *                           CLIENT <--> SERVER
 ******************************************************************************/
#define AMBATEST_CLIENT_PORT    6866

/* header struct, all client-server messages have this header */
typedef struct {
	uint32_t        len;
	uint32_t        seqnum;
	uint32_t        type;
	uint32_t        ret;
} atc_header_t;

/* REGISTER: a client registers with test server */
typedef struct {
        char            client_username[32];
        char            client_ipaddr[32];
} atc_register_t;

/* OPEN: client opens the connection to test farm */
#define AMBA_TEST_MSG_OPEN      0x00000010
typedef struct {
        atd_spec_t      device_spec;
#define ATC_CONN_MODE_SINGLE    0x0
#define ATC_CONN_MODE_MULTI     0x1
	uint32_t        mode;
        uint32_t        priority;
        uint32_t        num_jobs;
} atc_open_t;

/* CLOSE: client closes the connection to test farm */
#define AMBA_TEST_MSG_CLOSE     0x00000011
typedef struct {
        uint32_t        connection_id;
} atc_close_t;

/* PUSH: client pushes data to device */
#define AMBA_TEST_MSG_PUSH      0x00000020
typedef struct {
	char            src[MAX_FILENAME_SIZE];
	char            dst[MAX_FILENAME_SIZE];
	uint32_t        len;
	uint32_t        crc;
} atc_push_t;

/* PUSH: client pulls data to device */
#define AMBA_TEST_MSG_PULL      0x00000021
typedef struct {
	char            src[MAX_FILENAME_SIZE];
	char            dst[MAX_FILENAME_SIZE];
	uint32_t        len;
	uint32_t        crc;
} atc_pull_t;

/* DRUN: runs a command on test device(s) */
#define AMBA_TEST_MSG_DRUN      0x00000022
typedef struct {
        char            command[256];
        uint32_t        timeout;
} atc_drun_t;

/* STAT: gets the server statistics */
#define AMBA_TEST_MSG_STAT      0x00000040
typedef struct {
        uint32_t        id;
        uint32_t        num_jobs;
        uint32_t        uptime;
        uint32_t        busytime;
        atd_spec_t      spec;
} atd_stat_t;

typedef struct {
        uint32_t        id;
        int32_t         device_id;
        uint32_t        uptime;
        uint32_t        cputime;
        atd_spec_t      spec;
        uint32_t        job_total;
        uint32_t        job_done;
        char            username[32];
} atc_stat_t;

typedef struct {
        char            username[32];
        uint32_t        cputime;
        uint32_t        num_clients;
        uint32_t        num_jobs;
} atu_stat_t;

typedef struct {
        uint32_t        device_num;
        uint32_t        device_stat_offset;
        uint32_t        client_num;
        uint32_t        client_stat_offset;
        uint32_t        user_num;
        uint32_t        user_stat_offset;
        uint64_t        num_jobs;
} at_stat_t;

#endif  //__AMBA_TEST_MSG__
