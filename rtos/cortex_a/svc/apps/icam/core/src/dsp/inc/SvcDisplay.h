/**
*  @file SvcDisplay.h
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
*  @details svc display functions
*
*/

#ifndef SVC_DISPLAY_H
#define SVC_DISPLAY_H

typedef struct {
    UINT32                          VoutID;
    AMBA_DSP_VOUT_MIXER_CONFIG_s    MixerCfg;
    AMBA_DSP_VOUT_VIDEO_CFG_s       VideoCfg;
    AMBA_DSP_VOUT_OSD_BUF_CONFIG_s  OsdCfg;
    AMBA_DSP_DISPLAY_CONFIG_s       SyncCfg;
    #define SVC_DISP_CVBS_NONE      (0x0U)
    #define SVC_DISP_CVBS_NTSC      (0x1U)
    #define SVC_DISP_CVBS_PAL       (0x2U)
    UINT32                          CvbsMode;
} SVC_DISP_CFG_s;

typedef struct {
    UINT32          *pNumDisp;
    SVC_DISP_CFG_s  *pDispCfg;
} SVC_DISP_INFO_s;

typedef struct {
    UINT32  VoutID;
    UINT32  EnableVideo;
    UINT32  EnableOsd;
} SVC_DISP_CTRL_s;

void SvcDisplay_InfoGet(SVC_DISP_INFO_s *pInfo);

void SvcDisplay_Init(void);
void SvcDisplay_Config(void);
void SvcDisplay_Update(void);
void SvcDisplay_Ctrl(UINT32 NumCtrl, const SVC_DISP_CTRL_s *pCtrlArr);

void SvcDisplay_Dump(void);

UINT32 SvcDisplay_SetDefImg(UINT8 Enable, UINT8 VoutID, const AMBA_DSP_VOUT_DEFAULT_IMG_CONFIG_s *pCfg);

#endif  /* SVC_DISPLAY_H */
