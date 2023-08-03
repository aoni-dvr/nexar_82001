/**
 *  @file AmbaDSP_ArmComm_Priv.c
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
 *  @details ARM-DSP communication private module
 *
 */

#include "AmbaDSP_Int.h"
#include "AmbaDSP_Log.h"
#include "AmbaDSP_VideoDec.h"
#include "AmbaDSP_EventCtrl.h"
#include "AmbaDSP_MsgDispatcher.h"

const UINT8 SyncCmdBufCtrlIdMap[AMBA_DSP_NUM_CMD_WRITE] = {
    [AMBA_DSP_CMD_DEFAULT_WRITE]                = AMBA_DSP_NUM_SYNC_CMD_BUF_CTRL,
    [AMBA_DSP_CMD_VDSP_NORMAL_WRITE]            = AMBA_DSP_SYNC_CMD_BUF_VDSP0,
    [AMBA_DSP_CMD_VDSP_GROUP_WRITE]             = AMBA_DSP_SYNC_CMD_BUF_VDSP0,
    [AMBA_DSP_CMD_VCAP_NORMAL_WRITE]            = AMBA_DSP_SYNC_CMD_BUF_VDSP1,
    [AMBA_DSP_CMD_VCAP_GROUP_WRITE]             = AMBA_DSP_SYNC_CMD_BUF_VDSP1,
    [AMBA_DSP_CMD_VCAP2_NORMAL_WRITE]           = AMBA_DSP_SYNC_CMD_BUF_VDSP2,
    [AMBA_DSP_CMD_VCAP2_GROUP_WRITE]            = AMBA_DSP_SYNC_CMD_BUF_VDSP2,
    [AMBA_DSP_CMD_ASYNC_WRITE]                  = AMBA_DSP_NUM_SYNC_CMD_BUF_CTRL,
    [AMBA_DSP_CMD_NOP]                          = AMBA_DSP_NUM_SYNC_CMD_BUF_CTRL,
};

static inline UINT32 ReplaceInitDataPhysAddr(AMBA_DSP_INIT_DATA_s *pInitData, const AMBA_DSP_SYS_CONFIG_s *pDspSysConfig)
{
    UINT32 Rval = OK;
    void *pVoid;
    DSP_MEM_BLK_t MemBlk = {0};

    Rval = DSP_GetProtBuf(DSP_PROT_BUF_CHIP_DATA, 0U, &MemBlk);
    LL_PrintErrLine(Rval, __func__, __LINE__);
    if (Rval == OK) {
        Rval = dsp_osal_virt2cli(MemBlk.Base, &pInitData->Data.chip_id_daddr);
        LL_PrintErrLine(Rval, __func__, __LINE__);
    }

    if (Rval == OK) {
        Rval = DSP_GetProtBuf(DSP_PROT_BUF_ASSERT_INFO, 0U, &MemBlk);
        LL_PrintErrLine(Rval, __func__, __LINE__);
        if (Rval == OK) {
            Rval = dsp_osal_virt2cli(MemBlk.Base, &pInitData->Data.orc_assert_daddr);
            LL_PrintErrLine(Rval, __func__, __LINE__);
        }
    }

    /* Reset DSP log buffer to avoid garbage info */
    if (Rval == OK) {
        dsp_osal_typecast(&pVoid, &pDspSysConfig->DebugLogDataAreaAddr);
        Rval = dsp_osal_memset(pVoid, 0, pDspSysConfig->DebugLogDataAreaSize);
        LL_PrintErrLine(Rval, __func__, __LINE__);
        Rval = dsp_osal_virt2cli(pDspSysConfig->DebugLogDataAreaAddr, &pInitData->Data.dsp_debug_daddr);
        LL_PrintErrLine(Rval, __func__, __LINE__);
        pInitData->Data.dsp_debug_dsize = pDspSysConfig->DebugLogDataAreaSize;
    }

    /* Assign dsp work buffer */
    if (Rval == OK) {
        Rval = dsp_osal_virt2cli(pDspSysConfig->WorkAreaAddr, &pInitData->Data.dsp_dram_daddr);
        LL_PrintErrLine(Rval, __func__, __LINE__);
        pInitData->Data.dsp_dram_size = pDspSysConfig->WorkAreaSize;
        AmbaLL_LogUInt5("[DSP_INIT]Work Size %d Log Size %d", pDspSysConfig->WorkAreaSize, pDspSysConfig->DebugLogDataAreaSize, 0U, 0U, 0U);
    }

    /* Replace VpMsg */
    if ((Rval == OK) && (pDspSysConfig->VpMsgCtrlBufAddr != 0U)) {
        const AMBA_DSP_MSG_BUFF_CTRL_s *VpMsgBufCtrl = NULL;

        Rval = dsp_osal_virt2cli(pDspSysConfig->VpMsgCtrlBufAddr, &pInitData->Data.msg_q_info_data_daddr[3]);
        LL_PrintErrLine(Rval, __func__, __LINE__);
        dsp_osal_typecast(&VpMsgBufCtrl, &(pDspSysConfig->VpMsgCtrlBufAddr));

        pInitData->Data.run_with_vp = (VpMsgBufCtrl->reserved[0U] == 1U) ? (UINT8)0U: (UINT8)1U;
    }

    return Rval;
}

static inline UINT32 SetSyncCmdBufData(const dsp_init_data_t *pDSPInitData)
{
    UINT32 Rval = OK;
    ULONG ULAddr;
    AMBA_DSP_SYNC_CMD_BUF_CTRL_s *pSyncCmdBufCtrl;
    DSP_MEM_BLK_t SyncCmdBufMemBlk = {0};
    const UINT32 *pULAddr;

    /* VDSP */
    pULAddr = &(pDSPInitData->cmd_data_daddr[0U]);
    dsp_osal_typecast(&ULAddr, &pULAddr);
    Rval = DSP_GetProtBuf(DSP_PROT_BUF_SYNC_CMD_CTRL, AMBA_DSP_SYNC_CMD_BUF_VDSP0, &SyncCmdBufMemBlk);
    LL_PrintErrLine(Rval, __func__, __LINE__);
    if (Rval == OK) {
        dsp_osal_typecast(&pSyncCmdBufCtrl, &SyncCmdBufMemBlk.Base);
        dsp_osal_typecast(&pSyncCmdBufCtrl->pCmdQueueBaseAddr, &ULAddr);
    }

    /* VCAP */
    pULAddr = &(pDSPInitData->cmd_data_daddr[1U]);
    dsp_osal_typecast(&ULAddr, &pULAddr);
    Rval = DSP_GetProtBuf(DSP_PROT_BUF_SYNC_CMD_CTRL, AMBA_DSP_SYNC_CMD_BUF_VDSP1, &SyncCmdBufMemBlk);
    LL_PrintErrLine(Rval, __func__, __LINE__);
    if (Rval == OK) {
        dsp_osal_typecast(&pSyncCmdBufCtrl, &SyncCmdBufMemBlk.Base);
        dsp_osal_typecast(&pSyncCmdBufCtrl->pCmdQueueBaseAddr, &ULAddr);
    }

    Rval = DSP_GetProtBuf(DSP_PROT_BUF_SYNC_CMD_CTRL, AMBA_DSP_SYNC_CMD_BUF_VDSP2, &SyncCmdBufMemBlk);
    LL_PrintErrLine(Rval, __func__, __LINE__);
    if (Rval == OK) {
        dsp_osal_typecast(&pSyncCmdBufCtrl, &SyncCmdBufMemBlk.Base);
        dsp_osal_typecast(&pSyncCmdBufCtrl->pCmdQueueBaseAddr, &ULAddr);
    }

    return Rval;
}
/**
 * Setup the DSP init data
 * The data structure dsp_init_data_t is defined in cmd_msg_dsp.h
 * @param [in] pDspSysConfig Pointer to DSP system configuration data
 * @param [in/out] ChipInfoAddr Pointer to ChipInfo buffer address
 * @return 0 - OK, others - ErrorCode
 */
UINT32 SetupDSPInitData(const AMBA_DSP_SYS_CONFIG_s *pDspSysConfig, ULONG *pChipInfoAddr)
{
    UINT32 Rval = OK;
    const void *pVoid;
    dsp_init_data_t *pDSPInitData;
    UINT8 SysState = pDspSysConfig->SysState;
    osal_ucode_addr_t BaseAddr = {0};
    DSP_MEM_BLK_t MemBlk = {0};
    DSP_MEM_BLK_t InitDataMemBlk = {0};
    AMBA_DSP_INIT_DATA_s *pInitData = NULL;

    Rval = dsp_osal_get_ucode_base_addr(&BaseAddr);
    LL_PrintErrLine(Rval, __func__, __LINE__);
    if (Rval == OK) {
        BaseAddr.CodeAddr += DSP_INIT_DATA_OFFSET; // byte offset
        dsp_osal_typecast(&pDSPInitData, &BaseAddr.CodeAddr);

        AmbaLL_Log(AMBALLLOG_TYPE_INIT, "DSP_INIT_DATA, 0x%X%X", DSP_GetU64Msb((UINT64)BaseAddr.CodeAddr), DSP_GetU64Lsb((UINT64)BaseAddr.CodeAddr), 0);

        /* Assign the default command address in the dsp_init_data_t */
        Rval = DSP_GetProtBuf(DSP_PROT_BUF_DEF_CMD, 0U, &MemBlk);
        Rval |= DSP_GetProtBuf(DSP_PROT_BUF_INIT_DATA, 0U, &InitDataMemBlk);
        LL_PrintErrLine(Rval, __func__, __LINE__);

        if (Rval == OK) {
            dsp_osal_typecast(&pInitData, &InitDataMemBlk.Base);

            if ((SysState == AMBA_DSP_SYS_STATE_LIVEVIEW) ||
                (SysState == AMBA_DSP_SYS_STATE_PLAYBACK) ||
                (SysState == AMBA_DSP_SYS_STATE_SENSORLESS)) {
                Rval = dsp_osal_virt2cli(MemBlk.Base, &pInitData->Data.default_config_daddr);
                LL_PrintErrLine(Rval, __func__, __LINE__);
            } else {
                AmbaLL_LogUInt5("[Err] Unknown SysState[%u]", SysState, 0U, 0U, 0U, 0U);
                Rval = DSP_ERR_0001;
            }
        }

        /* Virt2Phys */
        if (Rval == OK) {
            pInitData->Data.enable_vout_ln_sync_monitor = 0U;
            /* This bit is used under is_partial_load_en=1 */
            //pInitData->Data.is_orccode_fully_load = 0U;

            /* use for CV Algo protection */
            pInitData->Data.is_prt_opt = (UINT8)1U;
            pInitData->Data.delta_audio_ticks = ALGO_PROTECT_PERIOD_10SEC;

            pInitData->Data.idsp_ext_dram_daddr = 0U;
            pInitData->Data.idsp_ext_dram_dsize = 0U;

            /* Virt2Phys */
            Rval = ReplaceInitDataPhysAddr(pInitData, pDspSysConfig);
            LL_PrintErrLine(Rval, __func__, __LINE__);
            *pChipInfoAddr = pInitData->Data.chip_id_daddr;

            /* Enable smem grouping for better Dram performance in LPDDR4 */
            dsp_osal_dbgport_set_smem_grp();

            Rval = DSP_CheckDefaultCmds();
        }

        if (Rval == OK) {
            /* Note: DSP_INIT_DATA is embedded at the 0x200 offset of orccode.bin */
            AmbaLL_Log(AMBALLLOG_TYPE_INIT, "Load DSP_INIT_DATA, size = %d", DSP_INIT_DATA_SIZE, 0, 0);
            Rval = dsp_osal_memcpy(pDSPInitData, pInitData, DSP_INIT_DATA_SIZE);
            LL_PrintErrLine(Rval, __func__, __LINE__);

            Rval = SetSyncCmdBufData(pDSPInitData);
            LL_PrintErrLine(Rval, __func__, __LINE__);

            dsp_osal_typecast(&pVoid, &pDSPInitData);
            Rval = dsp_osal_cache_clean(pVoid, sizeof(dsp_init_data_t));
            LL_PrintErrLine(Rval, __func__, __LINE__);
        }
    }

    return Rval;
}

#if 0
UINT32 DSP_ResumeSetupInitData(UINT8 SysState)
{
    UINT32 Rval;
    dsp_init_data_t *pDSPInitData;
    osal_ucode_addr_t BaseAddr = {0};
    ULONG ULAddr;
    DSP_MEM_BLK_t MemBlk = {0};
    DSP_MEM_BLK_t InitDataMemBlk = {0};
    DSP_MEM_BLK_t SyncCmdBufMemBlk = {0};
    AMBA_DSP_INIT_DATA_s *pInitData = NULL;
    AMBA_DSP_SYNC_CMD_BUF_CTRL_s *pSyncCmdBufCtrl;

    Rval = dsp_osal_get_ucode_base_addr(&BaseAddr);
    LL_PrintErrLine(Rval, __func__, __LINE__);
    BaseAddr.CodeAddr += DSP_INIT_DATA_OFFSET;
    dsp_osal_typecast(&pDSPInitData, &BaseAddr.CodeAddr);

    // Make sure the buffer is initialized before using AmbaDSP_DefaultCmds
    Rval = ArmCommInit();

    if (Rval == OK) {
        /* Assign the default command address in the dsp_init_data_t */
        Rval = DSP_GetProtBuf(DSP_PROT_BUF_DEF_CMD, 0U, &MemBlk);
        LL_PrintErrLine(Rval, __func__, __LINE__);
        Rval = DSP_GetProtBuf(DSP_PROT_BUF_INIT_DATA, 0U, &InitDataMemBlk);
        LL_PrintErrLine(Rval, __func__, __LINE__);
        dsp_osal_typecast(&pInitData, &InitDataMemBlk.Base);
        if (SysState == AMBA_DSP_SYS_STATE_LIVEVIEW) {
            Rval = dsp_osal_virt2cli(MemBlk.Base, &pInitData->Data.default_config_daddr);
            LL_PrintErrLine(Rval, __func__, __LINE__);
            pInitData->Data.default_config_dsize = MemBlk.Size;
        } else if (SysState == AMBA_DSP_SYS_STATE_PLAYBACK) {
            // TBD
        } else if (SysState == AMBA_DSP_SYS_STATE_SENSORLESS) {
            Rval = dsp_osal_virt2cli(MemBlk.Base, &pInitData->Data.default_config_daddr);
            LL_PrintErrLine(Rval, __func__, __LINE__);
            pInitData->Data.default_config_dsize = MemBlk.Size;
        } else {
            AmbaLL_LogUInt5("[Err] Unknown SysState[%u]", SysState, 0U, 0U, 0U, 0U);
            Rval = DSP_ERR_0001;
        }
    }

    if (Rval == OK) {
        Rval = DSP_CheckDefaultCmds();
    }

    if (Rval == OK) {
        const UINT32 *pULAddr;

        /* Note: DSP_INIT_DATA is embedded at the 0x200 offset of orccode.bin */
        AmbaLL_Log(AMBALLLOG_TYPE_INIT, "Load DSP_INIT_DATA, size = %d", DSP_INIT_DATA_SIZE, 0, 0);
        Rval = dsp_osal_memcpy(pDSPInitData, pInitData, DSP_INIT_DATA_SIZE);
        LL_PrintErrLine(Rval, __func__, __LINE__);

        pULAddr = &(pDSPInitData->cmd_data_daddr[0U]);
        dsp_osal_typecast(&ULAddr, &pULAddr);
        Rval = DSP_GetProtBuf(DSP_PROT_BUF_SYNC_CMD_CTRL, AMBA_DSP_SYNC_CMD_BUF_VDSP0, &SyncCmdBufMemBlk);
        LL_PrintErrLine(Rval, __func__, __LINE__);
        dsp_osal_typecast(&pSyncCmdBufCtrl, &SyncCmdBufMemBlk.Base);
        dsp_osal_typecast(&pSyncCmdBufCtrl->pCmdQueueBaseAddr, &ULAddr);

        pULAddr = &(pDSPInitData->cmd_data_daddr[1U]);
        dsp_osal_typecast(&ULAddr, &pULAddr);
        Rval = DSP_GetProtBuf(DSP_PROT_BUF_SYNC_CMD_CTRL, AMBA_DSP_SYNC_CMD_BUF_VDSP1, &SyncCmdBufMemBlk);
        LL_PrintErrLine(Rval, __func__, __LINE__);
        dsp_osal_typecast(&pSyncCmdBufCtrl, &SyncCmdBufMemBlk.Base);
        dsp_osal_typecast(&pSyncCmdBufCtrl->pCmdQueueBaseAddr, &ULAddr);
        Rval = DSP_GetProtBuf(DSP_PROT_BUF_SYNC_CMD_CTRL, AMBA_DSP_SYNC_CMD_BUF_VDSP2, &SyncCmdBufMemBlk);
        LL_PrintErrLine(Rval, __func__, __LINE__);
        dsp_osal_typecast(&pSyncCmdBufCtrl, &SyncCmdBufMemBlk.Base);
        dsp_osal_typecast(&pSyncCmdBufCtrl->pCmdQueueBaseAddr, &ULAddr);

        DSP_ResetCmdMsgBuf();
    }

    //DSP_DumpDspInitDataInfo();
    return Rval;
}
#endif

UINT8 IsNormalWriteMode(UINT8 WriteMode)
{
    UINT8 Rval = 0U;

    switch (WriteMode) {
        case AMBA_DSP_CMD_VDSP_NORMAL_WRITE:
        case AMBA_DSP_CMD_VCAP_NORMAL_WRITE:
        case AMBA_DSP_CMD_VCAP2_NORMAL_WRITE:
            Rval = 1U;
            break;
        default:
            Rval = 0U;
            break;
    }
    return Rval;
}

UINT8 IsGroupWriteMode(UINT8 WriteMode)
{
    UINT8 Rval = 0U;

    switch (WriteMode) {
        case AMBA_DSP_CMD_VDSP_GROUP_WRITE:
        case AMBA_DSP_CMD_VCAP_GROUP_WRITE:
        case AMBA_DSP_CMD_VCAP2_GROUP_WRITE:
            Rval = 1U;
            break;
        default:
            Rval = 0U;
            break;
    }
    return Rval;
}

