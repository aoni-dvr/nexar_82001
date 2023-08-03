/*
*  @file AmbaTUNE_USBCtrl_cv2x.h
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
*/



#ifndef AMBA_USB_DATACTRL_CV5X_H
#define AMBA_USB_DATACTRL_CV5X_H

#include "AmbaTypes.h"
#include "AmbaDSP_EventInfo.h"

typedef struct {
    UINT32 (*GetIkCtxId)(UINT8 IsHiso, UINT8 CeNeeded, UINT32 *pIkCtxId);
    UINT32 (*GetCapBuffer)(AMBA_DSP_RAW_BUF_s *RawInfo,  AMBA_DSP_BUF_s *CeInfo, UINT8 IsHiso, UINT8 AlignOutputHeight);
    UINT32 (*StillRawEncodeProc)(AMBA_DSP_YUV_IMG_BUF_s *YUVInfo, AMBA_DSP_ENC_PIC_RDY_s *JpgInfo);
} AmbaItn_USBCtrl_RawEnc_Config_s;

UINT32 USBDataCtrl_Init_RawEncode(const AmbaItn_USBCtrl_RawEnc_Config_s *pRawEncCfg);
INT32 AmbaItn_USBCtrl_Communication (UINT32 Parameter1, UINT32 Parameter2, UINT32 Parameter3, UINT32 Parameter4, UINT32 Parameter5, UINT32* length, UINT32* dir);
INT32 AmbaItn_USBCtrl_Save(UINT8 *ObjectBuffer, UINT32 ObjectOffset, UINT32 ObjectLengthRequested, UINT32 *ObjectActualLength);
INT32 AmbaItn_USBCtrl_Load(const UINT8 *ObjectBuffer, UINT32 ObjectOffset, UINT32 ObjectLength);

#endif
