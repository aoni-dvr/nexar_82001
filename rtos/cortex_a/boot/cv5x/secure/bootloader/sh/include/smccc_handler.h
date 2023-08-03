/**
 * Copyright (c) 2020 Ambarella International LP
 *
 * This file and its contents ("Software") are protected by intellectual
 * property rights including, without limitation, U.S. and/or foreign
 * copyrights. This Software is also the confidential and proprietary
 * information of Ambarella International LP and its licensors. You may not use, reproduce,
 * disclose, distribute, modify, or otherwise prepare derivative works of this
 * Software or any portion thereof except pursuant to a signed license agreement
 * or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
 * In the absence of such an agreement, you agree to promptly notify and return
 * this Software to Ambarella International LP
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 * MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */

#ifndef __SMCCC_HANDLER_H__
#define __SMCCC_HANDLER_H__

/*******************************************************************************
 * gp register context
 ******************************************************************************/
#define CTX_GPREGS_OFFSET    (0x0)
#define CTX_GPREG_X0        (0x0)
#define CTX_GPREG_X1        (0x8)
#define CTX_GPREG_X2        (0x10)
#define CTX_GPREG_X3        (0x18)
#define CTX_GPREG_X4        (0x20)
#define CTX_GPREG_X5        (0x28)
#define CTX_GPREG_X6        (0x30)
#define CTX_GPREG_X7        (0x38)
#define CTX_GPREG_X8        (0x40)
#define CTX_GPREG_X9        (0x48)
#define CTX_GPREG_X10        (0x50)
#define CTX_GPREG_X11        (0x58)
#define CTX_GPREG_X12        (0x60)
#define CTX_GPREG_X13        (0x68)
#define CTX_GPREG_X14        (0x70)
#define CTX_GPREG_X15        (0x78)
#define CTX_GPREG_X16        (0x80)
#define CTX_GPREG_X17        (0x88)
#define CTX_GPREG_X18        (0x90)
#define CTX_GPREG_X19        (0x98)
#define CTX_GPREG_X20        (0xa0)
#define CTX_GPREG_X21        (0xa8)
#define CTX_GPREG_X22        (0xb0)
#define CTX_GPREG_X23        (0xb8)
#define CTX_GPREG_X24        (0xc0)
#define CTX_GPREG_X25        (0xc8)
#define CTX_GPREG_X26        (0xd0)
#define CTX_GPREG_X27        (0xd8)
#define CTX_GPREG_X28        (0xe0)
#define CTX_GPREG_X29        (0xe8)
#define CTX_GPREG_LR        (0xf0)
#define CTX_GPREG_SP_EL0    (0xf8)
#define CTX_GPREGS_END        (0x100)

/*******************************************************************************
 * el3_stat register context
 ******************************************************************************/
#define CTX_EL3STATE_OFFSET    (CTX_GPREGS_OFFSET + CTX_GPREGS_END)
#define CTX_SCR_EL3        (0x0)
#define CTX_ESR_EL3        (0x8)
#define CTX_RUNTIME_SP        (0x10)
#define CTX_SPSR_EL3        (0x18)
#define CTX_ELR_EL3        (0x20)
#define CTX_UNUSED        (0x28)
#define CTX_EL3STATE_END    (0x30)

/*******************************************************************************
 * system register context
 ******************************************************************************/
#define CTX_SYSREGS_OFFSET    (CTX_EL3STATE_OFFSET + CTX_EL3STATE_END)
#define CTX_SPSR_EL1        (0x0)
#define CTX_ELR_EL1        (0x8)
#define CTX_SCTLR_EL1        (0x10)
#define CTX_ACTLR_EL1        (0x18)
#define CTX_CPACR_EL1        (0x20)
#define CTX_CSSELR_EL1        (0x28)
#define CTX_SP_EL1        (0x30)
#define CTX_ESR_EL1        (0x38)
#define CTX_TTBR0_EL1        (0x40)
#define CTX_TTBR1_EL1        (0x48)
#define CTX_MAIR_EL1        (0x50)
#define CTX_AMAIR_EL1        (0x58)
#define CTX_TCR_EL1        (0x60)
#define CTX_TPIDR_EL1        (0x68)
#define CTX_TPIDR_EL0        (0x70)
#define CTX_TPIDRRO_EL0        (0x78)
#define CTX_PAR_EL1        (0x80)
#define CTX_FAR_EL1        (0x88)
#define CTX_AFSR0_EL1        (0x90)
#define CTX_AFSR1_EL1        (0x98)
#define CTX_CONTEXTIDR_EL1    (0xa0)
#define CTX_VBAR_EL1        (0xa8)
#define CTX_PMCR_EL0        (0xb0)
#define CTX_SYSREGS_END        (0xc0)

/* Exception Syndrome register bits */
#define DAIF_ABT_BIT        (1 << 2)
#define ESR_EC_SHIFT        (26)
#define ESR_EC_MASK        (0x3f)
#define ESR_EC_LENGTH        (6)
#define EC_AARCH64_SMC        (0x17)

/* context & stack size for smc calling */
#define CONTEXT_STACK_SIZE    (0x1000)
#define CTX_GPREG_ALL        (CTX_GPREGS_END >> 3)
#define CTX_EL3STATE_ALL    (CTX_EL3STATE_END >> 3)
#define CTX_SYSREG_ALL        (CTX_SYSREGS_END >> 3)
#define CPU_CONTEXT_INDEX(x)    ((x) >> 3)

#if 0 /* --- tw start */
#ifndef __ASM__
struct smc_cpu_context {
    unsigned long gp_reg[CTX_GPREG_ALL];
    unsigned long el3stat_reg[CTX_EL3STATE_ALL];
    unsigned long sys_reg[CTX_SYSREG_ALL];
};

extern void amboot_psci_ctx_init(void);
extern void amboot_power_down_wfi(void);
extern void master_cpu_gic_setup(void);
extern void amboot_psci_exit(void);
#endif /* +++ tw end */
#endif

#endif
