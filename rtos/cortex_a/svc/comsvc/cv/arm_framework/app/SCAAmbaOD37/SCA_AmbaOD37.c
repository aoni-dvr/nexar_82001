/**
 *  @file SCA_AmbaOD3737.c
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
 *  @details Implementation of AmbaOD37 FlexiDAG
 *
 */
#include "AmbaTypes.h"
#include "cvapi_memio_interface.h"
#include "cvapi_ambacv_flexidag.h"
#include "cvapi_flexidag_ref_util.h"
#include "cvapi_amba_odv37_interface_private.h"
#include "idsp_roi_msg.h"
#include "AmbaPrint.h"
#include "AmbaMisraFix.h"
#include "AmbaWrap.h"
#include "AmbaUtility.h"
#include "AmbaCache.h"
#include "SvcCvAlgo.h"
#include "cvapi_svccvalgo_ambaod.h"


//#define DBG_LOG     AmbaPrint_PrintUInt5
//#define CRI_LOG     AmbaPrint_PrintUInt5
//#define DBG {AmbaPrint_PrintInt5("OBJ@%d", __LINE__, 0, 0, 0, 0);AmbaKAL_TaskSleep(50);}

static void ODCbHdlrFD0(const CCF_OUTPUT_s *pEvnetData)
{
    const SVC_CV_ALGO_HANDLE_s* Hdlr;
    SVC_CV_ALGO_OUTPUT_s Out;
    UINT32 i;

    AmbaMisra_TypeCast32(&Hdlr, &(pEvnetData->pCvAlgoHdlr));

    Out.pUserData   = pEvnetData->pUserData;
    Out.pOutput     = pEvnetData->pOut;
    Out.pExtOutput  = pEvnetData->pOut;

    for (i = 0; i < MAX_CALLBACK_NUM; i++) {
        if (Hdlr->Callback[i] != NULL) {
            (void)Hdlr->Callback[i](CALLBACK_EVENT_FREE_INPUT, &Out);
            (void)Hdlr->Callback[i](CALLBACK_EVENT_OUTPUT, &Out);
        }
    }
}

static UINT32 AmbaOD37_Query(SVC_CV_ALGO_HANDLE_s* Hdlr, SVC_CV_ALGO_QUERY_CFG_s *Cfg)
{
    const UINT32 *pExt = NULL;
    UINT32 ROIs = 1;
    UINT32 ChipID = 0x0, MemReq = 0;

    if (Cfg->pExtQueryCfg != NULL) {
        AmbaMisra_TypeCast(&pExt, &Cfg->pExtQueryCfg);
        ROIs = *pExt & 0x000000FFU;
        ChipID = *pExt & CVCHIP_MASK;
    }

    if (ChipID == CVCHIP_CV2A) {
        MemReq = 62U;
    } else if ((ChipID == CVCHIP_CV5) || (ChipID == CVCHIP_CV52)) {
        MemReq = 102U;
    } else {
        MemReq = 44U;
    }

    AmbaPrint_PrintUInt5("ROIs: 0x%x, ChipID: 0x%x", ROIs, ChipID, 0U, 0U, 0U);

    Hdlr->pAlgoObj = Cfg->pAlgoObj;
    Cfg->TotalReqBufSz = ((UINT32) MemReq << 20U) * ROIs;

    return CVALGO_OK;
}

static UINT32 AmbaOD37_Control(SVC_CV_ALGO_HANDLE_s* Hdlr, const SVC_CV_ALGO_CTRL_CFG_s *Cfg)
{
// UUID is defined in flow table
#define AMBAOD_VP_0_UUID        (2U)
#define AMBAOD_VP_1_UUID        (12U)
#define AMBAOD_ARM_UUID         (4U)
    UINT32 MsgSz = 0U;
    UINT32 UUID = 0U;
    UINT32 MsgCode = 0U;
    UINT32 Ret = 0U;
    Hdlr->State = 2U;

    AmbaMisra_TypeCast32(&MsgCode, Cfg->pCtrlParam);

    switch (MsgCode) {
    case AMBA_ROI_CONFIG_MSG:
        MsgSz = sizeof(amba_roi_config_t);
        if (Cfg->CtrlType == SCA_CT_AMBAOD_ROI_F0) {
            UUID = AMBAOD_VP_0_UUID;
        } else {
            UUID = AMBAOD_VP_1_UUID;
        }
        break;
    case AMBANET_MSG_TYPE_OD_NMS:
        MsgSz = 1024U;
        UUID = AMBAOD_ARM_UUID;
        break;
    case AMBANET_MSG_TYPE_OD_GROUPING:
        MsgSz = sizeof(amba_od_grouping_t);
        UUID = AMBAOD_ARM_UUID;
        break;
    case AMBANET_MSG_TYPE_LICENSE:
        MsgSz = (sizeof(UINT32) * 256U);
        UUID = AMBAOD_VP_0_UUID;
        break;
    case AMBANET_MSG_TYPE_ROI:
        MsgSz = sizeof(ambanet_roi_config_t);
        UUID = AMBAOD_ARM_UUID;
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
        if (AMBANET_MSG_TYPE_LICENSE == MsgCode) {
            if (Cfg->CtrlType == SCA_CT_AMBAOD_LCS_2SCALE) {
                UUID = AMBAOD_VP_1_UUID;
                MsgCfg.UUID = UUID;
                Ret = CvCommFlexi_SendMsg(&Hdlr->FDs[0], &MsgCfg);
            }
        }
    }

    return Ret;
}

static void AmbaOD37_DefaultConfigCCF(SVC_CV_ALGO_HANDLE_s* pHdlr)
{
    AMBA_FS_FILE *Fp;
    const char *CfgFile = "ambaod37_cfg.bin";
    uint32_t BytesRead, FileSize;
    uint32_t Ret;
    uint8_t Buf[sizeof(amba_od_nms_configs_t) + \
                sizeof(amba_od_grouping_t) + \
                (sizeof(ambanet_roi_config_t) * 2U)];
    uint8_t *pBuf = Buf;
    const amba_od_nms_configs_t *Nms;
    const amba_od_grouping_t *Group;
    const ambanet_roi_config_t *RoiCfg[2];
    amba_roi_config_t RoiMsg;
    amba_roi_config_t *pRoiMsg = &RoiMsg;
    SVC_CV_ALGO_CTRL_CFG_s MsgCfg;

    MsgCfg.pExtCtrlCfg = NULL;
    MsgCfg.CtrlType = 0;

    AmbaPrint_PrintUInt5("Start defaul config!!", 0U, 0U, 0U, 0U, 0U);

    Ret = AmbaFS_FileOpen(CfgFile, "rb", &Fp);

    {
        /* To fix Misra-C not initialized variable issue. */
        pRoiMsg->msg_type            = AMBA_ROI_CONFIG_MSG;
        pRoiMsg->image_pyramid_index = 0;
        pRoiMsg->source_vin          = 0;
        pRoiMsg->roi_start_col       = 0;
        pRoiMsg->roi_start_row       = 0;
        pRoiMsg->roi_width           = 0;
        pRoiMsg->roi_height          = 0;
    }

    if (Ret != 0U) {
        AmbaPrint_PrintUInt5("can't read config file!!", 0U, 0U, 0U, 0U, 0U);
    } else {
        (void) AmbaCV_UtilityFileSize(CfgFile, &FileSize);

        Ret = AmbaFS_FileRead(pBuf, 1U, (UINT32)FileSize, Fp, &BytesRead);
        if ((BytesRead == 0U) || (Ret !=  0U)) {
            AmbaPrint_PrintUInt5("Error while reading file!! (0x%x)",
                                 Ret, 0U, 0U, 0U, 0U);
        }

        Ret = AmbaFS_FileClose(Fp);
        if(Ret !=  0U) {
            AmbaPrint_PrintUInt5("Error while closing file!! (0x%x)",
                                 Ret, 0U, 0U, 0U, 0U);
        }

        AmbaMisra_TypeCast32(&Nms, &pBuf);

        pBuf     = &Buf[sizeof(amba_od_nms_configs_t)];
        AmbaMisra_TypeCast32(&Group, &pBuf);

        pBuf     = &Buf[sizeof(amba_od_nms_configs_t) + \
                                                      sizeof(amba_od_grouping_t)];
        AmbaMisra_TypeCast32(&RoiCfg[0], &pBuf);

        pBuf     = &Buf[sizeof(amba_od_nms_configs_t) + \
                                                      sizeof(amba_od_grouping_t) + \
                                                      sizeof(ambanet_roi_config_t)];
        AmbaMisra_TypeCast32(&RoiCfg[1], &pBuf);

        pRoiMsg->msg_type                = AMBA_ROI_CONFIG_MSG;
        pRoiMsg->image_pyramid_index     = RoiCfg[0]->pyramid_index;
        pRoiMsg->source_vin              = 0;
        pRoiMsg->roi_start_col           = RoiCfg[0]->roi_start_col;
        pRoiMsg->roi_start_row           = RoiCfg[0]->roi_start_row;
        pRoiMsg->roi_width               = RoiCfg[0]->roi_width;
        pRoiMsg->roi_height              = RoiCfg[0]->roi_height;

        AmbaMisra_TypeCast32(&MsgCfg.pCtrlParam, &pRoiMsg);
        (void)AmbaOD37_Control(pHdlr, &MsgCfg);

        AmbaMisra_TypeCast32(&MsgCfg.pCtrlParam, &Nms);
        (void)AmbaOD37_Control(pHdlr, &MsgCfg);

        AmbaMisra_TypeCast32(&MsgCfg.pCtrlParam, &Group);
        (void)AmbaOD37_Control(pHdlr, &MsgCfg);

        AmbaMisra_TypeCast32(&MsgCfg.pCtrlParam, &RoiCfg[0]);
        (void)AmbaOD37_Control(pHdlr, &MsgCfg);
    }

    {
        const char *LcsFile = "ambadef.lcs";
        UINT32 Msg[258];
        const uint32_t *pU32;
        void* pVp;

        /* read license filfe */
        Ret = AmbaFS_FileOpen(LcsFile, "rb", &Fp);

        if (Ret != 0U) {
            AmbaPrint_PrintUInt5("can't read license file!!",
                                 0U, 0U, 0U, 0U, 0U);
        } else {
            (void) AmbaCV_UtilityFileSize(LcsFile, &FileSize);

            Msg[0] = AMBANET_MSG_TYPE_LICENSE;
            pU32 = &Msg[1];
            AmbaMisra_TypeCast32(&pVp, &pU32);
            Ret = AmbaFS_FileRead(pVp, 1U, (UINT32)FileSize, Fp,
                                  &BytesRead);
            if ((BytesRead == 0U) || (Ret != 0U)) {
                AmbaPrint_PrintUInt5("Error while reading file!! (0x%x)",
                                     Ret, 0U, 0U, 0U, 0U);
            }

            Ret = AmbaFS_FileClose(Fp);
            if(Ret != 0U) {
                AmbaPrint_PrintUInt5("Error while closing file!! (0x%x)",
                                     Ret, 0U, 0U, 0U, 0U);
            }

            pU32 = &Msg[0];
            AmbaMisra_TypeCast32(&MsgCfg.pCtrlParam, &pU32);
            (void)AmbaOD37_Control(pHdlr, &MsgCfg);
        }
    }

}

static UINT32 AmbaOD37_Create(SVC_CV_ALGO_HANDLE_s* Hdlr, SVC_CV_ALGO_CREATE_CFG_s *Cfg)
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

    //create ccf for FD0
    AmbaUtility_StringCopy(CreCfg.FDName, 16U, "AmbaOD37");
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

    if (Cfg->pExtCreateCfg != NULL) {
        AmbaMisra_TypeCast(&pExt, &Cfg->pExtCreateCfg);
        if (pExt->MagicCode == ExtTaskCreateCfgMagic) {
            (void) AmbaWrap_memcpy(&CreCfg.RunTskCtrl, &pExt->RunTskCtrl, sizeof(CCF_TSK_CTRL_s));
            (void) AmbaWrap_memcpy(&CreCfg.CBTskCtrl, &pExt->CBTskCtrl, sizeof(CCF_TSK_CTRL_s));
        }
    }

    CreCfg.InBufDepth = 0;
    CreCfg.OutBufDepth = 0;
    CreCfg.MemPoolId = Hdlr->MemPoolId;
    CreCfg.pBin = Cfg->pBin[0];
    Ret = CvCommFlexi_Create(&Hdlr->FDs[0], &CreCfg, &OutInfo);

    //Reg CB for FD0
    if (Ret == CVALGO_OK) {
        CBCfg.Mode = 0U;
        CBCfg.Callback = ODCbHdlrFD0;
        Ret = CvCommFlexi_RegCallback(&Hdlr->FDs[0], &CBCfg);

        Cfg->OutputNum = OutInfo.OutputNum;
        for (i = 0; i < OutInfo.OutputNum; i++) {
            Cfg->OutputSz[i] = OutInfo.OutputSz[i];
        }

        Hdlr->State = 1U;
    }
    return Ret;
}

static UINT32 AmbaOD37_Delete(SVC_CV_ALGO_HANDLE_s* Hdlr, const SVC_CV_ALGO_DELETE_CFG_s *Cfg)
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

static UINT32 AmbaOD37_Feed(SVC_CV_ALGO_HANDLE_s* Hdlr, const SVC_CV_ALGO_FEED_CFG_s *Cfg)
{
    CCF_FEED_CFG_s FeedCfg;
    UINT32 Ret;

    if (Hdlr->State == 1U) {
        // send default config
        AmbaOD37_DefaultConfigCCF(Hdlr);
        Hdlr->State = 2U;
    }

    FeedCfg.pCvAlgoHdlr = Hdlr;
    FeedCfg.pIn = Cfg->pIn;
    FeedCfg.pOut = Cfg->pOut;
    FeedCfg.pUserData = Cfg->pUserData;
    FeedCfg.pInternal = NULL;
    FeedCfg.pRaw = NULL;
    FeedCfg.pPic = NULL;
    Ret = CvCommFlexi_Feed(&Hdlr->FDs[0], &FeedCfg);

    return Ret;
}

SVC_CV_ALGO_OBJ_s AmbaOD37AlgoObj = {
    .Name       = "AmbaOD37",
    .Query      = AmbaOD37_Query,
    .Create     = AmbaOD37_Create,
    .Delete     = AmbaOD37_Delete,
    .Feed       = AmbaOD37_Feed,
    .Control    = AmbaOD37_Control
};


