/**
 *  @file SCA_AmbaSeg39.c
 *
 *  @copyright Copyright (c) 2020 Ambarella, Inc.
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
 *  @details Implementation of AmbaSeg39 FlexiDAG
 *
 */
#include "AmbaTypes.h"
#include "cvapi_memio_interface.h"
#include "cvapi_ambacv_flexidag.h"
#include "cvapi_amba_mask_interface.h"
#include "cvapi_idsp_interface.h"
#include "cvapi_memio_interface.h"
#include "idsp_roi_msg.h"
#include "AmbaPrint.h"
#include "AmbaMisraFix.h"
#include "AmbaWrap.h"
#include "AmbaUtility.h"
#include "AmbaMMU.h"
#include "AmbaCache.h"
#include "AmbaFS.h"
#include "SvcCvAlgo.h"
#include "cvapi_svccvalgo_ambaseg.h"
#include "cvapi_svccvalgo_ambaod.h"


//#define DBG_LOG     AmbaPrint_PrintUInt5
//#define CRI_LOG     AmbaPrint_PrintUInt5
//#define DBG {AmbaPrint_PrintInt5("OBJ@%d", __LINE__, 0, 0, 0, 0);AmbaKAL_TaskSleep(50);}

static void SegCbHdlrFD0(const CCF_OUTPUT_s *pEventData)
{
    const SVC_CV_ALGO_HANDLE_s* Hdlr;
    UINT32 i, Num;
    SVC_CV_ALGO_OUTPUT_s Out;
    amba_mask_out_t *pMaskOut;

    AmbaMisra_TypeCast(&Hdlr, &(pEventData->pCvAlgoHdlr));

    Out.pUserData = pEventData->pUserData;

    /* there is an extea buffer for amba_mask_out_t. */
    Num = pEventData->pOut->num_of_buf;

    // fill descriptor
    AmbaMisra_TypeCast(&pMaskOut, &(pEventData->pOut->buf[Num].pBuffer));
#if 1
    AmbaMisra_TypeCast(&pMaskOut->buff_addr, &(pEventData->pOut->buf[0].pBuffer));
    //AmbaMisra_TypeCast(&pMaskOut->buff_addr, &(pEvnetData->pOut->buf[1].pBuffer));
    //AmbaMisra_TypeCast(&pMaskOut->buff_addr, &(pEvnetData->pOut->buf[2].pBuffer));
    //AmbaMisra_TypeCast(&pMaskOut->buff_addr, &(pEvnetData->pOut->buf[3].pBuffer));
#endif

    pMaskOut->type                  = 0U;
    pMaskOut->length                = 1280U * 512U;

    pMaskOut->buff_width            = 1280U;
    pMaskOut->buff_height           = 512U;
    pMaskOut->mask_start_col        = 0U;
    pMaskOut->mask_start_row        = 0U;
    pMaskOut->mask_width            = 1280U;
    pMaskOut->mask_height           = 512U;

    Out.pOutput         = pEventData->pOut;
    Out.pExtOutput      = pMaskOut;

    for (i = 0; i < MAX_CALLBACK_NUM; i++) {
        if (Hdlr->Callback[i] != NULL) {
            (void)Hdlr->Callback[i](CALLBACK_EVENT_FREE_INPUT, &Out);
            (void)Hdlr->Callback[i](CALLBACK_EVENT_OUTPUT, &Out);
        }
    }
}

static UINT32 AmbaSeg39_Query(SVC_CV_ALGO_HANDLE_s* Hdlr, SVC_CV_ALGO_QUERY_CFG_s *Cfg)
{
    // assign obj
    Hdlr->pAlgoObj = Cfg->pAlgoObj;
    Cfg->TotalReqBufSz = ((UINT32)22U << 20U); // 22M

    return CVALGO_OK;
}

static UINT32 AmbaSeg39_Control(SVC_CV_ALGO_HANDLE_s* Hdlr, const SVC_CV_ALGO_CTRL_CFG_s *Cfg)
{
// UUID is defined in flow table
#define AMBASEG_VP  (2U)

    UINT32 MsgSz = 0U;
    UINT32 UUID = 0U;
    UINT32 *pMsgCode;
    UINT32 Ret = 0U;
    Hdlr->State = 2U;

    AmbaMisra_TypeCast(&pMsgCode, &Cfg->pCtrlParam);

    switch (*pMsgCode) {
    case AMBA_ROI_CONFIG_MSG:
        MsgSz = sizeof(amba_roi_config_t);
        UUID = AMBASEG_VP;
        break;
    case AMBA_MASK_CONFIG_MSG_TYPE:
        MsgSz = sizeof(amba_mask_config_t);
        UUID = AMBASEG_VP;
        break;
    case AMBANET_MSG_TYPE_LICENSE:
        MsgSz = (sizeof(UINT32) * 256U);
        UUID = AMBASEG_VP;
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

static void AmbaSeg39_DefaultConfigCCF(SVC_CV_ALGO_HANDLE_s* pHdlr)
{
    void* vp;
    amba_mask_config_t config;
    amba_mask_config_t *cfg = &config;
    amba_roi_config_t roi_msg;
    amba_roi_config_t *p_roi_msg = &roi_msg;
    AMBA_FS_FILE *Fp;
    const char *LcsFile = "ambadef.lcs";
    uint32_t BytesRead, FileSize;
    uint32_t Ret;
    UINT32 msg[258];
    UINT32 *pU32;
    SVC_CV_ALGO_CTRL_CFG_s MsgCfg;

    cfg->msg_type               = AMBA_MASK_CONFIG_MSG_TYPE;
    cfg->image_pyramid_index    = 1U;
    cfg->roi_start_col          = 40U;
    cfg->roi_start_row          = 180U;
    cfg->roi_width              = 1280U;
    cfg->roi_height             = 512U;
    cfg->buff_width             = 1280U;
    cfg->buff_height            = 512U;
    cfg->mask_width             = 1280U;
    cfg->mask_height            = 512U;
    cfg->mask_start_col         = 0U;
    cfg->mask_start_row         = 0U;
    cfg->num_categories         = 7U;

    p_roi_msg->msg_type                = AMBA_ROI_CONFIG_MSG;
    p_roi_msg->image_pyramid_index     = cfg->image_pyramid_index;
    p_roi_msg->source_vin              = 0;
    p_roi_msg->roi_start_col           = cfg->roi_start_col;
    p_roi_msg->roi_start_row           = cfg->roi_start_row;
    p_roi_msg->roi_width               = cfg->roi_width;
    p_roi_msg->roi_height              = cfg->roi_height;

    MsgCfg.pExtCtrlCfg = NULL;
    AmbaMisra_TypeCast(&MsgCfg.pCtrlParam, &p_roi_msg);
    (void)AmbaSeg39_Control(pHdlr, &MsgCfg);

    /* read license filfe */
    Ret = AmbaFS_FileOpen(LcsFile, "rb", &Fp);
    if (Ret != 0U) {
        AmbaPrint_PrintUInt5("can't read license file!!",
                             0U, 0U, 0U, 0U, 0U);
    } else {
        (void) AmbaCV_UtilityFileSize(LcsFile, &FileSize);

        msg[0] = AMBANET_MSG_TYPE_LICENSE;
        pU32 = &msg[1];
        AmbaMisra_TypeCast(&vp, &pU32);
        Ret = AmbaFS_FileRead(vp, 1U, (UINT32)FileSize, Fp,
                              &BytesRead);
        if ((BytesRead == 0U) || (Ret != 0U)) {
            AmbaPrint_PrintUInt5("Error while reading file!!",
                                 0U, 0U, 0U, 0U, 0U);
        }

        Ret = AmbaFS_FileClose(Fp);
        if(Ret != 0U) {
            AmbaPrint_PrintUInt5("_AmbaSEG_DefaultConfig: "
                                 "AmbaFS_FileClose fail",
                                 0U, 0U, 0U, 0U, 0U);
        }

        pU32 = &msg[0];
        MsgCfg.pExtCtrlCfg = NULL;
        AmbaMisra_TypeCast(&MsgCfg.pCtrlParam, &pU32);
        (void)AmbaSeg39_Control(pHdlr, &MsgCfg);
    }
}

static UINT32 AmbaSeg39_Create(SVC_CV_ALGO_HANDLE_s* Hdlr, SVC_CV_ALGO_CREATE_CFG_s *Cfg)
{
    CCF_TSK_CTRL_s Ctrl;
    CCF_CREATE_CFG_s CreCfg;
    CCF_OUTPUT_INFO_s OutInfo;
    CCF_REGCB_CFG_s CBCfg;
    UINT32 i;
    UINT32 Ret;
    const ExtTaskCreateCfg *pExt = NULL;

    // create mempool
    Hdlr->pAlgoBuf = Cfg->pAlgoBuf;
    (void)ArmMemPool_Create(Hdlr->pAlgoBuf, &Hdlr->MemPoolId);

    // clean cb function;
    for (i = 0; i < MAX_CALLBACK_NUM; i++) {
        Hdlr->Callback[i] = NULL;
    }

    Hdlr->NumFD = 1U;

    // create ccf for FD0
    AmbaUtility_StringCopy(CreCfg.FDName, 16U, "AmbaSeg39");
    Ctrl.CoreSel = 0xE;
    Ctrl.Priority = 50;
    Ctrl.StackSz = ((UINT32)16U << 10U); // 16K
    CreCfg.RunTskCtrl = Ctrl;
    AmbaMisra_TouchUnused(&Ctrl);

    Ctrl.CoreSel = 0xE;
    Ctrl.Priority = 51;
    Ctrl.StackSz = ((UINT32)16U << 10U); // 16K
    CreCfg.CBTskCtrl = Ctrl;
    AmbaMisra_TouchUnused(&Ctrl);

    if (Cfg->pExtCreateCfg != NULL) {
        AmbaMisra_TypeCast(&pExt, &Cfg->pExtCreateCfg);
        if (pExt->MagicCode == ExtTaskCreateCfgMagic) {
            AmbaWrap_memcpy(&CreCfg.RunTskCtrl, &pExt->RunTskCtrl, sizeof(CCF_TSK_CTRL_s));
            AmbaWrap_memcpy(&CreCfg.CBTskCtrl, &pExt->CBTskCtrl, sizeof(CCF_TSK_CTRL_s));
        }
    }

    CreCfg.InBufDepth = 0;
    CreCfg.OutBufDepth = 0;
    CreCfg.MemPoolId = Hdlr->MemPoolId;
    CreCfg.pBin = Cfg->pBin[0];
    Ret = CvCommFlexi_Create(&Hdlr->FDs[0], &CreCfg, &OutInfo);

    // Reg CB for FD0
    if (Ret == CVALGO_OK) {
        CBCfg.Mode = 0U;
        CBCfg.Callback = SegCbHdlrFD0;
        Ret = CvCommFlexi_RegCallback(&Hdlr->FDs[0], &CBCfg);

        Cfg->OutputNum = OutInfo.OutputNum;
        for (i = 0; i < OutInfo.OutputNum; i++) {
            Cfg->OutputSz[i] = OutInfo.OutputSz[i];
        }

        /* Add 1 more output for amba_mask_out_t */
        Cfg->OutputNum = OutInfo.OutputNum + 1U;
        Cfg->OutputSz[OutInfo.OutputNum] = sizeof(amba_mask_out_t);
        Hdlr->State = 1U;
    }

    return Ret;
}

static UINT32 AmbaSeg39_Delete(SVC_CV_ALGO_HANDLE_s* Hdlr, const SVC_CV_ALGO_DELETE_CFG_s *Cfg)
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

static UINT32 AmbaSeg39_Feed(SVC_CV_ALGO_HANDLE_s* Hdlr, const SVC_CV_ALGO_FEED_CFG_s *Cfg)
{
    CCF_FEED_CFG_s FeedCfg;
    UINT32 Ret;

    if (Hdlr->State == 1U) {
        // send default config
        AmbaSeg39_DefaultConfigCCF(Hdlr);
        Hdlr->State = 2U;
    }

    FeedCfg.pCvAlgoHdlr         = Hdlr;
    FeedCfg.pIn                 = Cfg->pIn;
    FeedCfg.pOut                = Cfg->pOut;
    FeedCfg.pUserData           = Cfg->pUserData;
    FeedCfg.pInternal           = NULL;
    FeedCfg.pRaw                = NULL;
    FeedCfg.pPic                = NULL;
    /* Replace the num_of_buf with the number from FD */
    /* because amba_mask_out_t is an extra output which is not belong to FD. */
    FeedCfg.pOut->num_of_buf    = Hdlr->FDs[0].Handle.mem_req.flexidag_num_outputs;

    Ret = CvCommFlexi_Feed(&Hdlr->FDs[0], &FeedCfg);

    return Ret;
}

SVC_CV_ALGO_OBJ_s AmbaSeg39AlgoObj = {
    .Name       = "AmbaSeg39",
    .Query      = AmbaSeg39_Query,
    .Create     = AmbaSeg39_Create,
    .Delete     = AmbaSeg39_Delete,
    .Feed       = AmbaSeg39_Feed,
    .Control    = AmbaSeg39_Control
};


