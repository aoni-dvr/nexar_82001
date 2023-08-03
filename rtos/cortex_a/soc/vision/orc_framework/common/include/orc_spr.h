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

#ifndef ORC_SPR_H_
#define ORC_SPR_H_

#include <cv1-def.h>
#include <dsp_types.h>

/*!
 @file orc_spr.h

 @brief Wrappers for reading and writing sprs on the various orcs

 This file also contains non-orc-specific names for orc-specific SPR
 addresses.  For instance, SPR_TH_ID is defined to be the appropriate
 address for the calling orc.

 These defines aren not documented because they are different for
 every orc, and doxygen doesn't handle that well.  See the source.
*/


/* C and assembly definitions.  These remove the Orc-specific part of
   the name so assembly and C functions that are Orc-independent can
   use the same name for SPRs that exist on all Orcs */

#ifdef ORCVIS

#define SPR_TH_ID                    SPR_VIS_TH_ID
#define SPR_WAIT_TIMEOUT_CYCLES      SPR_VIS_WAIT_TIMEOUT_CYCLES
#define SPR_WAIT_TIMEOUT_REASONS     SPR_VIS_WAIT_TIMEOUT_REASONS
#define SPR_CACHE_CONFIG             SPR_VIS_CACHE_CONFIG   
#define SPR_CACHE_INVALIDATE         SPR_VIS_CACHE_INVALIDATE
#define SPR_NONCACHEABLE_DRAM_BASE   SPR_VIS_NONCACHEABLE_DRAM_BASE
#define SPR_PRIORITY_BASE            SPR_VIS_PRIORITY_BASE
#define SPR_PEEK_BASE                SPR_VIS_PEEK_BASE 

#endif


/* C-only definitions */
#ifndef __ASSEMBLER__

#ifdef MISRA_COMPLIANCE
uint32_t read_spr(uint32_t spr);
uint32_t read_spr_imm(uint32_t spr);

#else /* non-MISRA_COMPLIANCE mode */

/*!
  @brief Reads a constant SPR into a C variable

  Using the imm flavor when the SPR address is known at compile time
  saves a register allocation.

  Name is in caps because it doesn't behave like a function.
  
  @param _spr SPR address to read.  Must be a constant.
  @param _dest_var C variable (L-value) to store result.  Should be
  register allocatable.
*/
#define READ_SPR_VAR_IMM(_spr, _dest_var)                       \
asm volatile("l.mfspr %0, r0, %1":"=r"(_dest_var):"g"(_spr))

/*!
  @brief Reads an SPR into a C variable

  Name is in caps because it doesn't behave like a function.

  @param _spr SPR address to read.  Need not be a constant.
  @param _dest_var C variable (L-value) to store result.  Should be
  register allocatable.
*/
#define READ_SPR_VAR(_spr, _dest_var)                         \
asm volatile("l.mfspr %0, %1, 0":"=r"(_dest_var):"r"(_spr))

/*!
  @brief Reads and returns the value of a constant SPR

  Using the imm flavor when the SPR address is known at compile time
  saves a register allocation.
  
  @param _spr SPR address to read.  Must be a constant.
*/
#define read_spr_imm(_spr)                      \
({                                              \
  uint32_t var__;                               \
  READ_SPR_VAR_IMM(_spr, var__);                \
  var__;                                        \
})

/*!
  @brief Reads and returns the value of an SPR

  @param _spr SPR address to read.  Need not be a constant.
*/
#define read_spr(_spr)                          \
({                                              \
  uint32_t var__;                               \
  READ_SPR_VAR(_spr, var__);                    \
  var__;                                        \
})

#endif /* !MISRA_COMPLIANCE */

/*!
  @brief Writes to a constant SPR

  Using the imm flavor when the SPR address is known at compile time
  saves a register allocation.
  
  @param _spr SPR address to write.  Must be a constant
  @param _val Value (32-bit) to write.
*/
#ifdef MISRA_COMPLIANCE
void write_spr_imm(uint32_t spr, uint32_t val);
#else /* !MISRA_COMPLIANCE */
#define write_spr_imm(_spr, _val)                       \
asm volatile("l.mtspr r0, %0, %1"::"r"(_val),"g"(_spr))
#endif /* ?MISRA_COMPLIANCE */

/*!
  @brief Writes to an SPR

  @param _spr SPR address to write.  Need not be a constant.
  @param _val Value (32-bit) to write.
*/
#ifdef MISRA_COMPLIANCE
void write_spr(uint32_t spr, uint32_t val);
#else /* !MISRA_COMPLIANCE */
#define write_spr(_spr, _val)                           \
asm volatile("l.mtspr %0, %1, 0"::"r"(_spr), "r"(_val))
#endif /* ?MISRA_COMPLIANCE */


/*!
  @brief Returns the TID of the current thread

*/
#ifdef MISRA_COMPLIANCE
uint32_t get_self_thread(void);
#else /* !MISRA_COMPLIANCE */
#define get_self_thread() read_spr_imm( SPR_TH_ID )
#endif /* ?MISRA_COMPLIANCE */

#endif

#endif
