/**
 * @file crc32.S
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
 */

#include "AmbaBST.h"

//#define CRC32_THRUPUT

        .global verify_crc32

/*
 * w15/x15 - Length of BLD
 * w28/x28 - DRAM address of BLD/ATF binary
 * w16/x16 - BLD crc32 stored in PTB
 */

 /*
 * w1/x1 - data source
 * w3/x3 - crc value
 */

verify_crc32:

    mov w2, w15             // BLD residule length
    mov x4, #0              // BLD offset
    ldr w3, =0xFFFFFFFF     // w3 = ~0

#ifdef CRC32_THRUPUT
    /* X29 = Base Address of RCT registers */
    /* X24 = timer control */
    /* X25 = timer counter */
    LDR     W29, =(AMBA_DBG_PORT_RCT_BASE_ADDR - AMBA_CORTEX_A53_SECURE_APB_VIRT_BASE_ADDR + AMBA_CORTEX_A53_SECURE_APB_PHYS_BASE_ADDR)
    MOVZ    W24, #1          // W24 = 1
    STR     W24, [X29, #RCT_TIMER_2_CTRL_REG]          /* Reset RCT Timer */
    MOVZ    W24, #0          // W24 = 0
    STR     W24, [X29, #RCT_TIMER_2_CTRL_REG]          /* Enable RCT Timer */
#endif

__loop_64bit:
    cmp w2, #8
    blt __loop_32bit        // branch if length < 8 bytes
    ldr x1, [x28, x4]       // load data source
    crc32x  w3, w3, x1            // calculate 64 bits checksum
    add	x4, x4, #8          // update drame offset; w4 = w4 + 8;
    sub w2, w2, #8          // update residule length; w2 = w2 - 8;
    b   __loop_64bit
__loop_32bit:
    cmp w2, #4
    blt __loop_16bit        // branch if length < 4 bytes
    ldr w1, [x28, x4]        // load data source
    crc32w  w3, w3, w1            // calculate 32 bits checksum
    add	x4, x4, #4          // update drame offset; w4 = w4 + 4;
    sub w2, w2, #4          // update residule length; w2 = w2 - 4;
    b   __loop_32bit
__loop_16bit:
    cmp w2, #2
    blt __loop_8bit          // branch if length < 2 bytes
    ldr w1, [x28, x4]        // load data source
    crc32h  w3, w3, w1             // calculate 16 bits checksum
    add	x4, x4, #2           // update drame offset; w4 = w4 + 2;
    sub w2, w2, #2           // update residule length; w2 = w2 - 2;
    b   __loop_16bit
__loop_8bit:
    cmp w2, #1
    blt __compare            // branch if length < 1 bytes; calculation is done
    ldr w1, [x28, x4]        // load data source
    crc32b  w3, w3, w1             // calculate 8 bits checksum
    add	x4, x4, #1           // update drame offset; w4 = w4 + 1;
    sub w2, w2, #1           // update residule length; w2 = w2 - 1;
    b   __loop_8bit
__compare:
    ldr w0, =0xFFFFFFFF
    eor w3, w3, w0           // finalize the value; w3 = w3 ^ ~0

#ifdef CRC32_THRUPUT
    MOVZ    W24, #2                                  /* W0 = 2 */
    STR     W24, [X29, #RCT_TIMER_2_CTRL_REG]        /* Freeze RCT Timer */
    LDR     W25, [X29, #RCT_TIMER_2_COUNT_REG]       /* W25 = Current RCT Timer Counter */
    b       .
#endif
    cmp w3, w16               // compare the final value with that on PTB
    bne halt_cpu
    ret     lr

halt_cpu:
    b	.
