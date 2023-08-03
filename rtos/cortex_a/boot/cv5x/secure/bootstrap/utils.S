/**
 * History:
 *    2019/01/31 - [Cao Rongrong]
 *
 * Copyright (c) 2016 Ambarella International LP
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
#if 0
#include <ambhw/drctl.h>
#include <ambhw/rct.h>
#include <ambhw/gpio.h>
#include <ambhw/wdt.h>
#include <bst_macro.S>

.weak bsp_hw_index
.weak bsp_hw_init
.globl params_load

.globl trigger_uinst
.globl set_dram_mode
.globl set_init_ctl
.globl gpio_pin_check
.globl copy_loop
#endif
#include "AmbaBST.h"
#include "AmbaCSL_DDRC.h"
#include <bst_macro.S>
#define WDT_BASE    AMBA_CORTEX_A76_WDT_BASE_ADDR
.globl spad_result_backup
.globl spad_result_restore
.globl rct_timer_delay

.weak enable_watchdog
.weak disable_watchdog
.globl axi_clk_phase_test

#if 0
/**
 * Get hardware index to select proper parameters later.
 * This function can be overwritten in bsp_bst.S in bsp dir.
 *
 * !!! WARNING: Invalid index will hangup the boot.
 */
bsp_hw_index:
	mov	w0, #0
	ret


/**
 * Init board hardware
 */
bsp_hw_init:
	ret

#if defined(CONFIG_AMBST_MULTI_DRAM_INI_SUPPORT)
/**
 * Select and load dram paramters based on hw index
 */
params_load:
	mov	x28, x30
	bl	bsp_hw_index
	cbz	w0, 1f
	ldr	w2, =__params_multi_start
	ldr	w1, __params_multi_size
	madd	w0, w0, w1, w2
	add	w1, w1, w0
	bl	copy_loop
1:	ret	x28

#else

params_load:
	ret

#endif


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


/*
 * gpio_pin_check: check pin state
 * input: w0: input gpio pin addr
 * output: w0=0,pin=low;  else pin=high
 * crob w1,w2
 */
gpio_pin_check:
	bic	x1, x0, #0x1f
	orr	x1, x1, #0x2000000000			/* gpio bank base */
	and	w2, w0, #0x1f				/* gpio offset in this bank */
	mov	w0, #0x1
	str	w0, [x1, #GPIO_ENABLE_OFFSET]		/* Enable */
	lsl	w0, w0, w2				/* w0 = 1 << w2 */
	str	w0, [x1, #GPIO_MASK_OFFSET]		/* Mask: data accessible */
	ldr	w2, [x1, #GPIO_DIR_OFFSET]		/* Direction: input */
	bic	w2, w2, w0
	str	w2, [x1, #GPIO_DIR_OFFSET]
	ldr	w0, [x1, #GPIO_DATA_OFFSET]
	ret

#endif

.section .text.second_stage

/**
 * !!! WARNING: x0, x2 should be aligned to 8-byte.
 *
 *   x0 - source start address
 *   x1 - source end address
 *   x2 - destination address
 *
 *   Garbles x3 - x4
 */
copy_loop:
	ldp	x3, x4, [x0], #16
	stp	x3, x4, [x2], #16
	cmp	x0, x1
	b.lt	copy_loop
	ret


/**
 * rct_timer_delay:
 *
 * x0 - expected to have count for delay
 * crob x1,x2
 */
rct_timer_delay:
	mov_imm	x2, RCT_BASE
	mov	w1, #0x1
	str	w1, [x2, #RCT_TIMER_CTRL_OFFSET]
	str	wzr, [x2, #RCT_TIMER_CTRL_OFFSET]
rct_timer_read:
	ldr	w1, [x2, #RCT_TIMER_OFFSET]
	cmp	w0, w1
	bhi	rct_timer_read
	ret

.section .text.first_stage
dram_rsvd_space_param:          .int 0x1ff00000

#if defined(CONFIG_AMBST_CV5_REBOOT_WORKAROUND)


/**
 * initialize and enable internal watchdog.
 * it can be overwritten in bsp_bst.S if using
 * external watchdog instead.
 */
enable_watchdog:
	/* configure WDT reset pin */
	mov_imm	x1, IOMUX_BASE
	ldr	w0, [x1, #0x00]
	bic	w0, w0, #0x1
	str	w0, [x1, #0x00]
	ldr	w0, [x1, #0x04]
	orr	w0, w0, #0x1
	str	w0, [x1, #0x04]
	ldr	w0, [x1, #0x08]
	bic	w0, w0, #0x1
	str	w0, [x1, #0x08]
	mov	w0, #0x1
	str	w0, [x1, #0xf0]
	str	wzr, [x1, #0xf0]
	/* initialize and enable WDT */
	mov_imm	x1, WDT_BASE
	mov	w0, #0x100000
	str	w0, [x1, #0x18]
	str	w0, [x1, #0x04]
	mov	w0, #0x4755
	str	w0, [x1, #0x08]
	mov	w0, #0x9
	str	w0, [x1, #0x0c]
	ret

#elif !defined(CONFIG_WDT_PIN_NON)

/**
 * disable internal watchdog
 * it can be overwritten in bsp_bst.S if using
 * external watchdog instead.
 */
disable_watchdog:
	mov_imm	x1, WDT_BASE
	str	wzr, [x1, #0x0c]
	ret


/**
 * check if axi clock is out of phase
 * crob x0,x1,x2
 */
axi_clk_phase_test:
	mov	x29, x30

	mrs	x4, vbar_el3		/* save vbar_el3 */
	/* bl	enable_watchdog */

	ldr	w3, dram_rsvd_space_param
	adr	x2, axi_clk_ret
	mov_imm	x1, 0x20ee000000	/* Exception for Current EL with ELx_SP */
	ldr	w0, =0xd51e4022		/* msr	elr_el3, x2 */
	str	w0, [x1, #0x200]
	ldr	w0, =0xd69f03e0		/* eret */
	str	w0, [x1, #0x204]
	msr	vbar_el3, x1
	dsb	sy
	isb

	mov	w1, #8
axi_clk_retry:
	str	wzr, [x19, #0x10]	/* program dram axi0 credit */
	str	wzr, [x19, #0x14]	/* program dram axi1 credit */
	mov	w0, #0x20000000		/* program dram axi0 priority */
	str	w0, [x19, #0x110]
	mov	w0, #0x0000ffff		/* program dram bank throttle */
	str	w0, [x19, #0x220]
	isb
	blr	x3

axi_clk_ret:
	ic	ialluis
	isb	sy
	sub	w1, w1, #1
	cbnz	w1, axi_clk_retry

	bl	disable_watchdog
	msr	vbar_el3, x4		/* restore vbar_el3 to original value */

	ret	x29

#else

axi_clk_phase_test:
	ret

#endif

.section .text.second_stage

/**
 * This function backup the content of DDRCT_RESULT (0x20e0033f80~0x20e0033fff) to DRAM parameter location.
 * It is because 2nd BST 8K will be loaded and copy to 0x20e0032000~0x20e0033fff.
 * The DRAM parameter was been used in DRAM init function which executed in 1st BST, only some config/MR registers are still need in training.
 * So the DLL setting and delay parameters should be safe to be overwritten for backup purpose.
 */
spad_result_backup:
    /* Backup scratch pad result to DRAM parameter because the next load will overwrite it */
    ldr x0, =DDRCT_RESULT_SPACE
    add x1, x0, #DDRCT_RESULT_SIZE
    ldr x2, =DLL_Setting0D1Param_Host1
    bic x2, x2, #0x7    /* Make sure we are 8 bytes align */
    b copy_loop

/**
 * This function restore the content of DDRCT_RESULT (0x20e0033f80~0x20e0033fff) from DRAM parameter location
 */
spad_result_restore:
    /* Restore scratch pad result from BSS */
    ldr x0, =DLL_Setting0D1Param_Host1
    bic x0, x0, #0x7    /* Make sure we are 8 bytes align */
    add x1, x0, #DDRCT_RESULT_SIZE
    ldr x2, =DDRCT_RESULT_SPACE
    b copy_loop

