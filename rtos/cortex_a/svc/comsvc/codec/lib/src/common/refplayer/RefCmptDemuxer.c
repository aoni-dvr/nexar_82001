 /**
 *  @file RefCmptDemuxer.c
 *
 * Copyright (c) [2020] Ambarella International LP
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
 * This file includes sample code and is only for internal testing and evaluation.  If you 
 * distribute this sample code (whether in source, object, or binary code form), it will be 
 * without any warranty or indemnity protection from Ambarella International LP or its affiliates.
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
 *  @details Reference compatible demuxer
 *
 */

#include "AmbaTypes.h"
#include "AmbaKAL.h"
#include "AmbaDSP_VideoDec.h"
#include "SvcFile.h"

#include "AmbaSvcWrap.h"
#include "RefCmptDemuxerImpl.h"
#include "RefCmptPlayerImpl.h"
#include "AmbaCodecCom.h"

#define REF_CMPT_FRAME_COUNT_EOS    (0xFFFFFFFFUL)       ///< VideoDecFrmCnt of 0xFFFFFFFF indicates EOS
#define REF_CMPT_DEMUX_STACK_SIZE (4096U)                 ///< Size of the stack for demux task
#define REF_CMPT_DEMUX_MAX_NUM (1U)                         ///< Maximum number of demux handler

#define AMBA_ISO_BOX_SIZE_TAG_SIZE  (8U)

// internal error code
#define REF_CMPT_DEMUX_ERR_NOT_ENOUGH_SPACE  2U
#define REF_CMPT_DEMUX_END                   0xFFFFFFFEU

static AMBA_KAL_TASK_t RefCmptDemuxTask;       // Demux task

static REF_CMPT_DEMUX_HDLR_s RefCmptDemuxHdlr[REF_CMPT_DEMUX_MAX_NUM]  __attribute__((section (".bss.noinit")));  // Demux handler (for each codec)

#define REF_CMPT_PRINT_DBG      (0U)
#define MAX_FRAME_COUNT         (60U * 60U * 1U)
#define MAX_CHUNK_TABLE_COUNT   (32U)
#define MAX_CHUNK_COUNT         MAX_FRAME_COUNT
#define ISO_STSD_MAX_ENC_NAME_LEN   (32U)   /**< Max stsd encode name length */
#define AMBA_H264_STRAT_CODE_SIZE   (4U)
#define AMBA_H264_GOP_NALU_SIZE     (22U)
#define AMBA_H265_STRAT_CODE_SIZE   (4U)
#define AMBA_H265_GOP_NALU_SIZE     (22U)
#define AVC1 0x61766331U     /**< 'avc1' */
//#define HVC1 0x68766331U
#define AVCC 0x61766343U     /**< 'avcc' */
#define HVCC 0x68766343U     /**< 'hvcc' */
#define MINF 0x6D696E66U     /**< 'minf' */
#define MDIA 0x6D646961U     /**< 'mdia' */
#define MOOV 0x6D6F6F76U     /**< 'moov' */
#define STBL 0x7374626CU     /**< 'stbl' */
#define STSD 0x73747364U     /**< 'stsd' */
#define STSC 0x73747363U     /**< 'stsc' */
#define STSZ 0x7374737AU     /**< 'stsz' */
#define STCO 0x7374636FU     /**< 'stco' */
#define TRAK 0x7472616bU     /**< 'trak' */

typedef struct {
    UINT8 MediaId;          /**< The media type of the track (The ID is a media ID. See SVC_FORMAT_MID_e.) */
    UINT16 VideoWidth;
    UINT16 VideoHeight;
    UINT16 VPSLen;          /**< The SPS size of H265 */
    UINT16 SPSLen;          /**< The SPS size of H264/H265 */
    UINT16 PPSLen;          /**< The PPS size of H264/H265 */
    UINT8 VPS[SVC_FORMAT_MAX_VPS_LENGTH];   /**< The VPS of H265 */
    UINT8 SPS[SVC_FORMAT_MAX_SPS_LENGTH];   /**< The SPS of H264/H265 */
    UINT8 PPS[SVC_FORMAT_MAX_PPS_LENGTH];   /**< The PPS of H264/H265 */
    UINT32 ChunkTableCount;
    UINT32 FirstChunk[MAX_CHUNK_TABLE_COUNT];
    UINT32 FramePerChunk[MAX_CHUNK_TABLE_COUNT];
    UINT32 ChunkCount;
    UINT32 ChunkPos[MAX_CHUNK_COUNT];
    UINT32 FrameCount;
    UINT32 FramePos[MAX_FRAME_COUNT];
    UINT32 FrameSize[MAX_FRAME_COUNT];
    // user must assign the below variables
    UINT32 GOPSize;
    UINT8 *BufferBase;
    UINT8 *BufferLimit; // Limit = Base + Size
} VIDEO_TRACK_INFO_s;

static VIDEO_TRACK_INFO_s VideoTrackInfo = {0};

static UINT32 RefCmptDemuxer_Le2Be32(UINT32 Code)
{
    UINT32 Rval;
    Rval  = (Code & 0xFFU) << 24U;
    Rval += (Code & 0xFF00U) << 8U;
    Rval += (Code & 0xFF0000U) >> 8U;
    Rval += (Code & 0xFF000000U) >> 24U;
    return Rval;
}

static inline UINT32 ST2A(UINT32 Ret)
{
    UINT32 Rval = REFCODE_OK;

    if (Ret != OK) {
        if (Ret == STREAM_ERR_0002) {
            Rval = REFCODE_IO_ERR;
        } else {
            Rval = REFCODE_GENERAL_ERR;
        }
    }

    return Rval;
}

static inline UINT32 K2A(UINT32 Ret)
{
    UINT32 Rval = REFCODE_OK;
    if (Ret != KAL_ERR_NONE) {
        Rval = REFCODE_GENERAL_ERR;
    }
    return Rval;
}

static inline UINT32 DSP2A(UINT32 Ret)
{
    UINT32 Rval = REFCODE_OK;
    if (Ret != OK) {
        Rval = REFCODE_GENERAL_ERR;
    }
    return Rval;
}

static UINT32 RefCmptDemuxer_GetByte(SVC_STREAM_HDLR_s *File, UINT8 *Buffer)
{
    UINT32 Rval;
    UINT32 Count;
    Rval = ST2A(File->Func->Read(File, 1U, Buffer, &Count));
    if (Rval == REFCODE_OK) {
        if (Count != 1U) {
            AmbaPrint_PrintStr5("%s: [ERROR] Read data from Stream fail", __func__, NULL, NULL, NULL, NULL);
            Rval = REFCODE_GENERAL_ERR;
        }
    } else {
        AmbaPrint_PrintStr5("%s: [ERROR] Fail to read file", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

static UINT32 RefCmptDemuxer_GetBe16(SVC_STREAM_HDLR_s *File, UINT16 *Buffer)
{
    UINT32 Rval;
    UINT8 Rtn[2];
    UINT32 Count;
    Rval = ST2A(File->Func->Read(File, 2U, Rtn, &Count));
    if (Rval == REFCODE_OK) {
        if (Count == 2U) {
            *Buffer = (((UINT16)Rtn[0]<<8) | (UINT16)Rtn[1]);
        } else {
            AmbaPrint_PrintStr5("%s: [ERROR] Read data from Stream fail", __func__, NULL, NULL, NULL, NULL);
            Rval = REFCODE_GENERAL_ERR;
        }
    } else {
        AmbaPrint_PrintStr5("%s: [ERROR] Fail to read file", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

static UINT32 RefCmptDemuxer_GetBe32(SVC_STREAM_HDLR_s *File, UINT32 *Buffer)
{
    UINT32 Rval;
    UINT8 Rtn[4];
    UINT32 Count;
    Rval = ST2A(File->Func->Read(File, 4U, Rtn, &Count));
    if (Rval == REFCODE_OK) {
        if (Count == 4U) {
            *Buffer = (((UINT32)Rtn[0]<<24) | ((UINT32)Rtn[1]<<16) | ((UINT32)Rtn[2]<<8) | (UINT32)Rtn[3]);
        } else {
            AmbaPrint_PrintStr5("%s: [ERROR] Read data from Stream fail", __func__, NULL, NULL, NULL, NULL);
            Rval = REFCODE_GENERAL_ERR;
        }
    } else {
        AmbaPrint_PrintStr5("%s: [ERROR] Fail to read file", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

static UINT32 RefCmptDemuxer_GetBoxHeader(SVC_STREAM_HDLR_s *File, UINT32 *BoxSize, UINT32 *BoxTag)
{
    UINT32 Rval = RefCmptDemuxer_GetBe32(File, BoxSize);
    if (Rval == REFCODE_OK) {
        Rval = RefCmptDemuxer_GetBe32(File, BoxTag);
        if (Rval == REFCODE_OK) {
            if ((*BoxSize == 0U) || (*BoxTag == 0xffffffffU)) {
                AmbaPrint_PrintUInt5("RefCmptDemuxer_GetBoxHeader: [ERROR] box size (%u) or box tag (%u) error", *BoxSize, *BoxTag, 0U, 0U, 0U);
                Rval = REFCODE_GENERAL_ERR;
            }
        } else {
            AmbaPrint_PrintStr5("%s: [ERROR] RefCmptDemuxer_GetBe32 BoxTag fail", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_PrintStr5("%s: [ERROR] RefCmptDemuxer_GetBe32 BoxSize fail", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

static UINT32 RefCmptDemuxer_ParseVideoAvcc(SVC_STREAM_HDLR_s *File, VIDEO_TRACK_INFO_s *Track)
{
    UINT32 Rval;
    UINT8 Tmp8;
    UINT32 Tmp32;
    Rval = RefCmptDemuxer_GetByte(File, &Tmp8);
    if (Rval == REFCODE_OK) {
        Rval = RefCmptDemuxer_GetByte(File, &Tmp8);
    }
    if (Rval == REFCODE_OK) {
        Rval = RefCmptDemuxer_GetByte(File, &Tmp8);
    }
    if (Rval == REFCODE_OK) {
        Rval = RefCmptDemuxer_GetByte(File, &Tmp8);
    }
    if (Rval == REFCODE_OK) {
        Rval = RefCmptDemuxer_GetByte(File, &Tmp8);
    }
    if (Rval == REFCODE_OK) {
        // SPS num
        Rval = RefCmptDemuxer_GetByte(File, &Tmp8);
    }
    if (Rval == REFCODE_OK) {
        Rval = RefCmptDemuxer_GetBe16(File, &Track->SPSLen);
    }
    if (Rval == REFCODE_OK) {
        Rval = ST2A(File->Func->Read(File, Track->SPSLen, Track->SPS, &Tmp32));
        if (Rval == REFCODE_OK) {
            if (Tmp32 != Track->SPSLen) {
                AmbaPrint_PrintStr5("%s: [ERROR] SPS not enough!", __func__, NULL, NULL, NULL, NULL);
                Rval = REFCODE_GENERAL_ERR;
            }
        } else {
            AmbaPrint_PrintStr5("%s: [ERROR] Read SPS fail!", __func__, NULL, NULL, NULL, NULL);
        }
    }
    if (Rval == REFCODE_OK) {
        Rval = RefCmptDemuxer_GetByte(File, &Tmp8);
    }
    if (Rval == REFCODE_OK) {
        Rval = RefCmptDemuxer_GetBe16(File, &Track->PPSLen);
    }
    if (Rval == REFCODE_OK) {
        Rval = ST2A(File->Func->Read(File, Track->PPSLen, Track->PPS, &Tmp32));
        if (Rval == REFCODE_OK) {
            if (Tmp32 != Track->PPSLen) {
                AmbaPrint_PrintStr5("%s: [ERROR] PPS not enough!", __func__, NULL, NULL, NULL, NULL);
                Rval = REFCODE_GENERAL_ERR;
            }
        } else {
            AmbaPrint_PrintStr5("%s: [ERROR] Read PPS fail!", __func__, NULL, NULL, NULL, NULL);
        }
    }
    return Rval;
}

static UINT32 RefCmptDemuxer_ParseVideoHvcc(SVC_STREAM_HDLR_s *File, VIDEO_TRACK_INFO_s *Track)
{
    UINT32 Rval;
    UINT32 Tmp32;
    UINT16 Tmp16;
    UINT8 Tmp8;
    UINT8 I;
    Rval = RefCmptDemuxer_GetByte(File, &Tmp8);
    if (Rval == REFCODE_OK) {
        for (I = 0U; I < SVC_FORMAT_MAX_VPS_GEN_CFG_LENGTH; I++) {
            Rval = RefCmptDemuxer_GetByte(File, &Tmp8);
            if (Rval != REFCODE_OK) {
                break;
            }
        }
    }
    if (Rval == REFCODE_OK) {
        Rval = RefCmptDemuxer_GetBe16(File, &Tmp16);
    }
    if (Rval == REFCODE_OK) {
        Rval = RefCmptDemuxer_GetByte(File, &Tmp8);
    }
    if (Rval == REFCODE_OK) {
        Rval = RefCmptDemuxer_GetByte(File, &Tmp8);
    }
    if (Rval == REFCODE_OK) {
        Rval = RefCmptDemuxer_GetByte(File, &Tmp8);
    }
    if (Rval == REFCODE_OK) {
        Rval = RefCmptDemuxer_GetByte(File, &Tmp8);
    }
    if (Rval == REFCODE_OK) {
        Rval = RefCmptDemuxer_GetBe16(File, &Tmp16);
    }
    if (Rval == REFCODE_OK) {
        Rval = RefCmptDemuxer_GetByte(File, &Tmp8);
    }
    if (Rval == REFCODE_OK) {
        Rval = RefCmptDemuxer_GetByte(File, &Tmp8);
    }
    if (Rval == REFCODE_OK) {
        Rval = RefCmptDemuxer_GetByte(File, &Tmp8);
    }
    /*vps*/
    if (Rval == REFCODE_OK) {
        Rval = RefCmptDemuxer_GetBe16(File, &Tmp16);
    }
    if (Rval == REFCODE_OK) {
        Rval = RefCmptDemuxer_GetBe16(File, &Track->VPSLen);
    }
    if (Rval == REFCODE_OK) {
        Rval = ST2A(File->Func->Read(File, Track->VPSLen, Track->VPS, &Tmp32));
        if (Rval == REFCODE_OK) {
            if (Tmp32 != Track->VPSLen) {
                AmbaPrint_PrintStr5("%s: [ERROR] VPS not enough!", __func__, NULL, NULL, NULL, NULL);
                Rval = REFCODE_GENERAL_ERR;
            }
        } else {
            AmbaPrint_PrintStr5("%s: [ERROR] Read VPS fail!", __func__, NULL, NULL, NULL, NULL);
        }
    }
    if (Rval == REFCODE_OK) {
        Rval = RefCmptDemuxer_GetByte(File, &Tmp8);
    }
    /*sps*/
    if (Rval == REFCODE_OK) {
        Rval = RefCmptDemuxer_GetBe16(File, &Tmp16);
    }
    if (Rval == REFCODE_OK) {
        Rval = RefCmptDemuxer_GetBe16(File, &Track->SPSLen);
    }
    if (Rval == REFCODE_OK) {
        Rval = ST2A(File->Func->Read(File, Track->SPSLen, Track->SPS, &Tmp32));
        if (Rval == REFCODE_OK) {
            if (Tmp32 != Track->SPSLen) {
                AmbaPrint_PrintStr5("%s: [ERROR] SPS not enough!", __func__, NULL, NULL, NULL, NULL);
                Rval = REFCODE_GENERAL_ERR;
            }
        } else {
            AmbaPrint_PrintStr5("%s: [ERROR] Read SPS fail!", __func__, NULL, NULL, NULL, NULL);
        }
    }
    /*pps*/
    if (Rval == REFCODE_OK) {
        Rval = RefCmptDemuxer_GetByte(File, &Tmp8);
    }
    if (Rval == REFCODE_OK) {
        Rval = RefCmptDemuxer_GetBe16(File, &Tmp16);
    }
    if (Rval == REFCODE_OK) {
        Rval = RefCmptDemuxer_GetBe16(File, &Track->PPSLen);
    }
    if (Rval == REFCODE_OK) {
        Rval = ST2A(File->Func->Read(File, Track->PPSLen, Track->PPS, &Tmp32));
        if (Rval == REFCODE_OK) {
            if (Tmp32 != Track->PPSLen) {
                AmbaPrint_PrintStr5("%s: [ERROR] PPS not enough!", __func__, NULL, NULL, NULL, NULL);
                Rval = REFCODE_GENERAL_ERR;
            }
        } else {
            AmbaPrint_PrintStr5("%s: [ERROR] Read PPS fail!", __func__, NULL, NULL, NULL, NULL);
        }
    }
    return Rval;
}

static UINT32 RefCmptDemuxer_ParseVideoStsd(SVC_STREAM_HDLR_s *File, UINT64 ParseStartPos, UINT64 ParseEndPos, VIDEO_TRACK_INFO_s *Track)
{
    UINT8 Tmp8 = 0U;
    UINT16 Tmp16 = 0U;
    UINT32 Tmp32 = 0U;
    UINT32 BoxSize = 0U;
    UINT32 BoxTag = 0U;
    UINT64 ParsePos = ParseStartPos;
    UINT32 Count = 0U;
    UINT32 Rval = RefCmptDemuxer_GetByte(File, &Tmp8);
    if (Rval == REFCODE_OK) {
        Rval = RefCmptDemuxer_GetByte(File, &Tmp8);
    }
    if (Rval == REFCODE_OK) {
        Rval = RefCmptDemuxer_GetByte(File, &Tmp8);
    }
    if (Rval == REFCODE_OK) {
        Rval = RefCmptDemuxer_GetByte(File, &Tmp8);
    }
    if (Rval == REFCODE_OK) {
        Rval = RefCmptDemuxer_GetBe32(File, &Tmp32);
    }
    if (Rval == REFCODE_OK) {
        if (Tmp32 == 1U) {
            ParsePos += 8U;
            // TODO: BoxSize, maybe it is essential for seeking?
            Rval = RefCmptDemuxer_GetBe32(File, &Tmp32);
        } else {
            AmbaPrint_PrintStr5("%s: [ERROR] Incorrect entry error!", __func__, NULL, NULL, NULL, NULL);
            Rval = REFCODE_GENERAL_ERR;
        }
    }
    if (Rval == REFCODE_OK) {
        Rval = RefCmptDemuxer_GetBe32(File, &Tmp32);
    }
    if (Rval == REFCODE_OK) {
        UINT32 I;
        if (Tmp32 == AVC1) {
            Track->MediaId = SVC_FORMAT_MID_AVC;
        } else {
            Track->MediaId = SVC_FORMAT_MID_HVC;
        }
        for (I = 0U; I < 6U; I++) {
            Rval = RefCmptDemuxer_GetByte(File, &Tmp8);
            if (Rval != REFCODE_OK) {
                break;
            }
        }
    }
    if (Rval == REFCODE_OK) {
        // data reference index
        Rval = RefCmptDemuxer_GetBe16(File, &Tmp16);
    }
    if (Rval == REFCODE_OK) {
        // version
        Rval = RefCmptDemuxer_GetBe16(File, &Tmp16);
    }
    if (Rval == REFCODE_OK) {
        // revision level
        Rval = RefCmptDemuxer_GetBe16(File, &Tmp16);
    }
    if (Rval == REFCODE_OK) {
        // vendor
        Rval = RefCmptDemuxer_GetBe32(File, &Tmp32);
    }
    if (Rval == REFCODE_OK) {
        // temporal quality
        Rval = RefCmptDemuxer_GetBe32(File, &Tmp32);
    }
    if (Rval == REFCODE_OK) {
        // spatial quality
        Rval = RefCmptDemuxer_GetBe32(File, &Tmp32);
    }
    if (Rval == REFCODE_OK) {
        Rval = RefCmptDemuxer_GetBe16(File, &Track->VideoWidth);
    }
    if (Rval == REFCODE_OK) {
        Rval = RefCmptDemuxer_GetBe16(File, &Track->VideoHeight);
    }
    if (Rval == REFCODE_OK) {
        // horizontal resolution
        Rval = RefCmptDemuxer_GetBe32(File, &Tmp32);
    }
    if (Rval == REFCODE_OK) {
        // vertical resolution
        Rval = RefCmptDemuxer_GetBe32(File, &Tmp32);
    }
    if (Rval == REFCODE_OK) {
        Rval = RefCmptDemuxer_GetBe32(File, &Tmp32);
    }
    if (Rval == REFCODE_OK) {
        Rval = RefCmptDemuxer_GetBe16(File, &Tmp16);
    }
    if (Rval == REFCODE_OK) {
        UINT8 Name[ISO_STSD_MAX_ENC_NAME_LEN];
        Rval = ST2A(File->Func->Read(File, ISO_STSD_MAX_ENC_NAME_LEN, Name, &Count));
        if (Rval != REFCODE_OK) {
            AmbaPrint_PrintStr5("%s: [ERROR] Fail to read file", __func__, NULL, NULL, NULL, NULL);
        }
    }
    if (Rval == REFCODE_OK) {
        if (Count != ISO_STSD_MAX_ENC_NAME_LEN) {
            AmbaPrint_PrintStr5("%s: [ERROR] Read data from stream error!", __func__, NULL, NULL, NULL, NULL);
            Rval = REFCODE_GENERAL_ERR;
        }
    }
    if (Rval == REFCODE_OK) {
        Rval = RefCmptDemuxer_GetBe16(File, &Tmp16);
    }
    if (Rval == REFCODE_OK) {
        Rval = RefCmptDemuxer_GetBe16(File, &Tmp16);
    }
    if (Rval == REFCODE_OK) {
        ParsePos += 86U;
        do {
            if (ParsePos == ParseEndPos) {
                break;
            } else if (ParsePos > ParseEndPos) {
                AmbaPrint_PrintStr5("%s: [ERROR] IsoDmx_ParseVideoStsd() fail!", __func__, NULL, NULL, NULL, NULL);
                Rval = REFCODE_GENERAL_ERR;
            } else {
                Rval = RefCmptDemuxer_GetBoxHeader(File, &BoxSize, &BoxTag);
                if (Rval == REFCODE_OK) {
                    if (BoxTag == AVCC) {
                        Rval = RefCmptDemuxer_ParseVideoAvcc(File, Track);
                    } else if (BoxTag == HVCC) {
                        Rval = RefCmptDemuxer_ParseVideoHvcc(File, Track);
                    } else {
                        Rval = ST2A(File->Func->Seek(File, (INT64) ParsePos + (INT64) BoxSize, SVC_STREAM_SEEK_START));
                        if (Rval != REFCODE_OK) {
                            AmbaPrint_PrintStr5("%s: [ERROR] Fail to seek file", __func__, NULL, NULL, NULL, NULL);
                        }
                    }
                    if (Rval == REFCODE_OK) {
                        ParsePos += BoxSize;
                    }
                }
            }
        } while (Rval == REFCODE_OK);
    }
    return Rval;
}

static UINT32 RefCmptDemuxer_ParseStsc(SVC_STREAM_HDLR_s *File, VIDEO_TRACK_INFO_s *Track)
{
    UINT8 Tmp8;
    UINT32 Tmp32;
    UINT32 Rval = RefCmptDemuxer_GetByte(File, &Tmp8);
    if (Rval == REFCODE_OK) {
        Rval = RefCmptDemuxer_GetByte(File, &Tmp8);
    }
    if (Rval == REFCODE_OK) {
        Rval = RefCmptDemuxer_GetByte(File, &Tmp8);
    }
    if (Rval == REFCODE_OK) {
        Rval = RefCmptDemuxer_GetByte(File, &Tmp8);
    }
    if (Rval == REFCODE_OK) {
        Rval = RefCmptDemuxer_GetBe32(File, &Track->ChunkTableCount);
    }
    if (Rval == REFCODE_OK) {
        if (Track->ChunkTableCount <= MAX_CHUNK_TABLE_COUNT) {
            UINT32 I;
            for (I = 0; I < Track->ChunkTableCount; I++) {
                Rval = RefCmptDemuxer_GetBe32(File, &Track->FirstChunk[I]);
                if (Rval == REFCODE_OK) {
                    Rval = RefCmptDemuxer_GetBe32(File, &Track->FramePerChunk[I]);
                }
                if (Rval == REFCODE_OK) {
                    Rval = RefCmptDemuxer_GetBe32(File, &Tmp32);
                }
                if (Rval != REFCODE_OK) {
                    break;
                }
                AmbaPrint_PrintUInt5("RefCmptDemuxer_ParseStsc: Chunk table[%u] first %u size %u", I, Track->FirstChunk[I], Track->FramePerChunk[I], 0, 0);
            }
        } else {
            AmbaPrint_PrintUInt5("RefCmptDemuxer_ParseStsc: [ERROR] Incorrect chunk table count %u!", Track->ChunkTableCount, 0, 0, 0, 0);
            Rval = REFCODE_GENERAL_ERR;
        }
    }
    return Rval;
}

static UINT32 RefCmptDemuxer_ParseStsz(SVC_STREAM_HDLR_s *File, VIDEO_TRACK_INFO_s *Track)
{
    UINT8 Tmp8;
    UINT32 Tmp32;
    UINT32 Rval = RefCmptDemuxer_GetByte(File, &Tmp8);
    if (Rval == REFCODE_OK) {
        Rval = RefCmptDemuxer_GetByte(File, &Tmp8);
    }
    if (Rval == REFCODE_OK) {
        Rval = RefCmptDemuxer_GetByte(File, &Tmp8);
    }
    if (Rval == REFCODE_OK) {
        Rval = RefCmptDemuxer_GetByte(File, &Tmp8);
    }
    if (Rval == REFCODE_OK) {
        Rval = RefCmptDemuxer_GetBe32(File, &Tmp32);
    }
    if (Rval == REFCODE_OK) {
        Rval = RefCmptDemuxer_GetBe32(File, &Track->FrameCount);
    }
    if (Rval == REFCODE_OK) {
        if ((Track->FrameCount > 0U) && (Track->FrameCount <= MAX_FRAME_COUNT)) {
            const UINT32 *AddrU32 = Track->FrameSize;
            UINT8 *AddrU8;
            AmbaMisra_TypeCast(&AddrU8, &AddrU32);
            Rval = ST2A(File->Func->Read(File, Track->FrameCount * sizeof(UINT32), AddrU8, &Tmp32));
            if (Rval == REFCODE_OK) {
                if (Tmp32 == (Track->FrameCount * sizeof(UINT32))) {
                    UINT32 I;
                    for (I = 0; I < Track->FrameCount; I++) {
                        Track->FrameSize[I] = RefCmptDemuxer_Le2Be32(Track->FrameSize[I]);
#if REF_CMPT_PRINT_DBG
                        AmbaPrint_PrintUInt5("RefCmptDemuxer_ParseStsz: Frame[%u] size %p", I, Track->FrameSize[I], 0U, 0U, 0U);
#endif
                    }
                } else {
                    AmbaPrint_PrintStr5("%s: [ERROR] Data not enough!", __func__, NULL, NULL, NULL, NULL);
                    Rval = REFCODE_GENERAL_ERR;
                }
            } else {
                AmbaPrint_PrintStr5("%s: [ERROR] Fail to read file", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            AmbaPrint_PrintUInt5("RefCmptDemuxer_ParseStsz: [ERROR] Incorrect frame count %u!", Track->FrameCount, 0U, 0U, 0U, 0U);
            Rval = REFCODE_GENERAL_ERR;
        }
    }
    return Rval;
}

static UINT32 RefCmptDemuxer_ParseStco(SVC_STREAM_HDLR_s *File, VIDEO_TRACK_INFO_s *Track)
{
    UINT8 Tmp8;
    UINT32 Tmp32;
    UINT32 Rval = RefCmptDemuxer_GetByte(File, &Tmp8);
    if (Rval == REFCODE_OK) {
        Rval = RefCmptDemuxer_GetByte(File, &Tmp8);
    }
    if (Rval == REFCODE_OK) {
        Rval = RefCmptDemuxer_GetByte(File, &Tmp8);
    }
    if (Rval == REFCODE_OK) {
        Rval = RefCmptDemuxer_GetByte(File, &Tmp8);
    }
    if (Rval == REFCODE_OK) {
        Rval = RefCmptDemuxer_GetBe32(File, &Track->ChunkCount);
    }
    if (Rval == REFCODE_OK) {
        if ((Track->ChunkCount > 0U) && (Track->ChunkCount < MAX_CHUNK_COUNT)) {
            const UINT32 *AddrU32 = Track->ChunkPos;
            UINT8 *AddrU8;
            AmbaMisra_TypeCast(&AddrU8, &AddrU32);
            Rval = ST2A(File->Func->Read(File, Track->ChunkCount * sizeof(UINT32), AddrU8, &Tmp32));
            if (Rval == REFCODE_OK) {
                if (Tmp32 == (Track->ChunkCount * sizeof(UINT32))) {
                    UINT32 I;
                    for (I = 0; I < Track->ChunkCount; I++) {
                        Track->ChunkPos[I] = RefCmptDemuxer_Le2Be32(Track->ChunkPos[I]);
#if REF_CMPT_PRINT_DBG
                        AmbaPrint_PrintUInt5("RefCmptDemuxer_ParseStco: Chunk[%u] at %p", I, Track->ChunkPos[I], 0U, 0U, 0U);
#endif
                    }
                } else {
                    AmbaPrint_PrintStr5("%s: [ERROR] Data not enough!", __func__, NULL, NULL, NULL, NULL);
                    Rval = REFCODE_GENERAL_ERR;
                }
            } else {
                AmbaPrint_PrintStr5("%s: [ERROR] Fail to read file", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            AmbaPrint_PrintUInt5("RefCmptDemuxer_ParseStco: [ERROR] Incorrect chunk count %u!", Track->ChunkCount, 0U, 0U, 0U, 0U);
            Rval = REFCODE_GENERAL_ERR;
        }
    }
    return Rval;
}

static UINT32 RefCmptDemuxer_ParseVideoStbl(SVC_STREAM_HDLR_s *File, UINT64 ParseStartPos, UINT64 ParseEndPos, VIDEO_TRACK_INFO_s *Track)
{
    UINT32 Rval = REFCODE_OK;
    UINT64 ParsePos = ParseStartPos;
    UINT32 BoxSize;
    UINT32 BoxTag;
    do {
        if (ParsePos == ParseEndPos) {
            break;
        } else if (ParsePos > ParseEndPos) {
            AmbaPrint_PrintStr5("%s: [ERROR] ParseStbl() fail!", __func__, NULL, NULL, NULL, NULL);
            Rval = REFCODE_GENERAL_ERR;
        } else {
            Rval = RefCmptDemuxer_GetBoxHeader(File, &BoxSize, &BoxTag);
            if (Rval == REFCODE_OK) {
                if (BoxTag == STSD) {
                    Rval = RefCmptDemuxer_ParseVideoStsd(File, ParsePos + AMBA_ISO_BOX_SIZE_TAG_SIZE, ParsePos + BoxSize, Track);
                } else if (BoxTag == STSC) {
                    Rval = RefCmptDemuxer_ParseStsc(File, Track);
                } else if (BoxTag == STSZ) {
                    Rval = RefCmptDemuxer_ParseStsz(File, Track);
                } else if (BoxTag == STCO) {
                    Rval = RefCmptDemuxer_ParseStco(File, Track);
                } else {
                    Rval = ST2A(File->Func->Seek(File, (INT64) ParsePos + (INT64) BoxSize, SVC_STREAM_SEEK_START));
                    if (Rval != REFCODE_OK) {
                        AmbaPrint_PrintStr5("%s: [ERROR] Fail to seek file", __func__, NULL, NULL, NULL, NULL);
                    }
                }
                if (Rval == REFCODE_OK) {
                    ParsePos += BoxSize;
                }
            }
        }
    } while (Rval == REFCODE_OK);
    if (Rval == REFCODE_OK) {
        UINT32 I, J, K;
        UINT32 ChunkCount = 0;
        UINT32 FrameCount = 0;
        for (I = 0; I < Track->ChunkTableCount; I++) {
            // convert FirstChunk to NumChunk of each table
            if (I == (Track->ChunkTableCount - 1U)) {
                Track->FirstChunk[I] = Track->ChunkCount + 1U - Track->FirstChunk[I];
            } else {
                Track->FirstChunk[I] = Track->FirstChunk[I + 1U] - Track->FirstChunk[I];
            }
            AmbaPrint_PrintUInt5("RefCmptDemuxer_ParseVideoStbl: Total %u chunks for chunk table %u", Track->FirstChunk[I], I, 0U, 0U, 0U);
        }
        for (I = 0U; I < Track->ChunkTableCount; I++) {
            for (J = 0U; J < Track->FirstChunk[I]; J++) {
                for (K = 0U; K < Track->FramePerChunk[I]; K++) {
                    if (K == 0U) {
                        Track->FramePos[FrameCount] = Track->ChunkPos[ChunkCount];
                    } else {
                        if (FrameCount > 0U) { /* Should always be true. Add this check to avoid MisraC error: Possibly negative subscript (-1) in operator '[' */
                            Track->FramePos[FrameCount] = Track->FramePos[FrameCount - 1U] + Track->FrameSize[FrameCount - 1U];
                        }
                    }
#if REF_CMPT_PRINT_DBG
                    AmbaPrint_PrintUInt5("RefCmptDemuxer_ParseVideoStbl: Frame[%u] Pos %p Size %p", FrameCount, Track->FramePos[FrameCount], Track->FrameSize[FrameCount], 0, 0);
#endif
                    FrameCount++;
                }
                ChunkCount++;
            }
        }
        if ((ChunkCount != Track->ChunkCount) || (FrameCount != Track->FrameCount)) {
            AmbaPrint_PrintUInt5("RefCmptDemuxer_ParseVideoStbl: [ERROR] Incorrect chunk count %u/%u or frame count %u/%u", ChunkCount, Track->ChunkCount, FrameCount, Track->FrameCount, 0);
            Rval = REFCODE_GENERAL_ERR;
        }
    }
    return Rval;
}

static UINT32 RefCmptDemuxer_ParseVideoMinf(SVC_STREAM_HDLR_s *File, UINT64 ParseStartPos, UINT64 ParseEndPos, VIDEO_TRACK_INFO_s *Track)
{
    UINT32 Rval = REFCODE_OK;
    UINT64 ParsePos = ParseStartPos;
    UINT32 BoxSize;
    UINT32 BoxTag;
    do {
        if (ParsePos == ParseEndPos) {
            break;
        } else if (ParsePos > ParseEndPos) {
            AmbaPrint_PrintStr5("%s: [ERROR] ParseMinf() fail!", __func__, NULL, NULL, NULL, NULL);
            Rval = REFCODE_GENERAL_ERR;
        } else {
            /* Get Box Size, Tag */
            Rval = RefCmptDemuxer_GetBoxHeader(File, &BoxSize, &BoxTag);
            if (Rval == REFCODE_OK) {
                if (BoxTag == STBL) {
                    Rval = RefCmptDemuxer_ParseVideoStbl(File, ParsePos + AMBA_ISO_BOX_SIZE_TAG_SIZE, ParsePos + BoxSize, Track);
                } else {
                    Rval = ST2A(File->Func->Seek(File, (INT64) ParsePos + (INT64) BoxSize, SVC_STREAM_SEEK_START));
                    if (Rval != REFCODE_OK) {
                        AmbaPrint_PrintStr5("%s: [ERROR] Fail to seek file", __func__, NULL, NULL, NULL, NULL);
                    }
                }
                if (Rval == REFCODE_OK) {
                    ParsePos += BoxSize;
                }
            }
        }
    } while (Rval == REFCODE_OK);
    return Rval;
}

static UINT32 RefCmptDemuxer_ParseVideoMdia(SVC_STREAM_HDLR_s *File, UINT64 ParseStartPos, UINT64 ParseEndPos, VIDEO_TRACK_INFO_s *Track)
{
    UINT32 Rval = REFCODE_OK;
    UINT64 ParsePos = ParseStartPos;
    UINT32 BoxSize;
    UINT32 BoxTag;
    do {
        if (ParsePos == ParseEndPos) {
            break;
        } else if (ParsePos > ParseEndPos) {
            AmbaPrint_PrintStr5("%s: [ERROR] ParseTrak() fail!", __func__, NULL, NULL, NULL, NULL);
            Rval = REFCODE_GENERAL_ERR;
        } else {
            /* Get Box Size, Tag */
            Rval = RefCmptDemuxer_GetBoxHeader(File, &BoxSize, &BoxTag);
            if (Rval == REFCODE_OK) {
                if (BoxTag == MINF) {
                    Rval = RefCmptDemuxer_ParseVideoMinf(File, ParsePos + AMBA_ISO_BOX_SIZE_TAG_SIZE, ParsePos + BoxSize, Track);
                } else {
                    Rval = ST2A(File->Func->Seek(File, (INT64) ParsePos + (INT64) BoxSize, SVC_STREAM_SEEK_START));
                    if (Rval != REFCODE_OK) {
                        AmbaPrint_PrintStr5("%s: [ERROR] Fail to seek file", __func__, NULL, NULL, NULL, NULL);
                    }
                }
                if (Rval == REFCODE_OK) {
                    ParsePos += BoxSize;
                }
            }
        }
    } while (Rval == REFCODE_OK);
    return Rval;
}

static UINT32 RefCmptDemuxer_ParseVideoTrak(SVC_STREAM_HDLR_s *File, UINT64 ParseStartPos, UINT64 ParseEndPos, VIDEO_TRACK_INFO_s *Track)
{
    UINT32 Rval = REFCODE_OK;
    UINT64 ParsePos = ParseStartPos;
    UINT32 BoxSize;
    UINT32 BoxTag;
    do {
        if (ParsePos == ParseEndPos) {
            break;
        } else if (ParsePos > ParseEndPos) {
            AmbaPrint_PrintStr5("%s: [ERROR] ParseTrak() fail!", __func__, NULL, NULL, NULL, NULL);
            Rval = REFCODE_GENERAL_ERR;
        } else {
            /* Get Box Size, Tag */
            Rval = RefCmptDemuxer_GetBoxHeader(File, &BoxSize, &BoxTag);
            if (Rval == REFCODE_OK) {
                if (BoxTag == MDIA) {
                    Rval = RefCmptDemuxer_ParseVideoMdia(File, ParsePos + AMBA_ISO_BOX_SIZE_TAG_SIZE, ParsePos + BoxSize, Track);
                } else {
                    Rval = ST2A(File->Func->Seek(File, (INT64) ParsePos + (INT64) BoxSize, SVC_STREAM_SEEK_START));
                    if (Rval != REFCODE_OK) {
                        AmbaPrint_PrintStr5("%s: [ERROR] Fail to seek file", __func__, NULL, NULL, NULL, NULL);
                    }
                }
                if (Rval == REFCODE_OK) {
                    ParsePos += BoxSize;
                }
            }
        }
    } while (Rval == REFCODE_OK);
    return Rval;
}

static UINT32 RefCmptDemuxer_ParseMoov(SVC_STREAM_HDLR_s *File, UINT64 ParseStartPos, UINT64 ParseEndPos, VIDEO_TRACK_INFO_s *Track)
{
    UINT32 Rval = REFCODE_OK;
    UINT64 ParsePos = ParseStartPos;
    UINT32 BoxSize;
    UINT32 BoxTag;
    UINT8 TrackCount = 0;
    do {
        if (ParsePos == ParseEndPos) {
            break;
        } else if (ParsePos > ParseEndPos) {
            AmbaPrint_PrintStr5("%s: [ERROR] Parse position incorrect!", __func__, NULL, NULL, NULL, NULL);
            Rval = REFCODE_GENERAL_ERR;
        } else {
            /* Get Box Size, Tag */
            Rval = RefCmptDemuxer_GetBoxHeader(File, &BoxSize, &BoxTag);
            if (Rval == REFCODE_OK) {
                if ((BoxTag == TRAK) && (TrackCount == 0U)) {
                    // trick, only parse the first TRAK (must be video)
                    Rval = RefCmptDemuxer_ParseVideoTrak(File, ParsePos + AMBA_ISO_BOX_SIZE_TAG_SIZE, ParsePos + BoxSize, Track);
                    TrackCount++;
                } else {
                    Rval = ST2A(File->Func->Seek(File, (INT64) ParsePos + (INT64) BoxSize, SVC_STREAM_SEEK_START));
                    if (Rval != REFCODE_OK) {
                        AmbaPrint_PrintStr5("%s: [ERROR] Fail to seek file", __func__, NULL, NULL, NULL, NULL);
                    }
                }
                if (Rval == REFCODE_OK) {
                    ParsePos += BoxSize;
                }
            }
        }
    } while (Rval == REFCODE_OK);
    return Rval;
}

static UINT32 RefCmptDemuxer_GetFileLength(SVC_STREAM_HDLR_s *File, UINT64 *Length)
{
    UINT32 Rval;
    Rval = ST2A(File->Func->GetLength(File, Length));
    if (Rval != REFCODE_OK) {
        AmbaPrint_PrintUInt5("RefCmptDemuxer_GetFileLength: [ERROR] Fail to get length", 0U, 0U, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 RefCmptDemuxer_Parse(SVC_STREAM_HDLR_s *File, VIDEO_TRACK_INFO_s *Track)
{
    UINT32 Rval;
    UINT64 ParseEndPos;
    Rval = RefCmptDemuxer_GetFileLength(File, &ParseEndPos);
    if (Rval == REFCODE_OK) {
        Rval = ST2A(File->Func->Seek(File, 0, SVC_STREAM_SEEK_START));
        if (Rval == REFCODE_OK) {
            UINT64 ParsePos = 0;
            UINT32 BoxSize;
            UINT32 BoxTag;
            do {
                if (ParsePos == ParseEndPos) {
                    break;
                } else if (ParsePos > ParseEndPos) {
                    AmbaPrint_PrintStr5("%s: [ERROR] parse position incorrect!", __func__, NULL, NULL, NULL, NULL);
                    Rval = REFCODE_GENERAL_ERR;
                } else {
                    /* Get Box Size, Tag */
                    Rval = RefCmptDemuxer_GetBoxHeader(File, &BoxSize, &BoxTag);
                    if (Rval == REFCODE_OK) {
                        if (BoxTag == MOOV) {
                            Rval = RefCmptDemuxer_ParseMoov(File, ParsePos + AMBA_ISO_BOX_SIZE_TAG_SIZE, ParsePos + BoxSize, Track);
                        } else {
                            Rval = ST2A(File->Func->Seek(File, (INT64) ParsePos + (INT64) BoxSize, SVC_STREAM_SEEK_START));
                            if (Rval != REFCODE_OK) {
                                AmbaPrint_PrintStr5("%s: [ERROR] Seek failed in FREE!", __func__, NULL, NULL, NULL, NULL);
                            }
                        }
                        if (Rval == REFCODE_OK) {
                            ParsePos += BoxSize;
                        }
                    }
                }
            } while (Rval == REFCODE_OK);
        } else {
            AmbaPrint_PrintStr5("%s: [ERROR] Seek the beginning of the file failed!", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_PrintStr5("%s: [ERROR] Get file length error!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

static UINT32 RefCmptDemuxer_ReadFileImpl(SVC_STREAM_HDLR_s *File, UINT32 Size, UINT8 *Addr)
{
    UINT32 Rval;
    UINT32 Count;
    Rval = ST2A(File->Func->Read(File, Size, Addr, &Count));
    if (Rval == REFCODE_OK) {
        if (Count != Size) {
            AmbaPrint_PrintStr5("%s: [ERROR] Read data from Stream fail!", __func__, NULL, NULL, NULL, NULL);
            Rval = REFCODE_GENERAL_ERR;
        }
    } else {
        AmbaPrint_PrintStr5("%s: [ERROR] Fail to read file", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

static UINT32 RefCmptDemuxer_ReadFile(SVC_STREAM_HDLR_s *File, UINT32 Size, UINT8 *Addr, UINT8 *Base, const UINT8 *Limit, UINT8 **NewAddr)
{
    UINT32 Rval = REFCODE_OK;
    ULONG  AddrU32, BaseU32, LimitU32;

    AmbaMisra_TypeCast(&AddrU32, &Addr);
    AmbaMisra_TypeCast(&BaseU32, &Base);
    AmbaMisra_TypeCast(&LimitU32, &Limit);

    if ((AddrU32 < BaseU32) || (AddrU32 >= LimitU32)) {
        AmbaPrint_PrintStr5("%s: [ERROR] Wrong Addr!", __func__, NULL, NULL, NULL, NULL);
        Rval = REFCODE_GENERAL_ERR;
    } else {
        UINT32 RetU32 = 0U;
        if ((AddrU32 + Size) <= LimitU32) {
            Rval = RefCmptDemuxer_ReadFileImpl(File, Size, Addr);
            if (Rval == REFCODE_OK) {
                RetU32 = AddrU32 + Size;
            }
        } else {
            const ULONG Rear = LimitU32 - AddrU32;
            Rval = RefCmptDemuxer_ReadFileImpl(File, Rear, Addr);
            if (Rval == REFCODE_OK){
                Rval = RefCmptDemuxer_ReadFileImpl(File, Size - Rear, Base);
                if (Rval == REFCODE_OK) {
                    RetU32 = BaseU32 + Size - Rear;
                }
            }
        }
        if (Rval == REFCODE_OK) {
            UINT8 *Temp = NULL;
            AmbaMisra_TypeCast(&Temp, &RetU32);
            if (Temp == Limit) {
                Temp = Base;
            }
            *NewAddr = Temp;
        }
    }
    return Rval;
}

static UINT32 RefCmptDemuxer_ReadMem(const UINT8 *Data, UINT32 Size, UINT8 **Addr, UINT8 *Base, const UINT8 *Limit)
{
    UINT32 Rval = REFCODE_OK;
    UINT8 *DstAddr = *Addr;
    UINT8 *NewAddr;
    ULONG AddrU32, BaseU32, LimitU32, NewAddrU32;

    AmbaMisra_TypeCast(&AddrU32, &DstAddr);
    AmbaMisra_TypeCast(&BaseU32, &Base);
    AmbaMisra_TypeCast(&LimitU32, &Limit);

    if ((AddrU32 < BaseU32) || (AddrU32 >= LimitU32)) {
        AmbaPrint_PrintStr5("%s: [ERROR] Wrong Addr!", __func__, NULL, NULL, NULL, NULL);
        Rval = REFCODE_GENERAL_ERR;
    } else {
        if ((AddrU32 + Size) <= LimitU32) {
            (void)AmbaWrap_memcpy(DstAddr, Data, Size);
            NewAddrU32 = AddrU32 + Size;
        } else {
            const UINT32 Rear = LimitU32 - AddrU32;
            (void)AmbaWrap_memcpy(DstAddr, Data, Rear);
            (void)AmbaWrap_memcpy(Base, &Data[Rear], Size - Rear);
            NewAddrU32 = BaseU32 + Size - Rear;
        }
        AmbaMisra_TypeCast(&NewAddr, &NewAddrU32);
        if (NewAddr == Limit) {
            NewAddr = Base;
        }
        *Addr = NewAddr;
    }
    return Rval;
}

static UINT32 RefCmptDemuxer_ReadH264(SVC_STREAM_HDLR_s *File, UINT8 *Addr, UINT8 *Base, const UINT8 *Limit, UINT32 FrameSize, UINT32 *DataSize)
{
    UINT32 Rval = REFCODE_OK;
    UINT8 Start[AMBA_H264_STRAT_CODE_SIZE] = {0, 0, 0, 1};
    UINT32 Tmp32;
    UINT32 TmpFrameSize;
    UINT8 *AddrTmp = Addr;
    *DataSize = 0;
    TmpFrameSize = FrameSize;
    while (TmpFrameSize != 0U) {
        Rval = RefCmptDemuxer_GetBe32(File, &Tmp32);
        if (Rval != REFCODE_OK) {
            AmbaPrint_PrintStr5("%s: [ERROR] RefCmptDemuxer_GetBe32 fail!", __func__, NULL, NULL, NULL, NULL);
        }
        if (Rval == REFCODE_OK) {
            Rval = RefCmptDemuxer_ReadMem(Start, AMBA_H264_STRAT_CODE_SIZE, &AddrTmp, Base, Limit);
            if (Rval != REFCODE_OK) {
                AmbaPrint_PrintStr5("%s: [ERROR] RefCmptDemuxer_ReadMem fail!", __func__, NULL, NULL, NULL, NULL);
            }
        }
        if (Rval == REFCODE_OK) {
            Rval = RefCmptDemuxer_ReadFile(File, Tmp32, AddrTmp, Base, Limit, &AddrTmp);
            if (Rval != REFCODE_OK) {
                AmbaPrint_PrintStr5("%s: [ERROR] RefCmptDemuxer_ReadFile fail!", __func__, NULL, NULL, NULL, NULL);
            }
        }
        if (Rval != REFCODE_OK) {
            break;
        }
        TmpFrameSize -= (Tmp32 + 4U);
        *DataSize += (Tmp32 + AMBA_H264_STRAT_CODE_SIZE);
    }
    return Rval;
}

static UINT32 RefCmptDemuxer_ReadH265(SVC_STREAM_HDLR_s *File, UINT8 *Addr, UINT8 *Base, const UINT8 *Limit, UINT32 FrameSize, UINT32 *DataSize)
{
    UINT32 Rval = REFCODE_OK;
    UINT8 Start[AMBA_H265_STRAT_CODE_SIZE] = {0, 0, 0, 1};
    UINT32 TmpU32;
    UINT32 TmpFrameSize;
    UINT8 *AddrTmp = Addr;
    *DataSize = 0;
    TmpFrameSize = FrameSize;
    while (TmpFrameSize != 0U) {
        Rval = RefCmptDemuxer_GetBe32(File, &TmpU32);
        if (Rval != REFCODE_OK) {
            AmbaPrint_PrintStr5("%s: [ERROR] RefCmptDemuxer_GetBe32 fail!", __func__, NULL, NULL, NULL, NULL);
        }
        if (Rval == REFCODE_OK) {
            Rval = RefCmptDemuxer_ReadMem(Start, AMBA_H265_STRAT_CODE_SIZE, &AddrTmp, Base, Limit);
            if (Rval != REFCODE_OK) {
                AmbaPrint_PrintStr5("%s: [ERROR] RefCmptDemuxer_ReadMem fail!", __func__, NULL, NULL, NULL, NULL);
            }
        }
        if (Rval == REFCODE_OK) {
            Rval = RefCmptDemuxer_ReadFile(File, TmpU32, AddrTmp, Base, Limit, &AddrTmp);
            if (Rval != REFCODE_OK) {
                AmbaPrint_PrintStr5("%s: [ERROR] RefCmptDemuxer_ReadFile fail!", __func__, NULL, NULL, NULL, NULL);
            }
        }
        if (Rval == REFCODE_OK) {
            TmpFrameSize -= (TmpU32 + 4U);
            *DataSize += (TmpU32 + AMBA_H265_STRAT_CODE_SIZE);
        }
        if (Rval != REFCODE_OK) {
            break;
        }
    }
    return Rval;
}

#define REF_CMPT_CLOCK   (90000U)        /**< The standard clock frequency of the system (It is 90KHz in the system.) */
static inline UINT64 REF_CMPT_NORMALIZE_CLOCK(UINT64 X, UINT64 Y)   {return (X * REF_CMPT_CLOCK) / (Y);}   /**< Normalize the codec clock to DSP Clock. */
static inline UINT16 REF_CMPT_HIGH_WORD(UINT32 x) {return (UINT16)((x >> 16) & 0x0000FFFFU);}
static inline UINT16 REF_CMPT_LOW_WORD(UINT32 x) {return (UINT16)(x & 0x0000FFFFU);}

static UINT32 RefCmptDemuxer_PutH264GOPHeader(REF_CMPT_DEMUX_HDLR_s* DemuxHdlr, const VIDEO_TRACK_INFO_s *Track, UINT8 SkipFirstI, UINT8 SkipLastI, UINT64 PTS)
{
    UINT32 Rval = REFCODE_OK;
    UINT16 High, Low;
    UINT8 Header[AMBA_H264_GOP_NALU_SIZE];
    UINT64 TmpPts = PTS;
    TmpPts = REF_CMPT_NORMALIZE_CLOCK(TmpPts, DemuxHdlr->AvcUserData.nTimeScale);
    AmbaPrint_PrintUInt5("RefCmptDemuxer_PutH264GOPHeader: Pts = %u TimeScale = %u TimePerFrame = %u M = %u N = %u",
                         (UINT32)TmpPts, DemuxHdlr->AvcUserData.nTimeScale, DemuxHdlr->AvcUserData.nTimePerFrame,  DemuxHdlr->AvcUserData.nM, DemuxHdlr->AvcUserData.nN);
    Header[0] = (UINT8)0x00;
    Header[1] = (UINT8)0x00;
    Header[2] = (UINT8)0x00;
    Header[3] = (UINT8)0x01;
    /* NAL Header */
    Header[4] = (UINT8)0x7a;
    /* Main version */
    Header[5] = (UINT8)0x01;
    /* Sub version */
    Header[6] = (UINT8)0x01;
    High = REF_CMPT_HIGH_WORD(DemuxHdlr->AvcUserData.nTimePerFrame);
    Low = REF_CMPT_LOW_WORD(DemuxHdlr->AvcUserData.nTimePerFrame);
    Header[7] = ((UINT8)SkipFirstI << (8U - 1U)) | ((UINT8)SkipLastI << (7U - 1U)) | ((UINT8)(High >> (16U - 6U)));
    Header[8] = ((UINT8)(High >> (10U - 8U)));
    Header[9] = ((UINT8)(High << (8U - 2U))) | (1U << (6U - 1U)) | ((UINT8)(Low >> (16U - 5U)));
    Header[10] = ((UINT8)(Low >> (11U - 8U)));
    High = REF_CMPT_HIGH_WORD(DemuxHdlr->AvcUserData.nTimeScale);
    Header[11] = ((UINT8)(Low << (8U - 3U))) | (1U << (5U - 1U)) | ((UINT8)(High >> (16U - 4U)));
    Header[12] = ((UINT8)(High >> (12U - 8U)));
    Low = REF_CMPT_LOW_WORD(DemuxHdlr->AvcUserData.nTimeScale);
    Header[13] = ((UINT8)(High << (8U - 4U))) | (1U << (4U - 1U)) | ((UINT8)(Low >> (16U - 3U)));
    Header[14] = ((UINT8)(Low >> (13U - 8U)));
    High = REF_CMPT_HIGH_WORD((UINT32)TmpPts);
    Header[15] = ((UINT8)(Low << (8U - 5U))) | (1U << (3U - 1U)) | ((UINT8)(High >> (16U - 2U)));
    Header[16] = ((UINT8)(High >> (14U - 8U)));
    Low = REF_CMPT_LOW_WORD((UINT32)TmpPts);
    Header[17] = ((UINT8)(High << (8U - 6U))) | (1U << (2U - 1U)) | ((UINT8)(Low >> (16U - 1U)));
    Header[18] = ((UINT8)(Low >> (15U - 8U)));
    Header[19] = ((UINT8)(Low << (8U - 7U))) | (1U >> (1U - 1U));
    Header[20] = ((UINT8)DemuxHdlr->AvcUserData.nN << (8U - 8U));
    Header[21] = ((UINT8)DemuxHdlr->AvcUserData.nM << (8U - 4U)) & 0xf0U;
    Rval = RefCmptDemuxer_ReadMem(Header, AMBA_H264_GOP_NALU_SIZE, &DemuxHdlr->AvcRawWritePoint, Track->BufferBase, Track->BufferLimit);
    return Rval;
}

static UINT32 RefCmptDemuxer_PutH265GOPHeader(REF_CMPT_DEMUX_HDLR_s* DemuxHdlr, const VIDEO_TRACK_INFO_s *Track, UINT8 SkipFirstI, UINT8 SkipLastI, UINT64 PTS)
{
    UINT32 Rval = REFCODE_OK;
    UINT8 Header[AMBA_H265_GOP_NALU_SIZE];
    UINT16 High, Low;
    UINT64 PtsTmp = PTS;
    PtsTmp = REF_CMPT_NORMALIZE_CLOCK(PtsTmp, DemuxHdlr->AvcUserData.nTimeScale);
    AmbaPrint_PrintUInt5("RefCmptDemuxer_PutH265GOPHeader: Pts = %u TimeScale = %u TimePerFrame = %u M = %u N = %u",
                         (UINT32)PtsTmp, DemuxHdlr->AvcUserData.nTimeScale, DemuxHdlr->AvcUserData.nTimePerFrame,  DemuxHdlr->AvcUserData.nM, DemuxHdlr->AvcUserData.nN);
    Header[0] = (UINT8)0x00;
    Header[1] = (UINT8)0x00;
    Header[2] = (UINT8)0x01;
    Header[3] = (UINT8)0x34;
    /* NAL Header */
    Header[4] = (UINT8)0x00;
    /* Main version */
    Header[5] = (UINT8)0x01;
    /* Sub version */
    Header[6] = (UINT8)0x01;
    High = REF_CMPT_HIGH_WORD(DemuxHdlr->AvcUserData.nTimePerFrame);
    Low = REF_CMPT_LOW_WORD(DemuxHdlr->AvcUserData.nTimePerFrame);
    Header[7] = ((UINT8)SkipFirstI << (8U - 1U)) | ((UINT8)SkipLastI << (7U - 1U)) | ((UINT8)(High >> (16U - 6U)));
    Header[8] = ((UINT8)(High >> (10U - 8U)));
    Header[9] = ((UINT8)(High << (8U - 2U))) | (1U << (6U - 1U)) | ((UINT8)(Low >> (16U - 5U)));
    Header[10] = ((UINT8)(Low >> (11U - 8U)));
    High = REF_CMPT_HIGH_WORD(DemuxHdlr->AvcUserData.nTimeScale);
    Header[11] = ((UINT8)(Low << (8U - 3U))) | (1U << (5U - 1U)) | ((UINT8)(High >> (16U - 4U)));
    Header[12] = ((UINT8)(High >> (12U - 8U)));
    Low = REF_CMPT_LOW_WORD(DemuxHdlr->AvcUserData.nTimeScale);
    Header[13] = ((UINT8)(High << (8U - 4U))) | (1U << (4U - 1U)) | ((UINT8)(Low >> (16U - 3U)));
    Header[14] = ((UINT8)(Low >> (13U - 8U)));
    High = REF_CMPT_HIGH_WORD((UINT32)PtsTmp);
    Header[15] = ((UINT8)(Low << (8U - 5U))) | (1U << (3U - 1U)) | ((UINT8)(High >> (16U - 2U)));
    Header[16] = ((UINT8)(High >> (14U - 8U)));
    Low = REF_CMPT_LOW_WORD((UINT32)PtsTmp);
    Header[17] = ((UINT8)(High << (8U - 6U))) | (1U << (2U - 1U)) | ((UINT8)(Low >> (16U - 1U)));
    Header[18] = ((UINT8)(Low >> (15U - 8U)));
    Header[19] = ((UINT8)(Low << (8U - 7U))) | (1U >> (1U - 1U));
    Header[20] = ((UINT8)DemuxHdlr->AvcUserData.nN << (8U - 8U));
    Header[21] = ((UINT8)DemuxHdlr->AvcUserData.nM << (8U - 4U)) & 0xf0U;
    Rval = RefCmptDemuxer_ReadMem(Header, AMBA_H265_GOP_NALU_SIZE, &DemuxHdlr->AvcRawWritePoint, Track->BufferBase, Track->BufferLimit);
    return Rval;
}

static UINT32 RefCmptDemuxer_PutSPSPPS(REF_CMPT_DEMUX_HDLR_s* DemuxHdlr, const VIDEO_TRACK_INFO_s *Track)
{
    UINT32 Rval = REFCODE_GENERAL_ERR;
    UINT8 Start[AMBA_H264_STRAT_CODE_SIZE] = {0, 0, 0, 1};
    Rval = RefCmptDemuxer_ReadMem(Start, AMBA_H264_STRAT_CODE_SIZE, &DemuxHdlr->AvcRawWritePoint, Track->BufferBase, Track->BufferLimit);
    if (Rval == REFCODE_OK) {
        Rval = RefCmptDemuxer_ReadMem(Track->SPS, Track->SPSLen, &DemuxHdlr->AvcRawWritePoint, Track->BufferBase, Track->BufferLimit);
        if (Rval == REFCODE_OK) {
            Rval = RefCmptDemuxer_ReadMem(Start, AMBA_H264_STRAT_CODE_SIZE, &DemuxHdlr->AvcRawWritePoint, Track->BufferBase, Track->BufferLimit);
            if (Rval == REFCODE_OK) {
                Rval = RefCmptDemuxer_ReadMem(Track->PPS, Track->PPSLen, &DemuxHdlr->AvcRawWritePoint, Track->BufferBase, Track->BufferLimit);
            }
        }
    }
    return Rval;
}

static UINT32 RefCmptDemuxer_PutVPSSPSPPS(REF_CMPT_DEMUX_HDLR_s* DemuxHdlr, const VIDEO_TRACK_INFO_s *Track)
{
    UINT32 Rval = REFCODE_GENERAL_ERR;
    UINT8 Start[AMBA_H265_STRAT_CODE_SIZE] = {0, 0, 0, 1};
    Rval = RefCmptDemuxer_ReadMem(Start, AMBA_H265_STRAT_CODE_SIZE, &DemuxHdlr->AvcRawWritePoint, Track->BufferBase, Track->BufferLimit);
    if (Rval == REFCODE_OK) {
        Rval = RefCmptDemuxer_ReadMem(Track->VPS, Track->VPSLen, &DemuxHdlr->AvcRawWritePoint, Track->BufferBase, Track->BufferLimit);
        if (Rval == REFCODE_OK) {
            Rval = RefCmptDemuxer_ReadMem(Start, AMBA_H265_STRAT_CODE_SIZE, &DemuxHdlr->AvcRawWritePoint, Track->BufferBase, Track->BufferLimit);
            if (Rval == REFCODE_OK) {
                Rval = RefCmptDemuxer_ReadMem(Track->SPS, Track->SPSLen, &DemuxHdlr->AvcRawWritePoint, Track->BufferBase, Track->BufferLimit);
                if (Rval == REFCODE_OK) {
                    Rval = RefCmptDemuxer_ReadMem(Start, AMBA_H265_STRAT_CODE_SIZE, &DemuxHdlr->AvcRawWritePoint, Track->BufferBase, Track->BufferLimit);
                    if (Rval == REFCODE_OK) {
                        Rval = RefCmptDemuxer_ReadMem(Track->PPS, Track->PPSLen, &DemuxHdlr->AvcRawWritePoint, Track->BufferBase, Track->BufferLimit);
                    }
                }
            }
        }
    }
    return Rval;
}

static UINT32 RefCmptDemuxer_PutEOF(REF_CMPT_DEMUX_HDLR_s* DemuxHdlr, const VIDEO_TRACK_INFO_s *Track)
{
#define DECODER_EOS_SIZE_H264 (5U)
#define DECODER_EOS_SIZE_H265 (6U)
    static const UINT8 DECODER_EOS_H264[DECODER_EOS_SIZE_H264] = { 0x00, 0x00, 0x00, 0x01, 0x0B };
    static const UINT8 DECODER_EOS_H265[DECODER_EOS_SIZE_H265] = { 0x00, 0x00, 0x00, 0x01, 0x4A, 0x00 };
    UINT32 Rval = REFCODE_GENERAL_ERR;
    if (Track->MediaId == SVC_FORMAT_MID_AVC) {
        Rval = RefCmptDemuxer_ReadMem(DECODER_EOS_H264, DECODER_EOS_SIZE_H264, &DemuxHdlr->AvcRawWritePoint, Track->BufferBase, Track->BufferLimit);
    } else {
        Rval = RefCmptDemuxer_ReadMem(DECODER_EOS_H265, DECODER_EOS_SIZE_H265, &DemuxHdlr->AvcRawWritePoint, Track->BufferBase, Track->BufferLimit);
    }
    DemuxHdlr->VideoDecFrmCnt = REF_CMPT_FRAME_COUNT_EOS;
    return Rval;
}

static UINT8 *RefCmptDemuxer_PlusAddr(UINT32 Size, UINT8 *Addr, UINT8 *Base, const UINT8 *Limit)
{
    UINT8 *Rval = Addr;
    UINT32 AddrU32, BaseU32, LimitU32, RvalU32;
    AmbaMisra_TypeCast(&AddrU32, &Addr);
    AmbaMisra_TypeCast(&BaseU32, &Base);
    AmbaMisra_TypeCast(&LimitU32, &Limit);
    if ((AddrU32 < BaseU32) || (AddrU32 >= LimitU32)) {
        AmbaPrint_PrintStr5("%s: [ERROR] Wrong AddrU32!", __func__, NULL, NULL, NULL, NULL);
        Rval = NULL;
    } else {
        if ((AddrU32 + Size) <= LimitU32) {
            RvalU32 = AddrU32 + Size;
        } else {
            const UINT32 Rear = LimitU32 - AddrU32;
            RvalU32 = BaseU32 + Size - Rear;
        }
        AmbaMisra_TypeCast(&Rval, &RvalU32);
        if (Rval == Limit) {
            Rval = Base;
        }
    }
    return Rval;
}

static UINT32 RefCmptDemuxer_FeedIdrFrame(REF_CMPT_DEMUX_HDLR_s* DemuxHdlr, SVC_STREAM_HDLR_s *File, const VIDEO_TRACK_INFO_s *Track, UINT32 FrameNo)
{
    UINT32 Rval = REFCODE_OK;
    UINT32 FreeSpace = 0U;
    if (FrameNo >= Track->FrameCount) {
        AmbaPrint_PrintStr5("%s: [ERROR] FrameNo overflow", __func__, NULL, NULL, NULL, NULL);
        Rval = REF_CMPT_DEMUX_END;
    }
    if (Rval == REFCODE_OK) {
        Rval = RefCmptDemuxer_GetFreeSpace(DemuxHdlr, &FreeSpace);
        if (Rval != REFCODE_OK) {
            AmbaPrint_PrintStr5("%s: [ERROR] RefCmptDemuxer_GetFreeSpace fail", __func__, NULL, NULL, NULL, NULL);
        }
    }
    if (Rval == REFCODE_OK) {
        if (FreeSpace >= Track->FrameSize[FrameNo]) {
            Rval = ST2A(File->Func->Seek(File, (INT64)Track->FramePos[FrameNo], SVC_STREAM_SEEK_START));
            if (Rval == REFCODE_OK) {
                UINT64 Pts = (UINT64)FrameNo * (UINT64)DemuxHdlr->AvcUserData.nTimePerFrame; // assume CBR
                UINT32 DataSize;
                UINT32 OldWpU32;
                AmbaMisra_TypeCast(&OldWpU32, &DemuxHdlr->AvcRawWritePoint);
                if (Track->MediaId == SVC_FORMAT_MID_AVC) {
                    DataSize = AMBA_H264_GOP_NALU_SIZE + Track->FrameSize[FrameNo] + (AMBA_H264_STRAT_CODE_SIZE*2U) + Track->SPSLen + Track->PPSLen;
                    /* Put GOP Header */
                    Rval = RefCmptDemuxer_PutH264GOPHeader(DemuxHdlr, Track, 0U, 0U, Pts);
                    if (Rval != REFCODE_OK) {
                        AmbaPrint_PrintStr5("%s: [ERROR] RefCmptDemuxer_PutH264GOPHeader fail", __func__, NULL, NULL, NULL, NULL);
                    }
                    if (Rval == REFCODE_OK) {
                        /* Put sps, pps */
                        Rval = RefCmptDemuxer_PutSPSPPS(DemuxHdlr, Track);
                        if (Rval != REFCODE_OK) {
                            AmbaPrint_PrintStr5("%s: [ERROR] RefCmptDemuxer_PutSPSPPS fail", __func__, NULL, NULL, NULL, NULL);
                        }
                    }
                    if (Rval == REFCODE_OK) {
                        Rval = RefCmptDemuxer_ReadH264(File, DemuxHdlr->AvcRawWritePoint, Track->BufferBase, Track->BufferLimit, Track->FrameSize[FrameNo], &DataSize);
                        if (Rval != REFCODE_OK) {
                            AmbaPrint_PrintStr5("%s: [ERROR] RefCmptDemuxer_ReadH264 fail", __func__, NULL, NULL, NULL, NULL);
                        }
                    }
                } else {
                    DataSize = AMBA_H265_GOP_NALU_SIZE + Track->FrameSize[FrameNo] + (AMBA_H265_STRAT_CODE_SIZE*3U) + Track->VPSLen + Track->SPSLen + Track->PPSLen;
                    /* Put GOP Header */
                    Rval = RefCmptDemuxer_PutH265GOPHeader(DemuxHdlr, Track, 0U, 0U, Pts);
                    if (Rval != REFCODE_OK) {
                        AmbaPrint_PrintStr5("%s: [ERROR] RefCmptDemuxer_PutH265GOPHeader fail", __func__, NULL, NULL, NULL, NULL);
                    }
                    if (Rval == REFCODE_OK) {
                        /* Put vps, sps, pps */
                        Rval = RefCmptDemuxer_PutVPSSPSPPS(DemuxHdlr, Track);
                        if (Rval != REFCODE_OK) {
                            AmbaPrint_PrintStr5("%s: [ERROR] RefCmptDemuxer_PutVPSSPSPPS fail", __func__, NULL, NULL, NULL, NULL);
                        }
                    }
                    if (Rval == REFCODE_OK) {
                        Rval = RefCmptDemuxer_ReadH265(File, DemuxHdlr->AvcRawWritePoint, Track->BufferBase, Track->BufferLimit, Track->FrameSize[FrameNo], &DataSize);
                        if (Rval != REFCODE_OK) {
                            AmbaPrint_PrintStr5("%s: [ERROR] RefCmptDemuxer_ReadH265 fail", __func__, NULL, NULL, NULL, NULL);
                        }
                    }
                }
                if (Rval == REFCODE_OK) {
                    UINT8 *NewAddr = RefCmptDemuxer_PlusAddr(DataSize, DemuxHdlr->AvcRawWritePoint, Track->BufferBase, Track->BufferLimit);
                    if (NewAddr == NULL) {
                        AmbaPrint_PrintStr5("%s: [ERROR] RefCmptDemuxer_PlusAddr fail", __func__, NULL, NULL, NULL, NULL);
                        Rval = REFCODE_GENERAL_ERR;
                    } else {
                        DemuxHdlr->AvcRawWritePoint = NewAddr;
                        if (FrameNo == (Track->FrameCount - 1U)) {
                            Rval = RefCmptDemuxer_PutEOF(DemuxHdlr, Track);
                            if (Rval != REFCODE_OK) {
                                AmbaPrint_PrintStr5("%s: [ERROR] RefCmptDemuxer_PutEOF fail", __func__, NULL, NULL, NULL, NULL);
                            }
                        }

                        if (Rval == REFCODE_OK) {
                            // Send bits fifo update command to DSP
                            AMBA_DSP_VIDDEC_BITS_FIFO_s BitsFIFO;
                            UINT16 StreamIdx[1] = {0};
                            ULONG NewWpU32;
                            ULONG BufferBaseU32;
                            ULONG BufferLimitU32;
                            AmbaMisra_TypeCast(&NewWpU32, &DemuxHdlr->AvcRawWritePoint);
                            AmbaMisra_TypeCast(&BufferLimitU32, &Track->BufferLimit);
                            AmbaMisra_TypeCast(&BufferBaseU32, &Track->BufferBase);
                            if (NewWpU32 >= OldWpU32) {
                                ULONG StartAddr = OldWpU32 & 0xFFFFFFC0U;
                                ULONG EndAddr = (NewWpU32 + 63U) & 0xFFFFFFC0U;
                                UINT32 FrmSize = EndAddr - StartAddr;
                                (void)AmbaSvcWrap_CacheClean(StartAddr, FrmSize);
                            } else {
                                ULONG  StartAddr = OldWpU32 & 0xFFFFFFC0U;
                                ULONG  EndAddr = (NewWpU32 + 63U) & 0xFFFFFFC0U;
                                UINT32 RearSize = BufferLimitU32 - StartAddr;
                                UINT32 FrontSize = EndAddr - BufferBaseU32;
                                (void)AmbaSvcWrap_CacheClean(StartAddr, RearSize);
                                (void)AmbaSvcWrap_CacheClean(BufferBaseU32, FrontSize);
                            }
                            BitsFIFO.StartAddr = OldWpU32;
                            if (DemuxHdlr->AvcRawWritePoint != DemuxHdlr->AvcRawBuffer) {
                                BitsFIFO.EndAddr = NewWpU32 - 1U;
                            } else {
                                BitsFIFO.EndAddr = BufferLimitU32 - 1U;
                            }
                            // Update bits fifo to dsp only in RUN or PUASE state
                            if (RefCmptPlayer_IsVideoRunning() == 1U) {
                                Rval = DSP2A(AmbaDSP_VideoDecBitsFifoUpdate(1, StreamIdx, &BitsFIFO));
                                if (Rval != REFCODE_OK) {
                                    AmbaPrint_PrintStr5("%s: [ERROR] AmbaDSP_VideoDecBitsFifoUpdate fail", __func__, NULL, NULL, NULL, NULL);
                                }
                            }
                        }
                    }
                }
            } else {
                AmbaPrint_PrintStr5("%s: [ERROR] Fail to seek file", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            Rval = REF_CMPT_DEMUX_ERR_NOT_ENOUGH_SPACE;
        }
    }
    return Rval;
}

static UINT32 RefCmptDemuxer_FeedOtherFrame(REF_CMPT_DEMUX_HDLR_s* DemuxHdlr, SVC_STREAM_HDLR_s *File, const VIDEO_TRACK_INFO_s *Track, UINT32 FrameNo)
{
    UINT32 Rval = REFCODE_OK;
    UINT32 FreeSpace = 0U;
    if (FrameNo >= Track->FrameCount) {
        AmbaPrint_PrintStr5("%s: [ERROR] FrameNo overflow", __func__, NULL, NULL, NULL, NULL);
        Rval = REF_CMPT_DEMUX_END;
    }
    if (Rval == REFCODE_OK) {
        Rval = RefCmptDemuxer_GetFreeSpace(DemuxHdlr, &FreeSpace);
        if (Rval != REFCODE_OK) {
            AmbaPrint_PrintStr5("%s: [ERROR] RefCmptDemuxer_GetFreeSpace fail", __func__, NULL, NULL, NULL, NULL);
        }
    }
    if (Rval == REFCODE_OK) {
        if (FreeSpace >= Track->FrameSize[FrameNo]) {
            Rval = ST2A(File->Func->Seek(File, (INT64)Track->FramePos[FrameNo], SVC_STREAM_SEEK_START));
            if (Rval == REFCODE_OK) {
                UINT32 DataSize;
                ULONG  OldWpU32;
                AmbaMisra_TypeCast(&OldWpU32, &DemuxHdlr->AvcRawWritePoint);
                if (Track->MediaId == SVC_FORMAT_MID_AVC) {
                    Rval = RefCmptDemuxer_ReadH264(File, DemuxHdlr->AvcRawWritePoint, Track->BufferBase, Track->BufferLimit, Track->FrameSize[FrameNo], &DataSize);
                    if (Rval != REFCODE_OK) {
                        AmbaPrint_PrintStr5("%s: [ERROR] RefCmptDemuxer_ReadH264 fail", __func__, NULL, NULL, NULL, NULL);
                    }
                } else {
                    Rval = RefCmptDemuxer_ReadH265(File, DemuxHdlr->AvcRawWritePoint, Track->BufferBase, Track->BufferLimit, Track->FrameSize[FrameNo], &DataSize);
                    if (Rval != REFCODE_OK) {
                        AmbaPrint_PrintStr5("%s: [ERROR] RefCmptDemuxer_ReadH265 fail", __func__, NULL, NULL, NULL, NULL);
                    }
                }
                if (Rval == REFCODE_OK) {
                    UINT8 *NewAddr = RefCmptDemuxer_PlusAddr(DataSize, DemuxHdlr->AvcRawWritePoint, Track->BufferBase, Track->BufferLimit);
                    if (NewAddr == NULL) {
                        Rval = REFCODE_GENERAL_ERR;
                    } else {
                        DemuxHdlr->AvcRawWritePoint = NewAddr;
                        if (FrameNo == (Track->FrameCount - 1U)) {
                            Rval = RefCmptDemuxer_PutEOF(DemuxHdlr, Track);
                            if (Rval != REFCODE_OK) {
                                AmbaPrint_PrintStr5("%s: [ERROR] RefCmptDemuxer_PutEOF fail", __func__, NULL, NULL, NULL, NULL);
                            }
                        }

                        if (Rval == REFCODE_OK) {
                            // Send bits fifo update command to DSP
                            AMBA_DSP_VIDDEC_BITS_FIFO_s BitsFIFO;
                            UINT16 StreamIdx[1] = {0};
                            ULONG NewWpU32;
                            ULONG BufferBaseU32;
                            ULONG BufferLimitU32;
                            AmbaMisra_TypeCast(&NewWpU32, &DemuxHdlr->AvcRawWritePoint);
                            AmbaMisra_TypeCast(&BufferLimitU32, &Track->BufferLimit);
                            AmbaMisra_TypeCast(&BufferBaseU32, &Track->BufferBase);
                            if (NewWpU32 >= OldWpU32) {
                                ULONG  StartAddr = OldWpU32 & 0xFFFFFFC0U;
                                ULONG  EndAddr = (NewWpU32 + 63U) & 0xFFFFFFC0U;
                                UINT32 FrmSize = EndAddr - StartAddr;
                                (void)AmbaSvcWrap_CacheClean(StartAddr, FrmSize);
                            } else {
                                ULONG  StartAddr = OldWpU32 & 0xFFFFFFC0U;
                                ULONG  EndAddr = (NewWpU32 + 63U) & 0xFFFFFFC0U;
                                UINT32 RearSize = BufferLimitU32 - StartAddr;
                                UINT32 FrontSize = EndAddr - BufferBaseU32;
                                (void)AmbaSvcWrap_CacheClean(StartAddr, RearSize);
                                (void)AmbaSvcWrap_CacheClean(BufferBaseU32, FrontSize);
                            }
                            BitsFIFO.StartAddr = OldWpU32;
                            if (DemuxHdlr->AvcRawWritePoint != DemuxHdlr->AvcRawBuffer) {
                                BitsFIFO.EndAddr = NewWpU32 - 1U;
                            } else {
                                BitsFIFO.EndAddr = BufferLimitU32 - 1U;
                            }
                            // Update bits fifo to dsp only in RUN or PUASE state
                            if (RefCmptPlayer_IsVideoRunning() == 1U) {
                                Rval = DSP2A(AmbaDSP_VideoDecBitsFifoUpdate(1, StreamIdx, &BitsFIFO));
                                if (Rval != REFCODE_OK) {
                                    AmbaPrint_PrintStr5("%s: [ERROR] AmbaDSP_VideoDecBitsFifoUpdate fail", __func__, NULL, NULL, NULL, NULL);
                                }
                            }
                        }
                    }
                }
            } else {
                AmbaPrint_PrintStr5("%s: [ERROR] Fail to seek file", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            Rval = REF_CMPT_DEMUX_ERR_NOT_ENOUGH_SPACE;
        }
    }
    return Rval;
}

/**
* lock demuxer
* @param [in]  DemuxHdlr demux handler
* @param [in]  Timeout
* @return ErrorCode
*/
UINT32 RefCmptDemuxer_Lock(REF_CMPT_DEMUX_HDLR_s* DemuxHdlr, UINT32 Timeout)
{
    UINT32 Rval = REFCODE_OK;

    if (DemuxHdlr == NULL) {
        AmbaPrint_PrintStr5("%s: [ERROR] DemuxHdlr is NULL", __func__, NULL, NULL, NULL, NULL);
        Rval = REFCODE_GENERAL_ERR; // Error
    } else {
        // Take mutex. Enter critical section.
        Rval = K2A(AmbaKAL_MutexTake(&DemuxHdlr->VideoDecFrmMutex, Timeout));
        if (Rval != REFCODE_OK) {
            AmbaPrint_PrintStr5("%s: [ERROR] AmbaKAL_MutexTake fail", __func__, NULL, NULL, NULL, NULL);
        }
    }
    return Rval;
}

/**
* unlock demuxer
* @param [in]  DemuxHdlr demux handler
* @return ErrorCode
*/
UINT32 RefCmptDemuxer_UnLock(REF_CMPT_DEMUX_HDLR_s* DemuxHdlr)
{
    UINT32 Rval = REFCODE_OK;

    if (DemuxHdlr == NULL) {
        AmbaPrint_PrintStr5("%s: [ERROR] DemuxHdlr is NULL", __func__, NULL, NULL, NULL, NULL);
        Rval = REFCODE_GENERAL_ERR; // Error
    } else {
        // Give mutex. Exit critical section.
        Rval = K2A(AmbaKAL_MutexGive(&DemuxHdlr->VideoDecFrmMutex));
        if (Rval != REFCODE_OK) {
            AmbaPrint_PrintStr5("%s: [ERROR] AmbaKAL_MutexGive fail", __func__, NULL, NULL, NULL, NULL);
        }
    }
    return Rval;
}

/**
* GetFreeSpace
* @param [in]  DemuxHdlr demux handler
* @param [out]  Space
* @return ErrorCode
*/
UINT32 RefCmptDemuxer_GetFreeSpace(const REF_CMPT_DEMUX_HDLR_s* DemuxHdlr, UINT32* Space)
{
#define REF_CMPT_RESERVE_SPACE (1U*1024U*1024U)
    ULONG  BaseU32;
    ULONG  LimitU32;
    ULONG  RP;
    ULONG  WP;
    UINT32 Rval = REFCODE_OK;

    AmbaMisra_TypeCast(&BaseU32, &VideoTrackInfo.BufferBase);
    AmbaMisra_TypeCast(&LimitU32, &VideoTrackInfo.BufferLimit);
    AmbaMisra_TypeCast(&RP, &DemuxHdlr->AvcRawReadPoint);
    AmbaMisra_TypeCast(&WP, &DemuxHdlr->AvcRawWritePoint);

    if (DemuxHdlr == NULL) {
        AmbaPrint_PrintStr5("%s: [ERROR] DemuxHdlr is NULL", __func__, NULL, NULL, NULL, NULL);
        Rval = REFCODE_GENERAL_ERR; // Error
    } else {
        // Assume overflow never happen. should be blocked before calling the function
        if (RP > WP) {
            *Space = RP - WP - 1U; // -1 to have wp != rp after write
        } else {
            *Space = (LimitU32 - WP) + (RP - BaseU32) - 1U; //(limit - wp) + (rp - base) - 1;// -1 to make wp != rp after write
        }
        /* Reserve space to avoid overwrite */
        if (*Space >= REF_CMPT_RESERVE_SPACE) {
            *Space -= REF_CMPT_RESERVE_SPACE;
            // AmbaPrint_PrintUInt5("!!! RP %u, WP %u, Space %u", RP, WP, *Space, 0, 0);
        } else {
            *Space = 0U;
        }
    }

    return Rval;
}

/**
 * Feed one frame into raw buffer if there're enough space.
 *
 * @param [in] DemuxHdlr      Demux handler
 *
 * @return 0 - REFCODE_OK, others - Error
 */
static UINT32 RefCmptDemuxer_FeedFrame(REF_CMPT_DEMUX_HDLR_s* DemuxHdlr)
{
    UINT32 Rval;

    // Take mutex. Enter critical section.
    Rval = RefCmptDemuxer_Lock(DemuxHdlr, AMBA_KAL_WAIT_FOREVER);

    if (Rval == REFCODE_OK) {
        if ((DemuxHdlr->IsOpened == 0U) || (DemuxHdlr->Mp4File == NULL)) {
            AmbaPrint_PrintStr5("%s: [ERROR] File not opened", __func__, NULL, NULL, NULL, NULL);
            Rval = REFCODE_GENERAL_ERR;
        }
    }
    if (Rval == REFCODE_OK) {
        if ((DemuxHdlr->VideoDecFrmCnt == 0U) || (DemuxHdlr->VideoDecFrmCnt == REF_CMPT_FRAME_COUNT_EOS) || (DemuxHdlr->FrameSeqNum >= VideoTrackInfo.FrameCount)) {
            Rval = REF_CMPT_DEMUX_END;
        }
    }

    if (Rval == REFCODE_OK) {
        if ((DemuxHdlr->FrameSeqNum % VideoTrackInfo.GOPSize) == 0U) {
            Rval = RefCmptDemuxer_FeedIdrFrame(DemuxHdlr, DemuxHdlr->Mp4File, &VideoTrackInfo, DemuxHdlr->FrameSeqNum);
            if (Rval == REFCODE_OK) {
                // AmbaPrint_PrintUInt5("!!! RefCmptDemuxer_FeedIdrFrame %u", Rval, 0, 0, 0, 0);
            }
        } else {
            Rval = RefCmptDemuxer_FeedOtherFrame(DemuxHdlr, DemuxHdlr->Mp4File, &VideoTrackInfo, DemuxHdlr->FrameSeqNum);
            if (Rval == REFCODE_OK) {
                // AmbaPrint_PrintUInt5("!!! RefCmptDemuxer_FeedOtherFrame %u", Rval, 0, 0, 0, 0);
            }
        }
    }

    if (Rval == REFCODE_OK) {
        DemuxHdlr->FrameSeqNum++;
        if (DemuxHdlr->FrameSeqNum == VideoTrackInfo.FrameCount) {
            DemuxHdlr->VideoDecFrmCnt = REF_CMPT_FRAME_COUNT_EOS;
        }
        if(DemuxHdlr->VideoDecFrmCnt != REF_CMPT_FRAME_COUNT_EOS) {
            DemuxHdlr->VideoDecFrmCnt--;
        }
        Rval = RefCmptDemuxer_UnLock(DemuxHdlr);
        if (Rval != REFCODE_OK) {
            AmbaPrint_PrintStr5("%s: [ERROR] RefCmptDemuxer_UnLock fail", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        (void) RefCmptDemuxer_UnLock(DemuxHdlr);
    }
    return Rval; // Success
}

/**
 * Simple and Demux Task.
 * An endless loop feeding frames to raw buffer. Does not use fancy mutex/flag/msg to do control
 *
 * @param [in] Info info         EntryArg
 *
 * @return None
 */
static void* RefCmptDemuxer_Task(void* EntryArg)
{
    const ULONG *pArg;
    UINT32 Rval;

    AmbaMisra_TouchUnused(EntryArg);
    AmbaMisra_TypeCast(&pArg, &EntryArg);

    AmbaPrint_PrintUInt5("RefCmptDemuxer_Task Start! %d", *pArg, 0U, 0U, 0U, 0U);

    // Raw file used format nhnt
    // check http://gpac.wp.mines-telecom.fr/mp4box/media-import/nhnt-format/ for more info
    // we could get it by using MP4box ( -nhnl )
    for (;;) {
        UINT8 DmxID;

        // Deal with each demuxer
        for (DmxID = 0U; DmxID < REF_CMPT_DEMUX_MAX_NUM; ++DmxID) {
            REF_CMPT_DEMUX_HDLR_s *DemuxHdlr;

            // Get demux handler
            DemuxHdlr = &RefCmptDemuxHdlr[DmxID];

            // Feed one frame to raw buffer
            if ((DemuxHdlr->IsInit == 1U) && (DemuxHdlr->IsOpened == 1U)) {
                Rval = RefCmptDemuxer_FeedFrame(DemuxHdlr);

                if (Rval == REF_CMPT_DEMUX_ERR_NOT_ENOUGH_SPACE) {
                    DemuxHdlr->IsWaitingSpace = 1U;
                } else {
                    DemuxHdlr->IsWaitingSpace = 0U;
                }

                if ((Rval != REFCODE_OK) && (Rval != REF_CMPT_DEMUX_END) && (Rval != REF_CMPT_DEMUX_ERR_NOT_ENOUGH_SPACE)) {
                    AmbaPrint_PrintUInt5("RefCmptDemuxer_Task: [ERROR] RefCmptDemuxer_Task fail %p", Rval, 0U, 0U, 0U, 0U);
                    break;
                }
            }
        }
        (void) AmbaKAL_TaskSleep(1);
    }

    //AmbaPrint_PrintUInt5("RefCmptDemuxer_Task End! %d", *pArg, 0U, 0U, 0U, 0U);

    return NULL;
}

//TODO should use flag
static void RefCmptDemuxer_WaitFeedDone(const REF_CMPT_DEMUX_HDLR_s* DemuxHdlr)
{
    UINT32 PreviourNum = 0U;
    UINT32 BlockedCnt = 0U;


    while (DemuxHdlr->VideoDecFrmCnt != 0U) {
        if (PreviourNum == DemuxHdlr->VideoDecFrmCnt) {
            BlockedCnt++;
        }
        if (BlockedCnt > 1000U) {
            AmbaPrint_PrintUInt5("RefCmptDemuxer_WaitFeedDone: Feed too slow?", 0U, 0U, 0U, 0U, 0U);
        }
        PreviourNum = DemuxHdlr->VideoDecFrmCnt;
        if ((DemuxHdlr->VideoDecFrmCnt == REF_CMPT_FRAME_COUNT_EOS) || (BlockedCnt > 1000U) || (DemuxHdlr->IsWaitingSpace != 0U)) { // EOS
            break;
        }
        (void) AmbaKAL_TaskSleep(10);
        if ((BlockedCnt % 128U) == 0U) {
            AmbaPrint_PrintUInt5("RefCmptDemuxer_WaitFeedDone: %d", DemuxHdlr->VideoDecFrmCnt, 0U, 0U, 0U, 0U);
        }
    }

    if (BlockedCnt > 1000U){
        AmbaPrint_PrintUInt5("RefCmptDemuxer_WaitFeedDone: Feed timeout. Can't continue", 0U, 0U, 0U, 0U, 0U);
    }

    return;
}

/**
* init demuxer
* @return ErrorCode
*/
UINT32 RefCmptDemuxer_Init(void)
{
    UINT32 Rval = REFCODE_OK; // 0: Success

    // Initialize all demux handlers
    (void) AmbaWrap_memset(RefCmptDemuxHdlr, 0, sizeof(RefCmptDemuxHdlr));

    return Rval;
}

/**
* create demuxer
* @param [in]  DemuxCfg demux config
* @param [out]  DemuxHdlr demux handler
* @param [in]  pMp4File pMp4File
* @return ErrorCode
*/
UINT32 RefCmptDemuxer_Create(const REF_CMPT_DEMUX_CREATE_CFG_s* DemuxCfg, REF_CMPT_DEMUX_HDLR_s** DemuxHdlr, SVC_STREAM_HDLR_s **pMp4File)
{
    UINT32 T;
    REF_CMPT_DEMUX_HDLR_s* RetDemuxHdlr = NULL;
    UINT32 Rval = REFCODE_OK;
    static char DmxMtxName[] = "DMtx";

    if (DemuxHdlr == NULL) {
        Rval = REFCODE_GENERAL_ERR;
    } else {
        // Search for free handler
        for (T = 0U; T < REF_CMPT_DEMUX_MAX_NUM; ++T) {
            if (RefCmptDemuxHdlr[T].IsInit == 0U) {
                RetDemuxHdlr = &RefCmptDemuxHdlr[T];
                break;
            }
        }
        if (RetDemuxHdlr == NULL) {
            AmbaPrint_PrintStr5("%s: [ERROR] No more handlers", __func__, NULL, NULL, NULL, NULL);
            Rval = REFCODE_GENERAL_ERR;
        } else {
            // Setup config
            RetDemuxHdlr->AvcRawBuffer = DemuxCfg->AvcRawBuffer;
            RetDemuxHdlr->AvcRawLimit = DemuxCfg->AvcRawLimit;
            RetDemuxHdlr->AvcRawWritePoint = DemuxCfg->AvcRawBuffer;
            RetDemuxHdlr->AvcRawReadPoint = DemuxCfg->AvcRawBuffer;
            VideoTrackInfo.BufferBase = DemuxCfg->AvcRawBuffer;
            VideoTrackInfo.BufferLimit = &DemuxCfg->AvcRawLimit[1]; // BufferLimit = BufferBase + BufferSize = AvcRawLimit + 1


            // Create mutex for reading and writing frames to fifo
            Rval = K2A(AmbaKAL_MutexCreate(&RetDemuxHdlr->VideoDecFrmMutex, DmxMtxName));
            if (Rval != REFCODE_OK) {
                AmbaPrint_PrintStr5("%s: [ERROR] AmbaKAL_MutexCreate fail", __func__, NULL, NULL, NULL, NULL);
            }

            /* Create File Stream */
            if (Rval == REFCODE_OK) {
                // SVC_FILE_STREAM_CFG_s FileCfg;
                // Rval = ST2A(SvcFileStream_GetDefaultCfg(&FileCfg));
                // if (Rval != REFCODE_OK) {
                //     AmbaPrint_PrintStr5("%s: [ERROR] SvcFileStream_GetDefaultCfg failed", __func__, NULL, NULL, NULL, NULL);
                // }
                // if (Rval == REFCODE_OK) {
                //     Rval = ST2A(SvcFileStream_Create(&FileCfg, &RetDemuxHdlr->Mp4File));
                //     if ((Rval != REFCODE_OK) || (RetDemuxHdlr->Mp4File == NULL)) {
                //         AmbaPrint_PrintStr5("%s: [ERROR] SvcFileStream_Create failed", __func__, NULL, NULL, NULL, NULL);
                //         Rval = REFCODE_GENERAL_ERR;
                //     }
                // }
                AmbaMisra_TouchUnused(pMp4File);
                RetDemuxHdlr->Mp4File = *pMp4File;
            }

            if (Rval == REFCODE_OK) {
                RetDemuxHdlr->IsInit = 1U;
                // Return result
                *DemuxHdlr = RetDemuxHdlr;
            }
        }
    }
    return Rval;
}

/**
* open demuxer
* @param [in]  DemuxHdlr demux handler
* @param [in]  pRawFn
* @return ErrorCode
*/
UINT32 RefCmptDemuxer_Open(REF_CMPT_DEMUX_HDLR_s* DemuxHdlr,
                                  const char* pRawFn)
{
    const char *FsFileName;
    UINT32 Rval = REFCODE_OK;

    if (DemuxHdlr->IsInit == 0U) {
        Rval = REFCODE_GENERAL_ERR;
    }

    if (Rval == REFCODE_OK) {
        Rval = RefCmptDemuxer_Lock(DemuxHdlr, AMBA_KAL_WAIT_FOREVER);
    }
    if (Rval == REFCODE_OK) {
        // Open raw file (.h264)
        AmbaPrint_PrintStr5("%s: Open file %s", __func__, pRawFn, NULL, NULL, NULL);
        // Set input
        FsFileName = pRawFn;

        DemuxHdlr->NeedGOPHeader = 1U;

        // Open file
        Rval = ST2A(DemuxHdlr->Mp4File->Func->Open(DemuxHdlr->Mp4File, FsFileName, SVC_STREAM_MODE_RDONLY));
        if (Rval != REFCODE_OK) {
            AmbaPrint_PrintStr5("%s: [ERROR] Failed to open raw file %s", __func__, FsFileName, NULL, NULL, NULL);
        }
    }

    VideoTrackInfo.GOPSize = DemuxHdlr->AvcUserData.nIdrInterval * DemuxHdlr->AvcUserData.nN;

    // TODO: user can check Track->Width and Track->Height
    if (Rval == REFCODE_OK) {
        Rval = RefCmptDemuxer_Parse(DemuxHdlr->Mp4File, &VideoTrackInfo);
        if (Rval != REFCODE_OK) {
            AmbaPrint_PrintStr5("%s: [ERROR] RefCmptDemuxer_Parse fail", __func__, NULL, NULL, NULL, NULL);
        }
    }

    if (Rval == REFCODE_OK) {
        DemuxHdlr->AvcUserData.nVideoWidth = VideoTrackInfo.VideoWidth;
        DemuxHdlr->AvcUserData.nVideoHeight = VideoTrackInfo.VideoHeight;
        DemuxHdlr->VideoDecFrmCnt = 0;
        DemuxHdlr->FrameSeqNum = 0;
        DemuxHdlr->PutHeader = 1;
        DemuxHdlr->IsFirstGop = 1;
        if (VideoTrackInfo.MediaId == SVC_FORMAT_MID_AVC) {
            DemuxHdlr->CodingType = REF_CMPT_DEMUX_PB_H264;
        } else {
            DemuxHdlr->CodingType = REF_CMPT_DEMUX_PB_H265;
        }
        DemuxHdlr->PreviousPts = REF_CMPT_FRAME_COUNT_EOS;
        DemuxHdlr->IsOpened = 1U;
        Rval = RefCmptDemuxer_UnLock(DemuxHdlr);
        AmbaPrint_PrintUInt5("RefCmptDemuxer_Open: IdrInterval %u TimeScale = %u TimePerFrame = %u M = %u N = %u",
                DemuxHdlr->AvcUserData.nIdrInterval, DemuxHdlr->AvcUserData.nTimeScale, DemuxHdlr->AvcUserData.nTimePerFrame, DemuxHdlr->AvcUserData.nM, DemuxHdlr->AvcUserData.nN);
    }

    return Rval;
}

/**
* start demuxer
* @param [in]  DemuxHdlr demux handler
* @param [in]  startTime
* @param [in]  direction
* @param [in]  speed
* @return ErrorCode
*/
UINT32 RefCmptDemuxer_Start(REF_CMPT_DEMUX_HDLR_s* DemuxHdlr,
                               UINT32* startTime,
                               UINT8 direction,
                               UINT32 speed)
{
static char RefCmptDemuxerStack[REF_CMPT_DEMUX_STACK_SIZE] __attribute__((section(".bss.noinit"))); // Stack for demux task
    static char DmxTaskName[] = "RefCmptDemuxTask";
    UINT32 TargetPTS; // Convert to DSP 90000 base
    UINT32 PrefeedNum = 32U;
    UINT32 Rval = REFCODE_OK;

    //  TODO: 1. Whether erase previous frames or not?
    //  TODO: 2. Change definition of speed, enable 2x, 4x, ...

    if (DemuxHdlr->IsOpened == 0U) {
        AmbaPrint_PrintStr5("%s: [ERROR] Open demux first", __func__, NULL, NULL, NULL, NULL);
        Rval = REFCODE_GENERAL_ERR;
    } else if (direction != 0U) {
        AmbaPrint_PrintStr5("%s: [ERROR] Backward not supported", __func__, NULL, NULL, NULL, NULL);
        Rval = REFCODE_GENERAL_ERR;
    } else if (speed > 256U) {
        AmbaPrint_PrintStr5("%s: [ERROR] Fast forward not supported", __func__, NULL, NULL, NULL, NULL);
        Rval = REFCODE_GENERAL_ERR;
    } else {
        TargetPTS = 0;

        *startTime = TargetPTS;

        DemuxHdlr->IsPreFeeding = 1U;
        DemuxHdlr->IsWaitingSpace = 0U; // Set IsWaitingSpace before feeding start
        DemuxHdlr->VideoDecFrmCnt = 0U;
        DemuxHdlr->FrameSeqNum = 0U;
        PrefeedNum = DemuxHdlr->AvcUserData.nN;
        Rval = RefCmptDemuxer_Feed(DemuxHdlr, PrefeedNum); // prefeed frames

        // Create demux task
        Rval = K2A(AmbaKAL_TaskCreate(&RefCmptDemuxTask, // pTask
                DmxTaskName,                        // pTaskName
                30U,                                 // Priority
                RefCmptDemuxer_Task,          // void (*EntryFunction)(UINT32)
                NULL,                                 // EntryArg
                RefCmptDemuxerStack,   // pStackBase
                REF_CMPT_DEMUX_STACK_SIZE, // StackByteSize
                1U));               // AutoStart
        if (Rval != REFCODE_OK) {
            AmbaPrint_PrintStr5("%s: [ERROR] AmbaKAL_TaskCreate fail", __func__, NULL, NULL, NULL, NULL);
            Rval = REFCODE_GENERAL_ERR;
        }
    }

    if (Rval == REFCODE_OK) {
        // Wait until feeding complete
        RefCmptDemuxer_WaitFeedDone(DemuxHdlr);
        DemuxHdlr->IsPreFeeding = 0;
        AmbaPrint_PrintUInt5("RefCmptDemuxer_Start: Prefeed %u/%u done", DemuxHdlr->FrameSeqNum, PrefeedNum, 0U, 0U, 0U);
    }

    return Rval;
}

/**
* stop demuxer
* @param [in]  DemuxHdlr demux handler
* @return ErrorCode
*/
UINT32 RefCmptDemuxer_Stop(REF_CMPT_DEMUX_HDLR_s* DemuxHdlr)
{
    UINT32 Rval = REFCODE_OK;
    if (DemuxHdlr->IsInit == 0U) {
        Rval = REFCODE_GENERAL_ERR;
    }
    if (DemuxHdlr->IsOpened == 0U) {
        Rval = REFCODE_GENERAL_ERR;
    }
    /*
     * Delete Task.
     * Take mutex before deleting task to make sure that task is not in the middle of process.
     */
    if (Rval == REFCODE_OK) {
        Rval = RefCmptDemuxer_Lock(DemuxHdlr, AMBA_KAL_WAIT_FOREVER);
        if (Rval == REFCODE_OK) {
            AMBA_KAL_TASK_INFO_s TaskInfo;
            Rval = K2A(AmbaKAL_TaskQuery(&RefCmptDemuxTask, &TaskInfo));
            if (Rval == REFCODE_OK) {
                if ((TaskInfo.TaskState != TX_COMPLETED) && (TaskInfo.TaskState != TX_TERMINATED)) {
                    Rval = K2A(AmbaKAL_TaskTerminate(&RefCmptDemuxTask));
                    if (Rval != REFCODE_OK) {
                        AmbaPrint_PrintStr5("%s: [ERROR] AmbaKAL_TaskTerminate fail", __func__, NULL, NULL, NULL, NULL);
                    }
                }
                if (Rval == REFCODE_OK) {
                    Rval = K2A(AmbaKAL_TaskDelete(&RefCmptDemuxTask));
                    if (Rval != REFCODE_OK) {
                        AmbaPrint_PrintStr5("%s: [ERROR] AmbaKAL_TaskDelete fail", __func__, NULL, NULL, NULL, NULL);
                    }
                }
            } else {
                AmbaPrint_PrintStr5("%s: [ERROR] AmbaKAL_TaskQuery fail", __func__, NULL, NULL, NULL, NULL);
            }

            if (RefCmptDemuxer_UnLock(DemuxHdlr) != REFCODE_OK) {
                AmbaPrint_PrintStr5("%s: [ERROR] RefCmptDemuxer_UnLock fail", __func__, NULL, NULL, NULL, NULL);
                Rval = REFCODE_GENERAL_ERR;
            }
        } else {
            AmbaPrint_PrintStr5("%s: [ERROR] RefCmptDemuxer_Lock fail", __func__, NULL, NULL, NULL, NULL);
        }
    }
    return Rval;
}

/**
* close demuxer
* @param [in]  DemuxHdlr demux handler
* @return ErrorCode
*/
UINT32 RefCmptDemuxer_Close(REF_CMPT_DEMUX_HDLR_s* DemuxHdlr)
{
    UINT32 Rval = REFCODE_OK;
    if (DemuxHdlr->IsInit == 0U) {
        Rval = REFCODE_GENERAL_ERR;
    } else if (DemuxHdlr->IsOpened == 0U) {
        Rval = REFCODE_GENERAL_ERR;
    } else {
        Rval = RefCmptDemuxer_Lock(DemuxHdlr, 5000U);
    }
    if (Rval == REFCODE_OK) {
        DemuxHdlr->PreviousPts = REF_CMPT_FRAME_COUNT_EOS;
        DemuxHdlr->PutHeader = 0U;
        DemuxHdlr->VideoDecFrmCnt = REF_CMPT_FRAME_COUNT_EOS;
        DemuxHdlr->FrameSeqNum = 0U;
        DemuxHdlr->AvcRawWritePoint = DemuxHdlr->AvcRawBuffer;
        DemuxHdlr->AvcRawReadPoint = DemuxHdlr->AvcRawBuffer;

        Rval = ST2A(DemuxHdlr->Mp4File->Func->Close(DemuxHdlr->Mp4File));
        if (Rval != REFCODE_OK) {
            AmbaPrint_PrintStr5("%s: [ERROR] Failed to close raw file", __func__, NULL, NULL, NULL, NULL);
        }

        if (Rval == REFCODE_OK) {
            DemuxHdlr->IsOpened = 0U;
            AmbaPrint_PrintStr5("%s: Demuxer data file closed success", __func__, NULL, NULL, NULL, NULL);
            Rval = RefCmptDemuxer_UnLock(DemuxHdlr);
        } else {
            (void) RefCmptDemuxer_UnLock(DemuxHdlr);
        }
    }
    return Rval;
}

/**
* delete demuxer
* @param [in]  DemuxHdlr demux handler
* @return ErrorCode
*/
UINT32 RefCmptDemuxer_Delete(REF_CMPT_DEMUX_HDLR_s* DemuxHdlr)
{
    UINT32 Rval = REFCODE_OK;

    if (DemuxHdlr->IsInit != 0U) {
        if (DemuxHdlr->IsOpened == 1U) {
            Rval = RefCmptDemuxer_Close(DemuxHdlr);
        }

        // Delete mutex
        if (Rval == REFCODE_OK) {
            Rval = K2A(AmbaKAL_MutexDelete(&DemuxHdlr->VideoDecFrmMutex));
            if (Rval != REFCODE_OK) {
                AmbaPrint_PrintStr5("%s: [ERROR] AmbaKAL_MutexDelete fail", __func__, NULL, NULL, NULL, NULL);
            }
        }
        if (Rval == REFCODE_OK) {
            (void) AmbaWrap_memset(DemuxHdlr, 0, sizeof(REF_CMPT_DEMUX_HDLR_s));
            DemuxHdlr->IsInit = 0U;
        }
    }

    return Rval;
}

/**
* exit demuxer
* @return ErrorCode
*/
UINT32 RefCmptDemuxer_Exit(void)
{
    UINT32 T = 0;
    UINT32 Rval = REFCODE_OK;

    // Delete all demux handlers
    for (T = 0; T < REF_CMPT_DEMUX_MAX_NUM; ++T) {
        Rval = RefCmptDemuxer_Delete(&RefCmptDemuxHdlr[T]);
        if (Rval != REFCODE_OK) {
            break;
        }
    }
    return Rval;
}

/**
* feed frame
* @param [in]  DemuxHdlr demux handler
* @param [in]  Req
* @return ErrorCode
*/
UINT32 RefCmptDemuxer_Feed(REF_CMPT_DEMUX_HDLR_s* DemuxHdlr,
                                      UINT32 Req)
{
    UINT32 Rval = REFCODE_OK;
    if (DemuxHdlr->IsInit == 0U) {
        Rval = REFCODE_GENERAL_ERR;
    } else if ((DemuxHdlr->IsOpened == 0U) || (DemuxHdlr->VideoDecFrmCnt == REF_CMPT_FRAME_COUNT_EOS)) {
        Rval = REFCODE_GENERAL_ERR;
    } else {
        DemuxHdlr->VideoDecFrmCnt += Req;
    }
    return Rval;
}

/**
* RefCmptDemuxer_SetGopConfig
* @param [in]  DemuxHdlr demux handler
* @param [in]  IdrInterval
* @param [in]  TimeScale
* @param [in]  TimePerFrame
* @param [in]  N
* @param [in]  M
* @return ErrorCode
*/
UINT32 RefCmptDemuxer_SetGopConfig(REF_CMPT_DEMUX_HDLR_s* DemuxHdlr, UINT32 IdrInterval, UINT32 TimeScale, UINT32 TimePerFrame, UINT32 N, UINT32 M)
{
    UINT32 Rval = REFCODE_OK;
    DemuxHdlr->AvcUserData.nIdrInterval = IdrInterval;
    DemuxHdlr->AvcUserData.nTimeScale = TimeScale;
    DemuxHdlr->AvcUserData.nTimePerFrame = TimePerFrame;
    DemuxHdlr->AvcUserData.nN = N;
    DemuxHdlr->AvcUserData.nM = M;
    DemuxHdlr->AvcUserData.nVideoWidth = 0U;
    DemuxHdlr->AvcUserData.nVideoHeight = 0U;
    DemuxHdlr->AvcUserData.nInterlaced = 0U;
    return Rval;
}
