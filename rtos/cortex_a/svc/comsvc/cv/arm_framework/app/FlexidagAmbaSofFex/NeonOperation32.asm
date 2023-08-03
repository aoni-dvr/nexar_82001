	.cpu cortex-a53
	.eabi_attribute 28, 1
	.eabi_attribute 20, 1
	.eabi_attribute 21, 1
	.eabi_attribute 23, 3
	.eabi_attribute 24, 1
	.eabi_attribute 25, 1
	.eabi_attribute 26, 2
	.eabi_attribute 30, 2
	.eabi_attribute 34, 1
	.eabi_attribute 18, 4
	.file	"NeonOperation32.asm"
	.text
	.align	2
	.global	Do16bytesLessThanCmp
	.arch armv8-a
	.arch_extension crc
	.syntax unified
	.arm
	.fpu neon
	.type	Do16bytesLessThanCmp, %function
Do16bytesLessThanCmp:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	vld1.8	{d18-d19}, [r0]
	vld1.8	{d16-d17}, [r1]
	vcgt.u8	q8, q8, q9
	vst1.8	{d16-d17}, [r2]
	bx	lr
	.size	Do16bytesLessThanCmp, .-Do16bytesLessThanCmp
	.ident	"GCC: (GNU Tools for Arm Embedded Processors 8-2018-q4-major) 8.2.1 20181213 (release) [gcc-8-branch revision 267074 with DYNAMIC_REENT by Ambarella, r1]"
