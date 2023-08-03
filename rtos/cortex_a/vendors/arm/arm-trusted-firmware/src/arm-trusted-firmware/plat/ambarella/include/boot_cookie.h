/**
 * Copyright (c) 2029 Ambarella International LP
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
#ifndef __AMB_BOOT_COOKIE_H__
#define __AMB_BOOT_COOKIE_H__

/*
 * WARNING:
 * This header file is used by BST, BLD and ATF, please sync between:
 * amboot/include/boot_cookie.h
 * arm-trusted-firmware/plat/ambarella/include/boot_cookie.h
 */

/*===========================================================================*/

#define BOOT_COOKIE_MAGIC_NUM			(0x40455641)

#define BOOT_COOKIE_MAGIC_OFFSET		(0x00)
#define BOOT_COOKIE_VERSION_OFFSET		(0x04)
#define BOOT_COOKIE_RSVD0_OFFSET		(0x08)
#define BOOT_COOKIE_SOC_NOTIFY_OFFSET		(0x0c)
#define BOOT_COOKIE_BLD_RAM_START_OFFSET	(0x10)
#define BOOT_COOKIE_RSVD1_OFFSET		(0x14)
#define BOOT_COOKIE_BLD_MEDIA_OFFSET		(0x18)
#define BOOT_COOKIE_BAK_BLD_MEDIA_OFFSET	(0x1c)
#define BOOT_COOKIE_RSVD2_OFFSET		(0x20)
#define BOOT_COOKIE_DRAM_TRAINING_OFFSET	(0x24)
#define BOOT_COOKIE_DRAM_PRAM_OFFSET		(0x28)
#define BOOT_COOKIE_DTB_RAM_START_OFFSET	(0x2c)
#define BOOT_COOKIE_DRAM_CFG_OFFSET		(0x30)
#define BOOT_COOKIE_KEYINDX_OFFSET		(0x34)
#define BOOT_COOKIE_SYS_RESET_MODE_OFFSET	(0x38)

#define BOOT_COOKIE_MAX_SIZE			(0x80)

#ifndef __ASM__

typedef struct {
	unsigned int magic;
	unsigned int version;
	unsigned int rsvd0;
	unsigned int soc_notify_gpioaddr;	/* soc notify --> mcu */
	unsigned int bld_ram_start;
	unsigned int rsvd1;
	unsigned int bld_media_start;
	unsigned int bak_bld_media_start;
	unsigned int rsvd2;
	unsigned int dram_training;
	unsigned int dram_param_media_start;
	unsigned int dtb_ram_start;
	unsigned int dram_config_reg_value;
	unsigned int bst_key_index;
	unsigned int sys_reset_mode;
	unsigned int padding[32-15];
} boot_cookie_t;

boot_cookie_t *boot_cookie_init(void);	/* called only once at very beginning */
boot_cookie_t *boot_cookie_ptr(void);

#endif

#endif	/* __AMB_BOOT_COOKIE_H__ */
