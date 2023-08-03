/**
 *  @file AmbaImgSystem_Platform.h
 *
 *  Copyright (c) [2020] Ambarella International LP
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
 *  This file includes sample code and is only for internal testing and evaluation.  If you
 *  distribute this sample code (whether in source, object, or binary code form), it will be
 *  without any warranty or indemnity protection from Ambarella International LP or its affiliates.
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
 *  @details Constants and Definitions of Amba Image System for Common SOC (CV2FS)
 *
 */

#ifndef AMBA_IMG_SYSTEM_COMSOC_H
#define AMBA_IMG_SYSTEM_COMSOC_H

UINT32 AmbaImgSystem_PlatformReset(UINT32 VinId);
UINT32 AmbaImgSystem_IkContextIdGet(UINT32 ZoneIndex);

UINT32 AmbaImgSystem_StaticBlcWrite(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT32 Mode, const AMBA_IMG_SYSTEM_PIPE_BLC_s *pBlc, UINT32 ZoneIndex);
UINT32 AmbaImgSystem_WbGainWrite(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT32 Mode, const AMBA_IMG_SYSTEM_PIPE_WB_CTX_s *pWbCtx, UINT32 ZoneIndex);

UINT32 AmbaImgSystem_AikExecute(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT32 Mode);
UINT32 AmbaImgSystem_CmdMsgAheadGet(UINT32 VinId, UINT32 *pCmdMsgAhead);
UINT32 AmbaImgSystem_CmdMsgDecRateGet(UINT32 VinId, UINT32 *pCmdMsgDec);
UINT32 AmbaImgSystem_DeferBlackLvlGet(UINT32 VinId, UINT32 *pEnable);
UINT32 AmbaImgSystem_SlowShutterChange(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT32 SsIndex, UINT32 Mode);

UINT32 AmbaImgSystem_R2yDelay(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT32 R2yDelay);

#endif  /* AMBA_IMG_SYSTEM_COMSOC_H */
