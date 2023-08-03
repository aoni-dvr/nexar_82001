/**
 *  @file AmbaDebugPort_IDSP.h
 *
 *  @copyright Copyright (c) 2020 Ambarella International LP
 *
 *  This file and its contents ("Software") are protected by intellectual
 *  property rights including, without limitation, U.S. and/or foreign
 *  copyrights. This Software is also the confidential and proprietary
 *  information of Ambarella International LP and its licensors. You may not use, reproduce,
 *  disclose, distribute, modify, or otherwise prepare derivative works of this
 *  Software or any portion thereof except pursuant to a signed license agreement
 *  or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
 *  In the absence of such an agreement, you agree to promptly notify and return
 *  this Software to Ambarella International LP.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 *  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 *  MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 *  IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 *  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details Definitions & Constants for IDSP Cluster
 *
 */

#ifndef AMBA_DEBUG_PORT_IDSP_H
#define AMBA_DEBUG_PORT_IDSP_H

/*
 * IDSP Debug Port: Address Extension
 */
typedef struct {
    UINT32 FilterAddress:            3;      /* [2:0]: Filter Address MSB */
    UINT32 Reserved:                 1;      /* [3] */
    UINT32 FilterNumber:             5;      /* [8:4]: Filter Number */
    UINT32 Reserved1:                3;      /* [11:9] */
    UINT32 SectionNumber:            4;      /* [15:12]: Section Number */
    UINT32 Reserved2:                16;     /* [31:16] */
} AMBA_DBG_PORT_IDSP_ADD_EXT_REG_s;

#define AMBA_DBG_PORT_IDSP_SECTION1     1U   /* Section 1  */
#define AMBA_DBG_PORT_IDSP_SECTION2     2U   /* Section 2  */
#define AMBA_DBG_PORT_IDSP_SECTION3     3U   /* Section 3  */
#define AMBA_DBG_PORT_IDSP_SECTION4     4U   /* Section 4  */
#define AMBA_DBG_PORT_IDSP_SECTION5     5U   /* Section 5  */
#define AMBA_DBG_PORT_IDSP_SECTION6     6U   /* Section 6  */
#define AMBA_DBG_PORT_IDSP_SECTION7     7U   /* Section 7  */
#define AMBA_DBG_PORT_IDSP_SECTION8     8U   /* Section 8  */
#define AMBA_DBG_PORT_IDSP_SECTION9     9U   /* Section 9  */
#define AMBA_DBG_PORT_IDSP_SECTION10    10U  /* Section 10 */
#define AMBA_DBG_PORT_IDSP_SECTION11    11U  /* Section 11 */
#define AMBA_DBG_PORT_IDSP_SECTION12    12U  /* Section 12 */
#define AMBA_DBG_PORT_IDSP_SECTION13    13U  /* Section 13 */
#define AMBA_DBG_PORT_IDSP_SECTION14    14U  /* Section 14 */
#define AMBA_DBG_PORT_IDSP_SECTION15    15U  /* Section 15 */
#define AMBA_DBG_PORT_IDSP_SECTION16    16U  /* Section 16 */
#define AMBA_DBG_PORT_IDSP_SECTION17    17U  /* Section 17 */
#define AMBA_DBG_PORT_IDSP_SECTION18    18U  /* Section 18 */
#define AMBA_DBG_PORT_IDSP_SECTION19    19U  /* Section 19 */

#define AMBA_DBG_PORT_IDSP_VIN0         (AMBA_DBG_PORT_IDSP_SECTION1)
#define AMBA_DBG_PORT_IDSP_VIN1         (AMBA_DBG_PORT_IDSP_SECTION10)
#define AMBA_DBG_PORT_IDSP_VIN2         (AMBA_DBG_PORT_IDSP_SECTION11)
#define AMBA_DBG_PORT_IDSP_VIN3         (AMBA_DBG_PORT_IDSP_SECTION12)
#define AMBA_DBG_PORT_IDSP_VIN4         (AMBA_DBG_PORT_IDSP_SECTION13)
#define AMBA_DBG_PORT_IDSP_VIN8         (AMBA_DBG_PORT_IDSP_SECTION19)

/*
 * IDSP Debug Port: Config Controller Internal Debug State
 */
typedef struct {
    UINT32 Status:                  14;     /* [13:0]: debug status from config_datagen */
    UINT32 Reserved:                18;     /* [31:14] */
} AMBA_DBG_PORT_IDSP_CFG_DATA_STATUS_REG_s;

/*
 * IDSP Debug Port: Section Reset Status
 */
typedef struct {
    UINT32 Status:                  31;     /* [30:0]: section reset status */
    UINT32 Reserved:                1;      /* [31] */
} AMBA_DBG_PORT_IDSP_SEC_RESET_STATUS_REG_s;

/*
 * IDSP Debug Port: Internal Error Status
 */
typedef struct {
    UINT32 Reserved:                1;      /* [0] */
    UINT32 CommandOverflow:         19;     /* [19:1]: Command overflow status for sections */
    UINT32 Reserved1:               12;     /* [31:20] */
} AMBA_DBG_PORT_IDSP_SECTION_ERR_STATUS_REG_s;

/*
 * IDSP Debug Port: Section command count
 */
typedef struct {
    UINT32 CommandDoneCnt:          8;      /* [7:0] */
    UINT32 CommandRcvdCnt:          8;      /* [15:8] */
    UINT32 ResetCommandCnt:         8;      /* [23:16] */
    UINT32 TestCommandDoneCnt:      8;      /* [31:24] */
} AMBA_DBG_PORT_IDSP_SECTION_COMMAND_CNT_REG_s;

/*
 * IDSP Debug Port: Top Error Mask Register can be used to mask-out various errors in IDSP top/controller
 */
typedef struct {
    UINT32 CommandBuf:              1;      /* [0] Main controller command buffer parity error */
    UINT32 IdspClkDomainLockstep:   1;      /* [1] Main controller lockstep error (IDSP clock domain) */
    UINT32 CoreClkDomainLockstep:   1;      /* [2] Main controller lockstep error (Core clock domain) */
    UINT32 InputInterfaceParity:    1;      /* [3] Main controller input interface parity error */
    UINT32 OutputInterfaceParity:   1;      /* [4] Main controller output interface parity error */
    UINT32 DebugInterfaceParity:    1;      /* [5] Main controller debug interface parity error */
    UINT32 SynchronizerErr:         1;      /* [6] Main controller synchronizer errors */
    UINT32 Reserved:                1;      /* [7] */
    UINT32 Ctl0Interface:           1;      /* [8] Cluster-0 sub-controller interface error */
    UINT32 Ctl0Lockstep:            1;      /* [9] Cluster-0 sub-controller lockstep error */
    UINT32 Ctl0SplitterMux:         1;      /* [10] Cluster-0 splitter/mux error */
    UINT32 Reserved1:               5;      /* [15:11] */
    UINT32 Ctl1Interface:           1;      /* [16] Cluster-1 sub-controller interface error */
    UINT32 Ctl1Lockstep:            1;      /* [17] Cluster-1 sub-controller lockstep error */
    UINT32 Ctl1SplitterMux:         1;      /* [18] Cluster-1 splitter/mux error */
    UINT32 Reserved2:               13;     /* [31:19] */
} AMBA_DBG_PORT_IDSP_TOP_ERROR_MASK_REG_s;

/*
 * IDSP Debug Port: Error status register for sub-controller in IDSP-0 cluster
 */
typedef struct {
    UINT32 InterfaceErr:            1;      /* [0] Interface error on sub-controller-0 */
    UINT32 LockStepErr:             1;      /* [1] Lockstep error */
    UINT32 SplitterMuxErr:          1;      /* [2] Splitter mux error in IDSP-0 */
    UINT32 Reserved:                2;      /* [4:3] */
    UINT32 SmemInterfaceErr:        1;      /* [5] smem-interface safety error */
    UINT32 Reserved1:               26;     /* [31:6] */
} AMBA_DBG_PORT_IDSP_CTL0_ERROR_STATUS_REG_s;

/*
 * IDSP Debug Port: Error status register for sub-controller in IDSP-1 cluster
 */
typedef struct {
    UINT32 InterfaceErr:            1;      /* [0] Interface error on sub-controller-1 */
    UINT32 LockStepErr:             1;      /* [1] Lockstep error */
    UINT32 SplitterMuxErr:          1;      /* [2] Splitter mux error in IDSP-1 */
    UINT32 Reserved:                3;      /* [5:3] */
    UINT32 SmemTileInterfaceErr:    1;      /* [6] smem-tile-interface safety error */
    UINT32 SmemConfigInterfaceErr:  1;      /* [7] smem-config-interface safety error */
    UINT32 Reserved1:               24;     /* [31:8] */
} AMBA_DBG_PORT_IDSP_CTL1_ERROR_STATUS_REG_s;

/*
 * IDSP Debug Port: Error status register for main controller / IDSP top. Can be masked by IDSP Top Error Mask register
 */
typedef struct {
    UINT32 CommandBuf:              1;      /* [0] Main controller command buffer parity error */
    UINT32 IdspClkDomainLockstep:   1;      /* [1] Main controller lockstep error (IDSP clock domain) */
    UINT32 CoreClkDomainLockstep:   1;      /* [2] Main controller lockstep error (Core clock domain) */
    UINT32 InputInterfaceParity:    1;      /* [3] Main controller input interface parity error */
    UINT32 OutputInterfaceParity:   1;      /* [4] Main controller output interface parity error */
    UINT32 DebugInterfaceParity:    1;      /* [5] Main controller debug interface parity error */
    UINT32 SynchronizerErr:         1;      /* [6] Main controller synchronizer errors */
    UINT32 ErrInsert:               1;      /* [7] Error insert register */
    UINT32 CtlErr:                  1;      /* [8] controller error */
    UINT32 Idsp0Err:                1;      /* [9] idsp0 error */
    UINT32 Idsp1Err:                1;      /* [10] Idsp1 error */
    UINT32 Reserved:                21;     /* [31:11] */
} AMBA_DBG_PORT_IDSP_ERROR_STATUS_REG_s;

/*
 * IDSP Debug Port: Interrupt Mask register
 */
typedef struct {
    UINT32 TimeoutToArm:            1;      /* [0] Mask Controller's timeout errors to ARM */
    UINT32 TimeoutToCehu:           1;      /* [1] Mask Controller's timeout errors to CEHU */
    UINT32 VinSafetyErrToArm:       1;      /* [2] Mask VIN safety errors to ARM */
    UINT32 VinSafteyErrToCehu:      1;      /* [3] Mask VIN safety errors to CEHU */
    UINT32 Reserved:                28;     /* [31:4] */
} AMBA_DBG_PORT_IDSP_INTR_MASK_REG_s;

/*
 * IDSP Debug Port: Section Reset Register
 */
typedef struct {
    UINT32 ResetVin0SyncOut:        1;      /* [0]: Reset VIN0 Master SYNC Generation Logic */
    UINT32 CorrespondSection:       19;     /* [19:1]: Corresponding to Section (19 - 1) */
    UINT32 Reserved:                4;      /* [23:20] */
    UINT32 ResetMipiPhy:            6;      /* [29:24]: Reset VIN MIPI PHY (Level) */
    UINT32 Reserved1:               1;      /* [30] */
    UINT32 ResetVin1SyncOut:        1;      /* [31]: Reset VIN1 Master SYNC Generation Logic */
} AMBA_DBG_PORT_IDSP_SEC_RESET_REG_s;

/*
 * IDSP Debug Port All Registers
 */
typedef struct {
    volatile UINT32                                       AddressExtension;       /* 0x8000: Address Extension Register */
    volatile AMBA_DBG_PORT_IDSP_CFG_DATA_STATUS_REG_s     ConfigDataStatus;       /* 0x8004: Config Controller Internal Debug State */
    volatile UINT32                                       Reserved[4];            /* 0x8008 - 0x8014 */
    volatile AMBA_DBG_PORT_IDSP_SEC_RESET_STATUS_REG_s    SectionResetStatus;     /* 0x8018: Section Reset Status */
    volatile UINT32                                       Reserved1;              /* 0x801C */
    volatile AMBA_DBG_PORT_IDSP_SECTION_ERR_STATUS_REG_s  SectionErrorStatus;     /* 0x8020: Internal Error Status */
    volatile UINT32                                       Reserved2;              /* 0x8024 */
    volatile UINT32                                       CmdDbgStatus0;          /* 0x8028: Command register debug status-0 */
    volatile UINT32                                       CmdDbgStatus1;          /* 0x802C: Command register debug status-1 */
    volatile UINT32                                       CmdDbgStatus2;          /* 0x8030: Command register debug status-2 */
    volatile UINT32                                       CmdDbgStatus3;          /* 0x8034: Command register debug status-3 */
    volatile UINT32                                       CmdDbgStatus4;          /* 0x8038: Command register debug status-4 */
    volatile UINT32                                       CmdDbgStatus5;          /* 0x803C: Command register debug status-5 */
    volatile UINT32                                       CmdDbgStatus6;          /* 0x8040: Command register debug status-6 */
    volatile UINT32                                       CmdDbgStatus7;          /* 0x8044: Command register debug status-7 */
    volatile UINT32                                       IdspTimeoutStatus;      /* 0x8048: bit[0]: ORC cmd timeout */
    volatile UINT32                                       CtlVinIntrLog;          /* 0x804C: bit[5:0]: VIN0-8 stat error interrupts */
    volatile UINT32                                       Reserved3;              /* 0x8050 */
    volatile UINT32                                       ClearSmErrDoneStatus;   /* 0x8054: bit[19:1]: Section19-1 clear SM error done status */
    volatile UINT32                                       Reserved4[10];          /* 0x8058 - 0x807C */
    volatile AMBA_DBG_PORT_IDSP_SECTION_COMMAND_CNT_REG_s Section1CmdCnt;         /* 0x8080: Section-1 command count */
    volatile AMBA_DBG_PORT_IDSP_SECTION_COMMAND_CNT_REG_s Section2CmdCnt;         /* 0x8084: Section-2 command count */
    volatile AMBA_DBG_PORT_IDSP_SECTION_COMMAND_CNT_REG_s Section3CmdCnt;         /* 0x8088: Section-3 command count */
    volatile AMBA_DBG_PORT_IDSP_SECTION_COMMAND_CNT_REG_s Section4CmdCnt;         /* 0x808C: Section-4 command count */
    volatile AMBA_DBG_PORT_IDSP_SECTION_COMMAND_CNT_REG_s Section5CmdCnt;         /* 0x8090: Section-5 command count */
    volatile AMBA_DBG_PORT_IDSP_SECTION_COMMAND_CNT_REG_s Section6CmdCnt;         /* 0x8094: Section-6 command count */
    volatile AMBA_DBG_PORT_IDSP_SECTION_COMMAND_CNT_REG_s Section7CmdCnt;         /* 0x8098: Section-7 command count */
    volatile AMBA_DBG_PORT_IDSP_SECTION_COMMAND_CNT_REG_s Section8CmdCnt;         /* 0x809C: Section-8 command count */
    volatile AMBA_DBG_PORT_IDSP_SECTION_COMMAND_CNT_REG_s Section9CmdCnt;         /* 0x80A0: Section-9 command count */
    volatile AMBA_DBG_PORT_IDSP_SECTION_COMMAND_CNT_REG_s Section10CmdCnt;        /* 0x80A4: Section-10 command count */
    volatile AMBA_DBG_PORT_IDSP_SECTION_COMMAND_CNT_REG_s Section11CmdCnt;        /* 0x80A8: Section-11 command count */
    volatile AMBA_DBG_PORT_IDSP_SECTION_COMMAND_CNT_REG_s Section12CmdCnt;        /* 0x80AC: Section-12 command count */
    volatile AMBA_DBG_PORT_IDSP_SECTION_COMMAND_CNT_REG_s Section13CmdCnt;        /* 0x80B0: Section-13 command count */
    volatile UINT32                                       Reserved5[3];           /* 0x80B4 - 0x80BC */
    volatile AMBA_DBG_PORT_IDSP_SECTION_COMMAND_CNT_REG_s Section17CmdCnt;        /* 0x80C0: Section-17 command count */
    volatile AMBA_DBG_PORT_IDSP_SECTION_COMMAND_CNT_REG_s Section18CmdCnt;        /* 0x80C4: Section-18 command count */
    volatile AMBA_DBG_PORT_IDSP_SECTION_COMMAND_CNT_REG_s Section19CmdCnt;        /* 0x80C8: Section-19 command count */
    volatile UINT32                                       Reserved6[13];          /* 0x80CC - 0x80FC */
    volatile UINT32                                       CmdFifoDbgStatus[5];    /* 0x8100 - 0x8110: Command FIFO debug status for Section 1-19 */
    volatile UINT32                                       Reserved7[27];          /* 0x8114 - 0x817C */
    volatile UINT32                                       SectionReset;           /* 0x8180: Section Reset Register */
    volatile UINT32                                       Reserved8[7839];        /* 0x8184 - 0xFBFC */
    volatile AMBA_DBG_PORT_IDSP_TOP_ERROR_MASK_REG_s      TopErrMask;             /* 0xFC00: Top Err Mask Register */
    volatile UINT32                                       Section1ErrMask;        /* 0xFC04: Section-1 Error Mask Register */
    volatile UINT32                                       Section2ErrMask;        /* 0xFC08: Section-2 Error Mask Register */
    volatile UINT32                                       Section3ErrMask;        /* 0xFC0C: Section-3 Error Mask Register */
    volatile UINT32                                       Section4ErrMask;        /* 0xFC10: Section-4 Error Mask Register */
    volatile UINT32                                       Section5ErrMask;        /* 0xFC14: Section-5 Error Mask Register */
    volatile UINT32                                       Section6ErrMask;        /* 0xFC18: Section-6 Error Mask Register */
    volatile UINT32                                       Section7ErrMask;        /* 0xFC1C: Section-7 Error Mask Register */
    volatile UINT32                                       Section8ErrMask;        /* 0xFC20: Section-8 Error Mask Register */
    volatile UINT32                                       Section9ErrMask;        /* 0xFC24: Section-9 Error Mask Register */
    volatile UINT32                                       Section10ErrMask;       /* 0xFC28: Section-10 Error Mask Register */
    volatile UINT32                                       Section11ErrMask;       /* 0xFC2C: Section-11 Error Mask Register */
    volatile UINT32                                       Section12ErrMask;       /* 0xFC30: Section-12 Error Mask Register */
    volatile UINT32                                       Section13ErrMask;       /* 0xFC34: Section-13 Error Mask Register */
    volatile UINT32                                       Section14ErrMask;       /* 0xFC38: Section-14 Error Mask Register */
    volatile UINT32                                       Section15ErrMask;       /* 0xFC3C: Section-15 Error Mask Register */
    volatile UINT32                                       Section16ErrMask;       /* 0xFC40: Section-16 Error Mask Register */
    volatile UINT32                                       Section17ErrMask;       /* 0xFC44: Section-17 Error Mask Register */
    volatile UINT32                                       Section18ErrMask;       /* 0xFC48: Section-18 Error Mask Register */
    volatile UINT32                                       Section19ErrMask;       /* 0xFC4C: Section-19 Error Mask Register */
    volatile UINT32                                       Reserved9[12];          /* 0xFC50 - 0xFC7C */
    volatile AMBA_DBG_PORT_IDSP_CTL0_ERROR_STATUS_REG_s   Ctl0ErrStatus;          /* 0xFC80: Error Status Register for sub-controller in IDSP-0 cluster */
    volatile AMBA_DBG_PORT_IDSP_CTL1_ERROR_STATUS_REG_s   Ctl1ErrStatus;          /* 0xFC84: Error Status Register for sub-controller in IDSP-1 cluster */
    volatile UINT32                                       Reserved10[6];          /* 0xFC88 - 0xFC9C */
    volatile AMBA_DBG_PORT_IDSP_ERROR_STATUS_REG_s        IdspErrStatus;          /* 0xFCA0: Error Status for main controller / IDSP top */
    volatile UINT32                                       Reserved11[7];          /* 0xFCA4 - 0xFCBC */
    volatile UINT32                                       Section1ErrStatus;      /* 0xFCC0: Section-1 Error Status Register */
    volatile UINT32                                       Section2ErrStatus;      /* 0xFCC4: Section-2 Error Status Register */
    volatile UINT32                                       Section3ErrStatus;      /* 0xFCC8: Section-3 Error Status Register */
    volatile UINT32                                       Section4ErrStatus;      /* 0xFCCC: Section-4 Error Status Register */
    volatile UINT32                                       Section5ErrStatus;      /* 0xFCD0: Section-5 Error Status Register */
    volatile UINT32                                       Section6ErrStatus;      /* 0xFCD4: Section-6 Error Status Register */
    volatile UINT32                                       Section7ErrStatus;      /* 0xFCD8: Section-7 Error Status Register */
    volatile UINT32                                       Section8ErrStatus;      /* 0xFCDC: Section-8 Error Status Register */
    volatile UINT32                                       Section9ErrStatus;      /* 0xFCE0: Section-9 Error Status Register */
    volatile UINT32                                       Section10ErrStatus;     /* 0xFCE4: Section-10 Error Status Register */
    volatile UINT32                                       Section11ErrStatus;     /* 0xFCE8: Section-11 Error Status Register */
    volatile UINT32                                       Section12ErrStatus;     /* 0xFCEC: Section-12 Error Status Register */
    volatile UINT32                                       Section13ErrStatus;     /* 0xFCF0: Section-13 Error Status Register */
    volatile UINT32                                       Section14ErrStatus;     /* 0xFCF4: Section-14 Error Status Register */
    volatile UINT32                                       Section15ErrStatus;     /* 0xFCF8: Section-15 Error Status Register */
    volatile UINT32                                       Section16ErrStatus;     /* 0xFCFC: Section-16 Error Status Register */
    volatile UINT32                                       Section17ErrStatus;     /* 0xFD00: Section-17 Error Status Register */
    volatile UINT32                                       Section18ErrStatus;     /* 0xFD04: Section-18 Error Status Register */
    volatile UINT32                                       Section19ErrStatus;     /* 0xFD08: Section-19 Error Status Register */
    volatile UINT32                                       Reserved12[173];        /* 0xFD0C - 0xFFBC */
    volatile UINT32                                       IdspClkDomainDummyReset;/* 0xFFC0: resettable dummy register for IDSP domain */
    volatile UINT32                                       CoreClkDomainDummyReset;/* 0xFFC4: resettable dummy register for CORE domain */
    volatile UINT32                                       Reserved13;             /* 0xFFC8 */
    volatile UINT32                                       SectionClearSMError;    /* 0xFFCC: Bit[0]: Clears safety error for controller/smem interface blocks and top level mux/splitters, Bit[19:1] Clears safety error for sections 19-1 */
    volatile AMBA_DBG_PORT_IDSP_INTR_MASK_REG_s           IntrMask;               /* 0xFFD0:  */
    volatile UINT32                                       SectionWatchdogTimeout; /* 0xFFD4: Watchdog programming for section execution timeout */
    volatile UINT32                                       ORCWatchdogTimeout;     /* 0xFFD8: Watchdog programming for ORC command timeout */
    volatile UINT32                                       Reserved14[3];          /* 0xFFDC - 0xFFE4 */
    volatile UINT32                                       Idsp1DebugClkMonCounter;/* 0xFFE8: Current counter value for Debug clock monitor in IDSP-1 cluster */
    volatile UINT32                                       Idsp1CoreClkMonCounter; /* 0xFFEC: Current counter value for Core clock monitor in IDSP-1 cluster */
    volatile UINT32                                       Idsp1IdspClkMonCounter; /* 0xFFF0: Current counter value for Idsp clock monitor in IDSP-1 cluster */
    volatile UINT32                                       Idsp0DebugClkMonCounter;/* 0xFFF4: Current counter value for Debug clock monitor in IDSP-0 cluster */
    volatile UINT32                                       Idsp0CoreClkMonCounter; /* 0xFFF8: Current counter value for Core clock monitor in IDSP-0 cluster */
    volatile UINT32                                       Idsp0IdspClkMonCounter; /* 0xFFFC: Current counter value for Idsp clock monitor in IDSP-0 cluster */
} AMBA_DBG_PORT_IDSP_CONTROLLER_REG_s;

typedef struct {
    UINT32  SwReset:                1;      /* [0] Reset core VIN logic when config is loaded */
    UINT32  Enable:                 1;      /* [1] Enable Sensor Interface logic */
    UINT32  OutputEnable:           1;      /* [2] Enable output of VIN */
    UINT32  BitsPerPixel:           3;      /* [5:3] Sensor data width; 0=8-bit; 1=10-bit; 2=12-bit; 3=14-bit; 4=16-bit */
    UINT32  Reserved:               26;     /* [31:6] Reserved */
} AMBA_DBG_PORT_IDSP_CTRL0_REG_s;

typedef struct {
    UINT32  DataLaneEnable:         16;     /* [15:0] 1 = Enable SLVS/MIPI Physical Data Lane. Set bits 15:12 as 0 */
    UINT32  Reserved:               16;     /* [31:16] Reserved */
} AMBA_DBG_PORT_IDSP_CTRL1_REG_s;

typedef struct {
    UINT32  PadType:                1;      /* [0] Pad type: 0=LVCMOS; 1=LVDS */
    UINT32  ParallelDataRate:       1;      /* [1] Data rate: 0=SDR; 1=DDR */
    UINT32  ParallelWidth:          1;      /* [2] 0=1-Pixel wide input; 1=2-Pixel wide input */
    UINT32  ParallelDataEdge:       1;      /* [3] Data valid on 0=posedge; 1=negedge */
    UINT32  SyncType:               1;      /* [4] Sync type: 0=Embedded/BT.656 Sync; 1=External/BT.601 Sync */
    UINT32  ParallelEmbSyncLoc:     2;      /* [6:5] For 2-pix width input, sync is on: 0=lower pix; 1=upper pix; 2/3=Interleaved across both */
    UINT32  YuvEnable:              1;      /* [7] Input type is: 0=RGB; 1=YUV */
    UINT32  YuvOrder:               2;      /* [9:8] Reorder pixels {3,2,1,0} --> 0={3,2,1,0} (Passthrough); 1={0,3,2,1}; 2={2,3,0,1}; 3={1,2,3,0} */
    UINT32  VsyncPolarity:          1;      /* [10] External/601 VSYNC is 0=Active High; 1=Active Low */
    UINT32  HsyncPolarity:          1;      /* [11] External/601 HSYNC is 0=Active High; 1=Active Low */
    UINT32  FieldPolarity:          1;      /* [12] External/601 FIELD is 0=Non-inverted; 1=Inverted */
    UINT32  FieldMode:              1;      /* [13] 0=Field sampled at active edge of vsync; 1=Field sampled continuously */
    UINT32  DisableAfterSync:       1;      /* [14] Deactivate VIN frontend after vsync (delayed vsync keeps counting) */
    UINT32  DoubleBufferEnable:     1;      /* [15] Enable double-buffering on config data */
    UINT32  Reserved:               16;     /* [31:16] Reserved */
} AMBA_DBG_PORT_IDSP_DVP_CTRL_REG_s;

typedef struct {
    UINT32  FieldPinSelect:         5;      /* [4:0] Typically set to 2. Uses pin SPCLK_P_1. To use a different pin, consult the Ambarella support team */
    UINT32  VsyncPinSelect:         5;      /* [9:5] Typically set to 1. Uses pin SPCLK_N_1. To use a different pin, consult the Ambarella support team */
    UINT32  HsyncPinSelect:         5;      /* [14:10] Typically set to 0. Uses pin SPCLK_N_0. To use a different pin, consult the Ambarella support team */
    UINT32  Reserved:               17;     /* [31:15] */
} AMBA_DBG_PORT_IDSP_SYNC_PIN_MUX_REG_s;

typedef struct {
    UINT32  DataLane0_4_8PinSelect:  4;     /* [3:0] pad_p[x] and pad_n[x] */
    UINT32  DataLane1_5_9PinSelect:  4;     /* [7:4] pad_p[x] and pad_n[x] */
    UINT32  DataLane2_6_10PinSelect: 4;     /* [11:8] pad_p[x] and pad_n[x] */
    UINT32  DataLane3_7_11PinSelect: 4;     /* [15:12] pad_p[x] and pad_n[x] */
    UINT32  Reserved:                16;    /* [31:16] Reserved */
} AMBA_DBG_PORT_IDSP_DATA_PIN_MUX_REG_s;

typedef struct {
    UINT32  DataLane12_PinSelect:    4;     /* [3:0] Not used. Set to default reset value */
    UINT32  DataLane13_PinSelect:    4;     /* [7:4] Not used. Set to default reset value */
    UINT32  DataLane14_PinSelect:    4;     /* [11:8] Not used. Set to default reset value */
    UINT32  DataLane15_PinSelect:    4;     /* [15:12] Not used. Set to default reset value */
    UINT32  Reserved:                16;    /* [31:16] Reserved */
} AMBA_DBG_PORT_IDSP_RESERVED_DATA_PIN_MUX_REG_s;

typedef struct {
    UINT32  DisableCropRegion:      1;      /* [0] Disable crop region; if it is 1, ignore the crop_start/end_col/row */
    UINT32  DbgNonBlocking:         1;      /* [1] Only set to 1 when bring up VIN to check sof/eof interrupts */
    UINT32  DbgEolHold:             1;      /* [2] reserved bit, only set to 1 when requested by designer */
    UINT32  MipiMaxSkewCycle:       3;      /* [5:3] Only set it to non-zero value in 8-lane MIPI sensor; its unit is mipi_byteclk cycle; or 4*mipi_sensor_clk cycle */
    UINT32  Reserved:               26;     /* [31:6] */
} AMBA_DBG_PORT_IDSP_CTRL2_REG_s;

typedef struct {
    UINT32  SplitWidth:             16;     /* [15:0]: readouts that pack multiple exposure lines into a single readout line (0=disabled) */
    UINT32  Reserved:               16;     /* [31:16] Reserved */
} AMBA_DBG_PORT_IDSP_SPLIT_CTRL_REG_s;

typedef struct {
    UINT32  SyncInterleaving:       2;      /* [1:0] 0=none; 1=2-lane interleaving; 2=4-lane interleaving (For HiSPi Streaming-S) */
    UINT32  SyncTolerance:          2;      /* [3:2] 0=No error tolerance; 1=Allow a corrupted sync every other code; 2=Always allow corruption */
    UINT32  SyncAllLanes:           1;      /* [4] 1=Sync code must be detected on all active lanes in each group; 0=Majority of active lanes */
    UINT32  Reserved:               27;     /* [31:5] */
} AMBA_DBG_PORT_IDSP_SLVS_SYNC_LOCATION_REG_s;

typedef struct {
    UINT32  AllowPartialCode:       1;      /* [0] 1=Allow one of the all-0/all-1 words to be corrupted and still be a valid sync */
    UINT32  LockSyncPhase:          1;      /* [1] 0=Constantly re-align phase; 1=Lock/Coast sync phase to first detected code */
    UINT32  SyncCorrection:         1;      /* [2] Enable correction of sync code curruption by examining the codes across muliple lanes */
    UINT32  DeskewEnable:           1;      /* [3] 0=disable/bypass deskew; 1=enable */
    UINT32  SyncType:               1;      /* [4] 0=ITU-656; 1=Custom (field status bit will only toggle in ITU-656) */
    UINT32  Enable656Ecc:           1;      /* [5] Enable ITU-656 sync error correction/detection */
    UINT32  PatMaskAlignment:       1;      /* [6] Sync code patterns/masks are: 0=LSB aligned; 1=MSB aligned */
    UINT32  UnlockOnTimeout:        1;      /* [7] Force sync unlock on sync timeout */
    UINT32  UnlockOnDeskewError:    1;      /* [8] Force sync unlock on deskew error */
    UINT32  LineReorder:            1;      /* [9] Enable line reordering for SSLL --> SLSL conversion */
    UINT32  Reserved:               22;     /* [31:10] */
} AMBA_DBG_PORT_IDSP_SLVS_CTRL_REG_s;

typedef struct {
    UINT32  DetectSol:              1;      /* [0] Enable Detection of SOL sync codes */
    UINT32  DetectEol:              1;      /* [1] Enable Detection of EOL sync codes */
    UINT32  DetectSof:              1;      /* [2] Enable Detection of SOF sync codes */
    UINT32  DetectEof:              1;      /* [3] Enable Detection of EOF sync codes */
    UINT32  DetectSov:              1;      /* [4] Enable Detection of SOV sync codes */
    UINT32  DetectEov:              1;      /* [5] Enable Detection of EOV sync codes */
    UINT32  Reserved:               26;     /* [31:6] */
} AMBA_DBG_PORT_IDSP_SLVS_SYNC_DETECT_REG_s;

typedef struct {
    UINT32  VirtChanMask:           2;      /* [1:0] 2-bit value to mask the corresponding virtual channel ID bit. Set it to 0x3 to capture all virtual channels */
    UINT32  VirtChanPattern:        2;      /* [3:2] 2-bit value to indicate the pattern of the virtual channel ID a VIN is supposed to capture */
    UINT32  DataTypeMask:           6;      /* [9:4] 6-bit value to mask the corresponding data type ID bit. Set it to 0x3f to capture all data types */
    UINT32  DataTypePattern:        6;      /* [15:10] 6-bit value to indicate the pattern of the data type ID a VIN is supposed to capture */
    UINT32  Reserved:               16;     /* [31:16] Reserved */
} AMBA_DBG_PORT_IDSP_MIPI_CTRL0_REG_s;

typedef struct {
    UINT32  EDDataTypeMask:         6;      /* [5:0] 6-bit value to mask the corresponding embedded data ID bit. Set it to 0x3f to capture all data */
    UINT32  EDDataTypePattern:      6;      /* [11:6] 6-bit value to indicate the pattern of the embedded data ID a VIN is supposed to capture */
    UINT32  ByteSwapEnable:         1;      /* [12] 1 = Reverse order of pixels */
    UINT32  EccEnable:              1;      /* [13] 1 = Enable packet header ECC */
    UINT32  ForwardEccEnable:       1;      /* [14] 1 = Enable errored packets forwarding (default mode) */
    UINT32  MipiEDEnable:           1;      /* [15] 1 = Enable Embedded data capturing */
    UINT32  Reserved:               16;     /* [31:16] Reserved */
} AMBA_DBG_PORT_IDSP_MIPI_CTRL1_REG_s;

typedef struct {
    UINT32  Mode:                   2;      /* [1:0] 0=Disabled; 1=Even Fields; 2=Odd Fields; 3=All Fields */
    UINT32  Line:                   14;     /* [15:2] VOUT sync0 is asserted for the duration of the specified active line */
    UINT32  Reserved:               16;     /* [31:16] Reserved */
} AMBA_DBG_PORT_IDSP_VOUT_SYNC_REG_s;

typedef struct {
    UINT32  Enable:                 1;      /* [0] Enable GPIO strig */
    UINT32  Polarity:               1;      /* [1] 0=Active low; 1=Active high */
    UINT32  StartLine:              14;     /* [15:2] Active line number to start asserting strig */
    UINT32  Reserved:               16;     /* [31:16] Reserved */
} AMBA_DBG_PORT_IDSP_STRIG_CTRL_REG_s;

typedef struct {
    UINT32  Sign:                   1;      /* [0] sign bit of black level offset */
    UINT32  Reserved:               31;     /* [31:1] Reserved */
} AMBA_DBG_PORT_IDSP_BLACKLEVEL_CTRL_REG_s;

typedef struct {
    UINT32  WatchdogTimeout:        1;      /* [0] when one frame takes long than programmed watchdog threshold value cycles to finish */
    UINT32  ShortFrame:             1;      /* [1] */
    UINT32  ShortLine:              1;      /* [2] */
    UINT32  SfifoOverFlow:          1;      /* [3] */
    UINT32  AfifoOverflow:          1;      /* [4] */
    UINT32  LostLockAfterSof:       1;      /* [5] */
    UINT32  UnknownSyncCode:        1;      /* [6] */
    UINT32  Uncorrectable656Error:  1;      /* [7] */
    UINT32  SerialDeskewError:      1;      /* [8] */
    UINT32  FrameSyncError:         1;      /* [9] Wrong frame sync packet sequence in MIPI stream */
    UINT32  CrcError:               1;      /* [10] When detected CRC error number is larger than crc_error_threshold */
    UINT32  EccError1Bit:           1;      /* [11] When detected 1bit ECC error number is larger than ecc_corr_err_threshold */
    UINT32  EccError2Bit:           1;      /* [12] */
    UINT32  Reserved:               19;     /* [31:13] Reserved */
} AMBA_DBG_PORT_IDSP_ERR_MASK_REG_s;

typedef struct {
    UINT32  GotActSof:              1;      /* [0] Detected start of active region (write 1 to clear) */
    UINT32  GotWinSof:              1;      /* [1] Reached start of crop region (write 1 to clear) */
    UINT32  GotWinEof:              1;      /* [2] Reached end of crop region (write 1 to clear) */
    UINT32  GotVsync:               1;      /* [3] Reached end of active region (start of VBLANK) (write 1 to clear) */
    UINT32  AfifoOverflow:          1;      /* [4] AFIFO overflow (must reset VIN) */
    UINT32  SfifoOverFlow:          1;      /* [5] SFIFO has overflowed (must reset VIN) */
    UINT32  ShortLine:              1;      /* [6] EOL received before reaching end of programmed crop width (write 1 to clear) */
    UINT32  ShortFrame:             1;      /* [7] EOF received before reaching end of programmed crop region (write 1 to clear) */
    UINT32  Field:                  1;      /* [8] Field bit value */
    UINT32  Reserved:               1;      /* [9] Not used */
    UINT32  SentMasterVsync:        1;      /* [10] Sent master vsync (from master sync generator unit) (write 1 to clear) */
    UINT32  Uncorrectable656Error:  1;      /* [11] ITU-656 sync code was corrupted and uncorrectable (write 1 to clear) */
    UINT32  SyncLocked:             1;      /* [12] VIN front-end sync is locked */
    UINT32  LostLockAfterSof:       1;      /* [13] Lost lock after detecting start of frame (must reset VIN) */
    UINT32  PartialSyncDetected:    1;      /* [14] Partial/corrupted sync was detected (write 1 to clear) */
    UINT32  UnknownSyncCode:        1;      /* [15] Sync code did not match any patterns (write 1 to clear) */
    UINT32  Reserved1:              16;     /* [31:16] Reserved */
} AMBA_DBG_PORT_IDSP_STATUS_REG_s;

typedef struct {
    UINT32  SyncState:              2;      /* [1:0] State of front-end sync FSM (0=IDLE; 1=ACTIVE; 2=HBLANK; 3=VBLANK) */
    UINT32  SerialDeskewError:      1;      /* [2] */
    UINT32  WatchdogTimeout:        1;      /* [3] */
    UINT32  SfifoCount:             11;     /* [14:4] Number of entries in SFIFO/Buffer */
    UINT32  Reserved:               17;     /* [31:15] */
} AMBA_DBG_PORT_IDSP_SLVS_STATUS_REG_s;

typedef struct {
    UINT32  EccError2Bit:           1;      /* [0] 2-bit uncorrectable ECC error */
    UINT32  EccError1Bit:           1;      /* [1] 1-bit ECC error corrected */
    UINT32  CrcError:               1;      /* [2] Long Packet CRC Error */
    UINT32  FrameSyncError:         1;      /* [3] Frame sync error (Received unpaired SOF or EOF) */
    UINT32  ProtState:              2;      /* [5:4] State of FSM in MIPI protocol module */
    UINT32  SofPacketReceived:      1;      /* [6] SOF Short Packet Received */
    UINT32  EofPacketReceived:      1;      /* [7] EOF Short Packet Received */
    UINT32  Reserved2:              24;     /* [31:8] */
} AMBA_DBG_PORT_IDSP_MIPI_STATUS0_REG_s;

typedef struct {
    UINT32  Count:                  1;      /* [0] fatal error status error count */
    UINT32  Reserved:               9;      /* [9:1] */
    UINT32  Module:                 5;      /* [14:10] fatal error status module ID. Can be used to debug which internal RTL module gener¬ated error. */
    UINT32  Reserved1:              17;     /* [31:15] */
} AMBA_DBG_PORT_IDSP_ERR_STATUS0_REG_s;

typedef struct {
    UINT32  Count:                  10;     /* [9:0] non-fatal error status error count */
    UINT32  Module:                 5;      /* [14:10] non-Fatal error status module ID. Can be used to debug which internal RTL module gener¬ated error. */
    UINT32  Reserved:               17;     /* [31:15] */
} AMBA_DBG_PORT_IDSP_ERR_STATUS1_REG_s;

typedef struct {
    UINT32  Fatal:                  1;      /* [0] insert fatal error */
    UINT32  NonFatal:               1;      /* [1] insert non-fatal error */
    UINT32  Reserved:               30;     /* [31:2] */
} AMBA_DBG_PORT_IDSP_INSERT_ERR_REG_s;

typedef struct {
    UINT32  NonFatal:               10;     /* [9:0] threshold for non-fatal error */
    UINT32  Reserved:               22;     /* [31:10] */
} AMBA_DBG_PORT_IDSP_ERR_THRESHOLD_REG_s;

typedef struct {
    UINT32  HsyncPolarity:          1;      /* [0] 0=Active Low; 1=Active High */
    UINT32  VsyncPolarity:          1;      /* [1] 0=Active Low; 1=Active High */
    UINT32  NoVblankHsync:          1;      /* [2] Don't toggle HSYNC during VBLANK */
    UINT32  InterruptMode:          1;      /* [3] VSYNC interrupt at 0=End of VBLANK period; 1=End of VBLANK Pulse */
    UINT32  VSyncWidthUnit:         1;      /* [4] VSYNC_WIDTH is measured in: 0=cycles; 1=HSYNCS */
    UINT32  NumVsyncs:              8;      /* [12:5] Number of VSYNC cycles to send before loading next command (if present); 0=Turn off sync */
    UINT32  Continuous:             1;      /* [13] If no new command is available after sending NUM_VSYNCS: 0=Stop; 1=Continue until new command is received */
    UINT32  Preempt:                1;      /* [14] Load this command on the next VSYNC even if the current command hasn't finished */
    UINT32  Reserved:               17;     /* [31:15] */
} AMBA_DBG_PORT_IDSP_MASTER_SYNC_CTRL_REG_s;

typedef struct {
    UINT32  SensorType:             2;      /* [1:0] 0=Serial SLVS; 1=Parallel SLVS; 2=Parallel LVCMOS; 3=MIPI */
    UINT32  SlvsClkMode:            1;      /* [2] 0=All Lanes latched with spclk[0]; 1=Every 4 lanes latched by group/phy clock */
    UINT32  AFIFOBypass:            1;      /* [3] Bypass CDC/SLVS FIFOs; all PHY data is latched by slvs clk0 */
    UINT32  LvcmosEdge:             1;      /* [4] 0=posedge; 1=negedge */
    UINT32  MipiVCEnable:           1;      /* [5] set to 1 if virtual channels are preset */
    UINT32  Reserved:               26;     /* [31:6] */
} AMBA_DBG_PORT_IDSP_GLOBAL_REG_s;

typedef struct {
    volatile AMBA_DBG_PORT_IDSP_CTRL0_REG_s                 Ctrl0;                  /* 0x0000: Sensor interface module control0 */
    volatile AMBA_DBG_PORT_IDSP_CTRL1_REG_s                 Ctrl1;                  /* 0x0004: Sensor interface module control1 */
    volatile AMBA_DBG_PORT_IDSP_DVP_CTRL_REG_s              DvpCtrl;                /* 0x0008: Parallel control register */
    volatile AMBA_DBG_PORT_IDSP_SYNC_PIN_MUX_REG_s          SyncPinMux;             /* 0x000C: VIN sync pin MUX Configuration Register */
    volatile AMBA_DBG_PORT_IDSP_DATA_PIN_MUX_REG_s          DataPinMux;             /* 0x0010: VIN data pin MUX Configuration Register */
    volatile AMBA_DBG_PORT_IDSP_DATA_PIN_MUX_REG_s          DataPinMux1;            /* 0x0014: VIN data pin MUX Configuration Register */
    volatile AMBA_DBG_PORT_IDSP_DATA_PIN_MUX_REG_s          DataPinMux2;            /* 0x0018: VIN data pin MUX Configuration Register */
    volatile AMBA_DBG_PORT_IDSP_RESERVED_DATA_PIN_MUX_REG_s DataPinMux3;            /* 0x001C: VIN data pin MUX Configuration Register */
    volatile AMBA_DBG_PORT_IDSP_CTRL2_REG_s                 Ctrl2;                  /* 0x0020: Sensor interface module control2 */
    volatile UINT32                                         ActiveRegionWidth;      /* 0x0024: active width - 1, 0=auto detect (Only supported for protocols with EOL/EAV codes) */
    volatile UINT32                                         ActiveRegionHeight;     /* 0x0028: active height - 1, 0=auto-detect */
    volatile AMBA_DBG_PORT_IDSP_SPLIT_CTRL_REG_s            SplitCtrl;              /* 0x002C: SPLIT_WIDTH pixels (0=disabled) */
    volatile UINT32                                         CropStartCol;           /* 0x0030: Start column index of crop region */
    volatile UINT32                                         CropStartRow;           /* 0x0034: Start row index of crop region */
    volatile UINT32                                         CropEndCol;             /* 0x0038: End column index of crop region */
    volatile UINT32                                         CropEndRow;             /* 0x003C: End row index of crop region */
    volatile AMBA_DBG_PORT_IDSP_SLVS_SYNC_LOCATION_REG_s    SlvsSyncCodeLocation;   /* 0x0040: SLVS Sync code location */
    volatile AMBA_DBG_PORT_IDSP_SLVS_CTRL_REG_s             SlvsCtrl;               /* 0x0044: SLVS control */
    volatile AMBA_DBG_PORT_IDSP_SLVS_SYNC_DETECT_REG_s      SlvsSyncCodeDetect;     /* 0x0048: SLVS sync code detection enable */
    volatile UINT32                                         WatchdogTimeoutL;       /* 0x004C: Watchdog timeout value (Bit[15:0]) in unit of idsp cycles, 0=disabled */
    volatile UINT32                                         WatchdogTimeoutH;       /* 0x0050: Watchdog timeout value (Bit[31:16]) in unit of idsp cycles, 0=disabled */
    volatile UINT32                                         SlvsSyncDetectMask;     /* 0x0054: VIN compares ( SYNC_DETECT_MASK & SYNC_CODE ) against SYNC_DETECT_PAT */
    volatile UINT32                                         SlvsSyncDetectPattern;  /* 0x0058 */
    volatile UINT32                                         SlvsSyncCompareMask;    /* 0x005C: Mask to apply to sync code before comparing to patterns */
    volatile UINT32                                         SlvsSolPattern;         /* 0x0060: Pattern to match to indicate SOL code */
    volatile UINT32                                         SlvsEolPattern;         /* 0x0064: Pattern to match to indicate EOL code */
    volatile UINT32                                         SlvsSofPattern;         /* 0x0068: Pattern to match to indicate SOF code */
    volatile UINT32                                         SlvsEofPattern;         /* 0x006C: Pattern to match to indicate EOF code */
    volatile UINT32                                         SlvsSovPattern;         /* 0x0070: Pattern to match to indicate SOV code */
    volatile UINT32                                         SlvsEovPattern;         /* 0x0074: Pattern to match to indicate EOV code */
    volatile AMBA_DBG_PORT_IDSP_MIPI_CTRL0_REG_s            MipiCtrl0;              /* 0x0078 */
    volatile AMBA_DBG_PORT_IDSP_MIPI_CTRL1_REG_s            MipiCtrl1;              /* 0x007C */
    volatile AMBA_DBG_PORT_IDSP_VOUT_SYNC_REG_s             VoutSync[2];            /* 0x0080 - 0x0084 */
    volatile AMBA_DBG_PORT_IDSP_STRIG_CTRL_REG_s            Strig0;                 /* 0x0088 */
    volatile UINT32                                         Strig0EndLine;          /* 0x008C */
    volatile AMBA_DBG_PORT_IDSP_STRIG_CTRL_REG_s            Strig1;                 /* 0x0090 */
    volatile UINT32                                         Strig1EndLine;          /* 0x0094 */
    volatile UINT32                                         SyncDelayL;             /* 0x0098: Delay in cycles after VSYNC before assertion of 'delayed vsync' signal to controller */
    volatile UINT32                                         SyncDelayH;             /* 0x009C */
    volatile UINT32                                         IntrDelayL;             /* 0x00A0: Delay in cycles after VSYNC before assertion of 'delayed vsync' interrupt to arm */
    volatile UINT32                                         IntrDelayH;             /* 0x00A4 */
    volatile AMBA_DBG_PORT_IDSP_BLACKLEVEL_CTRL_REG_s       BlackLevelCtrl;         /* 0x00A8 */
    volatile UINT32                                         BlackLevelOffset16LSB;  /* 0x00AC */
    volatile AMBA_DBG_PORT_IDSP_ERR_MASK_REG_s              ErrIntrMask;            /* 0x00B0: Error won't be reported to idsp intr when set to zero */
    volatile AMBA_DBG_PORT_IDSP_ERR_MASK_REG_s              ECRMask;                /* 0x00B4: Error won't be reported to sm err_vld when set to zero */
    volatile AMBA_DBG_PORT_IDSP_STATUS_REG_s                Status;                 /* 0x00B8 */
    volatile AMBA_DBG_PORT_IDSP_SLVS_STATUS_REG_s           SlvsStatus;             /* 0x00BC */
    volatile UINT32                                         DetectedActiveWidth;    /* 0x00C0: Detected Active Width */
    volatile UINT32                                         DetectedActiveHeight;   /* 0x00C4: Detected Active Height */
    volatile UINT32                                         SyncCode;               /* 0x00C8: Value of last received sync code (16-bit MSB aligned) */
    volatile UINT32                                         ActiveFrameHeight;      /* 0x00CC: Current active region height */
    volatile UINT32                                         ActiveFrameWidth;       /* 0x00D0: Current active region width */
    volatile UINT32                                         CropRegionHeight;       /* 0x00D4: Current crop region height */
    volatile UINT32                                         CropRegionWidth;        /* 0x00D8: Current crop region width */
    volatile AMBA_DBG_PORT_IDSP_MIPI_STATUS0_REG_s          MipiStatus0;            /* 0x00DC */
    volatile UINT32                                         MipiLongPacketCount;    /* 0x00E0: Number of long packets received */
    volatile UINT32                                         MipiShortPacketCount;   /* 0x00E4: Number of short packets received */
    volatile UINT32                                         MipiLongPacketSize;     /* 0x00E8: Number of bytes in last received long packet */
    volatile UINT32                                         MipiNumCrcErrPacket;    /* 0x00EC: Number of packets with CRC errors */
    volatile UINT32                                         MipiFrameNumber;        /* 0x00F0: Frame number (from short-packet/header info) */
    volatile UINT32                                         MipiCrcErrThreshold;    /* 0x00F4: Number of CRC err threshold that when crossed, error will be reported */
    volatile UINT32                                         MipiCorrEccErrThreshold;/* 0x00F8: Number of correctable threshold that when crossed, error will be reported */
    volatile UINT32                                         MipiNumCorrEccErrPacket;/* 0x00FC: Number of packets with correctable errors */
    volatile UINT32                                         Reserved2[64];          /* 0x0100-0x01FC */
    volatile UINT32                                         ConfigDone;             /* 0x0200: manually shift VIN programming via debug bus */
} AMBA_DBG_PORT_IDSP_VIN_MAIN_REG_s;

typedef struct {
    volatile AMBA_DBG_PORT_IDSP_ERR_STATUS0_REG_s           ErrorStatus0;           /* 0x7FC0: fatal error status */
    volatile UINT32                                         Reserved;               /* 0x7FC4 */
    volatile AMBA_DBG_PORT_IDSP_ERR_STATUS1_REG_s           ErrorStatus1;           /* 0x7FC8: non-fatal error status */
    volatile UINT32                                         Reserved1;              /* 0x7FCC */
    volatile AMBA_DBG_PORT_IDSP_INSERT_ERR_REG_s            ErrorInsert;            /* 0x7FD0: insert fata/non-fatal error */
    volatile AMBA_DBG_PORT_IDSP_ERR_THRESHOLD_REG_s         ErrorThreshold;         /* 0x7FD4: threshold for non-fatal error */
} AMBA_DBG_PORT_IDSP_VIN_MAIN_SM_DBG_REG_s; /* Bank 7 */

typedef struct {
    volatile UINT32                                         HSyncPeriodL;           /* 0x0400: HSYNC period in cycles(Bit[15:0]) */
    volatile UINT32                                         HSyncPeriodH;           /* 0x0404: HSYNC period in cycles(Bit[31:16])*/
    volatile UINT32                                         HSyncWidth;             /* 0x0408: Width of HSYNC pulse in cycles */
    volatile UINT32                                         HSyncOffset;            /* 0x040C: Offset of HSYNC pulse in cycles */
    volatile UINT32                                         VSyncPeriodL;           /* 0x0410: Number of HSYNCs between VSYNCs */
    volatile UINT32                                         VSyncPeriodH;           /* 0x0414: Number of HSYNCs between VSYNCs */
    volatile UINT32                                         VSyncWidth;             /* 0x0418: Width of VSYNC pulse in cycles OR HSYNCs (see VSYNC_WIDTH_UNIT) */
    volatile UINT32                                         VSyncOffset;            /* 0x041C: Offset between HSYNC and VSYNC edges (in cycles); VSYNC trails HSYNC */
    volatile AMBA_DBG_PORT_IDSP_MASTER_SYNC_CTRL_REG_s      MasterSyncCtrl;         /* 0x0420 */
} AMBA_DBG_PORT_IDSP_VIN_MASTER_SYNC_REG_s;

typedef struct {
    volatile AMBA_DBG_PORT_IDSP_GLOBAL_REG_s                GlobalConfig;           /* 0x0800 */
} AMBA_DBG_PORT_IDSP_VIN_GLOBAL_REG_s;

#endif /* AMBA_DEBUG_PORT_IDSP_H */
