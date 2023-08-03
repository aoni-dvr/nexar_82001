/**
 *  @file FlexidagAmbaOD.c
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
 *  @details Implementation of AmbaOD  FlexiDAG
 *
 */
#include "AmbaTypes.h"
#include "cvapi_memio_interface.h"
#include "cvapi_ambacv_flexidag.h"
#include "cvapi_flexidag_ref_util.h"
#include "cvapi_amba_od_interface.h"
#include "idsp_roi_msg.h"
#include "AmbaPrint.h"
#include "AmbaMisraFix.h"
#include "AmbaWrap.h"
#include "AmbaUtility.h"
#include "AmbaMMU.h"
#include "AmbaCache.h"
#include "AmbaFS.h"
#include "SvcCvAlgo.h"
#include "cvapi_svccvalgo_ambaod.h"


//#define DBG_LOG     AmbaPrint_PrintUInt5
//#define CRI_LOG     AmbaPrint_PrintUInt5
//#define DBG {AmbaPrint_PrintInt5("OBJ@%d", __LINE__, 0, 0, 0, 0);AmbaKAL_TaskSleep(50);}

static void ODCbHdlrFD0(const CCF_OUTPUT_s *pEvnetData)
{
    const SVC_CV_ALGO_HANDLE_s* Hdlr;
    UINT32 i;
    amba_od_out_t *pOut;
    AmbaMisra_TypeCast32(&Hdlr, &(pEvnetData->pCvAlgoHdlr));
    for (i = 0; i < MAX_CALLBACK_NUM; i++) {
        if (Hdlr->Callback[i] != NULL) {
            SVC_CV_ALGO_OUTPUT_s Out;
            Out.pUserData = pEvnetData->pUserData;
            Out.pOutput = pEvnetData->pOut;
            AmbaMisra_TypeCast32(&pOut,&(pEvnetData->pOut->buf[0].pBuffer));
            Out.pExtOutput = pOut;

            (void)Hdlr->Callback[i](CALLBACK_EVENT_FREE_INPUT, &Out);
            (void)Hdlr->Callback[i](CALLBACK_EVENT_OUTPUT, &Out);
        }
    }
}

static UINT32 AmbaOD_Query(SVC_CV_ALGO_HANDLE_s* Hdlr, SVC_CV_ALGO_QUERY_CFG_s *Cfg)
{
    const UINT32 *pNumScale;

    // assign obj
    Hdlr->pAlgoObj = Cfg->pAlgoObj;
    Cfg->TotalReqBufSz = ((UINT32)30U<<20U); //30M

    if (Cfg->pExtQueryCfg != NULL) {
        AmbaMisra_TypeCast32(&pNumScale, &Cfg->pExtQueryCfg);
        Cfg->TotalReqBufSz *= *pNumScale;
    }

    return CVALGO_OK;
}

static UINT32 AmbaOD_Control(SVC_CV_ALGO_HANDLE_s* Hdlr, const SVC_CV_ALGO_CTRL_CFG_s *Cfg)
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
    case AMBANET_MSG_TYPE_OD_STEP1:
        MsgSz = sizeof(amba_od_step1_config_t);
        if (Cfg->CtrlType == SCA_CT_AMBAOD_ROI_F0) {
            UUID = AMBAOD_VP_0_UUID;
        } else {
            UUID = AMBAOD_VP_1_UUID;
        }
        break;
    case AMBANET_MSG_TYPE_OD_NMS:
        MsgSz = sizeof(amba_od_nms_configs_t);
        UUID = AMBAOD_ARM_UUID;
        break;
    case AMBANET_MSG_TYPE_OD_GROUPING:
        MsgSz = sizeof(amba_od_grouping_t);
        UUID = AMBAOD_ARM_UUID;
        break;
    case AMBANET_MSG_TYPE_OD_ROIS:
        MsgSz = sizeof(amba_od_rois_config_t);
        UUID = AMBAOD_ARM_UUID;
        break;
    case AMBANET_MSG_TYPE_LICENSE:
        MsgSz = (sizeof(UINT32) * 256U);
        UUID = AMBAOD_VP_0_UUID;
        break;
    case AMBANET_MSG_TYPE_OD_BENCHMARK:
        MsgSz = 8U;
        UUID = AMBAOD_ARM_UUID;
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

static void AmbaOD_DefaultConfigCCF(SVC_CV_ALGO_HANDLE_s* pHdlr)
{
    AMBA_FS_FILE *Fp;
    const char *CfgFile = "ambaod_cfg.bin";
    uint32_t BytesRead, FileSize;
    uint32_t Ret;
    uint8_t Buf[sizeof(amba_od_step1_config_t) + \
                                               sizeof(amba_od_nms_configs_t) + \
                                               sizeof(amba_od_grouping_t) + \
                                               sizeof(amba_od_tracker_config_t) +
                                               sizeof(amba_od_rois_config_t)];
    uint8_t *pBuf = Buf;
    const amba_od_step1_config_t *Step1;
    const amba_od_nms_configs_t *Nms;
    const amba_od_grouping_t *Group;
    amba_od_tracker_config_t *Tracker;
    amba_roi_config_t roi_msg = {0};
    amba_roi_config_t *p_roi_msg = &roi_msg;
    SVC_CV_ALGO_CTRL_CFG_s MsgCfg;

    MsgCfg.pExtCtrlCfg = NULL;
    MsgCfg.CtrlType = 0;

    Ret = AmbaFS_FileOpen(CfgFile, "rb", &Fp);

    {
        /* To fix Misra-C not initialized variable issue. */
        AmbaMisra_TypeCast32(&Tracker, &pBuf);

        Tracker->frame_width = 0;
        Tracker->frame_height = 0;

        p_roi_msg->msg_type            = AMBA_ROI_CONFIG_MSG;
        p_roi_msg->image_pyramid_index = 0;
        p_roi_msg->source_vin          = 0;
        p_roi_msg->roi_start_col       = 0;
        p_roi_msg->roi_start_row       = 0;
        p_roi_msg->roi_width           = 0;
        p_roi_msg->roi_height          = 0;

    }

    if (Ret != 0U) {
        AmbaPrint_PrintUInt5("can't read config file!!",
                             0U, 0U, 0U, 0U, 0U);
    } else {
        (void) AmbaCV_UtilityFileSize(CfgFile, &FileSize);

        Ret = AmbaFS_FileRead(pBuf, 1U, (UINT32)FileSize, Fp, &BytesRead);
        if ((BytesRead == 0U) || (Ret !=  0U)) {
            AmbaPrint_PrintUInt5("Error while reading file!!",
                                 0U, 0U, 0U, 0U, 0U);
        }

        Ret = AmbaFS_FileClose(Fp);
        if(Ret !=  0U) {
            AmbaPrint_PrintUInt5("_AmbaOD_DefaultConfig: "
                                 "AmbaFS_FileClose fail",
                                 0U, 0U, 0U, 0U, 0U);
        }

        AmbaMisra_TypeCast32(&Step1, &pBuf);
        pBuf     = &Buf[sizeof(amba_od_step1_config_t)];
        AmbaMisra_TypeCast32(&Nms, &pBuf);

        pBuf     = &Buf[sizeof(amba_od_step1_config_t) + \
                                                       sizeof(amba_od_nms_configs_t)];
        AmbaMisra_TypeCast32(&Tracker, &pBuf);

        pBuf     = &Buf[sizeof(amba_od_step1_config_t) + \
                                                       sizeof(amba_od_nms_configs_t) + \
                                                       sizeof(amba_od_tracker_config_t)];
        AmbaMisra_TypeCast32(&Group, &pBuf);

        p_roi_msg->msg_type                = AMBA_ROI_CONFIG_MSG;
        p_roi_msg->image_pyramid_index     = Step1->image_pyramid_index;
        p_roi_msg->source_vin              = 0;
        p_roi_msg->roi_start_col           = Step1->roi_start_col;
        p_roi_msg->roi_start_row           = Step1->roi_start_row;
        p_roi_msg->roi_width               = Step1->roi_width;
        p_roi_msg->roi_height              = Step1->roi_height;

        AmbaMisra_TypeCast32(&MsgCfg.pCtrlParam, &p_roi_msg);
        (void)AmbaOD_Control(pHdlr, &MsgCfg);

        AmbaMisra_TypeCast32(&MsgCfg.pCtrlParam, &Nms);
        (void)AmbaOD_Control(pHdlr, &MsgCfg);

        AmbaMisra_TypeCast32(&MsgCfg.pCtrlParam, &Group);
        (void)AmbaOD_Control(pHdlr, &MsgCfg);
    }

    {
#define SQRT2       (1414213U)
#define BASE_VAL    (1000000U)
        amba_od_rois_config_t cfg5 = {0};
        amba_od_rois_config_t *pCfg5 = &cfg5;
        UINT64 Scale[MAX_HALF_OCTAVES];
        uint32_t i;

        Scale[0] = BASE_VAL;
        // pw[0]    = pRoiCfg->MainWidth * BASE_VAL;
        // ph[0]    = pRoiCfg->MainHeight * BASE_VAL;
        for (i = 1U; i < MAX_HALF_OCTAVES; i++) {
            Scale[i] = (Scale[i - 1U] * SQRT2) / BASE_VAL;
            // pw[i] = (pw[i - 1U] * BASE_VAL) / SQRT2;
            // ph[i] = (ph[i - 1U] * BASE_VAL) / SQRT2;
        }

        /* config step 5 */
        pCfg5->msg_type              = AMBANET_MSG_TYPE_OD_ROIS;
        pCfg5->frame_width           = (UINT16) Tracker->frame_width;
        pCfg5->frame_height          = (UINT16) Tracker->frame_height;
        pCfg5->pyramid_index[0]      = (UINT16) p_roi_msg->image_pyramid_index;
        pCfg5->pyramid_ratio_col[0]  = (UINT16)((Scale[p_roi_msg->image_pyramid_index] * 256U) / BASE_VAL);
        /* floor((s[cfg1.image_pyramid_index]*256)+0.5); */
        /* u8.8 phase increment x of each image pyramid input */
        /* with respect to pre-hierarchical resampler space */

        pCfg5->pyramid_ratio_row[0]  = (UINT16)((Scale[p_roi_msg->image_pyramid_index] * 256U) / BASE_VAL);
        /* floor((s[cfg1.image_pyramid_index]*256)+0.5); */
        /* u8.8 phase increment y of each image pyramid input */
        /* with respect to pre-hierarchical resampler space */

        pCfg5->roi_width[0]          = (UINT16) p_roi_msg->roi_width;
        pCfg5->roi_height[0]         = (UINT16) p_roi_msg->roi_height;

        pCfg5->pyramid_index[1]      = 0;
        pCfg5->pyramid_ratio_col[1]  = 256;
        pCfg5->pyramid_ratio_row[1]  = 256;
        pCfg5->roi_width[1]          = 0;
        pCfg5->roi_height[1]         = 0;

        pCfg5->pyramid_index[2]      = 0;
        pCfg5->pyramid_ratio_col[2]  = 256;
        pCfg5->pyramid_ratio_row[2]  = 256;
        pCfg5->roi_width[2]          = 0;
        pCfg5->roi_height[2]         = 0;

        /* global offset of each image pyramid input
         * in pre-hierarchical resampler space */
        pCfg5->pyramid_offset_col[0] = 0;
        pCfg5->pyramid_offset_row[0] = 0;
        pCfg5->roi_start_col[0]      = (UINT16) p_roi_msg->roi_start_col;    /* start x of each roi within its image */
        pCfg5->roi_start_row[0]      = (UINT16) p_roi_msg->roi_start_row;    /* start y of each roi within its image */

        pCfg5->pyramid_offset_col[1] = 0;
        pCfg5->pyramid_offset_row[1] = 0;
        pCfg5->roi_start_col[1]      = 0;
        pCfg5->roi_start_row[1]      = 0;

        pCfg5->pyramid_offset_col[2] = 0;
        pCfg5->pyramid_offset_row[2] = 0;
        pCfg5->roi_start_col[2]      = 0;
        pCfg5->roi_start_row[2]      = 0;

        AmbaMisra_TypeCast32(&MsgCfg.pCtrlParam, &pCfg5);
        (void)AmbaOD_Control(pHdlr, &MsgCfg);
    }

    {
        const char *LcsFile = "ambadef.lcs";
        UINT32 msg[258];
        const uint32_t *pU32;
        void* vp;

        /* read license filfe */
        Ret = AmbaFS_FileOpen(LcsFile, "rb", &Fp);

        if (Ret != 0U) {
            AmbaPrint_PrintUInt5("can't read license file!!",
                                 0U, 0U, 0U, 0U, 0U);
        } else {
            (void) AmbaCV_UtilityFileSize(LcsFile, &FileSize);

            msg[0] = AMBANET_MSG_TYPE_LICENSE;
            pU32 = &msg[1];
            AmbaMisra_TypeCast32(&vp, &pU32);
            Ret = AmbaFS_FileRead(vp, 1U, (UINT32)FileSize, Fp,
                                  &BytesRead);
            if ((BytesRead == 0U) || (Ret != 0U)) {
                AmbaPrint_PrintUInt5("Error while reading file!!",
                                     0U, 0U, 0U, 0U, 0U);
            }

            Ret = AmbaFS_FileClose(Fp);
            if(Ret != 0U) {
                AmbaPrint_PrintUInt5("_AmbaOD_DefaultConfig: "
                                     "AmbaFS_FileClose fail", 0U, 0U, 0U, 0U, 0U);
            }

            pU32 = &msg[0];
            AmbaMisra_TypeCast32(&MsgCfg.pCtrlParam, &pU32);
            (void)AmbaOD_Control(pHdlr, &MsgCfg);
        }
    }

    {
        UINT32 msg2[2];
        const uint32_t *pU32;

        /* Determine is it benchmark mode or not. */
        msg2[0] = AMBANET_MSG_TYPE_OD_BENCHMARK;
        /* 0x0: normal mode, 0x1: benchmark mode, 0x2: draw ROI window */
        msg2[1] = 0x0;
        pU32 = &msg2[0];

        AmbaMisra_TypeCast32(&MsgCfg.pCtrlParam, &pU32);
        (void)AmbaOD_Control(pHdlr, &MsgCfg);
    }
}

static UINT32 AmbaOD_Create(SVC_CV_ALGO_HANDLE_s* Hdlr, SVC_CV_ALGO_CREATE_CFG_s *Cfg)
{
    CCF_TSK_CTRL_s Ctrl;
    CCF_CREATE_CFG_s CreCfg;
    CCF_OUTPUT_INFO_s OutInfo;
    CCF_REGCB_CFG_s CBCfg;
    UINT32 i;
    UINT32 Ret;

    // create mempool
    Hdlr->pAlgoBuf = Cfg->pAlgoBuf;
    (void)ArmMemPool_Create(Hdlr->pAlgoBuf, &Hdlr->MemPoolId);
    // clean cb function;
    for (i = 0; i < MAX_CALLBACK_NUM; i++) {
        Hdlr->Callback[i] = NULL;
    }

    Hdlr->NumFD = 1U;

    //create ccf for FD0
    AmbaUtility_StringCopy(CreCfg.FDName, 8U, "AmbaOD36");
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

static UINT32 AmbaOD_Delete(SVC_CV_ALGO_HANDLE_s* Hdlr, const SVC_CV_ALGO_DELETE_CFG_s *Cfg)
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

static UINT32 AmbaOD_Feed(SVC_CV_ALGO_HANDLE_s* Hdlr, const SVC_CV_ALGO_FEED_CFG_s *Cfg)
{
    CCF_FEED_CFG_s FeedCfg;
    UINT32 Ret;

    if (Hdlr->State == 1U) {
        // send default config
        AmbaOD_DefaultConfigCCF(Hdlr);
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

SVC_CV_ALGO_OBJ_s AmbaODAlgoObj = {
    .Name       = "AmbaOD36",
    .Query = AmbaOD_Query,
    .Create = AmbaOD_Create,
    .Delete = AmbaOD_Delete,
    .Feed = AmbaOD_Feed,
    .Control = AmbaOD_Control
};


