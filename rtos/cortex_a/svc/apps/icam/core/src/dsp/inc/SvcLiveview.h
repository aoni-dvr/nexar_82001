/**
*  @file SvcLiveview.h
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
*  @details svc liveview functions
*
*/

#ifndef SVC_LIVEVIEW_H
#define SVC_LIVEVIEW_H

#define SVC_LIV_PURPOSE_MEM         (0x01U)      /* 0x01 << DSP_LV_STRM_PURPOSE_MEMORY_IDX */
#define SVC_LIV_PURPOSE_ENC         (0x02U)      /* 0x01 << DSP_LV_STRM_PURPOSE_ENCODE_IDX */
#define SVC_LIV_PURPOSE_VOUT        (0x04U)      /* 0x01 << DSP_LV_STRM_PURPOSE_VOUT_IDX   */
#define SVC_LIV_PURPOSE_RAWCAP      (0x08U)      /* 0x01 << DSP_LV_STRM_PURPOSE_RAWCAP_IDX */

#define SVC_LIV_MAX_BUF_TBL_ENTRY   (16U)

typedef struct {
    UINT32                       VinID;
    UINT32                       SubChNum;
    AMBA_DSP_VIN_SUB_CHAN_CFG_s  SubChCfg[AMBA_DSP_MAX_VIRT_CHAN_NUM];
    UINT16                       SubChTDFrmNum[AMBA_DSP_MAX_VIRT_CHAN_NUM][AMBA_DSP_MAX_VIN_TD_NUM];
    AMBA_DSP_VIN_SUB_CHAN_CFG_s  EmbChCfg;
} SVC_LIV_VINCAP_s;

typedef struct {
    ULONG                        BufAddr[SVC_LIV_MAX_BUF_TBL_ENTRY];
} SVC_LIV_BUFTBL_s;

typedef AMBA_DSP_LIVEVIEW_CHANNEL_WINDOW_s SVC_LIV_STRM_CHAN_t[AMBA_DSP_MAX_YUVSTRM_VIEW_NUM];

typedef struct {
    UINT16  FovIDArr[AMBA_DSP_MAX_VIEWZONE_NUM];
    UINT8   EnableArr[AMBA_DSP_MAX_VIEWZONE_NUM];
} SVC_LIV_FOV_CTRL_s;

typedef struct {
    UINT32                            *pVinPostBitsSet;
    UINT32                            *pNumVin;
    SVC_LIV_VINCAP_s                  *pVinCapWin;

    AMBA_DSP_RESOURCE_s               *pDspRes;

    UINT32                            *pNumFov;
    AMBA_DSP_LIVEVIEW_VIEWZONE_CFG_s  *pFovCfg;
#if defined(CONFIG_ICAM_PIPE_LOWDLY_SUPPORTED)
    AMBA_DSP_LIVEVIEW_SLICE_CFG_s     *pLowDlyCfg;
#endif
    AMBA_DSP_LIVEVIEW_YUV_BUF_s       *pPyramidBuf;
    AMBA_DSP_LIVEVIEW_YUV_BUF_s       *pPyramidExScaleBuf;
    SVC_LIV_BUFTBL_s                  *pPyramidTbl;
    SVC_LIV_BUFTBL_s                  *pPyramidExScaleTbl;

    AMBA_DSP_LIVEVIEW_YUV_BUF_s       *pMainY12Buf;
    SVC_LIV_BUFTBL_s                  *pMainY12Tbl;

    AMBA_DSP_LIVEVIEW_YUV_BUF_s       *pFeedExtDataBuf;
    SVC_LIV_BUFTBL_s                  *pFeedExtDataTbl;

    UINT32                            *pNumStrm;
    AMBA_DSP_LIVEVIEW_STREAM_CFG_s    *pStrmCfg;
    SVC_LIV_STRM_CHAN_t               *pStrmChan;
} SVC_LIV_INFO_s;

void SvcLiveview_InfoGet(SVC_LIV_INFO_s *pInfo);

void SvcLiveview_Update(void);
void SvcLiveview_UpdateStream(UINT32 StrmIdx);
void SvcLiveview_Config(void);
void SvcLiveview_Ctrl(UINT32 NumFov, const SVC_LIV_FOV_CTRL_s *pFovCtrl);
void SvcLiveview_SetStillVinPostBits(UINT32 VinPostBits);

void SvcLiveview_Dump(AMBA_SHELL_PRINT_f PrintFunc);

#endif  /* SVC_LIVEVIEW_H */
