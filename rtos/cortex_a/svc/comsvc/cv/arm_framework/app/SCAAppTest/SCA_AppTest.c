/**
 *  @file SCA_AppTest.c
 *
 *  @copyright Copyright (c) 2019 Ambarella, Inc.
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
 *  @details Implementation of SCA AppTest
 *
 */
#include "AmbaTypes.h"
#include "cvapi_memio_interface.h"
#include "cvapi_ambacv_flexidag.h"
#include "cvapi_amba_mask_interface.h"
#include "idsp_roi_msg.h"
#include "AmbaPrint.h"
#include "AmbaMisraFix.h"
#include "AmbaWrap.h"
#include "AmbaUtility.h"
#include "AmbaMMU.h"
#include "AmbaCache.h"
#include "AmbaFS.h"
#include "SvcCvAlgo.h"
#include "cvapi_svccvalgo_openseg.h"
#include "cvapi_svccvalgo_apptest.h"


//#define DBG_LOG     AmbaPrint_PrintUInt5
//#define CRI_LOG     AmbaPrint_PrintUInt5
//#define DBG {AmbaPrint_PrintInt5("OBJ@%d", __LINE__, 0, 0, 0, 0);AmbaKAL_TaskSleep(50);}

static void CCFCbHdlrFD0(const CCF_OUTPUT_s *pEvnetData)
{
    const SVC_CV_ALGO_HANDLE_s* Hdlr;
    UINT32 idx;
    SVC_CV_ALGO_OUTPUT_s Out;
    amba_mask_out_t *pMaskOut;
    AmbaMisra_TypeCast(&Hdlr, &(pEvnetData->pCvAlgoHdlr));
    Out.pUserData = pEvnetData->pUserData;
    Out.pOutput = pEvnetData->pOut;
    //fill descriptor
    AmbaMisra_TypeCast(&pMaskOut, &(pEvnetData->pOut->buf[1].pBuffer));
    AmbaMisra_TypeCast(&pMaskOut->buff_addr, &(pEvnetData->pOut->buf[0].pBuffer));

    pMaskOut->buff_width            = 320U;
    pMaskOut->buff_height           = 128U;
    pMaskOut->mask_start_col        = 0U;
    pMaskOut->mask_start_row        = 0U;
    pMaskOut->mask_width            = 320U;
    pMaskOut->mask_height           = 128U;

    Out.pExtOutput = pMaskOut;

    for (idx=0 ; idx<MAX_CALLBACK_NUM ; idx++) {
        if (Hdlr->Callback[idx] != NULL) {
            (void)Hdlr->Callback[idx](CALLBACK_EVENT_FREE_INPUT, &Out);
            (void)Hdlr->Callback[idx](CALLBACK_EVENT_OUTPUT, &Out);
        }
    }
}

static UINT32 AppTest_Query(SVC_CV_ALGO_HANDLE_s* Hdlr, SVC_CV_ALGO_QUERY_CFG_s *Cfg)
{
    // assign obj
    Hdlr->pAlgoObj = Cfg->pAlgoObj;
    Cfg->TotalReqBufSz = ((UINT32)70U<<20U); //70M

    return CVALGO_OK;
}

static UINT32 AppTest_Control(SVC_CV_ALGO_HANDLE_s* Hdlr, const SVC_CV_ALGO_CTRL_CFG_s *Cfg)
{
// UUID is defined in flow table
#define OPENSEG_VP  (2U)

    UINT32 MsgSz = 0U;
    UINT32 UUID = 0U;
    UINT32 MsgCode = 0U;
    UINT32 Ret = 0U;

    Hdlr->State = 2U;

    Ret = AmbaWrap_memcpy(&MsgCode, Cfg->pCtrlParam, sizeof(UINT32));
    if (0U != Ret) {
        AmbaPrint_PrintUInt5("memcpy MsgCode failed", 0U, 0U, 0U, 0U, 0U);
    }

    switch (MsgCode) {
    case AMBA_ROI_CONFIG_MSG:
        MsgSz = sizeof(amba_roi_config_t);
        UUID = OPENSEG_VP;
        break;
    case AMBA_MASK_CONFIG_MSG_TYPE:
        MsgSz = sizeof(amba_mask_config_t);
        UUID = OPENSEG_VP;
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

static void AppTest_DefaultConfigCCF(SVC_CV_ALGO_HANDLE_s* pHdlr)
{
    AmbaMisra_TouchUnused(pHdlr);
}

static UINT32 AppTest_Create(SVC_CV_ALGO_HANDLE_s* Hdlr, SVC_CV_ALGO_CREATE_CFG_s *Cfg)
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
    for (idx=0 ; idx<MAX_CALLBACK_NUM ; idx++) {
        Hdlr->Callback[idx] = NULL;
    }

    Hdlr->NumFD = 1U;

    //create ccf for FD0
    AmbaUtility_StringCopy(CreCfg.FDName, 8U, "AppTest");
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
        for (idx=0 ; idx<OutInfo.OutputNum ; idx++) {
            Cfg->OutputSz[idx] = OutInfo.OutputSz[idx];
        }
        Cfg->OutputNum = 2U;
        Cfg->OutputSz[1U] = sizeof(amba_mask_out_t);
        Hdlr->State = 1U;
    }
    return Ret;
}

static UINT32 AppTest_Delete(SVC_CV_ALGO_HANDLE_s* Hdlr, const SVC_CV_ALGO_DELETE_CFG_s *Cfg)
{
    UINT32 Ret;

    (void)Cfg;

    Ret = CvCommFlexi_Delete(&Hdlr->FDs[0]);
    if (Ret == CVALGO_OK) {
        Ret = ArmMemPool_Free(Hdlr->MemPoolId, Hdlr->pAlgoBuf);
        Ret |= ArmMemPool_Delete(Hdlr->MemPoolId);
    }

    return Ret;
}

static UINT32 AppTest_Feed(SVC_CV_ALGO_HANDLE_s* Hdlr, const SVC_CV_ALGO_FEED_CFG_s *Cfg)
{
    CCF_FEED_CFG_s FeedCfg;
    UINT32 Ret;

    if (Hdlr->State == 1U) {
        // send default config
        AppTest_DefaultConfigCCF(Hdlr);
        Hdlr->State = 2U;
    }

    FeedCfg.pCvAlgoHdlr = Hdlr;
    FeedCfg.pIn = Cfg->pIn;
    FeedCfg.pOut = Cfg->pOut;
    FeedCfg.pUserData = Cfg->pUserData;
    FeedCfg.pOut->num_of_buf = 1U;
	FeedCfg.pInternal = NULL;
    FeedCfg.pPic = NULL;
    FeedCfg.pRaw = NULL;
    Ret = CvCommFlexi_Feed(&Hdlr->FDs[0], &FeedCfg);
    return Ret;
}

SVC_CV_ALGO_OBJ_s AppTestAlgoObj = {
    .Query = AppTest_Query,
    .Create = AppTest_Create,
    .Delete = AppTest_Delete,
    .Feed = AppTest_Feed,
    .Control = AppTest_Control
};


