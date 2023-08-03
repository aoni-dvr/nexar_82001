/**
*  @file AmbaDSP_ContextUtility.h
*
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
 *
*  @details Definitions & Constants for the APIs of SSP Encoder
*
*/

#ifndef AMBADSP_CONTEXTUTILITY_H
#define AMBADSP_CONTEXTUTILITY_H

#include <AmbaVer.h>
#include "AmbaDSP_Priv.h"
#include "AmbaDSP_VOUT.h"
#include "AmbaDSP_VideoEnc.h"
#include "AmbaDSP_VideoDec.h"
#include "AmbaDSP_Liveview.h"
#include "AmbaDSP_StillCapture.h"
#include "AmbaDSP_MsgDispatcher.h"

/* Context for Main */
// DspOpStatus
#define DSP_OP_STATUS_ACTIVE            (0x0U)
#define DSP_OP_STATUS_ACTIVE_2_SUSPEND  (0x1U)
#define DSP_OP_STATUS_SUSPEND           (0x2U)
#define DSP_OP_STATUS_SUSPEND_2_ACTIVE  (0x3U)
#define DSP_OP_STATUS_UNKNOWN           (0x4U)

// RescState
#define HL_RESC_CONFIGED                (0x0U)
#define HL_RESC_SETTLED                 (0x1U)

typedef struct {
    /* General */
    AMBA_DSP_VERSION_INFO_s DspVer;
    ULONG  ChipInfoAddr; //Address comes from dsp_buffer.c
    UINT32 DspSysState;
    UINT32 DspOpStatus;
    UINT8 RescState;
    /*
     * CV2/CV22/CV25/H32 Thread
     *   PlayBack
     *     th[0]:   Boot/VOUT/PostP
     *     th[1~3]: none
     *     th[4]:   dproc, use when stitch mode
     *     th[5]:   proxy
     *     th[6]:   coding cabac/cavlc
     *     th[7]:   schedule
     *   Camera
     *     th[0]:   Boot/VOUT
     *     th[1]:   VIN
     *     th[2~3]: IDSP
     *     th[4]:   VPROC
     *     th[5]:   proxy
     *     th[6]:   coding cabac/cavlc
     *     th[7]:   schedule
     */
    UINT32 DspDbgThrd;
    UINT32 DspDbgLvl;
    UINT32 DspDbgMod;
    UINT32 DspDbgThrdValid;
    UINT32 ProfWorkSize;
    ULONG  VpMsgCtrlBufAddr;

    /* Boot */
    AMBA_DSP_SYS_CONFIG_s DspSysCfg;
    UINT32 ParLoadEn:1;
    UINT32 ParLoadRsvd:31;
#define PARLOAD_IDX_LEN  (8U) //Num of bit every region index has in Mask
    UINT32 ParLoadMask; //B[0:7]VOUT B[8:15]LV B[16:23]ENC B[24:31]DEC

    /* Clock */
    UINT32 IdspClk;
    UINT32 CoreClk;
    UINT32 DramClk;
    UINT32 AudioClk;

    /* Liveview */
    UINT32 VideoPipe[AMBA_DSP_MAX_VIEWZONE_NUM];
    UINT16 LowDelayMode;
    UINT16 MaxVinBit;
    UINT16 MaxViewZoneNum;
    UINT16 MaxVinVirtChanBit[AMBA_DSP_MAX_VIN_NUM];
    UINT16 MaxVinVirtChanOutputNum[AMBA_DSP_MAX_VIN_NUM][AMBA_DSP_MAX_VIRT_CHAN_NUM];
    UINT16 MaxVinVirtChanWidth[AMBA_DSP_MAX_VIN_NUM][AMBA_DSP_MAX_VIRT_CHAN_NUM];
    UINT16 MaxVinVirtChanHeight[AMBA_DSP_MAX_VIN_NUM][AMBA_DSP_MAX_VIRT_CHAN_NUM];
    UINT8  MctfEnable;
    UINT8  EffectLogicBufNum;
#define EFFECT_BUF_MASK_DEPTH (4U)
    UINT32 EffectLogicBufMask[EFFECT_BUF_MASK_DEPTH]; // There are at most 20x4 output buffers, use 128 bits to store it
    UINT16 VprocGrpNum;
    UINT16 VprocGrpFreeRunBit;

    /* ExtMem */
    UINT16 RawAllocType[AMBA_DSP_MAX_VIN_NUM];
    UINT16 RawBufNum[AMBA_DSP_MAX_VIN_NUM];
    AMBA_DSP_RAW_BUF_s RawBuf[AMBA_DSP_MAX_VIN_NUM];
    ULONG RawBufTbl[AMBA_DSP_MAX_VIN_NUM][MAX_EXT_DISTINCT_DEPTH];
    AMBA_DSP_RAW_BUF_s CeBuf[AMBA_DSP_MAX_VIN_NUM];
    ULONG CeBufTbl[AMBA_DSP_MAX_VIN_NUM][MAX_EXT_DISTINCT_DEPTH];

    /* Default Raw and Aux buffer */
    AMBA_DSP_RAW_BUF_s DefaultRawBuf[AMBA_DSP_MAX_VIN_NUM];
    AMBA_DSP_RAW_BUF_s DefaultAuxRawBuf[AMBA_DSP_MAX_VIN_NUM];

    UINT32 MaxVinBootTimeout[AMBA_DSP_MAX_VIN_NUM];
    UINT32 MaxVinTimeout[AMBA_DSP_MAX_VIN_NUM];
    UINT16 MaxVinShortFrameThrd[AMBA_DSP_MAX_VIN_NUM];
    UINT16 MaxRaw2YuvDelay[AMBA_DSP_MAX_VIN_NUM];
    UINT16 MaxWarpDma[AMBA_DSP_MAX_VIEWZONE_NUM];
    UINT16 MaxWarpWaitLineLuma[AMBA_DSP_MAX_VIEWZONE_NUM];
    UINT16 MaxWarpWaitLineChroma[AMBA_DSP_MAX_VIEWZONE_NUM];
    UINT16 MaxHierWidth[AMBA_DSP_MAX_VIEWZONE_NUM];
    UINT16 MaxHierHeight[AMBA_DSP_MAX_VIEWZONE_NUM];
    UINT16 MaxLndtWidth[AMBA_DSP_MAX_VIEWZONE_NUM];
    UINT16 MaxLndtHeight[AMBA_DSP_MAX_VIEWZONE_NUM];
    UINT16 MaxHorWarpComp[AMBA_DSP_MAX_VIEWZONE_NUM];
    UINT16 MaxVirtVinBit;   //User domain
    UINT16 MaxVirtVinOutputNum[AMBA_DSP_MAX_VIRT_VIN_NUM];
    UINT16 MaxVirtVinWidth[AMBA_DSP_MAX_VIRT_VIN_NUM];
    UINT16 MaxVirtVinHeight[AMBA_DSP_MAX_VIRT_VIN_NUM];

    UINT16 VinBit; //Phy+Virt
    UINT16 ViewZoneNum;
    UINT16 YuvStrmNum;
    UINT32 ViewZoneActiveBit;
    UINT32 ViewZoneDisableBit;

    /* Encode */
    UINT16 MaxEncodeStream;
    UINT8  MaxGopM[AMBA_DSP_MAX_STREAM_NUM];
    UINT8  MaxExtraReconNum[AMBA_DSP_MAX_STREAM_NUM];
    UINT8  MaxSearchRange[AMBA_DSP_MAX_STREAM_NUM];
#define ENC_STRM_FMT_AVC_BIT_IDX        (0U)
#define ENC_STRM_FMT_AVC_LEN            (1U)
#define ENC_STRM_FMT_HEVC_BIT_IDX       (1U)
#define ENC_STRM_FMT_HEVC_LEN           (1U)
#define ENC_STRM_FMT_JPG_BIT_IDX        (2U)
#define ENC_STRM_FMT_JPG_LEN            (1U)
#define ENC_STRM_FMT_OSDSYNC_BIT_IDX    (4U)
#define ENC_STRM_FMT_OSDSYNC_LEN        (1U)
#define ENC_STRM_FMT_ROTATE_BIT_IDX     (5U)
#define ENC_STRM_FMT_ROTATE_LEN         (1U)
#define ENC_STRM_FMT_HIER_P_BIT_IDX     (6U)
#define ENC_STRM_FMT_HIER_P_LEN         (1U)
#define ENC_STRM_FMT_TIMELAPSE_BIT_IDX  (7U)
#define ENC_STRM_FMT_TIMELAPSE_LEN      (1U)

    UINT8  MaxStrmFmt[AMBA_DSP_MAX_STREAM_NUM];
    UINT16 MaxExtMemWidth[AMBA_DSP_MAX_STREAM_NUM];
    UINT16 MaxExtMemHeight[AMBA_DSP_MAX_STREAM_NUM];
    UINT16 MaxOsdSyncDly[AMBA_DSP_MAX_STREAM_NUM];

    /* Still */
    UINT8  MaxProcessFormat;
    UINT16 MaxStlVinWidth[AMBA_DSP_MAX_VIN_NUM][AMBA_DSP_MAX_VIRT_CHAN_NUM];
    UINT16 MaxStlVinHeight[AMBA_DSP_MAX_VIN_NUM][AMBA_DSP_MAX_VIRT_CHAN_NUM];
    UINT16 MaxStlMainWidth;
    UINT16 MaxStlMainHeight;
    UINT16 MaxStlRawInputWidth;
    UINT16 MaxStlRawInputHeight;
    UINT16 MaxStlYuvInputWidth;
    UINT16 MaxStlYuvInputHeight;
    UINT16 MaxStlYuvEncWidth;
    UINT16 MaxStlYuvEncHeight;

    /* Vout */
    UINT16 MaxVoutWidth[AMBA_DSP_MAX_VOUT_NUM];
    UINT16 MaxOsdBufWidth[AMBA_DSP_MAX_VOUT_NUM];
    UINT8 VoutBit;          //default 0xFF
    UINT8 VoutRotate[AMBA_DSP_MAX_VOUT_NUM];

    /* Decode */
    UINT16 DecMaxStreamNum;
    UINT16 DecMaxStrmFmt[AMBA_DSP_MAX_DEC_STREAM_NUM];
    UINT16 DecMaxFrameWidth;
    UINT16 DecMaxFrameHeight;
    UINT32 DecMaxBitRate;
    UINT16 DecMaxVideoPlaneWidth;
    UINT16 DecMaxVideoPlaneHeight;
    UINT16 DecMaxRatioOfGopNM;      /* Bit[3:0] means MaximumGopM */
    UINT16 DecBackwardTrickPlayMode;
    UINT16 DecMaxPicWidth;
    UINT16 DecMaxPicHeight;

    /* TimeLapse */
    UINT16 MaxTimeLapseNum;

    /* Max Tile in vproc, define smem resource */
    UINT16 MaxVprocTileWidth;
    UINT16 MinVprocTileNum;
    UINT16 MaxVprocTileNum;
    UINT16 MaxVprocTileOverlap;

    /*
     * Common buffer link status
     * CV series/H32 have 4 common buffers for effect
     * they are PostMain/PostPip/PostComm0/PostComm1
     * PostMain/A can link to Encode
     * PostA/B/C can link to vout
     * Mask is bit-wise of YuvStrmIdx
     */
    UINT32 PostPinLinkMask[DSP_VPROC_PIN_NUM];
} CTX_RESOURCE_INFO_s;

/* Context for Dsp instance */
typedef struct {
    UINT8 MaxVirtVinNum;    // AMBA_DSP_MAX_VIRT_VIN_NUM
    UINT8 MaxVpocNum;       // AMBA_DSP_MAX_VIEWZONE_NUM
    UINT8 Rsvd[2U];

    /* Virtual Vin */
    UINT32 VirtVinBitMask;  // Bit-wise to indicate which VirtVin is occupied

    /* Vproc */
    UINT32 VprocBitMask;    // Bit-wise to indicate which Vproc is occupied
#define VPROC_PURPOSE_USER_DEFINED  (0U)
#define VPROC_PURPOSE_STILL         (1U)
    UINT8  VprocPurpose[AMBA_DSP_MAX_VIEWZONE_NUM]; //Vproc purpose

    /* Decode shared VirtVinId */
    UINT16 DecSharedVirtVinId;
} CTX_DSP_INST_INFO_s;

/* Context fot Sensor */
typedef struct {
    UINT8   FieldFormat;
    UINT8   BitResolution;
    UINT8   BayerPattern;
    UINT32  ReadoutMode;
} CTX_SENSOR_INFO_s;

typedef struct {
    UINT16 Status;

    UINT32 PinUsage[DSP_VPROC_PIN_NUM];   //Pin occupied status, every bit indicates YuvStrmIdx
    UINT32 PinUsageOnStrm[DSP_VPROC_PIN_NUM][AMBA_DSP_MAX_YUVSTRM_NUM];   //Pin occupied status on StrmIdx
                                                                          //Every bit indicates ChanIdx
    UINT8  PinUsageNumOnStrm[DSP_VPROC_PIN_NUM][AMBA_DSP_MAX_YUVSTRM_NUM];   //Pin occupied duplicated number on StrmIdx
                                                                             //Every bit indicates ChanIdx
                                                                             //Happened when using same output size
    UINT8  IsSetPinExtMem[DSP_VPROC_PIN_NUM];    //ext_mem on demand, for yuv capture/feed/y2y

    UINT16 MaxTileWidth;    // derived after VprocCfgCmd : TileW = MaxW/MaxTile
    UINT8  VprocTileMode;
    UINT8  VprocInfoRsvd;
} CTX_VPROC_INFO_s;

/* Context for ViewZone */
// FlowBypassOption
#define FLOW_BYPASS_VIN_IDX             (0U)
#define FLOW_BYPASS_R2Y_IDX             (1U)
#define FLOW_BYPASS_VWARP_IDX           (2U)
#define FLOW_BYPASS_MCTF_IDX            (3U)
#define FLOW_BYPASS_MCTS_IDX            (4U)
#define FLOW_BYPASS_MCTS_DRAM_OUT_IDX   (5U) // can't encode if we turn off Mcts when cv2xfs

#define VIN_SRC_FROM_HW             (0U) // from VinHW, see VinInfo from AmbaVin
#define VIN_SRC_FROM_DRAM           (1U) // from dram as CFA format
#define VIN_SRC_FROM_DRAM_422       (2U) // from dram as Yuv422 format
#define VIN_SRC_FROM_DRAM_420       (3U) // from dram as Yuv420 format
#define VIN_SRC_FROM_DEC            (4U) // from decoder
#define VIN_SRC_FROM_RECON          (5U) // from encode recon frame
#define VIN_SRC_NUM                 (6U)

#define PYMD_BUF_STATUS_UNKNOWN     (0U)
#define PYMD_BUF_STATUS_CONFIG      (1U)
#define PYMD_BUF_STATUS_ALLOCATED   (2U)

#define LNDT_BUF_STATUS_UNKNOWN     (0U)
#define LNDT_BUF_STATUS_CONFIG      (1U)
#define LNDT_BUF_STATUS_ALLOCATED   (2U)

#define INT_MAIN_BUF_STATUS_UNKNOWN     (0U)
#define INT_MAIN_BUF_STATUS_CONFIG      (1U)
#define INT_MAIN_BUF_STATUS_ALLOCATED   (2U)

#define YUVSTRM_BUF_STATUS_UNKNOWN      (0U)
#define YUVSTRM_BUF_STATUS_CONFIG       (1U)
#define YUVSTRM_BUF_STATUS_ALLOCATED    (2U)

#define EFCT_SYNC_OFF               (0U) // no effect sync running
#define EFCT_SYNC_ON                (1U) // start effect sync
#define EFCT_SYNC_PROCESS           (2U) // effect sync processing

#define VIEWZONE_VIN_NORMAL         (0U)
#define VIEWZONE_VIN_TIMEOUT        (1U)

#define MAX_RAW_SEQ_Q               (16U)
#define MAX_RAW_SEQ_Q_MASK          (15U)
typedef struct {
    UINT32 RawCapSeq[MAX_RAW_SEQ_Q];
    UINT64 RawCapSeq64[MAX_RAW_SEQ_Q];
    UINT64 RawCapSeqBase;
    UINT32 RawCapPts[MAX_RAW_SEQ_Q];
    UINT64 RawCapPts64[MAX_RAW_SEQ_Q];
    UINT64 RawCapPtsBase;
    UINT32 DonePts[MAX_RAW_SEQ_Q];
    UINT64 DonePts64[MAX_RAW_SEQ_Q];
    UINT64 DonePtsBase;
    UINT8  RawSeqWp;
} CTX_YUV_INPUT_CTRL_s;

typedef struct {
    UINT32 CeSetup:1;
    UINT32 HdrSetup:1;
    UINT32 SetFovLayout:1;
    UINT32 Rsvd:29;

    AMBA_DSP_WINDOW_s       CapWindow;

    /* For VinInfo */
    UINT16                  Option[AMBA_DSP_MAX_VIRT_CHAN_NUM];
    UINT16                  IntcNum[AMBA_DSP_MAX_VIRT_CHAN_NUM];

    /* For ViewzoneInfo */
    UINT16                  SourceVin;
    UINT16                  HdrBlendHeight;
    UINT16                  HdrRawYOffset[VIN_HDR_MAX_EXP_NUM];
    UINT16                  Rsvd1[4U - VIN_HDR_MAX_EXP_NUM];
    UINT8                   HdrBlendNumMinusOne;
    UINT8                   LinearCe;
    UINT8                   InputFromMemory;
    UINT8                   Rsvd2;

} CTX_CMD_NEED_UPDATE_s;

#define VIEWZONE_VIN_VIRT_BIT_IDX       (0U)
#define VIEWZONE_VIN_TD_BIT_IDX         (1U)
#define VIEWZONE_VIN_DECI_BIT_IDX       (2U)
#define VIEWZONE_PROC_RAW_BIT_IDX       (3U)
#define VIEWZONE_INPUT_MUXSEL_BIT_IDX   (4U)
#define VIEWZONE_UPSTRM_SRC_BIT_IDX     (5U)
#define VIEWZONE_POSTSTRM_BIT_IDX       (6U)
#define VIEWZONE_ATTR_BIT_NUM           (7U)
#define DSP_VIEWZONE_IDX_INVALID        (0xFFFFU)
typedef struct {
    UINT16                  SourceVin; //bitwise
#define VIEWZONE_SRC_TD_IDX_NULL    (0xFFU)
    UINT8                   SourceTdIdx; //TimeDiviosnGrpIndex
    UINT8                   InputFromMemory; //indicate input source is external memory
    UINT32                  FlowBypassOption; // b[0]:Vin, b[1]:R2y B[2]:vWarp, b[3]:Mctf, b[4]:Mcts
    UINT8                   LinearCe;
    UINT8                   HdrBlendNumMinusOne;
    UINT16                  HdrBlendHeight;
    AMBA_DSP_WINDOW_s       CapWindow;
    AMBA_DSP_WINDOW_s       PinWindow[DSP_VPROC_PIN_NUM];
    AMBA_DSP_WINDOW_s       PinMaxWindow[DSP_VPROC_PIN_NUM];    //not set in LvCfg
    AMBA_DSP_WINDOW_s       Main;
#define DSP_MCTF_OFF_IDX    (0U)
#define DSP_MCTF_OFF_LEN    (1U)
#define DSP_MCTF_CMPR_IDX   (6U)
#define DSP_MCTF_CMPR_LEN   (2U)
    UINT8                   MctfCmpr;
#define DSP_MCTS_OFF_IDX        (0U)
#define DSP_MCTS_OFF_LEN        (1U)
#define DSP_MCTS_DRAM_OUT_IDX   (7U)
#define DSP_MCTS_DRAM_OUT_LEN   (1U)
    UINT8                   VinDecimationRate; // 0 is same as 1
    UINT8                   MainReportRate;                     //no used
    UINT8                   MainRotate;
    UINT32                  StartIsoCfgIndex;                   //not set in LvCfg, in HL_FillVinStartCfg
    ULONG                   StartIsoCfgAddr;                    //not set in LvCfg, in HL_FillVinStartCfg
    UINT8                   StartIsoCtxIndex;                   //not set in LvCfg, in HL_FillVinStartCfg
    UINT8                   StartHdrCfgIndex;                   //not set in LvCfg, in UpdateIsoCfg
    UINT8                   UsedStartIsoCfg;                    //not set in LvCfg, in HL_LvFeedRawOnSndData/HL_LvFeedYuvDataOnMem
                                                                //the first iso-cfg has been used, only for mem_input viewzone
    UINT8                   IsEffectChanMember;                 //set in LvCfg yuvstrm

    AMBA_DSP_LIVEVIEW_PYRAMID_s Pyramid;
    /* Pyramid Ext memory */
    UINT8   PymdBufferStatus;
    UINT8   PymdIsExtMem;
    UINT16  PymdAllocType;
    UINT16  PymdBufNum;
    AMBA_DSP_YUV_IMG_BUF_s PymdYuvBuf;
    ULONG   PymdYuvBufTbl[MAX_EXT_DISTINCT_DEPTH];

    AMBA_DSP_LIVEVIEW_LANE_DETECT_s LaneDetect;
    /* LaneDetect Ext memory */
    UINT8   LndtBufferStatus;
    UINT8   LndtIsExtMem;
    UINT16  LndtAllocType;
    UINT16  LndtBufNum;
    AMBA_DSP_YUV_IMG_BUF_s LndtYuvBuf;
    ULONG   LndtYuvBufTbl[MAX_EXT_DISTINCT_DEPTH];

    /* IntMain Ext memory */
    UINT8   IntMainBufferStatus;
    UINT8   IntMainIsExtMem;
    UINT16  IntMainAllocType;
    UINT16  IntMainBufNum;
    AMBA_DSP_YUV_IMG_BUF_s IntMainBuf;
    ULONG   IntMainBufTbl[MAX_EXT_DISTINCT_DEPTH];

    ULONG                   LastIdspCfgAddr;                    //not set in LvCfg, in MsgDispatcher

    /* BatchCmdQ */
    DSP_POOL_DESC_s         BatchQPoolDesc;                     //not set in LvCfg

    /* SideBand */
    UINT8                   SidebandNum;                        //not set in LvCfg, in LvSideBandUpdate
    UINT8                   SidebandAddrIdx;                    //not set in LvCfg, in LvSideBandUpdate

    /* Yuv Input Info */
    CTX_YUV_INPUT_CTRL_s    YuvInput;                           //not set in LvCfg, in LvCtrl
    CTX_YUV_INPUT_CTRL_s    PymdInput;                          //not set in LvCfg, in LvCtrl
    CTX_YUV_INPUT_CTRL_s    IntMainInput;                       //not set in LvCfg, in LvCtrl

    /* GroupSync */
    UINT8                   EffectSyncState;                    //not set in LvCfg, in UpdateIsoCfg

    /* HDR Setting */
    UINT16                  HdrRawYOffset[VIN_HDR_MAX_EXP_NUM]; //not set in LvCfg, in HL_FillVinHdrSetup

    /* Fov Cmd Update control */
    CTX_CMD_NEED_UPDATE_s   CmdUpdate;                          //not set in LvCfg, multiple place

    /* VinAttachEvent control */
#define DSP_VZ_ATTCH_PIN_NUM    (DSP_VPROC_PIN_NUM + 1U)
#define DSP_PROC_RAW_PIN_ID     (DSP_VPROC_PIN_NUM)
    UINT32                  AttachEventCountDown[DSP_VZ_ATTCH_PIN_NUM];    //not set in LvCfg, StlXXX
    UINT32                  ExtraEventMask[DSP_VZ_ATTCH_PIN_NUM];          //not set in LvCfg, StlXXX

    /* Slice */
    UINT8                   SliceNumCol;                        //not set in LvCfg, in LvSliceCfg
    UINT8                   SliceNumRow;                        //not set in LvCfg, in LvSliceCfg
    UINT16                  WarpLumaWaitLine;                   //not set in LvCfg, in LvSliceCfg
    UINT16                  WarpChromaWaitLine;                 //not set in LvCfg, in LvSliceCfg
    UINT16                  WarpOverLap;                        //not set in LvCfg, in LvSliceCfg
    UINT16                  VinDragLine;                        //not set in LvCfg, in LvSliceCfg
#define ENC_SYNC_ENABLE_IDX    (0U)
    UINT8                   EncSyncOpt; //B[0]EncSync, MJPG and PrevB only, not in LvCfg, in LvSliceCfg

    /* Process Postpone */
    UINT32                  ProcPostPoneTime[AMBA_DSP_VZ_POSTPONE_STAGE_NUM];   //100usec, not set in LvCfg, in LvVzPPCfg

    UINT8                   PinIsExtMem; //BitMask from DSP_VPROC_PIN_NUM, indicate this Pin create ExtMem
    UINT8                   PinComposeExtMem; //BitMask from DSP_VPROC_PIN_NUM, indicate this Pin will compose ExtMem

    /* PipeLine */
    UINT8                   Pipe;

    /* External yuv feeding */
    UINT8                   CurrentBind;            /* 0xFF means not used */
    UINT32                  YuvFrameCnt;
    UINT16                  ExtYuvBufIdx;
    AMBA_DSP_YUV_IMG_BUF_s  ExtYuvBuf;

    /* ExtraDramOut */
    UINT8                   IsProcRawDramOut;

    /* Timeout re-order */
    UINT8                   TimeoutState;
    UINT8                   IsMaster;       //current master vz

    /* Input Mux Select */
#define VZ_IN_MUXSEL_SRC_BIT_IDX        (30U)
#define VZ_IN_MUXSEL_SRC_LEN            (2U)
#define VZ_IN_MUXSEL_HOR_DEC_BIT_IDX    (10U)
#define VZ_IN_MUXSEL_VER_DEC_BIT_IDX    (0U)
#define VZ_IN_MUXSEL_DEC_LEN            (10U)
    UINT8                   IsInputMuxSel;
    UINT8                   InputMuxSelSrc;
    UINT16                  InputMuxSelHorDeci;
    UINT16                  InputMuxSelVerDeci;

    /* UpStreamSource */
#define VZ_UPSTRMSRC_TYPE_BIT_IDX        (28U)
#define VZ_UPSTRMSRC_TYPE_LEN            (4U)
#define VZ_UPSTRMSRC_DATA_BIND_IDX       (26U)
#define VZ_UPSTRMSRC_DATA_BIND_LEN       (2U)
#define VZ_UPSTRMSRC_IDX_BIT_IDX         (0U)
#define VZ_UPSTRMSRC_IDX_LEN             (16U)
    UINT8                   IsUpStrmSrc;
    UINT8                   IsPostStrm;
#define VZ_UPSTRMSRC_TYPE_FROM_VZ        (0U)
    UINT8                   UpStrmSrcType;
    UINT16                  UpStrmSrcId;
    UINT8                   UpStrmDataBind;

    /* IkCfg */
    UINT32                  IkSensorMode;
    UINT32                  IkCompression;
    UINT8                   IkPipe;
    UINT32                  IkStlPipe;
    UINT32                  IkId;
    ULONG                   IkIsoAddr;

    /* Vproc Grouping */
    UINT8                   VprocGrpId;
} CTX_VIEWZONE_INFO_s;

/* Context for Yuv Stream */
typedef struct {
    UINT16 NumChan; // how many channel composite this stream
    UINT16 CopyJobNum[AMBA_DSP_MAX_YUVSTRM_VIEW_NUM];
    AMBA_DSP_LIVEVIEW_CHANNEL_WINDOW_s ChanCfg[AMBA_DSP_MAX_YUVSTRM_VIEW_NUM];
    AMBA_DSP_BUF_s BlendCfg[AMBA_DSP_MAX_YUVSTRM_VIEW_NUM][DSP_MAX_PP_STRM_BLEND_NUMBER];
    DSP_EFFECT_COPY_JOB_s CopyJobDesc[AMBA_DSP_MAX_YUVSTRM_VIEW_NUM][DSP_MAX_PP_STRM_COPY_NUMBER];
    DSP_EFFECT_BLEND_JOB_s BlendJobDesc[AMBA_DSP_MAX_YUVSTRM_VIEW_NUM][DSP_MAX_PP_STRM_BLEND_NUMBER_CTX];
    DSP_EFFECT_BUF_DESC_s EffectBufDesc;
    UINT32 ChanLinked; //bit-wise, represent which ChannelIndex is linked to YuvStream
    UINT16 CurrentMasterViewId;
} CTX_YUV_STRM_LAYOUT_s;

#define DSP_SRC_BUF_IDX_INVALID (0xFFFFU) //indicate YuvStream don't need SrcBufIdx
typedef struct {
    UINT16 Purpose;
    UINT16 DestVout;
    UINT32 DestEnc;
    UINT16 Width;
    UINT16 Height;
    UINT16 MaxWidth;
    UINT16 MaxHeight;
    CTX_YUV_STRM_LAYOUT_s Layout;

    /* for PpStrmCmd @ MaxUcode */
    DSP_POOL_DESC_s PpStrmPoolDesc;

    /* All possible channel for YuvStream, BitMask for ViewZone index
     */
    UINT32 MaxChanBitMask;
    /* The maximal number of channels at the same time
     * which means this YuvStrm has potential for being effect-channel
     * Currently, only support 16(AMBA_DSP_MAX_YUVSTRM_VIEW_NUM) at most
     */
    UINT16 MaxChanNum;
    /* Max possible channel size, every entry indicates one ViewZone
     */
    AMBA_DSP_WINDOW_DIMENSION_s MaxChanWin[AMBA_DSP_MAX_YUVSTRM_VIEW_NUM];
    UINT32 ChanBitMask;

    /* H22 use SrcBuf */
    UINT16 DspSrcBufIdx; //use in Issac uCode

    /* Stream from Ext memory */
    UINT8  IsExtMem;
    UINT16 AllocType;
    UINT16 BufNum;
    AMBA_DSP_YUV_IMG_BUF_s YuvBuf;
    ULONG  YuvBufTbl[MAX_EXT_DISTINCT_DEPTH];
    UINT8  BufferStatus;

    UINT32 DestDeciRate;
#define YUVSTRM_OPT_EFFECT_IDX (0U)
#define YUVSTRM_OPT_EFFECT_LEN (4U)
    UINT32 OptimizeOption:4;
    UINT32 Reserved:28;

    UINT16 ChanIdxArray[AMBA_DSP_MAX_YUVSTRM_VIEW_NUM];
} CTX_YUV_STRM_INFO_s;

/* Context for Yuv Stream EffectSync Ctrl */
/* Effect Sync Job description */
#define EFCT_SYNC_JOB_ID_DONTCARE       (0xFFFFU)

#define EFCT_SYNC_JOB_SUBID_LENGTH      (5U)
#define EFCT_SYNC_JOB_TYPE_LENGTH       (8U)
#define EFCT_SYNC_JOB_TYPE_SHIFT_BIT    (5U)
#define EFCT_SYNC_JOB_SEQ_LENGTH        (4U)
#define EFCT_SYNC_JOB_SEQ_SHIFT_BIT     (20U)
#define EFCT_SYNC_JOB_YUVSTRM_LENGTH    (6U)
#define EFCT_SYNC_JOB_YUVSTRM_SHIFT_BIT (24U)
#define EFCT_SYNC_JOB_ID_TYPE_LENGTH    (2U)
#define EFCT_SYNC_JOB_ID_TYPE_SHIFT_BIT (30U)

#define EFCT_SYNC_JOB_BIT_DISPLAY       (0U) // Osd/Vout
#define EFCT_SYNC_JOB_BIT_GEO           (1U) // Warp/CA
#define EFCT_SYNC_JOB_BIT_LAYOUT        (2U) // PpStrm
#define EFCT_SYNC_JOB_BIT_VINROI        (3U) // fov layout
#define EFCT_SYNC_JOB_BIT_NUM           (4U)

typedef struct {
    UINT32 SubJobIdx:5; // Shall start from 1
    UINT32 JobTypeBit:8;
    UINT32 Rsvd:7;
    UINT32 SeqIdx:4;    // Shall start from 1
    UINT32 YuvStrmId:6;
    UINT32 Type:2; //[0]SyncJobId, [1]BatchId
} DSP_EFCT_SYNC_JOB_ID_s;

typedef struct {
    UINT32 PoolId:10;
    UINT32 ViewZoneId:16;
    UINT32 Rsvd:4;
    UINT32 RsvdForSyncJobId:2;
} DSP_BATCH_CMD_ID_s;

typedef struct {
  UINT32 CmdCode;
  UINT32 Content[(CMD_SIZE_IN_BYTE / 4U) - 1U];
} DSP_CMD_s;

typedef struct {
    DSP_EFCT_SYNC_JOB_ID_s JobId;

    UINT32 WarpUpdate:1;
    UINT32 CaWarpUpdate:1;
    UINT32 PrevSizeUpdate:1;
    UINT32 GeoUpdateRsvd:29;

    /* Display Job */
    UINT8 VoutIdx;
    UINT8 OsdRsvd;
    /* Geo Job */
    UINT16 ViewZoneIdx;

    DSP_CMD_s WarpCmd;
    DSP_CMD_s CaWarpCmd;
} DSP_EFCT_SYNC_JOB_s;

#define EFCT_SYNC_JOB_STATUS_BATCH_BIT  (15U)    //MasterBatchJob

#define EFCT_SYNC_JOBSET_STATUS_IDLE    (0U)    // all job done
#define EFCT_SYNC_JOBSET_STATUS_CONFIG  (1U)    // job added or job be filled into cmdQ
#define EFCT_SYNC_JOBSET_STATUS_PROC    (2U)    // job sent to DSP
#define EFCT_SYNC_JOBSET_STATUS_NUM     (3U)
typedef struct {
    DSP_EFCT_SYNC_JOB_ID_s JobId;
    UINT8  JobNum; //0xFF means all JobDone and JobBuffer removed
    UINT8  Status;
    UINT16 JobStatus; //BitMask, 1 means Configed, 0 means finished(Rcv Id from ucode), B[15] indicates MasterBatchJob
    DSP_EFCT_SYNC_JOB_s Job[MAX_GROUP_CMD_SUBJOB_NUM];

    /* Display Job, only few(AMBA_DSP_MAX_VOUT_NUM) display placement in one JobSet */
    UINT8 OsdEnable[AMBA_DSP_MAX_VOUT_NUM];
    AMBA_DSP_VOUT_OSD_BUF_CONFIG_s OsdCfg[AMBA_DSP_MAX_VOUT_NUM];

    /* Layout Job, only one layout placement in one JobSet */
    /* Move Layout out of job set */
    /* Since job set is init everytime in sync start */
    /* We need to keep layout as reference to check prev size is changed or not, compared to last job */
    /* Sometimes user didn't update layout in one job, but sync start will reset the Layout into zero */
    // CTX_YUV_STRM_LAYOUT_s Layout;

    /* VIN ROI */
    AMBA_DSP_WINDOW_s VinRoi[AMBA_DSP_MAX_YUVSTRM_VIEW_NUM];
} CTX_EFCT_SYNC_JOB_SET_s;

typedef struct {
    /* EffectSync */
    UINT8  State;
    UINT8  Rsvd[3U];

    DSP_POOL_DESC_s PoolDesc;
} CTX_YUVSTRM_EFCT_SYNC_CTRL_s;

typedef struct {
    UINT8  VinNeedReset;
    UINT8  VinState;
    UINT32 RawCapSeq[MAX_RAW_SEQ_Q];
    UINT64 RawCapSeq64[MAX_RAW_SEQ_Q];
    UINT64 RawCapSeqBase;
    UINT32 RawCapPts[MAX_RAW_SEQ_Q];
    UINT64 RawCapPts64[MAX_RAW_SEQ_Q];
    UINT64 RawCapPtsBase;
    UINT64 VinVprocDelayPts64[MAX_RAW_SEQ_Q];
    UINT8  RawSeqWp;
} CTX_VIDEO_INPUT_CTRL_s;

#define STL_PROC_JOB_ID_DONTCARE       (0xFFFFU)
typedef struct {
    UINT32 DataFmt:2;   // 0 : YUV420, 1:Yuv422, 2:GenAAA
    UINT32 OutputPin:6; // BitWise, DSP_VPROC_PIN_NUM
    UINT32 ExtMem:1; // store result to ExtMem
    UINT32 VprocId:5;
    UINT32 Rsvd:18;
} DSP_STL_PROC_JOB_ID_s;

/* Context for Vin */
#define VIN_IDX_INVALID     (0xFFFFU)
#define VIN_VIRT_IDX        (15U)
typedef struct {
    UINT8                   SensorNum; //current we assume only one sensor info
    UINT8                   SensorIndexTable[AMBA_DSP_MAX_VIN_SENSOR_NUM];
    UINT8                   InputFormat; // VCAP_INPUT_FORMAT
    UINT8                   CfaCompressed;
    UINT8                   CfaCompact;
    UINT8                   AuxCmprFromCapCfg;
    UINT8                   AuxCfaCompressed;
    UINT8                   AuxCfaCompact;
    UINT8                   SensorMode;  //[0]CFA [1]RGB-IR [2]RCCB [32] YUV
    UINT8                   SkipFrame;
    AMBA_DSP_FRAME_RATE_s   FrameRate;
    AMBA_DSP_WINDOW_s       CapWindow[AMBA_DSP_MAX_VIRT_CHAN_NUM];
    UINT8                   PostCfgType;

    /* Vin state control */
    CTX_VIDEO_INPUT_CTRL_s  VinCtrl;

    /* Vin Cfg control */
    UINT8                   CompandEna;
    UINT8                   DeCompandEna;
    ULONG                   CompandTableAddr;
    ULONG                   DeCompandTableAddr;

    /* Virtual Channel */
    UINT32                  SubChanBit;
    UINT16                  Option[AMBA_DSP_MAX_VIRT_CHAN_NUM];
    UINT16                  ConCatNum[AMBA_DSP_MAX_VIRT_CHAN_NUM];
    UINT16                  IntcNum[AMBA_DSP_MAX_VIRT_CHAN_NUM];

    /* Vin Cmd Update control */
    CTX_CMD_NEED_UPDATE_s   CmdUpdate;

    /* Time Division */
    UINT16                  TimeDivisionNum[AMBA_DSP_MAX_VIRT_CHAN_NUM];
    UINT16                  TimeDivisionFrmNum[AMBA_DSP_MAX_VIRT_CHAN_NUM][AMBA_DSP_MAX_VIN_TD_NUM];
    UINT16                  TimeDivisionVinId[AMBA_DSP_MAX_VIRT_CHAN_NUM][AMBA_DSP_MAX_VIN_TD_NUM];

    /* Vin Attach Event check */
    UINT32                  AttachEventCountDownCheck; // BitWise, ViewZoneLocalIndexOnVin

    /* Vin ext_mem */
    UINT8                   IsSetPinExtMem;

    /* StlCap */
    UINT8                   StlCfaCmpr;

    UINT16                  ProcDeciRate;
} CTX_VIN_INFO_s;

typedef struct {
    UINT32 BufId:8; // start from 1
    UINT32 StrmId:8; // encode stream id
    UINT32 Rsvd:16;
} DSP_OSD_SYNC_JOB_s;

/* Context for Enc */
#define ENC_STRM_CFG_NONE    (0U)   //after stream config take effect
#define ENC_STRM_CFG_UPDATE  (1U)   //after SSP stream config API
#define ENC_STRM_CFG_DONE    (2U)   //after stream config cmd issued
typedef struct {
    UINT8  CfgStatus;
    UINT8  StartMethod;
    UINT8  StopMethod;
#define ENC_STRM_EXTMEM_IDX (0xFFFFU)
    UINT16 SourceYuvStrmId;
    UINT8  VideoTimeLapseActive;
    UINT8  EnableSPSAndPPSReport;
    UINT8  BitsIndex;
    ULONG  BitsStartAddr;
    UINT32 BitsBufSize;
    ULONG  BitsBufAddr;
    UINT64 TotalEncodeByte;
    AMBA_DSP_WINDOW_s Window; //encode window
    AMBA_DSP_FRAME_RATE_s FrameRate;
    UINT8  Rotate;
    UINT32 FrmRateDivisor; // 0 means no divisor

    /* From EncodeConfig */
    UINT8  ProfileIDC;
    UINT8  LevelIDC;
    UINT8  IsCabac;
    UINT8  CodingFmt;
    AMBA_DSP_VIDEO_GOP_CONFIG_s GOPCfg;
    UINT8  TierIDC;
    UINT8  NumPRef;
    UINT8  NumBRef;
    UINT8  FirstGOPStartB;
    AMBA_DSP_VIDEO_DEBLOCK_CTRL_s DeblockFilter;
    AMBA_DSP_VIDEO_FRAME_CROP_CTRL_s FrameCrop;
    AMBA_DSP_VIDEO_ENC_VUI_s Vui;
    AMBA_DSP_VIDEO_BIT_RATE_CFG_s BitRateCfg;
    UINT8  NumSlice;
    UINT8  NumTile;
    UINT8  NumExtraRecon;
    AMBA_DSP_VIDEO_INTRA_REFRESH_CTRL_s IntraRefreshCtrl;
    UINT8  PicOrderCntType;
    UINT8  SPSPPSHeaderInsertMode;
    UINT8  AUDMode;
    UINT8  NonRefPIntvl;
    UINT8  InsertSeiUserData; // 0: no user data buffer 1: 128Byte user data
    ULONG  QuantMatrixAddr;
    AMBA_DSP_JPEG_BIT_RATE_CTRL_s JpgBitRateCtrl;

    /* From EncRealTimeCfg */
#define ENC_RT_CTRL_IDLE        (0U)
#define ENC_RT_CTRL_UPDATED     (1U)
#define ENC_RT_CTRL_ISSUED      (2U)
#define ENC_RT_CTRL_PROCESSED   (3U)
    UINT32 BitRateCtrlStatus:2;
    UINT32 GopCtrlStatus:2;
    UINT32 QpCtrlStatus:2;
    UINT32 RoiCtrlStatus:2;
    UINT32 ActThrdCtrlStatus:2;
    UINT32 IsMJPG:1;
    UINT32 Rsvd:21;
    AMBA_DSP_VIDEO_QP_CONFIG_s QpCfg;
    AMBA_DSP_VIDEO_ROI_CONFIG_s RoiCfg;
    AMBA_DSP_VIDEO_ACT_THRD_CONFIG_s ActThrdCfg;

    /* TimeLapse */
#define DSP_TIMELAPSE_IDX_INVALID  (0xFFFFU) //indicate TimeLapseId is not needed
    UINT16 TimeLapseId;   //CTX_TIMELAPSE_INFO_s index

    /* MV */
    ULONG  MvBufBase;
    UINT32 MvBufSize;
    UINT32 MvBufUnitSize;
    UINT32 MvOption;
    ULONG  ActBufBase;
    UINT32 ActBufSize;
    UINT32 ActBufUnitSize;

    /* Encode option */
    UINT8 PicRdyInfoOpt;  // indicate CaptureTimeStamp type on the AMBA_DSP_ENC_PIC_RDY_s

    /* For EncodeBlend */
    UINT8 EmbedCode;
#ifdef SUPPORT_VPROC_OSD_INSERT
    AMBA_DSP_VIDEO_BLEND_CONFIG_s BldCfg[DSP_MAX_OSD_BLEND_AREA_NUMBER];
    DSP_POOL_DESC_s OsdInsrtBufPool;
    UINT32 OsdSyncBufUserData[DSP_MAX_OSD_INSERT_BUF_NUM];
#endif
} CTX_STREAM_INFO_s;

/* Context for VOUT */
// DisplayStatus
#define DSP_VOUT_DISPLAY_NO_INIT   (0x0U)
#define DSP_VOUT_DISPLAY_RESET     (0x1U)
#define DSP_VOUT_DISPLAY_START     (0x2U)

// DveStatus
#define DSP_VOUT_DVE_NO_INIT   (0x0U)
#define DSP_VOUT_DVE_RESET     (0x1U)
#define DSP_VOUT_DVE_CFGED     (0x2U)
#define DSP_VOUT_DVE_START     (0x3U)

// MixerStatus
#define DSP_VOUT_MIXER_NO_INIT   (0x0U)
#define DSP_VOUT_MIXER_START     (0x1U)

// OsdStatus
#define DSP_VOUT_OSD_NO_INIT   (0x0U)

typedef struct {
    UINT8  DisplayStatus;   //current status
    UINT8  MixerStatus;
    UINT8  OsdStatus;
    UINT8  DveStatus; //follow Display reset status

    AMBA_DSP_VOUT_MIXER_CONFIG_s MixerCfg;
    UINT8  MixerHighlightThreshold; //compare luma
    UINT8  MixerCscCtrl;
    UINT32 MixerBGColor; //YUV 888
    UINT32 MixerHighlightColor; //YUV888
    UINT32 CscMatrix[DSP_VOUT_CSC_MATRIX_DEPTH];

    AMBA_DSP_VOUT_OSD_BUF_CONFIG_s OsdCfg;
    UINT8  OsdEnable;

    AMBA_DSP_DISPLAY_CONFIG_s DispCfg;
    UINT8  GammaEnable;
    ULONG  GammaTableAddr;

    AMBA_DSP_VOUT_VIDEO_CFG_s VideoCfg;
    UINT8  VideoEnable;     //user desire, updated by outside APIs
    UINT8  Source;

    UINT8  DveMode;

    UINT8  CscUpdated;
    UINT8  CscType;

    AMBA_DSP_VOUT_DATA_PATH_CFG_s DataPath;

    UINT32 StartPts[MAX_RAW_SEQ_Q];
    UINT64 StartPts64[MAX_RAW_SEQ_Q];
    UINT64 StartPtsBase;
    UINT32 DonePts[MAX_RAW_SEQ_Q];
    UINT64 DonePts64[MAX_RAW_SEQ_Q];
    UINT64 DonePtsBase;
    UINT8  PtsWp;
} CTX_VOUT_INFO_s;

/* Context for Encode */
typedef struct {
    UINT16  ActiveBit;
} CTX_ENCODER_INFO_s;

/* Context for Decoder */
typedef struct {
    UINT8  DecoderStatus;       /* DSP_DEC_OPM_INVALID:0 DSP_DEC_OPM_IDLE:1 DSP_DEC_OPM_RUN:2 */
    UINT8  ErrorState;
    UINT8  DecoderID;
    ULONG  BitsBufAddrNext;
    UINT32 DecPicNum;           /* Decoded picture number */
    UINT32 DisPicNum;
    UINT64 DisplayPTS;
} CTX_DECODER_INFO_s;

/* Context for VidDec */
#define DECODE_MODE_INVALID     (0U)
#define DECODE_MODE_VIDEO       (1U)
#define DECODE_MODE_STILL       (2U)
#define DECODE_MODE_XCODE       (3U)
#define DECODE_MODE_NUM         (4U)

#define VIDDEC_STATE_INVALID    (0U)
#define VIDDEC_STATE_READY      (1U)
#define VIDDEC_STATE_OPENED     (2U)
#define VIDDEC_STATE_RUN        (3U)
#define VIDDEC_STATE_PAUSE      (4U)
#define VIDDEC_STATE_NUM        (5U)
typedef struct {
    UINT8                       State;                  /* VIDDEC_STATE_NUM */
    UINT8                       DecoderMode;            /* DECODE_MODE_NUM */
    UINT16                      StreamID;
    UINT16                      BitsFormat;
    ULONG                       BitsBufAddr;
    UINT32                      BitsBufSize;
    UINT16                      MaxFrameWidth;          /* 0 - use default value */
    UINT16                      MaxFrameHeight;         /* 0 - use default value */
    UINT8                       XcodeMode;              /* AMBA_DSP_XCODE_ */
    UINT16                      XcodeWidth;
    UINT16                      XcodeHeight;
    UINT16                      MaxVideoBufferWidth;    /* Max buffer width */
    UINT16                      MaxVideoBufferHeight;   /* Max buffer height */
    AMBA_DSP_VIDDEC_POST_CTRL_s PostCtlCfg[AMBA_DSP_MAX_VOUT_NUM];  /* Decode Info for PostP */
    UINT8                       PostpOpCfgEnable;       /*READY:1 OPENED:2 RUN:3 PAUSE:4*/
    UINT8                       DprocOpCfgEnable;       /*READY:1 OPENED:2 RUN:3 PAUSE:4*/
    UINT32                      DspState;               /* dec_op_mode_t */
    UINT16                      YuvInVirtVinId;         /* 0xFFFF means not used */
    UINT16                      ViewZoneId;             /* 0xFFFF means not used */
    UINT8                       CurrentBind;            /* 0xFF means not used */
    UINT32                      YuvFrameCnt;
    UINT16                      ExtYuvBufIdx;
    AMBA_DSP_YUV_IMG_BUF_s      ExtYuvBuf;
    AMBA_DSP_YUV_IMG_BUF_s      LastYuvBuf;
    UINT32                      LastYuvBufWidth;
    UINT32                      LastYuvBufHeight;
    UINT8                       FirstIsoCfgIssued;      /* Only care when Dec2Vproc case */
} CTX_VID_DEC_INFO_s;

/* Context for Still */
#define DSP_VIRT_VIN_IDX_INVALID    (0xFFFFU) //indicate VIRT_VIN is not needed
#define DSP_VPROC_IDX_INVALID       (0xFFFFU) //indicate VPROC is not needed
typedef struct {
    /* VirtualVin */
    UINT16 RawInVirtVinId;  //0xFFFF means not used
    UINT16 YuvInVirtVinId;  //0xFFFF means not used

    UINT16 YuvEncVirtVinId; //0xFFFF means not used
    UINT16 RawInVprocId;    //0xFFFF means not used

    UINT16 YuvInVprocId;    //0xFFFF means not used
    UINT16 EncStmId;        //0xFFFF means not used

#define STL_VPROC_STATUS_IDLE       (0x0U)
#define STL_VPROC_STATUS_CONFIG     (0x1U)
#define STL_VPROC_STATUS_RUN        (0x2U)
#define STL_VPROC_STATUS_COMPLETE   (0x3U)
#define STL_VPROC_STATUS_STOPPED    (0x4U)
    UINT8  RawInVprocStatus;
    UINT8  YuvInVprocStatus;
    UINT8  YuvInVprocPin;
    UINT8  RawInVprocPin;
#define STL_ENC_STATUS_IDLE         (0x0U)
#define STL_ENC_STATUS_CONFIG       (0x1U)
#define STL_ENC_STATUS_RUN          (0x2U)
#define STL_ENC_STATUS_COMPLETE     (0x3U)
    UINT8  EncStatus;

#ifndef SUPPORT_VPROC_RT_RESTART
    UINT16 Yuv422InVprocId;
    UINT8  Yuv422InVprocStatus;
    UINT8  rsvd1;
#endif

    /* IkCfg */
    UINT32 IkDolOfstY[VIN_HDR_MAX_EXP_NUM];
    UINT32 IkSensorMode;
    UINT32 IkCompression;
} CTX_STILL_INFO_s;

/* Context for DataCap */
#define DATA_CAP_STATUS_IDLE    (0U)
#define DATA_CAP_STATUS_UPDATED (1U)
#define DATA_CAP_STATUS_RUNNING (2U)
#define DATA_CAP_STATUS_2_RUN   (3U)  // for synced yuv
#define DATA_CAP_STATUS_2_STOP  (4U)  // stop running capture
typedef struct {
    UINT8  Status;
    UINT8  VprocIdle;
    UINT8  rsvd[2U];
    UINT32 CountDown;

    UINT8  BufMemType;      //indicate what memory type is used for carry this Buffer, NUM_VPROC_EXT_MEM_TYPE
    UINT8  AuxBufMemType;   //indicate what memory type is used for carry this Buffer, NUM_VPROC_EXT_MEM_TYPE
    UINT8  rsvd1[2U];
    AMBA_DSP_DATACAP_CFG_s Cfg;
    ULONG  BufTbl[MAX_EXT_DISTINCT_DEPTH];
    ULONG  AuxBufTbl[MAX_EXT_DISTINCT_DEPTH];
} CTX_DATACAP_INFO_s;

/* Context for BatchQ info */
#define BATCHQ_INFO_ISO_CFG                 (0U)
#define BATCHQ_INFO_IMG_PRMD                (1U)
#define BATCHQ_INFO_YUVSTRM_GRP             (2U)
#define BATCHQ_INFO_STL_PROC                (3U)
typedef struct {
    UINT32 IsoCfgId;
    UINT32 ImgPrmdId;
    UINT32 YuvStrmGrpId;
    UINT32 StlProcId;
} CTX_BATCHQ_INFO_s;

/* Context for TimeLaspe */
typedef struct {
    /* VirtualVin */
    UINT16 VirtVinId;      //0xFFFF means not used
    UINT16 StrmId;
    UINT64 TotalIssuedMemEncodeNum;

#define ENC_TIMELAPSE_STATUS_IDLE       (0x0U)
#define ENC_TIMELAPSE_STATUS_CONFIG     (0x1U)
#define ENC_TIMELAPSE_STATUS_RUN        (0x2U)
#define ENC_TIMELAPSE_STATUS_COMPLETE   (0x3U)
    UINT8  Status;
    UINT8  rsvd0[3U];

    AMBA_DSP_YUV_IMG_BUF_s  LastYuvBuf;
    ULONG  LastMe1BufAddr;
} CTX_TIMELAPSE_INFO_s;

/* Context for Encode group */
typedef struct {
    UINT16 Purpose;
    UINT16 StrmNum;

    UINT8  Stride;
    UINT8  MonoIncrement;
    UINT8  Rsvd[2U];

    ULONG  RcCfgAddr;
#define DSP_ENC_GRP_STRM_IDX_INVALID        (0xFFFFU)
    UINT16 StrmIdx[AMBA_DSP_MAX_ENC_GRP_STRM_NUM];
} CTX_ENC_GRP_INFO_s;

extern CTX_RESOURCE_INFO_s *HL_CtxResInfoPtr;
extern CTX_VIEWZONE_INFO_s *HL_CtxViewZoneInfoPtr;
extern CTX_YUV_STRM_INFO_s *HL_CtxYuvStrmInfoPtr;
extern CTX_YUVSTRM_EFCT_SYNC_CTRL_s *HL_CtxYuvStrmEfctSyncCtrlPtr;
extern CTX_EFCT_SYNC_JOB_SET_s *HL_CtxYuvStrmEfctSyncJobSetPtr;
extern CTX_YUV_STRM_LAYOUT_s *HL_CtxYuvStrmEfctLayoutPtr;
extern CTX_VPROC_INFO_s *HL_CtxVprocInfoPtr;
extern CTX_STREAM_INFO_s *HL_CtxStrmInfoPtr;

extern UINT32 HL_CtxInit(void);
extern void HL_CtxLvDataPathReset(UINT32 ViewZoneOI);
extern void HL_CtxLvReset(void);
#ifdef __SUPPROT_IWONG_UCODE__
extern void HL_GetPointerToDspSrcBufCfg(UINT16 Index, DSP_SRCBUF_CFG **pInfo);
#endif
extern void HL_GetPointerDspEncStrmCfg(UINT16 Index, DSP_ENC_CFG **pInfo);
#ifdef SUPPORT_MAX_UCODE
extern void HL_GetPointerToDspVinStartFovCfg(UINT16 Index, UINT16 LocalIndex, vin_fov_cfg_t **pInfo);
extern void HL_GetPointerToDspVinFovLayoutCfg(UINT16 Index, UINT16 LocalIndex, fov_layout_t **pInfo);
extern void HL_GetPointerToDspFovSideBandCfg(UINT16 ViewZoneId, UINT16 TokenIndex, sideband_info_t **pInfo);
extern void HL_GetPointerToDspPpStrmCtrlCfg(UINT16 YuvStrmIdx, UINT16 BufIdx, UINT16 ChanIdx, vproc_pp_stream_cntl_t **pInfo);
extern void HL_GetPointerToDspPpStrmCopyCfg(UINT16 YuvStrmIdx, UINT16 BufIdx, UINT16 ChanIdx, vproc_post_r2y_copy_cfg_t **pInfo);
extern void HL_GetPointerToDspPpStrmBldCfg(UINT16 YuvStrmIdx, UINT16 BufIdx, UINT16 ChanIdx, vproc_y2y_blending_cfg_t **pInfo);
//extern void HL_GetPointerToDspPpDummyBldTbl(UINT8 **pInfo);
extern void HL_GetPointerToDspMaxEncResolCfg(UINT16 **pInfo);
extern void HL_GetPointerToDspBatchCmdSet(UINT16 ViewZoneId, UINT16 Idx, vin_fov_batch_cmd_set_t **pInfo);
extern void HL_GetPointerToDspBatchQ(UINT16 ViewZoneId, UINT16 BatchIdx, UINT32 **pInfo, UINT32 *pBatchCmdId);
extern void HL_GetBatchCmdId(UINT16 ViewZoneId, UINT16 BatchIdx, UINT32 *pBatchCmdId);
extern void HL_ResetDspBatchQ(const UINT32 *pBatchQ);
extern void HL_GetPointerToDspGroupCmdQ(UINT16 YuvStrmId, UINT16 SeqIdx, UINT16 SubJobIdx, UINT32 **pInfo);
extern void HL_GetPointerToDspViewZoneGroupCmdQ(UINT16 ViewZoneId, UINT16 SeqIdx, UINT32 **pInfo);
extern void HL_GetPointerToDspExtRawBufArray(UINT16 VinIdx, UINT16 Type, UINT32 **pInfo);
extern void HL_UpdateCtxViewZoneBatchQRPtr(const UINT32 BatchCmdId);
extern void HL_SetDspBatchQInfo(UINT32 BatchCmdId, UINT8 AutoReset, UINT8 InfoIdx, UINT32 InfoData);
extern void HL_GetDspBatchQInfo(UINT32 BatchCmdId, CTX_BATCHQ_INFO_s *BatchQInfo);
extern void HL_GetDspBatchCmdId(ULONG BatchCmdQAddr, UINT16 ViewZoneId, UINT32 *pBatchCmdId);
extern void HL_CtrlBatchQBufMtx(const UINT8 MtxOpt, UINT16 ViewZoneId);
extern void HL_GetPointerToDspVprocGrpNum(UINT16 GroupId, UINT8 **pInfo);
extern void HL_GetPointerToDspVprocGrpOrd(const UINT16 GroupId, const UINT16 OrderId, UINT8 **pInfo);
extern void HL_GetPointerToDspVprocOrder(const UINT16 GroupId, const UINT16 OrderId, UINT8 **pInfo);
extern void HL_GetPointerToDspExtStlBufArray(UINT8 StageId, UINT32 **pInfo);
extern void HL_GetPointerToDspStlBatchQ(UINT8 StageId, UINT32 **pInfo);
extern void HL_GetPointerToDspExtPymdBufArray(const UINT16 ViewZoneId, UINT32 **pInfo);
extern void HL_GetPointerToDspExtLndtBufArray(const UINT16 ViewZoneId, UINT32 **pInfo);
extern void HL_GetPointerToDspExtIntMainBufArray(const UINT16 ViewZoneId, UINT32 **pInfo);
extern void HL_GetPointerToDspExtYuvStrmBufArray(const UINT16 YuvStrmId, UINT32 **pInfo);
extern void HL_GetPointerToDspExtYuvStrmAuxBufArray(const UINT16 YuvStrmId, UINT32 **pInfo);
extern void HL_GetPointerToDspExtVinRawBufArray(const UINT16 VinId, UINT32 **pInfo);
extern void HL_GetPointerToDspExtVinAuxBufArray(const UINT16 VinId, UINT32 **pInfo);
extern void HL_GetPointerToDspDataCapBufArray(const UINT16 Id, UINT32 **pInfo);
extern void HL_GetPointerToDspDataCapAuxBufArray(const UINT16 Id, UINT32 **pInfo);
#endif
//extern void HL_GetResource(const UINT8 MtxOpt, CTX_RESOURCE_INFO_s *pInfo);
//extern void HL_SetResource(const UINT8 MtxOpt, const CTX_RESOURCE_INFO_s *pInfo);
extern void HL_GetResourcePtr(CTX_RESOURCE_INFO_s **pInfo);
extern void HL_GetResourceLock(CTX_RESOURCE_INFO_s **pInfo);
extern void HL_GetResourceUnLock(void);
extern void HL_GetDspInstance(const UINT8 MtxOpt, CTX_DSP_INST_INFO_s *pInfo);
extern void HL_SetDspInstance(const UINT8 MtxOpt, const CTX_DSP_INST_INFO_s *pInfo);
extern void HL_GetSensorInfo(const UINT8 MtxOpt, UINT16 Index, CTX_SENSOR_INFO_s *pInfo);
extern void HL_SetSensorInfo(const UINT8 MtxOpt, UINT16 Index, const CTX_SENSOR_INFO_s *pInfo);
extern void HL_GetVinInfo(const UINT8 MtxOpt, UINT16 Index, CTX_VIN_INFO_s *pInfo);
extern void HL_SetVinInfo(const UINT8 MtxOpt, UINT16 Index, CTX_VIN_INFO_s *pInfo);
//extern void HL_GetViewZoneInfo(const UINT8 MtxOpt, UINT16 Index, CTX_VIEWZONE_INFO_s *pInfo);
//extern void HL_SetViewZoneInfo(const UINT8 MtxOpt, UINT16 Index, CTX_VIEWZONE_INFO_s *pInfo);
extern void HL_GetViewZoneInfoLock(UINT16 Index, CTX_VIEWZONE_INFO_s **pInfo);
extern void HL_GetViewZoneInfoUnLock(UINT16 Index);
extern void HL_GetViewZoneInfoPtr(UINT16 Index, CTX_VIEWZONE_INFO_s **pInfo);
extern UINT8 HL_IsDec2Vproc(void);
extern UINT8 HL_HasDec2Vproc(void);
extern UINT8 HL_HasDecResource(void);
//extern void HL_GetYuvStrmInfo(const UINT8 MtxOpt, UINT16 Index, CTX_YUV_STRM_INFO_s *pInfo);
extern void HL_GetYuvStrmInfoLock(UINT16 Index, CTX_YUV_STRM_INFO_s **pInfo);
extern void HL_GetYuvStrmInfoUnLock(UINT16 Index);
extern void HL_GetYuvStrmInfoPtr(const UINT16 Index, CTX_YUV_STRM_INFO_s **pInfo);
//extern void HL_SetYuvStrmInfo(const UINT8 MtxOpt, UINT16 Index, const CTX_YUV_STRM_INFO_s *pInfo);
extern void HL_GetYuvStrmEfctSyncCtrl(const UINT8 MtxOpt, UINT16 Index, CTX_YUVSTRM_EFCT_SYNC_CTRL_s *pInfo);
#if 0
extern void HL_SetYuvStrmEfctSyncCtrl(const UINT8 MtxOpt, UINT16 Index, const CTX_YUVSTRM_EFCT_SYNC_CTRL_s *pInfo);
#endif
extern void HL_GetYuvStrmEfctSyncCtrlLock(const UINT16 Index, CTX_YUVSTRM_EFCT_SYNC_CTRL_s **pInfo);
extern void HL_GetYuvStrmEfctSyncCtrlUnLock(const UINT16 Index);
#if 0
extern void HL_GetYuvStrmEfctSyncJobSet(const UINT8 MtxOpt, UINT16 YuvStrmIdx, UINT16 JobIdx, CTX_EFCT_SYNC_JOB_SET_s *pInfo);
extern void HL_SetYuvStrmEfctSyncJobSet(const UINT8 MtxOpt, UINT16 YuvStrmIdx, UINT16 JobIdx, const CTX_EFCT_SYNC_JOB_SET_s *pInfo);
#endif
extern void HL_GetYuvStrmEfctSyncJobSetLock(const UINT16 YuvStrmIdx, const UINT16 JobIdx, CTX_EFCT_SYNC_JOB_SET_s **pInfo);
extern void HL_GetYuvStrmEfctSyncJobSetLayoutLock(const UINT16 YuvStrmIdx, const UINT16 JobIdx, CTX_YUV_STRM_LAYOUT_s **pInfo);
extern void HL_GetYuvStrmEfctSyncJobSetUnLock(const UINT16 YuvStrmIdx, const UINT16 JobIdx);
extern UINT32 HL_GetYuvStrmEfctSyncJobSetChanCfg(const UINT16 YuvStrmIdx,
                                                 const UINT16 JobIdx,
                                                 const UINT16 ViewZoneId,
                                                 AMBA_DSP_LIVEVIEW_CHANNEL_WINDOW_s **ChanCfg);
extern void HL_GetVprocInfo(const UINT8 MtxOpt, UINT16 Index, CTX_VPROC_INFO_s *pInfo);
extern void HL_SetVprocInfo(const UINT8 MtxOpt, UINT16 Index, const CTX_VPROC_INFO_s *pInfo);
extern void HL_GetVprocInfoPtr(const UINT16 Index, CTX_VPROC_INFO_s **pInfo);
extern void HL_GetVprocInfoLock(UINT16 Index, CTX_VPROC_INFO_s **pInfo);
extern void HL_GetVprocInfoUnLock(UINT16 Index);

extern void HL_GetStrmInfo(const UINT8 MtxOpt, UINT16 Index, CTX_STREAM_INFO_s *pInfo);
extern void HL_SetStrmInfo(const UINT8 MtxOpt, UINT16 Index, const CTX_STREAM_INFO_s *pInfo);
extern void HL_GetStrmInfoLock(UINT16 Index, CTX_STREAM_INFO_s **pInfo);
extern void HL_GetStrmInfoUnLock(UINT16 Index);
extern void HL_GetStrmInfoPtr(const UINT16 Index, CTX_STREAM_INFO_s **pInfo);

extern void HL_GetVoutInfo(const UINT8 MtxOpt, UINT16 Index, CTX_VOUT_INFO_s *pInfo);
extern void HL_SetVoutInfo(const UINT8 MtxOpt, UINT16 Index, const CTX_VOUT_INFO_s *pInfo);
extern void HL_GetEncoderInfo(const UINT8 MtxOpt, CTX_ENCODER_INFO_s *pInfo);
extern void HL_SetEncoderInfo(const UINT8 MtxOpt, const CTX_ENCODER_INFO_s *pInfo);
extern void HL_GetDecoderInfo(const UINT8 MtxOpt, CTX_DECODER_INFO_s *pInfo);
extern void HL_SetDecoderInfo(const UINT8 MtxOpt, const CTX_DECODER_INFO_s *pInfo);
extern void HL_GetVidDecInfo(const UINT8 MtxOpt, UINT16 Index, CTX_VID_DEC_INFO_s *pInfo);
extern void HL_SetVidDecInfo(const UINT8 MtxOpt, UINT16 Index, const CTX_VID_DEC_INFO_s *pInfo);
extern void HL_GetStlInfo(const UINT8 MtxOpt, CTX_STILL_INFO_s *pInfo);
extern void HL_SetStlInfo(const UINT8 MtxOpt, const CTX_STILL_INFO_s *pInfo);
extern void HL_GetDataCapInfo(const UINT8 MtxOpt, UINT16 Index, CTX_DATACAP_INFO_s *pInfo);
extern void HL_SetDataCapInfo(const UINT8 MtxOpt, UINT16 Index, const CTX_DATACAP_INFO_s *pInfo);
extern void HL_GetTimeLapseInfo(const UINT8 MtxOpt, UINT16 Index, CTX_TIMELAPSE_INFO_s *pInfo);
extern void HL_SetTimeLapseInfo(const UINT8 MtxOpt, UINT16 Index, const CTX_TIMELAPSE_INFO_s *pInfo);
extern void HL_GetEncGrpInfo(const UINT8 MtxOpt, UINT16 Index, CTX_ENC_GRP_INFO_s *pInfo);
extern void HL_SetEncGrpInfo(const UINT8 MtxOpt, UINT16 Index, const CTX_ENC_GRP_INFO_s *pInfo);
extern UINT8 HL_GetRescState(void);
extern void HL_SetRescState(UINT8 RescState);
extern void HL_ResetViewZoneInfo(void);
extern void HL_ResetYuvStreamInfo(void);
extern void HL_ResetYuvStreamInfoPP(void);
extern void HL_ResetYuvStrmEfctSyncCtrl(void);
extern void HL_ResetYuvStrmEfctSyncJobSet(void);
extern void HL_ResetVprocInfo(void);

extern const UINT8 HL_DecCodec2DspDecCodec[AMBA_DSP_DEC_BITS_FORMAT_NUM];

#define HL_MTX_OPT_ALL  (0x0U) //lock/unlock mutex
#define HL_MTX_OPT_GET  (0x1U) //lock mutex
#define HL_MTX_OPT_SET  (0x2U) //unlock mutex
#define HL_MTX_OPT_NONE (0x3U) //mutex safe
#define MUTEX_TIMEOUT       (0xFFFFFFFFU)

extern UINT8 HL_IS_ROTATE(UINT8 RotateFlip);
extern UINT8 HL_IS_HFLIP(UINT8 RotateFlip);
extern UINT8 HL_IS_VFLIP(UINT8 RotateFlip);
//extern UINT8 HL_ROTATE_FLIP_COMPOSITE(UINT8 Rotate, UINT8 Flip);
extern UINT8 HL_GET_ROTATE(UINT8 RotateFlip);
extern UINT8 HL_GET_FLIP(UINT8 RotateFlip);
extern UINT32 HL_FRAME_RATE_MAP(const UINT8 Type, AMBA_DSP_FRAME_RATE_s Frate, UINT32 *NewFrate);
//extern UINT32 HL_VOUT_YUV_COMPOSITE(UINT8 Y, UINT8 U, UINT8 V);
#define VOUT_GET_Y  (2U)
#define VOUT_GET_U  (1U)
#define VOUT_GET_V  (0U)
extern UINT8 HL_VOUT_GET_YUV(const UINT8 Type, UINT32 Yuv);

#if defined (CONFIG_THREADX)
extern void AmbaVer_SetDspDspKernelHL(AMBA_VerInfo_s *pVerInfo);
#endif

extern void HL_AcqCmdBuffer(UINT8 *Id, void **BufferAddr);
extern void HL_RelCmdBuffer(const UINT8 Id);

extern cmd_dsp_config_t *HL_DefCtxCmdBufPtrSys;
extern cmd_dsp_hal_inf_t *HL_DefCtxCmdBufPtrHal;
extern cmd_dsp_suspend_profile_t *HL_DefCtxCmdBufPtrSus;
extern cmd_set_debug_level_t *HL_DefCtxCmdBufPtrDbgLvl;
extern cmd_print_th_disable_mask_t *HL_DefCtxCmdBufPtrDbgthd;
extern cmd_binding_cfg_t *HL_DefCtxCmdBufPtrBind;
extern cmd_dsp_activate_profile_t *HL_DefCtxCmdBufPtrActPrf;
extern cmd_dsp_vproc_flow_max_cfg_t *HL_DefCtxCmdBufPtrVpcMax;
extern cmd_dsp_vin_flow_max_cfg_t *HL_DefCtxCmdBufPtrVinMax;
extern cmd_dsp_set_profile_t *HL_DefCtxCmdBufPtrSetPrf;
extern cmd_dsp_enc_flow_max_cfg_t *HL_DefCtxCmdBufPtrEncMax;
extern cmd_dsp_dec_flow_max_cfg_t *HL_DefCtxCmdBufPtrDecMax;

extern cmd_vproc_cfg_t *HL_DefCtxCmdBufPtrVpc;
extern cmd_vproc_setup_t *HL_DefCtxCmdBufPtrVpcSetup;
//extern cmd_vproc_mctf_config_t *HL_DefCtxCmdBufPtrVpcMctf;
extern cmd_vproc_img_pyramid_setup_t *HL_DefCtxCmdBufPtrVpcPymd;
extern cmd_vproc_prev_setup_t *HL_DefCtxCmdBufPtrVpcPrev;
extern cmd_vproc_lane_det_setup_t *HL_DefCtxCmdBufPtrVpcLndt;
extern cmd_vproc_set_ext_mem_t *HL_DefCtxCmdBufPtrVpcExtMem;
extern cmd_vproc_stop_t *HL_DefCtxCmdBufPtrVpcStop;
extern cmd_vproc_osd_blend_t *HL_DefCtxCmdBufPtrVpcOsd;
extern cmd_vproc_pin_out_deci_t *HL_DefCtxCmdBufPtrVpcDeci;
//extern cmd_vproc_fov_grp_cmd_t *HL_DefCtxCmdBufPtrVpcFovGrp;
#if defined (CONFIG_ENABLE_DSP_DIAG)
extern cmd_vproc_echo_t *HL_DefCtxCmdBufPtrVpcEcho;
#endif
#ifdef SUPPORT_VPROC_OSD_INSERT
extern cmd_vproc_osd_insert_t *HL_DefCtxCmdBufPtrVpcOsdInsert;
extern cmd_vproc_set_streams_delay_t *HL_DefCtxVprocSetStreamDelay;
#endif
extern cmd_vproc_multi_stream_pp_t *HL_DefCtxCmdBufPtrVpcPp;
//extern cmd_vproc_set_effect_buf_img_sz_t *HL_DefCtxCmdBufPtrVpcEff;
extern cmd_vproc_multi_chan_proc_order_t *HL_DefCtxCmdBufPtrVpcChOrd;
extern cmd_vproc_set_vproc_grping *HL_DefCtxCmdBufPtrVpcGrp;

extern cmd_vin_start_t *HL_DefCtxCmdBufPtrVinStart;
extern cmd_vin_idle_t *HL_DefCtxCmdBufPtrVinIdle;
extern cmd_vin_set_raw_frm_cap_cnt_t *HL_DefCtxCmdBufPtrVinCnt;
extern cmd_vin_set_ext_mem_t *HL_DefCtxCmdBufPtrVinExtMem;
extern cmd_vin_send_input_data_t *HL_DefCtxCmdBufPtrVinSndData;
extern cmd_vin_initiate_raw_cap_to_ext_buf_t *HL_DefCtxCmdBufPtrVinInitCap;
extern cmd_vin_attach_proc_cfg_to_cap_frm_t *HL_DefCtxCmdBufPtrVinAttfrm;
extern cmd_vin_attach_event_to_raw_t *HL_DefCtxCmdBufPtrVinAttRaw;
//extern cmd_vin_cmd_msg_dec_rate_t *HL_DefCtxCmdBufPtrVinMsgRate;
extern cmd_vin_ce_setup_t *HL_DefCtxCmdBufPtrVinCe;
extern cmd_vin_hdr_setup_t *HL_DefCtxCmdBufPtrVinHdr;
extern cmd_vin_set_frm_lvl_flip_rot_control_t *HL_DefCtxCmdBufPtrVinFlip;
extern cmd_vin_set_fov_layout_t *HL_DefCtxCmdBufPtrVinLayout;
//extern cmd_vin_set_frm_vproc_delay_t *HL_DefCtxCmdBufPtrVinDly;
extern cmd_vin_attach_sideband_info_to_cap_frm_t *HL_DefCtxCmdBufPtrVinSidBend;
//extern cmd_vin_vcap_dram_in_setup_t *HL_DefCtxCmdBufPtrVinCapDram;
extern cmd_vin_vout_lock_setup_t *HL_DefCtxCmdBufPtrVinVoutLock;
//extern cmd_vin_attach_metadata_t *HL_DefCtxCmdBufPtrVinAttDta;
extern cmd_vin_temporal_demux_setup_t *HL_DefCtxCmdBufPtrVinTd;

extern cmd_vout_mixer_setup_t *HL_DefCtxCmdBufPtrVoutMx;
extern cmd_vout_video_setup_t *HL_DefCtxCmdBufPtrVoutVdo;
//extern cmd_vout_default_img_setup_t *HL_DefCtxCmdBufPtrVoutDef;
extern cmd_vout_osd_setup_t *HL_DefCtxCmdBufPtrVoutOsd;
//extern cmd_vout_osd_buf_setup_t *HL_DefCtxCmdBufPtrVoutOsdBuf;
extern cmd_vout_osd_clut_setup_t *HL_DefCtxCmdBufPtrVoutOsdClut;
extern cmd_vout_display_setup_t *HL_DefCtxCmdBufPtrVoutDisp;
extern cmd_vout_dve_setup_t *HL_DefCtxCmdBufPtrVoutDve;
extern cmd_vout_reset_t *HL_DefCtxCmdBufPtrVoutReset;
//extern cmd_vout_display_csc_setup_t *HL_DefCtxCmdBufPtrVoutCac;
//extern cmd_vout_digital_output_mode_setup_t *HL_DefCtxCmdBufPtrVoutDigMode;
extern cmd_vout_gamma_setup_t *HL_DefCtxCmdBufPtrVoutGamma;

extern cmd_encoder_setup_t *HL_DefCtxCmdBufPtrEnc;
extern cmd_encoder_start_t *HL_DefCtxCmdBufPtrEncStart;
extern cmd_encoder_stop_t *HL_DefCtxCmdBufPtrEncStop;
extern cmd_encoder_jpeg_setup_t *HL_DefCtxCmdBufPtrEncJpg;
extern ipcam_real_time_encode_param_setup_t *HL_DefCtxCmdBufPtrEncRt;

extern cmd_decoder_setup_t *HL_DefCtxCmdBufPtrDec;
extern cmd_decoder_start_t *HL_DefCtxCmdBufPtrDecStart;
extern cmd_decoder_stop_t *HL_DefCtxCmdBufPtrDecStop;
extern cmd_decoder_bitsfifo_update_t *HL_DefCtxCmdBufPtrDecBsUpt;
extern cmd_decoder_speed_t *HL_DefCtxCmdBufPtrDecSpeed;
extern cmd_decoder_trickplay_t *HL_DefCtxCmdBufPtrDecTrick;
extern cmd_decoder_stilldec_t *HL_DefCtxCmdBufPtrDecStlDec;
extern cmd_decoder_batch_t *HL_DefCtxCmdBufPtrDecBch;
//extern cmd_dproc_setup_t *HL_DefCtxCmdBufPtrDpc;
extern cmd_dproc_yuv_to_yuv_t *HL_DefCtxCmdBufPtrDpcY2y;
extern cmd_dproc_yuv_display_t *HL_DefCtxCmdBufPtrDpcDisp;
//extern cmd_dproc_yuv_paint_t *HL_DefCtxCmdBufPtrDpcPaint;
extern cmd_dproc_vrscl_layout_t *HL_DefCtxCmdBufPtrDpcLayout;
extern cmd_dproc_vrscl_start_t *HL_DefCtxCmdBufPtrDpcStart;
extern cmd_dproc_vrscl_stop_t *HL_DefCtxCmdBufPtrDpcStop;
extern cmd_dproc_vrscl_vid_win_t *HL_DefCtxCmdBufPtrDpcWin;
//extern cmd_dproc_vrscl_send_ext_pic_t *HL_DefCtxCmdBufPtrDpcExt;

extern set_vin_config_t *HL_DefCtxCmdBufPtrVinCfg;
extern set_hdsp_config_t *HL_DefCtxCmdBufPtrHdsp;
extern lossy_compression_t *HL_DefCtxCmdBufPtrCmpr;
extern send_idsp_debug_cmd_t *HL_DefCtxCmdBufPtrDbgIdsp;
extern cmd_postproc_t *HL_DefCtxCmdBufPtrPostP;

#endif //AMBADSP_CONTEXTUTILITY_H
