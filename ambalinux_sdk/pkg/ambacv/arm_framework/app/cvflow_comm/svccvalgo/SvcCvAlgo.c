/**
*  @file SvcCvAlgo.c
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
*   @details Implementation of SvcCvAlgo.
*
*/

#include "SvcCvAlgo.h"


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: SvcCvAlgo_Query
 *
 *  @Description:: CvAlgo query
 *
 *  @Input      ::
 *
 *  @Output     ::
 *
 *  @Return     ::
 *
\*-----------------------------------------------------------------------------------------------*/
UINT32 SvcCvAlgo_Query(SVC_CV_ALGO_HANDLE_s* pHdlr, SVC_CV_ALGO_QUERY_CFG_s *pCfg)
{
    UINT32 RetVal = CVALGO_ERR_INVALID_API;

    if (pHdlr != NULL) {
        if (pCfg->pAlgoObj != NULL) {
            if (pCfg->pAlgoObj->Query != NULL) {
                RetVal = pCfg->pAlgoObj->Query(pHdlr, pCfg);
            }
        }
    }
    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: SvcCvAlgo_Create
 *
 *  @Description:: CvAlgo create
 *
 *  @Input      ::
 *
 *  @Output     ::
 *
 *  @Return     ::
 *
\*-----------------------------------------------------------------------------------------------*/
UINT32 SvcCvAlgo_Create(SVC_CV_ALGO_HANDLE_s* pHdlr, SVC_CV_ALGO_CREATE_CFG_s *pCfg)
{
    UINT32 RetVal = CVALGO_ERR_INVALID_API;

    if (pHdlr != NULL) {
        if (pHdlr->pAlgoObj != NULL) {
            if (pHdlr->pAlgoObj->Create != NULL) {
                RetVal = pHdlr->pAlgoObj->Create(pHdlr, pCfg);
            }
        }
    }
    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: SvcCvAlgo_Delete
 *
 *  @Description:: CvAlgo delete
 *
 *  @Input      ::
 *
 *  @Output     ::
 *
 *  @Return     ::
 *
\*-----------------------------------------------------------------------------------------------*/
UINT32 SvcCvAlgo_Delete(SVC_CV_ALGO_HANDLE_s* pHdlr, const SVC_CV_ALGO_DELETE_CFG_s *pCfg)
{
    UINT32 RetVal = CVALGO_ERR_INVALID_API;

    if (pHdlr != NULL) {
        if (pHdlr->pAlgoObj != NULL) {
            if (pHdlr->pAlgoObj->Delete != NULL) {
                RetVal = pHdlr->pAlgoObj->Delete(pHdlr, pCfg);
            }
        }
    }
    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: SvcCvAlgo_Feed
 *
 *  @Description:: CvAlgo feed
 *
 *  @Input      ::
 *
 *  @Output     ::
 *
 *  @Return     ::
 *
\*-----------------------------------------------------------------------------------------------*/
UINT32 SvcCvAlgo_Feed(SVC_CV_ALGO_HANDLE_s* pHdlr, const SVC_CV_ALGO_FEED_CFG_s *pCfg)
{
    UINT32 RetVal = CVALGO_ERR_INVALID_API;

    if (pHdlr != NULL) {
        if (pHdlr->pAlgoObj != NULL) {
            if (pHdlr->pAlgoObj->Feed != NULL) {
                RetVal = pHdlr->pAlgoObj->Feed(pHdlr, pCfg);
            }
        }
    }
    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: SvcCvAlgo_Control
 *
 *  @Description:: CvAlgo control
 *
 *  @Input      ::
 *
 *  @Output     ::
 *
 *  @Return     ::
 *
\*-----------------------------------------------------------------------------------------------*/
UINT32 SvcCvAlgo_Control(SVC_CV_ALGO_HANDLE_s* pHdlr, const SVC_CV_ALGO_CTRL_CFG_s *pCfg)
{
    UINT32 RetVal = CVALGO_ERR_INVALID_API;

    if (pHdlr != NULL) {
        if (pHdlr->pAlgoObj != NULL) {
            if (pHdlr->pAlgoObj->Control != NULL) {
                RetVal = pHdlr->pAlgoObj->Control(pHdlr, pCfg);
            }
        }
    }
    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: SvcCvAlgo_RegCallback
 *
 *  @Description:: CvAlgo register callback
 *
 *  @Input      ::
 *
 *  @Output     ::
 *
 *  @Return     ::
 *
\*-----------------------------------------------------------------------------------------------*/
UINT32 SvcCvAlgo_RegCallback(SVC_CV_ALGO_HANDLE_s* pHdlr, const SVC_CV_ALGO_REGCB_CFG_s *pCfg)
{
    UINT32 i;
    UINT32 Rval = CVALGO_NG;
    if ((pHdlr != NULL) && (pCfg != NULL)) {
        if (pCfg->Mode == 0U) {    // Find an empty space to register
            for (i = 0U; i < MAX_CALLBACK_NUM; i++) {
                if ((pHdlr->Callback[i] == pCfg->Callback) || (pHdlr->Callback[i] == NULL)) {   //Already register or empty
                    pHdlr->Callback[i] = pCfg->Callback;
                    Rval = CVALGO_OK;
                    break;
                }
            }
        } else { // Find callback and remove it
            for (i = 0U; i < MAX_CALLBACK_NUM; i++) {
                if (pHdlr->Callback[i] == pCfg->Callback) {
                    pHdlr->Callback[i] = NULL;
                    break;
                }
            }
        }
    }
    return Rval;
}


