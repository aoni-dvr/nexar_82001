/**
*  @file SvcEnc.h
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
*  @details svc encoder control
*
*/

#ifndef SVC_ENC_H
#define SVC_ENC_H

#include "AmbaDSP_VideoEnc.h"
#include "AmbaDSP_Event.h"

#define SVC_ENC_PMT_CODING          (0U)
#define SVC_ENC_PMT_BRATE           (1U)
#define SVC_ENC_PMT_TILE            (2U)
#define SVC_ENC_PMT_SLICE           (3U)
#define SVC_ENC_PMT_ROTATE_FLIP     (4U)
#define SVC_ENC_PMT_BRATECTRL       (5U)
#define SVC_ENC_PMT_NUM             (6U)

/* HEVC tile */
#define SVC_ENC_HEVC_W_LLMT         (1280U)

#if defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
#define SVC_ENC_HEVC_MAX_TILE       (5U)
#define SVC_ENC_HEVC_MIN_TILE       (4U)
#else
#define SVC_ENC_HEVC_MAX_TILE       (3U)
#define SVC_ENC_HEVC_MIN_TILE       (1U)
#endif

typedef struct {
    AMBA_DSP_EVENT_HANDLER_f  pCBStart;
    AMBA_DSP_EVENT_HANDLER_f  pCBStop;
    AMBA_DSP_EVENT_HANDLER_f  pCBDataRdy;
    AMBA_DSP_EVENT_HANDLER_f  pCBMVData;
} SVC_ENC_EVTCB_s;

typedef struct {
    UINT32                            *pNumStrm;
    AMBA_DSP_VIDEO_ENC_STRM_CONFIG_s  *pStrmCfg;
#if defined(CONFIG_ICAM_ENCMV_SUPPORTED)
    AMBA_DSP_VIDEO_ENC_MV_CFG_s       *pStrmMV;
#endif
} SVC_ENC_INFO_s;

/* MV info */
typedef struct {
    UINT16  IntraActivity;
} GNU_MIN_PADDING ENC_BLK_INTRA_s;

typedef struct {
    ENC_BLK_INTRA_s  Rsvd0[4U];
    ENC_BLK_INTRA_s  Intra_0_0;
    ENC_BLK_INTRA_s  Intra_0_1;
    ENC_BLK_INTRA_s  Intra_1_0;
    ENC_BLK_INTRA_s  Intra_1_1;
    ENC_BLK_INTRA_s  Rsvd1[2U];
} GNU_MIN_PADDING ENC_INTRA_s;

typedef struct {
    UINT32  MvX              :14;
    UINT32  Rsvd             :1;
    UINT32  MvY              :12;
    UINT32  Rsvd1            :3;
    UINT32  RefIsLongTerm    :1;
    UINT32  Valid            :1;
} GNU_MIN_PADDING ENC_BLK_MV_s;

typedef struct {
    ENC_BLK_MV_s  MV_0_0;
    ENC_BLK_MV_s  Rsvd0[1U];
    ENC_BLK_MV_s  MV_0_1;
    ENC_BLK_MV_s  Rsvd1[3U];
    ENC_BLK_MV_s  MV_1_0;
    ENC_BLK_MV_s  Rsvd2[1U];
    ENC_BLK_MV_s  MV_1_1;
    ENC_BLK_MV_s  Rsvd3[3U];
} GNU_MIN_PADDING ENC_MV_s;

void SvcEnc_InfoGet(SVC_ENC_INFO_s *pInfo);
void SvcEnc_HookEvent(UINT32 Enable, const SVC_ENC_EVTCB_s *pEvtCB);

void SvcEnc_Config(void);
void SvcEnc_Start(UINT16 NumStrm,
                  const UINT16 *pStreamIdx,
                  const AMBA_DSP_VIDEO_ENC_START_CONFIG_s *pStaArr);
void SvcEnc_Stop(UINT16 NumStrm, const UINT16 *pStreamIdx, const UINT8 *pStopOpt);

void SvcEnc_ParamCtrl(UINT32 ParamType,
                      UINT16 NumStrm,
                      const UINT16 *pStreamIdx,
                      const UINT32 *pNewVal);

void SvcEnc_Dump(void);

#endif  /* SVC_ENC_H */
