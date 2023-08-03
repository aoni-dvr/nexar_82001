	.cpu cortex-a9
	.eabi_attribute 28, 1
	.fpu neon
	.eabi_attribute 20, 1
	.eabi_attribute 21, 1
	.eabi_attribute 23, 3
	.eabi_attribute 24, 1
	.eabi_attribute 25, 1
	.eabi_attribute 26, 1
	.eabi_attribute 30, 2
	.eabi_attribute 34, 1
	.eabi_attribute 18, 4
	.file	"TableRowDiff.c"
	.text
	.align	2
	.global	WarpTbl_RowDiff
	.type	WarpTbl_RowDiff, %function
WarpTbl_RowDiff:
	@ args = 0, pretend = 0, frame = 56
	@ frame_needed = 0, uses_anonymous_args = 0
	stmfd	sp!, {r4, r5, r6, r7, r8, r9, r10, fp, lr}
	mov	r10, r2
	mov	r2, r2, asr #3
	sub	sp, sp, #60
	cmp	r3, #1
	str	r3, [sp, #32]
	mov	r3, r2, asl #3
	str	r1, [sp, #12]
	str	r3, [sp, #24]
	ble	.L1
	mov	ip, r2, asl #4
	mov	r5, r10, asl #1
	str	r2, [sp]
	mov	lr, ip
	str	ip, [sp, #36]
	mov	ip, r3
	add	r3, r10, r3
	mov	r3, r3, asl #1
	mov	r9, #0
	mov	r7, r9
	str	r10, [sp, #4]
	mov	r4, r3
	str	r3, [sp, #8]
	add	r3, r0, lr
	add	fp, r0, r4
	mov	lr, r3
	str	r3, [sp, #52]
	rsb	r3, ip, r10
	mov	r8, r5
	mov	r6, r3
	str	r3, [sp, #28]
	add	ip, r1, r4
	sub	r3, r6, #1
	str	r3, [sp, #48]
	mov	r3, #1
	str	r3, [sp, #20]
.L8:
	ldr	r3, [sp]
	ldr	r4, [sp, #12]
	cmp	r3, #0
	add	r3, r8, r7
	mov	r1, r3
	str	r3, [sp, #16]
	add	r4, r4, r1
	ldr	r1, [sp, #4]
	add	r3, r0, r3
	movgt	r5, #0
	ldrgt	r2, [sp]
	add	r10, r1, r9
	add	r1, r0, r7
	ble	.L7
.L6:
	vld1.16	{d16-d17}, [r3]
	add	r5, r5, #1
	cmp	r5, r2
	add	r3, r3, #16
	vld1.16	{d18-d19}, [r1]
	add	r1, r1, #16
	vsub.i16	q8, q9, q8
	vst1.16	{d16-d17}, [r4]
	add	r4, r4, #16
	bne	.L6
	str	r2, [sp]
.L7:
	ldr	r3, [sp, #4]
	ldr	r1, [sp, #24]
	cmp	r3, r1
	ble	.L5
	add	r1, ip, #16
	add	r4, fp, #16
	cmp	r1, lr
	add	r3, lr, #16
	movhi	r5, #0
	movls	r5, #1
	cmp	ip, r3
	movcc	r3, r5
	orrcs	r3, r5, #1
	cmp	r4, ip
	cmphi	r1, fp
	ldr	r1, [sp, #28]
	movls	r4, #1
	movhi	r4, #0
	and	r4, r4, r3
	ldr	r3, [sp, #36]
	cmp	r1, #9
	movls	r4, #0
	andhi	r4, r4, #1
	cmp	r4, #0
	add	r6, r3, r7
	beq	.L10
	ubfx	r3, lr, #1, #2
	rsb	r3, r3, #0
	and	r3, r3, #7
	cmp	r1, r3
	movcs	r1, r3
	cmp	r1, #0
	beq	.L20
	ldr	r3, [sp, #52]
	cmp	r1, #1
	ldrh	r5, [fp]
	ldrh	r4, [r3, r7]
	ldr	r7, [sp, #24]
	rsb	r4, r5, r4
	add	r3, r7, #1
	strh	r4, [ip]	@ movhi
	beq	.L11
	ldrh	r4, [lr, #2]
	cmp	r1, #2
	ldrh	r5, [fp, #2]
	add	r3, r7, #2
	rsb	r4, r5, r4
	strh	r4, [ip, #2]	@ movhi
	beq	.L11
	ldrh	r4, [lr, #4]
	cmp	r1, #3
	ldrh	r5, [fp, #4]
	add	r3, r7, #3
	rsb	r4, r5, r4
	strh	r4, [ip, #4]	@ movhi
	beq	.L11
	ldrh	r4, [lr, #6]
	cmp	r1, #4
	ldrh	r5, [fp, #6]
	add	r3, r7, #4
	rsb	r4, r5, r4
	strh	r4, [ip, #6]	@ movhi
	beq	.L11
	ldrh	r4, [lr, #8]
	cmp	r1, #5
	ldrh	r5, [fp, #8]
	add	r3, r7, #5
	rsb	r4, r5, r4
	strh	r4, [ip, #8]	@ movhi
	beq	.L11
	ldrh	r4, [lr, #10]
	cmp	r1, #7
	ldrh	r5, [fp, #10]
	add	r3, r7, #6
	rsb	r4, r5, r4
	strh	r4, [ip, #10]	@ movhi
	bne	.L11
	ldrh	r4, [lr, #12]
	add	r3, r7, #7
	ldrh	r5, [fp, #12]
	rsb	r4, r5, r4
	strh	r4, [ip, #12]	@ movhi
.L11:
	ldr	r4, [sp, #28]
	ldr	r5, [sp, #48]
	rsb	r4, r1, r4
	str	r4, [sp, #40]
	sub	r4, r4, #8
	mov	r4, r4, lsr #3
	rsb	r5, r1, r5
	cmp	r5, #6
	add	r4, r4, #1
	mov	r5, r4, asl #3
	str	r5, [sp, #44]
	bls	.L13
	mov	r1, r1, asl #1
	ldr	r2, [sp, #12]
	mov	r7, #0
	add	r5, r1, r6
	ldr	r6, [sp, #8]
	add	r5, r0, r5
	add	r1, r1, r6
	add	r6, r0, r1
	add	r1, r2, r1
	ldr	r2, [sp]
.L14:
	vld1.64	{d18-d19}, [r5:64]
	add	r7, r7, #1
	cmp	r7, r4
	add	r5, r5, #16
	vld1.16	{q8}, [r6]
	add	r6, r6, #16
	vsub.i16	q8, q9, q8
	vst1.16	{q8}, [r1]
	add	r1, r1, #16
	bcc	.L14
	ldr	r1, [sp, #44]
	ldr	r4, [sp, #40]
	str	r2, [sp]
	add	r3, r3, r1
	cmp	r4, r1
	beq	.L5
.L13:
	add	r4, r3, r10
	add	r5, r9, r3
	mov	r4, r4, asl #1
	mov	r5, r5, asl #1
	ldr	r7, [sp, #4]
	ldrh	r5, [r0, r5]
	add	r1, r3, #1
	ldrh	r6, [r0, r4]
	ldr	r2, [sp, #12]
	cmp	r7, r1
	rsb	r6, r6, r5
	strh	r6, [r2, r4]	@ movhi
	mov	r5, r2
	ble	.L5
	add	r5, r1, r10
	add	r1, r9, r1
	mov	r5, r5, asl #1
	mov	r4, r1, asl #1
	ldrh	r6, [r0, r5]
	add	r1, r3, #2
	ldrh	r4, [r0, r4]
	cmp	r7, r1
	rsb	r6, r6, r4
	strh	r6, [r2, r5]	@ movhi
	ble	.L5
	add	r5, r1, r10
	add	r1, r9, r1
	mov	r5, r5, asl #1
	mov	r4, r1, asl #1
	ldrh	r6, [r0, r5]
	add	r1, r3, #3
	ldrh	r4, [r0, r4]
	cmp	r7, r1
	rsb	r6, r6, r4
	strh	r6, [r2, r5]	@ movhi
	ble	.L5
	add	r5, r1, r10
	add	r1, r9, r1
	mov	r5, r5, asl #1
	mov	r4, r1, asl #1
	ldrh	r6, [r0, r5]
	add	r1, r3, #4
	ldrh	r4, [r0, r4]
	cmp	r7, r1
	rsb	r6, r6, r4
	strh	r6, [r2, r5]	@ movhi
	ble	.L5
	add	r5, r1, r10
	add	r1, r9, r1
	mov	r5, r5, asl #1
	mov	r4, r1, asl #1
	ldrh	r6, [r0, r5]
	add	r1, r3, #5
	ldrh	r4, [r0, r4]
	cmp	r7, r1
	rsb	r6, r6, r4
	strh	r6, [r2, r5]	@ movhi
	ble	.L5
	add	r4, r1, r10
	add	r1, r9, r1
	mov	r4, r4, asl #1
	mov	r1, r1, asl #1
	ldrh	r5, [r0, r4]
	add	r3, r3, #6
	ldrh	r1, [r0, r1]
	cmp	r7, r3
	rsb	r5, r5, r1
	strh	r5, [r2, r4]	@ movhi
	ble	.L5
	add	r9, r9, r3
	add	r3, r3, r10
	mov	r3, r3, asl #1
	mov	r9, r9, asl #1
	ldrh	r4, [r0, r3]
	ldrh	r1, [r0, r9]
	rsb	r1, r4, r1
	ldr	r4, [sp, #12]
	strh	r1, [r4, r3]	@ movhi
.L5:
	ldr	r3, [sp, #20]
	add	lr, lr, r8
	ldr	r1, [sp, #32]
	add	fp, fp, r8
	add	ip, ip, r8
	ldr	r7, [sp, #16]
	add	r3, r3, #1
	mov	r9, r10
	str	r3, [sp, #20]
	cmp	r3, r1
	ldr	r3, [sp, #8]
	add	r3, r3, r8
	str	r3, [sp, #8]
	bne	.L8
.L1:
	add	sp, sp, #60
	@ sp needed
	ldmfd	sp!, {r4, r5, r6, r7, r8, r9, r10, fp, pc}
.L20:
	ldr	r3, [sp, #24]
	b	.L11
.L10:
	ldr	r3, [sp, #16]
	mov	r6, ip
	ldr	r2, [sp]
	mov	r4, fp
	mov	r5, lr
	add	r7, r8, r3
	add	r7, r0, r7
.L18:
	ldrh	r1, [r4], #2
	ldrh	r3, [r5], #2
	cmp	r4, r7
	rsb	r3, r1, r3
	strh	r3, [r6], #2	@ movhi
	bne	.L18
	str	r2, [sp]
	b	.L5
	.size	WarpTbl_RowDiff, .-WarpTbl_RowDiff
	.align	2
	.global	WarpTbl_RowDiffRecover
	.type	WarpTbl_RowDiffRecover, %function
WarpTbl_RowDiffRecover:
	@ args = 0, pretend = 0, frame = 64
	@ frame_needed = 0, uses_anonymous_args = 0
	stmfd	sp!, {r4, r5, r6, r7, r8, r9, r10, fp, lr}
	cmp	r3, #1
	mov	fp, r2
	mov	r2, r2, asr #3
	sub	sp, sp, #68
	str	r3, [sp, #36]
	mov	r10, r2, asl #3
	str	r1, [sp, #4]
	ble	.L45
	mov	r3, r2, asl #4
	rsb	lr, r10, fp
	mov	r4, fp, asl #1
	mov	r8, lr
	mov	r6, r3
	str	r3, [sp, #40]
	add	r3, fp, r10
	add	ip, r0, r6
	mov	r3, r3, asl #1
	mov	r9, #0
	str	ip, [sp, #60]
	mov	r5, ip
	str	lr, [sp, #24]
	add	ip, r0, r3
	add	lr, r1, r3
	str	r3, [sp, #12]
	add	r3, r6, #16
	str	fp, [sp, #20]
	str	r3, [sp, #44]
	mov	fp, r10
	sub	r3, r8, #1
	mov	r7, r9
	mov	r10, r4
	str	r3, [sp, #56]
	mov	r3, #1
	str	r2, [sp, #8]
	str	r3, [sp, #32]
.L52:
	ldr	r3, [sp, #8]
	ldr	r2, [sp, #4]
	cmp	r3, #0
	add	r3, r10, r7
	mov	r1, r3
	str	r3, [sp, #28]
	add	r4, r2, r1
	ldr	r2, [sp, #20]
	add	r3, r0, r3
	add	r1, r0, r7
	movgt	r6, #0
	add	r2, r2, r9
	str	r2, [sp, #16]
	ldrgt	r2, [sp, #8]
	ble	.L51
.L50:
	vld1.16	{d18-d19}, [r1]
	add	r6, r6, #1
	cmp	r6, r2
	add	r1, r1, #16
	vld1.16	{d16-d17}, [r4]
	add	r4, r4, #16
	vsub.i16	q8, q9, q8
	vst1.16	{d16-d17}, [r3]
	add	r3, r3, #16
	bne	.L50
	str	r2, [sp, #8]
.L51:
	ldr	r3, [sp, #20]
	cmp	r3, fp
	ble	.L49
	ldr	r2, [sp, #44]
	add	r3, lr, #16
	add	r1, ip, #16
	ldr	r8, [sp, #24]
	cmp	lr, r1
	cmpcc	ip, r3
	add	r1, r2, r7
	ldr	r2, [sp, #40]
	movcs	r3, #1
	movcc	r3, #0
	cmp	r8, #9
	movls	r3, #0
	andhi	r3, r3, #1
	add	r8, fp, r9
	add	r6, r2, r7
	ldr	r2, [sp, #12]
	add	r4, r2, #16
	cmp	r6, r4
	cmplt	r2, r1
	movge	r1, #1
	movlt	r1, #0
	tst	r3, r1
	beq	.L54
	ldr	r1, [sp, #24]
	ubfx	r3, r5, #1, #2
	rsb	r3, r3, #0
	and	r3, r3, #7
	cmp	r1, r3
	movcs	r1, r3
	cmp	r1, #0
	moveq	r3, fp
	beq	.L55
	ldr	r3, [sp, #60]
	cmp	r1, #1
	ldrh	r6, [lr]
	ldrh	r4, [r3, r7]
	add	r3, fp, #1
	rsb	r4, r6, r4
	strh	r4, [ip]	@ movhi
	beq	.L55
	ldrh	r4, [r5, #2]
	cmp	r1, #2
	ldrh	r6, [lr, #2]
	add	r3, fp, #2
	rsb	r4, r6, r4
	strh	r4, [ip, #2]	@ movhi
	beq	.L55
	ldrh	r4, [r5, #4]
	cmp	r1, #3
	ldrh	r6, [lr, #4]
	add	r3, fp, #3
	rsb	r4, r6, r4
	strh	r4, [ip, #4]	@ movhi
	beq	.L55
	ldrh	r4, [r5, #6]
	cmp	r1, #4
	ldrh	r6, [lr, #6]
	add	r3, fp, #4
	rsb	r4, r6, r4
	strh	r4, [ip, #6]	@ movhi
	beq	.L55
	ldrh	r4, [r5, #8]
	cmp	r1, #5
	ldrh	r6, [lr, #8]
	add	r3, fp, #5
	rsb	r4, r6, r4
	strh	r4, [ip, #8]	@ movhi
	beq	.L55
	ldrh	r6, [lr, #10]
	cmp	r1, #7
	ldrh	r4, [r5, #10]
	add	r3, fp, #6
	addeq	r3, fp, #7
	rsb	r4, r6, r4
	strh	r4, [ip, #10]	@ movhi
	ldreqh	r4, [r5, #12]
	ldreqh	r6, [lr, #12]
	rsbeq	r4, r6, r4
	streqh	r4, [ip, #12]	@ movhi
.L55:
	ldr	r2, [sp, #24]
	ldr	r4, [sp, #56]
	rsb	r2, r1, r2
	rsb	r6, r1, r4
	sub	r4, r2, #8
	mov	r4, r4, lsr #3
	str	r2, [sp, #48]
	cmp	r6, #6
	add	r4, r4, #1
	mov	r2, r4, asl #3
	str	r2, [sp, #52]
	bls	.L57
	ldr	r2, [sp, #12]
	add	r6, r1, r8
	add	r6, r0, r6, asl #1
	mov	r8, #0
	add	r1, r2, r1, asl #1
	ldr	r2, [sp, #4]
	add	r7, r2, r1
	ldr	r2, [sp, #8]
	add	r1, r0, r1
.L58:
	vld1.64	{d18-d19}, [r6:64]
	add	r8, r8, #1
	cmp	r8, r4
	add	r6, r6, #16
	vld1.16	{q8}, [r7]
	add	r7, r7, #16
	vsub.i16	q8, q9, q8
	vst1.16	{q8}, [r1]
	add	r1, r1, #16
	bcc	.L58
	str	r2, [sp, #8]
	ldr	r1, [sp, #48]
	ldr	r2, [sp, #52]
	cmp	r1, r2
	add	r3, r3, r2
	beq	.L49
.L57:
	ldr	r8, [sp, #16]
	add	r6, r9, r3
	ldr	r1, [sp, #4]
	mov	r6, r6, asl #1
	ldr	r2, [sp, #20]
	add	r4, r3, r8
	ldrh	r6, [r0, r6]
	mov	r4, r4, asl #1
	ldrh	r7, [r1, r4]
	add	r1, r3, #1
	cmp	r2, r1
	rsb	r7, r7, r6
	strh	r7, [r0, r4]	@ movhi
	ble	.L49
	add	r6, r1, r8
	add	r1, r9, r1
	mov	r4, r1, asl #1
	ldr	r1, [sp, #4]
	mov	r6, r6, asl #1
	ldrh	r4, [r0, r4]
	ldrh	r7, [r1, r6]
	add	r1, r3, #2
	cmp	r2, r1
	rsb	r7, r7, r4
	strh	r7, [r0, r6]	@ movhi
	ble	.L49
	add	r6, r1, r8
	add	r1, r9, r1
	mov	r4, r1, asl #1
	ldr	r1, [sp, #4]
	mov	r6, r6, asl #1
	ldrh	r4, [r0, r4]
	ldrh	r7, [r1, r6]
	add	r1, r3, #3
	cmp	r2, r1
	rsb	r7, r7, r4
	strh	r7, [r0, r6]	@ movhi
	ble	.L49
	add	r6, r1, r8
	add	r1, r9, r1
	mov	r4, r1, asl #1
	ldr	r1, [sp, #4]
	mov	r6, r6, asl #1
	ldrh	r4, [r0, r4]
	ldrh	r7, [r1, r6]
	add	r1, r3, #4
	cmp	r2, r1
	rsb	r7, r7, r4
	strh	r7, [r0, r6]	@ movhi
	ble	.L49
	add	r6, r1, r8
	add	r1, r9, r1
	mov	r4, r1, asl #1
	ldr	r1, [sp, #4]
	mov	r6, r6, asl #1
	ldrh	r4, [r0, r4]
	ldrh	r7, [r1, r6]
	add	r1, r3, #5
	cmp	r2, r1
	rsb	r7, r7, r4
	strh	r7, [r0, r6]	@ movhi
	ble	.L49
	add	r4, r1, r8
	ldr	r6, [sp, #4]
	add	r1, r9, r1
	mov	r4, r4, asl #1
	mov	r1, r1, asl #1
	add	r3, r3, #6
	ldrh	r6, [r6, r4]
	cmp	r2, r3
	ldrh	r1, [r0, r1]
	rsb	r6, r6, r1
	strh	r6, [r0, r4]	@ movhi
	ble	.L49
	ldr	r2, [sp, #16]
	add	r9, r9, r3
	mov	r9, r9, asl #1
	add	r3, r3, r2
	ldr	r2, [sp, #4]
	mov	r3, r3, asl #1
	ldrh	r1, [r0, r9]
	ldrh	r4, [r2, r3]
	rsb	r1, r4, r1
	strh	r1, [r0, r3]	@ movhi
.L49:
	ldr	r3, [sp, #32]
	add	r5, r5, r10
	ldr	r2, [sp, #36]
	add	lr, lr, r10
	add	ip, ip, r10
	ldr	r7, [sp, #28]
	add	r3, r3, #1
	ldr	r9, [sp, #16]
	str	r3, [sp, #32]
	cmp	r3, r2
	ldr	r3, [sp, #12]
	add	r3, r3, r10
	str	r3, [sp, #12]
	bne	.L52
.L45:
	add	sp, sp, #68
	@ sp needed
	ldmfd	sp!, {r4, r5, r6, r7, r8, r9, r10, fp, pc}
.L54:
	ldr	r3, [sp, #28]
	mov	r4, ip
	ldr	r2, [sp, #8]
	mov	r7, lr
	mov	r6, r5
	add	r8, r10, r3
	add	r8, r0, r8
.L62:
	ldrh	r3, [r6], #2
	ldrh	r1, [r7], #2
	rsb	r3, r1, r3
	strh	r3, [r4], #2	@ movhi
	cmp	r4, r8
	bne	.L62
	str	r2, [sp, #8]
	b	.L49
	.size	WarpTbl_RowDiffRecover, .-WarpTbl_RowDiffRecover
	.align	2
	.global	VigTbl_RowDiff
	.type	VigTbl_RowDiff, %function
VigTbl_RowDiff:
	@ args = 0, pretend = 0, frame = 56
	@ frame_needed = 0, uses_anonymous_args = 0
	stmfd	sp!, {r4, r5, r6, r7, r8, r9, r10, fp, lr}
	mov	r10, r2
	mov	r2, r2, asr #3
	sub	sp, sp, #60
	cmp	r3, #1
	str	r3, [sp, #32]
	mov	r3, r2, asl #3
	str	r1, [sp, #12]
	str	r3, [sp, #24]
	ble	.L88
	mov	ip, r2, asl #4
	mov	r5, r10, asl #1
	str	r2, [sp]
	mov	lr, ip
	str	ip, [sp, #36]
	mov	ip, r3
	add	r3, r10, r3
	mov	r3, r3, asl #1
	mov	r9, #0
	mov	r7, r9
	str	r10, [sp, #4]
	mov	r4, r3
	str	r3, [sp, #8]
	add	r3, r0, lr
	add	fp, r0, r4
	mov	lr, r3
	str	r3, [sp, #52]
	rsb	r3, ip, r10
	mov	r8, r5
	mov	r6, r3
	str	r3, [sp, #28]
	add	ip, r1, r4
	sub	r3, r6, #1
	str	r3, [sp, #48]
	mov	r3, #1
	str	r3, [sp, #20]
.L95:
	ldr	r3, [sp]
	ldr	r4, [sp, #12]
	cmp	r3, #0
	add	r3, r8, r7
	mov	r1, r3
	str	r3, [sp, #16]
	add	r4, r4, r1
	ldr	r1, [sp, #4]
	add	r3, r0, r3
	movgt	r5, #0
	ldrgt	r2, [sp]
	add	r10, r1, r9
	add	r1, r0, r7
	ble	.L94
.L93:
	vld1.16	{d16-d17}, [r3]
	add	r5, r5, #1
	cmp	r5, r2
	add	r3, r3, #16
	vld1.16	{d18-d19}, [r1]
	add	r1, r1, #16
	vsub.i16	q8, q9, q8
	vst1.16	{d16-d17}, [r4]
	add	r4, r4, #16
	bne	.L93
	str	r2, [sp]
.L94:
	ldr	r3, [sp, #4]
	ldr	r1, [sp, #24]
	cmp	r3, r1
	ble	.L92
	add	r1, ip, #16
	add	r4, fp, #16
	cmp	r1, lr
	add	r3, lr, #16
	movhi	r5, #0
	movls	r5, #1
	cmp	ip, r3
	movcc	r3, r5
	orrcs	r3, r5, #1
	cmp	r4, ip
	cmphi	r1, fp
	ldr	r1, [sp, #28]
	movls	r4, #1
	movhi	r4, #0
	and	r4, r4, r3
	ldr	r3, [sp, #36]
	cmp	r1, #9
	movls	r4, #0
	andhi	r4, r4, #1
	cmp	r4, #0
	add	r6, r3, r7
	beq	.L97
	ubfx	r3, lr, #1, #2
	rsb	r3, r3, #0
	and	r3, r3, #7
	cmp	r1, r3
	movcs	r1, r3
	cmp	r1, #0
	beq	.L107
	ldr	r3, [sp, #52]
	cmp	r1, #1
	ldrh	r5, [fp]
	ldrh	r4, [r3, r7]
	ldr	r7, [sp, #24]
	rsb	r4, r5, r4
	add	r3, r7, #1
	strh	r4, [ip]	@ movhi
	beq	.L98
	ldrh	r4, [lr, #2]
	cmp	r1, #2
	ldrh	r5, [fp, #2]
	add	r3, r7, #2
	rsb	r4, r5, r4
	strh	r4, [ip, #2]	@ movhi
	beq	.L98
	ldrh	r4, [lr, #4]
	cmp	r1, #3
	ldrh	r5, [fp, #4]
	add	r3, r7, #3
	rsb	r4, r5, r4
	strh	r4, [ip, #4]	@ movhi
	beq	.L98
	ldrh	r4, [lr, #6]
	cmp	r1, #4
	ldrh	r5, [fp, #6]
	add	r3, r7, #4
	rsb	r4, r5, r4
	strh	r4, [ip, #6]	@ movhi
	beq	.L98
	ldrh	r4, [lr, #8]
	cmp	r1, #5
	ldrh	r5, [fp, #8]
	add	r3, r7, #5
	rsb	r4, r5, r4
	strh	r4, [ip, #8]	@ movhi
	beq	.L98
	ldrh	r4, [lr, #10]
	cmp	r1, #7
	ldrh	r5, [fp, #10]
	add	r3, r7, #6
	rsb	r4, r5, r4
	strh	r4, [ip, #10]	@ movhi
	bne	.L98
	ldrh	r4, [lr, #12]
	add	r3, r7, #7
	ldrh	r5, [fp, #12]
	rsb	r4, r5, r4
	strh	r4, [ip, #12]	@ movhi
.L98:
	ldr	r4, [sp, #28]
	ldr	r5, [sp, #48]
	rsb	r4, r1, r4
	str	r4, [sp, #40]
	sub	r4, r4, #8
	mov	r4, r4, lsr #3
	rsb	r5, r1, r5
	cmp	r5, #6
	add	r4, r4, #1
	mov	r5, r4, asl #3
	str	r5, [sp, #44]
	bls	.L100
	mov	r1, r1, asl #1
	ldr	r2, [sp, #12]
	mov	r7, #0
	add	r5, r1, r6
	ldr	r6, [sp, #8]
	add	r5, r0, r5
	add	r1, r1, r6
	add	r6, r0, r1
	add	r1, r2, r1
	ldr	r2, [sp]
.L101:
	vld1.64	{d18-d19}, [r5:64]
	add	r7, r7, #1
	cmp	r7, r4
	add	r5, r5, #16
	vld1.16	{q8}, [r6]
	add	r6, r6, #16
	vsub.i16	q8, q9, q8
	vst1.16	{q8}, [r1]
	add	r1, r1, #16
	bcc	.L101
	ldr	r1, [sp, #44]
	ldr	r4, [sp, #40]
	str	r2, [sp]
	add	r3, r3, r1
	cmp	r4, r1
	beq	.L92
.L100:
	add	r4, r3, r10
	add	r5, r9, r3
	mov	r4, r4, asl #1
	mov	r5, r5, asl #1
	ldr	r7, [sp, #4]
	ldrh	r5, [r0, r5]
	add	r1, r3, #1
	ldrh	r6, [r0, r4]
	ldr	r2, [sp, #12]
	cmp	r7, r1
	rsb	r6, r6, r5
	strh	r6, [r2, r4]	@ movhi
	mov	r5, r2
	ble	.L92
	add	r5, r1, r10
	add	r1, r9, r1
	mov	r5, r5, asl #1
	mov	r4, r1, asl #1
	ldrh	r6, [r0, r5]
	add	r1, r3, #2
	ldrh	r4, [r0, r4]
	cmp	r7, r1
	rsb	r6, r6, r4
	strh	r6, [r2, r5]	@ movhi
	ble	.L92
	add	r5, r1, r10
	add	r1, r9, r1
	mov	r5, r5, asl #1
	mov	r4, r1, asl #1
	ldrh	r6, [r0, r5]
	add	r1, r3, #3
	ldrh	r4, [r0, r4]
	cmp	r7, r1
	rsb	r6, r6, r4
	strh	r6, [r2, r5]	@ movhi
	ble	.L92
	add	r5, r1, r10
	add	r1, r9, r1
	mov	r5, r5, asl #1
	mov	r4, r1, asl #1
	ldrh	r6, [r0, r5]
	add	r1, r3, #4
	ldrh	r4, [r0, r4]
	cmp	r7, r1
	rsb	r6, r6, r4
	strh	r6, [r2, r5]	@ movhi
	ble	.L92
	add	r5, r1, r10
	add	r1, r9, r1
	mov	r5, r5, asl #1
	mov	r4, r1, asl #1
	ldrh	r6, [r0, r5]
	add	r1, r3, #5
	ldrh	r4, [r0, r4]
	cmp	r7, r1
	rsb	r6, r6, r4
	strh	r6, [r2, r5]	@ movhi
	ble	.L92
	add	r4, r1, r10
	add	r1, r9, r1
	mov	r4, r4, asl #1
	mov	r1, r1, asl #1
	ldrh	r5, [r0, r4]
	add	r3, r3, #6
	ldrh	r1, [r0, r1]
	cmp	r7, r3
	rsb	r5, r5, r1
	strh	r5, [r2, r4]	@ movhi
	ble	.L92
	add	r9, r9, r3
	add	r3, r3, r10
	mov	r3, r3, asl #1
	mov	r9, r9, asl #1
	ldrh	r4, [r0, r3]
	ldrh	r1, [r0, r9]
	rsb	r1, r4, r1
	ldr	r4, [sp, #12]
	strh	r1, [r4, r3]	@ movhi
.L92:
	ldr	r3, [sp, #20]
	add	lr, lr, r8
	ldr	r1, [sp, #32]
	add	fp, fp, r8
	add	ip, ip, r8
	ldr	r7, [sp, #16]
	add	r3, r3, #1
	mov	r9, r10
	str	r3, [sp, #20]
	cmp	r3, r1
	ldr	r3, [sp, #8]
	add	r3, r3, r8
	str	r3, [sp, #8]
	bne	.L95
.L88:
	add	sp, sp, #60
	@ sp needed
	ldmfd	sp!, {r4, r5, r6, r7, r8, r9, r10, fp, pc}
.L107:
	ldr	r3, [sp, #24]
	b	.L98
.L97:
	ldr	r3, [sp, #16]
	mov	r6, ip
	ldr	r2, [sp]
	mov	r4, fp
	mov	r5, lr
	add	r7, r8, r3
	add	r7, r0, r7
.L105:
	ldrh	r1, [r4], #2
	ldrh	r3, [r5], #2
	cmp	r4, r7
	rsb	r3, r1, r3
	strh	r3, [r6], #2	@ movhi
	bne	.L105
	str	r2, [sp]
	b	.L92
	.size	VigTbl_RowDiff, .-VigTbl_RowDiff
	.align	2
	.global	VigTbl_RowDiffRecover
	.type	VigTbl_RowDiffRecover, %function
VigTbl_RowDiffRecover:
	@ args = 0, pretend = 0, frame = 64
	@ frame_needed = 0, uses_anonymous_args = 0
	stmfd	sp!, {r4, r5, r6, r7, r8, r9, r10, fp, lr}
	cmp	r3, #1
	mov	fp, r2
	mov	r2, r2, asr #3
	sub	sp, sp, #68
	str	r3, [sp, #36]
	mov	r10, r2, asl #3
	str	r1, [sp, #4]
	ble	.L131
	mov	r3, r2, asl #4
	rsb	lr, r10, fp
	mov	r4, fp, asl #1
	mov	r8, lr
	mov	r6, r3
	str	r3, [sp, #40]
	add	r3, fp, r10
	add	ip, r0, r6
	mov	r3, r3, asl #1
	mov	r9, #0
	str	ip, [sp, #60]
	mov	r5, ip
	str	lr, [sp, #24]
	add	ip, r0, r3
	add	lr, r1, r3
	str	r3, [sp, #12]
	add	r3, r6, #16
	str	fp, [sp, #20]
	str	r3, [sp, #44]
	mov	fp, r10
	sub	r3, r8, #1
	mov	r7, r9
	mov	r10, r4
	str	r3, [sp, #56]
	mov	r3, #1
	str	r2, [sp, #8]
	str	r3, [sp, #32]
.L138:
	ldr	r3, [sp, #8]
	ldr	r2, [sp, #4]
	cmp	r3, #0
	add	r3, r10, r7
	mov	r1, r3
	str	r3, [sp, #28]
	add	r4, r2, r1
	ldr	r2, [sp, #20]
	add	r3, r0, r3
	add	r1, r0, r7
	movgt	r6, #0
	add	r2, r2, r9
	str	r2, [sp, #16]
	ldrgt	r2, [sp, #8]
	ble	.L137
.L136:
	vld1.16	{d18-d19}, [r1]
	add	r6, r6, #1
	cmp	r6, r2
	add	r1, r1, #16
	vld1.16	{d16-d17}, [r4]
	add	r4, r4, #16
	vsub.i16	q8, q9, q8
	vst1.16	{d16-d17}, [r3]
	add	r3, r3, #16
	bne	.L136
	str	r2, [sp, #8]
.L137:
	ldr	r3, [sp, #20]
	cmp	r3, fp
	ble	.L135
	ldr	r2, [sp, #44]
	add	r3, lr, #16
	add	r1, ip, #16
	ldr	r8, [sp, #24]
	cmp	lr, r1
	cmpcc	ip, r3
	add	r1, r2, r7
	ldr	r2, [sp, #40]
	movcs	r3, #1
	movcc	r3, #0
	cmp	r8, #9
	movls	r3, #0
	andhi	r3, r3, #1
	add	r8, fp, r9
	add	r6, r2, r7
	ldr	r2, [sp, #12]
	add	r4, r2, #16
	cmp	r6, r4
	cmplt	r2, r1
	movge	r1, #1
	movlt	r1, #0
	tst	r3, r1
	beq	.L140
	ldr	r1, [sp, #24]
	ubfx	r3, r5, #1, #2
	rsb	r3, r3, #0
	and	r3, r3, #7
	cmp	r1, r3
	movcs	r1, r3
	cmp	r1, #0
	moveq	r3, fp
	beq	.L141
	ldr	r3, [sp, #60]
	cmp	r1, #1
	ldrh	r6, [lr]
	ldrh	r4, [r3, r7]
	add	r3, fp, #1
	rsb	r4, r6, r4
	strh	r4, [ip]	@ movhi
	beq	.L141
	ldrh	r4, [r5, #2]
	cmp	r1, #2
	ldrh	r6, [lr, #2]
	add	r3, fp, #2
	rsb	r4, r6, r4
	strh	r4, [ip, #2]	@ movhi
	beq	.L141
	ldrh	r4, [r5, #4]
	cmp	r1, #3
	ldrh	r6, [lr, #4]
	add	r3, fp, #3
	rsb	r4, r6, r4
	strh	r4, [ip, #4]	@ movhi
	beq	.L141
	ldrh	r4, [r5, #6]
	cmp	r1, #4
	ldrh	r6, [lr, #6]
	add	r3, fp, #4
	rsb	r4, r6, r4
	strh	r4, [ip, #6]	@ movhi
	beq	.L141
	ldrh	r4, [r5, #8]
	cmp	r1, #5
	ldrh	r6, [lr, #8]
	add	r3, fp, #5
	rsb	r4, r6, r4
	strh	r4, [ip, #8]	@ movhi
	beq	.L141
	ldrh	r6, [lr, #10]
	cmp	r1, #7
	ldrh	r4, [r5, #10]
	add	r3, fp, #6
	addeq	r3, fp, #7
	rsb	r4, r6, r4
	strh	r4, [ip, #10]	@ movhi
	ldreqh	r4, [r5, #12]
	ldreqh	r6, [lr, #12]
	rsbeq	r4, r6, r4
	streqh	r4, [ip, #12]	@ movhi
.L141:
	ldr	r2, [sp, #24]
	ldr	r4, [sp, #56]
	rsb	r2, r1, r2
	rsb	r6, r1, r4
	sub	r4, r2, #8
	mov	r4, r4, lsr #3
	str	r2, [sp, #48]
	cmp	r6, #6
	add	r4, r4, #1
	mov	r2, r4, asl #3
	str	r2, [sp, #52]
	bls	.L143
	ldr	r2, [sp, #12]
	add	r6, r1, r8
	add	r6, r0, r6, asl #1
	mov	r8, #0
	add	r1, r2, r1, asl #1
	ldr	r2, [sp, #4]
	add	r7, r2, r1
	ldr	r2, [sp, #8]
	add	r1, r0, r1
.L144:
	vld1.64	{d18-d19}, [r6:64]
	add	r8, r8, #1
	cmp	r8, r4
	add	r6, r6, #16
	vld1.16	{q8}, [r7]
	add	r7, r7, #16
	vsub.i16	q8, q9, q8
	vst1.16	{q8}, [r1]
	add	r1, r1, #16
	bcc	.L144
	str	r2, [sp, #8]
	ldr	r1, [sp, #48]
	ldr	r2, [sp, #52]
	cmp	r1, r2
	add	r3, r3, r2
	beq	.L135
.L143:
	ldr	r8, [sp, #16]
	add	r6, r9, r3
	ldr	r1, [sp, #4]
	mov	r6, r6, asl #1
	ldr	r2, [sp, #20]
	add	r4, r3, r8
	ldrh	r6, [r0, r6]
	mov	r4, r4, asl #1
	ldrh	r7, [r1, r4]
	add	r1, r3, #1
	cmp	r2, r1
	rsb	r7, r7, r6
	strh	r7, [r0, r4]	@ movhi
	ble	.L135
	add	r6, r1, r8
	add	r1, r9, r1
	mov	r4, r1, asl #1
	ldr	r1, [sp, #4]
	mov	r6, r6, asl #1
	ldrh	r4, [r0, r4]
	ldrh	r7, [r1, r6]
	add	r1, r3, #2
	cmp	r2, r1
	rsb	r7, r7, r4
	strh	r7, [r0, r6]	@ movhi
	ble	.L135
	add	r6, r1, r8
	add	r1, r9, r1
	mov	r4, r1, asl #1
	ldr	r1, [sp, #4]
	mov	r6, r6, asl #1
	ldrh	r4, [r0, r4]
	ldrh	r7, [r1, r6]
	add	r1, r3, #3
	cmp	r2, r1
	rsb	r7, r7, r4
	strh	r7, [r0, r6]	@ movhi
	ble	.L135
	add	r6, r1, r8
	add	r1, r9, r1
	mov	r4, r1, asl #1
	ldr	r1, [sp, #4]
	mov	r6, r6, asl #1
	ldrh	r4, [r0, r4]
	ldrh	r7, [r1, r6]
	add	r1, r3, #4
	cmp	r2, r1
	rsb	r7, r7, r4
	strh	r7, [r0, r6]	@ movhi
	ble	.L135
	add	r6, r1, r8
	add	r1, r9, r1
	mov	r4, r1, asl #1
	ldr	r1, [sp, #4]
	mov	r6, r6, asl #1
	ldrh	r4, [r0, r4]
	ldrh	r7, [r1, r6]
	add	r1, r3, #5
	cmp	r2, r1
	rsb	r7, r7, r4
	strh	r7, [r0, r6]	@ movhi
	ble	.L135
	add	r4, r1, r8
	ldr	r6, [sp, #4]
	add	r1, r9, r1
	mov	r4, r4, asl #1
	mov	r1, r1, asl #1
	add	r3, r3, #6
	ldrh	r6, [r6, r4]
	cmp	r2, r3
	ldrh	r1, [r0, r1]
	rsb	r6, r6, r1
	strh	r6, [r0, r4]	@ movhi
	ble	.L135
	ldr	r2, [sp, #16]
	add	r9, r9, r3
	mov	r9, r9, asl #1
	add	r3, r3, r2
	ldr	r2, [sp, #4]
	mov	r3, r3, asl #1
	ldrh	r1, [r0, r9]
	ldrh	r4, [r2, r3]
	rsb	r1, r4, r1
	strh	r1, [r0, r3]	@ movhi
.L135:
	ldr	r3, [sp, #32]
	add	r5, r5, r10
	ldr	r2, [sp, #36]
	add	lr, lr, r10
	add	ip, ip, r10
	ldr	r7, [sp, #28]
	add	r3, r3, #1
	ldr	r9, [sp, #16]
	str	r3, [sp, #32]
	cmp	r3, r2
	ldr	r3, [sp, #12]
	add	r3, r3, r10
	str	r3, [sp, #12]
	bne	.L138
.L131:
	add	sp, sp, #68
	@ sp needed
	ldmfd	sp!, {r4, r5, r6, r7, r8, r9, r10, fp, pc}
.L140:
	ldr	r3, [sp, #28]
	mov	r4, ip
	ldr	r2, [sp, #8]
	mov	r7, lr
	mov	r6, r5
	add	r8, r10, r3
	add	r8, r0, r8
.L148:
	ldrh	r3, [r6], #2
	ldrh	r1, [r7], #2
	rsb	r3, r1, r3
	strh	r3, [r4], #2	@ movhi
	cmp	r4, r8
	bne	.L148
	str	r2, [sp, #8]
	b	.L135
	.size	VigTbl_RowDiffRecover, .-VigTbl_RowDiffRecover
	.ident	"GCC: (GNU Tools for ARM Embedded Processors) 4.9.3 20150529 (release) [ARM/embedded-4_9-branch revision 227977]"
