/*
 * Copyright (c) 2018-2018 Ambarella, Inc.
 * 2018/06/01 - [Zhikan Yang] created file
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

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/mm.h>
#include <linux/of_irq.h>
#include <linux/interrupt.h>
#include <linux/uaccess.h>
#include "ambacv_kal.h"
#include "cavalry_ioctl.h"
#include "cavalry.h"
#include "cavalry_mem.h"
#include "cavalry_log.h"
#include "cavalry_print.h"

/*================================================================================================*/
/* Directly imported from ambarella/private/cavalry_drv git repository                            */
/*                                                                                                */
/* File source : "cavalry_mem.c"                                                                  */
/* File size   : 6895 bytes                                                                       */
/* File md5sum : 69f65a4a5e3ee224307b9b31ee8d1e18                                                 */
/* Git refhash : 5ab455629964ee0db3dbbf1f50f76bf8f4cba2f8                                         */
/*                                                                                                */
/* Changes made while importing:                                                                  */
/*   + Update to setting of cavalry->cavalry_mem_info[CAVALRY_MEM_USER] to work with CVFLOW       */
/*================================================================================================*/

void cavalry_mem_init(struct ambarella_cavalry *cavalry, unsigned long total_size)
{
	void *vbase = NULL;
	unsigned long base = 0, size = 0;

	base = cavalry->cma_private.phys;
	size = cavalry->cma_private.size;
	vbase = cavalry->cma_private.virt;

	cavalry->cavalry_mem_info[CAVALRY_MEM_ALL].phys = base;
	cavalry->cavalry_mem_info[CAVALRY_MEM_ALL].virt = vbase;
	cavalry->cavalry_mem_info[CAVALRY_MEM_ALL].size = total_size;

	cavalry->cavalry_mem_info[CAVALRY_MEM_CMD].phys = base + CAVALRY_MEM_CMD_OFFSET;
	cavalry->cavalry_mem_info[CAVALRY_MEM_CMD].virt = vbase + CAVALRY_MEM_CMD_OFFSET;
	cavalry->cavalry_mem_info[CAVALRY_MEM_CMD].size = CAVALRY_MEM_CMD_SIZE;

	cavalry->cavalry_mem_info[CAVALRY_MEM_MSG].phys = base + CAVALRY_MEM_MSG_OFFSET;
	cavalry->cavalry_mem_info[CAVALRY_MEM_MSG].virt = vbase + CAVALRY_MEM_MSG_OFFSET;
	cavalry->cavalry_mem_info[CAVALRY_MEM_MSG].size = CAVALRY_MEM_MSG_SIZE;

	cavalry->cavalry_mem_info[CAVALRY_MEM_LOG].phys = base + CAVALRY_MEM_LOG_OFFSET;
	cavalry->cavalry_mem_info[CAVALRY_MEM_LOG].virt = vbase + CAVALRY_MEM_LOG_OFFSET;
	cavalry->cavalry_mem_info[CAVALRY_MEM_LOG].size = CAVALRY_MEM_LOG_SIZE;

	cavalry->cavalry_mem_info[CAVALRY_MEM_CMD_FEX].phys = base + CAVALRY_MEM_CMD_FEX_OFFSET;
	cavalry->cavalry_mem_info[CAVALRY_MEM_CMD_FEX].virt = vbase + CAVALRY_MEM_CMD_FEX_OFFSET;
	cavalry->cavalry_mem_info[CAVALRY_MEM_CMD_FEX].size = CAVALRY_MEM_CMD_FEX_SIZE;

	cavalry->cavalry_mem_info[CAVALRY_MEM_MSG_FEX].phys = base + CAVALRY_MEM_MSG_FEX_OFFSET;
	cavalry->cavalry_mem_info[CAVALRY_MEM_MSG_FEX].virt = vbase + CAVALRY_MEM_MSG_FEX_OFFSET;
	cavalry->cavalry_mem_info[CAVALRY_MEM_MSG_FEX].size = CAVALRY_MEM_MSG_FEX_SIZE;

	cavalry->cavalry_mem_info[CAVALRY_MEM_CMD_FMA].phys = base + CAVALRY_MEM_CMD_FMA_OFFSET;
	cavalry->cavalry_mem_info[CAVALRY_MEM_CMD_FMA].virt = vbase + CAVALRY_MEM_CMD_FMA_OFFSET;
	cavalry->cavalry_mem_info[CAVALRY_MEM_CMD_FMA].size = CAVALRY_MEM_CMD_FMA_SIZE;

	cavalry->cavalry_mem_info[CAVALRY_MEM_MSG_FMA].phys = base + CAVALRY_MEM_MSG_FMA_OFFSET;
	cavalry->cavalry_mem_info[CAVALRY_MEM_MSG_FMA].virt = vbase + CAVALRY_MEM_MSG_FMA_OFFSET;
	cavalry->cavalry_mem_info[CAVALRY_MEM_MSG_FMA].size = CAVALRY_MEM_MSG_FMA_SIZE;

	cavalry->cavalry_mem_info[CAVALRY_MEM_UCODE].phys = base + CAVALRY_MEM_UCODE_OFFSET;
	cavalry->cavalry_mem_info[CAVALRY_MEM_UCODE].virt = vbase + CAVALRY_MEM_UCODE_OFFSET;
	cavalry->cavalry_mem_info[CAVALRY_MEM_UCODE].size = CAVALRY_MEM_HOTLINK_OFFSET - CAVALRY_MEM_UCODE_OFFSET;

	cavalry->cavalry_mem_info[CAVALRY_MEM_HOTLINK_SLOT0].phys = base + CAVALRY_MEM_HOTLINK_SLOT0_OFFSET;
	cavalry->cavalry_mem_info[CAVALRY_MEM_HOTLINK_SLOT0].virt = vbase + CAVALRY_MEM_HOTLINK_SLOT0_OFFSET;
	cavalry->cavalry_mem_info[CAVALRY_MEM_HOTLINK_SLOT0].size = CAVALRY_MEM_HOTLINK_SLOT1_OFFSET - CAVALRY_MEM_HOTLINK_SLOT0_OFFSET;

	cavalry->cavalry_mem_info[CAVALRY_MEM_HOTLINK_SLOT1].phys = base + CAVALRY_MEM_HOTLINK_SLOT1_OFFSET;
	cavalry->cavalry_mem_info[CAVALRY_MEM_HOTLINK_SLOT1].virt = vbase + CAVALRY_MEM_HOTLINK_SLOT1_OFFSET;
	cavalry->cavalry_mem_info[CAVALRY_MEM_HOTLINK_SLOT1].size = CAVALRY_MEM_HOTLINK_SLOT2_OFFSET - CAVALRY_MEM_HOTLINK_SLOT1_OFFSET;

	cavalry->cavalry_mem_info[CAVALRY_MEM_HOTLINK_SLOT2].phys = base + CAVALRY_MEM_HOTLINK_SLOT2_OFFSET;
	cavalry->cavalry_mem_info[CAVALRY_MEM_HOTLINK_SLOT2].virt = vbase + CAVALRY_MEM_HOTLINK_SLOT2_OFFSET;
	cavalry->cavalry_mem_info[CAVALRY_MEM_HOTLINK_SLOT2].size = CAVALRY_MEM_HOTLINK_SLOT3_OFFSET - CAVALRY_MEM_HOTLINK_SLOT2_OFFSET;

	cavalry->cavalry_mem_info[CAVALRY_MEM_HOTLINK_SLOT3].phys = base + CAVALRY_MEM_HOTLINK_SLOT3_OFFSET;
	cavalry->cavalry_mem_info[CAVALRY_MEM_HOTLINK_SLOT3].virt = vbase + CAVALRY_MEM_HOTLINK_SLOT3_OFFSET;
	cavalry->cavalry_mem_info[CAVALRY_MEM_HOTLINK_SLOT3].size = CAVALRY_MEM_USER_OFFSET - CAVALRY_MEM_HOTLINK_SLOT3_OFFSET;

#if 0 /* Original CAVALRY implementation */
	cavalry->cavalry_mem_info[CAVALRY_MEM_USER].phys = base + CAVALRY_MEM_USER_OFFSET;
	cavalry->cavalry_mem_info[CAVALRY_MEM_USER].virt = vbase + CAVALRY_MEM_USER_OFFSET;
	cavalry->cavalry_mem_info[CAVALRY_MEM_USER].size = total_size - CAVALRY_MEM_USER_OFFSET;
#else /* Updated CVFLOW implementation */
	cavalry->cavalry_mem_info[CAVALRY_MEM_USER].phys = base + size;
	cavalry->cavalry_mem_info[CAVALRY_MEM_USER].virt = vbase + size;
	cavalry->cavalry_mem_info[CAVALRY_MEM_USER].size = total_size - size;
#endif /* CAVALRY/CVFLOW implementation */

	mutex_init(&cavalry->cavalry_buf_query_mutex);
}

int cavalry_query_buf(struct ambarella_cavalry *cavalry, void __user *arg)
{
	struct cavalry_querybuf cavalry_querybuf;
	int buf_id = 0;
	int rval = 0;

	mutex_lock(&cavalry->cavalry_buf_query_mutex);

	if (copy_from_user(&cavalry_querybuf, arg, sizeof(cavalry_querybuf))) {
		return -EFAULT;
	}

	buf_id = cavalry_querybuf.buf;

	if (buf_id < CAVALRY_MEM_FIRST || buf_id > CAVALRY_MEM_LAST) {
		prt_err("Invalid buf ID %u\n", buf_id);
		rval = -EINVAL;
	} else {
		cavalry_querybuf.length = cavalry->cavalry_mem_info[buf_id].size;
		cavalry_querybuf.offset = ambacv_c2p(cavalry->cavalry_mem_info[buf_id].phys);
	}

	if (!rval) {
		rval = copy_to_user(arg, &cavalry_querybuf, sizeof(cavalry_querybuf));
	}

	mutex_unlock(&cavalry->cavalry_buf_query_mutex);

	return rval;
}

