/*
 * $QNXLicenseC:
 * Copyright 2018, QNX Software Systems.
 * Copyright 2020, Ambarella International LP
 *
 * Licensed under the Apache License, Version 2.0 (the "License"). You
 * may not reproduce, modify or distribute this software except in
 * compliance with the License. You may obtain a copy of the License
 * at: http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTIES OF ANY KIND, either express or implied.
 *
 * This file may contain contributions from others, either as
 * contributors under the License or as licensors under other terms.
 * Please review this entire file for other proprietary rights or license
 * notices, as well as the QNX Development Suite License Guide at
 * http://licensing.qnx.com/license-guide/ for other information.
 * $
 */

#include <stdio.h>
#include <string.h>
#include <sys/mman.h>

#include "diag.h"
#include "AmbaKAL.h"
#include "AmbaSYS.h"

#include "AmbaTypes.h"

#ifndef AMBA_DRAMC_DEF_H
#include "AmbaDRAMC_Def.h"
#endif

#ifndef AMBA_CORTEX_A53_H
#include "AmbaCortexA53.h"
#endif

/* Definitions of DDR controller */
#define AMBA_DDRC0                  0U      /* DDRC0 */
#define AMBA_DDRC1                  1U      /* DDRC1 */
#define AMBA_NUM_DDRC               2U      /* Number of DDR Controller */

/* Definitions of DDRC DLL byte */
#define AMBA_DDRC_DLL_BYTE_0        0U      /* DLL_Read */
#define AMBA_DDRC_DLL_BYTE_1        1U      /* DLL_Sync */
#define AMBA_DDRC_DLL_BYTE_2        2U      /* DLL_Write */

static const UINT8 DiagDdrPhaseSetting[64] = {  /* for DLL write/sync/read phase shift  */
    0x3F, 0x3E, 0x3D, 0x3C, 0x3B, 0x3A, 0x39, 0x38, 0x37, 0x36, 0x35, 0x34, 0x33, 0x32, 0x31, 0x30,
    0x2F, 0x2E, 0x2D, 0x2C, 0x2B, 0x2A, 0x29, 0x28, 0x27, 0x26, 0x25, 0x24, 0x23, 0x22, 0x21, 0x20,
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F
};

/*
 * DRAMC: All Registers
 */
typedef struct {
    volatile UINT32 ClientRequestStatis[32];    /* 0x1000-0x107c: Statistics for each client's number of requests */
    volatile UINT32 ClientBurstStatis[32];      /* 0x1080-0x10Fc: Statistics for each client's number of bursts */
    volatile UINT32 ClientMaskWriteStatis[32];  /* 0x1100-0x117c: Statistics for each client's number of masked write bursts */
    volatile UINT32 BankOpensStatis[8];         /* 0x1180-0x119c: Statistics for global numbers of Bank opens */
    volatile UINT32 GlobalReadWriteStatis;      /* 0x11a0: Statistics for Global numbers of read/write toggles */
    volatile UINT32 Reserved7[7];               /* 0x11a4-0x11bc: Reserved */
    volatile UINT32 DramStatisCtrl;             /* 0x11c0: 0 = OFF; 1 = ON; 2 = Reset, When reset is done, the read value will be changed to 0(OFF) */
} AMBA_DRAMC_REG_s;

/*
 * DDRC: DRAM Controller All Registers
 */
typedef struct {
    volatile UINT32                         Ctrl;                   /* 0x000(RW): DRAM control */
    volatile UINT32                         Config;                 /* 0x004(RW): DRAM config  */
    volatile UINT32                         Timing1;                /* 0x008(RW): DRAM timing1 */
    volatile UINT32                         Timing2;                /* 0x00C(RW): DRAM timing2 */
    volatile UINT32                         Timing3;                /* 0x010(RW): DRAM timing3 */
    volatile UINT32                         Timing4;                /* 0x014(RW): DRAM timing4 */
    volatile UINT32                         InitCtrl;               /* 0x018(RW): DRAM init control */
    volatile UINT32                         ModeReg;                /* 0x01C(RW): DRAM mode register */
    volatile UINT32                         SelfRefresh;            /* 0x020(RW): DRAM self refresh */
    volatile UINT32                         DqsSync;                /* 0x024(RO): DRAM DQS sync */
    volatile UINT32                         PadTerm;                /* 0x028(RW): DRAM Pad termination */
    volatile UINT32                         ZqCalib;                /* 0x02C(RW): DRAM ZQ calibration */
    volatile UINT32                         RsvdSpace;              /* 0x030(RW): Base address of the reserved DRAM space, must be 128-byte aligned */
    volatile UINT32                         ByteMap;                /* 0x034(RW): Mappping of DRAM DDRIO bytes and DDRC bytes */
    volatile UINT32                         DramPowerDownCtrl;      /* 0x038(RW): Power down */
    volatile UINT32                         DllCalib;               /* 0x03C(RW): DLL calibration */
    volatile UINT32                         DramDebug;              /* 0x040(RW): DRAM debug */
    volatile UINT32                         Ddr4CtrlTiming;         /* 0x044(RW): DDR4 control timing */
    volatile UINT32                         PadTerm2;               /* 0x048(RW): DRAM Pad termination */
    volatile UINT32                         PadTerm3;               /* 0x04C(RW): DRAM Pad termination */
    volatile UINT32                         MpcTrainWriteData[8];   /* 0x050-0x06C(RW): LPDDR4 MPC Training Write Data Register */
    volatile UINT32                         MpcTrainReadData[8];    /* 0x070-0x08C(RW): LPDDR4 MPC Training Read Data Register */
    volatile UINT32                         UInstruct;              /* 0x090(RW): LPDDR4 U-instruction Register */
    volatile UINT32                         TrainDataMask;          /* 0x094(RW): LPDDR4 Training Data Mask Register */
    volatile UINT32                         TrainRingOsciConut;     /* 0x098(RO): LPDDR4 Training Ring Oscillator Count Register */
    volatile UINT32                         TrainCommandDone;       /* 0x09C(RO/WC): LPDDR4 Training Command Done Register */
    volatile UINT32                         TrainDqCapture;         /* 0x0A0(RO): LPDDR4 Training DQ Capture Register */
    volatile UINT32                         TrainDqWriteDly;        /* 0x0A4(RW): LPDDR4 Training DQ Write Delay Register */
    volatile UINT32                         TrainDqReadDly;         /* 0x0A8(RW): LPDDR4 Training DQ READ Delay Register */
    volatile UINT32                         TrainDqsWriteDly;       /* 0x0AC(RW): LPDDR4 Training DQS Write Delay Register */
    volatile UINT32                         TrainCkCaWriteDly;      /* 0x0B0(RW): LPDDR4 Training CK/CA Write Delay Register */
    volatile UINT32                         TrainDqCaVref;          /* 0x0B4(RW): LPDDR4 Training DQ/CA VREF Register */
    volatile UINT32                         TrainSwScratchpad;      /* 0x0B8(RW): LPDDR4 Training Scratchpad Register */
    volatile UINT32                         RingOsciTimerConut;     /* 0x0BC(RW): LPDDR4 Ring Oscillator Timer Register */
    volatile UINT32                         Lpddr4VrefCtrl;         /* 0x0C0(RW): LPDDR4 VREF Control Register */
    volatile UINT32                         Lpddr4DmRead;           /* 0x0C4(RW): LPDDR4 Dm Read Register */
    volatile UINT32                         Lpddr4ModeRead;         /* 0x0C8(RO): LPDDR4 Mode Read Register */
    volatile UINT32                         Lpddr4MpcReadDelay;     /* 0x0CC(RW): LPDDR4 MPC Read Delay Register */
    volatile UINT32                         Reserved0[12];          /* 0x0D0-0x0FC: Reserved */
    volatile UINT32                         DdrioDll0;              /* 0x100(RW): DDRIO DLL0 */
    volatile UINT32                         DdrioDll1;              /* 0x104(RW): DDRIO DLL1 */
    volatile UINT32                         DdrioDll2;              /* 0x108(RW): DDRIO DLL2 */
    volatile UINT32                         DdrioDll3;              /* 0x10C(RW): DDRIO DLL3 */
    volatile UINT32                         DdrioDllCtrlMisc;       /* 0x110(RW): DDRIO DLL Ctrl Misc */
    volatile UINT32                         Reserved1[3];           /* 0x114-0x11C: Reserved */
    volatile UINT32                         DdrioDllCtrlSel0;       /* 0x120(RW): DDRIO DLL Ctrl Select0 */
    volatile UINT32                         DdrioDllCtrlSel1;       /* 0x124(RW): DDRIO DLL Ctrl Select1 */
    volatile UINT32                         DdrioDllCtrlSel2;       /* 0x128(RW): DDRIO DLL Ctrl Select2 */
    volatile UINT32                         DdrioDllCtrlSel3;       /* 0x12C(RW): DDRIO DLL Ctrl Select3 */
    volatile UINT32                         DdrioDllStatusSel0;     /* 0x130(RW): DDRIO DLL Status Select0 */
    volatile UINT32                         DdrioDllStatusSel1;     /* 0x134(RW): DDRIO DLL Status Select1 */
    volatile UINT32                         DdrioDllStatusSel2;     /* 0x138(RW): DDRIO DLL Status Select2 */
    volatile UINT32                         DdrioDllStatusSel3;     /* 0x13C(RW): DDRIO DLL Status Select3 */
    volatile UINT32                         DdrioDllSyncCtrlSel0;   /* 0x140(RW): DDRIO DLL Sync Ctrl Select0 */
    volatile UINT32                         DdrioDllSyncCtrlSel1;   /* 0x144(RW): DDRIO DLL Sync Ctrl Select1 */
    volatile UINT32                         DdrioDllSyncCtrlSel2;   /* 0x148(RW): DDRIO DLL Sync Ctrl Select2 */
    volatile UINT32                         DdrioDllSyncCtrlSel3;   /* 0x14C(RW): DDRIO DLL Sync Ctrl Select3 */
    volatile UINT32                         DdrioDllSyncObsv0;      /* 0x150(RW): DDRIO DLL Sync Obsv0 */
    volatile UINT32                         DdrioDllSyncObsv1;      /* 0x154(RW): DDRIO DLL Sync Obsv1 */
    volatile UINT32                         DdrioDllCAStatus;       /* 0x158(RW): DDRIO DLL Sync CA Status */
    volatile UINT32                         Reserved2;              /* 0x15C: Reserved */
    volatile UINT32                         DdrioDutyCtrlDq0;       /* 0x160(RW): DDRIO Duty Ctrl Byte0 (DQ) */
    volatile UINT32                         DdrioDutyCtrlDq1;       /* 0x164(RW): DDRIO Duty Ctrl Byte1 (DQ) */
    volatile UINT32                         DdrioDutyCtrlDq2;       /* 0x168(RW): DDRIO Duty Ctrl Byte2 (DQ) */
    volatile UINT32                         DdrioDutyCtrlDq3;       /* 0x16C(RW): DDRIO Duty Ctrl Byte3 (DQ) */
    volatile UINT32                         DdrioDutyCtrlDqs0;      /* 0x170(RW): DDRIO Duty Ctrl Byte0 (DQS) */
    volatile UINT32                         DdrioDutyCtrlDqs1;      /* 0x174(RW): DDRIO Duty Ctrl Byte1 (DQS) */
    volatile UINT32                         DdrioDutyCtrlDqs2;      /* 0x178(RW): DDRIO Duty Ctrl Byte2 (DQS) */
    volatile UINT32                         DdrioDutyCtrlDqs3;      /* 0x17C(RW): DDRIO Duty Ctrl Byte3 (DQS) */
    volatile UINT32                         DdrioDutyCtrlCaA;       /* 0x180(RW): DDRIO Duty Ctrl (CA_A) */
    volatile UINT32                         DdrioDutyCtrlCaB;       /* 0x184(RW): DDRIO Duty Ctrl (CA_B) */
    volatile UINT32                         DdrioDutyCtrlCkA;       /* 0x188(RW): DDRIO Duty Ctrl (CK_A) */
    volatile UINT32                         DdrioDutyCtrlCkB;       /* 0x18C(RW): DDRIO Duty Ctrl (CK_B) */
} AMBA_DDRC_REG_s;

#define PHYSICAL_DDRC_ADDR(ddrc, addr) \
    ((UINT32)addr - (UINT32)&pAmbaDDRC_Reg[DdrcId]->Ctrl + ((ddrc == 0) ? AMBA_CORTEX_A53_DDR_CTRL0_BASE_ADDR : AMBA_CORTEX_A53_DDR_CTRL1_BASE_ADDR))

/*
 * Defined in AmbaMmioBase.asm
 */
static AMBA_DDRC_REG_s * pAmbaDDRC_Reg[2] = {0U};
static AMBA_DRAMC_REG_s *pAmbaDRAMC_Reg = NULL;

static UINT32 DramElapTimeStart = 0U;

void IoDiag_DramResetStatis(void)
{
    UINT32 Count = 0U;

    AmbaKAL_GetSysTickCount(&DramElapTimeStart);
    pAmbaDRAMC_Reg->DramStatisCtrl = 0x2U;
    while ((pAmbaDRAMC_Reg->DramStatisCtrl != 0x0U) && (Count < 2000000U)) {
        Count ++;
    }
    pAmbaDRAMC_Reg->DramStatisCtrl = 0x1U;
}

UINT32 IoDiag_DramShowStatisInfo(void)
{
    uintptr_t virt_base;
    UINT32 Index;
    AMBA_DRAMC_STATIS_s DramStatic = {0};
    static const char *DramClientName[NUM_DRAM_CLIENT] = {
        [DRAM_CLIENT_CA53]          = "AXI_CA53",
        [DRAM_CLIENT_DMA0]          = "DMA0    ",
        [DRAM_CLIENT_DMA1]          = "DMA1    ",
        [DRAM_CLIENT_ENET]          = "ENET    ",
        [DRAM_CLIENT_FDMA]          = "FDMA    ",
        [DRAM_CLIENT_CAN0]          = "CAN0    ",
        [DRAM_CLIENT_CAN1]          = "CAN1    ",
        [DRAM_CLIENT_GDMA]          = "GDMA    ",
        [DRAM_CLIENT_SD0]           = "SD0     ",
        [DRAM_CLIENT_SD1]           = "SD1     ",
        [DRAM_CLIENT_USB_DEVICE]    = "USB_DEV ",
        [DRAM_CLIENT_USB_HOST]      = "USE_HOST",
        [DRAM_CLIENT_ORCME]         = "ORCME   ",
        [DRAM_CLIENT_ORCCODE]       = "ORCCODE ",
        [DRAM_CLIENT_ORCVP]         = "ORCVP   ",
        [DRAM_CLIENT_ORCL2]         = "ORCL2   ",
        [DRAM_CLIENT_SMEM]          = "SMEM    ",
        [DRAM_CLIENT_VMEM]          = "VMEM    ",
        [DRAM_CLIENT_FEX]           = "FEX     ",
        [DRAM_CLIENT_BMEM]          = "BMEM    ",
    };
    UINT32 ElapsedTime, CpuId = 0U;
    UINT32 i, RetVal = DRAMC_ERR_NONE;

    pAmbaDRAMC_Reg->DramStatisCtrl = 0x0U; //Disable

    for(Index = 0U; Index < 32U; Index++) {
        DramStatic.ClientRequestStatis[Index] = pAmbaDRAMC_Reg->ClientRequestStatis[Index];
        DramStatic.ClientBurstStatis[Index] = pAmbaDRAMC_Reg->ClientBurstStatis[Index];
        DramStatic.ClientMaskWriteStatis[Index] = pAmbaDRAMC_Reg->ClientMaskWriteStatis[Index];
    }

    AmbaKAL_GetSysTickCount(&ElapsedTime);
    ElapsedTime -= DramElapTimeStart;
    ElapsedTime ++;

    printf("----------------------------------------------------------\n");
    printf(" Elapsed time: ");
    printf("%u", ElapsedTime);
    printf("  microseconds\n");
    printf("----------------------------------------------------------\n");
    printf(" ID  CLIENT_NAME REQUEST         BURST       MASK_BURST\n");
    printf("----------------------------------------------------------\n");

    for(i = 0U; i < NUM_DRAM_CLIENT; i++) {
        printf("%d", i);
        printf("  ");
        printf(DramClientName[i]);
        printf("  ");
        printf("%u", DramStatic.ClientRequestStatis[i]);
        printf("  ");
        printf("%u", DramStatic.ClientBurstStatis[i]);
        printf("  ");
        printf("%u", DramStatic.ClientMaskWriteStatis[i]);
        printf("\n");

        printf("----------------------------------------------------------\n");
    }

    return RetVal;
}


static void IoDiag_DdrCmdUsage(char * const *pArgVector)
{
    printf("Usage: ");
    printf(pArgVector[1]);
    printf(" dump\n");
    printf("       ");
    printf(pArgVector[1]);
    printf(" statis [reset]\n");
    printf("       ");
    printf(pArgVector[1]);
    printf(" shmoo [HostId] [sync|read|write] [+|-] [time(ms)]\n");
    printf("          How to run DDR quick shmoo :\n");
    printf("           - 1. Type \"sync + 1000\" : Increase sync value and wait 1000ms, shmoo the sync byte one by one until system hang, ex: 0x20->0x00->0x01->0x02\n");
    printf("           - 2. Type \"sync - 1000\" : Decrease sync value and wait 1000ms, shmoo the sync byte one by one until system hang, ex: 0x20->0x21->0x22->0x23\n");
    printf("           - 3. Repeat the above procedures on Read and Write phase.\n");
    printf("           - 4. Collect the workable ranges of sync, read and write byte.\n");
    printf("          HostID : [0|1]\n");
    printf("       ");
    printf(pArgVector[1]);
    printf(" set [sync|read|write] <value>\n");
    printf("\n The following commands are for LPDDR4 only!\n");
    printf(" [lp4rdly|lp4vref|lp4wdly|lp4mr14|lp4mr12] [0:DDRC0|1:DDRC1] [+|-] [time(ms)]\n");
    printf(" [lp4rdly|lp4vref|lp4wdly|lp4mr14|lp4mr12] [0:DDRC0] [+|-] [time(ms)]\n");
    printf("          diag ddr lp4rdly 0 + 1000 \n");
    printf("          Read : lp4rdly and lp4vref\n");
    printf("          Write: lp4wdly and lp4mr14\n");
}

static void IoDiag_DdrDump()
{
    const char DiagDdrDramType[8][8] = {
        [0] = "DDR2",
        [1] = "DDR3",
        [2] = "LPDDR",
        [3] = "LPDDR2",
        [4] = "LPDDR3",
        [5] = "DDR4",
        [6] = "LPDDR4",
        [7] = "N/A"
    };
    const char DiagDdrDramSize[8][8] = {
        [0] = "256Mb",
        [1] = "512Mb",
        [2] = "1Gb",
        [3] = "2Gb",
        [4] = "4Gb",
        [5] = "8Gb",
        [6] = "16Gb",
        [7] = "N/A"
    };
    const char DiagDdrPageSize[4][8] = {
        [0] = "1KB",
        [1] = "2KB",
        [2] = "4KB",
        [3] = "N/A"
    };
    const char DiagDdrDqBusWidth[2][8] = {
        [0] = "32-bit",
        [1] = "16-bit"
    };

    UINT32 Value;
    UINT32 DdrcId;

    printf("--------------------------------------------------------------------------------\n");

    /* dram freq */
    AmbaSYS_GetClkFreq(AMBA_SYS_CLK_DRAM, &Value);
    printf("DRAM Frequency      = %u\n", Value);

    for (DdrcId = AMBA_DDRC0; DdrcId < AMBA_NUM_DDRC; DdrcId ++) {
        printf("--------------------------------------------------------------------------------\n");

        /* ddrc id */
        if (DdrcId == AMBA_DDRC0) {
            printf("DDRC-0:\n");
        } else {
            printf("DDRC-1:\n");
        }

        /* dram type */
        Value = (pAmbaDDRC_Reg[DdrcId]->Config>>27U) & 0x7;
        printf("DRAM Type = ");
        printf(DiagDdrDramType[Value]);
        printf("\n");
        /* dram size */
        Value = (pAmbaDDRC_Reg[DdrcId]->Config>>5U) & 0x7;
        printf("DRAM Size = ");
        printf(DiagDdrDramSize[Value]);
        printf("\n");
        /* DQ bus width */
        Value = (pAmbaDDRC_Reg[DdrcId]->Config>>8U) & 0x1;
        printf("DQ width = ");
        printf(DiagDdrDqBusWidth[Value]);
        printf("\n");
        /* page size */
        Value = (pAmbaDDRC_Reg[DdrcId]->Config>>2U) & 0x3;
        printf("Page size = ");
        printf(DiagDdrPageSize[Value]);
        printf("\n");

        printf("DRAM_Ctrl                    (0x%x)->[0x%x] = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->Ctrl,                  PHYSICAL_DDRC_ADDR(DdrcId, &pAmbaDDRC_Reg[DdrcId]->Ctrl),                  pAmbaDDRC_Reg[DdrcId]->Ctrl);
        printf("DRAM_Config                  (0x%x)->[0x%x] = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->Config,                PHYSICAL_DDRC_ADDR(DdrcId, &pAmbaDDRC_Reg[DdrcId]->Config),                pAmbaDDRC_Reg[DdrcId]->Config);
        printf("DRAM_Timing1                 (0x%x)->[0x%x] = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->Timing1,               PHYSICAL_DDRC_ADDR(DdrcId, &pAmbaDDRC_Reg[DdrcId]->Timing1),               pAmbaDDRC_Reg[DdrcId]->Timing1);
        printf("DRAM_Timing2                 (0x%x)->[0x%x] = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->Timing2,               PHYSICAL_DDRC_ADDR(DdrcId, &pAmbaDDRC_Reg[DdrcId]->Timing2),               pAmbaDDRC_Reg[DdrcId]->Timing2);
        printf("DRAM_Timing3                 (0x%x)->[0x%x] = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->Timing3,               PHYSICAL_DDRC_ADDR(DdrcId, &pAmbaDDRC_Reg[DdrcId]->Timing3),               pAmbaDDRC_Reg[DdrcId]->Timing3);
        printf("DRAM_Timing4                 (0x%x)->[0x%x] = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->Timing4,               PHYSICAL_DDRC_ADDR(DdrcId, &pAmbaDDRC_Reg[DdrcId]->Timing4),               pAmbaDDRC_Reg[DdrcId]->Timing4);
        printf("DRAM_InitCtrl                (0x%x)->[0x%x] = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->InitCtrl,              PHYSICAL_DDRC_ADDR(DdrcId, &pAmbaDDRC_Reg[DdrcId]->InitCtrl),              pAmbaDDRC_Reg[DdrcId]->InitCtrl);
        printf("DRAM_ModeReg                 (0x%x)->[0x%x] = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->ModeReg,               PHYSICAL_DDRC_ADDR(DdrcId, &pAmbaDDRC_Reg[DdrcId]->ModeReg),               pAmbaDDRC_Reg[DdrcId]->ModeReg);
        printf("DRAM_DqsSync                 (0x%x)->[0x%x] = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->DqsSync,               PHYSICAL_DDRC_ADDR(DdrcId, &pAmbaDDRC_Reg[DdrcId]->DqsSync),               pAmbaDDRC_Reg[DdrcId]->DqsSync);
        printf("DRAM_PadTerm                 (0x%x)->[0x%x] = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->PadTerm,               PHYSICAL_DDRC_ADDR(DdrcId, &pAmbaDDRC_Reg[DdrcId]->PadTerm),               pAmbaDDRC_Reg[DdrcId]->PadTerm);
        printf("DRAM_PadTerm2                (0x%x)->[0x%x] = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->PadTerm2,              PHYSICAL_DDRC_ADDR(DdrcId, &pAmbaDDRC_Reg[DdrcId]->PadTerm2),              pAmbaDDRC_Reg[DdrcId]->PadTerm2);
        printf("DRAM_PadTerm3                (0x%x)->[0x%x] = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->PadTerm3,              PHYSICAL_DDRC_ADDR(DdrcId, &pAmbaDDRC_Reg[DdrcId]->PadTerm3),              pAmbaDDRC_Reg[DdrcId]->PadTerm3);
        printf("DRAM_ZqCalib                 (0x%x)->[0x%x] = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->ZqCalib,               PHYSICAL_DDRC_ADDR(DdrcId, &pAmbaDDRC_Reg[DdrcId]->ZqCalib),               pAmbaDDRC_Reg[DdrcId]->ZqCalib);
        printf("DRAM_RsvdSpace               (0x%x)->[0x%x] = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->RsvdSpace,             PHYSICAL_DDRC_ADDR(DdrcId, &pAmbaDDRC_Reg[DdrcId]->RsvdSpace),             pAmbaDDRC_Reg[DdrcId]->RsvdSpace);
        printf("DRAM_ByteMap                 (0x%x)->[0x%x] = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->ByteMap,               PHYSICAL_DDRC_ADDR(DdrcId, &pAmbaDDRC_Reg[DdrcId]->ByteMap),               pAmbaDDRC_Reg[DdrcId]->ByteMap);
        printf("DRAM_lpddr4DqWriteDlyParam   (0x%x)->[0x%x] = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->TrainDqWriteDly,       PHYSICAL_DDRC_ADDR(DdrcId, &pAmbaDDRC_Reg[DdrcId]->TrainDqWriteDly),       pAmbaDDRC_Reg[DdrcId]->TrainDqWriteDly);
        printf("DRAM_lpddr4DqReadDlyParam    (0x%x)->[0x%x] = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->TrainDqReadDly,        PHYSICAL_DDRC_ADDR(DdrcId, &pAmbaDDRC_Reg[DdrcId]->TrainDqReadDly),        pAmbaDDRC_Reg[DdrcId]->TrainDqReadDly);
        printf("DRAM_lpddr4DqsWriteDlyParam  (0x%x)->[0x%x] = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->TrainDqsWriteDly,      PHYSICAL_DDRC_ADDR(DdrcId, &pAmbaDDRC_Reg[DdrcId]->TrainDqsWriteDly),      pAmbaDDRC_Reg[DdrcId]->TrainDqsWriteDly);
        printf("DRAM_lpddr4CkCaWriteDlyParam (0x%x)->[0x%x] = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->TrainCkCaWriteDly,     PHYSICAL_DDRC_ADDR(DdrcId, &pAmbaDDRC_Reg[DdrcId]->TrainCkCaWriteDly),     pAmbaDDRC_Reg[DdrcId]->TrainCkCaWriteDly);
        printf("DRAM_lpddr4DqCaVrefParam     (0x%x)->[0x%x] = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->TrainDqCaVref,         PHYSICAL_DDRC_ADDR(DdrcId, &pAmbaDDRC_Reg[DdrcId]->TrainDqCaVref),         pAmbaDDRC_Reg[DdrcId]->TrainDqCaVref);
        printf("DLL_Setting0                 (0x%x)->[0x%x] = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->DdrioDll0,             PHYSICAL_DDRC_ADDR(DdrcId, &pAmbaDDRC_Reg[DdrcId]->DdrioDll0),             pAmbaDDRC_Reg[DdrcId]->DdrioDll0);
        printf("DLL_Setting1                 (0x%x)->[0x%x] = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->DdrioDll1,             PHYSICAL_DDRC_ADDR(DdrcId, &pAmbaDDRC_Reg[DdrcId]->DdrioDll1),             pAmbaDDRC_Reg[DdrcId]->DdrioDll1);
        printf("DLL_Setting2                 (0x%x)->[0x%x] = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->DdrioDll2,             PHYSICAL_DDRC_ADDR(DdrcId, &pAmbaDDRC_Reg[DdrcId]->DdrioDll2),             pAmbaDDRC_Reg[DdrcId]->DdrioDll2);
        printf("DLL_Setting3                 (0x%x)->[0x%x] = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->DdrioDll3,             PHYSICAL_DDRC_ADDR(DdrcId, &pAmbaDDRC_Reg[DdrcId]->DdrioDll3),             pAmbaDDRC_Reg[DdrcId]->DdrioDll3);
        printf("DLL_CtrlSel0                 (0x%x)->[0x%x] = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->DdrioDllCtrlSel0,      PHYSICAL_DDRC_ADDR(DdrcId, &pAmbaDDRC_Reg[DdrcId]->DdrioDllCtrlSel0),      pAmbaDDRC_Reg[DdrcId]->DdrioDllCtrlSel0);
        printf("DLL_CtrlSel1                 (0x%x)->[0x%x] = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->DdrioDllCtrlSel1,      PHYSICAL_DDRC_ADDR(DdrcId, &pAmbaDDRC_Reg[DdrcId]->DdrioDllCtrlSel1),      pAmbaDDRC_Reg[DdrcId]->DdrioDllCtrlSel1);
        printf("DLL_CtrlSel2                 (0x%x)->[0x%x] = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->DdrioDllCtrlSel2,      PHYSICAL_DDRC_ADDR(DdrcId, &pAmbaDDRC_Reg[DdrcId]->DdrioDllCtrlSel2),      pAmbaDDRC_Reg[DdrcId]->DdrioDllCtrlSel2);
        printf("DLL_CtrlSel3                 (0x%x)->[0x%x] = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->DdrioDllCtrlSel3,      PHYSICAL_DDRC_ADDR(DdrcId, &pAmbaDDRC_Reg[DdrcId]->DdrioDllCtrlSel3),      pAmbaDDRC_Reg[DdrcId]->DdrioDllCtrlSel3);
        printf("DLL_CtrlSelMisc              (0x%x)->[0x%x] = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->DdrioDllCtrlMisc,      PHYSICAL_DDRC_ADDR(DdrcId, &pAmbaDDRC_Reg[DdrcId]->DdrioDllCtrlMisc),      pAmbaDDRC_Reg[DdrcId]->DdrioDllCtrlMisc);
        printf("DLL_SyncCtrlSel0             (0x%x)->[0x%x] = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->DdrioDllSyncCtrlSel0,  PHYSICAL_DDRC_ADDR(DdrcId, &pAmbaDDRC_Reg[DdrcId]->DdrioDllSyncCtrlSel0),  pAmbaDDRC_Reg[DdrcId]->DdrioDllSyncCtrlSel0);
        printf("DLL_SyncCtrlSel1             (0x%x)->[0x%x] = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->DdrioDllSyncCtrlSel1,  PHYSICAL_DDRC_ADDR(DdrcId, &pAmbaDDRC_Reg[DdrcId]->DdrioDllSyncCtrlSel1),  pAmbaDDRC_Reg[DdrcId]->DdrioDllSyncCtrlSel1);
        printf("DLL_SyncCtrlSel2             (0x%x)->[0x%x] = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->DdrioDllSyncCtrlSel2,  PHYSICAL_DDRC_ADDR(DdrcId, &pAmbaDDRC_Reg[DdrcId]->DdrioDllSyncCtrlSel2),  pAmbaDDRC_Reg[DdrcId]->DdrioDllSyncCtrlSel2);
        printf("DLL_SyncCtrlSel3             (0x%x)->[0x%x] = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->DdrioDllSyncCtrlSel3,  PHYSICAL_DDRC_ADDR(DdrcId, &pAmbaDDRC_Reg[DdrcId]->DdrioDllSyncCtrlSel3),  pAmbaDDRC_Reg[DdrcId]->DdrioDllSyncCtrlSel3);
    }
}

static UINT32 IoDiag_DdrGetDllValIndex(UINT8 Param)
{
    UINT32 i, RetVal = ~OK;

    for (i = sizeof(DiagDdrPhaseSetting); i > 0U; i--) {
        if (Param == DiagDdrPhaseSetting[i - 1U]) {
            RetVal = (i - 1U);
            break;
        }
    }

    return RetVal;
}

static void IoDiag_DdrSetDLL(UINT32 HostID, UINT32 DllByte, UINT32 DllValue)
{
    UINT32 DllRegVal = pAmbaDDRC_Reg[HostID]->DdrioDll0;

    switch (DllByte) {
    case AMBA_DDRC_DLL_BYTE_0:
        DllRegVal &= 0xffff00UL;
        DllRegVal |= (DllValue & 0xffUL);
        break;

    case AMBA_DDRC_DLL_BYTE_1:
        DllRegVal &= 0xff00ffUL;
        DllRegVal |= (DllValue & 0xffUL) << 8UL;
        break;

    case AMBA_DDRC_DLL_BYTE_2:
        DllRegVal &= 0x00ffffUL;
        DllRegVal |= (DllValue & 0xffUL) << 16UL;
        break;

    default:
        DllRegVal &= 0xffffffUL;
        break;
    }

    pAmbaDDRC_Reg[HostID]->DdrioDll0 = DllRegVal;
    pAmbaDDRC_Reg[HostID]->DdrioDll1 = DllRegVal;
    pAmbaDDRC_Reg[HostID]->DdrioDll2 = DllRegVal;
    pAmbaDDRC_Reg[HostID]->DdrioDll3 = DllRegVal;
}

static void IoDiag_DdrQuickShmoo(UINT32 ArgCount, char * const *pArgVector)
{
    UINT32 DllRegVal;
    UINT32 DllByte = 0xffffffffUL, DllValue = 0xffffffffUL;
    UINT32 TimeInterval = 1000UL;
    UINT32 i, Index;
    UINT32 HostId = 0U;

    if (ArgCount < 6U) {
        IoDiag_DdrCmdUsage(pArgVector);
    } else {
        HostId = strtol(pArgVector[3],NULL,0);
        if (HostId < 2) {
            DllRegVal = pAmbaDDRC_Reg[HostId]->DdrioDll0;
            if (strncmp(pArgVector[4], "sync", 4U) == 0) {
                DllByte = AMBA_DDRC_DLL_BYTE_1;
                DllValue = (DllRegVal & 0x00ff00UL) >> 8UL;
            }
            if (strncmp(pArgVector[4], "read", 4U) == 0) {
                DllByte = AMBA_DDRC_DLL_BYTE_0;
                DllValue = (DllRegVal & 0x0000ffUL);
            }
            if (strncmp(pArgVector[4], "write", 5U) == 0) {
                DllByte = AMBA_DDRC_DLL_BYTE_2;
                DllValue = (DllRegVal & 0xff0000UL) >> 16UL;
            }

            Index = IoDiag_DdrGetDllValIndex(DllValue);

            if (ArgCount > 6U) {
                TimeInterval = strtol(pArgVector[6],NULL,0);
            }

            if (strncmp(pArgVector[5], "+", 1U) == 0) {
                for (i = Index; i < sizeof(DiagDdrPhaseSetting); i ++) {
                    IoDiag_DdrSetDLL(HostId, DllByte, DiagDdrPhaseSetting[i]);
                    printf("[DDR Quick Shmoo]DLL = 0x%06X\n", pAmbaDDRC_Reg[HostId]->DdrioDll0);
                    (void)AmbaKAL_TaskSleep(TimeInterval);
                }
            }
            if (strncmp(pArgVector[5], "-", 1U) == 0) {
                for (i = Index + 1U; i > 0U; i --) {
                    IoDiag_DdrSetDLL(HostId, DllByte, DiagDdrPhaseSetting[i - 1U]);
                    printf("[DDR Quick Shmoo]DLL = 0x%06X\n", pAmbaDDRC_Reg[HostId]->DdrioDll0);
                    (void)AmbaKAL_TaskSleep(TimeInterval);
                }
            }
        } else {
            IoDiag_DdrCmdUsage(pArgVector);
        }
    }
}

static void IoDiag_DdrLp4ReadDlyShmoo(UINT32 ArgCount, char * const *pArgVector)
{

    UINT32 TimeInterval = 1000UL;
    INT32 i = 0;
    UINT32 CurRegValue = 0, NewRegValue = 0;
    UINT32 DdrcId = AMBA_DDRC0;

    DdrcId = strtol(pArgVector[3],NULL,0);
    CurRegValue = ((pAmbaDDRC_Reg[DdrcId]->TrainDqReadDly) & 0x1EU) >> 1;

    if (ArgCount > 5U) {
        TimeInterval = strtol(pArgVector[5],NULL,0);
    }
    if (strncmp(pArgVector[4], "+", 1U) == 0) {
        for (i = CurRegValue; i < 16; i++) {
            NewRegValue = 0x08421UL;
            NewRegValue = NewRegValue | (i << 16U) | (i << 11U) | (i << 6U) | (i << 1U);
            pAmbaDDRC_Reg[DdrcId]->TrainDqReadDly = NewRegValue;
            NewRegValue = ((pAmbaDDRC_Reg[DdrcId]->TrainDqReadDly) & 0x1EU) >> 1;
            printf("[DDRC%d][Read Delay]TrainDqReadDly = 0x%08x (value = 0x%x)\n", DdrcId, (pAmbaDDRC_Reg[DdrcId]->TrainDqReadDly), NewRegValue);
            (void)AmbaKAL_TaskSleep(TimeInterval);
        }
    }
    if (strncmp(pArgVector[4], "-", 1U) == 0) {
        for (i = CurRegValue; i >= 0; i--) {
            NewRegValue = 0x08421UL;
            NewRegValue = NewRegValue | (i << 16U) | (i << 11U) | (i << 6U) | (i << 1U);
            pAmbaDDRC_Reg[DdrcId]->TrainDqReadDly = NewRegValue;
            NewRegValue = ((pAmbaDDRC_Reg[DdrcId]->TrainDqReadDly) & 0x1EU) >> 1;
            printf("[DDRC%d][Read Delay]TrainDqReadDly = 0x%08x (value = 0x%x)\n", DdrcId, (pAmbaDDRC_Reg[DdrcId]->TrainDqReadDly), NewRegValue);
            (void)AmbaKAL_TaskSleep(TimeInterval);
        }
    }
}

static void IoDiag_DdrLp4VrefShmoo(UINT32 ArgCount, char * const *pArgVector)
{

    UINT32 TimeInterval = 1000UL;
    INT32 i = 0;
    UINT32 CurRegValue = 0, NewRegValue = 0;
    UINT32 DdrcId = AMBA_DDRC0;

    DdrcId = strtol(pArgVector[3],NULL,0);
    CurRegValue = (pAmbaDDRC_Reg[DdrcId]->TrainDqCaVref) & 0x3fU; /* bit[6] is reserved */

    if (ArgCount > 5U) {
        TimeInterval = strtol(pArgVector[5],NULL,0);
    }

    if (strncmp(pArgVector[4], "+", 1U) == 0) {
        for (i = CurRegValue; i < 64; i++) {
            NewRegValue = (i << 21U) | (i << 14U) | (i << 7U) | (i << 0U);
            pAmbaDDRC_Reg[DdrcId]->TrainDqCaVref = NewRegValue;
            NewRegValue = (pAmbaDDRC_Reg[DdrcId]->TrainDqCaVref) & 0x3FU;
            printf("[DDRC%d][DQ Vref]TrainDqVref = 0x%08x (value = 0x%x)\n", DdrcId, (pAmbaDDRC_Reg[DdrcId]->TrainDqCaVref), NewRegValue);
            (void)AmbaKAL_TaskSleep(TimeInterval);
        }
    }
    if (strncmp(pArgVector[4], "-", 1U) == 0) {
        for (i = CurRegValue; i >= 0; i--) {
            NewRegValue = (i << 21U) | (i << 14U) | (i << 7U) | (i << 0U);
            pAmbaDDRC_Reg[DdrcId]->TrainDqCaVref = NewRegValue;
            NewRegValue = (pAmbaDDRC_Reg[DdrcId]->TrainDqCaVref) & 0x3FU;
            printf("[DDRC%d][DQ Vref]TrainDqVref = 0x%08x (value = 0x%x)\n", DdrcId, (pAmbaDDRC_Reg[DdrcId]->TrainDqCaVref), NewRegValue);
            (void)AmbaKAL_TaskSleep(TimeInterval);
        }
    }
}

static void IoDiag_DdrLp4Mr14Shmoo(UINT32 ArgCount, char * const *pArgVector)
{
    UINT32 TimeInterval = 1000UL;
    INT32 i = 0;
    UINT32 CurRegValue = 0, NewRegValue = 0;
    UINT32 DdrcId = AMBA_DDRC0;

    DdrcId = strtol(pArgVector[3],NULL,0);
    pAmbaDDRC_Reg[DdrcId]->ModeReg = 0x000e0000U;
    while (0x0U != ((pAmbaDDRC_Reg[DdrcId]->ModeReg) & 0x80000000U)) {
        ;
    }
    CurRegValue = (pAmbaDDRC_Reg[DdrcId]->ModeReg) & 0x3fU; /* bit[5:0] */

    if (ArgCount > 5U) {
        TimeInterval = strtol(pArgVector[5],NULL,0);
    }

    if (strncmp(pArgVector[4], "+", 1U) == 0) {
        for (i = CurRegValue; i < 51; i++) {
            NewRegValue = 0x010e0000UL | i;
            pAmbaDDRC_Reg[DdrcId]->ModeReg = NewRegValue;
            (void)AmbaKAL_TaskSleep(100);
            pAmbaDDRC_Reg[DdrcId]->ModeReg = 0x000e0000;
            while (0x0U != ((pAmbaDDRC_Reg[DdrcId]->ModeReg) & 0x80000000U)) {
                ;
            }
            printf("[LPDDR4_%d][MR14 Vref]Lpddr4DqVref = 0x%08x (value = 0x%x)\n", DdrcId, (pAmbaDDRC_Reg[DdrcId]->ModeReg), ((pAmbaDDRC_Reg[DdrcId]->ModeReg) & 0x3FU));
            (void)AmbaKAL_TaskSleep(TimeInterval);
        }
    }
    if (strncmp(pArgVector[4], "-", 1U) == 0) {
        for (i = CurRegValue; i >= 0; i--) {
            NewRegValue = 0x010e0000UL | i;
            pAmbaDDRC_Reg[DdrcId]->ModeReg = NewRegValue;
            (void)AmbaKAL_TaskSleep(100);
            pAmbaDDRC_Reg[DdrcId]->ModeReg = 0x000e0000;
            while (0x0U != ((pAmbaDDRC_Reg[DdrcId]->ModeReg) & 0x80000000U)) {
                ;
            }
            printf("[LPDDR4_%d][MR14 Vref]Lpddr4DqVref = 0x%08x (value = 0x%x)\n", DdrcId, (pAmbaDDRC_Reg[DdrcId]->ModeReg), ((pAmbaDDRC_Reg[DdrcId]->ModeReg) & 0x3FU));
            (void)AmbaKAL_TaskSleep(TimeInterval);

        }
    }
}

static void IoDiag_DdrLp4Mr12Shmoo(UINT32 ArgCount, char * const *pArgVector)
{
    UINT32 TimeInterval = 1000UL;
    INT32 i = 0;
    UINT32 CurRegValue = 0, NewRegValue = 0;
    UINT32 DdrcId = AMBA_DDRC0;

    DdrcId = strtol(pArgVector[3],NULL,0);
    (pAmbaDDRC_Reg[DdrcId]->ModeReg) = 0x000c0000;
    while (0x0U != ((pAmbaDDRC_Reg[DdrcId]->ModeReg) & 0x80000000U)) {
        ;
    }
    CurRegValue = (pAmbaDDRC_Reg[DdrcId]->ModeReg) & 0x3fU; /* bit[5:0] */

    if (ArgCount > 5U) {
        TimeInterval = strtol(pArgVector[5],NULL,0);
    }

    if (strncmp(pArgVector[4], "+", 1U) == 0) {
        for (i = CurRegValue; i < 51; i++) {
            NewRegValue = 0x010c0000UL | i;
            pAmbaDDRC_Reg[DdrcId]->ModeReg = NewRegValue;
            (void)AmbaKAL_TaskSleep(100);
            pAmbaDDRC_Reg[DdrcId]->ModeReg = 0x000c0000;
            while (0x0U != ((pAmbaDDRC_Reg[DdrcId]->ModeReg) & 0x80000000U)) {
                ;
            }
            printf("[LPDDR4_%d][MR12 Vref]Lpddr4CaVref = 0x%08x (value = 0x%x)\n", DdrcId, (pAmbaDDRC_Reg[DdrcId]->ModeReg), ((pAmbaDDRC_Reg[DdrcId]->ModeReg) & 0x3FU));
            (void)AmbaKAL_TaskSleep(TimeInterval);
        }
    }
    if (strncmp(pArgVector[4], "-", 1U) == 0) {
        for (i = CurRegValue; i >= 0; i--) {
            NewRegValue = 0x010c0000UL | i;
            pAmbaDDRC_Reg[DdrcId]->ModeReg = NewRegValue;
            (void)AmbaKAL_TaskSleep(100);
            pAmbaDDRC_Reg[DdrcId]->ModeReg = 0x000c0000;
            while (0x0U != ((pAmbaDDRC_Reg[DdrcId]->ModeReg) & 0x80000000U)) {
                ;
            }
            printf("[LPDDR4_%d][MR12 Vref]Lpddr4CaVref = 0x%08x (value = 0x%x)\n", DdrcId, (pAmbaDDRC_Reg[DdrcId]->ModeReg), ((pAmbaDDRC_Reg[DdrcId]->ModeReg) & 0x3FU));
            (void)AmbaKAL_TaskSleep(TimeInterval);

        }
    }
}

static void IoDiag_DdrLp4WrtieDlyShmoo(UINT32 ArgCount, char * const *pArgVector)
{

    UINT32 TimeInterval = 1000UL;
    INT32 i = 0;
    UINT32 CurRegValue = 0, NewRegValue = 0;
    UINT32 DdrcId = AMBA_DDRC0;

    DdrcId = strtol(pArgVector[3],NULL,0);
    CurRegValue = (pAmbaDDRC_Reg[DdrcId]->TrainDqWriteDly) & 0x7fU;

    if (ArgCount > 5U) {
        TimeInterval = strtol(pArgVector[5],NULL,0);
    }

    if (strncmp(pArgVector[4], "+", 1U) == 0) {
        for (i = CurRegValue; i < 128; i++) {
            NewRegValue = 0xC0000000UL | (i << 21U) | (i << 14U) | (i << 7U) | (i << 0U);
            pAmbaDDRC_Reg[DdrcId]->TrainDqWriteDly = NewRegValue;
            printf("[DDRC%d][Write Delay]TrainWriteDly = 0x%08x (value = 0x%x)\n", DdrcId, pAmbaDDRC_Reg[DdrcId]->TrainDqWriteDly, ((pAmbaDDRC_Reg[DdrcId]->TrainDqWriteDly) & 0x7FU));
            (void)AmbaKAL_TaskSleep(TimeInterval);
        }
    }
    if (strncmp(pArgVector[4], "-", 1U) == 0) {
        for (i = CurRegValue; i >= 0; i--) {
            NewRegValue = 0xC0000000UL | (i << 21U) | (i << 14U) | (i << 7U) | (i << 0U);
            pAmbaDDRC_Reg[DdrcId]->TrainDqWriteDly = NewRegValue;
            printf("[DDRC%d][Write Delay]TrainWriteDly = 0x%08x (value = 0x%x)\n", DdrcId, pAmbaDDRC_Reg[DdrcId]->TrainDqWriteDly, ((pAmbaDDRC_Reg[DdrcId]->TrainDqWriteDly) & 0x7FU));
            (void)AmbaKAL_TaskSleep(TimeInterval);
        }
    }
}

int DoDramShmooDiag(int argc, char *argv[])
{
    uintptr_t virt_base;
    UINT8 HostId = 0U;

    if (NULL == pAmbaDDRC_Reg[0u]) {
        virt_base = mmap_device_io(0x1000, 0xdffe0800);
        if (virt_base == (uintptr_t) MAP_FAILED) {
            perror("mmap_device_io");
            return -1;
        }
        pAmbaDDRC_Reg[0] = (AMBA_DDRC_REG_s *)virt_base;

        virt_base = mmap_device_io(0x1000, 0xdffe0a00);
        if (virt_base == (uintptr_t) MAP_FAILED) {
            perror("mmap_device_io");
            return -1;
        }
        pAmbaDDRC_Reg[1] = (AMBA_DDRC_REG_s *)virt_base;

    }

    if (NULL == pAmbaDRAMC_Reg) {
        virt_base = mmap_device_io(0x1000, 0xdffe1000);
        if (virt_base == (uintptr_t) MAP_FAILED) {
            perror("mmap_device_io");
            return -1;
        }
        pAmbaDRAMC_Reg = (AMBA_DRAMC_REG_s *)virt_base;
    }

    if (argc < 3U) {
        IoDiag_DdrCmdUsage(argv);
    } else {
        if (strncmp(argv[2], "dump", 4U) == 0) {
            IoDiag_DdrDump();
        } else if (strncmp(argv[2], "shmoo", 5U) == 0) {
            IoDiag_DdrQuickShmoo(argc, argv);
        } else if (strncmp(argv[2], "set", 3U) == 0) {
            //IoDiag_DdrSetValue(argc, argv);
        } else if (strncmp(argv[2], "statis", 7U) == 0) {
            if (argc > 3U) {
                if (strncmp(argv[3], "reset", 5U) == 0) {
                    IoDiag_DramResetStatis();
                }
            } else {
                IoDiag_DramShowStatisInfo();
            }
        } else if (strncmp(argv[2], "lp4rdly", 7U) == 0) {
            IoDiag_DdrLp4ReadDlyShmoo(argc, argv);
        } else if (strncmp(argv[2], "lp4vref", 7U) == 0) {
            IoDiag_DdrLp4VrefShmoo(argc, argv);
        } else if (strncmp(argv[2], "lp4wdly", 7U) == 0) {
            IoDiag_DdrLp4WrtieDlyShmoo(argc, argv);
        } else if (strncmp(argv[2], "lp4mr14", 7U) == 0) {
            IoDiag_DdrLp4Mr14Shmoo(argc, argv);
        } else if (strncmp(argv[2], "lp4mr12", 7U) == 0) {
            IoDiag_DdrLp4Mr12Shmoo(argc, argv);
        } else {
            IoDiag_DdrCmdUsage(argv);
        }
    }
    return 0;
}

