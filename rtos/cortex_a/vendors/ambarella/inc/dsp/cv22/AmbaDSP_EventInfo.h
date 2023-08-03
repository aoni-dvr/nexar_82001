/**
 *  @file AmbaDSP_EventInfo.h
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
 *  @details Definitions & Constants for Ambarella DSP Driver Event info
 *
 */

#ifndef AMBA_DSP_EVENT_INFO_H
#define AMBA_DSP_EVENT_INFO_H

#include "AmbaDSP_Capability.h"

typedef struct {
    UINT8   Interlace;                  /* 1 - Interlace; 0 - Progressive */
    UINT32  TimeScale;                  /* time scale */
    UINT32  NumUnitsInTick;             /* Frames per Second = TimeScale / (NumUnitsInTick * (1 + Interlace)) */
} AMBA_DSP_FRAME_RATE_s;

typedef struct {
    UINT16  OffsetX;                    /* Horizontal offset of the window */
    UINT16  OffsetY;                    /* Vertical offset of the window */
    UINT16  Width;                      /* Number of pixels per line in the window */
    UINT16  Height;                     /* Number of lines in the window */
} AMBA_DSP_WINDOW_s;

typedef struct {
    UINT16  Width;                      /* window width */
    UINT16  Height;                     /* window height */
} AMBA_DSP_WINDOW_DIMENSION_s;

typedef struct {
    ULONG               BaseAddr;
    UINT16              Pitch;
    AMBA_DSP_WINDOW_s   Window;
} AMBA_DSP_BUF_s;

typedef struct {
    UINT8               Compressed;     /* 1 - compressed raw data, 0 - uncompressed raw data */
    ULONG               BaseAddr;       /* raw buffer address */
    UINT16              Pitch;          /* raw buffer pitch */
    AMBA_DSP_WINDOW_s   Window;         /* Window position and size */
} AMBA_DSP_RAW_BUF_s;

typedef struct {
    AMBA_DSP_RAW_BUF_s  RawBuffer;      /* raw buffer information */
    AMBA_DSP_BUF_s      AuxBuffer;      /* Aux raw buffer information */
    UINT16              VinId;          /* Vin index, Bit[15] means VirtVin */
    UINT16              VinVirtChId;    /* Vin virtual channel index, B[7:0]VirtChanIdx, B[15:8]TimeDivisionIdx */
    UINT8               IsVirtChan;     /* Is virtual Channel
                                         * B[0] IsVirtChan
                                         * B[1] IsTimeDivision
                                         * B[2] IsUsingDefaultRaw
                                         * B[3] IsProcessedRaw(After Blend+CE), VinId indicates ViewZoneId
                                         * B[4] IsDroppedRaw, could be a short frame
                                         */
    UINT64              CapPts;         /* capture pts tick */
    UINT64              CapSequence;    /* capture sequence number */
} AMBA_DSP_RAW_DATA_RDY_s;

typedef struct {
    AMBA_DSP_RAW_BUF_s  RawBuffer;      /* raw buffer information */
    AMBA_DSP_BUF_s      AuxBuffer;      /* Aux raw buffer information */
    UINT16              VinId;          /* Vin index, Bit[15] means VirtVin */
    UINT16              VinVirtChId;    /* Vin virtual channel index, B[7:0]VirtChanIdx, B[15:8]TimeDivisionIdx */
    UINT8               IsVirtChan;     /* Is virtual Channel
                                         * B[0] IsVirtChan
                                         * B[1] IsTimeDivision
                                         * B[2] IsUsingDefaultRaw
                                         * B[3] IsProcessedRaw(After Blend+CE), VinId indicates ViewZoneId
                                         * B[4] IsDroppedRaw
                                         */
    UINT64              CapPts;         /* capture pts tick */
    UINT64              CapSequence;    /* capture sequence number */
    AMBA_DSP_BUF_s      EmbedBuffer;    /* Embed data buffer information */
    AMBA_DSP_RAW_BUF_s  Aux2Buffer;     /* Aux2 raw buffer information */
    UINT8               RawBufferNumFree;   /* Latest raw buffer in use */
} AMBA_DSP_RAW_DATA_RDY_EXT3_s;

#define AMBA_DSP_YUV420         (0x0U)
#define AMBA_DSP_YUV422         (0x1U)
#define AMBA_DSP_YUV400         (0x2U) //mono
#define AMBA_DSP_YUV420_10BIT   (0x10U)
#define AMBA_DSP_YUV422_10BIT   (0x11U)
typedef struct {
    UINT8               DataFmt;        /* YUV Data format */
    ULONG               BaseAddrY;      /* Luma (Y) data buffer address */
    ULONG               BaseAddrUV;     /* Chroma (UV) buffer address */

    UINT16              Pitch;          /* YUV data buffer pitch */
    AMBA_DSP_WINDOW_s   Window;         /* Window position and size */
} AMBA_DSP_YUV_IMG_BUF_s;

typedef struct {
    AMBA_DSP_YUV_IMG_BUF_s  Buffer;         /* Yuv buffer information */
    UINT32                  ViewZoneId;     /* view zone index */
    UINT64                  CapPts;         /* capture pts of this yuv */
    UINT64                  YuvPts;         /* yuv generated pts */
    UINT64                  CapSequence;    /* capture sequence number */
} AMBA_DSP_YUV_DATA_RDY_s;

typedef struct {
    AMBA_DSP_YUV_IMG_BUF_s  Buffer;         /* Yuv buffer information */
    UINT32                  ViewZoneId;     /* view zone index */
    UINT64                  CapPts;         /* capture pts of this yuv */
    UINT64                  YuvPts;         /* yuv generated pts */
    UINT64                  CapSequence;    /* capture sequence number */
    UINT64                  YuvSequence;    /* The sequence number of YuvStream or ViewZone's main */
    UINT16                  VinId;          /* Vin index, Bit[15] means VirtVin */
    UINT16                  VinVirtChId;    /* Vin virtual channel index, B[7:0]VirtChanIdx, B[15:8]TimeDivisionIdx */
    UINT8                   IsVirtChan;     /* Is virtual Channel, B[0]IsVirtChan, B[1]IsTimeDivision, B[2]IsUsingDefaultRaw */
    UINT8                   Reserved;
    UINT16                  SourceViewBit;  /* The source ViewZone index of YuvStream */
    UINT32                  Reserved2[4U];
} AMBA_DSP_YUV_DATA_RDY_EXTEND_s;

typedef struct {
    UINT16 HierBit;
    UINT16 IsPolySqrt2;     /* when set, B[15:8] indicate decimation-rate, decimation = 0 means no decimation */
    UINT16 HierPolyWidth;
    UINT16 HierPolyHeight;
    AMBA_DSP_WINDOW_s HierCropWindow[AMBA_DSP_MAX_HIER_NUM];
    UINT32 HierTag;
} AMBA_DSP_LIVEVIEW_PYRAMID_s;

typedef struct {
    UINT16                      ViewZoneId;                     /* view zone index */
    UINT64                      CapPts;                         /* capture pts for this yuv */
    UINT64                      YuvPts;                         /* yuv generated pts */
    UINT64                      CapSequence;                    /* capture sequence number */
    AMBA_DSP_LIVEVIEW_PYRAMID_s Config;                         /* Pyramid Configuration */
    AMBA_DSP_YUV_IMG_BUF_s      YuvBuf[AMBA_DSP_MAX_HIER_NUM];  /* Pyramid Yuv buffer information */
} AMBA_DSP_PYMD_DATA_RDY_s;

#define AMBA_DSP_ENC_END_MARK (0xFFFFFFFFU)  /* PicSize indicates the end of a stream */
#define PIC_FRAME_IDR   (0x1U)
#define PIC_FRAME_I     (0x2U)
#define PIC_FRAME_P     (0x3U)
#define PIC_FRAME_B     (0x4U)
#define PIC_FRAME_JPG   (0x5U)
typedef struct {
    UINT8  StreamId;
    UINT32 FrmNo;
    UINT64 Pts;
    ULONG  StartAddr;
    UINT8  FrameType;
    UINT8  PicStruct;
    UINT32 PicSize;
    UINT8  TileIdx;
    UINT8  NumTile;
    UINT8  SliceIdx;
    UINT8  NumSlice;
    UINT64 CaptureTimeStamp;
    UINT64 EncodeTimeStamp;
    ULONG  InputYAddr;
    ULONG  InputUVAddr;
    UINT16 InputYuvPitch;
    UINT8  PicLevel;
} AMBA_DSP_ENC_PIC_RDY_s;

/* Video path status */
#define VID_PATH_STATUS_NONE        (0x0U)
#define VID_PATH_STATUS_VIN_TIMEOUT (0x1U)

typedef struct {
    UINT32 Status;     /* Video path status */
    UINT32 Data;       /* Index, when Status = 1, Data is VinId */
} AMBA_DSP_VIDEO_PATH_INFO_s;

typedef struct {
    UINT32  StreamId;
} AMBA_DSP_EVENT_VIDEO_ENC_STATUS_s;

/* Decode status */
#define DEC_ERR_NONE        (0x0U)
#define DEC_ERR_WARNING     (0x1U)  /* Decode warning, decoder works fine, no need to do anything*/
                                    /* Case 1: Bits buffer no data, need update new data */
                                    /* Case 2: Frame height limited warning, ucode internal check */
#define DEC_ERR_RECOVERABLE (0x2U)  /* None used, old chip for broadcasting streaming */
#define DEC_ERR_FATAL       (0x3U)  /* Decode stop is necessary, Case1: Bits buffer get error bits */
#define DEC_ERR_NUM         (0x4U)

#define DEC_STATE_INVALID               (0x0U)
#define DEC_STATE_IDLE                  (0x1U)
#define DEC_STATE_RUN                   (0x2U)
#define DEC_STATE_IDLE_WITH_LAST_PIC    (0x3U)
#define DEC_STATE_NUM                   (0x4U)

typedef struct {
    UINT32 DecoderId;
    UINT8  DecState;
    UINT8  ErrState;            /* DEC_ERR_NUM, Decode Error Level */
    UINT32 NumOfDecodedPic;
    ULONG  BitsNextReadAddr;

    UINT8  EndOfStream;
    UINT64 DisplayFramePTS;
    UINT32 NumOfDisplayPic;
    UINT32 DecErrState;         /* Decode Error State, dec_err_code_t check with ucoder */
} AMBA_DSP_EVENT_VIDEO_DEC_STATUS_UPDATE_s;

typedef struct {
    UINT8  VoutIdx;         /* Vout index */
    UINT8  DataFmt;         /* YUV Data format */
    ULONG  YAddr;           /* memory address to Luma (Y) data area */
    ULONG  UVAddr;          /* memory address to Chroma (UV) data area */
    UINT32 Width;           /* YUV width */
    UINT32 Height;          /* YUV height */
    UINT32 Pitch;           /* YUV pitch */
} AMBA_DSP_VIDEO_DEC_YUV_DISP_REPORT_s;

typedef struct {
    UINT32 Num;             /* Decode frame number */
    UINT8  DataFmt;         /* YUV Data format */
    ULONG  YAddr;           /* memory address to Luma (Y) data area */
    ULONG  UVAddr;          /* memory address to Chroma (UV) data area */
    UINT32 Width;           /* YUV width */
    UINT32 Height;          /* YUV height */
    UINT32 Pitch;           /* YUV pitch */
} AMBA_DSP_EVENT_VIDEO_DEC_PIC_INFO_s;

/* Extension version of AMBA_DSP_EVENT_VIDEO_DEC_PIC_INFO_s */
typedef struct {
    UINT32 Num;             /* Decode frame number */
    UINT8  DataFmt;         /* YUV Data format */
    ULONG  YAddr;           /* memory address to Luma (Y) data area */
    ULONG  UVAddr;          /* memory address to Chroma (UV) data area */
    UINT32 Width;           /* YUV width */
    UINT32 Height;          /* YUV height */
    UINT32 Pitch;           /* YUV pitch */
    UINT32 DecoderId;       /* Decoder index */
    UINT64 DecPts;          /* Decode Pts tick */
    UINT64 DecDonePts;      /* Decode Info Pts tick */
                            /* Indicate when decode ready for next stage */
} AMBA_DSP_EVENT_VIDDEC_PIC_EX_s;

/* AMBA_DSP_STLDEC_STATUS_s.Status */
#define AMBA_DSP_STLDEC_DONE                (0U)    /* decode done */
#define AMBA_DSP_STLDEC_PENDING_BITS        (1U)    /* pending on input bits */
#define AMBA_DSP_STLDEC_JPEG_HEADER_ERR     (2U)    /* jpeg header error */
#define AMBA_DSP_STLDEC_JPEG_MCU_ERR        (3U)    /* jpeg mcu error */
#define AMBA_DSP_STLDEC_H264_DEC_ERR        (4U)    /* h264 picture error */
#define AMBA_DSP_STLDEC_STATUS_NUM          (5U)

#define AMBA_DSP_STLDEC_OP_DEC              (0U)    /* decode opertation */
#define AMBA_DSP_STLDEC_OP_Y2Y              (1U)    /* Yuv2Yuv opertation */
#define AMBA_DSP_STLDEC_OP_BLD              (2U)    /* YuvBlend opertation */
#define AMBA_DSP_STLDEC_OP_NUM              (3U)
typedef struct {
    UINT32 Index;           /* Decoder Index */
    UINT32 Status;          /* Decode result status, see AMBA_DSP_STLDEC_STATUS_NUM */
    UINT8  Operation;       /* Decode process type, see AMBA_DSP_STLDEC_OP_NUM */
    UINT8  DataFmt;         /* YUV Data format */
    UINT8  Rsvd[2];
    ULONG  YAddr;           /* memory address to Luma (Y) data area */
    ULONG  UVAddr;          /* memory address to Chroma (UV) data area */
    UINT16 Pitch;           /* YUV pitch */
    UINT16 Width;           /* YUV width */
    UINT16 Height;          /* YUV height */
    UINT16 Rsvd1;
} AMBA_DSP_STLDEC_STATUS_s;

typedef AMBA_DSP_VIDEO_DEC_YUV_DISP_REPORT_s AMBA_DSP_STILL_DEC_YUV_DISP_REPORT_s;

typedef struct {
    UINT16 VinIdx;          /* Vin Index in Bit-wise */
} AMBA_DSP_VIN_POST_CONFIG_STATE_s;

typedef struct {
    UINT8  Status;          /* 0:Job done, 1:Job discard */
    UINT8  Rsvd[3U];
    UINT32 JobId;           /* refer to which JobId */
} AMBA_DSP_LV_SYNC_JOB_INFO_s;

typedef struct {
    UINT8  VoutIdx;          /* Vout index */
    UINT8  Status;           /* B[0] : DVE polarity status */
    UINT8  Rsvd[2U];

    AMBA_DSP_YUV_IMG_BUF_s YuvBuf;
    UINT64 DispStartTime;   /* vout device display yuv buffer start time */
    UINT64 DispDoneTime;    /* vout device display yuv buffer finish time */
} AMBA_DSP_VOUT_DATA_INFO_s;

typedef struct {
    UINT16 CapInstance;     /* Capture instance */
    UINT16 Rsvd;

    UINT8  BufFormat;       /* RAW_FORMAT_MEM_NUM */
    UINT8  AuxBufFormat;    /* RAW_FORMAT_MEM_NUM */
    UINT16 Rsvd1;
    AMBA_DSP_BUF_s Buf;     /* Luma (Y) buf */
    AMBA_DSP_BUF_s AuxBuf;  /* Chroma (UV) buf  */
    UINT64 CapPts;          /* capture pts tick */
    UINT64 CapSequence;     /* capture sequence number */

    AMBA_DSP_BUF_s Aux2Buf; /* AuxBuf data for DSP_DATACAP_TYPE_VIDEO_YUV */
} AMBA_DSP_STL_RAW_DATA_INFO_s;

typedef struct {
    UINT16 StreamId;
    UINT8  MvBufType;       /* 0:Ctb 1:Coloc 2:AVC */
    UINT8  Rsvd;

    UINT64 CapPts;          /* capture pts tick */
    UINT64 CapSequence;     /* capture sequence number */

    ULONG  MvBufAddr;

    UINT16 MvBufPitch;
    UINT16 MvBufWidth;

    UINT16 MvBufHeight;
    UINT16 Rsvd1;
} AMBA_DSP_ENC_MV_RDY_s;

/* Internal used structure */
typedef struct {
    UINT32 ViewZoneId;
    ULONG  PrivateInfoAddr;   /* Points to a user defined structure */
} AMBA_DSP_LIVEVIEW_SIDEBAND_s;

/* Internal used structure */
typedef struct {
    UINT32                       ViewZoneID;
    UINT32                       CapSeqNo;                          /* capture sequence number */
    AMBA_DSP_LIVEVIEW_SIDEBAND_s Info[AMBA_DSP_MAX_TOKEN_ARRAY];    /* SideBandInfo */
} AMBA_DSP_SIDEBAND_DATA_RDY_s;

typedef struct {
    UINT16                  ViewZoneId;     /* view zone index */
    UINT16                  HierSource;     /* lane detect source from which Pyramid output */
    UINT64                  CapPts;         /* capture pts for this yuv */
    UINT64                  YuvPts;         /* yuv generated pts */
    UINT64                  CapSequence;    /* capture sequence number */
    AMBA_DSP_WINDOW_s       CropWindow;
    AMBA_DSP_YUV_IMG_BUF_s  YuvBuf;         /* Yuv buffer information */
} AMBA_DSP_LNDT_DATA_RDY_s;

/* AMBA_DSP_EVENT_ERROR_INFO_s.Type */
#define AMBA_DSP_ERROR_BITSINFO          (0U)  /* Encoder bitstream information */
#define AMBA_DSP_ERROR_ASSERT            (1U)  /* DSP assertion */
#define AMBA_DSP_ERROR_VIN_CMDREQ        (2U)  /* Vin commend request timeout */
#define AMBA_DSP_ERROR_VOUT_CMDREQ       (3U)  /* Vout commend request timeout */
#define AMBA_DSP_ERROR_NUM               (4U)

#define AMBA_DSP_ERROR_CAT_VOUT     (0U)
#define AMBA_DSP_ERROR_CAT_VIN      (1U)
#define AMBA_DSP_ERROR_CAT_IDSP     (2U)
#define AMBA_DSP_ERROR_CAT_IDSP1    (3U)
#define AMBA_DSP_ERROR_CAT_IDSP2    (4U)
#define AMBA_DSP_ERROR_CAT_CODEC    (5U)
#define AMBA_DSP_ERROR_CAT_CODEC1   (6U)
#define AMBA_DSP_ERROR_CAT_CODEC2   (7U)
#define AMBA_DSP_ERROR_CAT_NUM      (8U)
typedef struct {
    UINT32                              Type;  /* Error type, see AMBA_DSP_ERROR_NUM */
    UINT32                              Info;  /* Error information: */
                                               /*   AMBA_DSP_ERROR_BITSINFO: 0U */
                                               /*   AMBA_DSP_ERROR_ASSERT:  Ucode error id */
    UINT32                              InfoAux;    /* Error Category: */
                                                    /*   AMBA_DSP_ERROR_ASSERT: Error thread, see AMBA_DSP_ERROR_CAT_NUM */
} AMBA_DSP_EVENT_ERROR_INFO_s;

#define AMBA_DSP_CLOCK_ID_CORE          (0U)        /**< core clock */
#define AMBA_DSP_CLOCK_ID_NUM           (1U)        /**< Total clock id number */

#define AMBA_DSP_CLOCK_STAGE_PREPROC    (0U)        /**< clock change stage preproc */
#define AMBA_DSP_CLOCK_STAGE_POSTPROC   (1U)        /**< clock change stage postproc */
#define AMBA_DSP_CLOCK_STAGE_NUM        (2U)        /**< Total clock stage number */

/**
 * @brief AMBA_DSP_EVENT_CLK_UPDATE_STATUS structure.
 */
typedef struct {
    UINT32                              ClockId;  /*!< clock id:\n
                                                     @ref AMBA_DSP_CLOCK_ID_CORE\n
                                                     @ref AMBA_DSP_CLOCK_ID_NUM */
    UINT32                              Stage;    /*!< clock update stage:\n
                                                     @ref AMBA_DSP_CLOCK_STAGE_PREPROC\n
                                                     @ref AMBA_DSP_CLOCK_STAGE_POSTPROC */
} AMBA_DSP_EVENT_CLK_UPDATE_STATUS_INFO_s;

#define AMBA_DSP_VDO_BLD_OK         (0U)
#define AMBA_DSP_VDO_BLD_SYNC_ERROR (1U)    /* UserData is not synced in osd-buffer first word of extra line */
typedef struct {
    UINT16 Id;
    UINT16 Rsvd;

    UINT32 UserData;
    UINT32 Status;
} AMBA_DSP_VIDEO_BLEND_INFO_s;

#endif  /* AMBA_DSP_EVENT_INFO_H */
