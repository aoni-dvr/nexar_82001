/**
 *  @file AmbaCSL_VIN.h
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
 *  @details Definitions & Constants for VIN CSL APIs
 *
 */

#ifndef AMBA_CSL_VIN_H
#define AMBA_CSL_VIN_H

#ifndef AMBA_REG_RCT_H
#include "AmbaReg_RCT.h"
#endif

#ifndef AMBA_REG_CPHY_H
#include "AmbaReg_DCPHY.h"
#endif

#ifndef AMBA_VIN_DEF_H
#include "AmbaVIN_Def.h"
#endif


#define AMBA_CSL_VIN_MAIN_CONFIG_SIZE           129U
#define AMBA_CSL_VIN_GLOBAL_CONFIG_SIZE         1U
#define AMBA_CSL_VIN_MASTER_SYNC_CONFIG_SIZE    9U

#define AMBA_VIN_NUM_DATA_LANE                  8U   /* maximum number of data lane for one VIN instance (excluding SLVS-EC) */
#define AMBA_VIN_NUM_SLVSEC_LANE                12U

typedef struct {
    UINT32  SensorType:                 3;      /* [2:0] 0=Serial SLVS; 1=Parallel SLVS; 2=Parallel LVCMOS; 3=MIPI D-PHY; 5=SLVS-EC; 6=MIPI C-PHY */
    UINT32  SlvsClkMode:                1;      /* [3] 0=All Lanes latched with spclk[0]; 1=Every 4 lanes latched by group/phy clock */
    UINT32  AFIFOBypass:                1;      /* [4] Bypass CDC/SLVS FIFOs; all PHY data is latched by slvs clk0 */
    UINT32  SlvsEcClkMode:              1;      /* [5] */
    UINT32  SlvsEcPhyReset:             1;      /* [6] */
    UINT32  SlvsEcPhySetup:             1;      /* [7] */
    UINT32  SlvsEcPhyStandby:           1;      /* [8] */
    UINT32  SlvsEcPhyConfig:            1;      /* [9] */
    UINT32  SlvsEcPhyPulseDuration:     5;      /* [14:10] */
    UINT32  SlvsEcPhyOverride:          1;      /* [15] */
    UINT32  Reserved:                   16;     /* Unused */
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
        UINT32  DataLaneEnable:         16;     /* [15:0] 1 = Enable SLVS/MIPI Physical Data Lane */
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
    } DataPinMux0_1;

    struct {
        /*
         * Offset 6
         */
        UINT32  DataLane8PinSelect:     4;      /* [3:0] pad_p[x] and pad_n[x] */
        UINT32  DataLane9PinSelect:     4;      /* [7:4] pad_p[x] and pad_n[x] */
        UINT32  DataLane10PinSelect:    4;      /* [11:8] pad_p[x] and pad_n[x] */
        UINT32  DataLane11PinSelect:    4;      /* [15:12] pad_p[x] and pad_n[x] */

        /*
         * Offset 7
         */
        UINT32  DataLane12PinSelect:    4;      /* [3:0] pad_p[x] and pad_n[x] */
        UINT32  DataLane13PinSelect:    4;      /* [7:4] pad_p[x] and pad_n[x] */
        UINT32  DataLane14PinSelect:    4;      /* [11:8] pad_p[x] and pad_n[x] */
        UINT32  DataLane15PinSelect:    4;      /* [15:12] pad_p[x] and pad_n[x] */
    } DataPinMux2_3;

    struct {
        /*
         * Offset 8
         */
        UINT32  DisableCropRegion:      1;      /* [0] Disable crop region; if it is 1, ignore the crop_start/end_col/row */
        UINT32  DbgNonBlocking:         1;      /* [1] Only set to 1 when bring up VIN to check sof/eof interrupts */
        UINT32  DbgEolHold:             1;      /* [2] reserved bit, only set to 1 when requested by designer */
        UINT32  MipiMaxSkewCycle:       3;      /* [5:3] Only set it to non-zero value in 8-lane MIPI sensor; its unit is mipi_byteclk cycle; or 4*mipi_sensor_clk cycle */
        UINT32  Reserved1:              10;     /* [15:6] */

        UINT32  ActiveRegionWidth:      16;     /* Offset 9: Active Video Region Width Register. 0=auto-detect, else=active width - 1 */
    } Ctrl2_ActiveRegionWidth;

    UINT16              ActiveRegionHeight;     /* Offset 10: Active Video Region Height Register. 0=auto-detect, else=active height - 1 */
    UINT16              SplitWidth;             /* Offset 11: For readouts that pack multiple exposure lines into a single readout line (0=disabled) */
    UINT16              CropStartCol;           /* Offset 12: Start column index of crop region */
    UINT16              CropStartRow;           /* Offset 13: Start row index of crop region */
    UINT16              CropEndCol;             /* Offset 14: End column index of crop region */
    UINT16              CropEndRow;             /* Offset 15: End row index of crop region */

    struct {
        /*
         * Offset 16
         */
        UINT32  SyncInterleaving:       2;      /* [1:0] 0=none; 1=2-lane interleaving; 2=4-lane interleaving (For HiSPi Streaming-S) */
        UINT32  SyncTolerance:          2;      /* [3:2] 0=No error tolerance; 1=Allow a corrupted sync every other code; 2=Always allow corruption */
        UINT32  SyncAllLanes:           1;      /* [4] 1=Sync code must be detected on all active lanes in each group; 0=Majority of active lanes */
        UINT32  Reserved:               11;     /* [15:5] */

        /*
         * Offset 17
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
        UINT32  Reserved1:              6;      /* [15:10] */
    } SlvsCtrl;

    struct {
        /*
         * Offset 18
         */
        UINT32  DetectSol:              1;      /* [0] Enable Detection of SOL sync codes */
        UINT32  DetectEol:              1;      /* [1] Enable Detection of EOL sync codes */
        UINT32  DetectSof:              1;      /* [2] Enable Detection of SOF sync codes */
        UINT32  DetectEof:              1;      /* [3] Enable Detection of EOF sync codes */
        UINT32  DetectSov:              1;      /* [4] Enable Detection of SOV sync codes */
        UINT32  DetectEov:              1;      /* [5] Enable Detection of EOV sync codes */
        UINT32  Reserved1:              10;     /* [15:6] */

        UINT32  WatchdogTimeoutL:       16;     /* Offset 19: Watchdog timeout value (Bit[15:0]) in unit of idsp cycles, 0=disabled */
    } SlvsSyncCodeDetect_Watchdog;

    UINT16              WatchdogTimeoutH;       /* Offset 20: Watchdog timeout value (Bit[31:16]) in unit of idsp cycles, 0=disabled */
    UINT16              SlvsSyncDetectMask;     /* Offset 21: End row index of crop region */
    UINT16              SlvsSyncDetectPattern;  /* Offset 22: */
    UINT16              SlvsSyncCompareMask;    /* Offset 23: Mask to patch sync code before comparing to desired patterns */
    UINT16              SlvsSolPattern;         /* Offset 24: Pattern to match to indicate SOL code */
    UINT16              SlvsEolPattern;         /* Offset 25: Pattern to match to indicate EOL code */
    UINT16              SlvsSofPattern;         /* Offset 26: Pattern to match to indicate SOF code */
    UINT16              SlvsEofPattern;         /* Offset 27: Pattern to match to indicate EOF code */
    UINT16              SlvsSovPattern;         /* Offset 28: Pattern to match to indicate SOV code */
    UINT16              SlvsEovPattern;         /* Offset 29: Pattern to match to indicate EOV code */

    struct {
        /*
         * Offset 30
         */
        UINT32  VirtChanMask:           2;      /* [1:0] 2-bit value to mask the corresponding virtual channel ID bit. Set it to 0x3 to capture all virtual channels */
        UINT32  VirtChanPattern:        2;      /* [3:2] 2-bit value to indicate the pattern of the virtual channel ID a VIN is supposed to capture */
        UINT32  DataTypeMask:           6;      /* [9:4] 6-bit value to mask the corresponding data type ID bit. Set it to 0x3f to capture all data types */
        UINT32  DataTypePattern:        6;      /* [15:10] 6-bit value to indicate the pattern of the data type ID a VIN is supposed to capture */

        /*
         * Offset 31
         */
        UINT32  EDDataTypeMask:         6;      /* [5:0] 6-bit value to mask the corresponding embedded data ID bit. Set it to 0x3f to capture all data */
        UINT32  EDDataTypePattern:      6;      /* [11:6] 6-bit value to indicate the pattern of the embedded data ID a VIN is supposed to capture */
        UINT32  ByteSwapEnable:         1;      /* [12] 1 = Reverse order of pixels */
        UINT32  EccEnable:              1;      /* [13] 1 = Enable packet header ECC */
        UINT32  ForwardEccEnable:       1;      /* [14] 1 = Enable errored packets forwarding (default mode) */
        UINT32  MipiEDEnable:           1;      /* [15] 1 = Enable Embedded data capturing */
    } MipiCtrl0_1;

    struct {
        /*
         * Offset 32
         */
        UINT32  GclkSoVinPassThr:       1;      /* [0] Only used while C-PHY is working; 0:bclk is selected from word clock or gclk_so_vin by rx_activehs; 1:bclk use gclk_so_vin pass through */
        UINT32  EccVCxOverride:         1;      /* [1] 0:vcx bits will be operated in D-PHY ECC(CSI-2 2.0); 1:vcx bits will be overrided in D-PHY ECC(For CSI-2 1.0); */
        UINT32  VCxMask:                3;      /* [4:2] */
        UINT32  VCxPattern:             3;      /* [7:5] */
        UINT32  Reserved:               8;      /* [15:8] Reserved */

        /*
         * Offset 33
         */
        UINT32  BypassMode:             1;      /* [0] Bypass SLVS-EC pipeline (for 10b8b passthrough) */
        UINT32  Reserved1:              3;      /* [3:1] Reserved */
        UINT32  ValidOnly:              1;      /* [4] Outputs lines only when valid bit in header is set */
        UINT32  BypassFifo:             1;      /* [5] Sends phy data straight to the output */
        UINT32  LinkType:               2;      /* [7:6] 0/1=single link;2=dual link (pixel interleave);3=dual link (pixel interleave, swap) */
        UINT32  EccOption:              2;      /* [9:8] 0=ecc disabled;1=1t ecc;2=2t ecc */
        UINT32  Reserved2:              6;      /* [15:10] Reserved */
    } MipiCtrl2_SlvsEcCtrl0;

    struct {
        /*
         * Offset 34, 35
         */
        UINT32  Sync0Mode:              2;      /* [1:0] 0=Disabled; 1=Even Fields; 2=Odd Fields; 3=All Fields */
        UINT32  Sync0Line:              14;     /* [15:2] VOUT sync0 is asserted for the duration of the specified active line */

        UINT32  Sync1Mode:              2;      /* [1:0] 0=Disabled; 1=Even Fields; 2=Odd Fields; 3=All Fields */
        UINT32  Sync1Line:              14;     /* [15:2] VOUT sync1 is asserted for the duration of the specified active line */

    } VoutSync0_1;

    struct {
        /*
         * Offset 36
         */
        UINT32  Sync2Mode:              2;      /* [1:0] 0=Disabled; 1=Even Fields; 2=Odd Fields; 3=All Fields */
        UINT32  Sync2Line:              14;     /* [15:2] VOUT sync0 is asserted for the duration of the specified active line */

        /*
         * Offset 37
         */
        UINT32  Enable:                 1;      /* [0] Enable GPIO strig0 */
        UINT32  Polarity:               1;      /* [1] 0=Active low; 1=Active high */
        UINT32  StartLine:              14;     /* [15:2] Active line number to start asserting strig0 */
    } VoutSync2_Strig0Ctrl;


    struct {
        UINT32  Strig0EndLine:          16;     /* Offset 38: Delay in cycles after VSYNC before assertion of 'delayed vsync' signal to controller */

        /*
         * Offset 39
         */
        UINT32  Enable:                 1;      /* [0] Enable GPIO strig0 */
        UINT32  Polarity:               1;      /* [1] 0=Active low; 1=Active high */
        UINT32  StartLine:              14;     /* [15:2] Active line number to start asserting strig0 */

    } Strig0EndLine_Strig1Ctrl;

    UINT16              Strig1EndLine;          /* Offset 40: Delay in cycles after VSYNC before assertion of 'delayed vsync' signal to controller */
    UINT16              SyncDelayL;             /* Offset 41: Delay in cycles after VSYNC before assertion of 'delayed vsync' signal to controller */
    UINT16              SyncDelayH;             /* Offset 42: */
    UINT16              IntrDelayL;             /* Offset 43: Delay in cycles after VSYNC before assertion of 'delayed vsync' interrupt to arm */

    struct {
        UINT32  IntrDelayH:             16;     /* Offset 44: */

        /*
         * Offset 45
         */
        UINT32  Sign:                   1;      /* [0] sign bit of black level offset */
        UINT32  Reserved:               15;     /* [15:1] */
    } IntrDelayH_BlackLevelCtrl;

    struct {
        UINT32  BlackLevelOffset16LSB:  16   ;  /* Offset 46: */

        /*
         * Offset 47
         */
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
    } BlackLevelOffset_VinStatus;

    struct {
        /*
         * Offset 48
         */
        UINT32  SyncState:              2;      /* [1:0] State of front-end sync FSM (0=IDLE; 1=ACTIVE; 2=HBLANK; 3=VBLANK) */
        UINT32  SerialDeskewError:      1;      /* [2] */
        UINT32  WatchdogTimeout:        1;      /* [3] */
        UINT32  SfifoCount:             11;     /* [14:4] Number of entries in SFIFO/Buffer */
        UINT32  Reserved:               1;      /* [15] */

        UINT32  DetectedActiveWidth:    16;     /* Offset 49: Detected Active Width */
    } SlvsStatus_DetectedActiveWidth;

    UINT16              DetectedActiveHeight;   /* Offset 50: Detected Active Height */
    UINT16              SyncCode;               /* Offset 51: Value of last received sync code (16-bit MSB aligned) */
    UINT16              ActiveFrameHeight;      /* Offset 52: Current active region height */
    UINT16              ActiveFrameWidth;       /* Offset 53: Current active region width */
    UINT16              CropRegionHeight;       /* Offset 54: Current crop region height */
    UINT16              CropRegionWidth;        /* Offset 55: Current crop region width */

    struct {
        /*
         * Offset 56
         */
        UINT32  EccError2Bit:           1;      /* [0] 2-bit uncorrectable ECC error */
        UINT32  EccError1Bit:           1;      /* [1] 1-bit ECC error corrected */
        UINT32  CrcPhError:             1;      /* [2] C-PHY Packet Header CRC Error */
        UINT32  CrcPkError:             1;      /* [3] Long Packet CRC Error */
        UINT32  FrameSyncError:         1;      /* [4] Frame sync error (Received unpaired SOF or EOF) */
        UINT32  ProtState:              2;      /* [6:5] State of FSM in MIPI protocol module */
        UINT32  SofPacketReceived:      1;      /* [7] SOF Short Packet Received */
        UINT32  EofPacketReceived:      1;      /* [8] EOF Short Packet Received */
        UINT32  Reserved:               7;      /* [15:9] */

        UINT32  MipiLongPacketCount:    16;     /* Offset 57: Number of long packets received*/
    } MipiStatus0;

    UINT16              MipiShortPacketCount;   /* Offset 58: Number of short packets received */
    UINT16              MipiLongPacketSize;     /* Offset 59: Number of bytes in last received long packet */
    UINT16              MipiNumCrcPhErrPacket;  /* Offset 60: Number of Header with CRC errors */
    UINT16              MipiNumCrcPkErrPacket;  /* Offset 61: Number of Packet with CRC errors */
    UINT16              MipiFrameNumber;        /* Offset 62: Frame number (from short-packet/header info) */
    UINT16              MipiNumCorrEccErrPacket;/* Offset 63: Number of packets with correctable errors */

    struct {
        /*
         * Offset 64
         */
        UINT32  CrcError:               1;      /* [0] CRC Error */
        UINT32  LineBoundaryError:      1;      /* [1] Line Boundary Error */
        UINT32  DelayedLineEnd:         1;      /* [2] Delayed Line End Error */
        UINT32  TruncatedPacket:        1;      /* [3] Truncated Packet Error */
        UINT32  IllegalStandby:         1;      /* [4] Illegal Standby Error */
        UINT32  B2PPushState:           3;      /* [7:5] State of B2P module */
        UINT32  DepacketState:          2;      /* [9:8] State of Depacket module */
        UINT32  PhyRxState:             4;      /* [13:10] State of PHY FSM */
        UINT32  Reserved:               2;      /* [15:14] */

        UINT32  SlvsEcLineNumber:       16;     /* Offset 65: Line number in header packet*/
    } SlvsEcStatus0;

    struct {
        UINT32  SlvsEcFrameNumber:      16;     /* Offset 66: Frame number in header packet */

        /*
         * Offset 67
         */
        UINT32  Channel:                2;      /* [1:0] Channel in header packet */
        UINT32  PhyStandbyDet:          12;     /* [13:2] Standby status from phy */
        UINT32  Reserved:               2;      /* [15:14] */
    } SlvsEcStatus1;

    struct {
        /*
         * Offset 68
         */
        UINT32  PhyReady:               12;     /* [11:0] READY status from phy */
        UINT32  Reserved:               4;      /* [15:12] */

        /*
         * Offset 69
         */
        UINT32  PhySymbolErr:           12;     /* [11:0] SYMBOLERR received from PHY (write to clear) */
        UINT32  Reserved1:              4;      /* [15:12] */
    } SlvsEcStatus2_3;

    struct {
        /*
         * Offset 70
         */
        UINT32  Deskew0:                12;     /* [11:0] Deskew status lane0-3 */
        UINT32  Reserved:               4;      /* [15:12] */

        /*
         * Offset 71
         */
        UINT32  Deskew1:                12;     /* [11:0] Deskew status lane4-7 */
        UINT32  Reserved2:              4;      /* [15:12] */
    } SlvsEcStatus4_5;

    struct {
        /*
         * Offset 72
         */
        UINT32  Deskew2:                12;     /* [11:0] Deskew status lane4-7 */
        UINT32  Reserved:               4;      /* [15:12] */

        /*
         * Offset 73
         */
        UINT32  Correct:                1;      /* [0] ECC correct */
        UINT32  Correctable:            1;      /* [1] ECC correctable*/
        UINT32  UnCorrectable:          1;      /* [2] ECC uncorrectable*/
        UINT32  Reserved1:              13;     /* [15:3] */
    } SlvsEcStatus6_Link0EccStat;

    UINT16              SlvsEcLink0EccDbg0;     /* Offset 74: */
    UINT16              SlvsEcLink0EccDbg1;     /* Offset 75: */

    struct {
        UINT32  SlvsEcLink0EccDbg2:     16;     /* Offset 76: */

        /*
         * Offset 77
         */
        UINT32  Correct:                1;      /* [0] ECC correct */
        UINT32  Correctable:            1;      /* [1] ECC correctable*/
        UINT32  UnCorrectable:          1;      /* [2] ECC uncorrectable*/
        UINT32  Reserved:               13;     /* [15:3] */

    } SlvsEcLink0EccDbg2_Link1EccStat;

    UINT16              SlvsEcLink1EccDbg0;     /* Offset 78: */
    UINT16              SlvsEcLink1EccDbg1;     /* Offset 79: */
    UINT16              SlvsEcLink1EccDbg2;     /* Offset 80: */

    UINT16              Reserved[47] ;          /* Offset 81 - 127 */

    struct {
        /*
         * Offset 128
         */
        UINT32  DebugCommandShift:      1;      /* [0] To manually shift VIN programming via debug bus */
        UINT32  Reserved:               15;     /* [15:1] */
        UINT32  Reserved1:              16;     /* Unused */
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
        UINT32  Reserved:               1;      /* [15] */

        UINT32  Reserved1:              16;     /* Unused */
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
    pAmbaMIPI_Reg->Dphy0Ctrl0.BitMode0 = (UINT8)d;
}
static inline void AmbaCSL_VinSetLvdsBitMode1(UINT32 d)
{
    pAmbaMIPI_Reg->Dphy0Ctrl0.BitMode1 = (UINT8)d;
}
static inline void AmbaCSL_VinSetLvdsRsel(UINT32 d)
{
    pAmbaMIPI_Reg->Dphy0Ctrl0.LvdsResistanceSelect = (UINT8)d;
}

static inline UINT32 AmbaCSL_VinGetLvdsBitMode0(void)
{
    return pAmbaMIPI_Reg->Dphy0Ctrl0.BitMode0;
}
static inline UINT32 AmbaCSL_VinGetLvdsBitMode1(void)
{
    return pAmbaMIPI_Reg->Dphy0Ctrl0.BitMode1;
}
static inline UINT32 AmbaCSL_VinGetLvdsLvdsRsel(void)
{
    return pAmbaMIPI_Reg->Dphy0Ctrl0.LvdsResistanceSelect;
}

static inline void AmbaCSL_VinSetLvCmosMode(UINT32 d)
{
    pAmbaMIPI_Reg->Dphy0Ctrl3.LvCmosMode = (UINT8)d;
}
static inline void AmbaCSL_VinSetLvCmosModeSpclk(UINT32 d)
{
    pAmbaMIPI_Reg->Dphy0Ctrl3.LvCmosSpClk = (UINT8)d;
}
static inline void AmbaCSL_VinSetMipiMode(UINT32 d)
{
    pAmbaMIPI_Reg->Dphy0Ctrl3.MipiMode = (UINT8)d;
}

static inline UINT32 AmbaCSL_VinGetLvCmosMode(void)
{
    return pAmbaMIPI_Reg->Dphy0Ctrl3.LvCmosMode;
}
static inline UINT32 AmbaCSL_VinGetLvCmosModeSpclk(void)
{
    return pAmbaMIPI_Reg->Dphy0Ctrl3.LvCmosSpClk;
}
static inline UINT32 AmbaCSL_VinGetMipiMode(void)
{
    return pAmbaMIPI_Reg->Dphy0Ctrl3.MipiMode;
}

static inline void AmbaCSL_VinSetLvdsIbCtrl(UINT32 d)
{
    pAmbaMIPI_Reg->Dphy0Ctrl1.LvdsIbCtrl = (UINT8)d;
}
static inline void AmbaCSL_RctSetLvdsCtrlPowerDown(UINT32 d)
{
    pAmbaMIPI_Reg->Dphy0Ctrl1.PowerDownLvds = (UINT8)d;
}
static inline void AmbaCSL_VinSetDphyRstAfe(UINT32 d)
{
    pAmbaMIPI_Reg->Dphy0Ctrl0.RstDphyAfe = (UINT8)d;
}
static inline void AmbaCSL_VinSetDphyRst0(UINT32 d)
{
    pAmbaMIPI_Reg->Dphy0Ctrl0.DphyRst0 = (UINT8)d;
}
static inline void AmbaCSL_VinSetDphyRst1(UINT32 d)
{
    pAmbaMIPI_Reg->Dphy0Ctrl0.DphyRst1 = (UINT8)d;
}

static inline UINT32 AmbaCSL_VinGetLvdsIbCtrl(void)
{
    return pAmbaMIPI_Reg->Dphy0Ctrl1.LvdsIbCtrl;
}
static inline UINT32 AmbaCSL_VinGetDphyRstAfe(void)
{
    return pAmbaMIPI_Reg->Dphy0Ctrl0.RstDphyAfe;
}
static inline UINT32 AmbaCSL_VinGetDphyRst0(void)
{
    return pAmbaMIPI_Reg->Dphy0Ctrl0.DphyRst0;
}
static inline UINT32 AmbaCSL_VinGetDphyRst1(void)
{
    return pAmbaMIPI_Reg->Dphy0Ctrl0.DphyRst1;
}

static inline void AmbaCSL_VinSetMipiHsSettle0(UINT32 d)
{
    pAmbaMIPI_Reg->Dphy0Ctrl4.HsSettleCtrl0 = (UINT8)d;
}
static inline void AmbaCSL_VinSetMipiHsTerm0(UINT32 d)
{
    pAmbaMIPI_Reg->Dphy0Ctrl4.HsTermCtrl0 = (UINT8)d;
}
static inline void AmbaCSL_VinSetMipiClkSettle0(UINT32 d)
{
    pAmbaMIPI_Reg->Dphy0Ctrl5.ClkSettleCtrl0 = (UINT8)d;
}
static inline void AmbaCSL_VinSetMipiClkTerm0(UINT32 d)
{
    pAmbaMIPI_Reg->Dphy0Ctrl5.ClkTermCtrl0 = (UINT8)d;
}
static inline void AmbaCSL_VinSetMipiClkMiss0(UINT32 d)
{
    pAmbaMIPI_Reg->Dphy0Ctrl4.ClkMissCtrl0 = (UINT8)d;
}
static inline void AmbaCSL_VinSetMipiRxInit0(UINT32 d)
{
    pAmbaMIPI_Reg->Dphy0Ctrl4.InitRxCtrl0 = (UINT8)d;
}
static inline void AmbaCSL_VinSetMipiForceClkHs0(UINT32 d)
{
    pAmbaMIPI_Reg->Dphy0Ctrl2.ForceClkHs0 = (UINT8)d;
}

static inline UINT32 AmbaCSL_VinGetMipiHsSettle0(void)
{
    return pAmbaMIPI_Reg->Dphy0Ctrl4.HsSettleCtrl0;
}
static inline UINT32 AmbaCSL_VinGetMipiHsTerm0(void)
{
    return pAmbaMIPI_Reg->Dphy0Ctrl4.HsTermCtrl0;
}
static inline UINT32 AmbaCSL_VinGetMipiClkSettle0(void)
{
    return pAmbaMIPI_Reg->Dphy0Ctrl5.ClkSettleCtrl0;
}
static inline UINT32 AmbaCSL_VinGetMipiClkTerm0(void)
{
    return pAmbaMIPI_Reg->Dphy0Ctrl5.ClkTermCtrl0;
}
static inline UINT32 AmbaCSL_VinGetMipiClkMiss0(void)
{
    return pAmbaMIPI_Reg->Dphy0Ctrl4.ClkMissCtrl0;
}
static inline UINT32 AmbaCSL_VinGetMipiRxInit0(void)
{
    return pAmbaMIPI_Reg->Dphy0Ctrl4.InitRxCtrl0;
}
static inline UINT32 AmbaCSL_VinGetMipiForceClkHs0(void)
{
    return pAmbaMIPI_Reg->Dphy0Ctrl2.ForceClkHs0;
}
static inline UINT32 AmbaCSL_VinGetMipiRxClkActHs0(void)
{
    return pAmbaMIPI_Reg->Dphy0Obsv0.RxClkActiveHS0;
}

static inline void AmbaCSL_VinSetMipiHsSettle1(UINT32 d)
{
    pAmbaMIPI_Reg->Dphy0Ctrl7.HsSettleCtrl1 = (UINT8)d;
}
static inline void AmbaCSL_VinSetMipiHsTerm1(UINT32 d)
{
    pAmbaMIPI_Reg->Dphy0Ctrl7.HsTermCtrl1 = (UINT8)d;
}
static inline void AmbaCSL_VinSetMipiClkSettle1(UINT32 d)
{
    pAmbaMIPI_Reg->Dphy0Ctrl8.ClkSettleCtrl1 = (UINT8)d;
}
static inline void AmbaCSL_VinSetMipiClkTerm1(UINT32 d)
{
    pAmbaMIPI_Reg->Dphy0Ctrl8.ClkTermCtrl1 = (UINT8)d;
}
static inline void AmbaCSL_VinSetMipiClkMiss1(UINT32 d)
{
    pAmbaMIPI_Reg->Dphy0Ctrl7.ClkMissCtrl1 = (UINT8)d;
}
static inline void AmbaCSL_VinSetMipiRxInit1(UINT32 d)
{
    pAmbaMIPI_Reg->Dphy0Ctrl7.InitRxCtrl1 = (UINT8)d;
}
static inline void AmbaCSL_VinSetMipiForceClkHs1(UINT32 d)
{
    pAmbaMIPI_Reg->Dphy0Ctrl2.ForceClkHs1 = (UINT8)d;
}

static inline UINT32 AmbaCSL_VinGetMipiHsSettle1(void)
{
    return pAmbaMIPI_Reg->Dphy0Ctrl7.HsSettleCtrl1;
}
static inline UINT32 AmbaCSL_VinGetMipiHsTerm1(void)
{
    return pAmbaMIPI_Reg->Dphy0Ctrl7.HsTermCtrl1;
}
static inline UINT32 AmbaCSL_VinGetMipiClkSettle1(void)
{
    return pAmbaMIPI_Reg->Dphy0Ctrl8.ClkSettleCtrl1;
}
static inline UINT32 AmbaCSL_VinGetMipiClkTerm1(void)
{
    return pAmbaMIPI_Reg->Dphy0Ctrl8.ClkTermCtrl1;
}
static inline UINT32 AmbaCSL_VinGetMipiClkMiss1(void)
{
    return pAmbaMIPI_Reg->Dphy0Ctrl7.ClkMissCtrl1;
}
static inline UINT32 AmbaCSL_VinGetMipiRxInit1(void)
{
    return pAmbaMIPI_Reg->Dphy0Ctrl7.InitRxCtrl1;
}
static inline UINT32 AmbaCSL_VinGetMipiForceClkHs1(void)
{
    return pAmbaMIPI_Reg->Dphy0Ctrl2.ForceClkHs1;
}
static inline UINT32 AmbaCSL_VinGetMipiRxClkActHs1(void)
{
    return pAmbaMIPI_Reg->Dphy0Obsv0.RxClkActiveHS1;
}

static inline void AmbaCSL_VinSetMipiSkewCalEn(UINT32 d)
{
    pAmbaMIPI_Reg->Dphy0Ctrl9.EnCal = (UINT8)d;
    pAmbaMIPI_Reg->Dphy0Ctrl9.SkewCalExpire = 0x2U; //Suggested by Alex and result looks fine on 2.5G datarate
    pAmbaMIPI_Reg->Dphy0Ctrl9.TimeWindow = 0x2U;
}

/* vin reference clock select */
static inline void AmbaCSL_VinSetRefClkSel(UINT32 d)
{
    pAmbaRCT_Reg->VinClkSelect.VinRefClkSelect = (UINT8)d;
}

static inline UINT32 AmbaCSL_VinGetRefClkSel(void)
{
    return pAmbaRCT_Reg->VinClkSelect.VinRefClkSelect;
}

/*
 * Defined in AmbaCSL_VIN.c
 */
void AmbaCSL_VinSetMipiCtrl0(UINT32 d);
UINT32 AmbaCSL_VinGetMipiCtrl0(void);

UINT32 AmbaCSL_VinSetVin8LvdsPadMode(UINT32 PadMode, UINT32 DataLaneEnable);
UINT32 AmbaCSL_VinSetVin11LvdsPadMode(UINT32 PadMode, UINT32 DataLaneEnable);
UINT32 AmbaCSL_VinSetDcphyParam(UINT32 VinID, const AMBA_VIN_DCPHY_PARAM_s* pDcphyParam, UINT32 EnabledPin);
UINT32 AmbaCSL_VinDcphyEnable(UINT32 VinID, UINT32 EnabledPin, UINT32 ClkLaneEnable);
UINT32 AmbaCSL_VinSetSlvsEcPhyParam(UINT32 EnabledPin, UINT64 DataRate, UINT32 Stage);
UINT32 AmbaCSL_VinGetSlvsEcPhyRxState(UINT32 RxPortNum);
UINT32 AmbaCSL_VinSlvsConfigGlobal(UINT32 VinID);
UINT32 AmbaCSL_VinMipiConfigGlobal(UINT32 VinID);
UINT32 AmbaCSL_VinMipiCphyConfigGlobal(UINT32 VinID);
UINT32 AmbaCSL_VinSlvsEcConfigGlobal(UINT32 VinID);
UINT32 AmbaCSL_VinSlvsConfigMain(UINT32 VinID, const AMBA_VIN_SLVS_CONFIG_s *pVinSlvsConfig);
UINT32 AmbaCSL_VinMipiConfigMain(UINT32 VinID, const AMBA_VIN_MIPI_CONFIG_s *pVinMipiConfig);
UINT32 AmbaCSL_VinMipiCphyConfigMain(UINT32 VinID, const AMBA_VIN_MIPI_CONFIG_s *pVinMipiCphyConfig);
UINT32 AmbaCSL_VinSlvsEcConfigMain(UINT32 VinID, const AMBA_VIN_SLVSEC_CONFIG_s *pVinSlvsEcConfig);
UINT32 AmbaCSL_VinMasterSyncConfig(UINT32 MSyncID, const AMBA_VIN_MASTER_SYNC_CONFIG_s *pMSyncConfig);
UINT32 AmbaCSL_VinDataLaneRemap(UINT32 VinID, const AMBA_VIN_LANE_REMAP_CONFIG_s *pLaneRemapConfig);
UINT32 AmbaCSL_VinMipiVirtChanConfig(UINT32 VinID, const AMBA_VIN_MIPI_VC_CONFIG_s *pMipiVirtChanConfig);
UINT32 AmbaCSL_VinMipiEmbDataConfig(UINT32 VinID, const AMBA_VIN_MIPI_EMB_DATA_CONFIG_s *pMipiEmbDataConfig);
UINT32 AmbaCSL_VinConfigMainThrDbgPort(UINT32 VinID, const ULONG MainCfgBufAddr);

UINT32 AmbaCSL_VinCaptureConfig(UINT32 VinID, const AMBA_CSL_VIN_WINDOW_s *pCaptureWindow);
UINT32 AmbaCSL_VinGetMainCfgBufAddr(UINT32 VinID, ULONG *pAddr);
UINT32 AmbaCSL_VinGetGlobalCfgBufAddr(UINT32 VinID, ULONG *pAddr);
UINT32 AmbaCSL_VinGetMSyncCfgBufAddr(UINT32 MSyncID, ULONG *pAddr);
UINT32 AmbaCSL_VinGetInfo(UINT32 VinID, AMBA_CSL_VIN_INFO_s *pInfo);
UINT32 AmbaCSL_VinIsEnabled(UINT32 VinID);
void AmbaCSL_VinSetSensorClkDriveStr(UINT32 Value);

void AmbaCSL_VinSetDcphyRstn0(UINT32 PhyMode, UINT32 Value);
void AmbaCSL_VinSetDcphyRstn1(UINT32 PhyMode, UINT32 Value);

#endif /* AMBA_CSL_VIN_H */
