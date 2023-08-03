/**
 *  @file SvcDirtDetect.c
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
 *  @details svc Dirt Detection task
 *
 */

#include "AmbaTypes.h"
#include "AmbaKAL.h"
#include "AmbaMisraFix.h"
#include "AmbaWrap.h"
#include "AmbaFS.h"
#include "AmbaUtility.h"
#include "AmbaGDMA.h"
#include "AmbaCache.h"

/* ssp */
#include "AmbaDSP_EventInfo.h"
#include "AmbaDSP_Event.h"
#include "AmbaDirtDetect.h"

/* svc-framework */
#include "SvcErrCode.h"
#include "SvcLog.h"
#include "SvcTask.h"
#include "SvcWrap.h"
#include "SvcUtil.h"
#include "SvcPlat.h"

/* svc-shared */
#include "SvcCan.h"
#include "SvcRecMain.h"
#include "SvcDirtDetect.h"

/* image proc*/
#include "AmbaImg_Proc.h"
#include "AmbaImg_External_CtrlFunc.h"

#define DIRT_DETECT_STACK_SIZE (0x4000)
#define DIRT_DETECT_YUV_WIDTH  (3840U)
#define DIRT_DETECT_YUV_HEIGHT (2160U)
#define DIRT_DETECT_YUV_SIZE   (DIRT_DETECT_YUV_WIDTH * DIRT_DETECT_YUV_HEIGHT * 2U)
#define DIRT_MAP_X_MAX         (32U)
#define DIRT_MAP_Y_MAX         (32U)
//#define OSD_COLOR_TRANS        (0U)
#define OSD_COLOR_RED          (87U)
//#define OSD_COLOR_GREEN        (176U)
#define OSD_COLOR_BLUE         (219U)
//#define DIRT_DETECT_CANVAS_WIDTH_MAX    (1920U)
//#define DIRT_DETECT_CANVAS_HEIGHT_MAX   (1080U)
//#define DIRT_DETECT_INTERVAL_DEFAULT    (4U)

#define SVC_LOG_DIRT_DETECT     "DIRT_DETECT"

typedef struct{
    UINT32 Width;
    UINT32 Height;
    UINT32 Pitch;
}AMBA_DIRT_DETECT_IMGSIZE;

/* Task Controller for Dirt Detection */
static SVC_TASK_CTRL_s DirtDetectCpCtrl GNU_SECTION_NOZEROINIT;
static SVC_TASK_CTRL_s DirtDetectAlgoCtrl GNU_SECTION_NOZEROINIT;
/* Event flag for flow controling */
static AMBA_KAL_EVENT_FLAG_t DirtDetectEventFlag GNU_SECTION_NOZEROINIT;
/* Target flag indicating which FovId need to be processed */
static UINT32 DirtDetectFovFlag = 1U;
/* Ring Queue for keeping the pyramid "content", need to copy the necessary information out */
static SVC_DIRT_DETECT_QUEUE_s DirtDetectQueue = {0U};
/* The Pyramid data copied from Pyramid event */
static SVC_DIRT_DETECT_PYRAMID_INFO_s DirtDetectPyramidData GNU_SECTION_NOZEROINIT;
static SVC_DIRT_DETECT_CONFIG_s DirtDetectConfig = {0U};

static UINT32 DirtDetectStatus    = SVC_DIRT_DETECT_STATUS_IDLE;
static UINT32 DirtDetectScene     = SVC_DIRT_DETECT_DAY_SCENE;
static UINT32 DirtDetectOpMode    = SVC_DIRT_DETECT_ONE_TIME;

/* Internal function */
static UINT32 DirtDetect_PyramidReady(const void *pEventInfo);
static void* DirtDetect_DataCpEntry(void* EntryArg);
static void* DirtDetect_AlgoEntry(void* EntryArg);
static UINT32 DirtDetect_GetYuvInfo(void);
static UINT32 DirtDetect_SkipYuvInfo(void);
static UINT32 DirtDetect_Algo(void);

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: SvcDirtDetect_Init
 *
 *  @Description:: Init the Dirt detection module
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *                  SVC_OK: Initialize successfully
 *                  SVC_NG: Initialize failed
\*-----------------------------------------------------------------------------------------------*/
UINT32 SvcDirtDetect_Init(void)
{
    static char FlagName[] = "DirtDetectEventFlag";
    UINT32 RetVal;

    SvcLog_OK(SVC_LOG_DIRT_DETECT, "SvcDirtDetect_Init start", 0U, 0U);

    /* Event flag to inform the Pyramid is ready */
    RetVal = AmbaKAL_EventFlagCreate(&DirtDetectEventFlag, FlagName);
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_DIRT_DETECT, "SvcDirtDetect_Init err. Event flag create failed %d", RetVal, 0U);
    }

    if (RetVal == OK) {
        SvcLog_OK(SVC_LOG_DIRT_DETECT, "SvcDirtDetect_Init success", 0U, 0U);
    } else {
        SvcLog_NG(SVC_LOG_DIRT_DETECT, "SvcDirtDetect_Init failed %d", RetVal, 0U);
    }

    #if 0
    DirtDetectConfig.AlgoInterval = DIRT_DETECT_INTERVAL_DEFAULT;
    DirtDetectConfig.PyramidHier  = 0U;
    DirtDetectConfig.pOsdOutputBuf = NULL;
    DirtDetectConfig.OsdOutputWidth = DIRT_DETECT_CANVAS_WIDTH_MAX;
    DirtDetectConfig.OsdOutputHeight = DIRT_DETECT_CANVAS_HEIGHT_MAX;
    DirtDetectConfig.pDetectDoneFunc = NULL;
    #endif

    SvcLog_OK(SVC_LOG_DIRT_DETECT, "SvcDirtDetect_Init done", 0U, 0U);

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: SvcDirtDetect_Config
 *
 *  @Description:: Configure the Dirt detection module
 *
 *  @Input      ::
 *                 pConfig: Input configuration to control the Dirt Detect module
 *
 *  @Output     ::
 *
 *  @Return     ::
 *                  SVC_OK: Configure successfully
 *                  SVC_NG: Configure failed
\*-----------------------------------------------------------------------------------------------*/
UINT32 SvcDirtDetect_Config(const SVC_DIRT_DETECT_CONFIG_s *pConfig)
{
    UINT32 RetVal;

    if (pConfig != NULL) {
        RetVal = AmbaWrap_memcpy(&DirtDetectConfig, pConfig, sizeof(SVC_DIRT_DETECT_CONFIG_s));
        DirtDetectFovFlag = ((UINT32) 1U) << DirtDetectConfig.FovIdx;
        if ((DirtDetectConfig.OsdOutputBlockX > DIRT_MAP_X_MAX) || (DirtDetectConfig.OsdOutputBlockY > DIRT_MAP_Y_MAX)) {
            SvcLog_NG(SVC_LOG_DIRT_DETECT, "OsdOutputBlock should not over X:%d Y:%d", DIRT_MAP_X_MAX, DIRT_MAP_Y_MAX);
            SvcLog_NG(SVC_LOG_DIRT_DETECT, "OsdOutputBlockX: %d OsdOutputBlockY: %d", DirtDetectConfig.OsdOutputBlockX, DirtDetectConfig.OsdOutputBlockY);
            RetVal = SVC_NG;
        }
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_DIRT_DETECT, "SvcDirtDetect_Config err. memcpy failed %d", RetVal, 0U);
        } else {
            RetVal = SVC_OK;
        }
    } else {
        SvcLog_NG(SVC_LOG_DIRT_DETECT, "SvcDirtDetect_Config err. Input configuration null", 0U, 0U);
        RetVal = SVC_NG;
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: SvcDirtDetect_ConfigGet
 *
 *  @Description:: Get the Dirt detection module configuration
 *
 *  @Input      ::
 *                 pConfig: Input configuration to control the Dirt Detect module
 *
 *  @Output     ::
 *                 pConfig: Current Configuration setting
 *  @Return     ::
 *                  SVC_OK: Get configure successfully
 *                  SVC_NG: Get configure failed
\*-----------------------------------------------------------------------------------------------*/
UINT32 SvcDirtDetect_ConfigGet(SVC_DIRT_DETECT_CONFIG_s *pConfig)
{
    UINT32 RetVal;

    if (pConfig != NULL) {
        RetVal = AmbaWrap_memcpy(pConfig, &DirtDetectConfig, sizeof(SVC_DIRT_DETECT_CONFIG_s));
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_DIRT_DETECT, "SvcDirtDetect_ConfigGet err. memcpy failed %d", RetVal, 0U);
        } else {
            RetVal = SVC_OK;
        }
    } else {
        SvcLog_NG(SVC_LOG_DIRT_DETECT, "SvcDirtDetect_ConfigGet err. Input configuration null", 0U, 0U);
        RetVal = SVC_NG;
    }

    return RetVal;
}



/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: SvcDirtDetect_Start
 *
 *  @Description:: Start the Dirt detection module
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *                  SVC_OK: Start successfully
 *                  SVC_NG: Start failed
\*-----------------------------------------------------------------------------------------------*/
UINT32 SvcDirtDetect_Start(UINT32 Priority, UINT32 CpuBits)
{
    UINT32 RetVal;

    static UINT8 DirtDetectDataCpTaskStack[DIRT_DETECT_STACK_SIZE] GNU_SECTION_NOZEROINIT;
    static UINT8 DirtDetectAlgoTaskStack[DIRT_DETECT_STACK_SIZE] GNU_SECTION_NOZEROINIT;

    DirtDetectStatus = SVC_DIRT_DETECT_STATUS_EXEC;

    DirtDetectCpCtrl.Priority   = Priority;
    DirtDetectCpCtrl.EntryFunc  = DirtDetect_DataCpEntry;
    DirtDetectCpCtrl.EntryArg   = 0U;
    DirtDetectCpCtrl.pStackBase = DirtDetectDataCpTaskStack;
    DirtDetectCpCtrl.StackSize  = DIRT_DETECT_STACK_SIZE;
    DirtDetectCpCtrl.CpuBits    = CpuBits;

    DirtDetectAlgoCtrl.Priority   = Priority + 1U;
    DirtDetectAlgoCtrl.EntryFunc  = DirtDetect_AlgoEntry;
    DirtDetectAlgoCtrl.EntryArg   = 0U;
    DirtDetectAlgoCtrl.pStackBase = DirtDetectAlgoTaskStack;
    DirtDetectAlgoCtrl.StackSize  = DIRT_DETECT_STACK_SIZE;
    DirtDetectAlgoCtrl.CpuBits    = CpuBits;

    RetVal = SvcTask_Create("SvcDirtDetectCpTask", &DirtDetectCpCtrl);
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_DIRT_DETECT, "SvcDirtDetect_Start err. Task (cp) create failed %d", RetVal, 0U);
    }

    if (SVC_OK == RetVal) {
        RetVal = SvcTask_Create("SvcDirtDetectAlgoTask", &DirtDetectAlgoCtrl);
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_DIRT_DETECT, "SvcDirtDetect_Start err. Task (algo) create failed %d", RetVal, 0U);
        }
    }

    if (SVC_OK == RetVal) {
        /* Register PYRAMID Event */
        RetVal = AmbaDSP_EventHandlerRegister(AMBA_DSP_EVENT_LV_PYRAMID_RDY, DirtDetect_PyramidReady);
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_DIRT_DETECT, "SvcDirtDetect_Init err. DSP Event register failed %d", RetVal, 0U);
        }
    }

    if (DirtDetectConfig.pOsdOutputBuf == NULL) {
        SvcLog_NG(SVC_LOG_DIRT_DETECT, "SvcDirtDetect_Start warning. No Osd buffer provided. Will not draw anything output.", 0U, 0U);
    }

    if (DirtDetectConfig.pDetectDoneFunc == NULL) {
        SvcLog_NG(SVC_LOG_DIRT_DETECT, "SvcDirtDetect_Start warning. No specified callback function exist.", 0U, 0U);
    }

    if (SVC_OK == RetVal) {
        SvcLog_OK(SVC_LOG_DIRT_DETECT, "SvcDirtDetect_Start Done", 0U, 0U);
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: SvcDirtDetect_Stop
 *
 *  @Description:: Stop the Dirt detection module
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *                  SVC_OK: Stop successfully
 *                  SVC_NG: Stop failed
\*-----------------------------------------------------------------------------------------------*/
UINT32 SvcDirtDetect_Stop(void)
{
    UINT32 RetVal;

    /* Register PYRAMID Event */
    RetVal = AmbaDSP_EventHandlerUnRegister(AMBA_DSP_EVENT_LV_PYRAMID_RDY, DirtDetect_PyramidReady);

    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_DIRT_DETECT, "SvcDirtDetect_Init err. DSP Event register failed %d", RetVal, 0U);
    }

    if (SVC_OK == RetVal){
        RetVal = AmbaDirtDetect_Reset(AMBA_DSP_SINGLE_CHAN);
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_DIRT_DETECT, "call AmabDirtDetect_Reset err. Task (algo) destroy failed", RetVal, 0U);
        }

    }

    if (SVC_OK == RetVal) {
        RetVal = SvcTask_Destroy(&DirtDetectAlgoCtrl);
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_DIRT_DETECT, "SvcDirtDetect_Stop err. Task (algo) destroy failed", RetVal, 0U);
        }
    }

    if (SVC_OK == RetVal) {
        RetVal = SvcTask_Destroy(&DirtDetectCpCtrl);
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_DIRT_DETECT, "SvcDirtDetect_Stop err. Task (cp) destroy failed", RetVal, 0U);
        }
    }

    if (SVC_OK == RetVal) {
        RetVal = AmbaKAL_EventFlagClear(&DirtDetectEventFlag, 0xFFFFFFFFU);
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_DIRT_DETECT, "SvcDirtDetect_Stop err. Flag clear failed", RetVal, 0U);
        }
    }

    if (SVC_OK == RetVal) {
        RetVal = AmbaWrap_memset(&DirtDetectQueue, 0, sizeof(SVC_DIRT_DETECT_QUEUE_s));
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_DIRT_DETECT, "SvcDirtDetect_Stop err. Queue clear failed", RetVal, 0U);
        }
    }

    DirtDetectStatus = SVC_DIRT_DETECT_STATUS_IDLE;

    return RetVal;
}

static UINT32 DirtDetect_PyramidReady(const void *pEventInfo)
{
    UINT32 RetVal;

    const AMBA_DSP_PYMD_DATA_RDY_s *pPyramidInfo;

    /* Cast to correct Type */
    AmbaMisra_TypeCast(&pPyramidInfo, &pEventInfo);

    /* DirtDetect_Entry is not busy */
    if ((((UINT32)1U << pPyramidInfo->ViewZoneId) & DirtDetectFovFlag) > 0U) {
        UINT32 WrIdx = DirtDetectQueue.WrIdx;

        /* Put the "content" into Queue */
        DirtDetectQueue.PyramidInfo[WrIdx] = *pPyramidInfo;

        DirtDetectQueue.WrIdx = (DirtDetectQueue.WrIdx + 1U) % SVC_DIRT_DETECT_QUEUE_SIZE;

        RetVal = AmbaKAL_EventFlagSet(&DirtDetectEventFlag, ((UINT32)1U << pPyramidInfo->ViewZoneId));
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_DIRT_DETECT, "DirtDetect_PyramidReady err. Event flag set failed %d", RetVal, 0U);
        }
    }


    return SVC_OK;
}

static void* DirtDetect_DataCpEntry(void* EntryArg)
{
    static UINT32 DirtDetectDataCounter = 0U;

    UINT32 RetVal = SVC_OK;
    UINT32 ActualFlag = 0U;
    const ULONG  *pArg;

    AmbaMisra_TouchUnused(EntryArg);
    AmbaMisra_TypeCast(&pArg, &EntryArg);

    while ((*pArg) == 0U) {
        UINT32 SkipYuv = 0U;

        /* Wait for the target Fov */
        RetVal = AmbaKAL_EventFlagGet(&DirtDetectEventFlag, DirtDetectFovFlag, AMBA_KAL_FLAGS_ALL, AMBA_KAL_FLAGS_CLEAR_AUTO, &ActualFlag, AMBA_KAL_WAIT_FOREVER);

        if (SVC_OK == RetVal) {
            /* Target Fov is ready */
            if ((ActualFlag & DirtDetectFovFlag) == DirtDetectFovFlag) {
                #if defined(CONFIG_BUILD_AMBA_ADAS)
                DOUBLE CanSpeed = 0.0;
                SvcDirtDetect_GetCanSpeed(&CanSpeed);
                if(CanSpeed >= 10.0) {
                    /* Copy data every interval */
                    if ((DirtDetectDataCounter % DirtDetectConfig.AlgoInterval) == 0U) {
                        /* Get the Yuv Pyramid Data */
                        RetVal = DirtDetect_GetYuvInfo();
                        if (SVC_OK == RetVal) {
                            AMBA_KAL_EVENT_FLAG_INFO_s EventFlagInfo;

                            /* Check whether previous algo is done or not */
                            RetVal = AmbaKAL_EventFlagQuery(&DirtDetectEventFlag, &EventFlagInfo);
                            if (SVC_OK == RetVal) {
                                if((EventFlagInfo.CurrentFlags & SVC_DIRT_DETECT_ALGO_PROCESSING) > 0U) {
                                    SvcLog_NG(SVC_LOG_DIRT_DETECT, "DirtDetect_DataCpEntry warning. Algorithm processing too slow", 0U, 0U);
                                }
                            }

                            /* Put into buffer anyway */
                            RetVal = AmbaKAL_EventFlagSet(&DirtDetectEventFlag, SVC_DIRT_DETECT_PYRAMID_CP_DONE);
                            if (SVC_OK != RetVal) {
                                SvcLog_NG(SVC_LOG_DIRT_DETECT, "DirtDetect_DataCpEntry err. Flag copy done failed", RetVal, 0U);
                            }
                        } else {
                            SvcLog_NG(SVC_LOG_DIRT_DETECT, "DirtDetect_DataCpEntry err. Gdma copy too slow", 0U, 0U);
                            SvcLog_NG(SVC_LOG_DIRT_DETECT, "DirtDetect_DataCpEntry err. Get Yuv info from queue failed", RetVal, 0U);
                        }
                    } else {
                        SkipYuv = 1U;
                    }
                } else {
                    SvcLog_NG(SVC_LOG_DIRT_DETECT, "CanSpeed < 10km/h, Skip the coming YUV", 0U, 0U);
                    SkipYuv = 1U;
                }
                #else
                /* Copy data every interval */
                if ((DirtDetectDataCounter % DirtDetectConfig.AlgoInterval) == 0U) {
                    /* Get the Yuv Pyramid Data */
                    RetVal = DirtDetect_GetYuvInfo();
                    if (SVC_OK == RetVal) {
                        AMBA_KAL_EVENT_FLAG_INFO_s EventFlagInfo;

                        /* Check whether previous algo is done or not */
                        RetVal = AmbaKAL_EventFlagQuery(&DirtDetectEventFlag, &EventFlagInfo);
                        if (SVC_OK == RetVal) {
                            if((EventFlagInfo.CurrentFlags & SVC_DIRT_DETECT_ALGO_PROCESSING) > 0U) {
                                SvcLog_NG(SVC_LOG_DIRT_DETECT, "DirtDetect_DataCpEntry warning. Algorithm processing too slow", 0U, 0U);
                            }
                        }

                        /* Put into buffer anyway */
                        RetVal = AmbaKAL_EventFlagSet(&DirtDetectEventFlag, SVC_DIRT_DETECT_PYRAMID_CP_DONE);
                        if (SVC_OK != RetVal) {
                            SvcLog_NG(SVC_LOG_DIRT_DETECT, "DirtDetect_DataCpEntry err. Flag copy done failed", RetVal, 0U);
                        }
                    } else {
                        SvcLog_NG(SVC_LOG_DIRT_DETECT, "DirtDetect_DataCpEntry err. Gdma copy too slow", 0U, 0U);
                        SvcLog_NG(SVC_LOG_DIRT_DETECT, "DirtDetect_DataCpEntry err. Get Yuv info from queue failed", RetVal, 0U);
                    }
                } else {
                    SkipYuv = 1U;
                }
                #endif

                if(SkipYuv == 1U){
                    /* Skip the coming YUV if it is not desired */
                    RetVal = DirtDetect_SkipYuvInfo();
                    if (SVC_OK != RetVal) {
                        SvcLog_NG(SVC_LOG_DIRT_DETECT, "DirtDetect_DataCpEntry err. skip failed", RetVal, 0U);
                    }
                }
                DirtDetectDataCounter = DirtDetectDataCounter + 1U;
            }
        } else {
            SvcLog_NG(SVC_LOG_DIRT_DETECT, "DirtDetect_DataCpEntry err. Event flag get failed %d", RetVal, 0U);
        }
    }

    return NULL;
}

static void* DirtDetect_AlgoEntry(void* EntryArg)
{
    UINT32 RetVal = SVC_OK;
    UINT32 ActualFlag = 0U;
    UINT32 tStart, tEnd;
    const ULONG  *pArg;

    AmbaMisra_TouchUnused(EntryArg);
    AmbaMisra_TypeCast(&pArg, &EntryArg);

    while ((*pArg) == 0U) {
        /* Wait for the target Fov */
        RetVal = AmbaKAL_EventFlagGet(&DirtDetectEventFlag, SVC_DIRT_DETECT_PYRAMID_CP_DONE, AMBA_KAL_FLAGS_ALL, AMBA_KAL_FLAGS_CLEAR_NONE, &ActualFlag, AMBA_KAL_WAIT_FOREVER);

        if (SVC_OK == RetVal) {
            RetVal = AmbaKAL_EventFlagSet(&DirtDetectEventFlag, SVC_DIRT_DETECT_ALGO_PROCESSING);
            if (SVC_OK == RetVal) {

                RetVal = AmbaKAL_GetSysTickCount(&tStart);
                if (Rval != SVC_OK) {
                    SvcLog_NG(__func__, "%d", __LINE__, 0U);
                }
                /* Dirt Detection algorithm here */
                RetVal = DirtDetect_Algo();
                if (Rval != SVC_OK) {
                    SvcLog_NG(__func__, "%d", __LINE__, 0U);
                }
                RetVal = AmbaKAL_GetSysTickCount(&tEnd);
                if (Rval != SVC_OK) {
                    SvcLog_NG(__func__, "%d", __LINE__, 0U);
                }
                // SvcLog_DBG(SVC_LOG_DIRT_DETECT, "Time %d", tEnd-tStart, 0U);

                /* Dirt Detection algorithm done */
                if (SVC_OK == RetVal) {
                    /* Trigger SvcGui to update */
                    if (DirtDetectConfig.pDetectDoneFunc != NULL) {
                        DirtDetectConfig.pDetectDoneFunc();
                    }
                } else {
                    SvcLog_NG(SVC_LOG_DIRT_DETECT, "DirtDetect_AlgoEntry err. Algorithm executed failed %d", RetVal, 0U);
                }
            } else {
                SvcLog_NG(SVC_LOG_DIRT_DETECT, "DirtDetect_AlgoEntry err. Pyramid data copied failed %d", RetVal, 0U);
            }

            RetVal = AmbaKAL_EventFlagClear(&DirtDetectEventFlag, SVC_DIRT_DETECT_PYRAMID_CP_DONE | SVC_DIRT_DETECT_ALGO_PROCESSING);
            if (SVC_OK != RetVal) {
                SvcLog_NG(SVC_LOG_DIRT_DETECT, "DirtDetect_AlgoEntry err. Flag clear failed %d", RetVal, 0U);
            }
        } else {
            SvcLog_NG(SVC_LOG_DIRT_DETECT, "DirtDetect_AlgoEntry err. Flag get failed %d", RetVal, 0U);
        }
    }

    return NULL;
}

static UINT32 DirtDetect_GetYuvInfo(void)
{
    /* YUV processing buffer, 0 for Y, 1 for UV */
    static UINT8  DirtDetectYuvBuffer[DIRT_DETECT_YUV_SIZE] GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE;

    UINT32 RetVal = SVC_OK;
    ULONG  Addr;

    /* Copy the desired content out */
    UINT32 WrIdx = DirtDetectQueue.WrIdx;
    UINT32 RdIdx = DirtDetectQueue.RdIdx;

    if (RdIdx != WrIdx) {
        UINT32 YWidth = 0U, UVWidth = 0U;
        UINT32 YHeight = 0U, UVHeight = 0U;
        const AMBA_DSP_YUV_IMG_BUF_s *pYuvBuf = &DirtDetectQueue.PyramidInfo[RdIdx].YuvBuf[DirtDetectConfig.PyramidHier];

        DirtDetectPyramidData.FovIdx      = (UINT32) DirtDetectQueue.PyramidInfo[RdIdx].ViewZoneId;
        DirtDetectPyramidData.PyramidHier = DirtDetectConfig.PyramidHier;
        DirtDetectPyramidData.CapSequence = DirtDetectQueue.PyramidInfo[RdIdx].CapSequence;

        DirtDetectPyramidData.Pitch       = (UINT32) pYuvBuf->Pitch;
        DirtDetectPyramidData.Width       = (UINT32) pYuvBuf->Window.Width;
        DirtDetectPyramidData.Height      = (UINT32) pYuvBuf->Window.Height;
        DirtDetectPyramidData.DataFmt     = pYuvBuf->DataFmt;
        DirtDetectPyramidData.pBufferY    = DirtDetectYuvBuffer;
        DirtDetectPyramidData.pBufferUV   = &(DirtDetectYuvBuffer[DirtDetectPyramidData.Width * DirtDetectPyramidData.Height]);


        if (pYuvBuf->DataFmt == AMBA_DSP_YUV420) {
            YWidth = pYuvBuf->Window.Width;
            UVWidth = pYuvBuf->Window.Width;
            YHeight = pYuvBuf->Window.Height;
            UVHeight = (UINT32)(pYuvBuf->Window.Height) >> 1U;
        } else if (pYuvBuf->DataFmt == AMBA_DSP_YUV422) {
            YWidth = pYuvBuf->Window.Width;
            UVWidth = pYuvBuf->Window.Width;
            YHeight = pYuvBuf->Window.Height;
            UVHeight = pYuvBuf->Window.Height;
        } else {
            SvcLog_NG(SVC_LOG_DIRT_DETECT, "DirtDetect_GetYuvInfo err. Not support DataFmt", 0U, 0U);
            RetVal = SVC_NG;
        }

        if (SVC_OK == RetVal) {
            ULONG  Addr;
            UINT8  *pBufferY;
            UINT8  *pBufferUV;

            Addr = pYuvBuf->BaseAddrY;
            AmbaMisra_TypeCast(&pBufferY, &Addr);
            Addr = pYuvBuf->BaseAddrUV;
            AmbaMisra_TypeCast(&pBufferUV, &Addr);

            RetVal = AmbaWrap_memset(DirtDetectYuvBuffer, 0, sizeof(DirtDetectYuvBuffer));
            if (SVC_OK != RetVal) {
                SvcLog_NG(SVC_LOG_DIRT_DETECT, "DirtDetect_GetYuvInfo err. memset failed %d", RetVal, 0U);
            }

            /* GDMA copy */
            if (SVC_OK == RetVal) {
                AMBA_GDMA_BLOCK_s GdmaBlockY = {
                    .pSrcImg = pBufferY,
                    .pDstImg = DirtDetectPyramidData.pBufferY,
                    .SrcRowStride = DirtDetectPyramidData.Pitch,
                    .DstRowStride = DirtDetectPyramidData.Width,
                    .BltWidth  = YWidth,
                    .BltHeight = YHeight,
                    .PixelFormat = AMBA_GDMA_8_BIT,
                };

                AMBA_GDMA_BLOCK_s GdmaBlockUV = {
                    .pSrcImg = pBufferUV,
                    .pDstImg = DirtDetectPyramidData.pBufferUV,
                    .SrcRowStride = DirtDetectPyramidData.Pitch,
                    .DstRowStride = DirtDetectPyramidData.Width,
                    .BltWidth  = UVWidth,
                    .BltHeight = UVHeight,
                    .PixelFormat = AMBA_GDMA_8_BIT,
                };

                // SvcLog_OK(SVC_LOG_DIRT_DETECT, "GDMA Copy start CapSequence = %d", (UINT32)DirtDetectPyramidData.CapSequence, 0U);
                AmbaMisra_TypeCast(&Addr, &DirtDetectYuvBuffer);
                RetVal = SvcPlat_CacheInvalidate(Addr, sizeof(DirtDetectYuvBuffer));
                if (SVC_OK != RetVal) {
                    SvcLog_NG(SVC_LOG_DIRT_DETECT, "DirtDetect_GetYuvInfo err. Cache clean err %d", RetVal, 0U);
                }
                RetVal = AmbaGDMA_BlockCopy(&GdmaBlockY, NULL, 0U, 1000U /* Wait 1000ms */);
                if (SVC_OK != RetVal) {
                    SvcLog_NG(SVC_LOG_DIRT_DETECT, "DirtDetect_GetYuvInfo err. Gdma copy Y failed %d", RetVal, 0U);
                }
                RetVal = AmbaGDMA_WaitAllCompletion(1000U);
                if (SVC_OK != RetVal) {
                    SvcLog_NG(SVC_LOG_DIRT_DETECT, "DirtDetect_GetYuvInfo err. Gdma copy Y wait err %d", RetVal, 0U);
                }
                RetVal = AmbaGDMA_BlockCopy(&GdmaBlockUV, NULL, 1U, 1000U /* wait 500ms */);
                if (SVC_OK != RetVal) {
                    SvcLog_NG(SVC_LOG_DIRT_DETECT, "DirtDetect_GetYuvInfo err. Gdma copy UV failed %d", RetVal, 0U);
                }
                RetVal = AmbaGDMA_WaitAllCompletion(1000U);
                if (SVC_OK != RetVal) {
                    SvcLog_NG(SVC_LOG_DIRT_DETECT, "DirtDetect_GetYuvInfo err. Gdma copy UV wait err %d", RetVal, 0U);
                }
            }
        }
        DirtDetectQueue.RdIdx = (DirtDetectQueue.RdIdx + 1U) % SVC_DIRT_DETECT_QUEUE_SIZE;
    } else {
        RetVal = SVC_NG;
    }

    return RetVal;
}

static UINT32 DirtDetect_SkipYuvInfo(void)
{
    DirtDetectQueue.RdIdx = (DirtDetectQueue.RdIdx + 1U) % SVC_DIRT_DETECT_QUEUE_SIZE;

    return SVC_OK;
}

static UINT32 SvcDirtDetect_Min_U32(UINT32 x, UINT32 y)
{
    UINT32 ret = x;

    if( x > y ) {
       ret = y;
    } else {
       ret = x;
    }

    return ret;
}


static void SvcDirtDetect_BlockUpsampling(AMBA_DIRT_DETECT_IMGSIZE InSize, AMBA_DIRT_DETECT_IMGSIZE OutSize, const UINT8 *input, UINT8 *output, UINT8 OSDValue)
{
    UINT32 inW, inH, outW, outH;
    UINT32 ratioX, ratioY;
    DOUBLE DratioX,DratioY;

    inW = InSize.Width; //16
    inH = InSize.Height; //9
    outW = OutSize.Width; //960
    outH = OutSize.Height; //480



    (void)AmbaWrap_ceil((DOUBLE)outW/(DOUBLE)inW,&DratioX);  //60
    (void)AmbaWrap_ceil((DOUBLE)outH/(DOUBLE)inH,&DratioY);  //53
    ratioX = (UINT32)DratioX;
    ratioY = (UINT32)DratioY;

#if 0
    SvcLog_NG(SVC_LOG_DIRT_DETECT, "inW/inH %d/%d", inW, inH);
    SvcLog_NG(SVC_LOG_DIRT_DETECT, "outW/outH %d/%d", outW, outH);
    SvcLog_NG(SVC_LOG_DIRT_DETECT, "ratioX/ratioY %d/%d", ratioX, ratioY);
#endif

    AmbaSvcWrap_MisraMemset(output, 0 , sizeof(UINT8)*outW*outH);

    for(UINT32 j = 0U; j<outH; j++){
        UINT32 idxY = SvcDirtDetect_Min_U32((UINT32)j/ratioY,inH);
        for(UINT32 i = 0U; i<outW; i++){
            UINT32 idxX = SvcDirtDetect_Min_U32((UINT32)i/ratioX,inW);
              output[(j*outW)+i] = (input[(idxY * inW) + idxX]>128U) ? OSDValue:0U;
        }
    }
}



static UINT32 DirtDetect_Algo(void)
{
    static UINT8 DetectDirtMap[DIRT_MAP_X_MAX * DIRT_MAP_Y_MAX];

    static UINT8 Init = 0U;
    static UINT8 AlgoNode = AMBA_DSP_SINGLE_CHAN;

    UINT32 RetVal = SVC_OK;
    UINT32 ProcRet = 0U;
    // UINT32 ViewID = 0;
    UINT32 OpMode;

    AMBA_DIRT_DETECT_IMGSIZE InSize,OutSize;
    // AMBA_AE_INFO_s AEInfo;

    AMBA_DIRT_DETECT_INIT_DATA_s DirtDetectInitConfig;

    AMBA_DIRT_DETECT_IN_INFO_s DDInput = {
        .pYAddr         = DirtDetectPyramidData.pBufferY,
        .pUVAddr        = DirtDetectPyramidData.pBufferUV,
        .YUVFormat      = 0U,
        .InWidth        = DirtDetectPyramidData.Width,
        .InHeight       = DirtDetectPyramidData.Height,
        .InPitch        = DirtDetectPyramidData.Pitch,
        .Reserve1       = 0,
        .Reserve2       = 0,
        .Reserve3       = 0,
    };

    AMBA_DIRT_DETECT_OUT_INFO_s DDOutput = {
        .ResultFlag = 0,
        .pDirtMap = DirtDetectConfig.pOutputBuf,
        .pDirtBlockMap = DetectDirtMap,
    };

    DirtDetectInitConfig.ChanID = 0U;
    DirtDetectInitConfig.WorkSize.Width = DirtDetectPyramidData.Width;
    DirtDetectInitConfig.WorkSize.Height = DirtDetectPyramidData.Height;
    DirtDetectInitConfig.BlockNum.Width = DirtDetectConfig.OsdOutputBlockX;
    DirtDetectInitConfig.BlockNum.Height = DirtDetectConfig.OsdOutputBlockY;
    DirtDetectInitConfig.HorizonCenter.x = DirtDetectConfig.HOZ_x;
    DirtDetectInitConfig.HorizonCenter.y = DirtDetectConfig.HOZ_y;
    DirtDetectInitConfig.ROI.LT.x = 0U;
    DirtDetectInitConfig.ROI.LT.y = 0U;
    DirtDetectInitConfig.ROI.RB.x = DirtDetectPyramidData.Width;
    DirtDetectInitConfig.ROI.RB.y = DirtDetectPyramidData.Height;
    DirtDetectInitConfig.UPref.Parameters1 = 2U;
    DirtDetectInitConfig.UPref.Parameters2 = 3U;
    DirtDetectInitConfig.UPref.Parameters3 = 0U;
    DirtDetectInitConfig.UPref.Parameters4 = 0U;
    DirtDetectInitConfig.UPref.Parameters5 = 0U;

    ///////////////////Dirt Detect Init//////////////////////////////////
    if(Init == 0U){
        ULONG Addr = SvcDirtDetect_InitAddress(0U);
        UINT8 *pAddr;

        AmbaMisra_TypeCast(&pAddr, &Addr);
        if(DirtDetect_NG_0001 == AmbaDirtDetect_Init(AlgoNode, &DirtDetectInitConfig, pAddr)){
            SvcLog_NG(SVC_LOG_DIRT_DETECT,"AmabDirtDetect_Init failed",0U,0U);
        } else {
            Init ++;
        }
    }

    ///////////////////Dirt Detect Proc//////////////////////////////////
    // (void)AmbaImgProc_AEGetExpInfo(ViewID, 0U, IP_MODE_VIDEO, &AEInfo);
    // DDInput.Reserve1 = AEInfo.EvIndex;
    ProcRet = AmbaDirtDetect_Proc(&DDInput,&DDOutput);

    ///////////////////Dirt Detect Output Handeling//////////////////////////////////
    InSize.Width = 16U;
    InSize.Height = 10U;
    OutSize.Width = DirtDetectConfig.OsdOutputWidth;
    OutSize.Height = DirtDetectConfig.OsdOutputHeight;

    if(ProcRet == DirtDetect_OK) {
        if(DDOutput.ResultFlag ==  2U) {
            SvcDirtDetect_BlockUpsampling(InSize, OutSize, DDOutput.pDirtBlockMap, DirtDetectConfig.pOsdOutputBuf, OSD_COLOR_BLUE);
            //SvcLog_OK(SVC_LOG_DIRT_DETECT, " ===== Algo Result =====", 0U, 0U);
            DirtDetectScene = SVC_DIRT_DETECT_DAY_SCENE;
        } else if(DDOutput.ResultFlag == AMBA_DIRT_DETECT_FINAL) {
            SvcLog_OK(SVC_LOG_DIRT_DETECT, " ==========================", 0U, 0U);
            SvcLog_OK(SVC_LOG_DIRT_DETECT, " ===== Final Converge =====", 0U, 0U);
            SvcLog_OK(SVC_LOG_DIRT_DETECT, " ==========================", 0U, 0U);
            SvcDirtDetect_BlockUpsampling(InSize, OutSize, DDOutput.pDirtBlockMap, DirtDetectConfig.pOsdOutputBuf, OSD_COLOR_RED);
            Init = 0U;
            SvcDirtDetect_GetOpMode(&OpMode);
            if(OpMode == SVC_DIRT_DETECT_ONE_TIME) {
                DirtDetectStatus = SVC_DIRT_DETECT_STATUS_FORCE_STOP;
            }
        } else if(DDOutput.ResultFlag ==  1U) {
            //SvcLog_OK(SVC_LOG_DIRT_DETECT, " ===========================", 0U, 0U);
            //SvcLog_OK(SVC_LOG_DIRT_DETECT, " ===== Normal Converge =====", 0U, 0U);
            //SvcLog_OK(SVC_LOG_DIRT_DETECT, " ===========================", 0U, 0U);
            //SvcDirtDetect_BlockUpsampling(InSize, OutSize, DDOutput.pDirtBlockMap, DirtDetectConfig.pOsdOutputBuf, OSD_COLOR_GREEN);
            SvcDirtDetect_BlockUpsampling(InSize, OutSize, DDOutput.pDirtBlockMap, DirtDetectConfig.pOsdOutputBuf, OSD_COLOR_BLUE);
        } else {
            SvcLog_NG(SVC_LOG_DIRT_DETECT, "unknow result flag", 0U, 0U);
        }
        //SvcLog_OK(SVC_LOG_DIRT_DETECT, "Algo exec done", 0U, 0U);
        DirtDetectScene = SVC_DIRT_DETECT_DAY_SCENE;
    } else if(ProcRet == DirtDetect_NG_0002) {
        AmbaSvcWrap_MisraMemset(DDOutput.pDirtBlockMap, 0,  InSize.Width * InSize.Height);
        SvcDirtDetect_BlockUpsampling(InSize, OutSize, DDOutput.pDirtBlockMap, DirtDetectConfig.pOsdOutputBuf, 0);
        SvcLog_NG(SVC_LOG_DIRT_DETECT, "Night Scene!!", 0U, 0U);
        RetVal = AmbaDirtDetect_Reset(AMBA_DSP_SINGLE_CHAN);
        //DirtDetectStatus = SVC_DIRT_DETECT_STATUS_FORCE_STOP;
        DirtDetectScene = SVC_DIRT_DETECT_NIGHT_SCENE;
    } else {
        SvcLog_NG(SVC_LOG_DIRT_DETECT,"AmabDirtDetect_Proc failed %d",ProcRet ,0U);
        DirtDetectStatus = SVC_DIRT_DETECT_STATUS_FORCE_STOP;
        DirtDetectScene = SVC_DIRT_DETECT_DAY_SCENE;
    }

    // if(0){
    //     char YuvFileNameOSD[16];
    //     AMBA_FS_FILE *pFileOSD;
    //     UINT32 Length;
    //     UINT32 NumSuccess;
    //     static UINT32 YuvFileCounter = 0U;
    //     Length = SvcWrap_sprintfU32(YuvFileNameOSD, 16, "C:\\OSD_%d.y", 1, &YuvFileCounter);
    //     RetVal = AmbaFS_FileOpen(YuvFileNameOSD, "wb", &pFileOSD);
    //     RetVal = AmbaFS_FileWrite(DirtDetectConfig.pOsdOutputBuf, OsdOutputWidth * OsdOutputHeight, 1, pFileOSD, &NumSuccess);
    //     RetVal = AmbaFS_FileClose(pFileOSD);
    //     YuvFileCounter = YuvFileCounter + 1U;
    // }


    /* Algorithm done */
    return RetVal;
}

#if defined(CONFIG_BUILD_AMBA_ADAS)
void SvcDirtDetect_GetCanSpeed(DOUBLE *TransferSpeed)
{
    extern AMBA_SR_CANBUS_RAW_DATA_s CanbusRawData;
    AMBA_SR_CANBUS_TRANSFER_DATA_s CanbusTransData; // canbus transfer data

    /* init CanbusTransData */
    CanbusTransData.CANBusGearStatus = 0U;
    CanbusTransData.CANBusTurnLightStatus = 0U;
    CanbusTransData.FlagValidTransferData = 0U;
    CanbusTransData.TransferSpeed = 0.0;
    CanbusTransData.TransferWheelAngle = 0.0;
    CanbusTransData.TransferWheelAngleSpeed = 0.0;
    CanbusTransData.WheelDir = 0U;
    CanbusTransData.CapTS = 0U;

    /* canbus transfer */
    (void)SvcCan_Transfer(&CanbusRawData, &CanbusTransData);
    if(CanbusTransData.FlagValidTransferData == 0U) {
        *TransferSpeed = 85.0;
    } else {
        *TransferSpeed = CanbusTransData.TransferSpeed;
    }
}
#endif

void SvcDirtDetect_GetStatus(UINT32 *Status)
{
    *Status = DirtDetectStatus;
}

void SvcDirtDetect_GetScene(UINT32 *Scene)
{
    *Scene = DirtDetectScene;
}

void SvcDirtDetect_GetOpMode(UINT32 *OpMode)
{
    *OpMode = DirtDetectOpMode;
}

void SvcDirtDetect_SetOpMode(UINT32 OpMode)
{
    DirtDetectOpMode = OpMode;
}

UINT32 SvcDirtDetect_InitAddress(UINT32 Addr)
{
    static UINT32 InitAddress = 0U;
    UINT32 rval = 0U;

    if(Addr == 0U) {
        rval = InitAddress;
    } else {
        InitAddress = Addr;
    }

    #if 0
    SVC_WRAP_PRINT " InitAddress %d"
    SVC_PRN_ARG_S __func__
    SVC_PRN_ARG_PROC SvcLog_NG
    SVC_PRN_ARG_UINT32 InitAddress SVC_PRN_ARG_POST
    SVC_PRN_ARG_E
    AmbaPrint_Flush();
    #endif

    return rval;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: SvcDirtDetect_MVDataCB
 *
 *  @Description:: callback function to receive MV data
 *
 *  @Input      ::
 *    pMVData     : MV data
 *
 *  @Output     :: none
 *
 *  @Return     ::
\*-----------------------------------------------------------------------------------------------*/
void SvcDirtDetect_MVDataCB(const void *pMVData)
{
#if 0
    UINT32                   i, j, Addr;
    const SVC_ENC_MV_DATA_s  *pData;
    const ENC_INTRA_s        *pEncIntra;
    const ENC_MV_s           *pEncMV;

    AmbaMisra_TypeCast(&pData, &pMVData);

    AmbaPrint_PrintUInt5("[Intra]", 0U, 0U, 0U, 0U, 0U);
    for (i = 0U; i < pData->CtbRowNum; i++) {
        Addr = pData->IntraBufBase + (i * pData->IntraBufPitch);
        AmbaMisra_TypeCast(&pEncIntra, &Addr);

        for (j = 0U; j < pData->CtbColNum; j++) {
            AmbaPrint_PrintUInt5("  Col[%d] %d %d %d %d"
                                    , j
                                    , pEncIntra[j].Intra_0_0.IntraActivity
                                    , pEncIntra[j].Intra_0_1.IntraActivity
                                    , pEncIntra[j].Intra_1_0.IntraActivity
                                    , pEncIntra[j].Intra_1_1.IntraActivity);
        }
    }

    AmbaPrint_PrintUInt5("[MV]", 0U, 0U, 0U, 0U, 0U);
    for (i = 0U; i < pData->CtbRowNum; i++) {
        Addr = pData->MVBufBase + (i * pData->MVBufPitch);
        AmbaMisra_TypeCast(&pEncMV, &Addr);

        for (j = 0U; j < pData->CtbColNum; j++) {
            AmbaPrint_PrintUInt5("  Col[%d] %d %d %d %d"
                                    , j
                                    , pEncMV[j].MV_0_0.MvX
                                    , pEncMV[j].MV_0_1.MvX
                                    , pEncMV[j].MV_1_0.MvX
                                    , pEncMV[j].MV_1_1.MvX);
        }
    }
#else
    AmbaMisra_TouchUnused(&pMVData);
#endif
}
