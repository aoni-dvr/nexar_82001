/**
 * Copyright (c) 2021 Ambarella International LP
 *
 * This file and its contents ("Software") are protected by intellectual
 * property rights including, without limitation, U.S. and/or foreign
 * copyrights. This Software is also the confidential and proprietary
 * information of Ambarella International LP and its licensors. You may not use, reproduce,
 * disclose, distribute, modify, or otherwise prepare derivative works of this
 * Software or any portion thereof except pursuant to a signed license agreement
 * or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
 * In the absence of such an agreement, you agree to promptly notify and return
 * this Software to Ambarella International LP
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
 */
#include <config.h>
#include <memory.h>
#include <common.h>
#include <io.h>

#define AMBDL_VENDOR_ID			0x4255
#define AMBDL_PRODUCT_ID		0x0001

#define AMBDL_SIGNATURE_CMD		0x55434d44
#define AMBDL_SIGNATURE_RESP		0x55525350

#define TWISTY_CMD_GET_DRAM_SIZE	0x444D535A	/* DMSZ */

#define AMBDL_RESP_SUCCESS		0
#define AMBDL_RESP_FAIL			1

typedef struct ambdl_protocol_cmd {
	unsigned int signature;
	unsigned int command;
	unsigned int param0;
	unsigned int param1;
	union {
		struct {
			unsigned int param2;
			unsigned int param3;
		};
		struct {
			unsigned long long param23;
		};
	};

	union {
		struct {
			unsigned int param4;
			unsigned int param5;
		};
		struct {
			unsigned long long param45;
		};
	};
} HOST_CMD;

typedef struct ambdl_protocol_resp {
	unsigned int signature;
	unsigned int response;
	unsigned int param0;
	unsigned int param1;
} HOST_RESP;

static void *download_base;
static unsigned int download_size;

static void send_response(unsigned int resp, unsigned int param0, unsigned int param1)
{
	HOST_RESP response;

	response.signature = AMBDL_SIGNATURE_RESP;
	response.response = resp;
	response.param0 = param0;
	response.param1 = param1;

	udc_bulk_transfer(&response, sizeof(HOST_RESP));
}
static void ambdl_inquiry_status(HOST_CMD *cmd)
{
	switch(cmd->param0)
	{
	case 1: /* query CHIP_REV */
		send_response(0, CHIP_REV, 0);
		break;
	case 2: /* query BLD_RAM_START */
		send_response(0, 0, 0);
		break;
	case 3: /* read Register */
		send_response(AMBDL_RESP_FAIL, 0, 0);
		break;
	default:
		break;
	}
}

static void ambdl_ready_send(HOST_CMD *cmd)
{
	unsigned int size = cmd->param3;

	switch(cmd->param1) {
	case 1: /* READY to send memory to host, response a fake crc value */
		send_response(0, size, 0xffffffff);
		break;
	default: /* Unsupported, response to avoid block */
		send_response(0, 0, 0);
		break;
	}
}

static void ambdl_send_data(HOST_CMD *cmd)
{
	/* Start to send data */
	udc_bulk_transfer((void *)(unsigned long)cmd->param2, cmd->param3);

	__asm__ volatile("dsb sy");

	/* data xfer-done */
	send_response(0, 0, 0);
}

static void ambdl_extension_cmd(HOST_CMD *cmd)
{
	void (*jump)(void);

	switch(cmd->param0)
	{
	case 2: /* FLAG_ADDRESS */
		download_base = (void *)(unsigned long)cmd->param1;
		send_response(0, 0, 0);
		break;
	case 8: /* FLAG_CMD */
		switch (cmd->param1){
		case 1: /* Read memory */
			if (cmd->param2 == TWISTY_CMD_GET_DRAM_SIZE)
				send_response(0, plat_get_dram_size_mb(), 0);
			else
				send_response(0, readl(cmd->param23), 0);
			break;
		case 2: /* WRITE memory */
			writel(cmd->param2, cmd->param3);
			send_response(0, 0, 0);
			break;

		case 9:/* Write Register in UST */
			writel(cmd->param23, cmd->param4);
			send_response(0, 0, 0);
			break;
		case 10:/* Read Register in UST */
			send_response(0, readl(cmd->param23), 0);
			break;
		default: /* Unsupported */
			pr_log("%s: Unimplemented. \n", __func__);
			break;
		}
		break;
	case 0x15: /* JUMP */

		pr_log("Start to run ... %x\n", download_base);

		jump = download_base;

		send_response(0, 0, 0);
		mdelay(10);

		jump ();

		break;
	default:
		pr_log("%s: Unimplemented %x\n", __func__, cmd->param0);
		break;
	}
}

static void ambdl_data_handle(void *data, unsigned int size)
{
	memcpy(download_base + download_size, data, size);

	download_size += size;

	if (size < 512) {
		/* finish data xfer */
		send_response(0, 0, 0);
		udc_state_cmd();
	}
}

static void ambdl_cmd_handle(void *data,unsigned int size)
{
	HOST_CMD *cmd = data;

#if 1
	pr_log("%08x: [C]%08x [0]%08x [1]%08x [2].%08x [3].%08x\n",
			cmd->signature,
			cmd->command,
			cmd->param0,
			cmd->param1,
			cmd->param2,
			cmd->param3);
#endif

	if (cmd->signature != AMBDL_SIGNATURE_CMD)
		return ;

	switch(cmd->command) {
	case 0:	/* RDY_TO_RCV */
		ambdl_extension_cmd(cmd);
		break;
	case 1: /* RCV_DATA */
		udc_state_download();
		download_size = 0;
		download_base = (void *)(unsigned long)cmd->param1;
		break;
	case 2: /* RDY_TO_SND */
		ambdl_ready_send(cmd);
		break;
	case 3: /* SND_DATA */
		ambdl_send_data(cmd);
		break;
	case 4: /* INQUIRY_STATUS */
		ambdl_inquiry_status(cmd);
		break;
	default:
		break;
	}
}

void ambdl_helper(void)
{
	udc_register_helper(AMBDL_VENDOR_ID, AMBDL_PRODUCT_ID,
			ambdl_cmd_handle, ambdl_data_handle);
}
