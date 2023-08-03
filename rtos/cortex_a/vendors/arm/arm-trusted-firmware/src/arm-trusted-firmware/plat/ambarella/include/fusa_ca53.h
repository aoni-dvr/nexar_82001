/*
 * @file fusa_ca53.h
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

#ifndef FUSA_CA53_H
#define FUSA_CA53_H

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

#define NUM_CORES   4  // Cortex-A53 has 4 cores on CV2FS
#define FIFO_SIZE   4  // FIFO queue size

/**
 * CPU exceptions self-diagnostic results.
 */
struct ex_result_s {
    UINT32 ntest;   /** Holds bit pattern of test performed */
    UINT32 npass;   /** Holds bit pattern of test passed */
#define USR_TEST 0x1U
#define FIQ_TEST 0x2U
#define IRQ_TEST 0x4U
#define SVR_TEST 0x8U
#define MON_TEST 0x10U
#define ABT_TEST 0x20U
#define HYP_TEST 0x40U
#define UND_TEST 0x80U
#define SYS_TEST 0x100U
};

/**
 * CPU exceptions self-diagnostic execution statistics.
 */
struct ex_stat_s {
    UINT32 min_us;  /**< Minimum value of execution time in microseconds */
    UINT32 max_us;  /**< Maximum value of execution time in microseconds */
    UINT32 avg_us;  /**< Average value of execution time in microseconds */
    FLOAT f_avg_us; /**< The average in floating point format */
    UINT32 count;   /**< Count of total diagnostics performed */
    UINT32 npass;    /**< Pass counter from safety CPU */
    UINT32 nfail;    /**< Fail counter from safety CPU */
};

/**
 * Comprehensive ARM instruction set self-diagnostic. This data structure
 * represents a diagnostic transaction for one Cortex-A53 CPU core. It contains
 * a seed input (a randomly generated word), output words as the diagnostic
 * performs a chain of computations, and statistics that are captured during
 * the diagnostic.
 */
struct isa_result_s {
    UINT64 input;        /**< Input (randomized) seed word */
    UINT64 output_gen;   /**< Result from aarch64 General instructions */
    UINT64 output_data;  /**< Result from aarch64 Data instructions */
    UINT64 output_float; /**< Result from aarch64 Floating-point instructions */
    UINT64 output_simds; /**< Result from aarch64 SIMD scalar instructions */
    UINT64 output_simdv; /**< Result from aarch64 SIMD vector instructions */
    UINT64 output_crypt; /**< Result from aarch64 crypto instructions */
};

/**
 * ISA self-diagnostics execution statistics.
 */
struct isa_stat_s {
    UINT32 min_bus_cycles;    /**< Minimum value of bus cycles captured */
    UINT32 max_bus_cycles;    /**< Maximum value of bus cycles captured */
    UINT32 avg_bus_cycles;    /**< Average value of bus cycles captured */
    FLOAT f_avg_bus_cycles;   /**< The average in floating point format */
    UINT32 min_cpu_cycles;    /**< Minimum value of cpu cycles captured */
    UINT32 max_cpu_cycles;    /**< Maximum value of cpu cycles captured */
    UINT32 avg_cpu_cycles;    /**< Average value of cpu cycles captured */
    FLOAT f_avg_cpu_cycles;   /**< The average in floating point format */
    UINT32 min_mem_access;    /**< Minimum value of memory accesses */
    UINT32 max_mem_access;    /**< Maximum value of memory accesses */
    UINT32 avg_mem_access;    /**< Averge value of memory accesses */
    FLOAT f_avg_mem_access;   /**< The average in floating point format */
    UINT32 min_inst_retired;  /**< Minimum value of instructions retired */
    UINT32 max_inst_retired;  /**< Maximum value of instructions retired */
    UINT32 avg_inst_retired;  /**< Average value of instructions retired */
    FLOAT f_avg_inst_retired; /**< The average in floating point format */
    UINT32 min_us;    /**< Minimum value of execution time in microseconds */
    UINT32 max_us;    /**< Maximum value of execution time in microseconds */
    UINT32 avg_us;    /**< Average value of execution time in microseconds */
    FLOAT f_avg_us;   /**< The average in floating point format */
    UINT32 count;  /**< Count of total diagnostics performed */
    UINT32 npass;   /**< Pass counter from safety CPU */
    UINT32 nfail;   /**< Fail counter from safety CPU */
};

/**
 * Cortex-A53 configuration registers self-diagnostic. This data structure
 * contains read-out values of a set of hard-configured registers that are
 * invariant, and should be monitored by the FuSa library for the
 * invariability to ensure correct system run-time operations.
 */
struct creg_regs_s {
    UINT32 l2ctrl_el1;       /**< L2 Control Register */
    UINT32 mpidr_el1;        /**< Multiprocessor Affinity Register */
    UINT64 id_aa64pfr0_el1;  /**< AArch64 processor feature register 0 */
    UINT32 id_pfr0_el1;      /**< AArch32 processor feature register 0 */
    UINT64 id_aa64isar0_el1; /**< AArch64 instruction set attribute register 0 */
    UINT32 id_isar0_el1;  /**< AArch32 instruction set attribute register 0 */
    UINT32 icache_ccsidr_el1;   /**< I-cache size */
    UINT32 dcache_ccsidr_el1;   /**< D-cache size */
    UINT32 l2cache_ccsidr_el1;  /**< L2-cache size */
    UINT32 mvfr0_el1;  /**< Media and Floating-point Feature Register 0 */
    UINT32 mvfr1_el1;  /**< Media and Floating-point Feature Register 1 */
    UINT32 mvfr2_el1;  /**< Media and Floating-point Feature Register 2 */
};

/**
 * Configuration registers self-diagnostic execution statistics.
 */
struct creg_stat_s {
    UINT32 min_us;    /**< Minimum value of execution time in microseconds */
    UINT32 max_us;    /**< Maximum value of execution time in microseconds */
    UINT32 avg_us;    /**< Average value of execution time in microseconds */
    FLOAT f_avg_us;   /**< The average in floating point format */
    UINT32 count;  /**< Count of total diagnostics performed */
    UINT32 npass;  /**< Pass counter from safety CPU */
    UINT32 nfail;  /**< Fail counter from safety CPU */
};

/**
 * Branch-prediction performance self-diagnostic. This data structure
 * stores the diagnostic performance output. These performance numbers are
 * to be periodically checked to make sure that they are within pre-defined
 * thresholds to ensure that the BP hardware is functioning correctly on the
 * Cortex-A53 CPU cores.
 */
struct bp_result_s {
    UINT32 br_pred;           /**< PMU capture of predictive branches
                               *   speculatively executed */
    UINT32 cpu_cycles;        /**< PMU capture of CPU cycles */
    UINT32 br_mis_pred;       /**< PMU capture of mispredicted or not
                               *   predicted branches speculatively executed */
    UINT32 br_immed_retried;  /**< PMU capture of instruction architecturally
                               *   executed - immediate branch */
};

/*
 * Branch prediction self-diagnostic execution statistics.
 */
struct bp_stat_s {
    UINT32 min_br_pred;        /**< Minimum value of branches predicted */
    UINT32 max_br_pred;        /**< Maximum value of branches predicted */
    UINT32 avg_br_pred;        /**< Average value of branches predicted */
    FLOAT f_avg_br_pred;       /**< The average in floating point format */
    UINT32 min_cpu_cycles;     /**< Minimum value of CPU cycles spent */
    UINT32 max_cpu_cycles;     /**< Maximum value of CPU cycles spent */
    UINT32 avg_cpu_cycles;     /**< Average value of CPU cycles spent */
    FLOAT f_avg_cpu_cycles;    /**< The average in floating point format */
    UINT32 min_br_mis_pred;    /**< Minimum value of branch mispredictions */
    UINT32 max_br_mis_pred;    /**< Maximum value of branch mispredictions */
    UINT32 avg_br_mis_pred;    /**< Average value of branch mispredictions */
    FLOAT f_avg_br_mis_pred;   /**< The average in floating point format */
    UINT32 min_br_immed_retried;  /**< Minimum of br_immed_retried */
    UINT32 max_br_immed_retried;  /**< Maximum of br_immed_retried */
    UINT32 avg_br_immed_retried;  /**< Average of br_immed_retried */
    FLOAT f_avg_br_immed_retried;  /**< The average in floating point format */
    UINT32 min_us;    /**< Minimum value of execution time in microseconds */
    UINT32 max_us;    /**< Maximum value of execution time in microseconds */
    UINT32 avg_us;    /**< Average value of execution time in microseconds */
    FLOAT f_avg_us;   /**< The average in floating point format */
    UINT32 count;     /**< Count of total diagnostics performed */
    UINT32 npass;      /**< Pass counter from safety CPU */
    UINT32 nfail;      /**< Fail counter from safety CPU */
};

/**
 * Branch-prediction performance monitor threshold values. The parameters in
 * this data structure are used by the monitor running on the safety
 * CPU (Cortex-R52) to ensure that the BP hardware of Cortex-A53 is operating
 * within valid conditions.
 */
struct bp_param_s {
    UINT32 br_pred_hi;          /**< Upper threshold of PMU br_pred stat */
    UINT32 br_pred_lo;          /**< Lower threshold of PMU br_pref stat */
    UINT32 cpu_cycles_hi;       /**< Upper threshold of PMU cpu_cycles stat */
    UINT32 cpu_cycles_lo;       /**< Lower threshold of PMU cpu_cycles stat */
    UINT32 br_mis_pred_hi;      /**< Upper threshold of PMU br_mis_pred stat */
    UINT32 br_mis_pred_lo;      /**< Lower threshold of PMU br_mis_pred stat */
    UINT32 br_immed_retried_hi; /**< Upper threshold of PMU
                                 *   br_immed_retried stat */
    UINT32 br_immed_retried_lo; /**< Lower threshold of PMU
                                 *   br_immed_retried stat */
};

/**
 * L2 victims performance self-diagnostic. This data structure stores
 * the diagnostic performance output. These performance numbers are to
 * be periodically checked to make sure that they are within pre-defined
 * thresholds to ensure that the L2 hardware is functioning correctly on
 * the Cortex-A53 CPU.
 */
struct l2_result_s {
    UINT32 l2d_cache;  /**< PMU capture of L2 data cache accesses */
    UINT32 mem_access; /**< PMU capture of memory accesses */
    UINT32 l1d_cache;  /**< PMU capture of L1 data cache accesses */
};

/**
 * L2 victims performance self-diagnostics execution statistics.
 */
struct l2_stat_s {
    UINT32 min_l2d_cache;   /**< Minimum value of L2 D-cache accesses */
    UINT32 max_l2d_cache;   /**< Maximum value of L2 D-cache accesses */
    UINT32 avg_l2d_cache;   /**< Average value of L2 D-cache accesses */
    FLOAT f_avg_l2d_cache;  /**< The average in floating point format */
    UINT32 min_mem_access;  /**< Minimum value of memory accesses */
    UINT32 max_mem_access;  /**< Maximum value of memory accesses */
    UINT32 avg_mem_access;  /**< Average value of memory accesses */
    FLOAT f_avg_mem_access; /**< The average in floating point format */
    UINT32 min_l1d_cache;   /**< Minimum value of L1 D-cache accesses */
    UINT32 max_l1d_cache;   /**< Maximum value of L1 D-cache accesses */
    UINT32 avg_l1d_cache;   /**< Average value of memory accesses */
    FLOAT f_avg_l1d_cache;  /**< The average in floating point format */
    UINT32 min_us;  /**< Minimum value of execution time in microseconds */
    UINT32 max_us;  /**< Maximum value of execution time in microseconds */
    UINT32 avg_us;  /**< Average value of execution time in microseconds */
    FLOAT f_avg_us; /**< The average in floating point format */
    UINT32 count;   /**< Count of total diagnostics performed */
    UINT32 npass;   /**< Pass counter from safety CPU */
    UINT32 nfail;   /**< Fail counter from safety CPU */
};

/**
 * L2 victims performance monitor threshold values. The parameters in this
 * data structure are used by the monitor running on the safety CPU
 * (Cortex-R52) to ensure that the L2 hardware of Cortex-A53 is operating
 * within valid conditions.
 */
struct l2_param_s {
    UINT32 l2d_cache_hi;  /**< Upper threshold of PMU l2d_cache */
    UINT32 l2d_cache_lo;  /**< Lower threshold of PMU l2d_cache */
    UINT32 mem_access_hi; /**< Upper threshold of PMU mem_access */
    UINT32 mem_access_lo; /**< Lower threshold of PMU mem_access */
    UINT32 l1d_cache_hi;  /**< Upper threshold of PMU l1d_cache */
    UINT32 l1d_cache_lo;  /**< Lower threshold of PMU l1d_cache */
};

/**
 * ECC fault injection self-diagnostic.
 */
struct eccfi_result_s {
    UINT32 result;  /**< Result pattern */
};

/**
 * ECC fault injection self-diagnostic execution statistics.
 */
struct eccfi_stat_s {
    UINT32 min_us;  /**< Minimum value of execution time in microseconds */
    UINT32 max_us;  /**< Maximum value of execution time in microseconds */
    UINT32 avg_us;  /**< Average value of execution time in microseconds */
    FLOAT f_avg_us; /**< The average in floating point format */
    UINT32 count;   /**< Count of total diagnostics performed */
    UINT32 npass;   /**< Pass counter from safety CPU */
    UINT32 nfail;   /**< Fail counter from safety CPU */
};

/**
 * SRAM latent fault self-diagnostic.
 */
struct srlf_result_s {
    UINT32 result;  /**< Result pattern */
};

/**
 * SRAM latent fault self-diagnostic execution statistics.
 */
struct srlf_stat_s {
    UINT32 min_us;  /**< Minimum value of execution time in microseconds */
    UINT32 max_us;  /**< Maximum value of execution time in microseconds */
    UINT32 avg_us;  /**< Average value of execution time in microseconds */
    FLOAT f_avg_us; /**< The average in floating point format */
    UINT32 count;   /**< Count of total diagnostics performed */
    UINT32 npass;   /**< Pass counter from safety CPU */
    UINT32 nfail;   /**< Fail counter from safety CPU */
};

/**
 * Shared-memory object for communications beteen the Cortex-A53 (application)
 * and Cortex-R52 (safety) CPUs.
 */
struct fusa_mem {
    /**
     * The header (to hold run-time information).
     */
    struct {
        char ca53_vers[128];  /**< libfusa_ca53.a version string */
        char cr52_vers[128];  /**< libfusa_cr52.a version string */
    } header;

    /**
     * Exceptions self-diagnostics (per CPU core).
     */
    struct {
        struct ex_result_s res[FIFO_SIZE];  /**< Result FIFO queue */
        UINT32 prod;  /**< Producer index */
        UINT32 cons;  /**< Consumer index */
        struct ex_stat_s stat;  /**< Collected statistics */
    } ex_q[NUM_CORES];

    /**
     * Comprehensive ARMv8 instructions self-diagnostics (per CPU core).
     */
    struct {
        struct isa_result_s res[FIFO_SIZE];  /**< Result FIFO queue */
        UINT32 prod;  /**< Producer index */
        UINT32 cons;  /**< Consumer index */
        struct isa_stat_s stat;  /**< Collected statistics */
    } isa_q[NUM_CORES];

    /**
     * Configuration registers self-diagnostics (primary core).
     */
    struct {
        struct creg_regs_s res[FIFO_SIZE];  /**< Result FIFO queue */
        UINT32 prod;  /**< Producer index */
        UINT32 cons;  /**< Consumer index */
        struct creg_stat_s stat;  /**< Collected statistics */
    } creg_q;

    /**
     * Branch prediction self-diagnostics (primary core).
     */
    struct {
        struct bp_result_s res[FIFO_SIZE];  /**< Results FIFO queue */
        UINT32 prod;  /**< Producer index */
        UINT32 cons;  /**< Consumer index */
        struct bp_stat_s stat;    /**< Collected statistics */
        struct bp_param_s param;  /**< Monitor threshold params */
    } bp_q;

    /**
     * L2 self-diagnostics (primary core).
     */
    struct {
        struct l2_result_s res[FIFO_SIZE];  /**< Results FIFO queue */
        UINT32 prod;  /**< Producer index */
        UINT32 cons;  /**< Consumer index */
        struct l2_stat_s stat;    /**< Collected statistics */
        struct l2_param_s param;  /**< Monitor threshold params */
    } l2_q;

    /**
     * ECC fault-injection self-diagnostics (primary core).
     */
    struct {
        struct eccfi_result_s res[FIFO_SIZE];  /**< Results FIFO queue */
        UINT32 prod;  /**< Producer index */
        UINT32 cons;  /**< Consumer index */
        struct eccfi_stat_s stat;  /**< Collected statistics */
    } eccfi_q;

    /**
     * SRAM latent fault self-diagnostics (primary core).
     */
    struct {
        struct srlf_result_s res[FIFO_SIZE];  /**< Results FIFO queue */
        UINT32 prod;  /**< Producer index */
        UINT32 cons;  /**< Consumer index */
        struct srlf_stat_s stat;  /**< Collected statistics */
    } srlf_q;

    /**
     * Diagnostics executed directly by Cortex-R52 safety CPU.
     */
    struct {
        UINT32 ecctcm_pass;         /**< Pass counter ECC/TCM */
        UINT32 ecctcm_fail;         /**< Fail counter ECC/TCM */
        UINT32 ecccache_pass;       /**< Pass counter ECC/cache */
        UINT32 ecccache_fail;       /**< Fail counter ECC/cache */
        UINT32 lscompfi_pass;       /**< Pass counter lock-step comps */
        UINT32 lscompfi_fail;       /**< Fail counter lock-step comps */
        UINT32 r52rdatafi_pass;     /**< Pass counter R52_RDATA */
        UINT32 r52rdatafi_fail;     /**< Fail counter R52_RDATA */
        UINT32 a53rdatafi_pass;     /**< Pass counter A53_RDATA */
        UINT32 a53rdatafi_fail;     /**< Fail counter A53_RDATA */
        UINT32 systimerr52fi_pass;  /**< Pass counter SYSTIMER_R52 */
        UINT32 systimerr52fi_fail;  /**< Fail counter SYSTIMER_R52 */
        UINT32 systimera53fi_pass;  /**< Pass counter SYSTIMER_A53 */
        UINT32 systimera53fi_fail;  /**< Fail counter SYSTIMER_A53 */
    } cr52_diag;
};

/**
 * Initialize the libfusa_ca53.a - call this with a pointer to a shared memory
 * area (preferrably 4KB aligned) in an uncached region.
 */
extern void init_fusa_ca53(void *shmem);

/**
 * Exception self-diagnostics. Covers synchronous and asynchronous exceptions
 * of the Cortex-A53 CPU.
 */
extern void ex_self_diag(void);

/**
 * This function is used to perform ARMv8-a instruction sets of the aarch64
 * variants in the groups:
 *  - general
 *  - data
 *  - floating point
 *  - SIMD scalar
 *  - SIMD vector
 *  - cryptographic
 * The caller should invoke this functions periodically according to the
 * safety requirements (e.g., within 99ms interval).
 * The computed results are passed to the safety CPU and verified by the
 * corresponding FuSa library running on it. The safety CPU would assert
 * error status on wrong computational results, violation of performance
 * thresholds, or violation of deadlines to the error handler. However, for
 * the caller on Cortex-A53, this function always returns without any status.
 *
 * This diagnostic function is run on the CPU core of the caller. Since the
 * Cortex-A53 has four cores, it should assign (up-to four) threads to perform
 * periodic self-diagnostic.
 */
extern void isa_self_diag(void);

/**
 * Configuration registers self-diagnostics.
 */
extern void creg_self_diag(void);

/**
 * Branch-predictions self-diagnostics.
 */
extern void bp_self_diag(void);

/**
 * L2 self-diagnostics.
 */
extern void l2_self_diag(void);

/**
 * ECC (SRAM) fault-injection self-diagnostics.
 */
extern void eccfi_self_diag(void);


#define SRLF_SELF_DIAG_ICACHE  0x1
#define SRLF_SELF_DIAG_DCACHE  0x2
#define SRLF_SELF_DIAG_TLB     0x4

/**
 * ECC (SRAM) latent fault self-diagnostics.
 */
extern void srlf_self_diag(void *buf, size_t size, UINT32 mask);

#define FUSACA53_FI_EX    0x1U
#define FUSACA53_FI_ISA   0x2U
#define FUSACA53_FI_CREG  0x4U
#define FUSACA53_FI_BP    0x8U
#define FUSACA53_FI_L2    0x10U
#define FUSACA53_FI_ECCFI 0x20U
#define FUSACA53_FI_SRLF  0x40U

/**
 * Fault injection into libfusa_ca53 for coverage testing.
 *
 * @param mask   The bit fields specify which functions should receive
 *               fault injection to produce a negative result the next
 *               it is called.
 */
extern void libfusaca53_fault_inject(UINT32 mask);

/*
 * ATF SMC SIP handler IDs
 */
#define FUSA_SIP_EX_SELF_DIAG       0x8200ff81U
#define FUSA_SIP_ISA_SELF_DIAG      0x8200ff82U
#define FUSA_SIP_CREG_SELF_DIAG     0x8200ff83U
#define FUSA_SIP_BP_SELF_DIAG       0x8200ff84U
#define FUSA_SIP_L2_SELF_DIAG       0x8200ff85U
#define FUSA_SIP_ECCFI_SELF_DIAG    0x8200ff86U
#define FUSA_SIP_SRLF_SELF_DIAG     0x8200ff87U
#define FUSA_SIP_FUSA_FAULT_INJECT  0x8200ff88U

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
