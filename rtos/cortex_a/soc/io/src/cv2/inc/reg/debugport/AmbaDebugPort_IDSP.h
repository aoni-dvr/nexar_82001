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

#define AMBA_DBG_PORT_IDSP_VIN          (AMBA_DBG_PORT_IDSP_SECTION1)
#define AMBA_DBG_PORT_IDSP_PIP_VIN      (AMBA_DBG_PORT_IDSP_SECTION10)
#define AMBA_DBG_PORT_IDSP_PIP2_VIN     (AMBA_DBG_PORT_IDSP_SECTION11)
#define AMBA_DBG_PORT_IDSP_PIP3_VIN     (AMBA_DBG_PORT_IDSP_SECTION12)
#define AMBA_DBG_PORT_IDSP_PIP4_VIN     (AMBA_DBG_PORT_IDSP_SECTION13)
#define AMBA_DBG_PORT_IDSP_PIP5_VIN     (AMBA_DBG_PORT_IDSP_SECTION14)

/*
 * IDSP Debug Port: Config Controller Internal Debug State
 */
typedef struct {
    UINT32 WriteDataValid:          1;      /* [0]: Write Data Valid */
    UINT32 ConfigUpperHalf:         1;      /* [1]: Config Upper Half of Word */
    UINT32 ConfigDataWords:         6;      /* [7:2]: Config Data Words Remaining in the Buffer */
    UINT32 ConfigStreamID:          6;      /* [13:8]: Configuration stream ID */
    UINT32 Reserved:                18;     /* [31:14] */
} AMBA_DBG_PORT_IDSP_CFG_DATA_STATUS_REG_s;

/*
 * IDSP Debug Port: Section Reset Register
 */
typedef struct {
    UINT32 ResetVinSyncOut:         1;      /* [0]: Reset VIN Master SYNC Generation Logic */
    UINT32 CorrespondSection:       15;     /* [15:1]: Corresponding to Section (15 - 1) */
    UINT32 Reserved:                8;      /* [23:16] */
    UINT32 ResetMipiPhy:            6;      /* [29:24]: Reset VIN MIPI PHY (Level) */
    UINT32 Reserved1:               1;      /* [30] */
    UINT32 ResetPipSyncOut:         1;      /* [31]: Reset PIP Master SYNC Generation Logic */
} AMBA_DBG_PORT_IDSP_SEC_RESET_REG_s;

/*
 * IDSP Debug Port: Internal Error Status
 */
typedef struct {
    UINT32 Reserved:                1;      /* [0] */
    UINT32 CommandOverwrite:        15;     /* [15:1]: Section (15 - 1) Command Overwrite */
    UINT32 Reserved1:               16;     /* [31:16] */
} AMBA_DBG_PORT_IDSP_ERR_STATUS_REG_s;

/*
 * IDSP Debug Port All Registers
 */
typedef struct {
    volatile UINT32                                   AddressExtension;       /* 0x8000: Address Extension Register */
    volatile AMBA_DBG_PORT_IDSP_CFG_DATA_STATUS_REG_s ConfigDataStatus;       /* 0x8004: Config Controller Internal Debug State */
    volatile UINT32                                   Reserved[5];            /* 0x8008 - 0x8018 */
    volatile UINT32                                   SectionReset;           /* 0x801C: Section Reset Register */
    volatile AMBA_DBG_PORT_IDSP_ERR_STATUS_REG_s      ErrorStatus;            /* 0x8020: Internal Error Status */
} AMBA_DBG_PORT_IDSP_CONTROLLER_REG_s;

typedef struct {
    UINT32  SwReset:                1;      /* [0] Reset core VIN logic when config is loaded */
    UINT32  Enable:                 1;      /* [1] Enable Sensor Interface logic */
    UINT32  OutputEnable:           1;      /* [2] Enable output of VIN */
    UINT32  BitsPerPixel:           3;      /* [5:3] Sensor data width; 0=8-bit; 1=10-bit; 2=12-bit; 3=14-bit; 4=16-bit */
    UINT32  Reserved:               26;     /* [31:6] Reserved */
} AMBA_DBG_PORT_IDSP_CTRL0_REG_s;

typedef struct {
    UINT32  DataLaneEnable:         12;     /* [11:0] 1 = Enable SLVS/MIPI Physical Data Lane */
    UINT32  Reserved:               20;     /* [31:12] Reserved */
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
    UINT32  DisableAfterSync:       1;      /* [14] */
    UINT32  DoubleBufferEnable:     1;      /* [15] */
    UINT32  Reserved:               16;     /* [31:16] Reserved */
} AMBA_DBG_PORT_IDSP_DVP_CTRL_REG_s;

typedef struct {
    UINT32  FieldPinSelect:         5;      /* [4:0] for VIN 0:clk_n[0]; 1:clk_n[1]; 2:clk_p[1]; for PIP 0:clk_n[3]; 1:clk_n[4]; 2:clk_p[4] */
    UINT32  VsyncPinSelect:         5;      /* [9:5] for VIN 0:clk_n[0]; 1:clk_n[1]; 2:clk_p[1]; for PIP 0:clk_n[3]; 1:clk_n[4]; 2:clk_p[4] */
    UINT32  HsyncPinSelect:         5;      /* [14:10] for VIN 0:clk_n[0]; 1:clk_n[1]; 2:clk_p[1]; for PIP 0:clk_n[3]; 1:clk_n[4]; 2:clk_p[4] */
    UINT32  Reserved:               17;     /* [31:15] */
} AMBA_DBG_PORT_IDSP_SYNC_PIN_MUX_REG_s;

typedef struct {
    UINT32  DataLane0_4_8PinSelect:  4;      /* [3:0] pad_p[x] and pad_n[x] */
    UINT32  DataLane1_5_9PinSelect:  4;      /* [7:4] pad_p[x] and pad_n[x] */
    UINT32  DataLane2_6_10PinSelect: 4;      /* [11:8] pad_p[x] and pad_n[x] */
    UINT32  DataLane3_7_11PinSelect: 4;      /* [15:12] pad_p[x] and pad_n[x] */
    UINT32  Reserved:                16;     /* [31:16] Reserved */
} AMBA_DBG_PORT_IDSP_DATA_PIN_MUX_REG_s;

typedef struct {
    UINT32  DisableCropRegion:      1;      /* [0] Disable crop region; if it is 1, ignore the crop_start/end_col/row */
    UINT32  DbgNonBlocking:         1;      /* [1] Only set to 1 when bring up VIN to check sof/eof interrupts */
    UINT32  DbgEolHold:             1;      /* [2] reserved bit, only set to 1 when requested by designer */
    UINT32  MipiMaxSkewCycle:       3;      /* [5:3] Only set it to non-zero value in 8-lane MIPI sensor; its unit is mipi_byteclk cycle; or 4*mipi_sensor_clk cycle */
    UINT32  Reserved:               26;     /* [31:6] */
} AMBA_DBG_PORT_IDSP_CTRL2_REG_s;

typedef struct {
    UINT32  SplitWidth:             14;     /* [13:0]: Split each input line to lines of SPLIT_WIDTH pixels(including intermediate HBLANK) */
    UINT32  NumSplits:              2;      /* [15:14]: number of splits to make */
    UINT32  Reserved:               16;     /* [31:16] Reserved */
} AMBA_DBG_PORT_IDSP_SPLIT_CTRL_REG_s;

typedef struct {
    UINT32  SyncInterleaving:       2;     /* [1:0] 0=none; 1=2-lane interleaving; 2=4-lane interleaving (For HiSPi Streaming-S) */
    UINT32  SyncTolerance:          2;     /* [3:2] 0=No error tolerance; 1=Allow a corrupted sync every other code; 2=Always allow corruption */
    UINT32  SyncAllLanes:           1;     /* [4] 1=Sync code must be detected on all active lanes in each group; 0=Majority of active lanes */
    UINT32  Reserved:               27;    /* [31:5] */
} AMBA_DBG_PORT_IDSP_SLVS_SYNC_LOCATION_REG_s;

typedef struct {
    UINT32  AllowPartialCode:       1;      /* [0] 1=Allow one of the all-0/all-1 words to be corrupted and still be a valid sync */
    UINT32  LockSyncPhase:          1;      /* [1] 0=Constantly re-align phase; 1=Lock/Coast sync phase to first detected code */
    UINT32  SyncCorrection:         1;      /* [2] Enable correction of sync code curruption by examining the codes across muliple lanes */
    UINT32  DeskewEnable:           1;      /* [3] 0=disable/bypass deskew; 1=enable */
    UINT32  SyncType:               1;      /* [4] 0=ITU-656; 1=Custom */
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
    UINT32  VirtChanNumMinus1:      2;      /* [1:0] Number of MIPI virtual channel number minus 1 */
    UINT32  VirtChanMask:           2;      /* [3:2] MIPI virtual channel mask (Data captured when (~VC_MASK & virtual_channel) == VC_PAT */
    UINT32  FSVirtChanPattern:      2;      /* [5:4] MIPI virtual channel pattern to match for FS of the first exposure */
    UINT32  FEVirtChanPattern:      2;      /* [7:6] MIPI virtual channel pattern to match for FE of the last exposure */
    UINT32  Exp0VirtChanPattern:    2;      /* [9:8] MIPI virtual channel pattern to match for valid data of the first exposure */
    UINT32  Exp1VirtChanPattern:    2;      /* [11:10] MIPI virtual channel pattern to match for valid data of the second exposure */
    UINT32  Exp2VirtChanPattern:    2;      /* [13:12] MIPI virtual channel pattern to match for valid data of the third exposure */
    UINT32  Exp3VirtChanPattern:    2;      /* [15:14] MIPI virtual channel pattern to match for valid data of the fourth exposure */
    UINT32  Reserved:               16;     /* [31:16] Reserved */
} AMBA_DBG_PORT_IDSP_MIPI_CTRL0_REG_s;

typedef struct {
    UINT32  DataTypeMask:           6;      /* [5:0] MIPI data type mask (Data captured when (~DT_MASK & data_id) == DT_PAT */
    UINT32  DataTypePattern:        6;      /* [11:6] MIPI data type pattern to match for valid data */
    UINT32  ByteSwapEnable:         1;      /* [12] 1 = Reverse order of pixels */
    UINT32  EccEnable:              1;      /* [13] 1 = Enable packet header ECC */
    UINT32  ForwardEccEnable:       1;      /* [14] 1 = Enable errored packets forwarding (default mode) */
    UINT32  MipiCropEnable:         1;      /* [15] 1 = Enable MIPI crop (used in multiple VC mode) */
    UINT32  Reserved:               16;     /* [31:16] */
} AMBA_DBG_PORT_IDSP_MIPI_CTRL1_REG_s;

typedef struct {
    UINT32  Mode:                   2;      /* [1:0] 0=Disabled; 1=Even Fields; 2=Odd Fields; 3=All Fields */
    UINT32  Line:                   14;     /* [15:2] VOUT sync0 is asserted for the duration of the specified active line */
    UINT32  Reserved:               16;     /* [31:16] Reserved */
} AMBA_DBG_PORT_IDSP_VOUT_SYNC_REG_s;

typedef struct {
    UINT32  Enable:                 1;      /* [0] Enable GPIO strig0 */
    UINT32  Polarity:               1;      /* [1] 0=Active low; 1=Active high */
    UINT32  StartLine:              14;     /* [15:2] Active line number to start asserting strig */
    UINT32  Reserved:               16;     /* [31:16] Reserved */
} AMBA_DBG_PORT_IDSP_STRIG_CTRL_REG_s;

typedef struct {
    UINT32  Sign:                   1;      /* [0] sign bit of black level offset */
    UINT32  Reserved:               31;     /* [31:1] Reserved */
} AMBA_DBG_PORT_IDSP_BLACKLEVEL_CTRL_REG_s;

typedef struct {
    UINT32  GotActSof:              1;      /* [0] Detected start of active region (write 1 to clear) */
    UINT32  Strig0Status:           1;      /* [1] NOT used any more */
    UINT32  Strig1Status:           1;      /* [2] NOT used any more */
    UINT32  SfifoOverFlow:          1;      /* [3] SFIFO has overflowed (must reset VIN) */
    UINT32  ShortLine:              1;      /* [4] EOL received before reaching end of programmed crop width (write 1 to clear) */
    UINT32  ShortFrame:             1;      /* [5] EOF received before reaching end of programmed crop region (write 1 to clear) */
    UINT32  Field:                  3;      /* [8:6] Field bit value */
    UINT32  SyncTimeout:            1;      /* [9] Sync timeout specified in SYNC_TIMEOUT has triggered (write 1 to clear) */
    UINT32  DbgReserved0:           1;      /* [10] */
    UINT32  GotVsync:               1;      /* [11] Reached end of active region (start of VBLANK) (write 1 to clear) */
    UINT32  SentMasterVsync:        1;      /* [12] Sent master vsync (from master sync generator unit) (write 1 to clear) */
    UINT32  GotWinEof:              1;      /* [13] Reached end of crop region (write 1 to clear) */
    UINT32  Uncorrectable656Error:  1;      /* [14] ITU-656 sync code was corrupted and uncorrectable (write 1 to clear) */
    UINT32  Reserved:               17;     /* [31:15] Reserved */
} AMBA_DBG_PORT_IDSP_STATUS_REG_s;

typedef struct {
    UINT32  SyncLocked:             1;      /* [0] VIN front-end sync is locked */
    UINT32  LostLockAfterSof:       1;      /* [1] Lost lock after detecting start of frame (must reset VIN) */
    UINT32  PartialSyncDetected:    1;      /* [2] Partial/corrupted sync was detected (write 1 to clear) */
    UINT32  UnknownSyncCode:        1;      /* [3] Sync code did not match any patterns (write 1 to clear) */
    UINT32  GotWinSof:              1;      /* [4] Reached start of crop region (write 1 to clear) */
    UINT32  AfifoOverflow:          1;      /* [5] AFIFO overflow (must reset VIN) */
    UINT32  SyncState:              2;      /* [7:6] State of front-end sync FSM (0=IDLE; 1=ACTIVE; 2=HBLANK; 3=VBLANK) */
    UINT32  Reserved:               24;     /* [31:8] */
} AMBA_DBG_PORT_IDSP_SLVS_STATUS_REG_s;

typedef struct {
    UINT32  EccError2Bit:           1;      /* [0] 2-bit uncorrectable ECC error */
    UINT32  EccError1Bit:           1;      /* [1] 1-bit ECC error corrected */
    UINT32  Reserved:               1;      /* [2] */
    UINT32  ErrId:                  1;      /* [3] Unknown packet data ID received */
    UINT32  FrameSyncError:         1;      /* [4] Frame sync error (Received unpaired SOF or EOF) */
    UINT32  ProtState:              2;      /* [6:5] State of FSM in MIPI protocol module */
    UINT32  SofPacketReceived:      1;      /* [7] SOF Short Packet Received */
    UINT32  EofPacketReceived:      1;      /* [8] EOF Short Packet Received */
    UINT32  Reserved1:              3;      /* [11:9] */
    UINT32  CrcError:               1;      /* [12] Long Packet CRC Error */
    UINT32  Reserved2:              19;     /* [31:13] */
} AMBA_DBG_PORT_IDSP_MIPI_STATUS0_REG_s;

typedef struct {
    UINT32  FrameID:                1;      /* [0] Frame ID of the data, only meaningful when gcfg.mipi_hdr_frame_ovlp is 1 */
    UINT32  VirtChanID:             2;      /* [2:1] Virtual channel ID */
    UINT32  Frame0VirtChanDone:     3;      /* [5:3] Frame done signal of each VC when frame ID is 0 */
    UINT32  Frame1VirtChanDone:     3;      /* [8:6] Frame done signal of each VC when frame ID is 1 */
    UINT32  MipiCropShortLine:      1;      /* [9] Short line of the crop module inside MIPI */
    UINT32  MipiCropShortFrame:     1;      /* [10] Short frame of the crop module inside MIPI */
    UINT32  Reserved:               21;     /* [31:11] */
} AMBA_DBG_PORT_IDSP_MIPI_STATUS1_REG_s;

typedef struct {
    UINT32  DebugCommandShift:      1;      /* [0] To manually shift VIN programming via debug bus */
    UINT32  FrameID:                1;      /* [1] Frame ID sent vid debug bus when there is no config_done */
    UINT32  Reserved:               30;     /* [31:2] */
} AMBA_DBG_PORT_IDSP_CONFIG_DONE_REG_s;

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
    UINT32  MipiVCHdr:              2;      /* [6:5] 0: no virtual channel; 1: with virtual channel but no overlap (frame id is not alternate);
                                                     2/3: with virtual channel; and frame id alternates, it supports both overlap and nonoverlap */
    UINT32  Reserved:               25;     /* [31:7] */
} AMBA_DBG_PORT_IDSP_GLOBAL_REG_s;

typedef struct {
    volatile AMBA_DBG_PORT_IDSP_CTRL0_REG_s                 Ctrl0;                  /* 0x0000: Sensor interface module control0 */
    volatile AMBA_DBG_PORT_IDSP_CTRL1_REG_s                 Ctrl1;                  /* 0x0004: Sensor interface module control1 */
    volatile AMBA_DBG_PORT_IDSP_DVP_CTRL_REG_s              DvpCtrl;                /* 0x0008: Parallel control register */
    volatile AMBA_DBG_PORT_IDSP_SYNC_PIN_MUX_REG_s          SyncPinMux;             /* 0x000c: VIN sync pin MUX Configuration Register */
    volatile AMBA_DBG_PORT_IDSP_DATA_PIN_MUX_REG_s          DataPinMux;             /* 0x0010: VIN data pin MUX Configuration Register */
    volatile AMBA_DBG_PORT_IDSP_DATA_PIN_MUX_REG_s          DataPinMux1;            /* 0x0014: VIN data pin MUX Configuration Register */
    volatile AMBA_DBG_PORT_IDSP_DATA_PIN_MUX_REG_s          DataPinMux2;            /* 0x0018: VIN data pin MUX Configuration Register */
    volatile AMBA_DBG_PORT_IDSP_CTRL2_REG_s                 Ctrl2;                  /* 0x001C: Sensor interface module control2 */
    volatile UINT32                                         ActiveRegionWidth;      /* 0x0020: active width - 1, 0=auto detected */
    volatile UINT32                                         ActiveRegionHeight;     /* 0x0024: active height - 1, 0=auto-detect */
    volatile AMBA_DBG_PORT_IDSP_SPLIT_CTRL_REG_s            SplitCtrl;              /* 0x0028: SPLIT_WIDTH pixels (0=disabled) */
    volatile UINT32                                         CropStartCol;           /* 0x002C: Start column index of crop region */
    volatile UINT32                                         CropStartRow;           /* 0x0030: Start row index of crop region */
    volatile UINT32                                         CropEndCol;             /* 0x0034: End column index of crop region */
    volatile UINT32                                         CropEndRow;             /* 0x0038: End row index of crop region */
    volatile AMBA_DBG_PORT_IDSP_SLVS_SYNC_LOCATION_REG_s    SlvsSyncCodeLocation;   /* 0x003C: SLVS Sync code location */
    volatile AMBA_DBG_PORT_IDSP_SLVS_CTRL_REG_s             SlvsCtrl;               /* 0x0040: SLVS control */
    volatile AMBA_DBG_PORT_IDSP_SLVS_SYNC_DETECT_REG_s      SlvsSyncCodeDetect;     /* 0x0044: SLVS sync code detection enable */
    volatile UINT32                                         SyncTimeOutL;           /* 0x0048: Timeout value for sync detection (Bit[15:0]) , 0=disabled */
    volatile UINT32                                         SyncTimeOutH;           /* 0x004C: Timeout value for sync detection (Bit[31:16]), 0=disabled */
    volatile UINT32                                         SlvsSyncDetectMask;     /* 0x0050: Sync code detect mask */
    volatile UINT32                                         SlvsSyncDetectPattern;  /* 0x0054 */
    volatile UINT32                                         SlvsSyncCompareMask;    /* 0x0058 */
    volatile UINT32                                         SlvsSolPattern;         /* 0x005c: Pattern to match to indicate SOL code */
    volatile UINT32                                         SlvsEolPattern;         /* 0x0060: Pattern to match to indicate EOL code */
    volatile UINT32                                         SlvsSofPattern;         /* 0x0064: Pattern to match to indicate SOF code */
    volatile UINT32                                         SlvsEofPattern;         /* 0x0068: Pattern to match to indicate EOF code */
    volatile UINT32                                         SlvsSovPattern;         /* 0x006c: Pattern to match to indicate SOV code */
    volatile UINT32                                         SlvsEovPattern;         /* 0x0070: Pattern to match to indicate EOV code */
    volatile AMBA_DBG_PORT_IDSP_MIPI_CTRL0_REG_s            MipiCtrl0;              /* 0x0074 */
    volatile AMBA_DBG_PORT_IDSP_MIPI_CTRL1_REG_s            MipiCtrl1;              /* 0x0078 */
    volatile UINT32                                         MipiExp1Offset;         /* 0x007C: The second expousre offset compared to the first exposure */
    volatile UINT32                                         MipiExp2Offset;         /* 0x0080: The thrid expousre offset compared to the first exposure */
    volatile UINT32                                         MipiExp3Offset;         /* 0x0084: The fourth expousre offset compared to the first exposure */
    volatile UINT32                                         MipiCropRowStart;       /* 0x0088: Crop Row start (used in VC HDR mode) */
    volatile UINT32                                         MipiCropColStart;       /* 0x008C: Crop column start (used in HDR VC mode) */
    volatile UINT32                                         MipiCropWidthMinus1;    /* 0x0090: Crop width minus 1 (used in VC HDR mode) */
    volatile UINT32                                         MipiCropHeightMinus1;   /* 0x0094: Crop height minus 1 (used in VC HDR mode) */

    volatile AMBA_DBG_PORT_IDSP_VOUT_SYNC_REG_s             VoutSync[2];            /* 0x0098 -0x009C */
    volatile AMBA_DBG_PORT_IDSP_STRIG_CTRL_REG_s            Strig;                  /* 0x00A0 */
    volatile UINT32                                         StrigEndLine;           /* 0x00A4 */
    volatile AMBA_DBG_PORT_IDSP_STRIG_CTRL_REG_s            Strig1;                 /* 0x00A8 */
    volatile UINT32                                         Strig1EndLine;          /* 0x00AC */
    volatile UINT32                                         SyncDelayL;             /* 0x00B0: Delay in cycles after VSYNC before assertion of 'delayed vsync' signal to controller */
    volatile UINT32                                         SyncDelayH;             /* 0x00B4 */
    volatile UINT32                                         IntrDelayL;             /* 0x00B8: Delay in cycles after VSYNC before assertion of 'delayed vsync' interrupt to arm */
    volatile UINT32                                         IntrDelayH;             /* 0x00BC */
    volatile AMBA_DBG_PORT_IDSP_BLACKLEVEL_CTRL_REG_s       BlackLevelCtrl;         /* 0x00C0 */
    volatile UINT32                                         BlackLevelOffset16LSB;  /* 0x00C4 */
    volatile AMBA_DBG_PORT_IDSP_STATUS_REG_s                Status;                 /* 0x00C8 */
    volatile AMBA_DBG_PORT_IDSP_SLVS_STATUS_REG_s           SlvsStatus;             /* 0x00CC */
    volatile UINT32                                         SfifoCount;             /* 0x00D0: Number of entries in SFIFO/Buffer */
    volatile UINT32                                         DetectedActiveWidth;    /* 0x00D4: Detected Active Width */
    volatile UINT32                                         DetectedActiveHeight;   /* 0x00D8: Detected Active Height */
    volatile UINT32                                         SyncCode;               /* 0x00DC: Value of last received sync code (16-bit MSB aligned) */
    volatile UINT32                                         ActiveFrameHeight;      /* 0x00E0: Current active region height */
    volatile UINT32                                         ActiveFrameWidth;       /* 0x00E4: Current active region width */
    volatile UINT32                                         CropRegionHeight;       /* 0x00E8: Current crop region height */
    volatile UINT32                                         CropRegionWidth;        /* 0x00EC: Current crop region width */
    volatile AMBA_DBG_PORT_IDSP_MIPI_STATUS0_REG_s          MipiStatus0;            /* 0x00F0 */
    volatile UINT32                                         MipiLongPacketCount;    /* 0x00F4 */
    volatile UINT32                                         MipiShortPacketCount;   /* 0x00F8 */
    volatile UINT32                                         MipiLongPacketSize;     /* 0x00FC */
    volatile UINT32                                         MipiNumErrorFrames;     /* 0x0100 */
    volatile UINT32                                         MipiFrameNumber;        /* 0x0104 */
    volatile AMBA_DBG_PORT_IDSP_MIPI_STATUS1_REG_s          MipiStatus1;            /* 0x0108 */
    volatile UINT32                                         MipiFrame0Exp0RowCount; /* 0x010C: The first exposure row count of frame 0 */
    volatile UINT32                                         MipiFrame0Exp1RowCount; /* 0x0110: The second exposure row count of frame 0 */
    volatile UINT32                                         MipiFrame0Exp2RowCount; /* 0x0114: The third exposure row count of frame 0 */
    volatile UINT32                                         MipiFrame0Exp3RowCount; /* 0x0118: The fourth exposure row count of frame 0 */
    volatile UINT32                                         MipiFrame1Exp0RowCount; /* 0x011C: The first exposure row count of frame 1 */
    volatile UINT32                                         MipiFrame1Exp1RowCount; /* 0x0120: The second exposure row count of frame 1 */
    volatile UINT32                                         MipiFrame1Exp2RowCount; /* 0x0124: The third exposure row count of frame 1 */
    volatile UINT32                                         MipiFrame1Exp3RowCount; /* 0x0128: The fourth exposure row count of frame 1 */
    volatile UINT32                                         Reserved2[53];          /* 0x012C-0x01FC */
    volatile AMBA_DBG_PORT_IDSP_CONFIG_DONE_REG_s           ConfigDone;             /* 0x0200: manually shift VIN programming via debug bus */
} AMBA_DBG_PORT_IDSP_VIN_MAIN_REG_s;

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
