/**
 *  @file AmbaReg_AxiConfig.h
 *
 *  @copyright Copyright (c) 2019 Ambarella, Inc.
 *
 *  This file and its contents ("Software") are protected by intellectual property rights including, without limitation,
 *  U.S. and/or foreign copyrights.  This Software is also the confidential and proprietary information of Ambarella, Inc.
 *  and its licensors.  You may not use, reproduce, disclose, distribute, modify, or otherwise prepare derivative works
 *  of this Software or any portion thereof except pursuant to a signed license agreement or nondisclosure agreement with
 *  Ambarella, Inc. or its authorized affiliates.  In the absence of such an agreement, you agree to promptly notify and
 *  return this Software to Ambarella, Inc.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 *  AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details Definitions & Constants for AXI Internal Configuration APIs
 *
 */

#ifndef AMBA_REG_AXI_CONFIG_H
#define AMBA_REG_AXI_CONFIG_H

#ifndef AMBA_TYPES_H
#include "AmbaTypes.h"
#endif

/*
 * AXI Config: Cortex Misc Control Register
 */
typedef struct {
    UINT32  CortexCore0SecureDisable:   1;      /* [0] 1 = Disable Cortex core-0 CP15 security extension */
    UINT32  CortexCore1SecureDisable:   1;      /* [1] 1 = Disable Cortex core-1 CP15 security extension */
    UINT32  CortexCore2SecureDisable:   1;      /* [2] 1 = Disable Cortex core-2 CP15 security extension */
    UINT32  CortexCore3SecureDisable:   1;      /* [3] 1 = Disable Cortex core-3 CP15 security extension */
    UINT32  GicSecureDisable:           1;      /* [4] 1 = Disable GIC secure lockdown */
    UINT32  Reserved:                   27;     /* [31:5] */
} AMBA_AXI_CORTEX_MISC_REG_s;

/*
 * AXI Config: System Timer Register
 */
typedef struct {
    UINT32  CortexSysTimerEnable:       1;      /* [0] 1 = Enable system counter for Cortex0 */
    UINT32  CortexClkDivider:           3;      /* [3:1] System counter divider value */
    UINT32  Reserved:                   28;     /* [31:4] */
} AMBA_AXI_SYS_TIMER_CTRL_REG_s;

/*
 * AXI Config: NIC-400 GPV Control Register
 */
typedef struct {
    UINT32  ClkEnable:                  1;      /* [0] 1 = Enable NIC-400 main clock (NIC-400 GPV clock) */
    UINT32  Reserved:                   31;     /* [31:1] */
} AMBA_AXI_NIC400_GPV_CTRL_REG_s;

/*
 * AXI Config: Cortex Crypto Control Register
 */
typedef struct {
    UINT32  CortexCore0CryptoCtrl:      1;      /* [0] 1 = Disable cryptography extension of Cortex core0 */
    UINT32  CortexCore1CryptoCtrl:      1;      /* [1] 1 = Disable cryptography extension of Cortex core1 */
    UINT32  CortexCore2CryptoCtrl:      1;      /* [2] 1 = Disable cryptography extension of Cortex core2 */
    UINT32  CortexCore3CryptoCtrl:      1;      /* [3] 1 = Disable cryptography extension of Cortex core3 */
    UINT32  Reserved:                   28;     /* [31:4] */
} AMBA_AXI_CORTEX_CRYPTO_CTRL_REG_s;

/*
 * AXI Config: Cortex Architecture Control Register
 */
typedef struct {
    UINT32  CortexCore0ExecutionState:  1;      /* [0] Cortex core-0 execution state, 0 = AArch32, 1 = AArch64 */
    UINT32  CortexCore1ExecutionState:  1;      /* [1] Cortex core-1 execution state, 0 = AArch32, 1 = AArch64 */
    UINT32  CortexCore2ExecutionState:  1;      /* [2] Cortex core-2 execution state, 0 = AArch32, 1 = AArch64 */
    UINT32  CortexCore3ExecutionState:  1;      /* [3] Cortex core-3 execution state, 0 = AArch32, 1 = AArch64 */
    UINT32  Reserved:                   28;     /* [31:4] */
} AMBA_AXI_CORTEX_ARCH_CTRL_REG_s;

/*
 * AXI Config: Cortex Misc Status Register
 */
typedef struct {
    UINT32  Reserved0:                  2;      /* [1:0] */
    UINT32  CortexCore0WFI:             1;      /* [2] 1 = Cortex core-0 in WFI(wait for interrupt) state */
    UINT32  CortexCore1WFI:             1;      /* [3] 1 = Cortex core-1 in WFI(wait for interrupt) state */
    UINT32  CortexCore2WFI:             1;      /* [4] 1 = Cortex core-2 in WFI(wait for interrupt) state */
    UINT32  CortexCore3WFI:             1;      /* [5] 1 = Cortex core-3 in WFI(wait for interrupt) state */
    UINT32  CortexCore0WFE:             1;      /* [6] 1 = Cortex core-0 in WFE(wait for event) state */
    UINT32  CortexCore1WFE:             1;      /* [7] 1 = Cortex core-1 in WFE(wait for event) state */
    UINT32  CortexCore2WFE:             1;      /* [8] 1 = Cortex core-2 in WFE(wait for event) state */
    UINT32  CortexCore3WFE:             1;      /* [9] 1 = Cortex core-3 in WFE(wait for event) state */
    UINT32  CortexWfiL2:                1;      /* [10] Indicate if processor in WFIL2(L2 idle and doesn't accept transactions) state */
    UINT32  Reserved1:                  9;      /* [19:11] */
    UINT32  CortexCore0SmpEable:        1;      /* [20] Indicate Cortex core-0 in SMP or AMP mode, 1 = SMP, 0 = AMP */
    UINT32  CortexCore1SmpEable:        1;      /* [21] Indicate Cortex core-1 in SMP or AMP mode, 1 = SMP, 0 = AMP */
    UINT32  CortexCore2SmpEable:        1;      /* [22] Indicate Cortex core-2 in SMP or AMP mode, 1 = SMP, 0 = AMP */
    UINT32  CortexCore3SmpEable:        1;      /* [23] Indicate Cortex core-3 in SMP or AMP mode, 1 = SMP, 0 = AMP */
    UINT32  Reserved2:                  8;      /* [31:24] */
} AMBA_AXI_CORTEX_MISC_STATUS_REG_s;

/*
 * AXI Config: Cortex Reset Register
 */
typedef struct {
    UINT32  Reserved0:                  4;      /* [3:0] */
    UINT32  CortexWfiEvent:             1;      /* [4] Reset to Cortex event input for processor wake up from WFE state */
    UINT32  Reserved1:                  11;     /* [15:5] */
    UINT32  CortexCore0Reset:           1;      /* [16] 1 = Reset Cortex core-0, after power on reset */
    UINT32  CortexCore1Reset:           1;      /* [17] 1 = Reset Cortex core-1, after power on reset */
    UINT32  CortexCore2Reset:           1;      /* [18] 1 = Reset Cortex core-2, after power on reset */
    UINT32  CortexCore3Reset:           1;      /* [19] 1 = Reset Cortex core-3, after power on reset */
    UINT32  CortexL2Reset:              1;      /* [20] 1 = Reset Cortex L2 cache */
    UINT32  Reserved2:                  11;     /* [31:21] */
} AMBA_AXI_CORTEX_RESET_REG_s;

/*
 * AXI Config: Cortex Vector Table Control Register
 */
typedef struct {
    UINT32  Core0HighVector:            1;      /* [0] Exception vector table is at 0 = 0x00000000, 1 = 0xFFFF0000 */
    UINT32  Core1HighVector:            1;      /* [1] Exception vector table is at 0 = 0x00000000, 1 = 0xFFFF0000 */
    UINT32  Core2HighVector:            1;      /* [2] Exception vector table is at 0 = 0x00000000, 1 = 0xFFFF0000 */
    UINT32  Core3HighVector:            1;      /* [3] Exception vector table is at 0 = 0x00000000, 1 = 0xFFFF0000 */
    UINT32  Reserved:                   28;     /* [31:4] */
} AMBA_AXI_CORTEX_VECTOR_CTRL_REG_s;

/*
 * AXI Config: All Registers
 */
typedef struct {
    volatile UINT32                             Reserved0;              /* 0x000: Reserved */
    volatile AMBA_AXI_CORTEX_MISC_REG_s         CortexMiscCtrl;         /* 0x004(RW): AXI Cortex Misc Control Register */
    volatile UINT32                             Reserved1[3];           /* 0x008-0x010: Reserved */
    volatile AMBA_AXI_SYS_TIMER_CTRL_REG_s      SysTimerCtrl;           /* 0x014(RW): AXI Syetem Timer Control Register */
    volatile AMBA_AXI_NIC400_GPV_CTRL_REG_s     Nic400GpvCtrl;          /* 0x018(RW): NIC400 GPV Control Register */
    volatile AMBA_AXI_CORTEX_CRYPTO_CTRL_REG_s  CryptoCtrl;             /* 0x01C(RW): Coretex Crypto Control Register */
    volatile AMBA_AXI_CORTEX_ARCH_CTRL_REG_s    ArchCtrl;               /* 0x020(RW): Coretex Arch Control Register */
    volatile AMBA_AXI_CORTEX_MISC_STATUS_REG_s  MiscStatus;             /* 0x024(RO): Coretex Misc Status Register */
    volatile AMBA_AXI_CORTEX_RESET_REG_s        CortexReset;            /* 0x028(RW): Coretex Reset Register */
    volatile UINT32                             FlashIoBootDone;        /* 0x02C(RO): Flash IO Boot Done Status (LSB 1-bit) */
    volatile UINT32                             Reserved2[7];           /* 0x030-0x048: Reserved */
    volatile UINT32                             JtagTopStatus;          /* 0x04C(RO): JTAG Top Status (LSB 1-bit) */
    volatile UINT32                             AxiDebugCtrl;           /* 0x050(RW): AXI Debug Control */
    volatile UINT32                             Reserved3[4];           /* 0x054-0x060: Reserved */
    volatile UINT32                             CortexCore0RVBA;        /* 0x064(RW): A64 Reset Vector Base Address for Cortex cluster 0 core-0 */
    volatile UINT32                             CortexCore1RVBA;        /* 0x068(RW): A64 Reset Vector Base Address for Cortex cluster 0 core-1 */
    volatile UINT32                             CortexCore2RVBA;        /* 0x06C(RW): A64 Reset Vector Base Address for Cortex cluster 0 core-2 */
    volatile UINT32                             CortexCore3RVBA;        /* 0x070(RW): A64 Reset Vector Base Address for Cortex cluster 0 core-3 */
    volatile UINT32                             Reserved4;              /* 0x074: Reserved */
    volatile AMBA_AXI_CORTEX_VECTOR_CTRL_REG_s  CortexVectorCtrl;       /* 0x078(RW): A32 Location of the Exception Vectors at reset for Cortex cluster 0 */
    volatile UINT32                             Reserved5;              /* 0x07C: Reserved */
    volatile UINT32                             BypassBootChange;       /* 0x080(RW): Polling this register until 1 at bypass boot with NAND */
    volatile UINT32                             BypassBootTarget;       /* 0x084(RW): Target Address for bypass boot with NAND */
    volatile UINT32                             Reserved6[2];           /* 0x088-0x08C: Reserved */
    volatile UINT32                             SecureLvl[3];           /* 0x090-0x098(RW): Security Control of each Module */
    volatile UINT32                             Reserved7[9];           /* 0x09C-0x0BC: Reserved */
    volatile UINT32                             CodeCurrTime;           /* 0x0C0(RW): Current time from CODE */
} AMBA_AXI_CONFIG_REG_s;

#endif /* AMBA_REG_AXI_CONFIG_H */
