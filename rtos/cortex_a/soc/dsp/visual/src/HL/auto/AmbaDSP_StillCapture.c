/**
 *  @file AmbaDSP_StillCapture.c
 *
 * Copyright (c) 2020 Ambarella International LP
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
 *  @details Implementation of SSP Still capture control API
 *
 */

#include "AmbaDSP_Log.h"
#include "AmbaDSP_Cmds.h"
#include "AmbaDSP_ArmComm.h"
#include "AmbaDSP_ApiDump.h"
#include "AmbaDSP_ApiCheck.h"
#include "AmbaDSP_ApiTiming.h"
#include "AmbaDSP_EncodeAPI.h"
#include "AmbaDSP_ApiResource.h"
#include "AmbaDSP_EncodeUtility.h"
#include "AmbaDSP_ImageDefine.h"
#include "dsp_priv_api.h"

static inline void HL_GetYuvStrmIdOnCapCfg(const AMBA_DSP_DATACAP_CFG_s *pDataCapCfg, UINT16 *pYuvStrmId)
{
    CTX_STREAM_INFO_s StrmInfo;

    if (pDataCapCfg->CapDataType == DSP_DATACAP_TYPE_YUV) {
        *pYuvStrmId = pDataCapCfg->Index;
    } else {
        HL_GetStrmInfo(HL_MTX_OPT_ALL, pDataCapCfg->Index, &StrmInfo);
        *pYuvStrmId = StrmInfo.SourceYuvStrmId;
    }

//    AmbaLL_LogUInt5("[HL_GetYuvStrmIdOnCapCfg] CapDataType:%u Idx:%u pYuvStrmId:%u",
//            pDataCapCfg->CapDataType, pDataCapCfg->Index, *pYuvStrmId, 0U, 0U);
}

static UINT8 HL_StillCaptureChkOverFlwCtrl(const UINT16 NumCapInstance,
                                           const AMBA_DSP_DATACAP_CFG_s *pDataCapCfg)
{
    /* Case 0.
     * Original use 1,
     *
     * Case 1.
     * If user need to do infinite capture with limited buffer (e.g. buf=1, capture multiple times)
     * We need to use:
     *     (1) allocation_mode=new / append / append / append ....
     *     (2) overflow_cntl=1
     *     (3) ext_fb_idx=0xfffffffe
     *
     * Case 2.
     * If user need to do limited capture with limited buffer (e.g. buf=5, cap=2)
     * Let user control it, user should do:
     *     (1) allocation_mode=new / append / append / append ....
     *     (2) overflow_cntl=1
     *     (3) ext_fb_idx depends on latest idx
     *
     * NG case 1:
     *     (1) allocation_mode=new / append / append / append ....
     *     (2) overflow_cntl=0
     *     (3) ext_fb_idx=0xfffffffe
     *
     *     Assert at 16s
     *
     * NG case 2:
     *     (1) allocation_mode=new / append / append / append ....
     *     (2) overflow_cntl=0
     *     (3) ext_fb_idx=0
     *
     *     Assert at 64s
     *
     * NG case 3:
     *     (1) allocation_mode=new / new / new / new ....
     *     (2) overflow_cntl=0
     *     (3) ext_fb_idx=0
     *
     *     Assert at 64s
     * */

    UINT8 OverFlwCtrl = VIN_EXT_MEM_CTRL_CIRCULAR;

    /* case 0 */
    if (pDataCapCfg->BufNum == 1U) {
        OverFlwCtrl = VIN_EXT_MEM_CTRL_WAIT_APPEND;
    /* case 1 */
    } else if ((pDataCapCfg->BufNum == NumCapInstance)) {
        OverFlwCtrl = VIN_EXT_MEM_CTRL_WAIT_APPEND;
    /* case 2 */
    } else {
        OverFlwCtrl = (pDataCapCfg->OverFlowCtrl == 0U)? VIN_EXT_MEM_CTRL_WAIT_APPEND: VIN_EXT_MEM_CTRL_CIRCULAR;
    }
    return OverFlwCtrl;
}

/*
 * IsSetPinExtMem = 0, not set ext_mem for that target pin(memory_type) yet, use new
 *                = 1, ext_mem settled, need to use append
 */
static inline UINT8 HL_GetVprocExtMemStatus(const AMBA_DSP_DATACAP_CFG_s *pDataCapCfg)
{
    UINT8 IsSetPinExtMem = 1U, i, PinIdx = DSP_VPROC_PIN_NUM;
    UINT16 ViewZoneId, YuvStrmId = 0U;
    CTX_VPROC_INFO_s VprocInfo;
    CTX_YUV_STRM_INFO_s *YuvStrmInfo = HL_CtxYuvStrmInfoPtr;

    HL_GetYuvStrmIdOnCapCfg(pDataCapCfg, &YuvStrmId);
    HL_GetYuvStrmInfoPtr(YuvStrmId, &YuvStrmInfo);
    //FIXME, First ViewZone when Effect?
    ViewZoneId = YuvStrmInfo->Layout.ChanCfg[0].ViewZoneId;
    HL_GetVprocInfo(HL_MTX_OPT_ALL, ViewZoneId, &VprocInfo);
    for (i=0U; i<DSP_VPROC_PIN_NUM; i++) {
        if (1U == DSP_GetBit(VprocInfo.PinUsage[i], YuvStrmId, 1U)) {
            PinIdx = i;
        }
    }
    if (PinIdx<DSP_VPROC_PIN_NUM) {
        IsSetPinExtMem = VprocInfo.IsSetPinExtMem[PinIdx];
    } else {
        AmbaLL_LogUInt5("[HL_GetVprocExtMemStatus] Get error", ViewZoneId, 0U, 0U, 0U, 0U);
    }
    return IsSetPinExtMem;
}

static inline void HL_SetVprocExtMemStatus(const AMBA_DSP_DATACAP_CFG_s *pDataCapCfg, UINT8 Status)
{
    UINT8 ExitKLoop, i;
    UINT16 ViewZoneId, YuvStrmId = 0U;
    CTX_VPROC_INFO_s VprocInfo;
    CTX_YUV_STRM_INFO_s *YuvStrmInfo = HL_CtxYuvStrmInfoPtr;

    HL_GetYuvStrmIdOnCapCfg(pDataCapCfg, &YuvStrmId);
    HL_GetYuvStrmInfoPtr(YuvStrmId, &YuvStrmInfo);
    //FIXME, First ViewZone when Effect?
    ViewZoneId = YuvStrmInfo->Layout.ChanCfg[0].ViewZoneId;
    HL_GetVprocInfo(HL_MTX_OPT_GET, ViewZoneId, &VprocInfo);

    ExitKLoop = 0U;
    for (i=0U; i<DSP_VPROC_PIN_NUM; i++) {
        if (1U == DSP_GetBit(VprocInfo.PinUsage[i], YuvStrmId, 1U)) {
            VprocInfo.IsSetPinExtMem[i] = Status;
            ExitKLoop = 1U;
            break;
        }
    }
    HL_SetVprocInfo(HL_MTX_OPT_SET, ViewZoneId, &VprocInfo);
    if (ExitKLoop == 1U) {
        //
    } else {
        AmbaLL_LogUInt5("[HL_SetVprocExtMemStatus] Get error", ViewZoneId, 0U, 0U, 0U, 0U);
    }
}


static UINT32 HL_CountDownVinAttachEvent(UINT16 VinId)
{
    UINT32 Rval = DSP_ERR_NONE, ErrLine = 0U;
    CTX_VIN_INFO_s VinInfo = {0};
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;
    UINT16 i, j, ViewZoneVinId, PhysVinId = 0U;
    UINT32 LocalIndex = 0U;

    if (VinId >= AMBA_DSP_MAX_VIN_NUM) {
        Rval = DSP_ERR_0000;
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
    } else {
        for (i = 0U; i < AMBA_DSP_MAX_VIEWZONE_NUM; i++) {
            HL_GetViewZoneVinId(i, &ViewZoneVinId);
            HL_GetViewZoneInfoPtr(i, &ViewZoneInfo);
            HL_GetViewZonePhysVinId(ViewZoneInfo, &PhysVinId);

            if (ViewZoneVinId == VIN_IDX_INVALID) {
                continue;
            }

            HL_GetVinInfo(HL_MTX_OPT_ALL, ViewZoneVinId, &VinInfo);
            if ((VinInfo.AttachEventCountDownCheck >> LocalIndex) == 0U) {
                continue;
            }

            if (PhysVinId == VinId) {
                if (1U == DSP_GetBit(VinInfo.AttachEventCountDownCheck, LocalIndex, 1U)) {
                    UINT32 IssueCmd = 0U, KeepEventMask = 0U, DefaultEventMask = 0U, LocalIndexKeepCheck = 0U;

                    for (j = 0U; j < DSP_VZ_ATTCH_PIN_NUM; j++) {
                        if (ViewZoneInfo->AttachEventCountDown[j] == DSP_DATACAP_INFINITE_CAP_NUM) {
                            // Infinite capture, don't need countdown cap_num, just keep event mask
                            KeepEventMask |= ViewZoneInfo->ExtraEventMask[j];
                        } else if (ViewZoneInfo->AttachEventCountDown[j] == 1U) {
                            // Last capture, need to stop VinAttachEvent and clear event mask
                            IssueCmd = 1;

                            HL_GetViewZoneInfoLock(i, &ViewZoneInfo);
                            ViewZoneInfo->AttachEventCountDown[j]--;
                            ViewZoneInfo->ExtraEventMask[j] = 0U;
                            HL_GetViewZoneInfoUnLock(i);
                        } else if (ViewZoneInfo->AttachEventCountDown[j] > 0U) {
                            // Keep event mask
                            LocalIndexKeepCheck = 1U;
                            KeepEventMask |= ViewZoneInfo->ExtraEventMask[j];
                            HL_GetViewZoneInfoLock(i, &ViewZoneInfo);
                            ViewZoneInfo->AttachEventCountDown[j]--;
                            HL_GetViewZoneInfoUnLock(i);
                        } else {
                            // To nothing
                        }
                    }

                    if (IssueCmd == 1U) {
                        UINT8 WriteMode = AMBA_DSP_CMD_VDSP_NORMAL_WRITE;
                        UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
                        cmd_vin_attach_event_to_raw_t *VinAttachEvent = HL_DefCtxCmdBufPtrVinAttRaw;

                        HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
                        dsp_osal_typecast(&VinAttachEvent, &CmdBufferAddr);
                        Rval = HL_GetVinAttachEventDefaultMask(VinId, &DefaultEventMask);
                        DSP_FillErrline(Rval, &ErrLine, __LINE__);
                        VinAttachEvent->vin_id = (UINT8)ViewZoneVinId;
                        VinAttachEvent->ch_id = (UINT8)0U; //TBD
                        VinAttachEvent->event_mask = KeepEventMask|DefaultEventMask;
                        VinAttachEvent->repeat_cnt = 0x0;
                        VinAttachEvent->time_duration_bet_rpts = 0U;
                        VinAttachEvent->is_indep_fov_control = (UINT8)1U;
                        DSP_SetBit(&VinAttachEvent->fov_mask_valid, LocalIndex);

                        if (Rval == DSP_ERR_NONE) {
                            WriteMode = HL_GetVin2CmdNormalWrite(VinId);
                            Rval = AmbaHL_CmdVinAttachEventToRaw(WriteMode, VinAttachEvent);
                            DSP_FillErrline(Rval, &ErrLine, __LINE__);
                        }
                        HL_RelCmdBuffer(CmdBufferId);
                    }

                    if (LocalIndexKeepCheck == 0U) {
                        HL_GetVinInfo(HL_MTX_OPT_GET, VinId, &VinInfo);
                        DSP_ClearBit(&VinInfo.AttachEventCountDownCheck, LocalIndex);
                        HL_SetVinInfo(HL_MTX_OPT_SET, VinId, &VinInfo);
                    }
                }
                LocalIndex++;
            }
        }
    }

    if (Rval != DSP_ERR_NONE) {
        AmbaLL_LogUInt5("HL_CountDownVinAttachEvent Error[0x%x][%u]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

/*
 * IsSetPinExtMem = 0, not set ext_mem for that target vin yet, use new
 *                = 1, ext_mem settled, need to use append
 */
static inline UINT8 HL_GetVinExtMemStatus(const AMBA_DSP_DATACAP_CFG_s *pDataCapCfg)
{
    UINT8 IsSetPinExtMem;
    CTX_VIN_INFO_s VinInfo;

    HL_GetVinInfo(HL_MTX_OPT_ALL, pDataCapCfg->Index, &VinInfo);
    IsSetPinExtMem = VinInfo.IsSetPinExtMem;

    return IsSetPinExtMem;
}

static inline void HL_SetVinExtMemStatus(const AMBA_DSP_DATACAP_CFG_s *pDataCapCfg, UINT8 Status)
{
    CTX_VIN_INFO_s VinInfo;

    HL_GetVinInfo(HL_MTX_OPT_GET, pDataCapCfg->Index, &VinInfo);
    VinInfo.IsSetPinExtMem = Status;
    HL_SetVinInfo(HL_MTX_OPT_SET, pDataCapCfg->Index, &VinInfo);
}

static inline UINT32 HL_FillVinExtMemInRawCap(cmd_vin_set_ext_mem_t *pVinExtMem,
                                              const UINT8 IsSetPinExtMem,
                                              const UINT16 CapId,
                                              const CTX_DATACAP_INFO_s *pDataCapInfo,
                                              const CTX_VIN_INFO_s *pVinInfo,
                                              const UINT8 OverFlowCtrl)
{
    UINT32 Rval = DSP_ERR_NONE, ErrLine = 0U;
    ULONG ULAddr;
    const AMBA_DSP_DATACAP_CFG_s *pDataCapCfg = &pDataCapInfo->Cfg;
    UINT16 i;
    UINT32 *pBufTblAddr = NULL;
    UINT32 PhysAddr;

    pVinExtMem->vin_id = (UINT8)pDataCapCfg->Index;
    pVinExtMem->allocation_type = (pDataCapCfg->AllocType == ALLOC_EXTERNAL_DISTINCT)? VIN_EXT_MEM_ALLOC_TYPE_DISTINCT: VIN_EXT_MEM_ALLOC_TYPE_CONTIGUOUS;
    /* 0 = first time, need new, 0=append,
     * Need to check with target vin/vproc every time before capture
     * Every mem type need to do new once, then append until vin/vproc restart
     * vin/vproc reset it's value at it's start */
    pVinExtMem->allocation_mode = (IsSetPinExtMem == 0U) ? VIN_EXT_MEM_MODE_NEW : VIN_EXT_MEM_MODE_APPEND;
    pVinExtMem->overflow_cntl = OverFlowCtrl;

    pVinExtMem->num_frm_buf = pDataCapCfg->BufNum;
    if (pVinExtMem->num_frm_buf > EXT_MEM_VIN_MAX_NUM) {
        AmbaLL_LogUInt5("Only %d ExtMem supported", EXT_MEM_VIN_MAX_NUM, 0U, 0U, 0U, 0U);
    }
    pVinExtMem->buf_pitch = ALIGN_NUM16(pDataCapCfg->DataBuf.Pitch, (UINT16)DSP_BUF_ALIGNMENT);
    pVinExtMem->buf_width = pVinExtMem->buf_pitch;
    pVinExtMem->buf_height = (UINT16)pDataCapCfg->DataBuf.Window.Height;

    if (pDataCapCfg->AllocType == ALLOC_EXTERNAL_DISTINCT) {
        HL_GetPointerToDspDataCapBufArray(CapId, &pBufTblAddr);
        for (i = 0; i < pDataCapCfg->BufNum; i++) {
            Rval = dsp_osal_virt2cli(pDataCapInfo->BufTbl[i], &PhysAddr);
            DSP_FillErrline(Rval, &ErrLine, __LINE__);
            pBufTblAddr[i] = PhysAddr;
        }
        if (Rval == DSP_ERR_NONE) {
            dsp_osal_typecast(&ULAddr, &pBufTblAddr);
            Rval = dsp_osal_virt2cli(ULAddr, &pVinExtMem->buf_addr);
            DSP_FillErrline(Rval, &ErrLine, __LINE__);
        }
    } else {
        Rval = dsp_osal_virt2cli(pDataCapCfg->DataBuf.BaseAddr, &pVinExtMem->buf_addr);
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
    }

    if (HL_IS_YUV422_INPUT(pVinInfo->InputFormat) == 1U) {
        // YUV Vin
        pVinExtMem->buf_width = (UINT16)pDataCapCfg->DataBuf.Window.Width;
        pVinExtMem->memory_type = DSP_VIN_EXT_MEM_TYPE_YUV422;
        pVinExtMem->chroma_format = DSP_YUV_422;
        /*
         * vin_yuv_enc_fbp_init will allocate resource for virtual vin yuv input from ext_mem at vin_flow_max stage
         * Need to allocate resource (vin_yuv_enc_fbp_disable=0) when send ext_mem after vproc
         * */
        pVinExtMem->vin_yuv_enc_fbp_disable = 0U;
    } else if (HL_IS_YUV420_INPUT(pVinInfo->InputFormat) == 1U) {
        // YUV Vin
        pVinExtMem->buf_width = (UINT16)pDataCapCfg->DataBuf.Window.Width;
        pVinExtMem->memory_type = DSP_VIN_EXT_MEM_TYPE_YUV422;
        pVinExtMem->chroma_format = DSP_YUV_420;
        /*
         * vin_yuv_enc_fbp_init will allocate resource for virtual vin yuv input from ext_mem at vin_flow_max stage
         * Need to allocate resource (vin_yuv_enc_fbp_disable=0) when send ext_mem after vproc
         * */
        pVinExtMem->vin_yuv_enc_fbp_disable = 0U;
    } else {
        // Sensor Vin
        pVinExtMem->chroma_format = DSP_YUV_MONO;
        pVinExtMem->memory_type = DSP_VIN_EXT_MEM_TYPE_RAW;
    }

    if (Rval != DSP_ERR_NONE) {
        AmbaLL_LogUInt5("HL_FillVinExtMemInRawCap Error[0x%x][%u]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

static inline UINT32 HL_FillVinExtAuxMemInRawCap(cmd_vin_set_ext_mem_t *pVinExtMem,
                                                 const UINT8 IsSetPinExtMem,
                                                 const UINT16 CapId,
                                                 const CTX_DATACAP_INFO_s *pDataCapInfo,
                                                 const UINT8 OverFlowCtrl)
{
    UINT32 Rval = DSP_ERR_NONE, ErrLine = 0U;
    ULONG ULAddr;
    const AMBA_DSP_DATACAP_CFG_s *pDataCapCfg = &pDataCapInfo->Cfg;
    UINT16 i;
    UINT32 *pBufTblAddr = NULL;
    UINT32 PhysAddr;

    pVinExtMem->vin_id = (UINT8)pDataCapCfg->Index;
    pVinExtMem->allocation_type = (pDataCapCfg->AllocType == ALLOC_EXTERNAL_DISTINCT)? VIN_EXT_MEM_ALLOC_TYPE_DISTINCT: VIN_EXT_MEM_ALLOC_TYPE_CONTIGUOUS;
    /* 0 = first time, need new, 0=append,
     * Need to check with target vin/vproc every time before capture
     * Every mem type need to do new once, then append until vin/vproc restart
     * vin/vproc reset it's value at it's start */
    pVinExtMem->allocation_mode = (IsSetPinExtMem == 0U) ? VIN_EXT_MEM_MODE_NEW : VIN_EXT_MEM_MODE_APPEND;
    pVinExtMem->overflow_cntl = OverFlowCtrl;

    pVinExtMem->num_frm_buf = pDataCapCfg->BufNum;
    if (pVinExtMem->num_frm_buf > EXT_MEM_VIN_MAX_NUM) {
        AmbaLL_LogUInt5("Only %d ExtMem supported", EXT_MEM_VIN_MAX_NUM, 0U, 0U, 0U, 0U);
    }
    pVinExtMem->buf_pitch = ALIGN_NUM16(pDataCapCfg->AuxDataBuf.Pitch, (UINT16)DSP_BUF_ALIGNMENT);
    pVinExtMem->buf_width = pVinExtMem->buf_pitch;
    pVinExtMem->buf_height = (UINT16)pDataCapCfg->AuxDataBuf.Window.Height;
    if (pDataCapCfg->AllocType == ALLOC_EXTERNAL_DISTINCT) {
        HL_GetPointerToDspDataCapAuxBufArray(CapId, &pBufTblAddr);
        for (i = 0; i < pDataCapCfg->BufNum; i++) {
            Rval = dsp_osal_virt2cli(pDataCapInfo->AuxBufTbl[i], &PhysAddr);
            DSP_FillErrline(Rval, &ErrLine, __LINE__);
            pBufTblAddr[i] = PhysAddr;
        }
        if (Rval == DSP_ERR_NONE) {
            dsp_osal_typecast(&ULAddr, &pBufTblAddr);
            Rval = dsp_osal_virt2cli(ULAddr, &pVinExtMem->buf_addr);
            DSP_FillErrline(Rval, &ErrLine, __LINE__);
        }
    } else {
        Rval = dsp_osal_virt2cli(pDataCapCfg->AuxDataBuf.BaseAddr, &pVinExtMem->buf_addr);
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
    }

    pVinExtMem->chroma_format = DSP_YUV_MONO;
    pVinExtMem->memory_type = DSP_VIN_EXT_MEM_TYPE_HDS;

    if (Rval != DSP_ERR_NONE) {
        AmbaLL_LogUInt5("HL_FillVinExtAuxMemInRawCap Error[0x%x][%u]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

static inline void HL_FillVinInitCapInRawCap(cmd_vin_initiate_raw_cap_to_ext_buf_t *pVinInitRawCap,
                                               const CTX_DATACAP_INFO_s *pDataCapInfo,
                                               const CTX_VIN_INFO_s *pVinInfo)
{
    const AMBA_DSP_DATACAP_CFG_s *pDataCapCfg = &pDataCapInfo->Cfg;
    UINT8 CfaCompressed = 0U, CfaCompact = 0U;

    pVinInitRawCap->vin_id = (UINT8)pDataCapCfg->Index;
    pVinInitRawCap->stop_condition = pDataCapInfo->CountDown;
    if (HL_IS_YUV422_INPUT(pVinInfo->InputFormat) == 1U) {
        // YUV Vin
        pVinInitRawCap->input_data_type = (UINT8)DSP_VIN_SEND_IN_DATA_YUV422;
    } else if (HL_IS_YUV420_INPUT(pVinInfo->InputFormat) == 1U) {
        // YUV Vin
        pVinInitRawCap->input_data_type = (UINT8)DSP_VIN_SEND_IN_DATA_YUV420;
    } else {
        // Sensor Vin
        pVinInitRawCap->input_data_type = (UINT8)DSP_VIN_SEND_IN_DATA_RAW;
    }

    if (pVinInfo->VinCtrl.VinState == DSP_VIN_STATUS_TIMER) {
        CfaCompressed = pDataCapCfg->CmprRate;
        CfaCompact = pDataCapCfg->CmptRate;
    } else {
        CfaCompressed = pVinInfo->CfaCompressed;
        CfaCompact = pVinInfo->CfaCompact;
    }
    if (CfaCompressed == (UINT8)0U) {
        if (CfaCompact != 0U) {
            pVinInitRawCap->raw_compression_type = (UINT8)(RawCmptRateTable[CfaCompact].Mantissa);
        } else {
            pVinInitRawCap->raw_compression_type = (UINT8)DSP_VIN_CMPR_NONE;
        }
    } else {
        pVinInitRawCap->raw_compression_type = (UINT8)DSP_VIN_CMPR_6_75;
    }

    /*
     * There are two applications: (1) PIV raw capture or (2) Still raw capture
     * In (2) Vin will be restarted along with init_raw_cap
     */
    if (pVinInfo->VinCtrl.VinState == DSP_VIN_STATUS_TIMER) {
        pVinInitRawCap->raw_cap_done_sw_ctrl = DSP_VIN_CAP_DONE_2_IDLE;
        pVinInitRawCap->raw_buffer_proc_ctrl = (UINT8)DSP_VIN_CAP_ONLY;
    } else {
        pVinInitRawCap->raw_cap_done_sw_ctrl = (UINT8)DSP_VIN_CAP_DONE_2_INT_BUF;
        pVinInitRawCap->raw_buffer_proc_ctrl = (UINT8)DSP_VIN_CAP_TO_YUV;
    }
//FIXME
    pVinInitRawCap->raw_cap_sync_event = (UINT8)0U;

}

static inline void HL_GetVcapWriteModeOnTimerMode(UINT16 VinBit, UINT8 *AllVinIdle, UINT8 *WriteMode)
{
    CTX_VIN_INFO_s VinInfo;
    UINT16 i, MinActiveVinId = 0xFFU, NumOfActiveCapVin = 0U;

    for (i=0U; i<AMBA_DSP_MAX_VIN_NUM; i++) {
        HL_GetVinInfo(HL_MTX_OPT_ALL, i, &VinInfo);
        if (VinInfo.VinCtrl.VinState == DSP_VIN_STATUS_ACTIVE) {
            if (MinActiveVinId == 0xFFU) {
                MinActiveVinId = i;
            }
            if (DSP_GetU16Bit(VinBit, i, 1U) == 1U) {
                NumOfActiveCapVin++;
            }
        }
    }
    if (NumOfActiveCapVin == 0U) {
        if (MinActiveVinId == 0xFFU) {
            *WriteMode = AMBA_DSP_CMD_VCAP_GROUP_WRITE;
        } else {
            *WriteMode = HL_GetVin2CmdGroupWrite(MinActiveVinId);
        }
        *AllVinIdle = 1U;
    } else {
        *AllVinIdle = 0U;
    }
}

static inline UINT32 HL_StillCaptureRawImpl2ndHalf(const UINT16 *pCapInstance, const CTX_VIN_INFO_s *pVinInfo,
                                                 const AMBA_DSP_DATACAP_CFG_s *pDataCapCfg, UINT8 WriteMode, UINT8 VinStartPurpose,
                                                 UINT16 *pVinPostCfgBitMask, CTX_DATACAP_INFO_s *pDataCapInfo, UINT16 i,
                                                 cmd_vin_initiate_raw_cap_to_ext_buf_t *pVinInitRawCap)
{
    UINT32 Rval = DSP_ERR_NONE, ErrLine = 0U;

    if (pVinInfo->VinCtrl.VinState == DSP_VIN_STATUS_TIMER) {
        UINT8 CmdSkip = (UINT8)0U;
        UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
        set_vin_config_t *VinCfg = HL_DefCtxCmdBufPtrVinCfg;
        cmd_vin_start_t *VinStartCfg = HL_DefCtxCmdBufPtrVinStart;
        cmd_vin_ce_setup_t *CeSetup = HL_DefCtxCmdBufPtrVinCe;
        lossy_compression_t *CeCmprSetup = HL_DefCtxCmdBufPtrCmpr;
        set_hdsp_config_t *HdspSetup = HL_DefCtxCmdBufPtrHdsp;

        HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
        dsp_osal_typecast(&VinCfg, &CmdBufferAddr);
        Rval = HL_FillVinConfig(VinCfg, pDataCapCfg->Index);
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
        if (Rval == DSP_ERR_NONE) {
            Rval = AmbaHL_CmdSetVinConfig(WriteMode, VinCfg);
            DSP_FillErrline(Rval, &ErrLine, __LINE__);
        }
        HL_RelCmdBuffer(CmdBufferId);

        if ((Rval == DSP_ERR_NONE) && (pDataCapCfg->AuxDataNeeded > 0U)) {
            UINT8 SkipCmd = 0U;

            HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
            dsp_osal_typecast(&CeCmprSetup, &CmdBufferAddr);
            CmdSkip = (UINT8)0U;
            Rval = HL_FillVinHdsCmpr(pDataCapCfg->Index,
                                     VIN_START_STILL_RAW_ONLY,
                                     pDataCapCfg,
                                     CeCmprSetup,
                                     &SkipCmd);
            DSP_FillErrline(Rval, &ErrLine, __LINE__);
            if (CmdSkip == 1U) {
                // Skip it
            } else {
                Rval = AmbaHL_CmdSetHdsCompression(WriteMode, CeCmprSetup);
                DSP_FillErrline(Rval, &ErrLine, __LINE__);
            }
            HL_RelCmdBuffer(CmdBufferId);

            HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
            dsp_osal_typecast(&CeSetup, &CmdBufferAddr);
            CeSetup->vin_id = (UINT8)(pDataCapCfg->Index);
            CeSetup->number_ce_exp = 0U; //obsolete in CV2/CV22
            CeSetup->primary_out_exp = 0U; //obsolete in CV2/CV22
            CeSetup->secondary_out_exp = 0U; //obsolete in CV2/CV22
            CeSetup->ce_width = pDataCapCfg->AuxDataBuf.Window.Width; //Fixed 4x DownScale
            CeSetup->ce_height = pDataCapCfg->AuxDataBuf.Window.Height;
            CeSetup->ce_out_format = 0U; //obsolete in CV2/CV222
            if (Rval == DSP_ERR_NONE) {
                Rval = AmbaHL_CmdVinCeSetup(WriteMode, CeSetup);
                DSP_FillErrline(Rval, &ErrLine, __LINE__);
                HL_RelCmdBuffer(CmdBufferId);

                HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
                dsp_osal_typecast(&HdspSetup, &CmdBufferAddr);
                Rval = HL_FillVinHdspSetup(pDataCapCfg->Index, HdspSetup);
                DSP_FillErrline(Rval, &ErrLine, __LINE__);
                Rval = AmbaHL_CmdSetHdspConfig(WriteMode, HdspSetup);
                DSP_FillErrline(Rval, &ErrLine, __LINE__);
                HL_RelCmdBuffer(CmdBufferId);
            } else {
                HL_RelCmdBuffer(CmdBufferId);
            }
        }

        if (Rval == DSP_ERR_NONE) {
            HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
            dsp_osal_typecast(&VinStartCfg, &CmdBufferAddr);
            CmdSkip = (UINT8)0U;
            Rval = HL_FillVinStartCfg(VinStartCfg,
                                     pDataCapCfg->Index,
                                     VinStartPurpose,
                                     VIN_START_CFG_CMD,
                                     pDataCapCfg,
                                     &CmdSkip);
            DSP_FillErrline(Rval, &ErrLine, __LINE__);
            Rval = AmbaHL_CmdVinStart(WriteMode, VinStartCfg);
            DSP_FillErrline(Rval, &ErrLine, __LINE__);
            HL_RelCmdBuffer(CmdBufferId);
        }

        /* Find Vin needing PostCfg */
        if (((UINT16)pVinInfo->PostCfgType & AMBA_DSP_VIN_CONFIG_POST) > 0U) {
            DSP_SetU16Bit(pVinPostCfgBitMask, pDataCapCfg->Index);
        }

        HL_GetDataCapInfo(HL_MTX_OPT_GET, pCapInstance[i], pDataCapInfo);
        pDataCapInfo->VprocIdle = 1U;
        HL_SetDataCapInfo(HL_MTX_OPT_SET, pCapInstance[i], pDataCapInfo);
    }

    HL_FillVinInitCapInRawCap(pVinInitRawCap, pDataCapInfo, pVinInfo);
    if (pVinInfo->VinCtrl.VinState == DSP_VIN_STATUS_TIMER) {
        pVinInitRawCap->raw_cap_done_sw_ctrl = DSP_VIN_CAP_DONE_2_IDLE;
    }
    if (Rval == DSP_ERR_NONE) {
        Rval = AmbaHL_CmdVinInitiateRawCapToExtBuf(WriteMode, pVinInitRawCap);
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
    }
    if (pDataCapInfo->Status != DATA_CAP_STATUS_2_STOP) {
        HL_GetDataCapInfo(HL_MTX_OPT_GET, pCapInstance[i], pDataCapInfo);
        pDataCapInfo->Status = DATA_CAP_STATUS_RUNNING;
        HL_SetDataCapInfo(HL_MTX_OPT_SET, pCapInstance[i], pDataCapInfo);
    }

    if (Rval != DSP_ERR_NONE) {
        AmbaLL_LogUInt5("HL_StillCaptureRawImpl2ndHalf Error[0x%x][%u]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

static inline UINT32 HL_StlCapRawPreproc(const UINT16 NumCapInstance,
                                       const UINT16 CapId,
                                       const AMBA_DSP_DATACAP_CFG_s *pDataCapCfg,
                                       const CTX_DATACAP_INFO_s *pDataCapInfo,
                                       const UINT8 WriteMode,
                                       CTX_VIN_INFO_s *pVinInfo)
{
    UINT32 Rval = DSP_ERR_NONE, ErrLine = 0U;

    HL_GetVinInfo(HL_MTX_OPT_ALL, pDataCapCfg->Index, pVinInfo);
    if (pVinInfo->VinCtrl.VinState == DSP_VIN_STATUS_TIMER) {
        HL_GetVinInfo(HL_MTX_OPT_GET, pDataCapCfg->Index, pVinInfo);
        if (pDataCapCfg->CapDataType == DSP_DATACAP_TYPE_RAW) {
            pVinInfo->StlCfaCmpr = pDataCapCfg->CmprRate;
        } else {
            // keep the same
        }
        HL_SetVinInfo(HL_MTX_OPT_SET, pDataCapCfg->Index, pVinInfo);
    } else {
        //Capture under Liveview, keep VinCmpr the same as Liveview
    }

    if (pDataCapInfo->Status != DATA_CAP_STATUS_2_STOP) {
        UINT8 OverFlowCtrl = HL_StillCaptureChkOverFlwCtrl(NumCapInstance, pDataCapCfg);
        UINT8 IsSetPinExtMem = HL_GetVinExtMemStatus(pDataCapCfg);
        UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
        cmd_vin_set_ext_mem_t *VinExtMem = HL_DefCtxCmdBufPtrVinExtMem;

        // SetExtMem
        HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
        dsp_osal_typecast(&VinExtMem, &CmdBufferAddr);

        /* 1= first time, need new, 0=append,
         * Need to check with target vin/vproc every time before capture
         * Every mem type need to do new once, then append until vin/vproc restart
         * vin/vproc reset it's value at it's start */

        Rval = HL_FillVinExtMemInRawCap(VinExtMem,
                                       IsSetPinExtMem,
                                       CapId,
                                       pDataCapInfo,
                                       pVinInfo,
                                       OverFlowCtrl);
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
        if (Rval == DSP_ERR_NONE) {
            Rval = AmbaHL_CmdVinSetExtMem(WriteMode, VinExtMem);
            DSP_FillErrline(Rval, &ErrLine, __LINE__);
        }
        HL_RelCmdBuffer(CmdBufferId);

        if ((Rval == DSP_ERR_NONE) && (pDataCapCfg->AuxDataNeeded > 0U)) {
            HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
            dsp_osal_typecast(&VinExtMem, &CmdBufferAddr);

            Rval = HL_FillVinExtAuxMemInRawCap(VinExtMem,
                                              IsSetPinExtMem,
                                              CapId,
                                              pDataCapInfo,
                                              OverFlowCtrl);
            DSP_FillErrline(Rval, &ErrLine, __LINE__);
            if (Rval == DSP_ERR_NONE) {
                Rval = AmbaHL_CmdVinSetExtMem(WriteMode, VinExtMem);
                DSP_FillErrline(Rval, &ErrLine, __LINE__);
            }
            HL_RelCmdBuffer(CmdBufferId);
        }

        if (IsSetPinExtMem == 0U) {
            HL_SetVinExtMemStatus(pDataCapCfg, 1U);
        }
    }

    if (Rval != DSP_ERR_NONE) {
        AmbaLL_LogUInt5("HL_StlCapRawPreproc Error[0x%x][%u]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;

}

static inline UINT32 HL_StlCapRawImpl(const UINT16 NumCapInstance, const UINT16 *pCapInstance)
{
    UINT8 AllVinIdle;
    UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
    UINT8 WriteMode = AMBA_DSP_CMD_VCAP_GROUP_WRITE;
    UINT8 VinStartPurpose = VIN_START_STILL_RAW_ONLY;
    UINT16 i, VinPostCfgBitMask = 0U, CapVinBit = 0U;
    UINT32 Rval = DSP_ERR_NONE, ErrLine = 0U;
    CTX_VIN_INFO_s VinInfo;
    CTX_DATACAP_INFO_s DataCapInfo;
    cmd_vin_initiate_raw_cap_to_ext_buf_t *VinInitRawCap = HL_DefCtxCmdBufPtrVinInitCap;
    const AMBA_DSP_DATACAP_CFG_s *pDataCapCfg;


    for (i = 0U; i < NumCapInstance; i++) {
        HL_GetDataCapInfo(HL_MTX_OPT_ALL, pCapInstance[i], &DataCapInfo);
        pDataCapCfg = &DataCapInfo.Cfg;
        DSP_SetU16Bit(&CapVinBit, pDataCapCfg->Index);
    }
    HL_GetVcapWriteModeOnTimerMode(CapVinBit, &AllVinIdle, &WriteMode);

    for (i=0U; i<NumCapInstance; i++) {
        HL_GetDataCapInfo(HL_MTX_OPT_ALL, pCapInstance[i], &DataCapInfo);
        pDataCapCfg = &DataCapInfo.Cfg;

        /* Timing sanity check */
        if ((DataCapInfo.Status != DATA_CAP_STATUS_UPDATED) &&
            (DataCapInfo.Status != DATA_CAP_STATUS_2_STOP)) {
            continue;
        }
        /* Input sanity check */
        if ((pDataCapCfg->CapDataType != DSP_DATACAP_TYPE_RAW) &&
            (pDataCapCfg->CapDataType != DSP_DATACAP_TYPE_COMP_RAW)) {
            continue;
        }

        if (AllVinIdle == 0U) {
            WriteMode = HL_GetVin2CmdGroupWrite(pDataCapCfg->Index);
        }

        Rval = HL_StlCapRawPreproc(NumCapInstance,
                            pCapInstance[i],
                            pDataCapCfg,
                            &DataCapInfo,
                            WriteMode,
                            &VinInfo);
        DSP_FillErrline(Rval, &ErrLine, __LINE__);

        // Trigger capture
        HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
        dsp_osal_typecast(&VinInitRawCap, &CmdBufferAddr);

        // Trigger Vin-start if it's currently in timer mode
        if (Rval == DSP_ERR_NONE) {
            Rval = HL_StillCaptureRawImpl2ndHalf(pCapInstance, &VinInfo, pDataCapCfg, WriteMode, VinStartPurpose, &VinPostCfgBitMask,
                                          &DataCapInfo, i, VinInitRawCap);
            DSP_FillErrline(Rval, &ErrLine, __LINE__);
        }
        HL_RelCmdBuffer(CmdBufferId);

        if ((Rval == DSP_ERR_NONE) && (AllVinIdle == 0U)) {
            Rval = DSP_ProcessBufCmds(WriteMode);
            DSP_FillErrline(Rval, &ErrLine, __LINE__);
        }
    }
    if ((Rval == DSP_ERR_NONE) && (AllVinIdle == 1U)) {
        Rval = DSP_ProcessBufCmds(WriteMode);
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
    }

    /* Handle VinPostCfg first, Wait until VinRegCfged */
    if (Rval == DSP_ERR_NONE) {
        Rval = HL_VinPostCfgHandler(VinPostCfgBitMask);
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
    }

    if (Rval != DSP_ERR_NONE) {
        AmbaLL_LogUInt5("HL_StlCapRawImpl Error[0x%x][%u]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;

}

static inline UINT32 HL_FillVprocExtMemInYuvCap(cmd_vproc_set_ext_mem_t *pVprocExtMem,
                                                const CTX_DATACAP_INFO_s *pDataCapInfo,
                                                const UINT8 IsSetPinExtMem,
                                                const UINT16 YuvStrmId,
                                                const UINT8 MemType,
                                                const UINT8 OverFlowCtrl)
{
    UINT32 Rval = DSP_ERR_NONE, ErrLine = 0U;
    ULONG ULAddr;
    AMBA_DSP_BUF_s CalcExtBuf = {0U};
    const AMBA_DSP_DATACAP_CFG_s *pDataCapCfg = &pDataCapInfo->Cfg;
    UINT16 i;
    UINT32 *pBufTblAddr = NULL;
    UINT32 PhysAddr;

    (void)OverFlowCtrl;
    (void)IsSetPinExtMem;
    pVprocExtMem->memory_type = MemType;
    pVprocExtMem->allocation_type = (pDataCapCfg->AllocType == ALLOC_EXTERNAL_DISTINCT)? VIN_EXT_MEM_ALLOC_TYPE_DISTINCT: VIN_EXT_MEM_ALLOC_TYPE_CONTIGUOUS;
    /* 0 = first time, need new, 1=append,
     * Need to check with target vin/vproc every time before capture
     * Every mem type need to do new once, then append until vin/vproc restart
     * vin/vproc reset it's value at it's start */

    /* For vdo thm case / capture main case
     * If doing p60+p30 together, wait vcap for vin0 will be delayed by vin1
     * since overflow_cntl=1 need accurate control on the frame
     * so we use overflow_cntl=0 and allocation_mode always new to do it
     * */
    pVprocExtMem->allocation_mode = VIN_EXT_MEM_MODE_NEW;
    pVprocExtMem->overflow_cntl = VIN_EXT_MEM_CTRL_CIRCULAR;
    pVprocExtMem->num_frm_buf = pDataCapCfg->BufNum;
    if (pVprocExtMem->num_frm_buf > EXT_MEM_VPROC_MAX_NUM) {
        AmbaLL_LogUInt5("Only %d ExtMem supported", EXT_MEM_VPROC_MAX_NUM, 0U, 0U, 0U, 0U);
    }
    Rval = HL_StillCaptureCalcExtBufImpl(YuvStrmId, DSP_DATACAP_CALTYPE_LUMA, &CalcExtBuf);
    DSP_FillErrline(Rval, &ErrLine, __LINE__);
    pVprocExtMem->buf_pitch = CalcExtBuf.Pitch;
    pVprocExtMem->buf_width = CalcExtBuf.Window.Width;
    pVprocExtMem->buf_height= CalcExtBuf.Window.Height;

    if (Rval == DSP_ERR_NONE) {
        if (pDataCapCfg->AllocType == ALLOC_EXTERNAL_DISTINCT) {
            HL_GetPointerToDspExtYuvStrmBufArray(YuvStrmId, &pBufTblAddr);
            for (i = 0; i < pDataCapCfg->BufNum; i++) {
                if (Rval == DSP_ERR_NONE) {
                    Rval = dsp_osal_virt2cli(pDataCapInfo->BufTbl[i], &PhysAddr);
                    DSP_FillErrline(Rval, &ErrLine, __LINE__);
                    pBufTblAddr[i] = PhysAddr;
                }
            }
            dsp_osal_typecast(&ULAddr, &pBufTblAddr);
            if (Rval == DSP_ERR_NONE) {
                Rval = dsp_osal_virt2cli(ULAddr, &pVprocExtMem->buf_addr);
                DSP_FillErrline(Rval, &ErrLine, __LINE__);
            }
        } else {
                Rval = dsp_osal_virt2cli(pDataCapCfg->DataBuf.BaseAddr, &pVprocExtMem->buf_addr);
                DSP_FillErrline(Rval, &ErrLine, __LINE__);
        }
    }
    pVprocExtMem->img_width = pDataCapCfg->DataBuf.Window.Width;
    pVprocExtMem->img_height = pDataCapCfg->DataBuf.Window.Height;
//FIXME, any possiblity that Yuv422 is used?
    pVprocExtMem->chroma_format = DSP_YUV_420;

    if (Rval != DSP_ERR_NONE) {
        AmbaLL_LogUInt5("HL_FillVprocExtMemInYuvCap Error[0x%x][%u]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

static inline UINT32 HL_FillVprocExtMemInProcRawCap(cmd_vproc_set_ext_mem_t *pVprocExtMem,
                                                    const CTX_DATACAP_INFO_s *pDataCapInfo,
                                                    const UINT8 IsSetPinExtMem,
                                                    const UINT16 ViewZoneId,
                                                    const UINT8 MemType,
                                                    const UINT8 OverFlowCtrl)
{
    UINT32 Rval = DSP_ERR_NONE, ErrLine = 0U;
    ULONG ULAddr;
    AMBA_DSP_BUF_s CalcExtBuf = {0U};
    const AMBA_DSP_DATACAP_CFG_s *pDataCapCfg = &pDataCapInfo->Cfg;

    (void)OverFlowCtrl;
    (void)IsSetPinExtMem;
    pVprocExtMem->memory_type = MemType;
    pVprocExtMem->allocation_type = (pDataCapCfg->AllocType == ALLOC_EXTERNAL_DISTINCT)? VIN_EXT_MEM_ALLOC_TYPE_DISTINCT: VIN_EXT_MEM_ALLOC_TYPE_CONTIGUOUS;
    /* 0 = first time, need new, 1=append,
     * Need to check with target vin/vproc every time before capture
     * Every mem type need to do new once, then append until vin/vproc restart
     * vin/vproc reset it's value at it's start */

    /* For vdo thm case / capture main case
     * If doing p60+p30 together, wait vcap for vin0 will be delayed by vin1
     * since overflow_cntl=1 need accurate control on the frame
     * so we use overflow_cntl=0 and allocation_mode always new to do it
     * */
    pVprocExtMem->allocation_mode = VIN_EXT_MEM_MODE_NEW;
    pVprocExtMem->overflow_cntl = VIN_EXT_MEM_CTRL_CIRCULAR;
    pVprocExtMem->num_frm_buf = pDataCapCfg->BufNum;
    if (pVprocExtMem->num_frm_buf > EXT_MEM_VPROC_MAX_NUM) {
        AmbaLL_LogUInt5("Only %d ExtMem supported", EXT_MEM_VPROC_MAX_NUM, 0U, 0U, 0U, 0U);
    }
    Rval = HL_StillCaptureCalcExtBufImpl(ViewZoneId, DSP_DATACAP_CALTYPE_PROCRAW, &CalcExtBuf);
    DSP_FillErrline(Rval, &ErrLine, __LINE__);
    pVprocExtMem->buf_pitch = CalcExtBuf.Pitch;
    pVprocExtMem->buf_width = CalcExtBuf.Pitch;
    pVprocExtMem->buf_height= CalcExtBuf.Window.Height;

    if (Rval == DSP_ERR_NONE) {
        if (pDataCapCfg->AllocType == ALLOC_EXTERNAL_DISTINCT) {
            dsp_osal_typecast(&ULAddr, &pDataCapCfg->pBufTbl);
            Rval = dsp_osal_virt2cli(ULAddr, &pVprocExtMem->buf_addr);
            DSP_FillErrline(Rval, &ErrLine, __LINE__);
        } else {
            Rval = dsp_osal_virt2cli(pDataCapCfg->DataBuf.BaseAddr, &pVprocExtMem->buf_addr);
            DSP_FillErrline(Rval, &ErrLine, __LINE__);
        }
    }
    pVprocExtMem->img_width = 0U;//pDataCapCfg->DataBuf.Window.Width;
    pVprocExtMem->img_height = 0U;//pDataCapCfg->DataBuf.Window.Height;
    pVprocExtMem->chroma_format = DSP_YUV_MONO;

    if (Rval != DSP_ERR_NONE) {
        AmbaLL_LogUInt5("HL_FillVprocExtMemInProcRawCap Error[0x%x][%u]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

static inline UINT32 HL_FillVprocExtAuxMemInYuvCap(cmd_vproc_set_ext_mem_t *pVprocExtMem,
                                                   const CTX_DATACAP_INFO_s *pDataCapInfo,
                                                   const UINT8 IsSetPinExtMem,
                                                   const UINT16 YuvStrmId,
                                                   const UINT8 MemType,
                                                   const UINT8 OverFlowCtrl)
{
    UINT32 Rval = DSP_ERR_NONE, ErrLine = 0U;
    ULONG ULAddr;
    AMBA_DSP_BUF_s CalcExtBuf = {0U};
    const AMBA_DSP_DATACAP_CFG_s *pDataCapCfg = &pDataCapInfo->Cfg;
    UINT16 i;
    UINT32 *pBufTblAddr = NULL;
    UINT32 PhysAddr;

    pVprocExtMem->memory_type = MemType;
    pVprocExtMem->allocation_type = (pDataCapCfg->AllocType == ALLOC_EXTERNAL_DISTINCT)? VIN_EXT_MEM_ALLOC_TYPE_DISTINCT: VIN_EXT_MEM_ALLOC_TYPE_CONTIGUOUS;
    /* 0 = first time, need new, 1=append,
     * Need to check with target vin/vproc every time before capture
     * Every mem type need to do new once, then append until vin/vproc restart
     * vin/vproc reset it's value at it's start */
    /* For vdo thm case / capture main case
     * If doing p60+p30 together, wait vcap for vin0 will be delayed by vin1
     * since overflow_cntl=1 need accurate control on the frame
     * so we use overflow_cntl=0 and allocation_mode always new to do it
     * */
//    pVprocExtMem->allocation_mode = (IsSetPinExtMem == 0U) ? VIN_EXT_MEM_MODE_NEW : VIN_EXT_MEM_MODE_APPEND;
//    pVprocExtMem->overflow_cntl = OverFlowCtrl;
    (void)OverFlowCtrl;
    (void)IsSetPinExtMem;
    pVprocExtMem->allocation_mode = VIN_EXT_MEM_MODE_NEW;
    pVprocExtMem->overflow_cntl = VIN_EXT_MEM_CTRL_CIRCULAR;
    pVprocExtMem->num_frm_buf = pDataCapCfg->BufNum;
    if (pVprocExtMem->num_frm_buf > EXT_MEM_VPROC_MAX_NUM) {
        AmbaLL_LogUInt5("Only %d ExtMem supported", EXT_MEM_VPROC_MAX_NUM, 0U, 0U, 0U, 0U);
    }
    // ME1 buf from AuxBuf
    Rval = HL_StillCaptureCalcExtBufImpl(YuvStrmId, DSP_DATACAP_CALTYPE_ME1, &CalcExtBuf);
    DSP_FillErrline(Rval, &ErrLine, __LINE__);
    pVprocExtMem->buf_pitch = CalcExtBuf.Pitch;
    pVprocExtMem->buf_width = CalcExtBuf.Window.Width;
    pVprocExtMem->buf_height= CalcExtBuf.Window.Height;

    if (Rval == DSP_ERR_NONE) {
        if (pDataCapCfg->AllocType == ALLOC_EXTERNAL_DISTINCT) {
            HL_GetPointerToDspExtYuvStrmAuxBufArray(YuvStrmId, &pBufTblAddr);
            for (i = 0; i < pDataCapCfg->BufNum; i++) {
                if (Rval == DSP_ERR_NONE) {
                    Rval = dsp_osal_virt2cli(pDataCapInfo->AuxBufTbl[i], &PhysAddr);
                    DSP_FillErrline(Rval, &ErrLine, __LINE__);
                    pBufTblAddr[i] = PhysAddr;
                }
            }
            dsp_osal_typecast(&ULAddr, &pBufTblAddr);
            if (Rval == DSP_ERR_NONE) {
                Rval = dsp_osal_virt2cli(ULAddr, &pVprocExtMem->buf_addr);
                DSP_FillErrline(Rval, &ErrLine, __LINE__);
            }
        } else {
                Rval = dsp_osal_virt2cli(pDataCapCfg->AuxDataBuf.BaseAddr, &pVprocExtMem->buf_addr);
                DSP_FillErrline(Rval, &ErrLine, __LINE__);
        }
    }

    pVprocExtMem->img_width = pDataCapCfg->AuxDataBuf.Window.Width >> DSP_VIDEO_ME1_DATA_SHIFT;
    pVprocExtMem->img_height = pDataCapCfg->AuxDataBuf.Window.Height >> DSP_VIDEO_ME1_DATA_SHIFT;
    pVprocExtMem->chroma_format = DSP_YUV_MONO;

    // ME0 buf from AuxBuf
    if (Rval == DSP_ERR_NONE) {
        Rval = HL_StillCaptureCalcExtBufImpl(YuvStrmId, DSP_DATACAP_CALTYPE_ME0, &CalcExtBuf);
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
    }
    pVprocExtMem->aux_pitch = CalcExtBuf.Pitch;
    pVprocExtMem->aux_width = CalcExtBuf.Window.Width;
    pVprocExtMem->aux_height= CalcExtBuf.Window.Height;
    pVprocExtMem->aux_img_ofs_x = 0U;
    pVprocExtMem->aux_img_ofs_y = 0U;
    pVprocExtMem->aux_img_width = pDataCapCfg->AuxDataBuf.Window.Width >> DSP_VIDEO_ME0_DATA_SHIFT;
    pVprocExtMem->aux_img_height = pDataCapCfg->AuxDataBuf.Window.Height >> DSP_VIDEO_ME0_DATA_SHIFT;

    if (Rval != DSP_ERR_NONE) {
        AmbaLL_LogUInt5("HL_FillVprocExtAuxMemInYuvCap Error[0x%x][%u]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

static inline void HL_GetEfctPassThroughOnYuvStrm(const CTX_YUV_STRM_INFO_s *pYuvStrmInfo, UINT8 *pPassThrough)
{
    if (pYuvStrmInfo->Layout.NumChan == 1U) {
        *pPassThrough = 1U;
    } else {
        if ((pYuvStrmInfo->Layout.EffectBufDesc.CpyBufId != DSP_EFCT_INVALID_IDX) ||
            (pYuvStrmInfo->Layout.EffectBufDesc.Y2YBufId != DSP_EFCT_INVALID_IDX)) {
            *pPassThrough = 0U;
        } else {
            *pPassThrough = 1U;
        }
    }
}

typedef struct {
    UINT16 VinId;
    UINT16 PhysVinId;
    UINT32 EventMask;
    UINT8  Active;
} YUVCAP_TEMP_INFO_s;
static inline UINT32 HL_ComposeYuvCapInfo(const CTX_YUV_STRM_INFO_s *pYuvStrmInfo,
                                          UINT16 YuvStrmId,
                                          UINT8 PassThrough,
                                          const CTX_DATACAP_INFO_s *pDataCapInfo,
                                          YUVCAP_TEMP_INFO_s *pYuvCapTempInfo,
                                          UINT16 *pPrevPinIdx)
{
    UINT32 Rval = DSP_ERR_NONE, ErrLine = 0U;
    UINT32 AttachChan;
    UINT16 j, k, ViewZoneId;
    CTX_VPROC_INFO_s VprocInfo;
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;
    const AMBA_DSP_DATACAP_CFG_s *pDataCapCfg = &pDataCapInfo->Cfg;

    for (j=0U; j<pYuvStrmInfo->Layout.NumChan; j++) {
        AttachChan = 0U;

        ViewZoneId = pYuvStrmInfo->Layout.ChanCfg[j].ViewZoneId;
        HL_GetVprocInfo(HL_MTX_OPT_ALL, ViewZoneId, &VprocInfo);

        for (k=0U; k<DSP_VPROC_PIN_NUM; k++) {
            if (1U == DSP_GetBit(VprocInfo.PinUsage[k], YuvStrmId, 1U)) {
                *pPrevPinIdx = k;
                break;
            }
        }
        if (PassThrough == 1U) {
            AttachChan = 1U;
        } else {
            /*
             * In Effect Case, we shall EnableExtMemBit for following Channel
             *   - The one create final output buffer, who physically use ExtMem
             *   - The one is last channel, who carry information to next stage
             * if Temp buffer create channel is on, dsp will request ExtMem as well
             */
            /* Only one blend job per channel */
            if ((pYuvStrmInfo->Layout.BlendJobDesc[j][0U].NeedY2YBuf == BLD_JOB_Y2Y_BUF_NEW) ||
                (pYuvStrmInfo->Layout.BlendJobDesc[j][0U].NeedY2YBuf == BLD_JOB_Y2Y_BUF_NEW_INT) ||
                (pYuvStrmInfo->Layout.BlendJobDesc[j][0U].NeedY2YBuf == BLD_JOB_Y2Y_BUF_NEW_INT_DUMMY)) {
                // channel create temp buffer, DONOT raise ExtMem flag
            } else {
                AttachChan = 1U;
            }
        }
        if (AttachChan == 1U) {
            UINT32 KeepMask = 0U, SingleMask = 0U;

            if (pYuvCapTempInfo[ViewZoneId].Active == 0U) {
                HL_GetViewZoneInfoPtr(ViewZoneId, &ViewZoneInfo);
                DSP_Bit2U16Idx(ViewZoneInfo->SourceVin, &(pYuvCapTempInfo[ViewZoneId].VinId));
                HL_GetViewZonePhysVinId(ViewZoneInfo, &(pYuvCapTempInfo[ViewZoneId].PhysVinId));
                Rval = HL_GetVinAttachEventDefaultMask(pYuvCapTempInfo[ViewZoneId].VinId,
                                                      &(pYuvCapTempInfo[ViewZoneId].EventMask));
                DSP_FillErrline(Rval, &ErrLine, __LINE__);
                pYuvCapTempInfo[ViewZoneId].Active = 1U;
            }

            if (pDataCapInfo->CountDown > 0U) {
                if (pDataCapCfg->CapDataType == DSP_DATACAP_TYPE_SYNCED_YUV) {
                    DSP_SetBit(&SingleMask, DSP_BIT_POS_ENC_START);
                }
                // For Me ext_mem
                if (pDataCapCfg->CapDataType == DSP_DATACAP_TYPE_VIDEO_YUV) {
                    DSP_SetBit(&KeepMask, HL_VprocPinVinAttachMeBitMap[*pPrevPinIdx]);
                }
                if (pDataCapCfg->CapDataType == DSP_DATACAP_TYPE_COMP_RAW) {
                    DSP_SetBit(&KeepMask, DSP_BIT_POS_EXT_COMP_RAW);
                }
                DSP_SetBit(&KeepMask, HL_VprocPinVinAttachBitMap[*pPrevPinIdx]);
                pYuvCapTempInfo[ViewZoneId].EventMask |= SingleMask;
                pYuvCapTempInfo[ViewZoneId].EventMask |= KeepMask;
            }

            // Save KeepMask to ViewZoneInfo for HL_CountDownVinAttachEvent
            HL_GetViewZoneInfoLock(ViewZoneId, &ViewZoneInfo);
            ViewZoneInfo->AttachEventCountDown[*pPrevPinIdx] = pDataCapInfo->CountDown;
            ViewZoneInfo->ExtraEventMask[*pPrevPinIdx] = KeepMask;
            HL_GetViewZoneInfoUnLock(ViewZoneId);
        }
    }

    if (Rval != DSP_ERR_NONE) {
        AmbaLL_LogUInt5("HL_ComposeYuvCapInfo Error[0x%x][%u]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

static inline UINT32 HL_ComposeYuvCapInfoOnProcRaw(UINT16 ViewZoneId,
                                                   const CTX_DATACAP_INFO_s *pDataCapInfo,
                                                   YUVCAP_TEMP_INFO_s *pYuvCapTempInfo)
{
    UINT32 Rval = DSP_ERR_NONE, ErrLine = 0U;
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;
    UINT32 KeepMask = 0U;
    UINT8 PinIdx = DSP_PROC_RAW_PIN_ID;

    if (pYuvCapTempInfo[ViewZoneId].Active == 0U) {
        HL_GetViewZoneInfoPtr(ViewZoneId, &ViewZoneInfo);
        DSP_Bit2U16Idx(ViewZoneInfo->SourceVin, &(pYuvCapTempInfo[ViewZoneId].VinId));
        HL_GetViewZonePhysVinId(ViewZoneInfo, &(pYuvCapTempInfo[ViewZoneId].PhysVinId));
        Rval = HL_GetVinAttachEventDefaultMask(pYuvCapTempInfo[ViewZoneId].VinId,
                                              &(pYuvCapTempInfo[ViewZoneId].EventMask));
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
        pYuvCapTempInfo[ViewZoneId].Active = 1U;
    }

    if (pDataCapInfo->CountDown > 0U) {
        DSP_SetBit(&KeepMask, DSP_BIT_POS_EXT_COMP_RAW);
        pYuvCapTempInfo[ViewZoneId].EventMask |= KeepMask;
    }

    // Save KeepMask to ViewZoneInfo for HL_CountDownVinAttachEvent
    HL_GetViewZoneInfoLock(ViewZoneId, &ViewZoneInfo);
    ViewZoneInfo->AttachEventCountDown[PinIdx] = pDataCapInfo->CountDown;
    ViewZoneInfo->ExtraEventMask[PinIdx] = KeepMask;
    HL_GetViewZoneInfoUnLock(ViewZoneId);

    if (Rval != DSP_ERR_NONE) {
        AmbaLL_LogUInt5("HL_ComposeYuvCapInfoOnProcRaw Error[0x%x][%u]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}


static inline void HL_MergeYuvCapInfoEventMask(YUVCAP_TEMP_INFO_s *pYuvCapTempInfo)
{
    UINT16 i, j;
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;

    for (i=0U; i<AMBA_DSP_MAX_VIEWZONE_NUM; i++) {
        if (pYuvCapTempInfo[i].Active == 1U) {
            HL_GetViewZoneInfoPtr(i, &ViewZoneInfo);

            for (j=0U; j<DSP_VPROC_PIN_NUM; j++) {
                if (ViewZoneInfo->AttachEventCountDown[j] > 0U) {
                    pYuvCapTempInfo[i].EventMask |= ViewZoneInfo->ExtraEventMask[j];
                }
            }
        }
    }
}

static inline UINT32 HL_StlCapYuvImplPreproc(const UINT16 NumCapInstance,
                                           const UINT16 *pCapInstance,
                                           YUVCAP_TEMP_INFO_s *YuvCapTempInfo)
{
    UINT32 Rval = DSP_ERR_NONE, ErrLine = 0U;
    UINT8 PurePassThrough;
    UINT8 WriteMode = AMBA_DSP_CMD_VDSP_NORMAL_WRITE;
    UINT8 BufMemType = 0U, AuxBufMemType = 0U;
    UINT16 i, ViewZoneId;
    UINT16 YuvStrmId = 0U, PrevPinIdx = 0U, PhysVinId = 0U;
    CTX_DATACAP_INFO_s DataCapInfo;
    CTX_YUV_STRM_INFO_s *YuvStrmInfo = HL_CtxYuvStrmInfoPtr;
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;
    UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
    cmd_vproc_set_ext_mem_t *VprocExtMem = HL_DefCtxCmdBufPtrVpcExtMem;
    const AMBA_DSP_DATACAP_CFG_s *pDataCapCfg;

    for (i=0U; i<NumCapInstance; i++) {
        HL_GetDataCapInfo(HL_MTX_OPT_ALL, pCapInstance[i], &DataCapInfo);
        pDataCapCfg = &DataCapInfo.Cfg;

//        AmbaLL_LogUInt5("[HL_StillCaptureYuvImpl] [%u] NumCapInstance:%u CapDataType:%u Idx:%u",
//                __LINE__, i, pDataCapCfg->CapDataType, pDataCapCfg->Index, 0U);

        /* Timing sanity check */
        if ((DataCapInfo.Status != DATA_CAP_STATUS_UPDATED) &&
            (DataCapInfo.Status != DATA_CAP_STATUS_2_RUN) &&
            (DataCapInfo.Status != DATA_CAP_STATUS_2_STOP)) {
            continue;
        }
        /* Input sanity check */
        if ((pDataCapCfg->CapDataType != DSP_DATACAP_TYPE_YUV) &&
            (pDataCapCfg->CapDataType != DSP_DATACAP_TYPE_SYNCED_YUV) &&
            (pDataCapCfg->CapDataType != DSP_DATACAP_TYPE_VIDEO_YUV) &&
            (pDataCapCfg->CapDataType != DSP_DATACAP_TYPE_COMP_RAW)) {
            continue;
        }

        /* Parse all viewzone info (VinID/EventMask) and keep it in YuvCapTempInfo */
        if ((pDataCapCfg->CapDataType == DSP_DATACAP_TYPE_YUV) ||
            (pDataCapCfg->CapDataType == DSP_DATACAP_TYPE_SYNCED_YUV) ||
            (pDataCapCfg->CapDataType == DSP_DATACAP_TYPE_VIDEO_YUV)) {
            HL_GetYuvStrmIdOnCapCfg(pDataCapCfg, &YuvStrmId);
            HL_GetYuvStrmInfoPtr(YuvStrmId, &YuvStrmInfo);

//FIXME, First ViewZone when Effect?
            ViewZoneId = YuvStrmInfo->Layout.ChanCfg[0U].ViewZoneId;
            HL_GetEfctPassThroughOnYuvStrm(YuvStrmInfo, &PurePassThrough);
            Rval = HL_ComposeYuvCapInfo(YuvStrmInfo,
                                       YuvStrmId,
                                       PurePassThrough,
                                       &DataCapInfo,
                                       &YuvCapTempInfo[0U],
                                       &PrevPinIdx);
            DSP_FillErrline(Rval, &ErrLine, __LINE__);
        } else {
            ViewZoneId = pDataCapCfg->Index;
            Rval = HL_ComposeYuvCapInfoOnProcRaw(ViewZoneId,
                                                &DataCapInfo,
                                                &YuvCapTempInfo[0U]);
            DSP_FillErrline(Rval, &ErrLine, __LINE__);
        }

        HL_GetViewZoneInfoPtr(ViewZoneId, &ViewZoneInfo);
        HL_GetViewZonePhysVinId(ViewZoneInfo, &PhysVinId);
        WriteMode = HL_GetVin2CmdNormalWrite(PhysVinId);
        if (pDataCapCfg->CapDataType == DSP_DATACAP_TYPE_COMP_RAW) {
            BufMemType = (UINT8)VPROC_EXT_MEM_TYPE_COMP_RAW;
        } else {
            BufMemType = (UINT8)HL_VprocPinVprocMemTypePoolMap[PrevPinIdx];
        }

        if (DataCapInfo.Status != DATA_CAP_STATUS_2_STOP) {
            UINT8 OverFlowCtrl = HL_StillCaptureChkOverFlwCtrl(NumCapInstance, pDataCapCfg);
            UINT8 IsSetPinExtMem = HL_GetVprocExtMemStatus(pDataCapCfg);

            HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
            dsp_osal_typecast(&VprocExtMem, &CmdBufferAddr);
            VprocExtMem->channel_id = (UINT8)ViewZoneId;
            // Set ExtMem
            if ((pDataCapCfg->CapDataType == DSP_DATACAP_TYPE_YUV) ||
                (pDataCapCfg->CapDataType == DSP_DATACAP_TYPE_SYNCED_YUV) ||
                (pDataCapCfg->CapDataType == DSP_DATACAP_TYPE_VIDEO_YUV)) {
                Rval = HL_FillVprocExtMemInYuvCap(VprocExtMem, &DataCapInfo,
                                                 IsSetPinExtMem,
                                                 YuvStrmId,
                                                 BufMemType,
                                                 OverFlowCtrl);
                DSP_FillErrline(Rval, &ErrLine, __LINE__);
            } else {
                Rval = HL_FillVprocExtMemInProcRawCap(VprocExtMem, &DataCapInfo,
                                                     IsSetPinExtMem,
                                                     ViewZoneId,
                                                     BufMemType,
                                                     OverFlowCtrl);
                DSP_FillErrline(Rval, &ErrLine, __LINE__);

            }
            Rval = AmbaHL_CmdVprocSetExtMem(WriteMode, VprocExtMem);
            DSP_FillErrline(Rval, &ErrLine, __LINE__);
            HL_RelCmdBuffer(CmdBufferId);

            // Set ME data
            if (pDataCapCfg->CapDataType == DSP_DATACAP_TYPE_VIDEO_YUV) {
                HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
                dsp_osal_typecast(&VprocExtMem, &CmdBufferAddr);
                AuxBufMemType = (UINT8)HL_VprocPinVprocMemMeTypeMap[PrevPinIdx];
                VprocExtMem->channel_id = (UINT8)ViewZoneId;

                Rval = HL_FillVprocExtAuxMemInYuvCap(VprocExtMem, &DataCapInfo,
                                                    IsSetPinExtMem,
                                                    YuvStrmId,
                                                    AuxBufMemType,
                                                    OverFlowCtrl);
                DSP_FillErrline(Rval, &ErrLine, __LINE__);
                Rval = AmbaHL_CmdVprocSetExtMem(WriteMode, VprocExtMem);
                DSP_FillErrline(Rval, &ErrLine, __LINE__);
                HL_RelCmdBuffer(CmdBufferId);
            }
            HL_GetDataCapInfo(HL_MTX_OPT_GET, pCapInstance[i], &DataCapInfo);
            DataCapInfo.BufMemType = BufMemType;
            DataCapInfo.AuxBufMemType = AuxBufMemType;
            DataCapInfo.Status = DATA_CAP_STATUS_RUNNING;
            HL_SetDataCapInfo(HL_MTX_OPT_SET, pCapInstance[i], &DataCapInfo);

            /* set SetNewExtMem in vproc info */
            if (IsSetPinExtMem == 0U) {
                HL_SetVprocExtMemStatus(pDataCapCfg, 1U);
            }
        }
    }

    if (Rval != DSP_ERR_NONE) {
        AmbaLL_LogUInt5("HL_StlCapYuvImplPreproc Error[0x%x][%u]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

UINT32 HL_StillCaptureYuvImpl(const UINT16 NumCapInstance, const UINT16 *pCapInstance)
{
    UINT32 Rval = DSP_ERR_NONE, ErrLine = 0U;
    UINT8 WriteMode = AMBA_DSP_CMD_VDSP_NORMAL_WRITE;
    UINT16 i, j, VinExist = 0U;
    UINT16 WaitEventVinBit = 0U, WaitEventVinNum = 0U;
    UINT16 WaitEventVinIdx[AMBA_DSP_MAX_VIN_NUM] = {0U};
    CTX_VIN_INFO_s VinInfo;
    YUVCAP_TEMP_INFO_s YuvCapTempInfo[AMBA_DSP_MAX_VIEWZONE_NUM];
    UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
    cmd_vin_attach_event_to_raw_t *VinAttachEvent = HL_DefCtxCmdBufPtrVinAttRaw;

    /* Keep info for merging VinAttachEventToRaw command */
    Rval = dsp_osal_memset(&YuvCapTempInfo[0], 0, sizeof(YUVCAP_TEMP_INFO_s)*AMBA_DSP_MAX_VIEWZONE_NUM);
    DSP_FillErrline(Rval, &ErrLine, __LINE__);

    Rval = HL_StlCapYuvImplPreproc(NumCapInstance, pCapInstance, YuvCapTempInfo);
    DSP_FillErrline(Rval, &ErrLine, __LINE__);

    // Merge the running VinAttachEventMask
    HL_MergeYuvCapInfoEventMask(&YuvCapTempInfo[0U]);

    if (Rval == DSP_ERR_NONE) {
        // Trigger YuvCap
        for (i = 0U; i < AMBA_DSP_MAX_VIEWZONE_NUM; i++) {
            if (YuvCapTempInfo[i].Active == 1U) {
                HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
                dsp_osal_typecast(&VinAttachEvent, &CmdBufferAddr);

                VinAttachEvent->vin_id = (UINT8)YuvCapTempInfo[i].VinId;
                /* For waiting VCAP event */
                if (YuvCapTempInfo[i].PhysVinId != VIN_IDX_INVALID) {
                    DSP_SetU16Bit(&WaitEventVinBit, (UINT32)YuvCapTempInfo[i].PhysVinId);
                }
                VinAttachEvent->ch_id = (UINT8)0U; //TBD
                VinAttachEvent->repeat_cnt = 0x10001;
                VinAttachEvent->time_duration_bet_rpts = 0U;
                VinAttachEvent->is_indep_fov_control = (UINT8)1U;
                VinAttachEvent->event_mask = YuvCapTempInfo[i].EventMask;
                DSP_SetBit(&VinAttachEvent->fov_mask_valid, HL_GetViewZoneLocalIndexOnVin(i));

                // Merge the fov that has the same event mask
                for (j = (i+1U); j < AMBA_DSP_MAX_VIEWZONE_NUM; j++) {
                    if ((YuvCapTempInfo[j].Active == 1U) &&
                        (YuvCapTempInfo[j].VinId == YuvCapTempInfo[i].VinId) &&
                        (YuvCapTempInfo[j].EventMask == YuvCapTempInfo[i].EventMask)) {

                        DSP_SetBit(&VinAttachEvent->fov_mask_valid, HL_GetViewZoneLocalIndexOnVin(j));
                        YuvCapTempInfo[j].Active = 0U; // Be merged
                    }
                }
                WriteMode = HL_GetVin2CmdNormalWrite(YuvCapTempInfo[i].PhysVinId);
                Rval = AmbaHL_CmdVinAttachEventToRaw(WriteMode, VinAttachEvent);
                DSP_FillErrline(Rval, &ErrLine, __LINE__);

                HL_GetVinInfo(HL_MTX_OPT_GET, YuvCapTempInfo[i].VinId, &VinInfo);
                VinInfo.AttachEventCountDownCheck |= VinAttachEvent->fov_mask_valid;
                HL_SetVinInfo(HL_MTX_OPT_SET, YuvCapTempInfo[i].VinId, &VinInfo);
                HL_RelCmdBuffer(CmdBufferId);
            }
        }

    }

    if (Rval == DSP_ERR_NONE) {
        /* We need to wait for a VCAP event interrupt to make sure the VinAttachEvent cmd is sent before stoping YUV capture */
        DSP_Bit2U16Cnt(WaitEventVinBit, &WaitEventVinNum);
        j = 0U;
        for (i=0U; i<AMBA_DSP_MAX_VIN_NUM; i++) {
            if (1U == DSP_GetBit(WaitEventVinBit, i, 1U)) {
                WaitEventVinIdx[j] = i;
                j++;
            }
        }
        Rval = DSP_WaitVcapEvent(WaitEventVinNum, WaitEventVinIdx, 1, WAIT_FLAG_TIMEOUT);
        DSP_FillErrline(Rval, &ErrLine, __LINE__);

        // Countdown VinAttachEvent's capture number
        for (i = 0U; i < AMBA_DSP_MAX_VIN_NUM; i++) {
            VinExist = 0U;
            HL_GetVinExistence(i, &VinExist);
            if (VinExist == 1U) {
                Rval = HL_CountDownVinAttachEvent(i);
                DSP_FillErrline(Rval, &ErrLine, __LINE__);
            }
        }
    }

    if (Rval != DSP_ERR_NONE) {
        AmbaLL_LogUInt5("HL_StillCaptureYuvImpl Error[0x%x][%u]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;

}

static inline void HL_CalcProcRawBuf(UINT16 ViewZoneId, AMBA_DSP_BUF_s *pExtBuf)
{
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;
    UINT16 VinId = 0U;

    // Based on VprocSetup.W0/H0 + C2Y burst
    HL_GetResourcePtr(&Resource);
    HL_GetViewZoneInfoPtr(ViewZoneId, &ViewZoneInfo);

//FIXME, filterOut Dec/Recon source

    DSP_Bit2U16Idx(ViewZoneInfo->SourceVin, &VinId);
    pExtBuf->Window.Width = Resource->MaxVinVirtChanWidth[VinId][0U];
    if (ViewZoneInfo->HdrBlendNumMinusOne > 0U) {
        pExtBuf->Window.Height = ViewZoneInfo->HdrBlendHeight;
    } else {
        pExtBuf->Window.Height = Resource->MaxVinVirtChanHeight[VinId][0U];
    }

    pExtBuf->Pitch = pExtBuf->Window.Width*2U;
    pExtBuf->Pitch = ALIGN_NUM16(pExtBuf->Pitch, (UINT16)DSP_BUF_ALIGNMENT);
}

UINT32 HL_StillCaptureCalcExtBufImpl(const UINT16 StreamIdx, const UINT16 CalType, AMBA_DSP_BUF_s *pExtBuf)
{
    UINT32 Rval = DSP_ERR_NONE, ErrLine = 0U;
    CTX_YUV_STRM_INFO_s *YuvStrmInfo = HL_CtxYuvStrmInfoPtr;
    CTX_VPROC_INFO_s VprocInfo;
    UINT16 i, j, PrevPinIdx = 0U;
    UINT8 ExitILoop;

    /* DSP CtxInit */
    Rval = HL_CtxInit();
    DSP_FillErrline(Rval, &ErrLine, __LINE__);

    if (Rval == DSP_ERR_NONE) {
        if (CalType == DSP_DATACAP_CALTYPE_PROCRAW) {
            HL_CalcProcRawBuf(StreamIdx/*ViewZoneId*/, pExtBuf);
        } else {
            HL_GetYuvStrmInfoPtr(StreamIdx, &YuvStrmInfo);
            if (CalType == DSP_DATACAP_CALTYPE_LUMA) {
                ExitILoop = 0U;
                for (i=0U; i<YuvStrmInfo->Layout.NumChan; i++) {
                    HL_GetVprocInfo(HL_MTX_OPT_ALL, YuvStrmInfo->Layout.ChanCfg[i].ViewZoneId, &VprocInfo);
                    for (j=0U; j<DSP_VPROC_PIN_NUM; j++) {
                        if (1U == DSP_GetBit(VprocInfo.PinUsage[j], StreamIdx, 1U)) {
                            ExitILoop = 1U;
                            PrevPinIdx = j;
                            break;
                        }
                    }
                    if (ExitILoop == 1U) {
                        break;
                    }
                }

                pExtBuf->Pitch = ALIGN_NUM16(YuvStrmInfo->Width, (UINT16)DSP_BUF_ALIGNMENT);
                pExtBuf->Window.Width = YuvStrmInfo->Width;

                /* Per ChenHan, if we prefer to have Mctf/Mcts Yuv, we need Align32(H+16) buffer height, it's Mctf HW limitation */
                if (PrevPinIdx == DSP_VPROC_PIN_MAIN) {
                    if (YuvStrmInfo->MaxChanNum > 1U) {
                        /* Mctf ouput buffer need to have extra padding for following AVC/HEVC encode */
                        /* since AVC need 16 align, so here we set padding number as 16 */
                        UINT16 BufHeight = YuvStrmInfo->Height + DSP_POSTP_MAIN_PADDING_FOR_ENC;
                        pExtBuf->Window.Height = ALIGN_NUM16(BufHeight + 16U, 32U);
                    } else {
                        pExtBuf->Window.Height = ALIGN_NUM16(YuvStrmInfo->Height + 16U, 32U);
                    }
                } else if (1U == DSP_GetU16Bit(YuvStrmInfo->Purpose, DSP_LV_STRM_PURPOSE_ENCODE_IDX, 1U)) {
                    pExtBuf->Window.Height = ALIGN_NUM16(YuvStrmInfo->Height, 16U);
                } else {
                    pExtBuf->Window.Height = ALIGN_NUM16(YuvStrmInfo->Height, 2U);
                }
            } else if (CalType == DSP_DATACAP_CALTYPE_ME1) {
                pExtBuf->Pitch = ALIGN_NUM16((YuvStrmInfo->Width >> DSP_VIDEO_ME1_DATA_SHIFT), (UINT16)DSP_BUF_ALIGNMENT);
                pExtBuf->Window.Width = ALIGN_NUM16((YuvStrmInfo->Width >> DSP_VIDEO_ME1_DATA_SHIFT), 8U);
                pExtBuf->Window.Height = ALIGN_NUM16((YuvStrmInfo->Height >> DSP_VIDEO_ME1_DATA_SHIFT), 8U);
            } else if (CalType == DSP_DATACAP_CALTYPE_ME0) {
                pExtBuf->Pitch = ALIGN_NUM16((YuvStrmInfo->Width >> DSP_VIDEO_ME0_DATA_SHIFT), (UINT16)DSP_BUF_ALIGNMENT);
                pExtBuf->Window.Width = ALIGN_NUM16((YuvStrmInfo->Width >> DSP_VIDEO_ME0_DATA_SHIFT), 4U);
                pExtBuf->Window.Height = ALIGN_NUM16((YuvStrmInfo->Height >> DSP_VIDEO_ME0_DATA_SHIFT), 4U);
            } else {
                Rval = DSP_ERR_0001;
                DSP_FillErrline(Rval, &ErrLine, __LINE__);
            }
        }

        // Unused
        pExtBuf->BaseAddr = 0U;
        pExtBuf->Window.OffsetX = 0U;
        pExtBuf->Window.OffsetY = 0U;
    }

    if (Rval != DSP_ERR_NONE) {
        AmbaLL_LogUInt5("HL_StillCaptureCalcExtBufImpl Error[0x%x][%u]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

static inline UINT32 HL_StillCaptureNumCheck(const UINT16 DataCapIdx, const UINT32 CapNum)
{
#ifndef SUPPORT_DSP_EXT_PIN_BUF
    UINT32 Rval = DSP_ERR_0001;
    CTX_DATACAP_INFO_s DataCapInfo = {0U};
    CTX_STREAM_INFO_s StrmInfo = {0U};
    CTX_YUV_STRM_INFO_s *YuvStrmInfo = HL_CtxYuvStrmInfoPtr;
    CTX_VPROC_INFO_s VprocInfo = {0U};
    const AMBA_DSP_DATACAP_CFG_s *pDataCapCfg;
    UINT16 YuvStrmId, StrmId, i, PrevPinIdx = DSP_VPROC_PIN_MAIN;
    UINT8 IsMemPurpose, IsEncPurpose, IsVoutPurpose;

    HL_GetDataCapInfo(HL_MTX_OPT_ALL, DataCapIdx, &DataCapInfo);
    pDataCapCfg = &DataCapInfo.Cfg;

    if (CapNum == DSP_DATACAP_INFINITE_CAP_NUM) {
        /* Current uCode haves 2 ExtFbNum limitation */
        /* For performance reasons, only the following conditions support infinite yuv capute */
        /* 1. EncPurpose and doing Timelapse encoder */
        /* 2. MemPurpose YuvStream */

        if (pDataCapCfg->CapDataType == DSP_DATACAP_TYPE_YUV) {
            HL_GetYuvStrmInfoPtr(pDataCapCfg->Index, &YuvStrmInfo);
            YuvStrmId = pDataCapCfg->Index;

            IsMemPurpose = (1U == DSP_GetU16Bit(YuvStrmInfo->Purpose, DSP_LV_STRM_PURPOSE_MEMORY_IDX, 1U))? 1U: 0U;
            IsEncPurpose =(1U == DSP_GetU16Bit(YuvStrmInfo->Purpose, DSP_LV_STRM_PURPOSE_ENCODE_IDX, 1U))? 1U: 0U;
            IsVoutPurpose = (1U == DSP_GetU16Bit(YuvStrmInfo->Purpose, DSP_LV_STRM_PURPOSE_VOUT_IDX, 1U))? 1U: 0U;

            if (IsEncPurpose == 1U) {
                DSP_Bit2U16Idx(YuvStrmInfo->DestEnc, &StrmId);
                HL_GetStrmInfo(HL_MTX_OPT_ALL, StrmId, &StrmInfo);

                if ((StrmInfo.VideoTimeLapseActive > 0U) &&
                    (StrmInfo.TimeLapseId != DSP_TIMELAPSE_IDX_INVALID)) {
                    HL_GetVprocInfo(HL_MTX_OPT_ALL, YuvStrmInfo->Layout.ChanCfg[0].ViewZoneId, &VprocInfo);
                    for (i=0U; i<DSP_VPROC_PIN_NUM; i++) {
                        if (1U == DSP_GetBit(VprocInfo.PinUsage[i], YuvStrmId, 1U)) {
                            PrevPinIdx = i;
                            break;
                        }
                    }
                    if (PrevPinIdx != DSP_VPROC_PIN_MAIN) {
                        // Not support Timelapse MainEncoder
                        Rval = DSP_ERR_NONE;
                    }
                }
            } else if ((IsMemPurpose == 1U) && (IsVoutPurpose == 0U)) {
                Rval = DSP_ERR_NONE;
            } else {
                Rval = DSP_ERR_0001;
            }
        } else if (pDataCapCfg->CapDataType == DSP_DATACAP_TYPE_VIDEO_YUV) {
            HL_GetStrmInfo(HL_MTX_OPT_ALL, pDataCapCfg->Index, &StrmInfo);
            HL_GetYuvStrmInfoPtr(StrmInfo.SourceYuvStrmId, &YuvStrmInfo);
            YuvStrmId = StrmInfo.SourceYuvStrmId;

            if ((StrmInfo.VideoTimeLapseActive > 0U) &&
                (StrmInfo.TimeLapseId != DSP_TIMELAPSE_IDX_INVALID)) {
                HL_GetVprocInfo(HL_MTX_OPT_ALL, YuvStrmInfo->Layout.ChanCfg[0].ViewZoneId, &VprocInfo);
                for (i=0U; i<DSP_VPROC_PIN_NUM; i++) {
                    if (1U == DSP_GetBit(VprocInfo.PinUsage[i], YuvStrmId, 1U)) {
                        PrevPinIdx = i;
                        break;
                    }
                }
                if (PrevPinIdx != DSP_VPROC_PIN_MAIN) {
                    // Not support Timelapse MainEncoder
                    Rval = DSP_ERR_NONE;
                }
            }
        } else {
            Rval = DSP_ERR_0001;
        }
    } else if (CapNum > 1U) {
        Rval = DSP_ERR_0001;
    } else {
        Rval = DSP_ERR_NONE;
    }
#else
    /* uCode support UserDefined ExtFbNum */
    UINT32 Rval = DSP_ERR_0001;
    CTX_DATACAP_INFO_s DataCapInfo = {0U};
    const AMBA_DSP_DATACAP_CFG_s *pDataCapCfg;

    HL_GetDataCapInfo(HL_MTX_OPT_ALL, DataCapIdx, &DataCapInfo);
    pDataCapCfg = &DataCapInfo.Cfg;

    if (CapNum == DSP_DATACAP_INFINITE_CAP_NUM) {
        if (pDataCapCfg->CapDataType == DSP_DATACAP_TYPE_YUV) {
            Rval = DSP_ERR_NONE;
        } else if (pDataCapCfg->CapDataType == DSP_DATACAP_TYPE_VIDEO_YUV) {
            Rval = DSP_ERR_NONE;
        } else {
            Rval = DSP_ERR_NONE;
        }
    } else if (CapNum > 1U) {
        Rval = DSP_ERR_NONE;
    } else {
        Rval = DSP_ERR_NONE;
    }
#endif

    return Rval;
}

static inline UINT32 HL_StillVprocConfig(cmd_vproc_cfg_t *VprocCfg,
                                         UINT16 ViewZoneId, UINT32 InputFmt,
                                         UINT8 HdrExpNum, UINT8 WriteMode)
{
    UINT32 Rval;
    UINT16 GrpIdx = 0U;
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;
    CTX_STILL_INFO_s StlInfo = {0};
    UINT16 C2YInTileX = 0U, C2YTileX = 0U, MctfTileX = 0U;
    CTX_VPROC_INFO_s VprocInfo = {0U};

    HL_GetResourcePtr(&Resource);
    HL_GetStlInfo(HL_MTX_OPT_ALL, &StlInfo);

    VprocCfg->channel_id = (UINT8)ViewZoneId;
#ifdef SUPPORT_VPROC_RT_RESTART
(void)InputFmt;
    VprocCfg->input_format = DSP_VPROC_INPUT_RAW_RGB; // Max Possible Fmt
#else
    VprocCfg->input_format = (InputFmt == AMBA_DSP_YUV420)? DSP_VPROC_INPUT_YUV_420: DSP_VPROC_INPUT_YUV_422;
#endif

    VprocCfg->prev_mode = 0U; //DramPreview
    VprocCfg->num_of_exp = HdrExpNum;

    Rval = HL_GetVprocGroupIdx(ViewZoneId, &GrpIdx, 1/*IsStlProc*/);
    if (Rval == OK) {
        VprocCfg->grp_id = (UINT8)GrpIdx;
    }

    VprocCfg->is_li_enabled = (UINT8)1U;
    VprocCfg->is_hdr_enabled = (UINT8)0U;
    VprocCfg->is_warp_enabled = (UINT8)1U;

    VprocCfg->raw_width_max = (UINT16)(Resource->MaxStlRawInputWidth << 1U); // For UnCmpr Raw
    VprocCfg->raw_height_max = Resource->MaxStlRawInputHeight;
    VprocCfg->W0_max = (Resource->MaxStlYuvInputWidth > Resource->MaxStlRawInputWidth)? Resource->MaxStlYuvInputWidth: Resource->MaxStlRawInputWidth;
    VprocCfg->H0_max = (Resource->MaxStlYuvInputHeight > Resource->MaxStlRawInputHeight)? Resource->MaxStlYuvInputHeight: Resource->MaxStlRawInputHeight;

    VprocCfg->W_main_max = Resource->MaxStlMainWidth;
    VprocCfg->H_main_max = ALIGN_NUM16(Resource->MaxStlMainHeight, 16U);
    VprocCfg->W_pre_warp_luma_max = VprocCfg->W_main_max;
    VprocCfg->H_pre_warp_luma_max = VprocCfg->H_main_max;

    if ((StlInfo.RawInVprocPin == DSP_VPROC_PIN_PREVA) ||
        (StlInfo.YuvInVprocPin == DSP_VPROC_PIN_PREVA)) {
        VprocCfg->prev_a_fb_num = CV2X_MAX_STL_PREVA_FB_NUM;
        VprocCfg->prev_a_me_fb_num = DSP_VPROC_FB_NUM_DISABLE;
        VprocCfg->prev_a_w_max = VprocCfg->W_main_max;
        VprocCfg->prev_a_h_max = VprocCfg->H_main_max;
    } else {
        VprocCfg->prev_a_fb_num = DSP_VPROC_FB_NUM_DISABLE;
        VprocCfg->prev_a_me_fb_num = DSP_VPROC_FB_NUM_DISABLE;
    }

    if ((StlInfo.RawInVprocPin == DSP_VPROC_PIN_PREVB) ||
        (StlInfo.YuvInVprocPin == DSP_VPROC_PIN_PREVB)) {
        VprocCfg->prev_b_fb_num = CV2X_MAX_STL_PREVB_FB_NUM;
        VprocCfg->prev_b_me_fb_num = DSP_VPROC_FB_NUM_DISABLE;
        VprocCfg->prev_b_w_max = VprocCfg->W_main_max;
        VprocCfg->prev_b_h_max = VprocCfg->H_main_max;
    } else {
        VprocCfg->prev_b_fb_num = DSP_VPROC_FB_NUM_DISABLE;
        VprocCfg->prev_b_me_fb_num = DSP_VPROC_FB_NUM_DISABLE;
    }

    if ((StlInfo.RawInVprocPin == DSP_VPROC_PIN_PREVC) ||
        (StlInfo.YuvInVprocPin == DSP_VPROC_PIN_PREVC)) {
        VprocCfg->prev_c_fb_num = CV2X_MAX_STL_PREVC_FB_NUM;
        VprocCfg->prev_c_me_fb_num = DSP_VPROC_FB_NUM_DISABLE;
        VprocCfg->prev_c_w_max = VprocCfg->W_main_max;
        VprocCfg->prev_c_h_max = VprocCfg->H_main_max;
    } else {
        VprocCfg->prev_c_fb_num = DSP_VPROC_FB_NUM_DISABLE;
        VprocCfg->prev_c_me_fb_num = DSP_VPROC_FB_NUM_DISABLE;
    }

    VprocCfg->main_fb_num = CV2X_MAX_STL_MAIN_FB_NUM;
    VprocCfg->main_me01_fb_num = CV2X_MAX_STL_MAIN_FB_NUM;
    VprocCfg->ln_dec_fb_num = DSP_VPROC_FB_NUM_DISABLE;
    VprocCfg->hier_fb_num = DSP_VPROC_FB_NUM_DISABLE;
    VprocCfg->c2y_int_fb_num = CV2X_MAX_STL_C2Y_FB_NUM;
    VprocCfg->extra_sec2_vert_out_max = 0U;
    VprocCfg->warp_a_y_in_blk_h_max = 0U;
    VprocCfg->warp_a_uv_in_blk_h_max = 0U;

    /* Forbid MainResampling in Y2Y */
    Rval = HL_CalcVideoTileViewZone(ViewZoneId, VprocCfg->W_main_max,
                                    Resource->MaxVprocTileWidth, 0U,
                                    &C2YInTileX);
    if (Rval == OK) {
        Rval = HL_CalcVideoTileViewZone(ViewZoneId, VprocCfg->W_main_max,
                                        Resource->MaxVprocTileWidth, 0U,
                                        &C2YTileX);
        if (Rval == OK) {
            Rval = HL_CalcVideoTileViewZone(ViewZoneId, VprocCfg->W_main_max,
                                            Resource->MaxVprocTileWidth, 0U,
                                            &MctfTileX);
            if (Rval == OK) {
                HL_GetVprocInfo(HL_MTX_OPT_GET, ViewZoneId, &VprocInfo);
                Rval = HL_CalcVideoTileWidth(VprocCfg->W_main_max,
                                             C2YInTileX,
                                             &VprocInfo.MaxTileWidth);
                if (Rval == OK) {
                    VprocInfo.VprocTileMode = ((C2YInTileX > 1U) || (MctfTileX > 1U))? (UINT8)1U: (UINT8)0U;
                    HL_SetVprocInfo(HL_MTX_OPT_SET, ViewZoneId, &VprocInfo);

                    C2YTileX = MAX2_16(C2YInTileX, C2YTileX);
                    VprocCfg->c2y_tile_num_x_max = (UINT8)C2YTileX;
                    VprocCfg->c2y_tile_num_y_max = 1U;
                    VprocCfg->warp_tile_num_x_max = (UINT8)MctfTileX;
                    VprocCfg->warp_tile_num_y_max = 1U;
                }
            }
        }
    }

    if (Rval == OK) {
        Rval = AmbaHL_CmdVprocConfig(WriteMode, VprocCfg);
    }

    return Rval;
}

static UINT32 HL_DataCapCfgChk(const UINT16 CapInstance, const AMBA_DSP_DATACAP_CFG_s *pDataCapCfg)
{
    UINT32 Rval;
    const Dsp_DumpApi_t* DumpApi = AmbaDSP_GetDumpApiFunc();
    const Dsp_CheckApi_t* CheckApi = AmbaDSP_GetCheckApiFunc();
    CTX_DATACAP_INFO_s DataCapInfo;

    /* DSP CtxInit */
    Rval = HL_CtxInit();

    /* Input sanity check */
    if ((Rval == OK) &&
        (DumpApi != NULL)) {
        DumpApi->pDataCapCfg(CapInstance, pDataCapCfg);
    }
    if ((Rval == OK) &&
        (CheckApi != NULL)) {
        Rval = CheckApi->pDataCapCfg(CapInstance, pDataCapCfg);
    }

    /* Timing sanity check */
    if (Rval == OK) {
        /* DataCap status check: Not allowed Running to Updated */
        HL_GetDataCapInfo(HL_MTX_OPT_ALL, CapInstance, &DataCapInfo);
        if (DataCapInfo.Status > DATA_CAP_STATUS_UPDATED) {
            AmbaLL_LogUInt5("DataCapCfg status check failed [%d]", DataCapInfo.Status, 0U, 0U, 0U, 0U);
            Rval = DSP_ERR_0004;
        }
    }

    /* Logic sanity check */
    if (Rval == OK) {
        /* If the users ask for capturing aux-data, they must provide the corresponding buffer */
        if (pDataCapCfg->CapDataType == DSP_DATACAP_TYPE_RAW) {
            if ((pDataCapCfg->AuxDataNeeded == 1U) &&
                (((pDataCapCfg->AllocType == ALLOC_EXTERNAL_DISTINCT) && (pDataCapCfg->pAuxBufTbl == NULL)) ||
                 ((pDataCapCfg->AllocType == ALLOC_EXTERNAL_CYCLIC) && (pDataCapCfg->AuxDataBuf.BaseAddr == 0U)))) {
                AmbaLL_LogUInt5("No buffer provided for auxiliary data", 0U, 0U, 0U, 0U, 0U);
                Rval = DSP_ERR_0001;
            }
        }
    }

    return Rval;
}

UINT32 dsp_data_cap_cfg(UINT16 CapInstance, const AMBA_DSP_DATACAP_CFG_s *pDataCapCfg)
{
    UINT32 Rval;
    UINT16 i;
    CTX_DATACAP_INFO_s DataCapInfo;

    Rval = HL_DataCapCfgChk(CapInstance, pDataCapCfg);

    /* Body */
    if (Rval == OK) {
        HL_GetDataCapInfo(HL_MTX_OPT_GET, CapInstance, &DataCapInfo);
        (void)dsp_osal_memcpy(&DataCapInfo.Cfg, pDataCapCfg, sizeof(AMBA_DSP_DATACAP_CFG_s));
        if (pDataCapCfg->AllocType == ALLOC_EXTERNAL_DISTINCT) {
            if (pDataCapCfg->pBufTbl != NULL) {
                for (i = 0; i < pDataCapCfg->BufNum; i++) {
                    DataCapInfo.BufTbl[i] = pDataCapCfg->pBufTbl[i];
                }
            }
            if (pDataCapCfg->pAuxBufTbl != NULL) {
                for (i = 0; i < pDataCapCfg->BufNum; i++) {
                    DataCapInfo.AuxBufTbl[i] = pDataCapCfg->pAuxBufTbl[i];
                }
            }
        }
        DataCapInfo.Status = (UINT8)DATA_CAP_STATUS_UPDATED;
        HL_SetDataCapInfo(HL_MTX_OPT_SET, CapInstance, &DataCapInfo);
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("AmbaDSP_DataCapCfg Error[0x%X]", Rval, 0U, 0U, 0U, 0U);
    }

    return Rval;
}

static inline UINT32 HL_FillVinExtMemInCapUpdate(cmd_vin_set_ext_mem_t *pVinExtMem,
                                                 const UINT16 CapId,
                                                 const CTX_DATACAP_INFO_s *pDataCapInfo,
                                                 const AMBA_DSP_DATACAP_BUF_CFG_s *pCapBuf)
{
    UINT32 Rval = OK;
    ULONG ULAddr;
    CTX_VIN_INFO_s VinInfo;
    const AMBA_DSP_DATACAP_CFG_s *pDataCapCfg = &pDataCapInfo->Cfg;
    UINT16 i;
    UINT32 *pBufTblAddr = NULL;
    UINT32 PhysAddr;

    pVinExtMem->vin_id = (UINT8)pDataCapCfg->Index;
    pVinExtMem->allocation_type = (pDataCapCfg->AllocType == ALLOC_EXTERNAL_DISTINCT)? VIN_EXT_MEM_ALLOC_TYPE_DISTINCT: VIN_EXT_MEM_ALLOC_TYPE_CONTIGUOUS;
    pVinExtMem->allocation_mode = VIN_EXT_MEM_MODE_APPEND;
    pVinExtMem->overflow_cntl = (pDataCapCfg->OverFlowCtrl == 0U)? VIN_EXT_MEM_CTRL_WAIT_APPEND: VIN_EXT_MEM_CTRL_CIRCULAR;

    pVinExtMem->num_frm_buf = pCapBuf->BufNum;
    pVinExtMem->buf_pitch = ALIGN_NUM16(pDataCapCfg->DataBuf.Pitch, (UINT16)DSP_BUF_ALIGNMENT);
    pVinExtMem->buf_width = pVinExtMem->buf_pitch;
    pVinExtMem->buf_height = (UINT16)pDataCapCfg->DataBuf.Window.Height;

    if (pDataCapCfg->AllocType == ALLOC_EXTERNAL_DISTINCT) {
        HL_GetPointerToDspDataCapBufArray(CapId, &pBufTblAddr);
        for (i = 0; i < pDataCapCfg->BufNum; i++) {
            (void)dsp_osal_virt2cli(pDataCapInfo->BufTbl[i], &PhysAddr);
            pBufTblAddr[i] = PhysAddr;
        }
        dsp_osal_typecast(&ULAddr, &pBufTblAddr);
        (void)dsp_osal_virt2cli(ULAddr, &pVinExtMem->buf_addr);
    } else {
        (void)dsp_osal_virt2cli(pCapBuf->BufAddr, &pVinExtMem->buf_addr);
    }

    HL_GetVinInfo(HL_MTX_OPT_ALL,  pDataCapCfg->Index, &VinInfo);
    if (HL_IS_YUV422_INPUT(VinInfo.InputFormat) == 1U) {
        // YUV Vin
        pVinExtMem->buf_width = (UINT16)pDataCapCfg->DataBuf.Window.Width;
        pVinExtMem->memory_type = DSP_VIN_EXT_MEM_TYPE_YUV422;
        pVinExtMem->chroma_format = DSP_YUV_422;
        /*
         * vin_yuv_enc_fbp_init will allocate resource for virtual vin yuv input from ext_mem at vin_flow_max stage
         * Need to allocate resource (vin_yuv_enc_fbp_disable=0) when send ext_mem after vproc
         * */
        pVinExtMem->vin_yuv_enc_fbp_disable = 0U;
    } else if (HL_IS_YUV420_INPUT(VinInfo.InputFormat) == 1U) {
        // YUV Vin
        pVinExtMem->buf_width = (UINT16)pDataCapCfg->DataBuf.Window.Width;
        pVinExtMem->memory_type = DSP_VIN_EXT_MEM_TYPE_YUV422;
        pVinExtMem->chroma_format = DSP_YUV_420;
        /*
         * vin_yuv_enc_fbp_init will allocate resource for virtual vin yuv input from ext_mem at vin_flow_max stage
         * Need to allocate resource (vin_yuv_enc_fbp_disable=0) when send ext_mem after vproc
         * */
        pVinExtMem->vin_yuv_enc_fbp_disable = 0U;
    } else {
        // Sensor Vin
        pVinExtMem->chroma_format = DSP_YUV_MONO;
        pVinExtMem->memory_type = DSP_VIN_EXT_MEM_TYPE_RAW;
    }

    return Rval;
}

static inline UINT32 HL_FillVinExtAuxMemInCapUpdate(cmd_vin_set_ext_mem_t *pVinExtMem,
                                                    const UINT16 CapId,
                                                    const CTX_DATACAP_INFO_s *pDataCapInfo,
                                                    const AMBA_DSP_DATACAP_BUF_CFG_s *pCapBuf)
{
    UINT32 Rval = OK;
    ULONG ULAddr;
    const AMBA_DSP_DATACAP_CFG_s *pDataCapCfg = &pDataCapInfo->Cfg;
    UINT16 i;
    UINT32 *pBufTblAddr = NULL;
    UINT32 PhysAddr;

    pVinExtMem->vin_id = (UINT8)pDataCapCfg->Index;
    pVinExtMem->allocation_type = (pDataCapCfg->AllocType == ALLOC_EXTERNAL_DISTINCT)? VIN_EXT_MEM_ALLOC_TYPE_DISTINCT: VIN_EXT_MEM_ALLOC_TYPE_CONTIGUOUS;
    pVinExtMem->allocation_mode = VIN_EXT_MEM_MODE_APPEND;
    /* case 2, user control */
    pVinExtMem->overflow_cntl = (pDataCapCfg->OverFlowCtrl == 0U)? VIN_EXT_MEM_CTRL_WAIT_APPEND: VIN_EXT_MEM_CTRL_CIRCULAR;

    pVinExtMem->num_frm_buf = pCapBuf->BufNum;
    pVinExtMem->buf_pitch = ALIGN_NUM16(pDataCapCfg->AuxDataBuf.Pitch, (UINT16)DSP_BUF_ALIGNMENT);
    pVinExtMem->buf_width = pVinExtMem->buf_pitch;
    pVinExtMem->buf_height = (UINT16)pDataCapCfg->AuxDataBuf.Window.Height;

    if (pDataCapCfg->AllocType == ALLOC_EXTERNAL_DISTINCT) {
        HL_GetPointerToDspDataCapBufArray(CapId, &pBufTblAddr);
        for (i = 0; i < pDataCapCfg->BufNum; i++) {
            (void)dsp_osal_virt2cli(pDataCapInfo->AuxBufTbl[i], &PhysAddr);
            pBufTblAddr[i] = PhysAddr;
        }
        dsp_osal_typecast(&ULAddr, &pBufTblAddr);
        (void)dsp_osal_virt2cli(ULAddr, &pVinExtMem->buf_addr);
    } else {
        (void)dsp_osal_virt2cli(pCapBuf->AuxBufAddr, &pVinExtMem->buf_addr);
    }

    pVinExtMem->chroma_format = DSP_YUV_MONO;
    pVinExtMem->memory_type = DSP_VIN_EXT_MEM_TYPE_HDS;

    return Rval;
}

static inline UINT32 HL_FillVprocExtMemInCapUpdate(cmd_vproc_set_ext_mem_t *pVprocExtMem,
                                                   const CTX_DATACAP_INFO_s *pDataCapInfo,
                                                   const UINT16 YuvStrmId,
                                                   const AMBA_DSP_DATACAP_BUF_CFG_s *pCapBuf)
{
    UINT32 Rval = OK;
    ULONG ULAddr;
    AMBA_DSP_BUF_s CalcExtBuf = {0U};
    const AMBA_DSP_DATACAP_CFG_s *pDataCapCfg = &pDataCapInfo->Cfg;

    pVprocExtMem->memory_type = pDataCapInfo->BufMemType;
    pVprocExtMem->allocation_type = (pDataCapCfg->AllocType == ALLOC_EXTERNAL_DISTINCT)? VIN_EXT_MEM_ALLOC_TYPE_DISTINCT: VIN_EXT_MEM_ALLOC_TYPE_CONTIGUOUS;
    pVprocExtMem->allocation_mode = VIN_EXT_MEM_MODE_APPEND;
    /* case 2, user control */
    pVprocExtMem->overflow_cntl = (pDataCapCfg->OverFlowCtrl == 0U)? VIN_EXT_MEM_CTRL_WAIT_APPEND: VIN_EXT_MEM_CTRL_CIRCULAR;
    pVprocExtMem->num_frm_buf = pCapBuf->BufNum;
    if (pVprocExtMem->num_frm_buf > EXT_MEM_VPROC_MAX_NUM) {
        AmbaLL_LogUInt5("Only %d ExtMem supported", EXT_MEM_VPROC_MAX_NUM, 0U, 0U, 0U, 0U);
    }
    (void)HL_StillCaptureCalcExtBufImpl(YuvStrmId, DSP_DATACAP_CALTYPE_LUMA, &CalcExtBuf);
    pVprocExtMem->buf_pitch = CalcExtBuf.Pitch;
    pVprocExtMem->buf_width = CalcExtBuf.Window.Width;
    pVprocExtMem->buf_height= CalcExtBuf.Window.Height;

    if (pDataCapCfg->AllocType == ALLOC_EXTERNAL_DISTINCT) {
        dsp_osal_typecast(&ULAddr, &pCapBuf->pBufTbl);
        (void)dsp_osal_virt2cli(ULAddr, &pVprocExtMem->buf_addr);
    } else {
        (void)dsp_osal_virt2cli(pCapBuf->BufAddr, &pVprocExtMem->buf_addr);
    }

    pVprocExtMem->img_width = pDataCapCfg->DataBuf.Window.Width;
    pVprocExtMem->img_height = pDataCapCfg->DataBuf.Window.Height;
//FIXME, any possiblity that Yuv422 is used?
    pVprocExtMem->chroma_format = DSP_YUV_420;

    return Rval;
}

static inline UINT32 HL_FillVprocExtAuxMemInCapUpdate(cmd_vproc_set_ext_mem_t *pVprocExtMem,
                                                      const CTX_DATACAP_INFO_s *pDataCapInfo,
                                                      const UINT16 YuvStrmId,
                                                      const AMBA_DSP_DATACAP_BUF_CFG_s *pCapBuf)
{
    UINT32 Rval;
    ULONG ULAddr;
    AMBA_DSP_BUF_s CalcExtBuf = {0U};
    const AMBA_DSP_DATACAP_CFG_s *pDataCapCfg = &pDataCapInfo->Cfg;

    pVprocExtMem->memory_type = pDataCapInfo->AuxBufMemType;
    pVprocExtMem->allocation_type = (pDataCapCfg->AllocType == ALLOC_EXTERNAL_DISTINCT)? VIN_EXT_MEM_ALLOC_TYPE_DISTINCT: VIN_EXT_MEM_ALLOC_TYPE_CONTIGUOUS;
    pVprocExtMem->allocation_mode = VIN_EXT_MEM_MODE_APPEND;
    /* case 2, user control */
    pVprocExtMem->overflow_cntl = (pDataCapCfg->OverFlowCtrl == 0U)? VIN_EXT_MEM_CTRL_WAIT_APPEND: VIN_EXT_MEM_CTRL_CIRCULAR;
    pVprocExtMem->num_frm_buf = pCapBuf->BufNum;
    if (pVprocExtMem->num_frm_buf > EXT_MEM_VPROC_MAX_NUM) {
        AmbaLL_LogUInt5("Only %d ExtMem supported", EXT_MEM_VPROC_MAX_NUM, 0U, 0U, 0U, 0U);
    }
    // ME1 buf from AuxBuf
    Rval = HL_StillCaptureCalcExtBufImpl(YuvStrmId, DSP_DATACAP_CALTYPE_ME1, &CalcExtBuf);
    if (Rval == OK) {
        pVprocExtMem->buf_pitch = CalcExtBuf.Pitch;
        pVprocExtMem->buf_width = CalcExtBuf.Window.Width;
        pVprocExtMem->buf_height= CalcExtBuf.Window.Height;

        if (pDataCapCfg->AllocType == ALLOC_EXTERNAL_DISTINCT) {
            dsp_osal_typecast(&ULAddr, &pCapBuf->pAuxBufTbl);
            (void)dsp_osal_virt2cli(ULAddr, &pVprocExtMem->buf_addr);
        } else {
            (void)dsp_osal_virt2cli(pCapBuf->AuxBufAddr, &pVprocExtMem->buf_addr);
        }

        pVprocExtMem->img_width = pDataCapCfg->AuxDataBuf.Window.Width >> DSP_VIDEO_ME1_DATA_SHIFT;
        pVprocExtMem->img_height = pDataCapCfg->AuxDataBuf.Window.Height >> DSP_VIDEO_ME1_DATA_SHIFT;
        pVprocExtMem->chroma_format = DSP_YUV_MONO;

        // ME0 buf from AuxBuf
        Rval = HL_StillCaptureCalcExtBufImpl(YuvStrmId, DSP_DATACAP_CALTYPE_ME0, &CalcExtBuf);
        if (Rval == OK) {
            pVprocExtMem->aux_pitch = CalcExtBuf.Pitch;
            pVprocExtMem->aux_width = CalcExtBuf.Window.Width;
            pVprocExtMem->aux_height= CalcExtBuf.Window.Height;
            pVprocExtMem->aux_img_ofs_x = 0U;
            pVprocExtMem->aux_img_ofs_y = 0U;
            pVprocExtMem->aux_img_width = pDataCapCfg->AuxDataBuf.Window.Width >> DSP_VIDEO_ME0_DATA_SHIFT;
            pVprocExtMem->aux_img_height = pDataCapCfg->AuxDataBuf.Window.Height >> DSP_VIDEO_ME0_DATA_SHIFT;
        }
    }
    return Rval;
}

static inline UINT32 HL_UpdateCapBufferChk(const UINT16 CapInstance,
                                           const AMBA_DSP_DATACAP_BUF_CFG_s *pCapBuf,
                                           const UINT64 *pAttachedRawSeq)
{
    UINT32 Rval;
    const Dsp_DumpApi_t* DumpApi = AmbaDSP_GetDumpApiFunc();
    const Dsp_CheckApi_t* CheckApi = AmbaDSP_GetCheckApiFunc();
    const Dsp_ResourceApi_t* ResourceApi = AmbaDSP_GetResourceApiFunc();
    CTX_DATACAP_INFO_s DataCapInfo;

    /* DSP CtxInit */
    Rval = HL_CtxInit();

    /* Timing sanity check */
    /* Input sanity check */
    if ((Rval == OK) &&
        (DumpApi != NULL)) {
        DumpApi->pUpdateCapBuffer(CapInstance, pCapBuf, pAttachedRawSeq);
    }
    if ((Rval == OK) &&
        (CheckApi != NULL)) {
        Rval = CheckApi->pUpdateCapBuffer(CapInstance, pCapBuf, pAttachedRawSeq);
    }
    /* HW/SW resources check */
    if ((Rval == OK) &&
        (ResourceApi != NULL)) {
        Rval = ResourceApi->pUpdateCapBuffer(CapInstance, pCapBuf, pAttachedRawSeq);
    }

    /* Logic sanity check */
    if (Rval == OK) {
        HL_GetDataCapInfo(HL_MTX_OPT_ALL, CapInstance, &DataCapInfo);
        if (DataCapInfo.Status != DATA_CAP_STATUS_RUNNING) {
            Rval = DSP_ERR_0004;
        } else if (DataCapInfo.Cfg.AllocType != pCapBuf->AllocType) {
            Rval = DSP_ERR_0001;
        } else {
            //TBD
        }
    }

    return Rval;
}

UINT32 dsp_update_cap_buffer(UINT16 CapInstance, const AMBA_DSP_DATACAP_BUF_CFG_s *pCapBuf, UINT64 *pAttachedRawSeq)
{
    UINT32 Rval;
    CTX_DATACAP_INFO_s DataCapInfo;
    CTX_YUV_STRM_INFO_s *YuvStrmInfo = HL_CtxYuvStrmInfoPtr;
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;
    const AMBA_DSP_DATACAP_CFG_s *pDataCapCfg;
    UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
    cmd_vin_set_ext_mem_t *VinExtMem = HL_DefCtxCmdBufPtrVinExtMem;
    cmd_vproc_set_ext_mem_t *VprocExtMem = HL_DefCtxCmdBufPtrVpcExtMem;
    UINT8 WriteMode = AMBA_DSP_CMD_VDSP_NORMAL_WRITE;
    UINT16 YuvStrmId = 0U, VinId = 0U;

    Rval = HL_UpdateCapBufferChk(CapInstance, pCapBuf, pAttachedRawSeq);
    AmbaMisra_TouchUnused(pAttachedRawSeq);

    /* Body */
    if (Rval == OK) {
        HL_GetDataCapInfo(HL_MTX_OPT_ALL, CapInstance, &DataCapInfo);
        pDataCapCfg = &DataCapInfo.Cfg;
        if (DataCapInfo.Cfg.CapDataType == DSP_DATACAP_TYPE_RAW) {
            HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
            dsp_osal_typecast(&VinExtMem, &CmdBufferAddr);

            /* case 2, user control */
            (void)HL_FillVinExtMemInCapUpdate(VinExtMem, CapInstance, &DataCapInfo, pCapBuf);
            Rval = AmbaHL_CmdVinSetExtMem(WriteMode, VinExtMem);
            HL_RelCmdBuffer(CmdBufferId);
            if (Rval == OK) {
                if (pDataCapCfg->AuxDataNeeded > 0U) {
                    HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
                    dsp_osal_typecast(&VinExtMem, &CmdBufferAddr);

                    /* case 2, user control */
                    (void)HL_FillVinExtAuxMemInCapUpdate(VinExtMem, CapInstance, &DataCapInfo, pCapBuf);
                    Rval = AmbaHL_CmdVinSetExtMem(WriteMode, VinExtMem);
                    HL_RelCmdBuffer(CmdBufferId);
                }
            }
        } else if ((DataCapInfo.Cfg.CapDataType == DSP_DATACAP_TYPE_YUV) ||
            (DataCapInfo.Cfg.CapDataType == DSP_DATACAP_TYPE_VIDEO_YUV)) {

            HL_GetYuvStrmIdOnCapCfg(pDataCapCfg, &YuvStrmId);
            HL_GetYuvStrmInfoPtr(YuvStrmId, &YuvStrmInfo);

            // SetExtMem
            HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
            dsp_osal_typecast(&VprocExtMem, &CmdBufferAddr);
            HL_GetViewZoneInfoPtr(YuvStrmInfo->Layout.ChanCfg[0U].ViewZoneId, &ViewZoneInfo);
            DSP_Bit2U16Idx(ViewZoneInfo->SourceVin, &VinId);
            WriteMode = HL_GetVin2CmdNormalWrite(VinId);
            VprocExtMem->channel_id = (UINT8)YuvStrmInfo->Layout.ChanCfg[0U].ViewZoneId;
            /* case 2, user control */
            (void)HL_FillVprocExtMemInCapUpdate(VprocExtMem, &DataCapInfo, YuvStrmId, pCapBuf);
            Rval = AmbaHL_CmdVprocSetExtMem(WriteMode, VprocExtMem);
            HL_RelCmdBuffer(CmdBufferId);
            if (Rval == OK) {
                if (DataCapInfo.Cfg.CapDataType == DSP_DATACAP_TYPE_VIDEO_YUV) {
                    // Set ME data
                    HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
                    dsp_osal_typecast(&VprocExtMem, &CmdBufferAddr);
                    VprocExtMem->channel_id = (UINT8)YuvStrmInfo->Layout.ChanCfg[0U].ViewZoneId;
                    /* case 2, user control */
                    Rval = HL_FillVprocExtAuxMemInCapUpdate(VprocExtMem, &DataCapInfo, YuvStrmId, pCapBuf);
                    if (Rval == OK) {
                        Rval = AmbaHL_CmdVprocSetExtMem(WriteMode, VprocExtMem);
                    }
                    HL_RelCmdBuffer(CmdBufferId);
                }
            }
        } else {
            //TBD
        }
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("AmbaDSP_UpdateCapBuffer Error[0x%X]", Rval, 0U, 0U, 0U, 0U);
    }

    return Rval;
}

static inline UINT32 HL_DataCapCtrlChk(const UINT16 NumCapInstance,
                                       const UINT16 *pCapInstance,
                                       const AMBA_DSP_DATACAP_CTRL_s *pDataCapCtrl,
                                       const UINT64 *pAttachedRawSeq)
{
    UINT32 Rval;
    const Dsp_DumpApi_t* DumpApi = AmbaDSP_GetDumpApiFunc();
    const Dsp_CheckApi_t* CheckApi = AmbaDSP_GetCheckApiFunc();
    const Dsp_ResourceApi_t* ResourceApi = AmbaDSP_GetResourceApiFunc();

    /* DSP CtxInit */
    Rval = HL_CtxInit();

    /* Timing sanity check */
    /* Input sanity check */
    if ((Rval == OK) &&
        (DumpApi != NULL)) {
        DumpApi->pDataCapCtrl(NumCapInstance, pCapInstance, pDataCapCtrl, pAttachedRawSeq);
    }
    if ((Rval == OK) &&
        (CheckApi != NULL)) {
        Rval = CheckApi->pDataCapCtrl(NumCapInstance, pCapInstance, pDataCapCtrl, pAttachedRawSeq);
    }
    /* HW/SW resources check */
    if ((Rval == OK) &&
        (ResourceApi != NULL)) {
        Rval = ResourceApi->pDataCapCtrl(NumCapInstance, pCapInstance, pDataCapCtrl, pAttachedRawSeq);
    }

    /* Logic sanity check */

    return Rval;
}

static inline void HL_DataCapCtrlPreProc(const AMBA_DSP_DATACAP_CFG_s *pDataCapCfg,
                                         const UINT16 CapInstance,
                                         const UINT32 CapNum,
                                         CTX_DATACAP_INFO_s *pDataCapInfo,
                                         UINT16 *RawCapInst,
                                         UINT16 *pRawCapNum,
                                         UINT16 *YuvCapInst,
                                         UINT16 *pYuvCapNum)
{
    UINT16 RawCapNum = *pRawCapNum;
    UINT16 YuvCapNum = *pYuvCapNum;

    if (pDataCapCfg->CapDataType == DSP_DATACAP_TYPE_RAW) {
        HL_GetDataCapInfo(HL_MTX_OPT_GET, CapInstance, pDataCapInfo);
        pDataCapInfo->CountDown = CapNum;
        /* Note: Using circular overflow-control only when the number of buffers is enough
         * for capture to avoid late Vin-Idle causing buffer reuse */
        if (CapNum > pDataCapCfg->BufNum) {
            pDataCapInfo->Cfg.OverFlowCtrl = 0U; /*Wait append*/
        } else {
            pDataCapInfo->Cfg.OverFlowCtrl = 1U; /*Circular*/
        }
        HL_SetDataCapInfo(HL_MTX_OPT_SET, CapInstance, pDataCapInfo);

        RawCapInst[RawCapNum] = CapInstance;
        RawCapNum++;
    } else if (pDataCapCfg->CapDataType == DSP_DATACAP_TYPE_YUV) {
        if (HL_StillCaptureNumCheck(CapInstance, CapNum) == OK) {
            HL_GetDataCapInfo(HL_MTX_OPT_GET, CapInstance, pDataCapInfo);
            pDataCapInfo->CountDown = CapNum;
            HL_SetDataCapInfo(HL_MTX_OPT_SET, CapInstance, pDataCapInfo);

            YuvCapInst[YuvCapNum] = CapInstance;
            YuvCapNum++;
        } else {
            AmbaLL_LogUInt5("DataCapCtrl[%d] CapNum(0x%X) check failed in Yuv mode: support 1, 0x%X/0 (only for memory purpose)",
                CapInstance, CapNum, DSP_DATACAP_INFINITE_CAP_NUM, 0U, 0U);
        }
    } else if (pDataCapCfg->CapDataType == DSP_DATACAP_TYPE_SYNCED_YUV) {
        /* Only one Yuv supported */
        if (HL_StillCaptureNumCheck(CapInstance, CapNum) == OK) {
            HL_GetDataCapInfo(HL_MTX_OPT_GET, CapInstance, pDataCapInfo);
            pDataCapInfo->CountDown = CapNum;
            pDataCapInfo->Status = DATA_CAP_STATUS_2_RUN;
            HL_SetDataCapInfo(HL_MTX_OPT_SET, CapInstance, pDataCapInfo);
        } else {
            AmbaLL_LogUInt5("DataCapCtrl[%d] CapNum(%d) check failed in SyncedYuv mode: only support 1",
                CapInstance, CapNum, 0U, 0U, 0U);
        }
    } else if (pDataCapCfg->CapDataType == DSP_DATACAP_TYPE_COMP_RAW) {
        HL_GetDataCapInfo(HL_MTX_OPT_GET, CapInstance, pDataCapInfo);
        pDataCapInfo->CountDown = CapNum;
        HL_SetDataCapInfo(HL_MTX_OPT_SET, CapInstance, pDataCapInfo);

        YuvCapInst[YuvCapNum] = CapInstance;
        YuvCapNum++;
    } else if (pDataCapCfg->CapDataType == DSP_DATACAP_TYPE_VIDEO_YUV) {
        if (HL_StillCaptureNumCheck(CapInstance, CapNum) == OK) {
            HL_GetDataCapInfo(HL_MTX_OPT_GET, CapInstance, pDataCapInfo);
            pDataCapInfo->CountDown = CapNum;
            HL_SetDataCapInfo(HL_MTX_OPT_SET, CapInstance, pDataCapInfo);

            YuvCapInst[YuvCapNum] = CapInstance;
            YuvCapNum++;
        } else {
            AmbaLL_LogUInt5("DataCapCtrl[%d] CapNum(0x%X) check failed in VideoYuv mode: support 1, 0x%X/0 (only for pip Timelapse) ",
                CapInstance, CapNum, DSP_DATACAP_INFINITE_CAP_NUM, 0U, 0U);
        }
    } else {
        //TBD
    }

    *pRawCapNum = RawCapNum;
    *pYuvCapNum = YuvCapNum;
}

UINT32 dsp_data_cap_ctrl(UINT16 NumCapInstance,
                         const UINT16 *pCapInstance,
                         const AMBA_DSP_DATACAP_CTRL_s *pDataCapCtrl,
                         UINT64 *pAttachedRawSeq)
{
    UINT16 i;
    UINT16 RawCapNum = 0U, YuvCapNum = 0U;
    UINT16 RawCapInst[AMBA_DSP_MAX_DATACAP_NUM] = {0};
    UINT16 YuvCapInst[AMBA_DSP_MAX_DATACAP_NUM] = {0};
    UINT32 Rval;
    CTX_DATACAP_INFO_s DataCapInfo;
    const AMBA_DSP_DATACAP_CFG_s *pDataCapCfg;

    Rval = HL_DataCapCtrlChk(NumCapInstance,
                             pCapInstance,
                             pDataCapCtrl,
                             pAttachedRawSeq);
    AmbaMisra_TouchUnused(pAttachedRawSeq);

    /* Body */
    if (Rval == OK) {
        for (i = 0U; i < NumCapInstance; i++) {
            HL_GetDataCapInfo(HL_MTX_OPT_ALL, pCapInstance[i], &DataCapInfo);
            pDataCapCfg = &DataCapInfo.Cfg;

            if (DataCapInfo.Status != DATA_CAP_STATUS_UPDATED) {
                if ((DataCapInfo.Status == DATA_CAP_STATUS_RUNNING) &&
                    (pDataCapCtrl[i].CapNum == 0U)) {
                    // Stop Running
                    HL_GetDataCapInfo(HL_MTX_OPT_GET, pCapInstance[i], &DataCapInfo);
                    DataCapInfo.Status = DATA_CAP_STATUS_2_STOP;
                    HL_SetDataCapInfo(HL_MTX_OPT_SET, pCapInstance[i], &DataCapInfo);
                } else {
                    // Status checking failed
                    AmbaLL_LogUInt5("DataCapCtrl[%d] status check failed [Status:%d] [CapNum:%d]",
                        pCapInstance[i], DataCapInfo.Status, pDataCapCtrl[i].CapNum, 0U, 0U);
                    continue;
                }
            } else if (pDataCapCtrl[i].CapNum == 0U) {
                // DO NOTHING
                continue;
            } else {
                // DO NOTHING
            }

            HL_DataCapCtrlPreProc(pDataCapCfg,
                                  pCapInstance[i],
                                  pDataCapCtrl[i].CapNum,
                                  &DataCapInfo,
                                  RawCapInst,
                                  &RawCapNum,
                                  YuvCapInst,
                                  &YuvCapNum);
        }
        // Run raw capture implement
        if (RawCapNum > 0U) {
            Rval = HL_StlCapRawImpl(RawCapNum, &RawCapInst[0U]);
        }
        // Run yuv capture implement
        if (YuvCapNum > 0U) {
            Rval = HL_StillCaptureYuvImpl(YuvCapNum, &YuvCapInst[0U]);
        }
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("AmbaDSP_DataCapCtrl Error[0x%X]", Rval, 0U, 0U, 0U, 0U);
    }

    return Rval;
}

static inline UINT32 HL_StlY2yChk(const AMBA_DSP_YUV_IMG_BUF_s *pYuvIn,
                                  const AMBA_DSP_YUV_IMG_BUF_s *pYuvOut,
                                  const AMBA_DSP_ISOCFG_CTRL_s *pIsoCfg,
                                  const UINT32 Opt,
                                  const UINT64 *pAttachedRawSeq)
{
    UINT32 Rval;
    CTX_STILL_INFO_s StlInfo = {0};
    const Dsp_DumpApi_t* DumpApi = AmbaDSP_GetDumpApiFunc();
    const Dsp_CheckApi_t* CheckApi = AmbaDSP_GetCheckApiFunc();
    const Dsp_TimingApi_t* TimingApi = AmbaDSP_GetTimingApiFunc();
    const Dsp_ResourceApi_t* ResourceApi = AmbaDSP_GetResourceApiFunc();

    /* DSP CtxInit */
    Rval = HL_CtxInit();

    if ((Rval == OK) && (DumpApi != NULL)) {
        DumpApi->pStillYuv2Yuv(pYuvIn,
                               pYuvOut,
                               pIsoCfg,
                               Opt,
                               pAttachedRawSeq);
    }
    /* Input sanity check */
    if ((Rval == OK) && (CheckApi != NULL)) {
        Rval = CheckApi->pStillYuv2Yuv(pYuvIn,
                                       pYuvOut,
                                       pIsoCfg,
                                       Opt,
                                       pAttachedRawSeq);
    }
    /* HW/SW resources check */
    if ((Rval == OK) && (ResourceApi != NULL)) {
        Rval = ResourceApi->pStillYuv2Yuv(pYuvIn,
                                          pYuvOut,
                                          pIsoCfg,
                                          Opt,
                                          pAttachedRawSeq);
    }

    /* Timing sanity check */
    if ((Rval == OK) && (TimingApi != NULL)) {
        Rval = TimingApi->pStillYuv2Yuv(pYuvIn,
                                        pYuvOut,
                                        pIsoCfg,
                                        Opt,
                                        pAttachedRawSeq);
    }

    /* Logic sanity check */
    HL_GetStlInfo(HL_MTX_OPT_ALL, &StlInfo);
    if ((StlInfo.YuvInVirtVinId == DSP_VIRT_VIN_IDX_INVALID) ||
        (StlInfo.YuvInVprocId == DSP_VIRT_VIN_IDX_INVALID)) {
        Rval = DSP_ERR_0004;
    }

    return Rval;
}

/*
 * 20190614, Per ChenHan, Bind/Unbind shall go through VinCmdQ if relative VinFP.
 *           ucode may hit concurrent access if go through VoutCmdQ
 */
static inline UINT32 HL_StlY2yBind(const AMBA_DSP_YUV_IMG_BUF_s *pYuvIn,
                                   const CTX_STILL_INFO_s *pStlInfo,
                                   UINT8 WriteMode)
{
    UINT8 VprocStatus;
    UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
    UINT16 Y2yVinId = pStlInfo->YuvInVirtVinId + AMBA_DSP_MAX_VIN_NUM;
    UINT16 ViewZoneId;
    UINT32 Rval;
    cmd_binding_cfg_t *FpBindCfg = HL_DefCtxCmdBufPtrBind;

#ifndef SUPPORT_VPROC_RT_RESTART
    ViewZoneId = (pYuvIn->DataFmt == AMBA_DSP_YUV420)? pStlInfo->YuvInVprocId: pStlInfo->Yuv422InVprocId;
    VprocStatus = (pYuvIn->DataFmt == AMBA_DSP_YUV420)? pStlInfo->YuvInVprocStatus: pStlInfo->Yuv422InVprocStatus;
#else
    (void)pYuvIn;
    ViewZoneId = pStlInfo->YuvInVprocId;
    VprocStatus = pStlInfo->YuvInVprocStatus;
#endif

    /* UnBind first */
    if (VprocStatus == STL_VPROC_STATUS_COMPLETE) {
        HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
        dsp_osal_typecast(&FpBindCfg, &CmdBufferAddr);
        Rval = HL_FillFpBindSetup(FpBindCfg,
                                  BIND_STILL_PROC,
                                  0U/*IsBind*/,
                                  Y2yVinId,
                                  0U/*SrcPinId*/,
                                  ViewZoneId);
        if (Rval != OK) {
            AmbaLL_LogUInt5("[%d]FpBind filling fail", __LINE__, 0U, 0U, 0U, 0U);
        } else {
            (void)AmbaHL_CmdDspBindingCfg(WriteMode, FpBindCfg);
        }
        HL_RelCmdBuffer(CmdBufferId);
#ifdef SUPPORT_VPROC_RT_RESTART
    } else if (pStlInfo->RawInVprocStatus == STL_VPROC_STATUS_COMPLETE) {
        HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
        dsp_osal_typecast(&FpBindCfg, &CmdBufferAddr);
        Rval = HL_FillFpBindSetup(FpBindCfg,
                                  BIND_STILL_PROC,
                                  0U/*IsBind*/,
                                  pStlInfo->RawInVirtVinId + AMBA_DSP_MAX_VIN_NUM,
                                  0U/*SrcPinId*/,
                                  ViewZoneId);
        if (Rval != OK) {
            AmbaLL_LogUInt5("[%d]FpBind filling fail", __LINE__, 0U, 0U, 0U, 0U);
        } else {
            (void)AmbaHL_CmdDspBindingCfg(WriteMode, FpBindCfg);
        }
        HL_RelCmdBuffer(CmdBufferId);
    } else {
        //TBD
#endif
    }

    /* Bind VirtVin and Vproc */
    HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
    dsp_osal_typecast(&FpBindCfg, &CmdBufferAddr);
    Rval = HL_FillFpBindSetup(FpBindCfg,
                              BIND_STILL_PROC,
                              1U/*IsBind*/,
                              Y2yVinId,
                              0U/*SrcPinId*/,
                              ViewZoneId);
    if (Rval != OK) {
        AmbaLL_LogUInt5("[%d]FpBind filling fail", __LINE__, 0U, 0U, 0U, 0U);
    } else {
        (void)AmbaHL_CmdDspBindingCfg(WriteMode, FpBindCfg);
    }
    HL_RelCmdBuffer(CmdBufferId);
    return Rval;
}

static inline UINT32 HL_FillVprocSetupInY2Y(cmd_vproc_setup_t *pVprocSetup,
                                            const AMBA_DSP_YUV_IMG_BUF_s *pYuvIn,
                                            const AMBA_DSP_YUV_IMG_BUF_s *pYuvOut,
                                            UINT16 ViewZoneId,
                                            const CTX_STILL_INFO_s *pStlInfo)
{
    UINT32 Rval = OK;
#ifdef SUPPORT_DSP_EXT_PIN_BUF
    ULONG ULAddr;
    ext_mem_desc_t *pExtMemDesc = NULL;
#endif
    CTX_VPROC_INFO_s VprocInfo = {0};

    pVprocSetup->channel_id = (UINT8)ViewZoneId;
    pVprocSetup->input_format = (pYuvIn->DataFmt == AMBA_DSP_YUV420)? DSP_VPROC_INPUT_YUV_420: DSP_VPROC_INPUT_YUV_422;
    pVprocSetup->is_raw_compressed = (UINT8)0U;

    pVprocSetup->W_main = pYuvIn->Window.Width;
    pVprocSetup->H_main = ALIGN_NUM16(pYuvIn->Window.Height, 16U);

    /*
     * 20190703 per uCoder[ChenHan]
     * uCode allocates VprocChannel resource based on VprocCfg + pVprocSetup->is_tile_mode
     * so we need to consider TileMode for preventing too much resource be requested
     */
    HL_GetVprocInfo(HL_MTX_OPT_ALL, ViewZoneId, &VprocInfo);
    pVprocSetup->is_tile_mode = VprocInfo.VprocTileMode;

    pVprocSetup->is_li_enabled = (UINT8)1U;
    pVprocSetup->is_warp_enabled = (UINT8)1U;
    pVprocSetup->is_hdr_enabled = (UINT8)0U;

    pVprocSetup->prev_mode = (UINT8)0U;
    if (pStlInfo->YuvInVprocPin == DSP_VPROC_PIN_PREVA) {
        pVprocSetup->is_prev_a_enabled = (UINT8)1U;
        pVprocSetup->prev_a_format = VPROC_PREV_FMT_PROG;
        pVprocSetup->prev_a_src = VPROC_PREV_SRC_DRAM;
        pVprocSetup->prev_a_dst = AMBA_DSP_PREV_DEST_PIN;
        pVprocSetup->prev_a_frame_rate = 0U;
        pVprocSetup->prev_a_ch_fmt = (pYuvOut->DataFmt == AMBA_DSP_YUV420)? DSP_YUV_420: DSP_YUV_422;
        pVprocSetup->prev_a_w = pYuvOut->Window.Width;
        pVprocSetup->prev_a_h = pYuvOut->Window.Height;
    } else {
        pVprocSetup->is_prev_a_enabled = (UINT8)0U;
        pVprocSetup->prev_a_format = VPROC_PREV_FMT_PROG;
        pVprocSetup->prev_a_src = VPROC_PREV_SRC_DRAM;
        pVprocSetup->prev_a_dst = AMBA_DSP_PREV_DEST_DUMMY;
        pVprocSetup->prev_a_frame_rate = 0U;
        pVprocSetup->prev_a_ch_fmt = DSP_YUV_420;
    }

    if (pStlInfo->YuvInVprocPin == DSP_VPROC_PIN_PREVB) {
        pVprocSetup->is_prev_b_enabled = (UINT8)1U;
        pVprocSetup->prev_b_dst = AMBA_DSP_PREV_DEST_PIN;
        pVprocSetup->prev_b_frame_rate = 0U;
        pVprocSetup->prev_b_ch_fmt = (pYuvOut->DataFmt == AMBA_DSP_YUV420)? DSP_YUV_420: DSP_YUV_422;
        pVprocSetup->prev_b_w = pYuvOut->Window.Width;
        pVprocSetup->prev_b_h = pYuvOut->Window.Height;
    } else {
        pVprocSetup->is_prev_b_enabled = (UINT8)0U;
        pVprocSetup->prev_b_dst = AMBA_DSP_PREV_DEST_DUMMY;
        pVprocSetup->prev_b_frame_rate = 0U;
        pVprocSetup->prev_b_ch_fmt = DSP_YUV_420;
    }

    if (pStlInfo->YuvInVprocPin == DSP_VPROC_PIN_PREVC) {
        pVprocSetup->is_prev_c_enabled = (UINT8)1U;
        pVprocSetup->prev_c_format = VPROC_PREV_FMT_PROG;
        pVprocSetup->prev_c_src = VPROC_PREV_SRC_DRAM;
        pVprocSetup->prev_c_dst = AMBA_DSP_PREV_DEST_PIN;
        pVprocSetup->prev_c_frame_rate = 0U;
        pVprocSetup->prev_c_ch_fmt = (pYuvOut->DataFmt == AMBA_DSP_YUV420)? DSP_YUV_420: DSP_YUV_422;
        pVprocSetup->prev_c_w = pYuvOut->Window.Width;
        pVprocSetup->prev_c_h = pYuvOut->Window.Height;
    } else {
        pVprocSetup->is_prev_c_enabled = (UINT8)0U;
        pVprocSetup->prev_c_format = VPROC_PREV_FMT_PROG;
        pVprocSetup->prev_c_src = VPROC_PREV_SRC_DRAM;
        pVprocSetup->prev_c_dst = AMBA_DSP_PREV_DEST_DUMMY;
        pVprocSetup->prev_c_frame_rate = 0U;
        pVprocSetup->prev_c_ch_fmt = DSP_YUV_420;
    }

    pVprocSetup->is_prev_hier_enabled = (UINT8)0U;
    pVprocSetup->is_prev_ln_enabled = (UINT8)0U;
    pVprocSetup->ln_det_src = 0U;

    pVprocSetup->is_c2y_burst_tiling_out = (UINT8)0U;
    pVprocSetup->is_hier_burst_tiling_out = (UINT8)0U;

    pVprocSetup->is_aaa_enabled = (UINT8)0U;
    pVprocSetup->cfg_aaa_by_ae_y = (pVprocSetup->is_tile_mode > 0U)? 1U: 0U;
    pVprocSetup->aaa_cfa_mux_sel = 0U; //TBD
    pVprocSetup->is_dzoom_enabled = 0U; //No Dzoom in YuvInput

    pVprocSetup->is_fast_y2y_en = (UINT8)0U;
    pVprocSetup->is_compressed_out_enabled = (UINT8)0U;
    pVprocSetup->is_hdr_blend_dram_out_enabled = (UINT8)0U;
    pVprocSetup->is_warp_dram_out_enabled = (UINT8)0U;

    pVprocSetup->is_mctf_enabled = (UINT8)0U;
    pVprocSetup->is_mctf_ref_win_mode = 0U;
    pVprocSetup->mctf_mode = VPROC_MCTF_MODE_FILTER;
    pVprocSetup->mctf_chan = VPROC_MCTF_CAHN_YUV;
    pVprocSetup->mcts_chan = VPROC_MCTS_CAHN_LUMA;
    pVprocSetup->is_mcts_dram_out_enabled = 0U;

    if (pVprocSetup->is_tile_mode > 0U) {
        UINT16 C2YInTileX = 0U, C2YTileX = 0U, MctfTileX = 0U;

        /* Forbid MainResampling in Y2Y */
        Rval = HL_CalcVideoTileViewZone(ViewZoneId, pYuvIn->Window.Width,
                                        VprocInfo.MaxTileWidth, 0U,
                                        &C2YInTileX);
        if (Rval == OK) {
            Rval = HL_CalcVideoTileViewZone(ViewZoneId, pYuvIn->Window.Width,
                                            VprocInfo.MaxTileWidth, 0U,
                                            &C2YTileX);
            if (Rval == OK) {
                Rval = HL_CalcVideoTileViewZone(ViewZoneId, pYuvIn->Window.Width,
                                                VprocInfo.MaxTileWidth, 0U,
                                                &MctfTileX);
                if (Rval == OK) {
                    C2YTileX = MAX2_16(C2YInTileX, C2YTileX);
                    pVprocSetup->c2y_tile_num_x = (UINT8)C2YTileX;
                    pVprocSetup->c2y_tile_num_y = 1U;
                    pVprocSetup->warp_tile_num_x = (UINT8)MctfTileX;
                    pVprocSetup->warp_tile_num_y = 1U;
                }
            }
        }
    } else {
        pVprocSetup->c2y_tile_num_x = 1U;
        pVprocSetup->c2y_tile_num_y = 1U;
        pVprocSetup->warp_tile_num_x = 1U;
        pVprocSetup->warp_tile_num_y = 1U;
    }
    if (Rval == OK) {
        pVprocSetup->raw_tile_overlap_x = TILE_OVERLAP_WIDTH;
        pVprocSetup->warp_tile_overlap_x = TILE_OVERLAP_WIDTH;
        pVprocSetup->warp_tile_overlap_y = 0U;

        pVprocSetup->is_mipi_yuyv_enabled = 0U;
        pVprocSetup->is_pic_info_cache_enabled = 0U;
        pVprocSetup->is_h_slice_low_delay = 0U;
#ifdef SUPPORT_DSP_EXT_PIN_BUF
        DSP_GetDspVprocExtMemDescBuf(ViewZoneId, &pExtMemDesc);
        dsp_osal_typecast(&ULAddr, &pExtMemDesc);

        pVprocSetup->ext_mem_cfg_num = MAX_VPROC_EXT_MEM_TYPE_NUM;
        pExtMemDesc[DSP_VprocPinVprocMemTypeMap[pStlInfo->YuvInVprocPin]].enable = (UINT8)1U;
        pExtMemDesc[DSP_VprocPinVprocMemTypeMap[pStlInfo->YuvInVprocPin]].pool_buf_num = (UINT8)DEFAULT_EXT_MEM_POOL_VPROC_BUF_NUM;
        pExtMemDesc[DSP_VprocPinVprocMemTypeMap[pStlInfo->YuvInVprocPin]].max_daddr_slot = (UINT8)DEFAULT_EXT_MEM_DADDR_NUM;
        (void)dsp_osal_virt2cli(ULAddr, &pVprocSetup->ext_mem_cfg_addr);
#endif
        pVprocSetup->proc_mode = VPROC_MODE_STILL_LISO;
        pVprocSetup->num_of_exp = 1U;
    }
    return Rval;
}

static inline UINT16 HL_StlEncCtrCalExtMemBufH(ULONG BaseAddrY, ULONG BaseAddrUV, UINT16 Pitch, UINT16 WindowHeight)
{
    UINT16 BufHeight = 0U;

    if ((BaseAddrUV > 0U) && (BaseAddrUV >= BaseAddrY)) {
        BufHeight = (UINT16)((BaseAddrUV - BaseAddrY)/Pitch);
    } else {
        BufHeight = WindowHeight;
    }

    return BufHeight;
}

static inline UINT32 HL_StlY2ySetExtMemForVproc(UINT8 WriteMode, UINT8 VprocStatus, UINT16 ViewZoneId,
                                                const CTX_STILL_INFO_s *pStlInfo,
                                                const AMBA_DSP_YUV_IMG_BUF_s *pYuvIn,
                                                const AMBA_DSP_YUV_IMG_BUF_s *pYuvOut,
                                                UINT16 *pOutputPinMask)
{
    UINT32 Rval = OK;
    cmd_vproc_setup_t *VprocSetup = HL_DefCtxCmdBufPtrVpcSetup;
    cmd_vproc_set_ext_mem_t *VprocExtMem = HL_DefCtxCmdBufPtrVpcExtMem;
    UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
    cmd_vin_attach_event_to_raw_t *VinAttachEvent = HL_DefCtxCmdBufPtrVinAttRaw;
    UINT32 *pOutputBufTblAddr;
    UINT32 PhysAddr = 0U;
    const void *pVoid = NULL;
    ULONG ULAddr = 0x0U;

#ifndef SUPPORT_DSP_EXT_PIN_BUF
    HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
    dsp_osal_typecast(&VprocExtMem, &CmdBufferAddr);
    VprocExtMem->channel_id = ViewZoneId;
//FIXME with better Preview filter choice
    VprocExtMem->memory_type = (UINT8)HL_VprocPinVprocMemTypePoolMap[pStlInfo->YuvInVprocPin];
    VprocExtMem->allocation_type = VIN_EXT_MEM_ALLOC_TYPE_DISTINCT;
    VprocExtMem->allocation_mode = VIN_EXT_MEM_MODE_NEW;
    VprocExtMem->overflow_cntl = VIN_EXT_MEM_CTRL_CIRCULAR;
    VprocExtMem->num_frm_buf = MAX_EXT_MEM_BUF_NUM;
#if 0
    if (VprocExtMem->num_frm_buf > EXT_MEM_VPROC_MAX_NUM) {
        AmbaLL_LogUInt5("Only %d ExtMem supported", EXT_MEM_VPROC_MAX_NUM, 0U, 0U, 0U, 0U);
    }
#endif
    VprocExtMem->buf_pitch = ALIGN_NUM16(pYuvOut->Window.Width, (UINT16)DSP_BUF_ALIGNMENT);
    VprocExtMem->buf_width = pYuvOut->Window.Width;
    VprocExtMem->buf_height = pYuvOut->Window.Height;
    HL_GetPointerToDspExtStlBufArray(1U/*Y2Y*/, &pOutputBufTblAddr);
    (void)dsp_osal_virt2cli(pYuvOut->BaseAddrY, &PhysAddr);
    pOutputBufTblAddr[0U] = PhysAddr;
    dsp_osal_typecast(&pVoid, &pOutputBufTblAddr);
    dsp_osal_typecast(&ULAddr, &pOutputBufTblAddr);
    (void)dsp_osal_cache_clean(pVoid, sizeof(UINT32)*MAX_EXT_MEM_BUF_NUM);
    (void)dsp_osal_virt2cli(ULAddr, &VprocExtMem->buf_addr);
    VprocExtMem->img_width = pYuvOut->Window.Width;
    VprocExtMem->img_height = pYuvOut->Window.Height;
    VprocExtMem->chroma_format = (pYuvOut->DataFmt == AMBA_DSP_YUV420)? DSP_YUV_420: DSP_YUV_422;
    (void)AmbaHL_CmdVprocSetExtMem(WriteMode, VprocExtMem);
    HL_RelCmdBuffer(CmdBufferId);

    //VinAttached for VprocOuput
    HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
    dsp_osal_typecast(&VinAttachEvent, &CmdBufferAddr);
    VinAttachEvent->vin_id = (UINT8)(pStlInfo->YuvInVirtVinId + AMBA_DSP_MAX_VIN_NUM);
    VinAttachEvent->ch_id = (UINT8)0U; //TBD
//FIXME with better Preview filter choice
    DSP_SetBit(&VinAttachEvent->event_mask, HL_VprocPinVinAttachBitMap[pStlInfo->YuvInVprocPin]);
    DSP_SetU16Bit(pOutputPinMask, pStlInfo->YuvInVprocPin);
    VinAttachEvent->repeat_cnt = 0x10001;
    VinAttachEvent->time_duration_bet_rpts = 0U;
    Rval = AmbaHL_CmdVinAttachEventToRaw(WriteMode, VinAttachEvent);
    HL_RelCmdBuffer(CmdBufferId);
#endif


    //Vproc setup
#ifndef SUPPORT_DSP_EXT_PIN_BUF
    if (Rval == OK) {
#endif
        if ((VprocStatus == STL_VPROC_STATUS_IDLE) ||
            (VprocStatus == STL_VPROC_STATUS_CONFIG) ||
            (VprocStatus == STL_VPROC_STATUS_STOPPED)) {
            HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
            dsp_osal_typecast(&VprocSetup, &CmdBufferAddr);
            Rval = HL_FillVprocSetupInY2Y(VprocSetup, pYuvIn, pYuvOut, ViewZoneId, pStlInfo);
            if (Rval == OK) {
                Rval = AmbaHL_CmdVprocSetup(WriteMode, VprocSetup);
            }
            HL_RelCmdBuffer(CmdBufferId);
        }
#ifndef SUPPORT_DSP_EXT_PIN_BUF
    }
#endif
    //SetExtMem for Vproc
#ifdef SUPPORT_DSP_EXT_PIN_BUF
    if (Rval == OK) {
        UINT8 U8Val;

        HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
        dsp_osal_typecast(&VprocExtMem, &CmdBufferAddr);
        VprocExtMem->channel_id = (UINT8)ViewZoneId;
//FIXME with better Preview filter choice
        VprocExtMem->memory_type = (UINT8)HL_VprocPinVprocMemTypePoolMap[pStlInfo->YuvInVprocPin];
        VprocExtMem->allocation_type = VIN_EXT_MEM_ALLOC_TYPE_DISTINCT;
        VprocExtMem->allocation_mode = VIN_EXT_MEM_MODE_NEW;
        /* case 1, ssp control, fixed usage, always new */
        VprocExtMem->overflow_cntl = VIN_EXT_MEM_CTRL_CIRCULAR;
        VprocExtMem->num_frm_buf = MAX_EXT_MEM_BUF_NUM;
#if 0
        if (VprocExtMem->num_frm_buf > EXT_MEM_VPROC_MAX_NUM) {
            AmbaLL_LogUInt5("Only %d ExtMem supported", EXT_MEM_VPROC_MAX_NUM, 0U, 0U, 0U, 0U);
        }
#endif
        VprocExtMem->buf_pitch = ALIGN_NUM16(pYuvOut->Pitch, (UINT16)DSP_BUF_ALIGNMENT);
        VprocExtMem->buf_width = pYuvOut->Window.Width;
        VprocExtMem->buf_height = HL_StlEncCtrCalExtMemBufH(pYuvOut->BaseAddrY, pYuvOut->BaseAddrUV,
                                                            pYuvOut->Pitch, pYuvOut->Window.Height);
        HL_GetPointerToDspExtStlBufArray(1U/*Y2Y*/, &pOutputBufTblAddr);
        (void)dsp_osal_virt2cli(pYuvOut->BaseAddrY, &PhysAddr);
        pOutputBufTblAddr[0U] = PhysAddr;
        dsp_osal_typecast(&pVoid, &pOutputBufTblAddr);
        dsp_osal_typecast(&ULAddr, &pOutputBufTblAddr);
        (void)dsp_osal_cache_clean(pVoid, (UINT32)(sizeof(UINT32)*MAX_EXT_MEM_BUF_NUM));
        (void)dsp_osal_virt2cli(ULAddr, &VprocExtMem->buf_addr);
        VprocExtMem->img_width = pYuvOut->Window.Width;
        VprocExtMem->img_height = pYuvOut->Window.Height;
        VprocExtMem->chroma_format = (pYuvOut->DataFmt == AMBA_DSP_YUV420)? DSP_YUV_420: DSP_YUV_422;
        (void)AmbaHL_CmdVprocSetExtMem(WriteMode, VprocExtMem);
        HL_RelCmdBuffer(CmdBufferId);

        //VinAttached for VprocOuput
        U8Val = (UINT8)(pStlInfo->YuvInVirtVinId + AMBA_DSP_MAX_VIN_NUM);
        HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
        dsp_osal_typecast(&VinAttachEvent, &CmdBufferAddr);
        VinAttachEvent->vin_id = U8Val;
        VinAttachEvent->ch_id = (UINT8)0U; //TBD
//FIXME with better Preview filter choice
        DSP_SetBit(&VinAttachEvent->event_mask, HL_VprocPinVinAttachBitMap[pStlInfo->YuvInVprocPin]);
        DSP_SetU16Bit(pOutputPinMask, pStlInfo->YuvInVprocPin);
        VinAttachEvent->repeat_cnt = 0x10001;
        VinAttachEvent->time_duration_bet_rpts = 0U;
        Rval = AmbaHL_CmdVinAttachEventToRaw(WriteMode, VinAttachEvent);
        HL_RelCmdBuffer(CmdBufferId);
    }
#endif

    return Rval;
}

static inline UINT32 HL_Y2ySendInputDataForVin(UINT8 WriteMode, UINT16 ViewZoneId,
                                               CTX_STILL_INFO_s *pStlInfo,
                                               const AMBA_DSP_YUV_IMG_BUF_s *pYuvIn,
                                               const AMBA_DSP_YUV_IMG_BUF_s *pYuvOut,
                                               const AMBA_DSP_ISOCFG_CTRL_s *pIsoCfg,
                                               UINT16 OutputPinMask)
{
    UINT32 Rval;
    UINT32 *pBatchQAddr;
    const void *pVoid;
    dsp_cmd_t *pDspCmd = NULL;
    ULONG ULAddr = 0x0U;
    UINT32 StlProcId = 0U, NewWp = 0U, BatchCmdId = 0U;
    UINT16 IsExtMem = 1U;
    UINT8 BatchCmdNum = 0U, U8Val;
    process_idsp_iso_config_cmd_t *pIsoCfgCmd;
    UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
    cmd_vin_send_input_data_t *VinFeedData = HL_DefCtxCmdBufPtrVinSndData;
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;
    DSP_STL_PROC_JOB_ID_s StlProcJobId = {0};

    U8Val = (UINT8)(pStlInfo->YuvInVirtVinId + AMBA_DSP_MAX_VIN_NUM);
    HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
    dsp_osal_typecast(&VinFeedData, &CmdBufferAddr);
    VinFeedData->vin_id = U8Val;
    VinFeedData->send_data_dest = DSP_VIN_SEND_IN_DATA_DEST_VPROC;
    VinFeedData->input_data_type = (pYuvIn->DataFmt == AMBA_DSP_YUV420)? (UINT32)DSP_VIN_SEND_IN_DATA_YUV420: (UINT32)DSP_VIN_SEND_IN_DATA_YUV422;
    HL_GetPointerToDspStlBatchQ(1U/*Y2Y*/, &pBatchQAddr);
    dsp_osal_typecast(&pVoid, &pBatchQAddr);
    dsp_osal_typecast(&ULAddr, &pBatchQAddr);
    dsp_osal_typecast(&pIsoCfgCmd, &pBatchQAddr);
    dsp_osal_typecast(&pDspCmd, &pBatchQAddr);
    pIsoCfgCmd->cmd_code = PROCESS_ISO_CONFIG_CMD;
    pIsoCfgCmd->flag.iso_config_common_update = (UINT8)1U;
    pIsoCfgCmd->flag.iso_config_color_update = (UINT8)1U;
    pIsoCfgCmd->flag.iso_config_mctf_update = (UINT8)1U;
    pIsoCfgCmd->flag.iso_config_step1_update = (UINT8)1U;
    pIsoCfgCmd->flag.iso_config_aaa_update = (UINT8)1U;
    pIsoCfgCmd->sequence_id = 0U; //TBD
    pIsoCfgCmd->flag.iso_config_warp_update = (UINT8)1U;
    (void)dsp_osal_virt2cli(pIsoCfg->CfgAddress, &pIsoCfgCmd->iso_cfg_daddr);
    (void)dsp_osal_cache_clean(pVoid, (UINT32)sizeof(process_idsp_iso_config_cmd_t));
    BatchCmdNum++;
    pDspCmd[BatchCmdNum].cmd_code = MAX_BATCH_CMD_END;
    (void)dsp_osal_virt2cli(ULAddr, &VinFeedData->batch_cmd_set_info.addr);
    HL_GetViewZoneInfoLock(ViewZoneId, &ViewZoneInfo);
    /* Request BatchCmdQ buffer */
    Rval = DSP_ReqBuf(&ViewZoneInfo->BatchQPoolDesc, 1U, &NewWp, 1U/*FullnessCheck*/);
    if (Rval != OK) {
        HL_GetViewZoneInfoUnLock(ViewZoneId);
    } else {
        ViewZoneInfo->BatchQPoolDesc.Wp = NewWp;
        HL_GetViewZoneInfoUnLock(ViewZoneId);
        HL_GetBatchCmdId(ViewZoneId, (UINT16)ViewZoneInfo->BatchQPoolDesc.Wp, &BatchCmdId);

        HL_SetDspBatchQInfo(BatchCmdId, 1U/*AutoReset*/, BATCHQ_INFO_ISO_CFG, pIsoCfg->CfgIndex);
        Rval = HL_ComposeStlProcJobId(&StlProcJobId, (UINT16)pYuvOut->DataFmt, OutputPinMask, IsExtMem, ViewZoneId);
        if (Rval == OK) {
            (void)dsp_osal_memcpy(&StlProcId, &StlProcJobId, sizeof(UINT32));
            HL_SetDspBatchQInfo(BatchCmdId, 0U/*AutoReset*/, BATCHQ_INFO_STL_PROC, StlProcId);
            VinFeedData->batch_cmd_set_info.id = BatchCmdId;
            VinFeedData->batch_cmd_set_info.size = HL_GetBatchCmdNumber(ULAddr)*CMD_SIZE_IN_BYTE;
            Rval = AmbaHL_CmdVinSendInputData(WriteMode, VinFeedData);
            if (Rval == OK) {
                HL_GetStlInfo(HL_MTX_OPT_GET, pStlInfo);
#ifndef SUPPORT_VPROC_RT_RESTART
                if (pYuvIn->DataFmt == AMBA_DSP_YUV420) {
                    pStlInfo->YuvInVprocStatus = STL_VPROC_STATUS_RUN;
                } else {
                    pStlInfo->Yuv422InVprocStatus = STL_VPROC_STATUS_RUN;
                }
#else
                pStlInfo->YuvInVprocStatus = STL_VPROC_STATUS_RUN;
#endif
                HL_SetStlInfo(HL_MTX_OPT_SET, pStlInfo);
            }
        }
    }
    HL_RelCmdBuffer(CmdBufferId);

    return Rval;
}

/**
* Still Yuv2Yuv
* @param [in]  pYuvIn input yuv buffer
* @param [in]  pYuvOut output yuv buffer
* @param [in]  pIsoCfg Iso config structure
* @param [in]  Opt Y2Y option
* @param [out] pAttachedRawSeq attached raw sequence
* @return ErrorCode
*/
UINT32 dsp_still_yuv2yuv(const AMBA_DSP_YUV_IMG_BUF_s *pYuvIn,
                         const AMBA_DSP_YUV_IMG_BUF_s *pYuvOut,
                         const AMBA_DSP_ISOCFG_CTRL_s *pIsoCfg,
                         UINT32 Opt,
                         UINT64 *pAttachedRawSeq)
{
    UINT32 Rval;

    Rval = HL_StlY2yChk(pYuvIn,
                        pYuvOut,
                        pIsoCfg,
                        Opt,
                        pAttachedRawSeq);

    AmbaMisra_TouchUnused(pAttachedRawSeq);

    /* Body */
    if (Rval == OK) {
        UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
        UINT8 WriteMode;
        UINT8 VprocStatus;
        UINT16 ViewZoneId, OutputPinMask = 0U;
        UINT32 *pBufTblAddr = NULL;
        ULONG ULAddr = 0x0U;
        cmd_vproc_cfg_t *VprocCfg = HL_DefCtxCmdBufPtrVpc;
#ifdef SUPPORT_VPROC_RT_RESTART
        cmd_vproc_stop_t *VprocStop = HL_DefCtxCmdBufPtrVpcStop;
#endif

        CTX_STILL_INFO_s StlInfo = {0};

        HL_GetStlInfo(HL_MTX_OPT_ALL, &StlInfo);
#ifndef SUPPORT_VPROC_RT_RESTART
        ViewZoneId = (pYuvIn->DataFmt == AMBA_DSP_YUV420)? StlInfo.YuvInVprocId: StlInfo.Yuv422InVprocId;
        VprocStatus = (pYuvIn->DataFmt == AMBA_DSP_YUV420)? StlInfo.YuvInVprocStatus: StlInfo.Yuv422InVprocStatus;
#else
        ViewZoneId = StlInfo.YuvInVprocId;
        VprocStatus = StlInfo.YuvInVprocStatus;
#endif

        WriteMode = HL_GetVin2CmdNormalWrite(StlInfo.YuvInVirtVinId + AMBA_DSP_MAX_VIN_NUM);
        //FIXME, Binding manager
        Rval = HL_StlY2yBind(pYuvIn, &StlInfo, WriteMode);
        if (Rval != OK) {
            AmbaLL_LogUInt5("HL_StlY2yBind fail", 0U, 0U, 0U, 0U, 0U);
        }

        /* Stop Vproc if needed */
        /*
         * To have different InputFmt, There are few methods
         * Method #0
         *   - Each InputFmt occupied one vproc(raw/yuv422/yuv420), VprocCfg/Setup only once with MaxPossbile size on each vproc
         *   - Pros : no VprocStop/Restart can save same
         *   - Cons : extra vproc definetely occupued total vproc number and consume cmem/smem of ucode
         * Method #1
         *   - One vproc for raw/yuv422/yuv420, VprocCfg only be issued once with MaxPossbile size on each vproc, VprocStop/Setup every round
         *   - Pros : no VprocCfg can save some ucode process time, only one vproc can min. cmem/smem usage
         *   - Cons : VprocStop/Setup takes time to wait vproc status transit
         * Method #2
         *   - One vproc for raw/yuv422/yuv420, VprocStop/Config/Setup every round
         *   - Pros : Only one vproc can min. cmem/smem usage, VprocCfg every time can min. every round's cmem/seme usage
         *   - Cons : VprocStop/Config/Setup takes more time(compare to #1) to wait vproc status transit
         * There we choose Method#1 as default value with Following reason
         *   - Since user already know worst case, we shall allocate worst resource with less ucode CPU cycle
         */
#ifndef SUPPORT_VPROC_RT_RESTART
        //Vproc always running
#else
        if (Rval == OK) {
            if ((VprocStatus == STL_VPROC_STATUS_COMPLETE) ||
                (StlInfo.RawInVprocStatus == STL_VPROC_STATUS_COMPLETE)) {

                HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
                dsp_osal_typecast(&VprocStop, &CmdBufferAddr);
                VprocStop->channel_id = (UINT8)ViewZoneId;
                VprocStop->stop_cap_seq_no = 0U; //obsoleted
                VprocStop->reset_option = 0U; //obsoleted
                Rval = AmbaHL_CmdVprocStop(WriteMode, VprocStop);
                HL_RelCmdBuffer(CmdBufferId);
                if (Rval == OK) {
                    Rval = DSP_WaitVprocState((UINT8)ViewZoneId, DSP_VPROC_STATUS_TIMER, MODE_SWITCH_TIMEOUT, 1U);
                }
                if (Rval == OK) {
#ifdef SUPPORT_VPROC_RT_RESTART_MTH_1
                    HL_GetStlInfo(HL_MTX_OPT_GET, &StlInfo);
                    StlInfo.YuvInVprocStatus = STL_VPROC_STATUS_CONFIG;
                    if (StlInfo.RawInVprocStatus == STL_VPROC_STATUS_COMPLETE) {
                        StlInfo.RawInVprocStatus = STL_VPROC_STATUS_CONFIG;
                    }
                    HL_SetStlInfo(HL_MTX_OPT_SET, &StlInfo);
                    VprocStatus = STL_VPROC_STATUS_CONFIG;
#else //method2
                    HL_GetStlInfo(HL_MTX_OPT_GET, &StlInfo);
                    StlInfo.YuvInVprocStatus = STL_VPROC_STATUS_IDLE;
                    if (StlInfo.RawInVprocStatus == STL_VPROC_STATUS_COMPLETE) {
                        StlInfo.RawInVprocStatus = STL_VPROC_STATUS_IDLE;
                    }
                    HL_SetStlInfo(HL_MTX_OPT_SET, &StlInfo);
                    VprocStatus = STL_VPROC_STATUS_IDLE;
#endif
                }
            }
        }
#endif

        /* Activate Vproc */
        //Vproc Config
        if (Rval == OK) {
            if (VprocStatus == STL_VPROC_STATUS_IDLE) {
                HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
                dsp_osal_typecast(&VprocCfg, &CmdBufferAddr);
                Rval = HL_StillVprocConfig(VprocCfg, ViewZoneId, pYuvIn->DataFmt, 1U, WriteMode);
                HL_RelCmdBuffer(CmdBufferId);
            }
        }

        if (Rval == OK) {
            Rval = HL_StlY2ySetExtMemForVproc(WriteMode, VprocStatus, ViewZoneId,
                                              &StlInfo, pYuvIn, pYuvOut, &OutputPinMask);
        }

        //SetExtMem for Vin
//FIXME with API
        if (Rval == OK) {
            UINT8 U8Val = (UINT8)(StlInfo.YuvInVirtVinId + AMBA_DSP_MAX_VIN_NUM);
            cmd_vin_set_ext_mem_t *VinExtMem = HL_DefCtxCmdBufPtrVinExtMem;

            HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
            dsp_osal_typecast(&VinExtMem, &CmdBufferAddr);
            VinExtMem->vin_id = U8Val;
            VinExtMem->memory_type = DSP_VIN_EXT_MEM_TYPE_YUV422;
            VinExtMem->allocation_type = VIN_EXT_MEM_ALLOC_TYPE_DISTINCT;
            VinExtMem->allocation_mode = VIN_EXT_MEM_MODE_NEW;
            /* case 1, ssp control, fixed usage, always new */
            VinExtMem->overflow_cntl = VIN_EXT_MEM_CTRL_CIRCULAR;
            VinExtMem->num_frm_buf = MAX_EXT_MEM_BUF_NUM;
#if 0
            if (VinExtMem->num_frm_buf > EXT_MEM_VIN_MAX_NUM) {
                AmbaLL_LogUInt5("Only %d ExtMem supported", EXT_MEM_VIN_MAX_NUM, 0U, 0U, 0U, 0U);
            }
#endif
            VinExtMem->chroma_format = (pYuvIn->DataFmt == AMBA_DSP_YUV420)? DSP_YUV_420: DSP_YUV_422;
            VinExtMem->buf_pitch = pYuvIn->Pitch;
            VinExtMem->buf_width = pYuvIn->Window.Width;
            VinExtMem->buf_height = HL_StlEncCtrCalExtMemBufH(pYuvIn->BaseAddrY, pYuvIn->BaseAddrUV,
                                                             pYuvIn->Pitch, pYuvIn->Window.Height);
            /* To make life easier, uCode only use Yuv422 buffer size */
            /* Per YK 20180719, BufH == ImgH when YuvFormat */
            VinExtMem->img_width = pYuvIn->Window.Width;
            VinExtMem->img_height = pYuvIn->Window.Height;

            /*
             * vin_yuv_enc_fbp_init will allocate resource for virtual vin yuv input from ext_mem at vin_flow_max stage
             * Need to allocate resource (vin_yuv_enc_fbp_disable=0) when send ext_mem after vproc
             * */
            VinExtMem->vin_yuv_enc_fbp_disable = 0U;
            HL_GetPointerToDspExtRawBufArray((UINT16)StlInfo.YuvInVirtVinId + AMBA_DSP_MAX_VIN_NUM, 0U/*Raw*/, &pBufTblAddr);
            (void)dsp_osal_virt2cli(pYuvIn->BaseAddrY, &pBufTblAddr[0U]);
            dsp_osal_typecast(&ULAddr, &pBufTblAddr);
            (void)dsp_osal_virt2cli(ULAddr, &VinExtMem->buf_addr);
            Rval = AmbaHL_CmdVinSetExtMem(WriteMode, VinExtMem);
            HL_RelCmdBuffer(CmdBufferId);
        }

        //Send Input Data for Vin
        if (Rval == OK) {
            Rval = HL_Y2ySendInputDataForVin(WriteMode, ViewZoneId, &StlInfo, pYuvIn, pYuvOut, pIsoCfg, OutputPinMask);
        }
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("AmbaDSP_StillYuv2Yuv Error[0x%X]", Rval, 0U, 0U, 0U, 0U);
    }

    return Rval;
}

static inline UINT32 HL_StlEncCtrlChk(const UINT16 StreamIdx,
                                      const AMBA_DSP_STLENC_CTRL_s *pStlEncCtrl,
                                      const UINT64 *pAttachedRawSeq)
{
    UINT32 Rval;
    CTX_STILL_INFO_s StlInfo = {0};
    const Dsp_DumpApi_t* DumpApi = AmbaDSP_GetDumpApiFunc();
    const Dsp_CheckApi_t* CheckApi = AmbaDSP_GetCheckApiFunc();
    const Dsp_ResourceApi_t* ResourceApi = AmbaDSP_GetResourceApiFunc();
    const Dsp_TimingApi_t* TimingApi = AmbaDSP_GetTimingApiFunc();

    /* DSP CtxInit */
    Rval = HL_CtxInit();

    /* Timing sanity check */
    if ((Rval == OK) &&
        (TimingApi != NULL)) {
        Rval = TimingApi->pStillEncodeCtrl(StreamIdx, pStlEncCtrl, pAttachedRawSeq);
    }

    /* Input sanity check */
    if ((Rval == OK) && (DumpApi != NULL)) {
        DumpApi->pStillEncodeCtrl(StreamIdx, pStlEncCtrl, pAttachedRawSeq);
    }

    if ((Rval == OK) && (CheckApi != NULL)) {
        Rval = CheckApi->pStillEncodeCtrl(StreamIdx, pStlEncCtrl, pAttachedRawSeq);
    }

    /* HW/SW resources check */
    if ((Rval == OK) && (ResourceApi != NULL)) {
        Rval = ResourceApi->pStillEncodeCtrl(StreamIdx, pStlEncCtrl, pAttachedRawSeq);
    }

    /* Logic sanity check */
    HL_GetStlInfo(HL_MTX_OPT_ALL, &StlInfo);
    if (StlInfo.YuvEncVirtVinId == DSP_VIRT_VIN_IDX_INVALID) {
        Rval = DSP_ERR_0004;
        AmbaLL_LogUInt5("YuvEncVirtVin not exist", 0U, 0U, 0U, 0U, 0U);
    }

    return Rval;
}

static inline UINT32 HL_StlEncCtrlBindVirtVinToEnc(UINT8 *pWriteMode, UINT16 StreamIdx, const CTX_STILL_INFO_s *pStlInfo)
{
    UINT32 Rval;
    UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
    cmd_binding_cfg_t *FpBindCfg = HL_DefCtxCmdBufPtrBind;

//FIXME, Binding manager
    /* UnBind first */
    HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
    dsp_osal_typecast(&FpBindCfg, &CmdBufferAddr);
    Rval = HL_FillFpBindSetup(FpBindCfg,
                              BIND_STILL_ENCODE,
                              0U/*IsBind*/,
                              pStlInfo->YuvEncVirtVinId + AMBA_DSP_MAX_VIN_NUM,
                              0U/*SrcPinId*/,
                              StreamIdx);
    if (Rval != OK) {
        AmbaLL_LogUInt5("[%d]FpBind filling fail", __LINE__, 0U, 0U, 0U, 0U);
    } else {
        *pWriteMode = HL_GetVin2CmdNormalWrite(pStlInfo->YuvEncVirtVinId + AMBA_DSP_MAX_VIN_NUM);
        Rval = AmbaHL_CmdDspBindingCfg(*pWriteMode, FpBindCfg);
    }
    HL_RelCmdBuffer(CmdBufferId);

    /* Bind VirtVin to Enc */
    if (Rval == OK) {
        HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
        dsp_osal_typecast(&FpBindCfg, &CmdBufferAddr);
        Rval = HL_FillFpBindSetup(FpBindCfg,
                                  BIND_STILL_ENCODE,
                                  1U/*IsBind*/,
                                  pStlInfo->YuvEncVirtVinId + AMBA_DSP_MAX_VIN_NUM,
                                  0U/*SrcPinId*/,
                                  StreamIdx);
        if (Rval != OK) {
            AmbaLL_LogUInt5("[%d]FpBind filling fail", __LINE__, 0U, 0U, 0U, 0U);
        } else {
            *pWriteMode = HL_GetVin2CmdNormalWrite(pStlInfo->YuvEncVirtVinId + AMBA_DSP_MAX_VIN_NUM);
            Rval = AmbaHL_CmdDspBindingCfg(*pWriteMode, FpBindCfg);
        }
        HL_RelCmdBuffer(CmdBufferId);
    }

    return Rval;
}

static inline UINT8 HL_StlEncCtrGetEncRotate(UINT8 RotateFlip, UINT8 DataFmt)
{
    UINT8 EncRotate = 0U;

    if ((HL_IS_ROTATE(RotateFlip) > 0U) && (DataFmt == AMBA_DSP_YUV420)) {
        EncRotate = 1U;
    }

    return EncRotate;
}

/**
* Still Encode Control
* @param [in]  StreamIdx strean index
* @param [in]  pStlEncCtrl encode control setting
* @param [out] pAttachedRawSeq attached raw sequence
* @return ErrorCode
*/
UINT32 dsp_still_encode_ctrl(UINT16 StreamIdx,
                             const AMBA_DSP_STLENC_CTRL_s *pStlEncCtrl,
                             UINT64 *pAttachedRawSeq)
{
    UINT8 WriteMode = AMBA_DSP_CMD_VDSP_NORMAL_WRITE;
    UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
    UINT32 Rval;
    UINT32 PhysAddr = 0x0U, NumOfMCUs;
    UINT32 *pBufTblAddr = NULL;
    ULONG ULAddr = 0U;
    CTX_STILL_INFO_s StlInfo = {0};
    CTX_STREAM_INFO_s StrmInfo;
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;
    cmd_encoder_start_t *EncStart = HL_DefCtxCmdBufPtrEncStart;
    cmd_encoder_jpeg_setup_t *JpgSetup = HL_DefCtxCmdBufPtrEncJpg;
    cmd_vin_send_input_data_t *VinFeedData = HL_DefCtxCmdBufPtrVinSndData;
    AMBA_DSP_FRAME_RATE_s FrameRate;

    Rval = HL_StlEncCtrlChk(StreamIdx, pStlEncCtrl, pAttachedRawSeq);
    AmbaMisra_TouchUnused(pAttachedRawSeq);

    /* Body */
    if (Rval == OK) {
        HL_GetStlInfo(HL_MTX_OPT_ALL, &StlInfo);
        HL_GetResourcePtr(&Resource);

        /* Update encoder status */
        HL_GetStlInfo(HL_MTX_OPT_GET, &StlInfo);
        StlInfo.EncStatus = STL_ENC_STATUS_CONFIG;
        HL_SetStlInfo(HL_MTX_OPT_SET, &StlInfo);

        Rval = HL_StlEncCtrlBindVirtVinToEnc(&WriteMode, StreamIdx, &StlInfo);

        if (Rval == OK) {
            HL_GetStrmInfo(HL_MTX_OPT_GET, StreamIdx, &StrmInfo);
            /* Use IsMJPG in StrmInfo as an indicator of still-encode */
            StrmInfo.IsMJPG = (UINT8)0U;
            HL_SetStrmInfo(HL_MTX_OPT_SET, StreamIdx, &StrmInfo);

            /* EncodeSetup */
            HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
            dsp_osal_typecast(&JpgSetup, &CmdBufferAddr);
            JpgSetup->stream_id = (UINT8)StreamIdx;
            JpgSetup->coding_type = DSP_ENC_FMT_JPEG;
            JpgSetup->enc_start_method = DSP_ENC_START_FROM_PICINFO; //VirtualVin Feed
            JpgSetup->enc_stop_method = DSP_ENC_STOP_FROM_PICINFO; //VirtualVin Feed
            JpgSetup->chroma_format = (pStlEncCtrl->YuvBuf.DataFmt == AMBA_DSP_YUV420)? DSP_YUV_420: DSP_YUV_422;
            JpgSetup->is_mjpeg = (UINT8)1U; // for rotate-flip
            JpgSetup->enc_hflip = HL_IS_HFLIP(pStlEncCtrl->RotateFlip);
            JpgSetup->enc_vflip = HL_IS_VFLIP(pStlEncCtrl->RotateFlip);

            JpgSetup->enc_rotation = HL_StlEncCtrGetEncRotate(pStlEncCtrl->RotateFlip, pStlEncCtrl->YuvBuf.DataFmt);
            JpgSetup->initial_qlevel = (UINT8)pStlEncCtrl->QualityLevel;
            (void)dsp_osal_virt2cli(pStlEncCtrl->QTblAddr, &JpgSetup->dqt_daddr);

            (void)dsp_osal_virt2cli(pStlEncCtrl->BitsBufAddr, &JpgSetup->bits_fifo_base);
            JpgSetup->bits_fifo_size = pStlEncCtrl->BitsBufSize;
            if ((pStlEncCtrl->BitsBufAddr > 0U) && (pStlEncCtrl->BitsBufSize > 0U)) {
                LL_SetMsgDispatcherBitsInfo(pStlEncCtrl->BitsBufAddr, pStlEncCtrl->BitsBufSize);
            }
            JpgSetup->bits_fifo_offset = 0U;

            DSP_GetBitsDescBufferAddr(&ULAddr);
            (void)dsp_osal_virt2cli(ULAddr, &JpgSetup->info_fifo_base);
            JpgSetup->info_fifo_size = AMBA_DSP_VIDEOENC_DESC_BUF_SIZE;

            //NotSupport
            JpgSetup->target_bpp = 0U;
            JpgSetup->tolerance = 0U;
            JpgSetup->max_reenc_loops = 0U;
            JpgSetup->rct_sample_num = 0U;
            JpgSetup->rct_daddr = 0U;

            //Obsolete
            JpgSetup->stream_type = 0U;
            JpgSetup->enc_src = 0U;
            JpgSetup->efm_enable = 0U;

            //MJPG?
            /* Set restart_interval to 65536, the maximal MCU number ucode can handle,
             * when the total MCU number is greater than that */
            NumOfMCUs = ((UINT32)Resource->MaxStlYuvEncWidth/16U)*((UINT32)Resource->MaxStlYuvEncHeight/\
                        (8U*((pStlEncCtrl->YuvBuf.DataFmt == AMBA_DSP_YUV422)? (UINT32)1UL: (UINT32)2UL)));
            JpgSetup->restart_interval = (NumOfMCUs > 65535U)? 65535U: 0U;

            /* 2022/08/04, fill still encode frm-rate as 1fps to make time bomb work */
            FrameRate.Interlace = (UINT8)0U;
            FrameRate.TimeScale = 1000U;
            FrameRate.NumUnitsInTick = 1000U;
            (void)HL_ConvertDspFrameRate(FrameRate, &JpgSetup->frame_rate);

            /* Note: If enc_rotation is enabled, YUV image and buffer's height must be 16-aligned
             * or there will be a garbage line on the vertical margin */
            if (JpgSetup->enc_rotation == 1U) {
                JpgSetup->encode_w = pStlEncCtrl->YuvBuf.Window.Height; //HL adapt PicInfo from Vin
                JpgSetup->encode_h = pStlEncCtrl->YuvBuf.Window.Width; //HL adapt PicInfo from Vin
                JpgSetup->encode_h_ofs = 0U;
                if (JpgSetup->enc_hflip == 0U) {
                    JpgSetup->encode_h_ofs = (UINT16)(pStlEncCtrl->YuvBuf.Window.Height - ALIGN_NUM16(pStlEncCtrl->YuvBuf.Window.Height, 16U));
                }
            } else {
                JpgSetup->encode_w = pStlEncCtrl->YuvBuf.Window.Width; //HL adapt PicInfo from Vin
                JpgSetup->encode_h = pStlEncCtrl->YuvBuf.Window.Height; //HL adapt PicInfo from Vin
            }
            JpgSetup->encode_w_ofs = 0U;
            Rval = AmbaHL_CmdEncoderJpegSetup(WriteMode, JpgSetup);
            HL_RelCmdBuffer(CmdBufferId);
        }

        /* 2020/10/16 ucoder(ChenHan) suggest to issue enc-start even in jpg encode case for a complete encode round */
        if (Rval == OK) {
            HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
            dsp_osal_typecast(&EncStart, &CmdBufferAddr);
            EncStart->stream_id = (UINT8)StreamIdx;
            Rval = AmbaHL_CmdEncoderStart(WriteMode, EncStart);
            HL_RelCmdBuffer(CmdBufferId);
        }

        //SetExtMem for Vin
//FIXME with API
        if (Rval == OK) {
            UINT8 U8Val = (UINT8)(StlInfo.YuvEncVirtVinId + AMBA_DSP_MAX_VIN_NUM);
            cmd_vin_set_ext_mem_t *VinExtMem = HL_DefCtxCmdBufPtrVinExtMem;

            HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
            dsp_osal_typecast(&VinExtMem, &CmdBufferAddr);
            VinExtMem->vin_id = U8Val;
            VinExtMem->memory_type = DSP_VIN_EXT_MEM_TYPE_YUV422;
            VinExtMem->allocation_type = VIN_EXT_MEM_ALLOC_TYPE_DISTINCT;
            VinExtMem->allocation_mode = VIN_EXT_MEM_MODE_NEW;
            /* case 1, ssp control, fixed usage, always new */
            VinExtMem->overflow_cntl = VIN_EXT_MEM_CTRL_CIRCULAR;
            VinExtMem->num_frm_buf = MAX_EXT_MEM_BUF_NUM;
#if 0
            if (VinExtMem->num_frm_buf > EXT_MEM_VIN_MAX_NUM) {
                AmbaLL_LogUInt5("Only %d ExtMem supported", EXT_MEM_VIN_MAX_NUM, 0U, 0U, 0U, 0U);
            }
#endif
            VinExtMem->chroma_format = (pStlEncCtrl->YuvBuf.DataFmt == AMBA_DSP_YUV420)? DSP_YUV_420: DSP_YUV_422;
            VinExtMem->buf_pitch = pStlEncCtrl->YuvBuf.Pitch;
            VinExtMem->buf_width = pStlEncCtrl->YuvBuf.Window.Width;
            VinExtMem->buf_height = HL_StlEncCtrCalExtMemBufH(pStlEncCtrl->YuvBuf.BaseAddrY, pStlEncCtrl->YuvBuf.BaseAddrUV,
                                                             pStlEncCtrl->YuvBuf.Pitch, pStlEncCtrl->YuvBuf.Window.Height);
            VinExtMem->img_width = pStlEncCtrl->YuvBuf.Window.Width;
            VinExtMem->img_height = pStlEncCtrl->YuvBuf.Window.Height;

            /*
             * vin_yuv_enc_fbp_init will allocate resource for virtual vin yuv input from ext_mem at vin_flow_max stage
             * Need to allocate resource (vin_yuv_enc_fbp_disable=0) when send ext_mem after vproc
             * */
            VinExtMem->vin_yuv_enc_fbp_disable = 0U;

            HL_GetPointerToDspExtRawBufArray(StlInfo.YuvEncVirtVinId + AMBA_DSP_MAX_VIN_NUM, 0U/*Raw*/, &pBufTblAddr);
            (void)dsp_osal_virt2cli(pStlEncCtrl->YuvBuf.BaseAddrY, &PhysAddr);
            pBufTblAddr[0U] = PhysAddr;
            dsp_osal_typecast(&ULAddr, &pBufTblAddr);
            (void)dsp_osal_virt2cli(ULAddr, &VinExtMem->buf_addr);
            Rval = AmbaHL_CmdVinSetExtMem(WriteMode, VinExtMem);
            HL_RelCmdBuffer(CmdBufferId);
        }

        //Send Input Data for Vin
//FIXME with API
        if (Rval == OK) {
            UINT8 U8Val = (UINT8)((UINT8)StlInfo.YuvEncVirtVinId + (UINT8)AMBA_DSP_MAX_VIN_NUM);

            HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
            dsp_osal_typecast(&VinFeedData, &CmdBufferAddr);
            VinFeedData->vin_id = U8Val;
            VinFeedData->send_data_dest = DSP_VIN_SEND_IN_DATA_DEST_ENC;
            VinFeedData->encode_start_idc = (UINT8)1U;
            VinFeedData->encode_stop_idc = (UINT8)1U;
            VinFeedData->ext_fb_idx = 0U; // indicate which index in VinSetExtMemBufTbl
            VinFeedData->ext_ce_fb_idx = 0xFFFFFFFFU; // 0xFFFFFFFF means no such buffer, used when JPG encode
            VinFeedData->input_data_type = (pStlEncCtrl->YuvBuf.DataFmt == AMBA_DSP_YUV420)? (UINT32)DSP_VIN_SEND_IN_DATA_YUV420: (UINT32)DSP_VIN_SEND_IN_DATA_YUV422;
            Rval = AmbaHL_CmdVinSendInputData(WriteMode, VinFeedData);
            HL_RelCmdBuffer(CmdBufferId);
            if (Rval == OK) {
                HL_GetStlInfo(HL_MTX_OPT_GET, &StlInfo);
                StlInfo.EncStatus = STL_ENC_STATUS_RUN;
                HL_SetStlInfo(HL_MTX_OPT_SET, &StlInfo);
            }
        }
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("AmbaDSP_StillEncodeCtrl Error[0x%X]", Rval, 0U, 0U, 0U, 0U);
    }

    return Rval;
}

static inline UINT32 HL_CalcStillYuvExtBufSizeChk(const UINT16 StreamIdx,
                                                  const UINT16 BufType,
                                                  const UINT16 *pBufPitch,
                                                  const UINT32 *pBufUnitSize)
{
    UINT32 Rval;
    const Dsp_DumpApi_t* DumpApi = AmbaDSP_GetDumpApiFunc();
    const Dsp_CheckApi_t* CheckApi = AmbaDSP_GetCheckApiFunc();

    /* DSP CtxInit */
    Rval = HL_CtxInit();

    /* Timing sanity check */
    /* Input sanity check */
    if ((Rval == OK) &&
        (DumpApi != NULL)) {
        DumpApi->pCalcStillYuvExtBufSize(StreamIdx, BufType, pBufPitch, pBufUnitSize);
    }
    if ((Rval == OK) &&
        (CheckApi != NULL)) {
        Rval = CheckApi->pCalcStillYuvExtBufSize(StreamIdx, BufType, pBufPitch, pBufUnitSize);
    }
    return Rval;
}

UINT32 dsp_calc_still_yuv_extbuf_size(UINT16 StreamIdx, UINT16 BufType, UINT16 *pBufPitch, UINT32 *pBufUnitSize)
{
    UINT32 Rval;

    Rval = HL_CalcStillYuvExtBufSizeChk(StreamIdx, BufType, pBufPitch, pBufUnitSize);

    if (Rval == OK) {
        if (BufType == DSP_DATACAP_BUFTYPE_YUV) {
            /* Luma + Chroma Buffer */
            AMBA_DSP_BUF_s CalcLumaBuf = {0U};

            Rval = HL_StillCaptureCalcExtBufImpl(StreamIdx, DSP_DATACAP_CALTYPE_LUMA, &CalcLumaBuf);
            if (Rval == OK) {
                *pBufPitch = CalcLumaBuf.Pitch;
                *pBufUnitSize = (UINT32)(CalcLumaBuf.Pitch)*(CalcLumaBuf.Window.Height);
                *pBufUnitSize = ((*pBufUnitSize)*3U)/2U; // Luma + Chroma
                *pBufUnitSize = ALIGN_NUM(*pBufUnitSize, (UINT16)DSP_BUF_ALIGNMENT);
            }
        } else if (BufType == DSP_DATACAP_BUFTYPE_AUX) {
            /* ME1 + ME0 Buffer */
            AMBA_DSP_BUF_s CalcMe1Buf = {0U}, CalcMe0Buf = {0U};

            Rval = HL_StillCaptureCalcExtBufImpl(StreamIdx, DSP_DATACAP_CALTYPE_ME1, &CalcMe1Buf);
            if (Rval == OK) {
                Rval = HL_StillCaptureCalcExtBufImpl(StreamIdx, DSP_DATACAP_CALTYPE_ME0, &CalcMe0Buf);
                if (Rval == OK) {
                    *pBufPitch = CalcMe1Buf.Pitch;
                    *pBufUnitSize = (((UINT32)(CalcMe1Buf.Pitch)*(CalcMe1Buf.Window.Height)) +
                                     ((UINT32)(CalcMe0Buf.Pitch)*(CalcMe0Buf.Window.Height)));
                    *pBufUnitSize = ALIGN_NUM(*pBufUnitSize, (UINT16)DSP_BUF_ALIGNMENT);
                }
            }
        } else if (BufType == DSP_DATACAP_BUFTYPE_COMP_RAW) {
            AMBA_DSP_BUF_s CalcRawBuf = {0U};

            Rval = HL_StillCaptureCalcExtBufImpl(StreamIdx, DSP_DATACAP_CALTYPE_PROCRAW, &CalcRawBuf);
            if (Rval == OK) {
                *pBufPitch = CalcRawBuf.Pitch;
                *pBufUnitSize = (((UINT32)(CalcRawBuf.Pitch)*(CalcRawBuf.Window.Height)) +
                                 ((UINT32)(CalcRawBuf.Pitch)*(CalcRawBuf.Window.Height)));
                *pBufUnitSize = ALIGN_NUM(*pBufUnitSize, (UINT16)DSP_BUF_ALIGNMENT);
            }
        } else {
            Rval = DSP_ERR_0001;
        }
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("AmbaDSP_CalcStillYuvExtBufSize Error[0x%X]", Rval, 0U, 0U, 0U, 0U);
    }

    return Rval;
}

static inline void HL_StlProcUnbind(const UINT8 RawVprocStatus,
                                    const UINT8 YuvInVprocStatus,
                                    const UINT8 WriteMode,
                                    const UINT16 ViewZoneId,
                                    const UINT16 YuvVinId,
                                    const UINT16 RawVinId,
                                    cmd_binding_cfg_t *pFpBindCfg)
{
    UINT32 Rval;

    /*
     * 20190614, Per ChenHan, Bind/Unbind shall go through VinCmdQ if relative VinFP.
     *           ucode may hit concurrent access if go through VoutCmdQ
     */
    /* UnBind first */
    if (RawVprocStatus == STL_VPROC_STATUS_COMPLETE) {
        Rval = HL_FillFpBindSetup(pFpBindCfg, BIND_STILL_PROC, 0U/*IsBind*/, RawVinId, 0U/*SrcPinId*/, ViewZoneId);
        if (Rval != OK) {
            AmbaLL_LogUInt5("[%d]FpBind filling fail", __LINE__, 0U, 0U, 0U, 0U);
        } else {
            (void)AmbaHL_CmdDspBindingCfg(WriteMode, pFpBindCfg);
        }
#ifdef SUPPORT_VPROC_RT_RESTART
    } else if (YuvInVprocStatus == STL_VPROC_STATUS_COMPLETE) {
        Rval = HL_FillFpBindSetup(pFpBindCfg, BIND_STILL_PROC, 0U/*IsBind*/, YuvVinId, 0U/*SrcPinId*/, ViewZoneId);
        if (Rval != OK) {
            AmbaLL_LogUInt5("[%d]FpBind filling fail", __LINE__, 0U, 0U, 0U, 0U);
        } else {
            (void)AmbaHL_CmdDspBindingCfg(WriteMode, pFpBindCfg);
        }
    } else {
        //TBD
#endif
    }
}

static inline void HL_StlProcbind(const UINT16 RawVinId,
                                  const UINT8 WriteMode,
                                  const UINT16 ViewZoneId,
                                  cmd_binding_cfg_t *pFpBindCfg)
{
    UINT32 Rval;

    Rval = HL_FillFpBindSetup(pFpBindCfg, BIND_STILL_PROC, 1U/*IsBind*/, RawVinId, 0U/*SrcPinId*/, ViewZoneId);
    if (Rval != OK) {
        AmbaLL_LogUInt5("[%d]FpBind filling fail", __LINE__, 0U, 0U, 0U, 0U);
    } else {
        (void)AmbaHL_CmdDspBindingCfg(WriteMode, pFpBindCfg);
    }
}

static inline void HL_FillSetExtHdsMemInR2y(cmd_vin_set_ext_mem_t *pVinExtMem,
                                            const AMBA_DSP_BUF_s *pAuxBufIn,
                                            const UINT16 VinId,
                                            const UINT16 InputHeight)
{
    UINT32 *pBufTblAddr = NULL;
    UINT32 PhysAddr = 0U;
    ULONG ULAddr = 0U;

    pVinExtMem->vin_id = (UINT8)VinId;
    pVinExtMem->memory_type = DSP_VIN_EXT_MEM_TYPE_HDS;
    pVinExtMem->allocation_type = VIN_EXT_MEM_ALLOC_TYPE_DISTINCT;
    pVinExtMem->allocation_mode = VIN_EXT_MEM_MODE_NEW;
    /* case 1, ssp control, fixed usage, always new */
    pVinExtMem->overflow_cntl = VIN_EXT_MEM_CTRL_CIRCULAR;
    pVinExtMem->num_frm_buf = MAX_EXT_MEM_BUF_NUM;
    pVinExtMem->chroma_format = DSP_YUV_MONO;
    pVinExtMem->luma_img_ofs_x = 0U;
    pVinExtMem->luma_img_ofs_y = 0U;
    pVinExtMem->chroma_img_ofs_x = 0U;
    pVinExtMem->chroma_img_ofs_y = 0U;
    pVinExtMem->img_width = 0U;
    pVinExtMem->img_height = 0U;

    pVinExtMem->aux_pitch = 0U;
    pVinExtMem->aux_width = 0U;
    pVinExtMem->aux_height = 0U;
    pVinExtMem->aux_img_ofs_x = 0U;
    pVinExtMem->aux_img_ofs_y = 0U;
    pVinExtMem->aux_img_width = 0U;
    pVinExtMem->aux_img_height = 0U;

    pVinExtMem->buf_pitch = pAuxBufIn->Pitch;
    pVinExtMem->buf_width = pAuxBufIn->Pitch;
    pVinExtMem->buf_height = InputHeight;

    /*
     * vin_yuv_enc_fbp_init will allocate resource for virtual vin yuv input from ext_mem at vin_flow_max stage
     * Need to allocate resource (vin_yuv_enc_fbp_disable=0) when send ext_mem after vproc
     * */
    pVinExtMem->vin_yuv_enc_fbp_disable = 0U;

    HL_GetPointerToDspExtRawBufArray(VinId, 1U/*Aux*/, &pBufTblAddr);
    (void)dsp_osal_virt2cli(pAuxBufIn->BaseAddr, &PhysAddr);
    pBufTblAddr[0U] = PhysAddr;
    dsp_osal_typecast(&ULAddr, &pBufTblAddr);
    (void)dsp_osal_virt2cli(ULAddr, &pVinExtMem->buf_addr);
}

static inline void HL_GetIsoModeFromIkId(UINT32 IkId, UINT8 *pIsoMode)
{
    const UINT32 *pU32Val = &IkId;
    const IK_ID_s *pIkId;

    dsp_osal_typecast(&pIkId, &pU32Val);
    *pIsoMode = pIkId->Ability;
}

static inline void HL_GetExposureNumFromIsoMode(UINT8 IsoMode, UINT8 *pExpNum)
{
    if (IsoMode == (UINT8)AMBA_IK_VIDEO_HDR_EXPO_2) {
        *pExpNum = 2U;
    } else if (IsoMode == (UINT8)AMBA_IK_VIDEO_HDR_EXPO_3) {
        *pExpNum = 3U;
    } else {
        *pExpNum = 1U;
    }
}

static inline UINT8 HL_IsHdrEnabled(UINT8 IsoMode)
{
    UINT8 Enabled = 0U;
    if ((IsoMode == (UINT8)AMBA_IK_VIDEO_HDR_EXPO_2) ||
        (IsoMode == (UINT8)AMBA_IK_VIDEO_HDR_EXPO_3) ||
        (IsoMode == (UINT8)AMBA_IK_VIDEO_LINEAR_CE)) {
        Enabled = 1U;
    } else {
        Enabled = 0U;
    }
    return Enabled;
}

static inline void HL_FillHdrSetupInR2y(cmd_vin_hdr_setup_t *pHdrSetup,
                                        const AMBA_DSP_ISOCFG_CTRL_s *pIsoCfg,
                                        const CTX_STILL_INFO_s *pStlInfo,
                                        UINT8 *pSkipCmd)
{
    UINT8 HdrExpNum, BlendNum;
    UINT8 IsoMode, U8Val;

    HL_GetIsoModeFromIkId(pIsoCfg->CfgIndex, &IsoMode);
    HL_GetExposureNumFromIsoMode(IsoMode, &HdrExpNum);

    if (HdrExpNum > 1U) {
        U8Val = (UINT8)(pStlInfo->RawInVirtVinId + AMBA_DSP_MAX_VIN_NUM);
        pHdrSetup->vin_id = U8Val;
        BlendNum = HdrExpNum - 1U;
        pHdrSetup->num_exp_minus_1 = BlendNum;
        pHdrSetup->num_hdr_blends = BlendNum; //obsolete in CV2/CV22
        pHdrSetup->aggregate_raw_frame_width = 0U; //obsolete in CV2/CV22
        pHdrSetup->aggregate_raw_frame_height = 0U; //obsolete in CV2/CV22
        pHdrSetup->source_width = 0U; //obsolete in CV2/CV22
        pHdrSetup->source_height = 0U; //obsolete in CV2/CV22
        pHdrSetup->bayer_pattern = 0U; //obsolete in CV2/CV22

        if (HdrExpNum == 2U) {
            pHdrSetup->exp_vert_offset_table[0U] = (UINT16)pStlInfo->IkDolOfstY[0U]; //LE
            pHdrSetup->exp_vert_offset_table[1U] = (UINT16)pStlInfo->IkDolOfstY[1U]; //SE
        } else if (HdrExpNum == 3U) {
            pHdrSetup->exp_vert_offset_table[0U] = (UINT16)pStlInfo->IkDolOfstY[0U]; //LE
            pHdrSetup->exp_vert_offset_table[1U] = (UINT16)pStlInfo->IkDolOfstY[1U]; //SE
            pHdrSetup->exp_vert_offset_table[2U] = (UINT16)pStlInfo->IkDolOfstY[2U]; //VSE
        } else {
            //TBD
        }
        *pSkipCmd = 0U;
    } else {
        *pSkipCmd = 1U;
    }
}

static inline void HL_SetAuxBuffer(const AMBA_DSP_RAW_BUF_s *pRawIn,
                                   const AMBA_DSP_BUF_s *pAuxBufIn,
                                   const AMBA_DSP_ISOCFG_CTRL_s *pIsoCfg,
                                   cmd_vin_set_ext_mem_t *pVinExtMem,
                                   const CTX_STILL_INFO_s *pStlInfo,
                                   const UINT16 InputHeight,
                                   const UINT8 WriteMode)
{
    if (pAuxBufIn->BaseAddr != 0x0U) {
        UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
        UINT8 SkipCmd = 1U, U8Val;
        cmd_vin_ce_setup_t *CeSetup = HL_DefCtxCmdBufPtrVinCe;
        cmd_vin_hdr_setup_t *HdrSetup = HL_DefCtxCmdBufPtrVinHdr;

        HL_FillSetExtHdsMemInR2y(pVinExtMem,
                                 pAuxBufIn,
                                 pStlInfo->RawInVirtVinId + AMBA_DSP_MAX_VIN_NUM,
                                 InputHeight);

        (void)AmbaHL_CmdVinSetExtMem(WriteMode, pVinExtMem);

        //CeSetup
        if (IsAligned4(pRawIn->Window.Width) == 1U) {
            U8Val = (UINT8)(pStlInfo->RawInVirtVinId + AMBA_DSP_MAX_VIN_NUM);
            HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
            dsp_osal_typecast(&CeSetup, &CmdBufferAddr);
            CeSetup->vin_id = U8Val;
            CeSetup->number_ce_exp = 0U; //obsolete in CV2/CV22
            CeSetup->primary_out_exp = 0U; //obsolete in CV2/CV22
            CeSetup->secondary_out_exp = 0U; //obsolete in CV2/CV22

            CeSetup->ce_width = pRawIn->Window.Width/4U; //Fixed 4x DownScale
            CeSetup->ce_height = InputHeight;
            CeSetup->ce_out_format = 0U; //obsolete in CV2/CV222
            (void)AmbaHL_CmdVinCeSetup(WriteMode, CeSetup);
            HL_RelCmdBuffer(CmdBufferId);
        }

        //HdrSetup
        HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
        dsp_osal_typecast(&HdrSetup, &CmdBufferAddr);
        HL_FillHdrSetupInR2y(HdrSetup, pIsoCfg, pStlInfo, &SkipCmd);
        if (SkipCmd == 0U) {
            (void)AmbaHL_CmdVinHdrSetup(WriteMode, HdrSetup);
        }
        HL_RelCmdBuffer(CmdBufferId);
    }
}

static inline void HL_CalcExpNumHeight(const AMBA_DSP_RAW_BUF_s *pRawIn,
                                       const AMBA_DSP_ISOCFG_CTRL_s *pIsoCfg,
                                       UINT8 *pHdrExpNum,
                                       UINT16 *pInputHeight,
                                       const CTX_STILL_INFO_s *pStlInfo)
{
    UINT8 HdrExpNum = 0U;
    UINT16 InputHeight = 0U;

    if (pIsoCfg != NULL) {
        UINT8 IsoMode;

        HL_GetIsoModeFromIkId(pIsoCfg->CfgIndex, &IsoMode);
        HL_GetExposureNumFromIsoMode(IsoMode, &HdrExpNum);

        if (HdrExpNum > 1U) {
            InputHeight = (pRawIn->Window.Height*HdrExpNum) + (UINT16)pStlInfo->IkDolOfstY[HdrExpNum - 1U] - 1U;
        } else {
            InputHeight = pRawIn->Window.Height;
        }
    }
    *pHdrExpNum = HdrExpNum;
    *pInputHeight = InputHeight;
}

static inline UINT32 HL_FillVprocSetupInR2y(cmd_vproc_setup_t *pVprocSetup,
                                            const AMBA_DSP_ISOCFG_CTRL_s *pIsoCfg,
                                            const AMBA_DSP_RAW_BUF_s *pRawIn,
                                            const AMBA_DSP_YUV_IMG_BUF_s *pYuvOut,
                                            UINT16 ViewZoneId,
                                            const CTX_STILL_INFO_s *pStlInfo,
                                            UINT8 HdrExpNum)
{
    UINT32 Rval = OK;
#ifdef SUPPORT_DSP_EXT_PIN_BUF
    ULONG ULAddr;
    ext_mem_desc_t *pExtMemDesc = NULL;
#endif
    CTX_VPROC_INFO_s VprocInfo = {0};
    UINT8 IsoMode = (UINT8)AMBA_IK_VIDEO_LINEAR;

    pVprocSetup->channel_id = (UINT8)ViewZoneId;
    pVprocSetup->input_format = DSP_VPROC_INPUT_RAW_RGB;
    pVprocSetup->is_raw_compressed = 0U;

    pVprocSetup->W_main = pRawIn->Window.Width;
    pVprocSetup->H_main = ALIGN_NUM16(pRawIn->Window.Height, 16U);

    /* Vin or Main > 1920 */
    HL_GetVprocInfo(HL_MTX_OPT_ALL, ViewZoneId, &VprocInfo);
    pVprocSetup->is_tile_mode = VprocInfo.VprocTileMode;

    pVprocSetup->is_li_enabled = (UINT8)1U;
    pVprocSetup->is_warp_enabled = (UINT8)1U;
    HL_GetIsoModeFromIkId(pIsoCfg->CfgIndex, &IsoMode);
    pVprocSetup->is_hdr_enabled = (UINT8)HL_IsHdrEnabled(IsoMode);

    pVprocSetup->prev_mode = (UINT8)0U;
    if (pStlInfo->RawInVprocPin == DSP_VPROC_PIN_PREVA) {
        pVprocSetup->is_prev_a_enabled = (UINT8)1U;
        pVprocSetup->prev_a_format = VPROC_PREV_FMT_PROG;
        pVprocSetup->prev_a_src = VPROC_PREV_SRC_DRAM;
        pVprocSetup->prev_a_dst = AMBA_DSP_PREV_DEST_PIN;
        pVprocSetup->prev_a_frame_rate = 0U;
        pVprocSetup->prev_a_ch_fmt = (pYuvOut->DataFmt == AMBA_DSP_YUV420)? DSP_YUV_420: DSP_YUV_422;
        pVprocSetup->prev_a_w = pYuvOut->Window.Width;
        pVprocSetup->prev_a_h = pYuvOut->Window.Height;
    } else {
        pVprocSetup->is_prev_a_enabled = (UINT8)0U;
        pVprocSetup->prev_a_format = VPROC_PREV_FMT_PROG;
        pVprocSetup->prev_a_src = VPROC_PREV_SRC_DRAM;
        pVprocSetup->prev_a_dst = AMBA_DSP_PREV_DEST_DUMMY;
        pVprocSetup->prev_a_frame_rate = 0U;
        pVprocSetup->prev_a_ch_fmt = DSP_YUV_420;
    }

    if (pStlInfo->RawInVprocPin == DSP_VPROC_PIN_PREVB) {
        pVprocSetup->is_prev_b_enabled = (UINT8)1U;
        pVprocSetup->prev_b_dst = AMBA_DSP_PREV_DEST_PIN;
        pVprocSetup->prev_b_frame_rate = 0U;
        pVprocSetup->prev_b_ch_fmt = (pYuvOut->DataFmt == AMBA_DSP_YUV420)? DSP_YUV_420: DSP_YUV_422;
        pVprocSetup->prev_b_w = pYuvOut->Window.Width;
        pVprocSetup->prev_b_h = pYuvOut->Window.Height;
    } else {
        pVprocSetup->is_prev_b_enabled = (UINT8)0U;
        pVprocSetup->prev_b_dst = AMBA_DSP_PREV_DEST_DUMMY;
        pVprocSetup->prev_b_frame_rate = 0U;
        pVprocSetup->prev_b_ch_fmt = DSP_YUV_420;
    }

    if (pStlInfo->RawInVprocPin == DSP_VPROC_PIN_PREVC) {
        pVprocSetup->is_prev_c_enabled = (UINT8)1U;
        pVprocSetup->prev_c_format = VPROC_PREV_FMT_PROG;
        pVprocSetup->prev_c_src = VPROC_PREV_SRC_DRAM;
        pVprocSetup->prev_c_dst = AMBA_DSP_PREV_DEST_PIN;
        pVprocSetup->prev_c_frame_rate = 0U;
        pVprocSetup->prev_c_ch_fmt = (pYuvOut->DataFmt == AMBA_DSP_YUV420)? DSP_YUV_420: DSP_YUV_422;
        pVprocSetup->prev_c_w = pYuvOut->Window.Width;
        pVprocSetup->prev_c_h = pYuvOut->Window.Height;
    } else {
        pVprocSetup->is_prev_c_enabled = (UINT8)0U;
        pVprocSetup->prev_c_format = VPROC_PREV_FMT_PROG;
        pVprocSetup->prev_c_src = VPROC_PREV_SRC_DRAM;
        pVprocSetup->prev_c_dst = AMBA_DSP_PREV_DEST_DUMMY;
        pVprocSetup->prev_c_frame_rate = 0U;
        pVprocSetup->prev_c_ch_fmt = DSP_YUV_420;
    }

    pVprocSetup->is_prev_hier_enabled = (UINT8)0U;
    pVprocSetup->is_prev_ln_enabled = (UINT8)0U;
    pVprocSetup->ln_det_src = 0U;

    pVprocSetup->is_c2y_burst_tiling_out = (UINT8)0U;
    pVprocSetup->is_hier_burst_tiling_out = (UINT8)0U;

    pVprocSetup->is_aaa_enabled = (UINT8)0U;
    pVprocSetup->cfg_aaa_by_ae_y = (pVprocSetup->is_tile_mode > 0U)? 1U: 0U;;
    pVprocSetup->aaa_cfa_mux_sel = 0U; //TBD
    pVprocSetup->is_dzoom_enabled = 0U; //No Dzoom in YuvInput

    pVprocSetup->is_fast_y2y_en = (UINT8)0U;
    pVprocSetup->is_compressed_out_enabled = (UINT8)0U;
    pVprocSetup->is_hdr_blend_dram_out_enabled = (UINT8)0U;
    pVprocSetup->is_warp_dram_out_enabled = (UINT8)0U;

    pVprocSetup->is_mctf_enabled = (UINT8)0U;
    pVprocSetup->is_mctf_ref_win_mode = 0U;
    pVprocSetup->mctf_mode = VPROC_MCTF_MODE_FILTER;
    pVprocSetup->mctf_chan = VPROC_MCTF_CAHN_YUV;
    pVprocSetup->mcts_chan = VPROC_MCTS_CAHN_LUMA;
    pVprocSetup->is_mcts_dram_out_enabled = 0U;

    if (pVprocSetup->is_tile_mode > 0U) {
        UINT16 C2YInTileX = 0U, C2YTileX = 0U, MctfTileX = 0U;

        /* Forbid MainResampling in Y2Y */
        Rval = HL_CalcVideoTileViewZone(ViewZoneId, pRawIn->Window.Width,
                                        VprocInfo.MaxTileWidth, 0U,
                                        &C2YInTileX);
        if (Rval == OK) {
            Rval = HL_CalcVideoTileViewZone(ViewZoneId, pRawIn->Window.Width,
                                            VprocInfo.MaxTileWidth, 0U,
                                            &C2YTileX);
            if (Rval == OK) {
                Rval = HL_CalcVideoTileViewZone(ViewZoneId, pRawIn->Window.Width,
                                                VprocInfo.MaxTileWidth, 0U,
                                                &MctfTileX);
                if (Rval == OK) {
                    pVprocSetup->c2y_tile_num_x = (UINT8)C2YTileX;
                    pVprocSetup->c2y_tile_num_y = 1U;
                    pVprocSetup->warp_tile_num_x = (UINT8)MctfTileX;
                    pVprocSetup->warp_tile_num_y = 1U;
                }
            }
        }
    } else {
        pVprocSetup->c2y_tile_num_x = 1U;
        pVprocSetup->c2y_tile_num_y = 1U;
        pVprocSetup->warp_tile_num_x = 1U;
        pVprocSetup->warp_tile_num_y = 1U;
    }
    if (Rval == OK) {
        pVprocSetup->raw_tile_overlap_x = TILE_OVERLAP_WIDTH;
        pVprocSetup->warp_tile_overlap_x = TILE_OVERLAP_WIDTH;
        pVprocSetup->warp_tile_overlap_y = CV2X_WARP_OVERLAP_Y_HIER_OFF;

        pVprocSetup->is_mipi_yuyv_enabled = 0U;
        pVprocSetup->is_pic_info_cache_enabled = 0U;
        pVprocSetup->is_h_slice_low_delay = 0U;

#ifdef SUPPORT_DSP_EXT_PIN_BUF
        DSP_GetDspVprocExtMemDescBuf(ViewZoneId, &pExtMemDesc);
        dsp_osal_typecast(&ULAddr, &pExtMemDesc);

        pVprocSetup->ext_mem_cfg_num = MAX_VPROC_EXT_MEM_TYPE_NUM;
        pExtMemDesc[DSP_VprocPinVprocMemTypeMap[pStlInfo->RawInVprocPin]].enable = (UINT8)1U;
        pExtMemDesc[DSP_VprocPinVprocMemTypeMap[pStlInfo->RawInVprocPin]].pool_buf_num = (UINT8)DEFAULT_EXT_MEM_POOL_VPROC_BUF_NUM;
        pExtMemDesc[DSP_VprocPinVprocMemTypeMap[pStlInfo->RawInVprocPin]].max_daddr_slot = (UINT8)DEFAULT_EXT_MEM_DADDR_NUM;
        (void)dsp_osal_virt2cli(ULAddr, &pVprocSetup->ext_mem_cfg_addr);
#endif

        pVprocSetup->is_3A_stat_only = 0U;
        pVprocSetup->proc_mode = (IsoMode == (UINT8)AMBA_IK_STILL_HISO)? VPROC_MODE_STILL_HISO: VPROC_MODE_STILL_LISO;
        pVprocSetup->num_of_exp = HdrExpNum;
    }
    return Rval;
}

static inline UINT32 HL_FillVprocSetupInR2r(cmd_vproc_setup_t *pVprocSetup,
                                            const AMBA_DSP_RAW_BUF_s *pRawIn,
                                            const AMBA_DSP_BUF_s *pAuxBufIn,
                                            UINT16 ViewZoneId, UINT8 WriteMode,
                                            UINT32 Opt)
{
    UINT32 Rval = OK;
    UINT16 ExtBufMask = 0;
#ifdef SUPPORT_DSP_EXT_PIN_BUF
    ULONG ULAddr;
    ext_mem_desc_t *pExtMemDesc = NULL;
#endif
    CTX_VPROC_INFO_s VprocInfo = {0};

    pVprocSetup->channel_id = (UINT8)ViewZoneId;
    pVprocSetup->input_format = DSP_VPROC_INPUT_RAW_RGB;
    pVprocSetup->is_raw_compressed = 0U;

    pVprocSetup->W_main = pRawIn->Window.Width;
    pVprocSetup->H_main = ALIGN_NUM16(pRawIn->Window.Height, 16U);

    /* Vin or Main > 1920 */
    HL_GetVprocInfo(HL_MTX_OPT_ALL, ViewZoneId, &VprocInfo);
    pVprocSetup->is_tile_mode = VprocInfo.VprocTileMode;

    pVprocSetup->is_li_enabled = (UINT8)1U;
    pVprocSetup->is_warp_enabled = (UINT8)0U;
    if (pAuxBufIn->BaseAddr != 0x0U) {
        pVprocSetup->is_hdr_enabled = (UINT8)1U;
    } else {
        pVprocSetup->is_hdr_enabled = (UINT8)0U;
    }

    pVprocSetup->prev_mode = (UINT8)0U;
    pVprocSetup->is_prev_a_enabled = (UINT8)0U;
    pVprocSetup->prev_a_format = VPROC_PREV_FMT_PROG;
    pVprocSetup->prev_a_src = VPROC_PREV_SRC_DRAM;
    pVprocSetup->prev_a_dst = AMBA_DSP_PREV_DEST_DUMMY;
    pVprocSetup->prev_a_frame_rate = 0U;
    pVprocSetup->prev_a_ch_fmt = DSP_YUV_420;

    pVprocSetup->is_prev_b_enabled = (UINT8)0U;
    pVprocSetup->prev_b_dst = AMBA_DSP_PREV_DEST_DUMMY;
    pVprocSetup->prev_b_frame_rate = 0U;
    pVprocSetup->prev_b_ch_fmt = DSP_YUV_420;

    pVprocSetup->is_prev_c_enabled = (UINT8)0U;
    pVprocSetup->prev_c_format = VPROC_PREV_FMT_PROG;
    pVprocSetup->prev_c_src = VPROC_PREV_SRC_DRAM;
    pVprocSetup->prev_c_dst = AMBA_DSP_PREV_DEST_DUMMY;
    pVprocSetup->prev_c_frame_rate = 0U;
    pVprocSetup->prev_c_ch_fmt = DSP_YUV_420;

    pVprocSetup->is_prev_hier_enabled = (UINT8)0U;
    pVprocSetup->is_prev_ln_enabled = (UINT8)0U;
    pVprocSetup->ln_det_src = 0U;

    pVprocSetup->is_c2y_burst_tiling_out = (UINT8)0U;
    pVprocSetup->is_hier_burst_tiling_out = (UINT8)0U;

    pVprocSetup->is_aaa_enabled = (UINT8)1U;
    pVprocSetup->cfg_aaa_by_ae_y = (pVprocSetup->is_tile_mode > 0U)? 1U: 0U;;
    pVprocSetup->aaa_cfa_mux_sel = 0U; //TBD
    if (Opt == STL_R2R_OPT_GEN_AAA) {
        pVprocSetup->is_dzoom_enabled = 0U; //No Dzoom in YuvInput
    } else {
        pVprocSetup->is_dzoom_enabled = 1U;
    }

    pVprocSetup->is_fast_y2y_en = (UINT8)0U;
    if (Opt == STL_R2R_OPT_GEN_AAA) {
        pVprocSetup->is_compressed_out_enabled = (UINT8)0U;
    } else {
        pVprocSetup->is_compressed_out_enabled = (UINT8)1U;
    }
    pVprocSetup->is_hdr_blend_dram_out_enabled = (UINT8)0U;
    pVprocSetup->is_warp_dram_out_enabled = (UINT8)0U;

    pVprocSetup->is_mctf_enabled = (UINT8)0U;
    pVprocSetup->is_mctf_ref_win_mode = 0U;
    pVprocSetup->mctf_mode = VPROC_MCTF_MODE_FILTER;
    pVprocSetup->mctf_chan = VPROC_MCTF_CAHN_YUV;
    pVprocSetup->mcts_chan = VPROC_MCTS_CAHN_LUMA;
    pVprocSetup->is_mcts_dram_out_enabled = 1U;

    if (pVprocSetup->is_tile_mode > 0U) {
        UINT16 C2YInTileX = 0U, C2YTileX = 0U, MctfTileX = 0U;

        /* Forbid MainResampling in Y2Y */
        Rval = HL_CalcVideoTileViewZone(ViewZoneId, pRawIn->Window.Width,
                                        VprocInfo.MaxTileWidth, 0U,
                                        &C2YInTileX);
        if (Rval == OK) {
            Rval = HL_CalcVideoTileViewZone(ViewZoneId, pRawIn->Window.Width,
                                            VprocInfo.MaxTileWidth, 0U,
                                            &C2YTileX);
            if (Rval == OK) {
                Rval = HL_CalcVideoTileViewZone(ViewZoneId, pRawIn->Window.Width,
                                                VprocInfo.MaxTileWidth, 0U,
                                                &MctfTileX);

            }
        }
        if (Rval == OK) {
            pVprocSetup->c2y_tile_num_x = (UINT8)C2YTileX;
            pVprocSetup->c2y_tile_num_y = 1U;
            pVprocSetup->warp_tile_num_x = (UINT8)MctfTileX;
            pVprocSetup->warp_tile_num_y = 1U;
        }
    } else {
        pVprocSetup->c2y_tile_num_x = 1U;
        pVprocSetup->c2y_tile_num_y = 1U;
        pVprocSetup->warp_tile_num_x = 1U;
        pVprocSetup->warp_tile_num_y = 1U;
    }
    if (Rval == OK) {
        pVprocSetup->raw_tile_overlap_x = TILE_OVERLAP_WIDTH;
        pVprocSetup->warp_tile_overlap_x = TILE_OVERLAP_WIDTH;
        pVprocSetup->warp_tile_overlap_y = CV2X_WARP_OVERLAP_Y_HIER_OFF;

        pVprocSetup->is_mipi_yuyv_enabled = 0U;
        pVprocSetup->is_pic_info_cache_enabled = 0U;
        pVprocSetup->is_h_slice_low_delay = 0U;
        DSP_SetU16Bit(&ExtBufMask, VPROC_EXT_MEM_TYPE_COMP_RAW);
        pVprocSetup->ext_buf_mask = ExtBufMask;

#ifdef SUPPORT_DSP_EXT_PIN_BUF
        DSP_GetDspVprocExtMemDescBuf(ViewZoneId, &pExtMemDesc);
        dsp_osal_typecast(&ULAddr, &pExtMemDesc);

        pVprocSetup->ext_mem_cfg_num = MAX_VPROC_EXT_MEM_TYPE_NUM;

        if (Opt != STL_R2R_OPT_GEN_AAA) {
            //pExtMemDesc[DSP_VprocPinVprocMemTypeMap[pStlInfo->RawInVprocPin]].enable = (UINT8)1U;
            //pExtMemDesc[DSP_VprocPinVprocMemTypeMap[pStlInfo->RawInVprocPin]].pool_buf_num = (UINT8)DEFAULT_EXT_MEM_POOL_VPROC_BUF_NUM;
            //pExtMemDesc[DSP_VprocPinVprocMemTypeMap[pStlInfo->RawInVprocPin]].max_daddr_slot = (UINT8)DEFAULT_EXT_MEM_DADDR_NUM;
            pExtMemDesc[VPROC_EXT_MEM_TYPE_COMP_RAW].enable = (UINT8)1U;
            pExtMemDesc[VPROC_EXT_MEM_TYPE_COMP_RAW].pool_buf_num = (UINT8)DEFAULT_EXT_MEM_POOL_VPROC_BUF_NUM;
            pExtMemDesc[VPROC_EXT_MEM_TYPE_COMP_RAW].max_daddr_slot = (UINT8)DEFAULT_EXT_MEM_DADDR_NUM;
        }
        (void)dsp_osal_virt2cli(ULAddr, &pVprocSetup->ext_mem_cfg_addr);
#endif
        if (Opt == STL_R2R_OPT_GEN_AAA) {
            pVprocSetup->is_3A_stat_only = 1U;
        } else {
            pVprocSetup->is_3A_stat_only = 0U;
        }
        if (Opt == STL_R2R_OPT_GEN_AAA) {
            pVprocSetup->raw_tile_alignment_mode = (UINT8)0U;
        } else {
            pVprocSetup->raw_tile_alignment_mode = (UINT8)1U;
        }
        pVprocSetup->proc_mode = VPROC_MODE_STILL_LISO;
        pVprocSetup->num_of_exp = 0U;

        Rval = AmbaHL_CmdVprocSetup(WriteMode, pVprocSetup);
    }

    return Rval;
}

static inline UINT32 HL_FillSendInputDataInR2y(cmd_vin_send_input_data_t *pVinFeedData,
                                               UINT8 OutputFormat,
                                               const AMBA_DSP_ISOCFG_CTRL_s *pIsoCfg,
                                               UINT16 ViewZoneId,
                                               UINT16 Compression,
                                               const AMBA_DSP_RAW_BUF_s *pRawIn,
                                               const CTX_STILL_INFO_s *pStlInfo)
{
    UINT32 *pBatchQAddr;
    const void *pVoid;
    dsp_cmd_t *pDspCmd = NULL;
    ULONG ULAddr = 0U;
    UINT8 BatchCmdNum = 0U;
    UINT16 IsExtMem = 1U, Mantissa, BlkSz, RawWidth = 0U, Pitch = 0U, OutputPinMask = 0U;
    UINT32 StlProcId = 0U, NewWp = 0U, BatchCmdId = 0U;
    UINT32 Rval;
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;
    DSP_STL_PROC_JOB_ID_s StlProcJobId = {0};
    process_idsp_iso_config_cmd_t *pIsoCfgCmd;
    UINT8 CompressedRate = 0U, CompactMode = 0U, U8Val;

    HL_DisassembleVinCompression(Compression, &CompressedRate, &CompactMode);
    (void)HL_GetCmprRawBufInfo(pRawIn->Window.Width, CompressedRate, CompactMode, &RawWidth, &Pitch, &Mantissa, &BlkSz);

    U8Val = (UINT8)(pStlInfo->RawInVirtVinId + AMBA_DSP_MAX_VIN_NUM);
    pVinFeedData->vin_id = U8Val;
    pVinFeedData->send_data_dest = DSP_VIN_SEND_IN_DATA_DEST_VPROC;
    pVinFeedData->input_data_type = DSP_VIN_SEND_IN_DATA_RAW;
    pVinFeedData->is_compression_en = pRawIn->Compressed;
    pVinFeedData->blk_sz = (BlkSz == 0xFFU)? 7U: (UINT8)BlkSz;
    pVinFeedData->mantissa = (UINT8)Mantissa;

    HL_GetPointerToDspStlBatchQ(0U/*R2Y*/, &pBatchQAddr);
    dsp_osal_typecast(&pVoid, &pBatchQAddr);
    dsp_osal_typecast(&ULAddr, &pBatchQAddr);
    dsp_osal_typecast(&pIsoCfgCmd, &pBatchQAddr);
    dsp_osal_typecast(&pDspCmd, &pBatchQAddr);
    pIsoCfgCmd->cmd_code = PROCESS_ISO_CONFIG_CMD;
    pIsoCfgCmd->flag.iso_config_common_update = (UINT8)1U;
    pIsoCfgCmd->flag.iso_config_color_update = (UINT8)1U;
    pIsoCfgCmd->flag.iso_config_mctf_update = (UINT8)1U;
    pIsoCfgCmd->flag.iso_config_step1_update = (UINT8)1U;
    pIsoCfgCmd->flag.iso_config_aaa_update = (UINT8)1U;
    pIsoCfgCmd->sequence_id = 0U; //TBD
    pIsoCfgCmd->flag.iso_config_warp_update = (UINT8)1U;
    (void)dsp_osal_virt2cli(pIsoCfg->CfgAddress, &pIsoCfgCmd->iso_cfg_daddr);
    (void)dsp_osal_cache_clean(pVoid, (UINT32)sizeof(process_idsp_iso_config_cmd_t));
    BatchCmdNum++;
    pDspCmd[BatchCmdNum].cmd_code = MAX_BATCH_CMD_END;
    (void)dsp_osal_virt2cli(ULAddr, &pVinFeedData->batch_cmd_set_info.addr);

    HL_GetViewZoneInfoLock(ViewZoneId, &ViewZoneInfo);
    /* Request BatchCmdQ buffer */
    Rval = DSP_ReqBuf(&ViewZoneInfo->BatchQPoolDesc, 1U, &NewWp, 1U/*FullnessCheck*/);
    if (Rval != OK) {
        HL_GetViewZoneInfoUnLock(ViewZoneId);
    } else {
        ViewZoneInfo->BatchQPoolDesc.Wp = NewWp;
        HL_GetViewZoneInfoUnLock(ViewZoneId);
        HL_GetBatchCmdId(ViewZoneId, (UINT16)ViewZoneInfo->BatchQPoolDesc.Wp, &BatchCmdId);
    }

    if (Rval == OK) {
        HL_SetDspBatchQInfo(BatchCmdId, 1U/*AutoReset*/, BATCHQ_INFO_ISO_CFG, pIsoCfg->CfgIndex);
        DSP_SetU16Bit(&OutputPinMask, pStlInfo->RawInVprocPin);
        Rval = HL_ComposeStlProcJobId(&StlProcJobId, (UINT16)OutputFormat, OutputPinMask, IsExtMem, ViewZoneId);
    }
    if (Rval == OK) {
        (void)dsp_osal_memcpy(&StlProcId, &StlProcJobId, sizeof(UINT32));
        HL_SetDspBatchQInfo(BatchCmdId, 0U/*AutoReset*/, BATCHQ_INFO_STL_PROC, StlProcId);
        pVinFeedData->batch_cmd_set_info.id = BatchCmdId;
        pVinFeedData->batch_cmd_set_info.size = HL_GetBatchCmdNumber(ULAddr)*CMD_SIZE_IN_BYTE;
    }

    return Rval;
}

static inline UINT32 HL_FillSendInputDataInR2r(cmd_vin_send_input_data_t *pVinFeedData,
                                               const AMBA_DSP_ISOCFG_CTRL_s *pIsoCfg,
                                               UINT16 ViewZoneId,
                                               UINT8 Compression,
                                               const AMBA_DSP_RAW_BUF_s *pRawIn,
                                               const CTX_STILL_INFO_s *pStlInfo)
{
    UINT32 *pBatchQAddr;
    const void *pVoid;
    dsp_cmd_t *pDspCmd = NULL;
    ULONG ULAddr = 0x0U;
    UINT8 BatchCmdNum = 0U;
    UINT16 IsExtMem = 1U, Mantissa, BlkSz, RawWidth = 0U, Pitch = 0U, OutputPinMask = 0U;
    UINT32 StlProcId = 0U, NewWp = 0U, BatchCmdId = 0U;
    UINT32 Rval;
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;
    DSP_STL_PROC_JOB_ID_s StlProcJobId = {0};
    process_idsp_iso_config_cmd_t *pIsoCfgCmd;
    UINT8 CompressedRate = 0U, CompactMode = 0U;
    UINT8 U8Val;

    HL_DisassembleVinCompression(Compression, &CompressedRate, &CompactMode);
    (void)HL_GetCmprRawBufInfo(pRawIn->Window.Width, CompressedRate, CompactMode, &RawWidth, &Pitch, &Mantissa, &BlkSz);

    U8Val = (UINT8)(pStlInfo->RawInVirtVinId + AMBA_DSP_MAX_VIN_NUM);
    pVinFeedData->vin_id = U8Val;
    pVinFeedData->send_data_dest = DSP_VIN_SEND_IN_DATA_DEST_VPROC;
    pVinFeedData->input_data_type = DSP_VIN_SEND_IN_DATA_RAW;
    pVinFeedData->is_compression_en = pRawIn->Compressed;
    pVinFeedData->blk_sz = (BlkSz == 0xFFU)? 7U: (UINT8)BlkSz;
    pVinFeedData->mantissa = (UINT8)Mantissa;

    HL_GetPointerToDspStlBatchQ(2U/*R2R*/, &pBatchQAddr);
    dsp_osal_typecast(&pVoid, &pBatchQAddr);
    dsp_osal_typecast(&ULAddr, &pBatchQAddr);
    dsp_osal_typecast(&pIsoCfgCmd, &pBatchQAddr);
    dsp_osal_typecast(&pDspCmd, &pBatchQAddr);

    pIsoCfgCmd->cmd_code = PROCESS_ISO_CONFIG_CMD;
    pIsoCfgCmd->flag.iso_config_common_update = (UINT8)1U;
    pIsoCfgCmd->flag.iso_config_color_update = (UINT8)1U;
    pIsoCfgCmd->flag.iso_config_mctf_update = (UINT8)1U;
    pIsoCfgCmd->flag.iso_config_step1_update = (UINT8)1U;
    pIsoCfgCmd->flag.iso_config_aaa_update = (UINT8)1U;
    pIsoCfgCmd->sequence_id = 0U; //TBD
    pIsoCfgCmd->flag.iso_config_warp_update = (UINT8)1U;
    (void)dsp_osal_virt2cli(pIsoCfg->CfgAddress, &pIsoCfgCmd->iso_cfg_daddr);
    (void)dsp_osal_cache_clean(pVoid, (UINT32)sizeof(process_idsp_iso_config_cmd_t));
    BatchCmdNum++;
    pDspCmd[BatchCmdNum].cmd_code = MAX_BATCH_CMD_END;
    (void)dsp_osal_virt2cli(ULAddr, &pVinFeedData->batch_cmd_set_info.addr);

    HL_GetViewZoneInfoLock(ViewZoneId, &ViewZoneInfo);
    /* Request BatchCmdQ buffer */
    Rval = DSP_ReqBuf(&ViewZoneInfo->BatchQPoolDesc, 1U, &NewWp, 1U/*FullnessCheck*/);
    if (Rval != OK) {
        HL_GetViewZoneInfoUnLock(ViewZoneId);
    } else {
        ViewZoneInfo->BatchQPoolDesc.Wp = NewWp;
        HL_GetViewZoneInfoUnLock(ViewZoneId);
        HL_GetBatchCmdId(ViewZoneId, (UINT16)ViewZoneInfo->BatchQPoolDesc.Wp, &BatchCmdId);
    }

    DSP_SetU16Bit(&OutputPinMask, pStlInfo->RawInVprocPin);
    HL_SetDspBatchQInfo(BatchCmdId, 1U/*AutoReset*/, BATCHQ_INFO_ISO_CFG, pIsoCfg->CfgIndex);
    Rval = HL_ComposeStlProcJobId(&StlProcJobId, 2U, OutputPinMask, IsExtMem, ViewZoneId);
    if (Rval == OK) {
        (void)dsp_osal_memcpy(&StlProcId, &StlProcJobId, sizeof(UINT32));
        HL_SetDspBatchQInfo(BatchCmdId, 0U/*AutoReset*/, BATCHQ_INFO_STL_PROC, StlProcId);
        pVinFeedData->batch_cmd_set_info.id = BatchCmdId;
        pVinFeedData->batch_cmd_set_info.size = HL_GetBatchCmdNumber(ULAddr)*CMD_SIZE_IN_BYTE;
    }
    return Rval;
}

static inline void HL_FillSetVprocExtMemOfRawBuf(cmd_vproc_set_ext_mem_t *pVprocExtMem,
                                               const AMBA_DSP_RAW_BUF_s *pRawOut,
                                               UINT16 Compression,
                                               const UINT16 ViewZoneID,
                                               const UINT16 OutputHeight)
{
    UINT16 Mantissa, BlkSz, RawWidth = 0U, Pitch = 0U;
    UINT32 *pBufTblAddr = NULL;
    UINT32 PhysAddr;
    ULONG ULAddr = 0x0U;
    UINT8 CompressedRate = 0U, CompactMode = 0U;

    pVprocExtMem->channel_id = (UINT8)ViewZoneID;
    pVprocExtMem->memory_type = VPROC_EXT_MEM_TYPE_COMP_RAW;
    pVprocExtMem->allocation_type = VIN_EXT_MEM_ALLOC_TYPE_DISTINCT;
    pVprocExtMem->allocation_mode = VIN_EXT_MEM_MODE_NEW;
    pVprocExtMem->overflow_cntl = VIN_EXT_MEM_CTRL_CIRCULAR;
    pVprocExtMem->num_frm_buf = MAX_EXT_MEM_BUF_NUM;
    pVprocExtMem->chroma_format = DSP_YUV_MONO;
    pVprocExtMem->luma_img_ofs_x = 0U;
    pVprocExtMem->luma_img_ofs_y = 0U;
    pVprocExtMem->chroma_img_ofs_x = 0U;
    pVprocExtMem->chroma_img_ofs_y = 0U;
    pVprocExtMem->img_width = 0U;
    pVprocExtMem->img_height = 0U;

    pVprocExtMem->aux_pitch = 0U;
    pVprocExtMem->aux_width = 0U;
    pVprocExtMem->aux_height = 0U;
    pVprocExtMem->aux_img_ofs_x = 0U;
    pVprocExtMem->aux_img_ofs_y = 0U;
    pVprocExtMem->aux_img_width = 0U;
    pVprocExtMem->aux_img_height = 0U;

    pVprocExtMem->buf_pitch = pRawOut->Pitch;
    HL_DisassembleVinCompression(Compression, &CompressedRate, &CompactMode);
    (void)HL_GetCmprRawBufInfo(pRawOut->Window.Width, CompressedRate, CompactMode, &RawWidth, &Pitch, &Mantissa, &BlkSz);
    pVprocExtMem->buf_width = RawWidth;
    pVprocExtMem->buf_height = OutputHeight;

    HL_GetPointerToDspExtRawBufArray(ViewZoneID, 0U/*Raw*/, &pBufTblAddr);
    (void)dsp_osal_virt2cli(pRawOut->BaseAddr, &PhysAddr);
    pBufTblAddr[0U] = PhysAddr;
    dsp_osal_typecast(&ULAddr, &pBufTblAddr);
    (void)dsp_osal_virt2cli(ULAddr, &pVprocExtMem->buf_addr);
}

static inline void HL_FillSetExtRawMemOfRawBuf(cmd_vin_set_ext_mem_t *pVinExtMem,
                                               const AMBA_DSP_RAW_BUF_s *pRawIn,
                                               UINT16 Compression,
                                               const UINT16 VinId,
                                               const UINT16 InputHeight)
{
    UINT16 Mantissa, BlkSz, RawWidth = 0U, Pitch = 0U;
    UINT32 *pBufTblAddr = NULL;
    UINT32 PhysAddr;
    ULONG ULAddr = 0x0U;
    UINT8 CompressedRate = 0U, CompactMode = 0U;

    pVinExtMem->vin_id = (UINT8)VinId;
    pVinExtMem->memory_type = DSP_VIN_EXT_MEM_TYPE_RAW;
    pVinExtMem->allocation_type = VIN_EXT_MEM_ALLOC_TYPE_DISTINCT;
    pVinExtMem->allocation_mode = VIN_EXT_MEM_MODE_NEW;
    /* case 1, ssp control, fixed usage, always new */
    pVinExtMem->overflow_cntl = VIN_EXT_MEM_CTRL_CIRCULAR;
    pVinExtMem->num_frm_buf = MAX_EXT_MEM_BUF_NUM;
    pVinExtMem->chroma_format = DSP_YUV_MONO;
    pVinExtMem->luma_img_ofs_x = 0U;
    pVinExtMem->luma_img_ofs_y = 0U;
    pVinExtMem->chroma_img_ofs_x = 0U;
    pVinExtMem->chroma_img_ofs_y = 0U;
    pVinExtMem->img_width = 0U;
    pVinExtMem->img_height = 0U;

    pVinExtMem->aux_pitch = 0U;
    pVinExtMem->aux_width = 0U;
    pVinExtMem->aux_height = 0U;
    pVinExtMem->aux_img_ofs_x = 0U;
    pVinExtMem->aux_img_ofs_y = 0U;
    pVinExtMem->aux_img_width = 0U;
    pVinExtMem->aux_img_height = 0U;

    pVinExtMem->buf_pitch = pRawIn->Pitch;
    HL_DisassembleVinCompression(Compression, &CompressedRate, &CompactMode);
    (void)HL_GetCmprRawBufInfo(pRawIn->Window.Width, CompressedRate, CompactMode, &RawWidth, &Pitch, &Mantissa, &BlkSz);
    pVinExtMem->buf_width = RawWidth;
    pVinExtMem->buf_height = InputHeight;

    /*
     * vin_yuv_enc_fbp_init will allocate resource for virtual vin yuv input from ext_mem at vin_flow_max stage
     * Need to allocate resource (vin_yuv_enc_fbp_disable=0) when send ext_mem after vproc
     * */
    pVinExtMem->vin_yuv_enc_fbp_disable = 0U;

    HL_GetPointerToDspExtRawBufArray(VinId, 0U/*Raw*/, &pBufTblAddr);
    (void)dsp_osal_virt2cli(pRawIn->BaseAddr, &PhysAddr);
    pBufTblAddr[0U] = PhysAddr;
    dsp_osal_typecast(&ULAddr, &pBufTblAddr);
    (void)dsp_osal_virt2cli(ULAddr, &pVinExtMem->buf_addr);
}

static inline UINT32 HL_StillRaw2YuvChk(const AMBA_DSP_RAW_BUF_s *pRawIn,
                                        const AMBA_DSP_BUF_s *pAuxBufIn,
                                        const AMBA_DSP_YUV_IMG_BUF_s *pYuvOut,
                                        const AMBA_DSP_ISOCFG_CTRL_s *pIsoCfg,
                                        const UINT32 Opt,
                                        const UINT64 *pAttachedRawSeq)
{
    UINT32 Rval;
    const Dsp_DumpApi_t* DumpApi = AmbaDSP_GetDumpApiFunc();
    const Dsp_CheckApi_t* CheckApi = AmbaDSP_GetCheckApiFunc();
    const Dsp_TimingApi_t* TimingApi = AmbaDSP_GetTimingApiFunc();
    const Dsp_ResourceApi_t* ResourceApi = AmbaDSP_GetResourceApiFunc();
    CTX_STILL_INFO_s StlInfo = {0};

    /* DSP CtxInit */
    Rval = HL_CtxInit();

    /* Input sanity check */
    if ((Rval == OK) &&
        (DumpApi != NULL)) {
        DumpApi->pStillRaw2Yuv(pRawIn, pAuxBufIn, pYuvOut, pIsoCfg, Opt, pAttachedRawSeq);
    }
    if ((Rval == OK) &&
        (CheckApi != NULL)) {
        Rval = CheckApi->pStillRaw2Yuv(pRawIn, pAuxBufIn, pYuvOut, pIsoCfg, Opt, pAttachedRawSeq);
    }
    /* HW/SW resources check */
    if ((Rval == OK) &&
        (ResourceApi != NULL)) {
        Rval = ResourceApi->pStillRaw2Yuv(pRawIn, pAuxBufIn, pYuvOut, pIsoCfg, Opt, pAttachedRawSeq);
    }

    if (Rval == OK) {
        HL_GetStlInfo(HL_MTX_OPT_ALL, &StlInfo);
        /* Logic sanity check */
        if ((StlInfo.RawInVirtVinId == DSP_VIRT_VIN_IDX_INVALID) ||
            (StlInfo.RawInVprocId == DSP_VIRT_VIN_IDX_INVALID)) {
            Rval = DSP_ERR_0004;
        }
    }

    /* Timing sanity check */
    if ((Rval == OK) &&
        (TimingApi != NULL)) {
        Rval = TimingApi->pStillRaw2Yuv(pRawIn, pAuxBufIn, pYuvOut, pIsoCfg, Opt, pAttachedRawSeq);
    }

    return Rval;
}

static inline UINT32 HL_StillR2ySetupVproc(UINT8 WriteMode, UINT8 RawVprocStatus, UINT16 ViewZoneId,
                                           UINT8 HdrExpNum,
                                           const AMBA_DSP_RAW_BUF_s *pRawIn,
                                           const AMBA_DSP_YUV_IMG_BUF_s *pYuvOut,
                                           const AMBA_DSP_ISOCFG_CTRL_s *pIsoCfg,
                                           const CTX_STILL_INFO_s *pStlInfo)
{
    UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
    UINT32 Rval = OK;
    UINT32 PhysAddr = 0U;
    UINT32 *pOutputBufTblAddr;
    ULONG ULAddr;
    cmd_vproc_setup_t *VprocSetup = HL_DefCtxCmdBufPtrVpcSetup;
    cmd_vproc_set_ext_mem_t *VprocExtMem = HL_DefCtxCmdBufPtrVpcExtMem;
    cmd_vin_attach_event_to_raw_t *VinAttachEvent = HL_DefCtxCmdBufPtrVinAttRaw;
    const void *pVoid = NULL;

#ifndef SUPPORT_DSP_EXT_PIN_BUF
//FIXME with API
        HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
        dsp_osal_typecast(&VprocExtMem, &CmdBufferAddr);
        VprocExtMem->channel_id = ViewZoneId;
        VprocExtMem->memory_type = (UINT8)HL_VprocPinVprocMemTypePoolMap[pStlInfo->RawInVprocPin];
        VprocExtMem->allocation_type = VIN_EXT_MEM_ALLOC_TYPE_DISTINCT;
        VprocExtMem->allocation_mode = VIN_EXT_MEM_MODE_NEW;
        VprocExtMem->overflow_cntl = VIN_EXT_MEM_CTRL_CIRCULAR;
        VprocExtMem->num_frm_buf = MAX_EXT_MEM_BUF_NUM;
#if 0
        if (VprocExtMem->num_frm_buf > EXT_MEM_VPROC_MAX_NUM) {
            AmbaLL_LogUInt5("Only %d ExtMem supported", EXT_MEM_VPROC_MAX_NUM, 0U, 0U, 0U, 0U);
        }
#endif
        VprocExtMem->buf_pitch = ALIGN_NUM16(pYuvOut->Pitch, (UINT16)DSP_BUF_ALIGNMENT);
        VprocExtMem->buf_width = VprocExtMem->buf_pitch;
        VprocExtMem->buf_height = pYuvOut->Window.Height;
        HL_GetPointerToDspExtStlBufArray(0U/*R2Y*/, &pOutputBufTblAddr);
        (void)dsp_osal_virt2cli(pYuvOut->BaseAddrY, &PhysAddr);
        pOutputBufTblAddr[0U] = PhysAddr;
        dsp_osal_typecast(&pVoid, &pOutputBufTblAddr);
        dsp_osal_typecast(&ULAddr, &pOutputBufTblAddr);
        (void)dsp_osal_cache_clean(pVoid, sizeof(UINT32)*MAX_EXT_MEM_BUF_NUM);
        (void)dsp_osal_virt2cli(ULAddr, &VprocExtMem->buf_addr);

        VprocExtMem->img_width = pYuvOut->Window.Width;
        VprocExtMem->img_height = pYuvOut->Window.Height;
        VprocExtMem->chroma_format = (pYuvOut->DataFmt == AMBA_DSP_YUV420)? DSP_YUV_420: DSP_YUV_422;
        Rval = AmbaHL_CmdVprocSetExtMem(WriteMode, VprocExtMem);
        HL_RelCmdBuffer(CmdBufferId);
        if (Rval == OK) {
            //VinAttached for VprocOuput
            HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
            dsp_osal_typecast(&VinAttachEvent, &CmdBufferAddr);
            VinAttachEvent->vin_id = (UINT8)(pStlInfo->RawInVirtVinId + AMBA_DSP_MAX_VIN_NUM);
            VinAttachEvent->ch_id = (UINT8)0U; //TBD
            DSP_SetBit(&VinAttachEvent->event_mask, HL_VprocPinVinAttachBitMap[pStlInfo->RawInVprocPin]);
            DSP_SetU16Bit(pOutputPinMask, pStlInfo->RawInVprocPin);
            VinAttachEvent->repeat_cnt = 0x10001;
            VinAttachEvent->time_duration_bet_rpts = 0U;
            Rval = AmbaHL_CmdVinAttachEventToRaw(WriteMode, VinAttachEvent);
            HL_RelCmdBuffer(CmdBufferId);
        }
#endif

        //Vproc setup
//FIXME with API
#ifndef SUPPORT_DSP_EXT_PIN_BUF
        if (Rval == OK) {
#endif
            if ((RawVprocStatus == STL_VPROC_STATUS_IDLE) ||
                (RawVprocStatus == STL_VPROC_STATUS_CONFIG) ||
                (RawVprocStatus == STL_VPROC_STATUS_STOPPED)) {
                HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
                dsp_osal_typecast(&VprocSetup, &CmdBufferAddr);
                Rval = HL_FillVprocSetupInR2y(VprocSetup, pIsoCfg, pRawIn, pYuvOut, ViewZoneId, pStlInfo, HdrExpNum);
                if (Rval == OK) {
                    Rval = AmbaHL_CmdVprocSetup(WriteMode, VprocSetup);
                }
                HL_RelCmdBuffer(CmdBufferId);
            }
#ifndef SUPPORT_DSP_EXT_PIN_BUF
        }
#endif

#ifdef SUPPORT_DSP_EXT_PIN_BUF
//FIXME with API
        if (Rval == OK) {
            HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
            dsp_osal_typecast(&VprocExtMem, &CmdBufferAddr);
            VprocExtMem->channel_id = (UINT8)ViewZoneId;
            VprocExtMem->memory_type = (UINT8)HL_VprocPinVprocMemTypePoolMap[pStlInfo->RawInVprocPin];
            VprocExtMem->allocation_type = VIN_EXT_MEM_ALLOC_TYPE_DISTINCT;
            VprocExtMem->allocation_mode = VIN_EXT_MEM_MODE_NEW;
            /* case 1, ssp control, fixed usage, always new */
            VprocExtMem->overflow_cntl = VIN_EXT_MEM_CTRL_CIRCULAR;
            VprocExtMem->num_frm_buf = MAX_EXT_MEM_BUF_NUM;
#if 0
            if (VprocExtMem->num_frm_buf > EXT_MEM_VPROC_MAX_NUM) {
                AmbaLL_LogUInt5("Only %d ExtMem supported", EXT_MEM_VPROC_MAX_NUM, 0U, 0U, 0U, 0U);
            }
#endif
            VprocExtMem->buf_pitch = ALIGN_NUM16(pYuvOut->Pitch, (UINT16)DSP_BUF_ALIGNMENT);
            VprocExtMem->buf_width = VprocExtMem->buf_pitch;
            VprocExtMem->buf_height = HL_StlEncCtrCalExtMemBufH(pYuvOut->BaseAddrY, pYuvOut->BaseAddrUV,
                                                                pYuvOut->Pitch, pYuvOut->Window.Height);
            HL_GetPointerToDspExtStlBufArray(0U/*R2Y*/, &pOutputBufTblAddr);
            (void)dsp_osal_virt2cli(pYuvOut->BaseAddrY, &PhysAddr);
            pOutputBufTblAddr[0U] = PhysAddr;
            dsp_osal_typecast(&pVoid, &pOutputBufTblAddr);
            dsp_osal_typecast(&ULAddr, &pOutputBufTblAddr);
            (void)dsp_osal_cache_clean(pVoid, (UINT32)sizeof(UINT32)*MAX_EXT_MEM_BUF_NUM);
            (void)dsp_osal_virt2cli(ULAddr, &VprocExtMem->buf_addr);

            VprocExtMem->img_width = pYuvOut->Window.Width;
            VprocExtMem->img_height = pYuvOut->Window.Height;
            VprocExtMem->chroma_format = (pYuvOut->DataFmt == AMBA_DSP_YUV420)? DSP_YUV_420: DSP_YUV_422;
            Rval = AmbaHL_CmdVprocSetExtMem(WriteMode, VprocExtMem);
            HL_RelCmdBuffer(CmdBufferId);
        }

        //VinAttached for VprocOuput
        if (Rval == OK) {
            UINT8 U8Val = (UINT8)(pStlInfo->RawInVirtVinId + AMBA_DSP_MAX_VIN_NUM);

            HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
            dsp_osal_typecast(&VinAttachEvent, &CmdBufferAddr);
            VinAttachEvent->vin_id = U8Val;
            VinAttachEvent->ch_id = 0U; //TBD
            DSP_SetBit(&VinAttachEvent->event_mask, HL_VprocPinVinAttachBitMap[pStlInfo->RawInVprocPin]);
            VinAttachEvent->repeat_cnt = 0x10001;
            VinAttachEvent->time_duration_bet_rpts = 0U;
            Rval = AmbaHL_CmdVinAttachEventToRaw(WriteMode, VinAttachEvent);
            HL_RelCmdBuffer(CmdBufferId);
        }
#endif

    return Rval;
}

static inline UINT32 HL_R2ySendInputData(UINT8 WriteMode, UINT16 ViewZoneId,
                                         const AMBA_DSP_RAW_BUF_s *pRawIn,
                                         UINT8 OutputFormat,
                                         const AMBA_DSP_ISOCFG_CTRL_s *pIsoCfg,
                                         UINT16 Compression,
                                         CTX_STILL_INFO_s *pStlInfo)
{
    UINT32 Rval;
    UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
    cmd_vin_send_input_data_t *VinFeedData = HL_DefCtxCmdBufPtrVinSndData;

    HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
    dsp_osal_typecast(&VinFeedData, &CmdBufferAddr);
    Rval = HL_FillSendInputDataInR2y(VinFeedData, OutputFormat, pIsoCfg, ViewZoneId,
                                     Compression, pRawIn, pStlInfo);

    if (Rval == OK) {
        Rval = AmbaHL_CmdVinSendInputData(WriteMode, VinFeedData);
    }
    HL_RelCmdBuffer(CmdBufferId);
    if (Rval == OK) {
        HL_GetStlInfo(HL_MTX_OPT_GET, pStlInfo);
        pStlInfo->RawInVprocStatus = STL_VPROC_STATUS_RUN;
        HL_SetStlInfo(HL_MTX_OPT_SET, pStlInfo);
    }

    return Rval;
}

/**
* Still Raw2Yuv
* @param [in]  pRawIn input raw buffer
* @param [in]  pAuxBufIn input aux buffer
* @param [in]  pYuvOut output yuv buffer
* @param [in]  pIsoCfg Iso config structure
* @param [in]  Opt R2Y option
* @param [out] pAttachedRawSeq attached raw sequence
* @return ErrorCode
*/
UINT32 dsp_still_raw2yuv(const AMBA_DSP_RAW_BUF_s *pRawIn,
                         const AMBA_DSP_BUF_s *pAuxBufIn,
                         const AMBA_DSP_YUV_IMG_BUF_s *pYuvOut,
                         const AMBA_DSP_ISOCFG_CTRL_s *pIsoCfg,
                         UINT32 Opt,
                         UINT64 *pAttachedRawSeq)
{
    UINT8 WriteMode = AMBA_DSP_CMD_VDSP_NORMAL_WRITE, HdrExpNum = 1U;
    UINT8 RawVprocStatus, YuvInVprocStatus;
    UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
    UINT16 ViewZoneId, YuvVinId, RawVinId, Compression = 0U;
    UINT16 InputHeight = 0U;
    UINT32 Rval;

    CTX_STILL_INFO_s StlInfo = {0};
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;
    cmd_vproc_cfg_t *VprocCfg = HL_DefCtxCmdBufPtrVpc;
    cmd_binding_cfg_t *FpBindCfg = HL_DefCtxCmdBufPtrBind;
#ifdef SUPPORT_VPROC_RT_RESTART
    cmd_vproc_stop_t *VprocStop = HL_DefCtxCmdBufPtrVpcStop;
#endif
    cmd_vin_set_ext_mem_t *VinExtMem = HL_DefCtxCmdBufPtrVinExtMem;

    Rval = HL_StillRaw2YuvChk(pRawIn, pAuxBufIn, pYuvOut, pIsoCfg, Opt, pAttachedRawSeq);
    AmbaMisra_TouchUnused(pAttachedRawSeq);

    HL_GetStlInfo(HL_MTX_OPT_ALL, &StlInfo);
    /* Body */
    if (Rval == OK) {
        HL_GetResourcePtr(&Resource);

        ViewZoneId = StlInfo.RawInVprocId;
        RawVprocStatus = StlInfo.RawInVprocStatus;
        YuvInVprocStatus = StlInfo.YuvInVprocStatus;
        YuvVinId = StlInfo.YuvInVirtVinId + AMBA_DSP_MAX_VIN_NUM;
        RawVinId = StlInfo.RawInVirtVinId + AMBA_DSP_MAX_VIN_NUM;
        WriteMode = HL_GetVin2CmdNormalWrite(RawVinId);
//FIXME, Binding manager
        /*
         * 20190614, Per ChenHan, Bind/Unbind shall go through VinCmdQ if relative VinFP.
         *           ucode may hit concurrent access if go through VoutCmdQ
         */

        /* UnBind first */
        HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
        dsp_osal_typecast(&FpBindCfg, &CmdBufferAddr);
        HL_StlProcUnbind(RawVprocStatus, YuvInVprocStatus,
                         WriteMode, ViewZoneId, YuvVinId, RawVinId,
                         FpBindCfg);
        HL_RelCmdBuffer(CmdBufferId);

        /* Bind VirtVin and Vproc */
        HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
        dsp_osal_typecast(&FpBindCfg, &CmdBufferAddr);
        HL_StlProcbind(RawVinId, WriteMode, ViewZoneId, FpBindCfg);
        HL_RelCmdBuffer(CmdBufferId);

        /* Stop Vproc if needed */
        /*
         * To have different InputFmt, There are few methods
         * Method #0
         *   - Each InputFmt occupied one vproc(raw/yuv422/yuv420), VprocCfg/Setup only once with MaxPossbile size on each vproc
         *   - Pros : no VprocStop/Restart can save same
         *   - Cons : extra vproc definitely occupied total vproc number and consume cmem/smem of ucode
         * Method #1
         *   - One vproc for raw/yuv422/yuv420, VprocCfg only be issued once with MaxPossbile size on each vproc, VprocStop/Setup every round
         *   - Pros : no VprocCfg can save some ucode process time, only one vproc can min. cmem/smem usage
         *   - Cons : VprocStop/Setup takes time to wait vproc status transit
         * Method #2
         *   - One vproc for raw/yuv422/yuv420, VprocStop/Config/Setup every round
         *   - Pros : Only one vproc can min. cmem/smem usage, VprocCfg every time can min. every round's cmem/seme usage
         *   - Cons : VprocStop/Config/Setup takes more time(compare to #1) to wait vproc status transit
         * There we choose Method#1 as default value with Following reason
         *   - Since user already know worst case, we shall allocate worst resource with less ucode CPU cycle
         */
#ifndef SUPPORT_VPROC_RT_RESTART
        //Vproc always running
#else
        if ((RawVprocStatus == STL_VPROC_STATUS_COMPLETE) ||
            (YuvInVprocStatus == STL_VPROC_STATUS_COMPLETE)) {
            HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
            dsp_osal_typecast(&VprocStop, &CmdBufferAddr);
            VprocStop->channel_id = (UINT8)ViewZoneId;
            VprocStop->stop_cap_seq_no = 0U; //obsoleted
            VprocStop->reset_option = 0U; //obsoleted

            Rval = AmbaHL_CmdVprocStop(WriteMode, VprocStop);
            HL_RelCmdBuffer(CmdBufferId);
            if (Rval == OK) {
                Rval = DSP_WaitVprocState((UINT8)ViewZoneId, DSP_VPROC_STATUS_TIMER, MODE_SWITCH_TIMEOUT, 1U);
            }
            if (Rval == OK) {
#ifdef SUPPORT_VPROC_RT_RESTART_MTH_1
                HL_GetStlInfo(HL_MTX_OPT_GET, &StlInfo);
                StlInfo.RawInVprocStatus = STL_VPROC_STATUS_CONFIG;
                if (YuvInVprocStatus == STL_VPROC_STATUS_COMPLETE) {
                    StlInfo.YuvInVprocStatus = STL_VPROC_STATUS_CONFIG;
                }
                HL_SetStlInfo(HL_MTX_OPT_SET, &StlInfo);
                RawVprocStatus = STL_VPROC_STATUS_CONFIG;
#else //method2
                HL_GetStlInfo(HL_MTX_OPT_GET, &StlInfo);
                StlInfo.RawInVprocStatus = STL_VPROC_STATUS_IDLE;
                if (YuvInVprocStatus == STL_VPROC_STATUS_COMPLETE) {
                    StlInfo.YuvInVprocStatus = STL_VPROC_STATUS_IDLE;
                }
                HL_SetStlInfo(HL_MTX_OPT_SET, &StlInfo);
                VprocStatus = STL_VPROC_STATUS_IDLE;
#endif
            }
        }
#endif

        //Vproc Config
        if (Rval == OK) {
            HL_CalcExpNumHeight(pRawIn, pIsoCfg, &HdrExpNum, &InputHeight, &StlInfo);

            /* Activate Vproc */
            if (RawVprocStatus == STL_VPROC_STATUS_IDLE) {
                HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
                dsp_osal_typecast(&VprocCfg, &CmdBufferAddr);
                Rval = HL_StillVprocConfig(VprocCfg, ViewZoneId, DSP_VPROC_INPUT_RAW_RGB, HdrExpNum, WriteMode);
                HL_RelCmdBuffer(CmdBufferId);
            }
        }

        //VprocSetup and SetExtMem for Vproc
        if (Rval == OK) {
            Rval = HL_StillR2ySetupVproc(WriteMode, RawVprocStatus, ViewZoneId, HdrExpNum,
                                         pRawIn, pYuvOut, pIsoCfg, &StlInfo);
        }
        HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
        dsp_osal_typecast(&VinExtMem, &CmdBufferAddr);
        //SetExtMem for Vin
        if (Rval == OK) {
            Compression = (UINT16)StlInfo.IkCompression;
            HL_FillSetExtRawMemOfRawBuf(VinExtMem,
                                        pRawIn,
                                        Compression,
                                        StlInfo.RawInVirtVinId + AMBA_DSP_MAX_VIN_NUM,
                                        InputHeight);

            Rval = AmbaHL_CmdVinSetExtMem(WriteMode, VinExtMem);
        }
        if (Rval == OK) {
            HL_SetAuxBuffer(pRawIn, pAuxBufIn, pIsoCfg,
                            VinExtMem, &StlInfo, InputHeight, WriteMode);
        }
        HL_RelCmdBuffer(CmdBufferId);

        //Send Input Data for Vin
        if (Rval == OK) {
            Rval = HL_R2ySendInputData(WriteMode, ViewZoneId,
                                       pRawIn, pYuvOut->DataFmt, pIsoCfg, Compression, &StlInfo);
        }
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("AmbaDSP_StillRaw2Yuv Error[0x%X]", Rval, 0U, 0U, 0U, 0U);
    }

    return Rval;
}

static inline UINT32 HL_StillRaw2RawChk(const AMBA_DSP_RAW_BUF_s *pRawIn,
                                        const AMBA_DSP_BUF_s *pAuxBufIn,
                                        const AMBA_DSP_RAW_BUF_s *pRawOut,
                                        const AMBA_DSP_ISOCFG_CTRL_s *pIsoCfg,
                                        const UINT32 Opt,
                                        const UINT64 *pAttachedRawSeq)
{
    UINT32 Rval;
    const Dsp_DumpApi_t* DumpApi = AmbaDSP_GetDumpApiFunc();
    const Dsp_CheckApi_t* CheckApi = AmbaDSP_GetCheckApiFunc();
    const Dsp_TimingApi_t* TimingApi = AmbaDSP_GetTimingApiFunc();
    const Dsp_ResourceApi_t* ResourceApi = AmbaDSP_GetResourceApiFunc();
    CTX_STILL_INFO_s StlInfo = {0};

    /* DSP CtxInit */
    Rval = HL_CtxInit();

    /* Input sanity check */
    if ((Rval == OK) &&
        (DumpApi != NULL)) {
        DumpApi->pStillRaw2Raw(pRawIn, pAuxBufIn, pRawOut, pIsoCfg, Opt, pAttachedRawSeq);
    }
    if ((Rval == OK) &&
        (CheckApi != NULL)) {
        Rval = CheckApi->pStillRaw2Raw(pRawIn, pAuxBufIn, pRawOut, pIsoCfg, Opt, pAttachedRawSeq);
    }
    /* HW/SW resources check */
    if ((Rval == OK) &&
        (ResourceApi != NULL)) {
        Rval = ResourceApi->pStillRaw2Raw(pRawIn, pAuxBufIn, pRawOut, pIsoCfg, Opt, pAttachedRawSeq);
    }

    if (Rval == OK) {
        HL_GetStlInfo(HL_MTX_OPT_ALL, &StlInfo);
        /* Logic sanity check */
        if ((StlInfo.RawInVirtVinId == DSP_VIRT_VIN_IDX_INVALID) ||
            (StlInfo.RawInVprocId == DSP_VIRT_VIN_IDX_INVALID)) {
            Rval = DSP_ERR_0004;
        }
    }

    /* Timing sanity check */
    if ((Rval == OK) &&
        (TimingApi != NULL)) {
        Rval = TimingApi->pStillRaw2Raw(pRawIn, pAuxBufIn, pRawOut, pIsoCfg, Opt, pAttachedRawSeq);
    }

    return Rval;
}

static inline UINT32 HL_R2rSendInputData(UINT8 WriteMode, UINT16 ViewZoneId,
                                         const AMBA_DSP_RAW_BUF_s *pRawIn,
                                         const AMBA_DSP_ISOCFG_CTRL_s *pIsoCfg,
                                         CTX_STILL_INFO_s *pStlInfo)
{
    UINT32 Rval;
    UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
    cmd_vin_send_input_data_t *VinFeedData = HL_DefCtxCmdBufPtrVinSndData;

    HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
    dsp_osal_typecast(&VinFeedData, &CmdBufferAddr);
    Rval = HL_FillSendInputDataInR2r(VinFeedData, pIsoCfg, ViewZoneId,
                                     pRawIn->Compressed, pRawIn, pStlInfo);
    if (Rval == OK) {
        Rval = AmbaHL_CmdVinSendInputData(WriteMode, VinFeedData);
    }
    HL_RelCmdBuffer(CmdBufferId);
    if (Rval == OK) {
        HL_GetStlInfo(HL_MTX_OPT_GET, pStlInfo);
        pStlInfo->RawInVprocStatus = STL_VPROC_STATUS_RUN;
        HL_SetStlInfo(HL_MTX_OPT_SET, pStlInfo);
    }

    return Rval;
}

static inline UINT32 HL_R2rSetExtMem(UINT8 WriteMode, UINT16 ViewZoneId, UINT32 Opt,
                                         UINT16 InputHeight, UINT16 OutputHeight,
                                         const AMBA_DSP_RAW_BUF_s *pRawIn,
                                         const AMBA_DSP_RAW_BUF_s *pRawOut,
                                         const AMBA_DSP_BUF_s *pAuxBufIn,
                                         const AMBA_DSP_ISOCFG_CTRL_s *pIsoCfg,
                                         const CTX_STILL_INFO_s *pStlInfo)
{
    UINT32 Rval;
    UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
    UINT16 Compression = 0U;
    cmd_vin_set_ext_mem_t *VinExtMem = HL_DefCtxCmdBufPtrVinExtMem;
    cmd_vproc_set_ext_mem_t *VprocExtMem = HL_DefCtxCmdBufPtrVpcExtMem;

    HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
    dsp_osal_typecast(&VinExtMem, &CmdBufferAddr);
    //SetExtMem for Vin
    Compression = (UINT16)pStlInfo->IkCompression;
    HL_FillSetExtRawMemOfRawBuf(VinExtMem,
                                pRawIn,
                                Compression,
                                pStlInfo->RawInVirtVinId + AMBA_DSP_MAX_VIN_NUM,
                                InputHeight);
    Rval = AmbaHL_CmdVinSetExtMem(WriteMode, VinExtMem);

    if (Rval == OK) {
        HL_SetAuxBuffer(pRawIn, pAuxBufIn, pIsoCfg,
                        VinExtMem, pStlInfo, InputHeight, WriteMode);
    }
    HL_RelCmdBuffer(CmdBufferId);
    //SetExtMem for Vproc
    if (Opt != STL_R2R_OPT_GEN_AAA) {
        HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
        dsp_osal_typecast(&VprocExtMem, &CmdBufferAddr);
        if (Rval == OK) {
            Compression = (UINT16)pStlInfo->IkCompression;
            HL_FillSetVprocExtMemOfRawBuf(VprocExtMem,
                                                pRawOut,
                                                Compression,
                                                ViewZoneId,
                                                OutputHeight);
            Rval = AmbaHL_CmdVprocSetExtMem(WriteMode, VprocExtMem);
        }
        if (Rval == OK) {
            HL_SetAuxBuffer(pRawOut, pAuxBufIn, pIsoCfg,
                            VinExtMem, pStlInfo, OutputHeight, WriteMode);
        }
        HL_RelCmdBuffer(CmdBufferId);
    }
    return Rval;
}

/**
* Still Raw2Raw
* @param [in]  pRawIn input raw buffer
* @param [in]  pAuxBufIn input aux buffer
* @param [in]  pRawOut output yuv buffer
* @param [in]  pIsoCfg Iso config structure
* @param [in]  Opt R2R option
* @param [out] pAttachedRawSeq attached raw sequence
* @return ErrorCode
*/
UINT32 dsp_still_raw2raw(const AMBA_DSP_RAW_BUF_s *pRawIn,
                         const AMBA_DSP_BUF_s *pAuxBufIn,
                         const AMBA_DSP_RAW_BUF_s *pRawOut,
                         const AMBA_DSP_ISOCFG_CTRL_s *pIsoCfg,
                         UINT32 Opt,
                         UINT64 *pAttachedRawSeq)
{
    //FIXME, ring buffer?
    UINT8 WriteMode = AMBA_DSP_CMD_VDSP_NORMAL_WRITE, HdrExpNum = 1U;
    UINT8 RawVprocStatus, YuvInVprocStatus;
    UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
    UINT16 ViewZoneId, YuvVinId, RawVinId;
    UINT16 InputHeight = 0U;
    UINT16 OutputHeight = 0U;
    UINT32 Rval;
    CTX_STILL_INFO_s StlInfo = {0};
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;
    cmd_vproc_cfg_t *VprocCfg = HL_DefCtxCmdBufPtrVpc;
    cmd_binding_cfg_t *FpBindCfg = HL_DefCtxCmdBufPtrBind;
    cmd_vproc_setup_t *VprocSetup = HL_DefCtxCmdBufPtrVpcSetup;
#ifdef SUPPORT_VPROC_RT_RESTART
    cmd_vproc_stop_t *VprocStop = HL_DefCtxCmdBufPtrVpcStop;
#endif

    //misra
    (void)Opt;
    AmbaMisra_TouchUnused(pAttachedRawSeq);

    Rval = HL_StillRaw2RawChk(pRawIn, pAuxBufIn, pRawOut, pIsoCfg, Opt, pAttachedRawSeq);

    HL_GetStlInfo(HL_MTX_OPT_ALL, &StlInfo);

    /* Body */
    if (Rval == OK) {
        HL_GetResourcePtr(&Resource);

        ViewZoneId = StlInfo.RawInVprocId;
        RawVprocStatus = StlInfo.RawInVprocStatus;
        YuvInVprocStatus = StlInfo.YuvInVprocStatus;
        YuvVinId = StlInfo.YuvInVirtVinId + AMBA_DSP_MAX_VIN_NUM;
        RawVinId = StlInfo.RawInVirtVinId + AMBA_DSP_MAX_VIN_NUM;
        WriteMode = HL_GetVin2CmdNormalWrite(RawVinId);
//FIXME, Binding manager
        /*
         * 20190614, Per ChenHan, Bind/Unbind shall go through VinCmdQ if relative VinFP.
         *           ucode may hit concurrent access if go through VoutCmdQ
         */

        /* UnBind first */
        HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
        dsp_osal_typecast(&FpBindCfg, &CmdBufferAddr);
        HL_StlProcUnbind(RawVprocStatus, YuvInVprocStatus,
                         WriteMode, ViewZoneId, YuvVinId, RawVinId,
                         FpBindCfg);
        HL_RelCmdBuffer(CmdBufferId);

        /* Bind VirtVin and Vproc */
        HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
        dsp_osal_typecast(&FpBindCfg, &CmdBufferAddr);
        HL_StlProcbind(RawVinId, WriteMode, ViewZoneId, FpBindCfg);
        HL_RelCmdBuffer(CmdBufferId);

        /* Stop Vproc if needed */
        /*
         * To have different InputFmt, There are few methods
         * Method #0
         *   - Each InputFmt occupied one vproc(raw/yuv422/yuv420), VprocCfg/Setup only once with MaxPossbile size on each vproc
         *   - Pros : no VprocStop/Restart can save same
         *   - Cons : extra vproc definitely occupied total vproc number and consume cmem/smem of ucode
         * Method #1
         *   - One vproc for raw/yuv422/yuv420, VprocCfg only be issued once with MaxPossbile size on each vproc, VprocStop/Setup every round
         *   - Pros : no VprocCfg can save some ucode process time, only one vproc can min. cmem/smem usage
         *   - Cons : VprocStop/Setup takes time to wait vproc status transit
         * Method #2
         *   - One vproc for raw/yuv422/yuv420, VprocStop/Config/Setup every round
         *   - Pros : Only one vproc can min. cmem/smem usage, VprocCfg every time can min. every round's cmem/seme usage
         *   - Cons : VprocStop/Config/Setup takes more time(compare to #1) to wait vproc status transit
         * There we choose Method#1 as default value with Following reason
         *   - Since user already know worst case, we shall allocate worst resource with less ucode CPU cycle
         */
#ifndef SUPPORT_VPROC_RT_RESTART
        //Vproc always running
#else
        if ((RawVprocStatus == STL_VPROC_STATUS_COMPLETE) ||
            (YuvInVprocStatus == STL_VPROC_STATUS_COMPLETE)) {

            HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
            dsp_osal_typecast(&VprocStop, &CmdBufferAddr);
            VprocStop->channel_id = (UINT8)ViewZoneId;
            VprocStop->stop_cap_seq_no = 0U; //obsoleted
            VprocStop->reset_option = 0U; //obsoleted

            Rval = AmbaHL_CmdVprocStop(WriteMode, VprocStop);
            HL_RelCmdBuffer(CmdBufferId);
            if (Rval == OK) {
                Rval = DSP_WaitVprocState((UINT8)ViewZoneId, DSP_VPROC_STATUS_TIMER, MODE_SWITCH_TIMEOUT, 1U);
                if (Rval == OK) {
#ifdef SUPPORT_VPROC_RT_RESTART_MTH_1
                    HL_GetStlInfo(HL_MTX_OPT_GET, &StlInfo);
                    StlInfo.RawInVprocStatus = STL_VPROC_STATUS_CONFIG;
                    if (YuvInVprocStatus == STL_VPROC_STATUS_COMPLETE) {
                        StlInfo.YuvInVprocStatus = STL_VPROC_STATUS_CONFIG;
                    }
                    HL_SetStlInfo(HL_MTX_OPT_SET, &StlInfo);
                    RawVprocStatus = STL_VPROC_STATUS_CONFIG;
#else //method2
                    HL_GetStlInfo(HL_MTX_OPT_GET, &StlInfo);
                    StlInfo.RawInVprocStatus = STL_VPROC_STATUS_IDLE;
                    if (YuvInVprocStatus == STL_VPROC_STATUS_COMPLETE) {
                        StlInfo.YuvInVprocStatus = STL_VPROC_STATUS_IDLE;
                    }
                    HL_SetStlInfo(HL_MTX_OPT_SET, &StlInfo);
                    VprocStatus = STL_VPROC_STATUS_IDLE;
#endif
                }
            }
        }
#endif
        if (Rval == OK) {
            HL_CalcExpNumHeight(pRawIn, pIsoCfg, &HdrExpNum, &InputHeight, &StlInfo);
            HL_CalcExpNumHeight(pRawOut, pIsoCfg, &HdrExpNum, &OutputHeight, &StlInfo);
            /* Activate Vproc */
            //Vproc Config
            if (RawVprocStatus == STL_VPROC_STATUS_IDLE) {
                HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
                dsp_osal_typecast(&VprocCfg, &CmdBufferAddr);
                Rval = HL_StillVprocConfig(VprocCfg, ViewZoneId, DSP_VPROC_INPUT_RAW_RGB, HdrExpNum, WriteMode);
                HL_RelCmdBuffer(CmdBufferId);
            }
        }
        //Vproc setup
        if (Rval == OK) {
            if ((RawVprocStatus == STL_VPROC_STATUS_IDLE) ||
                (RawVprocStatus == STL_VPROC_STATUS_CONFIG) ||
                (RawVprocStatus == STL_VPROC_STATUS_STOPPED)) {
                HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
                dsp_osal_typecast(&VprocSetup, &CmdBufferAddr);
                Rval = HL_FillVprocSetupInR2r(VprocSetup, pRawIn, pAuxBufIn, ViewZoneId, WriteMode, Opt);
                HL_RelCmdBuffer(CmdBufferId);
            }
        }
        if (Rval == OK) {
            Rval = HL_R2rSetExtMem(WriteMode, ViewZoneId, Opt,
                            InputHeight, OutputHeight,
                            pRawIn, pRawOut, pAuxBufIn, pIsoCfg, &StlInfo);
        }
        if (Rval == OK) {
            //Send Input Data for Vin
            Rval = HL_R2rSendInputData(WriteMode, ViewZoneId, pRawIn, pIsoCfg, &StlInfo);
        }
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("AmbaDSP_StillRaw2Raw Error[0x%X][%d]", Rval, 0U, 0U, 0U, 0U);
    }

    return Rval;
}
