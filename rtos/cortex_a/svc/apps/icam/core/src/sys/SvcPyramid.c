/**
 *  @file SvcPyramid.c
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
 *  @details svc pyramid related function
 *
 */
#include "AmbaTypes.h"
#include "AmbaKAL.h"
#include "AmbaMisraFix.h"
#include "AmbaUtility.h"
#include "AmbaWrap.h"
#include "AmbaGDMA.h"
#include "AmbaCache.h"
#include "AmbaPrint.h"

/* ssp */
#include "AmbaDSP_EventInfo.h"
#include "AmbaDSP_Event.h"
#include "AmbaDSP_Liveview.h"

/* svc-framework */
#include "SvcErrCode.h"
#include "SvcLog.h"
#include "SvcWrap.h"
#include "SvcTask.h"
#include "SvcPlat.h"

/* svc-shared */
#include "SvcResCfg.h"

#include "SvcPyramid.h"

#define PYRAMID_CP_TASK_STACK_SIZE  (0x4000)
#define PYRAMID_CP_TASK_PRIORITY    (50U)

#define PYRAMID_CP_TASK_TIMEOUT     (5000U) /* 5 seconds for time-out */

#define PYRAMID_CP_YUV_WIDTH        (3840U)
#define PYRAMID_CP_YUV_HEIGHT       (2160U)
#define PYRAMID_CP_YUV_SIZE         (PYRAMID_CP_YUV_WIDTH * PYRAMID_CP_YUV_HEIGHT * 2U)

#define PYRAMID_CP_EVENT_IDLE       (0x1U)
#define PYRAMID_CP_EVENT_DATA_READY (0x2U)
#define PYRAMID_CP_EVENT_COPY_DONE  (0x4U)

#define SVC_LOG_PYRAMID_CP          "PYRAMID_CP"

static AMBA_KAL_EVENT_FLAG_t Pyramid_DataCpEvent;

static AMBA_DSP_PYMD_DATA_RDY_s Pyramid_DataCpEventInfo;

static UINT32 Pyramid_DataCpFov = 0U;
static UINT32 Pyramid_Hier = 0U;
static UINT32 Pyramid_Flag = 0U;

static UINT8  Pyramid_YuvBuffer[PYRAMID_CP_YUV_SIZE] GNU_ALIGNED_CACHESAFE GNU_SECTION_NOZEROINIT;

static SVC_PYRAMID_CP_INFO_s Pyramid_CpInfo = {0};

static UINT32 Pyramid_DspEventData(const void *pEventInfo);
static void* Pyramid_DataCpEntry(void* EntryArg);
extern UINT32 Pyramid_12BCompactTo8B(UINT8 *pSrcBuf, UINT32 SrcWidth, UINT32 SrcHeight, UINT32 SrcPitch, UINT8 *pDstBuf, UINT32 DstPitch);

/**
* info get of pyramid data
* @param [in] pInfo info block of pyramid
* @return 0-OK, 1-NG
*/
UINT32 SvcPyramid_InfoGet(SVC_PYRAMID_CP_INFO_s *pInfo)
{
    UINT32 RetVal = SVC_OK;

    if (pInfo == NULL) {
        SvcLog_NG(SVC_LOG_PYRAMID_CP, "Input Info should not be null", 0U, 0U);
        RetVal = SVC_NG;
    } else {
        Pyramid_CpInfo.pBufAddr = Pyramid_YuvBuffer;
        Pyramid_CpInfo.BufSize  = PYRAMID_CP_YUV_SIZE;
        RetVal = AmbaWrap_memcpy(pInfo, &Pyramid_CpInfo, sizeof(SVC_PYRAMID_CP_INFO_s));
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_PYRAMID_CP, "Pyramid info copy failed %d", RetVal, 0U);
        }
    }

    return RetVal;
}

/**
* pyramid data store
* @param [in] FovIdx desired index of fov
* @param [in] Hier desired hier
* @return 0-OK, 1-NG
*/
UINT32 SvcPyramid_Save(UINT32 FovIdx, UINT32 Hier)
{
    UINT32 RetVal = SVC_OK;
    UINT32 FovIdxs[AMBA_DSP_MAX_VIEWZONE_NUM], FovNum;

    static SVC_TASK_CTRL_s Pyramid_TaskCtrl;
    static UINT8 PyramidCp_TaskStack[PYRAMID_CP_TASK_STACK_SIZE] GNU_SECTION_NOZEROINIT;
    static char  PyramidCp_TaskName[16] = "Pyramid_CpTask";
    static char  PyramidCp_EventName[32] = "Pyramid_DataCpEvent";


    AmbaMisra_TouchUnused(PyramidCp_TaskName);

    RetVal = AmbaWrap_memset(&Pyramid_CpInfo, 0, sizeof(SVC_PYRAMID_CP_INFO_s));
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_PYRAMID_CP, "Pyramid_CpInfo memset failed %d", RetVal, 0U);
    }

    RetVal = AmbaWrap_memset(FovIdxs, 0, sizeof(FovIdxs));
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_PYRAMID_CP, "FovIdxs memset failed %d", RetVal, 0U);
    }

    RetVal = SvcResCfg_GetFovIdxs(FovIdxs, &FovNum);
    if (SVC_OK == RetVal) {
        UINT32 i;
        UINT32 FovExist = 0U;
        for (i = 0U; i < FovNum; i++) {
            if (FovIdx == FovIdxs[i]) {
                FovExist = 1U;
                break;
            }
        }
        if (FovExist == 1U) {
            const SVC_RES_CFG_s *pCfg = SvcResCfg_Get();
            if ((pCfg->FovCfg[FovIdx].PyramidBits & ((UINT32) 1U << Hier)) > 0U) {
                Pyramid_DataCpFov = FovIdx;
                Pyramid_Hier = Hier;
                Pyramid_Flag = pCfg->FovCfg[FovIdx].PyramidFlag;
                SvcLog_OK(SVC_LOG_PYRAMID_CP, "SvcPyramid_Save FovIdx %d, Hier %d", FovIdx, Hier);
                RetVal = SVC_OK;
            } else {
                SvcLog_NG(SVC_LOG_PYRAMID_CP, "Invalid Hier. Valid FovIdx %d Hier Bits 0x%x", FovIdx, pCfg->FovCfg[FovIdx].PyramidBits);
                RetVal = SVC_NG;
            }
        } else {
            SvcLog_NG(SVC_LOG_PYRAMID_CP, "Invalid FovIdx %d", FovIdx, 0U);
            RetVal = SVC_NG;
        }
    } else {
        SvcLog_NG(SVC_LOG_PYRAMID_CP, "Get ResCfg failed", 0U, 0U);
    }

    /* Create Pyramid Data Copy Event */
    if (SVC_OK == RetVal) {
        RetVal = AmbaKAL_EventFlagCreate(&Pyramid_DataCpEvent, PyramidCp_EventName);
        if (SVC_OK == RetVal) {
            RetVal = AmbaKAL_EventFlagSet(&Pyramid_DataCpEvent, PYRAMID_CP_EVENT_IDLE);
            if (SVC_OK != RetVal) {
                SvcLog_NG(SVC_LOG_PYRAMID_CP, "Pyramid idle event set failed", 0U, 0U);
            }
        } else {
            SvcLog_NG(SVC_LOG_PYRAMID_CP, "Pyramid copy event create failed", 0U, 0U);
        }
    }

    /* Create task for Pyramid Data Copy */
    if (SVC_OK == RetVal) {
        Pyramid_TaskCtrl.Priority   = PYRAMID_CP_TASK_PRIORITY;
        Pyramid_TaskCtrl.EntryFunc  = Pyramid_DataCpEntry;
        Pyramid_TaskCtrl.EntryArg   = 0U;
        Pyramid_TaskCtrl.pStackBase = PyramidCp_TaskStack;
        Pyramid_TaskCtrl.StackSize  = PYRAMID_CP_TASK_STACK_SIZE;
        Pyramid_TaskCtrl.CpuBits    = 0x1U;

        RetVal = SvcTask_Create(PyramidCp_TaskName, &Pyramid_TaskCtrl);
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_PYRAMID_CP, "Pyramid_CpTask err. Task create failed %d", RetVal, 0U);
        }
    }

    /* Register PYRAMID DSP Event */
    if (SVC_OK == RetVal) {
        RetVal = AmbaDSP_EventHandlerRegister(AMBA_DSP_EVENT_LV_PYRAMID_RDY, Pyramid_DspEventData);
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_PYRAMID_CP, "SvcDirtDetect_Init err. DSP Event register failed %d", RetVal, 0U);
        }
    }

    /* Wait Data Ready */
    if (SVC_OK == RetVal) {
        UINT32 ActualFlag = 0U;
        RetVal = AmbaKAL_EventFlagGet(&Pyramid_DataCpEvent, PYRAMID_CP_EVENT_DATA_READY, AMBA_KAL_FLAGS_ALL, AMBA_KAL_FLAGS_CLEAR_NONE, &ActualFlag, PYRAMID_CP_TASK_TIMEOUT);
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_PYRAMID_CP, "Pyramid Data timeout", 0U, 0U);
        }
    }

    /* Unregister DSP Event */
    if (SVC_OK == RetVal) {
        RetVal = AmbaDSP_EventHandlerUnRegister(AMBA_DSP_EVENT_LV_PYRAMID_RDY, Pyramid_DspEventData);
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_PYRAMID_CP, "DSP Event unregister failed", 0U, 0U);
        }
    }

    /* Wait Data Copy done */
    if (SVC_OK == RetVal) {
        UINT32 ActualFlag = 0U;
        RetVal = AmbaKAL_EventFlagGet(&Pyramid_DataCpEvent, PYRAMID_CP_EVENT_COPY_DONE, AMBA_KAL_FLAGS_ALL, AMBA_KAL_FLAGS_CLEAR_NONE, &ActualFlag, PYRAMID_CP_TASK_TIMEOUT);
        if (SVC_OK == RetVal) {
            if ((ActualFlag & PYRAMID_CP_EVENT_COPY_DONE) > 0U) {
                /* Do nothing */
            } else {
                SvcLog_NG(SVC_LOG_PYRAMID_CP, "Unknown status", 0U, 0U);
            }
        } else {
            SvcLog_NG(SVC_LOG_PYRAMID_CP, "Pyramid Copy timeout", 0U, 0U);
            RetVal = SVC_OK;
        }
    }

    /* Destroy Pyramid Data Copy task */
    if (SVC_OK == RetVal) {
        RetVal = SvcTask_Destroy(&Pyramid_TaskCtrl);
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_PYRAMID_CP, "Task delete failed", 0U, 0U);
        }
    }

    /* Delete Data Copy Event */
    if (SVC_OK == RetVal) {
        RetVal = AmbaKAL_EventFlagDelete(&Pyramid_DataCpEvent);
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_PYRAMID_CP, "Pyramid Cp Event delete failed", 0U, 0U);
        }
    }

    if (SVC_OK == RetVal) {
        SvcLog_OK(SVC_LOG_PYRAMID_CP, "SvcPyramid_Save Done", 0U, 0U);
    }

    return RetVal;
}

static UINT32 Pyramid_DspEventData(const void *pEventInfo)
{
    UINT32 RetVal;
    UINT32 ActualFlag = 0U;

    const AMBA_DSP_PYMD_DATA_RDY_s *pPyramidInfo;

    /* Cast to correct Type */
    AmbaMisra_TypeCast(&pPyramidInfo, &pEventInfo);

    RetVal = AmbaKAL_EventFlagGet(&Pyramid_DataCpEvent, PYRAMID_CP_EVENT_IDLE, AMBA_KAL_FLAGS_ALL, AMBA_KAL_FLAGS_CLEAR_AUTO, &ActualFlag, AMBA_KAL_NO_WAIT);
    if (SVC_OK == RetVal) {
        /* DirtDetect_Entry is not busy */
        if (pPyramidInfo->ViewZoneId == Pyramid_DataCpFov) {

            Pyramid_DataCpEventInfo = *pPyramidInfo;

            RetVal = AmbaKAL_EventFlagSet(&Pyramid_DataCpEvent, PYRAMID_CP_EVENT_DATA_READY);
            if (SVC_OK != RetVal) {
                SvcLog_NG(SVC_LOG_PYRAMID_CP, "DirtDetect_PyramidReady err. Event flag set failed %d", RetVal, 0U);
            }
        }
    } else {
        /* Do nothing */
    }

    return SVC_OK;
}

static void* Pyramid_DataCpEntry(void* EntryArg)
{
    UINT32 RetVal = SVC_OK;
    UINT32 ActualFlag = 0U;

    AmbaMisra_TouchUnused(EntryArg);

    RetVal = AmbaKAL_EventFlagGet(&Pyramid_DataCpEvent, PYRAMID_CP_EVENT_DATA_READY, AMBA_KAL_FLAGS_ALL, AMBA_KAL_FLAGS_CLEAR_NONE, &ActualFlag, PYRAMID_CP_TASK_TIMEOUT);
    if (SVC_OK == RetVal) {
        if ((ActualFlag & PYRAMID_CP_EVENT_DATA_READY) > 0U) {
            UINT32 YWidth = 0U, UVWidth = 0U;
            UINT32 YHeight = 0U, UVHeight = 0U;
            UINT32 Pitch = 0U;
            const AMBA_DSP_YUV_IMG_BUF_s *pYuvBuf = &Pyramid_DataCpEventInfo.YuvBuf[Pyramid_Hier];

            AmbaPrint_PrintUInt5("Pyramid DataFmt: %d, %d x %d, P: %d, Flag: %d",
                pYuvBuf->DataFmt, pYuvBuf->Window.Width, pYuvBuf->Window.Height, pYuvBuf->Pitch, Pyramid_Flag);
            SvcWrap_PrintUL("Pyramid Y 0x%x, UV 0x%x", pYuvBuf->BaseAddrY, pYuvBuf->BaseAddrUV, 0U, 0U, 0U);

            if ((Pyramid_Flag & SVC_RES_PYRAMID_12BITS_Y) > 0U) {
                YWidth = ((UINT32)pYuvBuf->Window.Width * 3U) >> 1U;
                UVWidth = ((UINT32)pYuvBuf->Window.Width * 3U) >> 1U;
                YHeight = pYuvBuf->Window.Height;
                UVHeight = ((UINT32) pYuvBuf->Window.Height >> 1U);
                Pitch = pYuvBuf->Pitch;
            } else if (pYuvBuf->DataFmt == AMBA_DSP_YUV420) {
                YWidth = pYuvBuf->Window.Width;
                UVWidth = pYuvBuf->Window.Width;
                YHeight = pYuvBuf->Window.Height;
                UVHeight = ((UINT32) pYuvBuf->Window.Height >> 1U);
                Pitch = pYuvBuf->Pitch;
            } else if (pYuvBuf->DataFmt == AMBA_DSP_YUV422) {
                YWidth = pYuvBuf->Window.Width;
                UVWidth = pYuvBuf->Window.Width;
                YHeight = pYuvBuf->Window.Height;
                UVHeight = pYuvBuf->Window.Height;
                Pitch = pYuvBuf->Pitch;
            } else {
                SvcLog_NG(SVC_LOG_PYRAMID_CP, "DirtDetect_GetYuvInfo err. Not support DataFmt", 0U, 0U);
                RetVal = SVC_NG;
            }

            if (SVC_OK == RetVal) {
                UINT8 *pBufferY;
                UINT8 *pBufferUV;
                UINT8 *pDstBufferY = Pyramid_YuvBuffer;
                UINT8 *pDstBufferUV = &Pyramid_YuvBuffer[YHeight * YWidth];
                ULONG Pyramid_YuvBufferAddr;

                AmbaMisra_TypeCast(&pBufferY, &pYuvBuf->BaseAddrY);
                AmbaMisra_TypeCast(&pBufferUV, &pYuvBuf->BaseAddrUV);

                RetVal = AmbaWrap_memset(Pyramid_YuvBuffer, 0, sizeof(Pyramid_YuvBuffer));
                if (SVC_OK != RetVal) {
                    SvcLog_NG(SVC_LOG_PYRAMID_CP, "DirtDetect_GetYuvInfo err. memset failed %d", RetVal, 0U);
                }

                /* GDMA copy */
                if (SVC_OK == RetVal) {
                    AMBA_GDMA_BLOCK_s GdmaBlockY = {
                        .pSrcImg = pBufferY,
                        .pDstImg = pDstBufferY,
                        .SrcRowStride = Pitch,
                        .DstRowStride = YWidth,
                        .BltWidth  = YWidth,
                        .BltHeight = YHeight,
                        .PixelFormat = AMBA_GDMA_8_BIT,
                    };

                    AMBA_GDMA_BLOCK_s GdmaBlockUV = {
                        .pSrcImg = pBufferUV,
                        .pDstImg = pDstBufferUV,
                        .SrcRowStride = Pitch,
                        .DstRowStride = UVWidth,
                        .BltWidth  = UVWidth,
                        .BltHeight = UVHeight,
                        .PixelFormat = AMBA_GDMA_8_BIT,
                    };

                    SvcLog_OK(SVC_LOG_PYRAMID_CP, "GDMA Copy start CapSequence", 0U, 0U);

                    if (NULL != pBufferY) {
                        AmbaMisra_TypeCast(&Pyramid_YuvBufferAddr, &pDstBufferY);
                        RetVal = SvcPlat_CacheInvalidate(Pyramid_YuvBufferAddr, sizeof(Pyramid_YuvBuffer));
                        if (SVC_OK != RetVal) {
                            SvcLog_NG(SVC_LOG_PYRAMID_CP, "DirtDetect_GetYuvInfo err. Cache clean err %d", RetVal, 0U);
                        }
                        RetVal = AmbaGDMA_BlockCopy(&GdmaBlockY, NULL, 0U, 1000U /* Wait 1000ms */);
                        if (SVC_OK != RetVal) {
                            SvcLog_NG(SVC_LOG_PYRAMID_CP, "DirtDetect_GetYuvInfo err. Gdma copy Y failed %d", RetVal, 0U);
                        }
                        RetVal = AmbaGDMA_WaitAllCompletion(1000U);
                        if (SVC_OK != RetVal) {
                            SvcLog_NG(SVC_LOG_PYRAMID_CP, "DirtDetect_GetYuvInfo err. Gdma copy Y wait err %d", RetVal, 0U);
                        }
                    }

                    if (NULL != pBufferUV) {
                        RetVal = AmbaGDMA_BlockCopy(&GdmaBlockUV, NULL, 1U, 1000U /* wait 500ms */);
                        if (SVC_OK != RetVal) {
                            SvcLog_NG(SVC_LOG_PYRAMID_CP, "DirtDetect_GetYuvInfo err. Gdma copy UV failed %d", RetVal, 0U);
                        }
                        RetVal = AmbaGDMA_WaitAllCompletion(1000U);
                        if (SVC_OK != RetVal) {
                            SvcLog_NG(SVC_LOG_PYRAMID_CP, "DirtDetect_GetYuvInfo err. Gdma copy UV wait err %d", RetVal, 0U);
                        }
                    }
                }

                Pyramid_CpInfo.FovIdx   = Pyramid_DataCpFov;
                Pyramid_CpInfo.Hier     = Pyramid_Hier;
                Pyramid_CpInfo.Flag     = Pyramid_Flag;
                Pyramid_CpInfo.pYAddr   = pDstBufferY;
                Pyramid_CpInfo.YWidth   = YWidth;
                Pyramid_CpInfo.YHeight  = YHeight;
                Pyramid_CpInfo.pUVAddr  = pDstBufferUV;
                Pyramid_CpInfo.UVWidth  = UVWidth;
                Pyramid_CpInfo.UVHeight = UVHeight;
            }

            SvcLog_OK(SVC_LOG_PYRAMID_CP, "Pyramid Copy done", 0U, 0U);

            RetVal = AmbaKAL_EventFlagSet(&Pyramid_DataCpEvent, PYRAMID_CP_EVENT_COPY_DONE);
            if (SVC_OK != RetVal) {
                SvcLog_NG(SVC_LOG_PYRAMID_CP, "Pyramid Event Copy Done Flag set failed", 0U, 0U);
            }
        }
    } else {
        SvcLog_NG(SVC_LOG_PYRAMID_CP, "Pyramid Event Data Ready Flag get time out", 0U, 0U);
    }

    return NULL;
}

