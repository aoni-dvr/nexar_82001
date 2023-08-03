/**
 * History:
 *    2019/01/31 - [Cao Rongrong]
 *    2021/10/01 - [Jorney] update
 *
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
#include <arch_cv5/drctl.h>

.globl trigger_uinst
.globl set_dram_mode
.globl set_init_ctl

/*
 * Set value to dram mode register
 * x0 - tmp
 * x1 - uinst1
 * x2 - uinst2
 * x4 - uinst4
 * x5 - uinst5
 * x20 - DRAMC_DDRC_BASE
 */
trigger_uinst:
	str	w1, [x20, #DRAM_UINST1]
	str	w2, [x20, #DRAM_UINST2]
	str	w4, [x20, #DRAM_UINST4]
	mov	w0, #1
	str	w0, [x20, #DRAM_UINST5]
1:	ldr	w0, [x20, #DRAM_UINST5]
	tbnz	w0, #0, 1b	/* check UINST_ENABLE */
	ret


/*
 * Set value to dram mode register
 * x0 - value to set
 * x1 - tmp
 * x20 - DRAMC_DDRC_BASE
 */
set_dram_mode:
	str	w0, [x20, #DRAM_MODE_REG]
1:	ldr	w1, [x20, #DRAM_MODE_REG]
	tbnz	w1, #31, 1b	/* check DRAM_MSB_BUSY */
	ret


/*
 * Set dram init control register
 * x0 - value to set
 * x1 - tmp
 * x20 - DRAMC_DDRC_BASE
 */
set_init_ctl:
	str	w0, [x20, #DRAM_INIT_CTL]
1:	ldr	w1, [x20, #DRAM_INIT_CTL]
	and	w1, w0, w1
	bic	w1, w1, #DRAM_INIT_CTL_RTT_DIE
	cbnz	w1, 1b
	ret

set_dram_pll:
	

