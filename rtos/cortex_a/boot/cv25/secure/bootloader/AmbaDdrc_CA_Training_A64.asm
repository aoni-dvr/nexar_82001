/**
 *  @file AmbaDdrc_CA_Training_A64.asm
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
 *  @details Dram Write Training Code
 *
 */

#include "AmbaCortexA53.h"
#include "AmbaDramTrain.h"

.section .lpddr4_traning_seq_stage_ca
.global ddrc_train_ca_start

.Align(3)
ddrc_train_ca_start:
   /* initialize stack and the log space in scratch pad */
   ldr x14, =AHB_SCRATCH_PAD_LOG_SPACE
   mov sp, x14
   mov w12, #0
   str w12, [x14, #4] /* clear the temp word */
   str w12, [x14, #8] /* clear the bit count */
   add w12, w14, #0xc
   str w12, [x14] /* write pointer */

   LDR X0, =(AMBA_CORTEX_A53_DDR_CTRL0_BASE_ADDR - AMBA_CORTEX_A53_DRAM_CONFIG_VIRT_BASE_ADDR + AMBA_CORTEX_A53_DRAM_CONFIG_PHYS_BASE_ADDR)

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
ddrc_add_host1_offset:
  /* host id in w20 */
  /* w12 <- w12 + 24 for host 1 */
   STP x10,x30, [sp, #-16]!
   cmp w20, #1
   bne 3f
   add w12, w12, #24
   b 3f
.Align(3)
3:
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
   STP     x29, x30, [sp, #-16]!
   orr     w12, w0, w12
   ldr     w9, [x12]
   and     w11, w11, w10
   bic     w9, w9, w10
   orr     w9, w9, w11
   str     w9, [x12]
   LDP     x29, x16, [sp], #16
   LDP     x11, x12, [sp], #16
   LDP     x9, x10, [sp], #16
   RET     x16

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
ddrc_check_ca_pattern:
   /* w6 - Pattern Fail Status */
   /* w7 - Pattern */

   STP     x10, x30, [sp, #-16]!
   /* check pattern 0 */
   eor w10, w7, #0x3f
   orr w10, w10, w10, lsl #12
   orr w10, w10, w7, lsl #6
   mov w12, #DDRC_UINST_CAD
   mov w11, #0
   bl ddrc_uinst
   mov w12, #DDRC_UINST_DQC
   mov w10, #0
   bl ddrc_uinst

   ldr w12, [x0, #DDRC_DQ_CAPTURE_REG]
   eor w12, w12, w7
   and w12, w12, #0x3f
   orr w6, w6, w12
   /* return */
   LDP     x10, x16, [sp], #16
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
   and w2, w8, #1
   mov w16, w2
   orr w16, w16, w2, LSL #1
   bic w4, w4, w16
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
   orr w9, w9, #4

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
   ldr w1, [x0, #DDRC_SCRATCHPAD_REG]
   bic w1, w1, #0xf
   str w1, [x0, #DDRC_SCRATCHPAD_REG]
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
ddrc_ca_training_status:
  /* check if training succeeded */
  /* w2 - min Vref, w3 - max Vref, w4 - found */
  /* return value w1 - mid Vref, w5 - fail */

   mov w5, #0 /* clear fail */
   ldr w6, =0x3f3f
   cmp w4, w6
   mov w16, w5
   orr w17, w5, #1
   csel w5, w17, w16, NE /* set fail if not found */
   cmp w2, w3
   mov w16, w5
   orr w17, w5, #0x1
   csel w5, w17, w16, GT /* set fail if w2 > w3 */

   /* compute mid point */
   add w1, w2, w3
   add w1, w1, #1
   lsr w1, w1, #1

   /* record status */
   ldr w12, [x0, #DDRC_SCRATCHPAD_REG]
   orr w12, w12, w5
   str w12, [x0, #DDRC_SCRATCHPAD_REG]

   /* convert linear vref to range0/1 */
   subs w4, w1, #51
   bmi ddrc_ca_training_status_return /* return if less than 51 */
   add w4, w4, #21
   orr w1,w4, #0x40

   /* return */
.Align(3)
ddrc_ca_training_status_return:
   RET

.Align(3)
ddrc_ca_training:
   /* w12 - rank, w11 - channel, w10 - set no */
   /* returns vref in w1 */

   STP     x2, x3, [sp, #-16]!
   STP     x4, x5, [sp, #-16]!
   STP     x6, x7, [sp, #-16]!
   STP     x8, x9, [sp, #-16]!
   STP     x10, x11, [sp, #-16]!
   STP     x12, x30, [sp, #-16]!
   bl ddrc_init     /* this will move set no from w10 to w9 */

   /* change the write set and go into training mode*/
   mov w12, #DDRC_UINST_MRW
   mov w11, #13
   lsl w10, w9, #6
   bl ddrc_uinst
   orr w10, w10, #1
   bl ddrc_uinst

   /* drive dqs low, and then cke low */
   mov w12, #DDRC_UINST_DQS0D
   mov w11, #0
   mov w10, #0
   bl ddrc_uinst
   mov w12, #DDRC_UINST_CKED
   bl ddrc_uinst

   /* Change to high frequency */
   /* mov w12, #0 */
   /* bl ddrc_freq_change */

   /* save w9 */
   STP     x29, x9, [sp, #-16]!
   /* train */
   /* range0: 0 - 29; range1: 30 - 80 */
   /* logic value of 1 is used to train for maxVref */
   /* logic value of 0 is used to train for minVref */

   /* w1 - bit counter
   w2 - min Vref
   w3 - max Vref
   w4 - found, 1:VH Found 0:VL Found
   w5 - current vref value
   w6 - Pattern Fail Status
   w7 - Pattern
   w8 - VL
   w9 - VH
   */
   mov w4, #0
   mov w2, #0 /* Min Vref */
   mov w3, #80 /* Max Vref */
   mov w8, #0  /* VL */
   mov w9, #80 /* VH */
   mov w5, w2 /* Start from Min Vref */
   b ddrc_ca_vref_loop
.Align(3)
ddrc_ca_vref_loop:
   subs w10, w5, #51
   mov w16, w5
   csel w10, w16, w10, mi
   bmi ddrc_drive_vref
   add w10, w10, #21
   orr w10, w10, #0x40
   b ddrc_drive_vref
.Align(3)
ddrc_drive_vref:
   mov w12, #DDRC_UINST_DQD
   mov w11, #0
   bl ddrc_uinst
   mov w6, #0 /* Start with all pass */
   mov w7, #0x3f
   bl ddrc_check_ca_pattern
   mov w7, #0x0
   bl ddrc_check_ca_pattern
   mov w7, #0xaa
   bl ddrc_check_ca_pattern
   mov w7, #0x55
   bl ddrc_check_ca_pattern
   /* w6 - Final Pattern Status Here */
   cmp w6, #0 /* Current Vref is passing */
   beq ddrc_ca_pass
   cmp w4, #3 /* VH/VL Found + Current Vref fail */
   beq ddrc_exit_loop
   /* vref fail condition done */
   b ddrc_next_vref
.Align(3)
ddrc_ca_pass:
   /* vref pass condition done */
   cmp w4, #3 /* VH/VL Found + Current Vref pass */
   beq ddrc_ca_update_vh
   /* Update VL & Set Found Flag to 3 */
   mov w8, w5
   mov w4, #3
   b ddrc_ca_update_vh
.Align(3)
ddrc_ca_update_vh:
   mov w9, w5
   b ddrc_next_vref
.Align(3)
ddrc_next_vref:
   add w5, w5, #1
   cmp w5, w3
   blt ddrc_ca_vref_loop
   b ddrc_exit_loop
.Align(3)
ddrc_exit_loop:
   /* mov vh from w9 to w7 */
   mov w7, w9
   /* bring back w9 */
   LDP    x29, x9, [sp], #16

   /* pull cke high, and then dqs high */
   mov w12, #DDRC_UINST_CKEU
   mov w11, #0
   mov w10, #0
   bl ddrc_uinst
   mov w12, #DDRC_UINST_DQS0U
   bl ddrc_uinst

   /* exit training */
   mov w12, #DDRC_UINST_MRW
   mov w11, #13
   mov w10, #0xc0
   bl ddrc_uinst

   ldr w12, [x0, #DDRC_SCRATCHPAD_REG]
   and w11, w12, #0xff000000
   and w12, w12, #0x00ffffff
   lsl w12, w12, #4
   orr w12, w11, w12
   str w12, [x0, #DDRC_SCRATCHPAD_REG]

   /* skip writing if failed */
   cmp w4, #3
   bne ddrc_ca_exit
   /* Put mid vref into w10*/
   add w10, w7, w8
   add w10, w10, #1
   lsr w10, w10, #1
   /* convert linear vref to range0/1 */
   subs w1, w10, #51
   bmi  program_vref
   add w1, w1, #21
   orr w10, w1, #0x40
.Align(3)
program_vref:
   mov w1, w10
   mov w12, #DDRC_UINST_MRW
   mov w11, #12
   bl ddrc_uinst
   /* Update SET0 */
   mov w12, #DDRC_UINST_MRW
   mov w11, #13
   mov w10, #0
   mov w12, #DDRC_UINST_MRW
   mov w11, #12
   mov w10, w1
   bl ddrc_uinst
   bl ddrc_ca_exit

.Align(3)
ddrc_ca_exit:

   /* clear up control reg */
   mov w12, #DDRC_CONTROL_REG
   mov w11, #6
   ldr w10, =0xE0000000
   bl ddrc_read_modify_write

   /* return */
   LDP     x12, x16, [sp], #16
   LDP     x10, x11, [sp], #16
   LDP     x8, x9, [sp], #16
   LDP     x6, x7, [sp], #16
   LDP     x4, x5, [sp], #16
   LDP     x2, x3, [sp], #16
   RET    x16

.Align(3)
lpddr4_training:
   STP     x0, x30, [sp, #-16]!
   /* initialize the log space in scratch pad */

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

   /* CA training*/
   mov w12, #0 /* rank 0 */
   mov w11, #0 /* channel 0 */
   mov w10, #1 /* set 1 */
   mov w18, #0
   bl ddrc_ca_training
   mov w2, w1 /* save vref0 in w2 */

   /* log vref0 */
   mov w12, w1
   mov w10, #32
   bl ddrc_push_to_scratchpad

   ldr w12, [x0, #DDRC_SCRATCHPAD_REG]
   and w10, w12, #0xff000000
   and w12, w12, #0x00ffffff
   lsl w12, w12, #8
   orr w12, w10, w12
   str w12, [x0, #DDRC_SCRATCHPAD_REG]
   nop

   mov w12, #0 /* rank 0*/
   mov w11, #1 /* channel 1 */
   mov w10, #1 /* set 1 */
   mov w18, #2
   bl ddrc_ca_training
   mov w3, w1 /* save vref1 in w3 */

   /* log vref1 */
   mov w12, w1
   mov w10, #32
   bl ddrc_push_to_scratchpad

   /* exit ddrc from training mode */
   mov w12, #DDRC_TIMING5_LP4TRAIN
   ldr w11, =0x0
   ldr w10, =0x1000000
   bl ddrc_read_modify_write

   //bl ddrc_exit

//.Align(3)
//ddrc_exit:
   /* clear up control reg */
   mov w12, #DDRC_CONTROL_REG
   ldr w11, =0x60000000
   ldr w10, =0xE0000000
   bl ddrc_read_modify_write

   /* do RTT */
   /* disable DRAM */
   mov w12, #DDRC_CONTROL_REG
   mov w11, #0
   mov w10, #1
   bl ddrc_read_modify_write

   /* trigger RTT */
   mov w11, #4
   str w11, [x0, #DDRC_INIT_CTL_REG]

   /* Poll bit 2 for 0 */
   b ddrc_rtt_wait_host0
.Align(3)
ddrc_rtt_wait_host0:
   ldr w11, [x0, #DDRC_INIT_CTL_REG]
   ands w11, w11, #4
   bne ddrc_rtt_wait_host0

   /* Enable DRAM */
   mov w12, #DDRC_CONTROL_REG
   mov w11, #1
   mov w10, #1
   bl ddrc_read_modify_write

   /* return */
   LDP     x0, x16, [sp], #16
   RET    x16

   .Align(3)

   DRAM_TrainingParam:
  /* maximum stack needed is 112 */
