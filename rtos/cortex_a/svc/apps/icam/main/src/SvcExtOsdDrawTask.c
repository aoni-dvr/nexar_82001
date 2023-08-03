/**
 *  @file SvcExtOsdDrawTask.c
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
 *  @details svc segmentation draw task
 *
 */

#include "AmbaTypes.h"
#include "AmbaKAL.h"
#include "AmbaWrap.h"
#include "AmbaUtility.h"

/* ssp */
#include "AmbaDSP_Capability.h"
#include "AmbaDSP_EventInfo.h"
#include "AmbaDSP_VOUT.h"
#include "AmbaDSP_Liveview.h"

/* svc-framework */
#include "SvcErrCode.h"
#include "SvcLog.h"
#include "SvcWrap.h"

/* svc-shared */
#include "SvcGui.h"
#include "SvcOsd.h"
#include "SvcResCfg.h"
#include "SvcCvFlow.h"
#include "SvcCvFlow_Comm.h"
#include "SvcCvImgUtil.h"

/* svc-icam */
#include "SvcCvAppDef.h"
#include "SvcExtOsdDrawTask.h"

#define SVC_LOG_EXTOSD_TASK     "EXT_OSD_DRAW_TASK"
#define SVC_EXT_OSD_GUI_LEVEL           (0U)

#define SVC_CV_FLOW_CHAN_MAX            (16U)


#define OSD_1_1_RATIO           (0)
#define OSD_3_1_RATIO           (1)

#define MAX_OSD_VIEWZONE_NUM     AMBA_DSP_MAX_VIEWZONE_NUM

typedef struct {
    UINT32 XStart;
    UINT32 YStart;
    UINT32 XEnd;
    UINT32 YEnd;
    UINT32 Havedata;
} OSD_VIDEZONE_s;

typedef struct {
    UINT32 RescaleMode;
    UINT32 CvFlowNum;
    UINT32 CvFlowBits;
    SVC_BIT_MAP_RESCALE_REPT_s Cfg;
    OSD_VIDEZONE_s Zone[MAX_OSD_VIEWZONE_NUM];
} SVC_EXT_OSD_BUF_CTRL_s;

static UINT32 DisplayMode = OSD_NORMAL_MODE;
static UINT32 TargetVout = VOUT_IDX_B;

/* The Cv Flow registered ID */
static UINT32 CvFlowRegisterID[SVC_CV_FLOW_CHAN_MAX] = {0};
static UINT32 HaveExtData[SVC_RES_CV_FLOW_NUM] GNU_SECTION_NOZEROINIT;
static UINT8  *SrcBufPtr[SVC_RES_CV_FLOW_NUM];
static UINT32 ExtOsdDrawUpdate = 0U;

static AMBA_KAL_MUTEX_t ExtOsdMutex GNU_SECTION_NOZEROINIT;
static void SvcExtOsd_MutexCreate(void)
{
    if (KAL_ERR_NONE != AmbaKAL_MutexCreate(&ExtOsdMutex, NULL)) {
        SvcLog_NG(SVC_LOG_EXTOSD_TASK, "MutexCreate error", 0U, 0U);
    }
}

static void SvcExtOsd_MutexTake(void)
{
    if (KAL_ERR_NONE != AmbaKAL_MutexTake(&ExtOsdMutex, 5000)) {
        SvcLog_NG(SVC_LOG_EXTOSD_TASK, "SvcExtOsd_MutexTake: timeout", 0U, 0U);
    }
}

static void SvcExtOsd_MutexGive(void)
{
    if (KAL_ERR_NONE != AmbaKAL_MutexGive(&ExtOsdMutex)) {
        SvcLog_NG(SVC_LOG_EXTOSD_TASK, "SvcExtOsd_MutexGive: error", 0U, 0U);
    }
}


/**
 *  Set Mode
 *  @return error code
 */
UINT32 SvcExtOsd_SetMode(UINT32 Mode)
{
    SvcExtOsd_MutexTake();
    DisplayMode = Mode;
    SvcExtOsd_MutexGive();

    return SVC_OK;
}

static void SvcExtOsdDraw(UINT32 VoutIdx, UINT32 Level)
{
    static UINT8 PixelMap_3_1[3] = {0, 1, 0};
    static UINT8 LineMap_3_1[3]  = {0, 1, 0};
    static UINT8 PixelMap_1_1[3] = {1, 1, 1};
    static UINT8 LineMap_1_1[3]  = {1, 1, 1};
    static const SVC_EXT_OSD_BUF_CTRL_s g_OSDView[] = {
    {
      .RescaleMode = OSD_1_1_RATIO,  /* OSD_NORMAL_MODE */
      .CvFlowNum = 1,
      .CvFlowBits = 0x1U,
      .Cfg = {
          .pSrc = NULL,
          .SrcWidth = 960U,
          .SrcHeight = 540U,
          .SrcPitch = 960U,
          .SrcStartY = 0U,
          .pDst = NULL,
          .DstWidth = 960U,
          .DstHeight = 540U,
          .DstPitch = 960U,
          .HorizantalMapLen = 3U,
          .pHorizantalMap = PixelMap_1_1,
          .VerticalMapLen = 3U,
          .pVerticalMap = LineMap_1_1,
          .ColorOffset = 0U,
          .pClut = NULL,
          .NumColor = 0U,
      },
      .Zone = {
          [0] = {
              .XStart = 0U,
              .YStart = 0U,
              .XEnd = 960U,
              .YEnd = 540U,
          },
          [1] = {0},
          [2] = {0},
          [3] = {0},
          [4] = {0},
          [5] = {0},
          [6] = {0},
          [7] = {0},
          [8] = {0},
          [9] = {0},
          [10] = {0},
          [11] = {0},
          [12] = {0},
          [13] = {0},
          [14] = {0},
          [15] = {0},
      }
    },
    {
      .RescaleMode = OSD_3_1_RATIO,  /* OSD_DMS_DEMO_MODE */
      .CvFlowNum = 2,
      .CvFlowBits = 0x3U,
      .Cfg = {
          .SrcWidth = 960U,
          .SrcHeight = 540U,
          .SrcPitch = 960U,
          .SrcStartY = 0U,
          .pDst = NULL,
          .DstWidth = 320U,
          .DstHeight = 180U,
          .DstPitch = 320U,
          .HorizantalMapLen = 3U,
          .pHorizantalMap = PixelMap_3_1,
          .VerticalMapLen = 3U,
          .pVerticalMap = LineMap_3_1,
          .ColorOffset = 0U,
          .pClut = NULL,
          .NumColor = 0U,
      },
      .Zone = {
          [0] = {
              .XStart = 0U,
              .YStart = 360U,
              .XEnd = 320U,
              .YEnd = 540U,
          },
          [1] = {
              .XStart = 320U,
              .YStart = 360U,
              .XEnd = 640U,
              .YEnd = 540U,
          },
          [2] = {0},
          [3] = {0},
          [4] = {0},
          [5] = {0},
          [6] = {0},
          [7] = {0},
          [8] = {0},
          [9] = {0},
          [10] = {0},
          [11] = {0},
          [12] = {0},
          [13] = {0},
          [14] = {0},
          [15] = {0},
      }
    },
    {
      .RescaleMode = OSD_1_1_RATIO,  /* OSD_FHD_MODE */
      .CvFlowNum = 1,
      .CvFlowBits = 0x1U,
      .Cfg = {
          .pSrc = NULL,
          .SrcWidth = 1920U,
          .SrcHeight = 1080U,
          .SrcPitch = 1920U,
          .SrcStartY = 0U,
          .pDst = NULL,
          .DstWidth = 1920U,
          .DstHeight = 1080U,
          .DstPitch = 1920U,
          .HorizantalMapLen = 3U,
          .pHorizantalMap = PixelMap_1_1,
          .VerticalMapLen = 3U,
          .pVerticalMap = LineMap_1_1,
          .ColorOffset = 0U,
          .pClut = NULL,
          .NumColor = 0U,
      },
      .Zone = {
          [0] = {
              .XStart = 0U,
              .YStart = 0U,
              .XEnd = 1920U,
              .YEnd = 1080U,
          },
          [1] = {0},
          [2] = {0},
          [3] = {0},
          [4] = {0},
          [5] = {0},
          [6] = {0},
          [7] = {0},
          [8] = {0},
          [9] = {0},
          [10] = {0},
          [11] = {0},
          [12] = {0},
          [13] = {0},
          [14] = {0},
          [15] = {0},
      }
    },
    };
    UINT32 i;
    UINT32 XStart, YStart, XEnd, YEnd;

    SvcExtOsd_MutexTake();
    AmbaMisra_TouchUnused(&VoutIdx);
    AmbaMisra_TouchUnused(&Level);
    AmbaMisra_TouchUnused(PixelMap_3_1);
    AmbaMisra_TouchUnused(LineMap_3_1);
    AmbaMisra_TouchUnused(PixelMap_1_1);
    AmbaMisra_TouchUnused(LineMap_1_1);

    for (i = 0U; i < g_OSDView[DisplayMode].CvFlowNum; i++) {
        if ((g_OSDView[DisplayMode].CvFlowBits & ((UINT32) 1U << i)) > 0U) {
            if (1U == HaveExtData[i]) {
                XStart = g_OSDView[DisplayMode].Zone[i].XStart;
                YStart = g_OSDView[DisplayMode].Zone[i].YStart;
                XEnd = g_OSDView[DisplayMode].Zone[i].XEnd;
                YEnd = g_OSDView[DisplayMode].Zone[i].YEnd;
                (void)SvcOsd_DrawBin(TargetVout, XStart, YStart, XEnd, YEnd, SrcBufPtr[i]);
            }
        }
    }

    SvcExtOsd_MutexGive();
}

static void SvcExtOsdDrawUpdate(UINT32 VoutIdx, UINT32 Level, UINT32 *pUpdate)
{
    AmbaMisra_TouchUnused(&VoutIdx);
    AmbaMisra_TouchUnused(&Level);

    if (ExtOsdDrawUpdate == 1U) {
        ExtOsdDrawUpdate = 0U;
        *pUpdate = 1U;
    } else {
        *pUpdate = 0U;
    }
}

static void SvcExtOsdDraw_DetectionResultCallback(UINT32 Chan, UINT32 CvType, void *pDetResMsg)
{
    const ULONG *pMsg;
    const SVC_CV_OUT_BUF_s *pOutBuf;
    const SVC_RES_CFG_s *pResCfg = SvcResCfg_Get();
    UINT32 i;
    UINT32 CvFlowNum = pResCfg->CvFlowNum;
    UINT32 CvFlowBits = pResCfg->CvFlowBits;
    const SVC_CV_FLOW_s *pCvFlow = pResCfg->CvFlow;

    AmbaMisra_TouchUnused(&Chan);
    AmbaMisra_TouchUnused(&CvType);
    AmbaMisra_TypeCast(&pMsg, &pDetResMsg);

    if (*pMsg == SVC_CV_OSD_BUFFER) {
        SvcExtOsd_MutexTake();
        (void) pDetResMsg;
        AmbaMisra_TypeCast(&pOutBuf, &pDetResMsg);
        for (i = 0U; i < CvFlowNum; i++) {
            if ((CvFlowBits & ((UINT32) 1U << i)) > 0U) {
                if (pCvFlow[i].InputCfg.Input[0].StrmId == pOutBuf->Source) {
                    AmbaMisra_TypeCast(&SrcBufPtr[i], &pOutBuf->Addr);

                    HaveExtData[i] = 1U;
                    ExtOsdDrawUpdate = 1U;
                }
            }
        }
        SvcExtOsd_MutexGive();
    } else {
        /* Do nothing */
    }
}

/**
 *  Init the ext osd draw module
 *  @return error code
 */
UINT32 SvcExtOsdDrawTask_Init(void)
{
    SvcExtOsd_MutexCreate();

#if defined (CONFIG_SOC_CV28)
    TargetVout = VOUT_IDX_A;
    DisplayMode = OSD_FHD_MODE;
#else
#if defined (CONFIG_ICAM_VOUTB_OSD_BUF_FHD)
    DisplayMode = OSD_FHD_MODE;
#endif
#endif

    return SVC_OK;
}

/**
 *  Start the object detection
 *  @return error code
 */
UINT32 SvcExtOsdDrawTask_Start(void)
{
    const SVC_RES_CFG_s *pResCfg = SvcResCfg_Get();
    UINT32 i;
    UINT32 RetVal = SVC_OK;
    UINT32 CvFlowNum = pResCfg->CvFlowNum;
    UINT32 CvFlowBits = pResCfg->CvFlowBits;
    const SVC_CV_FLOW_s *pCvFlow = pResCfg->CvFlow;

    /* Canvas register */
    SvcGui_Register(TargetVout, SVC_EXT_OSD_GUI_LEVEL, "ExtOsd", SvcExtOsdDraw, SvcExtOsdDrawUpdate);

    for (i = 0U; i < CvFlowNum; i++) {
        if ((CvFlowBits & ((UINT32) 1U << i)) > 0U) {
            if ((pCvFlow[i].CvFlowType == SVC_CV_FLOW_REF_OD) || (pCvFlow[i].CvFlowType == SVC_CV_FLOW_REF_OD_FDAG)) {
                RetVal |= SvcCvFlow_Register(i, SvcExtOsdDraw_DetectionResultCallback, &CvFlowRegisterID[i]);
                if (SVC_OK != RetVal) {
                    SvcLog_NG(SVC_LOG_EXTOSD_TASK, "SvcExtOsdDraw_Init err. SvcCvFlow_Register failed %d", RetVal, 0U);
                }
            }
        }
        HaveExtData[i] = 0U;
    }
    return RetVal;
}

/**
 *  Stop and Release Segmentation resource
 *  @return error code
 */
UINT32 SvcExtOsdDrawTask_Stop(void)
{
    UINT32 RetVal = SVC_OK;
    UINT32 i;
    const SVC_RES_CFG_s *pResCfg = SvcResCfg_Get();
    UINT32 CvFlowNum = pResCfg->CvFlowNum;
    UINT32 CvFlowBits = pResCfg->CvFlowBits;
    const SVC_CV_FLOW_s *pCvFlow = pResCfg->CvFlow;

    for (i = 0U; i < CvFlowNum; i++) {
        if ((CvFlowBits & ((UINT32) 1U << i)) > 0U) {
            if ((pCvFlow[i].CvFlowType == SVC_CV_FLOW_REF_OD) || (pCvFlow[i].CvFlowType == SVC_CV_FLOW_REF_OD_FDAG)) {
                RetVal |= SvcCvFlow_Unregister(i, CvFlowRegisterID[i]);
                if (SVC_OK != RetVal) {
                    SvcLog_NG(SVC_LOG_EXTOSD_TASK, "SvcExtOsdDraw_Stop err. SvcCvFlow_Unregister failed %d", RetVal, 0U);
                }
            }
        }
        HaveExtData[i] = 0U;
    }

    /* Un-register Gui canvas*/
    SvcGui_Unregister(TargetVout, SVC_EXT_OSD_GUI_LEVEL);
    return RetVal;
}
