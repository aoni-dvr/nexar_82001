/**
 *  @file AmbaDebugPort_SMEM.h
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
 *  @details Definitions & Constants for SMEM Cluster
 *
 */

#ifndef AMBA_DEBUG_PORT_SMEM_H
#define AMBA_DEBUG_PORT_SMEM_H

/*
 * SMEM Debug Port: SYNC counter 0-63
 */
typedef union {
    UINT32  Data;

    struct {
        UINT32 NewMaxCounterValue:  12;     /* [11:0]: New Maximum Counter Value */
        UINT32 NewCounterValue:     12;     /* [23:12]: New Sync Counter Value */
        UINT32 NotChangeMaxValue:   1;      /* [24]: 1 = Not Change Maximum Sync Counter Value */
        UINT32 NotChangeCurValue:   1;      /* [25]: 1 = Not Change Current Sync Counter Value */
        UINT32 Reserved:            6;      /* [31:26] */
    } Bits;
} AMBA_DBG_PORT_SMEM_SYNC_COUNTER_REG_u;

/*
 * SMEM Debug Port: Dram Direct Settings
 */
typedef union {
    UINT32  Data;

    struct {
        UINT32 DeepQueueConfig:     2;     /* [1:0]: TSFM-LD Deep Queue Configuration */
        UINT32 Reserved:            6;     /* [7:2] */
        UINT32 DirectDataAmount:    2;     /* [9:8]: Amount of TSDM DRAM-Direct data */
        UINT32 Reserved1:           2;     /* [11:10] */
        UINT32 DramPriorityLevel:   2;     /* [13:12]: DRAM Priority Level for TSFM-LD DRAM Direct Fetches */
        UINT32 Reserved2:           2;     /* [15:14] */
        UINT32 DramPageSize:        2;     /* [17:16]: DRAM Page Size(0 - 1kB; 1 - 2kB; 2 - 4kB) */
        UINT32 Reserved3:           2;     /* [19:18] */
        UINT32 DramBanks:           1;     /* [20]: #DRAM banks(1 = 8; 0 = 4) */
        UINT32 Reserved4:           10;    /* [30:21] */
        UINT32 DramOverrideInfo:    1;     /* [31]: Override DRAM Bank/Page Info from DRAM Block */
    } Bits;
} AMBA_DBG_PORT_SMEM_DRAM_DIRECT_REG_u;

/*
 * SMEM Debug Port: FMEM Reflesh Value
 */
typedef union {
    UINT32  Data;

    struct {
        UINT32 RefleshValue:        8;     /* [7:0]: Initialized to 255 on Reset */
        UINT32 Reserved:            24;    /* [31:8] */
    } Bits;
} AMBA_DBG_PORT_SMEM_FMEM_REFRESH_REG_u;

/*
 * SMEM Debug Port: Exact Internal Behavior of the DRAM RMB
 */
typedef union {
    UINT32  Data;

    struct {
        UINT32 RequestStandby:      1;     /* [0]: 1 = DRAM RMB Not Start Making Requests for a New Priority 0 until Previous Requests have Completed */
        UINT32 Reserved0:           31;    /* [31:1] */
    } Bits;
} AMBA_DBG_PORT_SMEM_DRAM_BEHAVIOR_REG_u;

/*
 * SMEM Debug Port: FMEM Datapath Config
 */
typedef union {
    UINT32  Data;

    struct {
        UINT32 DatapathDisable:     1;     /* [0]: 1 = Second Load Datapath from Being Used is Disable */
        UINT32 Reserved:            31;    /* [31:1] */
    } Bits;
} AMBA_DBG_PORT_SMEM_FMEM_DATAPATH_REG_u;

/*
 * SMEM Debug Port: Maximum Number of Outstanding Transfers Allowed for Prioryty Group 0
 */
typedef union {
    UINT32  Data;

    struct {
        UINT32 Reserved:            28;    /* [27:0] */
        UINT32 MaximunNumber:       2;     /* [29:28]: Maximum Number of Outstanding Transfers, minus 1 */
        UINT32 Reserved1:           2;     /* [31:30] */
    } Bits;
} AMBA_DBG_PORT_SMEM_MAXNUM_GROUP0_REG_u;

/*
 * SMEM Debug Port: Maximum Number of Outstanding Transfers Allowed for Prioryty Group 1
 */
typedef union {
    UINT32  Data;

    struct {
        UINT32 Reserved:            28;    /* [27:0] */
        UINT32 MaximunNumber:       2;     /* [29:28]: Maximum Number of Outstanding Transfers, minus 1 */
        UINT32 Reserved1:           2;     /* [31:30] */
    } Bits;
} AMBA_DBG_PORT_SMEM_MAXNUM_GROUP1_REG_u;

/*
 * SMEM Debug Port: Maximum Number of Outstanding Transfers Allowed for Prioryty Group 2
 */
typedef union {
    UINT32  Data;

    struct {
        UINT32 Reserved:            28;    /* [27:0] */
        UINT32 MaximunNumber:       2;     /* [29:28]: Maximum Number of Outstanding Transfers, minus 1 */
        UINT32 Reserved1:           2;     /* [31:30] */
    } Bits;
} AMBA_DBG_PORT_SMEM_MAXNUM_GROUP2_REG_u;

/*
 * SMEM Debug Port: Maximum Number of Outstanding Transfers Allowed for Prioryty Group 3
 */
typedef union {
    UINT32  Data;

    struct {
        UINT32 Reserved:            28;    /* [27:0] */
        UINT32 MaximunNumber:       2;     /* [29:28]: Maximum Number of Outstanding Transfers, minus 1 */
        UINT32 Reserved1:           2;     /* [31:30] */
    } Bits;
} AMBA_DBG_PORT_SMEM_MAXNUM_GROUP3_REG_u;

/*
 * SMEM Debug Port: XFER DRAM Transfer Status
 */
typedef union {
    UINT32  Data;

    struct {
        UINT32 DramWriteStatus:     5;      /* [4:0]: DRAM-Write RMB Status */
        UINT32 Reserved0:           3;      /* [7:5] */
        UINT32 DramReadStatus:      6;      /* [13:8]: DRAM-Read RMB Status */
        UINT32 Reserved1:           18;     /* [31:14] */
    } Bits;
} AMBA_DBG_PORT_SMEM_XFER_DRAM_TRANS_REG_u;

/*
 * SMEM Debug Port: XFER DRAM Client Transfer Status
 */
typedef union {
    UINT32  Data;

    struct {
        UINT32 TotalCountOutStand:  7;      /* [6:0]: Total XFER Count Outstanding */
        UINT32 Reserved0:           1;      /* [7] */
        UINT32 BlockState:          3;      /* [10:8]: XFRT Block State */
        UINT32 BlockActive:         1;      /* [11]: XFER Block Active (making request to DRAM) */
        UINT32 Reserved1:           4;      /* [15:12] */
        UINT32 ClientBlockState:   13;      /* [28:16]: Client xfers blocked state (IDSP-ST to IDSP-TILE) */
        UINT32 Reserved2:           3;      /* [31:29] */
    } Bits;
} AMBA_DBG_PORT_SMEM_XFER_CLIENT_STATE_REG_u;

/*
 * SMEM Debug Port: SYNC Counter Status for CODE orc
 */
typedef union {
    UINT32  Data;

    struct {
        UINT32 CodeTh0:             3;      /* [2:0]: CODE TH0 */
        UINT32 CodeTh1:             3;      /* [5:3]: CODE TH1 */
        UINT32 CodeTh2:             3;      /* [8:6]: CODE TH2 */
        UINT32 CodeTh3:             3;      /* [11:9]: CODE TH3 */
        UINT32 CodeTh4:             3;      /* [14:12]: CODE TH4 */
        UINT32 CodeTh5:             3;      /* [17:15]: CODE TH5 */
        UINT32 CodeTh6:             3;      /* [20:18]: CODE TH6 */
        UINT32 CodeTh7:             3;      /* [23:21]: CODE TH7 */
        UINT32 MeTh0:               3;      /* [26:24]: ME TH0 */
        UINT32 MeTh1:               3;      /* [29:27]: ME TH1 */
        UINT32 Reserved:            2;      /* [31:30] */
    } Bits;
} AMBA_DBG_PORT_SMEM_SYNC_STATUS0_REG_u;

/*
 * SMEM Debug Port: SYNC Counter Status for ME/MD orc
 */
typedef union {
    UINT32  Data;

    struct {
        UINT32 MeTh2:               3;      /* [2:0]: ME TH2 */
        UINT32 MeTh3:               3;      /* [5:3]: ME TH3 */
        UINT32 MdTh0:               3;      /* [8:6]: MD ID0 */
        UINT32 MdTh1:               3;      /* [11:9]: MD ID1 */
        UINT32 IdspID0:             3;      /* [14:12]: IDSP ID0 */
        UINT32 IdspID1:             3;      /* [17:15]: IDSP ID1 */
        UINT32 IdspID2:             3;      /* [20:18]: IDSP ID2 */
        UINT32 IdspID3:             3;      /* [23:21]: IDSP ID3 */
        UINT32 IdspID4:             3;      /* [26:24]: IDSP ID4 */
        UINT32 IdspID5:             3;      /* [29:27]: IDSP ID5 */
        UINT32 Reserved:            2;      /* [31:30] */
    } Bits;
} AMBA_DBG_PORT_SMEM_SYNC_STATUS1_REG_u;

/*
 * SMEM Debug Port: SYNC Counter Status for IDSP
 */
typedef union {
    UINT32  Data;

    struct {
        UINT32 IdspID6:             3;      /* [2:0]: IDSP ID6 */
        UINT32 IdspID7:             3;      /* [5:3]: IDSP ID7 */
        UINT32 IdspID8:             3;      /* [8:6]: IDSP ID8 */
        UINT32 IdspID9:             3;      /* [11:9]: IDSP ID9 */
        UINT32 VoutID0:             3;      /* [14:12]: VOUT ID0 */
        UINT32 VoutID1:             3;      /* [17:15]: VOUT ID1 */
        UINT32 VoutID2:             3;      /* [20:18]: VOUT ID2 */
        UINT32 VoutID3:             3;      /* [23:21]: VOUT ID3 */
        UINT32 VoutID4:             3;      /* [26:24]: VOUT ID4 */
        UINT32 VoutID5:             3;      /* [29:27]: VOUT ID5 */
        UINT32 Reserved:            2;      /* [31:30] */
    } Bits;
} AMBA_DBG_PORT_SMEM_SYNC_STATUS2_REG_u;

/*
 * SMEM Debug Port: Decompression Logic Status (SMEM side)
 */
typedef union {
    UINT32  Data;

    struct {
        UINT32 DramChanFetchWait:   8;      /* [7:0]: DRAM Channel Fetch Waiting on Stream N */
        UINT32 DummyFetchWait:      1;      /* [8]: Dummy Fetch Waiting */
        UINT32 StreamEnable:        1;      /* [9]: Stream Enabled with Size 0 */
        UINT32 Reserved:            10;     /* [19:10]: Reserved */
        UINT32 IndicateIgnore:      12;     /* [31:20]: Indicates to Ignore Certain Types of Errors */
    } Bits;
} AMBA_DBG_PORT_SMEM_DECOMP_STATUS0_REG_u;

/*
 * SMEM Debug Port: Decompression EOF Status (SMEM side)
 */
typedef union {
    UINT32  Data;

    struct {
        UINT32 EarlyEos:            8;      /* [7:0]:  Early EOS - Stream N Finished Before Unit Reached EOF*/
        UINT32 LateEos:             8;      /* [15:8]: Late EOS ??Unit N Reached EOF Before Stream Finished */
        UINT32 EarlyEof:            8;      /* [23:16]: Early EOF ??DRAM Channel Reached EOF Before Unit N */
        UINT32 LateEof:             8;      /* [31:24]: Late EOF ??Unit N Reached EOF Before DRAM Channel */
    } Bits;
} AMBA_DBG_PORT_SMEM_DECOMP_STATUS1_REG_u;

/*
 * SMEM Debug Port: Decompression Error Status
 */
typedef union {
    UINT32  Data;

    struct {
        UINT32 StreamOutputError:   8;      /* [7:0]: Stream N signalled an output error */
        UINT32 UnitNotFed:          8;      /* [15:8]: Stream N has input data available, but unit is not being fed */
        UINT32 UnitNotProducing:    8;      /* [23:16]: Unit N is being fed data, but it is not producing output */
        UINT32 StreamNoActivity:    8;      /* [31:24]: DRAM channel fetch is waiting on stream N, but there is no activity in that stream */
    } Bits;
} AMBA_DBG_PORT_SMEM_DECOMP_STATUS2_REG_u;

/*
 * SMEM Debug Port: SMEM Error Status
 */
typedef union {
    UINT32  Data;

    struct {
        UINT32 Reserved:            1;      /* [0] */
        UINT32 SameDmaError:        1;      /* [1]: Top and Bottom Clip Occurred in Same DMA */
        UINT32 SmvmaxLimitError:    1;      /* [2]: DMA Reached Smvmax Limit */
        UINT32 FieldDmaError:       1;      /* [3]: one of Srow, Drowmax, Scliprow,Rowinc Odd in Field Dma */
        UINT32 ChannelNotFull:      1;      /* [4]: WM Vflip Set When Channel is Not Full Frame */
        UINT32 SbmWidthError:       1;      /* [5]: Generated SBM DRAM Transfer Wider than Scolmaxs */
        UINT32 Reserved0:           2;      /* [7:6] */
        UINT32 ChannelDupe:         1;      /* [8]: More than the maximum Transfers on Same DRAM Channel */
        UINT32 Reserved1:           3;      /* [11:9] */
        UINT32 IllegalIdspSync:     1;      /* [12]: Illegal IDSP SYNC Event */
        UINT32 IllegalVoutSync:     1;      /* [13]: Illegal VOUT SYNC Event */
        UINT32 Reserved3:           18;     /* [31:14] */
    } Bits;
} AMBA_DBG_PORT_SMEM_ERROR_STATUS_REG_u;

/*
 * SMEM Debug Port: SMEM Error Info
 */
typedef union {
    UINT32  Data;

    struct {
        UINT32 FirstErrorStatus:    13;     /* [12:0]: Client;Channel;ID of First Error */
        UINT32 Reserved:            3;      /* [15:13] */
        UINT32 FirstErrorID:        4;      /* [19:16]: Error ID of First Error */
        UINT32 Reserved1:           11;     /* [30:20] */
        UINT32 ErrorInfoValid:      1;      /* [31]: Error Info is Valid */
    } Bits;
} AMBA_DBG_PORT_SMEM_ERROR_INFO_REG_u;

/*
 * SMEM Debug Port All Registers
 */
typedef struct {
    volatile UINT32                                     SmemChanSub0[384];      /* 0x0000 - 0x05FC: Smem Channel Register, Subregister0 */
    volatile UINT32                                     SmemChanSub1[384];      /* 0x1000 - 0x15FC: Smem Channel Register, Subregister1 */
    volatile UINT32                                     SmemChanSub2[384];      /* 0x2000 - 0x25FC: Smem Channel Register, Subregister2 */
    volatile UINT32                                     SmemChanSub3[384];      /* 0x3000 - 0x35FC: Smem Channel Register, Subregister3 */
    volatile UINT32                                     SmemChanSub4[384];      /* 0x4000 - 0x45FC: Smem Channel Register, Subregister4 */
    volatile UINT32                                     SmemChanSub5[384];      /* 0x5000 - 0x55FC: Smem Channel Register, Subregister5 */
    volatile UINT32                                     SmemChanSub6[384];      /* 0x6000 - 0x65FC: Smem Channel Register, Subregister6 */
    volatile UINT32                                     SmemChanSub7[384];      /* 0x7000 - 0x75FC: Smem Channel Register, Subregister7 */
} AMBA_DBG_PORT_SMEM_REG_s;

#endif /* AMBA_DEBUG_PORT_SMEM_H */
