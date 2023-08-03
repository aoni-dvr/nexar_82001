/**
 *  @file AmbaReg_GIC.h
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
 *  @details Definitions & Constants for CoreLink GIC-400 with Security Extensions Registers
 *
 */

#ifndef AMBA_REG_GIC_H
#define AMBA_REG_GIC_H

#ifndef AMBA_TYPES_H
#include "AmbaTypes.h"
#endif

#define NUM_INT_GROUP_REG           16U
#define NUM_INT_CONFIG_REG          32U
#define NUM_INT_PRIORITY_REG        128U
#define NUM_INT_TARGET_REG          128U
#define NUM_INT_CLEAN_PENDING_REG   16U
#define NUM_INT_CLEAN_ENABLE_REG    16U

/*
 * GIC Distributor: Control Register (GICD_CTLR)
 */
typedef struct {
    UINT32  EnableGroup0:           1;      /* [0]: 1 = Enable signaling of Group 0 interrupts */
    UINT32  EnableGroup1:           1;      /* [1]: 1 = Enable signaling of Group 1 interrupts */
    UINT32  Reserved:               30;     /* [31:2]: Reserved */
} AMBA_GIC_DIST_CTRL_REG_s;

/*
 * GIC Distributor: Interrupt Controller Type Register (GICD_TYPER)
 */
typedef struct {
    UINT32  IntLinesNumber:         5;      /* [4:0]: the maximum number of interrupts = 32*(n+1) */
    UINT32  CpuNumber:              3;      /* [7:5]: the number of implemented CPU interfaces = n+1 */
    UINT32  Reserved0:              2;      /* [9:8]: Reserved */
    UINT32  SecurityExtn:           1;      /* [10]: 1 = Security Extensions implemented */
    UINT32  LockableSPIs:           5;      /* [15:11]: the number of Lockable Shared Peripheral Interrupts (LSPIs) */
    UINT32  Reserved1:              16;     /* [31:16]: Reserved */
} AMBA_GIC_DIST_INT_CTRL_TYPE_REG_s;

/*
 * GIC Distributor: Implementer Identification Register (GICD_IIDR)
 */
typedef struct {
    UINT32  Implementer:            12;     /* [11:0]: JPE106 code of the Implementer, ARM = 0x43B */
    UINT32  Revision:               4;      /* [15:12]: Revision number, 0x1 */
    UINT32  Variant:                4;      /* [19:16]: IMPLEMENTATION DEFINED variant number, 0x0 */
    UINT32  Reserved:               4;      /* [23:20]: Reserved */
    UINT32  ProductID:              8;      /* [31:24]: IMPLEMENTATION DEFINED product identifier, GIC-400 = 0x02 */
} AMBA_GIC_DIST_INT_VENDOR_ID_REG_s;

/*
 * GIC Distributor: Interrupt Configuration Register (GICD_ICFGRn)
 */
typedef struct {
    UINT32  IntConfig0:             2;      /* [1:0]: Interrupt N+0 is 2'b00 = level-sensitive, 2'b10 = edge-triggered */
    UINT32  IntConfig1:             2;      /* [3:2]: Interrupt N+1 is 2'b00 = level-sensitive, 2'b10 = edge-triggered */
    UINT32  IntConfig2:             2;      /* [5:4]: Interrupt N+2 is 2'b00 = level-sensitive, 2'b10 = edge-triggered */
    UINT32  IntConfig3:             2;      /* [7:6]: Interrupt N+3 is 2'b00 = level-sensitive, 2'b10 = edge-triggered */
    UINT32  IntConfig4:             2;      /* [9:8]: Interrupt N+4 is 2'b00 = level-sensitive, 2'b10 = edge-triggered */
    UINT32  IntConfig5:             2;      /* [11:10]: Interrupt N+5 is 2'b00 = level-sensitive, 2'b10 = edge-triggered */
    UINT32  IntConfig6:             2;      /* [13:12]: Interrupt N+6 is 2'b00 = level-sensitive, 2'b10 = edge-triggered */
    UINT32  IntConfig7:             2;      /* [15:14]: Interrupt N+7 is 2'b00 = level-sensitive, 2'b10 = edge-triggered */
    UINT32  IntConfig8:             2;      /* [17:16]: Interrupt N+8 is 2'b00 = level-sensitive, 2'b10 = edge-triggered */
    UINT32  IntConfig9:             2;      /* [19:18]: Interrupt N+9 is 2'b00 = level-sensitive, 2'b10 = edge-triggered */
    UINT32  IntConfig10:            2;      /* [21:20]: Interrupt N+10 is 2'b00 = level-sensitive, 2'b10 = edge-triggered */
    UINT32  IntConfig11:            2;      /* [23:22]: Interrupt N+11 is 2'b00 = level-sensitive, 2'b10 = edge-triggered */
    UINT32  IntConfig12:            2;      /* [25:24]: Interrupt N+12 is 2'b00 = level-sensitive, 2'b10 = edge-triggered */
    UINT32  IntConfig13:            2;      /* [27:26]: Interrupt N+13 is 2'b00 = level-sensitive, 2'b10 = edge-triggered */
    UINT32  IntConfig14:            2;      /* [29:28]: Interrupt N+14 is 2'b00 = level-sensitive, 2'b10 = edge-triggered */
    UINT32  IntConfig15:            2;      /* [31:30]: Interrupt N+15 is 2'b00 = level-sensitive, 2'b10 = edge-triggered */
} AMBA_GIC_DIST_INT_CONFIG_REG_s;

/*
 * GIC Distributor: Private Peripheral Interrupt Status Register (GICD_PPISR)
 */
typedef struct {
    UINT32  Reserved0:              9;      /* [8:0]: Reserved */
    UINT32  VirtualMaintenanceInt:  1;      /* [9]: 1 = Virtual maintenance interrupt */
    UINT32  HypervisorTimerInt:     1;      /* [10]: 1 = Hypervisor timer event */
    UINT32  VirtTimerInt:           1;      /* [11]: 1 = Virtual timer event */
    UINT32  LegacyFIQ:              1;      /* [12]: 1 = nLEGACYFIQ signal */
    UINT32  SecurePhysTimerInt:     1;      /* [13]: 1 = Secure physical timer event */
    UINT32  NonSecurePhysTimerInt:  1;      /* [14]: 1 = Non-Secure physical timer event */
    UINT32  LegacyIRQ:              1;      /* [15]: 1 = nLEGACYIRQ signal */
    UINT32  Reserved1:              16;     /* [31:16]: Reserved */
} AMBA_GIC_DIST_PPI_STATUS_REG_s;

/*
 * GIC Distributor: Software Generated Interrupt Register (GICD_SGIR)
 */
typedef struct {
    UINT32  SgiIntID:               4;      /* [3:0]: The Interrupt ID of the SGI to send to the specified CPU interfaces */
    UINT32  Reserved0:              11;     /* [14:4]: Reserved, SBZ */
    UINT32  Attribute:              1;      /* [15]: Specifies the required security value of the SGI */
    UINT32  CpuTargetList:          8;      /* [23:16]: When TargetList Filter = 0b00, defines the CPU interfaces the Distributor must send to */
    UINT32  CpuTargetListFilter:    2;      /* [25:24]: CPU Target List Filter */
    UINT32  Reserved1:              6;      /* [31:26]: Reserved */
} AMBA_GIC_DIST_SGI_REG_s;

/*
 * GIC Distributor All Registers
 */
typedef struct {                                                                            /* Offset   : Description */
    volatile UINT32                             IntCtrl;                                    /* 0x000(RW,Secure/Non-secure banked): Distributor Control Register (GICD_CTLR) (LSB 1-bit): 1 = Global enable */
    volatile AMBA_GIC_DIST_INT_CTRL_TYPE_REG_s  IntCtrlType;                                /* 0x004(RO): Interrupt Controller Type Register (GICD_TYPER) */
    volatile AMBA_GIC_DIST_INT_VENDOR_ID_REG_s  IntVendorID;                                /* 0x008(RO): Distributor Implementer Identification Register (GICD_IIDR) */
    volatile UINT32                             Reserved0[29];                              /* 0x00C-0x07C: Reserved */

    volatile UINT32                             IntGroup[NUM_INT_GROUP_REG];                /* 0x080-0x0BC(RW,Secure Only): Interrupt Group Registers (GICD_IGROUPRn) */
    volatile UINT32                             Reserved1[16];                              /* 0x0C0-0x0FC: Reserved */

    volatile UINT32                             IntSetEnable[16];                           /* 0x100-0x13C(RW): Interrupt Set-Enable Registers (GICD_ISENABLERn), 1 = Enable */
    volatile UINT32                             Reserved2[16];                              /* 0x140-0x17C: Reserved */

    volatile UINT32                             IntClearEnable[NUM_INT_CLEAN_ENABLE_REG];   /* 0x180-0x1BC(RW): Interrupt Clear-Enable Registers (GICD_ICENABLERn) */
    volatile UINT32                             Reserved3[16];                              /* 0x1C0-0x1FC: Reserved */

    volatile UINT32                             IntSetPending[16];                          /* 0x200-0x23C(RW): Interrupt Set-Pending Registers (GICD_ISPENDRn) */
    volatile UINT32                             Reserved4[16];                              /* 0x240-0x27C: Reserved */

    volatile UINT32                             IntClearPending[NUM_INT_CLEAN_PENDING_REG]; /* 0x280-0x2BC(RW): Interrupt Clear-Pending Registers (GICD_ICPENDRn) */
    volatile UINT32                             Reserved5[16];                              /* 0x2C0-0x2FC: Reserved */

    volatile UINT32                             IntSetActive[16];                           /* 0x300-0x33C(RW): Interrupt Set-Active Registers (GICD_ISACTIVERn) */
    volatile UINT32                             Reserved6[16];                              /* 0x340-0x37C: Reserved */

    volatile UINT32                             IntClearActive[16];                         /* 0x380-0x3BC(RW): Interrupt Clear-Active Registers (GICD_ICACTIVERn) */
    volatile UINT32                             Reserved7[16];                              /* 0x3C0-0x3FC: Reserved */

    volatile UINT32                             IntPriority[NUM_INT_PRIORITY_REG];          /* 0x400-0x5FC(RW): Interrupt Priority Registers (GICD_IPRIORITYRn) */
    volatile UINT32                             Reserved8[128];                             /* 0x600-0x7FC: Reserved */

    volatile UINT32                             IntTarget[NUM_INT_TARGET_REG];              /* 0x800-0x9FC(RO/RW): Interrupt Processor Targets Registers (GICD_ITARGETSRn) */
    volatile UINT32                             Reserved9[128];                             /* 0xA00-0xBFC: Reserved */

    volatile UINT32                             IntConfig[NUM_INT_CONFIG_REG];              /* 0xC00-0xC7C(RO/RW): Interrupt Configuration Registers (GICD_ICFGRn) */
    volatile UINT32                             Reserved10[32];                             /* 0xC80-0xCFC: Reserved */

    volatile AMBA_GIC_DIST_PPI_STATUS_REG_s     PpiStatus;                                  /* 0xD00(RO): Private Peripheral Interrupt Status Register (GICD_PPISR) */
    volatile UINT32                             SpiStatus[15];                              /* 0xD04-0xD3C(RO): Shared Peripheral Interrupt Status Registers,0/1 = IRQS Low/High (GICD_SPISRn) */
    volatile UINT32                             Reserved11[112];                            /* 0xD40-0xEFC: Reserved */

    volatile UINT32                             SGI;                                        /* 0xF00(WO): Software Generated Interrupt Register (GICD_SGIR) */
    volatile UINT32                             Reserved12[3];                              /* 0xF04-0xF0C: Reserved */
    volatile UINT32                             SgiClearPending[4];                         /* 0xF10-0xF1C(RW): SGI Clear-Pending Registers (GICD_CPENDSGIRn)*/
    volatile UINT32                             SgiSetPending[4];                           /* 0xF20-0xF2C(RW): SGI Set-Pending Registers (GICD_SPENDSGIRn)*/
    volatile UINT32                             Reserved13[40];                             /* 0xF30-0xFCC: Reserved */

    volatile UINT32                             PeripheralID4;                              /* 0xFD0(RO): Peripheral ID4 (GICD_PIDR4), ARM = 0x04 */
    volatile UINT32                             PeripheralID5;                              /* 0xFD4(RO): Peripheral ID5 (GICD_PIDR5), ARM = 0x00 */
    volatile UINT32                             PeripheralID6;                              /* 0xFD8(RO): Peripheral ID6 (GICD_PIDR6), ARM = 0x00 */
    volatile UINT32                             PeripheralID7;                              /* 0xFDC(RO): Peripheral ID7 (GICD_PIDR7), ARM = 0x00 */
    volatile UINT32                             PeripheralID0;                              /* 0xFE0(RO): Peripheral ID0 (GICD_PIDR0), ARM = 0x90 */
    volatile UINT32                             PeripheralID1;                              /* 0xFE4(RO): Peripheral ID1 (GICD_PIDR1), ARM = 0xB4 */
    volatile UINT32                             PeripheralID2;                              /* 0xFE8(RO): Peripheral ID2 (GICD_PIDR2), ARM = 0x2B */
    volatile UINT32                             PeripheralID3;                              /* 0xFEC(RO): Peripheral ID3 (GICD_PIDR3), ARM = 0x00 */
    volatile UINT32                             ComponentID0;                               /* 0xFF0(RO): Component ID0 (GICD_CIDR0), ARM = 0x0D */
    volatile UINT32                             ComponentID1;                               /* 0xFF4(RO): Component ID0 (GICD_CIDR1), ARM = 0xF0 */
    volatile UINT32                             ComponentID2;                               /* 0xFF8(RO): Component ID0 (GICD_CIDR2), ARM = 0x05 */
    volatile UINT32                             ComponentID3;                               /* 0xFFC(RO): Component ID0 (GICD_CIDR3), ARM = 0xB1 */
} AMBA_GIC_DISTRIBUTOR_REG_s;

/*
 * GIC CPU Interface Control Register (ICCICR) with Security Extensions
 */
typedef struct {
    UINT32  EnableGroup0:           1;      /* [0]: 1 = Enable signaling of Group 0 interrupts */
    UINT32  EnableGroup1:           1;      /* [1]: 1 = Enable signaling of Group 1 interrupts */
    UINT32  AckCtrl:                1;      /* [2]: 0 = Use a spurious interrupt ID 1022 to indicate the highest prority pending interrupt is a Group 1 interrupt */
    UINT32  FiqEnable:              1;      /* [3]: 1 = Group 0 interrupts are FIQ, 0 = Group 0 interrupts are IRQ */
    UINT32  BinaryPointRegSelect:   1;      /* [4]: 0 = GICC_BPR for Group 0 and GICC_ABPR for Group 1, 1 = GICC_BPR for both Group 0,1 */
    UINT32  FiqBypassDisable0:      1;      /* [5]: 1 = Disable the FIQ bypassing for processor powering down */
    UINT32  IrqBypassDisable0:      1;      /* [6]: 1 = Disable the IRQ bypassing for processor powering down */
    UINT32  FiqBypassDisable1:      1;      /* [7]: 1 = Disable the FIQ bypassing for processor powering down */
    UINT32  IrqBypassDisable1:      1;      /* [8]: 1 = Disable the IRQ bypassing for processor powering down */
    UINT32  Reserved:               23;     /* [31:9]: Reserved */
} AMBA_GIC_CPU_CTRL_REG_s;

/*
 * GIC CPU Interface: Interrupt Priority Mask Register (ICCPMR)
 */
typedef struct {
    UINT32  Priority:               8;      /* [7:0]: Priority mask level for the CPU interface */
    UINT32  Reserved:               24;     /* [31:8]: Reserved */
} AMBA_GIC_CPU_PRIORITY_MASK_REG_s;

/*
 * GIC CPU Interface: Binary Point Register (ICCBPR)
 */
typedef struct {
    UINT32  SplitValue:             3;      /* [2:0]: split into a group priority  */
    UINT32  Reserved:               29;     /* [31:3]: Reserved */
} AMBA_GIC_CPU_BINARY_POINT_REG_s;

/*
 * GIC CPU Interface: Interrupt Acknowledge Register (ICCIAR)
 */
typedef struct {
    UINT32  ActIntID:               10;     /* [9:0]: The interrupt ID */
    UINT32  CpuID:                  3;      /* [12:10]: The CPU ID which generated the SGI */
    UINT32  Reserved:               19;     /* [31:13]: Reserved */
} AMBA_GIC_CPU_INT_ACK_REG_s;

/*
 * GIC CPU Interface: End of Interrupt Register (ICCEOIR)
 */
typedef struct {
    UINT32  EoiIntID:               10;     /* [9:0]: The ACKINTID value from the corresponding ICCIAR access */
    UINT32  CpuID:                  3;      /* [12:10]: The CPUID value from the corresponding ICCIAR access */
    UINT32  Reserved:               19;     /* [31:13]: Reserved */
} AMBA_GIC_CPU_EOI_REG_s;

/*
 * GIC CPU Interface: Running Priority Register (ICCRPR)
 */
typedef struct {
    UINT32  Priority:               8;      /* [7:0]: The highest priority interrupt that is active on the CPU interface */
    UINT32  Reserved:               24;     /* [31:8]: Reserved */
} AMBA_GIC_CPU_RUN_PRIORITY_REG_s;

/*
 * GIC CPU Interface: Highest Pending Interrupt Register (ICCHPIR)
 */
typedef struct {
    UINT32  PendIntID:              10;     /* [9:0]: The interrupt ID of the highest priority pending interrupt */
    UINT32  CpuID:                  3;      /* [12:10]: The CPUID of the highest priority pending interrupt */
    UINT32  Reserved:               19;     /* [31:13]: Reserved */
} AMBA_GIC_CPU_HIGHEST_PEND_REG_s;

/*
 * GIC CPU Interface Identification Register (ICCIIDR)
 */
typedef struct {
    UINT32  Implementer:            12;     /* [11:0]:  Implementer Number, ARM = 0x43B */
    UINT32  Revision:               4;      /* [15:12]: Revision number */
    UINT32  Architecture:           4;      /* [19:16]: Architecture version, GICv1 = 0x1 */
    UINT32  ProductID:              12;     /* [31:20]: ProductID, ARM = 0x390 */
} AMBA_GIC_CPU_VENDOR_ID_REG_s;

/*
 * GIC CPU Interface: Highest Pending Interrupt Register (ICCHPIR)
 */
typedef struct {
    UINT32  IntID:                  10;     /* [9:0]: The interrupt ID of the highest priority pending interrupt */
    UINT32  CpuID:                  3;      /* [12:10]: The CPUID of the highest priority pending interrupt */
    UINT32  Reserved:               19;     /* [31:13]: Reserved */
} AMBA_GIC_CPU_DEACTIVATE_INT_REG_s;

/*
 * GIC (Virtual) CPU Interface All Registers
 */
typedef struct {                                                            /* Base      : Description */
    volatile UINT32                             Ctrl;                       /* 0x0000(RW,Secure/Non-secure banked): CPU Interface Control Register (GICC_CTLR/GICV_CTLR) */
    volatile AMBA_GIC_CPU_PRIORITY_MASK_REG_s   PriorityMask;               /* 0x0004(RW): Interrupt Priority Mask Register (GICC_PMR/GICV_PMR) */
    volatile AMBA_GIC_CPU_BINARY_POINT_REG_s    BinaryPoint;                /* 0x0008(RW): Binary Point Register (GICC_BPR/GICV_BPR) */
    volatile UINT32                             IntAck;                     /* 0x000C(RO): Interrupt Acknowledge Register (GICC_IAR/GICV_IAR) */
    volatile UINT32                             EndOfInt;                   /* 0x0010(WO): End of Interrupt Register (GICC_EOIR/GICV_EOIR) */
    volatile AMBA_GIC_CPU_RUN_PRIORITY_REG_s    RunningPriority;            /* 0x0014(RO): Running Priority Register (GICC_RPR/GICV_RPR) */
    volatile AMBA_GIC_CPU_HIGHEST_PEND_REG_s    HighestPendingInt;          /* 0x0018(RO): Highest Pending Interrupt Register (GICC_HPPIR/GICV_HPPIR) */
    volatile AMBA_GIC_CPU_BINARY_POINT_REG_s    AliasedBinaryPoint;         /* 0x001C(RW): Aliased Binary Point Register (GICC_ABPR/GICV_ABPR) */
    volatile UINT32                             AliasedIntAck;              /* 0x0020(RO): Aliased Interrupt Acknowledge Register (GICC_AIAR/GICV_AIAR) */
    volatile UINT32                             AliasedEndOfInt;            /* 0x0024(WO): Aliased End of Interrupt Register (GICC_AEOIR/GICV_AEOIR) */
    volatile AMBA_GIC_CPU_HIGHEST_PEND_REG_s    AliasedHighestPendingInt;   /* 0x0028(RO): Aliased Highest Priority Pending Interrupt Register (GICC_AHPPIR/GICV_AHPPIR) */
    volatile UINT32                             Reserved0[41];              /* 0x002C-0x00CC: Reserved */
    volatile UINT32                             ActivePriority;             /* 0x00D0(RW): Active Priority Register (GICC_APR0/GICV_APR0) */
    volatile UINT32                             Reserved1[3];               /* 0x00D4-0x00DC: Reserved */
    volatile UINT32                             NonSecureActivePriority;    /* 0x00E0(RW): Non-Secure Active Priority Register (GICC_NSAPR0) */
    volatile UINT32                             Reserved2[6];               /* 0x00E4-0x00F8: Reserved */
    volatile AMBA_GIC_CPU_VENDOR_ID_REG_s       VendorID;                   /* 0x00FC(RO): CPU Interface Identification Register (GICC_IIDR/GICV_IIDR) */
    volatile UINT32                             Reserved3[960];             /* 0x0100-0x0FFC: Reserved */
    volatile AMBA_GIC_CPU_DEACTIVATE_INT_REG_s  DeActivateInt;              /* 0x1000(WO): Deactivate Interrupt Register (GICC_DIR/GICV_DIR) */
} AMBA_GIC_CPU_IF_REG_s;

/*
 * GIC Virtual Interface: Hypervisor Control Register (GICH_HCR)
 */
typedef struct {
    UINT32  Eable:                  1;      /* [0]: Global enable bit for the virtual CPU interface, 1: enable */
    UINT32  UnderFlowIntEable:      1;      /* [1]: Underflow Interrupt Enable, 1: Eable interrupt */
    UINT32  LRENPIE:                1;      /* [2]: List Register Entry Not Present Interrupt Enable, 1: A maintenance interrupt is asserted while the EOICount field is not 0. */
    UINT32  NoPendIntEable:         1;      /* [3]: No Pending Interruput Enable, 1:Maintenance interrupt signaled while the List registers contain no interrupts in the pending state. */
    UINT32  VGroup0EIE:             1;      /* [4]: VM Disable Group 1 Interrupt Enable, 1: Maintenance interrupt signaled while GICV_CTLR.EnableGrp0 is set to 1. */
    UINT32  VGroup0DIE:             1;      /* [5]: VM Disable Group 1 Interrupt Enable, 1: Maintenance interrupt signaled while GICV_CTLR.EnableGrp0 is set to 0. */
    UINT32  VGroup1EIE:             1;      /* [6]: VM Disable Group 1 Interrupt Enable, 1: Maintenance interrupt signaled while GICV_CTLR.EnableGrp1 is set to 1. */
    UINT32  VGroup1DIE:             1;      /* [7]: VM Disable Group 1 Interrupt Enable, 1: Maintenance interrupt signaled while GICV_CTLR.EnableGrp1 is set to 0. */
    UINT32  Reserved:               19;     /* [26:8] */
    UINT32  EoiCount:               5;      /* [31:27]: Counts the number of EOIs received that do not have a corresponding entry in the List registers */
} AMBA_GIC_VIR_HYPERVISOR_CTRL_REG_s;

/*
 * GIC Virtual Interface: Virtual GIC Type Register (GICH_VTR)
 */
typedef struct {
    UINT32  ListRegs:               6;      /* [5:0]: Indicate the number of List Registers - 1 */
    UINT32  Reserved:               20;     /* [25:6] */
    UINT32  PreBits:                3;      /* [28:26]: Indicate the number of preemption bits - 1 */
    UINT32  PriBits:                3;      /* [31:29]: Indicate the number of priority bits - 1 */
} AMBA_GIC_VIR_TYPE_INT_REG_s;

/*
 * GIC Virtual Interface: Virtual Machine Control Register (GICH_VMCR)
 */
typedef struct {
    UINT32  Group0Eable:            1;      /* [0]: Alias of GICV_CTLR.EnableGrp0 */
    UINT32  Group1Eable:            1;      /* [1]: Alias of GICV_CTLR.EnableGrp1 */
    UINT32  AckCtrl:                1;      /* [2]: Alias of GICV_CTLR.AckCtl */
    UINT32  FiqEable:               1;      /* [3]: Alias of GICV_CTLR.FIQEn */
    UINT32  CVBR:                   1;      /* [4]: Alias of GICV_CTLR.CBPR */
    UINT32  Reserved0:              4;      /* [8:5] */
    UINT32  EoiMode:                1;      /* [9]: Alias of GICV_CTLR.EOImode */
    UINT32  Reserved1:              8;      /* [17:10] */
    UINT32  AliasedBinaryPoint:     3;      /* [20:18]: Alias of GICV_ABPR.Binary point */
    UINT32  BinaryPoint:            3;      /* [23:21]: Binary Point, Alias of GICV_BPR */
    UINT32  Reserved2:              3;      /* [26:24] */
    UINT32  VMPriMask:              5;      /* [31:27]: Priority Mask, Alias of GICV_PMR.Priority */
} AMBA_GIC_VIR_VIRTUAL_MACHINE_CTRL_REG_s;

/*
 * GIC Virtual Interface: Maintenance Interrupt Status Register (GICH_MISR)
 */
typedef struct {
    UINT32  EOI:                    1;      /* [0]: EOI maintenance interrupt */
    UINT32  UnderFlow:              1;      /* [1]: Underflow interrupt */
    UINT32  LRENP:                  1;      /* [2]: List Register Entry Not Present maintenance interrupt */
    UINT32  NoPendingInt:           1;      /* [3]: No Pending Interruput Enable */
    UINT32  Group0Eable:            1;      /* [4]: Eable Group0 Interruput, 1: Enable */
    UINT32  Group0Disable:          1;      /* [5]: Disable Group0 Interruput, 1: Disable */
    UINT32  Group1Eable:            1;      /* [6]: Eable Group1 Interruput, 1: Enable */
    UINT32  Group1Disable:          1;      /* [7]: Disable Group1 Interruput, 1: Disable */
    UINT32  Reserved:               24;     /* [31:8] */
} AMBA_GIC_VIR_MAINTENANCE_INT_STATUS_REG_s;

/*
 * GIC Virtual Interface: List Register (GICH_LRn)
 */
typedef struct {
    UINT32  VirtualID:              10;     /* [9:0]: This ID is returned to the Guest OS when the interrupt is acknowledged through the VM Interrupt Acknowledge register */
    UINT32  PhysicalID:             10;     /* [19:10]: Depends on the value of the GICH_LR.HW bit */
    /*          GICH_LR.HW == 1, indicates the physical interrupt ID */
    /*          GICH_LR.HW == 1, [19] EOI, [18:13] Reserved, [12:10] CPUID */
    UINT32  Reserved:               3;      /* [22:20] */
    UINT32  Priority:               5;      /* [27:23]: The priority of this interrupt */
    UINT32  State:                  2;      /* [29:28]: The state of the interrupt, 2b'00:invalid, 2b'01:pending, 2b'10:active, 2b'11:pending and active. */
    UINT32  Group1Int:              1;      /* [30]: Indicates whether this virtual interrupt is a Group 1 virtual interrupt */
    UINT32  HwInt:                  1;      /* [31]: Indicates whether this virtual interrupt is a hardware interrupt */
} AMBA_GIC_VIR_LIST_REG_s;

/*
 * GIC Virtual Interface Control All Registers
 */
typedef struct {                                                                    /* Base      : Description */
    volatile AMBA_GIC_VIR_HYPERVISOR_CTRL_REG_s         HypCtrl;                    /* 0x0000(RW): Hypervisor Control Register (GICH_HCR) */
    volatile AMBA_GIC_VIR_TYPE_INT_REG_s                VgicType;                   /* 0x0004(RO): VGIC Type Register (GICH_VTR) */
    volatile AMBA_GIC_VIR_VIRTUAL_MACHINE_CTRL_REG_s    VmCtrl;                     /* 0x0008(RW): Virtual Machine Control Register (GICH_VMCR) */
    volatile UINT32                                     Reserved0;                  /* 0x000C: Reserved */
    volatile AMBA_GIC_VIR_MAINTENANCE_INT_STATUS_REG_s  MaintenanceIntStatus;       /* 0x0010(RO): Maintenance Interrupt Status Register (GICH_MISR) */
    volatile UINT32                                     Reserved1[3];               /* 0x0014-0x001C: Reserved */
    volatile UINT32                                     EndOfIntStatus[2];          /* 0x0020-0x0024(RO): End of Interrupt Registers 0 and 1 (GICH_EISR0,GICH_EISR1) */
    volatile UINT32                                     Reserved2[2];               /* 0x0028-0x002C: Reserved */
    volatile UINT32                                     EmptyListStatus[2];         /* 0x0030-0x0034(RO): Empty List Register Status Registers 0 and 1 (GICH_ELSR0,GICH_ELSR1) */
    volatile UINT32                                     Reserved3[46];              /* 0x0038-0x00EC: Reserved */
    volatile UINT32                                     ActivePriority;             /* 0x00F0(RW): Active Priorities Register (GICH_APR) */
    volatile UINT32                                     Reserved4[3];               /* 0x00F4-0x00FC: Reserved for GICH_APR1-GICH_APR3 */
    volatile AMBA_GIC_VIR_LIST_REG_s                    List[64];                   /* 0x0100-0x01FC(RW): List Registers 0-63 (GICH_LR0-GICH_LR63) */
} AMBA_GIC_VIRTUAL_IF_CTRL_REG_s;

/*
 * Defined in AmbaMmioBase.asm
 */
extern AMBA_GIC_DISTRIBUTOR_REG_s * pAmbaGIC_DistReg;
extern AMBA_GIC_CPU_IF_REG_s      * pAmbaGIC_CpuIfReg;

#endif /* AMBA_REG_GIC_H */
