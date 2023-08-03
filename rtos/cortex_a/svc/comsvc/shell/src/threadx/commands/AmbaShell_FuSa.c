/**
 *  @file AmbaShell_FuSa.c
 *
 * Copyright (c) [2020] Ambarella International LP
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

#include <AmbaTypes.h>
#include <AmbaKAL.h>
#include <AmbaMisraFix.h>
#include <AmbaShell.h>
#include <AmbaShell_Utility.h>
#include <AmbaShell_Commands.h>
#include <AmbaSafety.h>
#include <AmbaFusa.h>
#include <fusa_ca53.h>

static inline void print_uint32(UINT32 v, AMBA_SHELL_PRINT_f print)
{
    char buf[UTIL_MAX_INT_STR_LEN];
    (void)AmbaUtility_StringPrintUInt32(buf, sizeof(buf), "%8x", 1U, &v);
    print(buf);
}

static inline void print_int32(UINT32 v, AMBA_SHELL_PRINT_f print)
{
    char buf[UTIL_MAX_INT_STR_LEN];
    (void)AmbaUtility_StringPrintUInt32(buf, sizeof(buf), "%d", 1U, &v);
    print(buf);
}

static void Fusa_PrintVers(const struct fusa_mem *fm,
                           AMBA_SHELL_PRINT_f print)
{
    UINT32 baseaddr;

    AmbaMisra_TypeCast32(&baseaddr, &fm);

    print("fusa_mem=0x");
    print_uint32(baseaddr, print);
    print(",0x");
    print_uint32(sizeof(struct fusa_mem), print);
    print("\n");
    print("ca53_vers=");
    print(fm->header.ca53_vers);
    print("\n");
    print("cr52_vers=");
    print(fm->header.cr52_vers);
    print("\n");
}

static void Fusa_PrintEx(const struct fusa_mem *fm,
                         AMBA_SHELL_PRINT_f print)
{
    UINT32 num_cores = NUM_CORES;
    UINT32 i;

    print("------------------------- ex -------------------------\n");
    for (i = 0U; i < num_cores; i++) {
        if (fm->ex_q[i].stat.count == 0U) {
            continue;
        }
        print("** core");
        print_int32(i, print);
        print(" **");
        print("\ntime(us) (min/avg/max): ");
        print_int32(fm->ex_q[i].stat.min_us, print);
        print("/");
        print_int32(fm->ex_q[i].stat.avg_us, print);
        print("/");
        print_int32(fm->ex_q[i].stat.max_us, print);
        print("\ncount/pass/fail:        ");
        print_int32(fm->ex_q[i].stat.count, print);
        print("/");
        print_int32(fm->ex_q[i].stat.npass, print);
        print("/");
        print_int32(fm->ex_q[i].stat.nfail, print);
        print("\n");
    }
}

static void Fusa_PrintIsa(const struct fusa_mem *fm,
                          AMBA_SHELL_PRINT_f print)
{
    UINT32 num_cores = NUM_CORES;
    UINT32 i;

    print("------------------------- isa -------------------------\n");
    for (i = 0U; i < num_cores; i++) {
        if (fm->isa_q[i].stat.count == 0U) {
            continue;
        }
        print("** core");
        print_int32(i, print);
        print(" **");
        print("\nbus_cycles (min/avg/max):   ");
        print_int32(fm->isa_q[i].stat.min_bus_cycles, print);
        print("/");
        print_int32(fm->isa_q[i].stat.avg_bus_cycles, print);
        print("/");
        print_int32(fm->isa_q[i].stat.max_bus_cycles, print);
        print("\ncpu_cycles (min/avg/max):   ");
        print_int32(fm->isa_q[i].stat.min_cpu_cycles, print);
        print("/");
        print_int32(fm->isa_q[i].stat.avg_cpu_cycles, print);
        print("/");
        print_int32(fm->isa_q[i].stat.max_cpu_cycles, print);
        print("\nmem_access (min/avg/max):   ");
        print_int32(fm->isa_q[i].stat.min_mem_access, print);
        print("/");
        print_int32(fm->isa_q[i].stat.avg_mem_access, print);
        print("/");
        print_int32(fm->isa_q[i].stat.max_mem_access, print);
        print("\ninst_retired (min/avg/max): ");
        print_int32(fm->isa_q[i].stat.min_inst_retired, print);
        print("/");
        print_int32(fm->isa_q[i].stat.avg_inst_retired, print);
        print("/");
        print_int32(fm->isa_q[i].stat.max_inst_retired, print);
        print("\ntime(us) (min/avg/max):     ");
        print_int32(fm->isa_q[i].stat.min_us, print);
        print("/");
        print_int32(fm->isa_q[i].stat.avg_us, print);
        print("/");
        print_int32(fm->isa_q[i].stat.max_us, print);
        print("\ncount/pass/fail:            ");
        print_int32(fm->isa_q[i].stat.count, print);
        print("/");
        print_int32(fm->isa_q[i].stat.npass, print);
        print("/");
        print_int32(fm->isa_q[i].stat.nfail, print);
        print("\n");
    }
}

static void Fusa_PrintCreg(const struct fusa_mem *fm,
                           AMBA_SHELL_PRINT_f print)
{
    print("------------------------- creg -------------------------");
    print("\ntime(us) (min/avg/max): ");
    print_int32(fm->creg_q.stat.min_us, print);
    print("/");
    print_int32(fm->creg_q.stat.avg_us, print);
    print("/");
    print_int32(fm->creg_q.stat.max_us, print);
    print("\ncount/pass/fail:        ");
    print_int32(fm->creg_q.stat.count, print);
    print("/");
    print_int32(fm->creg_q.stat.npass, print);
    print("/");
    print_int32(fm->creg_q.stat.nfail, print);
    print("\n");
}

static void Fusa_PrintBp(const struct fusa_mem *fm,
                         AMBA_SHELL_PRINT_f print)
{
    print("------------------------- bp -------------------------");
    print("\nbr_pred (min/avg/max):          ");
    print_int32(fm->bp_q.stat.min_br_pred, print);
    print("/");
    print_int32(fm->bp_q.stat.avg_br_pred, print);
    print("/");
    print_int32(fm->bp_q.stat.max_br_pred, print);
    print("\ncpu_cycles (min/avg/max):       ");
    print_int32(fm->bp_q.stat.min_cpu_cycles, print);
    print("/");
    print_int32(fm->bp_q.stat.avg_cpu_cycles, print);
    print("/");
    print_int32(fm->bp_q.stat.max_cpu_cycles, print);
    print("\nbr_mis_pred (min/avg/max):      ");
    print_int32(fm->bp_q.stat.min_br_mis_pred, print);
    print("/");
    print_int32(fm->bp_q.stat.avg_br_mis_pred, print);
    print("/");
    print_int32(fm->bp_q.stat.max_br_mis_pred, print);
    print("\nbr_immed_retried (min/avg/max): ");
    print_int32(fm->bp_q.stat.min_br_immed_retried, print);
    print("/");
    print_int32(fm->bp_q.stat.avg_br_immed_retried, print);
    print("/");
    print_int32(fm->bp_q.stat.max_br_immed_retried, print);
    print("\ntime(us) (min/avg/max):         ");
    print_int32(fm->bp_q.stat.min_us, print);
    print("/");
    print_int32(fm->bp_q.stat.avg_us, print);
    print("/");
    print_int32(fm->bp_q.stat.max_us, print);
    print("\ncount/pass/fail:                ");
    print_int32(fm->bp_q.stat.count, print);
    print("/");
    print_int32(fm->bp_q.stat.npass, print);
    print("/");
    print_int32(fm->bp_q.stat.nfail, print);
    print("\n");
}

static void Fusa_PrintL2(const struct fusa_mem *fm,
                         AMBA_SHELL_PRINT_f print)
{
    print("------------------------- l2 -------------------------");
    print("\nl2d_cache (min/avg/max):  ");
    print_int32(fm->l2_q.stat.min_l2d_cache, print);
    print("/");
    print_int32(fm->l2_q.stat.avg_l2d_cache, print);
    print("/");
    print_int32(fm->l2_q.stat.max_l2d_cache, print);
    print("\nmem_access (min/avg/max): ");
    print_int32(fm->l2_q.stat.min_mem_access, print);
    print("/");
    print_int32(fm->l2_q.stat.avg_mem_access, print);
    print("/");
    print_int32(fm->l2_q.stat.max_mem_access, print);
    print("\nl1d_cache (min/avg/max):  ");
    print_int32(fm->l2_q.stat.min_l1d_cache, print);
    print("/");
    print_int32(fm->l2_q.stat.avg_l1d_cache, print);
    print("/");
    print_int32(fm->l2_q.stat.max_l1d_cache, print);
    print("\ntime(us) (min/avg/max):   ");
    print_int32(fm->l2_q.stat.min_us, print);
    print("/");
    print_int32(fm->l2_q.stat.avg_us, print);
    print("/");
    print_int32(fm->l2_q.stat.max_us, print);
    print("\ncount/pass/fail:          ");
    print_int32(fm->l2_q.stat.count, print);
    print("/");
    print_int32(fm->l2_q.stat.npass, print);
    print("/");
    print_int32(fm->l2_q.stat.nfail, print);
    print("\n");
}

static void Fusa_PrintEccfi(const struct fusa_mem *fm,
                            AMBA_SHELL_PRINT_f print)
{
    print("------------------------- eccfi -------------------------");
    print("\ntime(us) (min/avg/max): ");
    print_int32(fm->eccfi_q.stat.min_us, print);
    print("/");
    print_int32(fm->eccfi_q.stat.avg_us, print);
    print("/");
    print_int32(fm->eccfi_q.stat.max_us, print);
    print("\ncount/pass/fail:        ");
    print_int32(fm->eccfi_q.stat.count, print);
    print("/");
    print_int32(fm->eccfi_q.stat.npass, print);
    print("/");
    print_int32(fm->eccfi_q.stat.nfail, print);
    print("\n");
}

static void Fusa_PrintR52(const struct fusa_mem *fm,
                          AMBA_SHELL_PRINT_f print)
{
    print("------------------------- r52 -------------------------");
    print("\necctcm (p/f):        ");
    print_int32(fm->cr52_diag.ecctcm_pass, print);
    print("/");
    print_int32(fm->cr52_diag.ecctcm_fail, print);
    print("\necccache (p/f):      ");
    print_int32(fm->cr52_diag.ecccache_pass, print);
    print("/");
    print_int32(fm->cr52_diag.ecccache_fail, print);
    print("\nlscompfi (p/f):      ");
    print_int32(fm->cr52_diag.lscompfi_pass, print);
    print("/");
    print_int32(fm->cr52_diag.lscompfi_fail, print);
    print("\nr52rdatafi (p/f):    ");
    print_int32(fm->cr52_diag.r52rdatafi_pass, print);
    print("/");
    print_int32(fm->cr52_diag.r52rdatafi_fail, print);
    print("\na53rdatafi (p/f):    ");
    print_int32(fm->cr52_diag.a53rdatafi_pass, print);
    print("/");
    print_int32(fm->cr52_diag.a53rdatafi_fail, print);
    print("\nsystimerr52fi (p/f): ");
    print_int32(fm->cr52_diag.systimerr52fi_pass, print);
    print("/");
    print_int32(fm->cr52_diag.systimerr52fi_fail, print);
    print("\nsystimera53fi (p/f): ");
    print_int32(fm->cr52_diag.systimera53fi_pass, print);
    print("/");
    print_int32(fm->cr52_diag.systimera53fi_fail, print);
    print("\n");
}

static void AmbaShell_CommandFuSa_Usage(char * const * pArgVector,
                                        AMBA_SHELL_PRINT_f print)
{
    print("Usage: ");
    print(pArgVector[0]);
    print("\n");
    print("  vers    - library versions\n");
    print("  stats   - execution statistics\n");
    print("  enable  [ex|isa|creg|bp|l2|eccfi]\n");
    print("  disable [ex|isa|creg|bp|l2|eccfi]\n");
    print("  inject  [ex|isa|creg|bp|l2|eccfi]\n");
}

/**
 *  AmbaShell_CommandFuSa - execute fusa command
 *
 *  @param[in] ArgCount argument count
 *  @param[in] pArgVector argument
 *  @param[in] PrintFunc print function pointer
 */
void AmbaShell_CommandFuSa(UINT32 ArgCount, char * const * pArgVector,
                           AMBA_SHELL_PRINT_f PrintFunc)
{
    const struct fusa_mem *fm = NULL;
    UINT32 baseaddr = SHADOWBASE_FUSA;

    AmbaMisra_TypeCast32(&fm, &baseaddr);

    if (AmbaUtility_StringCompare(pArgVector[1], "vers", 4U) == 0) {
        if (ArgCount == 2U) {
            Fusa_PrintVers(fm, PrintFunc);
        } else {
            AmbaShell_CommandFuSa_Usage(pArgVector, PrintFunc);
        }
    } else if (AmbaUtility_StringCompare(pArgVector[1], "stats", 5U) == 0) {
        if (ArgCount == 2U) {
            Fusa_PrintEx(fm, PrintFunc);
            Fusa_PrintIsa(fm, PrintFunc);
            Fusa_PrintCreg(fm, PrintFunc);
            Fusa_PrintBp(fm, PrintFunc);
            Fusa_PrintL2(fm, PrintFunc);
            Fusa_PrintEccfi(fm, PrintFunc);
            Fusa_PrintR52(fm, PrintFunc);
        } else {
            AmbaShell_CommandFuSa_Usage(pArgVector, PrintFunc);
        }
    } else if (AmbaUtility_StringCompare(pArgVector[1], "enable", 6U) == 0) {
        if (ArgCount == 3U) {
            if (AmbaUtility_StringCompare(pArgVector[2], "ex", 2U) == 0) {
                AmbaCortexA53FusaEnable(FUSA_RUN_EXCEPTION);
            } else if (AmbaUtility_StringCompare(pArgVector[2], "isa", 3U) == 0) {
                AmbaCortexA53FusaEnable(FUSA_RUN_ISA);
            } else if (AmbaUtility_StringCompare(pArgVector[2], "creg", 4U) == 0) {
                AmbaCortexA53FusaEnable(FUSA_RUN_CREG);
            } else if (AmbaUtility_StringCompare(pArgVector[2], "bp", 2U) == 0) {
                AmbaCortexA53FusaEnable(FUSA_RUN_BP);
            } else if (AmbaUtility_StringCompare(pArgVector[2], "l2", 2U) == 0) {
                AmbaCortexA53FusaEnable(FUSA_RUN_L2);
            } else if (AmbaUtility_StringCompare(pArgVector[2], "eccfi", 5U) == 0) {
                AmbaCortexA53FusaEnable(FUSA_RUN_ECCFI);
            } else {
                AmbaShell_CommandFuSa_Usage(pArgVector, PrintFunc);
            }
        } else {
            AmbaShell_CommandFuSa_Usage(pArgVector, PrintFunc);
        }
    } else if (AmbaUtility_StringCompare(pArgVector[1], "disable", 7U) == 0) {
        if (ArgCount == 3U) {
            if (AmbaUtility_StringCompare(pArgVector[2], "ex", 2U) == 0) {
                AmbaCortexA53FusaDisable(FUSA_RUN_EXCEPTION);
            } else if (AmbaUtility_StringCompare(pArgVector[2], "isa", 3U) == 0) {
                AmbaCortexA53FusaDisable(FUSA_RUN_ISA);
            } else if (AmbaUtility_StringCompare(pArgVector[2], "creg", 4U) == 0) {
                AmbaCortexA53FusaDisable(FUSA_RUN_CREG);
            } else if (AmbaUtility_StringCompare(pArgVector[2], "bp", 2U) == 0) {
                AmbaCortexA53FusaDisable(FUSA_RUN_BP);
            } else if (AmbaUtility_StringCompare(pArgVector[2], "l2", 2U) == 0) {
                AmbaCortexA53FusaDisable(FUSA_RUN_L2);
            } else if (AmbaUtility_StringCompare(pArgVector[2], "eccfi", 5U) == 0) {
                AmbaCortexA53FusaDisable(FUSA_RUN_ECCFI);
            } else {
                AmbaShell_CommandFuSa_Usage(pArgVector, PrintFunc);
            }
        } else {
            AmbaShell_CommandFuSa_Usage(pArgVector, PrintFunc);
        }
    } else if (AmbaUtility_StringCompare(pArgVector[1], "inject", 6U) == 0) {
        if (ArgCount == 3U) {
            if (AmbaUtility_StringCompare(pArgVector[2], "ex", 2U) == 0) {
                AmbaCortexA53FusaInject(FUSA_RUN_EXCEPTION);
            } else if (AmbaUtility_StringCompare(pArgVector[2], "isa", 3U) == 0) {
                AmbaCortexA53FusaInject(FUSA_RUN_ISA);
            } else if (AmbaUtility_StringCompare(pArgVector[2], "creg", 4U) == 0) {
                AmbaCortexA53FusaInject(FUSA_RUN_CREG);
            } else if (AmbaUtility_StringCompare(pArgVector[2], "bp", 2U) == 0) {
                AmbaCortexA53FusaInject(FUSA_RUN_BP);
            } else if (AmbaUtility_StringCompare(pArgVector[2], "l2", 2U) == 0) {
                AmbaCortexA53FusaInject(FUSA_RUN_L2);
            } else if (AmbaUtility_StringCompare(pArgVector[2], "eccfi", 5U) == 0) {
                AmbaCortexA53FusaInject(FUSA_RUN_ECCFI);
            } else {
                AmbaShell_CommandFuSa_Usage(pArgVector, PrintFunc);
            }
        } else {
            AmbaShell_CommandFuSa_Usage(pArgVector, PrintFunc);
        }
    } else {
        AmbaShell_CommandFuSa_Usage(pArgVector, PrintFunc);
    }

    return;
}

/*
 * Local variables:
 * mode: C
 * c-file-style: "BSD"
 * c-basic-offset: 4
 * tab-width: 4
 * indent-tabs-mode: nil
 * End:
 */
