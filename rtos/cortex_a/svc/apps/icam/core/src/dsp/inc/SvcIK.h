/**
*  @file SvcIK.h
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
*  @details svc image kernel functions
*
*/

#ifndef SVC_IK_H
#define SVC_IK_H

#define SVC_IK_SENSOR_RGB       (0U)    /* sensor RGB type */
#define SVC_IK_SENSOR_RGB_IR    (1U)    /* sensor RGB type */
#define SVC_IK_SENSOR_RCCC      (2U)    /* sensor RGB type */

typedef struct {
    AMBA_SENSOR_CHANNEL_s       Chan;
    AMBA_DSP_WINDOW_s           Cap;
    AMBA_DSP_WINDOW_s           Raw;
    AMBA_IK_WINDOW_DIMENSION_s  Main;
    UINT32                      IsCfaCustom;
    AMBA_IK_WINDOW_DIMENSION_s  Cfa;
    UINT32                      Option;         /* Capture option */
    UINT32                      ConCatNum;      /* Concate number */
} SVC_IK_FOV_WIN_s;

UINT32 SvcIK_Init(AMBA_IK_CONTEXT_SETTING_s *pCtxSetting);
UINT32 SvcIK_InitArch(ULONG WorkBase, UINT32 MaxSize);
UINT32 SvcIK_CtxInit(void);

UINT32 SvcIK_SensorConfig(const AMBA_IK_MODE_CFG_s *pImgMode);
UINT32 SvcIK_FovWinConfig(const AMBA_IK_MODE_CFG_s *pImgMode, const SVC_IK_FOV_WIN_s *pWin);

UINT32 SvcIK_ImgWarpCtrl(const AMBA_IK_MODE_CFG_s *pImgMode, UINT32 Enable);
UINT32 SvcIK_ImgDzoomCtrl(const AMBA_IK_MODE_CFG_s *pImgMode, const AMBA_IK_DZOOM_INFO_s *pDzoom);

UINT32 SvcIK_QueryArchMemSize(UINT32 *pNeededSize);

UINT32 SvcIK_RawCompressConfig(UINT32 VinID, UINT32 RawCompression);

#endif  /* SVC_IK_H */
