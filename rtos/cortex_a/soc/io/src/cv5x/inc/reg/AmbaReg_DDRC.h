/**
 *  @file AmbaReg_DDRC.h
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
 *  @details Definitions & Constants for DDR Controller APIs.
 *
 */

#ifndef AMBA_REG_DDRC_H
#define AMBA_REG_DDRC_H

#ifndef AMBA_TYPES_H
#include "AmbaTypes.h"
#endif

#ifndef AMBA_CORTEX_A76_H
#include "AmbaCortexA76.h"
#endif

#define AHOST_DDRC_REG_BASE_ADDR AMBA_CORTEX_A76_DDR_CTRL_ALL_BASE_ADDR
#define HOST0_DDRC_REG_BASE_ADDR AMBA_CORTEX_A76_DDR_CTRL0_BASE_ADDR
#define HOST1_DDRC_REG_BASE_ADDR AMBA_CORTEX_A76_DDR_CTRL1_BASE_ADDR
#define HOST_OFFSET (AMBA_CORTEX_A76_DDR_CTRL1_BASE_ADDR - AMBA_CORTEX_A76_DDR_CTRL0_BASE_ADDR)

/*
 * DDRC: DRAM Controller All Registers
 */
typedef struct {
    UINT32  DramType:           2;  /* [1:0]: 0 = LPDDR4, 1 - LPDDR5, Others = Reserved */
    UINT32  DramSize:           4;  /* [5:2]: 0 = 2 Gb, 1 = 3 Gb, 2 = 4 Gb, 3 = 6Gb, 4 = 8 Gb, 5 = 12 Gb, 6 = 16 Gb, 7 = 24 Gb, 8 = 32 Gb, Others = Reserved */
    UINT32  BusMode:            1;  /* [6]: Reserved */
    UINT32  AccuMaskMode:       1;  /* [7]: 1:accumulated Mask mode(recommended value is 1) */
    UINT32  MaxPostedRefCredit: 4;  /* [11:8]: Max number of posted auto-refreshes */
    UINT32  MinPostedRefCredit: 4;  /* [15:12]: Min number of posted auto-refreshes */
    UINT32  HighskewOffset:     4;  /* [19:16]: HIGHSKEW FIFO's offset for read pointer(recommended value is -1) */
    UINT32  Lp5BankGrpMode:     1;  /* [20]: 0 = BG mode, 1 = 16 bank mode */
    UINT32  IndDqDqsReadEnable: 1;  /* [21]: 0 = READ EN for DQ is same as DQS, 1 = READ EN for DQ is independent of DQS */
    UINT32  WckSuspendMode:     1;  /* [22]: For LP5 mode only, unused in LP4, 1 = Issue CAS WCK SUS command when no active commands in pipeline. Next read will be at least 4 cycles away once this command is issued. */
    UINT32  UseMsbForChbMrw:    1;  /* [23]: 0 = DQ[7:0] Die0/1 Channel A, DQ[15:8] Die0/1 Channel B, 1 = DQ[7:0] Die 0 channel A, DQ[15:8] Die 1 channel A, DQ[23:16] Die 0 Channel B, DQ[31:24] Die 1 Channel B */
    UINT32  DieMapMode:         1;  /* [24]: 1 = die id is at MSB of addressable range {die, row, bank, col}, 0 = die id is after bank {row, die, bank, col} */
    UINT32  BgMapMode:          1;  /* [25]: LP5 only, 0 = Bank group is {addr[13], addr[6]}, 1 = Bank group is {addr[7], addr[6]} */
    UINT32  DuelDieEn:          1;  /* [26]: 0 = single die, 1 = dual die enable */
    UINT32  Reserved1:          5;  /* [31:27]: Reserved */
} AMBA_DDRC_CONFIG0_REG_s;

typedef struct {
    volatile UINT32      Ctrl;                   /* 0x000 */
    volatile AMBA_DDRC_CONFIG0_REG_s      Config0;                /* 0x004 */
    volatile UINT32      Config1;                /* 0x008 */
    volatile UINT32      Timing0;                /* 0x00C */
    volatile UINT32      Timing1;                /* 0x010 */
    volatile UINT32      Timing2;                /* 0x014 */
    volatile UINT32      Timing3;                /* 0x018 */
    volatile UINT32      Timing4;                /* 0x01C */
    volatile UINT32      DuelDieTiming;          /* 0x020 */
    volatile UINT32      RefreshTiming;          /* 0x024 */
    volatile UINT32      Lp5Timing;              /* 0x028 */
    volatile UINT32      InitCtrl;               /* 0x02C */
    volatile UINT32      ModeReg;                /* 0x030 */
    volatile UINT32      SelfRefresh;            /* 0x034 */
    volatile UINT32      RevdSpace;              /* 0x038 */
    volatile UINT32      ByteMap;                /* 0x03C */
    volatile UINT32      MpcTrainWriteData[8];   /* 0x040-0x05C */
    volatile UINT32      WMask;                  /* 0x060 */
    volatile UINT32      ChaMpcTrainReadData[8]; /* 0x064-0x080 */
    volatile UINT32      ChbMpcTrainReadData[8]; /* 0x084-0x0A0 */
    volatile UINT32      RMask0;                 /* 0x0A4 */
    volatile UINT32      RMask1;                 /* 0x0A8 */
    volatile UINT32      Uinstruction[6];        /* 0x0AC-0x0C0 */
    volatile UINT32      TrnSwScratchpad;        /* 0x0C4 */
    volatile UINT32      WdqsTiming;             /* 0x0C8 */
    volatile UINT32      MpcRdDataClr;           /* 0x0CC */
    volatile UINT32      IoCtrl;                 /* 0x0D0 */
    volatile UINT32      DdrcMisc1;              /* 0x0D4 */
    volatile UINT32      Reserved0[10];          /* 0x0D8-0x0FC */
    volatile UINT32      DtteReg0;               /* 0x100 */
    volatile UINT32      DtteReg1;               /* 0x104 */
    volatile UINT32      DtteDly0[2];            /* 0x108-0x10c */
    volatile UINT32      DtteDly1[2];            /* 0x110-0x114 */
    volatile UINT32      DtteDly2[2];            /* 0x118-0x11c */
    volatile UINT32      DtteDly3[2];            /* 0x120-0x124 */
    volatile UINT32      DtteDlyDm;              /* 0x128 */
    volatile UINT32      DtteVref;               /* 0x12c */
    volatile UINT32      DtteCmd;                /* 0x130 */
    volatile UINT32      DtteDlyMap;             /* 0x134 */
    volatile UINT32      DtteCpInfoVref;         /* 0x138 */
    volatile UINT32      DtteCpInfoVfine;        /* 0x13c */
    volatile UINT32      DtteCpInfoDmDly;        /* 0x140 */
    volatile UINT32      DtteCpInfoDlyB0A;       /* 0x144 */
    volatile UINT32      DtteCpInfoDlyB0B;       /* 0x148 */
    volatile UINT32      DtteCpInfoDlyB1A;       /* 0x14c */
    volatile UINT32      DtteCpInfoDlyB1B;       /* 0x150 */
    volatile UINT32      DtteCpInfoDlyB2A;       /* 0x154 */
    volatile UINT32      DtteCpInfoDlyB2B;       /* 0x158 */
    volatile UINT32      DtteCpInfoDlyB3A;       /* 0x15c */
    volatile UINT32      DtteCpInfoDlyB3B;       /* 0x160 */
    volatile UINT32      WriteVref0;             /* 0x164 */
    volatile UINT32      WriteVref1;             /* 0x168 */
    volatile UINT32      DtteTiming;             /* 0x16c */
    volatile UINT32      Reserved1[36];          /* 0x170-0x1FC */
    volatile UINT32      DllCtrlSbc[4];          /* 0x200-0x20c */
    volatile UINT32      DllCtrlSel0D0;          /* 0x210 */
    volatile UINT32      DllCtrlSel0D1;          /* 0x214 */
    volatile UINT32      DllCtrlSel1D0;          /* 0x218 */
    volatile UINT32      DllCtrlSel1D1;          /* 0x21c */
    volatile UINT32      DllCtrlSel2D0;          /* 0x220 */
    volatile UINT32      DllCtrlSel2D1;          /* 0x224 */
    volatile UINT32      Byte0Die0Dly[6];        /* 0x228-0x23c */
    volatile UINT32      Byte1Die0Dly[6];        /* 0x240-0x254 */
    volatile UINT32      Byte2Die0Dly[6];        /* 0x258-0x26c */
    volatile UINT32      Byte3Die0Dly[6];        /* 0x270-0x284 */
    volatile UINT32      Byte0Die1Dly[6];        /* 0x288-0x29c */
    volatile UINT32      Byte1Die1Dly[6];        /* 0x2a0-0x2b4 */
    volatile UINT32      Byte2Die1Dly[6];        /* 0x2b8-0x2cc */
    volatile UINT32      Byte3Die1Dly[6];        /* 0x2d0-0x2e4 */
    volatile UINT32      CkDly;                  /* 0x2e8 */
    volatile UINT32      CaDlyCoarse;            /* 0x2ec */
    volatile UINT32      CaDlyFineChADie0[2];    /* 0x2f0-0x2f4 */
    volatile UINT32      CaDlyFineChADie1[2];    /* 0x2f8-0x2fc */
    volatile UINT32      CaDlyFineChBDie0[2];    /* 0x300-0x304 */
    volatile UINT32      CaDlyFineChBDie1[2];    /* 0x308-0x30c */
    volatile UINT32      CsDlyFine;              /* 0x310 */
    volatile UINT32      CkeDlyCoarse;           /* 0x314 */
    volatile UINT32      CkeDlyFine;             /* 0x318 */
    volatile UINT32      DqDutyCtrl[4];          /* 0x31c-0x328 */
    volatile UINT32      DqsDutyCtrl[4];         /* 0x32c-0x338 */
    volatile UINT32      WckDutyCtrl[4];         /* 0x33c-0x348 */
    volatile UINT32      CaDutyCtrlChA;          /* 0x34c */
    volatile UINT32      CaDutyCtrlChB;          /* 0x350 */
    volatile UINT32      CkDutyCtrlChA;          /* 0x354 */
    volatile UINT32      CkDutyCtrlChB;          /* 0x358 */
    volatile UINT32      DqsTestIrcv;            /* 0x35c */
    volatile UINT32      PadCtrl;                /* 0x360 */
    volatile UINT32      DqsPuPd;                /* 0x364 */
    volatile UINT32      Rsvd;                   /* 0x368 */
    volatile UINT32      Zctrl;                  /* 0x36c */
    volatile UINT32      CaPadCtrl;              /* 0x370 */
    volatile UINT32      DqPadCtrl;              /* 0x374 */
    volatile UINT32      RdVref0;                /* 0x378 */
    volatile UINT32      RdVref1;                /* 0x37c */
    volatile UINT32      IbiasCtrl;              /* 0x380 */
    volatile UINT32      ZctrlStatus;            /* 0x384 */
    volatile UINT32      DllStatus0;             /* 0x388 */
    volatile UINT32      DllStatus1;             /* 0x38c */
    volatile UINT32      PhyDebug;               /* 0x390 */
    volatile UINT32      Reserved2;              /* 0x394 */
    volatile UINT32      RttDebug0;              /* 0x398 */
    volatile UINT32      RttDebug1;              /* 0x39c */
    volatile UINT32      RttDebug2;              /* 0x3a0 */
    volatile UINT32      DdrcStauts;             /* 0x3a4 */
    volatile UINT32      Reserved3[6];           /* 0x3a8-0x3bc */
    volatile UINT32      DdrcMisc2;              /* 0x3c0 */
    volatile UINT32      ReadLinkEccStatus[4];   /* 0x3c4~0x3d0 */
} AMBA_DDRC_REG_s;

#define DDRC_OFST_CTRL                      offsetof(AMBA_DDRC_REG_s, Ctrl)
#define DDRC_OFST_CONFIG0                   offsetof(AMBA_DDRC_REG_s, Config0)
#define DDRC_OFST_CONFIG1                   offsetof(AMBA_DDRC_REG_s, Config1)
#define DDRC_OFST_TIMING0                   offsetof(AMBA_DDRC_REG_s, Timing0)
#define DDRC_OFST_TIMING1                   offsetof(AMBA_DDRC_REG_s, Timing1)
#define DDRC_OFST_TIMING2                   offsetof(AMBA_DDRC_REG_s, Timing2)
#define DDRC_OFST_TIMING3                   offsetof(AMBA_DDRC_REG_s, Timing3)
#define DDRC_OFST_TIMING4                   offsetof(AMBA_DDRC_REG_s, Timing4)
#define DDRC_OFST_DUELDIETIMING             offsetof(AMBA_DDRC_REG_s, DuelDieTiming)
#define DDRC_OFST_REFRESHTIMING             offsetof(AMBA_DDRC_REG_s, RefreshTiming)
#define DDRC_OFST_LP5TIMING                 offsetof(AMBA_DDRC_REG_s, Lp5Timing)
#define DDRC_OFST_INITCTRL                  offsetof(AMBA_DDRC_REG_s, InitCtrl)
#define DDRC_OFST_MODEREG                   offsetof(AMBA_DDRC_REG_s, ModeReg)
#define DDRC_OFST_SELFREFRESH               offsetof(AMBA_DDRC_REG_s, SelfRefresh)
#define DDRC_OFST_REVDSPACE                 offsetof(AMBA_DDRC_REG_s, RevdSpace)
#define DDRC_OFST_BYTEMAP                   offsetof(AMBA_DDRC_REG_s, ByteMap)
#define DDRC_OFST_MPCTRAINWRITEDATA(x)      offsetof(AMBA_DDRC_REG_s, MpcTrainWriteData[x])
#define DDRC_OFST_WMASK                     offsetof(AMBA_DDRC_REG_s, WMask)
#define DDRC_OFST_CHAMPCTRAINREADDATA(x)    offsetof(AMBA_DDRC_REG_s, ChaMpcTrainReadData[x])
#define DDRC_OFST_CHBMPCTRAINREADDATA(x)    offsetof(AMBA_DDRC_REG_s, ChbMpcTrainReadData[x])
#define DDRC_OFST_RMASK0                    offsetof(AMBA_DDRC_REG_s, RMask0)
#define DDRC_OFST_RMASK1                    offsetof(AMBA_DDRC_REG_s, RMask1)
#define DDRC_OFST_UINSTRUCTION(x)           offsetof(AMBA_DDRC_REG_s, Uinstruction[x])
#define DDRC_OFST_TRNSWSCRATCHPAD           offsetof(AMBA_DDRC_REG_s, TrnSwScratchpad)
#define DDRC_OFST_WDQSTIMING                offsetof(AMBA_DDRC_REG_s, WdqsTiming)
#define DDRC_OFST_MPCRDDATACLR              offsetof(AMBA_DDRC_REG_s, MpcRdDataClr)
#define DDRC_OFST_IOCTRL                    offsetof(AMBA_DDRC_REG_s, IoCtrl)
#define DDRC_OFST_DDRCMISC1                 offsetof(AMBA_DDRC_REG_s, DdrcMisc1)
#define DDRC_OFST_RESERVED0(x)              offsetof(AMBA_DDRC_REG_s, Reserved0[x])
#define DDRC_OFST_DTTEREG0                  offsetof(AMBA_DDRC_REG_s, DtteReg0)
#define DDRC_OFST_DTTEREG1                  offsetof(AMBA_DDRC_REG_s, DtteReg1)
#define DDRC_OFST_DTTEDLY0(x)               offsetof(AMBA_DDRC_REG_s, DtteDly0[x])
#define DDRC_OFST_DTTEDLY1(x)               offsetof(AMBA_DDRC_REG_s, DtteDly1[x])
#define DDRC_OFST_DTTEDLY2(x)               offsetof(AMBA_DDRC_REG_s, DtteDly2[x])
#define DDRC_OFST_DTTEDLY3(x)               offsetof(AMBA_DDRC_REG_s, DtteDly3[x])
#define DDRC_OFST_DTTEDLYDM                 offsetof(AMBA_DDRC_REG_s, DtteDlyDm)
#define DDRC_OFST_DTTEVREF                  offsetof(AMBA_DDRC_REG_s, DtteVref)
#define DDRC_OFST_DTTECMD                   offsetof(AMBA_DDRC_REG_s, DtteCmd)
#define DDRC_OFST_DTTEDLYMAP                offsetof(AMBA_DDRC_REG_s, DtteDlyMap)
#define DDRC_OFST_DTTECPINFOVREF            offsetof(AMBA_DDRC_REG_s, DtteCpInfoVref)
#define DDRC_OFST_DTTECPINFOVFINE           offsetof(AMBA_DDRC_REG_s, DtteCpInfoVfine)
#define DDRC_OFST_DTTECPINFODMDLY           offsetof(AMBA_DDRC_REG_s, DtteCpInfoDmDly)
#define DDRC_OFST_DTTECPINFODLYB0A          offsetof(AMBA_DDRC_REG_s, DtteCpInfoDlyB0A)
#define DDRC_OFST_DTTECPINFODLYB0B          offsetof(AMBA_DDRC_REG_s, DtteCpInfoDlyB0B)
#define DDRC_OFST_DTTECPINFODLYB1A          offsetof(AMBA_DDRC_REG_s, DtteCpInfoDlyB1A)
#define DDRC_OFST_DTTECPINFODLYB1B          offsetof(AMBA_DDRC_REG_s, DtteCpInfoDlyB1B)
#define DDRC_OFST_DTTECPINFODLYB2A          offsetof(AMBA_DDRC_REG_s, DtteCpInfoDlyB2A)
#define DDRC_OFST_DTTECPINFODLYB2B          offsetof(AMBA_DDRC_REG_s, DtteCpInfoDlyB2B)
#define DDRC_OFST_DTTECPINFODLYB3A          offsetof(AMBA_DDRC_REG_s, DtteCpInfoDlyB3A)
#define DDRC_OFST_DTTECPINFODLYB3B          offsetof(AMBA_DDRC_REG_s, DtteCpInfoDlyB3B)
#define DDRC_OFST_WRITEVREF0                offsetof(AMBA_DDRC_REG_s, WriteVref0)
#define DDRC_OFST_WRITEVREF1                offsetof(AMBA_DDRC_REG_s, WriteVref1)
#define DDRC_OFST_DTTETIMING                offsetof(AMBA_DDRC_REG_s, DtteTiming)
#define DDRC_OFST_RESERVED1(x)              offsetof(AMBA_DDRC_REG_s, Reserved1[x])
#define DDRC_OFST_DLLCTRLSBC(x)             offsetof(AMBA_DDRC_REG_s, DllCtrlSbc[x])
#define DDRC_OFST_DLLCTRLSEL0D0             offsetof(AMBA_DDRC_REG_s, DllCtrlSel0D0)
#define DDRC_OFST_DLLCTRLSEL0D1             offsetof(AMBA_DDRC_REG_s, DllCtrlSel0D1)
#define DDRC_OFST_DLLCTRLSEL1D0             offsetof(AMBA_DDRC_REG_s, DllCtrlSel1D0)
#define DDRC_OFST_DLLCTRLSEL1D1             offsetof(AMBA_DDRC_REG_s, DllCtrlSel1D1)
#define DDRC_OFST_DLLCTRLSEL2D0             offsetof(AMBA_DDRC_REG_s, DllCtrlSel2D0)
#define DDRC_OFST_DLLCTRLSEL2D1             offsetof(AMBA_DDRC_REG_s, DllCtrlSel2D1)
#define DDRC_OFST_BYTE0DIE0DLY(x)           offsetof(AMBA_DDRC_REG_s, Byte0Die0Dly[x])
#define DDRC_OFST_BYTE1DIE0DLY(x)           offsetof(AMBA_DDRC_REG_s, Byte1Die0Dly[x])
#define DDRC_OFST_BYTE2DIE0DLY(x)           offsetof(AMBA_DDRC_REG_s, Byte2Die0Dly[x])
#define DDRC_OFST_BYTE3DIE0DLY(x)           offsetof(AMBA_DDRC_REG_s, Byte3Die0Dly[x])
#define DDRC_OFST_BYTE0DIE1DLY(x)           offsetof(AMBA_DDRC_REG_s, Byte0Die1Dly[x])
#define DDRC_OFST_BYTE1DIE1DLY(x)           offsetof(AMBA_DDRC_REG_s, Byte1Die1Dly[x])
#define DDRC_OFST_BYTE2DIE1DLY(x)           offsetof(AMBA_DDRC_REG_s, Byte2Die1Dly[x])
#define DDRC_OFST_BYTE3DIE1DLY(x)           offsetof(AMBA_DDRC_REG_s, Byte3Die1Dly[x])
#define DDRC_OFST_CKDLY                     offsetof(AMBA_DDRC_REG_s, CkDly)
#define DDRC_OFST_CADLYCOARSE               offsetof(AMBA_DDRC_REG_s, CaDlyCoarse)
#define DDRC_OFST_CADLYFINECHADIE0(x)       offsetof(AMBA_DDRC_REG_s, CaDlyFineChADie0[x])
#define DDRC_OFST_CADLYFINECHADIE1(x)       offsetof(AMBA_DDRC_REG_s, CaDlyFineChADie1[x])
#define DDRC_OFST_CADLYFINECHBDIE0(x)       offsetof(AMBA_DDRC_REG_s, CaDlyFineChBDie0[x])
#define DDRC_OFST_CADLYFINECHBDIE1(x)       offsetof(AMBA_DDRC_REG_s, CaDlyFineChBDie1[x])
#define DDRC_OFST_CSDLYFINE                 offsetof(AMBA_DDRC_REG_s, CsDlyFine)
#define DDRC_OFST_CKEDLYCOARSE              offsetof(AMBA_DDRC_REG_s, CkeDlyCoarse)
#define DDRC_OFST_CKEDLYFINE                offsetof(AMBA_DDRC_REG_s, CkeDlyFine)
#define DDRC_OFST_DQDUTYCTRL(x)             offsetof(AMBA_DDRC_REG_s, DqDutyCtrl[x])
#define DDRC_OFST_DQSDUTYCTRL(x)            offsetof(AMBA_DDRC_REG_s, DqsDutyCtrl[x])
#define DDRC_OFST_WCKDUTYCTRL(x)            offsetof(AMBA_DDRC_REG_s, WckDutyCtrl[x])
#define DDRC_OFST_CADUTYCTRLCHA             offsetof(AMBA_DDRC_REG_s, CaDutyCtrlChA)
#define DDRC_OFST_CADUTYCTRLCHB             offsetof(AMBA_DDRC_REG_s, CaDutyCtrlChB)
#define DDRC_OFST_CKDUTYCTRLCHA             offsetof(AMBA_DDRC_REG_s, CkDutyCtrlChA)
#define DDRC_OFST_CKDUTYCTRLCHB             offsetof(AMBA_DDRC_REG_s, CkDutyCtrlChB)
#define DDRC_OFST_DQSTESTIRCV               offsetof(AMBA_DDRC_REG_s, DqsTestIrcv)
#define DDRC_OFST_PADCTRL                   offsetof(AMBA_DDRC_REG_s, PadCtrl)
#define DDRC_OFST_DQSPUPD                   offsetof(AMBA_DDRC_REG_s, DqsPuPd)
#define DDRC_OFST_RSVD                      offsetof(AMBA_DDRC_REG_s, Rsvd)
#define DDRC_OFST_ZCTRL                     offsetof(AMBA_DDRC_REG_s, Zctrl)
#define DDRC_OFST_CAPADCTRL                 offsetof(AMBA_DDRC_REG_s, CaPadCtrl)
#define DDRC_OFST_DQPADCTRL                 offsetof(AMBA_DDRC_REG_s, DqPadCtrl)
#define DDRC_OFST_RDVREF0                   offsetof(AMBA_DDRC_REG_s, RdVref0)
#define DDRC_OFST_RDVREF1                   offsetof(AMBA_DDRC_REG_s, RdVref1)
#define DDRC_OFST_IBIASCTRL                 offsetof(AMBA_DDRC_REG_s, IbiasCtrl)
#define DDRC_OFST_ZCTRLSTATUS               offsetof(AMBA_DDRC_REG_s, ZctrlStatus)
#define DDRC_OFST_DLLSTATUS0                offsetof(AMBA_DDRC_REG_s, DllStatus0)
#define DDRC_OFST_DLLSTATUS1                offsetof(AMBA_DDRC_REG_s, DllStatus1)
#define DDRC_OFST_PHYDEBUG                  offsetof(AMBA_DDRC_REG_s, PhyDebug)
#define DDRC_OFST_RESERVED2                 offsetof(AMBA_DDRC_REG_s, Reserved2)
#define DDRC_OFST_RTTDEBUG0                 offsetof(AMBA_DDRC_REG_s, RttDebug0)
#define DDRC_OFST_RTTDEBUG1                 offsetof(AMBA_DDRC_REG_s, RttDebug1)
#define DDRC_OFST_RTTDEBUG2                 offsetof(AMBA_DDRC_REG_s, RttDebug2)
#define DDRC_OFST_DDRCSTAUTS                offsetof(AMBA_DDRC_REG_s, DdrcStauts)
#define DDRC_OFST_RESERVED3(x)              offsetof(AMBA_DDRC_REG_s, Reserved3[x])
#define DDRC_OFST_DDRCMISC2                 offsetof(AMBA_DDRC_REG_s, DdrcMisc2)
#define DDRC_OFST_READLINKECC(x)            offsetof(AMBA_DDRC_REG_s, ReadLinkEccStatus[x])


/*
 * Defined in AmbaMmioBase.asm
 */
extern AMBA_DDRC_REG_s * pAmbaDDRC_Reg[2U];

#endif /* AMBA_REG_DDRC_H */
