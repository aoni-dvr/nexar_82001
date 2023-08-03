/**
 *  @file AmbaDSPSimilar_Context.c
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

#include "AmbaDSPSimilar_Context.h"

static UINT8 SimilarEnabled = 0U;
/* Mutex Table */
static osal_mutex_t CtxSimilarResMtx GNU_SECTION_NOZEROINIT;

/* Ctx Table */
static CTX_SIMILAR_INFO_s CtxSimilarResInfo GNU_SECTION_NOZEROINIT;

/* Cmd buffer pool */
#define CTX_CMD_BUF_POOL_NUM            (8U)
typedef struct {
    ULONG BaseAddr;
    UINT8 PoolUsage[CTX_CMD_BUF_POOL_NUM];
    osal_mutex_t PoolMtx;
} CTX_SIMILAR_CMD_BUF_INFO_s;

static CTX_SIMILAR_CMD_BUF_INFO_s CtxSimilarCmdBufInfo GNU_SECTION_NOZEROINIT;

UINT32 HL_SimilarCtxInit(void)
{
    static dsp_cmd_t SIM_DefCtxCmdBuf GNU_SECTION_NOZEROINIT;
    static dsp_cmd_t CtxSimilarCmdBuf[CTX_CMD_BUF_POOL_NUM] GNU_SECTION_NOZEROINIT;
    UINT32 Rval = OK;
    UINT8 i = 0U;
    static UINT8 CtxInit = 0U;

    if (CtxInit == 0U) {
        static char SimilarResCtxMtx[] = "DspSimilarResCtxMtx";

        SimilarEnabled = 1U;
        Rval |= dsp_osal_mutex_init(&CtxSimilarResMtx, SimilarResCtxMtx);

        for (i = 0U; i<AMBA_DSP_MAX_VIEWZONE_NUM; i++) {
            CtxSimilarResInfo.BatchQPoolDesc[i].StartAddr = 0U;
            CtxSimilarResInfo.BatchQPoolDesc[i].Size = MAX_BATCH_CMD_POOL_NUM;
            CtxSimilarResInfo.BatchQPoolDesc[i].Wp = MAX_BATCH_CMD_POOL_NUM - 1U;
            CtxSimilarResInfo.BatchQPoolDesc[i].Rp = MAX_BATCH_CMD_POOL_NUM - 1U;
        }

        CtxSimilarResInfo.MaxVprocTileWidth = 0U;

        /* Init Cmd buffer pool */
        {
            static char CmdBufMtxStr[] = "DspCtxCmdBufMtx";
            const dsp_cmd_t *pCtxCmdBuf = CtxSimilarCmdBuf;

            (void)dsp_osal_mutex_init(&CtxSimilarCmdBufInfo.PoolMtx, CmdBufMtxStr);
            (void)dsp_osal_memset(CtxSimilarCmdBuf, 0, sizeof(CtxSimilarCmdBuf));
            dsp_osal_typecast(&CtxSimilarCmdBufInfo.BaseAddr, &pCtxCmdBuf);
            for (i = 0U; i<CTX_CMD_BUF_POOL_NUM; i++) {
                CtxSimilarCmdBufInfo.PoolUsage[i] = 0U;
            }

            pCtxCmdBuf = &SIM_DefCtxCmdBuf;

            dsp_osal_typecast(&SIM_DefCtxCmdBufPtrSys, &pCtxCmdBuf);
            dsp_osal_typecast(&SIM_DefCtxCmdBufPtrHal, &pCtxCmdBuf);
            dsp_osal_typecast(&SIM_DefCtxCmdBufPtrBind, &pCtxCmdBuf);
            dsp_osal_typecast(&SIM_DefCtxCmdBufPtrVpcMax, &pCtxCmdBuf);
            dsp_osal_typecast(&SIM_DefCtxCmdBufPtrVinMax, &pCtxCmdBuf);
            dsp_osal_typecast(&SIM_DefCtxCmdBufPtrEncMax, &pCtxCmdBuf);
            dsp_osal_typecast(&SIM_DefCtxCmdBufPtrDecMax, &pCtxCmdBuf);

            dsp_osal_typecast(&SIM_DefCtxCmdBufPtrVpc, &pCtxCmdBuf);
            dsp_osal_typecast(&SIM_DefCtxCmdBufPtrVpcSetup, &pCtxCmdBuf);
            //dsp_osal_typecast(&SIM_DefCtxCmdBufPtrVpcIkCfg, &pCtxCmdBuf);
            dsp_osal_typecast(&SIM_DefCtxCmdBufPtrVpcPymd, &pCtxCmdBuf);
            dsp_osal_typecast(&SIM_DefCtxCmdBufPtrVpcPrev, &pCtxCmdBuf);
            dsp_osal_typecast(&SIM_DefCtxCmdBufPtrVpcLndt, &pCtxCmdBuf);
            dsp_osal_typecast(&SIM_DefCtxCmdBufPtrVpcChOrd, &pCtxCmdBuf);
            dsp_osal_typecast(&SIM_DefCtxCmdBufPtrVpcGrp, &pCtxCmdBuf);
            dsp_osal_typecast(&SIM_DefCtxCmdBufPtrVpcDeci, &pCtxCmdBuf);

            dsp_osal_typecast(&SIM_DefCtxCmdBufPtrVinStart, &pCtxCmdBuf);
            dsp_osal_typecast(&SIM_DefCtxCmdBufPtrVinCe, &pCtxCmdBuf);
            dsp_osal_typecast(&SIM_DefCtxCmdBufPtrVinHdr, &pCtxCmdBuf);
            dsp_osal_typecast(&SIM_DefCtxCmdBufPtrVinLayout, &pCtxCmdBuf);
            dsp_osal_typecast(&SIM_DefCtxCmdBufPtrVinSidBend, &pCtxCmdBuf);
            dsp_osal_typecast(&SIM_DefCtxCmdBufPtrVinInitCap, &pCtxCmdBuf);

            dsp_osal_typecast(&SIM_DefCtxCmdBufPtrVoutOsd, &pCtxCmdBuf);

            dsp_osal_typecast(&SIM_DefCtxCmdBufPtrVinCfg, &pCtxCmdBuf);

        }
        CtxInit = 1U;
    }

    return Rval;
}

void HL_GetSimilarInfo(const UINT8 MtxOpt, CTX_SIMILAR_INFO_s *pInfo)
{
    (void)HL_MtxLock(&CtxSimilarResMtx, MtxOpt);
    (void)dsp_osal_memcpy(pInfo, &CtxSimilarResInfo, sizeof(CTX_SIMILAR_INFO_s));
    (void)HL_MtxUnLock(&CtxSimilarResMtx, MtxOpt);
}

void HL_SetSimilarInfo(const UINT8 MtxOpt, const CTX_SIMILAR_INFO_s *pInfo)
{
    (void)HL_MtxLock(&CtxSimilarResMtx, MtxOpt);
    (void)dsp_osal_memcpy(&CtxSimilarResInfo, pInfo, sizeof(CTX_SIMILAR_INFO_s));
    (void)HL_MtxUnLock(&CtxSimilarResMtx, MtxOpt);
}

void SIM_UpdateCtxViewZoneBatchQWPtr(const UINT32 ViewZoneId)
{
    CTX_SIMILAR_INFO_s SimilarInfo = {0};
    UINT32 NewWp = 0U;
    UINT32 Rval;

    HL_GetSimilarInfo(HL_MTX_OPT_GET, &SimilarInfo);
    Rval = DSP_ReqBuf(&SimilarInfo.BatchQPoolDesc[ViewZoneId], 1U, &NewWp, 1U/*FullnessCheck*/);
    if (Rval != OK) {
        HL_SetSimilarInfo(HL_MTX_OPT_SET, &SimilarInfo);
    } else {
        SimilarInfo.BatchQPoolDesc[ViewZoneId].Wp = NewWp;
        HL_SetSimilarInfo(HL_MTX_OPT_SET, &SimilarInfo);
    }
}

void SIM_UpdateCtxViewZoneBatchQRPtr(const UINT32 BatchCmdId)
{
    if (BatchCmdId > 0U) {
        UINT16 ViewZoneIdx, BatchCmdQPool;

        ViewZoneIdx = (UINT16)(BatchCmdId >> 16);
        BatchCmdQPool = (UINT16)(BatchCmdId & 0x0000ffffU) - 1U;

        if (ViewZoneIdx < AMBA_DSP_MAX_VIEWZONE_NUM) {
            CTX_SIMILAR_INFO_s SimilarInfo = {0};

            HL_GetSimilarInfo(HL_MTX_OPT_GET, &SimilarInfo);
            if (SimilarInfo.BatchQPoolDesc[ViewZoneIdx].Wp == SimilarInfo.BatchQPoolDesc[ViewZoneIdx].Rp) {
                // Nothing to be read
            } else{
                UINT8 ExitLoop = 0U;
                UINT32 TempPtr = SimilarInfo.BatchQPoolDesc[ViewZoneIdx].Rp;

                if (TempPtr == BatchCmdQPool) {
                    // Already update
                } else {
                    // Find batch pool index from mapping table
                    while ((TempPtr != SimilarInfo.BatchQPoolDesc[ViewZoneIdx].Wp) && (ExitLoop == 0U)) {
                        TempPtr = (TempPtr + 1U) % (SimilarInfo.BatchQPoolDesc[ViewZoneIdx].Size);

                        if (TempPtr == BatchCmdQPool) {
                            ExitLoop = 1U;
                        }
                    }
                    if (ExitLoop == 1U) {
                        SimilarInfo.BatchQPoolDesc[ViewZoneIdx].Rp = TempPtr;
                    }
                }
            }
            HL_SetSimilarInfo(HL_MTX_OPT_SET, &SimilarInfo);
        }
    }
}

void SIM_ResetViewZoneInfo(void)
{
    UINT8 i;
    (void)HL_MtxLock(&CtxSimilarResMtx, HL_MTX_OPT_ALL);

    (void)dsp_osal_memset(&CtxSimilarResInfo, 0, sizeof(CTX_SIMILAR_INFO_s));
    for (i = 0U; i<AMBA_DSP_MAX_VIEWZONE_NUM; i++) {
        CtxSimilarResInfo.BatchQPoolDesc[i].StartAddr = 0U;
        CtxSimilarResInfo.BatchQPoolDesc[i].Size = MAX_BATCH_CMD_POOL_NUM;
        CtxSimilarResInfo.BatchQPoolDesc[i].Wp = MAX_BATCH_CMD_POOL_NUM - 1U;
        CtxSimilarResInfo.BatchQPoolDesc[i].Rp = MAX_BATCH_CMD_POOL_NUM - 1U;
    }

    (void)HL_MtxUnLock(&CtxSimilarResMtx, HL_MTX_OPT_ALL);
}

void SIM_CtxLvReset(void)
{
    CtxSimilarResInfo.MaxVprocTileWidth = 0U;
}

UINT8 IsSimilarEnabled(void)
{
    return SimilarEnabled;
}

cmd_dsp_config_t *SIM_DefCtxCmdBufPtrSys;
cmd_dsp_hal_inf_t *SIM_DefCtxCmdBufPtrHal;
cmd_binding_cfg_t *SIM_DefCtxCmdBufPtrBind;
cmd_dsp_vproc_flow_max_cfg_t *SIM_DefCtxCmdBufPtrVpcMax;
cmd_dsp_vin_flow_max_cfg_t *SIM_DefCtxCmdBufPtrVinMax;
cmd_dsp_enc_flow_max_cfg_t *SIM_DefCtxCmdBufPtrEncMax;
cmd_dsp_dec_flow_max_cfg_t *SIM_DefCtxCmdBufPtrDecMax;

cmd_vproc_cfg_t *SIM_DefCtxCmdBufPtrVpc;
cmd_vproc_setup_t *SIM_DefCtxCmdBufPtrVpcSetup;
//cmd_vproc_ik_config_t *SIM_DefCtxCmdBufPtrVpcIkCfg;
cmd_vproc_img_pyramid_setup_t *SIM_DefCtxCmdBufPtrVpcPymd;
cmd_vproc_prev_setup_t *SIM_DefCtxCmdBufPtrVpcPrev;
cmd_vproc_lane_det_setup_t *SIM_DefCtxCmdBufPtrVpcLndt;
cmd_vproc_set_vproc_grping *SIM_DefCtxCmdBufPtrVpcGrp;
cmd_vproc_multi_chan_proc_order_t *SIM_DefCtxCmdBufPtrVpcChOrd;
cmd_vproc_pin_out_deci_t *SIM_DefCtxCmdBufPtrVpcDeci;

cmd_vin_start_t *SIM_DefCtxCmdBufPtrVinStart;
cmd_vin_ce_setup_t *SIM_DefCtxCmdBufPtrVinCe;
cmd_vin_hdr_setup_t *SIM_DefCtxCmdBufPtrVinHdr;
cmd_vin_set_fov_layout_t *SIM_DefCtxCmdBufPtrVinLayout;
cmd_vin_initiate_raw_cap_to_ext_buf_t *SIM_DefCtxCmdBufPtrVinInitCap;
cmd_vin_attach_sideband_info_to_cap_frm_t *SIM_DefCtxCmdBufPtrVinSidBend;

cmd_vout_osd_setup_t *SIM_DefCtxCmdBufPtrVoutOsd;

set_vin_config_t *SIM_DefCtxCmdBufPtrVinCfg;

UINT32 SIM_CalcCheckSum32Add(const UINT32 *pBuffer, UINT32 Size, UINT32 CrcIn)
{
    UINT32 Crc = CrcIn;
    UINT32 Idx, IdxMax = Size/(UINT32)sizeof(UINT32);

    for(Idx = 0U; Idx < IdxMax; Idx++) {
        Crc = (((Crc << 1U) ^ ((UINT32)(~(Crc >> 31U)) & 1U))) ^ pBuffer[Idx];
    }

    return Crc;
}

void SIM_AcqCmdBuffer(UINT8 *Id, void **BufferAddr)
{
    UINT8 Idx;
    ULONG AcqBufferAddr = 0U;

    (void)HL_MtxLock(&CtxSimilarCmdBufInfo.PoolMtx, HL_MTX_OPT_GET);
    for (Idx=0U; Idx<CTX_CMD_BUF_POOL_NUM; Idx++) {
        if (CtxSimilarCmdBufInfo.PoolUsage[Idx] == 0U) {
            CtxSimilarCmdBufInfo.PoolUsage[Idx] = 1U;
            *Id = Idx;
            AcqBufferAddr = CtxSimilarCmdBufInfo.BaseAddr + (ULONG)(Idx*sizeof(dsp_cmd_t));
            dsp_osal_typecast(BufferAddr, &AcqBufferAddr);
            (void)dsp_osal_memset(*BufferAddr, 0, sizeof(dsp_cmd_t));
            break;
        }
    }
    (void)HL_MtxUnLock(&CtxSimilarCmdBufInfo.PoolMtx, HL_MTX_OPT_SET);
    if (AcqBufferAddr == 0U) {
        *Id = CTX_CMD_BUF_POOL_NUM;
        dsp_osal_typecast(BufferAddr, &HL_DefCtxCmdBufPtrSys);
//        AmbaLL_LogUInt5("CtxCmdBufInfo pool full", 0U, 0U, 0U, 0U, 0U);
        dsp_osal_print_flush();
    }
}

void SIM_RelCmdBuffer(const UINT8 Id)
{
    if (Id < CTX_CMD_BUF_POOL_NUM) {
        (void)HL_MtxLock(&CtxSimilarCmdBufInfo.PoolMtx, HL_MTX_OPT_GET);
        CtxSimilarCmdBufInfo.PoolUsage[Id] = 0U;
        (void)HL_MtxUnLock(&CtxSimilarCmdBufInfo.PoolMtx, HL_MTX_OPT_SET);
    }
}
