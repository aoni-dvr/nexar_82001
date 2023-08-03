/**
 *  @file SCA_Plain.c
 *
 *  @copyright Copyright (c) 2019 Ambarella International LP
 *
 *  This file and its contents ("Software") are protected by intellectual
 *  property rights including, without limitation, U.S. and/or foreign
 *  copyrights. This Software is also the confidential and proprietary
 *  information of Ambarella International LP and its licensors. You may not use, reproduce,
 *  disclose, distribute, modify, or otherwise prepare derivative works of this
 *  Software or any portion thereof except pursuant to a signed license agreement
 *  or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
 *  In the absence of such an agreement, you agree to promptly notify and return
 *  this Software to Ambarella International LP.
 *
 *  This file includes sample code and is only for internal testing and evaluation.  If you
 *  distribute this sample code (whether in source, object, or binary code form), it will be
 *  without any warranty or indemnity protection from Ambarella International LP or its affiliates.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 *  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 *  MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 *  IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 *  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details Implementation of SCA Plain
 *
 */
#include "AmbaTypes.h"
#include "cvapi_memio_interface.h"
#include "cvapi_ambacv_flexidag.h"
#include "idsp_roi_msg.h"
#include "AmbaPrint.h"
#include "AmbaMisraFix.h"
#include "AmbaWrap.h"
#include "AmbaUtility.h"
#include "AmbaFS.h"
#include "SvcCvAlgo.h"
#include "cvapi_svccvalgo_plain.h"

//ROI settings used in DefaultConfig, only for picinfo
#define ROI_START_COL 42U
#define ROI_START_ROW 180U
#define ROI_WIDTH     1280U
#define ROI_HEIGHT    512U

static void CCFCbHdlrFD0(const CCF_OUTPUT_s *pEventData)
{
    const SVC_CV_ALGO_HANDLE_s* Hdlr;
    UINT32 idx;
    SVC_CV_ALGO_OUTPUT_s Out;

    AmbaMisra_TypeCast(&Hdlr, &(pEventData->pCvAlgoHdlr));

    Out.pUserData = pEventData->pUserData;
    Out.pOutput = pEventData->pOut;
    Out.pExtOutput = NULL;

    for (idx = 0U; idx < MAX_CALLBACK_NUM; idx++) {
        if (Hdlr->Callback[idx] != NULL) {
            (void)Hdlr->Callback[idx](CALLBACK_EVENT_FREE_INPUT, &Out);
            (void)Hdlr->Callback[idx](CALLBACK_EVENT_OUTPUT, &Out);
        }
    }
}

static UINT32 Plain_Query(SVC_CV_ALGO_HANDLE_s* Hdlr, SVC_CV_ALGO_QUERY_CFG_s *Cfg)
{
    // assign obj
    Hdlr->pAlgoObj = Cfg->pAlgoObj;
    Cfg->TotalReqBufSz = ((UINT32)45U<<20U); //45M

    return CVALGO_OK;
}

static UINT32 Plain_Control(SVC_CV_ALGO_HANDLE_s* Hdlr, const SVC_CV_ALGO_CTRL_CFG_s *Cfg)
{
// UUID is defined in flow table
#define Plain_VP  (2U)

    UINT32 MsgSz = 0U;
    UINT32 UUID = 0U;
    UINT32 MsgCode = 0U;
    UINT32 Ret = 0U;

    Hdlr->State = 2U;

    AmbaMisra_TypeCast(&MsgCode, Cfg->pCtrlParam);

    switch (MsgCode) {
    case AMBA_ROI_CONFIG_MSG:
        MsgSz = (UINT32)(sizeof(amba_roi_config_t));
        UUID = Plain_VP;
        break;
    default:
        Ret = 0xFFFFFFFFU;
        break;
    }

    if (Ret == 0U) {
        CCF_SEND_MSG_CFG_s MsgCfg;
        MsgCfg.MsgSize = MsgSz;
        MsgCfg.pMsg = Cfg->pCtrlParam;
        MsgCfg.UUID = UUID;
        Ret = CvCommFlexi_SendMsg(&Hdlr->FDs[0], &MsgCfg);
    }

    return Ret;
}

static void Plain_DefaultConfigCCF(SVC_CV_ALGO_HANDLE_s* pHdlr)
{
    amba_roi_config_t roi_msg;
    amba_roi_config_t *p_roi_msg = &roi_msg;
    SVC_CV_ALGO_CTRL_CFG_s MsgCfg;

    p_roi_msg->msg_type                = AMBA_ROI_CONFIG_MSG;
    p_roi_msg->image_pyramid_index     = 1U;
    p_roi_msg->source_vin              = 0U;
    p_roi_msg->roi_start_col           = ROI_START_COL;
    p_roi_msg->roi_start_row           = ROI_START_ROW;
    p_roi_msg->roi_width               = ROI_WIDTH;
    p_roi_msg->roi_height              = ROI_HEIGHT;

    MsgCfg.pExtCtrlCfg = NULL;
    MsgCfg.CtrlType = 0;
    AmbaMisra_TypeCast(&MsgCfg.pCtrlParam, &p_roi_msg);
    (void)Plain_Control(pHdlr, &MsgCfg);
}

static UINT32 Plain_Create(SVC_CV_ALGO_HANDLE_s* Hdlr, SVC_CV_ALGO_CREATE_CFG_s *Cfg)
{
    CCF_TSK_CTRL_s Ctrl;
    CCF_CREATE_CFG_s CreCfg;
    CCF_OUTPUT_INFO_s OutInfo;
    CCF_REGCB_CFG_s CBCfg;
    UINT32 idx;
    UINT32 Ret;

    // create mempool
    Hdlr->pAlgoBuf = Cfg->pAlgoBuf;
    (void)ArmMemPool_Create(Hdlr->pAlgoBuf, &Hdlr->MemPoolId);
    // clean cb function;
    for (idx = 0U; idx < MAX_CALLBACK_NUM; idx++) {
        Hdlr->Callback[idx] = NULL;
    }

    Hdlr->NumFD = 1U;

    //create ccf for FD0
    AmbaUtility_StringCopy(CreCfg.FDName, 6U, "Plain");
    Ctrl.CoreSel = 0xE;
    Ctrl.Priority = 50;
    Ctrl.StackSz = ((UINT32)16U<<10U); //16K
    CreCfg.RunTskCtrl = Ctrl;
    AmbaMisra_TouchUnused(&Ctrl);

    Ctrl.CoreSel = 0xE;
    Ctrl.Priority = 51;
    Ctrl.StackSz = ((UINT32)16U<<10U); //16K
    CreCfg.CBTskCtrl = Ctrl;
    AmbaMisra_TouchUnused(&Ctrl);

    CreCfg.InBufDepth = 0;
    CreCfg.OutBufDepth = 0;
    CreCfg.MemPoolId = Hdlr->MemPoolId;
    CreCfg.pBin = Cfg->pBin[0];
    Ret = CvCommFlexi_Create(&Hdlr->FDs[0], &CreCfg, &OutInfo);
    //Reg CB for FD0
    if (Ret == CVALGO_OK) {
        CBCfg.Mode = 0U;
        CBCfg.Callback = CCFCbHdlrFD0;
        Ret = CvCommFlexi_RegCallback(&Hdlr->FDs[0], &CBCfg);

        Cfg->OutputNum = OutInfo.OutputNum;
        for (idx = 0U; idx < OutInfo.OutputNum; idx++) {
            Cfg->OutputSz[idx] = OutInfo.OutputSz[idx];
        }
        Hdlr->State = 1U;
    }
    return Ret;
}

static UINT32 Plain_Delete(SVC_CV_ALGO_HANDLE_s* Hdlr, const SVC_CV_ALGO_DELETE_CFG_s *Cfg)
{
    UINT32 Ret;

    (void)Cfg;

    Ret = ArmMemPool_Free(Hdlr->MemPoolId, &(Hdlr->FDs[0].FlexiBuf.State));
    Ret |= ArmMemPool_Free(Hdlr->MemPoolId, &(Hdlr->FDs[0].FlexiBuf.Temp));
    if (Ret == ARM_OK) {
        Ret = CvCommFlexi_Delete(&Hdlr->FDs[0]);
        if (Ret == CVALGO_OK) {
            Ret = ArmMemPool_Delete(Hdlr->MemPoolId);
        }
    }

    return Ret;
}

static UINT32 Plain_Feed(SVC_CV_ALGO_HANDLE_s* Hdlr, const SVC_CV_ALGO_FEED_CFG_s *Cfg)
{
    CCF_FEED_CFG_s FeedCfg;
    UINT32 i, Ret;

    if (Hdlr->State == 1U) {
        // send default config
        Plain_DefaultConfigCCF(Hdlr);
        Hdlr->State = 2U;
    }

    FeedCfg.pCvAlgoHdlr = Hdlr;
    FeedCfg.pIn = Cfg->pIn;
    FeedCfg.pOut = Cfg->pOut;
    FeedCfg.pUserData = Cfg->pUserData;
    FeedCfg.pOut->num_of_buf = Cfg->pOut->num_of_buf;
    FeedCfg.pInternal = NULL;
    FeedCfg.pRaw = NULL;
    FeedCfg.pPic = NULL;

    for (i = 0U; i < FeedCfg.pOut->num_of_buf; i++) {
        (void) AmbaCV_UtilityCmaMemClean(&FeedCfg.pOut->buf[i]);
    }

    Ret = CvCommFlexi_Feed(&Hdlr->FDs[0], &FeedCfg);

    for (i = 0U; i < FeedCfg.pOut->num_of_buf; i++) {
        (void) AmbaCV_UtilityCmaMemInvalid(&FeedCfg.pOut->buf[i]);
    }

    return Ret;
}

SVC_CV_ALGO_OBJ_s PlainAlgoObj = {
    .Query = Plain_Query,
    .Create = Plain_Create,
    .Delete = Plain_Delete,
    .Feed = Plain_Feed,
    .Control = Plain_Control
};


