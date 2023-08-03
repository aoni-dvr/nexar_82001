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
/**   Port Specific                                                       */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/


/**************************************************************************/
/*                                                                        */
/*  PORT SPECIFIC C INFORMATION                            RELEASE        */
/*                                                                        */
/*    tx_port.h                                        Cortex-A5x-SMP/ARM */
/*                                                           5.5          */
/*                                                                        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    William E. Lamie, Express Logic, Inc.                               */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This file contains data type definitions that make the ThreadX      */
/*    real-time kernel function identically on a variety of different     */
/*    processor architectures.  For example, the size or number of bits   */
/*    in an "int" data type vary between microprocessor architectures and */
/*    even C compilers for the same microprocessor.  ThreadX does not     */
/*    directly use native C data types.  Instead, ThreadX creates its     */
/*    own special types that can be mapped to actual data types by this   */
/*    file to guarantee consistency in the interface and functionality.   */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  08-01-2016     William E. Lamie         Initial Cortex-A5x-SMP/ARM    */
/*                                            Support Version 5.0         */
/*  06-01-2017     William E. Lamie         Modified comment(s), added    */
/*                                            macros/defines to utilize   */
/*                                            generic C code, removed     */
/*                                            defines and prototypes that */
/*                                            were moved to tx_api.h, and */
/*                                            modified code for MISRA     */
/*                                            compliance, and updated     */
/*                                            version string, resulting   */
/*                                            in version 5.1              */
/*  12-12-2017     William E. Lamie         Modified comment(s), and      */
/*                                            updated version string,     */
/*                                            resulting in version 5.2    */
/*  04-01-2018     William E. Lamie         Modified comment(s), added    */
/*                                            macros/defines to utilize   */
/*                                            generic C code, and updated */
/*                                            version ID, resulting in    */
/*                                            version 5.3                 */
/*  09-01-2018     William E. Lamie         Modified comment(s), and      */
/*                                            updated version ID,         */
/*                                            resulting in version 5.4    */
/*  03-26-2019     Scott Larson             Modified comment(s),          */
/*                                            calculate CPU ID using      */
/*                                            core and cluster IDs.       */
/*                                            resulting in version 5.5    */
/*                                                                        */
/**************************************************************************/

#ifndef TX_PORT_H
#define TX_PORT_H

/************* Define ThreadX SMP constants.  *************/

/* Define the ThreadX SMP maximum number of cores.  */
#ifdef CORE_ID
#define TX_THREAD_SMP_MAX_CORES                 CORE_ID
#define TX_THREAD_SMP_CORE_MASK                 ((1U << (ULONG)CORE_ID) - 1U)
#elif defined(CONFIG_KAL_THREADX_SMP_NUM_CORES)
#if defined(CONFIG_SOC_CV28) || defined(CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
#if (CONFIG_KAL_THREADX_SMP_NUM_CORES > 2)
#error (CONFIG_KAL_THREADX_SMP_NUM_CORES > 2)
#define TX_THREAD_SMP_MAX_CORES                 2
#define TX_THREAD_SMP_CORE_MASK                 0x3
#else
#define TX_THREAD_SMP_MAX_CORES                 CONFIG_KAL_THREADX_SMP_NUM_CORES
#define TX_THREAD_SMP_CORE_MASK                 ((1U << (ULONG)CONFIG_KAL_THREADX_SMP_NUM_CORES) - 1U)
#endif
#else
#define TX_THREAD_SMP_MAX_CORES                 CONFIG_KAL_THREADX_SMP_NUM_CORES
#define TX_THREAD_SMP_CORE_MASK                 ((1U << (ULONG)CONFIG_KAL_THREADX_SMP_NUM_CORES) - 1U)
#endif
#endif

#ifndef TX_THREAD_SMP_MAX_CORES
#if defined(CONFIG_SOC_CV28) || defined(CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
#define TX_THREAD_SMP_MAX_CORES                 2
#else
#define TX_THREAD_SMP_MAX_CORES                 4
#endif
#endif


/* Define the ThreadX SMP core mask. */

#ifndef TX_THREAD_SMP_CORE_MASK
#define TX_THREAD_SMP_CORE_MASK                 0xF            /* Where bit 0 represents Core 0, bit 1 represents Core 1, etc.  */
#endif


/* Define INLINE_DECLARE to whitespace for ARM compiler.  */

//#define INLINE_DECLARE


/* Define ThreadX SMP initialization macro.  */

#define TX_PORT_SPECIFIC_PRE_INITIALIZATION


/* Define ThreadX SMP pre-scheduler initialization.  */

#define TX_PORT_SPECIFIC_PRE_SCHEDULER_INITIALIZATION


/* Enable the inter-core interrupt logic.  */

#define TX_THREAD_SMP_INTER_CORE_INTERRUPT


/* Determine if there is customer-specific wakeup logic needed.  */

#ifdef TX_THREAD_SMP_WAKEUP_LOGIC

/* Include customer-specific wakeup code.  */

#include "tx_thread_smp_core_wakeup.h"
#else

#define TX_THREAD_SMP_DEFAULT_WAKEUP_LOGIC
#ifdef TX_THREAD_SMP_DEFAULT_WAKEUP_LOGIC

/* Default wakeup code.  */
#define TX_THREAD_SMP_WAKEUP_LOGIC
#define TX_THREAD_SMP_WAKEUP(i)                _tx_thread_smp_core_preempt(i)
#endif
#endif


/* Ensure that the in-line resume/suspend define is not allowed.  */

#ifdef TX_INLINE_THREAD_RESUME_SUSPEND
#undef TX_INLINE_THREAD_RESUME_SUSPEND
#endif


/************* End ThreadX SMP constants.  *************/


/* Determine if the optional ThreadX user define file should be used.  */
#ifndef TX_INCLUDE_USER_DEFINE_FILE
#define TX_INCLUDE_USER_DEFINE_FILE
#endif  /* TX_INCLUDE_USER_DEFINE_FILE */

#ifdef TX_INCLUDE_USER_DEFINE_FILE
/* Yes, include the user defines in tx_user.h. The defines in this file may
   alternately be defined on the command line.  */

#include "tx_user.h"
#endif


/* Define compiler library include files.  */
/* TBD
#include <stdlib.h>
#include <string.h>
*/

/* Define ThreadX basic types for this port.  */

#define VOID                                    void
typedef char                                    CHAR;
typedef unsigned char                           UCHAR;
typedef int                                     INT;
typedef unsigned int                            UINT;
/* TBD */
/* typedef int                                     LONG; */
/* typedef unsigned int                            UINT32; */
/* typedef unsigned long long                      ULONG64; */
typedef short                                   SHORT;
typedef unsigned short                          USHORT;

#if 0 /* Move the necessary definitions here rather than including the entire header file */
#include "tx_execution_profile.h"
#else
#ifdef TX_EXECUTION_64BIT_TIME
typedef UINT64              EXECUTION_TIME;
typedef UINT64              EXECUTION_TIME_SOURCE_TYPE;
#define TX_THREAD_EXECUTION_TIME        EXECUTION_TIME  tx_thread_execution_time_total; \
                                        EXECUTION_TIME  tx_thread_execution_time_last_start;
#define TX_EXECUTION_MAX_TIME_SOURCE    0xFFFFFFFFFFFFFFFFU
#else
typedef UINT32                   EXECUTION_TIME;
typedef UINT32                   EXECUTION_TIME_SOURCE_TYPE;
#define TX_THREAD_EXECUTION_TIME        EXECUTION_TIME  tx_thread_execution_time_total; \
                                        EXECUTION_TIME  tx_thread_execution_time_last_start;
#define TX_EXECUTION_MAX_TIME_SOURCE    0xFFFFFFFFU
#endif
#endif

/* Override the alignment type to use 64-bit alignment and storage for pointers.  */

#define ALIGN_TYPE_DEFINED
typedef unsigned long long                      ALIGN_TYPE;


/* Override the free block marker for byte pools to be a 64-bit constant.   */

#define TX_BYTE_BLOCK_FREE                      ((ALIGN_TYPE) 0xFFFFEEEEFFFFEEEE)


/* Define the priority levels for ThreadX.  Legal values range
   from 32 to 1024 and MUST be evenly divisible by 32.  */

#ifndef TX_MAX_PRIORITIES
#define TX_MAX_PRIORITIES                       32
#endif


/* Define the minimum stack for a ThreadX thread on this processor. If the size supplied during
   thread creation is less than this value, the thread create call will return an error.  */

#ifndef TX_MINIMUM_STACK
#define TX_MINIMUM_STACK                        200         /* Minimum stack size for this port  */
#endif


/* Define the system timer thread's default stack size and priority.  These are only applicable
   if TX_TIMER_PROCESS_IN_ISR is not defined.  */

#ifndef TX_TIMER_THREAD_STACK_SIZE
#define TX_TIMER_THREAD_STACK_SIZE              4096        /* Default timer thread stack size  */
#endif

#ifndef TX_TIMER_THREAD_PRIORITY
#define TX_TIMER_THREAD_PRIORITY                0           /* Default timer thread priority    */
#endif


/* Define various constants for the ThreadX ARM port.  */

#define TX_INT_DISABLE                          0xC0        /* Disable IRQ & FIQ interrupts     */
#define TX_INT_ENABLE                           0x00        /* Enable IRQ & FIQ interrupts            */


/* Define the clock source for trace event entry time stamp. The following two item are port specific.
   For example, if the time source is at the address 0x0a800024 and is 16-bits in size, the clock
   source constants would be:

#define TX_TRACE_TIME_SOURCE                    *((UINT32 *) 0x0a800024)
#define TX_TRACE_TIME_MASK                      0x0000FFFFUL

*/

#ifndef TX_MISRA_ENABLE
#ifndef TX_TRACE_TIME_SOURCE
#define TX_TRACE_TIME_SOURCE                    _tx_thread_smp_time_get()
#endif
#else
#ifndef TX_TRACE_TIME_SOURCE
UINT32   _tx_misra_time_stamp_get(VOID);
#define TX_TRACE_TIME_SOURCE                    _tx_misra_time_stamp_get()
#endif
#endif
#ifndef TX_TRACE_TIME_MASK
#define TX_TRACE_TIME_MASK                      0xFFFFFFFFUL
#endif


/* Define the port specific options for the _tx_build_options variable. This variable indicates
   how the ThreadX library was built.  */

#ifdef TX_ENABLE_FIQ_SUPPORT
#define TX_FIQ_ENABLED                          1
#else
#define TX_FIQ_ENABLED                          0
#endif

#ifdef TX_ENABLE_IRQ_NESTING
#define TX_IRQ_NESTING_ENABLED                  2
#else
#define TX_IRQ_NESTING_ENABLED                  0
#endif

#ifdef TX_ENABLE_FIQ_NESTING
#define TX_FIQ_NESTING_ENABLED                  4
#else
#define TX_FIQ_NESTING_ENABLED                  0
#endif

#define TX_PORT_SPECIFIC_BUILD_OPTIONS          (TX_FIQ_ENABLED | TX_IRQ_NESTING_ENABLED | TX_FIQ_NESTING_ENABLED)


/* Define the in-line initialization constant so that modules with in-line
   initialization capabilities can prevent their initialization from being
   a function call.  */

#ifdef TX_MISRA_ENABLE
#define TX_DISABLE_INLINE
#else
#define TX_INLINE_INITIALIZATION
#endif


/* Determine whether or not stack checking is enabled. By default, ThreadX stack checking is
   disabled. When the following is defined, ThreadX thread stack checking is enabled.  If stack
   checking is enabled (TX_ENABLE_STACK_CHECKING is defined), the TX_DISABLE_STACK_FILLING
   define is negated, thereby forcing the stack fill which is necessary for the stack checking
   logic.  */

#ifndef TX_MISRA_ENABLE
#ifdef TX_ENABLE_STACK_CHECKING
#undef TX_DISABLE_STACK_FILLING
#endif
#endif


/* Define the TX_THREAD control block extensions for this port. The main reason
   for the multiple macros is so that backward compatibility can be maintained with
   existing ThreadX kernel awareness modules.  */

#define TX_THREAD_EXTENSION_0
#define TX_THREAD_EXTENSION_1
#define TX_THREAD_EXTENSION_2                               UINT32       _tx_thread_fp_enable;

#define TX_PAD_SIZE         (1864)

#if defined(CONFIG_SSP_THREADX_NEWLIB) && !defined(VCAST_AMBA)
#ifndef _REENT_H_
typedef struct _reent {
    char tx_pad[TX_PAD_SIZE];
};
#endif
#define TX_THREAD_EXTENSION_3                               TX_THREAD_EXECUTION_TIME \
                                                            struct _reent   impure_data;
#else
#define TX_THREAD_EXTENSION_3                               TX_THREAD_EXECUTION_TIME \
                                                            char tx_pad[TX_PAD_SIZE];
#endif


/* Define the port extensions of the remaining ThreadX objects.  */

#define TX_BLOCK_POOL_EXTENSION
#define TX_BYTE_POOL_EXTENSION
#define TX_EVENT_FLAGS_GROUP_EXTENSION
#define TX_MUTEX_EXTENSION
#define TX_QUEUE_EXTENSION
#define TX_SEMAPHORE_EXTENSION
#define TX_TIMER_EXTENSION


/* Define the user extension field of the thread control block.  Nothing
   additional is needed for this port so it is defined as white space.  */

#ifndef TX_THREAD_USER_EXTENSION
#define TX_THREAD_USER_EXTENSION                            UINT32 UserValue; \
                                                            ULONG AmbaExtValue;
#endif

/* Define the macros for processing extensions in tx_thread_create, tx_thread_delete,
   tx_thread_shell_entry, and tx_thread_terminate.  */


#define TX_THREAD_CREATE_EXTENSION(thread_ptr)
#define TX_THREAD_DELETE_EXTENSION(thread_ptr)
#define TX_THREAD_COMPLETED_EXTENSION(thread_ptr)
#define TX_THREAD_TERMINATED_EXTENSION(thread_ptr)


/* Define the ThreadX object creation extensions for the remaining objects.  */

#define TX_BLOCK_POOL_CREATE_EXTENSION(pool_ptr)
#define TX_BYTE_POOL_CREATE_EXTENSION(pool_ptr)
#define TX_EVENT_FLAGS_GROUP_CREATE_EXTENSION(group_ptr)
#define TX_MUTEX_CREATE_EXTENSION(mutex_ptr)
#define TX_QUEUE_CREATE_EXTENSION(queue_ptr)
#define TX_SEMAPHORE_CREATE_EXTENSION(semaphore_ptr)
#define TX_TIMER_CREATE_EXTENSION(timer_ptr)


/* Define the ThreadX object deletion extensions for the remaining objects.  */

#define TX_BLOCK_POOL_DELETE_EXTENSION(pool_ptr)
#define TX_BYTE_POOL_DELETE_EXTENSION(pool_ptr)
#define TX_EVENT_FLAGS_GROUP_DELETE_EXTENSION(group_ptr)
#define TX_MUTEX_DELETE_EXTENSION(mutex_ptr)
#define TX_QUEUE_DELETE_EXTENSION(queue_ptr)
#define TX_SEMAPHORE_DELETE_EXTENSION(semaphore_ptr)
#define TX_TIMER_DELETE_EXTENSION(timer_ptr)


/* Determine if the ARM architecture has the CLZ instruction. This is available on
   architectures v5 and above. If available, redefine the macro for calculating the
   lowest bit set.  */

#ifndef TX_DISABLE_INLINE

#define TX_LOWEST_SET_BIT_CALCULATE(m, b)       b =  (UINT) __builtin_ctz((unsigned int) m);

#endif


/* Define the internal timer extension to also hold the thread pointer such that _tx_thread_timeout
   can figure out what thread timeout to process.  */

#define TX_TIMER_INTERNAL_EXTENSION             VOID    *tx_timer_internal_extension_ptr;


/* Define the thread timeout setup logic in _tx_thread_create.  */

#define TX_THREAD_CREATE_TIMEOUT_SETUP(t)    (t) -> tx_thread_timer.tx_timer_internal_timeout_function =    &(_tx_thread_timeout);            \
                                             (t) -> tx_thread_timer.tx_timer_internal_timeout_param =       0;                                \
                                             (t) -> tx_thread_timer.tx_timer_internal_extension_ptr =       (VOID *) (t);


/* Define the thread timeout pointer setup in _tx_thread_timeout.  */

#define TX_THREAD_TIMEOUT_POINTER_SETUP(t)   (t) =  (TX_THREAD *) _tx_timer_expired_timer_ptr -> tx_timer_internal_extension_ptr;


/************* Define ThreadX SMP data types and function prototypes.  *************/

struct TX_THREAD_STRUCT;


/* Define the ThreadX SMP protection structure.   */

typedef struct TX_THREAD_SMP_PROTECT_STRUCT
{
    UINT32           tx_thread_smp_protect_in_force;
    UINT32           tx_thread_smp_protect_core;
    UINT32           tx_thread_smp_protect_count;
    UINT32           tx_thread_smp_protect_pad_0;
    UINT32           tx_thread_smp_protect_pad_1;
    UINT32           tx_thread_smp_protect_pad_2;
    UINT32           tx_thread_smp_protect_pad_3;
} TX_THREAD_SMP_PROTECT;


/* Define ThreadX interrupt lockout and restore macros for protection on
   access of critical kernel information.  The restore interrupt macro must
   restore the interrupt posture of the running thread prior to the value
   present prior to the disable macro.  In most cases, the save area macro
   is used to define a local function save area for the disable and restore
   macros.  */

#define TX_INTERRUPT_SAVE_AREA                  unsigned int interrupt_save;

#define TX_DISABLE                              interrupt_save =  _tx_thread_smp_protect();
#define TX_RESTORE                              _tx_thread_smp_unprotect(interrupt_save);

/************* End ThreadX SMP data type and function prototype definitions.  *************/


/* Define the interrupt lockout macros for each ThreadX object.  */

#define TX_BLOCK_POOL_DISABLE                   TX_DISABLE
#define TX_BYTE_POOL_DISABLE                    TX_DISABLE
#define TX_EVENT_FLAGS_GROUP_DISABLE            TX_DISABLE
#define TX_MUTEX_DISABLE                        TX_DISABLE
#define TX_QUEUE_DISABLE                        TX_DISABLE
#define TX_SEMAPHORE_DISABLE                    TX_DISABLE


/* Define VFP extension for the Cortex-A5x.  Each is assumed to be called in the context of the executing
   thread.  */
#define tx_thread_fp_enable                     _tx_thread_fp_enable
#define tx_thread_fp_disable                    _tx_thread_fp_disable

VOID    tx_thread_fp_enable(VOID);
VOID    tx_thread_fp_disable(VOID);

#ifdef  CONFIG_SSP_THREADX_NEWLIB
#ifdef  TX_GLOBAL_VFP_ENABLE
#define TX_THREAD_STARTED_EXTENSION(thread_ptr)                     _REENT_INIT_PTR(&(thread_ptr->impure_data)); \
                                                                    tx_thread_fp_enable();
#else
#define TX_THREAD_STARTED_EXTENSION(thread_ptr)                     _REENT_INIT_PTR(&(thread_ptr->impure_data));
#endif
#else   /* CONFIG_SSP_THREADX_NEWLIB */

#ifdef  TX_GLOBAL_VFP_ENABLE
#define TX_THREAD_STARTED_EXTENSION(thread_ptr)                     tx_thread_fp_enable();
#else
#define TX_THREAD_STARTED_EXTENSION(thread_ptr)
#endif
#endif


/* Define the version ID of ThreadX.  This may be utilized by the application.  */

#ifdef TX_THREAD_INIT
CHAR                            _tx_version_id[] =
                                    "Copyright (c) 1996-2019 Express Logic Inc. * ThreadX Cortex-A5x-SMP/ARM Version GVVVV.5.5 SN: ZZZZ *";
#else
extern  CHAR                    _tx_version_id[];
#endif


#endif


