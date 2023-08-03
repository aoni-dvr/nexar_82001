/*
 * @file fusa_cr52.h
 *
 * @copyright Copyright (C) 2020 Ambarella International LP
 *
 * This file and its contents ("Software") are protected by intellectual
 * property rights including, without limitation, U.S. and/or foreign
 * copyrights.  This Software is also the confidential and proprietary
 * information of Ambarella, Inc. and its licensors.  You may not use,
 * reproduce, disclose, distribute, modify, or otherwise prepare derivative
 * works of this Software or any portion thereof except pursuant to a signed
 * license agreement or nondisclosure agreement with Ambarella, Inc. or
 * its authorized affiliates.  In the absence of such an agreement, you
 * agree to promptly notify and return this Software to Ambarella, Inc.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 * MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN
 * NO EVENT SHALL AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef FUSA_CR52_H
#define FUSA_CR52_H

#if defined(CONFIG_THREADX) && !defined(IMAGE_BL31)
/* ThreadX */
#define FUSA_TYPES_DEFINED
#elif defined(__KERNEL__)
/* Linux Kernel */
#include <linux/kernel.h>
#else
/* Bare-metal */
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#if !defined(FUSA_TYPES_DEFINED)
typedef uint8_t UINT8;
typedef uint32_t UINT32;
typedef uint64_t UINT64;
typedef float FLOAT;
#define FUSA_TYPES_DEFINED
#endif
#endif

/**
 * Initialize the libfusa_cr52.a - call this with a pointer to a shared memory
 * area (preferrably 4KB aligned) in an uncached region.
 */
extern void init_fusa_cr52(void *shmem);

/**
 * ECC on TCM SRAM all cells walk-through write-readback to detect if any
 * triggers ECC event.
 */
extern UINT8 ecc_tcm_walk_self_diag(void);

/**
 * ECC on cache SRAM all bits refill to detect if any triggers ECC event.
 */
extern UINT8 ecc_cache_refill_self_diag(void);

/**
 * Lock-step comparators fault-injection self-diagnostics.
 */
extern UINT8 lscomp_fault_inject_self_diag(void);

/**
 * R52_RDATA wire fault-injection.
 */
extern UINT8 r52_rdata_fault_inject_self_diag(void);

/**
 * A53_RDATA wire fault-injection.
 */
extern UINT8 a53_rdata_fault_inject_self_diag(void);

/**
 * SYSTIMER_R52 wire fault-injection.
 */
extern UINT8 systimer_r52_ls_fi_self_diag(void);

/**
 * SYSTIMER_A53 wire fault-injection.
 */
extern UINT8 systimer_a53_ls_fi_self_diag(void);

/**
 * Verify result received from Cortex-A53 for its CPU exceptions self-
 * diagnostics execution and that they were taken correctly as expected.
 */
extern UINT8 verify_ca53_ex_result(void);

/**
 * Verify result received from Cortex-A53 for its comprehensive instructions
 * self-diagnostics execution and that they were computed correctly.
 */
extern UINT8 verify_ca53_isa_result(void);

/**
 * Verify result received from Cortex-A53 for its configuration registers
 * readouts and that the values match.
 */
extern UINT8 verify_ca53_creg_result(void);

/**
 * Verify result received from Cortex-A53 for its branch predictions
 * performance and that they are within specified thresholds, indicating that
 * the BP hardware is functional.
 */
extern UINT8 verify_ca53_bp_result(void);

/**
 * Verify result reveived from Cortex-A53 for its L2 self-diagnostic performance
 * and that the L1 data cache, memory accesses, and L2 data cache event counters
 * are within specified thesholds, indicating that the L2 hardware is
 * functional.
 */
extern UINT8 verify_ca53_l2_result(void);

/**
 * Verify result received from Cortex-A53 for its ECC fault-injection diagnostic
 * to indicate that its ECC is functional.
 */
extern UINT8 verify_ca53_eccfi_result(void);

#define FUSACR52_FI_ETW    0x1U
#define FUSACR52_FI_ECR    0x2U
#define FUSACR52_FI_LFI    0x4U
#define FUSACR52_FI_RRFI   0x8U
#define FUSACR52_FI_ARFI   0x10U
#define FUSACR52_FI_SRLF   0x20U
#define FUSACR52_FI_SALF   0x40U
#define FUSACR52_FI_VEX    0x80U
#define FUSACR52_FI_VISA   0x100U
#define FUSACR52_FI_VCREG  0x200U
#define FUSACR52_FI_VBP    0x400U
#define FUSACR52_FI_VL2    0x800U
#define FUSACR52_FI_VECCFI 0x1000U

/**
 * Fault injection into libfusa_cr52 for coverage testing.
 *
 * @param mask   The bit fields specify which functions should receive
 *               fault injection to produce a negative result the next
 *               it is called.
 */
extern void libfusacr52_fault_inject(UINT32 mask);

#endif

/*
 * Local variables:
 * mode: C
 * c-file-style: "BSD"
 * c-basic-offset: 4
 * tab-width: 4
 * indent-tabs-mode: nil
 * End:
 */
