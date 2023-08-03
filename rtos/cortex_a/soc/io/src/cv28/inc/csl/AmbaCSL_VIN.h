/**
 *  @file AmbaCSL_VIN.h
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
 *  @details Definitions & Constants for VIN CSL APIs
 *
 */

#ifndef AMBA_CSL_VIN_H
#define AMBA_CSL_VIN_H

#ifndef AMBA_REG_RCT_H
#include "AmbaReg_RCT.h"
#endif

#ifndef AMBA_VIN_DEF_H
#include "AmbaVIN_Def.h"
#endif

#define AMBA_CSL_VIN_MAIN_CONFIG_SIZE           129U
#define AMBA_CSL_VIN_GLOBAL_CONFIG_SIZE         1U
#define AMBA_CSL_VIN_MASTER_SYNC_CONFIG_SIZE    9U

#define AMBA_VIN_NUM_DATA_LANE                  8U  /* maximum number of data lane for one VIN instance */

typedef struct {
    UINT32  SensorType:                 2;  /* [1:0] 0=Serial SLVS; 1=Parallel SLVS; 2=Parallel LVCMOS; 3=MIPI */
    UINT32  SlvsClkMode:                1;  /* [2] 0=All Lanes latched with spclk[0]; 1=Every 4 lanes latched by group/phy clock */
    UINT32  AFIFOBypass:                1;  /* [3] Bypass CDC/SLVS FIFOs; all PHY data is latched by slvs clk0 */
    UINT32  LvcmosEdge:                 1;  /* [4] 0=posedge; 1=negedge */
    UINT32  MipiVCHdr:                  2;  /* [6:5] 0: no virtual channel; 1: with virtual channel but no overlap (frame id is not alternate);
                                                     2/3: with virtual channel; and frame id alternates, it supports both overlap and nonoverlap */
    UINT32  Reserved:                   25; /* [15:7] */
} AMBA_CSL_VIN_GLOBAL_CONFIG_DATA_s;

typedef struct {
    struct {
        /*
         * Offset 0
         */
        UINT32  SwReset:                1;      /* [0] Reset core VIN logic when config is loaded */
        UINT32  Enable:                 1;      /* [1] Enable Sensor Interface logic */
        UINT32  OutputEnable:           1;      /* [2] Enable output of VIN */
        UINT32  BitsPerPixel:           3;      /* [5:3] Sensor data width; 0=8-bit; 1=10-bit; 2=12-bit; 3=14-bit; 4=16-bit */
        UINT32  Reserved:               10;     /* [15:6] */

        /*
         * Offset 1
         */
        UINT32  DataLaneEnable:         8;      /* [7:0] 1 = Enable SLVS/MIPI Physical Data Lane */
        UINT32  Reserved1:              8;      /* [15:8] */
    } Ctrl;

    struct {
        /*
         * Offset 2
         */
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

        /*
         * Offset 3
         */
        UINT32  FieldPinSelect:         5;      /* [4:0] 0:clk_n[0]; 1:clk_n[1]; 2:clk_p[1] */
        UINT32  VsyncPinSelect:         5;      /* [9:5] 0:clk_n[0]; 1:clk_n[1]; 2:clk_p[1] */
        UINT32  HsyncPinSelect:         5;      /* [14:10] 0:clk_n[0]; 1:clk_n[1]; 2:clk_p[1] */
        UINT32  Reserved:               1;      /* [15] */
    } DvpCtrl_SyncPinSelect;

    struct {
        /*
         * Offset 4
         */
        UINT32  DataLane0PinSelect:     4;      /* [3:0] pad_p[x] and pad_n[x] */
        UINT32  DataLane1PinSelect:     4;      /* [7:4] pad_p[x] and pad_n[x] */
        UINT32  DataLane2PinSelect:     4;      /* [11:8] pad_p[x] and pad_n[x] */
        UINT32  DataLane3PinSelect:     4;      /* [15:12] pad_p[x] and pad_n[x] */

        /*
         * Offset 5
         */
        UINT32  DataLane4PinSelect:     4;      /* [3:0] pad_p[x] and pad_n[x] */
        UINT32  DataLane5PinSelect:     4;      /* [7:4] pad_p[x] and pad_n[x] */
        UINT32  DataLane6PinSelect:     4;      /* [11:8] pad_p[x] and pad_n[x] */
        UINT32  DataLane7PinSelect:     4;      /* [15:12] pad_p[x] and pad_n[x] */
    } DataLanePinSelect;


    struct {
        UINT32  Reserved:               16;     /* Offset 6: dummy register so the register layout is the same as cv2 */


        /*
         * Offset 7
         */
        UINT32  DisableCropRegion:      1;      /* [0] Disable crop region; if it is 1, ignore the crop_start/end_col/row */
        UINT32  DbgNonBlocking:         1;      /* [1] Only set to 1 when bring up VIN to check sof/eof interrupts */
        UINT32  DbgEolHold:             1;      /* [2] reserved bit, only set to 1 when requested by designer */
        UINT32  MipiMaxSkewCycle:       3;      /* [5:3] Only set it to non-zero value in 8-lane MIPI sensor; its unit is mipi_byteclk cycle; or 4*mipi_sensor_clk cycle */
        UINT32  Reserved1:              10;     /* [15:6] */
    } Ctrl2;

    UINT16              ActiveRegionWidth;      /* Offset 8: Active Video Region Width Register. 0=auto-detect, else=active width - 1 */
    UINT16              ActiveRegionHeight;     /* Offset 9: Active Video Region Height Register. 0=auto-detect, else=active height - 1 */

    UINT16              SplitWidth;             /* Offset 10: For readouts that pack multiple exposure lines into a single readout line (0=disabled) */
    UINT16              CropStartCol;           /* Offset 11: Start column index of crop region */
    UINT16              CropStartRow;           /* Offset 12: Start row index of crop region */
    UINT16              CropEndCol;             /* Offset 13: End column index of crop region */


    struct {
        UINT32  CropEndRow:             16;    /* Offset 14: End row index of crop region */

        /*
         * Offset 15
         */
        UINT32  SyncInterleaving:       2;     /* [1:0] 0=none; 1=2-lane interleaving; 2=4-lane interleaving (For HiSPi Streaming-S) */
        UINT32  SyncTolerance:          2;     /* [3:2] 0=No error tolerance; 1=Allow a corrupted sync every other code; 2=Always allow corruption */
        UINT32  SyncAllLanes:           1;     /* [4] 1=Sync code must be detected on all active lanes in each group; 0=Majority of active lanes */
        UINT32  Reserved:               11;    /* [15:5] */
    } CropEndRow_SlvsSyncCodeLocation;

    struct {
        /*
         * Offset 16
         */
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
        UINT32  Reserved:               6;      /* [15:10] */

        /*
         * Offset 17
         */
        UINT32  DetectSol:              1;      /* [0] Enable Detection of SOL sync codes */
        UINT32  DetectEol:              1;      /* [1] Enable Detection of EOL sync codes */
        UINT32  DetectSof:              1;      /* [2] Enable Detection of SOF sync codes */
        UINT32  DetectEof:              1;      /* [3] Enable Detection of EOF sync codes */
        UINT32  DetectSov:              1;      /* [4] Enable Detection of SOV sync codes */
        UINT32  DetectEov:              1;      /* [5] Enable Detection of EOV sync codes */
        UINT32  Reserved1:              10;     /* [15:6] */
    } SlvsCtrl;

    struct {
        /*
         * Offset 18
         */
        UINT32  TimeOutL:               16;      /* Timeout value for sync detection (Bit[15:0]) */
        /*
         * Offset 19
         */
        UINT32  TimeOutH:               16;      /* Timeout value for sync detection (Bit[31:16]) */
    } SyncTimeOut;

    UINT16              SlvsSyncDetectMask;     /* Offset 20: End row index of crop region */
    UINT16              SlvsSyncDetectPattern;  /* Offset 21: */
    UINT16              SlvsSyncCompareMask;    /* Offset 22: Mask to patch sync code before comparing to desired patterns */
    UINT16              SlvsSolPattern;         /* Offset 23: Pattern to match to indicate SOL code */
    UINT16              SlvsEolPattern;         /* Offset 24: Pattern to match to indicate EOL code */
    UINT16              SlvsSofPattern;         /* Offset 25: Pattern to match to indicate SOF code */
    UINT16              SlvsEofPattern;         /* Offset 26: Pattern to match to indicate EOF code */
    UINT16              SlvsSovPattern;         /* Offset 27: Pattern to match to indicate SOV code */


    struct {
        UINT32  SlvsEovPattern:         16;     /* Offset 28: Pattern to match to indicate EOV code */

        /*
         * Offset 29
         */
        UINT32  VirtChanNumMinus1:      2;      /* [1:0] Number of MIPI virtual channel number minus 1 */
        UINT32  VirtChanMask:           2;      /* [3:2] MIPI virtual channel mask (Data captured when (~VC_MASK & virtual_channel) == VC_PAT */
        UINT32  FSVirtChanPattern:      2;      /* [5:4] MIPI virtual channel pattern to match for FS of the first exposure */
        UINT32  FEVirtChanPattern:      2;      /* [7:6] MIPI virtual channel pattern to match for FE of the last exposure */
        UINT32  Exp0VirtChanPattern:    2;      /* [9:8] MIPI virtual channel pattern to match for valid data of the first exposure */
        UINT32  Exp1VirtChanPattern:    2;      /* [11:10] MIPI virtual channel pattern to match for valid data of the second exposure */
        UINT32  Exp2VirtChanPattern:    2;      /* [13:12] MIPI virtual channel pattern to match for valid data of the third exposure */
        UINT32  Exp3VirtChanPattern:    2;      /* [15:14] MIPI virtual channel pattern to match for valid data of the fourth exposure */
    } SlvsEovPattern_MipiCtrl0;

    struct {
        /*
         * Offset 30
         */
        UINT32  DataTypeMask:           6;      /* [5:0] MIPI data type mask (Data captured when (~DT_MASK & data_id) == DT_PAT */
        UINT32  DataTypePattern:        6;      /* [11:6] MIPI data type pattern to match for valid data */
        UINT32  ByteSwapEnable:         1;      /* [12] 1 = Reverse order of pixels */
        UINT32  EccEnable:              1;      /* [13] 1 = Enable packet header ECC */
        UINT32  ForwardEccEnable:       1;      /* [14] 1 = Enable errored packets forwarding (default mode) */
        UINT32  MipiCropEnable:         1;      /* [15] 1 = Enable MIPI crop (used in multiple VC mode) */

        UINT32  MipiExp1Offset:         16;     /* Offset 31: The second expousre offset compared to the first exposure */

    } MipiCtrl1_MipiExp1Offset;

    UINT16              MipiExp2Offset;         /* Offset 32: The thrid expousre offset compared to the first exposure */
    UINT16              MipiExp3Offset;         /* Offset 33: The fourth expousre offset compared to the first exposure */

    UINT16              MipiCropRowStart;       /* Offset 34: Crop Row start (used in VC HDR mode) */
    UINT16              MipiCropColStart;       /* Offset 35: Crop column start (used in HDR VC mode) */
    UINT16              MipiCropWidthMinus1;    /* Offset 36: Crop width minus 1 (used in VC HDR mode) */
    UINT16              MipiCropHeightMinus1;   /* Offset 37: Crop height minus 1 (used in VC HDR mode) */

    struct {
        /*
         * Offset 38, 39
         */
        UINT32  Sync0Mode:                   2;      /* [1:0] 0=Disabled; 1=Even Fields; 2=Odd Fields; 3=All Fields */
        UINT32  Sync0Line:                   14;     /* [15:2] VOUT sync0 is asserted for the duration of the specified active line */

        UINT32  Sync1Mode:                   2;      /* [1:0] 0=Disabled; 1=Even Fields; 2=Odd Fields; 3=All Fields */
        UINT32  Sync1Line:                   14;     /* [15:2] VOUT sync1 is asserted for the duration of the specified active line */

    } VoutSync;

    struct {
        /*
         * Offset 40
         */
        UINT32  Enable:                 1;      /* [0] Enable GPIO strig0 */
        UINT32  Polarity:               1;      /* [1] 0=Active low; 1=Active high */
        UINT32  StartLine:              14;     /* [15:2] Active line number to start asserting strig0 */

        UINT32  Strig0EndLine:          16;     /* Offset 41 */

    } Strig0Ctrl;


    struct {
        /*
         * Offset 42
         */
        UINT32  Enable:                 1;      /* [0] Enable GPIO strig0 */
        UINT32  Polarity:               1;      /* [1] 0=Active low; 1=Active high */
        UINT32  StartLine:              14;     /* [15:2] Active line number to start asserting strig0 */
        UINT32  Strig1EndLine:          16;     /* Offset 43: N/A for B5 */

    } Strig1Ctrl;

    UINT16              SyncDelayL;             /* Offset 44: Delay in cycles after VSYNC before assertion of 'delayed vsync' signal to controller */
    UINT16              SyncDelayH;             /* Offset 45: */
    UINT16              IntrDelayL;             /* Offset 46: Delay in cycles after VSYNC before assertion of 'delayed vsync' interrupt to arm */
    UINT16              IntrDelayH;             /* Offset 47: */

    struct {
        /*
         * Offset 48
         */
        UINT32  Sign:                  1;      /* [0] sign bit of black level offset */
        UINT32  Reserved:              15;     /* [15:1] */

        UINT32  Offset16LSB:           16;     /* Offset 49: black level offset (16 least significant bits) */

    } BlackLevelOffsetCtrl;


    struct {
        /*
         * Offset 50
         */
        UINT32  GotActSof:              1;      /* [0] Detected start of active region (write 1 to clear) */
        UINT32  Strig0Status:           1;      /* [1] not used any more */
        UINT32  Strig1Status:           1;      /* [2] not used any more */
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
        UINT32  Reserved:               1;      /* [15] */

        /*
         * Offset 51
         */
        UINT32  SyncLocked:             1;      /* [0] VIN front-end sync is locked */
        UINT32  LostLockAfterSof:       1;      /* [1] Lost lock after detecting start of frame (must reset VIN) */
        UINT32  PartialSyncDetected:    1;      /* [2] Partial/corrupted sync was detected (write 1 to clear) */
        UINT32  UnknownSyncCode:        1;      /* [3] Sync code did not match any patterns (write 1 to clear) */
        UINT32  GotWinSof:              1;      /* [4] Reached start of crop region (write 1 to clear) */
        UINT32  AfifoOverflow:          1;      /* [5] AFIFO overflow (must reset VIN) */
        UINT32  SyncState:              2;      /* [7:6] State of front-end sync FSM (0=IDLE; 1=ACTIVE; 2=HBLANK; 3=VBLANK) */
        UINT32  Reserved1:              8;      /* [15:8] */
    } Status;

    UINT16              SfifoCount;             /* Offset 52: Number of entries in SFIFO/Buffer */
    UINT16              DetectedActiveWidth;    /* Offset 53: Detected Active Width */
    UINT16              DetectedActiveHeight;   /* Offset 54: Detected Active Height */
    UINT16              SyncCode;               /* Offset 55: Value of last received sync code (16-bit MSB aligned) */
    UINT16              ActiveFrameHeight;      /* Offset 56: Current active region height */
    UINT16              ActiveFrameWidth;       /* Offset 57: Current active region width */
    UINT16              CropRegionHeight;       /* Offset 58: Current crop region height */
    UINT16              CropRegionWidth;        /* Offset 59: Current crop region width */

    struct {
        /*
         * Offset 60
         */
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
        UINT32  Reserved2:              3;      /* [15:13] */

        UINT32  MipiLongPacketCount:    16;     /* Offset 61: Number of long packets received */

    } MipiStatus0;

    UINT16              MipiShortPacketCount;   /* Offset 62: Number of short packets received */
    UINT16              MipiLongPacketSize;     /* Offset 63: Number of bytes in last received long packet */
    UINT16              MipiNumErrorFrames;     /* Offset 64: Number of frames with CRC errors */
    UINT16              MipiFrameNumber;        /* Offset 65: Frame number (from short-packet/header info) */

    struct {
        /*
         * Offset 66
         */
        UINT32  FrameID:                1;      /* [0] Frame ID of the data, only meaningful when gcfg.mipi_hdr_frame_ovlp is 1 */
        UINT32  VirtChanID:             2;      /* [2:1] Virtual channel ID */
        UINT32  Frame0VirtChanDone:     3;      /* [5:3] Frame done signal of each VC when frame ID is 0 */
        UINT32  Frame1VirtChanDone:     3;      /* [8:6] Frame done signal of each VC when frame ID is 1 */
        UINT32  MipiCropShortLine:      1;      /* [9] Short line of the crop module inside MIPI */
        UINT32  MipiCropShortFrame:     1;      /* [10] Short frame of the crop module inside MIPI */
        UINT32  Reserved:               5;      /* [15:11] */

        UINT32  MipiFrame0Exp0RowCount: 16;     /* Offset 67: The first exposure row count of frame 0 */

    } MipiStatus1_Frame0Exp0RowCount;

    UINT16              MipiFrame0Exp1RowCount; /* Offset 68: The second exposure row count of frame 0 */
    UINT16              MipiFrame0Exp2RowCount; /* Offset 69: The third exposure row count of frame 0 */
    UINT16              MipiFrame0Exp3RowCount; /* Offset 70: The fourth exposure row count of frame 0 */
    UINT16              MipiFrame1Exp0RowCount; /* Offset 71: The first exposure row count of frame 1 */
    UINT16              MipiFrame1Exp1RowCount; /* Offset 72: The second exposure row count of frame 1 */
    UINT16              MipiFrame1Exp2RowCount; /* Offset 73: The third exposure row count of frame 1 */
    UINT16              MipiFrame1Exp3RowCount; /* Offset 74: The fourth exposure row count of frame 1 */

    UINT16              Reserved[53] ;          /* Offset 75 - 127 */

    struct {
        /*
         * Offset 128
         */
        UINT32  DebugCommandShift:      1;      /* [0] To manually shift VIN programming via debug bus */
        UINT32  FrameID:                1;      /* [1] Frame ID sent vid debug bus when there is no config_done */
        UINT32  Reserved:               30;     /* [15:2] */
    } ConfigDone;

} AMBA_CSL_VIN_CONFIG_DATA_s;

typedef struct {
    UINT16              HSyncPeriodL;           /* Offset 0: HSYNC period in cycles(low bits) */
    UINT16              HSyncPeriodH;           /* Offset 1: HSYNC period in cycles(high bits) */
    UINT16              HSyncWidth;             /* Offset 2: Width of HSYNC pulse in cycles */
    UINT16              HSyncOffset;            /* Offset 3: Offset of HSYNC pulse in cycles */
    UINT16              VSyncPeriodL;           /* Offset 4: Number of HSYNCs between VSYNCs (low bits) */
    UINT16              VSyncPeriodH;           /* Offset 5: Number of HSYNCs between VSYNCs (high bits) */
    UINT16              VSyncWidth;             /* Offset 6: Width of VSYNC pulse in cycles OR HSYNCs (see VSYNC_WIDTH_UNIT) */
    UINT16              VSyncOffset;            /* Offset 7: Offset between HSYNC and VSYNC edges (in cycles); VSYNC trails HSYNC */

    struct {
        /*
         * Offset 8
         */
        UINT32  HsyncPolarity:          1;      /* [0] 0=Active Low; 1=Active High */
        UINT32  VsyncPolarity:          1;      /* [1] 0=Active Low; 1=Active High */
        UINT32  NoVblankHsync:          1;      /* [2] Don't toggle HSYNC during VBLANK */
        UINT32  InterruptMode:          1;      /* [3] VSYNC interrupt at 0=End of VBLANK period; 1=End of VBLANK Pulse */
        UINT32  VSyncWidthUnit:         1;      /* [4] VSYNC_WIDTH is measured in: 0=cycles; 1=HSYNCS */
        UINT32  NumVsyncs:              8;      /* [12:5] Number of VSYNC cycles to send before loading next command (if present); 0=Turn off sync */
        UINT32  Continuous:             1;      /* [13] If no new command is available after sending NUM_VSYNCS: 0=Stop; 1=Continue until new command is received */
        UINT32  Preempt:                1;      /* [14] Load this command on the next VSYNC even if the current command hasn't finished */
        UINT32  Reserved:               17;      /* [15] */
    } MasterSyncCtrl;

} AMBA_CSL_VIN_MASTER_SYNC_CONFIG_DATA_s;

typedef struct {
    UINT16  OffsetX;                    /* Horizontal offset of the window */
    UINT16  OffsetY;                    /* Vertical offset of the window */
    UINT16  Width;                      /* Number of pixels per line in the window */
    UINT16  Height;                     /* Number of lines in the window */
} AMBA_CSL_VIN_WINDOW_s;

typedef struct {
    AMBA_VIN_FRAME_RATE_s   FrameRate;
    UINT8                   ColorSpace;     /* Color space of the input data. 0U: RGB, 1U: YUVs, 2U: RGB-IR, 3U: RCCC */
    UINT8                   BayerPattern;   /* Color coding of the first 2x2 pixels */
    UINT8                   YuvOrder;       /* Pixel order */
    UINT32                  NumDataBits;    /* Bit resolution of the input pixel data */
    UINT8                   NumSkipFrame;   /* Number of frames to be skipped if VIN configuration is changed */
} AMBA_CSL_VIN_INFO_s;

/*
 * Macro Definitions
 */
static inline void AmbaCSL_VinSetLvdsBitMode0(UINT32 d)
{
    pAmbaMIPI_Reg->Dphy0Ctrl0.Bits.BitMode0 = (UINT8)d;
}
static inline void AmbaCSL_VinSetLvdsBitMode1(UINT32 d)
{
    pAmbaMIPI_Reg->Dphy0Ctrl0.Bits.BitMode1 = (UINT8)d;
}
static inline void AmbaCSL_VinSetLvdsBitMode2(UINT32 d)
{
    pAmbaMIPI_Reg->Dphy0Ctrl0.Bits.BitMode2 = (UINT8)d;
}
static inline void AmbaCSL_VinSetLvdsRsel(UINT32 d)
{
    pAmbaMIPI_Reg->Dphy0Ctrl0.Bits.LvdsResistanceSelect = (UINT8)d;
}

static inline UINT32 AmbaCSL_VinGetLvdsBitMode0(void)
{
    return pAmbaMIPI_Reg->Dphy0Ctrl0.Bits.BitMode0;
}
static inline UINT32 AmbaCSL_VinGetLvdsBitMode1(void)
{
    return pAmbaMIPI_Reg->Dphy0Ctrl0.Bits.BitMode1;
}
static inline UINT32 AmbaCSL_VinGetLvdsBitMode2(void)
{
    return pAmbaMIPI_Reg->Dphy0Ctrl0.Bits.BitMode2;
}
static inline UINT32 AmbaCSL_VinGetLvdsLvdsRsel(void)
{
    return pAmbaMIPI_Reg->Dphy0Ctrl0.Bits.LvdsResistanceSelect;
}

static inline void AmbaCSL_VinSetLvCmosMode(UINT32 d)
{
    pAmbaMIPI_Reg->Dphy0Ctrl3.Bits.LvCmosMode = (UINT8)d;
}
static inline void AmbaCSL_VinSetLvCmosModeSpclk(UINT32 d)
{
    pAmbaMIPI_Reg->Dphy0Ctrl3.Bits.LvCmosSpClk = (UINT8)d;
}
static inline void AmbaCSL_VinSetMipiMode(UINT32 d)
{
    pAmbaMIPI_Reg->Dphy0Ctrl3.Bits.MipiMode = (UINT8)d;
}

static inline UINT32 AmbaCSL_VinGetLvCmosMode(void)
{
    return pAmbaMIPI_Reg->Dphy0Ctrl3.Bits.LvCmosMode;
}
static inline UINT32 AmbaCSL_VinGetLvCmosModeSpclk(void)
{
    return pAmbaMIPI_Reg->Dphy0Ctrl3.Bits.LvCmosSpClk;
}
static inline UINT32 AmbaCSL_VinGetMipiMode(void)
{
    return pAmbaMIPI_Reg->Dphy0Ctrl3.Bits.MipiMode;
}

static inline void AmbaCSL_VinSetLvdsIbCtrl(UINT32 d)
{
    pAmbaMIPI_Reg->Dphy0Ctrl1.Bits.LvdsIbCtrl = (UINT8)d;
}
static inline void AmbaCSL_RctSetLvdsCtrlPowerDown(UINT32 d)
{
    pAmbaMIPI_Reg->Dphy0Ctrl1.Bits.PowerDownLvds = (UINT8)d;
}
static inline void AmbaCSL_VinSetDphyRstAfe(UINT32 d)
{
    pAmbaMIPI_Reg->Dphy0Ctrl0.Bits.RstDphyAfe = (UINT8)d;
}
static inline void AmbaCSL_VinSetDphyRst0(UINT32 d)
{
    pAmbaMIPI_Reg->Dphy0Ctrl0.Bits.DphyRst0 = (UINT8)d;
}
static inline void AmbaCSL_VinSetDphyRst1(UINT32 d)
{
    pAmbaMIPI_Reg->Dphy0Ctrl0.Bits.DphyRst1 = (UINT8)d;
}
static inline void AmbaCSL_VinSetDphyRst2(UINT32 d)
{
    pAmbaMIPI_Reg->Dphy0Ctrl0.Bits.DphyRst2 = (UINT8)d;
}

static inline UINT32 AmbaCSL_VinGetLvdsIbCtrl(void)
{
    return pAmbaMIPI_Reg->Dphy0Ctrl1.Bits.LvdsIbCtrl;
}
static inline UINT32 AmbaCSL_VinGetDphyRstAfe(void)
{
    return pAmbaMIPI_Reg->Dphy0Ctrl0.Bits.RstDphyAfe;
}
static inline UINT32 AmbaCSL_VinGetDphyRst0(void)
{
    return pAmbaMIPI_Reg->Dphy0Ctrl0.Bits.DphyRst0;
}
static inline UINT32 AmbaCSL_VinGetDphyRst1(void)
{
    return pAmbaMIPI_Reg->Dphy0Ctrl0.Bits.DphyRst1;
}
static inline UINT32 AmbaCSL_VinGetDphyRst2(void)
{
    return pAmbaMIPI_Reg->Dphy0Ctrl0.Bits.DphyRst2;
}

static inline void AmbaCSL_VinSetMipiCtrl0(UINT32 d)
{
    pAmbaMIPI_Reg->Dphy0Ctrl0.Data = d;
}
static inline UINT32 AmbaCSL_VinGetMipiCtrl0(void)
{
    return pAmbaMIPI_Reg->Dphy0Ctrl0.Data;
}

static inline void AmbaCSL_VinSetMipiCtrl1(UINT32 d)
{
    pAmbaMIPI_Reg->Dphy0Ctrl1.Data = d;
}
static inline UINT32 AmbaCSL_VinGetMipiCtrl1(void)
{
    return pAmbaMIPI_Reg->Dphy0Ctrl1.Data;
}

static inline void AmbaCSL_VinSetVinMipiHsSettleTime(UINT32 d)
{
    pAmbaMIPI_Reg->Dphy0Ctrl4.Bits.HsSettleCtrl0 = (UINT8)d;
}
static inline void AmbaCSL_VinSetVinMipiHsTermTime(UINT32 d)
{
    pAmbaMIPI_Reg->Dphy0Ctrl4.Bits.HsTermCtrl0 = (UINT8)d;
}
static inline void AmbaCSL_VinSetVinMipiClkSettleTime(UINT32 d)
{
    pAmbaMIPI_Reg->Dphy0Ctrl5.Bits.ClkSettleCtrl0 = (UINT8)d;
}
static inline void AmbaCSL_VinSetVinMipiClkTermTime(UINT32 d)
{
    pAmbaMIPI_Reg->Dphy0Ctrl5.Bits.ClkTermCtrl0 = (UINT8)d;
}
static inline void AmbaCSL_VinSetVinMipiClkMissTime(UINT32 d)
{
    pAmbaMIPI_Reg->Dphy0Ctrl4.Bits.ClkMissCtrl0 = (UINT8)d;
}
static inline void AmbaCSL_VinSetVinMipiRxInitTime(UINT32 d)
{
    pAmbaMIPI_Reg->Dphy0Ctrl4.Bits.InitRxCtrl0 = (UINT8)d;
}
static inline void AmbaCSL_VinSetVinMipiForceClkHs(UINT32 d)
{
    pAmbaMIPI_Reg->Dphy0Ctrl2.Bits.ForceClkHs0 = (UINT8)d;
}

static inline UINT32 AmbaCSL_VinGetVinMipiHsSettleTime(void)
{
    return pAmbaMIPI_Reg->Dphy0Ctrl4.Bits.HsSettleCtrl0;
}
static inline UINT32 AmbaCSL_VinGetVinMipiHsTermTime(void)
{
    return pAmbaMIPI_Reg->Dphy0Ctrl4.Bits.HsTermCtrl0;
}
static inline UINT32 AmbaCSL_VinGetVinMipiClkSettleTime(void)
{
    return pAmbaMIPI_Reg->Dphy0Ctrl5.Bits.ClkSettleCtrl0;
}
static inline UINT32 AmbaCSL_VinGetVinMipiClkTermTime(void)
{
    return pAmbaMIPI_Reg->Dphy0Ctrl5.Bits.ClkTermCtrl0;
}
static inline UINT32 AmbaCSL_VinGetVinMipiClkMissTime(void)
{
    return pAmbaMIPI_Reg->Dphy0Ctrl4.Bits.ClkMissCtrl0;
}
static inline UINT32 AmbaCSL_VinGetVinMipiRxInitTime(void)
{
    return pAmbaMIPI_Reg->Dphy0Ctrl4.Bits.InitRxCtrl0;
}
static inline UINT32 AmbaCSL_VinGetVinMipiForceClkHs(void)
{
    return pAmbaMIPI_Reg->Dphy0Ctrl2.Bits.ForceClkHs0;
}
static inline UINT32 AmbaCSL_VinGetVinMipiRxClkActiveHs(void)
{
    return pAmbaMIPI_Reg->Dphy0Obsv0.Bits.RxClkActiveHS0;
}

static inline void AmbaCSL_VinSetPipMipiHsSettleTime(UINT32 d)
{
    pAmbaMIPI_Reg->Dphy0Ctrl7.Bits.HsSettleCtrl1 = (UINT8)d;
}
static inline void AmbaCSL_VinSetPipMipiHsTermTime(UINT32 d)
{
    pAmbaMIPI_Reg->Dphy0Ctrl7.Bits.HsTermCtrl1 = (UINT8)d;
}
static inline void AmbaCSL_VinSetPipMipiClkSettleTime(UINT32 d)
{
    pAmbaMIPI_Reg->Dphy0Ctrl8.Bits.ClkSettleCtrl1 = (UINT8)d;
}
static inline void AmbaCSL_VinSetPipMipiClkTermTime(UINT32 d)
{
    pAmbaMIPI_Reg->Dphy0Ctrl8.Bits.ClkTermCtrl1 = (UINT8)d;
}
static inline void AmbaCSL_VinSetPipMipiClkMissTime(UINT32 d)
{
    pAmbaMIPI_Reg->Dphy0Ctrl7.Bits.ClkMissCtrl1 = (UINT8)d;
}
static inline void AmbaCSL_VinSetPipMipiRxInitTime(UINT32 d)
{
    pAmbaMIPI_Reg->Dphy0Ctrl7.Bits.InitRxCtrl1 = (UINT8)d;
}
static inline void AmbaCSL_VinSetPipMipiForceClkHs(UINT32 d)
{
    pAmbaMIPI_Reg->Dphy0Ctrl2.Bits.ForceClkHs1 = (UINT8)d;
}

static inline UINT32 AmbaCSL_VinGetPipMipiHsSettleTime(void)
{
    return pAmbaMIPI_Reg->Dphy0Ctrl7.Bits.HsSettleCtrl1;
}
static inline UINT32 AmbaCSL_VinGetPipMipiHsTermTime(void)
{
    return pAmbaMIPI_Reg->Dphy0Ctrl7.Bits.HsTermCtrl1;
}
static inline UINT32 AmbaCSL_VinGetPipMipiClkSettleTime(void)
{
    return pAmbaMIPI_Reg->Dphy0Ctrl8.Bits.ClkSettleCtrl1;
}
static inline UINT32 AmbaCSL_VinGetPipMipiClkTermTime(void)
{
    return pAmbaMIPI_Reg->Dphy0Ctrl8.Bits.ClkTermCtrl1;
}
static inline UINT32 AmbaCSL_VinGetPipMipiClkMissTime(void)
{
    return pAmbaMIPI_Reg->Dphy0Ctrl7.Bits.ClkMissCtrl1;
}
static inline UINT32 AmbaCSL_VinGetPipMipiRxInitTime(void)
{
    return pAmbaMIPI_Reg->Dphy0Ctrl7.Bits.InitRxCtrl1;
}
static inline UINT32 AmbaCSL_VinGetPipMipiForceClkHs(void)
{
    return pAmbaMIPI_Reg->Dphy0Ctrl2.Bits.ForceClkHs1;
}
static inline UINT32 AmbaCSL_VinGetPipMipiRxClkActiveHs(void)
{
    return pAmbaMIPI_Reg->Dphy0Obsv0.Bits.RxClkActiveHS1;
}

static inline void AmbaCSL_VinSetPip2MipiHsSettleTime(UINT32 d)
{
    pAmbaMIPI_Reg->Dphy0Ctrl9.Bits.HsSettleCtrl2 = (UINT8)d;
}
static inline void AmbaCSL_VinSetPip2MipiHsTermTime(UINT32 d)
{
    pAmbaMIPI_Reg->Dphy0Ctrl9.Bits.HsTermCtrl2 = (UINT8)d;
}
static inline void AmbaCSL_VinSetPip2MipiClkSettleTime(UINT32 d)
{
    pAmbaMIPI_Reg->Dphy0Ctrl10.Bits.ClkSettleCtrl2 = (UINT8)d;
}
static inline void AmbaCSL_VinSetPip2MipiClkTermTime(UINT32 d)
{
    pAmbaMIPI_Reg->Dphy0Ctrl10.Bits.ClkTermCtrl2 = (UINT8)d;
}
static inline void AmbaCSL_VinSetPip2MipiClkMissTime(UINT32 d)
{
    pAmbaMIPI_Reg->Dphy0Ctrl9.Bits.ClkMissCtrl2 = (UINT8)d;
}
static inline void AmbaCSL_VinSetPip2MipiRxInitTime(UINT32 d)
{
    pAmbaMIPI_Reg->Dphy0Ctrl9.Bits.InitRxCtrl2 = (UINT8)d;
}
static inline void AmbaCSL_VinSetPip2MipiForceClkHs(UINT32 d)
{
    pAmbaMIPI_Reg->Dphy0Ctrl2.Bits.ForceClkHs2 = (UINT8)d;
}

static inline UINT32 AmbaCSL_VinGetPip2MipiHsSettleTime(void)
{
    return pAmbaMIPI_Reg->Dphy0Ctrl9.Bits.HsSettleCtrl2;
}
static inline UINT32 AmbaCSL_VinGetPip2MipiHsTermTime(void)
{
    return pAmbaMIPI_Reg->Dphy0Ctrl9.Bits.HsTermCtrl2;
}
static inline UINT32 AmbaCSL_VinGetPip2MipiClkSettleTime(void)
{
    return pAmbaMIPI_Reg->Dphy0Ctrl10.Bits.ClkSettleCtrl2;
}
static inline UINT32 AmbaCSL_VinGetPip2MipiClkTermTime(void)
{
    return pAmbaMIPI_Reg->Dphy0Ctrl10.Bits.ClkTermCtrl2;
}
static inline UINT32 AmbaCSL_VinGetPip2MipiClkMissTime(void)
{
    return pAmbaMIPI_Reg->Dphy0Ctrl9.Bits.ClkMissCtrl2;
}
static inline UINT32 AmbaCSL_VinGetPip2MipiRxInitTime(void)
{
    return pAmbaMIPI_Reg->Dphy0Ctrl9.Bits.InitRxCtrl2;
}
static inline UINT32 AmbaCSL_VinGetPip2MipiForceClkHs(void)
{
    return pAmbaMIPI_Reg->Dphy0Ctrl2.Bits.ForceClkHs2;
}
static inline UINT32 AmbaCSL_VinGetPip2MipiRxClkActiveHs(void)
{
    return pAmbaMIPI_Reg->Dphy0Obsv0.Bits.RxClkActiveHS2;
}

/* vin reference clock select */
static inline void AmbaCSL_VinSetRefClkSel(UINT32 d)
{
    pAmbaRCT_Reg->VinClkSelect.VinRefClkSelect = (UINT8)d;
}

/*
 * Defined in AmbaCSL_VIN.c
 */

UINT32 AmbaCSL_VinSetVinLvdsPadMode(UINT32 PadMode, UINT32 DataLaneEnable);
UINT32 AmbaCSL_VinSetPipLvdsPadMode(UINT32 PadMode, UINT32 DataLaneEnable);
UINT32 AmbaCSL_VinSetPip2LvdsPadMode(UINT32 PadMode, UINT32 DataLaneEnable);
UINT32 AmbaCSL_VinSlvsConfigGlobal(UINT32 VinID);
UINT32 AmbaCSL_VinMipiConfigGlobal(UINT32 VinID, UINT32 VirtChanHDREnable);
UINT32 AmbaCSL_VinDvpConfigGlobal(UINT32 VinID);
UINT32 AmbaCSL_VinSlvsConfigMain(UINT32 VinID, const AMBA_VIN_SLVS_CONFIG_s *pVinSlvsConfig);
UINT32 AmbaCSL_VinMipiConfigMain(UINT32 VinID, const AMBA_VIN_MIPI_CONFIG_s *pVinMipiConfig);
UINT32 AmbaCSL_VinDvpConfigMain(UINT32 VinID, const AMBA_VIN_DVP_CONFIG_s *pVinDvpConfig);
UINT32 AmbaCSL_VinMasterSyncConfig(UINT32 MSyncID, const AMBA_VIN_MASTER_SYNC_CONFIG_s *pMSyncConfig);
UINT32 AmbaCSL_VinDataLaneRemap(UINT32 VinID, const AMBA_VIN_LANE_REMAP_CONFIG_s *pLaneRemapConfig);
UINT32 AmbaCSL_VinMipiVirtChanConfig(UINT32 VinID, const AMBA_VIN_MIPI_VC_CONFIG_s *pMipiVirtChanConfig);
UINT32 AmbaCSL_VinConfigMainThrDbgPort(UINT32 VinID, const ULONG MainCfgBufAddr, UINT32 ToggleFrameID);

UINT32 AmbaCSL_VinCaptureConfig(UINT32 VinID, const AMBA_CSL_VIN_WINDOW_s *pCaptureWindow);
UINT32 AmbaCSL_VinGetMainCfgBufAddr(UINT32 VinID, ULONG *pAddr);
UINT32 AmbaCSL_VinGetGlobalCfgBufAddr(UINT32 VinID, ULONG *pAddr); /* will be removed on CV22 */
UINT32 AmbaCSL_VinGetMSyncCfgBufAddr(UINT32 MSyncID, ULONG *pAddr); /* will be removed on CV22 */
UINT32 AmbaCSL_VinGetInfo(UINT32 VinID, AMBA_CSL_VIN_INFO_s *pInfo);
UINT32 AmbaCSL_VinIsEnabled(UINT32 VinID);
void AmbaCSL_VinSetSensorClkDriveStr(UINT32 Value);

#endif /* AMBA_CSL_VIN_H */
