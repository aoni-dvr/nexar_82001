
    .extern EL1_Reset_Handler

    .text
    .global _Reset

//----------------------------------------------------------------
// Entry point for the Reset handler
//----------------------------------------------------------------
_Reset:
//----------------------------------------------------------------
// EL2 Exception Vector Table
//----------------------------------------------------------------
// Note: LDR PC instructions are used here, though branch (B) instructions
// could also be used, unless the exception handlers are >32MB away.

EL2_Vectors:
        LDR PC, EL2_Reset_Addr
#if 1
        LDR PC, EL2_Reset_Addr
        LDR PC, EL2_Reset_Addr
        LDR PC, EL2_Reset_Addr
        LDR PC, EL2_Reset_Addr
        LDR PC, EL2_Reset_Addr
        LDR PC, EL2_Reset_Addr
        LDR PC, EL2_Reset_Addr
#else
        LDR PC, EL2_Undefined_Addr
        LDR PC, EL2_HVC_Addr
        LDR PC, EL2_Prefetch_Addr
        LDR PC, EL2_Abort_Addr
        LDR PC, EL2_HypModeEntry_Addr
        LDR PC, EL2_IRQ_Addr
        LDR PC, EL2_FIQ_Addr
#endif


EL2_Reset_Addr:         .word    EL2_Reset_Handler
EL2_Undefined_Addr:     .word    EL2_Undefined_Handler
EL2_HVC_Addr:           .word    EL2_HVC_Handler
EL2_Prefetch_Addr:      .word    EL2_Prefetch_Handler
EL2_Abort_Addr:         .word    EL2_Abort_Handler
EL2_HypModeEntry_Addr:  .word    EL2_HypModeEntry_Handler
EL2_IRQ_Addr:           .word    EL2_IRQ_Handler
EL2_FIQ_Addr:           .word    EL2_FIQ_Handler


//----------------------------------------------------------------
// EL2 Exception Handlers
//----------------------------------------------------------------

.type EL2_Undefined_Handler, "function"
EL2_Undefined_Handler:
        B   EL2_Undefined_Handler
.type EL2_HVC_Handler, "function"
EL2_HVC_Handler:
        B   EL2_HVC_Handler
.type EL2_Prefetch_Handler, "function"
EL2_Prefetch_Handler:
        B   EL2_Prefetch_Handler
.type EL2_Abort_Handler, "function"
EL2_Abort_Handler:
        B   EL2_Abort_Handler
.type EL2_HypModeEntry_Handler, "function"
EL2_HypModeEntry_Handler:
        B   EL2_HypModeEntry_Handler
.type EL2_IRQ_Handler, "function"
EL2_IRQ_Handler:
        B   EL2_IRQ_Handler
.type EL2_FIQ_Handler, "function"
EL2_FIQ_Handler:
        B   EL2_FIQ_Handler

//----------------------------------------------------------------
// EL1 Exception Vector Table
//----------------------------------------------------------------
// Note: LDR PC instructions are used here, though branch (B) instructions
// could also be used, unless the exception handlers are >32MB away.

    .align 5
EL1_Vectors:
        LDR PC, EL1_Reset_Addr
        LDR PC, EL1_Undefined_Addr
        LDR PC, EL1_SVC_Addr
        LDR PC, EL1_Prefetch_Addr
        LDR PC, EL1_Abort_Addr
        LDR PC, EL1_Reserved_Addr
        LDR PC, EL1_IRQ_Addr
        LDR PC, EL1_FIQ_Addr


EL1_Reset_Addr:     .word    EL1_Reset_Handler
EL1_Undefined_Addr: .word    EL1_Undefined_Handler
EL1_SVC_Addr:       .word    EL1_SVC_Handler
EL1_Prefetch_Addr:  .word    EL1_Prefetch_Handler
EL1_Abort_Addr:     .word    EL1_Abort_Handler
EL1_Reserved_Addr:  .word    EL1_Reserved
EL1_IRQ_Addr:       .word    EL1_IRQ_Handler
EL1_FIQ_Addr:       .word    EL1_FIQ_Handler


//----------------------------------------------------------------
// EL1 Exception Handlers
//----------------------------------------------------------------

.type EL1_Undefined_Handler, "function"
EL1_Undefined_Handler:
        B   EL1_Undefined_Handler
.type EL1_SVC_Handler, "function"
EL1_SVC_Handler:
        B   EL1_SVC_Handler
.type EL1_Prefetch_Handler, "function"
EL1_Prefetch_Handler:
        B   EL1_Prefetch_Handler
.type EL1_Abort_Handler, "function"
EL1_Abort_Handler:
        B   EL1_Abort_Handler
EL1_Reserved:
        B   EL1_Reserved
.type EL1_IRQ_Handler, "function"
EL1_IRQ_Handler:
        B   EL1_IRQ_Handler
.type EL1_FIQ_Handler, "function"
EL1_FIQ_Handler:
        B   EL1_FIQ_Handler

//----------------------------------------------------------------
// EL2 Reset Handler
//----------------------------------------------------------------
/*
    .section  RESET,"ax"
    .align 3
*/
#ifdef __THUMB__
    .thumb
#endif

.type EL2_Reset_Handler, "function"
EL2_Reset_Handler:
        //check current EL
        MRS R0, cpsr
        AND R0, R0, #0x1F
        CMP R0, #0x1A
        BNE EL1_Reset_Handler

        // Change EL2 exception base address
        LDR r0, =EL2_Vectors
        MCR p15, 4, r0, c12, c0, 0      //  Write to HVBAR

        // Init HSCTLR
        LDR r0, =0x30C50818             // See TRM for decoding
        MCR p15, 4, r0, c1, c0, 0       // Write to HSCTLR

        // Enable EL1 access to all IMP DEF registers
        LDR r0, =0x3781
        DSB
        MCR p15, 4, r0, c1, c0, 1       // Write to HACTLR
        ISB

        // Change EL1 exception base address
        LDR r0, =EL1_Vectors
        MCR p15, 0, r0, c12, c0, 0      // Write to VBAR

        // Go to SVC mode
        MRS r0, cpsr
        MOV r1, #0x13                   // Mode SVC
        BFI r0, r1, #0, #5
#ifdef __THUMB__
        ORR r0, r0, #(0x1 << 5)         // Set T bit
#endif
        MSR SPSR_hyp, r0
        LDR r0, =EL1_Reset_Handler
        MSR ELR_hyp, r0
        DSB
        ISB
        ERET

