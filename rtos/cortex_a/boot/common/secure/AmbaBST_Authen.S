/**
 *  @file AmbaBST_Authen.asm
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
 *  @details Authentication support for security purpose
 *
 */

#define LOCK_BIT_C1		5
#define LOCK_BIT_C2		6
#define LOCK_BIT_C3		7
#define DATAINVLD_BIT_C1	0
#define DATAINVLD_BIT_C2	1

/*
 * void armv8_sha256_one_shot(unsigned char *p, int length, unsigned char *output)
 *     @ p: firmware address
 *     @ length: firmware length
 *     @ output: buffer address for sha256 of the firmware, size is 256-bit (32-byte)
 *
 * int armv8_rsa2048_exp65537_verify(UINT8 *hash, UINT8 *signature, UINT32 *X, UINT32 *N, UINT32 *W, UINT32 *T);
 *     @ hash: output parameter returned from armv8_sha256_one_shot(), size is 32 bytes
 *     @ signature: firmware signature address, size is 256 bytes
 *     @ X: address of "2^4096 modulus N", size is 260 bytes, from OTP
 *     @ N: address of "public key N", size is 260 bytes, from OTP
 *     @ W: buffer W, size is 257*4 = 1028 bytes
 *     @ T: buffer T, size is 256*2*4 = 2048 bytes
 *     Note: stack size required: 300 bytes
*/
#define armv8_rsa2048_exp65537_verify	0xffff0e24
#define armv8_sha256_one_shot		0xffff09c0


/*
 * void otp_copy_loop(u32 start, u32 end, void *dest)
 *     @ start: otp start bit addr
 *     @ end: otp end bit addr
 *     @ dest: dest start address
*/
#define otp_copy_loop			0xffff0818


/*
 *	SP_0_RAM - RW
 *
 *  ^ --------------------- ^
 *  |	SHA256  -   32 byte |
 *  * --------------------- *
 *  |	SIGN(c) -  256 byte |
 *  * --------------------- *
 *  |	RSA-X(c) - 256 byte |
 *  * --------------------- *
 *  |	RSA-XH  -  16 byte  |
 *  * --------------------- *
 *  |	RSA-N(c) - 256 byte |
 *  * --------------------- *
 *  |	RSA-NH  -  16 byte  |
 *  * --------------------- *
 *  |	RSA-W   -  260 byte |
 *  * --------------------- *
 *  |	RSA-T   -  520 byte |
 *  * --------------------- *
 *  |	STACK   -  256 byte |
 *  * --------------------- *
 *  |	VECTOR  -  ~ byte   |
 *  * --------------------- *
 *
 *  (c) -> Copy
 *
 */

#define SHA256_BUFFER_ADDRESS		(AHB_SP0_RAM_BASE)		/* 0xE8020000 */
#define	SIGNATURE_KEY_COPY_ADDR		(SHA256_BUFFER_ADDRESS		+ 0x20)
#define WRITE_LOCK_BIT_COPY_ADDR	(SIGNATURE_KEY_COPY_ADDR        + 0x100)
#define DATA_INVALID_BIT_COPY_ADDR	(WRITE_LOCK_BIT_COPY_ADDR       + 0x20)

#define RSA_PUBLIC_KEY_X_COPY_ADDR	(DATA_INVALID_BIT_COPY_ADDR	+ 0x20)
#define RSA_PUBLIC_KEY_XH_ADDR		(RSA_PUBLIC_KEY_X_COPY_ADDR	+ 0x100)
#define RSA_PUBLIC_KEY_N_COPY_ADDR	(RSA_PUBLIC_KEY_XH_ADDR		+ 0x10)
#define RSA_PUBLIC_KEY_NH_ADDR		(RSA_PUBLIC_KEY_N_COPY_ADDR	+ 0x100)
#define RSA_BUFFER_W_ADDRESS		(RSA_PUBLIC_KEY_NH_ADDR		+ 0x10)
#define RSA_BUFFER_T_ADDRESS		(RSA_BUFFER_W_ADDRESS		+ 0x104)
#define STACK_END_ADDRESS		(RSA_BUFFER_T_ADDRESS		+ 0x104 * 2 + 0x100 + 0x44)


/*
 *	Image in DRAM
 *
 *  ^ --------------------- ^
 *  |          ~~~          |
 *  * --------------------- *
 *  |    SIGN   -  256 byte |
 *  * --------------------- *
 *
 */

.text

.global verify_signature


verify_signature:
	tbz	w8, #6, verify_signature_end	/* branch out if not Secure Boot */

	/* Stack point must be aligned to 16 bytes */
	ldr	x0, =STACK_END_ADDRESS
	mov	sp, x0
	stp	x29, x30, [sp, #-16]!
	stp	x24, x25, [sp, #-16]!
	stp	x22, x23, [sp, #-16]!
	stp	x20, x21, [sp, #-16]!
	stp	x8, x9, [sp, #-16]!
	stp	x5, x6, [sp, #-16]!

	/* check if valid Public Key exist */
__find_valid_key:
	ldr	x0, =WRITE_LOCK_BIT_COPY_ADDR
	ldr	w4, [x0];
	ldr	x0, =DATA_INVALID_BIT_COPY_ADDR
	ldr	w5, [x0];

__check_C1:
	tbz	w4, #LOCK_BIT_C1, __end_rsa
	mov	w13, #0x5000			/* C1 public key offset */
	tbz	w5, #DATAINVLD_BIT_C1, __read_public_key

__check_C2:
	tbz	w4, #LOCK_BIT_C2, __end_rsa
	mov	w13, #0x6000			/* C2 public key offset */
	tbz	w5, #DATAINVLD_BIT_C2, __read_public_key

__check_C3:  /* don't check datainvalid bit for key3 */
	tbz	w4, #LOCK_BIT_C3, __end_rsa
	mov	w13, #0x7000			/* C3 public key offset */

__read_public_key:
	mov	x0, x13
	add	x1, x0, #0x800
	ldr	x2, =RSA_PUBLIC_KEY_N_COPY_ADDR
	ldr	x3, =otp_copy_loop
	blr	x3

	add	x0, x13, #0x800
	add	x1, x0, #0x800
	ldr	x2, =RSA_PUBLIC_KEY_X_COPY_ADDR
	ldr	x3, =otp_copy_loop
	blr	x3

/*
 * w15/x15 - Length of BLD/ATF binary
 * w28/x28 - DRAM address of BLD/ATF binary
 */
__cp_signature:
	/* copy 'SIGNATURE' key from DRAM to SCRATCHPAD RAM */
	ldr	x2, =SIGNATURE_KEY_COPY_ADDR;
	add	x1, x28, x15
	sub	x0, x1, #0x100
	bl	__rsa_copy_loop

__start_rsa:
	/* clear 'NH' for rsa 'N' */
	ldr	x0, =RSA_PUBLIC_KEY_NH_ADDR
	str	xzr, [x0]
	/* clear 'XH' for rsa 'X' */
	ldr	x0, =RSA_PUBLIC_KEY_XH_ADDR
	str	xzr, [x0]

	mov	w0, w28
	sub	w1, w15, #0x100
	ldr	x2, =SHA256_BUFFER_ADDRESS
	ldr	x6, =armv8_sha256_one_shot
	blr	x6

	ldr	w0, =SHA256_BUFFER_ADDRESS
	ldr	w1, =SIGNATURE_KEY_COPY_ADDR
	ldr	w2, =RSA_PUBLIC_KEY_X_COPY_ADDR
	ldr	w3, =RSA_PUBLIC_KEY_N_COPY_ADDR
	ldr	w4, =RSA_BUFFER_W_ADDRESS
	ldr	w5, =RSA_BUFFER_T_ADDRESS
	ldr	x6, =armv8_rsa2048_exp65537_verify
	blr	x6

	cbnz	w0, halt_cpu

__end_rsa:
	ldp	x5, x6, [sp], #16
	ldp	x8, x9, [sp], #16
	ldp	x20, x21, [sp], #16
	ldp	x22, x23, [sp], #16
	ldp	x24, x25, [sp], #16
	ldp	x29, x30, [sp], #16

verify_signature_end:
	ret

halt_cpu:
	b	.

/*
 * __rsa_copy_loop()
 *
 * x0: src start addr
 * x1: src end addr
 * x2: des start addr
 *
 * !!! WARNING: x0, x2 should be aligned to 16-byte.
 *
 */
__rsa_copy_loop:
	ldp	x26, x27, [x0], #16
	stp	x26, x27, [x2], #16
	cmp	x0, x1
	b.lt	__rsa_copy_loop
	ret

