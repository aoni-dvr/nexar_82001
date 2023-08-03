/**
 *  @file AmbaDdrc_DLL_Training_A64.asm
 *
 *  Copyright (c) 2020 Ambarella International LP
 *
 *  This file and its contents ("Software") are protected by intellectual property rights including, without limitation,
 *  U.S. and/or foreign copyrights.  This Software is also the confidential and proprietary information of Ambarella, Inc.
 *  and its licensors.  You may not use, reproduce, disclose, distribute, modify, or otherwise prepare derivative works
 *  of this Software or any portion thereof except pursuant to a signed license agreement or nondisclosure agreement with
 *  Ambarella, Inc. or its authorized affiliates.  In the absence of such an agreement, you agree to promptly notify and
 *  return this Software to Ambarella, Inc.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 *  AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details Dram DLL Training Code
 *
 */

#include "AmbaCortexA53.h"
#include "AmbaDramTrain.h"

#define BLD_TESTADDR        (CONFIG_FWPROG_BLD_LOADADDR + 0x100000)
#define SAVE_TUNING_RESULT  0

.section .lpddr4_traning_seq_stage_read
.global ddrc_train_read_start

.Align(3)
ddrc_train_read_start:
   /* initialize stack and the log space in scratch pad */
   ldr x24, =AHB_SCRATCH_PAD_LOG_SPACE
   mov sp, x24
   mov w12, #0
   str w12, [x24, #4] /* clear the temp word */
   str w12, [x24, #8] /* clear the bit count */
   add w12, w24, #0xc
   str w12, [x24] /* write pointer */

   ldr w20, =AMBA_CORTEX_A53_DDR_CTRL0_BASE_ADDR

   bl dram_training

   LDR     X1, =AMBA_RAM_DRAM_TRAINING_STAGE2_OFFSET
   BR      X1

/****************************************************************************
*  DDR4 tuning for Read/Sync/Write DLl.
*
*  PS: there are 3 types of dram data test: solidbits, bitflip and aa55,
*      but perpaphs only one test is enough to verify the dram.
*
*  x0 - x4, x6 - x9: tmp used
*
*  x11 - byte index: byte3, byte2, byte1, or byte0
*  x12 - byte0/byte1/byte2/byte3 DLL register offset
*  x13 - Read/Sync/Write DLL bit offset
*
*  x14 - DLL lock position
*  x15 - the original DLL lock position
*  x16 - the start DLL lock position
*
*  x20 - DRAMC_DDRC_BASE
*  x21 - Self Refresh flag
*
*****************************************************************************/

dram_training:
	mov	x25, x30

	/* ldr	w7, training_param */
	mov	w7, #5          /* training_param is set to 5 by default */
	cbz	w7, exit

	/*
	 * currently we don''t support SR with dram tuning,
	 * so if dram is in SR, we will exit directly.
	 */
#if 0     /* We don't have GPIO to support Self-Refresh yet */
	tbnz	w21, #0, exit
#endif

#if SAVE_TUNING_RESULT
	/*ldr	w6, =AHB_SP1_RAM_BASE*/
	add	w6, w24, #0x10
#endif

	/* now start tuning */
	mov	w11, #4			/* byte index for 32bit bus width */
	ldr	w0, [x20, #DDRC_CONFIG_REG]
	tbz	w0, #8, do_tuning
	mov	w11, #2			/* byte index for 16bit bus width */
do_tuning:
	sub	w11, w11, #1
	lsl	w12, w11, #2
	bl	dram_tuning_dll
	cbnz	w11, do_tuning		/* training next byte */

exit:
	ret	x25



/*********************************************************/


dram_tuning_dll:
	mov	x26, x30

tuning_read_dll:
	tbz	w7, #0, tuning_sync_dll
	mov	w13, #0
	bl	dram_tuning_dll_sel

tuning_sync_dll:
#if 0
	tbz	w7, #1, tuning_write_dll
	mov	w13, #8
	bl	dram_tuning_dll_sel
#endif

tuning_write_dll:
	tbz	w7, #2, tuning_dll_done
	mov	w13, #16
	bl	dram_tuning_dll_sel

tuning_dll_done:
	ret	x26



/*********************************************************/


/**
 * x14 - DLL lock position
 * x15 - the original DLL lock position
 * x16 - the start DLL lock position
 */
dram_tuning_dll_sel:
	mov	x27, x30

	add	w2, w12, #HOST0_DLL0_REG
	ldr	w0, [x20, x2]		/* read DDRIO0_DLLx_REG */
	lsr	w2, w0, w13
	ubfx	w1, w2, #0, #5		/* get SEL0, SEL1 or SEL2 */

	/* calculate DLL lock position */
	add	w0, w12, #(DDRIO_DLL_STATUS_BASE + 2)
	ldrb	w0, [x20, x0] 		/* vfine value */
	add	w14, w0, w1
	tbz	w2, #5, 1f
	sub	w14, w0, w1
1:	cmp	w14, #0
	csel	w14, w14, wzr, ge
	mov	w0, #0x1f
	cmp	w14, #0x1f
	csel	w14, w14, w0, lt

	mov	w15, w14		/* save the original DLL lock position */

	/* the range of DLL lock position is [0, 0x1f] */
0:	cmp	w14, #0
	blt	1f

	bl	set_dll
	bl	dram_verify_data
	cbnz	w0, 1f			/* branch out if data error */

	sub	w14, w14, #1		/* decrease the DLL lock position and test again */
	b	0b

1:	add	w14, w14, #1
	mov	w16, w14		/* save the "start" DLL lock position */
	mov	w14, w15		/* load the original DLL lock position */

2:	cmp	w14, #0x1f
	bgt	3f

	bl	set_dll
	bl	dram_verify_data
	cbnz	w0, 3f			/* branch out if data error */

	add	w14, w14, #1		/* increase the DLL lock position and test again */
	b	2b

3:	sub	w14, w14, #1

#if SAVE_TUNING_RESULT
	strh	w16, [x6], #2		/* save the "start" DLL lock position to ScratchPad RAM */
	strh	w14, [x6], #2		/* save the "end" DLL lock position to ScratchPad RAM */
	strh	w11, [x6], #2
	strh	w13, [x6], #2
#endif

	/*
	 * the final DLL lock position is:
	 *   if (w16 == 0x00) && (w14 < 0x1f)
	 *      DLL = w14 / 4
	 *   else
	 *      DLL = (w16 + w14) / 2
	 *
	 * PS: we choose this rule according to experience.
	 */
	add	w0, w14, w16
	cbnz	w16, 1f
	cmp	w14, #0x1f
	beq	1f
	lsr	w0, w0, #1
1:	lsr	w14, w0, #1

	/* adjusted rddll or wrdll by offset */
	sbfx	w0, w7, #8, #8
	cbz	w13, 2f
	sbfx	w0, w7, #16, #8
2:	add	w14, w14, w0
	bl	set_dll

	ret	x27


/*********************************************************/


dram_verify_data:
	mov	x28, x30

	bl	solidbits_test
	cbnz	w0, 1f

	bl	bitflip_test
	cbnz	w0, 1f

	bl	aa55_test
	cbnz	w0, 1f

1:	ret	x28



/*********************************************************/


/**
 * x12 - byte0/byte1/byte2/byte3 DLL register offset
 * x13 - Read/Sync/Write DLL bit offset
 * x14 - DLL lock position
 */
set_dll:
	add	w0, w12, #(DDRIO_DLL_STATUS_BASE + 2)
	ldrb	w0, [x20, x0]		/* vfine value */
	sub	w1, w0, w14
	orr	w1, w1, #0x20
	sub	w2, w14, w0
	cmp	w14, w0
	csel	w1, w1, w2, lt

	lsl	w1, w1, w13		/* bit shift for SEL value */
	mov	w2, #0xff
	lsl	w2, w2, w13		/* bit shift for SEL mask */

	add	w3, w12, #HOST0_DLL0_REG
	ldr	w0, [x20, x3]		/* read DDRIO0_DLLx_REG */
	bic	w0, w0, w2
	orr	w0, w0, w1
	str	w0, [x20, x3]
	ret



/*********************************************************/


solidbits_test:
	mov	x29, x30

	ldr	w8, =BLD_TESTADDR	/* address 0 */
	add	w9, w8, #0x800		/* address 1 */
	add	w4, w8, #0x800

	mvn	x0, xzr
	mov	x1, xzr
1:
	stp	x0, x1, [x8], #16
	stp	x0, x1, [x9], #16
	cmp	w8, w4
	blo	1b

	bl	compare_dram_data

	ret	x29



/*********************************************************/


bitflip_test:
	mov	x29, x30

	ldr	w8, =BLD_TESTADDR	/* address 0 */
	add	w9, w8, #0x800		/* address 1 */
	add	w4, w8, #0x800

	mov	x2, #0x8000000000000000
1:
	mov	x0, x2
	mvn	x1, x0
	stp	x0, x1, [x8], #16
	stp	x0, x1, [x9], #16
	ror	x2, x2, #1
	cmp	w8, w4
	blo	1b

	bl	compare_dram_data

	ret	x29



/*********************************************************/


aa55_test:
	mov	x29, x30

	ldr	w8, =BLD_TESTADDR	/* address 0 */
	add	w9, w8, #0x800		/* address 1 */
	add	w4, w8, #0x800

	ldr	x0, =0x5555555555555555
	mvn	x1, x0
1:
	stp	x0, x1, [x8], #16
	stp	x0, x1, [x9], #16
	cmp	w8, w4
	blo	1b

	bl	compare_dram_data

	ret	x29



/*********************************************************/


compare_dram_data:
	ldr	w8, =BLD_TESTADDR	/* address 0 */
	add	w9, w8, #0x800		/* address 1 */
	add	w4, w8, #0x800
1:
	ldp	x0, x1, [x8], #16
	ldp	x2, x3, [x9], #16
	cmp	x0, x2
	bne	2f
	cmp	x1, x3
	bne	2f
	cmp	w8, w4
	blo	1b
2:
	/* good if w8==w4 */
	cmp	w8, w4
	cset	w0, lo
	ret



/*********************************************************/
