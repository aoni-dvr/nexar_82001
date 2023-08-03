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
#include <common.h>

#define FASTBOOT_HEADER_SIZE		2048
#define FASTBOOT_HEADER_MAGIC		"ANDROID!"
#define FASTBOOT_HEADER_MAGIC_SIZE	8

#define FASTBOOT_VENDOR_ID		0x18d1
#define FASTBOOT_PRODUCT_ID		0x4ee0

struct boot_imgage_hdr {
	unsigned long magic;
	unsigned int size;
	unsigned int kernel_addr;
};

struct fastboot_cmd
{
	const char *prefix;
	void (*handle)(const char *arg, void *data, unsigned int sz);
};

static void *download_base;
static unsigned int download_size;
static unsigned int cur_size;

static void cmd_download(const char *arg, void *data, unsigned sz);
static void cmd_boot(const char *arg, void *data, unsigned sz);

static struct fastboot_cmd cmd_list[] =
{

	{
		.prefix = "download:",
		.handle = cmd_download,
	},

	{
		.prefix = "boot",
		.handle = cmd_boot,
	},
};

static void fastboot_ack(const char *code, const char *reason)
{
	char response[64];

	memcpy(response, code, strlen(code));
	memcpy(response + strlen(code), reason, strlen(reason));
	udc_bulk_transfer(response, strlen(code) + strlen(reason));
}

static void fastboot_okay(const char *str)
{
	fastboot_ack("OKAY", str);
}

static void cmd_download(const char *arg, void *data, unsigned sz)
{
	unsigned int len = hex2unsigned(arg);
	char response[64] = "DATA";
	memcpy(response + 4, arg, strlen(arg));
	udc_bulk_transfer(response, strlen(response));

	udc_state_download();
	download_size = len;
}

static void cmd_boot(const char *arg, void *data, unsigned sz)
{
	void (*jump)(void) = (void *)(data);

	fastboot_okay("");

	mdelay(10);

	pr_log("\nStart to run ... 0x%x\n", (unsigned long)data);

	jump ();
	while(1);
}

void fastboot_cmd_parse(void *data, unsigned int len)
{
	int i;
	void (*fb_cmd)(const char *, void *, unsigned int) = NULL;

	for (i = 0; i < sizeof(cmd_list) / sizeof(struct fastboot_cmd); i++)
	{
		if (!memcmp(cmd_list[i].prefix, data, strlen(cmd_list[i].prefix))) {
			pr_log("fastboot:");
			pr_log((const char *)data);
			pr_log("\n");
			fb_cmd = cmd_list[i].handle;
			break;
		}
	}

	if (fb_cmd)
		fb_cmd((const char *)data + strlen(cmd_list[i].prefix),
				download_base, download_size);
	else
		fastboot_ack("FAIL", "unknown command");
}

void fastboot_data_handle(void *data, unsigned int size)
{
	struct boot_imgage_hdr *image_hdr;

	if (!download_base) {
		image_hdr = (struct boot_imgage_hdr *)data;
		if (!memcmp(&image_hdr->magic, FASTBOOT_HEADER_MAGIC,
					FASTBOOT_HEADER_MAGIC_SIZE))
			download_base = (void *)(unsigned long)(image_hdr->kernel_addr - 0x8000);
		else
			download_base = (void *)CONFIG_DEFAULT_LOADADDR;
	}

	if (cur_size >= FASTBOOT_HEADER_SIZE)
		memcpy(download_base + cur_size - FASTBOOT_HEADER_SIZE,
				data, size);

	cur_size += size;
	if (cur_size >= download_size) {
		cur_size = 0;
		fastboot_okay("");
		udc_state_cmd();
	}
}

void fastboot_helper(void)
{
	udc_register_helper(FASTBOOT_VENDOR_ID, FASTBOOT_PRODUCT_ID,
			fastboot_cmd_parse, fastboot_data_handle);
}
