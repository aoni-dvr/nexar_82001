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

#ifndef CV1_DEF_H
#define CV1_DEF_H

/*!
 @file cv1-def.h

 @brief Low-level constants and macros for CV1.  Usable from C or
 assembly.
 */

/// Argument to l.nop that will cause amalgam to backdoor PASS
#define NOP_PASS	0xbeef
/// Argument to l.nop that will cause amalgam to backdoor FAIL
#define NOP_FAIL	0xdead

//////////////////////////////////////////////////////////////////////////////

#if defined(__ASSEMBLER__)||defined(DOXYGEN)

/// Assembly-only macro to set a 32-bit immediate into a register
#define SET_REG(reg, val)   l.movhi reg,hi(val);        \
        		    l.ori   reg,reg,lo(val)

#endif

//////////////////////////////////////////////////////////////////////////////

///@{ @name OrcVis Wait Mask
/// @brief OrcVis wait mask
#define VIS_W_COPROC     (1<<0)
#define VIS_W_DRAM_DMA   (1<<1)
#define VIS_W_TIMER      (1<<2)
#define VIS_W_COPROC_DMA (1<<3)
#define VIS_W_INTERRUPT  (1<<4)     // CV6 only
/// @}

//////////////////////////////////////////////////////////////////////////////

///@{ @name Vis Timers
/// @brief OrcVise Timers
#define VIS_TSR_CURRENTTIME       0
#define VIS_TSR_RESCHEDULETIME    1
#define VIS_TSR_RESCHEDULETIMEDELTA   2
/// @}

/// @{
/// @name OrcVis SPR
/// @brief OrcVis Special Purpose Registers
#define SPR_VIS_TH_ID                  0xc001
#define SPR_VIS_WAIT_TIMEOUT_CYCLES    0xc002
#define SPR_VIS_WAIT_TIMEOUT_REASONS   0xc003
#define SPR_VIS_CACHE_CONFIG           0xc008
#define SPR_VIS_CACHE_INVALIDATE       0xc009
#define SPR_VIS_NONCACHEABLE_DRAM_BASE 0xc00a
#define SPR_VMEM_BURST_SIZE            0xc00b   /* For CV2+ */
#define SPR_RESET_AUDIO_TIMER          0xc00c   /* For CV6+ */
#define SPR_VIS_PRIORITY_BASE          0xc030
#define SPR_VIS_PEEK_BASE              0xc060 
/// @}

#endif
