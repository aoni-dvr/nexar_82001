/*
 * Copyright (c) 2019 Ambarella, Inc.
 * 2019/09/17 - [Tao Wu] created file
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

/*================================================================================================*/
/* Directly imported from ambarella/private/cavalry_drv git repository                            */
/*                                                                                                */
/* File source : "cavalry_misc.c"                                                                 */
/* File size   : 3910 bytes                                                                       */
/* File md5sum : 4c2f161a3242e94a746a9a2c9d772cec                                                 */
/* Git refhash : 5ab455629964ee0db3dbbf1f50f76bf8f4cba2f8                                         */
/*                                                                                                */
/* Changes made while importing: None                                                             */
/*================================================================================================*/

#include <linux/uaccess.h>
#include <asm/io.h>

#include "cavalry_ioctl.h"
#include "cavalry.h"
#include "cavalry_print.h"

static void __iomem *rct_io_base = NULL;

#ifndef REF_CLK_FREQ
#define REF_CLK_FREQ		24000000
#endif

#ifndef DBGBUS_BASE
#define DBGBUS_BASE		0xED000000
#endif

#ifndef RCT_OFFSET
#define RCT_OFFSET			0x00080000
#endif

#ifndef RCT_BASE
#define RCT_BASE			(DBGBUS_BASE + RCT_OFFSET)
#endif

#ifndef RCT_REG
//#define RCT_REG(x)			(RCT_BASE + (x))
#define RCT_REG(x)			(rct_io_base + (x))
#endif

#ifndef RCT_SIZE
#define RCT_SIZE				(0x1000)
#endif

#define PLL_AUDIO_CTRL_OFFSET		0x54
#define PLL_AUDIO_FRAC_OFFSET		0x58
#define SCALER_AUDIO_POST_OFFSET	0x5C
#define SCALER_AUDIO_PRE_OFFSET		0x60

#define PLL_AUDIO_CTRL_REG		RCT_REG(PLL_AUDIO_CTRL_OFFSET)
#define PLL_AUDIO_FRAC_REG		RCT_REG(PLL_AUDIO_FRAC_OFFSET)
#define SCALER_AUDIO_POST_REG		RCT_REG(SCALER_AUDIO_POST_OFFSET)
#define SCALER_AUDIO_PRE_REG		RCT_REG(SCALER_AUDIO_PRE_OFFSET)

static uint64_t get_audio_pll(void)
{
	uint64_t value = 0;
	uint32_t postscale, prescale;
	uint32_t ctrl, frac;
	uint32_t sdiv, sdout, inter;

	if (rct_io_base == NULL) {
#if defined (CHIP_CV5) || defined(CHIP_CV52)
		rct_io_base = ioremap((RCT_BASE + 0x2000000000UL), RCT_SIZE);
#elif (defined(CHIP_CV2) || defined(CHIP_CV22) || defined(CHIP_CV25) || defined(CHIP_CV28) || defined(CHIP_CV2A) || defined(CHIP_CV2FS) || defined(CHIP_CV22A) || defined(CHIP_CV22FS))
		rct_io_base = ioremap(RCT_BASE, RCT_SIZE);
#else
#error "unsupported CHIP ID, please check your PROJECT setting"
#endif
		if (rct_io_base == NULL) {
			pr_err("ioremap RCT registers failed\n");
			return DEFAULT_AUDIO_CLK_HZ;
		}
	}
	ctrl = readl(PLL_AUDIO_CTRL_REG);
	frac = readl(PLL_AUDIO_FRAC_REG);
	prescale = readl(SCALER_AUDIO_PRE_REG);
	postscale = readl(SCALER_AUDIO_POST_REG);
	if (rct_io_base != NULL) {
		iounmap(rct_io_base);
		rct_io_base = NULL;
	}

	inter = (ctrl >> 24) & 0x7F;
	sdiv = (ctrl >> 12) & 0xF;
	sdout = (ctrl >> 16) & 0xF;

	value = REF_CLK_FREQ;
	value *= (sdiv + 1);
	value *= ((1UL << 32) * (inter + 1) + frac);

	value /= (1UL << 32);
	value /= (sdout + 1);
	value /= (1 + ((prescale >> 4) & 0xF));
	value /= (1 + ((postscale >> 4) & 0xF));

	prt_info("Audio Clock: %llu Hz\n", value);

	return value;
}

int cavalry_get_audio_clk(struct ambarella_cavalry *cavalry, void __user *arg)
{
	uint64_t audio_clk = 0;
	int rval = 0;

	audio_clk = cavalry->audio_clk;
	if (copy_to_user(arg, &audio_clk, sizeof(audio_clk))) {
		rval = -EFAULT;
	}

	return rval;
}

int cavalry_misc_init(struct ambarella_cavalry *cavalry)
{
	cavalry->audio_clk = get_audio_pll();
	return 0;
}

int cavalry_misc_exit(struct ambarella_cavalry *cavalry)
{
	return 0;
}
