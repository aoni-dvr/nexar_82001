/**
 *  @file SvcCvCtrlTask.c
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
 *  @details svc cv control task
 *
 */

#include "AmbaTypes.h"
#include "AmbaKAL.h"
#include "AmbaWrap.h"
#include "AmbaUtility.h"

#include "AmbaDSP.h"
#include "AmbaDSP_Liveview.h"

/* svc-framework */
#include "SvcErrCode.h"
#include "SvcLog.h"
#include "SvcMem.h"
#include "SvcBuffer.h"
#include "SvcWrap.h"
#include "SvcSysStat.h"

/* app-shared */
#include "SvcResCfg.h"
#include "SvcCvAlgo.h"
#include "SvcCvFlow.h"
#include "SvcCvFlowUtil.h"
#include "SvcCvFlowAlgoUtil.h"
#include "SvcCvFlowInputUtil.h"
#include "SvcCvFlow_Comm.h"
#include "SvcImgFIFO.h"
#include "SvcImgFeeder.h"
#include "SvcTiming.h"
#if (defined(CONFIG_AMBALINK_BOOT_OS) || defined(CONFIG_LINUX))
#include "AmbaIPC_FlexidagIO.h"
#include "SvcCvFlexidagIO.h"
#endif

/* app-icam */
#include "SvcCvCtrlTask.h"
#include "SvcCvMainTask.h"
#include "SvcBufMap.h"
#include "SvcAppStat.h"

static AMBA_KAL_EVENT_FLAG_t SvcCvCtrlEvt;
// #define SVC_CV_CTRL_EVT_INIT      (0x1U)
#define SVC_CV_CTRL_EVT_CONFIG    (0x2U)
#define SVC_CV_CTRL_EVT_LOAD      (0x4U)
#define SVC_LOG_CV_CTRL_TASK      "CV_CTRL_TASK"

static UINT32 CvFlowRdyBits = 0U;
static UINT32 CvFlowStatusID[SVC_CV_FLOW_CHAN_MAX] = {0U};

static UINT32 SvcCvCtrlTask_PackCvCfg(const SVC_CV_FLOW_s *pResCvCfg, SVC_CV_FLOW_CFG_s *pCvCfg);
static void CvCtrlTask_CvBootStatusCb(UINT32 StatIdx, void *pInfo);
static void CvCtrlTask_CvFlowStatusRegister(void);
static void CvCtrlTask_CvFlowStatusCb(UINT32 CvFlowChan, UINT32 Event, void *pInfo);
static void CvCtrlTask_CvFlowStatusUnregister(void);
#ifdef CONFIG_AMBALINK_BOOT_OS
static void CvCtrlTask_LinuxBootStatusCb(UINT32 StatIdx, void *pInfo);
#endif

static UINT32 SvcCvCtrlTask_PackCvCfg(const SVC_CV_FLOW_s *pResCvCfg, SVC_CV_FLOW_CFG_s *pCvCfg)
{
    UINT32 RetVal = SVC_OK;
    UINT32 i, j;
    SVC_CV_INPUT_CFG_s *pInputCfg;
    const SVC_RES_CV_INPUT_CFG_s *pResInputCfg;
    SVC_CV_OUTPUT_CFG_s *pOutputCfg;
    const SVC_RES_CV_OUTPUT_CFG_s *pResOutputCfg;

    RetVal = AmbaWrap_memset(pCvCfg, 0, sizeof(SVC_CV_FLOW_CFG_s));
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_CV_CTRL_TASK, "memset CvCfg failed", 0U, 0U);
    }

    pInputCfg = &pCvCfg->InputCfg;
    pResInputCfg = &pResCvCfg->InputCfg;
    if (pResInputCfg->InputNum <= SVC_CV_FLOW_INPUT_MAX) {
        pCvCfg->CvModeID = pResCvCfg->CvModeID;

        pInputCfg->InputNum = pResInputCfg->InputNum;
        for (i = 0; i < pResInputCfg->InputNum; i++) {
            pInputCfg->Input[i].DataSrc = pResInputCfg->Input[i].DataSrc;
            pInputCfg->Input[i].StrmId = pResInputCfg->Input[i].StrmId;
            pInputCfg->Input[i].FrameWidth = pResInputCfg->Input[i].FrameWidth;
            pInputCfg->Input[i].FrameHeight = pResInputCfg->Input[i].FrameHeight;

            if (pResInputCfg->Input[i].NumRoi <= SVC_CV_MAX_ROI) {
                pInputCfg->Input[i].RoiInfo.NumRoi = pResInputCfg->Input[i].NumRoi;
                for (j = 0; j < pResInputCfg->Input[i].NumRoi; j++) {
                    pInputCfg->Input[i].RoiInfo.Roi[j].Index  = pResInputCfg->Input[i].Roi[j].Index;
                    pInputCfg->Input[i].RoiInfo.Roi[j].StartX = pResInputCfg->Input[i].Roi[j].StartX;
                    pInputCfg->Input[i].RoiInfo.Roi[j].StartY = pResInputCfg->Input[i].Roi[j].StartY;
                    pInputCfg->Input[i].RoiInfo.Roi[j].Width  = pResInputCfg->Input[i].Roi[j].Reserved0;
                    pInputCfg->Input[i].RoiInfo.Roi[j].Height = pResInputCfg->Input[i].Roi[j].Reserved1;
                }
            } else {
                RetVal = SVC_NG;
                break;
            }
        }

        pInputCfg->SrcRate = pResInputCfg->SrcRate;

        if (pResInputCfg->EnableFeeder == 1U) {
            SVC_IMG_FEEDER_CREATE_CFG_s CreateCfg = {0};

#if defined CONFIG_ICAM_IMG_FEEDER_CORE_SELECT
            CreateCfg.TaskPriority = CONFIG_ICAM_IMG_FEEDER_TASK_PRIORITY;
            CreateCfg.CpuBits = CONFIG_ICAM_IMG_FEEDER_CORE_SELECT;
#else
            CreateCfg.TaskPriority = 50U;
            CreateCfg.CpuBits = 0x01U;
#endif
            CreateCfg.FeederFlag   = pResInputCfg->FeederFlag;
            RetVal = SvcImgFeeder_Create(pResInputCfg->FeederID, &CreateCfg);
            if (SVC_OK == RetVal) {
                pInputCfg->EnableFeeder = 1U;
                pInputCfg->FeederID     = pResInputCfg->FeederID;
                pInputCfg->FeederFlag   = pResInputCfg->FeederFlag;
            } else {
                SvcLog_NG(SVC_LOG_CV_CTRL_TASK, "fail to SvcImgFeeder_Create(%d)", pInputCfg->FeederID, 0U);
            }
        }
    } else {
        RetVal = SVC_NG;
    }

    if (RetVal == SVC_OK) {
        pOutputCfg = &pCvCfg->OutputCfg;
        pResOutputCfg = &pResCvCfg->OutputCfg;
        for (i = 0; i < SVC_CV_FLOW_OUTPUT_PORT_MAX; i++) {
            pOutputCfg->OutputTag[i] = pResOutputCfg->OutputTag[i];
        }

        pCvCfg->PrivFlag = pResCvCfg->PrivFlag;

        RetVal = AmbaWrap_memcpy(&pCvCfg->Config[0], &pResCvCfg->Config[0], sizeof(pCvCfg->Config));
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_CV_CTRL_TASK, "memcpy Config[] failed", 0U, 0U);
        }
    }

    return RetVal;
}

/**
 *  Init the CV Flow task
 *  @return error code
 */
UINT32 SvcCvCtrlTask_Init(void)
{
    UINT32 RetVal;
    static char SvcCvCtrlEvtName[32] = "SvcCvLoaderEvt";
    UINT32 CtrlID;

    RetVal  = AmbaKAL_EventFlagCreate(&SvcCvCtrlEvt, SvcCvCtrlEvtName);
    RetVal |= AmbaKAL_EventFlagClear(&SvcCvCtrlEvt, 0xFFFFFFFFU);
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_CV_CTRL_TASK, "SvcCvCtrl create event flag failed", 0U, 0U);
    }

    RetVal = SvcSysStat_Register(SVC_APP_STAT_CV_BOOT, CvCtrlTask_CvBootStatusCb, &CtrlID);
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_CV_CTRL_TASK, "Register CV status failed", 0U, 0U);
    }

#if (defined(CONFIG_AMBALINK_BOOT_OS) || defined(CONFIG_LINUX))
    RetVal = SvcCv_FlexiIOInit();
    if (RetVal != SVC_OK) {
        SvcLog_DBG(SVC_LOG_CV_CTRL_TASK, "SvcCv_FlexiIOInit failed", 0U, 0U);
    }

#if (defined(CONFIG_AMBALINK_BOOT_OS))
    /* DualOS */
    RetVal = SvcSysStat_Register(SVC_APP_STAT_LINUX, CvCtrlTask_LinuxBootStatusCb, &CtrlID);
    if (RetVal != SVC_OK) {
        SvcLog_DBG(SVC_LOG_CV_CTRL_TASK, "Register Linux status failed", 0U, 0U);
    }
#else
    /* Single Linux */
    RetVal = SvcCv_FlexiIOSysEventNotify(SVC_CV_FLEXIDAG_IO_IPC_READY);
    if (RetVal != SVC_OK) {
        SvcLog_NG(SVC_LOG_CV_CTRL_TASK, "FlexiIOSysEventNotify failed(0x%x)", RetVal, 0U);
    }
#endif
#endif

    SvcLog_DBG(SVC_LOG_CV_CTRL_TASK, "SvcCvCtrlTask_Init done", 0U, 0U);

    return RetVal;
}

/**
 *  Configure the CV channels
 *  @return error code
 */
UINT32 SvcCvCtrlTask_Config(void)
{
    UINT32 RetVal = SVC_OK;
    UINT32 i;
    SVC_CV_FLOW_UTIL_CFG_s UtilCfg = {0};
    const SVC_RES_CFG_s *pResCfg = SvcResCfg_Get();
    UINT32 CvFlowNum = pResCfg->CvFlowNum;
    UINT32 CvFlowBits = pResCfg->CvFlowBits;
    const SVC_CV_FLOW_s *pCvFlow = pResCfg->CvFlow;
    SVC_CV_FLOW_OBJ_s *pCvObj;
    SVC_CV_FLOW_CFG_s CvCfg;
    SVC_IMG_FIFO_INIT_CFG_s FIFOCfg = {0};
    SVC_IMG_FEEDER_INIT_CFG_s FeederCfg = {0};
    ULONG BufBase;
    static UINT8 InputUtilInit = 0U;

    SvcLog_DBG(SVC_LOG_CV_CTRL_TASK, "SvcCvCtrlTask_Config Start", 0U, 0U);

    if (CvFlowNum > 0U) {
        if (SVC_OK == SvcBuffer_Request(SVC_BUFFER_FIXED, FMEM_ID_CV_ALGO, &BufBase, &UtilCfg.MemSize)) {
            UtilCfg.MemBase = BufBase;
            if (SVC_OK != SvcCvFlow_UtilInit(&UtilCfg)) {
                SvcLog_DBG(SVC_LOG_CV_CTRL_TASK, "fail to SvcCvFlow_UtilInit", 0U, 0U);
            }
        } else {
            SvcLog_DBG(SVC_LOG_CV_CTRL_TASK, "fail to SvcBuffer_Request", 0U, 0U);
        }

        if (InputUtilInit == 0U) {
            InputUtilInit = 1U;
            if (SVC_OK == SvcImgFIFO_Init(&FIFOCfg)) {
                if (SVC_OK != SvcImgFeeder_Init(&FeederCfg)) {
                    SvcLog_DBG(SVC_LOG_CV_CTRL_TASK, "fail to SvcImgFeeder_Init", 0U, 0U);
                }
            } else {
                SvcLog_DBG(SVC_LOG_CV_CTRL_TASK, "fail to SvcImgFIFO_Init", 0U, 0U);
            }
        }

        /* Init CvCtrl CV Flow based on ResCfg */
        for (i = 0U; i < CvFlowNum; i++) {
            if ((CvFlowBits & ((UINT32) 1U << i)) > 0U) {
                AmbaMisra_TypeCast(&pCvObj, &(pCvFlow[i].pCvObj));
                RetVal = SvcCvFlow_Hook(i, pCvObj);

                if (SVC_OK == RetVal) {
                    RetVal = SvcCvFlow_Init(i);

                    if (SVC_OK == RetVal) {
                        RetVal = SvcCvCtrlTask_PackCvCfg(&pCvFlow[i], &CvCfg);

                        if (SVC_OK == RetVal) {
                            RetVal = SvcCvFlow_Config(i, &CvCfg);

                            if (SVC_OK != RetVal) {
                                SvcLog_NG(SVC_LOG_CV_CTRL_TASK, "SvcCvFlow_Config(%d) failed(0x%x)", i, RetVal);
                            }
                        } else {
                            SvcLog_NG(SVC_LOG_CV_CTRL_TASK, "SvcCvFlow(%d) pack info failed(0x%x)", i, RetVal);
                        }
                    } else {
                        SvcLog_NG(SVC_LOG_CV_CTRL_TASK, "SvcCvFlow_Init(%d) failed(0x%x)", i, RetVal);
                    }
                } else {
                    SvcLog_NG(SVC_LOG_CV_CTRL_TASK, "SvcCvFlow_Hook(%d) failed(0x%x)", i, RetVal);
                }
            }
        }

        if (SVC_OK == RetVal) {
            RetVal = AmbaKAL_EventFlagSet(&SvcCvCtrlEvt, SVC_CV_CTRL_EVT_CONFIG);
            if (SVC_OK != RetVal) {
                SvcLog_NG(SVC_LOG_CV_CTRL_TASK, "Set SVC_CV_CTRL_EVT_CONFIG flag failed(0x%x)", RetVal, 0U);
            }
        }

        CvCtrlTask_CvFlowStatusRegister();
    }

    SvcLog_DBG(SVC_LOG_CV_CTRL_TASK, "SvcCvCtrlTask_Config Done", 0U, 0U);

    return RetVal;
}

/**
 *  Load resource for CV channels
 *  @return error code
 */
UINT32 SvcCvCtrlTask_Load(void)
{
    UINT32 RetVal = SVC_OK;
    UINT32 i;
    const SVC_RES_CFG_s *pResCfg = SvcResCfg_Get();
    UINT32 CvFlowNum = pResCfg->CvFlowNum;
    UINT32 CvFlowBits = pResCfg->CvFlowBits;
    SVC_CV_FLOW_LOAD_INFO_s LoadInfo;
    UINT32 ActualFlags = 0U;
    SVC_CV_MAIN_BOOT_FLAG_s BootFlag;

    SvcLog_DBG(SVC_LOG_CV_CTRL_TASK, "SvcCvCtrlTask_Load Start", 0U, 0U);
    #if defined(CONFIG_ICAM_TIMING_LOG)
    SvcTime(SVC_TIME_CV_FLEXIDAG_LOAD_START, "CV FlexiDag load START");
    #endif

    if (CvFlowNum > 0U) {
        if (SVC_OK == SvcCvMainTask_GetBootFlag(&BootFlag)) {
            if (BootFlag.Source == SVC_CV_MAIN_BOOT_FLAG_ROMFS) {
                RetVal = AmbaKAL_EventFlagGet(&SvcCvCtrlEvt, SVC_CV_CTRL_EVT_CONFIG, AMBA_KAL_FLAGS_ANY, AMBA_KAL_FLAGS_CLEAR_NONE, &ActualFlags, AMBA_KAL_WAIT_FOREVER);
                if (SVC_OK != RetVal) {
                    SvcLog_NG(SVC_LOG_CV_CTRL_TASK, "Get SVC_CV_CTRL_EVT_CONFIG flag failed(0x%x)", RetVal, 0U);

                } else {
                    /* NumBin = 0. Decided by CvFlow app internally. */
                    RetVal = AmbaWrap_memset(&LoadInfo, 0, sizeof(SVC_CV_FLOW_LOAD_INFO_s));
                    for (i = 0U; i < CvFlowNum; i++) {
                        if ((CvFlowBits & ((UINT32) 1U << i)) > 0U) {
                            RetVal = SvcCvFlow_Load(i, &LoadInfo);
                            if (SVC_OK != RetVal) {
                                SvcLog_NG(SVC_LOG_CV_CTRL_TASK, "SvcCvFlow_Load failed(0x%x)", RetVal, 0U);
                                break;
                            }
                        }
                    }

                    if (SVC_OK == RetVal) {
                        RetVal = AmbaKAL_EventFlagSet(&SvcCvCtrlEvt, SVC_CV_CTRL_EVT_LOAD);
                        if (SVC_OK != RetVal) {
                            SvcLog_NG(SVC_LOG_CV_CTRL_TASK, "Set SVC_CV_CTRL_EVT_LOAD flag failed(0x%x)", RetVal, 0U);
                        }
                    }
                }
            } else {
                /* Load by user commands in CvMain_BootTask. */
            }
        } else {
            SvcLog_DBG(SVC_LOG_CV_CTRL_TASK, "GetBootFlag failed", 0U, 0U);
        }
    }

    #if defined(CONFIG_ICAM_TIMING_LOG)
    SvcTime(SVC_TIME_CV_FLEXIDAG_LOAD_DONE, "CV FlexiDag load DONE");
    #endif
    SvcLog_DBG(SVC_LOG_CV_CTRL_TASK, "SvcCvCtrlTask_Load Done", 0U, 0U);

    return RetVal;
}

/**
 *  Start the CV channels
 *  @return error code
 */
UINT32 SvcCvCtrlTask_Start(void)
{
    UINT32 RetVal = SVC_OK;
    UINT32 i;
    const SVC_RES_CFG_s *pResCfg = SvcResCfg_Get();
    UINT32 CvFlowNum = pResCfg->CvFlowNum;
    UINT32 CvFlowBits = pResCfg->CvFlowBits;
    UINT32 ActualFlags = 0U;

    SvcLog_DBG(SVC_LOG_CV_CTRL_TASK, "SvcCvCtrlTask_Start Start", 0U, 0U);

    if (CvFlowNum > 0U) {
        RetVal = AmbaKAL_EventFlagGet(&SvcCvCtrlEvt, SVC_CV_CTRL_EVT_LOAD, AMBA_KAL_FLAGS_ANY, AMBA_KAL_FLAGS_CLEAR_NONE, &ActualFlags, AMBA_KAL_WAIT_FOREVER);
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_CV_CTRL_TASK, "Get SVC_CV_CTRL_EVT_LOAD flag failed(0x%x)", RetVal, 0U);

        } else {
            /* Register and start CvCtrl */
            for (i = 0U; i < CvFlowNum; i++) {
                if ((CvFlowBits & ((UINT32) 1U << i)) > 0U) {
                    RetVal |= SvcCvFlow_Start(i, NULL);
                    if (SVC_OK != RetVal) {
                        SvcLog_NG(SVC_LOG_CV_CTRL_TASK, "SvcCvFlow_Start(%d) failed(0x%x)", i, RetVal);
                    }
                }
            }
        }
    }

    SvcLog_DBG(SVC_LOG_CV_CTRL_TASK, "SvcCvCtrlTask_Start Done", 0U, 0U);

    return RetVal;
}

/**
 *  Stop the CV channels
 *  @return error code
 */
UINT32 SvcCvCtrlTask_Stop(void)
{
    UINT32 RetVal = SVC_OK;
    UINT32 i;
    const SVC_RES_CFG_s *pResCfg = SvcResCfg_Get();
    UINT32 CvFlowNum = pResCfg->CvFlowNum;
    UINT32 CvFlowBits = pResCfg->CvFlowBits;

    SvcLog_DBG(SVC_LOG_CV_CTRL_TASK, "SvcCvCtrlTask_Stop Start", 0U, 0U);

    if (CvFlowNum > 0U) {
        /* Stop CvCtrl */
        for (i = 0U; i < CvFlowNum; i++) {
            if ((CvFlowBits & ((UINT32) 1U << i)) > 0U) {
                RetVal |= SvcCvFlow_Stop(i, NULL);
                if (SVC_OK == RetVal) {
                     RetVal = SvcCvFlow_UnHook(i);
                    if (SVC_OK != RetVal) {
                        SvcLog_NG(SVC_LOG_CV_CTRL_TASK, "SvcCvFlow_UnHook(%d) failed(0x%x)", i, RetVal);
                    }
                } else {
                    SvcLog_NG(SVC_LOG_CV_CTRL_TASK, "SvcCvFlow_Stop(%d) failed(0x%x)", i, RetVal);
                }
            }
        }

        if (SVC_OK != SvcCvFlow_UtilDeInit()) {
            SvcLog_DBG(SVC_LOG_CV_CTRL_TASK, "fail to SvcCvFlow_UtilDeInit", 0U, 0U);
        }

        if (SVC_OK != AmbaKAL_EventFlagClear(&SvcCvCtrlEvt, 0xFFFFFFFFU)) {
            SvcLog_NG(SVC_LOG_CV_CTRL_TASK, "Clear Event Flag failed", 0U, 0U);
        }

        CvCtrlTask_CvFlowStatusUnregister();
        CvFlowRdyBits = 0U;
    }

    SvcLog_DBG(SVC_LOG_CV_CTRL_TASK, "SvcCvCtrlTask_Stop Done", 0U, 0U);

    return RetVal;
}

/**
 *  Enable/Disable the CV channels
 *  @param[in] Enable enable/disable CvCtrlTask
 *  @return error code
 */
UINT32 SvcCvCtrlTask_Enable(UINT8 Enable)
{
    UINT32 RetVal = SVC_OK;
    UINT32 i;
    const SVC_RES_CFG_s *pResCfg = SvcResCfg_Get();
    UINT32 CvFlowNum = pResCfg->CvFlowNum;
    UINT32 CvFlowBits = pResCfg->CvFlowBits;

    SvcLog_DBG(SVC_LOG_CV_CTRL_TASK, "SvcCvCtrlTask_Enable Start", 0U, 0U);

    /* Enable/Disable CvCtrl */
    for (i = 0U; i < CvFlowNum; i++) {
        if ((CvFlowBits & ((UINT32) 1U << i)) > 0U) {
            RetVal |= SvcCvFlow_Control(i, SVC_CV_CTRL_ENABLE, &Enable);
            if (SVC_OK != RetVal) {
                SvcLog_NG(SVC_LOG_CV_CTRL_TASK, "SvcCvFlow_Control(%d) failed(0x%x)", i, RetVal);
            }
        }
    }

    SvcLog_DBG(SVC_LOG_CV_CTRL_TASK, "SvcCvCtrlTask_Enable Done", 0U, 0U);

    return RetVal;
}

static void CvCtrlTask_CvBootStatusCb(UINT32 StatIdx, void *pInfo)
{
    UINT32 RetVal = SVC_OK;
    const SVC_APP_STAT_CV_BOOT_s *pCvStatus;
    SVC_CV_MAIN_BOOT_FLAG_s BootFlag;

    AmbaMisra_TypeCast(&pCvStatus, &pInfo);
    if (pCvStatus->Status == SVC_APP_STAT_CV_BOOT_DONE) {
        if (SVC_OK == SvcCvMainTask_GetBootFlag(&BootFlag)) {
            if ((BootFlag.Source == SVC_CV_MAIN_BOOT_FLAG_SD) ||
                (BootFlag.Source == SVC_CV_MAIN_BOOT_FLAG_CMDMODE)) {
                /* Load by user commands in CvMain_BootTask. */
                RetVal = AmbaKAL_EventFlagSet(&SvcCvCtrlEvt, SVC_CV_CTRL_EVT_LOAD);
                if (SVC_OK != RetVal) {
                    SvcLog_NG(SVC_LOG_CV_CTRL_TASK, "Set SVC_CV_CTRL_EVT_LOAD flag failed(0x%x)", RetVal, 0U);
                }
            } else {
                /* Load by SvcCvCtrlTask_Load. */
            }
        } else {
            SvcLog_DBG(SVC_LOG_CV_CTRL_TASK, "MainTaskStatus: GetBootFlag failed", 0U, 0U);
        }
    }

    AmbaMisra_TouchUnused(&StatIdx);
    AmbaMisra_TouchUnused(pInfo);
}

static void CvCtrlTask_CvFlowStatusRegister(void)
{
    UINT32 RetVal = SVC_OK;
    const SVC_RES_CFG_s *pResCfg = SvcResCfg_Get();
    UINT32 CvFlowNum = pResCfg->CvFlowNum;
    UINT32 CvFlowBits = pResCfg->CvFlowBits;

    for (UINT32 i = 0U; i < CvFlowNum; i++) {
        if ((CvFlowBits & ((UINT32) 1U << i)) > 0U) {
            RetVal = SvcCvFlow_StatusEventRegister(i, CvCtrlTask_CvFlowStatusCb, &CvFlowStatusID[i]);
            if (SVC_OK != RetVal) {
                SvcLog_NG(SVC_LOG_CV_CTRL_TASK, "SvcCvFlow_StatusEventRegister(%d) failed(0x%x)", i, RetVal);
            }
        }
    }
}

static void CvCtrlTask_CvFlowStatusUnregister(void)
{
    UINT32 RetVal = SVC_OK;
    const SVC_RES_CFG_s *pResCfg = SvcResCfg_Get();
    UINT32 CvFlowNum = pResCfg->CvFlowNum;
    UINT32 CvFlowBits = pResCfg->CvFlowBits;

    for (UINT32 i = 0U; i < CvFlowNum; i++) {
        if ((CvFlowBits & ((UINT32) 1U << i)) > 0U) {
            RetVal = SvcCvFlow_StatusEventUnregister(i, CvFlowStatusID[i]);
            if (SVC_OK != RetVal) {
                SvcLog_NG(SVC_LOG_CV_CTRL_TASK, "SvcCvFlow_StatusEventUnregister(%d) failed(0x%x)", i, RetVal);
            }
        }
    }
}

static void CvCtrlTask_CvFlowStatusCb(UINT32 CvFlowChan, UINT32 Event, void *pInfo)
{
    const SVC_RES_CFG_s *pResCfg = SvcResCfg_Get();
    UINT32 CvFlowBits = pResCfg->CvFlowBits;
    SVC_CV_MAIN_BOOT_FLAG_s BootFlag;

    if (Event == SVC_CV_EVT_IPC_CHAN_READY) {
        /* DualOS - Linux FlexiDAG */
        CvFlowRdyBits |= ((UINT32)1U << CvFlowChan);
        if (CvFlowRdyBits == CvFlowBits) {
            if (SVC_OK == SvcCvMainTask_GetBootFlag(&BootFlag)) {
                if ((BootFlag.Source == SVC_CV_MAIN_BOOT_FLAG_CMDMODE) &&
                    (BootFlag.Flag == SVC_CV_MAIN_BOOT_FLAG_AUTO_CONTINUE)) {
                    if (SVC_OK != SvcCvMainTask_Ctrl("continue", NULL)) {
                        SvcLog_NG(SVC_LOG_CV_CTRL_TASK, "CvFlowStatusCb: SvcCvMainTask_Ctrl failed", 0U, 0U);
                    }
                }
            } else {
                SvcLog_NG(SVC_LOG_CV_CTRL_TASK, "CvFlowStatusCb: GetBootFlag failed", 0U, 0U);
            }
        }
    }

    AmbaMisra_TouchUnused(pInfo);
}

#ifdef CONFIG_AMBALINK_BOOT_OS
static void CvCtrlTask_LinuxBootStatusCb(UINT32 StatIdx, void *pInfo)
{
    UINT32 RetVal;
    const SVC_APP_STAT_LINUX_s *pLinuxStatus;

    AmbaMisra_TypeCast(&pLinuxStatus, &pInfo);
    if (0U < (pLinuxStatus->Status & SVC_APP_STAT_LINUX_READY)) {
        RetVal = SvcCv_FlexiIOSysEventNotify(SVC_CV_FLEXIDAG_IO_IPC_READY);
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_CV_CTRL_TASK, "FlexiIOSysEventNotify failed(0x%x)", RetVal, 0U);
        }
    }

    AmbaMisra_TouchUnused(&StatIdx);
    AmbaMisra_TouchUnused(pInfo);
}
#endif

