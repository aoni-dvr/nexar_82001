/**
 *  @file SvcCvFlow.c
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
 *  @details Implementation of SvcCvFlow. SvcCvFlow is the interface to control Cv Flow, i.e., control SuperDAG life cycle
 *
 */

#include "AmbaTypes.h"

#include "AmbaDSP.h"
#include "AmbaDSP_Liveview.h"
#include "AmbaNAND.h"
#include "AmbaUtility.h"
#include "AmbaWrap.h"
#include "AmbaCache.h"

#include "SvcErrCode.h"
#include "SvcLog.h"
#include "SvcWrap.h"

#include "SvcCvAlgo.h"
#include "SvcResCfg.h"
#include "SvcCvFlow.h"
#include "SvcCvFlowAlgoUtil.h"

#define SVC_LOG_CV_FLOW         "CV_FLOW"

typedef struct {
    SVC_CV_FLOW_EVENT_f     pEventCallback;
    UINT32                  EventID;
} SVC_CV_FLOW_EVENT_INFO_s;

typedef struct {
    SVC_CV_FLOW_EVENT_INFO_s EventInfo[SVC_CV_FLOW_EVENT_MAX];
} SVC_CV_FLOW_EVENT_PORT_INFO_s;

typedef struct {
    SVC_CV_FLOW_EVENT_PORT_INFO_s PortInfo[SVC_CV_FLOW_OUTPUT_PORT_MAX];
} SVC_CV_FLOW_EVENT_CHAN_s;

typedef struct {
    SVC_CV_FLOW_STATUS_EVENT_f  pEventCallback;
    UINT32                      EventID;
} SVC_CV_FLOW_STATUS_EVENT_INFO_s;

typedef struct {
    SVC_CV_FLOW_STATUS_EVENT_INFO_s EventInfo[SVC_CV_FLOW_EVENT_MAX];
} SVC_CV_FLOW_STATUS_EVENT_CHAN_s;

static UINT32 SvcCvFlowDebugEnable = 1U;
static SVC_CV_FLOW_OBJ_s *pCvFlowObj[SVC_CV_FLOW_CHAN_MAX];
static SVC_CV_FLOW_EVENT_CHAN_s CvFlowEvent[SVC_CV_FLOW_CHAN_MAX] GNU_SECTION_NOZEROINIT;
static SVC_CV_FLOW_STATUS_EVENT_CHAN_s CvFlowStatusEvent[SVC_CV_FLOW_CHAN_MAX] GNU_SECTION_NOZEROINIT;

static UINT32 CvFlow_GetEmptyEventInfo(UINT32 CvFlowChan, UINT32 OutputPort, SVC_CV_FLOW_EVENT_INFO_s **pEmptyInfo);
static UINT32 CvFlow_GetEventInfo(UINT32 CvFlowChan, UINT32 EventID, SVC_CV_FLOW_EVENT_INFO_s **pInfo);
static UINT32 CvFlow_GetEmptyStatusEventInfo(UINT32 CvFlowChan, SVC_CV_FLOW_STATUS_EVENT_INFO_s **pEmptyInfo);
static UINT32 CvFlow_GetStatusEventInfo(UINT32 CvFlowChan, UINT32 EventID, SVC_CV_FLOW_STATUS_EVENT_INFO_s **pInfo);

static void CvFlow_Dbg(const char *pFormat, UINT32 Arg1, UINT32 Arg2)
{
    if (SvcCvFlowDebugEnable > 0U) {
        SvcLog_OK(SVC_LOG_CV_FLOW, pFormat, Arg1, Arg2);
    }
}

static void CvFlow_Err(const char *pFormat, UINT32 Arg1, UINT32 Arg2)
{
    if (SvcCvFlowDebugEnable > 0U) {
        SvcLog_NG(SVC_LOG_CV_FLOW, pFormat, Arg1, Arg2);
    }
}

static inline UINT32 CvFlow_GetEventID(UINT32 CvFlowChan, UINT32 Port, UINT32 Index)
{
    return ((CvFlowChan << (UINT32) 16U) | (Port << (UINT32) 8U) | Index);
}

static inline UINT32 CvFlow_GetOutputPort(UINT32 CvFlowChan)
{
    return ((CvFlowChan >> 8U) & 0x000000FFU);
}

/**
* Hook cvflow object on the cvflow channel
* @param [in] CvFlowChan cvflow channel
* @param [in] pCvObj cvflow object
* @return 0-OK, 1-NG
*/
UINT32 SvcCvFlow_Hook(UINT32 CvFlowChan, SVC_CV_FLOW_OBJ_s *pCvObj)
{
    UINT32 RetVal = SVC_OK;
    UINT32 i;

    if (CvFlowChan < SVC_CV_FLOW_CHAN_MAX) {
        if (pCvObj != NULL) {
            if (pCvFlowObj[CvFlowChan] == NULL) {
                for (i = 0U; i < SVC_CV_FLOW_OUTPUT_PORT_MAX; i++) {
                    RetVal = AmbaWrap_memset(&CvFlowEvent[CvFlowChan], 0, sizeof(SVC_CV_FLOW_EVENT_CHAN_s));
                    if (SVC_OK != RetVal) {
                        CvFlow_Err("CvFlowChan (%d) memset event failed", CvFlowChan, 0U);
                    }
                }

                RetVal = AmbaWrap_memset(&CvFlowStatusEvent[CvFlowChan], 0, sizeof(SVC_CV_FLOW_STATUS_EVENT_CHAN_s));
                if (SVC_OK != RetVal) {
                    CvFlow_Err("CvFlowChan (%d) memset status failed", CvFlowChan, 0U);
                }

                pCvFlowObj[CvFlowChan] = pCvObj;
                RetVal = SVC_OK;
            } else {
                CvFlow_Dbg("CvFlowChan (%d) has been occupied, please check the CvFlowChan usage", CvFlowChan, 0U);
                RetVal = SVC_NG;
            }
        } else {
            CvFlow_Err("pCvObj = NULL", 0U, 0U);
            RetVal = SVC_NG;
        }
    } else {
        CvFlow_Err("CvFlowChan (%d) over the maximum (%d)", CvFlowChan, SVC_CV_FLOW_CHAN_MAX);
    }

    if (SVC_OK == RetVal) {
        ULONG Addr;
        AmbaMisra_TypeCast(&Addr, &pCvObj);
        SvcCvFlow_PrintULong("SvcCvFlow_Hook CvFlowChan (%d) succeed with Obj (0x%x)", CvFlowChan, Addr, 0U, 0U, 0U);
    }

    return RetVal;
}

/**
* Unhook cv flow object from the cvflow channel
* @param [in] CvFlowChan cvflow channel
* @return 0-OK, 1-NG
*/
UINT32 SvcCvFlow_UnHook(UINT32 CvFlowChan)
{
    UINT32 RetVal = SVC_OK;

    if (CvFlowChan < SVC_CV_FLOW_CHAN_MAX) {
            if (pCvFlowObj[CvFlowChan] != NULL) {
                pCvFlowObj[CvFlowChan] = NULL;
                RetVal = SVC_OK;
            } else {
                CvFlow_Dbg("CvFlowChan (%d) is not hooked before, please check setting", CvFlowChan, 0U);
                RetVal = SVC_NG;
            }
    } else {
        CvFlow_Err("CvFlowChan (%d) over the maximum (%d)", CvFlowChan, SVC_CV_FLOW_CHAN_MAX);
    }

    return RetVal;
}

/**
* Register output callback function to receive output of the cvflow channel
* @param [in] CvFlowChan cvflow channel
* @param [in] pEventCallback user defined callback function
* @param [out] pRegisterID ID for user to identify the callback function
* @return 0-OK, 1-NG
*/
UINT32 SvcCvFlow_Register(UINT32 CvFlowChan, SVC_CV_FLOW_EVENT_f pEventCallback, UINT32 *pRegisterID)
{
    return SvcCvFlow_RegisterEx(CvFlowChan, SVC_CV_FLOW_OUTPUT_PORT0, pEventCallback, pRegisterID);
}

/**
* Register output callback function to receive output of the cvflow channel
* @param [in] CvFlowChan cvflow channel
* @param [in] OutputPort output port of the channel
* @param [in] pEventCallback user defined callback function
* @param [out] pRegisterID ID for user to identify the callback function
* @return 0-OK, 1-NG
*/
UINT32 SvcCvFlow_RegisterEx(UINT32 CvFlowChan, UINT32 OutputPort, SVC_CV_FLOW_EVENT_f pEventCallback, UINT32 *pRegisterID)
{
    UINT32 RetVal;
    SVC_CV_FLOW_EVENT_INFO_s *pEmptyInfo = NULL;

    RetVal = CvFlow_GetEmptyEventInfo(CvFlowChan, OutputPort, &pEmptyInfo);

    if (SVC_OK == RetVal) {
        if (pEmptyInfo != NULL) {
            pEmptyInfo->pEventCallback = pEventCallback;
            *pRegisterID = pEmptyInfo->EventID;
        } else {
            CvFlow_Err("SvcCvFlow_Register failed. Cannot get empty event info for register", 0U, 0U);
            RetVal = SVC_NG;
        }
    } else {
        CvFlow_Err("SvcCvFlow_Register failed. CvFlowChan (%d) Get empty event info err (%d)", CvFlowChan, RetVal);
    }

    if (SVC_OK == RetVal) {
        CvFlow_Dbg("SvcCvFlow_Register succeed (%d)", CvFlowChan, 0U);
    }

    return RetVal;
}

/**
* Unregister output callback function from the cvflow channel
* @param [in] CvFlowChan cvflow channel
* @param [in] RegisterID ID of the callback function returned by SvcCvFlow_Register
* @return 0-OK, 1-NG
*/
UINT32 SvcCvFlow_Unregister(UINT32 CvFlowChan, UINT32 RegisterID)
{
    UINT32 RetVal;
    SVC_CV_FLOW_EVENT_INFO_s *pInfo = NULL;

    RetVal = CvFlow_GetEventInfo(CvFlowChan, RegisterID, &pInfo);

    if (SVC_OK == RetVal) {
        if (pInfo != NULL) {
            pInfo->pEventCallback = NULL;
            pInfo->EventID = 0U;
        } else {
            CvFlow_Err("SvcCvFlow_Unregister failed. Cannot get info for unregister with chan (%d) and RegisterID (%d)", CvFlowChan, RegisterID);
            RetVal = SVC_NG;
        }
    } else {
        CvFlow_Err("SvcCvFlow_Unregister failed. CvFlowChan (%d) Get info err (%d)", CvFlowChan, RetVal);
    }

    return RetVal;
}

/**
* Initialize the cvflow channel
* @param [in] CvFlowChan cvflow channel
* @return 0-OK, 1-NG
*/
UINT32 SvcCvFlow_Init(UINT32 CvFlowChan)
{
    UINT32 RetVal;

    if (pCvFlowObj[CvFlowChan] != NULL) {
        RetVal = pCvFlowObj[CvFlowChan]->Init(CvFlowChan, NULL);
    } else {
        CvFlow_Err("SvcCvFlow_Init failed. CvFlowChan(%d) has no object hooked", CvFlowChan, 0U);
        RetVal = SVC_NG;
    }

    return RetVal;
}

/**
* Configure the cvflow channel
* @param [in] CvFlowChan cvflow channel
* @param [in] pConfig configuration of cvflow channel, depending on driver
* @return 0-OK, 1-NG
*/
UINT32 SvcCvFlow_Config(UINT32 CvFlowChan, void *pConfig)
{
    UINT32 RetVal;

    if (pCvFlowObj[CvFlowChan] != NULL) {
        RetVal = pCvFlowObj[CvFlowChan]->Config(CvFlowChan, pConfig);
    } else {
        CvFlow_Err("SvcCvFlow_Config failed. CvFlowChan(%d) has no object hooked", CvFlowChan, 0U);
        RetVal = SVC_NG;
    }

    return RetVal;
}

/**
* Load resource for the cvflow channel
* @param [in] CvFlowChan cvflow channel
* @param [in] pInfo load configuration, depending on driver
* @return 0-OK, 1-NG
*/
UINT32 SvcCvFlow_Load(UINT32 CvFlowChan, void *pInfo)
{
    UINT32 RetVal;

    if (pCvFlowObj[CvFlowChan] != NULL) {
        RetVal = pCvFlowObj[CvFlowChan]->Load(CvFlowChan, pInfo);
    } else {
        CvFlow_Err("SvcCvFlow_Load failed. CvFlowChan(%d) has no object hooked", CvFlowChan, 0U);
        RetVal = SVC_NG;
    }

    return RetVal;
}

/**
* Control interface of the cvflow channel
* @param [in] CvFlowChan cvflow channel
* @param [in] CtrlType control type, depending on driver
* @param [in] pCtrl control configuration, depending on driver
* @return 0-OK, 1-NG
*/
UINT32 SvcCvFlow_Control(UINT32 CvFlowChan, UINT32 CtrlType, const void *pCtrl)
{
    UINT32 RetVal;

    if (pCvFlowObj[CvFlowChan] != NULL) {
        RetVal = pCvFlowObj[CvFlowChan]->Control(CvFlowChan, CtrlType, pCtrl);
    } else {
        CvFlow_Err("SvcCvFlow_Control failed. CvFlowChan(%d) has no object hooked", CvFlowChan, 0U);
        RetVal = SVC_NG;
    }

    return RetVal;
}

/**
* Start the cvflow channel
* @param [in] CvFlowChan cvflow channel
* @param [in] pInput start configuration, depending on driver
* @return 0-OK, 1-NG
*/
UINT32 SvcCvFlow_Start(UINT32 CvFlowChan, void *pInput)
{
    UINT32 RetVal;

    if (pCvFlowObj[CvFlowChan] != NULL) {
        RetVal = pCvFlowObj[CvFlowChan]->Start(CvFlowChan, pInput);
    } else {
        CvFlow_Err("SvcCvFlow_Start failed. CvFlowChan(%d) has no object hooked", CvFlowChan, 0U);
        RetVal = SVC_NG;
    }

    return RetVal;
}

/**
* Stop the cvflow channel
* @param [in] CvFlowChan cvflow channel
* @param [in] pInput stop configuration, depending on driver
* @return 0-OK, 1-NG
*/
UINT32 SvcCvFlow_Stop(UINT32 CvFlowChan, void *pInput)
{
    UINT32 RetVal;

    if (pCvFlowObj[CvFlowChan] != NULL) {
        RetVal = pCvFlowObj[CvFlowChan]->Stop(CvFlowChan, pInput);
    } else {
        CvFlow_Err("SvcCvFlow_Stop failed. CvFlowChan(%d) has no object hooked", CvFlowChan, 0U);
        RetVal = SVC_NG;
    }

    return RetVal;
}

/**
* Debug interface of the cvflow channel
* @param [in] CvFlowChan cvflow channel
* @param [in] pCmd command string, depending on driver
* @param [in] Param0 parameter
* @param [in] Param1 parameter
* @param [in] Param2 parameter
* @return 0-OK, 1-NG
*/
UINT32 SvcCvFlow_Debug(UINT32 CvFlowChan, const char *pCmd, UINT32 Param0, UINT32 Param1, UINT32 Param2)
{
    UINT32 RetVal;

    if (pCvFlowObj[CvFlowChan] != NULL) {
        RetVal = pCvFlowObj[CvFlowChan]->Debug(CvFlowChan, pCmd, Param0, Param1, Param2);
    } else {
        CvFlow_Err("SvcCvFlow_Debug failed. CvFlowChan(%d) has no object hooked", CvFlowChan, 0U);
        RetVal = SVC_NG;
    }

    return RetVal;
}

/**
* Output interface of driver implementation. issue output data to the output callback function.
* @param [in] CvFlowChan cvflow channel
* @param [in] OutputPort output port of the channel
* @param [in] OutputTag user defined output tag
* @param [in] pOutput output data
* @return 0-OK, 1-NG
*/
UINT32 SvcCvFlow_Issue(UINT32 CvFlowChan, UINT32 OutputPort, UINT32 OutputTag, void *pOutput)
{
    UINT32 RetVal;
    UINT32 i;
    const SVC_CV_FLOW_EVENT_PORT_INFO_s *pPortInfo;

    if (CvFlowChan < SVC_CV_FLOW_CHAN_MAX) {
        if (OutputPort < SVC_CV_FLOW_OUTPUT_PORT_MAX) {
            pPortInfo = &CvFlowEvent[CvFlowChan].PortInfo[OutputPort];
            for (i = 0U; i < SVC_CV_FLOW_EVENT_MAX; i++) {
                if (pPortInfo->EventInfo[i].pEventCallback != NULL) {
                    pPortInfo->EventInfo[i].pEventCallback(CvFlowChan, OutputTag, pOutput);
                }
            }
            RetVal = SVC_OK;
        } else {
            RetVal = SVC_NG;
        }
    } else {
        RetVal = SVC_NG;
    }

    return RetVal;
}

static UINT32 CvFlow_GetEmptyEventInfo(UINT32 CvFlowChan, UINT32 OutputPort, SVC_CV_FLOW_EVENT_INFO_s **pEmptyInfo)
{
    UINT32 RetVal;
    UINT32 i;
    SVC_CV_FLOW_EVENT_PORT_INFO_s *pPortInfo;

    if (CvFlowChan < SVC_CV_FLOW_CHAN_MAX) {
        if (OutputPort < SVC_CV_FLOW_OUTPUT_PORT_MAX) {
            pPortInfo = &CvFlowEvent[CvFlowChan].PortInfo[OutputPort];
            for (i = 0U; i < SVC_CV_FLOW_EVENT_MAX; i++) {
                if (pPortInfo->EventInfo[i].pEventCallback == NULL) {
                    pPortInfo->EventInfo[i].EventID = CvFlow_GetEventID(CvFlowChan, OutputPort, i);
                    *pEmptyInfo = &pPortInfo->EventInfo[i];
                    break;
                }
            }
            RetVal = SVC_OK;
        } else {
            RetVal = SVC_NG;
        }
    } else {
        RetVal = SVC_NG;
    }

    return RetVal;
}

static UINT32 CvFlow_GetEventInfo(UINT32 CvFlowChan, UINT32 EventID, SVC_CV_FLOW_EVENT_INFO_s **pInfo)
{
    UINT32 RetVal;
    UINT32 i;
    UINT32 OutputPort;
    SVC_CV_FLOW_EVENT_PORT_INFO_s *pPortInfo;

    if (CvFlowChan < SVC_CV_FLOW_CHAN_MAX) {
        OutputPort = CvFlow_GetOutputPort(EventID);
        if (OutputPort < SVC_CV_FLOW_OUTPUT_PORT_MAX) {
            pPortInfo = &CvFlowEvent[CvFlowChan].PortInfo[OutputPort];
            for (i = 0U; i < SVC_CV_FLOW_EVENT_MAX; i++) {
                if (pPortInfo->EventInfo[i].EventID == EventID) {
                    *pInfo = &pPortInfo->EventInfo[i];
                    break;
                }
            }
            RetVal = SVC_OK;
        } else {
            RetVal = SVC_NG;
        }
    } else {
        RetVal = SVC_NG;
    }

    return RetVal;
}

/**
* Register callback function to receive status event of the cvflow channel
* @param [in] CvFlowChan cvflow channel
* @param [in] pEventCallback user defined callback function
* @param [out] pRegisterID ID for user to identify the callback function
* @return 0-OK, 1-NG
*/
UINT32 SvcCvFlow_StatusEventRegister(UINT32 CvFlowChan, SVC_CV_FLOW_STATUS_EVENT_f pEventCallback, UINT32 *pRegisterID)
{
    UINT32 RetVal;
    SVC_CV_FLOW_STATUS_EVENT_INFO_s *pEmptyInfo = NULL;

    RetVal = CvFlow_GetEmptyStatusEventInfo(CvFlowChan, &pEmptyInfo);

    if (SVC_OK == RetVal) {
        if (pEmptyInfo != NULL) {
            pEmptyInfo->pEventCallback = pEventCallback;
            *pRegisterID = pEmptyInfo->EventID;
        } else {
            CvFlow_Err("SvcCvFlow_StatusEventRegister failed. Cannot get empty event info for register", 0U, 0U);
            RetVal = SVC_NG;
        }
    } else {
        CvFlow_Err("SvcCvFlow_StatusEventRegister failed. CvFlowChan (%d) Get empty event info err (%d)", CvFlowChan, RetVal);
    }

    if (SVC_OK == RetVal) {
        CvFlow_Dbg("SvcCvFlow_StatusEventRegister succeed (%d)", CvFlowChan, 0U);
    }

    return RetVal;
}

/**
* Unregister status event callback function from the cvflow channel
* @param [in] CvFlowChan cvflow channel
* @param [in] RegisterID ID of the callback function returned by SvcCvFlow_StatusEventRegister
* @return 0-OK, 1-NG
*/
UINT32 SvcCvFlow_StatusEventUnregister(UINT32 CvFlowChan, UINT32 RegisterID)
{
    UINT32 RetVal;
    SVC_CV_FLOW_STATUS_EVENT_INFO_s *pInfo = NULL;

    RetVal = CvFlow_GetStatusEventInfo(CvFlowChan, RegisterID, &pInfo);

    if (SVC_OK == RetVal) {
        if (pInfo != NULL) {
            pInfo->pEventCallback = NULL;
            pInfo->EventID = 0U;
        } else {
            CvFlow_Err("SvcCvFlow_StatusEventUnregister failed. Cannot get info for unregister with chan (%d) and RegisterID (%d)", CvFlowChan, RegisterID);
            RetVal = SVC_NG;
        }
    } else {
        CvFlow_Err("SvcCvFlow_StatusEventUnregister failed. CvFlowChan (%d) Get info err (%d)", CvFlowChan, RetVal);
    }

    return RetVal;
}

/**
* Status event interface of driver implementation. issue status event to the status event callback function.
* @param [in] CvFlowChan cvflow channel
* @param [in] Event event, depending on driver
* @param [in] pInfo content of the event
* @return 0-OK, 1-NG
*/
UINT32 SvcCvFlow_StatusEventIssue(UINT32 CvFlowChan, UINT32 Event, void *pInfo)
{
    UINT32 RetVal;
    UINT32 i;

    if (CvFlowChan < SVC_CV_FLOW_CHAN_MAX) {
        for (i = 0U; i < SVC_CV_FLOW_EVENT_MAX; i++) {
            if (CvFlowStatusEvent[CvFlowChan].EventInfo[i].pEventCallback != NULL) {
                CvFlowStatusEvent[CvFlowChan].EventInfo[i].pEventCallback(CvFlowChan, Event, pInfo);
            }
        }
        RetVal = SVC_OK;
    } else {
        RetVal = SVC_NG;
    }

    return RetVal;
}

static UINT32 CvFlow_GetEmptyStatusEventInfo(UINT32 CvFlowChan, SVC_CV_FLOW_STATUS_EVENT_INFO_s **pEmptyInfo)
{
    UINT32 RetVal;
    UINT32 i;

    if (CvFlowChan < SVC_CV_FLOW_CHAN_MAX) {
        for (i = 0U; i < SVC_CV_FLOW_EVENT_MAX; i++) {
            if (CvFlowStatusEvent[CvFlowChan].EventInfo[i].pEventCallback == NULL) {
                CvFlowStatusEvent[CvFlowChan].EventInfo[i].EventID = CvFlow_GetEventID(CvFlowChan, 0U, i);
                *pEmptyInfo = &CvFlowStatusEvent[CvFlowChan].EventInfo[i];
                break;
            }
        }
        RetVal = SVC_OK;
    } else {
        RetVal = SVC_NG;
    }

    return RetVal;
}

static UINT32 CvFlow_GetStatusEventInfo(UINT32 CvFlowChan, UINT32 EventID, SVC_CV_FLOW_STATUS_EVENT_INFO_s **pInfo)
{
    UINT32 RetVal;
    UINT32 i;

    if (CvFlowChan < SVC_CV_FLOW_CHAN_MAX) {
        for (i = 0U; i < SVC_CV_FLOW_EVENT_MAX; i++) {
            if (CvFlowStatusEvent[CvFlowChan].EventInfo[i].EventID == EventID) {
                *pInfo = &CvFlowStatusEvent[CvFlowChan].EventInfo[i];
                break;
            }
        }
        RetVal = SVC_OK;
    } else {
        RetVal = SVC_NG;
    }

    return RetVal;
}

