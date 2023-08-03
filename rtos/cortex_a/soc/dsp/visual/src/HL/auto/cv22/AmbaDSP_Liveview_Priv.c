/**
 *  @file AmbaDSP_Liveview_Priv.c
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
 *  @details Implementation of SSP Liveview private control API
 *
 */

#include "AmbaDSP_Log.h"
#include "AmbaDSP_Cmds.h"
#include "AmbaDSP_IntChk.h"
#include "AmbaDSP_BaseCfg.h"
#include "AmbaDSP_ApiDump.h"
#include "AmbaDSP_ArmComm.h"
#include "AmbaDSP_ApiCheck.h"
#include "AmbaDSP_EncodeAPI.h"
#include "AmbaDSP_ApiResource.h"
#include "AmbaDSP_EncodeUtility.h"
#include "dsp_priv_api.h"

static inline UINT32 HL_LiveviewUpdatePymdCfgChk(const UINT16 NumViewZone,
                                                 const UINT16 *pViewZoneId,
                                                 const AMBA_DSP_LIVEVIEW_PYRAMID_s *pPyramid,
                                                 const AMBA_DSP_LIVEVIEW_YUV_BUF_s *pPyramidBuf,
                                                 const UINT64 *pAttachedRawSeq)
{
    UINT32 Rval;
    const Dsp_DumpApi_t* DumpApi = AmbaDSP_GetDumpApiFunc();
    const Dsp_CheckApi_t* CheckApi = AmbaDSP_GetCheckApiFunc();
    const Dsp_ResourceApi_t* ResourceApi = AmbaDSP_GetResourceApiFunc();

    //FIXME, Misra
    (void)pAttachedRawSeq;

    /* DSP CtxInit */
    Rval = HL_CtxInit();

    /* Debug print */
    if ((Rval == OK) &&
        (DumpApi != NULL)) {
        DumpApi->pLiveviewUpdatePymdCfg(NumViewZone, pViewZoneId, pPyramid, pPyramidBuf, pAttachedRawSeq);
    }
    /* Input sanity check */
    if ((Rval == OK) &&
        (CheckApi != NULL)) {
        Rval = CheckApi->pLiveviewUpdatePymdCfg(NumViewZone, pViewZoneId, pPyramid, pPyramidBuf, pAttachedRawSeq);
    }
    /* HW/SW resources check */
    if ((Rval == OK) &&
        (ResourceApi != NULL)) {
        Rval = ResourceApi->pLiveviewUpdatePymdCfg(NumViewZone, pViewZoneId, pPyramid, pPyramidBuf, pAttachedRawSeq);
    }

    return Rval;
}

/**
* Liveview Pyramid config update
* @param [in]  NumViewZone number of viewzone
* @param [in]  pViewZoneId viewzone index
* @param [in]  pPyramid pyramid config information
* @param [in]  pPyramidBuf pyramid buffer config information
* @param [out] pAttachedRawSeq attached raw sequence
* @return ErrorCode
*/
UINT32 dsp_liveview_update_pymdcfg(UINT16 NumViewZone,
                                   const UINT16 *pViewZoneId,
                                   const AMBA_DSP_LIVEVIEW_PYRAMID_s *pPyramid,
                                   const AMBA_DSP_LIVEVIEW_YUV_BUF_s *pPyramidBuf,
                                   UINT64 *pAttachedRawSeq)
{
    UINT32 Rval, i, j;
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;
    UINT8 WriteMode;

    AmbaMisra_TouchUnused(pAttachedRawSeq);

    Rval = HL_LiveviewUpdatePymdCfgChk(NumViewZone, pViewZoneId, pPyramid, pPyramidBuf, pAttachedRawSeq);

    /* Body */

    if (Rval == OK) {
        /* check status?? */
        if (DSP_PROF_STATUS_CAMERA != DSP_GetProfState()) {
            Rval = DSP_ERR_0004;
        } else {

            ULONG ULAddr;
            INT32 Cmp;
            UINT16 VinId = 0U;
            UINT32 PhysAddr;
            UINT32 *pBufTblAddr = NULL;
            UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
#ifdef SUPPORT_VIN_ATTACH_PYMD_UPDATE
            UINT32 *pBatchQAddr, BatchCmdId = 0U;
            cmd_vproc_img_pyramid_setup_t *VprocPrmdSetup = HL_DefCtxCmdBufPtrVpcPymd;
            void *pVoid;
#else
            cmd_vproc_img_pyramid_setup_t *VprocPrmdSetup = HL_DefCtxCmdBufPtrVpcPymd;
#endif

            for (i=0U; i<NumViewZone; i++) {
                HL_GetViewZoneInfoPtr(pViewZoneId[i], &ViewZoneInfo);
                DSP_Bit2U16Idx(ViewZoneInfo->SourceVin, &VinId);
                WriteMode = HL_GetVin2CmdNormalWrite(VinId);

                if ((ViewZoneInfo->PymdBufferStatus == PYMD_BUF_STATUS_CONFIG) &&
                    (ViewZoneInfo->PymdAllocType == ALLOC_EXTERNAL_DISTINCT)) {

                    cmd_vproc_set_ext_mem_t *VprocExtMem = HL_DefCtxCmdBufPtrVpcExtMem;

                    HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
                    dsp_osal_typecast(&VprocExtMem, &CmdBufferAddr);
                    VprocExtMem->channel_id = (UINT8)pViewZoneId[i];
                    VprocExtMem->memory_type = VPROC_EXT_MEM_TYPE_HIER_0;
                    VprocExtMem->allocation_type = VIN_EXT_MEM_ALLOC_TYPE_DISTINCT;
                    VprocExtMem->allocation_mode = VIN_EXT_MEM_MODE_NEW;
                    VprocExtMem->overflow_cntl = VIN_EXT_MEM_CTRL_CIRCULAR;
                    VprocExtMem->num_frm_buf = pPyramidBuf[i].BufNum;
                    if (VprocExtMem->num_frm_buf > EXT_MEM_VPROC_MAX_NUM) {
                        AmbaLL_LogUInt5("[%d]Only %d ExtMem supported", __LINE__, EXT_MEM_VPROC_MAX_NUM, 0U, 0U, 0U);
                    }
                    VprocExtMem->buf_pitch = ALIGN_NUM16(pPyramidBuf[i].YuvBuf.Pitch, DSP_BUF_ALIGNMENT);
                    VprocExtMem->buf_width = pPyramidBuf[i].YuvBuf.Window.Width;
                    VprocExtMem->buf_height = pPyramidBuf[i].YuvBuf.Window.Height;

                    HL_GetPointerToDspExtPymdBufArray(pViewZoneId[i], &pBufTblAddr);
                    for (j = 0; j<pPyramidBuf[i].BufNum; j++) {
                        (void)dsp_osal_virt2cli(pPyramidBuf[i].pYuvBufTbl[j], &PhysAddr);
                        pBufTblAddr[j] = PhysAddr;
                    }
                    dsp_osal_typecast(&ULAddr, &pBufTblAddr);
                    (void)dsp_osal_virt2cli(ULAddr, &VprocExtMem->buf_addr);
                    VprocExtMem->img_width = pPyramidBuf[i].YuvBuf.Window.Width;
                    VprocExtMem->img_height = pPyramidBuf[i].YuvBuf.Window.Height;
                    VprocExtMem->chroma_format = (pPyramidBuf[i].YuvBuf.DataFmt == AMBA_DSP_YUV420)? DSP_YUV_420: DSP_YUV_422;
                    VprocExtMem->luma_img_ofs_x = 0U;
                    VprocExtMem->luma_img_ofs_y = 0U;
                    VprocExtMem->chroma_img_ofs_x = 0U;
                    VprocExtMem->chroma_img_ofs_y = 0U;

                    Rval = AmbaHL_CmdVprocSetExtMem(WriteMode, VprocExtMem);
                    HL_RelCmdBuffer(CmdBufferId);
                    if (Rval == OK) {
                        HL_GetViewZoneInfoLock((UINT16)i, &ViewZoneInfo);
                        ViewZoneInfo->PymdBufNum = pPyramidBuf[i].BufNum;
                        (void)dsp_osal_memcpy(&ViewZoneInfo->PymdYuvBuf, &pPyramidBuf[i].YuvBuf, sizeof(AMBA_DSP_YUV_IMG_BUF_s));
                        for (j = 0U; j < ViewZoneInfo->PymdBufNum; j++) {
                            ViewZoneInfo->PymdYuvBufTbl[j] = pPyramidBuf[i].pYuvBufTbl[j];
                        }

                        ViewZoneInfo->PymdBufferStatus = PYMD_BUF_STATUS_ALLOCATED;
                        HL_GetViewZoneInfoUnLock(pViewZoneId[i]);
                    }
                }

                if (Rval == OK) {
                    /* Pymd Setup */
                    (void)dsp_osal_memcmp(&ViewZoneInfo->Pyramid, &pPyramid[i], sizeof(AMBA_DSP_LIVEVIEW_PYRAMID_s), &Cmp);
                    if (Cmp != 0) {
                        HL_GetViewZoneInfoLock(pViewZoneId[i], &ViewZoneInfo);
                        (void)dsp_osal_memcpy(&ViewZoneInfo->Pyramid, &pPyramid[i], sizeof(AMBA_DSP_LIVEVIEW_PYRAMID_s));
                        HL_GetViewZoneInfoUnLock(pViewZoneId[i]);

#ifdef SUPPORT_VIN_ATTACH_PYMD_UPDATE
                        {
                            UINT32 NewWp = 0U;

                            HL_GetViewZoneInfoLock(pViewZoneId[i], &ViewZoneInfo);
                            /* Reguest BatchCmdQ buffer */
                            Rval = DSP_ReqBuf(&ViewZoneInfo->BatchQPoolDesc, 1U, &NewWp, 1U/*FullnessCheck*/);
                            if (Rval != OK) {
                                HL_GetViewZoneInfoUnLock(pViewZoneId[i]);
                                AmbaLL_LogWarnUInt5((UINT16)AMBALLLOG_WARN_BATCHQ_FULL, "[LiveviewUpdatePymdCfg] ViewZone(%d) batch pool is full"
                                    , pViewZoneId[i], 0U, 0U, 0U, 0U);
                            } else {
                                ViewZoneInfo->BatchQPoolDesc.Wp = NewWp;
                                HL_GetViewZoneInfoUnLock(pViewZoneId[i]);
                                HL_GetPointerToDspBatchQ(pViewZoneId[i], (UINT16)ViewZoneInfo->BatchQPoolDesc.Wp, &pBatchQAddr, &BatchCmdId);
                                dsp_osal_typecast(&VprocPrmdSetup, &pBatchQAddr);
                            }
                        }
                        Rval = HL_FillVideoProcPyramidSetup(VprocPrmdSetup, pViewZoneId[i]);
#else
                        HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
                        dsp_osal_typecast(&VprocPrmdSetup, &CmdBufferAddr);
                        Rval = HL_FillVideoProcPyramidSetup(VprocPrmdSetup, pViewZoneId[i]);
#endif
                        if (Rval != OK) {
                            AmbaLL_LogUInt5("PyramidSetup[%d] filling fail", i, 0U, 0U, 0U, 0U);
                        } else {
#ifdef SUPPORT_VIN_ATTACH_PYMD_UPDATE
                            UINT8 VinatchCmdBufferId = 0U; void *VinatchCmdBufferAddr = NULL;
                            cmd_vin_attach_proc_cfg_to_cap_frm_t *VinAttachProcCfg = NULL;
                            vin_fov_batch_cmd_set_t *FovBatchSet;
                            UINT16 VinId = 0U;

                            //Fill BatchCmdEnd
                            pBatchQAddr[0U] = CMD_VPROC_IMG_PRMD_SETUP;
                            pBatchQAddr[CMD_SIZE_IN_WORD] = MAX_BATCH_CMD_END;

                            HL_GetPointerToDspBatchCmdSet(pViewZoneId[i], 0U/*Idx*/, &FovBatchSet);

                            dsp_osal_typecast(&ULAddr, &pBatchQAddr);
                            dsp_osal_typecast(&pVoid, &pBatchQAddr);
                            (void)dsp_osal_virt2cli(ULAddr, &FovBatchSet->batch_cmd_set_info.addr);
                            HL_SetDspBatchQInfo(BatchCmdId, 1/*AutoReset*/, BATCHQ_INFO_IMG_PRMD, 0X80000001U);
                            DSP_Bit2U16Idx(ViewZoneInfo->SourceVin, &VinId);
                            FovBatchSet->vin_id = VinId;
                            FovBatchSet->chan_id = (UINT16)HL_GetViewZoneLocalIndexOnVin(pViewZoneId[i]);
                            FovBatchSet->batch_cmd_set_info.id = BatchCmdId;
                            FovBatchSet->batch_cmd_set_info.size = HL_GetBatchCmdNumber(ULAddr)*CMD_SIZE_IN_BYTE;

                            //CacheClean
                            (void)dsp_osal_cache_clean(pVoid, FovBatchSet->batch_cmd_set_info.size);

                            HL_AcqCmdBuffer(&VinatchCmdBufferId, &VinatchCmdBufferAddr);
                            dsp_osal_typecast(&VinAttachProcCfg, &VinatchCmdBufferAddr);
                            VinAttachProcCfg->vin_id = (UINT8)VinId;
                            VinAttachProcCfg->fov_num = (UINT8)1U;

                            dsp_osal_typecast(&ULAddr, &FovBatchSet);
                            dsp_osal_typecast(&pVoid, &FovBatchSet);
                            dsp_osal_virt2cli(ULAddr, &VinAttachProcCfg->fov_batch_cmd_set_addr);
                            (void)dsp_osal_cache_clean(pVoid, sizeof(vin_fov_batch_cmd_set_t));

                            Rval = AmbaHL_CmdVinAttachCfgToCapFrm(WriteMode, VinAttachProcCfg);
#else
                            Rval = AmbaHL_CmdVprocImgPrmdSetup(WriteMode, VprocPrmdSetup);
#endif
                        }
                        HL_RelCmdBuffer(CmdBufferId);
                    }
                }
                if (Rval != OK) {
                    break;
                }
            }
        }
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("AmbaDSP_LiveviewUpdatePymdCfg Error[0x%X]", Rval, 0U, 0U, 0U, 0U);
    }

    return Rval;
}

UINT32 dsp_liveview_parse_vp_msg(ULONG VpMsgAddr)
{
    UINT32 Rval = OK;

    if (VpMsgAddr == 0U) {
        Rval = DSP_ERR_0001;
    }

    if (Rval == OK) {
        const AMBA_DSP_MSG_BUFF_CTRL_s *pMsgBufCtrl = NULL;
        const msg_vp_hier_out_lane_out_t *pHierInfo = NULL;
        UINT32 i, U32Val;
        CTX_BATCHQ_INFO_s BatchQInfo = {0};
        const void *pVoid = NULL;
        ULONG ULAddr;

        dsp_osal_typecast(&pMsgBufCtrl, &VpMsgAddr);
        dsp_osal_typecast(&pVoid, &VpMsgAddr);
        (void)dsp_osal_cache_invalidate(pVoid, sizeof(AMBA_DSP_MSG_BUFF_CTRL_s));

        /* parse latest message */
        (void)dsp_osal_cli2virt(pMsgBufCtrl->MsgBufAddr + (ALIGN_NUM(sizeof(msg_vp_hier_out_lane_out_t), 128U)*pMsgBufCtrl->WP), &ULAddr);
        dsp_osal_typecast(&pHierInfo, &ULAddr);
        dsp_osal_typecast(&pVoid, &ULAddr);
        (void)dsp_osal_cache_invalidate(pVoid, sizeof(msg_vp_hier_out_lane_out_t));

        AmbaLL_LogUInt5("  ============ Hier ============", 0, 0, 0, 0, 0);
        AmbaLL_LogUInt5("  Header                        ", 0, 0, 0, 0, 0);
        AmbaLL_LogUInt5("    channel_id           = %u  ", pHierInfo->header.channel_id, 0, 0, 0, 0);
        AmbaLL_LogUInt5("    cap_seq_no           = %u  ", pHierInfo->header.cap_seq_no, 0, 0, 0, 0);
        AmbaLL_LogUInt5("    batch_cmd_id         = 0x%X", pHierInfo->header.batch_cmd_id, 0, 0, 0, 0);
        HL_GetDspBatchQInfo(pHierInfo->header.batch_cmd_id, &BatchQInfo);
        AmbaLL_LogUInt5("      IsoCfgId           = 0x%X", BatchQInfo.IsoCfgId, 0, 0, 0, 0);
        AmbaLL_LogUInt5("      ImgPrmdId          = 0x%X", BatchQInfo.ImgPrmdId, 0, 0, 0, 0);
        for (i=0; i<(UINT32)MAX_SIDEBAND_BUF_ARRAY_NUM ;i++) {
            AmbaLL_LogUInt5("    sideband[%d]          = %d  ", i, 0, 0, 0, 0);
            AmbaLL_LogUInt5("      fov_id             = %d  ", pHierInfo->header.side_band_info[i].fov_id, 0, 0, 0, 0);
            AmbaLL_LogUInt5("      info_addr          = 0x%X", pHierInfo->header.side_band_info[i].fov_private_info_addr, 0, 0, 0, 0);
        }
        for (i=0; i<(UINT32)MAX_ROI_CMD_TAG_NUM ;i++) {
            AmbaLL_LogUInt5("    RoiTag[%d]            = 0x%X", i, pHierInfo->header.roi_cmd_tag[i], 0, 0, 0);
        }
        AmbaLL_LogUInt5("    ln_det_hier_mask     = 0x%X", pHierInfo->header.ln_det_hier_mask, 0, 0, 0, 0);
        AmbaLL_LogUInt5("    is_hier_out_tile_mode= %d  ", pHierInfo->header.is_hier_out_tile_mode, 0, 0, 0, 0);
        AmbaLL_LogUInt5("    vin_cap_time         = %d  ", pHierInfo->header.vin_cap_time, 0, 0, 0, 0);
        AmbaLL_LogUInt5("    proc_roi_cmd_time    = %d  ", pHierInfo->header.proc_roi_cmd_time, 0, 0, 0, 0);
        AmbaLL_LogUInt5("    c2y_done_time        = %d  ", pHierInfo->header.c2y_done_time, 0, 0, 0, 0);
        AmbaLL_LogUInt5("    warp_mctf_done_time  = %d  ", pHierInfo->header.warp_mctf_done_time, 0, 0, 0, 0);

        for (i=0; i<DSP_HIER_NUM ;i++) {
            AmbaLL_LogUInt5("  Hier[%d]                        ", i, 0, 0, 0, 0);
            AmbaLL_LogUInt5("    luma_buffer_addr     = 0x%X", pHierInfo->luma_hier_outs[i].buffer_addr, 0, 0, 0, 0);
            AmbaLL_LogUInt5("    luma_buffer_pitch    = %d  ", pHierInfo->luma_hier_outs[i].buffer_pitch, 0, 0, 0, 0);
            AmbaLL_LogUInt5("    luma_img_width       = %d  ", pHierInfo->luma_hier_outs[i].img_width, 0, 0, 0, 0);
            AmbaLL_LogUInt5("    luma_img_height      = %d  ", pHierInfo->luma_hier_outs[i].img_height, 0, 0, 0, 0);
            AmbaLL_LogUInt5("    chroma_buffer_addr   = 0x%X", pHierInfo->chroma_hier_outs[i].buffer_addr, 0, 0, 0, 0);
            AmbaLL_LogUInt5("    chroma_buffer_pitch  = %d  ", pHierInfo->chroma_hier_outs[i].buffer_pitch, 0, 0, 0, 0);
            AmbaLL_LogUInt5("    chroma_img_width     = %d  ", pHierInfo->chroma_hier_outs[i].img_width, 0, 0, 0, 0);
            AmbaLL_LogUInt5("    chroma_img_height    = %d  ", pHierInfo->chroma_hier_outs[i].img_height, 0, 0, 0, 0);
            dsp_osal_typecast(&U32Val, &pHierInfo->hier_out_offsets[i].x_offset);
            AmbaLL_LogUInt5("    x_offset             = %d  ", U32Val, 0, 0, 0, 0);
            dsp_osal_typecast(&U32Val, &pHierInfo->hier_out_offsets[i].y_offset);
            AmbaLL_LogUInt5("    y_offset             = %d  ", U32Val, 0, 0, 0, 0);
        }

        AmbaLL_LogUInt5("  Lndt                        ", 0, 0, 0, 0, 0);
        AmbaLL_LogUInt5("    luma_buffer_addr     = 0x%X", pHierInfo->luma_lane_det_out.buffer_addr, 0, 0, 0, 0);
        AmbaLL_LogUInt5("    luma_buffer_pitch    = %d  ", pHierInfo->luma_lane_det_out.buffer_pitch, 0, 0, 0, 0);
        AmbaLL_LogUInt5("    luma_img_width       = %d  ", pHierInfo->luma_lane_det_out.img_width, 0, 0, 0, 0);
        AmbaLL_LogUInt5("    luma_img_height      = %d  ", pHierInfo->luma_lane_det_out.img_height, 0, 0, 0, 0);
        AmbaLL_LogUInt5("    chroma_buffer_addr   = 0x%X", pHierInfo->chroma_lane_det_out.buffer_addr, 0, 0, 0, 0);
        AmbaLL_LogUInt5("    chroma_buffer_pitch  = %d  ", pHierInfo->chroma_lane_det_out.buffer_pitch, 0, 0, 0, 0);
        AmbaLL_LogUInt5("    chroma_img_width     = %d  ", pHierInfo->chroma_lane_det_out.img_width, 0, 0, 0, 0);
        AmbaLL_LogUInt5("    chroma_img_height    = %d  ", pHierInfo->chroma_lane_det_out.img_height, 0, 0, 0, 0);
        dsp_osal_typecast(&U32Val, &pHierInfo->lane_det_out_offset.x_offset);
        AmbaLL_LogUInt5("    x_offset             = %d  ", U32Val, 0, 0, 0, 0);
        dsp_osal_typecast(&U32Val, &pHierInfo->lane_det_out_offset.x_offset);
        AmbaLL_LogUInt5("    y_offset             = %d  ", U32Val, 0, 0, 0, 0);
    }

    return Rval;
}

static inline UINT32 HL_LiveviewSideBandUpdateChk(UINT16 ViewZoneId, UINT8 NumBand, const ULONG *pSidebandBufAddr)
{
    UINT32 Rval;
    const Dsp_DumpApi_t* DumpApi = AmbaDSP_GetDumpApiFunc();
    const Dsp_CheckApi_t* CheckApi = AmbaDSP_GetCheckApiFunc();
    const Dsp_ResourceApi_t* ResourceApi = AmbaDSP_GetResourceApiFunc();
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;

    /* DSP CtxInit */
    Rval = HL_CtxInit();

    /* Timing sanity check */
    /* Input sanity check */
    if ((Rval == OK) &&
        (DumpApi != NULL)) {
        DumpApi->pLiveviewSidebandUpdate(ViewZoneId, NumBand, pSidebandBufAddr);
    }
    if ((Rval == OK) &&
        (CheckApi != NULL)) {
        Rval = CheckApi->pLiveviewSidebandUpdate(ViewZoneId, NumBand, pSidebandBufAddr);
    }
    /* HW/SW resources check */
    if ((Rval == OK) &&
        (ResourceApi != NULL)) {
        Rval = ResourceApi->pLiveviewSideBandUpdate(ViewZoneId, NumBand, pSidebandBufAddr);
    }

    /* Logic sanity check */
    if (Rval == OK) {
        HL_GetResourcePtr(&Resource);
        if (Resource->VpMsgCtrlBufAddr == 0U) {
            Rval = DSP_ERR_0004;
            AmbaLL_LogUInt5("Not support SideBand update when VpMsgCtrlBuf = NULL", 0U, 0U, 0U, 0U, 0U);
        }
    }
    return Rval;
}

/**
* Liveview ViewZone sideband update function, USED WHEN IDSPRX(VpMsg) ENABLE
* @param [in]  ViewZoneId viewzone index
* @param [in]  NumBand sideband number
* @param [out] pSidebandBufAddr SideBand information
* @return ErrorCode
*/
UINT32 dsp_liveview_sb_update(UINT16 ViewZoneId, UINT8 NumBand, const ULONG *pSidebandBufAddr)
{
    UINT32 Rval;
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;
    UINT32 LocalViewZoneId = 0U;

    Rval = HL_LiveviewSideBandUpdateChk(ViewZoneId, NumBand, pSidebandBufAddr);

    /* Body */
    if (Rval == OK) {
        sideband_info_t *pSidebandInfo = NULL;
        UINT8 i;
        UINT32 ViewZoneVinId;

        HL_GetViewZoneInfoLock(ViewZoneId, &ViewZoneInfo);
        // Update Sideband info to Viewzone info
        ViewZoneInfo->SidebandNum = NumBand;
        ViewZoneInfo->SidebandAddrIdx++;
        if (ViewZoneInfo->SidebandAddrIdx >= MAX_SIDEBAND_BUF_NUM) {
            ViewZoneInfo->SidebandAddrIdx = 0U;
        }
        HL_GetViewZoneInfoUnLock(ViewZoneId);
        LocalViewZoneId = HL_GetViewZoneLocalIndexOnVin(ViewZoneId);
        DSP_Bit2Idx((UINT32)ViewZoneInfo->SourceVin, &ViewZoneVinId);

        // Save SidebandInfo to CtxDspFovSideBandInfo
        HL_GetPointerToDspFovSideBandCfg(ViewZoneId, ViewZoneInfo->SidebandAddrIdx, &pSidebandInfo);
        for (i = 0U; i < NumBand; i++) {
            pSidebandInfo[i].fov_id = LocalViewZoneId;
            (void)dsp_osal_virt2cli(pSidebandBufAddr[i], &pSidebandInfo[i].fov_private_info_addr);
        }

        if (DSP_PROF_STATUS_CAMERA == DSP_GetProfState()) {
            // Runtime update
            if (ViewZoneInfo->InputFromMemory == VIN_SRC_FROM_HW) {
                UINT8 WriteMode;
                UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
                cmd_vin_attach_sideband_info_to_cap_frm_t *VinAttachSideband = HL_DefCtxCmdBufPtrVinSidBend;

                WriteMode = HL_GetVin2CmdNormalWrite((UINT16)ViewZoneVinId);
                HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
                dsp_osal_typecast(&VinAttachSideband, &CmdBufferAddr);
                Rval = HL_FillVideoProcSideBandConfig(VinAttachSideband, ViewZoneId);
                if (Rval == OK) {
                    Rval = AmbaHL_CmdVinAttachSbToCapFrm(WriteMode, VinAttachSideband);
                }
                HL_RelCmdBuffer(CmdBufferId);
            }
        }
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("AmbaDSP_LiveviewSideBandUpdate Error[0x%X]", Rval, 0U, 0U, 0U, 0U);
    }

    return Rval;
}
