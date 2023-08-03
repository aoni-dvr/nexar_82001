/**
 *  @file AmbaDSP_StillCapture_Def.h
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
 *  @details Definitions & Constants for Ambarella DSP Driver Still capture Definitions
 *
 */
#ifndef AMBA_DSP_STLCAPTURE_DEF_H
#define AMBA_DSP_STLCAPTURE_DEF_H

#include "AmbaDSP.h"
#include "AmbaDSP_EventInfo.h"

#define DSP_DATACAP_TYPE_RAW            (0U)
#define DSP_DATACAP_TYPE_YUV            (1U)
#define DSP_DATACAP_TYPE_SYNCED_YUV     (2U)  /* Use for VideoThumbnail, synced with First Encode frame, be triggered when encode start */
#define DSP_DATACAP_TYPE_COMP_RAW       (3U)  /* ProcedRaw, After CE/Blend, only support under Liveivew */
#define DSP_DATACAP_TYPE_VIDEO_YUV      (4U)  /* Use for TimeLapse */
#define DSP_DATACAP_TYPE_NUM            (5U)

#define DSP_DATACAP_BUFTYPE_YUV         (0U)
#define DSP_DATACAP_BUFTYPE_AUX         (1U)
#define DSP_DATACAP_BUFTYPE_COMP_RAW    (2U)
#define DSP_DATACAP_BUFTYPE_NUM         (3U)
typedef struct {
    UINT8  CapDataType;             /* Data type to be captured */
    UINT8  AuxDataNeeded;           /* 1: HDS data when TYPE_RAW */
    UINT16 Index;                   /* Indicate VinIdx when TYPE_RAW,
                                     * YuvStrmId when TYPE_YUV
                                     * ViewZoneId when TYPE_COMP_RAW
                                     * EncStrmId when TYPE_SYNCED_YUV/TYPE_VIDEO_YUV */

    UINT16 AllocType;               /* MUST be ALLOC_EXTERNAL_DISTINCT and ALLOC_EXTERNAL_CYCLIC */
    UINT16 BufNum;
    UINT8  OverFlowCtrl;            /* 0 : Wait for Append, 1: Rounding */
    UINT8  CmprRate;                /* compression rate, refer to IK_RAW_COMPRESS_XX.
                                       Only valid when CapDataTyep=TYPE_RAW,
                                       when Liveview is running, CmprRate must be then same as Liveview's setting */
    UINT8  CmptRate;                /* compact rate, refer to IK_RAW_COMPACT_XX - 255U.
                                       Only valid when CapDataTyep=TYPE_RAW,
                                       when Liveview is running, CmprRate must be then same as Liveview's setting */
    UINT8  Rsvd[1U];

    AMBA_DSP_BUF_s DataBuf;         /* BaseAddr valid when ALLOC_EXTERNAL_CYCLIC */
    AMBA_DSP_BUF_s AuxDataBuf;      /* HDS data when RawInput, BaseAddr valid when ALLOC_EXTERNAL_CYCLIC
                                       ME data when TYPE_VIDEO_YUV, BaseAddr valid when ALLOC_EXTERNAL_CYCLIC **/

    ULONG  *pBufTbl;                /* valid when ALLOC_EXTERNAL_DISTINCT */
    ULONG  *pAuxBufTbl;             /* valid when ALLOC_EXTERNAL_DISTINCT */
} AMBA_DSP_DATACAP_CFG_s;

typedef struct {
    UINT16 BufNum;
    UINT16 AllocType;               /* MUST be ALLOC_EXTERNAL_DISTINCT and ALLOC_EXTERNAL_CYCLIC, same as CFG one */
    UINT32 Rsvd;

    ULONG  BufAddr;                 /* BaseAddr valid when ALLOC_EXTERNAL_CYCLIC */
    ULONG  AuxBufAddr;              /* BaseAddr valid when ALLOC_EXTERNAL_CYCLIC */
    ULONG  *pBufTbl;                /* valid when ALLOC_EXTERNAL_DISTINCT */
    ULONG  *pAuxBufTbl;             /* valid when ALLOC_EXTERNAL_DISTINCT */
} AMBA_DSP_DATACAP_BUF_CFG_s;

typedef struct {
    UINT32 CapNum;                  /* 0xFFFFFFFF imply to infinite capture, 0 imply to stop one instance */
    UINT16 Rsvd[2U];
} AMBA_DSP_DATACAP_CTRL_s;

typedef struct {
    ULONG  QTblAddr;                   /* Pointer to Q-table array, size of each Q-table is 128 bytes */
    UINT16 QualityLevel;               /* Initial quality level */
    UINT16 TargetBitRate;              /* Target bit per pixel */
    UINT32 Tolerance;                  /* Bitrate tolerance */
    UINT32 RateCurvPoints;             /* Number of rate curve points */
    ULONG  RateCurvAddr;               /* Pointer to rate curve points buffer */
    UINT8  MaxEncLoop;                 /* Maximum Encode loop */
    UINT8  RotateFlip;                 /* rotate and flip setting, rotate only for 420 */
    UINT8  Rsvd[2U];

    ULONG  BitsBufAddr;                /* pointer to bitstream buffer */

    UINT32 BitsBufSize;                /* bitstream buffer size */

    AMBA_DSP_YUV_IMG_BUF_s YuvBuf;     /* Source YUV buffer*/
    UINT16 EncWidth;                   /* Main JPEG width */
    UINT16 EncHeight;                  /* Main JPEG height */
} AMBA_DSP_STLENC_CTRL_s;

#define STL_R2R_OPT_GEN_AAA  (0U)
#define STL_R2R_OPT_RESCALE  (1U)
#define STL_R2R_OPT_DOLBLEND (2U)
#define STL_R2R_OPT_NUM      (3U)

#endif  /* AMBA_DSP_STLCAPTURE_DEF_H */
