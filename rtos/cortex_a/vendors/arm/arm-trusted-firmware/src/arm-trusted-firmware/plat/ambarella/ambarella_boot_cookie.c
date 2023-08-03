/**
 *
 * History:
 *    2019/10/22 - [Cao Rongrong] created file
 *
 * Copyright (c) 2016 Ambarella, Inc.
 *
 * This file and its contents ("Software") are protected by intellectual
 * property rights including, without limitation, U.S. and/or foreign
 * copyrights. This Software is also the confidential and proprietary
 * information of Ambarella, Inc. and its licensors. You may not use, reproduce,
 * disclose, distribute, modify, or otherwise prepare derivative works of this
 * Software or any portion thereof except pursuant to a signed license agreement
 * or nondisclosure agreement with Ambarella, Inc. or its authorized affiliates.
 * In the absence of such an agreement, you agree to promptly notify and return
 * this Software to Ambarella, Inc.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 * MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */
#include <string.h>
#include <lib/mmio.h>
#include <plat/common/platform.h>
#include <boot_cookie.h>

static boot_cookie_t g_boot_cookie;

boot_cookie_t *boot_cookie_init(void)
{
#if defined(CONFIG_ATF_AMBALINK)
	memset(&g_boot_cookie, 0x0, sizeof(boot_cookie_t));
	g_boot_cookie.magic = BOOT_COOKIE_MAGIC_NUM;

	/* amboot address */
	g_boot_cookie.bld_ram_start = BL33_BASE;

#if defined(AMBARELLA_CV2FS) || defined(AMBARELLA_CV22FS)
	/* BLD of R52 pass FIP byte address */
	g_boot_cookie.bld_media_start = mmio_read_32(SCRATCHPAD_REG(0x70));
#else
	/* BST write FIP byte address into 0xf2000080 [AXI_BASE 0xf2000000  + AXI_BYPASS_BOOT_CHANGE 0x80] */
	g_boot_cookie.bld_media_start = mmio_read_32(AXI_BASE + AXI_BYPASS_BOOT_CHANGE);
#endif
#else
	uintptr_t data3 = mmio_read_32(SCRATCHPAD_BASE + AHBSP_DATA3_OFFSET);
	boot_cookie_t *boot_cookie = (boot_cookie_t *)data3;

	memset(&g_boot_cookie, 0, sizeof(boot_cookie_t));

	/* santiy check, valid value must be 4B aligned. */
	if ((data3 & 0x3) || boot_cookie->magic != BOOT_COOKIE_MAGIC_NUM)
		return NULL;

	memcpy(&g_boot_cookie, boot_cookie, sizeof(boot_cookie_t));

#endif
	return &g_boot_cookie;
}

boot_cookie_t *boot_cookie_ptr(void)
{
	return (g_boot_cookie.magic == BOOT_COOKIE_MAGIC_NUM) ? &g_boot_cookie : NULL;
}

