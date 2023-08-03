
    .extern lk_mmu_ttbr

	.text
#if 1 //ndef ARM_R52
    .global _Reset
    .global _start

_Reset:
_start:
#else
    .global EL1_Reset_Handler

EL1_Reset_Handler:
#endif
    /* Stack */
    ldr     sp, =stack_top

    /*
     * http://infocenter.arm.com/help/topic/com.arm.doc.ddi0438i/DDI0438I_cortex_a15_r4p0_trm.pdf
     */
    /* 4.3.29 Coprocessor Access Control Register */
    /* 14.2.2 Enabling Advanced SIMD and VFP extensions */
    // Enable Non-secure access to CP10 and CP11 and clear the NSASEDIS bit in the NSACR
    mrc     p15, 0, r0, c1, c0, 2
    orr     r0, r0, #(3<<10)	    // Enable Non-secure access to CP10 and CP11
    bic     r0, r0, #(3<<14)	    // Clear NSASEDIS bit
    mcr     p15, 0, r0, c1, c0, 2
    isb
    // Enable access to CP10 and CP11 and clear the ASEDIS bit in the CPACR
    mov     r0, #0x00F00000
    mcr     p15, 0, r0, c1, c0, 2
    isb
    // Set the FPEXC.EN bit to enable Advanced SIMD and VFP
    mov     r0, #0x40000000
    vmsr    fpexc, r0
    isb

    /*
     * http://infocenter.arm.com/help/topic/com.arm.doc.dai0527a/DAI0527A_baremetal_boot_code_for_ARMv8_A_processors.pdf
     * 4.3 Configuring the MMU and caches
     */
    // Disable L1 Caches.
    mrc     p15, 0, r1, c1, C0, 0   // Read SCTLR.
    bic     r1, r1, #(0x1 << 2)     // Disable D Cache.
    mcr     p15, 0, r1, c1, c0, 0   // Write SCTLR.

    // Invalidate Data cache to create general-purpose code. Calculate the
    // cache size first and loop through each set + way.
    mov     r0, #0x0                // R0 = 0x0 for L1 dcache 0x2 for L2 dcache.
    mcr     p15, 2, r0, c0, c0, 0   // CSSELR Cache Size Selection Register.
    mrc     p15, 1, r4, c0, c0, 0   // CCSIDR read Cache Size.
    and     r1, r4, #0x7
    add     r1, r1, #0x4            // R1 = Cache Line Size.
    ldr     r3, =0x7FFF
    and     r2, r3, r4, lsr #13     // R2 = Cache Set Number - 1.
    ldr     r3, =0x3FF
    and     r3, r3, r4, lsr #3      // R3 = Cache Associativity Number - 1.
    clz     r4, r3                  // R4 = way position in CISW instruction.
    mov     r5, #0                  // R5 = way loop counter.
way_loop:
    mov     r6, #0                  // R6 = set loop counter.
set_loop:
    orr     r7, r0, r5, lsl r4      // Set way.
    orr     r7, r7, r6, lsl r1      // Set set.
    mcr     p15, 0, r7, c7, c6, 2   // DCCISW R7.
    add     r6, r6, #1              // Increment set counter.
    cmp     r6, r2                  // Last set reached yet?
    ble     set_loop                // If not, iterate set_loop,
    add     r5, r5, #1              // else, next way.
    cmp     r5, r3                  // Last way reached yet?
    ble     way_loop                // if not, iterate way_loop.

#ifndef ARM_R52
    // Initialize TTBCR.
    mov     r0, #0                  // Use short descriptor.
    mcr     p15, 0, r0, c2, c0, 2   // Base address is 16KB aligned.
                                    // Perform translation table walk for TTBR0.
    // Initialize DACR.
    ldr     r1, =0x55555555         // Set all domains as clients.
    mcr     p15, 0, r1, c3, c0, 0   // Accesses are checked against the
                                    // permission bits in the translation tables.
    // Initialize SCTLR.AFE.
    mrc     p15, 0, r1, c1, c0, 0   // Read SCTLR.
    bic     r1, r1, #(0x1 <<29)     // Set AFE to 0 and disable Access Flag.
    mcr     p15, 0, r1, c1, c0, 0   // Write SCTLR.

    // Initialize TTBR0.
    ldr     r0, =lk_mmu_ttbr        // ttb0_base must be a 16KB-aligned address.
    mov     r1, #0x2B               // The translation table walk is normal, inner
    orr     r1, r0, r1              // and outer cacheable, WB WA, and inner
    mcr     p15, 0, r1, c2, c0, 0   // shareable.

    // Set up translation table entries in memory
    ldr     r4, =0x00100000 // Increase 1MB address each time.
    /*
     * Set up translation table descriptor with
     * Secure, global, full accessibility,
     * executable.
     * Domain 0, Shareable, Normal cacheable memory
     */
    ldr     r2, =0x00015C06
    /* executes the loop N times to set up
     * N=1024 descriptors to cover 0 - (N * 1M) B memory.
     * lk_mmu_ttbr_end is reserved for 4G
     * So MMU will cover 0 - 0x1_0000_0000
     */
    ldr     r3, =4096

loop:
    str     r2, [r0], #4    // Build a page table section entry.
    add     r2, r2, r4      // Update address part for next descriptor.
    subs    r3, #1
    bne     loop
    dsb

    // Enable caches and the MMU.
    mrc     p15, 0, r1, c1, c0, 0   // Read SCTLR.
    orr     r1, r1, #(0x1 << 2)     // The C bit (data cache).
    orr     r1, r1, #(0x1 << 12)    // The I bit (instruction cache).
    orr     r1, r1, #0x1            // The M bit (MMU).
    mcr     p15, 0, r1, c1, c0, 0   // Write SCTLR.
    dsb
    isb
#endif // MMU, #ifndef ARM_R52


    /*
     * Clear .bss
     */
    mov     r0, #0
    ldr     r1, =lk_bss_start
    ldr     r2, =lk_bss_end

loop_clr_bss:
    cmp     r1, r2
    strlo   r0, [r1], #4    /* If not reach end, store 0 and increase pointer */
    blo     loop_clr_bss

    blx     libc_init_array
#if 0
    /* TODO: argc, argv, (FP does not supprt) */
    ldr     r0, =g_argc
    ldr     r0, [r0]
    ldr     r1, =g_argv
#endif
    /* main() */
    blx     main
#ifndef CONFIG_CC_USESTD
    blx     semi_exit
#else
    blx     _exit
#endif
    /* Would not be here */
    b       .


