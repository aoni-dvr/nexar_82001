/**
 *  @file AmbaB8Reg_PHY.h
 *
 *  @copyright Copyright (c) 2018 Ambarella, Inc.
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
 *  @details Definitions & Constants for B8 PHY Control Registers
 *
 */

#ifndef AMBA_B8_REG_PHY_H
#define AMBA_B8_REG_PHY_H

/*-----------------------------------------------------------------------------------*\
 * B8 PHY : Serdes tx setup reg Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  SerdesTxSetup:                   1;       /* [0] */
    UINT32  Reserved0:                       31;      /* [31:1] */
} B8_SERDES_TX_SETUP_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B8 PHY  : Serdes tx config reg Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  SerdesTxConfig:                  1;       /* [0] */
    UINT32  Reserved0:                       31;      /* [31:1] */
} B8_SERDES_TX_CONFIG_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B8 PHY  : Serdes tx standby reg Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  SerdesTxStandby:                 1;       /* [0] */
    UINT32  Reserved0:                       31;      /* [31:1] */
} B8_SERDES_TX_STANDBY_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B8 PHY  : Serdes tx reset reg Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  SerdesTxReset:                   1;       /* [0] */
    UINT32  Reserved0:                       31;      /* [31:1] */
} B8_SERDES_TX_RESET_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B8 PHY  : Serdes rx setup reg Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  SerdesRxSetup:                   1;       /* [0] */
    UINT32  Reserved0:                       31;      /* [31:1] */
} B8_SERDES_RX_SETUP_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B8 PHY  : Serdes rx config reg Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  SerdesRxConfig:                  1;       /* [0] */
    UINT32  Reserved0:                       31;      /* [31:1] */
} B8_SERDES_RX_CONFIG_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B8 PHY  : Serdes rx standby reg Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  SerdesRxStandby:                 1;       /* [0] */
    UINT32  Reserved0:                       31;      /* [31:1] */
} B8_SERDES_RX_STANDBY_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B8 PHY  : Serdes rx reset reg Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  SerdesRxReset:                   1;       /* [0] */
    UINT32  Reserved0:                       31;      /* [31:1] */
} B8_SERDES_RX_RESET_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B8 PHY  : Rct pwr tx tdsetup reg Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  TxDsetupTmr:                     19;      /* [18:0] */
    UINT32  Reserved0:                       13;      /* [31:19] */
} B8_RCT_PWR_TX_TDSETUP_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B8 PHY  : Rct pwr tx tpsetup reg Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  TxPsetupTmr:                     19;      /* [18:0] */
    UINT32  Reserved0:                       13;      /* [31:19] */
} B8_RCT_PWR_TX_TPSETUP_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B8 PHY  : Rct pwr tx tchk reg Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  TxChkTimeoutTmr:                 26;      /* [25:0] */
    UINT32  Reserved0:                       6;       /* [31:26] */
} B8_RCT_PWR_TX_TCHK_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B8 PHY  : Rct pwr rx tdsetup reg Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  RxDsetupTmr:                     19;      /* [18:0] */
    UINT32  Reserved0:                       13;      /* [31:19] */
} B8_RCT_PWR_RX_TDSETUP_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B8 PHY  : Rct pwr rx tpsetup reg Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  RxPsetupTmr:                     19;      /* [18:0] */
    UINT32  Reserved0:                       13;      /* [31:19] */
} B8_RCT_PWR_RX_TPSETUP_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B8 PHY  : Rct modechg ctrl reg Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  ModechgEnFromPhase2:             1;       /* [0] */
    UINT32  Reserved0:                       3;       /* [3:1] */
    UINT32  ModechgEnFromStr:                1;       /* [4] */
    UINT32  Reserved1:                       27;      /* [31:5] */
} B8_RCT_MODECHG_CTRL_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B8 PHY  : Mc mphy rx rst reg Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  McMphyRxRstCycle:                16;      /* [15:0] */
    UINT32  McMphyRxRstPre:                  6;       /* [21:16] */
    UINT32  Reserved0:                       10;      /* [31:22] */
} B8_MC_MPHY_RX_RST_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B8 PHY  : Mc mphy rx turn around p1 reg Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  McMphyRxTurnAroundP1Val:         16;      /* [15:0] */
    UINT32  Reserved0:                       16;      /* [31:16] */
} B8_MC_MPHY_RX_TURN_AROUND_P1_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B8 PHY  : Mc mphy rx cdr lock reg Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  McMphyRxCdrLockVal:              18;      /* [17:0] */
    UINT32  Reserved0:                       14;      /* [31:18] */
} B8_MC_MPHY_RX_CDR_LOCK_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B8 PHY  : Mc mphy rx timeout reg Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  McMphyRxTimeoutVal:              26;      /* [25:0] */
    UINT32  Reserved0:                       6;       /* [31:26] */
} B8_MC_MPHY_RX_TIMEOUT_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B8 PHY  : Mc mphy rx turn around p2 reg Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  McMphyRxTurnAroundP2Val:         16;      /* [15:0] */
    UINT32  Reserved0:                       16;      /* [31:16] */
} B8_MC_MPHY_RX_TURN_AROUND_P2_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B8 PHY  : Rx link heartbeat reg Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  RxLinkHeartbeat:                 1;       /* [0] */
    UINT32  Reserved0:                       31;      /* [31:1] */
} B8_RX_LINK_HEARTBEAT_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B8 PHY  : Dsi 8lane mode reg Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  RctDsi_8laneMode:                1;       /* [0] */
    UINT32  Reserved0:                       31;      /* [31:1] */
} B8_DSI_8LANE_MODE_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B8 PHY  : Dsi0 ctrl reg0 reg Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  RctDsi0TxClkgenEn:               1;       /* [0] */
    UINT32  RctDsi0TxClkescEn:               1;       /* [1] */
    UINT32  RctDsi0TxClkoutPol:              1;       /* [2] */
    UINT32  Reserved0:                       1;       /* [3] */
    UINT32  RctDsi0TxClkescDiv:              3;       /* [6:4] */
    UINT32  Reserved1:                       1;       /* [7] */
    UINT32  RctDsi0TxBiasEn:                 1;       /* [8] */
    UINT32  RctDsi0TxIbCtrl:                 3;       /* [11:9] */
    UINT32  Reserved2:                       4;       /* [15:12] */
    UINT32  RctDsi0TxVmdrv:                  1;       /* [16] */
    UINT32  RctDsi0TxVcmset:                 3;       /* [19:17] */
    UINT32  RctDsi0TxPib:                    4;       /* [23:20] */
    UINT32  RctDsi0TxLvdsResEn:              1;       /* [24] */
    UINT32  Reserved3:                       3;       /* [27:25] */
    UINT32  RctDsi0TxLvdsRsel:               4;       /* [31:28] */
} B8_DSI0_CTRL_REG0_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B8 PHY  : Dsi0 ctrl reg1 reg Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  RctDsi0TxSlewCtrl:               3;       /* [2:0] */
    UINT32  Reserved0:                       1;       /* [3] */
    UINT32  RctDsi0TxLpDly:                  2;       /* [5:4] */
    UINT32  Reserved1:                       26;      /* [31:6] */
} B8_DSI0_CTRL_REG1_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B8 PHY  : Dsi0 ctrl reg2 reg Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  RctDsi0TxEnableClk:              1;       /* [0] */
    UINT32  RctDsi0TxEnable:                 4;       /* [4:1] */
    UINT32  Reserved0:                       3;       /* [7:5] */
    UINT32  RctDsi0ForceInitClk:             1;       /* [8] */
    UINT32  Reserved1:                       3;       /* [11:9] */
    UINT32  RctDsi0ForceInit:                4;       /* [15:12] */
    UINT32  RctDsi0ForceStopClk:             1;       /* [16] */
    UINT32  Reserved2:                       3;       /* [19:17] */
    UINT32  RctDsi0ForceStop:                4;       /* [23:20] */
    UINT32  RctDsi0LpdtTimeoutCtrl:          5;       /* [28:24] */
    UINT32  Reserved3:                       3;       /* [31:29] */
} B8_DSI0_CTRL_REG2_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B8 PHY  : Dsi0 ctrl reg3 reg Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  RctDsi0InitTxCtrl:               6;       /* [5:0] */
    UINT32  Reserved0:                       2;       /* [7:6] */
    UINT32  RctDsi0ClkTrailCtrl:             5;       /* [12:8] */
    UINT32  Reserved1:                       3;       /* [15:13] */
    UINT32  RctDsi0ClkZeroCtrl:              6;       /* [21:16] */
    UINT32  Reserved2:                       2;       /* [23:22] */
    UINT32  RctDsi0ClkPrepareCtrl:           6;       /* [29:24] */
    UINT32  RctDsi0ClkContClkMode:           1;       /* [30] */
    UINT32  Reserved3:                       1;       /* [31] */
} B8_DSI0_CTRL_REG3_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B8 PHY  : Dsi0 ctrl reg4 reg Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  RctDsi0HsZeroCtrl:               6;       /* [5:0] */
    UINT32  Reserved0:                       2;       /* [7:6] */
    UINT32  RctDsi0HsPrepareCtrl:            6;       /* [13:8] */
    UINT32  Reserved1:                       2;       /* [15:14] */
    UINT32  RctDsi0HsTrailCtrl:              5;       /* [20:16] */
    UINT32  Reserved2:                       3;       /* [23:21] */
    UINT32  RctDsi0HsLpxCtrl:                8;       /* [31:24] */
} B8_DSI0_CTRL_REG4_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B8 PHY  : Dsi0 ctrl reg5 reg Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  RctDsi0TxPrbsEn:                 1;       /* [0] */
    UINT32  Reserved0:                       7;       /* [7:1] */
    UINT32  RctDsi0TxClkMonitorEn:           1;       /* [8] */
    UINT32  Reserved1:                       3;       /* [11:9] */
    UINT32  RctDsi0LbStart:                  1;       /* [12] */
    UINT32  Reserved2:                       19;      /* [31:13] */
} B8_DSI0_CTRL_REG5_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B8 PHY  : Dsi0 ctrl aux0 reg Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  AuxCtrl0TxAuxEn:                 1;       /* [0] */
    UINT32  AuxCtrl0TxPreEn:                 1;       /* [1] */
    UINT32  Reserved0:                       2;       /* [3:2] */
    UINT32  AuxCtrl0TxPre:                   3;       /* [6:4] */
    UINT32  Reserved1:                       1;       /* [7] */
    UINT32  AuxCtrl0TxModeSel:               2;       /* [9:8] */
    UINT32  Reserved2:                       2;       /* [11:10] */
    UINT32  AuxCtrl0TxUlvcm:                 1;       /* [12] */
    UINT32  AuxCtrl0FpdHlfrate:              1;       /* [13] */
    UINT32  Reserved3:                       1;       /* [14] */
    UINT32  AuxCtrl0TxTestpatEn:             1;       /* [15] */
    UINT32  AuxCtrl0TxTestpat:               8;       /* [23:16] */
    UINT32  AuxCtrl0ObsvSel:                 3;       /* [26:24] */
    UINT32  Reserved4:                       5;       /* [31:27] */
} B8_DSI0_CTRL_AUX0_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B8 PHY  : Dsi0 obsv reg0 reg Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  Dsi0RctTxState:                  4;       /* [3:0] */
    UINT32  Dsi0RctEscState0:                3;       /* [6:4] */
    UINT32  Reserved0:                       1;       /* [7] */
    UINT32  Dsi0RctEscState1:                1;       /* [8] */
    UINT32  Dsi0RctEscState2:                1;       /* [9] */
    UINT32  Dsi0RctEscState3:                1;       /* [10] */
    UINT32  Reserved1:                       5;       /* [15:11] */
    UINT32  Dsi0RctEscState4:                1;       /* [16] */
    UINT32  Dsi0RctEscState5:                1;       /* [17] */
    UINT32  Reserved2:                       6;       /* [23:18] */
    UINT32  Dsi0RctEscState6:                8;       /* [31:24] */
} B8_DSI0_OBSV_REG0_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B8 PHY  : Dsi0 obsv reg1 reg Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  Dsi0RctEscCmd:                   8;       /* [7:0] */
    UINT32  Reserved0:                       8;       /* [15:8] */
    UINT32  Dsi0RctHsData:                   8;       /* [23:16] */
    UINT32  Reserved1:                       8;       /* [31:24] */
} B8_DSI0_OBSV_REG1_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B8 PHY  : Dsi1 ctrl reg0 reg Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  RctDsi1TxClkgenEn:               1;       /* [0] */
    UINT32  RctDsi1TxClkescEn:               1;       /* [1] */
    UINT32  RctDsi1TxClkoutPol:              1;       /* [2] */
    UINT32  Reserved0:                       1;       /* [3] */
    UINT32  RctDsi1TxClkescDiv:              3;       /* [6:4] */
    UINT32  Reserved1:                       1;       /* [7] */
    UINT32  RctDsi1TxBiasEn:                 1;       /* [8] */
    UINT32  RctDsi1TxIbCtrl:                 3;       /* [11:9] */
    UINT32  Reserved2:                       4;       /* [15:12] */
    UINT32  RctDsi1TxVmdrv:                  1;       /* [16] */
    UINT32  RctDsi1TxVcmset:                 3;       /* [19:17] */
    UINT32  RctDsi1TxPib:                    4;       /* [23:20] */
    UINT32  RctDsi1TxLvdsResEn:              1;       /* [24] */
    UINT32  Reserved3:                       3;       /* [27:25] */
    UINT32  RctDsi1TxLvdsRsel:               4;       /* [31:28] */
} B8_DSI1_CTRL_REG0_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B8 PHY  : Dsi1 ctrl reg1 reg Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  RctDsi1TxSlewCtrl:               3;       /* [2:0] */
    UINT32  Reserved0:                       1;       /* [3] */
    UINT32  RctDsi1TxLpDly:                  2;       /* [5:4] */
    UINT32  Reserved1:                       26;      /* [31:6] */
} B8_DSI1_CTRL_REG1_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B8 PHY  : Dsi1 ctrl reg2 reg Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  RctDsi1TxEnableClk:              1;       /* [0] */
    UINT32  RctDsi1TxEnable:                 4;       /* [4:1] */
    UINT32  Reserved0:                       3;       /* [7:5] */
    UINT32  RctDsi1ForceInitClk:             1;       /* [8] */
    UINT32  Reserved1:                       3;       /* [11:9] */
    UINT32  RctDsi1ForceInit:                4;       /* [15:12] */
    UINT32  RctDsi1ForceStopClk:             1;       /* [16] */
    UINT32  Reserved2:                       3;       /* [19:17] */
    UINT32  RctDsi1ForceStop:                4;       /* [23:20] */
    UINT32  RctDsi1LpdtTimeoutCtrl:          5;       /* [28:24] */
    UINT32  Reserved3:                       3;       /* [31:29] */
} B8_DSI1_CTRL_REG2_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B8 PHY  : Dsi1 ctrl reg3 reg Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  RctDsi1InitTxCtrl:               6;       /* [5:0] */
    UINT32  Reserved0:                       2;       /* [7:6] */
    UINT32  RctDsi1ClkTrailCtrl:             5;       /* [12:8] */
    UINT32  Reserved1:                       3;       /* [15:13] */
    UINT32  RctDsi1ClkZeroCtrl:              6;       /* [21:16] */
    UINT32  Reserved2:                       2;       /* [23:22] */
    UINT32  RctDsi1ClkPrepareCtrl:           6;       /* [29:24] */
    UINT32  RctDsi1ClkContClkMode:           1;       /* [30] */
    UINT32  Reserved3:                       1;       /* [31] */
} B8_DSI1_CTRL_REG3_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B8 PHY  : Dsi1 ctrl reg4 reg Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  RctDsi1HsZeroCtrl:               6;       /* [5:0] */
    UINT32  Reserved0:                       2;       /* [7:6] */
    UINT32  RctDsi1HsPrepareCtrl:            6;       /* [13:8] */
    UINT32  Reserved1:                       2;       /* [15:14] */
    UINT32  RctDsi1HsTrailCtrl:              5;       /* [20:16] */
    UINT32  Reserved2:                       3;       /* [23:21] */
    UINT32  RctDsi1HsLpxCtrl:                8;       /* [31:24] */
} B8_DSI1_CTRL_REG4_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B8 PHY  : Dsi1 ctrl reg5 reg Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  RctDsi1TxPrbsEn:                 1;       /* [0] */
    UINT32  Reserved0:                       7;       /* [7:1] */
    UINT32  RctDsi1TxClkMonitorEn:           1;       /* [8] */
    UINT32  Reserved1:                       3;       /* [11:9] */
    UINT32  RctDsi1LbStart:                  1;       /* [12] */
    UINT32  Reserved2:                       19;      /* [31:13] */
} B8_DSI1_CTRL_REG5_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B8 PHY  : Dsi1 ctrl aux0 reg Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  AuxCtrl0TxAuxEn:                 1;       /* [0] */
    UINT32  AuxCtrl0TxPreEn:                 1;       /* [1] */
    UINT32  Reserved0:                       2;       /* [3:2] */
    UINT32  AuxCtrl0TxPre:                   3;       /* [6:4] */
    UINT32  Reserved1:                       1;       /* [7] */
    UINT32  AuxCtrl0TxModeSel:               2;       /* [9:8] */
    UINT32  Reserved2:                       2;       /* [11:10] */
    UINT32  AuxCtrl0TxUlvcm:                 1;       /* [12] */
    UINT32  AuxCtrl0FpdHlfrate:              1;       /* [13] */
    UINT32  Reserved3:                       1;       /* [14] */
    UINT32  AuxCtrl0TxTestpatEn:             1;       /* [15] */
    UINT32  AuxCtrl0TxTestpat:               8;       /* [23:16] */
    UINT32  AuxCtrl0ObsvSel:                 3;       /* [26:24] */
    UINT32  Reserved4:                       5;       /* [31:27] */
} B8_DSI1_CTRL_AUX0_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B8 PHY  : Dsi1 obsv reg0 reg Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  Dsi1RctTxState:                  4;       /* [3:0] */
    UINT32  Dsi1RctEscState0:                3;       /* [6:4] */
    UINT32  Reserved0:                       1;       /* [7] */
    UINT32  Dsi1RctEscState1:                1;       /* [8] */
    UINT32  Dsi1RctEscState2:                1;       /* [9] */
    UINT32  Dsi1RctEscState3:                1;       /* [10] */
    UINT32  Reserved1:                       5;       /* [15:11] */
    UINT32  Dsi1RctEscState4:                1;       /* [16] */
    UINT32  Dsi1RctEscState5:                1;       /* [17] */
    UINT32  Reserved2:                       6;       /* [23:18] */
    UINT32  Dsi1RctEscState6:                8;       /* [31:24] */
} B8_DSI1_OBSV_REG0_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B8 PHY  : Dsi1 obsv reg1 reg Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  Dsi1RctEscCmd:                   8;       /* [7:0] */
    UINT32  Reserved0:                       8;       /* [15:8] */
    UINT32  Dsi1RctHsData:                   8;       /* [23:16] */
    UINT32  Reserved1:                       8;       /* [31:24] */
} B8_DSI1_OBSV_REG1_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B8 PHY  : Rct mphy ctrl0 reg Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  RctMphyCtrl0Reserved3:           12;      /* [11:0] */
    UINT32  SlvsecInitLength:                4;       /* [15:12] */
    UINT32  RctMphyCtrl0Reserved0:           4;       /* [19:16] */
    UINT32  CkRecSel:                        4;       /* [23:20] */
    UINT32  CkMonitorSourceSel:              1;       /* [24] */
    UINT32  RctMphyCtrl0Reserved2:           3;       /* [27:25] */
    UINT32  CkMonitorModeTxch0:              1;       /* [28] */
    UINT32  CkMonitorModeTxch1:              1;       /* [29] */
    UINT32  RctMphyCtrl0Reserved1:           2;       /* [31:30] */
} B8_RCT_MPHY_CTRL0_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B8 PHY  : Rct mphy ctrl1 reg Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  SlvsecSyncSymbol:                8;       /* [7:0] */
    UINT32  SlvsecDeskewSymbol:              8;       /* [15:8] */
    UINT32  SlvsecStandbySymbol:             8;       /* [23:16] */
    UINT32  SlvsecIdleCode:                  8;       /* [31:24] */
} B8_RCT_MPHY_CTRL1_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B8 PHY  : Rct mphy ctrl2 reg Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  SlvsecSyncLength:                4;       /* [3:0] */
    UINT32  SlvsecDeskewInterval:            4;       /* [7:4] */
    UINT32  SlvsecDeskewLength:              4;       /* [11:8] */
    UINT32  SlvsecStandbyLength:             4;       /* [15:12] */
    UINT32  LsDeskewInterval:                4;       /* [19:16] */
    UINT32  LsDeskewLength:                  4;       /* [23:20] */
    UINT32  LsSyncLength:                    4;       /* [27:24] */
    UINT32  RctMphyCtrl2Reserved0:           4;       /* [31:28] */
} B8_RCT_MPHY_CTRL2_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B8 PHY  : Rct mphy ctrl3 reg Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  JbertSel:                        4;       /* [3:0] */
    UINT32  RctMphyCtrl3Reserved0:           28;      /* [31:4] */
} B8_RCT_MPHY_CTRL3_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B8 PHY  : Rct mphy ctrl5 reg Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  SetupRxRxch0:                    1;       /* [0] */
    UINT32  RxStandbyDetEnRxch0:             1;       /* [1] */
    UINT32  MphyForceRxReadyRxch0:           1;       /* [2] */
    UINT32  NoRdErrorRxch0:                  1;       /* [3] */
    UINT32  MphyRstnRxRxch0:                 1;       /* [4] */
    UINT32  Reserved0:                       3;       /* [7:5] */
    UINT32  RefclkFreqRxch0:                 1;       /* [8] */
    UINT32  RctMphyCtrl5Reserved3:           3;       /* [11:9] */
    UINT32  MphySymRealignRxch0:             2;       /* [13:12] */
    UINT32  RctMphyCtrl5Reserved1:           3;       /* [16:14] */
    UINT32  SlvsecModeRxch0:                 1;       /* [17] */
    UINT32  CdrRstnBypassRxch0:              1;       /* [18] */
    UINT32  CdrRstnRxch0:                    1;       /* [19] */
    UINT32  ForceSymSyncRxch0:               4;       /* [23:20] */
    UINT32  SlvsecLoopbkEnRxch0:             1;       /* [24] */
    UINT32  SlvsecLoopbkStartRxch0:          1;       /* [25] */
    UINT32  RctMphyCtrl5Reserved0:           6;       /* [31:26] */
} B8_RCT_MPHY_CTRL5_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B8 PHY  : Rct mphy ctrl6 reg Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  RxLineresetCtrlRxch0:            5;       /* [4:0] */
    UINT32  IsExactDeskewRxch0:              1;       /* [5] */
    UINT32  IsExactEndRxch0:                 1;       /* [6] */
    UINT32  IsExactPadRxch0:                 1;       /* [7] */
    UINT32  IsExactStandbyRxch0:             1;       /* [8] */
    UINT32  IsExactStartRxch0:               1;       /* [9] */
    UINT32  MphyFastSimRxch0:                1;       /* [10] */
    UINT32  RctMphyCtrl6Reserved3:           1;       /* [11] */
    UINT32  RstnRxAfeRxch0:                  1;       /* [12] */
    UINT32  RctMphyCtrl6Reserved2:           3;       /* [15:13] */
    UINT32  ChipIdRxch0:                     2;       /* [17:16] */
    UINT32  NoDeskewRxch0:                   1;       /* [18] */
    UINT32  PrbsloopbkEnRxch0:               1;       /* [19] */
    UINT32  PrbsInTestRxch0:                 1;       /* [20] */
    UINT32  RctMphyCtrl6Reserved1:           7;       /* [27:21] */
    UINT32  Bypass_8b10bRxch0:               1;       /* [28] */
    UINT32  Reserved0:                       1;       /* [29] */
    UINT32  MphySymAlignNolockRxch0:         1;       /* [30] */
    UINT32  RctMphyCtrl6Reserved0:           1;       /* [31] */
} B8_RCT_MPHY_CTRL6_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B8 PHY  : Rct mphy ctrl7 reg Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  RctMphyCtrl7Reserved1:           15;      /* [14:0] */
    UINT32  CdrCntCtrlRxch0:                 17;      /* [31:15] */
} B8_RCT_MPHY_CTRL7_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B8 PHY  : Mphy rct obsv3 reg Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  CdrLockRxch0:                    1;       /* [0] */
    UINT32  MphyRctObsv3Reserved0:           31;      /* [31:1] */
} B8_MPHY_RCT_OBSV3_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B8 PHY  : Mphy rct obsv5 reg Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  LoopbkPassRxch0:                 1;       /* [0] */
    UINT32  LoopbkReadyRxch0:                1;       /* [1] */
    UINT32  MphyRctObsv5Reserved1:           2;       /* [3:2] */
    UINT32  MphyRxStateRxch0:                4;       /* [7:4] */
    UINT32  SerdesResObRxch0:                16;      /* [23:8] */
    UINT32  MphyRctObsv5Reserved0:           8;       /* [31:24] */
} B8_MPHY_RCT_OBSV5_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B8 PHY  : Rct mphy ctrl9 reg Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  AccumFModePiRxch0:               17;      /* [16:0] */
    UINT32  ManualPiRxch0:                   1;       /* [17] */
    UINT32  PauseLfPiRxch0:                  1;       /* [18] */
    UINT32  SgnvosInRxch0:                   1;       /* [19] */
    UINT32  PctrlInRxch0:                    8;       /* [27:20] */
    UINT32  BpLatchPiRxch0:                  1;       /* [28] */
    UINT32  IbiasmodeRxch0:                  1;       /* [29] */
    UINT32  SlvsModeRxch0:                   1;       /* [30] */
    UINT32  CdrCtrl0Reserve0Rxch0:           1;       /* [31] */
} B8_RCT_MPHY_CTRL9_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B8 PHY  : Rct mphy ctrl10 reg Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  UpdnInPiRxch0:                   5;       /* [4:0] */
    UINT32  ManualDfeVcmRxch0:               1;       /* [5] */
    UINT32  ManualCtleVcmRxch0:              1;       /* [6] */
    UINT32  ManualEqVcmRxch0:                1;       /* [7] */
    UINT32  LockSetRxch0:                    3;       /* [10:8] */
    UINT32  FlipPiRxch0:                     1;       /* [11] */
    UINT32  LockTimeRxch0:                   3;       /* [14:12] */
    UINT32  EnFPathRxch0:                    1;       /* [15] */
    UINT32  SavingRxch0:                     1;       /* [16] */
    UINT32  ObsvCalibRxch0:                  1;       /* [17] */
    UINT32  SwapCtleOffsetCalibRxch0:        1;       /* [18] */
    UINT32  FilterModeRxch0:                 1;       /* [19] */
    UINT32  UpdnFDivInRxch0:                 12;      /* [31:20] */
} B8_RCT_MPHY_CTRL10_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B8 PHY  : Rct mphy ctrl11 reg Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  VcmFilterCtrlRxch0:              3;       /* [2:0] */
    UINT32  AutoBandRxch0:                   1;       /* [3] */
    UINT32  CapPrepiCtrlRxch0:               2;       /* [5:4] */
    UINT32  CapFilterCtrlRxch0:              2;       /* [7:6] */
    UINT32  BandShiftInRxch0:                4;       /* [11:8] */
    UINT32  CfPreTuneRxch0:                  2;       /* [13:12] */
    UINT32  DacIctrlRxch0:                   2;       /* [15:14] */
    UINT32  DcoGainRxch0:                    3;       /* [18:16] */
    UINT32  ClearOfRxch0:                    1;       /* [19] */
    UINT32  BinthmShiftInRxch0:              3;       /* [22:20] */
    UINT32  EnDitherRxch0:                   1;       /* [23] */
    UINT32  FaDataTypeRxch0:                 2;       /* [25:24] */
    UINT32  ManualSlicerBufferVcmRxch0:      1;       /* [26] */
    UINT32  ObErrSymbolRxch0:                1;       /* [27] */
    UINT32  EnDsmRxch0:                      1;       /* [28] */
    UINT32  DcoPdRxch0:                      1;       /* [29] */
    UINT32  HaltDcoRxch0:                    1;       /* [30] */
    UINT32  DcoBypassRxch0:                  1;       /* [31] */
} B8_RCT_MPHY_CTRL11_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B8 PHY  : Rct mphy ctrl12 reg Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  DfebwRxch0:                      2;       /* [1:0] */
    UINT32  VrefgearRxch0:                   2;       /* [3:2] */
    UINT32  PibwRxch0:                       4;       /* [7:4] */
    UINT32  KdccRxch0:                       3;       /* [10:8] */
    UINT32  DccmodeRxch0:                    1;       /* [11] */
    UINT32  DccbiasRxch0:                    3;       /* [14:12] */
    UINT32  Seldiv4Rxch0:                    1;       /* [15] */
    UINT32  VocmdfeInRxch0:                  5;       /* [20:16] */
    UINT32  BypasspiRxch0:                   1;       /* [21] */
    UINT32  CodeInEdge1Rxch0:                6;       /* [27:22] */
    UINT32  SwapData0Rxch0:                  1;       /* [28] */
    UINT32  SwapData1Rxch0:                  1;       /* [29] */
    UINT32  SwapEdge0Rxch0:                  1;       /* [30] */
    UINT32  SwapEdge1Rxch0:                  1;       /* [31] */
} B8_RCT_MPHY_CTRL12_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B8 PHY  : Rct mphy ctrl13 reg Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  ReadyTimerRxch0:                 12;      /* [11:0] */
    UINT32  IctrlCdrRxch0:                   3;       /* [14:12] */
    UINT32  SgnvrefRxch0:                    1;       /* [15] */
    UINT32  IctrlDfeRxch0:                   3;       /* [18:16] */
    UINT32  PdDesRxch0:                      1;       /* [19] */
    UINT32  VosgearRxch0:                    2;       /* [21:20] */
    UINT32  DccWinMsbRxch0:                  2;       /* [23:22] */
    UINT32  DccStartRxch0:                   1;       /* [24] */
    UINT32  DccManualRxch0:                  1;       /* [25] */
    UINT32  DccManualLoadRxch0:              1;       /* [26] */
    UINT32  DccSwapRxch0:                    1;       /* [27] */
    UINT32  DcoFaSelRxch0:                   1;       /* [28] */
    UINT32  PderrRxch0:                      1;       /* [29] */
    UINT32  PdeyeRxch0:                      1;       /* [30] */
    UINT32  VoscalibEnRxch0:                 1;       /* [31] */
} B8_RCT_MPHY_CTRL13_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B8 PHY  : Rct mphy ctrl14 reg Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  DivFPiRxch0:                     4;       /* [3:0] */
    UINT32  DivPPiRxch0:                     4;       /* [7:4] */
    UINT32  MultiFPiRxch0:                   4;       /* [11:8] */
    UINT32  MultiPPiRxch0:                   4;       /* [15:12] */
    UINT32  IctrlCkgenRxch0:                 3;       /* [18:16] */
    UINT32  DccPrbsEnRxch0:                  1;       /* [19] */
    UINT32  SlicerbwInRxch0:                 2;       /* [21:20] */
    UINT32  DccSwapSkewRxch0:                1;       /* [22] */
    UINT32  ManualCtleOffsetRxch0:           1;       /* [23] */
    UINT32  PhasedFrGainRxch0:               4;       /* [27:24] */
    UINT32  IctrlPiRxch0:                    3;       /* [30:28] */
    UINT32  BypassctleRxch0:                 1;       /* [31] */
} B8_RCT_MPHY_CTRL14_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B8 PHY  : Rct mphy ctrl15 reg Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  AccumFModeDcoRxch0:              12;      /* [11:0] */
    UINT32  Reserved0:                       2;       /* [13:12] */
    UINT32  FreqDivSelRxch0:                 2;       /* [15:14] */
    UINT32  FlipDcoRxch0:                    1;       /* [16] */
    UINT32  PauseLfDcoRxch0:                 1;       /* [17] */
    UINT32  DctrlInRxch0:                    14;      /* [31:18] */
} B8_RCT_MPHY_CTRL15_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B8 PHY  : Rct mphy ctrl16 reg Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  PdSlicerRxch0:                   1;       /* [0] */
    UINT32  SelCalibRxch0:                   3;       /* [3:1] */
    UINT32  FreqLockModeRxch0:               1;       /* [4] */
    UINT32  RefClkSelDcoRxch0:               1;       /* [5] */
    UINT32  Reserved0:                       2;       /* [7:6] */
    UINT32  FreqAdjustRxch0:                 16;      /* [23:8] */
    UINT32  DctrlClampRxch0:                 8;       /* [31:24] */
} B8_RCT_MPHY_CTRL16_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B8 PHY  : Rct mphy ctrl17 reg Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  DivIntDcoRxch0:                  4;       /* [3:0] */
    UINT32  DivPDcoRxch0:                    4;       /* [7:4] */
    UINT32  FreqoffsetThrshRxch0:            4;       /* [11:8] */
    UINT32  ForceFreqLockRxch0:              1;       /* [12] */
    UINT32  RctMphyCtrl17Reserved1:          3;       /* [15:13] */
    UINT32  DatarateSelRxch0:                2;       /* [17:16] */
    UINT32  DcoLfDesSelRxch0:                1;       /* [18] */
    UINT32  Reserved0:                       1;       /* [19] */
    UINT32  CdrEyemonEnRxch0:                1;       /* [20] */
    UINT32  SlicerOffsetManualRxch0:         1;       /* [21] */
    UINT32  RctMphyCtrl17Reserved0:          8;       /* [29:22] */
    UINT32  FreqReacquireRxch0:              1;       /* [30] */
    UINT32  StartCalibRxch0:                 1;       /* [31] */
} B8_RCT_MPHY_CTRL17_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B8 PHY  : Rct mphy ctrl18 reg Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  Tap1InRxch0:                     6;       /* [5:0] */
    UINT32  Reserved0:                       2;       /* [7:6] */
    UINT32  Tap2InRxch0:                     5;       /* [12:8] */
    UINT32  SslmsDfeManualRxch0:             1;       /* [13] */
    UINT32  SslmsDfePauseRxch0:              1;       /* [14] */
    UINT32  CalibLoadRxch0:                  1;       /* [15] */
    UINT32  Tap3InRxch0:                     4;       /* [19:16] */
    UINT32  Tap4InRxch0:                     4;       /* [23:20] */
    UINT32  SslmsDfeGainRxch0:               3;       /* [26:24] */
    UINT32  VocmdfebufInRxch0:               5;       /* [31:27] */
} B8_RCT_MPHY_CTRL18_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B8 PHY  : Rct mphy ctrl19 reg Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  VrefInRxch0:                     7;       /* [6:0] */
    UINT32  SslmsVrefPauseRxch0:             1;       /* [7] */
    UINT32  TermResThermRxch0:               5;       /* [12:8] */
    UINT32  RctMphyCtrl19Reserved0:          2;       /* [14:13] */
    UINT32  SslmsVrefManualRxch0:            1;       /* [15] */
    UINT32  PiQInRxch0:                      6;       /* [21:16] */
    UINT32  RctMphyCtrl19Reserved1:          2;       /* [23:22] */
    UINT32  PiEyeRxch0:                      8;       /* [31:24] */
} B8_RCT_MPHY_CTRL19_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B8 PHY  : Rct mphy ctrl20 reg Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  VcmSetRxch0:                     8;       /* [7:0] */
    UINT32  OffsetCtleCodeInRxch0:           6;       /* [13:8] */
    UINT32  CdrCtrl11Reserve0Rxch0:          18;      /* [31:14] */
} B8_RCT_MPHY_CTRL20_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B8 PHY  : Rct mphy ctrl21 reg Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  CtleCctrl0InRxch0:               4;       /* [3:0] */
    UINT32  CtleCctrl1InRxch0:               4;       /* [7:4] */
    UINT32  CtleCctrl2InRxch0:               4;       /* [11:8] */
    UINT32  CtleRctrl0InRxch0:               2;       /* [13:12] */
    UINT32  CtleRctrl1InRxch0:               2;       /* [15:14] */
    UINT32  CtleRctrl2InRxch0:               2;       /* [17:16] */
    UINT32  SslmsCtleManualRxch0:            1;       /* [18] */
    UINT32  SslmsCtleRpauseRxch0:            1;       /* [19] */
    UINT32  SslmsCtleRgainRxch0:             3;       /* [22:20] */
    UINT32  RctMphyCtrl21Reserved:           1;       /* [23] */
    UINT32  IctrlEqRxch0:                    3;       /* [26:24] */
    UINT32  TermResBinaryRxch0:              5;       /* [31:27] */
} B8_RCT_MPHY_CTRL21_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B8 PHY  : Rct mphy ctrl22 reg Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  VcmDatabufCtrlpinInRxch0:        5;       /* [4:0] */
    UINT32  SslmsEqModeRxch0:                2;       /* [6:5] */
    UINT32  Reserved0:                       1;       /* [7] */
    UINT32  VcmCtleCtrlpinInRxch0:           5;       /* [12:8] */
    UINT32  SslmsVrefGainRxch0:              3;       /* [15:13] */
    UINT32  SslmsCtleCgainRxch0:             3;       /* [18:16] */
    UINT32  SslmsCtleCpauseRxch0:            1;       /* [19] */
    UINT32  TermResEnRxch0:                  1;       /* [20] */
    UINT32  RctMphyCtrl22Reserved0:          11;      /* [31:21] */
} B8_RCT_MPHY_CTRL22_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B8 PHY  : Rct mphy ctrl23 reg Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  VcmFbSelinRxch0:                 2;       /* [1:0] */
    UINT32  PdCtleRxch0:                     1;       /* [2] */
    UINT32  PdCalibRxch0:                    1;       /* [3] */
    UINT32  CtleFuncEnInRxch0:               3;       /* [6:4] */
    UINT32  DccManualInvRxch0:               1;       /* [7] */
    UINT32  StartVcmCalibDatabufRxch0:       1;       /* [8] */
    UINT32  StartOffsetCalibRxch0:           1;       /* [9] */
    UINT32  StartVcmCalibCtleRxch0:          1;       /* [10] */
    UINT32  StartChannelAdaptationRxch0:     1;       /* [11] */
    UINT32  RstnVcmCalibDatabufRxch0:        1;       /* [12] */
    UINT32  RstnOffsetCalibRxch0:            1;       /* [13] */
    UINT32  RstnVcmCalibCtleRxch0:           1;       /* [14] */
    UINT32  RstnChannelAdaptationRxch0:      1;       /* [15] */
    UINT32  LpgainOffsetRxch0:               4;       /* [19:16] */
    UINT32  LpgainVcmRxch0:                  4;       /* [23:20] */
    UINT32  VcmCalibDatabufModeRxch0:        1;       /* [24] */
    UINT32  OffsetCalibModeRxch0:            1;       /* [25] */
    UINT32  VcmCalibCtleModeRxch0:           1;       /* [26] */
    UINT32  ChannelAdaptationModeRxch0:      1;       /* [27] */
    UINT32  Ob0SelRxch0:                     4;       /* [31:28] */
} B8_RCT_MPHY_CTRL23_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B8 PHY  : Rct mphy ctrl24 reg Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  DccQInRxch0:                     12;      /* [11:0] */
    UINT32  DccIInRxch0:                     12;      /* [23:12] */
    UINT32  DccselInRxch0:                   5;       /* [28:24] */
    UINT32  CalibdoneRxch0:                  1;       /* [29] */
    UINT32  EqbufGainRxch0:                  2;       /* [31:30] */
} B8_RCT_MPHY_CTRL24_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B8 PHY  : Rct mphy ctrl25 reg Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  DccEyeInRxch0:                   12;      /* [11:0] */
    UINT32  LstxOffsetcalibModeRxch0:        1;       /* [12] */
    UINT32  DfeModeRxch0:                    1;       /* [13] */
    UINT32  StartLstxOffsetcalibRxch0:       1;       /* [14] */
    UINT32  RstnLstxOffsetcalibRxch0:        1;       /* [15] */
    UINT32  LstxOffsetcalibLpgainRxch0:      4;       /* [19:16] */
    UINT32  DfeTap0CtrlRxch0:                4;       /* [23:20] */
    UINT32  DfeTap1CtrlRxch0:                4;       /* [27:24] */
    UINT32  PdCtlePathRxch0:                 1;       /* [28] */
    UINT32  PdDatabufPathRxch0:              1;       /* [29] */
    UINT32  PdDfePathRxch0:                  1;       /* [30] */
    UINT32  PdEqRxch0:                       1;       /* [31] */
} B8_RCT_MPHY_CTRL25_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B8 PHY  : Rct mphy ctrl26 reg Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  LsRRxch0:                        4;       /* [3:0] */
    UINT32  LsIbRxch0:                       4;       /* [7:4] */
    UINT32  LsCtrRxch0:                      8;       /* [15:8] */
    UINT32  LsCocRxch0:                      4;       /* [19:16] */
    UINT32  PibCmpRxch0:                     4;       /* [23:20] */
    UINT32  PibRepRxch0:                     4;       /* [27:24] */
    UINT32  RocRxch0:                        3;       /* [30:28] */
    UINT32  PdbLstxRxch0:                    1;       /* [31] */
} B8_RCT_MPHY_CTRL26_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B8 PHY  : Rct mphy ctrl27 reg Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  PibMode0Rxch0:                   4;       /* [3:0] */
    UINT32  CdrOutSelectRxch0:               6;       /* [9:4] */
    UINT32  CodeInData0Rxch0:                6;       /* [15:10] */
    UINT32  CodeInData1Rxch0:                6;       /* [21:16] */
    UINT32  CodeInEdge0Rxch0:                6;       /* [27:22] */
    UINT32  Reserved0:                       1;       /* [28] */
    UINT32  RctMphyCtrl27Reserved0:          3;       /* [31:29] */
} B8_RCT_MPHY_CTRL27_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B8 PHY  : Rct mphy ctrl29 reg Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  SetupTxch0:                      1;       /* [0] */
    UINT32  RstnTxAfeTxch0:                  1;       /* [1] */
    UINT32  MphyRstNTxch0:                   1;       /* [2] */
    UINT32  PrbsloopbkEnTxch0:               1;       /* [3] */
    UINT32  BypSerTxch0:                     1;       /* [4] */
    UINT32  Reserved0:                       3;       /* [7:5] */
    UINT32  AcCoupledTxch0:                  1;       /* [8] */
    UINT32  PwmClk_3xTxch0:                  1;       /* [9] */
    UINT32  PwmDutycycleTxch0:               1;       /* [10] */
    UINT32  MphyFastSimTxch0:                1;       /* [11] */
    UINT32  TxPesTxch0:                      12;      /* [23:12] */
    UINT32  SlvsEcModeTxch0:                 1;       /* [24] */
    UINT32  SlvsecLoopbkEnTxch0:             1;       /* [25] */
    UINT32  SlvsecLoopbkStartTxch0:          1;       /* [26] */
    UINT32  RefclkFreqTxch0:                 1;       /* [27] */
    UINT32  GfilterBwTxch0:                  2;       /* [29:28] */
    UINT32  TxStaticModeTxch0:               1;       /* [30] */
    UINT32  TxStaticCtrlTxch0:               1;       /* [31] */
} B8_RCT_MPHY_CTRL29_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B8 PHY  : Rct mphy ctrl30 reg Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  BpGlitchFilterTxch0:             1;       /* [0] */
    UINT32  ForceLsModeRctTxch0:             1;       /* [1] */
    UINT32  MphyTxStateSelTxch0:             2;       /* [3:2] */
    UINT32  MphyAfeReadyTxch0:               1;       /* [4] */
    UINT32  GfilterCkselTxch0:               1;       /* [5] */
    UINT32  RctMphyCtrl30Reserved1:          2;       /* [7:6] */
    UINT32  LsRxRTxch0:                      4;       /* [11:8] */
    UINT32  LsRxCtrTxch0:                    8;       /* [19:12] */
    UINT32  LsRxCTxch0:                      4;       /* [23:20] */
    UINT32  TxModeTxch0:                     2;       /* [25:24] */
    UINT32  RctMphyCtrl30Reserved2:          2;       /* [27:26] */
    UINT32  TxSymbolTypeTxch0:               2;       /* [29:28] */
    UINT32  RctMphyCtrl30Reserved3:          2;       /* [31:30] */
} B8_RCT_MPHY_CTRL30_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B8 PHY  : Rct mphy ctrl31 reg Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  TxDrvPdTxch0:                    1;       /* [0] */
    UINT32  TxDriverPolarityTxch0:           1;       /* [1] */
    UINT32  TxModuleTypeTxch0:               1;       /* [2] */
    UINT32  TxSetManualTxch0:                1;       /* [3] */
    UINT32  TcCalcodeTxch0:                  8;       /* [11:4] */
    UINT32  TxHsSlewrateTxch0:               3;       /* [14:12] */
    UINT32  Reserved0:                       1;       /* [15] */
    UINT32  TxHsBiasTxch0:                   8;       /* [23:16] */
    UINT32  PdbLsRxTxch0:                    1;       /* [24] */
    UINT32  RctMphyCtrl31Reserved0:          3;       /* [27:25] */
    UINT32  JbertEnTxch0:                    1;       /* [28] */
    UINT32  Bypass_8b10bTxch0:               1;       /* [29] */
    UINT32  TxEndSkipDeskewTxch0:            1;       /* [30] */
    UINT32  TxResetBypassTxch0:              1;       /* [31] */
} B8_RCT_MPHY_CTRL31_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B8 PHY  : Rct mphy ctrl32 reg Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  RctMphyCtrl32Reserved1:          16;      /* [15:0] */
    UINT32  LsRxIbctrlTxch0:                 4;       /* [19:16] */
    UINT32  HsCtrTxch0:                      8;       /* [27:20] */
    UINT32  RctMphyCtrl32Reserved0:          4;       /* [31:28] */
} B8_RCT_MPHY_CTRL32_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B8 PHY  : Mphy rct obsv10 reg Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  MphyTxStateTxch0:                4;       /* [3:0] */
    UINT32  MphyRctObsv10Reserved1:          4;       /* [7:4] */
    UINT32  MphyTxSubStateTxch0:             5;       /* [12:8] */
    UINT32  TxSlvsecCodeSelTxch0:            3;       /* [15:13] */
    UINT32  JbertRemptyTxch0:                1;       /* [16] */
    UINT32  JbertWfullTxch0:                 1;       /* [17] */
    UINT32  MphyRctObsv10Reserved0:          14;      /* [31:18] */
} B8_MPHY_RCT_OBSV10_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B8 PHY  : Rct dphy ctrl0 reg Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  LvdsRsel:                        4;       /* [3:0] */
    UINT32  DphyCtrl0Reserved4:              4;       /* [7:4] */
    UINT32  DphyRstAfe:                      1;       /* [8] */
    UINT32  DphyCtrl0Reserved3:              3;       /* [11:9] */
    UINT32  NoVbn:                           1;       /* [12] */
    UINT32  DphyCtrl0Reserved2:              3;       /* [15:13] */
    UINT32  BitMode:                         2;       /* [17:16] */
    UINT32  DphyCtrl0Reserved1:              2;       /* [19:18] */
    UINT32  AsyncSpclk:                      1;       /* [20] */
    UINT32  DphyCtrl0Reserved0:              3;       /* [23:21] */
    UINT32  Async:                           8;       /* [31:24] */
} B8_RCT_DPHY_CTRL0_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B8 PHY  : Rct dphy ctrl1 reg Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  LvdsIbCtrl:                      2;       /* [1:0] */
    UINT32  DphyCtrl1Reserved5:              2;       /* [3:2] */
    UINT32  PdFunc:                          2;       /* [5:4] */
    UINT32  DphyCtrl1Reserved4:              2;       /* [7:6] */
    UINT32  LvdsPd:                          1;       /* [8] */
    UINT32  DphyCtrl1Reserved3:              3;       /* [11:9] */
    UINT32  DataR100Off:                     8;       /* [19:12] */
    UINT32  ClkR100Off:                      2;       /* [21:20] */
    UINT32  DphyCtrl1Reserved2:              2;       /* [23:22] */
    UINT32  DphyRst:                         2;       /* [25:24] */
    UINT32  DphyCtrl1Reserved1:              2;       /* [27:26] */
    UINT32  SlvsSync:                        2;       /* [29:28] */
    UINT32  DphyCtrl1Reserved0:              2;       /* [31:30] */
} B8_RCT_DPHY_CTRL1_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B8 PHY  : Rct dphy ctrl2 reg Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  Forcerxmode0:                    1;       /* [0] */
    UINT32  Forcerxmode1:                    1;       /* [1] */
    UINT32  Forcerxmode2:                    1;       /* [2] */
    UINT32  Forcerxmode3:                    1;       /* [3] */
    UINT32  Forcerxmode4:                    1;       /* [4] */
    UINT32  Forcerxmode5:                    1;       /* [5] */
    UINT32  Forcerxmode6:                    1;       /* [6] */
    UINT32  Forcerxmode7:                    1;       /* [7] */
    UINT32  DphyCtrl2Reserved2:              8;       /* [15:8] */
    UINT32  ForceClkHs0:                     1;       /* [16] */
    UINT32  ForceClkHs1:                     1;       /* [17] */
    UINT32  DphyCtrl2Reserved1:              2;       /* [19:18] */
    UINT32  ForceMipiHs:                     1;       /* [20] */
    UINT32  DphyCtrl2Reserved0:              11;      /* [31:21] */
} B8_RCT_DPHY_CTRL2_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B8 PHY  : Rct dphy ctrl3 reg Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  LvcmosMode:                      8;       /* [7:0] */
    UINT32  LvcmosModeSpclk:                 2;       /* [9:8] */
    UINT32  DphyCtrl3Reserved2:              2;       /* [11:10] */
    UINT32  MipiMode:                        2;       /* [13:12] */
    UINT32  DphyCtrl3Reserved1:              2;       /* [15:14] */
    UINT32  EightLaneMipiMode:               1;       /* [16] */
    UINT32  DphyCtrl3Reserved0:              15;      /* [31:17] */
} B8_RCT_DPHY_CTRL3_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B8 PHY  : Rct dphy ctrl4 reg Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  HsTermCtrl0:                     5;       /* [4:0] */
    UINT32  DphyCtrl4Reserved3:              3;       /* [7:5] */
    UINT32  HsSettleCtrl0:                   6;       /* [13:8] */
    UINT32  DphyCtrl4Reserved2:              2;       /* [15:14] */
    UINT32  InitRxCtrl0:                     7;       /* [22:16] */
    UINT32  DphyCtrl4Reserved1:              1;       /* [23] */
    UINT32  ClkMissCtrl0:                    5;       /* [28:24] */
    UINT32  DphyCtrl4Reserved0:              3;       /* [31:29] */
} B8_RCT_DPHY_CTRL4_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B8 PHY  : Rct dphy ctrl5 reg Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  ClkSettleCtrl0:                  6;       /* [5:0] */
    UINT32  DphyCtrl5Reserved1:              2;       /* [7:6] */
    UINT32  ClkTermCtrl0:                    5;       /* [12:8] */
    UINT32  DphyCtrl5Reserved0:              19;      /* [31:13] */
} B8_RCT_DPHY_CTRL5_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B8 PHY  : Rct dphy ctrl6 reg Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  HsTermCtrl1:                     5;       /* [4:0] */
    UINT32  DphyCtrl6Reserved3:              3;       /* [7:5] */
    UINT32  HsSettleCtrl1:                   6;       /* [13:8] */
    UINT32  DphyCtrl6Reserved2:              2;       /* [15:14] */
    UINT32  InitRxCtrl1:                     7;       /* [22:16] */
    UINT32  DphyCtrl6Reserved1:              1;       /* [23] */
    UINT32  ClkMissCtrl1:                    5;       /* [28:24] */
    UINT32  DphyCtrl6Reserved0:              3;       /* [31:29] */
} B8_RCT_DPHY_CTRL6_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B8 PHY  : Rct dphy ctrl7 reg Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  ClkSettleCtrl1:                  6;       /* [5:0] */
    UINT32  DphyCtrl7Reserved1:              2;       /* [7:6] */
    UINT32  ClkTermCtrl1:                    5;       /* [12:8] */
    UINT32  DphyCtrl7Reserved0:              19;      /* [31:13] */
} B8_RCT_DPHY_CTRL7_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B8 PHY  : Rct dphy ctrl8 reg Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  RxStateDataSel:                  4;       /* [3:0] */
    UINT32  RxStateClkSel:                   2;       /* [5:4] */
    UINT32  DphyCtrl8Reserved1:              6;       /* [11:6] */
    UINT32  DataRctDebSel:                   4;       /* [15:12] */
    UINT32  RstNRctDeb:                      1;       /* [16] */
    UINT32  DphyCtrl8Reserved0:              15;      /* [31:17] */
} B8_RCT_DPHY_CTRL8_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B8 PHY  : Rct dphy ctrl9 reg Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  DphyLbEn0:                       1;       /* [0] */
    UINT32  DphyCtrl9Reserved5:              3;       /* [3:1] */
    UINT32  DphyLbClr0:                      1;       /* [4] */
    UINT32  DphyCtrl9Reserved4:              3;       /* [7:5] */
    UINT32  DphyLbOutsel0:                   3;       /* [10:8] */
    UINT32  DphyCtrl9Reserved3:              5;       /* [15:11] */
    UINT32  DphyLbEn1:                       1;       /* [16] */
    UINT32  DphyCtrl9Reserved2:              3;       /* [19:17] */
    UINT32  DphyLbClr1:                      1;       /* [20] */
    UINT32  DphyCtrl9Reserved1:              3;       /* [23:21] */
    UINT32  DphyLbOutsel1:                   3;       /* [26:24] */
    UINT32  DphyCtrl9Reserved0:              5;       /* [31:27] */
} B8_RCT_DPHY_CTRL9_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B8 PHY  : Dphy rct obsv0 reg Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  DphyRctObsv0Reserved2:           4;       /* [3:0] */
    UINT32  Rxclkactivehs0:                  1;       /* [4] */
    UINT32  Rxclkactivehs1:                  1;       /* [5] */
    UINT32  DphyRctObsv0Reserved1:           2;       /* [7:6] */
    UINT32  Rxclkstopstate0:                 1;       /* [8] */
    UINT32  Rxclkstopstate1:                 1;       /* [9] */
    UINT32  DphyRctObsv0Reserved0:           22;      /* [31:10] */
} B8_DPHY_RCT_OBSV0_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B8 PHY  : Dphy rct obsv1 reg Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  Errcontrol0:                     1;       /* [0] */
    UINT32  Errcontrol1:                     1;       /* [1] */
    UINT32  Errcontrol2:                     1;       /* [2] */
    UINT32  Errcontrol3:                     1;       /* [3] */
    UINT32  Errcontrol4:                     1;       /* [4] */
    UINT32  Errcontrol5:                     1;       /* [5] */
    UINT32  Errcontrol6:                     1;       /* [6] */
    UINT32  Errcontrol7:                     1;       /* [7] */
    UINT32  Errsoths0:                       1;       /* [8] */
    UINT32  Errsoths1:                       1;       /* [9] */
    UINT32  Errsoths2:                       1;       /* [10] */
    UINT32  Errsoths3:                       1;       /* [11] */
    UINT32  Errsoths4:                       1;       /* [12] */
    UINT32  Errsoths5:                       1;       /* [13] */
    UINT32  Errsoths6:                       1;       /* [14] */
    UINT32  Errsoths7:                       1;       /* [15] */
    UINT32  Errsotsynchs0:                   1;       /* [16] */
    UINT32  Errsotsynchs1:                   1;       /* [17] */
    UINT32  Errsotsynchs2:                   1;       /* [18] */
    UINT32  Errsotsynchs3:                   1;       /* [19] */
    UINT32  Errsotsynchs4:                   1;       /* [20] */
    UINT32  Errsotsynchs5:                   1;       /* [21] */
    UINT32  Errsotsynchs6:                   1;       /* [22] */
    UINT32  Errsotsynchs7:                   1;       /* [23] */
    UINT32  DphyRctObsv1Reserved0:           8;       /* [31:24] */
} B8_DPHY_RCT_OBSV1_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B8 PHY  : Dphy rct obsv2 reg Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  Rxvalidhs0:                      1;       /* [0] */
    UINT32  Rxvalidhs1:                      1;       /* [1] */
    UINT32  Rxvalidhs2:                      1;       /* [2] */
    UINT32  Rxvalidhs3:                      1;       /* [3] */
    UINT32  Rxvalidhs4:                      1;       /* [4] */
    UINT32  Rxvalidhs5:                      1;       /* [5] */
    UINT32  Rxvalidhs6:                      1;       /* [6] */
    UINT32  Rxvalidhs7:                      1;       /* [7] */
    UINT32  DphyRctObsv2Reserved0:           24;      /* [31:8] */
} B8_DPHY_RCT_OBSV2_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B8 PHY  : Dphy rct obsv3 reg Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  Rxsynchs0:                       1;       /* [0] */
    UINT32  Rxsynchs1:                       1;       /* [1] */
    UINT32  Rxsynchs2:                       1;       /* [2] */
    UINT32  Rxsynchs3:                       1;       /* [3] */
    UINT32  Rxsynchs4:                       1;       /* [4] */
    UINT32  Rxsynchs5:                       1;       /* [5] */
    UINT32  Rxsynchs6:                       1;       /* [6] */
    UINT32  Rxsynchs7:                       1;       /* [7] */
    UINT32  Rxactivehs0:                     1;       /* [8] */
    UINT32  Rxactivehs1:                     1;       /* [9] */
    UINT32  Rxactivehs2:                     1;       /* [10] */
    UINT32  Rxactivehs3:                     1;       /* [11] */
    UINT32  Rxactivehs4:                     1;       /* [12] */
    UINT32  Rxactivehs5:                     1;       /* [13] */
    UINT32  Rxactivehs6:                     1;       /* [14] */
    UINT32  Rxactivehs7:                     1;       /* [15] */
    UINT32  Rxstopstate0:                    1;       /* [16] */
    UINT32  Rxstopstate1:                    1;       /* [17] */
    UINT32  Rxstopstate2:                    1;       /* [18] */
    UINT32  Rxstopstate3:                    1;       /* [19] */
    UINT32  Rxstopstate4:                    1;       /* [20] */
    UINT32  Rxstopstate5:                    1;       /* [21] */
    UINT32  Rxstopstate6:                    1;       /* [22] */
    UINT32  Rxstopstate7:                    1;       /* [23] */
    UINT32  DphyRctObsv3Reserved0:           8;       /* [31:24] */
} B8_DPHY_RCT_OBSV3_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B8 PHY  : Dphy rct obsv5 reg Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  RxStateDataO:                    3;       /* [2:0] */
    UINT32  DphyRctObsv5Reserved1:           1;       /* [3] */
    UINT32  RxStateClkO:                     4;       /* [7:4] */
    UINT32  DphyRctObsv5Reserved0:           24;      /* [31:8] */
} B8_DPHY_RCT_OBSV5_REG_s;

/*-----------------------------------------------------------------------------------------------*\
 * B8 PHY  : All Registers
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    volatile B8_SERDES_TX_SETUP_REG_s           SerdesTxSetupReg;                /* 0x0(RW): */
    volatile B8_SERDES_TX_CONFIG_REG_s          SerdesTxConfigReg;               /* 0x4(RW): */
    volatile B8_SERDES_TX_STANDBY_REG_s         SerdesTxStandbyReg;              /* 0x8(RW): */
    volatile B8_SERDES_TX_RESET_REG_s           SerdesTxResetReg;                /* 0xC(RW): */
    volatile B8_SERDES_RX_SETUP_REG_s           SerdesRxSetupReg;                /* 0x10(RW): */
    volatile B8_SERDES_RX_CONFIG_REG_s          SerdesRxConfigReg;               /* 0x14(RW): */
    volatile B8_SERDES_RX_STANDBY_REG_s         SerdesRxStandbyReg;              /* 0x18(RW): */
    volatile B8_SERDES_RX_RESET_REG_s           SerdesRxResetReg;                /* 0x1C(RW): */
    volatile B8_RCT_PWR_TX_TDSETUP_REG_s        RctPwrTxTdsetupReg;              /* 0x20(RW): */
    volatile B8_RCT_PWR_TX_TPSETUP_REG_s        RctPwrTxTpsetupReg;              /* 0x24(RW): */
    volatile B8_RCT_PWR_TX_TCHK_REG_s           RctPwrTxTchkReg;                 /* 0x28(RW): */
    volatile UINT32                             Reserved0;                       /* 0x2C(R): */
    volatile B8_RCT_PWR_RX_TDSETUP_REG_s        RctPwrRxTdsetupReg;              /* 0x30(RW): */
    volatile B8_RCT_PWR_RX_TPSETUP_REG_s        RctPwrRxTpsetupReg;              /* 0x34(RW): */
    volatile UINT32                             Reserved1;                       /* 0x38(R): */
    volatile B8_RCT_MODECHG_CTRL_REG_s          RctModechgCtrlReg;               /* 0x3C(RW): */
    volatile UINT32                             Reserved2;                       /* 0x40(R): */
    volatile B8_MC_MPHY_RX_RST_REG_s            McMphyRxRstReg;                  /* 0x44(RW): */
    volatile UINT32                             Reserved3[2];                    /* 0x48-0x4C(R): */
    volatile B8_MC_MPHY_RX_TURN_AROUND_P1_REG_s McMphyRxTurnAroundP1Reg;         /* 0x50(RW): */
    volatile B8_MC_MPHY_RX_CDR_LOCK_REG_s       McMphyRxCdrLockReg;              /* 0x54(RW): */
    volatile B8_MC_MPHY_RX_TIMEOUT_REG_s        McMphyRxTimeoutReg;              /* 0x58(RW): */
    volatile B8_MC_MPHY_RX_TURN_AROUND_P2_REG_s McMphyRxTurnAroundP2Reg;         /* 0x5C(RW): */
    volatile B8_RX_LINK_HEARTBEAT_REG_s         RxLinkHeartbeatReg;              /* 0x60(RWC): */
    volatile B8_DSI_8LANE_MODE_REG_s            Dsi8LaneModeReg;                 /* 0x64(RW): */
    volatile UINT32                             Reserved4[38];                   /* 0x68-0xFC(R): */
    volatile B8_DSI0_CTRL_REG0_REG_s            Dsi0CtrlReg0Reg;                 /* 0x100(RW): */
    volatile B8_DSI0_CTRL_REG1_REG_s            Dsi0CtrlReg1Reg;                 /* 0x104(RW): */
    volatile B8_DSI0_CTRL_REG2_REG_s            Dsi0CtrlReg2Reg;                 /* 0x108(RW): */
    volatile B8_DSI0_CTRL_REG3_REG_s            Dsi0CtrlReg3Reg;                 /* 0x10C(RW): */
    volatile B8_DSI0_CTRL_REG4_REG_s            Dsi0CtrlReg4Reg;                 /* 0x110(RW): */
    volatile B8_DSI0_CTRL_REG5_REG_s            Dsi0CtrlReg5Reg;                 /* 0x114(RW): */
    volatile B8_DSI0_CTRL_AUX0_REG_s            Dsi0CtrlAux0Reg;                 /* 0x118(RW): */
    volatile B8_DSI0_OBSV_REG0_REG_s            Dsi0ObsvReg0Reg;                 /* 0x11C(R): */
    volatile B8_DSI0_OBSV_REG1_REG_s            Dsi0ObsvReg1Reg;                 /* 0x120(R): */
    volatile UINT32                             Reserved5[23];                   /* 0x124-0x17C(R): */
    volatile B8_DSI1_CTRL_REG0_REG_s            Dsi1CtrlReg0Reg;                 /* 0x180(RW): */
    volatile B8_DSI1_CTRL_REG1_REG_s            Dsi1CtrlReg1Reg;                 /* 0x184(RW): */
    volatile B8_DSI1_CTRL_REG2_REG_s            Dsi1CtrlReg2Reg;                 /* 0x188(RW): */
    volatile B8_DSI1_CTRL_REG3_REG_s            Dsi1CtrlReg3Reg;                 /* 0x18C(RW): */
    volatile B8_DSI1_CTRL_REG4_REG_s            Dsi1CtrlReg4Reg;                 /* 0x190(RW): */
    volatile B8_DSI1_CTRL_REG5_REG_s            Dsi1CtrlReg5Reg;                 /* 0x194(RW): */
    volatile B8_DSI1_CTRL_AUX0_REG_s            Dsi1CtrlAux0Reg;                 /* 0x198(RW): */
    volatile B8_DSI1_OBSV_REG0_REG_s            Dsi1ObsvReg0Reg;                 /* 0x19C(R): */
    volatile B8_DSI1_OBSV_REG1_REG_s            Dsi1ObsvReg1Reg;                 /* 0x1A0(R): */
    volatile UINT32                             Reserved6[23];                   /* 0x1A4-0x1FC(R): */
    volatile B8_RCT_MPHY_CTRL0_REG_s            RctMphyCtrl0Reg;                 /* 0x200(RW): */
    volatile B8_RCT_MPHY_CTRL1_REG_s            RctMphyCtrl1Reg;                 /* 0x204(RW): */
    volatile B8_RCT_MPHY_CTRL2_REG_s            RctMphyCtrl2Reg;                 /* 0x208(RW): */
    volatile B8_RCT_MPHY_CTRL3_REG_s            RctMphyCtrl3Reg;                 /* 0x20C(RW): */
    volatile UINT32                             MphyRctObsv0Reg;                 /* 0x210(R): */
    volatile B8_RCT_MPHY_CTRL5_REG_s            RctMphyCtrl5Reg;                 /* 0x214(RW): */
    volatile B8_RCT_MPHY_CTRL6_REG_s            RctMphyCtrl6Reg;                 /* 0x218(RW): */
    volatile B8_RCT_MPHY_CTRL7_REG_s            RctMphyCtrl7Reg;                 /* 0x21C(RW): */
    volatile UINT32                             MphyRctObsv2Reg;                 /* 0x220(R): */
    volatile B8_MPHY_RCT_OBSV3_REG_s            MphyRctObsv3Reg;                 /* 0x224(R): */
    volatile UINT32                             MphyRctObsv4Reg;                 /* 0x228(R): */
    volatile B8_MPHY_RCT_OBSV5_REG_s            MphyRctObsv5Reg;                 /* 0x22C(R): */
    volatile UINT32                             MphyRctObsv6Reg;                 /* 0x230(R): */
    volatile UINT32                             MphyRctObsv7Reg;                 /* 0x234(R): */
    volatile B8_RCT_MPHY_CTRL9_REG_s            RctMphyCtrl9Reg;                 /* 0x238(RW): */
    volatile B8_RCT_MPHY_CTRL10_REG_s           RctMphyCtrl10Reg;                /* 0x23C(RW): */
    volatile B8_RCT_MPHY_CTRL11_REG_s           RctMphyCtrl11Reg;                /* 0x240(RW): */
    volatile B8_RCT_MPHY_CTRL12_REG_s           RctMphyCtrl12Reg;                /* 0x244(RW): */
    volatile B8_RCT_MPHY_CTRL13_REG_s           RctMphyCtrl13Reg;                /* 0x248(RW): */
    volatile B8_RCT_MPHY_CTRL14_REG_s           RctMphyCtrl14Reg;                /* 0x24C(RW): */
    volatile B8_RCT_MPHY_CTRL15_REG_s           RctMphyCtrl15Reg;                /* 0x250(RW): */
    volatile B8_RCT_MPHY_CTRL16_REG_s           RctMphyCtrl16Reg;                /* 0x254(RW): */
    volatile B8_RCT_MPHY_CTRL17_REG_s           RctMphyCtrl17Reg;                /* 0x258(RW): */
    volatile B8_RCT_MPHY_CTRL18_REG_s           RctMphyCtrl18Reg;                /* 0x25C(RW): */
    volatile B8_RCT_MPHY_CTRL19_REG_s           RctMphyCtrl19Reg;                /* 0x260(RW): */
    volatile B8_RCT_MPHY_CTRL20_REG_s           RctMphyCtrl20Reg;                /* 0x264(RW): */
    volatile B8_RCT_MPHY_CTRL21_REG_s           RctMphyCtrl21Reg;                /* 0x268(RW): */
    volatile B8_RCT_MPHY_CTRL22_REG_s           RctMphyCtrl22Reg;                /* 0x26C(RW): */
    volatile B8_RCT_MPHY_CTRL23_REG_s           RctMphyCtrl23Reg;                /* 0x270(RW): */
    volatile B8_RCT_MPHY_CTRL24_REG_s           RctMphyCtrl24Reg;                /* 0x274(RW): */
    volatile B8_RCT_MPHY_CTRL25_REG_s           RctMphyCtrl25Reg;                /* 0x278(RW): */
    volatile B8_RCT_MPHY_CTRL26_REG_s           RctMphyCtrl26Reg;                /* 0x27C(RW): */
    volatile B8_RCT_MPHY_CTRL27_REG_s           RctMphyCtrl27Reg;                /* 0x280(RW): */
    volatile B8_RCT_MPHY_CTRL29_REG_s           RctMphyCtrl29Reg;                /* 0x284(RW): */
    volatile B8_RCT_MPHY_CTRL30_REG_s           RctMphyCtrl30Reg;                /* 0x288(RW): */
    volatile B8_RCT_MPHY_CTRL31_REG_s           RctMphyCtrl31Reg;                /* 0x28C(RW): */
    volatile B8_RCT_MPHY_CTRL32_REG_s           RctMphyCtrl32Reg;                /* 0x290(RW): */
    volatile B8_MPHY_RCT_OBSV10_REG_s           MphyRctObsv10Reg;                /* 0x294(R): */
    volatile UINT32                             Reserved7[90];                   /* 0x298-0x3FC(R): */
    volatile B8_RCT_DPHY_CTRL0_REG_s            RctDphyCtrl0Reg;                 /* 0x400(RW): */
    volatile B8_RCT_DPHY_CTRL1_REG_s            RctDphyCtrl1Reg;                 /* 0x404(RW): */
    volatile B8_RCT_DPHY_CTRL2_REG_s            RctDphyCtrl2Reg;                 /* 0x408(RW): */
    volatile B8_RCT_DPHY_CTRL3_REG_s            RctDphyCtrl3Reg;                 /* 0x40C(RW): */
    volatile B8_RCT_DPHY_CTRL4_REG_s            RctDphyCtrl4Reg;                 /* 0x410(RW): */
    volatile B8_RCT_DPHY_CTRL5_REG_s            RctDphyCtrl5Reg;                 /* 0x414(RW): */
    volatile B8_RCT_DPHY_CTRL6_REG_s            RctDphyCtrl6Reg;                 /* 0x418(RW): */
    volatile B8_RCT_DPHY_CTRL7_REG_s            RctDphyCtrl7Reg;                 /* 0x41C(RW): */
    volatile B8_RCT_DPHY_CTRL8_REG_s            RctDphyCtrl8Reg;                 /* 0x420(RW): */
    volatile B8_RCT_DPHY_CTRL9_REG_s            RctDphyCtrl9Reg;                 /* 0x424(RW): */
    volatile UINT32                             RctDphyCtrl10Reg;                /* 0x428(RW): */
    volatile UINT32                             RctDphyCtrl11Reg;                /* 0x42C(RW): */
    volatile B8_DPHY_RCT_OBSV0_REG_s            DphyRctObsv0Reg;                 /* 0x430(R): */
    volatile B8_DPHY_RCT_OBSV1_REG_s            DphyRctObsv1Reg;                 /* 0x434(R): */
    volatile B8_DPHY_RCT_OBSV2_REG_s            DphyRctObsv2Reg;                 /* 0x438(R): */
    volatile B8_DPHY_RCT_OBSV3_REG_s            DphyRctObsv3Reg;                 /* 0x43C(R): */
    volatile UINT32                             DphyRctObsv4Reg;                 /* 0x440(R): */
    volatile B8_DPHY_RCT_OBSV5_REG_s            DphyRctObsv5Reg;                 /* 0x444(R): */
    volatile UINT32                             DphyRctObsv6Reg;                 /* 0x448(R): */
    volatile UINT32                             DphyRctObsv7Reg;                 /* 0x44C(R): */
} B8_PHY_REG_s;

#endif /* AMBA_B8_REG_PHY_H */

