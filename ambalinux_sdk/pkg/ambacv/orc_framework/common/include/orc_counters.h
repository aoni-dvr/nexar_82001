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

#ifndef ORC_COUNTERS_H_
#define ORC_COUNTERS_H_

#include <cv1-def.h>
#include <orc_wait.h>
#include <ucode_base.h> /* for orc_assert */

/*!
  @file orc_counters.h

  @brief Access sync counters in an orc-independent way
*/

/// When reading raw counters, left-shift from where current value is stored
#define COUNTER_CUR_SHIFT 12
/// Right-justified mask for extracting current value from raw counter
#define COUNTER_CUR_MASK  0xfff

/// When reading raw counters, left-shift from where max value is stored
#define COUNTER_MAX_SHIFT 0
/// Right-justified mask for extracting max value from raw counter
#define COUNTER_MAX_MASK  0xfff

//////////////////////////////////////////////////////////////////////////////

#ifndef __ASSEMBLER__

#ifdef ORCVIS

//////////////////////////////////////////////////////////////////////////////

#ifdef MISRA_COMPLIANCE

void set_counter(uint32_t counter, uint32_t cur_count, uint32_t max_count);
uint32_t  get_counter(uint32_t counter);

#else /* !MISRA_COMPLIANCE */

/// Given a raw counter, extract the current value
#define COUNTER_VALUE_2_CURRENT( value_ )       \
  (((value_)>>COUNTER_CUR_SHIFT)&COUNTER_CUR_MASK)

/// Given a raw counter, extract the max value
#define COUNTER_VALUE_2_MAX( value_ )       \
  (((value_)>>COUNTER_MAX_SHIFT)&COUNTER_MAX_MASK)

/// Construct a raw counter value from current value and max value
#define COUNTER_VALUE( _cur_count, _max_count )         \
((((_cur_count)&COUNTER_CUR_MASK)<<(COUNTER_CUR_SHIFT))| \
 (((_max_count)&COUNTER_MAX_MASK)<<(COUNTER_MAX_SHIFT)))

#define set_counter( _ctr, _cur_count, _max_count )                     \
do {                                                                    \
  uint32_t value__ = COUNTER_VALUE( (_cur_count), (_max_count) );       \
  uint32_t addr__  = 0xfffdf000U + ((_ctr)<<2);                          \
  asm volatile("l.sw 0(%0), %1" ::"r"( addr__ ), "r"( value__ ) : "memory" ); \
} while(0)

#define get_counter_value( _ctr )                                       \
({                                                                      \
  uint32_t value__;                                                     \
  uint32_t addr__  = 0xfffdf000U + ((_ctr)<<2);                          \
  asm volatile("l.lwz %0, 0(%1)" : "=r"( value__ ) : "r"( addr__ ));     \
  value__;                                                              \
})

#define get_counter( _ctr )                             \
  COUNTER_VALUE_2_CURRENT( get_counter_value( _ctr ) )

/*! @brief Sets the specified timer to a speecified value.  Coding orcs only.
 
    @param _timer Timer to write.  Must be constant.
    @param _value Value to write to timer.
*/
#define set_timer(_timer, _value)                                       \
asm volatile("lv.settimerstate %0, %1"::"r"(_value), "g"(_timer))

/*! @brief Reads the specfied timer and writes it to a C variable.
    Vision orcs only.

    Name is in all caps because its calling conventions are not
    function-like.
    
    @param _timer Timer to read.  Must be a constant.
    @param _dest_var C variable to write - must be an lvalue and must
    be register allocatable.

    @see get_timer() for a more standard calling syntax
*/
#define GET_TIMER_VAR(_timer, _dest_var)                        \
asm volatile("lv.gettimerstate %0, %1":"=r"(_dest_var):"g"(_timer))

/*! @brief Reads a timer and returns it.  Vision orcs only.

    @param _timer Timer to read.  Must be a constant.
*/
#define get_timer(_timer)                       \
({                                              \
  uint32_t var__;                               \
  GET_TIMER_VAR(_timer, var__);                 \
  var__;                                        \
})
#endif /* ?MISRA_COMPLIANCE */

#ifdef MISRA_COMPLIANCE

/// Returns current time.  Vision orcs only.
uint32_t get_cur_time(void);

/// Writes current time.  Vision orcs only.
void  set_cur_time(uint32_t value);

/// Sets reschedule timer (absolute).  Vision orcs only.
void  set_resch_time(uint32_t value);

/// Sets reschedule timer (delta) .  Vision orcs only.
void  set_resch_time_delta(uint32_t value);

#else /* !MISRA_COMPLIANCE */

/// Returns current time.  Vision orcs only.
#define get_cur_time() get_timer(VIS_TSR_CURRENTTIME)

/// Writes current time.  Vision orcs only.
#define set_cur_time(_value) set_timer(VIS_TSR_CURRENTTIME, _value)

/// Sets reschedule timer (absolute).  Vision orcs only.
#define set_resch_time(_value) set_timer(VIS_TSR_RESCHEDULETIME, _value)

/// Sets reschedule timer (delta) .  Vision orcs only.
#define set_resch_time_delta(_value) set_timer(VIS_TSR_RESCHEDULETIMEDELTA, _value)

#endif /* ?MISRA_COMPLIANCE */


#ifdef MISRA_COMPLIANCE
void  orc_sleep(uint32_t time_in_ticks);
#else /* !MISRA_COMPLIANCE */
#define orc_sleep(_value)                                       \
do {                                                            \
  preempt_hold();                                               \
  set_resch_time_delta(_value);                                 \
  wait_vis_w_timer();                                           \
} while (0)
#endif /* ?MISRA_COMPLIANCE */

#ifdef CHIP_CV6
#define vis_take_mutex(x)         //orc_printf("[DEBUG] : vis_take_mutex(%d) taken in file "__FILE__" at line %d; ignored - not supported in CV6\n", x, __LINE__);
#define vis_release_mutex(x)      //orc_printf("[DEBUG] : vis_release_mutex(%d) taken in file "__FILE__" at line %d; ignored - not supported in CV6\n", x, __LINE__);
#else /* !CHIP_CV6 */
#ifdef AMALGAM_DIAG
#define vis_take_mutex(x)                 \
do {                                      \
  wait_s_sync(x);                         \
  preempt_hold();                         \
} while (0)

#define vis_release_mutex(x)              \
do {                                      \
  preempt_release();                      \
  wait_d_sync(x);                         \
} while (0)

#else /* !AMALGAM_DIAG */

#ifdef MISRA_COMPLIANCE
void vis_take_mutex(uint32_t x);
void vis_release_mutex(uint32_t x);
#else /* !MISRA_COMPLIANCE */
#define vis_take_mutex(x)                 \
do {                                      \
  wait_s_sync(x);                         \
  preempt_hold();                         \
} while (0)

#define vis_release_mutex(x)              \
do {                                      \
  preempt_release();                      \
  wait_d_sync(x);                         \
} while (0)
#endif /* ?MISRA_COMPLIANCE */
#endif /* ?AMALGAM_DIAG */
#endif /* ?CHIP_CV6 */

#ifdef CHIP_CV6

// "A" interrupts are for messages
// "B" interrupts are for jobs

#define SPR_LOCAL_IRQ               0xC010U
#define SPR_LOCAL_IRQ_MASK          0xC011U
#define SPR_REMOTE_IRQA             0xC012U
#define SPR_REMOTE_IRQA_MASK        0xC013U
#define SPR_REMOTE_IRQB             0xC014U
#define SPR_REMOTE_IRQB_MASK        0xC015U

#define VISHW_LOCAL_MGR_INTERRUPT   0x0000U
#define VISHW_LOCAL_USR_INTERRUPT   0x0001U

#define VISHW_SCH0A_INTERRUPT       0x0020U
#define VISHW_SCH0B_INTERRUPT       0x0021U

#define VISHW_NVP0A_INTERRUPT       0x0070U
#define VISHW_NVP0B_INTERRUPT       0x0071U
#define VISHW_NVP1A_INTERRUPT       0x0080U
#define VISHW_NVP1B_INTERRUPT       0x0081U
#define VISHW_NVP2A_INTERRUPT       0x0090U
#define VISHW_NVP2B_INTERRUPT       0x0091U
#define VISHW_NVP3A_INTERRUPT       0x00A0U
#define VISHW_NVP3B_INTERRUPT       0x00A1U
#define VISHW_NVP4A_INTERRUPT       0x00B0U
#define VISHW_NVP4B_INTERRUPT       0x00B1U
#define VISHW_NVP5A_INTERRUPT       0x00C0U
#define VISHW_NVP5B_INTERRUPT       0x00C1U
#define VISHW_GVP0A_INTERRUPT       0x00D0U
#define VISHW_GVP0B_INTERRUPT       0x00D1U
#define VISHW_GVP1A_INTERRUPT       0x00E0U
#define VISHW_GVP1B_INTERRUPT       0x00E1U
#define VISHW_FEX0A_INTERRUPT       0x00F0U
#define VISHW_FEX0B_INTERRUPT       0x00F1U
#define VISHW_FEX1A_INTERRUPT       0x0100U
#define VISHW_FEX1B_INTERRUPT       0x0101U
#define VISHW_FMA0A_INTERRUPT       0x0110U
#define VISHW_FMA0B_INTERRUPT       0x0111U
#define VISHW_FMA1A_INTERRUPT       0x0120U
#define VISHW_FMA1B_INTERRUPT       0x0121U



#ifdef MISRA_COMPLIANCE
void vishw_send_interrupt(uint32_t target);
#else /* !MISRA_COMPLIANCE */
#define vishw_send_interrupt(x) { asm volatile("l.msync"); asm volatile("lv.sendintr %0"::"r"(x)); }
#endif /* ?MISRA_COMPLIANCE */

#endif /* ?CHIP_CV6 */


//////////////////////////////////////////////////////////////////////////////
#endif /* ORCVIS */

#endif

#endif
