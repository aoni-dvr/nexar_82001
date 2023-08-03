/**
 *  @file dsp_api.c
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
 *  @details DSP APIs
 *
 */

#include "dsp_wrapper_osal.h"
#include "ambadsp_ioctl.h"
#include "dsp_priv_api.h"
#include "AmbaDSP.h"
#include "AmbaDSP_VOUT.h"
#include "AmbaDSP_Liveview.h"
#include "AmbaDSP_VideoEnc.h"
#include "AmbaDSP_StillCapture.h"
#include "AmbaDSP_VideoDec.h"
#include "AmbaDSP_StillDec.h"
#include "AmbaVIN.h"
#include "AmbaVOUT.h"
#include "AmbaDSP_ImageUtility.h"
#include "AmbaDSP_ImageFilter.h"
#include "AmbaDSP_Log.h"
#if defined (CONFIG_ENABLE_DSP_DIAG)
#include "AmbaDSP_Diag.h"
#endif
#include "AmbaMemProt.h"

#define SUPPORT_AMBA_IK
#define SUPPORT_AMBA_SYS_GET_DRAM_CLK

#if defined (CONFIG_SOC_CV2FS) || defined (CONFIG_SOC_CV22FS) || defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
#define SUPPORT_IK_FRM_INFO_QUERY
#endif

#define DSP_VZ_IK_IDX_INVALID (0xFFFFU) //indicate ViewZone don't need IK setting
static UINT32 ViewZoneIkCtxId[AMBA_DSP_MAX_VIEWZONE_NUM] = {0};
static UINT8  ViewZonePipe[AMBA_DSP_MAX_VIEWZONE_NUM] = {0};
static UINT8  ViewZoneRotate[AMBA_DSP_MAX_VIEWZONE_NUM] = {0};
#if defined (CONFIG_SOC_CV2FS) || defined (CONFIG_SOC_CV22FS) || defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
static UINT16 ViewZoneSource[AMBA_DSP_MAX_VIEWZONE_NUM] = {0};
#endif

#if defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
static UINT32 VoutDisplayOption[AMBA_DSP_MAX_VOUT_NUM] = {0};
static UINT16 ViewZoneMaxHorWarpComp[AMBA_DSP_MAX_VIEWZONE_NUM] = {0};
#endif

extern UINT32 AmbaIK_GetIsoHdrRawOffest(const void *pIsocfg, AMBA_IK_HDR_RAW_INFO_s *pHdrRawInfo);

#if defined (CONFIG_SOC_CV2FS) || defined (CONFIG_SOC_CV22FS) || defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
static AMBA_DSP_LIVEVIEW_SLICE_CFG_s ViewZoneSliceCfg[AMBA_DSP_MAX_VIEWZONE_NUM] = {0};
static UINT32 IsLvTileMode = 0U;
#endif

UINT32 AmbaDSP_MainGetDspBinAddr(AMBA_DSP_BIN_ADDR_s *Info);
UINT32 AmbaDSP_MainGetDspBinAddr(AMBA_DSP_BIN_ADDR_s *Info)
{
    return dsp_main_get_dsp_bin_addr(Info);
}

UINT32 AmbaDSP_MainInit(const AMBA_DSP_SYS_CONFIG_s *pDspSysConfig)
{
    UINT32 Rval = DSP_ERR_NONE;
    UINT32 Freq;

    Rval = AmbaSYS_GetClkFreq(AMBA_SYS_CLK_IDSP, &Freq);
    if (Rval != DSP_ERR_NONE) {
        dsp_wrapper_osal_printU5("[ERR] AmbaDSP_MainInit() : AmbaSYS_GetClkFreq[IDSP] fail 0x%X", Rval, 0U, 0U, 0U, 0U);
    } else {
        Rval = dsp_sys_drv_cfg(DSP_SYS_DRV_CFG_TYPE_IDSP_CLK, &Freq);
    }

#if defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
    if (Rval == DSP_ERR_NONE) {
        Rval = AmbaSYS_GetClkFreq(AMBA_SYS_CLK_IDSPV, &Freq);
        if (Rval != DSP_ERR_NONE) {
            dsp_wrapper_osal_printU5("[ERR] AmbaDSP_MainInit() : AmbaSYS_GetClkFreq[IDSPV] fail 0x%X", Rval, 0U, 0U, 0U, 0U);
        } else {
            Rval = dsp_sys_drv_cfg(DSP_SYS_DRV_CFG_TYPE_IDSPV_CLK, &Freq);
        }
    }
#endif

    if (Rval == DSP_ERR_NONE) {
        Rval = AmbaSYS_GetClkFreq(AMBA_SYS_CLK_CORE, &Freq);
        if (Rval != DSP_ERR_NONE) {
            dsp_wrapper_osal_printU5("[ERR] AmbaDSP_MainInit() : AmbaSYS_GetClkFreq[Core] fail 0x%X", Rval, 0U, 0U, 0U, 0U);
        } else {
            Rval = dsp_sys_drv_cfg(DSP_SYS_DRV_CFG_TYPE_CORE_CLK, &Freq);
        }
    }

    if (Rval == DSP_ERR_NONE) {
#ifdef SUPPORT_AMBA_SYS_GET_DRAM_CLK
        Rval = AmbaSYS_GetClkFreq(AMBA_SYS_CLK_DRAM, &Freq);
        if (Rval != DSP_ERR_NONE) {
            dsp_wrapper_osal_printU5("[ERR] AmbaDSP_MainInit() : AmbaSYS_GetClkFreq[DDR] fail 0x%X", Rval, 0U, 0U, 0U, 0U);
        } else
#else
        dsp_wrapper_osal_printU5("AmbaSYS_GetClkFreq[DDR] n/a Force 2016Mhz", Rval, 0U, 0U, 0U, 0U);
        Freq = 2016000000U;
#endif
        {
            Rval = dsp_sys_drv_cfg(DSP_SYS_DRV_CFG_TYPE_DDR_CLK, &Freq);
        }
    }

    if (Rval == DSP_ERR_NONE) {
        Rval = AmbaSYS_GetClkFreq(AMBA_SYS_CLK_DSP_SYS, &Freq);
        if (Rval != DSP_ERR_NONE) {
            dsp_wrapper_osal_printU5("[ERR] AmbaDSP_MainInit() : AmbaSYS_GetClkFreq[AUD] fail 0x%X", Rval, 0U, 0U, 0U, 0U);
        } else {
            Rval = dsp_sys_drv_cfg(DSP_SYS_DRV_CFG_TYPE_AUD_CLK, &Freq);
        }
    }

    if (Rval == DSP_ERR_NONE) {
        Rval = dsp_main_init(pDspSysConfig);
    }

    return Rval;
}

UINT32 AmbaDSP_MainGetDefaultSysCfg(AMBA_DSP_SYS_CONFIG_s *pDspSysConfig)
{
    return dsp_main_get_defsyscfg(pDspSysConfig);
}

UINT32 AmbaDSP_MainSuspend(void)
{
    return dsp_main_suspend();
}

UINT32 AmbaDSP_MainResume(void)
{
    return dsp_main_resume();
}

UINT32 AmbaDSP_MainSetWorkArea(ULONG WorkAreaAddr, UINT32 WorkSize)
{
    return dsp_main_set_work_area(WorkAreaAddr, WorkSize);
}

UINT32 AmbaDSP_MainGetDspVerInfo(AMBA_DSP_VERSION_INFO_s *Info)
{
    return dsp_main_get_dsp_ver_info(Info);
}

UINT32 AmbaDSP_MainMsgParseEntry(UINT32 EntryArg)
{
    return dsp_main_msg_parse_entry(EntryArg);
}

UINT32 AmbaDSP_MainWaitVinInterrupt(UINT32 Flag, UINT32 *ActualFlag, UINT32 TimeOut)
{
    return dsp_main_wait_vin_interrupt(Flag, ActualFlag, TimeOut);
}

UINT32 AmbaDSP_MainWaitVoutInterrupt(UINT32 Flag, UINT32 *ActualFlag, UINT32 TimeOut)
{
    return dsp_main_wait_vout_interrupt(Flag, ActualFlag, TimeOut);
}

UINT32 AmbaDSP_MainWaitFlag(UINT32 Flag, UINT32 *ActualFlag, UINT32 TimeOut)
{
    return dsp_main_wait_flag(Flag, ActualFlag, TimeOut);
}

UINT32 AmbaDSP_ResourceLimitConfig(const AMBA_DSP_RESOURCE_s *pResource)
{
#if defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
    UINT32 Rval = DSP_ERR_NONE;
    UINT16 i;

    if (pResource != NULL) {
        for (i = 0U; i < pResource->LiveviewResource.MaxViewZoneNum; i++) {
            ViewZoneMaxHorWarpComp[i] = pResource->LiveviewResourceEx.MaxHorWarpCompensation[i];
        }
        Rval = dsp_resource_limit_config(pResource);
    } else {
        Rval = DSP_ERR_0000;
    }
    return Rval;

#else
    return dsp_resource_limit_config(pResource);
#endif
}

UINT32 AmbaDSP_CalHierBufferSize(const UINT16 *pMaxMainWidth, const UINT16 *pMaxMainHeight,
                                 const UINT16 *pMaxHierWidth, const UINT16 *pMaxHierHeight,
                                 const UINT16 OctaveMode,
                                 UINT32 *pHierBufSize, UINT16 *pHierBufWidth, UINT16 *pHierBufHeight)
{
    return dsp_cal_hier_buffer_size(pMaxMainWidth, pMaxMainHeight,
                                    pMaxHierWidth, pMaxHierHeight,
                                    OctaveMode,
                                    pHierBufSize, pHierBufWidth, pHierBufHeight);
}

UINT32 AmbaDSP_GetCmprRawBufInfo(UINT16 Width, UINT16 CmprRate, UINT16 *pRawWidth, UINT16 *pRawPitch)
{
    return dsp_get_cmpr_raw_buf_info(Width, CmprRate, pRawWidth, pRawPitch);
}

UINT32 AmbaDSP_ParLoadConfig(UINT32 Enable, UINT32 Data)
{
    return dsp_parload_config(Enable, Data);
}

UINT32 AmbaDSP_ParLoadRegionUnlock(UINT16 RegionIdx, UINT16 SubRegionIdx)
{
    return dsp_parload_region_unlock(RegionIdx, SubRegionIdx);
}

UINT32 AmbaDSP_CalcEncMvBufInfo(UINT16 Width, UINT16 Height, UINT32 Option, UINT32 *pBufSize)
{
    return dsp_calc_enc_mv_buf_info(Width, Height, Option, pBufSize);
}

UINT32 AmbaDSP_CalVpMsgBufferSize(const UINT32 *NumMsgs, UINT32 *MsgSize)
{
    return dsp_calc_vp_msg_buf_size(NumMsgs, MsgSize);
}

UINT32 AmbaDSP_VirtToPhys(ULONG VirtAddr, UINT32 *pPhysAddr)
{
    UINT32 Rval = DSP_ERR_NONE;
    UINT32 U32VirtAddr;

    if (pPhysAddr == NULL) {
        dsp_wrapper_osal_printU5("[ERR] AmbaDSP_VirtToPhys: Null addr", 0U, 0U, 0U, 0U, 0U);
        Rval = DSP_ERR_0000;
    } else {
        /* TBD, Phys2Client */

        U32VirtAddr = (UINT32)VirtAddr;
        *pPhysAddr = U32VirtAddr;
    }

    return Rval;
}

#ifndef DSP_ATT_CLIENT_ID
#if defined (CONFIG_SOC_CV22) || defined (CONFIG_SOC_CV2) || defined (CONFIG_SOC_CV25) || defined (CONFIG_SOC_CV28) || defined (CONFIG_SOC_H32) || defined (CONFIG_SOC_CV2FS) || defined (CONFIG_SOC_CV22FS)
#define DSP_ATT_CLIENT_ID   (AMBA_DRAM_CLIENT_ORCCODE)
#else
#define DSP_ATT_CLIENT_ID   (AMBA_DRAM_CLIENT_ORCCODE0)
#endif
#endif
UINT32 AmbaDSP_VirtToCli(ULONG VirtAddr, UINT32 *pCliAddr)
{
    UINT32 Rval = DSP_ERR_NONE;
    ULONG PhysAddr;
    ULONG CliAddr;

    if (pCliAddr == NULL) {
        dsp_wrapper_osal_printU5("[ERR] AmbaDSP_VirtToCli: Null addr", 0U, 0U, 0U, 0U, 0U);
        Rval = DSP_ERR_0000;
    } else {
        PhysAddr = VirtAddr;

        (void)AmbaMemProt_QueryIntmdAddr(PhysAddr, DSP_ATT_CLIENT_ID, &CliAddr);
        *pCliAddr = (UINT32)CliAddr;
    }

    return Rval;
}

UINT32 AmbaDSP_CliToVirt(UINT32 CliAddr, ULONG *pVirtAddr)
{
    UINT32 Rval = DSP_ERR_NONE;
    ULONG U64CliAddr;
    ULONG U64PhysAddr;

    if (pVirtAddr == NULL) {
        dsp_wrapper_osal_printU5("[ERR] AmbaDSP_CliToVirt: Null addr", 0U, 0U, 0U, 0U, 0U);
        Rval = DSP_ERR_0000;
    } else {
        U64CliAddr = (ULONG)CliAddr;

        (void)AmbaMemProt_QueryPhysAddr(U64CliAddr, DSP_ATT_CLIENT_ID, &U64PhysAddr);
        *pVirtAddr = U64PhysAddr;
    }

    return Rval;
}

#if defined (CONFIG_DEVICE_TREE_SUPPORT)
UINT32 AmbaDSP_MainGetBufInfo(UINT32 Type, AMBA_DSP_BUF_INFO_s *pBufInfo)
{
    return dsp_main_get_buf_info(Type, pBufInfo);
}
#endif

#if defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
UINT32 AmbaDSP_SliceCfgCalc(UINT16 Id,
                            const AMBA_DSP_WINDOW_DIMENSION_s *pIn,
                            const AMBA_DSP_WINDOW_DIMENSION_s *pOut,
                            AMBA_DSP_SLICE_CFG_s *pSliceCfg)
{
    return dsp_slice_cfg_calc(Id, pIn, pOut, pSliceCfg);
}
#endif

UINT32 AmbaDSP_ParseMvBuf(UINT16 Width, UINT16 Height, UINT16 BufType, UINT32 Option, ULONG MvBufAddr);
UINT32 AmbaDSP_ParseMvBuf(UINT16 Width, UINT16 Height, UINT16 BufType, UINT32 Option, ULONG MvBufAddr)
{
    return dsp_parse_mv_buf(Width, Height, BufType, Option, MvBufAddr);
}

#if defined (CONFIG_SOC_CV2FS) || defined (CONFIG_SOC_CV22FS)
UINT32 AmbaDSP_TestFrameConfig(UINT32 StageId, const AMBA_DSP_TEST_FRAME_CFG_s *pCfg)
{
    return dsp_test_frame_cfg(StageId, pCfg);
}

UINT32 AmbaDSP_TestFrameCtrl(UINT32 NumStage, const AMBA_DSP_TEST_FRAME_CTRL_s *pCtrl)
{
    return dsp_test_frame_ctrl(NumStage, pCtrl);
}

UINT32 AmbaDSP_MainSafetyChk(const UINT32 ID, const UINT32 SubID, UINT32 *pStatus)
{
    return dsp_main_safety_chk(ID, SubID, pStatus);
}

UINT32 AmbaDSP_MainSafetyCfg(const UINT32 ID, const UINT32 Val0, const UINT32 Val1)
{
    return dsp_main_safety_cfg(ID, Val0, Val1);
}
#endif

UINT32 AmbaDSP_MainSetProtectArea(UINT32 Type, ULONG AreaAddr, UINT32 Size, UINT32 IsCached)
{
    UINT32 Rval = OK;
(void)Type;
(void)AreaAddr;
(void)Size;
(void)IsCached;

    return Rval;
}

UINT32 AmbaDSP_MainApiDumpInit(UINT8 Enable, UINT32 Mask);
UINT32 AmbaDSP_MainApiDumpInit(UINT8 Enable, UINT32 Mask)
{
    UINT32 Rval = OK;

    dsp_main_api_dump_init(Enable, Mask);

    return Rval;
}

UINT32 AmbaDSP_MainApiCheckInit(UINT8 Disable, UINT32 Mask);
UINT32 AmbaDSP_MainApiCheckInit(UINT8 Disable, UINT32 Mask)
{
    UINT32 Rval = OK;

    dsp_main_api_check_init(Disable, Mask);

    return Rval;
}

void AmbaDSP_CmdShow(UINT32 CmdCode, UINT8 On);
void AmbaDSP_CmdShow(UINT32 CmdCode, UINT8 On)
{
    dsp_cmd_show(CmdCode, On);
}

void AmbaDSP_CmdShowAll(UINT8 On);
void AmbaDSP_CmdShowAll(UINT8 On)
{
    dsp_cmd_show_all(On);
}

void AmbaDSP_MsgShow(UINT32 MsgCode, UINT8 On);
void AmbaDSP_MsgShow(UINT32 MsgCode, UINT8 On)
{
    dsp_msg_show(MsgCode, On);
}

void AmbaDSP_MsgShowAll(UINT8 On);
void AmbaDSP_MsgShowAll(UINT8 On)
{
    dsp_msg_show_all(On);
}

UINT32 AmbaDSP_SetDebugThread(UINT32 ThreadValid, UINT32 ThreadMask, UINT8 CmdType);
UINT32 AmbaDSP_SetDebugThread(UINT32 ThreadValid, UINT32 ThreadMask, UINT8 CmdType)
{
    return dsp_set_debug_thread(ThreadValid, ThreadMask, CmdType);
}

UINT32 AmbaDSP_SetDebugLevel(UINT32 Module, UINT32 Level, UINT8 CmdType);
UINT32 AmbaDSP_SetDebugLevel(UINT32 Module, UINT32 Level, UINT8 CmdType)
{
    return dsp_set_debug_level(Module, Level, CmdType);
}

void AmbaDSP_LogShow(UINT32 Line);
void AmbaDSP_LogShow(UINT32 Line)
{
    AmbaLL_LogShow(Line);
}

void AmbaDSP_ResetDSP(void);
void AmbaDSP_ResetDSP(void)
{
    dsp_reset_dsp();
}

UINT32 AmbaDSP_Stop(UINT32 CmdType);
UINT32 AmbaDSP_Stop(UINT32 CmdType)
{
    return dsp_stop(CmdType);
}

void AmbaDSP_SetSystemCfg(UINT8 ParIdx, UINT32 Val);
void AmbaDSP_SetSystemCfg(UINT8 ParIdx, UINT32 Val)
{
    dsp_set_system_config(ParIdx, Val);
}

UINT32 AmbaDSP_GetStatus(UINT32 Type, UINT32 Id, UINT32 *pData0, UINT32 *pData1)
{
    UINT32 Rval;
    dsp_status_t DspStatus = {0};

    DspStatus.Type = Type;
    DspStatus.Id = Id;

    Rval = dsp_get_status(&DspStatus);
    if (Rval == DSP_ERR_NONE) {
        *pData0 = DspStatus.Data[0U];
        *pData1 = DspStatus.Data[1U];
    }

    return Rval;
}

UINT32 AmbaDSP_EventHandlerCtrlConfig(UINT16 EventID, UINT16 MaxNumHandlers, AMBA_DSP_EVENT_HANDLER_f *pEventHandlers)
{
    return dsp_event_hdlr_ctrl_cfg(EventID, MaxNumHandlers, pEventHandlers);
}

UINT32 AmbaDSP_EventHandlerCtrlReset(UINT16 EventID)
{
    return dsp_event_hdlr_ctrl_reset(EventID);
}

UINT32 AmbaDSP_EventHandlerRegister(UINT16 EventID, AMBA_DSP_EVENT_HANDLER_f EventHandler)
{
    return dsp_event_hdlr_register(EventID, EventHandler);
}

UINT32 AmbaDSP_EventHandlerUnRegister(UINT16 EventID, AMBA_DSP_EVENT_HANDLER_f EventHandler)
{
    return dsp_event_hdlr_unregister(EventID, EventHandler);
}


UINT32 AmbaDSP_VoutReset(const UINT8 VoutIdx)
{
    return dsp_vout_reset(VoutIdx);
}

UINT32 AmbaDSP_VoutMixerConfig(const UINT8 VoutIdx, const AMBA_DSP_VOUT_MIXER_CONFIG_s *pConfig)
{
    return dsp_vout_mixer_cfg(VoutIdx, pConfig);
}

UINT32 AmbaDSP_VoutMixerConfigBackColor(const UINT8 VoutIdx, UINT32 BackColorYUV)
{
    return dsp_vout_mixer_cfg_bgcolor(VoutIdx, BackColorYUV);
}

UINT32 AmbaDSP_VoutMixerConfigHighlightColor(const UINT8 VoutIdx, UINT8 LumaThreshold, UINT32 HighlightColorYUV)
{
    return dsp_vout_mixer_cfg_highcolor(VoutIdx, LumaThreshold, HighlightColorYUV);
}

UINT32 AmbaDSP_VoutMixerConfigCsc(const UINT8 VoutIdx, const UINT8 CscCtrl)
{
    return dsp_vout_mixer_cfg_csc(VoutIdx, CscCtrl);
}

UINT32 AmbaDSP_VoutMixerCtrl(const UINT8 VoutIdx)
{
    return dsp_vout_mixer_ctrl(VoutIdx);
}

UINT32 AmbaDSP_VoutOsdConfigBuf(const UINT8 VoutIdx, const AMBA_DSP_VOUT_OSD_BUF_CONFIG_s *pBufConfig)
{
    return dsp_vout_osd_cfg_buf(VoutIdx, pBufConfig);
}

UINT32 AmbaDSP_VoutOsdCtrl(const UINT8 VoutIdx, UINT8 Enable, UINT8 SyncWithVin, UINT64 *pAttachedRawSeq)
{
    return dsp_vout_osd_ctrl(VoutIdx, Enable, SyncWithVin, pAttachedRawSeq);
}

UINT32 AmbaDSP_VoutDisplayConfig(const UINT8 VoutIdx, const AMBA_DSP_DISPLAY_CONFIG_s* pConfig)
{
    UINT32 Rval;
    ULONG  CfgAddr = 0U;
    UINT32 CfgSize = 0U;
    UINT8  DispCfg[512U/*508U*/];
    UINT32 MixerCsc[9U];
    const void *pVoid;

    /* Display driver config */
    if (VoutIdx == VOUT_IDX_A) {
        Rval = AmbaVout_GetDisp0ConfigAddr(&CfgAddr, &CfgSize);
    } else {
#if defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
        if ((VoutIdx == VOUT_IDX_B) &&
            (VoutDisplayOption[VoutIdx] == 1U)) {
            Rval = AmbaVout_GetDisp1ConfigAddr(&CfgAddr, &CfgSize);
        } else {
            Rval = AmbaVout_GetDisp2ConfigAddr(&CfgAddr, &CfgSize);
        }
#else
        Rval = AmbaVout_GetDisp1ConfigAddr(&CfgAddr, &CfgSize);
#endif
    }
    if (Rval != DSP_ERR_NONE) {
        dsp_wrapper_osal_printU5("Disp[%d] Config is NULL", VoutIdx, 0U, 0U, 0U, 0U);
    } else {
        (void)dsp_wrapper_osal_memset(&DispCfg[0U], 0, sizeof(DispCfg));

        dsp_wrapper_osal_typecast(&pVoid, &CfgAddr);
        (void)dsp_wrapper_osal_memcpy(&DispCfg[0U], pVoid, sizeof(DispCfg));

        Rval = dsp_vout_drv_cfg(VoutIdx, DSP_VOUT_DRV_CFG_TYPE_DISP, &DispCfg[0U]);
    }

    if (Rval != DSP_ERR_NONE) {
        if (VoutIdx == VOUT_IDX_A) {
            Rval = AmbaVout_GetDisp0DigiCscAddr(&CfgAddr, &CfgSize);
        } else {
#if defined (CONFIG_SOC_CV2FS) || defined (CONFIG_SOC_CV22FS)
            Rval = AmbaVout_GetDisp1DigiCscAddr(&CfgAddr, &CfgSize);
#elif defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
            if ((VoutIdx == VOUT_IDX_B) &&
                (VoutDisplayOption[VoutIdx] == 1U)) {
                Rval = AmbaVout_GetDisp1DigiCscAddr(&CfgAddr, &CfgSize);
            } else {
                Rval = AmbaVout_GetDisp2HdmiCscAddr(&CfgAddr, &CfgSize);
            }
#else
            Rval = AmbaVout_GetDisp1HdmiCscAddr(&CfgAddr, &CfgSize);
#endif
        }

        if (Rval != DSP_ERR_NONE) {
            dsp_wrapper_osal_printU5("Csc[%d] Config is NULL", VoutIdx, 0U, 0U, 0U, 0U);
        } else {
            (void)dsp_wrapper_osal_memset(&MixerCsc[0U], 0, sizeof(MixerCsc));

            dsp_wrapper_osal_typecast(&pVoid, &CfgAddr);
            (void)dsp_wrapper_osal_memcpy(&MixerCsc[0U], pVoid, sizeof(MixerCsc));
            Rval = dsp_vout_drv_cfg(VoutIdx, DSP_VOUT_DRV_CFG_TYPE_MIXER_CSC, &MixerCsc[0U]);
            if (Rval != DSP_ERR_NONE) {
                dsp_wrapper_osal_printU5("Disp[%d] DSP_VOUT_DRV_CFG_TYPE_MIXER_CSC Error %d", VoutIdx, Rval, 0U, 0U, 0U);
            }
        }
    }

    Rval = dsp_vout_display_cfg(VoutIdx, pConfig);

    return Rval;
}

UINT32 AmbaDSP_VoutDisplayCtrl(const UINT8 VoutIdx)
{
    return dsp_vout_display_ctrl(VoutIdx);
}

UINT32 AmbaDSP_VoutDisplayConfigGamma(const UINT8 VoutIdx, ULONG TableAddr)
{
    return dsp_vout_display_cfg_gamma(VoutIdx, TableAddr);
}

UINT32 AmbaDSP_VoutDisplayControlGamma(const UINT8 VoutIdx, UINT8 Enable)
{
    return dsp_vout_display_ctrl_gamma(VoutIdx, Enable);
}

UINT32 AmbaDSP_VoutVideoConfig(const UINT8 VoutIdx, const AMBA_DSP_VOUT_VIDEO_CFG_s *pConfig)
{
    return dsp_vout_video_cfg(VoutIdx, pConfig);
}

UINT32 AmbaDSP_VoutVideoCtrl(const UINT8 VoutIdx, UINT8 Enable, UINT8 SyncWithVin, UINT64 *pAttachedRawSeq)
{
    return dsp_vout_video_ctrl(VoutIdx, Enable, SyncWithVin, pAttachedRawSeq);
}

UINT32 AmbaDSP_VoutConfigMixerBinding(UINT8 NumVout, const UINT8 *pVoutIdx, const AMBA_DSP_VOUT_DATA_PATH_CFG_s *pConfig)
{
    UINT32 Rval = DSP_ERR_NONE;

    if (NumVout > 0U) {
#if defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
        UINT16 i;
        for (i = 0U; i < NumVout; i++) {
            if ((pVoutIdx != NULL) && (pConfig != NULL)) {
                VoutDisplayOption[pVoutIdx[i]] = pConfig[i].DisplayOption;
            }
        }
#endif
        Rval = dsp_vout_cfg_mixer_binding(NumVout, pVoutIdx, pConfig);
    }
    return Rval;
}

static inline INT32 Float2Int32(FLOAT WorkFLOAT)
{
    INT32 WorkINT;

    if (WorkFLOAT >= 0.0F) {
        WorkINT = (INT32)(WorkFLOAT);
    } else {
        WorkINT = 0 - (INT32)(-WorkFLOAT);
    }
    return WorkINT;
}

UINT32 AmbaDSP_VoutMixerCscMatrixConfig(const UINT8 VoutIdx, const AMBA_DSP_VOUT_CSC_MATRIX_s *pCscMatrix)
{
    dsp_vout_csc_matrix_s VoutCscMatrix = {0};

    VoutCscMatrix.Coef[0U][0U] = Float2Int32(pCscMatrix->Coef[0U][0U] * 1024.0F);
    VoutCscMatrix.Coef[0U][1U] = Float2Int32(pCscMatrix->Coef[0U][1U] * 1024.0F);
    VoutCscMatrix.Coef[0U][2U] = Float2Int32(pCscMatrix->Coef[0U][2U] * 1024.0F);
    VoutCscMatrix.Coef[1U][0U] = Float2Int32(pCscMatrix->Coef[1U][0U] * 1024.0F);
    VoutCscMatrix.Coef[1U][1U] = Float2Int32(pCscMatrix->Coef[1U][1U] * 1024.0F);
    VoutCscMatrix.Coef[1U][2U] = Float2Int32(pCscMatrix->Coef[1U][2U] * 1024.0F);
    VoutCscMatrix.Coef[2U][0U] = Float2Int32(pCscMatrix->Coef[2U][0U] * 1024.0F);
    VoutCscMatrix.Coef[2U][1U] = Float2Int32(pCscMatrix->Coef[2U][1U] * 1024.0F);
    VoutCscMatrix.Coef[2U][2U] = Float2Int32(pCscMatrix->Coef[2U][2U] * 1024.0F);
    VoutCscMatrix.Offset[0U]   = Float2Int32(pCscMatrix->Offset[0U]);
    VoutCscMatrix.Offset[1U]   = Float2Int32(pCscMatrix->Offset[1U]);
    VoutCscMatrix.Offset[2U]   = Float2Int32(pCscMatrix->Offset[2U]);
    VoutCscMatrix.MinVal[0U]   = pCscMatrix->MinVal[0U];
    VoutCscMatrix.MaxVal[0U]   = pCscMatrix->MaxVal[0U];
    VoutCscMatrix.MinVal[1U]   = pCscMatrix->MinVal[1U];
    VoutCscMatrix.MaxVal[1U]   = pCscMatrix->MaxVal[1U];
    VoutCscMatrix.MinVal[2U]   = pCscMatrix->MinVal[2U];
    VoutCscMatrix.MaxVal[2U]   = pCscMatrix->MaxVal[2U];
    return dsp_vout_mixer_csc_matrix_cfg(VoutIdx, &VoutCscMatrix);
}

UINT32 AmbaDSP_VoutDveConfig(const UINT8 VoutIdx, UINT8 DveMode)
{
    UINT32 Rval = DSP_ERR_NONE;
#if !defined (CONFIG_SOC_CV2FS) && !defined (CONFIG_SOC_CV22FS)

/* DSP TV system */

    ULONG TveCfgAddr = 0U;
    UINT32 TveCfgSize = 0U;
    UINT32 Tve[128U];
    const void *pVoid;

#if defined (CONFIG_SOC_CV28)
    if (VoutIdx == VOUT_IDX_A) {
        Rval = AmbaVout_GetTvEncAddr(&TveCfgAddr, &TveCfgSize);
        if (Rval != DSP_ERR_NONE) {
            dsp_wrapper_osal_printU5("Tve[%d] Config is NULL", VoutIdx, 0U, 0U, 0U, 0U);
            Rval = DSP_ERR_0001;
        } else {
            dsp_wrapper_osal_printU5("Tve[%d] Drv-Addr [0x%X]", VoutIdx, TveCfgAddr, 0U, 0U, 0U);
        }
    } else {
        dsp_wrapper_osal_printU5("Tve[%d] Config is N/A", VoutIdx, 0U, 0U, 0U, 0U);
        Rval = DSP_ERR_0001;
    }
#else
    if (VoutIdx == VOUT_IDX_B) {
#if defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
        Rval = AmbaVout_GetTvEncAddr(&TveCfgAddr, &TveCfgSize);
#else
        Rval = AmbaVout_GetTvEncAddr(&TveCfgAddr, &TveCfgSize);
#endif
        if (Rval != DSP_ERR_NONE) {
            dsp_wrapper_osal_printU5("Tve[%d] Config is NULL", VoutIdx, 0U, 0U, 0U, 0U);
            Rval = DSP_ERR_0001;
        }
    } else {
        dsp_wrapper_osal_printU5("Tve[%d] Config is N/A", VoutIdx, 0U, 0U, 0U, 0U);
        Rval = DSP_ERR_0001;
    }
#endif
    if (Rval == DSP_ERR_NONE) {
        (void)dsp_wrapper_osal_memset(&Tve[0U], 0, sizeof(Tve));

        dsp_wrapper_osal_typecast(&pVoid, &TveCfgAddr);
        (void)dsp_wrapper_osal_memcpy(&Tve[0U], pVoid, sizeof(Tve));
        Rval = dsp_vout_drv_cfg(VoutIdx, DSP_VOUT_DRV_CFG_TYPE_TVE, &Tve[0U]);
        if (Rval != DSP_ERR_NONE) {
            dsp_wrapper_osal_printU5("Tve[%d] DSP_VOUT_DRV_CFG_TYPE_TVE Error", VoutIdx, 0U, 0U, 0U, 0U);
            Rval = DSP_ERR_0001;
        }
    }

    if (Rval != DSP_ERR_NONE) {
        dsp_wrapper_osal_printU5("Tve[%d] Config pre Error %d", VoutIdx, Rval, 0U, 0U, 0U);
        Rval = DSP_ERR_0001;
    } else {
        Rval = dsp_vout_dve_cfg(VoutIdx, DveMode);
    }
#else
    Rval = dsp_vout_dve_cfg(VoutIdx, DveMode);
#endif
    return Rval;
}

UINT32 AmbaDSP_VoutDveCtrl(const UINT8 VoutIdx)
{
    return dsp_vout_dve_ctrl(VoutIdx);
}

UINT32 AmbaDSP_LiveviewConfig(UINT16 NumViewZone, const AMBA_DSP_LIVEVIEW_VIEWZONE_CFG_s *pViewZoneCfg, UINT16 NumYUVStream, const AMBA_DSP_LIVEVIEW_STREAM_CFG_s *pYUVStrmCfg)
{
    UINT16 i;
    UINT32 Rval = DSP_ERR_NONE;

    if ((pViewZoneCfg != NULL) && (pYUVStrmCfg != NULL)) {
        for (i = 0U; i < NumViewZone; i++) {
            if (pViewZoneCfg[i].VinSource != DSP_LV_SOURCE_MEM_RECON) {
                ViewZoneIkCtxId[pViewZoneCfg[i].ViewZoneId] = pViewZoneCfg[i].ViewZoneId;
            } else {
                ViewZoneIkCtxId[pViewZoneCfg[i].ViewZoneId] = DSP_VZ_IK_IDX_INVALID;
            }

            ViewZoneRotate[pViewZoneCfg[i].ViewZoneId] = pViewZoneCfg[i].RotateFlip;

            ViewZonePipe[pViewZoneCfg[i].ViewZoneId] = pViewZoneCfg[i].DramUtilityPipe;

#if defined (CONFIG_SOC_CV2FS) || defined (CONFIG_SOC_CV22FS) || defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
            ViewZoneSource[pViewZoneCfg[i].ViewZoneId] = pViewZoneCfg[i].VinSource;
#endif
        }
        Rval = dsp_liveview_cfg(NumViewZone, pViewZoneCfg, NumYUVStream, pYUVStrmCfg);
    } else {
        Rval = DSP_ERR_0000;
    }

    return Rval;
}

#ifndef IK_HORIZONTAL_FLIP
    #define IK_HORIZONTAL_FLIP          (0x1U)
#endif
#ifndef IK_VERTICAL_FLIP
    #define IK_VERTICAL_FLIP            (0x2U)
#endif
#ifndef IK_FLIP_RAW_V
    #define IK_FLIP_RAW_V               (IK_VERTICAL_FLIP)
#endif
#ifndef IK_FLIP_YUV_H
    #define IK_FLIP_YUV_H               (IK_HORIZONTAL_FLIP<<16U)
#endif
static inline void dsp_wrap_fill_ik_flip(UINT16 ViewZoneId, UINT32 *pFlipMode)
{
    if ((ViewZoneRotate[ViewZoneId] == AMBA_DSP_ROTATE_0_HORZ_FLIP) ||
        (ViewZoneRotate[ViewZoneId] == AMBA_DSP_ROTATE_180) ||
        (ViewZoneRotate[ViewZoneId] == AMBA_DSP_ROTATE_270) ||
        (ViewZoneRotate[ViewZoneId] == AMBA_DSP_ROTATE_270_VERT_FLIP)) {
        *pFlipMode |= (UINT32)IK_FLIP_YUV_H;
    }
    if ((ViewZoneRotate[ViewZoneId] == AMBA_DSP_ROTATE_90_VERT_FLIP) ||
        (ViewZoneRotate[ViewZoneId] == AMBA_DSP_ROTATE_180) ||
        (ViewZoneRotate[ViewZoneId] == AMBA_DSP_ROTATE_180_HORZ_FLIP) ||
        (ViewZoneRotate[ViewZoneId] == AMBA_DSP_ROTATE_270)) {
        *pFlipMode |= (UINT32)IK_FLIP_RAW_V;
    }
}

#if defined (CONFIG_SOC_CV2FS) || defined (CONFIG_SOC_CV22FS) || defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
static inline void dsp_wrap_fill_ik_stitch(UINT16 ViewZoneId, AMBA_IK_STITCH_INFO_s *pStitchInfo)
{
    if ((ViewZoneSliceCfg[ViewZoneId].SliceNumCol > 1U) ||
        (ViewZoneSliceCfg[ViewZoneId].SliceNumRow > 1U) ||
        (IsLvTileMode == 1U)) {
        pStitchInfo->Enable = 1U;
        pStitchInfo->TileNum_x = ViewZoneSliceCfg[ViewZoneId].SliceNumCol;
        pStitchInfo->TileNum_y = ViewZoneSliceCfg[ViewZoneId].SliceNumRow;
    } else {
        pStitchInfo->Enable = 0U;
        pStitchInfo->TileNum_x = 1U;
        pStitchInfo->TileNum_y = 1U;
    }
}
#endif

#if defined (CONFIG_SOC_CV2FS) || defined (CONFIG_SOC_CV22FS) || defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
static inline UINT8 dsp_wrap_lv_need_ik_stitch(const UINT8 Pipe,
                                               const UINT16 Source)
{
    UINT8 Rval;

    if ((Pipe != (UINT8)DSP_DRAM_PIPE_RAW_ONLY) &&
        (Source != (UINT8)DSP_LV_SOURCE_MEM_DEC) &&
        (Source != (UINT8)DSP_LV_SOURCE_MEM_RECON)) {
        Rval = (UINT8)1U;
    } else {
        Rval = (UINT8)0U;
    }

    return Rval;
}
#endif

#if defined (CONFIG_SOC_CV2FS) || defined (CONFIG_SOC_CV22FS) || defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
static inline UINT8 dsp_wrap_need_snsr_info(const AMBA_IK_ABILITY_s *pIkAbility)
{
    UINT8 Rval = (UINT8)0U;

    if (pIkAbility->VideoPipe != AMBA_IK_VIDEO_Y2Y) {
        Rval = (UINT8)1U;
    }
    return Rval;
}
#else
static inline UINT8 dsp_wrap_need_snsr_info(const UINT16 ViewZoneId,
                                            const AMBA_IK_ABILITY_s *pIkAbility)
{
    UINT8 Rval = (UINT8)0U;

    if ((ViewZonePipe[ViewZoneId] == (UINT8)DSP_DRAM_PIPE_MIPI_RAW) ||
        (ViewZonePipe[ViewZoneId] == (UINT8)DSP_DRAM_PIPE_RAW_PROC) ||
        ((pIkAbility->VideoPipe != AMBA_IK_VIDEO_Y2Y) &&
         (pIkAbility->VideoPipe != AMBA_IK_VIDEO_Y2Y_MIPI))) {
        Rval = (UINT8)1U;
    }
    return Rval;
}
#endif

static inline UINT32 dsp_wrap_lv_ctrl_pre_proc(const UINT16 ViewZoneId,
                                               const AMBA_IK_MODE_CFG_s *pImgMode,
                                               const AMBA_IK_ABILITY_s *pIkAbility)
{
    const void *pVoid;
    UINT32 Rval = DSP_ERR_NONE;
    UINT32 FlipMode;
    dsp_ik_drv_cfg_ability_t Ability;
    AMBA_IK_VIN_SENSOR_INFO_s VinSensorInfo;
    dsp_ik_drv_cfg_sensor_info_t SensorInfo;
#if defined (CONFIG_SOC_CV2FS) || defined (CONFIG_SOC_CV22FS) || defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
    AMBA_IK_STITCH_INFO_s StitchInfo;
#endif
    UINT32 ErrLine = 0U;

#if defined (CONFIG_SOC_CV2FS) || defined (CONFIG_SOC_CV22FS) || defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
    if ((UINT8)1 == dsp_wrap_need_snsr_info(pIkAbility)) {
#else
    if ((UINT8)1 == dsp_wrap_need_snsr_info(ViewZoneId, pIkAbility)) {
#endif
        (void)dsp_wrapper_osal_memset(&VinSensorInfo, 0, sizeof(AMBA_IK_VIN_SENSOR_INFO_s));
#ifdef SUPPORT_AMBA_IK
        Rval = AmbaIK_GetVinSensorInfo(pImgMode, &VinSensorInfo);
#endif
        if (Rval == OK) {
            (void)dsp_wrapper_osal_memset(&SensorInfo, 0, sizeof(dsp_ik_drv_cfg_sensor_info_t));
            dsp_wrapper_osal_typecast(&pVoid, &SensorInfo);
            SensorInfo.Pipe = DSP_IK_PIPE_VDO;
            SensorInfo.SensorMode = VinSensorInfo.SensorMode;
            SensorInfo.Compression = VinSensorInfo.Compression;
            Rval = dsp_liveview_ik_drv_cfg(ViewZoneId, DSP_IK_DRV_CFG_TYPE_SNSR_INFO, &SensorInfo);
        }
    }

    if ((Rval == DSP_ERR_NONE) &&
        (ViewZonePipe[ViewZoneId] != (UINT8)DSP_DRAM_PIPE_RAW_ONLY)) {
        FlipMode = 0U;
        dsp_wrap_fill_ik_flip(ViewZoneId, &FlipMode);
        if (FlipMode != 0U) {
#ifdef SUPPORT_AMBA_IK
            Rval = AmbaIK_SetFlipMode(pImgMode, FlipMode);
#endif
        }
    }

    if ((Rval == DSP_ERR_NONE) &&
        (ViewZonePipe[ViewZoneId] != (UINT8)DSP_DRAM_PIPE_RAW_ONLY)) {
        (void)dsp_wrapper_osal_memset(&Ability, 0, sizeof(dsp_ik_drv_cfg_ability_t));
        dsp_wrapper_osal_typecast(&pVoid, &Ability);
        Ability.Pipe = pIkAbility->Pipe;
        if (pIkAbility->Pipe == AMBA_IK_PIPE_STILL) {
#if !defined (CONFIG_SOC_CV2FS) && !defined (CONFIG_SOC_CV22FS)
            Ability.StlPipe = (pIkAbility->StillPipe == AMBA_IK_STILL_HISO)? DSP_IK_STL_PIPE_HI: DSP_IK_STL_PIPE_LI;
#else
            Ability.StlPipe = DSP_IK_STL_PIPE_LI;
#endif
        }
        Rval = dsp_liveview_ik_drv_cfg(ViewZoneId, DSP_IK_DRV_CFG_TYPE_ABILITY, &Ability);
    }

    /* Query stitch */
#if defined (CONFIG_SOC_CV2FS) || defined (CONFIG_SOC_CV22FS) || defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
    if ((Rval == DSP_ERR_NONE) &&
        ((UINT8)1U == dsp_wrap_lv_need_ik_stitch(ViewZonePipe[ViewZoneId], ViewZoneSource[ViewZoneId]))) {
        /* Query stitch */
        (void)dsp_wrapper_osal_memset(&StitchInfo, 0, sizeof(AMBA_IK_STITCH_INFO_s));
        dsp_wrap_fill_ik_stitch(ViewZoneId, &StitchInfo);
#ifdef SUPPORT_AMBA_IK
        Rval = AmbaIK_SetStitchingInfo(pImgMode, &StitchInfo);
#if defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
        if ((Rval == DSP_ERR_NONE) &&
            (pIkAbility->Pipe != AMBA_IK_PIPE_STILL)) {
            AMBA_IK_BURST_TILE_s BurstTile = {0};
            AMBA_IK_OVERLAP_X_s OverLap = {0};

            if ((StitchInfo.Enable == 1U) &&
                (StitchInfo.TileNum_x > 1U) &&
                (ViewZonePipe[ViewZoneId] != DSP_DRAM_PIPE_EFCY)) {
                BurstTile.Enable = 1U;
                OverLap.OverlapX = ViewZoneMaxHorWarpComp[ViewZoneId];
            } else {
                BurstTile.Enable = 0U;
                OverLap.OverlapX = 0U;
            }
            Rval = AmbaIK_SetBurstTile(pImgMode, &BurstTile);
            if (Rval == DSP_ERR_NONE) {
                Rval = AmbaIK_SetOverlapX(pImgMode, &OverLap);
            }

        }
#endif
#endif
    }
#endif

    if (Rval != OK) {
        dsp_wrapper_osal_printU5("VZ[%d] dsp_wrap_lv_ctrl_pre_proc Fail [0x%X][%d]", ViewZoneId, Rval, ErrLine, 0U, 0U);
    }

    return Rval;
}

static inline UINT32 dsp_wrap_lv_ctrl_post_exec(const UINT16 ViewZoneId,
                                               const AMBA_IK_MODE_CFG_s *pImgMode,
                                               const AMBA_IK_ABILITY_s *pIkAbility)
{
    ULONG IsoAddr;
    UINT16 i;
    UINT32 IsoId = 0U;
    UINT32 Rval = DSP_ERR_NONE;
    dsp_ik_drv_cfg_iso_t Iso;
    dsp_ik_drv_cfg_dol_offset_t DolOfst;
    AMBA_IK_EXECUTE_CONTAINER_s CfgExecInfo;
#if defined (CONFIG_SOC_CV2FS) || defined (CONFIG_SOC_CV22FS) || defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
#ifdef SUPPORT_IK_FRM_INFO_QUERY
    AMBA_IK_QUERY_FRAME_INFO_s FrmInfo = {0};
#endif
#else
    const void *pVoid;
    AMBA_IK_HDR_RAW_INFO_s HdrRawInfo = {0};
#endif
#if defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
    dsp_ik_drv_cfg_ext_raw_cmpr_info_t CfgExtRawCmpr;
#endif

(void)pIkAbility;

    /* Post-exe */
    if (ViewZonePipe[ViewZoneId] != (UINT8)DSP_DRAM_PIPE_RAW_ONLY) {
        (void)dsp_wrapper_osal_memset(&CfgExecInfo, 0, sizeof(AMBA_IK_EXECUTE_CONTAINER_s));
#ifdef SUPPORT_AMBA_IK
        Rval = AmbaIK_ExecuteConfig(pImgMode, &CfgExecInfo);
#endif
        if (Rval != OK) {
            dsp_wrapper_osal_printU5("!!! IK Exec[%d] Fail(0x%X)", pImgMode->ContextId, Rval, 0U, 0U, 0U);
            Rval = DSP_ERR_0001;
        } else {
            if (CfgExecInfo.pIsoCfg == NULL) {
                dsp_wrapper_osal_printU5("!!! IK Exec[%d] Null IsoCfg", pImgMode->ContextId, 0U, 0U, 0U, 0U);
                Rval = DSP_ERR_0001;
            } else {
                IsoId = CfgExecInfo.IkId;
                dsp_wrapper_osal_typecast(&IsoAddr, &CfgExecInfo.pIsoCfg);
                //dsp_wrapper_osal_printU5("!!! IK Exec[%d] IsoAddr V 0x%X id 0x%X", ViewZoneId, IsoAddr, IsoId, 0U, 0U);

                (void)dsp_wrapper_osal_memset(&Iso, 0, sizeof(dsp_ik_drv_cfg_iso_t));
                Iso.Id = IsoId;
                Iso.Addr = IsoAddr;
                Rval = dsp_liveview_ik_drv_cfg(ViewZoneId, DSP_IK_DRV_CFG_TYPE_ISO, &Iso);
                if (Rval != OK) {
                    dsp_wrapper_osal_printU5("VZ[%d] DSP_IK_DRV_CFG_TYPE_ISO Fail", pImgMode->ContextId, 0U, 0U, 0U, 0U);
                    Rval = DSP_ERR_0001;
                }
            }
        }
    }

    if ((Rval == DSP_ERR_NONE) &&
        (ViewZonePipe[ViewZoneId] != (UINT8)DSP_DRAM_PIPE_RAW_ONLY)) {

        (void)dsp_wrapper_osal_memset(&DolOfst, 0, sizeof(dsp_ik_drv_cfg_dol_offset_t));
        DolOfst.Pipe = DSP_IK_PIPE_VDO;

        if ((pIkAbility->VideoPipe == AMBA_IK_VIDEO_HDR_EXPO_2) ||
            (pIkAbility->VideoPipe == AMBA_IK_VIDEO_HDR_EXPO_3)) {
#if defined (CONFIG_SOC_CV2FS) || defined (CONFIG_SOC_CV22FS) || defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
#ifdef SUPPORT_IK_FRM_INFO_QUERY
            (void)dsp_wrapper_osal_memset(&FrmInfo, 0, sizeof(AMBA_IK_QUERY_FRAME_INFO_s));
#ifdef SUPPORT_AMBA_IK
            Rval = AmbaIK_QueryFrameInfo(pImgMode, IsoId, &FrmInfo);
#endif
            if (Rval != OK) {
                dsp_wrapper_osal_printU5("VZ[%d] AmbaIK_QueryFrameInfo Fail %x", pImgMode->ContextId, Rval, 0U, 0U, 0U);
                Rval = DSP_ERR_0001;
            }
#else
            dsp_wrapper_osal_printU5("VZ[%d] AmbaIK_QueryFrameInfo n/a", pImgMode->ContextId, 0U, 0U, 0U, 0U);
#endif
#else
            dsp_wrapper_osal_typecast(&pVoid, &IsoAddr);
#ifdef SUPPORT_AMBA_IK
            (void)AmbaIK_GetIsoHdrRawOffest(pVoid, &HdrRawInfo);
#endif
#endif

#if defined (CONFIG_SOC_CV2FS) || defined (CONFIG_SOC_CV22FS) || defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
#ifdef SUPPORT_IK_FRM_INFO_QUERY
            for (i = 0U; i < IK_MAX_HDR_EXPOSURE_NUM; i++) {
                DolOfst.Y[i] = FrmInfo.HdrRawInfo.YOffset[i];
            }
#else
            dsp_wrapper_osal_printU5("VZ[%d] Fixed DOL ofst[0]", pImgMode->ContextId, 0U, 0U, 0U, 0U);
            for (i = 0U; i < IK_MAX_HDR_EXPOSURE_NUM; i++) {
                DolOfst.Y[i] = 0U;
            }
#endif
#else
            for (i = 0U; i < MAX_IK_MAX_HDR_EXPOSURE_NUM; i++) {
                DolOfst.Y[i] = HdrRawInfo.YOffset[i];
            }
#endif
        } else {
            for (i = 0U; i < DSP_IK_DOL_OFST_NUM; i++) {
                DolOfst.Y[i] = 0U;
            }
        }

        (void)dsp_liveview_ik_drv_cfg(ViewZoneId, DSP_IK_DRV_CFG_TYPE_DOL_OFST, &DolOfst);

#if defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
        (void)dsp_wrapper_osal_memset(&CfgExtRawCmpr, 0, sizeof(dsp_ik_drv_cfg_ext_raw_cmpr_info_t));

#ifdef SUPPORT_AMBA_IK
        if ((Rval == OK) &&
            ((pIkAbility->VideoPipe != AMBA_IK_VIDEO_Y2Y) &&
             (pIkAbility->StillPipe != AMBA_IK_STILL_HISO))) {
            Rval = AmbaIK_GetExtRawOutMode(pImgMode, &CfgExtRawCmpr.Compression);
        } else {
            CfgExtRawCmpr.Compression = 0U;
        }
#endif
        if (Rval == OK) {
            CfgExtRawCmpr.Pipe = DSP_IK_PIPE_VDO;
            Rval = dsp_liveview_ik_drv_cfg(ViewZoneId, DSP_IK_DRV_CFG_TYPE_EXT_RAW_CMPR, &CfgExtRawCmpr);
        }
#endif
    }

    return Rval;
}

#if defined (CONFIG_SOC_CV2FS) || defined (CONFIG_SOC_CV22FS) || defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
static inline UINT32 dsp_wrap_lv_ctrl_vz_info_collect(UINT16 NumViewZone, const UINT16 *pViewZoneId, const UINT8 *pEnable)
{
    UINT16 i;
    UINT32 Rval = DSP_ERR_NONE;

    IsLvTileMode = 0U;
    for (i = 0U; i < NumViewZone; i++) {
        if (pEnable[i] == (UINT8)0U) {
            continue;
        }

        if (ViewZoneIkCtxId[pViewZoneId[i]] == DSP_VZ_IK_IDX_INVALID) {
            continue;
        }

        /* Query slice config */
        if ((Rval == DSP_ERR_NONE) &&
            ((ViewZonePipe[pViewZoneId[i]] != (UINT8)DSP_DRAM_PIPE_RAW_ONLY) &&
             (ViewZoneSource[pViewZoneId[i]] != (UINT8)DSP_LV_SOURCE_MEM_DEC) &&
             (ViewZoneSource[pViewZoneId[i]] != (UINT8)DSP_LV_SOURCE_MEM_RECON))) {
            (void)dsp_wrapper_osal_memset(&ViewZoneSliceCfg[pViewZoneId[i]], 0, sizeof(AMBA_DSP_LIVEVIEW_SLICE_CFG_s));

            Rval = dsp_liveview_slice_cfg_calc(pViewZoneId[i], &ViewZoneSliceCfg[pViewZoneId[i]]);
            if (Rval != OK) {
                dsp_wrapper_osal_printU5("Ctx[%d] slice_cfg_calc Fail[0x%X]", pViewZoneId[i], Rval, 0U, 0U, 0U);
            } else {
                //AmbaLL_LogUInt5("AmbaDSP_LiveviewCtrl %u vz:%u Rval:0x%X", __LINE__, pViewZoneId[i], Rval, 0, 0);
                //AmbaLL_LogUInt5("LvSliceCfg SliceNum:%u %u EncSyncOpt:%u",
                //        ViewZoneSliceCfg[pViewZoneId[i]].SliceNumCol, ViewZoneSliceCfg[pViewZoneId[i]].SliceNumRow, ViewZoneSliceCfg[pViewZoneId[i]].EncSyncOpt, 0, 0);
                //AmbaLL_LogUInt5("LvSliceCfg Warp:%u %u %u VinDragLine:%u",
                //        ViewZoneSliceCfg[pViewZoneId[i]].WarpLumaWaitLine, ViewZoneSliceCfg[pViewZoneId[i]].WarpChromaWaitLine, ViewZoneSliceCfg[pViewZoneId[i]].WarpOverLap, ViewZoneSliceCfg[pViewZoneId[i]].VinDragLine, 0);
            }

            if ((ViewZoneSliceCfg[pViewZoneId[i]].SliceNumCol > 1U) ||
                (ViewZoneSliceCfg[pViewZoneId[i]].SliceNumRow > 1U)) {
                IsLvTileMode = 1U;
            }
        }
    }

    return Rval;
}
#endif

UINT32 AmbaDSP_LiveviewCtrl(UINT16 NumViewZone, const UINT16 *pViewZoneId, const UINT8 *pEnable)
{
    UINT16 i;
    UINT32 Rval = DSP_ERR_NONE;
    AMBA_IK_MODE_CFG_s ImgMode;
    AMBA_IK_ABILITY_s IkAbility = {0};

    if ((pEnable != NULL) && (pViewZoneId != NULL)) {
    /* Collecxt All viewzone information */
#if defined (CONFIG_SOC_CV2FS) || defined (CONFIG_SOC_CV22FS) || defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
        Rval = dsp_wrap_lv_ctrl_vz_info_collect(NumViewZone, pViewZoneId, pEnable);
#endif
        for (i = 0U; i < NumViewZone; i++) {
            if (pEnable[i] == (UINT8)0U) {
                continue;
            }

            if (ViewZoneIkCtxId[pViewZoneId[i]] == DSP_VZ_IK_IDX_INVALID) {
                continue;
            }

            ImgMode.ContextId = ViewZoneIkCtxId[pViewZoneId[i]];
#ifdef SUPPORT_AMBA_IK
            Rval = AmbaIK_GetContextAbility(&ImgMode, &IkAbility);
#endif
            if (Rval != OK) {
                dsp_wrapper_osal_printU5("Ctx[%d] GetContextAbility Fail", ViewZoneIkCtxId[pViewZoneId[i]], 0U, 0U, 0U, 0U);
                Rval = DSP_ERR_0001;
            }

            if (Rval == DSP_ERR_NONE) {
                Rval = dsp_wrap_lv_ctrl_pre_proc(pViewZoneId[i], &ImgMode, &IkAbility);
            }

            if (Rval == DSP_ERR_NONE) {
                Rval = dsp_wrap_lv_ctrl_post_exec(pViewZoneId[i], &ImgMode, &IkAbility);
                if (Rval != DSP_ERR_NONE) {
                    dsp_wrapper_osal_printU5("Ctx[%d] dsp_wrap_lv_ctrl_post_exec Fail", ViewZoneIkCtxId[pViewZoneId[i]], 0U, 0U, 0U, 0U);
                    Rval = DSP_ERR_0001;
                }
            } else {
                dsp_wrapper_osal_printU5("Ctx[%d] dsp_wrap_lv_ctrl_pre_proc Fail 0x%X", ViewZoneIkCtxId[pViewZoneId[i]], Rval, 0U, 0U, 0U);
                Rval = DSP_ERR_0001;
            }
        }
        if (Rval != OK) {
            dsp_wrapper_osal_printU5("Ctx[%d] dsp_liveview_ctrl pre Error 0x%X", ViewZoneIkCtxId[pViewZoneId[i]], Rval, 0U, 0U, 0U);
        } else {
            Rval = dsp_liveview_ctrl(NumViewZone, pViewZoneId, pEnable);
        }
    } else {
        Rval = DSP_ERR_0000;
    }

    return Rval;
}

UINT32 AmbaDSP_LiveviewUpdateConfig(UINT16 NumYuvStream, const AMBA_DSP_LIVEVIEW_STREAM_CFG_s *pYuvStrmCfg, UINT64 *pAttachedRawSeq)
{
    return dsp_liveview_update_cfg(NumYuvStream, pYuvStrmCfg, pAttachedRawSeq);
}

UINT32 AmbaDSP_LiveviewUpdateIsoCfg(UINT16 NumViewZone, const AMBA_DSP_LIVEVIEW_ISOCFG_CTRL_s *pIsoCfgCtrl, UINT64 *pAttachedRawSeq)
{
    UINT32 Rval = DSP_ERR_NONE;
    UINT16 i, j;
    AMBA_IK_MODE_CFG_s ImgMode;
    AMBA_IK_ABILITY_s IkAbility = {0};
#if defined (CONFIG_SOC_CV2FS) || defined (CONFIG_SOC_CV22FS) || defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
#ifdef SUPPORT_IK_FRM_INFO_QUERY
    AMBA_IK_QUERY_FRAME_INFO_s FrmInfo = {0};
#endif
#else
    const void *pVoid;
    AMBA_IK_HDR_RAW_INFO_s HdrRawInfo = {0};
#endif
    dsp_ik_drv_cfg_dol_offset_t DolOfst;

    for (i = 0U; i < NumViewZone; i++) {
        if (pIsoCfgCtrl == NULL) {
            break;
        }
        if (ViewZonePipe[pIsoCfgCtrl[i].ViewZoneId] != (UINT8)DSP_DRAM_PIPE_RAW_ONLY) {
            if (pIsoCfgCtrl[i].CfgAddress > 0U) {
                ImgMode.ContextId = pIsoCfgCtrl[i].CtxIndex;
                (void)AmbaIK_GetContextAbility(&ImgMode, &IkAbility);
                (void)dsp_wrapper_osal_memset(&DolOfst, 0, sizeof(dsp_ik_drv_cfg_dol_offset_t));
                DolOfst.Pipe = DSP_IK_PIPE_VDO;

                if ((IkAbility.VideoPipe == AMBA_IK_VIDEO_HDR_EXPO_2) ||
                    (IkAbility.VideoPipe == AMBA_IK_VIDEO_HDR_EXPO_3)) {
#if defined (CONFIG_SOC_CV2FS) || defined (CONFIG_SOC_CV22FS) || defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
#ifdef SUPPORT_IK_FRM_INFO_QUERY
#ifdef SUPPORT_AMBA_IK
                    Rval = AmbaIK_QueryFrameInfo(&ImgMode, pIsoCfgCtrl[i].CfgIndex, &FrmInfo);
#endif
                    if (Rval != OK) {
                        dsp_wrapper_osal_printU5("VZ[%d] AmbaIK_QueryFrameInfo Fail %x", ImgMode.ContextId, Rval, 0U, 0U, 0U);
                        Rval = DSP_ERR_0001;
                    }
#else
                    dsp_wrapper_osal_printU5("VZ[%d] AmbaIK_QueryFrameInfo n/a", ImgMode.ContextId, 0U, 0U, 0U, 0U);
#endif
#else
                    dsp_wrapper_osal_typecast(&pVoid, &pIsoCfgCtrl[i].CfgAddress);
                    if (Rval == OK) {
#ifdef SUPPORT_AMBA_IK
                        Rval = AmbaIK_GetIsoHdrRawOffest(pVoid, &HdrRawInfo);
#endif
                    }
#endif
                    if (Rval != OK) {
                        dsp_wrapper_osal_printU5("Ctx[%d] GetIsoHdrRawOffest Fail", ViewZoneIkCtxId[pIsoCfgCtrl[i].ViewZoneId], 0U, 0U, 0U, 0U);
                    } else {
#if defined (CONFIG_SOC_CV2FS) || defined (CONFIG_SOC_CV22FS) || defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52) || defined (CONFIG_SOC_CV7)
#ifdef SUPPORT_IK_FRM_INFO_QUERY
                        for (j = 0U; j < IK_MAX_HDR_EXPOSURE_NUM; j++) {
                            DolOfst.Y[j] = FrmInfo.HdrRawInfo.YOffset[j];
                        }
#else
                        dsp_wrapper_osal_printU5("VZ[%d] Fixed DOL ofst[0]", ImgMode.ContextId, 0U, 0U, 0U, 0U);
                        for (j = 0U; j < IK_MAX_HDR_EXPOSURE_NUM; j++) {
                            DolOfst.Y[i] = 0U;
                        }
#endif
#else
                        for (j = 0U; j < MAX_IK_MAX_HDR_EXPOSURE_NUM; j++) {
                            DolOfst.Y[j] = HdrRawInfo.YOffset[j];
                        }
#endif
                    }
                } else {
                    for (j = 0U; j < DSP_IK_DOL_OFST_NUM; j++) {
                        DolOfst.Y[j] = 0U;
                    }
                }
                Rval = dsp_liveview_ik_drv_cfg(pIsoCfgCtrl[i].ViewZoneId, DSP_IK_DRV_CFG_TYPE_DOL_OFST, &DolOfst);
                if (Rval != OK) {
                    dsp_wrapper_osal_printU5("VZ[%d] DSP_IK_DRV_CFG_TYPE_DOL_OFST Fail", pIsoCfgCtrl[i].ViewZoneId, 0U, 0U, 0U, 0U);
                    Rval = DSP_ERR_0001;
                }
            }
        }
    }
    if ((pIsoCfgCtrl != NULL) && (pAttachedRawSeq != NULL)) {
        if (Rval != OK) {
            dsp_wrapper_osal_printU5("Ctx[%d] GetIsoHdrRawOffest Fail", ViewZoneIkCtxId[pIsoCfgCtrl[i].ViewZoneId], 0U, 0U, 0U, 0U);
        } else {
            Rval = dsp_liveview_update_isocfg(NumViewZone, pIsoCfgCtrl, pAttachedRawSeq);
        }
    } else {
        Rval = DSP_ERR_0000;
    }


    return Rval;
}

UINT32 AmbaDSP_LiveviewUpdateVinCfg(UINT16 VinId,
                                    UINT16 SubChNum,
                                    const AMBA_DSP_VIN_SUB_CHAN_s *pSubCh,
                                    const AMBA_DSP_LIVEVIEW_VINCFG_CTRL_s *pLvVinCfgCtrl,
                                    UINT64 *pAttachedRawSeq)
{
    return dsp_liveview_update_vincfg(VinId, SubChNum, pSubCh, pLvVinCfgCtrl, pAttachedRawSeq);
}

UINT32 AmbaDSP_LiveviewUpdatePymdCfg(UINT16 NumViewZone,
                                     const UINT16 *pViewZoneId,
                                     const AMBA_DSP_LIVEVIEW_PYRAMID_s *pPyramid,
                                     const AMBA_DSP_LIVEVIEW_YUV_BUF_s *pPyramidBuf,
                                     UINT64 *pAttachedRawSeq)
{
    return dsp_liveview_update_pymdcfg(NumViewZone, pViewZoneId, pPyramid, pPyramidBuf, pAttachedRawSeq);
}

UINT32 AmbaDSP_LiveviewConfigVinCapture(UINT16 VinId, UINT16 SubChNum, const AMBA_DSP_VIN_SUB_CHAN_CFG_s *pSubChCfg)
{
    UINT32 Rval = DSP_ERR_NONE;
    AMBA_VIN_WINDOW_s VinWin;
    AMBA_VIN_INFO_s VinInfo;
    ULONG VinCfgAddr = 0U;
    UINT32 VinCfgSize = 0U;
    const void *pVoid;
    UINT8 CfgData[128U];
    dsp_vin_drv_cfg_cfa_t CfgCfa;

    /* Only config when physical vin */
    if ((VinId & 0x8000U) == 0U) {
#if defined (CONFIG_SOC_CV2FS) || defined (CONFIG_SOC_CV22FS) || defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
        if (pSubChCfg[0U].Option != AMBA_DSP_VIN_CAP_OPT_EMBD)
#endif
        {
            VinWin.Width = pSubChCfg[0U].CaptureWindow.Width;
            VinWin.Height = pSubChCfg[0U].CaptureWindow.Height;
            VinWin.OffsetX = pSubChCfg[0U].CaptureWindow.OffsetX;
            VinWin.OffsetY = pSubChCfg[0U].CaptureWindow.OffsetY;
            Rval = AmbaVIN_CaptureConfig(VinId, &VinWin);

            /* Get VinCfg */
            if (Rval == DSP_ERR_NONE) {
                Rval = AmbaVIN_GetMainCfgBufInfo(VinId, &VinCfgAddr, &VinCfgSize);
                if (Rval != DSP_ERR_NONE) {
                    dsp_wrapper_osal_printU5("Vin[%d] Config is NULL", VinId, 0U, 0U, 0U, 0U);
                    Rval = DSP_ERR_0001;
                } else {
                    (void)dsp_wrapper_osal_memset(&CfgData[0U], 0, sizeof(CfgData));
                    dsp_wrapper_osal_typecast(&pVoid, &VinCfgAddr);
                    (void)dsp_wrapper_osal_memcpy(&CfgData[0U], pVoid, sizeof(CfgData));
                    Rval = dsp_liveview_vin_drv_cfg(VinId, DSP_VIN_DRV_CFG_TYPE_DATA, &CfgData[0U]);
                    if (Rval != OK) {
                        dsp_wrapper_osal_printU5("VinId[%d] DSP_VIN_DRV_CFG_TYPE_DATA Fail %x", VinId, Rval, 0U, 0U, 0U);
                        Rval = DSP_ERR_0001;
                    }
                }
            }

            /* Get VinSetting */
            if (Rval == DSP_ERR_NONE) {
                Rval = AmbaVIN_GetInfo(VinId, &VinInfo);
                if (Rval != DSP_ERR_NONE) {
                    dsp_wrapper_osal_printU5("Vin[%d] Info is NULL", VinId, 0U, 0U, 0U, 0U);
                    Rval = DSP_ERR_0001;
                } else {
                    (void)dsp_wrapper_osal_memset(&CfgCfa, 0, sizeof(dsp_vin_drv_cfg_cfa_t));
                    (void)dsp_wrapper_osal_memcpy(&CfgCfa.FrameRate, &VinInfo.FrameRate, sizeof(AMBA_DSP_FRAME_RATE_s));
                    CfgCfa.ColorSpace = VinInfo.ColorSpace;
                    CfgCfa.BayerPattern = VinInfo.BayerPattern;
                    CfgCfa.YuvOrder = VinInfo.YuvOrder;
                    CfgCfa.NumDataBits = VinInfo.NumDataBits;
                    CfgCfa.NumSkipFrame = VinInfo.NumSkipFrame;
                    Rval = dsp_liveview_vin_drv_cfg(VinId, DSP_VIN_DRV_CFG_TYPE_CFA, &CfgCfa);
                    if (Rval != OK) {
                        dsp_wrapper_osal_printU5("VinId[%d] DSP_VIN_DRV_CFG_TYPE_CFA Fail %x", VinId, Rval, 0U, 0U, 0U);
                        Rval = DSP_ERR_0001;
                    }
                }
            }
        }
    }

    if (Rval != OK) {
        dsp_wrapper_osal_printU5("Vin[%d] dsp_liveview_cfg_vin_cap pre Error %x", VinId, Rval, 0U, 0U, 0U);
        Rval = DSP_ERR_0001;
    } else {
        Rval = dsp_liveview_cfg_vin_cap(VinId, SubChNum, pSubChCfg);
        if (Rval != OK) {
            dsp_wrapper_osal_printU5("Vin[%d] Config is Fail %x", VinId, Rval, 0U, 0U, 0U);
            Rval = DSP_ERR_0001;
        }
    }

    return Rval;
}

UINT32 AmbaDSP_LiveviewConfigVinPost(const UINT8 Type, UINT16 VinId)
{
    return dsp_liveview_cfg_vin_post(Type, VinId);
}

UINT32 AmbaDSP_LiveviewFeedRawData(UINT16 NumViewZone, const UINT16 *pViewZoneId, const AMBA_DSP_LIVEVIEW_EXT_RAW_s *pExtBuf)
{
    return dsp_liveview_feed_raw_data(NumViewZone, pViewZoneId, pExtBuf);
}

UINT32 AmbaDSP_LiveviewFeedYuvData(UINT16 NumViewZone, const UINT16 *pViewZoneId, const AMBA_DSP_EXT_YUV_BUF_s *pExtYuvBuf)
{
    return dsp_liveview_feed_yuv_data(NumViewZone, pViewZoneId, pExtYuvBuf);
}

UINT32 AmbaDSP_LiveviewYuvStreamSync(UINT16 YuvStrmIdx, const AMBA_DSP_LIVEVIEW_SYNC_CTRL_s *pYuvStrmSyncCtrl, UINT32 *pSyncJobId, UINT64 *pAttachedRawSeq)
{
    return dsp_liveview_yuvstream_sync(YuvStrmIdx, pYuvStrmSyncCtrl, pSyncJobId, pAttachedRawSeq);
}

UINT32 AmbaDSP_LiveviewUpdateGeoCfg(UINT16 ViewZoneId, const AMBA_DSP_LIVEVIEW_GEOCFG_CTRL_s *pGeoCfgCtrl, UINT64 *pAttachedRawSeq)
{
    return dsp_liveview_update_geocfg(ViewZoneId, pGeoCfgCtrl, pAttachedRawSeq);
}

UINT32 AmbaDSP_LiveviewUpdateVZSource(UINT16 NumViewZone, const AMBA_DSP_LIVEVIEW_VZ_SRC_CFG_s *pVzSrcCfg, UINT64 *pAttachedRawSeq)
{
    return dsp_liveview_update_vz_src(NumViewZone, pVzSrcCfg, pAttachedRawSeq);
}

UINT32 AmbaDSP_LiveviewSideBandUpdate(UINT16 ViewZoneId, UINT8 NumBand, const ULONG *pSidebandBufAddr)
{
    return dsp_liveview_sb_update(ViewZoneId, NumBand, pSidebandBufAddr);
}

UINT32 AmbaDSP_LiveviewSliceCfg(UINT16 ViewZoneId, const AMBA_DSP_LIVEVIEW_SLICE_CFG_s *pLvSliceCfg)
{
    return dsp_liveview_slice_cfg(ViewZoneId, pLvSliceCfg);
}

UINT32 AmbaDSP_LiveviewSliceCfgCalc(UINT16 ViewZoneId, AMBA_DSP_LIVEVIEW_SLICE_CFG_s *pLvSliceCfg)
{
    return dsp_liveview_slice_cfg_calc(ViewZoneId, pLvSliceCfg);
}

UINT32 AmbaDSP_LiveviewVZPostponeCfg(UINT16 ViewZoneId, const AMBA_DSP_LV_VZ_POSTPONE_CFG_s *pVzPostPoneCfg)
{
    return dsp_liveview_vz_postpone_cfg(ViewZoneId, pVzPostPoneCfg);
}

UINT32 AmbaDSP_LiveviewUpdateVinState(UINT16 NumVin, const AMBA_DSP_LV_VIN_STATE_s *pVinState)
{
    return dsp_liveview_update_vin_state(NumVin, pVinState);
}

UINT32 AmbaDSP_LiveviewSlowShutterCtrl(UINT16 NumViewZone, const UINT16* pViewZoneId, const UINT32* pRatio)
{
    return dsp_liveview_slow_shutter_ctrl(NumViewZone, pViewZoneId, pRatio);
}

#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
UINT32 AmbaDSP_LiveviewDropRepeatCtrl(UINT16 NumViewZone, const UINT16* pViewZoneId, const AMBA_DSP_LIVEVIEW_DROP_REPEAT_s* pDropRptCfg)
{
    return dsp_liveview_drop_repeat_ctrl(NumViewZone, pViewZoneId, pDropRptCfg);
}
#endif

UINT32 AmbaDSP_LiveviewParseVpMsg(ULONG VpMsgAddr);
UINT32 AmbaDSP_LiveviewParseVpMsg(ULONG VpMsgAddr)
{
    return dsp_liveview_parse_vp_msg(VpMsgAddr);
}

UINT32 AmbaDSP_LiveviewGetIDspCfg(UINT16 ViewZoneId, ULONG *CfgAddr);
UINT32 AmbaDSP_LiveviewGetIDspCfg(UINT16 ViewZoneId, ULONG *CfgAddr)
{
    return dsp_liveview_get_idsp_cfg(ViewZoneId, CfgAddr);
}

UINT32 AmbaDSP_VideoEncConfig(UINT16 NumStream, const UINT16 *pStreamIdx, const AMBA_DSP_VIDEO_ENC_STRM_CONFIG_s *pStreamConfig)
{
    return dsp_video_enc_cfg(NumStream, pStreamIdx, pStreamConfig);
}

UINT32 AmbaDSP_VideoEncStart(UINT16 NumStream, const UINT16 *pStreamIdx, const AMBA_DSP_VIDEO_ENC_START_CONFIG_s *pStartConfig, UINT64 *pAttachedRawSeq)
{
    return dsp_video_enc_start(NumStream, pStreamIdx, pStartConfig, pAttachedRawSeq);
}

UINT32 AmbaDSP_VideoEncStop(UINT16 NumStream, const UINT16 *pStreamIdx, const UINT8 *pStopOption, UINT64 *pAttachedRawSeq)
{
    return dsp_video_enc_stop(NumStream, pStreamIdx, pStopOption, pAttachedRawSeq);
}

UINT32 AmbaDSP_VideoEncControlFrameRate(UINT16 NumStream, const UINT16 *pStreamIdx, const UINT32 *pDivisor, UINT64 *pAttachedRawSeq)
{
    return dsp_video_enc_ctrl_framerate(NumStream, pStreamIdx, pDivisor, pAttachedRawSeq);
}

UINT32 AmbaDSP_VideoEncControlRepeatDrop(UINT16 NumStream, const UINT16* pStreamIdx, const AMBA_DSP_VIDEO_REPEAT_DROP_CONFIG_s *pRepeatDropCfg, const UINT64* pAttachedRawSeq)
{
    return dsp_video_enc_ctrl_repeat_drop(NumStream, pStreamIdx, pRepeatDropCfg, pAttachedRawSeq);
}

UINT32 AmbaDSP_VideoEncControlSlowShutter(UINT16 NumVin, const UINT16 *pVinIdx, const UINT32 *pUpSamplingRate, const UINT64 *pAttachedRawSeq)
{
    return dsp_video_enc_ctrl_slowshutter(NumVin, pVinIdx, pUpSamplingRate, pAttachedRawSeq);
}

UINT32 AmbaDSP_VideoEncControlBlend(UINT16 NumStream, const UINT16 *pStreamIdx, const AMBA_DSP_VIDEO_BLEND_CONFIG_s *pBlendCfg)
{
    return dsp_video_enc_ctrl_blend(NumStream, pStreamIdx, pBlendCfg);
}

UINT32 AmbaDSP_VideoEncControlQuality(UINT16 NumStream, const UINT16 *pStreamIdx, const AMBA_DSP_VIDEO_ENC_QUALITY_s *pQCtrl)
{
    return dsp_video_enc_ctrl_quality(NumStream, pStreamIdx, pQCtrl);
}

UINT32 AmbaDSP_VideoEncExecIntervalCap(UINT16 NumStream, const UINT16 *pStreamIdx)
{
    return dsp_video_enc_exec_intervalcap(NumStream, pStreamIdx);
}

UINT32 AmbaDSP_VideoEncFeedYuvData(UINT16 NumStream, const UINT16 *pStreamIdx, const AMBA_DSP_EXT_YUV_BUF_s *pExtYuvBuf)
{
    return dsp_video_enc_feed_yuv_data(NumStream, pStreamIdx, pExtYuvBuf);
}

UINT32 AmbaDSP_VideoEncGrpConfig(const AMBA_DSP_VIDEO_ENC_GROUP_s *pGrpCfg)
{
    return dsp_video_enc_grp_cfg(pGrpCfg);
}

UINT32 AmbaDSP_VideoEncMvConfig(UINT16 StreamIdx, const AMBA_DSP_VIDEO_ENC_MV_CFG_s *pMvCfg)
{
    return dsp_video_enc_mv_cfg(StreamIdx, pMvCfg);
}

UINT32 AmbaDSP_VideoEncDescFmtConfig(UINT16 StreamIdx, UINT16 CatIdx, UINT32 OptVal)
{
    return dsp_video_enc_desc_fmt_cfg(StreamIdx, CatIdx, OptVal);
}

UINT32 AmbaDSP_DataCapCfg(UINT16 CapInstance, const AMBA_DSP_DATACAP_CFG_s *pDataCapCfg)
{
    return dsp_data_cap_cfg(CapInstance, pDataCapCfg);
}

UINT32 AmbaDSP_UpdateCapBuffer(UINT16 CapInstance, const AMBA_DSP_DATACAP_BUF_CFG_s *pCapBuf, UINT64 *pAttachedRawSeq)
{
    return dsp_update_cap_buffer(CapInstance, pCapBuf, pAttachedRawSeq);
}

UINT32 AmbaDSP_DataCapCtrl(UINT16 NumCapInstance, const UINT16 *pCapInstance, const AMBA_DSP_DATACAP_CTRL_s *pDataCapCtrl, UINT64 *pAttachedRawSeq)
{
    return dsp_data_cap_ctrl(NumCapInstance, pCapInstance, pDataCapCtrl, pAttachedRawSeq);
}

UINT32 AmbaDSP_StillYuv2Yuv(const AMBA_DSP_YUV_IMG_BUF_s *pYuvIn, const AMBA_DSP_YUV_IMG_BUF_s *pYuvOut, const AMBA_DSP_ISOCFG_CTRL_s *pIsoCfg, UINT32 Opt, UINT64 *pAttachedRawSeq)
{
    return dsp_still_yuv2yuv(pYuvIn, pYuvOut, pIsoCfg, Opt, pAttachedRawSeq);
}

UINT32 AmbaDSP_StillEncodeCtrl(UINT16 StreamIdx, const AMBA_DSP_STLENC_CTRL_s *pStlEncCtrl, UINT64 *pAttachedRawSeq)
{
    return dsp_still_encode_ctrl(StreamIdx, pStlEncCtrl, pAttachedRawSeq);
}

UINT32 AmbaDSP_CalcStillYuvExtBufSize(UINT16 StreamIdx, UINT16 BufType, UINT16 *pBufPitch, UINT32 *pBufUnitSize)
{
    return dsp_calc_still_yuv_extbuf_size(StreamIdx, BufType, pBufPitch, pBufUnitSize);
}

UINT32 AmbaDSP_StillRaw2Yuv(const AMBA_DSP_RAW_BUF_s *pRawIn,
                            const AMBA_DSP_BUF_s *pAuxBufIn,
                            const AMBA_DSP_YUV_IMG_BUF_s *pYuvOut,
                            const AMBA_DSP_ISOCFG_CTRL_s *pIsoCfg,
                            UINT32 Opt,
                            UINT64 *pAttachedRawSeq)
{
    UINT32 Rval = DSP_ERR_NONE;
    UINT16 i;
    const void *pVoid;
    AMBA_IK_MODE_CFG_s ImgMode;
    AMBA_IK_ABILITY_s IkAbility = {0};
    AMBA_IK_VIN_SENSOR_INFO_s VinSensorInfo = {0};
#if defined (CONFIG_SOC_CV2FS) || defined (CONFIG_SOC_CV22FS) || defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
#ifdef SUPPORT_IK_FRM_INFO_QUERY
    AMBA_IK_QUERY_FRAME_INFO_s FrmInfo = {0};
#endif
#else
    AMBA_IK_HDR_RAW_INFO_s HdrRawInfo = {0};
#endif
    dsp_ik_drv_cfg_sensor_info_t SensorInfo;
    dsp_ik_drv_cfg_dol_offset_t DolOfst;

    if (pIsoCfg != NULL) {
        ImgMode.ContextId = pIsoCfg->CtxIndex;
#ifdef SUPPORT_AMBA_IK
        Rval = AmbaIK_GetVinSensorInfo(&ImgMode, &VinSensorInfo);
#endif
        if (Rval != OK) {
            dsp_wrapper_osal_printU5("StlR2Y GetVinSensorInfo Fail", 0U, 0U, 0U, 0U, 0U);
            Rval = DSP_ERR_0001;
        } else {
            (void)dsp_wrapper_osal_memset(&SensorInfo, 0, sizeof(dsp_ik_drv_cfg_sensor_info_t));
            dsp_wrapper_osal_typecast(&pVoid, &SensorInfo);
            SensorInfo.Pipe = DSP_IK_PIPE_STL;
            SensorInfo.SensorMode = VinSensorInfo.SensorMode;
            SensorInfo.Compression = VinSensorInfo.Compression;
            Rval = dsp_liveview_ik_drv_cfg(0U/*DONT CARE*/, DSP_IK_DRV_CFG_TYPE_SNSR_INFO, &SensorInfo);
            if (Rval != OK) {
                dsp_wrapper_osal_printU5("VZ[%d] DSP_IK_DRV_CFG_TYPE_SNSR_INFO Fail", ImgMode.ContextId, 0U, 0U, 0U, 0U);
                Rval = DSP_ERR_0001;
            }
        }

        if (pIsoCfg->CfgAddress > 0U) {
            (void)AmbaIK_GetContextAbility(&ImgMode, &IkAbility);
            (void)dsp_wrapper_osal_memset(&DolOfst, 0, sizeof(dsp_ik_drv_cfg_dol_offset_t));
            DolOfst.Pipe = DSP_IK_PIPE_STL;

            if ((IkAbility.VideoPipe == AMBA_IK_VIDEO_HDR_EXPO_2) ||
                (IkAbility.VideoPipe == AMBA_IK_VIDEO_HDR_EXPO_3)) {
#if defined (CONFIG_SOC_CV2FS) || defined (CONFIG_SOC_CV22FS) || defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
                if (Rval != OK) {
                    dsp_wrapper_osal_printU5("StlR2Y previous Error %x", Rval, 0U, 0U, 0U, 0U);
                    Rval = DSP_ERR_0001;
                } else {
#ifdef SUPPORT_IK_FRM_INFO_QUERY
#ifdef SUPPORT_AMBA_IK
                    Rval = AmbaIK_QueryFrameInfo(&ImgMode, pIsoCfg->CfgIndex, &FrmInfo);
#endif
                    if (Rval != OK) {
                        dsp_wrapper_osal_printU5("VZ[%d] AmbaIK_QueryFrameInfo Fail %x", ImgMode.ContextId, Rval, 0U, 0U, 0U);
                        Rval = DSP_ERR_0001;
                    }
#else
                    dsp_wrapper_osal_printU5("VZ[%d] AmbaIK_QueryFrameInfo n/a", ImgMode.ContextId, 0U, 0U, 0U, 0U);
#endif
                }
#else
                dsp_wrapper_osal_typecast(&pVoid, &pIsoCfg->CfgAddress);
                if (Rval != OK) {
                    dsp_wrapper_osal_printU5("VZ[%d] AmbaIK_GetIsoHdrRawOffest pre Error %d", ImgMode.ContextId, Rval, 0U, 0U, 0U);
                    Rval = DSP_ERR_0001;
                } else {
#ifdef SUPPORT_AMBA_IK
                    Rval = AmbaIK_GetIsoHdrRawOffest(pVoid, &HdrRawInfo);
#endif
                }
#endif
                if (Rval != OK) {
                    dsp_wrapper_osal_printU5("StlR2Y GetIsoHdrRawOffest Fail", 0U, 0U, 0U, 0U, 0U);
                } else {
#if defined (CONFIG_SOC_CV2FS) || defined (CONFIG_SOC_CV22FS) || defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
#ifdef SUPPORT_IK_FRM_INFO_QUERY
                    for (i = 0U; i < IK_MAX_HDR_EXPOSURE_NUM; i++) {
                        DolOfst.Y[i] = FrmInfo.HdrRawInfo.YOffset[i];
                    }
#else
                    dsp_wrapper_osal_printU5("VZ[%d] Fixed DOL ofst[0]", ImgMode.ContextId, 0U, 0U, 0U, 0U);
                    for (i = 0U; i < IK_MAX_HDR_EXPOSURE_NUM; i++) {
                        DolOfst.Y[i] = 0U;
                    }
#endif
#else
                    for (i = 0U; i < MAX_IK_MAX_HDR_EXPOSURE_NUM; i++) {
                        DolOfst.Y[i] = HdrRawInfo.YOffset[i];
                    }
#endif
                }
            } else {
                for (i = 0U; i < DSP_IK_DOL_OFST_NUM; i++) {
                    DolOfst.Y[i] = 0U;
                }
            }
            Rval = dsp_liveview_ik_drv_cfg(0U/*DONT CARE*/, DSP_IK_DRV_CFG_TYPE_DOL_OFST, &DolOfst);
            if (Rval != OK) {
                dsp_wrapper_osal_printU5("VZ[%d] DSP_IK_DRV_CFG_TYPE_DOL_OFST Fail", ImgMode.ContextId, 0U, 0U, 0U, 0U);
                Rval = DSP_ERR_0001;
            }
        }

        if (Rval == OK) {
            Rval = dsp_still_raw2yuv(pRawIn, pAuxBufIn, pYuvOut, pIsoCfg, Opt, pAttachedRawSeq);
        } else {
            dsp_wrapper_osal_printU5("StlR2Y dsp_still_raw2yuv Error %x", Rval, 0U, 0U, 0U, 0U);
            Rval = DSP_ERR_0001;
        }
    }

    return Rval;
}

UINT32 AmbaDSP_StillRaw2Raw(const AMBA_DSP_RAW_BUF_s *pRawIn,
                            const AMBA_DSP_BUF_s *pAuxBufIn,
                            const AMBA_DSP_RAW_BUF_s *pRawOut,
                            const AMBA_DSP_ISOCFG_CTRL_s *pIsoCfg,
                            UINT32 Opt,
                            UINT64 *pAttachedRawSeq)
{
    UINT32 Rval = DSP_ERR_NONE;
    UINT16 i;
    const void *pVoid;
    AMBA_IK_MODE_CFG_s ImgMode;
    AMBA_IK_ABILITY_s IkAbility = {0};
    AMBA_IK_VIN_SENSOR_INFO_s VinSensorInfo = {0};
#if defined (CONFIG_SOC_CV2FS) || defined (CONFIG_SOC_CV22FS) || defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
#ifdef SUPPORT_IK_FRM_INFO_QUERY
    AMBA_IK_QUERY_FRAME_INFO_s FrmInfo = {0};
#endif
#else
    AMBA_IK_HDR_RAW_INFO_s HdrRawInfo = {0};
#endif
    dsp_ik_drv_cfg_sensor_info_t SensorInfo;
    dsp_ik_drv_cfg_dol_offset_t DolOfst;
#if defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
    dsp_ik_drv_cfg_ext_raw_cmpr_info_t CfgExtRawCmpr;
#endif

    if (pIsoCfg != NULL) {
        ImgMode.ContextId = pIsoCfg->CtxIndex;
#ifdef SUPPORT_AMBA_IK
        Rval = AmbaIK_GetVinSensorInfo(&ImgMode, &VinSensorInfo);
#endif
        if (Rval != OK) {
            dsp_wrapper_osal_printU5("StlR2R GetVinSensorInfo Fail", 0U, 0U, 0U, 0U, 0U);
            Rval = DSP_ERR_0001;
        } else {
            (void)dsp_wrapper_osal_memset(&SensorInfo, 0, sizeof(dsp_ik_drv_cfg_sensor_info_t));
            dsp_wrapper_osal_typecast(&pVoid, &SensorInfo);
            SensorInfo.Pipe = DSP_IK_PIPE_STL;
            SensorInfo.SensorMode = VinSensorInfo.SensorMode;
            SensorInfo.Compression = VinSensorInfo.Compression;
            Rval = dsp_liveview_ik_drv_cfg(0U/*DONT CARE*/, DSP_IK_DRV_CFG_TYPE_SNSR_INFO, &SensorInfo);
            if (Rval != OK) {
                dsp_wrapper_osal_printU5("VZ[%d] DSP_IK_DRV_CFG_TYPE_SNSR_INFO Fail", ImgMode.ContextId, 0U, 0U, 0U, 0U);
                Rval = DSP_ERR_0001;
            }
        }


        if (pIsoCfg->CfgAddress > 0U) {
            if (Rval != OK) {
                dsp_wrapper_osal_printU5("StlR2R previous Error %x", Rval, 0U, 0U, 0U, 0U);
                Rval = DSP_ERR_0001;
            } else {
                (void)dsp_wrapper_osal_memset(&DolOfst, 0, sizeof(dsp_ik_drv_cfg_dol_offset_t));
                DolOfst.Pipe = DSP_IK_PIPE_STL;
                (void)AmbaIK_GetContextAbility(&ImgMode, &IkAbility);

                if ((IkAbility.VideoPipe == AMBA_IK_VIDEO_HDR_EXPO_2) ||
                    (IkAbility.VideoPipe == AMBA_IK_VIDEO_HDR_EXPO_3)) {
#if defined (CONFIG_SOC_CV2FS) || defined (CONFIG_SOC_CV22FS) || defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
#ifdef SUPPORT_IK_FRM_INFO_QUERY
#ifdef SUPPORT_AMBA_IK
                    Rval = AmbaIK_QueryFrameInfo(&ImgMode, pIsoCfg->CfgIndex, &FrmInfo);
#endif
                    if (Rval != OK) {
                        dsp_wrapper_osal_printU5("VZ[%d] AmbaIK_QueryFrameInfo Fail %x", ImgMode.ContextId, Rval, 0U, 0U, 0U);
                        Rval = DSP_ERR_0001;
                    }
#else
                    dsp_wrapper_osal_printU5("VZ[%d] AmbaIK_QueryFrameInfo n/a", ImgMode.ContextId, 0U, 0U, 0U, 0U);
#endif
#else
                    dsp_wrapper_osal_typecast(&pVoid, &pIsoCfg->CfgAddress);
#ifdef SUPPORT_AMBA_IK
                    Rval = AmbaIK_GetIsoHdrRawOffest(pVoid, &HdrRawInfo);
#endif
                    if (Rval != OK) {
                        dsp_wrapper_osal_printU5("VZ[%d] AmbaIK_GetIsoHdrRawOffest Fail %x", ImgMode.ContextId, Rval, 0U, 0U, 0U);
                        Rval = DSP_ERR_0001;
                    }
#endif
                    if (Rval != OK) {
                        dsp_wrapper_osal_printU5("StlR2R previous Error %x", Rval, 0U, 0U, 0U, 0U);
                    } else {
#if defined (CONFIG_SOC_CV2FS) || defined (CONFIG_SOC_CV22FS) || defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
#ifdef SUPPORT_IK_FRM_INFO_QUERY
                        for (i = 0U; i < IK_MAX_HDR_EXPOSURE_NUM; i++) {
                            DolOfst.Y[i] = FrmInfo.HdrRawInfo.YOffset[i];
                        }
#else
                        dsp_wrapper_osal_printU5("VZ[%d] Fixed DOL ofst[0]", ImgMode.ContextId, 0U, 0U, 0U, 0U);
                        for (i = 0U; i < IK_MAX_HDR_EXPOSURE_NUM; i++) {
                            DolOfst.Y[i] = 0U;
                        }
#endif
#else
                        for (i = 0U; i < MAX_IK_MAX_HDR_EXPOSURE_NUM; i++) {
                            DolOfst.Y[i] = HdrRawInfo.YOffset[i];
                        }
#endif
                    }
                } else {
                    for (i = 0U; i < DSP_IK_DOL_OFST_NUM; i++) {
                        DolOfst.Y[i] = 0U;
                    }
                }

                Rval = dsp_liveview_ik_drv_cfg(0U/*DONT CARE*/, DSP_IK_DRV_CFG_TYPE_DOL_OFST, &DolOfst);
                if (Rval != OK) {
                    dsp_wrapper_osal_printU5("VZ[%d] DSP_IK_DRV_CFG_TYPE_DOL_OFST Fail", ImgMode.ContextId, 0U, 0U, 0U, 0U);
                    Rval = DSP_ERR_0001;
                }

#if defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
                (void)dsp_wrapper_osal_memset(&CfgExtRawCmpr, 0, sizeof(dsp_ik_drv_cfg_ext_raw_cmpr_info_t));

#ifdef SUPPORT_AMBA_IK
                if (Rval == OK) {
                    Rval = AmbaIK_GetExtRawOutMode(&ImgMode, &CfgExtRawCmpr.Compression);
                }
#endif
                if (Rval == OK) {
                    CfgExtRawCmpr.Pipe = DSP_IK_PIPE_STL;
                    Rval = dsp_liveview_ik_drv_cfg(0U/*DONT CARE*/, DSP_IK_DRV_CFG_TYPE_EXT_RAW_CMPR, &CfgExtRawCmpr);
                }
#endif
            }
        }
        if (Rval == OK) {
            Rval = dsp_still_raw2raw(pRawIn, pAuxBufIn, pRawOut, pIsoCfg, Opt, pAttachedRawSeq);
        } else {
            dsp_wrapper_osal_printU5("StlR2R dsp_still_raw2raw Error %x", Rval, 0U, 0U, 0U, 0U);
            Rval = DSP_ERR_0001;
        }
    } else {
        Rval = DSP_ERR_0000;
    }

    return Rval;
}

UINT32 AmbaDSP_VideoDecConfig(UINT16 MaxNumStream, const AMBA_DSP_VIDDEC_STREAM_CONFIG_s *pStreamConfig)
{
    return dsp_video_dec_cfg(MaxNumStream, pStreamConfig);
}

UINT32 AmbaDSP_VideoDecStart(UINT16 NumStream, const UINT16 *pStreamIdx, const AMBA_DSP_VIDDEC_START_CONFIG_s *pStartConfig)
{
    return dsp_video_dec_start(NumStream, pStreamIdx, pStartConfig);
}

UINT32 AmbaDSP_VideoDecStop(UINT16 NumStream, const UINT16 *pStreamIdx, const UINT8 *pShowLastFrame)
{
    return dsp_video_dec_stop(NumStream, pStreamIdx, pShowLastFrame);
}

UINT32 AmbaDSP_VideoDecTrickPlay(UINT16 NumStream, const UINT16 *pStreamIdx, const AMBA_DSP_VIDDEC_TRICKPLAY_s *pTrickPlay)
{
    return dsp_video_dec_trickplay(NumStream, pStreamIdx, pTrickPlay);
}

UINT32 AmbaDSP_VideoDecBitsFifoUpdate(UINT16 NumStream, const UINT16 *pStreamIdx, const AMBA_DSP_VIDDEC_BITS_FIFO_s *pBitsFifo)
{
    return dsp_video_dec_bitsfifo_update(NumStream, pStreamIdx, pBitsFifo);
}

UINT32 AmbaDSP_VideoDecPostCtrl(UINT16 StreamIdx, UINT16 NumPostCtrl, const AMBA_DSP_VIDDEC_POST_CTRL_s *pPostCtrl)
{
    return dps_video_dec_post_ctrl(StreamIdx, NumPostCtrl, pPostCtrl);
}

UINT32 AmbaDSP_StillDecStart(UINT16 StreamIdx, const AMBA_DSP_STLDEC_START_CONFIG_s *pDecConfig)
{
    return dsp_still_dec_start(StreamIdx, pDecConfig);
}

UINT32 AmbaDSP_StillDecStop(void)
{
    return dsp_still_dec_stop();
}

UINT32 AmbaDSP_StillDecYuv2Yuv(const AMBA_DSP_YUV_IMG_BUF_s *pSrcYuvBufAddr,
                               const AMBA_DSP_YUV_IMG_BUF_s *pDestYuvBufAddr,
                               const AMBA_DSP_STLDEC_YUV2YUV_s *pOperation)
{
    return dsp_still_dec_yuv2yuv(pSrcYuvBufAddr, pDestYuvBufAddr, pOperation);
}

UINT32 AmbaDSP_StillDecYuvBlend(const AMBA_DSP_YUV_IMG_BUF_s *pSrc1YuvBufAddr,
                                const AMBA_DSP_YUV_IMG_BUF_s *pSrc2YuvBufAddr,
                                const AMBA_DSP_YUV_IMG_BUF_s *pDestYuvBufAddr,
                                const AMBA_DSP_STLDEC_BLEND_s *pOperation)
{
    return dsp_still_dec_yuv_blend(pSrc1YuvBufAddr, pSrc2YuvBufAddr, pDestYuvBufAddr, pOperation);
}

UINT32 AmbaDSP_StillDecDispYuvImg(UINT8 VoutIdx,
                                  const AMBA_DSP_YUV_IMG_BUF_s *pYuvBufAddr,
                                  const AMBA_DSP_VOUT_VIDEO_CFG_s *pVoutConfig)
{
    return dsp_still_dec_disp_yuv_img(VoutIdx, pYuvBufAddr, pVoutConfig);
}

#if defined (CONFIG_ENABLE_DSP_DIAG)
UINT32 AmbaDSP_DiagCase(UINT32 DiagId)
{
    return dsp_diag_case(DiagId);
}
#endif

#ifdef CONFIG_ENABLE_DSP_MONITOR
UINT32 AmbaDSP_MonitorInit(void)
{
    return dsp_mon_init();
}

UINT32 AmbaDSP_MonitorErrorNotify(const AMBA_DSP_MONITOR_ERR_NOTIFY_s *pErrNotify)
{
    return dsp_mon_error_notify(pErrNotify);
}

UINT32 AmbaDSP_MonitorCrcCmpr(const AMBA_DSP_MONITOR_CRC_CMPR_s *pCrcCmpr)
{
    return dsp_mon_crc_cmpr(pCrcCmpr);
}

UINT32 AmbaDSP_GetMonitorConfig(UINT32 ModuleId, AMBA_DSP_MONITOR_CONFIG_s *pMonCfg)
{
    return dsp_mon_get_cfg(ModuleId, pMonCfg);
}

UINT32 AmbaDSP_MonitorHeartBeatCfg(const AMBA_DSP_MONITOR_HEARTBEAT_s *pHeartBeatCfg)
{
    return dsp_mon_heartbeat_cfg(pHeartBeatCfg);
}

#endif

UINT32 AmbaDSP_ClockUpdate(UINT32 ClockId, UINT32 Stage, UINT32 Freq)
{
    UINT32 Rval = DSP_ERR_NONE;
    UINT16 Type = 0U;

    if (ClockId == AMBA_DSP_CLOCK_ID_CORE) {
        if (Stage == AMBA_DSP_CLOCK_STAGE_PREPROC) {
            Type = DSP_SYS_DRV_CFG_TYPE_CORE_CLK_PRE;
        } else {
            Type = DSP_SYS_DRV_CFG_TYPE_CORE_CLK;
        }
        Rval = dsp_sys_drv_cfg(Type, &Freq);
    }

    return Rval;
}

