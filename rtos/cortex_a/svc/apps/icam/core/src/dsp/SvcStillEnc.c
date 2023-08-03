/**
*  @file SvcStillEnc.c
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
*  @details svc boot device
*
*/

#include "AmbaKAL.h"
#include "AmbaMisraFix.h"
#include "AmbaWrap.h"
#include "AmbaCache.h"
#include "AmbaUtility.h"

#include "AmbaDSP_Event.h"
#include "AmbaDSP_EventInfo.h"
#include "AmbaDSP_StillCapture.h"

#include "SvcLog.h"
#include "SvcWrap.h"
#include "SvcErrCode.h"
#include "SvcResCfg.h"
#include "SvcStill.h"
#include "SvcStillEnc.h"
#include "SvcStillMux.h"
#include "SvcStillCap.h"
#include "SvcTask.h"
#include "SvcClock.h"
#include "SvcRecMain.h"
#include "SvcPlat.h"

#define SVC_Y2J_AVAILABLE (1)

#define SVC_JPEG_Q_LEVEL  (95)

typedef struct {
    UINT16             RawSeq;
    UINT16             PicView;
    UINT16             StreamIdx;
    UINT16             Reserved;
    ULONG              BitsBufAddr;
    UINT32             BitsBufSize;
    SVC_YUV_IMG_BUF_s  YuvBufInfo;
} SVC_STL_ENC_STRM_INFO_s;

typedef struct {
    UINT8                 PicType[SVC_STL_MUX_NUM_FILES * SVC_STL_NUM_VIEW];
    UINT16                RawSeq[(UINT32)SVC_STL_MUX_NUM_FILES * SVC_STL_NUM_VIEW];
    UINT16                PicView[(UINT32)SVC_STL_MUX_NUM_FILES * SVC_STL_NUM_VIEW];
    UINT16                Width[(UINT32)SVC_STL_MUX_NUM_FILES * SVC_STL_NUM_VIEW];
    UINT16                Height[(UINT32)SVC_STL_MUX_NUM_FILES * SVC_STL_NUM_VIEW];
    UINT32                StartTime[(UINT32)SVC_STL_MUX_NUM_FILES * SVC_STL_NUM_VIEW];
    UINT8                 InfoWp;
    UINT8                 InfoRp;
    UINT16                StreamIdx;
    SVC_STL_ENC_STRM_INFO_s    Y2jQueBuf[SVC_STL_MUX_NUM_FILES * SVC_STL_NUM_VIEW];
    UINT32                Y2jTime[SVC_STL_NUM_VIEW];
    ULONG                 BitsBufAddr;
    UINT32                BitsBufSize;
    ULONG                 QTblAddr;
    AMBA_KAL_MSG_QUEUE_t  Y2jQue;
    AMBA_KAL_EVENT_FLAG_t Y2jFlg;
    SVC_TASK_CTRL_s       TaskCtrl;
} SVC_STL_ENC_CTRL_s;

static UINT8 StlEnc_DebugOn = 0;
static UINT8 StlEnc_Created = 0;
static SVC_STL_ENC_CTRL_s StlEnc GNU_SECTION_NOZEROINIT;

static void SvcStillEnc_EncSetup(AMBA_DSP_STLENC_CTRL_s *pEncCtrl)
{
    UINT32 Rval;
    static UINT8 JpegQTable[SVC_STL_NUM_VIEW][SIZE_JPEG_Q_TABLE] = {
        [SVC_STL_MAIN_VIEW] = {
            16, 11, 10, 16, 124, 140, 151, 161,
            12, 12, 14, 19, 126, 158, 160, 155,
            14, 13, 16, 24, 140, 157, 169, 156,
            14, 17, 22, 29, 151, 187, 180, 162,
            18, 22, 37, 56, 168, 109, 103, 177,
            24, 35, 55, 64, 181, 104, 113, 192,
            49, 64, 78, 87, 103, 121, 120, 101,
            72, 92, 95, 98, 112, 100, 103, 99 ,
            17, 18, 24, 47, 99 , 99 , 99 , 99 ,
            18, 21, 26, 66, 99 , 99 , 99 , 99 ,
            24, 26, 56, 99, 99 , 99 , 99 , 99 ,
            47, 66, 99, 99, 99 , 99 , 99 , 99 ,
            99, 99, 99, 99, 99 , 99 , 99 , 99 ,
            99, 99, 99, 99, 99 , 99 , 99 , 99 ,
            99, 99, 99, 99, 99 , 99 , 99 , 99 ,
            99, 99, 99, 99, 99 , 99 , 99 , 99
        },
        [SVC_STL_SCRN_VIEW] = {
            16, 11, 10, 16, 124, 140, 151, 161,
            12, 12, 14, 19, 126, 158, 160, 155,
            14, 13, 16, 24, 140, 157, 169, 156,
            14, 17, 22, 29, 151, 187, 180, 162,
            18, 22, 37, 56, 168, 109, 103, 177,
            24, 35, 55, 64, 181, 104, 113, 192,
            49, 64, 78, 87, 103, 121, 120, 101,
            72, 92, 95, 98, 112, 100, 103, 99 ,
            17, 18, 24, 47, 99 , 99 , 99 , 99 ,
            18, 21, 26, 66, 99 , 99 , 99 , 99 ,
            24, 26, 56, 99, 99 , 99 , 99 , 99 ,
            47, 66, 99, 99, 99 , 99 , 99 , 99 ,
            99, 99, 99, 99, 99 , 99 , 99 , 99 ,
            99, 99, 99, 99, 99 , 99 , 99 , 99 ,
            99, 99, 99, 99, 99 , 99 , 99 , 99 ,
            99, 99, 99, 99, 99 , 99 , 99 , 99
        },
        [SVC_STL_THMB_VIEW] = {
            16, 11, 10, 16, 124, 140, 151, 161,
            12, 12, 14, 19, 126, 158, 160, 155,
            14, 13, 16, 24, 140, 157, 169, 156,
            14, 17, 22, 29, 151, 187, 180, 162,
            18, 22, 37, 56, 168, 109, 103, 177,
            24, 35, 55, 64, 181, 104, 113, 192,
            49, 64, 78, 87, 103, 121, 120, 101,
            72, 92, 95, 98, 112, 100, 103, 99 ,
            17, 18, 24, 47, 99 , 99 , 99 , 99 ,
            18, 21, 26, 66, 99 , 99 , 99 , 99 ,
            24, 26, 56, 99, 99 , 99 , 99 , 99 ,
            47, 66, 99, 99, 99 , 99 , 99 , 99 ,
            99, 99, 99, 99, 99 , 99 , 99 , 99 ,
            99, 99, 99, 99, 99 , 99 , 99 , 99 ,
            99, 99, 99, 99, 99 , 99 , 99 , 99 ,
            99, 99, 99, 99, 99 , 99 , 99 , 99
        }
    };

    const UINT8 *pSrc = &JpegQTable[0][0];
    UINT8 *pDst;

    pEncCtrl->QualityLevel = SVC_JPEG_Q_LEVEL;
    SvcStill_CalJpegDqt(JpegQTable[SVC_STL_MAIN_VIEW], (INT32)pEncCtrl->QualityLevel);
    AmbaMisra_TypeCast(&pDst, &StlEnc.QTblAddr);
    Rval = AmbaWrap_memcpy(pDst, pSrc, SIZE_JPEG_Q_TABLE);
    if (Rval == OK) {
        pEncCtrl->QTblAddr = StlEnc.QTblAddr;                   /* Pointer to Q-table array, size of each Q-table is 128 bytes */
        Rval = SvcPlat_CacheClean(StlEnc.QTblAddr, SIZE_JPEG_Q_TABLE);
        if (Rval != OK) {
            SvcLog_NG(SVC_LOG_STL_ENC, "[line %u] SvcPlat_CacheClean return 0x%x", __LINE__, Rval);
        }
        if (StlEnc_DebugOn == 1U) {
            SvcLog_OK(SVC_LOG_STL_ENC, "[SvcStillEnc_EncSetup] w %u h %u", pEncCtrl->YuvBuf.Window.Width, pEncCtrl->YuvBuf.Window.Height);
        }
        pEncCtrl->EncWidth = pEncCtrl->YuvBuf.Window.Width;                   /* Main JPEG width */
        pEncCtrl->EncHeight = pEncCtrl->YuvBuf.Window.Height;                  /* Main JPEG height */
    } else {
        SvcLog_NG(SVC_LOG_STL_ENC, "[line %u] AmbaWrap_memcpy return 0x%x", __LINE__, Rval);
    }
}

/**
* inform the StillMux the specified picture view of YUV and JPEG are skipped
* @param [in] PicView picture view
* @return 0-OK, 1-NG
*/
UINT32 SvcStillEnc_DummyInput(UINT16 PicView)
{
    UINT32 Rval;

    SVC_STL_MUX_INPUT_s MuxInput;
    Rval = AmbaWrap_memset(&MuxInput, 0, sizeof(SVC_STL_MUX_INPUT_s));
    if (Rval != OK) {
        SvcLog_NG(SVC_LOG_STL_ENC, "[line %u] AmbaWrap_memset return 0x%x", __LINE__, Rval);
    } else {
        MuxInput.PicView = PicView;
        Rval = SvcStillMux_InputPic(&MuxInput);
    }
    return Rval;
}

static UINT32 JpegStop(const void *pEventInfo)
{
    UINT32 Rval = AmbaKAL_EventFlagSet(&StlEnc.Y2jFlg, SVC_Y2J_AVAILABLE);
    if (OK != Rval) {
        SvcLog_NG(SVC_LOG_STL_ENC, "Event flag set failed with 0x%x", Rval, 0U);
    } else {
        SvcStillCap_ClearStatus(SVC_STL_ENC_BUSY);
    }
    AmbaMisra_TouchUnused(&pEventInfo);
    return Rval;
}

static UINT32 JpegDataReady(const void *pEventInfo)
{
    SVC_STL_MUX_INPUT_s MuxInput;
    UINT32 DoneTime, Rval, i;

    Rval = AmbaKAL_GetSysTickCount(&DoneTime);
    if (Rval != OK) {
        SvcLog_NG(SVC_LOG_STL_ENC, "AmbaKAL_GetSysTickCount failed! return 0x%x", Rval, 0U);
    } else {
        if (StlEnc_DebugOn == 1U) {
            SvcLog_DBG(SVC_LOG_STL_ENC, "StlEnc.PicView %u, AmbaKAL_GetSysTickCount %u", StlEnc.PicView[StlEnc.InfoRp], DoneTime);
        }
    }
    if (DoneTime >= StlEnc.StartTime[StlEnc.InfoRp]) {
        StlEnc.Y2jTime[StlEnc.PicView[StlEnc.InfoRp]] = DoneTime - StlEnc.StartTime[StlEnc.InfoRp];
    } else {
        StlEnc.Y2jTime[StlEnc.PicView[StlEnc.InfoRp]] = (0xFFFFFFFFU - StlEnc.StartTime[StlEnc.InfoRp]) + DoneTime;
    }
    if (StlEnc_DebugOn == 1U) {
        SvcLog_DBG(SVC_LOG_STL_ENC, "Y2j start %u, Y2jTime %u", StlEnc.StartTime[StlEnc.InfoRp],  StlEnc.Y2jTime[StlEnc.PicView[StlEnc.InfoRp]]);
    }
    Rval = AmbaWrap_memcpy(&MuxInput.PicRdy, pEventInfo, sizeof(AMBA_DSP_ENC_PIC_RDY_s));
    if (Rval != OK) {
        SvcLog_NG(SVC_LOG_STL_ENC, "[line %u] AmbaWrap_memcpy return 0x%x", __LINE__, Rval);
    } else {
        SvcLog_OK(SVC_LOG_STL_ENC, "[JpegDataReady]", 0, 0);
        if (StlEnc.PicView[StlEnc.InfoRp] == SVC_STL_THMB_VIEW) {
            SvcLog_OK(SVC_LOG_STL_ENC, "y2j time: main %u ms, scrn %u ms", StlEnc.Y2jTime[SVC_STL_MAIN_VIEW], StlEnc.Y2jTime[SVC_STL_SCRN_VIEW]);
            SvcLog_OK(SVC_LOG_STL_ENC, "          thm %u ms, total %u ms", StlEnc.Y2jTime[SVC_STL_THMB_VIEW], StlEnc.Y2jTime[SVC_STL_MAIN_VIEW] + StlEnc.Y2jTime[SVC_STL_SCRN_VIEW] + StlEnc.Y2jTime[SVC_STL_THMB_VIEW]);
            for (i = 0; i < SVC_STL_NUM_VIEW; i ++) {
                StlEnc.Y2jTime[i] = 0;
            }
        }
        if (MuxInput.PicRdy.FrameType == PIC_FRAME_JPG) {
            MuxInput.PicView = StlEnc.PicView[StlEnc.InfoRp];
            MuxInput.RawSeq = StlEnc.RawSeq[StlEnc.InfoRp];
            MuxInput.Width = StlEnc.Width[StlEnc.InfoRp];
            MuxInput.Height = StlEnc.Height[StlEnc.InfoRp];
            MuxInput.PicType = StlEnc.PicType[StlEnc.InfoRp];
            if (MuxInput.PicType == SVC_STL_TYPE_NULL_WR) {
                Rval = OK;
            } else {
                Rval = SvcStillMux_InputPic(&MuxInput);
            }
            if (OK == Rval) {
                UINT32 StartBits;
                Rval = AmbaWrap_memset(&StartBits, 0, sizeof(StartBits));
                if (Rval != OK) {
                    SvcLog_NG(SVC_LOG_STL_ENC, "[%u] AmbaWrap_memset return 0x%x", __LINE__, Rval);
                }
                SvcRecMain_Control(SVC_RCM_GET_ENC_STATUS, 0, NULL, &StartBits);
                SvcClock_FeatureCtrl(0, SVC_CLK_FEA_BIT_CODEC);
            }
            if (StlEnc_DebugOn == 1U) {
                SvcLog_DBG(SVC_LOG_STL_ENC, "StlEnc.InfoRp %u", StlEnc.InfoRp, 0);
            }
            if (StlEnc.InfoRp < (((UINT8)SVC_STL_MUX_NUM_FILES * SVC_STL_NUM_VIEW) - 1U)) {
                StlEnc.InfoRp ++;
            } else {
                StlEnc.InfoRp = 0;
            }
        }
    }
    return Rval;
}

/**
* wait still encode done
* @return 0-OK, 1-NG
*/
UINT32 SvcStillEnc_WaitJpegDone(void)
{
    UINT32 Rval, ActualFlags = 0;
    Rval = AmbaKAL_EventFlagGet(&StlEnc.Y2jFlg, SVC_Y2J_AVAILABLE, AMBA_KAL_FLAGS_ALL, AMBA_KAL_FLAGS_CLEAR_AUTO, &ActualFlags, 1000);
    if (OK != Rval) {
        SvcLog_NG(SVC_LOG_STL_ENC, "Event flag get failed with 0x%x", Rval, 0U);
    }
    return Rval;
}

/**
* set encode stream index for JPEG
* @param [in] StreamIdx index of record stream
* @return 0-OK, 1-NG
*/
UINT32 SvcStillEnc_SetStreamIndex(UINT16 StreamIdx)
{
    StlEnc.StreamIdx = StreamIdx;
    return SVC_OK;
}

/**
* set jpeg buffer address and size
* @param [in] BitsBufAddr base of jpeg buffer
* @param [in] BitsBufSize size of jpeg buffer
* @return 0-OK, 1-NG
*/
UINT32 SvcStillEnc_SetJpegBufer(ULONG BitsBufAddr, UINT32 BitsBufSize)
{
    StlEnc.BitsBufAddr = BitsBufAddr;
    StlEnc.BitsBufSize = BitsBufSize;
    return SVC_OK;
}

/**
* input yuv to still encode task
* @param [in] pInfo pointer to input yuv info
* @return 0-OK, 1-NG
*/
UINT32 SvcStillEnc_InputYuv(const SVC_STL_ENC_INFO_s *pInfo)
{
    UINT32 Rval;
    Rval = AmbaKAL_EventFlagClear(&StlEnc.Y2jFlg, 0xFFFFFFFFU);
    if (OK != Rval) {
        SvcLog_NG(SVC_LOG_STL_ENC, "Event flag clear failed with 0x%x", Rval, 0U);
    }
    if (OK == Rval) {
        SVC_STL_ENC_STRM_INFO_s Info;
        Info.RawSeq = pInfo->RawSeq;
        Info.PicView = pInfo->PicView;
        Info.StreamIdx = StlEnc.StreamIdx;
        Info.BitsBufAddr = StlEnc.BitsBufAddr;
        Info.BitsBufSize = StlEnc.BitsBufSize;
        Info.YuvBufInfo.BaseAddrY  = pInfo->YuvBufInfo.BaseAddrY ;      /* Luma (Y) data buffer address */
        Info.YuvBufInfo.BaseAddrUV = pInfo->YuvBufInfo.BaseAddrUV;     /* Chroma (UV) buffer address */
        Info.YuvBufInfo.Pitch      = pInfo->YuvBufInfo.Pitch     ;          /* YUV data buffer pitch */
        Info.YuvBufInfo.Width      = pInfo->YuvBufInfo.Width     ;
        Info.YuvBufInfo.Height     = pInfo->YuvBufInfo.Height    ;
        Info.YuvBufInfo.DataFmt    = pInfo->YuvBufInfo.DataFmt   ;        /* YUV Data format */
        StlEnc.PicType[StlEnc.InfoWp] = pInfo->PicType;

        SvcStillCap_SetStatus(SVC_STL_ENC_BUSY);

        Rval = AmbaKAL_MsgQueueSend(&StlEnc.Y2jQue, &Info, 1000);
        if (OK != Rval) {
            SvcLog_NG(SVC_LOG_STL_ENC, "Message send failed with 0x%x", Rval, 0U);
        }
    }
    return Rval;
}

static void* SvcStillEnc_TaskEntry(void* EntryArg)
{
    SVC_STL_ENC_STRM_INFO_s Msg;
    AMBA_DSP_STLENC_CTRL_s EncCtrl;
    UINT32 Rval = OK;;

    AmbaMisra_TouchUnused(EntryArg);

    while (Rval == OK) {
        Rval = AmbaKAL_MsgQueueReceive(&StlEnc.Y2jQue, &Msg, AMBA_KAL_WAIT_FOREVER);
        if (OK == Rval) {
            Rval = AmbaWrap_memset(&EncCtrl, 0, sizeof(AMBA_DSP_STLENC_CTRL_s));
            if (Rval != OK) {
                SvcLog_NG(SVC_LOG_STL_ENC, "[line %u] AmbaWrap_memset return 0x%x", __LINE__, Rval);
            } else {
                //SvcLog_OK(SVC_LOG_STL_ENC, "sizeof(AMBA_DSP_YUV_IMG_BUF_s) %u sizeof(Msg.YuvBufInfo) %u", sizeof(AMBA_DSP_YUV_IMG_BUF_s), sizeof(Msg.YuvBufInfo));
                SvcStill_SvcYuv2SspYuv(&Msg.YuvBufInfo, &EncCtrl.YuvBuf);
                //SvcLog_OK(SVC_LOG_STL_ENC, "Msg.YuvBufInfo %u %u", Msg.YuvBufInfo.Width, Msg.YuvBufInfo.Height);
                //SvcLog_OK(SVC_LOG_STL_ENC, "EncCtrl.YuvBuf %u %u", EncCtrl.YuvBuf.Window.Width, EncCtrl.YuvBuf.Window.Height);
                SvcStillEnc_EncSetup(&EncCtrl);
                StlEnc.PicView[StlEnc.InfoWp] = Msg.PicView;
                StlEnc.RawSeq[StlEnc.InfoWp] = Msg.RawSeq;
                StlEnc.Width[StlEnc.InfoWp] = Msg.YuvBufInfo.Width;
                StlEnc.Height[StlEnc.InfoWp] = Msg.YuvBufInfo.Height;
                EncCtrl.BitsBufAddr = Msg.BitsBufAddr;
                EncCtrl.BitsBufSize = Msg.BitsBufSize;
                if (StlEnc_DebugOn == 1U) {
                    SvcStillLog("[SvcStillEnc_TaskEntry] raw_seq %u, view 0x%x, EncCtrl.BitsBufAddr %p EncCtrl.BitsBufSize 0x%x", Msg.RawSeq, Msg.PicView, EncCtrl.BitsBufAddr, EncCtrl.BitsBufSize, 0);
                }
                {
                    UINT32 StartBits;
                    Rval = AmbaWrap_memset(&StartBits, 0, sizeof(StartBits));
                    if (Rval != OK) {
                        SvcLog_NG(SVC_LOG_STL_ENC, "[%u] AmbaWrap_memset return 0x%x", __LINE__, Rval);
                    }
                    SvcRecMain_Control(SVC_RCM_GET_ENC_STATUS, 0, NULL, &StartBits);
                    SvcClock_FeatureCtrl(1, SVC_CLK_FEA_BIT_CODEC);
                }
                Rval = AmbaKAL_GetSysTickCount(&StlEnc.StartTime[StlEnc.InfoWp]);
                if (Rval != OK) {
                    SvcLog_NG(SVC_LOG_STL_ENC, "AmbaKAL_GetSysTickCount failed! return 0x%x", Rval, 0U);
                } else {
                    if (StlEnc_DebugOn == 1U) {
                        SvcLog_DBG(SVC_LOG_STL_ENC, "StlEnc.PicView %u, AmbaKAL_GetSysTickCount %u", StlEnc.PicView[StlEnc.InfoWp], StlEnc.StartTime[StlEnc.InfoWp]);
                    }
                }
                if (StlEnc_DebugOn == 1U) {
                    SvcLog_DBG(SVC_LOG_STL_ENC, "StlEnc.InfoWp %u", StlEnc.InfoWp, 0);
                }
                if (StlEnc.InfoWp < (((UINT8)SVC_STL_MUX_NUM_FILES * SVC_STL_NUM_VIEW) - 1U)) {
                    StlEnc.InfoWp ++;
                } else {
                    StlEnc.InfoWp = 0;
                }
                if (Rval == OK) {
                    if (StlEnc_DebugOn == 1U) {
                        SvcLog_DBG(SVC_LOG_STL_ENC, "AmbaDSP_StillEncodeCtrl() start", 0, 0U);
                    }
                    Rval = AmbaDSP_StillEncodeCtrl(Msg.StreamIdx, &EncCtrl, NULL);
                    if (StlEnc_DebugOn == 1U) {
                        SvcLog_DBG(SVC_LOG_STL_ENC, "AmbaDSP_StillEncodeCtrl() return", 0, 0U);
                    }
                }
                if (OK != Rval) {
                    SvcLog_NG(SVC_LOG_STL_ENC, "AmbaDSP_StillEncodeCtrl() failed. return 0x%x", Rval, 0U);
                }
            }
        }
    }

    return NULL;
}

/**
* turn on/off debugging
* @param [in] On 0-off, 1-on
*/
void SvcStillEnc_Debug(UINT8 On)
{
    StlEnc_DebugOn = On;
    SvcLog_OK(SVC_LOG_STL_ENC, "SvcStillEnc_Debug %u", On, 0);
}

/**
* initialize and create still encode task
* @param [in] pCfg pointer to configuration of still encode task
* @return 0-OK, 1-NG
*/
UINT32 SvcStillEnc_Create(const SVC_STL_ENC_CONFIG_s *pCfg)
{
    UINT32                  Rval, i;
    static UINT8            SvcStillEnc[SVC_STL_ENC_STACK_SIZE] GNU_SECTION_NOZEROINIT;
    static char             MsgQueName[] = "Y2jQue";
    static char             FlgName[] = "Y2jFlg";

    if (StlEnc_Created == 0U) {
        Rval = AmbaWrap_memset(&StlEnc, 0x0, sizeof(StlEnc));
        if (Rval != OK) {
            SvcLog_NG(SVC_LOG_STL_ENC, "[line %u] AmbaWrap_memset return 0x%x", __LINE__, Rval);
        } else {
            /* task create */
            StlEnc.TaskCtrl.Priority    = pCfg->Priority;
            StlEnc.TaskCtrl.EntryFunc   = SvcStillEnc_TaskEntry;
            StlEnc.TaskCtrl.EntryArg    = 0U;
            StlEnc.TaskCtrl.pStackBase  = SvcStillEnc;
            StlEnc.TaskCtrl.StackSize   = SVC_STL_ENC_STACK_SIZE;
            StlEnc.TaskCtrl.CpuBits     = pCfg->CpuBits;
            StlEnc.QTblAddr = pCfg->QAddr;

            for (i = 0; i < SVC_STL_NUM_VIEW; i ++) {
                StlEnc.Y2jTime[i] = 0;
            }

            Rval = AmbaKAL_EventFlagCreate(&StlEnc.Y2jFlg, FlgName);
            if (OK != Rval) {
                SvcLog_NG(SVC_LOG_STL_ENC, "Event flag create failed with 0x%x", Rval, 0U);
            } else {
                Rval = AmbaKAL_EventFlagSet(&StlEnc.Y2jFlg, SVC_Y2J_AVAILABLE);
                if (OK != Rval) {
                    SvcLog_NG(SVC_LOG_STL_ENC, "Event flag set failed with 0x%x", Rval, 0U);
                }
            }
            if (Rval == OK) {
                Rval = AmbaDSP_EventHandlerRegister(AMBA_DSP_EVENT_JPEG_DATA_RDY, JpegDataReady);
                if (OK != Rval) {
                    SvcLog_NG(SVC_LOG_STL_ENC, "AmbaDSP_EventHandlerRegister fail, return 0x%x", Rval, 0);
                }
            }
            if (Rval == OK) {
                Rval = AmbaDSP_EventHandlerRegister(AMBA_DSP_EVENT_VIDEO_ENC_STOP, JpegStop);
                if (OK != Rval) {
                    SvcLog_NG(SVC_LOG_STL_ENC, "AmbaDSP_EventHandlerRegister fail for jpeg stop, return 0x%x", Rval, 0);
                }
            }
            if (Rval == OK) {
                Rval = AmbaKAL_MsgQueueCreate(&StlEnc.Y2jQue, MsgQueName, (UINT32)sizeof(SVC_STL_ENC_STRM_INFO_s), StlEnc.Y2jQueBuf, (UINT32)sizeof(StlEnc.Y2jQueBuf));
                if (OK != Rval) {
                    SvcLog_NG(SVC_LOG_STL_ENC, "Y2jQue created failed! return 0x%x", Rval, 0U);
                }
            }
            if (Rval == OK) {
                Rval = SvcTask_Create("SvcStillEnc", &StlEnc.TaskCtrl);
                if (SVC_OK != Rval) {
                    SvcLog_NG(SVC_LOG_STL_ENC, "SvcStillEnc_Task created failed! return 0x%x", Rval, 0U);
                } else {
                    StlEnc_Created = 1;
                }
            }
        }
    } else {
        Rval = SVC_OK;
    }
    return Rval;
}

/**
* de-initialize and delete still encode task
* @return 0-OK, 1-NG
*/
UINT32 SvcStillEnc_Delete(void)
{
    UINT32 Rval;

    if (StlEnc_Created == 1U) {
        Rval = SvcTask_Destroy(&StlEnc.TaskCtrl);
        if (Rval != SVC_OK) {
            SvcLog_NG(SVC_LOG_STL_ENC, "SvcStillEnc_Task delete failed! return 0x%x", Rval, 0U);
        }
        if (Rval == SVC_OK) {
            Rval = AmbaKAL_MsgQueueDelete(&StlEnc.Y2jQue);
            if (Rval != SVC_OK) {
                SvcLog_NG(SVC_LOG_STL_ENC, "Y2jQue delete failed! return 0x%x", Rval, 0U);
            }
        }
        if (Rval == SVC_OK) {
            Rval = AmbaDSP_EventHandlerUnRegister(AMBA_DSP_EVENT_JPEG_DATA_RDY, JpegDataReady);
            if (Rval != SVC_OK) {
                SvcLog_NG(SVC_LOG_STL_ENC, "AmbaDSP_EventHandlerUnRegister fail, return 0x%x", Rval, 0);
            }
        }
        if (Rval == SVC_OK) {
            Rval = AmbaDSP_EventHandlerUnRegister(AMBA_DSP_EVENT_VIDEO_ENC_STOP, JpegStop);
            if (Rval != SVC_OK) {
                SvcLog_NG(SVC_LOG_STL_ENC, "AmbaDSP_EventHandlerUnRegister fail for jpeg stop, return 0x%x", Rval, 0);
            }
        }
        if (Rval == SVC_OK) {
            Rval = AmbaKAL_EventFlagDelete(&StlEnc.Y2jFlg);
            if (Rval == SVC_OK) {
                StlEnc_Created = 0;
            } else {
                SvcLog_NG(SVC_LOG_STL_ENC, "AmbaKAL_EventFlagDelete fail, return 0x%x", Rval, 0);
            }
        }
    } else {
        Rval = SVC_OK;
    }
    return Rval;
}
