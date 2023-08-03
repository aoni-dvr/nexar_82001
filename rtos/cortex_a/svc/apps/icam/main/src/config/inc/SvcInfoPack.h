/**
*  @file SvcInfoPack.h
 *
 * Copyright (c) [2020] Ambarella International LP
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
 * This file includes sample code and is only for internal testing and evaluation.  If you
 * distribute this sample code (whether in source, object, or binary code form), it will be
 * without any warranty or indemnity protection from Ambarella International LP or its affiliates.
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
*  @details svc information pack
*
*/

#ifndef SVC_INFOPACK_H
#define SVC_INFOPACK_H

typedef struct {
    AMBA_DSP_LIVEVIEW_YUV_BUF_s  *pPyramidBufArr;
    SVC_LIV_BUFTBL_s             *pPyramidBufTbl;
    AMBA_DSP_LIVEVIEW_YUV_BUF_s  *pPyramidExScaleBufArr;
    SVC_LIV_BUFTBL_s             *pPyramidExScaleBufTbl;
    AMBA_DSP_LIVEVIEW_YUV_BUF_s  *pMainY12BufArr;
    SVC_LIV_BUFTBL_s             *pMainY12BufTbl;
    AMBA_DSP_LIVEVIEW_YUV_BUF_s  *pFeedExtDataBufArr;
    SVC_LIV_BUFTBL_s             *pFeedExtDataBufTbl;
} SVC_INFO_PACK_LIV_FOV_BUF_ARR_s;

void SvcInfoPack_VinSrcInit(SVC_VIN_SRC_INIT_s *pSrcInit);
void SvcInfoPack_VoutSrcInit(SVC_VOUT_SRC_INIT_s *pSrcInit);

void SvcInfoPack_IKFovWin(UINT32 FovId, SVC_IK_FOV_WIN_s *pIKFovWin);

void SvcInfoPack_DspResource(AMBA_DSP_RESOURCE_s *pDspRes);
void SvcInfoPack_LivVinCap(UINT32 *pNumVin, UINT32 *pVinPostBits, SVC_LIV_VINCAP_s *pVinCapArr);
void SvcInfoPack_LivFovCfg(UINT32 *pNumFov,
                           AMBA_DSP_LIVEVIEW_VIEWZONE_CFG_s *pFovCfgArr,
                           SVC_INFO_PACK_LIV_FOV_BUF_ARR_s *pFovBufArr);
#if defined(CONFIG_ICAM_PIPE_LOWDLY_SUPPORTED)
void SvcInfoPack_LivLowDlyCfg(AMBA_DSP_LIVEVIEW_SLICE_CFG_s *pLowDlyArr);
#endif
void SvcInfoPack_LivStrmCfg(UINT32 *pNumStrm,
                            AMBA_DSP_LIVEVIEW_STREAM_CFG_s *pStrmCfgArr,
                            SVC_LIV_STRM_CHAN_t *pStrmChanArr);

void SvcInfoPack_DispConfig(const SVC_DISP_INFO_s *pInfo);

void SvcInfoPack_DefRawMemQry(UINT32 *pMemSize);
void SvcInfoPack_DefRawCfg(AMBA_DSP_LIVEVIEW_RESOURCE_s *pLivRes);

UINT32 SvcInfoPack_ExtDataMemSizeQuery(UINT32 *pWorkSize);

#endif  /* SVC_INFOPACK_H */
