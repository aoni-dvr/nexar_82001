/*
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
 * this Software to Ambarella International LP.
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
 */

#ifndef AMBA_REG_CPHY_H
#define AMBA_REG_CPHY_H

#ifndef AMBA_TYPES_H
#include "AmbaTypes.h"
#endif

/**
 *  Definitions for BIAS configuration
 */
typedef struct {
    UINT32  IDivSel:                    2;      /* [1:0] */
    UINT32  QuadExtForce:               1;      /* [2] */
    UINT32  Reserved0:                  1;      /* [3] Unused */
    UINT32  IResCntl:                   3;      /* [6:4] */
    UINT32  Reserved1:                  1;      /* [7] Unused */
    UINT32  IBgrSlTrim:                 4;      /* [11:8] */
    UINT32  IBgrIeTrim:                 4;      /* [15:12] */
    UINT32  Reserved2:                  16;     /* [31:16] Unused */
} AMBA_DCPHY_BIAS_CON0_REG_s;

typedef struct {
    UINT32  ILadderSel:                 3;      /* [2:0] */
    UINT32  Reserved0:                  1;      /* [3] Unused */
    UINT32  IBgrVrefSel:                2;      /* [5:4] */
    UINT32  Reserved1:                  2;      /* [7:6] Unused */
    UINT32  IVbgSel:                    2;      /* [9:8] */
    UINT32  Reserved2:                  22;     /* [31:10] Unused */
} AMBA_DCPHY_BIAS_CON1_REG_s;

typedef struct {
    UINT32  Reg645M:                    3;      /* [2:0] */
    UINT32  Reserved0:                  1;      /* [3] Unused */
    UINT32  Reg400M:                    3;      /* [6:4] */
    UINT32  Reserved1:                  1;      /* [7] Unused */
    UINT32  RegLp400M:                  3;      /* [10:8] */
    UINT32  Reserved2:                  1;      /* [11] Unused */
    UINT32  Reg325M:                    3;      /* [14:12] */
    UINT32  Reserved3:                  17;     /* [31:15] Unused */
} AMBA_DCPHY_BIAS_CON2_REG_s;

typedef struct {
    UINT32  Reserved0:                  12;     /* [11:0] Unused */
    UINT32  IntDtbMuxSel:               4;      /* [15:12] */
    UINT32  Reserved1:                  16;     /* [31:16] Unused */
} AMBA_DCPHY_BIAS_CON3_REG_s;

typedef struct {
    UINT32  IloopBackEn:                1;      /* [0] */
    UINT32  SResetn:                    1;      /* [1] */
    UINT32  IqResetnPolCon:             1;      /* [2] */
    UINT32  ILadderEn:                  1;      /* [3] */
    UINT32  IAtbSelBg:                  1;      /* [4] */
    UINT32  IMuxSel:                    2;      /* [6:5] */
    UINT32  IMuxSelLp:                  1;      /* [7] */
    UINT32  IClkSel:                    1;      /* [8] */
    UINT32  IClkEn:                     1;      /* [9] */
    UINT32  IBypassEn:                  1;      /* [10] */
    UINT32  QuadCntMan:                 5;      /* [15:11] */
    UINT32  Reserved0:                  16;     /* [31:16] Unused */
} AMBA_DCPHY_BIAS_CON4_REG_s;

/**
 *  Definitions for PLL(PLL0) configuration
 */
typedef struct {
    UINT32  P:                          6;      /* [5:0] */
    UINT32  Reserved0:                  2;      /* [7:6] Unused */
    UINT32  S:                          3;      /* [10:8] */
    UINT32  Reserved1:                  1;      /* [11] Unused */
    UINT32  PllEn:                      1;      /* [12] */
    UINT32  Reserved2:                  19;     /* [31:13] Unused */
} AMBA_DCPHY_PLL_CON0_REG_s;

typedef struct {
    UINT32  K:                          16;     /* [15:0] */
    UINT32  Reserved0:                  16;     /* [31:16] Unused */
} AMBA_DCPHY_PLL_CON1_REG_s;

typedef struct {
    UINT32  M:                          10;     /* [9:0] */
    UINT32  Reserved0:                  2;      /* [11:10] Unused */
    UINT32  FeedEn:                     1;      /* [12] */
    UINT32  FoutMask:                   1;      /* [13] */
    UINT32  MEscRefEn:                  1;      /* [14] */
    UINT32  UseSdwReg:                  1;      /* [15] */
    UINT32  Reserved1:                  16;     /* [31:16] Unused */
} AMBA_DCPHY_PLL_CON2_REG_s;

typedef struct {
    UINT32  Mfr:                        8;      /* [7:0] */
    UINT32  Mrr:                        6;      /* [13:8] */
    UINT32  Reserved0:                  18;     /* [31:14] Unused */
} AMBA_DCPHY_PLL_CON3_REG_s;

typedef struct {
    UINT32  Extafc:                     5;      /* [4:0] */
    UINT32  Reserved0:                  3;      /* [7:5] Unused */
    UINT32  AfcEnb:                     1;      /* [8] */
    UINT32  Bypass:                     1;      /* [9] */
    UINT32  Fsel:                       1;      /* [10] */
    UINT32  SscgEn:                     1;      /* [11] */
    UINT32  RSel:                       4;      /* [15:12] */
    UINT32  Reserved1:                  16;     /* [31:16] Unused */
} AMBA_DCPHY_PLL_CON4_REG_s;

typedef struct {
    UINT32  SelPf:                      2;      /* [1:0] */
    UINT32  Reserved0:                  2;      /* [3:2] Unused */
    UINT32  Icp:                        2;      /* [5:4] */
    UINT32  Reserved1:                  1;      /* [6] Unused */
    UINT32  SelExtclk:                  1;      /* [7] */
    UINT32  PLLEnableSel:               1;      /* [8] */
    UINT32  MsEnableClkSel:             1;      /* [9] */
    UINT32  ResetNSel:                  1;      /* [10] */
    UINT32  SelExtClkWord:              1;      /* [11] */
    UINT32  CalSelData:                 1;      /* [12] */
    UINT32  CalSelCk:                   1;      /* [13] */
    UINT32  FOutEn:                     1;      /* [14] */
    UINT32  ClkBuffEnSel:               1;      /* [15] */
    UINT32  Reserved2:                  16;     /* [31:16] Unused */
} AMBA_DCPHY_PLL_CON5_REG_s;

typedef struct {
    UINT32  BgrRefClkEn:                1;      /* [0] */
    UINT32  BitClkDiv8OutEnb:           1;      /* [1] */
    UINT32  BitClkDiv4OutEnb:           1;      /* [2] */
    UINT32  BitClkDiv2OutEnb:           1;      /* [3] */
    UINT32  MResetnIqSel:               1;      /* [4] */
    UINT32  InternalLoopbackEn:         1;      /* [5] */
    UINT32  D2aClkBufEnSel:             1;      /* [6] */
    UINT32  PllTestDivEnb:              1;      /* [7] */
    UINT32  WClkBufSftCnt:              4;      /* [11:8] */
    UINT32  ClkGatingDisable:           1;      /* [12] */
    UINT32  PllQxiTestSel:              1;      /* [13] */
    UINT32  Reserved0:                  18;     /* [31:14] Unused */
} AMBA_DCPHY_PLL_CON6_REG_s;

typedef struct {
    UINT32  PllLockCnt:                 16;     /* [15:0] */
    UINT32  Reserved0:                  16;     /* [31:16] Unused */
} AMBA_DCPHY_PLL_CON7_REG_s;

typedef struct {
    UINT32  PllStbCnt:                  16;     /* [15:0] */
    UINT32  Reserved0:                  16;     /* [31:16] Unused */
} AMBA_DCPHY_PLL_CON8_REG_s;

typedef struct {
    UINT32  BiasCnt:                    12;     /* [11:0] */
    UINT32  BiasCntEn:                  1;      /* [12] */
    UINT32  Reserved0:                  19;     /* [31:13] Unused */
} AMBA_DCPHY_PLL_CON9_REG_s;

typedef struct {
    UINT32  PllLock:                    1;      /* [0] */
    UINT32  Reserved0:                  31;     /* [31:1] Unused */
} AMBA_DCPHY_PLL_ST0_REG_s;

/**
 *  Definitions for MC(DPHY Master 0 Clock Lane) configuration
 */

typedef struct {
    UINT32  ResDn:                      4;      /* [3:0] */
    UINT32  ResUp:                      4;      /* [7:4] */
    UINT32  EdgeConEn:                  1;      /* [8] */
    UINT32  EdgeConDir:                 1;      /* [9] */
    UINT32  Reserved0:                  2;      /* [11:10] Unused */
    UINT32  EdgeCon:                    3;      /* [14:12] */
    UINT32  R2wClkLaneConfSel:          1;      /* [15] */
    UINT32  Reserved1:                  16;     /* [31:16] Unused */
} AMBA_DCPHY_MC_ANA_CON0_REG_s;

typedef struct {
    UINT32  HsVregAmpIcon:              2;      /* [1:0] */
    UINT32  Reserved0:                  30;     /* [31:2] Unused */
} AMBA_DCPHY_MC_ANA_CON2_REG_s;

typedef struct {
    UINT32  Reserved0:                  4;      /* [3:0] Unused */
    UINT32  TLpx:                       8;      /* [11:4] */
    UINT32  ClkHsTxSel:                 1;      /* [12] */
    UINT32  Reserved1:                  19;     /* [31:13] Unused */
} AMBA_DCPHY_MC_TIME_CON0_REG_s;

typedef struct {
    UINT32  TClkPrepare:                8;      /* [7:0] */
    UINT32  TClkZero:                   8;      /* [15:8] */
    UINT32  Reserved0:                  16;     /* [31:16] Unused */
} AMBA_DCPHY_MC_TIME_CON1_REG_s;

typedef struct {
    UINT32  TClkTrail:                  8;      /* [7:0] */
    UINT32  THsExit:                    8;      /* [15:8] */
    UINT32  Reserved0:                  16;     /* [31:16] Unused */
} AMBA_DCPHY_MC_TIME_CON2_REG_s;

typedef struct {
    UINT32  TClkPost:                   8;      /* [7:0] */
    UINT32  Reserved0:                  24;     /* [31:8] Unused */
} AMBA_DCPHY_MC_TIME_CON3_REG_s;

typedef struct {
    UINT32  Reserved0:                  1;      /* [0] Unused */
    UINT32  ClkInv:                     1;      /* [1] */
    UINT32  ClkGatingDisable:           1;      /* [2] */
    UINT32  Reserved1:                  2;      /* [4:3] Unused */
    UINT32  LptxSwap:                   1;      /* [5] */
    UINT32  Reserved2:                  26;     /* [31:6] Unused */
} AMBA_DCPHY_MC_DATA_CON0_REG_s;

typedef struct {
    UINT32  SkewCalEn:                  1;      /* [0] */
    UINT32  Reserved0:                  3;      /* [3:1] Unused */
    UINT32  SkewCalInitWaitTime:        4;      /* [7:4] */
    UINT32  SkewCalInitRunTime:         4;      /* [11:8] */
    UINT32  SkewCalRunTime:             4;      /* [15:12] */
    UINT32  Reserved1:                  16;     /* [31:16] Unused */
} AMBA_DCPHY_MC_DESKEW_CON0_REG_s;

typedef struct {
    UINT32  DbgStateHsTxWordStop:       1;      /* [0] */
    UINT32  DbgStateHsTxWordGo:         1;      /* [1] */
    UINT32  DbgStateHsTxWordSync:       1;      /* [2] */
    UINT32  DbgStateHsTxWordMode:       1;      /* [3] */
    UINT32  DbgStateHsTxWordPost:       1;      /* [4] */
    UINT32  DbgStateHsTxWordExit:       1;      /* [5] */
    UINT32  Reserved0:                  2;      /* [7:6] Unused */
    UINT32  DbgStateHsTxHsStop:         1;      /* [8] */
    UINT32  DbgStateHsTxHsRequest:      1;      /* [9] */
    UINT32  DbgStateHsTxHsPrepare:      1;      /* [10] */
    UINT32  DbgStateHsTxHsZero:         1;      /* [11] */
    UINT32  DbgStateHsTxHsMode:         1;      /* [12] */
    UINT32  DbgStateHsTxHsTrail:        1;      /* [13] */
    UINT32  DbgStateHsTxHsExit:         1;      /* [14] */
    UINT32  Reserved1:                  17;     /* [31:15] Unused */
} AMBA_DCPHY_MC_DBG_ST0_REG_s;

typedef struct {
    UINT32  DbgStateHsTxScInitStop:     1;      /* [0] */
    UINT32  DbgStateHsTxScInitRqst:     1;      /* [1] */
    UINT32  DbgStateHsTxScStop:         1;      /* [2] */
    UINT32  DbgStateHsTxScRqst:         1;      /* [3] */
    UINT32  Reserved0:                  4;      /* [7:4] Unused */
    UINT32  DbgStatePatGenIdle:         1;      /* [8] */
    UINT32  DbgStatePatGenEn:           1;      /* [9] */
    UINT32  Reserved1:                  22;     /* [31:10] Unused */
} AMBA_DCPHY_MC_DBG_ST1_REG_s;

typedef struct {
    UINT32  TxReadyHs:                  1;      /* [0] */
    UINT32  StopState:                  1;      /* [1] */
    UINT32  UlpsActiveNot:              1;      /* [2] */
    UINT32  Reserved0:                  5;      /* [7:3] Unused */
    UINT32  BistRxValidHs:              1;      /* [8] */
    UINT32  BistRxActiveHs:             1;      /* [9] */
    UINT32  BistStopState:              1;      /* [10] */
    UINT32  Reserved1:                  21;     /* [31:11] Unused */
} AMBA_DCPHY_MC_PPI_ST0_REG_s;

typedef struct {
    UINT32  BiasEn:                     1;      /* [0] */
    UINT32  LpTxEn:                     1;      /* [1] */
    UINT32  LpTxUlps:                   1;      /* [2] */
    UINT32  TxRegEn:                    1;      /* [3] */
    UINT32  TxHsEn:                     1;      /* [4] */
    UINT32  IntLbEn:                    1;      /* [5] */
    UINT32  Reserved0:                  26;     /* [31:6] Unused */
} AMBA_DCPHY_MC_TX_BIST_5_REG_s;

/**
 *  Definitions for CMD(COMBO Master 0 Data 0/1/2 Lane) configuration
 */

typedef struct {
    UINT32  DpdnSwap:                   1;      /* [0] */
    UINT32  LpcdHysSel:                 1;      /* [1] */
    UINT32  PulseRejEnb:                1;      /* [2] */
    UINT32  EnRstnSel:                  1;      /* [3] */
    UINT32  CntUlpshys:                 1;      /* [4] */
    UINT32  SelVref:                    1;      /* [5] */
    UINT32  SelEscpol:                  1;      /* [6] */
    UINT32  CntRejectionPulse:          1;      /* [7] */
    UINT32  DtbSel:                     3;      /* [10:8] */
    UINT32  Reserved0:                  1;      /* [11] Unused */
    UINT32  LpRxHysSel:                 2;      /* [13:12] */
    UINT32  Reserved1:                  18;     /* [31:14] Unused */
} AMBA_DCPHY_CMD_ANA_CON2_REG_s;

typedef struct {
    UINT32  IregCtl:                    2;      /* [1:0] */
    UINT32  VReg12ExtPwr:               1;      /* [2] */
    UINT32  Reserved0:                  1;      /* [3] Unused */
    UINT32  LpcdBiasControl:            2;      /* [5:4] */
    UINT32  Reserved1:                  2;      /* [7:6] Unused */
    UINT32  LpRxBiasControl:            2;      /* [9:8] */
    UINT32  Reserved2:                  2;      /* [11:10] Unused */
    UINT32  Sel520mv:                   2;      /* [13:12] */
    UINT32  UlpsHysSw:                  2;      /* [15:14] */
    UINT32  Reserved3:                  16;     /* [31:16] Unused */
} AMBA_DCPHY_CMD_ANA_CON3_REG_s;

typedef struct {
    UINT32  DataSwap:                   1;      /* [0] */
    UINT32  DataInv:                    1;      /* [1] */
    UINT32  ClkGatingDisable:           1;      /* [2] */
    UINT32  Reserved0:                  1;      /* [3] Unused */
    UINT32  SymbolSwap:                 1;      /* [4] */
    UINT32  LptxSwap:                   1;      /* [5] */
    UINT32  Reserved1:                  2;      /* [7:6] Unused */
    UINT32  TrigExitClkEn:              1;      /* [8] */
    UINT32  Reserved2:                  3;      /* [11:9] Unused */
    UINT32  ErrContentionDisable:       1;      /* [12] */
    UINT32  Reserved3:                  19;     /* [31:13] Unused */
} AMBA_DCPHY_CMD_DATA_CON0_REG_s;

typedef struct {
    UINT32  ProgSeqEn:                  1;      /* [0] */
    UINT32  Reserved0:                  31;     /* [31:1] Unused */
} AMBA_DCPHY_CMD_PSEQ_CON0_REG_s;

typedef struct {
    UINT32  ProgSeq:                    10;     /* [9:0] */
    UINT32  Reserved0:                  22;     /* [31:10] Unused */
} AMBA_DCPHY_CMD_PSEQ_CON1_REG_s;

typedef struct {
    UINT32  ProgSeq:                    16;     /* [15:0] */
    UINT32  Reserved0:                  16;     /* [31:16] Unused */
} AMBA_DCPHY_CMD_PSEQ_CON2_REG_s;

typedef struct {
    UINT32  ProgSeq:                    16;     /* [15:0] */
    UINT32  Reserved0:                  16;     /* [31:16] Unused */
} AMBA_DCPHY_CMD_PSEQ_CON3_REG_s;
typedef struct {
    UINT32  TestMode:                   3;      /* [2:0] */
    UINT32  TestIntLbEn:                1;      /* [3] */
    UINT32  TestDataSwap:               1;      /* [4] */
    UINT32  TestDataInv:                1;      /* [5] */
    UINT32  Reserved0:                  2;      /* [7:6] Unused */
    UINT32  TestPatternSel:             3;      /* [10:8] */
    UINT32  TestLbUserDataSel:          1;      /* [11] */
    UINT32  TestLbPrbsSel:              2;      /* [13:12] */
    UINT32  Reserved1:                  18;     /* [31:14] Unused */
} AMBA_DCPHY_CMD_TEST_CON1_REG_s;

typedef struct {
    UINT32  DbgStateHsTxWordStop:       1;      /* [0] */
    UINT32  DbgStateHsTxWordPrebegin:   1;      /* [1] */
    UINT32  DbgStateHsTxWordProgram:    1;      /* [2] */
    UINT32  DbgStateHsTxWordPreend:     1;      /* [3] */
    UINT32  DbgStateHsTxWordSync:       1;      /* [4] */
    UINT32  DbgStateHsTxWordMode:       1;      /* [5] */
    UINT32  DbgStateHsTxWordPost:       1;      /* [6] */
    UINT32  DbgStateHsTxWordExit:       1;      /* [7] */
    UINT32  DbgStateHsTxHsStop:         1;      /* [8] */
    UINT32  DbgStateHsTxHsRequest:      1;      /* [9] */
    UINT32  DbgStateHsTxHsPrepare:      1;      /* [10] */
    UINT32  DbgStateHsTxHsExit:         1;      /* [11] */
    UINT32  Reserved0:                  20;     /* [31:12] Unused */
} AMBA_DCPHY_CMD_C_DBG_ST0_REG_s;

/**
 *  Definitions for MD(DPHY Master 0 Data 3 Lane) configuration
 */

typedef struct {
    UINT32  DataSwap:                   1;      /* [0] */
    UINT32  DataInv:                    1;      /* [1] */
    UINT32  Reserved0:                  3;      /* [4:2] Unused */
    UINT32  LptxSwap:                   1;      /* [5] */
    UINT32  Reserved1:                  2;      /* [7:6] Unused */
    UINT32  TrigExitClkEn:              1;      /* [8] */
    UINT32  Reserved2:                  3;      /* [11:9] Unused */
    UINT32  ErrContentionDisable:       1;      /* [12] */
    UINT32  Reserved3:                  19;     /* [31:13] Unused */
} AMBA_DCPHY_MD_DATA_CON0_REG_s;

/**
 *  Definitions for SC(DPHY Slave 0 Clock Lane) configuration
 */

typedef struct {
    UINT32  HsRxCntRout:                3;      /* [2:0] */
    UINT32  SelXiClk:                   1;      /* [3] */
    UINT32  Reserved0:                  28;     /* [31:4] Unused */
} AMBA_DCPHY_SC_ANA_CON0_REG_s;

typedef struct {
    UINT32  RxTermSw:                   3;      /* [2:0] */
    UINT32  Reserved0:                  2;      /* [4:3] Unused */
    UINT32  HsUntermEn:                 1;      /* [5] */
    UINT32  Reserved1:                  1;      /* [6] Unused */
    UINT32  SkewcalClkSel:              1;      /* [7] */
    UINT32  ClkDiv1234Mc:               2;      /* [9:8] */
    UINT32  HsmodeManSel:               1;      /* [10] */
    UINT32  ClkTermEnMan:               1;      /* [11] */
    UINT32  Reserved2:                  3;      /* [14:12] Unused */
    UINT32  ClkHsRxEnableMan:           1;      /* [15] */
    UINT32  Reserved3:                  16;     /* [31:16] Unused */
} AMBA_DCPHY_SC_ANA_CON2_REG_s;

typedef struct {
    UINT32  AtbSelDmy:                  4;      /* [3:0] */
    UINT32  LpVref520Sw:                2;      /* [5:4] */
    UINT32  VrefBgrVddSel:              1;      /* [6] */
    UINT32  AtbSelVref:                 1;      /* [7] */
    UINT32  Reserved0:                  3;      /* [10:8] Unused */
    UINT32  Reg0511:                    1;      /* [11] */
    UINT32  ClkDblCtrl:                 2;      /* [13:12] */
    UINT32  PulseRejVer:                1;      /* [14] */
    UINT32  PhyChkCnt:                  1;      /* [15] */
    UINT32  Reserved1:                  16;     /* [31:16] Unused */
} AMBA_DCPHY_SC_ANA_CON4_REG_s;

typedef struct {
    UINT32  RxBiasCnt:                  10;     /* [9:0] */
    UINT32  Reserved0:                  5;      /* [14:10] Unused */
    UINT32  ClkMissDisable:             1;      /* [15] */
    UINT32  Reserved1:                  16;     /* [31:16] Unused */
} AMBA_DCPHY_SC_ANA_CON5_REG_s;

typedef struct {
    UINT32  TClkSettle:                 8;      /* [7:0] */
    UINT32  TClkMiss:                   4;      /* [11:8] */
    UINT32  Reserved0:                  20;     /* [31:12] Unused */
} AMBA_DCPHY_SC_TIME_CON0_REG_s;

typedef struct {
    UINT32  ClkMissDisable:             1;      /* [0] */
    UINT32  Reserved0:                  31;     /* [31:1] Unused */
} AMBA_DCPHY_SC_DATA_CON0_REG_s;

typedef struct {
    UINT32  TestIn:                     2;      /* [1:0] */
    UINT32  Reserved0:                  2;      /* [3:2] Unused */
    UINT32  TestOut:                    2;      /* [5:4] */
    UINT32  Reserved1:                  26;     /* [31:6] Unused */
} AMBA_DCPHY_SC_TEST_CON0_REG_s;

typedef struct {
    UINT32  TestMode:                   3;      /* [2:0] */
    UINT32  TestIntLbEn:                1;      /* [3] */
    UINT32  Reserved0:                  28;     /* [31:4] Unused */
} AMBA_DCPHY_SC_TEST_CON1_REG_s;

typedef struct {
    UINT32  DbgStateRxStop:             1;      /* [0] */
    UINT32  DbgStateRxUlpsRqst:         1;      /* [1] */
    UINT32  DbgStateRxUlps:             1;      /* [2] */
    UINT32  DbgStateRxUlpsExit:         1;      /* [3] */
    UINT32  DbgStateRxHsRqs:            1;      /* [4] */
    UINT32  DbgStateRxHsClk:            1;      /* [5] */
    UINT32  Reserved0:                  26;     /* [31:6] Unused */
} AMBA_DCPHY_SC_DBG_ST0_REG_s;

typedef struct {
    UINT32  DbgStateSettleIdl:          1;      /* [0] */
    UINT32  DbgStateSettleEn:           1;      /* [1] */
    UINT32  Reserved0:                  30;     /* [31:2] Unused */
} AMBA_DCPHY_SC_DBG_ST1_REG_s;

typedef struct {
    UINT32  RxClkActiveHs:              1;      /* [0] */
    UINT32  StopState:                  1;      /* [1] */
    UINT32  RxUlpClkNot:                1;      /* [2] */
    UINT32  UlpsActiveNot:              1;      /* [3] */
    UINT32  Reserved0:                  28;     /* [31:4] Unused */
} AMBA_DCPHY_SC_PPI_ST0_REG_s;

typedef struct {
    UINT32  BiasEn:                     1;      /* [0] */
    UINT32  TermEn:                     1;      /* [1] */
    UINT32  HsRxEn:                     1;      /* [2] */
    UINT32  HsRxSettle:                 1;      /* [3] */
    UINT32  LpRxEn:                     1;      /* [4] */
    UINT32  LpRxUlps:                   1;      /* [5] */
    UINT32  ClkMiss:                    1;      /* [6] */
    UINT32  Reserved0:                  25;     /* [31:7] Unused */
} AMBA_DCPHY_SC_ADI_ST0_REG_s;

/**
 *  Definitions for CSD(COMBO Slave 0 Data 0/1/2 Lane) configuration
 */

typedef struct {
    UINT32  HsRxCntRout:                3;      /* [2:0] */
    UINT32  SelXiClk:                   1;      /* [3] */
    UINT32  RxNcCoef:                   5;      /* [8:4] */
    UINT32  Reserved0:                  23;     /* [31:9] Unused */
} AMBA_DCPHY_CSD_ANA_CON0_REG_s;

typedef struct {
    UINT32  HsRxCapCnt:                 4;      /* [3:0] */
    UINT32  HsRxRselCnt:                5;      /* [8:4] */
    UINT32  HsRxFlatband:               1;      /* [9] */
    UINT32  HsRxBiasCon:                6;      /* [15:10] */
    UINT32  Reserved0:                  16;     /* [31:16] Unused */
} AMBA_DCPHY_CSD_ANA_CON1_REG_s;

typedef struct {
    UINT32  RxTermSw:                   3;      /* [2:0] */
    UINT32  Reserved0:                  2;      /* [4:3] Unused */
    UINT32  HsUntermEn:                 1;      /* [5] */
    UINT32  SkewCalExtForce:            1;      /* [6] */
    UINT32  RxDdrClkHsSel:              1;      /* [7] */
    UINT32  SkewDlySel:                 2;      /* [9:8] */
    UINT32  HsmodeManSel:               1;      /* [10] */
    UINT32  HsTermEnMan:                1;      /* [11] */
    UINT32  UdlySpdcnt:                 2;      /* [13:12] */
    UINT32  SelFdly:                    1;      /* [14] */
    UINT32  Reserved1:                  17;     /* [31:15] Unused */
} AMBA_DCPHY_CSD_ANA_CON2_REG_s;

typedef struct {
    UINT32  BsCntSg:                    4;      /* [3:0] */
    UINT32  PbiasCntSg:                 2;      /* [5:4] */
    UINT32  EnDesk:                     1;      /* [6] */
    UINT32  CksymPol:                   1;      /* [7] */
    UINT32  SelDly:                     3;      /* [10:8] */
    UINT32  SelCrcCode:                 1;      /* [11] */
    UINT32  Reserved0:                  20;     /* [31:12] Unused */
} AMBA_DCPHY_CSD_ANA_CON5_REG_s;

typedef struct {
    UINT32  HsmodeManSelCphy:           1;      /* [0] */
    UINT32  DataDeserEnCphyMan:         1;      /* [1] */
    UINT32  ClkHsRxEmableCphyMan:       1;      /* [2] */
    UINT32  ExtDlMuxEn:                 1;      /* [3] */
    UINT32  UiMaskCtrl:                 2;      /* [5:4] */
    UINT32  DlDataComp:                 2;      /* [7:6] */
    UINT32  DlCompCtrl:                 3;      /* [10:8] */
    UINT32  AfcMuxEn:                   1;      /* [11] */
    UINT32  SelEespol:                  1;      /* [12] */
    UINT32  HsRxAmpDvCnt:               1;      /* [13] */
    UINT32  HsUntermEnCphy:             1;      /* [14] */
    UINT32  PhyChkCnt:                  1;      /* [15] */
    UINT32  Reserved0:                  16;     /* [31:16] Unused */
} AMBA_DCPHY_CSD_ANA_CON6_REG_s;

typedef struct {
    UINT32  Reserved0:                  6;      /* [5:0] Unused */
    UINT32  ClkDblCtrl:                 2;      /* [7:6] */
    UINT32  AtbSelPowerIr:              2;      /* [9:8] */
    UINT32  AteEn:                      1;      /* [10] */
    UINT32  Reserved1:                  1;      /* [11] Unused */
    UINT32  AteHsRxSel:                 2;      /* [13:12] */
    UINT32  HsTermEnCphyMan:            1;      /* [14] */
    UINT32  HsSettleInCphyMan:          1;      /* [15] */
    UINT32  Reserved2:                  16;     /* [31:16] Unused */
} AMBA_DCPHY_CSD_ANA_CON7_REG_s;

typedef struct {
    UINT32  DataSwap:                   1;      /* [0] */
    UINT32  DataInv:                    1;      /* [1] */
    UINT32  Reserved0:                  2;      /* [3:2] Unused */
    UINT32  SymbolSwap:                 1;      /* [4] */
    UINT32  Reserved1:                  27;     /* [31:5] Unused */
} AMBA_DCPHY_CSD_DATA_CON0_REG_s;

typedef struct {
    UINT32  CrcForceCodeEn:             1;      /* [0] */
    UINT32  Reserved0:                  3;      /* [3:1] Unused */
    UINT32  CrcForceCode:               5;      /* [8:4] */
    UINT32  Reserved1:                  23;     /* [31:9] Unused */
} AMBA_DCPHY_CSD_CRC_CON0_REG_s;

typedef struct {
    UINT32  CrcAvgSel:                  2;      /* [1:0] */
    UINT32  CrcSotSyncErrChk:           1;      /* [2] */
    UINT32  CrcPolSel:                  1;      /* [3] */
    UINT32  CrcSettleMaskSel:           3;      /* [6:4] */
    UINT32  CrcPostGatingEn:            1;      /* [7] */
    UINT32  CrcPostGatingCnt:           8;      /* [15:8] */
    UINT32  Reserved0:                  16;     /* [31:16] Unused */
} AMBA_DCPHY_CSD_CRC_CON1_REG_s;

typedef struct {
    UINT32  CrcCodeTune:                4;      /* [3:0] */
    UINT32  CrcCodeTol:                 4;      /* [7:4] */
    UINT32  CrcCodeFix:                 1;      /* [8] */
    UINT32  Reserved0:                  3;      /* [11:9] Unused */
    UINT32  CrcBurstCnt:                2;      /* [13:12] */
    UINT32  Reserved1:                  18;     /* [31:14] Unused */
} AMBA_DCPHY_CSD_CRC_CON2_REG_s;

typedef struct {
    UINT32  TestMode:                   3;      /* [2:0] */
    UINT32  TestIntLbEn:                1;      /* [3] */
    UINT32  TestDataSwap:               1;      /* [4] */
    UINT32  TestDataInv:                1;      /* [5] */
    UINT32  Reserved0:                  2;      /* [7:6] Unused */
    UINT32  TestPatternSel:             3;      /* [10:8] */
    UINT32  TestLbUserDataSel:          1;      /* [11] */
    UINT32  TestLbPrbsSel:              2;      /* [13:12] */
    UINT32  TestLbPassSel:              2;      /* [15:14] */
    UINT32  Reserved1:                  16;     /* [31:16] Unused */
} AMBA_DCPHY_CSD_TEST_CON1_REG_s;

typedef struct {
    UINT32  MonPktHeadNxt:              16;     /* [15:0] */
    UINT32  Reserved0:                  16;     /* [31:16] Unused */
} AMBA_DCPHY_CSD_PKT_ST6_REG_s;

typedef struct {
    UINT32  MonPktHeadNxt:              16;     /* [15:0] */
    UINT32  Reserved0:                  16;     /* [31:16] Unused */
} AMBA_DCPHY_CSD_PKT_ST7_REG_s;

typedef struct {
    UINT32  MonPktHeadNxt:              16;     /* [15:0] */
    UINT32  Reserved0:                  16;     /* [31:16] Unused */
} AMBA_DCPHY_CSD_PKT_ST8_REG_s;

typedef struct {
    UINT32  MonPktSotSyncErrCnt:        8;      /* [7:0] */
    UINT32  MonPktSotSyncErrPre:        1;      /* [8] */
    UINT32  MonPktSotSyncErrCur:        1;      /* [9] */
    UINT32  MonPktSotSyncErrNxt:        1;      /* [10] */
    UINT32  Reserved0:                  21;     /* [31:11] Unused */
} AMBA_DCPHY_CSD_PKT_ST9_REG_s;

typedef struct {
    UINT32  MonPktSotSyncErrNum:        16;     /* [15:0] */
    UINT32  Reserved0:                  16;     /* [31:16] Unused */
} AMBA_DCPHY_CSD_PKT_ST10_REG_s;

typedef struct {
    UINT32  MonDecErrCnt:               8;      /* [7:0] */
    UINT32  MonDecErr:                  1;      /* [8] */
    UINT32  Reserved0:                  23;     /* [31:9] Unused */
} AMBA_DCPHY_CSD_PKT_ST11_REG_s;

typedef struct {
    UINT32  DbgStateHsRxStop:           1;      /* [0] */
    UINT32  DbgStateHsRxSync:           1;      /* [1] */
    UINT32  DbgStateHsRxMode:           1;      /* [2] */
    UINT32  DbgStateHsRxPost:           1;      /* [3] */
    UINT32  DbgStateHsRxSyncFail:       1;      /* [4] */
    UINT32  Reserved0:                  3;      /* [7:5] Unused */
    UINT32  DbgStateSettleIdle:         1;      /* [8] */
    UINT32  DbgStateSettleEn:           1;      /* [9] */
    UINT32  Reserved1:                  2;      /* [11:10] Unused */
    UINT32  DbgStatePatChkIdle:         1;      /* [12] */
    UINT32  DbgStatePatChkEn:           1;      /* [13] */
    UINT32  DbgStateWireStateIdle:      1;      /* [14] */
    UINT32  DbgStateWireStateSync:      1;      /* [15] */
    UINT32  Reserved2:                  16;     /* [31:16] Unused */
} AMBA_DCPHY_CSD_C_DBG_ST0_REG_s;

typedef struct {
    UINT32  BiasEn:                     1;      /* [0] */
    UINT32  TermEn:                     1;      /* [1] */
    UINT32  HsRxEn:                     1;      /* [2] */
    UINT32  HsRxSettle:                 1;      /* [3] */
    UINT32  LpRxEn:                     1;      /* [4] */
    UINT32  LpRxUlps:                   1;      /* [5] */
    UINT32  IntLbEn:                    1;      /* [6] */
    UINT32  CrcEn:                      1;      /* [7] */
    UINT32  CrcCodeEn:                  1;      /* [8] */
    UINT32  Reserved0:                  23;     /* [31:9] Unused */
} AMBA_DCPHY_CSD_C_ADI_ST0_REG_s;

/**
 *  Definitions for SD(DPHY Slave 0 Data 3 Lane) configuration
 */

typedef struct {
    UINT32  HsRxCntRout:                3;      /* [2:0] */
    UINT32  Reserved0:                  29;     /* [31:3] Unused */
} AMBA_DCPHY_SD_ANA_CON0_REG_s;

typedef struct {
    UINT32  RxTermSw:                   3;      /* [2:0] */
    UINT32  Reserved0:                  2;      /* [4:3] Unused */
    UINT32  HsUntermEn:                 1;      /* [5] */
    UINT32  SkewCalExtForce:            1;      /* [6] */
    UINT32  RxDdrClkHsSel:              1;      /* [7] */
    UINT32  SkewDlySel:                 2;      /* [9:8] */
    UINT32  HsmodeManSel:               1;      /* [10] */
    UINT32  HsTermEnMan:                1;      /* [11] */
    UINT32  UdlySpdcnt:                 2;      /* [13:12] */
    UINT32  SelFdly:                    1;      /* [14] */
    UINT32  ResetnCfgSel:               1;      /* [15] */
    UINT32  Reserved1:                  16;     /* [31:16] Unused */
} AMBA_DCPHY_SD_ANA_CON2_REG_s;

typedef struct {
    UINT32  DataSwap:                   1;      /* [0] */
    UINT32  DataInv:                    1;      /* [1] */
    UINT32  Reserved0:                  30;     /* [31:2] Unused */
} AMBA_DCPHY_SD_DATA_CON0_REG_s;

typedef struct {
    UINT32  TestMode:                   3;      /* [2:0] */
    UINT32  TestIntLbEn:                1;      /* [3] */
    UINT32  TestDataSwap:               1;      /* [4] */
    UINT32  TestDataInv:                1;      /* [5] */
    UINT32  Reserved0:                  2;      /* [7:6] Unused */
    UINT32  TestPatternSel:             2;      /* [9:8] */
    UINT32  Reserved1:                  2;      /* [11:10] Unused */
    UINT32  TestLbPrbsSel:              1;      /* [12] */
    UINT32  Reserved2:                  1;      /* [13] Unused */
    UINT32  TestLbPassSel:              2;      /* [15:14] */
    UINT32  Reserved3:                  16;     /* [31:16] Unused */
} AMBA_DCPHY_SD_TEST_CON1_REG_s;

typedef struct {
    UINT32  MonPktHeadNxt:              16;     /* [15:0] */
    UINT32  Reserved0:                  16;     /* [31:16] Unused */
} AMBA_DCPHY_SD_PKT_ST4_REG_s;

typedef struct {
    UINT32  MonPktHeadNxt:              16;     /* [15:0] */
    UINT32  Reserved0:                  16;     /* [31:16] Unused */
} AMBA_DCPHY_SD_PKT_ST5_REG_s;

typedef struct {
    UINT32  MonPktSotSyncErrCnt:        8;      /* [7:0] */
    UINT32  MonPktSotSyncErrPre:        1;      /* [8] */
    UINT32  MonPktSotSyncErrCur:        1;      /* [9] */
    UINT32  MonPktSotSyncErrNxt:        1;      /* [10] */
    UINT32  Reserved0:                  21;     /* [31:11] Unused */
} AMBA_DCPHY_SD_PKT_ST6_REG_s;

typedef struct {
    UINT32  MonPktSotSyncErrNum:        16;     /* [15:0] */
    UINT32  Reserved0:                  16;     /* [31:16] Unused */
} AMBA_DCPHY_SD_PKT_ST7_REG_s;

/**
 *  Definitions for Common configuration
 */

typedef struct {
    UINT32  Enable:                     1;      /* [0] */
    UINT32  PhyReady:                   1;      /* [1] */
    UINT32  Reserved0:                  30;     /* [31:2] Unused */
} AMBA_DCPHY_GNR_CON0_REG_s;

typedef struct {
    UINT32  TPhyReady:                  16;     /* [15:0] */
    UINT32  Reserved0:                  16;     /* [31:16] Unused */
} AMBA_DCPHY_GNR_CON1_REG_s;

typedef struct {
    UINT32  TestIn:                     2;      /* [1:0] */
    UINT32  Reserved0:                  2;      /* [3:2] Unused */
    UINT32  TestOut:                    2;      /* [5:4] */
    UINT32  Reserved1:                  2;      /* [7:6] Unused */
    UINT32  TestLbPass:                 1;      /* [8] */
    UINT32  TestLbFail:                 1;      /* [9] */
    UINT32  Reserved2:                  22;     /* [31:10] Unused */
} AMBA_DCPHY_TEST_CON0_REG_s;

typedef struct {
    UINT32  TestLbPrbsSeed:             16;     /* [15:0] */
    UINT32  Reserved0:                  16;     /* [31:16] Unused */
} AMBA_DCPHY_MCDSD_TEST_CON2_REG_s; //MC,MD,SD

typedef struct {
    UINT32  TestLbUserData:             16;     /* [15:0] */
    UINT32  Reserved0:                  16;     /* [31:16] Unused */
} AMBA_DCPHY_MCDSD_TEST_CON3_REG_s;

typedef struct {
    UINT32  TestLbUserData:             16;     /* [15:0] */
    UINT32  Reserved0:                  16;     /* [31:16] Unused */
} AMBA_DCPHY_MCDSD_TEST_CON4_REG_s;

typedef struct {
    UINT32  MonPktHeadCur:              16;     /* [15:0] */
    UINT32  Reserved0:                  16;     /* [31:16] Unused */
} AMBA_DCPHY_PKT_CUR_REG_s;

/**
 *  Definitions for Master 0 Clock,Data0/1/2/3 Lane Configuration
 */

typedef struct {
    UINT32  Emp:                        2;      /* [1:0] */
    UINT32  Reserved0:                  2;      /* [3:2] Unused */
    UINT32  LptxSrCtrl:                 2;      /* [5:4] */
    UINT32  Reserved1:                  2;      /* [7:6] Unused */
    UINT32  LptxSrUp:                   2;      /* [9:8] */
    UINT32  AtbSelVdd:                  1;      /* [10] */
    UINT32  AtbSelGnd:                  1;      /* [11] */
    UINT32  DpdnSwap:                   1;      /* [12] */
    UINT32  VReg12ValidSel:             1;      /* [13] */
    UINT32  VReg12Valid:                1;      /* [14] */
    UINT32  IqResetnPolCon:             1;      /* [15] */
    UINT32  Reserved2:                  16;     /* [31:16] Unused */
} AMBA_DCPHY_M_ANA_CON1_REG_s;  //MC,MD,CMD

typedef struct {
    UINT32  TUlpsExit:                  10;     /* [9:0] */
    UINT32  Reserved0:                  22;     /* [31:10] Unused */
} AMBA_DCPHY_M_TIME_CON4_REG_s;

typedef struct {
    UINT32  TestMode:                   3;      /* [2:0] */
    UINT32  TestIntLbEn:                1;      /* [3] */
    UINT32  TestDataSwap:               1;      /* [4] */
    UINT32  TestDataInv:                1;      /* [5] */
    UINT32  Reserved0:                  2;      /* [7:6] Unused */
    UINT32  TestPatternSel:             2;      /* [9:8] */
    UINT32  Reserved1:                  2;      /* [11:10] Unused */
    UINT32  TestLbPrbsSel:              1;      /* [12] */
    UINT32  Reserved2:                  19;     /* [31:13] Unused */
} AMBA_DCPHY_MCD_TEST_CON1_REG_s; //MC,MD

typedef struct {
    UINT32  THsSettle:                  8;      /* [7:0] */
    UINT32  ClkSettleSel:               1;      /* [8] */
    UINT32  Reserved0:                  23;     /* [31:9] Unused */
} AMBA_DCPHY_M_BIST_CON0_REG_s;

typedef struct {
    UINT32  TErrSotSync:                8;      /* [7:0] */
    UINT32  Reserved0:                  24;     /* [31:8] Unused */
} AMBA_DCPHY_M_BIST_CON1_REG_s;

typedef struct {
    UINT32  ResDn:                      4;      /* [3:0] */
    UINT32  ResUp:                      4;      /* [7:4] */
    UINT32  EdgeConEn:                  1;      /* [8] */
    UINT32  EdgeConDir:                 1;      /* [9] */
    UINT32  Reserved0:                  2;      /* [11:10] Unused */
    UINT32  EdgeCon:                    3;      /* [14:12] */
    UINT32  Reserved1:                  17;     /* [31:15] Unused */
} AMBA_DCPHY_MD_ANA_CON0_REG_s; //MD, CMD

typedef struct {
    UINT32  TLpEntrySkew:               2;      /* [1:0] */
    UINT32  TLpExitSkew:                2;      /* [3:2] */
    UINT32  TLpx:                       8;      /* [11:4] */
    UINT32  ClkHsTxSel:                 1;      /* [12] */
    UINT32  Reserved0:                  19;     /* [31:13] Unused */
} AMBA_DCPHY_MD_TIME_CON0_REG_s;

typedef struct {
    UINT32  TTaGo:                      4;      /* [3:0] */
    UINT32  TTaGet:                     4;      /* [7:4] */
    UINT32  Reserved0:                  24;     /* [31:8] Unused */
} AMBA_DCPHY_MD_TIME_CON3_REG_s;

typedef struct {
    UINT32  DbgStateHsTxWordStop:       1;      /* [0] */
    UINT32  DbgStateHsTxWordGo:         1;      /* [1] */
    UINT32  DbgStateHsTxWordSync:       1;      /* [2] */
    UINT32  DbgStateHsTxWordMode:       1;      /* [3] */
    UINT32  DbgStateHsTxWordExit:       1;      /* [4] */
    UINT32  Reserved0:                  3;      /* [7:5] Unused */
    UINT32  DbgStateHsTxHsStop:         1;      /* [8] */
    UINT32  DbgStateHsTxHsRequest:      1;      /* [9] */
    UINT32  DbgStateHsTxHsPrepare:      1;      /* [10] */
    UINT32  DbgStateHsTxHsMode:         1;      /* [11] */
    UINT32  DbgStateHsTxHsTrail:        1;      /* [12] */
    UINT32  DbgStateHsTxHsExit:         1;      /* [13] */
    UINT32  Reserved1:                  18;     /* [31:14] Unused */
} AMBA_DCPHY_MD_DBG_ST0_REG_s;

typedef struct {
    UINT32  Reserved0:                  8;      /* [7:0] Unused */
    UINT32  DbgStatePatGenIdle:         1;      /* [8] */
    UINT32  DbgStatePatGenEn:           1;      /* [9] */
    UINT32  Reserved1:                  22;     /* [31:10] Unused */
} AMBA_DCPHY_MD_DBG_ST1_REG_s;

typedef struct {
    UINT32  DbgStateFsmTxStop:          1;      /* [0] */
    UINT32  DbgStateFsmTxLpRqst:        1;      /* [1] */
    UINT32  DbgStateFsmTxLpYield:       1;      /* [2] */
    UINT32  DbgStateFsmTxEscRqst:       1;      /* [3] */
    UINT32  DbgStateFsmTxEscGo:         1;      /* [4] */
    UINT32  DbgStateFsmTxEscCmd:        1;      /* [5] */
    UINT32  DbgStateFsmTxTrigger:       1;      /* [6] */
    UINT32  DbgStateFsmTxUlps:          1;      /* [7] */
    UINT32  DbgStateFsmTxUlpsCnt:       1;      /* [8] */
    UINT32  DbgStateFsmTxUlpsRdy:       1;      /* [9] */
    UINT32  DbgStateFsmTxLpdt:          1;      /* [10] */
    UINT32  DbgStateFsmTxMark:          1;      /* [11] */
    UINT32  DbgStateFsmTxTaRqst1:       1;      /* [12] */
    UINT32  DbgStateFsmTxTaYield:       1;      /* [13] */
    UINT32  DbgStateFsmTxTaRqst2:       1;      /* [14] */
    UINT32  DbgStateFsmTxTaGo:          1;      /* [15] */
    UINT32  Reserved0:                  16;     /* [31:16] Unused */
} AMBA_DCPHY_MD_DBG_ST2_REG_s;

typedef struct {
    UINT32  DbgStateFsmRxTaLook:        1;      /* [0] */
    UINT32  DbgStateFsmRxTaAck:         1;      /* [1] */
    UINT32  DbgStateFsmRxStop:          1;      /* [2] */
    UINT32  DbgStateFsmRxLpRqst:        1;      /* [3] */
    UINT32  DbgStateFsmRxLpYield:       1;      /* [4] */
    UINT32  DbgStateFsmRxEscRqst:       1;      /* [5] */
    UINT32  DbgStateFsmRxEscCmd:        1;      /* [6] */
    UINT32  DbgStateFsmRxUlps:          1;      /* [7] */
    UINT32  DbgStateFsmRxLpdt:          1;      /* [8] */
    UINT32  DbgStateFsmRxWait:          1;      /* [9] */
    UINT32  DbgStateFsmRxTaRqst:        1;      /* [10] */
    UINT32  DbgStateFsmRxTaWait:        1;      /* [11] */
    UINT32  DbgStateFsmTxTaGet:         1;      /* [12] */
    UINT32  DbgStateFsmTxTaAck:         1;      /* [13] */
    UINT32  Reserved0:                  18;     /* [31:14] Unused */
} AMBA_DCPHY_MD_DBG_ST3_REG_s;

typedef struct {
    UINT32  TxReadyHs:                  1;      /* [0] */
    UINT32  TxReadyEsc:                 1;      /* [1] */
    UINT32  RxLpdtEsc:                  1;      /* [2] */
    UINT32  RxUlpsEsc:                  1;      /* [3] */
    UINT32  RxTriggerEsc:               4;      /* [7:4] */
    UINT32  RxValidEsc:                 1;      /* [8] */
    UINT32  Direction:                  1;      /* [9] */
    UINT32  StopState:                  1;      /* [10] */
    UINT32  UlpsActiveNot:              1;      /* [11] */
    UINT32  BistRxValidHs:              1;      /* [12] */
    UINT32  BistRxActiveHs:             1;      /* [13] */
    UINT32  BistStopState:              1;      /* [14] */
    UINT32  Reserved0:                  17;     /* [31:15] Unused */
} AMBA_DCPHY_MD_PPI_ST0_REG_s;

typedef struct {
    UINT32  BiasEn:                     1;      /* [0] */
    UINT32  LpTxEn:                     1;      /* [1] */
    UINT32  LpTxUlps:                   1;      /* [2] */
    UINT32  LpRxEn:                     1;      /* [3] */
    UINT32  LpRxUlps:                   1;      /* [4] */
    UINT32  LpcdEn:                     1;      /* [5] */
    UINT32  TxHsEn:                     1;      /* [6] */
    UINT32  IntLbEn:                    1;      /* [7] */
    UINT32  Reserved0:                  24;     /* [31:8] Unused */
} AMBA_DCPHY_MD_ADI_ST0_REG_s;

/**
 *  Definitions for COMBO Master/Slave 0 Data0/1/2/3 Lane Configuration
 */

typedef struct {
    UINT32  TestLbPrbsSeed:             2;      /* [1:0] */
    UINT32  Reserved0:                  30;     /* [31:2] Unused */
} AMBA_DCPHY_CMSD_TEST_CON2_REG_s;

typedef struct {
    UINT32  TestLbPrbsSeed:             16;     /* [15:0] */
    UINT32  Reserved0:                  16;     /* [31:16] Unused */
} AMBA_DCPHY_CMSD_TEST_CON3_REG_s;

typedef struct {
    UINT32  TestLbUserData:             10;     /* [9:0] */
    UINT32  Reserved0:                  22;     /* [31:10] Unused */
} AMBA_DCPHY_CMSD_TEST_CON4_REG_s;

typedef struct {
    UINT32  TestLbUserData:             16;     /* [15:0] */
    UINT32  Reserved0:                  16;     /* [31:16] Unused */
} AMBA_DCPHY_CMSD_TEST_CON5_REG_s;

typedef struct {
    UINT32  TestLbUserData:             16;     /* [15:0] */
    UINT32  Reserved0:                  16;     /* [31:16] Unused */
} AMBA_DCPHY_CMSD_TEST_CON6_REG_s;

typedef struct {
    UINT32  THsTrail:                   8;      /* [7:0] */
    UINT32  THsExit:                    8;      /* [15:8] */
    UINT32  Reserved0:                  16;     /* [31:16] Unused */
} AMBA_DCPHY_MSD_THS2_REG_s; //MD, SD, CMD, CSD

/**
 *  Definitions for Slave 0 Clock,Data0/1/2/3 Lane Configuration
 */

typedef struct {
    UINT32  HsRxCapCnt:                 4;      /* [3:0] */
    UINT32  HsRxRselCnt:                5;      /* [8:4] */
    UINT32  HsRxFlatband:               1;      /* [9] */
    UINT32  Reserved0:                  1;      /* [10] Unused */
    UINT32  HsRxBiasCon:                5;      /* [15:11] */
    UINT32  Reserved1:                  16;     /* [31:16] Unused */
} AMBA_DCPHY_SCD_ANA_CON1_REG_s; //SC,SD

typedef struct {
    UINT32  LpRxBiasCtrl:               2;      /* [1:0] */
    UINT32  CulpsHys:                   1;      /* [2] */
    UINT32  PulseRejEnb:                1;      /* [3] */
    UINT32  LpHysSw:                    2;      /* [5:4] */
    UINT32  LpEnPrechargeSel:           1;      /* [6] */
    UINT32  CntRejectPulse:             1;      /* [7] */
    UINT32  UlpsHysSwDphy:              3;      /* [10:8] */
    UINT32  SelEscpol:                  1;      /* [11] */
    UINT32  HsDelayCon:                 2;      /* [13:12] */
    UINT32  DpdnSwap:                   1;      /* [14] */
    UINT32  EnRstnSel:                  1;      /* [15] */
    UINT32  Reserved0:                  16;     /* [31:16] Unused */
} AMBA_DCPHY_S_ANA_CON3_REG_s; //SC, SD, CSD

typedef struct {
    UINT32  AtbSelDmy:                  4;      /* [3:0] */
    UINT32  AtbSelPowerIr:              2;      /* [5:4] */
    UINT32  Reserved0:                  5;      /* [10:6] Unused */
    UINT32  Reg0511:                    1;      /* [11] */
    UINT32  Reserved1:                  2;      /* [13:12] Unused */
    UINT32  PulseRejVer:                1;      /* [14] */
    UINT32  PhyChkCnt:                  1;      /* [15] */
    UINT32  Reserved2:                  16;     /* [31:16] Unused */
} AMBA_DCPHY_SD_ANA_CON4_REG_s; //SD, CSD

typedef struct {
    UINT32  THsSettle:                  8;      /* [7:0] */
    UINT32  ClkSettleSel:               1;      /* [8] */
    UINT32  Reserved0:                  23;     /* [31:9] Unused */
} AMBA_DCPHY_SD_TIME_CON0_REG_s;

typedef struct {
    UINT32  TErrSotSync:                8;      /* [7:0] */
    UINT32  Reserved0:                  24;     /* [31:8] Unused */
} AMBA_DCPHY_SD_TIME_CON1_REG_s;

typedef struct {
    UINT32  SkewCalEn:                  1;      /* [0] */
    UINT32  Reserved0:                  3;      /* [3:1] Unused */
    UINT32  SkewCalDesSel:              1;      /* [4] */
    UINT32  Reserved1:                  27;     /* [31:5] Unused */
} AMBA_DCPHY_SD_DESKEW_CON0_REG_s;

typedef struct {
    UINT32  SkewCalCompRunTime:         4;      /* [3:0] */
    UINT32  SkewCalCompWaitTime:        2;      /* [5:4] */
    UINT32  SkewCalCompTol:             2;      /* [7:6] */
    UINT32  SkewCalFinalCompTime:       8;      /* [15:8] */
    UINT32  Reserved0:                  16;     /* [31:16] Unused */
} AMBA_DCPHY_SD_DESKEW_CON1_REG_s;

typedef struct {
    UINT32  SkewCalClkCoarseSet:        5;      /* [4:0] */
    UINT32  Reserved0:                  3;      /* [7:5] Unused */
    UINT32  SkewCalDataCoarseSet:       5;      /* [12:8] */
    UINT32  Reserved1:                  19;     /* [31:13] Unused */
} AMBA_DCPHY_SD_DESKEW_CON2_REG_s;

typedef struct {
    UINT32  SkewCalClkFineRiseSet:      4;      /* [3:0] */
    UINT32  SkewCalClkFineFallSet:      4;      /* [7:4] */
    UINT32  SkewCalDataFineRiseSet:     4;      /* [11:8] */
    UINT32  SkewCalDataFineFallSet:     4;      /* [15:12] */
    UINT32  Reserved0:                  16;     /* [31:16] Unused */
} AMBA_DCPHY_SD_DESKEW_CON3_REG_s;

typedef struct {
    UINT32  SkewCalCoarseMaxSet:        5;      /* [4:0] */
    UINT32  Reserved0:                  3;      /* [7:5] Unused */
    UINT32  SkewCalFineMaxSet:          4;      /* [11:8] */
    UINT32  Reserved1:                  20;     /* [31:12] Unused */
} AMBA_DCPHY_SD_DESKEW_CON4_REG_s;

typedef struct {
    UINT32  MonSkewCalCoarseClk:        5;      /* [4:0] */
    UINT32  Reserved0:                  3;      /* [7:5] Unused */
    UINT32  MonSkewCalCoarseData:       5;      /* [12:8] */
    UINT32  MonSkewCalDone:             1;      /* [13] */
    UINT32  Reserved1:                  18;     /* [31:14] Unused */
} AMBA_DCPHY_SD_DESKEW_MON0_REG_s;

typedef struct {
    UINT32  MonSkewCalFineClkRise:      4;      /* [3:0] */
    UINT32  MonSkewCalFineClkFall:      4;      /* [7:4] */
    UINT32  MonSkewCalFineDataRise:     4;      /* [11:8] */
    UINT32  MonSkewCalFineDataFall:     4;      /* [15:12] */
    UINT32  Reserved0:                  16;     /* [31:16] Unused */
} AMBA_DCPHY_SD_DESKEW_MON1_REG_s;

typedef struct {
    UINT32  TLpEntrySkew:               2;      /* [1:0] */
    UINT32  TLpExitSkew:                2;      /* [3:2] */
    UINT32  TLpx:                       8;      /* [11:4] */
    UINT32  ClkHsTxSel:                 1;      /* [12] */
    UINT32  Reserved0:                  19;     /* [31:13] Unused */
} AMBA_DCPHY_SD_BIST_CON0_REG_s;

typedef struct {
    UINT32  THsPrepare:                 8;      /* [7:0] */
    UINT32  THsZero:                    8;      /* [15:8] */
    UINT32  Reserved0:                  16;     /* [31:16] Unused */
} AMBA_DCPHY_MSD_THS1_REG_s;

typedef struct {
    UINT32  MonPktNum:                  16;     /* [15:0] */
    UINT32  Reserved0:                  16;     /* [31:16] Unused */
} AMBA_DCPHY_SD_PKT_MON_CON0_REG_s;

typedef struct {
    UINT32  MonPktHeadPre:              16;     /* [15:0] */
    UINT32  Reserved0:                  16;     /* [31:16] Unused */
} AMBA_DCPHY_SD_PKT_PRE_REG_s;

typedef struct {
    UINT32  DbgStateHsRxStop:           1;      /* [0] */
    UINT32  DbgStateHsRxSync:           1;      /* [1] */
    UINT32  DbgStateHsRxMode:           1;      /* [2] */
    UINT32  DbgStateHsRxSyncFail:       1;      /* [3] */
    UINT32  DbgStateHsRxSkewCal:        1;      /* [4] */
    UINT32  Reserved0:                  3;      /* [7:5] Unused */
    UINT32  DbgStateSettleIdle:         1;      /* [8] */
    UINT32  DbgStateSettleEn:           1;      /* [9] */
    UINT32  Reserved1:                  2;      /* [11:10] Unused */
    UINT32  DbgStatePatChkIdle:         1;      /* [12] */
    UINT32  DbgStatePatChkEn:           1;      /* [13] */
    UINT32  Reserved2:                  18;     /* [31:14] Unused */
} AMBA_DCPHY_SD_DBG_ST0_REG_s;

typedef struct {
    UINT32  DbgStateEscRxLpStop:        1;      /* [0] */
    UINT32  DbgStateEscRxLpRqst:        1;      /* [1] */
    UINT32  DbgStateEscRxLpYield:       1;      /* [2] */
    UINT32  DbgStateEscRxEscRqst:       1;      /* [3] */
    UINT32  DbgStateEscRxEscCmd:        1;      /* [4] */
    UINT32  DbgStateEscRxUlps:          1;      /* [5] */
    UINT32  DbgStateEscRxWait:          1;      /* [6] */
    UINT32  DbgStateEscRxHsRqst:        1;      /* [7] */
    UINT32  DbgStateEscRxHsMode:        1;      /* [8] */
    UINT32  DbgStateEscRxEscCmdErr:     1;      /* [9] */
    UINT32  Reserved0:                  22;     /* [31:10] Unused */
} AMBA_DCPHY_SD_DBG_ST1_REG_s;

typedef struct {
    UINT32  RxValidHs:                  1;      /* [0] */
    UINT32  RxActiveHs:                 1;      /* [1] */
    UINT32  RxUlpsEsc:                  1;      /* [2] */
    UINT32  StopState:                  1;      /* [3] */
    UINT32  UlpsActiveNot:              1;      /* [4] */
    UINT32  ErrEsc:                     1;      /* [5] */
    UINT32  ErrControl:                 1;      /* [6] */
    UINT32  Reserved0:                  1;      /* [7] Unused */
    UINT32  BistTxReadyHs:              1;      /* [8] */
    UINT32  BistStopState:              1;      /* [9] */
    UINT32  Reserved1:                  22;     /* [31:10] Unused */
} AMBA_DCPHY_SD_PPI_ST0_REG_s;

typedef struct {
    UINT32  BiasEn:                     1;      /* [0] */
    UINT32  TermEn:                     1;      /* [1] */
    UINT32  HsRxEn:                     1;      /* [2] */
    UINT32  HsRxSettle:                 1;      /* [3] */
    UINT32  LpRxEn:                     1;      /* [4] */
    UINT32  LpRxUlps:                   1;      /* [5] */
    UINT32  IntLbEn:                    1;      /* [6] */
    UINT32  RxDdrClkEn:                 1;      /* [7] */
    UINT32  Reserved0:                  24;     /* [31:8] Unused */
} AMBA_DCPHY_SD_ADI_ST0_REG_s;

/*
 * DCPhy: All Registers
 */
#if 0
typedef struct {
    //BIAS: 0x0000-0x00FF
    volatile AMBA_DCPHY_BIAS_CON0_REG_s             DcphyBiasCon0;          /* 0x0000(RW): Bias Configuration0 */
    volatile AMBA_DCPHY_BIAS_CON1_REG_s             DcphyBiasCon1;          /* 0x0004(RW): Bias Configuration1 */
    volatile AMBA_DCPHY_BIAS_CON2_REG_s             DcphyBiasCon2;          /* 0x0008(RW): Bias Configuration2 */
    volatile AMBA_DCPHY_BIAS_CON3_REG_s             DcphyBiasCon3;          /* 0x000C(RW): Bias Configuration3 */
    volatile AMBA_DCPHY_BIAS_CON4_REG_s             DcphyBiasCon4;          /* 0x0010(RW): Bias Configuration4 */
    volatile UINT32                                 Reserved0[59];          /* 0x014-0x0FC: Reserved */
    //PLL: 0x0100-0x01FF
    volatile AMBA_DCPHY_PLL_CON0_REG_s              DcphyPllCon0;           /* 0x100(RW): Pll Configuration0 */
    volatile AMBA_DCPHY_PLL_CON1_REG_s              DcphyPllCon1;           /* 0x104(RW): Pll Configuration1 */
    volatile AMBA_DCPHY_PLL_CON2_REG_s              DcphyPllCon2;           /* 0x108(RW): Pll Configuration2 */
    volatile AMBA_DCPHY_PLL_CON3_REG_s              DcphyPllCon3;           /* 0x10C(RW): Pll Configuration3 */
    volatile AMBA_DCPHY_PLL_CON4_REG_s              DcphyPllCon4;           /* 0x110(RW): Pll Configuration4 */
    volatile AMBA_DCPHY_PLL_CON5_REG_s              DcphyPllCon5;           /* 0x114(RW): Pll Configuration5 */
    volatile AMBA_DCPHY_PLL_CON6_REG_s              DcphyPllCon6;           /* 0x118(RW): Pll Configuration6 */
    volatile AMBA_DCPHY_PLL_CON7_REG_s              DcphyPllCon7;           /* 0x11C(RW): Pll Configuration7 */
    volatile AMBA_DCPHY_PLL_CON8_REG_s              DcphyPllCon8;           /* 0x120(RW): Pll Configuration8 */
    volatile AMBA_DCPHY_PLL_CON9_REG_s              DcphyPllCon9;           /* 0x124(RW): Pll Configuration9 */
    volatile UINT32                                 Reserved1[6];           /* 0x128-0x13C: Reserved */
    volatile AMBA_DCPHY_PLL_ST0_REG_s               DcphyPllSt0;            /* 0x140(RO): Pll Status0 */
    volatile UINT32                                 Reserved2[47];          /* 0x144-0x1FC: Reserved */

    volatile UINT32                                 Reserved3[64];          /* 0x200-0x2FC: Reserved */
    //MC: 0x0300-0x03FF
    volatile AMBA_DCPHY_GNR_CON0_REG_s              DcphyMcGnrCon0;         /* 0x300(RW): MC GNR Configuration0 */
    volatile AMBA_DCPHY_GNR_CON1_REG_s              DcphyMcGnrCon1;         /* 0x304(RW): MC GNR Configuration1 */
    volatile AMBA_DCPHY_MC_ANA_CON0_REG_s           DcphyMcAnaCon0;         /* 0x308(RW): MC ANA Configuration0 */
    volatile AMBA_DCPHY_M_ANA_CON1_REG_s            DcphyMcAnaCon1;         /* 0x30C(RW): MC ANA Configuration1 */
    volatile AMBA_DCPHY_MC_ANA_CON2_REG_s           DcphyMcAnaCon2;         /* 0x310(RW): MC ANA Configuration2 */
    volatile UINT32                                 Reserved4[7];           /* 0x314-0x32C: Reserved */
    volatile AMBA_DCPHY_MC_TIME_CON0_REG_s          DcphyMcTimeCon0;        /* 0x330(RW): MC TIME Configuration0 */
    volatile AMBA_DCPHY_MC_TIME_CON1_REG_s          DcphyMcTimeCon1;        /* 0x334(RW): MC TIME Configuration1 */
    volatile AMBA_DCPHY_MC_TIME_CON2_REG_s          DcphyMcTimeCon2;        /* 0x338(RW): MC TIME Configuration2 */
    volatile AMBA_DCPHY_MC_TIME_CON3_REG_s          DcphyMcTimeCon3;        /* 0x33C(RW): MC TIME Configuration3 */
    volatile AMBA_DCPHY_M_TIME_CON4_REG_s           DcphyMcTimeCon4;        /* 0x340(RW): MC TIME Configuration4 */
    volatile AMBA_DCPHY_MC_DATA_CON0_REG_s          DcphyMcDataCon0;        /* 0x344(RW): MC DATA Configuration0 */
    volatile UINT32                                 Reserved5[2];           /* 0x348-0x34C: Reserved */
    volatile AMBA_DCPHY_MC_DESKEW_CON0_REG_s        DcphyMcDeskewCon0;      /* 0x350(RW): MC DESKEW Configuration0 */
    volatile UINT32                                 Reserved6[7];           /* 0x354-0x36C: Reserved */
    volatile AMBA_DCPHY_TEST_CON0_REG_s             DcphyMcTestCon0;        /* 0x370(RW): MC TEST Configuration0 */
    volatile AMBA_DCPHY_MCD_TEST_CON1_REG_s         DcphyMcTestCon1;        /* 0x374(RW): MC TEST Configuration1 */
    volatile UINT32                                 Reserved7;              /* 0x378: Reserved */
    volatile AMBA_DCPHY_MCDSD_TEST_CON2_REG_s       DcphyMcTestCon2;        /* 0x37C(RW): MC TEST Configuration2 */
    volatile UINT32                                 Reserved8;              /* 0x380: Reserved */
    volatile AMBA_DCPHY_MCDSD_TEST_CON3_REG_s       DcphyMcTestCon3;        /* 0x384(RW): MC TEST Configuration3 */
    volatile AMBA_DCPHY_MCDSD_TEST_CON4_REG_s       DcphyMcTestCon4;        /* 0x388(RW): MC TEST Configuration4 */
    volatile UINT32                                 Reserved9;              /* 0x38C: Reserved */
    volatile AMBA_DCPHY_M_BIST_CON0_REG_s           DcphyMcBistCon0;        /* 0x390(RW): MC BIST Configuration0 */
    volatile AMBA_DCPHY_M_BIST_CON1_REG_s           DcphyMcBistCon1;        /* 0x394(RW): MC BIST Configuration1 */
    volatile UINT32                                 Reserved10[18];         /* 0x398-0x3DC: Reserved */
    volatile AMBA_DCPHY_MC_DBG_ST0_REG_s            DcphyMcDbgSt0;          /* 0x3E0(RO): MC DBG Status0 */
    volatile AMBA_DCPHY_MC_DBG_ST1_REG_s            DcphyMcDbgSt1;          /* 0x3E4(RO): MC DBG Status1 */
    volatile AMBA_DCPHY_MC_PPI_ST0_REG_s            DcphyMcPpiSt0;          /* 0x3E8(RO): MC PPI Status0 */
    volatile AMBA_DCPHY_MC_TX_BIST_5_REG_s          DcphyMcTxBist5;         /* 0x3EC(RO): MC Built-In Self Test 5 */
    volatile UINT32                                 Reserved11[4];          /* 0x3F0-0x3FC: Reserved */

    //CMD:0x400-0x6FF
    //Master 0 Data 0 Lane (Combo):0x4XX
    volatile AMBA_DCPHY_GNR_CON0_REG_s              DcphyCmd0GnrCon0;       /* 0x400(RW): CMD0 GNR Configuration0 */
    volatile AMBA_DCPHY_GNR_CON1_REG_s              DcphyCmd0GnrCon1;       /* 0x404(RW): CMD0 GNR Configuration1 */
    volatile AMBA_DCPHY_MD_ANA_CON0_REG_s           DcphyCmd0AnaCon0;       /* 0x408(RW): CMD0 ANA Configuration0 */
    volatile AMBA_DCPHY_M_ANA_CON1_REG_s            DcphyCmd0AnaCon1;       /* 0x40C(RW): CMD0 ANA Configuration1 */
    volatile AMBA_DCPHY_CMD_ANA_CON2_REG_s          DcphyCmd0AnaCon2;       /* 0x410(RW): CMD0 ANA Configuration2 */
    volatile AMBA_DCPHY_CMD_ANA_CON3_REG_s          DcphyCmd0AnaCon3;       /* 0x414(RW): CMD0 ANA Configuration3 */
    volatile UINT32                                 Reserved12[6];          /* 0x418-0x42C: Reserved */
    volatile AMBA_DCPHY_MD_TIME_CON0_REG_s          DcphyCmd0TimeCon0;      /* 0x430(RW): CMD0 TIME Configuration0 */
    volatile AMBA_DCPHY_MSD_THS1_REG_s              DcphyCmd0TimeCon1;      /* 0x434(RW): CMD0 TIME Configuration1 */
    volatile AMBA_DCPHY_MSD_THS2_REG_s              DcphyCmd0TimeCon2;      /* 0x438(RW): CMD0 TIME Configuration2 */
    volatile AMBA_DCPHY_MD_TIME_CON3_REG_s          DcphyCmd0TimeCon3;      /* 0x43C(RW): CMD0 TIME Configuration3 */
    volatile AMBA_DCPHY_M_TIME_CON4_REG_s           DcphyCmd0TimeCon4;      /* 0x440(RW): CMD0 TIME Configuration4 */
    volatile AMBA_DCPHY_CMD_DATA_CON0_REG_s         DcphyCmd0DataCon0;      /* 0x444(RW): CMD0 DATA Configuration0 */
    volatile UINT32                                 Reserved13[6];          /* 0x448-0x45C: Reserved */
    volatile AMBA_DCPHY_CMD_PSEQ_CON0_REG_s         DcphyCmd0PSeqCon0;      /* 0x460(RW): CMD0 PRGSEQ Configuration0 */
    volatile AMBA_DCPHY_CMD_PSEQ_CON1_REG_s         DcphyCmd0PSeqCon1;      /* 0x464(RW): CMD0 PRGSEQ Configuration1 */
    volatile AMBA_DCPHY_CMD_PSEQ_CON2_REG_s         DcphyCmd0PSeqCon2;      /* 0x468(RW): CMD0 PRGSEQ Configuration2 */
    volatile AMBA_DCPHY_CMD_PSEQ_CON3_REG_s         DcphyCmd0PSeqCon3;      /* 0x46C(RW): CMD0 PRGSEQ Configuration3 */
    volatile AMBA_DCPHY_TEST_CON0_REG_s             DcphyCmd0TestCon0;      /* 0x470(RW): CMD0 TEST Configuration0 */
    volatile AMBA_DCPHY_CMD_TEST_CON1_REG_s         DcphyCmd0TestCon1;      /* 0x474(RW): CMD0 TEST Configuration1 */
    volatile AMBA_DCPHY_CMSD_TEST_CON2_REG_s        DcphyCmd0TestCon2;      /* 0x478(RW): CMD0 TEST Configuration2 */
    volatile AMBA_DCPHY_CMSD_TEST_CON3_REG_s        DcphyCmd0TestCon3;      /* 0x47C(RW): CMD0 TEST Configuration3 */
    volatile AMBA_DCPHY_CMSD_TEST_CON4_REG_s        DcphyCmd0TestCon4;      /* 0x480(RW): CMD0 TEST Configuration4 */
    volatile AMBA_DCPHY_CMSD_TEST_CON5_REG_s        DcphyCmd0TestCon5;      /* 0x484(RW): CMD0 TEST Configuration5 */
    volatile AMBA_DCPHY_CMSD_TEST_CON6_REG_s        DcphyCmd0TestCon6;      /* 0x488(RW): CMD0 TEST Configuration6 */
    volatile UINT32                                 Reserved14;             /* 0x48C: Reserved */
    volatile AMBA_DCPHY_M_BIST_CON0_REG_s           DcphyCmd0BistCon0;      /* 0x490(RW): CMD0 Built-In Self Test Configuration0 */
    volatile AMBA_DCPHY_M_BIST_CON1_REG_s           DcphyCmd0BistCon1;      /* 0x494(RW): CMD0 Built-In Self Test Configuration0 */
    volatile UINT32                                 Reserved15[10];         /* 0x498-0x4BC: Reserved */
    volatile AMBA_DCPHY_MD_DBG_ST0_REG_s            DcphyCmd0DphyDbgSt0;    /* 0x4C0(RO): CMD0 DPhy DBG Status0 */
    volatile AMBA_DCPHY_MD_DBG_ST1_REG_s            DcphyCmd0DphyDbgSt1;    /* 0x4C4(RO): CMD0 DPhy DBG Status1 */
    volatile AMBA_DCPHY_MD_DBG_ST2_REG_s            DcphyCmd0DphyDbgSt2;    /* 0x4C8(RO): CMD0 DPhy DBG Status2 */
    volatile AMBA_DCPHY_MD_DBG_ST3_REG_s            DcphyCmd0DphyDbgSt3;    /* 0x4CC(RO): CMD0 DPhy DBG Status3 */
    volatile AMBA_DCPHY_MD_PPI_ST0_REG_s            DcphyCmd0DphyPpiSt0;    /* 0x4D0(RO): CMD0 DPhy PPI Status0 */
    volatile AMBA_DCPHY_MD_ADI_ST0_REG_s            DcphyCmd0DphyAdiSt0;    /* 0x4D4(RO): CMD0 DPhy ADI Status0 */
    volatile UINT32                                 Reserved16[2];          /* 0x4D8-0x4DC: Reserved */
    volatile AMBA_DCPHY_CMD_C_DBG_ST0_REG_s         DcphyCmd0CphyDbgSt0;    /* 0x4E0(RO): CMD0 CPhy DBG Status0 */
    volatile AMBA_DCPHY_MD_DBG_ST1_REG_s            DcphyCmd0CphyDbgSt1;    /* 0x4E4(RO): CMD0 CPhy DBG Status1 */
    volatile AMBA_DCPHY_MD_DBG_ST2_REG_s            DcphyCmd0CphyDbgSt2;    /* 0x4E8(RO): CMD0 CPhy DBG Status2 */
    volatile AMBA_DCPHY_MD_DBG_ST3_REG_s            DcphyCmd0CphyDbgSt3;    /* 0x4EC(RO): CMD0 CPhy DBG Status3 */
    volatile AMBA_DCPHY_MD_PPI_ST0_REG_s            DcphyCmd0CphyPpiSt0;    /* 0x4F0(RO): CMD0 CPhy PPI Status0 */
    volatile AMBA_DCPHY_MD_ADI_ST0_REG_s            DcphyCmd0CphyAdiSt0;    /* 0x4F4(RO): CMD0 CPhy ADI Status0 */
    volatile UINT32                                 Reserved17[2];          /* 0x4F8-0x4FC: Reserved */

    //Master 0 Data 1 Lane (Combo):0x5XX
    volatile AMBA_DCPHY_GNR_CON0_REG_s              DcphyCmd1GnrCon0;       /* 0x500(RW): CMD1 GNR Configuration0 */
    volatile AMBA_DCPHY_GNR_CON1_REG_s              DcphyCmd1GnrCon1;       /* 0x504(RW): CMD1 GNR Configuration1 */
    volatile AMBA_DCPHY_MD_ANA_CON0_REG_s           DcphyCmd1AnaCon0;       /* 0x508(RW): CMD1 ANA Configuration0 */
    volatile AMBA_DCPHY_M_ANA_CON1_REG_s            DcphyCmd1AnaCon1;       /* 0x50C(RW): CMD1 ANA Configuration1 */
    volatile AMBA_DCPHY_CMD_ANA_CON2_REG_s          DcphyCmd1AnaCon2;       /* 0x510(RW): CMD1 ANA Configuration2 */
    volatile AMBA_DCPHY_CMD_ANA_CON3_REG_s          DcphyCmd1AnaCon3;       /* 0x514(RW): CMD1 ANA Configuration3 */
    volatile UINT32                                 Reserved18[6];          /* 0x518-0x52C: Reserved */
    volatile AMBA_DCPHY_MD_TIME_CON0_REG_s          DcphyCmd1TimeCon0;      /* 0x530(RW): CMD1 TIME Configuration0 */
    volatile AMBA_DCPHY_MSD_THS1_REG_s              DcphyCmd1TimeCon1;      /* 0x534(RW): CMD1 TIME Configuration1 */
    volatile AMBA_DCPHY_MSD_THS2_REG_s              DcphyCmd1TimeCon2;      /* 0x538(RW): CMD1 TIME Configuration2 */
    volatile AMBA_DCPHY_MD_TIME_CON3_REG_s          DcphyCmd1TimeCon3;      /* 0x53C(RW): CMD1 TIME Configuration3 */
    volatile AMBA_DCPHY_M_TIME_CON4_REG_s           DcphyCmd1TimeCon4;      /* 0x540(RW): CMD1 TIME Configuration4 */
    volatile AMBA_DCPHY_CMD_DATA_CON0_REG_s         DcphyCmd1DataCon0;      /* 0x544(RW): CMD1 DATA Configuration0 */
    volatile UINT32                                 Reserved19[6];          /* 0x548-0x55C: Reserved */
    volatile AMBA_DCPHY_CMD_PSEQ_CON0_REG_s         DcphyCmd1PSeqCon0;      /* 0x560(RW): CMD1 PRGSEQ Configuration0 */
    volatile AMBA_DCPHY_CMD_PSEQ_CON1_REG_s         DcphyCmd1PSeqCon1;      /* 0x564(RW): CMD1 PRGSEQ Configuration1 */
    volatile AMBA_DCPHY_CMD_PSEQ_CON2_REG_s         DcphyCmd1PSeqCon2;      /* 0x568(RW): CMD1 PRGSEQ Configuration2 */
    volatile AMBA_DCPHY_CMD_PSEQ_CON3_REG_s         DcphyCmd1PSeqCon3;      /* 0x56C(RW): CMD1 PRGSEQ Configuration3 */
    volatile AMBA_DCPHY_TEST_CON0_REG_s             DcphyCmd1TestCon0;      /* 0x570(RW): CMD1 TEST Configuration0 */
    volatile AMBA_DCPHY_CMD_TEST_CON1_REG_s         DcphyCmd1TestCon1;      /* 0x574(RW): CMD1 TEST Configuration1 */
    volatile AMBA_DCPHY_CMSD_TEST_CON2_REG_s        DcphyCmd1TestCon2;      /* 0x578(RW): CMD1 TEST Configuration2 */
    volatile AMBA_DCPHY_CMSD_TEST_CON3_REG_s        DcphyCmd1TestCon3;      /* 0x57C(RW): CMD1 TEST Configuration3 */
    volatile AMBA_DCPHY_CMSD_TEST_CON4_REG_s        DcphyCmd1TestCon4;      /* 0x580(RW): CMD1 TEST Configuration4 */
    volatile AMBA_DCPHY_CMSD_TEST_CON5_REG_s        DcphyCmd1TestCon5;      /* 0x584(RW): CMD1 TEST Configuration5 */
    volatile AMBA_DCPHY_CMSD_TEST_CON6_REG_s        DcphyCmd1TestCon6;      /* 0x588(RW): CMD1 TEST Configuration6 */
    volatile UINT32                                 Reserved20;             /* 0x58C: Reserved */
    volatile AMBA_DCPHY_M_BIST_CON0_REG_s           DcphyCmd1BistCon0;      /* 0x590(RW): CMD1 Built-In Self Test Configuration0 */
    volatile AMBA_DCPHY_M_BIST_CON1_REG_s           DcphyCmd1BistCon1;      /* 0x594(RW): CMD1 Built-In Self Test Configuration0 */
    volatile UINT32                                 Reserved21[10];         /* 0x598-0x5BC: Reserved */
    volatile AMBA_DCPHY_MD_DBG_ST0_REG_s            DcphyCmd1DphyDbgSt0;    /* 0x5C0(RO): CMD1 DPhy DBG Status0 */
    volatile AMBA_DCPHY_MD_DBG_ST1_REG_s            DcphyCmd1DphyDbgSt1;    /* 0x5C4(RO): CMD1 DPhy DBG Status1 */
    volatile AMBA_DCPHY_MD_DBG_ST2_REG_s            DcphyCmd1DphyDbgSt2;    /* 0x5C8(RO): CMD1 DPhy DBG Status2 */
    volatile AMBA_DCPHY_MD_DBG_ST3_REG_s            DcphyCmd1DphyDbgSt3;    /* 0x5CC(RO): CMD1 DPhy DBG Status3 */
    volatile AMBA_DCPHY_MD_PPI_ST0_REG_s            DcphyCmd1DphyPpiSt0;    /* 0x5D0(RO): CMD1 DPhy PPI Status0 */
    volatile AMBA_DCPHY_MD_ADI_ST0_REG_s            DcphyCmd1DphyAdiSt0;    /* 0x5D4(RO): CMD1 DPhy ADI Status0 */
    volatile UINT32                                 Reserved22[2];          /* 0x5D8-0x5DC: Reserved */
    volatile AMBA_DCPHY_CMD_C_DBG_ST0_REG_s         DcphyCmd1CphyDbgSt0;    /* 0x5E0(RO): CMD1 CPhy DBG Status0 */
    volatile AMBA_DCPHY_MD_DBG_ST1_REG_s            DcphyCmd1CphyDbgSt1;    /* 0x5E4(RO): CMD1 CPhy DBG Status1 */
    volatile AMBA_DCPHY_MD_DBG_ST2_REG_s            DcphyCmd1CphyDbgSt2;    /* 0x5E8(RO): CMD1 CPhy DBG Status2 */
    volatile AMBA_DCPHY_MD_DBG_ST3_REG_s            DcphyCmd1CphyDbgSt3;    /* 0x5EC(RO): CMD1 CPhy DBG Status3 */
    volatile AMBA_DCPHY_MD_PPI_ST0_REG_s            DcphyCmd1CphyPpiSt0;    /* 0x5F0(RO): CMD1 CPhy PPI Status0 */
    volatile AMBA_DCPHY_MD_ADI_ST0_REG_s            DcphyCmd1CphyAdiSt0;    /* 0x5F4(RO): CMD1 CPhy ADI Status0 */
    volatile UINT32                                 Reserved23[2];          /* 0x5F8-0x5FC: Reserved */

    //Master 0 Data 2 Lane (Combo):0x6XX
    volatile AMBA_DCPHY_GNR_CON0_REG_s              DcphyCmd2GnrCon0;       /* 0x600(RW): CMD2 GNR Configuration0 */
    volatile AMBA_DCPHY_GNR_CON1_REG_s              DcphyCmd2GnrCon1;       /* 0x604(RW): CMD2 GNR Configuration1 */
    volatile AMBA_DCPHY_MD_ANA_CON0_REG_s           DcphyCmd2AnaCon0;       /* 0x608(RW): CMD2 ANA Configuration0 */
    volatile AMBA_DCPHY_M_ANA_CON1_REG_s            DcphyCmd2AnaCon1;       /* 0x60C(RW): CMD2 ANA Configuration1 */
    volatile AMBA_DCPHY_CMD_ANA_CON2_REG_s          DcphyCmd2AnaCon2;       /* 0x610(RW): CMD2 ANA Configuration2 */
    volatile AMBA_DCPHY_CMD_ANA_CON3_REG_s          DcphyCmd2AnaCon3;       /* 0x614(RW): CMD2 ANA Configuration3 */
    volatile UINT32                                 Reserved24[6];          /* 0x618-0x62C: Reserved */
    volatile AMBA_DCPHY_MD_TIME_CON0_REG_s          DcphyCmd2TimeCon0;      /* 0x630(RW): CMD2 TIME Configuration0 */
    volatile AMBA_DCPHY_MSD_THS1_REG_s              DcphyCmd2TimeCon1;      /* 0x634(RW): CMD2 TIME Configuration1 */
    volatile AMBA_DCPHY_MSD_THS2_REG_s              DcphyCmd2TimeCon2;      /* 0x638(RW): CMD2 TIME Configuration2 */
    volatile AMBA_DCPHY_MD_TIME_CON3_REG_s          DcphyCmd2TimeCon3;      /* 0x63C(RW): CMD2 TIME Configuration3 */
    volatile AMBA_DCPHY_M_TIME_CON4_REG_s           DcphyCmd2TimeCon4;      /* 0x640(RW): CMD2 TIME Configuration4 */
    volatile AMBA_DCPHY_CMD_DATA_CON0_REG_s         DcphyCmd2DataCon0;      /* 0x644(RW): CMD2 DATA Configuration0 */
    volatile UINT32                                 Reserved25[6];          /* 0x648-0x65C: Reserved */
    volatile AMBA_DCPHY_CMD_PSEQ_CON0_REG_s         DcphyCmd2PSeqCon0;      /* 0x660(RW): CMD2 PRGSEQ Configuration0 */
    volatile AMBA_DCPHY_CMD_PSEQ_CON1_REG_s         DcphyCmd2PSeqCon1;      /* 0x664(RW): CMD2 PRGSEQ Configuration1 */
    volatile AMBA_DCPHY_CMD_PSEQ_CON2_REG_s         DcphyCmd2PSeqCon2;      /* 0x668(RW): CMD2 PRGSEQ Configuration2 */
    volatile AMBA_DCPHY_CMD_PSEQ_CON3_REG_s         DcphyCmd2PSeqCon3;      /* 0x66C(RW): CMD2 PRGSEQ Configuration3 */
    volatile AMBA_DCPHY_TEST_CON0_REG_s             DcphyCmd2TestCon0;      /* 0x670(RW): CMD2 TEST Configuration0 */
    volatile AMBA_DCPHY_CMD_TEST_CON1_REG_s         DcphyCmd2TestCon1;      /* 0x674(RW): CMD2 TEST Configuration1 */
    volatile AMBA_DCPHY_CMSD_TEST_CON2_REG_s        DcphyCmd2TestCon2;      /* 0x678(RW): CMD2 TEST Configuration2 */
    volatile AMBA_DCPHY_CMSD_TEST_CON3_REG_s        DcphyCmd2TestCon3;      /* 0x67C(RW): CMD2 TEST Configuration3 */
    volatile AMBA_DCPHY_CMSD_TEST_CON4_REG_s        DcphyCmd2TestCon4;      /* 0x680(RW): CMD2 TEST Configuration4 */
    volatile AMBA_DCPHY_CMSD_TEST_CON5_REG_s        DcphyCmd2TestCon5;      /* 0x684(RW): CMD2 TEST Configuration5 */
    volatile AMBA_DCPHY_CMSD_TEST_CON6_REG_s        DcphyCmd2TestCon6;      /* 0x688(RW): CMD2 TEST Configuration6 */
    volatile UINT32                                 Reserved26;             /* 0x68C: Reserved */
    volatile AMBA_DCPHY_M_BIST_CON0_REG_s           DcphyCmd2BistCon0;      /* 0x690(RW): CMD2 Built-In Self Test Configuration0 */
    volatile AMBA_DCPHY_M_BIST_CON1_REG_s           DcphyCmd2BistCon1;      /* 0x694(RW): CMD2 Built-In Self Test Configuration0 */
    volatile UINT32                                 Reserved27[10];         /* 0x698-0x6BC: Reserved */
    volatile AMBA_DCPHY_MD_DBG_ST0_REG_s            DcphyCmd2DphyDbgSt0;    /* 0x6C0(RO): CMD2 DPhy DBG Status0 */
    volatile AMBA_DCPHY_MD_DBG_ST1_REG_s            DcphyCmd2DphyDbgSt1;    /* 0x6C4(RO): CMD2 DPhy DBG Status1 */
    volatile AMBA_DCPHY_MD_DBG_ST2_REG_s            DcphyCmd2DphyDbgSt2;    /* 0x6C8(RO): CMD2 DPhy DBG Status2 */
    volatile AMBA_DCPHY_MD_DBG_ST3_REG_s            DcphyCmd2DphyDbgSt3;    /* 0x6CC(RO): CMD2 DPhy DBG Status3 */
    volatile AMBA_DCPHY_MD_PPI_ST0_REG_s            DcphyCmd2DphyPpiSt0;    /* 0x6D0(RO): CMD2 DPhy PPI Status0 */
    volatile AMBA_DCPHY_MD_ADI_ST0_REG_s            DcphyCmd2DphyAdiSt0;    /* 0x6D4(RO): CMD2 DPhy ADI Status0 */
    volatile UINT32                                 Reserved28[2];          /* 0x6D8-0x6DC: Reserved */
    volatile AMBA_DCPHY_CMD_C_DBG_ST0_REG_s         DcphyCmd2CphyDbgSt0;    /* 0x6E0(RO): CMD2 CPhy DBG Status0 */
    volatile AMBA_DCPHY_MD_DBG_ST1_REG_s            DcphyCmd2CphyDbgSt1;    /* 0x6E4(RO): CMD2 CPhy DBG Status1 */
    volatile AMBA_DCPHY_MD_DBG_ST2_REG_s            DcphyCmd2CphyDbgSt2;    /* 0x6E8(RO): CMD2 CPhy DBG Status2 */
    volatile AMBA_DCPHY_MD_DBG_ST3_REG_s            DcphyCmd2CphyDbgSt3;    /* 0x6EC(RO): CMD2 CPhy DBG Status3 */
    volatile AMBA_DCPHY_MD_PPI_ST0_REG_s            DcphyCmd2CphyPpiSt0;    /* 0x6F0(RO): CMD2 CPhy PPI Status0 */
    volatile AMBA_DCPHY_MD_ADI_ST0_REG_s            DcphyCmd2CphyAdiSt0;    /* 0x6F4(RO): CMD2 CPhy ADI Status0 */
    volatile UINT32                                 Reserved29[2];          /* 0x6F8-0x6FC: Reserved */

    //MD:0x700-0x7FF
    volatile AMBA_DCPHY_GNR_CON0_REG_s              DcphyMdGnrCon0;         /* 0x700(RW): MD GNR Configuration0 */
    volatile AMBA_DCPHY_GNR_CON1_REG_s              DcphyMdGnrCon1;         /* 0x704(RW): MD GNR Configuration1 */
    volatile AMBA_DCPHY_MD_ANA_CON0_REG_s           DcphyMdAnaCon0;         /* 0x708(RW): MD ANA Configuration0 */
    volatile AMBA_DCPHY_M_ANA_CON1_REG_s            DcphyMdAnaCon1;         /* 0x70C(RW): MD ANA Configuration1 */
    volatile UINT32                                 Reserved30[8];          /* 0x710-0x72C: Reserved */
    volatile AMBA_DCPHY_MD_TIME_CON0_REG_s          DcphyMdTimeCon0;        /* 0x730(RW): MD TIME Configuration0 */
    volatile AMBA_DCPHY_MSD_THS1_REG_s              DcphyMdTimeCon1;        /* 0x734(RW): MD TIME Configuration1 */
    volatile AMBA_DCPHY_MSD_THS2_REG_s              DcphyMdTimeCon2;        /* 0x738(RW): MD TIME Configuration2 */
    volatile AMBA_DCPHY_MD_TIME_CON3_REG_s          DcphyMdTimeCon3;        /* 0x73C(RW): MD TIME Configuration3 */
    volatile AMBA_DCPHY_M_TIME_CON4_REG_s           DcphyMdTimeCon4;        /* 0x740(RW): MD TIME Configuration4 */
    volatile AMBA_DCPHY_MD_DATA_CON0_REG_s          DcphyMdDataCon0;        /* 0x744(RW): MD DATA Configuration0 */
    volatile UINT32                                 Reserved31[10];         /* 0x748-0x76C: Reserved */
    volatile AMBA_DCPHY_TEST_CON0_REG_s             DcphyMdTestCon0;        /* 0x770(RW): MD TEST Configuration0 */
    volatile AMBA_DCPHY_MCD_TEST_CON1_REG_s         DcphyMdTestCon1;        /* 0x774(RW): MD TEST Configuration1 */
    volatile AMBA_DCPHY_MCDSD_TEST_CON2_REG_s       DcphyMdTestCon2;        /* 0x77C(RW): MD TEST Configuration2 */
    volatile UINT32                                 Reserved32;             /* 0x780: Reserved */
    volatile AMBA_DCPHY_MCDSD_TEST_CON3_REG_s       DcphyMdTestCon3;        /* 0x784(RW): MD TEST Configuration3 */
    volatile AMBA_DCPHY_MCDSD_TEST_CON4_REG_s       DcphyMdTestCon4;        /* 0x788(RW): MD TEST Configuration4 */
    volatile AMBA_DCPHY_M_BIST_CON0_REG_s           DcphyMdBistCon0;        /* 0x790(RW): MD Built-In Self Test Configuration0 */
    volatile AMBA_DCPHY_M_BIST_CON1_REG_s           DcphyMdBistCon1;        /* 0x794(RW): MD Built-In Self Test Configuration1 */
    volatile UINT32                                 Reserved33[10];         /* 0x798-0x7BC: Reserved */
    volatile AMBA_DCPHY_MD_DBG_ST0_REG_s            DcphyMdDbgSt0;          /* 0x7C0(RO): MD DBG Status0 */
    volatile AMBA_DCPHY_MD_DBG_ST1_REG_s            DcphyMdDbgSt1;          /* 0x7C4(RO): MD DBG Status1 */
    volatile AMBA_DCPHY_MD_DBG_ST2_REG_s            DcphyMdDbgSt2;          /* 0x7C8(RO): MD DBG Status2 */
    volatile AMBA_DCPHY_MD_DBG_ST3_REG_s            DcphyMdDbgSt3;          /* 0x7CC(RO): MD DBG Status3 */
    volatile AMBA_DCPHY_MD_PPI_ST0_REG_s            DcphyMdPpiSt0;          /* 0x7D0(RO): MD PPI Status0 */
    volatile AMBA_DCPHY_MD_ADI_ST0_REG_s            DcphyMdAdiSt0;          /* 0x7D4(RO): MD ADI Status0 */
    volatile UINT32                                 Reserved34[10];         /* 0x7D8-0x7FC: Reserved */

    volatile UINT32                                 Reserved35[64];         /* 0x800-0x8FC: Reserved */
    volatile UINT32                                 Reserved36[64];         /* 0x900-0x9FC: Reserved */
    volatile UINT32                                 Reserved37[64];         /* 0xA00-0xAFC: Reserved */

    //SC:0xB00-0BFF
    volatile AMBA_DCPHY_GNR_CON0_REG_s              DcphyScGnrCon0;         /* 0xB00(RW): SC GNR Configuration0 */
    volatile AMBA_DCPHY_GNR_CON1_REG_s              DcphyScGnrCon1;         /* 0xB04(RW): SC GNR Configuration1 */
    volatile AMBA_DCPHY_SC_ANA_CON0_REG_s           DcphyScAnaCon0;         /* 0xB08(RW): SC ANA Configuration0 */
    volatile AMBA_DCPHY_SCD_ANA_CON1_REG_s          DcphyScAnaCon1;         /* 0xB0C(RW): SC ANA Configuration1 */
    volatile AMBA_DCPHY_SC_ANA_CON2_REG_s           DcphyScAnaCon2;         /* 0xB10(RW): SC ANA Configuration2 */
    volatile AMBA_DCPHY_S_ANA_CON3_REG_s            DcphyScAnaCon3;         /* 0xB14(RW): SC ANA Configuration3 */
    volatile AMBA_DCPHY_SC_ANA_CON4_REG_s           DcphyScAnaCon4;         /* 0xB18(RW): SC ANA Configuration4 */
    volatile AMBA_DCPHY_SC_ANA_CON5_REG_s           DcphyScAnaCon5;         /* 0xB1C(RW): SC ANA Configuration5 */
    volatile UINT32                                 Reserved38[4];          /* 0xB20-0xB2C: Reserved */
    volatile AMBA_DCPHY_SC_TIME_CON0_REG_s          DcphyScTimeCon0;        /* 0xB30(RW): SC TIME Configuration0 */
    volatile UINT32                                 Reserved39[3];          /* 0xB34-0xB3C: Reserved */
    volatile AMBA_DCPHY_SC_DATA_CON0_REG_s          DcphyScDataCon0;        /* 0xB40(RW): SC DATA Configuration0 */
    volatile UINT32                                 Reserved40[11];         /* 0xB44-0xB6C: Reserved */
    volatile AMBA_DCPHY_SC_TEST_CON0_REG_s          DcphyScTestCon0;        /* 0xB70(RW): SC TEST Configuration0 */
    volatile AMBA_DCPHY_SC_TEST_CON1_REG_s          DcphyScTestCon1;        /* 0xB74(RW): SC TEST Configuration1 */
    volatile UINT32                                 Reserved41[26];         /* 0xB78-0xBDC: Reserved */
    volatile AMBA_DCPHY_SC_DBG_ST0_REG_s            DcphyScDbgSt0;          /* 0xBE0(RO): SC DBG Status0 */
    volatile AMBA_DCPHY_SC_DBG_ST1_REG_s            DcphyScDbgSt1;          /* 0xBE4(RO): SC DBG Status1 */
    volatile AMBA_DCPHY_SC_PPI_ST0_REG_s            DcphyScPpiSt0;          /* 0xBE8(RO): SC PPI Status0 */
    volatile AMBA_DCPHY_SC_ADI_ST0_REG_s            DcphyScAdiSt0;          /* 0xBEC(RO): SC ADI Status0 */
    volatile UINT32                                 Reserved42[4];          /* 0xBF0-0xBFC: Reserved */

    //CSD:0xc00-0xefc
    //Slave 0 Data 0 Lane (Combo):0xCXX
    volatile AMBA_DCPHY_GNR_CON0_REG_s              DcphyCsd0GnrCon0;       /* 0xC00(RW): CSD0 GNR Configuration0 */
    volatile AMBA_DCPHY_GNR_CON1_REG_s              DcphyCsd0GnrCon1;       /* 0xC04(RW): CSD0 GNR Configuration1 */
    volatile AMBA_DCPHY_CSD_ANA_CON0_REG_s          DcphyCsd0AnaCon0;       /* 0xC08(RW): CSD0 ANA Configuration0 */
    volatile AMBA_DCPHY_CSD_ANA_CON1_REG_s          DcphyCsd0AnaCon1;       /* 0xC0C(RW): CSD0 ANA Configuration1 */
    volatile AMBA_DCPHY_SD_ANA_CON2_REG_s           DcphyCsd0AnaCon2;       /* 0xC10(RW): CSD0 ANA Configuration2 */
    volatile AMBA_DCPHY_S_ANA_CON3_REG_s            DcphyCsd0AnaCon3;       /* 0xC14(RW): CSD0 ANA Configuration3 */
    volatile AMBA_DCPHY_SD_ANA_CON4_REG_s           DcphyCsd0AnaCon4;       /* 0xC18(RW): CSD0 ANA Configuration4 */
    volatile AMBA_DCPHY_CSD_ANA_CON5_REG_s          DcphyCsd0AnaCon5;       /* 0xC1C(RW): CSD0 ANA Configuration5 */
    volatile AMBA_DCPHY_CSD_ANA_CON6_REG_s          DcphyCsd0AnaCon6;       /* 0xC20(RW): CSD0 ANA Configuration6 */
    volatile AMBA_DCPHY_CSD_ANA_CON7_REG_s          DcphyCsd0AnaCon7;       /* 0xC24(RW): CSD0 ANA Configuration7 */
    volatile UINT32                                 Reserved43[2];          /* 0xC28-0xC2C: Reserved */
    volatile AMBA_DCPHY_SD_TIME_CON0_REG_s          DcphyCsd0TimeCon0;      /* 0xC30(RW): CSD0 TIME Configuration0 */
    volatile AMBA_DCPHY_SD_TIME_CON1_REG_s          DcphyCsd0TimeCon1;      /* 0xC34(RW): CSD0 TIME Configuration1 */
    volatile AMBA_DCPHY_CSD_DATA_CON0_REG_s         DcphyCsd0DataCon0;      /* 0xC38(RW): CSD0 DATA Configuration0 */
    volatile UINT32                                 Reserved44;             /* 0xC3C: Reserved */
    volatile AMBA_DCPHY_SD_DESKEW_CON0_REG_s        DcphyCsd0DeskewCon0;    /* 0xC40(RW): CSD0 DESKEW Configuration0 */
    volatile AMBA_DCPHY_SD_DESKEW_CON1_REG_s        DcphyCsd0DeskewCon1;    /* 0xC44(RW): CSD0 DESKEW Configuration1 */
    volatile AMBA_DCPHY_SD_DESKEW_CON2_REG_s        DcphyCsd0DeskewCon2;    /* 0xC48(RW): CSD0 DESKEW Configuration2 */
    volatile AMBA_DCPHY_SD_DESKEW_CON3_REG_s        DcphyCsd0DeskewCon3;    /* 0xC4C(RW): CSD0 DESKEW Configuration3 */
    volatile AMBA_DCPHY_SD_DESKEW_CON4_REG_s        DcphyCsd0DeskewCon4;    /* 0xC50(RW): CSD0 DESKEW Configuration4 */
    volatile AMBA_DCPHY_SD_DESKEW_MON0_REG_s        DcphyCsd0DeskewMon0;    /* 0xC54(RO): CSD0 DESKEW MON0 */
    volatile AMBA_DCPHY_SD_DESKEW_MON0_REG_s        DcphyCsd0DeskewMon1;    /* 0xC58(RO): CSD0 DESKEW MON1 */
    volatile UINT32                                 Reserved45;             /* 0xC5C: Reserved */
    volatile AMBA_DCPHY_CSD_CRC_CON0_REG_s          DcphyCsd0CrcCon0;       /* 0xC60(RW): CSD0 CRC Configuration1 */
    volatile AMBA_DCPHY_CSD_CRC_CON0_REG_s          DcphyCsd0CrcCon1;       /* 0xC64(RW): CSD0 CRC Configuration2 */
    volatile AMBA_DCPHY_CSD_CRC_CON0_REG_s          DcphyCsd0CrcCon2;       /* 0xC68(RW): CSD0 CRC Configuration3 */
    volatile UINT32                                 Reserved46;             /* 0xC6C: Reserved */
    volatile AMBA_DCPHY_TEST_CON0_REG_s             DcphyCsd0TestCon0;      /* 0xC70(RW): CSD0 TEST Configuration0 */
    volatile AMBA_DCPHY_CSD_TEST_CON1_REG_s         DcphyCsd0TestCon1;      /* 0xC74(RW): CSD0 TEST Configuration1 */
    volatile AMBA_DCPHY_CMSD_TEST_CON2_REG_s        DcphyCsd0TestCon2;      /* 0xC78(RW): CSD0 TEST Configuration2 */
    volatile AMBA_DCPHY_CMSD_TEST_CON3_REG_s        DcphyCsd0TestCon3;      /* 0xC7C(RW): CSD0 TEST Configuration3 */
    volatile AMBA_DCPHY_CMSD_TEST_CON4_REG_s        DcphyCsd0TestCon4;      /* 0xC80(RW): CSD0 TEST Configuration4 */
    volatile AMBA_DCPHY_CMSD_TEST_CON5_REG_s        DcphyCsd0TestCon5;      /* 0xC84(RW): CSD0 TEST Configuration5 */
    volatile AMBA_DCPHY_CMSD_TEST_CON6_REG_s        DcphyCsd0TestCon6;      /* 0xC88(RW): CSD0 TEST Configuration6 */
    volatile UINT32                                 Reserved47;             /* 0xC8C: Reserved */
    volatile AMBA_DCPHY_SD_BIST_CON0_REG_s          DcphyCsd0BistCon0;      /* 0xC90(RW): CSD0 Built-In Self Test Configuration0 */
    volatile AMBA_DCPHY_MSD_THS1_REG_s              DcphyCsd0BistCon1;      /* 0xC94(RW): CSD0 Built-In Self Test Configuration1 */
    volatile AMBA_DCPHY_MSD_THS2_REG_s              DcphyCsd0BistCon2;      /* 0xC98(RW): CSD0 Built-In Self Test Configuration2 */
    volatile UINT32                                 Reserved48;             /* 0xC9C: Reserved */
    volatile AMBA_DCPHY_SD_PKT_MON_CON0_REG_s       DcphyCsd0PktMonCon0;    /* 0xCA0(RW): CSD0 PKT MON Configuration0 */
    volatile UINT32                                 Reserved49[3];          /* 0xCA4-0xCAC: Reserved */
    volatile AMBA_DCPHY_SD_PKT_PRE_REG_s            DcphyCsd0PktSt0;        /* 0xCB0(RO): CSD0 PKT Status0 */
    volatile AMBA_DCPHY_SD_PKT_PRE_REG_s            DcphyCsd0PktSt1;        /* 0xCB4(RO): CSD0 PKT Status0 */
    volatile AMBA_DCPHY_SD_PKT_PRE_REG_s            DcphyCsd0PktSt2;        /* 0xCB8(RO): CSD0 PKT Status0 */
    volatile AMBA_DCPHY_PKT_CUR_REG_s               DcphyCsd0PktSt3;        /* 0xCBC(RO): CSD0 PKT Status0 */
    volatile AMBA_DCPHY_PKT_CUR_REG_s               DcphyCsd0PktSt4;        /* 0xCC0(RO): CSD0 PKT Status0 */
    volatile AMBA_DCPHY_PKT_CUR_REG_s               DcphyCsd0PktSt5;        /* 0xCC4(RO): CSD0 PKT Status0 */
    volatile AMBA_DCPHY_CSD_PKT_ST6_REG_s           DcphyCsd0PktSt6;        /* 0xCC8(RO): CSD0 PKT Status0 */
    volatile AMBA_DCPHY_CSD_PKT_ST7_REG_s           DcphyCsd0PktSt7;        /* 0xCCC(RO): CSD0 PKT Status0 */
    volatile AMBA_DCPHY_CSD_PKT_ST8_REG_s           DcphyCsd0PktSt8;        /* 0xCD0(RO): CSD0 PKT Status0 */
    volatile AMBA_DCPHY_CSD_PKT_ST9_REG_s           DcphyCsd0PktSt9;        /* 0xCD4(RO): CSD0 PKT Status0 */
    volatile AMBA_DCPHY_CSD_PKT_ST10_REG_s          DcphyCsd0PktSt10;       /* 0xCD8(RO): CSD0 PKT Status0 */
    volatile AMBA_DCPHY_CSD_PKT_ST11_REG_s          DcphyCsd0PktSt11;       /* 0xCDC(RO): CSD0 PKT Status0 */
    volatile AMBA_DCPHY_SD_DBG_ST0_REG_s            DcphyCsd0DphyDbgSt0;    /* 0xCE0(RO): CSD0 DPHY DBG Status0 */
    volatile AMBA_DCPHY_SD_DBG_ST1_REG_s            DcphyCsd0DphyDbgSt1;    /* 0xCE4(RO): CSD0 DPHY DBG Status1 */
    volatile AMBA_DCPHY_SD_PPI_ST0_REG_s            DcphyCsd0DphyPpiSt0;    /* 0xCE8(RO): CSD0 DPHY PPI Status0 */
    volatile AMBA_DCPHY_SD_ADI_ST0_REG_s            DcphyCsd0DphyAdiSt0;    /* 0xCEC(RO): CSD0 DPHY ADI Status0 */
    volatile AMBA_DCPHY_CSD_C_DBG_ST0_REG_s         DcphyCsd0CphyDbgSt0;    /* 0xCF0(RO): CSD0 CPHY DBG Status0 */
    volatile AMBA_DCPHY_SD_DBG_ST1_REG_s            DcphyCsd0CphyDbgSt1;    /* 0xCF4(RO): CSD0 CPHY DBG Status1 */
    volatile AMBA_DCPHY_SD_PPI_ST0_REG_s            DcphyCsd0CphyPpiSt0;    /* 0xCF8(RO): CSD0 CPHY PPI Status0 */
    volatile AMBA_DCPHY_CSD_C_ADI_ST0_REG_s         DcphyCsd0CphyAdiSt0;    /* 0xCFC(RO): CSD0 CPHY ADI Status0 */

    //Slave 0 Data 1 Lane (Combo):0xDXX
    volatile AMBA_DCPHY_GNR_CON0_REG_s              DcphyCsd1GnrCon0;       /* 0xD00(RW): CSD1 GNR Configuration0 */
    volatile AMBA_DCPHY_GNR_CON1_REG_s              DcphyCsd1GnrCon1;       /* 0xD04(RW): CSD1 GNR Configuration1 */
    volatile AMBA_DCPHY_CSD_ANA_CON0_REG_s          DcphyCsd1AnaCon0;       /* 0xD08(RW): CSD1 ANA Configuration0 */
    volatile AMBA_DCPHY_CSD_ANA_CON1_REG_s          DcphyCsd1AnaCon1;       /* 0xD0C(RW): CSD1 ANA Configuration1 */
    volatile AMBA_DCPHY_SD_ANA_CON2_REG_s           DcphyCsd1AnaCon2;       /* 0xD10(RW): CSD1 ANA Configuration2 */
    volatile AMBA_DCPHY_S_ANA_CON3_REG_s            DcphyCsd1AnaCon3;       /* 0xD14(RW): CSD1 ANA Configuration3 */
    volatile AMBA_DCPHY_SD_ANA_CON4_REG_s           DcphyCsd1AnaCon4;       /* 0xD18(RW): CSD1 ANA Configuration4 */
    volatile AMBA_DCPHY_CSD_ANA_CON5_REG_s          DcphyCsd1AnaCon5;       /* 0xD1C(RW): CSD1 ANA Configuration5 */
    volatile AMBA_DCPHY_CSD_ANA_CON6_REG_s          DcphyCsd1AnaCon6;       /* 0xD20(RW): CSD1 ANA Configuration6 */
    volatile AMBA_DCPHY_CSD_ANA_CON7_REG_s          DcphyCsd1AnaCon7;       /* 0xD24(RW): CSD1 ANA Configuration7 */
    volatile UINT32                                 Reserved50[2];          /* 0xD28-0xD2C: Reserved */
    volatile AMBA_DCPHY_SD_TIME_CON0_REG_s          DcphyCsd1TimeCon0;      /* 0xD30(RW): CSD1 TIME Configuration0 */
    volatile AMBA_DCPHY_SD_TIME_CON1_REG_s          DcphyCsd1TimeCon1;      /* 0xD34(RW): CSD1 TIME Configuration1 */
    volatile AMBA_DCPHY_CSD_DATA_CON0_REG_s         DcphyCsd1DataCon0;      /* 0xD38(RW): CSD1 DATA Configuration0 */
    volatile UINT32                                 Reserved51;             /* 0xD3C: Reserved */
    volatile AMBA_DCPHY_SD_DESKEW_CON0_REG_s        DcphyCsd1DeskewCon0;    /* 0xD40(RW): CSD1 DESKEW Configuration0 */
    volatile AMBA_DCPHY_SD_DESKEW_CON1_REG_s        DcphyCsd1DeskewCon1;    /* 0xD44(RW): CSD1 DESKEW Configuration1 */
    volatile AMBA_DCPHY_SD_DESKEW_CON2_REG_s        DcphyCsd1DeskewCon2;    /* 0xD48(RW): CSD1 DESKEW Configuration2 */
    volatile AMBA_DCPHY_SD_DESKEW_CON3_REG_s        DcphyCsd1DeskewCon3;    /* 0xD4C(RW): CSD1 DESKEW Configuration3 */
    volatile AMBA_DCPHY_SD_DESKEW_CON4_REG_s        DcphyCsd1DeskewCon4;    /* 0xD50(RW): CSD1 DESKEW Configuration4 */
    volatile AMBA_DCPHY_SD_DESKEW_MON0_REG_s        DcphyCsd1DeskewMon0;    /* 0xD54(RO): CSD1 DESKEW MON0 */
    volatile AMBA_DCPHY_SD_DESKEW_MON0_REG_s        DcphyCsd1DeskewMon1;    /* 0xD58(RO): CSD1 DESKEW MON1 */
    volatile UINT32                                 Reserved52;             /* 0xD5C: Reserved */
    volatile AMBA_DCPHY_CSD_CRC_CON0_REG_s          DcphyCsd1CrcCon0;       /* 0xD60(RW): CSD1 CRC Configuration1 */
    volatile AMBA_DCPHY_CSD_CRC_CON0_REG_s          DcphyCsd1CrcCon1;       /* 0xD64(RW): CSD1 CRC Configuration2 */
    volatile AMBA_DCPHY_CSD_CRC_CON0_REG_s          DcphyCsd1CrcCon2;       /* 0xD68(RW): CSD1 CRC Configuration3 */
    volatile UINT32                                 Reserved53;             /* 0xD6C: Reserved */
    volatile AMBA_DCPHY_TEST_CON0_REG_s             DcphyCsd1TestCon0;      /* 0xD70(RW): CSD1 TEST Configuration0 */
    volatile AMBA_DCPHY_CSD_TEST_CON1_REG_s         DcphyCsd1TestCon1;      /* 0xD74(RW): CSD1 TEST Configuration1 */
    volatile AMBA_DCPHY_CMSD_TEST_CON2_REG_s        DcphyCsd1TestCon2;      /* 0xD78(RW): CSD1 TEST Configuration2 */
    volatile AMBA_DCPHY_CMSD_TEST_CON3_REG_s        DcphyCsd1TestCon3;      /* 0xD7C(RW): CSD1 TEST Configuration3 */
    volatile AMBA_DCPHY_CMSD_TEST_CON4_REG_s        DcphyCsd1TestCon4;      /* 0xD80(RW): CSD1 TEST Configuration4 */
    volatile AMBA_DCPHY_CMSD_TEST_CON5_REG_s        DcphyCsd1TestCon5;      /* 0xD84(RW): CSD1 TEST Configuration5 */
    volatile AMBA_DCPHY_CMSD_TEST_CON6_REG_s        DcphyCsd1TestCon6;      /* 0xD88(RW): CSD1 TEST Configuration6 */
    volatile UINT32                                 Reserved54;             /* 0xD8C: Reserved */
    volatile AMBA_DCPHY_SD_BIST_CON0_REG_s          DcphyCsd1BistCon0;      /* 0xD90(RW): CSD1 Built-In Self Test Configuration0 */
    volatile AMBA_DCPHY_MSD_THS1_REG_s              DcphyCsd1BistCon1;      /* 0xD94(RW): CSD1 Built-In Self Test Configuration1 */
    volatile AMBA_DCPHY_MSD_THS2_REG_s              DcphyCsd1BistCon2;      /* 0xD98(RW): CSD1 Built-In Self Test Configuration2 */
    volatile UINT32                                 Reserved55;             /* 0xD9C: Reserved */
    volatile AMBA_DCPHY_SD_PKT_MON_CON0_REG_s       DcphyCsd1PktMonCon0;    /* 0xDA0(RW): CSD1 PKT MON Configuration0 */
    volatile UINT32                                 Reserved56[3];          /* 0xDA4-0xDAC: Reserved */
    volatile AMBA_DCPHY_SD_PKT_PRE_REG_s            DcphyCsd1PktSt0;        /* 0xDB0(RO): CSD1 PKT Status0 */
    volatile AMBA_DCPHY_SD_PKT_PRE_REG_s            DcphyCsd1PktSt1;        /* 0xDB4(RO): CSD1 PKT Status0 */
    volatile AMBA_DCPHY_SD_PKT_PRE_REG_s            DcphyCsd1PktSt2;        /* 0xDB8(RO): CSD1 PKT Status0 */
    volatile AMBA_DCPHY_PKT_CUR_REG_s               DcphyCsd1PktSt3;        /* 0xDBC(RO): CSD1 PKT Status0 */
    volatile AMBA_DCPHY_PKT_CUR_REG_s               DcphyCsd1PktSt4;        /* 0xDC0(RO): CSD1 PKT Status0 */
    volatile AMBA_DCPHY_PKT_CUR_REG_s               DcphyCsd1PktSt5;        /* 0xDC4(RO): CSD1 PKT Status0 */
    volatile AMBA_DCPHY_CSD_PKT_ST6_REG_s           DcphyCsd1PktSt6;        /* 0xDC8(RO): CSD1 PKT Status0 */
    volatile AMBA_DCPHY_CSD_PKT_ST7_REG_s           DcphyCsd1PktSt7;        /* 0xDCC(RO): CSD1 PKT Status0 */
    volatile AMBA_DCPHY_CSD_PKT_ST8_REG_s           DcphyCsd1PktSt8;        /* 0xDD0(RO): CSD1 PKT Status0 */
    volatile AMBA_DCPHY_CSD_PKT_ST9_REG_s           DcphyCsd1PktSt9;        /* 0xDD4(RO): CSD1 PKT Status0 */
    volatile AMBA_DCPHY_CSD_PKT_ST10_REG_s          DcphyCsd1PktSt10;       /* 0xDD8(RO): CSD1 PKT Status0 */
    volatile AMBA_DCPHY_CSD_PKT_ST11_REG_s          DcphyCsd1PktSt11;       /* 0xDDC(RO): CSD1 PKT Status0 */
    volatile AMBA_DCPHY_SD_DBG_ST0_REG_s            DcphyCsd1DphyDbgSt0;    /* 0xDE0(RO): CSD1 DPHY DBG Status0 */
    volatile AMBA_DCPHY_SD_DBG_ST1_REG_s            DcphyCsd1DphyDbgSt1;    /* 0xDE4(RO): CSD1 DPHY DBG Status1 */
    volatile AMBA_DCPHY_SD_PPI_ST0_REG_s            DcphyCsd1DphyPpiSt0;    /* 0xDE8(RO): CSD1 DPHY PPI Status0 */
    volatile AMBA_DCPHY_SD_ADI_ST0_REG_s            DcphyCsd1DphyAdiSt0;    /* 0xDEC(RO): CSD1 DPHY ADI Status0 */
    volatile AMBA_DCPHY_CSD_C_DBG_ST0_REG_s         DcphyCsd1CphyDbgSt0;    /* 0xDF0(RO): CSD1 CPHY DBG Status0 */
    volatile AMBA_DCPHY_SD_DBG_ST1_REG_s            DcphyCsd1CphyDbgSt1;    /* 0xDF4(RO): CSD1 CPHY DBG Status1 */
    volatile AMBA_DCPHY_SD_PPI_ST0_REG_s            DcphyCsd1CphyPpiSt0;    /* 0xDF8(RO): CSD1 CPHY PPI Status0 */
    volatile AMBA_DCPHY_CSD_C_ADI_ST0_REG_s         DcphyCsd1CphyAdiSt0;    /* 0xDFC(RO): CSD1 CPHY ADI Status0 */

    //Slave 0 Data 2 Lane (Combo):0xEXX
    volatile AMBA_DCPHY_GNR_CON0_REG_s              DcphyCsd2GnrCon0;       /* 0xE00(RW): CSD2 GNR Configuration0 */
    volatile AMBA_DCPHY_GNR_CON1_REG_s              DcphyCsd2GnrCon1;       /* 0xE04(RW): CSD2 GNR Configuration1 */
    volatile AMBA_DCPHY_CSD_ANA_CON0_REG_s          DcphyCsd2AnaCon0;       /* 0xE08(RW): CSD2 ANA Configuration0 */
    volatile AMBA_DCPHY_CSD_ANA_CON1_REG_s          DcphyCsd2AnaCon1;       /* 0xE0C(RW): CSD2 ANA Configuration1 */
    volatile AMBA_DCPHY_SD_ANA_CON2_REG_s           DcphyCsd2AnaCon2;       /* 0xE10(RW): CSD2 ANA Configuration2 */
    volatile AMBA_DCPHY_S_ANA_CON3_REG_s            DcphyCsd2AnaCon3;       /* 0xE14(RW): CSD2 ANA Configuration3 */
    volatile AMBA_DCPHY_SD_ANA_CON4_REG_s           DcphyCsd2AnaCon4;       /* 0xE18(RW): CSD2 ANA Configuration4 */
    volatile AMBA_DCPHY_CSD_ANA_CON5_REG_s          DcphyCsd2AnaCon5;       /* 0xE1C(RW): CSD2 ANA Configuration5 */
    volatile AMBA_DCPHY_CSD_ANA_CON6_REG_s          DcphyCsd2AnaCon6;       /* 0xE20(RW): CSD2 ANA Configuration6 */
    volatile AMBA_DCPHY_CSD_ANA_CON7_REG_s          DcphyCsd2AnaCon7;       /* 0xE24(RW): CSD2 ANA Configuration7 */
    volatile UINT32                                 Reserved57[2];          /* 0xE28-0xE2C: Reserved */
    volatile AMBA_DCPHY_SD_TIME_CON0_REG_s          DcphyCsd2TimeCon0;      /* 0xE30(RW): CSD2 TIME Configuration0 */
    volatile AMBA_DCPHY_SD_TIME_CON1_REG_s          DcphyCsd2TimeCon1;      /* 0xE34(RW): CSD2 TIME Configuration1 */
    volatile AMBA_DCPHY_CSD_DATA_CON0_REG_s         DcphyCsd2DataCon0;      /* 0xE38(RW): CSD2 DATA Configuration0 */
    volatile UINT32                                 Reserved58;             /* 0xE3C: Reserved */
    volatile AMBA_DCPHY_SD_DESKEW_CON0_REG_s        DcphyCsd2DeskewCon0;    /* 0xE40(RW): CSD2 DESKEW Configuration0 */
    volatile AMBA_DCPHY_SD_DESKEW_CON1_REG_s        DcphyCsd2DeskewCon1;    /* 0xE44(RW): CSD2 DESKEW Configuration1 */
    volatile AMBA_DCPHY_SD_DESKEW_CON2_REG_s        DcphyCsd2DeskewCon2;    /* 0xE48(RW): CSD2 DESKEW Configuration2 */
    volatile AMBA_DCPHY_SD_DESKEW_CON3_REG_s        DcphyCsd2DeskewCon3;    /* 0xE4C(RW): CSD2 DESKEW Configuration3 */
    volatile AMBA_DCPHY_SD_DESKEW_CON4_REG_s        DcphyCsd2DeskewCon4;    /* 0xE50(RW): CSD2 DESKEW Configuration4 */
    volatile AMBA_DCPHY_SD_DESKEW_MON0_REG_s        DcphyCsd2DeskewMon0;    /* 0xE54(RO): CSD2 DESKEW MON0 */
    volatile AMBA_DCPHY_SD_DESKEW_MON0_REG_s        DcphyCsd2DeskewMon1;    /* 0xE58(RO): CSD2 DESKEW MON1 */
    volatile UINT32                                 Reserved59;             /* 0xE5C: Reserved */
    volatile AMBA_DCPHY_CSD_CRC_CON0_REG_s          DcphyCsd2CrcCon0;       /* 0xE60(RW): CSD2 CRC Configuration1 */
    volatile AMBA_DCPHY_CSD_CRC_CON0_REG_s          DcphyCsd2CrcCon1;       /* 0xE64(RW): CSD2 CRC Configuration2 */
    volatile AMBA_DCPHY_CSD_CRC_CON0_REG_s          DcphyCsd2CrcCon2;       /* 0xE68(RW): CSD2 CRC Configuration3 */
    volatile UINT32                                 Reserved60;             /* 0xE6C: Reserved */
    volatile AMBA_DCPHY_TEST_CON0_REG_s             DcphyCsd2TestCon0;      /* 0xE70(RW): CSD2 TEST Configuration0 */
    volatile AMBA_DCPHY_CSD_TEST_CON1_REG_s         DcphyCsd2TestCon1;      /* 0xE74(RW): CSD2 TEST Configuration1 */
    volatile AMBA_DCPHY_CMSD_TEST_CON2_REG_s        DcphyCsd2TestCon2;      /* 0xE78(RW): CSD2 TEST Configuration2 */
    volatile AMBA_DCPHY_CMSD_TEST_CON3_REG_s        DcphyCsd2TestCon3;      /* 0xE7C(RW): CSD2 TEST Configuration3 */
    volatile AMBA_DCPHY_CMSD_TEST_CON4_REG_s        DcphyCsd2TestCon4;      /* 0xE80(RW): CSD2 TEST Configuration4 */
    volatile AMBA_DCPHY_CMSD_TEST_CON5_REG_s        DcphyCsd2TestCon5;      /* 0xE84(RW): CSD2 TEST Configuration5 */
    volatile AMBA_DCPHY_CMSD_TEST_CON6_REG_s        DcphyCsd2TestCon6;      /* 0xE88(RW): CSD2 TEST Configuration6 */
    volatile UINT32                                 Reserved61;             /* 0xE8C: Reserved */
    volatile AMBA_DCPHY_SD_BIST_CON0_REG_s          DcphyCsd2BistCon0;      /* 0xE90(RW): CSD2 Built-In Self Test Configuration0 */
    volatile AMBA_DCPHY_MSD_THS1_REG_s              DcphyCsd2BistCon1;      /* 0xE94(RW): CSD2 Built-In Self Test Configuration1 */
    volatile AMBA_DCPHY_MSD_THS2_REG_s              DcphyCsd2BistCon2;      /* 0xE98(RW): CSD2 Built-In Self Test Configuration2 */
    volatile UINT32                                 Reserved62;             /* 0xE9C: Reserved */
    volatile AMBA_DCPHY_SD_PKT_MON_CON0_REG_s       DcphyCsd2PktMonCon0;    /* 0xEA0(RW): CSD2 PKT MON Configuration0 */
    volatile UINT32                                 Reserved63[3];          /* 0xEA4-0xEAC: Reserved */
    volatile AMBA_DCPHY_SD_PKT_PRE_REG_s            DcphyCsd2PktSt0;        /* 0xEB0(RO): CSD2 PKT Status0 */
    volatile AMBA_DCPHY_SD_PKT_PRE_REG_s            DcphyCsd2PktSt1;        /* 0xEB4(RO): CSD2 PKT Status1 */
    volatile AMBA_DCPHY_SD_PKT_PRE_REG_s            DcphyCsd2PktSt2;        /* 0xEB8(RO): CSD2 PKT Status2 */
    volatile AMBA_DCPHY_PKT_CUR_REG_s               DcphyCsd2PktSt3;        /* 0xEBC(RO): CSD2 PKT Status3 */
    volatile AMBA_DCPHY_PKT_CUR_REG_s               DcphyCsd2PktSt4;        /* 0xEC0(RO): CSD2 PKT Status4 */
    volatile AMBA_DCPHY_PKT_CUR_REG_s               DcphyCsd2PktSt5;        /* 0xEC4(RO): CSD2 PKT Status5 */
    volatile AMBA_DCPHY_CSD_PKT_ST6_REG_s           DcphyCsd2PktSt6;        /* 0xEC8(RO): CSD2 PKT Status6 */
    volatile AMBA_DCPHY_CSD_PKT_ST7_REG_s           DcphyCsd2PktSt7;        /* 0xECC(RO): CSD2 PKT Status7 */
    volatile AMBA_DCPHY_CSD_PKT_ST8_REG_s           DcphyCsd2PktSt8;        /* 0xED0(RO): CSD2 PKT Status8 */
    volatile AMBA_DCPHY_CSD_PKT_ST9_REG_s           DcphyCsd2PktSt9;        /* 0xED4(RO): CSD2 PKT Status9 */
    volatile AMBA_DCPHY_CSD_PKT_ST10_REG_s          DcphyCsd2PktSt10;       /* 0xED8(RO): CSD2 PKT Status10 */
    volatile AMBA_DCPHY_CSD_PKT_ST11_REG_s          DcphyCsd2PktSt11;       /* 0xEDC(RO): CSD2 PKT Status11 */
    volatile AMBA_DCPHY_SD_DBG_ST0_REG_s            DcphyCsd2DphyDbgSt0;    /* 0xEE0(RO): CSD2 DPHY DBG Status0 */
    volatile AMBA_DCPHY_SD_DBG_ST1_REG_s            DcphyCsd2DphyDbgSt1;    /* 0xEE4(RO): CSD2 DPHY DBG Status1 */
    volatile AMBA_DCPHY_SD_PPI_ST0_REG_s            DcphyCsd2DphyPpiSt0;    /* 0xEE8(RO): CSD2 DPHY PPI Status0 */
    volatile AMBA_DCPHY_SD_ADI_ST0_REG_s            DcphyCsd2DphyAdiSt0;    /* 0xEEC(RO): CSD2 DPHY ADI Status0 */
    volatile AMBA_DCPHY_CSD_C_DBG_ST0_REG_s         DcphyCsd2CphyDbgSt0;    /* 0xEF0(RO): CSD2 CPHY DBG Status0 */
    volatile AMBA_DCPHY_SD_DBG_ST1_REG_s            DcphyCsd2CphyDbgSt1;    /* 0xEF4(RO): CSD2 CPHY DBG Status1 */
    volatile AMBA_DCPHY_SD_PPI_ST0_REG_s            DcphyCsd2CphyPpiSt0;    /* 0xEF8(RO): CSD2 CPHY PPI Status0 */
    volatile AMBA_DCPHY_CSD_C_ADI_ST0_REG_s         DcphyCsd2CphyAdiSt0;    /* 0xEFC(RO): CSD2 CPHY ADI Status0 */

    //SD:0xF00-0xFFF
    volatile AMBA_DCPHY_GNR_CON0_REG_s              DcphySdGnrCon0;         /* 0xF00(RW): SD GNR Configuration0 */
    volatile AMBA_DCPHY_GNR_CON1_REG_s              DcphySdGnrCon1;         /* 0xF04(RW): SD GNR Configuration1 */
    volatile AMBA_DCPHY_SD_ANA_CON0_REG_s           DcphySdAnaCon0;         /* 0xF08(RW): SD ANA Configuration0 */
    volatile AMBA_DCPHY_SCD_ANA_CON1_REG_s          DcphySdAnaCon1;         /* 0xF0C(RW): SD ANA Configuration1 */
    volatile AMBA_DCPHY_SD_ANA_CON2_REG_s           DcphySdAnaCon2;         /* 0xF10(RW): SD ANA Configuration2 */
    volatile AMBA_DCPHY_S_ANA_CON3_REG_s            DcphySdAnaCon3;         /* 0xF14(RW): SD ANA Configuration3 */
    volatile AMBA_DCPHY_SD_ANA_CON4_REG_s           DcphySdAnaCon4;         /* 0xF18(RW): SD ANA Configuration4 */
    volatile UINT32                                 Reserved64[5];          /* 0xF1C-0xF2C: Reserved */
    volatile AMBA_DCPHY_SD_TIME_CON0_REG_s          DcphySdTimeCon0;        /* 0xF30(RW): SD TIME Configuration0 */
    volatile AMBA_DCPHY_SD_TIME_CON1_REG_s          DcphySdTimeCon1;        /* 0xF34(RW): SD TIME Configuration1 */
    volatile AMBA_DCPHY_SD_DATA_CON0_REG_s          DcphySdDataCon0;        /* 0xF38(RW): SD DATA Configuration0 */
    volatile UINT32                                 Reserved65;             /* 0xF3C: Reserved */
    volatile AMBA_DCPHY_SD_DESKEW_CON0_REG_s        DcphySdDeskewCon0;      /* 0xF40(RW): SD DESKEW Configuration0 */
    volatile AMBA_DCPHY_SD_DESKEW_CON1_REG_s        DcphySdDeskewCon1;      /* 0xF44(RW): SD DESKEW Configuration1 */
    volatile AMBA_DCPHY_SD_DESKEW_CON2_REG_s        DcphySdDeskewCon2;      /* 0xF48(RW): SD DESKEW Configuration2 */
    volatile AMBA_DCPHY_SD_DESKEW_CON3_REG_s        DcphySdDeskewCon3;      /* 0xF4C(RW): SD DESKEW Configuration3 */
    volatile AMBA_DCPHY_SD_DESKEW_CON4_REG_s        DcphySdDeskewCon4;      /* 0xF50(RW): SD DESKEW Configuration4 */
    volatile AMBA_DCPHY_SD_DESKEW_MON0_REG_s        DcphySdDeskewMon0;      /* 0xF54(RO): SD DESKEW MON0 */
    volatile AMBA_DCPHY_SD_DESKEW_MON0_REG_s        DcphySdDeskewMon1;      /* 0xF58(RO): SD DESKEW MON1 */
    volatile UINT32                                 Reserved66[5];          /* 0xF5C-0xF6C: Reserved */
    volatile AMBA_DCPHY_TEST_CON0_REG_s             DcphySdTestCon0;        /* 0xF70(RW): SD TEST Configuration0 */
    volatile AMBA_DCPHY_SD_TEST_CON1_REG_s          DcphySdTestCon1;        /* 0xF74(RW): SD TEST Configuration1 */
    volatile UINT32                                 Reserved67;             /* 0xF78: Reserved */
    volatile AMBA_DCPHY_MCDSD_TEST_CON2_REG_s       DcphySdTestCon2;        /* 0xF7C(RW): SD TEST Configuration2 */
    volatile UINT32                                 Reserved68;             /* 0xF80: Reserved */
    volatile AMBA_DCPHY_MCDSD_TEST_CON3_REG_s       DcphySdTestCon3;        /* 0xF84(RW): SD TEST Configuration3 */
    volatile AMBA_DCPHY_MCDSD_TEST_CON4_REG_s       DcphySdTestCon4;        /* 0xF88(RW): SD TEST Configuration4 */
    volatile UINT32                                 Reserved69;             /* 0xF8C: Reserved */
    volatile AMBA_DCPHY_SD_BIST_CON0_REG_s          DcphySdBistCon0;        /* 0xF90(RW): SD Built-In Self Test Configuration0 */
    volatile AMBA_DCPHY_MSD_THS1_REG_s              DcphySdBistCon1;        /* 0xF94(RW): SD Built-In Self Test Configuration1 */
    volatile AMBA_DCPHY_MSD_THS2_REG_s              DcphySdBistCon2;        /* 0xF98(RW): SD Built-In Self Test Configuration2 */
    volatile UINT32                                 Reserved70;             /* 0xF9C: Reserved */
    volatile AMBA_DCPHY_SD_PKT_MON_CON0_REG_s       DcphySdPktMonCon0;      /* 0xFA0(RW): SD PKT MON Configuration0 */
    volatile UINT32                                 Reserved71[4];          /* 0xFA4-0xFB0: Reserved */
    volatile AMBA_DCPHY_SD_PKT_PRE_REG_s            DcphySdPktSt0;          /* 0xFB4(RO): SD PKT Status0 */
    volatile AMBA_DCPHY_SD_PKT_PRE_REG_s            DcphySdPktSt1;          /* 0xFB8(RO): SD PKT Status1 */
    volatile UINT32                                 Reserved72;             /* 0xFBC: Reserved */
    volatile AMBA_DCPHY_PKT_CUR_REG_s               DcphySdPktSt2;          /* 0xFC0(RO): SD PKT Status2 */
    volatile AMBA_DCPHY_PKT_CUR_REG_s               DcphySdPktSt3;          /* 0xFC4(RO): SD PKT Status3 */
    volatile AMBA_DCPHY_SD_PKT_ST4_REG_s            DcphySdPktSt4;          /* 0xFCC(RO): SD PKT Status4 */
    volatile AMBA_DCPHY_SD_PKT_ST5_REG_s            DcphySdPktSt5;          /* 0xFD0(RO): SD PKT Status5 */
    volatile AMBA_DCPHY_SD_PKT_ST6_REG_s            DcphySdPktSt6;          /* 0xFD4(RO): SD PKT Status6 */
    volatile AMBA_DCPHY_SD_PKT_ST7_REG_s            DcphySdPktSt7;          /* 0xFD8(RO): SD PKT Status7 */
    volatile UINT32                                 Reserved73;             /* 0xFDC: Reserved */
    volatile AMBA_DCPHY_SD_DBG_ST0_REG_s            DcphySdDphyDbgSt0;      /* 0xFE0(RO): SD DBG Status0 */
    volatile AMBA_DCPHY_SD_DBG_ST1_REG_s            DcphySdDphyDbgSt1;      /* 0xFE4(RO): SD DBG Status1 */
    volatile AMBA_DCPHY_SD_PPI_ST0_REG_s            DcphySdDphyPpiSt0;      /* 0xFE8(RO): SD PPI Status0 */
    volatile AMBA_DCPHY_SD_ADI_ST0_REG_s            DcphySdDphyAdiSt0;      /* 0xFEC(RO): SD ADI Status0 */

} AMBA_DCPHY_REG_s;
#else

typedef struct {
    volatile AMBA_DCPHY_GNR_CON0_REG_s          GnrCon0;                /* 0x00(RW): GNR Configuration0 */
    volatile UINT32                             GnrCon1;                /* 0x04(RW): GNR Configuration1 */
    volatile AMBA_DCPHY_CSD_ANA_CON0_REG_s      AnaCon0;                /* 0x08(RW): ANA Configuration0 */
    volatile AMBA_DCPHY_CSD_ANA_CON1_REG_s      AnaCon1;                /* 0x0C(RW): ANA Configuration1 */
    volatile AMBA_DCPHY_SD_ANA_CON2_REG_s       AnaCon2;                /* 0x10(RW): ANA Configuration2 */
    volatile UINT32                             AnaCon3;                /* 0x14(RW): ANA Configuration3 */
    volatile UINT32                             AnaCon4;                /* 0x18(RW): ANA Configuration4 */
    volatile AMBA_DCPHY_CSD_ANA_CON5_REG_s      AnaCon5;                /* 0x1C(RW): ANA Configuration5 */
    volatile AMBA_DCPHY_CSD_ANA_CON6_REG_s      AnaCon6;                /* 0x20(RW): ANA Configuration6 */
    volatile UINT32                             AnaCon7;                /* 0x24(RW): ANA Configuration7 */
    volatile UINT32                             Reserved[2];            /* 0x28-0xC2C: Reserved */
    volatile AMBA_DCPHY_SD_TIME_CON0_REG_s      TimeCon0;               /* 0x30(RW): TIME Configuration0 */
    volatile AMBA_DCPHY_SD_TIME_CON1_REG_s      TimeCon1;               /* 0x34(RW): TIME Configuration1 */
    volatile UINT32                             DataCon0;               /* 0x38(RW): DATA Configuration0 */
    volatile UINT32                             Reserved1;              /* 0x3C: Reserved */
    volatile AMBA_DCPHY_SD_DESKEW_CON0_REG_s    DeskewCon0;             /* 0x40(RW): DESKEW Configuration0 */
    volatile UINT32                             DeskewCon1;             /* 0x44(RW): DESKEW Configuration1 */
    volatile UINT32                             DeskewCon2;             /* 0x48(RW): DESKEW Configuration2 */
    volatile UINT32                             DeskewCon3;             /* 0x4C(RW): DESKEW Configuration3 */
    volatile UINT32                             DeskewCon4;             /* 0x50(RW): DESKEW Configuration4 */
    volatile AMBA_DCPHY_SD_DESKEW_MON0_REG_s    DeskewMon0;             /* 0x54(RO): DESKEW MON0 */
    volatile AMBA_DCPHY_SD_DESKEW_MON0_REG_s    DeskewMon1;             /* 0x58(RO): DESKEW MON1 */
    volatile UINT32                             Reserved2;              /* 0x5C: Reserved */
    volatile UINT32                             CrcCon0;                /* 0x60(RW): CRC Configuration1 */
    volatile AMBA_DCPHY_CSD_CRC_CON1_REG_s      CrcCon1;                /* 0x64(RW): CRC Configuration2 */
    volatile AMBA_DCPHY_CSD_CRC_CON2_REG_s      CrcCon2;                /* 0x68(RW): CRC Configuration3 */
    volatile UINT32                             Reserved3;              /* 0x6C: Reserved */
    volatile UINT32                             TestCon0;               /* 0x70(RW): TEST Configuration0 */
    volatile UINT32                             TestCon1;               /* 0x74(RW): TEST Configuration1 */
    volatile UINT32                             TestCon2;               /* 0x78(RW): TEST Configuration2 */
    volatile UINT32                             TestCon3;               /* 0x7C(RW): TEST Configuration3 */
    volatile UINT32                             TestCon4;               /* 0x80(RW): TEST Configuration4 */
    volatile UINT32                             TestCon5;               /* 0x84(RW): TEST Configuration5 */
    volatile UINT32                             TestCon6;               /* 0x88(RW): TEST Configuration6 */
    volatile UINT32                             Reserved4;              /* 0x8C: Reserved */
    volatile AMBA_DCPHY_SD_BIST_CON0_REG_s      BistCon0;               /* 0x90(RW): Built-In Self Test Configuration0 */
    volatile AMBA_DCPHY_MSD_THS1_REG_s          BistCon1;               /* 0x94(RW): Built-In Self Test Configuration1 */
    volatile AMBA_DCPHY_MSD_THS2_REG_s          BistCon2;               /* 0x98(RW): Built-In Self Test Configuration2 */
    volatile UINT32                             Reserved5;              /* 0x9C: Reserved */
    volatile AMBA_DCPHY_SD_PKT_MON_CON0_REG_s   PktMonCon0;             /* 0xA0(RW): PKT MON Configuration0 */
    volatile UINT32                             Reserved6[3];           /* 0xA4-0xCAC: Reserved */
    volatile AMBA_DCPHY_SD_PKT_PRE_REG_s        PktSt0;                 /* 0xB0(RO): PKT Status0 */
    volatile AMBA_DCPHY_SD_PKT_PRE_REG_s        PktSt1;                 /* 0xB4(RO): PKT Status0 */
    volatile AMBA_DCPHY_SD_PKT_PRE_REG_s        PktSt2;                 /* 0xB8(RO): PKT Status0 */
    volatile AMBA_DCPHY_PKT_CUR_REG_s           PktSt3;                 /* 0xBC(RO): PKT Status0 */
    volatile AMBA_DCPHY_PKT_CUR_REG_s           PktSt4;                 /* 0xC0(RO): PKT Status0 */
    volatile AMBA_DCPHY_PKT_CUR_REG_s           PktSt5;                 /* 0xC4(RO): PKT Status0 */
    volatile AMBA_DCPHY_CSD_PKT_ST6_REG_s       PktSt6;                 /* 0xC8(RO): PKT Status0 */
    volatile AMBA_DCPHY_CSD_PKT_ST7_REG_s       PktSt7;                 /* 0xCC(RO): PKT Status0 */
    volatile AMBA_DCPHY_CSD_PKT_ST8_REG_s       PktSt8;                 /* 0xD0(RO): PKT Status0 */
    volatile AMBA_DCPHY_CSD_PKT_ST9_REG_s       PktSt9;                 /* 0xD4(RO): PKT Status0 */
    volatile AMBA_DCPHY_CSD_PKT_ST10_REG_s      PktSt10;                /* 0xD8(RO): PKT Status0 */
    volatile AMBA_DCPHY_CSD_PKT_ST11_REG_s      PktSt11;                /* 0xDC(RO): PKT Status0 */
    volatile AMBA_DCPHY_SD_DBG_ST0_REG_s        DphyDbgSt0;             /* 0xE0(RO): DPHY DBG Status0 */
    volatile AMBA_DCPHY_SD_DBG_ST1_REG_s        DphyDbgSt1;             /* 0xE4(RO): DPHY DBG Status1 */
    volatile AMBA_DCPHY_SD_PPI_ST0_REG_s        DphyPpiSt0;             /* 0xE8(RO): DPHY PPI Status0 */
    volatile AMBA_DCPHY_SD_ADI_ST0_REG_s        DphyAdiSt0;             /* 0xEC(RO): DPHY ADI Status0 */
    volatile AMBA_DCPHY_CSD_C_DBG_ST0_REG_s     CphyDbgSt0;             /* 0xF0(RO): CPHY DBG Status0 */
    volatile AMBA_DCPHY_SD_DBG_ST1_REG_s        CphyDbgSt1;             /* 0xF4(RO): CPHY DBG Status1 */
    volatile AMBA_DCPHY_SD_PPI_ST0_REG_s        CphyPpiSt0;             /* 0xF8(RO): CPHY PPI Status0 */
    volatile AMBA_DCPHY_CSD_C_ADI_ST0_REG_s     CphyAdiSt0;             /* 0xFC(RO): CPHY ADI Status0 */

} AMBA_DCPHY_CSD_REG_s;


typedef struct {
    //BIAS: 0x0000-0x00FF
    volatile UINT32                                 DcphyBiasCon0;          /* 0x0000(RW): Bias Configuration0 */
    volatile UINT32                                 DcphyBiasCon1;          /* 0x0004(RW): Bias Configuration1 */
    volatile UINT32                                 DcphyBiasCon2;          /* 0x0008(RW): Bias Configuration2 */
    volatile UINT32                                 DcphyBiasCon3;          /* 0x000C(RW): Bias Configuration3 */
    volatile AMBA_DCPHY_BIAS_CON4_REG_s             DcphyBiasCon4;          /* 0x0010(RW): Bias Configuration4 */
    volatile UINT32                                 Reserved0[59];          /* 0x014-0x0FC: Reserved */
    //PLL: 0x0100-0x01FF
    volatile AMBA_DCPHY_PLL_CON0_REG_s              DcphyPllCon0;           /* 0x100(RW): Pll Configuration0 */
    volatile AMBA_DCPHY_PLL_CON1_REG_s              DcphyPllCon1;           /* 0x104(RW): Pll Configuration1 */
    volatile AMBA_DCPHY_PLL_CON2_REG_s              DcphyPllCon2;           /* 0x108(RW): Pll Configuration2 */
    volatile AMBA_DCPHY_PLL_CON3_REG_s              DcphyPllCon3;           /* 0x10C(RW): Pll Configuration3 */
    volatile AMBA_DCPHY_PLL_CON4_REG_s              DcphyPllCon4;           /* 0x110(RW): Pll Configuration4 */
    volatile AMBA_DCPHY_PLL_CON5_REG_s              DcphyPllCon5;           /* 0x114(RW): Pll Configuration5 */
    volatile AMBA_DCPHY_PLL_CON6_REG_s              DcphyPllCon6;           /* 0x118(RW): Pll Configuration6 */
    volatile AMBA_DCPHY_PLL_CON7_REG_s              DcphyPllCon7;           /* 0x11C(RW): Pll Configuration7 */
    volatile AMBA_DCPHY_PLL_CON8_REG_s              DcphyPllCon8;           /* 0x120(RW): Pll Configuration8 */
    volatile AMBA_DCPHY_PLL_CON9_REG_s              DcphyPllCon9;           /* 0x124(RW): Pll Configuration9 */
    volatile UINT32                                 Reserved1[6];           /* 0x128-0x13C: Reserved */
    volatile AMBA_DCPHY_PLL_ST0_REG_s               DcphyPllSt0;            /* 0x140(RO): Pll Status0 */
    volatile UINT32                                 Reserved2[47];          /* 0x144-0x1FC: Reserved */

    volatile UINT32                                 Reserved3[64];          /* 0x200-0x2FC: Reserved */
    //MC: 0x0300-0x03FF
    volatile AMBA_DCPHY_GNR_CON0_REG_s              DcphyMcGnrCon0;         /* 0x300(RW): MC GNR Configuration0 */
    volatile AMBA_DCPHY_GNR_CON1_REG_s              DcphyMcGnrCon1;         /* 0x304(RW): MC GNR Configuration1 */
    volatile AMBA_DCPHY_MC_ANA_CON0_REG_s           DcphyMcAnaCon0;         /* 0x308(RW): MC ANA Configuration0 */
    volatile AMBA_DCPHY_M_ANA_CON1_REG_s            DcphyMcAnaCon1;         /* 0x30C(RW): MC ANA Configuration1 */
    volatile AMBA_DCPHY_MC_ANA_CON2_REG_s           DcphyMcAnaCon2;         /* 0x310(RW): MC ANA Configuration2 */
    volatile UINT32                                 Reserved4[7];           /* 0x314-0x32C: Reserved */
    volatile AMBA_DCPHY_MC_TIME_CON0_REG_s          DcphyMcTimeCon0;        /* 0x330(RW): MC TIME Configuration0 */
    volatile AMBA_DCPHY_MC_TIME_CON1_REG_s          DcphyMcTimeCon1;        /* 0x334(RW): MC TIME Configuration1 */
    volatile AMBA_DCPHY_MC_TIME_CON2_REG_s          DcphyMcTimeCon2;        /* 0x338(RW): MC TIME Configuration2 */
    volatile AMBA_DCPHY_MC_TIME_CON3_REG_s          DcphyMcTimeCon3;        /* 0x33C(RW): MC TIME Configuration3 */
    volatile AMBA_DCPHY_M_TIME_CON4_REG_s           DcphyMcTimeCon4;        /* 0x340(RW): MC TIME Configuration4 */
    volatile AMBA_DCPHY_MC_DATA_CON0_REG_s          DcphyMcDataCon0;        /* 0x344(RW): MC DATA Configuration0 */
    volatile UINT32                                 Reserved5[2];           /* 0x348-0x34C: Reserved */
    volatile AMBA_DCPHY_MC_DESKEW_CON0_REG_s        DcphyMcDeskewCon0;      /* 0x350(RW): MC DESKEW Configuration0 */
    volatile UINT32                                 Reserved6[7];           /* 0x354-0x36C: Reserved */
    volatile AMBA_DCPHY_TEST_CON0_REG_s             DcphyMcTestCon0;        /* 0x370(RW): MC TEST Configuration0 */
    volatile AMBA_DCPHY_MCD_TEST_CON1_REG_s         DcphyMcTestCon1;        /* 0x374(RW): MC TEST Configuration1 */
    volatile UINT32                                 Reserved7;              /* 0x378: Reserved */
    volatile AMBA_DCPHY_MCDSD_TEST_CON2_REG_s       DcphyMcTestCon2;        /* 0x37C(RW): MC TEST Configuration2 */
    volatile UINT32                                 Reserved8;              /* 0x380: Reserved */
    volatile AMBA_DCPHY_MCDSD_TEST_CON3_REG_s       DcphyMcTestCon3;        /* 0x384(RW): MC TEST Configuration3 */
    volatile AMBA_DCPHY_MCDSD_TEST_CON4_REG_s       DcphyMcTestCon4;        /* 0x388(RW): MC TEST Configuration4 */
    volatile UINT32                                 Reserved9;              /* 0x38C: Reserved */
    volatile AMBA_DCPHY_M_BIST_CON0_REG_s           DcphyMcBistCon0;        /* 0x390(RW): MC BIST Configuration0 */
    volatile AMBA_DCPHY_M_BIST_CON1_REG_s           DcphyMcBistCon1;        /* 0x394(RW): MC BIST Configuration1 */
    volatile UINT32                                 Reserved10[18];         /* 0x398-0x3DC: Reserved */
    volatile AMBA_DCPHY_MC_DBG_ST0_REG_s            DcphyMcDbgSt0;          /* 0x3E0(RO): MC DBG Status0 */
    volatile AMBA_DCPHY_MC_DBG_ST1_REG_s            DcphyMcDbgSt1;          /* 0x3E4(RO): MC DBG Status1 */
    volatile AMBA_DCPHY_MC_PPI_ST0_REG_s            DcphyMcPpiSt0;          /* 0x3E8(RO): MC PPI Status0 */
    volatile AMBA_DCPHY_MC_TX_BIST_5_REG_s          DcphyMcTxBist5;         /* 0x3EC(RO): MC Built-In Self Test 5 */
    volatile UINT32                                 Reserved11[4];          /* 0x3F0-0x3FC: Reserved */

    //CMD:0x400-0x6FF
    //Master 0 Data 0 Lane (Combo):0x4XX
    volatile AMBA_DCPHY_GNR_CON0_REG_s              DcphyCmd0GnrCon0;       /* 0x400(RW): CMD0 GNR Configuration0 */
    volatile AMBA_DCPHY_GNR_CON1_REG_s              DcphyCmd0GnrCon1;       /* 0x404(RW): CMD0 GNR Configuration1 */
    volatile AMBA_DCPHY_MD_ANA_CON0_REG_s           DcphyCmd0AnaCon0;       /* 0x408(RW): CMD0 ANA Configuration0 */
    volatile AMBA_DCPHY_M_ANA_CON1_REG_s            DcphyCmd0AnaCon1;       /* 0x40C(RW): CMD0 ANA Configuration1 */
    volatile AMBA_DCPHY_CMD_ANA_CON2_REG_s          DcphyCmd0AnaCon2;       /* 0x410(RW): CMD0 ANA Configuration2 */
    volatile AMBA_DCPHY_CMD_ANA_CON3_REG_s          DcphyCmd0AnaCon3;       /* 0x414(RW): CMD0 ANA Configuration3 */
    volatile UINT32                                 Reserved12[6];          /* 0x418-0x42C: Reserved */
    volatile AMBA_DCPHY_MD_TIME_CON0_REG_s          DcphyCmd0TimeCon0;      /* 0x430(RW): CMD0 TIME Configuration0 */
    volatile AMBA_DCPHY_MSD_THS1_REG_s              DcphyCmd0TimeCon1;      /* 0x434(RW): CMD0 TIME Configuration1 */
    volatile AMBA_DCPHY_MSD_THS2_REG_s              DcphyCmd0TimeCon2;      /* 0x438(RW): CMD0 TIME Configuration2 */
    volatile AMBA_DCPHY_MD_TIME_CON3_REG_s          DcphyCmd0TimeCon3;      /* 0x43C(RW): CMD0 TIME Configuration3 */
    volatile AMBA_DCPHY_M_TIME_CON4_REG_s           DcphyCmd0TimeCon4;      /* 0x440(RW): CMD0 TIME Configuration4 */
    volatile AMBA_DCPHY_CMD_DATA_CON0_REG_s         DcphyCmd0DataCon0;      /* 0x444(RW): CMD0 DATA Configuration0 */
    volatile UINT32                                 Reserved13[6];          /* 0x448-0x45C: Reserved */
    volatile AMBA_DCPHY_CMD_PSEQ_CON0_REG_s         DcphyCmd0PSeqCon0;      /* 0x460(RW): CMD0 PRGSEQ Configuration0 */
    volatile AMBA_DCPHY_CMD_PSEQ_CON1_REG_s         DcphyCmd0PSeqCon1;      /* 0x464(RW): CMD0 PRGSEQ Configuration1 */
    volatile AMBA_DCPHY_CMD_PSEQ_CON2_REG_s         DcphyCmd0PSeqCon2;      /* 0x468(RW): CMD0 PRGSEQ Configuration2 */
    volatile AMBA_DCPHY_CMD_PSEQ_CON3_REG_s         DcphyCmd0PSeqCon3;      /* 0x46C(RW): CMD0 PRGSEQ Configuration3 */
    volatile AMBA_DCPHY_TEST_CON0_REG_s             DcphyCmd0TestCon0;      /* 0x470(RW): CMD0 TEST Configuration0 */
    volatile AMBA_DCPHY_CMD_TEST_CON1_REG_s         DcphyCmd0TestCon1;      /* 0x474(RW): CMD0 TEST Configuration1 */
    volatile AMBA_DCPHY_CMSD_TEST_CON2_REG_s        DcphyCmd0TestCon2;      /* 0x478(RW): CMD0 TEST Configuration2 */
    volatile AMBA_DCPHY_CMSD_TEST_CON3_REG_s        DcphyCmd0TestCon3;      /* 0x47C(RW): CMD0 TEST Configuration3 */
    volatile AMBA_DCPHY_CMSD_TEST_CON4_REG_s        DcphyCmd0TestCon4;      /* 0x480(RW): CMD0 TEST Configuration4 */
    volatile AMBA_DCPHY_CMSD_TEST_CON5_REG_s        DcphyCmd0TestCon5;      /* 0x484(RW): CMD0 TEST Configuration5 */
    volatile AMBA_DCPHY_CMSD_TEST_CON6_REG_s        DcphyCmd0TestCon6;      /* 0x488(RW): CMD0 TEST Configuration6 */
    volatile UINT32                                 Reserved14;             /* 0x48C: Reserved */
    volatile AMBA_DCPHY_M_BIST_CON0_REG_s           DcphyCmd0BistCon0;      /* 0x490(RW): CMD0 Built-In Self Test Configuration0 */
    volatile AMBA_DCPHY_M_BIST_CON1_REG_s           DcphyCmd0BistCon1;      /* 0x494(RW): CMD0 Built-In Self Test Configuration0 */
    volatile UINT32                                 Reserved15[10];         /* 0x498-0x4BC: Reserved */
    volatile AMBA_DCPHY_MD_DBG_ST0_REG_s            DcphyCmd0DphyDbgSt0;    /* 0x4C0(RO): CMD0 DPhy DBG Status0 */
    volatile AMBA_DCPHY_MD_DBG_ST1_REG_s            DcphyCmd0DphyDbgSt1;    /* 0x4C4(RO): CMD0 DPhy DBG Status1 */
    volatile AMBA_DCPHY_MD_DBG_ST2_REG_s            DcphyCmd0DphyDbgSt2;    /* 0x4C8(RO): CMD0 DPhy DBG Status2 */
    volatile AMBA_DCPHY_MD_DBG_ST3_REG_s            DcphyCmd0DphyDbgSt3;    /* 0x4CC(RO): CMD0 DPhy DBG Status3 */
    volatile AMBA_DCPHY_MD_PPI_ST0_REG_s            DcphyCmd0DphyPpiSt0;    /* 0x4D0(RO): CMD0 DPhy PPI Status0 */
    volatile AMBA_DCPHY_MD_ADI_ST0_REG_s            DcphyCmd0DphyAdiSt0;    /* 0x4D4(RO): CMD0 DPhy ADI Status0 */
    volatile UINT32                                 Reserved16[2];          /* 0x4D8-0x4DC: Reserved */
    volatile AMBA_DCPHY_CMD_C_DBG_ST0_REG_s         DcphyCmd0CphyDbgSt0;    /* 0x4E0(RO): CMD0 CPhy DBG Status0 */
    volatile AMBA_DCPHY_MD_DBG_ST1_REG_s            DcphyCmd0CphyDbgSt1;    /* 0x4E4(RO): CMD0 CPhy DBG Status1 */
    volatile AMBA_DCPHY_MD_DBG_ST2_REG_s            DcphyCmd0CphyDbgSt2;    /* 0x4E8(RO): CMD0 CPhy DBG Status2 */
    volatile AMBA_DCPHY_MD_DBG_ST3_REG_s            DcphyCmd0CphyDbgSt3;    /* 0x4EC(RO): CMD0 CPhy DBG Status3 */
    volatile AMBA_DCPHY_MD_PPI_ST0_REG_s            DcphyCmd0CphyPpiSt0;    /* 0x4F0(RO): CMD0 CPhy PPI Status0 */
    volatile AMBA_DCPHY_MD_ADI_ST0_REG_s            DcphyCmd0CphyAdiSt0;    /* 0x4F4(RO): CMD0 CPhy ADI Status0 */
    volatile UINT32                                 Reserved17[2];          /* 0x4F8-0x4FC: Reserved */

    //Master 0 Data 1 Lane (Combo):0x5XX
    volatile AMBA_DCPHY_GNR_CON0_REG_s              DcphyCmd1GnrCon0;       /* 0x500(RW): CMD1 GNR Configuration0 */
    volatile AMBA_DCPHY_GNR_CON1_REG_s              DcphyCmd1GnrCon1;       /* 0x504(RW): CMD1 GNR Configuration1 */
    volatile AMBA_DCPHY_MD_ANA_CON0_REG_s           DcphyCmd1AnaCon0;       /* 0x508(RW): CMD1 ANA Configuration0 */
    volatile AMBA_DCPHY_M_ANA_CON1_REG_s            DcphyCmd1AnaCon1;       /* 0x50C(RW): CMD1 ANA Configuration1 */
    volatile AMBA_DCPHY_CMD_ANA_CON2_REG_s          DcphyCmd1AnaCon2;       /* 0x510(RW): CMD1 ANA Configuration2 */
    volatile AMBA_DCPHY_CMD_ANA_CON3_REG_s          DcphyCmd1AnaCon3;       /* 0x514(RW): CMD1 ANA Configuration3 */
    volatile UINT32                                 Reserved18[6];          /* 0x518-0x52C: Reserved */
    volatile AMBA_DCPHY_MD_TIME_CON0_REG_s          DcphyCmd1TimeCon0;      /* 0x530(RW): CMD1 TIME Configuration0 */
    volatile AMBA_DCPHY_MSD_THS1_REG_s              DcphyCmd1TimeCon1;      /* 0x534(RW): CMD1 TIME Configuration1 */
    volatile AMBA_DCPHY_MSD_THS2_REG_s              DcphyCmd1TimeCon2;      /* 0x538(RW): CMD1 TIME Configuration2 */
    volatile AMBA_DCPHY_MD_TIME_CON3_REG_s          DcphyCmd1TimeCon3;      /* 0x53C(RW): CMD1 TIME Configuration3 */
    volatile AMBA_DCPHY_M_TIME_CON4_REG_s           DcphyCmd1TimeCon4;      /* 0x540(RW): CMD1 TIME Configuration4 */
    volatile AMBA_DCPHY_CMD_DATA_CON0_REG_s         DcphyCmd1DataCon0;      /* 0x544(RW): CMD1 DATA Configuration0 */
    volatile UINT32                                 Reserved19[6];          /* 0x548-0x55C: Reserved */
    volatile AMBA_DCPHY_CMD_PSEQ_CON0_REG_s         DcphyCmd1PSeqCon0;      /* 0x560(RW): CMD1 PRGSEQ Configuration0 */
    volatile AMBA_DCPHY_CMD_PSEQ_CON1_REG_s         DcphyCmd1PSeqCon1;      /* 0x564(RW): CMD1 PRGSEQ Configuration1 */
    volatile AMBA_DCPHY_CMD_PSEQ_CON2_REG_s         DcphyCmd1PSeqCon2;      /* 0x568(RW): CMD1 PRGSEQ Configuration2 */
    volatile AMBA_DCPHY_CMD_PSEQ_CON3_REG_s         DcphyCmd1PSeqCon3;      /* 0x56C(RW): CMD1 PRGSEQ Configuration3 */
    volatile AMBA_DCPHY_TEST_CON0_REG_s             DcphyCmd1TestCon0;      /* 0x570(RW): CMD1 TEST Configuration0 */
    volatile AMBA_DCPHY_CMD_TEST_CON1_REG_s         DcphyCmd1TestCon1;      /* 0x574(RW): CMD1 TEST Configuration1 */
    volatile AMBA_DCPHY_CMSD_TEST_CON2_REG_s        DcphyCmd1TestCon2;      /* 0x578(RW): CMD1 TEST Configuration2 */
    volatile AMBA_DCPHY_CMSD_TEST_CON3_REG_s        DcphyCmd1TestCon3;      /* 0x57C(RW): CMD1 TEST Configuration3 */
    volatile AMBA_DCPHY_CMSD_TEST_CON4_REG_s        DcphyCmd1TestCon4;      /* 0x580(RW): CMD1 TEST Configuration4 */
    volatile AMBA_DCPHY_CMSD_TEST_CON5_REG_s        DcphyCmd1TestCon5;      /* 0x584(RW): CMD1 TEST Configuration5 */
    volatile AMBA_DCPHY_CMSD_TEST_CON6_REG_s        DcphyCmd1TestCon6;      /* 0x588(RW): CMD1 TEST Configuration6 */
    volatile UINT32                                 Reserved20;             /* 0x58C: Reserved */
    volatile AMBA_DCPHY_M_BIST_CON0_REG_s           DcphyCmd1BistCon0;      /* 0x590(RW): CMD1 Built-In Self Test Configuration0 */
    volatile AMBA_DCPHY_M_BIST_CON1_REG_s           DcphyCmd1BistCon1;      /* 0x594(RW): CMD1 Built-In Self Test Configuration0 */
    volatile UINT32                                 Reserved21[10];         /* 0x598-0x5BC: Reserved */
    volatile AMBA_DCPHY_MD_DBG_ST0_REG_s            DcphyCmd1DphyDbgSt0;    /* 0x5C0(RO): CMD1 DPhy DBG Status0 */
    volatile AMBA_DCPHY_MD_DBG_ST1_REG_s            DcphyCmd1DphyDbgSt1;    /* 0x5C4(RO): CMD1 DPhy DBG Status1 */
    volatile AMBA_DCPHY_MD_DBG_ST2_REG_s            DcphyCmd1DphyDbgSt2;    /* 0x5C8(RO): CMD1 DPhy DBG Status2 */
    volatile AMBA_DCPHY_MD_DBG_ST3_REG_s            DcphyCmd1DphyDbgSt3;    /* 0x5CC(RO): CMD1 DPhy DBG Status3 */
    volatile AMBA_DCPHY_MD_PPI_ST0_REG_s            DcphyCmd1DphyPpiSt0;    /* 0x5D0(RO): CMD1 DPhy PPI Status0 */
    volatile AMBA_DCPHY_MD_ADI_ST0_REG_s            DcphyCmd1DphyAdiSt0;    /* 0x5D4(RO): CMD1 DPhy ADI Status0 */
    volatile UINT32                                 Reserved22[2];          /* 0x5D8-0x5DC: Reserved */
    volatile AMBA_DCPHY_CMD_C_DBG_ST0_REG_s         DcphyCmd1CphyDbgSt0;    /* 0x5E0(RO): CMD1 CPhy DBG Status0 */
    volatile AMBA_DCPHY_MD_DBG_ST1_REG_s            DcphyCmd1CphyDbgSt1;    /* 0x5E4(RO): CMD1 CPhy DBG Status1 */
    volatile AMBA_DCPHY_MD_DBG_ST2_REG_s            DcphyCmd1CphyDbgSt2;    /* 0x5E8(RO): CMD1 CPhy DBG Status2 */
    volatile AMBA_DCPHY_MD_DBG_ST3_REG_s            DcphyCmd1CphyDbgSt3;    /* 0x5EC(RO): CMD1 CPhy DBG Status3 */
    volatile AMBA_DCPHY_MD_PPI_ST0_REG_s            DcphyCmd1CphyPpiSt0;    /* 0x5F0(RO): CMD1 CPhy PPI Status0 */
    volatile AMBA_DCPHY_MD_ADI_ST0_REG_s            DcphyCmd1CphyAdiSt0;    /* 0x5F4(RO): CMD1 CPhy ADI Status0 */
    volatile UINT32                                 Reserved23[2];          /* 0x5F8-0x5FC: Reserved */

    //Master 0 Data 2 Lane (Combo):0x6XX
    volatile AMBA_DCPHY_GNR_CON0_REG_s              DcphyCmd2GnrCon0;       /* 0x600(RW): CMD2 GNR Configuration0 */
    volatile AMBA_DCPHY_GNR_CON1_REG_s              DcphyCmd2GnrCon1;       /* 0x604(RW): CMD2 GNR Configuration1 */
    volatile AMBA_DCPHY_MD_ANA_CON0_REG_s           DcphyCmd2AnaCon0;       /* 0x608(RW): CMD2 ANA Configuration0 */
    volatile AMBA_DCPHY_M_ANA_CON1_REG_s            DcphyCmd2AnaCon1;       /* 0x60C(RW): CMD2 ANA Configuration1 */
    volatile AMBA_DCPHY_CMD_ANA_CON2_REG_s          DcphyCmd2AnaCon2;       /* 0x610(RW): CMD2 ANA Configuration2 */
    volatile AMBA_DCPHY_CMD_ANA_CON3_REG_s          DcphyCmd2AnaCon3;       /* 0x614(RW): CMD2 ANA Configuration3 */
    volatile UINT32                                 Reserved24[6];          /* 0x618-0x62C: Reserved */
    volatile AMBA_DCPHY_MD_TIME_CON0_REG_s          DcphyCmd2TimeCon0;      /* 0x630(RW): CMD2 TIME Configuration0 */
    volatile AMBA_DCPHY_MSD_THS1_REG_s              DcphyCmd2TimeCon1;      /* 0x634(RW): CMD2 TIME Configuration1 */
    volatile AMBA_DCPHY_MSD_THS2_REG_s              DcphyCmd2TimeCon2;      /* 0x638(RW): CMD2 TIME Configuration2 */
    volatile AMBA_DCPHY_MD_TIME_CON3_REG_s          DcphyCmd2TimeCon3;      /* 0x63C(RW): CMD2 TIME Configuration3 */
    volatile AMBA_DCPHY_M_TIME_CON4_REG_s           DcphyCmd2TimeCon4;      /* 0x640(RW): CMD2 TIME Configuration4 */
    volatile AMBA_DCPHY_CMD_DATA_CON0_REG_s         DcphyCmd2DataCon0;      /* 0x644(RW): CMD2 DATA Configuration0 */
    volatile UINT32                                 Reserved25[6];          /* 0x648-0x65C: Reserved */
    volatile AMBA_DCPHY_CMD_PSEQ_CON0_REG_s         DcphyCmd2PSeqCon0;      /* 0x660(RW): CMD2 PRGSEQ Configuration0 */
    volatile AMBA_DCPHY_CMD_PSEQ_CON1_REG_s         DcphyCmd2PSeqCon1;      /* 0x664(RW): CMD2 PRGSEQ Configuration1 */
    volatile AMBA_DCPHY_CMD_PSEQ_CON2_REG_s         DcphyCmd2PSeqCon2;      /* 0x668(RW): CMD2 PRGSEQ Configuration2 */
    volatile AMBA_DCPHY_CMD_PSEQ_CON3_REG_s         DcphyCmd2PSeqCon3;      /* 0x66C(RW): CMD2 PRGSEQ Configuration3 */
    volatile AMBA_DCPHY_TEST_CON0_REG_s             DcphyCmd2TestCon0;      /* 0x670(RW): CMD2 TEST Configuration0 */
    volatile AMBA_DCPHY_CMD_TEST_CON1_REG_s         DcphyCmd2TestCon1;      /* 0x674(RW): CMD2 TEST Configuration1 */
    volatile AMBA_DCPHY_CMSD_TEST_CON2_REG_s        DcphyCmd2TestCon2;      /* 0x678(RW): CMD2 TEST Configuration2 */
    volatile AMBA_DCPHY_CMSD_TEST_CON3_REG_s        DcphyCmd2TestCon3;      /* 0x67C(RW): CMD2 TEST Configuration3 */
    volatile AMBA_DCPHY_CMSD_TEST_CON4_REG_s        DcphyCmd2TestCon4;      /* 0x680(RW): CMD2 TEST Configuration4 */
    volatile AMBA_DCPHY_CMSD_TEST_CON5_REG_s        DcphyCmd2TestCon5;      /* 0x684(RW): CMD2 TEST Configuration5 */
    volatile AMBA_DCPHY_CMSD_TEST_CON6_REG_s        DcphyCmd2TestCon6;      /* 0x688(RW): CMD2 TEST Configuration6 */
    volatile UINT32                                 Reserved26;             /* 0x68C: Reserved */
    volatile AMBA_DCPHY_M_BIST_CON0_REG_s           DcphyCmd2BistCon0;      /* 0x690(RW): CMD2 Built-In Self Test Configuration0 */
    volatile AMBA_DCPHY_M_BIST_CON1_REG_s           DcphyCmd2BistCon1;      /* 0x694(RW): CMD2 Built-In Self Test Configuration0 */
    volatile UINT32                                 Reserved27[10];         /* 0x698-0x6BC: Reserved */
    volatile AMBA_DCPHY_MD_DBG_ST0_REG_s            DcphyCmd2DphyDbgSt0;    /* 0x6C0(RO): CMD2 DPhy DBG Status0 */
    volatile AMBA_DCPHY_MD_DBG_ST1_REG_s            DcphyCmd2DphyDbgSt1;    /* 0x6C4(RO): CMD2 DPhy DBG Status1 */
    volatile AMBA_DCPHY_MD_DBG_ST2_REG_s            DcphyCmd2DphyDbgSt2;    /* 0x6C8(RO): CMD2 DPhy DBG Status2 */
    volatile AMBA_DCPHY_MD_DBG_ST3_REG_s            DcphyCmd2DphyDbgSt3;    /* 0x6CC(RO): CMD2 DPhy DBG Status3 */
    volatile AMBA_DCPHY_MD_PPI_ST0_REG_s            DcphyCmd2DphyPpiSt0;    /* 0x6D0(RO): CMD2 DPhy PPI Status0 */
    volatile AMBA_DCPHY_MD_ADI_ST0_REG_s            DcphyCmd2DphyAdiSt0;    /* 0x6D4(RO): CMD2 DPhy ADI Status0 */
    volatile UINT32                                 Reserved28[2];          /* 0x6D8-0x6DC: Reserved */
    volatile AMBA_DCPHY_CMD_C_DBG_ST0_REG_s         DcphyCmd2CphyDbgSt0;    /* 0x6E0(RO): CMD2 CPhy DBG Status0 */
    volatile AMBA_DCPHY_MD_DBG_ST1_REG_s            DcphyCmd2CphyDbgSt1;    /* 0x6E4(RO): CMD2 CPhy DBG Status1 */
    volatile AMBA_DCPHY_MD_DBG_ST2_REG_s            DcphyCmd2CphyDbgSt2;    /* 0x6E8(RO): CMD2 CPhy DBG Status2 */
    volatile AMBA_DCPHY_MD_DBG_ST3_REG_s            DcphyCmd2CphyDbgSt3;    /* 0x6EC(RO): CMD2 CPhy DBG Status3 */
    volatile AMBA_DCPHY_MD_PPI_ST0_REG_s            DcphyCmd2CphyPpiSt0;    /* 0x6F0(RO): CMD2 CPhy PPI Status0 */
    volatile AMBA_DCPHY_MD_ADI_ST0_REG_s            DcphyCmd2CphyAdiSt0;    /* 0x6F4(RO): CMD2 CPhy ADI Status0 */
    volatile UINT32                                 Reserved29[2];          /* 0x6F8-0x6FC: Reserved */

    //MD:0x700-0x7FF
    volatile AMBA_DCPHY_GNR_CON0_REG_s              DcphyMdGnrCon0;         /* 0x700(RW): MD GNR Configuration0 */
    volatile AMBA_DCPHY_GNR_CON1_REG_s              DcphyMdGnrCon1;         /* 0x704(RW): MD GNR Configuration1 */
    volatile AMBA_DCPHY_MD_ANA_CON0_REG_s           DcphyMdAnaCon0;         /* 0x708(RW): MD ANA Configuration0 */
    volatile AMBA_DCPHY_M_ANA_CON1_REG_s            DcphyMdAnaCon1;         /* 0x70C(RW): MD ANA Configuration1 */
    volatile UINT32                                 Reserved30[8];          /* 0x710-0x72C: Reserved */
    volatile AMBA_DCPHY_MD_TIME_CON0_REG_s          DcphyMdTimeCon0;        /* 0x730(RW): MD TIME Configuration0 */
    volatile AMBA_DCPHY_MSD_THS1_REG_s              DcphyMdTimeCon1;        /* 0x734(RW): MD TIME Configuration1 */
    volatile AMBA_DCPHY_MSD_THS2_REG_s              DcphyMdTimeCon2;        /* 0x738(RW): MD TIME Configuration2 */
    volatile AMBA_DCPHY_MD_TIME_CON3_REG_s          DcphyMdTimeCon3;        /* 0x73C(RW): MD TIME Configuration3 */
    volatile AMBA_DCPHY_M_TIME_CON4_REG_s           DcphyMdTimeCon4;        /* 0x740(RW): MD TIME Configuration4 */
    volatile AMBA_DCPHY_MD_DATA_CON0_REG_s          DcphyMdDataCon0;        /* 0x744(RW): MD DATA Configuration0 */
    volatile UINT32                                 Reserved31[10];         /* 0x748-0x76C: Reserved */
    volatile AMBA_DCPHY_TEST_CON0_REG_s             DcphyMdTestCon0;        /* 0x770(RW): MD TEST Configuration0 */
    volatile AMBA_DCPHY_MCD_TEST_CON1_REG_s         DcphyMdTestCon1;        /* 0x774(RW): MD TEST Configuration1 */
    volatile UINT32                                 Reserved311;            /* 0x778: Reserved */
    volatile AMBA_DCPHY_MCDSD_TEST_CON2_REG_s       DcphyMdTestCon2;        /* 0x77C(RW): MD TEST Configuration2 */
    volatile UINT32                                 Reserved32;             /* 0x780: Reserved */
    volatile AMBA_DCPHY_MCDSD_TEST_CON3_REG_s       DcphyMdTestCon3;        /* 0x784(RW): MD TEST Configuration3 */
    volatile AMBA_DCPHY_MCDSD_TEST_CON4_REG_s       DcphyMdTestCon4;        /* 0x788(RW): MD TEST Configuration4 */
    volatile UINT32                                 Reserved321;            /* 0x78C: Reserved */
    volatile AMBA_DCPHY_M_BIST_CON0_REG_s           DcphyMdBistCon0;        /* 0x790(RW): MD Built-In Self Test Configuration0 */
    volatile AMBA_DCPHY_M_BIST_CON1_REG_s           DcphyMdBistCon1;        /* 0x794(RW): MD Built-In Self Test Configuration1 */
    volatile UINT32                                 Reserved33[10];         /* 0x798-0x7BC: Reserved */
    volatile AMBA_DCPHY_MD_DBG_ST0_REG_s            DcphyMdDbgSt0;          /* 0x7C0(RO): MD DBG Status0 */
    volatile AMBA_DCPHY_MD_DBG_ST1_REG_s            DcphyMdDbgSt1;          /* 0x7C4(RO): MD DBG Status1 */
    volatile AMBA_DCPHY_MD_DBG_ST2_REG_s            DcphyMdDbgSt2;          /* 0x7C8(RO): MD DBG Status2 */
    volatile AMBA_DCPHY_MD_DBG_ST3_REG_s            DcphyMdDbgSt3;          /* 0x7CC(RO): MD DBG Status3 */
    volatile AMBA_DCPHY_MD_PPI_ST0_REG_s            DcphyMdPpiSt0;          /* 0x7D0(RO): MD PPI Status0 */
    volatile AMBA_DCPHY_MD_ADI_ST0_REG_s            DcphyMdAdiSt0;          /* 0x7D4(RO): MD ADI Status0 */
    volatile UINT32                                 Reserved34[10];         /* 0x7D8-0x7FC: Reserved */

    volatile UINT32                                 Reserved35[64];         /* 0x800-0x8FC: Reserved */
    volatile UINT32                                 Reserved36[64];         /* 0x900-0x9FC: Reserved */
    volatile UINT32                                 Reserved37[64];         /* 0xA00-0xAFC: Reserved */

    //SC:0xB00-0BFF
    volatile AMBA_DCPHY_GNR_CON0_REG_s              DcphyScGnrCon0;         /* 0xB00(RW): SC GNR Configuration0 */
    volatile UINT32                                 DcphyScGnrCon1;         /* 0xB04(RW): SC GNR Configuration1 */
    volatile AMBA_DCPHY_SC_ANA_CON0_REG_s           DcphyScAnaCon0;         /* 0xB08(RW): SC ANA Configuration0 */
    volatile AMBA_DCPHY_SCD_ANA_CON1_REG_s          DcphyScAnaCon1;         /* 0xB0C(RW): SC ANA Configuration1 */
    volatile AMBA_DCPHY_SC_ANA_CON2_REG_s           DcphyScAnaCon2;         /* 0xB10(RW): SC ANA Configuration2 */
    volatile UINT32                                 DcphyScAnaCon3;         /* 0xB14(RW): SC ANA Configuration3 */
    volatile UINT32                                 DcphyScAnaCon4;         /* 0xB18(RW): SC ANA Configuration4 */
    volatile UINT32                                 DcphyScAnaCon5;         /* 0xB1C(RW): SC ANA Configuration5 */
    volatile UINT32                                 Reserved38[4];          /* 0xB20-0xB2C: Reserved */
    volatile UINT32                                 DcphyScTimeCon0;        /* 0xB30(RW): SC TIME Configuration0 */
    volatile UINT32                                 Reserved39[3];          /* 0xB34-0xB3C: Reserved */
    volatile UINT32                                 DcphyScDataCon0;        /* 0xB40(RW): SC DATA Configuration0 */
    volatile UINT32                                 Reserved40[11];         /* 0xB44-0xB6C: Reserved */
    volatile UINT32                                 DcphyScTestCon0;        /* 0xB70(RW): SC TEST Configuration0 */
    volatile UINT32                                 DcphyScTestCon1;        /* 0xB74(RW): SC TEST Configuration1 */
    volatile UINT32                                 Reserved41[26];         /* 0xB78-0xBDC: Reserved */
    volatile AMBA_DCPHY_SC_DBG_ST0_REG_s            DcphyScDbgSt0;          /* 0xBE0(RO): SC DBG Status0 */
    volatile AMBA_DCPHY_SC_DBG_ST1_REG_s            DcphyScDbgSt1;          /* 0xBE4(RO): SC DBG Status1 */
    volatile AMBA_DCPHY_SC_PPI_ST0_REG_s            DcphyScPpiSt0;          /* 0xBE8(RO): SC PPI Status0 */
    volatile AMBA_DCPHY_SC_ADI_ST0_REG_s            DcphyScAdiSt0;          /* 0xBEC(RO): SC ADI Status0 */
    volatile UINT32                                 Reserved42[4];          /* 0xBF0-0xBFC: Reserved */

    /* CSD:0xc00-0xefc */
    /* Slave 0 Data 0 Lane (Combo):0xCXX */
    /* Slave 0 Data 1 Lane (Combo):0xDXX */
    /* Slave 0 Data 2 Lane (Combo):0xEXX */
    AMBA_DCPHY_CSD_REG_s                            DcphyCsd[3];            /* 0xC00-0xEFC*/

    /* SD:0xF00-0xFFF */
    volatile AMBA_DCPHY_GNR_CON0_REG_s              DcphySdGnrCon0;         /* 0xF00(RW): SD GNR Configuration0 */
    volatile UINT32                                 DcphySdGnrCon1;         /* 0xF04(RW): SD GNR Configuration1 */
    volatile AMBA_DCPHY_SD_ANA_CON0_REG_s           DcphySdAnaCon0;         /* 0xF08(RW): SD ANA Configuration0 */
    volatile AMBA_DCPHY_SCD_ANA_CON1_REG_s          DcphySdAnaCon1;         /* 0xF0C(RW): SD ANA Configuration1 */
    volatile AMBA_DCPHY_SD_ANA_CON2_REG_s           DcphySdAnaCon2;         /* 0xF10(RW): SD ANA Configuration2 */
    volatile UINT32                                 DcphySdAnaCon3;         /* 0xF14(RW): SD ANA Configuration3 */
    volatile UINT32                                 DcphySdAnaCon4;         /* 0xF18(RW): SD ANA Configuration4 */
    volatile UINT32                                 Reserved64[5];          /* 0xF1C-0xF2C: Reserved */
    volatile AMBA_DCPHY_SD_TIME_CON0_REG_s          DcphySdTimeCon0;        /* 0xF30(RW): SD TIME Configuration0 */
    volatile AMBA_DCPHY_SD_TIME_CON1_REG_s          DcphySdTimeCon1;        /* 0xF34(RW): SD TIME Configuration1 */
    volatile UINT32                                 DcphySdDataCon0;        /* 0xF38(RW): SD DATA Configuration0 */
    volatile UINT32                                 Reserved65;             /* 0xF3C: Reserved */
    volatile AMBA_DCPHY_SD_DESKEW_CON0_REG_s        DcphySdDeskewCon0;      /* 0xF40(RW): SD DESKEW Configuration0 */
    volatile UINT32                                 DcphySdDeskewCon1;      /* 0xF44(RW): SD DESKEW Configuration1 */
    volatile UINT32                                 DcphySdDeskewCon2;      /* 0xF48(RW): SD DESKEW Configuration2 */
    volatile UINT32                                 DcphySdDeskewCon3;      /* 0xF4C(RW): SD DESKEW Configuration3 */
    volatile UINT32                                 DcphySdDeskewCon4;      /* 0xF50(RW): SD DESKEW Configuration4 */
    volatile AMBA_DCPHY_SD_DESKEW_MON0_REG_s        DcphySdDeskewMon0;      /* 0xF54(RO): SD DESKEW MON0 */
    volatile AMBA_DCPHY_SD_DESKEW_MON0_REG_s        DcphySdDeskewMon1;      /* 0xF58(RO): SD DESKEW MON1 */
    volatile UINT32                                 Reserved66[5];          /* 0xF5C-0xF6C: Reserved */
    volatile UINT32                                 DcphySdTestCon0;        /* 0xF70(RW): SD TEST Configuration0 */
    volatile UINT32                                 DcphySdTestCon1;        /* 0xF74(RW): SD TEST Configuration1 */
    volatile UINT32                                 Reserved67;             /* 0xF78: Reserved */
    volatile UINT32                                 DcphySdTestCon2;        /* 0xF7C(RW): SD TEST Configuration2 */
    volatile UINT32                                 Reserved68;             /* 0xF80: Reserved */
    volatile UINT32                                 DcphySdTestCon3;        /* 0xF84(RW): SD TEST Configuration3 */
    volatile UINT32                                 DcphySdTestCon4;        /* 0xF88(RW): SD TEST Configuration4 */
    volatile UINT32                                 Reserved69;             /* 0xF8C: Reserved */
    volatile AMBA_DCPHY_SD_BIST_CON0_REG_s          DcphySdBistCon0;        /* 0xF90(RW): SD Built-In Self Test Configuration0 */
    volatile AMBA_DCPHY_MSD_THS1_REG_s              DcphySdBistCon1;        /* 0xF94(RW): SD Built-In Self Test Configuration1 */
    volatile AMBA_DCPHY_MSD_THS2_REG_s              DcphySdBistCon2;        /* 0xF98(RW): SD Built-In Self Test Configuration2 */
    volatile UINT32                                 Reserved70;             /* 0xF9C: Reserved */
    volatile AMBA_DCPHY_SD_PKT_MON_CON0_REG_s       DcphySdPktMonCon0;      /* 0xFA0(RW): SD PKT MON Configuration0 */
    volatile UINT32                                 Reserved71[4];          /* 0xFA4-0xFB0: Reserved */
    volatile AMBA_DCPHY_SD_PKT_PRE_REG_s            DcphySdPktSt0;          /* 0xFB4(RO): SD PKT Status0 */
    volatile AMBA_DCPHY_SD_PKT_PRE_REG_s            DcphySdPktSt1;          /* 0xFB8(RO): SD PKT Status1 */
    volatile UINT32                                 Reserved72;             /* 0xFBC: Reserved */
    volatile AMBA_DCPHY_PKT_CUR_REG_s               DcphySdPktSt2;          /* 0xFC0(RO): SD PKT Status2 */
    volatile AMBA_DCPHY_PKT_CUR_REG_s               DcphySdPktSt3;          /* 0xFC4(RO): SD PKT Status3 */
    volatile UINT32                                 Reserved721;            /* 0xFC8: Reserved */
    volatile AMBA_DCPHY_SD_PKT_ST4_REG_s            DcphySdPktSt4;          /* 0xFCC(RO): SD PKT Status4 */
    volatile AMBA_DCPHY_SD_PKT_ST5_REG_s            DcphySdPktSt5;          /* 0xFD0(RO): SD PKT Status5 */
    volatile AMBA_DCPHY_SD_PKT_ST6_REG_s            DcphySdPktSt6;          /* 0xFD4(RO): SD PKT Status6 */
    volatile AMBA_DCPHY_SD_PKT_ST7_REG_s            DcphySdPktSt7;          /* 0xFD8(RO): SD PKT Status7 */
    volatile UINT32                                 Reserved73;             /* 0xFDC: Reserved */
    volatile AMBA_DCPHY_SD_DBG_ST0_REG_s            DcphySdDphyDbgSt0;      /* 0xFE0(RO): SD DBG Status0 */
    volatile AMBA_DCPHY_SD_DBG_ST1_REG_s            DcphySdDphyDbgSt1;      /* 0xFE4(RO): SD DBG Status1 */
    volatile AMBA_DCPHY_SD_PPI_ST0_REG_s            DcphySdDphyPpiSt0;      /* 0xFE8(RO): SD PPI Status0 */
    volatile AMBA_DCPHY_SD_ADI_ST0_REG_s            DcphySdDphyAdiSt0;      /* 0xFEC(RO): SD ADI Status0 */

} AMBA_DCPHY_REG_s;
#endif
/*
 * Defined in AmbaMmioBase.asm
 */
extern AMBA_DCPHY_REG_s * pAmbaDCPHY_Regs[2];

#endif /* AMBA_REG_CPHY_H */
