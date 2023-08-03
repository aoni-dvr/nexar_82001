/**
 *  @file SCA_Stixel.c
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
 *  @details Implementation of SCA Stixel

 *
 */
#include "AmbaTypes.h"
#include "cvapi_memio_interface.h"
#include "cvapi_ambacv_flexidag.h"
#include "AmbaFS.h"
#include "AmbaUtility.h"
#include "SvcCvAlgo.h"
#include "cvapi_svccvalgo_stixel.h"

static void CCFCbHdlrFD0(const CCF_OUTPUT_s *pEvnetData)
{
    const SVC_CV_ALGO_HANDLE_s* Hdlr;
    UINT32 Idx;
    (void) ArmStdC_memcpy(&Hdlr, &(pEvnetData->pCvAlgoHdlr), sizeof(void*));
    for (Idx = 0U ; Idx < MAX_CALLBACK_NUM ; Idx++) {
        if (Hdlr->Callback[Idx] != NULL) {
            SVC_CV_ALGO_OUTPUT_s Out;
            Out.pUserData = pEvnetData->pUserData;
            Out.pOutput = pEvnetData->pOut;
            Out.pExtOutput = NULL;

            (void)Hdlr->Callback[Idx](CALLBACK_EVENT_FREE_INPUT, &Out);
            (void)Hdlr->Callback[Idx](CALLBACK_EVENT_OUTPUT, &Out);
        }
    }
}

static UINT32 Stixel_Create(SVC_CV_ALGO_HANDLE_s* Hdlr, SVC_CV_ALGO_CREATE_CFG_s *Cfg, const char *pFdName, UINT32 FdNameLen)
{
    CCF_TSK_CTRL_s Ctrl;
    CCF_CREATE_CFG_s CreCfg;
    CCF_OUTPUT_INFO_s OutInfo;
    CCF_REGCB_CFG_s CBCfg;
    UINT32 Idx;
    UINT32 Ret;

    // create mempool
    Hdlr->pAlgoBuf = Cfg->pAlgoBuf;
    Ret = ArmMemPool_Create(Hdlr->pAlgoBuf, &Hdlr->MemPoolId);

    // clean cb function;
    for (Idx = 0U ; Idx < MAX_CALLBACK_NUM; Idx++) {
        Hdlr->Callback[Idx] = NULL;
    }

    Hdlr->NumFD = 1U;

    //create cfg for FD0
    AmbaUtility_StringCopy(CreCfg.FDName, FdNameLen+1U, pFdName);
    Ctrl.CoreSel = 0xE;
    Ctrl.Priority = 50U;
    Ctrl.StackSz = ((UINT32)16U<<10U); //16K
    CreCfg.RunTskCtrl = Ctrl;

    Ctrl.CoreSel = 0xE;
    Ctrl.Priority = 51U;
    Ctrl.StackSz = ((UINT32)16U<<10U); //16K
    CreCfg.CBTskCtrl = Ctrl;

    CreCfg.InBufDepth = 0U;
    CreCfg.OutBufDepth = 0U;
    CreCfg.MemPoolId = Hdlr->MemPoolId;
    CreCfg.pBin = Cfg->pBin[0U];
    Ret |= CvCommFlexi_Create(&Hdlr->FDs[0U], &CreCfg, &OutInfo);

    //Register Callback for FD0
    if (Ret == CVALGO_OK) {
        CBCfg.Mode = 0U;
        CBCfg.Callback = CCFCbHdlrFD0;
        Ret = CvCommFlexi_RegCallback(&Hdlr->FDs[0U], &CBCfg);

        Cfg->OutputNum = OutInfo.OutputNum;
        for (Idx = 0U; Idx < OutInfo.OutputNum ; Idx++) {
            Cfg->OutputSz[Idx] = OutInfo.OutputSz[Idx];
        }

        Hdlr->State = 1U;
    }

    return Ret;
}

static UINT32 Stixel_Query(SVC_CV_ALGO_HANDLE_s* Hdlr, SVC_CV_ALGO_QUERY_CFG_s *Cfg, UINT32 TotalReqBufSz)
{
    // assign obj
    Hdlr->pAlgoObj = Cfg->pAlgoObj;

    //state buff + temp buff
    Cfg->TotalReqBufSz = TotalReqBufSz;
    return CVALGO_OK;
}

static UINT32 Stixel_Control(SVC_CV_ALGO_HANDLE_s* Hdlr, const SVC_CV_ALGO_CTRL_CFG_s *Cfg)
{
    UINT32 Ret = CVALGO_OK;
    (void)Hdlr;
    (void)Cfg;
    return Ret;
}

static UINT32 Stixel_Delete(SVC_CV_ALGO_HANDLE_s* Hdlr, const SVC_CV_ALGO_DELETE_CFG_s *Cfg)
{
    UINT32 Ret;
    (void)Cfg;
    Ret = CvCommFlexi_Delete(&Hdlr->FDs[0U]);
    if (Ret == CVALGO_OK) {
        Ret = ArmMemPool_Free(Hdlr->MemPoolId, Hdlr->pAlgoBuf);
        Ret = ArmMemPool_Delete(Hdlr->MemPoolId);
    }
    return Ret;
}

static UINT32 Stixel_Feed(SVC_CV_ALGO_HANDLE_s* Hdlr, const SVC_CV_ALGO_FEED_CFG_s *Cfg)
{
    CCF_FEED_CFG_s FeedCfg;
    UINT32 Ret;

    FeedCfg.pCvAlgoHdlr = Hdlr;
    FeedCfg.pIn = Cfg->pIn;
    FeedCfg.pOut = Cfg->pOut;
    FeedCfg.pUserData = Cfg->pUserData;
    Ret = CvCommFlexi_Feed(&Hdlr->FDs[0U], &FeedCfg);
    return Ret;
}

static UINT32 StixelPreprocess_Create(SVC_CV_ALGO_HANDLE_s* Hdlr, SVC_CV_ALGO_CREATE_CFG_s *Cfg)
{
    const char *pFdName = "StixelPreprocess";
    UINT32 FdNameLen = 16U;
    return Stixel_Create(Hdlr, Cfg, pFdName, FdNameLen);
}

static UINT32 StixelPreprocess_Query(SVC_CV_ALGO_HANDLE_s* Hdlr, SVC_CV_ALGO_QUERY_CFG_s *Cfg)
{
    UINT32 TotalReqBufSz = STIXEL_REQUIRED_CV_SIZE_PREPROCESS;
    return Stixel_Query(Hdlr, Cfg, TotalReqBufSz);
}

static UINT32 StixelHeightSeg_Create(SVC_CV_ALGO_HANDLE_s* Hdlr, SVC_CV_ALGO_CREATE_CFG_s *Cfg)
{
    const char *pFdName = "StixelHeightSeg";
    UINT32 FdNameLen = 15U;
    return Stixel_Create(Hdlr, Cfg, pFdName, FdNameLen);
}

static UINT32 StixelHeightSeg_Query(SVC_CV_ALGO_HANDLE_s* Hdlr, SVC_CV_ALGO_QUERY_CFG_s *Cfg)
{
    UINT32 TotalReqBufSz = STIXEL_REQUIRED_CV_SIZE_HEIGHT_SEG;
    return Stixel_Query(Hdlr, Cfg, TotalReqBufSz);
}

static UINT32 StixelGenEstiRoad_Create(SVC_CV_ALGO_HANDLE_s* Hdlr, SVC_CV_ALGO_CREATE_CFG_s *Cfg)
{
    const char *pFdName = "StixelGenEstiRoad";
    UINT32 FdNameLen = 17U;
    return Stixel_Create(Hdlr, Cfg, pFdName, FdNameLen);
}

static UINT32 StixelGenEstiRoad_Query(SVC_CV_ALGO_HANDLE_s* Hdlr, SVC_CV_ALGO_QUERY_CFG_s *Cfg)
{
    UINT32 TotalReqBufSz = STIXEL_REQUIRED_CV_SIZE_GEN_ESTI_ROAD;
    return Stixel_Query(Hdlr, Cfg, TotalReqBufSz);
}

static UINT32 StixelFreeSpace_Create(SVC_CV_ALGO_HANDLE_s* Hdlr, SVC_CV_ALGO_CREATE_CFG_s *Cfg)
{
    const char *pFdName = "StixelFreeSpace";
    UINT32 FdNameLen = 15U;
    return Stixel_Create(Hdlr, Cfg, pFdName, FdNameLen);
}

static UINT32 StixelFreeSpace_Query(SVC_CV_ALGO_HANDLE_s* Hdlr, SVC_CV_ALGO_QUERY_CFG_s *Cfg)
{
    UINT32 TotalReqBufSz = STIXEL_REQUIRED_CV_SIZE_FREE_SPACE;
    return Stixel_Query(Hdlr, Cfg, TotalReqBufSz);
}

static UINT32 StixelVDisp_Create(SVC_CV_ALGO_HANDLE_s* Hdlr, SVC_CV_ALGO_CREATE_CFG_s *Cfg)
{
    const char *pFdName = "StixelVDisp";
    UINT32 FdNameLen = 11U;
    return Stixel_Create(Hdlr, Cfg, pFdName, FdNameLen);
}

static UINT32 StixelVDisp_Query(SVC_CV_ALGO_HANDLE_s* Hdlr, SVC_CV_ALGO_QUERY_CFG_s *Cfg)
{
    UINT32 TotalReqBufSz = STIXEL_REQUIRED_CV_SIZE_VDISP;
    return Stixel_Query(Hdlr, Cfg, TotalReqBufSz);
}

SVC_CV_ALGO_OBJ_s StixelPreprocessAlgoObj = {
    .Query = StixelPreprocess_Query,
    .Create = StixelPreprocess_Create,
    .Delete = Stixel_Delete,
    .Feed = Stixel_Feed,
    .Control = Stixel_Control
};

SVC_CV_ALGO_OBJ_s StixelHeightSegAlgoObj = {
    .Query = StixelHeightSeg_Query,
    .Create = StixelHeightSeg_Create,
    .Delete = Stixel_Delete,
    .Feed = Stixel_Feed,
    .Control = Stixel_Control
};

SVC_CV_ALGO_OBJ_s StixelGenEstiRoadAlgoObj = {
    .Query = StixelGenEstiRoad_Query,
    .Create = StixelGenEstiRoad_Create,
    .Delete = Stixel_Delete,
    .Feed = Stixel_Feed,
    .Control = Stixel_Control
};

SVC_CV_ALGO_OBJ_s StixelFreeSpaceAlgoObj = {
    .Query = StixelFreeSpace_Query,
    .Create = StixelFreeSpace_Create,
    .Delete = Stixel_Delete,
    .Feed = Stixel_Feed,
    .Control = Stixel_Control
};

SVC_CV_ALGO_OBJ_s StixelVDispAlgoObj = {
    .Query = StixelVDisp_Query,
    .Create = StixelVDisp_Create,
    .Delete = Stixel_Delete,
    .Feed = Stixel_Feed,
    .Control = Stixel_Control
};

