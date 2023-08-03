/**
 *  @file RefCode_FileFeed.c
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
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 *  AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details Avtp talker H264 file feed
 *
 */
#include "AmbaKAL.h"
#include "AmbaCFS.h"
#include "DummyVideoEnc.h"
#include "AmbaDSP_VideoEnc.h"
#include "RefCode_FileFeed.h"

#define FILE_STREAM_RAW_SIZE  (20U * 1024U * 1024U)   /**< 10MB raw buffer => 6s for 1.5 H265*/
#define FILE_STREAM_FIFO_NUM  (2048U)                 /**< number of entry of FIFO (at least 20s * 60fps * 3tiles) */

FILE_FEED_CFG_s FileFeedConfig[MAX_FILE_FEED_NUM];
UINT8 InitFileFeedConfig = 0;

static DUMMY_VIDEO_ENC_s g_VideoEnc[MAX_FILE_FEED_NUM];     /**< video encoder */


static inline void RefCode_Perror(const char *Func, UINT32 Line, const char *Message)
{
    char LineStr[8];
    (void)AmbaUtility_UInt32ToStr(LineStr, sizeof(LineStr), Line, 10);\
    AmbaPrint_PrintStr5("[Error]%s:%s %s", Func, LineStr, Message, NULL, NULL);
}

static char *RefCode_GetVideoCodecName(UINT32 Type)
{
    static char H264[8] = "h264";
    static char H265[8] = "h265";
    char *Ret = NULL;
    if (Type == AMBA_DSP_ENC_CODING_TYPE_H264) {
        Ret = H264;
    } else {
        if (Type == AMBA_DSP_ENC_CODING_TYPE_H265) {
            Ret = H265;
        }
    }
    return Ret;
}

static RING_BUFFER_s *RefCode_GetRingBuf(const AMBA_FIFO_HDLR_s *Fifo)
{
    UINT32 i;
    RING_BUFFER_s *Buffer = NULL;
    for (i = 0; i < MAX_FILE_FEED_NUM; i++) {
        if ( (g_VideoEnc[i].Fifo != NULL) && (g_VideoEnc[i].Fifo == Fifo)) {
            Buffer = &g_VideoEnc[i].RingBuf;
            break;
        }
    }

    if (Buffer == NULL) {
        AmbaPrint_PrintStr5("%s failed!", __FUNCTION__, NULL, NULL, NULL, NULL);
    }
    return Buffer;
}

/**
 * Muxer UT - encoder event callback function.
 *
 * @param [in] pHdlr video encoder handler.
 * @param [in] nEvent event id.
 * @param [in] pInfo callback information.
 * @return 0 - OK, others - fail
 */
static UINT32 RefCode_DummyEncCB(const AMBA_FIFO_HDLR_s *Hdlr, UINT8 Event, AMBA_FIFO_CALLBACK_INFO_s *Info)
{
    UINT32 Rval = OK;
    AmbaMisra_TouchUnused(&Info);
    AmbaMisra_TouchUnused(&Info->Desc);
    if (Event == AMBA_FIFO_EVENT_DATA_CONSUMED) {
        RING_BUFFER_s *Buffer = RefCode_GetRingBuf(Hdlr);
        if (Buffer != NULL) {
            UINT32 i;
            for (i = 0; i < Info->FrameNum; i++) {
                Rval = RingBuf_DeleteFrame(Buffer);
                if (Rval != OK) {
                    if (Rval == UT_ERR_DATA_EMPTY) {
                        Rval = OK;
                    } else {
                        AmbaPrint_PrintStr5("%s : RingBuf_DeleteFrame Fail", __FUNCTION__, NULL, NULL, NULL, NULL);
                        break;
                    }
                }
            }
        } else {
            Rval = UT_ERR_GENERAL_ERROR;
        }
    }
    return Rval;
}

static UINT32 RefCode_CreateEncoder(UINT32 Id, FILE_FEED_CFG_s *pFileFeedCfg)
{
#define DUMMY_VIDEO_BUFFER_SIZE (FILE_STREAM_RAW_SIZE + (sizeof(RING_BUFFER_DESC_s) * FILE_STREAM_FIFO_NUM))
    static UINT8 VideoBuffer[MAX_FILE_FEED_NUM][DUMMY_VIDEO_BUFFER_SIZE] GNU_ALIGNED_CACHESAFE;
#define VIDEO_FIFO_BUFFER_SIZE  (sizeof(AMBA_FIFO_BITS_DESC_s) * FILE_STREAM_FIFO_NUM)
    static UINT8 VfifoBuffer[MAX_FILE_FEED_NUM][VIDEO_FIFO_BUFFER_SIZE] GNU_ALIGNED_CACHESAFE;
    UINT32 Rval;
    UINT32 BufferSize;
    char HdrFile[AMBA_CFS_MAX_FILENAME_LENGTH];
    char BsFile[AMBA_CFS_MAX_FILENAME_LENGTH];
    AMBA_FIFO_CFG_s FifoCfg;
    AMBA_FIFO_HDLR_s *Fifo = NULL;

    // create codec fifo
    Rval = F2U(AmbaFIFO_GetDefaultCfg(&FifoCfg));
    if (Rval == OK) {
        FifoCfg.BaseHdlr = NULL;
        FifoCfg.NumEntries = FILE_STREAM_FIFO_NUM;
        FifoCfg.CbEvent = RefCode_DummyEncCB;
        Rval = F2U(AmbaFIFO_GetHdlrBufferSize(FifoCfg.NumEntries, &BufferSize));
        if (Rval == OK) {
            AmbaPrint_PrintUInt5("VideoFifo: %u", BufferSize, 0, 0, 0, 0);
            if (BufferSize <= VIDEO_FIFO_BUFFER_SIZE) {
                FifoCfg.BufferAddr = VfifoBuffer[Id];
                FifoCfg.BufferSize = BufferSize;
                Rval = F2U(AmbaFIFO_Create(&FifoCfg, &Fifo));
            } else {
                RefCode_Perror(__FUNCTION__, __LINE__, NULL);
                Rval = UT_ERR_GENERAL_ERROR;
            }
        } else {
            RefCode_Perror(__FUNCTION__, __LINE__, NULL);
        }
    } else {
        RefCode_Perror(__FUNCTION__, __LINE__, NULL);
    }

    // Init DummyVideoEnc
    if (Rval == OK) {
        g_VideoEnc[Id].Fifo = Fifo;
        BufferSize = DummyVideoEnc_GetRequiredBufSize(FILE_STREAM_RAW_SIZE, FILE_STREAM_FIFO_NUM);
        AmbaPrint_PrintUInt5("DummyVideo: %u", BufferSize, 0, 0, 0, 0);
        pFileFeedCfg->pBuffer = VideoBuffer[Id];
        pFileFeedCfg->BufferSize = FILE_STREAM_RAW_SIZE;
        if (BufferSize <= DUMMY_VIDEO_BUFFER_SIZE) {
            const char *Args[2];
            Args[0] = pFileFeedCfg->Prefix;
            (void)AmbaUtility_StringPrintStr(HdrFile, AMBA_CFS_MAX_FILENAME_LENGTH, "%s.nhnt", 1, Args);
            Args[1] = RefCode_GetVideoCodecName(pFileFeedCfg->Type);
            (void)AmbaUtility_StringPrintStr(BsFile, AMBA_CFS_MAX_FILENAME_LENGTH, "%s.%s", 2, Args);
            Rval = DummyVideoEnc_Init(&g_VideoEnc[Id], BsFile, HdrFile, pFileFeedCfg->pBuffer, BufferSize, FILE_STREAM_FIFO_NUM, Fifo, (UINT8)pFileFeedCfg->Type);
        } else {
            RefCode_Perror(__FUNCTION__, __LINE__, NULL);
            Rval = UT_ERR_GENERAL_ERROR;
        }
    } else {
        RefCode_Perror(__FUNCTION__, __LINE__, NULL);
    }

    return Rval;
}

static UINT32 RefCode_StartEncoder(UINT32 Id)
{
    UINT32 Rval;

    Rval = DummyVideoEnc_Start(&g_VideoEnc[Id]);

    return Rval;
}

static UINT32 RefCode_StopEncoder(UINT32 Id)
{
    UINT32 Rval;

    Rval = DummyVideoEnc_Stop(&g_VideoEnc[Id]);

    return Rval;
}

static UINT32 RefCode_DeleteEncoder(UINT32 Id)
{
    UINT32 Rval;

    Rval = F2U(AmbaFIFO_Delete(g_VideoEnc[Id].Fifo));
    if (Rval == OK) {
        g_VideoEnc[Id].Fifo = NULL;
        Rval = DummyVideoEnc_Deinit(&g_VideoEnc[Id]);
        if (Rval != OK) {
            RefCode_Perror(__FUNCTION__, __LINE__, NULL);
        }
    } else {
        RefCode_Perror(__FUNCTION__, __LINE__, NULL);
    }

    return Rval;
}

UINT32 RefCode_FileFeed_Start(UINT32 Id)
{
    UINT32 Rval = OK;

    FILE_FEED_CFG_s *pFileFeedConfig;

    if( Id >= MAX_FILE_FEED_NUM ) {
        AmbaPrint_PrintUInt5("RefCode_FileFeed_Start Id [%d] is over range", Id, 0U, 0U, 0U, 0U);
        Rval = 1U;
    } else {
        pFileFeedConfig = &FileFeedConfig[Id];
        AmbaPrint_PrintStr5("VideoEnc [%s]", pFileFeedConfig->Prefix, NULL, NULL, NULL, NULL);
        Rval = RefCode_CreateEncoder(Id, pFileFeedConfig);
        if (Rval == OK) {
                Rval = RefCode_StartEncoder(Id);
                pFileFeedConfig->pFifo = g_VideoEnc[Id].Fifo;
                pFileFeedConfig->TimeScale = g_VideoEnc[Id].TimeScale;
                pFileFeedConfig->TimePerFrame = g_VideoEnc[Id].TimePerFrame;
        }
    }

    return Rval;
}

UINT32 RefCode_FileFeed_Stop(UINT32 Id)
{
    UINT32 Rval = OK;

    if( Id >= MAX_FILE_FEED_NUM ) {
        AmbaPrint_PrintUInt5("RefCode_FileFeed_Stop Id [%d] is over range", Id, 0U, 0U, 0U, 0U);
    } else {
        Rval = RefCode_StopEncoder(Id);
        if (Rval == OK) {
            Rval = RefCode_DeleteEncoder(Id);
        }
    }

    return Rval;
}

