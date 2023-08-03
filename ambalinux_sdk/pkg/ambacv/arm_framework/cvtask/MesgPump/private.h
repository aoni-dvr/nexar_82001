/*
 * Copyright (c) 2017-2017 Ambarella, Inc.
 *
 * This file and its contents ("Software") are protected by intellectual
 * property rights including, without limitation, U.S. and/or foreign copyrights.
 * This Software is also the confidential and proprietary information of
 * Ambarella, Inc. and its licensors.  You may not use, reproduce, disclose,
 * distribute, modify, or otherwise prepare derivative works of this Software
 * or any portion thereof except pursuant to a signed license agreement or
 * nondisclosure agreement with Ambarella, Inc. or its authorized affiliates.
 * In the absence of such an agreement, you agree to promptly notify and
 * return this Software to Ambarella, Inc.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 * MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include <stdint.h>

#ifdef MPX_TEST
#define CMSG                    printf
#define IMSG                    printf
#define DMSG                    printf
#define VMSG                    printf
#else
#include "cvtask_api.h"
#include "cvtask_errno.h"
#include "cvtask_ossrv.h"
#define CMSG(...)               CVTASK_PRINTF(LVL_CRITICAL, __VA_ARGS__)
#define IMSG(...)               CVTASK_PRINTF(LVL_NORMAL,   __VA_ARGS__)
#define DMSG(...)               CVTASK_PRINTF(LVL_DEBUG,    __VA_ARGS__)
#define VMSG(...)               CVTASK_PRINTF(LVL_VERBOSE,  __VA_ARGS__)
#endif

#define MAX_BINARY_COUNT 32
#define MAX_CVTASK_COUNT 64
#define MAX_PMESG_COUNT  1024
#define MAX_MPX_SIZE     (32*1024)



struct pumper_mesg {
	int       cvtask_id;
	int       frame_send;
	int       frame_deliver;
	int       binary_id;
	int       payload_off;
	int       payload_len;
};

struct priv_data {
	FILE*     binary_fp[MAX_BINARY_COUNT];
	uint32_t  sysflow_id[MAX_CVTASK_COUNT];
	uint32_t  uuid[MAX_CVTASK_COUNT];
	char      sysflow_name[MAX_CVTASK_COUNT][32];
	struct pumper_mesg pmesg[MAX_PMESG_COUNT];
	char*     mesg_buffer;
	int       binary_count;
	int       pmesg_count;
	int       pmesg_index;
	char      mpx_buff[MAX_MPX_SIZE];
};

void parse_message_manifest(struct priv_data *db, char *buf, int size);
