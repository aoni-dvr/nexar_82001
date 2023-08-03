/*
 * Copyright (c) 2017-2018 Ambarella, Inc.
 *
 * This file and its contents ("Software") are protected by intellectual
 * property rights including, without limitation, U.S. and/or foreign copyrights.
 * This Software is also the confidential and proprietary information of
 * Ambarella, Inc. and its licensors.  You may not use, reproduce, disclose,
 * distribute, modify, or otherwise prepare derivative works of this Software
 * or any portion thereof except pursuant to a signed license agreement or
 * nondisclosure agreement with Ambarella, Inc. or its authorized affiliates.
 * In the absence of such an agreement, you agree to promptly notify and
 * return this Software to Ambarella, Inc.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 * MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef UCODE_BASE_H_
#define UCODE_BASE_H_

#include <orc_wait.h>
#include <orc_counters.h>
// FROM DIAG
#include "static_counters.h"

/*!
  @file ucode_base.h
  
  @brief This file contains definitions for some basic functions
  needed by all diags.

  Most of these functions are callable from C or assembly.  See the
  doucmentation for each.

  This file requires that the diag defines a header file called
  static_counters.h (in the include path) that contains a definition
  for CTR_park.  That counter is used to implement the PARK function.

*/

#ifndef __ASSEMBLER__

/*!
 @brief Diag should exit with passing status

 Callable from C or assembly.
 
 On amalgam, any thread calling pass will make the diag quit with
 passing exit status.  On RTL, all threads in the finish mask must
 call pass.  The safest thing is for one thread to call PASS afer it
 knows that all other threads are done.  The finish mask should
 contain only one thread.
*/
extern void pass(void);
/*!
 @brief Diag should abort with failing status

 Callable from C or assembly.
*/
extern void fail(void);

/*!
  @brief Regular printf with up to 5 arguments.  No %%s.

  Function can be called from C.  From assembly, you can call the
  function, but you have to pass arguments like any other function
  (i.e., you can't just put "orc_printf()" in your asm file).
  
  On amalgam, this is a backdoor function: no cost, and not clobbering
  registers.  On hardware, assume it's a real function call with
  actual overhead.
*/
#ifdef MISRA_COMPLIANCE
extern void orc_printf(void);
#else /* !MISRA_COMPLIANCE */
extern void orc_printf();
#endif /* ?MISRA_COMPLIANCE */

/*!
  @brief Enable stack checking on the current thread.

  The first argument is the minimum value for the stack.  Each time
  the stack pointer (R1) is updated, amalgam asserts that R1 > min.
  The second argument is the max value for the stack.  Each time the
  stack pointer is updated, amalgam asserts that R1 <= max.

  Not every register update is checked, only addi instructions, since
  that's how the compiler always manipulates the stack pointer.
  
  On amalgam, this is a backdoor function: no cost, and not clobbering
  registers.  On hardware, it's a function that does ntohing.
*/
extern void bdoor_check_stack(void);

/*!
  @brief Disables stack checking for the current thread.
*/
extern void bdoor_disable_check_stack(void);

#ifndef MISRA_COMPLIANCE /* Cannot use these defines in a MISRA-compliant compile */

/*!
 @brief If the condition is false, abort with failing status

 Callable only from C.
*/
#define orc_assert(_cond)                                               \
do {                                                                    \
  if(!(_cond)) {                                                        \
    orc_printf("Assertion `"#_cond"' at "__FILE__":%d failed\n",        \
               __LINE__);                                               \
    fail();                                                             \
  }                                                                     \
} while(0)

/*!
 @brief Same as orc_assert, but with printf args.

 Callable only from C.

 Same restrictions as orc_printf.
*/
#define assert_msg(_cond, _msg... )                                     \
do {                                                                    \
  if(!(_cond)) {                                                        \
    orc_printf("Assertion failure at "__FILE__":%d: \n", __LINE__);     \
    orc_printf(_msg);                                                   \
    fail();                                                             \
  }                                                                     \
}  while(0)
#endif /* ?MISRA_COMPLIANCE */

#endif

#ifdef __ASSEMBLER__
#  define pass() l.nop NOP_PASS;
#  define fail() l.nop NOP_FAIL;

#ifdef ORCVIS
#  define construct_stack_th0() l.jal _init_stack_th0; l.jal _bdoor_check_stack( _stack_th0_bottom, _stack_th0_top )
#  define construct_stack_th1() l.jal _init_stack_th1; l.jal _bdoor_check_stack( _stack_th1_bottom, _stack_th1_top )
#  define construct_stack_th2() l.jal _init_stack_th0; l.jal _bdoor_check_stack( _stack_th2_bottom, _stack_th2_top )
#  define construct_stack_th3() l.jal _init_stack_th3; l.jal _bdoor_check_stack( _stack_th3_bottom, _stack_th3_top )
   
#endif

#else

#ifndef MISRA_COMPLIANCE /* Cannot use these defines in a MISRA-compliant compile */
/*! @brief Thread should sleep and never wake up. */
#  define park() do {orc_sleep(0x2bf20000);} while (1);
#endif
#endif /* !MISRA_COMPLIANCE */

#endif
