/**
 *  @file AmbaReg_Merger.h
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
 *  @details Definitions & Constants for B6 Merger Control Registers
 *
 */

#ifndef AMBA_B8_REG_MERGER_H
#define AMBA_B8_REG_MERGER_H

/*-----------------------------------------------------------------------------------*\
 * B6 Merger : Control Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  Enable:                 1;      /* [0] 0: Disable, 1: Enable */
    UINT32  lvdsLaneWidth:          5;      /* [5:1] 1-lane: 'b00001, 2-lane: b'00010, 4-lane: 'b00100, 8-lane: 'b01000, 10-lane: 'b10000 */
    UINT32  VoutPixelWidth:         4;      /* [9:6] 8-bit: 'b0001, 10-bit: 'b0010, 12-bit: 'b0100, 14-bit: 'b1000 */
    UINT32  B8anEnable:             1;      /* [10]] 0: B8A-F mode, 1: B8A-N mode */
    UINT32  FarEndMode:             2;      /* [12:11]] 0: Directly bypass mode, 1: swap vin bypass mode, 2: Split mode, 3: Merge mode */
    UINT32  FarEndDemuxSel:         1;      /* [13]] 0: Merger out to merger0, 1: Merger out to merger1 */
    UINT32  FarEndmuxSel:           1;      /* [14]] 0: Select VIN 0 to split, 1: Select VIN 1 to split */
    UINT32  NearEndLvdsEn:          1;      /* [15]] 0: Drive vout path, 1: Drive lvds path */
    UINT32  B8dVoutBypass:          1;      /* [16]] 0: Normal mode, 1: Bypass pp_merger_0 to merger_vout_path */
    UINT32  Reserved0:              14;     /* [30:17] Reserved */
    UINT32  MergerClear:            1;      /* [31] Software clear */
} B8_MERGER_CTRL_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B6 Merger : Source Select Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  VoutSensorSelect:       4;      /* [3:0] [i]=1, from near end vin, [i]=0, fron post-processor, [1] and [3] are useless */
    UINT32  Reserved0:              12;     /* [15:4] Reserved */
    UINT32  DecmpSourceEn:          4;      /* [19:16] [i]=1, path de-compressor enable, [i]=0, path de-compressor is disable, [1] and [3] are useless */
    UINT32  Reserved1:              12;     /* [31:20] Reserved */
} B8_MERGER_SOURCE_SEL_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B6 Merger : Channel Mux Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  VoufMuxCh0:             2;      /* [1:0] 0: Ch0 from 0th input stream, 1: Ch0 from 1th input stream... */
    UINT32  VoufMuxCh1:             2;      /* [3:2] 0: Ch1 from 0th input stream, 1: Ch1 from 1th input stream... */
    UINT32  VoufMuxCh2:             2;      /* [5:4] 0: Ch2 from 0th input stream, 1: Ch2 from 1th input stream... */
    UINT32  VoufMuxCh3:             2;      /* [7:6] 0: Ch3 from 0th input stream, 1: Ch3 from 1th input stream... */
    UINT32  VoufLastCh:             2;      /* [9:8] Reserved */
    UINT32  Reserved1:              22;     /* [31:10] Reserved */
} B8_MERGER_CHANNEL_MUX_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B6 Merger : Mode configure Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  VoutfBorderValue:       14;     /* [13:0] 14-bits, Border pixel value */
    UINT32  InitialMode:            1;      /* [14] 0: No additional one line of VB on initial state, 1: additional one line of VB on initial state */
    UINT32  EavEnable:              1;      /* [15] EAV enable */
    UINT32  VoutfBlankValue:        14;     /* [29:16] 14-bits, Voutf blank pixel value */
    UINT32  LineInterleave:         1;      /* [30] 0: Line concatenating mode, 1: Line interleaving mode */
    UINT32  Reserved0:              1;      /* [31] Reserved */
} B8_MERGER_MODE_CFG_REG_s;


/*-----------------------------------------------------------------------------------*\
 * B6 Merger : Stream width Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  LineWidth_0_2:          14;     /* [13:0] Stream #0/#2 width */
    UINT32  Reserved0:              2;      /* [15:14] Reserved */
    UINT32  LineWidth_1_3:          14;     /* [29:16] Stream #1/#3 width */
    UINT32  Reserved1:              2;      /* [31:30] Reserved */
} B8_MERGER_STREAM_WIDTH_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B6 Merger : Split stream width Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  SplitWidth:             14;     /* [13:0] Split width */
    UINT32  Reserved0:              2;      /* [15:14] Reserved */
    UINT32  SplitWidthTh:           14;     /* [29:16] Split width threshold */
    UINT32  Reserved1:              2;      /* [31:30] Reserved */
} B8_MERGER_SPLIT_STREAM_WIDTH_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B6 Merger : Split stream height Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  SplitHeight:            14;     /* [13:0] Split height */
    UINT32  Reserved0:              18;     /* [31:14] Reserved */
} B8_MERGER_SPLIT_STREAM_HEIGHT_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B6 Merger : Output frame format Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  OutputFrameHeight:      14;     /* [13:0] Output frame height */
    UINT32  MinHB:                  10;     /* [23:14] Minimum pixels from EAV to SAV */
    UINT32  MinVB:                  8;      /* [31:24] Minimum line count of vertical blank */
} B8_MERGER_OUTPUT_FORMAT0_REG_s;

typedef struct {
    UINT32  MaxHB:                  16;     /* [15:0] Maximum horizontal blanking to prevent short frame */
    UINT32  VoutfBorderWidth:       4;      /* [19:16] Border width in LVDS lane pixel, 4-bits, 0-15 */
    UINT32  Reserved0:              12;     /* [31:20] Reserved */
} B8_MERGER_OUTPUT_FORMAT1_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B6 Merger : Threshold Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  LineBufferRaemptyTh:    15;     /* [14:0] 15-bits, Read threshold of line_buffer_i */
    UINT32  Reserved0:              1;      /* [15] Reserved */
    UINT32  LineBufferWafullTh:     15;     /* [30:16] 15-bits, Force to read line_buffer_i */
    UINT32  Reserved1:              1;      /* [31] Reserved */
} B8_MERGER_THRESHOLD_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B6 Merger : HDR sensor configuration Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  WidthDiv4Minus1_0:       12;     /* [11:0] HDR width div4 for VIN0 */
    UINT32  NumExpMinus1_0:          2;      /* [13:12] Number of exposures for VIN0, which should be set to match with VIN format and CFA Codec */
    UINT32  Reserved0:               2;      /* [15:14] Reserved */
    UINT32  WidthDiv4Minus1_1:       12;     /* [27:16] HDR width div4 for VIN1 */
    UINT32  NumExpMinus1_1:          2;      /* [29:28] Number of exposures for VIN1, which should be set to match with VIN format and CFA Codec */
    UINT32  Reserved1:               2;      /* [31:30] Reserved */
} B8_MERGER_HDR_SENSOR_CFG_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B6 Merger : Sensor path Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  VoutGenEnable:          4;      /* [3:0] Enable sensor path */
    UINT32  Reserved0:              12;     /* [15:4] Reserved */
    UINT32  VoutInternalGen:        4;      /* [19:16] 0: VOUTF from sensor, 1: Internal generated pattern (on-the-fly) */
    UINT32  Reserved1:              12;     /* [31:20] Reserved */
} B8_MERGER_SENSOR_PATH_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B6 Merger : Internal generation Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  VoutInternalGenIncr:    4;      /* [3:0] 0: Fixed value, 1: Internal generated pattern incrementally */
    UINT32  Reserved0:              12;     /* [15:4] Reserved */
    UINT32  VoutInternalGenValue:   14;     /* [29:16] 14 bits, Voutf internal generation value */
    UINT32  Reserved1:              2;      /* [31:30] Reserved */
} B8_MERGER_INTERNAL_GEN_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B6 Merger : Merger mode Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  LegacyMerge2En:         1;      /* [0] Legacy B6 split mode */
    UINT32  Reserved0:              1;      /* [1] Legacy B6 split mode */
    UINT32  VlcMerge2En:            2;      /* [3:2] [0]: non-merger2_en for 2*i and merger 2*i+1 channels, [1]: merger 2*i and merge 2*i+1 channels */
    UINT32  Reserved1:              28;     /* [31:4] Reserved */
} B8_MERGER_MODE_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B6 Merger : Merger error packet loss Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  ErrPktLossIt:           4;      /* [3:0] Packet loss occur on channel i */
    UINT32  Reserved0:              12;     /* [15:4] Reserved */
    UINT32  ErrPktLossRitr:         4;      /* [19:16] Packet loss occur on channel i */
    UINT32  Reserved1:              4;      /* [23:20] Reserved */
    UINT32  ErrPktLossMask:         4;      /* [27:24] Packet loss occur on channel i */
    UINT32  Reserved2:              4;      /* [31:28] Reserved */
} B8_MERGER_ERR_PKT_LOSS_REG_s;

/*-----------------------------------------------------------------------------------------------*\
 * B6 Merger : All Registers
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    volatile B8_MERGER_CTRL_REG_s                   Ctrl;                       /* 0x00(RW): Merger control */
    volatile B8_MERGER_SOURCE_SEL_REG_s             SourceSel;                  /* 0x04(RW): Merger source select */
    volatile B8_MERGER_CHANNEL_MUX_REG_s            ChannelMux;                 /* 0x08(RW): Merger channel mux */
    volatile B8_MERGER_MODE_CFG_REG_s               ModeConfig;                 /* 0x0C(RW): Reserved */
    volatile B8_MERGER_STREAM_WIDTH_REG_s           InputStreamWidth0;          /* 0x10(RW): Mode configuration */
    volatile B8_MERGER_STREAM_WIDTH_REG_s           InputStreamWidth1;          /* 0x14(RW): Input stream width 0 */
    volatile UINT32                                 Reserved0[2];               /* 0x18-0x1C(RW): Reserved */
    volatile B8_MERGER_SPLIT_STREAM_WIDTH_REG_s     SplitStreamWidth;           /* 0x20(RW): Split stream width */
    volatile B8_MERGER_SPLIT_STREAM_HEIGHT_REG_s    SplitStreamHeight;          /* 0x24(RW): Split stream height */
    volatile UINT32                                 ClampEnable;                /* 0x28: 0U: Not clamping */
    volatile B8_MERGER_OUTPUT_FORMAT0_REG_s         OutputFrameFormat0;         /* 0x2C(RW): Output frame format 0 */
    volatile B8_MERGER_OUTPUT_FORMAT1_REG_s         OutputFrameFormat1;         /* 0x30(RW): Output frame format 1 */
    volatile B8_MERGER_THRESHOLD_REG_s              Threshold0;                 /* 0x34(RW): Threshold 0 */
    volatile B8_MERGER_THRESHOLD_REG_s              Threshold1;                 /* 0x38(RW): Threshold 1 */
    volatile B8_MERGER_THRESHOLD_REG_s              Threshold2;                 /* 0x3C(RW): Threshold 2 */
    volatile B8_MERGER_THRESHOLD_REG_s              Threshold3;                 /* 0x40(RW): Threshold 3 */
    volatile UINT32                                 Reserved3[7];               /* 0x44-5C: Reserved */
    volatile B8_MERGER_HDR_SENSOR_CFG_REG_s         HdrSensorCfg;               /* 0x60(RW): HDR sensor configuration */
    volatile B8_MERGER_SENSOR_PATH_REG_s            SensorPath;                 /* 0x64(RW): Sensor path */
    volatile B8_MERGER_INTERNAL_GEN_REG_s           InternalGen;                /* 0x68(RW): Internal generation value */
    volatile B8_MERGER_MODE_REG_s                   MergerMode;                 /* 0x6C(RW): Merger Mode */
    volatile UINT32                                 CfgDone;                    /* 0x70(RW): Configuration done */
    volatile UINT32                                 ErrorStatusInterrupt0;      /* 0x74(RO): Error status interrupt 0 */
    volatile UINT32                                 ErrorStatusInterrupt1;      /* 0x78(RO): Error status interrupt 1 */
    volatile UINT32                                 ErrorStatusRawInterrupt0;   /* 0x7C(RWC): Error status raw interrupt 0 */
    volatile UINT32                                 ErrorStatusRawInterrupt1;   /* 0x80(RWC): Error status raw interrupt 1 */
    volatile UINT32                                 ErrorStatusMask0;           /* 0x84(RW): Error status mask 0 */
    volatile UINT32                                 ErrorStatusMask1;           /* 0x88(RW): Error status mask 1 */
    volatile UINT32                                 PllCounter0;                /* 0x8C(RO): PLL counter 0 */
    volatile UINT32                                 PllCounter1;                /* 0x90(RO): PLL counter 1 */
    volatile UINT32                                 PllCounter2;                /* 0x94(RO): PLL counter 2 */
    volatile UINT32                                 PllCounter3;                /* 0x98(RO): PLL counter 3 */
    volatile UINT32                                 Reserved4[8];               /* 0x9C-B8: Reserved */
    volatile UINT32                                 PllCounterEn;               /* 0xBC(RW): PLL counter enable */
    volatile UINT32                                 PllCounter;                 /* 0xC0(RW): PLL counter */
    volatile UINT32                                 Reserved5;                  /* 0xC4    : Reserved */
    volatile UINT32                                 PacketLossCnt;              /* 0xC8(RO): Packet loss count */
    volatile UINT32                                 Reserved6;                  /* 0xCC    : Reserved */
    volatile UINT32                                 PacketLossFifoX0;           /* 0xD0(RO): Packet loss fifo x 0 */
    volatile UINT32                                 PacketLossFifoX1;           /* 0xD4(RO): Packet loss fifo x 1 */
    volatile UINT32                                 PacketLossFifoX2;           /* 0xD8(RO): Packet loss fifo x 2 */
    volatile UINT32                                 PacketLossFifoX3;           /* 0xDC(RO): Packet loss fifo x 3 */
    volatile UINT32                                 PacketLossFifoY0;           /* 0xE0(RO): Packet loss fifo y 0 */
    volatile UINT32                                 PacketLossFifoY1;           /* 0xE4(RO): Packet loss fifo y 1 */
    volatile UINT32                                 PacketLossFifoY2;           /* 0xE8(RO): Packet loss fifo y 2 */
    volatile UINT32                                 PacketLossFifoY3;           /* 0xEC(RO): Packet loss fifo y 3 */
} B8_MERGER_REG_s;

#endif /* AMBA_B8_REG_MERGER_H */
