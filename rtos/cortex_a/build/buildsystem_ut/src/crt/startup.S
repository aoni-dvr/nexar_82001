

.global _Reset
_Reset:
    ldr     sp, =stack_top

    /*
     * Enable all mode to access coprocessor
     * cf. http://infocenter.arm.com/help/topic/com.arm.doc.ddi0388f/DDI0388F_cortex_a9_r2p2_trm.pdf
     *     4.3.11 Coprocessor Access Control Register
     */
    mov     r0, #0xFFFFFFFF
    mcr     p15, 0, r0, c1, c0, 2

    /*
     * Enable NEON and VFP
     * cf. http://infocenter.arm.com/help/topic/com.arm.doc.ddi0408i/DDI0408I_cortex_a9_fpu_r4p1_trm.pdf
     *     2.5.3 Floating-Point Exception Register
     */
    ldr     r0, =1<<30      /* VFP extension is enabled and operates normally. */
    fmxr    fpexc, r0


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

    bl      main
    bl      semi_AppExit
    b       .


