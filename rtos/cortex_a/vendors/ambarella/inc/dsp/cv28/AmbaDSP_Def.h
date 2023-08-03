/**
 *  @file AmbaDSP_Def.h
 *
 *  @copyright Copyright (c) 2019 Ambarella, Inc.
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
 *  @details Definitions & Constants for Ambarella DSP Driver Definition
 *
 */
#ifndef AMBA_DSP_DEF_H
#define AMBA_DSP_DEF_H

#include "AmbaTypes.h"
#if defined(CONFIG_THREADX)
#include "AmbaKAL.h"
#include "AmbaPrint.h"//AmbaPrint
#include "AmbaErrorCode.h"
#include "AmbaWrap.h"
#include "AmbaMisraFix.h"
#endif
#include "AmbaDSP_EventInfo.h"

/* Error definition */
#define DSP_ERR_NONE                        (0U)
#define DSP_ERR_0000                        (SSP_ERR_BASE       )   // Invalid argument (null pointer)
#define DSP_ERR_0001                        (SSP_ERR_BASE + 0x1U)   // Input argument out of range
#define DSP_ERR_0002                        (SSP_ERR_BASE + 0x2U)   // Buffer alignment error
#define DSP_ERR_0003                        (SSP_ERR_BASE + 0x3U)   // OS resource error
#define DSP_ERR_0004                        (SSP_ERR_BASE + 0x4U)   // Invalid timing
#define DSP_ERR_0005                        (SSP_ERR_BASE + 0x5U)   // Queue full
#define DSP_ERR_0006                        (SSP_ERR_BASE + 0x6U)   // Liveview routing fail
#define DSP_ERR_0007                        (SSP_ERR_BASE + 0x7U)   // Internal write command error
#define DSP_ERR_0008                        (SSP_ERR_BASE + 0x8U)   // not support or not implement
#define DSP_ERR_TIMEOUT                     (SSP_ERR_BASE + 0x9U)   // Timeout
#define DSP_ERR_0200                        (SSP_ERR_BASE + 0x200U) // DSP uCode fatal error

/* DSP System State */
#define AMBA_DSP_SYS_STATE_LIVEVIEW         (0U)
#define AMBA_DSP_SYS_STATE_PLAYBACK         (1U)
#define AMBA_DSP_SYS_STATE_XCODE            (2U)
#define AMBA_DSP_SYS_STATE_DUPLEX           (3U)
#define AMBA_DSP_SYS_STATE_SENSORLESS       (4U)
#define AMBA_DSP_SYS_STATE_UNKNOWN          (5U)
#define AMBA_DSP_SYS_STATE_NUM              (6U)

/* Rotate Flip */
#define AMBA_DSP_ROTATE_0                   (0U)
#define AMBA_DSP_ROTATE_0_HORZ_FLIP         (1U)
#define AMBA_DSP_ROTATE_90                  (2U)
#define AMBA_DSP_ROTATE_90_VERT_FLIP        (3U)
#define AMBA_DSP_ROTATE_180                 (4U)
#define AMBA_DSP_ROTATE_180_HORZ_FLIP       (5U)
#define AMBA_DSP_ROTATE_270                 (6U)
#define AMBA_DSP_ROTATE_270_VERT_FLIP       (7U)
#define AMBA_DSP_ROTATE_NUM                 (8U)

/* Vin wait Flag */
#define AMBA_DSP_VIN0_SOF                   ((UINT32)0x1U)
#define AMBA_DSP_VIN0_EOF                   ((UINT32)0x2U)
#define AMBA_DSP_VIN1_SOF                   ((UINT32)0x4U)
#define AMBA_DSP_VIN1_EOF                   ((UINT32)0x8U)
#define AMBA_DSP_VIN2_SOF                   ((UINT32)0x10U)
#define AMBA_DSP_VIN2_EOF                   ((UINT32)0x20U)

/* Vout wait Flag */
#define AMBA_DSP_VOUT0_INT                  ((UINT32)0x1U)

typedef struct {
    UINT32 MsgBufAddr;        /* base address of Message */
    UINT32 MsgNum;            /* maximum number of messages */
    UINT32 RP;                /* read pointer */
    UINT32 WP;                /* write pointer */
    UINT32 reserved[12];
} AMBA_DSP_MSG_BUFF_CTRL_s;

/* DSP System cofiguration */
typedef struct {
    UINT8   SysState;                       /* DSP System state */
    ULONG   DebugLogDataAreaAddr;           /* DSP Debug Log buffer address */
    UINT32  DebugLogDataAreaSize;           /* DSP Debug Log Data area in Bytes */
    ULONG   WorkAreaAddr;                   /* DSP Work buffer address */
    UINT32  WorkAreaSize;                   /* DSP Work Data area in Bytes */
    ULONG   VpMsgCtrlBufAddr;               /* address of Vp message buffer, refer to AMBA_DSP_MSG_BUFF_CTRL_s */
} AMBA_DSP_SYS_CONFIG_s;

/* DSP version information */
typedef struct {
    UINT8  Month;
    UINT8  Day;
    UINT16 Year;
    UINT32 UCodeVer;
    UINT32 ApiVer;
    UINT32 SiliconVer;
    UINT32 LinkBase;
    UINT32 SSPVer;
    UINT32 ChipId;
} AMBA_DSP_VERSION_INFO_s;

typedef struct {
    ULONG CoreStartAddr;
    ULONG MeStartAddr;
    ULONG MdxfStartAddr;
    ULONG DefaultDataStartAddr;
    ULONG CoreEndAddr;
    ULONG MeEndAddr;
    ULONG MdxfEndAddr;
    ULONG DefaultDataEndAddr;
} AMBA_DSP_BIN_ADDR_s;

#define AMBA_DSP_PROTECT_DATA_LEN (64U)
typedef struct {
    UINT8 Data[AMBA_DSP_PROTECT_DATA_LEN];
} AMBA_DSP_PROTECT_s;

/* DSP Flag in AmbaDSP_MainWaitFlag */
#define AMBA_DSP_MESSAGE_ARRIVED           ((UINT32)0x1U)

#define ALLOC_INTERNAL          (0U)
#define ALLOC_EXTERNAL_DISTINCT (1U)
#define ALLOC_EXTERNAL_CYCLIC   (2U)
#define RAW_FORMAT_MEM_RAW      (0U)
#define RAW_FORMAT_MEM_YUV422   (1U)
#define RAW_FORMAT_MEM_YUV420   (2U)
#define RAW_FORMAT_MEM_NUM      (3U)
#define MAX_EXT_DISTINCT_DEPTH  (64U)
/* DSP resoure relative setting */
typedef struct {
    UINT16 AllocType;
    UINT16 BufNum;
    AMBA_DSP_RAW_BUF_s Buf;
    ULONG  *pRawBufTbl;
    AMBA_DSP_RAW_BUF_s AuxBuf;
    ULONG  *pAuxBufTbl;
    UINT16 RawFormat;
} AMBA_DSP_LIVEVIEW_RAW_BUF_s;

typedef struct {
    AMBA_DSP_YUV_IMG_BUF_s ExtYuvBuf;
    AMBA_DSP_BUF_s *pExtME1Buf;
    AMBA_DSP_BUF_s *pExtME0Buf;
    UINT8  IsLast;
} AMBA_DSP_EXT_YUV_BUF_s;

typedef struct {
    UINT32 CtxIndex;
    UINT32 CfgIndex;
    ULONG  CfgAddress;
    UINT32 HdrCfgIndex;
} AMBA_DSP_ISOCFG_CTRL_s;
typedef AMBA_DSP_ISOCFG_CTRL_s AMBA_DSP_DEFAULT_RAW_ISOCFG_CTRL_s;

typedef struct {
    UINT32 VideoPipe;
    UINT16 LowDelayMode;
    UINT16 MaxVinBit;
    UINT16 MaxViewZoneNum;
    UINT16 MaxVinVirtChanBit[AMBA_DSP_MAX_VIN_NUM];
    UINT16 MaxVinVirtChanOutputNum[AMBA_DSP_MAX_VIN_NUM][AMBA_DSP_MAX_VIRT_CHAN_NUM];
    UINT16 MaxVinVirtChanWidth[AMBA_DSP_MAX_VIN_NUM][AMBA_DSP_MAX_VIRT_CHAN_NUM];
    UINT16 MaxVinVirtChanHeight[AMBA_DSP_MAX_VIN_NUM][AMBA_DSP_MAX_VIRT_CHAN_NUM];
    AMBA_DSP_LIVEVIEW_RAW_BUF_s RawBuf[AMBA_DSP_MAX_VIN_NUM][AMBA_DSP_MAX_VIRT_CHAN_NUM];
    UINT32 MaxVinBootTimeout[AMBA_DSP_MAX_VIN_NUM];
    UINT32 MaxVinTimeout[AMBA_DSP_MAX_VIN_NUM];
    UINT16 MaxRaw2YuvDelay[AMBA_DSP_MAX_VIN_NUM];
    UINT16 MaxWarpDma[AMBA_DSP_MAX_VIEWZONE_NUM];
    UINT16 MaxWarpWaitLineLuma[AMBA_DSP_MAX_VIEWZONE_NUM];
    UINT16 MaxWarpWaitLineChroma[AMBA_DSP_MAX_VIEWZONE_NUM];
    UINT16 MaxHierWidth[AMBA_DSP_MAX_VIEWZONE_NUM];
    UINT16 MaxHierHeight[AMBA_DSP_MAX_VIEWZONE_NUM];
    UINT16 MaxLndtWidth[AMBA_DSP_MAX_VIEWZONE_NUM];
    UINT16 MaxLndtHeight[AMBA_DSP_MAX_VIEWZONE_NUM];
    UINT16 MaxVirtVinBit;
    UINT16 MaxVirtVinOutputNum[AMBA_DSP_MAX_VIRT_VIN_NUM];
    UINT16 MaxVirtVinWidth[AMBA_DSP_MAX_VIRT_VIN_NUM];
    UINT16 MaxVirtVinHeight[AMBA_DSP_MAX_VIRT_VIN_NUM];
    AMBA_DSP_DEFAULT_RAW_ISOCFG_CTRL_s DefaultRawIsoConfigAddr[AMBA_DSP_MAX_VIEWZONE_NUM];
    AMBA_DSP_RAW_BUF_s DefaultRawBuf[AMBA_DSP_MAX_VIN_NUM][AMBA_DSP_MAX_VIRT_CHAN_NUM];
    AMBA_DSP_RAW_BUF_s DefaultAuxRawBuf[AMBA_DSP_MAX_VIN_NUM][AMBA_DSP_MAX_VIRT_CHAN_NUM];
    UINT16 DefaultRawFormat[AMBA_DSP_MAX_VIN_NUM];
} AMBA_DSP_LIVEVIEW_RESOURCE_s;

typedef struct {
    UINT16 MaxEncodeStream;
    UINT8  MaxGopM[AMBA_DSP_MAX_STREAM_NUM];
    UINT8  MaxExtraReconNum[AMBA_DSP_MAX_STREAM_NUM];
    UINT8  MaxSearchRange[AMBA_DSP_MAX_STREAM_NUM];
    UINT8  MaxStrmFmt[AMBA_DSP_MAX_STREAM_NUM]; //B[0]:H264, B[1]:H265, B[2]:JPG, B[6]:HierP, B[7]:TimeLapse, B[5]:Rotate, B[4]:OSD sync
    UINT16 MaxOsdSyncDly[AMBA_DSP_MAX_STREAM_NUM]; // in msec unit, OSD is executed before delay timeout after yuv generated
} AMBA_DSP_ENCODE_RESOURCE_s;

typedef struct {
    UINT8  MaxProcessFormat;    //B[0]LISO, B[1]HISO, B[2]CE, B[3]HDR
    UINT16 MaxVinWidth[AMBA_DSP_MAX_VIN_NUM];   // 0 means not used
    UINT16 MaxVinHeight[AMBA_DSP_MAX_VIN_NUM];  // 0 means not used
    UINT16 MaxMainWidth;        // 0 means not used
    UINT16 MaxMainHeight;       // 0 means not used
    UINT16 MaxRawInputWidth;    // 0 means not used
    UINT16 MaxRawInputHeight;   // 0 means not used
    UINT16 MaxYuvInputWidth;    // 0 means not used
    UINT16 MaxYuvInputHeight;   // 0 means not used
    UINT16 MaxYuvEncWidth;      // 0 means not used
    UINT16 MaxYuvEncHeight;     // 0 means not used
} AMBA_DSP_STILL_RESOURCE_s;

#define AMBA_DSP_DEC_BITS_FORMAT_JPEG   (0U)
#define AMBA_DSP_DEC_BITS_FORMAT_H264   (1U)
#define AMBA_DSP_DEC_BITS_FORMAT_H265   (2U)
#define AMBA_DSP_DEC_BITS_FORMAT_NUM    (3U)
typedef struct {
    UINT16 MaxDecodeStreamNum;
    UINT16 MaxStrmFmt[AMBA_DSP_MAX_DEC_STREAM_NUM];
    UINT16 MaxFrameWidth;
    UINT16 MaxFrameHeight;
    UINT32 MaxBitRate;
    UINT16 MaxVideoPlaneWidth;
    UINT16 MaxVideoPlaneHeight;
    UINT16 MaxRatioOfGopNM;
    UINT16 BackwardTrickPlayMode;
    UINT16 MaxPicWidth;
    UINT16 MaxPicHeight;
} AMBA_DSP_DECODE_RESOURCE_s;

typedef struct {
    UINT16 MaxVoutWidth[AMBA_DSP_MAX_VOUT_NUM];
    UINT16 MaxOsdBufWidth[AMBA_DSP_MAX_VOUT_NUM];
} AMBA_DSP_DISPLAY_RESOURCE_s;

typedef struct {
    UINT16 MaxEncWidth[AMBA_DSP_MAX_STREAM_NUM];
    UINT16 MaxEncHeight[AMBA_DSP_MAX_STREAM_NUM];
} AMBA_DSP_EXTMEM_ENC_RESOURCE_s;

typedef struct {
    UINT16 MaxHorWarpCompensation[AMBA_DSP_MAX_VIEWZONE_NUM];   // pixel unit
    UINT32 MaxVinCapability[AMBA_DSP_MAX_VIN_NUM];  // B[0] 0 means HDS follow Liveview. 1 means HDS force on
    UINT32 Rsvd[4];
    UINT16 MaxVinShortFrameThreshold[AMBA_DSP_MAX_VIN_NUM]; // in msec unit, once capture time difference smaller than threshold, raw frame will be dropped
} AMBA_DSP_LIVEVIEW_RESOURCE_EX_s;

typedef struct {
    AMBA_DSP_LIVEVIEW_RESOURCE_s    LiveviewResource;
    AMBA_DSP_ENCODE_RESOURCE_s      EncodeResource;
    AMBA_DSP_STILL_RESOURCE_s       StillResource;
    AMBA_DSP_DECODE_RESOURCE_s      DecodeResource;
    AMBA_DSP_DISPLAY_RESOURCE_s     DisplayResource;
    AMBA_DSP_EXTMEM_ENC_RESOURCE_s  ExtMemEncResource;
    AMBA_DSP_LIVEVIEW_RESOURCE_EX_s LiveviewResourceEx;
} AMBA_DSP_RESOURCE_s;

/* Data in AmbaDSP_ParLoadConfig() */
#define DSP_REGION_VOUT_IDX     (0U) //Support VOUT feature
#define DSP_REGION_LV_IDX       (1U) //Support Liveview feature
#define DSP_REGION_ENC_IDX      (2U) //Support Encode feature
#define DSP_REGION_DEC_IDX      (3U) //Support Decode feature
#define DSP_REGION_NUM          (4U)

typedef struct {
    ULONG    BaseAddr;
    ULONG    BasePhysAddr;
    UINT32   Size;
    UINT32   IsCached;
} AMBA_DSP_BUF_INFO_s;

#endif  /* AMBA_DSP_DEF_H */
