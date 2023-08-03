	.arch armv8-a+crc
	.file	"NeonOperation64.asm"
	.text
	.align	2
	.p2align 4,,15
	.global	Do16bytesLessThanCmp
	.type	Do16bytesLessThanCmp, %function
Do16bytesLessThanCmp:
.LFB3825:
	.cfi_startproc
	ldr	q1, [x0]
	ldr	q0, [x1]
	cmhi	v0.16b, v0.16b, v1.16b
	str	q0, [x2]
	ret
	.cfi_endproc
.LFE3825:
	.size	Do16bytesLessThanCmp, .-Do16bytesLessThanCmp
	.ident	"GCC: (Linaro GCC 8.2-2018.08~dev) 8.2.1 20180802"
	.section	.note.GNU-stack,"",@progbits
