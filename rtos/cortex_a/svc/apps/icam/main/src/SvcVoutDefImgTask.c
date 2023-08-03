/**
 *  @file SvcVoutDefImgTask.c
 *
 *  @copyright Copyright (c) 2018 Ambarella, Inc.
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
 *  @details
 *
 */

#include "AmbaTypes.h"
#include "AmbaKAL.h"
#include "AmbaDSP_Event.h"
#include "AmbaDSP_EventInfo.h"
#include "AmbaDSP_VOUT.h"

#include "AmbaDspInt.h"
#include "AmbaSvcWrap.h"

#include "SvcErrCode.h"
#include "SvcLog.h"
#include "SvcTask.h"
#include "SvcMem.h"
#include "SvcBuffer.h"
#include "SvcBufMap.h"
#include "SvcDisplay.h"
#include "SvcVoutDefImgTask.h"

#include "AmbaGDMA.h"
#define USING_DSP_INTERNAL_ADDR
#define VOUT_DEF_IMG_GDMA_ENABLE
#define VOUT_DEF_IMG_GDMA_TIMEOUT           (50U)

#define SVC_LOG_VOUT_DEF_IMG_TASK           "VOUT_DEF_IMG_TASK"
#define VOUT_DEF_IMG_TASK_STACK_SIZE        (0x8000U)
#define VOUT_DEF_IMG_TASK_BUF_DEPTH         (16U)
#define VOUT_DEF_IMG_TASK_BUF_WIDTH         (1920U)
#define VOUT_DEF_IMG_TASK_BUF_HEIGHT        (1080U)
#define VOUT_DEF_IMG_TASK_BUF_MARGIN        (180U)
#define VOUT_DEF_IMG_TASK_BUF_SIZE_Y        (VOUT_DEF_IMG_TASK_BUF_WIDTH*VOUT_DEF_IMG_TASK_BUF_HEIGHT)
#define VOUT_DEF_IMG_TASK_BUF_SIZE_UV       (VOUT_DEF_IMG_TASK_BUF_WIDTH*VOUT_DEF_IMG_TASK_BUF_HEIGHT)
#define VOUT_DEF_IMG_TASK_BUF_SIZE          (VOUT_DEF_IMG_TASK_BUF_SIZE_Y+VOUT_DEF_IMG_TASK_BUF_SIZE_UV)
#define VOUT_DEF_IMG_TASK_DISP_GRID_H       (3U)
#define VOUT_DEF_IMG_MSG_QUEUE_SIZE         (64U)
#define VOUT_DEF_IMG_MAX_VIN_NUM            (6U)
#define VOUT_DEF_IMG_MAX_VIN_SELECT_BITS    (0x3FU)
#define VOUT_DEF_IMG_MAIN_STREAM_BASE       (0x80000000U)
#define VOUT_DEF_IMG_YUV_STREAM_BASE        (0x80000006U)
#define VOUT_DEF_IMG_STRM_TIMEOUT           (100U)
#define VOUT_DEF_IMG_FLAG_VOUT_TASK_READY   (0x00000001U)

typedef struct {
   AMBA_DSP_YUV_DATA_RDY_s      MainYuvInfo;
} SRC_INFO_MSG_s;

typedef struct {
   ULONG                       YAddr;
   ULONG                       UVAddr;
} VOUT_DEF_IMG_MSG_s;

typedef struct {
   UINT32                       DispAlt;
   AMBA_KAL_MSG_QUEUE_t         SrcQueId;
   AMBA_KAL_MSG_QUEUE_t         VoutQueId;
   AMBA_DSP_INT_PORT_s          IntPort;
   AMBA_DSP_VOUT_VIDEO_CFG_s    VideoCfg;
} VOUT_DEF_IMG_CTRL_s;

static VOUT_DEF_IMG_CTRL_s g_VoutDefImgCtrl GNU_SECTION_NOZEROINIT;
static AMBA_KAL_EVENT_FLAG_t g_VoutDefImgFlag;
static UINT8 gDebug = 0U;

static UINT32 VoutDefImgDataRdyHandler(const void *pEventInfo)
{
    SRC_INFO_MSG_s Msg;

    AmbaSvcWrap_MisraMemcpy(&(Msg.MainYuvInfo), pEventInfo, sizeof(AMBA_DSP_YUV_DATA_RDY_s));

    if (((g_VoutDefImgCtrl.DispAlt < VOUT_DEF_IMG_MAX_VIN_NUM) && (Msg.MainYuvInfo.ViewZoneId == (g_VoutDefImgCtrl.DispAlt+VOUT_DEF_IMG_MAIN_STREAM_BASE))) ||
        ((g_VoutDefImgCtrl.DispAlt == VOUT_DEF_IMG_MAX_VIN_NUM) && (Msg.MainYuvInfo.ViewZoneId >= VOUT_DEF_IMG_YUV_STREAM_BASE))) {
        (void)AmbaKAL_MsgQueueSend(&(g_VoutDefImgCtrl.SrcQueId), &Msg, AMBA_KAL_NO_WAIT);
    }

    return SVC_OK;
}

static UINT32 PrepareDefImgBuf(UINT8 **ppYBuf, UINT8 **ppUVBuf)
{
    UINT8 *pSrc;
    UINT8 *pDst;
    UINT32 RetVal = SVC_OK, i, j, ExitLoop = 0U, DefBufRdy = 0U;
    UINT32 TimeStart = 0U, TimeEnd = 0U, TimeDiff = 0U;
    ULONG YAddr = 0, UVAddr = 0, DstYAddr, DstUVAddr;
    static UINT32 YuvReadyBits = VOUT_DEF_IMG_MAX_VIN_SELECT_BITS;
    UINT32 Pitch = VOUT_DEF_IMG_TASK_BUF_WIDTH, Height = VOUT_DEF_IMG_TASK_BUF_HEIGHT;
    UINT32 DstPitch = g_VoutDefImgCtrl.VideoCfg.DefaultImgConfig.Pitch;
    SRC_INFO_MSG_s Msg;

    (void)AmbaKAL_GetSysTickCount(&TimeStart);
    do {
        RetVal = AmbaKAL_MsgQueueReceive(&(g_VoutDefImgCtrl.SrcQueId), &Msg, AMBA_KAL_WAIT_FOREVER);
        if (KAL_ERR_NONE != RetVal) {
            SvcLog_NG(SVC_LOG_VOUT_DEF_IMG_TASK, "Receive SrcQueId error(0x%x)", RetVal, 0U);
            RetVal = SVC_NG;
        }

        if (SVC_OK == RetVal) {
            Pitch = Msg.MainYuvInfo.Buffer.Pitch;
            Height = Msg.MainYuvInfo.Buffer.Window.Height;

#ifndef USING_DSP_INTERNAL_ADDR
            AmbaMisra_TypeCast(&YAddr, &(*ppYBuf));
            AmbaMisra_TypeCast(&UVAddr, &(*ppUVBuf));
            AmbaMisra_TouchUnused(&(*ppYBuf));
            AmbaMisra_TouchUnused(&(*ppUVBuf));
#endif

            if (Msg.MainYuvInfo.ViewZoneId < VOUT_DEF_IMG_YUV_STREAM_BASE) {
#ifdef USING_DSP_INTERNAL_ADDR
                YAddr = Msg.MainYuvInfo.Buffer.BaseAddrY;
                UVAddr = Msg.MainYuvInfo.Buffer.BaseAddrUV;
                AmbaMisra_TypeCast(&(*ppYBuf), &YAddr);
                AmbaMisra_TypeCast(&(*ppUVBuf), &UVAddr);
                if (gDebug != 0U) {
                    SvcLog_DBG(SVC_LOG_VOUT_DEF_IMG_TASK, "  SrcYAddr = 0x%llx, SrcUVAddr = 0x%llx", Msg.MainYuvInfo.Buffer.BaseAddrY, Msg.MainYuvInfo.Buffer.BaseAddrUV);
                }
#else
#ifdef VOUT_DEF_IMG_GDMA_ENABLE
                {
                    AMBA_GDMA_BLOCK_s BlockCopy;

                    AmbaMisra_TypeCast(&pSrc, &(Msg.MainYuvInfo.Buffer.BaseAddrY));
                    AmbaMisra_TypeCast(&pDst, &(YAddr));
                    BlockCopy.BltWidth = Pitch;
                    BlockCopy.BltHeight = Height;
                    BlockCopy.DstRowStride = DstPitch;
                    BlockCopy.SrcRowStride = Pitch;
                    BlockCopy.pDstImg = pDst;
                    BlockCopy.pSrcImg = pSrc;
                    BlockCopy.PixelFormat = AMBA_GDMA_8_BIT;
                    RetVal = AmbaGDMA_BlockCopy(&BlockCopy, NULL, 0U, VOUT_DEF_IMG_GDMA_TIMEOUT);
                    RetVal |= AmbaGDMA_WaitAllCompletion(VOUT_DEF_IMG_GDMA_TIMEOUT);

                    AmbaMisra_TypeCast(&pSrc, &(Msg.MainYuvInfo.Buffer.BaseAddrUV));
                    AmbaMisra_TypeCast(&pDst, &(UVAddr));
                    BlockCopy.BltWidth = Pitch;
                    BlockCopy.BltHeight = (Height/2U);
                    BlockCopy.DstRowStride = DstPitch;
                    BlockCopy.SrcRowStride = Pitch;
                    BlockCopy.pDstImg = pDst;
                    BlockCopy.pSrcImg = pSrc;
                    BlockCopy.PixelFormat = AMBA_GDMA_8_BIT;
                    RetVal |= AmbaGDMA_BlockCopy(&BlockCopy, NULL, 0U, VOUT_DEF_IMG_GDMA_TIMEOUT);
                    RetVal |= AmbaGDMA_WaitAllCompletion(VOUT_DEF_IMG_GDMA_TIMEOUT);
                }
#else
                {
                    UINT32 Size;
                    AmbaMisra_TypeCast(&pSrc, &(Msg.MainYuvInfo.Buffer.BaseAddrY));
                    AmbaMisra_TypeCast(&pDst, &(YAddr));
                    Size = Pitch * Height;
                    AmbaSvcWrap_MisraMemcpy(pDst, pSrc, Size);

                    AmbaMisra_TypeCast(&pSrc, &(Msg.MainYuvInfo.Buffer.BaseAddrUV));
                    AmbaMisra_TypeCast(&pDst, &(UVAddr));
                    Size /= 2U;
                    AmbaSvcWrap_MisraMemcpy(pDst, pSrc, Size);
                }
#endif
#endif
                DefBufRdy = VOUT_DEF_IMG_MAX_VIN_SELECT_BITS;
                ExitLoop = 1U;
            } else {
                UINT32 YuvStrm = Msg.MainYuvInfo.ViewZoneId - VOUT_DEF_IMG_YUV_STREAM_BASE;
                ULONG SrcYAddr = Msg.MainYuvInfo.Buffer.BaseAddrY;
                ULONG SrcUVAddr = Msg.MainYuvInfo.Buffer.BaseAddrUV;
                DstYAddr = YAddr;
                DstUVAddr = UVAddr;
                DstPitch = g_VoutDefImgCtrl.VideoCfg.DefaultImgConfig.Pitch;

                DstYAddr += (VOUT_DEF_IMG_TASK_BUF_WIDTH*VOUT_DEF_IMG_TASK_BUF_MARGIN);
                DstYAddr += (Height * ((YuvStrm / VOUT_DEF_IMG_TASK_DISP_GRID_H) * DstPitch));
                DstYAddr += (Pitch * (YuvStrm % VOUT_DEF_IMG_TASK_DISP_GRID_H));

                DstUVAddr += ((VOUT_DEF_IMG_TASK_BUF_WIDTH*VOUT_DEF_IMG_TASK_BUF_MARGIN) / 2U);
                DstUVAddr += ((Height * ((YuvStrm / VOUT_DEF_IMG_TASK_DISP_GRID_H) * DstPitch)) / 2U);
                DstUVAddr += (Pitch * (YuvStrm % VOUT_DEF_IMG_TASK_DISP_GRID_H));

                if (gDebug != 0U) {
                    SvcLog_NG(SVC_LOG_VOUT_DEF_IMG_TASK, "  YuvStrm %u",YuvStrm,0);
                    SvcLog_NG(SVC_LOG_VOUT_DEF_IMG_TASK, "  SrcYAddr = 0x%llx, SrcUVAddr = 0x%llx", SrcUVAddr, SrcUVAddr);
                    SvcLog_NG(SVC_LOG_VOUT_DEF_IMG_TASK, "  Height %u Pitch %u",Height,Pitch);
                    SvcLog_NG(SVC_LOG_VOUT_DEF_IMG_TASK, "  DstYAddr = 0x%llx, DstUVAddr = 0x%llx", DstYAddr, DstUVAddr);
                    SvcLog_NG(SVC_LOG_VOUT_DEF_IMG_TASK, "  DstPitch %u ",DstPitch,0);
                }

#ifdef VOUT_DEF_IMG_GDMA_ENABLE
                {
                    AMBA_GDMA_BLOCK_s BlockCopy;

                    AmbaMisra_TypeCast(&pSrc, &(SrcYAddr));
                    AmbaMisra_TypeCast(&pDst, &(DstYAddr));
                    BlockCopy.BltWidth = Pitch;
                    BlockCopy.BltHeight = Height;
                    BlockCopy.DstRowStride = DstPitch;
                    BlockCopy.SrcRowStride = Pitch;
                    BlockCopy.pDstImg = pDst;
                    BlockCopy.pSrcImg = pSrc;
                    BlockCopy.PixelFormat = AMBA_GDMA_8_BIT;
                    RetVal = AmbaGDMA_BlockCopy(&BlockCopy, NULL, 0U, VOUT_DEF_IMG_GDMA_TIMEOUT);
                    RetVal |= AmbaGDMA_WaitAllCompletion(VOUT_DEF_IMG_GDMA_TIMEOUT);

                    AmbaMisra_TypeCast(&pSrc, &(SrcUVAddr));
                    AmbaMisra_TypeCast(&pDst, &(DstUVAddr));
                    BlockCopy.BltWidth = Pitch;
                    BlockCopy.BltHeight = (Height/2U);
                    BlockCopy.DstRowStride = DstPitch;
                    BlockCopy.SrcRowStride = Pitch;
                    BlockCopy.pDstImg = pDst;
                    BlockCopy.pSrcImg = pSrc;
                    BlockCopy.PixelFormat = AMBA_GDMA_8_BIT;
                    RetVal |= AmbaGDMA_BlockCopy(&BlockCopy, NULL, 0U, VOUT_DEF_IMG_GDMA_TIMEOUT);
                    RetVal |= AmbaGDMA_WaitAllCompletion(VOUT_DEF_IMG_GDMA_TIMEOUT);

                    if ((VOUT_DEF_IMG_MAX_VIN_NUM - 1U) == YuvStrm) {
                        DstYAddr  = YAddr + (VOUT_DEF_IMG_TASK_BUF_WIDTH*(VOUT_DEF_IMG_TASK_BUF_HEIGHT-VOUT_DEF_IMG_TASK_BUF_MARGIN));
                        DstUVAddr = UVAddr + ((VOUT_DEF_IMG_TASK_BUF_WIDTH*(VOUT_DEF_IMG_TASK_BUF_HEIGHT-VOUT_DEF_IMG_TASK_BUF_MARGIN))/2U);
                    }
                }
#else
                {
                    for (i = 0U; i < Height; i++) {
                        AmbaMisra_TypeCast(&pSrc, &(SrcYAddr));
                        AmbaMisra_TypeCast(&pDst, &(DstYAddr));
                        AmbaSvcWrap_MisraMemcpy(pDst, pSrc, Pitch);
                        SrcYAddr += Pitch;
                        DstYAddr += DstPitch;
                    }

                    for (i = 0U; i < (Height/2U); i++) {
                        AmbaMisra_TypeCast(&pSrc, &(SrcUVAddr));
                        AmbaMisra_TypeCast(&pDst, &(DstUVAddr));
                        AmbaSvcWrap_MisraMemcpy(pDst, pSrc, Pitch);
                        SrcUVAddr += Pitch;
                        DstUVAddr += DstPitch;
                    }
                }

#endif

                if ((VOUT_DEF_IMG_MAX_VIN_NUM - 1U) == YuvStrm) {
                    ULONG BtmOSDYAddr  = YAddr;
                    ULONG BtmOSDUVAddr = UVAddr;

                    BtmOSDYAddr += (VOUT_DEF_IMG_TASK_BUF_WIDTH*VOUT_DEF_IMG_TASK_BUF_MARGIN);
                    BtmOSDYAddr += (Height * ((VOUT_DEF_IMG_MAX_VIN_NUM / VOUT_DEF_IMG_TASK_DISP_GRID_H) * DstPitch));

                    BtmOSDUVAddr += ((VOUT_DEF_IMG_TASK_BUF_WIDTH*VOUT_DEF_IMG_TASK_BUF_MARGIN) / 2U);
                    BtmOSDUVAddr += ((Height * ((VOUT_DEF_IMG_MAX_VIN_NUM / VOUT_DEF_IMG_TASK_DISP_GRID_H) * DstPitch)) / 2U);

                    /* Draw Top OSD */
                    AmbaMisra_TypeCast(&pDst, &(YAddr));
                    AmbaSvcWrap_MisraMemset(pDst, 0x0, (VOUT_DEF_IMG_TASK_BUF_WIDTH*VOUT_DEF_IMG_TASK_BUF_MARGIN));
                    AmbaMisra_TypeCast(&pDst, &(UVAddr));
                    AmbaSvcWrap_MisraMemset(pDst, 0x80, ((VOUT_DEF_IMG_TASK_BUF_WIDTH*VOUT_DEF_IMG_TASK_BUF_MARGIN)/2U));

                    /* Draw Bottom OSD */
                    AmbaMisra_TypeCast(&pDst, &(BtmOSDYAddr));
                    AmbaSvcWrap_MisraMemset(pDst, 0x0, (VOUT_DEF_IMG_TASK_BUF_WIDTH*VOUT_DEF_IMG_TASK_BUF_MARGIN));
                    AmbaMisra_TypeCast(&pDst, &(BtmOSDUVAddr));
                    AmbaSvcWrap_MisraMemset(pDst, 0x80, ((VOUT_DEF_IMG_TASK_BUF_WIDTH*VOUT_DEF_IMG_TASK_BUF_MARGIN)/2U));
                }

                DefBufRdy |= (1U << YuvStrm);

                (void)AmbaKAL_GetSysTickCount(&TimeEnd);
                if (TimeEnd >= TimeStart) {
                    TimeDiff = TimeEnd - TimeStart;
                } else {
                    TimeDiff = (0xFFFFFFFFU - TimeStart) + TimeEnd;
                }

                if ((DefBufRdy >= YuvReadyBits) || (TimeDiff > VOUT_DEF_IMG_STRM_TIMEOUT)) {
                    ExitLoop = 1U;
                    YuvReadyBits = DefBufRdy;
                } else {
                    ExitLoop = 0U;
                }

                //SvcLog_OK(SVC_LOG_VOUT_DEF_IMG_TASK, "DefBufRdy 0x%08X YuvStrm %d", DefBufRdy, YuvStrm);
                //SvcLog_OK(SVC_LOG_VOUT_DEF_IMG_TASK, "DstYAddr 0x%08X DstUVAddr 0x%08X", DstYAddr, DstUVAddr);
            }
        }
    } while (ExitLoop == 0U);

    if (DefBufRdy < VOUT_DEF_IMG_MAX_VIN_SELECT_BITS) {
        for (i = 0U; i < VOUT_DEF_IMG_MAX_VIN_NUM; i++) {
            if ((DefBufRdy & ((UINT32)0x1U << i)) == 0U) {
                SvcLog_OK(SVC_LOG_VOUT_DEF_IMG_TASK, "YuvStrm %d is absent", i, 0U);
                DstYAddr = YAddr;
                DstUVAddr = UVAddr;

                DstYAddr += (VOUT_DEF_IMG_TASK_BUF_WIDTH*VOUT_DEF_IMG_TASK_BUF_MARGIN);
                DstYAddr += (Height * ((i / VOUT_DEF_IMG_TASK_DISP_GRID_H) * DstPitch));
                DstYAddr += (Pitch * (i % VOUT_DEF_IMG_TASK_DISP_GRID_H));

                DstUVAddr += ((VOUT_DEF_IMG_TASK_BUF_WIDTH*VOUT_DEF_IMG_TASK_BUF_MARGIN) / 2U);
                DstUVAddr += ((Height * ((i / VOUT_DEF_IMG_TASK_DISP_GRID_H) * DstPitch)) / 2U);
                DstUVAddr += (Pitch * (i % VOUT_DEF_IMG_TASK_DISP_GRID_H));

                for (j = 0U; j < Height; j++) {
                    AmbaMisra_TypeCast(&pDst, &(DstYAddr));
                    AmbaSvcWrap_MisraMemset(pDst, 0x0, Pitch);
                    DstYAddr += DstPitch;
                }

                for (j = 0U; j < (Height/2U); j++) {
                    AmbaMisra_TypeCast(&pDst, &(DstUVAddr));
                    AmbaSvcWrap_MisraMemset(pDst, 0x80, Pitch);
                    DstUVAddr += DstPitch;
                }
            }
        }
    }

    return RetVal;
}

static void* SrcInfoTaskEntry(void* EntryArg)
{
    UINT32 RetVal;
    ULONG BufBase = 0U;
    UINT32 BufSize = 0U, ActualFlags = 0U;
    VOUT_DEF_IMG_MSG_s Msg;
    AmbaMisra_TouchUnused(EntryArg);

    RetVal = SvcBuffer_Request(SVC_BUFFER_FIXED, FMEM_ID_VOUT_DEF_IMG, &BufBase, &BufSize);

    if ((SVC_OK != RetVal) || (0U == BufBase) || (0U == BufSize)) {
        RetVal = SVC_NG;
        SvcLog_NG(SVC_LOG_VOUT_DEF_IMG_TASK, "Invalid buffer", 0U, 0U);
    }

    Msg.YAddr = BufBase;
    Msg.UVAddr = BufBase + VOUT_DEF_IMG_TASK_BUF_SIZE_Y;

    if (SVC_OK == RetVal) {
        RetVal = AmbaKAL_EventFlagGet(&g_VoutDefImgFlag, VOUT_DEF_IMG_FLAG_VOUT_TASK_READY, AMBA_KAL_FLAGS_ANY, AMBA_KAL_FLAGS_CLEAR_AUTO, &ActualFlags, 2000);
         if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_VOUT_DEF_IMG_TASK, "AmbaKAL_EventFlagGet VOUT_DEF_IMG_FLAG_SRC_TASK_READY failed! RetVal 0x%x", RetVal, 0);
        }
    }

    if (SVC_OK == RetVal) {
        RetVal = AmbaDSP_EventHandlerRegister(AMBA_DSP_EVENT_LV_YUV_DATA_RDY, VoutDefImgDataRdyHandler);
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_VOUT_DEF_IMG_TASK, "Register YUV ready event error(0x%X)", RetVal, 0U);
        }
    }

    do {
        UINT8 *pYBuf, *pUVBuf;
        ULONG YAddr, UVAddr;
        Msg.YAddr  += VOUT_DEF_IMG_TASK_BUF_SIZE;
        Msg.UVAddr += VOUT_DEF_IMG_TASK_BUF_SIZE;
        if ((Msg.UVAddr >= (BufBase + BufSize))) {
            Msg.YAddr = BufBase;
            Msg.UVAddr = BufBase + VOUT_DEF_IMG_TASK_BUF_SIZE_Y;
        }

        YAddr = Msg.YAddr;
        UVAddr = Msg.UVAddr;
        AmbaMisra_TypeCast(&pYBuf, &YAddr);
        AmbaMisra_TypeCast(&pUVBuf, &UVAddr);
        RetVal = PrepareDefImgBuf(&pYBuf, &pUVBuf);
#ifdef USING_DSP_INTERNAL_ADDR
        AmbaMisra_TypeCast(&YAddr, &pYBuf);
        AmbaMisra_TypeCast(&UVAddr, &pUVBuf);
        Msg.YAddr = YAddr;
        Msg.UVAddr = UVAddr;
#endif

        if (SVC_OK == RetVal) {
            (void)AmbaKAL_MsgQueueSend(&(g_VoutDefImgCtrl.VoutQueId), &Msg, AMBA_KAL_NO_WAIT);
        }
    } while (SVC_OK == RetVal);

    SvcLog_DBG(SVC_LOG_VOUT_DEF_IMG_TASK, "SrcInfoTaskEntry exit", 0U, 0U);

    return NULL;
}

static UINT32 SetupDefImg(void)
{
    UINT32 RetVal, ActualFlag = 0U, i = 0U;
    UINT64 RawSeq = 0ULL;
    VOUT_DEF_IMG_MSG_s Msg = {0U};
    AMBA_KAL_MSG_QUEUE_INFO_s QueueInfo;
    UINT8 IsCreateInt = 0U;

    AmbaSvcWrap_MisraMemset(&Msg,0,sizeof(VOUT_DEF_IMG_MSG_s));

    RetVal = AmbaDspInt_Open(&(g_VoutDefImgCtrl.IntPort));
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_VOUT_DEF_IMG_TASK, "Fail to take dsp int", 0U, 0U);
    } else {
        IsCreateInt = 1U;
    }

    if (SVC_OK == RetVal) {
        RetVal = AmbaKAL_MsgQueueQuery(&g_VoutDefImgCtrl.VoutQueId, &QueueInfo);
        if (KAL_ERR_NONE == RetVal) {
            if (0U == QueueInfo.NumEnqueued) {
                RetVal = AmbaKAL_MsgQueueReceive(&g_VoutDefImgCtrl.VoutQueId, &Msg, AMBA_KAL_WAIT_FOREVER);
            } else {
                for (i = 0; i < QueueInfo.NumEnqueued; i++) {
                    RetVal = AmbaKAL_MsgQueueReceive(&g_VoutDefImgCtrl.VoutQueId, &Msg, AMBA_KAL_NO_WAIT);
                }
            }
        }
    }

    if (SVC_OK == RetVal) {
        RetVal = AmbaDspInt_Take(&(g_VoutDefImgCtrl.IntPort), SVC_DSP_VOUT1_INT, &ActualFlag, AMBA_KAL_WAIT_FOREVER);
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_VOUT_DEF_IMG_TASK, "Fail to take dsp int", 0U, 0U);
        }
    }

    if (SVC_OK == RetVal) {
        g_VoutDefImgCtrl.VideoCfg.DefaultImgConfig.BaseAddrY   = Msg.YAddr;
        g_VoutDefImgCtrl.VideoCfg.DefaultImgConfig.BaseAddrUV  = Msg.UVAddr;

        if (gDebug != 0U) {
            SvcLog_DBG(SVC_LOG_VOUT_DEF_IMG_TASK, "def img y 0x%llx, uv 0x%llx", Msg.YAddr, Msg.UVAddr);
        }
        RetVal |= AmbaDSP_VoutVideoConfig(VOUT_IDX_B, &(g_VoutDefImgCtrl.VideoCfg));
        RetVal |= AmbaDSP_VoutVideoCtrl(VOUT_IDX_B, 1, 0, &RawSeq);
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_VOUT_DEF_IMG_TASK, "Vout video config/ctrl failed", 0U, 0U);
        }
    }

    if (1U == IsCreateInt) {
        RetVal = AmbaDspInt_Close(&(g_VoutDefImgCtrl.IntPort));
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_VOUT_DEF_IMG_TASK, "Fail to take dsp int", 0U, 0U);
        }
    }

    return RetVal;
}

static void* VoutDefImgTaskEntry(void* EntryArg)
{
    UINT32 RetVal, i;
    SVC_DISP_INFO_s DispInfo;
    AmbaMisra_TouchUnused(EntryArg);
    SvcDisplay_InfoGet(&DispInfo);
    for (i = 0; i < *(DispInfo.pNumDisp); i++) {
        if (VOUT_IDX_B == DispInfo.pDispCfg[i].VoutID) {
            AmbaSvcWrap_MisraMemcpy(&(g_VoutDefImgCtrl.VideoCfg), &(DispInfo.pDispCfg[i].VideoCfg), sizeof(AMBA_DSP_VOUT_VIDEO_CFG_s));
            break;
        }
    }
    g_VoutDefImgCtrl.VideoCfg.VideoSource                  = VOUT_SOURCE_DEFAULT_IMAGE;
    g_VoutDefImgCtrl.VideoCfg.DefaultImgConfig.Pitch       = VOUT_DEF_IMG_TASK_BUF_WIDTH;
    g_VoutDefImgCtrl.VideoCfg.DefaultImgConfig.DataFormat  = AMBA_DSP_YUV420;
    g_VoutDefImgCtrl.VideoCfg.DefaultImgConfig.FieldRepeat = 0U;

    RetVal = AmbaKAL_EventFlagSet(&g_VoutDefImgFlag, VOUT_DEF_IMG_FLAG_VOUT_TASK_READY);
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_VOUT_DEF_IMG_TASK, "AmbaKAL_EventFlagSet VOUT_DEF_IMG_FLAG_VOUT_TASK_READY failed! RetVal 0x%x", RetVal, 0);
    }

    while (SVC_OK == RetVal) {
        RetVal = SetupDefImg();
    }

    SvcLog_DBG(SVC_LOG_VOUT_DEF_IMG_TASK, "VoutDefImgTaskEntry exit", 0U, 0U);

    return NULL;
}

UINT32 SvcVoutDefImgTask_Control(UINT32 DispAlt)
{
    g_VoutDefImgCtrl.DispAlt = DispAlt;
    SvcLog_OK(SVC_LOG_VOUT_DEF_IMG_TASK, "Change default vout display to %d", DispAlt, 0U);
    return SVC_OK;
}

/**
* query default vout task requested memory size
* @param [out] pMemSize - pointer to output memory size
* @return 0-OK, 1-NG
*/
UINT32 SvcVoutDefImgTask_MemQry(UINT32 *pMemSize)
{
    *pMemSize = VOUT_DEF_IMG_TASK_BUF_SIZE * VOUT_DEF_IMG_TASK_BUF_DEPTH;
    SvcLog_OK(SVC_LOG_VOUT_DEF_IMG_TASK, "Query the default vout task memory size 0x%x", *pMemSize, 0U);
    return SVC_OK;
}

/**
* Create task to copy yuv to default vout buffer
* @return 0-OK, 1-NG
*/
UINT32 SvcVoutDefImgTask_Init(void)
{
    UINT32 RetVal;

    static SVC_TASK_CTRL_s SrcInfoTaskCtrl GNU_SECTION_NOZEROINIT;
    static SVC_TASK_CTRL_s VoutDefImgTaskCtrl GNU_SECTION_NOZEROINIT;
    static UINT8 SrcInfoTaskStack[VOUT_DEF_IMG_TASK_STACK_SIZE] GNU_SECTION_NOZEROINIT;
    static UINT8 VoutDefImgTaskStack[VOUT_DEF_IMG_TASK_STACK_SIZE] GNU_SECTION_NOZEROINIT;
    static char SrcQueName[] = "SrcInfoMsgQue";
    static char VoutQueName[] = "VoutDefImgMsgQue";
    static SRC_INFO_MSG_s SrcInfoQue[VOUT_DEF_IMG_MSG_QUEUE_SIZE] GNU_SECTION_NOZEROINIT;
    static VOUT_DEF_IMG_MSG_s VoutDefImgQue[VOUT_DEF_IMG_MSG_QUEUE_SIZE] GNU_SECTION_NOZEROINIT;

    AmbaSvcWrap_MisraMemset(&g_VoutDefImgCtrl, 0, sizeof(VOUT_DEF_IMG_CTRL_s));
    g_VoutDefImgCtrl.DispAlt = 0U;

    SrcInfoTaskCtrl.Priority   = 150U;
    SrcInfoTaskCtrl.EntryFunc  = SrcInfoTaskEntry;
    SrcInfoTaskCtrl.EntryArg   = 0;
    SrcInfoTaskCtrl.pStackBase = SrcInfoTaskStack;
    SrcInfoTaskCtrl.StackSize  = VOUT_DEF_IMG_TASK_STACK_SIZE;
    SrcInfoTaskCtrl.CpuBits    = 0x02U;

    VoutDefImgTaskCtrl.Priority   = 70U;
    VoutDefImgTaskCtrl.EntryFunc  = VoutDefImgTaskEntry;
    VoutDefImgTaskCtrl.EntryArg   = 0;
    VoutDefImgTaskCtrl.pStackBase = VoutDefImgTaskStack;
    VoutDefImgTaskCtrl.StackSize  = VOUT_DEF_IMG_TASK_STACK_SIZE;
    VoutDefImgTaskCtrl.CpuBits    = 0x02U;

    RetVal = AmbaKAL_MsgQueueCreate(&(g_VoutDefImgCtrl.SrcQueId),
                                    SrcQueName,
                                    sizeof(SRC_INFO_MSG_s),
                                    &SrcInfoQue[0],
                                    VOUT_DEF_IMG_MSG_QUEUE_SIZE * sizeof(SRC_INFO_MSG_s));
    if (KAL_ERR_NONE != RetVal) {
        SvcLog_NG(SVC_LOG_VOUT_DEF_IMG_TASK, "Create SrcInfoQue error(0x%X)", RetVal, 0U);
        RetVal =  SVC_NG;
    }

    if (SVC_OK == RetVal) {
        RetVal = AmbaKAL_EventFlagCreate(&g_VoutDefImgFlag, NULL);
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_VOUT_DEF_IMG_TASK, "call AmbaKAL_EventFlagCreate() failed, can not create event flag, RetVal:0x%x", RetVal, 0U);
        } else {
            RetVal = AmbaKAL_EventFlagClear(&g_VoutDefImgFlag, 0xFFFFFFFFU);
            if (SVC_OK != RetVal) {
                SvcLog_NG(SVC_LOG_VOUT_DEF_IMG_TASK, "call AmbaKAL_EventFlagClear() failed, can not clear event flag, RetVal:0x%x", RetVal, 0U);
            }
        }
    }

    if (SVC_OK == RetVal) {
        RetVal = AmbaKAL_MsgQueueCreate(&(g_VoutDefImgCtrl.VoutQueId),
                                        VoutQueName,
                                        sizeof(VOUT_DEF_IMG_MSG_s),
                                        &VoutDefImgQue[0],
                                        VOUT_DEF_IMG_MSG_QUEUE_SIZE * sizeof(VOUT_DEF_IMG_MSG_s));
        if (KAL_ERR_NONE != RetVal) {
            SvcLog_NG(SVC_LOG_VOUT_DEF_IMG_TASK, "Create VoutDefImgQue error(0x%X)", RetVal, 0U);
            RetVal =  SVC_NG;
        }
    }

    if (SVC_OK == RetVal) {
        RetVal = SvcTask_Create("SvcSrcInfoTask", &SrcInfoTaskCtrl);
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_VOUT_DEF_IMG_TASK, "Source Info task isn't created", 0U, 0U);
        }
    }

    if (SVC_OK == RetVal) {
        RetVal = SvcTask_Create("SvcVoutDefImgTask", &VoutDefImgTaskCtrl);
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_VOUT_DEF_IMG_TASK, "Vout default image task isn't created", 0U, 0U);
        }
    }

    if (SVC_OK == RetVal) {
        SvcLog_OK(SVC_LOG_VOUT_DEF_IMG_TASK, "Init vout default image task success", 0U, 0U);
    }

    return RetVal;
}

void SvcVoutDefImgTask_Debug(void)
{
    gDebug = 1U;
}



