#ifndef __SCHDRMSG_DEF_H__
#define __SCHDRMSG_DEF_H__

#ifdef __KERNEL__
#include <linux/types.h>
typedef unsigned int                    ptr32_t;
#else
#include "ambint.h"
#endif

#include <cavalry_ioctl.h>
#include <cvsched_api.h>
#include "cavalry.h"

#define ARM_CACHELINE_SIZE              64U
#define ROUND_UP_TO_CACHELINE(x)        ( (x + ARM_CACHELINE_SIZE - 1) & (~(ARM_CACHELINE_SIZE - 1)) )

typedef struct {
	uint32_t*               wptr;
	char*                   base;
	uint32_t                mask;
	uint32_t                esize;
	uint32_t                wridx;

	uint64_t                wptr_daddr;
	uint64_t                base_daddr;
} schdr_log_info_t;

#endif //__SCHDRMSG_DEF_H__
