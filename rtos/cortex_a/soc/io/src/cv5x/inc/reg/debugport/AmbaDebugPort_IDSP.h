/**
 *  @file AmbaDebugPort_IDSP.h
 *
 *  @copyright Copyright (c) 2021 Ambarella International LP
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
    UINT32 FilterAddress:            3;      /* [2:0]: Filter Address MSB (Bank#) */
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
#define AMBA_DBG_PORT_IDSP_SECTION15    15U  /* Section 15 (for main IDSP only) */
#define AMBA_DBG_PORT_IDSP_SECTION16    16U  /* Section 16 (for main IDSP only) */
#define AMBA_DBG_PORT_IDSP_SECTION17    17U  /* Section 17 (for main IDSP only) */
#define AMBA_DBG_PORT_IDSP_SECTION18    18U  /* Section 18 (for main IDSP only) */

#define AMBA_DBG_PORT_IDSP_VIN0         (AMBA_DBG_PORT_IDSP_SECTION1)
#define AMBA_DBG_PORT_IDSP_VIN1         (AMBA_DBG_PORT_IDSP_SECTION2)
#define AMBA_DBG_PORT_IDSP_VIN2         (AMBA_DBG_PORT_IDSP_SECTION3)
#define AMBA_DBG_PORT_IDSP_VIN3         (AMBA_DBG_PORT_IDSP_SECTION4)
#define AMBA_DBG_PORT_IDSP_VIN4         (AMBA_DBG_PORT_IDSP_SECTION5)
#define AMBA_DBG_PORT_IDSP_VIN5         (AMBA_DBG_PORT_IDSP_SECTION6)
#define AMBA_DBG_PORT_IDSP_VIN6         (AMBA_DBG_PORT_IDSP_SECTION7)
#define AMBA_DBG_PORT_IDSP_VIN7         (AMBA_DBG_PORT_IDSP_SECTION8)
#define AMBA_DBG_PORT_IDSP_VIN8         (AMBA_DBG_PORT_IDSP_SECTION9)
#define AMBA_DBG_PORT_IDSP_VIN9         (AMBA_DBG_PORT_IDSP_SECTION10)
#define AMBA_DBG_PORT_IDSP_VIN10        (AMBA_DBG_PORT_IDSP_SECTION11)
#define AMBA_DBG_PORT_IDSP_VIN11        (AMBA_DBG_PORT_IDSP_SECTION12)
#define AMBA_DBG_PORT_IDSP_VIN12        (AMBA_DBG_PORT_IDSP_SECTION13)
#define AMBA_DBG_PORT_IDSP_VIN13        (AMBA_DBG_PORT_IDSP_SECTION14)

/*
 * IDSP Debug Port: Section Reset Register
 */
typedef struct {
    UINT32 ResetVin0SyncOut:        1;      /* [0]: Reset Vin0 Master SYNC Generation Logic */
    UINT32 CorrespondSection:       18;     /* [18:1]: Corresponding to Section (Main IDSP: 18 - 1, Vin IDSP: 14 - 1) */
    UINT32 Reserved:                12;     /* [30:19] */
    UINT32 ResetVin4SyncOut:        1;      /* [31]: Reset Vin4 Master SYNC Generation Logic */
} AMBA_DBG_PORT_IDSP_SEC_RESET_REG_s;

/*
 * IDSP Debug Port All Registers
 */
typedef struct {
    volatile UINT32                                   AddressExtension;       /* 0x8000: Address Extension Register (AMBA_DBG_PORT_IDSP_ADD_EXT_REG_s) */
    volatile UINT32                                   ConfigDataStatus;       /* 0x8004: Config Controller Internal Debug State */
    volatile UINT32                                   Reserved[4];            /* 0x8008 - 0x8014: */
    volatile UINT32                                   SectionResetStatus;     /* 0x8018: Section Reset Status */
    volatile UINT32                                   Reserved1;              /* 0x801C: */
    volatile UINT32                                   InternalErrStatus;      /* 0x8020: Internal Error Status */
    volatile UINT32                                   Reserved2[87];          /* 0x8024 - 0x817C: */
    volatile UINT32                                   SectionReset;           /* 0x8180: Section Reset Register (AMBA_DBG_PORT_IDSP_SEC_RESET_REG_s) */
} AMBA_DBG_PORT_IDSP_CONTROLLER_REG_s;

typedef struct {
    UINT32  SwReset:                1;      /* [0] Reset core VIN logic when config is loaded */
    UINT32  Enable:                 1;      /* [1] Enable Sensor Interface logic */
    UINT32  OutputEnable:           1;      /* [2] Enable output of VIN */
    UINT32  BitsPerPixel:           3;      /* [5:3] Sensor data width; 0=8-bit; 1=10-bit; 2=12-bit; 3=14-bit; 4=16-bit */
    UINT32  Reserved:               26;     /* [31:6] Reserved */
} AMBA_DBG_PORT_IDSP_CTRL0_REG_s;

typedef struct {
    UINT32  DataLaneEnable:         16;     /* [15:0] 1 = Enable SLVS/MIPI Physical Data Lane */
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
    UINT32  DisableAfterSync:       1;      /* [14] */
    UINT32  DoubleBufferEnable:     1;      /* [15] */
    UINT32  Reserved:               16;     /* [31:16] Reserved */
} AMBA_DBG_PORT_IDSP_DVP_CTRL_REG_s;

typedef struct {
    UINT32  FieldPinSelect:         5;      /* [4:0] 0:clk_n[0]; 1:clk_n[1]; 2:clk_p[1] */
    UINT32  VsyncPinSelect:         5;      /* [9:5] 0:clk_n[0]; 1:clk_n[1]; 2:clk_p[1] */
    UINT32  HsyncPinSelect:         5;      /* [14:10] 0:clk_n[0]; 1:clk_n[1]; 2:clk_p[1] */
    UINT32  Reserved:               17;     /* [31:15] */
} AMBA_DBG_PORT_IDSP_SYNC_PIN_MUX_REG_s;

typedef struct {
    UINT32  DataLane0_4_8_12PinSel: 4;      /* [3:0] pad_p[x] and pad_n[x] */
    UINT32  DataLane1_5_9_13PinSel: 4;      /* [7:4] pad_p[x] and pad_n[x] */
    UINT32  DataLane2_6_10_14PinSel:4;      /* [11:8] pad_p[x] and pad_n[x] */
    UINT32  DataLane3_7_11_15PinSel:4;      /* [15:12] pad_p[x] and pad_n[x] */
    UINT32  Reserved:               16;     /* [31:16] Reserved */
} AMBA_DBG_PORT_IDSP_DATA_PIN_MUX_REG_s;

typedef struct {
    UINT32  DisableCropRegion:      1;      /* [0] Disable crop region; if it is 1, ignore the crop_start/end_col/row */
    UINT32  DbgNonBlocking:         1;      /* [1] Only set to 1 when bring up VIN to check sof/eof interrupts */
    UINT32  DbgEolHold:             1;      /* [2] reserved bit, only set to 1 when requested by designer */
    UINT32  MipiMaxSkewCycle:       3;      /* [5:3] Only set it to non-zero value in 8-lane MIPI sensor; its unit is mipi_byteclk cycle; or 4*mipi_sensor_clk cycle */
    UINT32  Reserved:               26;     /* [31:6] */
} AMBA_DBG_PORT_IDSP_CTRL2_REG_s;

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
    UINT32  GclkSoVinPassThr:       1;      /* [0] Only used while C-PHY is working; 0:bclk is selected from word clock or gclk_so_vin by rx_activehs; 1:bclk use gclk_so_vin pass through */
    UINT32  EccVCxOverride:         1;      /* [1] 0:vcx bits will be operated in D-PHY ECC(CSI-2 2.0); 1:vcx bits will be overrided in D-PHY ECC(For CSI-2 1.0); */
    UINT32  VCxMask:                3;      /* [4:2] */
    UINT32  VCxPattern:             3;      /* [7:5] */
    UINT32  Reserved:               24;     /* [31:8] Reserved */
} AMBA_DBG_PORT_IDSP_MIPI_CTRL2_REG_s;

typedef struct {
    UINT32  BypassMode:             1;      /* [0] Bypass SLVS-EC pipeline (for 10b8b passthrough) */
    UINT32  Reserved:               3;      /* [3:1] Reserved */
    UINT32  ValidOnly:              1;      /* [4] Outputs lines only when valid bit in header is set */
    UINT32  BypassFifo:             1;      /* [5] Sends phy data straight to the output */
    UINT32  LinkType:               2;      /* [7:6] 0/1=single link;2=dual link (pixel interleave);3=dual link (pixel interleave, swap) */
    UINT32  EccOption:              2;      /* [9:8] 0=ecc disabled;1=1t ecc;2=2t ecc */
    UINT32  Reserved1:              22;     /* [31:10] Reserved */
} AMBA_DBG_PORT_IDSP_SLVSEC_CTRL0_REG_s;

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
    UINT32  GotWinSof:              1;      /* [1] Reached start of crop region (write 1 to clear) */
    UINT32  GotWinEof:              1;      /* [2] Reached end of crop region (write 1 to clear) */
    UINT32  GotVsync:               1;      /* [3] Reached end of active region (start of VBLANK) (write 1 to clear) */
    UINT32  AfifoOverflow:          1;      /* [4] AFIFO overflow (must reset VIN) */
    UINT32  SfifoOverFlow:          1;      /* [5] SFIFO has overflowed (must reset VIN) */
    UINT32  ShortLine:              1;      /* [6] EOL received before reaching end of programmed crop width (write 1 to clear) */
    UINT32  ShortFrame:             1;      /* [7] EOF received before reaching end of programmed crop region (write 1 to clear) */
    UINT32  Field:                  1;      /* [8] Field bit value */
    UINT32  Reserved:               1;      /* [9] Reserved */
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
    UINT32  CrcPhError:             1;      /* [2] C-PHY Packet Header CRC Error */
    UINT32  CrcPkError:             1;      /* [3] Long Packet CRC Error */
    UINT32  FrameSyncError:         1;      /* [4] Frame sync error (Received unpaired SOF or EOF) */
    UINT32  ProtState:              2;      /* [6:5] State of FSM in MIPI protocol module */
    UINT32  SofPacketReceived:      1;      /* [7] SOF Short Packet Received */
    UINT32  EofPacketReceived:      1;      /* [8] EOF Short Packet Received */
    UINT32  Reserved:               23;     /* [31:9] */
} AMBA_DBG_PORT_IDSP_MIPI_STATUS0_REG_s;

typedef struct {
    UINT32  CrcError:               1;      /* [0] CRC Error */
    UINT32  LineBoundaryError:      1;      /* [1] Line Boundary Error */
    UINT32  DelayedLineEnd:         1;      /* [2] Delayed Line End Error */
    UINT32  TruncatedPacket:        1;      /* [3] Truncated Packet Error */
    UINT32  IllegalStandby:         1;      /* [4] Illegal Standby Error */
    UINT32  B2PPushState:           3;      /* [7:5] State of B2P module */
    UINT32  DepacketState:          2;      /* [9:8] State of Depacket module */
    UINT32  PhyRxState:             4;      /* [13:10] State of PHY FSM */
    UINT32  Reserved:               18;     /* [31:14] */
} AMBA_DBG_PORT_IDSP_SLVSEC_ST0_REG_s;

typedef struct {
    UINT32  Channel:                2;      /* [1:0] Channel in header packet */
    UINT32  PhyStandbyDet:          12;     /* [13:2] Standby status from phy */
    UINT32  Reserved:               18;     /* [31:14] */
} AMBA_DBG_PORT_IDSP_SLVSEC_ST1_REG_s;

typedef struct {
    UINT32  PhyReady:               12;     /* [11:0] READY status from phy */
    UINT32  Reserved:               20;     /* [31:12] */
} AMBA_DBG_PORT_IDSP_SLVSEC_ST2_REG_s;

typedef struct {
    UINT32  PhySymbolErr:           12;     /* [11:0] SYMBOLERR received from PHY (write to clear) */
    UINT32  Reserved:               20;     /* [31:12] */
} AMBA_DBG_PORT_IDSP_SLVSEC_ST3_REG_s;

typedef struct {
    UINT32  Deskew0:                12;     /* [11:0] Deskew status lane0-3 */
    UINT32  Reserved:               20;     /* [31:12] */
} AMBA_DBG_PORT_IDSP_SLVSEC_ST4_REG_s;

typedef struct {
    UINT32  Deskew1:                12;     /* [11:0] Deskew status lane4-7 */
    UINT32  Reserved:               20;     /* [31:12] */
} AMBA_DBG_PORT_IDSP_SLVSEC_ST5_REG_s;

typedef struct {
    UINT32  Deskew2:                12;     /* [11:0] Deskew status lane8-12 */
    UINT32  Reserved:               20;     /* [31:12] */
} AMBA_DBG_PORT_IDSP_SLVSEC_ST6_REG_s;

typedef struct {
    UINT32  Correct:                1;      /* [0] ECC correct */
    UINT32  Correctable:            1;      /* [1] ECC correctable*/
    UINT32  UnCorrectable:          1;      /* [2] ECC uncorrectable*/
    UINT32  Reserved:               29;     /* [31:3] */
} AMBA_DBG_PORT_IDSP_SLVSEC_ECC_ST_REG_s;

typedef struct {
    UINT32  DebugCommandShift:      1;      /* [0] To manually shift VIN programming via debug bus */
    UINT32  Reserved:               31;     /* [31:1] */
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
    UINT32  SensorType:             3;      /* [2:0] 0=Serial SLVS; 1=Parallel SLVS; 2=Parallel LVCMOS; 3=MIPI D-PHY; 5=SLVS-EC; 6=MIPI C-PHY */
    UINT32  SlvsClkMode:            1;      /* [3] 0=All Lanes latched with spclk[0]; 1=Every 4 lanes latched by group/phy clock */
    UINT32  AFIFOBypass:            1;      /* [4] Bypass CDC/SLVS FIFOs; all PHY data is latched by slvs clk0 */
    UINT32  SlvsEcClkMode:          1;      /* [5] */
    UINT32  SlvsEcPhyReset:         1;      /* [6] */
    UINT32  SlvsEcPhySetup:         1;      /* [7] */
    UINT32  SlvsEcPhyStandby:       1;      /* [8] */
    UINT32  SlvsEcPhyConfig:        1;      /* [9] */
    UINT32  SlvsEcPhyPulseDuration: 5;      /* [14:10] */
    UINT32  SlvsEcPhyOverride:      1;      /* [15] */
    UINT32  Reserved:               16;     /* [31:16] */
} AMBA_DBG_PORT_IDSP_GLOBAL_REG_s;

typedef struct {
    volatile AMBA_DBG_PORT_IDSP_CTRL0_REG_s                 Ctrl0;                  /* 0x0000: Sensor interface module control0 */
    volatile AMBA_DBG_PORT_IDSP_CTRL1_REG_s                 Ctrl1;                  /* 0x0004: Sensor interface module control1 */
    volatile AMBA_DBG_PORT_IDSP_DVP_CTRL_REG_s              DvpCtrl;                /* 0x0008: Parallel control register */
    volatile AMBA_DBG_PORT_IDSP_SYNC_PIN_MUX_REG_s          SyncPinMux;             /* 0x000C: VIN sync pin MUX Configuration Register */
    volatile AMBA_DBG_PORT_IDSP_DATA_PIN_MUX_REG_s          DataPinMux0;            /* 0x0010: VIN data pin MUX Configuration Register, for data pin 0/1/2/3 */
    volatile AMBA_DBG_PORT_IDSP_DATA_PIN_MUX_REG_s          DataPinMux1;            /* 0x0014: VIN data pin MUX Configuration Register, for data pin 4/5/6/7 */
    volatile AMBA_DBG_PORT_IDSP_DATA_PIN_MUX_REG_s          DataPinMux2;            /* 0x0018: VIN data pin MUX Configuration Register, for data pin 8/9/10/11 */
    volatile AMBA_DBG_PORT_IDSP_DATA_PIN_MUX_REG_s          DataPinMux3;            /* 0x001C: VIN data pin MUX Configuration Register, for data pin 12/13/14/15 */
    volatile AMBA_DBG_PORT_IDSP_CTRL2_REG_s                 Ctrl2;                  /* 0x0020: Sensor interface module control2 */
    volatile UINT32                                         ActiveRegionWidth;      /* 0x0024: active width - 1, 0=auto detected */
    volatile UINT32                                         ActiveRegionHeight;     /* 0x0028: active height - 1, 0=auto-detect */
    volatile UINT32                                         SplitWidth;             /* 0x002C: For readouts that pack multiple exposure lines into a single readout line (0=disabled) */
    volatile UINT32                                         CropStartCol;           /* 0x0030: Start column index of crop region */
    volatile UINT32                                         CropStartRow;           /* 0x0034: Start row index of crop region */
    volatile UINT32                                         CropEndCol;             /* 0x0038: End column index of crop region */
    volatile UINT32                                         CropEndRow;             /* 0x003C: End row index of crop region */
    volatile AMBA_DBG_PORT_IDSP_SLVS_SYNC_LOCATION_REG_s    SlvsSyncCodeLocation;   /* 0x0040: SLVS Sync code location */
    volatile AMBA_DBG_PORT_IDSP_SLVS_CTRL_REG_s             SlvsCtrl;               /* 0x0044: SLVS control */
    volatile AMBA_DBG_PORT_IDSP_SLVS_SYNC_DETECT_REG_s      SlvsSyncCodeDetect;     /* 0x0048: SLVS sync code detection enable */
    volatile UINT32                                         WatchdogTimeoutL;       /* 0x004C: Watchdog timeout value (Bit[15:0]) in unit of idsp cycles, 0=disabled */
    volatile UINT32                                         WatchdogTimeoutH;       /* 0x0050: Watchdog timeout value (Bit[31:16]) in unit of idsp cycles, 0=disabled */
    volatile UINT32                                         SlvsSyncDetectMask;     /* 0x0054: Sync code detect mask */
    volatile UINT32                                         SlvsSyncDetectPattern;  /* 0x0058 */
    volatile UINT32                                         SlvsSyncCompareMask;    /* 0x005C */
    volatile UINT32                                         SlvsSolPattern;         /* 0x0060: Pattern to match to indicate SOL code */
    volatile UINT32                                         SlvsEolPattern;         /* 0x0064: Pattern to match to indicate EOL code */
    volatile UINT32                                         SlvsSofPattern;         /* 0x0068: Pattern to match to indicate SOF code */
    volatile UINT32                                         SlvsEofPattern;         /* 0x006C: Pattern to match to indicate EOF code */
    volatile UINT32                                         SlvsSovPattern;         /* 0x0070: Pattern to match to indicate SOV code */
    volatile UINT32                                         SlvsEovPattern;         /* 0x0074: Pattern to match to indicate EOV code */
    volatile AMBA_DBG_PORT_IDSP_MIPI_CTRL0_REG_s            MipiCtrl0;              /* 0x0078 */
    volatile AMBA_DBG_PORT_IDSP_MIPI_CTRL1_REG_s            MipiCtrl1;              /* 0x007C */
    volatile AMBA_DBG_PORT_IDSP_MIPI_CTRL2_REG_s            MipiCtrl2;              /* 0x0080 */
    volatile AMBA_DBG_PORT_IDSP_SLVSEC_CTRL0_REG_s          SlvsEcCtrl0;            /* 0x0084 */
    volatile AMBA_DBG_PORT_IDSP_VOUT_SYNC_REG_s             VoutSync[3];            /* 0x0088 -0x0090 */
    volatile AMBA_DBG_PORT_IDSP_STRIG_CTRL_REG_s            Strig0Ctrl;             /* 0x0094 */
    volatile UINT32                                         Strig0EndLine;          /* 0x0098 */
    volatile AMBA_DBG_PORT_IDSP_STRIG_CTRL_REG_s            Strig1Ctrl;             /* 0x009C */
    volatile UINT32                                         Strig1EndLine;          /* 0x00A0 */
    volatile UINT32                                         SyncDelayL;             /* 0x00A4: Delay in cycles after VSYNC before assertion of 'delayed vsync' signal to controller */
    volatile UINT32                                         SyncDelayH;             /* 0x00A8 */
    volatile UINT32                                         IntrDelayL;             /* 0x00AC: Delay in cycles after VSYNC before assertion of 'delayed vsync' interrupt to arm */
    volatile UINT32                                         IntrDelayH;             /* 0x00B0: */
    volatile AMBA_DBG_PORT_IDSP_BLACKLEVEL_CTRL_REG_s       BlackLevelCtrl;         /* 0x00B4: */
    volatile UINT32                                         BlackLevelOffset16LSB;  /* 0x00B8: */
    volatile AMBA_DBG_PORT_IDSP_STATUS_REG_s                Status;                 /* 0x00BC: */
    volatile AMBA_DBG_PORT_IDSP_SLVS_STATUS_REG_s           SlvsStatus;             /* 0x00C0: */
    volatile UINT32                                         DetectedActiveWidth;    /* 0x00C4: Detected Active Width */
    volatile UINT32                                         DetectedActiveHeight;   /* 0x00C8: Detected Active Height */
    volatile UINT32                                         SyncCode;               /* 0x00CC: Value of last received sync code (16-bit MSB aligned) */
    volatile UINT32                                         ActiveFrameHeight;      /* 0x00D0: Current active region height */
    volatile UINT32                                         ActiveFrameWidth;       /* 0x00D4: Current active region width */
    volatile UINT32                                         CropRegionHeight;       /* 0x00D8: Current crop region height */
    volatile UINT32                                         CropRegionWidth;        /* 0x00DC: Current crop region width */
    volatile AMBA_DBG_PORT_IDSP_MIPI_STATUS0_REG_s          MipiStatus0;            /* 0x00E0: */
    volatile UINT32                                         MipiLongPacketCount;    /* 0x00E4: Number of long packets received */
    volatile UINT32                                         MipiShortPacketCount;   /* 0x00E8: Number of short packets received */
    volatile UINT32                                         MipiLongPacketSize;     /* 0x00EC: Number of bytes in last received long packet */
    volatile UINT32                                         MipiNumCrcPhErrPacket;  /* 0x00F0: Number of Header with CRC errors */
    volatile UINT32                                         MipiNumCrcPkErrPacket;  /* 0x00F4: Number of Packet with CRC errors */
    volatile UINT32                                         MipiFrameNumber;        /* 0x00F8: Frame number (from short-packet/header info) */
    volatile UINT32                                         MipiNumCorrEccErrPacket;/* 0x00FC: Number of packets with correctable errors */
    volatile AMBA_DBG_PORT_IDSP_SLVSEC_ST0_REG_s            SlvsEcStatus0;          /* 0x0100: SlvsEc state & error status */
    volatile UINT32                                         SlvsEcLineNumber;       /* 0x0104: Line number in header packet */
    volatile UINT32                                         SlvsEcFrameNumber;      /* 0x0108: Frame number in header packet */
    volatile AMBA_DBG_PORT_IDSP_SLVSEC_ST1_REG_s            SlvsEcStatus1;          /* 0x010C: */
    volatile AMBA_DBG_PORT_IDSP_SLVSEC_ST2_REG_s            SlvsEcStatus2;          /* 0x0110: */
    volatile AMBA_DBG_PORT_IDSP_SLVSEC_ST3_REG_s            SlvsEcStatus3;          /* 0x0114: */
    volatile AMBA_DBG_PORT_IDSP_SLVSEC_ST4_REG_s            SlvsEcStatus4;          /* 0x0118: */
    volatile AMBA_DBG_PORT_IDSP_SLVSEC_ST5_REG_s            SlvsEcStatus5;          /* 0x011C: */
    volatile AMBA_DBG_PORT_IDSP_SLVSEC_ST6_REG_s            SlvsEcStatus6;          /* 0x0120: */
    volatile AMBA_DBG_PORT_IDSP_SLVSEC_ECC_ST_REG_s         SlvsEcLink0EccStatus;   /* 0x0124 */
    volatile UINT32                                         SlvsEcLink0EccDbg[3];   /* 0x0128-0x0130 */
    volatile AMBA_DBG_PORT_IDSP_SLVSEC_ECC_ST_REG_s         SlvsEcLink1EccStatus;   /* 0x0134 */
    volatile UINT32                                         SlvsEcLink1EccDbg[3];   /* 0x0138-0x0140 */
    volatile UINT32                                         Reserved2[47];          /* 0x0144-0x01FC */
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
