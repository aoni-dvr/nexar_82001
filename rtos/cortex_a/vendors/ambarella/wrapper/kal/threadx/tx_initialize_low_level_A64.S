/**************************************************************************/
/*                                                                        */
/*            Copyright (c) 1996-2019 by Express Logic Inc.               */
/*                                                                        */
/*  This software is copyrighted by and is the sole property of Express   */
/*  Logic, Inc.  All rights, title, ownership, or other interests         */
/*  in the software remain the property of Express Logic, Inc.  This      */
/*  software may only be used in accordance with the corresponding        */
/*  license agreement.  Any unauthorized use, duplication, transmission,  */
/*  distribution, or disclosure of this software is expressly forbidden.  */
/*                                                                        */
/*  This Copyright notice may not be removed or modified without prior    */
/*  written consent of Express Logic, Inc.                                */
/*                                                                        */
/*  Express Logic, Inc. reserves the right to modify this software        */
/*  without notice.                                                       */
/*                                                                        */
/*  Express Logic, Inc.                     info@expresslogic.com         */
/*  11423 West Bernardo Court               http://www.expresslogic.com   */
/*  San Diego, CA  92127                                                  */
/*                                                                        */
/**************************************************************************/


/**************************************************************************/
/**************************************************************************/
/**                                                                       */
/** ThreadX Component                                                     */
/**                                                                       */
/**   Initialize                                                          */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/


/* #define TX_SOURCE_CODE  */


/* Include necessary system files.  */

/*
#include "tx_api.h"
#include "tx_initialize.h"
#include "tx_thread.h"
#include "tx_timer.h"
*/

/* This definition is used to enable the flow save/store floating register */
/* If this flow is not enabled, and system will disable floating registers in ISR */
#define SAVE_RESTORE_FLOAT_REGISTER_ENABLE

    .text
    .align 3
/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _tx_initialize_low_level                         Cortex-A5x-SMP/ARM */
/*                                                           5.5          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    William E. Lamie, Express Logic, Inc.                               */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function is responsible for any low-level processor            */
/*    initialization, including setting up interrupt vectors, setting     */
/*    up a periodic timer interrupt source, saving the system stack       */
/*    pointer for use in ISR processing later, and finding the first      */
/*    available RAM memory address for tx_application_define.             */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _tx_initialize_kernel_enter           ThreadX entry function        */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  08-01-2016     William E. Lamie         Initial Version 5.0           */
/*  06-01-2017     William E. Lamie         Modified comment(s),          */
/*                                            resulting in version 5.1    */
/*  12-12-2017     William E. Lamie         Modified comment(s),          */
/*                                            resulting in version 5.2    */
/*  04-01-2018     William E. Lamie         Modified comment(s),          */
/*                                            resulting in version 5.3    */
/*  09-01-2018     William E. Lamie         Modified comment(s),          */
/*                                            resulting in version 5.4    */
/*  03-26-2019     Scott Larson             Modified comment(s),          */
/*                                            align and map heap_limit,   */
/*                                            resulting in version 5.5    */
/*                                                                        */
/**************************************************************************/
/* VOID   _tx_initialize_low_level(VOID)
{ */
    .global  _tx_initialize_low_level
    .type    _tx_initialize_low_level, @function
_tx_initialize_low_level:

    MSR     DAIFSet, 0x3                        // Lockout interrupts


    /* Save the system stack pointer.  */
    /* _tx_thread_system_stack_ptr = (VOID_PTR) (sp);  */

    LDR     x0, =_tx_thread_system_stack_ptr    // Pickup address of system stack ptr
    MOV     x1, sp                              // Pickup SP
    SUB     x1, x1, #15                         //
    BIC     x1, x1, #0xF                        // Get 16-bit alignment
    STR     x1, [x0]                            // Store system stack

    /* Save the first available memory address.  */
    /* _tx_initialize_unused_memory =  (VOID_PTR) Image$$HEAP$$ZI$$Limit;  */
/* TBD */
//    LDR     x0, =_tx_initialize_unused_memory   // Pickup address of unused memory ptr
//    LDR     x1, =heap_limit                     // Pickup unused memory address - A free
//    LDR     x1, [x1]                            //   memory section must be setup after the
//                                                //   heap section.
//    STR     x1, [x0]                            // Store unused memory address

    /* Done, return to caller.  */

    RET                                         // Return to caller
/* } */

//    .align 3

    .globl  __tx_irq_handler

__tx_irq_handler:
    /* Jump to context save to save system context.  */
    BL      _tx_thread_context_save

    /* At this point execution is still in the IRQ mode.  The CPSR, point of
       interrupt, and all C scratch registers are available for use.  In
       addition, IRQ interrupts may be re-enabled - with certain restrictions -
       if nested IRQ interrupts are desired.  Interrupts may be re-enabled over
       small code sequences where lr is saved before enabling interrupts and
       restored after interrupts are again disabled.  */

#ifdef SAVE_RESTORE_FLOAT_REGISTER_ENABLE
    STP     q0,  q1,  [sp, #-32]!               // Save q0, q1
    STP     q2,  q3,  [sp, #-32]!               // Save q2, q3
    STP     q4,  q5,  [sp, #-32]!               // Save q4, q5
    STP     q6,  q7,  [sp, #-32]!               // Save q6, q7
    STP     q8,  q9,  [sp, #-32]!               // Save q8, q9
    STP     q10, q11, [sp, #-32]!               // Save q10, q11
    STP     q12, q13, [sp, #-32]!               // Save q12, q13
    STP     q14, q15, [sp, #-32]!               // Save q14, q15
    STP     q16, q17, [sp, #-32]!               // Save q16, q17
    STP     q18, q19, [sp, #-32]!               // Save q18, q19
    STP     q20, q21, [sp, #-32]!               // Save q20, q21
    STP     q22, q23, [sp, #-32]!               // Save q22, q23
    STP     q24, q25, [sp, #-32]!               // Save q24, q25
    STP     q26, q27, [sp, #-32]!               // Save q26, q27
    STP     q28, q29, [sp, #-32]!               // Save q28, q29
    STP     q30, q31, [sp, #-32]!               // Save q30, q31
    MRS     x2, FPSR                            // Pickup FPSR
    MRS     x3, FPCR                            // Pickup FPCR
    STP     x2, x3, [sp, #-16]!                 // Save FPSR, FPCR
#else
    MOV     X0, #(0)
    MSR     CPACR_EL1, X0
#endif

#if defined(CONFIG_XEN_SUPPORT) && !defined(AMBA_FWPROG)
    /* TODO */
#else
    /* Enable FIQ.  */
    MSR     DAIFClr, #1
#endif

    /* Application IRQ handlers can be called here!  */
    BL      AmbaRTSL_GicIsrIRQ                  /* Ambarella C-ISR for IRQ */

#ifdef SAVE_RESTORE_FLOAT_REGISTER_ENABLE
    LDP     x0, x1,   [sp], #16                 // Pickup FPSR, FPCR
    MSR     FPSR, x0                            // Recover FPSR
    MSR     FPCR, x1                            // Recover FPCR
    LDP     q30, q31, [sp], #32                 // Recover q30, q31
    LDP     q28, q29, [sp], #32                 // Recover q28, q29
    LDP     q26, q27, [sp], #32                 // Recover q26, q27
    LDP     q24, q25, [sp], #32                 // Recover q24, q25
    LDP     q22, q23, [sp], #32                 // Recover q22, q23
    LDP     q20, q21, [sp], #32                 // Recover q20, q21
    LDP     q18, q19, [sp], #32                 // Recover q18, q19
    LDP     q16, q17, [sp], #32                 // Recover q16, q17
    LDP     q14, q15, [sp], #32                 // Recover q14, q15
    LDP     q12, q13, [sp], #32                 // Recover q12, q13
    LDP     q10, q11, [sp], #32                 // Recover q10, q11
    LDP     q8,  q9,  [sp], #32                 // Recover q8, q9
    LDP     q6,  q7,  [sp], #32                 // Recover q6, q7
    LDP     q4,  q5,  [sp], #32                 // Recover q4, q5
    LDP     q2,  q3,  [sp], #32                 // Recover q2, q3
    LDP     q0,  q1,  [sp], #32                 // Recover q0, q1
#else
    MOV     X0, #(3 << 20)
    MSR     CPACR_EL1, X0
#endif

    /* Jump to context restore to restore system context.  */
    B       _tx_thread_context_restore

    .globl  __tx_fiq_handler
__tx_fiq_handler:
#ifndef TX_ENABLE_FIQ_SUPPORT
    B       __tx_fiq_handler
#else
    /* Jump to fiq context save to save system context.  */
    BL      _tx_thread_context_save

    /* At this point execution is still in the FIQ mode.  The CPSR, point of
       interrupt, and all C scratch registers are available for use.  */

#ifdef SAVE_RESTORE_FLOAT_REGISTER_ENABLE
    STP     q0,  q1,  [sp, #-32]!               // Save q0, q1
    STP     q2,  q3,  [sp, #-32]!               // Save q2, q3
    STP     q4,  q5,  [sp, #-32]!               // Save q4, q5
    STP     q6,  q7,  [sp, #-32]!               // Save q6, q7
    STP     q8,  q9,  [sp, #-32]!               // Save q8, q9
    STP     q10, q11, [sp, #-32]!               // Save q10, q11
    STP     q12, q13, [sp, #-32]!               // Save q12, q13
    STP     q14, q15, [sp, #-32]!               // Save q14, q15
    STP     q16, q17, [sp, #-32]!               // Save q16, q17
    STP     q18, q19, [sp, #-32]!               // Save q18, q19
    STP     q20, q21, [sp, #-32]!               // Save q20, q21
    STP     q22, q23, [sp, #-32]!               // Save q22, q23
    STP     q24, q25, [sp, #-32]!               // Save q24, q25
    STP     q26, q27, [sp, #-32]!               // Save q26, q27
    STP     q28, q29, [sp, #-32]!               // Save q28, q29
    STP     q30, q31, [sp, #-32]!               // Save q30, q31
    MRS     x2, FPSR                            // Pickup FPSR
    MRS     x3, FPCR                            // Pickup FPCR
    STP     x2, x3, [sp, #-16]!                 // Save FPSR, FPCR
#else
    MOV     X0, #(0 << 20)
    MSR     CPACR_EL1, X0
#endif

    /* Application FIQ handlers can be called here!  */
    BL      AmbaRTSL_GicIsrFIQ                  /* Ambarella C-ISR for FIQ */

#ifdef SAVE_RESTORE_FLOAT_REGISTER_ENABLE
    LDP     x0, x1,   [sp], #16                 // Pickup FPSR, FPCR
    MSR     FPSR, x0                            // Recover FPSR
    MSR     FPCR, x1                            // Recover FPCR
    LDP     q30, q31, [sp], #32                 // Recover q30, q31
    LDP     q28, q29, [sp], #32                 // Recover q28, q29
    LDP     q26, q27, [sp], #32                 // Recover q26, q27
    LDP     q24, q25, [sp], #32                 // Recover q24, q25
    LDP     q22, q23, [sp], #32                 // Recover q22, q23
    LDP     q20, q21, [sp], #32                 // Recover q20, q21
    LDP     q18, q19, [sp], #32                 // Recover q18, q19
    LDP     q16, q17, [sp], #32                 // Recover q16, q17
    LDP     q14, q15, [sp], #32                 // Recover q14, q15
    LDP     q12, q13, [sp], #32                 // Recover q12, q13
    LDP     q10, q11, [sp], #32                 // Recover q10, q11
    LDP     q8,  q9,  [sp], #32                 // Recover q8, q9
    LDP     q6,  q7,  [sp], #32                 // Recover q6, q7
    LDP     q4,  q5,  [sp], #32                 // Recover q4, q5
    LDP     q2,  q3,  [sp], #32                 // Recover q2, q3
    LDP     q0,  q1,  [sp], #32                 // Recover q0, q1
#else
    MOV     X0, #(3 << 20)
    MSR     CPACR_EL1, X0
#endif

    /* Jump to fiq context restore to restore system context.  */
    B       _tx_thread_context_restore
#endif

//heap_limit:
//    .quad (Image$$TOP_OF_RAM$$ZI$$Limit)

