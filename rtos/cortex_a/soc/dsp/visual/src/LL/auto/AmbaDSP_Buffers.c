/**
 *  @file AmbaDSP_Buffers.c
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
 *  @details Declarations of all DSP various non-cached buffers
 *
 */

#include "AmbaDSP_Buffers.h"
#include "AmbaDSP_CommonAPI.h"
#include "AmbaDSP_Log.h"
#include "AmbaDSP_Image3aStatistics.h"

/*---------------------------------------------------------------------------*\
 * Idsp debug Buffer, TBM to context utility?
\*---------------------------------------------------------------------------*/
#ifndef USE_STAT_IDSP_BUF
IDSP_INFO_s AmbaDSP_IdspInfo[AMBA_DSP_MAX_VIEWZONE_NUM] GNU_ALIGNED_CACHESAFE GNU_SECTION_NOZEROINIT;
#endif

void DSP_GetBitsDescBufferAddr(ULONG *pULAddr)
{
    DSP_MEM_BLK_t MemBlk = {0};
    UINT32 Rval;

    Rval = DSP_GetProtBuf(DSP_PROT_BUF_VENC_DESC_BUF, 0U, &MemBlk);
    if (Rval != OK) {
        AmbaLL_LogUInt5("[Err] DSP_GetProtBuf[%d][%d] return NG [0x%X][%d]", DSP_PROT_BUF_VENC_DESC_BUF, 0U, Rval, __LINE__, 0U);
    } else {
        *pULAddr = MemBlk.Base;
    }
}

void DSP_GetVprocExtMsgBufferInfo(ULONG *pULAddr)
{
    DSP_MEM_BLK_t MemBlk = {0};
    UINT32 Rval;

    Rval = DSP_GetProtBuf(DSP_PROT_BUF_VPROC_EXT_MSG_Q, 0U, &MemBlk);
    if (Rval != OK) {
        AmbaLL_LogUInt5("[Err] DSP_GetProtBuf[%d][%d] return[0x%X] @%d", DSP_PROT_BUF_VPROC_EXT_MSG_Q, 0U, Rval, __LINE__, 0U);
    } else {
        *pULAddr = MemBlk.Base;
    }
}

/*---------------------------------------------------------------------------*\
 * DSP_ENC_CFG
\*---------------------------------------------------------------------------*/
void DSP_GetDspEncStrmCfg(const UINT16 Index, DSP_ENC_CFG **pInfo)
{
    DSP_MEM_BLK_t MemBlk = {0};
    DSP_ENC_CFG *pEncCfg;
    ULONG ULAddr;
    UINT32 Rval;

    Rval = DSP_GetProtBuf(DSP_PROT_BUF_ENC_STRM_CFG, 0U, &MemBlk);
    if (Rval != OK) {
        AmbaLL_LogUInt5("[Err] DSP_GetProtBuf[%d][%d] return[0x%X] @%d", DSP_PROT_BUF_ENC_STRM_CFG, 0U, Rval, __LINE__, 0U);
    } else {
        ULAddr = MemBlk.Base + (sizeof(DSP_ENC_CFG)*Index);
        dsp_osal_typecast(&pEncCfg, &ULAddr);
        *pInfo = pEncCfg;
    }
}

/*---------------------------------------------------------------------------*\
 * VIN_CONFIG
\*---------------------------------------------------------------------------*/
void DSP_GetDspVinCfg(const UINT16 Index, AMBA_DSP_VIN_MAIN_CONFIG_DATA_s **pInfo)
{
    DSP_MEM_BLK_t MemBlk = {0};
    AMBA_DSP_VIN_MAIN_CONFIG_DATA_s *pVinMainCfg;
    ULONG ULAddr;
    UINT32 Rval;

    Rval = DSP_GetProtBuf(DSP_PROT_BUF_VIN_MAIN_CFG, Index, &MemBlk);
    if (Rval != OK) {
        AmbaLL_LogUInt5("[Err] DSP_GetProtBuf[%d][%d] return [0x%X] @%d", DSP_PROT_BUF_VIN_MAIN_CFG, Index, Rval, __LINE__, 0U);
    } else {
        ULAddr = MemBlk.Base;
        dsp_osal_typecast(&pVinMainCfg, &ULAddr);
        *pInfo = pVinMainCfg;
    }
}

/*---------------------------------------------------------------------------*\
 * VOUT_DISP_CONFIG
\*---------------------------------------------------------------------------*/
void DSP_GetDspVoutDispCfg(const UINT16 Index, AMBA_DSP_VOUT_DISP_CFG_DATA_s **pInfo)
{
    DSP_MEM_BLK_t MemBlk = {0};
    AMBA_DSP_VOUT_DISP_CFG_DATA_s *pVoutDispCfg;
    ULONG ULAddr;
    UINT32 Rval;

    Rval = DSP_GetProtBuf(DSP_PROT_BUF_VOUT_DISP_CFG, Index, &MemBlk);
    if (Rval != OK) {
        AmbaLL_LogUInt5("[Err] DSP_GetProtBuf[%d][%d] return [0x%X] @%d", DSP_PROT_BUF_VOUT_DISP_CFG, Index, Rval, __LINE__, 0U);
    } else {
        ULAddr = MemBlk.Base;
        dsp_osal_typecast(&pVoutDispCfg, &ULAddr);
        *pInfo = pVoutDispCfg;
    }
}

/*---------------------------------------------------------------------------*\
 * VOUT_MIXER_CSC_CONFIG
\*---------------------------------------------------------------------------*/
void DSP_GetDspVoutMixerCscCfg(const UINT16 Index, AMBA_DSP_VOUT_MIXER_CSC_DATA_s **pInfo)
{
    DSP_MEM_BLK_t MemBlk = {0};
    AMBA_DSP_VOUT_MIXER_CSC_DATA_s *pVoutMixerCsc;
    ULONG ULAddr;
    UINT32 Rval;

    Rval = DSP_GetProtBuf(DSP_PROT_BUF_VOUT_MIXER_CSC_CFG, Index, &MemBlk);
    if (Rval != OK) {
        AmbaLL_LogUInt5("[Err] DSP_GetProtBuf[%d][%d] return [0x%X] @%d", DSP_PROT_BUF_VOUT_MIXER_CSC_CFG, Index, Rval, __LINE__, 0U);
    } else {
        ULAddr = MemBlk.Base;
        dsp_osal_typecast(&pVoutMixerCsc, &ULAddr);
        *pInfo = pVoutMixerCsc;
    }
}

/*---------------------------------------------------------------------------*\
 * VOUT_TVE_CONFIG
\*---------------------------------------------------------------------------*/
void DSP_GetDspVoutTveCfg(const UINT16 Index, AMBA_DSP_VOUT_TVE_DATA_s **pInfo)
{
    DSP_MEM_BLK_t MemBlk = {0};
    AMBA_DSP_VOUT_TVE_DATA_s *pVoutTve;
    ULONG ULAddr;
    UINT32 Rval;

    Rval = DSP_GetProtBuf(DSP_PROT_BUF_VOUT_TVE_CFG, Index, &MemBlk);
    if (Rval != OK) {
        AmbaLL_LogUInt5("[Err] DSP_GetProtBuf[%d][%d] return [0x%X] @%d", DSP_PROT_BUF_VOUT_TVE_CFG, Index, Rval, __LINE__, 0U);
    } else {
        ULAddr = MemBlk.Base;
        dsp_osal_typecast(&pVoutTve, &ULAddr);
        *pInfo = pVoutTve;
    }
}

/*---------------------------------------------------------------------------*\
 * External RawBuffer table
\*---------------------------------------------------------------------------*/
void DSP_GetDspExtRawBufArray(const UINT16 VinIdx, const UINT16 Type, UINT32 **pInfo)
{
    DSP_MEM_BLK_t MemBlk = {0};
    UINT32 *pU32;
    ULONG ULAddr;
    UINT32 Rval;

    Rval = DSP_GetProtBuf(DSP_PROT_BUF_EXT_RAW_BUF_ARRAY, 0U, &MemBlk);
    if (Rval != OK) {
        AmbaLL_LogUInt5("[Err] DSP_GetProtBuf[%d][%d] return [0x%X] @%d", DSP_PROT_BUF_EXT_RAW_BUF_ARRAY, 0U, Rval, __LINE__, 0U);
    } else {
        ULAddr = MemBlk.Base;
        ULAddr += ((sizeof(UINT32)*VinIdx)*MAX_EXT_MEM_BUF_TYPE_NUM)*MAX_EXT_MEM_BUF_NUM;
        ULAddr += (sizeof(UINT32)*Type)*MAX_EXT_MEM_BUF_NUM;
        dsp_osal_typecast(&pU32, &ULAddr);
        *pInfo = pU32;
    }
}

/*---------------------------------------------------------------------------*\
 * PpStrmInfo
\*---------------------------------------------------------------------------*/
void DSP_GetDspPpStrmCtrlCfg(const UINT16 YuvStrmIdx, const UINT16 BufIdx, const UINT16 ChanIdx, vproc_pp_stream_cntl_t **pInfo)
{
    /* 24X32X6X16 = 72K */
    DSP_MEM_BLK_t MemBlk = {0};
    vproc_pp_stream_cntl_t *pPpStrmCtrl;
    ULONG ULAddr;
    UINT32 Rval;

    Rval = DSP_GetProtBuf(DSP_PROT_BUF_PP_STRM_CTRL, 0U, &MemBlk);
    if (Rval != OK) {
        AmbaLL_LogUInt5("[Err] DSP_GetProtBuf[%d][%d] return [0x%X] @%d", DSP_PROT_BUF_PP_STRM_CTRL, 0U, Rval, __LINE__, 0U);
    } else {
        ULAddr = MemBlk.Base;
        ULAddr += ((sizeof(vproc_pp_stream_cntl_t)*YuvStrmIdx)*MAX_GROUP_CMD_POOL_NUM)*AMBA_DSP_MAX_YUVSTRM_VIEW_NUM;
        ULAddr += (sizeof(vproc_pp_stream_cntl_t)*BufIdx)*AMBA_DSP_MAX_YUVSTRM_VIEW_NUM;
        ULAddr += sizeof(vproc_pp_stream_cntl_t)*ChanIdx;
        dsp_osal_typecast(&pPpStrmCtrl, &ULAddr);
        *pInfo = pPpStrmCtrl;
    }
}

/*---------------------------------------------------------------------------*\
 * PpCopyCfg
\*---------------------------------------------------------------------------*/
void DSP_GetDspPpStrmCopyCfg(const UINT16 YuvStrmIdx, const UINT16 BufIdx, const UINT16 ChanIdx, vproc_post_r2y_copy_cfg_t **pInfo)
{
    /* 28X32X6X16X2 = 144K */
    DSP_MEM_BLK_t MemBlk = {0};
    vproc_post_r2y_copy_cfg_t *pPpStrmCopy;
    ULONG ULAddr;
    UINT32 Rval;

    Rval = DSP_GetProtBuf(DSP_PROT_BUF_PP_STRM_COPY, 0U, &MemBlk);
    if (Rval != OK) {
        AmbaLL_LogUInt5("[Err] DSP_GetProtBuf[%d][%d] return [0x%X] @%d", DSP_PROT_BUF_PP_STRM_COPY, 0U, Rval, __LINE__, 0U);
    } else {
        ULAddr = MemBlk.Base;
        ULAddr += (((sizeof(vproc_post_r2y_copy_cfg_t)*YuvStrmIdx)*MAX_GROUP_CMD_POOL_NUM)*AMBA_DSP_MAX_YUVSTRM_VIEW_NUM)*DSP_MAX_PP_STRM_COPY_NUMBER;
        ULAddr += ((sizeof(vproc_post_r2y_copy_cfg_t)*BufIdx)*AMBA_DSP_MAX_YUVSTRM_VIEW_NUM)*DSP_MAX_PP_STRM_COPY_NUMBER;
        ULAddr += (sizeof(vproc_post_r2y_copy_cfg_t)*ChanIdx)*DSP_MAX_PP_STRM_COPY_NUMBER;
        dsp_osal_typecast(&pPpStrmCopy, &ULAddr);
        *pInfo = pPpStrmCopy;
    }
}

/*---------------------------------------------------------------------------*\
 * PpBldCfg
\*---------------------------------------------------------------------------*/
void DSP_GetDspPpStrmBldCfg(const UINT16 YuvStrmIdx, const UINT16 BufIdx, const UINT16 ChanIdx, vproc_y2y_blending_cfg_t **pInfo)
{
    /* 40X32X6X16X2 = 240K */
    DSP_MEM_BLK_t MemBlk = {0};
    vproc_y2y_blending_cfg_t *pPpStrmBld;
    ULONG ULAddr;
    UINT32 Rval;

    Rval = DSP_GetProtBuf(DSP_PROT_BUF_PP_STRM_BLEND, 0U, &MemBlk);
    if (Rval != OK) {
        AmbaLL_LogUInt5("[Err] DSP_GetProtBuf[%d][%d] return [0x%X] @%d", DSP_PROT_BUF_PP_STRM_BLEND, 0U, Rval, __LINE__, 0U);
    } else {
        ULAddr = MemBlk.Base;
        ULAddr += (((sizeof(vproc_y2y_blending_cfg_t)*YuvStrmIdx)*MAX_GROUP_CMD_POOL_NUM)*AMBA_DSP_MAX_YUVSTRM_VIEW_NUM)*DSP_MAX_PP_STRM_BLEND_NUMBER_CTX;
        ULAddr += ((sizeof(vproc_y2y_blending_cfg_t)*BufIdx)*AMBA_DSP_MAX_YUVSTRM_VIEW_NUM)*DSP_MAX_PP_STRM_BLEND_NUMBER_CTX;
        ULAddr += (sizeof(vproc_y2y_blending_cfg_t)*ChanIdx)*DSP_MAX_PP_STRM_BLEND_NUMBER_CTX;
        dsp_osal_typecast(&pPpStrmBld, &ULAddr);
        *pInfo = pPpStrmBld;
    }
}

void DSP_GetDspPpDummyBldTbl(UINT8 **pInfo)
{
    DSP_MEM_BLK_t MemBlk = {0};
    UINT8 *pU8;
    UINT32 Rval;

    Rval = DSP_GetProtBuf(DSP_PROT_BUF_PP_DUMMY_BLEND, 0U, &MemBlk);
    if (Rval != OK) {
        AmbaLL_LogUInt5("[Err] DSP_GetProtBuf[%d][%d] return [0x%X] @%d", DSP_PROT_BUF_PP_DUMMY_BLEND, 0U, Rval, __LINE__, 0U);
    } else {
        dsp_osal_typecast(&pU8, &MemBlk.Base);
        *pInfo = pU8;
    }
}

/*---------------------------------------------------------------------------*\
 * SideBandInfo
 * Every FovSideband has 3 addr buffer, each addr buffer has 4 addr
\*---------------------------------------------------------------------------*/
void DSP_GetFovSideBandInfoCfg(const UINT16 ViewZoneId, const UINT16 TokenIndex, sideband_info_t **pInfo)
{
    DSP_MEM_BLK_t MemBlk = {0};
    sideband_info_t *pSideBand;
    ULONG ULAddr;
    UINT32 Rval;

    Rval = DSP_GetProtBuf(DSP_PROT_BUF_SIDEBAND, 0U, &MemBlk);
    if (Rval != OK) {
        AmbaLL_LogUInt5("[Err] DSP_GetProtBuf[%d][%d] return [0x%X] @%d", DSP_PROT_BUF_SIDEBAND, 0U, Rval, __LINE__, 0U);
    } else {
        ULAddr = MemBlk.Base;
        ULAddr += ((sizeof(sideband_info_t)*ViewZoneId)*MAX_SIDEBAND_BUF_NUM)*MAX_SIDEBAND_BUF_ARRAY_NUM;
        ULAddr += (sizeof(sideband_info_t)*TokenIndex)*MAX_SIDEBAND_BUF_ARRAY_NUM;
        dsp_osal_typecast(&pSideBand, &ULAddr);
        *pInfo = pSideBand;
    }
}

/*---------------------------------------------------------------------------*\
 * VinFovLayout
\*---------------------------------------------------------------------------*/
void DSP_GetDspVinFovLayoutCfg(const UINT16 Index, const UINT16 LocalIndex, fov_layout_t **pInfo)
{
    DSP_MEM_BLK_t MemBlk = {0};
    fov_layout_t *pFovLayout;
    ULONG ULAddr;
    UINT32 Rval;

    Rval = DSP_GetProtBuf(DSP_PROT_BUF_FOV_LAYOUT, 0U, &MemBlk);
    if (Rval != OK) {
        AmbaLL_LogUInt5("[Err] DSP_GetProtBuf[%d][%d] return [0x%X] @%d", DSP_PROT_BUF_FOV_LAYOUT, 0U, Rval, __LINE__, 0U);
    } else {
        ULAddr = MemBlk.Base;
        ULAddr += (sizeof(fov_layout_t)*Index)*AMBA_DSP_MAX_VIEWZONE_NUM;
        ULAddr += sizeof(fov_layout_t)*LocalIndex;
        dsp_osal_typecast(&pFovLayout, &ULAddr);
        *pInfo = pFovLayout;
    }
}

/*---------------------------------------------------------------------------*\
 * VinStartFovLayout
\*---------------------------------------------------------------------------*/
void DSP_GetDspVinStartFovLayoutCfg(const UINT16 Index, const UINT16 LocalIndex, vin_fov_cfg_t **pInfo)
{
    DSP_MEM_BLK_t MemBlk = {0};
    vin_fov_cfg_t *pFovCfg;
    ULONG ULAddr;
    UINT32 Rval;

    Rval = DSP_GetProtBuf(DSP_PROT_BUF_VIN_FOV_CFG, 0U, &MemBlk);
    if (Rval != OK) {
        AmbaLL_LogUInt5("[Err] DSP_GetProtBuf[%d][%d] return [0x%X] @%d", DSP_PROT_BUF_VIN_FOV_CFG, 0U, Rval, __LINE__, 0U);
    } else {
        ULAddr = MemBlk.Base;
        ULAddr += (sizeof(vin_fov_cfg_t)*Index)*AMBA_DSP_MAX_VIEWZONE_NUM;
        ULAddr += sizeof(vin_fov_cfg_t)*LocalIndex;
        dsp_osal_typecast(&pFovCfg, &ULAddr);
        *pInfo = pFovCfg;
    }
}

/*---------------------------------------------------------------------------*\
 * Batch Cmd buffer Q
\*---------------------------------------------------------------------------*/
void DSP_GetDspBatchCmdQ(const UINT16 ViewZoneId, const UINT16 BatchIdx, UINT32 **pInfo)
{
    DSP_MEM_BLK_t MemBlk = {0};
    UINT32 *pU32;
    ULONG ULAddr;
    UINT32 Rval;

    Rval = DSP_GetProtBuf(DSP_PROT_BUF_BATCH_CMD_Q, 0U, &MemBlk);
    if (Rval != OK) {
        AmbaLL_LogUInt5("[Err] DSP_GetProtBuf[%d][%d] return [0x%X] @%d", DSP_PROT_BUF_BATCH_CMD_Q, 0U, Rval, __LINE__, 0U);
    } else {
        ULAddr = MemBlk.Base;
        ULAddr += ((sizeof(dsp_cmd_t)*ViewZoneId)*MAX_BATCH_CMD_POOL_NUM)*MAX_BATCH_CMD_NUM;
        ULAddr += (sizeof(dsp_cmd_t)*BatchIdx)*MAX_BATCH_CMD_NUM;
        dsp_osal_typecast(&pU32, &ULAddr);
        *pInfo = pU32;
    }
}

void DSP_GetDspBatchCmdSet(const UINT16 ViewZoneId, const UINT16 Idx, vin_fov_batch_cmd_set_t **pInfo)
{
    DSP_MEM_BLK_t MemBlk = {0};
    vin_fov_batch_cmd_set_t *pBatchCmdSet;
    ULONG ULAddr;
    UINT32 Rval;

    Rval = DSP_GetProtBuf(DSP_PROT_BUF_BATCH_CMD_SET, 0U, &MemBlk);
    if (Rval != OK) {
        AmbaLL_LogUInt5("[Err] DSP_GetProtBuf[%d][%d] return [0x%X] @%d", DSP_PROT_BUF_BATCH_CMD_SET, 0U, Rval, __LINE__, 0U);
    } else {
        ULAddr = MemBlk.Base;
        ULAddr += (sizeof(vin_fov_batch_cmd_set_t)*ViewZoneId)*MAX_BATCH_CMD_SET_DEPTH;
        ULAddr += sizeof(vin_fov_batch_cmd_set_t)*Idx;
        dsp_osal_typecast(&pBatchCmdSet, &ULAddr);
        *pInfo = pBatchCmdSet;
    }
}

void DSP_GetDspGroupCmdQ(const UINT16 YuvStreamId, const UINT16 SeqIdx, const UINT16 SubJobIdx, UINT32 **pInfo)
{
    DSP_MEM_BLK_t MemBlk = {0};
    UINT32 *pU32;
    ULONG ULAddr;
    UINT32 Rval;

    Rval = DSP_GetProtBuf(DSP_PROT_BUF_GROUP_CMD_Q, 0U, &MemBlk);
    if (Rval != OK) {
        AmbaLL_LogUInt5("[Err] DSP_GetProtBuf[%d][%d] return [0x%X] @%d", DSP_PROT_BUF_GROUP_CMD_Q, 0U, Rval, __LINE__, 0U);
    } else {
        ULAddr = MemBlk.Base;
        ULAddr += (((sizeof(dsp_cmd_t)*YuvStreamId)*MAX_GROUP_CMD_POOL_NUM)*MAX_GROUP_CMD_SUBJOB_NUM)*MAX_GROUP_CMD_NUM;
        ULAddr += ((sizeof(dsp_cmd_t)*SeqIdx)*MAX_GROUP_CMD_SUBJOB_NUM)*MAX_GROUP_CMD_NUM;
        ULAddr += (sizeof(dsp_cmd_t)*SubJobIdx)*MAX_GROUP_CMD_NUM;
        dsp_osal_typecast(&pU32, &ULAddr);
        *pInfo = pU32;
    }
}

void DSP_GetDspViewZoneGroupCmdQ(const UINT16 ViewZoneId, const UINT16 SeqIdx, UINT32 **pInfo)
{
    DSP_MEM_BLK_t MemBlk = {0};
    UINT32 *pU32;
    ULONG ULAddr;
    UINT32 Rval;

    Rval = DSP_GetProtBuf(DSP_PROT_BUF_VIEW_GROUP_CMD_Q, 0U, &MemBlk);
    if (Rval != OK) {
        AmbaLL_LogUInt5("[Err] DSP_GetProtBuf[%d][%d] return [0x%X] @%d", DSP_PROT_BUF_VIEW_GROUP_CMD_Q, 0U, Rval, __LINE__, 0U);
    } else {
        ULAddr = MemBlk.Base;
        ULAddr += ((sizeof(dsp_cmd_t)*ViewZoneId)*MAX_GROUP_CMD_POOL_NUM)*MAX_GROUP_CMD_NUM;
        ULAddr += (sizeof(dsp_cmd_t)*SeqIdx)*MAX_GROUP_CMD_NUM;
        dsp_osal_typecast(&pU32, &ULAddr);
        *pInfo = pU32;
    }
}

/*---------------------------------------------------------------------------*\
 * Vproc grouping buffer
\*---------------------------------------------------------------------------*/
void DSP_GetDspVprocGrpingNumBuf(const UINT16 GrpId, UINT8 **pInfo)
{
    DSP_MEM_BLK_t MemBlk = {0};
    UINT8 *pU8;
    ULONG ULAddr;
    UINT32 Rval;

    Rval = DSP_GetProtBuf(DSP_PROT_BUF_GRPING_NUM, 0U, &MemBlk);
    if (Rval != OK) {
        AmbaLL_LogUInt5("[Err] DSP_GetProtBuf[%d][%d] return [0x%X] @%d", DSP_PROT_BUF_GRPING_NUM, 0U, Rval, __LINE__, 0U);
    } else {
        ULAddr = MemBlk.Base;
        ULAddr += (sizeof(UINT8)*GrpId);
        dsp_osal_typecast(&pU8, &ULAddr);
        *pInfo = pU8;
    }
}

//for CMD_VPROC_SET_VPROC_GRPING (0x02000067)
void DSP_GetDspVprocGrpingOrdBuf(const UINT16 GrpId, const UINT16 OrdId, UINT8 **pInfo)
{
    DSP_MEM_BLK_t MemBlk = {0};
    UINT8 *pU8;
    ULONG ULAddr;
    UINT32 Rval;

    Rval = DSP_GetProtBuf(DSP_PROT_BUF_VPROC_GRP_ORDER, 0U, &MemBlk);
    if (Rval != OK) {
        AmbaLL_LogUInt5("[Err] DSP_GetProtBuf[%d][%d] return [0x%X] @%d", DSP_PROT_BUF_VPROC_GRP_ORDER, 0U, Rval, __LINE__, 0U);
    } else {
        ULAddr = MemBlk.Base;
        ULAddr += (sizeof(UINT8)*GrpId)*NUM_VPROC_MAX_CHAN;
        ULAddr += sizeof(UINT8)*OrdId;
        dsp_osal_typecast(&pU8, &ULAddr);
        *pInfo = pU8;
    }
}

//for CMD_VPROC_MULTI_CHAN_PROC_ORDER (0x02000066)
void DSP_GetDspVprocOrderBuf(const UINT16 GrpId, const UINT16 OrdId, UINT8 **pInfo)
{
    DSP_MEM_BLK_t MemBlk = {0};
    UINT8 *pU8;
    ULONG ULAddr;
    UINT32 Rval;

    Rval = DSP_GetProtBuf(DSP_PROT_BUF_VPROC_ORDER, 0U, &MemBlk);
    if (Rval != OK) {
        AmbaLL_LogUInt5("[Err] DSP_GetProtBuf[%d][%d] return [0x%X] @%d", DSP_PROT_BUF_VPROC_ORDER, 0U, Rval, __LINE__, 0U);
    } else {
        ULAddr = MemBlk.Base;
        ULAddr += (sizeof(UINT8)*GrpId)*NUM_VPROC_MAX_CHAN;
        ULAddr += (ULONG)(sizeof(UINT8)*OrdId);
        dsp_osal_typecast(&pU8, &ULAddr);
        *pInfo = pU8;
    }
}

/*---------------------------------------------------------------------------*\
 * Vproc slice layout buffer
\*---------------------------------------------------------------------------*/
void DSP_GetDspVprocSliceLayoutBuf(const UINT16 ViewZoneId, DSP_SLICE_LAYOUT_s **pInfo)
{
    DSP_MEM_BLK_t MemBlk = {0};
    DSP_SLICE_LAYOUT_s *pSliceLayout;
    ULONG ULAddr;
    UINT32 Rval;

    Rval = DSP_GetProtBuf(DSP_PROT_BUF_VPROC_SLICE, 0U, &MemBlk);
    if (Rval != OK) {
        AmbaLL_LogUInt5("[Err] DSP_GetProtBuf[%d][%d] return [0x%X] @%d", DSP_PROT_BUF_VPROC_SLICE, 0U, Rval, __LINE__, 0U);
    } else {
        ULAddr = MemBlk.Base;
        ULAddr += (sizeof(DSP_SLICE_LAYOUT_s)*ViewZoneId)*MAX_TOTAL_SLICE_NUM;
        dsp_osal_typecast(&pSliceLayout, &ULAddr);
        *pInfo = pSliceLayout;
    }
}

#ifdef SUPPORT_VPROC_OSD_INSERT
void DSP_GetDspVprocOsdInsertBuf(const UINT16 StremId, UINT16 BufId, UINT16 Id, osd_insert_buf_t **pInfo)
{
    UINT32 Rval;
    DSP_MEM_BLK_t MemBlk = {0};
    osd_insert_buf_t *pOsdInsertBuf;
    ULONG ULAddr;
    UINT32 AuxId = StremId | ((UINT32)BufId << 16U);

    Rval = DSP_GetProtBuf(DSP_PROT_BUF_VPROC_OSD_INSERT, AuxId, &MemBlk);
    if (Rval != DSP_ERR_NONE) {
        AmbaLL_LogUInt5("[Err] DSP_GetProtBuf[%d][%d] return [0x%X] @%d", DSP_PROT_BUF_VPROC_OSD_INSERT, 0U, Rval, __LINE__, 0U);
    } else {
        ULAddr = MemBlk.Base;
        ULAddr += sizeof(osd_insert_buf_t)*Id;
        dsp_osal_typecast(&pOsdInsertBuf, &ULAddr);
        *pInfo = pOsdInsertBuf;
    }
}
#endif

/*---------------------------------------------------------------------------*\
 * Vproc External memory description buffer
\*---------------------------------------------------------------------------*/
void DSP_GetDspVprocExtMemDescBuf(const UINT16 ViewZoneId, ext_mem_desc_t **pInfo)
{
    DSP_MEM_BLK_t MemBlk = {0};
    ext_mem_desc_t *pExtMemDesc;
    ULONG ULAddr;
    UINT32 Rval;

    Rval = DSP_GetProtBuf(DSP_PROT_BUF_VPROC_EXT_MEM_DESC, 0U, &MemBlk);
    if (Rval != OK) {
        AmbaLL_LogUInt5("[Err] DSP_GetProtBuf[%d][%d] return [0x%X] @%d", DSP_PROT_BUF_VPROC_EXT_MEM_DESC, 0U, Rval, __LINE__, 0U);
    } else {
        ULAddr = MemBlk.Base;
        ULAddr += (sizeof(ext_mem_desc_t)*ViewZoneId)*MAX_VPROC_EXT_MEM_TYPE_NUM;
        dsp_osal_typecast(&pExtMemDesc, &ULAddr);
        *pInfo = pExtMemDesc;
    }
}

void DSP_GetDspExtStlBufArray(UINT8 StageId, UINT32 **pInfo)
{
    DSP_MEM_BLK_t MemBlk = {0};
    UINT32 *pU32;
    ULONG ULAddr;
    UINT32 Rval;

    Rval = DSP_GetProtBuf(DSP_PROT_BUF_STL_EXT_MEM_TBL, StageId, &MemBlk);
    if (Rval != OK) {
        AmbaLL_LogUInt5("[Err] DSP_GetProtBuf[%d][%d] return [0x%X] @%d", DSP_PROT_BUF_STL_EXT_MEM_TBL, StageId, Rval, __LINE__, 0U);
    } else {
        ULAddr = MemBlk.Base;
        dsp_osal_typecast(&pU32, &ULAddr);
        *pInfo = pU32;
    }
}

void DSP_GetDspStlBatchCmdQ(UINT8 StageId, UINT32 **pInfo)
{
    DSP_MEM_BLK_t MemBlk = {0};
    UINT32 *pU32;
    ULONG ULAddr;
    UINT32 Rval;

    Rval = DSP_GetProtBuf(DSP_PROT_BUF_STL_BATCH_CMD_Q, StageId, &MemBlk);
    if (Rval != OK) {
        AmbaLL_LogUInt5("[Err] DSP_GetProtBuf[%d][%d] return [0x%X] @%d", DSP_PROT_BUF_STL_BATCH_CMD_Q, StageId, Rval, __LINE__, 0U);
    } else {
        ULAddr = MemBlk.Base;
        dsp_osal_typecast(&pU32, &ULAddr);
        *pInfo = pU32;
    }
}

void DSP_GetDspExtPymdBufArray(const UINT16 ViewZoneId, UINT32 **pInfo)
{
    DSP_MEM_BLK_t MemBlk = {0};
    UINT32 *pU32;
    ULONG ULAddr;
    UINT32 Rval;

    Rval = DSP_GetProtBuf(DSP_PROT_BUF_VID_EXT_PYMD_TBL, ViewZoneId, &MemBlk);
    if (Rval != OK) {
        AmbaLL_LogUInt5("[Err] DSP_GetProtBuf[%d][%d] return [0x%X] @%d", DSP_PROT_BUF_VID_EXT_PYMD_TBL, ViewZoneId, Rval, __LINE__, 0U);
    } else {
        ULAddr = MemBlk.Base;
        dsp_osal_typecast(&pU32, &ULAddr);
        *pInfo = pU32;
    }
}

void DSP_GetDspExtLndtBufArray(const UINT16 ViewZoneId, UINT32 **pInfo)
{
    DSP_MEM_BLK_t MemBlk = {0};
    UINT32 *pU32;
    ULONG ULAddr;
    UINT32 Rval;

    Rval = DSP_GetProtBuf(DSP_PROT_BUF_VID_EXT_LNDT_TBL, ViewZoneId, &MemBlk);
    if (Rval != OK) {
        AmbaLL_LogUInt5("[Err] DSP_GetProtBuf[%d][%d] return [0x%X] @%d", DSP_PROT_BUF_VID_EXT_LNDT_TBL, ViewZoneId, Rval, __LINE__, 0U);
    } else {
        ULAddr = MemBlk.Base;
        dsp_osal_typecast(&pU32, &ULAddr);
        *pInfo = pU32;
    }
}

void DSP_GetDspExtIntMainBufArray(const UINT16 ViewZoneId, UINT32 **pInfo)
{
    DSP_MEM_BLK_t MemBlk = {0};
    UINT32 *pU32;
    ULONG ULAddr;
    UINT32 Rval;

    Rval = DSP_GetProtBuf(DSP_PROT_BUF_VID_EXT_INT_MAIN_TBL, ViewZoneId, &MemBlk);
    if (Rval != OK) {
        AmbaLL_LogUInt5("[Err] DSP_GetProtBuf[%d][%d] return [0x%X] @%d", DSP_PROT_BUF_VID_EXT_INT_MAIN_TBL, ViewZoneId, Rval, __LINE__, 0U);
    } else {
        ULAddr = MemBlk.Base;
        dsp_osal_typecast(&pU32, &ULAddr);
        *pInfo = pU32;
    }
}

void DSP_GetDspExtYuvStrmBufArray(const UINT16 YuvStrmId, UINT32 **pInfo)
{
    DSP_MEM_BLK_t MemBlk = {0};
    UINT32 *pU32;
    ULONG ULAddr;
    UINT32 Rval;

    Rval = DSP_GetProtBuf(DSP_PROT_BUF_VID_EXT_YUV_STRM_TBL, YuvStrmId, &MemBlk);
    if (Rval != OK) {
        AmbaLL_LogUInt5("[Err] DSP_GetProtBuf[%d][%d] return [0x%X] @%d", DSP_PROT_BUF_VID_EXT_YUV_STRM_TBL, YuvStrmId, Rval, __LINE__, 0U);
    } else {
        ULAddr = MemBlk.Base;
        dsp_osal_typecast(&pU32, &ULAddr);
        *pInfo = pU32;
    }
}

void DSP_GetDspExtYuvStrmAuxBufArray(const UINT16 YuvStrmId, UINT32 **pInfo)
{
    DSP_MEM_BLK_t MemBlk = {0};
    UINT32 *pU32;
    ULONG ULAddr;
    UINT32 Rval;

    Rval = DSP_GetProtBuf(DSP_PROT_BUF_VID_EXT_YUV_AUX_TBL, YuvStrmId, &MemBlk);
    if (Rval != OK) {
        AmbaLL_LogUInt5("[Err] DSP_GetProtBuf[%d][%d] return [0x%X] @%d", DSP_PROT_BUF_VID_EXT_YUV_AUX_TBL, YuvStrmId, Rval, __LINE__, 0U);
    } else {
        ULAddr = MemBlk.Base;
        dsp_osal_typecast(&pU32, &ULAddr);
        *pInfo = pU32;
    }
}

void DSP_GetDspExtVinRawBufArray(const UINT16 VinId, UINT32 **pInfo)
{
    DSP_MEM_BLK_t MemBlk = {0};
    UINT32 *pU32;
    ULONG ULAddr;
    UINT32 Rval;

    Rval = DSP_GetProtBuf(DSP_PROT_BUF_VIN_EXT_RAW_TBL, VinId, &MemBlk);
    if (Rval != OK) {
        AmbaLL_LogUInt5("[Err] DSP_GetProtBuf[%d][%d] return [0x%X] @%d", DSP_PROT_BUF_VIN_EXT_RAW_TBL, VinId, Rval, __LINE__, 0U);
    } else {
        ULAddr = MemBlk.Base;
        dsp_osal_typecast(&pU32, &ULAddr);
        *pInfo = pU32;
    }
}

void DSP_GetDspExtVinAuxBufArray(const UINT16 VinId, UINT32 **pInfo)
{
    DSP_MEM_BLK_t MemBlk = {0};
    UINT32 *pU32;
    ULONG ULAddr;
    UINT32 Rval;

    Rval = DSP_GetProtBuf(DSP_PROT_BUF_VIN_EXT_AUX_TBL, VinId, &MemBlk);
    if (Rval != OK) {
        AmbaLL_LogUInt5("[Err] DSP_GetProtBuf[%d][%d] return [0x%X] @%d", DSP_PROT_BUF_VIN_EXT_AUX_TBL, VinId, Rval, __LINE__, 0U);
    } else {
        ULAddr = MemBlk.Base;
        dsp_osal_typecast(&pU32, &ULAddr);
        *pInfo = pU32;
    }
}

void DSP_GetDspDataCapBufArray(const UINT16 Id, UINT32 **pInfo)
{
    DSP_MEM_BLK_t MemBlk = {0};
    UINT32 *pU32;
    ULONG ULAddr;
    UINT32 Rval;

    Rval = DSP_GetProtBuf(DSP_PROT_BUF_DATA_CAP_TBL, Id, &MemBlk);
    if (Rval != OK) {
        AmbaLL_LogUInt5("[Err] DSP_GetProtBuf[%d][%d] return [0x%X] @%d", DSP_PROT_BUF_DATA_CAP_TBL, Id, Rval, __LINE__, 0U);
    } else {
        ULAddr = MemBlk.Base;
        dsp_osal_typecast(&pU32, &ULAddr);
        *pInfo = pU32;
    }
}

void DSP_GetDspDataCapAuxBufArray(const UINT16 Id, UINT32 **pInfo)
{
    DSP_MEM_BLK_t MemBlk = {0};
    UINT32 *pU32;
    ULONG ULAddr;
    UINT32 Rval;

    Rval = DSP_GetProtBuf(DSP_PROT_BUF_DATA_CAP_AUX_TBL, Id, &MemBlk);
    if (Rval != OK) {
        AmbaLL_LogUInt5("[Err] DSP_GetProtBuf[%d][%d] return [0x%X] @%d", DSP_PROT_BUF_DATA_CAP_AUX_TBL, Id, Rval, __LINE__, 0U);
    } else {
        ULAddr = MemBlk.Base;
        dsp_osal_typecast(&pU32, &ULAddr);
        *pInfo = pU32;
    }
}

void DSP_GetDspEventInfoAddr(const UINT16 Id, ULONG *pULAddr)
{
    DSP_MEM_BLK_t MemBlk = {0};

    (void)DSP_GetProtBuf(DSP_PROT_BUF_EVENT, Id, &MemBlk);
    *pULAddr = MemBlk.Base;
}

void DSP_GetDspStatBufAddr(const UINT16 StatId, const UINT16 BufId, ULONG *pULAddr)
{
    DSP_MEM_BLK_t MemBlk = {0};

    (void)DSP_GetStatBuf(StatId, BufId, &MemBlk);
    *pULAddr = MemBlk.Base;
}

UINT32 DSP_GetProtectBuf(ULONG *pAddr, UINT32 *pSize)
{
    const UINT8 *pU8Val;
    ULONG ULAddr;
    UINT32 Rval = OK;
    const AMBA_DSP_CHIP_DATA_s *pChipData;
    DSP_MEM_BLK_t MemBlk = {0};

    if ((pAddr == NULL) || (pSize == NULL)) {
        Rval = DSP_ERR_0000;
    } else {
        Rval = DSP_GetProtBuf(DSP_PROT_BUF_CHIP_DATA, 0U, &MemBlk);
        if (Rval != OK) {
            AmbaLL_LogUInt5("[Err] DSP_GetProtectBuf[0x%X][%d]", Rval, __LINE__, 0U, 0U, 0U);
        } else {
            dsp_osal_typecast(&pChipData, &MemBlk.Base);
            pU8Val = &pChipData->ProtectPattern[0U];
            dsp_osal_typecast(&ULAddr, &pU8Val);
            *pAddr = ULAddr;
            *pSize = (UINT32)sizeof(pChipData->ProtectPattern[0U])*AMBA_DSP_PROTECT_DATA_LEN;
        }
    }

    return Rval;
}

void DSP_GetDspChipDataBuf(ULONG *pULAddr)
{
    DSP_MEM_BLK_t MemBlk = {0};

    (void)DSP_GetProtBuf(DSP_PROT_BUF_CHIP_DATA, 0U, &MemBlk);
    *pULAddr = MemBlk.Base;
}

UINT32 AmbaDSP_StatBufCreate(AMBA_DSP_STAT_BUF_t *pDspStatBuf)
{
    UINT32 Rval;
    UINT16 i;
    ULONG ULAddr;
    UINT32 ReqSize;
    UINT32 ErrLine = __LINE__;

    /*******************************************/
    /*               Stat Buffers              */
    /*******************************************/
    Rval = dsp_osal_get_stat_buf(&ULAddr, &ReqSize);
    if (Rval != DSP_ERR_NONE) {
        ErrLine = __LINE__;
    } else {
        pDspStatBuf->StatBuf.Base = ULAddr;
        pDspStatBuf->StatBuf.Size = ReqSize;
        pDspStatBuf->StatBuf.IsCached = 1U; //stat buffer shall be cached
    }

    /* Alloc CFA */
    for (i = 0U; i < DSP_3A_BUFFER; i++) {
        ReqSize = (UINT32)sizeof(AMBA_IK_CFA_3A_DATA_s);
        Rval = dsp_osal_alloc_stat_buf(&ULAddr, ReqSize, DSP_BUF_ADDR_ALIGN);
        if (Rval != DSP_ERR_NONE) {
            ErrLine = __LINE__;
        } else {
            pDspStatBuf->CfaStat[i].Base = ULAddr;
            pDspStatBuf->CfaStat[i].Size = ReqSize;
        }
    }

    /* Alloc PG */
    for (i = 0U; i < DSP_3A_BUFFER; i++) {
        ReqSize = (UINT32)sizeof(AMBA_IK_PG_3A_DATA_s);
        Rval = dsp_osal_alloc_stat_buf(&ULAddr, ReqSize, DSP_BUF_ADDR_ALIGN);
        if (Rval != DSP_ERR_NONE) {
            ErrLine = __LINE__;
        } else {
            pDspStatBuf->PgStat[i].Base = ULAddr;
            pDspStatBuf->PgStat[i].Size = ReqSize;
        }
    }

    /* Alloc Hist0 */
    for (i = 0U; i < DSP_3A_BUFFER; i++) {
        ReqSize = (UINT32)sizeof(AMBA_IK_CFA_HIST_STAT_s);
        Rval = dsp_osal_alloc_stat_buf(&ULAddr, ReqSize, DSP_BUF_ADDR_ALIGN);
        if (Rval != DSP_ERR_NONE) {
            ErrLine = __LINE__;
        } else {
            pDspStatBuf->Hist0Stat[i].Base = ULAddr;
            pDspStatBuf->Hist0Stat[i].Size = ReqSize;
        }
    }

    /* Alloc Hist1 */
    for (i = 0U; i < DSP_3A_BUFFER; i++) {
        ReqSize = (UINT32)sizeof(AMBA_IK_CFA_HIST_STAT_s);
        Rval = dsp_osal_alloc_stat_buf(&ULAddr, ReqSize, DSP_BUF_ADDR_ALIGN);
        if (Rval != DSP_ERR_NONE) {
            ErrLine = __LINE__;
        } else {
            pDspStatBuf->Hist1Stat[i].Base = ULAddr;
            pDspStatBuf->Hist1Stat[i].Size = ReqSize;
        }
    }

    /* Alloc Hist2 */
    for (i = 0U; i < DSP_3A_BUFFER; i++) {
        ReqSize = (UINT32)sizeof(AMBA_IK_CFA_HIST_STAT_s);
        Rval = dsp_osal_alloc_stat_buf(&ULAddr, ReqSize, DSP_BUF_ADDR_ALIGN);
        if (Rval != DSP_ERR_NONE) {
            ErrLine = __LINE__;
        } else {
            pDspStatBuf->Hist2Stat[i].Base = ULAddr;
            pDspStatBuf->Hist2Stat[i].Size = ReqSize;
        }
    }

    /* Alloc IdspCfg */
    for (i = 0U; i < DSP_IDSP_CFG_NUM; i++) {
        ReqSize = (UINT32)sizeof(IDSP_INFO_s);
        Rval = dsp_osal_alloc_stat_buf(&ULAddr, ReqSize, DSP_BUF_ADDR_ALIGN);
        if (Rval != DSP_ERR_NONE) {
            ErrLine = __LINE__;
        } else {
            pDspStatBuf->IdspCfg[i].Base = ULAddr;
            pDspStatBuf->IdspCfg[i].Size = ReqSize;
        }
    }

    if (Rval != DSP_ERR_NONE) {
        AmbaLL_LogUInt5("[Err] StatBufCreate[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

static UINT32 AmbaDSP_CreateMsgFifoBuf(AMBA_DSP_PROT_BUF_t *pDspProtBuf)
{
    UINT32 Rval;
    ULONG ULAddr;
    UINT32 ReqSize;
    UINT32 ErrLine = __LINE__;

    ReqSize = (UINT32)sizeof(dsp_msg_q_info_t);
    Rval = dsp_osal_alloc_prot_buf(&ULAddr, ReqSize, DSP_BUF_ADDR_ALIGN);
    if (Rval != DSP_ERR_NONE) {
        ErrLine = __LINE__;
    } else {
        pDspProtBuf->GeneralMsgFifoCtrl.Base = ULAddr;
        pDspProtBuf->GeneralMsgFifoCtrl.Size = ReqSize;
    }

    /* Alloc VCapMsgFifoCtrl */
    if (Rval == DSP_ERR_NONE) {
        ReqSize = (UINT32)sizeof(dsp_msg_q_info_t);
        Rval = dsp_osal_alloc_prot_buf(&ULAddr, ReqSize, DSP_BUF_ADDR_ALIGN);
        if (Rval != DSP_ERR_NONE) {
            ErrLine = __LINE__;
        } else {
            pDspProtBuf->VCapMsgFifoCtrl.Base = ULAddr;
            pDspProtBuf->VCapMsgFifoCtrl.Size = ReqSize;
        }
    }

    /* Alloc VProcMsgFifoCtrl */
    if (Rval == DSP_ERR_NONE) {
        ReqSize = (UINT32)sizeof(dsp_msg_q_info_t);
        Rval = dsp_osal_alloc_prot_buf(&ULAddr, ReqSize, DSP_BUF_ADDR_ALIGN);
        if (Rval != DSP_ERR_NONE) {
            ErrLine = __LINE__;
        } else {
            pDspProtBuf->VProcMsgFifoCtrl.Base = ULAddr;
            pDspProtBuf->VProcMsgFifoCtrl.Size = ReqSize;
        }
    }

    /* Alloc AsyncEncMsgFifoCtrl */
#ifdef SUPPORT_DSP_ASYNC_ENCMSG
    if (Rval == DSP_ERR_NONE) {
        ReqSize = (UINT32)sizeof(dsp_msg_q_info_t);
        Rval = dsp_osal_alloc_prot_buf(&ULAddr, ReqSize, DSP_BUF_ADDR_ALIGN);
        if (Rval != DSP_ERR_NONE) {
            ErrLine = __LINE__;
        } else {
            pDspProtBuf->AsyncEncMsgFifoCtrl.Base = ULAddr;
            pDspProtBuf->AsyncEncMsgFifoCtrl.Size = ReqSize;
        }
    }
#endif

    /* Alloc VPMsgFifoCtrl */
    if (Rval == DSP_ERR_NONE) {
        ReqSize = (UINT32)sizeof(dsp_msg_q_info_t);
        Rval = dsp_osal_alloc_prot_buf(&ULAddr, ReqSize, DSP_BUF_ADDR_ALIGN);
        if (Rval != DSP_ERR_NONE) {
            ErrLine = __LINE__;
        } else {
            pDspProtBuf->VPMsgFifoCtrl.Base = ULAddr;
            pDspProtBuf->VPMsgFifoCtrl.Size = ReqSize;
        }
    }

    if (Rval != DSP_ERR_NONE) {
        AmbaLL_LogUInt5("[Err] CreateMsgFifoBuf[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

static UINT32 AmbaDSP_CreateCommonBuf(AMBA_DSP_PROT_BUF_t *pDspProtBuf)
{
    UINT32 Rval;
    ULONG ULAddr;
    UINT32 ReqSize;
    UINT32 ErrLine = __LINE__;

    /* Alloc InitData */
    ReqSize = (UINT32)sizeof(AMBA_DSP_INIT_DATA_s);
    Rval = dsp_osal_alloc_prot_buf(&ULAddr, ReqSize, DSP_BUF_ADDR_ALIGN);
    if (Rval != DSP_ERR_NONE) {
        ErrLine = __LINE__;
    } else {
        pDspProtBuf->InitData.Base = ULAddr;
        pDspProtBuf->InitData.Size = ReqSize;
    }

    /* Alloc ChipData */
    if (Rval == DSP_ERR_NONE) {
        ReqSize = (UINT32)sizeof(AMBA_DSP_CHIP_DATA_s);
        Rval = dsp_osal_alloc_prot_buf(&ULAddr, ReqSize, DSP_BUF_ADDR_ALIGN);
        if (Rval != DSP_ERR_NONE) {
            ErrLine = __LINE__;
        } else {
            pDspProtBuf->ChipData.Base = ULAddr;
            pDspProtBuf->ChipData.Size = ReqSize;
        }
    }

    /* Alloc AssertInfo */
    if (Rval == DSP_ERR_NONE) {
        ReqSize = (UINT32)sizeof(AMBA_DSP_ASSERT_INFO_s)*DSP_MAX_NUM_ASSERT_INFO;
        Rval = dsp_osal_alloc_prot_buf(&ULAddr, ReqSize, DSP_BUF_ADDR_ALIGN);
        if (Rval != DSP_ERR_NONE) {
            ErrLine = __LINE__;
        } else {
            pDspProtBuf->AssertInfo.Base = ULAddr;
            pDspProtBuf->AssertInfo.Size = ReqSize;
        }
    }

    /* Alloc DefaultCmds */
    if (Rval == DSP_ERR_NONE) {
        ReqSize = (UINT32)sizeof(AMBA_DSP_DEFAULT_CMDS_s);
        Rval = dsp_osal_alloc_prot_buf(&ULAddr, ReqSize, DSP_BUF_ADDR_ALIGN);
        if (Rval != DSP_ERR_NONE) {
            ErrLine = __LINE__;
        } else {
            pDspProtBuf->DefaultCmds.Base = ULAddr;
            pDspProtBuf->DefaultCmds.Size = ReqSize;
        }
    }

    if (Rval != DSP_ERR_NONE) {
        AmbaLL_LogUInt5("[Err] CreateCommonBuf[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

static UINT32 AmbaDSP_CreateSyncCtrlBuf(AMBA_DSP_PROT_BUF_t *pDspProtBuf)
{
    UINT32 Rval = DSP_ERR_NONE;
    UINT16 i;
    ULONG ULAddr;
    UINT32 ReqSize;
    UINT32 ErrLine = __LINE__;

    /* Alloc SyncCmdBufCtrl */
    for (i = 0U; i < AMBA_DSP_NUM_SYNC_CMD_BUF_CTRL; i++) {
        if (Rval == DSP_ERR_NONE) {
            ReqSize = (UINT32)sizeof(AMBA_DSP_SYNC_CMD_BUF_CTRL_s);
            Rval = dsp_osal_alloc_prot_buf(&ULAddr, ReqSize, DSP_BUF_ADDR_ALIGN);
            if (Rval != DSP_ERR_NONE) {
                ErrLine = __LINE__;
            } else {
                pDspProtBuf->SyncCmdBufCtrl[i].Base = ULAddr;
                pDspProtBuf->SyncCmdBufCtrl[i].Size = ReqSize;
            }
        }
    }

    /* Alloc VProcMsgBufCtrl */
    if (Rval == DSP_ERR_NONE) {
        ReqSize = (UINT32)sizeof(AMBA_DSP_MSG_BUF_CTRL_s);
        Rval = dsp_osal_alloc_prot_buf(&ULAddr, ReqSize, DSP_BUF_ADDR_ALIGN);
        if (Rval != DSP_ERR_NONE) {
            ErrLine = __LINE__;
        } else {
            pDspProtBuf->VProcMsgBufCtrl.Base = ULAddr;
            pDspProtBuf->VProcMsgBufCtrl.Size = ReqSize;
        }
    }

    /* Alloc AsyncEncMsgBufCtrl */
    if (Rval == DSP_ERR_NONE) {
        ReqSize = (UINT32)sizeof(AMBA_DSP_MSG_BUF_CTRL_s);
        Rval = dsp_osal_alloc_prot_buf(&ULAddr, ReqSize, DSP_BUF_ADDR_ALIGN);
        if (Rval != DSP_ERR_NONE) {
            ErrLine = __LINE__;
        } else {
            pDspProtBuf->AsyncEncMsgBufCtrl.Base = ULAddr;
            pDspProtBuf->AsyncEncMsgBufCtrl.Size = ReqSize;
        }
    }

    if (Rval != DSP_ERR_NONE) {
        AmbaLL_LogUInt5("[Err] CreateSyncCtrlBuf[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

static UINT32 AmbaDSP_CreateEncStrmBuf(AMBA_DSP_PROT_BUF_t *pDspProtBuf)
{
    UINT32 Rval;
    ULONG ULAddr;
    UINT32 ReqSize;
    UINT32 ErrLine = __LINE__;

    /* Alloc VidEncDescBuf */
    ReqSize = (UINT32)sizeof(UINT8)*AMBA_DSP_VIDEOENC_DESC_BUF_SIZE;
    Rval = dsp_osal_alloc_prot_buf(&ULAddr, ReqSize, DSP_BUF_ADDR_ALIGN);
    if (Rval != DSP_ERR_NONE) {
        ErrLine = __LINE__;
    } else {
        pDspProtBuf->VidEncDescBuf.Base = ULAddr;
        pDspProtBuf->VidEncDescBuf.Size = ReqSize;
    }

    /* Alloc EncStrmCfg */
    if (Rval == DSP_ERR_NONE) {
        ReqSize = (UINT32)sizeof(DSP_ENC_CFG)*AMBA_DSP_MAX_STREAM_NUM;
        Rval = dsp_osal_alloc_prot_buf(&ULAddr, ReqSize, DSP_BUF_ADDR_ALIGN);
        if (Rval != DSP_ERR_NONE) {
            ErrLine = __LINE__;
        } else {
            pDspProtBuf->EncStrmCfg.Base = ULAddr;
            pDspProtBuf->EncStrmCfg.Size = ReqSize;
        }
    }

    if (Rval != DSP_ERR_NONE) {
        AmbaLL_LogUInt5("[Err] CreateEncStrmBuf[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

static inline void CreateVprocAuxDataExtBuf(AMBA_DSP_PROT_BUF_t *pDspProtBuf, UINT32 *pRval, UINT32 *pErrLine)
{
    UINT16 i;
    ULONG ULAddr;
    UINT32 ReqSize;

    /* Alloc VprocPymdExtMemTbl */
    for (i = 0U; i < AMBA_DSP_MAX_VIEWZONE_NUM; i++) {
        if (*pRval == DSP_ERR_NONE) {
            ReqSize = (UINT32)sizeof(UINT32)*MAX_EXT_DISTINCT_DEPTH;
            *pRval = dsp_osal_alloc_prot_buf(&ULAddr, ReqSize, DSP_BUF_ADDR_ALIGN);
            if (*pRval != DSP_ERR_NONE) {
                *pErrLine = __LINE__;
            } else {
                pDspProtBuf->VprocPymdExtMemTbl[i].Base = ULAddr;
                pDspProtBuf->VprocPymdExtMemTbl[i].Size = ReqSize;
            }
        }
    }

    /* Alloc VprocLndtExtMemTbl */
    for (i = 0U; i < AMBA_DSP_MAX_VIEWZONE_NUM; i++) {
        if (*pRval == DSP_ERR_NONE) {
            ReqSize = (UINT32)sizeof(UINT32)*MAX_EXT_DISTINCT_DEPTH;
            *pRval = dsp_osal_alloc_prot_buf(&ULAddr, ReqSize, DSP_BUF_ADDR_ALIGN);
            if (*pRval != DSP_ERR_NONE) {
                *pErrLine = __LINE__;
            } else {
                pDspProtBuf->VprocLndtExtMemTbl[i].Base = ULAddr;
                pDspProtBuf->VprocLndtExtMemTbl[i].Size = ReqSize;
            }
        }
    }

    /* Alloc VprocIntMainExtMemTbl */
    for (i = 0U; i < AMBA_DSP_MAX_VIEWZONE_NUM; i++) {
        if (*pRval == DSP_ERR_NONE) {
            ReqSize = (UINT32)sizeof(UINT32)*MAX_EXT_DISTINCT_DEPTH;
            *pRval = dsp_osal_alloc_prot_buf(&ULAddr, ReqSize, DSP_BUF_ADDR_ALIGN);
            if (*pRval != DSP_ERR_NONE) {
                *pErrLine = __LINE__;
            } else {
                pDspProtBuf->VprocIntMainExtMemTbl[i].Base = ULAddr;
                pDspProtBuf->VprocIntMainExtMemTbl[i].Size = ReqSize;
            }
        }
    }
}

static UINT32 AmbaDSP_CreateVprocExtBuf(AMBA_DSP_PROT_BUF_t *pDspProtBuf)
{
    UINT32 Rval;
    UINT16 i;
    ULONG ULAddr;
    UINT32 ReqSize;
    UINT32 ErrLine = __LINE__;

    /* Alloc VprocExtMsgQ */
    ReqSize = ((UINT32)sizeof(UINT8)*AMBA_DSP_VPROC_EXT_MSG_NUM)*AMBA_DSP_VPROC_EXT_MSG_UNIT_SIZE;
    Rval = dsp_osal_alloc_prot_buf(&ULAddr, ReqSize, DSP_BUF_ADDR_ALIGN);
    if (Rval != DSP_ERR_NONE) {
        ErrLine = __LINE__;
    } else {
        pDspProtBuf->VprocExtMsgQ.Base = ULAddr;
        pDspProtBuf->VprocExtMsgQ.Size = ReqSize;
    }

    /* Alloc VprocExtMemDesc */
    if (Rval == DSP_ERR_NONE) {
        ReqSize = ((UINT32)sizeof(ext_mem_desc_t)*AMBA_DSP_MAX_VIEWZONE_NUM)*MAX_VPROC_EXT_MEM_TYPE_NUM;
        Rval = dsp_osal_alloc_prot_buf(&ULAddr, ReqSize, DSP_BUF_ADDR_ALIGN);
        if (Rval != DSP_ERR_NONE) {
            ErrLine = __LINE__;
        } else {
            pDspProtBuf->VprocExtMemDesc.Base = ULAddr;
            pDspProtBuf->VprocExtMemDesc.Size = ReqSize;
        }
    }

    CreateVprocAuxDataExtBuf(pDspProtBuf, &Rval, &ErrLine);

    /* Alloc YuvStrmExtMemTbl */
    for (i = 0U; i < AMBA_DSP_MAX_YUVSTRM_NUM; i++) {
        if (Rval == DSP_ERR_NONE) {
            ReqSize = (UINT32)sizeof(UINT32)*MAX_EXT_DISTINCT_DEPTH;
            Rval = dsp_osal_alloc_prot_buf(&ULAddr, ReqSize, DSP_BUF_ADDR_ALIGN);
            if (Rval != DSP_ERR_NONE) {
                ErrLine = __LINE__;
            } else {
                pDspProtBuf->YuvStrmExtMemTbl[i].Base = ULAddr;
                pDspProtBuf->YuvStrmExtMemTbl[i].Size = ReqSize;
            }
        }
    }

    if (Rval != DSP_ERR_NONE) {
        AmbaLL_LogUInt5("[Err] CreateVprocExtBuf[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

static UINT32 AmbaDSP_CreateVinExtBuf(AMBA_DSP_PROT_BUF_t *pDspProtBuf)
{
    UINT32 Rval;
    UINT16 i;
    ULONG ULAddr;
    UINT32 ReqSize;
    UINT32 ErrLine = __LINE__;

    /* Alloc ExtRawBufArray */
    ReqSize = (((UINT32)sizeof(UINT32)*DSP_VIN_MAX_NUM)*MAX_EXT_MEM_BUF_TYPE_NUM)*MAX_EXT_MEM_BUF_NUM;
    Rval = dsp_osal_alloc_prot_buf(&ULAddr, ReqSize, DSP_BUF_ADDR_ALIGN);
    if (Rval != DSP_ERR_NONE) {
        ErrLine = __LINE__;
    } else {
        pDspProtBuf->ExtRawBufArray.Base = ULAddr;
        pDspProtBuf->ExtRawBufArray.Size = ReqSize;
    }

    /* Alloc VinRawExtMemTbl */
    for (i = 0U; i<AMBA_DSP_MAX_VIN_NUM; i++) {
        if (Rval == DSP_ERR_NONE) {
            ReqSize = (UINT32)sizeof(UINT32)*MAX_EXT_DISTINCT_DEPTH;
            Rval = dsp_osal_alloc_prot_buf(&ULAddr, ReqSize, DSP_BUF_ADDR_ALIGN);
            if (Rval != DSP_ERR_NONE) {
                ErrLine = __LINE__;
            } else {
                pDspProtBuf->VinRawExtMemTbl[i].Base = ULAddr;
                pDspProtBuf->VinRawExtMemTbl[i].Size = ReqSize;
            }
        }
    }

    /* Alloc VinAuxExtMemTbl */
    for (i = 0U; i<AMBA_DSP_MAX_VIN_NUM; i++) {
        if (Rval == DSP_ERR_NONE) {
            ReqSize = (UINT32)sizeof(UINT32)*MAX_EXT_DISTINCT_DEPTH;
            Rval = dsp_osal_alloc_prot_buf(&ULAddr, ReqSize, DSP_BUF_ADDR_ALIGN);
            if (Rval != DSP_ERR_NONE) {
                ErrLine = __LINE__;
            } else {
                pDspProtBuf->VinAuxExtMemTbl[i].Base = ULAddr;
                pDspProtBuf->VinAuxExtMemTbl[i].Size = ReqSize;
            }
        }
    }

    /* Alloc VinMainCfg */
    for (i = 0U; i<AMBA_DSP_MAX_VIN_NUM; i++) {
        if (Rval == DSP_ERR_NONE) {
            ReqSize = (UINT32)sizeof(AMBA_DSP_VIN_MAIN_CONFIG_DATA_s);
            Rval = dsp_osal_alloc_prot_buf(&ULAddr, ReqSize, DSP_BUF_ADDR_ALIGN);
            if (Rval != DSP_ERR_NONE) {
                ErrLine = __LINE__;
            } else {
                pDspProtBuf->VinMainCfg[i].Base = ULAddr;
                pDspProtBuf->VinMainCfg[i].Size = ReqSize;
            }
        }
    }

    if (Rval != DSP_ERR_NONE) {
        AmbaLL_LogUInt5("[Err] CreateVinExtBuf[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

static UINT32 AmbaDSP_CreateYuvStrmBuf(AMBA_DSP_PROT_BUF_t *pDspProtBuf)
{
    UINT32 Rval;
    ULONG ULAddr;
    UINT32 ReqSize;
    UINT16 i;
    UINT32 ErrLine = __LINE__;

    /* Alloc PpStrmCtrl */
    ReqSize = (((UINT32)sizeof(vproc_pp_stream_cntl_t)*AMBA_DSP_MAX_YUVSTRM_NUM)*MAX_GROUP_CMD_POOL_NUM)*AMBA_DSP_MAX_YUVSTRM_VIEW_NUM;
    Rval = dsp_osal_alloc_prot_buf(&ULAddr, ReqSize, DSP_BUF_ADDR_ALIGN);
    if (Rval != DSP_ERR_NONE) {
        ErrLine = __LINE__;
    } else {
        pDspProtBuf->PpStrmCtrl.Base = ULAddr;
        pDspProtBuf->PpStrmCtrl.Size = ReqSize;
    }

    /* Alloc PpStrmCopy */
    if (Rval == DSP_ERR_NONE) {
        ReqSize = ((((UINT32)sizeof(vproc_post_r2y_copy_cfg_t)*AMBA_DSP_MAX_YUVSTRM_NUM)*MAX_GROUP_CMD_POOL_NUM)*AMBA_DSP_MAX_YUVSTRM_VIEW_NUM)*DSP_MAX_PP_STRM_COPY_NUMBER;
        Rval = dsp_osal_alloc_prot_buf(&ULAddr, ReqSize, DSP_BUF_ADDR_ALIGN);
        if (Rval != DSP_ERR_NONE) {
            ErrLine = __LINE__;
        } else {
            pDspProtBuf->PpStrmCopy.Base = ULAddr;
            pDspProtBuf->PpStrmCopy.Size = ReqSize;
        }
    }

    /* Alloc PpStrmBlend */
    if (Rval == DSP_ERR_NONE) {
        ReqSize = ((((UINT32)sizeof(vproc_y2y_blending_cfg_t)*AMBA_DSP_MAX_YUVSTRM_NUM)*MAX_GROUP_CMD_POOL_NUM)*AMBA_DSP_MAX_YUVSTRM_VIEW_NUM)*DSP_MAX_PP_STRM_BLEND_NUMBER_CTX;
        Rval = dsp_osal_alloc_prot_buf(&ULAddr, ReqSize, DSP_BUF_ADDR_ALIGN);
        if (Rval != DSP_ERR_NONE) {
            ErrLine = __LINE__;
        } else {
            pDspProtBuf->PpStrmBlend.Base = ULAddr;
            pDspProtBuf->PpStrmBlend.Size = ReqSize;
        }
    }

    /* Alloc DummyBlend */
    if (Rval == DSP_ERR_NONE) {
        ReqSize = (UINT32)sizeof(UINT8)*DSP_DMY_BLD_SIZE;
        Rval = dsp_osal_alloc_prot_buf(&ULAddr, ReqSize, DSP_BUF_ADDR_ALIGN);
        if (Rval != DSP_ERR_NONE) {
            ErrLine = __LINE__;
        } else {
            pDspProtBuf->DummyBlend.Base = ULAddr;
            pDspProtBuf->DummyBlend.Size = ReqSize;
        }
    }

    /* Alloc YuvStrmAuxExtMemTbl */
    for (i = 0U; i<AMBA_DSP_MAX_YUVSTRM_NUM; i++) {
        if (Rval == DSP_ERR_NONE) {
            ReqSize = (UINT32)sizeof(UINT32)*MAX_EXT_DISTINCT_DEPTH;
            Rval = dsp_osal_alloc_prot_buf(&ULAddr, ReqSize, DSP_BUF_ADDR_ALIGN);
            if (Rval != DSP_ERR_NONE) {
                ErrLine = __LINE__;
            } else {
                pDspProtBuf->YuvStrmAuxExtMemTbl[i].Base = ULAddr;
                pDspProtBuf->YuvStrmAuxExtMemTbl[i].Size = ReqSize;
            }
        }
    }

    if (Rval != DSP_ERR_NONE) {
        AmbaLL_LogUInt5("[Err] CreateYuvStrmBuf[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

static UINT32 AmbaDSP_CreateFovInfoBuf(AMBA_DSP_PROT_BUF_t *pDspProtBuf)
{
    UINT32 Rval;
    ULONG ULAddr;
    UINT32 ReqSize;
    UINT32 ErrLine = __LINE__;

    /* Alloc SideBand */
    ReqSize = (((UINT32)sizeof(sideband_info_t)*AMBA_DSP_MAX_VIEWZONE_NUM)*MAX_SIDEBAND_BUF_NUM)*MAX_SIDEBAND_BUF_ARRAY_NUM;
    Rval = dsp_osal_alloc_prot_buf(&ULAddr, ReqSize, DSP_BUF_ADDR_ALIGN);
    if (Rval != DSP_ERR_NONE) {
        ErrLine = __LINE__;
    } else {
        pDspProtBuf->SideBand.Base = ULAddr;
        pDspProtBuf->SideBand.Size = ReqSize;
    }

    /* Alloc FovLayout */
    if (Rval == DSP_ERR_NONE) {
        ReqSize = ((UINT32)sizeof(fov_layout_t)*DSP_VIN_MAX_NUM)*AMBA_DSP_MAX_VIEWZONE_NUM;
        Rval = dsp_osal_alloc_prot_buf(&ULAddr, ReqSize, DSP_BUF_ADDR_ALIGN);
        if (Rval != DSP_ERR_NONE) {
            ErrLine = __LINE__;
        } else {
            pDspProtBuf->FovLayout.Base = ULAddr;
            pDspProtBuf->FovLayout.Size = ReqSize;
        }
    }

    /* Alloc VinFovCfg */
    if (Rval == DSP_ERR_NONE) {
        ReqSize = ((UINT32)sizeof(vin_fov_cfg_t)*DSP_VIN_MAX_NUM)*AMBA_DSP_MAX_VIEWZONE_NUM;
        Rval = dsp_osal_alloc_prot_buf(&ULAddr, ReqSize, DSP_BUF_ADDR_ALIGN);
        if (Rval != DSP_ERR_NONE) {
            ErrLine = __LINE__;
        } else {
            pDspProtBuf->VinFovCfg.Base = ULAddr;
            pDspProtBuf->VinFovCfg.Size = ReqSize;
        }
    }

    if (Rval != DSP_ERR_NONE) {
        AmbaLL_LogUInt5("[Err] CreateFovInfoBuf[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

static UINT32 AmbaDSP_CreateVprocGrpBuf(AMBA_DSP_PROT_BUF_t *pDspProtBuf)
{
    UINT32 Rval;
    ULONG ULAddr;
    UINT32 ReqSize;
    UINT32 ErrLine = __LINE__;
#ifdef SUPPORT_VPROC_OSD_INSERT
    UINT16 i, j;
#endif

    /* Alloc GrpingNum */
    ReqSize = (UINT32)sizeof(UINT8)*NUM_VPROC_MAX_GROUP;
    Rval = dsp_osal_alloc_prot_buf(&ULAddr, ReqSize, DSP_BUF_ADDR_ALIGN);
    if (Rval != DSP_ERR_NONE) {
        ErrLine = __LINE__;
    } else {
        pDspProtBuf->GrpingNum.Base = ULAddr;
        pDspProtBuf->GrpingNum.Size = ReqSize;
    }

    /* Alloc VprocGrpOrder */
    if (Rval == DSP_ERR_NONE) {
        ReqSize = ((UINT32)sizeof(UINT8)*NUM_VPROC_MAX_GROUP)*NUM_VPROC_MAX_CHAN;
        Rval = dsp_osal_alloc_prot_buf(&ULAddr, ReqSize, DSP_BUF_ADDR_ALIGN);
        if (Rval != DSP_ERR_NONE) {
            ErrLine = __LINE__;
        } else {
            pDspProtBuf->VprocGrpOrder.Base = ULAddr;
            pDspProtBuf->VprocGrpOrder.Size = ReqSize;
        }
    }

    /* Alloc VprocOrder */
    if (Rval == DSP_ERR_NONE) {
        ReqSize = ((UINT32)sizeof(UINT8)*NUM_VPROC_MAX_GROUP)*NUM_VPROC_MAX_CHAN;
        Rval = dsp_osal_alloc_prot_buf(&ULAddr, ReqSize, DSP_BUF_ADDR_ALIGN);
        if (Rval != DSP_ERR_NONE) {
            ErrLine = __LINE__;
        } else {
            pDspProtBuf->VprocOrder.Base = ULAddr;
            pDspProtBuf->VprocOrder.Size = ReqSize;
        }
    }

    /* Alloc VprocSliceLayout */
    if (Rval == DSP_ERR_NONE) {
        ReqSize = ((UINT32)sizeof(DSP_SLICE_LAYOUT_s)*AMBA_DSP_MAX_VIEWZONE_NUM)*MAX_TOTAL_SLICE_NUM;
        Rval = dsp_osal_alloc_prot_buf(&ULAddr, ReqSize, DSP_BUF_ADDR_ALIGN);
        if (Rval != DSP_ERR_NONE) {
            ErrLine = __LINE__;
        } else {
            pDspProtBuf->VprocSliceLayout.Base = ULAddr;
            pDspProtBuf->VprocSliceLayout.Size = ReqSize;
        }
    }

#ifdef SUPPORT_VPROC_OSD_INSERT
    /* Alloc VprocOsdInsert */
    for (i = 0U; i < AMBA_DSP_MAX_STREAM_NUM; i++) {
        for (j = 0U; j < DSP_MAX_OSD_INSERT_BUF_NUM; j++) {
            if (Rval == DSP_ERR_NONE) {
                ReqSize = (UINT32)sizeof(osd_insert_buf_t)*DSP_MAX_OSD_INSERT_AREA_NUMBER;
                Rval = dsp_osal_alloc_prot_buf(&ULAddr, ReqSize, DSP_BUF_ADDR_ALIGN);
                if (Rval != DSP_ERR_NONE) {
                    ErrLine = __LINE__;
                } else {
                    pDspProtBuf->VprocOsdInsert[i][j].Base = ULAddr;
                    pDspProtBuf->VprocOsdInsert[i][j].Size = ReqSize;
                }
            }
        }

    }
#endif

    if (Rval != DSP_ERR_NONE) {
        AmbaLL_LogUInt5("[Err] CreateVprocGrpBuf[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

static UINT32 AmbaDSP_CreateCmdQBuf(AMBA_DSP_PROT_BUF_t *pDspProtBuf)
{
    UINT32 Rval;
    ULONG ULAddr;
    UINT32 ReqSize;
    UINT32 ErrLine = __LINE__;

    /* Alloc BatchCmdQ */
    ReqSize = (((UINT32)sizeof(dsp_cmd_t)*AMBA_DSP_MAX_VIEWZONE_NUM)*MAX_BATCH_CMD_POOL_NUM)*MAX_BATCH_CMD_NUM;
    Rval = dsp_osal_alloc_prot_buf(&ULAddr, ReqSize, DSP_BUF_ADDR_ALIGN);
    if (Rval != DSP_ERR_NONE) {
        ErrLine = __LINE__;
    } else {
        pDspProtBuf->BatchCmdQ.Base = ULAddr;
        pDspProtBuf->BatchCmdQ.Size = ReqSize;
    }

    /* Alloc BatchCmdSet */
    if (Rval == DSP_ERR_NONE) {
        ReqSize = ((UINT32)sizeof(vin_fov_batch_cmd_set_t)*AMBA_DSP_MAX_VIEWZONE_NUM)*MAX_BATCH_CMD_SET_DEPTH;
        Rval = dsp_osal_alloc_prot_buf(&ULAddr, ReqSize, DSP_BUF_ADDR_ALIGN);
        if (Rval != DSP_ERR_NONE) {
            ErrLine = __LINE__;
        } else {
            pDspProtBuf->BatchCmdSet.Base = ULAddr;
            pDspProtBuf->BatchCmdSet.Size = ReqSize;
        }
    }

    /* Alloc GroupCmdQ */
    if (Rval == DSP_ERR_NONE) {
        ReqSize = ((((UINT32)sizeof(dsp_cmd_t)*MAX_GROUP_STRM_NUM)*MAX_GROUP_CMD_POOL_NUM)*MAX_GROUP_CMD_SUBJOB_NUM)*MAX_GROUP_CMD_NUM;
        Rval = dsp_osal_alloc_prot_buf(&ULAddr, ReqSize, DSP_BUF_ADDR_ALIGN);
        if (Rval != DSP_ERR_NONE) {
            ErrLine = __LINE__;
        } else {
            pDspProtBuf->GroupCmdQ.Base = ULAddr;
            pDspProtBuf->GroupCmdQ.Size = ReqSize;
        }
    }

    /* Alloc ViewZoneGroupCmdQ */
    if (Rval == DSP_ERR_NONE) {
        ReqSize = (((UINT32)sizeof(dsp_cmd_t)*MAX_VIEWZONE_GROUP_NUM)*MAX_GROUP_CMD_POOL_NUM)*MAX_VIEWZONE_GROUP_CMD_NUM;
        Rval = dsp_osal_alloc_prot_buf(&ULAddr, ReqSize, DSP_BUF_ADDR_ALIGN);
        if (Rval != DSP_ERR_NONE) {
            ErrLine = __LINE__;
        } else {
            pDspProtBuf->ViewZoneGroupCmdQ.Base = ULAddr;
            pDspProtBuf->ViewZoneGroupCmdQ.Size = ReqSize;
        }
    }

    if (Rval != DSP_ERR_NONE) {
        AmbaLL_LogUInt5("[Err] CreateCmdQBuf[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

static UINT32 AmbaDSP_CreateStillBuf(AMBA_DSP_PROT_BUF_t *pDspProtBuf)
{
    UINT32 Rval = DSP_ERR_NONE;
    UINT16 i;
    ULONG ULAddr;
    UINT32 ReqSize;
    UINT32 ErrLine = __LINE__;

    /* Alloc StlExtMemTbl */
    for (i = 0U; i < MAX_STL_EXT_MEM_STAGE_NUM; i++) {
        if (Rval == DSP_ERR_NONE) {
            ReqSize = (UINT32)sizeof(UINT32);
            Rval = dsp_osal_alloc_prot_buf(&ULAddr, ReqSize, DSP_BUF_ADDR_ALIGN);
            if (Rval != DSP_ERR_NONE) {
                ErrLine = __LINE__;
            } else {
                pDspProtBuf->StlExtMemTbl[i].Base = ULAddr;
                pDspProtBuf->StlExtMemTbl[i].Size = ReqSize;
            }
        }
    }

    /* Alloc StlBatchCmdQ */
    for (i = 0U; i < MAX_STL_EXT_MEM_STAGE_NUM; i++) {
        if (Rval == DSP_ERR_NONE) {
            ReqSize = (UINT32)sizeof(dsp_cmd_t)*MAX_STL_BATCH_CMD_POOL_NUM;
            Rval = dsp_osal_alloc_prot_buf(&ULAddr, ReqSize, DSP_BUF_ADDR_ALIGN);
            if (Rval != DSP_ERR_NONE) {
                ErrLine = __LINE__;
            } else {
                pDspProtBuf->StlBatchCmdQ[i].Base = ULAddr;
                pDspProtBuf->StlBatchCmdQ[i].Size = ReqSize;
            }
        }
    }

    /* Alloc DataCapMemTbl */
    for (i = 0U; i < AMBA_DSP_MAX_DATACAP_NUM; i++) {
        if (Rval == DSP_ERR_NONE) {
            ReqSize = (UINT32)sizeof(UINT32)*MAX_EXT_DISTINCT_DEPTH;
            Rval = dsp_osal_alloc_prot_buf(&ULAddr, ReqSize, DSP_BUF_ADDR_ALIGN);
            if (Rval != DSP_ERR_NONE) {
                ErrLine = __LINE__;
            } else {
                pDspProtBuf->DataCapMemTbl[i].Base = ULAddr;
                pDspProtBuf->DataCapMemTbl[i].Size = ReqSize;
            }
        }
    }

    /* Alloc DataCapAuxMemTbl */
    for (i = 0U; i < AMBA_DSP_MAX_DATACAP_NUM; i++) {
        if (Rval == DSP_ERR_NONE) {
            ReqSize = (UINT32)sizeof(UINT32)*MAX_EXT_DISTINCT_DEPTH;
            Rval = dsp_osal_alloc_prot_buf(&ULAddr, ReqSize, DSP_BUF_ADDR_ALIGN);
            if (Rval != DSP_ERR_NONE) {
                ErrLine = __LINE__;
            } else {
                pDspProtBuf->DataCapAuxMemTbl[i].Base = ULAddr;
                pDspProtBuf->DataCapAuxMemTbl[i].Size = ReqSize;
            }
        }
    }

    if (Rval != DSP_ERR_NONE) {
        AmbaLL_LogUInt5("[Err] CreateStillBuf[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

static UINT32 AmbaDSP_CreateVoutCfgBuf(AMBA_DSP_PROT_BUF_t *pDspProtBuf)
{
    UINT32 Rval = DSP_ERR_NONE;
#if defined (CONFIG_SOC_CV28)
    UINT16 i = 0U;
#else
    UINT16 i;
#endif
    ULONG ULAddr;
    UINT32 ReqSize;
    UINT32 ErrLine = __LINE__;

    /* Alloc VoutDispCfg */
#if !defined (CONFIG_SOC_CV28)
    for (i = 0U; i<AMBA_DSP_MAX_VOUT_NUM; i++)
    {
        if (Rval == DSP_ERR_NONE)
#endif
        {
            ReqSize = (UINT32)sizeof(UINT8)*AMBA_DSP_VOUT_DISP_CFG_LEN;
            Rval = dsp_osal_alloc_prot_buf(&ULAddr, ReqSize, DSP_BUF_ADDR_ALIGN);
            if (Rval != DSP_ERR_NONE) {
                ErrLine = __LINE__;
            } else {
                pDspProtBuf->VoutDispCfg[i].Base = ULAddr;
                pDspProtBuf->VoutDispCfg[i].Size = ReqSize;
            }
        }
#if !defined (CONFIG_SOC_CV28)
    }
#endif

    /* Alloc VoutMixerCscCfg */
#if !defined (CONFIG_SOC_CV28)
    for (i = 0U; i<AMBA_DSP_MAX_VOUT_NUM; i++)
#endif
    {
        if (Rval == DSP_ERR_NONE) {
            ReqSize = (UINT32)sizeof(UINT32)*DSP_VOUT_CSC_MATRIX_DEPTH;
            Rval = dsp_osal_alloc_prot_buf(&ULAddr, ReqSize, DSP_BUF_ADDR_ALIGN);
            if (Rval != DSP_ERR_NONE) {
                ErrLine = __LINE__;
            } else {
                pDspProtBuf->VoutMixerCscCfg[i].Base = ULAddr;
                pDspProtBuf->VoutMixerCscCfg[i].Size = ReqSize;
            }
        }
    }

    /* Alloc VoutTveCfg */
#if !defined (CONFIG_SOC_CV28)
    for (i = 0U; i<AMBA_DSP_MAX_VOUT_NUM; i++)
#endif
    {
        if (Rval == DSP_ERR_NONE) {
            ReqSize = (UINT32)sizeof(UINT8)*DSP_TVE_CFG_SIZE;
            Rval = dsp_osal_alloc_prot_buf(&ULAddr, ReqSize, DSP_BUF_ADDR_ALIGN);
            if (Rval != DSP_ERR_NONE) {
                ErrLine = __LINE__;
            } else {
                pDspProtBuf->VoutTveCfg[i].Base = ULAddr;
                pDspProtBuf->VoutTveCfg[i].Size = ReqSize;
            }
        }
    }

    if (Rval != DSP_ERR_NONE) {
        AmbaLL_LogUInt5("[Err] CreateVoutCfgBuf[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

static UINT32 AmbaDSP_CreateCmdBuf(AMBA_DSP_PROT_BUF_t *pDspProtBuf)
{
    UINT32 Rval;
    UINT16 i;
    ULONG ULAddr;
    UINT32 ReqSize;
    UINT32 ErrLine = __LINE__;

    /* Alloc GeneralCmdBuf */
    ReqSize = (UINT32)(sizeof(dsp_cmd_t)*AMBA_MAX_NUM_DSP_GENERAL_CMD_BUF)*DSP_MAX_NUM_GENERAL_CMD;
    Rval = dsp_osal_alloc_prot_buf(&ULAddr, ReqSize, DSP_BUF_ADDR_ALIGN);
    if (Rval != DSP_ERR_NONE) {
        ErrLine = __LINE__;
    } else {
        pDspProtBuf->GeneralCmdBuf.Base = ULAddr;
        pDspProtBuf->GeneralCmdBuf.Size = ReqSize;
    }

    /* Alloc VinCmdBuf */
    for (i = 0U; i<AMBA_DSP_MAX_VIN_NUM; i++) {
        if (Rval == DSP_ERR_NONE) {
            ReqSize = (UINT32)(sizeof(dsp_cmd_t)*AMBA_MAX_NUM_DSP_VCAP_CMD_BUF)*DSP_MAX_NUM_VCAP_CMD;
            Rval = dsp_osal_alloc_prot_buf(&ULAddr, ReqSize, DSP_BUF_ADDR_ALIGN);
            if (Rval != DSP_ERR_NONE) {
                ErrLine = __LINE__;
            } else {
                pDspProtBuf->VinCmdBuf[i].Base = ULAddr;
                pDspProtBuf->VinCmdBuf[i].Size = ReqSize;
            }
        }
    }

    /* Alloc GroupGeneralCmdBuf */
    if (Rval == DSP_ERR_NONE) {
        ReqSize = (UINT32)sizeof(dsp_cmd_t)*DSP_MAX_NUM_GENERAL_CMD;
        Rval = dsp_osal_alloc_prot_buf(&ULAddr, ReqSize, DSP_BUF_ADDR_ALIGN);
        if (Rval != DSP_ERR_NONE) {
            ErrLine = __LINE__;
        } else {
            pDspProtBuf->GroupGeneralCmdBuf.Base = ULAddr;
            pDspProtBuf->GroupGeneralCmdBuf.Size = ReqSize;
        }
    }

    /* Alloc GroupVinCmdBuf */
    for (i = 0U; i<AMBA_DSP_MAX_VIN_NUM; i++) {
        if (Rval == DSP_ERR_NONE) {
            ReqSize = (UINT32)sizeof(dsp_cmd_t)*DSP_MAX_NUM_VCAP_CMD;
            Rval = dsp_osal_alloc_prot_buf(&ULAddr, ReqSize, DSP_BUF_ADDR_ALIGN);
            if (Rval != DSP_ERR_NONE) {
                ErrLine = __LINE__;
            } else {
                pDspProtBuf->GroupVinCmdBuf[i].Base = ULAddr;
                pDspProtBuf->GroupVinCmdBuf[i].Size = ReqSize;
            }
        }
    }

    if (Rval != DSP_ERR_NONE) {
        AmbaLL_LogUInt5("[Err] CreateCmdBuf[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

static UINT32 AmbaDSP_CreateMsgBuf(AMBA_DSP_PROT_BUF_t *pDspProtBuf)
{
    UINT32 Rval;
    ULONG ULAddr;
    UINT32 ReqSize;
    UINT32 ErrLine = __LINE__;

    /* Alloc GeneralMsgBuf */
    ReqSize = (UINT32)sizeof(dsp_msg_t)*DSP_MAX_NUM_GENERAL_MSG;
    Rval = dsp_osal_alloc_prot_buf(&ULAddr, ReqSize, DSP_BUF_ADDR_ALIGN);
    if (Rval != DSP_ERR_NONE) {
        ErrLine = __LINE__;
    } else {
        pDspProtBuf->GeneralMsgBuf.Base = ULAddr;
        pDspProtBuf->GeneralMsgBuf.Size = ReqSize;
    }

    /* Alloc VCapMsgBuf */
    if (Rval == DSP_ERR_NONE) {
        ReqSize = (UINT32)sizeof(dsp_msg_t)*DSP_MAX_NUM_VCAP_MSG;
        Rval = dsp_osal_alloc_prot_buf(&ULAddr, ReqSize, DSP_BUF_ADDR_ALIGN);
        if (Rval != DSP_ERR_NONE) {
            ErrLine = __LINE__;
        } else {
            pDspProtBuf->VCapMsgBuf.Base = ULAddr;
            pDspProtBuf->VCapMsgBuf.Size = ReqSize;
        }
    }

    /* Alloc VProcMsgBuf */
    if (Rval == DSP_ERR_NONE) {
        ReqSize = (UINT32)sizeof(dsp_msg_t)*DSP_MAX_NUM_VPROC_MSG;
        Rval = dsp_osal_alloc_prot_buf(&ULAddr, ReqSize, DSP_BUF_ADDR_ALIGN);
        if (Rval != DSP_ERR_NONE) {
            ErrLine = __LINE__;
        } else {
            pDspProtBuf->VProcMsgBuf.Base = ULAddr;
            pDspProtBuf->VProcMsgBuf.Size = ReqSize;
        }
    }

    /* Alloc AsyncEncMsgBuf */
#ifdef SUPPORT_DSP_ASYNC_ENCMSG
    if (Rval == DSP_ERR_NONE) {
        ReqSize = (UINT32)sizeof(dsp_msg_t)*DSP_MAX_NUM_ENC_MSG;
        Rval = dsp_osal_alloc_prot_buf(&ULAddr, ReqSize, DSP_BUF_ADDR_ALIGN);
        if (Rval != DSP_ERR_NONE) {
            ErrLine = __LINE__;
        } else {
            pDspProtBuf->AsyncEncMsgBuf.Base = ULAddr;
            pDspProtBuf->AsyncEncMsgBuf.Size = ReqSize;
        }
    }
#endif

    /* Alloc VpMsgBuf */
    if (Rval == DSP_ERR_NONE) {
        ReqSize = (UINT32)sizeof(AMBA_DSP_MSG_VP_HIER_INFO_s)*DSP_MAX_NUM_VP_MSG;
        Rval = dsp_osal_alloc_prot_buf(&ULAddr, ReqSize, DSP_BUF_ADDR_ALIGN);
        if (Rval != DSP_ERR_NONE) {
            ErrLine = __LINE__;
        } else {
            pDspProtBuf->VpMsgBuf.Base = ULAddr;
            pDspProtBuf->VpMsgBuf.Size = ReqSize;
        }
    }

    if (Rval != DSP_ERR_NONE) {
        AmbaLL_LogUInt5("[Err] CreateMsgBuf[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

static UINT32 AmbaDSP_CreateEventBuf(AMBA_DSP_PROT_BUF_t *pDspProtBuf)
{
    UINT32 Rval;
    UINT16 i;
    ULONG ULAddr, StatStartAddr = 0U;
    UINT32 ReqSize;
    UINT32 ErrLine = __LINE__;

    /*******************************************/
    /*               Event Buffers              */
    /*******************************************/

    /* Alloc EventInfo */
    for (i = 0U; i < DSP_EVENT_INFO_POOL_DEPTH; i++) {
        ReqSize = (UINT32)sizeof(AMBA_DSP_EVENT_INFO_s);
        Rval = dsp_osal_alloc_prot_buf(&ULAddr, ReqSize, DSP_BUF_ADDR_ALIGN);
        if (Rval != DSP_ERR_NONE) {
            ErrLine = __LINE__;
        } else {
            pDspProtBuf->EventInfo[i].Base = ULAddr;
            pDspProtBuf->EventInfo[i].Size = ReqSize;
            if (i == 0U) {
                StatStartAddr = ULAddr;
            }
        }
    }

    if (Rval != DSP_ERR_NONE) {
        AmbaLL_LogUInt5("[Err] CreateEventBuf[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    } else {
        pDspProtBuf->EventBuf.Base = StatStartAddr;
        pDspProtBuf->EventBuf.Size = (UINT32)((ULAddr + (ULONG)ReqSize) - StatStartAddr);
        pDspProtBuf->EventBuf.IsCached = 0U;
    }

    return Rval;
}

UINT32 AmbaDSP_ProtBufCreate(AMBA_DSP_PROT_BUF_t *pDspProtBuf)
{
    UINT32 Rval;

    /* EventInfo Buffers */
    Rval = AmbaDSP_CreateEventBuf(pDspProtBuf);

    if (Rval == DSP_ERR_NONE) {
        /* Physical Msg Buffers */
        Rval = AmbaDSP_CreateMsgFifoBuf(pDspProtBuf);
    }
    if (Rval == DSP_ERR_NONE) {
        /* Common Buffers */
        Rval = AmbaDSP_CreateCommonBuf(pDspProtBuf);
    }
    if (Rval == DSP_ERR_NONE) {
        /* Sync Command Buffer Control */
        Rval = AmbaDSP_CreateSyncCtrlBuf(pDspProtBuf);
    }
    if (Rval == DSP_ERR_NONE) {
        /* Video Encode Buffers */
        Rval = AmbaDSP_CreateEncStrmBuf(pDspProtBuf);
    }
    if (Rval == DSP_ERR_NONE) {
        /* Vproc External Buffers */
        Rval = AmbaDSP_CreateVprocExtBuf(pDspProtBuf);
    }
    if (Rval == DSP_ERR_NONE) {
        /* Vin External Memery and Config Buffers */
        Rval = AmbaDSP_CreateVinExtBuf(pDspProtBuf);
    }
    if (Rval == DSP_ERR_NONE) {
        /* Yuv Stream Buffers */
        Rval = AmbaDSP_CreateYuvStrmBuf(pDspProtBuf);
    }
    if (Rval == DSP_ERR_NONE) {
        /* Fov Info Buffers */
        Rval = AmbaDSP_CreateFovInfoBuf(pDspProtBuf);
    }
    if (Rval == DSP_ERR_NONE) {
        /* Vproc Group Buffers */
        Rval = AmbaDSP_CreateVprocGrpBuf(pDspProtBuf);
    }
    if (Rval == DSP_ERR_NONE) {
        /* Command Queue Buffers */
        Rval = AmbaDSP_CreateCmdQBuf(pDspProtBuf);
    }
    if (Rval == DSP_ERR_NONE) {
        /* Still Buffers */
        Rval = AmbaDSP_CreateStillBuf(pDspProtBuf);
    }
    if (Rval == DSP_ERR_NONE) {
        /* Vout Config Buffers */
        Rval = AmbaDSP_CreateVoutCfgBuf(pDspProtBuf);
    }
    if (Rval == DSP_ERR_NONE) {
        /* Chip Depend Command Buffers */
        Rval = AmbaDSP_CreateCmdBuf(pDspProtBuf);
    }
    if (Rval == DSP_ERR_NONE) {
        /* Chip Depend Message Buffers*/
        Rval = AmbaDSP_CreateMsgBuf(pDspProtBuf);
    }

    return Rval;
}

static UINT32 AmbaDSP_GetSubStatBuf01(const AMBA_DSP_STAT_BUF_t *pDspStatBuf, UINT32 Id, UINT32 AuxId, DSP_MEM_BLK_t *pBuf)
{
    UINT32 Rval = DSP_ERR_NONE;

    if (pBuf == NULL) {
        Rval = DSP_ERR_0000;
    } else if (pDspStatBuf == NULL) {
        Rval = DSP_ERR_0000;
    } else {
        switch (Id) {
        case DSP_STAT_BUF_ALL:
        {
            pBuf->Base = pDspStatBuf->StatBuf.Base;
            pBuf->Size = pDspStatBuf->StatBuf.Size;
            pBuf->IsCached = pDspStatBuf->StatBuf.IsCached;
        }
            break;
        case DSP_STAT_BUF_CFA:
        {
            if (AuxId < DSP_3A_BUFFER) {
                pBuf->Base = pDspStatBuf->CfaStat[AuxId].Base;
                pBuf->Size = pDspStatBuf->CfaStat[AuxId].Size;
            } else {
                Rval = DSP_ERR_0001;
            }
        }
            break;
        case DSP_STAT_BUF_PG:
        {
            if (AuxId < DSP_3A_BUFFER) {
                pBuf->Base = pDspStatBuf->PgStat[AuxId].Base;
                pBuf->Size = pDspStatBuf->PgStat[AuxId].Size;
            } else {
                Rval = DSP_ERR_0001;
            }
        }
            break;
        case DSP_STAT_BUF_HIST0:
        {
            if (AuxId < DSP_3A_BUFFER) {
                pBuf->Base = pDspStatBuf->Hist0Stat[AuxId].Base;
                pBuf->Size = pDspStatBuf->Hist0Stat[AuxId].Size;
            } else {
                Rval = DSP_ERR_0001;
            }
        }
            break;
        case DSP_STAT_BUF_HIST1:
        {
            if (AuxId < DSP_3A_BUFFER) {
                pBuf->Base = pDspStatBuf->Hist1Stat[AuxId].Base;
                pBuf->Size = pDspStatBuf->Hist1Stat[AuxId].Size;
            } else {
                Rval = DSP_ERR_0001;
            }
        }
            break;
        case DSP_STAT_BUF_HIST2:
        {
            if (AuxId < DSP_3A_BUFFER) {
                pBuf->Base = pDspStatBuf->Hist2Stat[AuxId].Base;
                pBuf->Size = pDspStatBuf->Hist2Stat[AuxId].Size;
            } else {
                Rval = DSP_ERR_0001;
            }
        }
            break;
        default:
            AmbaLL_LogUInt5("[Err@GetSubStatBuf01] Invalid StatBufId[%d]", Id, 0U, 0U, 0U, 0U);
            Rval = DSP_ERR_0001;
            break;
        }
    }
    return Rval;
}

static UINT32 AmbaDSP_GetSubStatBuf02(const AMBA_DSP_STAT_BUF_t *pDspStatBuf, UINT32 Id, UINT32 AuxId, DSP_MEM_BLK_t *pBuf)
{
    UINT32 Rval = DSP_ERR_NONE;

    if (pBuf == NULL) {
        Rval = DSP_ERR_0000;
    } else if (pDspStatBuf == NULL) {
        Rval = DSP_ERR_0000;
    } else {
        switch (Id) {
        case DSP_STAT_BUF_IDSP:
        {
            if (AuxId < DSP_IDSP_CFG_NUM) {
                pBuf->Base = pDspStatBuf->IdspCfg[AuxId].Base;
                pBuf->Size = pDspStatBuf->IdspCfg[AuxId].Size;
            } else {
                Rval = DSP_ERR_0001;
            }
        }
            break;
        default:
            AmbaLL_LogUInt5("[Err@GetSubStatBuf02] Invalid StatBufId[%d]", Id, 0U, 0U, 0U, 0U);
            Rval = DSP_ERR_0001;
            break;
        }
    }
    return Rval;
}

UINT32 AmbaDSP_GetStatBuf(const AMBA_DSP_STAT_BUF_t *pDspStatBuf, UINT32 Id, UINT32 AuxId, DSP_MEM_BLK_t *pBuf)
{
    UINT32 Rval;

    if (pBuf == NULL) {
        Rval = DSP_ERR_0000;
    } else if (pDspStatBuf == NULL) {
        Rval = DSP_ERR_0000;
    } else {
        if (Id < DSP_STAT_BUF_IDSP) {
            Rval = AmbaDSP_GetSubStatBuf01(pDspStatBuf, Id, AuxId, pBuf);
        } else {
            Rval = AmbaDSP_GetSubStatBuf02(pDspStatBuf, Id, AuxId, pBuf);
        }
    }
    return Rval;
}

static UINT32 AmbaDSP_GetSubProtBuf01(const AMBA_DSP_PROT_BUF_t *pDspProtBuf, UINT32 Id, UINT32 AuxId, DSP_MEM_BLK_t *pBuf)
{
    UINT32 Rval = DSP_ERR_NONE;

    if (pBuf == NULL) {
        Rval = DSP_ERR_0000;
    } else if (pDspProtBuf == NULL) {
        Rval = DSP_ERR_0000;
    } else {
        switch (Id) {
        case DSP_PROT_BUF_INIT_DATA:
        {
            pBuf->Base = pDspProtBuf->InitData.Base;
            pBuf->Size = pDspProtBuf->InitData.Size;
        }
            break;
        case DSP_PROT_BUF_CHIP_DATA:
        {
            pBuf->Base = pDspProtBuf->ChipData.Base;
            pBuf->Size = pDspProtBuf->ChipData.Size;
        }
            break;
        case DSP_PROT_BUF_ASSERT_INFO:
        {
            pBuf->Base = pDspProtBuf->AssertInfo.Base;
            pBuf->Size = pDspProtBuf->AssertInfo.Size;
        }
            break;
        case DSP_PROT_BUF_DEF_CMD:
        {
            pBuf->Base = pDspProtBuf->DefaultCmds.Base;
            pBuf->Size = pDspProtBuf->DefaultCmds.Size;
        }
            break;
        case DSP_PROT_BUF_SYNC_CMD_CTRL:
        {
            if (AuxId < AMBA_DSP_NUM_SYNC_CMD_BUF_CTRL) {
                pBuf->Base = pDspProtBuf->SyncCmdBufCtrl[AuxId].Base;
                pBuf->Size = pDspProtBuf->SyncCmdBufCtrl[AuxId].Size;
            }
        }
            break;
        case DSP_PROT_BUF_VPROC_MSG_BUF_CTRL:
        {
            pBuf->Base = pDspProtBuf->VProcMsgBufCtrl.Base;
            pBuf->Size = pDspProtBuf->VProcMsgBufCtrl.Size;
        }
            break;
        case DSP_PROT_BUF_AYNC_ENC_MSG_BUF_CTRL:
        {
            pBuf->Base = pDspProtBuf->AsyncEncMsgBufCtrl.Base;
            pBuf->Size = pDspProtBuf->AsyncEncMsgBufCtrl.Size;
        }
            break;
        case DSP_PROT_BUF_VENC_DESC_BUF:
        {
            pBuf->Base = pDspProtBuf->VidEncDescBuf.Base;
            pBuf->Size = pDspProtBuf->VidEncDescBuf.Size;
        }
            break;
        case DSP_PROT_BUF_ENC_STRM_CFG:
        {
            pBuf->Base = pDspProtBuf->EncStrmCfg.Base;
            pBuf->Size = pDspProtBuf->EncStrmCfg.Size;
        }
            break;
        default:
            AmbaLL_LogUInt5("[Err@GetSubProtBuf01] Invalid ProtBufId[%d]", Id, 0U, 0U, 0U, 0U);
            Rval = DSP_ERR_0001;
            break;
        }
    }
    return Rval;
}

static UINT32 AmbaDSP_GetSubProtBuf02(const AMBA_DSP_PROT_BUF_t *pDspProtBuf, UINT32 Id, UINT32 AuxId, DSP_MEM_BLK_t *pBuf)
{
    UINT32 Rval = DSP_ERR_NONE;

    if (pBuf == NULL) {
        Rval = DSP_ERR_0000;
    } else if (pDspProtBuf == NULL) {
        Rval = DSP_ERR_0000;
    } else {
        switch (Id) {
        case DSP_PROT_BUF_VPROC_EXT_MSG_Q:
        {
            pBuf->Base = pDspProtBuf->VprocExtMsgQ.Base;
            pBuf->Size = pDspProtBuf->VprocExtMsgQ.Size;
        }
            break;
        case DSP_PROT_BUF_VPROC_EXT_MEM_DESC:
        {
            pBuf->Base = pDspProtBuf->VprocExtMemDesc.Base;
            pBuf->Size = pDspProtBuf->VprocExtMemDesc.Size;
        }
            break;
        case DSP_PROT_BUF_VID_EXT_PYMD_TBL:
        {
            if (AuxId < AMBA_DSP_MAX_VIEWZONE_NUM) {
                pBuf->Base = pDspProtBuf->VprocPymdExtMemTbl[AuxId].Base;
                pBuf->Size = pDspProtBuf->VprocPymdExtMemTbl[AuxId].Size;
            }
        }
            break;
        case DSP_PROT_BUF_VID_EXT_LNDT_TBL:
        {
            if (AuxId < AMBA_DSP_MAX_VIEWZONE_NUM) {
                pBuf->Base = pDspProtBuf->VprocLndtExtMemTbl[AuxId].Base;
                pBuf->Size = pDspProtBuf->VprocLndtExtMemTbl[AuxId].Size;
            }
        }
            break;
        case DSP_PROT_BUF_VID_EXT_YUV_STRM_TBL:
        {
            if (AuxId < AMBA_DSP_MAX_YUVSTRM_NUM) {
                pBuf->Base = pDspProtBuf->YuvStrmExtMemTbl[AuxId].Base;
                pBuf->Size = pDspProtBuf->YuvStrmExtMemTbl[AuxId].Size;
            }
        }
            break;
        default:
            AmbaLL_LogUInt5("[Err@GetSubProtBuf02] Invalid ProtBufId[%d]", Id, 0U, 0U, 0U, 0U);
            Rval = DSP_ERR_0001;
            break;
        }
    }
    return Rval;
}

static UINT32 AmbaDSP_GetSubProtBuf03(const AMBA_DSP_PROT_BUF_t *pDspProtBuf, UINT32 Id, UINT32 AuxId, DSP_MEM_BLK_t *pBuf)
{
    UINT32 Rval = DSP_ERR_NONE;

    if (pBuf == NULL) {
        Rval = DSP_ERR_0000;
    } else if (pDspProtBuf == NULL) {
        Rval = DSP_ERR_0000;
    } else {
        switch (Id) {
        case DSP_PROT_BUF_EXT_RAW_BUF_ARRAY:
        {
            pBuf->Base = pDspProtBuf->ExtRawBufArray.Base;
            pBuf->Size = pDspProtBuf->ExtRawBufArray.Size;
        }
            break;
        case DSP_PROT_BUF_VIN_EXT_RAW_TBL:
        {
            if (AuxId < AMBA_DSP_MAX_VIN_NUM) {
                pBuf->Base = pDspProtBuf->VinRawExtMemTbl[AuxId].Base;
                pBuf->Size = pDspProtBuf->VinRawExtMemTbl[AuxId].Size;
            }
        }
            break;
        case DSP_PROT_BUF_VIN_EXT_AUX_TBL:
        {
            if (AuxId < AMBA_DSP_MAX_VIN_NUM) {
                pBuf->Base = pDspProtBuf->VinAuxExtMemTbl[AuxId].Base;
                pBuf->Size = pDspProtBuf->VinAuxExtMemTbl[AuxId].Size;
            }
        }
            break;
        case DSP_PROT_BUF_VIN_MAIN_CFG:
        {
            if (AuxId < AMBA_DSP_MAX_VIN_NUM) {
                pBuf->Base = pDspProtBuf->VinMainCfg[AuxId].Base;
                pBuf->Size = pDspProtBuf->VinMainCfg[AuxId].Size;
            }
        }
            break;
        case DSP_PROT_BUF_PP_STRM_CTRL:
        {
            pBuf->Base = pDspProtBuf->PpStrmCtrl.Base;
            pBuf->Size = pDspProtBuf->PpStrmCtrl.Size;
        }
            break;
        case DSP_PROT_BUF_PP_STRM_COPY:
        {
            pBuf->Base = pDspProtBuf->PpStrmCopy.Base;
            pBuf->Size = pDspProtBuf->PpStrmCopy.Size;
        }
            break;
        case DSP_PROT_BUF_PP_STRM_BLEND:
        {
            pBuf->Base = pDspProtBuf->PpStrmBlend.Base;
            pBuf->Size = pDspProtBuf->PpStrmBlend.Size;
        }
            break;
        default:
            AmbaLL_LogUInt5("[Err@GetSubProtBuf03] Invalid ProtBufId[%d]", Id, 0U, 0U, 0U, 0U);
            Rval = DSP_ERR_0001;
            break;
        }
    }
    return Rval;
}

static UINT32 AmbaDSP_GetSubProtBuf04(const AMBA_DSP_PROT_BUF_t *pDspProtBuf, UINT32 Id, UINT32 AuxId, DSP_MEM_BLK_t *pBuf)
{
    UINT32 Rval = DSP_ERR_NONE;

AmbaMisra_TouchUnused(&AuxId);

    if (pBuf == NULL) {
        Rval = DSP_ERR_0000;
    } else if (pDspProtBuf == NULL) {
        Rval = DSP_ERR_0000;
    } else {
        switch (Id) {
        case DSP_PROT_BUF_SIDEBAND:
        {
            pBuf->Base = pDspProtBuf->SideBand.Base;
            pBuf->Size = pDspProtBuf->SideBand.Size;
        }
            break;
        case DSP_PROT_BUF_FOV_LAYOUT:
        {
            pBuf->Base = pDspProtBuf->FovLayout.Base;
            pBuf->Size = pDspProtBuf->FovLayout.Size;
        }
            break;
        case DSP_PROT_BUF_VIN_FOV_CFG:
        {
            pBuf->Base = pDspProtBuf->VinFovCfg.Base;
            pBuf->Size = pDspProtBuf->VinFovCfg.Size;
        }
            break;
        case DSP_PROT_BUF_GRPING_NUM:
        {
            pBuf->Base = pDspProtBuf->GrpingNum.Base;
            pBuf->Size = pDspProtBuf->GrpingNum.Size;
        }
            break;
        case DSP_PROT_BUF_VPROC_GRP_ORDER:
        {
            pBuf->Base = pDspProtBuf->VprocGrpOrder.Base;
            pBuf->Size = pDspProtBuf->VprocGrpOrder.Size;
        }
            break;
        case DSP_PROT_BUF_VPROC_ORDER:
        {
            pBuf->Base = pDspProtBuf->VprocOrder.Base;
            pBuf->Size = pDspProtBuf->VprocOrder.Size;
        }
            break;
        case DSP_PROT_BUF_VPROC_SLICE:
        {
            pBuf->Base = pDspProtBuf->VprocSliceLayout.Base;
            pBuf->Size = pDspProtBuf->VprocSliceLayout.Size;
        }
            break;
        case DSP_PROT_BUF_BATCH_CMD_Q:
        {
            pBuf->Base = pDspProtBuf->BatchCmdQ.Base;
            pBuf->Size = pDspProtBuf->BatchCmdQ.Size;
        }
            break;
        case DSP_PROT_BUF_BATCH_CMD_SET:
        {
            pBuf->Base = pDspProtBuf->BatchCmdSet.Base;
            pBuf->Size = pDspProtBuf->BatchCmdSet.Size;
        }
            break;
        case DSP_PROT_BUF_GROUP_CMD_Q:
        {
            pBuf->Base = pDspProtBuf->GroupCmdQ.Base;
            pBuf->Size = pDspProtBuf->GroupCmdQ.Size;
        }
            break;
        default:
            AmbaLL_LogUInt5("[Err@GetSubProtBuf04] Invalid ProtBufId[%d]", Id, 0U, 0U, 0U, 0U);
            Rval = DSP_ERR_0001;
            break;
        }
    }
    return Rval;
}

static UINT32 AmbaDSP_GetSubProtBuf05(const AMBA_DSP_PROT_BUF_t *pDspProtBuf, UINT32 Id, UINT32 AuxId, DSP_MEM_BLK_t *pBuf)
{
    UINT32 Rval = DSP_ERR_NONE;

    if (pBuf == NULL) {
        Rval = DSP_ERR_0000;
    } else if (pDspProtBuf == NULL) {
        Rval = DSP_ERR_0000;
    } else {
        switch (Id) {
        case DSP_PROT_BUF_STL_EXT_MEM_TBL:
        {
            if (AuxId < MAX_STL_EXT_MEM_STAGE_NUM) {
                pBuf->Base = pDspProtBuf->StlExtMemTbl[AuxId].Base;
                pBuf->Size = pDspProtBuf->StlExtMemTbl[AuxId].Size;
            }
        }
            break;
        case DSP_PROT_BUF_STL_BATCH_CMD_Q:
        {
            if (AuxId < MAX_STL_EXT_MEM_STAGE_NUM) {
                pBuf->Base = pDspProtBuf->StlBatchCmdQ[AuxId].Base;
                pBuf->Size = pDspProtBuf->StlBatchCmdQ[AuxId].Size;
            }
        }
            break;
        case DSP_PROT_BUF_VOUT_DISP_CFG:
        {
            if (AuxId < AMBA_DSP_MAX_VOUT_NUM) {
                pBuf->Base = pDspProtBuf->VoutDispCfg[AuxId].Base;
                pBuf->Size = pDspProtBuf->VoutDispCfg[AuxId].Size;
            }
        }
            break;
        case DSP_PROT_BUF_VOUT_MIXER_CSC_CFG:
        {
            if (AuxId < AMBA_DSP_MAX_VOUT_NUM) {
                pBuf->Base = pDspProtBuf->VoutMixerCscCfg[AuxId].Base;
                pBuf->Size = pDspProtBuf->VoutMixerCscCfg[AuxId].Size;
            }
        }
            break;
        case DSP_PROT_BUF_VOUT_TVE_CFG:
        {
            if (AuxId < AMBA_DSP_MAX_VOUT_NUM) {
                pBuf->Base = pDspProtBuf->VoutTveCfg[AuxId].Base;
                pBuf->Size = pDspProtBuf->VoutTveCfg[AuxId].Size;
            }
        }
            break;
        default:
            AmbaLL_LogUInt5("[Err@GetSubProtBuf05] Invalid ProtBufId[%d]", Id, 0U, 0U, 0U, 0U);
            Rval = DSP_ERR_0001;
            break;
        }
    }
    return Rval;
}

static UINT32 AmbaDSP_GetSubProtBuf06(const AMBA_DSP_PROT_BUF_t *pDspProtBuf, UINT32 Id, UINT32 AuxId, DSP_MEM_BLK_t *pBuf)
{
    UINT32 Rval = DSP_ERR_NONE;

    if (pBuf == NULL) {
        Rval = DSP_ERR_0000;
    } else if (pDspProtBuf == NULL) {
        Rval = DSP_ERR_0000;
    } else {
        switch (Id) {
        case DSP_PROT_BUF_PP_DUMMY_BLEND:
        {
            pBuf->Base = pDspProtBuf->DummyBlend.Base;
            pBuf->Size = pDspProtBuf->DummyBlend.Size;
        }
            break;
        case DSP_PROT_BUF_VID_EXT_YUV_AUX_TBL:
        {
            if (AuxId < AMBA_DSP_MAX_YUVSTRM_NUM) {
                pBuf->Base = pDspProtBuf->YuvStrmAuxExtMemTbl[AuxId].Base;
                pBuf->Size = pDspProtBuf->YuvStrmAuxExtMemTbl[AuxId].Size;
            }
        }
            break;
        default:
            AmbaLL_LogUInt5("[Err@GetSubProtBuf06] Invalid ProtBufId[%d]", Id, 0U, 0U, 0U, 0U);
            Rval = DSP_ERR_0001;
            break;
        }
    }
    return Rval;
}

static UINT32 AmbaDSP_GetSubProtBuf07(const AMBA_DSP_PROT_BUF_t *pDspProtBuf, UINT32 Id, UINT32 AuxId, DSP_MEM_BLK_t *pBuf)
{
    UINT32 Rval = DSP_ERR_NONE;

    if (pBuf == NULL) {
        Rval = DSP_ERR_0000;
    } else if (pDspProtBuf == NULL) {
        Rval = DSP_ERR_0000;
    } else {
        switch (Id) {
        case DSP_PROT_BUF_DATA_CAP_TBL:
        {
            if (AuxId < AMBA_DSP_MAX_DATACAP_NUM) {
                pBuf->Base = pDspProtBuf->DataCapMemTbl[AuxId].Base;
                pBuf->Size = pDspProtBuf->DataCapMemTbl[AuxId].Size;
            }
        }
            break;
        case DSP_PROT_BUF_DATA_CAP_AUX_TBL:
        {
            if (AuxId < AMBA_DSP_MAX_DATACAP_NUM) {
                pBuf->Base = pDspProtBuf->DataCapAuxMemTbl[AuxId].Base;
                pBuf->Size = pDspProtBuf->DataCapAuxMemTbl[AuxId].Size;
            }
        }
            break;
        case DSP_PROT_BUF_VID_EXT_INT_MAIN_TBL:
        {
            if (AuxId < AMBA_DSP_MAX_VIEWZONE_NUM) {
                pBuf->Base = pDspProtBuf->VprocIntMainExtMemTbl[AuxId].Base;
                pBuf->Size = pDspProtBuf->VprocIntMainExtMemTbl[AuxId].Size;
            }
        }
            break;
        default:
            AmbaLL_LogUInt5("[Err@GetSubProtBuf07] Invalid ProtBufId[%d]", Id, 0U, 0U, 0U, 0U);
            Rval = DSP_ERR_0001;
            break;
        }
    }
    return Rval;
}

static UINT32 AmbaDSP_GetSubProtBuf08(const AMBA_DSP_PROT_BUF_t *pDspProtBuf, UINT32 Id, UINT32 AuxId, DSP_MEM_BLK_t *pBuf)
{
    UINT32 Rval = DSP_ERR_NONE;

    if (pBuf == NULL) {
        Rval = DSP_ERR_0001;
    } else if (pDspProtBuf == NULL) {
        Rval = DSP_ERR_0001;
    } else {
        switch (Id) {
        case DSP_PROT_BUF_EVENT_ALL:
        {
            pBuf->Base = pDspProtBuf->EventBuf.Base;
            pBuf->Size = pDspProtBuf->EventBuf.Size;
            pBuf->IsCached = pDspProtBuf->EventBuf.IsCached;
        }
            break;
        case DSP_PROT_BUF_EVENT:
        {
            if (AuxId < DSP_EVENT_INFO_POOL_DEPTH) {
                pBuf->Base = pDspProtBuf->EventInfo[AuxId].Base;
                pBuf->Size = pDspProtBuf->EventInfo[AuxId].Size;
            }
        }
            break;
        case DSP_PROT_BUF_VIEW_GROUP_CMD_Q:
        {
            pBuf->Base = pDspProtBuf->ViewZoneGroupCmdQ.Base;
            pBuf->Size = pDspProtBuf->ViewZoneGroupCmdQ.Size;
        }
            break;
#ifdef SUPPORT_VPROC_OSD_INSERT
        case DSP_PROT_BUF_VPROC_OSD_INSERT:
        {
            UINT16 StrmId = (UINT16)DSP_GetBit(AuxId, 0U, 16U);
            UINT16 BufId = (UINT16)DSP_GetBit(AuxId, 16U, 16U);
            pBuf->Base = pDspProtBuf->VprocOsdInsert[StrmId][BufId].Base;
            pBuf->Size = pDspProtBuf->VprocOsdInsert[StrmId][BufId].Size;
        }
            break;
#endif
        default:
            AmbaLL_LogUInt5("[Err@GetSubProtBuf09] Invalid ProtBufId[%d]", Id, 0U, 0U, 0U, 0U);
            Rval = DSP_ERR_0001;
            break;
        }
    }

    return Rval;
}

UINT32 AmbaDSP_GetProtBuf(const AMBA_DSP_PROT_BUF_t *pDspProtBuf, UINT32 Id, UINT32 AuxId, DSP_MEM_BLK_t *pBuf)
{
    UINT32 Rval;

    if (pBuf == NULL) {
        Rval = DSP_ERR_0000;
    } else if (pDspProtBuf == NULL) {
        Rval = DSP_ERR_0000;
    } else {
        if (Id < DSP_PROT_BUF_VPROC_EXT_MSG_Q) {
            Rval = AmbaDSP_GetSubProtBuf01(pDspProtBuf, Id, AuxId, pBuf);
        } else if (Id < DSP_PROT_BUF_EXT_RAW_BUF_ARRAY) {
            Rval = AmbaDSP_GetSubProtBuf02(pDspProtBuf, Id, AuxId, pBuf);
        } else if (Id < DSP_PROT_BUF_SIDEBAND) {
            Rval = AmbaDSP_GetSubProtBuf03(pDspProtBuf, Id, AuxId, pBuf);
        } else if (Id < DSP_PROT_BUF_STL_EXT_MEM_TBL) {
            Rval = AmbaDSP_GetSubProtBuf04(pDspProtBuf, Id, AuxId, pBuf);
        } else if (Id < DSP_PROT_BUF_PP_DUMMY_BLEND) {
            Rval = AmbaDSP_GetSubProtBuf05(pDspProtBuf, Id, AuxId, pBuf);
        } else if (Id < DSP_PROT_BUF_DATA_CAP_TBL) {
            Rval = AmbaDSP_GetSubProtBuf06(pDspProtBuf, Id, AuxId, pBuf);
        } else if (Id < DSP_PROT_BUF_EVENT_ALL) {
            Rval = AmbaDSP_GetSubProtBuf07(pDspProtBuf, Id, AuxId, pBuf);
        } else {
            Rval = AmbaDSP_GetSubProtBuf08(pDspProtBuf, Id, AuxId, pBuf);
        }
    }
    return Rval;
}

