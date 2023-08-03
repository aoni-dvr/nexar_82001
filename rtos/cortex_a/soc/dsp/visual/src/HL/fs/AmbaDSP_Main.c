/**
 *  @file AmbaDSP_Main.c
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
 *  @details Implementation of SSP general API
 *
 */

#include "AmbaDSP_Int.h"
#include "AmbaDSP_Log.h"
#include "AmbaDSP_Cmds.h"
#include "AmbaDSP_ApiDump.h"
#include "AmbaDSP_ApiCheck.h"
#include "AmbaDSP_ApiTiming.h"
#include "AmbaDSP_EventCtrl.h"
#include "AmbaDSP_EncodeAPI.h"
#include "AmbaDSP_ApiResource.h"
#include "AmbaDSP_MsgDispatcher.h"
#include "AmbaDSP_ContextUtility.h"
#include "dsp_priv_api.h"
#include "ambadsp_ioctl.h"
#include "AmbaDSP_EncodeUtility.h"

extern void HL_GetUcodeBaseAddr(ULONG *pCoreAddr, ULONG *pMeAddr, ULONG *pMdxfAddr, ULONG *pDefaultDataAddr);
void HL_GetUcodeBaseAddr(ULONG *pCoreAddr, ULONG *pMeAddr,
                         ULONG *pMdxfAddr, ULONG *pDefaultDataAddr)
{
    osal_ucode_addr_t BaseAddr = {0};

    (void)dsp_osal_get_ucode_base_addr(&BaseAddr);
    *pCoreAddr = BaseAddr.CodeAddr;
    *pMeAddr = BaseAddr.MeAddr;
    *pMdxfAddr = BaseAddr.MdxfAddr;
    *pDefaultDataAddr = BaseAddr.DefBinAddr;
}

extern void HL_GetUcodeEndAddr(ULONG *pCoreAddr, ULONG *pMeAddr, ULONG *pMdxfAddr, ULONG *pDefaultDataAddr);
void HL_GetUcodeEndAddr(ULONG *pCoreAddr, ULONG *pMeAddr,
                        ULONG *pMdxfAddr, ULONG *pDefaultDataAddr)
{
    osal_ucode_addr_t BaseAddr = {0};

    (void)dsp_osal_get_ucode_end_addr(&BaseAddr);
    *pCoreAddr = BaseAddr.CodeAddr;
    *pMeAddr = BaseAddr.MeAddr;
    *pMdxfAddr = BaseAddr.MdxfAddr;
    *pDefaultDataAddr = BaseAddr.DefBinAddr;
}

void HL_GetUCodeVerInfo(AMBA_DSP_VERSION_INFO_s *pInfo)
{
    osal_ucode_addr_t BaseAddr = {0};
    const ucode_info_t *uInfo;
    const AMBA_DSP_CHIP_DATA_s *pChipData;
    ULONG ULAddr = 0UL;

    (void)dsp_osal_get_ucode_base_addr(&BaseAddr);
    BaseAddr.CodeAddr += (UINT32)DSP_UCODE_INFO_OFFSET;
    dsp_osal_typecast(&uInfo, &BaseAddr.CodeAddr);
    pInfo->Day = (UINT8)(uInfo->ucode_date & (UINT32)0x0000FFU);
    pInfo->Month = (UINT8)((uInfo->ucode_date & (UINT32)0x0000FF00U) >> 8U);
    pInfo->Year = (UINT16)((uInfo->ucode_date & (UINT32)0xFFFF0000U) >> 16U);
    pInfo->UCodeVer = uInfo->ucode_version;
    pInfo->ApiVer = uInfo->ucode_api_version;
    pInfo->SiliconVer = uInfo->ucode_silicon_version;
    pInfo->LinkBase = uInfo->ucode_link_base;

    (void)DSP_GetDspChipDataBuf(&ULAddr);
    dsp_osal_typecast(&pChipData, &ULAddr);
    pInfo->ChipId = pChipData->Id;
}

static void HL_SetUCodeVerInfo(AMBA_DSP_VERSION_INFO_s *pInfo)
{
    osal_ucode_addr_t BaseAddr = {0};
    const ucode_info_t *uInfo;
    const AMBA_DSP_CHIP_DATA_s *pChipData;
    ULONG ULAddr = 0UL;

    (void)dsp_osal_get_ucode_base_addr(&BaseAddr);
    BaseAddr.CodeAddr += (UINT32)DSP_UCODE_INFO_OFFSET;
    dsp_osal_typecast(&uInfo, &BaseAddr.CodeAddr);
    pInfo->Day = (UINT8)(uInfo->ucode_date & (UINT32)0x0000FFU);
    pInfo->Month = (UINT8)((uInfo->ucode_date & (UINT32)0x0000FF00U) >> 8U);
    pInfo->Year = (UINT16)((uInfo->ucode_date & (UINT32)0xFFFF0000U) >> 16U);
    pInfo->UCodeVer = uInfo->ucode_version;
    pInfo->ApiVer = uInfo->ucode_api_version;
    pInfo->SiliconVer = uInfo->ucode_silicon_version;
    pInfo->LinkBase = uInfo->ucode_link_base;

    (void)DSP_GetDspChipDataBuf(&ULAddr);
    dsp_osal_typecast(&pChipData, &ULAddr);
    pInfo->ChipId = pChipData->Id;

#if 0 //let App to print
    AmbaLL_LogUInt5("==========uCode Info==========", 0U, 0U, 0U, 0U, 0U);
    AmbaLL_LogUInt5("version: %d",        pInfo->UCodeVer, 0U, 0U, 0U, 0U);
    AmbaLL_LogUInt5("date: %d/%d/%d",     pInfo->Year, pInfo->Month, pInfo->Day, 0U, 0U);
    AmbaLL_LogUInt5("api: %d",            pInfo->ApiVer, 0U, 0U, 0U, 0U);
    AmbaLL_LogUInt5("silicon: %d",        pInfo->SiliconVer, 0U, 0U, 0U, 0U);
    AmbaLL_LogUInt5("init_data: %x",      pInfo->LinkBase, 0U, 0U, 0U, 0U);
    AmbaLL_LogUInt5("DspDrv: %d",         pInfo->SSPVer, 0U, 0U, 0U, 0U);
    AmbaLL_LogUInt5("ChipId: %d",         pInfo->ChipId, 0U, 0U, 0U, 0U);
    AmbaLL_LogUInt5("=============================", 0U, 0U, 0U, 0U, 0U);
#endif
}

void HL_GetProtectBufInfo(ULONG *Addr, UINT32 *Size)
{
    if ((Addr != NULL) && (Size != NULL)) {
        (void)DSP_GetProtectBuf(Addr, Size);
    }
}

void HL_SwitchDspSysState(const UINT8 CurOpModeStatus)
{
    UINT8 CurDspSysState = AMBA_DSP_SYS_STATE_LIVEVIEW;
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;

    if ((CurOpModeStatus == DSP_REPORTED_PROF_2_IDLE) || (CurOpModeStatus == DSP_REPORTED_PROF_IDLE)) {
        CurDspSysState = AMBA_DSP_SYS_STATE_SENSORLESS;
    } else {
        CurDspSysState = AMBA_DSP_SYS_STATE_LIVEVIEW;
    }

    HL_GetResourceLock(&Resource);

    AmbaLL_LogUInt5("DspSysState update %d to %d", Resource->DspSysState, CurDspSysState, 0U, 0U, 0U);
    Resource->DspSysState = CurDspSysState;
    HL_GetResourceUnLock();
}

static inline UINT32 HL_SwitchProfileParkVoutSourceToBG(const CTX_VOUT_INFO_s *pVoutInfo)
{
    UINT32 Rval = DSP_ERR_NONE, ErrLine = 0U;
    UINT16 i;
#ifndef SUPPORT_VOUT_BG_ONE_SYNC_RESET
    UINT32 ActualFlg = 0U;
#endif
    /* Park vout source to BG */
    for (i = 0U; i<AMBA_DSP_MAX_VOUT_NUM; i++) {
        if ((pVoutInfo[i].DisplayStatus == DSP_VOUT_DISPLAY_START) &&
            (pVoutInfo[i].VideoEnable > 0U) &&
            (pVoutInfo[i].Source != DSP_VOUT_SRC_BG)) {
            Rval = HL_VoutVideoSetupSourceSelect((UINT8)i, DSP_VOUT_SRC_BG, HL_VOUT_SOURCE_NONE);
            DSP_FillErrline(Rval, &ErrLine, __LINE__);
            if (Rval == DSP_ERR_NONE) {
                Rval = HL_VoutVideoSetup((UINT8)i, AMBA_DSP_CMD_VDSP_NORMAL_WRITE);
                DSP_FillErrline(Rval, &ErrLine, __LINE__);
            }
            /* 2019/10/15, no need to wait one sync, if doing reset together */
#ifndef SUPPORT_VOUT_BG_ONE_SYNC_RESET
            if (Rval == DSP_ERR_NONE) {
                Rval = LL_WaitVoutInterrupt((UINT32)1U << i, &ActualFlg, WAIT_INTERRUPT_TIMEOUT);
                DSP_FillErrline(Rval, &ErrLine, __LINE__);
            }
#endif
        }

        /* Need to stop mixer by reset command before profile switch
         * Enc and Dec has it own variable control Vout,
         * If not stopped vout, running vout will corrupt new profile smem
         * 2019/10/02, According ChenHan, we always need this reset
         */
#if 0
        if ((DspOpModePlayback == 1U) &&
            (VoutInfo[i].DisplayStatus == DSP_VOUT_DISPLAY_START) &&
            (VoutInfo[i].VideoEnable > 0U) &&
            (VoutInfo[i].OsdEnable > 0U)) {
#else
        if ((pVoutInfo[i].DisplayStatus == DSP_VOUT_DISPLAY_START) &&
            (pVoutInfo[i].VideoEnable > 0U)) {
#endif
            if (Rval == DSP_ERR_NONE) {
                Rval = HL_VoutResetSetup((UINT8)i, AMBA_DSP_CMD_VDSP_NORMAL_WRITE);
                DSP_FillErrline(Rval, &ErrLine, __LINE__);
            }
            if (Rval == DSP_ERR_NONE) {
                /* After VoutReset, VOUT-ISR will be gone as well, so we shall wait vdsp0 instead */
                Rval = DSP_WaitVdspEvent((UINT32)EVENT_FLAG_INT_VDSP0_EVENT, 1, WAIT_INTERRUPT_TIMEOUT);
                DSP_FillErrline(Rval, &ErrLine, __LINE__);
            }
        }
    }

    if (Rval != DSP_ERR_NONE) {
        AmbaLL_LogUInt5("HL_SwitchProfileParkVoutSourceToBG Error[0x%x][%u]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

static inline UINT32 HL_SwitchProfilePreproc(UINT8 TargetProfile,
                                             UINT8 DspOpModeCamera,
                                             UINT8 DspOpModePlayback,
                                             const CTX_VOUT_INFO_s *pVoutInfo)
{
#ifndef SUPPORT_VOUT_BG_ONE_SYNC_RESET
    UINT32 ActualFlg = 0U;
#endif
    UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
    UINT16 i;
    UINT32 Rval = DSP_ERR_NONE, ErrLine = 0U;
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;
    cmd_dsp_suspend_profile_t *SusOpMode = HL_DefCtxCmdBufPtrSus;

    if ((DspOpModeCamera == 1U) ||
        (DspOpModePlayback == 1U)) {

        Rval = HL_SwitchProfileParkVoutSourceToBG(pVoutInfo);

        /* Park DSP status */
//FIXME, make sure VDSP_NORMAL always exist
        if (Rval == DSP_ERR_NONE) {
            HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
            dsp_osal_typecast(&SusOpMode, &CmdBufferAddr);
            Rval = AmbaHL_CmdDspSuspendProfile(AMBA_DSP_CMD_VDSP_NORMAL_WRITE, SusOpMode);
            HL_RelCmdBuffer(CmdBufferId);
            DSP_FillErrline(Rval, &ErrLine, __LINE__);
        }
        if (Rval == DSP_ERR_NONE) {
            Rval = DSP_WaitProfState(DSP_PROF_STATUS_IDLE, MODE_SWITCH_TIMEOUT);
            DSP_FillErrline(Rval, &ErrLine, __LINE__);
        }

        /* Reset certain information */
        HL_GetResourceLock(&Resource);
        Resource->ProfWorkSize = 0U;
        HL_GetResourceUnLock();

        for (i = 0U; i<AMBA_DSP_NUM_SYNC_CMD_BUF_CTRL; i++) {
            if (Rval == DSP_ERR_NONE) {
                Rval = DSP_ClearGroupCmdBuffer((UINT8)i);
                DSP_FillErrline(Rval, &ErrLine, __LINE__);
            }
        }
        DSP_ClrDefCfgBuffer();

        if (DspOpModeCamera == 1U) {
            // DO NOTHING
        } else if (DspOpModePlayback == 1U) {
            CTX_VID_DEC_INFO_s VidDecInfo = {0};
            UINT16 DecIdx;

            for (DecIdx=0U; DecIdx < AMBA_DSP_MAX_DEC_STREAM_NUM; DecIdx++) {
                HL_GetVidDecInfo(HL_MTX_OPT_GET, DecIdx, &VidDecInfo);
                if (TargetProfile != DSP_PROF_STATUS_PLAYBACK) {
                    VidDecInfo.State = VIDDEC_STATE_INVALID;
                }
                VidDecInfo.DecoderMode = DECODE_MODE_INVALID;
                HL_SetVidDecInfo(HL_MTX_OPT_SET, DecIdx, &VidDecInfo);
            }
        } else {
            // DO NOTHING
        }
    }

    if ((Rval != DSP_ERR_NONE) && (ErrLine != 0U)) {
        AmbaLL_LogUInt5("HL_SwitchProfilePreproc Error[0x%x][%u]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

/*
 * Used after DSP BOOTED
 * Preproc :
 *   - check vout status, witch to BG when VOUT on
 *   - Suspend DSP if DSP still in any active profile
 *   - Clear GrpCmdBuf
 *   - Clear DefCmdBuf
 *   - Clear Original Profile CTX setting
 * Proc :
 *   - Flow must fill it's DefCmd for new Profile before enter this Timing
 *   - Active Profile and wait until new Profile activated
 * PostProc :
 *   - DO NOTHING now
 */
UINT32 HL_SwitchProfile(UINT8 TargetProfile, UINT8 Timing)
{
    UINT8 DspOpModeCamera, DspOpModePlayback;
    UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
    UINT16 i;
    UINT32 Rval = DSP_ERR_NONE, ErrLine = 0U;
    CTX_VOUT_INFO_s VoutInfo[AMBA_DSP_MAX_VOUT_NUM] = {0};
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;
    cmd_dsp_set_profile_t *DspSetProfile = HL_DefCtxCmdBufPtrSetPrf;

    DspOpModeCamera = (DSP_GetProfState() == DSP_PROF_STATUS_CAMERA)? 1U: 0U;
    DspOpModePlayback = (DSP_GetProfState() == DSP_PROF_STATUS_PLAYBACK)? 1U: 0U;
    for (i = 0U; i < AMBA_DSP_MAX_VOUT_NUM; i++) {
        HL_GetVoutInfo(HL_MTX_OPT_ALL, i, &VoutInfo[i]);
    }
    HL_GetResourcePtr(&Resource);

    AmbaLL_LogUInt5("Switch2Prof[%d %d] Timing[%d]", TargetProfile, DSP_GetProfState(), Timing, 0U, 0U);
    for (i = 0U; i < AMBA_DSP_MAX_VOUT_NUM; i++) {
        AmbaLL_LogUInt5("VoutCmd[%d] VdoEn[%d] OsdEn[%d] Csc[%d]", i,
                         VoutInfo[i].VideoEnable, VoutInfo[i].OsdEnable,
                         VoutInfo[i].CscUpdated, 0);
        AmbaLL_LogUInt5("           MaxWidth[%d] MaxOsd[%d] StDisp[%d] StMix[%d]",
                              Resource->MaxVoutWidth[i], Resource->MaxOsdBufWidth[i],
                              VoutInfo[i].DisplayStatus, VoutInfo[i].MixerStatus, 0U);
        AmbaLL_LogUInt5("           StOsd[%d %d] StDve[%d %d]",
                              VoutInfo[i].OsdStatus, VoutInfo[i].DveStatus,
                              0U, 0U, 0U);
    }

    if (Timing == SWITCH_PROF_PREPROC) {
        Rval = HL_SwitchProfilePreproc(TargetProfile,
                                       DspOpModeCamera,
                                       DspOpModePlayback,
                                       VoutInfo);
    } else if (Timing == SWITCH_PROF_PROC) {
//FIXME, Add Protection to prevent any one issue Cmd in Next API
        DSP_ResetCmdMsgBuf();

        //Trigger DSP to read Cmd from DefCmdQ
        HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
        dsp_osal_typecast(&DspSetProfile, &CmdBufferAddr);
        Rval = AmbaHL_CmdDspSetProfile(AMBA_DSP_CMD_VDSP_NORMAL_WRITE/*In DspIdleMode*/, DspSetProfile);
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
        HL_RelCmdBuffer(CmdBufferId);

        if (Rval == DSP_ERR_NONE) {
            Rval = DSP_WaitProfState(TargetProfile, MODE_SWITCH_TIMEOUT);
            DSP_FillErrline(Rval, &ErrLine, __LINE__);
        }
    } else if (Timing == SWITCH_PROF_POSTPROC) {
        //DO NOTHING
    } else {
        //DO NOTHING
    }

    if ((Rval != DSP_ERR_NONE) && (ErrLine != 0U)) {
        AmbaLL_LogUInt5("HL_SwitchProfile Error[0x%x][%u]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

static inline UINT32 HL_ChkRescChangeOnLv(const CTX_RESOURCE_INFO_s *pRescInfo,
                                          const AMBA_DSP_RESOURCE_s *pResource,
                                          UINT8 *pRescChg,
                                          UINT32 *pChgLine)
{
    UINT32 Rval = DSP_ERR_NONE;
    UINT16 i;
    const AMBA_DSP_LIVEVIEW_RESOURCE_s *pLvRes = &pResource->LiveviewResource;
    const AMBA_DSP_LIVEVIEW_RESOURCE_EX_s *pLvResEx = &pResource->LiveviewResourceEx;

    if ((pRescInfo->VideoPipe[0]   != pLvRes->VideoPipe     ) ||
        (pRescInfo->LowDelayMode   != pLvRes->LowDelayMode  ) ||
        (pRescInfo->MaxVinBit      != pLvRes->MaxVinBit     ) ||
        (pRescInfo->MaxViewZoneNum != pLvRes->MaxViewZoneNum) ||
        (pRescInfo->MaxVirtVinBit  != pLvRes->MaxVirtVinBit )) {
        *pRescChg = 1U;
        *pChgLine = __LINE__;
    }

    if (*pRescChg == 0U) {
        for (i=0; i < AMBA_DSP_MAX_VIEWZONE_NUM; i++) {
            if ((pRescInfo->MaxWarpDma[i]            != pLvRes->MaxWarpDma[i]           ) ||
                (pRescInfo->MaxWarpWaitLineLuma[i]   != pLvRes->MaxWarpWaitLineLuma[i]  ) ||
                (pRescInfo->MaxWarpWaitLineChroma[i] != pLvRes->MaxWarpWaitLineChroma[i]) ||
                (pRescInfo->MaxHierWidth[i]          != pLvRes->MaxHierWidth[i]         ) ||
                (pRescInfo->MaxHierHeight[i]         != pLvRes->MaxHierHeight[i]        ) ||
                (pRescInfo->MaxLndtWidth[i]          != pLvRes->MaxLndtWidth[i]         ) ||
                (pRescInfo->MaxLndtHeight[i]         != pLvRes->MaxLndtHeight[i]        ) ||
                (pRescInfo->MaxHorWarpComp[i]        != pLvResEx->MaxHorWarpCompensation[i])) {
                *pRescChg = 1U;
                *pChgLine = __LINE__;
                break;
            }
        }
    }

    return Rval;
}

static inline void HL_ChkRescChangeOnLvVinVirtVin(const CTX_RESOURCE_INFO_s *pRescInfo,
                                                  const AMBA_DSP_LIVEVIEW_RESOURCE_s *pLvRes,
                                                  UINT8 *pRescChg,
                                                  UINT32 *pChgLine)

{
    UINT16 i;

    if (*pRescChg == 0U) {
        for (i=0; i < AMBA_DSP_MAX_VIRT_VIN_NUM; i++) {
            if ((pRescInfo->MaxVirtVinOutputNum[i] != pLvRes->MaxVirtVinOutputNum[i]) ||
                (pRescInfo->MaxVirtVinWidth[i]     != pLvRes->MaxVirtVinWidth[i]    ) ||
                (pRescInfo->MaxVirtVinHeight[i]    != pLvRes->MaxVirtVinHeight[i]   )) {
                *pRescChg = 1U;
                *pChgLine = __LINE__;
                break;
            }
        }
    }
}

static inline void HL_ChkRescChangeOnLvVinWidthHight(UINT16 i,
                                                     const CTX_RESOURCE_INFO_s *pRescInfo,
                                                     const AMBA_DSP_STILL_RESOURCE_s *pStlRes,
                                                     UINT8 *pRescChg,
                                                     UINT32 *pChgLine)

{
    if (*pRescChg == 0U) {
        if ((pRescInfo->MaxStlVinWidth[i][0U]  != pStlRes->MaxVinWidth[i] ) ||
            (pRescInfo->MaxStlVinHeight[i][0U] != pStlRes->MaxVinHeight[i])) {
            *pRescChg = 1U;
            *pChgLine = __LINE__;
        }
    }
}

static inline UINT32 HL_ChkRescChangeOnLvVinBufCmp(UINT16 i,
                                                     const CTX_RESOURCE_INFO_s *pRescInfo,
                                                     const AMBA_DSP_LIVEVIEW_RESOURCE_s *pLvRes,
                                                     UINT8 *pRescChg,
                                                     UINT32 *pChgLine)

{
    UINT32 Rval = DSP_ERR_NONE, ErrLine = 0U;
    UINT32 RawBufCmp = 0U, CeBufCmp = 0U, DefRawBufCmp = 0U, DefAuxRawBufCmp = 0U;

    Rval = dsp_osal_memcmp(&pRescInfo->RawBuf[i], &pLvRes->RawBuf[i][0U].Buf,
                                      sizeof(AMBA_DSP_RAW_BUF_s), &RawBufCmp);
    DSP_FillErrline(Rval, &ErrLine, __LINE__);

    if (Rval == DSP_ERR_NONE) {
        Rval = dsp_osal_memcmp(&pRescInfo->CeBuf[i], &pLvRes->RawBuf[i][0U].AuxBuf,
                                      sizeof(AMBA_DSP_RAW_BUF_s), &CeBufCmp);
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
    }

    if (Rval == DSP_ERR_NONE) {
        Rval = dsp_osal_memcmp(&pRescInfo->DefaultRawBuf[i], &pLvRes->DefaultRawBuf[i][0U],
                                      sizeof(AMBA_DSP_RAW_BUF_s), &DefRawBufCmp);
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
    }

    if (Rval == DSP_ERR_NONE) {
        Rval = dsp_osal_memcmp(&pRescInfo->DefaultAuxRawBuf[i], &pLvRes->DefaultAuxRawBuf[i][0U],
                                      sizeof(AMBA_DSP_RAW_BUF_s), &DefAuxRawBufCmp);
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
    }

    if ((RawBufCmp != 0U) || (CeBufCmp != 0U) || (DefRawBufCmp != 0U) || (DefAuxRawBufCmp != 0U)) {
        *pRescChg = 1U;
        *pChgLine = __LINE__;
    }

    if (Rval != DSP_ERR_NONE) {
        AmbaLL_LogUInt5("HL_ChkRescChangeOnLvVinBufCmp Error[0x%x][%u]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;

}

static inline void HL_ChkRescChangeOnLvVinRawBuf(UINT16 i,
                                                 const CTX_RESOURCE_INFO_s *pRescInfo,
                                                 const AMBA_DSP_LIVEVIEW_RESOURCE_s *pLvRes,
                                                 UINT8 *pRescChg,
                                                 UINT32 *pChgLine)

{
    if (*pRescChg == 0U) {
        if ((pRescInfo->RawAllocType[i]    != pLvRes->RawBuf[i][0U].AllocType ) ||
            (pRescInfo->RawBufNum[i]       != pLvRes->RawBuf[i][0U].BufNum    ) ||
            ((pLvRes->RawBuf[i][0U].pRawBufTbl != NULL) && (pRescInfo->RawBufTbl[i][0U] != pLvRes->RawBuf[i][0U].pRawBufTbl[0U])) ||
            ((pLvRes->RawBuf[i][0U].pAuxBufTbl != NULL) && (pRescInfo->CeBufTbl[i][0U] != pLvRes->RawBuf[i][0U].pAuxBufTbl[0U]))) {
            *pRescChg = 1U;
            *pChgLine = __LINE__;
        }
    }
}

static inline void HL_ChkRescChangeOnLvVinVirtChan(UINT16 i,
                                                   const CTX_RESOURCE_INFO_s *pRescInfo,
                                                   const AMBA_DSP_LIVEVIEW_RESOURCE_s *pLvRes,
                                                   UINT8 *pRescChg,
                                                   UINT32 *pChgLine)

{
    UINT16 j;

    if (*pRescChg == 0U) {
        for (j=0U; j < AMBA_DSP_MAX_VIRT_CHAN_NUM; j++) {
            if ((pRescInfo->MaxVinVirtChanOutputNum[i][j] != pLvRes->MaxVinVirtChanOutputNum[i][j]) ||
                (pRescInfo->MaxVinVirtChanWidth[i][j]     != pLvRes->MaxVinVirtChanWidth[i][j]    ) ||
                (pRescInfo->MaxVinVirtChanHeight[i][j]    != pLvRes->MaxVinVirtChanHeight[i][j]   )) {
                *pRescChg = 1U;
                *pChgLine = __LINE__;
                break;
            }
        }
    }
}

static inline void HL_ChkRescChangeOnLvVinTimeout(UINT16 i,
                                                  const CTX_RESOURCE_INFO_s *pRescInfo,
                                                  const AMBA_DSP_LIVEVIEW_RESOURCE_s *pLvRes,
                                                  UINT8 *pRescChg,
                                                  UINT32 *pChgLine)

{
    if (*pRescChg == 0U) {
        if ((pRescInfo->MaxVinVirtChanBit[i] != pLvRes->MaxVinVirtChanBit[i]) ||
            (pRescInfo->MaxVinBootTimeout[i] != pLvRes->MaxVinBootTimeout[i]) ||
            (pRescInfo->MaxVinTimeout[i]     != pLvRes->MaxVinTimeout[i]    ) ||
            (pRescInfo->MaxRaw2YuvDelay[i]   != pLvRes->MaxRaw2YuvDelay[i]  )) {
            *pRescChg = 1U;
            *pChgLine = __LINE__;
        }
    }
}

static inline UINT32 HL_ChkRescChangeOnLvVin(const CTX_RESOURCE_INFO_s *pRescInfo,
                                             const AMBA_DSP_RESOURCE_s *pResource,
                                             UINT8 *pRescChg,
                                             UINT32 *pChgLine)
{
    UINT32 Rval = DSP_ERR_NONE, ErrLine = 0U;
    UINT16 i;
    const AMBA_DSP_LIVEVIEW_RESOURCE_s *pLvRes = &pResource->LiveviewResource;
    const AMBA_DSP_STILL_RESOURCE_s *pStlRes = &pResource->StillResource;

    if (*pRescChg == 0U) {
        for (i=0; i < AMBA_DSP_MAX_VIN_NUM; i++) {
            HL_ChkRescChangeOnLvVinTimeout(i, pRescInfo, pLvRes, pRescChg, pChgLine);
            if (*pRescChg == 1U) {
                break;
            }

            HL_ChkRescChangeOnLvVinVirtChan(i, pRescInfo, pLvRes, pRescChg, pChgLine);

            HL_ChkRescChangeOnLvVinRawBuf(i, pRescInfo, pLvRes, pRescChg, pChgLine);

            Rval = HL_ChkRescChangeOnLvVinBufCmp(i, pRescInfo, pLvRes, pRescChg, pChgLine);
            DSP_FillErrline(Rval, &ErrLine, __LINE__);

            HL_ChkRescChangeOnLvVinWidthHight(i, pRescInfo, pStlRes, pRescChg, pChgLine);
        }
    }

    HL_ChkRescChangeOnLvVinVirtVin(pRescInfo, pLvRes, pRescChg, pChgLine);

    if (Rval != DSP_ERR_NONE) {
        AmbaLL_LogUInt5("HL_ChkRescChangeOnLvVin Error[0x%x][%u]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

static inline UINT32 HL_ChkRescChangeOnEnc(const CTX_RESOURCE_INFO_s *pRescInfo,
                                           const AMBA_DSP_RESOURCE_s *pResource,
                                           UINT8 *pRescChg,
                                           UINT32 *pChgLine)
{
    UINT32 Rval = DSP_ERR_NONE;
    UINT16 i;
    const AMBA_DSP_ENCODE_RESOURCE_s *pEncRes = &pResource->EncodeResource;
    const AMBA_DSP_EXTMEM_ENC_RESOURCE_s *pExtMemEncRes = &pResource->ExtMemEncResource;

    if (*pRescChg == 0U) {
        if (pRescInfo->MaxEncodeStream != pEncRes->MaxEncodeStream) {
            *pRescChg = 1U;
            *pChgLine = __LINE__;
        }
    }

    if (*pRescChg == 0U) {
        for (i=0; i < AMBA_DSP_MAX_STREAM_NUM; i++) {
            if ((pRescInfo->MaxGopM[i]          != pEncRes->MaxGopM[i]         ) ||
                (pRescInfo->MaxExtraReconNum[i] != pEncRes->MaxExtraReconNum[i]) ||
                (pRescInfo->MaxSearchRange[i]   != pEncRes->MaxSearchRange[i]  ) ||
                (pRescInfo->MaxStrmFmt[i]       != pEncRes->MaxStrmFmt[i]      )) {
                *pRescChg = 1U;
                *pChgLine = __LINE__;
                break;
            }
        }
    }

    /* ExtMemEnc */
    if (*pRescChg == 0U) {
        for (i=0; i < pEncRes->MaxEncodeStream; i++) {
            if ((pRescInfo->MaxExtMemWidth[i]  != pExtMemEncRes->MaxEncWidth[i]  ) ||
                (pRescInfo->MaxExtMemHeight[i] != pExtMemEncRes->MaxEncHeight[i])) {
                *pRescChg = 1U;
                *pChgLine = __LINE__;
                break;
            }
        }
    }

    return Rval;
}

static inline UINT32 HL_ChkRescChangeOnStl(const CTX_RESOURCE_INFO_s *pRescInfo,
                                           const AMBA_DSP_RESOURCE_s *pResource,
                                           UINT8 *pRescChg,
                                           UINT32 *pChgLine)
{
    UINT32 Rval = DSP_ERR_NONE;
    const AMBA_DSP_STILL_RESOURCE_s *pStlRes = &pResource->StillResource;

    if (*pRescChg == 0U) {
        if ((pRescInfo->MaxProcessFormat     != pStlRes->MaxProcessFormat ) ||
            (pRescInfo->MaxStlMainWidth      != pStlRes->MaxMainWidth     ) ||
            (pRescInfo->MaxStlMainHeight     != pStlRes->MaxMainHeight    ) ||
            (pRescInfo->MaxStlRawInputWidth  != pStlRes->MaxRawInputWidth ) ||
            (pRescInfo->MaxStlRawInputHeight != pStlRes->MaxRawInputHeight) ||
            (pRescInfo->MaxStlYuvInputWidth  != pStlRes->MaxYuvInputWidth ) ||
            (pRescInfo->MaxStlYuvInputHeight != pStlRes->MaxYuvInputHeight) ||
            (pRescInfo->MaxStlYuvEncWidth    != pStlRes->MaxYuvEncWidth   ) ||
            (pRescInfo->MaxStlYuvEncHeight   != pStlRes->MaxYuvEncHeight  )) {
            *pRescChg = 1U;
            *pChgLine = __LINE__;
        }
    }

    return Rval;
}

static inline UINT32 HL_ChkRescChangeOnDec(const CTX_RESOURCE_INFO_s *pRescInfo,
                                           const AMBA_DSP_RESOURCE_s *pResource,
                                           UINT8 *pRescChg,
                                           UINT32 *pChgLine)
{
    UINT32 Rval = DSP_ERR_NONE;
    UINT16 i;
    const AMBA_DSP_DECODE_RESOURCE_s *pDecRes = &pResource->DecodeResource;

    if (*pRescChg == 0U) {
        if ((pRescInfo->DecMaxStreamNum          != pDecRes->MaxDecodeStreamNum   ) ||
            (pRescInfo->DecMaxFrameWidth         != pDecRes->MaxFrameWidth        ) ||
            (pRescInfo->DecMaxFrameHeight        != pDecRes->MaxFrameHeight       ) ||
            (pRescInfo->DecMaxBitRate            != pDecRes->MaxBitRate           ) ||
            (pRescInfo->DecMaxVideoPlaneWidth    != pDecRes->MaxVideoPlaneWidth   ) ||
            (pRescInfo->DecMaxVideoPlaneHeight   != pDecRes->MaxVideoPlaneHeight  ) ||
            (pRescInfo->DecMaxRatioOfGopNM       != pDecRes->MaxRatioOfGopNM      ) ||
            (pRescInfo->DecBackwardTrickPlayMode != pDecRes->BackwardTrickPlayMode) ||
            (pRescInfo->DecMaxPicWidth           != pDecRes->MaxPicWidth          ) ||
            (pRescInfo->DecMaxPicHeight          != pDecRes->MaxPicHeight         )) {
            *pRescChg = 1U;
            *pChgLine = __LINE__;
        }
    }

    if (*pRescChg == 0U) {
        for (i=0U; i < AMBA_DSP_MAX_DEC_STREAM_NUM; i++) {
            if (pRescInfo->DecMaxStrmFmt[i] != pDecRes->MaxStrmFmt[i]) {
                *pRescChg = 1U;
                *pChgLine = __LINE__;
                break;
            }
        }
    }

    return Rval;
}

static inline UINT32 HL_ChkRescChangeOnDisp(const CTX_RESOURCE_INFO_s *pRescInfo,
                                            const AMBA_DSP_RESOURCE_s *pResource,
                                            UINT8 *pRescChg,
                                            UINT32 *pChgLine)
{
    UINT32 Rval = DSP_ERR_NONE;
    UINT16 i;
    const AMBA_DSP_DISPLAY_RESOURCE_s *pDispRes = &pResource->DisplayResource;

    if (*pRescChg == 0U) {
        for (i=0; i < AMBA_DSP_MAX_VOUT_NUM; i++) {
            if ((pRescInfo->MaxVoutWidth[i]   != pDispRes->MaxVoutWidth[i]  ) ||
                (pRescInfo->MaxOsdBufWidth[i] != pDispRes->MaxOsdBufWidth[i])) {
                *pRescChg = 1U;
                *pChgLine = __LINE__;
                break;
            }
        }
    }

    return Rval;
}

//#define DEBUG_RESOURCE_CHANGE
static inline UINT32 HL_CheckRescChange(const AMBA_DSP_RESOURCE_s *pResource, UINT8 *pRescChg, UINT32 *pChgLine)
{
    UINT32 Rval = DSP_ERR_NONE, ErrLine = 0U;
    CTX_RESOURCE_INFO_s *RescInfo = HL_CtxResInfoPtr;

    HL_GetResourcePtr(&RescInfo);

    /* Liveview */
    Rval = HL_ChkRescChangeOnLv(RescInfo, pResource, pRescChg, pChgLine);
    DSP_FillErrline(Rval, &ErrLine, __LINE__);
#ifdef DEBUG_RESOURCE_CHANGE
    if (*pRescChg != 0U) {
        AmbaLL_LogUInt5("RescLmt[%u]:%u @%u", __LINE__, *pRescChg, *pChgLine, 0U, 0U);
    }
#endif
    if (Rval == DSP_ERR_NONE) {
        Rval = HL_ChkRescChangeOnLvVin(RescInfo, pResource, pRescChg, pChgLine);
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
    }
#ifdef DEBUG_RESOURCE_CHANGE
    if (*pRescChg != 0U) {
        AmbaLL_LogUInt5("RescLmt[%u]:%u @%u", __LINE__, *pRescChg, *pChgLine, 0U, 0U);
    }
#endif
    /* Encode/Recon/ExtMemEnc */
    if (Rval == DSP_ERR_NONE) {
        Rval = HL_ChkRescChangeOnEnc(RescInfo, pResource, pRescChg, pChgLine);
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
    }
#ifdef DEBUG_RESOURCE_CHANGE
    if (*pRescChg != 0U) {
        AmbaLL_LogUInt5("RescLmt[%u]:%u @%u", __LINE__, *pRescChg, *pChgLine, 0U, 0U);
    }
#endif
    /* Still */
    if (Rval == DSP_ERR_NONE) {
        Rval = HL_ChkRescChangeOnStl(RescInfo, pResource, pRescChg, pChgLine);
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
    }
#ifdef DEBUG_RESOURCE_CHANGE
    if (*pRescChg != 0U) {
        AmbaLL_LogUInt5("RescLmt[%u]:%u @%u", __LINE__, *pRescChg, *pChgLine, 0U, 0U);
    }
#endif
    /* Decode */
    if (Rval == DSP_ERR_NONE) {
        Rval = HL_ChkRescChangeOnDec(RescInfo, pResource, pRescChg, pChgLine);
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
    }
#ifdef DEBUG_RESOURCE_CHANGE
    if (*pRescChg != 0U) {
        AmbaLL_LogUInt5("RescLmt[%u]:%u @%u", __LINE__, *pRescChg, *pChgLine, 0U, 0U);
    }
#endif
    /* Display */
    if (Rval == DSP_ERR_NONE) {
        Rval = HL_ChkRescChangeOnDisp(RescInfo, pResource, pRescChg, pChgLine);
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
    }
#ifdef DEBUG_RESOURCE_CHANGE
    if (*pRescChg != 0U) {
        AmbaLL_LogUInt5("RescLmt[%u]:%u @%u", __LINE__, *pRescChg, *pChgLine, 0U, 0U);
    }
#endif

    if (Rval != DSP_ERR_NONE) {
        AmbaLL_LogUInt5("HL_CheckRescChange Error[0x%x][%u]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

UINT32 dsp_set_debug_level(UINT32 Module, UINT32 Level, UINT8 CmdType)
{
    UINT32 Rval = DSP_ERR_NONE, ErrLine = 0U;
    UINT8 WriteType = AMBA_DSP_CMD_VDSP_NORMAL_WRITE;
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;

    /* DSP CtxInit */
    Rval = HL_CtxInit();
    DSP_FillErrline(Rval, &ErrLine, __LINE__);

    if (Rval == DSP_ERR_NONE) {
        if (CmdType == 1U) {
            WriteType = AMBA_DSP_CMD_VCAP_NORMAL_WRITE;
        } else {
            WriteType = AMBA_DSP_CMD_VDSP_NORMAL_WRITE;
        }

        HL_GetResourceLock(&Resource);
        Resource->DspDbgLvl = Level;
        Resource->DspDbgMod = Module;
        HL_GetResourceUnLock();

        AmbaLL_LogUInt5("[DebugLevel] Type %d, Level %d Sys %d", WriteType, Level, Resource->DspSysState, 0U, 0U);

        if (Resource->DspSysState != AMBA_DSP_SYS_STATE_UNKNOWN) {
            UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
            cmd_set_debug_level_t *DebugLevel = HL_DefCtxCmdBufPtrDbgLvl;

            HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
            dsp_osal_typecast(&DebugLevel, &CmdBufferAddr);
            DebugLevel->level = Resource->DspDbgLvl;
            DebugLevel->module = Resource->DspDbgMod;
            Rval = AmbaHL_CmdDspSetDebugLevel(WriteType, DebugLevel);
            DSP_FillErrline(Rval, &ErrLine, __LINE__);
            HL_RelCmdBuffer(CmdBufferId);
        }
    }

    if (Rval != DSP_ERR_NONE) {
        AmbaLL_LogUInt5("dsp_set_debug_level Error[0x%x][%u]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

UINT32 dsp_set_debug_thread(UINT32 ThreadValid, UINT32 ThreadMask, UINT8 CmdType)
{
    UINT32 Rval = DSP_ERR_NONE, ErrLine = 0U;
    UINT8 WriteType = AMBA_DSP_CMD_VDSP_NORMAL_WRITE;
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;

    //FIXME, Misra
    (void)ThreadValid;

    /* DSP CtxInit */
    Rval = HL_CtxInit();
    DSP_FillErrline(Rval, &ErrLine, __LINE__);

    if (Rval == OK) {
        if (CmdType == 1U) {
            WriteType = AMBA_DSP_CMD_VCAP_NORMAL_WRITE;
        } else {
            WriteType = AMBA_DSP_CMD_VDSP_NORMAL_WRITE;
        }

        HL_GetResourceLock(&Resource);
        Resource->DspDbgThrd = ThreadMask;
        Resource->DspDbgThrdValid = ThreadValid;
        HL_GetResourceUnLock();

        AmbaLL_LogUInt5("[DebugThread] ThreadMask 0x%X 0x%X", ThreadMask, ThreadValid, 0U, 0U, 0U);

        if (Resource->DspSysState != AMBA_DSP_SYS_STATE_UNKNOWN) {
            UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
            cmd_print_th_disable_mask_t *DebugThread = HL_DefCtxCmdBufPtrDbgthd;

            HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
            dsp_osal_typecast(&DebugThread, &CmdBufferAddr);
            DebugThread->orccode_mask_valid = (UINT8)DSP_GetBit(ThreadValid, 0U, 1U);
            DebugThread->orcme_mask_valid = (UINT8)DSP_GetBit(ThreadValid, 1U, 1U);
            DebugThread->orcmdxf_mask_valid = (UINT8)DSP_GetBit(ThreadValid, 2U, 1U);

            DebugThread->orccode_mask = (UINT16)DSP_GetBit(ThreadMask, 0U, 12U);
            DebugThread->orcme_mask = (UINT8)DSP_GetBit(ThreadMask, 12U, 2U);
            DebugThread->orcmdxf_mask = (UINT8)DSP_GetBit(ThreadMask, 14U, 2U);
            Rval = AmbaHL_CmdDspPrintThreadDisableMask(WriteType, DebugThread);
            DSP_FillErrline(Rval, &ErrLine, __LINE__);
            HL_RelCmdBuffer(CmdBufferId);
        }
    }

    if (Rval != DSP_ERR_NONE) {
        AmbaLL_LogUInt5("dsp_set_debug_thread Error[0x%x][%u]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

static inline void HL_MainInitOnLvBootPostprocVprocInfoSet(CTX_VIN_INFO_s *pVinInfo,
                                                           CTX_VPROC_INFO_s *pVprocInfo,
                                                           const CTX_RESOURCE_INFO_s *pResource)
{
    UINT16 VinId, ViewZoneId;

    /* Wait Vin Status */
    for (VinId = 0U; VinId < AMBA_DSP_MAX_VIN_NUM; VinId++) {
        if (0U == DSP_GetU16Bit(pResource->VinBit, VinId, 1U)) {
            continue;
        }

        if (OK != DSP_WaitVinState((UINT8)VinId, DSP_RAW_CAP_STATUS_VIDEO, MODE_SWITCH_TIMEOUT)) {
            AmbaLL_LogUInt5("%d DSP_WaitVinState %d fail", __LINE__, DSP_RAW_CAP_STATUS_VIDEO, 0U, 0U, 0U);
        } else {
            HL_GetVinInfo(HL_MTX_OPT_GET, VinId, pVinInfo);
            pVinInfo->VinCtrl.VinState = DSP_VIN_STATUS_ACTIVE;
            HL_SetVinInfo(HL_MTX_OPT_SET, VinId, pVinInfo);
        }
    }
    HL_LiveviewVoutSetupImpl(AMBA_DSP_CMD_VDSP_NORMAL_WRITE, VOUT_CFG_TIME_POST_VIN);
    /* Wait Vproc Status*/
    for (ViewZoneId = 0U; ViewZoneId < pResource->ViewZoneNum; ViewZoneId++) {
        CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;

        HL_GetViewZoneInfoPtr(ViewZoneId, &ViewZoneInfo);
        if ((1U == DSP_GetBit(pResource->ViewZoneDisableBit, ViewZoneId, 1U)) ||
            (ViewZoneInfo->InputFromMemory == VIN_SRC_FROM_DEC) ||
            (ViewZoneInfo->InputFromMemory == VIN_SRC_FROM_RECON)) {
            continue;
        }
        if (ViewZoneInfo->Pipe == DSP_DRAM_PIPE_RAW_ONLY) {
            continue;
        }

        if (OK != DSP_WaitVprocState((UINT8)ViewZoneId, DSP_VPROC_STATUS_ACTIVE, MODE_SWITCH_TIMEOUT, 1U)) {
            AmbaLL_LogUInt5("[Err][%d] DSP_WaitVprocState %d", __LINE__, DSP_VPROC_STATUS_ACTIVE, 0U, 0U, 0U);
            HL_GetVprocInfo(HL_MTX_OPT_GET, ViewZoneId, pVprocInfo);
            pVprocInfo->Status = DSP_VPROC_STATUS_INVALID;
            HL_SetVprocInfo(HL_MTX_OPT_SET, ViewZoneId, pVprocInfo);
        } else {
            HL_GetVprocInfo(HL_MTX_OPT_GET, ViewZoneId, pVprocInfo);
            pVprocInfo->Status = DSP_VPROC_STATUS_ACTIVE;
            HL_SetVprocInfo(HL_MTX_OPT_SET, ViewZoneId, pVprocInfo);
        }
    }
}

static inline UINT32 HL_MainInitOnLvBootPostproc(const CTX_RESOURCE_INFO_s *pResource)
{
    UINT32 Rval = DSP_ERR_NONE, ErrLine = 0U;
    UINT16 VinId, ViewZoneId, VinPostCfgBitMask = 0U;
    CTX_VPROC_INFO_s VprocInfo = {0};
    CTX_VIN_INFO_s VinInfo = {0};

    Rval = DSP_WaitProfState(DSP_PROF_STATUS_CAMERA, MODE_SWITCH_TIMEOUT);
    DSP_FillErrline(Rval, &ErrLine, __LINE__);
    if (Rval == DSP_ERR_NONE) {
        /* set master for all viewzone, since this is complete enable group */
        for (ViewZoneId = 0U; ViewZoneId < pResource->ViewZoneNum; ViewZoneId++) {
            UINT16 GrpId = 0U, FirstVprocId = 0U;
            Rval = HL_GetVprocGroupIdx(ViewZoneId, &GrpId, 0/*IsStlProc*/);
            DSP_FillErrline(Rval, &ErrLine, __LINE__);
            if (Rval == DSP_ERR_NONE) {
                HL_GetGroupFirstVprocId(GrpId, &FirstVprocId);
                if (FirstVprocId != AMBA_DSP_MAX_VIEWZONE_NUM) {
                    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;

                    HL_GetViewZoneInfoLock(FirstVprocId, &ViewZoneInfo);
//                  AmbaLL_LogUInt5("HL_MainInitOnLvBootPostproc vz:%u FirstVprocId:%u IsMaster:%u -> 1",
//                                   ViewZoneId, FirstVprocId, ViewZoneInfo->IsMaster, 0U, 0U);
                    ViewZoneInfo->IsMaster = 1U;
                    HL_GetViewZoneInfoUnLock(FirstVprocId);
                }
            } else {
                break;
            }
        }

        if (Rval == DSP_ERR_NONE) {
            /* Find Vin needing PostCfg */
            for (VinId=0U; VinId<AMBA_DSP_MAX_VIN_NUM; VinId++) {
                if (0U == DSP_GetU16Bit(pResource->VinBit, VinId, 1U)) {
                    continue;
                }

                HL_GetVinInfo(HL_MTX_OPT_ALL, VinId, &VinInfo);
                if (((UINT16)VinInfo.PostCfgType & AMBA_DSP_VIN_CONFIG_POST) > 0U) {
                    DSP_SetU16Bit(&VinPostCfgBitMask, VinId);
                }
            }
            /* Handle VinPostCfg first, Wait until VinRegCfged */
            Rval = HL_VinPostCfgHandler(VinPostCfgBitMask);
            DSP_FillErrline(Rval, &ErrLine, __LINE__);
            if (Rval == DSP_ERR_NONE) {
                HL_MainInitOnLvBootPostprocVprocInfoSet(&VinInfo, &VprocInfo, pResource);
            }
        }
    } else {
        /* DO NOTHING */
    }

    if (Rval != DSP_ERR_NONE) {
        AmbaLL_LogUInt5("dsp_set_debug_thread Error[0x%x][%u]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

static inline UINT32 HL_MainInitChk(const AMBA_DSP_SYS_CONFIG_s *pDspSysConfig)
{
    UINT32 Rval = DSP_ERR_NONE, ErrLine = 0U;
    const Dsp_DumpApi_t* DumpApi = AmbaDSP_GetDumpApiFunc();
    const Dsp_CheckApi_t* CheckApi = AmbaDSP_GetCheckApiFunc();
    const Dsp_TimingApi_t* TimingApi = AmbaDSP_GetTimingApiFunc();
#ifdef SUPPORT_RESC_CHECK
    const Dsp_ResourceApi_t* ResourceApi = AmbaDSP_GetResourceApiFunc();
#endif

    /* DSP CtxInit */
    Rval = HL_CtxInit();
    DSP_FillErrline(Rval, &ErrLine, __LINE__);

    if (Rval == DSP_ERR_NONE) {
        Rval = HL_EventInit();
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
    }

    /* Timing sanity check */
    if ((Rval == DSP_ERR_NONE) &&
        (TimingApi != NULL)) {
        Rval = TimingApi->pMainInit(pDspSysConfig);
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
    }
    /* Input sanity check */
    if ((Rval == DSP_ERR_NONE) &&
        (DumpApi != NULL)) {
        DumpApi->pMainInit(pDspSysConfig);
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
    }
    if ((Rval == DSP_ERR_NONE) &&
        (CheckApi != NULL)) {
        Rval = CheckApi->pMainInit(pDspSysConfig);
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
    }
#ifdef SUPPORT_RESC_CHECK
    /* HW/SW resources check */
    if ((Rval == DSP_ERR_NONE) &&
        (ResourceApi != NULL)) {
        Rval = ResourceApi->pMainInit(pDspSysConfig);
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
    }
#endif
    /* Logic sanity check */

    if (Rval != DSP_ERR_NONE) {
        AmbaLL_LogUInt5("HL_MainInitChk Error[0x%x][%u]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

/**
* DSP component initial function
* @param [in]  pDspSysConfig initial configuration
* @return ErrorCode
*/
UINT32 dsp_main_init(const AMBA_DSP_SYS_CONFIG_s *pDspSysConfig)
{
    UINT32 Rval = DSP_ERR_NONE, ErrLine = 0U;
    static UINT8 FirstMainInit = 1U;

    Rval = HL_MainInitChk(pDspSysConfig);
    DSP_FillErrline(Rval, &ErrLine, __LINE__);

    /* Body */
    if (Rval == OK) {
        CTX_RESOURCE_INFO_s *pResource = HL_CtxResInfoPtr;

        HL_GetResourceLock(&pResource);
        Rval = dsp_osal_memcpy(&pResource->DspSysCfg, pDspSysConfig, sizeof(AMBA_DSP_SYS_CONFIG_s));
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
        HL_GetResourceUnLock();

        if ((pDspSysConfig->SysState == AMBA_DSP_SYS_STATE_PLAYBACK) && (FirstMainInit == 1U)) {
            FirstMainInit = 0U;
            AmbaLL_LogUInt5("MainInit set resource for Playback", 0U, 0U, 0U, 0U, 0U);

            HL_GetResourceLock(&pResource);
            HL_SetUCodeVerInfo(&pResource->DspVer);
            HL_GetResourceUnLock();
        } else {
            if ((pDspSysConfig->SysState == AMBA_DSP_SYS_STATE_LIVEVIEW) ||
                (pDspSysConfig->SysState == AMBA_DSP_SYS_STATE_SENSORLESS)) {
                UINT8 WriteMode = AMBA_DSP_CMD_DEFAULT_WRITE;
                UINT8 ProfState = DSP_GetProfState();
                UINT8 DspVinState = DSP_GetVinState(0U/*VinId*/);
                UINT8 ModeSwitch = 0U;
                UINT8 IsPartialEnable = 0U;

                Rval = HL_LiveviewCmdPrepare(WriteMode, IsPartialEnable);
                DSP_FillErrline(Rval, &ErrLine, __LINE__);
                HL_SetRescState(HL_RESC_SETTLED);

                AmbaLL_LogUInt5("[SSP] WrMode %u, Prof %u, VinState %u, ModeSwitch %u OpStatus %u",
                    WriteMode, ProfState, DspVinState, ModeSwitch, pResource->DspOpStatus);
            }

            if (Rval == DSP_ERR_NONE) {
                ULONG ChipInfoAddr = 0U;

                HL_GetResourceLock(&pResource);
                pResource->VpMsgCtrlBufAddr = pDspSysConfig->VpMsgCtrlBufAddr;
                HL_GetResourceUnLock();
                Rval = DSP_Bootup(pDspSysConfig, &ChipInfoAddr, pResource->MonitorMask);
                DSP_FillErrline(Rval, &ErrLine, __LINE__);

                HL_GetResourceLock(&pResource);
                pResource->ChipInfoAddr = ChipInfoAddr;
                HL_GetResourceUnLock();
            }

            if (Rval == DSP_ERR_NONE) {
                if (pDspSysConfig->SysState == AMBA_DSP_SYS_STATE_LIVEVIEW) {
                    Rval = HL_MainInitOnLvBootPostproc(pResource);
                    DSP_FillErrline(Rval, &ErrLine, __LINE__);
                } else if (pDspSysConfig->SysState == AMBA_DSP_SYS_STATE_PLAYBACK) {
                    Rval = DSP_WaitProfState(DSP_PROF_STATUS_PLAYBACK, MODE_SWITCH_TIMEOUT);
                    DSP_FillErrline(Rval, &ErrLine, __LINE__);
                } else if (pDspSysConfig->SysState == AMBA_DSP_SYS_STATE_SENSORLESS) {
                    Rval = DSP_WaitProfState(DSP_PROF_STATUS_CAMERA, MODE_SWITCH_TIMEOUT);
                    DSP_FillErrline(Rval, &ErrLine, __LINE__);
                } else {
                    Rval = DSP_ERR_0001;
                    DSP_FillErrline(Rval, &ErrLine, __LINE__);
                }
                if (Rval == DSP_ERR_NONE) {
                    HL_GetResourceLock(&pResource);
                    pResource->DspSysState = pDspSysConfig->SysState;
                    pResource->DspOpStatus = DSP_OP_STATUS_ACTIVE;
                    HL_GetResourceUnLock();
                } else {
                    AmbaLL_LogUInt5("DSP boot to %u failed 0x%x~", pDspSysConfig->SysState, Rval, 0U, 0U, 0U);
                }

                /* TBD, open API for user to query this calc result */
                HL_GetResourcePtr(&pResource);
                dsp_osal_printU5("[SSP]CalcDspWork %d", pResource->ProfWorkSize, 0U, 0U, 0U, 0U);
            } else {
                AmbaLL_LogUInt5("DSP boot to %u failed 0x%x", pDspSysConfig->SysState, Rval, 0U, 0U, 0U);
            }
            HL_GetResourceLock(&pResource);
            HL_SetUCodeVerInfo(&pResource->DspVer);
            HL_GetResourceUnLock();
        }
        /* Check DSP assertion */
        LL_CheckDspAssert();
    }

    if (Rval != DSP_ERR_NONE) {
        AmbaLL_LogUInt5("dsp_main_init Error[0x%x][%u]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

static inline UINT32 HL_MainGetDefaultSysCfgChk(const AMBA_DSP_SYS_CONFIG_s *pDspSysConfig)
{
    UINT32 Rval = DSP_ERR_NONE, ErrLine = 0U;
    const Dsp_DumpApi_t* DumpApi = AmbaDSP_GetDumpApiFunc();
    const Dsp_CheckApi_t* CheckApi = AmbaDSP_GetCheckApiFunc();

    /* DSP CtxInit */
    Rval = HL_CtxInit();
    DSP_FillErrline(Rval, &ErrLine, __LINE__);

    /* Timing sanity check */
    /* Input sanity check */
    if ((Rval == DSP_ERR_NONE) &&
        (DumpApi != NULL)) {
        DumpApi->pMainGetDefaultSysCfg(pDspSysConfig);
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
    }
    if ((Rval == DSP_ERR_NONE) &&
        (CheckApi != NULL)) {
        Rval = CheckApi->pMainGetDefaultSysCfg(pDspSysConfig);
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
    }
    /* Logic sanity check */

    if (Rval != DSP_ERR_NONE) {
        AmbaLL_LogUInt5("HL_MainGetDefaultSysCfgChk Error[0x%x][%u]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

/**
* Get function for recommend initial configuration
* @param [out] pDspSysConfig initial configuration
* @return ErrorCode
*/
UINT32 dsp_main_get_defsyscfg(AMBA_DSP_SYS_CONFIG_s *pDspSysConfig)
{
    UINT32 Rval = DSP_ERR_NONE, ErrLine = 0U;

    Rval = HL_MainGetDefaultSysCfgChk(pDspSysConfig);
    DSP_FillErrline(Rval, &ErrLine, __LINE__);

    /* Body */
    if (Rval == DSP_ERR_NONE) {
        Rval =dsp_osal_memset(pDspSysConfig, 0x0, sizeof(AMBA_DSP_SYS_CONFIG_s));
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
        pDspSysConfig->SysState = AMBA_DSP_SYS_STATE_LIVEVIEW;
    }

    if (Rval != DSP_ERR_NONE) {
        AmbaLL_LogUInt5("dsp_main_get_defsyscfg Error[0x%x][%u]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

static UINT32 HL_MainSuspendChk(void)
{
    UINT32 Rval = DSP_ERR_NONE, ErrLine = 0U;

    /* DSP CtxInit */
    Rval = HL_CtxInit();
    DSP_FillErrline(Rval, &ErrLine, __LINE__);

    /* Timing sanity check */
    /* Input sanity check */
    /* Logic sanity check */

    if (Rval != DSP_ERR_NONE) {
        AmbaLL_LogUInt5("HL_MainSuspendChk Error[0x%x][%u]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

/**
* Suspend DSP component
* @return ErrorCode
*/
UINT32 dsp_main_suspend(void)
{
    UINT32 Rval = DSP_ERR_NONE, ErrLine = 0U;

    Rval = HL_MainSuspendChk();
    DSP_FillErrline(Rval, &ErrLine, __LINE__);

    /* Body */
    if (Rval == DSP_ERR_NONE) {
        CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;

        //suspend operation
//FIXME, complete it

        HL_GetResourceLock(&Resource);
        Resource->DspOpStatus = DSP_OP_STATUS_SUSPEND;
        HL_GetResourceUnLock();
    }

    if (Rval != DSP_ERR_NONE) {
        AmbaLL_LogUInt5("dsp_main_suspend Error[0x%x][%u]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

static UINT32 HL_MainResumeChk(void)
{
    UINT32 Rval = DSP_ERR_NONE, ErrLine = 0U;

    /* DSP CtxInit */
    Rval = HL_CtxInit();
    DSP_FillErrline(Rval, &ErrLine, __LINE__);

    /* Timing sanity check */
    /* Input sanity check */
    /* Logic sanity check */
    if (Rval != DSP_ERR_NONE) {
        AmbaLL_LogUInt5("HL_MainResumeChk Error[0x%x][%u]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

/**
* Resume DSP component
* @return ErrorCode
*/
UINT32 dsp_main_resume(void)
{
    UINT32 Rval = DSP_ERR_NONE, ErrLine = 0U;

    Rval = HL_MainResumeChk();
    DSP_FillErrline(Rval, &ErrLine, __LINE__);

    /* Body */
    if (Rval == DSP_ERR_NONE) {
        CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;

        //suspend operation
//FIXME, complete it

        HL_GetResourceLock(&Resource);
        Resource->DspOpStatus = DSP_OP_STATUS_ACTIVE;
        HL_GetResourceUnLock();
    }

    if (Rval != DSP_ERR_NONE) {
        AmbaLL_LogUInt5("dsp_main_resume Error[0x%x][%u]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

static inline UINT32 HL_MainSetWorkAreaChk(ULONG WorkAreaAddr, UINT32 WorkSize)
{
    UINT32 Rval = DSP_ERR_NONE, ErrLine = 0U;
    const Dsp_DumpApi_t* DumpApi = AmbaDSP_GetDumpApiFunc();
    const Dsp_CheckApi_t* CheckApi = AmbaDSP_GetCheckApiFunc();
    const Dsp_TimingApi_t* TimingApi = AmbaDSP_GetTimingApiFunc();

    /* DSP CtxInit */
    Rval = HL_CtxInit();
    DSP_FillErrline(Rval, &ErrLine, __LINE__);

    /* Timing sanity check */
    if ((Rval == OK) &&
        (TimingApi != NULL)) {
        Rval = TimingApi->pMainSetWorkArea(WorkAreaAddr, WorkSize);
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
    }
    /* Input sanity check */
    if ((Rval == OK) &&
        (DumpApi != NULL)) {
        DumpApi->pMainSetWorkArea(WorkAreaAddr, WorkSize);
    }
    if ((Rval == OK) &&
        (CheckApi != NULL)) {
        Rval = CheckApi->pMainSetWorkArea(WorkAreaAddr, WorkSize);
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
    }
    /* Logic sanity check */

    if (Rval != DSP_ERR_NONE) {
        AmbaLL_LogUInt5("HL_MainSetWorkAreaChk Error[0x%x][%u]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

/**
* Set DSP component working buffer
* @param [in]  WorkAreaAddr working buffer address
* @param [in]  WorkSize working buffer size
* @return ErrorCode
*/
UINT32 dsp_main_set_work_area(ULONG WorkAreaAddr, UINT32 WorkSize)
{
    UINT32 Rval = DSP_ERR_NONE, ErrLine = 0U;
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;

    Rval = HL_MainSetWorkAreaChk(WorkAreaAddr, WorkSize);
    DSP_FillErrline(Rval, &ErrLine, __LINE__);

    /* Body */
    if (Rval == DSP_ERR_NONE) {
        /* Set dsp working area into dsp init data,
         * Take effect on dsp boot or next mode switch */
        HL_GetResourceLock(&Resource);
        Resource->DspSysCfg.WorkAreaAddr = WorkAreaAddr;
        Resource->DspSysCfg.WorkAreaSize = WorkSize;
        HL_GetResourceUnLock();
        Rval = DSP_UpdateWorkBuffer(WorkAreaAddr, WorkSize);
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
    }

    if (Rval != DSP_ERR_NONE) {
        AmbaLL_LogUInt5("dsp_main_set_work_area Error[0x%x][%u]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

UINT32 dsp_main_set_protect_area(UINT32 Type, ULONG AreaAddr, UINT32 Size, UINT32 IsCached)
{
    UINT32 Rval = DSP_ERR_NONE;
(void)Type;
(void)AreaAddr;
(void)Size;
(void)IsCached;
    return Rval;
}

static inline UINT32 HL_MainGetDspVerInfoChk(const AMBA_DSP_VERSION_INFO_s *Info)
{
    UINT32 Rval = DSP_ERR_NONE, ErrLine = 0U;
    const Dsp_DumpApi_t* DumpApi = AmbaDSP_GetDumpApiFunc();
    const Dsp_CheckApi_t* CheckApi = AmbaDSP_GetCheckApiFunc();

    /* DSP CtxInit */
    Rval = HL_CtxInit();
    DSP_FillErrline(Rval, &ErrLine, __LINE__);

    /* Timing sanity check */
    /* Input sanity check */
    if ((Rval == DSP_ERR_NONE) &&
        (DumpApi != NULL)) {
        DumpApi->pMainGetDspVerInfo(Info);
    }

    if ((Rval == DSP_ERR_NONE) &&
        (CheckApi != NULL)) {
        Rval = CheckApi->pMainGetDspVerInfo(Info);
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
    }

    /* Logic sanity check */

    if (Rval != DSP_ERR_NONE) {
        AmbaLL_LogUInt5("HL_MainGetDspVerInfoChk Error[0x%x][%u]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

/**
* Get DSP component revision information
* @param [out] Info revision information
* @return ErrorCode
*/
UINT32 dsp_main_get_dsp_ver_info(AMBA_DSP_VERSION_INFO_s *Info)
{
    UINT32 Rval = DSP_ERR_NONE, ErrLine = 0U;

    Rval = HL_MainGetDspVerInfoChk(Info);
    DSP_FillErrline(Rval, &ErrLine, __LINE__);

    /* Body */
    if (Rval == DSP_ERR_NONE) {
        CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;

        HL_GetResourcePtr(&Resource);
        Rval = dsp_osal_memcpy(Info, &Resource->DspVer, sizeof(AMBA_DSP_VERSION_INFO_s));
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
    }

    if (Rval != DSP_ERR_NONE) {
        AmbaLL_LogUInt5("dsp_main_get_dsp_ver_info Error[0x%x][%u]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

static inline UINT32 HL_MainMsgParseEntryChk(UINT32 EntryArg)
{
    UINT32 Rval = DSP_ERR_NONE, ErrLine = 0U;
//    const Dsp_DumpApi_t* DumpApi = AmbaDSP_GetDumpApiFunc();
    const Dsp_CheckApi_t* CheckApi = AmbaDSP_GetCheckApiFunc();
    const Dsp_TimingApi_t* TimingApi = AmbaDSP_GetTimingApiFunc();

    /* DSP CtxInit */
    Rval = HL_CtxInit();
    DSP_FillErrline(Rval, &ErrLine, __LINE__);

    /* Timing sanity check */
    if ((Rval == DSP_ERR_NONE) &&
        (TimingApi != NULL)) {
        Rval = TimingApi->pMainMsgParseEntry(EntryArg);
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
    }
    /* Input sanity check */
//    if (DumpApi != NULL) {
//        DumpApi->pMainMsgParseEntry(EntryArg);
//    }
    if ((Rval == DSP_ERR_NONE) &&
        (CheckApi != NULL)) {
        Rval = CheckApi->pMainMsgParseEntry(EntryArg);
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
    }
    /* Logic sanity check */

    if (Rval != DSP_ERR_NONE) {
        AmbaLL_LogUInt5("HL_MainMsgParseEntryChk Error[0x%x][%u]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

/**
* DSP message parser entry function
* @param [in]  EntryArg entry argument
* @return ErrorCode
*/
UINT32 dsp_main_msg_parse_entry(UINT32 EntryArg)
{
    UINT32 Rval = DSP_ERR_NONE, ErrLine = 0U;

    Rval = HL_MainMsgParseEntryChk(EntryArg);

    /* Body */
    if (Rval == DSP_ERR_NONE) {
        Rval = LL_MsgParseEntry(EntryArg);
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
    }

    /* DSP assertion check */
    LL_CheckDspAssert();

    if (Rval != DSP_ERR_NONE) {
        AmbaLL_LogUInt5("dsp_main_msg_parse_entry Error[0x%x][%u]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

static inline UINT32 HL_MainWaitVinInterruptChk(UINT32 Flag, const UINT32 *ActualFlag, UINT32 TimeOut)
{
    UINT32 Rval = DSP_ERR_NONE, ErrLine = 0U;
//    const Dsp_DumpApi_t* DumpApi = AmbaDSP_GetDumpApiFunc();
    const Dsp_CheckApi_t* CheckApi = AmbaDSP_GetCheckApiFunc();
    const Dsp_TimingApi_t* TimingApi = AmbaDSP_GetTimingApiFunc();

    /* DSP CtxInit */
    Rval = HL_CtxInit();
    DSP_FillErrline(Rval, &ErrLine, __LINE__);

    /* Timing sanity check */
    if ((Rval == DSP_ERR_NONE) &&
        (TimingApi != NULL)) {
        Rval = TimingApi->pMainWaitVinInterrupt(Flag, ActualFlag, TimeOut);
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
    }
    /* Input sanity check */
//    if (DumpApi != NULL) {
//        DumpApi->pMainWaitVinInterrupt(Flag, ActualFlag, TimeOut);
//    }
    if ((Rval == DSP_ERR_NONE) &&
        (CheckApi != NULL)) {
        Rval = CheckApi->pMainWaitVinInterrupt(Flag, ActualFlag, TimeOut);
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
    }
    /* Logic sanity check */

    if (Rval != DSP_ERR_NONE) {
        AmbaLL_LogUInt5("HL_MainWaitVinInterruptChk Error[0x%x][%u]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

/**
* DSP Vin interrupt waiting service function
* @param [in]  Flag waiting flag
* @param [out] ActualFlag waited flag
* @param [in]  TimeOut time out setting
* @return ErrorCode
*/
UINT32 dsp_main_wait_vin_interrupt(UINT32 Flag, UINT32 *ActualFlag, UINT32 TimeOut)
{
    UINT32 Rval = DSP_ERR_NONE, ErrLine = 0U;

    Rval = HL_MainWaitVinInterruptChk(Flag, ActualFlag, TimeOut);
    DSP_FillErrline(Rval, &ErrLine, __LINE__);

    /* Body */
    if (Rval == DSP_ERR_NONE) {
        Rval = LL_WaitVinInterrupt(Flag, ActualFlag, TimeOut);
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
    }

    if (Rval != DSP_ERR_NONE) {
//        AmbaLL_LogUInt5("dsp_main_wait_vin_interrupt Error[0x%x][%u]", Rval, ErrLine, 0U, 0U, 0U);
    }


    return Rval;

}

static inline UINT32 HL_MainWaitVoutInterruptChk(UINT32 Flag, const UINT32 *ActualFlag, UINT32 TimeOut)
{
    UINT32 Rval = DSP_ERR_NONE, ErrLine = 0U;
//    const Dsp_DumpApi_t* DumpApi = AmbaDSP_GetDumpApiFunc();
    const Dsp_CheckApi_t* CheckApi = AmbaDSP_GetCheckApiFunc();
    const Dsp_TimingApi_t* TimingApi = AmbaDSP_GetTimingApiFunc();

    /* DSP CtxInit */
    Rval = HL_CtxInit();
    DSP_FillErrline(Rval, &ErrLine, __LINE__);

    /* Timing sanity check */
    if ((Rval == DSP_ERR_NONE) &&
        (TimingApi != NULL)) {
        Rval = TimingApi->pMainWaitVoutInterrupt(Flag, ActualFlag, TimeOut);
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
    }
    /* Input sanity check */
//    if (DumpApi != NULL) {
//        DumpApi->pMainWaitVoutInterrupt(Flag, ActualFlag, TimeOut);
//    }
    if ((Rval == DSP_ERR_NONE) &&
        (CheckApi != NULL)) {
        Rval = CheckApi->pMainWaitVoutInterrupt(Flag, ActualFlag, TimeOut);
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
    }
    /* Logic sanity check */

    if (Rval != DSP_ERR_NONE) {
        AmbaLL_LogUInt5("HL_MainWaitVoutInterruptChk Error[0x%x][%u]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

/**
* DSP Vout interrupt waiting service function
* @param [in]  Flag waiting flag
* @param [out] ActualFlag waited flag
* @param [in]  TimeOut time out setting
* @return ErrorCode
*/
UINT32 dsp_main_wait_vout_interrupt(UINT32 Flag, UINT32 *ActualFlag, UINT32 TimeOut)
{
    UINT32 Rval = DSP_ERR_NONE, ErrLine = 0U;

    Rval = HL_MainWaitVoutInterruptChk(Flag, ActualFlag, TimeOut);
    DSP_FillErrline(Rval, &ErrLine, __LINE__);

    /* Body */
    if (Rval == DSP_ERR_NONE) {
        Rval = LL_WaitVoutInterrupt(Flag, ActualFlag, TimeOut);
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
    }

    if (Rval != DSP_ERR_NONE) {
//        AmbaLL_LogUInt5("dsp_main_wait_vout_interrupt Error[0x%x][%u]", Rval, ErrLine, 0U, 0U, 0U);
    }


    return Rval;
}

static inline UINT32 HL_MainWaitFlagChk(UINT32 Flag, const UINT32 *ActualFlag, UINT32 TimeOut)
{
    UINT32 Rval = DSP_ERR_NONE, ErrLine = 0U;
//    const Dsp_DumpApi_t* DumpApi = AmbaDSP_GetDumpApiFunc();
    const Dsp_CheckApi_t* CheckApi = AmbaDSP_GetCheckApiFunc();
    const Dsp_TimingApi_t* TimingApi = AmbaDSP_GetTimingApiFunc();

    /* DSP CtxInit */
    Rval = HL_CtxInit();
    DSP_FillErrline(Rval, &ErrLine, __LINE__);

    /* Timing sanity check */
    if ((Rval == DSP_ERR_NONE) &&
        (TimingApi != NULL)) {
        Rval = TimingApi->pMainWaitFlag(Flag, ActualFlag, TimeOut);
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
    }
    /* Input sanity check */
//    if (DumpApi != NULL) {
//        DumpApi->pMainWaitFlag(Flag, ActualFlag, TimeOut);
//    }
    if ((Rval == DSP_ERR_NONE) &&
        (CheckApi != NULL)) {
        Rval = CheckApi->pMainWaitFlag(Flag, ActualFlag, TimeOut);
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
    }
    /* Logic sanity check */

    if (Rval != DSP_ERR_NONE) {
        AmbaLL_LogUInt5("HL_MainWaitFlagChk Error[0x%x][%u]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

/**
* DSP system message waiting service function
* @param [in]  Flag waiting flag
* @param [out] ActualFlag waited flag
* @param [in]  TimeOut time out setting
* @return ErrorCode
*/
UINT32 dsp_main_wait_flag(UINT32 Flag, UINT32 *ActualFlag, UINT32 TimeOut)
{
    UINT32 Rval = DSP_ERR_NONE, ErrLine = 0U;

    Rval = HL_MainWaitFlagChk(Flag, ActualFlag, TimeOut);
    DSP_FillErrline(Rval, &ErrLine, __LINE__);

    /* Body */
    if (Rval == DSP_ERR_NONE) {
        Rval = LL_WaitDspMsgFlag(Flag, ActualFlag, TimeOut);
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
    }

    if (Rval != DSP_ERR_NONE) {
        AmbaLL_LogUInt5("dsp_main_wait_flag Error[0x%x][%u]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

static inline void HL_ResourceLimitConfigTblInit(const UINT32 i,
                                                 const AMBA_DSP_RESOURCE_s *pResource,
                                                 CTX_RESOURCE_INFO_s *pCtxResource)
{
    UINT16 j;

    pCtxResource->MaxVinVirtChanBit[i] = pResource->LiveviewResource.MaxVinVirtChanBit[i];
    for (j = 0U; j < AMBA_DSP_MAX_VIRT_CHAN_NUM; j++) {
        pCtxResource->MaxVinVirtChanOutputNum[i][j] = pResource->LiveviewResource.MaxVinVirtChanOutputNum[i][j];
        pCtxResource->MaxVinVirtChanWidth[i][j] = pResource->LiveviewResource.MaxVinVirtChanWidth[i][j];
        pCtxResource->MaxVinVirtChanHeight[i][j] = pResource->LiveviewResource.MaxVinVirtChanHeight[i][j];
    }

    pCtxResource->MaxVinBootTimeout[i] = pResource->LiveviewResource.MaxVinBootTimeout[i];
    pCtxResource->MaxVinTimeout[i] = pResource->LiveviewResource.MaxVinTimeout[i];
    pCtxResource->MaxRaw2YuvDelay[i] = pResource->LiveviewResource.MaxRaw2YuvDelay[i];

    pCtxResource->RawAllocType[i] = pResource->LiveviewResource.RawBuf[i][0U].AllocType;
    pCtxResource->RawBufNum[i] = pResource->LiveviewResource.RawBuf[i][0U].BufNum;
    (void)dsp_osal_memcpy(&pCtxResource->RawBuf[i], &pResource->LiveviewResource.RawBuf[i][0U].Buf, sizeof(AMBA_DSP_RAW_BUF_s));
    if (pResource->LiveviewResource.RawBuf[i][0U].pRawBufTbl != NULL) {
        (void)dsp_osal_memcpy(&pCtxResource->RawBufTbl[i][0U], &pResource->LiveviewResource.RawBuf[i][0U].pRawBufTbl[0U], sizeof(ULONG)*pCtxResource->RawBufNum[i]);
    } else {
        (void)dsp_osal_memset(&pCtxResource->RawBufTbl[i][0U], 0, sizeof(ULONG)*MAX_EXT_DISTINCT_DEPTH);
    }

    (void)dsp_osal_memcpy(&pCtxResource->CeBuf[i], &pResource->LiveviewResource.RawBuf[i][0U].AuxBuf, sizeof(AMBA_DSP_RAW_BUF_s));
    if (pResource->LiveviewResource.RawBuf[i][0U].pAuxBufTbl != NULL) {
        (void)dsp_osal_memcpy(&pCtxResource->CeBufTbl[i][0U], &pResource->LiveviewResource.RawBuf[i][0U].pAuxBufTbl[0U], sizeof(ULONG)*pCtxResource->RawBufNum[i]);
    } else {
        (void)dsp_osal_memset(&pCtxResource->CeBufTbl[i][0U], 0, sizeof(ULONG)*MAX_EXT_DISTINCT_DEPTH);
    }
}

static inline UINT32 HL_ResourceLimitConfigChk(const AMBA_DSP_RESOURCE_s *pResource)
{
    UINT32 Rval = DSP_ERR_NONE, ErrLine = 0U;
    const Dsp_DumpApi_t* DumpApi = AmbaDSP_GetDumpApiFunc();
    const Dsp_CheckApi_t* CheckApi = AmbaDSP_GetCheckApiFunc();
    const Dsp_TimingApi_t* TimingApi = AmbaDSP_GetTimingApiFunc();

    /* DSP CtxInit */
    Rval = HL_CtxInit();
    DSP_FillErrline(Rval, &ErrLine, __LINE__);

    /* Timing sanity check */
    if ((Rval == DSP_ERR_NONE) &&
        (TimingApi != NULL)) {
        Rval = TimingApi->pMainResourceLimit(pResource);
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
    }
    /* Input sanity check */
    if ((Rval == DSP_ERR_NONE) &&
        (DumpApi != NULL)) {
        DumpApi->pMainResourceLimit(pResource);
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
    }
    if ((Rval == DSP_ERR_NONE) &&
        (CheckApi != NULL)) {
        Rval = CheckApi->pMainResourceLimit(pResource);
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
    }
    /* Logic sanity check */

    if (Rval != DSP_ERR_NONE) {
        AmbaLL_LogUInt5("dsp_main_wait_flag Error[0x%x][%u]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

/*
 *  Vin inlcudes VirtualVin
 *  Target2PlayBack :
 *       VinNum == 0 && ViewZone == 0 && EncNum == 0 && DecNum > 0
 *  Target2Camera :
 *       VinNum | ViewZone | DecNum | EncNum
 *          - VinNum > 0 && ViewZone > 0 : Liveview
 *          - VinNum > 0 && ViewZone > 0 && EncNum > 0 : Camera
 *          - VinNum > 0 && ViewZone > 0 && DecNum > 0 : Duplex
 *          - ViewZone > 0 && DecNum > 0 && EncNum > 0 : XCode
 */
/**
* DSP component resource limitation configuration
* @param [in]  pResource resource setting
* @return ErrorCode
*/
UINT32 dsp_resource_limit_config(const AMBA_DSP_RESOURCE_s *pResource)
{
    UINT32 Rval = DSP_ERR_NONE, ErrLine = 0U, i;
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;
    const AMBA_DSP_STILL_RESOURCE_s *pStlRes;

    Rval = HL_ResourceLimitConfigChk(pResource);
    DSP_FillErrline(Rval, &ErrLine, __LINE__);
#if 0
    /* check at dsp_cfg side */
    if (Rval == OK) {
        if (DSP_PROF_STATUS_INVALID != DSP_GetProfState()) {
            UINT8 VoutIdx;
            HL_GetResourcePtr(&Resource);
            for (VoutIdx=0; VoutIdx<AMBA_DSP_MAX_VOUT_NUM; VoutIdx++) {
                if ((Resource->MaxVoutWidth[VoutIdx] == 0U) &&
                    (pResource->DisplayResource->MaxVoutWidth[VoutIdx] != 0U)) {
                    AmbaLL_LogUInt5(" ResDisp Vout[%u] turned on", VoutIdx, 0U, 0U, 0U, 0U);
                    Rval = DSP_ERR_0001;
                } else if ((Resource->MaxVoutWidth[VoutIdx] != 0U) &&
                           (pResource->DisplayResource->MaxVoutWidth[VoutIdx] == 0U)) {
                    AmbaLL_LogUInt5(" ResDisp Vout[%u] turned off", VoutIdx, 0U, 0U, 0U, 0U);
                    Rval = DSP_ERR_0001;
                } else if ((Resource->MaxOsdBufWidth[VoutIdx] == 0U) &&
                        (pResource->DisplayResource->MaxOsdBufWidth[VoutIdx] != 0U)) {
                    AmbaLL_LogUInt5(" ResDisp Osd[%u] turned on", VoutIdx, 0U, 0U, 0U, 0U);
                    Rval = DSP_ERR_0001;
                } else if ((Resource->MaxOsdBufWidth[VoutIdx] != 0U) &&
                           (pResource->DisplayResource->MaxOsdBufWidth[VoutIdx] == 0U)) {
                    AmbaLL_LogUInt5(" ResDisp Osd[%u] turned off", VoutIdx, 0U, 0U, 0U, 0U);
                    Rval = DSP_ERR_0001;
                } else {
                    //
                }
            }
        }
    }
#endif

    /* Body */
    if (Rval == DSP_ERR_NONE) {
        /* Check Resource changed or not */
        UINT8 RescChg = 0U;
        UINT32 ChgLine = 0U;

        Rval = HL_CheckRescChange(pResource, &RescChg, &ChgLine);
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
        if (Rval == DSP_ERR_NONE) {
            if (RescChg == 1U) {
                HL_GetResourceLock(&Resource);
                pStlRes = &pResource->StillResource;

                /* Liveview */
                Resource->VideoPipe[0] = pResource->LiveviewResource.VideoPipe;
                Resource->LowDelayMode = pResource->LiveviewResource.LowDelayMode;
                Resource->MaxVinBit = pResource->LiveviewResource.MaxVinBit;
                Resource->MaxViewZoneNum = pResource->LiveviewResource.MaxViewZoneNum;
                for (i=0; i < AMBA_DSP_MAX_VIN_NUM; i++) {

                    HL_ResourceLimitConfigTblInit(i, pResource, Resource);

                    Rval =  dsp_osal_memcpy(&Resource->DefaultRawBuf[i], &pResource->LiveviewResource.DefaultRawBuf[i][0U], sizeof(AMBA_DSP_RAW_BUF_s));
                    DSP_FillErrline(Rval, &ErrLine, __LINE__);
                    Rval = dsp_osal_memcpy(&Resource->DefaultAuxRawBuf[i], &pResource->LiveviewResource.DefaultAuxRawBuf[i][0U], sizeof(AMBA_DSP_RAW_BUF_s));
                    DSP_FillErrline(Rval, &ErrLine, __LINE__);

                    Resource->MaxStlVinWidth[i][0U] = pStlRes->MaxVinWidth[i];
                    Resource->MaxStlVinHeight[i][0U] = pStlRes->MaxVinHeight[i];

                    /* Embedded Data */
                    Resource->MaxVinEmbdDataWidth[i] = pResource->LiveviewResourceEx.MaxVinEmbdDataWidth[i];
                    Resource->MaxVinEmbdDataHeight[i] = pResource->LiveviewResourceEx.MaxVinEmbdDataHeight[i];
                }

                Resource->MaxVirtVinBit = pResource->LiveviewResource.MaxVirtVinBit;
                for (i=0; i < AMBA_DSP_MAX_VIRT_VIN_NUM; i++) {
                    Resource->MaxVirtVinOutputNum[i] = pResource->LiveviewResource.MaxVirtVinOutputNum[i];
                    Resource->MaxVirtVinWidth[i] = pResource->LiveviewResource.MaxVirtVinWidth[i];
                    Resource->MaxVirtVinHeight[i] = pResource->LiveviewResource.MaxVirtVinHeight[i];
                }

                for (i=0; i < AMBA_DSP_MAX_VIEWZONE_NUM; i++) {
                    Resource->MaxWarpDma[i] = pResource->LiveviewResource.MaxWarpDma[i];
                    Resource->MaxWarpWaitLineLuma[i] = pResource->LiveviewResource.MaxWarpWaitLineLuma[i];
                    Resource->MaxWarpWaitLineChroma[i] = pResource->LiveviewResource.MaxWarpWaitLineChroma[i];
                    Resource->MaxHierWidth[i] = pResource->LiveviewResource.MaxHierWidth[i];
                    Resource->MaxHierHeight[i] = pResource->LiveviewResource.MaxHierHeight[i];
                    Resource->MaxLndtWidth[i] = pResource->LiveviewResource.MaxLndtWidth[i];
                    Resource->MaxLndtHeight[i] = pResource->LiveviewResource.MaxLndtHeight[i];
                    Resource->MaxHorWarpComp[i] = pResource->LiveviewResourceEx.MaxHorWarpCompensation[i];
                }

                /* Encode/Recon */
                Resource->MaxEncodeStream = pResource->EncodeResource.MaxEncodeStream;
                for (i=0; i < AMBA_DSP_MAX_STREAM_NUM; i++) {
                    Resource->MaxGopM[i] = pResource->EncodeResource.MaxGopM[i];
                    Resource->MaxExtraReconNum[i] = pResource->EncodeResource.MaxExtraReconNum[i];
                    Resource->MaxSearchRange[i] = pResource->EncodeResource.MaxSearchRange[i];
                    Resource->MaxStrmFmt[i] = pResource->EncodeResource.MaxStrmFmt[i];
                }

                /* Still */
                Resource->MaxProcessFormat = pStlRes->MaxProcessFormat;
                if (pStlRes->MaxProcessFormat > 0U) {
                    Resource->MaxStlMainWidth = pStlRes->MaxMainWidth;
                    Resource->MaxStlMainHeight = pStlRes->MaxMainHeight;

                    Resource->MaxStlRawInputWidth = pStlRes->MaxRawInputWidth;
                    Resource->MaxStlRawInputHeight = pStlRes->MaxRawInputHeight;
                    Resource->MaxStlYuvInputWidth = pStlRes->MaxYuvInputWidth;
                    Resource->MaxStlYuvInputHeight = pStlRes->MaxYuvInputHeight;
                }
                Resource->MaxStlYuvEncWidth = pStlRes->MaxYuvEncWidth;
                Resource->MaxStlYuvEncHeight = pStlRes->MaxYuvEncHeight;

                /* Decode */
                Resource->DecMaxStreamNum          = pResource->DecodeResource.MaxDecodeStreamNum;
                for (i=0; i < AMBA_DSP_MAX_DEC_STREAM_NUM; i++) {
                    Resource->DecMaxStrmFmt[i] = pResource->DecodeResource.MaxStrmFmt[i];
                }
                Resource->DecMaxFrameWidth         = pResource->DecodeResource.MaxFrameWidth;
                Resource->DecMaxFrameHeight        = pResource->DecodeResource.MaxFrameHeight;
                Resource->DecMaxBitRate            = pResource->DecodeResource.MaxBitRate;
                Resource->DecMaxVideoPlaneWidth    = pResource->DecodeResource.MaxVideoPlaneWidth;
                Resource->DecMaxVideoPlaneHeight   = pResource->DecodeResource.MaxVideoPlaneHeight;
                Resource->DecMaxRatioOfGopNM       = pResource->DecodeResource.MaxRatioOfGopNM;
                Resource->DecBackwardTrickPlayMode = pResource->DecodeResource.BackwardTrickPlayMode;
                Resource->DecMaxPicWidth           = pResource->DecodeResource.MaxPicWidth;
                Resource->DecMaxPicHeight          = pResource->DecodeResource.MaxPicHeight;

                /* Display */
                for (i=0; i < AMBA_DSP_MAX_VOUT_NUM; i++) {
                    Resource->MaxVoutWidth[i] = pResource->DisplayResource.MaxVoutWidth[i];
                    Resource->MaxOsdBufWidth[i] = pResource->DisplayResource.MaxOsdBufWidth[i];
                }

                /* TimeLapse */
                Resource->MaxTimeLapseNum = 0U;
                for (i=0; i < Resource->MaxEncodeStream; i++) {
                    if (1U == DSP_GetU8Bit(Resource->MaxStrmFmt[i], 7U/*TimeLapse*/ , 1U)) {
                        Resource->MaxTimeLapseNum++;
                    }
                }

                /* ExtMemEnc */
                for (i=0; i < Resource->MaxEncodeStream; i++) {
                    Resource->MaxExtMemWidth[i] = pResource->ExtMemEncResource.MaxEncWidth[i];
                    Resource->MaxExtMemHeight[i] = pResource->ExtMemEncResource.MaxEncHeight[i];
                }

                /* Set Resource state */
                Resource->RescState = HL_RESC_CONFIGED;
                HL_GetResourceUnLock();
            } else {
                //
            }
        }
    }

    if (Rval != DSP_ERR_NONE) {
        AmbaLL_LogUInt5("dsp_resource_limit_config Error[0x%x][%u]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

static inline UINT32 HL_GetCmprRawBufInfoChk(const UINT16 Width,
                                             const UINT16 CmprRate,
                                             const UINT16 *pRawWidth,
                                             const UINT16 *pRawPitch)
{
    UINT32 Rval = DSP_ERR_NONE, ErrLine = 0U;
    const Dsp_DumpApi_t* DumpApi = AmbaDSP_GetDumpApiFunc();
    const Dsp_CheckApi_t* CheckApi = AmbaDSP_GetCheckApiFunc();

    /* Input sanity check */
    if (DumpApi != NULL) {
        DumpApi->pGetCmprRawBufInfo(Width, CmprRate, pRawWidth, pRawPitch);
    }
    if (CheckApi != NULL) {
        Rval = CheckApi->pGetCmprRawBufInfo(Width, CmprRate, pRawWidth, pRawPitch);
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
    }
    /* Logic sanity check */

    if (Rval != DSP_ERR_NONE) {
        AmbaLL_LogUInt5("HL_GetCmprRawBufInfoChk Error[0x%x][%u]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

/**
* Compression Raw buffer size query API
* @param [in]  Width Raw image width
* @param [in]  CmprRate Max hier[0] height
* @param [in/out]  pRawWidth Raw buffer width
* @param [in/out]  pRawPitch Raw buffer pitch
* @return ErrorCode
*/
UINT32 dsp_get_cmpr_raw_buf_info(UINT16 Width, UINT16 CmprRate, UINT16 *pRawWidth, UINT16 *pRawPitch)
{
    UINT32 Rval = DSP_ERR_NONE, ErrLine = 0U;
    UINT16 Mantissa = 0U, BlkSz = 0U;
    UINT8 CompressedRate = 0U, CompactMode = 0U;

    Rval = HL_GetCmprRawBufInfoChk(Width, CmprRate, pRawWidth, pRawPitch);
    DSP_FillErrline(Rval, &ErrLine, __LINE__);

    /* Body */
    if (Rval == DSP_ERR_NONE) {
        HL_DisassembleVinCompression(CmprRate, &CompressedRate, &CompactMode);
        Rval = HL_GetCmprRawBufInfo(Width, CompressedRate, CompactMode, pRawWidth, pRawPitch, &Mantissa, &BlkSz);
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
    }

    if (Rval != DSP_ERR_NONE) {
        AmbaLL_LogUInt5("dsp_get_cmpr_raw_buf_info Error[0x%x][%u]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

static inline UINT32 HL_ParLoadConfigChk(UINT32 Enable, UINT32 Data)
{
    UINT32 Rval = DSP_ERR_NONE, ErrLine = 0U;
    const Dsp_TimingApi_t* TimingApi = AmbaDSP_GetTimingApiFunc();
    const Dsp_DumpApi_t* DumpApi = AmbaDSP_GetDumpApiFunc();
    const Dsp_CheckApi_t* CheckApi = AmbaDSP_GetCheckApiFunc();

    /* Timing sanity check */
    if (TimingApi != NULL) {
        Rval = TimingApi->pParLoadConfig(Enable, Data);
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
    }
    /* Input sanity check */
    if ((Rval == DSP_ERR_NONE) && (DumpApi != NULL)) {
        DumpApi->pParLoadConfig(Enable, Data);
    }
    if ((Rval == DSP_ERR_NONE) && (CheckApi != NULL)) {
        Rval = CheckApi->pParLoadConfig(Enable, Data);
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
    }
    /* Logic sanity check */

    if (Rval != DSP_ERR_NONE) {
        AmbaLL_LogUInt5("HL_ParLoadConfigChk Error[0x%x][%u]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

/**
* Config partial load, MUST be invoked before API:AmbaDSP_MainInit
* @param [in]  Enable Enable partial load or not
* @param [in]  Data reserved
* @return ErrorCode
*/
UINT32 dsp_parload_config(UINT32 Enable, UINT32 Data)
{
    UINT32 Rval = DSP_ERR_NONE, ErrLine = 0U;
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;

    Rval = HL_ParLoadConfigChk(Enable, Data);
    DSP_FillErrline(Rval, &ErrLine, __LINE__);

    /* Body */
    if (Rval == DSP_ERR_NONE) {
        HL_GetResourceLock(&Resource);
        Resource->ParLoadEn = (UINT8)Enable;
        HL_GetResourceUnLock();
    }

    if (Rval != DSP_ERR_NONE) {
        AmbaLL_LogUInt5("dsp_parload_config Error[0x%x][%u]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

static inline UINT32 HL_ParLoadRegionUnlockChk(UINT16 RegionIdx, UINT16 SubRegionIdx)
{
    UINT32 Rval = DSP_ERR_NONE, ErrLine = 0U;
    const Dsp_TimingApi_t* TimingApi = AmbaDSP_GetTimingApiFunc();
    const Dsp_DumpApi_t* DumpApi = AmbaDSP_GetDumpApiFunc();
    const Dsp_CheckApi_t* CheckApi = AmbaDSP_GetCheckApiFunc();

    /* DSP CtxInit */
    Rval = HL_CtxInit();
    DSP_FillErrline(Rval, &ErrLine, __LINE__);

    /* Timing sanity check */
    if ((Rval == DSP_ERR_NONE) &&
        (TimingApi != NULL)) {
        Rval = TimingApi->pParLoadRegionUnlock(RegionIdx, SubRegionIdx);
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
    }
    /* Input sanity check */
    if ((Rval == DSP_ERR_NONE) &&
        (DumpApi != NULL)) {
        DumpApi->pParLoadRegionUnlock(RegionIdx, SubRegionIdx);
    }
    if ((Rval == DSP_ERR_NONE) &&
        (CheckApi != NULL)) {
        Rval = CheckApi->pParLoadRegionUnlock(RegionIdx, SubRegionIdx);
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
    }
    /* Logic sanity check */

    if (Rval != DSP_ERR_NONE) {
        AmbaLL_LogUInt5("HL_ParLoadRegionUnlockChk Error[0x%x][%u]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

/**
* Unlock partial load region, which means relative feature binary is loaded
* @param [in]  RegionIdx index of binary region, see DSP_REGION_NUM
* @param [in]  SubRegionIdx index of sub-region, reserved now
* @return ErrorCode
*/
UINT32 dsp_parload_region_unlock(UINT16 RegionIdx, UINT16 SubRegionIdx)
{
    UINT32 Rval = DSP_ERR_NONE, ErrLine = 0U;
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;
    UINT16 i;
    UINT8 ExitILoop = 0U, RegionMask, SubRegionMask = 1U;
    UINT8 FullUnlock;

    Rval = HL_ParLoadRegionUnlockChk(RegionIdx, SubRegionIdx);
    DSP_FillErrline(Rval, &ErrLine, __LINE__);

    /* Body */
    if (Rval == DSP_ERR_NONE) {
        HL_GetResourcePtr(&Resource);

        if (Resource->ParLoadEn == (UINT8)1U) {
            HL_GetResourceLock(&Resource);
            DSP_SetBit(&Resource->ParLoadMask, (UINT32)RegionIdx*PARLOAD_IDX_LEN);
            HL_GetResourceUnLock();

            for (i=0U; i<DSP_REGION_NUM; i++) {
                RegionMask = (UINT8)DSP_GetBit(Resource->ParLoadMask, (UINT32)i*PARLOAD_IDX_LEN, PARLOAD_IDX_LEN);
//FIXME, Only one SubRegionNow
                if (SubRegionMask != RegionMask) {
                    ExitILoop = 1U;
                    break;
                }
            }

            if (ExitILoop == 0U) {
                FullUnlock = 1U;
                DSP_DspParLoadUnlock(FullUnlock, 0U/*Mask*/);
            } else {
//FIXME, SubRegion update
            }
        }
    }

    if (Rval != DSP_ERR_NONE) {
        AmbaLL_LogUInt5("dsp_parload_region_unlock Error[0x%x][%u]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

static inline UINT32 HL_SysDrvCfg_CorePostProc_impl(UINT32 CoreFreq)
{
    UINT32 Rval = DSP_ERR_NONE;
    UINT32 ErrLine = 0U;
    CTX_RESOURCE_INFO_s *pResource = HL_CtxResInfoPtr;
    cmd_dsp_hal_inf_t *pSysHalSetup = HL_DefCtxCmdBufPtrHal;
    UINT8 CmdBufId = 0U;
    void *pCmdBufAddr = NULL;
    UINT8 WriteMode = AMBA_DSP_CMD_VDSP_NORMAL_WRITE;
    ULONG EvntBufAddr = 0U;
    UINT16 EvntBufId = 0U;
    AMBA_DSP_EVENT_CLK_UPDATE_STATUS_INFO_s *pClkUptEvnt = NULL;

    HL_GetResourcePtr(&pResource);

    if (DSP_GetProfState() != DSP_PROF_STATUS_INVALID) {
        /*
         * 2022/10/26,
         *   HL use core clock to calculate wait-tick :
         *     vout frame wait time.
         *     cmd-reg wait time.
         *   when core clock change to higher runtime,
         *     we shall invoke CMD_DSP_HAL_INF to indicate HL update tick value before clock change,
         *     otherwise timeout may happen
         *   when core clokc change to lower
         *     we shall invoke CMD_DSP_HAL_INF to indicate HL update tick value after clock change,
         *     otherwise timeout may happen
         */
         if (CoreFreq < pResource->CoreClk) {
            /* invoke HAL */
            HL_AcqCmdBuffer(&CmdBufId, &pCmdBufAddr);
            dsp_osal_typecast(&pSysHalSetup, &pCmdBufAddr);

            Rval = HL_FillSystemHalSetup(pSysHalSetup);
            pSysHalSetup->core_clk_freq = CoreFreq;

            if (Rval == DSP_ERR_NONE) {
                Rval = AmbaHL_CmdDspHalInf(WriteMode, pSysHalSetup);
            }
            HL_RelCmdBuffer(CmdBufId);

            if (Rval == DSP_ERR_NONE) {
                /* wait 2 vdsp to make sure HL update new core clock */
                Rval = DSP_WaitVdspEvent((UINT32)EVENT_FLAG_INT_VDSP0_EVENT, 2U, WAIT_INTERRUPT_TIMEOUT);
                DSP_FillErrline(Rval, &ErrLine, __LINE__);
            }

            /* issue event */
            if (Rval == DSP_ERR_NONE) {
                (void)DSP_RequestEventInfoBuf(EVENT_INFO_POOL_PROT_BUF, &EvntBufAddr, &EvntBufId, LL_EVENT_CLK_UPT_RDY);
                dsp_osal_typecast(&pClkUptEvnt, &EvntBufAddr);
                pClkUptEvnt->ClockId = AMBA_DSP_CLOCK_ID_CORE;
                pClkUptEvnt->Stage = AMBA_DSP_CLOCK_STAGE_POSTPROC;

                (void)DSP_GiveEvent(LL_EVENT_CLK_UPT_RDY, pClkUptEvnt, EvntBufId);
            }
        }
    }

    HL_GetResourceLock(&pResource);
    pResource->CoreClk = CoreFreq;
    HL_GetResourceUnLock();

    return Rval;
}

static inline UINT32 HL_SysDrvCfg_CorePreProc_impl(UINT32 CoreFreq)
{
    UINT32 Rval = DSP_ERR_NONE;
    UINT32 ErrLine = 0U;
    CTX_RESOURCE_INFO_s *pResource = HL_CtxResInfoPtr;
    cmd_dsp_hal_inf_t *pSysHalSetup = HL_DefCtxCmdBufPtrHal;
    UINT8 CmdBufId = 0U;
    void *pCmdBufAddr = NULL;
    UINT8 WriteMode = AMBA_DSP_CMD_VDSP_NORMAL_WRITE;
    ULONG EvntBufAddr = 0U;
    UINT16 EvntBufId = 0U;
    AMBA_DSP_EVENT_CLK_UPDATE_STATUS_INFO_s *pClkUptEvnt = NULL;

    HL_GetResourcePtr(&pResource);

    if (DSP_GetProfState() != DSP_PROF_STATUS_INVALID) {
        /*
         * 2022/10/26,
         *   HL use core clock to calculate wait-tick :
         *     vout frame wait time.
         *     cmd-reg wait time.
         *   when core clock change to higher runtime,
         *     we shall invoke CMD_DSP_HAL_INF to indicate HL update tick value before clock change,
         *     otherwise timeout may happen
         *   when core clokc change to lower
         *     we shall invoke CMD_DSP_HAL_INF to indicate HL update tick value after clock change,
         *     otherwise timeout may happen
         */
         if (CoreFreq > pResource->CoreClk) {
            /* invoke HAL */
            HL_AcqCmdBuffer(&CmdBufId, &pCmdBufAddr);
            dsp_osal_typecast(&pSysHalSetup, &pCmdBufAddr);
            Rval = HL_FillSystemHalSetup(pSysHalSetup);
            pSysHalSetup->core_clk_freq = CoreFreq;

            if (Rval == DSP_ERR_NONE) {
                Rval = AmbaHL_CmdDspHalInf(WriteMode, pSysHalSetup);
            }
            HL_RelCmdBuffer(CmdBufId);

            if (Rval == DSP_ERR_NONE) {
                /* wait 2 vdsp to make sure HL update new core clock */
                Rval = DSP_WaitVdspEvent((UINT32)EVENT_FLAG_INT_VDSP0_EVENT, 2U, WAIT_INTERRUPT_TIMEOUT);
                DSP_FillErrline(Rval, &ErrLine, __LINE__);
            }

            /* issue event */
            if (Rval == DSP_ERR_NONE) {
                (void)DSP_RequestEventInfoBuf(EVENT_INFO_POOL_PROT_BUF, &EvntBufAddr, &EvntBufId, LL_EVENT_CLK_UPT_RDY);
                dsp_osal_typecast(&pClkUptEvnt, &EvntBufAddr);
                pClkUptEvnt->ClockId = AMBA_DSP_CLOCK_ID_CORE;
                pClkUptEvnt->Stage = AMBA_DSP_CLOCK_STAGE_PREPROC;

                (void)DSP_GiveEvent(LL_EVENT_CLK_UPT_RDY, pClkUptEvnt, EvntBufId);
            }
        }
    }
    return Rval;
}

UINT32 dsp_sys_drv_cfg(UINT16 Type, const void *pDrvData)
{
    UINT32 Rval = DSP_ERR_NONE, ErrLine = 0U;
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;
    UINT32 Val;

    /* DSP CtxInit */
    Rval = HL_CtxInit();
    DSP_FillErrline(Rval, &ErrLine, __LINE__);

    /* Input sanity check */
    if (Type >= NUM_DSP_SYS_DRV_CFG_TYPE) {
        Rval = DSP_ERR_0001;
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
    } else if (pDrvData == NULL) {
        Rval = DSP_ERR_0000;
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
    } else {
        // DO NOTHING
    }

    /* HW/SW resources check */

    /* Timing sanity check */

    /* Logic sanity check */

    /* Body */
    if (Rval == DSP_ERR_NONE) {
        if (Type == DSP_SYS_DRV_CFG_TYPE_IDSP_CLK) {
            HL_GetResourceLock(&Resource);
            Rval = dsp_osal_memcpy(&Resource->IdspClk, pDrvData, sizeof(UINT32));
            DSP_FillErrline(Rval, &ErrLine, __LINE__);
            HL_GetResourceUnLock();
        } else if (Type == DSP_SYS_DRV_CFG_TYPE_CORE_CLK) {
            Rval = dsp_osal_memcpy(&Val, pDrvData, sizeof(UINT32));
            DSP_FillErrline(Rval, &ErrLine, __LINE__);
            Rval = HL_SysDrvCfg_CorePostProc_impl(Val);
            DSP_FillErrline(Rval, &ErrLine, __LINE__);
        } else if (Type == DSP_SYS_DRV_CFG_TYPE_DDR_CLK) {
            HL_GetResourceLock(&Resource);
            Rval = dsp_osal_memcpy(&Resource->DramClk, pDrvData, sizeof(UINT32));
            DSP_FillErrline(Rval, &ErrLine, __LINE__);
            HL_GetResourceUnLock();
        } else if (Type == DSP_SYS_DRV_CFG_TYPE_AUD_CLK) {
            HL_GetResourceLock(&Resource);
            Rval = dsp_osal_memcpy(&Resource->AudioClk, pDrvData, sizeof(UINT32));
            DSP_FillErrline(Rval, &ErrLine, __LINE__);
            HL_GetResourceUnLock();
        } else if (Type == DSP_SYS_DRV_CFG_TYPE_CORE_CLK_PRE) {
            Rval = dsp_osal_memcpy(&Val, pDrvData, sizeof(UINT32));
            DSP_FillErrline(Rval, &ErrLine, __LINE__);
            Rval = HL_SysDrvCfg_CorePreProc_impl(Val);
            DSP_FillErrline(Rval, &ErrLine, __LINE__);
        } else {
            // DO NOTHING
        }
    }

    if (Rval != DSP_ERR_NONE) {
        AmbaLL_LogUInt5("dsp_sys_drv_cfg Error[0x%x][%u]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

UINT32 AmbaDSP_CalcEncAvcMvBuf(UINT16 Width,
                               UINT16 Height,
                               UINT16 *pBufPitch,
                               UINT16 *pBufWidth,
                               UINT16 *pBufHeight)
{
    UINT32 Rval = DSP_ERR_NONE;

    *pBufWidth = (UINT16)(((UINT32)Width + (AVC_MB_SIZE - 1U))/AVC_MB_SIZE)* AVC_MV_SIZE;
    *pBufPitch = ALIGN_NUM16(*pBufWidth, 32U);
    *pBufHeight = (UINT16)(((UINT32)Height + (AVC_MB_SIZE - 1U))/AVC_MB_SIZE);

    return Rval;
}

static inline UINT32 HL_CalcEncMvBufInfoChk(const UINT16 Width,
                                            const UINT16 Height,
                                            const UINT32 Option,
                                            const UINT32 *pBufSize)
{
    UINT32 Rval = DSP_ERR_NONE, ErrLine = 0U;
    const Dsp_TimingApi_t* TimingApi = AmbaDSP_GetTimingApiFunc();
    const Dsp_DumpApi_t* DumpApi = AmbaDSP_GetDumpApiFunc();
    const Dsp_CheckApi_t* CheckApi = AmbaDSP_GetCheckApiFunc();

    /* DSP CtxInit */
    Rval = HL_CtxInit();
    DSP_FillErrline(Rval, &ErrLine, __LINE__);

    /* Timing sanity check */
    if ((Rval == DSP_ERR_NONE) &&
        (TimingApi != NULL)) {
        Rval = TimingApi->pCalcEncMvBufInfo(Width, Height, Option, pBufSize);
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
    }
    /* Input sanity check */
    if ((Rval == DSP_ERR_NONE) &&
        (DumpApi != NULL)) {
        DumpApi->pCalcEncMvBufInfo(Width, Height, Option, pBufSize);
    }
    if ((Rval == DSP_ERR_NONE) &&
        (CheckApi != NULL)) {
        Rval = CheckApi->pCalcEncMvBufInfo(Width, Height, Option, pBufSize);
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
    }
    /* Logic sanity check */

    if (Rval != DSP_ERR_NONE) {
        AmbaLL_LogUInt5("HL_CalcEncMvBufInfoChk Error[0x%x][%u]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

#define HEVC_CTB_GRID_SIZE  (32U) //32 pixel
#define HEVC_CTB_INTRA_SIZE (20U)
#define HEVC_COLOC_MV_SIZE  (48U)
/**
* Calc MV buffer information
* @param [in]  Width Encode Stream width
* @param [in]  Height Encode Stream height
* @param [in]  Option optional setting, B[7:0] FmtType, see AMBA_DSP_ENC_MV_FMT_NUM
* @param [in]  pBufSize buffer size
* @return ErrorCode
*/
UINT32 dsp_calc_enc_mv_buf_info(UINT16 Width, UINT16 Height, UINT32 Option, UINT32 *pBufSize)
{
    UINT32 Rval = DSP_ERR_NONE, ErrLine = 0U;
    UINT8 FmtType = (UINT8)Option;

    Rval = HL_CalcEncMvBufInfoChk(Width, Height, Option, pBufSize);
    DSP_FillErrline(Rval, &ErrLine, __LINE__);

    /* Body */
    if (Rval == DSP_ERR_NONE) {
        if (FmtType == AMBA_DSP_ENC_MV_FMT_AVC) {
            UINT16 BufWidth, BufPitch, BufHeight;

            Rval = AmbaDSP_CalcEncAvcMvBuf(Width, Height, &BufPitch, &BufWidth, &BufHeight);
            DSP_FillErrline(Rval, &ErrLine, __LINE__);
            *pBufSize = (UINT32)BufPitch * BufHeight;
        }
        //AmbaLL_LogUInt5("CalcEncMvBuf [%d %d] %d", Width, Height, *pBufSize, 0U, 0U);
    }

    if (Rval != DSP_ERR_NONE) {
        AmbaLL_LogUInt5("dsp_calc_enc_mv_buf_info Error[0x%x][%u]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

UINT32 dsp_parse_mv_buf(UINT16 Width, UINT16 Height, UINT16 BufType, UINT32 Option, ULONG MvBufAddr)
{
    typedef struct {
        UINT16 IntraActivity;
    } GNU_MIN_PADDING CTB_BLK_INTRA_s;

    /* TBD, when using inter16 */
    typedef struct {
        CTB_BLK_INTRA_s Rsvd0[4U];
        CTB_BLK_INTRA_s Intra_0_0;
        CTB_BLK_INTRA_s Intra_0_1;
        CTB_BLK_INTRA_s Intra_1_0;
        CTB_BLK_INTRA_s Intra_1_1;
        CTB_BLK_INTRA_s Rsvd1[2U];
    } GNU_MIN_PADDING CTB_INTRA_s;

    typedef struct {
        UINT32 MvX:14;  //INT32
        UINT32 Rsvd:1;
        UINT32 MvY:12;  //INT32
        UINT32 Rsvd1:3;
        UINT32 RefIsLongTerm:1;
        UINT32 Valid:1;
    } GNU_MIN_PADDING COLOC_BLK_MV_s;

    /* TBD, when using inter16 */
    typedef struct {
        COLOC_BLK_MV_s MV_0_0;
        COLOC_BLK_MV_s Rsvd0[1U];
        COLOC_BLK_MV_s MV_0_1;
        COLOC_BLK_MV_s Rsvd1[3U];
        COLOC_BLK_MV_s MV_1_0;
        COLOC_BLK_MV_s Rsvd2[1U];
        COLOC_BLK_MV_s MV_1_1;
        COLOC_BLK_MV_s Rsvd3[3U];
    } COLOC_MV_s;

    typedef struct {
        UINT32 MvX:15;  //MV X, INT15
        UINT32 MvY:12;  //MV Y, INT12
        UINT32 Rsvd:5;
    } AVC_MV_s;

    UINT32 Rval = DSP_ERR_NONE, ErrLine = 0U;
    UINT16 CtbRowNum, CtbColNum, i, j;
    UINT32 CtbPitch;
    UINT8 FmtType;
    const CTB_INTRA_s *pCtbIntra;
    const COLOC_MV_s *pColocMv;
    const void *pVoid = NULL;
    ULONG ULAddr = 0U;

    if ((Width == 0U) ||
        (Height == 0U) ||
        (BufType > 2U) ||
        (MvBufAddr == 0U)) {
        Rval = DSP_ERR_0001;
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
    } else {
        FmtType = (UINT8)Option;

        if (FmtType == AMBA_DSP_ENC_MV_FMT_AVC) {
            UINT16 GridRowNum, GridColNum, idx;
            const AVC_MV_s *pMv;

            dsp_osal_typecast(&pMv, &MvBufAddr);
            GridColNum = (UINT16)(((UINT32)Width + (AVC_MB_SIZE - 1U))/AVC_MB_SIZE);
            GridRowNum = (UINT16)(((UINT32)Height + (AVC_MB_SIZE - 1U))/AVC_MB_SIZE);
#define MV_DUMP_SAMPLE_NUM   (4U)
            AmbaLL_LogUInt5("====== MV Col[%d] Row[%d] Pick 4X4 ======", GridColNum, GridRowNum, 0U, 0U, 0U);
            for (j=0U; j<MV_DUMP_SAMPLE_NUM; j++) {
                UINT16 UnitSampleGridColNum = GridColNum/MV_DUMP_SAMPLE_NUM;
                UINT16 UnitSampleGridRowNum = GridRowNum/MV_DUMP_SAMPLE_NUM;
                for (i=0U; i<MV_DUMP_SAMPLE_NUM; i++) {
                    idx = (j*UnitSampleGridColNum)+(i*UnitSampleGridRowNum);
                    AmbaLL_LogUInt5("  [%d][%d] MV[%d %d]", i, j, pMv[idx].MvX, pMv[idx].MvY, 0U);
                }
            }
        } else {
            CtbColNum = (UINT16)(ALIGN_NUM(Width, HEVC_CTB_GRID_SIZE) >> 5U);
            CtbRowNum =  (UINT16)(ALIGN_NUM(Height, HEVC_CTB_GRID_SIZE) >> 5U);

            if (BufType == 0U) {
                CtbPitch = ALIGN_NUM((((UINT32)CtbColNum)*HEVC_CTB_INTRA_SIZE), 64U);
                dsp_osal_typecast(&pVoid, &MvBufAddr);
                Rval = dsp_osal_cache_invalidate(pVoid, CtbPitch*CtbRowNum);
                DSP_FillErrline(Rval, &ErrLine, __LINE__);

                for (i=0U; i<CtbRowNum; i++) {
                    ULAddr = MvBufAddr + ((ULONG)i*CtbPitch);
                    dsp_osal_typecast(&pCtbIntra, &ULAddr);
                    AmbaLL_LogUInt5("CtbIntra Row[%d] Addr[0x%X%X]", i, DSP_GetU64Msb((UINT64)ULAddr), DSP_GetU64Lsb((UINT64)ULAddr), 0U, 0U);
                    for (j=0U; j<CtbColNum; j++) {
                        AmbaLL_LogUInt5("         Col[%d] Intra[%d %d %d %d]", j, pCtbIntra[j].Intra_0_0.IntraActivity,
                                                                                  pCtbIntra[j].Intra_0_1.IntraActivity,
                                                                                  pCtbIntra[j].Intra_1_0.IntraActivity,
                                                                                  pCtbIntra[j].Intra_1_1.IntraActivity);
                    }
                }
            } else if (BufType == 1U) {
                CtbPitch = ALIGN_NUM((((UINT32)CtbColNum + 1U)*HEVC_COLOC_MV_SIZE), 64U);
                dsp_osal_typecast(&pVoid, &MvBufAddr);
                Rval = dsp_osal_cache_invalidate(pVoid, CtbPitch*CtbRowNum);
                DSP_FillErrline(Rval, &ErrLine, __LINE__);

                for (i=0U; i<CtbRowNum; i++) {
                    ULAddr = MvBufAddr + ((ULONG)i*CtbPitch);
                    dsp_osal_typecast(&pColocMv, &ULAddr);
                    AmbaLL_LogUInt5("CtbMv Row[%d] Addr[0x%X%X]", i, DSP_GetU64Msb((UINT64)ULAddr), DSP_GetU64Lsb((UINT64)ULAddr), 0U, 0U);
                    for (j=0U; j<CtbColNum; j++) {
                        AmbaLL_LogUInt5("      Col[%d] Valid[%d] Ref[%d]", j, pColocMv[j].MV_0_0.Valid,
                                                                              pColocMv[j].MV_0_1.RefIsLongTerm,
                                                                              0U, 0U);
                        AmbaLL_LogUInt5("        MvX[%d] MvY[%d]", pColocMv[j].MV_0_0.MvX, pColocMv[j].MV_0_0.MvY, 0, 0, 0);
                    }
                }
            } else {
                //TBD
            }
        }
    }

    if (Rval != DSP_ERR_NONE) {
        AmbaLL_LogUInt5("dsp_parse_mv_buf Error[0x%x][%u]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

void dsp_main_api_dump_init(UINT8 Enable, UINT32 Mask)
{
    DumpApiReg(Enable, Mask);
}

void dsp_main_api_check_init(UINT8 Disable, UINT32 Mask)
{
    CheckApiReg(Disable, Mask, 0U);
}

void dsp_reset_dsp(void)
{
    UINT32 Rval = DSP_ERR_NONE, ErrLine = 0U;
    UINT32 Addr;
    const void *pVoid = NULL;
    /*
     * we can reset idsp/vdsp without reset cortex
     * it may not work 100%
     * [Step]
     *  1) Put ucode to idle profile (skip this step if ucode hang)
     *  2) Write 0x80000000 to 0xED160000/0xED020000/0xED030000 to reset all DSP ORC
     *  3) Wait 50ms for dram controller to drain outstanding dram transfers from DSP/SMEM
     *  4) Write 0x38  to 0xED080228 to reset smem/vdsp1/idsp.
     *  5) Write 0x0 to 0XED080228 to deassert the reset
     *  6) Reboot ucode
     */

    /* Park DSP */
    AmbaLL_LogUInt5("ResetDSP @ %d", DSP_GetProfState(), 0U, 0U, 0U, 0U);
    if (DSP_GetProfState() == DSP_PROF_STATUS_IDLE) {

    } else {
        /* Switch to idle */
    }

    /* Reset DSP ORC */
    Addr = 0xED160000U;
    (void)dsp_osal_memcpy(&pVoid, &Addr, sizeof(UINT32));
    dsp_osal_writel_relaxed(0x80000000U, pVoid);

    Addr = 0xED020000U;
    (void)dsp_osal_memcpy(&pVoid, &Addr, sizeof(UINT32));
    dsp_osal_writel_relaxed(0x80000000U, pVoid);

    Addr = 0xED030000U;
   (void)dsp_osal_memcpy(&pVoid, &Addr, sizeof(UINT32));
    dsp_osal_writel_relaxed(0x80000000U, pVoid);

    /* Wait 50msec */
    Rval = dsp_osal_sleep(50U);
    DSP_FillErrline(Rval, &ErrLine, __LINE__);

    if (Rval == DSP_ERR_NONE) {
        /* Assert RCT-isoc/dsp/vsoc/dsp/smem */
        Addr = 0xED080228U;
        (void)dsp_osal_memcpy(&pVoid, &Addr, sizeof(UINT32));
        dsp_osal_writel_relaxed(0x38U, pVoid);

        /* Deassert RCT-isoc/dsp/vsoc/dsp/smem */
        Addr = 0xED080228U;
        (void)dsp_osal_memcpy(&pVoid, &Addr, sizeof(UINT32));
        dsp_osal_writel_relaxed(0x0U, pVoid);

        Rval = DSP_SetProfState(DSP_PROF_STATUS_INVALID);
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
    }

    if (Rval == DSP_ERR_NONE) {
        AmbaLL_LogUInt5("ResetDSP Update Prof @ %d", DSP_GetProfState(), 0U, 0U, 0U, 0U);
        DSP_Suspend(1U);
    } else {
        AmbaLL_LogUInt5("dsp_reset_dsp Error[0x%x][%u]", Rval, ErrLine, 0U, 0U, 0U);
    }
}

UINT32 dsp_stop(UINT32 CmdType)
{
#ifdef SUPPORT_DSP_SET_IDSP_DBG_CMD
    UINT8 WriteMode = AMBA_DSP_CMD_VDSP_NORMAL_WRITE;
    UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
    UINT32 Rval = DSP_ERR_NONE, ErrLine = 0U;
    send_idsp_debug_cmd_t *IdspDebug = HL_DefCtxCmdBufPtrDbgIdsp;

    if (CmdType == 0U) {
        WriteMode = AMBA_DSP_CMD_VDSP_NORMAL_WRITE;
    } else {
        WriteMode = AMBA_DSP_CMD_VCAP_NORMAL_WRITE;
    }

    HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
    dsp_osal_typecast(&IdspDebug, &CmdBufferAddr);
    IdspDebug->mode = 1U;
    Rval = AmbaHL_CmdSendIdspDebugCmd(WriteMode, IdspDebug);
    DSP_FillErrline(Rval, &ErrLine, __LINE__);
    HL_RelCmdBuffer(CmdBufferId);

    if (Rval != DSP_ERR_NONE) {
        AmbaLL_LogUInt5("dsp_stop Error[0x%x][%u]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
#else
    /* Temp use Unknown Cmd(0x100BEEF) to make DSP stop */
    UINT8 WriteMode = AMBA_DSP_CMD_VDSP_NORMAL_WRITE;
    UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
    UINT32 Rval = DSP_ERR_NONE, ErrLine = 0U;
    cmd_dsp_suspend_profile_t *SusOpMode = HL_DefCtxCmdBufPtrSus;

    if (CmdType == 0U) {
        WriteMode = AMBA_DSP_CMD_VDSP_NORMAL_WRITE;
    } else {
        WriteMode = AMBA_DSP_CMD_VCAP_NORMAL_WRITE;
    }

    HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
    dsp_osal_typecast(&SusOpMode, &CmdBufferAddr);
    SusOpMode->cmd_code = 0x1000BEEFU;
    Rval = AmbaLL_CmdSend(WriteMode, SusOpMode, (UINT32)sizeof(cmd_dsp_suspend_profile_t));
    DSP_FillErrline(Rval, &ErrLine, __LINE__);
    HL_RelCmdBuffer(CmdBufferId);

    if (Rval != DSP_ERR_NONE) {
        AmbaLL_LogUInt5("dsp_stop Error[0x%x][%u]", Rval, ErrLine, 0U, 0U, 0U);
    }

    AmbaLL_LogUInt5("Issue 0x100BEEF to stop dsp", 0U, 0U, 0U, 0U, 0U);
    return Rval;
#endif
}

static inline UINT32 HL_MainGetBufInfoChk(const UINT32 Type, const AMBA_DSP_BUF_INFO_s *pBufInfo)
{
    UINT32 Rval = DSP_ERR_NONE, ErrLine = 0U;
    const Dsp_DumpApi_t* DumpApi = AmbaDSP_GetDumpApiFunc();
    const Dsp_CheckApi_t* CheckApi = AmbaDSP_GetCheckApiFunc();

    /* DSP CtxInit */
    Rval = HL_CtxInit();
    DSP_FillErrline(Rval, &ErrLine, __LINE__);

    /* Timing sanity check */
    /* Input sanity check */
    if ((Rval == DSP_ERR_NONE) &&
        (DumpApi != NULL)) {
        DumpApi->pMainGetBufInfo(Type, pBufInfo);
    }
    if ((Rval == DSP_ERR_NONE) &&
        (CheckApi != NULL)) {
        Rval = CheckApi->pMainGetBufInfo(Type, pBufInfo);
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
    }
    /* HW/SW resources check */
    /* Logic sanity check */

    if (Rval != DSP_ERR_NONE) {
        AmbaLL_LogUInt5("HL_MainGetBufInfoChk Error[0x%x][%u]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

UINT32 dsp_main_get_buf_info(UINT32 Type, AMBA_DSP_BUF_INFO_s *pBufInfo)
{
    UINT32 Rval;
    UINT32 ErrLine = 0U;
    DSP_MEM_BLK_t MemBlk = {0};
    ULONG PhysAddr;

    Rval = HL_MainGetBufInfoChk(Type, pBufInfo);
    DSP_FillErrline(Rval, &ErrLine, __LINE__);

    /* Body */
    if (Rval == DSP_ERR_NONE) {
        if (Type == DSP_BUF_TYPE_DATA) {
            Rval = dsp_osal_get_data_buf(&MemBlk.Base, &MemBlk.Size, &MemBlk.IsCached);
            DSP_FillErrline(Rval, &ErrLine, __LINE__);
            Rval = dsp_osal_virt2phys(MemBlk.Base, &PhysAddr);
            DSP_FillErrline(Rval, &ErrLine, __LINE__);
            pBufInfo->BaseAddr = PhysAddr;
            pBufInfo->Size = MemBlk.Size;
            pBufInfo->IsCached = MemBlk.IsCached;
        } else if (Type == DSP_BUF_TYPE_BIN) {
            Rval = dsp_osal_get_bin_buf(&MemBlk.Base, &MemBlk.Size, &MemBlk.IsCached);
            DSP_FillErrline(Rval, &ErrLine, __LINE__);
            Rval = dsp_osal_virt2phys(MemBlk.Base, &PhysAddr);
            DSP_FillErrline(Rval, &ErrLine, __LINE__);
            pBufInfo->BaseAddr = PhysAddr;
            pBufInfo->Size = MemBlk.Size;
            pBufInfo->IsCached = MemBlk.IsCached;
        } else if (Type == DSP_BUF_TYPE_STAT) {
            Rval = DSP_GetStatBuf(DSP_STAT_BUF_ALL, 0U, &MemBlk);
            DSP_FillErrline(Rval, &ErrLine, __LINE__);
            Rval = dsp_osal_virt2phys(MemBlk.Base, &PhysAddr);
            DSP_FillErrline(Rval, &ErrLine, __LINE__);
            pBufInfo->BaseAddr = PhysAddr;
            pBufInfo->Size = MemBlk.Size;
            pBufInfo->IsCached = MemBlk.IsCached;
        } else if (Type == DSP_BUF_TYPE_EVT) {
            Rval = DSP_GetProtBuf(DSP_PROT_BUF_EVENT_ALL, 0U, &MemBlk);
            DSP_FillErrline(Rval, &ErrLine, __LINE__);
            Rval = dsp_osal_virt2phys(MemBlk.Base, &PhysAddr);
            DSP_FillErrline(Rval, &ErrLine, __LINE__);
            pBufInfo->BaseAddr = PhysAddr;
            pBufInfo->Size = MemBlk.Size;
            pBufInfo->IsCached = MemBlk.IsCached;
        } else {
            Rval = DSP_ERR_0001;
            DSP_FillErrline(Rval, &ErrLine, __LINE__);
        }
    }

    if (Rval != DSP_ERR_NONE) {
        AmbaLL_LogUInt5("dsp_main_get_buf_info Error[0x%x][%u]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

UINT32 dsp_main_get_dsp_bin_addr(AMBA_DSP_BIN_ADDR_s *pInfo)
{
    UINT32 Rval = DSP_ERR_NONE;
    osal_ucode_addr_t BinAddr = {0};

    (void)dsp_osal_get_ucode_base_addr(&BinAddr);
    pInfo->CoreStartAddr = BinAddr.CodeAddr;
    pInfo->MeStartAddr = BinAddr.MeAddr;
    pInfo->MdxfStartAddr = BinAddr.MdxfAddr;
    pInfo->DefaultDataStartAddr = BinAddr.DefBinAddr;

    (void)dsp_osal_get_ucode_end_addr(&BinAddr);
    pInfo->CoreEndAddr = BinAddr.CodeAddr;
    pInfo->MeEndAddr = BinAddr.MeAddr;
    pInfo->MdxfEndAddr = BinAddr.MdxfAddr;
    pInfo->DefaultDataEndAddr = BinAddr.DefBinAddr;

    return Rval;
}

/**
 * @brief       DSP API for user to query status information
 * @param       Type    : [IN] query type
 * @param       Id      : [IN] query type
 * @param       pData0  : [OUT] return data0
 * @param       pData1  : [OUT] return data1
 * @return      DSP_ERR_NONE \n
                DSP_ERR_0000 \n
                DSP_ERR_0001 \n
 */
UINT32 dsp_get_status(dsp_status_t *pDspStatus)
{
    UINT32 Rval = DSP_ERR_NONE, ErrLine = 0U;
    CTX_RESOURCE_INFO_s *pResource = HL_CtxResInfoPtr;

    /* DSP CtxInit */
    Rval = HL_CtxInit();
    DSP_FillErrline(Rval, &ErrLine, __LINE__);

    if (Rval == DSP_ERR_NONE) {
        if (pDspStatus->Type == (UINT16)DSP_STATUS_TYPE_OP_MODE) {
            HL_GetResourceLock(&pResource);
            pDspStatus->Data[0U] = pResource->DspSysState;
            HL_GetResourceUnLock();
        } else if (pDspStatus->Type == (UINT16)DSP_STATUS_TYPE_ENC_MODE) {
            pDspStatus->Data[0U] = (UINT32)DSP_GetEncState((UINT16)pDspStatus->Id);
        } else if (pDspStatus->Type == (UINT16)DSP_STATUS_TYPE_DEC_MODE) {
            pDspStatus->Data[0U] = (UINT32)DSP_GetDecState((UINT16)pDspStatus->Id);
        } else{
            // do nothing
        }
    } else {
       AmbaLL_LogUInt5("dsp_get_status Error[0x%x][%u]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

