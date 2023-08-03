#ifndef __AMBA_HOST_MSG__
#define __AMBA_HOST_MSG__

#include <stdint.h>

#define MAX_FILENAME_SIZE      256

/*******************************************************************************
 *                LOOPBACK: a message will be loopback from target
 ******************************************************************************/
#define AMBA_HOST_MSG_LOOPBACK 0x00000001
struct AmbaHost_msg_loopback {
	char            message[0];
};

/*******************************************************************************
 *                                Fille Copy
 ******************************************************************************/
#define AMBA_HOST_MSG_COPYFROM  0x00000010
#define AMBA_HOST_MSG_COPYTO    0x00000011
struct AmbaHost_msg_copyfile {
	char            src_name[MAX_FILENAME_SIZE];
	char            dst_name[MAX_FILENAME_SIZE];
	uint32_t        len;
	uint32_t        crc;
};

/*******************************************************************************
 *                             Memory Read/Write
 ******************************************************************************/
#define AMBA_HOST_MSG_READ      0x00000020
#define AMBA_HOST_MSG_WRITE     0x00000021
struct AmbaHost_msg_memops {
	uint64_t        hAddr;
	uint32_t        tpAddr;
	uint32_t        size;
};

/*******************************************************************************
 *                FLOW-CONTROL: DONE, STOP, RESUME packets
 ******************************************************************************/
#define AMBA_HOST_MSG_PAUSE     0x00000030
#define AMBA_HOST_MSG_RESUME    0x00000031
#define AMBA_HOST_MSG_DONE      0x00000032
struct AmbaHost_msg_flowctrl {
	uint32_t        trxn_id;
};

/*******************************************************************************
 *                  AmbaHost Message Defintion: host <--> target
 ******************************************************************************/
typedef struct AmbaHost_msg_header {
	uint32_t        len;
	uint32_t        seqnum;
	uint32_t        type;
	uint32_t        ret;
} AHMSG_HDR;

#endif  //__AMBA_HOST_MSG__
