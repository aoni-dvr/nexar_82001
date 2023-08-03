/**
 *  @file AmbaDSP_Main_Priv.c
 *
 *  Copyright (c) 2020 Ambarella International LP
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
 *  @details Implementation of SSP general API
 *
 */

#include "AmbaDSP_Log.h"
#include "AmbaDSP_Int.h"
#include "AmbaDSP_Cmds.h"
#include "AmbaDSP_ApiDump.h"
#include "AmbaDSP_ApiCheck.h"
#include "AmbaDSP_ApiTiming.h"
#include "AmbaDSP_EncodeAPI.h"
#include "AmbaDSP_EventCtrl.h"
#include "AmbaDSP_ContextUtility.h"
#include "AmbaDSP_EncodeUtility.h"
#include "dsp_priv_api.h"

//#define DEBUG_TESTFRAME_STATUS

//#define SUPPORT_BURST_TILE
#define HIER_ROI_EXTRA_ROW      (16U)
#define HIER_ROI_EXTRA_COL      (16U)
static inline UINT32 HEIR_2X_DOWNSCALE(UINT32 x)
{
    return (((((x) >> 1U) + 1U) >> 1U) << 1U);
}

#ifdef SUPPORT_BURST_TILE
static inline UINT32 HEIR_IMG_SIZE(UINT32 FullSize, UINT32 Idx)
{
    return (((FullSize + ((1U<<Idx)-1U)) >> Idx) << 1U);
}

static inline UINT32 GET_TILE_PITCH(UINT32 Width)
{
    UINT32 Pitch;

    if (512U >= Width) {
        Pitch = 512U;
    } else if (1024U >= Width) {
        Pitch = 1024U;
    } else if (2048U >= Width) {
        Pitch = 2048U;
    } else if (4096U >= Width) {
        Pitch = 4096U;
    } else if (8192U >= Width) {
        Pitch = 8192U;
    } else if (16384U >= Width) {
        Pitch = 16384U;
    } else {
        Pitch = 8192U;
    }

    return Pitch;
}

static inline UINT32 GET_BASE_ALIGNMENT(UINT32 Pitch)
{
    UINT32 PageSize;
    UINT32 Alignment;

    PageSize = DSP_GetPageSize();

    if (PageSize == 1024U) {
        Alignment = ((2U*32U)*Pitch);
    } else if ((PageSize == 2048U) || (PageSize == 4096U)) {
        Alignment = ((2U*64U)*Pitch);
    } else {
        Alignment = ((2U*128U)*Pitch);
    }
    return Alignment;
}

static inline UINT16 QuiteHierTiledHeight(const UINT16 *pMaxMainWidth, const UINT16 *pMaxMainHeight)
{
    UINT32 i;
    UINT32 TileRowsHeight, BufHeightAdj, BufPitch, BufSize;
    UINT32 PageSize;

    PageSize = DSP_GetPageSize();

    if (PageSize == 1024U) {
        TileRowsHeight = 32U*2U;
    } else if ((PageSize == 2048U) || (PageSize == 4096U)) {
        TileRowsHeight = 64U*2U;
    } else {
        TileRowsHeight = 128U*2U;
    }

    for (i = 0U; i<AMBA_DSP_MAX_HIER_NUM; i+=2U) {
        BufPitch = GET_TILE_PITCH(ALIGN_NUM(HEIR_IMG_SIZE((UINT32)(*pMaxMainWidth), ((i>>1U)+1U)) + HIER_ROI_EXTRA_COL, 8U));
        BufHeightAdj = ALIGN_NUM(HEIR_IMG_SIZE((UINT32)(*pMaxMainHeight), ((i>>1U)+1U)) + HIER_ROI_EXTRA_ROW, 16U) + 16U;
        BufHeightAdj = ((BufHeightAdj + (TileRowsHeight - 1U))/TileRowsHeight)*TileRowsHeight;
        BufSize += (BufPitch * BufHeightAdj);
    }

    BufPitch = GET_TILE_PITCH(ALIGN_NUM(HEIR_IMG_SIZE((UINT32)(*pMaxMainWidth), 1U) + HIER_ROI_EXTRA_COL, 8U));
    BufSize += GET_BASE_ALIGNMENT(BufPitch);

    for (i = 1U; i<AMBA_DSP_MAX_HIER_NUM; i+=2U) {
        BufPitch = GET_TILE_PITCH(ALIGN_NUM(HEIR_IMG_SIZE((UINT32)(*pMaxMainWidth), (((i - 1U)>>1U)+1U)) + HIER_ROI_EXTRA_COL, 8U));
        BufHeightAdj = ALIGN_NUM(HEIR_IMG_SIZE((UINT32)(*pMaxMainHeight), (((i - 1U)>>1U)+1U)) + HIER_ROI_EXTRA_ROW, 16U) + 16U;
        BufHeightAdj = ((BufHeightAdj + (TileRowsHeight - 1U))/TileRowsHeight)*TileRowsHeight;
        BufSize += (BufPitch * BufHeightAdj);
    }

    BufPitch = ALIGN_NUM(GET_TILE_PITCH(HEIR_IMG_SIZE((UINT32)(*pMaxMainWidth), 1U) + HIER_ROI_EXTRA_COL), 8U);
    BufHeightAdj = ((BufSize + (BufPitch - 1U))/BufPitch); //based on minimum tile pitch
    BufHeightAdj = ((BufHeightAdj + (TileRowsHeight - 1U))/TileRowsHeight)*TileRowsHeight;

    return BufHeightAdj;
}
#endif

/**
* HierBuffer query API
* @param [in]  pMaxMainWidth Max hier[0] width
* @param [in]  pMaxMainHeight Max hier[0] height
* @param [in]  pMaxHierWidth Max hier[1] width
* @param [in]  pMaxHierHeight Max hier[1] height
* @param [in]  OctaveMode, B[0:2] octave setting
*                          B[7:6] indicate LumaDepth, 0 means 8bit, 1 means 12bit
*                          B[15:8] hier disable bit
* @param [in/out]  pHierBufSize buffer size
* @param [in/out]  pHierBufWidth buffer width
* @param [in/out]  pHierBufHeight buffer height
* @return ErrorCode
*/
UINT32 dsp_cal_hier_buffer_size(const UINT16 *pMaxMainWidth, const UINT16 *pMaxMainHeight,
                                const UINT16 *pMaxHierWidth, const UINT16 *pMaxHierHeight,
                                const UINT16 OctaveMode,
                                UINT32 *pHierBufSize, UINT16 *pHierBufWidth, UINT16 *pHierBufHeight)
{
    UINT32 Rval = DSP_ERR_NONE, ErrLine = 0U, i, TempSize;
    UINT16 BufWidth, BufHeight;
    UINT16 HeirDisableMask;
    UINT16 MaxWidth = 0U;
    UINT16 OctMode = 0U;
    UINT16 LumaDepth = 0U;

    if ((pMaxMainWidth == NULL) || (pMaxMainHeight == NULL) ||
        (pHierBufSize == NULL) ||
        (pHierBufWidth == NULL) || (pHierBufHeight == NULL)) {
        Rval = DSP_ERR_0000;
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
    } else {
        //reset
        *pHierBufSize = 0U;
        *pHierBufWidth = 0U;
        *pHierBufHeight = 0U;

        HeirDisableMask = (UINT16)DSP_GetU16Bit(OctaveMode, 8U, 8U);
        OctMode = (UINT16)DSP_GetU16Bit(OctaveMode, DSP_PYMD_PLOY_IDX, DSP_PYMD_PLOY_LEN);
        LumaDepth = (UINT16)DSP_GetU16Bit(OctaveMode, DSP_PYMD_Y12_IDX, DSP_PYMD_Y12_LEN);

#ifdef SUPPORT_BURST_TILE
        if (1U/* HierBurstTile */) {
            BufWidth = *pMaxMainWidth + HIER_ROI_EXTRA_COL;
            BufHeight = QuiteHierTiledHeight(pMaxMainWidth, pMaxMainHeight);
        } else
#endif
        {
            BufWidth = *pMaxMainWidth;
            BufHeight = ALIGN_NUM16(*pMaxMainHeight, 16U);
            for (i=0U; i<AMBA_DSP_MAX_HIER_NUM; i+=2U) {
                BufWidth = BufWidth + HIER_ROI_EXTRA_COL;
                BufHeight = BufHeight + HIER_ROI_EXTRA_ROW;

                if (0U == DSP_GetU16Bit(HeirDisableMask, i, 1U)) {
                    MaxWidth = (MaxWidth < BufWidth)? BufWidth: MaxWidth;
                    *pHierBufSize += ALIGN_NUM((UINT32)BufWidth, (UINT32)DSP_BUF_ALIGNMENT) * BufHeight;
                }
                BufWidth = (UINT16)HEIR_2X_DOWNSCALE(BufWidth);
                BufHeight = (UINT16)HEIR_2X_DOWNSCALE(BufHeight);
            }

            if (OctMode == DSP_HIER_HALF_OCTAVE_MODE) {
                BufWidth = *pMaxHierWidth;
                BufHeight = *pMaxHierHeight;
            } else {
                /*
                 * 20180831, ChenHan's formula reserved for worst case(larger than exactly size)
                 */
                BufWidth = (*pMaxMainWidth)/2U;
                BufHeight = (ALIGN_NUM16(*pMaxMainHeight, 16U))/2U;
            }
            for (i=1U; i<AMBA_DSP_MAX_HIER_NUM; i+=2U) {
                BufWidth = BufWidth + HIER_ROI_EXTRA_COL;
                BufHeight = BufHeight + HIER_ROI_EXTRA_ROW;
                if (0U == DSP_GetU16Bit(HeirDisableMask, i, 1U)) {
                    MaxWidth = (MaxWidth < BufWidth)? BufWidth: MaxWidth;
                    *pHierBufSize += ALIGN_NUM((UINT32)BufWidth, (UINT32)DSP_BUF_ALIGNMENT)* BufHeight;
                }
                BufWidth = (UINT16)HEIR_2X_DOWNSCALE(BufWidth);
                BufHeight = (UINT16)HEIR_2X_DOWNSCALE(BufHeight);
            }

            //12bit Luma
            if (LumaDepth == 1U) {
                MaxWidth = (MaxWidth*3U)>>1U;
            }

            *pHierBufWidth = ALIGN_NUM16(MaxWidth, (UINT16)DSP_BUF_ALIGNMENT);
            BufWidth = *pHierBufWidth - 1U;
            TempSize = *pHierBufSize + BufWidth;
            *pHierBufHeight = (UINT16)(TempSize/(*pHierBufWidth));
            *pHierBufHeight = ALIGN_NUM16(*pHierBufHeight, 2U);
            *pHierBufSize = (UINT32)(*pHierBufWidth)*(UINT32)(*pHierBufHeight);

//            AmbaLL_LogUInt5("Calc Buf %d %dx%d, Max %dx%d", *pHierBufSize, *pHierBufWidth, *pHierBufHeight, *pMaxMainWidth, *pMaxMainHeight);
        }
    }
    if (Rval != DSP_ERR_NONE) {
        AmbaLL_LogUInt5("dsp_cal_hier_buffer_size Error[0x%x][%u]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

UINT32 dsp_calc_vp_msg_buf_size(const UINT32 *NumMsgs, UINT32 *MsgSize)
{
    UINT32 Rval = DSP_ERR_NONE, ErrLine = 0U;

    if ((NumMsgs == NULL) || (MsgSize == NULL)) {
        Rval = DSP_ERR_0000;
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
    } else {
        *MsgSize = (*NumMsgs)*ALIGN_NUM(AMBA_DSP_HIER_UNIT_SIZE, 128U);
    }
    if (Rval != DSP_ERR_NONE) {
        AmbaLL_LogUInt5("dsp_calc_vp_msg_buf_size Error[0x%x][%u]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static inline UINT32 HL_TestFrameConfigChk(UINT32 StageId, const AMBA_DSP_TEST_FRAME_CFG_s *pTestFrame)
{
    UINT32 Rval = DSP_ERR_NONE, ErrLine = 0U;
    const Dsp_TimingApi_t* TimingApi = AmbaDSP_GetTimingApiFunc();
    const Dsp_DumpApi_t* DumpApi = AmbaDSP_GetDumpApiFunc();
    const Dsp_CheckApi_t* CheckApi = AmbaDSP_GetCheckApiFunc();

    /* Timing sanity check */
    if (TimingApi != NULL) {
        Rval = TimingApi->pTestFrameConfig(StageId, pTestFrame);
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
    }
    /* Input sanity check */
    if ((Rval == DSP_ERR_NONE) &&
        (DumpApi != NULL)) {
        DumpApi->pTestFrameConfig(StageId, pTestFrame);
    }
    if ((Rval == DSP_ERR_NONE) &&
        (CheckApi != NULL)) {
        Rval = CheckApi->pTestFrameConfig(StageId, pTestFrame);
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
    }
    /* Logic sanity check */

    if (Rval != DSP_ERR_NONE) {
        AmbaLL_LogUInt5("HL_TestFrameConfigChk Error[0x%x][%u]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

UINT32 dsp_test_frame_cfg(UINT32 StageId, const AMBA_DSP_TEST_FRAME_CFG_s *pCfg)
{
    UINT32 Rval, ErrLine = 0U;

    Rval = HL_TestFrameConfigChk(StageId, pCfg);
    DSP_FillErrline(Rval, &ErrLine, __LINE__);

    /* Body */
    if (Rval == DSP_ERR_NONE) {
        UINT16 i;
        CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;
        CTX_VPROC_INFO_s VprocInfo;

        HL_GetVprocInfo(HL_MTX_OPT_ALL, 0, &VprocInfo);
        HL_GetResourceLock(&Resource);

#ifdef DEBUG_TESTFRAME_STATUS
        AmbaLL_LogUInt5("dsp_test_frame_cfg %u StageId[%u] TestFrmStatus[%u] RescState[%u] VprocStat[%u]",
                __LINE__, StageId, Resource->TestFrmStatus[StageId], Resource->RescState, VprocInfo.Status);
#endif
        if (((Resource->TestFrmStatus[StageId] == DSP_TESTFRAME_STATE_DISABLE) ||
             (Resource->TestFrmStatus[StageId] == DSP_TESTFRAME_STATE_CONFIG) ||
             (Resource->TestFrmStatus[StageId] == DSP_TESTFRAME_STATE_STOP)) &&
            (Resource->RescState == HL_RESC_CONFIGED) &&
            (VprocInfo.Status < DSP_VPROC_STATUS_ACTIVE)) {

            Resource->TestFrmNumOnStage[StageId] = pCfg->NumTestFrame;
            Resource->TestFrmInterval[StageId] = pCfg->Interval;
            for (i=0U; i<DSP_MAX_TEST_FRAME_NUM; i++) {
                if (pCfg->TestFrameCfg[i] != 0U) {
                    Resource->TestCfgOnStage[StageId][i] = pCfg->TestFrameCfg[i];
#ifdef DEBUG_TESTFRAME_STATUS
                    AmbaLL_LogUInt5("dsp_test_frame_cfg stage:%u TestCfgOnStage[%u]: 0x%x",
                            StageId, i, Resource->TestCfgOnStage[StageId][i], 0U, 0U);
#endif
                }
            }

            if (StageId <= DSP_TEST_STAGE_IDSP_1) {
                const UINT8 TestInpStage[2U] = {
                        [DSP_TEST_STAGE_IDSP_0] = DSP_MAX_TEST_INP_STAGE0,
                        [DSP_TEST_STAGE_IDSP_1] = DSP_MAX_TEST_INP_STAGE1,
                };

                for (i=0U; i<TestInpStage[StageId]; i++) {
                    if (pCfg->TestFrameInp[i] != 0U) {
                        Resource->TestFrmInpTable[StageId][i] = pCfg->TestFrameInp[i];
#ifdef DEBUG_TESTFRAME_STATUS
                        AmbaLL_LogUInt5("dsp_test_frame_cfg stage:%u InpTable[%u]: 0x%x",
                                StageId, i, Resource->TestFrmInpTable[StageId][i], 0U, 0U);
#endif
                    }
                }
#ifdef DEBUG_TESTFRAME_STATUS
                for (i=0U; i<pCfg->NumTestFrame; i++) {
                    Resource->TestFrmOrd[StageId][i] = pCfg->TestFrameOrd[i];
                    AmbaLL_LogUInt5("dsp_test_frame_cfg stage:%u TestFrmOrd:[%u] 0x%x",
                            StageId, i, Resource->TestFrmOrd[StageId][i], 0U, 0U);
                }
#endif
            }

            if (StageId == DSP_TEST_STAGE_VDSP_0) {
                Resource->pTestEncBsBuf = pCfg->pBitsBufAddr;
                Resource->TestEncBsSize = pCfg->BitsBufSize;
            }
#ifdef DEBUG_TESTFRAME_STATUS
            AmbaLL_LogUInt5("dsp_test_frame_cfg %u TestFrmStatus [%u]: %u -> %u",
                    __LINE__, StageId, Resource->TestFrmStatus[StageId], DSP_TESTFRAME_STATE_CONFIG, 0U);
#endif
            Resource->TestFrmStatus[StageId] = DSP_TESTFRAME_STATE_CONFIG;
        } else {    /* test frame could only config before dsp boot */
            Rval = DSP_ERR_0004;
        }
        HL_GetResourceUnLock();
    }

    if (Rval != DSP_ERR_NONE) {
        AmbaLL_LogUInt5("dsp_test_frame_cfg Error[0x%x][%u]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

static inline UINT32 HL_TestFrameCtrlChk(UINT32 NumStage, const AMBA_DSP_TEST_FRAME_CTRL_s *pCtrl)
{
    UINT32 Rval = DSP_ERR_NONE, ErrLine = 0U;
    const Dsp_TimingApi_t* TimingApi = AmbaDSP_GetTimingApiFunc();
    const Dsp_DumpApi_t* DumpApi = AmbaDSP_GetDumpApiFunc();
    const Dsp_CheckApi_t* CheckApi = AmbaDSP_GetCheckApiFunc();

    /* Timing sanity check */
    if (TimingApi != NULL) {
        Rval = TimingApi->pTestFrameCtrl(NumStage, pCtrl);
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
    }
    /* Input sanity check */
    if ((Rval == DSP_ERR_NONE) &&
        (DumpApi != NULL)) {
        DumpApi->pTestFrameCtrl(NumStage, pCtrl);
    }
    if ((Rval == DSP_ERR_NONE) &&
        (CheckApi != NULL)) {
        Rval = CheckApi->pTestFrameCtrl(NumStage, pCtrl);
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
    }
    /* Logic sanity check */

    if (Rval != DSP_ERR_NONE) {
        AmbaLL_LogUInt5("HL_TestFrameCtrlChk Error[0x%x][%u]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

static inline UINT32 HL_TestFrameCtrlOnIdsp(const AMBA_DSP_TEST_FRAME_CTRL_s *pCtrl,
                                            const CTX_RESOURCE_INFO_s *pResource)
{
//FIXME. DSP assertion when run-time enable
    UINT8 WriteMode = AMBA_DSP_CMD_VDSP_NORMAL_WRITE;
    UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
    UINT16 j;
    UINT32 Cnt = 0U;
    UINT32 Rval, ErrLine = 0U;
    UINT32 *pBufTblAddr = NULL;
    ULONG ULAddr = 0U;
    cmd_vproc_set_testframe_t *VprocTestFrm = HL_DefCtxCmdBufPtrVpcTestFrm;

    DSP_Bit2Cnt(pCtrl->EnableMask , &Cnt);
    HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
    dsp_osal_typecast(&VprocTestFrm, &CmdBufferAddr);
    if (pCtrl->StageId == DSP_TEST_STAGE_IDSP_0) {
        VprocTestFrm->type = DSP_VPROC_C2Y_TESTFRAME_TYPE;
    } else {
        VprocTestFrm->type = DSP_VPROC_Y2Y_TESTFRAME_TYPE;
    }

    VprocTestFrm->num_of_cases = (UINT8)Cnt;
    VprocTestFrm->is_sending_jobs = (UINT8)1U;
    VprocTestFrm->tf_repeat_mode = pCtrl->IsRepeat;
    VprocTestFrm->issue_tf_interval = pResource->TestFrmInterval[pCtrl->StageId]/10U;

    /* order */
    for (j = 0U; j < DSP_MAX_TEST_FRAME_NUM; j++) {
        VprocTestFrm->tc_order[j] = (UINT8)pCtrl->Order[j];
    }

    /* input table */
    HL_GetPointerToDspTestFrmInpTbl(pCtrl->StageId, &pBufTblAddr);
    if (pBufTblAddr != NULL) {
        dsp_osal_typecast(&ULAddr, &pBufTblAddr);
    }
    Rval = dsp_osal_virt2cli(ULAddr, &VprocTestFrm->tf_in_tbl_addr);

    if (Cnt > 0U) {
        VprocTestFrm->stop_tf = 0U;
    } else {
        VprocTestFrm->stop_tf = 1U;
    }

    if (Rval == DSP_ERR_NONE) {
        Rval = AmbaHL_CmdVprocSetTestFrame(WriteMode, VprocTestFrm);
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
    }
    HL_RelCmdBuffer(CmdBufferId);

    if (Rval != DSP_ERR_NONE) {
        AmbaLL_LogUInt5("HL_TestFrameCtrlOnIdsp Error[0x%x][%u]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

static inline UINT32 HL_TFCtrlEncBind(const CTX_RESOURCE_INFO_s *pResource,
                                      const CTX_STREAM_INFO_s *pStrmInfo,
                                      const CTX_DSP_INST_INFO_s *pDspInstInfo)
{
    UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
    UINT32 Rval = DSP_ERR_NONE, ErrLine = 0U;
    UINT8 WriteMode = AMBA_DSP_CMD_VDSP_NORMAL_WRITE;
    cmd_binding_cfg_t *FpBindingCfg = HL_DefCtxCmdBufPtrBind;

    // EncBind
    if (pStrmInfo->CfgStatus == ENC_STRM_CFG_NONE) {
        HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
        dsp_osal_typecast(&FpBindingCfg, &CmdBufferAddr);
        Rval = HL_FillFpBindSetup(FpBindingCfg,
                                  BIND_VIDEO_INTERVAL_ENCODE,
                                  1U/*IsBind*/,
                                  pDspInstInfo->TestEncVirtVinId + AMBA_DSP_MAX_VIN_NUM,/*VinId*/
                                  0U/*SrcPinId*/,
                                  pResource->TestEncStrmId);
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
        if (Rval == DSP_ERR_NONE) {
            WriteMode = HL_GetVin2CmdNormalWrite((UINT16)(pDspInstInfo->TestEncVirtVinId + AMBA_DSP_MAX_VIN_NUM));
            Rval = AmbaHL_CmdDspBindingCfg(WriteMode, FpBindingCfg);
            DSP_FillErrline(Rval, &ErrLine, __LINE__);
        }
        HL_RelCmdBuffer(CmdBufferId);
    }

    if (Rval != DSP_ERR_NONE) {
        AmbaLL_LogUInt5("HL_TFCtrlEncBind Error[0x%x][%u]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

static inline UINT32 HL_TFCtrlEncEncCmd(const CTX_RESOURCE_INFO_s *pResource,
                                        CTX_STREAM_INFO_s *pStrmInfo,
                                        const UINT32 FrameMode)
{
    UINT8 WriteMode = AMBA_DSP_CMD_VDSP_NORMAL_WRITE;
    UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
    UINT32 Rval = DSP_ERR_NONE, ErrLine = 0U;
    ULONG ULAddr = 0U;
    cmd_encoder_setup_t *EncodeSetup = HL_DefCtxCmdBufPtrEnc;
    cmd_encoder_start_t *EncStart = HL_DefCtxCmdBufPtrEncStart;

    //EncSetup
    if ((pStrmInfo->CfgStatus == ENC_STRM_CFG_NONE) ||
        (pStrmInfo->CfgStatus == ENC_STRM_CFG_UPDATE)) {
        HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
        dsp_osal_typecast(&EncodeSetup, &CmdBufferAddr);
        EncodeSetup->stream_id = (UINT8)(pResource->TestEncStrmId);
        dsp_osal_typecast(&ULAddr, &pResource->pTestEncBsBuf);
        Rval = dsp_osal_virt2cli(ULAddr, &EncodeSetup->bits_fifo_base);
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
        EncodeSetup->bits_fifo_size = pResource->TestEncBsSize;
        EncodeSetup->bits_fifo_offset = 0U;
        DSP_GetBitsDescBufferAddr(&ULAddr);
        Rval = dsp_osal_virt2cli(ULAddr, &EncodeSetup->info_fifo_base);
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
        EncodeSetup->info_fifo_size = AMBA_DSP_VIDEOENC_DESC_BUF_SIZE;
        Rval = dsp_osal_virt2cli(pResource->TestCfgOnStage[DSP_TEST_STAGE_VDSP_0][0U], &EncodeSetup->test_binary_daddr);
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
        EncodeSetup->enc_test_cmd_mode = (UINT8)FrameMode;
        Rval = AmbaHL_CmdEncoderSetup(WriteMode, EncodeSetup);
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
        HL_RelCmdBuffer(CmdBufferId);
    }

    //EncStart
    if ((Rval == DSP_ERR_NONE) &&
        ((pStrmInfo->CfgStatus == ENC_STRM_CFG_NONE) ||
         (pStrmInfo->CfgStatus == ENC_STRM_CFG_UPDATE))) {

        HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
        dsp_osal_typecast(&EncStart, &CmdBufferAddr);
        EncStart->stream_id = (UINT8)(pResource->TestEncStrmId);
        Rval = AmbaHL_CmdEncoderStart(WriteMode, EncStart);
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
        HL_RelCmdBuffer(CmdBufferId);
        HL_GetStrmInfo(HL_MTX_OPT_GET, pResource->TestEncStrmId, pStrmInfo);
        pStrmInfo->CfgStatus = ENC_STRM_CFG_DONE;
        HL_SetStrmInfo(HL_MTX_OPT_SET, pResource->TestEncStrmId, pStrmInfo);

        if (Rval == DSP_ERR_NONE) {
            Rval = DSP_WaitVdspEvent((UINT32)EVENT_FLAG_INT_VDSP0_EVENT, 4, WAIT_INTERRUPT_TIMEOUT);
            DSP_FillErrline(Rval, &ErrLine, __LINE__);
        }
    }

    if (Rval != DSP_ERR_NONE) {
        AmbaLL_LogUInt5("HL_TFCtrlEncEncCmd Error[0x%x][%u]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

static inline UINT32 HL_TFCtrlEncVinCmd(const CTX_RESOURCE_INFO_s *pResource,
                                        CTX_STREAM_INFO_s *pStrmInfo,
                                        const CTX_DSP_INST_INFO_s *pDspInstInfo,
                                        const ULONG LumaOffest,
                                        const ULONG MeOffest)
{
    UINT8 VinId;
    UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
    UINT8 WriteMode = HL_GetVin2CmdNormalWrite((UINT16)(pDspInstInfo->TestEncVirtVinId + AMBA_DSP_MAX_VIN_NUM));
    UINT32 Rval, ErrLine = 0U;
    UINT32 *pBufTblAddr = NULL;
    ULONG ULAddr = 0U;
    test_binary_header_t EncHdr = {0};
    cmd_vin_set_ext_mem_t *pVinExtMem;

    HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
    dsp_osal_typecast(&pVinExtMem, &CmdBufferAddr);

    //SetExtMem0
    //YuvData
    VinId = (UINT8)(pDspInstInfo->TestEncVirtVinId + AMBA_DSP_MAX_VIN_NUM);
    pVinExtMem->vin_id_or_chan_id = VinId;
    pVinExtMem->memory_type = DSP_VIN_EXT_MEM_TYPE_YUV422;
    pVinExtMem->allocation_type = VIN_EXT_MEM_ALLOC_TYPE_DISTINCT;
    pVinExtMem->allocation_mode = (pStrmInfo->TotalEncodeNum > 0U)? VIN_EXT_MEM_MODE_APPEND: VIN_EXT_MEM_MODE_NEW;
    pVinExtMem->overflow_cntl = VIN_EXT_MEM_CTRL_WAIT_APPEND;
    pVinExtMem->num_frm_buf = MAX_EXT_MEM_BUF_NUM;
    Rval = HL_GetTestEncHeader(&EncHdr, pResource);
    DSP_FillErrline(Rval, &ErrLine, __LINE__);
    pVinExtMem->buf_pitch = (UINT16)EncHdr.luma0_p;
    pVinExtMem->buf_width = (UINT16)EncHdr.luma0_w;
    pVinExtMem->buf_height = (UINT16)EncHdr.luma0_h;
    pVinExtMem->chroma_format = DSP_YUV_420;
#ifdef SUPPORT_DSP_VIN_YUV_ENC_FBP_CTRL
    pVinExtMem->vin_yuv_enc_fbp_disable = 0U;
#endif
    HL_GetPointerToDspExtRawBufArray(pDspInstInfo->TestEncVirtVinId + AMBA_DSP_MAX_VIN_NUM,
                                     0U/*Raw*/,
                                     &pBufTblAddr);
    Rval = dsp_osal_virt2cli(LumaOffest, &pBufTblAddr[0U]);
    DSP_FillErrline(Rval, &ErrLine, __LINE__);
    dsp_osal_typecast(&ULAddr, &pBufTblAddr);

    if (Rval == DSP_ERR_NONE) {
        Rval = dsp_osal_virt2cli(ULAddr, &pVinExtMem->buf_addr);
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
    }

    if (Rval == DSP_ERR_NONE) {
        Rval = AmbaHL_CmdVinSetExtMem(WriteMode, pVinExtMem);
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
    }

    //MeData
    pVinExtMem->memory_type = DSP_VIN_EXT_MEM_TYPE_ME;
    pVinExtMem->allocation_mode = (pStrmInfo->TotalEncodeNum > 0U)? VIN_EXT_MEM_MODE_APPEND: VIN_EXT_MEM_MODE_NEW;
    pVinExtMem->buf_pitch = (UINT16)EncHdr.me0_p;
    pVinExtMem->buf_width = (UINT16)EncHdr.me0_w;
    pVinExtMem->buf_height = (UINT16)EncHdr.me0_h;
//FIXME, HL need valid aux_w to create fb, so we must fill it and set aux_h = 0 if no me0 data needed
    pVinExtMem->aux_pitch = (UINT16)EncHdr.me0_p/2U;
    pVinExtMem->aux_width = (UINT16)EncHdr.me0_w/2U;
    pVinExtMem->aux_height = 0U;
    pVinExtMem->chroma_format = YUV_MONO;
#ifdef SUPPORT_DSP_VIN_YUV_ENC_FBP_CTRL
    pVinExtMem->vin_yuv_enc_fbp_disable = 0U;
#endif
    HL_GetPointerToDspExtRawBufArray(pDspInstInfo->TestEncVirtVinId + AMBA_DSP_MAX_VIN_NUM,
                                     1U/*Me*/,
                                     &pBufTblAddr);
    if (Rval == DSP_ERR_NONE) {
        Rval = dsp_osal_virt2cli(MeOffest, &pBufTblAddr[0U]);
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
        dsp_osal_typecast(&ULAddr, &pBufTblAddr);
        Rval = dsp_osal_virt2cli(ULAddr, &pVinExtMem->buf_addr);
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
        Rval = AmbaHL_CmdVinSetExtMem(WriteMode, pVinExtMem);
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
    }
    HL_RelCmdBuffer(CmdBufferId);

    //SetInputData0
    if (Rval == DSP_ERR_NONE) {
        cmd_vin_send_input_data_t *pVinFeedData;

        HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
        dsp_osal_typecast(&pVinFeedData, &CmdBufferAddr);
        pVinFeedData->vin_id = VinId;
        pVinFeedData->chan_id = 0U; //obsoleted
        pVinFeedData->send_data_dest = DSP_VIN_SEND_IN_DATA_DEST_ENC;
        pVinFeedData->encode_start_idc = (pStrmInfo->TotalEncodeNum > 0U)? 0U: 1U;
        pVinFeedData->encode_stop_idc = 0U;
        pVinFeedData->input_data_type = DSP_VIN_SEND_IN_DATA_YUV420;
        pVinFeedData->ext_fb_idx = DSP_FB_IDX_FOR_NEW2APPEND;
        pVinFeedData->ext_ce_fb_idx = DSP_FB_IDX_FOR_NEW2APPEND;
        pVinFeedData->hw_pts = 0U;

        Rval = AmbaHL_CmdVinSendInputData(WriteMode, pVinFeedData);
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
        if (Rval == DSP_ERR_NONE) {
            HL_GetStrmInfo(HL_MTX_OPT_GET, pResource->TestEncStrmId, pStrmInfo);
            pStrmInfo->TotalEncodeNum += 1ULL;
            HL_SetStrmInfo(HL_MTX_OPT_SET, pResource->TestEncStrmId, pStrmInfo);
        }
        HL_RelCmdBuffer(CmdBufferId);
    }

    if (Rval != DSP_ERR_NONE) {
        AmbaLL_LogUInt5("HL_TFCtrlEncVinCmd Error[0x%x][%u]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

static inline UINT32 HL_TFCtrlEncEncStopCmd(const CTX_RESOURCE_INFO_s *pResource,
                                            CTX_STREAM_INFO_s *pStrmInfo)
{
    UINT8 WriteMode = AMBA_DSP_CMD_VDSP_NORMAL_WRITE;
    UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
    UINT32 Rval, ErrLine = 0U;
    cmd_encoder_stop_t *EncStop = HL_DefCtxCmdBufPtrEncStop;

    HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
    dsp_osal_typecast(&EncStop, &CmdBufferAddr);
    EncStop->channel_id = (UINT8)(pResource->TestEncStrmId);
    EncStop->stop_method = DSP_ENC_STOP_FROM_CMD;
    HL_GetStrmInfo(HL_MTX_OPT_GET, pResource->TestEncStrmId, pStrmInfo);
    pStrmInfo->CfgStatus = ENC_STRM_CFG_UPDATE;
    HL_SetStrmInfo(HL_MTX_OPT_SET, pResource->TestEncStrmId, pStrmInfo);
    Rval = AmbaHL_CmdEncoderStop(WriteMode, EncStop);
    DSP_FillErrline(Rval, &ErrLine, __LINE__);
    HL_RelCmdBuffer(CmdBufferId);

    return Rval;
}

static inline UINT32 HL_TFCtrlEncUnBind(const CTX_RESOURCE_INFO_s *pResource,
                                        const CTX_STREAM_INFO_s *pStrmInfo,
                                        const CTX_DSP_INST_INFO_s *pDspInstInfo)
{
    UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
    UINT32 Rval = DSP_ERR_NONE, ErrLine = 0U;
    UINT8 WriteMode = AMBA_DSP_CMD_VDSP_NORMAL_WRITE;
    cmd_binding_cfg_t *FpBindingCfg = HL_DefCtxCmdBufPtrBind;

    // EncBind
    if (pStrmInfo->CfgStatus == ENC_STRM_CFG_NONE) {
        HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
        dsp_osal_typecast(&FpBindingCfg, &CmdBufferAddr);
        Rval = HL_FillFpBindSetup(FpBindingCfg,
                                  BIND_VIDEO_INTERVAL_ENCODE,
                                  0U/*IsBind*/,
                                  pDspInstInfo->TestEncVirtVinId + AMBA_DSP_MAX_VIN_NUM,/*VinId*/
                                  0U/*SrcPinId*/,
                                  pResource->TestEncStrmId);
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
        if (Rval == DSP_ERR_NONE) {
            WriteMode = HL_GetVin2CmdNormalWrite((UINT16)(pDspInstInfo->TestEncVirtVinId + AMBA_DSP_MAX_VIN_NUM));
            Rval = AmbaHL_CmdDspBindingCfg(WriteMode, FpBindingCfg);
            DSP_FillErrline(Rval, &ErrLine, __LINE__);
        }
        HL_RelCmdBuffer(CmdBufferId);
    }

    if (Rval != DSP_ERR_NONE) {
        AmbaLL_LogUInt5("HL_TFCtrlEncUnBind Error[0x%x][%u]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

static inline void GetEncTestId(const UINT32 EnableMask, UINT32 *EnableId)
{
    if (EnableMask == 1U) {
        // 0: GOP N2M1 for IPIPIP with 1 slice (default)
        *EnableId = 0U;
    } else if (EnableMask == 2U) {
        // 1: GOP N4M1 for IPPPIPPP with 4 slices
        *EnableId = 1U;
    } else if (EnableMask == 4U) {
        // 2: GOP N5M1 for IPPPPIPPPP with 3 slices
        *EnableId = 2U;
    } else {
        *EnableId = 0U;
    }
}

static inline UINT32 HL_TestFrameCtrlOnVdspEnc(const AMBA_DSP_TEST_FRAME_CTRL_s *pCtrl,
                                               const CTX_RESOURCE_INFO_s *pResource)
{
    UINT32 Rval = DSP_ERR_NONE, ErrLine = 0U;
    CTX_DSP_INST_INFO_s DspInstInfo = {0};
    CTX_STREAM_INFO_s StrmInfo = {0};

    HL_GetDspInstance(HL_MTX_OPT_ALL, &DspInstInfo);
    HL_GetStrmInfo(HL_MTX_OPT_ALL, pResource->TestEncStrmId, &StrmInfo);
#ifdef DEBUG_TESTFRAME_STATUS
    AmbaLL_LogUInt5("HL_TestFrameCtrlOnVdspEnc EnableMask[0x%x] TestEncVirtVinId[%u] CfgStatus[%u] TotalEncodeNum[%u] TestFrmStatus[%u]",
            pCtrl->EnableMask, DspInstInfo.TestEncVirtVinId,
            StrmInfo.CfgStatus, StrmInfo.TotalEncodeNum, pResource->TestFrmStatus[DSP_TEST_STAGE_VDSP_0]);
#endif
    if (pCtrl->EnableMask > 0U) {
        if (pResource->TestFrmStatus[DSP_TEST_STAGE_VDSP_0] == DSP_TESTFRAME_STATE_ENABLE) {
            ULONG VdspCfgBaseAddr = pResource->TestCfgOnStage[DSP_TEST_STAGE_VDSP_0][0U];

            Rval = HL_TFCtrlEncBind(pResource,
                                    &StrmInfo,
                                    &DspInstInfo);
            DSP_FillErrline(Rval, &ErrLine, __LINE__);
            if (Rval == DSP_ERR_NONE) {
                UINT32 EnableId = 0U;

                GetEncTestId(pCtrl->EnableMask , &EnableId);
                Rval = HL_TFCtrlEncEncCmd(pResource,
                                          &StrmInfo,
                                          EnableId);
                DSP_FillErrline(Rval, &ErrLine, __LINE__);
            }
            if (Rval == DSP_ERR_NONE) {
                Rval = HL_TFCtrlEncVinCmd(pResource,
                                          &StrmInfo,
                                          &DspInstInfo,
                                          VdspCfgBaseAddr + ENC_TEST_LUMA0_OFFSET,
                                          VdspCfgBaseAddr + ENC_TEST_ME0_OFFSET);
                DSP_FillErrline(Rval, &ErrLine, __LINE__);
                if (Rval == DSP_ERR_NONE) {
                    Rval = DSP_WaitVdspEvent((UINT32)EVENT_FLAG_INT_VDSP1_EVENT, 2U, WAIT_INTERRUPT_TIMEOUT);
                    DSP_FillErrline(Rval, &ErrLine, __LINE__);
                }
            }

            if (Rval == DSP_ERR_NONE) {
                Rval = HL_TFCtrlEncVinCmd(pResource,
                                          &StrmInfo,
                                          &DspInstInfo,
                                          VdspCfgBaseAddr + ENC_TEST_LUMA1_OFFSET,
                                          VdspCfgBaseAddr + ENC_TEST_ME1_OFFSET);
                DSP_FillErrline(Rval, &ErrLine, __LINE__);
            }
        } else {
            Rval = DSP_ERR_0004;
        }
    } else {
        Rval = HL_TFCtrlEncEncStopCmd(pResource, &StrmInfo);
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
        if (Rval == DSP_ERR_NONE) {
            Rval = DSP_WaitVdspEvent((UINT32)EVENT_FLAG_INT_VDSP1_EVENT, 2U, WAIT_INTERRUPT_TIMEOUT);
            DSP_FillErrline(Rval, &ErrLine, __LINE__);
            Rval = HL_TFCtrlEncUnBind(pResource,
                                      &StrmInfo,
                                      &DspInstInfo);
            DSP_FillErrline(Rval, &ErrLine, __LINE__);
        }
    }

    if (Rval != DSP_ERR_NONE) {
        AmbaLL_LogUInt5("HL_TestFrameCtrlOnVdspEnc Error[0x%x][%u]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

static inline void GetDecTestId(const UINT32 EnableMask, UINT32 *EnableId)
{
    if (EnableMask == 1U) {
        // 0: Org
        *EnableId = 0U;
    } else if (EnableMask == 2U) {
        // 1: CV2a
        *EnableId = 1U;
    } else if (EnableMask == 4U) {
        // 2: forman
        *EnableId = 2U;
    } else {
        *EnableId = 0U;
    }
}

static inline UINT32 HL_TFCtrlDecCfg(const CTX_RESOURCE_INFO_s *pResource,
                                     const UINT32 FrameIdx)
{
    UINT32 Rval = DSP_ERR_0001;

    if (pResource->TestDecStrmId != INVALID_TESTVDSP_STRM_ID) {
        CTX_VID_DEC_INFO_s VidDecInfo = {0};

        HL_GetVidDecInfo(HL_MTX_OPT_GET, pResource->TestDecStrmId, &VidDecInfo);
        VidDecInfo.StreamID = pResource->TestDecStrmId;
        VidDecInfo.BitsFormat = pResource->DecMaxStrmFmt[pResource->TestDecStrmId];
        VidDecInfo.BitsBufAddr = pResource->TestCfgOnStage[DSP_TEST_STAGE_VDSP_1][FrameIdx] + DEC_TEST_BS_FIFO_OFFSET;
        VidDecInfo.BitsBufSize = DEC_TEST_BS_FIFO_SIZE;
        VidDecInfo.MaxFrameWidth = pResource->DecMaxFrameWidth;
        VidDecInfo.MaxFrameHeight = pResource->DecMaxFrameHeight;
        VidDecInfo.XcodeMode = (UINT8)0U;
        VidDecInfo.XcodeWidth = 0U;
        VidDecInfo.XcodeHeight = 0U;
        VidDecInfo.MaxVideoBufferWidth = pResource->DecMaxVideoPlaneWidth;
        VidDecInfo.MaxVideoBufferHeight = pResource->DecMaxVideoPlaneHeight;
        VidDecInfo.FrameRate.Interlace = (UINT8)0U;
        VidDecInfo.FrameRate.TimeScale = 0U;
        VidDecInfo.FrameRate.NumUnitsInTick = 0U;
//FIXME, state reset when profile switch in VidDecStart
        VidDecInfo.State = VIDDEC_STATE_READY;
        VidDecInfo.CurrentBind = BIND_VIDEO_DECODE_TEST;
        HL_SetVidDecInfo(HL_MTX_OPT_SET, pResource->TestDecStrmId, &VidDecInfo);

        Rval = DSP_ERR_NONE;
    }
    return Rval;
}

static inline UINT32 HL_TFCtrlDecStart(const CTX_RESOURCE_INFO_s *pResource,
                                       const UINT32 FrameIdx)
{
    UINT32 Rval, ErrLine = 0U;
    dec_test_bin_inf_t DecHdr = {0};
    AMBA_DSP_VIDDEC_START_CONFIG_s DecStartCfg = {0};

    Rval = HL_GetTestDecHeader(&DecHdr, pResource, FrameIdx);
    DSP_FillErrline(Rval, &ErrLine, __LINE__);

    if (Rval == DSP_ERR_NONE) {
        DecStartCfg.PreloadDataSize = DecHdr.bstrm_size;
        DecStartCfg.SpeedIndex = 0U; //DONT CARE
        DecStartCfg.Direction = 0U; //DONT CARE
        DecStartCfg.FirstDisplayPTS = 0U; //DONT CARE
        Rval = dsp_video_dec_start(1U, &pResource->TestDecStrmId, &DecStartCfg);
    }
    return Rval;
}

static inline UINT32 HL_TestFrameCtrlOnVdspDec(const AMBA_DSP_TEST_FRAME_CTRL_s *pCtrl,
                                               const CTX_RESOURCE_INFO_s *pResource)
{
    UINT32 Rval = DSP_ERR_NONE, ErrLine = 0U;

    if (pCtrl->EnableMask > 0U) {
        if (pResource->TestFrmStatus[DSP_TEST_STAGE_VDSP_1] == DSP_TESTFRAME_STATE_ENABLE) {
            UINT32 EnableId = 0U;

            GetDecTestId(pCtrl->EnableMask , &EnableId);
            Rval = HL_TFCtrlDecCfg(pResource, EnableId);
            DSP_FillErrline(Rval, &ErrLine, __LINE__);
            if (Rval == DSP_ERR_NONE) {
                Rval = HL_TFCtrlDecStart(pResource, EnableId);
                DSP_FillErrline(Rval, &ErrLine, __LINE__);
            }
        } else {
            Rval = DSP_ERR_0004;
        }
    } else {
//        Rval = HL_TFCtrlDecStop(pResource, 0);
//        DSP_FillErrline(Rval, &ErrLine, __LINE__);
//        if (Rval == DSP_ERR_NONE) {
//            Rval = DSP_WaitVdspEvent(EVENT_FLAG_INT_VDSP1_EVENT, 2U, WAIT_INTERRUPT_TIMEOUT);
//            DSP_FillErrline(Rval, &ErrLine, __LINE__);
//            Rval = HL_TFCtrlEncUnBind(pResource,
//                                      &StrmInfo,
//                                      &DspInstInfo);
//            DSP_FillErrline(Rval, &ErrLine, __LINE__);
//        }
    }

    if (Rval != DSP_ERR_NONE) {
        AmbaLL_LogUInt5("HL_TestFrameCtrlOnVdspDec Error[0x%x][%u]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

static inline UINT32 HL_TestFrmCtrlImplOnIdle(const UINT32 NumStage,
                                              const AMBA_DSP_TEST_FRAME_CTRL_s *pCtrl)
{
    UINT32 Rval = DSP_ERR_NONE;
    UINT32 i, j;
    CTX_RESOURCE_INFO_s *pResource = HL_CtxResInfoPtr;

    HL_GetResourceLock(&pResource);
    for (i = 0U; i<NumStage ;i++) {
        UINT8 StageId = pCtrl[i].StageId;

        pResource->TestFrmEnableMask[StageId] = pCtrl[i].EnableMask;
        pResource->TestFrmRepeat[StageId] = pCtrl[i].IsRepeat;
        for (j = 0U; j < DSP_MAX_TEST_FRAME_NUM; j++) {
            pResource->TestFrmOrder[StageId][j] = pCtrl[i].Order[j];
        }
        if (pCtrl[i].EnableMask > 0U) {
#ifdef DEBUG_TESTFRAME_STATUS
            AmbaLL_LogUInt5("HL_TestFrmCtrlImplOnIdle %u TestFrmStatus [%u]: %u -> %u",
                    __LINE__, StageId, pResource->TestFrmStatus[StageId], DSP_TESTFRAME_STATE_CONFIG, 0U);
#endif
            pResource->TestFrmStatus[StageId] = DSP_TESTFRAME_STATE_CONFIG;
        } else {    //stop case
#ifdef DEBUG_TESTFRAME_STATUS
            AmbaLL_LogUInt5("HL_TestFrmCtrlImplOnIdle %u TestFrmStatus [%u]: %u -> %u",
                    __LINE__, StageId, pResource->TestFrmStatus[StageId], DSP_TESTFRAME_STATE_STOP, 0U);
#endif
            pResource->TestFrmStatus[StageId] = DSP_TESTFRAME_STATE_STOP;
        }
        if ((StageId == DSP_TEST_STAGE_VDSP_0) || (StageId == DSP_TEST_STAGE_VDSP_1)) {
            pResource->TestFrmRepeat[StageId] = 1U;
        }
    }
    HL_GetResourceUnLock();

    return Rval;
}

static inline UINT32 HL_TestFrmCtrlImplOnRunning(const UINT32 NumStage,
                                                 const AMBA_DSP_TEST_FRAME_CTRL_s *pCtrl)
{
    UINT32 Rval = DSP_ERR_NONE;
    UINT32 ErrLine = 0U;
    UINT32 i;
    CTX_RESOURCE_INFO_s *pResource = HL_CtxResInfoPtr;

    for (i = 0U; i<NumStage ;i++) {
        UINT8 StageId = pCtrl[i].StageId;

        HL_GetResourceLock(&pResource);
        if (pCtrl[i].EnableMask > 0U) {
            pResource->TestFrmRepeat[StageId] = pCtrl[i].IsRepeat;

            if ((pResource->TestFrmStatus[StageId] == DSP_TESTFRAME_STATE_CONFIG) ||
                (pResource->TestFrmStatus[StageId] == DSP_TESTFRAME_STATE_STOP)) {
#ifdef DEBUG_TESTFRAME_STATUS
                AmbaLL_LogUInt5("HL_TestFrmCtrlImplOnRunning %u Mask:0x%x TestFrmStatus [%u]: %u -> %u",
                        __LINE__, pCtrl[i].EnableMask, StageId, pResource->TestFrmStatus[StageId], DSP_TESTFRAME_STATE_ENABLE);
#endif
                pResource->TestFrmStatus[StageId] = DSP_TESTFRAME_STATE_ENABLE;
            } else {
                //enable -> enable case
#ifdef DEBUG_TESTFRAME_STATUS
                AmbaLL_LogUInt5("HL_TestFrmCtrlImplOnRunning %u Mask:0x%x TestFrmStatus [%u]: %u",
                        __LINE__, pCtrl[i].EnableMask, StageId, pResource->TestFrmStatus[StageId], 0);
#endif
            }
        } else {    //stop case
#ifdef DEBUG_TESTFRAME_STATUS
            AmbaLL_LogUInt5("HL_TestFrmCtrlImplOnRunning %u TestFrmStatus [%u]: %u -> %u",
                    __LINE__, StageId, pResource->TestFrmStatus[StageId], DSP_TESTFRAME_STATE_STOP, 0U);
#endif
            pResource->TestFrmStatus[StageId] = DSP_TESTFRAME_STATE_STOP;
        }
        HL_GetResourceUnLock();

        if ((StageId == DSP_TEST_STAGE_IDSP_0) ||
            (StageId== DSP_TEST_STAGE_IDSP_1)) {
            Rval = HL_TestFrameCtrlOnIdsp(&pCtrl[i], pResource);
            DSP_FillErrline(Rval, &ErrLine, __LINE__);
        } else if (StageId == DSP_TEST_STAGE_VDSP_0) {
            pResource->TestFrmRepeat[StageId] = 1U;
            Rval = HL_TestFrameCtrlOnVdspEnc(&pCtrl[i], pResource);
            DSP_FillErrline(Rval, &ErrLine, __LINE__);
        } else {
            pResource->TestFrmRepeat[StageId] = 1U;
            Rval = HL_TestFrameCtrlOnVdspDec(&pCtrl[i], pResource);
            DSP_FillErrline(Rval, &ErrLine, __LINE__);
        }
    }

    return Rval;
}

UINT32 dsp_test_frame_ctrl(UINT32 NumStage, const AMBA_DSP_TEST_FRAME_CTRL_s *pCtrl)
{
    UINT32 Rval;
    UINT32 ErrLine = 0U;

    Rval = HL_TestFrameCtrlChk(NumStage, pCtrl);
    DSP_FillErrline(Rval, &ErrLine, __LINE__);

    /* Body */
    if (Rval == DSP_ERR_NONE) {
        UINT8 DspOpModeInit, DspOpModeIdle, DspOpModeCamera;
        CTX_VPROC_INFO_s VprocInfo;

        DspOpModeInit = (DSP_GetProfState() == DSP_PROF_STATUS_INVALID)? 1U: 0U;
        DspOpModeIdle = (DSP_GetProfState() == DSP_PROF_STATUS_IDLE)? 1U: 0U;
        DspOpModeCamera = (DSP_GetProfState() == DSP_PROF_STATUS_CAMERA)? 1U: 0U;
        HL_GetVprocInfo(HL_MTX_OPT_ALL, 0, &VprocInfo);
#ifdef DEBUG_TESTFRAME_STATUS
        AmbaLL_LogUInt5("dsp_test_frame_ctrl ProfState:%u VprocSt:%u", DSP_GetProfState(), VprocInfo.Status, 0U, 0U, 0U);
#endif
        if ((DspOpModeInit == 1U) ||
            (DspOpModeIdle == 1U) ||
            (VprocInfo.Status == DSP_VPROC_STATUS_TIMER) ||
            (VprocInfo.Status == DSP_VPROC_STATUS_INVALID)) {
            // Execute @ LV start flow
            Rval = HL_TestFrmCtrlImplOnIdle(NumStage, pCtrl);
        } else if (DspOpModeCamera == 1U) {
            Rval = HL_TestFrmCtrlImplOnRunning(NumStage, pCtrl);
        } else {
            // DO NOTHING
        }
    }

    if (Rval != DSP_ERR_NONE) {
        AmbaLL_LogUInt5("dsp_test_frame_ctrl Error[0x%x][%u]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

static inline UINT32 HL_SafetyApiChk(UINT32 ID, UINT32 SubID, const UINT32 *pStatus)
{
    UINT32 Rval = DSP_ERR_NONE, ErrLine = 0U;
    const Dsp_TimingApi_t* TimingApi = AmbaDSP_GetTimingApiFunc();
    const Dsp_DumpApi_t* DumpApi = AmbaDSP_GetDumpApiFunc();
    const Dsp_CheckApi_t* CheckApi = AmbaDSP_GetCheckApiFunc();

    /* Timing sanity check */
    if (TimingApi != NULL) {
        Rval = TimingApi->pMainSafetyChk(ID, SubID, pStatus);
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
    }
    /* Input sanity check */
    if ((Rval == DSP_ERR_NONE) &&
        (CheckApi != NULL)) {
        Rval = CheckApi->pMainSafetyChk(ID, SubID, pStatus);
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
    }

    /* Input dump */
    if ((Rval == DSP_ERR_NONE) &&
        (DumpApi != NULL)) {
        DumpApi->pMainSafetyChk(ID, SubID, pStatus);
    }
    /* Logic sanity check */

    if (Rval != DSP_ERR_NONE) {
        AmbaLL_LogUInt5("HL_SafetyApiChk Error[0x%x][%u]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

static inline UINT32 HL_GetResetStatusAddr(UINT32 ID,
                                           UINT32 SubID,
                                           UINT32 *pAddr,
                                           UINT32 *pDefValue,
                                           UINT32 *pErrLine)
{
    UINT32 Rval = DSP_ERR_NONE;

    if (ID < DSP_RESET_STATUS_VOUT_MIXER) {
        Rval = dsp_osal_dbgport_get_reset_status_info(ID, pAddr, pDefValue);
    } else if (SubID < 1U) {
        Rval = dsp_osal_dbgport_get_reset_status_info(ID, pAddr, pDefValue);
    } else if (ID < DSP_RESET_STATUS_NUM) {
        Rval = dsp_osal_dbgport_get_reset_status_info(ID + DSP_RESET_STATUS_VOUT_MIXER, pAddr, pDefValue);
    } else {
        Rval = DSP_ERR_0000;
        DSP_FillErrline(Rval, pErrLine, __LINE__);
    }

    return Rval;
}

void HL_VoutSetResetReg(UINT32 VoutId)
{
    UINT32 Rval, ErrLine = 0U;
    UINT32 VoutRegIdx;
    UINT32 StatusAddr = 0U;
    UINT32 StatusDefValue = 0U;

    for (VoutRegIdx = DSP_RESET_STATUS_VOUT_MIXER; VoutRegIdx<=DSP_RESET_STATUS_VOUT_BYTE; VoutRegIdx++) {
        Rval = HL_GetResetStatusAddr(VoutRegIdx, VoutId, &StatusAddr, &StatusDefValue, &ErrLine);
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
        if (StatusAddr != 0U) {
            void *pVoid;
            UINT32 StatusValue = 0U;

            (void)dsp_osal_memcpy(&pVoid, &StatusAddr, sizeof(UINT32));
            StatusValue = dsp_osal_readl_relaxed(pVoid);

            //check is default value or not
            if (StatusValue != StatusDefValue) {
                Rval = DSP_ERR_0007;
                DSP_FillErrline(Rval, &ErrLine, __LINE__);
//                AmbaLL_LogUInt5("HL_VoutSetResetReg Error[0x%x][%u] VoutId:%u VoutRegIdx:%u StatusValue:0x%x",
//                        Rval, ErrLine, VoutId, VoutRegIdx, *pStatusValue);
            }

            //write new value and check
            if (Rval == DSP_ERR_NONE) {
                dsp_osal_writel_relaxed((StatusDefValue + 1U), pVoid);
                StatusValue = dsp_osal_readl_relaxed(pVoid);
                if (StatusValue != (StatusDefValue + 1U)) {
                    Rval = DSP_ERR_0007;
                    DSP_FillErrline(Rval, &ErrLine, __LINE__);
//                    AmbaLL_LogUInt5("HL_VoutSetResetReg Error[0x%x][%u] VoutId:%u VoutRegIdx:%u StatusValue:0x%x",
//                            Rval, ErrLine, VoutId, VoutRegIdx, *pStatusValue);
                }
            }
        }
    }
}

UINT32 dsp_main_safety_chk(const UINT32 ID, const UINT32 SubID, UINT32 *pStatus)
{
    UINT32 Rval;
    UINT32 ErrLine = 0U;
    UINT32 ErrStValue = 0U;

    Rval = HL_SafetyApiChk(ID, SubID, pStatus);
    DSP_FillErrline(Rval, &ErrLine, __LINE__);

    /* Body */
    if (Rval == DSP_ERR_NONE) {
        UINT32 StatusAddr = 0U;
        UINT32 StatusDefValue = 0U;

        Rval = HL_GetResetStatusAddr(ID, SubID, &StatusAddr, &StatusDefValue, &ErrLine);
        if (StatusAddr != 0U) {
            volatile UINT32 *pStatusValue = NULL;

            (void)dsp_osal_memcpy(&pStatusValue, &StatusAddr, sizeof(UINT32));
            //check is default value or not
            if (*pStatusValue != StatusDefValue) {
                Rval = DSP_ERR_0007;
                DSP_FillErrline(Rval, &ErrLine, __LINE__);
                ErrStValue = *pStatusValue;
            }

            //write new value and check
            if (Rval == DSP_ERR_NONE) {
                *pStatusValue = StatusDefValue + 1U;
                if (*pStatusValue != (StatusDefValue + 1U)) {
                    Rval = DSP_ERR_0007;
                    DSP_FillErrline(Rval, &ErrLine, __LINE__);
                    ErrStValue = *pStatusValue;
                }
            }

            //write default value back and check again
            if (Rval == DSP_ERR_NONE) {
                *pStatusValue = StatusDefValue;
                if (*pStatusValue != StatusDefValue) {
                    Rval = DSP_ERR_0007;
                    DSP_FillErrline(Rval, &ErrLine, __LINE__);
                    ErrStValue = *pStatusValue;
                }
            }
        }
    }

    *pStatus = Rval;

    if (Rval != DSP_ERR_NONE) {
        AmbaLL_LogUInt5("dsp_main_safety_chk Error[0x%x][%u][0x%x]", Rval, ErrLine, ErrStValue, 0U, 0U);
    }

    return Rval;
}

static inline UINT32 HL_SafetyCfgApiChk(const UINT32 ID, const UINT32 Val0, UINT32 Val1)
{
    UINT32 Rval = DSP_ERR_NONE, ErrLine = 0U;
    const Dsp_TimingApi_t* TimingApi = AmbaDSP_GetTimingApiFunc();
    const Dsp_DumpApi_t* DumpApi = AmbaDSP_GetDumpApiFunc();
    const Dsp_CheckApi_t* CheckApi = AmbaDSP_GetCheckApiFunc();

    /* Timing sanity check */
    if (TimingApi != NULL) {
        Rval = TimingApi->pMainSafetyCfg(ID, Val0, Val1);
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
    }
    /* Input sanity check */
    if ((Rval == DSP_ERR_NONE) &&
        (CheckApi != NULL)) {
        Rval = CheckApi->pMainSafetyCfg(ID, Val0, Val1);
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
    }

    /* Input dump */
    if ((Rval == DSP_ERR_NONE) &&
        (DumpApi != NULL)) {
        DumpApi->pMainSafetyCfg(ID, Val0, Val1);
    }
    /* Logic sanity check */

    if (Rval != DSP_ERR_NONE) {
        AmbaLL_LogUInt5("HL_SafetyCfgApiChk Error[0x%x][%u]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

UINT32 dsp_main_safety_cfg(const UINT32 ID, const UINT32 Val0, const UINT32 Val1)
{
    UINT32 Rval;
    UINT32 ErrLine = 0U;

    Rval = HL_SafetyCfgApiChk(ID, Val0, Val1);
    DSP_FillErrline(Rval, &ErrLine, __LINE__);

    /* Body */
    if (Rval == DSP_ERR_NONE) {
        if (ID == DSP_SAFETY_ENET) {
            DSP_SetupInitDataEnent(Val0);
        } else {
            //Do nothing
        }
    }

    if (Rval != DSP_ERR_NONE) {
        AmbaLL_LogUInt5("dsp_main_safety_config Error[0x%x][%u][0x%x]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}
