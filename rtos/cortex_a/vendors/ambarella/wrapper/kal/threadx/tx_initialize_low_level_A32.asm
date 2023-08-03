@/**************************************************************************/
@/*                                                                        */
@/*            Copyright (c) 1996-2017 by Express Logic Inc.               */
@/*                                                                        */
@/*  This software is copyrighted by and is the sole property of Express   */
@/*  Logic, Inc.  All rights, title, ownership, or other interests         */
@/*  in the software remain the property of Express Logic, Inc.  This      */
@/*  software may only be used in accordance with the corresponding        */
@/*  license agreement.  Any unauthorized use, duplication, transmission,  */
@/*  distribution, or disclosure of this software is expressly forbidden.  */
@/*                                                                        */
@/*  This Copyright notice may not be removed or modified without prior    */
@/*  written consent of Express Logic, Inc.                                */
@/*                                                                        */
@/*  Express Logic, Inc. reserves the right to modify this software        */
@/*  without notice.                                                       */
@/*                                                                        */
@/*  Express Logic, Inc.                     info@expresslogic.com         */
@/*  11423 West Bernardo Court               http://www.expresslogic.com   */
@/*  San Diego, CA  92127                                                  */
@/*                                                                        */
@/**************************************************************************/
@
@
@/**************************************************************************/
@/**************************************************************************/
@/**                                                                       */
@/** ThreadX Component                                                     */
@/**                                                                       */
@/**   Initialize                                                          */
@/**                                                                       */
@/**************************************************************************/
@/**************************************************************************/
@
@
@#define TX_SOURCE_CODE
@
/* This definition is used to enable the flow save/store floating register */
/* If this flow is not enabled, and system will disable floating registers in ISR */
//#define SAVE_RESTORE_FLOAT_REGISTER_ENABLE
@
@/* Include necessary system files.  */
@
@#include "tx_api.h"
@#include "tx_initialize.h"
@#include "tx_thread.h"
@#include "tx_timer.h"
@
@
@   EXTERN      _tx_thread_system_stack_ptr
@   EXTERN      _tx_initialize_unused_memory
@   EXTERN      _tx_build_options
@   EXTERN      _tx_version_id
@
@
@
@/* Define the FREE_MEM segment that will specify where free memory is
@   defined.  This must also be located in at the end of other RAM segments
@   in the linker control file.  The value of this segment is what is passed
@   to tx_application_define.  */
@
@   RSEG    FREE_MEM:DATA
@   .globl  __tx_free_memory_start
@__tx_free_memory_start:
@   .word   0x0
@
@
@
@/**************************************************************************/
@/*                                                                        */
@/*  FUNCTION                                               RELEASE        */
@/*                                                                        */
@/*    _tx_initialize_low_level                          SMP/Cortex-A9/IAR */
@/*                                                           5.5          */
@/*  AUTHOR                                                                */
@/*                                                                        */
@/*    William E. Lamie, Express Logic, Inc.                               */
@/*                                                                        */
@/*  DESCRIPTION                                                           */
@/*                                                                        */
@/*    This function is responsible for any low-level processor            */
@/*    initialization, including setting up interrupt vectors, setting     */
@/*    up a periodic timer interrupt source, saving the system stack       */
@/*    pointer for use in ISR processing later, and finding the first      */
@/*    available RAM memory address for tx_application_define.             */
@/*                                                                        */
@/*  INPUT                                                                 */
@/*                                                                        */
@/*    None                                                                */
@/*                                                                        */
@/*  OUTPUT                                                                */
@/*                                                                        */
@/*    None                                                                */
@/*                                                                        */
@/*  CALLS                                                                 */
@/*                                                                        */
@/*    None                                                                */
@/*                                                                        */
@/*  CALLED BY                                                             */
@/*                                                                        */
@/*    _tx_initialize_kernel_enter           ThreadX entry function        */
@/*                                                                        */
@/*  RELEASE HISTORY                                                       */
@/*                                                                        */
@/*    DATE              NAME                      DESCRIPTION             */
@/*                                                                        */
@/*  12-12-2012     William E. Lamie         Initial Version 5.0           */
@/*  05-01-2014     William E. Lamie         Modified comment(s),          */
@/*                                            resulting in version 5.1    */
@/*  03-01-2015     William E. Lamie         Modified comment(s),          */
@/*                                            resulting in version 5.2    */
@/*  09-01-2015     William E. Lamie         Modified comment(s),          */
@/*                                            resulting in version 5.3    */
@/*  08-19-2016     William E. Lamie         Modified comment(s),          */
@/*                                            resulting in version 5.4    */
@/*  06-01-2017     William E. Lamie         Modified comment(s),          */
@/*                                            resulting in version 5.5    */
@/*                                                                        */
@/**************************************************************************/
@VOID   _tx_initialize_low_level(VOID)
@{
    .globl  _tx_initialize_low_level
_tx_initialize_low_level:
@
@    /* Pickup the start of free memory.  */
@
@    LDR     r0, =__tx_free_memory_start         ; Get end of non-initialized RAM area
@
@    /* Save the system stack pointer.  */
@    _tx_thread_system_stack_ptr[core] = (VOID_PTR) (sp);
@
@    /* Save the first available memory address.  */
@    _tx_initialize_unused_memory =  (VOID_PTR) FREE_MEM;
@
@   LDR     r2, =_tx_initialize_unused_memory   ; Pickup unused memory ptr address
@   STR     r0, [r2, #0]                        ; Save first free memory address
@
@    /* Done, return to caller.  */
@
#ifdef TX_THUMB
    BX      lr                                  @ Return to caller
#else
    MOV     pc, lr                              @ Return to caller
#endif
@}
@
@
    .globl  __tx_irq_handler
    .globl  __tx_irq_processing_return
__tx_irq_handler:
@
@    /* Jump to context save to save system context.  */
    B       _tx_thread_context_save
__tx_irq_processing_return:
@
@    /* Enable FIQ.  */
    CPSIE   f
@
@    /* At this point execution is still in the IRQ mode.  The CPSR, point of
@       interrupt, and all C scratch registers are available for use.  In
@       addition, IRQ interrupts may be re-enabled - with certain restrictions -
@       if nested IRQ interrupts are desired.  Interrupts may be re-enabled over
@       small code sequences where lr is saved before enabling interrupts and
@       restored after interrupts are again disabled.  */
@
@    /* Interrupt nesting is allowed after calling _tx_thread_irq_nesting_start
@       from IRQ mode with interrupts disabled.  This routine switches to the
@       system mode and returns with IRQ interrupts enabled.
@
@       NOTE:  It is very important to ensure all IRQ interrupts are cleared
@       prior to enabling nested IRQ interrupts.  */
#ifdef TX_ENABLE_IRQ_NESTING
    BL      _tx_thread_irq_nesting_start
#endif
@
@    /* Application IRQ handlers can be called here!  */

#ifdef SAVE_RESTORE_FLOAT_REGISTER_ENABLE
    VMRS    r0, FPSCR                           @ Pickup the FPSCR
    STR     r0, [sp, #-4]!                      @ Save FPSCR
    VSTMDB  sp!, {D16-D31}                      @ Save D16-D31
    VSTMDB  sp!, {D0-D15}                       @ Save D0-D15
#else
    VMRS    r0, FPSCR                           @ Pickup the FPSCR
    STR     r0, [sp, #-4]!                      @ Save FPSCR
    MOV     R0, #0
    VMSR    FPSCR, R0
    VMSR    FPEXC, R0
#endif

    BL      AmbaRTSL_GicIsrIRQ                  @ Ambarella C-ISR for IRQ

#ifdef SAVE_RESTORE_FLOAT_REGISTER_ENABLE
    VLDMIA  sp!, {D0-D15}                       @ Recover D0-D15
    VLDMIA  sp!, {D16-D31}                      @ Recover D16-D31
    LDR     r0, [sp], #4                        @ Pickup FPSCR
    VMSR    FPSCR, r0                           @ Restore FPSCR
#else
    MOV     R0, #0x40000000             /* FPEXC[30] = Enable Advanced SIMD and VFP extensions */
    VMSR    FPEXC, R0
    LDR     r0, [sp], #4                        @ Pickup FPSCR
    VMSR    FPSCR, r0                           @ Restore FPSCR
#endif

@
@    /* If interrupt nesting was started earlier, the end of interrupt nesting
@       service must be called before returning to _tx_thread_context_restore.
@       This routine returns in processing in IRQ mode with interrupts disabled.  */
#ifdef TX_ENABLE_IRQ_NESTING
    BL      _tx_thread_irq_nesting_end
#endif
@
@    /* Jump to context restore to restore system context.  */
    B       _tx_thread_context_restore
@
#ifdef TX_ENABLE_FIQ_SUPPORT
    .globl  __tx_fiq_handler
    .globl  __tx_fiq_processing_return
__tx_fiq_handler:
@
@    /* Jump to fiq context save to save system context.  */
    B       _tx_thread_fiq_context_save
__tx_fiq_processing_return:
@
@    /* At this point execution is still in the FIQ mode.  The CPSR, point of
@       interrupt, and all C scratch registers are available for use.  */
@
@    /* Interrupt nesting is allowed after calling _tx_thread_fiq_nesting_start
@       from FIQ mode with interrupts disabled.  This routine switches to the
@       system mode and returns with FIQ interrupts enabled.
@
@       NOTE:  It is very important to ensure all FIQ interrupts are cleared
@       prior to enabling nested FIQ interrupts.  */
#ifdef TX_ENABLE_FIQ_NESTING
    BL      _tx_thread_fiq_nesting_start
#endif
@
@    /* Application FIQ handlers can be called here!  */

#ifdef SAVE_RESTORE_FLOAT_REGISTER_ENABLE
    VMRS    r0, FPSCR                           @ Pickup the FPSCR
    STR     r0, [sp, #-4]!                      @ Save FPSCR
    VSTMDB  sp!, {D16-D31}                      @ Save D16-D31
    VSTMDB  sp!, {D0-D15}                       @ Save D0-D15
#else
    MOV     R0, #0x40000000             /* FPEXC[30] = Enable Advanced SIMD and VFP extensions */
    VMSR    FPEXC, R0

    VMRS    r0, FPSCR                           @ Pickup the FPSCR
    STR     r0, [sp, #-4]!                      @ Save FPSCR

    MOV     R0, #0
    VMSR    FPSCR, R0
    VMSR    FPEXC, R0
#endif

    BL      AmbaRTSL_GicIsrFIQ                  @ Ambarella C-ISR for FIQ

#ifdef SAVE_RESTORE_FLOAT_REGISTER_ENABLE
    VLDMIA  sp!, {D0-D15}                       @ Recover D0-D15
    VLDMIA  sp!, {D16-D31}                      @ Recover D16-D31
    LDR     r0, [sp], #4                        @ Pickup FPSCR
    VMSR    FPSCR, r0                           @ Restore FPSCR
#else
    MOV     R0, #0x40000000             /* FPEXC[30] = Enable Advanced SIMD and VFP extensions */
    VMSR    FPEXC, R0
    LDR     r0, [sp], #4                        @ Pickup FPSCR
    VMSR    FPSCR, r0                           @ Restore FPSCR
#endif

@
@    /* If interrupt nesting was started earlier, the end of interrupt nesting
@       service must be called before returning to _tx_thread_fiq_context_restore.  */
#ifdef TX_ENABLE_FIQ_NESTING
    BL      _tx_thread_fiq_nesting_end
#endif
@
@    /* Jump to fiq context restore to restore system context.  */
    B       _tx_thread_fiq_context_restore
@
@
#else
    .globl  __tx_fiq_handler
__tx_fiq_handler:
    B       __tx_fiq_handler                    @ FIQ interrupt handler
#endif
@
@

.globl  _tx_initilaize_rtos_core_id
_tx_initilaize_rtos_core_id:
    LDR     R2, =_tx_rtos_core_id
    STR     R0, [R2]

#ifdef TX_THUMB
    BX      lr                                  @ Return to caller
#else
    MOV     pc, lr                              @ Return to caller
#endif
    _tx_build_options: .word                    @ Reference to ensure it comes in
    _tx_version_id: .word                       @ Reference to ensure it comes in
