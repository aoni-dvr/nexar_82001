
	.text

	.globl	_Reset
    .global _start

_Reset:
_start:
    ldr     x1, =stack_top
    mov     sp, x1

    /* Dummy frame with saved fp=0 and lr=0 */
    mov     x29, 0
    stp     x29, x29, [sp, #-16]!
    mov     x29, sp

    // TODO: Clean cache if necessary.

set_mmu:
    // Initialize translation table control registers
    LDR     X1, =0x3520         // 4GB space 4KB granularity
    // Inner-shareable.
    MSR     TCR_EL3, X1         // Normal Inner and Outer Cacheable.
    LDR     X1, =0xFF440400     // ATTR0 Device-nGnRnE ATTR1 Device.
    MSR     MAIR_EL3, X1        // ATTR2 Normal Non-Cacheable.
    ADR     X0, ttb0_base       // ttb0_base must be a 4KB-aligned address.
    MSR     TTBR0_EL3, X0

    // Enable caches and the MMU.
    MRS     X0, SCTLR_EL3
    ORR     X0, X0, #(0x1 << 2)     // The C bit (data cache).
    ORR     X0, X0, #(0x1 << 12)    // The I bit (instruction cache).
    ORR     X0, X0, #0x1            // The M bit (MMU).
    MSR     SCTLR_EL3, X0
    DSB     SY
    ISB

    /*
     * Clear .bss, Suppose address is alignment
     */
    mov     x0, #0
    ldr     x1, =lk_bss_start
    ldr     x2, =lk_bss_end

loop_clr_bss:
    cmp     x1, x2
    bge     1f
    str     x0, [x1]
    add     x1, x1, #8
    b       loop_clr_bss
1:
    dsb     sy
    isb

    #  Create a stack frame and save any call-preserved registers
    stp     x29, x30, [sp, #-16]!
    stp     x27, x28, [sp, #-16]!
    stp     x25, x26, [sp, #-16]!
    stp     x23, x24, [sp, #-16]!
    stp     x21, x22, [sp, #-16]!
    stp     x19, x20, [sp, #-16]!

    bl      libc_init_array
#if 0
    /* TODO: argc, argv, (FP does not supprt) */
    ldr     x0, =g_argc
    ldr     x0, [x0]
    ldr     x1, =g_argv
#endif
    /* main() */
    bl      main
    bl      semi_exit
    /* Would not be here */
    b       .

.balign 0x10

// Put a 64-bit value with little endianness.
.macro PUT_64B high, low
.word \low
.word \high
.endm

// Create an entry for a 1GB block.
.macro BLOCK_1GB PA, ATTR_HI, ATTR_LO
PUT_64B \ATTR_HI, ((\PA) & 0xC0000000) | \ATTR_LO | 0x1
.endm

.align 12 // 12 for 4KB granule.
ttb0_base:
BLOCK_1GB 0x00000000, 0x00000, 0x000
BLOCK_1GB 0x40000000, 0x00000, 0x70C
BLOCK_1GB 0x80000000, 0x00000, 0x70C
BLOCK_1GB 0xC0000000, 0x00000, 0x000

