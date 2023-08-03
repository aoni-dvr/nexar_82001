/**
 *  @file AmbaReg_VIN.h
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
 *  @details Definitions & Constants for B6 VIN Control Registers
 *
 */

#ifndef AMBA_B8_REG_VIN_H
#define AMBA_B8_REG_VIN_H

/*-----------------------------------------------------------------------------------*\
 * B6 VIN : Control Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  SwReset:                1;      /* [0] Reset core VIN logic when config is loaded */
    UINT32  Enable:                 1;      /* [1] Enable Sensor Interface logic */
    UINT32  OutputEnable:           1;      /* [2] Enable output of VIN */
    UINT32  BitsPerPixel:           3;      /* [5:3] Sensor data width; 0=8-bit; 1=10-bit; 2=12-bit; 3=14-bit; 4=16-bit */
    UINT32  DataLane0Enable:        1;      /* [6] 1 = Enable SLVS/MIPI Physical Data Lane 0 */
    UINT32  DataLane1Enable:        1;      /* [7] 1 = Enable SLVS/MIPI Physical Data Lane 1 */
    UINT32  DataLane2Enable:        1;      /* [8] 1 = Enable SLVS/MIPI Physical Data Lane 2 */
    UINT32  DataLane3Enable:        1;      /* [9] 1 = Enable SLVS/MIPI Physical Data Lane 3 */
    UINT32  DataLane4Enable:        1;      /* [10] 1 = Enable SLVS/MIPI Physical Data Lane 4 */
    UINT32  DataLane5Enable:        1;      /* [11] 1 = Enable SLVS/MIPI Physical Data Lane 5 */
    UINT32  DataLane6Enable:        1;      /* [12] 1 = Enable SLVS/MIPI Physical Data Lane 6 */
    UINT32  DataLane7Enable:        1;      /* [13] 1 = Enable SLVS/MIPI Physical Data Lane 7 */
    UINT32  DataLane8Enable:        1;      /* [14] 1 = Enable SLVS/MIPI Physical Data Lane 8 */
    UINT32  DataLane9Enable:        1;      /* [15] 1 = Enable SLVS/MIPI Physical Data Lane 9 */
    UINT32  Reserved:               16;     /* [31:16] */

} B8_VIN_CTRL_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B6 VIN : DVP Control Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  PadType:                1;      /* [0] Pad type: 0=LVCMOS; 1=LVDS */
    UINT32  ParallelDataRate:       1;      /* [1] Data rate: 0=SDR; 1=DDR */
    UINT32  ParallelWidth:          1;      /* [2] 0=1-Pixel wide input; 1=2-Pixel wide input */
    UINT32  ParallelDataEdge:       1;      /* [3] Data valid on 0=posedge; 1=negedge */
    UINT32  SyncType:               1;      /* [4] Sync type: 0=Embedded/BT.656 Sync; 1=Separate/BT.601 Sync */
    UINT32  ParallelEmbSyncLoc:     2;      /* [6:5] For 2-pix width input, sync is on: 0=lower pix; 1=upper pix; 2/3=Interleaved across both */
    UINT32  YuvEnable:              1;      /* [7] Input type is: 0=RGB; 1=YUV */
    UINT32  YuvOrder:               2;      /* [9:8] Reorder pixels {3,2,1,0} --> 0={3,2,1,0} (Passthrough); 1={0,3,2,1}; 2={2,3,0,1}; 3={1,2,3,0} */
    UINT32  VsyncPolarity:          1;      /* [10] External/601 VSYNC is 0=Active High; 1=Active Low */
    UINT32  HsyncPolarity:          1;      /* [11] External/601 HSYNC is 0=Active High; 1=Active Low */
    UINT32  FieldPolarity:          1;      /* [12] External/601 FIELD is 0=Non-inverted; 1=Inverted */
    UINT32  FieldMode:              1;      /* [13] 0=Field sampled at active edge of vsync; 1=Field sampled continuously */
    UINT32  Reserved:               18;     /* [31:14] N/A for B6 */
} B8_VIN_DVP_CTRL_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B6 VIN : Separate/BT.601 Sync Signal Pin Select Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  FieldPinSelect:         5;      /* [4:0] 0-9: pad_p[x]; 10-19: pad_n[x]; 20: clk_n[0]; 21: clk_n[1]; 22: clk_p[1] */
    UINT32  VsyncPinSelect:         5;      /* [9:5] 0-9: pad_p[x]; 10-19: pad_n[x]; 20: clk_n[0]; 21: clk_n[1]; 22: clk_p[1] */
    UINT32  HsyncPinSelect:         5;      /* [14:10] 0-9: pad_p[x]; 10-19: pad_n[x]; 20: clk_n[0]; 21: clk_n[1]; 22: clk_p[1] */
    UINT32  Reserved:               17;     /* [31:15] */
} B8_VIN_SYNC_PIN_SELECT_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B6 VIN : SLVS Data Lane 0-3 Pin Select Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  DataLane0PinSelect:     4;      /* [3:0] pad_p[x] and pad_n[x] */
    UINT32  DataLane1PinSelect:     4;      /* [7:4] pad_p[x] and pad_n[x] */
    UINT32  DataLane2PinSelect:     4;      /* [11:8] pad_p[x] and pad_n[x] */
    UINT32  DataLane3PinSelect:     4;      /* [15:12] pad_p[x] and pad_n[x] */
    UINT32  Reserved:               16;     /* [31:16] */
} B8_VIN_DATA_LANE_PIN_SELECT0_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B6 VIN : SLVS Data Lane 4-7 Pin Select Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  DataLane4PinSelect:     4;      /* [3:0] pad_p[x] and pad_n[x] */
    UINT32  DataLane5PinSelect:     4;      /* [7:4] pad_p[x] and pad_n[x] */
    UINT32  DataLane6PinSelect:     4;      /* [11:8] pad_p[x] and pad_n[x] */
    UINT32  DataLane7PinSelect:     4;      /* [15:12] pad_p[x] and pad_n[x] */
    UINT32  Reserved:               16;     /* [31:16] */
} B8_VIN_DATA_LANE_PIN_SELECT1_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B6 VIN : SLVS Data Lane 8-9 Pin Select Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  CropDisable:            1;      /* [0] Disable crop region; 1: ignore the crop_start/end_col/row */
    UINT32  NonBlocking:            1;      /* [1] Only set to 1 when bring up VIN to check sof/eof interrupts */
    UINT32  EolHold:                1;      /* [2] reserved bit, only set to 1 when requested by designer */
    UINT32  Reserved:               29;     /* [31:3] */
} B8_VIN_DEBUG_CTRL_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B6 VIN : SLVS Sync Code Location Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  SyncInterleaving:       2;     /* [1:0] 0=none; 1=2-lane interleaving; 2=4-lane interleaving (For HiSPi Streaming-S) */
    UINT32  SyncTolerance:          2;     /* [3:2] 0=No error tolerance; 1=Allow a corrupted sync every other code; 2=Always allow corruption */
    UINT32  SyncAllLanes:           1;     /* [4] 1=Sync code must be detected on all active lanes in each group; 0=Majority of active lanes */
    UINT32  Reserved:               27;    /* [31:5] */
} B8_VIN_SLVS_SYNC_CODE_LOCATION_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B6 VIN : SLVS Control Register
\*-----------------------------------------------------------------------------------*/
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
} B8_VIN_SLVS_CTRL_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B6 VIN : SLVS Sync Code Detection Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  DetectSOL:              1;      /* [0] Enable Detection of SOL sync codes */
    UINT32  DetectEOL:              1;      /* [1] Enable Detection of EOL sync codes */
    UINT32  DetectSOF:              1;      /* [2] Enable Detection of SOF sync codes */
    UINT32  DetectEOF:              1;      /* [3] Enable Detection of EOF sync codes */
    UINT32  DetectSOV:              1;      /* [4] Enable Detection of SOV sync codes */
    UINT32  DetectEOV:              1;      /* [5] Enable Detection of EOV sync codes */
    UINT32  Reserved:               26;     /* [31:6] */
} B8_VIN_SLVS_SYNC_CODE_DETECT_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B6 VIN : MIPI Data Filter Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  VirtChanMask:           2;      /* [1:0] MIPI virtual channel mask (Data captured when (~VC_MASK & virtual_channel) == VC_PAT */
    UINT32  VirtChanPattern:        2;      /* [3:2] MIPI virtual channel pattern to match for valid data */
    UINT32  DataTypeMask:           6;      /* [9:4] MIPI data type mask (Data captured when (~DT_MASK & data_id) == DT_PAT */
    UINT32  DataTypePattern:        6;      /* [15:10] MIPI data type pattern to match for valid data */
    UINT32  Reserved:               16;     /* [31:16] */
} B8_VIN_MIPI_DATA_FILTER_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B6 VIN : MIPI Control Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  DeCompressionEnable:    1;      /* [0] 1 = Enable decompression */
    UINT32  DeCompressionMode:      2;      /* [2:1] Decompression Mode */
    UINT32  ByteSwapEnable:         1;      /* [3] 1 = Reverse order of pixels */
    UINT32  EccEnable:              1;      /* [4] 1 = Enable packet header ECC */
    UINT32  FwdErrPkt:              1;      /* [5] 0:Do not forward errored packets; 1: forward errored packets (default mode) */
    UINT32  MaxSkewCycle:           3;      /* [8:6] Only set it to non-zero in 8-lane MIPI sensor */
    UINT32  Reserved:               23;     /* [31:9] */
} B8_VIN_MIPI_CTRL_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B6 VIN : Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  Mode:                   2;      /* [1:0] 0=Disabled; 1=Even Fields; 2=Odd Fields; 3=All Fields */
    UINT32  Line:                   14;     /* [15:2] VOUT sync0 is asserted for the duration of the specified active line */
    UINT32  Reserved:               16;     /* [31:16] */
} B8_VIN_VOUT_SYNC_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B6 VIN : Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  Enable:                 1;      /* [0] Enable GPIO strig0 */
    UINT32  Polarity:               1;      /* [1] 0=Active low; 1=Active high */
    UINT32  StartLine:              14;     /* [15:2] Active line number to start asserting strig0 */
    UINT32  Reserved:               16;     /* [31:16] */
} B8_VIN_STRIG_CTRL_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B6 VIN : Status Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  GotActSof:              1;      /* [0] Detected start of active region (write 1 to clear) */
    UINT32  Strig0Status:           1;      /* [1] Strig0 triggered (write 1 to clear) */
    UINT32  Strig1Status:           1;      /* [2] Strig1 triggered (write 1 to clear) */
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
    UINT32  Reserved:               17;     /* [31:15] */
} B8_VIN_STATUS_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B6 VIN : SLVS Status Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  SyncLocked:             1;      /* [0] VIN front-end sync is locked */
    UINT32  LostLockAfterSof:       1;      /* [1] Lost lock after detecting start of frame (must reset VIN) */
    UINT32  PartialSyncDetected:    1;      /* [2] Partial/corrupted sync was detected (write 1 to clear) */
    UINT32  UnknownSyncCode:        1;      /* [3] Sync code did not match any patterns (write 1 to clear) */
    UINT32  GotWinSof:              1;      /* [4] Reached start of crop region (write 1 to clear) */
    UINT32  AfifoOverflow:          1;      /* [5] AFIFO overflow (must reset VIN) */
    UINT32  SyncState:              2;      /* [7:6] State of front-end sync FSM (0=IDLE; 1=ACTIVE; 2=HBLANK; 3=VBLANK) */
    UINT32  Reserved:               24;     /* [31:8] */
} B8_VIN_SLVS_STATUS_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B6 VIN : MIPI Status Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  EccError2Bit:           1;      /* [0] 2-bit uncorrectable ECC error */
    UINT32  EccError1Bit:           1;      /* [1] 1-bit ECC error corrected */
    UINT32  CrcError:               1;      /* [2] Long Packet CRC Error */
    UINT32  ErrId:                  1;      /* [3] Unexpected packet data ID received */
    UINT32  FrameSyncError:         1;      /* [4] Frame sync error (Received unpaired SOF or EOF) */
    UINT32  ProtState:              2;      /* [6:5] State of FSM in MIPI protocol module */
    UINT32  SofPacketReceived:      1;      /* [7] SOF Short Packet Received */
    UINT32  EofPacketReceived:      1;      /* [8] EOF Short Packet Received */
    UINT32  Reserved:               23;     /* [31:9] */
} B8_VIN_MIPI_STATUS_REG_s;

typedef struct {
    UINT32  SerdesSfifoOverflow:    1;      /* [0] */
    UINT32  SerdesSfifoCount:       12;     /* [12:1] */
    UINT32  Reserved:               19;     /* [31:13] */
} B8_VIN_SERDES_STATUS_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B6 VIN : Master Sync Control Register
\*-----------------------------------------------------------------------------------*/
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
} B8_VIN_MASTER_SYNC_CTRL_REG_s;

/*-----------------------------------------------------------------------------------------------*\
 * B6 VIN : All Registers
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    UINT32  TimeOutL;       /* Timeout value for sync detection (Bit[15:0]) */
    UINT32  TimeOutH;       /* Timeout value for sync detection (Bit[31:16]) */
} B8_VIN_SYNC_TIMEOUT_REG_s;

typedef struct {
    UINT32  SensorType:             2;      /* [1:0] 0=Serial SLVS; 1=Parallel LVCMOS; 2=MIPI; 3=Parallel LVDS */
    UINT32  SlvsClkMode:            1;      /* [2] 0=All Lanes latched with spclk[0]; 1=Every 4 lanes latched by group/phy clock */
    UINT32  AFifoBypass:            1;      /* [3] Bypass CDC/SLVS FIFOs; all PHY data is latched by slvs clk0 */
    UINT32  Reserved:               28;     /* [31:4] */
} B8_VIN_GLOBAL_REG_s;

typedef struct {
    B8_VIN_CTRL_REG_s                       Ctrl;                   /* 0x000(RW): Control Register */
    B8_VIN_DVP_CTRL_REG_s                   DvpCtrl;                /* 0x004(RW): DVP Control Register */
    B8_VIN_SYNC_PIN_SELECT_REG_s            SyncPinSelect;          /* 0x008(RW): Separate/BT.601 Sync Signal Pin Select Register */
    B8_VIN_DATA_LANE_PIN_SELECT0_REG_s      DataLanePinSelect0;     /* 0x00C(RW): SLVS Data Lane 0-3 Pin Select Register */
    B8_VIN_DATA_LANE_PIN_SELECT1_REG_s      DataLanePinSelect1;     /* 0x010(RW): SLVS Data Lane 4-7 Pin Select Register */
    B8_VIN_DEBUG_CTRL_REG_s                 DebugCtrl;              /* 0x014(RW): for debugging */
    UINT32                                  ActiveRegionWidth;      /* 0x018(RW): Active Video Region Width Register. 0=auto-detect, else=active width - 1 */
    UINT32                                  ActiveRegionHeight;     /* 0x01C(RW): Active Video Region Height Register. 0=auto-detect, else=active height - 1 */
    UINT32                                  LineSplitWidth;         /* 0x020(RW): Split each input line to several partital lines (0=disabled, otherwise partial lines width in pixels) */
    UINT32                                  CropStartCol;           /* 0x024(RW): Start column index of crop region */
    UINT32                                  CropStartRow;           /* 0x028(RW): Start row index of crop region */
    UINT32                                  CropEndCol;             /* 0x02C(RW): End column index of crop region */
    UINT32                                  CropEndRow;             /* 0x030(RW): End row index of crop region */
    B8_VIN_SLVS_SYNC_CODE_LOCATION_REG_s    SlvsSyncCodeLocation;   /* 0x034(RW): SLVS Sync Code Location */
    B8_VIN_SLVS_CTRL_REG_s                  SlvsCtrl;               /* 0x038(RW): SLVS Control */
    B8_VIN_SLVS_SYNC_CODE_DETECT_REG_s      SlvsSyncCodeDetect;     /* 0x03C(RW): SLVS Sync Code Detection */
    B8_VIN_SYNC_TIMEOUT_REG_s               SyncTimeOut;            /* 0x040-0x044(RW): 0 = disabled; otherwise = Timeout in sclk cycles*2 */
    UINT32                                  SlvsSyncDetectMask;     /* 0x048(RW): */
    UINT32                                  SlvsSyncDetectPattern;  /* 0x04C(RW): */
    UINT32                                  SlvsSyncCompareMask;    /* 0x050(RW): Mask to patch sync code before comparing to desired patterns */
    UINT32                                  SlvsSolPattern;         /* 0x054(RW): Pattern to match to indicate SOL code */
    UINT32                                  SlvsEolPattern;         /* 0x058(RW): Pattern to match to indicate EOL code */
    UINT32                                  SlvsSofPattern;         /* 0x05C(RW): Pattern to match to indicate SOF code */
    UINT32                                  SlvsEofPattern;         /* 0x060(RW): Pattern to match to indicate EOF code */
    UINT32                                  SlvsSovPattern;         /* 0x064(RW): Pattern to match to indicate SOV code */
    UINT32                                  SlvsEovPattern;         /* 0x068(RW): Pattern to match to indicate EOV code */
    B8_VIN_MIPI_DATA_FILTER_REG_s           MipiDataFilter;         /* 0x06C(RW): MIPI Data Filter */
    B8_VIN_MIPI_CTRL_REG_s                  MipiCtrl;               /* 0x070(RW): MIPI Control */
    B8_VIN_VOUT_SYNC_REG_s                  VoutSync0;              /* 0x074(RW): N/A for B6 */
    B8_VIN_VOUT_SYNC_REG_s                  VoutSync1;              /* 0x078(RW): N/A for B6 */
    B8_VIN_STRIG_CTRL_REG_s                 Strig0Ctrl;             /* 0x07C(RW): N/A for B6 */
    UINT32                                  Strig0EndLine;          /* 0x080(RW): N/A for B6 */
    B8_VIN_STRIG_CTRL_REG_s                 Strig1Ctrl;             /* 0x084(RW): N/A for B6 */
    UINT32                                  Strig1EndLine;          /* 0x088(RW): N/A for B6  */
    UINT32                                  SyncDelayL;             /* 0x08C(RW): Delay in cycles after VSYNC before assertion of 'delayed vsync' signal to controller */
    UINT32                                  SyncDelayH;             /* 0x090(RW): */
    UINT32                                  IntrDelayL;             /* 0x094(RW): Delay in cycles after VSYNC before assertion of 'delayed vsync' interrupt to arm */
    UINT32                                  IntrDelayH;             /* 0x098(RW): */
    B8_VIN_STATUS_REG_s                     Status;                 /* 0x09C(RWC): Status */
    B8_VIN_SLVS_STATUS_REG_s                SlvsStatus;             /* 0x0A0(RWC): SLVS Status*/
    UINT32                                  SfifoCount;             /* 0x0A4(RO): Number of entries in SFIFO/Buffer */
    UINT32                                  DetectedActiveWidth;    /* 0x0A8(RO): Detected Active Width */
    UINT32                                  DetectedActiveHeight;   /* 0x0AC(RO): Detected Active Height */
    UINT32                                  SyncCode;               /* 0x0B0(RO): Value of last received sync code (16-bit MSB aligned) */
    UINT32                                  ActiveFrameHeight;      /* 0x0B4(RO): Current active region height */
    UINT32                                  ActiveFrameWidth;       /* 0x0B8(RO): Current active region width */
    UINT32                                  CropRegionHeight;       /* 0x0BC(RO): Current crop region height */
    UINT32                                  CropRegionWidth;        /* 0x0C0(RO): Current crop region width */
    B8_VIN_MIPI_STATUS_REG_s                MipiStatus;             /* 0x0C4(RWC): MIPI Status */
    UINT32                                  MIPI_LONG_PKT_COUNT;    /* 0x0C8(RWC): Number of long packets received */
    UINT32                                  MIPI_SHORT_PKT_COUNT;   /* 0x0CC(RWC): Number of short packets received */
    UINT32                                  MIPI_LONG_PACKET_SIZE;  /* 0x0D0(RO): Number of bytes in last received long packet */
    UINT32                                  MIPI_NUM_ERR_FRAMES;    /* 0x0D4(RWC): Number of frames with CRC errors */
    UINT32                                  MIPI_FRAME_NUMBER;      /* 0x0D8(RO): Frame number (from short-packet/header info) */
    B8_VIN_SERDES_STATUS_REG_s              Serdes0Status;          /* 0x0DC(RO): sfifo assigned to the first serdes has overflowed */
    B8_VIN_SERDES_STATUS_REG_s              Serdes1Status;          /* 0x0E0(RO): sfifo assigned to the first serdes has overflowed */
    UINT32                                  Reserved0[6];           /* 0x0E4-0x0F8: */
    UINT32                                  ConfigDone;             /* 0x0FC(WO): Activate VIN Config */
    B8_VIN_GLOBAL_REG_s                     GlobalConfig;           /* 0x100(RW): Global Config */
    UINT32                                  Reserved1[63];          /* 0x104-0x1FC: */
    UINT32                                  HsyncPeriodL;           /* 0x200(RW): HSYNC period in cycles (low bits) */
    UINT32                                  HsyncPeriodH;           /* 0x204(RW): HSYNC period in cycles (high bits) */
    UINT32                                  HsyncWidth;             /* 0x208(RW): Width of HSYNC pulse in cycles */
    UINT32                                  HsyncOffset;            /* 0x20C(RW): Offset of HSYNC pulse in cycles */
    UINT32                                  VsyncPeriod;            /* 0x210(RW): Number of HSYNCs between VSYNCs */
    UINT32                                  VsyncWidth;             /* 0x214(RW): Width of VSYNC pulse in cycles OR HSYNCs (see VSYNC_WIDTH_sNIT) */
    UINT32                                  VsyncOffset;            /* 0x218(RW): Offset between HSYNC and VSYNC edges (in cycles); VSYNC trails HSYNC */
    B8_VIN_MASTER_SYNC_CTRL_REG_s           MasterSyncCtrl;         /* 0x21C(RW): Master Sync Control */
    UINT32                                  Reserved2[120];         /* 0x220-0x3FC: */
} B8_VIN_REG_s;

#endif /* AMBA_B8_REG_VIN_H */
