/**
 *  @file AmbaDSP_Buffers_Priv.c
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
 *  @details Declarations of all DSP private non-cached buffers
 *
 */

#include "AmbaDSP_Buffers.h"
#include "AmbaDSP_CommonAPI.h"

static AMBA_DSP_STAT_BUF_t DspStatBuf = {0};
static AMBA_DSP_PROT_BUF_t DspProtBuf = {0};

void AmbaDSP_InitBuffers(void)
{
    void *pVoid;
    UINT32 DefDataPhysAddr = 0U;
    UINT16 i;
    osal_ucode_addr_t BaseAddr = {0};
    dsp_msg_q_info_t *pDspMsgQInfo;
    AMBA_DSP_DEFAULT_CMDS_s *pDspDefCmds;
    AMBA_DSP_INIT_DATA_s *pInitData;
    AMBA_DSP_SYNC_CMD_BUF_CTRL_s *pSyncBufCtrl;
    AMBA_DSP_MSG_BUF_CTRL_s *pMsgBufCtrl;
static UINT8 InitBuffersFlag = 0U;
static UINT32 VdspCurCmdSeqNo = 0U, VcapCurCmdSeqNo = 0U;
static UINT32 VdspCmdReqRptr = 0U, VcapCmdReqRptr = 0U;
static UINT32 VProcCmdReqRptr = 0U, AsyncCmdReqRptr = 0U;
    UINT32 Rval = OK;
    UINT32 CmdReqTimeoutEnable = 1U, CmdReqTimeoutInterval = 4U;

    if (InitBuffersFlag == 0U) {
        /* Protocol buffer allocation */
        Rval = AmbaDSP_StatBufCreate(&DspStatBuf);
        Rval |= AmbaDSP_ProtBufCreate(&DspProtBuf);
        LL_PrintErrLine(Rval, __func__, __LINE__);

        // Init ChipData
        dsp_osal_typecast(&pVoid, &DspProtBuf.ChipData.Base);
        Rval = dsp_osal_memset(pVoid, 0, DspProtBuf.ChipData.Size);
        LL_PrintErrLine(Rval, __func__, __LINE__);

        // Init WatchDogBuffer
        dsp_osal_typecast(&pVoid, &DspProtBuf.WatchDogData.Base);
        Rval = dsp_osal_memset(pVoid, 0, DspProtBuf.WatchDogData.Size);
        LL_PrintErrLine(Rval, __func__, __LINE__);

        // Init AmbaDSP_InitData
        dsp_osal_typecast(&pInitData, &DspProtBuf.InitData.Base);
        dsp_osal_typecast(&pVoid, &DspProtBuf.InitData.Base);
        Rval = dsp_osal_memset(pVoid, 0, DspProtBuf.InitData.Size);
        LL_PrintErrLine(Rval, __func__, __LINE__);

        // Default Binary
        Rval = dsp_osal_get_ucode_base_addr(&BaseAddr);
        LL_PrintErrLine(Rval, __func__, __LINE__);
        if (Rval == OK) {
            Rval = dsp_osal_virt2cli(BaseAddr.DefBinAddr, &DefDataPhysAddr);
            LL_PrintErrLine(Rval, __func__, __LINE__);
            if (Rval == OK) {
                pInitData->Data.default_binary_data_daddr = DefDataPhysAddr;
            }
        }

        // Default Cmds
        Rval = dsp_osal_virt2cli(DspProtBuf.DefaultCmds.Base, &pInitData->Data.default_config_daddr);
        LL_PrintErrLine(Rval, __func__, __LINE__);

        // Init General Cmd Buffers
        Rval = dsp_osal_virt2cli(DspProtBuf.GeneralCmdBuf.Base, &pInitData->Data.cmd_data_daddr[0U]);
        LL_PrintErrLine(Rval, __func__, __LINE__);

        // Init Vcap Cmd Buffers
        Rval = dsp_osal_virt2cli(DspProtBuf.VinCmdBuf[0U].Base, &pInitData->Data.cmd_data_daddr[1U]);
        LL_PrintErrLine(Rval, __func__, __LINE__);

        // Init Msg Fifo Ctrl   // CH 2018/05/01 said no index skipping as CV1
        Rval = dsp_osal_virt2cli(DspProtBuf.GeneralMsgFifoCtrl.Base, &pInitData->Data.msg_q_info_data_daddr[0U]);
        LL_PrintErrLine(Rval, __func__, __LINE__);
        Rval = dsp_osal_virt2cli(DspProtBuf.VCapMsgFifoCtrl.Base, &pInitData->Data.msg_q_info_data_daddr[1U]);
        LL_PrintErrLine(Rval, __func__, __LINE__);
        Rval = dsp_osal_virt2cli(DspProtBuf.VProcMsgFifoCtrl.Base, &pInitData->Data.msg_q_info_data_daddr[2U]);
        LL_PrintErrLine(Rval, __func__, __LINE__);
        Rval = dsp_osal_virt2cli(DspProtBuf.VPMsgFifoCtrl.Base, &pInitData->Data.msg_q_info_data_daddr[3U]);
        LL_PrintErrLine(Rval, __func__, __LINE__);

#ifdef SUPPORT_DSP_ASYNC_ENCMSG
        Rval = dsp_osal_virt2cli(DspProtBuf.AsyncEncMsgFifoCtrl.Base, &pInitData->Data.msg_q_info_data_daddr[4U]);
        LL_PrintErrLine(Rval, __func__, __LINE__);
#endif

        // Remaining InitData
        pInitData->Data.default_binary_data_dsize = DSP_DEFAULT_BIN_DATA_SIZE;
        pInitData->Data.default_config_dsize      = DspProtBuf.DefaultCmds.Size;
        pInitData->Data.dsp_dram_daddr            = 0x0C000000U;   /* base address of DSP Work DRAM Area */
        pInitData->Data.dsp_dram_size             = 0x14000000U;   /* size in Bytes of DSP DRAM Area */
        pInitData->Data.cmd_data_dsize[0U]        = DSP_MAX_NUM_GENERAL_CMD * (UINT32)sizeof(dsp_cmd_t);
        pInitData->Data.cmd_data_dsize[1U]        = DSP_MAX_NUM_VCAP_CMD * (UINT32)sizeof(dsp_cmd_t);

        dsp_osal_get_cmd_req_timeout_cfg(&CmdReqTimeoutEnable, &CmdReqTimeoutInterval);
        pInitData->Data.req_cmd_timeout_assert    = (UINT8)CmdReqTimeoutEnable;
        pInitData->Data.cmd_req_timer             = (UINT8)CmdReqTimeoutInterval;

#define DSP_MSG_Q_FULL_CHECK        (0U) // temp disable it for StressTest
        pInitData->Data.msg_q_full_assert         = DSP_MSG_Q_FULL_CHECK;

        // AmbaDSP_SyncCmdBufCtrl
        for (i = 0U; i<AMBA_DSP_NUM_SYNC_CMD_BUF_CTRL; i++) {
            dsp_osal_typecast(&pVoid, &DspProtBuf.SyncCmdBufCtrl[i].Base);
            Rval = dsp_osal_memset(pVoid, 0, DspProtBuf.SyncCmdBufCtrl[i].Size);
            LL_PrintErrLine(Rval, __func__, __LINE__);
        }

        /* DSP_SYNC_CMD_BUF_VDSP0 */
        dsp_osal_typecast(&pSyncBufCtrl, &DspProtBuf.SyncCmdBufCtrl[AMBA_DSP_SYNC_CMD_BUF_VDSP0].Base);
        pSyncBufCtrl->SyncCounter             = DSP_SYNC_COUNTER_CMD_REQ_VDSP0;
        pSyncBufCtrl->MaxNumCmds              = DSP_MAX_NUM_GENERAL_CMD;
        pSyncBufCtrl->MaxNumCmdBuf            = AMBA_MAX_NUM_DSP_GENERAL_CMD_BUF;
        dsp_osal_typecast(&pSyncBufCtrl->pCmdBufBaseAddr, &DspProtBuf.GeneralCmdBuf.Base);
        dsp_osal_typecast(&pSyncBufCtrl->pGroupCmdBufBaseAddr, &DspProtBuf.GroupGeneralCmdBuf.Base);
        dsp_osal_typecast(&pSyncBufCtrl->MsgBufCtrl.pMsgFifo, &DspProtBuf.GeneralMsgBuf.Base);
        dsp_osal_typecast(&pSyncBufCtrl->MsgBufCtrl.pMsgFifoCtrl, &DspProtBuf.GeneralMsgFifoCtrl.Base);
        pSyncBufCtrl->MsgBufCtrl.pCmdReqRptr  = &VdspCmdReqRptr;
        pSyncBufCtrl->MsgBufCtrl.DspMsgRp     = 0U;
        pSyncBufCtrl->MsgBufCtrl.UnReadMsgNum = 0U;
        pSyncBufCtrl->MsgBufCtrl.DspMsgLastPtr= 0U;
        pSyncBufCtrl->Id                      = AMBA_DSP_SYNC_CMD_BUF_VDSP0;
        pSyncBufCtrl->CurCmdSeqNo             = &VdspCurCmdSeqNo;

        /* DSP_SYNC_CMD_BUF_VDSP1 */
        dsp_osal_typecast(&pSyncBufCtrl, &DspProtBuf.SyncCmdBufCtrl[AMBA_DSP_SYNC_CMD_BUF_VDSP1].Base);
        pSyncBufCtrl->SyncCounter             = DSP_SYNC_COUNTER_CMD_REQ_VDSP1;
        pSyncBufCtrl->MaxNumCmds              = DSP_MAX_NUM_VCAP_CMD;
        pSyncBufCtrl->MaxNumCmdBuf            = AMBA_MAX_NUM_DSP_VCAP_CMD_BUF;
        dsp_osal_typecast(&pSyncBufCtrl->pCmdBufBaseAddr, &DspProtBuf.VinCmdBuf[0U].Base);
        dsp_osal_typecast(&pSyncBufCtrl->pGroupCmdBufBaseAddr, &DspProtBuf.GroupVinCmdBuf[0U].Base);
        dsp_osal_typecast(&pSyncBufCtrl->MsgBufCtrl.pMsgFifo, &DspProtBuf.VCapMsgBuf.Base);
        dsp_osal_typecast(&pSyncBufCtrl->MsgBufCtrl.pMsgFifoCtrl, &DspProtBuf.VCapMsgFifoCtrl.Base);
        pSyncBufCtrl->MsgBufCtrl.pCmdReqRptr  = &VcapCmdReqRptr;
        pSyncBufCtrl->MsgBufCtrl.DspMsgRp     = 0U;
        pSyncBufCtrl->MsgBufCtrl.UnReadMsgNum = 0U;
        pSyncBufCtrl->MsgBufCtrl.DspMsgLastPtr= 0U;
        pSyncBufCtrl->Id                      = AMBA_DSP_SYNC_CMD_BUF_VDSP1;
        pSyncBufCtrl->CurCmdSeqNo             = &VcapCurCmdSeqNo;

        /* DSP_SYNC_CMD_BUF_VDSP2 */
        dsp_osal_typecast(&pSyncBufCtrl, &DspProtBuf.SyncCmdBufCtrl[AMBA_DSP_SYNC_CMD_BUF_VDSP2].Base);
        pSyncBufCtrl->SyncCounter             = DSP_SYNC_COUNTER_CMD_REQ_VDSP1;
        pSyncBufCtrl->MaxNumCmds              = DSP_MAX_NUM_VCAP_CMD;
        pSyncBufCtrl->MaxNumCmdBuf            = AMBA_MAX_NUM_DSP_VCAP_CMD_BUF;
        dsp_osal_typecast(&pSyncBufCtrl->pCmdBufBaseAddr, &DspProtBuf.VinCmdBuf[1U].Base);
        dsp_osal_typecast(&pSyncBufCtrl->pGroupCmdBufBaseAddr, &DspProtBuf.GroupVinCmdBuf[1U].Base);
        dsp_osal_typecast(&pSyncBufCtrl->MsgBufCtrl.pMsgFifo, &DspProtBuf.VCapMsgBuf.Base);
        dsp_osal_typecast(&pSyncBufCtrl->MsgBufCtrl.pMsgFifoCtrl, &DspProtBuf.VCapMsgFifoCtrl.Base);
        pSyncBufCtrl->MsgBufCtrl.pCmdReqRptr  = &VcapCmdReqRptr;
        pSyncBufCtrl->MsgBufCtrl.DspMsgRp     = 0U;
        pSyncBufCtrl->MsgBufCtrl.UnReadMsgNum = 0U;
        pSyncBufCtrl->MsgBufCtrl.DspMsgLastPtr= 0U;
        pSyncBufCtrl->Id                      = AMBA_DSP_SYNC_CMD_BUF_VDSP2;
        pSyncBufCtrl->CurCmdSeqNo             = &VcapCurCmdSeqNo;

        /* DSP_SYNC_CMD_BUF_VDSP2_3RD_VIN */
        dsp_osal_typecast(&pSyncBufCtrl, &DspProtBuf.SyncCmdBufCtrl[AMBA_DSP_SYNC_CMD_BUF_VDSP2_3RD_VIN].Base);
        pSyncBufCtrl->SyncCounter             = DSP_SYNC_COUNTER_CMD_REQ_VDSP1;
        pSyncBufCtrl->MaxNumCmds              = DSP_MAX_NUM_VCAP_CMD;
        pSyncBufCtrl->MaxNumCmdBuf            = AMBA_MAX_NUM_DSP_VCAP_CMD_BUF;
        dsp_osal_typecast(&pSyncBufCtrl->pCmdBufBaseAddr, &DspProtBuf.VinCmdBuf[2U].Base);
        dsp_osal_typecast(&pSyncBufCtrl->pGroupCmdBufBaseAddr, &DspProtBuf.GroupVinCmdBuf[2U].Base);
        dsp_osal_typecast(&pSyncBufCtrl->MsgBufCtrl.pMsgFifo, &DspProtBuf.VCapMsgBuf.Base);
        dsp_osal_typecast(&pSyncBufCtrl->MsgBufCtrl.pMsgFifoCtrl, &DspProtBuf.VCapMsgFifoCtrl.Base);
        pSyncBufCtrl->MsgBufCtrl.pCmdReqRptr  = &VcapCmdReqRptr;
        pSyncBufCtrl->MsgBufCtrl.DspMsgRp     = 0U;
        pSyncBufCtrl->MsgBufCtrl.UnReadMsgNum = 0U;
        pSyncBufCtrl->MsgBufCtrl.DspMsgLastPtr= 0U;
        pSyncBufCtrl->Id                      = AMBA_DSP_SYNC_CMD_BUF_VDSP2_3RD_VIN;
        pSyncBufCtrl->CurCmdSeqNo             = &VcapCurCmdSeqNo;

        /* DSP_SYNC_CMD_BUF_VDSP2_4TH_VIN */
        dsp_osal_typecast(&pSyncBufCtrl, &DspProtBuf.SyncCmdBufCtrl[AMBA_DSP_SYNC_CMD_BUF_VDSP2_4TH_VIN].Base);
        pSyncBufCtrl->SyncCounter             = DSP_SYNC_COUNTER_CMD_REQ_VDSP1;
        pSyncBufCtrl->MaxNumCmds              = DSP_MAX_NUM_VCAP_CMD;
        pSyncBufCtrl->MaxNumCmdBuf            = AMBA_MAX_NUM_DSP_VCAP_CMD_BUF;
        dsp_osal_typecast(&pSyncBufCtrl->pCmdBufBaseAddr, &DspProtBuf.VinCmdBuf[3U].Base);
        dsp_osal_typecast(&pSyncBufCtrl->pGroupCmdBufBaseAddr, &DspProtBuf.GroupVinCmdBuf[3U].Base);
        dsp_osal_typecast(&pSyncBufCtrl->MsgBufCtrl.pMsgFifo, &DspProtBuf.VCapMsgBuf.Base);
        dsp_osal_typecast(&pSyncBufCtrl->MsgBufCtrl.pMsgFifoCtrl, &DspProtBuf.VCapMsgFifoCtrl.Base);
        pSyncBufCtrl->MsgBufCtrl.pCmdReqRptr  = &VcapCmdReqRptr;
        pSyncBufCtrl->MsgBufCtrl.DspMsgRp     = 0U;
        pSyncBufCtrl->MsgBufCtrl.UnReadMsgNum = 0U;
        pSyncBufCtrl->MsgBufCtrl.DspMsgLastPtr= 0U;
        pSyncBufCtrl->Id                      = AMBA_DSP_SYNC_CMD_BUF_VDSP2_4TH_VIN;
        pSyncBufCtrl->CurCmdSeqNo             = &VcapCurCmdSeqNo;

        /* DSP_SYNC_CMD_BUF_VDSP2_5TH_VIN */
        dsp_osal_typecast(&pSyncBufCtrl, &DspProtBuf.SyncCmdBufCtrl[AMBA_DSP_SYNC_CMD_BUF_VDSP2_5TH_VIN].Base);
        pSyncBufCtrl->SyncCounter             = DSP_SYNC_COUNTER_CMD_REQ_VDSP1;
        pSyncBufCtrl->MaxNumCmds              = DSP_MAX_NUM_VCAP_CMD;
        pSyncBufCtrl->MaxNumCmdBuf            = AMBA_MAX_NUM_DSP_VCAP_CMD_BUF;
        dsp_osal_typecast(&pSyncBufCtrl->pCmdBufBaseAddr, &DspProtBuf.VinCmdBuf[4U].Base);
        dsp_osal_typecast(&pSyncBufCtrl->pGroupCmdBufBaseAddr, &DspProtBuf.GroupVinCmdBuf[4U].Base);
        dsp_osal_typecast(&pSyncBufCtrl->MsgBufCtrl.pMsgFifo, &DspProtBuf.VCapMsgBuf.Base);
        dsp_osal_typecast(&pSyncBufCtrl->MsgBufCtrl.pMsgFifoCtrl, &DspProtBuf.VCapMsgFifoCtrl.Base);
        pSyncBufCtrl->MsgBufCtrl.pCmdReqRptr  = &VcapCmdReqRptr;
        pSyncBufCtrl->MsgBufCtrl.DspMsgRp     = 0U;
        pSyncBufCtrl->MsgBufCtrl.UnReadMsgNum = 0U;
        pSyncBufCtrl->MsgBufCtrl.DspMsgLastPtr= 0U;
        pSyncBufCtrl->Id                      = AMBA_DSP_SYNC_CMD_BUF_VDSP2_5TH_VIN;
        pSyncBufCtrl->CurCmdSeqNo             = &VcapCurCmdSeqNo;

        /* DSP_SYNC_CMD_BUF_VDSP2_6TH_VIN */
        dsp_osal_typecast(&pSyncBufCtrl, &DspProtBuf.SyncCmdBufCtrl[AMBA_DSP_SYNC_CMD_BUF_VDSP2_6TH_VIN].Base);
        pSyncBufCtrl->SyncCounter             = DSP_SYNC_COUNTER_CMD_REQ_VDSP1;
        pSyncBufCtrl->MaxNumCmds              = DSP_MAX_NUM_VCAP_CMD;
        pSyncBufCtrl->MaxNumCmdBuf            = AMBA_MAX_NUM_DSP_VCAP_CMD_BUF;
        dsp_osal_typecast(&pSyncBufCtrl->pCmdBufBaseAddr, &DspProtBuf.VinCmdBuf[5U].Base);
        dsp_osal_typecast(&pSyncBufCtrl->pGroupCmdBufBaseAddr, &DspProtBuf.GroupVinCmdBuf[5U].Base);
        dsp_osal_typecast(&pSyncBufCtrl->MsgBufCtrl.pMsgFifo, &DspProtBuf.VCapMsgBuf.Base);
        dsp_osal_typecast(&pSyncBufCtrl->MsgBufCtrl.pMsgFifoCtrl, &DspProtBuf.VCapMsgFifoCtrl.Base);
        pSyncBufCtrl->MsgBufCtrl.pCmdReqRptr  = &VcapCmdReqRptr;
        pSyncBufCtrl->MsgBufCtrl.DspMsgRp     = 0U;
        pSyncBufCtrl->MsgBufCtrl.UnReadMsgNum = 0U;
        pSyncBufCtrl->MsgBufCtrl.DspMsgLastPtr= 0U;
        pSyncBufCtrl->Id                      = AMBA_DSP_SYNC_CMD_BUF_VDSP2_6TH_VIN;
        pSyncBufCtrl->CurCmdSeqNo             = &VcapCurCmdSeqNo;

        // AmbaDSP_GeneralMsgFifoCtrl
        dsp_osal_typecast(&pDspMsgQInfo, &DspProtBuf.GeneralMsgFifoCtrl.Base);
        Rval = dsp_osal_memset(pDspMsgQInfo, 0, DspProtBuf.GeneralMsgFifoCtrl.Size);
        LL_PrintErrLine(Rval, __func__, __LINE__);
        Rval = dsp_osal_virt2cli(DspProtBuf.GeneralMsgBuf.Base, &pDspMsgQInfo->base_daddr);
        LL_PrintErrLine(Rval, __func__, __LINE__);
        pDspMsgQInfo->max_num_msg    = DSP_MAX_NUM_GENERAL_MSG;
        pDspMsgQInfo->read_ptr       = 0U;
        pDspMsgQInfo->write_ptr      = 0U;

        // AmbaDSP_VCapMsgFifoCtrl
        dsp_osal_typecast(&pDspMsgQInfo, &DspProtBuf.VCapMsgFifoCtrl.Base);
        Rval = dsp_osal_memset(pDspMsgQInfo, 0, DspProtBuf.VCapMsgFifoCtrl.Size);
        LL_PrintErrLine(Rval, __func__, __LINE__);
        Rval = dsp_osal_virt2cli(DspProtBuf.VCapMsgBuf.Base, &pDspMsgQInfo->base_daddr);
        LL_PrintErrLine(Rval, __func__, __LINE__);
        pDspMsgQInfo->max_num_msg    = DSP_MAX_NUM_VCAP_MSG;
        pDspMsgQInfo->read_ptr       = 0U;
        pDspMsgQInfo->write_ptr      = 0U;

        // AmbaDSP_VprocMsgFifoCtrl
        dsp_osal_typecast(&pDspMsgQInfo, &DspProtBuf.VProcMsgFifoCtrl.Base);
        Rval = dsp_osal_memset(pDspMsgQInfo, 0, DspProtBuf.VProcMsgFifoCtrl.Size);
        LL_PrintErrLine(Rval, __func__, __LINE__);
        Rval = dsp_osal_virt2cli(DspProtBuf.VProcMsgBuf.Base, &pDspMsgQInfo->base_daddr);
        LL_PrintErrLine(Rval, __func__, __LINE__);
        pDspMsgQInfo->max_num_msg    = DSP_MAX_NUM_VPROC_MSG;
        pDspMsgQInfo->read_ptr       = 0U;
        pDspMsgQInfo->write_ptr      = 0U;

#ifdef SUPPORT_DSP_ASYNC_ENCMSG
        dsp_osal_typecast(&pDspMsgQInfo, &DspProtBuf.AsyncEncMsgFifoCtrl.Base);
        Rval = dsp_osal_memset(pDspMsgQInfo, 0, DspProtBuf.AsyncEncMsgFifoCtrl.Size);
        LL_PrintErrLine(Rval, __func__, __LINE__);
        Rval = dsp_osal_virt2cli(DspProtBuf.AsyncEncMsgBuf.Base, &pDspMsgQInfo->base_daddr);
        LL_PrintErrLine(Rval, __func__, __LINE__);
        pDspMsgQInfo->max_num_msg    = DSP_MAX_NUM_ENC_MSG;
        pDspMsgQInfo->read_ptr       = 0U;
        pDspMsgQInfo->write_ptr      = 0U;
#endif

        // AmbaDSP_VPMsgFifoCtrl
        dsp_osal_typecast(&pDspMsgQInfo, &DspProtBuf.VPMsgFifoCtrl.Base);
        Rval = dsp_osal_memset(pDspMsgQInfo, 0, DspProtBuf.VPMsgFifoCtrl.Size);
        LL_PrintErrLine(Rval, __func__, __LINE__);
        Rval = dsp_osal_virt2cli(DspProtBuf.VpMsgBuf.Base, &pDspMsgQInfo->base_daddr);
        LL_PrintErrLine(Rval, __func__, __LINE__);
        pDspMsgQInfo->max_num_msg    = DSP_MAX_NUM_VP_MSG;
        pDspMsgQInfo->read_ptr       = 0U;
        pDspMsgQInfo->write_ptr      = 0U;

        // Pure Msg
        dsp_osal_typecast(&pMsgBufCtrl, &DspProtBuf.VProcMsgBufCtrl.Base);
        Rval = dsp_osal_memset(pMsgBufCtrl, 0, DspProtBuf.VProcMsgBufCtrl.Size);
        LL_PrintErrLine(Rval, __func__, __LINE__);
        pMsgBufCtrl->pCmdReqRptr     = &VProcCmdReqRptr;
        dsp_osal_typecast(&pMsgBufCtrl->pMsgFifo, &DspProtBuf.VProcMsgBuf.Base);
        dsp_osal_typecast(&pMsgBufCtrl->pMsgFifoCtrl, &DspProtBuf.VProcMsgFifoCtrl.Base);

#ifdef SUPPORT_DSP_ASYNC_ENCMSG
        dsp_osal_typecast(&pMsgBufCtrl, &DspProtBuf.AsyncEncMsgBufCtrl.Base);
        Rval = dsp_osal_memset(pMsgBufCtrl, 0, DspProtBuf.AsyncEncMsgBufCtrl.Size);
        LL_PrintErrLine(Rval, __func__, __LINE__);
        pMsgBufCtrl->pCmdReqRptr     = &AsyncCmdReqRptr;
        dsp_osal_typecast(&pMsgBufCtrl->pMsgFifo, &DspProtBuf.AsyncEncMsgBuf.Base);
        dsp_osal_typecast(&pMsgBufCtrl->pMsgFifoCtrl, &DspProtBuf.AsyncEncMsgFifoCtrl.Base);
#endif

        /* Reset Cmds */
        // AmbaDSP_DefaultCmds
        dsp_osal_typecast(&pDspDefCmds, &DspProtBuf.DefaultCmds.Base);
        Rval = dsp_osal_memset(pDspDefCmds, 0, DspProtBuf.DefaultCmds.Size);
        LL_PrintErrLine(Rval, __func__, __LINE__);
        pDspDefCmds->Header.cmd_code = CMD_DSP_HEADER;

        // AmbaDSP_GeneralCmdBuf
        dsp_osal_typecast(&pVoid, &DspProtBuf.GeneralCmdBuf.Base);
        Rval = dsp_osal_memset(pVoid, 0, DspProtBuf.GeneralCmdBuf.Size);
        LL_PrintErrLine(Rval, __func__, __LINE__);

        // AmbaDSP_VCapCmdBuf
        dsp_osal_typecast(&pVoid, &DspProtBuf.VinCmdBuf[0U].Base);
        Rval = dsp_osal_memset(pVoid, 0, DspProtBuf.VinCmdBuf[0U].Size);
        LL_PrintErrLine(Rval, __func__, __LINE__);

        // AmbaDSP_2ndVinCmdBuf
        dsp_osal_typecast(&pVoid, &DspProtBuf.VinCmdBuf[1U].Base);
        Rval = dsp_osal_memset(pVoid, 0, DspProtBuf.VinCmdBuf[1U].Size);
        LL_PrintErrLine(Rval, __func__, __LINE__);

        // AmbaDSP_3rdVinCmdBuf
        dsp_osal_typecast(&pVoid, &DspProtBuf.VinCmdBuf[2U].Base);
        Rval = dsp_osal_memset(pVoid, 0, DspProtBuf.VinCmdBuf[2U].Size);
        LL_PrintErrLine(Rval, __func__, __LINE__);

        // AmbaDSP_4thVinCmdBuf
        dsp_osal_typecast(&pVoid, &DspProtBuf.VinCmdBuf[3U].Base);
        Rval = dsp_osal_memset(pVoid, 0, DspProtBuf.VinCmdBuf[3U].Size);
        LL_PrintErrLine(Rval, __func__, __LINE__);

        // AmbaDSP_5thVinCmdBuf
        dsp_osal_typecast(&pVoid, &DspProtBuf.VinCmdBuf[4U].Base);
        Rval = dsp_osal_memset(pVoid, 0, DspProtBuf.VinCmdBuf[4U].Size);
        LL_PrintErrLine(Rval, __func__, __LINE__);

        // AmbaDSP_6thVinCmdBuf
        dsp_osal_typecast(&pVoid, &DspProtBuf.VinCmdBuf[5U].Base);
        Rval = dsp_osal_memset(pVoid, 0, DspProtBuf.VinCmdBuf[5U].Size);
        LL_PrintErrLine(Rval, __func__, __LINE__);

        // AmbaDSP_GroupGeneralCmdBuf
        dsp_osal_typecast(&pVoid, &DspProtBuf.GroupGeneralCmdBuf.Base);
        Rval = dsp_osal_memset(pVoid, 0, DspProtBuf.GroupGeneralCmdBuf.Size);
        LL_PrintErrLine(Rval, __func__, __LINE__);

        // AmbaDSP_GroupVCapCmdBuf
        dsp_osal_typecast(&pVoid, &DspProtBuf.GroupVinCmdBuf[0U].Base);
        Rval = dsp_osal_memset(pVoid, 0, DspProtBuf.GroupVinCmdBuf[0U].Size);
        LL_PrintErrLine(Rval, __func__, __LINE__);

        // AmbaDSP_Group2ndVinCmdBuf
        dsp_osal_typecast(&pVoid, &DspProtBuf.GroupVinCmdBuf[1U].Base);
        Rval = dsp_osal_memset(pVoid, 0, DspProtBuf.GroupVinCmdBuf[1U].Size);
        LL_PrintErrLine(Rval, __func__, __LINE__);

        // AmbaDSP_Group3rdVinCmdBuf
        dsp_osal_typecast(&pVoid, &DspProtBuf.GroupVinCmdBuf[2U].Base);
        Rval = dsp_osal_memset(pVoid, 0, DspProtBuf.GroupVinCmdBuf[2U].Size);
        LL_PrintErrLine(Rval, __func__, __LINE__);

        // AmbaDSP_Group4thVinCmdBuf
        dsp_osal_typecast(&pVoid, &DspProtBuf.GroupVinCmdBuf[3U].Base);
        Rval = dsp_osal_memset(pVoid, 0, DspProtBuf.GroupVinCmdBuf[3U].Size);
        LL_PrintErrLine(Rval, __func__, __LINE__);

        // AmbaDSP_Group5thVinCmdBuf
        dsp_osal_typecast(&pVoid, &DspProtBuf.GroupVinCmdBuf[4U].Base);
        Rval = dsp_osal_memset(pVoid, 0, DspProtBuf.GroupVinCmdBuf[4U].Size);
        LL_PrintErrLine(Rval, __func__, __LINE__);

        // AmbaDSP_Group6thVinCmdBuf
        dsp_osal_typecast(&pVoid, &DspProtBuf.GroupVinCmdBuf[5U].Base);
        Rval = dsp_osal_memset(pVoid, 0, DspProtBuf.GroupVinCmdBuf[5U].Size);
        LL_PrintErrLine(Rval, __func__, __LINE__);

        // Init AmbaDSP_AssertInfo
        dsp_osal_typecast(&pVoid, &DspProtBuf.AssertInfo.Base);
        Rval = dsp_osal_memset(pVoid, 0, DspProtBuf.AssertInfo.Size);
        LL_PrintErrLine(Rval, __func__, __LINE__);

        // EncDesc
        dsp_osal_typecast(&pVoid, &DspProtBuf.VidEncDescBuf.Base);
        Rval = dsp_osal_memset(pVoid, 0, DspProtBuf.VidEncDescBuf.Size);
        LL_PrintErrLine(Rval, __func__, __LINE__);

        // VprocExtMsgQ
        dsp_osal_typecast(&pVoid, &DspProtBuf.VprocExtMsgQ.Base);
        Rval = dsp_osal_memset(pVoid, 0, DspProtBuf.VprocExtMsgQ.Size);
        LL_PrintErrLine(Rval, __func__, __LINE__);

        // IdspInfo
#ifndef USE_STAT_IDSP_BUF
        Rval = dsp_osal_memset(AmbaDSP_IdspInfo, 0, sizeof(IDSP_INFO_s)*AMBA_DSP_MAX_VIEWZONE_NUM);
        LL_PrintErrLine(Rval, __func__, __LINE__);
#else
        for (i = 0U; i < DSP_IDSP_CFG_NUM; i++) {
            dsp_osal_typecast(&pVoid, &DspStatBuf.IdspCfg[i].Base);
            Rval = dsp_osal_memset(pVoid, 0, DspStatBuf.IdspCfg[i].Size);
            LL_PrintErrLine(Rval, __func__, __LINE__);
        }
#endif

        InitBuffersFlag = 1U;
    }
}

UINT32 DSP_GetProtBuf(UINT32 Id, UINT32 AuxId, DSP_MEM_BLK_t *pBuf)
{
    UINT32 Rval = DSP_ERR_NONE;

    if (Id >= DSP_PROT_BUF_NUM) {
        Rval = DSP_ERR_0001;
    } else if (pBuf == NULL) {
        Rval = DSP_ERR_0000;
    } else {
        AmbaDSP_InitBuffers();
        Rval = AmbaDSP_GetProtBuf(&DspProtBuf, Id, AuxId, pBuf);
    }

    return Rval;
}

UINT32 DSP_GetStatBuf(UINT32 Id, UINT32 AuxId, DSP_MEM_BLK_t *pBuf)
{
    UINT32 Rval = DSP_ERR_NONE;

    if (Id >= DSP_PROT_BUF_NUM) {
        Rval = DSP_ERR_0001;
    } else if (pBuf == NULL) {
        Rval = DSP_ERR_0000;
    } else {
        AmbaDSP_InitBuffers();
        Rval = AmbaDSP_GetStatBuf(&DspStatBuf, Id, AuxId, pBuf);
    }

    return Rval;
}

UINT32 DSP_CreateAssertBuf(ULONG *pBase, UINT32 ReqSize, UINT32 Align)
{
    UINT32 Rval;
#if defined(CONFIG_ENABLE_DSP_MONITOR)
#if !defined(CONFIG_DISABLE_SHADOW_MEMORY_WITH_DSP_MONITOR)
    ULONG ULAddr;

    AmbaMisra_TouchUnused(&ReqSize);
    AmbaMisra_TouchUnused(&Align);
    Rval = DSPMon_GetAssertBufferAddr(&ULAddr);
    dsp_osal_typecast(pBase, &ULAddr);
#else
    Rval = dsp_osal_alloc_prot_buf(pBase, ReqSize, Align);
#endif
#else
    Rval = dsp_osal_alloc_prot_buf(pBase, ReqSize, Align);
#endif
    return Rval;
}
