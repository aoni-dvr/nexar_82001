/**
 *  @file AmbaB8Reg_Scratchpad.h
 *
 *  @copyright Copyright (c) 2015 Ambarella, Inc.
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
 *  @details Definitions & Constants for B8 Scratchpad Control Registers
 *
 */

#ifndef AMBA_B8_REG_SCRATCHPAD_H
#define AMBA_B8_REG_SCRATCHPAD_H

/*-----------------------------------------------------------------------------------*\
 * B6 Scratchpad : High Speed Bypass Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  Reserved0:                  2;  /* [1:0] */
    UINT32  EccDecErasureEnable_5_4:    2;  /* [3:2] */
    UINT32  Reserved1:                  2;  /* [5:4] */
    UINT32  HsVoutEnable_3_2:           2;  /* [7:6] */
    UINT32  Reserved2:                  2;  /* [9:8] */
    UINT32  LsTxNoGap:                  1;  /* [10] Make Ls no gap between packet to improve performance */
    UINT32  Reserved3:                  5;  /* [15:11] */
    UINT32  LsTxStartLength:            2;  /* [17:16] Cycles count of low-speed TX start */
    UINT32  LegacyEngineSel_3_2:        2;  /* [19:18] Set to 1 to select legacy decompressor for depacker stream */
    UINT32  Reserved4:                  1;  /* [20] */
    UINT32  LsRx0Gain:                  1;  /* [21] Adaptive gain of low-speed RX0 */
    UINT32  LegacyEngineSel_1_0:        2;  /* [23:22] Set to 1 to select legacy decompressor for depacker stream */
    UINT32  HsBypassConv:               2;  /* [25:24] Set to 1 when the source is also fed into cfa_codec */
    UINT32  Reserved5:                  2;  /* [27:26] */
    UINT32  HsBypassEn:                 2;  /* [29:28] Set to 1 to enable bypass path to B8D */
    UINT32  HsVoutEn_1_0:               2;  /* [31:30] Set to 1 to enable VOUT path for Depacker stream */
} B8_SCRATCHPAD_HS_BYPASS_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B6 Scratchpad : ECC Config Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  EccDecEn:                   6;     /* [5:0] ecc_dec_en[i]=1: Enable i-th ECC decoder */
    UINT32  EccDec0DropFail:            1;     /* [6] Set to force ECC decoder instance#0 to drop packets with decode-error */
    UINT32  Reserved:                   1;     /* [7] Reserved */
    UINT32  EccDec0Select:              2;     /* [9:8] Select t parameter for ECC decoder instance #0 */
    UINT32  Reserved1:                  6;     /* [15:10] */
    UINT32  EccEncEn:                   8;     /* [23:16] ecc_enc_en[i+16]=1: enable i-th ECC encoder */
    UINT32  EccEnc0Sel:                 2;     /* [25:24] Select t parameter for ECC encoder instance #0 */
    UINT32  Reserved2:                  2;     /* [27:26] Reserved */
    UINT32  EccDecErasureEnable_3_0:    4;     /* [31:28] */
} B8_SCRATCHPAD_ECC_CFG_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B6 Scratchpad : VINF Config Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  VinfFrameHeight:        14;    /* [13:0] The frame width should set as the size of frame_height */
    UINT32  Reserved0:              2;     /* [15:14] */
    UINT32  VinfFrameWidth:         14;    /* [29:16] The frame width should set as the size of compressed frame_width */
    UINT32  Reserved1:              1;     /* [30] */
    UINT32  VinfEnable:             1;     /* [31] Enable Vinf */
} B8_SCRATCHPAD_VINF_CFG_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B6 Scratchpad : VINF Cycle Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  VinfMaxCycle:           16;    /* [15:0] Refer to an Ambarella representative */
    UINT32  Reserved0:              16;    /* [31:16] Enable Vinf */
} B8_SCRATCHPAD_VINF_MAX_CYCLE_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B6 Scratchpad : Sync Config Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  VsyncPolarity:          1;     /* [0] 0 : Active low sync, 1 : Active high sync */
    UINT32  HsyncPolarity:          1;     /* [1] 0 : Active low sync, 1 : Active high sync */
    UINT32  Reserved0:              2;     /* [3:2] Refer to an Ambarella representative */
    UINT32  VsyncMasterVin1Sel:     1;     /* [4] 0 : From VIN0 Hsync, 1 : From VIN1 Hsync */
    UINT32  HsyncMasterVin1Sel:     1;     /* [5] 0 : From VIN0 Hsync, 1 : From VIN1 Hsync */
    UINT32  Vsync0Sel:              2;     /* [7:6] Vsync 0 output select */
    UINT32  Vsync1Sel:              2;     /* [9:8] Vsync 1 output select */
    UINT32  Hsync0Sel:              1;     /* [10] Hsync 0 output select */
    UINT32  Hsync1Sel:              1;     /* [11] Hsync 1 output select */
    UINT32  VsyncExtSel:            1;     /* [12] Delayed Vsync source select */
    UINT32  HsyncExtSel:            1;     /* [13] Delayed Hsync source select */
    UINT32  FsyncPwmdCh1Sel:        1;     /* [14] fsync_pwmd source select */
    UINT32  Vsync0FsyncSel:         1;     /* [15] Vsync 0 output select */
    UINT32  Vsync1FsyncSel:         1;     /* [16] Vsync 0 output select */
    UINT32  VsyncMasterFsyncSel:    1;     /* [17] Delayed vsync reference vsync source select */
    UINT32  FsyncSrcGpiSel:         2;     /* [19:18] Fsync_src_gpi select */
    UINT32  Reserved1:              12;    /* [31:20] */
} B8_SCRATCHPAD_SYNC_CFG_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B6 Scratchpad : Sync Delay Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  DlyMult0:               3;     /* [2:0] Delay multiplier for delayed Vsync 0 */
    UINT32  Dly0:                   10;    /* [12:3] Delay counter for delayed Vsync 0 */
    UINT32  Reserved0:              3;     /* [15:13] */
    UINT32  DlyMult1:               3;     /* [18:16] Delay multiplier for delayed Vsync 1 */
    UINT32  Dly1:                   10;    /* [28:19] Delay counter for delayed Vsync 1 */
    UINT32  Reserved1:              3;     /* [31:29] */
} B8_SCRATCHPAD_SYNC_DELAY_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B6 Scratchpad : PHY BIST Debug Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  AhbPhyBistCfgLaneEn:    10;    /* [9:0] One bit for each lane; Set it to 1 to enable lane testing */
    UINT32  AhbPhyBistCfgTestPat:   2;     /* [11:10] Four test patterns for each lane */
    UINT32  AhbPhyBistCfgMipi:      1;     /* [12] 0: SLVS, 1: MIPI */
    UINT32  AhbPhyBistRst:          1;     /* [13] External reset logic to phy_bist */
    UINT32  AhbPhyBistRefClkSel:    1;     /* [14] 0: Ref_clk, 1: Sensor clock */
    UINT32  Reserved0:              17;    /* [31:15] */
} B8_SCRATCHPAD_PHY_BIST_DBG0_REG_s;

typedef struct {
    UINT32  PhyBistAhbTestDone:     8;      /* [7:0] One bit for each lane; Asserted when test completes */
    UINT32  Reserved0:              8;      /* [15:8] */
    UINT32  PhyBistAhbFail:         8;      /* [23:16] 0: Pass, 1: Fail */
    UINT32  Reserved1:              8;      /* [31:24] */
} B8_SCRATCHPAD_PHY_BIST_DBG1_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B6 Scratchpad : Debug Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  DbgRegEotFail:          8;    /* [7:0] Internal use only */
    UINT32  Reserved0:              8;    /* [15:8] */
    UINT32  DbgRegEotPass:          8;    /* [23:16] Internal use only */
    UINT32  Reserved1:              8;    /* [31:24] */
} B8_SCRATCHPAD_DBG0_REG_s;

typedef struct {
    UINT32  DbgRegSotFail:          8;    /* [7:0] OInternal use only */
    UINT32  Reserved0:              8;    /* [15:8] */
    UINT32  DbgRegSotPass:          8;    /* [23:16] Internal use only */
    UINT32  Reserved1:              8;    /* [31:24] */
} B8_SCRATCHPAD_DBG1_REG_s;

typedef struct {
    UINT32  DbgRegTestPatFail:      8;    /* [7:0] OInternal use only */
    UINT32  Reserved0:              8;    /* [15:8] */
    UINT32  DbgRegTestPatPass:      8;    /* [23:16] Internal use only */
    UINT32  Reserved1:              8;    /* [31:24] */
} B8_SCRATCHPAD_DBG2_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B6 Scratchpad : VINF Debug Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  Vout0DisplayRstStatus:  1;      /* [0] For b6a_vout_display to monitor Reset status */
    UINT32  Display0MipiCmdStatus:  1;      /* [1] For b6a_vout_display to monitor MIPI command status */
    UINT32  Vout1DisplayRstStatus:  1;      /* [2] For b6a_vout_display to monitor Reset status */
    UINT32  Display1MipiCmdStatus:  1;      /* [3] For b6a_vout_display to monitor MIPI command status */
    UINT32  Reserved0:              12;     /* [15:4] */
    UINT32  Vinf0DebugState:        4;      /* [19:16] Debug signal to monitor FSM in VINF */
    UINT32  Reserved1:              12;     /* [31:20] */
} B8_SCRATCHPAD_VINF_DBG_REG_s;

typedef struct {
    UINT32  GpioLsTxEn:             4;      /* [3:0] Enable gpio_ls_tx[3:0] */
    UINT32  Reserved0:              4;      /* [7:4] */
    UINT32  GpioLsRxEn:             2;      /* [9:8] Enable gpio_ls_rx[1:0] */
    UINT32  Reserved1:              22;     /* [31:10] */
} B8_SCRATCHPAD_GPIO_LS_REG_s;

typedef struct {
    UINT32  Vin0BypassEn:           1;      /* [0] Enable of direct path from VIN0 to VOUT/Packer */
    UINT32  Vin1BypassEn:           1;      /* [1] Enable of direct path from VIN1 to VOUT/Packer */
    UINT32  Vin0BypassDest:         1;      /* [2] 0: to VOUT, 1: to packer */
    UINT32  Vin1BypassDest:         1;      /* [3] 0: to VOUT, 1: to packer */
    UINT32  Vout0SrcEn:             1;      /* [4] */
    UINT32  Vout1SrcEn:             1;      /* [5] */
    UINT32  Reserved0:              2;      /* [7:6] */
    UINT32  Vout0SrcSel:            1;      /* [8] 0: From merger, 1: From VIN */
    UINT32  Vout1SrcSel:            1;      /* [9] 0: From merger, 1: From VIN */
    UINT32  Reserved1:              2;      /* [11:10] */
    UINT32  VinRgbPackEn:           2;      /* [13:12] Enable data packing if VIN input sources is 8b RGB */
    UINT32  Reserved2:              18;     /* [31:14] */
} B8_SCRATCHPAD_VOUT_SRC_REG_s;

/*-----------------------------------------------------------------------------------------------*\
 * B6 SCRATCHPAD : All Registers
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    volatile B8_SCRATCHPAD_HS_BYPASS_REG_s      HighSpeedBypass;    /* 0x00(RW): */
    volatile B8_SCRATCHPAD_ECC_CFG_REG_s        EccConfig;          /* 0x04(RW): */
    volatile B8_SCRATCHPAD_VINF_CFG_REG_s       Vinf0Config;        /* 0x08(RW): */
    volatile B8_SCRATCHPAD_VINF_MAX_CYCLE_REG_s Vinf0MaxCycle;      /* 0x0C(RW): */
    volatile UINT32                             Reserved0[2];       /* 0x10-0x14: Reserved */
    volatile B8_SCRATCHPAD_SYNC_CFG_REG_s       SyncConfig;         /* 0x18(RW): */
    volatile B8_SCRATCHPAD_SYNC_DELAY_REG_s     SyncDelay;          /* 0x1C(RW): */
    volatile UINT32                             Bist0;              /* 0x20(RW): */
    volatile UINT32                             Bist1;              /* 0x24(RW): */
    volatile B8_SCRATCHPAD_PHY_BIST_DBG0_REG_s  PhyBistDebug0;      /* 0x28(RW): */
    volatile B8_SCRATCHPAD_PHY_BIST_DBG1_REG_s  PhyBistDebug1;      /* 0x2C(RO): */
    volatile B8_SCRATCHPAD_DBG0_REG_s           Debug0;             /* 0x30(RO): */
    volatile B8_SCRATCHPAD_DBG1_REG_s           Debug1;             /* 0x34(RO): */
    volatile B8_SCRATCHPAD_DBG2_REG_s           Debug2;             /* 0x38(RO): */
    volatile UINT32                             Reserved1;          /* 0x3C: Reserved */
    volatile UINT32                             SsiahbmLaunchEdge;  /* 0x40: ssiahbm spi read back data polarity */
    volatile UINT32                             Reserved2;          /* 0x44: Reserved */
    volatile UINT32                             LsRxDebug;          /* 0x48(RO): */
    volatile UINT32                             Reserved3;          /* 0x4C: */
    volatile B8_SCRATCHPAD_VINF_DBG_REG_s       VinfDebug;          /* 0x50(RWC): */
    volatile UINT32                             EccDec0Debug0;      /* 0x54(RO): */
    volatile UINT32                             EccDec0Debug1;      /* 0x58(RO): */
    volatile UINT32                             EccDec0Debug2;      /* 0x5C(RO): */
    volatile UINT32                             EccDec0Debug3;      /* 0x60(RO): */
    volatile UINT32                             Reserved4[4];       /* 0x64-0x70: */
    volatile UINT32                             ChipID;             /* 0x74: */
    volatile UINT32                             EfuseChipID_L;      /* 0x78: */
    volatile UINT32                             EfuseChipID_H;      /* 0x7C: */
    volatile UINT32                             Dummy0;             /* 0x80(RW): */
    volatile UINT32                             Dummy1;             /* 0x84(RW): */
    volatile UINT32                             Dummy2;             /* 0x88(RW): */
    volatile UINT32                             Dummy3;             /* 0x8C(RW): */
    volatile UINT32                             Reserved5[12];      /* 0x90-0xBC: */
    volatile B8_SCRATCHPAD_GPIO_LS_REG_s        GpioLs;             /* 0xC0(RW): */
    volatile B8_SCRATCHPAD_VOUT_SRC_REG_s       VoutSrc;            /* 0xC4(RW): */
} B8_SCRATCHPAD_REG_s;
#endif /* AMBA_B8_REG_SCRATCHPAD_H */
