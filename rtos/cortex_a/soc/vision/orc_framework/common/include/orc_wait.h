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

#ifndef ORC_WAIT_H_
#define ORC_WAIT_H_

#include <dsp_types.h>
#include <orc_spr.h>

/*! @file orc_wait.h
 
 @brief Macros for calling wait in an orc-independent way

 Note that for any wait macro that includes both "_r" and "_d", the
 order is signifcant.  The receive and decrements are done in the
 order in which "_r" and "_d" appear in the macro name.  It is more
 efficient if the decrement appears first, because if the decrement
 is second, the macro expands into two wait statements: the wait for
 everything except the decrement, followed by a wait.d with no
 mask.

 This file may be included by assembly, in which case the useful
 definitions are asm_wait, asm_wait_sfx, and asm_wait_no_mask.
*/

#ifdef ORCVIS

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

#ifndef MISRA_COMPLIANCE  /* Cannot define this for MISRA compliant code, even though this is for ASSEMBLER only */
/// Assembly instruction for wait on this orc.  Available on all orcs.
#define asm_wait lv.wait
/*! @brief Assembly instruction for wait on this orc with suffix
  appended.  Available on all orcs. */
#define asm_wait_sfx(_suffix) lv.wait##_suffix
/*! @brief Correct wait argument for saying "no wait mask" on this
  orc.  Available on all orcs. */
#define asm_no_wait_mask 0
#endif /* !MISRA_COMPLIANCE */

#ifdef MISRA_COMPLIANCE
#ifndef __ASSEMBLER__
void preempt_hold(void);
void preempt_release(void);
#endif /* __ASSEMBLER__ */
#else /* !MISRA_COMPLIANCE */
/// Start a no-preempt section.  Available on coding orcs.
#define preempt_hold()		asm volatile ("lv.nopreempt.start")
/// End a no-preempt section.  Available on coding orcs.
#define preempt_release()	asm volatile ("lv.nopreempt.end")
#endif /* ?MISRA_COMPLIANCE */

/*! @brief Wait on a specified condition.  Available on all
    orcs.

    Condition must be constant on coding orcs, but may be a variable
    on the low-level orcs.
*/

#ifdef MISRA_COMPLIANCE
#ifndef __ASSEMBLER__
void wait(uint32_t cond);
void wait_cs(uint32_t cond);
void wait_vis_w_coproc(void);
void wait_vis_w_coproc_cs(void);
void wait_vis_w_dram_dma(void);
void wait_vis_w_dram_dma_cs(void);
void wait_vis_w_timer(void);
void wait_vis_w_timer_cs(void);
void wait_vis_w_coproc_dma(void);
void wait_vis_w_coproc_dma_cs(void);
#ifdef CHIP_CV6
void wait_vis_w_interrupt(void);
void wait_vis_w_interrupt_cs(void);
#endif /* ?CHIP_CV6 */
#endif /* __ASSEMBLER__ */
#else /* !MISRA_COMPLIANCE */
#define wait(_cond)                 asm volatile("lv.wait %0"::"g"(_cond))
#define wait_cs(_cond)              asm volatile("lv.wait.hold %0"::"g"(_cond))
#define wait_vis_w_coproc()         wait(VIS_W_COPROC)
#define wait_vis_w_coproc_cs()      wait_cs(VIS_W_COPROC)
#define wait_vis_w_dram_dma()       wait(VIS_W_DRAM_DMA)
#define wait_vis_w_dram_dma_cs()    wait_cs(VIS_W_DRAM_DMA)
#define wait_vis_w_timer()          wait(VIS_W_TIMER)
#define wait_vis_w_timer_cs()       wait_cs(VIS_W_TIMER)
#define wait_vis_w_coproc_dma()     wait(VIS_W_COPROC_DMA)
#define wait_vis_w_coproc_dma_cs()  wait_cs(VIS_W_COPROC_DMA)
#ifdef CHIP_CV6
#define wait_vis_w_interrupt()      wait(VIS_W_INTERRUPT)
#define wait_vis_w_interrupt_cs()   wait_cs(VIS_W_INTERRUPT)
#endif /* ?CHIP_CV6 */
#endif /* ?MISRA_COMPLIANCE */

#ifndef MISRA_COMPLIANCE /* This cannot be used in a MISRA compliant compile */
/*! @brief Wait on a specified condition and send.  Available on all
    orcs.

    Send counter must be a constant.
    
    Condition must be constant on coding orcs, but may be a variable
    on the low-level orcs.
*/
#define wait_s(_sctr,_cond)                          \
  asm volatile("lv.wait.gpr.s %0, %1"::"r"(_sctr), "g"(_cond))

/*! @brief Wait on a specified condition and receive.  Available on all
    orcs.

    Receive counter must be a constant.
    
    Condition must be constant on coding orcs, but may be a variable
    on the low-level orcs.
*/
#define wait_r(_rctr,_cond)                      \
  asm volatile("lv.wait.gpr.r %0, %1"::"r"(_rctr), "g"(_cond))

/*! @brief Wait on a specified condition and decrement.  Available on all
    orcs.

    Receive/decrmenet counter must be a constant.
    
    Condition must be constant on coding orcs, but may be a variable
    on the low-level orcs.
*/
#define wait_d(_rctr,_cond)                      \
  asm volatile("lv.wait.gpr.d %0, %1"::"r"(_rctr), "g"(_cond))

/*! @brief Wait on a specified condition, decrement and receive (in
    that order).  Available on all orcs.

    Dec/Receive counter must be a constant.
    
    Condition must be constant on coding orcs, but may be a variable
    on the low-level orcs.
*/
#define wait_d_r(_rctr,_cond)                    \
  asm volatile("lv.wait.rd %0, %1"::"g"(_rctr), "g"(_cond))

/*! @brief Wait on a specified condition, send and receive (in
    parallel).  Available on all orcs.

    Send and receive counters must be constant.
    
    Condition must be constant on coding orcs, but may be a variable
    on the low-level orcs.
*/
#define wait_s_r(_sctr, _rctr,_cond)                             \
  asm volatile("lv.wait.sr %0, %1, %2"::"g"(_sctr), "g"(_rctr), "g"(_cond))

/*! @brief Wait on a specified condition, send and decrement (in
    parallel).  Available on all orcs.

    Send and decrement counters must be constant.
    
    Condition must be constant on coding orcs, but may be a variable
    on the low-level orcs.
*/
#define wait_s_d(_sctr, _rctr,_cond)                             \
  asm volatile("lv.wait.sd %0, %1, %2"::"g"(_sctr), "g"(_rctr), "g"(_cond))

/*! @brief Wait on a specified condition, send and decrement, then
    receive (in that order).  Available on all orcs.

    Counters must be constant.
    
    Condition must be constant on coding orcs, but may be a variable
    on the low-level orcs.
*/
#define wait_s_d_r(_sctr, _rctr,_cond)             \
  asm volatile("lv.wait.srd %0, %1, %2"::"g"(_sctr), "g"(_rctr), "g"(_cond))

/*! @brief Wait on a specified condition, receive, and then decrement
    (in that order).  Available on all orcs.

    Generates two instructions to get the specified order.

    Counters must be constant.
    
    Condition must be constant on coding orcs, but may be a variable
    on the low-level orcs.
*/
#define wait_r_d(_rctr,_cond)                   \
do {                                            \
  wait_r(_rctr,_cond);                          \
  wait_d_sync(_rctr);                           \
} while(0)
  
/*! @brief Wait on a specified condition, send and receive, and then
    decrement (in that order).  Available on all orcs.

    Generates two instructions to get the specified order.

    Counters must be constant.
    
    Condition must be constant on coding orcs, but may be a variable
    on the low-level orcs.
*/
#define wait_s_r_d(_sctr, _rctr,_cond)          \
do {                                            \
  wait_sr(_sctr, _rctr, _cond);                 \
  wait_d_sync(_rctr);                           \
} while(0)

#endif /* !MISRA_COMPLIANCE */
//////////////////////////////////////////////////////////////////////////////

#ifdef CHIP_CV6

#ifdef AMALGAM_DIAG
#define wait_s_sync(x)        orc_printf("[ERROR] wait_s_sync(%d) called from "__FILE__" at line %d - not supported in CV6\n", x, __LINE__)
#define wait_r_sync(x)        orc_printf("[ERROR] wait_r_sync(%d) called from "__FILE__" at line %d - not supported in CV6\n", x, __LINE__)
#define wait_d_sync(x)        orc_printf("[ERROR] wait_d_sync(%d) called from "__FILE__" at line %d - not supported in CV6\n", x, __LINE__)
#define wait_r_d_sync(x)      orc_printf("[ERROR] wait_r_d_sync(%d) called from "__FILE__" at line %d - not supported in CV6\n", x, __LINE__)
#else /* !AMALGAM_DIAG */
#define wait_s_sync(x)
#define wait_r_sync(x)
#define wait_d_sync(x)
#define wait_r_d_sync(x)
#endif /* ?AMALGAM_DIAG */

#else /* !CHIP_CV6 */

#ifdef MISRA_COMPLIANCE
#ifndef __ASSEMBLER__
void wait_s_sync(uint32_t ctr);
void wait_d_sync(uint32_t ctr);
void wait_r_sync(uint32_t ctr);
void wait_r_d_sync(uint32_t ctr);
#endif /* ?__ASSEMBLER__ */
#else /* !MISRA_COMPLIANCE */
/// Like wait_s() but with no wait mask
#define wait_s_sync(_sctr) wait_s((_sctr), 0)
/// Like wait_r() but with no wait mask
#define wait_r_sync(_rctr) wait_r((_rctr), 0)
/// Like wait_d() but with no wait mask
#define wait_d_sync(_rctr) wait_d((_rctr), 0)
/// Like wait_d_r() but with no wait mask
#define wait_d_r_sync(_rctr) wait_d_r((_rctr), 0)
/// Like wait_s_r() but with no wait mask
#define wait_s_r_sync(_sctr, _rctr) wait_s_r((_sctr), (_rctr), 0)
/// Like wait_s_d() but with no wait mask
#define wait_s_d_sync(_sctr, _rctr) wait_s_d((_sctr), (_rctr), 0)
/// Like wait_s_d_r() but with no wait mask
#define wait_s_d_r_sync(_sctr, _rctr) wait_s_d_r((_sctr), (_rctr), 0)
/// Like wait_r_d() but with no wait mask
#define wait_r_d_sync(_rctr) wait_r_d((_rctr), 0)
/// Like wait_s_r_d() but with no wait mask
#define wait_s_r_d_sync(_sctr, _rctr) wait_s_r_d((_sctr), (_rctr), 0)
#endif /* ?MISRA_COMPLIANCE */
#endif /* ?CHIP_CV6*/

//////////////////////////////////////////////////////////////////////////////

#endif

//////////////////////////////////////////////////////////////////////////////

// locks for mutex and the like

#ifndef MISRA_COMPLIANCE

/*! @brief Lock the mutex on the specified counter.  Counter must be
  constant.  Avaialable on all orcs. */
#define lock( _ctr )   wais_s_sync( _ctr )
/*! @brief Unlock the mutex on the specified counter.  Counter must be
  constant.  Avaialable on all orcs. */
#define unlock( _ctr ) wait_d_sync( _ctr )

#ifdef wait_s_sync_gpr

/*! @brief Lock the mutex on the specified counter.  Counter need not
  be constant.  Available on coding orcs. */
#define lock_gpr( _ctr ) wait_s_sync_gpr( _ctr )
/*! @brief Unlock the mutex on the specified counter.  Counter need not
  be constant.  Available on coding orcs. */
#define unlock_gpr( _ctr ) wait_d_sync_gpr( _ctr )

#endif /*?MISRA_COMPLIANCE */

#endif

//////////////////////////////////////////////////////////////////////////////

#endif
