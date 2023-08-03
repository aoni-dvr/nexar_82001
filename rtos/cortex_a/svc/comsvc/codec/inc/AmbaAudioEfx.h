/**
 *  @file AmbaAudioEfx.h
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
 *  @details audio effect handler apis
 *
 */



#ifndef SVC_AUDIO_EFX_H
#define SVC_AUDIO_EFX_H

#define AEFX_OK         0x0000U
#define AEFX_ERR_0000   0x0001U
#define AEFX_ERR_0001   0x0002U
#define AEFX_ERR_0002   0x0003U
#define AEFX_ERR_0003   0x0004U

#define AMBA_AENC_EFX_VOLUME_LEVLE  (127U)
#define AMBA_AENC_EFX_MAX_CH        (8U)

typedef struct {
    UINT32 Enable;
    UINT32 SrcCh;
    UINT32 DstCh;
    UINT32 SrcRes;
    UINT32 DstRes;
    UINT32 Shift;
    UINT32 FrameSize;
    INT32  ChAssignTable[AMBA_AENC_EFX_MAX_CH];
} AMBA_AENC_FLOW_EFX_FC_s;

typedef struct {
    UINT32 Enable;
    UINT32 Level;
    UINT32 Ch;
    UINT32 Res;
    UINT32 FrameSize;
} AMBA_AENC_FLOW_EFX_VOLUME_s;

typedef struct {
    UINT32 Enable;
    UINT32 SrcFreq;
    UINT32 DstFreq;
    UINT32 Ch;
    UINT32 Res;
    UINT32 FrameSize;
} AMBA_AENC_FLOW_EFX_DWS_s;

typedef struct {
    UINT32 Enable;
    UINT32 Alpha;
    UINT32 LastInput[8];
    UINT32 LastOutput[8];
    UINT32 Ch;
    UINT32 Res;
    UINT32 FrameSize;
} AMBA_AENC_DCBLOCKER_s;

typedef struct {
    AMBA_AENC_FLOW_EFX_FC_s          Fc;
    AMBA_AENC_DCBLOCKER_s            DcBlocker;
    AMBA_AENC_FLOW_EFX_VOLUME_s      Volume;
    AMBA_AENC_FLOW_EFX_DWS_s         Dws;
} AMBA_AENC_FLOW_AU_EFX_INFO_s;

UINT32 AmbaAudioBufEfx_VolumeProc(const AMBA_AENC_FLOW_EFX_VOLUME_s *pVolume, UINT32 *pSrc);
UINT32 AmbaAudioBufEfx_FcProc(const AMBA_AENC_FLOW_EFX_FC_s *pFc, UINT32 *pSrc, UINT32 *pDst);
UINT32 AmbaAudioBufEfx_DwsProc(const AMBA_AENC_FLOW_EFX_DWS_s *pDws, UINT32 *pSrc);
UINT32 AmbaAudioBufEfx_DcBlockerProc(AMBA_AENC_DCBLOCKER_s *pDcBlocker, UINT32 *pSrc);

#endif /* SVC_AUDIO_EFX_H */
