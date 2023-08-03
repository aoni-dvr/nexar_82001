/**
*  @file SvcSysStat.c
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
*  @details svc system status
*
*/

#include "AmbaKAL.h"
#include "AmbaWrap.h"
#include "AmbaMisraFix.h"
#include "AmbaUtility.h"

/* svc-framework */
#include "SvcErrCode.h"
#include "SvcLog.h"
#include "SvcWrap.h"

/* app-shared */
#include "SvcSysStat.h"

static SVC_SYS_STAT_CALLBACK_s *pSvcSysStatCallbackList = NULL;
static UINT32 SvcSysStatNum = 0U;
static UINT32 SvcSysStatInit = 0U;
static UINT32 SvcSysStatMaxStatusSize = 0U;
static ULONG  SvcSysStatStatusAddr = 0U;
/**
* initialization of system status module
* @param [in] StatNum number of status flag
* @return 0-OK, 1-NG
*/
UINT32 SvcSysStat_Init(UINT32 StatNum)
{
    SvcSysStatNum = StatNum;
    SvcSysStatInit = 1U;

    return SVC_OK;
}

/**
* configuration of system status flags
* @param [in] CallbackMemBase base of needed buffer
* @param [in] BufSize size of needed buffer
* @return 0-OK, 1-NG
*/
UINT32 SvcSysStat_Config(ULONG CallbackMemBase, UINT32 BufSize)
{
    UINT32 RetVal = SVC_OK;

    if ((CallbackMemBase != 0U) && (BufSize >= ( ((UINT32)sizeof(SVC_SYS_STAT_CALLBACK_s) * SvcSysStatNum) + SvcSysStatMaxStatusSize )) ) {
        RetVal = SVC_OK;
    } else {
        SvcLog_NG("SYS_STAT", "CallbackMemBase should not be 0 or BufSize too small (%d) < (%d)", BufSize, ( (UINT32)sizeof(SVC_SYS_STAT_CALLBACK_s) * SvcSysStatNum));
        RetVal = SVC_NG;
    }

    if (SVC_OK == RetVal) {
        AmbaMisra_TypeCast(&pSvcSysStatCallbackList, &CallbackMemBase);
        SvcSysStatStatusAddr = CallbackMemBase + ( sizeof(SVC_SYS_STAT_CALLBACK_s) * SvcSysStatNum);
        if (SVC_OK != AmbaWrap_memset(pSvcSysStatCallbackList, 0, BufSize)) {
            SvcLog_NG("SYS_STAT", "Init SysStatCallbackList to 0 failed", 0U, 0U);
        }
    } else {
        const void *pPointer = NULL;
        AmbaMisra_TypeCast(&pPointer, &CallbackMemBase);
        SVC_WRAP_PRINT "Buffer Request (Base %p, Size %d) failed"
            SVC_PRN_ARG_S "SYS_STAT"
            SVC_PRN_ARG_PROC SvcLog_NG
            SVC_PRN_ARG_CPOINT pPointer SVC_PRN_ARG_POST
            SVC_PRN_ARG_UINT32 BufSize  SVC_PRN_ARG_POST
            SVC_PRN_ARG_E

    }

    return RetVal;
}

/**
* hook of system status flags
* @param [in] StatIdx id of status flag
* @param [in] pStatus global block of status flag
* @param [in] StatusSize size of status flag
* @return 0-OK, 1-NG
*/
UINT32 SvcSysStat_Hook(UINT32 StatIdx, void *pStatus, UINT32 StatusSize)
{
    UINT32 RetVal = SVC_OK;

    if (pSvcSysStatCallbackList == NULL) {
        SvcLog_NG("SYS_STAT", "SysStat structure is not configured", 0U, 0U);
        RetVal = SVC_NG;
    }

    if (SVC_OK == RetVal) {
        if (StatIdx >= SvcSysStatNum) {
            SvcLog_NG("SYS_STAT", "StatIdx (%d) should be smaller than %d", StatIdx, SvcSysStatNum);
            RetVal = SVC_NG;
        }
    }

    if (SVC_OK == RetVal) {
        if (pSvcSysStatCallbackList[StatIdx].pStatus == NULL) {
            pSvcSysStatCallbackList[StatIdx].pStatus = pStatus;
            pSvcSysStatCallbackList[StatIdx].StatusSize = StatusSize;
            if (StatusSize > SvcSysStatMaxStatusSize){
                SvcSysStatMaxStatusSize = StatusSize;
            }
        } else {
            SvcLog_NG("SYS_STAT", "StatIdx (%d) has been hooked before", 0U, 0U);
            RetVal = SVC_NG;
        }
    }

    return RetVal;
}

/**
* register of system status flags
* @param [in] StatIdx id of status flag
* @param [in] Callback callback function of system status flags
* @param [out] pCtrlID control id
* @return 0-OK, 1-NG
*/
UINT32 SvcSysStat_Register(UINT32 StatIdx, SVC_SYS_STAT_f Callback, SVC_SYS_STAT_ID *pCtrlID)
{
    UINT32 RetVal = SVC_OK;
    UINT32 CtrlID = 0U;
    void *pInfo = NULL;

    if (pSvcSysStatCallbackList == NULL) {
        SvcLog_NG("SYS_STAT", "SysStat structure is not configured", 0U, 0U);
        RetVal = SVC_NG;
    }

    if (SVC_OK == RetVal) {
        if (StatIdx >= SvcSysStatNum) {
            SvcLog_NG("SYS_STAT", "StatIdx (%d) should be smaller than %d", StatIdx, SvcSysStatNum);
            RetVal = SVC_NG;
        }
    }

    if (SVC_OK == RetVal) {
        if (pSvcSysStatCallbackList[StatIdx].CallbackNum < SVC_SYS_STAT_CALLBACK_MAX) {
            for (CtrlID = 0U; CtrlID < SVC_SYS_STAT_CALLBACK_MAX; CtrlID++) {
                if (pSvcSysStatCallbackList[StatIdx].Callback[CtrlID] == NULL) {
                    pSvcSysStatCallbackList[StatIdx].Callback[CtrlID] = Callback;
                    *pCtrlID = CtrlID;
                    if (pSvcSysStatCallbackList[StatIdx].Callback[CtrlID] != NULL) {
                        if (pSvcSysStatCallbackList[StatIdx].pStatus != NULL) {
                            AmbaMisra_TypeCast(&pInfo, &SvcSysStatStatusAddr);
                            RetVal = AmbaWrap_memcpy(pInfo, pSvcSysStatCallbackList[StatIdx].pStatus, pSvcSysStatCallbackList[StatIdx].StatusSize);
                            if (SVC_OK != RetVal) {
                                SvcLog_NG("SYS_STAT", "StatIdx (%d) status copy failed (%d)", StatIdx, RetVal);
                            }
                        }
                        pSvcSysStatCallbackList[StatIdx].Callback[CtrlID](StatIdx, pInfo);
                    }
                    RetVal = SVC_OK;
                    break;
                }
            }
        } else {
            SvcLog_NG("SYS_STAT", "StatIdx %d Callback is full", StatIdx, 0U);
            RetVal = SVC_NG;
        }
    }

    return RetVal;
}

/**
* un-register of system status flags
* @param [in] StatIdx id of status flag
* @param [in] CtrlID control id
* @return 0-OK, 1-NG
*/
UINT32 SvcSysStat_Unregister(UINT32 StatIdx, SVC_SYS_STAT_ID CtrlID)
{
    UINT32 RetVal = SVC_OK;

    if (pSvcSysStatCallbackList == NULL) {
        SvcLog_NG("SYS_STAT", "SysStat structure is not configured", 0U, 0U);
        RetVal = SVC_NG;
    }

    if (SVC_OK == RetVal) {
        if (StatIdx >= SvcSysStatNum) {
            SvcLog_NG("SYS_STAT", "StatIdx (%d) should be smaller than %d", StatIdx, SvcSysStatNum);
            RetVal = SVC_NG;
        }
    }

    if (SVC_OK == RetVal) {
        if (pSvcSysStatCallbackList[StatIdx].Callback[CtrlID] != NULL) {
            pSvcSysStatCallbackList[StatIdx].Callback[CtrlID] = NULL;

            RetVal = SVC_OK;
        } else {
            SvcLog_NG("SYS_STAT", "StatIdx %d Callback %d is empty", StatIdx, CtrlID);
            RetVal = SVC_NG;
        }
    }

    return RetVal;
}

/**
* issue of system status flags
* @param [in] StatIdx id of status flag
* @param [in] pInfo info block of system status flags
* @return 0-OK, 1-NG
*/
UINT32 SvcSysStat_Issue(UINT32 StatIdx, void *pInfo)
{
    UINT32 RetVal = SVC_OK;
    UINT32 CtrlID = 0U;

    if (pSvcSysStatCallbackList == NULL) {
        SvcLog_NG("SYS_STAT", "SysStat structure is not configured", 0U, 0U);
        RetVal = SVC_NG;
    }

    if (SVC_OK == RetVal) {
        if (StatIdx >= SvcSysStatNum) {
            SvcLog_NG("SYS_STAT", "StatIdx (%d) should be smaller than %d", StatIdx, SvcSysStatNum);
            RetVal = SVC_NG;
        }
    }

    if (SVC_OK == RetVal) {
        if (pSvcSysStatCallbackList[StatIdx].pStatus != NULL) {
            RetVal = AmbaWrap_memcpy(pSvcSysStatCallbackList[StatIdx].pStatus, pInfo, pSvcSysStatCallbackList[StatIdx].StatusSize);
            if (SVC_OK == RetVal) {
                for (CtrlID = 0U; CtrlID < SVC_SYS_STAT_CALLBACK_MAX; CtrlID++) {
                    if (pSvcSysStatCallbackList[StatIdx].Callback[CtrlID] != NULL) {
                        pSvcSysStatCallbackList[StatIdx].Callback[CtrlID](StatIdx, pInfo);
                    }
                }
            } else {
                SvcLog_NG("SYS_STAT", "StatIdx (%d) status copy failed (%d)", StatIdx, RetVal);
            }
        } else {
            SvcLog_NG("SYS_STAT", "Unknown (Unhooked) status idx (%d)", StatIdx, 0U);
            RetVal = SVC_NG;
        }
    }

    return RetVal;
}

/**
* info get of system status flags
* @param [in] StatIdx id of status flag
* @param [out] pInfo info block of system status flags
* @return 0-OK, 1-NG
*/
UINT32 SvcSysStat_Get(UINT32 StatIdx, void *pInfo)
{
    UINT32 RetVal = SVC_OK;

    if (pInfo == NULL) {
        RetVal = SVC_NG;
    } else {
        if (pSvcSysStatCallbackList[StatIdx].pStatus != NULL) {
            RetVal = AmbaWrap_memcpy(pInfo, pSvcSysStatCallbackList[StatIdx].pStatus, pSvcSysStatCallbackList[StatIdx].StatusSize);
            if (SVC_OK != RetVal) {
                SvcLog_NG("SYS_STAT", "StatIdx (%d) status copy failed (%d)", StatIdx, RetVal);
            }
        } else {
            SvcLog_NG("SYS_STAT", "Unknown (Unhooked) status idx (%d)", StatIdx, 0U);
            RetVal = SVC_NG;
        }
    }

    return RetVal;
}

/**
* memory buffer query of system status flags
* @param [out] pBufferSize needed buffer size
* @return 0-OK, 1-NG
*/
UINT32 SvcSysStat_QueryBufSize(UINT32 *pBufferSize)
{
    UINT32 RetVal = SVC_OK;

    if (1U == SvcSysStatInit) {
        *pBufferSize = ( (UINT32)sizeof(SVC_SYS_STAT_CALLBACK_s) * SvcSysStatNum) + SvcSysStatMaxStatusSize;
    } else {
        SvcLog_NG("SYS_STAT", "SvcSysStat_Init() need to be issued before query buffer size", 0U, 0U);
        RetVal = SVC_NG;
    }

    return RetVal;
}
