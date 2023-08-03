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
#ifndef __AMBUST_COMMON_H__
#define __AMBUST_COMMON_H__

#define min(x, y)		((x) < (y) ? (x) : (y))
#define max(x, y)		((x) > (y) ? (x) : (y))
#define NULL			(void *)(0)
#define ARRAY_SIZE(x)		(sizeof(x) / sizeof((x)[0]))

#define ROUNDUP(x, a)		(!(a) ? (x) : ((x) + ((a) - 1)) / (a) * (a))
#define ROUNDDOWN(x, a)		(!(a) ? (x) : (x) - ((x) % (a)))


#if 0
#define pr_debug(fmt, ...)	printf(fmt, ##__VA_ARGS__)
#else
#define pr_debug(fmt, ...)	do { } while(0)
#endif

#if DEBUG
#define pr_log(fmt, ...)	printf(fmt, ##__VA_ARGS__)
#else
#define pr_log(fmt, ...)	do { } while(0)
#endif

enum {
	CMD_USLEEP,
	CMD_DDRC_WRITE,
	CMD_DDRH0_WRITE,
	CMD_DDRH1_WRITE,
	CMD_RCT_WRITE,
	CMD_POLL,
};

struct dram_param_t
{
	unsigned int cmd;
#if 0
	unsigned long reg;
	unsigned int value;
	unsigned int poll_value;
#else
	unsigned int reg;
	unsigned int value;
#endif
};

void dram_init(void);

void pinmux_init(const unsigned int (*pins)[2], int num);
void uart_init(void);
void uart_putchar(char c);
void uart_putstr(const char *s);

void fastboot(void);

int printf(const char *format, ...);

void *memset(void *s, int c, unsigned int n);
void *memcpy(void *dest, const void *src, unsigned int n);
int memcmp(const void *s1, const void *s2, unsigned int n);
unsigned int strlen(const char *s);
void *kmalloc(unsigned int size);
unsigned int hex2unsigned(const char *x);

int plat_get_dram_size_mb(void);
void plat_dram_init(void);
void plat_pinmux_init(void);
void plat_usb_reset(void);
void plat_enable_usb(void);
void mdelay(unsigned int ms);
void plat_set_uart_pll_div(int port, unsigned int div);

void usb_download(void);
void udc_state_download(void);
void udc_state_cmd(void);
void udc_bulk_transfer(void *data, unsigned len);
void udc_register_helper(unsigned short vid,
		unsigned short pid,
		void (*cmd)(void *, unsigned int),
		void (*data)(void *, unsigned int));

void fastboot_cmd_parse(void *data, unsigned int len);
void fastboot_data_handle(void *data, unsigned int len);


void ambdl_helper(void);
void fastboot_helper(void);
void memory_tester(void);
void setup_vector(void);

#endif
