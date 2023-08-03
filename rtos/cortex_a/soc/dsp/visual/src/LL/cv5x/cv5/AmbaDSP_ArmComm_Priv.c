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
    [AMBA_DSP_CMD_ASYNC_WRITE]                  = AMBA_DSP_NUM_SYNC_CMD_BUF_CTRL,
    [AMBA_DSP_CMD_VDSP_NORMAL_WRITE]            = AMBA_DSP_SYNC_CMD_BUF_VDSP0,
    [AMBA_DSP_CMD_VDSP_GROUP_WRITE]             = AMBA_DSP_SYNC_CMD_BUF_VDSP0,
    [AMBA_DSP_CMD_VCAP_NORMAL_WRITE]            = AMBA_DSP_SYNC_CMD_BUF_VDSP1_BASE,
    [AMBA_DSP_CMD_VCAP_GROUP_WRITE]             = AMBA_DSP_SYNC_CMD_BUF_VDSP1_BASE,
    [AMBA_DSP_CMD_VCAP_VIN1_NORMAL_WRITE]       = AMBA_DSP_SYNC_CMD_BUF_VDSP1_VIN1,
    [AMBA_DSP_CMD_VCAP_VIN1_GROUP_WRITE]        = AMBA_DSP_SYNC_CMD_BUF_VDSP1_VIN1,
    [AMBA_DSP_CMD_VCAP_VIN2_NORMAL_WRITE]       = AMBA_DSP_SYNC_CMD_BUF_VDSP1_VIN2,
    [AMBA_DSP_CMD_VCAP_VIN2_GROUP_WRITE]        = AMBA_DSP_SYNC_CMD_BUF_VDSP1_VIN2,
    [AMBA_DSP_CMD_VCAP_VIN3_NORMAL_WRITE]       = AMBA_DSP_SYNC_CMD_BUF_VDSP1_VIN3,
    [AMBA_DSP_CMD_VCAP_VIN3_GROUP_WRITE]        = AMBA_DSP_SYNC_CMD_BUF_VDSP1_VIN3,
    [AMBA_DSP_CMD_VCAP_VIN4_NORMAL_WRITE]       = AMBA_DSP_SYNC_CMD_BUF_VDSP1_VIN4,
    [AMBA_DSP_CMD_VCAP_VIN4_GROUP_WRITE]        = AMBA_DSP_SYNC_CMD_BUF_VDSP1_VIN4,
    [AMBA_DSP_CMD_VCAP_VIN5_NORMAL_WRITE]       = AMBA_DSP_SYNC_CMD_BUF_VDSP1_VIN5,
    [AMBA_DSP_CMD_VCAP_VIN5_GROUP_WRITE]        = AMBA_DSP_SYNC_CMD_BUF_VDSP1_VIN5,
    [AMBA_DSP_CMD_VCAP_VIN6_NORMAL_WRITE]       = AMBA_DSP_SYNC_CMD_BUF_VDSP1_VIN6,
    [AMBA_DSP_CMD_VCAP_VIN6_GROUP_WRITE]        = AMBA_DSP_SYNC_CMD_BUF_VDSP1_VIN6,
    [AMBA_DSP_CMD_VCAP_VIN7_NORMAL_WRITE]       = AMBA_DSP_SYNC_CMD_BUF_VDSP1_VIN7,
    [AMBA_DSP_CMD_VCAP_VIN7_GROUP_WRITE]        = AMBA_DSP_SYNC_CMD_BUF_VDSP1_VIN7,
    [AMBA_DSP_CMD_VCAP_VIN8_NORMAL_WRITE]       = AMBA_DSP_SYNC_CMD_BUF_VDSP1_VIN8,
    [AMBA_DSP_CMD_VCAP_VIN8_GROUP_WRITE]        = AMBA_DSP_SYNC_CMD_BUF_VDSP1_VIN8,
    [AMBA_DSP_CMD_VCAP_VIN9_NORMAL_WRITE]       = AMBA_DSP_SYNC_CMD_BUF_VDSP1_VIN9,
    [AMBA_DSP_CMD_VCAP_VIN9_GROUP_WRITE]        = AMBA_DSP_SYNC_CMD_BUF_VDSP1_VIN9,
    [AMBA_DSP_CMD_VCAP_VIN10_NORMAL_WRITE]      = AMBA_DSP_SYNC_CMD_BUF_VDSP1_VIN10,
    [AMBA_DSP_CMD_VCAP_VIN10_GROUP_WRITE]       = AMBA_DSP_SYNC_CMD_BUF_VDSP1_VIN10,
    [AMBA_DSP_CMD_VCAP_VIN11_NORMAL_WRITE]      = AMBA_DSP_SYNC_CMD_BUF_VDSP1_VIN11,
    [AMBA_DSP_CMD_VCAP_VIN11_GROUP_WRITE]       = AMBA_DSP_SYNC_CMD_BUF_VDSP1_VIN11,
    [AMBA_DSP_CMD_VCAP_VIN12_NORMAL_WRITE]      = AMBA_DSP_SYNC_CMD_BUF_VDSP1_VIN12,
    [AMBA_DSP_CMD_VCAP_VIN12_GROUP_WRITE]       = AMBA_DSP_SYNC_CMD_BUF_VDSP1_VIN12,
    [AMBA_DSP_CMD_VCAP_VIN13_NORMAL_WRITE]      = AMBA_DSP_SYNC_CMD_BUF_VDSP1_VIN13,
    [AMBA_DSP_CMD_VCAP_VIN13_GROUP_WRITE]       = AMBA_DSP_SYNC_CMD_BUF_VDSP1_VIN13,
    [AMBA_DSP_CMD_NOP]                          = AMBA_DSP_NUM_SYNC_CMD_BUF_CTRL,
};

#ifdef ENABLE_CMD_MSG_CRC
static inline void UpdateCmdBufCheckSum(AMBA_DSP_HEADER_CMD_s *pHeader, UINT32 CmdIdx, UINT32 CmdNum)
{
    const AMBA_DSP_HEADER_CMD_s* pNewCmd = &pHeader[CmdIdx];
    const UINT32 *pU32 = NULL;
    UINT32 CRCVal;
#ifdef DEBUG_CRC
    UINT32 Time = 0U;
    ULONG StartAddr, EndAddr;
#endif

    if (CmdIdx == 1U) {
        CRCVal = 0U;
    } else {
        CRCVal = pHeader->Contents.cmd_block_crc;
    }
#ifdef DEBUG_CRC
    AmbaLL_Log(AMBALLLOG_TYPE_CMD, "  Prev CheckSum 0x%x Cmd#%d CmdNum %d", CRCVal, CmdIdx, CmdNum);
#endif

    dsp_osal_typecast(&pU32, &pNewCmd);
#ifdef DEBUG_CRC
    Time = dsp_osal_get_orc_timer();
#endif
    CRCVal = CalcCheckSum32Add(pU32, sizeof(dsp_cmd_t)*CmdNum, CRCVal);
#ifdef DEBUG_CRC
    AmbaLL_Log(AMBALLLOG_TYPE_ISR, "UpdateCmdBufCheckSum %u, crc:%u", dsp_osal_get_orc_timer() - Time, CRCVal, 0);
    dsp_osal_typecast(&StartAddr, &pNewCmd);
    pNewCmd = &pHeader[CmdIdx+CmdNum];
    dsp_osal_typecast(&EndAddr, &pNewCmd);
    AmbaLL_Log(AMBALLLOG_TYPE_CMD, "   Cur CheckSum 0x%x AddrS 0x%X AddrE 0x%X", CRCVal, StartAddr, EndAddr);
#endif
    pHeader->Contents.cmd_block_crc = CRCVal;
}
#endif

static inline void DSP_SetupInitDataCrc(void)
{
#ifdef ENABLE_CMD_MSG_CRC
    const UINT32 *pU32;
    const dsp_cmd_t *pDspCmd;
    UINT32 PhysAddr = 0U, Crc;
    osal_ucode_addr_t BaseAddr = {0};
    ULONG ULAddr;
    DSP_MEM_BLK_t SyncCmdBufMemBlk = {0};
    dsp_init_data_t *pDSPInitData;
    const AMBA_DSP_SYNC_CMD_BUF_CTRL_s *pSyncCmdBufCtrl = NULL;
    const void *pVoid = NULL;
    UINT32 Rval;

    Rval = dsp_osal_get_ucode_base_addr(&BaseAddr);
    LL_PrintErrLine(Rval, __func__, __LINE__);
    if (Rval == OK) {
        BaseAddr.Code1Addr += DSP_INIT_DATA_OFFSET; // byte offset
        dsp_osal_typecast(&pDSPInitData, &BaseAddr.Code1Addr);
        pDSPInitData->crc_init_data = 0;

        Rval = DSP_GetProtBuf(DSP_PROT_BUF_SYNC_CMD_CTRL, AMBA_DSP_SYNC_CMD_BUF_VDSP0, &SyncCmdBufMemBlk);
        LL_PrintErrLine(Rval, __func__, __LINE__);
        if (Rval == OK) {
            dsp_osal_typecast(&pSyncCmdBufCtrl, &SyncCmdBufMemBlk.Base);

            /* Update the cmd addr specified in dsp_init_data_t */
            pDspCmd = &(pSyncCmdBufCtrl->pCmdBufBaseAddr[pSyncCmdBufCtrl->ReadPtr * pSyncCmdBufCtrl->MaxNumCmds]);
            dsp_osal_typecast(&ULAddr, &pDspCmd);

            Rval = dsp_osal_virt2cli(ULAddr, &PhysAddr);
            LL_PrintErrLine(Rval, __func__, __LINE__);
            if (Rval == OK) {
                //dsp_osal_typecast(&pSyncCmdBufCtrl->pCmdQueueBaseAddr, &ULAddrVirt);
                *pSyncCmdBufCtrl->pCmdQueueBaseAddr = PhysAddr;
                dsp_osal_typecast(&pVoid, &pSyncCmdBufCtrl->pCmdQueueBaseAddr);
                Rval = dsp_osal_cache_clean(pVoid, sizeof(UINT32));
                AmbaMisra_TouchUnused(&Rval);
            }
        }

        dsp_osal_typecast(&pU32, &pDSPInitData);
        Crc = CalcCheckSum32Add(pU32, sizeof(AMBA_DSP_INIT_DATA_s) - 4U, 0U);
        pDSPInitData->crc_init_data = Crc;
        AmbaLL_Log(AMBALLLOG_TYPE_INIT, "crc_init_data [%d]", pDSPInitData->crc_init_data, 0U, 0U);
        dsp_osal_typecast(&pVoid, &pDSPInitData);
        Rval = dsp_osal_cache_clean(pVoid, sizeof(dsp_init_data_t));
        AmbaMisra_TouchUnused(&Rval);
    }
#endif
}


static inline UINT32 WriteCmdBufSetupInitCrc(AMBA_DSP_SYNC_CMD_BUF_CTRL_s *pCmdBufCtrl, const void *pCmdData, UINT32 CmdDataSize)
{
    UINT32 Rval = OK;
    UINT32 NumCmds = 0, MaxNum = 0;
    osal_spinlock_t SpinLock = {0};
    AMBA_DSP_HEADER_CMD_s *pHeader;
    dsp_cmd_t *pDspCmd;

    if (CmdDataSize > DSP_CMD_SIZE) {
        AmbaLL_LogUInt5("[Err] Crc [%d] CmdDataSize[%d] exceeds pre-defined %d bytes", __LINE__, CmdDataSize, DSP_CMD_SIZE, 0U, 0U);
        Rval = DSP_ERR_0001;
    } else {
        /* prevent entering either IRQ or FIQ handler */
        Rval = dsp_osal_spinlock(&SpinLock);
        LL_PrintErrLine(Rval, __func__, __LINE__);

        pDspCmd = &(pCmdBufCtrl->pCmdBufBaseAddr[pCmdBufCtrl->WritePtr * pCmdBufCtrl->MaxNumCmds]);
        dsp_osal_typecast(&pHeader, &pDspCmd);

        MaxNum = pCmdBufCtrl->MaxNumCmds;

        NumCmds = pHeader->Contents.num_cmds + 1U;
        if (NumCmds >= MaxNum) {
            UINT8 TempWritePtr = pCmdBufCtrl->WritePtr;

            AmbaLL_LogUInt5("This command block is FULL", 0U, 0U, 0U, 0U, 0U);
            /* CmdBuf full, let WritePtr forward */
            TempWritePtr++;
            if (TempWritePtr >= pCmdBufCtrl->MaxNumCmdBuf) {
                TempWritePtr = 0U;
            }

            if (TempWritePtr == pCmdBufCtrl->ReadPtr) {
                AmbaLL_LogUInt5("No more free command block", 0U, 0U, 0U, 0U, 0U);
                Rval = DSP_ERR_0007;
            }
            if (Rval == OK) {
                pCmdBufCtrl->WritePtr = TempWritePtr;
                pDspCmd = &(pCmdBufCtrl->pCmdBufBaseAddr[pCmdBufCtrl->WritePtr * pCmdBufCtrl->MaxNumCmds]);
                dsp_osal_typecast(&pHeader, &pDspCmd);
                NumCmds = 1U;
            }
        }
        if (Rval == OK) {
            /* first of all, to clear the command area */
            Rval = dsp_osal_memset((&pDspCmd[NumCmds]), 0, sizeof(dsp_cmd_t));
            LL_PrintErrLine(Rval, __func__, __LINE__);
            /* save command data into the buffer */
            pHeader->Contents.num_cmds = NumCmds;
            Rval = dsp_osal_memcpy(&pHeader[NumCmds], pCmdData, CmdDataSize);
            LL_PrintErrLine(Rval, __func__, __LINE__);
#ifdef ENABLE_CMD_MSG_CRC
            UpdateCmdBufCheckSum(pHeader, NumCmds, 1U);
            dsp_osal_typecast(&pHeader, &pCmdData);
            if (pHeader->Contents.cmd_code == CMD_DSP_SET_PROFILE) {
                DSP_SetupInitDataCrc();
            }
#endif
        }
        Rval = dsp_osal_spinunlock(&SpinLock);
        LL_PrintErrLine(Rval, __func__, __LINE__);
    }
    return Rval;
}

static inline UINT32 ReplaceInitDataPhysAddr(AMBA_DSP_INIT_DATA_s *pInitData, const AMBA_DSP_SYS_CONFIG_s *pDspSysConfig)
{
    UINT32 Rval = OK;
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

    /* use for Safety WatchDog */
    //cv5x didnt need it

    /* Assign dsp log buffer */
    if (Rval == OK) {
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

        Rval = dsp_osal_virt2cli(pDspSysConfig->VpMsgCtrlBufAddr, &pInitData->Data.msg_q_info_data_daddr[4]);
        LL_PrintErrLine(Rval, __func__, __LINE__);
        dsp_osal_typecast(&VpMsgBufCtrl, &(pDspSysConfig->VpMsgCtrlBufAddr));

        pInitData->Data.run_with_vp = (VpMsgBufCtrl->reserved[0U] == 1U) ? (UINT8)0U: (UINT8)1U;
    }

    return Rval;
}

static inline UINT32 SetSyncCmdBufData(const dsp_init_data_t *pDSPInitData)
{
    UINT32 Rval = DSP_ERR_NONE;
    ULONG ULAddr;
    AMBA_DSP_SYNC_CMD_BUF_CTRL_s *pSyncCmdBufCtrl;
    DSP_MEM_BLK_t SyncCmdBufMemBlk = {0};
    const UINT32 *pULAddr;
    UINT16 i;

    /* VDSP : cmd_data_daddr[0U] */
    pULAddr = &(pDSPInitData->cmd_data_daddr[0U]);
    dsp_osal_typecast(&ULAddr, &pULAddr);
    Rval = DSP_GetProtBuf(DSP_PROT_BUF_SYNC_CMD_CTRL, AMBA_DSP_SYNC_CMD_BUF_VDSP0, &SyncCmdBufMemBlk);
    LL_PrintErrLine(Rval, __func__, __LINE__);
    if (Rval == DSP_ERR_NONE) {
        dsp_osal_typecast(&pSyncCmdBufCtrl, &SyncCmdBufMemBlk.Base);
        dsp_osal_typecast(&pSyncCmdBufCtrl->pCmdQueueBaseAddr, &ULAddr);
    }

    /* VCAP : cmd_data_daddr[1U] */
    pULAddr = &(pDSPInitData->cmd_data_daddr[1U]);
    dsp_osal_typecast(&ULAddr, &pULAddr);
    for (i = 0U; i < AMBA_DSP_MAX_VIN_NUM; i++) {
        Rval = DSP_GetProtBuf(DSP_PROT_BUF_SYNC_CMD_CTRL, (AMBA_DSP_SYNC_CMD_BUF_VDSP1_BASE + (UINT32)i), &SyncCmdBufMemBlk);
        LL_PrintErrLine(Rval, __func__, __LINE__);
        if (Rval == DSP_ERR_NONE) {
            dsp_osal_typecast(&pSyncCmdBufCtrl, &SyncCmdBufMemBlk.Base);
            dsp_osal_typecast(&pSyncCmdBufCtrl->pCmdQueueBaseAddr, &ULAddr);
        }
    }

    /* Async : cmd_data_daddr[2U] */

    return Rval;
}

/**
 * Setup the DSP init data
 * The data structure dsp_init_data_t is defined in cmd_msg_dsp.h
 * @param [in] pDspSysConfig Pointer to DSP system configuration data
 * @param [in/out] ChipInfoAddr Pointer to ChipInfo buffer address
 * @return 0 - OK, others - ErrorCode
 */
UINT32 SetupDSPInitData(const AMBA_DSP_SYS_CONFIG_s *pDspSysConfig, ULONG *pChipInfoAddr, UINT8 ParLoadEnable)
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
        BaseAddr.Code1Addr += DSP_INIT_DATA_OFFSET; // byte offset
        dsp_osal_typecast(&pDSPInitData, &BaseAddr.Code1Addr);

        AmbaLL_Log(AMBALLLOG_TYPE_INIT, "DSP_INIT_DATA, 0x%X%X", DSP_GetU64Msb((UINT64)BaseAddr.Code1Addr+DSP_INIT_DATA_OFFSET), DSP_GetU64Lsb((UINT64)BaseAddr.Code1Addr+DSP_INIT_DATA_OFFSET), 0);

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
            if (pDspSysConfig->DefaultBinarySize > 0U) {
                pInitData->Data.default_binary_data_dsize = pDspSysConfig->DefaultBinarySize;
            }

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
#ifdef SUPPORT_DRAM_SMEMGRP
            dsp_osal_dbgport_set_smem_grp();
#endif

            Rval = DSP_CheckDefaultCmds();
        }

        if (Rval == OK) {
            /* Note: DSP_INIT_DATA is embedded at the 0x200 offset of orcvin.bin */
            AmbaLL_Log(AMBALLLOG_TYPE_INIT, "Load DSP_INIT_DATA, size = %d", DSP_INIT_DATA_SIZE, 0, 0);
            Rval = dsp_osal_memcpy(pDSPInitData, pInitData, DSP_INIT_DATA_SIZE);
            LL_PrintErrLine(Rval, __func__, __LINE__);

            Rval = SetSyncCmdBufData(pDSPInitData);
            LL_PrintErrLine(Rval, __func__, __LINE__);

#ifdef ENABLE_CMD_MSG_CRC
            {
                const UINT32 *pU32;
                UINT32 Crc;

                dsp_osal_typecast(&pU32, &pDSPInitData);
                Crc = CalcCheckSum32Add(pU32, sizeof(AMBA_DSP_INIT_DATA_s) - 4U, 0U);
                pDSPInitData->crc_init_data = Crc;
                AmbaLL_Log(AMBALLLOG_TYPE_INIT, "DspInit: renew crc[%d]", pDSPInitData->crc_init_data, 0U, 0U);
            }
#else
            pDSPInitData->crc_init_data = 0U;
#endif
            dsp_osal_typecast(&pVoid, &pDSPInitData);
            Rval = dsp_osal_cache_clean(pVoid, (UINT32)sizeof(dsp_init_data_t));
            LL_PrintErrLine(Rval, __func__, __LINE__);
        }
    }

    /* 2021/6/5 all orc need to have init-data */
    if (Rval == DSP_ERR_NONE) {
        dsp_init_data_t *pTmpDSPInitData;

        if ((ParLoadEnable == 0U) ||
            (pInitData->Data.is_orccode_fully_load == 1U)) {
            /* OrcCode */
            BaseAddr.CodeAddr += DSP_INIT_DATA_OFFSET;
            dsp_osal_typecast(&pTmpDSPInitData, &BaseAddr.CodeAddr);
            Rval = dsp_osal_memcpy(pTmpDSPInitData, pDSPInitData, sizeof(dsp_init_data_t));
            LL_PrintErrLine(Rval, __func__, __LINE__);

            dsp_osal_typecast(&pVoid, &pTmpDSPInitData);
            Rval = dsp_osal_cache_clean(pVoid, (UINT32)sizeof(dsp_init_data_t));
            LL_PrintErrLine(Rval, __func__, __LINE__);
        }

        /* OrcIdsp0 */
        BaseAddr.Code2Addr += DSP_INIT_DATA_OFFSET;
        dsp_osal_typecast(&pTmpDSPInitData, &BaseAddr.Code2Addr);
        Rval = dsp_osal_memcpy(pTmpDSPInitData, pDSPInitData, sizeof(dsp_init_data_t));
        LL_PrintErrLine(Rval, __func__, __LINE__);

        dsp_osal_typecast(&pVoid, &pTmpDSPInitData);
        Rval = dsp_osal_cache_clean(pVoid, (UINT32)sizeof(dsp_init_data_t));
        LL_PrintErrLine(Rval, __func__, __LINE__);

        /* OrcIdsp1 */
        BaseAddr.Code3Addr += DSP_INIT_DATA_OFFSET;
        dsp_osal_typecast(&pTmpDSPInitData, &BaseAddr.Code3Addr);
        Rval = dsp_osal_memcpy(pTmpDSPInitData, pDSPInitData, sizeof(dsp_init_data_t));
        LL_PrintErrLine(Rval, __func__, __LINE__);

        dsp_osal_typecast(&pVoid, &pTmpDSPInitData);
        Rval = dsp_osal_cache_clean(pVoid, (UINT32)sizeof(dsp_init_data_t));
        LL_PrintErrLine(Rval, __func__, __LINE__);
    }

    //DSP_DumpDspInitDataInfo();
    return Rval;
}

#if 0
UINT32 DSP_ResumeSetupInitData(UINT8 SysState)
{
    UINT32 Rval;
    dsp_init_data_t *pDSPInitData;
    osal_ucode_addr_t BaseAddr = {0};
    ULONG ULAddr;
    UINT16 i;
    DSP_MEM_BLK_t MemBlk = {0};
    DSP_MEM_BLK_t InitDataMemBlk = {0};
    DSP_MEM_BLK_t SyncCmdBufMemBlk = {0};
    AMBA_DSP_INIT_DATA_s *pInitData = NULL;
    AMBA_DSP_SYNC_CMD_BUF_CTRL_s *pSyncCmdBufCtrl;

    Rval = dsp_osal_get_ucode_base_addr(&BaseAddr);
    LL_PrintErrLine(Rval, __func__, __LINE__);
    BaseAddr.Code1Addr += DSP_INIT_DATA_OFFSET;
    dsp_osal_typecast(&pDSPInitData, &BaseAddr.Code1Addr);

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

        /* VDSP */
        pULAddr = &(pDSPInitData->cmd_data_daddr[0U]);
        dsp_osal_typecast(&ULAddr, &pULAddr);
        Rval = DSP_GetProtBuf(DSP_PROT_BUF_SYNC_CMD_CTRL, AMBA_DSP_SYNC_CMD_BUF_VDSP0, &SyncCmdBufMemBlk);
        LL_PrintErrLine(Rval, __func__, __LINE__);
        dsp_osal_typecast(&pSyncCmdBufCtrl, &SyncCmdBufMemBlk.Base);
        dsp_osal_typecast(&pSyncCmdBufCtrl->pCmdQueueBaseAddr, &ULAddr);

        /* VCAP */
        pULAddr = &(pDSPInitData->cmd_data_daddr[1U]);
        dsp_osal_typecast(&ULAddr, &pULAddr);
        for (i = 0U; i < AMBA_DSP_MAX_VIN_NUM; i++) {
            Rval = DSP_GetProtBuf(DSP_PROT_BUF_SYNC_CMD_CTRL, AMBA_DSP_SYNC_CMD_BUF_VDSP1_BASE + i, &SyncCmdBufMemBlk);
            LL_PrintErrLine(Rval, __func__, __LINE__);
            dsp_osal_typecast(&pSyncCmdBufCtrl, &SyncCmdBufMemBlk.Base);
            dsp_osal_typecast(&pSyncCmdBufCtrl->pCmdQueueBaseAddr, &ULAddr);
        }

        /* Async */


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
        case AMBA_DSP_CMD_VCAP_VIN1_NORMAL_WRITE:
        case AMBA_DSP_CMD_VCAP_VIN2_NORMAL_WRITE:
        case AMBA_DSP_CMD_VCAP_VIN3_NORMAL_WRITE:
        case AMBA_DSP_CMD_VCAP_VIN4_NORMAL_WRITE:
        case AMBA_DSP_CMD_VCAP_VIN5_NORMAL_WRITE:
        case AMBA_DSP_CMD_VCAP_VIN6_NORMAL_WRITE:
        case AMBA_DSP_CMD_VCAP_VIN7_NORMAL_WRITE:
        case AMBA_DSP_CMD_VCAP_VIN8_NORMAL_WRITE:
        case AMBA_DSP_CMD_VCAP_VIN9_NORMAL_WRITE:
        case AMBA_DSP_CMD_VCAP_VIN10_NORMAL_WRITE:
        case AMBA_DSP_CMD_VCAP_VIN11_NORMAL_WRITE:
        case AMBA_DSP_CMD_VCAP_VIN12_NORMAL_WRITE:
        case AMBA_DSP_CMD_VCAP_VIN13_NORMAL_WRITE:
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
        case AMBA_DSP_CMD_VCAP_VIN1_GROUP_WRITE:
        case AMBA_DSP_CMD_VCAP_VIN2_GROUP_WRITE:
        case AMBA_DSP_CMD_VCAP_VIN3_GROUP_WRITE:
        case AMBA_DSP_CMD_VCAP_VIN4_GROUP_WRITE:
        case AMBA_DSP_CMD_VCAP_VIN5_GROUP_WRITE:
        case AMBA_DSP_CMD_VCAP_VIN6_GROUP_WRITE:
        case AMBA_DSP_CMD_VCAP_VIN7_GROUP_WRITE:
        case AMBA_DSP_CMD_VCAP_VIN8_GROUP_WRITE:
        case AMBA_DSP_CMD_VCAP_VIN9_GROUP_WRITE:
        case AMBA_DSP_CMD_VCAP_VIN10_GROUP_WRITE:
        case AMBA_DSP_CMD_VCAP_VIN11_GROUP_WRITE:
        case AMBA_DSP_CMD_VCAP_VIN12_GROUP_WRITE:
        case AMBA_DSP_CMD_VCAP_VIN13_GROUP_WRITE:
            Rval = 1U;
            break;
        default:
            Rval = 0U;
            break;
    }
    return Rval;
}

#ifdef ENABLE_CMD_MSG_CRC
//#define DEBUG_CRC
UINT32 AmbaLL_CmdSendSetupInitCrc(UINT8 WriteMode, const void *pCmdData, UINT32 CmdDataSize)
{
    UINT32 Rval;
    DSP_MEM_BLK_t MemBlk = {0};
    AMBA_DSP_SYNC_CMD_BUF_CTRL_s *pSyncCmdBufCtrl;

    // Make sure the buffer is initialized before using AmbaDSP_SyncCmdBufCtrl
    Rval = ArmCommInit();

    if (Rval == OK) {
        if (WriteMode < AMBA_DSP_NUM_CMD_WRITE) {
            if (WriteMode == AMBA_DSP_CMD_NOP) {
                Rval = OK;
            } else if (WriteMode == AMBA_DSP_CMD_DEFAULT_WRITE) {
                Rval = WriteDefaultCmdBuf(AMBA_DSP_SYS_STATE_NUM, pCmdData, CmdDataSize);
            } else if (WriteMode == AMBA_DSP_CMD_VDSP_NORMAL_WRITE) {
                Rval = DSP_GetProtBuf(DSP_PROT_BUF_SYNC_CMD_CTRL, SyncCmdBufCtrlIdMap[WriteMode], &MemBlk);
                LL_PrintErrLine(Rval, __func__, __LINE__);
                if (Rval == OK) {
                    dsp_osal_typecast(&pSyncCmdBufCtrl, &MemBlk.Base);
                    Rval = WriteCmdBufSetupInitCrc(pSyncCmdBufCtrl, pCmdData, CmdDataSize);
                }
            } else if (IsNormalWriteMode(WriteMode) == 1U) {
                Rval = DSP_GetProtBuf(DSP_PROT_BUF_SYNC_CMD_CTRL, SyncCmdBufCtrlIdMap[WriteMode], &MemBlk);
                LL_PrintErrLine(Rval, __func__, __LINE__);
                if (Rval == OK) {
                    dsp_osal_typecast(&pSyncCmdBufCtrl, &MemBlk.Base);
                    Rval = WriteCmdBuf(pSyncCmdBufCtrl, pCmdData, CmdDataSize);
                }
            } else if (IsGroupWriteMode(WriteMode) == 1U) {
                Rval = DSP_GetProtBuf(DSP_PROT_BUF_SYNC_CMD_CTRL, SyncCmdBufCtrlIdMap[WriteMode], &MemBlk);
                LL_PrintErrLine(Rval, __func__, __LINE__);
                if (Rval == OK) {
                    dsp_osal_typecast(&pSyncCmdBufCtrl, &MemBlk.Base);
                    Rval = WriteGroupCmdBuf(pSyncCmdBufCtrl, pCmdData, CmdDataSize);
                }
            } else {
                AmbaLL_LogUInt5("[Err] Invalid CmdType[%u]", WriteMode, 0U, 0U, 0U, 0U);
                Rval = DSP_ERR_0007;
            }
        } else {
            AmbaLL_LogUInt5("[Err] Invalid CmdType[%u]", WriteMode, 0U, 0U, 0U, 0U);
            Rval = DSP_ERR_0007;
        }
    }
    return Rval;
}

#endif

