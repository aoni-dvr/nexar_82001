/*******************************************************************************
 * AmbaBST_OTP.asm
 *
 * Copyright (c) 2020 Ambarella, Inc.
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
 ******************************************************************************/

#include <AmbaBST.h>
#include <bst_regs.h>

#define BIT(b)				(1 << (b))

#define RCT_BASE			    0xED080000
#define OTP_CTRL1_REG			(0x760)
#define OTP_CTRL2_REG			(0x764)
#define OTP_OBSV_REG			(0x768)
#define OTP_READ_CTRL_READ_DOUT	(0x76C)

/* OTP_CTRL1_REG bit define */
#define READ_FSM_ENABLE			BIT(22)
#define READ_ENABLE			    BIT(21)
#define DBG_READ_MODE			BIT(20)
#define FSM_WRITE_MODE			BIT(18)
#define PROG_ENABLE			    BIT(17)
#define PROG_FSM_ENABLE			BIT(16)


/* OTP_OBSV_REG bit define */
#define WRITE_PROG_DONE			BIT(4)
#define WRITE_PROG_FAIL			BIT(3)
#define WRITE_PROG_RDY			BIT(2)
#define READ_OBSV_RDY			BIT(1)
#define READ_OBSV_DONE			BIT(0)


.text
/*
 * __opt_copy_loop()
 *
 * x0: src start bit addr
 * x1: src end bit addr
 * x2: des start byte addr
 * w6: return if key all zero
 *
 */

__otp_copy_loop:
	/* program source start address [0:14] */
	and	w0, w0, #0x7fff

	/* program source end address [0:15] : w1 could be 0x800 to copy last bit */
	and	w1, w1, #0xffff

	ldr	x3, =RCT_BASE

	/* fsm_write_mode = 0 */
	ldr	w4, [x3, #OTP_CTRL1_REG]
	bic	w4, w4,	#FSM_WRITE_MODE

	/* debg_read_mode = 1 to stop otp auto load process*/
	orr	w4, w4,	#DBG_READ_MODE

	/* read_fsm_enable = 1 */
	orr	w4, w4,	#READ_FSM_ENABLE
	str	w4, [x3, #OTP_CTRL1_REG]

__otp__readl:
	/* while (!read_rdy) */
.L0:	ldr	w4, [x3, #OTP_OBSV_REG]
	tst	w4, #READ_OBSV_RDY
	b.eq	.L0

	/* set program address */
	ldr	w4, [x3, #OTP_CTRL1_REG]
	bic	w4, w4,	#0x7fff
	orr	w4, w4, w0

	/* read enable = 1 */
	orr	w4, w4, #READ_ENABLE
	str	w4, [x3, #OTP_CTRL1_REG]

	/* while (!read_done) */
.L1:	ldr	w4, [x3, #OTP_OBSV_REG]
	tst	w4, #READ_OBSV_DONE
	b.eq	.L1

	/* get 32bit data from OTP_READ_OUT */
	ldr	w5, [x3, #OTP_READ_CTRL_READ_DOUT]
	str	w5, [x2]
        orr     w6, w6, w5

	/* read enable = 0. 32bit data read finish */
	ldr	w4, [x3, #OTP_CTRL1_REG]
	bic	w4, w4, #READ_ENABLE
	str	w4, [x3, #OTP_CTRL1_REG]

	add	w0, w0, #32
	add	x2, x2, #4
	cmp	w0, w1
	b.lo	__otp__readl

	ret

.global __AmbaSecureBootCheck

__AmbaSecureBootCheck:

    mov     X29, X30    /* backup link register (X30) to X29 */

    // read OTP[0-63]
    mov	x0, #0
    add	x1, x0, #0x40
    ldr	x2, =OTP_CONFIG_COPY_ADDR
    bl  __otp_copy_loop

    // check Secure Boot status
    // if (OTP[6] == 1 && OTP[38] == 1) {
    //     secure_boot = 1;
    // } else if (POC[6] == 1) {
    //     secure_boot = 1;
    // } else {
    //     secure_boot = 0;
    // }
    ldr     X20, =(AMBA_DBG_PORT_RCT_BASE_ADDR - AMBA_CORTEX_A53_SECURE_APB_VIRT_BASE_ADDR + AMBA_CORTEX_A53_SECURE_APB_PHYS_BASE_ADDR)
    ldr     W0, [X20, #RCT_POC_REG] // W0: RCT SYS_CONFIG_REG
    ldr     X21, =OTP_CONFIG_COPY_ADDR
    ldr     X1, [X21]               // X1: OTP[0-63]
    and	    X1, X1, #0x4000000040   // X1 = X1 & 0x0000004000000040 for bit[6] and bit[38]
    ubfx	X0, X0, #6, #1
    mov	    X2, #0x4000000040       // X2 = 0x0000004000000040 for otp mask (bit[6] and bit[38])
    cmp	    X1, X2
    csinc	W0, W0, wzr, ne         // ne = any, W0: secure boot flag, 1 = secure boot, 0 = non-secure boot

    // save secure boot flag
    ldr     X1, =SECURE_BOOT_COPY_ADDR
    str     X0, [X1]                // save secure boot flag

    // overwrite POC[6] if secure boot flag = 1
    tbz     w0, #0, Done          /* branch out if not Secure Boot */
    ldr     w1, [X20, #RCT_POC_REG] // read RCT SYS_CONFIG_REG into W1
    orr     w1, w1, #0x40           // set W1[6] = POC[6] = 1
    str     w1, [X20, #RCT_POC_REG] // write W1 to RCT SYS_CONFIG_REG

Done:
    mov     X30, X29    /* restore link register (X30) from X29 */
    ret




