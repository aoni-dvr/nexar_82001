/**
*  @file AmbaDSP_CmdsSend.c
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
*  @details APIs for generating DSP commands
*
*/

#include "AmbaDSP_Log.h"
#include "AmbaDSP_ArmComm.h"
#include "AmbaDSP_CmdsSend.h"
#include "AmbaDSP_CmdMsgDump.h"
#include "dsp_priv_api.h"

#if defined (CONFIG_ENABLE_DSP_COMMAND_PARM_DBG) || defined (CONFIG_LINUX)
//#define CMD_HASH_VALUE_MAX  (355U)
#define CMD_HASH_BUCK_NUM   (12U)      //CMD_HASH_VALUE_MAX / 32, +4
#define CMD_ON              (1U)

/* Command log hash bit flag */
static UINT32 CmdLog[CMD_HASH_BUCK_NUM] = {0};

/**
 * Set command log hash bit
 * @param [in] Idx Index of the command
 * @param [in] Val The value is going to set
 */
static inline void SetCmdLogBit(UINT32 Idx, UINT8 Val)
{
    if (Val == 1U) {
        CmdLog[Idx >> 5U] |= (UINT32)(1UL << (Idx % 32UL));
    } else {
        CmdLog[Idx >> 5U] &= ~(UINT32)(1UL << (Idx % 32UL));
    }
}

/**
 * Get command log hash bit
 * @param [in] Idx Index of the command
 * @return Log bit value of index
 */
static inline UINT32 GetCmdLogBit(UINT32 Idx)
{
    return 0x1U & (CmdLog[Idx >> 5U] >> (Idx % 32U));
}

/**
 * Hash command code for log bit flag
 * @param [in] CmdCode The command code is going to hash
 * @return Hash result
 */
static inline UINT32 CmdCodeHash(UINT32 CmdCode)
{
    UINT32 Buck0;
    UINT32 Buck1;
    UINT32 Buck2;

    Buck0 = ((CmdCode >> 24U) & (0xFU)) - 1U;     //0~12,  13
    Buck1 = ((CmdCode >>  4U) & (0x1U));          //0~1 ,   2
    Buck2 = ((CmdCode       ) & (0xFU));          //0~15,  16

    return (Buck0 << 5UL) + ((Buck1 << 4UL) + (Buck2));
}

static inline UINT32 ChangeVprocCmdCode(UINT32 CmdCode)
{
    return (CmdCode & (0x200000FU)) | (0x10U);
}

static inline UINT32 ChangeVinCmdCode(UINT32 CmdCode)
{
    return (CmdCode & (0x400000FU)) | (0x10U);
}

/**
 * Check command bit value, to show command parameter or not
 * @param [in] CmdCode The command code is going to check
 * @return 0 - no need, 1 - show this command
 */
static inline UINT32 AmbaLL_CmdLogChk(UINT32 CmdCode)
{
    UINT32 Rval;

    if ((CmdCode == CMD_VPROC_MULTI_STREAM_PP) ||
        (CmdCode == CMD_VPROC_SET_EFFECT_BUF_IMG_SZ) ||
        (CmdCode == CMD_VPROC_SET_VPROC_GRPING) ||
        (CmdCode == CMD_VPROC_MULTI_CHAN_PROC_ORDER)) {
        Rval = GetCmdLogBit(CmdCodeHash(ChangeVprocCmdCode(CmdCode)));
    } else if ((CmdCode == CMD_VIN_VOUT_LOCK_SETUP) ||
        (CmdCode == CMD_VIN_ATTACH_METADATA) ||
        (CmdCode == CMD_VIN_TEMPORAL_DEMUX_SETUP)) {
        Rval = GetCmdLogBit(CmdCodeHash(ChangeVinCmdCode(CmdCode)));
    } else {
        Rval = GetCmdLogBit(CmdCodeHash(CmdCode));
    }

    return Rval;
}

static void AmbaLL_CmdShow(UINT32 CmdCode, UINT8 On)
{
    if ((CmdCode == CMD_VPROC_MULTI_STREAM_PP) ||
        (CmdCode == CMD_VPROC_SET_EFFECT_BUF_IMG_SZ) ||
        (CmdCode == CMD_VPROC_SET_VPROC_GRPING) ||
        (CmdCode == CMD_VPROC_MULTI_CHAN_PROC_ORDER)) {
        SetCmdLogBit(CmdCodeHash(ChangeVprocCmdCode(CmdCode)), On);
    } else if ((CmdCode == CMD_VIN_VOUT_LOCK_SETUP) ||
               (CmdCode == CMD_VIN_ATTACH_METADATA) ||
               (CmdCode == CMD_VIN_TEMPORAL_DEMUX_SETUP)) {
        SetCmdLogBit(CmdCodeHash(ChangeVinCmdCode(CmdCode)), On);
    } else {
        SetCmdLogBit(CmdCodeHash(CmdCode), On);
    }
    AmbaLL_LogUInt5("AmbaLL_CmdShow 0x%X %d", CmdCode, On, 0U, 0U, 0U);
}

static void AmbaLL_CmdShowCat(UINT8 CmdCat, UINT8 On)
{
    UINT32 Idx;
    UINT32 Start = (((UINT32)CmdCat - CAT_DSP_CFG) << 5U);
    UINT32 End = Start | (0x1FU);

    if (CmdCat <= CAT_IDSP) {
        for (Idx = Start; Idx <= End; Idx++) {
            SetCmdLogBit(Idx, On);
        }
    }
    AmbaLL_LogUInt5("AmbaLL_CmdShowCat 0x%d %d, %x %x", CmdCat, On, Start, End, 0U);
}

static void AmbaLL_CmdShowAll(UINT8 On)
{
    UINT32 Rval;
    if (On == 0U) {
        Rval = dsp_osal_memset(CmdLog, 0, (UINT32)sizeof(CmdLog));
        if (Rval != OK) {
            AmbaLL_LogUInt5("[Err][AmbaLL_CmdShowAll] dsp_osal_memset return 0x%x @%d", Rval, __LINE__, 0U, 0U, 0U);
        }
    } else {
        Rval = dsp_osal_memset(CmdLog, 0xFF, (UINT32)sizeof(CmdLog));
        if (Rval != OK) {
            AmbaLL_LogUInt5("[Err][AmbaLL_CmdShowAll] dsp_osal_memset return 0x%x @%d", Rval, __LINE__, 0U, 0U, 0U);
        }
    }
}

void dsp_msg_show_all(UINT8 On)
{
    UINT32 CatIdx;

    for (CatIdx = 1U/*CAT_DSP*/; CatIdx <= 9U/*CAT_IDSP*/; CatIdx++) {
        DSP_SetMsgLogCat(CatIdx, On);
    }
}

void dsp_msg_show(UINT32 MsgCode, UINT8 On)
{
    SetMsgLogBit(MsgCodeHash(MsgCode), On);
}

void dsp_cmd_show(UINT32 CmdCode, UINT8 On)
{
    AmbaLL_CmdShow(CmdCode, On);
}

void AmbaDSP_CmdShowCat(UINT32 CmdCat, UINT8 On)
{
    AmbaLL_CmdShowCat((UINT8)CmdCat, On);
}

void dsp_cmd_show_all(UINT8 On)
{
    AmbaLL_CmdShowAll(On);
}
#else
static inline UINT32 AmbaLL_CmdLogChk(UINT32 CmdCode)
{
    return 0U;
}

void AmbaDSP_CmdShowAll(UINT8 On)
{
    AmbaLL_LogUInt5("Please turn on ENABLE_DSP_COMMAND_PARM_DBG in menuconfig", On, 0U, 0U, 0U, 0U);
}

void AmbaLL_CmdShowCat(UINT8 CmdCat,
                       UINT8 On)
{
    AmbaLL_LogUInt5("Please turn on ENABLE_DSP_COMMAND_PARM_DBG in menuconfig", CmdCat, On, 0U, 0U, 0U);
}

void dsp_cmd_show(UINT32 CmdCode, UINT8 On)
{
(void)CmdCode;
    AmbaLL_LogUInt5("Please turn on ENABLE_DSP_COMMAND_PARM_DBG in menuconfig", On, 0U, 0U, 0U, 0U);
}


void AmbaDSP_CmdShowCat(UINT32 CmdCat,
                        UINT8 On)
{
    AmbaLL_LogUInt5("Please turn on ENABLE_DSP_COMMAND_PARM_DBG in menuconfig", 0U, 0U, 0U, 0U, 0U);
}

#endif
/*-----------------------------------------------------------------------------------------------*\
 * CAT_DSP_CFG (1)
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaLL_CmdDspConfig(UINT8 WriteMode, cmd_dsp_config_t *pCmdData)
{
    UINT32 Rval;
    CmdInfo_t Info = {.WriteMode = WriteMode};

    pCmdData->cmd_code = CMD_DSP_CONFIG;
    Rval = AmbaLL_CmdSend(WriteMode, pCmdData, (UINT32)sizeof(cmd_dsp_config_t));
    if ((Rval == OK) && (AmbaLL_CmdLogChk(CMD_DSP_CONFIG) == CMD_ON)){
        dump_cmd_dsp_config_t(pCmdData, &Info);
    }
    return Rval;
}

UINT32 AmbaLL_CmdDspHalInf(UINT8 WriteMode, cmd_dsp_hal_inf_t *pCmdData)
{
    UINT32 Rval;
    CmdInfo_t Info = {.WriteMode = WriteMode};

    pCmdData->cmd_code = CMD_DSP_HAL_INF;
    Rval = AmbaLL_CmdSend(WriteMode, pCmdData, (UINT32)sizeof(cmd_dsp_hal_inf_t));
    if ((Rval == OK) && (AmbaLL_CmdLogChk(CMD_DSP_HAL_INF) == CMD_ON)){
        dump_cmd_dsp_hal_inf_t(pCmdData, &Info);
    }
    return Rval;
}

UINT32 AmbaLL_CmdDspSuspendProfile(UINT8 WriteMode, cmd_dsp_suspend_profile_t *pCmdData)
{
    UINT32 Rval;
    CmdInfo_t Info = {.WriteMode = WriteMode};

    pCmdData->cmd_code = CMD_DSP_SUSPEND_PROFILE;
    Rval = AmbaLL_CmdSend(WriteMode, pCmdData, (UINT32)sizeof(cmd_dsp_suspend_profile_t));
    if ((Rval == OK) && (AmbaLL_CmdLogChk(CMD_DSP_SUSPEND_PROFILE) == CMD_ON)){
        dump_cmd_dsp_suspend_profile_t(pCmdData, &Info);
    }
    return Rval;
}

UINT32 AmbaLL_CmdDspSetDebugLevel(UINT8 WriteMode, cmd_set_debug_level_t *pCmdData)
{
    UINT32 Rval;
    CmdInfo_t Info = {.WriteMode = WriteMode};

    pCmdData->cmd_code = CMD_DSP_SET_DEBUG_LEVEL;
    Rval = AmbaLL_CmdSend(WriteMode, pCmdData, (UINT32)sizeof(cmd_set_debug_level_t));
    if ((Rval == OK) && (AmbaLL_CmdLogChk(CMD_DSP_SET_DEBUG_LEVEL) == CMD_ON)){
        dump_cmd_set_debug_level_t(pCmdData, &Info);
    }
    return Rval;
}

UINT32 AmbaLL_CmdDspPrintThreadDisableMask(UINT8 WriteMode, cmd_print_th_disable_mask_t *pCmdData)
{
    UINT32 Rval;
    CmdInfo_t Info = {.WriteMode = WriteMode};

    pCmdData->cmd_code = CMD_DSP_PRINT_THREAD_DISABLE_MASK;
    Rval = AmbaLL_CmdSend(WriteMode, pCmdData, (UINT32)sizeof(cmd_print_th_disable_mask_t));
    if ((Rval == OK) && (AmbaLL_CmdLogChk(CMD_DSP_PRINT_THREAD_DISABLE_MASK) == CMD_ON)){
        dump_cmd_print_th_disable_mask_t(pCmdData, &Info);
    }
    return Rval;
}

UINT32 AmbaLL_CmdDspBindingCfg(UINT8 WriteMode, cmd_binding_cfg_t *pCmdData)
{
    UINT32 Rval;
    CmdInfo_t Info = {.WriteMode = WriteMode};

    pCmdData->cmd_code = CMD_DSP_BINDING_CFG;
    Rval = AmbaLL_CmdSend(WriteMode, pCmdData, (UINT32)sizeof(cmd_binding_cfg_t));
    if ((Rval == OK) && (AmbaLL_CmdLogChk(CMD_DSP_BINDING_CFG) == CMD_ON)){
        dump_cmd_binding_cfg_t(pCmdData, &Info);
    }
    return Rval;
}

UINT32 AmbaLL_CmdDspActivateProfile(UINT8 WriteMode, cmd_dsp_activate_profile_t *pCmdData)
{
    UINT32 Rval;
    CmdInfo_t Info = {.WriteMode = WriteMode};

    pCmdData->cmd_code = CMD_DSP_ACTIVATE_PROFILE;
    Rval = AmbaLL_CmdSend(WriteMode, pCmdData, (UINT32)sizeof(cmd_dsp_activate_profile_t));
    if ((Rval == OK) && (AmbaLL_CmdLogChk(CMD_DSP_ACTIVATE_PROFILE) == CMD_ON)){
        dump_cmd_dsp_activate_profile_t(pCmdData, &Info);
    }
    return Rval;
}

UINT32 AmbaLL_CmdDspVprocFlowMaxCfg(UINT8 WriteMode, cmd_dsp_vproc_flow_max_cfg_t *pCmdData)
{
    UINT32 Rval;
    CmdInfo_t Info = {.WriteMode = WriteMode};

    pCmdData->cmd_code = CMD_DSP_VPROC_FLOW_MAX_CFG;
    Rval = AmbaLL_CmdSend(WriteMode, pCmdData, (UINT32)sizeof(cmd_dsp_vproc_flow_max_cfg_t));
    if ((Rval == OK) && (AmbaLL_CmdLogChk(CMD_DSP_VPROC_FLOW_MAX_CFG) == CMD_ON)){
        dump_cmd_dsp_vproc_flow_max_cfg_t(pCmdData, &Info);
    }
    return Rval;
}

UINT32 AmbaLL_CmdDspVinFlowMaxCfg(UINT8 WriteMode, cmd_dsp_vin_flow_max_cfg_t *pCmdData)
{
    UINT32 Rval;
    CmdInfo_t Info = {.WriteMode = WriteMode};

    pCmdData->cmd_code = CMD_DSP_VIN_FLOW_MAX_CFG;
    Rval = AmbaLL_CmdSend(WriteMode, pCmdData, (UINT32)sizeof(cmd_dsp_vin_flow_max_cfg_t));
    if ((Rval == OK) && (AmbaLL_CmdLogChk(CMD_DSP_VIN_FLOW_MAX_CFG) == CMD_ON)){
        dump_cmd_dsp_vin_flow_max_cfg_t(pCmdData, &Info);
    }
    return Rval;
}

UINT32 AmbaLL_CmdDspSetProfile(UINT8 WriteMode, cmd_dsp_set_profile_t *pCmdData)
{
    UINT32 Rval;
    CmdInfo_t Info = {.WriteMode = WriteMode};

    pCmdData->cmd_code = CMD_DSP_SET_PROFILE;
#ifdef SUPPORT_DSP_MSG_CRC
    Rval = AmbaLL_CmdSendSetupInitCrc(WriteMode, pCmdData, (UINT32)sizeof(cmd_dsp_set_profile_t));
#else
    Rval = AmbaLL_CmdSend(WriteMode, pCmdData, (UINT32)sizeof(cmd_dsp_set_profile_t));
#endif
    if ((Rval == OK) && (AmbaLL_CmdLogChk(CMD_DSP_SET_PROFILE) == CMD_ON)){
        dump_cmd_dsp_set_profile_t(pCmdData, &Info);
    }
    return Rval;
}

UINT32 AmbaLL_CmdDspEncFlowMaxCfg(UINT8 WriteMode, cmd_dsp_enc_flow_max_cfg_t *pCmdData, const UINT16 MaxStrmNum)
{
    UINT32 Rval;
    CmdInfo_t Info = {.WriteMode = WriteMode};

    pCmdData->cmd_code = CMD_DSP_ENC_FLOW_MAX_CFG;
    Rval = AmbaLL_CmdSend(WriteMode, pCmdData, (UINT32)sizeof(cmd_dsp_enc_flow_max_cfg_t));
    if ((Rval == OK) && (AmbaLL_CmdLogChk(CMD_DSP_ENC_FLOW_MAX_CFG) == CMD_ON)){
        dump_cmd_dsp_enc_flow_max_cfg_t(pCmdData, &Info, MaxStrmNum);
    }
    return Rval;
}

UINT32 AmbaLL_CmdDspDecFlowMaxCfg(UINT8 WriteMode, cmd_dsp_dec_flow_max_cfg_t *pCmdData)
{
    UINT32 Rval;
    CmdInfo_t Info = {.WriteMode = WriteMode};

    pCmdData->cmd_code = CMD_DSP_DEC_FLOW_MAX_CFG;
    Rval = AmbaLL_CmdSend(WriteMode, pCmdData, (UINT32)sizeof(cmd_dsp_dec_flow_max_cfg_t));
    if ((Rval == OK) && (AmbaLL_CmdLogChk(CMD_DSP_DEC_FLOW_MAX_CFG) == CMD_ON)){
        dump_cmd_dsp_dec_flow_max_cfg_t(pCmdData, &Info);
    }
    return Rval;
}

#if 0
UINT32 AmbaLL_CmdDspNOP(UINT8 WriteMode, dsp_cmd_t *pCmdData)
{
    UINT32 Rval;
    CmdInfo_t Info = {.WriteMode = WriteMode};

    pCmdData->cmd_code = CMD_DSP_NOP;
    Rval = AmbaLL_CmdSend(WriteMode, pCmdData, (UINT32)sizeof(dsp_cmd_t));
    if ((Rval == OK) && (AmbaLL_CmdLogChk(CMD_DSP_NOP) == CMD_ON)){
        dump_cmd_dsp_nop_t(pCmdData, &Info);
    }
    return Rval;
}
#endif

/*-----------------------------------------------------------------------------------------------*\
 * CAT_VPROC (2)
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaLL_CmdVprocConfig(UINT8 WriteMode, cmd_vproc_cfg_t *pCmdData)
{
    UINT32 Rval;
    CmdInfo_t Info = {.WriteMode = WriteMode};

    pCmdData->cmd_code = CMD_VPROC_CONFIG;
    Rval = AmbaLL_CmdSend(WriteMode, pCmdData, (UINT32)sizeof(cmd_vproc_cfg_t));
    if ((Rval == OK) && (AmbaLL_CmdLogChk(CMD_VPROC_CONFIG) == CMD_ON)){
        dump_cmd_vproc_cfg_t(pCmdData, &Info);
    }
    return Rval;
}

UINT32 AmbaLL_CmdVprocSetup(UINT8 WriteMode, cmd_vproc_setup_t *pCmdData)
{
    UINT32 Rval;
    CmdInfo_t Info = {.WriteMode = WriteMode};

    pCmdData->cmd_code = CMD_VPROC_SETUP;
    Rval = AmbaLL_CmdSend(WriteMode, pCmdData, (UINT32)sizeof(cmd_vproc_setup_t));
    if ((Rval == OK) && (AmbaLL_CmdLogChk(CMD_VPROC_SETUP) == CMD_ON)){
        dump_cmd_vproc_setup_t(pCmdData, &Info);
    }
    return Rval;
}

UINT32 AmbaLL_CmdVprocIkConfig(UINT8 WriteMode, cmd_vproc_ik_config_t *pCmdData)
{
    UINT32 Rval;
    CmdInfo_t Info = {.WriteMode = WriteMode};
    pCmdData->cmd_code = CMD_VPROC_IK_CONFIG;
    Rval = AmbaLL_CmdSend(WriteMode, pCmdData, (UINT32)sizeof(cmd_vproc_ik_config_t));

    if ((Rval == OK) && (AmbaLL_CmdLogChk(CMD_VPROC_IK_CONFIG) == CMD_ON)){
        dump_cmd_vproc_ik_config_t(pCmdData, &Info);
    }
    return Rval;
}

UINT32 AmbaLL_CmdVprocImgPrmdSetup(UINT8 WriteMode, cmd_vproc_img_pyramid_setup_t *pCmdData)
{
    UINT32 Rval;
    CmdInfo_t Info = {.WriteMode = WriteMode};

    pCmdData->cmd_code = CMD_VPROC_IMG_PRMD_SETUP;
    Rval = AmbaLL_CmdSend(WriteMode, pCmdData, (UINT32)sizeof(cmd_vproc_img_pyramid_setup_t));
    if ((Rval == OK) && (AmbaLL_CmdLogChk(CMD_VPROC_IMG_PRMD_SETUP) == CMD_ON)){
        dump_cmd_vproc_img_prmd_setup_t(pCmdData, &Info);
    }
    return Rval;
}

UINT32 AmbaLL_CmdVprocPrevSetup(UINT8 WriteMode, cmd_vproc_prev_setup_t *pCmdData)
{
    UINT32 Rval;
    CmdInfo_t Info = {.WriteMode = WriteMode};

    pCmdData->cmd_code = CMD_VPROC_PREV_SETUP;
    Rval = AmbaLL_CmdSend(WriteMode, pCmdData, (UINT32)sizeof(cmd_vproc_prev_setup_t));
    if ((Rval == OK) && (AmbaLL_CmdLogChk(CMD_VPROC_PREV_SETUP) == CMD_ON)){
        dump_cmd_vproc_prev_setup_t(pCmdData, &Info);
    }
    return Rval;
}

UINT32 AmbaLL_CmdVprocLaneDetSetup(UINT8 WriteMode, cmd_vproc_lane_det_setup_t *pCmdData)
{
    UINT32 Rval;
    CmdInfo_t Info = {.WriteMode = WriteMode};

    pCmdData->cmd_code = CMD_VPROC_LN_DET_SETUP;
    Rval = AmbaLL_CmdSend(WriteMode, pCmdData, (UINT32)sizeof(cmd_vproc_lane_det_setup_t));
    if ((Rval == OK) && (AmbaLL_CmdLogChk(CMD_VPROC_LN_DET_SETUP) == CMD_ON)){
        dump_cmd_vproc_lane_det_setup_t(pCmdData, &Info);
    }
    return Rval;
}

UINT32 AmbaLL_CmdVprocSetExtMem(UINT8 WriteMode, cmd_vproc_set_ext_mem_t *pCmdData)
{
    UINT32 Rval;
    CmdInfo_t Info = {.WriteMode = WriteMode};

    pCmdData->cmd_code = CMD_VPROC_SET_EXT_MEM;
    Rval = AmbaLL_CmdSend(WriteMode, pCmdData, (UINT32)sizeof(cmd_vproc_set_ext_mem_t));
    if ((Rval == OK) && (AmbaLL_CmdLogChk(CMD_VPROC_SET_EXT_MEM) == CMD_ON)){
        dump_cmd_vproc_set_ext_mem_t(pCmdData, &Info);
    }
    return Rval;
}

UINT32 AmbaLL_CmdVprocStop(UINT8 WriteMode, cmd_vproc_stop_t *pCmdData)
{
    UINT32 Rval;
    CmdInfo_t Info = {.WriteMode = WriteMode};

    pCmdData->cmd_code = CMD_VPROC_STOP;
    Rval = AmbaLL_CmdSend(WriteMode, pCmdData, (UINT32)sizeof(cmd_vproc_stop_t));
    if ((Rval == OK) && (AmbaLL_CmdLogChk(CMD_VPROC_STOP) == CMD_ON)){
        dump_cmd_vproc_stop_t(pCmdData, &Info);
    }
    return Rval;
}

UINT32 AmbaLL_CmdVprocOsdBlend(UINT8 WriteMode, cmd_vproc_osd_blend_t *pCmdData)
{
    UINT32 Rval;
    CmdInfo_t Info = {.WriteMode = WriteMode};

    pCmdData->cmd_code = CMD_VPROC_OSD_BLEND;
    Rval = AmbaLL_CmdSend(WriteMode, pCmdData, (UINT32)sizeof(cmd_vproc_osd_blend_t));
    if ((Rval == OK) && (AmbaLL_CmdLogChk(CMD_VPROC_OSD_BLEND) == CMD_ON)){
        dump_cmd_vproc_osb_blend_t(pCmdData, &Info);
    }
    return Rval;
}

UINT32 AmbaLL_CmdVprocPinOutDecimation(UINT8 WriteMode, cmd_vproc_pin_out_deci_t *pCmdData)
{
    UINT32 Rval;
    CmdInfo_t Info = {.WriteMode = WriteMode};

    pCmdData->cmd_code = CMD_VPROC_PIN_OUT_DECIMATION;
    Rval = AmbaLL_CmdSend(WriteMode, pCmdData, (UINT32)sizeof(cmd_vproc_pin_out_deci_t));
    if ((Rval == OK) && (AmbaLL_CmdLogChk(CMD_VPROC_PIN_OUT_DECIMATION) == CMD_ON)){
        dump_cmd_vproc_pin_out_deci_t(pCmdData, &Info);
    }
    return Rval;
}

UINT32 AmbaLL_CmdVprocGrpCmd(UINT8 WriteMode, cmd_vproc_fov_grp_cmd_t *pCmdData)
{
    UINT32 Rval;
    CmdInfo_t Info = {.WriteMode = WriteMode};

    pCmdData->cmd_code = CMD_VPROC_GRP_CMD;
    Rval = AmbaLL_CmdSend(WriteMode, pCmdData, (UINT32)sizeof(cmd_vproc_fov_grp_cmd_t));
    if ((Rval == OK) && (AmbaLL_CmdLogChk(CMD_VPROC_GRP_CMD) == CMD_ON)){
        dump_cmd_vproc_fov_grp_cmd_t(pCmdData, &Info);
    }
    return Rval;
}
#if 0
UINT32 AmbaLL_CmdVprocEchoCmd(UINT8 WriteMode, cmd_vproc_echo_t *pCmdData)
{
    UINT32 Rval;
    CmdInfo_t Info = {.WriteMode = WriteMode};

    pCmdData->cmd_code = CMD_VPROC_ECHO;
    Rval = AmbaLL_CmdSend(WriteMode, pCmdData, (UINT32)sizeof(cmd_vproc_echo_t));
    if ((Rval == OK) && (AmbaLL_CmdLogChk(CMD_VPROC_ECHO) == CMD_ON)){
        dump_cmd_vproc_echo_cmd_t(pCmdData, &Info);
    }
    return Rval;
}

UINT32 AmbaLL_CmdVprocWarpGroupCmd(UINT8 WriteMode, cmd_vproc_warp_group_update_t *pCmdData)
{
    UINT32 Rval;
    CmdInfo_t Info = {.WriteMode = WriteMode};
    pCmdData->cmd_code = CMD_VPROC_WARP_GROUP_UPDATE;
    Rval = AmbaLL_CmdSend(WriteMode, pCmdData, (UINT32)sizeof(cmd_vproc_warp_group_update_t));
    if ((Rval == OK) && (AmbaLL_CmdLogChk(CMD_VPROC_WARP_GROUP_UPDATE) == CMD_ON)){
        dump_cmd_vproc_warp_group_cmd_t(pCmdData, &Info);
    }
    return Rval;
}
#endif
UINT32 AmbaLL_CmdVprocMultiStrmPpCmd(UINT8 WriteMode, cmd_vproc_multi_stream_pp_t *pCmdData)
{
    UINT32 Rval;
    CmdInfo_t Info = {.WriteMode = WriteMode};

    pCmdData->cmd_code = CMD_VPROC_MULTI_STREAM_PP;
    Rval = AmbaLL_CmdSend(WriteMode, pCmdData, (UINT32)sizeof(cmd_vproc_multi_stream_pp_t));
    if ((Rval == OK) && (AmbaLL_CmdLogChk(CMD_VPROC_MULTI_STREAM_PP) == CMD_ON)){
        dump_cmd_vproc_multi_stream_pp_t(pCmdData, &Info);
    }
    return Rval;
}
#if 0
UINT32 AmbaLL_CmdVprocSetEffectBufImgSz(UINT8 WriteMode, cmd_vproc_set_effect_buf_img_sz_t *pCmdData)
{
    UINT32 Rval;
    CmdInfo_t Info = {.WriteMode = WriteMode};

    pCmdData->cmd_code = CMD_VPROC_SET_EFFECT_BUF_IMG_SZ;
    Rval = AmbaLL_CmdSend(WriteMode, pCmdData, (UINT32)sizeof(cmd_vproc_set_effect_buf_img_sz_t));
    if ((Rval == OK) && (AmbaLL_CmdLogChk(CMD_VPROC_SET_EFFECT_BUF_IMG_SZ) == CMD_ON)){
        dump_cmd_vproc_set_effect_buf_img_sz_t(pCmdData, &Info);
    }
    return Rval;
}
#endif
UINT32 AmbaLL_CmdVprocMultiChanProcOrder(UINT8 WriteMode, cmd_vproc_multi_chan_proc_order_t *pCmdData)
{
    UINT32 Rval;
    CmdInfo_t Info = {.WriteMode = WriteMode};

    pCmdData->cmd_code = CMD_VPROC_MULTI_CHAN_PROC_ORDER;
    Rval = AmbaLL_CmdSend(WriteMode, pCmdData, (UINT32)sizeof(cmd_vproc_multi_chan_proc_order_t));
    if ((Rval == OK) && (AmbaLL_CmdLogChk(CMD_VPROC_MULTI_CHAN_PROC_ORDER) == CMD_ON)){
        dump_cmd_vproc_multi_chan_proc_order_t(pCmdData, &Info);
    }
    return Rval;
}

UINT32 AmbaLL_CmdVprocSetVprocGrping(UINT8 WriteMode, cmd_vproc_set_vproc_grping *pCmdData)
{
    UINT32 Rval;
    CmdInfo_t Info = {.WriteMode = WriteMode};

    pCmdData->cmd_code = CMD_VPROC_SET_VPROC_GRPING;
    Rval = AmbaLL_CmdSend(WriteMode, pCmdData, (UINT32)sizeof(cmd_vproc_set_vproc_grping));
    if ((Rval == OK) && (AmbaLL_CmdLogChk(CMD_VPROC_SET_VPROC_GRPING) == CMD_ON)){
        dump_cmd_vproc_set_vproc_grping_t(pCmdData, &Info);
    }
    return Rval;
}

UINT32 AmbaLL_CmdVprocSetTestFrame(UINT8 WriteMode, cmd_vproc_set_testframe_t *pCmdData)
{
    UINT32 Rval;
    CmdInfo_t Info = {.WriteMode = WriteMode};
    pCmdData->cmd_code = CMD_VPROC_SET_TESTFRAME;
    Rval = AmbaLL_CmdSend(WriteMode, pCmdData, (UINT32)sizeof(cmd_vproc_set_testframe_t));
    if ((Rval == OK) && (AmbaLL_CmdLogChk(CMD_VPROC_SET_TESTFRAME) == CMD_ON)){
        dump_cmd_vproc_set_testframe_t(pCmdData, &Info);
    }
    return Rval;
}

/*-----------------------------------------------------------------------------------------------*\
 * CAT_STILL (3)
\*-----------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------*\
 * CAT_VIN (4)
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaLL_CmdVinStart(UINT8 WriteMode, cmd_vin_start_t *pCmdData)
{
    UINT32 Rval;
    CmdInfo_t Info = {.WriteMode = WriteMode};

    pCmdData->cmd_code = CMD_VIN_START;
    Rval = AmbaLL_CmdSend(WriteMode, pCmdData, (UINT32)sizeof(cmd_vin_start_t));
    if ((Rval == OK) && (AmbaLL_CmdLogChk(CMD_VIN_START) == CMD_ON)){
        dump_cmd_vin_start_t(pCmdData, &Info);
    }
    return Rval;
}

UINT32 AmbaLL_CmdVinIdle(UINT8 WriteMode, cmd_vin_idle_t *pCmdData)
{
    UINT32 Rval;
    CmdInfo_t Info = {.WriteMode = WriteMode};

    pCmdData->cmd_code = CMD_VIN_IDLE;
    Rval = AmbaLL_CmdSend(WriteMode, pCmdData, (UINT32)sizeof(cmd_vin_idle_t));
    if ((Rval == OK) && (AmbaLL_CmdLogChk(CMD_VIN_IDLE) == CMD_ON)){
        dump_cmd_vin_idle_t(pCmdData, &Info);
    }
    return Rval;
}

#ifdef SUPPORT_DSP_SET_RAW_CAP_CNT
UINT32 AmbaLL_CmdVinSetRawFrmCapCnt(UINT8 WriteMode, cmd_vin_set_raw_frm_cap_cnt_t *pCmdData)
{
    UINT32 Rval;
    CmdInfo_t Info = {.WriteMode = WriteMode};

    pCmdData->cmd_code = CMD_VIN_SET_RAW_FRM_CAP_CNT;
    Rval = AmbaLL_CmdSend(WriteMode, pCmdData, (UINT32)sizeof(cmd_vin_set_raw_frm_cap_cnt_t));
    if ((Rval == OK) && (AmbaLL_CmdLogChk(CMD_VIN_SET_RAW_FRM_CAP_CNT) == CMD_ON)){
        dump_cmd_vin_set_raw_frm_cap_cnt_t(pCmdData, &Info);
    }
    return Rval;
}
#endif

UINT32 AmbaLL_CmdVinSetExtMem(UINT8 WriteMode, cmd_vin_set_ext_mem_t *pCmdData)
{
    UINT32 Rval;
    CmdInfo_t Info = {.WriteMode = WriteMode};

    pCmdData->cmd_code = CMD_VIN_SET_EXT_MEM;
    Rval = AmbaLL_CmdSend(WriteMode, pCmdData, (UINT32)sizeof(cmd_vin_set_ext_mem_t));
    if ((Rval == OK) && (AmbaLL_CmdLogChk(CMD_VIN_SET_EXT_MEM) == CMD_ON)){
        dump_cmd_vin_set_ext_mem_t(pCmdData, &Info);
    }
    return Rval;
}

UINT32 AmbaLL_CmdVinSendInputData(UINT8 WriteMode, cmd_vin_send_input_data_t *pCmdData)
{
    UINT32 Rval;
    CmdInfo_t Info = {.WriteMode = WriteMode};

    pCmdData->cmd_code = CMD_VIN_SEND_INPUT_DATA;
    Rval = AmbaLL_CmdSend(WriteMode, pCmdData, (UINT32)sizeof(cmd_vin_send_input_data_t));
    if ((Rval == OK) && (AmbaLL_CmdLogChk(CMD_VIN_SEND_INPUT_DATA) == CMD_ON)){
        dump_cmd_vin_send_input_data_t(pCmdData, &Info);
    }
    return Rval;
}

UINT32 AmbaLL_CmdVinInitiateRawCapToExtBuf(UINT8 WriteMode, cmd_vin_initiate_raw_cap_to_ext_buf_t *pCmdData)
{
    UINT32 Rval;
    CmdInfo_t Info = {.WriteMode = WriteMode};

    pCmdData->cmd_code = CMD_VIN_INITIATE_RAW_CAP_TO_EXT_BUF;
    Rval = AmbaLL_CmdSend(WriteMode, pCmdData, (UINT32)sizeof(cmd_vin_initiate_raw_cap_to_ext_buf_t));
    if ((Rval == OK) && (AmbaLL_CmdLogChk(CMD_VIN_INITIATE_RAW_CAP_TO_EXT_BUF) == CMD_ON)){
        dump_cmd_vin_initiate_raw_cap_to_ext_buf_t(pCmdData, &Info);
    }
    return Rval;
}

UINT32 AmbaLL_CmdVinAttachCfgToCapFrm(UINT8 WriteMode, cmd_vin_attach_proc_cfg_to_cap_frm_t *pCmdData)
{
    UINT32 Rval;
    CmdInfo_t Info = {.WriteMode = WriteMode};

    pCmdData->cmd_code = CMD_VIN_ATTACH_PROC_CFG_TO_CAP_FRM;
    Rval = AmbaLL_CmdSend(WriteMode, pCmdData, (UINT32)sizeof(cmd_vin_attach_proc_cfg_to_cap_frm_t));
    if ((Rval == OK) && (AmbaLL_CmdLogChk(CMD_VIN_ATTACH_PROC_CFG_TO_CAP_FRM) == CMD_ON)){
        dump_cmd_vin_attach_proc_cfg_to_cap_frm_t(pCmdData, &Info);
    }
    return Rval;
}

UINT32 AmbaLL_CmdVinAttachEventToRaw(UINT8 WriteMode, cmd_vin_attach_event_to_raw_t *pCmdData)
{
    UINT32 Rval;
    CmdInfo_t Info = {.WriteMode = WriteMode};

    pCmdData->cmd_code = CMD_VIN_ATTACH_EVENT_TO_RAW;
    Rval = AmbaLL_CmdSend(WriteMode, pCmdData, (UINT32)sizeof(cmd_vin_attach_event_to_raw_t));
    if ((Rval == OK) && (AmbaLL_CmdLogChk(CMD_VIN_ATTACH_EVENT_TO_RAW) == CMD_ON)){
        dump_cmd_vin_attach_event_to_raw_t(pCmdData, &Info);
    }
    return Rval;
}

UINT32 AmbaLL_CmdVinMsgDecRate(UINT8 WriteMode, cmd_vin_cmd_msg_dec_rate_t *pCmdData)
{
    UINT32 Rval;
    CmdInfo_t Info = {.WriteMode = WriteMode};

    pCmdData->cmd_code = CMD_VIN_CMD_MSG_DECIMATION_RATE;
    Rval = AmbaLL_CmdSend(WriteMode, pCmdData, (UINT32)sizeof(cmd_vin_cmd_msg_dec_rate_t));
    if ((Rval == OK) && (AmbaLL_CmdLogChk(CMD_VIN_CMD_MSG_DECIMATION_RATE) == CMD_ON)){
        dump_cmd_vin_cmd_msg_dec_rate_t(pCmdData, &Info);
    }
    return Rval;
}

UINT32 AmbaLL_CmdVinCeSetup(UINT8 WriteMode, cmd_vin_ce_setup_t *pCmdData)
{
    UINT32 Rval;
    CmdInfo_t Info = {.WriteMode = WriteMode};

    pCmdData->cmd_code = CMD_VIN_CE_SETUP;
    Rval = AmbaLL_CmdSend(WriteMode, pCmdData, (UINT32)sizeof(cmd_vin_ce_setup_t));
    if ((Rval == OK) && (AmbaLL_CmdLogChk(CMD_VIN_CE_SETUP) == CMD_ON)){
        dump_cmd_vin_ce_setup_t(pCmdData, &Info);
    }
    return Rval;
}

UINT32 AmbaLL_CmdVinHdrSetup(UINT8 WriteMode, cmd_vin_hdr_setup_t *pCmdData)
{
    UINT32 Rval;
    CmdInfo_t Info = {.WriteMode = WriteMode};

    pCmdData->cmd_code = CMD_VIN_HDR_SETUP;
    Rval = AmbaLL_CmdSend(WriteMode, pCmdData, (UINT32)sizeof(cmd_vin_hdr_setup_t));
    if ((Rval == OK) && (AmbaLL_CmdLogChk(CMD_VIN_HDR_SETUP) == CMD_ON)){
        dump_cmd_vin_hdr_setup_t(pCmdData, &Info);
    }
    return Rval;
}

UINT32 AmbaLL_CmdVinSetFrmLvlFlipRotControl(UINT8 WriteMode, cmd_vin_set_frm_lvl_flip_rot_control_t *pCmdData)
{
    UINT32 Rval;
    CmdInfo_t Info = {.WriteMode = WriteMode};

    pCmdData->cmd_code = CMD_VIN_SET_FRM_LVL_FLIP_ROT_CONTROL;
    Rval = AmbaLL_CmdSend(WriteMode, pCmdData, (UINT32)sizeof(cmd_vin_set_frm_lvl_flip_rot_control_t));
    if ((Rval == OK) && (AmbaLL_CmdLogChk(CMD_VIN_SET_FRM_LVL_FLIP_ROT_CONTROL) == CMD_ON)){
        dump_cmd_vin_set_frm_lvl_flip_rot_control_t(pCmdData, &Info);
    }
    return Rval;
}

UINT32 AmbaLL_CmdVinSetFovLayout(UINT8 WriteMode, cmd_vin_set_fov_layout_t *pCmdData)
{
    UINT32 Rval;
    CmdInfo_t Info = {.WriteMode = WriteMode};

    pCmdData->cmd_code = CMD_VIN_SET_FOV_LAYOUT;
    Rval = AmbaLL_CmdSend(WriteMode, pCmdData, (UINT32)sizeof(cmd_vin_set_fov_layout_t));
    if ((Rval == OK) && (AmbaLL_CmdLogChk(CMD_VIN_SET_FOV_LAYOUT) == CMD_ON)){
        dump_cmd_vin_set_fov_layout_t(pCmdData, &Info);
    }
    return Rval;
}

UINT32 AmbaLL_CmdVinSetFrmVprocDelay(UINT8 WriteMode, cmd_vin_set_frm_vproc_delay_t *pCmdData)
{
    UINT32 Rval;
    CmdInfo_t Info = {.WriteMode = WriteMode};

    pCmdData->cmd_code = CMD_VIN_SET_FRM_VPROC_DELAY;
    Rval = AmbaLL_CmdSend(WriteMode, pCmdData, (UINT32)sizeof(cmd_vin_set_frm_vproc_delay_t));
    if ((Rval == OK) && (AmbaLL_CmdLogChk(CMD_VIN_SET_FRM_VPROC_DELAY) == CMD_ON)){
        dump_cmd_vin_set_frm_vproc_delay_t(pCmdData, &Info);
    }
    return Rval;
}

UINT32 AmbaLL_CmdVinAttachSbToCapFrm(UINT8 WriteMode, cmd_vin_attach_sideband_info_to_cap_frm_t *pCmdData)
{
    UINT32 Rval;
    CmdInfo_t Info = {.WriteMode = WriteMode};

    pCmdData->cmd_code = CMD_VIN_ATTACH_SIDEBAND_INFO_TO_CAP_FRM;
    Rval = AmbaLL_CmdSend(WriteMode, pCmdData, (UINT32)sizeof(cmd_vin_attach_sideband_info_to_cap_frm_t));
    if ((Rval == OK) && (AmbaLL_CmdLogChk(CMD_VIN_ATTACH_SIDEBAND_INFO_TO_CAP_FRM) == CMD_ON)){
        dump_cmd_vin_attach_sideband_info_to_cap_frm_t(pCmdData, &Info);
    }
    return Rval;
}

UINT32 AmbaLL_CmdVinVoutLockSetup(UINT8 WriteMode, cmd_vin_vout_lock_setup_t *pCmdData)
{
    UINT32 Rval;
    CmdInfo_t Info = {.WriteMode = WriteMode};

    pCmdData->cmd_code = CMD_VIN_VOUT_LOCK_SETUP;
    Rval = AmbaLL_CmdSend(WriteMode, pCmdData, (UINT32)sizeof(cmd_vin_vout_lock_setup_t));
    if ((Rval == OK) && (AmbaLL_CmdLogChk(CMD_VIN_VOUT_LOCK_SETUP) == CMD_ON)){
        dump_cmd_vin_vout_lock_setup_t(pCmdData, &Info);
    }
    return Rval;
}
#if 0
UINT32 AmbaLL_CmdVinAttachMetadata(UINT8 WriteMode, cmd_vin_attach_metadata_t *pCmdData)
{
    UINT32 Rval;
    CmdInfo_t Info = {.WriteMode = WriteMode};

    pCmdData->cmd_code = CMD_VIN_ATTACH_METADATA;
    Rval = AmbaLL_CmdSend(WriteMode, pCmdData, (UINT32)sizeof(cmd_vin_attach_metadata_t));
    if ((Rval == OK) && (AmbaLL_CmdLogChk(CMD_VIN_ATTACH_METADATA) == CMD_ON)){
        dump_cmd_vin_attach_metadata_t(pCmdData, &Info);
    }
    return Rval;
}
#endif

UINT32 AmbaLL_CmdVinTemporalDemuxSetup(UINT8 WriteMode, cmd_vin_temporal_demux_setup_t *pCmdData)
{
    UINT32 Rval;
    CmdInfo_t Info = {.WriteMode = WriteMode};

    pCmdData->cmd_code = CMD_VIN_TEMPORAL_DEMUX_SETUP;
    Rval = AmbaLL_CmdSend(WriteMode, pCmdData, (UINT32)sizeof(cmd_vin_temporal_demux_setup_t));
    if ((Rval == OK) && (AmbaLL_CmdLogChk(CMD_VIN_TEMPORAL_DEMUX_SETUP) == CMD_ON)){
        dump_cmd_vin_temporal_demux_setup_t(pCmdData, &Info);
    }
    return Rval;
}

/*-----------------------------------------------------------------------------------------------*\
 * CAT_VOUT (5)
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaLL_CmdVoutMixerSetup(UINT8 WriteMode, cmd_vout_mixer_setup_t *pCmdData)
{
    UINT32 Rval;
    CmdInfo_t Info = {.WriteMode = WriteMode};

    pCmdData->cmd_code = CMD_VOUT_MIXER_SETUP;
    Rval = AmbaLL_CmdSend(WriteMode, pCmdData, (UINT32)sizeof(cmd_vout_mixer_setup_t));
    if ((Rval == OK) && (AmbaLL_CmdLogChk(CMD_VOUT_MIXER_SETUP) == CMD_ON)){
        dump_cmd_vout_mixer_setup_t(pCmdData, &Info);
    }
    return Rval;
}

UINT32 AmbaLL_CmdVoutVideoSetup(UINT8 WriteMode, cmd_vout_video_setup_t *pCmdData)
{
    UINT32 Rval;
    CmdInfo_t Info = {.WriteMode = WriteMode};

    pCmdData->cmd_code = CMD_VOUT_VIDEO_SETUP;
    Rval = AmbaLL_CmdSend(WriteMode, pCmdData, (UINT32)sizeof(cmd_vout_video_setup_t));
    if ((Rval == OK) && (AmbaLL_CmdLogChk(CMD_VOUT_VIDEO_SETUP) == CMD_ON)){
        dump_cmd_vout_video_setup_t(pCmdData, &Info);
    }
    return Rval;
}
#if 0
UINT32 AmbaLL_CmdVoutDefaultImgSetup(UINT8 WriteMode, cmd_vout_default_img_setup_t *pCmdData)
{
    UINT32 Rval;
    CmdInfo_t Info = {.WriteMode = WriteMode};

    pCmdData->cmd_code = CMD_VOUT_DEFAULT_IMG_SETUP;
    Rval = AmbaLL_CmdSend(WriteMode, pCmdData, (UINT32)sizeof(cmd_vout_default_img_setup_t));
    if ((Rval == OK) && (AmbaLL_CmdLogChk(CMD_VOUT_DEFAULT_IMG_SETUP) == CMD_ON)){
        dump_cmd_vout_default_img_setup_t(pCmdData, &Info);
    }
    return Rval;
}
#endif

UINT32 AmbaLL_CmdVoutOsdSetup(UINT8 WriteMode, cmd_vout_osd_setup_t *pCmdData)
{
    UINT32 Rval;
    CmdInfo_t Info = {.WriteMode = WriteMode};

    pCmdData->cmd_code = CMD_VOUT_OSD_SETUP;
    Rval = AmbaLL_CmdSend(WriteMode, pCmdData, (UINT32)sizeof(cmd_vout_osd_setup_t));
    if ((Rval == OK) && (AmbaLL_CmdLogChk(CMD_VOUT_OSD_SETUP) == CMD_ON)){
        dump_cmd_vout_osd_setup_t(pCmdData, &Info);
    }
    return Rval;
}
#if 0
UINT32 AmbaLL_CmdVoutOsdBufferSetup(UINT8 WriteMode, cmd_vout_osd_buf_setup_t *pCmdData)
{
    UINT32 Rval;
    CmdInfo_t Info = {.WriteMode = WriteMode};

    pCmdData->cmd_code = CMD_VOUT_OSD_BUFFER_SETUP;
    Rval = AmbaLL_CmdSend(WriteMode, pCmdData, (UINT32)sizeof(cmd_vout_osd_buf_setup_t));
    if ((Rval == OK) && (AmbaLL_CmdLogChk(CMD_VOUT_OSD_BUFFER_SETUP) == CMD_ON)){
        dump_cmd_vout_osd_buf_setup_t(pCmdData, &Info);
    }
    return Rval;
}
#endif
UINT32 AmbaLL_CmdVoutOsdClutSetup(UINT8 WriteMode, cmd_vout_osd_clut_setup_t *pCmdData)
{
    UINT32 Rval;
    CmdInfo_t Info = {.WriteMode = WriteMode};

    pCmdData->cmd_code = CMD_VOUT_OSD_CLUT_SETUP;
    Rval = AmbaLL_CmdSend(WriteMode, pCmdData, (UINT32)sizeof(cmd_vout_osd_clut_setup_t));
    if ((Rval == OK) && (AmbaLL_CmdLogChk(CMD_VOUT_OSD_CLUT_SETUP) == CMD_ON)){
        dump_cmd_vout_osd_clut_setup_t(pCmdData, &Info);
    }
    return Rval;
}

UINT32 AmbaLL_CmdVoutDisplaySetup(UINT8 WriteMode, cmd_vout_display_setup_t *pCmdData)
{
    UINT32 Rval;
    CmdInfo_t Info = {.WriteMode = WriteMode};

    pCmdData->cmd_code = CMD_VOUT_DISPLAY_SETUP;
    Rval = AmbaLL_CmdSend(WriteMode, pCmdData, (UINT32)sizeof(cmd_vout_display_setup_t));
    if ((Rval == OK) && (AmbaLL_CmdLogChk(CMD_VOUT_DISPLAY_SETUP) == CMD_ON)){
        dump_cmd_vout_display_setup_t(pCmdData, &Info);
    }
    return Rval;
}

UINT32 AmbaLL_CmdVoutReset(UINT8 WriteMode, cmd_vout_reset_t *pCmdData)
{
    UINT32 Rval;
    CmdInfo_t Info = {.WriteMode = WriteMode};

    pCmdData->cmd_code = CMD_VOUT_RESET;
    Rval = AmbaLL_CmdSend(WriteMode, pCmdData, (UINT32)sizeof(cmd_vout_reset_t));
    if ((Rval == OK) && (AmbaLL_CmdLogChk(CMD_VOUT_RESET) == CMD_ON)){
        dump_cmd_vout_reset_t(pCmdData, &Info);
    }
    return Rval;
}
#if 0
UINT32 AmbaLL_CmdVoutDisplayCscSetup(UINT8 WriteMode, cmd_vout_display_csc_setup_t *pCmdData)
{
    UINT32 Rval;
    CmdInfo_t Info = {.WriteMode = WriteMode};

    pCmdData->cmd_code = CMD_VOUT_DISPLAY_CSC_SETUP;
    Rval = AmbaLL_CmdSend(WriteMode, pCmdData, (UINT32)sizeof(cmd_vout_display_csc_setup_t));
    if ((Rval == OK) && (AmbaLL_CmdLogChk(CMD_VOUT_DISPLAY_CSC_SETUP) == CMD_ON)){
        dump_cmd_vout_display_csc_setup_t(pCmdData, &Info);
    }
    return Rval;
}

UINT32 AmbaLL_CmdVoutDigitalOutputModeSetup(UINT8 WriteMode, cmd_vout_digital_output_mode_setup_t *pCmdData)
{
    UINT32 Rval;
    CmdInfo_t Info = {.WriteMode = WriteMode};

    pCmdData->cmd_code = CMD_VOUT_DIGITAL_OUTPUT_MODE_SETUP;
    Rval = AmbaLL_CmdSend(WriteMode, pCmdData, (UINT32)sizeof(cmd_vout_digital_output_mode_setup_t));
    if ((Rval == OK) && (AmbaLL_CmdLogChk(CMD_VOUT_DIGITAL_OUTPUT_MODE_SETUP) == CMD_ON)){
        dump_cmd_vout_digital_output_mode_setup_t(pCmdData, &Info);
    }
    return Rval;
}
#endif
UINT32 AmbaLL_CmdVoutGammaSetup(UINT8 WriteMode, cmd_vout_gamma_setup_t *pCmdData)
{
    UINT32 Rval;
    CmdInfo_t Info = {.WriteMode = WriteMode};

    pCmdData->cmd_code = CMD_VOUT_GAMMA_SETUP;
    Rval = AmbaLL_CmdSend(WriteMode, pCmdData, (UINT32)sizeof(cmd_vout_gamma_setup_t));
    if ((Rval == OK) && (AmbaLL_CmdLogChk(CMD_VOUT_GAMMA_SETUP) == CMD_ON)){
        dump_cmd_vout_gamma_setup_t(pCmdData, &Info);
    }
    return Rval;
}

/*-----------------------------------------------------------------------------------------------*\
 * CAT_ENC (6)
\*-----------------------------------------------------------------------------------------------*/


UINT32 AmbaLL_CmdEncoderSetup(UINT8 WriteMode, cmd_encoder_setup_t *pCmdData)
{
    UINT32 Rval;
    CmdInfo_t Info = {.WriteMode = WriteMode};

    pCmdData->cmd_code = CMD_ENCODER_SETUP;
    Rval = AmbaLL_CmdSend(WriteMode, pCmdData, (UINT32)sizeof(cmd_encoder_setup_t));
    if ((Rval == OK) && (AmbaLL_CmdLogChk(CMD_ENCODER_SETUP) == CMD_ON)){
        dump_cmd_encoder_setup_t(pCmdData, &Info);
    }
    return Rval;
}

UINT32 AmbaLL_CmdEncoderStart(UINT8 WriteMode, cmd_encoder_start_t *pCmdData)
{
    UINT32 Rval;
    CmdInfo_t Info = {.WriteMode = WriteMode};

    pCmdData->cmd_code = CMD_ENCODER_START;
    Rval = AmbaLL_CmdSend(WriteMode, pCmdData, (UINT32)sizeof(cmd_encoder_start_t));
    if ((Rval == OK) && (AmbaLL_CmdLogChk(CMD_ENCODER_START) == CMD_ON)){
        dump_cmd_encoder_start_t(pCmdData, &Info);
    }
    return Rval;
}

UINT32 AmbaLL_CmdEncoderStop(UINT8 WriteMode, cmd_encoder_stop_t *pCmdData)
{
    UINT32 Rval;
    CmdInfo_t Info = {.WriteMode = WriteMode};

    pCmdData->cmd_code = CMD_ENCODER_STOP;
    Rval = AmbaLL_CmdSend(WriteMode, pCmdData, (UINT32)sizeof(cmd_encoder_stop_t));
    if ((Rval == OK) && (AmbaLL_CmdLogChk(CMD_ENCODER_STOP) == CMD_ON)){
        dump_cmd_encoder_stop_t(pCmdData, &Info);
    }
    return Rval;
}

UINT32 AmbaLL_CmdEncoderRealtimeSetup(UINT8 WriteMode, encoder_realtime_setup_t *pCmdData)
{
    UINT32 Rval;
    CmdInfo_t Info = {.WriteMode = WriteMode};
    pCmdData->cmd_code = CMD_ENCODER_REALTIME_SETUP;
    Rval = AmbaLL_CmdSend(WriteMode, pCmdData, (UINT32)sizeof(encoder_realtime_setup_t));
    if ((Rval == OK) && (AmbaLL_CmdLogChk(CMD_ENCODER_REALTIME_SETUP) == CMD_ON)){
        dump_cmd_encoder_realtime_setup_t(pCmdData, &Info);
    }
    return Rval;
}

/*-----------------------------------------------------------------------------------------------*\
 * CAT_DEC (7)
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaLL_CmdDecoderSetup(UINT8 WriteMode, cmd_decoder_setup_t *pCmdData)
{
    UINT32 Rval;
    CmdInfo_t Info = {.WriteMode = WriteMode};

    pCmdData->hdr.cmd_code = CMD_DECODER_SETUP;
    Rval = AmbaLL_CmdSend(WriteMode, pCmdData, (UINT32)sizeof(cmd_decoder_setup_t));
    if ((Rval == OK) && (AmbaLL_CmdLogChk(CMD_DECODER_SETUP) == CMD_ON)){
        dump_cmd_decoder_setup_t(pCmdData, &Info);
    }
    return Rval;
}

UINT32 AmbaLL_CmdDecoderStart(UINT8 WriteMode, cmd_decoder_start_t *pCmdData)
{
    UINT32 Rval;
    CmdInfo_t Info = {.WriteMode = WriteMode};

    pCmdData->hdr.cmd_code = CMD_DECODER_START;
    Rval = AmbaLL_CmdSend(WriteMode, pCmdData, (UINT32)sizeof(cmd_decoder_start_t));
    if ((Rval == OK) && (AmbaLL_CmdLogChk(CMD_DECODER_START) == CMD_ON)){
        dump_cmd_decoder_start_t(pCmdData, &Info);
    }
    return Rval;
}

UINT32 AmbaLL_CmdDecoderStop(UINT8 WriteMode, cmd_decoder_stop_t *pCmdData)
{
    UINT32 Rval;
    CmdInfo_t Info = {.WriteMode = WriteMode};

    pCmdData->hdr.cmd_code = CMD_DECODER_STOP;
    Rval = AmbaLL_CmdSend(WriteMode, pCmdData, (UINT32)sizeof(cmd_decoder_stop_t));
    if ((Rval == OK) && (AmbaLL_CmdLogChk(CMD_DECODER_STOP) == CMD_ON)){
        dump_cmd_decoder_stop_t(pCmdData, &Info);
    }
    return Rval;
}

UINT32 AmbaLL_CmdDecoderBitsfifoUpdate(UINT8 WriteMode, cmd_decoder_bitsfifo_update_t *pCmdData)
{
    UINT32 Rval;
    CmdInfo_t Info = {.WriteMode = WriteMode};

    pCmdData->hdr.cmd_code = CMD_DECODER_BITSFIFO_UPDATE;
    Rval = AmbaLL_CmdSend(WriteMode, pCmdData, (UINT32)sizeof(cmd_decoder_bitsfifo_update_t));
    if ((Rval == OK) && (AmbaLL_CmdLogChk(CMD_DECODER_BITSFIFO_UPDATE) == CMD_ON)){
        dump_cmd_decoder_bitsfifo_update_t(pCmdData, &Info);
    }
    return Rval;
}

UINT32 AmbaLL_CmdDecoderSpeed(UINT8 WriteMode, cmd_decoder_speed_t *pCmdData)
{
    UINT32 Rval;
    CmdInfo_t Info = {.WriteMode = WriteMode};

    pCmdData->hdr.cmd_code = CMD_DECODER_SPEED;
    Rval = AmbaLL_CmdSend(WriteMode, pCmdData, (UINT32)sizeof(cmd_decoder_speed_t));
    if ((Rval == OK) && (AmbaLL_CmdLogChk(CMD_DECODER_SPEED) == CMD_ON)){
        dump_cmd_decoder_speed_t(pCmdData, &Info);
    }
    return Rval;
}

UINT32 AmbaLL_CmdDecoderTrickplay(UINT8 WriteMode, cmd_decoder_trickplay_t *pCmdData)
{
    UINT32 Rval;
    CmdInfo_t Info = {.WriteMode = WriteMode};

    pCmdData->hdr.cmd_code = CMD_DECODER_TRICKPLAY;
    Rval = AmbaLL_CmdSend(WriteMode, pCmdData, (UINT32)sizeof(cmd_decoder_trickplay_t));
    if ((Rval == OK) && (AmbaLL_CmdLogChk(CMD_DECODER_TRICKPLAY) == CMD_ON)){
        dump_cmd_decoder_trickplay_t(pCmdData, &Info);
    }
    return Rval;
}

UINT32 AmbaLL_CmdDecoderStilldec(UINT8 WriteMode, cmd_decoder_stilldec_t *pCmdData)
{
    UINT32 Rval;
    CmdInfo_t Info = {.WriteMode = WriteMode};

    pCmdData->hdr.cmd_code = CMD_DECODER_STILLDEC;
    Rval = AmbaLL_CmdSend(WriteMode, pCmdData, (UINT32)sizeof(cmd_decoder_stilldec_t));
    if ((Rval == OK) && (AmbaLL_CmdLogChk(CMD_DECODER_STILLDEC) == CMD_ON)){
        dump_cmd_decoder_stilldec_t(pCmdData, &Info);
    }
    return Rval;
}

#ifdef SUPPORT_DSP_DEC_BATCH
UINT32 AmbaLL_CmdDecoderBatch(UINT8 WriteMode, cmd_decoder_batch_t *pCmdData)
{
    UINT32 Rval;
    CmdInfo_t Info = {.WriteMode = WriteMode};

    pCmdData->hdr.cmd_code = CMD_DECODER_BATCH;
    Rval = AmbaLL_CmdSend(WriteMode, pCmdData, (UINT32)sizeof(cmd_decoder_batch_t));
    if ((Rval == OK) && (AmbaLL_CmdLogChk(CMD_DECODER_BATCH) == CMD_ON)){
        dump_cmd_decoder_batch_t(pCmdData, &Info);
    }
    return Rval;
}
#endif

/*-----------------------------------------------------------------------------------------------*\
 * CAT_IDSP (9)
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaLL_CmdSetVinConfig(UINT8 WriteMode, set_vin_config_t *pCmdData)
{
    UINT32 Rval;
    CmdInfo_t Info = {.WriteMode = WriteMode};
    pCmdData->cmd_code = SET_VIN_CONFIG;
    Rval = AmbaLL_CmdSend(WriteMode, pCmdData, (UINT32)sizeof(set_vin_config_t));
    if ((Rval == OK) && (AmbaLL_CmdLogChk(SET_VIN_CONFIG) == CMD_ON)){
        dump_cmd_set_vin_config_t(pCmdData, &Info);
    }
    return Rval;
}

#ifdef SUPPORT_DSP_SET_IDSP_DBG_CMD
UINT32 AmbaLL_CmdSendIdspDebugCmd(UINT8 WriteMode, send_idsp_debug_cmd_t *pCmdData)
{
    UINT32 Rval;
    CmdInfo_t Info = {.WriteMode = WriteMode};

    pCmdData->cmd_code = SEND_IDSP_DEBUG_CMD;
    Rval = AmbaLL_CmdSend(WriteMode, pCmdData, (UINT32)sizeof(send_idsp_debug_cmd_t));
    if ((Rval == OK) && (AmbaLL_CmdLogChk(SEND_IDSP_DEBUG_CMD) == CMD_ON)) {
        dump_cmd_send_idsp_debug_cmd_t(pCmdData, &Info);
    }
    return Rval;
}
#endif

/*-----------------------------------------------------------------------------------------------*\
 * CAT_POSTPROC (10)
\*-----------------------------------------------------------------------------------------------*/
 
