/**
 *  @file AmbaDdrc_Read_Training_A64.asm
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
 *  @details Dram Read Training Code
 *
 */

#include "AmbaCortexA53.h"
#include "AmbaDramTrain.h"

#define READ_VREF_MARGIN 0x0

#define ENABLE_OFFSET_VREF  1
#define ENABLE_OFFSET_READ  1
#define ENABLE_OFFSET_DLL   1

.section .lpddr4_traning_seq_stage_read
.global ddrc_train_read_start

.Align(3)
ddrc_train_read_start:
   /* initialize stack and the log space in scratch pad */
   ldr x24, =AHB_SCRATCH_PAD_LOG_SPACE
   sub sp, x24, #0x10	/* Reverse 0x10 for training parameters */
   mov w12, #0
   str w12, [x24, #4] /* clear the temp word */
   str w12, [x24, #8] /* clear the bit count */
   add w12, w24, #0xc
   str w12, [x24] /* write pointer */

   ldr w0, =AMBA_CORTEX_A53_DDR_CTRL0_BASE_ADDR

   bl lpddr4_training

   LDR     X1, =AMBA_RAM_DRAM_TRAINING_STAGE2_OFFSET
   BR      X1

.Align(3)
ddrc_uinst:
  /* w12 - opcode, w11 - addr, w10 - data */
   STP     x10, x30, [sp, #-16]!
   orr w10,w10,w11, lsl #8
   orr w10,w10,w12, lsl #28
   str w10,[x0,#DDRC_UINST_REG]
   b ddrc_uinst_busy_check

.Align(3)
ddrc_uinst_busy_check:
   ldr w10, [x0, #DDRC_UINST_BUSY_REG]
   ands w10, w10,#0x1
   bne ddrc_uinst_busy_check
   LDP     x10, x16, [sp], #16
   RET x16

.Align(3)
ddrc_wait:
  /* w12 contains count */
   STP     x12, x30, [sp, #-16]!
   b ddrc_wait_loop
.Align(3)
ddrc_wait_loop:
   subs w12,w12,#1
   bne ddrc_wait_loop
   LDP     x12, x16, [sp], #16
   RET    x16

.Align(3)
ddrc_read_modify_write:
  /* w0 - ddrc base, w12 - addr offset, w11 - data, w10 - enable */

   STP     x9, x10, [sp, #-16]!
   STP     x11, x12, [sp, #-16]!
   orr     w12, w0, w12
   ldr     w9, [x12]
   and     w11, w11, w10
   bic     w9, w9, w10
   orr     w9, w9, w11
   str     w9, [x12]
   LDP     x11, x12, [sp], #16
   LDP     x9, x10, [sp], #16
   RET

.Align(3)
ddrc_poll_for_one:
  /* w0 - ddrc base,  w12 - addr offset, w11 - bit position */

   STP     x10, x11, [sp, #-16]!
   STP     x12, x30, [sp, #-16]!
   orr w12, w0, w12
   b ddrc_poll_for_one_loop
.Align(3)
ddrc_poll_for_one_loop:
   ldr w10, [x12]
   ands w10, w10, w11
   beq ddrc_poll_for_one_loop
   LDP     x12, x16, [sp], #16
   LDP     x10, x11, [sp], #16
   RET    x16

.Align(3)
ddrc_read_multiMPC_check:
   STP     x5, x6, [sp, #-16]!
   STP     x7, x8, [sp, #-16]!
   STP     x9, x10, [sp, #-16]!
   STP     x11, x12, [sp, #-16]!
  /* w0 needs to have base address of ddrc */
  /* w1 will contain the fail */
  /* w6 - w12 registers are changed */
  /* w6 - write fifo loop variable */
  /* w7 - read fifo loop variable */
  /* w8 - data mask 0xff00ff or 0xff00ff00 */
  /* w9 - got data */
  /* w10 - expected data */
  /* w11 - address variable */
  /* w12 - inner loop fail variable */

   mov w1, #1 /* start that it failed */
   mov w6, #0
   ldr w8, =0xff00ff
   lsl w16, w8, #8
   ands w17, w18, #1
   csel w8, w16, w8, ne
   b ddrc_read_multiMPC_LOOP0
.Align(3)
ddrc_read_multiMPC_LOOP0:
   mov w7, #0
   mov w12, #0
  /* Read and compare */
   b ddrc_read_multiMPC_LOOP1
.Align(3)
ddrc_read_multiMPC_LOOP1:
  /* failed, then exit loop1 */
   cmp w12, #1
   beq ddrc_read_multiMPC_LOOP0_END
   add w11, w0, w7
   add w11, w11, #DDRC_READ_FIFO_BASE_REG
   ldr w9, [x11]
   add w11, w6, w7
   and w11, w11, #0x1f
   add w11, w0, w11
   add w11, w11, #DDRC_WRITE_FIFO_BASE_REG
   ldr w10, [x11]
   eor w10, w10, w9
  /* Compare byte */
   ands w11, w10, w8
   mov w16, w12
   orr w17, w12, #1
   csel w12, w17, w16, ne
  /* Check for loop1 */
   add w7, w7, #4
   cmp w7, #32
   bne ddrc_read_multiMPC_LOOP1
  /* if pass then update fail flags */
   ands w1, w1, w12
   cmp w1, #0
   beq ddrc_read_multiMCP_RETURN
   b ddrc_read_multiMPC_LOOP0_END
.Align(3)
ddrc_read_multiMPC_LOOP0_END:
  /* Check for outer loop */
   add w6, w6, #4
   cmp w6, #32
   bne ddrc_read_multiMPC_LOOP0
   b ddrc_read_multiMCP_RETURN
.Align(3)
ddrc_read_multiMCP_RETURN:
   LDP     x11, x12, [sp], #16
   LDP     x9, x10, [sp], #16
   LDP     x7, x8, [sp], #16
   LDP     x5, x6, [sp], #16
   RET

.Align(3)
ddrc_record_min_max:
  /* w3 - setting[2] vh */
  /* w4 - found[2] vh */
  /* w21 - setting[2] vl */
  /* w22 - found[2] vl */
  /* w6 - value to be recorded  */
  /* w9 - fail */
  /* w2,w10,w16,w17 - used as temp */

   ands w2, w9, #2
   bne ddrc_record_vl /* fail = 1*/
  /* check if found[0] is 1 */
   ands w10, w4, #1
   bne ddrc_dq_read_record0_pass;
  /* found[0] == 0, so record the setting */
   bic w3, w3, #0xff
   orr w3, w3, w6
   orr w4, w4, #1 /* set found[0] */
   b ddrc_dq_read_record0_pass
.Align(3)
ddrc_dq_read_record0_pass:
   bic w3, w3, #0xff00
   orr w3, w3, w6, LSL #8
   orr w4, w4, #2 /* set found[1] */

   b ddrc_record_vl
.Align(3)
ddrc_record_vl:
   ands w2, w9, #1
   bne ddrc_record_min_max_return /* fail = 1*/
  /* check if found[0] is 1 */
   ands w10, w22, #1
   bne ddrc_dq_read_record1_pass;
  /* found[0] == 0, so record the setting */
   bic w21, w21, #0xff
   orr w21, w21, w6
   orr w22, w22, #1 /* set found[0] */
   b ddrc_dq_read_record1_pass
.Align(3)
ddrc_dq_read_record1_pass:
   bic w21, w21, #0xff00
   orr w21, w21, w6, LSL #8
   orr w22, w22, #2 /* set found[1] */

   b ddrc_record_min_max_return
.Align(3)
ddrc_record_min_max_return:
   ldr w2, [x0,#DDRC_SCRATCHPAD_REG]
   and w10, w9, #3
   and w16, w2, #0xf
   cmp w16, #0
   beq hole_detect_byte0_state0
   cmp w16, #1
   beq hole_detect_byte0_state1
   cmp w16, #2
   beq hole_detect_byte0_state2
   b hole_detect_done
.Align(3)
hole_detect_byte0_state0:
   cmp w10, #0
   bne hole_detect_done
   orr w2, w2, #0x1
   b hole_detect_done
.Align(3)
hole_detect_byte0_state1:
   cmp w10, #0
   beq hole_detect_done
   bic w2, w2, #0x1
   orr w2, w2, #0x2
   b hole_detect_done
.Align(3)
hole_detect_byte0_state2:
   cmp w10, #0
   bne hole_detect_done
   orr w2, w2, #0x3
   b hole_detect_done
.Align(3)
hole_detect_done:
   str w2, [x0, #DDRC_SCRATCHPAD_REG]
   RET

.Align(3)
ddrc_update_vref_min_max:
   /* w1 - fail */
   /* w5 - current vref */
   /* w9 - passing window indicator */
   /* update min/max in w7/w8 */
   STP     x2, x3, [sp, #-16]!
   STP     x10, x11, [sp, #-16]!
   STP     x29, x30, [sp, #-16]!

   ands w2, w9, #4 /* bit 2 indicates that we are done */
   bne ddrc_update_vref_min_max_return

   and w2, w1, #1 /* pass/fail indicator */
   and w3, w9, #1 /* passing window indicator */
   cmp w2, #1
   beq ddrc_update_vref_min_max_fail
   cmp w9, #1 /* passing window indicator */
   beq ddrc_update_vref_min_max_update_max
   /* update min */
   mov w7, w5
   orr w9, w9, #1 /* set passing window */
   b ddrc_update_vref_min_max_update_max
.Align(3)
ddrc_update_vref_min_max_update_max:
   /* update max */
   mov w8, w5
   b ddrc_update_vref_min_max_return

.Align(3)
ddrc_update_vref_min_max_fail:
   cmp w3, #0
   beq ddrc_update_vref_min_max_return

   b ddrc_update_vref_min_max_return
.Align(3)
ddrc_update_vref_min_max_return:

   /* return */
   LDP     x29, x16, [sp], #16
   LDP     x10, x11, [sp], #16
   LDP     x2, x3, [sp], #16
   RET    x16

.Align(3)
ddrc_update_vhvl:
   /* w7 min, w8 max */
   /* track vl(min,max) and vh(min,max) in w24 if w14=1*/
   /* w15 range */
   cmp w14, #0
   b.eq ddrc_update_vhvl_RETURN
   sub w10, w8, w7
   cmp w10, w15
   b.lt ddrc_update_vhvl_RETURN
   mov w10, #0xff
   and w11, w24, w10
   cmp w7, w11
   csel w11, w7, w11, lt
   bic w24, w24, w10
   orr w24, w24, w11
   lsl w10, w10, #8
   and w11, w24, w10
   lsr w11, w11, #8
   cmp w7, w11
   csel w11, w7, w11, gt
   bic w24, w24, w10
   orr w24, w24, w11, LSL #8
   ldr w10, =0xff0000
   and w11, w24, w10
   lsr w11, w11, #16
   cmp w8, w11
   csel w11, w8, w11, lt
   bic w24, w24, w10
   orr w24, w24, w11, LSL #16
   lsl w10, w10, #8
   and w11, w24, w10
   lsr w11, w11, #24
   cmp w8, w11
   csel w11, w8, w11, gt
   bic w24, w24, w10
   orr w24, w24, w11, LSL #24
   b ddrc_update_vhvl_RETURN
.Align(3)
ddrc_update_vhvl_RETURN:
   RET
.Align(3)
ddrc_compute_vref_range:
   /* Min Vref is in w7 */
   /* Max Vref is in w8 */
   /* return range in w12 */
   /* w8-w7*/
   sub w12, w8, w7

   RET
.Align(3)
ddrc_compute_pass_fail:
   STP x1,x2, [sp, #-16]!
   STP x29,x30, [sp, #-16]!
   mov w9, #3
   sub w1, w8, w7
   cmp w1, w15
   b.lt ddrc_compute_pass_fail_RETURN
   ldr w1, =0xff
   and w2, w24, w1
   add w2, w2, w25
   and w16, w9, #2
   cmp w7, w2
   csel w9, w16, w9, le
   lsl w1, w1, #24
   and w2, w24, w1
   lsr w2, w2, #24
   sub w2, w2, w25
   and w16, w9, #1
   cmp w8, w2
   csel w9, w16, w9, ge
   b ddrc_compute_pass_fail_RETURN
.Align(3)
ddrc_compute_pass_fail_RETURN:
   LDP     x29, x16, [sp], #16
   LDP     x1, x2, [sp], #16
   RET x16
.Align(3)
ddrc_compute_vref:
   STP x1,x2, [sp, #-16]!
   STP x29,x30, [sp, #-16]!
   and w1, w24, #0xff
   mov w2, #0xff
   lsl w2, w2, #24
   and w2, w2, w24
   lsr w2, w2, #24
   add w12, w1, w2
   lsr w12, w12, #1
   LDP     x29, x16, [sp], #16
   LDP     x1, x2, [sp], #16
   RET x16
.Align(3)
ddrc_compute_delay:
   STP     x0, x1, [sp, #-16]!
   STP     x2, x3, [sp, #-16]!
   STP     x4, x5, [sp, #-16]!
   STP     x6, x7, [sp, #-16]!
   STP     x8, x9, [sp, #-16]!
   STP     x29, x30, [sp, #-16]!
   and w1, w4, w22
   cmp w1, #3
   b.ne ddrc_compute_delay_RESET_STATUS
   and w1, w3, #0xff
   and w2, w3, #0xff00
   lsr w2, w2, #8
   and w7, w21, #0xff
   and w8, w21, #0xff00
   lsr w8, w8, #8
   cmp w1, w7
   csel w1, w1, w7, gt
   cmp w2, w8
   csel w2, w2, w8, lt
   add w12, w1, w2
   lsr w12, w12, #1
   b ddrc_compute_delay_RETURN
.Align(3)
ddrc_compute_delay_RESET_STATUS:
   mov w1, #0x10000         /* Set the status to indicate Delay fall back */
   ldr w12, [x0, #DDRC_SCRATCHPAD_REG]
   orr w12, w12, w1
   str w12, [x0, #DDRC_SCRATCHPAD_REG]
   /* Set w12 to original value to avoid 0 */
   mov w1, #8
   mul w1, w1, w18     /* Translate byte ID to bit positiion in read delay (x 8) */
   lsr w12, w28, w1    /* Get corresponded read delay of byte ID in w12 */
   and w12, w12, #0x3f
   b ddrc_compute_delay_RETURN
.Align(3)
ddrc_compute_delay_RETURN:
   LDP     x29, x16, [sp], #16
   LDP     x8, x9, [sp], #16
   LDP     x6, x7, [sp], #16
   LDP     x4, x5, [sp], #16
   LDP     x2, x3, [sp], #16
   LDP     x0, x1, [sp], #16
   RET x16
.Align(3)
ddrc_get_vfine:
   ldr x12, =DDRIO_DLL_STATUS_BASE
   orr w12, w12, w0
   add w16, w12, #0x8
   ands w17, w18, #2
   csel w12, w16, w12, ne
   add w16, w12, #0x4
   ands w17, w18, #1
   csel w12, w16, w12, ne
   ldr w11, [x12]
   lsr w11, w11, #16
   and w11, w11, #0x1f
   RET
.Align(3)
ddrc_init:
  /* byte id is w18 */
  /* w12 - rank, w11 - channel, w10 - set no */

   STP     x29, x30, [sp, #-16]!
  /* tell ddrc rank/channel that is being trained */
  /* bit 29 - rank 0 is enabled */
  /* bit 30 - rank 1 is enabled */
   mov w9, w10 /* save set no */

  /* prepare data */
   mov w11, #0
   mov w16, #1
   ands w17, w18, #2
   csel w11, w16, w11, ne
   mov w12, #0
   lsl w11, w11, #31
   orr w11, w11, w12, LSL #30
   mvn w12, w12
   and w12, w12, #1
   orr w11, w11, w12, LSL #29

   mov w12, #DDRC_CONTROL_REG
   ldr w10, =0xE0000000
   bl ddrc_read_modify_write
   LDP     x29, x16, [sp], #16
   RET    x16

.Align(3)
ddrc_mid_delay_calc:
  /* It is not general function. Dont use */
  /* compute middle point for byte 0 */
   and w10, w3, #0xff
   lsr w11, w3, #8
   and w11, w11, #0xff
   add w10, w10, w11
   add w10, w10, #1
   lsr w1, w10, #1

   RET

.Align(3)
ddrc_push_to_scratchpad:
  /* w12 - value to be pushed */
   STP     x0, x1, [sp, #-16]!
   STP     x2, x3, [sp, #-16]!
   ldr x0, =AHB_SCRATCH_PAD_LOG_SPACE
   ldr w1, [x0] /* get the write pointer */
   ldr x2, =AHB_SCRATCH_PAD_LOG_SPACE_END
   cmp w1, w2
   beq ddrc_push_to_scratchpad_return
   ldr w2, [x0, #4] /* get the temp. word */
   ldr w3, [x0, #8] /* get the bit pos */
  /* update the temp word */
   lsl w12, w12, w3
   orr w2, w2, w12
   add w3, w3, w10 /* increment the bit pos */
   cmp w3, #32
   bne ddrc_push_to_scratchpad_finish

   str w2, [x1] /* store the word */
   add w1, w1, #4 /* increase the write pointer */
   str w1, [x0] /* store the pointer */
   mov w2, #0  /* clear the temp word */
   mov w3, #0  /* clear bit pos */
   b ddrc_push_to_scratchpad_finish

.Align(3)
ddrc_push_to_scratchpad_finish:
   str w2, [x0, #4] /* store the word */
   str w3, [x0, #8] /* store the bit pos*/
   b ddrc_push_to_scratchpad_return

.Align(3)
ddrc_push_to_scratchpad_return:
   LDP     x2, x3, [sp], #16
   LDP     x0, x1, [sp], #16
   RET

.Align(3)
ddrc_update_dqs_delay:
  /* w11 - delay value for from 0 to 63 */
  /* w11 - {b3,b2,b1,b0} */
   STP     x0, x1, [sp, #-16]!
   STP     x2, x3, [sp, #-16]!
   STP     x4, x5, [sp, #-16]!
   STP     x6, x7, [sp, #-16]!
   STP     x8, x9, [sp, #-16]!
   STP     x10, x11, [sp, #-16]!
   STP     x20, x21, [sp, #-16]!
   STP     x12, x30, [sp, #-16]!
   mov w2, w11 /* Save original w11 into w2, because w11 is used as sub-routinte arguments */

   mov w10, #0x3f /* data enable for read_modify_write sub routine */

   mov w11, w2
   and w11, w11, #0xff
   cmp w11, #32
   mov w16, w11
   eor w17, w11, #0x3f
   add w17, w17, #1
   csel w11, w17, w16, lt
   mov w16, w11
   sub w17, w11, #32
   csel w11, w17, w16, ge
   mov w12, w0
   mov w16, #HOST0_DLL0_REG
   orr w12, w12, w16
   bl ddrc_read_modify_write

   lsr w11, w2, #8
   and w11, w11, #0xff
   cmp w11, #32
   mov w16, w11
   eor w17, w11, #0x3f
   add w17, w17, #1
   csel w11, w17, w16, lt
   mov w16, w11
   sub w17, w11, #32
   csel w11, w17, w16, ge
   mov w12, w0
   mov w16, #HOST0_DLL1_REG
   orr w12, w12, w16
   bl ddrc_read_modify_write

   lsr w11, w2, #16
   and w11, w11, #0xff
   cmp w11, #32
   mov w16, w11
   eor w17, w11, #0x3f
   add w17, w17, #1
   csel w11, w17, w16, lt
   mov w16, w11
   sub w17, w11, #32
   csel w11, w17, w16, ge
   mov w12, w0
   mov w16, #HOST0_DLL2_REG
   orr w12, w12, w16
   bl ddrc_read_modify_write

   lsr w11, w2, #24
   and w11, w11, #0xff
   cmp w11, #32
   mov w16, w11
   eor w17, w11, #0x3f
   add w17, w17, #1
   csel w11, w17, w16, lt
   mov w16, w11
   sub w17, w11, #32
   csel w11, w17, w16, ge
   mov w12, w0
   mov w16, #HOST0_DLL3_REG
   orr w12, w12, w16
   bl ddrc_read_modify_write

   /* reset dll */
   /* reset through the RCT */
   /*  mov w10, #4
   str w10, [x0, #SOFT_OR_DLLRESET_REG]

   mov w10, #6
   str w10, [x0, #SOFT_OR_DLLRESET_REG]
   */

   LDP     x12, x16, [sp], #16
   LDP     x20, x21, [sp], #16
   LDP     x10, x11, [sp], #16
   LDP     x8, x9, [sp], #16
   LDP     x6, x7, [sp], #16
   LDP     x4, x5, [sp], #16
   LDP     x2, x3, [sp], #16
   LDP     x0, x1, [sp], #16
   RET    x16

.Align(3)
ddrc_translate_dll_to_dqs_delay:
   /* DLL value is in w11 */
   /* return value in w11 */
   /* translate DLL 0x3f~0x20,0x0~0x1f to 1~32,32~63 */
   and w11, w11, #0x3f
   cmp w11, #32
   eor w10, w11, #0x3f
   add w10, w10, #1
   add w11, w11, #32
   csel w11, w11, w10, lt
   RET

.Align(3)
ddrc_backup_dll_delay:
   /* Keep original DLL read values in w28 */
   STP     x11, x30, [sp, #-16]!

   ldr w11, [x0, #HOST0_DLL0_REG]
   bl ddrc_translate_dll_to_dqs_delay
   mov w28, w11
   ldr w11, [x0, #HOST0_DLL1_REG]
   bl ddrc_translate_dll_to_dqs_delay
   orr w28, w28, w11, LSL #8
   ldr w11, [x0, #HOST0_DLL2_REG]
   bl ddrc_translate_dll_to_dqs_delay
   orr w28, w28, w11, LSL #16
   ldr w11, [x0, #HOST0_DLL3_REG]
   bl ddrc_translate_dll_to_dqs_delay
   orr w28, w28, w11, LSL #24

   LDP     x11, x16, [sp], #16
   RET    x16

.Align(3)
training_env_init:
   STP     x14, x30, [sp, #-16]!

   bl ddrc_init

   /* Switch to new operating set */
   mov w12, #DDRC_UINST_MRW
   mov w11, #13
   mov w10, #0xc0 /* fixme: hardcode set info */
   bl ddrc_uinst

   mov w11, #0
   mov w10, #0xf
   mov w12, #DDRC_SCRATCHPAD_REG
   bl ddrc_read_modify_write
   /* Program the pattern to DDR */
   /* write pattern to MR; default MR15/20 */
   ldr w5, =0xaaaa  /* toggles every cycle */

   mov w12, #DDRC_UINST_MRW
   mov w11, #15
   mov w10, #0x55
   bl ddrc_uinst
   mov w11, #20
   bl ddrc_uinst

   and w10, w5, #0xff
   mov w11, #32
   bl ddrc_uinst

   and w10, w5, #0xff00
   lsr w10, w10, #8
   mov w11, #40
   bl ddrc_uinst

  /* Write the expected data into WRITE_FIFO_BASE_REG */
   mov w12, w0
   add w12, w12, #DDRC_WRITE_FIFO_BASE_REG
   ldr w11, =0xaaaa5555
   str w11, [x12]
   add w12, w12, #4
   str w11, [x12]
   add w12, w12, #4
   str w11, [x12]
   add w12, w12, #4
   str w11, [x12]
   add w12, w12, #4
   str w11, [x12]
   add w12, w12, #4
   str w11, [x12]
   add w12, w12, #4
   str w11, [x12]
   add w12, w12, #4
   str w11, [x12]

   LDP     x14, x16, [sp], #16
   RET    x16

.Align(3)
ddrc_dqdelay_calibration:
   /* w12 - rank, w11 - channel */
   /* Register allocation */
   /* # r2 -> dqdc (current dq delay) */
   /* # r3 -> dqdm (dq delay @max range) */
   /* # w5 -> current vref */
   /* # w7 -> min vref */
   /* # w8 -> max vref */
   /* # w9 -> in pass window */
   /* # w12 -> current range */
   /* # w13 -> max range byte */

   STP     x2, x3, [sp, #-16]!
   STP     x4, x5, [sp, #-16]!
   STP     x6, x7, [sp, #-16]!
   STP     x8, x9, [sp, #-16]!
   STP     x10, x11, [sp, #-16]!
   STP     x12, x13, [sp, #-16]!
   STP     x14, x30, [sp, #-16]!

   bl training_env_init

   /* read rct register that dictates negative push */
   bl ddrc_get_vfine
   mov w2, #44
   sub w2, w2, w11
   mov w11, w2
   orr w11, w11, w2, LSL #8
   orr w11, w11, w11, LSL #16
   bl ddrc_update_dqs_delay
   mov w2, #0
   mov w3, #0
   mov w13, #0
   b dqdelay_calib_dqdelay_loop
.Align(3)
dqdelay_calib_dqdelay_loop:
  /* Program the delay */
   ldr w10, =0x1e
   lsl w16, w10, #5
   ands w17, w18, #1
   csel w10, w16, w10, ne
   lsl w16, w10, #10
   ands w17, w18, #2
   csel w10, w16, w10, ne
   mov w12, #DDRC_READ_DQ_DELAY_REG
   lsl w11, w2, #1
   orr w11, w11, w2, lsl #6
   orr w11, w11, w2, lsl #11
   orr w11, w11, w2, lsl #16
   bl ddrc_read_modify_write
   mov w5, #0    /* vref */
   mov w7, #0    /* Min Vref */
   mov w8, #0    /* Max Vref */
   mov w9, #0    /* in pass window */
   b dqdelay_vref_loop

.Align(3)
dqdelay_vref_loop:
   /* program vref */
   mov w12, w5
   mov w12, #47
   sub w12, w12, w5
   lsl w11, w12, #0
   orr w11, w11, w12, LSL #7
   orr w11, w11, w12, LSL #14
   orr w11, w11, w12, LSL #21
   mov w12, #DDRC_READ_DQ_VREF_REG
   ldr w10, =0xfffffff
   bl ddrc_read_modify_write

   /* MPC read uinst */
   mov w12, #DDRC_UINST_MPCR_DQ
   mov w11, #0
   mov w10, #2
   bl ddrc_uinst

   bl ddrc_read_multiMPC_check

   /* based on pass/fail, update Min/Max Vref */
   bl ddrc_update_vref_min_max
   add w5, w5, #1
   cmp w5, #48
   blt dqdelay_vref_loop

   bl ddrc_compute_vref_range

   and w10, w12, #0xff
   cmp w10, w13
   csel w13, w10, w13, gt
   csel w3, w2, w3, gt
   add w2, w2, #1
   cmp w2, #16
   blt dqdelay_calib_dqdelay_loop
   /* Program the delay */
#if ENABLE_OFFSET_READ
    add w10, w18, #1    /* Parameter offset */
    mov w11, w3         /* Original value */
    mov w12, #15        /* Upper bound */
    bl apply_offset
    mov w3, w11
#endif
   ldr w10, =0x1e
   lsl w16, w10, #5
   ands w17, w18, #1
   csel w10, w16, w10, ne
   lsl w16, w10, #10
   ands w17, w18, #2
   csel w10, w16, w10, ne
   mov w12, #DDRC_READ_DQ_DELAY_REG
   lsl w11, w3, #1
   orr w11, w11, w3, lsl #6
   orr w11, w11, w3, lsl #11
   orr w11, w11, w3, lsl #16
   bl ddrc_read_modify_write

   /* return */
   LDP     x14, x16, [sp], #16
   LDP     x12, x13, [sp], #16
   LDP     x10, x11, [sp], #16
   LDP     x8, x9, [sp], #16
   LDP     x6, x7, [sp], #16
   LDP     x4, x5, [sp], #16
   LDP     x2, x3, [sp], #16
   RET    x16

.Align(3)
ddrc_dq_calibration:
   /* w12 - rank, w11 - channel */
   /* w14 - track vh/vl (1:yes|0:no) */
   /* return value in w3,w4,w21,w22 */

   /* Register allocation
   w3[7:0]   - setting[0][0]
   w3[15:8]  - setting[0][1]
   w3[23:16] - setting[0][0]
   w3[31:24] - setting[0][1]
   w4[3:0]   - found[1][1], found[1][0], found[0][1], found[0][0]
   w21/22 - w3/4 values corresponding to max range
   */

   STP     x6, x7, [sp, #-16]!
   STP     x8, x9, [sp, #-16]!
   STP     x10, x11, [sp, #-16]!
   STP     x12, x30, [sp, #-16]!

   /* save delay register value */
   ldr w11, [x0, #HOST0_DLL0_REG]
   ldr w12, [x0, #HOST0_DLL1_REG]
   STP     x11, x12, [sp, #-16]!
   ldr w11, [x0, #HOST0_DLL2_REG]
   ldr w12, [x0, #HOST0_DLL3_REG]
   STP     x11, x12, [sp, #-16]!

   bl training_env_init

   /* initialze settings */
   mov w3,#0
   mov w4, #0
   mov w21, #0
   mov w22, #0

   mov w5, #0 /* Vref */
   mov w6, #0 /* current delay */
   cmp w14, #0
   b.eq ddrc_dq_calibration_skip_0
   ldr w24, =0x00ff00ff /* tracks vl(min,max), vh(min,max) */
   b ddrc_dq_calibration_skip_0
.Align(3)
ddrc_dq_calibration_skip_0:

   /* read rct register that dictates negative push */
   bl ddrc_get_vfine
   mov w6, #32
   sub w6, w6, w11
   b read_delay_loop
.Align(3)
read_delay_loop:

   /* write the delay */
   mov w11, w6
   orr w11, w11, w11, LSL #8
   orr w11, w11, w11, LSL #16
   bl ddrc_update_dqs_delay
   mov w5, #0    /* vref */
   mov w7, #0    /* Min Vref */
   mov w8, #0    /* Max Vref */
   mov w9, #0    /* in pass window */
   b read_vref_loop

.Align(3)
read_vref_loop:
   /* program vref */
   mov w12, w5
   mov w12, #47
   sub w12, w12, w5
   lsl w11, w12, #0
   orr w11, w11, w12, LSL #7
   orr w11, w11, w12, LSL #14
   orr w11, w11, w12, LSL #21
   mov w12, #DDRC_READ_DQ_VREF_REG
   ldr w10, =0xfffffff
   bl ddrc_read_modify_write

   /* MPC read uinst */
   mov w12, #DDRC_UINST_MPCR_DQ
   mov w11, #0
   mov w10, #2
   bl ddrc_uinst

   bl ddrc_read_multiMPC_check

   /* based on pass/fail, update Min/Max Vref */
   bl ddrc_update_vref_min_max
   add w5, w5, #1
   cmp w5, #48
   blt read_vref_loop

   bl ddrc_update_vhvl
   cmp w14, #1
   b.eq ddrc_dq_calibration_skip_1
   /* compute pass/fail*/
   bl ddrc_compute_pass_fail
   /* record the min/max settings */
   bl ddrc_record_min_max
   b ddrc_dq_calibration_skip_1
.Align(3)
ddrc_dq_calibration_skip_1:
   add w6, w6, #1
   bl ddrc_get_vfine
   mov w12, #56/* max delay of range */
   sub w12, w12, w11
   cmp w6, w12
   blt read_delay_loop

   /* restore original delay setting */
   LDP     x11, x12, [sp], #16
   str w11, [x0, #HOST0_DLL2_REG]
   str w12, [x0, #HOST0_DLL3_REG]
   LDP     x11, x12, [sp], #16
   str w11, [x0, #HOST0_DLL0_REG]
   str w12, [x0, #HOST0_DLL1_REG]

   /* return */
   LDP     x12, x16, [sp], #16
   LDP     x10, x11, [sp], #16
   LDP     x8, x9, [sp], #16
   LDP     x6, x7, [sp], #16
   RET    x16


.Align(3)
apply_offset:
    /* w10 - offset address */
    /* w11 - original value and the new value after apply offset */
    /* w12 - Maximum value */
    STP x2, x3, [sp, #-16]!

    ldr w2, =AHB_SCRATCH_PAD_PARAM_SPACE    /* Parameter was passed in special address */
    add w2, w2, w10
    ldrb w10, [x2]                          /* Load offset from parameters */
    tst w10, #0x80                          /* Check sign bit */
    and w10, w10, #0x7f                     /* Remove sing bit */
    bne negative                            /* Jump to negative flow if offset is negative */
    add w11, w11, w10                       /* orig = orig + offset */
    cmp w11, w12                            /* orig = (orig > max) ? max : orig */
    csel w11, w12, w11, gt
    b   offset_done
negative:
    cmp w11, w10                            /* orig = (orig > offset) ? orig - offset : 0 */
    sub w11, w11, w10
    csel w11, w11, wzr, gt

offset_done:
    LDP x2, x3, [sp], #16
    RET


.Align(3)
lpddr4_training:
   STP     x0, x30, [sp, #-16]!

   /* put ddrc into training mode */
   mov w12, #DDRC_TIMING5_LP4TRAIN
   ldr w11, =0x1000000
   mov w10, w11
   bl ddrc_read_modify_write

   mov w12, #DDRC_TIMING5_LP4TRAIN
   mov w11, #1
   lsl w11, w11, #25
   bl ddrc_poll_for_one

   /* clear scratch pad register */
   ldr w12, [x0, #DDRC_SCRATCHPAD_REG]
   and w12, w12, 0xff000000
   str w12, [x0, #DDRC_SCRATCHPAD_REG]

   mov w12, #DDRC_CONTROL_REG
   mov w11, 0x0
   mov w10, 0x2
   bl ddrc_read_modify_write

   mov w12, #DDRC_CONTROL_REG
   mov w11, 0x0
   mov w10, 0x1
   bl ddrc_read_modify_write

   mov w12, #DDRC_UINST_SR
   mov w11, #0
   mov w10, #1
   bl ddrc_uinst

   /* Read data path training */
   ldr w23, [x0, #DDRC_READ_DQ_VREF_REG]    /* Keep original Vref in w23 for training failure */
   bl ddrc_backup_dll_delay                 /* Keep original Delay in w28 for training failure */
   mov w18, #0    /* Byte Count */
   mov w20, #0    /* Loop Count */
   mov w27, #0x00ff00ff /* tracks vl(min,max), vh(min,max) */
   b read_train_loop
.Align(3)
read_train_loop:
   bl ddrc_dqdelay_calibration
   ldr w25, [x0, #DDRC_DLL_CTRL_SEL_0_REG]
   tst w25, #0x20000
   mov w25, #10
   mov w15, #16
   csel w15, w15, w25, ne /* passing range */
   mov w25, #4     /* fix window */
   mov w14, #1
   bl ddrc_dq_calibration
   mov w14, #0
   bl ddrc_dq_calibration
   bl ddrc_compute_vref
   sub w12, w12, #READ_VREF_MARGIN
   mov w10, #8
   bl ddrc_push_to_scratchpad
   /* rddly */
   mov w12, #0
   bl ddrc_compute_delay
   mov w10, #8
   bl ddrc_push_to_scratchpad

   mov w14, #1
   and w7, w24, #0xff00
   lsr w7, w7, #8
   lsr w24, w24, #16
   and w8, w24, #0xff
   mov w24, w27
   bl ddrc_update_vhvl
   mov w27, w24

   add w18, w18, #1
   cmp w18, #4
   b.ne read_train_loop
   mov w18, #0
   add w20, w20, #1
   cmp w20, #3
   b.ne read_train_loop

   mov w10, #32
   mov w12, w24
   bl ddrc_push_to_scratchpad

   /*
    * @Original dram training flow
    *
    * ldr w12, =AHB_SCRATCH_PAD_LOG_SPACE
    * add w12, w12, #12
    * ldr w10, [x12]
    * and w11, w10, #0xff
    * lsr w10, w10, #16
    * and w1,  w10, #0xff
    * add w11, w11, w1
    * add w12, w12, #4
    * ldr w10, [x12]
    * and w12, w10, #0xff
    * add w11, w11, w12
    * lsr w10, w10, #16
    * and w12, w10, #0xff
    * add w11, w11, w12
    * lsr w11, w11, #2
    *
    */

   mov w24, #0x00ff00ff
   eor w24, w27, w24
   cmp w24, #0
   bne result_apply_trained /* Jump to result_apply0 if training result is passed */
   mov w1, #0x20000         /* Set the status to indicate Vref fall back */
   ldr w12, [x0, #DDRC_SCRATCHPAD_REG]
   orr w12, w12, w1
   str w12, [x0, #DDRC_SCRATCHPAD_REG]
   mov w11, w23             /* Restore the original Vref (keep in w23) on training failure */
   b result_apply

.Align(3)
result_apply_trained:
   /*
    * @tuned dram training flow
    */
   lsr w27, w27, #8
   and w11, w27, #0xff
   lsr w10, w27, #16
   and w1,  w10, #0xff
   add w11, w11, w1
   lsr w11, w11, #1


   mov w12, #47
   sub w12, w12, w11
#if ENABLE_OFFSET_VREF
    mov w10, #0         /* Parameter offset */
    mov w11, w12        /* Original value */
    mov w12, #47        /* Upper bound */
    bl apply_offset
    mov w12, w11
#endif
   lsl w11, w12, #0
   orr w11, w11, w12, LSL #7
   orr w11, w11, w12, LSL #14
   orr w11, w11, w12, LSL #21
   b result_apply

.Align(3)
result_apply:                   /* The value for Vref reg should be stored in w11 before here */
   mov w12, #DDRC_READ_DQ_VREF_REG
   ldr w10, =0xfffffff
   bl ddrc_read_modify_write
   mov w2, #0
   ldr w12, =AHB_SCRATCH_PAD_LOG_SPACE
   add w12, w12, #12
   ldr w1, [x12]
   lsr w1, w1, #8
   and w11, w1, #0xff
#if ENABLE_OFFSET_DLL
    mov w10, #5         /* Parameter offset */
    //mov w11, w11        /* Original value */
    mov w12, #63        /* Upper bound */
    bl apply_offset
    //mov w11, w11
#endif
   orr w2, w2, w11
   lsr w1, w1, #16
   and w11, w1, #0xff
#if ENABLE_OFFSET_DLL
    mov w10, #6         /* Parameter offset */
    //mov w11, w11         /* Original value */
    mov w12, #63        /* Upper bound */
    bl apply_offset
    //mov w11, w11
#endif
   orr w2, w2, w11, LSL #8
   ldr w12, =AHB_SCRATCH_PAD_LOG_SPACE
   add w12, w12, #16
   ldr w1, [x12]
   lsr w1, w1, #8
   and w11, w1, #0xff
#if ENABLE_OFFSET_DLL
    mov w10, #7         /* Parameter offset */
    //mov w11, w11         /* Original value */
    mov w12, #63        /* Upper bound */
    bl apply_offset
    //mov w11, w11
#endif
   orr w2, w2, w11, LSL #16
   lsr w1, w1, #16
   and w11, w1, #0xff
#if ENABLE_OFFSET_DLL
    mov w10, #8         /* Parameter offset */
    //mov w11, w11         /* Original value */
    mov w12, #63        /* Upper bound */
    bl apply_offset
    //mov w11, w11
#endif
   orr w2, w2, w11, LSL #24
   mov w11, w2
   bl ddrc_update_dqs_delay
   ldr w12, [x0, #DDRC_SCRATCHPAD_REG]
   and w12, w12, #0xffff0000
   str w12, [x0, #DDRC_SCRATCHPAD_REG]
   bl ddrc_exit

.Align(3)
ddrc_exit:
   /* clear up control reg */
   mov w12, #DDRC_CONTROL_REG
   ldr w11, =0x60000000
   ldr w10, =0xE0000000
   bl ddrc_read_modify_write

   /* exit self-refresh */
   mov w12, #DDRC_UINST_SR
   mov w11, #0
   mov w10, #0
   bl ddrc_uinst

   bl XSR_Wait

   mov w12, #DDRC_UINST_REFRESH
   mov w11, #0
   mov w10, #7
   bl ddrc_uinst

   /* enable AR */
   mov w12, #DDRC_CONTROL_REG
   mov w11, #2
   mov w10, #2
   bl ddrc_read_modify_write

   /* exit ddrc from training mode */
   mov w12, #DDRC_TIMING5_LP4TRAIN
   ldr w11, =0x0
   ldr w10, =0x1000000
   bl ddrc_read_modify_write

   /* do RTT */
   /* trigger RTT */
   mov w11, #4
   str w11, [x0, #DDRC_INIT_CTL_REG]

   /* Poll bit 2 for 0 */
   b ddrc_rtt_wait
.Align(3)
ddrc_rtt_wait:
   ldr w11, [x0, #DDRC_INIT_CTL_REG]
   ands w11, w11, #4
   bne ddrc_rtt_wait

   /* Enable DRAM */
   mov w12, #DDRC_CONTROL_REG
   mov w11, #1
   mov w10, #1
   bl ddrc_read_modify_write

   /* return */
   LDP     x0, x16, [sp], #16
   RET    x16


.Align(3)
XSR_Wait:
        MOV     W19, #120                   /* W19 = 1us */
        LDR     W20, =(AMBA_DBG_PORT_RCT_BASE_ADDR)
        MOVZ    W13, #1                     /* W0 = 1 */
        STR     W13, [X20, #0x258]          /* Reset RCT Timer */
        MOVZ    W13, #0                     /* W0 = 0 */
        STR     W13, [X20, #0x258]          /* Enable RCT Timer */

/**
 *  Fix RCT timer metastability issue : freeze RCT timer before fetch RCT Timer Counter.
 */
.Align(3)
RctTimer_Wait_L1:
        MOVZ    W13, #2                     /* W0 = 2 */
        STR     W13, [X20, #0x258]          /* Freeze RCT Timer */
        LDR     W18, [X20, #0x254]          /* W18 = Current RCT Timer Counter */
        MOVZ    W13, #0                     /* W0 = 0 */
        STR     W13, [X20, #0x258]          /* Enable RCT Timer */
        CMP     W19, W18                    /* W19 > W18 */
        B.HI    RctTimer_Wait_L1            /* yes, loop */
        RET
.Align(3)
