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
#include <ambhw/scratchpad.h>
#endif
#include "AmbaBST.h"
#include <bst_regs.h>

#define SAVE_TRAINING_RESULT		0
#define USE_UNIQUE_DATA_PATTERN		0

#define UPDATE_WRITE_DLL            0
#define COARSE_SEARCH_STEP          1   // We enable bigger search step here to speed up, but this needs more testing.

#if COARSE_SEARCH_STEP
#define SEARCH_STEP                 2
#else
#define SEARCH_STEP                 1
#endif

.text

.globl	dram_training
.globl	ddrc_uinst


/****************************************************************************
*  LPDDR4 training for rddly, rddll, vref, wrdly and lp4vref.
*
*  x0 - x19: tmp used
*  x20 - DRAMC_DDRC_BASE
*
*****************************************************************************/

dram_training:
	mov	x25, x30

	ldr	w7, training_param
	tbz	w7, #0, exit

#if SAVE_TRAINING_RESULT
	/* clear ScratchPad1 RAM */
	ldr	w6, =AHB_SP1_RAM_BASE
	mov	x0, #4096
0:	sub	x0, x0, #8
	str	xzr, [x6, x0]
	cbnz	x0, 0b
#endif
	/* init ScratchPad Reg for lp4vref */
	ldr	w0, =0xff00ff00
	str	w0, [x20, #DRAM_LPDDR4_SCRATCHPAD]

	/* Enter into training mode */
	ldr	w0, [x20, #DRAM_TIMING5_LP4TRAIN]
	orr	w0, w0, #(1 << 24)	/* enable LPDDR4 Training Mode */
	str	w0, [x20, #DRAM_TIMING5_LP4TRAIN]
1:	ldr	w0, [x20, #DRAM_TIMING5_LP4TRAIN]
	tbz	w0, #25, 1b

	mov	w14, #0xff00		/* end of best vref */

	mov	w10, #4			/* byte index: byte3, byte2, byte1, or byte0 */
	ldr	w0, [x20, #DRAM_CFG]
	tbz	w0, #8, do_training
	mov	w10, #2			/* byte index for 16bit bus width */
do_training:
	sub	w10, w10, #1
	bl	training_prepare
	bl	training_read_delay
	bl	training_read_dll
	bl	training_read_vref
	bl	training_write_delay
	bl	training_write_vref
	cbnz	w10, do_training	/* training next byte */

	/* Now stop training */
	ldr	w1, [x20, #DRAM_TIMING5_LP4TRAIN]
	bic	w1, w1, #(1 << 24)	/* disable LPDDR4 Training Mode */
	str	w1, [x20, #DRAM_TIMING5_LP4TRAIN]

	mov	w0, #0x6
	ldr	w1, [x20, #DRAM_CTL]
	bfi	w1, w0, #28, #4
	bic	w2, w1, #0x1		/* disable dram access */
	str	w2, [x20, #DRAM_CTL]

	mov	w0, #4			/* update RTT (Round-Trip-Time */
	str	w0, [x20, #DRAM_INIT_CTL]
2:	ldr	w0, [x20, #DRAM_INIT_CTL]
	tbnz	w0, #2, 2b

    /* Clear ScratchPad for BLD training */
	str	wzr, [x20, #DRAM_LPDDR4_SCRATCHPAD]

	str	w1, [x20, #DRAM_CTL]	/* enable dram access */

exit:
	ret	x25



training_read_delay:
	mov	x26, x30

	tbz	w7, #3, 1f

	mov	w19, wzr		/* the max range of global working vref  */

	mov	w9, #0x10		/* training rddly */
read_delay_loop:
	sub	w9, w9, #SEARCH_STEP
	bl	set_rddly
	bl	training_vref_inner
	cbnz	w9, read_delay_loop	/* training next rddly */

#if SAVE_TRAINING_RESULT
	bl	save_split_line
#endif

	/* update read delay register to the best rddly: (w16 + w17) / 2 */
	add	w9, w16, w17
	lsr	w9, w9, #1
	/* adjusted by offset */
	sbfx	w0, w7, #12, #4
	add	w9, w9, w0
	bl	set_rddly

1:	ret	x26



training_read_dll:
	mov	x26, x30

	tbz	w7, #3, 1f

	mov	w19, wzr		/* the max range of global working vref  */

	mov	w9, #24			/* training rddll */
read_dll_loop:
	sub	w9, w9, #SEARCH_STEP
	bl	set_dll
	bl	training_vref_inner
	cbnz	w9, read_dll_loop	/* training next rddll */

#if SAVE_TRAINING_RESULT
	bl	save_split_line
#endif

	/* update read DLL register to the best rddll: (w16 + w17) / 2 */
	add	w9, w16, w17
	lsr	w9, w9, #1
	bl	set_dll

1:	ret	x26



training_read_vref:
	mov	x26, x30

	tbz	w7, #2, 1f

	mov	w19, wzr		/* the max range of global working vref  */

#if SAVE_TRAINING_RESULT
	mov	w9, 0xbb		/* meaningless, just for marker when save */
#endif
	bl	training_vref_inner

	/* clamp each byte's vref start and end */
	mov	w0, w14
	bl	update_global_range
	mov	w14, w0

	/* update vref register to the best vref: (w2 + w3) / 2 */
	add	w8, w2, w3
	lsr	w8, w8, #1
	/* adjusted by offset */
	sbfx	w0, w7, #24, #8
	add	w8, w8, w0
	bl	set_vref

#if SAVE_TRAINING_RESULT
	bl	save_split_line
#endif

1:	ret	x26



training_vref_inner:
	mov	x27, x30

	mov	w18, wzr		/* the range of working vref  */

	mov	w8, #0x30		/* training vref */
vref_loop:
	sub	w8, w8, #SEARCH_STEP
	bl	set_vref

	ubfx	w15, w7, #5, #3		/* the number of repetition */
0:
	mov	w2, #0x3		/* MPC-Read DQ Calibration */
	bl	check_read_fifo
	cbnz	x0, 2f			/* branch out to training next vref if failed */

	sub	w15, w15, #1
	cbnz	w15, 0b

	/*
	 * update the working vref range for current rddly or rddll, and then also
	 * update the range of best rddly or rddll to w16(start) and w17(end).
	 */
	bl	update_best

2:	cbz	w0, 3f
	mov	w18, wzr		/* reset the range of working vref if failed */

3:	cbnz	w8, vref_loop		/* training next vref */

	ret	x27




training_write_delay:
	mov	x26, x30

	tbz	w7, #4, 1f

	mov	w19, wzr		/* the max range of global working lp4vref  */

	mov	w9, #0x40		/* training wrdly */
write_delay_loop:
	sub	w9, w9, #SEARCH_STEP
	bl	set_wrdly
	bl	training_lp4vref_inner
	cbnz	w9, write_delay_loop	/* training next wrdly */

#if SAVE_TRAINING_RESULT
	bl	save_split_line
#endif

	/* update write delay register to the best wrdly: (w16 + w17) / 2 */
	add	w9, w16, w17
	lsr	w9, w9, #1
	/* adjusted by offset */
	sbfx	w0, w7, #8, #4
	add	w9, w9, w0
	bl	set_wrdly

1:	ret	x26



training_write_vref:
	mov	x26, x30

	tbz	w7, #4, 3f

	mov	w19, wzr		/* the max range of global working lp4vref  */

#if SAVE_TRAINING_RESULT
	mov	w9, 0xcc		/* meaningless, just for marker when save */
#endif
	bl	training_lp4vref_inner

	/* clamp each channel's lp4vref start and end, then save to ScratchPad Reg */
	ldr	w4, [x20, #DRAM_LPDDR4_SCRATCHPAD]

	and	w1, w10, #2
	lsl	w1, w1, #3		/* w1 = 16 or 0 */
	lsr	w0, w4, w1

	/* update global range of lp4vref for each channel */
	bl	update_global_range

	tbnz	w10, #1, 1f
	bfi	w4, w0, #0, #16
	b	2f
1:	bfi	w4, w0, #16, #16
2:	str	w4, [x20, #DRAM_LPDDR4_SCRATCHPAD]

	/* update lp4vref to the best lp4vref: (w2 + w3) / 2 */
	add	w8, w2, w3
	lsr	w8, w8, #1
	/* adjusted by offset */
	sbfx	w0, w7, #16, #8
	add	w8, w8, w0
	cmp	w8, #0
	csel	w8, wzr, w8, lt
	bl	set_lp4vref

#if SAVE_TRAINING_RESULT
	bl	save_split_line
#endif

3:	ret	x26



training_lp4vref_inner:
	mov	x27, x30

	mov	w18, wzr		/* the range of working lp4vref  */

	mov	w8, #(80+SEARCH_STEP)	/* training lp4vref */
lp4vref_loop:
	sub	w8, w8, #SEARCH_STEP
	bl	set_lp4vref

	ubfx	w15, w7, #5, #3		/* the number of repetition */
0:
	mov	w2, #0x1		/* MPC-Write FIFO */
	mov	w1, #0x0
	mov	w3, #5
1:	bl	ddrc_uinst
	sub	w3, w3, #1
	cbnz	w3, 1b

	mov	w2, #0x2		/* MPC-Read FIFO */
	bl	check_read_fifo
	cbnz	x0, 2f			/* branch out if current training is failed */

	sub	w15, w15, #1
	cbnz	w15, 0b

	/*
	 * update the working lp4vref range for current wrdly, and then also
	 * update the range of best wrdly to w16(start) and w17(end).
	 */
	bl	update_best

2:	cbz	w0, 3f
	mov	w18, wzr		/* reset the range of working lp4vref if failed */

3:	cbnz	w8, lp4vref_loop	/* training next lp4vref */

	ret	x27



training_prepare:
	mov	x29, x30

	/* select channel: byte3/2 belong to channel B, byte1/0 belong to channel A */
	ldr	w0, [x20, #DRAM_CTL]
	lsr	w1, w10, #1
	bfi	w0, w1, #31, #1

	/*
	 * select rank
	 *
	 * NOTE:
	 *   if it's dual rank chip, we need to modify bit[30:29] accordingly.
	 */
	bic	w0, w0, #(1 << 30)
	orr	w0, w0, #(1 << 29)
	str	w0, [x20, #DRAM_CTL]

	/* Switch to new frequency set point: hardcode set FSP-OP, FSP-WR */
	mov	w0, #13
	mov	w1, #0xc0
	bl	ddrc_mrw

#if 0
	/* the default value of MR15/MR20 should be 0x55 */
	mov	w0, #15
	mov	w1, #0x55
	bl	ddrc_mrw
	mov	w0, #20
	mov	w1, #0x55
	bl	ddrc_mrw
#endif

#if USE_UNIQUE_DATA_PATTERN
	/* configure MR32/MR40 to get unique data pattern.*/
	mov	w0, #32
	mov	w1, #0xaa
	bl	ddrc_mrw
	mov	w0, #40
	bl	ddrc_mrw

	/* Write the expected data into WRITE_FIFO_BASE_REG */
	ldr	x0, =0xaaaa5555aaaa5555
	stp	x0, x0, [x20, #(DRAM_MPC_WRITE_DATA + 0x00)]
	stp	x0, x0, [x20, #(DRAM_MPC_WRITE_DATA + 0x10)]
#else
	/* Write the expected data into WRITE_FIFO_BASE_REG */
	ldr	w0, =0xaaaaaaaa
	ldr	w1, =0x5555aaaa
	mvn	w2, w0			/* w2 = 0x55555555 */
	mvn	w3, w1			/* w3 = 0xaaaa5555 */
	stp	w1, w2, [x20, #(DRAM_MPC_WRITE_DATA + 0x00)]
	stp	w0, w0, [x20, #(DRAM_MPC_WRITE_DATA + 0x08)]
	stp	w2, w3, [x20, #(DRAM_MPC_WRITE_DATA + 0x10)]
	stp	w3, w1, [x20, #(DRAM_MPC_WRITE_DATA + 0x18)]
#endif

#if UPDATE_WRITE_DLL
	mvn	w15, wzr		/* flag indicating to upate write dll */
#else
	mov	w15, wzr		/* Clear flag to avoid write dll update */
#endif
	mov	w9, #12
	bl	set_dll

	ret	x29



/* send micro-instruction to lpddr4: w0 - addr, w1 - data, w2 - op */
ddrc_mrw:
	mov	w2, #0x4		/* MRW */
ddrc_uinst:
	cmp	w2, #0x4
	csel	w0, wzr, w0, ne
	orr	w0, w1, w0, lsl #8
	orr	w0, w0, w2, lsl #28
	str	w0, [x20, #DRAM_UINST]
1:	ldr	w0, [x20, #DRAM_TRAINING_COMMAND_DONE]
	tbnz	w0, #0, 1b
	ret


#if USE_UNIQUE_DATA_PATTERN

check_read_fifo:
	mov	x29, x30

	/*
	 * it seems vref need some time to take effect, so
	 * the delay (about 200ns) is must have.
	 */
	mov	w0, #5
	bl	rct_timer_delay

	mov	w1, #2			/* issue 10 MPC */
	bl	ddrc_uinst

	and	w3, w10, #1		/* each channel has 16bit data */
	lsl	w3, w3, #3

	mov	x2, #DRAM_MPC_READ_DATA
1:	ldr	w0, [x20, x2]
	lsr	w0, w0, w3
	ldr	w1, =0x00ff00ff
	and	w0, w0, w1
	ldr	w1, =0x00aa0055
	eor	w0, w0, w1
	cbnz	w0, 2f
	add	x2, x2, #4
	cmp	x2, #(DRAM_MPC_READ_DATA + 0x20)
	blo	1b

2:	ret	x29

#else

compare_data:
	ldr	w1, [x3], #4
	ldr	w0, [x2], #4
	eor	w0, w0, w1
	and	w1, w10, #1		/* each channel has 16bit data */
	lsl	w1, w1, #3		/* w1= 8 or 0 */
	lsr	w0, w0, w1
	ldr	w1, =0x00ff00ff
	and	w0, w0, w1
	ret



check_read_fifo:
	mov	x29, x30

	/*
	 * it seems vref need some time to take effect, so
	 * the delay (about 200ns) is must have.
	 */
	mov	w0, #5
	bl	rct_timer_delay

	mov	w1, #2			/* issue 10 MPC */
	bl	ddrc_uinst

	add	x4, x20, #DRAM_MPC_WRITE_DATA
	add	x3, x20, #DRAM_MPC_WRITE_DATA

1:	add	x2, x20, #DRAM_MPC_READ_DATA
	cmp	x2, x3
	beq	4f
	bl	compare_data
	cbnz	w0, 1b

2:	add	x1, x20, #(DRAM_MPC_READ_DATA + 0x20)
	cmp	x1, x2
	beq	4f

	add	x0, x20, #DRAM_MPC_READ_DATA
	cmp	x0, x3
	bne	3f
	add	x3, x20, #DRAM_MPC_WRITE_DATA

3:	bl	compare_data
	cbz	w0, 2b

	add	x4, x4, #4
	mov	x3, x4
	b	1b

4:	ret	x29

#endif



set_dll:
	lsl	w3, w10, #2
	add	w2, w3, #(DDRIO0_DLL_STATUS_SEL0 + 2)
	ldrb	w2, [x20, x2]
	sub	w0, w2, w9
	orr	w0, w0, #0x20
	subs	w1, w9, w2
	csel	w0, w0, w1, lo
	add	w2, w3, #DDRIO0_DLL0_REG
	ldr	w1, [x20, x2]
	bfi	w1, w0, #0, #8
	tbz	w15, #31, 1f
	bfi	w1, w0, #16, #8
1:	str	w1, [x20, x2]
	ret



set_rddly:
	mov	w0, #5
	mul	w0, w0, w10
	mov	w1, #0xf
	lsl	w1, w1, w0
	lsl	w2, w9, w0
	ldr	w0, [x20, #DRAM_LPDDR4_DQ_READ_DELAY]
	bic	w0, w0, w1, lsl #1
	orr	w0, w0, w2, lsl #1
	str	w0, [x20, #DRAM_LPDDR4_DQ_READ_DELAY]
	ret



set_vref:
	orr	w0, w8, w8, lsl #7
	orr	w0, w0, w0, lsl #14
	str	w0, [x20, #DRAM_LPDDR4_DQCA_VREF]
	ret



set_lp4vref:
	mov	x29, x30
	sub	w1, w8, #30
	orr	w1, w1, #0x40
	cmp	w8, #50
	csel	w1, w1, w8, gt
	mov	w0, #14
	bl	ddrc_mrw
	ret	x29



set_wrdly:
	mov	w2, #7
	mul	w2, w2, w10
	mov	w1, #0x7f
	lsl	w1, w1, w2
	lsl	w2, w9, w2
	ldr	w0, [x20, #DRAM_LPDDR4_DQ_WRITE_DELAY]
	bic	w0, w0, w1
	orr	w0, w0, w2
	str	w0, [x20, #DRAM_LPDDR4_DQ_WRITE_DELAY]
	ret


update_global_range:
	/*
	 * w0 saves the packed range of global vref or lp4vref:
	 *    (start << 8) | end
	 */
	ubfx	w2, w0, #0, #8		/* end of saved vref or lp4vref */
	ubfx	w3, w0, #8, #8		/* start of saved vref or lp4vref */

	lsr	w0, w19, #16		/* end of working vref or lp4vref */
	and	w1, w19, #0xffff
	sub	w1, w1, #SEARCH_STEP
	add	w1, w1, w0		/* start of working vref or lp4vref */

	cmp	w0, w2
	csel	w2, w0, w2, hi
	cmp	w1, w3
	csel	w3, w1, w3, lo
	/* pack the range again */
	orr	w0, w2, w3, lsl #8

	ret



/* PS: don't corrupt w0/x0 */
update_best:
	mov	x28, x30

	/* update current range of vref or lp4vref if training is succesful */
	bfi	w18, w8, #16, #16
	add	w18, w18, #SEARCH_STEP

#if SAVE_TRAINING_RESULT
	bl	save_result
#endif

	and	w2, w18, #0xffff	/* current vref or lp4vref range */
	and	w1, w19, #0xffff	/* previous max vref or lp4vref range */
	cmp	w2, w1			/* branch out if current range is smaller */
	blo	1f

	csel	w16, w16, w9, eq	/* update the start of best rddly, rddll or wrdly */
	mov	w17, w9			/* update the end of best rddly, rddll or wrdly */

	/* update the range of global vref or lp4vref */
	mov	w19, w18

1:	ret	x28


/**
 * rct_timer_delay:
 *
 * x0 - expected to have count for delay
 * x1 - used
 * x23 - RCT address
 */
rct_timer_delay:
	mov	w1, #0x1
	str	w1, [x23, #RCT_TIMER_CTRL_OFFSET]
	str	wzr, [x23, #RCT_TIMER_CTRL_OFFSET]
rct_timer_read:
	ldr	w1, [x23, #RCT_TIMER_OFFSET]
	cmp	w0, w1
	bhi	rct_timer_read
	ret

#if SAVE_TRAINING_RESULT

/* PS: don't corrupt w0/x0 */
save_result:
0:	ldrb	w2, [x6]
	cmp	w9, w2
	bhs	1f
	add	x6, x6, #3
1:	strb	w9, [x6]

	ldrb	w2, [x6, #1]
	cmp	w8, w2
	blo	2f
	strb	w8, [x6, #1]

2:	ldrb	w2, [x6, #2]
	add	w1, w8, #1
	cmp	w1, w2
	add	x6, x6, #3
	blo	0b
	sub	x6, x6, #3
	strb	w8, [x6, #2]

	ret


/* PS: don't corrupt w0/x0 */
save_split_line:
	add	x6, x6, #3
	mvn	w1, wzr
	strb	w1, [x6], #1		/* split line */
	ret

#endif


